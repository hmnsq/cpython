#அந்தர்பாவ "Python.h"
#அந்தர்பாவ "pycore_token.h"

#அந்தர்பாவ "lexer_internal.h"
#அந்தர்பாவ "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

நிலையான முழு
lookahead(கட்டமைப்பு tok_state *tok, மாறிலி எழுத்து *test)
{
    மாறிலி எழுத்து *s = test;
    முழு res = 0;
    போது (1) {
        முழு c = tok_nextc(tok);
        எனில் (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        இல்லையெனில் எனில் (c == *s) {
            s++;
            தொடர்;
        }

        tok_backup(tok, c);
        போது (s != test) {
            tok_backup(tok, *--s);
        }
        திரும்பு res;
    }
}

நிலையான முழு
verify_end_of_எண்(கட்டமைப்பு tok_state *tok, முழு c, மாறிலி எழுத்து *kind) {
    எனில் (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        திரும்பு 1;
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
    முழு r = 0;
    எனில் (c == 'a') {
        r = lookahead(tok, "nd");
    }
    இல்லையெனில் எனில் (c == 'e') {
        r = lookahead(tok, "lse");
    }
    இல்லையெனில் எனில் (c == 'f') {
        r = lookahead(tok, "or");
    }
    இல்லையெனில் எனில் (c == 'i') {
        முழு c2 = tok_nextc(tok);
        எனில் (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    இல்லையெனில் எனில் (c == 'o') {
        r = lookahead(tok, "r");
    }
    இல்லையெனில் எனில் (c == 'n') {
        r = lookahead(tok, "ot");
    }
    எனில் (r) {
        tok_backup(tok, c);
        எனில் (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            திரும்பு 0;
        }
        tok_nextc(tok);
    }
    இல்லையெனில் /* In future releases, only error will remain. */
    எனில் (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        திரும்பு 0;
    }
    திரும்பு 1;
}

நிலையான முழு
tok_decimal_tail(கட்டமைப்பு tok_state *tok)
{
    முழு c;

    போது (1) {
        செய் {
            c = tok_nextc(tok);
        } போது (Py_ISDIGIT(c));
        எனில் (c != '_') {
            நிறுத்து;
        }
        c = tok_nextc(tok);
        எனில் (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            திரும்பு 0;
        }
    }
    திரும்பு c;
}

முழு
Py_Lexer_scan_எண்(கட்டமைப்பு tok_state *tok, கட்டமைப்பு token *token, முழு c,
                     முழு leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    எனில் (leading_dot) {
        செல் fraction;
    }
    எனில் (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        எனில் (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            செய் {
                எனில் (c == '_') {
                    c = tok_nextc(tok);
                }
                எனில் (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    திரும்பு MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                செய் {
                    c = tok_nextc(tok);
                } போது (Py_ISXDIGIT(c));
            } போது (c == '_');
            எனில் (!verify_end_of_எண்(tok, c, "hexadecimal")) {
                திரும்பு MAKE_TOKEN(ERRORTOKEN);
            }
        }
        இல்லையெனில் எனில் (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            செய் {
                எனில் (c == '_') {
                    c = tok_nextc(tok);
                }
                எனில் (c < '0' || c >= '8') {
                    எனில் (Py_ISDIGIT(c)) {
                        திரும்பு MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    இல்லையெனில் {
                        tok_backup(tok, c);
                        திரும்பு MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                செய் {
                    c = tok_nextc(tok);
                } போது ('0' <= c && c < '8');
            } போது (c == '_');
            எனில் (Py_ISDIGIT(c)) {
                திரும்பு MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            எனில் (!verify_end_of_எண்(tok, c, "octal")) {
                திரும்பு MAKE_TOKEN(ERRORTOKEN);
            }
        }
        இல்லையெனில் எனில் (c == 'b' || c == 'B') {
            /*இருமம்*/
            c = tok_nextc(tok);
            செய் {
                எனில் (c == '_') {
                    c = tok_nextc(tok);
                }
                எனில் (c != '0' && c != '1') {
                    எனில் (Py_ISDIGIT(c)) {
                        திரும்பு MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    இல்லையெனில் {
                        tok_backup(tok, c);
                        திரும்பு MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                செய் {
                    c = tok_nextc(tok);
                } போது (c == '0' || c == '1');
            } போது (c == '_');
            எனில் (Py_ISDIGIT(c)) {
                திரும்பு MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            எனில் (!verify_end_of_எண்(tok, c, "binary")) {
                திரும்பு MAKE_TOKEN(ERRORTOKEN);
            }
        }
        இல்லையெனில் {
            முழு nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            போது (1) {
                எனில் (c == '_') {
                    c = tok_nextc(tok);
                    எனில் (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        திரும்பு MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                எனில் (c != '0') {
                    நிறுத்து;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            எனில் (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                எனில் (c == 0) {
                    திரும்பு MAKE_TOKEN(ERRORTOKEN);
                }
            }
            எனில் (c == '.') {
                c = tok_nextc(tok);
                செல் fraction;
            }
            இல்லையெனில் எனில் (c == 'e' || c == 'E') {
                செல் exponent;
            }
            இல்லையெனில் எனில் (c == 'j' || c == 'J') {
                செல் imaginary;
            }
            இல்லையெனில் எனில் (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                திரும்பு MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (முழு)(tok->start + 1 - tok->வரி_start),
                        (முழு)(zeros_end - tok->வரி_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            எனில் (!verify_end_of_எண்(tok, c, "decimal")) {
                திரும்பு MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    இல்லையெனில் {
        /* Decimal */
        c = tok_decimal_tail(tok);
        எனில் (c == 0) {
            திரும்பு MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            எனில் (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                எனில் (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    எனில் (c == 0) {
                        திரும்பு MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            எனில் (c == 'e' || c == 'E') {
                முழு e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                எனில் (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    எனில் (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        திரும்பு MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } இல்லையெனில் எனில் (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    எனில் (!verify_end_of_எண்(tok, e, "decimal")) {
                        திரும்பு MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    திரும்பு MAKE_TOKEN(எண்);
                }
                c = tok_decimal_tail(tok);
                எனில் (c == 0) {
                    திரும்பு MAKE_TOKEN(ERRORTOKEN);
                }
            }
            எனில் (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                எனில் (!verify_end_of_எண்(tok, c, "imaginary")) {
                    திரும்பு MAKE_TOKEN(ERRORTOKEN);
                }
            }
            இல்லையெனில் எனில் (!verify_end_of_எண்(tok, c, "decimal")) {
                திரும்பு MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    திரும்பு MAKE_TOKEN(எண்);
}
