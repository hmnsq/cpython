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

整数
_PyLexer_refill(struct tok_state *tok)
{
    如果 (tok->done != E_OK) {
        返回 0;
    }
    整数 rc = _PyTok_ReaderUnderflow(tok);
#如果 defined(Py_DEBUG)
    如果 (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, _PyTok_SourcePointer(&tok->source, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->done);
    }
#endif
    如果 (!rc) {
        tok->cur = tok->inp;
        返回 0;
    }
    tok->line_start = tok->cur;
    _PyTok_Span line = {tok->line_start, tok->inp};
    如果 (_PyTok_SourceFindByte(&tok->source, line, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        返回 0;
    }
    返回 1;
}

/* Back-up one character */
void
_PyLexer_backup(struct tok_state *tok, 整数 c)
{
    如果 (c != EOF) {
        如果 (--tok->cur < tok->buf_offset) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        如果 (_PyTok_SourceByte(&tok->source, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
static 整数
verify_identifier(struct tok_state *tok)
{
    如果 (tok->tok_extra_tokens) {
        返回 1;
    }
    PyObject *s;
    如果 (tok_failed(tok))
        返回 0;
    s = PyUnicode_DecodeUTF8(_PyTok_SourcePointer(&tok->source, tok->start),
                             tok->cur - tok->start, NULL);
    如果 (s == NULL) {
        如果 (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->done = E_DECODE;
        }
        否则 {
            tok->done = E_ERROR;
        }
        返回 0;
    }
    Py_ssize_t invalid = _PyUnicode_ScanIdentifier(s);
    assert(invalid >= 0);
    assert(PyUnicode_GET_LENGTH(s) > 0);
    如果 (invalid < PyUnicode_GET_LENGTH(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        如果 (invalid + 1 < PyUnicode_GET_LENGTH(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            如果 (s != NULL) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            如果 (s == NULL) {
                tok->done = E_ERROR;
                返回 0;
            }
            error_cursor = tok->start + PyBytes_GET_SIZE(s);
        }
        Py_DECREF(s);
        如果 (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        否则 {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        返回 0;
    }
    Py_DECREF(s);
    返回 1;
}

整数
_PyLexer_get_normal(struct tok_state *tok, ftstring_state *current, struct token *token)
{
    assert(current == NULL ||
           (current->mode == FTSTRING_MODE_EXPRESSION &&
            current->replacement_depth > 0));
    整数 c;
    整数 blankline, nonascii;

    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
  nextline:
    tok->start = -1;
    tok->start_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    如果 (tok->layout.at_bol) {
        blankline = _PyLexer_BeginLine(tok);
        如果 (blankline < 0) {
            返回 MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) : -1};

    如果 (tok->layout.pending != 0) {
        返回 _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->start = -1;
    /* Skip spaces */
    do {
        c = tok_nextc(tok);
    } 当 (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->start = tok->cur - 1;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    如果 (c == '#') {

        当 (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        如果 (current != NULL) {
            _PyTok_Off comment_end = tok->cur;
            如果 (c == '\n' || c == '\r') {
                comment_end--;
            }
            如果 (_PyLexer_record_ftstring_comment(
                    tok, current, tok->start, comment_end) < 0) {
                tok->done = E_NOMEM;
                返回 MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->start;
        如果 (tok->type_comments) {
            const char *prefix = type_comment_prefix;
            当 (*prefix && comment < tok->cur) {
                如果 (*prefix == ' ') {
                    当 (comment < tok->cur) {
                        整数 ch = _PyTok_SourceByte(&tok->source, comment);
                        如果 (ch != ' ' && ch != '\t') {
                            break;
                        }
                        comment++;
                    }
                } 否则 如果 (*prefix == _PyTok_SourceByte(&tok->source, comment)) {
                    comment++;
                } 否则 {
                    break;
                }

                prefix++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            如果 (!*prefix) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                整数 is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(_PyTok_SourcePointer(&tok->source, comment),
                           "ignore", 6) == 0;
                如果 (is_type_ignore && comment + 6 < tok->cur) {
                    整数 ch = _PyTok_SourceByte(&tok->source, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                整数 type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_start = comment + (is_type_ignore ? 6 : 0);
                p_end = tok->cur;
                整数 start_col_offset = tok->start_loc.byte_col +
                    (整数)(p_start - tok->start);
                /* Consume the newline after a standalone type ignore. */
                如果 (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                _PyLexer_token_setup(tok, token, type, p_start, p_end);
                token->start_loc = (_PyTok_Loc){tok->lineno, start_col_offset};
                token->end_loc = (_PyTok_Loc){tok->lineno,
                                              _PyLexer_ByteColumn(tok)};
                返回 type;
            }
        }
        如果 (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.depth > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        如果 (tok->tok_extra_tokens) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_start = comment;
            p_end = tok->cur;
            tok->layout.comment_newline = blankline;
            返回 MAKE_TOKEN(COMMENT);
        }
    }

    如果 (tok->done == E_INTERACT_STOP) {
        返回 MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    如果 (c == EOF) {
        如果 (tok->level) {
            返回 MAKE_TOKEN(ERRORTOKEN);
        }
        返回 MAKE_TOKEN(tok->done == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    如果 (is_potential_identifier_start(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        整数 saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        当 (1) {
            如果 (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            否则 如果 (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            否则 如果 (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            否则 如果 (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            否则 如果 (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            否则 {
                break;
            }
            c = tok_nextc(tok);
            如果 (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                整数 status = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                如果 (status < 0) {
                    返回 MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                如果 (saw_f || saw_t) {
                    ftstring_kind kind = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    返回 _PyLexer_scan_fstring_start(tok, token, c, kind);
                }
                返回 _PyLexer_scan_string(tok, token, c);
            }
        }
        当 (is_potential_identifier_char(c)) {
            如果 (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        如果 (nonascii && !verify_identifier(tok)) {
            返回 MAKE_TOKEN(ERRORTOKEN);
        }

        p_start = tok->start;
        p_end = tok->cur;

        返回 MAKE_TOKEN(NAME);
    }

    如果 (c == '\r') {
        c = tok_nextc(tok);
    }

    如果 (c == '\n') {
        整数 type = _PyLexer_Newline(tok, token, blankline);
        如果 (type == 0) {
            goto nextline;
        }
        返回 type;
    }

    /* Period or number starting with period? */
    如果 (c == '.') {
        c = tok_nextc(tok);
        如果 (Py_ISDIGIT(c)) {
            返回 _PyLexer_scan_number(tok, token, c, 1);
        } 否则 如果 (c == '.') {
            c = tok_nextc(tok);
            如果 (c == '.') {
                p_start = tok->start;
                p_end = tok->cur;
                返回 MAKE_TOKEN(ELLIPSIS);
            }
            否则 {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        否则 {
            tok_backup(tok, c);
        }
        p_start = tok->start;
        p_end = tok->cur;
        返回 MAKE_TOKEN(DOT);
    }


    /* Number */
    如果 (Py_ISDIGIT(c)) {
        返回 _PyLexer_scan_number(tok, token, c, 0);
    }

    /* String */
    如果 (c == '\'' || c == '"') {
        返回 _PyLexer_scan_string(tok, token, c);
    }

    /* Line continuation */
    如果 (c == '\\') {
        如果 ((c = _PyLexer_ContinueLine(tok)) == -1) {
            返回 MAKE_TOKEN(ERRORTOKEN);
        }
        goto again; /* Read next line */
    }

    /* Punctuation character */
    整数 is_punctuation = (c == ':' || c == '}' || c == '!');
    如果 (is_punctuation && current != NULL) {
        整数 type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        如果 (type < 0) {
            返回 MAKE_TOKEN(ERRORTOKEN);
        }
        如果 (type != 0) {
            p_start = tok->start;
            p_end = tok->cur;
            返回 MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        整数 c2 = tok_nextc(tok);
        整数 current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        如果 (current_token != OP) {
            整数 c3 = tok_nextc(tok);
            整数 current_token3 = _PyToken_ThreeChars(c, c2, c3);
            如果 (current_token3 != OP) {
                current_token = current_token3;
            }
            否则 {
                tok_backup(tok, c3);
            }
            p_start = tok->start;
            p_end = tok->cur;
            返回 MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    switch (c) {
    case '(':
    case '[':
    case '{':
        如果 (tok->level >= MAXLEVEL) {
            返回 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (整数)(tok->start - tok->line_start);
        tok->level++;
        break;
    case ')':
    case ']':
    case '}':
        如果 (current != NULL &&
                _PyLexer_FTStringBracketDepth(tok, current) == 0) {
            如果 (c == '}') {
                返回 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    _PyLexer_StringPrefix(current->kind)));
            }
            返回 MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                _PyLexer_StringPrefix(current->kind), c));
        }
        如果 (!tok->tok_extra_tokens && !tok->level) {
            返回 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        如果 (tok->level > 0) {
            tok->level--;
            整数 opening = tok->parenstack[tok->level];
            如果 (!tok->tok_extra_tokens && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                如果 (current != NULL && opening == '{') {
                    整数 bracket_depth =
                        _PyLexer_FTStringBracketDepth(tok, current);
                    如果 (bracket_depth == current->replacement_depth - 1) {
                        返回 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            _PyLexer_StringPrefix(current->kind), c));
                    }
                }
                如果 (tok->parenlinenostack[tok->level] != tok->lineno) {
                    返回 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                否则 {
                    返回 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        如果 (current != NULL &&
                _PyLexer_close_ftstring_expr(tok, current, c) < 0) {
            返回 MAKE_TOKEN(ERRORTOKEN);
        }
        break;
    default:
        break;
    }

    如果 (!Py_UNICODE_ISPRINTABLE(c)) {
        返回 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    如果 (c == '=' && current != NULL) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_start = tok->start;
    p_end = tok->cur;
    返回 MAKE_TOKEN(_PyToken_OneChar(c));
}


void
_PyTokenizer_Get(struct tok_state *tok, struct token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    整数 result;
    如果 (current == NULL) {
        result = _PyLexer_get_normal(tok, NULL, token);
    }
    否则 {
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
    如果 (tok_failed(tok) && tok->done != E_INCOMPLETE) {
        result = ERRORTOKEN;
    }
    token->type = result;
}
