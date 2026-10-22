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
    ᨀᨛ (1) {
        puur_naa_nka c = tok_nextc(tok);
        ᨀᨘ (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        ᨀᨘᨁ ᨀᨘ (c == *s) {
            s++;
            continue;
        }

        tok_backup(tok, c);
        ᨀᨛ (s != test) {
            tok_backup(tok, *--s);
        }
        ᨀᨘᨒ res;
    }
}

static puur_naa_nka
verify_end_of_number(sa_nracanaa tok_state *tok, puur_naa_nka c, const var_na *kind) {
    ᨀᨘ (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        ᨀᨘᨒ 1;
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
    ᨀᨘ (c == 'a') {
        r = lookahead(tok, "nd");
    }
    ᨀᨘᨁ ᨀᨘ (c == 'e') {
        r = lookahead(tok, "lse");
    }
    ᨀᨘᨁ ᨀᨘ (c == 'f') {
        r = lookahead(tok, "or");
    }
    ᨀᨘᨁ ᨀᨘ (c == 'i') {
        puur_naa_nka c2 = tok_nextc(tok);
        ᨀᨘ (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    ᨀᨘᨁ ᨀᨘ (c == 'o') {
        r = lookahead(tok, "r");
    }
    ᨀᨘᨁ ᨀᨘ (c == 'n') {
        r = lookahead(tok, "ot");
    }
    ᨀᨘ (r) {
        tok_backup(tok, c);
        ᨀᨘ (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            ᨀᨘᨒ 0;
        }
        tok_nextc(tok);
    }
    ᨀᨘᨁ /* In future releases, only error will remain. */
    ᨀᨘ (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        ᨀᨘᨒ 0;
    }
    ᨀᨘᨒ 1;
}

static puur_naa_nka
tok_decimal_tail(sa_nracanaa tok_state *tok)
{
    puur_naa_nka c;

    ᨀᨛ (1) {
        do {
            c = tok_nextc(tok);
        } ᨀᨛ (Py_ISDIGIT(c));
        ᨀᨘ (c != '_') {
            bha_nga;
        }
        c = tok_nextc(tok);
        ᨀᨘ (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            ᨀᨘᨒ 0;
        }
    }
    ᨀᨘᨒ c;
}

puur_naa_nka
_PyLexer_scan_number(sa_nracanaa tok_state *tok, sa_nracanaa token *token, puur_naa_nka c,
                     puur_naa_nka leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ᨀᨘ (leading_dot) {
        goto fraction;
    }
    ᨀᨘ (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        ᨀᨘ (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            do {
                ᨀᨘ (c == '_') {
                    c = tok_nextc(tok);
                }
                ᨀᨘ (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    ᨀᨘᨒ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                do {
                    c = tok_nextc(tok);
                } ᨀᨛ (Py_ISXDIGIT(c));
            } ᨀᨛ (c == '_');
            ᨀᨘ (!verify_end_of_number(tok, c, "hexadecimal")) {
                ᨀᨘᨒ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ᨀᨘᨁ ᨀᨘ (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            do {
                ᨀᨘ (c == '_') {
                    c = tok_nextc(tok);
                }
                ᨀᨘ (c < '0' || c >= '8') {
                    ᨀᨘ (Py_ISDIGIT(c)) {
                        ᨀᨘᨒ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    ᨀᨘᨁ {
                        tok_backup(tok, c);
                        ᨀᨘᨒ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } ᨀᨛ ('0' <= c && c < '8');
            } ᨀᨛ (c == '_');
            ᨀᨘ (Py_ISDIGIT(c)) {
                ᨀᨘᨒ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            ᨀᨘ (!verify_end_of_number(tok, c, "octal")) {
                ᨀᨘᨒ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ᨀᨘᨁ ᨀᨘ (c == 'b' || c == 'B') {
            /* Binary */
            c = tok_nextc(tok);
            do {
                ᨀᨘ (c == '_') {
                    c = tok_nextc(tok);
                }
                ᨀᨘ (c != '0' && c != '1') {
                    ᨀᨘ (Py_ISDIGIT(c)) {
                        ᨀᨘᨒ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    ᨀᨘᨁ {
                        tok_backup(tok, c);
                        ᨀᨘᨒ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } ᨀᨛ (c == '0' || c == '1');
            } ᨀᨛ (c == '_');
            ᨀᨘ (Py_ISDIGIT(c)) {
                ᨀᨘᨒ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            ᨀᨘ (!verify_end_of_number(tok, c, "binary")) {
                ᨀᨘᨒ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ᨀᨘᨁ {
            puur_naa_nka nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            ᨀᨛ (1) {
                ᨀᨘ (c == '_') {
                    c = tok_nextc(tok);
                    ᨀᨘ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ᨀᨘᨒ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                ᨀᨘ (c != '0') {
                    bha_nga;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            ᨀᨘ (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                ᨀᨘ (c == 0) {
                    ᨀᨘᨒ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ᨀᨘ (c == '.') {
                c = tok_nextc(tok);
                goto fraction;
            }
            ᨀᨘᨁ ᨀᨘ (c == 'e' || c == 'E') {
                goto exponent;
            }
            ᨀᨘᨁ ᨀᨘ (c == 'j' || c == 'J') {
                goto imaginary;
            }
            ᨀᨘᨁ ᨀᨘ (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                ᨀᨘᨒ MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (puur_naa_nka)(tok->start + 1 - tok->line_start),
                        (puur_naa_nka)(zeros_end - tok->line_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            ᨀᨘ (!verify_end_of_number(tok, c, "decimal")) {
                ᨀᨘᨒ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    ᨀᨘᨁ {
        /* Decimal */
        c = tok_decimal_tail(tok);
        ᨀᨘ (c == 0) {
            ᨀᨘᨒ MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            ᨀᨘ (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                ᨀᨘ (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    ᨀᨘ (c == 0) {
                        ᨀᨘᨒ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            ᨀᨘ (c == 'e' || c == 'E') {
                puur_naa_nka e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                ᨀᨘ (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    ᨀᨘ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ᨀᨘᨒ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } ᨀᨘᨁ ᨀᨘ (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    ᨀᨘ (!verify_end_of_number(tok, e, "decimal")) {
                        ᨀᨘᨒ MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    ᨀᨘᨒ MAKE_TOKEN(NUMBER);
                }
                c = tok_decimal_tail(tok);
                ᨀᨘ (c == 0) {
                    ᨀᨘᨒ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ᨀᨘ (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                ᨀᨘ (!verify_end_of_number(tok, c, "imaginary")) {
                    ᨀᨘᨒ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ᨀᨘᨁ ᨀᨘ (!verify_end_of_number(tok, c, "decimal")) {
                ᨀᨘᨒ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    ᨀᨘᨒ MAKE_TOKEN(NUMBER);
}
