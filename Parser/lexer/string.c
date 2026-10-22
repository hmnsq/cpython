#شامل "Python.h"
#شامل "pycore_token.h"
#شامل "errcode.h"

#شامل "lexer_internal.h"
#شامل "../tokenizer/helpers.h"

#تعریف_کرو MAKE_TOKEN(token_type) Py_Lexer_token_تیاری(tok, token, token_type, p_آغاز, p_انتہا)

ساکن صحیح
string_error_token(ساخت tok_state *tok, ساخت token *token,
                   _PyTok_Off آغاز, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_ستون + 1},
        .متن_وسعت = {آغاز - location.byte_ستون, tok->inp},
    };
    صحیح type = Py_Lexer_token_تیاری(tok, token, ERRORTOKEN, -1, -1);
    token->آغاز_loc = location;
    token->انتہا_loc = (_PyTok_Loc){location.lineno, -1};
    واپس type;
}

ساکن صحیح
incomplete_string_token(ساخت tok_state *tok, ساخت token *token,
                        صحیح type, _PyTok_Off آغاز, _PyTok_Loc location)
{
    string_error_token(tok, token, آغاز, location);
    tok->مکمل = E_INCOMPLETE;
    واپس type;
}

صحیح
Py_Lexer_ریکارڈ_ftstring_comment(ساخت tok_state *tok, ftstring_state *state,
                                 _PyTok_Off آغاز, _PyTok_Off انتہا)
{
    أكد(state == _PyLexer_CurrentFTString(tok) && state->منوال == FTSTRING_منوال_عبارت);
    اگر (state->اظہاریہ_وسعت.انتہا >= 0) {
        واپس 0;
    }
    أكد(state->اظہاریہ_وسعت.آغاز >= 0);
    tokenizer_comments *comments = state->comments;
    اگر (comments == تہی || comments->گنتی == comments->استعداد) {
        Py_ssize_t max_استعداد = (PY_SSIZE_T_MAX -
            (Py_ssize_t)حجم(*comments)) /
            (Py_ssize_t)حجم(*comments->spans);
        اگر (comments != تہی && comments->استعداد > max_استعداد / 2) {
            Py_Err_No_حافظہ();
            واپس -1;
        }
        Py_ssize_t استعداد = comments == تہی ? 4 : comments->استعداد * 2;
        سائز_t سائز = حجم(*comments) +
            (سائز_t)استعداد * حجم(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, سائز);
        اگر (resized == تہی) {
            Py_Err_No_حافظہ();
            واپس -1;
        }
        comments = resized;
        اگر (state->comments == تہی) {
            comments->گنتی = 0;
        }
        comments->استعداد = استعداد;
        state->comments = comments;
    }
    comments->spans[comments->گنتی++] =
        (Py_Tok_وسعت){آغاز, انتہا};
    واپس 0;
}

ساکن صحیح
اختتام_ftstring_اظہاریہ(ساخت tok_state *tok, ftstring_state *state,
                     ساخت token *token)
{
    أكد(token != تہی && state == _PyLexer_CurrentFTString(tok));
    أكد(state->منوال == FTSTRING_منوال_عبارت && tok->آغاز >= 0);

    اگر (state->اظہاریہ_وسعت.انتہا >= 0) {
        واپس 0;
    }
    أكد(state->اظہاریہ_وسعت.آغاز >= 0);
    state->اظہاریہ_وسعت.انتہا = tok->آغاز;
    صحیح tstring_interpolation = _PyLexer_IsTString(state->قسم) &&
        state->replacement_گہرائی == 1;
    اگر (!(state->debug_اظہاریہ || tstring_interpolation) || token->metadata) {
        واپس 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    اگر (comments != تہی && comments->گنتی > 0) {
        Py_ssize_t stripped_سائز =
            state->اظہاریہ_وسعت.انتہا - state->اظہاریہ_وسعت.آغاز;
        Py_ssize_t comment_گنتی = 0;
        برائے (Py_ssize_t i = 0; i < comments->گنتی; i++) {
            Py_Tok_وسعت comment = comments->spans[i];
            أكد(Py_Tok_وسعت_Is_Valid(comment));
            أكد(comment.آغاز >= (i == 0 ? state->اظہاریہ_وسعت.آغاز
                                           : comments->spans[i - 1].انتہا));
            اگر (comment.آغاز >= state->اظہاریہ_وسعت.انتہا) {
                توڑو;
            }
            أكد(comment.انتہا <= state->اظہاریہ_وسعت.انتہا);
            stripped_سائز -= comment.انتہا - comment.آغاز;
            comment_گنتی++;
        }
        حرف *stripped = PyMem_Malloc((سائز_t)stripped_سائز);
        اگر (stripped == تہی) {
            Py_Err_No_حافظہ();
            واپس -1;
        }
        Py_Tok_وسعت kept = {state->اظہاریہ_وسعت.آغاز, state->اظہاریہ_وسعت.آغاز};
        Py_ssize_t stripped_len = 0;
        برائے (Py_ssize_t i = 0; i < comment_گنتی; i++) {
            kept.انتہا = comments->spans[i].آغاز;
            Py_ssize_t لمبائی_2;
            ثابت حرف *متن = Py_Tok_ماخذ_وسعت_View(
                &tok->ماخذ, kept, &لمبائی_2);
            حافظہ_نقل(stripped + stripped_len, متن, (سائز_t)لمبائی_2);
            stripped_len += لمبائی_2;
            kept.آغاز = comments->spans[i].انتہا;
        }
        kept.انتہا = state->اظہاریہ_وسعت.انتہا;
        Py_ssize_t لمبائی_2;
        ثابت حرف *متن = Py_Tok_ماخذ_وسعت_View(&tok->ماخذ, kept, &لمبائی_2);
        حافظہ_نقل(stripped + stripped_len, متن, (سائز_t)لمبائی_2);
        res = PyUnicode_DecodeUTF8(stripped, stripped_سائز, تہی);
        PyMem_Free(stripped);
    }
    ورنہ {
        Py_ssize_t اظہاریہ_len;
        ثابت حرف *اظہاریہ = Py_Tok_ماخذ_وسعت_View(
            &tok->ماخذ, state->اظہاریہ_وسعت, &اظہاریہ_len);
        res = PyUnicode_DecodeUTF8(اظہاریہ, اظہاریہ_len, تہی);
    }

    اگر (!res) {
        واپس -1;
    }
    token->metadata = res;
    واپس 0;
}

ساکن صحیح
begin_ftstring_اظہاریہ(ساخت tok_state *tok, ftstring_state *state,
                    _PyTok_Off اظہاریہ_آغاز)
{
    أكد(state->منوال != FTSTRING_منوال_عبارت);
    state->اظہاریہ_وسعت = (Py_Tok_وسعت){اظہاریہ_آغاز, -1};
    اگر (state->comments != تہی) {
        state->comments->گنتی = 0;
    }
    اگر (state->replacement_گہرائی >= MAX_اظہاریہ_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            Py_Lexer_String_سابقہ(state->قسم));
        واپس -1;
    }
    state->replacement_گہرائی++;
    state->منوال = FTSTRING_منوال_عبارت;
    state->debug_اظہاریہ = 0;
    واپس 0;
}

صحیح
_PyLexer_ftstring_punctuation(ساخت tok_state *tok, ftstring_state *state,
                              ساخت token *token, صحیح c)
{
    أكد(state->منوال == FTSTRING_منوال_عبارت);
    أكد(c == ':' || c == '}' || c == '!');
    اگر (Py_Lexer_FTString_Bracket_گہرائی(tok, state) != state->replacement_گہرائی) {
        واپس 0;
    }
    اگر (c == '!') {
        صحیح اگلا = tok_nextc(tok);
        tok_backup(tok, اگلا);
        اگر (اگلا == '=') {
            واپس 0;
        }
    }
    اگر (اختتام_ftstring_اظہاریہ(tok, state, token) < 0) {
        واپس -1;
    }
    اگر (c == ':') {
        state->منوال = FTSTRING_منوال_FORMAT_SPEC;
        واپس COLON;
    }
    واپس 0;
}

صحیح
Py_Lexer_close_ftstring_اظہاریہ(ساخت tok_state *tok, ftstring_state *state,
                             صحیح c)
{
    أكد(state->منوال == FTSTRING_منوال_عبارت);
    أكد(c == ')' || c == ']' || c == '}');
    صحیح گہرائی = Py_Lexer_FTString_Bracket_گہرائی(tok, state);
    اگر (گہرائی < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                Py_Lexer_String_سابقہ(state->قسم), c);
        واپس -1;
    }
    اگر (c == '}' && گہرائی == state->replacement_گہرائی - 1) {
        state->replacement_گہرائی--;
        state->منوال = FTSTRING_منوال_درمیان;
        state->debug_اظہاریہ = 0;
    }
    واپس 0;
}

خالی
_PyLexer_mark_ftstring_debug(ساخت tok_state *tok, ftstring_state *state)
{
    أكد(state->منوال == FTSTRING_منوال_عبارت);
    اگر (Py_Lexer_FTString_Bracket_گہرائی(tok, state) == state->replacement_گہرائی) {
        state->debug_اظہاریہ = 1;
    }
}

صحیح
_PyLexer_check_string_prefixes(ساخت tok_state *tok,
                                             صحیح saw_b, صحیح saw_r, صحیح saw_u,
                                             صحیح saw_f, صحیح saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#تعریف_کرو RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    کرو {                                                                  \
        (خالی)Py_Tokenizer_syntaxerror_معلوم_range(                       \
            tok, (صحیح)(tok->آغاز + 1 - tok->سطر_آغاز),                 \
            (صحیح)(tok->cur - tok->سطر_آغاز),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        واپس -1;                                                        \
    } جب (0)

    اگر (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    اگر (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    اگر (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    اگر (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    اگر (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    اگر (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    اگر (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#تعریف_منسوخ RETURN_SYNTAX_ERROR

    واپس 0;
}

صحیح
Py_Lexer_جائزہ_fstring_آغاز(ساخت tok_state *tok, ساخت token *token,
                            صحیح c, ftstring_قسم قسم)
{
    _PyTok_Off p_آغاز = -1;
    _PyTok_Off p_انتہا = -1;

    صحیح quote = c;
    صحیح quote_سائز = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    صحیح after_quote = tok_nextc(tok);
    اگر (after_quote == quote) {
        صحیح after_after_quote = tok_nextc(tok);
        اگر (after_after_quote == quote) {
            quote_سائز = 3;
        }
        ورنہ {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    اگر (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_آغاز = tok->آغاز;
    p_انتہا = tok->cur;
    ftstring_state *state = Py_Lexer_ڈالو_FTString(tok);
    اگر (state == تہی) {
        واپس MAKE_TOKEN(ERRORTOKEN);
    }
    state->منوال = FTSTRING_منوال_درمیان;
    state->quote = quote;
    state->quote_سائز = quote_سائز;
    state->paren_level = tok->level;
    state->آغاز = tok->آغاز;
    state->آغاز_loc = tok->آغاز_loc;
    state->اظہاریہ_وسعت = (Py_Tok_وسعت){-1, -1};

    state->قسم = قسم;
    واپس _PyLexer_IsTString(قسم)
        ? MAKE_TOKEN(TSTRING_آغاز) : MAKE_TOKEN(FSTRING_آغاز);
}

صحیح
Py_Lexer_جائزہ_string(ساخت tok_state *tok, ساخت token *token, صحیح c)
{
    _PyTok_Off p_آغاز = -1;
    _PyTok_Off p_انتہا = -1;

    صحیح quote = c;
    صحیح quote_سائز = 1;             /* 1 or 3 */
    صحیح انتہا_quote_سائز = 0;
    صحیح has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    اگر (c == quote) {
        c = tok_nextc(tok);
        اگر (c == quote) {
            quote_سائز = 3;
        }
        ورنہ {
            انتہا_quote_سائز = 1;     /* empty string found */
        }
    }
    اگر (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    جب (انتہا_quote_سائز != quote_سائز) {
        c = tok_nextc(tok);
        اگر (tok->مکمل == E_ERROR) {
            واپس MAKE_TOKEN(ERRORTOKEN);
        }
        اگر (tok->مکمل == E_DECODE) {
            توڑو;
        }
        اگر (c == EOF || (quote_سائز == 1 && c == '\n')) {
            اگر (tok_failed(tok)) {
                واپس MAKE_TOKEN(ERRORTOKEN);
            }
            اگر (c == EOF && tok->allow_incomplete_داخلہ) {
                واپس incomplete_string_token(tok, token, STRING,
                                               tok->آغاز, tok->آغاز_loc);
            }
            صحیح انتہا_lineno = tok->lineno;
            _PyTok_Loc location = tok->آغاز_loc;
            ثابت حرف *سطر = Py_Tok_ماخذ_اشارہ_گر(
                &tok->ماخذ, tok->آغاز - location.byte_ستون);
            Py_ssize_t cursor_انحراف = (Py_ssize_t)location.byte_ستون + 1;

            ثابت ftstring_state *state = _PyLexer_CurrentFTString(tok);
            اگر (state != تہی) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                اگر (state->quote == quote && state->quote_سائز == quote_سائز) {
                    صحیح level = state->paren_level + state->replacement_گہرائی - 1;
                    أكد(level >= 0 && level < tok->level);
                    أكد(tok->parenstack[level] == '{');
                    صحیح lineno = tok->parenlinenostack[level];
                    اگر (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, سطر, cursor_انحراف, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            Py_Lexer_String_سابقہ(state->قسم), lineno);
                    }
                    ورنہ {
                        _PyTokenizer_syntaxerror_at(
                            tok, سطر, cursor_انحراف, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            Py_Lexer_String_سابقہ(state->قسم));
                    }
                    واپس string_error_token(tok, token, tok->آغاز, location);
                }
            }

            اگر (quote_سائز == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, سطر, cursor_انحراف, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", انتہا_lineno);
                اگر (c != '\n') {
                    tok->مکمل = E_EOFS;
                }
                واپس string_error_token(tok, token, tok->آغاز, location);
            }
            ورنہ {
                اگر (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, سطر, cursor_انحراف, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        انتہا_lineno
                    );
                } ورنہ {
                    _PyTokenizer_syntaxerror_at(
                        tok, سطر, cursor_انحراف, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", انتہا_lineno
                    );
                }
                اگر (c != '\n') {
                    tok->مکمل = E_EOLS;
                }
                واپس string_error_token(tok, token, tok->آغاز, location);
            }
        }
        اگر (c == quote) {
            انتہا_quote_سائز += 1;
        }
        ورنہ {
            انتہا_quote_سائز = 0;
            اگر (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                اگر (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                اگر (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_آغاز = tok->آغاز;
    p_انتہا = tok->cur;
    واپس MAKE_TOKEN(STRING);
}

صحیح
_PyLexer_get_ftstring(ساخت tok_state *tok, ftstring_state *current, ساخت token *token)
{
    أكد(current == _PyLexer_CurrentFTString(tok) && current->منوال != FTSTRING_منوال_عبارت);
    أكد((current->quote_سائز == 1 || current->quote_سائز == 3) &&
           current->replacement_گہرائی <= MAX_اظہاریہ_NESTING);
    _PyTok_Off p_آغاز = -1;
    _PyTok_Off p_انتہا = -1;
    صحیح token_type;
    صحیح انتہا_quote_سائز = 0;
    صحیح unicode_escape = 0;
    صحیح quote = current->quote;
    صحیح quote_سائز = current->quote_سائز;
    صحیح اندر_format_spec = current->منوال == FTSTRING_منوال_FORMAT_SPEC;
    صحیح raw = _PyLexer_IsRawString(current->قسم);

    tok->آغاز = tok->cur;
    tok->آغاز_loc = (_PyTok_Loc){tok->lineno, Py_Lexer_Byte_عمود(tok)};

    جب (انتہا_quote_سائز != quote_سائز) {
        صحیح c = tok_nextc(tok);
        اگر (tok->مکمل == E_ERROR || tok->مکمل == E_DECODE) {
            واپس MAKE_TOKEN(ERRORTOKEN);
        }

        اگر (c == EOF || (quote_سائز == 1 && c == '\n')) {
            اگر (tok_failed(tok)) {
                واپس MAKE_TOKEN(ERRORTOKEN);
            }
            اگر (c == EOF && tok->allow_incomplete_داخلہ) {
                واپس incomplete_string_token(tok, token,
                    FTSTRING_درمیان(current), current->آغاز, current->آغاز_loc);
            }

            اگر (اندر_format_spec && c == '\n') {
                واپس MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    Py_Lexer_String_سابقہ(current->قسم),
                    Py_Lexer_String_سابقہ(current->قسم)));
            }

            صحیح انتہا_lineno = tok->lineno;
            _PyTok_Loc location = current->آغاز_loc;
            ثابت حرف *سطر = Py_Tok_ماخذ_اشارہ_گر(
                &tok->ماخذ, current->آغاز - location.byte_ستون);
            Py_ssize_t cursor_انحراف = (Py_ssize_t)location.byte_ستون + 1;

            اگر (quote_سائز == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, سطر, cursor_انحراف, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    Py_Lexer_String_سابقہ(current->قسم), انتہا_lineno);
                اگر (c != '\n') {
                    tok->مکمل = E_EOFS;
                }
                واپس string_error_token(tok, token,
                    current->آغاز, location);
            }
            ورنہ {
                _PyTokenizer_syntaxerror_at(
                    tok, سطر, cursor_انحراف, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    Py_Lexer_String_سابقہ(current->قسم), انتہا_lineno);
                واپس string_error_token(tok, token,
                    current->آغاز, location);
            }
        }

        اگر (c == quote) {
            انتہا_quote_سائز += 1;
            جاری;
        } ورنہ {
            انتہا_quote_سائز = 0;
        }

        اگر (c == '{') {
            صحیح peek = tok_nextc(tok);
            اگر (peek != '{' || اندر_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off اظہاریہ_آغاز = tok->cur;
                tok_backup(tok, c);
                اگر (begin_ftstring_اظہاریہ(tok, current, اظہاریہ_آغاز) < 0) {
                    واپس MAKE_TOKEN(ERRORTOKEN);
                }
                p_آغاز = tok->آغاز;
                p_انتہا = tok->cur;
                اگر (p_آغاز == p_انتہا) {
                    واپس Py_Lexer_get_معمول(tok, current, token);
                }
            } ورنہ {
                p_آغاز = tok->آغاز;
                p_انتہا = tok->cur - 1;
            }
            جاؤ emit_درمیان;
        } ورنہ اگر (c == '}') {
            اگر (unicode_escape) {
                p_آغاز = tok->آغاز;
                p_انتہا = tok->cur;
                جاؤ emit_درمیان;
            }
            صحیح peek = tok_nextc(tok);

            صحیح bracket_گہرائی = Py_Lexer_FTString_Bracket_گہرائی(tok, current);
            اگر (peek == '}' && !اندر_format_spec && bracket_گہرائی == 0) {
                p_آغاز = tok->آغاز;
                p_انتہا = tok->cur - 1;
            }
            ورنہ {
                tok_backup(tok, peek);
                اگر (!اندر_format_spec && bracket_گہرائی == 0) {
                    اگر (tok->آغاز == tok->cur - 1) {
                        واپس MAKE_TOKEN(_PyTokenizer_syntaxerror(
                            tok, "%c-string: single '}' is not allowed",
                            Py_Lexer_String_سابقہ(current->قسم)));
                    }
                    tok_backup(tok, c);
                    p_آغاز = tok->آغاز;
                    p_انتہا = tok->cur;
                    جاؤ emit_درمیان;
                }
                tok_backup(tok, c);
                current->منوال = FTSTRING_منوال_عبارت;
                p_آغاز = tok->آغاز;
                p_انتہا = tok->cur;
            }
            جاؤ emit_درمیان;
        } ورنہ اگر (c == '\\') {
            صحیح peek = tok_nextc(tok);
            اگر (peek == '\r') {
                peek = tok_nextc(tok);
            }
            اگر (peek == '{' || peek == '}') {
                اگر (!raw) {
                    اگر (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        واپس MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                جاری;
            }

            اگر (!raw) {
                اگر (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    اگر (peek == '{') {
                        unicode_escape = 1;
                    } ورنہ {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_آغاز = tok->آغاز;
    p_انتہا = tok->cur;
    اگر (p_انتہا - quote_سائز == p_آغاز) {
        صحیح انتہا_token = FTSTRING_انتہا(current);
        Py_Lexer_نکالو_FTString(tok);
        واپس MAKE_TOKEN(انتہا_token);
    }
    برائے (صحیح i = 0; i < quote_سائز; i++) {
        tok_backup(tok, quote);
    }
    p_انتہا = tok->cur;
emit_درمیان:
    token_type = MAKE_TOKEN(FTSTRING_درمیان(current));
    token->is_raw = raw;
    واپس token_type;
}
