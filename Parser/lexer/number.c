#شامل "Python.h"
#شامل "pycore_token.h"

#شامل "lexer_internal.h"
#شامل "../tokenizer/helpers.h"

#تعریف_کرو MAKE_TOKEN(token_type) Py_Lexer_token_تیاری(tok, token, token_type, p_آغاز, p_انتہا)

ساکن صحیح
lookahead(ساخت tok_state *tok, ثابت حرف *test)
{
    ثابت حرف *s = test;
    صحیح res = 0;
    جب (1) {
        صحیح c = tok_nextc(tok);
        اگر (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        ورنہ اگر (c == *s) {
            s++;
            جاری;
        }

        tok_backup(tok, c);
        جب (s != test) {
            tok_backup(tok, *--s);
        }
        واپس res;
    }
}

ساکن صحیح
verify_انتہا_کا_نمبر(ساخت tok_state *tok, صحیح c, ثابت حرف *قسم) {
    اگر (tok->tok_extra_علامات) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        واپس 1;
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
    صحیح r = 0;
    اگر (c == 'a') {
        r = lookahead(tok, "nd");
    }
    ورنہ اگر (c == 'e') {
        r = lookahead(tok, "lse");
    }
    ورنہ اگر (c == 'f') {
        r = lookahead(tok, "or");
    }
    ورنہ اگر (c == 'i') {
        صحیح c2 = tok_nextc(tok);
        اگر (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    ورنہ اگر (c == 'o') {
        r = lookahead(tok, "r");
    }
    ورنہ اگر (c == 'n') {
        r = lookahead(tok, "ot");
    }
    اگر (r) {
        tok_backup(tok, c);
        اگر (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", قسم))
        {
            واپس 0;
        }
        tok_nextc(tok);
    }
    ورنہ /* In future releases, only error will remain. */
    اگر (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", قسم);
        واپس 0;
    }
    واپس 1;
}

ساکن صحیح
tok_decimal_دم(ساخت tok_state *tok)
{
    صحیح c;

    جب (1) {
        کرو {
            c = tok_nextc(tok);
        } جب (Py_ISDIGIT(c));
        اگر (c != '_') {
            توڑو;
        }
        c = tok_nextc(tok);
        اگر (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            واپس 0;
        }
    }
    واپس c;
}

صحیح
Py_Lexer_جائزہ_نمبر(ساخت tok_state *tok, ساخت token *token, صحیح c,
                     صحیح leading_dot)
{
    _PyTok_Off p_آغاز = -1;
    _PyTok_Off p_انتہا = -1;

    اگر (leading_dot) {
        جاؤ fraction;
    }
    اگر (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        اگر (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            کرو {
                اگر (c == '_') {
                    c = tok_nextc(tok);
                }
                اگر (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    واپس MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                کرو {
                    c = tok_nextc(tok);
                } جب (Py_ISXDIGIT(c));
            } جب (c == '_');
            اگر (!verify_انتہا_کا_نمبر(tok, c, "hexadecimal")) {
                واپس MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ورنہ اگر (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            کرو {
                اگر (c == '_') {
                    c = tok_nextc(tok);
                }
                اگر (c < '0' || c >= '8') {
                    اگر (Py_ISDIGIT(c)) {
                        واپس MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    ورنہ {
                        tok_backup(tok, c);
                        واپس MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                کرو {
                    c = tok_nextc(tok);
                } جب ('0' <= c && c < '8');
            } جب (c == '_');
            اگر (Py_ISDIGIT(c)) {
                واپس MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            اگر (!verify_انتہا_کا_نمبر(tok, c, "octal")) {
                واپس MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ورنہ اگر (c == 'b' || c == 'B') {
            /*ثنائی*/
            c = tok_nextc(tok);
            کرو {
                اگر (c == '_') {
                    c = tok_nextc(tok);
                }
                اگر (c != '0' && c != '1') {
                    اگر (Py_ISDIGIT(c)) {
                        واپس MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    ورنہ {
                        tok_backup(tok, c);
                        واپس MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                کرو {
                    c = tok_nextc(tok);
                } جب (c == '0' || c == '1');
            } جب (c == '_');
            اگر (Py_ISDIGIT(c)) {
                واپس MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            اگر (!verify_انتہا_کا_نمبر(tok, c, "binary")) {
                واپس MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ورنہ {
            صحیح nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            جب (1) {
                اگر (c == '_') {
                    c = tok_nextc(tok);
                    اگر (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        واپس MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                اگر (c != '0') {
                    توڑو;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_انتہا = tok->cur;
            اگر (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_دم(tok);
                اگر (c == 0) {
                    واپس MAKE_TOKEN(ERRORTOKEN);
                }
            }
            اگر (c == '.') {
                c = tok_nextc(tok);
                جاؤ fraction;
            }
            ورنہ اگر (c == 'e' || c == 'E') {
                جاؤ exponent;
            }
            ورنہ اگر (c == 'j' || c == 'J') {
                جاؤ imaginary;
            }
            ورنہ اگر (nonzero && !tok->tok_extra_علامات) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                واپس MAKE_TOKEN(Py_Tokenizer_syntaxerror_معلوم_range(
                        tok, (صحیح)(tok->آغاز + 1 - tok->سطر_آغاز),
                        (صحیح)(zeros_انتہا - tok->سطر_آغاز),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            اگر (!verify_انتہا_کا_نمبر(tok, c, "decimal")) {
                واپس MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    ورنہ {
        /* Decimal */
        c = tok_decimal_دم(tok);
        اگر (c == 0) {
            واپس MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            اگر (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                اگر (Py_ISDIGIT(c)) {
                    c = tok_decimal_دم(tok);
                    اگر (c == 0) {
                        واپس MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            اگر (c == 'e' || c == 'E') {
                صحیح e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                اگر (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    اگر (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        واپس MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } ورنہ اگر (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    اگر (!verify_انتہا_کا_نمبر(tok, e, "decimal")) {
                        واپس MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_آغاز = tok->آغاز;
                    p_انتہا = tok->cur;
                    واپس MAKE_TOKEN(نمبر);
                }
                c = tok_decimal_دم(tok);
                اگر (c == 0) {
                    واپس MAKE_TOKEN(ERRORTOKEN);
                }
            }
            اگر (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                اگر (!verify_انتہا_کا_نمبر(tok, c, "imaginary")) {
                    واپس MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ورنہ اگر (!verify_انتہا_کا_نمبر(tok, c, "decimal")) {
                واپس MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_آغاز = tok->آغاز;
    p_انتہا = tok->cur;
    واپس MAKE_TOKEN(نمبر);
}
