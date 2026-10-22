#include "Python.h"
#include "pycore_token.h"
#include "errcode.h"

#include "state.h"
#include "../tokenizer/helpers.h"
#include "../tokenizer/reader.h"

ftstring_state *
_PyLexer_PushFTString(sa_nracanaa tok_state *tok)
{
    assert(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    ຈຳນວນເຕັມ next_depth = tok->ftstring_depth + 1;
    ຖ້າ (next_depth >= MAXFTSTRINGLEVEL) {
        _PyTokenizer_syntaxerror(
            tok, "too many nested f-strings or t-strings");
        ສົ່ງຄືນ NULL;
    }
    ຖ້າ (tok->ftstring_depth == tok->ftstring_capacity) {
        ຈຳນວນເຕັມ capacity = Py_MIN(Py_MAX(tok->ftstring_capacity * 2, 4),
                              MAXFTSTRINGLEVEL);
        size_t size = (size_t)capacity * parimaa_na(*tok->ftstring_stack);
        ftstring_state *stack;
        ຖ້າ (tok->ftstring_stack == tok->ftstring_stack_inline) {
            stack = PyMem_Malloc(size);
            ຖ້າ (stack != NULL) {
                memcpy(stack, tok->ftstring_stack,
                       (size_t)tok->ftstring_depth * parimaa_na(*stack));
            }
        }
        ບໍ່ດັ່ງນັ້ນ {
            stack = PyMem_Realloc(tok->ftstring_stack, size);
        }
        ຖ້າ (stack == NULL) {
            PyErr_NoMemory();
            tok->done = E_NOMEM;
            ສົ່ງຄືນ NULL;
        }
        tok->ftstring_stack = stack;
        tok->ftstring_capacity = capacity;
    }
    ftstring_state *state = &tok->ftstring_stack[tok->ftstring_depth];
    tok->ftstring_depth = next_depth;
    *state = (ftstring_state){0};
    ສົ່ງຄືນ state;
}

void
_PyLexer_PopFTString(sa_nracanaa tok_state *tok)
{
    ftstring_state *state = _PyLexer_CurrentFTString(tok);
    assert(state != NULL);
    PyMem_Free(state->comments);
    tok->ftstring_depth--;
}

/* Free a tok_state structure */
void
_PyTokenizer_Free(sa_nracanaa tok_state *tok)
{
    Py_XDECREF(tok->filename);
    Py_XDECREF(tok->module);
    _PyTok_ReaderFree(tok);
    _PyTok_SourceClear(&tok->source);
    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < tok->ftstring_depth; i++) {
        PyMem_Free(tok->ftstring_stack[i].comments);
    }
    ຖ້າ (tok->ftstring_stack != tok->ftstring_stack_inline) {
        PyMem_Free(tok->ftstring_stack);
    }
    PyMem_Free(tok);
}

void
_PyToken_Init(sa_nracanaa token *token) {
    *token = (sa_nracanaa token){
        .type = -1,
        .span = {-1, -1},
        .start_loc = {-1, -1},
        .end_loc = {-1, -1},
    };
}

ຈຳນວນເຕັມ
_PyLexer_token_setup(sa_nracanaa tok_state *tok, sa_nracanaa token *token, ຈຳນວນເຕັມ type, _PyTok_Off start, _PyTok_Off end)
{
    token->level = tok->level;
    token->is_raw = 0;
    assert((start == -1 && end == -1) || (start >= 0 && end >= start));
    token->span = (_PyTok_Span){start, end};
    ຖ້າ (start >= 0) {
        token->start_loc = tok->start_loc;
        token->end_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};
    }
    ບໍ່ດັ່ງນັ້ນ {
        token->start_loc = token->end_loc = (_PyTok_Loc){tok->lineno, -1};
    }
    ສົ່ງຄືນ type;
}
