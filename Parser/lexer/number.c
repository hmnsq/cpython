#شامل "Python.h"
#شامل "pycore_token.h"

#شامل "lexer_internal.h"
#شامل "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

ایستا صحیح
lookahead(ساختار tok_state *tok, ثابت نویسه *test)
{
    ثابت نویسه *s = test;
    صحیح res = 0;
    وقتی (1) {
        صحیح c = tok_nextc(tok);
        اگر (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        وگرنه اگر (c == *s) {
            s++;
            ادامه;
        }

        tok_backup(tok, c);
        وقتی (s != test) {
            tok_backup(tok, *--s);
        }
        بازگشت res;
    }
}

ایستا صحیح
verify_end_of_عدد(ساختار tok_state *tok, صحیح c, ثابت نویسه *kind) {
    اگر (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        بازگشت 1;
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
    وگرنه اگر (c == 'e') {
        r = lookahead(tok, "lse");
    }
    وگرنه اگر (c == 'f') {
        r = lookahead(tok, "or");
    }
    وگرنه اگر (c == 'i') {
        صحیح c2 = tok_nextc(tok);
        اگر (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    وگرنه اگر (c == 'o') {
        r = lookahead(tok, "r");
    }
    وگرنه اگر (c == 'n') {
        r = lookahead(tok, "ot");
    }
    اگر (r) {
        tok_backup(tok, c);
        اگر (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            بازگشت 0;
        }
        tok_nextc(tok);
    }
    وگرنه /* In future releases, only error will remain. */
    اگر (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        بازگشت 0;
    }
    بازگشت 1;
}

ایستا صحیح
tok_decimal_tail(ساختار tok_state *tok)
{
    صحیح c;

    وقتی (1) {
        انجامبده {
            c = tok_nextc(tok);
        } وقتی (Py_ISDIGIT(c));
        اگر (c != '_') {
            بشکن;
        }
        c = tok_nextc(tok);
        اگر (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            بازگشت 0;
        }
    }
    بازگشت c;
}

صحیح
Py_Lexer_scan_عدد(ساختار tok_state *tok, ساختار token *token, صحیح c,
                     صحیح leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    اگر (leading_dot) {
        برو fraction;
    }
    اگر (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        اگر (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            انجامبده {
                اگر (c == '_') {
                    c = tok_nextc(tok);
                }
                اگر (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    بازگشت MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                انجامبده {
                    c = tok_nextc(tok);
                } وقتی (Py_ISXDIGIT(c));
            } وقتی (c == '_');
            اگر (!verify_end_of_عدد(tok, c, "hexadecimal")) {
                بازگشت MAKE_TOKEN(ERRORTOKEN);
            }
        }
        وگرنه اگر (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            انجامبده {
                اگر (c == '_') {
                    c = tok_nextc(tok);
                }
                اگر (c < '0' || c >= '8') {
                    اگر (Py_ISDIGIT(c)) {
                        بازگشت MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    وگرنه {
                        tok_backup(tok, c);
                        بازگشت MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                انجامبده {
                    c = tok_nextc(tok);
                } وقتی ('0' <= c && c < '8');
            } وقتی (c == '_');
            اگر (Py_ISDIGIT(c)) {
                بازگشت MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            اگر (!verify_end_of_عدد(tok, c, "octal")) {
                بازگشت MAKE_TOKEN(ERRORTOKEN);
            }
        }
        وگرنه اگر (c == 'b' || c == 'B') {
            /*دودویی*/
            c = tok_nextc(tok);
            انجامبده {
                اگر (c == '_') {
                    c = tok_nextc(tok);
                }
                اگر (c != '0' && c != '1') {
                    اگر (Py_ISDIGIT(c)) {
                        بازگشت MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    وگرنه {
                        tok_backup(tok, c);
                        بازگشت MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                انجامبده {
                    c = tok_nextc(tok);
                } وقتی (c == '0' || c == '1');
            } وقتی (c == '_');
            اگر (Py_ISDIGIT(c)) {
                بازگشت MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            اگر (!verify_end_of_عدد(tok, c, "binary")) {
                بازگشت MAKE_TOKEN(ERRORTOKEN);
            }
        }
        وگرنه {
            صحیح nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            وقتی (1) {
                اگر (c == '_') {
                    c = tok_nextc(tok);
                    اگر (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        بازگشت MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                اگر (c != '0') {
                    بشکن;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            اگر (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                اگر (c == 0) {
                    بازگشت MAKE_TOKEN(ERRORTOKEN);
                }
            }
            اگر (c == '.') {
                c = tok_nextc(tok);
                برو fraction;
            }
            وگرنه اگر (c == 'e' || c == 'E') {
                برو exponent;
            }
            وگرنه اگر (c == 'j' || c == 'J') {
                برو imaginary;
            }
            وگرنه اگر (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                بازگشت MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (صحیح)(tok->start + 1 - tok->سطر_start),
                        (صحیح)(zeros_end - tok->سطر_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            اگر (!verify_end_of_عدد(tok, c, "decimal")) {
                بازگشت MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    وگرنه {
        /* Decimal */
        c = tok_decimal_tail(tok);
        اگر (c == 0) {
            بازگشت MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            اگر (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                اگر (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    اگر (c == 0) {
                        بازگشت MAKE_TOKEN(ERRORTOKEN);
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
                        بازگشت MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } وگرنه اگر (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    اگر (!verify_end_of_عدد(tok, e, "decimal")) {
                        بازگشت MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    بازگشت MAKE_TOKEN(عدد);
                }
                c = tok_decimal_tail(tok);
                اگر (c == 0) {
                    بازگشت MAKE_TOKEN(ERRORTOKEN);
                }
            }
            اگر (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                اگر (!verify_end_of_عدد(tok, c, "imaginary")) {
                    بازگشت MAKE_TOKEN(ERRORTOKEN);
                }
            }
            وگرنه اگر (!verify_end_of_عدد(tok, c, "decimal")) {
                بازگشت MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    بازگشت MAKE_TOKEN(عدد);
}
