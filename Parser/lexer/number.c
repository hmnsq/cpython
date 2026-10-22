#అన్తర్భావ "Python.h"
#అన్తర్భావ "pycore_token.h"

#అన్తర్భావ "lexer_internal.h"
#అన్తర్భావ "../tokenizer/helpers.h"

#నిర్వచించు MAKE_TOKEN(token_type) Py_Lexer_token_అమరిక(tok, token, token_type, p_ప్రారంభం, p_చివర)

స్థిర పూర్ణాంకం
lookahead(నిర్మాణం tok_state *tok, స్థిరరాశి అక్షరం *test)
{
    స్థిరరాశి అక్షరం *s = test;
    పూర్ణాంకం res = 0;
    అయితే (1) {
        పూర్ణాంకం c = tok_nextc(tok);
        ఒకవేళ (*s == 0) {
            res = !is_potential_identifier_char(c);
        }
        లేకపోతే ఒకవేళ (c == *s) {
            s++;
            కొనసాగు;
        }

        tok_backup(tok, c);
        అయితే (s != test) {
            tok_backup(tok, *--s);
        }
        తిరిగి res;
    }
}

స్థిర పూర్ణాంకం
verify_చివర_యొక్క_సంఖ్య(నిర్మాణం tok_state *tok, పూర్ణాంకం c, స్థిరరాశి అక్షరం *రకం) {
    ఒకవేళ (tok->tok_extra_టోకెన్లు) {
        // When we are parsing extra tokens, we don't want to emit warnings
        // about invalid literals, because we want to be a bit more liberal.
        తిరిగి 1;
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
    పూర్ణాంకం r = 0;
    ఒకవేళ (c == 'a') {
        r = lookahead(tok, "nd");
    }
    లేకపోతే ఒకవేళ (c == 'e') {
        r = lookahead(tok, "lse");
    }
    లేకపోతే ఒకవేళ (c == 'f') {
        r = lookahead(tok, "or");
    }
    లేకపోతే ఒకవేళ (c == 'i') {
        పూర్ణాంకం c2 = tok_nextc(tok);
        ఒకవేళ (c2 == 'f' || c2 == 'n' || c2 == 's') {
            r = 1;
        }
        tok_backup(tok, c2);
    }
    లేకపోతే ఒకవేళ (c == 'o') {
        r = lookahead(tok, "r");
    }
    లేకపోతే ఒకవేళ (c == 'n') {
        r = lookahead(tok, "ot");
    }
    ఒకవేళ (r) {
        tok_backup(tok, c);
        ఒకవేళ (_PyTokenizer_parser_warn(tok, PyExc_SyntaxWarning,
                "invalid %s literal", రకం))
        {
            తిరిగి 0;
        }
        tok_nextc(tok);
    }
    లేకపోతే /* In future releases, only error will remain. */
    ఒకవేళ (c < 128 && is_potential_identifier_char(c)) {
        tok_backup(tok, c);
        _PyTokenizer_syntaxerror(tok, "invalid %s literal", రకం);
        తిరిగి 0;
    }
    తిరిగి 1;
}

స్థిర పూర్ణాంకం
tok_decimal_తోక(నిర్మాణం tok_state *tok)
{
    పూర్ణాంకం c;

    అయితే (1) {
        చేయి {
            c = tok_nextc(tok);
        } అయితే (Py_ISDIGIT(c));
        ఒకవేళ (c != '_') {
            ఆపు;
        }
        c = tok_nextc(tok);
        ఒకవేళ (!Py_ISDIGIT(c)) {
            tok_backup(tok, c);
            _PyTokenizer_syntaxerror(tok, "invalid decimal literal");
            తిరిగి 0;
        }
    }
    తిరిగి c;
}

పూర్ణాంకం
Py_Lexer_పరిశీలన_సంఖ్య(నిర్మాణం tok_state *tok, నిర్మాణం token *token, పూర్ణాంకం c,
                     పూర్ణాంకం leading_dot)
{
    _PyTok_Off p_ప్రారంభం = -1;
    _PyTok_Off p_చివర = -1;

    ఒకవేళ (leading_dot) {
        వెళ్ళు fraction;
    }
    ఒకవేళ (c == '0') {
        /* Hex, octal or binary -- maybe. */
        c = tok_nextc(tok);
        ఒకవేళ (c == 'x' || c == 'X') {
            /* Hex */
            c = tok_nextc(tok);
            చేయి {
                ఒకవేళ (c == '_') {
                    c = tok_nextc(tok);
                }
                ఒకవేళ (!Py_ISXDIGIT(c)) {
                    tok_backup(tok, c);
                    తిరిగి MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                }
                చేయి {
                    c = tok_nextc(tok);
                } అయితే (Py_ISXDIGIT(c));
            } అయితే (c == '_');
            ఒకవేళ (!verify_చివర_యొక్క_సంఖ్య(tok, c, "hexadecimal")) {
                తిరిగి MAKE_TOKEN(ERRORTOKEN);
            }
        }
        లేకపోతే ఒకవేళ (c == 'o' || c == 'O') {
            /* Octal */
            c = tok_nextc(tok);
            చేయి {
                ఒకవేళ (c == '_') {
                    c = tok_nextc(tok);
                }
                ఒకవేళ (c < '0' || c >= '8') {
                    ఒకవేళ (Py_ISDIGIT(c)) {
                        తిరిగి MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                                "invalid digit '%c' in octal literal", c));
                    }
                    లేకపోతే {
                        tok_backup(tok, c);
                        తిరిగి MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid octal literal"));
                    }
                }
                చేయి {
                    c = tok_nextc(tok);
                } అయితే ('0' <= c && c < '8');
            } అయితే (c == '_');
            ఒకవేళ (Py_ISDIGIT(c)) {
                తిరిగి MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                        "invalid digit '%c' in octal literal", c));
            }
            ఒకవేళ (!verify_చివర_యొక్క_సంఖ్య(tok, c, "octal")) {
                తిరిగి MAKE_TOKEN(ERRORTOKEN);
            }
        }
        లేకపోతే ఒకవేళ (c == 'b' || c == 'B') {
            /*ద్విమాన*/
            c = tok_nextc(tok);
            చేయి {
                ఒకవేళ (c == '_') {
                    c = tok_nextc(tok);
                }
                ఒకవేళ (c != '0' && c != '1') {
                    ఒకవేళ (Py_ISDIGIT(c)) {
                        తిరిగి MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
                    }
                    లేకపోతే {
                        tok_backup(tok, c);
                        తిరిగి MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid binary literal"));
                    }
                }
                చేయి {
                    c = tok_nextc(tok);
                } అయితే (c == '0' || c == '1');
            } అయితే (c == '_');
            ఒకవేళ (Py_ISDIGIT(c)) {
                తిరిగి MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid digit '%c' in binary literal", c));
            }
            ఒకవేళ (!verify_చివర_యొక్క_సంఖ్య(tok, c, "binary")) {
                తిరిగి MAKE_TOKEN(ERRORTOKEN);
            }
        }
        లేకపోతే {
            పూర్ణాంకం nonzero = 0;
            /* maybe old-style octal; c is first char of it */
            /* in any case, allow '0' as a literal */
            అయితే (1) {
                ఒకవేళ (c == '_') {
                    c = tok_nextc(tok);
                    ఒకవేళ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        తిరిగి MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                }
                ఒకవేళ (c != '0') {
                    ఆపు;
                }
                c = tok_nextc(tok);
            }
            _PyTok_Off zeros_చివర = tok->cur;
            ఒకవేళ (Py_ISDIGIT(c)) {
                nonzero = 1;
                c = tok_decimal_తోక(tok);
                ఒకవేళ (c == 0) {
                    తిరిగి MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ఒకవేళ (c == '.') {
                c = tok_nextc(tok);
                వెళ్ళు fraction;
            }
            లేకపోతే ఒకవేళ (c == 'e' || c == 'E') {
                వెళ్ళు exponent;
            }
            లేకపోతే ఒకవేళ (c == 'j' || c == 'J') {
                వెళ్ళు imaginary;
            }
            లేకపోతే ఒకవేళ (nonzero && !tok->tok_extra_టోకెన్లు) {
                /* Old-style octal: now disallowed. */
                tok_backup(tok, c);
                తిరిగి MAKE_TOKEN(Py_Tokenizer_syntaxerror_తెలిసిన_range(
                        tok, (పూర్ణాంకం)(tok->ప్రారంభం + 1 - tok->పంక్తి_ప్రారంభం),
                        (పూర్ణాంకం)(zeros_చివర - tok->పంక్తి_ప్రారంభం),
                        "leading zeros in decimal integer "
                        "literals are not permitted; "
                        "use an 0o prefix for octal integers"));
            }
            ఒకవేళ (!verify_చివర_యొక్క_సంఖ్య(tok, c, "decimal")) {
                తిరిగి MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    లేకపోతే {
        /* Decimal */
        c = tok_decimal_తోక(tok);
        ఒకవేళ (c == 0) {
            తిరిగి MAKE_TOKEN(ERRORTOKEN);
        }
        {
            /* Accept floating-point numbers. */
            ఒకవేళ (c == '.') {
                c = tok_nextc(tok);
    fraction:
                /* Fraction */
                ఒకవేళ (Py_ISDIGIT(c)) {
                    c = tok_decimal_తోక(tok);
                    ఒకవేళ (c == 0) {
                        తిరిగి MAKE_TOKEN(ERRORTOKEN);
                    }
                }
            }
            ఒకవేళ (c == 'e' || c == 'E') {
                పూర్ణాంకం e;
              exponent:
                e = c;
                /* Exponent part */
                c = tok_nextc(tok);
                ఒకవేళ (c == '+' || c == '-') {
                    c = tok_nextc(tok);
                    ఒకవేళ (!Py_ISDIGIT(c)) {
                        tok_backup(tok, c);
                        తిరిగి MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }
                } లేకపోతే ఒకవేళ (!Py_ISDIGIT(c)) {
                    tok_backup(tok, c);
                    ఒకవేళ (!verify_చివర_యొక్క_సంఖ్య(tok, e, "decimal")) {
                        తిరిగి MAKE_TOKEN(ERRORTOKEN);
                    }
                    tok_backup(tok, e);
                    p_ప్రారంభం = tok->ప్రారంభం;
                    p_చివర = tok->cur;
                    తిరిగి MAKE_TOKEN(సంఖ్య);
                }
                c = tok_decimal_తోక(tok);
                ఒకవేళ (c == 0) {
                    తిరిగి MAKE_TOKEN(ERRORTOKEN);
                }
            }
            ఒకవేళ (c == 'j' || c == 'J') {
                /* Imaginary part */
    imaginary:
                c = tok_nextc(tok);
                ఒకవేళ (!verify_చివర_యొక్క_సంఖ్య(tok, c, "imaginary")) {
                    తిరిగి MAKE_TOKEN(ERRORTOKEN);
                }
            }
            లేకపోతే ఒకవేళ (!verify_చివర_యొక్క_సంఖ్య(tok, c, "decimal")) {
                తిరిగి MAKE_TOKEN(ERRORTOKEN);
            }
        }
    }
    tok_backup(tok, c);
    p_ప్రారంభం = tok->ప్రారంభం;
    p_చివర = tok->cur;
    తిరిగి MAKE_TOKEN(సంఖ్య);
}
