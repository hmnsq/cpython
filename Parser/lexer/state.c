#include "Python.h"
#include "pycore_token.h"
#include "errcode.h"

#include "state.h"
#include "../tokenizer/helpers.h"
#include "../tokenizer/reader.h"

ftstring_state *
_PyLexer_PushFTString(struct tok_state *tok)
{
    assert(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    số_nguyên next_depth = tok->ftstring_depth + 1;
    nếu (next_depth >= MAXFTSTRINGLEVEL) {
        _PyTokenizer_syntaxerror(
            tok, "too many nested f-strings or t-strings");
        trả_về NULL;
    }
    nếu (tok->ftstring_depth == tok->ftstring_capacity) {
        số_nguyên capacity = Py_MIN(Py_MAX(tok->ftstring_capacity * 2, 4),
                              MAXFTSTRINGLEVEL);
        size_t size = (size_t)capacity * sizeof(*tok->ftstring_stack);
        ftstring_state *stack;
        nếu (tok->ftstring_stack == tok->ftstring_stack_inline) {
            stack = PyMem_Malloc(size);
            nếu (stack != NULL) {
                memcpy(stack, tok->ftstring_stack,
                       (size_t)tok->ftstring_depth * sizeof(*stack));
            }
        }
        không_thì {
            stack = PyMem_Realloc(tok->ftstring_stack, size);
        }
        nếu (stack == NULL) {
            PyErr_NoMemory();
            tok->done = E_NOMEM;
            trả_về NULL;
        }
        tok->ftstring_stack = stack;
        tok->ftstring_capacity = capacity;
    }
    ftstring_state *state = &tok->ftstring_stack[tok->ftstring_depth];
    tok->ftstring_depth = next_depth;
    *state = (ftstring_state){0};
    trả_về state;
}

void
_PyLexer_PopFTString(struct tok_state *tok)
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
    cho (số_nguyên i = 0; i < tok->ftstring_depth; i++) {
        PyMem_Free(tok->ftstring_stack[i].comments);
    }
    nếu (tok->ftstring_stack != tok->ftstring_stack_inline) {
        PyMem_Free(tok->ftstring_stack);
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

số_nguyên
_PyLexer_token_setup(struct tok_state *tok, struct token *token, số_nguyên type, _PyTok_Off start, _PyTok_Off end)
{
    token->level = tok->level;
    token->is_raw = 0;
    assert((start == -1 && end == -1) || (start >= 0 && end >= start));
    token->span = (_PyTok_Span){start, end};
    nếu (start >= 0) {
        token->start_loc = tok->start_loc;
        token->end_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};
    }
    không_thì {
        token->start_loc = token->end_loc = (_PyTok_Loc){tok->lineno, -1};
    }
    trả_về type;
}
