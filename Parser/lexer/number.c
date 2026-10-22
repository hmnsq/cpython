#അന്തര്ഭാവ "Python.h"
#അന്തര്ഭാവ "pycore_token.h"

#അന്തര്ഭാവ "lexer_internal.h"
#അന്തര്ഭാവ "../tokenizer/helpers.h"

#define MAKE_TOKEN(token_type) _PyLexer_token_setup(tok, token, token_type, p_start, p_end)

സ്ഥിതം പൂർണ്ണസംഖ്യ
lookahead(ഘടന tok_state *tok, സ്ഥിരം അക്ഷരം *test)
{
    സ്ഥിരം അക്ഷരം *s = test;
    പൂർണ്ണസംഖ്യ res = 0;
    ആകുമ്പോൾ (1) {
        പൂർണ്ണസംഖ്യ c = tok_nextc(tok);
        എങ്കിൽ (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        അല്ലെങ്കിൽ എങ്കിൽ (c == *s) {
            s++;
            തുടരുക;
        }

        tok_backup(tok, c);
        ആകുമ്പോൾ (s != test) {
            tok_backup(tok, *--s);
        }
        തിരികെ res;
    }
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
verify_end_of_സംഖ്യ(ഘടന tok_state *tok, പൂർണ്ണസംഖ്യ c, സ്ഥിരം അക്ഷരം *kind) {
    എങ്കിൽ (tok->tok_extra_tokens) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        തിരികെ 1;
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
    പൂർണ്ണസംഖ്യ r = 0;
    എങ്കിൽ (c == 'a') {
        r = lookahead(tok, "nd");
    }
    അല്ലെങ്കിൽ എങ്കിൽ (c == 'e') {
        r = lookahead(tok, "lse");
    }
    അല്ലെങ്കിൽ എങ്കിൽ (c == 'f') {
        r = lookahead(tok, "or");
    }
    അല്ലെങ്കിൽ എങ്കിൽ (c == 'i') {
        പൂർണ്ണസംഖ്യ c2 = tok_nextc(tok);
        എങ്കിൽ (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (c == 'o') {
        r = lookahead(tok, "r");
    }
    അല്ലെങ്കിൽ എങ്കിൽ (c == 'n') {
        r = lookahead(tok, "ot");
    }
    എങ്കിൽ (r) {
        tok_backup(tok, c);
        എങ്കിൽ (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", kind))
        {
            തിരികെ 0;
        }
        tok_nextc(tok);
    }
    അല്ലെങ്കിൽ /* In future releases, only error will remain. */
    എങ്കിൽ (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", kind);
        തിരികെ 0;
    }
    തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
tok_decimal_tail(ഘടന tok_state *tok)
{
    പൂർണ്ണസംഖ്യ c;

    ആകുമ്പോൾ (1) {
        ചെയ്യുക {
            c = tok_nextc(tok);
        } ആകുമ്പോൾ (Py_ISDIGIT(c));
        എങ്കിൽ (c != '_') {
            നിർത്തുക;
        }
        c = tok_nextc(tok);
        എങ്കിൽ (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            തിരികെ 0;
        }
    }
    തിരികെ c;
}

പൂർണ്ണസംഖ്യ
Py_Lexer_scan_സംഖ്യ(ഘടന tok_state *tok, ഘടന token *token, പൂർണ്ണസംഖ്യ c,
                     പൂർണ്ണസംഖ്യ leading_dot)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;

    എങ്കിൽ (leading_dot) {
        പോകുക fraction;
    }
    എങ്കിൽ (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        എങ്കിൽ (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            ചെയ്യുക {
                എങ്കിൽ (c == '_') {
                    c = tok_nextc(tok);
                }
                എങ്കിൽ (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    തിരികെ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                ചെയ്യുക {
                    c = tok_nextc(tok);
                } ആകുമ്പോൾ (Py_ISXDIGIT(c));
            } ആകുമ്പോൾ (c == '_');
            എങ്കിൽ (!verify_end_of_സംഖ്യ(tok, c, "hexadecimal")) {
                തിരികെ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        അല്ലെങ്കിൽ എങ്കിൽ (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            ചെയ്യുക {
                എങ്കിൽ (c == '_') {
                    c = tok_nextc(tok);
                }
                എങ്കിൽ (c < '0' || c >= '8') {
                    എങ്കിൽ (Py_ISDIGIT(c)) {
                        തിരികെ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    അല്ലെങ്കിൽ {
                        tok_backup(tok, c);
                        തിരികെ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                ചെയ്യുക {
                    c = tok_nextc(tok);
                } ആകുമ്പോൾ ('0' <= c && c < '8');
            } ആകുമ്പോൾ (c == '_');
            എങ്കിൽ (Py_ISDIGIT(c)) {
                തിരികെ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            എങ്കിൽ (!verify_end_of_സംഖ്യ(tok, c, "octal")) {
                തിരികെ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        അല്ലെങ്കിൽ എങ്കിൽ (c == 'b' || c == 'B') {
            /*ദ്വിമാന*/
            c = tok_nextc(tok);
            ചെയ്യുക {
                എങ്കിൽ (c == '_') {
                    c = tok_nextc(tok);
                }
                എങ്കിൽ (c != '0' && c != '1') {
                    എങ്കിൽ (Py_ISDIGIT(c)) {
                        തിരികെ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    അല്ലെങ്കിൽ {
                        tok_backup(tok, c);
                        തിരികെ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                ചെയ്യുക {
                    c = tok_nextc(tok);
                } ആകുമ്പോൾ (c == '0' || c == '1');
            } ആകുമ്പോൾ (c == '_');
            എങ്കിൽ (Py_ISDIGIT(c)) {
                തിരികെ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            എങ്കിൽ (!verify_end_of_സംഖ്യ(tok, c, "binary")) {
                തിരികെ MAKE_TOKEN(ERRORTOKEN);
            }
        }
        അല്ലെങ്കിൽ {
            പൂർണ്ണസംഖ്യ nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            ആകുമ്പോൾ (1) {
                എങ്കിൽ (c == '_') {
                    c = tok_nextc(tok);
                    എങ്കിൽ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        തിരികെ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                എങ്കിൽ (c != '0') {
                    നിർത്തുക;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_end = tok->cur;
            എങ്കിൽ (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_tail(tok);
                എങ്കിൽ (c == 0) {
                    തിരികെ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            എങ്കിൽ (c == '.') {
                c = tok_nextc(tok);
                പോകുക fraction;
            }
            അല്ലെങ്കിൽ എങ്കിൽ (c == 'e' || c == 'E') {
                പോകുക exponent;
            }
            അല്ലെങ്കിൽ എങ്കിൽ (c == 'j' || c == 'J') {
                പോകുക imaginary;
            }
            അല്ലെങ്കിൽ എങ്കിൽ (nonzero && !tok->tok_extra_tokens) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                തിരികെ MAKE_TOKEN(_PyTokenizer_syntaxerror_known_range(
                        tok, (പൂർണ്ണസംഖ്യ)(tok->start + 1 - tok->വരി_start),
                        (പൂർണ്ണസംഖ്യ)(zeros_end - tok->വരി_start),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            എങ്കിൽ (!verify_end_of_സംഖ്യ(tok, c, "decimal")) {
                തിരികെ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    അല്ലെങ്കിൽ {
        /* Decimal */
        c = tok_decimal_tail(tok);
        എങ്കിൽ (c == 0) {
            തിരികെ MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            എങ്കിൽ (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                എങ്കിൽ (Py_ISDIGIT(c)) {
                    c = tok_decimal_tail(tok);
                    എങ്കിൽ (c == 0) {
                        തിരികെ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            എങ്കിൽ (c == 'e' || c == 'E') {
                പൂർണ്ണസംഖ്യ e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                എങ്കിൽ (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    എങ്കിൽ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        തിരികെ MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } അല്ലെങ്കിൽ എങ്കിൽ (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    എങ്കിൽ (!verify_end_of_സംഖ്യ(tok, e, "decimal")) {
                        തിരികെ MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_start = tok->start;
                    p_end = tok->cur;
                    തിരികെ MAKE_TOKEN(സംഖ്യ);
                }
                c = tok_decimal_tail(tok);
                എങ്കിൽ (c == 0) {
                    തിരികെ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            എങ്കിൽ (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                എങ്കിൽ (!verify_end_of_സംഖ്യ(tok, c, "imaginary")) {
                    തിരികെ MAKE_TOKEN(ERRORTOKEN);
                }
            }
            അല്ലെങ്കിൽ എങ്കിൽ (!verify_end_of_സംഖ്യ(tok, c, "decimal")) {
                തിരികെ MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_start = tok->start;
    p_end = tok->cur;
    തിരികെ MAKE_TOKEN(സംഖ്യ);
}
