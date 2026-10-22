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
static const var_na* type_comment_prefix = "# type: ";

จำนวนเต็ม
_PyLexer_refill(sa_nracanaa tok_state *tok)
{
    ถ้า (tok->done != E_OK) {
        คืน 0;
    }
    จำนวนเต็ม rc = _PyTok_ReaderUnderflow(tok);
#ถ้า defined(Py_DEBUG)
    ถ้า (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, _PyTok_SourcePointer(&tok->source, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->done);
    }
#endif
    ถ้า (!rc) {
        tok->cur = tok->inp;
        คืน 0;
    }
    tok->line_start = tok->cur;
    _PyTok_Span line = {tok->line_start, tok->inp};
    ถ้า (_PyTok_SourceFindByte(&tok->source, line, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        คืน 0;
    }
    คืน 1;
}

/* Back-up one character */
void
_PyLexer_backup(sa_nracanaa tok_state *tok, จำนวนเต็ม c)
{
    ถ้า (c != EOF) {
        ถ้า (--tok->cur < tok->buf_offset) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        ถ้า (_PyTok_SourceByte(&tok->source, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
static จำนวนเต็ม
verify_identifier(sa_nracanaa tok_state *tok)
{
    ถ้า (tok->tok_extra_tokens) {
        คืน 1;
    }
    PyObject *s;
    ถ้า (tok_failed(tok))
        คืน 0;
    s = PyUnicode_DecodeUTF8(_PyTok_SourcePointer(&tok->source, tok->start),
                             tok->cur - tok->start, NULL);
    ถ้า (s == NULL) {
        ถ้า (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->done = E_DECODE;
        }
        มิฉะนั้น {
            tok->done = E_ERROR;
        }
        คืน 0;
    }
    Py_ssize_t invalid = _PyUnicode_ScanIdentifier(s);
    assert(invalid >= 0);
    assert(PyUnicode_GET_LENGTH(s) > 0);
    ถ้า (invalid < PyUnicode_GET_LENGTH(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        ถ้า (invalid + 1 < PyUnicode_GET_LENGTH(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            ถ้า (s != NULL) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            ถ้า (s == NULL) {
                tok->done = E_ERROR;
                คืน 0;
            }
            error_cursor = tok->start + PyBytes_GET_SIZE(s);
        }
        Py_DECREF(s);
        ถ้า (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        มิฉะนั้น {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        คืน 0;
    }
    Py_DECREF(s);
    คืน 1;
}

จำนวนเต็ม
_PyLexer_get_normal(sa_nracanaa tok_state *tok, ftstring_state *current, sa_nracanaa token *token)
{
    assert(current == NULL ||
           (current->mode == FTSTRING_MODE_EXPRESSION &&
            current->replacement_depth > 0));
    จำนวนเต็ม c;
    จำนวนเต็ม blankline, nonascii;

    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
  nextline:
    tok->start = -1;
    tok->start_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    ถ้า (tok->layout.at_bol) {
        blankline = _PyLexer_BeginLine(tok);
        ถ้า (blankline < 0) {
            คืน MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) : -1};

    ถ้า (tok->layout.pending != 0) {
        คืน _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->start = -1;
    /* Skip spaces */
    do {
        c = tok_nextc(tok);
    } ขณะ (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->start = tok->cur - 1;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    ถ้า (c == '#') {

        ขณะ (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        ถ้า (current != NULL) {
            _PyTok_Off comment_end = tok->cur;
            ถ้า (c == '\n' || c == '\r') {
                comment_end--;
            }
            ถ้า (_PyLexer_record_ftstring_comment(
                    tok, current, tok->start, comment_end) < 0) {
                tok->done = E_NOMEM;
                คืน MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->start;
        ถ้า (tok->type_comments) {
            const var_na *prefix = type_comment_prefix;
            ขณะ (*prefix && comment < tok->cur) {
                ถ้า (*prefix == ' ') {
                    ขณะ (comment < tok->cur) {
                        จำนวนเต็ม ch = _PyTok_SourceByte(&tok->source, comment);
                        ถ้า (ch != ' ' && ch != '\t') {
                            bha_nga;
                        }
                        comment++;
                    }
                } มิฉะนั้น ถ้า (*prefix == _PyTok_SourceByte(&tok->source, comment)) {
                    comment++;
                } มิฉะนั้น {
                    bha_nga;
                }

                prefix++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            ถ้า (!*prefix) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                จำนวนเต็ม is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(_PyTok_SourcePointer(&tok->source, comment),
                           "ignore", 6) == 0;
                ถ้า (is_type_ignore && comment + 6 < tok->cur) {
                    จำนวนเต็ม ch = _PyTok_SourceByte(&tok->source, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                จำนวนเต็ม type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_start = comment + (is_type_ignore ? 6 : 0);
                p_end = tok->cur;
                จำนวนเต็ม start_col_offset = tok->start_loc.byte_col +
                    (จำนวนเต็ม)(p_start - tok->start);
                /* Consume the newline after a standalone type ignore. */
                ถ้า (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                _PyLexer_token_setup(tok, token, type, p_start, p_end);
                token->start_loc = (_PyTok_Loc){tok->lineno, start_col_offset};
                token->end_loc = (_PyTok_Loc){tok->lineno,
                                              _PyLexer_ByteColumn(tok)};
                คืน type;
            }
        }
        ถ้า (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.depth > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        ถ้า (tok->tok_extra_tokens) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_start = comment;
            p_end = tok->cur;
            tok->layout.comment_newline = blankline;
            คืน MAKE_TOKEN(COMMENT);
        }
    }

    ถ้า (tok->done == E_INTERACT_STOP) {
        คืน MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    ถ้า (c == EOF) {
        ถ้า (tok->level) {
            คืน MAKE_TOKEN(ERRORTOKEN);
        }
        คืน MAKE_TOKEN(tok->done == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    ถ้า (is_potential_identifier_start(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        จำนวนเต็ม saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        ขณะ (1) {
            ถ้า (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            มิฉะนั้น ถ้า (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            มิฉะนั้น ถ้า (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            มิฉะนั้น ถ้า (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            มิฉะนั้น ถ้า (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            มิฉะนั้น {
                bha_nga;
            }
            c = tok_nextc(tok);
            ถ้า (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                จำนวนเต็ม status = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                ถ้า (status < 0) {
                    คืน MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                ถ้า (saw_f || saw_t) {
                    ftstring_kind kind = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    คืน _PyLexer_scan_fstring_start(tok, token, c, kind);
                }
                คืน _PyLexer_scan_string(tok, token, c);
            }
        }
        ขณะ (is_potential_identifier_char(c)) {
            ถ้า (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        ถ้า (nonascii && !verify_identifier(tok)) {
            คืน MAKE_TOKEN(ERRORTOKEN);
        }

        p_start = tok->start;
        p_end = tok->cur;

        คืน MAKE_TOKEN(NAME);
    }

    ถ้า (c == '\r') {
        c = tok_nextc(tok);
    }

    ถ้า (c == '\n') {
        จำนวนเต็ม type = _PyLexer_Newline(tok, token, blankline);
        ถ้า (type == 0) {
            goto nextline;
        }
        คืน type;
    }

    /* Period or number starting with period? */
    ถ้า (c == '.') {
        c = tok_nextc(tok);
        ถ้า (Py_ISDIGIT(c)) {
            คืน _PyLexer_scan_number(tok, token, c, 1);
        } มิฉะนั้น ถ้า (c == '.') {
            c = tok_nextc(tok);
            ถ้า (c == '.') {
                p_start = tok->start;
                p_end = tok->cur;
                คืน MAKE_TOKEN(ELLIPSIS);
            }
            มิฉะนั้น {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        มิฉะนั้น {
            tok_backup(tok, c);
        }
        p_start = tok->start;
        p_end = tok->cur;
        คืน MAKE_TOKEN(DOT);
    }


    /* Number */
    ถ้า (Py_ISDIGIT(c)) {
        คืน _PyLexer_scan_number(tok, token, c, 0);
    }

    /* String */
    ถ้า (c == '\'' || c == '"') {
        คืน _PyLexer_scan_string(tok, token, c);
    }

    /* Line continuation */
    ถ้า (c == '\\') {
        ถ้า ((c = _PyLexer_ContinueLine(tok)) == -1) {
            คืน MAKE_TOKEN(ERRORTOKEN);
        }
        goto again; /* Read next line */
    }

    /* Punctuation character */
    จำนวนเต็ม is_punctuation = (c == ':' || c == '}' || c == '!');
    ถ้า (is_punctuation && current != NULL) {
        จำนวนเต็ม type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        ถ้า (type < 0) {
            คืน MAKE_TOKEN(ERRORTOKEN);
        }
        ถ้า (type != 0) {
            p_start = tok->start;
            p_end = tok->cur;
            คืน MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        จำนวนเต็ม c2 = tok_nextc(tok);
        จำนวนเต็ม current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        ถ้า (current_token != OP) {
            จำนวนเต็ม c3 = tok_nextc(tok);
            จำนวนเต็ม current_token3 = _PyToken_ThreeChars(c, c2, c3);
            ถ้า (current_token3 != OP) {
                current_token = current_token3;
            }
            มิฉะนั้น {
                tok_backup(tok, c3);
            }
            p_start = tok->start;
            p_end = tok->cur;
            คืน MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    switch (c) {
    pak_sa '(':
    pak_sa '[':
    pak_sa '{':
        ถ้า (tok->level >= MAXLEVEL) {
            คืน MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (จำนวนเต็ม)(tok->start - tok->line_start);
        tok->level++;
        bha_nga;
    pak_sa ')':
    pak_sa ']':
    pak_sa '}':
        ถ้า (current != NULL &&
                _PyLexer_FTStringBracketDepth(tok, current) == 0) {
            ถ้า (c == '}') {
                คืน MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    _PyLexer_StringPrefix(current->kind)));
            }
            คืน MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                _PyLexer_StringPrefix(current->kind), c));
        }
        ถ้า (!tok->tok_extra_tokens && !tok->level) {
            คืน MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        ถ้า (tok->level > 0) {
            tok->level--;
            จำนวนเต็ม opening = tok->parenstack[tok->level];
            ถ้า (!tok->tok_extra_tokens && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                ถ้า (current != NULL && opening == '{') {
                    จำนวนเต็ม bracket_depth =
                        _PyLexer_FTStringBracketDepth(tok, current);
                    ถ้า (bracket_depth == current->replacement_depth - 1) {
                        คืน MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            _PyLexer_StringPrefix(current->kind), c));
                    }
                }
                ถ้า (tok->parenlinenostack[tok->level] != tok->lineno) {
                    คืน MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                มิฉะนั้น {
                    คืน MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        ถ้า (current != NULL &&
                _PyLexer_close_ftstring_expr(tok, current, c) < 0) {
            คืน MAKE_TOKEN(ERRORTOKEN);
        }
        bha_nga;
    default:
        bha_nga;
    }

    ถ้า (!Py_UNICODE_ISPRINTABLE(c)) {
        คืน MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    ถ้า (c == '=' && current != NULL) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_start = tok->start;
    p_end = tok->cur;
    คืน MAKE_TOKEN(_PyToken_OneChar(c));
}


void
_PyTokenizer_Get(sa_nracanaa tok_state *tok, sa_nracanaa token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    จำนวนเต็ม result;
    ถ้า (current == NULL) {
        result = _PyLexer_get_normal(tok, NULL, token);
    }
    มิฉะนั้น {
        switch (current->mode) {
        pak_sa FTSTRING_MODE_EXPRESSION:
            result = _PyLexer_get_normal(tok, current, token);
            bha_nga;
        pak_sa FTSTRING_MODE_MIDDLE:
        pak_sa FTSTRING_MODE_FORMAT_SPEC:
            result = _PyLexer_get_ftstring(tok, current, token);
            bha_nga;
        default:
            Py_UNREACHABLE();
        }
    }
    ถ้า (tok_failed(tok) && tok->done != E_INCOMPLETE) {
        result = ERRORTOKEN;
    }
    token->type = result;
}
