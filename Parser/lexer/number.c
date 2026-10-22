#ضمن "Python.h"
#ضمن "pycore_token.h"

#ضمن "lexer_internal.h"
#ضمن "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

ساكن صحيح
lookahead(بنية tok_state *tok, ثابت حرف *test)
{
    ثابت حرف *s = test;
    صحيح res = 0;
    بينما (1) {
        صحيح c = tok_nextc(tok);
        إذا (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        وإلا إذا (c == *s) {
            s++;
            استمر;
        }

        tok_backup(tok, c);
        بينما (s != test) {
            tok_backup(tok, *--s);
        }
        إرجاع res;
    }
}

ساكن صحيح
verify_end_of_العدد(بنية tok_state *tok, صحيح c, ثابت حرف *kind) {
    إذا (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        إرجاع 1;
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
    صحيح r = 0;
    إذا (c == 'a') {
        r = lookahead(tok, "nd");
    }
    وإلا إذا (c == 'e') {
        r = lookahead(tok, "lse");
    }
    وإلا إذا (c == 'f') {
        r = lookahead(tok, "or");
    }
    وإلا إذا (c == 'i') {
        صحيح c2 = tok_nextc(tok);
        إذا (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    وإلا إذا (c == 'o') {
        r = lookahead(tok, "r");
    }
    وإلا إذا (c == 'n') {
        r = lookahead(tok, "ot");
    }
    إذا (r) {
        tok_backup(tok, c);
        إذا (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            إرجاع 0;
        }
        tok_nextc(tok);
    }
    وإلا /* In future releases, only error will remain. */
    إذا (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        إرجاع 0;
    }
    إرجاع 1;
}

ساكن صحيح
tok_decimal_tail(بنية tok_state *tok)
{
    صحيح c;

    بينما (1) {
        افعل {
            c = tok_nextc(tok);
        } بينما (Py_ISDIGIT(c));
        إذا (c != '_') {
            اكسر;
        }
        c = tok_nextc(tok);
        إذا (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            إرجاع 0;
        }
    }
    إرجاع c;
}

صحيح
Py_Lexer_scan_العدد(بنية tok_state *tok, بنية token *token, صحيح c,
                     صحيح leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    إذا (leading_dot) {
        اذهب fraction;
    }
    إذا (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        إذا (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            افعل {
                إذا (c == '_') {
                    c = tok_nextc(tok);
                }
                إذا (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    إرجاع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                افعل {
                    c = tok_nextc(tok);
                } بينما (Py_ISXDIGIT(c));
            } بينما (c == '_');
            إذا (!verify_end_of_العدد(tok, c, "hexadecimal")) {
                إرجاع MAKE_TOKEN(ERRORTOKEN);
            }
        }
        وإلا إذا (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            افعل {
                إذا (c == '_') {
                    c = tok_nextc(tok);
                }
                إذا (c < '0' || c >= '8') {
                    إذا (Py_ISDIGIT(c)) {
                        إرجاع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    وإلا {
                        tok_backup(tok, c);
                        إرجاع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                افعل {
                    c = tok_nextc(tok);
                } بينما ('0' <= c && c < '8');
            } بينما (c == '_');
            إذا (Py_ISDIGIT(c)) {
                إرجاع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            إذا (!verify_end_of_العدد(tok, c, "octal")) {
                إرجاع MAKE_TOKEN(ERRORTOKEN);
            }
        }
        وإلا إذا (c == 'b' || c == 'B') {
            /*ثنائي*/
            c = tok_nextc(tok);
            افعل {
                إذا (c == '_') {
                    c = tok_nextc(tok);
                }
                إذا (c != '0' && c != '1') {
                    إذا (Py_ISDIGIT(c)) {
                        إرجاع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    وإلا {
                        tok_backup(tok, c);
                        إرجاع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                افعل {
                    c = tok_nextc(tok);
                } بينما (c == '0' || c == '1');
            } بينما (c == '_');
            إذا (Py_ISDIGIT(c)) {
                إرجاع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            إذا (!verify_end_of_العدد(tok, c, "binary")) {
                إرجاع MAKE_TOKEN(ERRORTOKEN);
            }
        }
        وإلا {
            صحيح nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            بينما (1) {
                إذا (c == '_') {
                    c = tok_nextc(tok);
                    إذا (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        إرجاع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                إذا (c != '0') {
                    اكسر;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            إذا (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                إذا (c == 0) {
                    إرجاع MAKE_TOKEN(ERRORTOKEN);
                }
            }
            إذا (c == '.') {
                c = tok_nextc(tok);
                اذهب fraction;
            }
            وإلا إذا (c == 'e' || c == 'E') {
                اذهب exponent;
            }
            وإلا إذا (c == 'j' || c == 'J') {
                اذهب imaginary;
            }
            وإلا إذا (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                إرجاع MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (صحيح)(tok->start + 1 - tok->سطر_start),
                        (صحيح)(zeros_end - tok->سطر_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            إذا (!verify_end_of_العدد(tok, c, "decimal")) {
                إرجاع MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    وإلا {
        /* Decimal */
        c = tok_decimal_tail(tok);
        إذا (c == 0) {
            إرجاع MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            إذا (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                إذا (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    إذا (c == 0) {
                        إرجاع MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            إذا (c == 'e' || c == 'E') {
                صحيح e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                إذا (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    إذا (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        إرجاع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } وإلا إذا (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    إذا (!verify_end_of_العدد(tok, e, "decimal")) {
                        إرجاع MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    إرجاع MAKE_TOKEN(العدد);
                }
                c = tok_decimal_tail(tok);
                إذا (c == 0) {
                    إرجاع MAKE_TOKEN(ERRORTOKEN);
                }
            }
            إذا (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                إذا (!verify_end_of_العدد(tok, c, "imaginary")) {
                    إرجاع MAKE_TOKEN(ERRORTOKEN);
                }
            }
            وإلا إذا (!verify_end_of_العدد(tok, c, "decimal")) {
                إرجاع MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    إرجاع MAKE_TOKEN(العدد);
}
