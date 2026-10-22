#include "Python.h"
#include "pycore_token.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

static ціле
lookahead(struct tok_state *tok, const char *test)
{
    const char *s = test;
    ціле res = 0;
    поки (1) {
        ціле c = tok_nextc(tok);
        якщо (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        інакше якщо (c == *s) {
            s++;
            continue;
        }

        tok_backup(tok, c);
        поки (s != test) {
            tok_backup(tok, *--s);
        }
        повернути res;
    }
}

static ціле
verify_end_of_number(struct tok_state *tok, ціле c, const char *kind) {
    якщо (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        повернути 1;
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
    ціле r = 0;
    якщо (c == 'a') {
        r = lookahead(tok, "nd");
    }
    інакше якщо (c == 'e') {
        r = lookahead(tok, "lse");
    }
    інакше якщо (c == 'f') {
        r = lookahead(tok, "or");
    }
    інакше якщо (c == 'i') {
        ціле c2 = tok_nextc(tok);
        якщо (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    інакше якщо (c == 'o') {
        r = lookahead(tok, "r");
    }
    інакше якщо (c == 'n') {
        r = lookahead(tok, "ot");
    }
    якщо (r) {
        tok_backup(tok, c);
        якщо (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            повернути 0;
        }
        tok_nextc(tok);
    }
    інакше /* In future releases, only error will remain. */
    якщо (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        повернути 0;
    }
    повернути 1;
}

static ціле
tok_decimal_tail(struct tok_state *tok)
{
    ціле c;

    поки (1) {
        do {
            c = tok_nextc(tok);
        } поки (Py_ISDIGIT(c));
        якщо (c != '_') {
            break;
        }
        c = tok_nextc(tok);
        якщо (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            повернути 0;
        }
    }
    повернути c;
}

ціле
_PyLexer_scan_number(struct tok_state *tok, struct token *token, ціле c,
                     ціле leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    якщо (leading_dot) {
        goto fraction;
    }
    якщо (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        якщо (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            do {
                якщо (c == '_') {
                    c = tok_nextc(tok);
                }
                якщо (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    повернути MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                do {
                    c = tok_nextc(tok);
                } поки (Py_ISXDIGIT(c));
            } поки (c == '_');
            якщо (!verify_end_of_number(tok, c, "hexadecimal")) {
                повернути MAKE_TOKEN(ERRORTOKEN);
            }
        }
        інакше якщо (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            do {
                якщо (c == '_') {
                    c = tok_nextc(tok);
                }
                якщо (c < '0' || c >= '8') {
                    якщо (Py_ISDIGIT(c)) {
                        повернути MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    інакше {
                        tok_backup(tok, c);
                        повернути MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } поки ('0' <= c && c < '8');
            } поки (c == '_');
            якщо (Py_ISDIGIT(c)) {
                повернути MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            якщо (!verify_end_of_number(tok, c, "octal")) {
                повернути MAKE_TOKEN(ERRORTOKEN);
            }
        }
        інакше якщо (c == 'b' || c == 'B') {
            /* Binary */
            c = tok_nextc(tok);
            do {
                якщо (c == '_') {
                    c = tok_nextc(tok);
                }
                якщо (c != '0' && c != '1') {
                    якщо (Py_ISDIGIT(c)) {
                        повернути MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    інакше {
                        tok_backup(tok, c);
                        повернути MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } поки (c == '0' || c == '1');
            } поки (c == '_');
            якщо (Py_ISDIGIT(c)) {
                повернути MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            якщо (!verify_end_of_number(tok, c, "binary")) {
                повернути MAKE_TOKEN(ERRORTOKEN);
            }
        }
        інакше {
            ціле nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            поки (1) {
                якщо (c == '_') {
                    c = tok_nextc(tok);
                    якщо (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        повернути MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                якщо (c != '0') {
                    break;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            якщо (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                якщо (c == 0) {
                    повернути MAKE_TOKEN(ERRORTOKEN);
                }
            }
            якщо (c == '.') {
                c = tok_nextc(tok);
                goto fraction;
            }
            інакше якщо (c == 'e' || c == 'E') {
                goto exponent;
            }
            інакше якщо (c == 'j' || c == 'J') {
                goto imaginary;
            }
            інакше якщо (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                повернути MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (ціле)(tok->start + 1 - tok->line_start),
                        (ціле)(zeros_end - tok->line_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            якщо (!verify_end_of_number(tok, c, "decimal")) {
                повернути MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    інакше {
        /* Decimal */
        c = tok_decimal_tail(tok);
        якщо (c == 0) {
            повернути MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            якщо (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                якщо (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    якщо (c == 0) {
                        повернути MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            якщо (c == 'e' || c == 'E') {
                ціле e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                якщо (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    якщо (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        повернути MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } інакше якщо (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    якщо (!verify_end_of_number(tok, e, "decimal")) {
                        повернути MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    повернути MAKE_TOKEN(NUMBER);
                }
                c = tok_decimal_tail(tok);
                якщо (c == 0) {
                    повернути MAKE_TOKEN(ERRORTOKEN);
                }
            }
            якщо (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                якщо (!verify_end_of_number(tok, c, "imaginary")) {
                    повернути MAKE_TOKEN(ERRORTOKEN);
                }
            }
            інакше якщо (!verify_end_of_number(tok, c, "decimal")) {
                повернути MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    повернути MAKE_TOKEN(NUMBER);
}
