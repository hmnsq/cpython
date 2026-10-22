#ਅਨ੍ਤਰ੍ਭਾਵ "Python.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_token.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "lexer_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "../tokenizer/helpers.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAKE_TOKEN(token_type) Py_Lexer_token_ਤਿਆਰੀ(tok, token, token_type, p_ਸ਼ੁਰੂ, p_ਸਿਰਾ)

ਸਥਿਰ ਪੂਰਨ
lookahead(ਬਣਤਰ tok_state *tok, ਅਚਲ ਅੱਖਰ *test)
{
    ਅਚਲ ਅੱਖਰ *s = test;
    ਪੂਰਨ res = 0;
    ਜਦੋਂ (1) {
        ਪੂਰਨ c = tok_nextc(tok);
        ਜੇ (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        ਨਹੀਂਤਾਂ ਜੇ (c == *s) {
            s++;
            ਜਾਰੀ;
        }

        tok_backup(tok, c);
        ਜਦੋਂ (s != test) {
            tok_backup(tok, *--s);
        }
        ਵਾਪਸ res;
    }
}

ਸਥਿਰ ਪੂਰਨ
verify_ਸਿਰਾ_ਦਾ_ਸੰਖਿਆ(ਬਣਤਰ tok_state *tok, ਪੂਰਨ c, ਅਚਲ ਅੱਖਰ *ਕਿਸਮ) {
    ਜੇ (tok->tok_extra_ਟੋਕਨ) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        ਵਾਪਸ 1;
    }
    /* Emit a deprecation warning only if the numeric literal is immediately
     * followed by one of keywords which can occur after a numeric literal
     * in valid code: "and", "else", "for", "if", "in", "is" and "or".
     * It allows to gradually deprecate existing valid code without adding
     * warning before error in most cases of invalid numeric literal (which
     * would be confusing and break existing tests).
     * Raise a syntax error with slightly better message than plain
     * "invalid syntax" if the numeric literal is immediately followed by
     * other keyword or identifier.
     */
    ਪੂਰਨ r = 0;
    ਜੇ (c == 'a') {
        r = lookahead(tok, "nd");
    }
    ਨਹੀਂਤਾਂ ਜੇ (c == 'e') {
        r = lookahead(tok, "lse");
    }
    ਨਹੀਂਤਾਂ ਜੇ (c == 'f') {
        r = lookahead(tok, "or");
    }
    ਨਹੀਂਤਾਂ ਜੇ (c == 'i') {
        ਪੂਰਨ c2 = tok_nextc(tok);
        ਜੇ (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    ਨਹੀਂਤਾਂ ਜੇ (c == 'o') {
        r = lookahead(tok, "r");
    }
    ਨਹੀਂਤਾਂ ਜੇ (c == 'n') {
        r = lookahead(tok, "ot");
    }
    ਜੇ (r) {
        tok_backup(tok, c);
        ਜੇ (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", ਕਿਸਮ))
        {
            ਵਾਪਸ 0;
        }
        tok_nextc(tok);
    }
    ਨਹੀਂਤਾਂ /* In future releases, only error will remain. */
    ਜੇ (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", ਕਿਸਮ);
        ਵਾਪਸ 0;
    }
    ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ
tok_decimal_ਪੂਛ(ਬਣਤਰ tok_state *tok)
{
    ਪੂਰਨ c;

    ਜਦੋਂ (1) {
        ਕਰੋ {
            c = tok_nextc(tok);
        } ਜਦੋਂ (Py_ISDIGIT(c));
        ਜੇ (c != '_') {
            ਰੋਕੋ;
        }
        c = tok_nextc(tok);
        ਜੇ (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            ਵਾਪਸ 0;
        }
    }
    ਵਾਪਸ c;
}

ਪੂਰਨ
Py_Lexer_ਜਾਂਚ_ਸੰਖਿਆ(ਬਣਤਰ tok_state *tok, ਬਣਤਰ token *token, ਪੂਰਨ c,
                     ਪੂਰਨ leading_dot)
{
    _PyTok_Off p_ਸ਼ੁਰੂ = -1;
    _PyTok_Off p_ਸਿਰਾ = -1;

    ਜੇ (leading_dot) {
        ਜਾਓ fraction;
    }
    ਜੇ (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        ਜੇ (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            ਕਰੋ {
                ਜੇ (c == '_') {
                    c = tok_nextc(tok);
                }
                ਜੇ (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    ਵਾਪਸ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                ਕਰੋ {
                    c = tok_nextc(tok);
                } ਜਦੋਂ (Py_ISXDIGIT(c));
            } ਜਦੋਂ (c == '_');
            ਜੇ (!verify_ਸਿਰਾ_ਦਾ_ਸੰਖਿਆ(tok, c, "hexadecimal")) {
                ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ਨਹੀਂਤਾਂ ਜੇ (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            ਕਰੋ {
                ਜੇ (c == '_') {
                    c = tok_nextc(tok);
                }
                ਜੇ (c < '0' || c >= '8') {
                    ਜੇ (Py_ISDIGIT(c)) {
                        ਵਾਪਸ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    ਨਹੀਂਤਾਂ {
                        tok_backup(tok, c);
                        ਵਾਪਸ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                ਕਰੋ {
                    c = tok_nextc(tok);
                } ਜਦੋਂ ('0' <= c && c < '8');
            } ਜਦੋਂ (c == '_');
            ਜੇ (Py_ISDIGIT(c)) {
                ਵਾਪਸ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            ਜੇ (!verify_ਸਿਰਾ_ਦਾ_ਸੰਖਿਆ(tok, c, "octal")) {
                ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ਨਹੀਂਤਾਂ ਜੇ (c == 'b' || c == 'B') {
            /*ਦੁਆਧਾਰੀ*/
            c = tok_nextc(tok);
            ਕਰੋ {
                ਜੇ (c == '_') {
                    c = tok_nextc(tok);
                }
                ਜੇ (c != '0' && c != '1') {
                    ਜੇ (Py_ISDIGIT(c)) {
                        ਵਾਪਸ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    ਨਹੀਂਤਾਂ {
                        tok_backup(tok, c);
                        ਵਾਪਸ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                ਕਰੋ {
                    c = tok_nextc(tok);
                } ਜਦੋਂ (c == '0' || c == '1');
            } ਜਦੋਂ (c == '_');
            ਜੇ (Py_ISDIGIT(c)) {
                ਵਾਪਸ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            ਜੇ (!verify_ਸਿਰਾ_ਦਾ_ਸੰਖਿਆ(tok, c, "binary")) {
                ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ਨਹੀਂਤਾਂ {
            ਪੂਰਨ nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            ਜਦੋਂ (1) {
                ਜੇ (c == '_') {
                    c = tok_nextc(tok);
                    ਜੇ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ਵਾਪਸ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                ਜੇ (c != '0') {
                    ਰੋਕੋ;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_ਸਿਰਾ = tok->cur;
            ਜੇ (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_ਪੂਛ(tok);
                ਜੇ (c == 0) {
                    ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ਜੇ (c == '.') {
                c = tok_nextc(tok);
                ਜਾਓ fraction;
            }
            ਨਹੀਂਤਾਂ ਜੇ (c == 'e' || c == 'E') {
                ਜਾਓ exponent;
            }
            ਨਹੀਂਤਾਂ ਜੇ (c == 'j' || c == 'J') {
                ਜਾਓ imaginary;
            }
            ਨਹੀਂਤਾਂ ਜੇ (nonzero && !tok->tok_extra_ਟੋਕਨ) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                ਵਾਪਸ MAKE_TOKEN(Py_Tokenizer_syntaxerror_ਗਿਆਤ_range(
                        tok, (ਪੂਰਨ)(tok->ਸ਼ੁਰੂ + 1 - tok->ਸਤਰ_ਸ਼ੁਰੂ),
                        (ਪੂਰਨ)(zeros_ਸਿਰਾ - tok->ਸਤਰ_ਸ਼ੁਰੂ),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            ਜੇ (!verify_ਸਿਰਾ_ਦਾ_ਸੰਖਿਆ(tok, c, "decimal")) {
                ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    ਨਹੀਂਤਾਂ {
        /* Decimal */
        c = tok_decimal_ਪੂਛ(tok);
        ਜੇ (c == 0) {
            ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            ਜੇ (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                ਜੇ (Py_ISDIGIT(c)) {
                    c = tok_decimal_ਪੂਛ(tok);
                    ਜੇ (c == 0) {
                        ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            ਜੇ (c == 'e' || c == 'E') {
                ਪੂਰਨ e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                ਜੇ (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    ਜੇ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ਵਾਪਸ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } ਨਹੀਂਤਾਂ ਜੇ (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    ਜੇ (!verify_ਸਿਰਾ_ਦਾ_ਸੰਖਿਆ(tok, e, "decimal")) {
                        ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_ਸ਼ੁਰੂ = tok->ਸ਼ੁਰੂ;
                    p_ਸਿਰਾ = tok->cur;
                    ਵਾਪਸ MAKE_TOKEN(ਸੰਖਿਆ);
                }
                c = tok_decimal_ਪੂਛ(tok);
                ਜੇ (c == 0) {
                    ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ਜੇ (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                ਜੇ (!verify_ਸਿਰਾ_ਦਾ_ਸੰਖਿਆ(tok, c, "imaginary")) {
                    ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ਨਹੀਂਤਾਂ ਜੇ (!verify_ਸਿਰਾ_ਦਾ_ਸੰਖਿਆ(tok, c, "decimal")) {
                ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_ਸ਼ੁਰੂ = tok->ਸ਼ੁਰੂ;
    p_ਸਿਰਾ = tok->cur;
    ਵਾਪਸ MAKE_TOKEN(ਸੰਖਿਆ);
}
