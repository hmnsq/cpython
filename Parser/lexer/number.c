#ଅନ୍ତର୍ଭାଵ "Python.h"
#ଅନ୍ତର୍ଭାଵ "pycore_token.h"

#ଅନ୍ତର୍ଭାଵ "lexer_internal.h"
#ଅନ୍ତର୍ଭାଵ "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
lookahead(ସଂରଚନା tok_state *tok, ଅଚଳ ଅକ୍ଷର *test)
{
    ଅଚଳ ଅକ୍ଷର *s = test;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା res = 0;
    ଯେତେବେଳେ (1) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = tok_nextc(tok);
        ଯଦି (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        ଅନ୍ୟଥା ଯଦି (c == *s) {
            s++;
            ଜାରି;
        }

        tok_backup(tok, c);
        ଯେତେବେଳେ (s != test) {
            tok_backup(tok, *--s);
        }
        ଫେରନ୍ତୁ res;
    }
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
verify_end_of_ସଂଖ୍ୟା(ସଂରଚନା tok_state *tok, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ଅଚଳ ଅକ୍ଷର *kind) {
    ଯଦି (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        ଫେରନ୍ତୁ 1;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = 0;
    ଯଦି (c == 'a') {
        r = lookahead(tok, "nd");
    }
    ଅନ୍ୟଥା ଯଦି (c == 'e') {
        r = lookahead(tok, "lse");
    }
    ଅନ୍ୟଥା ଯଦି (c == 'f') {
        r = lookahead(tok, "or");
    }
    ଅନ୍ୟଥା ଯଦି (c == 'i') {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା c2 = tok_nextc(tok);
        ଯଦି (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    ଅନ୍ୟଥା ଯଦି (c == 'o') {
        r = lookahead(tok, "r");
    }
    ଅନ୍ୟଥା ଯଦି (c == 'n') {
        r = lookahead(tok, "ot");
    }
    ଯଦି (r) {
        tok_backup(tok, c);
        ଯଦି (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            ଫେରନ୍ତୁ 0;
        }
        tok_nextc(tok);
    }
    ଅନ୍ୟଥା /* In future releases, only error will remain. */
    ଯଦି (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tok_decimal_tail(ସଂରଚନା tok_state *tok)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;

    ଯେତେବେଳେ (1) {
        କର {
            c = tok_nextc(tok);
        } ଯେତେବେଳେ (Py_ISDIGIT(c));
        ଯଦି (c != '_') {
            ଭାଙ୍ଗ;
        }
        c = tok_nextc(tok);
        ଯଦି (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            ଫେରନ୍ତୁ 0;
        }
    }
    ଫେରନ୍ତୁ c;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
Py_Lexer_scan_ସଂଖ୍ୟା(ସଂରଚନା tok_state *tok, ସଂରଚନା token *token, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c,
                     ପୂର୍ଣ୍ଣସଂଖ୍ୟା leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ଯଦି (leading_dot) {
        ଯାଅ fraction;
    }
    ଯଦି (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        ଯଦି (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            କର {
                ଯଦି (c == '_') {
                    c = tok_nextc(tok);
                }
                ଯଦି (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    ଫେରନ୍ତୁ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                କର {
                    c = tok_nextc(tok);
                } ଯେତେବେଳେ (Py_ISXDIGIT(c));
            } ଯେତେବେଳେ (c == '_');
            ଯଦି (!verify_end_of_ସଂଖ୍ୟା(tok, c, "hexadecimal")) {
                ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ଅନ୍ୟଥା ଯଦି (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            କର {
                ଯଦି (c == '_') {
                    c = tok_nextc(tok);
                }
                ଯଦି (c < '0' || c >= '8') {
                    ଯଦି (Py_ISDIGIT(c)) {
                        ଫେରନ୍ତୁ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    ଅନ୍ୟଥା {
                        tok_backup(tok, c);
                        ଫେରନ୍ତୁ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                କର {
                    c = tok_nextc(tok);
                } ଯେତେବେଳେ ('0' <= c && c < '8');
            } ଯେତେବେଳେ (c == '_');
            ଯଦି (Py_ISDIGIT(c)) {
                ଫେରନ୍ତୁ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            ଯଦି (!verify_end_of_ସଂଖ୍ୟା(tok, c, "octal")) {
                ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ଅନ୍ୟଥା ଯଦି (c == 'b' || c == 'B') {
            /*ଦ୍ୱିମିକ*/
            c = tok_nextc(tok);
            କର {
                ଯଦି (c == '_') {
                    c = tok_nextc(tok);
                }
                ଯଦି (c != '0' && c != '1') {
                    ଯଦି (Py_ISDIGIT(c)) {
                        ଫେରନ୍ତୁ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    ଅନ୍ୟଥା {
                        tok_backup(tok, c);
                        ଫେରନ୍ତୁ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                କର {
                    c = tok_nextc(tok);
                } ଯେତେବେଳେ (c == '0' || c == '1');
            } ଯେତେବେଳେ (c == '_');
            ଯଦି (Py_ISDIGIT(c)) {
                ଫେରନ୍ତୁ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            ଯଦି (!verify_end_of_ସଂଖ୍ୟା(tok, c, "binary")) {
                ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ଅନ୍ୟଥା {
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            ଯେତେବେଳେ (1) {
                ଯଦି (c == '_') {
                    c = tok_nextc(tok);
                    ଯଦି (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ଫେରନ୍ତୁ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                ଯଦି (c != '0') {
                    ଭାଙ୍ଗ;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            ଯଦି (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                ଯଦି (c == 0) {
                    ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ଯଦି (c == '.') {
                c = tok_nextc(tok);
                ଯାଅ fraction;
            }
            ଅନ୍ୟଥା ଯଦି (c == 'e' || c == 'E') {
                ଯାଅ exponent;
            }
            ଅନ୍ୟଥା ଯଦି (c == 'j' || c == 'J') {
                ଯାଅ imaginary;
            }
            ଅନ୍ୟଥା ଯଦି (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                ଫେରନ୍ତୁ MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(tok->start + 1 - tok->ଧାଡ଼ି_start),
                        (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(zeros_end - tok->ଧାଡ଼ି_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            ଯଦି (!verify_end_of_ସଂଖ୍ୟା(tok, c, "decimal")) {
                ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    ଅନ୍ୟଥା {
        /* Decimal */
        c = tok_decimal_tail(tok);
        ଯଦି (c == 0) {
            ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            ଯଦି (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                ଯଦି (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    ଯଦି (c == 0) {
                        ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            ଯଦି (c == 'e' || c == 'E') {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                ଯଦି (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    ଯଦି (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ଫେରନ୍ତୁ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } ଅନ୍ୟଥା ଯଦି (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    ଯଦି (!verify_end_of_ସଂଖ୍ୟା(tok, e, "decimal")) {
                        ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    ଫେରନ୍ତୁ MAKE_TOKEN(ସଂଖ୍ୟା);
                }
                c = tok_decimal_tail(tok);
                ଯଦି (c == 0) {
                    ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ଯଦି (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                ଯଦି (!verify_end_of_ସଂଖ୍ୟା(tok, c, "imaginary")) {
                    ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ଅନ୍ୟଥା ଯଦି (!verify_end_of_ସଂଖ୍ୟା(tok, c, "decimal")) {
                ଫେରନ୍ତୁ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    ଫେରନ୍ତୁ MAKE_TOKEN(ସଂଖ୍ୟା);
}
