#అన్తర్భావ "Python.h"
#అన్తర్భావ "pycore_token.h"
#అన్తర్భావ "pycore_unicodeobject.h"
#అన్తర్భావ "errcode.h"

#అన్తర్భావ "lexer_internal.h"
#అన్తర్భావ "../tokenizer/helpers.h"
#అన్తర్భావ "../tokenizer/reader.h"

#నిర్వచించు MAKE_TOKEN(token_type) Py_Lexer_token_అమరిక(tok, token, token_type, p_ప్రారంభం, p_చివర)

/* Spaces in this constant are treated as "zero or more spaces or tabs" when
   tokenizing. */
స్థిర స్థిరరాశి అక్షరం* type_comment_ఉపసర్గ = "# type: ";

పూర్ణాంకం
_PyLexer_refill(నిర్మాణం tok_state *tok)
{
    ఒకవేళ (tok->పూర్తి != E_OK) {
        తిరిగి 0;
    }
    పూర్ణాంకం rc = _PyTok_ReaderUnderflow(tok);
#ఒకవేళ defined(Py_DEBUG)
    ఒకవేళ (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, Py_Tok_ఆధారం_సూచిక(&tok->ఆధారం, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->పూర్తి);
    }
#అయితే_ముగింపు
    ఒకవేళ (!rc) {
        tok->cur = tok->inp;
        తిరిగి 0;
    }
    tok->పంక్తి_ప్రారంభం = tok->cur;
    Py_Tok_పరిధి పంక్తి = {tok->పంక్తి_ప్రారంభం, tok->inp};
    ఒకవేళ (Py_Tok_ఆధారం_కనుగొను_Byte(&tok->ఆధారం, పంక్తి, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        తిరిగి 0;
    }
    తిరిగి 1;
}

/* Back-up one character */
శూన్య
_PyLexer_backup(నిర్మాణం tok_state *tok, పూర్ణాంకం c)
{
    ఒకవేళ (c != EOF) {
        ఒకవేళ (--tok->cur < tok->buf_స్థానభ్రంశం) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        ఒకవేళ (Py_Tok_ఆధారం_Byte(&tok->ఆధారం, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
స్థిర పూర్ణాంకం
verify_identifier(నిర్మాణం tok_state *tok)
{
    ఒకవేళ (tok->tok_extra_టోకెన్లు) {
        తిరిగి 1;
    }
    PyObject *s;
    ఒకవేళ (tok_failed(tok))
        తిరిగి 0;
    s = PyUnicode_DecodeUTF8(Py_Tok_ఆధారం_సూచిక(&tok->ఆధారం, tok->ప్రారంభం),
                             tok->cur - tok->ప్రారంభం, ఖాళీ);
    ఒకవేళ (s == ఖాళీ) {
        ఒకవేళ (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->పూర్తి = E_DECODE;
        }
        లేకపోతే {
            tok->పూర్తి = E_ERROR;
        }
        తిరిగి 0;
    }
    Py_ssize_t invalid = Py_Unicode_పరిశీలన_Identifier(s);
    ప్రతిజ్ఞా(invalid >= 0);
    ప్రతిజ్ఞా(Py_Unicode_GET_నిడివి(s) > 0);
    ఒకవేళ (invalid < Py_Unicode_GET_నిడివి(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        ఒకవేళ (invalid + 1 < Py_Unicode_GET_నిడివి(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            ఒకవేళ (s != ఖాళీ) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            ఒకవేళ (s == ఖాళీ) {
                tok->పూర్తి = E_ERROR;
                తిరిగి 0;
            }
            error_cursor = tok->ప్రారంభం + Py_బైట్లు_GET_కొలత(s);
        }
        Py_DECREF(s);
        ఒకవేళ (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, Py_Tok_ఆధారం_సూచిక(&tok->ఆధారం, tok->పంక్తి_ప్రారంభం),
                error_cursor - tok->పంక్తి_ప్రారంభం, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        లేకపోతే {
            _PyTokenizer_syntaxerror_at(
                tok, Py_Tok_ఆధారం_సూచిక(&tok->ఆధారం, tok->పంక్తి_ప్రారంభం),
                error_cursor - tok->పంక్తి_ప్రారంభం, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        తిరిగి 0;
    }
    Py_DECREF(s);
    తిరిగి 1;
}

పూర్ణాంకం
Py_Lexer_get_సాధారణ(నిర్మాణం tok_state *tok, ftstring_state *current, నిర్మాణం token *token)
{
    ప్రతిజ్ఞా(current == ఖాళీ ||
           (current->బహుళకం == FTSTRING_బహుళకం_వ్యక్తీకరణ &&
            current->replacement_లోతు > 0));
    పూర్ణాంకం c;
    పూర్ణాంకం blankline, nonascii;

    _PyTok_Off p_ప్రారంభం = -1;
    _PyTok_Off p_చివర = -1;
  nextline:
    tok->ప్రారంభం = -1;
    tok->ప్రారంభం_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    ఒకవేళ (tok->layout.at_bol) {
        blankline = Py_Lexer_Begin_పంక్తి(tok);
        ఒకవేళ (blankline < 0) {
            తిరిగి MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->ప్రారంభం = tok->cur;
    tok->ప్రారంభం_loc = (_PyTok_Loc){
        tok->lineno, tok->పంక్తి_ప్రారంభం >= 0 ? Py_Lexer_Byte_స్తంభం(tok) : -1};

    ఒకవేళ (tok->layout.pending != 0) {
        తిరిగి _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->ప్రారంభం = -1;
    /* Skip spaces */
    చేయి {
        c = tok_nextc(tok);
    } అయితే (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->ప్రారంభం = tok->cur - 1;
    tok->ప్రారంభం_loc = (_PyTok_Loc){
        tok->lineno, tok->పంక్తి_ప్రారంభం >= 0 ? Py_Lexer_Byte_స్తంభం(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    ఒకవేళ (c == '#') {

        అయితే (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        ఒకవేళ (current != ఖాళీ) {
            _PyTok_Off comment_చివర = tok->cur;
            ఒకవేళ (c == '\n' || c == '\r') {
                comment_చివర--;
            }
            ఒకవేళ (Py_Lexer_రికార్డు_ftstring_comment(
                    tok, current, tok->ప్రారంభం, comment_చివర) < 0) {
                tok->పూర్తి = E_NOMEM;
                తిరిగి MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->ప్రారంభం;
        ఒకవేళ (tok->type_comments) {
            స్థిరరాశి అక్షరం *ఉపసర్గ = type_comment_ఉపసర్గ;
            అయితే (*ఉపసర్గ && comment < tok->cur) {
                ఒకవేళ (*ఉపసర్గ == ' ') {
                    అయితే (comment < tok->cur) {
                        పూర్ణాంకం ch = Py_Tok_ఆధారం_Byte(&tok->ఆధారం, comment);
                        ఒకవేళ (ch != ' ' && ch != '\t') {
                            ఆపు;
                        }
                        comment++;
                    }
                } లేకపోతే ఒకవేళ (*ఉపసర్గ == Py_Tok_ఆధారం_Byte(&tok->ఆధారం, comment)) {
                    comment++;
                } లేకపోతే {
                    ఆపు;
                }

                ఉపసర్గ++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            ఒకవేళ (!*ఉపసర్గ) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                పూర్ణాంకం is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(Py_Tok_ఆధారం_సూచిక(&tok->ఆధారం, comment),
                           "ignore", 6) == 0;
                ఒకవేళ (is_type_ignore && comment + 6 < tok->cur) {
                    పూర్ణాంకం ch = Py_Tok_ఆధారం_Byte(&tok->ఆధారం, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                పూర్ణాంకం type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_ప్రారంభం = comment + (is_type_ignore ? 6 : 0);
                p_చివర = tok->cur;
                పూర్ణాంకం ప్రారంభం_నిలువు_స్థానభ్రంశం = tok->ప్రారంభం_loc.byte_నిలువు +
                    (పూర్ణాంకం)(p_ప్రారంభం - tok->ప్రారంభం);
                /* Consume the newline after a standalone type ignore. */
                ఒకవేళ (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                Py_Lexer_token_అమరిక(tok, token, type, p_ప్రారంభం, p_చివర);
                token->ప్రారంభం_loc = (_PyTok_Loc){tok->lineno, ప్రారంభం_నిలువు_స్థానభ్రంశం};
                token->చివర_loc = (_PyTok_Loc){tok->lineno,
                                              Py_Lexer_Byte_స్తంభం(tok)};
                తిరిగి type;
            }
        }
        ఒకవేళ (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.లోతు > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        ఒకవేళ (tok->tok_extra_టోకెన్లు) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_ప్రారంభం = comment;
            p_చివర = tok->cur;
            tok->layout.comment_newline = blankline;
            తిరిగి MAKE_TOKEN(COMMENT);
        }
    }

    ఒకవేళ (tok->పూర్తి == E_INTERACT_STOP) {
        తిరిగి MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    ఒకవేళ (c == EOF) {
        ఒకవేళ (tok->level) {
            తిరిగి MAKE_TOKEN(ERRORTOKEN);
        }
        తిరిగి MAKE_TOKEN(tok->పూర్తి == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    ఒకవేళ (is_potential_identifier_ప్రారంభం(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        పూర్ణాంకం saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        అయితే (1) {
            ఒకవేళ (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            లేకపోతే ఒకవేళ (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            లేకపోతే ఒకవేళ (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            లేకపోతే ఒకవేళ (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            లేకపోతే ఒకవేళ (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            లేకపోతే {
                ఆపు;
            }
            c = tok_nextc(tok);
            ఒకవేళ (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                పూర్ణాంకం స్థితి_2 = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                ఒకవేళ (స్థితి_2 < 0) {
                    తిరిగి MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                ఒకవేళ (saw_f || saw_t) {
                    ftstring_రకం రకం = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    తిరిగి Py_Lexer_పరిశీలన_fstring_ప్రారంభం(tok, token, c, రకం);
                }
                తిరిగి Py_Lexer_పరిశీలన_string(tok, token, c);
            }
        }
        అయితే (is_potential_identifier_char(c)) {
            ఒకవేళ (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        ఒకవేళ (nonascii && !verify_identifier(tok)) {
            తిరిగి MAKE_TOKEN(ERRORTOKEN);
        }

        p_ప్రారంభం = tok->ప్రారంభం;
        p_చివర = tok->cur;

        తిరిగి MAKE_TOKEN(NAME);
    }

    ఒకవేళ (c == '\r') {
        c = tok_nextc(tok);
    }

    ఒకవేళ (c == '\n') {
        పూర్ణాంకం type = _PyLexer_Newline(tok, token, blankline);
        ఒకవేళ (type == 0) {
            వెళ్ళు nextline;
        }
        తిరిగి type;
    }

    /* Period or number starting with period? */
    ఒకవేళ (c == '.') {
        c = tok_nextc(tok);
        ఒకవేళ (Py_ISDIGIT(c)) {
            తిరిగి Py_Lexer_పరిశీలన_సంఖ్య(tok, token, c, 1);
        } లేకపోతే ఒకవేళ (c == '.') {
            c = tok_nextc(tok);
            ఒకవేళ (c == '.') {
                p_ప్రారంభం = tok->ప్రారంభం;
                p_చివర = tok->cur;
                తిరిగి MAKE_TOKEN(ELLIPSIS);
            }
            లేకపోతే {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        లేకపోతే {
            tok_backup(tok, c);
        }
        p_ప్రారంభం = tok->ప్రారంభం;
        p_చివర = tok->cur;
        తిరిగి MAKE_TOKEN(DOT);
    }


    /*సంఖ్య*/
    ఒకవేళ (Py_ISDIGIT(c)) {
        తిరిగి Py_Lexer_పరిశీలన_సంఖ్య(tok, token, c, 0);
    }

    /* String */
    ఒకవేళ (c == '\'' || c == '"') {
        తిరిగి Py_Lexer_పరిశీలన_string(tok, token, c);
    }

    /* Line continuation */
    ఒకవేళ (c == '\\') {
        ఒకవేళ ((c = Py_Lexer_Continue_పంక్తి(tok)) == -1) {
            తిరిగి MAKE_TOKEN(ERRORTOKEN);
        }
        వెళ్ళు again; /* Read next line */
    }

    /* Punctuation character */
    పూర్ణాంకం is_punctuation = (c == ':' || c == '}' || c == '!');
    ఒకవేళ (is_punctuation && current != ఖాళీ) {
        పూర్ణాంకం type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        ఒకవేళ (type < 0) {
            తిరిగి MAKE_TOKEN(ERRORTOKEN);
        }
        ఒకవేళ (type != 0) {
            p_ప్రారంభం = tok->ప్రారంభం;
            p_చివర = tok->cur;
            తిరిగి MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        పూర్ణాంకం c2 = tok_nextc(tok);
        పూర్ణాంకం current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        ఒకవేళ (current_token != OP) {
            పూర్ణాంకం c3 = tok_nextc(tok);
            పూర్ణాంకం current_token3 = _PyToken_ThreeChars(c, c2, c3);
            ఒకవేళ (current_token3 != OP) {
                current_token = current_token3;
            }
            లేకపోతే {
                tok_backup(tok, c3);
            }
            p_ప్రారంభం = tok->ప్రారంభం;
            p_చివర = tok->cur;
            తిరిగి MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    ఎంపిక (c) {
    స్థితి '(':
    స్థితి '[':
    స్థితి '{':
        ఒకవేళ (tok->level >= MAXLEVEL) {
            తిరిగి MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (పూర్ణాంకం)(tok->ప్రారంభం - tok->పంక్తి_ప్రారంభం);
        tok->level++;
        ఆపు;
    స్థితి ')':
    స్థితి ']':
    స్థితి '}':
        ఒకవేళ (current != ఖాళీ &&
                Py_Lexer_FTString_Bracket_లోతు(tok, current) == 0) {
            ఒకవేళ (c == '}') {
                తిరిగి MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    Py_Lexer_String_ఉపసర్గ(current->రకం)));
            }
            తిరిగి MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                Py_Lexer_String_ఉపసర్గ(current->రకం), c));
        }
        ఒకవేళ (!tok->tok_extra_టోకెన్లు && !tok->level) {
            తిరిగి MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        ఒకవేళ (tok->level > 0) {
            tok->level--;
            పూర్ణాంకం opening = tok->parenstack[tok->level];
            ఒకవేళ (!tok->tok_extra_టోకెన్లు && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                ఒకవేళ (current != ఖాళీ && opening == '{') {
                    పూర్ణాంకం bracket_లోతు =
                        Py_Lexer_FTString_Bracket_లోతు(tok, current);
                    ఒకవేళ (bracket_లోతు == current->replacement_లోతు - 1) {
                        తిరిగి MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            Py_Lexer_String_ఉపసర్గ(current->రకం), c));
                    }
                }
                ఒకవేళ (tok->parenlinenostack[tok->level] != tok->lineno) {
                    తిరిగి MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                లేకపోతే {
                    తిరిగి MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        ఒకవేళ (current != ఖాళీ &&
                Py_Lexer_close_ftstring_సమాసం(tok, current, c) < 0) {
            తిరిగి MAKE_TOKEN(ERRORTOKEN);
        }
        ఆపు;
    అప్రమేయ:
        ఆపు;
    }

    ఒకవేళ (!Py_UNICODE_ISPRINTABLE(c)) {
        తిరిగి MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    ఒకవేళ (c == '=' && current != ఖాళీ) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_ప్రారంభం = tok->ప్రారంభం;
    p_చివర = tok->cur;
    తిరిగి MAKE_TOKEN(_PyToken_OneChar(c));
}


శూన్య
_PyTokenizer_Get(నిర్మాణం tok_state *tok, నిర్మాణం token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    పూర్ణాంకం ఫలితం;
    ఒకవేళ (current == ఖాళీ) {
        ఫలితం = Py_Lexer_get_సాధారణ(tok, ఖాళీ, token);
    }
    లేకపోతే {
        ఎంపిక (current->బహుళకం) {
        స్థితి FTSTRING_బహుళకం_వ్యక్తీకరణ:
            ఫలితం = Py_Lexer_get_సాధారణ(tok, current, token);
            ఆపు;
        స్థితి FTSTRING_బహుళకం_మధ్య:
        స్థితి FTSTRING_బహుళకం_FORMAT_SPEC:
            ఫలితం = _PyLexer_get_ftstring(tok, current, token);
            ఆపు;
        అప్రమేయ:
            Py_UNREACHABLE();
        }
    }
    ఒకవేళ (tok_failed(tok) && tok->పూర్తి != E_INCOMPLETE) {
        ఫలితం = ERRORTOKEN;
    }
    token->type = ఫలితం;
}
