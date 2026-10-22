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
    ܟܡܐ (1) {
        صحيح c = tok_nextc(tok);
        ܐܢ (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        ܐܚܪܢܐ ܐܢ (c == *s) {
            s++;
            استمر;
        }

        tok_backup(tok, c);
        ܟܡܐ (s != test) {
            tok_backup(tok, *--s);
        }
        ܦܢܐ res;
    }
}

ساكن صحيح
verify_end_of_number(بنية tok_state *tok, صحيح c, ثابت حرف *kind) {
    ܐܢ (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        ܦܢܐ 1;
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
    ܐܢ (c == 'a') {
        r = lookahead(tok, "nd");
    }
    ܐܚܪܢܐ ܐܢ (c == 'e') {
        r = lookahead(tok, "lse");
    }
    ܐܚܪܢܐ ܐܢ (c == 'f') {
        r = lookahead(tok, "or");
    }
    ܐܚܪܢܐ ܐܢ (c == 'i') {
        صحيح c2 = tok_nextc(tok);
        ܐܢ (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    ܐܚܪܢܐ ܐܢ (c == 'o') {
        r = lookahead(tok, "r");
    }
    ܐܚܪܢܐ ܐܢ (c == 'n') {
        r = lookahead(tok, "ot");
    }
    ܐܢ (r) {
        tok_backup(tok, c);
        ܐܢ (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            ܦܢܐ 0;
        }
        tok_nextc(tok);
    }
    ܐܚܪܢܐ /* In future releases, only error will remain. */
    ܐܢ (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        ܦܢܐ 0;
    }
    ܦܢܐ 1;
}

ساكن صحيح
tok_decimal_tail(بنية tok_state *tok)
{
    صحيح c;

    ܟܡܐ (1) {
        ܥܒܕ {
            c = tok_nextc(tok);
        } ܟܡܐ (Py_ISDIGIT(c));
        ܐܢ (c != '_') {
            اكسر;
        }
        c = tok_nextc(tok);
        ܐܢ (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            ܦܢܐ 0;
        }
    }
    ܦܢܐ c;
}

صحيح
_PyLexer_scan_number(بنية tok_state *tok, بنية token *token, صحيح c,
                     صحيح leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ܐܢ (leading_dot) {
        اذهب fraction;
    }
    ܐܢ (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        ܐܢ (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            ܥܒܕ {
                ܐܢ (c == '_') {
                    c = tok_nextc(tok);
                }
                ܐܢ (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    ܦܢܐ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                ܥܒܕ {
                    c = tok_nextc(tok);
                } ܟܡܐ (Py_ISXDIGIT(c));
            } ܟܡܐ (c == '_');
            ܐܢ (!verify_end_of_number(tok, c, "hexadecimal")) {
                ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ܐܚܪܢܐ ܐܢ (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            ܥܒܕ {
                ܐܢ (c == '_') {
                    c = tok_nextc(tok);
                }
                ܐܢ (c < '0' || c >= '8') {
                    ܐܢ (Py_ISDIGIT(c)) {
                        ܦܢܐ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    ܐܚܪܢܐ {
                        tok_backup(tok, c);
                        ܦܢܐ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                ܥܒܕ {
                    c = tok_nextc(tok);
                } ܟܡܐ ('0' <= c && c < '8');
            } ܟܡܐ (c == '_');
            ܐܢ (Py_ISDIGIT(c)) {
                ܦܢܐ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            ܐܢ (!verify_end_of_number(tok, c, "octal")) {
                ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ܐܚܪܢܐ ܐܢ (c == 'b' || c == 'B') {
            /* Binary */
            c = tok_nextc(tok);
            ܥܒܕ {
                ܐܢ (c == '_') {
                    c = tok_nextc(tok);
                }
                ܐܢ (c != '0' && c != '1') {
                    ܐܢ (Py_ISDIGIT(c)) {
                        ܦܢܐ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    ܐܚܪܢܐ {
                        tok_backup(tok, c);
                        ܦܢܐ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                ܥܒܕ {
                    c = tok_nextc(tok);
                } ܟܡܐ (c == '0' || c == '1');
            } ܟܡܐ (c == '_');
            ܐܢ (Py_ISDIGIT(c)) {
                ܦܢܐ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            ܐܢ (!verify_end_of_number(tok, c, "binary")) {
                ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ܐܚܪܢܐ {
            صحيح nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            ܟܡܐ (1) {
                ܐܢ (c == '_') {
                    c = tok_nextc(tok);
                    ܐܢ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ܦܢܐ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                ܐܢ (c != '0') {
                    اكسر;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            ܐܢ (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                ܐܢ (c == 0) {
                    ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ܐܢ (c == '.') {
                c = tok_nextc(tok);
                اذهب fraction;
            }
            ܐܚܪܢܐ ܐܢ (c == 'e' || c == 'E') {
                اذهب exponent;
            }
            ܐܚܪܢܐ ܐܢ (c == 'j' || c == 'J') {
                اذهب imaginary;
            }
            ܐܚܪܢܐ ܐܢ (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                ܦܢܐ MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (صحيح)(tok->start + 1 - tok->line_start),
                        (صحيح)(zeros_end - tok->line_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            ܐܢ (!verify_end_of_number(tok, c, "decimal")) {
                ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    ܐܚܪܢܐ {
        /* Decimal */
        c = tok_decimal_tail(tok);
        ܐܢ (c == 0) {
            ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            ܐܢ (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                ܐܢ (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    ܐܢ (c == 0) {
                        ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            ܐܢ (c == 'e' || c == 'E') {
                صحيح e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                ܐܢ (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    ܐܢ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ܦܢܐ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } ܐܚܪܢܐ ܐܢ (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    ܐܢ (!verify_end_of_number(tok, e, "decimal")) {
                        ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    ܦܢܐ MAKE_TOKEN(NUMBER);
                }
                c = tok_decimal_tail(tok);
                ܐܢ (c == 0) {
                    ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ܐܢ (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                ܐܢ (!verify_end_of_number(tok, c, "imaginary")) {
                    ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ܐܚܪܢܐ ܐܢ (!verify_end_of_number(tok, c, "decimal")) {
                ܦܢܐ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    ܦܢܐ MAKE_TOKEN(NUMBER);
}
