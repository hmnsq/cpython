#ضمن "Python.h"
#ضمن "pycore_token.h"
#ضمن "errcode.h"

#ضمن "state.h"
#ضمن "../tokenizer/helpers.h"
#ضمن "../tokenizer/reader.h"

ftstring_state *
Py_Lexer_ڈالو_FTString(بنية tok_state *tok)
{
    أكد(tok->ftstring_ڈونگھائی >= 0 && tok->ftstring_ڈونگھائی <= tok->ftstring_سمرتھا);
    صحيح اگلا_ڈونگھائی = tok->ftstring_ڈونگھائی + 1;
    إذا (اگلا_ڈونگھائی >= MAXFTSTRINGLEVEL) {
        _PyTokenizer_syntaxerror(
            tok, "too many nested f-strings or t-strings");
        أرجع عدم;
    }
    إذا (tok->ftstring_ڈونگھائی == tok->ftstring_سمرتھا) {
        صحيح سمرتھا = Py_MIN(Py_MAX(tok->ftstring_سمرتھا * 2, 4),
                              MAXFTSTRINGLEVEL);
        ناپ_t ناپ = (ناپ_t)سمرتھا * حجم(*tok->ftstring_ڈھیر);
        ftstring_state *ڈھیر;
        إذا (tok->ftstring_ڈھیر == tok->ftstring_ڈھیر_inline) {
            ڈھیر = PyMem_Malloc(ناپ);
            إذا (ڈھیر != عدم) {
                حافظہ_نقل(ڈھیر, tok->ftstring_ڈھیر,
                       (ناپ_t)tok->ftstring_ڈونگھائی * حجم(*ڈھیر));
            }
        }
        وإلا {
            ڈھیر = PyMem_Realloc(tok->ftstring_ڈھیر, ناپ);
        }
        إذا (ڈھیر == عدم) {
            Py_Err_No_حافظہ();
            tok->پورا = E_NOMEM;
            أرجع عدم;
        }
        tok->ftstring_ڈھیر = ڈھیر;
        tok->ftstring_سمرتھا = سمرتھا;
    }
    ftstring_state *state = &tok->ftstring_ڈھیر[tok->ftstring_ڈونگھائی];
    tok->ftstring_ڈونگھائی = اگلا_ڈونگھائی;
    *state = (ftstring_state){0};
    أرجع state;
}

فراغ
Py_Lexer_نکالو_FTString(بنية tok_state *tok)
{
    ftstring_state *state = _PyLexer_CurrentFTString(tok);
    أكد(state != عدم);
    PyMem_Free(state->comments);
    tok->ftstring_ڈونگھائی--;
}

/* Free a tok_state structure */
فراغ
_PyTokenizer_Free(بنية tok_state *tok)
{
    Py_XDECREF(tok->filename);
    Py_XDECREF(tok->module);
    _PyTok_ReaderFree(tok);
    Py_Tok_سروت_Clear(&tok->سروت);
    لكل (صحيح i = 0; i < tok->ftstring_ڈونگھائی; i++) {
        PyMem_Free(tok->ftstring_ڈھیر[i].comments);
    }
    إذا (tok->ftstring_ڈھیر != tok->ftstring_ڈھیر_inline) {
        PyMem_Free(tok->ftstring_ڈھیر);
    }
    PyMem_Free(tok);
}

فراغ
_PyToken_Init(بنية token *token) {
    *token = (بنية token){
        .type = -1,
        .وسعت = {-1, -1},
        .شروع_loc = {-1, -1},
        .سرا_loc = {-1, -1},
    };
}

صحيح
Py_Lexer_token_تیاری(بنية tok_state *tok, بنية token *token, صحيح type, _PyTok_Off شروع, _PyTok_Off سرا)
{
    token->level = tok->level;
    token->is_raw = 0;
    أكد((شروع == -1 && سرا == -1) || (شروع >= 0 && سرا >= شروع));
    token->وسعت = (Py_Tok_وسعت){شروع, سرا};
    إذا (شروع >= 0) {
        token->شروع_loc = tok->شروع_loc;
        token->سرا_loc = (_PyTok_Loc){tok->lineno, Py_Lexer_Byte_کالم(tok)};
    }
    وإلا {
        token->شروع_loc = token->سرا_loc = (_PyTok_Loc){tok->lineno, -1};
    }
    أرجع type;
}
