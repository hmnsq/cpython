#شامل "Python.h"
#شامل "pycore_token.h"
#شامل "errcode.h"

#شامل "state.h"
#شامل "../tokenizer/helpers.h"
#شامل "../tokenizer/reader.h"

ftstring_state *
Py_Lexer_افزودن_FTString(ساختار tok_state *tok)
{
    أكد(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    صحیح next_depth = tok->ftstring_depth + 1;
    اگر (next_depth >= MAXFTSTRINGLEVEL) {
        _PyTokenizer_syntaxerror(
            tok, "too many nested f-strings or t-strings");
        بازگشت پوچ;
    }
    اگر (tok->ftstring_depth == tok->ftstring_capacity) {
        صحیح capacity = Py_MIN(Py_MAX(tok->ftstring_capacity * 2, 4),
                              MAXFTSTRINGLEVEL);
        size_t size = (size_t)capacity * اندازه(*tok->ftstring_پشته);
        ftstring_state *پشته;
        اگر (tok->ftstring_پشته == tok->ftstring_پشته_inline) {
            پشته = PyMem_Malloc(size);
            اگر (پشته != پوچ) {
                memcpy(پشته, tok->ftstring_پشته,
                       (size_t)tok->ftstring_depth * اندازه(*پشته));
            }
        }
        وگرنه {
            پشته = PyMem_Realloc(tok->ftstring_پشته, size);
        }
        اگر (پشته == پوچ) {
            PyErr_NoMemory();
            tok->done = E_NOMEM;
            بازگشت پوچ;
        }
        tok->ftstring_پشته = پشته;
        tok->ftstring_capacity = capacity;
    }
    ftstring_state *state = &tok->ftstring_پشته[tok->ftstring_depth];
    tok->ftstring_depth = next_depth;
    *state = (ftstring_state){0};
    بازگشت state;
}

تهی
Py_Lexer_برداشت_FTString(ساختار tok_state *tok)
{
    ftstring_state *state = _PyLexer_CurrentFTString(tok);
    أكد(state != پوچ);
    PyMem_Free(state->comments);
    tok->ftstring_depth--;
}

/* Free a tok_state structure */
تهی
_PyTokenizer_Free(ساختار tok_state *tok)
{
    Py_XDECREF(tok->filename);
    Py_XDECREF(tok->module);
    _PyTok_ReaderFree(tok);
    _PyTok_SourceClear(&tok->source);
    برای (صحیح i = 0; i < tok->ftstring_depth; i++) {
        PyMem_Free(tok->ftstring_پشته[i].comments);
    }
    اگر (tok->ftstring_پشته != tok->ftstring_پشته_inline) {
        PyMem_Free(tok->ftstring_پشته);
    }
    PyMem_Free(tok);
}

تهی
_PyToken_Init(ساختار token *token) {
    *token = (ساختار token){
        .type = -1,
        .span = {-1, -1},
        .start_loc = {-1, -1},
        .end_loc = {-1, -1},
    };
}

صحیح
_PyLexer_token_setup(ساختار tok_state *tok, ساختار token *token, صحیح type, _PyTok_Off start, _PyTok_Off end)
{
    token->level = tok->level;
    token->is_raw = 0;
    أكد((start == -1 && end == -1) || (start >= 0 && end >= start));
    token->span = (_PyTok_Span){start, end};
    اگر (start >= 0) {
        token->start_loc = tok->start_loc;
        token->end_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};
    }
    وگرنه {
        token->start_loc = token->end_loc = (_PyTok_Loc){tok->lineno, -1};
    }
    بازگشت type;
}
