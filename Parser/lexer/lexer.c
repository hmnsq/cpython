#ಅನ್ತರ್ಭಾವ "Python.h"
#ಅನ್ತರ್ಭಾವ "pycore_token.h"
#ಅನ್ತರ್ಭಾವ "pycore_unicodeobject.h"
#ಅನ್ತರ್ಭಾವ "errcode.h"

#ಅನ್ತರ್ಭಾವ "lexer_internal.h"
#ಅನ್ತರ್ಭಾವ "../tokenizer/helpers.h"
#ಅನ್ತರ್ಭಾವ "../tokenizer/reader.h"

#ವ್ಯಾಖ್ಯಾನಿಸು MAKE_TOKEN(token_type) Py_Lexer_token_ಸಿದ್ಧತೆ(tok, token, token_type, p_ಆರಂಭ, p_ಕೊನೆ)

/* Spaces in this constant are treated as "zero or more spaces or tabs" when
   tokenizing. */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* type_comment_ಪೂರ್ವಪ್ರತ್ಯಯ = "# type: ";

ಪೂರ್ಣಾಂಕ
_PyLexer_refill(ರಚನೆ tok_state *tok)
{
    ಆದರೆ (tok->ಮುಗಿದ != E_OK) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಪೂರ್ಣಾಂಕ rc = _PyTok_ReaderUnderflow(tok);
#ಆದರೆ defined(Py_DEBUG)
    ಆದರೆ (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, Py_Tok_ಮೂಲ_ಸೂಚಕ(&tok->ಮೂಲ, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->ಮುಗಿದ);
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (!rc) {
        tok->cur = tok->inp;
        ಹಿಂತಿರುಗಿ 0;
    }
    tok->ಸಾಲು_ಆರಂಭ = tok->cur;
    Py_Tok_ಹರವು ಸಾಲು = {tok->ಸಾಲು_ಆರಂಭ, tok->inp};
    ಆದರೆ (Py_Tok_ಮೂಲ_ಕಂಡುಹಿಡಿ_Byte(&tok->ಮೂಲ, ಸಾಲು, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ 1;
}

/* Back-up one character */
ಶೂನ್ಯ
_PyLexer_backup(ರಚನೆ tok_state *tok, ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (c != EOF) {
        ಆದರೆ (--tok->cur < tok->buf_ಸ್ಥಾನಾಂತರ) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        ಆದರೆ (Py_Tok_ಮೂಲ_Byte(&tok->ಮೂಲ, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
verify_identifier(ರಚನೆ tok_state *tok)
{
    ಆದರೆ (tok->tok_extra_ಟೋಕನುಗಳು) {
        ಹಿಂತಿರುಗಿ 1;
    }
    PyObject *s;
    ಆದರೆ (tok_failed(tok))
        ಹಿಂತಿರುಗಿ 0;
    s = PyUnicode_DecodeUTF8(Py_Tok_ಮೂಲ_ಸೂಚಕ(&tok->ಮೂಲ, tok->ಆರಂಭ),
                             tok->cur - tok->ಆರಂಭ, ಖಾಲಿ);
    ಆದರೆ (s == ಖಾಲಿ) {
        ಆದರೆ (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->ಮುಗಿದ = E_DECODE;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            tok->ಮುಗಿದ = E_ERROR;
        }
        ಹಿಂತಿರುಗಿ 0;
    }
    Py_ssize_t invalid = Py_Unicode_ಪರಿಶೀಲನೆ_Identifier(s);
    ಪ್ರತಿಜ್ಞಾ(invalid >= 0);
    ಪ್ರತಿಜ್ಞಾ(Py_Unicode_GET_ಉದ್ದಳತೆ(s) > 0);
    ಆದರೆ (invalid < Py_Unicode_GET_ಉದ್ದಳತೆ(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        ಆದರೆ (invalid + 1 < Py_Unicode_GET_ಉದ್ದಳತೆ(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            ಆದರೆ (s != ಖಾಲಿ) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            ಆದರೆ (s == ಖಾಲಿ) {
                tok->ಮುಗಿದ = E_ERROR;
                ಹಿಂತಿರುಗಿ 0;
            }
            error_cursor = tok->ಆರಂಭ + Py_ಬೈಟುಗಳು_GET_ಗಾತ್ರ(s);
        }
        Py_DECREF(s);
        ಆದರೆ (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, Py_Tok_ಮೂಲ_ಸೂಚಕ(&tok->ಮೂಲ, tok->ಸಾಲು_ಆರಂಭ),
                error_cursor - tok->ಸಾಲು_ಆರಂಭ, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            _PyTokenizer_syntaxerror_at(
                tok, Py_Tok_ಮೂಲ_ಸೂಚಕ(&tok->ಮೂಲ, tok->ಸಾಲು_ಆರಂಭ),
                error_cursor - tok->ಸಾಲು_ಆರಂಭ, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        ಹಿಂತಿರುಗಿ 0;
    }
    Py_DECREF(s);
    ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ
Py_Lexer_get_ಸಾಮಾನ್ಯ(ರಚನೆ tok_state *tok, ftstring_state *current, ರಚನೆ token *token)
{
    ಪ್ರತಿಜ್ಞಾ(current == ಖಾಲಿ ||
           (current->ಬಹುಲಕ == FTSTRING_ಬಹುಲಕ_ಅಭಿವ್ಯಕ್ತಿ &&
            current->replacement_ಆಳ > 0));
    ಪೂರ್ಣಾಂಕ c;
    ಪೂರ್ಣಾಂಕ blankline, nonascii;

    _PyTok_Off p_ಆರಂಭ = -1;
    _PyTok_Off p_ಕೊನೆ = -1;
  nextline:
    tok->ಆರಂಭ = -1;
    tok->ಆರಂಭ_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    ಆದರೆ (tok->layout.at_bol) {
        blankline = Py_Lexer_Begin_ಸಾಲು(tok);
        ಆದರೆ (blankline < 0) {
            ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->ಆರಂಭ = tok->cur;
    tok->ಆರಂಭ_loc = (_PyTok_Loc){
        tok->lineno, tok->ಸಾಲು_ಆರಂಭ >= 0 ? Py_Lexer_Byte_ಸ್ತಂಭ(tok) : -1};

    ಆದರೆ (tok->layout.pending != 0) {
        ಹಿಂತಿರುಗಿ _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->ಆರಂಭ = -1;
    /* Skip spaces */
    ಮಾಡು {
        c = tok_nextc(tok);
    } ಆಗ (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->ಆರಂಭ = tok->cur - 1;
    tok->ಆರಂಭ_loc = (_PyTok_Loc){
        tok->lineno, tok->ಸಾಲು_ಆರಂಭ >= 0 ? Py_Lexer_Byte_ಸ್ತಂಭ(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    ಆದರೆ (c == '#') {

        ಆಗ (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        ಆದರೆ (current != ಖಾಲಿ) {
            _PyTok_Off comment_ಕೊನೆ = tok->cur;
            ಆದರೆ (c == '\n' || c == '\r') {
                comment_ಕೊನೆ--;
            }
            ಆದರೆ (Py_Lexer_ದಾಖಲೆ_ftstring_comment(
                    tok, current, tok->ಆರಂಭ, comment_ಕೊನೆ) < 0) {
                tok->ಮುಗಿದ = E_NOMEM;
                ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->ಆರಂಭ;
        ಆದರೆ (tok->type_comments) {
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ = type_comment_ಪೂರ್ವಪ್ರತ್ಯಯ;
            ಆಗ (*ಪೂರ್ವಪ್ರತ್ಯಯ && comment < tok->cur) {
                ಆದರೆ (*ಪೂರ್ವಪ್ರತ್ಯಯ == ' ') {
                    ಆಗ (comment < tok->cur) {
                        ಪೂರ್ಣಾಂಕ ch = Py_Tok_ಮೂಲ_Byte(&tok->ಮೂಲ, comment);
                        ಆದರೆ (ch != ' ' && ch != '\t') {
                            ನಿಲ್ಲಿಸು;
                        }
                        comment++;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ಪೂರ್ವಪ್ರತ್ಯಯ == Py_Tok_ಮೂಲ_Byte(&tok->ಮೂಲ, comment)) {
                    comment++;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ನಿಲ್ಲಿಸು;
                }

                ಪೂರ್ವಪ್ರತ್ಯಯ++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            ಆದರೆ (!*ಪೂರ್ವಪ್ರತ್ಯಯ) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                ಪೂರ್ಣಾಂಕ is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(Py_Tok_ಮೂಲ_ಸೂಚಕ(&tok->ಮೂಲ, comment),
                           "ignore", 6) == 0;
                ಆದರೆ (is_type_ignore && comment + 6 < tok->cur) {
                    ಪೂರ್ಣಾಂಕ ch = Py_Tok_ಮೂಲ_Byte(&tok->ಮೂಲ, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                ಪೂರ್ಣಾಂಕ type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_ಆರಂಭ = comment + (is_type_ignore ? 6 : 0);
                p_ಕೊನೆ = tok->cur;
                ಪೂರ್ಣಾಂಕ ಆರಂಭ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = tok->ಆರಂಭ_loc.byte_ಲಂಬಸಾಲು +
                    (ಪೂರ್ಣಾಂಕ)(p_ಆರಂಭ - tok->ಆರಂಭ);
                /* Consume the newline after a standalone type ignore. */
                ಆದರೆ (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                Py_Lexer_token_ಸಿದ್ಧತೆ(tok, token, type, p_ಆರಂಭ, p_ಕೊನೆ);
                token->ಆರಂಭ_loc = (_PyTok_Loc){tok->lineno, ಆರಂಭ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ};
                token->ಕೊನೆ_loc = (_PyTok_Loc){tok->lineno,
                                              Py_Lexer_Byte_ಸ್ತಂಭ(tok)};
                ಹಿಂತಿರುಗಿ type;
            }
        }
        ಆದರೆ (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.ಆಳ > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        ಆದರೆ (tok->tok_extra_ಟೋಕನುಗಳು) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_ಆರಂಭ = comment;
            p_ಕೊನೆ = tok->cur;
            tok->layout.comment_newline = blankline;
            ಹಿಂತಿರುಗಿ MAKE_TOKEN(COMMENT);
        }
    }

    ಆದರೆ (tok->ಮುಗಿದ == E_INTERACT_STOP) {
        ಹಿಂತಿರುಗಿ MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    ಆದರೆ (c == EOF) {
        ಆದರೆ (tok->level) {
            ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
        }
        ಹಿಂತಿರುಗಿ MAKE_TOKEN(tok->ಮುಗಿದ == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    ಆದರೆ (is_potential_identifier_ಆರಂಭ(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        ಪೂರ್ಣಾಂಕ saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        ಆಗ (1) {
            ಆದರೆ (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                ನಿಲ್ಲಿಸು;
            }
            c = tok_nextc(tok);
            ಆದರೆ (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2 = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                ಆದರೆ (ಸ್ಥಿತಿ_2 < 0) {
                    ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                ಆದರೆ (saw_f || saw_t) {
                    ftstring_ಬಗೆ ಬಗೆ = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    ಹಿಂತಿರುಗಿ Py_Lexer_ಪರಿಶೀಲನೆ_fstring_ಆರಂಭ(tok, token, c, ಬಗೆ);
                }
                ಹಿಂತಿರುಗಿ Py_Lexer_ಪರಿಶೀಲನೆ_string(tok, token, c);
            }
        }
        ಆಗ (is_potential_identifier_char(c)) {
            ಆದರೆ (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        ಆದರೆ (nonascii && !verify_identifier(tok)) {
            ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
        }

        p_ಆರಂಭ = tok->ಆರಂಭ;
        p_ಕೊನೆ = tok->cur;

        ಹಿಂತಿರುಗಿ MAKE_TOKEN(NAME);
    }

    ಆದರೆ (c == '\r') {
        c = tok_nextc(tok);
    }

    ಆದರೆ (c == '\n') {
        ಪೂರ್ಣಾಂಕ type = _PyLexer_Newline(tok, token, blankline);
        ಆದರೆ (type == 0) {
            ಹೋಗು nextline;
        }
        ಹಿಂತಿರುಗಿ type;
    }

    /* Period or number starting with period? */
    ಆದರೆ (c == '.') {
        c = tok_nextc(tok);
        ಆದರೆ (Py_ISDIGIT(c)) {
            ಹಿಂತಿರುಗಿ Py_Lexer_ಪರಿಶೀಲನೆ_ಸಂಖ್ಯೆ(tok, token, c, 1);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '.') {
            c = tok_nextc(tok);
            ಆದರೆ (c == '.') {
                p_ಆರಂಭ = tok->ಆರಂಭ;
                p_ಕೊನೆ = tok->cur;
                ಹಿಂತಿರುಗಿ MAKE_TOKEN(ELLIPSIS);
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            tok_backup(tok, c);
        }
        p_ಆರಂಭ = tok->ಆರಂಭ;
        p_ಕೊನೆ = tok->cur;
        ಹಿಂತಿರುಗಿ MAKE_TOKEN(DOT);
    }


    /*ಸಂಖ್ಯೆ*/
    ಆದರೆ (Py_ISDIGIT(c)) {
        ಹಿಂತಿರುಗಿ Py_Lexer_ಪರಿಶೀಲನೆ_ಸಂಖ್ಯೆ(tok, token, c, 0);
    }

    /* String */
    ಆದರೆ (c == '\'' || c == '"') {
        ಹಿಂತಿರುಗಿ Py_Lexer_ಪರಿಶೀಲನೆ_string(tok, token, c);
    }

    /* Line continuation */
    ಆದರೆ (c == '\\') {
        ಆದರೆ ((c = Py_Lexer_Continue_ಸಾಲು(tok)) == -1) {
            ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
        }
        ಹೋಗು again; /* Read next line */
    }

    /* Punctuation character */
    ಪೂರ್ಣಾಂಕ is_punctuation = (c == ':' || c == '}' || c == '!');
    ಆದರೆ (is_punctuation && current != ಖಾಲಿ) {
        ಪೂರ್ಣಾಂಕ type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        ಆದರೆ (type < 0) {
            ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
        }
        ಆದರೆ (type != 0) {
            p_ಆರಂಭ = tok->ಆರಂಭ;
            p_ಕೊನೆ = tok->cur;
            ಹಿಂತಿರುಗಿ MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        ಪೂರ್ಣಾಂಕ c2 = tok_nextc(tok);
        ಪೂರ್ಣಾಂಕ current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        ಆದರೆ (current_token != OP) {
            ಪೂರ್ಣಾಂಕ c3 = tok_nextc(tok);
            ಪೂರ್ಣಾಂಕ current_token3 = _PyToken_ThreeChars(c, c2, c3);
            ಆದರೆ (current_token3 != OP) {
                current_token = current_token3;
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                tok_backup(tok, c3);
            }
            p_ಆರಂಭ = tok->ಆರಂಭ;
            p_ಕೊನೆ = tok->cur;
            ಹಿಂತಿರುಗಿ MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    ಆಯ್ಕೆ (c) {
    ಸ್ಥಿತಿ '(':
    ಸ್ಥಿತಿ '[':
    ಸ್ಥಿತಿ '{':
        ಆದರೆ (tok->level >= MAXLEVEL) {
            ಹಿಂತಿರುಗಿ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (ಪೂರ್ಣಾಂಕ)(tok->ಆರಂಭ - tok->ಸಾಲು_ಆರಂಭ);
        tok->level++;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ ')':
    ಸ್ಥಿತಿ ']':
    ಸ್ಥಿತಿ '}':
        ಆದರೆ (current != ಖಾಲಿ &&
                Py_Lexer_FTString_Bracket_ಆಳ(tok, current) == 0) {
            ಆದರೆ (c == '}') {
                ಹಿಂತಿರುಗಿ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    Py_Lexer_String_ಪೂರ್ವಪ್ರತ್ಯಯ(current->ಬಗೆ)));
            }
            ಹಿಂತಿರುಗಿ MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                Py_Lexer_String_ಪೂರ್ವಪ್ರತ್ಯಯ(current->ಬಗೆ), c));
        }
        ಆದರೆ (!tok->tok_extra_ಟೋಕನುಗಳು && !tok->level) {
            ಹಿಂತಿರುಗಿ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        ಆದರೆ (tok->level > 0) {
            tok->level--;
            ಪೂರ್ಣಾಂಕ opening = tok->parenstack[tok->level];
            ಆದರೆ (!tok->tok_extra_ಟೋಕನುಗಳು && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                ಆದರೆ (current != ಖಾಲಿ && opening == '{') {
                    ಪೂರ್ಣಾಂಕ bracket_ಆಳ =
                        Py_Lexer_FTString_Bracket_ಆಳ(tok, current);
                    ಆದರೆ (bracket_ಆಳ == current->replacement_ಆಳ - 1) {
                        ಹಿಂತಿರುಗಿ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            Py_Lexer_String_ಪೂರ್ವಪ್ರತ್ಯಯ(current->ಬಗೆ), c));
                    }
                }
                ಆದರೆ (tok->parenlinenostack[tok->level] != tok->lineno) {
                    ಹಿಂತಿರುಗಿ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                ಇಲ್ಲದಿದ್ದರೆ {
                    ಹಿಂತಿರುಗಿ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        ಆದರೆ (current != ಖಾಲಿ &&
                Py_Lexer_close_ftstring_ಅಭಿವ್ಯಕ್ತಿ(tok, current, c) < 0) {
            ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
        }
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (!Py_UNICODE_ISPRINTABLE(c)) {
        ಹಿಂತಿರುಗಿ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    ಆದರೆ (c == '=' && current != ಖಾಲಿ) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_ಆರಂಭ = tok->ಆರಂಭ;
    p_ಕೊನೆ = tok->cur;
    ಹಿಂತಿರುಗಿ MAKE_TOKEN(_PyToken_OneChar(c));
}


ಶೂನ್ಯ
_PyTokenizer_Get(ರಚನೆ tok_state *tok, ರಚನೆ token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ;
    ಆದರೆ (current == ಖಾಲಿ) {
        ಫಲಿತಾಂಶ = Py_Lexer_get_ಸಾಮಾನ್ಯ(tok, ಖಾಲಿ, token);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        ಆಯ್ಕೆ (current->ಬಹುಲಕ) {
        ಸ್ಥಿತಿ FTSTRING_ಬಹುಲಕ_ಅಭಿವ್ಯಕ್ತಿ:
            ಫಲಿತಾಂಶ = Py_Lexer_get_ಸಾಮಾನ್ಯ(tok, current, token);
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ FTSTRING_ಬಹುಲಕ_ಮಧ್ಯ:
        ಸ್ಥಿತಿ FTSTRING_ಬಹುಲಕ_FORMAT_SPEC:
            ಫಲಿತಾಂಶ = _PyLexer_get_ftstring(tok, current, token);
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            Py_UNREACHABLE();
        }
    }
    ಆದರೆ (tok_failed(tok) && tok->ಮುಗಿದ != E_INCOMPLETE) {
        ಫಲಿತಾಂಶ = ERRORTOKEN;
    }
    token->type = ಫಲಿತಾಂಶ;
}
