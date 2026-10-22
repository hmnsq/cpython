#include "Python.h"
#include "pycore_token.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

static จำนวนเต็ม
lookahead(sa_nracanaa tok_state *tok, const var_na *test)
{
    const var_na *s = test;
    จำนวนเต็ม res = 0;
    ขณะ (1) {
        จำนวนเต็ม c = tok_nextc(tok);
        ถ้า (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        มิฉะนั้น ถ้า (c == *s) {
            s++;
            continue;
        }

        tok_backup(tok, c);
        ขณะ (s != test) {
            tok_backup(tok, *--s);
        }
        คืน res;
    }
}

static จำนวนเต็ม
verify_end_of_number(sa_nracanaa tok_state *tok, จำนวนเต็ม c, const var_na *kind) {
    ถ้า (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        คืน 1;
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
    จำนวนเต็ม r = 0;
    ถ้า (c == 'a') {
        r = lookahead(tok, "nd");
    }
    มิฉะนั้น ถ้า (c == 'e') {
        r = lookahead(tok, "lse");
    }
    มิฉะนั้น ถ้า (c == 'f') {
        r = lookahead(tok, "or");
    }
    มิฉะนั้น ถ้า (c == 'i') {
        จำนวนเต็ม c2 = tok_nextc(tok);
        ถ้า (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    มิฉะนั้น ถ้า (c == 'o') {
        r = lookahead(tok, "r");
    }
    มิฉะนั้น ถ้า (c == 'n') {
        r = lookahead(tok, "ot");
    }
    ถ้า (r) {
        tok_backup(tok, c);
        ถ้า (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            คืน 0;
        }
        tok_nextc(tok);
    }
    มิฉะนั้น /* In future releases, only error will remain. */
    ถ้า (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        คืน 0;
    }
    คืน 1;
}

static จำนวนเต็ม
tok_decimal_tail(sa_nracanaa tok_state *tok)
{
    จำนวนเต็ม c;

    ขณะ (1) {
        do {
            c = tok_nextc(tok);
        } ขณะ (Py_ISDIGIT(c));
        ถ้า (c != '_') {
            bha_nga;
        }
        c = tok_nextc(tok);
        ถ้า (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            คืน 0;
        }
    }
    คืน c;
}

จำนวนเต็ม
_PyLexer_scan_number(sa_nracanaa tok_state *tok, sa_nracanaa token *token, จำนวนเต็ม c,
                     จำนวนเต็ม leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ถ้า (leading_dot) {
        goto fraction;
    }
    ถ้า (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        ถ้า (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            do {
                ถ้า (c == '_') {
                    c = tok_nextc(tok);
                }
                ถ้า (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    คืน MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                do {
                    c = tok_nextc(tok);
                } ขณะ (Py_ISXDIGIT(c));
            } ขณะ (c == '_');
            ถ้า (!verify_end_of_number(tok, c, "hexadecimal")) {
                คืน MAKE_TOKEN(ERRORTOKEN);
            }
        }
        มิฉะนั้น ถ้า (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            do {
                ถ้า (c == '_') {
                    c = tok_nextc(tok);
                }
                ถ้า (c < '0' || c >= '8') {
                    ถ้า (Py_ISDIGIT(c)) {
                        คืน MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    มิฉะนั้น {
                        tok_backup(tok, c);
                        คืน MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } ขณะ ('0' <= c && c < '8');
            } ขณะ (c == '_');
            ถ้า (Py_ISDIGIT(c)) {
                คืน MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            ถ้า (!verify_end_of_number(tok, c, "octal")) {
                คืน MAKE_TOKEN(ERRORTOKEN);
            }
        }
        มิฉะนั้น ถ้า (c == 'b' || c == 'B') {
            /* Binary */
            c = tok_nextc(tok);
            do {
                ถ้า (c == '_') {
                    c = tok_nextc(tok);
                }
                ถ้า (c != '0' && c != '1') {
                    ถ้า (Py_ISDIGIT(c)) {
                        คืน MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    มิฉะนั้น {
                        tok_backup(tok, c);
                        คืน MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } ขณะ (c == '0' || c == '1');
            } ขณะ (c == '_');
            ถ้า (Py_ISDIGIT(c)) {
                คืน MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            ถ้า (!verify_end_of_number(tok, c, "binary")) {
                คืน MAKE_TOKEN(ERRORTOKEN);
            }
        }
        มิฉะนั้น {
            จำนวนเต็ม nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            ขณะ (1) {
                ถ้า (c == '_') {
                    c = tok_nextc(tok);
                    ถ้า (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        คืน MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                ถ้า (c != '0') {
                    bha_nga;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            ถ้า (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                ถ้า (c == 0) {
                    คืน MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ถ้า (c == '.') {
                c = tok_nextc(tok);
                goto fraction;
            }
            มิฉะนั้น ถ้า (c == 'e' || c == 'E') {
                goto exponent;
            }
            มิฉะนั้น ถ้า (c == 'j' || c == 'J') {
                goto imaginary;
            }
            มิฉะนั้น ถ้า (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                คืน MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (จำนวนเต็ม)(tok->start + 1 - tok->line_start),
                        (จำนวนเต็ม)(zeros_end - tok->line_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            ถ้า (!verify_end_of_number(tok, c, "decimal")) {
                คืน MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    มิฉะนั้น {
        /* Decimal */
        c = tok_decimal_tail(tok);
        ถ้า (c == 0) {
            คืน MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            ถ้า (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                ถ้า (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    ถ้า (c == 0) {
                        คืน MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            ถ้า (c == 'e' || c == 'E') {
                จำนวนเต็ม e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                ถ้า (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    ถ้า (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        คืน MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } มิฉะนั้น ถ้า (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    ถ้า (!verify_end_of_number(tok, e, "decimal")) {
                        คืน MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    คืน MAKE_TOKEN(NUMBER);
                }
                c = tok_decimal_tail(tok);
                ถ้า (c == 0) {
                    คืน MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ถ้า (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                ถ้า (!verify_end_of_number(tok, c, "imaginary")) {
                    คืน MAKE_TOKEN(ERRORTOKEN);
                }
            }
            มิฉะนั้น ถ้า (!verify_end_of_number(tok, c, "decimal")) {
                คืน MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    คืน MAKE_TOKEN(NUMBER);
}
