#include "Python.h"
#include "pycore_token.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

static số_nguyên
lookahead(struct tok_state *tok, const char *test)
{
    const char *s = test;
    số_nguyên res = 0;
    trong_khi (1) {
        số_nguyên c = tok_nextc(tok);
        nếu (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        không_thì nếu (c == *s) {
            s++;
            continue;
        }

        tok_backup(tok, c);
        trong_khi (s != test) {
            tok_backup(tok, *--s);
        }
        trả_về res;
    }
}

static số_nguyên
verify_end_of_number(struct tok_state *tok, số_nguyên c, const char *kind) {
    nếu (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        trả_về 1;
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
    số_nguyên r = 0;
    nếu (c == 'a') {
        r = lookahead(tok, "nd");
    }
    không_thì nếu (c == 'e') {
        r = lookahead(tok, "lse");
    }
    không_thì nếu (c == 'f') {
        r = lookahead(tok, "or");
    }
    không_thì nếu (c == 'i') {
        số_nguyên c2 = tok_nextc(tok);
        nếu (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    không_thì nếu (c == 'o') {
        r = lookahead(tok, "r");
    }
    không_thì nếu (c == 'n') {
        r = lookahead(tok, "ot");
    }
    nếu (r) {
        tok_backup(tok, c);
        nếu (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            trả_về 0;
        }
        tok_nextc(tok);
    }
    không_thì /* In future releases, only error will remain. */
    nếu (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        trả_về 0;
    }
    trả_về 1;
}

static số_nguyên
tok_decimal_tail(struct tok_state *tok)
{
    số_nguyên c;

    trong_khi (1) {
        do {
            c = tok_nextc(tok);
        } trong_khi (Py_ISDIGIT(c));
        nếu (c != '_') {
            break;
        }
        c = tok_nextc(tok);
        nếu (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            trả_về 0;
        }
    }
    trả_về c;
}

số_nguyên
_PyLexer_scan_number(struct tok_state *tok, struct token *token, số_nguyên c,
                     số_nguyên leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    nếu (leading_dot) {
        goto fraction;
    }
    nếu (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        nếu (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            do {
                nếu (c == '_') {
                    c = tok_nextc(tok);
                }
                nếu (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    trả_về MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                do {
                    c = tok_nextc(tok);
                } trong_khi (Py_ISXDIGIT(c));
            } trong_khi (c == '_');
            nếu (!verify_end_of_number(tok, c, "hexadecimal")) {
                trả_về MAKE_TOKEN(ERRORTOKEN);
            }
        }
        không_thì nếu (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            do {
                nếu (c == '_') {
                    c = tok_nextc(tok);
                }
                nếu (c < '0' || c >= '8') {
                    nếu (Py_ISDIGIT(c)) {
                        trả_về MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    không_thì {
                        tok_backup(tok, c);
                        trả_về MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } trong_khi ('0' <= c && c < '8');
            } trong_khi (c == '_');
            nếu (Py_ISDIGIT(c)) {
                trả_về MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            nếu (!verify_end_of_number(tok, c, "octal")) {
                trả_về MAKE_TOKEN(ERRORTOKEN);
            }
        }
        không_thì nếu (c == 'b' || c == 'B') {
            /* Binary */
            c = tok_nextc(tok);
            do {
                nếu (c == '_') {
                    c = tok_nextc(tok);
                }
                nếu (c != '0' && c != '1') {
                    nếu (Py_ISDIGIT(c)) {
                        trả_về MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    không_thì {
                        tok_backup(tok, c);
                        trả_về MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } trong_khi (c == '0' || c == '1');
            } trong_khi (c == '_');
            nếu (Py_ISDIGIT(c)) {
                trả_về MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            nếu (!verify_end_of_number(tok, c, "binary")) {
                trả_về MAKE_TOKEN(ERRORTOKEN);
            }
        }
        không_thì {
            số_nguyên nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            trong_khi (1) {
                nếu (c == '_') {
                    c = tok_nextc(tok);
                    nếu (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        trả_về MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                nếu (c != '0') {
                    break;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            nếu (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                nếu (c == 0) {
                    trả_về MAKE_TOKEN(ERRORTOKEN);
                }
            }
            nếu (c == '.') {
                c = tok_nextc(tok);
                goto fraction;
            }
            không_thì nếu (c == 'e' || c == 'E') {
                goto exponent;
            }
            không_thì nếu (c == 'j' || c == 'J') {
                goto imaginary;
            }
            không_thì nếu (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                trả_về MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (số_nguyên)(tok->start + 1 - tok->line_start),
                        (số_nguyên)(zeros_end - tok->line_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            nếu (!verify_end_of_number(tok, c, "decimal")) {
                trả_về MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    không_thì {
        /* Decimal */
        c = tok_decimal_tail(tok);
        nếu (c == 0) {
            trả_về MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            nếu (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                nếu (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    nếu (c == 0) {
                        trả_về MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            nếu (c == 'e' || c == 'E') {
                số_nguyên e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                nếu (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    nếu (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        trả_về MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } không_thì nếu (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    nếu (!verify_end_of_number(tok, e, "decimal")) {
                        trả_về MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    trả_về MAKE_TOKEN(NUMBER);
                }
                c = tok_decimal_tail(tok);
                nếu (c == 0) {
                    trả_về MAKE_TOKEN(ERRORTOKEN);
                }
            }
            nếu (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                nếu (!verify_end_of_number(tok, c, "imaginary")) {
                    trả_về MAKE_TOKEN(ERRORTOKEN);
                }
            }
            không_thì nếu (!verify_end_of_number(tok, c, "decimal")) {
                trả_về MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    trả_về MAKE_TOKEN(NUMBER);
}
