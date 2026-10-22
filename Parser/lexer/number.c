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
    כלעוד (1) {
        صحيح c = tok_nextc(tok);
        אם (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        אחרת אם (c == *s) {
            s++;
            استمر;
        }

        tok_backup(tok, c);
        כלעוד (s != test) {
            tok_backup(tok, *--s);
        }
        החזר res;
    }
}

ساكن صحيح
verify_end_of_number(بنية tok_state *tok, صحيح c, ثابت حرف *kind) {
    אם (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        החזר 1;
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
    אם (c == 'a') {
        r = lookahead(tok, "nd");
    }
    אחרת אם (c == 'e') {
        r = lookahead(tok, "lse");
    }
    אחרת אם (c == 'f') {
        r = lookahead(tok, "or");
    }
    אחרת אם (c == 'i') {
        صحيح c2 = tok_nextc(tok);
        אם (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    אחרת אם (c == 'o') {
        r = lookahead(tok, "r");
    }
    אחרת אם (c == 'n') {
        r = lookahead(tok, "ot");
    }
    אם (r) {
        tok_backup(tok, c);
        אם (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            החזר 0;
        }
        tok_nextc(tok);
    }
    אחרת /* In future releases, only error will remain. */
    אם (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        החזר 0;
    }
    החזר 1;
}

ساكن صحيح
tok_decimal_tail(بنية tok_state *tok)
{
    صحيح c;

    כלעוד (1) {
        עשה {
            c = tok_nextc(tok);
        } כלעוד (Py_ISDIGIT(c));
        אם (c != '_') {
            اكسر;
        }
        c = tok_nextc(tok);
        אם (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            החזר 0;
        }
    }
    החזר c;
}

صحيح
_PyLexer_scan_number(بنية tok_state *tok, بنية token *token, صحيح c,
                     صحيح leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    אם (leading_dot) {
        اذهب fraction;
    }
    אם (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        אם (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            עשה {
                אם (c == '_') {
                    c = tok_nextc(tok);
                }
                אם (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    החזר MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                עשה {
                    c = tok_nextc(tok);
                } כלעוד (Py_ISXDIGIT(c));
            } כלעוד (c == '_');
            אם (!verify_end_of_number(tok, c, "hexadecimal")) {
                החזר MAKE_TOKEN(ERRORTOKEN);
            }
        }
        אחרת אם (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            עשה {
                אם (c == '_') {
                    c = tok_nextc(tok);
                }
                אם (c < '0' || c >= '8') {
                    אם (Py_ISDIGIT(c)) {
                        החזר MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    אחרת {
                        tok_backup(tok, c);
                        החזר MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                עשה {
                    c = tok_nextc(tok);
                } כלעוד ('0' <= c && c < '8');
            } כלעוד (c == '_');
            אם (Py_ISDIGIT(c)) {
                החזר MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            אם (!verify_end_of_number(tok, c, "octal")) {
                החזר MAKE_TOKEN(ERRORTOKEN);
            }
        }
        אחרת אם (c == 'b' || c == 'B') {
            /* Binary */
            c = tok_nextc(tok);
            עשה {
                אם (c == '_') {
                    c = tok_nextc(tok);
                }
                אם (c != '0' && c != '1') {
                    אם (Py_ISDIGIT(c)) {
                        החזר MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    אחרת {
                        tok_backup(tok, c);
                        החזר MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                עשה {
                    c = tok_nextc(tok);
                } כלעוד (c == '0' || c == '1');
            } כלעוד (c == '_');
            אם (Py_ISDIGIT(c)) {
                החזר MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            אם (!verify_end_of_number(tok, c, "binary")) {
                החזר MAKE_TOKEN(ERRORTOKEN);
            }
        }
        אחרת {
            صحيح nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            כלעוד (1) {
                אם (c == '_') {
                    c = tok_nextc(tok);
                    אם (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        החזר MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                אם (c != '0') {
                    اكسر;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            אם (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                אם (c == 0) {
                    החזר MAKE_TOKEN(ERRORTOKEN);
                }
            }
            אם (c == '.') {
                c = tok_nextc(tok);
                اذهب fraction;
            }
            אחרת אם (c == 'e' || c == 'E') {
                اذهب exponent;
            }
            אחרת אם (c == 'j' || c == 'J') {
                اذهب imaginary;
            }
            אחרת אם (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                החזר MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (صحيح)(tok->start + 1 - tok->line_start),
                        (صحيح)(zeros_end - tok->line_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            אם (!verify_end_of_number(tok, c, "decimal")) {
                החזר MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    אחרת {
        /* Decimal */
        c = tok_decimal_tail(tok);
        אם (c == 0) {
            החזר MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            אם (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                אם (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    אם (c == 0) {
                        החזר MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            אם (c == 'e' || c == 'E') {
                صحيح e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                אם (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    אם (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        החזר MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } אחרת אם (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    אם (!verify_end_of_number(tok, e, "decimal")) {
                        החזר MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    החזר MAKE_TOKEN(NUMBER);
                }
                c = tok_decimal_tail(tok);
                אם (c == 0) {
                    החזר MAKE_TOKEN(ERRORTOKEN);
                }
            }
            אם (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                אם (!verify_end_of_number(tok, c, "imaginary")) {
                    החזר MAKE_TOKEN(ERRORTOKEN);
                }
            }
            אחרת אם (!verify_end_of_number(tok, c, "decimal")) {
                החזר MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    החזר MAKE_TOKEN(NUMBER);
}
