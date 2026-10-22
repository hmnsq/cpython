#ਅਨ੍ਤਰ੍ਭਾਵ "Python.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_token.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errcode.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "lexer_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "../tokenizer/helpers.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAKE_TOKEN(token_type) Py_Lexer_token_ਤਿਆਰੀ(tok, token, token_type, p_ਸ਼ੁਰੂ, p_ਸਿਰਾ)

ਸਥਿਰ ਪੂਰਨ
string_error_token(ਬਣਤਰ tok_state *tok, ਬਣਤਰ token *token,
                   _PyTok_Off ਸ਼ੁਰੂ, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_ਖਾਨਾ + 1},
        .ਪਾਠ_ਵਿਸਤਾਰ = {ਸ਼ੁਰੂ - location.byte_ਖਾਨਾ, tok->inp},
    };
    ਪੂਰਨ type = Py_Lexer_token_ਤਿਆਰੀ(tok, token, ERRORTOKEN, -1, -1);
    token->ਸ਼ੁਰੂ_loc = location;
    token->ਸਿਰਾ_loc = (_PyTok_Loc){location.lineno, -1};
    ਵਾਪਸ type;
}

ਸਥਿਰ ਪੂਰਨ
incomplete_string_token(ਬਣਤਰ tok_state *tok, ਬਣਤਰ token *token,
                        ਪੂਰਨ type, _PyTok_Off ਸ਼ੁਰੂ, _PyTok_Loc location)
{
    string_error_token(tok, token, ਸ਼ੁਰੂ, location);
    tok->ਪੂਰਾ = E_INCOMPLETE;
    ਵਾਪਸ type;
}

ਪੂਰਨ
Py_Lexer_ਰਿਕਾਰਡ_ftstring_comment(ਬਣਤਰ tok_state *tok, ftstring_state *state,
                                 _PyTok_Off ਸ਼ੁਰੂ, _PyTok_Off ਸਿਰਾ)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(state == _PyLexer_CurrentFTString(tok) && state->ਬਹੁਲਕ == FTSTRING_ਬਹੁਲਕ_ਵਿਅੰਜਕ);
    ਜੇ (state->ਵਿਅੰਜਕ_ਵਿਸਤਾਰ.ਸਿਰਾ >= 0) {
        ਵਾਪਸ 0;
    }
    ਪ੍ਰਤਿਜ੍ਞਾ(state->ਵਿਅੰਜਕ_ਵਿਸਤਾਰ.ਸ਼ੁਰੂ >= 0);
    tokenizer_comments *comments = state->comments;
    ਜੇ (comments == ਨਿਰਾ || comments->ਗਿਣਤੀ_2 == comments->ਸਮਰੱਥਾ) {
        Py_ssize_t max_ਸਮਰੱਥਾ = (PY_SSIZE_T_MAX -
            (Py_ssize_t)ਆਕਾਰ(*comments)) /
            (Py_ssize_t)ਆਕਾਰ(*comments->spans);
        ਜੇ (comments != ਨਿਰਾ && comments->ਸਮਰੱਥਾ > max_ਸਮਰੱਥਾ / 2) {
            Py_Err_No_ਸਮ੍ਰਿਤੀ();
            ਵਾਪਸ -1;
        }
        Py_ssize_t ਸਮਰੱਥਾ = comments == ਨਿਰਾ ? 4 : comments->ਸਮਰੱਥਾ * 2;
        ਮਾਪ_t ਮਾਪ = ਆਕਾਰ(*comments) +
            (ਮਾਪ_t)ਸਮਰੱਥਾ * ਆਕਾਰ(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, ਮਾਪ);
        ਜੇ (resized == ਨਿਰਾ) {
            Py_Err_No_ਸਮ੍ਰਿਤੀ();
            ਵਾਪਸ -1;
        }
        comments = resized;
        ਜੇ (state->comments == ਨਿਰਾ) {
            comments->ਗਿਣਤੀ_2 = 0;
        }
        comments->ਸਮਰੱਥਾ = ਸਮਰੱਥਾ;
        state->comments = comments;
    }
    comments->spans[comments->ਗਿਣਤੀ_2++] =
        (Py_Tok_ਵਿਸਤਾਰ){ਸ਼ੁਰੂ, ਸਿਰਾ};
    ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ
ਅੰਤ_ftstring_ਵਿਅੰਜਕ(ਬਣਤਰ tok_state *tok, ftstring_state *state,
                     ਬਣਤਰ token *token)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(token != ਨਿਰਾ && state == _PyLexer_CurrentFTString(tok));
    ਪ੍ਰਤਿਜ੍ਞਾ(state->ਬਹੁਲਕ == FTSTRING_ਬਹੁਲਕ_ਵਿਅੰਜਕ && tok->ਸ਼ੁਰੂ >= 0);

    ਜੇ (state->ਵਿਅੰਜਕ_ਵਿਸਤਾਰ.ਸਿਰਾ >= 0) {
        ਵਾਪਸ 0;
    }
    ਪ੍ਰਤਿਜ੍ਞਾ(state->ਵਿਅੰਜਕ_ਵਿਸਤਾਰ.ਸ਼ੁਰੂ >= 0);
    state->ਵਿਅੰਜਕ_ਵਿਸਤਾਰ.ਸਿਰਾ = tok->ਸ਼ੁਰੂ;
    ਪੂਰਨ tstring_interpolation = _PyLexer_IsTString(state->ਕਿਸਮ) &&
        state->replacement_ਡੂੰਘਾਈ == 1;
    ਜੇ (!(state->debug_ਵਿਅੰਜਕ || tstring_interpolation) || token->metadata) {
        ਵਾਪਸ 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    ਜੇ (comments != ਨਿਰਾ && comments->ਗਿਣਤੀ_2 > 0) {
        Py_ssize_t stripped_ਮਾਪ =
            state->ਵਿਅੰਜਕ_ਵਿਸਤਾਰ.ਸਿਰਾ - state->ਵਿਅੰਜਕ_ਵਿਸਤਾਰ.ਸ਼ੁਰੂ;
        Py_ssize_t comment_ਗਿਣਤੀ = 0;
        ਲਈ (Py_ssize_t i = 0; i < comments->ਗਿਣਤੀ_2; i++) {
            Py_Tok_ਵਿਸਤਾਰ comment = comments->spans[i];
            ਪ੍ਰਤਿਜ੍ਞਾ(Py_Tok_ਵਿਸਤਾਰ_Is_Valid(comment));
            ਪ੍ਰਤਿਜ੍ਞਾ(comment.ਸ਼ੁਰੂ >= (i == 0 ? state->ਵਿਅੰਜਕ_ਵਿਸਤਾਰ.ਸ਼ੁਰੂ
                                           : comments->spans[i - 1].ਸਿਰਾ));
            ਜੇ (comment.ਸ਼ੁਰੂ >= state->ਵਿਅੰਜਕ_ਵਿਸਤਾਰ.ਸਿਰਾ) {
                ਰੋਕੋ;
            }
            ਪ੍ਰਤਿਜ੍ਞਾ(comment.ਸਿਰਾ <= state->ਵਿਅੰਜਕ_ਵਿਸਤਾਰ.ਸਿਰਾ);
            stripped_ਮਾਪ -= comment.ਸਿਰਾ - comment.ਸ਼ੁਰੂ;
            comment_ਗਿਣਤੀ++;
        }
        ਅੱਖਰ *stripped = PyMem_Malloc((ਮਾਪ_t)stripped_ਮਾਪ);
        ਜੇ (stripped == ਨਿਰਾ) {
            Py_Err_No_ਸਮ੍ਰਿਤੀ();
            ਵਾਪਸ -1;
        }
        Py_Tok_ਵਿਸਤਾਰ kept = {state->ਵਿਅੰਜਕ_ਵਿਸਤਾਰ.ਸ਼ੁਰੂ, state->ਵਿਅੰਜਕ_ਵਿਸਤਾਰ.ਸ਼ੁਰੂ};
        Py_ssize_t stripped_len = 0;
        ਲਈ (Py_ssize_t i = 0; i < comment_ਗਿਣਤੀ; i++) {
            kept.ਸਿਰਾ = comments->spans[i].ਸ਼ੁਰੂ;
            Py_ssize_t ਲੰਮਾਈ;
            ਅਚਲ ਅੱਖਰ *ਪਾਠ = Py_Tok_ਸਰੋਤ_ਵਿਸਤਾਰ_View(
                &tok->ਸਰੋਤ, kept, &ਲੰਮਾਈ);
            ਸਮ੍ਰਿਤੀ_ਨਕਲ(stripped + stripped_len, ਪਾਠ, (ਮਾਪ_t)ਲੰਮਾਈ);
            stripped_len += ਲੰਮਾਈ;
            kept.ਸ਼ੁਰੂ = comments->spans[i].ਸਿਰਾ;
        }
        kept.ਸਿਰਾ = state->ਵਿਅੰਜਕ_ਵਿਸਤਾਰ.ਸਿਰਾ;
        Py_ssize_t ਲੰਮਾਈ;
        ਅਚਲ ਅੱਖਰ *ਪਾਠ = Py_Tok_ਸਰੋਤ_ਵਿਸਤਾਰ_View(&tok->ਸਰੋਤ, kept, &ਲੰਮਾਈ);
        ਸਮ੍ਰਿਤੀ_ਨਕਲ(stripped + stripped_len, ਪਾਠ, (ਮਾਪ_t)ਲੰਮਾਈ);
        res = PyUnicode_DecodeUTF8(stripped, stripped_ਮਾਪ, ਨਿਰਾ);
        PyMem_Free(stripped);
    }
    ਨਹੀਂਤਾਂ {
        Py_ssize_t ਵਿਅੰਜਕ_len;
        ਅਚਲ ਅੱਖਰ *ਵਿਅੰਜਕ = Py_Tok_ਸਰੋਤ_ਵਿਸਤਾਰ_View(
            &tok->ਸਰੋਤ, state->ਵਿਅੰਜਕ_ਵਿਸਤਾਰ, &ਵਿਅੰਜਕ_len);
        res = PyUnicode_DecodeUTF8(ਵਿਅੰਜਕ, ਵਿਅੰਜਕ_len, ਨਿਰਾ);
    }

    ਜੇ (!res) {
        ਵਾਪਸ -1;
    }
    token->metadata = res;
    ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ
begin_ftstring_ਵਿਅੰਜਕ(ਬਣਤਰ tok_state *tok, ftstring_state *state,
                    _PyTok_Off ਵਿਅੰਜਕ_ਸ਼ੁਰੂ)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(state->ਬਹੁਲਕ != FTSTRING_ਬਹੁਲਕ_ਵਿਅੰਜਕ);
    state->ਵਿਅੰਜਕ_ਵਿਸਤਾਰ = (Py_Tok_ਵਿਸਤਾਰ){ਵਿਅੰਜਕ_ਸ਼ੁਰੂ, -1};
    ਜੇ (state->comments != ਨਿਰਾ) {
        state->comments->ਗਿਣਤੀ_2 = 0;
    }
    ਜੇ (state->replacement_ਡੂੰਘਾਈ >= MAX_ਵਿਅੰਜਕ_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            Py_Lexer_String_ਅਗੇਤਰ(state->ਕਿਸਮ));
        ਵਾਪਸ -1;
    }
    state->replacement_ਡੂੰਘਾਈ++;
    state->ਬਹੁਲਕ = FTSTRING_ਬਹੁਲਕ_ਵਿਅੰਜਕ;
    state->debug_ਵਿਅੰਜਕ = 0;
    ਵਾਪਸ 0;
}

ਪੂਰਨ
_PyLexer_ftstring_punctuation(ਬਣਤਰ tok_state *tok, ftstring_state *state,
                              ਬਣਤਰ token *token, ਪੂਰਨ c)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(state->ਬਹੁਲਕ == FTSTRING_ਬਹੁਲਕ_ਵਿਅੰਜਕ);
    ਪ੍ਰਤਿਜ੍ਞਾ(c == ':' || c == '}' || c == '!');
    ਜੇ (Py_Lexer_FTString_Bracket_ਡੂੰਘਾਈ(tok, state) != state->replacement_ਡੂੰਘਾਈ) {
        ਵਾਪਸ 0;
    }
    ਜੇ (c == '!') {
        ਪੂਰਨ ਅਗਲਾ = tok_nextc(tok);
        tok_backup(tok, ਅਗਲਾ);
        ਜੇ (ਅਗਲਾ == '=') {
            ਵਾਪਸ 0;
        }
    }
    ਜੇ (ਅੰਤ_ftstring_ਵਿਅੰਜਕ(tok, state, token) < 0) {
        ਵਾਪਸ -1;
    }
    ਜੇ (c == ':') {
        state->ਬਹੁਲਕ = FTSTRING_ਬਹੁਲਕ_FORMAT_SPEC;
        ਵਾਪਸ COLON;
    }
    ਵਾਪਸ 0;
}

ਪੂਰਨ
Py_Lexer_close_ftstring_ਵਿਅੰਜਕ(ਬਣਤਰ tok_state *tok, ftstring_state *state,
                             ਪੂਰਨ c)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(state->ਬਹੁਲਕ == FTSTRING_ਬਹੁਲਕ_ਵਿਅੰਜਕ);
    ਪ੍ਰਤਿਜ੍ਞਾ(c == ')' || c == ']' || c == '}');
    ਪੂਰਨ ਡੂੰਘਾਈ = Py_Lexer_FTString_Bracket_ਡੂੰਘਾਈ(tok, state);
    ਜੇ (ਡੂੰਘਾਈ < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                Py_Lexer_String_ਅਗੇਤਰ(state->ਕਿਸਮ), c);
        ਵਾਪਸ -1;
    }
    ਜੇ (c == '}' && ਡੂੰਘਾਈ == state->replacement_ਡੂੰਘਾਈ - 1) {
        state->replacement_ਡੂੰਘਾਈ--;
        state->ਬਹੁਲਕ = FTSTRING_ਬਹੁਲਕ_ਵਿਚਕਾਰ;
        state->debug_ਵਿਅੰਜਕ = 0;
    }
    ਵਾਪਸ 0;
}

ਖਾਲੀ
_PyLexer_mark_ftstring_debug(ਬਣਤਰ tok_state *tok, ftstring_state *state)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(state->ਬਹੁਲਕ == FTSTRING_ਬਹੁਲਕ_ਵਿਅੰਜਕ);
    ਜੇ (Py_Lexer_FTString_Bracket_ਡੂੰਘਾਈ(tok, state) == state->replacement_ਡੂੰਘਾਈ) {
        state->debug_ਵਿਅੰਜਕ = 1;
    }
}

ਪੂਰਨ
_PyLexer_check_string_prefixes(ਬਣਤਰ tok_state *tok,
                                             ਪੂਰਨ saw_b, ਪੂਰਨ saw_r, ਪੂਰਨ saw_u,
                                             ਪੂਰਨ saw_f, ਪੂਰਨ saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    ਕਰੋ {                                                                  \
        (ਖਾਲੀ)Py_Tokenizer_syntaxerror_ਗਿਆਤ_range(                       \
            tok, (ਪੂਰਨ)(tok->ਸ਼ੁਰੂ + 1 - tok->ਸਤਰ_ਸ਼ੁਰੂ),                 \
            (ਪੂਰਨ)(tok->cur - tok->ਸਤਰ_ਸ਼ੁਰੂ),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        ਵਾਪਸ -1;                                                        \
    } ਜਦੋਂ (0)

    ਜੇ (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    ਜੇ (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    ਜੇ (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    ਜੇ (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    ਜੇ (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    ਜੇ (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    ਜੇ (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ RETURN_SYNTAX_ERROR

    ਵਾਪਸ 0;
}

ਪੂਰਨ
Py_Lexer_ਜਾਂਚ_fstring_ਸ਼ੁਰੂ(ਬਣਤਰ tok_state *tok, ਬਣਤਰ token *token,
                            ਪੂਰਨ c, ftstring_ਕਿਸਮ ਕਿਸਮ)
{
    _PyTok_Off p_ਸ਼ੁਰੂ = -1;
    _PyTok_Off p_ਸਿਰਾ = -1;

    ਪੂਰਨ quote = c;
    ਪੂਰਨ quote_ਮਾਪ = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    ਪੂਰਨ after_quote = tok_nextc(tok);
    ਜੇ (after_quote == quote) {
        ਪੂਰਨ after_after_quote = tok_nextc(tok);
        ਜੇ (after_after_quote == quote) {
            quote_ਮਾਪ = 3;
        }
        ਨਹੀਂਤਾਂ {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    ਜੇ (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_ਸ਼ੁਰੂ = tok->ਸ਼ੁਰੂ;
    p_ਸਿਰਾ = tok->cur;
    ftstring_state *state = Py_Lexer_ਧੱਕੋ_FTString(tok);
    ਜੇ (state == ਨਿਰਾ) {
        ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
    }
    state->ਬਹੁਲਕ = FTSTRING_ਬਹੁਲਕ_ਵਿਚਕਾਰ;
    state->quote = quote;
    state->quote_ਮਾਪ = quote_ਮਾਪ;
    state->paren_level = tok->level;
    state->ਸ਼ੁਰੂ = tok->ਸ਼ੁਰੂ;
    state->ਸ਼ੁਰੂ_loc = tok->ਸ਼ੁਰੂ_loc;
    state->ਵਿਅੰਜਕ_ਵਿਸਤਾਰ = (Py_Tok_ਵਿਸਤਾਰ){-1, -1};

    state->ਕਿਸਮ = ਕਿਸਮ;
    ਵਾਪਸ _PyLexer_IsTString(ਕਿਸਮ)
        ? MAKE_TOKEN(TSTRING_ਸ਼ੁਰੂ) : MAKE_TOKEN(FSTRING_ਸ਼ੁਰੂ);
}

ਪੂਰਨ
Py_Lexer_ਜਾਂਚ_string(ਬਣਤਰ tok_state *tok, ਬਣਤਰ token *token, ਪੂਰਨ c)
{
    _PyTok_Off p_ਸ਼ੁਰੂ = -1;
    _PyTok_Off p_ਸਿਰਾ = -1;

    ਪੂਰਨ quote = c;
    ਪੂਰਨ quote_ਮਾਪ = 1;             /* 1 or 3 */
    ਪੂਰਨ ਸਿਰਾ_quote_ਮਾਪ = 0;
    ਪੂਰਨ has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    ਜੇ (c == quote) {
        c = tok_nextc(tok);
        ਜੇ (c == quote) {
            quote_ਮਾਪ = 3;
        }
        ਨਹੀਂਤਾਂ {
            ਸਿਰਾ_quote_ਮਾਪ = 1;     /* empty string found */
        }
    }
    ਜੇ (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    ਜਦੋਂ (ਸਿਰਾ_quote_ਮਾਪ != quote_ਮਾਪ) {
        c = tok_nextc(tok);
        ਜੇ (tok->ਪੂਰਾ == E_ERROR) {
            ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
        }
        ਜੇ (tok->ਪੂਰਾ == E_DECODE) {
            ਰੋਕੋ;
        }
        ਜੇ (c == EOF || (quote_ਮਾਪ == 1 && c == '\n')) {
            ਜੇ (tok_failed(tok)) {
                ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
            }
            ਜੇ (c == EOF && tok->allow_incomplete_ਨਿਵੇਸ਼) {
                ਵਾਪਸ incomplete_string_token(tok, token, STRING,
                                               tok->ਸ਼ੁਰੂ, tok->ਸ਼ੁਰੂ_loc);
            }
            ਪੂਰਨ ਸਿਰਾ_lineno = tok->lineno;
            _PyTok_Loc location = tok->ਸ਼ੁਰੂ_loc;
            ਅਚਲ ਅੱਖਰ *ਸਤਰ = Py_Tok_ਸਰੋਤ_ਸੂਚਕ(
                &tok->ਸਰੋਤ, tok->ਸ਼ੁਰੂ - location.byte_ਖਾਨਾ);
            Py_ssize_t cursor_ਵਿੱਥ = (Py_ssize_t)location.byte_ਖਾਨਾ + 1;

            ਅਚਲ ftstring_state *state = _PyLexer_CurrentFTString(tok);
            ਜੇ (state != ਨਿਰਾ) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                ਜੇ (state->quote == quote && state->quote_ਮਾਪ == quote_ਮਾਪ) {
                    ਪੂਰਨ level = state->paren_level + state->replacement_ਡੂੰਘਾਈ - 1;
                    ਪ੍ਰਤਿਜ੍ਞਾ(level >= 0 && level < tok->level);
                    ਪ੍ਰਤਿਜ੍ਞਾ(tok->parenstack[level] == '{');
                    ਪੂਰਨ lineno = tok->parenlinenostack[level];
                    ਜੇ (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, ਸਤਰ, cursor_ਵਿੱਥ, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            Py_Lexer_String_ਅਗੇਤਰ(state->ਕਿਸਮ), lineno);
                    }
                    ਨਹੀਂਤਾਂ {
                        _PyTokenizer_syntaxerror_at(
                            tok, ਸਤਰ, cursor_ਵਿੱਥ, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            Py_Lexer_String_ਅਗੇਤਰ(state->ਕਿਸਮ));
                    }
                    ਵਾਪਸ string_error_token(tok, token, tok->ਸ਼ੁਰੂ, location);
                }
            }

            ਜੇ (quote_ਮਾਪ == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, ਸਤਰ, cursor_ਵਿੱਥ, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", ਸਿਰਾ_lineno);
                ਜੇ (c != '\n') {
                    tok->ਪੂਰਾ = E_EOFS;
                }
                ਵਾਪਸ string_error_token(tok, token, tok->ਸ਼ੁਰੂ, location);
            }
            ਨਹੀਂਤਾਂ {
                ਜੇ (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, ਸਤਰ, cursor_ਵਿੱਥ, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        ਸਿਰਾ_lineno
                    );
                } ਨਹੀਂਤਾਂ {
                    _PyTokenizer_syntaxerror_at(
                        tok, ਸਤਰ, cursor_ਵਿੱਥ, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", ਸਿਰਾ_lineno
                    );
                }
                ਜੇ (c != '\n') {
                    tok->ਪੂਰਾ = E_EOLS;
                }
                ਵਾਪਸ string_error_token(tok, token, tok->ਸ਼ੁਰੂ, location);
            }
        }
        ਜੇ (c == quote) {
            ਸਿਰਾ_quote_ਮਾਪ += 1;
        }
        ਨਹੀਂਤਾਂ {
            ਸਿਰਾ_quote_ਮਾਪ = 0;
            ਜੇ (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                ਜੇ (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                ਜੇ (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_ਸ਼ੁਰੂ = tok->ਸ਼ੁਰੂ;
    p_ਸਿਰਾ = tok->cur;
    ਵਾਪਸ MAKE_TOKEN(STRING);
}

ਪੂਰਨ
_PyLexer_get_ftstring(ਬਣਤਰ tok_state *tok, ftstring_state *current, ਬਣਤਰ token *token)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(current == _PyLexer_CurrentFTString(tok) && current->ਬਹੁਲਕ != FTSTRING_ਬਹੁਲਕ_ਵਿਅੰਜਕ);
    ਪ੍ਰਤਿਜ੍ਞਾ((current->quote_ਮਾਪ == 1 || current->quote_ਮਾਪ == 3) &&
           current->replacement_ਡੂੰਘਾਈ <= MAX_ਵਿਅੰਜਕ_NESTING);
    _PyTok_Off p_ਸ਼ੁਰੂ = -1;
    _PyTok_Off p_ਸਿਰਾ = -1;
    ਪੂਰਨ token_type;
    ਪੂਰਨ ਸਿਰਾ_quote_ਮਾਪ = 0;
    ਪੂਰਨ unicode_escape = 0;
    ਪੂਰਨ quote = current->quote;
    ਪੂਰਨ quote_ਮਾਪ = current->quote_ਮਾਪ;
    ਪੂਰਨ ਵਿੱਚ_format_spec = current->ਬਹੁਲਕ == FTSTRING_ਬਹੁਲਕ_FORMAT_SPEC;
    ਪੂਰਨ raw = _PyLexer_IsRawString(current->ਕਿਸਮ);

    tok->ਸ਼ੁਰੂ = tok->cur;
    tok->ਸ਼ੁਰੂ_loc = (_PyTok_Loc){tok->lineno, Py_Lexer_Byte_ਕਾਲਮ(tok)};

    ਜਦੋਂ (ਸਿਰਾ_quote_ਮਾਪ != quote_ਮਾਪ) {
        ਪੂਰਨ c = tok_nextc(tok);
        ਜੇ (tok->ਪੂਰਾ == E_ERROR || tok->ਪੂਰਾ == E_DECODE) {
            ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
        }

        ਜੇ (c == EOF || (quote_ਮਾਪ == 1 && c == '\n')) {
            ਜੇ (tok_failed(tok)) {
                ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
            }
            ਜੇ (c == EOF && tok->allow_incomplete_ਨਿਵੇਸ਼) {
                ਵਾਪਸ incomplete_string_token(tok, token,
                    FTSTRING_ਵਿਚਕਾਰ(current), current->ਸ਼ੁਰੂ, current->ਸ਼ੁਰੂ_loc);
            }

            ਜੇ (ਵਿੱਚ_format_spec && c == '\n') {
                ਵਾਪਸ MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    Py_Lexer_String_ਅਗੇਤਰ(current->ਕਿਸਮ),
                    Py_Lexer_String_ਅਗੇਤਰ(current->ਕਿਸਮ)));
            }

            ਪੂਰਨ ਸਿਰਾ_lineno = tok->lineno;
            _PyTok_Loc location = current->ਸ਼ੁਰੂ_loc;
            ਅਚਲ ਅੱਖਰ *ਸਤਰ = Py_Tok_ਸਰੋਤ_ਸੂਚਕ(
                &tok->ਸਰੋਤ, current->ਸ਼ੁਰੂ - location.byte_ਖਾਨਾ);
            Py_ssize_t cursor_ਵਿੱਥ = (Py_ssize_t)location.byte_ਖਾਨਾ + 1;

            ਜੇ (quote_ਮਾਪ == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, ਸਤਰ, cursor_ਵਿੱਥ, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    Py_Lexer_String_ਅਗੇਤਰ(current->ਕਿਸਮ), ਸਿਰਾ_lineno);
                ਜੇ (c != '\n') {
                    tok->ਪੂਰਾ = E_EOFS;
                }
                ਵਾਪਸ string_error_token(tok, token,
                    current->ਸ਼ੁਰੂ, location);
            }
            ਨਹੀਂਤਾਂ {
                _PyTokenizer_syntaxerror_at(
                    tok, ਸਤਰ, cursor_ਵਿੱਥ, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    Py_Lexer_String_ਅਗੇਤਰ(current->ਕਿਸਮ), ਸਿਰਾ_lineno);
                ਵਾਪਸ string_error_token(tok, token,
                    current->ਸ਼ੁਰੂ, location);
            }
        }

        ਜੇ (c == quote) {
            ਸਿਰਾ_quote_ਮਾਪ += 1;
            ਜਾਰੀ;
        } ਨਹੀਂਤਾਂ {
            ਸਿਰਾ_quote_ਮਾਪ = 0;
        }

        ਜੇ (c == '{') {
            ਪੂਰਨ peek = tok_nextc(tok);
            ਜੇ (peek != '{' || ਵਿੱਚ_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off ਵਿਅੰਜਕ_ਸ਼ੁਰੂ = tok->cur;
                tok_backup(tok, c);
                ਜੇ (begin_ftstring_ਵਿਅੰਜਕ(tok, current, ਵਿਅੰਜਕ_ਸ਼ੁਰੂ) < 0) {
                    ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
                }
                p_ਸ਼ੁਰੂ = tok->ਸ਼ੁਰੂ;
                p_ਸਿਰਾ = tok->cur;
                ਜੇ (p_ਸ਼ੁਰੂ == p_ਸਿਰਾ) {
                    ਵਾਪਸ Py_Lexer_get_ਸਾਧਾਰਨ(tok, current, token);
                }
            } ਨਹੀਂਤਾਂ {
                p_ਸ਼ੁਰੂ = tok->ਸ਼ੁਰੂ;
                p_ਸਿਰਾ = tok->cur - 1;
            }
            ਜਾਓ emit_ਵਿਚਕਾਰ;
        } ਨਹੀਂਤਾਂ ਜੇ (c == '}') {
            ਜੇ (unicode_escape) {
                p_ਸ਼ੁਰੂ = tok->ਸ਼ੁਰੂ;
                p_ਸਿਰਾ = tok->cur;
                ਜਾਓ emit_ਵਿਚਕਾਰ;
            }
            ਪੂਰਨ peek = tok_nextc(tok);

            ਪੂਰਨ bracket_ਡੂੰਘਾਈ = Py_Lexer_FTString_Bracket_ਡੂੰਘਾਈ(tok, current);
            ਜੇ (peek == '}' && !ਵਿੱਚ_format_spec && bracket_ਡੂੰਘਾਈ == 0) {
                p_ਸ਼ੁਰੂ = tok->ਸ਼ੁਰੂ;
                p_ਸਿਰਾ = tok->cur - 1;
            }
            ਨਹੀਂਤਾਂ {
                tok_backup(tok, peek);
                ਜੇ (!ਵਿੱਚ_format_spec && bracket_ਡੂੰਘਾਈ == 0) {
                    ਜੇ (tok->ਸ਼ੁਰੂ == tok->cur - 1) {
                        ਵਾਪਸ MAKE_TOKEN(_PyTokenizer_syntaxerror(
                            tok, "%c-string: single '}' is not allowed",
                            Py_Lexer_String_ਅਗੇਤਰ(current->ਕਿਸਮ)));
                    }
                    tok_backup(tok, c);
                    p_ਸ਼ੁਰੂ = tok->ਸ਼ੁਰੂ;
                    p_ਸਿਰਾ = tok->cur;
                    ਜਾਓ emit_ਵਿਚਕਾਰ;
                }
                tok_backup(tok, c);
                current->ਬਹੁਲਕ = FTSTRING_ਬਹੁਲਕ_ਵਿਅੰਜਕ;
                p_ਸ਼ੁਰੂ = tok->ਸ਼ੁਰੂ;
                p_ਸਿਰਾ = tok->cur;
            }
            ਜਾਓ emit_ਵਿਚਕਾਰ;
        } ਨਹੀਂਤਾਂ ਜੇ (c == '\\') {
            ਪੂਰਨ peek = tok_nextc(tok);
            ਜੇ (peek == '\r') {
                peek = tok_nextc(tok);
            }
            ਜੇ (peek == '{' || peek == '}') {
                ਜੇ (!raw) {
                    ਜੇ (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        ਵਾਪਸ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                ਜਾਰੀ;
            }

            ਜੇ (!raw) {
                ਜੇ (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    ਜੇ (peek == '{') {
                        unicode_escape = 1;
                    } ਨਹੀਂਤਾਂ {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_ਸ਼ੁਰੂ = tok->ਸ਼ੁਰੂ;
    p_ਸਿਰਾ = tok->cur;
    ਜੇ (p_ਸਿਰਾ - quote_ਮਾਪ == p_ਸ਼ੁਰੂ) {
        ਪੂਰਨ ਸਿਰਾ_token = FTSTRING_ਸਿਰਾ(current);
        Py_Lexer_ਕੱਢੋ_FTString(tok);
        ਵਾਪਸ MAKE_TOKEN(ਸਿਰਾ_token);
    }
    ਲਈ (ਪੂਰਨ i = 0; i < quote_ਮਾਪ; i++) {
        tok_backup(tok, quote);
    }
    p_ਸਿਰਾ = tok->cur;
emit_ਵਿਚਕਾਰ:
    token_type = MAKE_TOKEN(FTSTRING_ਵਿਚਕਾਰ(current));
    token->is_raw = raw;
    ਵਾਪਸ token_type;
}
