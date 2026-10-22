#अन्तर्भाव "Python.h"
#अन्तर्भाव "pycore_token.h"

#अन्तर्भाव "lexer_internal.h"
#अन्तर्भाव "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

स्थिर पूर्णांक
lookahead(संरचना tok_state *tok, स्थिरांक अक्षर *test)
{
    स्थिरांक अक्षर *s = test;
    पूर्णांक res = 0;
    पर्यंत (1) {
        पूर्णांक c = tok_nextc(tok);
        जर (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        अन्यथा जर (c == *s) {
            s++;
            सुरूठेवा;
        }

        tok_backup(tok, c);
        पर्यंत (s != test) {
            tok_backup(tok, *--s);
        }
        परत res;
    }
}

स्थिर पूर्णांक
verify_end_of_संख्या(संरचना tok_state *tok, पूर्णांक c, स्थिरांक अक्षर *kind) {
    जर (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        परत 1;
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
    पूर्णांक r = 0;
    जर (c == 'a') {
        r = lookahead(tok, "nd");
    }
    अन्यथा जर (c == 'e') {
        r = lookahead(tok, "lse");
    }
    अन्यथा जर (c == 'f') {
        r = lookahead(tok, "or");
    }
    अन्यथा जर (c == 'i') {
        पूर्णांक c2 = tok_nextc(tok);
        जर (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    अन्यथा जर (c == 'o') {
        r = lookahead(tok, "r");
    }
    अन्यथा जर (c == 'n') {
        r = lookahead(tok, "ot");
    }
    जर (r) {
        tok_backup(tok, c);
        जर (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            परत 0;
        }
        tok_nextc(tok);
    }
    अन्यथा /* In future releases, only error will remain. */
    जर (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        परत 0;
    }
    परत 1;
}

स्थिर पूर्णांक
tok_decimal_tail(संरचना tok_state *tok)
{
    पूर्णांक c;

    पर्यंत (1) {
        करा {
            c = tok_nextc(tok);
        } पर्यंत (Py_ISDIGIT(c));
        जर (c != '_') {
            थांबा;
        }
        c = tok_nextc(tok);
        जर (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            परत 0;
        }
    }
    परत c;
}

पूर्णांक
Py_Lexer_scan_संख्या(संरचना tok_state *tok, संरचना token *token, पूर्णांक c,
                     पूर्णांक leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    जर (leading_dot) {
        जा fraction;
    }
    जर (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        जर (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            करा {
                जर (c == '_') {
                    c = tok_nextc(tok);
                }
                जर (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    परत MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                करा {
                    c = tok_nextc(tok);
                } पर्यंत (Py_ISXDIGIT(c));
            } पर्यंत (c == '_');
            जर (!verify_end_of_संख्या(tok, c, "hexadecimal")) {
                परत MAKE_TOKEN(ERRORTOKEN);
            }
        }
        अन्यथा जर (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            करा {
                जर (c == '_') {
                    c = tok_nextc(tok);
                }
                जर (c < '0' || c >= '8') {
                    जर (Py_ISDIGIT(c)) {
                        परत MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    अन्यथा {
                        tok_backup(tok, c);
                        परत MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                करा {
                    c = tok_nextc(tok);
                } पर्यंत ('0' <= c && c < '8');
            } पर्यंत (c == '_');
            जर (Py_ISDIGIT(c)) {
                परत MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            जर (!verify_end_of_संख्या(tok, c, "octal")) {
                परत MAKE_TOKEN(ERRORTOKEN);
            }
        }
        अन्यथा जर (c == 'b' || c == 'B') {
            /*द्विमान*/
            c = tok_nextc(tok);
            करा {
                जर (c == '_') {
                    c = tok_nextc(tok);
                }
                जर (c != '0' && c != '1') {
                    जर (Py_ISDIGIT(c)) {
                        परत MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    अन्यथा {
                        tok_backup(tok, c);
                        परत MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                करा {
                    c = tok_nextc(tok);
                } पर्यंत (c == '0' || c == '1');
            } पर्यंत (c == '_');
            जर (Py_ISDIGIT(c)) {
                परत MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            जर (!verify_end_of_संख्या(tok, c, "binary")) {
                परत MAKE_TOKEN(ERRORTOKEN);
            }
        }
        अन्यथा {
            पूर्णांक nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            पर्यंत (1) {
                जर (c == '_') {
                    c = tok_nextc(tok);
                    जर (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        परत MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                जर (c != '0') {
                    थांबा;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            जर (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                जर (c == 0) {
                    परत MAKE_TOKEN(ERRORTOKEN);
                }
            }
            जर (c == '.') {
                c = tok_nextc(tok);
                जा fraction;
            }
            अन्यथा जर (c == 'e' || c == 'E') {
                जा exponent;
            }
            अन्यथा जर (c == 'j' || c == 'J') {
                जा imaginary;
            }
            अन्यथा जर (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                परत MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (पूर्णांक)(tok->start + 1 - tok->ओळ_start),
                        (पूर्णांक)(zeros_end - tok->ओळ_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            जर (!verify_end_of_संख्या(tok, c, "decimal")) {
                परत MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    अन्यथा {
        /* Decimal */
        c = tok_decimal_tail(tok);
        जर (c == 0) {
            परत MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            जर (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                जर (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    जर (c == 0) {
                        परत MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            जर (c == 'e' || c == 'E') {
                पूर्णांक e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                जर (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    जर (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        परत MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } अन्यथा जर (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    जर (!verify_end_of_संख्या(tok, e, "decimal")) {
                        परत MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    परत MAKE_TOKEN(संख्या);
                }
                c = tok_decimal_tail(tok);
                जर (c == 0) {
                    परत MAKE_TOKEN(ERRORTOKEN);
                }
            }
            जर (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                जर (!verify_end_of_संख्या(tok, c, "imaginary")) {
                    परत MAKE_TOKEN(ERRORTOKEN);
                }
            }
            अन्यथा जर (!verify_end_of_संख्या(tok, c, "decimal")) {
                परत MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    परत MAKE_TOKEN(संख्या);
}
