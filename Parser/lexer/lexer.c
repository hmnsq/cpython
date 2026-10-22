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

ຈຳນວນເຕັມ
_PyLexer_refill(sa_nracanaa tok_state *tok)
{
    ຖ້າ (tok->done != E_OK) {
        ສົ່ງຄືນ 0;
    }
    ຈຳນວນເຕັມ rc = _PyTok_ReaderUnderflow(tok);
#ຖ້າ defined(Py_DEBUG)
    ຖ້າ (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, _PyTok_SourcePointer(&tok->source, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->done);
    }
#endif
    ຖ້າ (!rc) {
        tok->cur = tok->inp;
        ສົ່ງຄືນ 0;
    }
    tok->line_start = tok->cur;
    _PyTok_Span line = {tok->line_start, tok->inp};
    ຖ້າ (_PyTok_SourceFindByte(&tok->source, line, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        ສົ່ງຄືນ 0;
    }
    ສົ່ງຄືນ 1;
}

/* Back-up one character */
void
_PyLexer_backup(sa_nracanaa tok_state *tok, ຈຳນວນເຕັມ c)
{
    ຖ້າ (c != EOF) {
        ຖ້າ (--tok->cur < tok->buf_offset) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        ຖ້າ (_PyTok_SourceByte(&tok->source, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
static ຈຳນວນເຕັມ
verify_identifier(sa_nracanaa tok_state *tok)
{
    ຖ້າ (tok->tok_extra_tokens) {
        ສົ່ງຄືນ 1;
    }
    PyObject *s;
    ຖ້າ (tok_failed(tok))
        ສົ່ງຄືນ 0;
    s = PyUnicode_DecodeUTF8(_PyTok_SourcePointer(&tok->source, tok->start),
                             tok->cur - tok->start, NULL);
    ຖ້າ (s == NULL) {
        ຖ້າ (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->done = E_DECODE;
        }
        ບໍ່ດັ່ງນັ້ນ {
            tok->done = E_ERROR;
        }
        ສົ່ງຄືນ 0;
    }
    Py_ssize_t invalid = _PyUnicode_ScanIdentifier(s);
    assert(invalid >= 0);
    assert(PyUnicode_GET_LENGTH(s) > 0);
    ຖ້າ (invalid < PyUnicode_GET_LENGTH(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        ຖ້າ (invalid + 1 < PyUnicode_GET_LENGTH(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            ຖ້າ (s != NULL) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            ຖ້າ (s == NULL) {
                tok->done = E_ERROR;
                ສົ່ງຄືນ 0;
            }
            error_cursor = tok->start + PyBytes_GET_SIZE(s);
        }
        Py_DECREF(s);
        ຖ້າ (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        ບໍ່ດັ່ງນັ້ນ {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        ສົ່ງຄືນ 0;
    }
    Py_DECREF(s);
    ສົ່ງຄືນ 1;
}

ຈຳນວນເຕັມ
_PyLexer_get_normal(sa_nracanaa tok_state *tok, ftstring_state *current, sa_nracanaa token *token)
{
    assert(current == NULL ||
           (current->mode == FTSTRING_MODE_EXPRESSION &&
            current->replacement_depth > 0));
    ຈຳນວນເຕັມ c;
    ຈຳນວນເຕັມ blankline, nonascii;

    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
  nextline:
    tok->start = -1;
    tok->start_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    ຖ້າ (tok->layout.at_bol) {
        blankline = _PyLexer_BeginLine(tok);
        ຖ້າ (blankline < 0) {
            ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) : -1};

    ຖ້າ (tok->layout.pending != 0) {
        ສົ່ງຄືນ _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->start = -1;
    /* Skip spaces */
    do {
        c = tok_nextc(tok);
    } ໃນຂະນະທີ່ (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->start = tok->cur - 1;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    ຖ້າ (c == '#') {

        ໃນຂະນະທີ່ (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        ຖ້າ (current != NULL) {
            _PyTok_Off comment_end = tok->cur;
            ຖ້າ (c == '\n' || c == '\r') {
                comment_end--;
            }
            ຖ້າ (_PyLexer_record_ftstring_comment(
                    tok, current, tok->start, comment_end) < 0) {
                tok->done = E_NOMEM;
                ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->start;
        ຖ້າ (tok->type_comments) {
            const var_na *prefix = type_comment_prefix;
            ໃນຂະນະທີ່ (*prefix && comment < tok->cur) {
                ຖ້າ (*prefix == ' ') {
                    ໃນຂະນະທີ່ (comment < tok->cur) {
                        ຈຳນວນເຕັມ ch = _PyTok_SourceByte(&tok->source, comment);
                        ຖ້າ (ch != ' ' && ch != '\t') {
                            bha_nga;
                        }
                        comment++;
                    }
                } ບໍ່ດັ່ງນັ້ນ ຖ້າ (*prefix == _PyTok_SourceByte(&tok->source, comment)) {
                    comment++;
                } ບໍ່ດັ່ງນັ້ນ {
                    bha_nga;
                }

                prefix++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            ຖ້າ (!*prefix) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                ຈຳນວນເຕັມ is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(_PyTok_SourcePointer(&tok->source, comment),
                           "ignore", 6) == 0;
                ຖ້າ (is_type_ignore && comment + 6 < tok->cur) {
                    ຈຳນວນເຕັມ ch = _PyTok_SourceByte(&tok->source, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                ຈຳນວນເຕັມ type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_start = comment + (is_type_ignore ? 6 : 0);
                p_end = tok->cur;
                ຈຳນວນເຕັມ start_col_offset = tok->start_loc.byte_col +
                    (ຈຳນວນເຕັມ)(p_start - tok->start);
                /* Consume the newline after a standalone type ignore. */
                ຖ້າ (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                _PyLexer_token_setup(tok, token, type, p_start, p_end);
                token->start_loc = (_PyTok_Loc){tok->lineno, start_col_offset};
                token->end_loc = (_PyTok_Loc){tok->lineno,
                                              _PyLexer_ByteColumn(tok)};
                ສົ່ງຄືນ type;
            }
        }
        ຖ້າ (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.depth > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        ຖ້າ (tok->tok_extra_tokens) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_start = comment;
            p_end = tok->cur;
            tok->layout.comment_newline = blankline;
            ສົ່ງຄືນ MAKE_TOKEN(COMMENT);
        }
    }

    ຖ້າ (tok->done == E_INTERACT_STOP) {
        ສົ່ງຄືນ MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    ຖ້າ (c == EOF) {
        ຖ້າ (tok->level) {
            ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
        }
        ສົ່ງຄືນ MAKE_TOKEN(tok->done == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    ຖ້າ (is_potential_identifier_start(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        ຈຳນວນເຕັມ saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        ໃນຂະນະທີ່ (1) {
            ຖ້າ (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            ບໍ່ດັ່ງນັ້ນ ຖ້າ (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            ບໍ່ດັ່ງນັ້ນ ຖ້າ (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            ບໍ່ດັ່ງນັ້ນ ຖ້າ (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            ບໍ່ດັ່ງນັ້ນ ຖ້າ (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            ບໍ່ດັ່ງນັ້ນ {
                bha_nga;
            }
            c = tok_nextc(tok);
            ຖ້າ (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                ຈຳນວນເຕັມ status = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                ຖ້າ (status < 0) {
                    ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                ຖ້າ (saw_f || saw_t) {
                    ftstring_kind kind = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    ສົ່ງຄືນ _PyLexer_scan_fstring_start(tok, token, c, kind);
                }
                ສົ່ງຄືນ _PyLexer_scan_string(tok, token, c);
            }
        }
        ໃນຂະນະທີ່ (is_potential_identifier_char(c)) {
            ຖ້າ (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        ຖ້າ (nonascii && !verify_identifier(tok)) {
            ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
        }

        p_start = tok->start;
        p_end = tok->cur;

        ສົ່ງຄືນ MAKE_TOKEN(NAME);
    }

    ຖ້າ (c == '\r') {
        c = tok_nextc(tok);
    }

    ຖ້າ (c == '\n') {
        ຈຳນວນເຕັມ type = _PyLexer_Newline(tok, token, blankline);
        ຖ້າ (type == 0) {
            goto nextline;
        }
        ສົ່ງຄືນ type;
    }

    /* Period or number starting with period? */
    ຖ້າ (c == '.') {
        c = tok_nextc(tok);
        ຖ້າ (Py_ISDIGIT(c)) {
            ສົ່ງຄືນ _PyLexer_scan_number(tok, token, c, 1);
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == '.') {
            c = tok_nextc(tok);
            ຖ້າ (c == '.') {
                p_start = tok->start;
                p_end = tok->cur;
                ສົ່ງຄືນ MAKE_TOKEN(ELLIPSIS);
            }
            ບໍ່ດັ່ງນັ້ນ {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        ບໍ່ດັ່ງນັ້ນ {
            tok_backup(tok, c);
        }
        p_start = tok->start;
        p_end = tok->cur;
        ສົ່ງຄືນ MAKE_TOKEN(DOT);
    }


    /* Number */
    ຖ້າ (Py_ISDIGIT(c)) {
        ສົ່ງຄືນ _PyLexer_scan_number(tok, token, c, 0);
    }

    /* String */
    ຖ້າ (c == '\'' || c == '"') {
        ສົ່ງຄືນ _PyLexer_scan_string(tok, token, c);
    }

    /* Line continuation */
    ຖ້າ (c == '\\') {
        ຖ້າ ((c = _PyLexer_ContinueLine(tok)) == -1) {
            ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
        }
        goto again; /* Read next line */
    }

    /* Punctuation character */
    ຈຳນວນເຕັມ is_punctuation = (c == ':' || c == '}' || c == '!');
    ຖ້າ (is_punctuation && current != NULL) {
        ຈຳນວນເຕັມ type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        ຖ້າ (type < 0) {
            ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
        }
        ຖ້າ (type != 0) {
            p_start = tok->start;
            p_end = tok->cur;
            ສົ່ງຄືນ MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        ຈຳນວນເຕັມ c2 = tok_nextc(tok);
        ຈຳນວນເຕັມ current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        ຖ້າ (current_token != OP) {
            ຈຳນວນເຕັມ c3 = tok_nextc(tok);
            ຈຳນວນເຕັມ current_token3 = _PyToken_ThreeChars(c, c2, c3);
            ຖ້າ (current_token3 != OP) {
                current_token = current_token3;
            }
            ບໍ່ດັ່ງນັ້ນ {
                tok_backup(tok, c3);
            }
            p_start = tok->start;
            p_end = tok->cur;
            ສົ່ງຄືນ MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    switch (c) {
    pak_sa '(':
    pak_sa '[':
    pak_sa '{':
        ຖ້າ (tok->level >= MAXLEVEL) {
            ສົ່ງຄືນ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (ຈຳນວນເຕັມ)(tok->start - tok->line_start);
        tok->level++;
        bha_nga;
    pak_sa ')':
    pak_sa ']':
    pak_sa '}':
        ຖ້າ (current != NULL &&
                _PyLexer_FTStringBracketDepth(tok, current) == 0) {
            ຖ້າ (c == '}') {
                ສົ່ງຄືນ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    _PyLexer_StringPrefix(current->kind)));
            }
            ສົ່ງຄືນ MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                _PyLexer_StringPrefix(current->kind), c));
        }
        ຖ້າ (!tok->tok_extra_tokens && !tok->level) {
            ສົ່ງຄືນ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        ຖ້າ (tok->level > 0) {
            tok->level--;
            ຈຳນວນເຕັມ opening = tok->parenstack[tok->level];
            ຖ້າ (!tok->tok_extra_tokens && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                ຖ້າ (current != NULL && opening == '{') {
                    ຈຳນວນເຕັມ bracket_depth =
                        _PyLexer_FTStringBracketDepth(tok, current);
                    ຖ້າ (bracket_depth == current->replacement_depth - 1) {
                        ສົ່ງຄືນ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            _PyLexer_StringPrefix(current->kind), c));
                    }
                }
                ຖ້າ (tok->parenlinenostack[tok->level] != tok->lineno) {
                    ສົ່ງຄືນ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                ບໍ່ດັ່ງນັ້ນ {
                    ສົ່ງຄືນ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        ຖ້າ (current != NULL &&
                _PyLexer_close_ftstring_expr(tok, current, c) < 0) {
            ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
        }
        bha_nga;
    default:
        bha_nga;
    }

    ຖ້າ (!Py_UNICODE_ISPRINTABLE(c)) {
        ສົ່ງຄືນ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    ຖ້າ (c == '=' && current != NULL) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_start = tok->start;
    p_end = tok->cur;
    ສົ່ງຄືນ MAKE_TOKEN(_PyToken_OneChar(c));
}


void
_PyTokenizer_Get(sa_nracanaa tok_state *tok, sa_nracanaa token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    ຈຳນວນເຕັມ result;
    ຖ້າ (current == NULL) {
        result = _PyLexer_get_normal(tok, NULL, token);
    }
    ບໍ່ດັ່ງນັ້ນ {
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
    ຖ້າ (tok_failed(tok) && tok->done != E_INCOMPLETE) {
        result = ERRORTOKEN;
    }
    token->type = result;
}
