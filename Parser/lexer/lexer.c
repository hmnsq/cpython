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

ཧྲིལ
_PyLexer_refill(sa_nracanaa tok_state *tok)
{
    གལ་ཏེ (tok->done != E_OK) {
        ཕྱིར 0;
    }
    ཧྲིལ rc = _PyTok_ReaderUnderflow(tok);
#གལ་ཏེ defined(Py_DEBUG)
    གལ་ཏེ (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, _PyTok_SourcePointer(&tok->source, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->done);
    }
#endif
    གལ་ཏེ (!rc) {
        tok->cur = tok->inp;
        ཕྱིར 0;
    }
    tok->line_start = tok->cur;
    _PyTok_Span line = {tok->line_start, tok->inp};
    གལ་ཏེ (_PyTok_SourceFindByte(&tok->source, line, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        ཕྱིར 0;
    }
    ཕྱིར 1;
}

/* Back-up one character */
void
_PyLexer_backup(sa_nracanaa tok_state *tok, ཧྲིལ c)
{
    གལ་ཏེ (c != EOF) {
        གལ་ཏེ (--tok->cur < tok->buf_offset) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        གལ་ཏེ (_PyTok_SourceByte(&tok->source, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
static ཧྲིལ
verify_identifier(sa_nracanaa tok_state *tok)
{
    གལ་ཏེ (tok->tok_extra_tokens) {
        ཕྱིར 1;
    }
    PyObject *s;
    གལ་ཏེ (tok_failed(tok))
        ཕྱིར 0;
    s = PyUnicode_DecodeUTF8(_PyTok_SourcePointer(&tok->source, tok->start),
                             tok->cur - tok->start, NULL);
    གལ་ཏེ (s == NULL) {
        གལ་ཏེ (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->done = E_DECODE;
        }
        དེ་མིན {
            tok->done = E_ERROR;
        }
        ཕྱིར 0;
    }
    Py_ssize_t invalid = _PyUnicode_ScanIdentifier(s);
    assert(invalid >= 0);
    assert(PyUnicode_GET_LENGTH(s) > 0);
    གལ་ཏེ (invalid < PyUnicode_GET_LENGTH(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        གལ་ཏེ (invalid + 1 < PyUnicode_GET_LENGTH(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            གལ་ཏེ (s != NULL) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            གལ་ཏེ (s == NULL) {
                tok->done = E_ERROR;
                ཕྱིར 0;
            }
            error_cursor = tok->start + PyBytes_GET_SIZE(s);
        }
        Py_DECREF(s);
        གལ་ཏེ (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        དེ་མིན {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        ཕྱིར 0;
    }
    Py_DECREF(s);
    ཕྱིར 1;
}

ཧྲིལ
_PyLexer_get_normal(sa_nracanaa tok_state *tok, ftstring_state *current, sa_nracanaa token *token)
{
    assert(current == NULL ||
           (current->mode == FTSTRING_MODE_EXPRESSION &&
            current->replacement_depth > 0));
    ཧྲིལ c;
    ཧྲིལ blankline, nonascii;

    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
  nextline:
    tok->start = -1;
    tok->start_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    གལ་ཏེ (tok->layout.at_bol) {
        blankline = _PyLexer_BeginLine(tok);
        གལ་ཏེ (blankline < 0) {
            ཕྱིར MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) : -1};

    གལ་ཏེ (tok->layout.pending != 0) {
        ཕྱིར _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->start = -1;
    /* Skip spaces */
    do {
        c = tok_nextc(tok);
    } སྐབས (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->start = tok->cur - 1;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    གལ་ཏེ (c == '#') {

        སྐབས (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        གལ་ཏེ (current != NULL) {
            _PyTok_Off comment_end = tok->cur;
            གལ་ཏེ (c == '\n' || c == '\r') {
                comment_end--;
            }
            གལ་ཏེ (_PyLexer_record_ftstring_comment(
                    tok, current, tok->start, comment_end) < 0) {
                tok->done = E_NOMEM;
                ཕྱིར MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->start;
        གལ་ཏེ (tok->type_comments) {
            const var_na *prefix = type_comment_prefix;
            སྐབས (*prefix && comment < tok->cur) {
                གལ་ཏེ (*prefix == ' ') {
                    སྐབས (comment < tok->cur) {
                        ཧྲིལ ch = _PyTok_SourceByte(&tok->source, comment);
                        གལ་ཏེ (ch != ' ' && ch != '\t') {
                            bha_nga;
                        }
                        comment++;
                    }
                } དེ་མིན གལ་ཏེ (*prefix == _PyTok_SourceByte(&tok->source, comment)) {
                    comment++;
                } དེ་མིན {
                    bha_nga;
                }

                prefix++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            གལ་ཏེ (!*prefix) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                ཧྲིལ is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(_PyTok_SourcePointer(&tok->source, comment),
                           "ignore", 6) == 0;
                གལ་ཏེ (is_type_ignore && comment + 6 < tok->cur) {
                    ཧྲིལ ch = _PyTok_SourceByte(&tok->source, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                ཧྲིལ type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_start = comment + (is_type_ignore ? 6 : 0);
                p_end = tok->cur;
                ཧྲིལ start_col_offset = tok->start_loc.byte_col +
                    (ཧྲིལ)(p_start - tok->start);
                /* Consume the newline after a standalone type ignore. */
                གལ་ཏེ (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                _PyLexer_token_setup(tok, token, type, p_start, p_end);
                token->start_loc = (_PyTok_Loc){tok->lineno, start_col_offset};
                token->end_loc = (_PyTok_Loc){tok->lineno,
                                              _PyLexer_ByteColumn(tok)};
                ཕྱིར type;
            }
        }
        གལ་ཏེ (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.depth > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        གལ་ཏེ (tok->tok_extra_tokens) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_start = comment;
            p_end = tok->cur;
            tok->layout.comment_newline = blankline;
            ཕྱིར MAKE_TOKEN(COMMENT);
        }
    }

    གལ་ཏེ (tok->done == E_INTERACT_STOP) {
        ཕྱིར MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    གལ་ཏེ (c == EOF) {
        གལ་ཏེ (tok->level) {
            ཕྱིར MAKE_TOKEN(ERRORTOKEN);
        }
        ཕྱིར MAKE_TOKEN(tok->done == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    གལ་ཏེ (is_potential_identifier_start(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        ཧྲིལ saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        སྐབས (1) {
            གལ་ཏེ (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            དེ་མིན གལ་ཏེ (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            དེ་མིན གལ་ཏེ (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            དེ་མིན གལ་ཏེ (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            དེ་མིན གལ་ཏེ (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            དེ་མིན {
                bha_nga;
            }
            c = tok_nextc(tok);
            གལ་ཏེ (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                ཧྲིལ status = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                གལ་ཏེ (status < 0) {
                    ཕྱིར MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                གལ་ཏེ (saw_f || saw_t) {
                    ftstring_kind kind = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    ཕྱིར _PyLexer_scan_fstring_start(tok, token, c, kind);
                }
                ཕྱིར _PyLexer_scan_string(tok, token, c);
            }
        }
        སྐབས (is_potential_identifier_char(c)) {
            གལ་ཏེ (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        གལ་ཏེ (nonascii && !verify_identifier(tok)) {
            ཕྱིར MAKE_TOKEN(ERRORTOKEN);
        }

        p_start = tok->start;
        p_end = tok->cur;

        ཕྱིར MAKE_TOKEN(NAME);
    }

    གལ་ཏེ (c == '\r') {
        c = tok_nextc(tok);
    }

    གལ་ཏེ (c == '\n') {
        ཧྲིལ type = _PyLexer_Newline(tok, token, blankline);
        གལ་ཏེ (type == 0) {
            goto nextline;
        }
        ཕྱིར type;
    }

    /* Period or number starting with period? */
    གལ་ཏེ (c == '.') {
        c = tok_nextc(tok);
        གལ་ཏེ (Py_ISDIGIT(c)) {
            ཕྱིར _PyLexer_scan_number(tok, token, c, 1);
        } དེ་མིན གལ་ཏེ (c == '.') {
            c = tok_nextc(tok);
            གལ་ཏེ (c == '.') {
                p_start = tok->start;
                p_end = tok->cur;
                ཕྱིར MAKE_TOKEN(ELLIPSIS);
            }
            དེ་མིན {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        དེ་མིན {
            tok_backup(tok, c);
        }
        p_start = tok->start;
        p_end = tok->cur;
        ཕྱིར MAKE_TOKEN(DOT);
    }


    /* Number */
    གལ་ཏེ (Py_ISDIGIT(c)) {
        ཕྱིར _PyLexer_scan_number(tok, token, c, 0);
    }

    /* String */
    གལ་ཏེ (c == '\'' || c == '"') {
        ཕྱིར _PyLexer_scan_string(tok, token, c);
    }

    /* Line continuation */
    གལ་ཏེ (c == '\\') {
        གལ་ཏེ ((c = _PyLexer_ContinueLine(tok)) == -1) {
            ཕྱིར MAKE_TOKEN(ERRORTOKEN);
        }
        goto again; /* Read next line */
    }

    /* Punctuation character */
    ཧྲིལ is_punctuation = (c == ':' || c == '}' || c == '!');
    གལ་ཏེ (is_punctuation && current != NULL) {
        ཧྲིལ type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        གལ་ཏེ (type < 0) {
            ཕྱིར MAKE_TOKEN(ERRORTOKEN);
        }
        གལ་ཏེ (type != 0) {
            p_start = tok->start;
            p_end = tok->cur;
            ཕྱིར MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        ཧྲིལ c2 = tok_nextc(tok);
        ཧྲིལ current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        གལ་ཏེ (current_token != OP) {
            ཧྲིལ c3 = tok_nextc(tok);
            ཧྲིལ current_token3 = _PyToken_ThreeChars(c, c2, c3);
            གལ་ཏེ (current_token3 != OP) {
                current_token = current_token3;
            }
            དེ་མིན {
                tok_backup(tok, c3);
            }
            p_start = tok->start;
            p_end = tok->cur;
            ཕྱིར MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    switch (c) {
    pak_sa '(':
    pak_sa '[':
    pak_sa '{':
        གལ་ཏེ (tok->level >= MAXLEVEL) {
            ཕྱིར MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (ཧྲིལ)(tok->start - tok->line_start);
        tok->level++;
        bha_nga;
    pak_sa ')':
    pak_sa ']':
    pak_sa '}':
        གལ་ཏེ (current != NULL &&
                _PyLexer_FTStringBracketDepth(tok, current) == 0) {
            གལ་ཏེ (c == '}') {
                ཕྱིར MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    _PyLexer_StringPrefix(current->kind)));
            }
            ཕྱིར MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                _PyLexer_StringPrefix(current->kind), c));
        }
        གལ་ཏེ (!tok->tok_extra_tokens && !tok->level) {
            ཕྱིར MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        གལ་ཏེ (tok->level > 0) {
            tok->level--;
            ཧྲིལ opening = tok->parenstack[tok->level];
            གལ་ཏེ (!tok->tok_extra_tokens && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                གལ་ཏེ (current != NULL && opening == '{') {
                    ཧྲིལ bracket_depth =
                        _PyLexer_FTStringBracketDepth(tok, current);
                    གལ་ཏེ (bracket_depth == current->replacement_depth - 1) {
                        ཕྱིར MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            _PyLexer_StringPrefix(current->kind), c));
                    }
                }
                གལ་ཏེ (tok->parenlinenostack[tok->level] != tok->lineno) {
                    ཕྱིར MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                དེ་མིན {
                    ཕྱིར MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        གལ་ཏེ (current != NULL &&
                _PyLexer_close_ftstring_expr(tok, current, c) < 0) {
            ཕྱིར MAKE_TOKEN(ERRORTOKEN);
        }
        bha_nga;
    default:
        bha_nga;
    }

    གལ་ཏེ (!Py_UNICODE_ISPRINTABLE(c)) {
        ཕྱིར MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    གལ་ཏེ (c == '=' && current != NULL) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_start = tok->start;
    p_end = tok->cur;
    ཕྱིར MAKE_TOKEN(_PyToken_OneChar(c));
}


void
_PyTokenizer_Get(sa_nracanaa tok_state *tok, sa_nracanaa token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    ཧྲིལ result;
    གལ་ཏེ (current == NULL) {
        result = _PyLexer_get_normal(tok, NULL, token);
    }
    དེ་མིན {
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
    གལ་ཏེ (tok_failed(tok) && tok->done != E_INCOMPLETE) {
        result = ERRORTOKEN;
    }
    token->type = result;
}
