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

ကိန်းပြည့်
_PyLexer_refill(sa_nracanaa tok_state *tok)
{
    အကယ်၍ (tok->done != E_OK) {
        ပြန်ပေး 0;
    }
    ကိန်းပြည့် rc = _PyTok_ReaderUnderflow(tok);
#အကယ်၍ defined(Py_DEBUG)
    အကယ်၍ (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, _PyTok_SourcePointer(&tok->source, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->done);
    }
#endif
    အကယ်၍ (!rc) {
        tok->cur = tok->inp;
        ပြန်ပေး 0;
    }
    tok->line_start = tok->cur;
    _PyTok_Span line = {tok->line_start, tok->inp};
    အကယ်၍ (_PyTok_SourceFindByte(&tok->source, line, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        ပြန်ပေး 0;
    }
    ပြန်ပေး 1;
}

/* Back-up one character */
void
_PyLexer_backup(sa_nracanaa tok_state *tok, ကိန်းပြည့် c)
{
    အကယ်၍ (c != EOF) {
        အကယ်၍ (--tok->cur < tok->buf_offset) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        အကယ်၍ (_PyTok_SourceByte(&tok->source, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
static ကိန်းပြည့်
verify_identifier(sa_nracanaa tok_state *tok)
{
    အကယ်၍ (tok->tok_extra_tokens) {
        ပြန်ပေး 1;
    }
    PyObject *s;
    အကယ်၍ (tok_failed(tok))
        ပြန်ပေး 0;
    s = PyUnicode_DecodeUTF8(_PyTok_SourcePointer(&tok->source, tok->start),
                             tok->cur - tok->start, NULL);
    အကယ်၍ (s == NULL) {
        အကယ်၍ (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->done = E_DECODE;
        }
        မဟုတ်ပါက {
            tok->done = E_ERROR;
        }
        ပြန်ပေး 0;
    }
    Py_ssize_t invalid = _PyUnicode_ScanIdentifier(s);
    assert(invalid >= 0);
    assert(PyUnicode_GET_LENGTH(s) > 0);
    အကယ်၍ (invalid < PyUnicode_GET_LENGTH(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        အကယ်၍ (invalid + 1 < PyUnicode_GET_LENGTH(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            အကယ်၍ (s != NULL) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            အကယ်၍ (s == NULL) {
                tok->done = E_ERROR;
                ပြန်ပေး 0;
            }
            error_cursor = tok->start + PyBytes_GET_SIZE(s);
        }
        Py_DECREF(s);
        အကယ်၍ (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        မဟုတ်ပါက {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        ပြန်ပေး 0;
    }
    Py_DECREF(s);
    ပြန်ပေး 1;
}

ကိန်းပြည့်
_PyLexer_get_normal(sa_nracanaa tok_state *tok, ftstring_state *current, sa_nracanaa token *token)
{
    assert(current == NULL ||
           (current->mode == FTSTRING_MODE_EXPRESSION &&
            current->replacement_depth > 0));
    ကိန်းပြည့် c;
    ကိန်းပြည့် blankline, nonascii;

    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
  nextline:
    tok->start = -1;
    tok->start_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    အကယ်၍ (tok->layout.at_bol) {
        blankline = _PyLexer_BeginLine(tok);
        အကယ်၍ (blankline < 0) {
            ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) : -1};

    အကယ်၍ (tok->layout.pending != 0) {
        ပြန်ပေး _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->start = -1;
    /* Skip spaces */
    do {
        c = tok_nextc(tok);
    } နေစဉ် (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->start = tok->cur - 1;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    အကယ်၍ (c == '#') {

        နေစဉ် (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        အကယ်၍ (current != NULL) {
            _PyTok_Off comment_end = tok->cur;
            အကယ်၍ (c == '\n' || c == '\r') {
                comment_end--;
            }
            အကယ်၍ (_PyLexer_record_ftstring_comment(
                    tok, current, tok->start, comment_end) < 0) {
                tok->done = E_NOMEM;
                ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->start;
        အကယ်၍ (tok->type_comments) {
            const var_na *prefix = type_comment_prefix;
            နေစဉ် (*prefix && comment < tok->cur) {
                အကယ်၍ (*prefix == ' ') {
                    နေစဉ် (comment < tok->cur) {
                        ကိန်းပြည့် ch = _PyTok_SourceByte(&tok->source, comment);
                        အကယ်၍ (ch != ' ' && ch != '\t') {
                            bha_nga;
                        }
                        comment++;
                    }
                } မဟုတ်ပါက အကယ်၍ (*prefix == _PyTok_SourceByte(&tok->source, comment)) {
                    comment++;
                } မဟုတ်ပါက {
                    bha_nga;
                }

                prefix++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            အကယ်၍ (!*prefix) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                ကိန်းပြည့် is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(_PyTok_SourcePointer(&tok->source, comment),
                           "ignore", 6) == 0;
                အကယ်၍ (is_type_ignore && comment + 6 < tok->cur) {
                    ကိန်းပြည့် ch = _PyTok_SourceByte(&tok->source, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                ကိန်းပြည့် type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_start = comment + (is_type_ignore ? 6 : 0);
                p_end = tok->cur;
                ကိန်းပြည့် start_col_offset = tok->start_loc.byte_col +
                    (ကိန်းပြည့်)(p_start - tok->start);
                /* Consume the newline after a standalone type ignore. */
                အကယ်၍ (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                _PyLexer_token_setup(tok, token, type, p_start, p_end);
                token->start_loc = (_PyTok_Loc){tok->lineno, start_col_offset};
                token->end_loc = (_PyTok_Loc){tok->lineno,
                                              _PyLexer_ByteColumn(tok)};
                ပြန်ပေး type;
            }
        }
        အကယ်၍ (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.depth > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        အကယ်၍ (tok->tok_extra_tokens) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_start = comment;
            p_end = tok->cur;
            tok->layout.comment_newline = blankline;
            ပြန်ပေး MAKE_TOKEN(COMMENT);
        }
    }

    အကယ်၍ (tok->done == E_INTERACT_STOP) {
        ပြန်ပေး MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    အကယ်၍ (c == EOF) {
        အကယ်၍ (tok->level) {
            ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
        }
        ပြန်ပေး MAKE_TOKEN(tok->done == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    အကယ်၍ (is_potential_identifier_start(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        ကိန်းပြည့် saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        နေစဉ် (1) {
            အကယ်၍ (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            မဟုတ်ပါက အကယ်၍ (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            မဟုတ်ပါက အကယ်၍ (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            မဟုတ်ပါက အကယ်၍ (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            မဟုတ်ပါက အကယ်၍ (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            မဟုတ်ပါက {
                bha_nga;
            }
            c = tok_nextc(tok);
            အကယ်၍ (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                ကိန်းပြည့် status = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                အကယ်၍ (status < 0) {
                    ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                အကယ်၍ (saw_f || saw_t) {
                    ftstring_kind kind = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    ပြန်ပေး _PyLexer_scan_fstring_start(tok, token, c, kind);
                }
                ပြန်ပေး _PyLexer_scan_string(tok, token, c);
            }
        }
        နေစဉ် (is_potential_identifier_char(c)) {
            အကယ်၍ (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        အကယ်၍ (nonascii && !verify_identifier(tok)) {
            ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
        }

        p_start = tok->start;
        p_end = tok->cur;

        ပြန်ပေး MAKE_TOKEN(NAME);
    }

    အကယ်၍ (c == '\r') {
        c = tok_nextc(tok);
    }

    အကယ်၍ (c == '\n') {
        ကိန်းပြည့် type = _PyLexer_Newline(tok, token, blankline);
        အကယ်၍ (type == 0) {
            goto nextline;
        }
        ပြန်ပေး type;
    }

    /* Period or number starting with period? */
    အကယ်၍ (c == '.') {
        c = tok_nextc(tok);
        အကယ်၍ (Py_ISDIGIT(c)) {
            ပြန်ပေး _PyLexer_scan_number(tok, token, c, 1);
        } မဟုတ်ပါက အကယ်၍ (c == '.') {
            c = tok_nextc(tok);
            အကယ်၍ (c == '.') {
                p_start = tok->start;
                p_end = tok->cur;
                ပြန်ပေး MAKE_TOKEN(ELLIPSIS);
            }
            မဟုတ်ပါက {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        မဟုတ်ပါက {
            tok_backup(tok, c);
        }
        p_start = tok->start;
        p_end = tok->cur;
        ပြန်ပေး MAKE_TOKEN(DOT);
    }


    /* Number */
    အကယ်၍ (Py_ISDIGIT(c)) {
        ပြန်ပေး _PyLexer_scan_number(tok, token, c, 0);
    }

    /* String */
    အကယ်၍ (c == '\'' || c == '"') {
        ပြန်ပေး _PyLexer_scan_string(tok, token, c);
    }

    /* Line continuation */
    အကယ်၍ (c == '\\') {
        အကယ်၍ ((c = _PyLexer_ContinueLine(tok)) == -1) {
            ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
        }
        goto again; /* Read next line */
    }

    /* Punctuation character */
    ကိန်းပြည့် is_punctuation = (c == ':' || c == '}' || c == '!');
    အကယ်၍ (is_punctuation && current != NULL) {
        ကိန်းပြည့် type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        အကယ်၍ (type < 0) {
            ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
        }
        အကယ်၍ (type != 0) {
            p_start = tok->start;
            p_end = tok->cur;
            ပြန်ပေး MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        ကိန်းပြည့် c2 = tok_nextc(tok);
        ကိန်းပြည့် current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        အကယ်၍ (current_token != OP) {
            ကိန်းပြည့် c3 = tok_nextc(tok);
            ကိန်းပြည့် current_token3 = _PyToken_ThreeChars(c, c2, c3);
            အကယ်၍ (current_token3 != OP) {
                current_token = current_token3;
            }
            မဟုတ်ပါက {
                tok_backup(tok, c3);
            }
            p_start = tok->start;
            p_end = tok->cur;
            ပြန်ပေး MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    switch (c) {
    pak_sa '(':
    pak_sa '[':
    pak_sa '{':
        အကယ်၍ (tok->level >= MAXLEVEL) {
            ပြန်ပေး MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (ကိန်းပြည့်)(tok->start - tok->line_start);
        tok->level++;
        bha_nga;
    pak_sa ')':
    pak_sa ']':
    pak_sa '}':
        အကယ်၍ (current != NULL &&
                _PyLexer_FTStringBracketDepth(tok, current) == 0) {
            အကယ်၍ (c == '}') {
                ပြန်ပေး MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    _PyLexer_StringPrefix(current->kind)));
            }
            ပြန်ပေး MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                _PyLexer_StringPrefix(current->kind), c));
        }
        အကယ်၍ (!tok->tok_extra_tokens && !tok->level) {
            ပြန်ပေး MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        အကယ်၍ (tok->level > 0) {
            tok->level--;
            ကိန်းပြည့် opening = tok->parenstack[tok->level];
            အကယ်၍ (!tok->tok_extra_tokens && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                အကယ်၍ (current != NULL && opening == '{') {
                    ကိန်းပြည့် bracket_depth =
                        _PyLexer_FTStringBracketDepth(tok, current);
                    အကယ်၍ (bracket_depth == current->replacement_depth - 1) {
                        ပြန်ပေး MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            _PyLexer_StringPrefix(current->kind), c));
                    }
                }
                အကယ်၍ (tok->parenlinenostack[tok->level] != tok->lineno) {
                    ပြန်ပေး MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                မဟုတ်ပါက {
                    ပြန်ပေး MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        အကယ်၍ (current != NULL &&
                _PyLexer_close_ftstring_expr(tok, current, c) < 0) {
            ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
        }
        bha_nga;
    default:
        bha_nga;
    }

    အကယ်၍ (!Py_UNICODE_ISPRINTABLE(c)) {
        ပြန်ပေး MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    အကယ်၍ (c == '=' && current != NULL) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_start = tok->start;
    p_end = tok->cur;
    ပြန်ပေး MAKE_TOKEN(_PyToken_OneChar(c));
}


void
_PyTokenizer_Get(sa_nracanaa tok_state *tok, sa_nracanaa token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    ကိန်းပြည့် result;
    အကယ်၍ (current == NULL) {
        result = _PyLexer_get_normal(tok, NULL, token);
    }
    မဟုတ်ပါက {
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
    အကယ်၍ (tok_failed(tok) && tok->done != E_INCOMPLETE) {
        result = ERRORTOKEN;
    }
    token->type = result;
}
