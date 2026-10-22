#ಅನ್ತರ್ಭಾವ "Python.h"
#ಅನ್ತರ್ಭಾವ "pycore_token.h"
#ಅನ್ತರ್ಭಾವ "pycore_unicodeobject.h"
#ಅನ್ತರ್ಭಾವ "errcode.h"

#ಅನ್ತರ್ಭಾವ "lexer_internal.h"
#ಅನ್ತರ್ಭಾವ "../tokenizer/helpers.h"
#ಅನ್ತರ್ಭಾವ "../tokenizer/reader.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

/* Spaces in this constant are treated as "zero or more spaces or tabs" when
   tokenizing. */
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* type_comment_prefix = "# type: ";

ಪೂರ್ಣಾಙ್ಕ
_PyLexer_refill(ಸಂರಚನಾ tok_state *tok)
{
    ಯದಿ (tok->done != E_OK) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪೂರ್ಣಾಙ್ಕ rc = _PyTok_ReaderUnderflow(tok);
#ಯದಿ defined(Py_DEBUG)
    ಯದಿ (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, _PyTok_SourcePointer(&tok->source, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->done);
    }
#endif
    ಯದಿ (!rc) {
        tok->cur = tok->inp;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    tok->ಸಾಲು_start = tok->cur;
    _PyTok_Span ಸಾಲು = {tok->ಸಾಲು_start, tok->inp};
    ಯದಿ (_PyTok_SourceFindByte(&tok->source, ಸಾಲು, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತ್ಯಾಗಮ 1;
}

/* Back-up one character */
ಶೂನ್ಯ
_PyLexer_backup(ಸಂರಚನಾ tok_state *tok, ಪೂರ್ಣಾಙ್ಕ c)
{
    ಯದಿ (c != EOF) {
        ಯದಿ (--tok->cur < tok->buf_offset) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        ಯದಿ (_PyTok_SourceByte(&tok->source, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
verify_identifier(ಸಂರಚನಾ tok_state *tok)
{
    ಯದಿ (tok->tok_extra_tokens) {
        ಪ್ರತ್ಯಾಗಮ 1;
    }
    PyObject *s;
    ಯದಿ (tok_failed(tok))
        ಪ್ರತ್ಯಾಗಮ 0;
    s = PyUnicode_DecodeUTF8(_PyTok_SourcePointer(&tok->source, tok->start),
                             tok->cur - tok->start, ರಿಕ್ತ);
    ಯದಿ (s == ರಿಕ್ತ) {
        ಯದಿ (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->done = E_DECODE;
        }
        ಅನ್ಯಥಾ {
            tok->done = E_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    Py_ssize_t invalid = _PyUnicode_ScanIdentifier(s);
    ಪ್ರತಿಜ್ಞಾ(invalid >= 0);
    ಪ್ರತಿಜ್ಞಾ(PyUnicode_GET_LENGTH(s) > 0);
    ಯದಿ (invalid < PyUnicode_GET_LENGTH(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        ಯದಿ (invalid + 1 < PyUnicode_GET_LENGTH(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            ಯದಿ (s != ರಿಕ್ತ) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            ಯದಿ (s == ರಿಕ್ತ) {
                tok->done = E_ERROR;
                ಪ್ರತ್ಯಾಗಮ 0;
            }
            error_cursor = tok->start + PyBytes_GET_SIZE(s);
        }
        Py_DECREF(s);
        ಯದಿ (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->ಸಾಲು_start),
                error_cursor - tok->ಸಾಲು_start, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        ಅನ್ಯಥಾ {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->ಸಾಲು_start),
                error_cursor - tok->ಸಾಲು_start, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    Py_DECREF(s);
    ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ
_PyLexer_get_normal(ಸಂರಚನಾ tok_state *tok, ftstring_state *current, ಸಂರಚನಾ token *token)
{
    ಪ್ರತಿಜ್ಞಾ(current == ರಿಕ್ತ ||
           (current->mode == FTSTRING_MODE_EXPRESSION &&
            current->replacement_depth > 0));
    ಪೂರ್ಣಾಙ್ಕ c;
    ಪೂರ್ಣಾಙ್ಕ blankline, nonascii;

    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
  nextline:
    tok->start = -1;
    tok->start_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    ಯದಿ (tok->layout.at_bol) {
        blankline = Py_Lexer_Begin_ಸಾಲು(tok);
        ಯದಿ (blankline < 0) {
            ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->ಸಾಲು_start >= 0 ? _PyLexer_ByteColumn(tok) : -1};

    ಯದಿ (tok->layout.pending != 0) {
        ಪ್ರತ್ಯಾಗಮ _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->start = -1;
    /* Skip spaces */
    ಕುರು {
        c = tok_nextc(tok);
    } ಯಾವತ್ (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->start = tok->cur - 1;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->ಸಾಲು_start >= 0 ? _PyLexer_ByteColumn(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    ಯದಿ (c == '#') {

        ಯಾವತ್ (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        ಯದಿ (current != ರಿಕ್ತ) {
            _PyTok_Off comment_end = tok->cur;
            ಯದಿ (c == '\n' || c == '\r') {
                comment_end--;
            }
            ಯದಿ (_PyLexer_record_ftstring_comment(
                    tok, current, tok->start, comment_end) < 0) {
                tok->done = E_NOMEM;
                ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->start;
        ಯದಿ (tok->type_comments) {
            ಧ್ರುವ ವರ್ಣ *prefix = type_comment_prefix;
            ಯಾವತ್ (*prefix && comment < tok->cur) {
                ಯದಿ (*prefix == ' ') {
                    ಯಾವತ್ (comment < tok->cur) {
                        ಪೂರ್ಣಾಙ್ಕ ch = _PyTok_SourceByte(&tok->source, comment);
                        ಯದಿ (ch != ' ' && ch != '\t') {
                            ಭಙ್ಗ;
                        }
                        comment++;
                    }
                } ಅನ್ಯಥಾ ಯದಿ (*prefix == _PyTok_SourceByte(&tok->source, comment)) {
                    comment++;
                } ಅನ್ಯಥಾ {
                    ಭಙ್ಗ;
                }

                prefix++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            ಯದಿ (!*prefix) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                ಪೂರ್ಣಾಙ್ಕ is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(_PyTok_SourcePointer(&tok->source, comment),
                           "ignore", 6) == 0;
                ಯದಿ (is_type_ignore && comment + 6 < tok->cur) {
                    ಪೂರ್ಣಾಙ್ಕ ch = _PyTok_SourceByte(&tok->source, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                ಪೂರ್ಣಾಙ್ಕ type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_start = comment + (is_type_ignore ? 6 : 0);
                p_end = tok->cur;
                ಪೂರ್ಣಾಙ್ಕ start_col_offset = tok->start_loc.byte_col +
                    (ಪೂರ್ಣಾಙ್ಕ)(p_start - tok->start);
                /* Consume the newline after a standalone type ignore. */
                ಯದಿ (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                _PyLexer_token_setup(tok, token, type, p_start, p_end);
                token->start_loc = (_PyTok_Loc){tok->lineno, start_col_offset};
                token->end_loc = (_PyTok_Loc){tok->lineno,
                                              _PyLexer_ByteColumn(tok)};
                ಪ್ರತ್ಯಾಗಮ type;
            }
        }
        ಯದಿ (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.depth > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        ಯದಿ (tok->tok_extra_tokens) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_start = comment;
            p_end = tok->cur;
            tok->layout.comment_newline = blankline;
            ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(COMMENT);
        }
    }

    ಯದಿ (tok->done == E_INTERACT_STOP) {
        ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    ಯದಿ (c == EOF) {
        ಯದಿ (tok->level) {
            ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
        }
        ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(tok->done == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    ಯದಿ (is_potential_identifier_start(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        ಪೂರ್ಣಾಙ್ಕ saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        ಯಾವತ್ (1) {
            ಯದಿ (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            ಅನ್ಯಥಾ ಯದಿ (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            ಅನ್ಯಥಾ ಯದಿ (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            ಅನ್ಯಥಾ ಯದಿ (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            ಅನ್ಯಥಾ ಯದಿ (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            ಅನ್ಯಥಾ {
                ಭಙ್ಗ;
            }
            c = tok_nextc(tok);
            ಯದಿ (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                ಪೂರ್ಣಾಙ್ಕ status = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                ಯದಿ (status < 0) {
                    ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                ಯದಿ (saw_f || saw_t) {
                    ftstring_kind kind = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    ಪ್ರತ್ಯಾಗಮ _PyLexer_scan_fstring_start(tok, token, c, kind);
                }
                ಪ್ರತ್ಯಾಗಮ _PyLexer_scan_string(tok, token, c);
            }
        }
        ಯಾವತ್ (is_potential_identifier_char(c)) {
            ಯದಿ (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        ಯದಿ (nonascii && !verify_identifier(tok)) {
            ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
        }

        p_start = tok->start;
        p_end = tok->cur;

        ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(NAME);
    }

    ಯದಿ (c == '\r') {
        c = tok_nextc(tok);
    }

    ಯದಿ (c == '\n') {
        ಪೂರ್ಣಾಙ್ಕ type = _PyLexer_Newline(tok, token, blankline);
        ಯದಿ (type == 0) {
            ಗಚ್ಛ nextline;
        }
        ಪ್ರತ್ಯಾಗಮ type;
    }

    /* Period or number starting with period? */
    ಯದಿ (c == '.') {
        c = tok_nextc(tok);
        ಯದಿ (Py_ISDIGIT(c)) {
            ಪ್ರತ್ಯಾಗಮ Py_Lexer_scan_ಸಂಖ್ಯೆ(tok, token, c, 1);
        } ಅನ್ಯಥಾ ಯದಿ (c == '.') {
            c = tok_nextc(tok);
            ಯದಿ (c == '.') {
                p_start = tok->start;
                p_end = tok->cur;
                ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ELLIPSIS);
            }
            ಅನ್ಯಥಾ {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        ಅನ್ಯಥಾ {
            tok_backup(tok, c);
        }
        p_start = tok->start;
        p_end = tok->cur;
        ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(DOT);
    }


    /*ಸಂಖ್ಯೆ*/
    ಯದಿ (Py_ISDIGIT(c)) {
        ಪ್ರತ್ಯಾಗಮ Py_Lexer_scan_ಸಂಖ್ಯೆ(tok, token, c, 0);
    }

    /* String */
    ಯದಿ (c == '\'' || c == '"') {
        ಪ್ರತ್ಯಾಗಮ _PyLexer_scan_string(tok, token, c);
    }

    /* Line continuation */
    ಯದಿ (c == '\\') {
        ಯದಿ ((c = Py_Lexer_Continue_ಸಾಲು(tok)) == -1) {
            ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
        }
        ಗಚ್ಛ again; /* Read next line */
    }

    /* Punctuation character */
    ಪೂರ್ಣಾಙ್ಕ is_punctuation = (c == ':' || c == '}' || c == '!');
    ಯದಿ (is_punctuation && current != ರಿಕ್ತ) {
        ಪೂರ್ಣಾಙ್ಕ type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        ಯದಿ (type < 0) {
            ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
        }
        ಯದಿ (type != 0) {
            p_start = tok->start;
            p_end = tok->cur;
            ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        ಪೂರ್ಣಾಙ್ಕ c2 = tok_nextc(tok);
        ಪೂರ್ಣಾಙ್ಕ current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        ಯದಿ (current_token != OP) {
            ಪೂರ್ಣಾಙ್ಕ c3 = tok_nextc(tok);
            ಪೂರ್ಣಾಙ್ಕ current_token3 = _PyToken_ThreeChars(c, c2, c3);
            ಯದಿ (current_token3 != OP) {
                current_token = current_token3;
            }
            ಅನ್ಯಥಾ {
                tok_backup(tok, c3);
            }
            p_start = tok->start;
            p_end = tok->cur;
            ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    ವಿಕಲ್ಪ (c) {
    ಪಕ್ಷ '(':
    ಪಕ್ಷ '[':
    ಪಕ್ಷ '{':
        ಯದಿ (tok->level >= MAXLEVEL) {
            ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (ಪೂರ್ಣಾಙ್ಕ)(tok->start - tok->ಸಾಲು_start);
        tok->level++;
        ಭಙ್ಗ;
    ಪಕ್ಷ ')':
    ಪಕ್ಷ ']':
    ಪಕ್ಷ '}':
        ಯದಿ (current != ರಿಕ್ತ &&
                _PyLexer_FTStringBracketDepth(tok, current) == 0) {
            ಯದಿ (c == '}') {
                ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    _PyLexer_StringPrefix(current->kind)));
            }
            ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                _PyLexer_StringPrefix(current->kind), c));
        }
        ಯದಿ (!tok->tok_extra_tokens && !tok->level) {
            ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        ಯದಿ (tok->level > 0) {
            tok->level--;
            ಪೂರ್ಣಾಙ್ಕ opening = tok->parenstack[tok->level];
            ಯದಿ (!tok->tok_extra_tokens && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                ಯದಿ (current != ರಿಕ್ತ && opening == '{') {
                    ಪೂರ್ಣಾಙ್ಕ bracket_depth =
                        _PyLexer_FTStringBracketDepth(tok, current);
                    ಯದಿ (bracket_depth == current->replacement_depth - 1) {
                        ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            _PyLexer_StringPrefix(current->kind), c));
                    }
                }
                ಯದಿ (tok->parenlinenostack[tok->level] != tok->lineno) {
                    ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                ಅನ್ಯಥಾ {
                    ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        ಯದಿ (current != ರಿಕ್ತ &&
                Py_Lexer_close_ftstring_ಅಭಿವ್ಯಕ್ತಿ(tok, current, c) < 0) {
            ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
        }
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        ಭಙ್ಗ;
    }

    ಯದಿ (!Py_UNICODE_ISPRINTABLE(c)) {
        ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    ಯದಿ (c == '=' && current != ರಿಕ್ತ) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_start = tok->start;
    p_end = tok->cur;
    ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(_PyToken_OneChar(c));
}


ಶೂನ್ಯ
_PyTokenizer_Get(ಸಂರಚನಾ tok_state *tok, ಸಂರಚನಾ token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    ಪೂರ್ಣಾಙ್ಕ result;
    ಯದಿ (current == ರಿಕ್ತ) {
        result = _PyLexer_get_normal(tok, ರಿಕ್ತ, token);
    }
    ಅನ್ಯಥಾ {
        ವಿಕಲ್ಪ (current->mode) {
        ಪಕ್ಷ FTSTRING_MODE_EXPRESSION:
            result = _PyLexer_get_normal(tok, current, token);
            ಭಙ್ಗ;
        ಪಕ್ಷ FTSTRING_MODE_ಮಧ್ಯ:
        ಪಕ್ಷ FTSTRING_MODE_FORMAT_SPEC:
            result = _PyLexer_get_ftstring(tok, current, token);
            ಭಙ್ಗ;
        ಸ್ವಭಾವ:
            Py_UNREACHABLE();
        }
    }
    ಯದಿ (tok_failed(tok) && tok->done != E_INCOMPLETE) {
        result = ERRORTOKEN;
    }
    token->type = result;
}
