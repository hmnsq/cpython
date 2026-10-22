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
    ᤀᤠ᤺ (1) {
        puur_naa_nka c = tok_nextc(tok);
        ᤀᤡ᤺ (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (c == *s) {
            s++;
            continue;
        }

        tok_backup(tok, c);
        ᤀᤠ᤺ (s != test) {
            tok_backup(tok, *--s);
        }
        ᤀᤡᤴ res;
    }
}

static puur_naa_nka
verify_end_of_number(sa_nracanaa tok_state *tok, puur_naa_nka c, const var_na *kind) {
    ᤀᤡ᤺ (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        ᤀᤡᤴ 1;
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
    ᤀᤡ᤺ (c == 'a') {
        r = lookahead(tok, "nd");
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (c == 'e') {
        r = lookahead(tok, "lse");
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (c == 'f') {
        r = lookahead(tok, "or");
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (c == 'i') {
        puur_naa_nka c2 = tok_nextc(tok);
        ᤀᤡ᤺ (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (c == 'o') {
        r = lookahead(tok, "r");
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (c == 'n') {
        r = lookahead(tok, "ot");
    }
    ᤀᤡ᤺ (r) {
        tok_backup(tok, c);
        ᤀᤡ᤺ (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            ᤀᤡᤴ 0;
        }
        tok_nextc(tok);
    }
    ᤀᤡ᤺ᤆᤡ /* In future releases, only error will remain. */
    ᤀᤡ᤺ (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        ᤀᤡᤴ 0;
    }
    ᤀᤡᤴ 1;
}

static puur_naa_nka
tok_decimal_tail(sa_nracanaa tok_state *tok)
{
    puur_naa_nka c;

    ᤀᤠ᤺ (1) {
        do {
            c = tok_nextc(tok);
        } ᤀᤠ᤺ (Py_ISDIGIT(c));
        ᤀᤡ᤺ (c != '_') {
            bha_nga;
        }
        c = tok_nextc(tok);
        ᤀᤡ᤺ (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            ᤀᤡᤴ 0;
        }
    }
    ᤀᤡᤴ c;
}

puur_naa_nka
_PyLexer_scan_number(sa_nracanaa tok_state *tok, sa_nracanaa token *token, puur_naa_nka c,
                     puur_naa_nka leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ᤀᤡ᤺ (leading_dot) {
        goto fraction;
    }
    ᤀᤡ᤺ (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        ᤀᤡ᤺ (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            do {
                ᤀᤡ᤺ (c == '_') {
                    c = tok_nextc(tok);
                }
                ᤀᤡ᤺ (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    ᤀᤡᤴ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                do {
                    c = tok_nextc(tok);
                } ᤀᤠ᤺ (Py_ISXDIGIT(c));
            } ᤀᤠ᤺ (c == '_');
            ᤀᤡ᤺ (!verify_end_of_number(tok, c, "hexadecimal")) {
                ᤀᤡᤴ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            do {
                ᤀᤡ᤺ (c == '_') {
                    c = tok_nextc(tok);
                }
                ᤀᤡ᤺ (c < '0' || c >= '8') {
                    ᤀᤡ᤺ (Py_ISDIGIT(c)) {
                        ᤀᤡᤴ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    ᤀᤡ᤺ᤆᤡ {
                        tok_backup(tok, c);
                        ᤀᤡᤴ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } ᤀᤠ᤺ ('0' <= c && c < '8');
            } ᤀᤠ᤺ (c == '_');
            ᤀᤡ᤺ (Py_ISDIGIT(c)) {
                ᤀᤡᤴ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            ᤀᤡ᤺ (!verify_end_of_number(tok, c, "octal")) {
                ᤀᤡᤴ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (c == 'b' || c == 'B') {
            /* Binary */
            c = tok_nextc(tok);
            do {
                ᤀᤡ᤺ (c == '_') {
                    c = tok_nextc(tok);
                }
                ᤀᤡ᤺ (c != '0' && c != '1') {
                    ᤀᤡ᤺ (Py_ISDIGIT(c)) {
                        ᤀᤡᤴ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    ᤀᤡ᤺ᤆᤡ {
                        tok_backup(tok, c);
                        ᤀᤡᤴ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } ᤀᤠ᤺ (c == '0' || c == '1');
            } ᤀᤠ᤺ (c == '_');
            ᤀᤡ᤺ (Py_ISDIGIT(c)) {
                ᤀᤡᤴ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            ᤀᤡ᤺ (!verify_end_of_number(tok, c, "binary")) {
                ᤀᤡᤴ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ᤀᤡ᤺ᤆᤡ {
            puur_naa_nka nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            ᤀᤠ᤺ (1) {
                ᤀᤡ᤺ (c == '_') {
                    c = tok_nextc(tok);
                    ᤀᤡ᤺ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ᤀᤡᤴ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                ᤀᤡ᤺ (c != '0') {
                    bha_nga;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            ᤀᤡ᤺ (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                ᤀᤡ᤺ (c == 0) {
                    ᤀᤡᤴ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ᤀᤡ᤺ (c == '.') {
                c = tok_nextc(tok);
                goto fraction;
            }
            ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (c == 'e' || c == 'E') {
                goto exponent;
            }
            ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (c == 'j' || c == 'J') {
                goto imaginary;
            }
            ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                ᤀᤡᤴ MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (puur_naa_nka)(tok->start + 1 - tok->line_start),
                        (puur_naa_nka)(zeros_end - tok->line_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            ᤀᤡ᤺ (!verify_end_of_number(tok, c, "decimal")) {
                ᤀᤡᤴ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    ᤀᤡ᤺ᤆᤡ {
        /* Decimal */
        c = tok_decimal_tail(tok);
        ᤀᤡ᤺ (c == 0) {
            ᤀᤡᤴ MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            ᤀᤡ᤺ (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                ᤀᤡ᤺ (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    ᤀᤡ᤺ (c == 0) {
                        ᤀᤡᤴ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            ᤀᤡ᤺ (c == 'e' || c == 'E') {
                puur_naa_nka e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                ᤀᤡ᤺ (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    ᤀᤡ᤺ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ᤀᤡᤴ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    ᤀᤡ᤺ (!verify_end_of_number(tok, e, "decimal")) {
                        ᤀᤡᤴ MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    ᤀᤡᤴ MAKE_TOKEN(NUMBER);
                }
                c = tok_decimal_tail(tok);
                ᤀᤡ᤺ (c == 0) {
                    ᤀᤡᤴ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ᤀᤡ᤺ (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                ᤀᤡ᤺ (!verify_end_of_number(tok, c, "imaginary")) {
                    ᤀᤡᤴ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (!verify_end_of_number(tok, c, "decimal")) {
                ᤀᤡᤴ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    ᤀᤡᤴ MAKE_TOKEN(NUMBER);
}
