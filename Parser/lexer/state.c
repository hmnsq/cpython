#ضمن "Python.h"
#ضمن "pycore_token.h"
#ضمن "errcode.h"

#ضمن "state.h"
#ضمن "../tokenizer/helpers.h"
#ضمن "../tokenizer/reader.h"

ftstring_state *
Py_Lexer_افزودن_FTString(بنية tok_state *tok)
{
    أكد(tok->ftstring_عمق >= 0 && tok->ftstring_عمق <= tok->ftstring_ظرفیت);
    صحيح بعدی_عمق = tok->ftstring_عمق + 1;
    إذا (بعدی_عمق >= MAXFTSTRINGLEVEL) {
        _PyTokenizer_syntaxerror(
            tok, "too many nested f-strings or t-strings");
        أرجع عدم;
    }
    إذا (tok->ftstring_عمق == tok->ftstring_ظرفیت) {
        صحيح ظرفیت = Py_MIN(Py_MAX(tok->ftstring_ظرفیت * 2, 4),
                              MAXFTSTRINGLEVEL);
        اندازه_t اندازه = (اندازه_t)ظرفیت * حجم(*tok->ftstring_پشته);
        ftstring_state *پشته;
        إذا (tok->ftstring_پشته == tok->ftstring_پشته_inline) {
            پشته = PyMem_Malloc(اندازه);
            إذا (پشته != عدم) {
                کپی_حافظه(پشته, tok->ftstring_پشته,
                       (اندازه_t)tok->ftstring_عمق * حجم(*پشته));
            }
        }
        وإلا {
            پشته = PyMem_Realloc(tok->ftstring_پشته, اندازه);
        }
        إذا (پشته == عدم) {
            Py_Err_No_حافظه();
            tok->انجامشده = E_NOMEM;
            أرجع عدم;
        }
        tok->ftstring_پشته = پشته;
        tok->ftstring_ظرفیت = ظرفیت;
    }
    ftstring_state *state = &tok->ftstring_پشته[tok->ftstring_عمق];
    tok->ftstring_عمق = بعدی_عمق;
    *state = (ftstring_state){0};
    أرجع state;
}

فراغ
Py_Lexer_برداشت_FTString(بنية tok_state *tok)
{
    ftstring_state *state = _PyLexer_CurrentFTString(tok);
    أكد(state != عدم);
    PyMem_Free(state->comments);
    tok->ftstring_عمق--;
}

/* Free a tok_state structure */
فراغ
_PyTokenizer_Free(بنية tok_state *tok)
{
    Py_XDECREF(tok->filename);
    Py_XDECREF(tok->module);
    _PyTok_ReaderFree(tok);
    Py_Tok_منبع_Clear(&tok->منبع);
    لكل (صحيح i = 0; i < tok->ftstring_عمق; i++) {
        PyMem_Free(tok->ftstring_پشته[i].comments);
    }
    إذا (tok->ftstring_پشته != tok->ftstring_پشته_inline) {
        PyMem_Free(tok->ftstring_پشته);
    }
    PyMem_Free(tok);
}

فراغ
_PyToken_Init(بنية token *token) {
    *token = (بنية token){
        .type = -1,
        .بازه = {-1, -1},
        .شروع_loc = {-1, -1},
        .انتها_loc = {-1, -1},
    };
}

صحيح
Py_Lexer_token_راهاندازی(بنية tok_state *tok, بنية token *token, صحيح type, _PyTok_Off شروع, _PyTok_Off انتها)
{
    token->level = tok->level;
    token->is_raw = 0;
    أكد((شروع == -1 && انتها == -1) || (شروع >= 0 && انتها >= شروع));
    token->بازه = (Py_Tok_بازه){شروع, انتها};
    إذا (شروع >= 0) {
        token->شروع_loc = tok->شروع_loc;
        token->انتها_loc = (_PyTok_Loc){tok->lineno, Py_Lexer_Byte_ستونی(tok)};
    }
    وإلا {
        token->شروع_loc = token->انتها_loc = (_PyTok_Loc){tok->lineno, -1};
    }
    أرجع type;
}
