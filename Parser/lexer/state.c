#شامل "Python.h"
#شامل "pycore_token.h"
#شامل "errcode.h"

#شامل "state.h"
#شامل "../tokenizer/helpers.h"
#شامل "../tokenizer/reader.h"

ftstring_state *
Py_Lexer_ڈالو_FTString(ساخت tok_state *tok)
{
    أكد(tok->ftstring_گہرائی >= 0 && tok->ftstring_گہرائی <= tok->ftstring_استعداد);
    صحیح اگلا_گہرائی = tok->ftstring_گہرائی + 1;
    اگر (اگلا_گہرائی >= MAXFTSTRINGLEVEL) {
        _PyTokenizer_syntaxerror(
            tok, "too many nested f-strings or t-strings");
        واپس تہی;
    }
    اگر (tok->ftstring_گہرائی == tok->ftstring_استعداد) {
        صحیح استعداد = Py_MIN(Py_MAX(tok->ftstring_استعداد * 2, 4),
                              MAXFTSTRINGLEVEL);
        سائز_t سائز = (سائز_t)استعداد * حجم(*tok->ftstring_ڈھیر);
        ftstring_state *ڈھیر;
        اگر (tok->ftstring_ڈھیر == tok->ftstring_ڈھیر_inline) {
            ڈھیر = PyMem_Malloc(سائز);
            اگر (ڈھیر != تہی) {
                حافظہ_نقل(ڈھیر, tok->ftstring_ڈھیر,
                       (سائز_t)tok->ftstring_گہرائی * حجم(*ڈھیر));
            }
        }
        ورنہ {
            ڈھیر = PyMem_Realloc(tok->ftstring_ڈھیر, سائز);
        }
        اگر (ڈھیر == تہی) {
            Py_Err_No_حافظہ();
            tok->مکمل = E_NOMEM;
            واپس تہی;
        }
        tok->ftstring_ڈھیر = ڈھیر;
        tok->ftstring_استعداد = استعداد;
    }
    ftstring_state *state = &tok->ftstring_ڈھیر[tok->ftstring_گہرائی];
    tok->ftstring_گہرائی = اگلا_گہرائی;
    *state = (ftstring_state){0};
    واپس state;
}

خالی
Py_Lexer_نکالو_FTString(ساخت tok_state *tok)
{
    ftstring_state *state = _PyLexer_CurrentFTString(tok);
    أكد(state != تہی);
    PyMem_Free(state->comments);
    tok->ftstring_گہرائی--;
}

/* Free a tok_state structure */
خالی
_PyTokenizer_Free(ساخت tok_state *tok)
{
    Py_XDECREF(tok->filename);
    Py_XDECREF(tok->module);
    _PyTok_ReaderFree(tok);
    Py_Tok_ماخذ_Clear(&tok->ماخذ);
    برائے (صحیح i = 0; i < tok->ftstring_گہرائی; i++) {
        PyMem_Free(tok->ftstring_ڈھیر[i].comments);
    }
    اگر (tok->ftstring_ڈھیر != tok->ftstring_ڈھیر_inline) {
        PyMem_Free(tok->ftstring_ڈھیر);
    }
    PyMem_Free(tok);
}

خالی
_PyToken_Init(ساخت token *token) {
    *token = (ساخت token){
        .type = -1,
        .وسعت = {-1, -1},
        .آغاز_loc = {-1, -1},
        .انتہا_loc = {-1, -1},
    };
}

صحیح
Py_Lexer_token_تیاری(ساخت tok_state *tok, ساخت token *token, صحیح type, _PyTok_Off آغاز, _PyTok_Off انتہا)
{
    token->level = tok->level;
    token->is_raw = 0;
    أكد((آغاز == -1 && انتہا == -1) || (آغاز >= 0 && انتہا >= آغاز));
    token->وسعت = (Py_Tok_وسعت){آغاز, انتہا};
    اگر (آغاز >= 0) {
        token->آغاز_loc = tok->آغاز_loc;
        token->انتہا_loc = (_PyTok_Loc){tok->lineno, Py_Lexer_Byte_عمود(tok)};
    }
    ورنہ {
        token->آغاز_loc = token->انتہا_loc = (_PyTok_Loc){tok->lineno, -1};
    }
    واپس type;
}
