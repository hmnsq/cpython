#ضمن "Python.h"
#ضمن "pycore_token.h"
#ضمن "errcode.h"

#ضمن "lexer_internal.h"
#ضمن "../tokenizer/helpers.h"

#تعریف_کن MAKE_TOKEN(token_type) Py_Lexer_token_راهاندازی(tok, token, token_type, p_شروع, p_انتها)

ساكن صحيح
string_error_token(بنية tok_state *tok, بنية token *token,
                   _PyTok_Off شروع, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_ستون + 1},
        .متن_بازه = {شروع - location.byte_ستون, tok->inp},
    };
    صحيح type = Py_Lexer_token_راهاندازی(tok, token, ERRORTOKEN, -1, -1);
    token->شروع_loc = location;
    token->انتها_loc = (_PyTok_Loc){location.lineno, -1};
    أرجع type;
}

ساكن صحيح
incomplete_string_token(بنية tok_state *tok, بنية token *token,
                        صحيح type, _PyTok_Off شروع, _PyTok_Loc location)
{
    string_error_token(tok, token, شروع, location);
    tok->انجامشده = E_INCOMPLETE;
    أرجع type;
}

صحيح
Py_Lexer_رکورد_ftstring_comment(بنية tok_state *tok, ftstring_state *state,
                                 _PyTok_Off شروع, _PyTok_Off انتها)
{
    أكد(state == _PyLexer_CurrentFTString(tok) && state->نما == FTSTRING_نما_عبارت);
    إذا (state->عبارت_بازه.انتها >= 0) {
        أرجع 0;
    }
    أكد(state->عبارت_بازه.شروع >= 0);
    tokenizer_comments *comments = state->comments;
    إذا (comments == عدم || comments->شمار == comments->ظرفیت) {
        Py_ssize_t max_ظرفیت = (PY_SSIZE_T_MAX -
            (Py_ssize_t)حجم(*comments)) /
            (Py_ssize_t)حجم(*comments->spans);
        إذا (comments != عدم && comments->ظرفیت > max_ظرفیت / 2) {
            Py_Err_No_حافظه();
            أرجع -1;
        }
        Py_ssize_t ظرفیت = comments == عدم ? 4 : comments->ظرفیت * 2;
        اندازه_t اندازه = حجم(*comments) +
            (اندازه_t)ظرفیت * حجم(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, اندازه);
        إذا (resized == عدم) {
            Py_Err_No_حافظه();
            أرجع -1;
        }
        comments = resized;
        إذا (state->comments == عدم) {
            comments->شمار = 0;
        }
        comments->ظرفیت = ظرفیت;
        state->comments = comments;
    }
    comments->spans[comments->شمار++] =
        (Py_Tok_بازه){شروع, انتها};
    أرجع 0;
}

ساكن صحيح
پایان_ftstring_عبارت(بنية tok_state *tok, ftstring_state *state,
                     بنية token *token)
{
    أكد(token != عدم && state == _PyLexer_CurrentFTString(tok));
    أكد(state->نما == FTSTRING_نما_عبارت && tok->شروع >= 0);

    إذا (state->عبارت_بازه.انتها >= 0) {
        أرجع 0;
    }
    أكد(state->عبارت_بازه.شروع >= 0);
    state->عبارت_بازه.انتها = tok->شروع;
    صحيح tstring_interpolation = _PyLexer_IsTString(state->نوع) &&
        state->replacement_عمق == 1;
    إذا (!(state->debug_عبارت || tstring_interpolation) || token->metadata) {
        أرجع 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    إذا (comments != عدم && comments->شمار > 0) {
        Py_ssize_t stripped_اندازه =
            state->عبارت_بازه.انتها - state->عبارت_بازه.شروع;
        Py_ssize_t comment_شمار = 0;
        لكل (Py_ssize_t i = 0; i < comments->شمار; i++) {
            Py_Tok_بازه comment = comments->spans[i];
            أكد(Py_Tok_بازه_Is_Valid(comment));
            أكد(comment.شروع >= (i == 0 ? state->عبارت_بازه.شروع
                                           : comments->spans[i - 1].انتها));
            إذا (comment.شروع >= state->عبارت_بازه.انتها) {
                اكسر;
            }
            أكد(comment.انتها <= state->عبارت_بازه.انتها);
            stripped_اندازه -= comment.انتها - comment.شروع;
            comment_شمار++;
        }
        حرف *stripped = PyMem_Malloc((اندازه_t)stripped_اندازه);
        إذا (stripped == عدم) {
            Py_Err_No_حافظه();
            أرجع -1;
        }
        Py_Tok_بازه kept = {state->عبارت_بازه.شروع, state->عبارت_بازه.شروع};
        Py_ssize_t stripped_len = 0;
        لكل (Py_ssize_t i = 0; i < comment_شمار; i++) {
            kept.انتها = comments->spans[i].شروع;
            Py_ssize_t درازی;
            ثابت حرف *متن = Py_Tok_منبع_بازه_View(
                &tok->منبع, kept, &درازی);
            کپی_حافظه(stripped + stripped_len, متن, (اندازه_t)درازی);
            stripped_len += درازی;
            kept.شروع = comments->spans[i].انتها;
        }
        kept.انتها = state->عبارت_بازه.انتها;
        Py_ssize_t درازی;
        ثابت حرف *متن = Py_Tok_منبع_بازه_View(&tok->منبع, kept, &درازی);
        کپی_حافظه(stripped + stripped_len, متن, (اندازه_t)درازی);
        res = PyUnicode_DecodeUTF8(stripped, stripped_اندازه, عدم);
        PyMem_Free(stripped);
    }
    وإلا {
        Py_ssize_t عبارت_len;
        ثابت حرف *عبارت = Py_Tok_منبع_بازه_View(
            &tok->منبع, state->عبارت_بازه, &عبارت_len);
        res = PyUnicode_DecodeUTF8(عبارت, عبارت_len, عدم);
    }

    إذا (!res) {
        أرجع -1;
    }
    token->metadata = res;
    أرجع 0;
}

ساكن صحيح
begin_ftstring_عبارت(بنية tok_state *tok, ftstring_state *state,
                    _PyTok_Off عبارت_شروع)
{
    أكد(state->نما != FTSTRING_نما_عبارت);
    state->عبارت_بازه = (Py_Tok_بازه){عبارت_شروع, -1};
    إذا (state->comments != عدم) {
        state->comments->شمار = 0;
    }
    إذا (state->replacement_عمق >= MAX_عبارت_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            Py_Lexer_String_پیشوند(state->نوع));
        أرجع -1;
    }
    state->replacement_عمق++;
    state->نما = FTSTRING_نما_عبارت;
    state->debug_عبارت = 0;
    أرجع 0;
}

صحيح
_PyLexer_ftstring_punctuation(بنية tok_state *tok, ftstring_state *state,
                              بنية token *token, صحيح c)
{
    أكد(state->نما == FTSTRING_نما_عبارت);
    أكد(c == ':' || c == '}' || c == '!');
    إذا (Py_Lexer_FTString_Bracket_عمق(tok, state) != state->replacement_عمق) {
        أرجع 0;
    }
    إذا (c == '!') {
        صحيح بعدی = tok_nextc(tok);
        tok_backup(tok, بعدی);
        إذا (بعدی == '=') {
            أرجع 0;
        }
    }
    إذا (پایان_ftstring_عبارت(tok, state, token) < 0) {
        أرجع -1;
    }
    إذا (c == ':') {
        state->نما = FTSTRING_نما_FORMAT_SPEC;
        أرجع COLON;
    }
    أرجع 0;
}

صحيح
Py_Lexer_close_ftstring_عبارت(بنية tok_state *tok, ftstring_state *state,
                             صحيح c)
{
    أكد(state->نما == FTSTRING_نما_عبارت);
    أكد(c == ')' || c == ']' || c == '}');
    صحيح عمق = Py_Lexer_FTString_Bracket_عمق(tok, state);
    إذا (عمق < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                Py_Lexer_String_پیشوند(state->نوع), c);
        أرجع -1;
    }
    إذا (c == '}' && عمق == state->replacement_عمق - 1) {
        state->replacement_عمق--;
        state->نما = FTSTRING_نما_میانه;
        state->debug_عبارت = 0;
    }
    أرجع 0;
}

فراغ
_PyLexer_mark_ftstring_debug(بنية tok_state *tok, ftstring_state *state)
{
    أكد(state->نما == FTSTRING_نما_عبارت);
    إذا (Py_Lexer_FTString_Bracket_عمق(tok, state) == state->replacement_عمق) {
        state->debug_عبارت = 1;
    }
}

صحيح
_PyLexer_check_string_prefixes(بنية tok_state *tok,
                                             صحيح saw_b, صحيح saw_r, صحيح saw_u,
                                             صحيح saw_f, صحيح saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#تعریف_کن RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    افعل {                                                                  \
        (فراغ)Py_Tokenizer_syntaxerror_معلوم_range(                       \
            tok, (صحيح)(tok->شروع + 1 - tok->سطر_شروع),                 \
            (صحيح)(tok->cur - tok->سطر_شروع),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        أرجع -1;                                                        \
    } طالما (0)

    إذا (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    إذا (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    إذا (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    إذا (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    إذا (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    إذا (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    إذا (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#لغو_تعریف RETURN_SYNTAX_ERROR

    أرجع 0;
}

صحيح
Py_Lexer_پیمایش_fstring_شروع(بنية tok_state *tok, بنية token *token,
                            صحيح c, ftstring_نوع نوع)
{
    _PyTok_Off p_شروع = -1;
    _PyTok_Off p_انتها = -1;

    صحيح quote = c;
    صحيح quote_اندازه = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    صحيح after_quote = tok_nextc(tok);
    إذا (after_quote == quote) {
        صحيح after_after_quote = tok_nextc(tok);
        إذا (after_after_quote == quote) {
            quote_اندازه = 3;
        }
        وإلا {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    إذا (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_شروع = tok->شروع;
    p_انتها = tok->cur;
    ftstring_state *state = Py_Lexer_افزودن_FTString(tok);
    إذا (state == عدم) {
        أرجع MAKE_TOKEN(ERRORTOKEN);
    }
    state->نما = FTSTRING_نما_میانه;
    state->quote = quote;
    state->quote_اندازه = quote_اندازه;
    state->paren_level = tok->level;
    state->شروع = tok->شروع;
    state->شروع_loc = tok->شروع_loc;
    state->عبارت_بازه = (Py_Tok_بازه){-1, -1};

    state->نوع = نوع;
    أرجع _PyLexer_IsTString(نوع)
        ? MAKE_TOKEN(TSTRING_شروع) : MAKE_TOKEN(FSTRING_شروع);
}

صحيح
Py_Lexer_پیمایش_string(بنية tok_state *tok, بنية token *token, صحيح c)
{
    _PyTok_Off p_شروع = -1;
    _PyTok_Off p_انتها = -1;

    صحيح quote = c;
    صحيح quote_اندازه = 1;             /* 1 or 3 */
    صحيح انتها_quote_اندازه = 0;
    صحيح has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    إذا (c == quote) {
        c = tok_nextc(tok);
        إذا (c == quote) {
            quote_اندازه = 3;
        }
        وإلا {
            انتها_quote_اندازه = 1;     /* empty string found */
        }
    }
    إذا (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    طالما (انتها_quote_اندازه != quote_اندازه) {
        c = tok_nextc(tok);
        إذا (tok->انجامشده == E_ERROR) {
            أرجع MAKE_TOKEN(ERRORTOKEN);
        }
        إذا (tok->انجامشده == E_DECODE) {
            اكسر;
        }
        إذا (c == EOF || (quote_اندازه == 1 && c == '\n')) {
            إذا (tok_failed(tok)) {
                أرجع MAKE_TOKEN(ERRORTOKEN);
            }
            إذا (c == EOF && tok->allow_incomplete_درون_داد) {
                أرجع incomplete_string_token(tok, token, STRING,
                                               tok->شروع, tok->شروع_loc);
            }
            صحيح انتها_lineno = tok->lineno;
            _PyTok_Loc location = tok->شروع_loc;
            ثابت حرف *سطر = Py_Tok_منبع_اشاره_گر(
                &tok->منبع, tok->شروع - location.byte_ستون);
            Py_ssize_t cursor_جابه_جایی = (Py_ssize_t)location.byte_ستون + 1;

            ثابت ftstring_state *state = _PyLexer_CurrentFTString(tok);
            إذا (state != عدم) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                إذا (state->quote == quote && state->quote_اندازه == quote_اندازه) {
                    صحيح level = state->paren_level + state->replacement_عمق - 1;
                    أكد(level >= 0 && level < tok->level);
                    أكد(tok->parenstack[level] == '{');
                    صحيح lineno = tok->parenlinenostack[level];
                    إذا (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, سطر, cursor_جابه_جایی, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            Py_Lexer_String_پیشوند(state->نوع), lineno);
                    }
                    وإلا {
                        _PyTokenizer_syntaxerror_at(
                            tok, سطر, cursor_جابه_جایی, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            Py_Lexer_String_پیشوند(state->نوع));
                    }
                    أرجع string_error_token(tok, token, tok->شروع, location);
                }
            }

            إذا (quote_اندازه == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, سطر, cursor_جابه_جایی, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", انتها_lineno);
                إذا (c != '\n') {
                    tok->انجامشده = E_EOFS;
                }
                أرجع string_error_token(tok, token, tok->شروع, location);
            }
            وإلا {
                إذا (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, سطر, cursor_جابه_جایی, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        انتها_lineno
                    );
                } وإلا {
                    _PyTokenizer_syntaxerror_at(
                        tok, سطر, cursor_جابه_جایی, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", انتها_lineno
                    );
                }
                إذا (c != '\n') {
                    tok->انجامشده = E_EOLS;
                }
                أرجع string_error_token(tok, token, tok->شروع, location);
            }
        }
        إذا (c == quote) {
            انتها_quote_اندازه += 1;
        }
        وإلا {
            انتها_quote_اندازه = 0;
            إذا (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                إذا (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                إذا (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_شروع = tok->شروع;
    p_انتها = tok->cur;
    أرجع MAKE_TOKEN(STRING);
}

صحيح
_PyLexer_get_ftstring(بنية tok_state *tok, ftstring_state *current, بنية token *token)
{
    أكد(current == _PyLexer_CurrentFTString(tok) && current->نما != FTSTRING_نما_عبارت);
    أكد((current->quote_اندازه == 1 || current->quote_اندازه == 3) &&
           current->replacement_عمق <= MAX_عبارت_NESTING);
    _PyTok_Off p_شروع = -1;
    _PyTok_Off p_انتها = -1;
    صحيح token_type;
    صحيح انتها_quote_اندازه = 0;
    صحيح unicode_escape = 0;
    صحيح quote = current->quote;
    صحيح quote_اندازه = current->quote_اندازه;
    صحيح درون_format_spec = current->نما == FTSTRING_نما_FORMAT_SPEC;
    صحيح raw = _PyLexer_IsRawString(current->نوع);

    tok->شروع = tok->cur;
    tok->شروع_loc = (_PyTok_Loc){tok->lineno, Py_Lexer_Byte_ستونی(tok)};

    طالما (انتها_quote_اندازه != quote_اندازه) {
        صحيح c = tok_nextc(tok);
        إذا (tok->انجامشده == E_ERROR || tok->انجامشده == E_DECODE) {
            أرجع MAKE_TOKEN(ERRORTOKEN);
        }

        إذا (c == EOF || (quote_اندازه == 1 && c == '\n')) {
            إذا (tok_failed(tok)) {
                أرجع MAKE_TOKEN(ERRORTOKEN);
            }
            إذا (c == EOF && tok->allow_incomplete_درون_داد) {
                أرجع incomplete_string_token(tok, token,
                    FTSTRING_میانه(current), current->شروع, current->شروع_loc);
            }

            إذا (درون_format_spec && c == '\n') {
                أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    Py_Lexer_String_پیشوند(current->نوع),
                    Py_Lexer_String_پیشوند(current->نوع)));
            }

            صحيح انتها_lineno = tok->lineno;
            _PyTok_Loc location = current->شروع_loc;
            ثابت حرف *سطر = Py_Tok_منبع_اشاره_گر(
                &tok->منبع, current->شروع - location.byte_ستون);
            Py_ssize_t cursor_جابه_جایی = (Py_ssize_t)location.byte_ستون + 1;

            إذا (quote_اندازه == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, سطر, cursor_جابه_جایی, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    Py_Lexer_String_پیشوند(current->نوع), انتها_lineno);
                إذا (c != '\n') {
                    tok->انجامشده = E_EOFS;
                }
                أرجع string_error_token(tok, token,
                    current->شروع, location);
            }
            وإلا {
                _PyTokenizer_syntaxerror_at(
                    tok, سطر, cursor_جابه_جایی, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    Py_Lexer_String_پیشوند(current->نوع), انتها_lineno);
                أرجع string_error_token(tok, token,
                    current->شروع, location);
            }
        }

        إذا (c == quote) {
            انتها_quote_اندازه += 1;
            استمر;
        } وإلا {
            انتها_quote_اندازه = 0;
        }

        إذا (c == '{') {
            صحيح peek = tok_nextc(tok);
            إذا (peek != '{' || درون_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off عبارت_شروع = tok->cur;
                tok_backup(tok, c);
                إذا (begin_ftstring_عبارت(tok, current, عبارت_شروع) < 0) {
                    أرجع MAKE_TOKEN(ERRORTOKEN);
                }
                p_شروع = tok->شروع;
                p_انتها = tok->cur;
                إذا (p_شروع == p_انتها) {
                    أرجع Py_Lexer_get_نرمال(tok, current, token);
                }
            } وإلا {
                p_شروع = tok->شروع;
                p_انتها = tok->cur - 1;
            }
            اذهب emit_میانه;
        } وإلا إذا (c == '}') {
            إذا (unicode_escape) {
                p_شروع = tok->شروع;
                p_انتها = tok->cur;
                اذهب emit_میانه;
            }
            صحيح peek = tok_nextc(tok);

            صحيح bracket_عمق = Py_Lexer_FTString_Bracket_عمق(tok, current);
            إذا (peek == '}' && !درون_format_spec && bracket_عمق == 0) {
                p_شروع = tok->شروع;
                p_انتها = tok->cur - 1;
            }
            وإلا {
                tok_backup(tok, peek);
                إذا (!درون_format_spec && bracket_عمق == 0) {
                    إذا (tok->شروع == tok->cur - 1) {
                        أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(
                            tok, "%c-string: single '}' is not allowed",
                            Py_Lexer_String_پیشوند(current->نوع)));
                    }
                    tok_backup(tok, c);
                    p_شروع = tok->شروع;
                    p_انتها = tok->cur;
                    اذهب emit_میانه;
                }
                tok_backup(tok, c);
                current->نما = FTSTRING_نما_عبارت;
                p_شروع = tok->شروع;
                p_انتها = tok->cur;
            }
            اذهب emit_میانه;
        } وإلا إذا (c == '\\') {
            صحيح peek = tok_nextc(tok);
            إذا (peek == '\r') {
                peek = tok_nextc(tok);
            }
            إذا (peek == '{' || peek == '}') {
                إذا (!raw) {
                    إذا (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        أرجع MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                استمر;
            }

            إذا (!raw) {
                إذا (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    إذا (peek == '{') {
                        unicode_escape = 1;
                    } وإلا {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_شروع = tok->شروع;
    p_انتها = tok->cur;
    إذا (p_انتها - quote_اندازه == p_شروع) {
        صحيح انتها_token = FTSTRING_انتها(current);
        Py_Lexer_برداشت_FTString(tok);
        أرجع MAKE_TOKEN(انتها_token);
    }
    لكل (صحيح i = 0; i < quote_اندازه; i++) {
        tok_backup(tok, quote);
    }
    p_انتها = tok->cur;
emit_میانه:
    token_type = MAKE_TOKEN(FTSTRING_میانه(current));
    token->is_raw = raw;
    أرجع token_type;
}
