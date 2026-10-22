#ضمن "Python.h"
#ضمن "pycore_token.h"
#ضمن "errcode.h"

#ضمن "lexer_internal.h"
#ضمن "../tokenizer/helpers.h"

#تعریف_کرو MAKE_TOKEN(token_type) Py_Lexer_token_تیاری(tok, token, token_type, p_شروع, p_سرا)

ساكن صحيح
string_error_token(بنية tok_state *tok, بنية token *token,
                   _PyTok_Off شروع, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_خانہ + 1},
        .متن_وسعت = {شروع - location.byte_خانہ, tok->inp},
    };
    صحيح type = Py_Lexer_token_تیاری(tok, token, ERRORTOKEN, -1, -1);
    token->شروع_loc = location;
    token->سرا_loc = (_PyTok_Loc){location.lineno, -1};
    أرجع type;
}

ساكن صحيح
incomplete_string_token(بنية tok_state *tok, بنية token *token,
                        صحيح type, _PyTok_Off شروع, _PyTok_Loc location)
{
    string_error_token(tok, token, شروع, location);
    tok->پورا = E_INCOMPLETE;
    أرجع type;
}

صحيح
Py_Lexer_ریکارڈ_ftstring_comment(بنية tok_state *tok, ftstring_state *state,
                                 _PyTok_Off شروع, _PyTok_Off سرا)
{
    أكد(state == _PyLexer_CurrentFTString(tok) && state->منوال == FTSTRING_منوال_عبارت);
    إذا (state->اظہاریہ_وسعت.سرا >= 0) {
        أرجع 0;
    }
    أكد(state->اظہاریہ_وسعت.شروع >= 0);
    tokenizer_comments *comments = state->comments;
    إذا (comments == عدم || comments->گنتی == comments->سمرتھا) {
        Py_ssize_t max_سمرتھا = (PY_SSIZE_T_MAX -
            (Py_ssize_t)حجم(*comments)) /
            (Py_ssize_t)حجم(*comments->spans);
        إذا (comments != عدم && comments->سمرتھا > max_سمرتھا / 2) {
            Py_Err_No_حافظہ();
            أرجع -1;
        }
        Py_ssize_t سمرتھا = comments == عدم ? 4 : comments->سمرتھا * 2;
        ناپ_t ناپ = حجم(*comments) +
            (ناپ_t)سمرتھا * حجم(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, ناپ);
        إذا (resized == عدم) {
            Py_Err_No_حافظہ();
            أرجع -1;
        }
        comments = resized;
        إذا (state->comments == عدم) {
            comments->گنتی = 0;
        }
        comments->سمرتھا = سمرتھا;
        state->comments = comments;
    }
    comments->spans[comments->گنتی++] =
        (Py_Tok_وسعت){شروع, سرا};
    أرجع 0;
}

ساكن صحيح
انت_ftstring_اظہاریہ(بنية tok_state *tok, ftstring_state *state,
                     بنية token *token)
{
    أكد(token != عدم && state == _PyLexer_CurrentFTString(tok));
    أكد(state->منوال == FTSTRING_منوال_عبارت && tok->شروع >= 0);

    إذا (state->اظہاریہ_وسعت.سرا >= 0) {
        أرجع 0;
    }
    أكد(state->اظہاریہ_وسعت.شروع >= 0);
    state->اظہاریہ_وسعت.سرا = tok->شروع;
    صحيح tstring_interpolation = _PyLexer_IsTString(state->قسم) &&
        state->replacement_ڈونگھائی == 1;
    إذا (!(state->debug_اظہاریہ || tstring_interpolation) || token->metadata) {
        أرجع 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    إذا (comments != عدم && comments->گنتی > 0) {
        Py_ssize_t stripped_ناپ =
            state->اظہاریہ_وسعت.سرا - state->اظہاریہ_وسعت.شروع;
        Py_ssize_t comment_گنتی = 0;
        لكل (Py_ssize_t i = 0; i < comments->گنتی; i++) {
            Py_Tok_وسعت comment = comments->spans[i];
            أكد(Py_Tok_وسعت_Is_Valid(comment));
            أكد(comment.شروع >= (i == 0 ? state->اظہاریہ_وسعت.شروع
                                           : comments->spans[i - 1].سرا));
            إذا (comment.شروع >= state->اظہاریہ_وسعت.سرا) {
                اكسر;
            }
            أكد(comment.سرا <= state->اظہاریہ_وسعت.سرا);
            stripped_ناپ -= comment.سرا - comment.شروع;
            comment_گنتی++;
        }
        حرف *stripped = PyMem_Malloc((ناپ_t)stripped_ناپ);
        إذا (stripped == عدم) {
            Py_Err_No_حافظہ();
            أرجع -1;
        }
        Py_Tok_وسعت kept = {state->اظہاریہ_وسعت.شروع, state->اظہاریہ_وسعت.شروع};
        Py_ssize_t stripped_len = 0;
        لكل (Py_ssize_t i = 0; i < comment_گنتی; i++) {
            kept.سرا = comments->spans[i].شروع;
            Py_ssize_t لمائی;
            ثابت حرف *متن = Py_Tok_سروت_وسعت_View(
                &tok->سروت, kept, &لمائی);
            حافظہ_نقل(stripped + stripped_len, متن, (ناپ_t)لمائی);
            stripped_len += لمائی;
            kept.شروع = comments->spans[i].سرا;
        }
        kept.سرا = state->اظہاریہ_وسعت.سرا;
        Py_ssize_t لمائی;
        ثابت حرف *متن = Py_Tok_سروت_وسعت_View(&tok->سروت, kept, &لمائی);
        حافظہ_نقل(stripped + stripped_len, متن, (ناپ_t)لمائی);
        res = PyUnicode_DecodeUTF8(stripped, stripped_ناپ, عدم);
        PyMem_Free(stripped);
    }
    وإلا {
        Py_ssize_t اظہاریہ_len;
        ثابت حرف *اظہاریہ = Py_Tok_سروت_وسعت_View(
            &tok->سروت, state->اظہاریہ_وسعت, &اظہاریہ_len);
        res = PyUnicode_DecodeUTF8(اظہاریہ, اظہاریہ_len, عدم);
    }

    إذا (!res) {
        أرجع -1;
    }
    token->metadata = res;
    أرجع 0;
}

ساكن صحيح
begin_ftstring_اظہاریہ(بنية tok_state *tok, ftstring_state *state,
                    _PyTok_Off اظہاریہ_شروع)
{
    أكد(state->منوال != FTSTRING_منوال_عبارت);
    state->اظہاریہ_وسعت = (Py_Tok_وسعت){اظہاریہ_شروع, -1};
    إذا (state->comments != عدم) {
        state->comments->گنتی = 0;
    }
    إذا (state->replacement_ڈونگھائی >= MAX_اظہاریہ_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            Py_Lexer_String_اگیتر(state->قسم));
        أرجع -1;
    }
    state->replacement_ڈونگھائی++;
    state->منوال = FTSTRING_منوال_عبارت;
    state->debug_اظہاریہ = 0;
    أرجع 0;
}

صحيح
_PyLexer_ftstring_punctuation(بنية tok_state *tok, ftstring_state *state,
                              بنية token *token, صحيح c)
{
    أكد(state->منوال == FTSTRING_منوال_عبارت);
    أكد(c == ':' || c == '}' || c == '!');
    إذا (Py_Lexer_FTString_Bracket_ڈونگھائی(tok, state) != state->replacement_ڈونگھائی) {
        أرجع 0;
    }
    إذا (c == '!') {
        صحيح اگلا = tok_nextc(tok);
        tok_backup(tok, اگلا);
        إذا (اگلا == '=') {
            أرجع 0;
        }
    }
    إذا (انت_ftstring_اظہاریہ(tok, state, token) < 0) {
        أرجع -1;
    }
    إذا (c == ':') {
        state->منوال = FTSTRING_منوال_FORMAT_SPEC;
        أرجع COLON;
    }
    أرجع 0;
}

صحيح
Py_Lexer_close_ftstring_اظہاریہ(بنية tok_state *tok, ftstring_state *state,
                             صحيح c)
{
    أكد(state->منوال == FTSTRING_منوال_عبارت);
    أكد(c == ')' || c == ']' || c == '}');
    صحيح ڈونگھائی = Py_Lexer_FTString_Bracket_ڈونگھائی(tok, state);
    إذا (ڈونگھائی < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                Py_Lexer_String_اگیتر(state->قسم), c);
        أرجع -1;
    }
    إذا (c == '}' && ڈونگھائی == state->replacement_ڈونگھائی - 1) {
        state->replacement_ڈونگھائی--;
        state->منوال = FTSTRING_منوال_درمیان;
        state->debug_اظہاریہ = 0;
    }
    أرجع 0;
}

فراغ
_PyLexer_mark_ftstring_debug(بنية tok_state *tok, ftstring_state *state)
{
    أكد(state->منوال == FTSTRING_منوال_عبارت);
    إذا (Py_Lexer_FTString_Bracket_ڈونگھائی(tok, state) == state->replacement_ڈونگھائی) {
        state->debug_اظہاریہ = 1;
    }
}

صحيح
_PyLexer_check_string_prefixes(بنية tok_state *tok,
                                             صحيح saw_b, صحيح saw_r, صحيح saw_u,
                                             صحيح saw_f, صحيح saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#تعریف_کرو RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
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

#تعریف_رد RETURN_SYNTAX_ERROR

    أرجع 0;
}

صحيح
Py_Lexer_جانچ_fstring_شروع(بنية tok_state *tok, بنية token *token,
                            صحيح c, ftstring_قسم قسم)
{
    _PyTok_Off p_شروع = -1;
    _PyTok_Off p_سرا = -1;

    صحيح quote = c;
    صحيح quote_ناپ = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    صحيح after_quote = tok_nextc(tok);
    إذا (after_quote == quote) {
        صحيح after_after_quote = tok_nextc(tok);
        إذا (after_after_quote == quote) {
            quote_ناپ = 3;
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
    p_سرا = tok->cur;
    ftstring_state *state = Py_Lexer_ڈالو_FTString(tok);
    إذا (state == عدم) {
        أرجع MAKE_TOKEN(ERRORTOKEN);
    }
    state->منوال = FTSTRING_منوال_درمیان;
    state->quote = quote;
    state->quote_ناپ = quote_ناپ;
    state->paren_level = tok->level;
    state->شروع = tok->شروع;
    state->شروع_loc = tok->شروع_loc;
    state->اظہاریہ_وسعت = (Py_Tok_وسعت){-1, -1};

    state->قسم = قسم;
    أرجع _PyLexer_IsTString(قسم)
        ? MAKE_TOKEN(TSTRING_شروع) : MAKE_TOKEN(FSTRING_شروع);
}

صحيح
Py_Lexer_جانچ_string(بنية tok_state *tok, بنية token *token, صحيح c)
{
    _PyTok_Off p_شروع = -1;
    _PyTok_Off p_سرا = -1;

    صحيح quote = c;
    صحيح quote_ناپ = 1;             /* 1 or 3 */
    صحيح سرا_quote_ناپ = 0;
    صحيح has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    إذا (c == quote) {
        c = tok_nextc(tok);
        إذا (c == quote) {
            quote_ناپ = 3;
        }
        وإلا {
            سرا_quote_ناپ = 1;     /* empty string found */
        }
    }
    إذا (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    طالما (سرا_quote_ناپ != quote_ناپ) {
        c = tok_nextc(tok);
        إذا (tok->پورا == E_ERROR) {
            أرجع MAKE_TOKEN(ERRORTOKEN);
        }
        إذا (tok->پورا == E_DECODE) {
            اكسر;
        }
        إذا (c == EOF || (quote_ناپ == 1 && c == '\n')) {
            إذا (tok_failed(tok)) {
                أرجع MAKE_TOKEN(ERRORTOKEN);
            }
            إذا (c == EOF && tok->allow_incomplete_داخلہ) {
                أرجع incomplete_string_token(tok, token, STRING,
                                               tok->شروع, tok->شروع_loc);
            }
            صحيح سرا_lineno = tok->lineno;
            _PyTok_Loc location = tok->شروع_loc;
            ثابت حرف *سطر = Py_Tok_سروت_اشارہ_گر(
                &tok->سروت, tok->شروع - location.byte_خانہ);
            Py_ssize_t cursor_وتھ = (Py_ssize_t)location.byte_خانہ + 1;

            ثابت ftstring_state *state = _PyLexer_CurrentFTString(tok);
            إذا (state != عدم) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                إذا (state->quote == quote && state->quote_ناپ == quote_ناپ) {
                    صحيح level = state->paren_level + state->replacement_ڈونگھائی - 1;
                    أكد(level >= 0 && level < tok->level);
                    أكد(tok->parenstack[level] == '{');
                    صحيح lineno = tok->parenlinenostack[level];
                    إذا (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, سطر, cursor_وتھ, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            Py_Lexer_String_اگیتر(state->قسم), lineno);
                    }
                    وإلا {
                        _PyTokenizer_syntaxerror_at(
                            tok, سطر, cursor_وتھ, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            Py_Lexer_String_اگیتر(state->قسم));
                    }
                    أرجع string_error_token(tok, token, tok->شروع, location);
                }
            }

            إذا (quote_ناپ == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, سطر, cursor_وتھ, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", سرا_lineno);
                إذا (c != '\n') {
                    tok->پورا = E_EOFS;
                }
                أرجع string_error_token(tok, token, tok->شروع, location);
            }
            وإلا {
                إذا (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, سطر, cursor_وتھ, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        سرا_lineno
                    );
                } وإلا {
                    _PyTokenizer_syntaxerror_at(
                        tok, سطر, cursor_وتھ, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", سرا_lineno
                    );
                }
                إذا (c != '\n') {
                    tok->پورا = E_EOLS;
                }
                أرجع string_error_token(tok, token, tok->شروع, location);
            }
        }
        إذا (c == quote) {
            سرا_quote_ناپ += 1;
        }
        وإلا {
            سرا_quote_ناپ = 0;
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
    p_سرا = tok->cur;
    أرجع MAKE_TOKEN(STRING);
}

صحيح
_PyLexer_get_ftstring(بنية tok_state *tok, ftstring_state *current, بنية token *token)
{
    أكد(current == _PyLexer_CurrentFTString(tok) && current->منوال != FTSTRING_منوال_عبارت);
    أكد((current->quote_ناپ == 1 || current->quote_ناپ == 3) &&
           current->replacement_ڈونگھائی <= MAX_اظہاریہ_NESTING);
    _PyTok_Off p_شروع = -1;
    _PyTok_Off p_سرا = -1;
    صحيح token_type;
    صحيح سرا_quote_ناپ = 0;
    صحيح unicode_escape = 0;
    صحيح quote = current->quote;
    صحيح quote_ناپ = current->quote_ناپ;
    صحيح وچ_format_spec = current->منوال == FTSTRING_منوال_FORMAT_SPEC;
    صحيح raw = _PyLexer_IsRawString(current->قسم);

    tok->شروع = tok->cur;
    tok->شروع_loc = (_PyTok_Loc){tok->lineno, Py_Lexer_Byte_کالم(tok)};

    طالما (سرا_quote_ناپ != quote_ناپ) {
        صحيح c = tok_nextc(tok);
        إذا (tok->پورا == E_ERROR || tok->پورا == E_DECODE) {
            أرجع MAKE_TOKEN(ERRORTOKEN);
        }

        إذا (c == EOF || (quote_ناپ == 1 && c == '\n')) {
            إذا (tok_failed(tok)) {
                أرجع MAKE_TOKEN(ERRORTOKEN);
            }
            إذا (c == EOF && tok->allow_incomplete_داخلہ) {
                أرجع incomplete_string_token(tok, token,
                    FTSTRING_درمیان(current), current->شروع, current->شروع_loc);
            }

            إذا (وچ_format_spec && c == '\n') {
                أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    Py_Lexer_String_اگیتر(current->قسم),
                    Py_Lexer_String_اگیتر(current->قسم)));
            }

            صحيح سرا_lineno = tok->lineno;
            _PyTok_Loc location = current->شروع_loc;
            ثابت حرف *سطر = Py_Tok_سروت_اشارہ_گر(
                &tok->سروت, current->شروع - location.byte_خانہ);
            Py_ssize_t cursor_وتھ = (Py_ssize_t)location.byte_خانہ + 1;

            إذا (quote_ناپ == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, سطر, cursor_وتھ, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    Py_Lexer_String_اگیتر(current->قسم), سرا_lineno);
                إذا (c != '\n') {
                    tok->پورا = E_EOFS;
                }
                أرجع string_error_token(tok, token,
                    current->شروع, location);
            }
            وإلا {
                _PyTokenizer_syntaxerror_at(
                    tok, سطر, cursor_وتھ, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    Py_Lexer_String_اگیتر(current->قسم), سرا_lineno);
                أرجع string_error_token(tok, token,
                    current->شروع, location);
            }
        }

        إذا (c == quote) {
            سرا_quote_ناپ += 1;
            استمر;
        } وإلا {
            سرا_quote_ناپ = 0;
        }

        إذا (c == '{') {
            صحيح peek = tok_nextc(tok);
            إذا (peek != '{' || وچ_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off اظہاریہ_شروع = tok->cur;
                tok_backup(tok, c);
                إذا (begin_ftstring_اظہاریہ(tok, current, اظہاریہ_شروع) < 0) {
                    أرجع MAKE_TOKEN(ERRORTOKEN);
                }
                p_شروع = tok->شروع;
                p_سرا = tok->cur;
                إذا (p_شروع == p_سرا) {
                    أرجع Py_Lexer_get_عام(tok, current, token);
                }
            } وإلا {
                p_شروع = tok->شروع;
                p_سرا = tok->cur - 1;
            }
            اذهب emit_درمیان;
        } وإلا إذا (c == '}') {
            إذا (unicode_escape) {
                p_شروع = tok->شروع;
                p_سرا = tok->cur;
                اذهب emit_درمیان;
            }
            صحيح peek = tok_nextc(tok);

            صحيح bracket_ڈونگھائی = Py_Lexer_FTString_Bracket_ڈونگھائی(tok, current);
            إذا (peek == '}' && !وچ_format_spec && bracket_ڈونگھائی == 0) {
                p_شروع = tok->شروع;
                p_سرا = tok->cur - 1;
            }
            وإلا {
                tok_backup(tok, peek);
                إذا (!وچ_format_spec && bracket_ڈونگھائی == 0) {
                    إذا (tok->شروع == tok->cur - 1) {
                        أرجع MAKE_TOKEN(_PyTokenizer_syntaxerror(
                            tok, "%c-string: single '}' is not allowed",
                            Py_Lexer_String_اگیتر(current->قسم)));
                    }
                    tok_backup(tok, c);
                    p_شروع = tok->شروع;
                    p_سرا = tok->cur;
                    اذهب emit_درمیان;
                }
                tok_backup(tok, c);
                current->منوال = FTSTRING_منوال_عبارت;
                p_شروع = tok->شروع;
                p_سرا = tok->cur;
            }
            اذهب emit_درمیان;
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
    p_سرا = tok->cur;
    إذا (p_سرا - quote_ناپ == p_شروع) {
        صحيح سرا_token = FTSTRING_سرا(current);
        Py_Lexer_نکالو_FTString(tok);
        أرجع MAKE_TOKEN(سرا_token);
    }
    لكل (صحيح i = 0; i < quote_ناپ; i++) {
        tok_backup(tok, quote);
    }
    p_سرا = tok->cur;
emit_درمیان:
    token_type = MAKE_TOKEN(FTSTRING_درمیان(current));
    token->is_raw = raw;
    أرجع token_type;
}
