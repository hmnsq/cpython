#ضمن "Python.h"
#ضمن "pycore_token.h"
#ضمن "errcode.h"

#ضمن "state.h"
#ضمن "../tokenizer/helpers.h"
#ضمن "../tokenizer/reader.h"

ftstring_state *
Py_Lexer_ټېل_FTString(بنية tok_state *tok)
{
    أكد(tok->ftstring_ژوروالی >= 0 && tok->ftstring_ژوروالی <= tok->ftstring_ظرفیت);
    صحيح بل_ژوروالی = tok->ftstring_ژوروالی + 1;
    إذا (بل_ژوروالی >= MAXFTSTRINGLEVEL) {
        _PyTokenizer_syntaxerror(
            tok, "too many nested f-strings or t-strings");
        أرجع عدم;
    }
    إذا (tok->ftstring_ژوروالی == tok->ftstring_ظرفیت) {
        صحيح ظرفیت = Py_MIN(Py_MAX(tok->ftstring_ظرفیت * 2, 4),
                              MAXFTSTRINGLEVEL);
        اندازه_t اندازه = (اندازه_t)ظرفیت * حجم(*tok->ftstring_ډېری);
        ftstring_state *ډېری;
        إذا (tok->ftstring_ډېری == tok->ftstring_ډېری_inline) {
            ډېری = PyMem_Malloc(اندازه);
            إذا (ډېری != عدم) {
                حافظه_نقل(ډېری, tok->ftstring_ډېری,
                       (اندازه_t)tok->ftstring_ژوروالی * حجم(*ډېری));
            }
        }
        وإلا {
            ډېری = PyMem_Realloc(tok->ftstring_ډېری, اندازه);
        }
        إذا (ډېری == عدم) {
            Py_Err_No_حافظه();
            tok->بشپړ = E_NOMEM;
            أرجع عدم;
        }
        tok->ftstring_ډېری = ډېری;
        tok->ftstring_ظرفیت = ظرفیت;
    }
    ftstring_state *state = &tok->ftstring_ډېری[tok->ftstring_ژوروالی];
    tok->ftstring_ژوروالی = بل_ژوروالی;
    *state = (ftstring_state){0};
    أرجع state;
}

فراغ
Py_Lexer_راوباسه_FTString(بنية tok_state *tok)
{
    ftstring_state *state = _PyLexer_CurrentFTString(tok);
    أكد(state != عدم);
    PyMem_Free(state->comments);
    tok->ftstring_ژوروالی--;
}

/* Free a tok_state structure */
فراغ
_PyTokenizer_Free(بنية tok_state *tok)
{
    Py_XDECREF(tok->filename);
    Py_XDECREF(tok->module);
    _PyTok_ReaderFree(tok);
    Py_Tok_سرچینه_Clear(&tok->سرچینه);
    لكل (صحيح i = 0; i < tok->ftstring_ژوروالی; i++) {
        PyMem_Free(tok->ftstring_ډېری[i].comments);
    }
    إذا (tok->ftstring_ډېری != tok->ftstring_ډېری_inline) {
        PyMem_Free(tok->ftstring_ډېری);
    }
    PyMem_Free(tok);
}

فراغ
_PyToken_Init(بنية token *token) {
    *token = (بنية token){
        .type = -1,
        .پراختیا = {-1, -1},
        .پیل_loc = {-1, -1},
        .پای_ټکی_loc = {-1, -1},
    };
}

صحيح
Py_Lexer_token_تیاری(بنية tok_state *tok, بنية token *token, صحيح type, _PyTok_Off پیل, _PyTok_Off پای_ټکی)
{
    token->level = tok->level;
    token->is_raw = 0;
    أكد((پیل == -1 && پای_ټکی == -1) || (پیل >= 0 && پای_ټکی >= پیل));
    token->پراختیا = (Py_Tok_پراختیا){پیل, پای_ټکی};
    إذا (پیل >= 0) {
        token->پیل_loc = tok->پیل_loc;
        token->پای_ټکی_loc = (_PyTok_Loc){tok->lineno, Py_Lexer_Byte_ستنه(tok)};
    }
    وإلا {
        token->پیل_loc = token->پای_ټکی_loc = (_PyTok_Loc){tok->lineno, -1};
    }
    أرجع type;
}
