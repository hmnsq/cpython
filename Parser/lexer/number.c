#include "Python.h"
#include "pycore_token.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

static ຈຳນວນເຕັມ
lookahead(sa_nracanaa tok_state *tok, const var_na *test)
{
    const var_na *s = test;
    ຈຳນວນເຕັມ res = 0;
    ໃນຂະນະທີ່ (1) {
        ຈຳນວນເຕັມ c = tok_nextc(tok);
        ຖ້າ (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == *s) {
            s++;
            continue;
        }

        tok_backup(tok, c);
        ໃນຂະນະທີ່ (s != test) {
            tok_backup(tok, *--s);
        }
        ສົ່ງຄືນ res;
    }
}

static ຈຳນວນເຕັມ
verify_end_of_number(sa_nracanaa tok_state *tok, ຈຳນວນເຕັມ c, const var_na *kind) {
    ຖ້າ (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        ສົ່ງຄືນ 1;
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
    ຈຳນວນເຕັມ r = 0;
    ຖ້າ (c == 'a') {
        r = lookahead(tok, "nd");
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == 'e') {
        r = lookahead(tok, "lse");
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == 'f') {
        r = lookahead(tok, "or");
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == 'i') {
        ຈຳນວນເຕັມ c2 = tok_nextc(tok);
        ຖ້າ (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == 'o') {
        r = lookahead(tok, "r");
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == 'n') {
        r = lookahead(tok, "ot");
    }
    ຖ້າ (r) {
        tok_backup(tok, c);
        ຖ້າ (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            ສົ່ງຄືນ 0;
        }
        tok_nextc(tok);
    }
    ບໍ່ດັ່ງນັ້ນ /* In future releases, only error will remain. */
    ຖ້າ (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        ສົ່ງຄືນ 0;
    }
    ສົ່ງຄືນ 1;
}

static ຈຳນວນເຕັມ
tok_decimal_tail(sa_nracanaa tok_state *tok)
{
    ຈຳນວນເຕັມ c;

    ໃນຂະນະທີ່ (1) {
        do {
            c = tok_nextc(tok);
        } ໃນຂະນະທີ່ (Py_ISDIGIT(c));
        ຖ້າ (c != '_') {
            bha_nga;
        }
        c = tok_nextc(tok);
        ຖ້າ (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            ສົ່ງຄືນ 0;
        }
    }
    ສົ່ງຄືນ c;
}

ຈຳນວນເຕັມ
_PyLexer_scan_number(sa_nracanaa tok_state *tok, sa_nracanaa token *token, ຈຳນວນເຕັມ c,
                     ຈຳນວນເຕັມ leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    ຖ້າ (leading_dot) {
        goto fraction;
    }
    ຖ້າ (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        ຖ້າ (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            do {
                ຖ້າ (c == '_') {
                    c = tok_nextc(tok);
                }
                ຖ້າ (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    ສົ່ງຄືນ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                do {
                    c = tok_nextc(tok);
                } ໃນຂະນະທີ່ (Py_ISXDIGIT(c));
            } ໃນຂະນະທີ່ (c == '_');
            ຖ້າ (!verify_end_of_number(tok, c, "hexadecimal")) {
                ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            do {
                ຖ້າ (c == '_') {
                    c = tok_nextc(tok);
                }
                ຖ້າ (c < '0' || c >= '8') {
                    ຖ້າ (Py_ISDIGIT(c)) {
                        ສົ່ງຄືນ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    ບໍ່ດັ່ງນັ້ນ {
                        tok_backup(tok, c);
                        ສົ່ງຄືນ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } ໃນຂະນະທີ່ ('0' <= c && c < '8');
            } ໃນຂະນະທີ່ (c == '_');
            ຖ້າ (Py_ISDIGIT(c)) {
                ສົ່ງຄືນ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            ຖ້າ (!verify_end_of_number(tok, c, "octal")) {
                ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == 'b' || c == 'B') {
            /* Binary */
            c = tok_nextc(tok);
            do {
                ຖ້າ (c == '_') {
                    c = tok_nextc(tok);
                }
                ຖ້າ (c != '0' && c != '1') {
                    ຖ້າ (Py_ISDIGIT(c)) {
                        ສົ່ງຄືນ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    ບໍ່ດັ່ງນັ້ນ {
                        tok_backup(tok, c);
                        ສົ່ງຄືນ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } ໃນຂະນະທີ່ (c == '0' || c == '1');
            } ໃນຂະນະທີ່ (c == '_');
            ຖ້າ (Py_ISDIGIT(c)) {
                ສົ່ງຄືນ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            ຖ້າ (!verify_end_of_number(tok, c, "binary")) {
                ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        ບໍ່ດັ່ງນັ້ນ {
            ຈຳນວນເຕັມ nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            ໃນຂະນະທີ່ (1) {
                ຖ້າ (c == '_') {
                    c = tok_nextc(tok);
                    ຖ້າ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ສົ່ງຄືນ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                ຖ້າ (c != '0') {
                    bha_nga;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            ຖ້າ (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                ຖ້າ (c == 0) {
                    ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ຖ້າ (c == '.') {
                c = tok_nextc(tok);
                goto fraction;
            }
            ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == 'e' || c == 'E') {
                goto exponent;
            }
            ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == 'j' || c == 'J') {
                goto imaginary;
            }
            ບໍ່ດັ່ງນັ້ນ ຖ້າ (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                ສົ່ງຄືນ MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (ຈຳນວນເຕັມ)(tok->start + 1 - tok->line_start),
                        (ຈຳນວນເຕັມ)(zeros_end - tok->line_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            ຖ້າ (!verify_end_of_number(tok, c, "decimal")) {
                ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    ບໍ່ດັ່ງນັ້ນ {
        /* Decimal */
        c = tok_decimal_tail(tok);
        ຖ້າ (c == 0) {
            ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            ຖ້າ (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                ຖ້າ (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    ຖ້າ (c == 0) {
                        ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            ຖ້າ (c == 'e' || c == 'E') {
                ຈຳນວນເຕັມ e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                ຖ້າ (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    ຖ້າ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ສົ່ງຄືນ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } ບໍ່ດັ່ງນັ້ນ ຖ້າ (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    ຖ້າ (!verify_end_of_number(tok, e, "decimal")) {
                        ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    ສົ່ງຄືນ MAKE_TOKEN(NUMBER);
                }
                c = tok_decimal_tail(tok);
                ຖ້າ (c == 0) {
                    ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ຖ້າ (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                ຖ້າ (!verify_end_of_number(tok, c, "imaginary")) {
                    ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ບໍ່ດັ່ງນັ້ນ ຖ້າ (!verify_end_of_number(tok, c, "decimal")) {
                ສົ່ງຄືນ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    ສົ່ງຄືນ MAKE_TOKEN(NUMBER);
}
