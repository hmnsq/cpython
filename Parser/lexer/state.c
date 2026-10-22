#ضمن "Python.h"
#ضمن "pycore_token.h"
#ضمن "errcode.h"

#ضمن "state.h"
#ضمن "../tokenizer/helpers.h"
#ضمن "../tokenizer/reader.h"

ftstring_state *
Py_Lexer_ڌڪو_FTString(بنية tok_state *tok)
{
    أكد(tok->ftstring_اونهائي >= 0 && tok->ftstring_اونهائي <= tok->ftstring_صلاحيت);
    صحيح ايندڙ_اونهائي = tok->ftstring_اونهائي + 1;
    إذا (ايندڙ_اونهائي >= MAXFTSTRINGLEVEL) {
        _PyTokenizer_syntaxerror(
            tok, "too many nested f-strings or t-strings");
        أرجع عدم;
    }
    إذا (tok->ftstring_اونهائي == tok->ftstring_صلاحيت) {
        صحيح صلاحيت = Py_MIN(Py_MAX(tok->ftstring_صلاحيت * 2, 4),
                              MAXFTSTRINGLEVEL);
        سائيز_t سائيز = (سائيز_t)صلاحيت * حجم(*tok->ftstring_ڍير);
        ftstring_state *ڍير;
        إذا (tok->ftstring_ڍير == tok->ftstring_ڍير_inline) {
            ڍير = PyMem_Malloc(سائيز);
            إذا (ڍير != عدم) {
                يادگيري_نقل(ڍير, tok->ftstring_ڍير,
                       (سائيز_t)tok->ftstring_اونهائي * حجم(*ڍير));
            }
        }
        وإلا {
            ڍير = PyMem_Realloc(tok->ftstring_ڍير, سائيز);
        }
        إذا (ڍير == عدم) {
            Py_Err_No_يادگيري();
            tok->پورو = E_NOMEM;
            أرجع عدم;
        }
        tok->ftstring_ڍير = ڍير;
        tok->ftstring_صلاحيت = صلاحيت;
    }
    ftstring_state *state = &tok->ftstring_ڍير[tok->ftstring_اونهائي];
    tok->ftstring_اونهائي = ايندڙ_اونهائي;
    *state = (ftstring_state){0};
    أرجع state;
}

فراغ
Py_Lexer_ڪڍو_FTString(بنية tok_state *tok)
{
    ftstring_state *state = _PyLexer_CurrentFTString(tok);
    أكد(state != عدم);
    PyMem_Free(state->comments);
    tok->ftstring_اونهائي--;
}

/* Free a tok_state structure */
فراغ
_PyTokenizer_Free(بنية tok_state *tok)
{
    Py_XDECREF(tok->filename);
    Py_XDECREF(tok->module);
    _PyTok_ReaderFree(tok);
    Py_Tok_ذريعو_Clear(&tok->ذريعو);
    لكل (صحيح i = 0; i < tok->ftstring_اونهائي; i++) {
        PyMem_Free(tok->ftstring_ڍير[i].comments);
    }
    إذا (tok->ftstring_ڍير != tok->ftstring_ڍير_inline) {
        PyMem_Free(tok->ftstring_ڍير);
    }
    PyMem_Free(tok);
}

فراغ
_PyToken_Init(بنية token *token) {
    *token = (بنية token){
        .type = -1,
        .وسعت = {-1, -1},
        .شروعات_loc = {-1, -1},
        .پڇاڙي_loc = {-1, -1},
    };
}

صحيح
Py_Lexer_token_تياري(بنية tok_state *tok, بنية token *token, صحيح type, _PyTok_Off شروعات, _PyTok_Off پڇاڙي)
{
    token->level = tok->level;
    token->is_raw = 0;
    أكد((شروعات == -1 && پڇاڙي == -1) || (شروعات >= 0 && پڇاڙي >= شروعات));
    token->وسعت = (Py_Tok_وسعت){شروعات, پڇاڙي};
    إذا (شروعات >= 0) {
        token->شروعات_loc = tok->شروعات_loc;
        token->پڇاڙي_loc = (_PyTok_Loc){tok->lineno, Py_Lexer_Byte_ڪالم(tok)};
    }
    وإلا {
        token->شروعات_loc = token->پڇاڙي_loc = (_PyTok_Loc){tok->lineno, -1};
    }
    أرجع type;
}
