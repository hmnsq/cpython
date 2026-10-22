#ضمن "Python.h"
#ضمن "pycore_token.h"
#ضمن "errcode.h"

#ضمن "lexer_internal.h"
#ضمن "../tokenizer/helpers.h"

#وصف_ڪريو MAKE_TOKEN(token_type) Py_Lexer_token_تياري(tok, token, token_type, p_شروعات, p_پڇاڙي)

ساكن صحيح
string_error_token(بنية tok_state *tok, بنية token *token,
                   _PyTok_Off شروعات, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_کانو + 1},
        .متن_وسعت = {شروعات - location.byte_کانو, tok->inp},
    };
    صحيح type = Py_Lexer_token_تياري(tok, token, ERRORTOKEN, -1, -1);
    token->شروعات_loc = location;
    token->پڇاڙي_loc = (_PyTok_Loc){location.lineno, -1};
    أرجع type;
}

ساكن صحيح
incomplete_string_token(بنية tok_state *tok, بنية token *token,
                        صحيح type, _PyTok_Off شروعات, _PyTok_Loc location)
{
    string_error_token(tok, token, شروعات, location);
    tok->پورو = E_INCOMPLETE;
    أرجع type;
}

صحيح
Py_Lexer_رڪارڊ_ftstring_comment(بنية tok_state *tok, ftstring_state *state,
                                 _PyTok_Off شروعات, _PyTok_Off پڇاڙي)
{
    أكد(state == _PyLexer_CurrentFTString(tok) && state->گهڻائي == FTSTRING_گهڻائي_اظهار);
    إذا (state->اظهار_وسعت.پڇاڙي >= 0) {
        أرجع 0;
    }
    أكد(state->اظهار_وسعت.شروعات >= 0);
    tokenizer_comments *comments = state->comments;
    إذا (comments == عدم || comments->ڳڻپ == comments->صلاحيت) {
        Py_ssize_t max_صلاحيت = (PY_SSIZE_T_MAX -
            (Py_ssize_t)حجم(*comments)) /
            (Py_ssize_t)حجم(*comments->spans);
        إذا (comments != عدم && comments->صلاحيت > max_صلاحيت / 2) {
            Py_Err_No_يادگيري();
            أرجع -1;
        }
        Py_ssize_t صلاحيت = comments == عدم ? 4 : comments->صلاحيت * 2;
        سائيز_t سائيز = حجم(*comments) +
            (سائيز_t)صلاحيت * حجم(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, سائيز);
        إذا (resized == عدم) {
            Py_Err_No_يادگيري();
            أرجع -1;
        }
        comments = resized;
        إذا (state->comments == عدم) {
            comments->ڳڻپ = 0;
        }
        comments->صلاحيت = صلاحيت;
        state->comments = comments;
    }
    comments->spans[comments->ڳڻپ++] =
        (Py_Tok_وسعت){شروعات, پڇاڙي};
    أرجع 0;
}

ساكن صحيح
پڄاڻي_ftstring_اظهار(بنية tok_state *tok, ftstring_state *state,
                     بنية token *token)
{
    أكد(token != عدم && state == _PyLexer_CurrentFTString(tok));
    أكد(state->گهڻائي == FTSTRING_گهڻائي_اظهار && tok->شروعات >= 0);

    إذا (state->اظهار_وسعت.پڇاڙي >= 0) {
        أرجع 0;
    }
    أكد(state->اظهار_وسعت.شروعات >= 0);
    state->اظهار_وسعت.پڇاڙي = tok->شروعات;
    صحيح tstring_interpolation = _PyLexer_IsTString(state->قسم) &&
        state->replacement_اونهائي == 1;
    إذا (!(state->debug_اظهار || tstring_interpolation) || token->metadata) {
        أرجع 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    إذا (comments != عدم && comments->ڳڻپ > 0) {
        Py_ssize_t stripped_سائيز =
            state->اظهار_وسعت.پڇاڙي - state->اظهار_وسعت.شروعات;
        Py_ssize_t comment_ڳڻپ = 0;
        لكل (Py_ssize_t i = 0; i < comments->ڳڻپ; i++) {
            Py_Tok_وسعت comment = comments->spans[i];
            أكد(Py_Tok_وسعت_Is_Valid(comment));
            أكد(comment.شروعات >= (i == 0 ? state->اظهار_وسعت.شروعات
                                           : comments->spans[i - 1].پڇاڙي));
            إذا (comment.شروعات >= state->اظهار_وسعت.پڇاڙي) {
                اكسر;
            }
            أكد(comment.پڇاڙي <= state->اظهار_وسعت.پڇاڙي);
            stripped_سائيز -= comment.پڇاڙي - comment.شروعات;
            comment_ڳڻپ++;
        }
        حرف *stripped = PyMem_Malloc((سائيز_t)stripped_سائيز);
        إذا (stripped == عدم) {
            Py_Err_No_يادگيري();
            أرجع -1;
        }
        Py_Tok_وسعت kept = {state->اظهار_وسعت.شروعات, state->اظهار_وسعت.شروعات};
        Py_ssize_t stripped_len = 0;
        لكل (Py_ssize_t i = 0; i < comment_ڳڻپ; i++) {
            kept.پڇاڙي = comments->spans[i].شروعات;
            Py_ssize_t ڊيگهائي;
            ثابت حرف *متن = Py_Tok_ذريعو_وسعت_View(
                &tok->ذريعو, kept, &ڊيگهائي);
            يادگيري_نقل(stripped + stripped_len, متن, (سائيز_t)ڊيگهائي);
            stripped_len += ڊيگهائي;
            kept.شروعات = comments->spans[i].پڇاڙي;
        }
        kept.پڇاڙي = state->اظهار_وسعت.پڇاڙي;
        Py_ssize_t ڊيگهائي;
        ثابت حرف *متن = Py_Tok_ذريعو_وسعت_View(&tok->ذريعو, kept, &ڊيگهائي);
        يادگيري_نقل(stripped + stripped_len, متن, (سائيز_t)ڊيگهائي);
        res = PyUnicode_DecodeUTF8(stripped, stripped_سائيز, عدم);
        PyMem_Free(stripped);
    }
    وإلا {
        Py_ssize_t اظهار_len;
        ثابت حرف *اظهار = Py_Tok_ذريعو_وسعت_View(
            &tok->ذريعو, state->اظهار_وسعت, &اظهار_len);
        res = PyUnicode_DecodeUTF8(اظهار, اظهار_len, عدم);
    }

    إذا (!res) {
        أرجع -1;
    }
    token->metadata = res;
    أرجع 0;
}

ساكن صحيح
begin_ftstring_اظهار(بنية tok_state *tok, ftstring_state *state,
                    _PyTok_Off اظهار_شروعات)
{
    أكد(state->گهڻائي != FTSTRING_گهڻائي_اظهار);
    state->اظهار_وسعت = (Py_Tok_وسعت){اظهار_شروعات, -1};
    إذا (state->comments != عدم) {
        state->comments->ڳڻپ = 0;
    }
    إذا (state->replacement_اونهائي >= MAX_اظهار_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            Py_Lexer_String_اڳياڙي(state->قسم));
        أرجع -1;
    }
    state->replacement_اونهائي++;
    state->گهڻائي = FTSTRING_گهڻائي_اظهار;
    state->debug_اظهار = 0;
    أرجع 0;
}

صحيح
_PyLexer_ftstring_punctuation(بنية tok_state *tok, ftstring_state *state,
                              بنية token *token, صحيح c)
{
    أكد(state->گهڻائي == FTSTRING_گهڻائي_اظهار);
    أكد(c == ':' || c == '}' || c == '!');
    إذا (Py_Lexer_FTString_Bracket_اونهائي(tok, state) != state->replacement_اونهائي) {
        أرجع 0;
    }
    إذا (c == '!') {
        صحيح ايندڙ = tok_nextc(tok);
        tok_backup(tok, ايندڙ);
        إذا (ايندڙ == '=') {
            أرجع 0;
        }
    }
    إذا (پڄاڻي_ftstring_اظهار(tok, state, token) < 0) {
        أرجع -1;
    }
    إذا (c == ':') {
        state->گهڻائي = FTSTRING_گهڻائي_FORMAT_SPEC;
        أرجع COLON;
    }
    أرجع 0;
}

صحيح
Py_Lexer_close_ftstring_اظهار(بنية tok_state *tok, ftstring_state *state,
                             صحيح c)
{
    أكد(state->گهڻائي == FTSTRING_گهڻائي_اظهار);
    أكد(c == ')' || c == ']' || c == '}');
    صحيح اونهائي = Py_Lexer_FTString_Bracket_اونهائي(tok, state);
    إذا (اونهائي < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                Py_Lexer_String_اڳياڙي(state->قسم), c);
        أرجع -1;
    }
    إذا (c == '}' && اونهائي == state->replacement_اونهائي - 1) {
        state->replacement_اونهائي--;
        state->گهڻائي = FTSTRING_گهڻائي_وچ;
        state->debug_اظهار = 0;
    }
    أرجع 0;
}

فراغ
_PyLexer_mark_ftstring_debug(بنية tok_state *tok, ftstring_state *state)
{
    أكد(state->گهڻائي == FTSTRING_گهڻائي_اظهار);
    إذا (Py_Lexer_FTString_Bracket_اونهائي(tok, state) == state->replacement_اونهائي) {
        state->debug_اظهار = 1;
    }
}

صحيح
_PyLexer_check_string_prefixes(بنية tok_state *tok,
                                             صحيح saw_b, صحيح saw_r, صحيح saw_u,
                                             صحيح saw_f, صحيح saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#وصف_ڪريو RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    افعل {                                                                  \
        (فراغ)Py_Tokenizer_syntaxerror_معلوم_range(                       \
            tok, (صحيح)(tok->شروعات + 1 - tok->سٽ_شروعات),                 \
            (صحيح)(tok->cur - tok->سٽ_شروعات),                            \
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

#وصف_رد RETURN_SYNTAX_ERROR

    أرجع 0;
}

صحيح
Py_Lexer_جاچ_fstring_شروعات(بنية tok_state *tok, بنية token *token,
                            صحيح c, ftstring_قسم قسم)
{
    _PyTok_Off p_شروعات = -1;
    _PyTok_Off p_پڇاڙي = -1;

    صحيح quote = c;
    صحيح quote_سائيز = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    صحيح after_quote = tok_nextc(tok);
    إذا (after_quote == quote) {
        صحيح after_after_quote = tok_nextc(tok);
        إذا (after_after_quote == quote) {
            quote_سائيز = 3;
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

    p_شروعات = tok->شروعات;
    p_پڇاڙي = tok->cur;
    ftstring_state *state = Py_Lexer_ڌڪو_FTString(tok);
    إذا (state == عدم) {
        أرجع MAKE_TOKEN(ERRORTOKEN);
    }
    state->گهڻائي = FTSTRING_گهڻائي_وچ;
    state->quote = quote;
    state->quote_سائيز = quote_سائيز;
    state->paren_level = tok->level;
    state->شروعات = tok->شروعات;
    state->شروعات_loc = tok->شروعات_loc;
    state->اظهار_وسعت = (Py_Tok_وسعت){-1, -1};

    state->قسم = قسم;
    أرجع _PyLexer_IsTString(قسم)
        ? MAKE_TOKEN(TSTRING_شروعات) : MAKE_TOKEN(FSTRING_شروعات);
}

صحيح
Py_Lexer_جاچ_string(بنية tok_state *tok, بنية token *token, صحيح c)
{
    _PyTok_Off p_شروعات = -1;
    _PyTok_Off p_پڇاڙي = -1;

    صحيح quote = c;
    صحيح quote_سائيز = 1;             /* 1 or 3 */
    صحيح پڇاڙي_quote_سائيز = 0;
    صحيح has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    إذا (c == quote) {
        c = tok_nextc(tok);
        إذا (c == quote) {
            quote_سائيز = 3;
        }
        وإلا {
            پڇاڙي_quote_سائيز = 1;     /* empty string found */
        }
    }
    إذا (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    طالما (پڇاڙي_quote_سائيز != quote_سائيز) {
        c = tok_nextc(tok);
        إذا (tok->پورو == E_ERROR) {
            أرجع MAKE_TOKEN(ERRORTOKEN);
        }
        إذا (tok->پورو == E_DECODE) {
            اكسر;
        }
        إذا (c == EOF || (quote_سائيز == 1 && c == '\n')) {
            إذا (tok_failed(tok)) {
                أرجع MAKE_TOKEN(ERRORTOKEN);
            }
            إذا (c == EOF && tok->allow_incomplete_داخلا) {
                أرجع incomplete_string_token(tok, token, STRING,
                                               tok->شروعات, tok->شروعات_loc);
            }
            صحيح پڇاڙي_lineno = tok->lineno;
            _PyTok_Loc location = tok->شروعات_loc;
            ثابت حرف *سٽ = Py_Tok_ذريعو_اشارو(
                &tok->ذريعو, tok->شروعات - location.byte_کانو);
            Py_ssize_t cursor_وٿي = (Py_ssize_t)location.byte_کانو + 1;

            ثابت ftstring_state *state = _PyLexer_CurrentFTString(tok);
            إذا (state != عدم) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                إذا (state->quote == quote && state->quote_سائيز == quote_سائيز) {
                    صحيح level = state->paren_level + state->replacement_اونهائي - 1;
                    أكد(level >= 0 && level < tok->level);
                    أكد(tok->parenstack[level] == '{');
                    صحيح lineno = tok->parenlinenostack[level];
                    إذا (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, سٽ, cursor_وٿي, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            Py_Lexer_String_اڳياڙي(state->قسم), lineno);
                    }
                    وإلا {
                        _PyTokenizer_syntaxerror_at(
                            tok, سٽ, cursor_وٿي, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            Py_Lexer_String_اڳياڙي(state->قسم));
                    }
                    أرجع string_error_token(tok, token, tok->شروعات, location);
                }
            }

            إذا (quote_سائيز == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, سٽ, cursor_وٿي, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", پڇاڙي_lineno);
                إذا (c != '\n') {
                    tok->پورو = E_EOFS;
                }
                أرجع string_error_token(tok, token, tok->شروعات, location);
            }
            وإلا {
                إذا (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, سٽ, cursor_وٿي, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        پڇاڙي_lineno
                    );
                } وإلا {
                    _PyTokenizer_syntaxerror_at(
                        tok, سٽ, cursor_وٿي, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", پڇاڙي_lineno
                    );
                }
                إذا (c != '\n') {
                    tok->پورو = E_EOLS;
                }
                أرجع string_error_token(tok, token, tok->شروعات, location);
            }
        }
        إذا (c == quote) {
            پڇاڙي_quote_سائيز += 1;
        }
        وإلا {
            پڇاڙي_quote_سائيز = 0;
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

    p_شروعات = tok->شروعات;
    p_پڇاڙي = tok->cur;
    أرجع MAKE_TOKEN(STRING);
}

صحيح
_PyLexer_get_ftstring(بنية tok_state *tok, ftstring_state *current, بنية token *token)
{
    أكد(current == _PyLexer_CurrentFTString(tok) && current->گهڻائي != FTSTRING_گهڻائي_اظهار);
    أكد((current->quote_سائيز == 1 || current->quote_سائيز == 3) &&
           current->replacement_اونهائي <= MAX_اظهار_NESTING);
    _PyTok_Off p_شروعات = -1;
    _PyTok_Off p_پڇاڙي = -1;
    صحيح token_type;
    صحيح پڇاڙي_quote_سائيز = 0;
    صحيح unicode_escape = 0;
    صحيح quote = current->quote;
    صحيح quote_سائيز = current->quote_سائيز;
    صحيح اندريون_format_spec = current->گهڻائي == FTSTRING_گهڻائي_FORMAT_SPEC;
    صحيح raw = _PyLexer_IsRawString(current->قسم);

    tok->شروعات = tok->cur;
    tok->شروعات_loc = (_PyTok_Loc){tok->lineno, Py_Lexer_Byte_ڪالم(tok)};

    طالما (پڇاڙي_quote_سائيز != quote_سائيز) {
        صحيح c = tok_nextc(tok);
        إذا (tok->پورو == E_ERROR || tok->پورو == E_DECODE) {
            أرجع MAKE_TOKEN(ERRORTOKEN);
        }

        إذا (c == EOF || (quote_سائيز == 1 && c == '\n')) {
            إذا (tok_failed(tok)) {
                أرجع MAKE_TOKEN(ERRORTOKEN);
            }
            إذا (c == EOF && tok->allow_incomplete_داخلا) {
                أرجع incomplete_string_token(tok, token,
                    FTSTRING_وچ(current), current->شروعات, current->شروعات_loc);
            }

            إذا (اندريون_format_spec && c == '\n') {
                أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    Py_Lexer_String_اڳياڙي(current->قسم),
                    Py_Lexer_String_اڳياڙي(current->قسم)));
            }

            صحيح پڇاڙي_lineno = tok->lineno;
            _PyTok_Loc location = current->شروعات_loc;
            ثابت حرف *سٽ = Py_Tok_ذريعو_اشارو(
                &tok->ذريعو, current->شروعات - location.byte_کانو);
            Py_ssize_t cursor_وٿي = (Py_ssize_t)location.byte_کانو + 1;

            إذا (quote_سائيز == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, سٽ, cursor_وٿي, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    Py_Lexer_String_اڳياڙي(current->قسم), پڇاڙي_lineno);
                إذا (c != '\n') {
                    tok->پورو = E_EOFS;
                }
                أرجع string_error_token(tok, token,
                    current->شروعات, location);
            }
            وإلا {
                _PyTokenizer_syntaxerror_at(
                    tok, سٽ, cursor_وٿي, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    Py_Lexer_String_اڳياڙي(current->قسم), پڇاڙي_lineno);
                أرجع string_error_token(tok, token,
                    current->شروعات, location);
            }
        }

        إذا (c == quote) {
            پڇاڙي_quote_سائيز += 1;
            استمر;
        } وإلا {
            پڇاڙي_quote_سائيز = 0;
        }

        إذا (c == '{') {
            صحيح peek = tok_nextc(tok);
            إذا (peek != '{' || اندريون_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off اظهار_شروعات = tok->cur;
                tok_backup(tok, c);
                إذا (begin_ftstring_اظهار(tok, current, اظهار_شروعات) < 0) {
                    أرجع MAKE_TOKEN(ERRORTOKEN);
                }
                p_شروعات = tok->شروعات;
                p_پڇاڙي = tok->cur;
                إذا (p_شروعات == p_پڇاڙي) {
                    أرجع Py_Lexer_get_عام(tok, current, token);
                }
            } وإلا {
                p_شروعات = tok->شروعات;
                p_پڇاڙي = tok->cur - 1;
            }
            اذهب emit_وچ;
        } وإلا إذا (c == '}') {
            إذا (unicode_escape) {
                p_شروعات = tok->شروعات;
                p_پڇاڙي = tok->cur;
                اذهب emit_وچ;
            }
            صحيح peek = tok_nextc(tok);

            صحيح bracket_اونهائي = Py_Lexer_FTString_Bracket_اونهائي(tok, current);
            إذا (peek == '}' && !اندريون_format_spec && bracket_اونهائي == 0) {
                p_شروعات = tok->شروعات;
                p_پڇاڙي = tok->cur - 1;
            }
            وإلا {
                tok_backup(tok, peek);
                إذا (!اندريون_format_spec && bracket_اونهائي == 0) {
                    إذا (tok->شروعات == tok->cur - 1) {
                        أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(
                            tok, "%c-string: single '}' is not allowed",
                            Py_Lexer_String_اڳياڙي(current->قسم)));
                    }
                    tok_backup(tok, c);
                    p_شروعات = tok->شروعات;
                    p_پڇاڙي = tok->cur;
                    اذهب emit_وچ;
                }
                tok_backup(tok, c);
                current->گهڻائي = FTSTRING_گهڻائي_اظهار;
                p_شروعات = tok->شروعات;
                p_پڇاڙي = tok->cur;
            }
            اذهب emit_وچ;
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

    p_شروعات = tok->شروعات;
    p_پڇاڙي = tok->cur;
    إذا (p_پڇاڙي - quote_سائيز == p_شروعات) {
        صحيح پڇاڙي_token = FTSTRING_پڇاڙي(current);
        Py_Lexer_ڪڍو_FTString(tok);
        أرجع MAKE_TOKEN(پڇاڙي_token);
    }
    لكل (صحيح i = 0; i < quote_سائيز; i++) {
        tok_backup(tok, quote);
    }
    p_پڇاڙي = tok->cur;
emit_وچ:
    token_type = MAKE_TOKEN(FTSTRING_وچ(current));
    token->is_raw = raw;
    أرجع token_type;
}
