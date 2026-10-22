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

ціле
_PyLexer_refill(struct tok_state *tok)
{
    якщо (tok->done != E_OK) {
        повернути 0;
    }
    ціле rc = _PyTok_ReaderUnderflow(tok);
#якщо defined(Py_DEBUG)
    якщо (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, _PyTok_SourcePointer(&tok->source, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->done);
    }
#endif
    якщо (!rc) {
        tok->cur = tok->inp;
        повернути 0;
    }
    tok->line_start = tok->cur;
    _PyTok_Span line = {tok->line_start, tok->inp};
    якщо (_PyTok_SourceFindByte(&tok->source, line, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        повернути 0;
    }
    повернути 1;
}

/* Back-up one character */
void
_PyLexer_backup(struct tok_state *tok, ціле c)
{
    якщо (c != EOF) {
        якщо (--tok->cur < tok->buf_offset) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        якщо (_PyTok_SourceByte(&tok->source, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
static ціле
verify_identifier(struct tok_state *tok)
{
    якщо (tok->tok_extra_tokens) {
        повернути 1;
    }
    PyObject *s;
    якщо (tok_failed(tok))
        повернути 0;
    s = PyUnicode_DecodeUTF8(_PyTok_SourcePointer(&tok->source, tok->start),
                             tok->cur - tok->start, NULL);
    якщо (s == NULL) {
        якщо (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->done = E_DECODE;
        }
        інакше {
            tok->done = E_ERROR;
        }
        повернути 0;
    }
    Py_ssize_t invalid = _PyUnicode_ScanIdentifier(s);
    assert(invalid >= 0);
    assert(PyUnicode_GET_LENGTH(s) > 0);
    якщо (invalid < PyUnicode_GET_LENGTH(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        якщо (invalid + 1 < PyUnicode_GET_LENGTH(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            якщо (s != NULL) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            якщо (s == NULL) {
                tok->done = E_ERROR;
                повернути 0;
            }
            error_cursor = tok->start + PyBytes_GET_SIZE(s);
        }
        Py_DECREF(s);
        якщо (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        інакше {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        повернути 0;
    }
    Py_DECREF(s);
    повернути 1;
}

ціле
_PyLexer_get_normal(struct tok_state *tok, ftstring_state *current, struct token *token)
{
    assert(current == NULL ||
           (current->mode == FTSTRING_MODE_EXPRESSION &&
            current->replacement_depth > 0));
    ціле c;
    ціле blankline, nonascii;

    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
  nextline:
    tok->start = -1;
    tok->start_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    якщо (tok->layout.at_bol) {
        blankline = _PyLexer_BeginLine(tok);
        якщо (blankline < 0) {
            повернути MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) : -1};

    якщо (tok->layout.pending != 0) {
        повернути _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->start = -1;
    /* Skip spaces */
    do {
        c = tok_nextc(tok);
    } поки (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->start = tok->cur - 1;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    якщо (c == '#') {

        поки (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        якщо (current != NULL) {
            _PyTok_Off comment_end = tok->cur;
            якщо (c == '\n' || c == '\r') {
                comment_end--;
            }
            якщо (_PyLexer_record_ftstring_comment(
                    tok, current, tok->start, comment_end) < 0) {
                tok->done = E_NOMEM;
                повернути MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->start;
        якщо (tok->type_comments) {
            const char *prefix = type_comment_prefix;
            поки (*prefix && comment < tok->cur) {
                якщо (*prefix == ' ') {
                    поки (comment < tok->cur) {
                        ціле ch = _PyTok_SourceByte(&tok->source, comment);
                        якщо (ch != ' ' && ch != '\t') {
                            break;
                        }
                        comment++;
                    }
                } інакше якщо (*prefix == _PyTok_SourceByte(&tok->source, comment)) {
                    comment++;
                } інакше {
                    break;
                }

                prefix++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            якщо (!*prefix) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                ціле is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(_PyTok_SourcePointer(&tok->source, comment),
                           "ignore", 6) == 0;
                якщо (is_type_ignore && comment + 6 < tok->cur) {
                    ціле ch = _PyTok_SourceByte(&tok->source, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                ціле type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_start = comment + (is_type_ignore ? 6 : 0);
                p_end = tok->cur;
                ціле start_col_offset = tok->start_loc.byte_col +
                    (ціле)(p_start - tok->start);
                /* Consume the newline after a standalone type ignore. */
                якщо (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                _PyLexer_token_setup(tok, token, type, p_start, p_end);
                token->start_loc = (_PyTok_Loc){tok->lineno, start_col_offset};
                token->end_loc = (_PyTok_Loc){tok->lineno,
                                              _PyLexer_ByteColumn(tok)};
                повернути type;
            }
        }
        якщо (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.depth > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        якщо (tok->tok_extra_tokens) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_start = comment;
            p_end = tok->cur;
            tok->layout.comment_newline = blankline;
            повернути MAKE_TOKEN(COMMENT);
        }
    }

    якщо (tok->done == E_INTERACT_STOP) {
        повернути MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    якщо (c == EOF) {
        якщо (tok->level) {
            повернути MAKE_TOKEN(ERRORTOKEN);
        }
        повернути MAKE_TOKEN(tok->done == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    якщо (is_potential_identifier_start(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        ціле saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        поки (1) {
            якщо (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            інакше якщо (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            інакше якщо (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            інакше якщо (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            інакше якщо (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            інакше {
                break;
            }
            c = tok_nextc(tok);
            якщо (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                ціле status = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                якщо (status < 0) {
                    повернути MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                якщо (saw_f || saw_t) {
                    ftstring_kind kind = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    повернути _PyLexer_scan_fstring_start(tok, token, c, kind);
                }
                повернути _PyLexer_scan_string(tok, token, c);
            }
        }
        поки (is_potential_identifier_char(c)) {
            якщо (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        якщо (nonascii && !verify_identifier(tok)) {
            повернути MAKE_TOKEN(ERRORTOKEN);
        }

        p_start = tok->start;
        p_end = tok->cur;

        повернути MAKE_TOKEN(NAME);
    }

    якщо (c == '\r') {
        c = tok_nextc(tok);
    }

    якщо (c == '\n') {
        ціле type = _PyLexer_Newline(tok, token, blankline);
        якщо (type == 0) {
            goto nextline;
        }
        повернути type;
    }

    /* Period or number starting with period? */
    якщо (c == '.') {
        c = tok_nextc(tok);
        якщо (Py_ISDIGIT(c)) {
            повернути _PyLexer_scan_number(tok, token, c, 1);
        } інакше якщо (c == '.') {
            c = tok_nextc(tok);
            якщо (c == '.') {
                p_start = tok->start;
                p_end = tok->cur;
                повернути MAKE_TOKEN(ELLIPSIS);
            }
            інакше {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        інакше {
            tok_backup(tok, c);
        }
        p_start = tok->start;
        p_end = tok->cur;
        повернути MAKE_TOKEN(DOT);
    }


    /* Number */
    якщо (Py_ISDIGIT(c)) {
        повернути _PyLexer_scan_number(tok, token, c, 0);
    }

    /* String */
    якщо (c == '\'' || c == '"') {
        повернути _PyLexer_scan_string(tok, token, c);
    }

    /* Line continuation */
    якщо (c == '\\') {
        якщо ((c = _PyLexer_ContinueLine(tok)) == -1) {
            повернути MAKE_TOKEN(ERRORTOKEN);
        }
        goto again; /* Read next line */
    }

    /* Punctuation character */
    ціле is_punctuation = (c == ':' || c == '}' || c == '!');
    якщо (is_punctuation && current != NULL) {
        ціле type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        якщо (type < 0) {
            повернути MAKE_TOKEN(ERRORTOKEN);
        }
        якщо (type != 0) {
            p_start = tok->start;
            p_end = tok->cur;
            повернути MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        ціле c2 = tok_nextc(tok);
        ціле current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        якщо (current_token != OP) {
            ціле c3 = tok_nextc(tok);
            ціле current_token3 = _PyToken_ThreeChars(c, c2, c3);
            якщо (current_token3 != OP) {
                current_token = current_token3;
            }
            інакше {
                tok_backup(tok, c3);
            }
            p_start = tok->start;
            p_end = tok->cur;
            повернути MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    switch (c) {
    case '(':
    case '[':
    case '{':
        якщо (tok->level >= MAXLEVEL) {
            повернути MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (ціле)(tok->start - tok->line_start);
        tok->level++;
        break;
    case ')':
    case ']':
    case '}':
        якщо (current != NULL &&
                _PyLexer_FTStringBracketDepth(tok, current) == 0) {
            якщо (c == '}') {
                повернути MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    _PyLexer_StringPrefix(current->kind)));
            }
            повернути MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                _PyLexer_StringPrefix(current->kind), c));
        }
        якщо (!tok->tok_extra_tokens && !tok->level) {
            повернути MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        якщо (tok->level > 0) {
            tok->level--;
            ціле opening = tok->parenstack[tok->level];
            якщо (!tok->tok_extra_tokens && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                якщо (current != NULL && opening == '{') {
                    ціле bracket_depth =
                        _PyLexer_FTStringBracketDepth(tok, current);
                    якщо (bracket_depth == current->replacement_depth - 1) {
                        повернути MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            _PyLexer_StringPrefix(current->kind), c));
                    }
                }
                якщо (tok->parenlinenostack[tok->level] != tok->lineno) {
                    повернути MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                інакше {
                    повернути MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        якщо (current != NULL &&
                _PyLexer_close_ftstring_expr(tok, current, c) < 0) {
            повернути MAKE_TOKEN(ERRORTOKEN);
        }
        break;
    default:
        break;
    }

    якщо (!Py_UNICODE_ISPRINTABLE(c)) {
        повернути MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    якщо (c == '=' && current != NULL) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_start = tok->start;
    p_end = tok->cur;
    повернути MAKE_TOKEN(_PyToken_OneChar(c));
}


void
_PyTokenizer_Get(struct tok_state *tok, struct token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    ціле result;
    якщо (current == NULL) {
        result = _PyLexer_get_normal(tok, NULL, token);
    }
    інакше {
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
    якщо (tok_failed(tok) && tok->done != E_INCOMPLETE) {
        result = ERRORTOKEN;
    }
    token->type = result;
}
