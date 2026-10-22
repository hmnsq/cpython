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

정수
_PyLexer_refill(struct tok_state *tok)
{
    만약 (tok->done != E_OK) {
        반환 0;
    }
    정수 rc = _PyTok_ReaderUnderflow(tok);
#만약 defined(Py_DEBUG)
    만약 (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, _PyTok_SourcePointer(&tok->source, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->done);
    }
#endif
    만약 (!rc) {
        tok->cur = tok->inp;
        반환 0;
    }
    tok->line_start = tok->cur;
    _PyTok_Span line = {tok->line_start, tok->inp};
    만약 (_PyTok_SourceFindByte(&tok->source, line, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        반환 0;
    }
    반환 1;
}

/* Back-up one character */
void
_PyLexer_backup(struct tok_state *tok, 정수 c)
{
    만약 (c != EOF) {
        만약 (--tok->cur < tok->buf_offset) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        만약 (_PyTok_SourceByte(&tok->source, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
static 정수
verify_identifier(struct tok_state *tok)
{
    만약 (tok->tok_extra_tokens) {
        반환 1;
    }
    PyObject *s;
    만약 (tok_failed(tok))
        반환 0;
    s = PyUnicode_DecodeUTF8(_PyTok_SourcePointer(&tok->source, tok->start),
                             tok->cur - tok->start, NULL);
    만약 (s == NULL) {
        만약 (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->done = E_DECODE;
        }
        아니면 {
            tok->done = E_ERROR;
        }
        반환 0;
    }
    Py_ssize_t invalid = _PyUnicode_ScanIdentifier(s);
    assert(invalid >= 0);
    assert(PyUnicode_GET_LENGTH(s) > 0);
    만약 (invalid < PyUnicode_GET_LENGTH(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        만약 (invalid + 1 < PyUnicode_GET_LENGTH(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            만약 (s != NULL) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            만약 (s == NULL) {
                tok->done = E_ERROR;
                반환 0;
            }
            error_cursor = tok->start + PyBytes_GET_SIZE(s);
        }
        Py_DECREF(s);
        만약 (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        아니면 {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        반환 0;
    }
    Py_DECREF(s);
    반환 1;
}

정수
_PyLexer_get_normal(struct tok_state *tok, ftstring_state *current, struct token *token)
{
    assert(current == NULL ||
           (current->mode == FTSTRING_MODE_EXPRESSION &&
            current->replacement_depth > 0));
    정수 c;
    정수 blankline, nonascii;

    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
  nextline:
    tok->start = -1;
    tok->start_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    만약 (tok->layout.at_bol) {
        blankline = _PyLexer_BeginLine(tok);
        만약 (blankline < 0) {
            반환 MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) : -1};

    만약 (tok->layout.pending != 0) {
        반환 _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->start = -1;
    /* Skip spaces */
    do {
        c = tok_nextc(tok);
    } 동안 (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->start = tok->cur - 1;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    만약 (c == '#') {

        동안 (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        만약 (current != NULL) {
            _PyTok_Off comment_end = tok->cur;
            만약 (c == '\n' || c == '\r') {
                comment_end--;
            }
            만약 (_PyLexer_record_ftstring_comment(
                    tok, current, tok->start, comment_end) < 0) {
                tok->done = E_NOMEM;
                반환 MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->start;
        만약 (tok->type_comments) {
            const char *prefix = type_comment_prefix;
            동안 (*prefix && comment < tok->cur) {
                만약 (*prefix == ' ') {
                    동안 (comment < tok->cur) {
                        정수 ch = _PyTok_SourceByte(&tok->source, comment);
                        만약 (ch != ' ' && ch != '\t') {
                            break;
                        }
                        comment++;
                    }
                } 아니면 만약 (*prefix == _PyTok_SourceByte(&tok->source, comment)) {
                    comment++;
                } 아니면 {
                    break;
                }

                prefix++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            만약 (!*prefix) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                정수 is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(_PyTok_SourcePointer(&tok->source, comment),
                           "ignore", 6) == 0;
                만약 (is_type_ignore && comment + 6 < tok->cur) {
                    정수 ch = _PyTok_SourceByte(&tok->source, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                정수 type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_start = comment + (is_type_ignore ? 6 : 0);
                p_end = tok->cur;
                정수 start_col_offset = tok->start_loc.byte_col +
                    (정수)(p_start - tok->start);
                /* Consume the newline after a standalone type ignore. */
                만약 (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                _PyLexer_token_setup(tok, token, type, p_start, p_end);
                token->start_loc = (_PyTok_Loc){tok->lineno, start_col_offset};
                token->end_loc = (_PyTok_Loc){tok->lineno,
                                              _PyLexer_ByteColumn(tok)};
                반환 type;
            }
        }
        만약 (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.depth > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        만약 (tok->tok_extra_tokens) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_start = comment;
            p_end = tok->cur;
            tok->layout.comment_newline = blankline;
            반환 MAKE_TOKEN(COMMENT);
        }
    }

    만약 (tok->done == E_INTERACT_STOP) {
        반환 MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    만약 (c == EOF) {
        만약 (tok->level) {
            반환 MAKE_TOKEN(ERRORTOKEN);
        }
        반환 MAKE_TOKEN(tok->done == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    만약 (is_potential_identifier_start(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        정수 saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        동안 (1) {
            만약 (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            아니면 만약 (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            아니면 만약 (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            아니면 만약 (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            아니면 만약 (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            아니면 {
                break;
            }
            c = tok_nextc(tok);
            만약 (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                정수 status = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                만약 (status < 0) {
                    반환 MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                만약 (saw_f || saw_t) {
                    ftstring_kind kind = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    반환 _PyLexer_scan_fstring_start(tok, token, c, kind);
                }
                반환 _PyLexer_scan_string(tok, token, c);
            }
        }
        동안 (is_potential_identifier_char(c)) {
            만약 (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        만약 (nonascii && !verify_identifier(tok)) {
            반환 MAKE_TOKEN(ERRORTOKEN);
        }

        p_start = tok->start;
        p_end = tok->cur;

        반환 MAKE_TOKEN(NAME);
    }

    만약 (c == '\r') {
        c = tok_nextc(tok);
    }

    만약 (c == '\n') {
        정수 type = _PyLexer_Newline(tok, token, blankline);
        만약 (type == 0) {
            goto nextline;
        }
        반환 type;
    }

    /* Period or number starting with period? */
    만약 (c == '.') {
        c = tok_nextc(tok);
        만약 (Py_ISDIGIT(c)) {
            반환 _PyLexer_scan_number(tok, token, c, 1);
        } 아니면 만약 (c == '.') {
            c = tok_nextc(tok);
            만약 (c == '.') {
                p_start = tok->start;
                p_end = tok->cur;
                반환 MAKE_TOKEN(ELLIPSIS);
            }
            아니면 {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        아니면 {
            tok_backup(tok, c);
        }
        p_start = tok->start;
        p_end = tok->cur;
        반환 MAKE_TOKEN(DOT);
    }


    /* Number */
    만약 (Py_ISDIGIT(c)) {
        반환 _PyLexer_scan_number(tok, token, c, 0);
    }

    /* String */
    만약 (c == '\'' || c == '"') {
        반환 _PyLexer_scan_string(tok, token, c);
    }

    /* Line continuation */
    만약 (c == '\\') {
        만약 ((c = _PyLexer_ContinueLine(tok)) == -1) {
            반환 MAKE_TOKEN(ERRORTOKEN);
        }
        goto again; /* Read next line */
    }

    /* Punctuation character */
    정수 is_punctuation = (c == ':' || c == '}' || c == '!');
    만약 (is_punctuation && current != NULL) {
        정수 type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        만약 (type < 0) {
            반환 MAKE_TOKEN(ERRORTOKEN);
        }
        만약 (type != 0) {
            p_start = tok->start;
            p_end = tok->cur;
            반환 MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        정수 c2 = tok_nextc(tok);
        정수 current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        만약 (current_token != OP) {
            정수 c3 = tok_nextc(tok);
            정수 current_token3 = _PyToken_ThreeChars(c, c2, c3);
            만약 (current_token3 != OP) {
                current_token = current_token3;
            }
            아니면 {
                tok_backup(tok, c3);
            }
            p_start = tok->start;
            p_end = tok->cur;
            반환 MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    switch (c) {
    case '(':
    case '[':
    case '{':
        만약 (tok->level >= MAXLEVEL) {
            반환 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (정수)(tok->start - tok->line_start);
        tok->level++;
        break;
    case ')':
    case ']':
    case '}':
        만약 (current != NULL &&
                _PyLexer_FTStringBracketDepth(tok, current) == 0) {
            만약 (c == '}') {
                반환 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    _PyLexer_StringPrefix(current->kind)));
            }
            반환 MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                _PyLexer_StringPrefix(current->kind), c));
        }
        만약 (!tok->tok_extra_tokens && !tok->level) {
            반환 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        만약 (tok->level > 0) {
            tok->level--;
            정수 opening = tok->parenstack[tok->level];
            만약 (!tok->tok_extra_tokens && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                만약 (current != NULL && opening == '{') {
                    정수 bracket_depth =
                        _PyLexer_FTStringBracketDepth(tok, current);
                    만약 (bracket_depth == current->replacement_depth - 1) {
                        반환 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            _PyLexer_StringPrefix(current->kind), c));
                    }
                }
                만약 (tok->parenlinenostack[tok->level] != tok->lineno) {
                    반환 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                아니면 {
                    반환 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        만약 (current != NULL &&
                _PyLexer_close_ftstring_expr(tok, current, c) < 0) {
            반환 MAKE_TOKEN(ERRORTOKEN);
        }
        break;
    default:
        break;
    }

    만약 (!Py_UNICODE_ISPRINTABLE(c)) {
        반환 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    만약 (c == '=' && current != NULL) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_start = tok->start;
    p_end = tok->cur;
    반환 MAKE_TOKEN(_PyToken_OneChar(c));
}


void
_PyTokenizer_Get(struct tok_state *tok, struct token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    정수 result;
    만약 (current == NULL) {
        result = _PyLexer_get_normal(tok, NULL, token);
    }
    아니면 {
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
    만약 (tok_failed(tok) && tok->done != E_INCOMPLETE) {
        result = ERRORTOKEN;
    }
    token->type = result;
}
