#ضمن "Python.h"
#ضمن "pycore_token.h"
#ضمن "errcode.h"

#ضمن "state.h"
#ضمن "../tokenizer/helpers.h"
#ضمن "../tokenizer/reader.h"

ftstring_state *
Py_Lexer_ڈالو_FTString(بنية tok_state *tok)
{
    أكد(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    صحيح next_depth = tok->ftstring_depth + 1;
    إذا (next_depth >= MAXFTSTRINGLEVEL) {
        _PyTokenizer_syntaxerror(
            tok, "too many nested f-strings or t-strings");
        أرجع عدم;
    }
    إذا (tok->ftstring_depth == tok->ftstring_capacity) {
        صحيح capacity = Py_MIN(Py_MAX(tok->ftstring_capacity * 2, 4),
                              MAXFTSTRINGLEVEL);
        size_t size = (size_t)capacity * حجم(*tok->ftstring_ڈھیر);
        ftstring_state *ڈھیر;
        إذا (tok->ftstring_ڈھیر == tok->ftstring_ڈھیر_inline) {
            ڈھیر = PyMem_Malloc(size);
            إذا (ڈھیر != عدم) {
                memcpy(ڈھیر, tok->ftstring_ڈھیر,
                       (size_t)tok->ftstring_depth * حجم(*ڈھیر));
            }
        }
        وإلا {
            ڈھیر = PyMem_Realloc(tok->ftstring_ڈھیر, size);
        }
        إذا (ڈھیر == عدم) {
            PyErr_NoMemory();
            tok->done = E_NOMEM;
            أرجع عدم;
        }
        tok->ftstring_ڈھیر = ڈھیر;
        tok->ftstring_capacity = capacity;
    }
    ftstring_state *state = &tok->ftstring_ڈھیر[tok->ftstring_depth];
    tok->ftstring_depth = next_depth;
    *state = (ftstring_state){0};
    أرجع state;
}

فراغ
Py_Lexer_نکالو_FTString(بنية tok_state *tok)
{
    ftstring_state *state = _PyLexer_CurrentFTString(tok);
    أكد(state != عدم);
    PyMem_Free(state->comments);
    tok->ftstring_depth--;
}

/* Free a tok_state structure */
فراغ
_PyTokenizer_Free(بنية tok_state *tok)
{
    Py_XDECREF(tok->filename);
    Py_XDECREF(tok->module);
    _PyTok_ReaderFree(tok);
    _PyTok_SourceClear(&tok->source);
    لكل (صحيح i = 0; i < tok->ftstring_depth; i++) {
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
        .span = {-1, -1},
        .start_loc = {-1, -1},
        .end_loc = {-1, -1},
    };
}

صحيح
_PyLexer_token_setup(بنية tok_state *tok, بنية token *token, صحيح type, _PyTok_Off start, _PyTok_Off end)
{
    token->level = tok->level;
    token->is_raw = 0;
    أكد((start == -1 && end == -1) || (start >= 0 && end >= start));
    token->span = (_PyTok_Span){start, end};
    إذا (start >= 0) {
        token->start_loc = tok->start_loc;
        token->end_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};
    }
    وإلا {
        token->start_loc = token->end_loc = (_PyTok_Loc){tok->lineno, -1};
    }
    أرجع type;
}
