#ضمن "Python.h"
#ضمن "pycore_token.h"
#ضمن "errcode.h"

#ضمن "lexer_internal.h"
#ضمن "../tokenizer/helpers.h"

#تعریف_کړه MAKE_TOKEN(token_type) Py_Lexer_token_تیاری(tok, token, token_type, p_پیل, p_پای_ټکی)

ساكن صحيح
string_error_token(بنية tok_state *tok, بنية token *token,
                   _PyTok_Off پیل, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_ستون + 1},
        .متن_پراختیا = {پیل - location.byte_ستون, tok->inp},
    };
    صحيح type = Py_Lexer_token_تیاری(tok, token, ERRORTOKEN, -1, -1);
    token->پیل_loc = location;
    token->پای_ټکی_loc = (_PyTok_Loc){location.lineno, -1};
    أرجع type;
}

ساكن صحيح
incomplete_string_token(بنية tok_state *tok, بنية token *token,
                        صحيح type, _PyTok_Off پیل, _PyTok_Loc location)
{
    string_error_token(tok, token, پیل, location);
    tok->بشپړ = E_INCOMPLETE;
    أرجع type;
}

صحيح
Py_Lexer_ریکارډ_ftstring_comment(بنية tok_state *tok, ftstring_state *state,
                                 _PyTok_Off پیل, _PyTok_Off پای_ټکی)
{
    أكد(state == _PyLexer_CurrentFTString(tok) && state->ډېرپېښ == FTSTRING_ډېرپېښ_افاده);
    إذا (state->بیان_پراختیا.پای_ټکی >= 0) {
        أرجع 0;
    }
    أكد(state->بیان_پراختیا.پیل >= 0);
    tokenizer_comments *comments = state->comments;
    إذا (comments == عدم || comments->شمېر == comments->ظرفیت) {
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
            comments->شمېر = 0;
        }
        comments->ظرفیت = ظرفیت;
        state->comments = comments;
    }
    comments->spans[comments->شمېر++] =
        (Py_Tok_پراختیا){پیل, پای_ټکی};
    أرجع 0;
}

ساكن صحيح
پای_ftstring_بیان(بنية tok_state *tok, ftstring_state *state,
                     بنية token *token)
{
    أكد(token != عدم && state == _PyLexer_CurrentFTString(tok));
    أكد(state->ډېرپېښ == FTSTRING_ډېرپېښ_افاده && tok->پیل >= 0);

    إذا (state->بیان_پراختیا.پای_ټکی >= 0) {
        أرجع 0;
    }
    أكد(state->بیان_پراختیا.پیل >= 0);
    state->بیان_پراختیا.پای_ټکی = tok->پیل;
    صحيح tstring_interpolation = _PyLexer_IsTString(state->ډول) &&
        state->replacement_ژوروالی == 1;
    إذا (!(state->debug_بیان || tstring_interpolation) || token->metadata) {
        أرجع 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    إذا (comments != عدم && comments->شمېر > 0) {
        Py_ssize_t stripped_اندازه =
            state->بیان_پراختیا.پای_ټکی - state->بیان_پراختیا.پیل;
        Py_ssize_t comment_شمېر = 0;
        لكل (Py_ssize_t i = 0; i < comments->شمېر; i++) {
            Py_Tok_پراختیا comment = comments->spans[i];
            أكد(Py_Tok_پراختیا_Is_Valid(comment));
            أكد(comment.پیل >= (i == 0 ? state->بیان_پراختیا.پیل
                                           : comments->spans[i - 1].پای_ټکی));
            إذا (comment.پیل >= state->بیان_پراختیا.پای_ټکی) {
                اكسر;
            }
            أكد(comment.پای_ټکی <= state->بیان_پراختیا.پای_ټکی);
            stripped_اندازه -= comment.پای_ټکی - comment.پیل;
            comment_شمېر++;
        }
        حرف *stripped = PyMem_Malloc((اندازه_t)stripped_اندازه);
        إذا (stripped == عدم) {
            Py_Err_No_حافظه();
            أرجع -1;
        }
        Py_Tok_پراختیا kept = {state->بیان_پراختیا.پیل, state->بیان_پراختیا.پیل};
        Py_ssize_t stripped_len = 0;
        لكل (Py_ssize_t i = 0; i < comment_شمېر; i++) {
            kept.پای_ټکی = comments->spans[i].پیل;
            Py_ssize_t اوږدتیا;
            ثابت حرف *متن = Py_Tok_سرچینه_پراختیا_View(
                &tok->سرچینه, kept, &اوږدتیا);
            حافظه_نقل(stripped + stripped_len, متن, (اندازه_t)اوږدتیا);
            stripped_len += اوږدتیا;
            kept.پیل = comments->spans[i].پای_ټکی;
        }
        kept.پای_ټکی = state->بیان_پراختیا.پای_ټکی;
        Py_ssize_t اوږدتیا;
        ثابت حرف *متن = Py_Tok_سرچینه_پراختیا_View(&tok->سرچینه, kept, &اوږدتیا);
        حافظه_نقل(stripped + stripped_len, متن, (اندازه_t)اوږدتیا);
        res = PyUnicode_DecodeUTF8(stripped, stripped_اندازه, عدم);
        PyMem_Free(stripped);
    }
    وإلا {
        Py_ssize_t بیان_len;
        ثابت حرف *بیان = Py_Tok_سرچینه_پراختیا_View(
            &tok->سرچینه, state->بیان_پراختیا, &بیان_len);
        res = PyUnicode_DecodeUTF8(بیان, بیان_len, عدم);
    }

    إذا (!res) {
        أرجع -1;
    }
    token->metadata = res;
    أرجع 0;
}

ساكن صحيح
begin_ftstring_بیان(بنية tok_state *tok, ftstring_state *state,
                    _PyTok_Off بیان_پیل)
{
    أكد(state->ډېرپېښ != FTSTRING_ډېرپېښ_افاده);
    state->بیان_پراختیا = (Py_Tok_پراختیا){بیان_پیل, -1};
    إذا (state->comments != عدم) {
        state->comments->شمېر = 0;
    }
    إذا (state->replacement_ژوروالی >= MAX_بیان_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            Py_Lexer_String_مختاړی(state->ډول));
        أرجع -1;
    }
    state->replacement_ژوروالی++;
    state->ډېرپېښ = FTSTRING_ډېرپېښ_افاده;
    state->debug_بیان = 0;
    أرجع 0;
}

صحيح
_PyLexer_ftstring_punctuation(بنية tok_state *tok, ftstring_state *state,
                              بنية token *token, صحيح c)
{
    أكد(state->ډېرپېښ == FTSTRING_ډېرپېښ_افاده);
    أكد(c == ':' || c == '}' || c == '!');
    إذا (Py_Lexer_FTString_Bracket_ژوروالی(tok, state) != state->replacement_ژوروالی) {
        أرجع 0;
    }
    إذا (c == '!') {
        صحيح بل = tok_nextc(tok);
        tok_backup(tok, بل);
        إذا (بل == '=') {
            أرجع 0;
        }
    }
    إذا (پای_ftstring_بیان(tok, state, token) < 0) {
        أرجع -1;
    }
    إذا (c == ':') {
        state->ډېرپېښ = FTSTRING_ډېرپېښ_FORMAT_SPEC;
        أرجع COLON;
    }
    أرجع 0;
}

صحيح
Py_Lexer_close_ftstring_بیان(بنية tok_state *tok, ftstring_state *state,
                             صحيح c)
{
    أكد(state->ډېرپېښ == FTSTRING_ډېرپېښ_افاده);
    أكد(c == ')' || c == ']' || c == '}');
    صحيح ژوروالی = Py_Lexer_FTString_Bracket_ژوروالی(tok, state);
    إذا (ژوروالی < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                Py_Lexer_String_مختاړی(state->ډول), c);
        أرجع -1;
    }
    إذا (c == '}' && ژوروالی == state->replacement_ژوروالی - 1) {
        state->replacement_ژوروالی--;
        state->ډېرپېښ = FTSTRING_ډېرپېښ_منځ;
        state->debug_بیان = 0;
    }
    أرجع 0;
}

فراغ
_PyLexer_mark_ftstring_debug(بنية tok_state *tok, ftstring_state *state)
{
    أكد(state->ډېرپېښ == FTSTRING_ډېرپېښ_افاده);
    إذا (Py_Lexer_FTString_Bracket_ژوروالی(tok, state) == state->replacement_ژوروالی) {
        state->debug_بیان = 1;
    }
}

صحيح
_PyLexer_check_string_prefixes(بنية tok_state *tok,
                                             صحيح saw_b, صحيح saw_r, صحيح saw_u,
                                             صحيح saw_f, صحيح saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#تعریف_کړه RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    افعل {                                                                  \
        (فراغ)Py_Tokenizer_syntaxerror_معلوم_range(                       \
            tok, (صحيح)(tok->پیل + 1 - tok->کرښه_پیل),                 \
            (صحيح)(tok->cur - tok->کرښه_پیل),                            \
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

#تعریف_لغوه RETURN_SYNTAX_ERROR

    أرجع 0;
}

صحيح
Py_Lexer_لټون_fstring_پیل(بنية tok_state *tok, بنية token *token,
                            صحيح c, ftstring_ډول ډول)
{
    _PyTok_Off p_پیل = -1;
    _PyTok_Off p_پای_ټکی = -1;

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

    p_پیل = tok->پیل;
    p_پای_ټکی = tok->cur;
    ftstring_state *state = Py_Lexer_ټېل_FTString(tok);
    إذا (state == عدم) {
        أرجع MAKE_TOKEN(ERRORTOKEN);
    }
    state->ډېرپېښ = FTSTRING_ډېرپېښ_منځ;
    state->quote = quote;
    state->quote_اندازه = quote_اندازه;
    state->paren_level = tok->level;
    state->پیل = tok->پیل;
    state->پیل_loc = tok->پیل_loc;
    state->بیان_پراختیا = (Py_Tok_پراختیا){-1, -1};

    state->ډول = ډول;
    أرجع _PyLexer_IsTString(ډول)
        ? MAKE_TOKEN(TSTRING_پیل) : MAKE_TOKEN(FSTRING_پیل);
}

صحيح
Py_Lexer_لټون_string(بنية tok_state *tok, بنية token *token, صحيح c)
{
    _PyTok_Off p_پیل = -1;
    _PyTok_Off p_پای_ټکی = -1;

    صحيح quote = c;
    صحيح quote_اندازه = 1;             /* 1 or 3 */
    صحيح پای_ټکی_quote_اندازه = 0;
    صحيح has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    إذا (c == quote) {
        c = tok_nextc(tok);
        إذا (c == quote) {
            quote_اندازه = 3;
        }
        وإلا {
            پای_ټکی_quote_اندازه = 1;     /* empty string found */
        }
    }
    إذا (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    طالما (پای_ټکی_quote_اندازه != quote_اندازه) {
        c = tok_nextc(tok);
        إذا (tok->بشپړ == E_ERROR) {
            أرجع MAKE_TOKEN(ERRORTOKEN);
        }
        إذا (tok->بشپړ == E_DECODE) {
            اكسر;
        }
        إذا (c == EOF || (quote_اندازه == 1 && c == '\n')) {
            إذا (tok_failed(tok)) {
                أرجع MAKE_TOKEN(ERRORTOKEN);
            }
            إذا (c == EOF && tok->allow_incomplete_ننوت) {
                أرجع incomplete_string_token(tok, token, STRING,
                                               tok->پیل, tok->پیل_loc);
            }
            صحيح پای_ټکی_lineno = tok->lineno;
            _PyTok_Loc location = tok->پیل_loc;
            ثابت حرف *کرښه = Py_Tok_سرچینه_اشاره_ګر(
                &tok->سرچینه, tok->پیل - location.byte_ستون);
            Py_ssize_t cursor_دننه_واټن = (Py_ssize_t)location.byte_ستون + 1;

            ثابت ftstring_state *state = _PyLexer_CurrentFTString(tok);
            إذا (state != عدم) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                إذا (state->quote == quote && state->quote_اندازه == quote_اندازه) {
                    صحيح level = state->paren_level + state->replacement_ژوروالی - 1;
                    أكد(level >= 0 && level < tok->level);
                    أكد(tok->parenstack[level] == '{');
                    صحيح lineno = tok->parenlinenostack[level];
                    إذا (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, کرښه, cursor_دننه_واټن, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            Py_Lexer_String_مختاړی(state->ډول), lineno);
                    }
                    وإلا {
                        _PyTokenizer_syntaxerror_at(
                            tok, کرښه, cursor_دننه_واټن, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            Py_Lexer_String_مختاړی(state->ډول));
                    }
                    أرجع string_error_token(tok, token, tok->پیل, location);
                }
            }

            إذا (quote_اندازه == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, کرښه, cursor_دننه_واټن, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", پای_ټکی_lineno);
                إذا (c != '\n') {
                    tok->بشپړ = E_EOFS;
                }
                أرجع string_error_token(tok, token, tok->پیل, location);
            }
            وإلا {
                إذا (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, کرښه, cursor_دننه_واټن, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        پای_ټکی_lineno
                    );
                } وإلا {
                    _PyTokenizer_syntaxerror_at(
                        tok, کرښه, cursor_دننه_واټن, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", پای_ټکی_lineno
                    );
                }
                إذا (c != '\n') {
                    tok->بشپړ = E_EOLS;
                }
                أرجع string_error_token(tok, token, tok->پیل, location);
            }
        }
        إذا (c == quote) {
            پای_ټکی_quote_اندازه += 1;
        }
        وإلا {
            پای_ټکی_quote_اندازه = 0;
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

    p_پیل = tok->پیل;
    p_پای_ټکی = tok->cur;
    أرجع MAKE_TOKEN(STRING);
}

صحيح
_PyLexer_get_ftstring(بنية tok_state *tok, ftstring_state *current, بنية token *token)
{
    أكد(current == _PyLexer_CurrentFTString(tok) && current->ډېرپېښ != FTSTRING_ډېرپېښ_افاده);
    أكد((current->quote_اندازه == 1 || current->quote_اندازه == 3) &&
           current->replacement_ژوروالی <= MAX_بیان_NESTING);
    _PyTok_Off p_پیل = -1;
    _PyTok_Off p_پای_ټکی = -1;
    صحيح token_type;
    صحيح پای_ټکی_quote_اندازه = 0;
    صحيح unicode_escape = 0;
    صحيح quote = current->quote;
    صحيح quote_اندازه = current->quote_اندازه;
    صحيح دننه_format_spec = current->ډېرپېښ == FTSTRING_ډېرپېښ_FORMAT_SPEC;
    صحيح raw = _PyLexer_IsRawString(current->ډول);

    tok->پیل = tok->cur;
    tok->پیل_loc = (_PyTok_Loc){tok->lineno, Py_Lexer_Byte_ستنه(tok)};

    طالما (پای_ټکی_quote_اندازه != quote_اندازه) {
        صحيح c = tok_nextc(tok);
        إذا (tok->بشپړ == E_ERROR || tok->بشپړ == E_DECODE) {
            أرجع MAKE_TOKEN(ERRORTOKEN);
        }

        إذا (c == EOF || (quote_اندازه == 1 && c == '\n')) {
            إذا (tok_failed(tok)) {
                أرجع MAKE_TOKEN(ERRORTOKEN);
            }
            إذا (c == EOF && tok->allow_incomplete_ننوت) {
                أرجع incomplete_string_token(tok, token,
                    FTSTRING_منځ(current), current->پیل, current->پیل_loc);
            }

            إذا (دننه_format_spec && c == '\n') {
                أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    Py_Lexer_String_مختاړی(current->ډول),
                    Py_Lexer_String_مختاړی(current->ډول)));
            }

            صحيح پای_ټکی_lineno = tok->lineno;
            _PyTok_Loc location = current->پیل_loc;
            ثابت حرف *کرښه = Py_Tok_سرچینه_اشاره_ګر(
                &tok->سرچینه, current->پیل - location.byte_ستون);
            Py_ssize_t cursor_دننه_واټن = (Py_ssize_t)location.byte_ستون + 1;

            إذا (quote_اندازه == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, کرښه, cursor_دننه_واټن, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    Py_Lexer_String_مختاړی(current->ډول), پای_ټکی_lineno);
                إذا (c != '\n') {
                    tok->بشپړ = E_EOFS;
                }
                أرجع string_error_token(tok, token,
                    current->پیل, location);
            }
            وإلا {
                _PyTokenizer_syntaxerror_at(
                    tok, کرښه, cursor_دننه_واټن, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    Py_Lexer_String_مختاړی(current->ډول), پای_ټکی_lineno);
                أرجع string_error_token(tok, token,
                    current->پیل, location);
            }
        }

        إذا (c == quote) {
            پای_ټکی_quote_اندازه += 1;
            استمر;
        } وإلا {
            پای_ټکی_quote_اندازه = 0;
        }

        إذا (c == '{') {
            صحيح peek = tok_nextc(tok);
            إذا (peek != '{' || دننه_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off بیان_پیل = tok->cur;
                tok_backup(tok, c);
                إذا (begin_ftstring_بیان(tok, current, بیان_پیل) < 0) {
                    أرجع MAKE_TOKEN(ERRORTOKEN);
                }
                p_پیل = tok->پیل;
                p_پای_ټکی = tok->cur;
                إذا (p_پیل == p_پای_ټکی) {
                    أرجع Py_Lexer_get_نورمال(tok, current, token);
                }
            } وإلا {
                p_پیل = tok->پیل;
                p_پای_ټکی = tok->cur - 1;
            }
            اذهب emit_منځ;
        } وإلا إذا (c == '}') {
            إذا (unicode_escape) {
                p_پیل = tok->پیل;
                p_پای_ټکی = tok->cur;
                اذهب emit_منځ;
            }
            صحيح peek = tok_nextc(tok);

            صحيح bracket_ژوروالی = Py_Lexer_FTString_Bracket_ژوروالی(tok, current);
            إذا (peek == '}' && !دننه_format_spec && bracket_ژوروالی == 0) {
                p_پیل = tok->پیل;
                p_پای_ټکی = tok->cur - 1;
            }
            وإلا {
                tok_backup(tok, peek);
                إذا (!دننه_format_spec && bracket_ژوروالی == 0) {
                    إذا (tok->پیل == tok->cur - 1) {
                        أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(
                            tok, "%c-string: single '}' is not allowed",
                            Py_Lexer_String_مختاړی(current->ډول)));
                    }
                    tok_backup(tok, c);
                    p_پیل = tok->پیل;
                    p_پای_ټکی = tok->cur;
                    اذهب emit_منځ;
                }
                tok_backup(tok, c);
                current->ډېرپېښ = FTSTRING_ډېرپېښ_افاده;
                p_پیل = tok->پیل;
                p_پای_ټکی = tok->cur;
            }
            اذهب emit_منځ;
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

    p_پیل = tok->پیل;
    p_پای_ټکی = tok->cur;
    إذا (p_پای_ټکی - quote_اندازه == p_پیل) {
        صحيح پای_ټکی_token = FTSTRING_پای_ټکی(current);
        Py_Lexer_راوباسه_FTString(tok);
        أرجع MAKE_TOKEN(پای_ټکی_token);
    }
    لكل (صحيح i = 0; i < quote_اندازه; i++) {
        tok_backup(tok, quote);
    }
    p_پای_ټکی = tok->cur;
emit_منځ:
    token_type = MAKE_TOKEN(FTSTRING_منځ(current));
    token->is_raw = raw;
    أرجع token_type;
}
