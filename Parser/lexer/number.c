#include "Python.h"
#include "pycore_token.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

static ཧྲིལ
lookahead(sa_nracanaa tok_state *tok, const var_na *test)
{
    const var_na *s = test;
    ཧྲིལ res = 0;
    སྐབས (1) {
        ཧྲིལ c = tok_nextc(tok);
        གལ་ཏེ (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        དེ་མིན གལ་ཏེ (c == *s) {
            s++;
            continue;
        }

        tok_backup(tok, c);
        སྐབས (s != test) {
            tok_backup(tok, *--s);
        }
        ཕྱིར res;
    }
}

static ཧྲིལ
verify_end_of_number(sa_nracanaa tok_state *tok, ཧྲིལ c, const var_na *kind) {
    གལ་ཏེ (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        ཕྱིར 1;
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
    ཧྲིལ r = 0;
    གལ་ཏེ (c == 'a') {
        r = lookahead(tok, "nd");
    }
    དེ་མིན གལ་ཏེ (c == 'e') {
        r = lookahead(tok, "lse");
    }
    དེ་མིན གལ་ཏེ (c == 'f') {
        r = lookahead(tok, "or");
    }
    དེ་མིན གལ་ཏེ (c == 'i') {
        ཧྲིལ c2 = tok_nextc(tok);
        གལ་ཏེ (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    དེ་མིན གལ་ཏེ (c == 'o') {
        r = lookahead(tok, "r");
    }
    དེ་མིན གལ་ཏེ (c == 'n') {
        r = lookahead(tok, "ot");
    }
    གལ་ཏེ (r) {
        tok_backup(tok, c);
        གལ་ཏེ (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            ཕྱིར 0;
        }
        tok_nextc(tok);
    }
    དེ་མིན /* In future releases, only error will remain. */
    གལ་ཏེ (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        ཕྱིར 0;
    }
    ཕྱིར 1;
}

static ཧྲིལ
tok_decimal_tail(sa_nracanaa tok_state *tok)
{
    ཧྲིལ c;

    སྐབས (1) {
        do {
            c = tok_nextc(tok);
        } སྐབས (Py_ISDIGIT(c));
        གལ་ཏེ (c != '_') {
            bha_nga;
        }
        c = tok_nextc(tok);
        གལ་ཏེ (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            ཕྱིར 0;
        }
    }
    ཕྱིར c;
}

ཧྲིལ
_PyLexer_scan_number(sa_nracanaa tok_state *tok, sa_nracanaa token *token, ཧྲིལ c,
                     ཧྲིལ leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    གལ་ཏེ (leading_dot) {
        goto fraction;
    }
    གལ་ཏེ (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        གལ་ཏེ (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            do {
                གལ་ཏེ (c == '_') {
                    c = tok_nextc(tok);
                }
                གལ་ཏེ (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    ཕྱིར MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                do {
                    c = tok_nextc(tok);
                } སྐབས (Py_ISXDIGIT(c));
            } སྐབས (c == '_');
            གལ་ཏེ (!verify_end_of_number(tok, c, "hexadecimal")) {
                ཕྱིར MAKE_TOKEN(ERRORTOKEN);
            }
        }
        དེ་མིན གལ་ཏེ (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            do {
                གལ་ཏེ (c == '_') {
                    c = tok_nextc(tok);
                }
                གལ་ཏེ (c < '0' || c >= '8') {
                    གལ་ཏེ (Py_ISDIGIT(c)) {
                        ཕྱིར MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    དེ་མིན {
                        tok_backup(tok, c);
                        ཕྱིར MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } སྐབས ('0' <= c && c < '8');
            } སྐབས (c == '_');
            གལ་ཏེ (Py_ISDIGIT(c)) {
                ཕྱིར MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            གལ་ཏེ (!verify_end_of_number(tok, c, "octal")) {
                ཕྱིར MAKE_TOKEN(ERRORTOKEN);
            }
        }
        དེ་མིན གལ་ཏེ (c == 'b' || c == 'B') {
            /* Binary */
            c = tok_nextc(tok);
            do {
                གལ་ཏེ (c == '_') {
                    c = tok_nextc(tok);
                }
                གལ་ཏེ (c != '0' && c != '1') {
                    གལ་ཏེ (Py_ISDIGIT(c)) {
                        ཕྱིར MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    དེ་མིན {
                        tok_backup(tok, c);
                        ཕྱིར MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } སྐབས (c == '0' || c == '1');
            } སྐབས (c == '_');
            གལ་ཏེ (Py_ISDIGIT(c)) {
                ཕྱིར MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            གལ་ཏེ (!verify_end_of_number(tok, c, "binary")) {
                ཕྱིར MAKE_TOKEN(ERRORTOKEN);
            }
        }
        དེ་མིན {
            ཧྲིལ nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            སྐབས (1) {
                གལ་ཏེ (c == '_') {
                    c = tok_nextc(tok);
                    གལ་ཏེ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ཕྱིར MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                གལ་ཏེ (c != '0') {
                    bha_nga;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            གལ་ཏེ (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                གལ་ཏེ (c == 0) {
                    ཕྱིར MAKE_TOKEN(ERRORTOKEN);
                }
            }
            གལ་ཏེ (c == '.') {
                c = tok_nextc(tok);
                goto fraction;
            }
            དེ་མིན གལ་ཏེ (c == 'e' || c == 'E') {
                goto exponent;
            }
            དེ་མིན གལ་ཏེ (c == 'j' || c == 'J') {
                goto imaginary;
            }
            དེ་མིན གལ་ཏེ (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                ཕྱིར MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (ཧྲིལ)(tok->start + 1 - tok->line_start),
                        (ཧྲིལ)(zeros_end - tok->line_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            གལ་ཏེ (!verify_end_of_number(tok, c, "decimal")) {
                ཕྱིར MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    དེ་མིན {
        /* Decimal */
        c = tok_decimal_tail(tok);
        གལ་ཏེ (c == 0) {
            ཕྱིར MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            གལ་ཏེ (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                གལ་ཏེ (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    གལ་ཏེ (c == 0) {
                        ཕྱིར MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            གལ་ཏེ (c == 'e' || c == 'E') {
                ཧྲིལ e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                གལ་ཏེ (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    གལ་ཏེ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ཕྱིར MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } དེ་མིན གལ་ཏེ (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    གལ་ཏེ (!verify_end_of_number(tok, e, "decimal")) {
                        ཕྱིར MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    ཕྱིར MAKE_TOKEN(NUMBER);
                }
                c = tok_decimal_tail(tok);
                གལ་ཏེ (c == 0) {
                    ཕྱིར MAKE_TOKEN(ERRORTOKEN);
                }
            }
            གལ་ཏེ (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                གལ་ཏེ (!verify_end_of_number(tok, c, "imaginary")) {
                    ཕྱིར MAKE_TOKEN(ERRORTOKEN);
                }
            }
            དེ་མིན གལ་ཏེ (!verify_end_of_number(tok, c, "decimal")) {
                ཕྱིར MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    ཕྱིར MAKE_TOKEN(NUMBER);
}
