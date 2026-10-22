#अन्तर्भाव "Python.h"
#अन्तर्भाव "pycore_token.h"
#अन्तर्भाव "errcode.h"

#अन्तर्भाव "lexer_internal.h"
#अन्तर्भाव "../tokenizer/helpers.h"

#परिभाषय MAKE_TOKEN(token_type) Py_Lexer_token_सज्जता(tok, token, token_type, p_start, p_अन्तः)

स्थिर पूर्णाङ्क
string_error_token(संरचना tok_state *tok, संरचना token *token,
                   _PyTok_Off start, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_col + 1},
        .text_विस्तृतिः = {start - location.byte_col, tok->inp},
    };
    पूर्णाङ्क type = Py_Lexer_token_सज्जता(tok, token, ERRORTOKEN, -1, -1);
    token->start_loc = location;
    token->अन्तः_loc = (_PyTok_Loc){location.lineno, -1};
    प्रत्यागम type;
}

स्थिर पूर्णाङ्क
incomplete_string_token(संरचना tok_state *tok, संरचना token *token,
                        पूर्णाङ्क type, _PyTok_Off start, _PyTok_Loc location)
{
    string_error_token(tok, token, start, location);
    tok->समाप्तम् = E_INCOMPLETE;
    प्रत्यागम type;
}

पूर्णाङ्क
Py_Lexer_लेखः_ftstring_comment(संरचना tok_state *tok, ftstring_state *state,
                                 _PyTok_Off start, _PyTok_Off अन्तः_2)
{
    प्रतिज्ञा(state == _PyLexer_CurrentFTString(tok) && state->बहुलकम् == FTSTRING_बहुलकम्_EXPRESSION);
    यदि (state->व्यञ्जकम्_विस्तृतिः.अन्तः_2 >= 0) {
        प्रत्यागम 0;
    }
    प्रतिज्ञा(state->व्यञ्जकम्_विस्तृतिः.start >= 0);
    tokenizer_comments *comments = state->comments;
    यदि (comments == रिक्त || comments->गणनम् == comments->क्षमता) {
        Py_ssize_t max_क्षमता = (PY_SSIZE_T_MAX -
            (Py_ssize_t)परिमाण(*comments)) /
            (Py_ssize_t)परिमाण(*comments->spans);
        यदि (comments != रिक्त && comments->क्षमता > max_क्षमता / 2) {
            Py_Err_No_स्मृतिः();
            प्रत्यागम -1;
        }
        Py_ssize_t क्षमता = comments == रिक्त ? 4 : comments->क्षमता * 2;
        आकार_t आकार = परिमाण(*comments) +
            (आकार_t)क्षमता * परिमाण(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, आकार);
        यदि (resized == रिक्त) {
            Py_Err_No_स्मृतिः();
            प्रत्यागम -1;
        }
        comments = resized;
        यदि (state->comments == रिक्त) {
            comments->गणनम् = 0;
        }
        comments->क्षमता = क्षमता;
        state->comments = comments;
    }
    comments->spans[comments->गणनम्++] =
        (Py_Tok_विस्तृतिः){start, अन्तः_2};
    प्रत्यागम 0;
}

स्थिर पूर्णाङ्क
finish_ftstring_व्यञ्जकम्(संरचना tok_state *tok, ftstring_state *state,
                     संरचना token *token)
{
    प्रतिज्ञा(token != रिक्त && state == _PyLexer_CurrentFTString(tok));
    प्रतिज्ञा(state->बहुलकम् == FTSTRING_बहुलकम्_EXPRESSION && tok->start >= 0);

    यदि (state->व्यञ्जकम्_विस्तृतिः.अन्तः_2 >= 0) {
        प्रत्यागम 0;
    }
    प्रतिज्ञा(state->व्यञ्जकम्_विस्तृतिः.start >= 0);
    state->व्यञ्जकम्_विस्तृतिः.अन्तः_2 = tok->start;
    पूर्णाङ्क tstring_interpolation = _PyLexer_IsTString(state->विधा) &&
        state->replacement_गभीरता == 1;
    यदि (!(state->debug_व्यञ्जकम् || tstring_interpolation) || token->metadata) {
        प्रत्यागम 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    यदि (comments != रिक्त && comments->गणनम् > 0) {
        Py_ssize_t stripped_आकार =
            state->व्यञ्जकम्_विस्तृतिः.अन्तः_2 - state->व्यञ्जकम्_विस्तृतिः.start;
        Py_ssize_t comment_गणनम् = 0;
        प्रति (Py_ssize_t i = 0; i < comments->गणनम्; i++) {
            Py_Tok_विस्तृतिः comment = comments->spans[i];
            प्रतिज्ञा(Py_Tok_विस्तृतिः_Is_Valid(comment));
            प्रतिज्ञा(comment.start >= (i == 0 ? state->व्यञ्जकम्_विस्तृतिः.start
                                           : comments->spans[i - 1].अन्तः_2));
            यदि (comment.start >= state->व्यञ्जकम्_विस्तृतिः.अन्तः_2) {
                भङ्ग;
            }
            प्रतिज्ञा(comment.अन्तः_2 <= state->व्यञ्जकम्_विस्तृतिः.अन्तः_2);
            stripped_आकार -= comment.अन्तः_2 - comment.start;
            comment_गणनम्++;
        }
        वर्ण *stripped = PyMem_Malloc((आकार_t)stripped_आकार);
        यदि (stripped == रिक्त) {
            Py_Err_No_स्मृतिः();
            प्रत्यागम -1;
        }
        Py_Tok_विस्तृतिः kept = {state->व्यञ्जकम्_विस्तृतिः.start, state->व्यञ्जकम्_विस्तृतिः.start};
        Py_ssize_t stripped_len = 0;
        प्रति (Py_ssize_t i = 0; i < comment_गणनम्; i++) {
            kept.अन्तः_2 = comments->spans[i].start;
            Py_ssize_t दीर्घता_2;
            ध्रुव वर्ण *text = Py_Tok_Source_विस्तृतिः_View(
                &tok->source, kept, &दीर्घता_2);
            स्मृतिं_प्रतिलिख(stripped + stripped_len, text, (आकार_t)दीर्घता_2);
            stripped_len += दीर्घता_2;
            kept.start = comments->spans[i].अन्तः_2;
        }
        kept.अन्तः_2 = state->व्यञ्जकम्_विस्तृतिः.अन्तः_2;
        Py_ssize_t दीर्घता_2;
        ध्रुव वर्ण *text = Py_Tok_Source_विस्तृतिः_View(&tok->source, kept, &दीर्घता_2);
        स्मृतिं_प्रतिलिख(stripped + stripped_len, text, (आकार_t)दीर्घता_2);
        res = PyUnicode_DecodeUTF8(stripped, stripped_आकार, रिक्त);
        PyMem_Free(stripped);
    }
    अन्यथा {
        Py_ssize_t व्यञ्जकम्_len;
        ध्रुव वर्ण *व्यञ्जकम् = Py_Tok_Source_विस्तृतिः_View(
            &tok->source, state->व्यञ्जकम्_विस्तृतिः, &व्यञ्जकम्_len);
        res = PyUnicode_DecodeUTF8(व्यञ्जकम्, व्यञ्जकम्_len, रिक्त);
    }

    यदि (!res) {
        प्रत्यागम -1;
    }
    token->metadata = res;
    प्रत्यागम 0;
}

स्थिर पूर्णाङ्क
begin_ftstring_व्यञ्जकम्(संरचना tok_state *tok, ftstring_state *state,
                    _PyTok_Off व्यञ्जकम्_start)
{
    प्रतिज्ञा(state->बहुलकम् != FTSTRING_बहुलकम्_EXPRESSION);
    state->व्यञ्जकम्_विस्तृतिः = (Py_Tok_विस्तृतिः){व्यञ्जकम्_start, -1};
    यदि (state->comments != रिक्त) {
        state->comments->गणनम् = 0;
    }
    यदि (state->replacement_गभीरता >= MAX_व्यञ्जकम्_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            Py_Lexer_String_उपसर्गः(state->विधा));
        प्रत्यागम -1;
    }
    state->replacement_गभीरता++;
    state->बहुलकम् = FTSTRING_बहुलकम्_EXPRESSION;
    state->debug_व्यञ्जकम् = 0;
    प्रत्यागम 0;
}

पूर्णाङ्क
_PyLexer_ftstring_punctuation(संरचना tok_state *tok, ftstring_state *state,
                              संरचना token *token, पूर्णाङ्क c)
{
    प्रतिज्ञा(state->बहुलकम् == FTSTRING_बहुलकम्_EXPRESSION);
    प्रतिज्ञा(c == ':' || c == '}' || c == '!');
    यदि (Py_Lexer_FTString_Bracket_गभीरता(tok, state) != state->replacement_गभीरता) {
        प्रत्यागम 0;
    }
    यदि (c == '!') {
        पूर्णाङ्क अग्र = tok_nextc(tok);
        tok_backup(tok, अग्र);
        यदि (अग्र == '=') {
            प्रत्यागम 0;
        }
    }
    यदि (finish_ftstring_व्यञ्जकम्(tok, state, token) < 0) {
        प्रत्यागम -1;
    }
    यदि (c == ':') {
        state->बहुलकम् = FTSTRING_बहुलकम्_FORMAT_SPEC;
        प्रत्यागम COLON;
    }
    प्रत्यागम 0;
}

पूर्णाङ्क
Py_Lexer_close_ftstring_व्यञ्जकम्(संरचना tok_state *tok, ftstring_state *state,
                             पूर्णाङ्क c)
{
    प्रतिज्ञा(state->बहुलकम् == FTSTRING_बहुलकम्_EXPRESSION);
    प्रतिज्ञा(c == ')' || c == ']' || c == '}');
    पूर्णाङ्क गभीरता = Py_Lexer_FTString_Bracket_गभीरता(tok, state);
    यदि (गभीरता < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                Py_Lexer_String_उपसर्गः(state->विधा), c);
        प्रत्यागम -1;
    }
    यदि (c == '}' && गभीरता == state->replacement_गभीरता - 1) {
        state->replacement_गभीरता--;
        state->बहुलकम् = FTSTRING_बहुलकम्_मध्यः;
        state->debug_व्यञ्जकम् = 0;
    }
    प्रत्यागम 0;
}

शून्य
_PyLexer_mark_ftstring_debug(संरचना tok_state *tok, ftstring_state *state)
{
    प्रतिज्ञा(state->बहुलकम् == FTSTRING_बहुलकम्_EXPRESSION);
    यदि (Py_Lexer_FTString_Bracket_गभीरता(tok, state) == state->replacement_गभीरता) {
        state->debug_व्यञ्जकम् = 1;
    }
}

पूर्णाङ्क
_PyLexer_check_string_prefixes(संरचना tok_state *tok,
                                             पूर्णाङ्क saw_b, पूर्णाङ्क saw_r, पूर्णाङ्क saw_u,
                                             पूर्णाङ्क saw_f, पूर्णाङ्क saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#परिभाषय RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    कुरु {                                                                  \
        (शून्य)Py_Tokenizer_syntaxerror_ज्ञातम्_range(                       \
            tok, (पूर्णाङ्क)(tok->start + 1 - tok->पङ्क्तिः_start),                 \
            (पूर्णाङ्क)(tok->cur - tok->पङ्क्तिः_start),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        प्रत्यागम -1;                                                        \
    } यावत् (0)

    यदि (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    यदि (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    यदि (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    यदि (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    यदि (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    यदि (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    यदि (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#अपरिभाषय RETURN_SYNTAX_ERROR

    प्रत्यागम 0;
}

पूर्णाङ्क
Py_Lexer_अवलोकनम्_fstring_start(संरचना tok_state *tok, संरचना token *token,
                            पूर्णाङ्क c, ftstring_विधा विधा)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_अन्तः = -1;

    पूर्णाङ्क quote = c;
    पूर्णाङ्क quote_आकार = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    पूर्णाङ्क after_quote = tok_nextc(tok);
    यदि (after_quote == quote) {
        पूर्णाङ्क after_after_quote = tok_nextc(tok);
        यदि (after_after_quote == quote) {
            quote_आकार = 3;
        }
        अन्यथा {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    यदि (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_start = tok->start;
    p_अन्तः = tok->cur;
    ftstring_state *state = Py_Lexer_प्रक्षिप_FTString(tok);
    यदि (state == रिक्त) {
        प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
    }
    state->बहुलकम् = FTSTRING_बहुलकम्_मध्यः;
    state->quote = quote;
    state->quote_आकार = quote_आकार;
    state->paren_level = tok->level;
    state->start = tok->start;
    state->start_loc = tok->start_loc;
    state->व्यञ्जकम्_विस्तृतिः = (Py_Tok_विस्तृतिः){-1, -1};

    state->विधा = विधा;
    प्रत्यागम _PyLexer_IsTString(विधा)
        ? MAKE_TOKEN(TSTRING_START) : MAKE_TOKEN(FSTRING_START);
}

पूर्णाङ्क
Py_Lexer_अवलोकनम्_string(संरचना tok_state *tok, संरचना token *token, पूर्णाङ्क c)
{
    _PyTok_Off p_start = -1;
    _PyTok_Off p_अन्तः = -1;

    पूर्णाङ्क quote = c;
    पूर्णाङ्क quote_आकार = 1;             /* 1 or 3 */
    पूर्णाङ्क अन्तः_quote_आकार = 0;
    पूर्णाङ्क has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    यदि (c == quote) {
        c = tok_nextc(tok);
        यदि (c == quote) {
            quote_आकार = 3;
        }
        अन्यथा {
            अन्तः_quote_आकार = 1;     /* empty string found */
        }
    }
    यदि (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    यावत् (अन्तः_quote_आकार != quote_आकार) {
        c = tok_nextc(tok);
        यदि (tok->समाप्तम् == E_ERROR) {
            प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
        }
        यदि (tok->समाप्तम् == E_DECODE) {
            भङ्ग;
        }
        यदि (c == EOF || (quote_आकार == 1 && c == '\n')) {
            यदि (tok_failed(tok)) {
                प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
            }
            यदि (c == EOF && tok->allow_incomplete_निवेशः) {
                प्रत्यागम incomplete_string_token(tok, token, STRING,
                                               tok->start, tok->start_loc);
            }
            पूर्णाङ्क अन्तः_lineno = tok->lineno;
            _PyTok_Loc location = tok->start_loc;
            ध्रुव वर्ण *पङ्क्तिः = _PyTok_SourcePointer(
                &tok->source, tok->start - location.byte_col);
            Py_ssize_t cursor_विस्थापनम् = (Py_ssize_t)location.byte_col + 1;

            ध्रुव ftstring_state *state = _PyLexer_CurrentFTString(tok);
            यदि (state != रिक्त) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                यदि (state->quote == quote && state->quote_आकार == quote_आकार) {
                    पूर्णाङ्क level = state->paren_level + state->replacement_गभीरता - 1;
                    प्रतिज्ञा(level >= 0 && level < tok->level);
                    प्रतिज्ञा(tok->parenstack[level] == '{');
                    पूर्णाङ्क lineno = tok->parenlinenostack[level];
                    यदि (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, पङ्क्तिः, cursor_विस्थापनम्, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            Py_Lexer_String_उपसर्गः(state->विधा), lineno);
                    }
                    अन्यथा {
                        _PyTokenizer_syntaxerror_at(
                            tok, पङ्क्तिः, cursor_विस्थापनम्, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            Py_Lexer_String_उपसर्गः(state->विधा));
                    }
                    प्रत्यागम string_error_token(tok, token, tok->start, location);
                }
            }

            यदि (quote_आकार == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, पङ्क्तिः, cursor_विस्थापनम्, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", अन्तः_lineno);
                यदि (c != '\n') {
                    tok->समाप्तम् = E_EOFS;
                }
                प्रत्यागम string_error_token(tok, token, tok->start, location);
            }
            अन्यथा {
                यदि (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, पङ्क्तिः, cursor_विस्थापनम्, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        अन्तः_lineno
                    );
                } अन्यथा {
                    _PyTokenizer_syntaxerror_at(
                        tok, पङ्क्तिः, cursor_विस्थापनम्, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", अन्तः_lineno
                    );
                }
                यदि (c != '\n') {
                    tok->समाप्तम् = E_EOLS;
                }
                प्रत्यागम string_error_token(tok, token, tok->start, location);
            }
        }
        यदि (c == quote) {
            अन्तः_quote_आकार += 1;
        }
        अन्यथा {
            अन्तः_quote_आकार = 0;
            यदि (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                यदि (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                यदि (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_start = tok->start;
    p_अन्तः = tok->cur;
    प्रत्यागम MAKE_TOKEN(STRING);
}

पूर्णाङ्क
_PyLexer_get_ftstring(संरचना tok_state *tok, ftstring_state *current, संरचना token *token)
{
    प्रतिज्ञा(current == _PyLexer_CurrentFTString(tok) && current->बहुलकम् != FTSTRING_बहुलकम्_EXPRESSION);
    प्रतिज्ञा((current->quote_आकार == 1 || current->quote_आकार == 3) &&
           current->replacement_गभीरता <= MAX_व्यञ्जकम्_NESTING);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_अन्तः = -1;
    पूर्णाङ्क token_type;
    पूर्णाङ्क अन्तः_quote_आकार = 0;
    पूर्णाङ्क unicode_escape = 0;
    पूर्णाङ्क quote = current->quote;
    पूर्णाङ्क quote_आकार = current->quote_आकार;
    पूर्णाङ्क अन्तर्_format_spec = current->बहुलकम् == FTSTRING_बहुलकम्_FORMAT_SPEC;
    पूर्णाङ्क raw = _PyLexer_IsRawString(current->विधा);

    tok->start = tok->cur;
    tok->start_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};

    यावत् (अन्तः_quote_आकार != quote_आकार) {
        पूर्णाङ्क c = tok_nextc(tok);
        यदि (tok->समाप्तम् == E_ERROR || tok->समाप्तम् == E_DECODE) {
            प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
        }

        यदि (c == EOF || (quote_आकार == 1 && c == '\n')) {
            यदि (tok_failed(tok)) {
                प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
            }
            यदि (c == EOF && tok->allow_incomplete_निवेशः) {
                प्रत्यागम incomplete_string_token(tok, token,
                    FTSTRING_मध्यः(current), current->start, current->start_loc);
            }

            यदि (अन्तर्_format_spec && c == '\n') {
                प्रत्यागम MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    Py_Lexer_String_उपसर्गः(current->विधा),
                    Py_Lexer_String_उपसर्गः(current->विधा)));
            }

            पूर्णाङ्क अन्तः_lineno = tok->lineno;
            _PyTok_Loc location = current->start_loc;
            ध्रुव वर्ण *पङ्क्तिः = _PyTok_SourcePointer(
                &tok->source, current->start - location.byte_col);
            Py_ssize_t cursor_विस्थापनम् = (Py_ssize_t)location.byte_col + 1;

            यदि (quote_आकार == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, पङ्क्तिः, cursor_विस्थापनम्, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    Py_Lexer_String_उपसर्गः(current->विधा), अन्तः_lineno);
                यदि (c != '\n') {
                    tok->समाप्तम् = E_EOFS;
                }
                प्रत्यागम string_error_token(tok, token,
                    current->start, location);
            }
            अन्यथा {
                _PyTokenizer_syntaxerror_at(
                    tok, पङ्क्तिः, cursor_विस्थापनम्, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    Py_Lexer_String_उपसर्गः(current->विधा), अन्तः_lineno);
                प्रत्यागम string_error_token(tok, token,
                    current->start, location);
            }
        }

        यदि (c == quote) {
            अन्तः_quote_आकार += 1;
            अनुवर्त;
        } अन्यथा {
            अन्तः_quote_आकार = 0;
        }

        यदि (c == '{') {
            पूर्णाङ्क peek = tok_nextc(tok);
            यदि (peek != '{' || अन्तर्_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off व्यञ्जकम्_start = tok->cur;
                tok_backup(tok, c);
                यदि (begin_ftstring_व्यञ्जकम्(tok, current, व्यञ्जकम्_start) < 0) {
                    प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
                }
                p_start = tok->start;
                p_अन्तः = tok->cur;
                यदि (p_start == p_अन्तः) {
                    प्रत्यागम Py_Lexer_get_प्रसामान्यम्(tok, current, token);
                }
            } अन्यथा {
                p_start = tok->start;
                p_अन्तः = tok->cur - 1;
            }
            गच्छ emit_मध्यः;
        } अन्यथा यदि (c == '}') {
            यदि (unicode_escape) {
                p_start = tok->start;
                p_अन्तः = tok->cur;
                गच्छ emit_मध्यः;
            }
            पूर्णाङ्क peek = tok_nextc(tok);

            पूर्णाङ्क bracket_गभीरता = Py_Lexer_FTString_Bracket_गभीरता(tok, current);
            यदि (peek == '}' && !अन्तर्_format_spec && bracket_गभीरता == 0) {
                p_start = tok->start;
                p_अन्तः = tok->cur - 1;
            }
            अन्यथा {
                tok_backup(tok, peek);
                यदि (!अन्तर्_format_spec && bracket_गभीरता == 0) {
                    यदि (tok->start == tok->cur - 1) {
                        प्रत्यागम MAKE_TOKEN(_PyTokenizer_syntaxerror(
                            tok, "%c-string: single '}' is not allowed",
                            Py_Lexer_String_उपसर्गः(current->विधा)));
                    }
                    tok_backup(tok, c);
                    p_start = tok->start;
                    p_अन्तः = tok->cur;
                    गच्छ emit_मध्यः;
                }
                tok_backup(tok, c);
                current->बहुलकम् = FTSTRING_बहुलकम्_EXPRESSION;
                p_start = tok->start;
                p_अन्तः = tok->cur;
            }
            गच्छ emit_मध्यः;
        } अन्यथा यदि (c == '\\') {
            पूर्णाङ्क peek = tok_nextc(tok);
            यदि (peek == '\r') {
                peek = tok_nextc(tok);
            }
            यदि (peek == '{' || peek == '}') {
                यदि (!raw) {
                    यदि (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        प्रत्यागम MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                अनुवर्त;
            }

            यदि (!raw) {
                यदि (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    यदि (peek == '{') {
                        unicode_escape = 1;
                    } अन्यथा {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_start = tok->start;
    p_अन्तः = tok->cur;
    यदि (p_अन्तः - quote_आकार == p_start) {
        पूर्णाङ्क अन्तः_token = FTSTRING_अन्तः(current);
        Py_Lexer_निष्कासय_FTString(tok);
        प्रत्यागम MAKE_TOKEN(अन्तः_token);
    }
    प्रति (पूर्णाङ्क i = 0; i < quote_आकार; i++) {
        tok_backup(tok, quote);
    }
    p_अन्तः = tok->cur;
emit_मध्यः:
    token_type = MAKE_TOKEN(FTSTRING_मध्यः(current));
    token->is_raw = raw;
    प्रत्यागम token_type;
}
