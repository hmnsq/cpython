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

ចំនួនគត់
_PyLexer_refill(sa_nracanaa tok_state *tok)
{
    ប្រសិនបើ (tok->done != E_OK) {
        ត្រឡប់ 0;
    }
    ចំនួនគត់ rc = _PyTok_ReaderUnderflow(tok);
#ប្រសិនបើ defined(Py_DEBUG)
    ប្រសិនបើ (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, _PyTok_SourcePointer(&tok->source, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->done);
    }
#endif
    ប្រសិនបើ (!rc) {
        tok->cur = tok->inp;
        ត្រឡប់ 0;
    }
    tok->line_start = tok->cur;
    _PyTok_Span line = {tok->line_start, tok->inp};
    ប្រសិនបើ (_PyTok_SourceFindByte(&tok->source, line, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        ត្រឡប់ 0;
    }
    ត្រឡប់ 1;
}

/* Back-up one character */
void
_PyLexer_backup(sa_nracanaa tok_state *tok, ចំនួនគត់ c)
{
    ប្រសិនបើ (c != EOF) {
        ប្រសិនបើ (--tok->cur < tok->buf_offset) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        ប្រសិនបើ (_PyTok_SourceByte(&tok->source, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
static ចំនួនគត់
verify_identifier(sa_nracanaa tok_state *tok)
{
    ប្រសិនបើ (tok->tok_extra_tokens) {
        ត្រឡប់ 1;
    }
    PyObject *s;
    ប្រសិនបើ (tok_failed(tok))
        ត្រឡប់ 0;
    s = PyUnicode_DecodeUTF8(_PyTok_SourcePointer(&tok->source, tok->start),
                             tok->cur - tok->start, NULL);
    ប្រសិនបើ (s == NULL) {
        ប្រសិនបើ (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->done = E_DECODE;
        }
        បើមិនដូច្នេះ {
            tok->done = E_ERROR;
        }
        ត្រឡប់ 0;
    }
    Py_ssize_t invalid = _PyUnicode_ScanIdentifier(s);
    assert(invalid >= 0);
    assert(PyUnicode_GET_LENGTH(s) > 0);
    ប្រសិនបើ (invalid < PyUnicode_GET_LENGTH(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        ប្រសិនបើ (invalid + 1 < PyUnicode_GET_LENGTH(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            ប្រសិនបើ (s != NULL) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            ប្រសិនបើ (s == NULL) {
                tok->done = E_ERROR;
                ត្រឡប់ 0;
            }
            error_cursor = tok->start + PyBytes_GET_SIZE(s);
        }
        Py_DECREF(s);
        ប្រសិនបើ (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        បើមិនដូច្នេះ {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        ត្រឡប់ 0;
    }
    Py_DECREF(s);
    ត្រឡប់ 1;
}

ចំនួនគត់
_PyLexer_get_normal(sa_nracanaa tok_state *tok, ftstring_state *current, sa_nracanaa token *token)
{
    assert(current == NULL ||
           (current->mode == FTSTRING_MODE_EXPRESSION &&
            current->replacement_depth > 0));
    ចំនួនគត់ c;
    ចំនួនគត់ blankline, nonascii;

    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
  nextline:
    tok->start = -1;
    tok->start_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    ប្រសិនបើ (tok->layout.at_bol) {
        blankline = _PyLexer_BeginLine(tok);
        ប្រសិនបើ (blankline < 0) {
            ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) : -1};

    ប្រសិនបើ (tok->layout.pending != 0) {
        ត្រឡប់ _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->start = -1;
    /* Skip spaces */
    do {
        c = tok_nextc(tok);
    } ខណៈពេល (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->start = tok->cur - 1;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    ប្រសិនបើ (c == '#') {

        ខណៈពេល (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        ប្រសិនបើ (current != NULL) {
            _PyTok_Off comment_end = tok->cur;
            ប្រសិនបើ (c == '\n' || c == '\r') {
                comment_end--;
            }
            ប្រសិនបើ (_PyLexer_record_ftstring_comment(
                    tok, current, tok->start, comment_end) < 0) {
                tok->done = E_NOMEM;
                ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->start;
        ប្រសិនបើ (tok->type_comments) {
            const var_na *prefix = type_comment_prefix;
            ខណៈពេល (*prefix && comment < tok->cur) {
                ប្រសិនបើ (*prefix == ' ') {
                    ខណៈពេល (comment < tok->cur) {
                        ចំនួនគត់ ch = _PyTok_SourceByte(&tok->source, comment);
                        ប្រសិនបើ (ch != ' ' && ch != '\t') {
                            bha_nga;
                        }
                        comment++;
                    }
                } បើមិនដូច្នេះ ប្រសិនបើ (*prefix == _PyTok_SourceByte(&tok->source, comment)) {
                    comment++;
                } បើមិនដូច្នេះ {
                    bha_nga;
                }

                prefix++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            ប្រសិនបើ (!*prefix) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                ចំនួនគត់ is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(_PyTok_SourcePointer(&tok->source, comment),
                           "ignore", 6) == 0;
                ប្រសិនបើ (is_type_ignore && comment + 6 < tok->cur) {
                    ចំនួនគត់ ch = _PyTok_SourceByte(&tok->source, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                ចំនួនគត់ type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_start = comment + (is_type_ignore ? 6 : 0);
                p_end = tok->cur;
                ចំនួនគត់ start_col_offset = tok->start_loc.byte_col +
                    (ចំនួនគត់)(p_start - tok->start);
                /* Consume the newline after a standalone type ignore. */
                ប្រសិនបើ (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                _PyLexer_token_setup(tok, token, type, p_start, p_end);
                token->start_loc = (_PyTok_Loc){tok->lineno, start_col_offset};
                token->end_loc = (_PyTok_Loc){tok->lineno,
                                              _PyLexer_ByteColumn(tok)};
                ត្រឡប់ type;
            }
        }
        ប្រសិនបើ (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.depth > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        ប្រសិនបើ (tok->tok_extra_tokens) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_start = comment;
            p_end = tok->cur;
            tok->layout.comment_newline = blankline;
            ត្រឡប់ MAKE_TOKEN(COMMENT);
        }
    }

    ប្រសិនបើ (tok->done == E_INTERACT_STOP) {
        ត្រឡប់ MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    ប្រសិនបើ (c == EOF) {
        ប្រសិនបើ (tok->level) {
            ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
        }
        ត្រឡប់ MAKE_TOKEN(tok->done == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    ប្រសិនបើ (is_potential_identifier_start(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        ចំនួនគត់ saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        ខណៈពេល (1) {
            ប្រសិនបើ (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            បើមិនដូច្នេះ ប្រសិនបើ (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            បើមិនដូច្នេះ ប្រសិនបើ (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            បើមិនដូច្នេះ ប្រសិនបើ (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            បើមិនដូច្នេះ ប្រសិនបើ (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            បើមិនដូច្នេះ {
                bha_nga;
            }
            c = tok_nextc(tok);
            ប្រសិនបើ (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                ចំនួនគត់ status = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                ប្រសិនបើ (status < 0) {
                    ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                ប្រសិនបើ (saw_f || saw_t) {
                    ftstring_kind kind = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    ត្រឡប់ _PyLexer_scan_fstring_start(tok, token, c, kind);
                }
                ត្រឡប់ _PyLexer_scan_string(tok, token, c);
            }
        }
        ខណៈពេល (is_potential_identifier_char(c)) {
            ប្រសិនបើ (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        ប្រសិនបើ (nonascii && !verify_identifier(tok)) {
            ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
        }

        p_start = tok->start;
        p_end = tok->cur;

        ត្រឡប់ MAKE_TOKEN(NAME);
    }

    ប្រសិនបើ (c == '\r') {
        c = tok_nextc(tok);
    }

    ប្រសិនបើ (c == '\n') {
        ចំនួនគត់ type = _PyLexer_Newline(tok, token, blankline);
        ប្រសិនបើ (type == 0) {
            goto nextline;
        }
        ត្រឡប់ type;
    }

    /* Period or number starting with period? */
    ប្រសិនបើ (c == '.') {
        c = tok_nextc(tok);
        ប្រសិនបើ (Py_ISDIGIT(c)) {
            ត្រឡប់ _PyLexer_scan_number(tok, token, c, 1);
        } បើមិនដូច្នេះ ប្រសិនបើ (c == '.') {
            c = tok_nextc(tok);
            ប្រសិនបើ (c == '.') {
                p_start = tok->start;
                p_end = tok->cur;
                ត្រឡប់ MAKE_TOKEN(ELLIPSIS);
            }
            បើមិនដូច្នេះ {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        បើមិនដូច្នេះ {
            tok_backup(tok, c);
        }
        p_start = tok->start;
        p_end = tok->cur;
        ត្រឡប់ MAKE_TOKEN(DOT);
    }


    /* Number */
    ប្រសិនបើ (Py_ISDIGIT(c)) {
        ត្រឡប់ _PyLexer_scan_number(tok, token, c, 0);
    }

    /* String */
    ប្រសិនបើ (c == '\'' || c == '"') {
        ត្រឡប់ _PyLexer_scan_string(tok, token, c);
    }

    /* Line continuation */
    ប្រសិនបើ (c == '\\') {
        ប្រសិនបើ ((c = _PyLexer_ContinueLine(tok)) == -1) {
            ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
        }
        goto again; /* Read next line */
    }

    /* Punctuation character */
    ចំនួនគត់ is_punctuation = (c == ':' || c == '}' || c == '!');
    ប្រសិនបើ (is_punctuation && current != NULL) {
        ចំនួនគត់ type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        ប្រសិនបើ (type < 0) {
            ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
        }
        ប្រសិនបើ (type != 0) {
            p_start = tok->start;
            p_end = tok->cur;
            ត្រឡប់ MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        ចំនួនគត់ c2 = tok_nextc(tok);
        ចំនួនគត់ current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        ប្រសិនបើ (current_token != OP) {
            ចំនួនគត់ c3 = tok_nextc(tok);
            ចំនួនគត់ current_token3 = _PyToken_ThreeChars(c, c2, c3);
            ប្រសិនបើ (current_token3 != OP) {
                current_token = current_token3;
            }
            បើមិនដូច្នេះ {
                tok_backup(tok, c3);
            }
            p_start = tok->start;
            p_end = tok->cur;
            ត្រឡប់ MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    switch (c) {
    pak_sa '(':
    pak_sa '[':
    pak_sa '{':
        ប្រសិនបើ (tok->level >= MAXLEVEL) {
            ត្រឡប់ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (ចំនួនគត់)(tok->start - tok->line_start);
        tok->level++;
        bha_nga;
    pak_sa ')':
    pak_sa ']':
    pak_sa '}':
        ប្រសិនបើ (current != NULL &&
                _PyLexer_FTStringBracketDepth(tok, current) == 0) {
            ប្រសិនបើ (c == '}') {
                ត្រឡប់ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    _PyLexer_StringPrefix(current->kind)));
            }
            ត្រឡប់ MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                _PyLexer_StringPrefix(current->kind), c));
        }
        ប្រសិនបើ (!tok->tok_extra_tokens && !tok->level) {
            ត្រឡប់ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        ប្រសិនបើ (tok->level > 0) {
            tok->level--;
            ចំនួនគត់ opening = tok->parenstack[tok->level];
            ប្រសិនបើ (!tok->tok_extra_tokens && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                ប្រសិនបើ (current != NULL && opening == '{') {
                    ចំនួនគត់ bracket_depth =
                        _PyLexer_FTStringBracketDepth(tok, current);
                    ប្រសិនបើ (bracket_depth == current->replacement_depth - 1) {
                        ត្រឡប់ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            _PyLexer_StringPrefix(current->kind), c));
                    }
                }
                ប្រសិនបើ (tok->parenlinenostack[tok->level] != tok->lineno) {
                    ត្រឡប់ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                បើមិនដូច្នេះ {
                    ត្រឡប់ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        ប្រសិនបើ (current != NULL &&
                _PyLexer_close_ftstring_expr(tok, current, c) < 0) {
            ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
        }
        bha_nga;
    default:
        bha_nga;
    }

    ប្រសិនបើ (!Py_UNICODE_ISPRINTABLE(c)) {
        ត្រឡប់ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    ប្រសិនបើ (c == '=' && current != NULL) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_start = tok->start;
    p_end = tok->cur;
    ត្រឡប់ MAKE_TOKEN(_PyToken_OneChar(c));
}


void
_PyTokenizer_Get(sa_nracanaa tok_state *tok, sa_nracanaa token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    ចំនួនគត់ result;
    ប្រសិនបើ (current == NULL) {
        result = _PyLexer_get_normal(tok, NULL, token);
    }
    បើមិនដូច្នេះ {
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
    ប្រសិនបើ (tok_failed(tok) && tok->done != E_INCOMPLETE) {
        result = ERRORTOKEN;
    }
    token->type = result;
}
