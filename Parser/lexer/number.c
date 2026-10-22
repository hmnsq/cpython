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
    याव (1) {
        पूर्णाङ्क c = tok_nextc(tok);
        सचे (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        अञ्ञथा सचे (c == *s) {
            s++;
            अनुवर्त;
        }

        tok_backup(tok, c);
        याव (s != test) {
            tok_backup(tok, *--s);
        }
        पटिनिवत्त res;
    }
}

स्थिर पूर्णाङ्क
verify_end_of_सङ्ख्या(संरचना tok_state *tok, पूर्णाङ्क c, ध्रुव वर्ण *kind) {
    सचे (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        पटिनिवत्त 1;
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
    सचे (c == 'a') {
        r = lookahead(tok, "nd");
    }
    अञ्ञथा सचे (c == 'e') {
        r = lookahead(tok, "lse");
    }
    अञ्ञथा सचे (c == 'f') {
        r = lookahead(tok, "or");
    }
    अञ्ञथा सचे (c == 'i') {
        पूर्णाङ्क c2 = tok_nextc(tok);
        सचे (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    अञ्ञथा सचे (c == 'o') {
        r = lookahead(tok, "r");
    }
    अञ्ञथा सचे (c == 'n') {
        r = lookahead(tok, "ot");
    }
    सचे (r) {
        tok_backup(tok, c);
        सचे (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            पटिनिवत्त 0;
        }
        tok_nextc(tok);
    }
    अञ्ञथा /* In future releases, only error will remain. */
    सचे (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        पटिनिवत्त 0;
    }
    पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क
tok_decimal_tail(संरचना tok_state *tok)
{
    पूर्णाङ्क c;

    याव (1) {
        करोहि {
            c = tok_nextc(tok);
        } याव (Py_ISDIGIT(c));
        सचे (c != '_') {
            भङ्ग;
        }
        c = tok_nextc(tok);
        सचे (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            पटिनिवत्त 0;
        }
    }
    पटिनिवत्त c;
}

पूर्णाङ्क
Py_Lexer_scan_सङ्ख्या(संरचना tok_state *tok, संरचना token *token, पूर्णाङ्क c,
                     पूर्णाङ्क leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    सचे (leading_dot) {
        गच्छ fraction;
    }
    सचे (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        सचे (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            करोहि {
                सचे (c == '_') {
                    c = tok_nextc(tok);
                }
                सचे (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    पटिनिवत्त MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                करोहि {
                    c = tok_nextc(tok);
                } याव (Py_ISXDIGIT(c));
            } याव (c == '_');
            सचे (!verify_end_of_सङ्ख्या(tok, c, "hexadecimal")) {
                पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
            }
        }
        अञ्ञथा सचे (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            करोहि {
                सचे (c == '_') {
                    c = tok_nextc(tok);
                }
                सचे (c < '0' || c >= '8') {
                    सचे (Py_ISDIGIT(c)) {
                        पटिनिवत्त MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    अञ्ञथा {
                        tok_backup(tok, c);
                        पटिनिवत्त MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                करोहि {
                    c = tok_nextc(tok);
                } याव ('0' <= c && c < '8');
            } याव (c == '_');
            सचे (Py_ISDIGIT(c)) {
                पटिनिवत्त MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            सचे (!verify_end_of_सङ्ख्या(tok, c, "octal")) {
                पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
            }
        }
        अञ्ञथा सचे (c == 'b' || c == 'B') {
            /*द्विमान*/
            c = tok_nextc(tok);
            करोहि {
                सचे (c == '_') {
                    c = tok_nextc(tok);
                }
                सचे (c != '0' && c != '1') {
                    सचे (Py_ISDIGIT(c)) {
                        पटिनिवत्त MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    अञ्ञथा {
                        tok_backup(tok, c);
                        पटिनिवत्त MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                करोहि {
                    c = tok_nextc(tok);
                } याव (c == '0' || c == '1');
            } याव (c == '_');
            सचे (Py_ISDIGIT(c)) {
                पटिनिवत्त MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            सचे (!verify_end_of_सङ्ख्या(tok, c, "binary")) {
                पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
            }
        }
        अञ्ञथा {
            पूर्णाङ्क nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            याव (1) {
                सचे (c == '_') {
                    c = tok_nextc(tok);
                    सचे (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        पटिनिवत्त MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                सचे (c != '0') {
                    भङ्ग;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            सचे (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                सचे (c == 0) {
                    पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
                }
            }
            सचे (c == '.') {
                c = tok_nextc(tok);
                गच्छ fraction;
            }
            अञ्ञथा सचे (c == 'e' || c == 'E') {
                गच्छ exponent;
            }
            अञ्ञथा सचे (c == 'j' || c == 'J') {
                गच्छ imaginary;
            }
            अञ्ञथा सचे (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                पटिनिवत्त MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (पूर्णाङ्क)(tok->start + 1 - tok->पङ्क्तिः_start),
                        (पूर्णाङ्क)(zeros_end - tok->पङ्क्तिः_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            सचे (!verify_end_of_सङ्ख्या(tok, c, "decimal")) {
                पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    अञ्ञथा {
        /* Decimal */
        c = tok_decimal_tail(tok);
        सचे (c == 0) {
            पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            सचे (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                सचे (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    सचे (c == 0) {
                        पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            सचे (c == 'e' || c == 'E') {
                पूर्णाङ्क e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                सचे (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    सचे (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        पटिनिवत्त MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } अञ्ञथा सचे (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    सचे (!verify_end_of_सङ्ख्या(tok, e, "decimal")) {
                        पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    पटिनिवत्त MAKE_TOKEN(सङ्ख्या);
                }
                c = tok_decimal_tail(tok);
                सचे (c == 0) {
                    पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
                }
            }
            सचे (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                सचे (!verify_end_of_सङ्ख्या(tok, c, "imaginary")) {
                    पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
                }
            }
            अञ्ञथा सचे (!verify_end_of_सङ्ख्या(tok, c, "decimal")) {
                पटिनिवत्त MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    पटिनिवत्त MAKE_TOKEN(सङ्ख्या);
}
