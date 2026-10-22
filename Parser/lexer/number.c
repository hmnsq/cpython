#include "Python.h"
#include "pycore_token.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

static 정수
lookahead(struct tok_state *tok, const char *test)
{
    const char *s = test;
    정수 res = 0;
    동안 (1) {
        정수 c = tok_nextc(tok);
        만약 (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        아니면 만약 (c == *s) {
            s++;
            continue;
        }

        tok_backup(tok, c);
        동안 (s != test) {
            tok_backup(tok, *--s);
        }
        반환 res;
    }
}

static 정수
verify_end_of_number(struct tok_state *tok, 정수 c, const char *kind) {
    만약 (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        반환 1;
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
    정수 r = 0;
    만약 (c == 'a') {
        r = lookahead(tok, "nd");
    }
    아니면 만약 (c == 'e') {
        r = lookahead(tok, "lse");
    }
    아니면 만약 (c == 'f') {
        r = lookahead(tok, "or");
    }
    아니면 만약 (c == 'i') {
        정수 c2 = tok_nextc(tok);
        만약 (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    아니면 만약 (c == 'o') {
        r = lookahead(tok, "r");
    }
    아니면 만약 (c == 'n') {
        r = lookahead(tok, "ot");
    }
    만약 (r) {
        tok_backup(tok, c);
        만약 (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            반환 0;
        }
        tok_nextc(tok);
    }
    아니면 /* In future releases, only error will remain. */
    만약 (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        반환 0;
    }
    반환 1;
}

static 정수
tok_decimal_tail(struct tok_state *tok)
{
    정수 c;

    동안 (1) {
        do {
            c = tok_nextc(tok);
        } 동안 (Py_ISDIGIT(c));
        만약 (c != '_') {
            break;
        }
        c = tok_nextc(tok);
        만약 (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            반환 0;
        }
    }
    반환 c;
}

정수
_PyLexer_scan_number(struct tok_state *tok, struct token *token, 정수 c,
                     정수 leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    만약 (leading_dot) {
        goto fraction;
    }
    만약 (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        만약 (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            do {
                만약 (c == '_') {
                    c = tok_nextc(tok);
                }
                만약 (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    반환 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                do {
                    c = tok_nextc(tok);
                } 동안 (Py_ISXDIGIT(c));
            } 동안 (c == '_');
            만약 (!verify_end_of_number(tok, c, "hexadecimal")) {
                반환 MAKE_TOKEN(ERRORTOKEN);
            }
        }
        아니면 만약 (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            do {
                만약 (c == '_') {
                    c = tok_nextc(tok);
                }
                만약 (c < '0' || c >= '8') {
                    만약 (Py_ISDIGIT(c)) {
                        반환 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    아니면 {
                        tok_backup(tok, c);
                        반환 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } 동안 ('0' <= c && c < '8');
            } 동안 (c == '_');
            만약 (Py_ISDIGIT(c)) {
                반환 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            만약 (!verify_end_of_number(tok, c, "octal")) {
                반환 MAKE_TOKEN(ERRORTOKEN);
            }
        }
        아니면 만약 (c == 'b' || c == 'B') {
            /* Binary */
            c = tok_nextc(tok);
            do {
                만약 (c == '_') {
                    c = tok_nextc(tok);
                }
                만약 (c != '0' && c != '1') {
                    만약 (Py_ISDIGIT(c)) {
                        반환 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    아니면 {
                        tok_backup(tok, c);
                        반환 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } 동안 (c == '0' || c == '1');
            } 동안 (c == '_');
            만약 (Py_ISDIGIT(c)) {
                반환 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            만약 (!verify_end_of_number(tok, c, "binary")) {
                반환 MAKE_TOKEN(ERRORTOKEN);
            }
        }
        아니면 {
            정수 nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            동안 (1) {
                만약 (c == '_') {
                    c = tok_nextc(tok);
                    만약 (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        반환 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                만약 (c != '0') {
                    break;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            만약 (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                만약 (c == 0) {
                    반환 MAKE_TOKEN(ERRORTOKEN);
                }
            }
            만약 (c == '.') {
                c = tok_nextc(tok);
                goto fraction;
            }
            아니면 만약 (c == 'e' || c == 'E') {
                goto exponent;
            }
            아니면 만약 (c == 'j' || c == 'J') {
                goto imaginary;
            }
            아니면 만약 (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                반환 MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (정수)(tok->start + 1 - tok->line_start),
                        (정수)(zeros_end - tok->line_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            만약 (!verify_end_of_number(tok, c, "decimal")) {
                반환 MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    아니면 {
        /* Decimal */
        c = tok_decimal_tail(tok);
        만약 (c == 0) {
            반환 MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            만약 (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                만약 (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    만약 (c == 0) {
                        반환 MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            만약 (c == 'e' || c == 'E') {
                정수 e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                만약 (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    만약 (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        반환 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } 아니면 만약 (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    만약 (!verify_end_of_number(tok, e, "decimal")) {
                        반환 MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    반환 MAKE_TOKEN(NUMBER);
                }
                c = tok_decimal_tail(tok);
                만약 (c == 0) {
                    반환 MAKE_TOKEN(ERRORTOKEN);
                }
            }
            만약 (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                만약 (!verify_end_of_number(tok, c, "imaginary")) {
                    반환 MAKE_TOKEN(ERRORTOKEN);
                }
            }
            아니면 만약 (!verify_end_of_number(tok, c, "decimal")) {
                반환 MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    반환 MAKE_TOKEN(NUMBER);
}
