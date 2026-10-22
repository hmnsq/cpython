#अन्तर्भाव "Python.h"
#अन्तर्भाव "pycore_token.h"
#अन्तर्भाव "pycore_unicodeobject.h"
#अन्तर्भाव "errcode.h"

#अन्तर्भाव "lexer_internal.h"
#अन्तर्भाव "../tokenizer/helpers.h"
#अन्तर्भाव "../tokenizer/reader.h"

#परिभाषय MAKE_TOKEN(token_type) Py_Lexer_token_सज्जता(tok, token, token_type, p_start, p_अन्तः)

/* Spaces in this constant are treated as "zero or more spaces or tabs" when
   tokenizing. */
स्थिर ध्रुव वर्ण* type_comment_उपसर्गः = "# type: ";

पूर्णाङ्क
_PyLexer_refill(संरचना tok_state *tok)
{
    यदि (tok->समाप्तम् != E_OK) {
        प्रत्यागम 0;
    }
    पूर्णाङ्क rc = _PyTok_ReaderUnderflow(tok);
#यदि defined(Py_DEBUG)
    यदि (tok->debug) {
        fprintf(stderr, "line[%d] = ", tok->lineno);
        _PyTokenizer_print_escape(
            stderr, _PyTok_SourcePointer(&tok->source, tok->cur),
            tok->inp - tok->cur);
        fprintf(stderr, "  tok->done = %d\n", tok->समाप्तम्);
    }
#यदि_अन्तः
    यदि (!rc) {
        tok->cur = tok->inp;
        प्रत्यागम 0;
    }
    tok->पङ्क्तिः_start = tok->cur;
    Py_Tok_विस्तृतिः पङ्क्तिः = {tok->पङ्क्तिः_start, tok->inp};
    यदि (_PyTok_SourceFindByte(&tok->source, पङ्क्तिः, '\0') >= 0) {
        _PyTokenizer_syntaxerror(tok, "source code cannot contain null bytes");
        tok->cur = tok->inp;
        प्रत्यागम 0;
    }
    प्रत्यागम 1;
}

/* Back-up one character */
शून्य
_PyLexer_backup(संरचना tok_state *tok, पूर्णाङ्क c)
{
    यदि (c != EOF) {
        यदि (--tok->cur < tok->buf_विस्थापनम्) {
            Py_FatalError("tokenizer beginning of buffer");
        }
        यदि (_PyTok_SourceByte(&tok->source, tok->cur) != Py_CHARMASK(c)) {
            Py_FatalError("tok_backup: wrong character");
        }
    }
}



/* Verify that the identifier follows PEP 3131. */
स्थिर पूर्णाङ्क
verify_identifier(संरचना tok_state *tok)
{
    यदि (tok->tok_extra_tokens) {
        प्रत्यागम 1;
    }
    PyObject *s;
    यदि (tok_failed(tok))
        प्रत्यागम 0;
    s = PyUnicode_DecodeUTF8(_PyTok_SourcePointer(&tok->source, tok->start),
                             tok->cur - tok->start, रिक्त);
    यदि (s == रिक्त) {
        यदि (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            tok->समाप्तम् = E_DECODE;
        }
        अन्यथा {
            tok->समाप्तम् = E_ERROR;
        }
        प्रत्यागम 0;
    }
    Py_ssize_t invalid = Py_Unicode_अवलोकनम्_Identifier(s);
    प्रतिज्ञा(invalid >= 0);
    प्रतिज्ञा(Py_Unicode_GET_दीर्घता(s) > 0);
    यदि (invalid < Py_Unicode_GET_दीर्घता(s)) {
        Py_UCS4 ch = PyUnicode_READ_CHAR(s, invalid);
        _PyTok_Off error_cursor = tok->cur;
        यदि (invalid + 1 < Py_Unicode_GET_दीर्घता(s)) {
            /* Determine the offset in UTF-8 encoded input */
            Py_SETREF(s, PyUnicode_Substring(s, 0, invalid + 1));
            यदि (s != रिक्त) {
                Py_SETREF(s, PyUnicode_AsUTF8String(s));
            }
            यदि (s == रिक्त) {
                tok->समाप्तम् = E_ERROR;
                प्रत्यागम 0;
            }
            error_cursor = tok->start + Py_अष्टकानि_GET_आकार(s);
        }
        Py_DECREF(s);
        यदि (Py_UNICODE_ISPRINTABLE(ch)) {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->पङ्क्तिः_start),
                error_cursor - tok->पङ्क्तिः_start, tok->lineno, -1, -1,
                "invalid character '%c' (U+%04X)", ch, ch);
        }
        अन्यथा {
            _PyTokenizer_syntaxerror_at(
                tok, _PyTok_SourcePointer(&tok->source, tok->पङ्क्तिः_start),
                error_cursor - tok->पङ्क्तिः_start, tok->lineno, -1, -1,
                "invalid non-printable character U+%04X", ch);
        }
        प्रत्यागम 0;
    }
    Py_DECREF(s);
    प्रत्यागम 1;
}

पूर्णाङ्क
Py_Lexer_get_प्रसामान्यम्(संरचना tok_state *tok, ftstring_state *current, संरचना token *token)
{
    प्रतिज्ञा(current == रिक्त ||
           (current->बहुलकम् == FTSTRING_बहुलकम्_EXPRESSION &&
            current->replacement_गभीरता > 0));
    पूर्णाङ्क c;
    पूर्णाङ्क blankline, nonascii;

    _PyTok_Off p_start = -1;
    _PyTok_Off p_अन्तः = -1;
  nextline:
    tok->start = -1;
    tok->start_loc = (_PyTok_Loc){tok->lineno, -1};
    blankline = 0;


    यदि (tok->layout.at_bol) {
        blankline = Py_Lexer_Begin_पङ्क्तिः(tok);
        यदि (blankline < 0) {
            प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
        }
    }

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->पङ्क्तिः_start >= 0 ? _PyLexer_ByteColumn(tok) : -1};

    यदि (tok->layout.pending != 0) {
        प्रत्यागम _PyLexer_IndentationToken(tok, token);
    }

    /* Peek ahead at the next character */
    c = tok_nextc(tok);
    tok_backup(tok, c);

 again:
    tok->start = -1;
    /* Skip spaces */
    कुरु {
        c = tok_nextc(tok);
    } यावत् (c == ' ' || c == '\t' || c == '\014');

    /* Set start of current token */
    tok->start = tok->cur - 1;
    tok->start_loc = (_PyTok_Loc){
        tok->lineno, tok->पङ्क्तिः_start >= 0 ? _PyLexer_ByteColumn(tok) - 1 : -1};

    /* Skip comment, unless it's a type comment */
    यदि (c == '#') {

        यावत् (c != EOF && c != '\n' && c != '\r') {
            c = tok_nextc(tok);
        }

        यदि (current != रिक्त) {
            _PyTok_Off comment_अन्तः = tok->cur;
            यदि (c == '\n' || c == '\r') {
                comment_अन्तः--;
            }
            यदि (Py_Lexer_लेखः_ftstring_comment(
                    tok, current, tok->start, comment_अन्तः) < 0) {
                tok->समाप्तम् = E_NOMEM;
                प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
            }
        }

        _PyTok_Off comment = tok->start;
        यदि (tok->type_comments) {
            ध्रुव वर्ण *उपसर्गः = type_comment_उपसर्गः;
            यावत् (*उपसर्गः && comment < tok->cur) {
                यदि (*उपसर्गः == ' ') {
                    यावत् (comment < tok->cur) {
                        पूर्णाङ्क ch = _PyTok_SourceByte(&tok->source, comment);
                        यदि (ch != ' ' && ch != '\t') {
                            भङ्ग;
                        }
                        comment++;
                    }
                } अन्यथा यदि (*उपसर्गः == _PyTok_SourceByte(&tok->source, comment)) {
                    comment++;
                } अन्यथा {
                    भङ्ग;
                }

                उपसर्गः++;
            }

            /* This is a type comment if we matched all of type_comment_prefix. */
            यदि (!*उपसर्गः) {
                tok_backup(tok, c);  /* don't eat the newline or EOF */
                /* A TYPE_IGNORE is "type: ignore" followed by the end of the token
                 * or anything ASCII and non-alphanumeric. */
                पूर्णाङ्क is_type_ignore = tok->cur - comment >= 6 &&
                    memcmp(_PyTok_SourcePointer(&tok->source, comment),
                           "ignore", 6) == 0;
                यदि (is_type_ignore && comment + 6 < tok->cur) {
                    पूर्णाङ्क ch = _PyTok_SourceByte(&tok->source, comment + 6);
                    is_type_ignore = ch < 128 && !Py_ISALNUM(ch);
                }

                पूर्णाङ्क type = is_type_ignore ? TYPE_IGNORE : TYPE_COMMENT;
                p_start = comment + (is_type_ignore ? 6 : 0);
                p_अन्तः = tok->cur;
                पूर्णाङ्क start_col_विस्थापनम् = tok->start_loc.byte_col +
                    (पूर्णाङ्क)(p_start - tok->start);
                /* Consume the newline after a standalone type ignore. */
                यदि (is_type_ignore && blankline) {
                    tok_nextc(tok);
                    tok->layout.at_bol = 1;
                }
                Py_Lexer_token_सज्जता(tok, token, type, p_start, p_अन्तः);
                token->start_loc = (_PyTok_Loc){tok->lineno, start_col_विस्थापनम्};
                token->अन्तः_loc = (_PyTok_Loc){tok->lineno,
                                              _PyLexer_ByteColumn(tok)};
                प्रत्यागम type;
            }
        }
        यदि (tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
            tok->layout.गभीरता > 0) {
            // Ending this comment lets the next line dedent the suite.
            tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
        }
        यदि (tok->tok_extra_tokens) {
            tok_backup(tok, c);  /* don't eat the newline or EOF */
            p_start = comment;
            p_अन्तः = tok->cur;
            tok->layout.comment_newline = blankline;
            प्रत्यागम MAKE_TOKEN(COMMENT);
        }
    }

    यदि (tok->समाप्तम् == E_INTERACT_STOP) {
        प्रत्यागम MAKE_TOKEN(ENDMARKER);
    }

    /* Check for EOF and errors now */
    यदि (c == EOF) {
        यदि (tok->level) {
            प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
        }
        प्रत्यागम MAKE_TOKEN(tok->समाप्तम् == E_EOF ? ENDMARKER : ERRORTOKEN);
    }

    /* Identifier (most frequent token!) */
    nonascii = 0;
    यदि (is_potential_identifier_start(c)) {
        /* Process the various legal combinations of b"", r"", u"", and f"". */
        पूर्णाङ्क saw_b = 0, saw_r = 0, saw_u = 0, saw_f = 0, saw_t = 0;
        यावत् (1) {
            यदि (!saw_b && (c == 'b' || c == 'B')) {
                saw_b = 1;
            }
            /* Since this is a backwards compatibility support literal we don't
               want to support it in arbitrary order like byte literals. */
            अन्यथा यदि (!saw_u && (c == 'u'|| c == 'U')) {
                saw_u = 1;
            }
            /* ur"" and ru"" are not supported */
            अन्यथा यदि (!saw_r && (c == 'r' || c == 'R')) {
                saw_r = 1;
            }
            अन्यथा यदि (!saw_f && (c == 'f' || c == 'F')) {
                saw_f = 1;
            }
            अन्यथा यदि (!saw_t && (c == 't' || c == 'T')) {
                saw_t = 1;
            }
            अन्यथा {
                भङ्ग;
            }
            c = tok_nextc(tok);
            यदि (c == '"' || c == '\'') {
                // Raise error on incompatible string prefixes:
                पूर्णाङ्क स्थितिः = _PyLexer_check_string_prefixes(
                    tok, saw_b, saw_r, saw_u, saw_f, saw_t);
                यदि (स्थितिः < 0) {
                    प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
                }

                // Handle valid f or t string creation:
                यदि (saw_f || saw_t) {
                    ftstring_विधा विधा = saw_t
                        ? (saw_r ? RAW_TSTRING : TSTRING)
                        : (saw_r ? RAW_FSTRING : FSTRING);
                    प्रत्यागम Py_Lexer_अवलोकनम्_fstring_start(tok, token, c, विधा);
                }
                प्रत्यागम Py_Lexer_अवलोकनम्_string(tok, token, c);
            }
        }
        यावत् (is_potential_identifier_char(c)) {
            यदि (c >= 128) {
                nonascii = 1;
            }
            c = tok_nextc(tok);
        }
        tok_backup(tok, c);
        यदि (nonascii && !verify_identifier(tok)) {
            प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
        }

        p_start = tok->start;
        p_अन्तः = tok->cur;

        प्रत्यागम MAKE_TOKEN(NAME);
    }

    यदि (c == '\r') {
        c = tok_nextc(tok);
    }

    यदि (c == '\n') {
        पूर्णाङ्क type = _PyLexer_Newline(tok, token, blankline);
        यदि (type == 0) {
            गच्छ nextline;
        }
        प्रत्यागम type;
    }

    /* Period or number starting with period? */
    यदि (c == '.') {
        c = tok_nextc(tok);
        यदि (Py_ISDIGIT(c)) {
            प्रत्यागम Py_Lexer_अवलोकनम्_सङ्ख्या(tok, token, c, 1);
        } अन्यथा यदि (c == '.') {
            c = tok_nextc(tok);
            यदि (c == '.') {
                p_start = tok->start;
                p_अन्तः = tok->cur;
                प्रत्यागम MAKE_TOKEN(ELLIPSIS);
            }
            अन्यथा {
                tok_backup(tok, c);
            }
            tok_backup(tok, '.');
        }
        अन्यथा {
            tok_backup(tok, c);
        }
        p_start = tok->start;
        p_अन्तः = tok->cur;
        प्रत्यागम MAKE_TOKEN(DOT);
    }


    /*सङ्ख्या*/
    यदि (Py_ISDIGIT(c)) {
        प्रत्यागम Py_Lexer_अवलोकनम्_सङ्ख्या(tok, token, c, 0);
    }

    /* String */
    यदि (c == '\'' || c == '"') {
        प्रत्यागम Py_Lexer_अवलोकनम्_string(tok, token, c);
    }

    /* Line continuation */
    यदि (c == '\\') {
        यदि ((c = Py_Lexer_Continue_पङ्क्तिः(tok)) == -1) {
            प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
        }
        गच्छ again; /* Read next line */
    }

    /* Punctuation character */
    पूर्णाङ्क is_punctuation = (c == ':' || c == '}' || c == '!');
    यदि (is_punctuation && current != रिक्त) {
        पूर्णाङ्क type = _PyLexer_ftstring_punctuation(tok, current, token, c);
        यदि (type < 0) {
            प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
        }
        यदि (type != 0) {
            p_start = tok->start;
            p_अन्तः = tok->cur;
            प्रत्यागम MAKE_TOKEN(type);
        }
    }

    /* Check for two-character token */
    {
        पूर्णाङ्क c2 = tok_nextc(tok);
        पूर्णाङ्क current_token = _PyToken_TwoChars(c, c2, tok->barry_as_bdfl);
        यदि (current_token != OP) {
            पूर्णाङ्क c3 = tok_nextc(tok);
            पूर्णाङ्क current_token3 = _PyToken_ThreeChars(c, c2, c3);
            यदि (current_token3 != OP) {
                current_token = current_token3;
            }
            अन्यथा {
                tok_backup(tok, c3);
            }
            p_start = tok->start;
            p_अन्तः = tok->cur;
            प्रत्यागम MAKE_TOKEN(current_token);
        }
        tok_backup(tok, c2);
    }

    /* Keep track of parentheses nesting level */
    विकल्प (c) {
    पक्ष '(':
    पक्ष '[':
    पक्ष '{':
        यदि (tok->level >= MAXLEVEL) {
            प्रत्यागम MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "too many nested parentheses"));
        }
        tok->parenstack[tok->level] = c;
        tok->parenlinenostack[tok->level] = tok->lineno;
        tok->parencolstack[tok->level] = (पूर्णाङ्क)(tok->start - tok->पङ्क्तिः_start);
        tok->level++;
        भङ्ग;
    पक्ष ')':
    पक्ष ']':
    पक्ष '}':
        यदि (current != रिक्त &&
                Py_Lexer_FTString_Bracket_गभीरता(tok, current) == 0) {
            यदि (c == '}') {
                प्रत्यागम MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                    "%c-string: single '}' is not allowed",
                    Py_Lexer_String_उपसर्गः(current->विधा)));
            }
            प्रत्यागम MAKE_TOKEN(_PyTokenizer_syntaxerror(
                tok, "%c-string: unmatched '%c'",
                Py_Lexer_String_उपसर्गः(current->विधा), c));
        }
        यदि (!tok->tok_extra_tokens && !tok->level) {
            प्रत्यागम MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "unmatched '%c'", c));
        }
        यदि (tok->level > 0) {
            tok->level--;
            पूर्णाङ्क opening = tok->parenstack[tok->level];
            यदि (!tok->tok_extra_tokens && !((opening == '(' && c == ')') ||
                                            (opening == '[' && c == ']') ||
                                            (opening == '{' && c == '}'))) {
                /* Do not match a closer against the brace that opened the
                 * current replacement field. */
                यदि (current != रिक्त && opening == '{') {
                    पूर्णाङ्क bracket_गभीरता =
                        Py_Lexer_FTString_Bracket_गभीरता(tok, current);
                    यदि (bracket_गभीरता == current->replacement_गभीरता - 1) {
                        प्रत्यागम MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "%c-string: unmatched '%c'",
                            Py_Lexer_String_उपसर्गः(current->विधा), c));
                    }
                }
                यदि (tok->parenlinenostack[tok->level] != tok->lineno) {
                    प्रत्यागम MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c' on line %d",
                            c, opening, tok->parenlinenostack[tok->level]));
                }
                अन्यथा {
                    प्रत्यागम MAKE_TOKEN(_PyTokenizer_syntaxerror(tok,
                            "closing parenthesis '%c' does not match "
                            "opening parenthesis '%c'",
                            c, opening));
                }
            }
        }
        यदि (current != रिक्त &&
                Py_Lexer_close_ftstring_व्यञ्जकम्(tok, current, c) < 0) {
            प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
        }
        भङ्ग;
    स्वभाव:
        भङ्ग;
    }

    यदि (!Py_UNICODE_ISPRINTABLE(c)) {
        प्रत्यागम MAKE_TOKEN(_PyTokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", c));
    }

    यदि (c == '=' && current != रिक्त) {
        _PyLexer_mark_ftstring_debug(tok, current);
    }

    /* Punctuation character */
    p_start = tok->start;
    p_अन्तः = tok->cur;
    प्रत्यागम MAKE_TOKEN(_PyToken_OneChar(c));
}


शून्य
_PyTokenizer_Get(संरचना tok_state *tok, संरचना token *token)
{
    _PyToken_Free(token);
    ftstring_state *current = _PyLexer_CurrentFTString(tok);
    पूर्णाङ्क परिणाम;
    यदि (current == रिक्त) {
        परिणाम = Py_Lexer_get_प्रसामान्यम्(tok, रिक्त, token);
    }
    अन्यथा {
        विकल्प (current->बहुलकम्) {
        पक्ष FTSTRING_बहुलकम्_EXPRESSION:
            परिणाम = Py_Lexer_get_प्रसामान्यम्(tok, current, token);
            भङ्ग;
        पक्ष FTSTRING_बहुलकम्_मध्यः:
        पक्ष FTSTRING_बहुलकम्_FORMAT_SPEC:
            परिणाम = _PyLexer_get_ftstring(tok, current, token);
            भङ्ग;
        स्वभाव:
            Py_UNREACHABLE();
        }
    }
    यदि (tok_failed(tok) && tok->समाप्तम् != E_INCOMPLETE) {
        परिणाम = ERRORTOKEN;
    }
    token->type = परिणाम;
}
