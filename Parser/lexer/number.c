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
    ၶၢဝ်း (1) {
        puur_naa_nka c = tok_nextc(tok);
        ပေႃး (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        ပေႃးဢမ်ႇ ပေႃး (c == *s) {
            s++;
            continue;
        }

        tok_backup(tok, c);
        ၶၢဝ်း (s != test) {
            tok_backup(tok, *--s);
        }
        ပွၵ်ႈ res;
    }
}

static puur_naa_nka
verify_end_of_number(sa_nracanaa tok_state *tok, puur_naa_nka c, const var_na *kind) {
    ပေႃး (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        ပွၵ်ႈ 1;
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
    ပေႃး (c == 'a') {
        r = lookahead(tok, "nd");
    }
    ပေႃးဢမ်ႇ ပေႃး (c == 'e') {
        r = lookahead(tok, "lse");
    }
    ပေႃးဢမ်ႇ ပေႃး (c == 'f') {
        r = lookahead(tok, "or");
    }
    ပေႃးဢမ်ႇ ပေႃး (c == 'i') {
        puur_naa_nka c2 = tok_nextc(tok);
        ပေႃး (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    ပေႃးဢမ်ႇ ပေႃး (c == 'o') {
        r = lookahead(tok, "r");
    }
    ပေႃးဢမ်ႇ ပေႃး (c == 'n') {
        r = lookahead(tok, "ot");
    }
    ပေႃး (r) {
        tok_backup(tok, c);
        ပေႃး (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            ပွၵ်ႈ 0;
        }
        tok_nextc(tok);
    }
    ပေႃးဢမ်ႇ /* In future releases, only error will remain. */
    ပေႃး (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        ပွၵ်ႈ 0;
    }
    ပွၵ်ႈ 1;
}

static puur_naa_nka
tok_decimal_tail(sa_nracanaa tok_state *tok)
{
    puur_naa_nka c;

    ၶၢဝ်း (1) {
        do {
            c = tok_nextc(tok);
        } ၶၢဝ်း (Py_ISDIGIT(c));
        ပေႃး (c != '_') {
            bha_nga;
        }
        c = tok_nextc(tok);
        ပေႃး (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            ပွၵ်ႈ 0;
        }
    }
    ပွၵ်ႈ c;
}

puur_naa_nka
_PyLexer_scan_number(sa_nracanaa tok_state *tok, sa_nracanaa token *token, puur_naa_nka c,
                     puur_naa_nka leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ပေႃး (leading_dot) {
        goto fraction;
    }
    ပေႃး (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        ပေႃး (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            do {
                ပေႃး (c == '_') {
                    c = tok_nextc(tok);
                }
                ပေႃး (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    ပွၵ်ႈ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                do {
                    c = tok_nextc(tok);
                } ၶၢဝ်း (Py_ISXDIGIT(c));
            } ၶၢဝ်း (c == '_');
            ပေႃး (!verify_end_of_number(tok, c, "hexadecimal")) {
                ပွၵ်ႈ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ပေႃးဢမ်ႇ ပေႃး (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            do {
                ပေႃး (c == '_') {
                    c = tok_nextc(tok);
                }
                ပေႃး (c < '0' || c >= '8') {
                    ပေႃး (Py_ISDIGIT(c)) {
                        ပွၵ်ႈ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    ပေႃးဢမ်ႇ {
                        tok_backup(tok, c);
                        ပွၵ်ႈ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } ၶၢဝ်း ('0' <= c && c < '8');
            } ၶၢဝ်း (c == '_');
            ပေႃး (Py_ISDIGIT(c)) {
                ပွၵ်ႈ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            ပေႃး (!verify_end_of_number(tok, c, "octal")) {
                ပွၵ်ႈ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ပေႃးဢမ်ႇ ပေႃး (c == 'b' || c == 'B') {
            /* Binary */
            c = tok_nextc(tok);
            do {
                ပေႃး (c == '_') {
                    c = tok_nextc(tok);
                }
                ပေႃး (c != '0' && c != '1') {
                    ပေႃး (Py_ISDIGIT(c)) {
                        ပွၵ်ႈ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    ပေႃးဢမ်ႇ {
                        tok_backup(tok, c);
                        ပွၵ်ႈ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } ၶၢဝ်း (c == '0' || c == '1');
            } ၶၢဝ်း (c == '_');
            ပေႃး (Py_ISDIGIT(c)) {
                ပွၵ်ႈ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            ပေႃး (!verify_end_of_number(tok, c, "binary")) {
                ပွၵ်ႈ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ပေႃးဢမ်ႇ {
            puur_naa_nka nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            ၶၢဝ်း (1) {
                ပေႃး (c == '_') {
                    c = tok_nextc(tok);
                    ပေႃး (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ပွၵ်ႈ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                ပေႃး (c != '0') {
                    bha_nga;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            ပေႃး (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                ပေႃး (c == 0) {
                    ပွၵ်ႈ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ပေႃး (c == '.') {
                c = tok_nextc(tok);
                goto fraction;
            }
            ပေႃးဢမ်ႇ ပေႃး (c == 'e' || c == 'E') {
                goto exponent;
            }
            ပေႃးဢမ်ႇ ပေႃး (c == 'j' || c == 'J') {
                goto imaginary;
            }
            ပေႃးဢမ်ႇ ပေႃး (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                ပွၵ်ႈ MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (puur_naa_nka)(tok->start + 1 - tok->line_start),
                        (puur_naa_nka)(zeros_end - tok->line_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            ပေႃး (!verify_end_of_number(tok, c, "decimal")) {
                ပွၵ်ႈ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    ပေႃးဢမ်ႇ {
        /* Decimal */
        c = tok_decimal_tail(tok);
        ပေႃး (c == 0) {
            ပွၵ်ႈ MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            ပေႃး (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                ပေႃး (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    ပေႃး (c == 0) {
                        ပွၵ်ႈ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            ပေႃး (c == 'e' || c == 'E') {
                puur_naa_nka e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                ပေႃး (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    ပေႃး (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ပွၵ်ႈ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } ပေႃးဢမ်ႇ ပေႃး (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    ပေႃး (!verify_end_of_number(tok, e, "decimal")) {
                        ပွၵ်ႈ MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    ပွၵ်ႈ MAKE_TOKEN(NUMBER);
                }
                c = tok_decimal_tail(tok);
                ပေႃး (c == 0) {
                    ပွၵ်ႈ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ပေႃး (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                ပေႃး (!verify_end_of_number(tok, c, "imaginary")) {
                    ပွၵ်ႈ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ပေႃးဢမ်ႇ ပေႃး (!verify_end_of_number(tok, c, "decimal")) {
                ပွၵ်ႈ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    ပွၵ်ႈ MAKE_TOKEN(NUMBER);
}
