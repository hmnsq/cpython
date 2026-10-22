#ضمن "Python.h"
#ضمن "pycore_token.h"

#ضمن "lexer_internal.h"
#ضمن "../tokenizer/helpers.h"

#تعریف_کرو MAKE_TOKEN(token_type) Py_Lexer_token_تیاری(tok, token, token_type, p_شروع, p_سرا)

ساكن صحيح
lookahead(بنية tok_state *tok, ثابت حرف *test)
{
    ثابت حرف *s = test;
    صحيح res = 0;
    طالما (1) {
        صحيح c = tok_nextc(tok);
        إذا (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        وإلا إذا (c == *s) {
            s++;
            استمر;
        }

        tok_backup(tok, c);
        طالما (s != test) {
            tok_backup(tok, *--s);
        }
        أرجع res;
    }
}

ساكن صحيح
verify_سرا_دا_نمبر(بنية tok_state *tok, صحيح c, ثابت حرف *قسم) {
    إذا (tok->tok_extra_ٹوکن) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        أرجع 1;
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
                "invalid %s literal", قسم))
        {
            أرجع 0;
        }
        tok_nextc(tok);
    }
    وإلا /* In future releases, only error will remain. */
    إذا (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", قسم);
        أرجع 0;
    }
    أرجع 1;
}

ساكن صحيح
tok_decimal_پوچھ(بنية tok_state *tok)
{
    صحيح c;

    طالما (1) {
        افعل {
            c = tok_nextc(tok);
        } طالما (Py_ISDIGIT(c));
        إذا (c != '_') {
            اكسر;
        }
        c = tok_nextc(tok);
        إذا (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            أرجع 0;
        }
    }
    أرجع c;
}

صحيح
Py_Lexer_جانچ_نمبر(بنية tok_state *tok, بنية token *token, صحيح c,
                     صحيح leading_dot)
{
    _PyTok_Off p_شروع = -1;
    _PyTok_Off p_سرا = -1;

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
                    أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                افعل {
                    c = tok_nextc(tok);
                } طالما (Py_ISXDIGIT(c));
            } طالما (c == '_');
            إذا (!verify_سرا_دا_نمبر(tok, c, "hexadecimal")) {
                أرجع MAKE_TOKEN(ERRORTOKEN);
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
                        أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    وإلا {
                        tok_backup(tok, c);
                        أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                افعل {
                    c = tok_nextc(tok);
                } طالما ('0' <= c && c < '8');
            } طالما (c == '_');
            إذا (Py_ISDIGIT(c)) {
                أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            إذا (!verify_سرا_دا_نمبر(tok, c, "octal")) {
                أرجع MAKE_TOKEN(ERRORTOKEN);
            }
        }
        وإلا إذا (c == 'b' || c == 'B') {
            /*ثنائی*/
            c = tok_nextc(tok);
            افعل {
                إذا (c == '_') {
                    c = tok_nextc(tok);
                }
                إذا (c != '0' && c != '1') {
                    إذا (Py_ISDIGIT(c)) {
                        أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    وإلا {
                        tok_backup(tok, c);
                        أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                افعل {
                    c = tok_nextc(tok);
                } طالما (c == '0' || c == '1');
            } طالما (c == '_');
            إذا (Py_ISDIGIT(c)) {
                أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            إذا (!verify_سرا_دا_نمبر(tok, c, "binary")) {
                أرجع MAKE_TOKEN(ERRORTOKEN);
            }
        }
        وإلا {
            صحيح nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            طالما (1) {
                إذا (c == '_') {
                    c = tok_nextc(tok);
                    إذا (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                إذا (c != '0') {
                    اكسر;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_سرا = tok->cur;
            إذا (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_پوچھ(tok);
                إذا (c == 0) {
                    أرجع MAKE_TOKEN(ERRORTOKEN);
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
            وإلا إذا (nonzero && !tok->tok_extra_ٹوکن) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                أرجع MAKE_TOKEN(Py_Tokenizer_syntaxerror_معلوم_range(
                        tok, (صحيح)(tok->شروع + 1 - tok->سطر_شروع),
                        (صحيح)(zeros_سرا - tok->سطر_شروع),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            إذا (!verify_سرا_دا_نمبر(tok, c, "decimal")) {
                أرجع MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    وإلا {
        /* Decimal */
        c = tok_decimal_پوچھ(tok);
        إذا (c == 0) {
            أرجع MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            إذا (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                إذا (Py_ISDIGIT(c)) {
                    c = tok_decimal_پوچھ(tok);
                    إذا (c == 0) {
                        أرجع MAKE_TOKEN(ERRORTOKEN);
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
                        أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } وإلا إذا (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    إذا (!verify_سرا_دا_نمبر(tok, e, "decimal")) {
                        أرجع MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_شروع = tok->شروع;
                    p_سرا = tok->cur;
                    أرجع MAKE_TOKEN(نمبر);
                }
                c = tok_decimal_پوچھ(tok);
                إذا (c == 0) {
                    أرجع MAKE_TOKEN(ERRORTOKEN);
                }
            }
            إذا (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                إذا (!verify_سرا_دا_نمبر(tok, c, "imaginary")) {
                    أرجع MAKE_TOKEN(ERRORTOKEN);
                }
            }
            وإلا إذا (!verify_سرا_دا_نمبر(tok, c, "decimal")) {
                أرجع MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_شروع = tok->شروع;
    p_سرا = tok->cur;
    أرجع MAKE_TOKEN(نمبر);
}
