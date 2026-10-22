#include "Python.h"
#include "pycore_token.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

static ចំនួនគត់
lookahead(sa_nracanaa tok_state *tok, const var_na *test)
{
    const var_na *s = test;
    ចំនួនគត់ res = 0;
    ខណៈពេល (1) {
        ចំនួនគត់ c = tok_nextc(tok);
        ប្រសិនបើ (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        បើមិនដូច្នេះ ប្រសិនបើ (c == *s) {
            s++;
            continue;
        }

        tok_backup(tok, c);
        ខណៈពេល (s != test) {
            tok_backup(tok, *--s);
        }
        ត្រឡប់ res;
    }
}

static ចំនួនគត់
verify_end_of_number(sa_nracanaa tok_state *tok, ចំនួនគត់ c, const var_na *kind) {
    ប្រសិនបើ (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        ត្រឡប់ 1;
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
    ចំនួនគត់ r = 0;
    ប្រសិនបើ (c == 'a') {
        r = lookahead(tok, "nd");
    }
    បើមិនដូច្នេះ ប្រសិនបើ (c == 'e') {
        r = lookahead(tok, "lse");
    }
    បើមិនដូច្នេះ ប្រសិនបើ (c == 'f') {
        r = lookahead(tok, "or");
    }
    បើមិនដូច្នេះ ប្រសិនបើ (c == 'i') {
        ចំនួនគត់ c2 = tok_nextc(tok);
        ប្រសិនបើ (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (c == 'o') {
        r = lookahead(tok, "r");
    }
    បើមិនដូច្នេះ ប្រសិនបើ (c == 'n') {
        r = lookahead(tok, "ot");
    }
    ប្រសិនបើ (r) {
        tok_backup(tok, c);
        ប្រសិនបើ (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            ត្រឡប់ 0;
        }
        tok_nextc(tok);
    }
    បើមិនដូច្នេះ /* In future releases, only error will remain. */
    ប្រសិនបើ (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        ត្រឡប់ 0;
    }
    ត្រឡប់ 1;
}

static ចំនួនគត់
tok_decimal_tail(sa_nracanaa tok_state *tok)
{
    ចំនួនគត់ c;

    ខណៈពេល (1) {
        do {
            c = tok_nextc(tok);
        } ខណៈពេល (Py_ISDIGIT(c));
        ប្រសិនបើ (c != '_') {
            bha_nga;
        }
        c = tok_nextc(tok);
        ប្រសិនបើ (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            ត្រឡប់ 0;
        }
    }
    ត្រឡប់ c;
}

ចំនួនគត់
_PyLexer_scan_number(sa_nracanaa tok_state *tok, sa_nracanaa token *token, ចំនួនគត់ c,
                     ចំនួនគត់ leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ប្រសិនបើ (leading_dot) {
        goto fraction;
    }
    ប្រសិនបើ (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        ប្រសិនបើ (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            do {
                ប្រសិនបើ (c == '_') {
                    c = tok_nextc(tok);
                }
                ប្រសិនបើ (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    ត្រឡប់ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                do {
                    c = tok_nextc(tok);
                } ខណៈពេល (Py_ISXDIGIT(c));
            } ខណៈពេល (c == '_');
            ប្រសិនបើ (!verify_end_of_number(tok, c, "hexadecimal")) {
                ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        បើមិនដូច្នេះ ប្រសិនបើ (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            do {
                ប្រសិនបើ (c == '_') {
                    c = tok_nextc(tok);
                }
                ប្រសិនបើ (c < '0' || c >= '8') {
                    ប្រសិនបើ (Py_ISDIGIT(c)) {
                        ត្រឡប់ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    បើមិនដូច្នេះ {
                        tok_backup(tok, c);
                        ត្រឡប់ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } ខណៈពេល ('0' <= c && c < '8');
            } ខណៈពេល (c == '_');
            ប្រសិនបើ (Py_ISDIGIT(c)) {
                ត្រឡប់ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            ប្រសិនបើ (!verify_end_of_number(tok, c, "octal")) {
                ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        បើមិនដូច្នេះ ប្រសិនបើ (c == 'b' || c == 'B') {
            /* Binary */
            c = tok_nextc(tok);
            do {
                ប្រសិនបើ (c == '_') {
                    c = tok_nextc(tok);
                }
                ប្រសិនបើ (c != '0' && c != '1') {
                    ប្រសិនបើ (Py_ISDIGIT(c)) {
                        ត្រឡប់ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    បើមិនដូច្នេះ {
                        tok_backup(tok, c);
                        ត្រឡប់ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } ខណៈពេល (c == '0' || c == '1');
            } ខណៈពេល (c == '_');
            ប្រសិនបើ (Py_ISDIGIT(c)) {
                ត្រឡប់ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            ប្រសិនបើ (!verify_end_of_number(tok, c, "binary")) {
                ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        បើមិនដូច្នេះ {
            ចំនួនគត់ nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            ខណៈពេល (1) {
                ប្រសិនបើ (c == '_') {
                    c = tok_nextc(tok);
                    ប្រសិនបើ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ត្រឡប់ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                ប្រសិនបើ (c != '0') {
                    bha_nga;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            ប្រសិនបើ (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                ប្រសិនបើ (c == 0) {
                    ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ប្រសិនបើ (c == '.') {
                c = tok_nextc(tok);
                goto fraction;
            }
            បើមិនដូច្នេះ ប្រសិនបើ (c == 'e' || c == 'E') {
                goto exponent;
            }
            បើមិនដូច្នេះ ប្រសិនបើ (c == 'j' || c == 'J') {
                goto imaginary;
            }
            បើមិនដូច្នេះ ប្រសិនបើ (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                ត្រឡប់ MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (ចំនួនគត់)(tok->start + 1 - tok->line_start),
                        (ចំនួនគត់)(zeros_end - tok->line_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            ប្រសិនបើ (!verify_end_of_number(tok, c, "decimal")) {
                ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    បើមិនដូច្នេះ {
        /* Decimal */
        c = tok_decimal_tail(tok);
        ប្រសិនបើ (c == 0) {
            ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            ប្រសិនបើ (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                ប្រសិនបើ (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    ប្រសិនបើ (c == 0) {
                        ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            ប្រសិនបើ (c == 'e' || c == 'E') {
                ចំនួនគត់ e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                ប្រសិនបើ (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    ប្រសិនបើ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ត្រឡប់ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } បើមិនដូច្នេះ ប្រសិនបើ (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    ប្រសិនបើ (!verify_end_of_number(tok, e, "decimal")) {
                        ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    ត្រឡប់ MAKE_TOKEN(NUMBER);
                }
                c = tok_decimal_tail(tok);
                ប្រសិនបើ (c == 0) {
                    ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ប្រសិនបើ (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                ប្រសិនបើ (!verify_end_of_number(tok, c, "imaginary")) {
                    ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            បើមិនដូច្នេះ ប្រសិនបើ (!verify_end_of_number(tok, c, "decimal")) {
                ត្រឡប់ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    ត្រឡប់ MAKE_TOKEN(NUMBER);
}
