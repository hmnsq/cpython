#ಅನ್ತರ್ಭಾವ "Python.h"
#ಅನ್ತರ್ಭಾವ "pycore_token.h"
#ಅನ್ತರ್ಭಾವ "errcode.h"

#ಅನ್ತರ್ಭಾವ "lexer_internal.h"
#ಅನ್ತರ್ಭಾವ "../tokenizer/helpers.h"

#ವ್ಯಾಖ್ಯಾನಿಸು MAKE_TOKEN(token_type) Py_Lexer_token_ಸಿದ್ಧತೆ(tok, token, token_type, p_ಆರಂಭ, p_ಕೊನೆ)

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
string_error_token(ರಚನೆ tok_state *tok, ರಚನೆ token *token,
                   _PyTok_Off ಆರಂಭ, _PyTok_Loc location)
{
    tok->diagnostic = (_PyTokenizer_Diagnostic){
        .location = {location.lineno, location.byte_ಲಂಬಸಾಲು + 1},
        .ಪಠ್ಯ_ಹರವು = {ಆರಂಭ - location.byte_ಲಂಬಸಾಲು, tok->inp},
    };
    ಪೂರ್ಣಾಂಕ type = Py_Lexer_token_ಸಿದ್ಧತೆ(tok, token, ERRORTOKEN, -1, -1);
    token->ಆರಂಭ_loc = location;
    token->ಕೊನೆ_loc = (_PyTok_Loc){location.lineno, -1};
    ಹಿಂತಿರುಗಿ type;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
incomplete_string_token(ರಚನೆ tok_state *tok, ರಚನೆ token *token,
                        ಪೂರ್ಣಾಂಕ type, _PyTok_Off ಆರಂಭ, _PyTok_Loc location)
{
    string_error_token(tok, token, ಆರಂಭ, location);
    tok->ಮುಗಿದ = E_INCOMPLETE;
    ಹಿಂತಿರುಗಿ type;
}

ಪೂರ್ಣಾಂಕ
Py_Lexer_ದಾಖಲೆ_ftstring_comment(ರಚನೆ tok_state *tok, ftstring_state *state,
                                 _PyTok_Off ಆರಂಭ, _PyTok_Off ಕೊನೆ)
{
    ಪ್ರತಿಜ್ಞಾ(state == _PyLexer_CurrentFTString(tok) && state->ಬಹುಲಕ == FTSTRING_ಬಹುಲಕ_ಅಭಿವ್ಯಕ್ತಿ);
    ಆದರೆ (state->ಅಭಿವ್ಯಕ್ತಿ_ಹರವು.ಕೊನೆ >= 0) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಪ್ರತಿಜ್ಞಾ(state->ಅಭಿವ್ಯಕ್ತಿ_ಹರವು.ಆರಂಭ >= 0);
    tokenizer_comments *comments = state->comments;
    ಆದರೆ (comments == ಖಾಲಿ || comments->ಎಣಿಕೆ_2 == comments->ಸಾಮರ್ಥ್ಯ) {
        Py_ssize_t max_ಸಾಮರ್ಥ್ಯ = (PY_SSIZE_T_MAX -
            (Py_ssize_t)ಗಾತ್ರ(*comments)) /
            (Py_ssize_t)ಗಾತ್ರ(*comments->spans);
        ಆದರೆ (comments != ಖಾಲಿ && comments->ಸಾಮರ್ಥ್ಯ > max_ಸಾಮರ್ಥ್ಯ / 2) {
            Py_Err_No_ಸ್ಮರಣೆ();
            ಹಿಂತಿರುಗಿ -1;
        }
        Py_ssize_t ಸಾಮರ್ಥ್ಯ = comments == ಖಾಲಿ ? 4 : comments->ಸಾಮರ್ಥ್ಯ * 2;
        ಗಾತ್ರ_t ಗಾತ್ರ_2 = ಗಾತ್ರ(*comments) +
            (ಗಾತ್ರ_t)ಸಾಮರ್ಥ್ಯ * ಗಾತ್ರ(*comments->spans);
        tokenizer_comments *resized = PyMem_Realloc(comments, ಗಾತ್ರ_2);
        ಆದರೆ (resized == ಖಾಲಿ) {
            Py_Err_No_ಸ್ಮರಣೆ();
            ಹಿಂತಿರುಗಿ -1;
        }
        comments = resized;
        ಆದರೆ (state->comments == ಖಾಲಿ) {
            comments->ಎಣಿಕೆ_2 = 0;
        }
        comments->ಸಾಮರ್ಥ್ಯ = ಸಾಮರ್ಥ್ಯ;
        state->comments = comments;
    }
    comments->spans[comments->ಎಣಿಕೆ_2++] =
        (Py_Tok_ಹರವು){ಆರಂಭ, ಕೊನೆ};
    ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಮುಕ್ತಾಯ_ftstring_ಅಭಿವ್ಯಕ್ತಿ(ರಚನೆ tok_state *tok, ftstring_state *state,
                     ರಚನೆ token *token)
{
    ಪ್ರತಿಜ್ಞಾ(token != ಖಾಲಿ && state == _PyLexer_CurrentFTString(tok));
    ಪ್ರತಿಜ್ಞಾ(state->ಬಹುಲಕ == FTSTRING_ಬಹುಲಕ_ಅಭಿವ್ಯಕ್ತಿ && tok->ಆರಂಭ >= 0);

    ಆದರೆ (state->ಅಭಿವ್ಯಕ್ತಿ_ಹರವು.ಕೊನೆ >= 0) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಪ್ರತಿಜ್ಞಾ(state->ಅಭಿವ್ಯಕ್ತಿ_ಹರವು.ಆರಂಭ >= 0);
    state->ಅಭಿವ್ಯಕ್ತಿ_ಹರವು.ಕೊನೆ = tok->ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ tstring_interpolation = _PyLexer_IsTString(state->ಬಗೆ) &&
        state->replacement_ಆಳ == 1;
    ಆದರೆ (!(state->debug_ಅಭಿವ್ಯಕ್ತಿ || tstring_interpolation) || token->metadata) {
        ಹಿಂತಿರುಗಿ 0;
    }
    tokenizer_comments *comments = state->comments;
    PyObject *res;
    ಆದರೆ (comments != ಖಾಲಿ && comments->ಎಣಿಕೆ_2 > 0) {
        Py_ssize_t stripped_ಗಾತ್ರ =
            state->ಅಭಿವ್ಯಕ್ತಿ_ಹರವು.ಕೊನೆ - state->ಅಭಿವ್ಯಕ್ತಿ_ಹರವು.ಆರಂಭ;
        Py_ssize_t comment_ಎಣಿಕೆ = 0;
        ಗಾಗಿ (Py_ssize_t i = 0; i < comments->ಎಣಿಕೆ_2; i++) {
            Py_Tok_ಹರವು comment = comments->spans[i];
            ಪ್ರತಿಜ್ಞಾ(Py_Tok_ಹರವು_Is_Valid(comment));
            ಪ್ರತಿಜ್ಞಾ(comment.ಆರಂಭ >= (i == 0 ? state->ಅಭಿವ್ಯಕ್ತಿ_ಹರವು.ಆರಂಭ
                                           : comments->spans[i - 1].ಕೊನೆ));
            ಆದರೆ (comment.ಆರಂಭ >= state->ಅಭಿವ್ಯಕ್ತಿ_ಹರವು.ಕೊನೆ) {
                ನಿಲ್ಲಿಸು;
            }
            ಪ್ರತಿಜ್ಞಾ(comment.ಕೊನೆ <= state->ಅಭಿವ್ಯಕ್ತಿ_ಹರವು.ಕೊನೆ);
            stripped_ಗಾತ್ರ -= comment.ಕೊನೆ - comment.ಆರಂಭ;
            comment_ಎಣಿಕೆ++;
        }
        ಅಕ್ಷರ *stripped = PyMem_Malloc((ಗಾತ್ರ_t)stripped_ಗಾತ್ರ);
        ಆದರೆ (stripped == ಖಾಲಿ) {
            Py_Err_No_ಸ್ಮರಣೆ();
            ಹಿಂತಿರುಗಿ -1;
        }
        Py_Tok_ಹರವು kept = {state->ಅಭಿವ್ಯಕ್ತಿ_ಹರವು.ಆರಂಭ, state->ಅಭಿವ್ಯಕ್ತಿ_ಹರವು.ಆರಂಭ};
        Py_ssize_t stripped_len = 0;
        ಗಾಗಿ (Py_ssize_t i = 0; i < comment_ಎಣಿಕೆ; i++) {
            kept.ಕೊನೆ = comments->spans[i].ಆರಂಭ;
            Py_ssize_t ಉದ್ದಳತೆ;
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ = Py_Tok_ಮೂಲ_ಹರವು_View(
                &tok->ಮೂಲ, kept, &ಉದ್ದಳತೆ);
            ಸ್ಮರಣೆ_ನಕಲು(stripped + stripped_len, ಪಠ್ಯ, (ಗಾತ್ರ_t)ಉದ್ದಳತೆ);
            stripped_len += ಉದ್ದಳತೆ;
            kept.ಆರಂಭ = comments->spans[i].ಕೊನೆ;
        }
        kept.ಕೊನೆ = state->ಅಭಿವ್ಯಕ್ತಿ_ಹರವು.ಕೊನೆ;
        Py_ssize_t ಉದ್ದಳತೆ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ = Py_Tok_ಮೂಲ_ಹರವು_View(&tok->ಮೂಲ, kept, &ಉದ್ದಳತೆ);
        ಸ್ಮರಣೆ_ನಕಲು(stripped + stripped_len, ಪಠ್ಯ, (ಗಾತ್ರ_t)ಉದ್ದಳತೆ);
        res = PyUnicode_DecodeUTF8(stripped, stripped_ಗಾತ್ರ, ಖಾಲಿ);
        PyMem_Free(stripped);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        Py_ssize_t ಅಭಿವ್ಯಕ್ತಿ_len;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಅಭಿವ್ಯಕ್ತಿ = Py_Tok_ಮೂಲ_ಹರವು_View(
            &tok->ಮೂಲ, state->ಅಭಿವ್ಯಕ್ತಿ_ಹರವು, &ಅಭಿವ್ಯಕ್ತಿ_len);
        res = PyUnicode_DecodeUTF8(ಅಭಿವ್ಯಕ್ತಿ, ಅಭಿವ್ಯಕ್ತಿ_len, ಖಾಲಿ);
    }

    ಆದರೆ (!res) {
        ಹಿಂತಿರುಗಿ -1;
    }
    token->metadata = res;
    ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
begin_ftstring_ಅಭಿವ್ಯಕ್ತಿ(ರಚನೆ tok_state *tok, ftstring_state *state,
                    _PyTok_Off ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ)
{
    ಪ್ರತಿಜ್ಞಾ(state->ಬಹುಲಕ != FTSTRING_ಬಹುಲಕ_ಅಭಿವ್ಯಕ್ತಿ);
    state->ಅಭಿವ್ಯಕ್ತಿ_ಹರವು = (Py_Tok_ಹರವು){ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ, -1};
    ಆದರೆ (state->comments != ಖಾಲಿ) {
        state->comments->ಎಣಿಕೆ_2 = 0;
    }
    ಆದರೆ (state->replacement_ಆಳ >= MAX_ಅಭಿವ್ಯಕ್ತಿ_NESTING) {
        _PyTokenizer_syntaxerror(
            tok, "%c-string: expressions nested too deeply",
            Py_Lexer_String_ಪೂರ್ವಪ್ರತ್ಯಯ(state->ಬಗೆ));
        ಹಿಂತಿರುಗಿ -1;
    }
    state->replacement_ಆಳ++;
    state->ಬಹುಲಕ = FTSTRING_ಬಹುಲಕ_ಅಭಿವ್ಯಕ್ತಿ;
    state->debug_ಅಭಿವ್ಯಕ್ತಿ = 0;
    ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ
_PyLexer_ftstring_punctuation(ರಚನೆ tok_state *tok, ftstring_state *state,
                              ರಚನೆ token *token, ಪೂರ್ಣಾಂಕ c)
{
    ಪ್ರತಿಜ್ಞಾ(state->ಬಹುಲಕ == FTSTRING_ಬಹುಲಕ_ಅಭಿವ್ಯಕ್ತಿ);
    ಪ್ರತಿಜ್ಞಾ(c == ':' || c == '}' || c == '!');
    ಆದರೆ (Py_Lexer_FTString_Bracket_ಆಳ(tok, state) != state->replacement_ಆಳ) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (c == '!') {
        ಪೂರ್ಣಾಂಕ ಮುಂದಿನ = tok_nextc(tok);
        tok_backup(tok, ಮುಂದಿನ);
        ಆದರೆ (ಮುಂದಿನ == '=') {
            ಹಿಂತಿರುಗಿ 0;
        }
    }
    ಆದರೆ (ಮುಕ್ತಾಯ_ftstring_ಅಭಿವ್ಯಕ್ತಿ(tok, state, token) < 0) {
        ಹಿಂತಿರುಗಿ -1;
    }
    ಆದರೆ (c == ':') {
        state->ಬಹುಲಕ = FTSTRING_ಬಹುಲಕ_FORMAT_SPEC;
        ಹಿಂತಿರುಗಿ COLON;
    }
    ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ
Py_Lexer_close_ftstring_ಅಭಿವ್ಯಕ್ತಿ(ರಚನೆ tok_state *tok, ftstring_state *state,
                             ಪೂರ್ಣಾಂಕ c)
{
    ಪ್ರತಿಜ್ಞಾ(state->ಬಹುಲಕ == FTSTRING_ಬಹುಲಕ_ಅಭಿವ್ಯಕ್ತಿ);
    ಪ್ರತಿಜ್ಞಾ(c == ')' || c == ']' || c == '}');
    ಪೂರ್ಣಾಂಕ ಆಳ = Py_Lexer_FTString_Bracket_ಆಳ(tok, state);
    ಆದರೆ (ಆಳ < 0) {
        _PyTokenizer_syntaxerror(tok, "%c-string: unmatched '%c'",
                                Py_Lexer_String_ಪೂರ್ವಪ್ರತ್ಯಯ(state->ಬಗೆ), c);
        ಹಿಂತಿರುಗಿ -1;
    }
    ಆದರೆ (c == '}' && ಆಳ == state->replacement_ಆಳ - 1) {
        state->replacement_ಆಳ--;
        state->ಬಹುಲಕ = FTSTRING_ಬಹುಲಕ_ಮಧ್ಯ;
        state->debug_ಅಭಿವ್ಯಕ್ತಿ = 0;
    }
    ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ
_PyLexer_mark_ftstring_debug(ರಚನೆ tok_state *tok, ftstring_state *state)
{
    ಪ್ರತಿಜ್ಞಾ(state->ಬಹುಲಕ == FTSTRING_ಬಹುಲಕ_ಅಭಿವ್ಯಕ್ತಿ);
    ಆದರೆ (Py_Lexer_FTString_Bracket_ಆಳ(tok, state) == state->replacement_ಆಳ) {
        state->debug_ಅಭಿವ್ಯಕ್ತಿ = 1;
    }
}

ಪೂರ್ಣಾಂಕ
_PyLexer_check_string_prefixes(ರಚನೆ tok_state *tok,
                                             ಪೂರ್ಣಾಂಕ saw_b, ಪೂರ್ಣಾಂಕ saw_r, ಪೂರ್ಣಾಂಕ saw_u,
                                             ಪೂರ್ಣಾಂಕ saw_f, ಪೂರ್ಣಾಂಕ saw_t) {
    // Supported: rb, rf, rt (in any order)
    // Unsupported: ub, ur, uf, ut, bf, bt, ft (in any order)

#ವ್ಯಾಖ್ಯಾನಿಸು RETURN_SYNTAX_ERROR(PREFIX1, PREFIX2)                             \
    ಮಾಡು {                                                                  \
        (ಶೂನ್ಯ)Py_Tokenizer_syntaxerror_ತಿಳಿದ_range(                       \
            tok, (ಪೂರ್ಣಾಂಕ)(tok->ಆರಂಭ + 1 - tok->ಸಾಲು_ಆರಂಭ),                 \
            (ಪೂರ್ಣಾಂಕ)(tok->cur - tok->ಸಾಲು_ಆರಂಭ),                            \
            "'" PREFIX1 "' and '" PREFIX2 "' prefixes are incompatible"); \
        ಹಿಂತಿರುಗಿ -1;                                                        \
    } ಆಗ (0)

    ಆದರೆ (saw_u && saw_b) {
        RETURN_SYNTAX_ERROR("u", "b");
    }
    ಆದರೆ (saw_u && saw_r) {
        RETURN_SYNTAX_ERROR("u", "r");
    }
    ಆದರೆ (saw_u && saw_f) {
        RETURN_SYNTAX_ERROR("u", "f");
    }
    ಆದರೆ (saw_u && saw_t) {
        RETURN_SYNTAX_ERROR("u", "t");
    }

    ಆದರೆ (saw_b && saw_f) {
        RETURN_SYNTAX_ERROR("b", "f");
    }
    ಆದರೆ (saw_b && saw_t) {
        RETURN_SYNTAX_ERROR("b", "t");
    }

    ಆದರೆ (saw_f && saw_t) {
        RETURN_SYNTAX_ERROR("f", "t");
    }

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು RETURN_SYNTAX_ERROR

    ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ
Py_Lexer_ಪರಿಶೀಲನೆ_fstring_ಆರಂಭ(ರಚನೆ tok_state *tok, ರಚನೆ token *token,
                            ಪೂರ್ಣಾಂಕ c, ftstring_ಬಗೆ ಬಗೆ)
{
    _PyTok_Off p_ಆರಂಭ = -1;
    _PyTok_Off p_ಕೊನೆ = -1;

    ಪೂರ್ಣಾಂಕ quote = c;
    ಪೂರ್ಣಾಂಕ quote_ಗಾತ್ರ = 1;             /* 1 or 3 */

    /* Find the quote size and start of string */
    ಪೂರ್ಣಾಂಕ after_quote = tok_nextc(tok);
    ಆದರೆ (after_quote == quote) {
        ಪೂರ್ಣಾಂಕ after_after_quote = tok_nextc(tok);
        ಆದರೆ (after_after_quote == quote) {
            quote_ಗಾತ್ರ = 3;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            // TODO: Check this
            tok_backup(tok, after_after_quote);
            tok_backup(tok, after_quote);
        }
    }
    ಆದರೆ (after_quote != quote) {
        tok_backup(tok, after_quote);
    }

    p_ಆರಂಭ = tok->ಆರಂಭ;
    p_ಕೊನೆ = tok->cur;
    ftstring_state *state = Py_Lexer_ತಳ್ಳು_FTString(tok);
    ಆದರೆ (state == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
    }
    state->ಬಹುಲಕ = FTSTRING_ಬಹುಲಕ_ಮಧ್ಯ;
    state->quote = quote;
    state->quote_ಗಾತ್ರ = quote_ಗಾತ್ರ;
    state->paren_level = tok->level;
    state->ಆರಂಭ = tok->ಆರಂಭ;
    state->ಆರಂಭ_loc = tok->ಆರಂಭ_loc;
    state->ಅಭಿವ್ಯಕ್ತಿ_ಹರವು = (Py_Tok_ಹರವು){-1, -1};

    state->ಬಗೆ = ಬಗೆ;
    ಹಿಂತಿರುಗಿ _PyLexer_IsTString(ಬಗೆ)
        ? MAKE_TOKEN(TSTRING_ಆರಂಭ) : MAKE_TOKEN(FSTRING_ಆರಂಭ);
}

ಪೂರ್ಣಾಂಕ
Py_Lexer_ಪರಿಶೀಲನೆ_string(ರಚನೆ tok_state *tok, ರಚನೆ token *token, ಪೂರ್ಣಾಂಕ c)
{
    _PyTok_Off p_ಆರಂಭ = -1;
    _PyTok_Off p_ಕೊನೆ = -1;

    ಪೂರ್ಣಾಂಕ quote = c;
    ಪೂರ್ಣಾಂಕ quote_ಗಾತ್ರ = 1;             /* 1 or 3 */
    ಪೂರ್ಣಾಂಕ ಕೊನೆ_quote_ಗಾತ್ರ = 0;
    ಪೂರ್ಣಾಂಕ has_escaped_quote = 0;

    /* Find the quote size and start of string */
    c = tok_nextc(tok);
    ಆದರೆ (c == quote) {
        c = tok_nextc(tok);
        ಆದರೆ (c == quote) {
            quote_ಗಾತ್ರ = 3;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            ಕೊನೆ_quote_ಗಾತ್ರ = 1;     /* empty string found */
        }
    }
    ಆದರೆ (c != quote) {
        tok_backup(tok, c);
    }

    /* Get rest of string */
    ಆಗ (ಕೊನೆ_quote_ಗಾತ್ರ != quote_ಗಾತ್ರ) {
        c = tok_nextc(tok);
        ಆದರೆ (tok->ಮುಗಿದ == E_ERROR) {
            ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
        }
        ಆದರೆ (tok->ಮುಗಿದ == E_DECODE) {
            ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ (c == EOF || (quote_ಗಾತ್ರ == 1 && c == '\n')) {
            ಆದರೆ (tok_failed(tok)) {
                ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
            }
            ಆದರೆ (c == EOF && tok->allow_incomplete_ಒಳಹರಿವು) {
                ಹಿಂತಿರುಗಿ incomplete_string_token(tok, token, STRING,
                                               tok->ಆರಂಭ, tok->ಆರಂಭ_loc);
            }
            ಪೂರ್ಣಾಂಕ ಕೊನೆ_lineno = tok->lineno;
            _PyTok_Loc location = tok->ಆರಂಭ_loc;
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು = Py_Tok_ಮೂಲ_ಸೂಚಕ(
                &tok->ಮೂಲ, tok->ಆರಂಭ - location.byte_ಲಂಬಸಾಲು);
            Py_ssize_t cursor_ಸ್ಥಾನಾಂತರ = (Py_ssize_t)location.byte_ಲಂಬಸಾಲು + 1;

            ಸ್ಥಿರಾಂಕ ftstring_state *state = _PyLexer_CurrentFTString(tok);
            ಆದರೆ (state != ಖಾಲಿ) {
                /* A matching quote may have been intended to close the
                 * surrounding formatted string instead of opening a string
                 * inside a replacement field. */
                ಆದರೆ (state->quote == quote && state->quote_ಗಾತ್ರ == quote_ಗಾತ್ರ) {
                    ಪೂರ್ಣಾಂಕ level = state->paren_level + state->replacement_ಆಳ - 1;
                    ಪ್ರತಿಜ್ಞಾ(level >= 0 && level < tok->level);
                    ಪ್ರತಿಜ್ಞಾ(tok->parenstack[level] == '{');
                    ಪೂರ್ಣಾಂಕ lineno = tok->parenlinenostack[level];
                    ಆದರೆ (lineno != location.lineno) {
                        _PyTokenizer_syntaxerror_at(
                            tok, ಸಾಲು, cursor_ಸ್ಥಾನಾಂತರ, location.lineno, -1, -1,
                            "%c-string: expecting '}' to close '{' on line %d",
                            Py_Lexer_String_ಪೂರ್ವಪ್ರತ್ಯಯ(state->ಬಗೆ), lineno);
                    }
                    ಇಲ್ಲದಿದ್ದರೆ {
                        _PyTokenizer_syntaxerror_at(
                            tok, ಸಾಲು, cursor_ಸ್ಥಾನಾಂತರ, location.lineno, -1, -1,
                            "%c-string: expecting '}'",
                            Py_Lexer_String_ಪೂರ್ವಪ್ರತ್ಯಯ(state->ಬಗೆ));
                    }
                    ಹಿಂತಿರುಗಿ string_error_token(tok, token, tok->ಆರಂಭ, location);
                }
            }

            ಆದರೆ (quote_ಗಾತ್ರ == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, ಸಾಲು, cursor_ಸ್ಥಾನಾಂತರ, location.lineno, -1, -1,
                    "unterminated triple-quoted string literal"
                    " (detected at line %d)", ಕೊನೆ_lineno);
                ಆದರೆ (c != '\n') {
                    tok->ಮುಗಿದ = E_EOFS;
                }
                ಹಿಂತಿರುಗಿ string_error_token(tok, token, tok->ಆರಂಭ, location);
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ (has_escaped_quote) {
                    _PyTokenizer_syntaxerror_at(
                        tok, ಸಾಲು, cursor_ಸ್ಥಾನಾಂತರ, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d); "
                        "perhaps you escaped the end quote?",
                        ಕೊನೆ_lineno
                    );
                } ಇಲ್ಲದಿದ್ದರೆ {
                    _PyTokenizer_syntaxerror_at(
                        tok, ಸಾಲು, cursor_ಸ್ಥಾನಾಂತರ, location.lineno, -1, -1,
                        "unterminated string literal (detected at line %d)", ಕೊನೆ_lineno
                    );
                }
                ಆದರೆ (c != '\n') {
                    tok->ಮುಗಿದ = E_EOLS;
                }
                ಹಿಂತಿರುಗಿ string_error_token(tok, token, tok->ಆರಂಭ, location);
            }
        }
        ಆದರೆ (c == quote) {
            ಕೊನೆ_quote_ಗಾತ್ರ += 1;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            ಕೊನೆ_quote_ಗಾತ್ರ = 0;
            ಆದರೆ (c == '\\') {
                c = tok_nextc(tok);  /* skip escaped char */
                ಆದರೆ (c == quote) {  /* but record whether the escaped char was a quote */
                    has_escaped_quote = 1;
                }
                ಆದರೆ (c == '\r') {
                    c = tok_nextc(tok);
                }
            }
        }
    }

    p_ಆರಂಭ = tok->ಆರಂಭ;
    p_ಕೊನೆ = tok->cur;
    ಹಿಂತಿರುಗಿ MAKE_TOKEN(STRING);
}

ಪೂರ್ಣಾಂಕ
_PyLexer_get_ftstring(ರಚನೆ tok_state *tok, ftstring_state *current, ರಚನೆ token *token)
{
    ಪ್ರತಿಜ್ಞಾ(current == _PyLexer_CurrentFTString(tok) && current->ಬಹುಲಕ != FTSTRING_ಬಹುಲಕ_ಅಭಿವ್ಯಕ್ತಿ);
    ಪ್ರತಿಜ್ಞಾ((current->quote_ಗಾತ್ರ == 1 || current->quote_ಗಾತ್ರ == 3) &&
           current->replacement_ಆಳ <= MAX_ಅಭಿವ್ಯಕ್ತಿ_NESTING);
    _PyTok_Off p_ಆರಂಭ = -1;
    _PyTok_Off p_ಕೊನೆ = -1;
    ಪೂರ್ಣಾಂಕ token_type;
    ಪೂರ್ಣಾಂಕ ಕೊನೆ_quote_ಗಾತ್ರ = 0;
    ಪೂರ್ಣಾಂಕ unicode_escape = 0;
    ಪೂರ್ಣಾಂಕ quote = current->quote;
    ಪೂರ್ಣಾಂಕ quote_ಗಾತ್ರ = current->quote_ಗಾತ್ರ;
    ಪೂರ್ಣಾಂಕ ಒಳ_format_spec = current->ಬಹುಲಕ == FTSTRING_ಬಹುಲಕ_FORMAT_SPEC;
    ಪೂರ್ಣಾಂಕ raw = _PyLexer_IsRawString(current->ಬಗೆ);

    tok->ಆರಂಭ = tok->cur;
    tok->ಆರಂಭ_loc = (_PyTok_Loc){tok->lineno, Py_Lexer_Byte_ಸ್ತಂಭ(tok)};

    ಆಗ (ಕೊನೆ_quote_ಗಾತ್ರ != quote_ಗಾತ್ರ) {
        ಪೂರ್ಣಾಂಕ c = tok_nextc(tok);
        ಆದರೆ (tok->ಮುಗಿದ == E_ERROR || tok->ಮುಗಿದ == E_DECODE) {
            ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
        }

        ಆದರೆ (c == EOF || (quote_ಗಾತ್ರ == 1 && c == '\n')) {
            ಆದರೆ (tok_failed(tok)) {
                ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
            }
            ಆದರೆ (c == EOF && tok->allow_incomplete_ಒಳಹರಿವು) {
                ಹಿಂತಿರುಗಿ incomplete_string_token(tok, token,
                    FTSTRING_ಮಧ್ಯ(current), current->ಆರಂಭ, current->ಆರಂಭ_loc);
            }

            ಆದರೆ (ಒಳ_format_spec && c == '\n') {
                ಹಿಂತಿರುಗಿ MAKE_TOKEN(_PyTokenizer_syntaxerror(
                    tok,
                    "%c-string: newlines are not allowed in format specifiers for single quoted %c-strings",
                    Py_Lexer_String_ಪೂರ್ವಪ್ರತ್ಯಯ(current->ಬಗೆ),
                    Py_Lexer_String_ಪೂರ್ವಪ್ರತ್ಯಯ(current->ಬಗೆ)));
            }

            ಪೂರ್ಣಾಂಕ ಕೊನೆ_lineno = tok->lineno;
            _PyTok_Loc location = current->ಆರಂಭ_loc;
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು = Py_Tok_ಮೂಲ_ಸೂಚಕ(
                &tok->ಮೂಲ, current->ಆರಂಭ - location.byte_ಲಂಬಸಾಲು);
            Py_ssize_t cursor_ಸ್ಥಾನಾಂತರ = (Py_ssize_t)location.byte_ಲಂಬಸಾಲು + 1;

            ಆದರೆ (quote_ಗಾತ್ರ == 3) {
                _PyTokenizer_syntaxerror_at(
                    tok, ಸಾಲು, cursor_ಸ್ಥಾನಾಂತರ, location.lineno, -1, -1,
                    "unterminated triple-quoted %c-string literal"
                    " (detected at line %d)",
                    Py_Lexer_String_ಪೂರ್ವಪ್ರತ್ಯಯ(current->ಬಗೆ), ಕೊನೆ_lineno);
                ಆದರೆ (c != '\n') {
                    tok->ಮುಗಿದ = E_EOFS;
                }
                ಹಿಂತಿರುಗಿ string_error_token(tok, token,
                    current->ಆರಂಭ, location);
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                _PyTokenizer_syntaxerror_at(
                    tok, ಸಾಲು, cursor_ಸ್ಥಾನಾಂತರ, location.lineno, -1, -1,
                    "unterminated %c-string literal (detected at line %d)",
                    Py_Lexer_String_ಪೂರ್ವಪ್ರತ್ಯಯ(current->ಬಗೆ), ಕೊನೆ_lineno);
                ಹಿಂತಿರುಗಿ string_error_token(tok, token,
                    current->ಆರಂಭ, location);
            }
        }

        ಆದರೆ (c == quote) {
            ಕೊನೆ_quote_ಗಾತ್ರ += 1;
            ಮುಂದುವರಿ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಕೊನೆ_quote_ಗಾತ್ರ = 0;
        }

        ಆದರೆ (c == '{') {
            ಪೂರ್ಣಾಂಕ peek = tok_nextc(tok);
            ಆದರೆ (peek != '{' || ಒಳ_format_spec) {
                tok_backup(tok, peek);
                _PyTok_Off ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ = tok->cur;
                tok_backup(tok, c);
                ಆದರೆ (begin_ftstring_ಅಭಿವ್ಯಕ್ತಿ(tok, current, ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ) < 0) {
                    ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
                }
                p_ಆರಂಭ = tok->ಆರಂಭ;
                p_ಕೊನೆ = tok->cur;
                ಆದರೆ (p_ಆರಂಭ == p_ಕೊನೆ) {
                    ಹಿಂತಿರುಗಿ Py_Lexer_get_ಸಾಮಾನ್ಯ(tok, current, token);
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                p_ಆರಂಭ = tok->ಆರಂಭ;
                p_ಕೊನೆ = tok->cur - 1;
            }
            ಹೋಗು emit_ಮಧ್ಯ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '}') {
            ಆದರೆ (unicode_escape) {
                p_ಆರಂಭ = tok->ಆರಂಭ;
                p_ಕೊನೆ = tok->cur;
                ಹೋಗು emit_ಮಧ್ಯ;
            }
            ಪೂರ್ಣಾಂಕ peek = tok_nextc(tok);

            ಪೂರ್ಣಾಂಕ bracket_ಆಳ = Py_Lexer_FTString_Bracket_ಆಳ(tok, current);
            ಆದರೆ (peek == '}' && !ಒಳ_format_spec && bracket_ಆಳ == 0) {
                p_ಆರಂಭ = tok->ಆರಂಭ;
                p_ಕೊನೆ = tok->cur - 1;
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                tok_backup(tok, peek);
                ಆದರೆ (!ಒಳ_format_spec && bracket_ಆಳ == 0) {
                    ಆದರೆ (tok->ಆರಂಭ == tok->cur - 1) {
                        ಹಿಂತಿರುಗಿ MAKE_TOKEN(_PyTokenizer_syntaxerror(
                            tok, "%c-string: single '}' is not allowed",
                            Py_Lexer_String_ಪೂರ್ವಪ್ರತ್ಯಯ(current->ಬಗೆ)));
                    }
                    tok_backup(tok, c);
                    p_ಆರಂಭ = tok->ಆರಂಭ;
                    p_ಕೊನೆ = tok->cur;
                    ಹೋಗು emit_ಮಧ್ಯ;
                }
                tok_backup(tok, c);
                current->ಬಹುಲಕ = FTSTRING_ಬಹುಲಕ_ಅಭಿವ್ಯಕ್ತಿ;
                p_ಆರಂಭ = tok->ಆರಂಭ;
                p_ಕೊನೆ = tok->cur;
            }
            ಹೋಗು emit_ಮಧ್ಯ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '\\') {
            ಪೂರ್ಣಾಂಕ peek = tok_nextc(tok);
            ಆದರೆ (peek == '\r') {
                peek = tok_nextc(tok);
            }
            ಆದರೆ (peek == '{' || peek == '}') {
                ಆದರೆ (!raw) {
                    ಆದರೆ (_PyTokenizer_warn_invalid_escape_sequence(tok, peek)) {
                        ಹಿಂತಿರುಗಿ MAKE_TOKEN(ERRORTOKEN);
                    }
                }
                tok_backup(tok, peek);
                ಮುಂದುವರಿ;
            }

            ಆದರೆ (!raw) {
                ಆದರೆ (peek == 'N') {
                    /* Handle named unicode escapes (\N{BULLET}) */
                    peek = tok_nextc(tok);
                    ಆದರೆ (peek == '{') {
                        unicode_escape = 1;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        tok_backup(tok, peek);
                    }
                }
            }
        }
    }

    p_ಆರಂಭ = tok->ಆರಂಭ;
    p_ಕೊನೆ = tok->cur;
    ಆದರೆ (p_ಕೊನೆ - quote_ಗಾತ್ರ == p_ಆರಂಭ) {
        ಪೂರ್ಣಾಂಕ ಕೊನೆ_token = FTSTRING_ಕೊನೆ(current);
        Py_Lexer_ತೆಗೆ_FTString(tok);
        ಹಿಂತಿರುಗಿ MAKE_TOKEN(ಕೊನೆ_token);
    }
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < quote_ಗಾತ್ರ; i++) {
        tok_backup(tok, quote);
    }
    p_ಕೊನೆ = tok->cur;
emit_ಮಧ್ಯ:
    token_type = MAKE_TOKEN(FTSTRING_ಮಧ್ಯ(current));
    token->is_raw = raw;
    ಹಿಂತಿರುಗಿ token_type;
}
