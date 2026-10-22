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
    ᱡᱚᱠᱷᱚᱱ (1) {
        puur_naa_nka c = tok_nextc(tok);
        ᱡᱩᱫᱤ (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        ᱵᱟᱝ ᱡᱩᱫᱤ (c == *s) {
            s++;
            continue;
        }

        tok_backup(tok, c);
        ᱡᱚᱠᱷᱚᱱ (s != test) {
            tok_backup(tok, *--s);
        }
        ᱨᱩᱣᱟᱹᱲ res;
    }
}

static puur_naa_nka
verify_end_of_number(sa_nracanaa tok_state *tok, puur_naa_nka c, const var_na *kind) {
    ᱡᱩᱫᱤ (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        ᱨᱩᱣᱟᱹᱲ 1;
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
    ᱡᱩᱫᱤ (c == 'a') {
        r = lookahead(tok, "nd");
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ (c == 'e') {
        r = lookahead(tok, "lse");
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ (c == 'f') {
        r = lookahead(tok, "or");
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ (c == 'i') {
        puur_naa_nka c2 = tok_nextc(tok);
        ᱡᱩᱫᱤ (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ (c == 'o') {
        r = lookahead(tok, "r");
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ (c == 'n') {
        r = lookahead(tok, "ot");
    }
    ᱡᱩᱫᱤ (r) {
        tok_backup(tok, c);
        ᱡᱩᱫᱤ (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            ᱨᱩᱣᱟᱹᱲ 0;
        }
        tok_nextc(tok);
    }
    ᱵᱟᱝ /* In future releases, only error will remain. */
    ᱡᱩᱫᱤ (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        ᱨᱩᱣᱟᱹᱲ 0;
    }
    ᱨᱩᱣᱟᱹᱲ 1;
}

static puur_naa_nka
tok_decimal_tail(sa_nracanaa tok_state *tok)
{
    puur_naa_nka c;

    ᱡᱚᱠᱷᱚᱱ (1) {
        do {
            c = tok_nextc(tok);
        } ᱡᱚᱠᱷᱚᱱ (Py_ISDIGIT(c));
        ᱡᱩᱫᱤ (c != '_') {
            bha_nga;
        }
        c = tok_nextc(tok);
        ᱡᱩᱫᱤ (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            ᱨᱩᱣᱟᱹᱲ 0;
        }
    }
    ᱨᱩᱣᱟᱹᱲ c;
}

puur_naa_nka
_PyLexer_scan_number(sa_nracanaa tok_state *tok, sa_nracanaa token *token, puur_naa_nka c,
                     puur_naa_nka leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ᱡᱩᱫᱤ (leading_dot) {
        goto fraction;
    }
    ᱡᱩᱫᱤ (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        ᱡᱩᱫᱤ (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            do {
                ᱡᱩᱫᱤ (c == '_') {
                    c = tok_nextc(tok);
                }
                ᱡᱩᱫᱤ (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    ᱨᱩᱣᱟᱹᱲ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                do {
                    c = tok_nextc(tok);
                } ᱡᱚᱠᱷᱚᱱ (Py_ISXDIGIT(c));
            } ᱡᱚᱠᱷᱚᱱ (c == '_');
            ᱡᱩᱫᱤ (!verify_end_of_number(tok, c, "hexadecimal")) {
                ᱨᱩᱣᱟᱹᱲ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ᱵᱟᱝ ᱡᱩᱫᱤ (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            do {
                ᱡᱩᱫᱤ (c == '_') {
                    c = tok_nextc(tok);
                }
                ᱡᱩᱫᱤ (c < '0' || c >= '8') {
                    ᱡᱩᱫᱤ (Py_ISDIGIT(c)) {
                        ᱨᱩᱣᱟᱹᱲ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    ᱵᱟᱝ {
                        tok_backup(tok, c);
                        ᱨᱩᱣᱟᱹᱲ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } ᱡᱚᱠᱷᱚᱱ ('0' <= c && c < '8');
            } ᱡᱚᱠᱷᱚᱱ (c == '_');
            ᱡᱩᱫᱤ (Py_ISDIGIT(c)) {
                ᱨᱩᱣᱟᱹᱲ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            ᱡᱩᱫᱤ (!verify_end_of_number(tok, c, "octal")) {
                ᱨᱩᱣᱟᱹᱲ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ᱵᱟᱝ ᱡᱩᱫᱤ (c == 'b' || c == 'B') {
            /* Binary */
            c = tok_nextc(tok);
            do {
                ᱡᱩᱫᱤ (c == '_') {
                    c = tok_nextc(tok);
                }
                ᱡᱩᱫᱤ (c != '0' && c != '1') {
                    ᱡᱩᱫᱤ (Py_ISDIGIT(c)) {
                        ᱨᱩᱣᱟᱹᱲ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    ᱵᱟᱝ {
                        tok_backup(tok, c);
                        ᱨᱩᱣᱟᱹᱲ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } ᱡᱚᱠᱷᱚᱱ (c == '0' || c == '1');
            } ᱡᱚᱠᱷᱚᱱ (c == '_');
            ᱡᱩᱫᱤ (Py_ISDIGIT(c)) {
                ᱨᱩᱣᱟᱹᱲ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            ᱡᱩᱫᱤ (!verify_end_of_number(tok, c, "binary")) {
                ᱨᱩᱣᱟᱹᱲ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ᱵᱟᱝ {
            puur_naa_nka nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            ᱡᱚᱠᱷᱚᱱ (1) {
                ᱡᱩᱫᱤ (c == '_') {
                    c = tok_nextc(tok);
                    ᱡᱩᱫᱤ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ᱨᱩᱣᱟᱹᱲ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                ᱡᱩᱫᱤ (c != '0') {
                    bha_nga;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            ᱡᱩᱫᱤ (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                ᱡᱩᱫᱤ (c == 0) {
                    ᱨᱩᱣᱟᱹᱲ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ᱡᱩᱫᱤ (c == '.') {
                c = tok_nextc(tok);
                goto fraction;
            }
            ᱵᱟᱝ ᱡᱩᱫᱤ (c == 'e' || c == 'E') {
                goto exponent;
            }
            ᱵᱟᱝ ᱡᱩᱫᱤ (c == 'j' || c == 'J') {
                goto imaginary;
            }
            ᱵᱟᱝ ᱡᱩᱫᱤ (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                ᱨᱩᱣᱟᱹᱲ MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (puur_naa_nka)(tok->start + 1 - tok->line_start),
                        (puur_naa_nka)(zeros_end - tok->line_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            ᱡᱩᱫᱤ (!verify_end_of_number(tok, c, "decimal")) {
                ᱨᱩᱣᱟᱹᱲ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    ᱵᱟᱝ {
        /* Decimal */
        c = tok_decimal_tail(tok);
        ᱡᱩᱫᱤ (c == 0) {
            ᱨᱩᱣᱟᱹᱲ MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            ᱡᱩᱫᱤ (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                ᱡᱩᱫᱤ (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    ᱡᱩᱫᱤ (c == 0) {
                        ᱨᱩᱣᱟᱹᱲ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            ᱡᱩᱫᱤ (c == 'e' || c == 'E') {
                puur_naa_nka e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                ᱡᱩᱫᱤ (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    ᱡᱩᱫᱤ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ᱨᱩᱣᱟᱹᱲ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } ᱵᱟᱝ ᱡᱩᱫᱤ (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    ᱡᱩᱫᱤ (!verify_end_of_number(tok, e, "decimal")) {
                        ᱨᱩᱣᱟᱹᱲ MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    ᱨᱩᱣᱟᱹᱲ MAKE_TOKEN(NUMBER);
                }
                c = tok_decimal_tail(tok);
                ᱡᱩᱫᱤ (c == 0) {
                    ᱨᱩᱣᱟᱹᱲ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ᱡᱩᱫᱤ (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                ᱡᱩᱫᱤ (!verify_end_of_number(tok, c, "imaginary")) {
                    ᱨᱩᱣᱟᱹᱲ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ᱵᱟᱝ ᱡᱩᱫᱤ (!verify_end_of_number(tok, c, "decimal")) {
                ᱨᱩᱣᱟᱹᱲ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    ᱨᱩᱣᱟᱹᱲ MAKE_TOKEN(NUMBER);
}
