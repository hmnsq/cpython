#ಅನ್ತರ್ಭಾವ "Python.h"
#ಅನ್ತರ್ಭಾವ "pycore_token.h"

#ಅನ್ತರ್ಭಾವ "lexer_internal.h"
#ಅನ್ತರ್ಭಾವ "../tokenizer/helpers.h"

#ವ್ಯಾಖ್ಯಾನಿಸು MAKE_TOKEN(token_type) Py_Lexer_token_ಸಿದ್ಧತೆ(tok, token, token_type, p_ಆರಂಭ, p_ಕೊನೆ)

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lookahead(ರಚನೆ tok_state *tok, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *test)
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = test;
    ಪೂರ್ಣಾಂಕ res = 0;
    ಆಗ (1) {
        ಪೂರ್ಣಾಂಕ c = tok_nextc(tok);
        ಆದರೆ (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == *s) {
            s++;
            ಮುಂದುವರಿ;
        }

        tok_backup(tok, c);
        ಆಗ (s != test) {
            tok_backup(tok, *--s);
        }
        ಹಿಂತಿರುಗಿ res;
    }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
verify_ಕೊನೆ_ದ_ಸಂಖ್ಯೆ(ರಚನೆ tok_state *tok, ಪೂರ್ಣಾಂಕ c, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಗೆ) {
    ಆದರೆ (tok->tok_extra_ಟೋಕನುಗಳು) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        ಹಿಂತಿರುಗಿ 1;
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
    ಪೂರ್ಣಾಂಕ r = 0;
    ಆದರೆ (c == 'a') {
        r = lookahead(tok, "nd");
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 'e') {
        r = lookahead(tok, "lse");
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 'f') {
        r = lookahead(tok, "or");
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 'i') {
        ಪೂರ್ಣಾಂಕ c2 = tok_nextc(tok);
        ಆದರೆ (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 'o') {
        r = lookahead(tok, "r");
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 'n') {
        r = lookahead(tok, "ot");
    }
    ಆದರೆ (r) {
        tok_backup(tok, c);
        ಆದರೆ (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", ಬಗೆ))
        {
            ಹಿಂತಿರುಗಿ 0;
        }
        tok_nextc(tok);
    }
    ಇಲ್ಲದಿದ್ದರೆ /* In future releases, only error will remain. */
    ಆದರೆ (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", ಬಗೆ);
        ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
tok_decimal_ಬಾಲ(ರಚನೆ tok_state *tok)
{
    ಪೂರ್ಣಾಂಕ c;

    ಆಗ (1) {
        ಮಾಡು {
            c = tok_nextc(tok);
        } ಆಗ (Py_ISDIGIT(c));
        ಆದರೆ (c != '_') {
            ನಿಲ್ಲಿಸು;
        }
        c = tok_nextc(tok);
        ಆದರೆ (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            ಹಿಂತಿರುಗಿ 0;
        }
    }
    ಹಿಂತಿರುಗಿ c;
}

ಪೂರ್ಣಾಂಕ
Py_Lexer_ಪರಿಶೀಲನೆ_ಸಂಖ್ಯೆ(ರಚನೆ tok_state *tok, ರಚನೆ token *token, ಪೂರ್ಣಾಂಕ c,
                     ಪೂರ್ಣಾಂಕ leading_dot)
{
    _PyTok_Off p_ಆರಂಭ = -1;
    _PyTok_Off p_ಕೊನೆ = -1;

    ಆದರೆ (leading_dot) {
        ಹೋಗು fraction;
    }
    ಆದರೆ (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        ಆದರೆ (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            ಮಾಡು {
                ಆದರೆ (c == '_') {
                    c = tok_nextc(tok);
                }
                ಆದರೆ (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    ಹಿಂತಿರುಗಿ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                ಮಾಡು {
                    c = tok_nextc(tok);
                } ಆಗ (Py_ISXDIGIT(c));
            } ಆಗ (c == '_');
            ಆದರೆ (!verify_ಕೊನೆ_ದ_ಸಂಖ್ಯೆ(tok, c, "hexadecimal")) {
                ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            ಮಾಡು {
                ಆದರೆ (c == '_') {
                    c = tok_nextc(tok);
                }
                ಆದರೆ (c < '0' || c >= '8') {
                    ಆದರೆ (Py_ISDIGIT(c)) {
                        ಹಿಂತಿರುಗಿ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    ಇಲ್ಲದಿದ್ದರೆ {
                        tok_backup(tok, c);
                        ಹಿಂತಿರುಗಿ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                ಮಾಡು {
                    c = tok_nextc(tok);
                } ಆಗ ('0' <= c && c < '8');
            } ಆಗ (c == '_');
            ಆದರೆ (Py_ISDIGIT(c)) {
                ಹಿಂತಿರುಗಿ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            ಆದರೆ (!verify_ಕೊನೆ_ದ_ಸಂಖ್ಯೆ(tok, c, "octal")) {
                ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 'b' || c == 'B') {
            /*ದ್ವಿಮಾನ*/
            c = tok_nextc(tok);
            ಮಾಡು {
                ಆದರೆ (c == '_') {
                    c = tok_nextc(tok);
                }
                ಆದರೆ (c != '0' && c != '1') {
                    ಆದರೆ (Py_ISDIGIT(c)) {
                        ಹಿಂತಿರುಗಿ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    ಇಲ್ಲದಿದ್ದರೆ {
                        tok_backup(tok, c);
                        ಹಿಂತಿರುಗಿ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                ಮಾಡು {
                    c = tok_nextc(tok);
                } ಆಗ (c == '0' || c == '1');
            } ಆಗ (c == '_');
            ಆದರೆ (Py_ISDIGIT(c)) {
                ಹಿಂತಿರುಗಿ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            ಆದರೆ (!verify_ಕೊನೆ_ದ_ಸಂಖ್ಯೆ(tok, c, "binary")) {
                ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            ಪೂರ್ಣಾಂಕ nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            ಆಗ (1) {
                ಆದರೆ (c == '_') {
                    c = tok_nextc(tok);
                    ಆದರೆ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ಹಿಂತಿರುಗಿ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                ಆದರೆ (c != '0') {
                    ನಿಲ್ಲಿಸು;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_ಕೊನೆ = tok->cur;
            ಆದರೆ (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_ಬಾಲ(tok);
                ಆದರೆ (c == 0) {
                    ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ಆದರೆ (c == '.') {
                c = tok_nextc(tok);
                ಹೋಗು fraction;
            }
            ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 'e' || c == 'E') {
                ಹೋಗು exponent;
            }
            ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 'j' || c == 'J') {
                ಹೋಗು imaginary;
            }
            ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nonzero && !tok->tok_extra_ಟೋಕನುಗಳು) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                ಹಿಂತಿರುಗಿ MAKE_TOKEN(Py_Tokenizer_syntaxerror_ತಿಳಿದ_range(
                        tok, (ಪೂರ್ಣಾಂಕ)(tok->ಆರಂಭ + 1 - tok->ಸಾಲು_ಆರಂಭ),
                        (ಪೂರ್ಣಾಂಕ)(zeros_ಕೊನೆ - tok->ಸಾಲು_ಆರಂಭ),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            ಆದರೆ (!verify_ಕೊನೆ_ದ_ಸಂಖ್ಯೆ(tok, c, "decimal")) {
                ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        /* Decimal */
        c = tok_decimal_ಬಾಲ(tok);
        ಆದರೆ (c == 0) {
            ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            ಆದರೆ (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                ಆದರೆ (Py_ISDIGIT(c)) {
                    c = tok_decimal_ಬಾಲ(tok);
                    ಆದರೆ (c == 0) {
                        ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            ಆದರೆ (c == 'e' || c == 'E') {
                ಪೂರ್ಣಾಂಕ e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                ಆದರೆ (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    ಆದರೆ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ಹಿಂತಿರುಗಿ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    ಆದರೆ (!verify_ಕೊನೆ_ದ_ಸಂಖ್ಯೆ(tok, e, "decimal")) {
                        ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_ಆರಂಭ = tok->ಆರಂಭ;
                    p_ಕೊನೆ = tok->cur;
                    ಹಿಂತಿರುಗಿ MAKE_TOKEN(ಸಂಖ್ಯೆ);
                }
                c = tok_decimal_ಬಾಲ(tok);
                ಆದರೆ (c == 0) {
                    ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ಆದರೆ (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                ಆದರೆ (!verify_ಕೊನೆ_ದ_ಸಂಖ್ಯೆ(tok, c, "imaginary")) {
                    ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!verify_ಕೊನೆ_ದ_ಸಂಖ್ಯೆ(tok, c, "decimal")) {
                ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_ಆರಂಭ = tok->ಆರಂಭ;
    p_ಕೊನೆ = tok->cur;
    ಹಿಂತಿರುಗಿ MAKE_TOKEN(ಸಂಖ್ಯೆ);
}
