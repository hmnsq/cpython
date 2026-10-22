#ಅನ್ತರ್ಭಾವ "Python.h"
#ಅನ್ತರ್ಭಾವ "pycore_token.h"

#ಅನ್ತರ್ಭಾವ "lexer_internal.h"
#ಅನ್ತರ್ಭಾವ "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
lookahead(ಸಂರಚನಾ tok_state *tok, ಧ್ರುವ ವರ್ಣ *test)
{
    ಧ್ರುವ ವರ್ಣ *s = test;
    ಪೂರ್ಣಾಙ್ಕ res = 0;
    ಯಾವತ್ (1) {
        ಪೂರ್ಣಾಙ್ಕ c = tok_nextc(tok);
        ಯದಿ (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        ಅನ್ಯಥಾ ಯದಿ (c == *s) {
            s++;
            ಅನುವರ್ತ;
        }

        tok_backup(tok, c);
        ಯಾವತ್ (s != test) {
            tok_backup(tok, *--s);
        }
        ಪ್ರತ್ಯಾಗಮ res;
    }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
verify_end_of_ಸಂಖ್ಯೆ(ಸಂರಚನಾ tok_state *tok, ಪೂರ್ಣಾಙ್ಕ c, ಧ್ರುವ ವರ್ಣ *kind) {
    ಯದಿ (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        ಪ್ರತ್ಯಾಗಮ 1;
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
    ಪೂರ್ಣಾಙ್ಕ r = 0;
    ಯದಿ (c == 'a') {
        r = lookahead(tok, "nd");
    }
    ಅನ್ಯಥಾ ಯದಿ (c == 'e') {
        r = lookahead(tok, "lse");
    }
    ಅನ್ಯಥಾ ಯದಿ (c == 'f') {
        r = lookahead(tok, "or");
    }
    ಅನ್ಯಥಾ ಯದಿ (c == 'i') {
        ಪೂರ್ಣಾಙ್ಕ c2 = tok_nextc(tok);
        ಯದಿ (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    ಅನ್ಯಥಾ ಯದಿ (c == 'o') {
        r = lookahead(tok, "r");
    }
    ಅನ್ಯಥಾ ಯದಿ (c == 'n') {
        r = lookahead(tok, "ot");
    }
    ಯದಿ (r) {
        tok_backup(tok, c);
        ಯದಿ (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        tok_nextc(tok);
    }
    ಅನ್ಯಥಾ /* In future releases, only error will remain. */
    ಯದಿ (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tok_decimal_tail(ಸಂರಚನಾ tok_state *tok)
{
    ಪೂರ್ಣಾಙ್ಕ c;

    ಯಾವತ್ (1) {
        ಕುರು {
            c = tok_nextc(tok);
        } ಯಾವತ್ (Py_ISDIGIT(c));
        ಯದಿ (c != '_') {
            ಭಙ್ಗ;
        }
        c = tok_nextc(tok);
        ಯದಿ (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            ಪ್ರತ್ಯಾಗಮ 0;
        }
    }
    ಪ್ರತ್ಯಾಗಮ c;
}

ಪೂರ್ಣಾಙ್ಕ
Py_Lexer_scan_ಸಂಖ್ಯೆ(ಸಂರಚನಾ tok_state *tok, ಸಂರಚನಾ token *token, ಪೂರ್ಣಾಙ್ಕ c,
                     ಪೂರ್ಣಾಙ್ಕ leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ಯದಿ (leading_dot) {
        ಗಚ್ಛ fraction;
    }
    ಯದಿ (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        ಯದಿ (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            ಕುರು {
                ಯದಿ (c == '_') {
                    c = tok_nextc(tok);
                }
                ಯದಿ (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                ಕುರು {
                    c = tok_nextc(tok);
                } ಯಾವತ್ (Py_ISXDIGIT(c));
            } ಯಾವತ್ (c == '_');
            ಯದಿ (!verify_end_of_ಸಂಖ್ಯೆ(tok, c, "hexadecimal")) {
                ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ಅನ್ಯಥಾ ಯದಿ (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            ಕುರು {
                ಯದಿ (c == '_') {
                    c = tok_nextc(tok);
                }
                ಯದಿ (c < '0' || c >= '8') {
                    ಯದಿ (Py_ISDIGIT(c)) {
                        ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    ಅನ್ಯಥಾ {
                        tok_backup(tok, c);
                        ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                ಕುರು {
                    c = tok_nextc(tok);
                } ಯಾವತ್ ('0' <= c && c < '8');
            } ಯಾವತ್ (c == '_');
            ಯದಿ (Py_ISDIGIT(c)) {
                ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            ಯದಿ (!verify_end_of_ಸಂಖ್ಯೆ(tok, c, "octal")) {
                ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ಅನ್ಯಥಾ ಯದಿ (c == 'b' || c == 'B') {
            /*ದ್ವಿಮಾನ*/
            c = tok_nextc(tok);
            ಕುರು {
                ಯದಿ (c == '_') {
                    c = tok_nextc(tok);
                }
                ಯದಿ (c != '0' && c != '1') {
                    ಯದಿ (Py_ISDIGIT(c)) {
                        ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    ಅನ್ಯಥಾ {
                        tok_backup(tok, c);
                        ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                ಕುರು {
                    c = tok_nextc(tok);
                } ಯಾವತ್ (c == '0' || c == '1');
            } ಯಾವತ್ (c == '_');
            ಯದಿ (Py_ISDIGIT(c)) {
                ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            ಯದಿ (!verify_end_of_ಸಂಖ್ಯೆ(tok, c, "binary")) {
                ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ಅನ್ಯಥಾ {
            ಪೂರ್ಣಾಙ್ಕ nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            ಯಾವತ್ (1) {
                ಯದಿ (c == '_') {
                    c = tok_nextc(tok);
                    ಯದಿ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                ಯದಿ (c != '0') {
                    ಭಙ್ಗ;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            ಯದಿ (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                ಯದಿ (c == 0) {
                    ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ಯದಿ (c == '.') {
                c = tok_nextc(tok);
                ಗಚ್ಛ fraction;
            }
            ಅನ್ಯಥಾ ಯದಿ (c == 'e' || c == 'E') {
                ಗಚ್ಛ exponent;
            }
            ಅನ್ಯಥಾ ಯದಿ (c == 'j' || c == 'J') {
                ಗಚ್ಛ imaginary;
            }
            ಅನ್ಯಥಾ ಯದಿ (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (ಪೂರ್ಣಾಙ್ಕ)(tok->start + 1 - tok->ಸಾಲು_start),
                        (ಪೂರ್ಣಾಙ್ಕ)(zeros_end - tok->ಸಾಲು_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            ಯದಿ (!verify_end_of_ಸಂಖ್ಯೆ(tok, c, "decimal")) {
                ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    ಅನ್ಯಥಾ {
        /* Decimal */
        c = tok_decimal_tail(tok);
        ಯದಿ (c == 0) {
            ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            ಯದಿ (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                ಯದಿ (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    ಯದಿ (c == 0) {
                        ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            ಯದಿ (c == 'e' || c == 'E') {
                ಪೂರ್ಣಾಙ್ಕ e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                ಯದಿ (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    ಯದಿ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } ಅನ್ಯಥಾ ಯದಿ (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    ಯದಿ (!verify_end_of_ಸಂಖ್ಯೆ(tok, e, "decimal")) {
                        ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ಸಂಖ್ಯೆ);
                }
                c = tok_decimal_tail(tok);
                ಯದಿ (c == 0) {
                    ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ಯದಿ (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                ಯದಿ (!verify_end_of_ಸಂಖ್ಯೆ(tok, c, "imaginary")) {
                    ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ಅನ್ಯಥಾ ಯದಿ (!verify_end_of_ಸಂಖ್ಯೆ(tok, c, "decimal")) {
                ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    ಪ್ರತ್ಯಾಗಮ MAKE_TOKEN(ಸಂಖ್ಯೆ);
}
