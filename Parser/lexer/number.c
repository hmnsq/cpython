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
    طالما (1) {
        صحيح c = tok_nextc(tok);
        𐡄𐡍 (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        وإلا 𐡄𐡍 (c == *s) {
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
verify_end_of_number(بنية tok_state *tok, صحيح c, ثابت حرف *kind) {
    𐡄𐡍 (tok->tok_extra_tokens) {
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
    𐡄𐡍 (c == 'a') {
        r = lookahead(tok, "nd");
    }
    وإلا 𐡄𐡍 (c == 'e') {
        r = lookahead(tok, "lse");
    }
    وإلا 𐡄𐡍 (c == 'f') {
        r = lookahead(tok, "or");
    }
    وإلا 𐡄𐡍 (c == 'i') {
        صحيح c2 = tok_nextc(tok);
        𐡄𐡍 (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    وإلا 𐡄𐡍 (c == 'o') {
        r = lookahead(tok, "r");
    }
    وإلا 𐡄𐡍 (c == 'n') {
        r = lookahead(tok, "ot");
    }
    𐡄𐡍 (r) {
        tok_backup(tok, c);
        𐡄𐡍 (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            أرجع 0;
        }
        tok_nextc(tok);
    }
    وإلا /* In future releases, only error will remain. */
    𐡄𐡍 (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        أرجع 0;
    }
    أرجع 1;
}

ساكن صحيح
tok_decimal_tail(بنية tok_state *tok)
{
    صحيح c;

    طالما (1) {
        𐡏𐡁𐡃 {
            c = tok_nextc(tok);
        } طالما (Py_ISDIGIT(c));
        𐡄𐡍 (c != '_') {
            اكسر;
        }
        c = tok_nextc(tok);
        𐡄𐡍 (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            أرجع 0;
        }
    }
    أرجع c;
}

صحيح
_PyLexer_scan_number(بنية tok_state *tok, بنية token *token, صحيح c,
                     صحيح leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    𐡄𐡍 (leading_dot) {
        اذهب fraction;
    }
    𐡄𐡍 (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        𐡄𐡍 (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            𐡏𐡁𐡃 {
                𐡄𐡍 (c == '_') {
                    c = tok_nextc(tok);
                }
                𐡄𐡍 (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                𐡏𐡁𐡃 {
                    c = tok_nextc(tok);
                } طالما (Py_ISXDIGIT(c));
            } طالما (c == '_');
            𐡄𐡍 (!verify_end_of_number(tok, c, "hexadecimal")) {
                أرجع MAKE_TOKEN(ERRORTOKEN);
            }
        }
        وإلا 𐡄𐡍 (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            𐡏𐡁𐡃 {
                𐡄𐡍 (c == '_') {
                    c = tok_nextc(tok);
                }
                𐡄𐡍 (c < '0' || c >= '8') {
                    𐡄𐡍 (Py_ISDIGIT(c)) {
                        أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    وإلا {
                        tok_backup(tok, c);
                        أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                𐡏𐡁𐡃 {
                    c = tok_nextc(tok);
                } طالما ('0' <= c && c < '8');
            } طالما (c == '_');
            𐡄𐡍 (Py_ISDIGIT(c)) {
                أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            𐡄𐡍 (!verify_end_of_number(tok, c, "octal")) {
                أرجع MAKE_TOKEN(ERRORTOKEN);
            }
        }
        وإلا 𐡄𐡍 (c == 'b' || c == 'B') {
            /* Binary */
            c = tok_nextc(tok);
            𐡏𐡁𐡃 {
                𐡄𐡍 (c == '_') {
                    c = tok_nextc(tok);
                }
                𐡄𐡍 (c != '0' && c != '1') {
                    𐡄𐡍 (Py_ISDIGIT(c)) {
                        أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    وإلا {
                        tok_backup(tok, c);
                        أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                𐡏𐡁𐡃 {
                    c = tok_nextc(tok);
                } طالما (c == '0' || c == '1');
            } طالما (c == '_');
            𐡄𐡍 (Py_ISDIGIT(c)) {
                أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            𐡄𐡍 (!verify_end_of_number(tok, c, "binary")) {
                أرجع MAKE_TOKEN(ERRORTOKEN);
            }
        }
        وإلا {
            صحيح nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            طالما (1) {
                𐡄𐡍 (c == '_') {
                    c = tok_nextc(tok);
                    𐡄𐡍 (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                𐡄𐡍 (c != '0') {
                    اكسر;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            𐡄𐡍 (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                𐡄𐡍 (c == 0) {
                    أرجع MAKE_TOKEN(ERRORTOKEN);
                }
            }
            𐡄𐡍 (c == '.') {
                c = tok_nextc(tok);
                اذهب fraction;
            }
            وإلا 𐡄𐡍 (c == 'e' || c == 'E') {
                اذهب exponent;
            }
            وإلا 𐡄𐡍 (c == 'j' || c == 'J') {
                اذهب imaginary;
            }
            وإلا 𐡄𐡍 (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (صحيح)(tok->start + 1 - tok->line_start),
                        (صحيح)(zeros_end - tok->line_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            𐡄𐡍 (!verify_end_of_number(tok, c, "decimal")) {
                أرجع MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    وإلا {
        /* Decimal */
        c = tok_decimal_tail(tok);
        𐡄𐡍 (c == 0) {
            أرجع MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            𐡄𐡍 (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                𐡄𐡍 (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    𐡄𐡍 (c == 0) {
                        أرجع MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            𐡄𐡍 (c == 'e' || c == 'E') {
                صحيح e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                𐡄𐡍 (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    𐡄𐡍 (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } وإلا 𐡄𐡍 (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    𐡄𐡍 (!verify_end_of_number(tok, e, "decimal")) {
                        أرجع MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    أرجع MAKE_TOKEN(NUMBER);
                }
                c = tok_decimal_tail(tok);
                𐡄𐡍 (c == 0) {
                    أرجع MAKE_TOKEN(ERRORTOKEN);
                }
            }
            𐡄𐡍 (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                𐡄𐡍 (!verify_end_of_number(tok, c, "imaginary")) {
                    أرجع MAKE_TOKEN(ERRORTOKEN);
                }
            }
            وإلا 𐡄𐡍 (!verify_end_of_number(tok, c, "decimal")) {
                أرجع MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    أرجع MAKE_TOKEN(NUMBER);
}
