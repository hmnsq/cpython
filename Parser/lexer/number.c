#अन्तर्भाव "Python.h"
#अन्तर्भाव "pycore_token.h"

#अन्तर्भाव "lexer_internal.h"
#अन्तर्भाव "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

स्थिर पूर्णाङ्क
lookahead(संरचना tok_state *tok, ध्रुव वर्ण *test)
{
    ध्रुव वर्ण *s = test;
    पूर्णाङ्क res = 0;
    जाव (1) {
        पूर्णाङ्क c = tok_nextc(tok);
        जइ (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        अण्णहा जइ (c == *s) {
            s++;
            अनुवर्त;
        }

        tok_backup(tok, c);
        जाव (s != test) {
            tok_backup(tok, *--s);
        }
        प्रत्यागम res;
    }
}

स्थिर पूर्णाङ्क
verify_end_of_सङ्ख्या(संरचना tok_state *tok, पूर्णाङ्क c, ध्रुव वर्ण *kind) {
    जइ (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        प्रत्यागम 1;
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
    पूर्णाङ्क r = 0;
    जइ (c == 'a') {
        r = lookahead(tok, "nd");
    }
    अण्णहा जइ (c == 'e') {
        r = lookahead(tok, "lse");
    }
    अण्णहा जइ (c == 'f') {
        r = lookahead(tok, "or");
    }
    अण्णहा जइ (c == 'i') {
        पूर्णाङ्क c2 = tok_nextc(tok);
        जइ (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    अण्णहा जइ (c == 'o') {
        r = lookahead(tok, "r");
    }
    अण्णहा जइ (c == 'n') {
        r = lookahead(tok, "ot");
    }
    जइ (r) {
        tok_backup(tok, c);
        जइ (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            प्रत्यागम 0;
        }
        tok_nextc(tok);
    }
    अण्णहा /* In future releases, only error will remain. */
    जइ (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        प्रत्यागम 0;
    }
    प्रत्यागम 1;
}

स्थिर पूर्णाङ्क
tok_decimal_tail(संरचना tok_state *tok)
{
    पूर्णाङ्क c;

    जाव (1) {
        करेहि {
            c = tok_nextc(tok);
        } जाव (Py_ISDIGIT(c));
        जइ (c != '_') {
            भङ्ग;
        }
        c = tok_nextc(tok);
        जइ (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            प्रत्यागम 0;
        }
    }
    प्रत्यागम c;
}

पूर्णाङ्क
Py_Lexer_scan_सङ्ख्या(संरचना tok_state *tok, संरचना token *token, पूर्णाङ्क c,
                     पूर्णाङ्क leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    जइ (leading_dot) {
        गच्छ fraction;
    }
    जइ (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        जइ (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            करेहि {
                जइ (c == '_') {
                    c = tok_nextc(tok);
                }
                जइ (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    प्रत्यागम MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                करेहि {
                    c = tok_nextc(tok);
                } जाव (Py_ISXDIGIT(c));
            } जाव (c == '_');
            जइ (!verify_end_of_सङ्ख्या(tok, c, "hexadecimal")) {
                प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
            }
        }
        अण्णहा जइ (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            करेहि {
                जइ (c == '_') {
                    c = tok_nextc(tok);
                }
                जइ (c < '0' || c >= '8') {
                    जइ (Py_ISDIGIT(c)) {
                        प्रत्यागम MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    अण्णहा {
                        tok_backup(tok, c);
                        प्रत्यागम MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                करेहि {
                    c = tok_nextc(tok);
                } जाव ('0' <= c && c < '8');
            } जाव (c == '_');
            जइ (Py_ISDIGIT(c)) {
                प्रत्यागम MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            जइ (!verify_end_of_सङ्ख्या(tok, c, "octal")) {
                प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
            }
        }
        अण्णहा जइ (c == 'b' || c == 'B') {
            /*द्विमान*/
            c = tok_nextc(tok);
            करेहि {
                जइ (c == '_') {
                    c = tok_nextc(tok);
                }
                जइ (c != '0' && c != '1') {
                    जइ (Py_ISDIGIT(c)) {
                        प्रत्यागम MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    अण्णहा {
                        tok_backup(tok, c);
                        प्रत्यागम MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                करेहि {
                    c = tok_nextc(tok);
                } जाव (c == '0' || c == '1');
            } जाव (c == '_');
            जइ (Py_ISDIGIT(c)) {
                प्रत्यागम MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            जइ (!verify_end_of_सङ्ख्या(tok, c, "binary")) {
                प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
            }
        }
        अण्णहा {
            पूर्णाङ्क nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            जाव (1) {
                जइ (c == '_') {
                    c = tok_nextc(tok);
                    जइ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        प्रत्यागम MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                जइ (c != '0') {
                    भङ्ग;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            जइ (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                जइ (c == 0) {
                    प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
                }
            }
            जइ (c == '.') {
                c = tok_nextc(tok);
                गच्छ fraction;
            }
            अण्णहा जइ (c == 'e' || c == 'E') {
                गच्छ exponent;
            }
            अण्णहा जइ (c == 'j' || c == 'J') {
                गच्छ imaginary;
            }
            अण्णहा जइ (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                प्रत्यागम MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (पूर्णाङ्क)(tok->start + 1 - tok->पङ्क्तिः_start),
                        (पूर्णाङ्क)(zeros_end - tok->पङ्क्तिः_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            जइ (!verify_end_of_सङ्ख्या(tok, c, "decimal")) {
                प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    अण्णहा {
        /* Decimal */
        c = tok_decimal_tail(tok);
        जइ (c == 0) {
            प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            जइ (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                जइ (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    जइ (c == 0) {
                        प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            जइ (c == 'e' || c == 'E') {
                पूर्णाङ्क e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                जइ (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    जइ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        प्रत्यागम MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } अण्णहा जइ (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    जइ (!verify_end_of_सङ्ख्या(tok, e, "decimal")) {
                        प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    प्रत्यागम MAKE_TOKEN(सङ्ख्या);
                }
                c = tok_decimal_tail(tok);
                जइ (c == 0) {
                    प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
                }
            }
            जइ (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                जइ (!verify_end_of_सङ्ख्या(tok, c, "imaginary")) {
                    प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
                }
            }
            अण्णहा जइ (!verify_end_of_सङ्ख्या(tok, c, "decimal")) {
                प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    प्रत्यागम MAKE_TOKEN(सङ्ख्या);
}
