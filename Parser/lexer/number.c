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
    ᮞᮨᮓᮨᮜ (1) {
        puur_naa_nka c = tok_nextc(tok);
        ᮚᮦᮔ᮪ (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        ᮜᮤᮚ ᮚᮦᮔ᮪ (c == *s) {
            s++;
            continue;
        }

        tok_backup(tok, c);
        ᮞᮨᮓᮨᮜ (s != test) {
            tok_backup(tok, *--s);
        }
        ᮝᮜᮤ res;
    }
}

static puur_naa_nka
verify_end_of_number(sa_nracanaa tok_state *tok, puur_naa_nka c, const var_na *kind) {
    ᮚᮦᮔ᮪ (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        ᮝᮜᮤ 1;
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
    ᮚᮦᮔ᮪ (c == 'a') {
        r = lookahead(tok, "nd");
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (c == 'e') {
        r = lookahead(tok, "lse");
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (c == 'f') {
        r = lookahead(tok, "or");
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (c == 'i') {
        puur_naa_nka c2 = tok_nextc(tok);
        ᮚᮦᮔ᮪ (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (c == 'o') {
        r = lookahead(tok, "r");
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (c == 'n') {
        r = lookahead(tok, "ot");
    }
    ᮚᮦᮔ᮪ (r) {
        tok_backup(tok, c);
        ᮚᮦᮔ᮪ (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            ᮝᮜᮤ 0;
        }
        tok_nextc(tok);
    }
    ᮜᮤᮚ /* In future releases, only error will remain. */
    ᮚᮦᮔ᮪ (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        ᮝᮜᮤ 0;
    }
    ᮝᮜᮤ 1;
}

static puur_naa_nka
tok_decimal_tail(sa_nracanaa tok_state *tok)
{
    puur_naa_nka c;

    ᮞᮨᮓᮨᮜ (1) {
        do {
            c = tok_nextc(tok);
        } ᮞᮨᮓᮨᮜ (Py_ISDIGIT(c));
        ᮚᮦᮔ᮪ (c != '_') {
            bha_nga;
        }
        c = tok_nextc(tok);
        ᮚᮦᮔ᮪ (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            ᮝᮜᮤ 0;
        }
    }
    ᮝᮜᮤ c;
}

puur_naa_nka
_PyLexer_scan_number(sa_nracanaa tok_state *tok, sa_nracanaa token *token, puur_naa_nka c,
                     puur_naa_nka leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ᮚᮦᮔ᮪ (leading_dot) {
        goto fraction;
    }
    ᮚᮦᮔ᮪ (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        ᮚᮦᮔ᮪ (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            do {
                ᮚᮦᮔ᮪ (c == '_') {
                    c = tok_nextc(tok);
                }
                ᮚᮦᮔ᮪ (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    ᮝᮜᮤ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                do {
                    c = tok_nextc(tok);
                } ᮞᮨᮓᮨᮜ (Py_ISXDIGIT(c));
            } ᮞᮨᮓᮨᮜ (c == '_');
            ᮚᮦᮔ᮪ (!verify_end_of_number(tok, c, "hexadecimal")) {
                ᮝᮜᮤ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ᮜᮤᮚ ᮚᮦᮔ᮪ (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            do {
                ᮚᮦᮔ᮪ (c == '_') {
                    c = tok_nextc(tok);
                }
                ᮚᮦᮔ᮪ (c < '0' || c >= '8') {
                    ᮚᮦᮔ᮪ (Py_ISDIGIT(c)) {
                        ᮝᮜᮤ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    ᮜᮤᮚ {
                        tok_backup(tok, c);
                        ᮝᮜᮤ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } ᮞᮨᮓᮨᮜ ('0' <= c && c < '8');
            } ᮞᮨᮓᮨᮜ (c == '_');
            ᮚᮦᮔ᮪ (Py_ISDIGIT(c)) {
                ᮝᮜᮤ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            ᮚᮦᮔ᮪ (!verify_end_of_number(tok, c, "octal")) {
                ᮝᮜᮤ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ᮜᮤᮚ ᮚᮦᮔ᮪ (c == 'b' || c == 'B') {
            /* Binary */
            c = tok_nextc(tok);
            do {
                ᮚᮦᮔ᮪ (c == '_') {
                    c = tok_nextc(tok);
                }
                ᮚᮦᮔ᮪ (c != '0' && c != '1') {
                    ᮚᮦᮔ᮪ (Py_ISDIGIT(c)) {
                        ᮝᮜᮤ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    ᮜᮤᮚ {
                        tok_backup(tok, c);
                        ᮝᮜᮤ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } ᮞᮨᮓᮨᮜ (c == '0' || c == '1');
            } ᮞᮨᮓᮨᮜ (c == '_');
            ᮚᮦᮔ᮪ (Py_ISDIGIT(c)) {
                ᮝᮜᮤ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            ᮚᮦᮔ᮪ (!verify_end_of_number(tok, c, "binary")) {
                ᮝᮜᮤ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ᮜᮤᮚ {
            puur_naa_nka nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            ᮞᮨᮓᮨᮜ (1) {
                ᮚᮦᮔ᮪ (c == '_') {
                    c = tok_nextc(tok);
                    ᮚᮦᮔ᮪ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ᮝᮜᮤ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                ᮚᮦᮔ᮪ (c != '0') {
                    bha_nga;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            ᮚᮦᮔ᮪ (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                ᮚᮦᮔ᮪ (c == 0) {
                    ᮝᮜᮤ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ᮚᮦᮔ᮪ (c == '.') {
                c = tok_nextc(tok);
                goto fraction;
            }
            ᮜᮤᮚ ᮚᮦᮔ᮪ (c == 'e' || c == 'E') {
                goto exponent;
            }
            ᮜᮤᮚ ᮚᮦᮔ᮪ (c == 'j' || c == 'J') {
                goto imaginary;
            }
            ᮜᮤᮚ ᮚᮦᮔ᮪ (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                ᮝᮜᮤ MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (puur_naa_nka)(tok->start + 1 - tok->line_start),
                        (puur_naa_nka)(zeros_end - tok->line_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            ᮚᮦᮔ᮪ (!verify_end_of_number(tok, c, "decimal")) {
                ᮝᮜᮤ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    ᮜᮤᮚ {
        /* Decimal */
        c = tok_decimal_tail(tok);
        ᮚᮦᮔ᮪ (c == 0) {
            ᮝᮜᮤ MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            ᮚᮦᮔ᮪ (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                ᮚᮦᮔ᮪ (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    ᮚᮦᮔ᮪ (c == 0) {
                        ᮝᮜᮤ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            ᮚᮦᮔ᮪ (c == 'e' || c == 'E') {
                puur_naa_nka e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                ᮚᮦᮔ᮪ (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    ᮚᮦᮔ᮪ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ᮝᮜᮤ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } ᮜᮤᮚ ᮚᮦᮔ᮪ (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    ᮚᮦᮔ᮪ (!verify_end_of_number(tok, e, "decimal")) {
                        ᮝᮜᮤ MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    ᮝᮜᮤ MAKE_TOKEN(NUMBER);
                }
                c = tok_decimal_tail(tok);
                ᮚᮦᮔ᮪ (c == 0) {
                    ᮝᮜᮤ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ᮚᮦᮔ᮪ (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                ᮚᮦᮔ᮪ (!verify_end_of_number(tok, c, "imaginary")) {
                    ᮝᮜᮤ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ᮜᮤᮚ ᮚᮦᮔ᮪ (!verify_end_of_number(tok, c, "decimal")) {
                ᮝᮜᮤ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    ᮝᮜᮤ MAKE_TOKEN(NUMBER);
}
