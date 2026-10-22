#অন্তর্ভাৱ "Python.h"
#অন্তর্ভাৱ "pycore_token.h"

#অন্তর্ভাৱ "lexer_internal.h"
#অন্তর্ভাৱ "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

স্থির পূর্ণাঙ্ক
lookahead(সংরচনা tok_state *tok, ধ্রুৱ ৱর্ণ *test)
{
    ধ্রুৱ ৱর্ণ *s = test;
    পূর্ণাঙ্ক res = 0;
    যেতিয়ালৈ (1) {
        পূর্ণাঙ্ক c = tok_nextc(tok);
        যদি (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        অন্যথা যদি (c == *s) {
            s++;
            অনুৱর্ত;
        }

        tok_backup(tok, c);
        যেতিয়ালৈ (s != test) {
            tok_backup(tok, *--s);
        }
        ঘূৰাই res;
    }
}

স্থির পূর্ণাঙ্ক
verify_end_of_সংখ্যা(সংরচনা tok_state *tok, পূর্ণাঙ্ক c, ধ্রুৱ ৱর্ণ *kind) {
    যদি (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        ঘূৰাই 1;
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
    পূর্ণাঙ্ক r = 0;
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
        পূর্ণাঙ্ক c2 = tok_nextc(tok);
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
            ঘূৰাই 0;
        }
        tok_nextc(tok);
    }
    অন্যথা /* In future releases, only error will remain. */
    যদি (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        ঘূৰাই 0;
    }
    ঘূৰাই 1;
}

স্থির পূর্ণাঙ্ক
tok_decimal_tail(সংরচনা tok_state *tok)
{
    পূর্ণাঙ্ক c;

    যেতিয়ালৈ (1) {
        কৰা {
            c = tok_nextc(tok);
        } যেতিয়ালৈ (Py_ISDIGIT(c));
        যদি (c != '_') {
            ভঙ্গ;
        }
        c = tok_nextc(tok);
        যদি (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            ঘূৰাই 0;
        }
    }
    ঘূৰাই c;
}

পূর্ণাঙ্ক
Py_Lexer_scan_সংখ্যা(সংরচনা tok_state *tok, সংরচনা token *token, পূর্ণাঙ্ক c,
                     পূর্ণাঙ্ক leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    যদি (leading_dot) {
        গচ্ছ fraction;
    }
    যদি (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        যদি (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            কৰা {
                যদি (c == '_') {
                    c = tok_nextc(tok);
                }
                যদি (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    ঘূৰাই MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                কৰা {
                    c = tok_nextc(tok);
                } যেতিয়ালৈ (Py_ISXDIGIT(c));
            } যেতিয়ালৈ (c == '_');
            যদি (!verify_end_of_সংখ্যা(tok, c, "hexadecimal")) {
                ঘূৰাই MAKE_TOKEN(ERRORTOKEN);
            }
        }
        অন্যথা যদি (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            কৰা {
                যদি (c == '_') {
                    c = tok_nextc(tok);
                }
                যদি (c < '0' || c >= '8') {
                    যদি (Py_ISDIGIT(c)) {
                        ঘূৰাই MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    অন্যথা {
                        tok_backup(tok, c);
                        ঘূৰাই MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                কৰা {
                    c = tok_nextc(tok);
                } যেতিয়ালৈ ('0' <= c && c < '8');
            } যেতিয়ালৈ (c == '_');
            যদি (Py_ISDIGIT(c)) {
                ঘূৰাই MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            যদি (!verify_end_of_সংখ্যা(tok, c, "octal")) {
                ঘূৰাই MAKE_TOKEN(ERRORTOKEN);
            }
        }
        অন্যথা যদি (c == 'b' || c == 'B') {
            /*দ্বিমিক*/
            c = tok_nextc(tok);
            কৰা {
                যদি (c == '_') {
                    c = tok_nextc(tok);
                }
                যদি (c != '0' && c != '1') {
                    যদি (Py_ISDIGIT(c)) {
                        ঘূৰাই MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    অন্যথা {
                        tok_backup(tok, c);
                        ঘূৰাই MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                কৰা {
                    c = tok_nextc(tok);
                } যেতিয়ালৈ (c == '0' || c == '1');
            } যেতিয়ালৈ (c == '_');
            যদি (Py_ISDIGIT(c)) {
                ঘূৰাই MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            যদি (!verify_end_of_সংখ্যা(tok, c, "binary")) {
                ঘূৰাই MAKE_TOKEN(ERRORTOKEN);
            }
        }
        অন্যথা {
            পূর্ণাঙ্ক nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            যেতিয়ালৈ (1) {
                যদি (c == '_') {
                    c = tok_nextc(tok);
                    যদি (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ঘূৰাই MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                যদি (c != '0') {
                    ভঙ্গ;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            যদি (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                যদি (c == 0) {
                    ঘূৰাই MAKE_TOKEN(ERRORTOKEN);
                }
            }
            যদি (c == '.') {
                c = tok_nextc(tok);
                গচ্ছ fraction;
            }
            অন্যথা যদি (c == 'e' || c == 'E') {
                গচ্ছ exponent;
            }
            অন্যথা যদি (c == 'j' || c == 'J') {
                গচ্ছ imaginary;
            }
            অন্যথা যদি (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                ঘূৰাই MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (পূর্ণাঙ্ক)(tok->start + 1 - tok->সাৰি_start),
                        (পূর্ণাঙ্ক)(zeros_end - tok->সাৰি_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            যদি (!verify_end_of_সংখ্যা(tok, c, "decimal")) {
                ঘূৰাই MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    অন্যথা {
        /* Decimal */
        c = tok_decimal_tail(tok);
        যদি (c == 0) {
            ঘূৰাই MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            যদি (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                যদি (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    যদি (c == 0) {
                        ঘূৰাই MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            যদি (c == 'e' || c == 'E') {
                পূর্ণাঙ্ক e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                যদি (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    যদি (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ঘূৰাই MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } অন্যথা যদি (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    যদি (!verify_end_of_সংখ্যা(tok, e, "decimal")) {
                        ঘূৰাই MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    ঘূৰাই MAKE_TOKEN(সংখ্যা);
                }
                c = tok_decimal_tail(tok);
                যদি (c == 0) {
                    ঘূৰাই MAKE_TOKEN(ERRORTOKEN);
                }
            }
            যদি (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                যদি (!verify_end_of_সংখ্যা(tok, c, "imaginary")) {
                    ঘূৰাই MAKE_TOKEN(ERRORTOKEN);
                }
            }
            অন্যথা যদি (!verify_end_of_সংখ্যা(tok, c, "decimal")) {
                ঘূৰাই MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    ঘূৰাই MAKE_TOKEN(সংখ্যা);
}
