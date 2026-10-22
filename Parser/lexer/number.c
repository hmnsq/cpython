#include "Python.h"
#include "pycore_token.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

static целое
lookahead(struct tok_state *tok, const char *test)
{
    const char *s = test;
    целое res = 0;
    пока (1) {
        целое c = tok_nextc(tok);
        если (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        иначе если (c == *s) {
            s++;
            continue;
        }

        tok_backup(tok, c);
        пока (s != test) {
            tok_backup(tok, *--s);
        }
        вернуть res;
    }
}

static целое
verify_end_of_number(struct tok_state *tok, целое c, const char *kind) {
    если (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        вернуть 1;
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
    целое r = 0;
    если (c == 'a') {
        r = lookahead(tok, "nd");
    }
    иначе если (c == 'e') {
        r = lookahead(tok, "lse");
    }
    иначе если (c == 'f') {
        r = lookahead(tok, "or");
    }
    иначе если (c == 'i') {
        целое c2 = tok_nextc(tok);
        если (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    иначе если (c == 'o') {
        r = lookahead(tok, "r");
    }
    иначе если (c == 'n') {
        r = lookahead(tok, "ot");
    }
    если (r) {
        tok_backup(tok, c);
        если (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            вернуть 0;
        }
        tok_nextc(tok);
    }
    иначе /* In future releases, only error will remain. */
    если (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        вернуть 0;
    }
    вернуть 1;
}

static целое
tok_decimal_tail(struct tok_state *tok)
{
    целое c;

    пока (1) {
        do {
            c = tok_nextc(tok);
        } пока (Py_ISDIGIT(c));
        если (c != '_') {
            break;
        }
        c = tok_nextc(tok);
        если (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            вернуть 0;
        }
    }
    вернуть c;
}

целое
_PyLexer_scan_number(struct tok_state *tok, struct token *token, целое c,
                     целое leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    если (leading_dot) {
        goto fraction;
    }
    если (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        если (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            do {
                если (c == '_') {
                    c = tok_nextc(tok);
                }
                если (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    вернуть MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                do {
                    c = tok_nextc(tok);
                } пока (Py_ISXDIGIT(c));
            } пока (c == '_');
            если (!verify_end_of_number(tok, c, "hexadecimal")) {
                вернуть MAKE_TOKEN(ERRORTOKEN);
            }
        }
        иначе если (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            do {
                если (c == '_') {
                    c = tok_nextc(tok);
                }
                если (c < '0' || c >= '8') {
                    если (Py_ISDIGIT(c)) {
                        вернуть MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    иначе {
                        tok_backup(tok, c);
                        вернуть MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } пока ('0' <= c && c < '8');
            } пока (c == '_');
            если (Py_ISDIGIT(c)) {
                вернуть MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            если (!verify_end_of_number(tok, c, "octal")) {
                вернуть MAKE_TOKEN(ERRORTOKEN);
            }
        }
        иначе если (c == 'b' || c == 'B') {
            /* Binary */
            c = tok_nextc(tok);
            do {
                если (c == '_') {
                    c = tok_nextc(tok);
                }
                если (c != '0' && c != '1') {
                    если (Py_ISDIGIT(c)) {
                        вернуть MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    иначе {
                        tok_backup(tok, c);
                        вернуть MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } пока (c == '0' || c == '1');
            } пока (c == '_');
            если (Py_ISDIGIT(c)) {
                вернуть MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            если (!verify_end_of_number(tok, c, "binary")) {
                вернуть MAKE_TOKEN(ERRORTOKEN);
            }
        }
        иначе {
            целое nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            пока (1) {
                если (c == '_') {
                    c = tok_nextc(tok);
                    если (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        вернуть MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                если (c != '0') {
                    break;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            если (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                если (c == 0) {
                    вернуть MAKE_TOKEN(ERRORTOKEN);
                }
            }
            если (c == '.') {
                c = tok_nextc(tok);
                goto fraction;
            }
            иначе если (c == 'e' || c == 'E') {
                goto exponent;
            }
            иначе если (c == 'j' || c == 'J') {
                goto imaginary;
            }
            иначе если (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                вернуть MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (целое)(tok->start + 1 - tok->line_start),
                        (целое)(zeros_end - tok->line_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            если (!verify_end_of_number(tok, c, "decimal")) {
                вернуть MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    иначе {
        /* Decimal */
        c = tok_decimal_tail(tok);
        если (c == 0) {
            вернуть MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            если (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                если (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    если (c == 0) {
                        вернуть MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            если (c == 'e' || c == 'E') {
                целое e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                если (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    если (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        вернуть MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } иначе если (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    если (!verify_end_of_number(tok, e, "decimal")) {
                        вернуть MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    вернуть MAKE_TOKEN(NUMBER);
                }
                c = tok_decimal_tail(tok);
                если (c == 0) {
                    вернуть MAKE_TOKEN(ERRORTOKEN);
                }
            }
            если (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                если (!verify_end_of_number(tok, c, "imaginary")) {
                    вернуть MAKE_TOKEN(ERRORTOKEN);
                }
            }
            иначе если (!verify_end_of_number(tok, c, "decimal")) {
                вернуть MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    вернуть MAKE_TOKEN(NUMBER);
}
