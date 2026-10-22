#include "Python.h"
#include "pycore_token.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

static ကိန်းပြည့်
lookahead(sa_nracanaa tok_state *tok, const var_na *test)
{
    const var_na *s = test;
    ကိန်းပြည့် res = 0;
    နေစဉ် (1) {
        ကိန်းပြည့် c = tok_nextc(tok);
        အကယ်၍ (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        မဟုတ်ပါက အကယ်၍ (c == *s) {
            s++;
            continue;
        }

        tok_backup(tok, c);
        နေစဉ် (s != test) {
            tok_backup(tok, *--s);
        }
        ပြန်ပေး res;
    }
}

static ကိန်းပြည့်
verify_end_of_number(sa_nracanaa tok_state *tok, ကိန်းပြည့် c, const var_na *kind) {
    အကယ်၍ (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        ပြန်ပေး 1;
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
    ကိန်းပြည့် r = 0;
    အကယ်၍ (c == 'a') {
        r = lookahead(tok, "nd");
    }
    မဟုတ်ပါက အကယ်၍ (c == 'e') {
        r = lookahead(tok, "lse");
    }
    မဟုတ်ပါက အကယ်၍ (c == 'f') {
        r = lookahead(tok, "or");
    }
    မဟုတ်ပါက အကယ်၍ (c == 'i') {
        ကိန်းပြည့် c2 = tok_nextc(tok);
        အကယ်၍ (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    မဟုတ်ပါက အကယ်၍ (c == 'o') {
        r = lookahead(tok, "r");
    }
    မဟုတ်ပါက အကယ်၍ (c == 'n') {
        r = lookahead(tok, "ot");
    }
    အကယ်၍ (r) {
        tok_backup(tok, c);
        အကယ်၍ (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            ပြန်ပေး 0;
        }
        tok_nextc(tok);
    }
    မဟုတ်ပါက /* In future releases, only error will remain. */
    အကယ်၍ (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        ပြန်ပေး 0;
    }
    ပြန်ပေး 1;
}

static ကိန်းပြည့်
tok_decimal_tail(sa_nracanaa tok_state *tok)
{
    ကိန်းပြည့် c;

    နေစဉ် (1) {
        do {
            c = tok_nextc(tok);
        } နေစဉ် (Py_ISDIGIT(c));
        အကယ်၍ (c != '_') {
            bha_nga;
        }
        c = tok_nextc(tok);
        အကယ်၍ (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            ပြန်ပေး 0;
        }
    }
    ပြန်ပေး c;
}

ကိန်းပြည့်
_PyLexer_scan_number(sa_nracanaa tok_state *tok, sa_nracanaa token *token, ကိန်းပြည့် c,
                     ကိန်းပြည့် leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    အကယ်၍ (leading_dot) {
        goto fraction;
    }
    အကယ်၍ (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        အကယ်၍ (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            do {
                အကယ်၍ (c == '_') {
                    c = tok_nextc(tok);
                }
                အကယ်၍ (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    ပြန်ပေး MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                do {
                    c = tok_nextc(tok);
                } နေစဉ် (Py_ISXDIGIT(c));
            } နေစဉ် (c == '_');
            အကယ်၍ (!verify_end_of_number(tok, c, "hexadecimal")) {
                ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
            }
        }
        မဟုတ်ပါက အကယ်၍ (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            do {
                အကယ်၍ (c == '_') {
                    c = tok_nextc(tok);
                }
                အကယ်၍ (c < '0' || c >= '8') {
                    အကယ်၍ (Py_ISDIGIT(c)) {
                        ပြန်ပေး MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    မဟုတ်ပါက {
                        tok_backup(tok, c);
                        ပြန်ပေး MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } နေစဉ် ('0' <= c && c < '8');
            } နေစဉ် (c == '_');
            အကယ်၍ (Py_ISDIGIT(c)) {
                ပြန်ပေး MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            အကယ်၍ (!verify_end_of_number(tok, c, "octal")) {
                ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
            }
        }
        မဟုတ်ပါက အကယ်၍ (c == 'b' || c == 'B') {
            /* Binary */
            c = tok_nextc(tok);
            do {
                အကယ်၍ (c == '_') {
                    c = tok_nextc(tok);
                }
                အကယ်၍ (c != '0' && c != '1') {
                    အကယ်၍ (Py_ISDIGIT(c)) {
                        ပြန်ပေး MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    မဟုတ်ပါက {
                        tok_backup(tok, c);
                        ပြန်ပေး MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } နေစဉ် (c == '0' || c == '1');
            } နေစဉ် (c == '_');
            အကယ်၍ (Py_ISDIGIT(c)) {
                ပြန်ပေး MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            အကယ်၍ (!verify_end_of_number(tok, c, "binary")) {
                ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
            }
        }
        မဟုတ်ပါက {
            ကိန်းပြည့် nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            နေစဉ် (1) {
                အကယ်၍ (c == '_') {
                    c = tok_nextc(tok);
                    အကယ်၍ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ပြန်ပေး MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                အကယ်၍ (c != '0') {
                    bha_nga;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            အကယ်၍ (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                အကယ်၍ (c == 0) {
                    ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
                }
            }
            အကယ်၍ (c == '.') {
                c = tok_nextc(tok);
                goto fraction;
            }
            မဟုတ်ပါက အကယ်၍ (c == 'e' || c == 'E') {
                goto exponent;
            }
            မဟုတ်ပါက အကယ်၍ (c == 'j' || c == 'J') {
                goto imaginary;
            }
            မဟုတ်ပါက အကယ်၍ (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                ပြန်ပေး MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (ကိန်းပြည့်)(tok->start + 1 - tok->line_start),
                        (ကိန်းပြည့်)(zeros_end - tok->line_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            အကယ်၍ (!verify_end_of_number(tok, c, "decimal")) {
                ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    မဟုတ်ပါက {
        /* Decimal */
        c = tok_decimal_tail(tok);
        အကယ်၍ (c == 0) {
            ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            အကယ်၍ (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                အကယ်၍ (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    အကယ်၍ (c == 0) {
                        ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            အကယ်၍ (c == 'e' || c == 'E') {
                ကိန်းပြည့် e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                အကယ်၍ (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    အကယ်၍ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ပြန်ပေး MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } မဟုတ်ပါက အကယ်၍ (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    အကယ်၍ (!verify_end_of_number(tok, e, "decimal")) {
                        ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    ပြန်ပေး MAKE_TOKEN(NUMBER);
                }
                c = tok_decimal_tail(tok);
                အကယ်၍ (c == 0) {
                    ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
                }
            }
            အကယ်၍ (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                အကယ်၍ (!verify_end_of_number(tok, c, "imaginary")) {
                    ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
                }
            }
            မဟုတ်ပါက အကယ်၍ (!verify_end_of_number(tok, c, "decimal")) {
                ပြန်ပေး MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    ပြန်ပေး MAKE_TOKEN(NUMBER);
}
