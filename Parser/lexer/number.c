#include "Python.h"
#include "pycore_token.h"

#include "lexer_internal.h"
#include "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

static පූර්ණ_සංඛ්‍යා
lookahead(sa_nracanaa tok_state *tok, const var_na *test)
{
    const var_na *s = test;
    පූර්ණ_සංඛ්‍යා res = 0;
    අතරතුර (1) {
        පූර්ණ_සංඛ්‍යා c = tok_nextc(tok);
        නම් (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        නැතහොත් නම් (c == *s) {
            s++;
            continue;
        }

        tok_backup(tok, c);
        අතරතුර (s != test) {
            tok_backup(tok, *--s);
        }
        ආපසු res;
    }
}

static පූර්ණ_සංඛ්‍යා
verify_end_of_number(sa_nracanaa tok_state *tok, පූර්ණ_සංඛ්‍යා c, const var_na *kind) {
    නම් (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        ආපසු 1;
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
    පූර්ණ_සංඛ්‍යා r = 0;
    නම් (c == 'a') {
        r = lookahead(tok, "nd");
    }
    නැතහොත් නම් (c == 'e') {
        r = lookahead(tok, "lse");
    }
    නැතහොත් නම් (c == 'f') {
        r = lookahead(tok, "or");
    }
    නැතහොත් නම් (c == 'i') {
        පූර්ණ_සංඛ්‍යා c2 = tok_nextc(tok);
        නම් (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    නැතහොත් නම් (c == 'o') {
        r = lookahead(tok, "r");
    }
    නැතහොත් නම් (c == 'n') {
        r = lookahead(tok, "ot");
    }
    නම් (r) {
        tok_backup(tok, c);
        නම් (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            ආපසු 0;
        }
        tok_nextc(tok);
    }
    නැතහොත් /* In future releases, only error will remain. */
    නම් (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        ආපසු 0;
    }
    ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා
tok_decimal_tail(sa_nracanaa tok_state *tok)
{
    පූර්ණ_සංඛ්‍යා c;

    අතරතුර (1) {
        do {
            c = tok_nextc(tok);
        } අතරතුර (Py_ISDIGIT(c));
        නම් (c != '_') {
            bha_nga;
        }
        c = tok_nextc(tok);
        නම් (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            ආපසු 0;
        }
    }
    ආපසු c;
}

පූර්ණ_සංඛ්‍යා
_PyLexer_scan_number(sa_nracanaa tok_state *tok, sa_nracanaa token *token, පූර්ණ_සංඛ්‍යා c,
                     පූර්ණ_සංඛ්‍යා leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    නම් (leading_dot) {
        goto fraction;
    }
    නම් (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        නම් (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            do {
                නම් (c == '_') {
                    c = tok_nextc(tok);
                }
                නම් (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    ආපසු MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                do {
                    c = tok_nextc(tok);
                } අතරතුර (Py_ISXDIGIT(c));
            } අතරතුර (c == '_');
            නම් (!verify_end_of_number(tok, c, "hexadecimal")) {
                ආපසු MAKE_TOKEN(ERRORTOKEN);
            }
        }
        නැතහොත් නම් (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            do {
                නම් (c == '_') {
                    c = tok_nextc(tok);
                }
                නම් (c < '0' || c >= '8') {
                    නම් (Py_ISDIGIT(c)) {
                        ආපසු MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    නැතහොත් {
                        tok_backup(tok, c);
                        ආපසු MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } අතරතුර ('0' <= c && c < '8');
            } අතරතුර (c == '_');
            නම් (Py_ISDIGIT(c)) {
                ආපසු MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            නම් (!verify_end_of_number(tok, c, "octal")) {
                ආපසු MAKE_TOKEN(ERRORTOKEN);
            }
        }
        නැතහොත් නම් (c == 'b' || c == 'B') {
            /* Binary */
            c = tok_nextc(tok);
            do {
                නම් (c == '_') {
                    c = tok_nextc(tok);
                }
                නම් (c != '0' && c != '1') {
                    නම් (Py_ISDIGIT(c)) {
                        ආපසු MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    නැතහොත් {
                        tok_backup(tok, c);
                        ආපසු MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                do {
                    c = tok_nextc(tok);
                } අතරතුර (c == '0' || c == '1');
            } අතරතුර (c == '_');
            නම් (Py_ISDIGIT(c)) {
                ආපසු MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            නම් (!verify_end_of_number(tok, c, "binary")) {
                ආපසු MAKE_TOKEN(ERRORTOKEN);
            }
        }
        නැතහොත් {
            පූර්ණ_සංඛ්‍යා nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            අතරතුර (1) {
                නම් (c == '_') {
                    c = tok_nextc(tok);
                    නම් (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ආපසු MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                නම් (c != '0') {
                    bha_nga;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            නම් (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                නම් (c == 0) {
                    ආපසු MAKE_TOKEN(ERRORTOKEN);
                }
            }
            නම් (c == '.') {
                c = tok_nextc(tok);
                goto fraction;
            }
            නැතහොත් නම් (c == 'e' || c == 'E') {
                goto exponent;
            }
            නැතහොත් නම් (c == 'j' || c == 'J') {
                goto imaginary;
            }
            නැතහොත් නම් (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                ආපසු MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (පූර්ණ_සංඛ්‍යා)(tok->start + 1 - tok->line_start),
                        (පූර්ණ_සංඛ්‍යා)(zeros_end - tok->line_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            නම් (!verify_end_of_number(tok, c, "decimal")) {
                ආපසු MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    නැතහොත් {
        /* Decimal */
        c = tok_decimal_tail(tok);
        නම් (c == 0) {
            ආපසු MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            නම් (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                නම් (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    නම් (c == 0) {
                        ආපසු MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            නම් (c == 'e' || c == 'E') {
                පූර්ණ_සංඛ්‍යා e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                නම් (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    නම් (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        ආපසු MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } නැතහොත් නම් (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    නම් (!verify_end_of_number(tok, e, "decimal")) {
                        ආපසු MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    ආපසු MAKE_TOKEN(NUMBER);
                }
                c = tok_decimal_tail(tok);
                නම් (c == 0) {
                    ආපසු MAKE_TOKEN(ERRORTOKEN);
                }
            }
            නම් (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                නම් (!verify_end_of_number(tok, c, "imaginary")) {
                    ආපසු MAKE_TOKEN(ERRORTOKEN);
                }
            }
            නැතහොත් නම් (!verify_end_of_number(tok, c, "decimal")) {
                ආපසු MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    ආපසු MAKE_TOKEN(NUMBER);
}
