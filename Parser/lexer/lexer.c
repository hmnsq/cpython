#ضمن "Python.h"
#ضمن "pycore_token.h"
#ضمن "pycore_unicodeobject.h"
#ضمن "errcode.h"

#ضمن "lexer_internal.h"
#ضمن "../tokenizer/helpers.h"
#ضمن "../tokenizer/reader.h"

#تعریف_کرو MAKE_TOKEN(token_type) Py_Lexer_token_تیاری(tok, token, token_type, p_شروع, p_سرا)

/* Spaces in this constant are treated as "zero or more spaces or tabs" when
   tokenizing. */
ساكن ثابت حرف* type_comment_اگیتر = "# type: ";

صحيح
_PyLexer_refill(بنية tok_state *tok)
{
    إذا (tok->پورا != E_OK) {
        أرجع 0;
    }
    صحيح rc = _PyTok_ReaderUnderflow(tok);
#إذا defined(Py_DEBUG)
    إذا (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, Py_Tok_سروت_اشارہ_گر(&tok->سروت, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->پورا);
    }
#جے_انت
    إذا (!rc) {
        tok->cur = tok->inp;
        أرجع 0;
    }
    tok->سطر_شروع = tok->cur;
    Py_Tok_وسعت سطر = {tok->سطر_شروع, tok->inp};
    إذا (Py_Tok_سروت_لبھو_Byte(&tok->سروت, سطر, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        أرجع 0;
    }
    أرجع 1;
}

/* Back-up one character */
فراغ
_PyLexer_backup(بنية tok_state *tok, صحيح c)
{
    إذا (c != EOF) {
        إذا (--tok->cur < tok->buf_وتھ) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        إذا (Py_Tok_سروت_Byte(&tok->سروت, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
ساكن صحيح
verify_identifier(بنية tok_state *tok)
{
    إذا (tok->tok_extra_ٹوکن) {
        أرجع 1;
    }
    PyObject *s;
    إذا (tok_failed(tok))
        أرجع 0;
    s = PyUnicode_DecodeUTF8(Py_Tok_سروت_اشارہ_گر(&tok->سروت, tok->شروع),
                             tok->cur - tok->شروع, عدم);
    إذا (s == عدم) {
        إذا (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->پورا = E_DECODE;
        }
        وإلا {
            tok->پورا = E_ERROR;
        }
        أرجع 0;
    }
    Py_ssize_t invalid = Py_Unicode_جانچ_Identifier(s);
    أكد(invalid >= 0);
    أكد(Py_Unicode_GET_لمائی(s) > 0);
    إذا (invalid < Py_Unicode_GET_لمائی(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        إذا (invalid + 1 < Py_Unicode_GET_لمائی(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            إذا (s != عدم) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            إذا (s == عدم) {
                tok->پورا = E_ERROR;
                أرجع 0;
            }
            error_cursor = tok->شروع + Py_بائٹاں_GET_ناپ(s);
        }
        Py_DECREF(s);
        إذا (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, Py_Tok_سروت_اشارہ_گر(&tok->سروت, tok->سطر_شروع),
                error_cursor - tok->سطر_شروع, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        وإلا {
            _PyTokenizer_syntaxerror_at(
                tok, Py_Tok_سروت_اشارہ_گر(&tok->سروت, tok->سطر_شروع),
                error_cursor - tok->سطر_شروع, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        أرجع 0;
    }
    Py_DECREF(s);
    أرجع 1;
}

صحيح
Py_Lexer_get_عام(بنية tok_state *tok, ftstring_state *current, بنية token *token)
{
    أكد(current == عدم ||
           (current->منوال == FTSTRING_منوال_عبارت &&
            current->replacement_ڈونگھائی > 0));
    صحيح c;
    صحيح blankline, nonascii;

    _PyTok_Off p_شروع = -1;
    _PyTok_Off p_سرا = -1;
  nextline:
    tok->شروع = -1;
    tok->شروع_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    إذا (tok->layout.at_bol) {
        blankline = Py_Lexer_Begin_سطر(tok);
        إذا (blankline < 0) {
            أرجع MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->شروع = tok->cur;
    tok->شروع_loc = (_PyTok_Loc){
        tok->lineno, tok->سطر_شروع >= 0 ? Py_Lexer_Byte_کالم(tok) : -1};

    إذا (tok->layout.pending != 0) {
        أرجع _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->شروع = -1;
    /* Skip spaces */
    افعل {
        c = tok_nextc(tok);
    } طالما (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->شروع = tok->cur - 1;
    tok->شروع_loc = (_PyTok_Loc){
        tok->lineno, tok->سطر_شروع >= 0 ? Py_Lexer_Byte_کالم(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    إذا (c == '#') {

        طالما (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        إذا (current != عدم) {
            _PyTok_Off comment_سرا = tok->cur;
            إذا (c == '\n' || c == '\r') {
                comment_سرا--;
            }
            إذا (Py_Lexer_ریکارڈ_ftstring_comment(
                    tok, current, tok->شروع, comment_سرا) < 0) {
                tok->پورا = E_NOMEM;
                أرجع MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->شروع;
        إذا (tok->type_comments) {
            ثابت حرف *اگیتر = type_comment_اگیتر;
            طالما (*اگیتر && comment < tok->cur) {
                إذا (*اگیتر == ' ') {
                    طالما (comment < tok->cur) {
                        صحيح ch = Py_Tok_سروت_Byte(&tok->سروت, comment);
                        إذا (ch != ' ' && ch != '\t') {
                            اكسر;
                        }
                        comment++;
                    }
                } وإلا إذا (*اگیتر == Py_Tok_سروت_Byte(&tok->سروت, comment)) {
                    comment++;
                } وإلا {
                    اكسر;
                }

                اگیتر++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            إذا (!*اگیتر) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                صحيح is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(Py_Tok_سروت_اشارہ_گر(&tok->سروت, comment),
                           "ignore", 6) == 0;
                إذا (is_type_ignore && comment + 6 < tok->cur) {
                    صحيح ch = Py_Tok_سروت_Byte(&tok->سروت, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                صحيح type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_شروع = comment + (is_type_ignore ? 6 : 0);
                p_سرا = tok->cur;
                صحيح شروع_خانہ_وتھ = tok->شروع_loc.byte_خانہ +
                    (صحيح)(p_شروع - tok->شروع);
                /* Consume the newline after a standalone type ignore. */
                إذا (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                Py_Lexer_token_تیاری(tok, token, type, p_شروع, p_سرا);
                token->شروع_loc = (_PyTok_Loc){tok->lineno, شروع_خانہ_وتھ};
                token->سرا_loc = (_PyTok_Loc){tok->lineno,
                                              Py_Lexer_Byte_کالم(tok)};
                أرجع type;
            }
        }
        إذا (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.ڈونگھائی > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        إذا (tok->tok_extra_ٹوکن) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_شروع = comment;
            p_سرا = tok->cur;
            tok->layout.comment_newline = blankline;
            أرجع MAKE_TOKEN(COMMENT);
        }
    }

    إذا (tok->پورا == E_INTERACT_STOP) {
        أرجع MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    إذا (c == EOF) {
        إذا (tok->level) {
            أرجع MAKE_TOKEN(ERRORTOKEN);
        }
        أرجع MAKE_TOKEN(tok->پورا == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    إذا (is_potential_identifier_شروع(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        صحيح saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        طالما (1) {
            إذا (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            وإلا إذا (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            وإلا إذا (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            وإلا إذا (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            وإلا إذا (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            وإلا {
                اكسر;
            }
            c = tok_nextc(tok);
            إذا (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                صحيح حالت = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                إذا (حالت < 0) {
                    أرجع MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                إذا (saw_f || saw_t) {
                    ftstring_قسم قسم = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    أرجع Py_Lexer_جانچ_fstring_شروع(tok, token, c, قسم);
                }
                أرجع Py_Lexer_جانچ_string(tok, token, c);
            }
        }
        طالما (is_potential_identifier_char(c)) {
            إذا (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        إذا (nonascii && !verify_identifier(tok)) {
            أرجع MAKE_TOKEN(ERRORTOKEN);
        }

        p_شروع = tok->شروع;
        p_سرا = tok->cur;

        أرجع MAKE_TOKEN(NAME);
    }

    إذا (c == '\r') {
        c = tok_nextc(tok);
    }

    إذا (c == '\n') {
        صحيح type = _PyLexer_Newline(tok, token, blankline);
        إذا (type == 0) {
            اذهب nextline;
        }
        أرجع type;
    }

    /* Period or number starting with period? */
    إذا (c == '.') {
        c = tok_nextc(tok);
        إذا (Py_ISDIGIT(c)) {
            أرجع Py_Lexer_جانچ_نمبر(tok, token, c, 1);
        } وإلا إذا (c == '.') {
            c = tok_nextc(tok);
            إذا (c == '.') {
                p_شروع = tok->شروع;
                p_سرا = tok->cur;
                أرجع MAKE_TOKEN(ELLIPSIS);
            }
            وإلا {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        وإلا {
            tok_backup(tok, c);
        }
        p_شروع = tok->شروع;
        p_سرا = tok->cur;
        أرجع MAKE_TOKEN(DOT);
    }


    /*نمبر*/
    إذا (Py_ISDIGIT(c)) {
        أرجع Py_Lexer_جانچ_نمبر(tok, token, c, 0);
    }

    /* String */
    إذا (c == '\'' || c == '"') {
        أرجع Py_Lexer_جانچ_string(tok, token, c);
    }

    /* Line continuation */
    إذا (c == '\\') {
        إذا ((c = Py_Lexer_Continue_سطر(tok)) == -1) {
            أرجع MAKE_TOKEN(ERRORTOKEN);
        }
        اذهب again; /* Read next line */
    }

    /* Punctuation character */
    صحيح is_punctuation = (c == ':' || c == '}' || c == '!');
    إذا (is_punctuation && current != عدم) {
        صحيح type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        إذا (type < 0) {
            أرجع MAKE_TOKEN(ERRORTOKEN);
        }
        إذا (type != 0) {
            p_شروع = tok->شروع;
            p_سرا = tok->cur;
            أرجع MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        صحيح c2 = tok_nextc(tok);
        صحيح current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        إذا (current_token != OP) {
            صحيح c3 = tok_nextc(tok);
            صحيح current_token3 = _PyToken_ThreeChars(c, c2, c3);
            إذا (current_token3 != OP) {
                current_token = current_token3;
            }
            وإلا {
                tok_backup(tok, c3);
            }
            p_شروع = tok->شروع;
            p_سرا = tok->cur;
            أرجع MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    بدل (c) {
    حالة '(':
    حالة '[':
    حالة '{':
        إذا (tok->level >= MAXLEVEL) {
            أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (صحيح)(tok->شروع - tok->سطر_شروع);
        tok->level++;
        اكسر;
    حالة ')':
    حالة ']':
    حالة '}':
        إذا (current != عدم &&
                Py_Lexer_FTString_Bracket_ڈونگھائی(tok, current) == 0) {
            إذا (c == '}') {
                أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    Py_Lexer_String_اگیتر(current->قسم)));
            }
            أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                Py_Lexer_String_اگیتر(current->قسم), c));
        }
        إذا (!tok->tok_extra_ٹوکن && !tok->level) {
            أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        إذا (tok->level > 0) {
            tok->level--;
            صحيح opening = tok->parenstack[tok->level];
            إذا (!tok->tok_extra_ٹوکن && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                إذا (current != عدم && opening == '{') {
                    صحيح bracket_ڈونگھائی =
                        Py_Lexer_FTString_Bracket_ڈونگھائی(tok, current);
                    إذا (bracket_ڈونگھائی == current->replacement_ڈونگھائی - 1) {
                        أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            Py_Lexer_String_اگیتر(current->قسم), c));
                    }
                }
                إذا (tok->parenlinenostack[tok->level] != tok->lineno) {
                    أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                وإلا {
                    أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        إذا (current != عدم &&
                Py_Lexer_close_ftstring_اظہاریہ(tok, current, c) < 0) {
            أرجع MAKE_TOKEN(ERRORTOKEN);
        }
        اكسر;
    افتراضي:
        اكسر;
    }

    إذا (!Py_UNICODE_ISPRINTABLE(c)) {
        أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    إذا (c == '=' && current != عدم) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_شروع = tok->شروع;
    p_سرا = tok->cur;
    أرجع MAKE_TOKEN(_PyToken_OneChar(c));
}


فراغ
_PyTokenizer_Get(بنية tok_state *tok, بنية token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    صحيح نتیجہ;
    إذا (current == عدم) {
        نتیجہ = Py_Lexer_get_عام(tok, عدم, token);
    }
    وإلا {
        بدل (current->منوال) {
        حالة FTSTRING_منوال_عبارت:
            نتیجہ = Py_Lexer_get_عام(tok, current, token);
            اكسر;
        حالة FTSTRING_منوال_درمیان:
        حالة FTSTRING_منوال_FORMAT_SPEC:
            نتیجہ = _PyLexer_get_ftstring(tok, current, token);
            اكسر;
        افتراضي:
            Py_UNREACHABLE();
        }
    }
    إذا (tok_failed(tok) && tok->پورا != E_INCOMPLETE) {
        نتیجہ = ERRORTOKEN;
    }
    token->type = نتیجہ;
}
