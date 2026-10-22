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

puur_naa_nka
_PyLexer_refill(sa_nracanaa tok_state *tok)
{
    ᬬᬾᬦ᭄ (tok->done != E_OK) {
        ᬯᬮᬶ 0;
    }
    puur_naa_nka rc = _PyTok_ReaderUnderflow(tok);
#ᬬᬾᬦ᭄ defined(Py_DEBUG)
    ᬬᬾᬦ᭄ (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, _PyTok_SourcePointer(&tok->source, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->done);
    }
#endif
    ᬬᬾᬦ᭄ (!rc) {
        tok->cur = tok->inp;
        ᬯᬮᬶ 0;
    }
    tok->line_start = tok->cur;
    _PyTok_Span line = {tok->line_start, tok->inp};
    ᬬᬾᬦ᭄ (_PyTok_SourceFindByte(&tok->source, line, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        ᬯᬮᬶ 0;
    }
    ᬯᬮᬶ 1;
}

/* Back-up one character */
void
_PyLexer_backup(sa_nracanaa tok_state *tok, puur_naa_nka c)
{
    ᬬᬾᬦ᭄ (c != EOF) {
        ᬬᬾᬦ᭄ (--tok->cur < tok->buf_offset) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        ᬬᬾᬦ᭄ (_PyTok_SourceByte(&tok->source, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
static puur_naa_nka
verify_identifier(sa_nracanaa tok_state *tok)
{
    ᬬᬾᬦ᭄ (tok->tok_extra_tokens) {
        ᬯᬮᬶ 1;
    }
    PyObject *s;
    ᬬᬾᬦ᭄ (tok_failed(tok))
        ᬯᬮᬶ 0;
    s = PyUnicode_DecodeUTF8(_PyTok_SourcePointer(&tok->source, tok->start),
                             tok->cur - tok->start, NULL);
    ᬬᬾᬦ᭄ (s == NULL) {
        ᬬᬾᬦ᭄ (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->done = E_DECODE;
        }
        ᬮᬶᬬ {
            tok->done = E_ERROR;
        }
        ᬯᬮᬶ 0;
    }
    Py_ssize_t invalid = _PyUnicode_ScanIdentifier(s);
    assert(invalid >= 0);
    assert(PyUnicode_GET_LENGTH(s) > 0);
    ᬬᬾᬦ᭄ (invalid < PyUnicode_GET_LENGTH(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        ᬬᬾᬦ᭄ (invalid + 1 < PyUnicode_GET_LENGTH(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            ᬬᬾᬦ᭄ (s != NULL) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            ᬬᬾᬦ᭄ (s == NULL) {
                tok->done = E_ERROR;
                ᬯᬮᬶ 0;
            }
            error_cursor = tok->start + PyBytes_GET_SIZE(s);
        }
        Py_DECREF(s);
        ᬬᬾᬦ᭄ (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        ᬮᬶᬬ {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        ᬯᬮᬶ 0;
    }
    Py_DECREF(s);
    ᬯᬮᬶ 1;
}

puur_naa_nka
_PyLexer_get_normal(sa_nracanaa tok_state *tok, ftstring_state *current, sa_nracanaa token *token)
{
    assert(current == NULL ||
           (current->mode == FTSTRING_MODE_EXPRESSION &&
            current->replacement_depth > 0));
    puur_naa_nka c;
    puur_naa_nka blankline, nonascii;

    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
  nextline:
    tok->start = -1;
    tok->start_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    ᬬᬾᬦ᭄ (tok->layout.at_bol) {
        blankline = _PyLexer_BeginLine(tok);
        ᬬᬾᬦ᭄ (blankline < 0) {
            ᬯᬮᬶ MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) : -1};

    ᬬᬾᬦ᭄ (tok->layout.pending != 0) {
        ᬯᬮᬶ _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->start = -1;
    /* Skip spaces */
    do {
        c = tok_nextc(tok);
    } ᬲᭂᬤᭂᬮ (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->start = tok->cur - 1;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    ᬬᬾᬦ᭄ (c == '#') {

        ᬲᭂᬤᭂᬮ (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        ᬬᬾᬦ᭄ (current != NULL) {
            _PyTok_Off comment_end = tok->cur;
            ᬬᬾᬦ᭄ (c == '\n' || c == '\r') {
                comment_end--;
            }
            ᬬᬾᬦ᭄ (_PyLexer_record_ftstring_comment(
                    tok, current, tok->start, comment_end) < 0) {
                tok->done = E_NOMEM;
                ᬯᬮᬶ MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->start;
        ᬬᬾᬦ᭄ (tok->type_comments) {
            const var_na *prefix = type_comment_prefix;
            ᬲᭂᬤᭂᬮ (*prefix && comment < tok->cur) {
                ᬬᬾᬦ᭄ (*prefix == ' ') {
                    ᬲᭂᬤᭂᬮ (comment < tok->cur) {
                        puur_naa_nka ch = _PyTok_SourceByte(&tok->source, comment);
                        ᬬᬾᬦ᭄ (ch != ' ' && ch != '\t') {
                            bha_nga;
                        }
                        comment++;
                    }
                } ᬮᬶᬬ ᬬᬾᬦ᭄ (*prefix == _PyTok_SourceByte(&tok->source, comment)) {
                    comment++;
                } ᬮᬶᬬ {
                    bha_nga;
                }

                prefix++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            ᬬᬾᬦ᭄ (!*prefix) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                puur_naa_nka is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(_PyTok_SourcePointer(&tok->source, comment),
                           "ignore", 6) == 0;
                ᬬᬾᬦ᭄ (is_type_ignore && comment + 6 < tok->cur) {
                    puur_naa_nka ch = _PyTok_SourceByte(&tok->source, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                puur_naa_nka type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_start = comment + (is_type_ignore ? 6 : 0);
                p_end = tok->cur;
                puur_naa_nka start_col_offset = tok->start_loc.byte_col +
                    (puur_naa_nka)(p_start - tok->start);
                /* Consume the newline after a standalone type ignore. */
                ᬬᬾᬦ᭄ (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                _PyLexer_token_setup(tok, token, type, p_start, p_end);
                token->start_loc = (_PyTok_Loc){tok->lineno, start_col_offset};
                token->end_loc = (_PyTok_Loc){tok->lineno,
                                              _PyLexer_ByteColumn(tok)};
                ᬯᬮᬶ type;
            }
        }
        ᬬᬾᬦ᭄ (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.depth > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        ᬬᬾᬦ᭄ (tok->tok_extra_tokens) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_start = comment;
            p_end = tok->cur;
            tok->layout.comment_newline = blankline;
            ᬯᬮᬶ MAKE_TOKEN(COMMENT);
        }
    }

    ᬬᬾᬦ᭄ (tok->done == E_INTERACT_STOP) {
        ᬯᬮᬶ MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    ᬬᬾᬦ᭄ (c == EOF) {
        ᬬᬾᬦ᭄ (tok->level) {
            ᬯᬮᬶ MAKE_TOKEN(ERRORTOKEN);
        }
        ᬯᬮᬶ MAKE_TOKEN(tok->done == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    ᬬᬾᬦ᭄ (is_potential_identifier_start(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        puur_naa_nka saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        ᬲᭂᬤᭂᬮ (1) {
            ᬬᬾᬦ᭄ (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            ᬮᬶᬬ ᬬᬾᬦ᭄ (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            ᬮᬶᬬ ᬬᬾᬦ᭄ (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            ᬮᬶᬬ ᬬᬾᬦ᭄ (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            ᬮᬶᬬ ᬬᬾᬦ᭄ (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            ᬮᬶᬬ {
                bha_nga;
            }
            c = tok_nextc(tok);
            ᬬᬾᬦ᭄ (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                puur_naa_nka status = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                ᬬᬾᬦ᭄ (status < 0) {
                    ᬯᬮᬶ MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                ᬬᬾᬦ᭄ (saw_f || saw_t) {
                    ftstring_kind kind = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    ᬯᬮᬶ _PyLexer_scan_fstring_start(tok, token, c, kind);
                }
                ᬯᬮᬶ _PyLexer_scan_string(tok, token, c);
            }
        }
        ᬲᭂᬤᭂᬮ (is_potential_identifier_char(c)) {
            ᬬᬾᬦ᭄ (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        ᬬᬾᬦ᭄ (nonascii && !verify_identifier(tok)) {
            ᬯᬮᬶ MAKE_TOKEN(ERRORTOKEN);
        }

        p_start = tok->start;
        p_end = tok->cur;

        ᬯᬮᬶ MAKE_TOKEN(NAME);
    }

    ᬬᬾᬦ᭄ (c == '\r') {
        c = tok_nextc(tok);
    }

    ᬬᬾᬦ᭄ (c == '\n') {
        puur_naa_nka type = _PyLexer_Newline(tok, token, blankline);
        ᬬᬾᬦ᭄ (type == 0) {
            goto nextline;
        }
        ᬯᬮᬶ type;
    }

    /* Period or number starting with period? */
    ᬬᬾᬦ᭄ (c == '.') {
        c = tok_nextc(tok);
        ᬬᬾᬦ᭄ (Py_ISDIGIT(c)) {
            ᬯᬮᬶ _PyLexer_scan_number(tok, token, c, 1);
        } ᬮᬶᬬ ᬬᬾᬦ᭄ (c == '.') {
            c = tok_nextc(tok);
            ᬬᬾᬦ᭄ (c == '.') {
                p_start = tok->start;
                p_end = tok->cur;
                ᬯᬮᬶ MAKE_TOKEN(ELLIPSIS);
            }
            ᬮᬶᬬ {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        ᬮᬶᬬ {
            tok_backup(tok, c);
        }
        p_start = tok->start;
        p_end = tok->cur;
        ᬯᬮᬶ MAKE_TOKEN(DOT);
    }


    /* Number */
    ᬬᬾᬦ᭄ (Py_ISDIGIT(c)) {
        ᬯᬮᬶ _PyLexer_scan_number(tok, token, c, 0);
    }

    /* String */
    ᬬᬾᬦ᭄ (c == '\'' || c == '"') {
        ᬯᬮᬶ _PyLexer_scan_string(tok, token, c);
    }

    /* Line continuation */
    ᬬᬾᬦ᭄ (c == '\\') {
        ᬬᬾᬦ᭄ ((c = _PyLexer_ContinueLine(tok)) == -1) {
            ᬯᬮᬶ MAKE_TOKEN(ERRORTOKEN);
        }
        goto again; /* Read next line */
    }

    /* Punctuation character */
    puur_naa_nka is_punctuation = (c == ':' || c == '}' || c == '!');
    ᬬᬾᬦ᭄ (is_punctuation && current != NULL) {
        puur_naa_nka type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        ᬬᬾᬦ᭄ (type < 0) {
            ᬯᬮᬶ MAKE_TOKEN(ERRORTOKEN);
        }
        ᬬᬾᬦ᭄ (type != 0) {
            p_start = tok->start;
            p_end = tok->cur;
            ᬯᬮᬶ MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        puur_naa_nka c2 = tok_nextc(tok);
        puur_naa_nka current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        ᬬᬾᬦ᭄ (current_token != OP) {
            puur_naa_nka c3 = tok_nextc(tok);
            puur_naa_nka current_token3 = _PyToken_ThreeChars(c, c2, c3);
            ᬬᬾᬦ᭄ (current_token3 != OP) {
                current_token = current_token3;
            }
            ᬮᬶᬬ {
                tok_backup(tok, c3);
            }
            p_start = tok->start;
            p_end = tok->cur;
            ᬯᬮᬶ MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    switch (c) {
    pak_sa '(':
    pak_sa '[':
    pak_sa '{':
        ᬬᬾᬦ᭄ (tok->level >= MAXLEVEL) {
            ᬯᬮᬶ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (puur_naa_nka)(tok->start - tok->line_start);
        tok->level++;
        bha_nga;
    pak_sa ')':
    pak_sa ']':
    pak_sa '}':
        ᬬᬾᬦ᭄ (current != NULL &&
                _PyLexer_FTStringBracketDepth(tok, current) == 0) {
            ᬬᬾᬦ᭄ (c == '}') {
                ᬯᬮᬶ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    _PyLexer_StringPrefix(current->kind)));
            }
            ᬯᬮᬶ MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                _PyLexer_StringPrefix(current->kind), c));
        }
        ᬬᬾᬦ᭄ (!tok->tok_extra_tokens && !tok->level) {
            ᬯᬮᬶ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        ᬬᬾᬦ᭄ (tok->level > 0) {
            tok->level--;
            puur_naa_nka opening = tok->parenstack[tok->level];
            ᬬᬾᬦ᭄ (!tok->tok_extra_tokens && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                ᬬᬾᬦ᭄ (current != NULL && opening == '{') {
                    puur_naa_nka bracket_depth =
                        _PyLexer_FTStringBracketDepth(tok, current);
                    ᬬᬾᬦ᭄ (bracket_depth == current->replacement_depth - 1) {
                        ᬯᬮᬶ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            _PyLexer_StringPrefix(current->kind), c));
                    }
                }
                ᬬᬾᬦ᭄ (tok->parenlinenostack[tok->level] != tok->lineno) {
                    ᬯᬮᬶ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                ᬮᬶᬬ {
                    ᬯᬮᬶ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        ᬬᬾᬦ᭄ (current != NULL &&
                _PyLexer_close_ftstring_expr(tok, current, c) < 0) {
            ᬯᬮᬶ MAKE_TOKEN(ERRORTOKEN);
        }
        bha_nga;
    default:
        bha_nga;
    }

    ᬬᬾᬦ᭄ (!Py_UNICODE_ISPRINTABLE(c)) {
        ᬯᬮᬶ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    ᬬᬾᬦ᭄ (c == '=' && current != NULL) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_start = tok->start;
    p_end = tok->cur;
    ᬯᬮᬶ MAKE_TOKEN(_PyToken_OneChar(c));
}


void
_PyTokenizer_Get(sa_nracanaa tok_state *tok, sa_nracanaa token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    puur_naa_nka result;
    ᬬᬾᬦ᭄ (current == NULL) {
        result = _PyLexer_get_normal(tok, NULL, token);
    }
    ᬮᬶᬬ {
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
    ᬬᬾᬦ᭄ (tok_failed(tok) && tok->done != E_INCOMPLETE) {
        result = ERRORTOKEN;
    }
    token->type = result;
}
