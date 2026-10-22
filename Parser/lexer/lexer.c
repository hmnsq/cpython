#include "Python.h"
#include "pycore_token.h"
#include "pycore_unicodeobject.h"
#include "errcode.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"
#include "../tokenizer/reader.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

/* Spaces in this constant are treated as "zero or more spaces or tabs" when
   tokenizing. */
static const char* type_comment_prefix = "# type: ";

số_nguyên
_PyLexer_refill(struct tok_state *tok)
{
    nếu (tok->done != E_OK) {
        trả_về 0;
    }
    số_nguyên rc = _PyTok_ReaderUnderflow(tok);
#nếu defined(Py_DEBUG)
    nếu (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, _PyTok_SourcePointer(&tok->source, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->done);
    }
#endif
    nếu (!rc) {
        tok->cur = tok->inp;
        trả_về 0;
    }
    tok->line_start = tok->cur;
    _PyTok_Span line = {tok->line_start, tok->inp};
    nếu (_PyTok_SourceFindByte(&tok->source, line, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        trả_về 0;
    }
    trả_về 1;
}

/* Back-up one character */
void
_PyLexer_backup(struct tok_state *tok, số_nguyên c)
{
    nếu (c != EOF) {
        nếu (--tok->cur < tok->buf_offset) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        nếu (_PyTok_SourceByte(&tok->source, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
static số_nguyên
verify_identifier(struct tok_state *tok)
{
    nếu (tok->tok_extra_tokens) {
        trả_về 1;
    }
    PyObject *s;
    nếu (tok_failed(tok))
        trả_về 0;
    s = PyUnicode_DecodeUTF8(_PyTok_SourcePointer(&tok->source, tok->start),
                             tok->cur - tok->start, NULL);
    nếu (s == NULL) {
        nếu (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->done = E_DECODE;
        }
        không_thì {
            tok->done = E_ERROR;
        }
        trả_về 0;
    }
    Py_ssize_t invalid = _PyUnicode_ScanIdentifier(s);
    assert(invalid >= 0);
    assert(PyUnicode_GET_LENGTH(s) > 0);
    nếu (invalid < PyUnicode_GET_LENGTH(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        nếu (invalid + 1 < PyUnicode_GET_LENGTH(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            nếu (s != NULL) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            nếu (s == NULL) {
                tok->done = E_ERROR;
                trả_về 0;
            }
            error_cursor = tok->start + PyBytes_GET_SIZE(s);
        }
        Py_DECREF(s);
        nếu (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        không_thì {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        trả_về 0;
    }
    Py_DECREF(s);
    trả_về 1;
}

số_nguyên
_PyLexer_get_normal(struct tok_state *tok, ftstring_state *current, struct token *token)
{
    assert(current == NULL ||
           (current->mode == FTSTRING_MODE_EXPRESSION &&
            current->replacement_depth > 0));
    số_nguyên c;
    số_nguyên blankline, nonascii;

    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
  nextline:
    tok->start = -1;
    tok->start_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    nếu (tok->layout.at_bol) {
        blankline = _PyLexer_BeginLine(tok);
        nếu (blankline < 0) {
            trả_về MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) : -1};

    nếu (tok->layout.pending != 0) {
        trả_về _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->start = -1;
    /* Skip spaces */
    do {
        c = tok_nextc(tok);
    } trong_khi (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->start = tok->cur - 1;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    nếu (c == '#') {

        trong_khi (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        nếu (current != NULL) {
            _PyTok_Off comment_end = tok->cur;
            nếu (c == '\n' || c == '\r') {
                comment_end--;
            }
            nếu (_PyLexer_record_ftstring_comment(
                    tok, current, tok->start, comment_end) < 0) {
                tok->done = E_NOMEM;
                trả_về MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->start;
        nếu (tok->type_comments) {
            const char *prefix = type_comment_prefix;
            trong_khi (*prefix && comment < tok->cur) {
                nếu (*prefix == ' ') {
                    trong_khi (comment < tok->cur) {
                        số_nguyên ch = _PyTok_SourceByte(&tok->source, comment);
                        nếu (ch != ' ' && ch != '\t') {
                            break;
                        }
                        comment++;
                    }
                } không_thì nếu (*prefix == _PyTok_SourceByte(&tok->source, comment)) {
                    comment++;
                } không_thì {
                    break;
                }

                prefix++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            nếu (!*prefix) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                số_nguyên is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(_PyTok_SourcePointer(&tok->source, comment),
                           "ignore", 6) == 0;
                nếu (is_type_ignore && comment + 6 < tok->cur) {
                    số_nguyên ch = _PyTok_SourceByte(&tok->source, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                số_nguyên type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_start = comment + (is_type_ignore ? 6 : 0);
                p_end = tok->cur;
                số_nguyên start_col_offset = tok->start_loc.byte_col +
                    (số_nguyên)(p_start - tok->start);
                /* Consume the newline after a standalone type ignore. */
                nếu (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                _PyLexer_token_setup(tok, token, type, p_start, p_end);
                token->start_loc = (_PyTok_Loc){tok->lineno, start_col_offset};
                token->end_loc = (_PyTok_Loc){tok->lineno,
                                              _PyLexer_ByteColumn(tok)};
                trả_về type;
            }
        }
        nếu (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.depth > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        nếu (tok->tok_extra_tokens) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_start = comment;
            p_end = tok->cur;
            tok->layout.comment_newline = blankline;
            trả_về MAKE_TOKEN(COMMENT);
        }
    }

    nếu (tok->done == E_INTERACT_STOP) {
        trả_về MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    nếu (c == EOF) {
        nếu (tok->level) {
            trả_về MAKE_TOKEN(ERRORTOKEN);
        }
        trả_về MAKE_TOKEN(tok->done == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    nếu (is_potential_identifier_start(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        số_nguyên saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        trong_khi (1) {
            nếu (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            không_thì nếu (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            không_thì nếu (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            không_thì nếu (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            không_thì nếu (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            không_thì {
                break;
            }
            c = tok_nextc(tok);
            nếu (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                số_nguyên status = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                nếu (status < 0) {
                    trả_về MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                nếu (saw_f || saw_t) {
                    ftstring_kind kind = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    trả_về _PyLexer_scan_fstring_start(tok, token, c, kind);
                }
                trả_về _PyLexer_scan_string(tok, token, c);
            }
        }
        trong_khi (is_potential_identifier_char(c)) {
            nếu (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        nếu (nonascii && !verify_identifier(tok)) {
            trả_về MAKE_TOKEN(ERRORTOKEN);
        }

        p_start = tok->start;
        p_end = tok->cur;

        trả_về MAKE_TOKEN(NAME);
    }

    nếu (c == '\r') {
        c = tok_nextc(tok);
    }

    nếu (c == '\n') {
        số_nguyên type = _PyLexer_Newline(tok, token, blankline);
        nếu (type == 0) {
            goto nextline;
        }
        trả_về type;
    }

    /* Period or number starting with period? */
    nếu (c == '.') {
        c = tok_nextc(tok);
        nếu (Py_ISDIGIT(c)) {
            trả_về _PyLexer_scan_number(tok, token, c, 1);
        } không_thì nếu (c == '.') {
            c = tok_nextc(tok);
            nếu (c == '.') {
                p_start = tok->start;
                p_end = tok->cur;
                trả_về MAKE_TOKEN(ELLIPSIS);
            }
            không_thì {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        không_thì {
            tok_backup(tok, c);
        }
        p_start = tok->start;
        p_end = tok->cur;
        trả_về MAKE_TOKEN(DOT);
    }


    /* Number */
    nếu (Py_ISDIGIT(c)) {
        trả_về _PyLexer_scan_number(tok, token, c, 0);
    }

    /* String */
    nếu (c == '\'' || c == '"') {
        trả_về _PyLexer_scan_string(tok, token, c);
    }

    /* Line continuation */
    nếu (c == '\\') {
        nếu ((c = _PyLexer_ContinueLine(tok)) == -1) {
            trả_về MAKE_TOKEN(ERRORTOKEN);
        }
        goto again; /* Read next line */
    }

    /* Punctuation character */
    số_nguyên is_punctuation = (c == ':' || c == '}' || c == '!');
    nếu (is_punctuation && current != NULL) {
        số_nguyên type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        nếu (type < 0) {
            trả_về MAKE_TOKEN(ERRORTOKEN);
        }
        nếu (type != 0) {
            p_start = tok->start;
            p_end = tok->cur;
            trả_về MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        số_nguyên c2 = tok_nextc(tok);
        số_nguyên current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        nếu (current_token != OP) {
            số_nguyên c3 = tok_nextc(tok);
            số_nguyên current_token3 = _PyToken_ThreeChars(c, c2, c3);
            nếu (current_token3 != OP) {
                current_token = current_token3;
            }
            không_thì {
                tok_backup(tok, c3);
            }
            p_start = tok->start;
            p_end = tok->cur;
            trả_về MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    switch (c) {
    case '(':
    case '[':
    case '{':
        nếu (tok->level >= MAXLEVEL) {
            trả_về MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (số_nguyên)(tok->start - tok->line_start);
        tok->level++;
        break;
    case ')':
    case ']':
    case '}':
        nếu (current != NULL &&
                _PyLexer_FTStringBracketDepth(tok, current) == 0) {
            nếu (c == '}') {
                trả_về MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    _PyLexer_StringPrefix(current->kind)));
            }
            trả_về MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                _PyLexer_StringPrefix(current->kind), c));
        }
        nếu (!tok->tok_extra_tokens && !tok->level) {
            trả_về MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        nếu (tok->level > 0) {
            tok->level--;
            số_nguyên opening = tok->parenstack[tok->level];
            nếu (!tok->tok_extra_tokens && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                nếu (current != NULL && opening == '{') {
                    số_nguyên bracket_depth =
                        _PyLexer_FTStringBracketDepth(tok, current);
                    nếu (bracket_depth == current->replacement_depth - 1) {
                        trả_về MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            _PyLexer_StringPrefix(current->kind), c));
                    }
                }
                nếu (tok->parenlinenostack[tok->level] != tok->lineno) {
                    trả_về MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                không_thì {
                    trả_về MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        nếu (current != NULL &&
                _PyLexer_close_ftstring_expr(tok, current, c) < 0) {
            trả_về MAKE_TOKEN(ERRORTOKEN);
        }
        break;
    default:
        break;
    }

    nếu (!Py_UNICODE_ISPRINTABLE(c)) {
        trả_về MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    nếu (c == '=' && current != NULL) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_start = tok->start;
    p_end = tok->cur;
    trả_về MAKE_TOKEN(_PyToken_OneChar(c));
}


void
_PyTokenizer_Get(struct tok_state *tok, struct token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    số_nguyên result;
    nếu (current == NULL) {
        result = _PyLexer_get_normal(tok, NULL, token);
    }
    không_thì {
        switch (current->mode) {
        case FTSTRING_MODE_EXPRESSION:
            result = _PyLexer_get_normal(tok, current, token);
            break;
        case FTSTRING_MODE_MIDDLE:
        case FTSTRING_MODE_FORMAT_SPEC:
            result = _PyLexer_get_ftstring(tok, current, token);
            break;
        default:
            Py_UNREACHABLE();
        }
    }
    nếu (tok_failed(tok) && tok->done != E_INCOMPLETE) {
        result = ERRORTOKEN;
    }
    token->type = result;
}
