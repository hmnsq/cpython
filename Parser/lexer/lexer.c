#شامل "Python.h"
#شامل "pycore_token.h"
#شامل "pycore_unicodeobject.h"
#شامل "errcode.h"

#شامل "lexer_internal.h"
#شامل "../tokenizer/helpers.h"
#شامل "../tokenizer/reader.h"

#تعریف_کرو MAKE_TOKEN(token_type) Py_Lexer_token_تیاری(tok, token, token_type, p_آغاز, p_انتہا)

/* Spaces in this constant are treated as "zero or more spaces or tabs" when
   tokenizing. */
ساکن ثابت حرف* type_comment_سابقہ = "# type: ";

صحیح
_PyLexer_refill(ساخت tok_state *tok)
{
    اگر (tok->مکمل != E_OK) {
        واپس 0;
    }
    صحیح rc = _PyTok_ReaderUnderflow(tok);
#اگر defined(Py_DEBUG)
    اگر (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, Py_Tok_ماخذ_اشارہ_گر(&tok->ماخذ, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->مکمل);
    }
#اگر_ختم
    اگر (!rc) {
        tok->cur = tok->inp;
        واپس 0;
    }
    tok->سطر_آغاز = tok->cur;
    Py_Tok_وسعت سطر = {tok->سطر_آغاز, tok->inp};
    اگر (Py_Tok_ماخذ_ڈھونڈو_Byte(&tok->ماخذ, سطر, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        واپس 0;
    }
    واپس 1;
}

/* Back-up one character */
خالی
_PyLexer_backup(ساخت tok_state *tok, صحیح c)
{
    اگر (c != EOF) {
        اگر (--tok->cur < tok->buf_انحراف) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        اگر (Py_Tok_ماخذ_Byte(&tok->ماخذ, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
ساکن صحیح
verify_identifier(ساخت tok_state *tok)
{
    اگر (tok->tok_extra_علامات) {
        واپس 1;
    }
    PyObject *s;
    اگر (tok_failed(tok))
        واپس 0;
    s = PyUnicode_DecodeUTF8(Py_Tok_ماخذ_اشارہ_گر(&tok->ماخذ, tok->آغاز),
                             tok->cur - tok->آغاز, تہی);
    اگر (s == تہی) {
        اگر (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->مکمل = E_DECODE;
        }
        ورنہ {
            tok->مکمل = E_ERROR;
        }
        واپس 0;
    }
    Py_ssize_t invalid = Py_Unicode_جائزہ_Identifier(s);
    أكد(invalid >= 0);
    أكد(Py_Unicode_GET_لمبائی(s) > 0);
    اگر (invalid < Py_Unicode_GET_لمبائی(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        اگر (invalid + 1 < Py_Unicode_GET_لمبائی(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            اگر (s != تہی) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            اگر (s == تہی) {
                tok->مکمل = E_ERROR;
                واپس 0;
            }
            error_cursor = tok->آغاز + Py_بائٹیں_GET_سائز(s);
        }
        Py_DECREF(s);
        اگر (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, Py_Tok_ماخذ_اشارہ_گر(&tok->ماخذ, tok->سطر_آغاز),
                error_cursor - tok->سطر_آغاز, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        ورنہ {
            _PyTokenizer_syntaxerror_at(
                tok, Py_Tok_ماخذ_اشارہ_گر(&tok->ماخذ, tok->سطر_آغاز),
                error_cursor - tok->سطر_آغاز, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        واپس 0;
    }
    Py_DECREF(s);
    واپس 1;
}

صحیح
Py_Lexer_get_معمول(ساخت tok_state *tok, ftstring_state *current, ساخت token *token)
{
    أكد(current == تہی ||
           (current->منوال == FTSTRING_منوال_عبارت &&
            current->replacement_گہرائی > 0));
    صحیح c;
    صحیح blankline, nonascii;

    _PyTok_Off p_آغاز = -1;
    _PyTok_Off p_انتہا = -1;
  nextline:
    tok->آغاز = -1;
    tok->آغاز_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    اگر (tok->layout.at_bol) {
        blankline = Py_Lexer_Begin_سطر(tok);
        اگر (blankline < 0) {
            واپس MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->آغاز = tok->cur;
    tok->آغاز_loc = (_PyTok_Loc){
        tok->lineno, tok->سطر_آغاز >= 0 ? Py_Lexer_Byte_عمود(tok) : -1};

    اگر (tok->layout.pending != 0) {
        واپس _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->آغاز = -1;
    /* Skip spaces */
    کرو {
        c = tok_nextc(tok);
    } جب (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->آغاز = tok->cur - 1;
    tok->آغاز_loc = (_PyTok_Loc){
        tok->lineno, tok->سطر_آغاز >= 0 ? Py_Lexer_Byte_عمود(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    اگر (c == '#') {

        جب (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        اگر (current != تہی) {
            _PyTok_Off comment_انتہا = tok->cur;
            اگر (c == '\n' || c == '\r') {
                comment_انتہا--;
            }
            اگر (Py_Lexer_ریکارڈ_ftstring_comment(
                    tok, current, tok->آغاز, comment_انتہا) < 0) {
                tok->مکمل = E_NOMEM;
                واپس MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->آغاز;
        اگر (tok->type_comments) {
            ثابت حرف *سابقہ = type_comment_سابقہ;
            جب (*سابقہ && comment < tok->cur) {
                اگر (*سابقہ == ' ') {
                    جب (comment < tok->cur) {
                        صحیح ch = Py_Tok_ماخذ_Byte(&tok->ماخذ, comment);
                        اگر (ch != ' ' && ch != '\t') {
                            توڑو;
                        }
                        comment++;
                    }
                } ورنہ اگر (*سابقہ == Py_Tok_ماخذ_Byte(&tok->ماخذ, comment)) {
                    comment++;
                } ورنہ {
                    توڑو;
                }

                سابقہ++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            اگر (!*سابقہ) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                صحیح is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(Py_Tok_ماخذ_اشارہ_گر(&tok->ماخذ, comment),
                           "ignore", 6) == 0;
                اگر (is_type_ignore && comment + 6 < tok->cur) {
                    صحیح ch = Py_Tok_ماخذ_Byte(&tok->ماخذ, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                صحیح type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_آغاز = comment + (is_type_ignore ? 6 : 0);
                p_انتہا = tok->cur;
                صحیح آغاز_ستون_انحراف = tok->آغاز_loc.byte_ستون +
                    (صحیح)(p_آغاز - tok->آغاز);
                /* Consume the newline after a standalone type ignore. */
                اگر (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                Py_Lexer_token_تیاری(tok, token, type, p_آغاز, p_انتہا);
                token->آغاز_loc = (_PyTok_Loc){tok->lineno, آغاز_ستون_انحراف};
                token->انتہا_loc = (_PyTok_Loc){tok->lineno,
                                              Py_Lexer_Byte_عمود(tok)};
                واپس type;
            }
        }
        اگر (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.گہرائی > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        اگر (tok->tok_extra_علامات) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_آغاز = comment;
            p_انتہا = tok->cur;
            tok->layout.comment_newline = blankline;
            واپس MAKE_TOKEN(COMMENT);
        }
    }

    اگر (tok->مکمل == E_INTERACT_STOP) {
        واپس MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    اگر (c == EOF) {
        اگر (tok->level) {
            واپس MAKE_TOKEN(ERRORTOKEN);
        }
        واپس MAKE_TOKEN(tok->مکمل == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    اگر (is_potential_identifier_آغاز(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        صحیح saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        جب (1) {
            اگر (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            ورنہ اگر (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            ورنہ اگر (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            ورنہ اگر (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            ورنہ اگر (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            ورنہ {
                توڑو;
            }
            c = tok_nextc(tok);
            اگر (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                صحیح حالت_2 = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                اگر (حالت_2 < 0) {
                    واپس MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                اگر (saw_f || saw_t) {
                    ftstring_قسم قسم = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    واپس Py_Lexer_جائزہ_fstring_آغاز(tok, token, c, قسم);
                }
                واپس Py_Lexer_جائزہ_string(tok, token, c);
            }
        }
        جب (is_potential_identifier_char(c)) {
            اگر (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        اگر (nonascii && !verify_identifier(tok)) {
            واپس MAKE_TOKEN(ERRORTOKEN);
        }

        p_آغاز = tok->آغاز;
        p_انتہا = tok->cur;

        واپس MAKE_TOKEN(NAME);
    }

    اگر (c == '\r') {
        c = tok_nextc(tok);
    }

    اگر (c == '\n') {
        صحیح type = _PyLexer_Newline(tok, token, blankline);
        اگر (type == 0) {
            جاؤ nextline;
        }
        واپس type;
    }

    /* Period or number starting with period? */
    اگر (c == '.') {
        c = tok_nextc(tok);
        اگر (Py_ISDIGIT(c)) {
            واپس Py_Lexer_جائزہ_نمبر(tok, token, c, 1);
        } ورنہ اگر (c == '.') {
            c = tok_nextc(tok);
            اگر (c == '.') {
                p_آغاز = tok->آغاز;
                p_انتہا = tok->cur;
                واپس MAKE_TOKEN(ELLIPSIS);
            }
            ورنہ {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        ورنہ {
            tok_backup(tok, c);
        }
        p_آغاز = tok->آغاز;
        p_انتہا = tok->cur;
        واپس MAKE_TOKEN(DOT);
    }


    /*نمبر*/
    اگر (Py_ISDIGIT(c)) {
        واپس Py_Lexer_جائزہ_نمبر(tok, token, c, 0);
    }

    /* String */
    اگر (c == '\'' || c == '"') {
        واپس Py_Lexer_جائزہ_string(tok, token, c);
    }

    /* Line continuation */
    اگر (c == '\\') {
        اگر ((c = Py_Lexer_Continue_سطر(tok)) == -1) {
            واپس MAKE_TOKEN(ERRORTOKEN);
        }
        جاؤ again; /* Read next line */
    }

    /* Punctuation character */
    صحیح is_punctuation = (c == ':' || c == '}' || c == '!');
    اگر (is_punctuation && current != تہی) {
        صحیح type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        اگر (type < 0) {
            واپس MAKE_TOKEN(ERRORTOKEN);
        }
        اگر (type != 0) {
            p_آغاز = tok->آغاز;
            p_انتہا = tok->cur;
            واپس MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        صحیح c2 = tok_nextc(tok);
        صحیح current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        اگر (current_token != OP) {
            صحیح c3 = tok_nextc(tok);
            صحیح current_token3 = _PyToken_ThreeChars(c, c2, c3);
            اگر (current_token3 != OP) {
                current_token = current_token3;
            }
            ورنہ {
                tok_backup(tok, c3);
            }
            p_آغاز = tok->آغاز;
            p_انتہا = tok->cur;
            واپس MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    انتخاب (c) {
    حالت '(':
    حالت '[':
    حالت '{':
        اگر (tok->level >= MAXLEVEL) {
            واپس MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (صحیح)(tok->آغاز - tok->سطر_آغاز);
        tok->level++;
        توڑو;
    حالت ')':
    حالت ']':
    حالت '}':
        اگر (current != تہی &&
                Py_Lexer_FTString_Bracket_گہرائی(tok, current) == 0) {
            اگر (c == '}') {
                واپس MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    Py_Lexer_String_سابقہ(current->قسم)));
            }
            واپس MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                Py_Lexer_String_سابقہ(current->قسم), c));
        }
        اگر (!tok->tok_extra_علامات && !tok->level) {
            واپس MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        اگر (tok->level > 0) {
            tok->level--;
            صحیح opening = tok->parenstack[tok->level];
            اگر (!tok->tok_extra_علامات && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                اگر (current != تہی && opening == '{') {
                    صحیح bracket_گہرائی =
                        Py_Lexer_FTString_Bracket_گہرائی(tok, current);
                    اگر (bracket_گہرائی == current->replacement_گہرائی - 1) {
                        واپس MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            Py_Lexer_String_سابقہ(current->قسم), c));
                    }
                }
                اگر (tok->parenlinenostack[tok->level] != tok->lineno) {
                    واپس MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                ورنہ {
                    واپس MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        اگر (current != تہی &&
                Py_Lexer_close_ftstring_اظہاریہ(tok, current, c) < 0) {
            واپس MAKE_TOKEN(ERRORTOKEN);
        }
        توڑو;
    طےشدہ:
        توڑو;
    }

    اگر (!Py_UNICODE_ISPRINTABLE(c)) {
        واپس MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    اگر (c == '=' && current != تہی) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_آغاز = tok->آغاز;
    p_انتہا = tok->cur;
    واپس MAKE_TOKEN(_PyToken_OneChar(c));
}


خالی
_PyTokenizer_Get(ساخت tok_state *tok, ساخت token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    صحیح نتیجہ;
    اگر (current == تہی) {
        نتیجہ = Py_Lexer_get_معمول(tok, تہی, token);
    }
    ورنہ {
        انتخاب (current->منوال) {
        حالت FTSTRING_منوال_عبارت:
            نتیجہ = Py_Lexer_get_معمول(tok, current, token);
            توڑو;
        حالت FTSTRING_منوال_درمیان:
        حالت FTSTRING_منوال_FORMAT_SPEC:
            نتیجہ = _PyLexer_get_ftstring(tok, current, token);
            توڑو;
        طےشدہ:
            Py_UNREACHABLE();
        }
    }
    اگر (tok_failed(tok) && tok->مکمل != E_INCOMPLETE) {
        نتیجہ = ERRORTOKEN;
    }
    token->type = نتیجہ;
}
