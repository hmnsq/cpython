#ਅਨ੍ਤਰ੍ਭਾਵ "Python.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_token.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_unicodeobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errcode.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "lexer_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "../tokenizer/helpers.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "../tokenizer/reader.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAKE_TOKEN(token_type) Py_Lexer_token_ਤਿਆਰੀ(tok, token, token_type, p_ਸ਼ੁਰੂ, p_ਸਿਰਾ)

/* Spaces in this constant are treated as "zero or more spaces or tabs" when
   tokenizing. */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ* type_comment_ਅਗੇਤਰ = "# type: ";

ਪੂਰਨ
_PyLexer_refill(ਬਣਤਰ tok_state *tok)
{
    ਜੇ (tok->ਪੂਰਾ != E_OK) {
        ਵਾਪਸ 0;
    }
    ਪੂਰਨ rc = _PyTok_ReaderUnderflow(tok);
#ਜੇ defined(Py_DEBUG)
    ਜੇ (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, Py_Tok_ਸਰੋਤ_ਸੂਚਕ(&tok->ਸਰੋਤ, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->ਪੂਰਾ);
    }
#ਜੇ_ਅੰਤ
    ਜੇ (!rc) {
        tok->cur = tok->inp;
        ਵਾਪਸ 0;
    }
    tok->ਸਤਰ_ਸ਼ੁਰੂ = tok->cur;
    Py_Tok_ਵਿਸਤਾਰ ਸਤਰ = {tok->ਸਤਰ_ਸ਼ੁਰੂ, tok->inp};
    ਜੇ (Py_Tok_ਸਰੋਤ_ਲੱਭੋ_Byte(&tok->ਸਰੋਤ, ਸਤਰ, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        ਵਾਪਸ 0;
    }
    ਵਾਪਸ 1;
}

/* Back-up one character */
ਖਾਲੀ
_PyLexer_backup(ਬਣਤਰ tok_state *tok, ਪੂਰਨ c)
{
    ਜੇ (c != EOF) {
        ਜੇ (--tok->cur < tok->buf_ਵਿੱਥ) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        ਜੇ (Py_Tok_ਸਰੋਤ_Byte(&tok->ਸਰੋਤ, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
ਸਥਿਰ ਪੂਰਨ
verify_identifier(ਬਣਤਰ tok_state *tok)
{
    ਜੇ (tok->tok_extra_ਟੋਕਨ) {
        ਵਾਪਸ 1;
    }
    PyObject *s;
    ਜੇ (tok_failed(tok))
        ਵਾਪਸ 0;
    s = PyUnicode_DecodeUTF8(Py_Tok_ਸਰੋਤ_ਸੂਚਕ(&tok->ਸਰੋਤ, tok->ਸ਼ੁਰੂ),
                             tok->cur - tok->ਸ਼ੁਰੂ, ਨਿਰਾ);
    ਜੇ (s == ਨਿਰਾ) {
        ਜੇ (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->ਪੂਰਾ = E_DECODE;
        }
        ਨਹੀਂਤਾਂ {
            tok->ਪੂਰਾ = E_ERROR;
        }
        ਵਾਪਸ 0;
    }
    Py_ssize_t invalid = Py_Unicode_ਜਾਂਚ_Identifier(s);
    ਪ੍ਰਤਿਜ੍ਞਾ(invalid >= 0);
    ਪ੍ਰਤਿਜ੍ਞਾ(Py_Unicode_GET_ਲੰਮਾਈ(s) > 0);
    ਜੇ (invalid < Py_Unicode_GET_ਲੰਮਾਈ(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        ਜੇ (invalid + 1 < Py_Unicode_GET_ਲੰਮਾਈ(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            ਜੇ (s != ਨਿਰਾ) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            ਜੇ (s == ਨਿਰਾ) {
                tok->ਪੂਰਾ = E_ERROR;
                ਵਾਪਸ 0;
            }
            error_cursor = tok->ਸ਼ੁਰੂ + Py_ਬਾਈਟਾਂ_GET_ਮਾਪ(s);
        }
        Py_DECREF(s);
        ਜੇ (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, Py_Tok_ਸਰੋਤ_ਸੂਚਕ(&tok->ਸਰੋਤ, tok->ਸਤਰ_ਸ਼ੁਰੂ),
                error_cursor - tok->ਸਤਰ_ਸ਼ੁਰੂ, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        ਨਹੀਂਤਾਂ {
            _PyTokenizer_syntaxerror_at(
                tok, Py_Tok_ਸਰੋਤ_ਸੂਚਕ(&tok->ਸਰੋਤ, tok->ਸਤਰ_ਸ਼ੁਰੂ),
                error_cursor - tok->ਸਤਰ_ਸ਼ੁਰੂ, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        ਵਾਪਸ 0;
    }
    Py_DECREF(s);
    ਵਾਪਸ 1;
}

ਪੂਰਨ
Py_Lexer_get_ਸਾਧਾਰਨ(ਬਣਤਰ tok_state *tok, ftstring_state *current, ਬਣਤਰ token *token)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(current == ਨਿਰਾ ||
           (current->ਬਹੁਲਕ == FTSTRING_ਬਹੁਲਕ_ਵਿਅੰਜਕ &&
            current->replacement_ਡੂੰਘਾਈ > 0));
    ਪੂਰਨ c;
    ਪੂਰਨ blankline, nonascii;

    _PyTok_Off p_ਸ਼ੁਰੂ = -1;
    _PyTok_Off p_ਸਿਰਾ = -1;
  nextline:
    tok->ਸ਼ੁਰੂ = -1;
    tok->ਸ਼ੁਰੂ_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    ਜੇ (tok->layout.at_bol) {
        blankline = Py_Lexer_Begin_ਸਤਰ(tok);
        ਜੇ (blankline < 0) {
            ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->ਸ਼ੁਰੂ = tok->cur;
    tok->ਸ਼ੁਰੂ_loc = (_PyTok_Loc){
        tok->lineno, tok->ਸਤਰ_ਸ਼ੁਰੂ >= 0 ? Py_Lexer_Byte_ਕਾਲਮ(tok) : -1};

    ਜੇ (tok->layout.pending != 0) {
        ਵਾਪਸ _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->ਸ਼ੁਰੂ = -1;
    /* Skip spaces */
    ਕਰੋ {
        c = tok_nextc(tok);
    } ਜਦੋਂ (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->ਸ਼ੁਰੂ = tok->cur - 1;
    tok->ਸ਼ੁਰੂ_loc = (_PyTok_Loc){
        tok->lineno, tok->ਸਤਰ_ਸ਼ੁਰੂ >= 0 ? Py_Lexer_Byte_ਕਾਲਮ(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    ਜੇ (c == '#') {

        ਜਦੋਂ (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        ਜੇ (current != ਨਿਰਾ) {
            _PyTok_Off comment_ਸਿਰਾ = tok->cur;
            ਜੇ (c == '\n' || c == '\r') {
                comment_ਸਿਰਾ--;
            }
            ਜੇ (Py_Lexer_ਰਿਕਾਰਡ_ftstring_comment(
                    tok, current, tok->ਸ਼ੁਰੂ, comment_ਸਿਰਾ) < 0) {
                tok->ਪੂਰਾ = E_NOMEM;
                ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->ਸ਼ੁਰੂ;
        ਜੇ (tok->type_comments) {
            ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ = type_comment_ਅਗੇਤਰ;
            ਜਦੋਂ (*ਅਗੇਤਰ && comment < tok->cur) {
                ਜੇ (*ਅਗੇਤਰ == ' ') {
                    ਜਦੋਂ (comment < tok->cur) {
                        ਪੂਰਨ ch = Py_Tok_ਸਰੋਤ_Byte(&tok->ਸਰੋਤ, comment);
                        ਜੇ (ch != ' ' && ch != '\t') {
                            ਰੋਕੋ;
                        }
                        comment++;
                    }
                } ਨਹੀਂਤਾਂ ਜੇ (*ਅਗੇਤਰ == Py_Tok_ਸਰੋਤ_Byte(&tok->ਸਰੋਤ, comment)) {
                    comment++;
                } ਨਹੀਂਤਾਂ {
                    ਰੋਕੋ;
                }

                ਅਗੇਤਰ++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            ਜੇ (!*ਅਗੇਤਰ) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                ਪੂਰਨ is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(Py_Tok_ਸਰੋਤ_ਸੂਚਕ(&tok->ਸਰੋਤ, comment),
                           "ignore", 6) == 0;
                ਜੇ (is_type_ignore && comment + 6 < tok->cur) {
                    ਪੂਰਨ ch = Py_Tok_ਸਰੋਤ_Byte(&tok->ਸਰੋਤ, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                ਪੂਰਨ type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_ਸ਼ੁਰੂ = comment + (is_type_ignore ? 6 : 0);
                p_ਸਿਰਾ = tok->cur;
                ਪੂਰਨ ਸ਼ੁਰੂ_ਖਾਨਾ_ਵਿੱਥ = tok->ਸ਼ੁਰੂ_loc.byte_ਖਾਨਾ +
                    (ਪੂਰਨ)(p_ਸ਼ੁਰੂ - tok->ਸ਼ੁਰੂ);
                /* Consume the newline after a standalone type ignore. */
                ਜੇ (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                Py_Lexer_token_ਤਿਆਰੀ(tok, token, type, p_ਸ਼ੁਰੂ, p_ਸਿਰਾ);
                token->ਸ਼ੁਰੂ_loc = (_PyTok_Loc){tok->lineno, ਸ਼ੁਰੂ_ਖਾਨਾ_ਵਿੱਥ};
                token->ਸਿਰਾ_loc = (_PyTok_Loc){tok->lineno,
                                              Py_Lexer_Byte_ਕਾਲਮ(tok)};
                ਵਾਪਸ type;
            }
        }
        ਜੇ (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.ਡੂੰਘਾਈ > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        ਜੇ (tok->tok_extra_ਟੋਕਨ) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_ਸ਼ੁਰੂ = comment;
            p_ਸਿਰਾ = tok->cur;
            tok->layout.comment_newline = blankline;
            ਵਾਪਸ MAKE_TOKEN(COMMENT);
        }
    }

    ਜੇ (tok->ਪੂਰਾ == E_INTERACT_STOP) {
        ਵਾਪਸ MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    ਜੇ (c == EOF) {
        ਜੇ (tok->level) {
            ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
        }
        ਵਾਪਸ MAKE_TOKEN(tok->ਪੂਰਾ == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    ਜੇ (is_potential_identifier_ਸ਼ੁਰੂ(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        ਪੂਰਨ saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        ਜਦੋਂ (1) {
            ਜੇ (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            ਨਹੀਂਤਾਂ ਜੇ (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            ਨਹੀਂਤਾਂ ਜੇ (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            ਨਹੀਂਤਾਂ ਜੇ (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            ਨਹੀਂਤਾਂ ਜੇ (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            ਨਹੀਂਤਾਂ {
                ਰੋਕੋ;
            }
            c = tok_nextc(tok);
            ਜੇ (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                ਪੂਰਨ ਸਥਿਤੀ = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                ਜੇ (ਸਥਿਤੀ < 0) {
                    ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                ਜੇ (saw_f || saw_t) {
                    ftstring_ਕਿਸਮ ਕਿਸਮ = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    ਵਾਪਸ Py_Lexer_ਜਾਂਚ_fstring_ਸ਼ੁਰੂ(tok, token, c, ਕਿਸਮ);
                }
                ਵਾਪਸ Py_Lexer_ਜਾਂਚ_string(tok, token, c);
            }
        }
        ਜਦੋਂ (is_potential_identifier_char(c)) {
            ਜੇ (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        ਜੇ (nonascii && !verify_identifier(tok)) {
            ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
        }

        p_ਸ਼ੁਰੂ = tok->ਸ਼ੁਰੂ;
        p_ਸਿਰਾ = tok->cur;

        ਵਾਪਸ MAKE_TOKEN(NAME);
    }

    ਜੇ (c == '\r') {
        c = tok_nextc(tok);
    }

    ਜੇ (c == '\n') {
        ਪੂਰਨ type = _PyLexer_Newline(tok, token, blankline);
        ਜੇ (type == 0) {
            ਜਾਓ nextline;
        }
        ਵਾਪਸ type;
    }

    /* Period or number starting with period? */
    ਜੇ (c == '.') {
        c = tok_nextc(tok);
        ਜੇ (Py_ISDIGIT(c)) {
            ਵਾਪਸ Py_Lexer_ਜਾਂਚ_ਸੰਖਿਆ(tok, token, c, 1);
        } ਨਹੀਂਤਾਂ ਜੇ (c == '.') {
            c = tok_nextc(tok);
            ਜੇ (c == '.') {
                p_ਸ਼ੁਰੂ = tok->ਸ਼ੁਰੂ;
                p_ਸਿਰਾ = tok->cur;
                ਵਾਪਸ MAKE_TOKEN(ELLIPSIS);
            }
            ਨਹੀਂਤਾਂ {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        ਨਹੀਂਤਾਂ {
            tok_backup(tok, c);
        }
        p_ਸ਼ੁਰੂ = tok->ਸ਼ੁਰੂ;
        p_ਸਿਰਾ = tok->cur;
        ਵਾਪਸ MAKE_TOKEN(DOT);
    }


    /*ਸੰਖਿਆ*/
    ਜੇ (Py_ISDIGIT(c)) {
        ਵਾਪਸ Py_Lexer_ਜਾਂਚ_ਸੰਖਿਆ(tok, token, c, 0);
    }

    /* String */
    ਜੇ (c == '\'' || c == '"') {
        ਵਾਪਸ Py_Lexer_ਜਾਂਚ_string(tok, token, c);
    }

    /* Line continuation */
    ਜੇ (c == '\\') {
        ਜੇ ((c = Py_Lexer_Continue_ਸਤਰ(tok)) == -1) {
            ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
        }
        ਜਾਓ again; /* Read next line */
    }

    /* Punctuation character */
    ਪੂਰਨ is_punctuation = (c == ':' || c == '}' || c == '!');
    ਜੇ (is_punctuation && current != ਨਿਰਾ) {
        ਪੂਰਨ type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        ਜੇ (type < 0) {
            ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
        }
        ਜੇ (type != 0) {
            p_ਸ਼ੁਰੂ = tok->ਸ਼ੁਰੂ;
            p_ਸਿਰਾ = tok->cur;
            ਵਾਪਸ MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        ਪੂਰਨ c2 = tok_nextc(tok);
        ਪੂਰਨ current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        ਜੇ (current_token != OP) {
            ਪੂਰਨ c3 = tok_nextc(tok);
            ਪੂਰਨ current_token3 = _PyToken_ThreeChars(c, c2, c3);
            ਜੇ (current_token3 != OP) {
                current_token = current_token3;
            }
            ਨਹੀਂਤਾਂ {
                tok_backup(tok, c3);
            }
            p_ਸ਼ੁਰੂ = tok->ਸ਼ੁਰੂ;
            p_ਸਿਰਾ = tok->cur;
            ਵਾਪਸ MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    ਚੋਣ (c) {
    ਹਾਲਤ '(':
    ਹਾਲਤ '[':
    ਹਾਲਤ '{':
        ਜੇ (tok->level >= MAXLEVEL) {
            ਵਾਪਸ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (ਪੂਰਨ)(tok->ਸ਼ੁਰੂ - tok->ਸਤਰ_ਸ਼ੁਰੂ);
        tok->level++;
        ਰੋਕੋ;
    ਹਾਲਤ ')':
    ਹਾਲਤ ']':
    ਹਾਲਤ '}':
        ਜੇ (current != ਨਿਰਾ &&
                Py_Lexer_FTString_Bracket_ਡੂੰਘਾਈ(tok, current) == 0) {
            ਜੇ (c == '}') {
                ਵਾਪਸ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    Py_Lexer_String_ਅਗੇਤਰ(current->ਕਿਸਮ)));
            }
            ਵਾਪਸ MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                Py_Lexer_String_ਅਗੇਤਰ(current->ਕਿਸਮ), c));
        }
        ਜੇ (!tok->tok_extra_ਟੋਕਨ && !tok->level) {
            ਵਾਪਸ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        ਜੇ (tok->level > 0) {
            tok->level--;
            ਪੂਰਨ opening = tok->parenstack[tok->level];
            ਜੇ (!tok->tok_extra_ਟੋਕਨ && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                ਜੇ (current != ਨਿਰਾ && opening == '{') {
                    ਪੂਰਨ bracket_ਡੂੰਘਾਈ =
                        Py_Lexer_FTString_Bracket_ਡੂੰਘਾਈ(tok, current);
                    ਜੇ (bracket_ਡੂੰਘਾਈ == current->replacement_ਡੂੰਘਾਈ - 1) {
                        ਵਾਪਸ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            Py_Lexer_String_ਅਗੇਤਰ(current->ਕਿਸਮ), c));
                    }
                }
                ਜੇ (tok->parenlinenostack[tok->level] != tok->lineno) {
                    ਵਾਪਸ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                ਨਹੀਂਤਾਂ {
                    ਵਾਪਸ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        ਜੇ (current != ਨਿਰਾ &&
                Py_Lexer_close_ftstring_ਵਿਅੰਜਕ(tok, current, c) < 0) {
            ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
        }
        ਰੋਕੋ;
    ਮੂਲ:
        ਰੋਕੋ;
    }

    ਜੇ (!Py_UNICODE_ISPRINTABLE(c)) {
        ਵਾਪਸ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    ਜੇ (c == '=' && current != ਨਿਰਾ) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_ਸ਼ੁਰੂ = tok->ਸ਼ੁਰੂ;
    p_ਸਿਰਾ = tok->cur;
    ਵਾਪਸ MAKE_TOKEN(_PyToken_OneChar(c));
}


ਖਾਲੀ
_PyTokenizer_Get(ਬਣਤਰ tok_state *tok, ਬਣਤਰ token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    ਪੂਰਨ ਨਤੀਜਾ;
    ਜੇ (current == ਨਿਰਾ) {
        ਨਤੀਜਾ = Py_Lexer_get_ਸਾਧਾਰਨ(tok, ਨਿਰਾ, token);
    }
    ਨਹੀਂਤਾਂ {
        ਚੋਣ (current->ਬਹੁਲਕ) {
        ਹਾਲਤ FTSTRING_ਬਹੁਲਕ_ਵਿਅੰਜਕ:
            ਨਤੀਜਾ = Py_Lexer_get_ਸਾਧਾਰਨ(tok, current, token);
            ਰੋਕੋ;
        ਹਾਲਤ FTSTRING_ਬਹੁਲਕ_ਵਿਚਕਾਰ:
        ਹਾਲਤ FTSTRING_ਬਹੁਲਕ_FORMAT_SPEC:
            ਨਤੀਜਾ = _PyLexer_get_ftstring(tok, current, token);
            ਰੋਕੋ;
        ਮੂਲ:
            Py_UNREACHABLE();
        }
    }
    ਜੇ (tok_failed(tok) && tok->ਪੂਰਾ != E_INCOMPLETE) {
        ਨਤੀਜਾ = ERRORTOKEN;
    }
    token->type = ਨਤੀਜਾ;
}
