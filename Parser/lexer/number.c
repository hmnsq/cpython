#include "Python.h"
#include "pycore_token.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

static цяло
lookahead(struct tok_state *tok, const char *test)
{
    const char *s = test;
    цяло res = 0;
    докато (1) {
        цяло c = tok_nextc(tok);
        ако (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        иначе ако (c == *s) {
            s++;
            continue;
        }

        tok_backup(tok, c);
        докато (s != test) {
            tok_backup(tok, *--s);
        }
        върни res;
    }
}

static цяло
verify_end_of_number(struct tok_state *tok, цяло c, const char *kind) {
    ако (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        върни 1;
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
    цяло r = 0;
    ако (c == 'a') {
        r = lookahead(tok, "nd");
    }
    иначе ако (c == 'e') {
        r = lookahead(tok, "lse");
    }
    иначе ако (c == 'f') {
        r = lookahead(tok, "or");
    }
    иначе ако (c == 'i') {
        цяло c2 = tok_nextc(tok);
        ако (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    иначе ако (c == 'o') {
        r = lookahead(tok, "r");
    }
    иначе ако (c == 'n') {
        r = lookahead(tok, "ot");
    }
    ако (r) {
        tok_backup(tok, c);
        ако (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            върни 0;
        }
        tok_nextc(tok);
    }
    иначе /* In future releases, only error will remain. */
    ако (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        върни 0;
    }
    върни 1;
}

static цяло
tok_decimal_tail(struct tok_state *tok)
{
    цяло c;

    докато (1) {
        do {
            c = tok_nextc(tok);
        } докато (Py_ISDIGIT(c));
        ако (c != '_') {
            break;
        }
        c = tok_nextc(tok);
        ако (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            върни 0;
        }
    }
    върни c;
}

цяло
_PyLexer_scan_number(struct tok_state *tok, struct token *token, цяло c,
                     цяло leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ако (leading_dot) {
        goto fraction;
    }
    ако (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        ако (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            do {
                ако (c == '_') {
                    c = tok_nextc(tok);
                }
                ако (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    върни MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                do {
                    c = tok_nextc(tok);
                } докато (Py_ISXDIGIT(c));
            } докато (c == '_');
            ако (!verify_end_of_number(tok, c, "hexadecimal")) {
                върни MAKE_TOKEN(ERRORTOKEN);
            }
        }
        иначе ако (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            do {
                ако (c == '_') {
                    c = tok_nextc(tok);
                }
                ако (c < '0' || c >= '8') {
                    ако (Py_ISDIGIT(c)) {
                        върни MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    иначе {
                        tok_backup(tok, c);
                        върни MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } докато ('0' <= c && c < '8');
            } докато (c == '_');
            ако (Py_ISDIGIT(c)) {
                върни MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            ако (!verify_end_of_number(tok, c, "octal")) {
                върни MAKE_TOKEN(ERRORTOKEN);
            }
        }
        иначе ако (c == 'b' || c == 'B') {
            /* Binary */
            c = tok_nextc(tok);
            do {
                ако (c == '_') {
                    c = tok_nextc(tok);
                }
                ако (c != '0' && c != '1') {
                    ако (Py_ISDIGIT(c)) {
                        върни MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    иначе {
                        tok_backup(tok, c);
                        върни MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } докато (c == '0' || c == '1');
            } докато (c == '_');
            ако (Py_ISDIGIT(c)) {
                върни MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            ако (!verify_end_of_number(tok, c, "binary")) {
                върни MAKE_TOKEN(ERRORTOKEN);
            }
        }
        иначе {
            цяло nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            докато (1) {
                ако (c == '_') {
                    c = tok_nextc(tok);
                    ако (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        върни MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                ако (c != '0') {
                    break;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            ако (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                ако (c == 0) {
                    върни MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ако (c == '.') {
                c = tok_nextc(tok);
                goto fraction;
            }
            иначе ако (c == 'e' || c == 'E') {
                goto exponent;
            }
            иначе ако (c == 'j' || c == 'J') {
                goto imaginary;
            }
            иначе ако (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                върни MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (цяло)(tok->start + 1 - tok->line_start),
                        (цяло)(zeros_end - tok->line_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            ако (!verify_end_of_number(tok, c, "decimal")) {
                върни MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    иначе {
        /* Decimal */
        c = tok_decimal_tail(tok);
        ако (c == 0) {
            върни MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            ако (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                ако (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    ако (c == 0) {
                        върни MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            ако (c == 'e' || c == 'E') {
                цяло e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                ако (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    ако (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        върни MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } иначе ако (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    ако (!verify_end_of_number(tok, e, "decimal")) {
                        върни MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    върни MAKE_TOKEN(NUMBER);
                }
                c = tok_decimal_tail(tok);
                ако (c == 0) {
                    върни MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ако (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                ако (!verify_end_of_number(tok, c, "imaginary")) {
                    върни MAKE_TOKEN(ERRORTOKEN);
                }
            }
            иначе ако (!verify_end_of_number(tok, c, "decimal")) {
                върни MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    върни MAKE_TOKEN(NUMBER);
}
