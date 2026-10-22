#include "Python.h"
#include "pycore_token.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

static puur_naa_nka
lookahead(sa_nracanaa tok_state *tok, const var_na *test)
{
    const var_na *s = test;
    puur_naa_nka res = 0;
    ᰀᰬ (1) {
        puur_naa_nka c = tok_nextc(tok);
        ᰀᰧ (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        ᰀᰧᰀ ᰀᰧ (c == *s) {
            s++;
            continue;
        }

        tok_backup(tok, c);
        ᰀᰬ (s != test) {
            tok_backup(tok, *--s);
        }
        ᰀᰧᰌ res;
    }
}

static puur_naa_nka
verify_end_of_number(sa_nracanaa tok_state *tok, puur_naa_nka c, const var_na *kind) {
    ᰀᰧ (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        ᰀᰧᰌ 1;
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
    puur_naa_nka r = 0;
    ᰀᰧ (c == 'a') {
        r = lookahead(tok, "nd");
    }
    ᰀᰧᰀ ᰀᰧ (c == 'e') {
        r = lookahead(tok, "lse");
    }
    ᰀᰧᰀ ᰀᰧ (c == 'f') {
        r = lookahead(tok, "or");
    }
    ᰀᰧᰀ ᰀᰧ (c == 'i') {
        puur_naa_nka c2 = tok_nextc(tok);
        ᰀᰧ (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    ᰀᰧᰀ ᰀᰧ (c == 'o') {
        r = lookahead(tok, "r");
    }
    ᰀᰧᰀ ᰀᰧ (c == 'n') {
        r = lookahead(tok, "ot");
    }
    ᰀᰧ (r) {
        tok_backup(tok, c);
        ᰀᰧ (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            ᰀᰧᰌ 0;
        }
        tok_nextc(tok);
    }
    ᰀᰧᰀ /* In future releases, only error will remain. */
    ᰀᰧ (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        ᰀᰧᰌ 0;
    }
    ᰀᰧᰌ 1;
}

static puur_naa_nka
tok_decimal_tail(sa_nracanaa tok_state *tok)
{
    puur_naa_nka c;

    ᰀᰬ (1) {
        do {
            c = tok_nextc(tok);
        } ᰀᰬ (Py_ISDIGIT(c));
        ᰀᰧ (c != '_') {
            bha_nga;
        }
        c = tok_nextc(tok);
        ᰀᰧ (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            ᰀᰧᰌ 0;
        }
    }
    ᰀᰧᰌ c;
}

puur_naa_nka
_PyLexer_scan_number(sa_nracanaa tok_state *tok, sa_nracanaa token *token, puur_naa_nka c,
                     puur_naa_nka leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ᰀᰧ (leading_dot) {
        goto fraction;
    }
    ᰀᰧ (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        ᰀᰧ (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            do {
                ᰀᰧ (c == '_') {
                    c = tok_nextc(tok);
                }
                ᰀᰧ (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    ᰀᰧᰌ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                do {
                    c = tok_nextc(tok);
                } ᰀᰬ (Py_ISXDIGIT(c));
            } ᰀᰬ (c == '_');
            ᰀᰧ (!verify_end_of_number(tok, c, "hexadecimal")) {
                ᰀᰧᰌ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ᰀᰧᰀ ᰀᰧ (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            do {
                ᰀᰧ (c == '_') {
                    c = tok_nextc(tok);
                }
                ᰀᰧ (c < '0' || c >= '8') {
                    ᰀᰧ (Py_ISDIGIT(c)) {
                        ᰀᰧᰌ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    ᰀᰧᰀ {
                        tok_backup(tok, c);
                        ᰀᰧᰌ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } ᰀᰬ ('0' <= c && c < '8');
            } ᰀᰬ (c == '_');
            ᰀᰧ (Py_ISDIGIT(c)) {
                ᰀᰧᰌ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            ᰀᰧ (!verify_end_of_number(tok, c, "octal")) {
                ᰀᰧᰌ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ᰀᰧᰀ ᰀᰧ (c == 'b' || c == 'B') {
            /* Binary */
            c = tok_nextc(tok);
            do {
                ᰀᰧ (c == '_') {
                    c = tok_nextc(tok);
                }
                ᰀᰧ (c != '0' && c != '1') {
                    ᰀᰧ (Py_ISDIGIT(c)) {
                        ᰀᰧᰌ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    ᰀᰧᰀ {
                        tok_backup(tok, c);
                        ᰀᰧᰌ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } ᰀᰬ (c == '0' || c == '1');
            } ᰀᰬ (c == '_');
            ᰀᰧ (Py_ISDIGIT(c)) {
                ᰀᰧᰌ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            ᰀᰧ (!verify_end_of_number(tok, c, "binary")) {
                ᰀᰧᰌ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ᰀᰧᰀ {
            puur_naa_nka nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            ᰀᰬ (1) {
                ᰀᰧ (c == '_') {
                    c = tok_nextc(tok);
                    ᰀᰧ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ᰀᰧᰌ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                ᰀᰧ (c != '0') {
                    bha_nga;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            ᰀᰧ (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                ᰀᰧ (c == 0) {
                    ᰀᰧᰌ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ᰀᰧ (c == '.') {
                c = tok_nextc(tok);
                goto fraction;
            }
            ᰀᰧᰀ ᰀᰧ (c == 'e' || c == 'E') {
                goto exponent;
            }
            ᰀᰧᰀ ᰀᰧ (c == 'j' || c == 'J') {
                goto imaginary;
            }
            ᰀᰧᰀ ᰀᰧ (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                ᰀᰧᰌ MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (puur_naa_nka)(tok->start + 1 - tok->line_start),
                        (puur_naa_nka)(zeros_end - tok->line_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            ᰀᰧ (!verify_end_of_number(tok, c, "decimal")) {
                ᰀᰧᰌ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    ᰀᰧᰀ {
        /* Decimal */
        c = tok_decimal_tail(tok);
        ᰀᰧ (c == 0) {
            ᰀᰧᰌ MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            ᰀᰧ (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                ᰀᰧ (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    ᰀᰧ (c == 0) {
                        ᰀᰧᰌ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            ᰀᰧ (c == 'e' || c == 'E') {
                puur_naa_nka e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                ᰀᰧ (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    ᰀᰧ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ᰀᰧᰌ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } ᰀᰧᰀ ᰀᰧ (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    ᰀᰧ (!verify_end_of_number(tok, e, "decimal")) {
                        ᰀᰧᰌ MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    ᰀᰧᰌ MAKE_TOKEN(NUMBER);
                }
                c = tok_decimal_tail(tok);
                ᰀᰧ (c == 0) {
                    ᰀᰧᰌ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ᰀᰧ (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                ᰀᰧ (!verify_end_of_number(tok, c, "imaginary")) {
                    ᰀᰧᰌ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ᰀᰧᰀ ᰀᰧ (!verify_end_of_number(tok, c, "decimal")) {
                ᰀᰧᰌ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    ᰀᰧᰌ MAKE_TOKEN(NUMBER);
}
