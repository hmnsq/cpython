#অন্তর্ভাব "Python.h"
#অন্তর্ভাব "pycore_token.h"

#অন্তর্ভাব "lexer_internal.h"
#অন্তর্ভাব "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

স্থির পূর্ণসংখ্যা
lookahead(কাঠামো tok_state *tok, ধ্রুবক অক্ষর *test)
{
    ধ্রুবক অক্ষর *s = test;
    পূর্ণসংখ্যা res = 0;
    যখন (1) {
        পূর্ণসংখ্যা c = tok_nextc(tok);
        যদি (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        অন্যথা যদি (c == *s) {
            s++;
            চালু;
        }

        tok_backup(tok, c);
        যখন (s != test) {
            tok_backup(tok, *--s);
        }
        ফেরত res;
    }
}

স্থির পূর্ণসংখ্যা
verify_end_of_সংখ্যা(কাঠামো tok_state *tok, পূর্ণসংখ্যা c, ধ্রুবক অক্ষর *kind) {
    যদি (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        ফেরত 1;
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
    পূর্ণসংখ্যা r = 0;
    যদি (c == 'a') {
        r = lookahead(tok, "nd");
    }
    অন্যথা যদি (c == 'e') {
        r = lookahead(tok, "lse");
    }
    অন্যথা যদি (c == 'f') {
        r = lookahead(tok, "or");
    }
    অন্যথা যদি (c == 'i') {
        পূর্ণসংখ্যা c2 = tok_nextc(tok);
        যদি (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    অন্যথা যদি (c == 'o') {
        r = lookahead(tok, "r");
    }
    অন্যথা যদি (c == 'n') {
        r = lookahead(tok, "ot");
    }
    যদি (r) {
        tok_backup(tok, c);
        যদি (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            ফেরত 0;
        }
        tok_nextc(tok);
    }
    অন্যথা /* In future releases, only error will remain. */
    যদি (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        ফেরত 0;
    }
    ফেরত 1;
}

স্থির পূর্ণসংখ্যা
tok_decimal_লেজ(কাঠামো tok_state *tok)
{
    পূর্ণসংখ্যা c;

    যখন (1) {
        করো {
            c = tok_nextc(tok);
        } যখন (Py_ISDIGIT(c));
        যদি (c != '_') {
            বিরতি;
        }
        c = tok_nextc(tok);
        যদি (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            ফেরত 0;
        }
    }
    ফেরত c;
}

পূর্ণসংখ্যা
Py_Lexer_scan_সংখ্যা(কাঠামো tok_state *tok, কাঠামো token *token, পূর্ণসংখ্যা c,
                     পূর্ণসংখ্যা leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    যদি (leading_dot) {
        যাও fraction;
    }
    যদি (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        যদি (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            করো {
                যদি (c == '_') {
                    c = tok_nextc(tok);
                }
                যদি (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    ফেরত MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                করো {
                    c = tok_nextc(tok);
                } যখন (Py_ISXDIGIT(c));
            } যখন (c == '_');
            যদি (!verify_end_of_সংখ্যা(tok, c, "hexadecimal")) {
                ফেরত MAKE_TOKEN(ERRORTOKEN);
            }
        }
        অন্যথা যদি (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            করো {
                যদি (c == '_') {
                    c = tok_nextc(tok);
                }
                যদি (c < '0' || c >= '8') {
                    যদি (Py_ISDIGIT(c)) {
                        ফেরত MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    অন্যথা {
                        tok_backup(tok, c);
                        ফেরত MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                করো {
                    c = tok_nextc(tok);
                } যখন ('0' <= c && c < '8');
            } যখন (c == '_');
            যদি (Py_ISDIGIT(c)) {
                ফেরত MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            যদি (!verify_end_of_সংখ্যা(tok, c, "octal")) {
                ফেরত MAKE_TOKEN(ERRORTOKEN);
            }
        }
        অন্যথা যদি (c == 'b' || c == 'B') {
            /*দ্বিমিক*/
            c = tok_nextc(tok);
            করো {
                যদি (c == '_') {
                    c = tok_nextc(tok);
                }
                যদি (c != '0' && c != '1') {
                    যদি (Py_ISDIGIT(c)) {
                        ফেরত MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    অন্যথা {
                        tok_backup(tok, c);
                        ফেরত MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                করো {
                    c = tok_nextc(tok);
                } যখন (c == '0' || c == '1');
            } যখন (c == '_');
            যদি (Py_ISDIGIT(c)) {
                ফেরত MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            যদি (!verify_end_of_সংখ্যা(tok, c, "binary")) {
                ফেরত MAKE_TOKEN(ERRORTOKEN);
            }
        }
        অন্যথা {
            পূর্ণসংখ্যা nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            যখন (1) {
                যদি (c == '_') {
                    c = tok_nextc(tok);
                    যদি (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ফেরত MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                যদি (c != '0') {
                    বিরতি;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            যদি (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_লেজ(tok);
                যদি (c == 0) {
                    ফেরত MAKE_TOKEN(ERRORTOKEN);
                }
            }
            যদি (c == '.') {
                c = tok_nextc(tok);
                যাও fraction;
            }
            অন্যথা যদি (c == 'e' || c == 'E') {
                যাও exponent;
            }
            অন্যথা যদি (c == 'j' || c == 'J') {
                যাও imaginary;
            }
            অন্যথা যদি (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                ফেরত MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (পূর্ণসংখ্যা)(tok->start + 1 - tok->সারি_start),
                        (পূর্ণসংখ্যা)(zeros_end - tok->সারি_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            যদি (!verify_end_of_সংখ্যা(tok, c, "decimal")) {
                ফেরত MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    অন্যথা {
        /* Decimal */
        c = tok_decimal_লেজ(tok);
        যদি (c == 0) {
            ফেরত MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            যদি (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                যদি (Py_ISDIGIT(c)) {
                    c = tok_decimal_লেজ(tok);
                    যদি (c == 0) {
                        ফেরত MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            যদি (c == 'e' || c == 'E') {
                পূর্ণসংখ্যা e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                যদি (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    যদি (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ফেরত MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } অন্যথা যদি (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    যদি (!verify_end_of_সংখ্যা(tok, e, "decimal")) {
                        ফেরত MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    ফেরত MAKE_TOKEN(সংখ্যা);
                }
                c = tok_decimal_লেজ(tok);
                যদি (c == 0) {
                    ফেরত MAKE_TOKEN(ERRORTOKEN);
                }
            }
            যদি (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                যদি (!verify_end_of_সংখ্যা(tok, c, "imaginary")) {
                    ফেরত MAKE_TOKEN(ERRORTOKEN);
                }
            }
            অন্যথা যদি (!verify_end_of_সংখ্যা(tok, c, "decimal")) {
                ফেরত MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    ফেরত MAKE_TOKEN(সংখ্যা);
}
