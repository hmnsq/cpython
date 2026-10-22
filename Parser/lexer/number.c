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
    ᯀᯬᯝ (1) {
        puur_naa_nka c = tok_nextc(tok);
        ᯀᯮᯝ (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ (c == *s) {
            s++;
            continue;
        }

        tok_backup(tok, c);
        ᯀᯬᯝ (s != test) {
            tok_backup(tok, *--s);
        }
        ᯀᯮᯞ res;
    }
}

static puur_naa_nka
verify_end_of_number(sa_nracanaa tok_state *tok, puur_naa_nka c, const var_na *kind) {
    ᯀᯮᯝ (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        ᯀᯮᯞ 1;
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
    ᯀᯮᯝ (c == 'a') {
        r = lookahead(tok, "nd");
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (c == 'e') {
        r = lookahead(tok, "lse");
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (c == 'f') {
        r = lookahead(tok, "or");
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (c == 'i') {
        puur_naa_nka c2 = tok_nextc(tok);
        ᯀᯮᯝ (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (c == 'o') {
        r = lookahead(tok, "r");
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (c == 'n') {
        r = lookahead(tok, "ot");
    }
    ᯀᯮᯝ (r) {
        tok_backup(tok, c);
        ᯀᯮᯝ (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            ᯀᯮᯞ 0;
        }
        tok_nextc(tok);
    }
    ᯀᯮᯝᯂᯪ /* In future releases, only error will remain. */
    ᯀᯮᯝ (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        ᯀᯮᯞ 0;
    }
    ᯀᯮᯞ 1;
}

static puur_naa_nka
tok_decimal_tail(sa_nracanaa tok_state *tok)
{
    puur_naa_nka c;

    ᯀᯬᯝ (1) {
        do {
            c = tok_nextc(tok);
        } ᯀᯬᯝ (Py_ISDIGIT(c));
        ᯀᯮᯝ (c != '_') {
            bha_nga;
        }
        c = tok_nextc(tok);
        ᯀᯮᯝ (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            ᯀᯮᯞ 0;
        }
    }
    ᯀᯮᯞ c;
}

puur_naa_nka
_PyLexer_scan_number(sa_nracanaa tok_state *tok, sa_nracanaa token *token, puur_naa_nka c,
                     puur_naa_nka leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ᯀᯮᯝ (leading_dot) {
        goto fraction;
    }
    ᯀᯮᯝ (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        ᯀᯮᯝ (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            do {
                ᯀᯮᯝ (c == '_') {
                    c = tok_nextc(tok);
                }
                ᯀᯮᯝ (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    ᯀᯮᯞ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                do {
                    c = tok_nextc(tok);
                } ᯀᯬᯝ (Py_ISXDIGIT(c));
            } ᯀᯬᯝ (c == '_');
            ᯀᯮᯝ (!verify_end_of_number(tok, c, "hexadecimal")) {
                ᯀᯮᯞ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            do {
                ᯀᯮᯝ (c == '_') {
                    c = tok_nextc(tok);
                }
                ᯀᯮᯝ (c < '0' || c >= '8') {
                    ᯀᯮᯝ (Py_ISDIGIT(c)) {
                        ᯀᯮᯞ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    ᯀᯮᯝᯂᯪ {
                        tok_backup(tok, c);
                        ᯀᯮᯞ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } ᯀᯬᯝ ('0' <= c && c < '8');
            } ᯀᯬᯝ (c == '_');
            ᯀᯮᯝ (Py_ISDIGIT(c)) {
                ᯀᯮᯞ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            ᯀᯮᯝ (!verify_end_of_number(tok, c, "octal")) {
                ᯀᯮᯞ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ (c == 'b' || c == 'B') {
            /* Binary */
            c = tok_nextc(tok);
            do {
                ᯀᯮᯝ (c == '_') {
                    c = tok_nextc(tok);
                }
                ᯀᯮᯝ (c != '0' && c != '1') {
                    ᯀᯮᯝ (Py_ISDIGIT(c)) {
                        ᯀᯮᯞ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    ᯀᯮᯝᯂᯪ {
                        tok_backup(tok, c);
                        ᯀᯮᯞ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } ᯀᯬᯝ (c == '0' || c == '1');
            } ᯀᯬᯝ (c == '_');
            ᯀᯮᯝ (Py_ISDIGIT(c)) {
                ᯀᯮᯞ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            ᯀᯮᯝ (!verify_end_of_number(tok, c, "binary")) {
                ᯀᯮᯞ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ᯀᯮᯝᯂᯪ {
            puur_naa_nka nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            ᯀᯬᯝ (1) {
                ᯀᯮᯝ (c == '_') {
                    c = tok_nextc(tok);
                    ᯀᯮᯝ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ᯀᯮᯞ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                ᯀᯮᯝ (c != '0') {
                    bha_nga;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            ᯀᯮᯝ (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                ᯀᯮᯝ (c == 0) {
                    ᯀᯮᯞ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ᯀᯮᯝ (c == '.') {
                c = tok_nextc(tok);
                goto fraction;
            }
            ᯀᯮᯝᯂᯪ ᯀᯮᯝ (c == 'e' || c == 'E') {
                goto exponent;
            }
            ᯀᯮᯝᯂᯪ ᯀᯮᯝ (c == 'j' || c == 'J') {
                goto imaginary;
            }
            ᯀᯮᯝᯂᯪ ᯀᯮᯝ (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                ᯀᯮᯞ MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (puur_naa_nka)(tok->start + 1 - tok->line_start),
                        (puur_naa_nka)(zeros_end - tok->line_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            ᯀᯮᯝ (!verify_end_of_number(tok, c, "decimal")) {
                ᯀᯮᯞ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    ᯀᯮᯝᯂᯪ {
        /* Decimal */
        c = tok_decimal_tail(tok);
        ᯀᯮᯝ (c == 0) {
            ᯀᯮᯞ MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            ᯀᯮᯝ (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                ᯀᯮᯝ (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    ᯀᯮᯝ (c == 0) {
                        ᯀᯮᯞ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            ᯀᯮᯝ (c == 'e' || c == 'E') {
                puur_naa_nka e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                ᯀᯮᯝ (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    ᯀᯮᯝ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ᯀᯮᯞ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    ᯀᯮᯝ (!verify_end_of_number(tok, e, "decimal")) {
                        ᯀᯮᯞ MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    ᯀᯮᯞ MAKE_TOKEN(NUMBER);
                }
                c = tok_decimal_tail(tok);
                ᯀᯮᯝ (c == 0) {
                    ᯀᯮᯞ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ᯀᯮᯝ (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                ᯀᯮᯝ (!verify_end_of_number(tok, c, "imaginary")) {
                    ᯀᯮᯞ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ᯀᯮᯝᯂᯪ ᯀᯮᯝ (!verify_end_of_number(tok, c, "decimal")) {
                ᯀᯮᯞ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    ᯀᯮᯞ MAKE_TOKEN(NUMBER);
}
