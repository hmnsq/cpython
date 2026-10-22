#include "Python.h"
#include "pycore_token.h"
#include "errcode.h"

#include "state.h"
#include "../tokenizer/helpers.h"
#include "../tokenizer/reader.h"

ftstring_state *
Py_Lexer_इतुलाक_FTString(struct tok_state *tok)
{
    assert(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    int next_depth = tok->ftstring_depth + 1;
    if (next_depth >= MAXFTSTRINGLEVEL) {
        _PyTokenizer_syntaxerror(
            tok, "too many nested f-strings or t-strings");
        return NULL;
    }
    if (tok->ftstring_depth == tok->ftstring_capacity) {
        int capacity = Py_MIN(Py_MAX(tok->ftstring_capacity * 2, 4),
                              MAXFTSTRINGLEVEL);
        size_t size = (size_t)capacity * sizeof(*tok->ftstring_सालानसान);
        ftstring_state *सालानसान;
        if (tok->ftstring_सालानसान == tok->ftstring_सालानसान_inline) {
            सालानसान = PyMem_Malloc(size);
            if (सालानसान != NULL) {
                memcpy(सालानसान, tok->ftstring_सालानसान,
                       (size_t)tok->ftstring_depth * sizeof(*सालानसान));
            }
        }
        else {
            सालानसान = PyMem_Realloc(tok->ftstring_सालानसान, size);
        }
        if (सालानसान == NULL) {
            PyErr_NoMemory();
            tok->done = E_NOMEM;
            return NULL;
        }
        tok->ftstring_सालानसान = सालानसान;
        tok->ftstring_capacity = capacity;
    }
    ftstring_state *state = &tok->ftstring_सालानसान[tok->ftstring_depth];
    tok->ftstring_depth = next_depth;
    *state = (ftstring_state){0};
    return state;
}

void
Py_Lexer_आलिसिन_FTString(struct tok_state *tok)
{
    ftstring_state *state = _PyLexer_CurrentFTString(tok);
    assert(state != NULL);
    PyMem_Free(state->comments);
    tok->ftstring_depth--;
}

/* Free a tok_state structure */
void
_PyTokenizer_Free(struct tok_state *tok)
{
    Py_XDECREF(tok->filename);
    Py_XDECREF(tok->module);
    _PyTok_ReaderFree(tok);
    _PyTok_SourceClear(&tok->source);
    for (int i = 0; i < tok->ftstring_depth; i++) {
        PyMem_Free(tok->ftstring_सालानसान[i].comments);
    }
    if (tok->ftstring_सालानसान != tok->ftstring_सालानसान_inline) {
        PyMem_Free(tok->ftstring_सालानसान);
    }
    PyMem_Free(tok);
}

void
_PyToken_Init(struct token *token) {
    *token = (struct token){
        .type = -1,
        .span = {-1, -1},
        .start_loc = {-1, -1},
        .end_loc = {-1, -1},
    };
}

int
_PyLexer_token_setup(struct tok_state *tok, struct token *token, int type, _PyTok_Off start, _PyTok_Off end)
{
    token->level = tok->level;
    token->is_raw = 0;
    assert((start == -1 && end == -1) || (start >= 0 && end >= start));
    token->span = (_PyTok_Span){start, end};
    if (start >= 0) {
        token->start_loc = tok->start_loc;
        token->end_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};
    }
    else {
        token->start_loc = token->end_loc = (_PyTok_Loc){tok->lineno, -1};
    }
    return type;
}
