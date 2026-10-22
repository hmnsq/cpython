#ಅನ್ತರ್ಭಾವ "Python.h"
#ಅನ್ತರ್ಭಾವ "pycore_token.h"
#ಅನ್ತರ್ಭಾವ "errcode.h"

#ಅನ್ತರ್ಭಾವ "state.h"
#ಅನ್ತರ್ಭಾವ "../tokenizer/helpers.h"
#ಅನ್ತರ್ಭಾವ "../tokenizer/reader.h"

ftstring_state *
Py_Lexer_ತಳ್ಳು_FTString(ರಚನೆ tok_state *tok)
{
    ಪ್ರತಿಜ್ಞಾ(tok->ftstring_ಆಳ >= 0 && tok->ftstring_ಆಳ <= tok->ftstring_ಸಾಮರ್ಥ್ಯ);
    ಪೂರ್ಣಾಂಕ ಮುಂದಿನ_ಆಳ = tok->ftstring_ಆಳ + 1;
    ಆದರೆ (ಮುಂದಿನ_ಆಳ >= MAXFTSTRINGLEVEL) {
        _PyTokenizer_syntaxerror(
            tok, "too many nested f-strings or t-strings");
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (tok->ftstring_ಆಳ == tok->ftstring_ಸಾಮರ್ಥ್ಯ) {
        ಪೂರ್ಣಾಂಕ ಸಾಮರ್ಥ್ಯ = Py_MIN(Py_MAX(tok->ftstring_ಸಾಮರ್ಥ್ಯ * 2, 4),
                              MAXFTSTRINGLEVEL);
        ಗಾತ್ರ_t ಗಾತ್ರ_2 = (ಗಾತ್ರ_t)ಸಾಮರ್ಥ್ಯ * ಗಾತ್ರ(*tok->ftstring_ರಾಶಿ);
        ftstring_state *ರಾಶಿ;
        ಆದರೆ (tok->ftstring_ರಾಶಿ == tok->ftstring_ರಾಶಿ_inline) {
            ರಾಶಿ = PyMem_Malloc(ಗಾತ್ರ_2);
            ಆದರೆ (ರಾಶಿ != ಖಾಲಿ) {
                ಸ್ಮರಣೆ_ನಕಲು(ರಾಶಿ, tok->ftstring_ರಾಶಿ,
                       (ಗಾತ್ರ_t)tok->ftstring_ಆಳ * ಗಾತ್ರ(*ರಾಶಿ));
            }
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            ರಾಶಿ = PyMem_Realloc(tok->ftstring_ರಾಶಿ, ಗಾತ್ರ_2);
        }
        ಆದರೆ (ರಾಶಿ == ಖಾಲಿ) {
            Py_Err_No_ಸ್ಮರಣೆ();
            tok->ಮುಗಿದ = E_NOMEM;
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
        tok->ftstring_ರಾಶಿ = ರಾಶಿ;
        tok->ftstring_ಸಾಮರ್ಥ್ಯ = ಸಾಮರ್ಥ್ಯ;
    }
    ftstring_state *state = &tok->ftstring_ರಾಶಿ[tok->ftstring_ಆಳ];
    tok->ftstring_ಆಳ = ಮುಂದಿನ_ಆಳ;
    *state = (ftstring_state){0};
    ಹಿಂತಿರುಗಿ state;
}

ಶೂನ್ಯ
Py_Lexer_ತೆಗೆ_FTString(ರಚನೆ tok_state *tok)
{
    ftstring_state *state = _PyLexer_CurrentFTString(tok);
    ಪ್ರತಿಜ್ಞಾ(state != ಖಾಲಿ);
    PyMem_Free(state->comments);
    tok->ftstring_ಆಳ--;
}

/* Free a tok_state structure */
ಶೂನ್ಯ
_PyTokenizer_Free(ರಚನೆ tok_state *tok)
{
    Py_XDECREF(tok->filename);
    Py_XDECREF(tok->module);
    _PyTok_ReaderFree(tok);
    Py_Tok_ಮೂಲ_Clear(&tok->ಮೂಲ);
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < tok->ftstring_ಆಳ; i++) {
        PyMem_Free(tok->ftstring_ರಾಶಿ[i].comments);
    }
    ಆದರೆ (tok->ftstring_ರಾಶಿ != tok->ftstring_ರಾಶಿ_inline) {
        PyMem_Free(tok->ftstring_ರಾಶಿ);
    }
    PyMem_Free(tok);
}

ಶೂನ್ಯ
_PyToken_Init(ರಚನೆ token *token) {
    *token = (ರಚನೆ token){
        .type = -1,
        .ಹರವು = {-1, -1},
        .ಆರಂಭ_loc = {-1, -1},
        .ಕೊನೆ_loc = {-1, -1},
    };
}

ಪೂರ್ಣಾಂಕ
Py_Lexer_token_ಸಿದ್ಧತೆ(ರಚನೆ tok_state *tok, ರಚನೆ token *token, ಪೂರ್ಣಾಂಕ type, _PyTok_Off ಆರಂಭ, _PyTok_Off ಕೊನೆ)
{
    token->level = tok->level;
    token->is_raw = 0;
    ಪ್ರತಿಜ್ಞಾ((ಆರಂಭ == -1 && ಕೊನೆ == -1) || (ಆರಂಭ >= 0 && ಕೊನೆ >= ಆರಂಭ));
    token->ಹರವು = (Py_Tok_ಹರವು){ಆರಂಭ, ಕೊನೆ};
    ಆದರೆ (ಆರಂಭ >= 0) {
        token->ಆರಂಭ_loc = tok->ಆರಂಭ_loc;
        token->ಕೊನೆ_loc = (_PyTok_Loc){tok->lineno, Py_Lexer_Byte_ಸ್ತಂಭ(tok)};
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        token->ಆರಂಭ_loc = token->ಕೊನೆ_loc = (_PyTok_Loc){tok->lineno, -1};
    }
    ಹಿಂತಿರುಗಿ type;
}
