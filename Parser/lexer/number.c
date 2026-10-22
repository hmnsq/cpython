#अन्तर्भाव "Python.h"
#अन्तर्भाव "pycore_token.h"

#अन्तर्भाव "lexer_internal.h"
#अन्तर्भाव "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

स्थिर पूर्णाङ्क
lookahead(संरचना tok_state *tok, अचर वर्ण *test)
{
    अचर वर्ण *s = test;
    पूर्णाङ्क res = 0;
    जबसम्म (1) {
        पूर्णाङ्क c = tok_nextc(tok);
        यदि (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        अन्यथा यदि (c == *s) {
            s++;
            जारी;
        }

        tok_backup(tok, c);
        जबसम्म (s != test) {
            tok_backup(tok, *--s);
        }
        फर्काऊ res;
    }
}

स्थिर पूर्णाङ्क
verify_end_of_सङ्ख्या(संरचना tok_state *tok, पूर्णाङ्क c, अचर वर्ण *kind) {
    यदि (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        फर्काऊ 1;
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
    यदि (c == 'a') {
        r = lookahead(tok, "nd");
    }
    अन्यथा यदि (c == 'e') {
        r = lookahead(tok, "lse");
    }
    अन्यथा यदि (c == 'f') {
        r = lookahead(tok, "or");
    }
    अन्यथा यदि (c == 'i') {
        पूर्णाङ्क c2 = tok_nextc(tok);
        यदि (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    अन्यथा यदि (c == 'o') {
        r = lookahead(tok, "r");
    }
    अन्यथा यदि (c == 'n') {
        r = lookahead(tok, "ot");
    }
    यदि (r) {
        tok_backup(tok, c);
        यदि (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            फर्काऊ 0;
        }
        tok_nextc(tok);
    }
    अन्यथा /* In future releases, only error will remain. */
    यदि (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        फर्काऊ 0;
    }
    फर्काऊ 1;
}

स्थिर पूर्णाङ्क
tok_decimal_tail(संरचना tok_state *tok)
{
    पूर्णाङ्क c;

    जबसम्म (1) {
        गर {
            c = tok_nextc(tok);
        } जबसम्म (Py_ISDIGIT(c));
        यदि (c != '_') {
            रोक;
        }
        c = tok_nextc(tok);
        यदि (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            फर्काऊ 0;
        }
    }
    फर्काऊ c;
}

पूर्णाङ्क
Py_Lexer_scan_सङ्ख्या(संरचना tok_state *tok, संरचना token *token, पूर्णाङ्क c,
                     पूर्णाङ्क leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    यदि (leading_dot) {
        गच्छ fraction;
    }
    यदि (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        यदि (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            गर {
                यदि (c == '_') {
                    c = tok_nextc(tok);
                }
                यदि (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    फर्काऊ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                गर {
                    c = tok_nextc(tok);
                } जबसम्म (Py_ISXDIGIT(c));
            } जबसम्म (c == '_');
            यदि (!verify_end_of_सङ्ख्या(tok, c, "hexadecimal")) {
                फर्काऊ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        अन्यथा यदि (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            गर {
                यदि (c == '_') {
                    c = tok_nextc(tok);
                }
                यदि (c < '0' || c >= '8') {
                    यदि (Py_ISDIGIT(c)) {
                        फर्काऊ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    अन्यथा {
                        tok_backup(tok, c);
                        फर्काऊ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                गर {
                    c = tok_nextc(tok);
                } जबसम्म ('0' <= c && c < '8');
            } जबसम्म (c == '_');
            यदि (Py_ISDIGIT(c)) {
                फर्काऊ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            यदि (!verify_end_of_सङ्ख्या(tok, c, "octal")) {
                फर्काऊ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        अन्यथा यदि (c == 'b' || c == 'B') {
            /*द्विआधारी*/
            c = tok_nextc(tok);
            गर {
                यदि (c == '_') {
                    c = tok_nextc(tok);
                }
                यदि (c != '0' && c != '1') {
                    यदि (Py_ISDIGIT(c)) {
                        फर्काऊ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    अन्यथा {
                        tok_backup(tok, c);
                        फर्काऊ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                गर {
                    c = tok_nextc(tok);
                } जबसम्म (c == '0' || c == '1');
            } जबसम्म (c == '_');
            यदि (Py_ISDIGIT(c)) {
                फर्काऊ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            यदि (!verify_end_of_सङ्ख्या(tok, c, "binary")) {
                फर्काऊ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        अन्यथा {
            पूर्णाङ्क nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            जबसम्म (1) {
                यदि (c == '_') {
                    c = tok_nextc(tok);
                    यदि (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        फर्काऊ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                यदि (c != '0') {
                    रोक;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            यदि (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                यदि (c == 0) {
                    फर्काऊ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            यदि (c == '.') {
                c = tok_nextc(tok);
                गच्छ fraction;
            }
            अन्यथा यदि (c == 'e' || c == 'E') {
                गच्छ exponent;
            }
            अन्यथा यदि (c == 'j' || c == 'J') {
                गच्छ imaginary;
            }
            अन्यथा यदि (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                फर्काऊ MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (पूर्णाङ्क)(tok->start + 1 - tok->पङ्क्ति_start),
                        (पूर्णाङ्क)(zeros_end - tok->पङ्क्ति_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            यदि (!verify_end_of_सङ्ख्या(tok, c, "decimal")) {
                फर्काऊ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    अन्यथा {
        /* Decimal */
        c = tok_decimal_tail(tok);
        यदि (c == 0) {
            फर्काऊ MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            यदि (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                यदि (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    यदि (c == 0) {
                        फर्काऊ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            यदि (c == 'e' || c == 'E') {
                पूर्णाङ्क e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                यदि (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    यदि (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        फर्काऊ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } अन्यथा यदि (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    यदि (!verify_end_of_सङ्ख्या(tok, e, "decimal")) {
                        फर्काऊ MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    फर्काऊ MAKE_TOKEN(सङ्ख्या);
                }
                c = tok_decimal_tail(tok);
                यदि (c == 0) {
                    फर्काऊ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            यदि (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                यदि (!verify_end_of_सङ्ख्या(tok, c, "imaginary")) {
                    फर्काऊ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            अन्यथा यदि (!verify_end_of_सङ्ख्या(tok, c, "decimal")) {
                फर्काऊ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    फर्काऊ MAKE_TOKEN(सङ्ख्या);
}
