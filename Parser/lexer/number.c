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
    ᬲᭂᬤᭂᬮ (1) {
        puur_naa_nka c = tok_nextc(tok);
        ᬬᬾᬦ᭄ (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        ᬮᬶᬬ ᬬᬾᬦ᭄ (c == *s) {
            s++;
            continue;
        }

        tok_backup(tok, c);
        ᬲᭂᬤᭂᬮ (s != test) {
            tok_backup(tok, *--s);
        }
        ᬯᬮᬶ res;
    }
}

static puur_naa_nka
verify_end_of_number(sa_nracanaa tok_state *tok, puur_naa_nka c, const var_na *kind) {
    ᬬᬾᬦ᭄ (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        ᬯᬮᬶ 1;
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
    ᬬᬾᬦ᭄ (c == 'a') {
        r = lookahead(tok, "nd");
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (c == 'e') {
        r = lookahead(tok, "lse");
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (c == 'f') {
        r = lookahead(tok, "or");
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (c == 'i') {
        puur_naa_nka c2 = tok_nextc(tok);
        ᬬᬾᬦ᭄ (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (c == 'o') {
        r = lookahead(tok, "r");
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (c == 'n') {
        r = lookahead(tok, "ot");
    }
    ᬬᬾᬦ᭄ (r) {
        tok_backup(tok, c);
        ᬬᬾᬦ᭄ (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            ᬯᬮᬶ 0;
        }
        tok_nextc(tok);
    }
    ᬮᬶᬬ /* In future releases, only error will remain. */
    ᬬᬾᬦ᭄ (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        ᬯᬮᬶ 0;
    }
    ᬯᬮᬶ 1;
}

static puur_naa_nka
tok_decimal_tail(sa_nracanaa tok_state *tok)
{
    puur_naa_nka c;

    ᬲᭂᬤᭂᬮ (1) {
        do {
            c = tok_nextc(tok);
        } ᬲᭂᬤᭂᬮ (Py_ISDIGIT(c));
        ᬬᬾᬦ᭄ (c != '_') {
            bha_nga;
        }
        c = tok_nextc(tok);
        ᬬᬾᬦ᭄ (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            ᬯᬮᬶ 0;
        }
    }
    ᬯᬮᬶ c;
}

puur_naa_nka
_PyLexer_scan_number(sa_nracanaa tok_state *tok, sa_nracanaa token *token, puur_naa_nka c,
                     puur_naa_nka leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ᬬᬾᬦ᭄ (leading_dot) {
        goto fraction;
    }
    ᬬᬾᬦ᭄ (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        ᬬᬾᬦ᭄ (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            do {
                ᬬᬾᬦ᭄ (c == '_') {
                    c = tok_nextc(tok);
                }
                ᬬᬾᬦ᭄ (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    ᬯᬮᬶ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                do {
                    c = tok_nextc(tok);
                } ᬲᭂᬤᭂᬮ (Py_ISXDIGIT(c));
            } ᬲᭂᬤᭂᬮ (c == '_');
            ᬬᬾᬦ᭄ (!verify_end_of_number(tok, c, "hexadecimal")) {
                ᬯᬮᬶ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ᬮᬶᬬ ᬬᬾᬦ᭄ (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            do {
                ᬬᬾᬦ᭄ (c == '_') {
                    c = tok_nextc(tok);
                }
                ᬬᬾᬦ᭄ (c < '0' || c >= '8') {
                    ᬬᬾᬦ᭄ (Py_ISDIGIT(c)) {
                        ᬯᬮᬶ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    ᬮᬶᬬ {
                        tok_backup(tok, c);
                        ᬯᬮᬶ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } ᬲᭂᬤᭂᬮ ('0' <= c && c < '8');
            } ᬲᭂᬤᭂᬮ (c == '_');
            ᬬᬾᬦ᭄ (Py_ISDIGIT(c)) {
                ᬯᬮᬶ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            ᬬᬾᬦ᭄ (!verify_end_of_number(tok, c, "octal")) {
                ᬯᬮᬶ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ᬮᬶᬬ ᬬᬾᬦ᭄ (c == 'b' || c == 'B') {
            /* Binary */
            c = tok_nextc(tok);
            do {
                ᬬᬾᬦ᭄ (c == '_') {
                    c = tok_nextc(tok);
                }
                ᬬᬾᬦ᭄ (c != '0' && c != '1') {
                    ᬬᬾᬦ᭄ (Py_ISDIGIT(c)) {
                        ᬯᬮᬶ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    ᬮᬶᬬ {
                        tok_backup(tok, c);
                        ᬯᬮᬶ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } ᬲᭂᬤᭂᬮ (c == '0' || c == '1');
            } ᬲᭂᬤᭂᬮ (c == '_');
            ᬬᬾᬦ᭄ (Py_ISDIGIT(c)) {
                ᬯᬮᬶ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            ᬬᬾᬦ᭄ (!verify_end_of_number(tok, c, "binary")) {
                ᬯᬮᬶ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ᬮᬶᬬ {
            puur_naa_nka nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            ᬲᭂᬤᭂᬮ (1) {
                ᬬᬾᬦ᭄ (c == '_') {
                    c = tok_nextc(tok);
                    ᬬᬾᬦ᭄ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ᬯᬮᬶ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                ᬬᬾᬦ᭄ (c != '0') {
                    bha_nga;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            ᬬᬾᬦ᭄ (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                ᬬᬾᬦ᭄ (c == 0) {
                    ᬯᬮᬶ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ᬬᬾᬦ᭄ (c == '.') {
                c = tok_nextc(tok);
                goto fraction;
            }
            ᬮᬶᬬ ᬬᬾᬦ᭄ (c == 'e' || c == 'E') {
                goto exponent;
            }
            ᬮᬶᬬ ᬬᬾᬦ᭄ (c == 'j' || c == 'J') {
                goto imaginary;
            }
            ᬮᬶᬬ ᬬᬾᬦ᭄ (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                ᬯᬮᬶ MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (puur_naa_nka)(tok->start + 1 - tok->line_start),
                        (puur_naa_nka)(zeros_end - tok->line_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            ᬬᬾᬦ᭄ (!verify_end_of_number(tok, c, "decimal")) {
                ᬯᬮᬶ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    ᬮᬶᬬ {
        /* Decimal */
        c = tok_decimal_tail(tok);
        ᬬᬾᬦ᭄ (c == 0) {
            ᬯᬮᬶ MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            ᬬᬾᬦ᭄ (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                ᬬᬾᬦ᭄ (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    ᬬᬾᬦ᭄ (c == 0) {
                        ᬯᬮᬶ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            ᬬᬾᬦ᭄ (c == 'e' || c == 'E') {
                puur_naa_nka e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                ᬬᬾᬦ᭄ (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    ᬬᬾᬦ᭄ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ᬯᬮᬶ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } ᬮᬶᬬ ᬬᬾᬦ᭄ (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    ᬬᬾᬦ᭄ (!verify_end_of_number(tok, e, "decimal")) {
                        ᬯᬮᬶ MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    ᬯᬮᬶ MAKE_TOKEN(NUMBER);
                }
                c = tok_decimal_tail(tok);
                ᬬᬾᬦ᭄ (c == 0) {
                    ᬯᬮᬶ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ᬬᬾᬦ᭄ (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                ᬬᬾᬦ᭄ (!verify_end_of_number(tok, c, "imaginary")) {
                    ᬯᬮᬶ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ᬮᬶᬬ ᬬᬾᬦ᭄ (!verify_end_of_number(tok, c, "decimal")) {
                ᬯᬮᬶ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    ᬯᬮᬶ MAKE_TOKEN(NUMBER);
}
