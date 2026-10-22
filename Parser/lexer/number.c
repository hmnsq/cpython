#include "Python.h"
#include "pycore_token.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

static 整数
lookahead(struct tok_state *tok, const char *test)
{
    const char *s = test;
    整数 res = 0;
    当 (1) {
        整数 c = tok_nextc(tok);
        如果 (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        否则 如果 (c == *s) {
            s++;
            continue;
        }

        tok_backup(tok, c);
        当 (s != test) {
            tok_backup(tok, *--s);
        }
        返回 res;
    }
}

static 整数
verify_end_of_number(struct tok_state *tok, 整数 c, const char *kind) {
    如果 (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        返回 1;
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
    整数 r = 0;
    如果 (c == 'a') {
        r = lookahead(tok, "nd");
    }
    否则 如果 (c == 'e') {
        r = lookahead(tok, "lse");
    }
    否则 如果 (c == 'f') {
        r = lookahead(tok, "or");
    }
    否则 如果 (c == 'i') {
        整数 c2 = tok_nextc(tok);
        如果 (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    否则 如果 (c == 'o') {
        r = lookahead(tok, "r");
    }
    否则 如果 (c == 'n') {
        r = lookahead(tok, "ot");
    }
    如果 (r) {
        tok_backup(tok, c);
        如果 (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            返回 0;
        }
        tok_nextc(tok);
    }
    否则 /* In future releases, only error will remain. */
    如果 (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        返回 0;
    }
    返回 1;
}

static 整数
tok_decimal_tail(struct tok_state *tok)
{
    整数 c;

    当 (1) {
        do {
            c = tok_nextc(tok);
        } 当 (Py_ISDIGIT(c));
        如果 (c != '_') {
            break;
        }
        c = tok_nextc(tok);
        如果 (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            返回 0;
        }
    }
    返回 c;
}

整数
_PyLexer_scan_number(struct tok_state *tok, struct token *token, 整数 c,
                     整数 leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    如果 (leading_dot) {
        goto fraction;
    }
    如果 (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        如果 (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            do {
                如果 (c == '_') {
                    c = tok_nextc(tok);
                }
                如果 (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    返回 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                do {
                    c = tok_nextc(tok);
                } 当 (Py_ISXDIGIT(c));
            } 当 (c == '_');
            如果 (!verify_end_of_number(tok, c, "hexadecimal")) {
                返回 MAKE_TOKEN(ERRORTOKEN);
            }
        }
        否则 如果 (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            do {
                如果 (c == '_') {
                    c = tok_nextc(tok);
                }
                如果 (c < '0' || c >= '8') {
                    如果 (Py_ISDIGIT(c)) {
                        返回 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    否则 {
                        tok_backup(tok, c);
                        返回 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } 当 ('0' <= c && c < '8');
            } 当 (c == '_');
            如果 (Py_ISDIGIT(c)) {
                返回 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            如果 (!verify_end_of_number(tok, c, "octal")) {
                返回 MAKE_TOKEN(ERRORTOKEN);
            }
        }
        否则 如果 (c == 'b' || c == 'B') {
            /* Binary */
            c = tok_nextc(tok);
            do {
                如果 (c == '_') {
                    c = tok_nextc(tok);
                }
                如果 (c != '0' && c != '1') {
                    如果 (Py_ISDIGIT(c)) {
                        返回 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    否则 {
                        tok_backup(tok, c);
                        返回 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } 当 (c == '0' || c == '1');
            } 当 (c == '_');
            如果 (Py_ISDIGIT(c)) {
                返回 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            如果 (!verify_end_of_number(tok, c, "binary")) {
                返回 MAKE_TOKEN(ERRORTOKEN);
            }
        }
        否则 {
            整数 nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            当 (1) {
                如果 (c == '_') {
                    c = tok_nextc(tok);
                    如果 (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        返回 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                如果 (c != '0') {
                    break;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            如果 (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                如果 (c == 0) {
                    返回 MAKE_TOKEN(ERRORTOKEN);
                }
            }
            如果 (c == '.') {
                c = tok_nextc(tok);
                goto fraction;
            }
            否则 如果 (c == 'e' || c == 'E') {
                goto exponent;
            }
            否则 如果 (c == 'j' || c == 'J') {
                goto imaginary;
            }
            否则 如果 (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                返回 MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (整数)(tok->start + 1 - tok->line_start),
                        (整数)(zeros_end - tok->line_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            如果 (!verify_end_of_number(tok, c, "decimal")) {
                返回 MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    否则 {
        /* Decimal */
        c = tok_decimal_tail(tok);
        如果 (c == 0) {
            返回 MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            如果 (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                如果 (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    如果 (c == 0) {
                        返回 MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            如果 (c == 'e' || c == 'E') {
                整数 e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                如果 (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    如果 (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        返回 MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } 否则 如果 (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    如果 (!verify_end_of_number(tok, e, "decimal")) {
                        返回 MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    返回 MAKE_TOKEN(NUMBER);
                }
                c = tok_decimal_tail(tok);
                如果 (c == 0) {
                    返回 MAKE_TOKEN(ERRORTOKEN);
                }
            }
            如果 (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                如果 (!verify_end_of_number(tok, c, "imaginary")) {
                    返回 MAKE_TOKEN(ERRORTOKEN);
                }
            }
            否则 如果 (!verify_end_of_number(tok, c, "decimal")) {
                返回 MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    返回 MAKE_TOKEN(NUMBER);
}
