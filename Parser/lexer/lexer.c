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

පූර්ණ_සංඛ්‍යා
_PyLexer_refill(sa_nracanaa tok_state *tok)
{
    නම් (tok->done != E_OK) {
        ආපසු 0;
    }
    පූර්ණ_සංඛ්‍යා rc = _PyTok_ReaderUnderflow(tok);
#නම් defined(Py_DEBUG)
    නම් (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, _PyTok_SourcePointer(&tok->source, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->done);
    }
#endif
    නම් (!rc) {
        tok->cur = tok->inp;
        ආපසු 0;
    }
    tok->line_start = tok->cur;
    _PyTok_Span line = {tok->line_start, tok->inp};
    නම් (_PyTok_SourceFindByte(&tok->source, line, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        ආපසු 0;
    }
    ආපසු 1;
}

/* Back-up one character */
void
_PyLexer_backup(sa_nracanaa tok_state *tok, පූර්ණ_සංඛ්‍යා c)
{
    නම් (c != EOF) {
        නම් (--tok->cur < tok->buf_offset) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        නම් (_PyTok_SourceByte(&tok->source, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
static පූර්ණ_සංඛ්‍යා
verify_identifier(sa_nracanaa tok_state *tok)
{
    නම් (tok->tok_extra_tokens) {
        ආපසු 1;
    }
    PyObject *s;
    නම් (tok_failed(tok))
        ආපසු 0;
    s = PyUnicode_DecodeUTF8(_PyTok_SourcePointer(&tok->source, tok->start),
                             tok->cur - tok->start, NULL);
    නම් (s == NULL) {
        නම් (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->done = E_DECODE;
        }
        නැතහොත් {
            tok->done = E_ERROR;
        }
        ආපසු 0;
    }
    Py_ssize_t invalid = _PyUnicode_ScanIdentifier(s);
    assert(invalid >= 0);
    assert(PyUnicode_GET_LENGTH(s) > 0);
    නම් (invalid < PyUnicode_GET_LENGTH(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        නම් (invalid + 1 < PyUnicode_GET_LENGTH(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            නම් (s != NULL) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            නම් (s == NULL) {
                tok->done = E_ERROR;
                ආපසු 0;
            }
            error_cursor = tok->start + PyBytes_GET_SIZE(s);
        }
        Py_DECREF(s);
        නම් (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        නැතහොත් {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
                error_cursor - tok->line_start, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        ආපසු 0;
    }
    Py_DECREF(s);
    ආපසු 1;
}

පූර්ණ_සංඛ්‍යා
_PyLexer_get_normal(sa_nracanaa tok_state *tok, ftstring_state *current, sa_nracanaa token *token)
{
    assert(current == NULL ||
           (current->mode == FTSTRING_MODE_EXPRESSION &&
            current->replacement_depth > 0));
    පූර්ණ_සංඛ්‍යා c;
    පූර්ණ_සංඛ්‍යා blankline, nonascii;

    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
  nextline:
    tok->start = -1;
    tok->start_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    නම් (tok->layout.at_bol) {
        blankline = _PyLexer_BeginLine(tok);
        නම් (blankline < 0) {
            ආපසු MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) : -1};

    නම් (tok->layout.pending != 0) {
        ආපසු _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->start = -1;
    /* Skip spaces */
    do {
        c = tok_nextc(tok);
    } අතරතුර (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->start = tok->cur - 1;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->line_start >= 0 ? _PyLexer_ByteColumn(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    නම් (c == '#') {

        අතරතුර (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        නම් (current != NULL) {
            _PyTok_Off comment_end = tok->cur;
            නම් (c == '\n' || c == '\r') {
                comment_end--;
            }
            නම් (_PyLexer_record_ftstring_comment(
                    tok, current, tok->start, comment_end) < 0) {
                tok->done = E_NOMEM;
                ආපසු MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->start;
        නම් (tok->type_comments) {
            const var_na *prefix = type_comment_prefix;
            අතරතුර (*prefix && comment < tok->cur) {
                නම් (*prefix == ' ') {
                    අතරතුර (comment < tok->cur) {
                        පූර්ණ_සංඛ්‍යා ch = _PyTok_SourceByte(&tok->source, comment);
                        නම් (ch != ' ' && ch != '\t') {
                            bha_nga;
                        }
                        comment++;
                    }
                } නැතහොත් නම් (*prefix == _PyTok_SourceByte(&tok->source, comment)) {
                    comment++;
                } නැතහොත් {
                    bha_nga;
                }

                prefix++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            නම් (!*prefix) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                පූර්ණ_සංඛ්‍යා is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(_PyTok_SourcePointer(&tok->source, comment),
                           "ignore", 6) == 0;
                නම් (is_type_ignore && comment + 6 < tok->cur) {
                    පූර්ණ_සංඛ්‍යා ch = _PyTok_SourceByte(&tok->source, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                පූර්ණ_සංඛ්‍යා type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_start = comment + (is_type_ignore ? 6 : 0);
                p_end = tok->cur;
                පූර්ණ_සංඛ්‍යා start_col_offset = tok->start_loc.byte_col +
                    (පූර්ණ_සංඛ්‍යා)(p_start - tok->start);
                /* Consume the newline after a standalone type ignore. */
                නම් (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                _PyLexer_token_setup(tok, token, type, p_start, p_end);
                token->start_loc = (_PyTok_Loc){tok->lineno, start_col_offset};
                token->end_loc = (_PyTok_Loc){tok->lineno,
                                              _PyLexer_ByteColumn(tok)};
                ආපසු type;
            }
        }
        නම් (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.depth > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        නම් (tok->tok_extra_tokens) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_start = comment;
            p_end = tok->cur;
            tok->layout.comment_newline = blankline;
            ආපසු MAKE_TOKEN(COMMENT);
        }
    }

    නම් (tok->done == E_INTERACT_STOP) {
        ආපසු MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    නම් (c == EOF) {
        නම් (tok->level) {
            ආපසු MAKE_TOKEN(ERRORTOKEN);
        }
        ආපසු MAKE_TOKEN(tok->done == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    නම් (is_potential_identifier_start(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        පූර්ණ_සංඛ්‍යා saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        අතරතුර (1) {
            නම් (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            නැතහොත් නම් (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            නැතහොත් නම් (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            නැතහොත් නම් (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            නැතහොත් නම් (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            නැතහොත් {
                bha_nga;
            }
            c = tok_nextc(tok);
            නම් (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                පූර්ණ_සංඛ්‍යා status = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                නම් (status < 0) {
                    ආපසු MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                නම් (saw_f || saw_t) {
                    ftstring_kind kind = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    ආපසු _PyLexer_scan_fstring_start(tok, token, c, kind);
                }
                ආපසු _PyLexer_scan_string(tok, token, c);
            }
        }
        අතරතුර (is_potential_identifier_char(c)) {
            නම් (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        නම් (nonascii && !verify_identifier(tok)) {
            ආපසු MAKE_TOKEN(ERRORTOKEN);
        }

        p_start = tok->start;
        p_end = tok->cur;

        ආපසු MAKE_TOKEN(NAME);
    }

    නම් (c == '\r') {
        c = tok_nextc(tok);
    }

    නම් (c == '\n') {
        පූර්ණ_සංඛ්‍යා type = _PyLexer_Newline(tok, token, blankline);
        නම් (type == 0) {
            goto nextline;
        }
        ආපසු type;
    }

    /* Period or number starting with period? */
    නම් (c == '.') {
        c = tok_nextc(tok);
        නම් (Py_ISDIGIT(c)) {
            ආපසු _PyLexer_scan_number(tok, token, c, 1);
        } නැතහොත් නම් (c == '.') {
            c = tok_nextc(tok);
            නම් (c == '.') {
                p_start = tok->start;
                p_end = tok->cur;
                ආපසු MAKE_TOKEN(ELLIPSIS);
            }
            නැතහොත් {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        නැතහොත් {
            tok_backup(tok, c);
        }
        p_start = tok->start;
        p_end = tok->cur;
        ආපසු MAKE_TOKEN(DOT);
    }


    /* Number */
    නම් (Py_ISDIGIT(c)) {
        ආපසු _PyLexer_scan_number(tok, token, c, 0);
    }

    /* String */
    නම් (c == '\'' || c == '"') {
        ආපසු _PyLexer_scan_string(tok, token, c);
    }

    /* Line continuation */
    නම් (c == '\\') {
        නම් ((c = _PyLexer_ContinueLine(tok)) == -1) {
            ආපසු MAKE_TOKEN(ERRORTOKEN);
        }
        goto again; /* Read next line */
    }

    /* Punctuation character */
    පූර්ණ_සංඛ්‍යා is_punctuation = (c == ':' || c == '}' || c == '!');
    නම් (is_punctuation && current != NULL) {
        පූර්ණ_සංඛ්‍යා type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        නම් (type < 0) {
            ආපසු MAKE_TOKEN(ERRORTOKEN);
        }
        නම් (type != 0) {
            p_start = tok->start;
            p_end = tok->cur;
            ආපසු MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        පූර්ණ_සංඛ්‍යා c2 = tok_nextc(tok);
        පූර්ණ_සංඛ්‍යා current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        නම් (current_token != OP) {
            පූර්ණ_සංඛ්‍යා c3 = tok_nextc(tok);
            පූර්ණ_සංඛ්‍යා current_token3 = _PyToken_ThreeChars(c, c2, c3);
            නම් (current_token3 != OP) {
                current_token = current_token3;
            }
            නැතහොත් {
                tok_backup(tok, c3);
            }
            p_start = tok->start;
            p_end = tok->cur;
            ආපසු MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    switch (c) {
    pak_sa '(':
    pak_sa '[':
    pak_sa '{':
        නම් (tok->level >= MAXLEVEL) {
            ආපසු MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (පූර්ණ_සංඛ්‍යා)(tok->start - tok->line_start);
        tok->level++;
        bha_nga;
    pak_sa ')':
    pak_sa ']':
    pak_sa '}':
        නම් (current != NULL &&
                _PyLexer_FTStringBracketDepth(tok, current) == 0) {
            නම් (c == '}') {
                ආපසු MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    _PyLexer_StringPrefix(current->kind)));
            }
            ආපසු MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                _PyLexer_StringPrefix(current->kind), c));
        }
        නම් (!tok->tok_extra_tokens && !tok->level) {
            ආපසු MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        නම් (tok->level > 0) {
            tok->level--;
            පූර්ණ_සංඛ්‍යා opening = tok->parenstack[tok->level];
            නම් (!tok->tok_extra_tokens && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                නම් (current != NULL && opening == '{') {
                    පූර්ණ_සංඛ්‍යා bracket_depth =
                        _PyLexer_FTStringBracketDepth(tok, current);
                    නම් (bracket_depth == current->replacement_depth - 1) {
                        ආපසු MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            _PyLexer_StringPrefix(current->kind), c));
                    }
                }
                නම් (tok->parenlinenostack[tok->level] != tok->lineno) {
                    ආපසු MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                නැතහොත් {
                    ආපසු MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        නම් (current != NULL &&
                _PyLexer_close_ftstring_expr(tok, current, c) < 0) {
            ආපසු MAKE_TOKEN(ERRORTOKEN);
        }
        bha_nga;
    default:
        bha_nga;
    }

    නම් (!Py_UNICODE_ISPRINTABLE(c)) {
        ආපසු MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    නම් (c == '=' && current != NULL) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_start = tok->start;
    p_end = tok->cur;
    ආපසු MAKE_TOKEN(_PyToken_OneChar(c));
}


void
_PyTokenizer_Get(sa_nracanaa tok_state *tok, sa_nracanaa token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    පූර්ණ_සංඛ්‍යා result;
    නම් (current == NULL) {
        result = _PyLexer_get_normal(tok, NULL, token);
    }
    නැතහොත් {
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
    නම් (tok_failed(tok) && tok->done != E_INCOMPLETE) {
        result = ERRORTOKEN;
    }
    token->type = result;
}
