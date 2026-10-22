#ਅਨ੍ਤਰ੍ਭਾਵ "Python.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_token.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errcode.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "state.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "../tokenizer/helpers.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "../tokenizer/reader.h"

ftstring_state *
Py_Lexer_ਧੱਕੋ_FTString(ਬਣਤਰ tok_state *tok)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(tok->ftstring_ਡੂੰਘਾਈ >= 0 && tok->ftstring_ਡੂੰਘਾਈ <= tok->ftstring_ਸਮਰੱਥਾ);
    ਪੂਰਨ ਅਗਲਾ_ਡੂੰਘਾਈ = tok->ftstring_ਡੂੰਘਾਈ + 1;
    ਜੇ (ਅਗਲਾ_ਡੂੰਘਾਈ >= MAXFTSTRINGLEVEL) {
        _PyTokenizer_syntaxerror(
            tok, "too many nested f-strings or t-strings");
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (tok->ftstring_ਡੂੰਘਾਈ == tok->ftstring_ਸਮਰੱਥਾ) {
        ਪੂਰਨ ਸਮਰੱਥਾ = Py_MIN(Py_MAX(tok->ftstring_ਸਮਰੱਥਾ * 2, 4),
                              MAXFTSTRINGLEVEL);
        ਮਾਪ_t ਮਾਪ = (ਮਾਪ_t)ਸਮਰੱਥਾ * ਆਕਾਰ(*tok->ftstring_ਢੇਰ);
        ftstring_state *ਢੇਰ;
        ਜੇ (tok->ftstring_ਢੇਰ == tok->ftstring_ਢੇਰ_inline) {
            ਢੇਰ = PyMem_Malloc(ਮਾਪ);
            ਜੇ (ਢੇਰ != ਨਿਰਾ) {
                ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਢੇਰ, tok->ftstring_ਢੇਰ,
                       (ਮਾਪ_t)tok->ftstring_ਡੂੰਘਾਈ * ਆਕਾਰ(*ਢੇਰ));
            }
        }
        ਨਹੀਂਤਾਂ {
            ਢੇਰ = PyMem_Realloc(tok->ftstring_ਢੇਰ, ਮਾਪ);
        }
        ਜੇ (ਢੇਰ == ਨਿਰਾ) {
            Py_Err_No_ਸਮ੍ਰਿਤੀ();
            tok->ਪੂਰਾ = E_NOMEM;
            ਵਾਪਸ ਨਿਰਾ;
        }
        tok->ftstring_ਢੇਰ = ਢੇਰ;
        tok->ftstring_ਸਮਰੱਥਾ = ਸਮਰੱਥਾ;
    }
    ftstring_state *state = &tok->ftstring_ਢੇਰ[tok->ftstring_ਡੂੰਘਾਈ];
    tok->ftstring_ਡੂੰਘਾਈ = ਅਗਲਾ_ਡੂੰਘਾਈ;
    *state = (ftstring_state){0};
    ਵਾਪਸ state;
}

ਖਾਲੀ
Py_Lexer_ਕੱਢੋ_FTString(ਬਣਤਰ tok_state *tok)
{
    ftstring_state *state = _PyLexer_CurrentFTString(tok);
    ਪ੍ਰਤਿਜ੍ਞਾ(state != ਨਿਰਾ);
    PyMem_Free(state->comments);
    tok->ftstring_ਡੂੰਘਾਈ--;
}

/* Free a tok_state structure */
ਖਾਲੀ
_PyTokenizer_Free(ਬਣਤਰ tok_state *tok)
{
    Py_XDECREF(tok->filename);
    Py_XDECREF(tok->module);
    _PyTok_ReaderFree(tok);
    Py_Tok_ਸਰੋਤ_Clear(&tok->ਸਰੋਤ);
    ਲਈ (ਪੂਰਨ i = 0; i < tok->ftstring_ਡੂੰਘਾਈ; i++) {
        PyMem_Free(tok->ftstring_ਢੇਰ[i].comments);
    }
    ਜੇ (tok->ftstring_ਢੇਰ != tok->ftstring_ਢੇਰ_inline) {
        PyMem_Free(tok->ftstring_ਢੇਰ);
    }
    PyMem_Free(tok);
}

ਖਾਲੀ
_PyToken_Init(ਬਣਤਰ token *token) {
    *token = (ਬਣਤਰ token){
        .type = -1,
        .ਵਿਸਤਾਰ = {-1, -1},
        .ਸ਼ੁਰੂ_loc = {-1, -1},
        .ਸਿਰਾ_loc = {-1, -1},
    };
}

ਪੂਰਨ
Py_Lexer_token_ਤਿਆਰੀ(ਬਣਤਰ tok_state *tok, ਬਣਤਰ token *token, ਪੂਰਨ type, _PyTok_Off ਸ਼ੁਰੂ, _PyTok_Off ਸਿਰਾ)
{
    token->level = tok->level;
    token->is_raw = 0;
    ਪ੍ਰਤਿਜ੍ਞਾ((ਸ਼ੁਰੂ == -1 && ਸਿਰਾ == -1) || (ਸ਼ੁਰੂ >= 0 && ਸਿਰਾ >= ਸ਼ੁਰੂ));
    token->ਵਿਸਤਾਰ = (Py_Tok_ਵਿਸਤਾਰ){ਸ਼ੁਰੂ, ਸਿਰਾ};
    ਜੇ (ਸ਼ੁਰੂ >= 0) {
        token->ਸ਼ੁਰੂ_loc = tok->ਸ਼ੁਰੂ_loc;
        token->ਸਿਰਾ_loc = (_PyTok_Loc){tok->lineno, Py_Lexer_Byte_ਕਾਲਮ(tok)};
    }
    ਨਹੀਂਤਾਂ {
        token->ਸ਼ੁਰੂ_loc = token->ਸਿਰਾ_loc = (_PyTok_Loc){tok->lineno, -1};
    }
    ਵਾਪਸ type;
}
