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
    정수 next_depth = tok->ftstring_depth + 1;
    만약 (next_depth >= MAXFTSTRINGLEVEL) {
        _PyTokenizer_syntaxerror(
            tok, "too many nested f-strings or t-strings");
        반환 NULL;
    }
    만약 (tok->ftstring_depth == tok->ftstring_capacity) {
        정수 capacity = Py_MIN(Py_MAX(tok->ftstring_capacity * 2, 4),
                              MAXFTSTRINGLEVEL);
        size_t size = (size_t)capacity * sizeof(*tok->ftstring_stack);
        ftstring_state *stack;
        만약 (tok->ftstring_stack == tok->ftstring_stack_inline) {
            stack = PyMem_Malloc(size);
            만약 (stack != NULL) {
                memcpy(stack, tok->ftstring_stack,
                       (size_t)tok->ftstring_depth * sizeof(*stack));
            }
        }
        아니면 {
            stack = PyMem_Realloc(tok->ftstring_stack, size);
        }
        만약 (stack == NULL) {
            PyErr_NoMemory();
            tok->done = E_NOMEM;
            반환 NULL;
        }
        tok->ftstring_stack = stack;
        tok->ftstring_capacity = capacity;
    }
    ftstring_state *state = &tok->ftstring_stack[tok->ftstring_depth];
    tok->ftstring_depth = next_depth;
    *state = (ftstring_state){0};
    반환 state;
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
    위해 (정수 i = 0; i < tok->ftstring_depth; i++) {
        PyMem_Free(tok->ftstring_stack[i].comments);
    }
    만약 (tok->ftstring_stack != tok->ftstring_stack_inline) {
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

정수
_PyLexer_token_setup(struct tok_state *tok, struct token *token, 정수 type, _PyTok_Off start, _PyTok_Off end)
{
    token->level = tok->level;
    token->is_raw = 0;
    assert((start == -1 && end == -1) || (start >= 0 && end >= start));
    token->span = (_PyTok_Span){start, end};
    만약 (start >= 0) {
        token->start_loc = tok->start_loc;
        token->end_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};
    }
    아니면 {
        token->start_loc = token->end_loc = (_PyTok_Loc){tok->lineno, -1};
    }
    반환 type;
}
