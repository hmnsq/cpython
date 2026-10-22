#અન્તર્ભાવ "Python.h"
#અન્તર્ભાવ "pycore_token.h"

#અન્તર્ભાવ "lexer_internal.h"
#અન્તર્ભાવ "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

સ્થિર પૂર્ણાંક
lookahead(સંરચના tok_state *tok, અચલ અક્ષર *test)
{
    અચલ અક્ષર *s = test;
    પૂર્ણાંક res = 0;
    જ્યારે (1) {
        પૂર્ણાંક c = tok_nextc(tok);
        જો (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        નહીંતર જો (c == *s) {
            s++;
            ચાલુ;
        }

        tok_backup(tok, c);
        જ્યારે (s != test) {
            tok_backup(tok, *--s);
        }
        પાછા res;
    }
}

સ્થિર પૂર્ણાંક
verify_end_of_સંખ્યા(સંરચના tok_state *tok, પૂર્ણાંક c, અચલ અક્ષર *kind) {
    જો (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        પાછા 1;
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
    પૂર્ણાંક r = 0;
    જો (c == 'a') {
        r = lookahead(tok, "nd");
    }
    નહીંતર જો (c == 'e') {
        r = lookahead(tok, "lse");
    }
    નહીંતર જો (c == 'f') {
        r = lookahead(tok, "or");
    }
    નહીંતર જો (c == 'i') {
        પૂર્ણાંક c2 = tok_nextc(tok);
        જો (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    નહીંતર જો (c == 'o') {
        r = lookahead(tok, "r");
    }
    નહીંતર જો (c == 'n') {
        r = lookahead(tok, "ot");
    }
    જો (r) {
        tok_backup(tok, c);
        જો (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            પાછા 0;
        }
        tok_nextc(tok);
    }
    નહીંતર /* In future releases, only error will remain. */
    જો (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        પાછા 0;
    }
    પાછા 1;
}

સ્થિર પૂર્ણાંક
tok_decimal_tail(સંરચના tok_state *tok)
{
    પૂર્ણાંક c;

    જ્યારે (1) {
        કરો {
            c = tok_nextc(tok);
        } જ્યારે (Py_ISDIGIT(c));
        જો (c != '_') {
            તોડો;
        }
        c = tok_nextc(tok);
        જો (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            પાછા 0;
        }
    }
    પાછા c;
}

પૂર્ણાંક
Py_Lexer_scan_સંખ્યા(સંરચના tok_state *tok, સંરચના token *token, પૂર્ણાંક c,
                     પૂર્ણાંક leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    જો (leading_dot) {
        જાઓ fraction;
    }
    જો (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        જો (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            કરો {
                જો (c == '_') {
                    c = tok_nextc(tok);
                }
                જો (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    પાછા MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                કરો {
                    c = tok_nextc(tok);
                } જ્યારે (Py_ISXDIGIT(c));
            } જ્યારે (c == '_');
            જો (!verify_end_of_સંખ્યા(tok, c, "hexadecimal")) {
                પાછા MAKE_TOKEN(ERRORTOKEN);
            }
        }
        નહીંતર જો (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            કરો {
                જો (c == '_') {
                    c = tok_nextc(tok);
                }
                જો (c < '0' || c >= '8') {
                    જો (Py_ISDIGIT(c)) {
                        પાછા MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    નહીંતર {
                        tok_backup(tok, c);
                        પાછા MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                કરો {
                    c = tok_nextc(tok);
                } જ્યારે ('0' <= c && c < '8');
            } જ્યારે (c == '_');
            જો (Py_ISDIGIT(c)) {
                પાછા MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            જો (!verify_end_of_સંખ્યા(tok, c, "octal")) {
                પાછા MAKE_TOKEN(ERRORTOKEN);
            }
        }
        નહીંતર જો (c == 'b' || c == 'B') {
            /*દ્વિઅંકી*/
            c = tok_nextc(tok);
            કરો {
                જો (c == '_') {
                    c = tok_nextc(tok);
                }
                જો (c != '0' && c != '1') {
                    જો (Py_ISDIGIT(c)) {
                        પાછા MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    નહીંતર {
                        tok_backup(tok, c);
                        પાછા MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                કરો {
                    c = tok_nextc(tok);
                } જ્યારે (c == '0' || c == '1');
            } જ્યારે (c == '_');
            જો (Py_ISDIGIT(c)) {
                પાછા MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            જો (!verify_end_of_સંખ્યા(tok, c, "binary")) {
                પાછા MAKE_TOKEN(ERRORTOKEN);
            }
        }
        નહીંતર {
            પૂર્ણાંક nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            જ્યારે (1) {
                જો (c == '_') {
                    c = tok_nextc(tok);
                    જો (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        પાછા MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                જો (c != '0') {
                    તોડો;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            જો (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                જો (c == 0) {
                    પાછા MAKE_TOKEN(ERRORTOKEN);
                }
            }
            જો (c == '.') {
                c = tok_nextc(tok);
                જાઓ fraction;
            }
            નહીંતર જો (c == 'e' || c == 'E') {
                જાઓ exponent;
            }
            નહીંતર જો (c == 'j' || c == 'J') {
                જાઓ imaginary;
            }
            નહીંતર જો (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                પાછા MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (પૂર્ણાંક)(tok->start + 1 - tok->લીટી_start),
                        (પૂર્ણાંક)(zeros_end - tok->લીટી_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            જો (!verify_end_of_સંખ્યા(tok, c, "decimal")) {
                પાછા MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    નહીંતર {
        /* Decimal */
        c = tok_decimal_tail(tok);
        જો (c == 0) {
            પાછા MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            જો (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                જો (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    જો (c == 0) {
                        પાછા MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            જો (c == 'e' || c == 'E') {
                પૂર્ણાંક e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                જો (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    જો (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        પાછા MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } નહીંતર જો (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    જો (!verify_end_of_સંખ્યા(tok, e, "decimal")) {
                        પાછા MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    પાછા MAKE_TOKEN(સંખ્યા);
                }
                c = tok_decimal_tail(tok);
                જો (c == 0) {
                    પાછા MAKE_TOKEN(ERRORTOKEN);
                }
            }
            જો (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                જો (!verify_end_of_સંખ્યા(tok, c, "imaginary")) {
                    પાછા MAKE_TOKEN(ERRORTOKEN);
                }
            }
            નહીંતર જો (!verify_end_of_સંખ્યા(tok, c, "decimal")) {
                પાછા MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    પાછા MAKE_TOKEN(સંખ્યા);
}
