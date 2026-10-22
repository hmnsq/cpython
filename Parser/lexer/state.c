#અન્તર્ભાવ "Python.h"
#અન્તર્ભાવ "pycore_token.h"
#અન્તર્ભાવ "errcode.h"

#અન્તર્ભાવ "state.h"
#અન્તર્ભાવ "../tokenizer/helpers.h"
#અન્તર્ભાવ "../tokenizer/reader.h"

ftstring_state *
Py_Lexer_ધકેલો_FTString(સંરચના tok_state *tok)
{
    પ્રતિજ્ઞા(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    પૂર્ણાંક next_depth = tok->ftstring_depth + 1;
    જો (next_depth >= MAXFTSTRINGLEVEL) {
        _PyTokenizer_syntaxerror(
            tok, "too many nested f-strings or t-strings");
        પાછા ખાલી;
    }
    જો (tok->ftstring_depth == tok->ftstring_capacity) {
        પૂર્ણાંક capacity = Py_MIN(Py_MAX(tok->ftstring_capacity * 2, 4),
                              MAXFTSTRINGLEVEL);
        size_t size = (size_t)capacity * કદ(*tok->ftstring_ઢગલો);
        ftstring_state *ઢગલો;
        જો (tok->ftstring_ઢગલો == tok->ftstring_ઢગલો_inline) {
            ઢગલો = PyMem_Malloc(size);
            જો (ઢગલો != ખાલી) {
                memcpy(ઢગલો, tok->ftstring_ઢગલો,
                       (size_t)tok->ftstring_depth * કદ(*ઢગલો));
            }
        }
        નહીંતર {
            ઢગલો = PyMem_Realloc(tok->ftstring_ઢગલો, size);
        }
        જો (ઢગલો == ખાલી) {
            PyErr_NoMemory();
            tok->done = E_NOMEM;
            પાછા ખાલી;
        }
        tok->ftstring_ઢગલો = ઢગલો;
        tok->ftstring_capacity = capacity;
    }
    ftstring_state *state = &tok->ftstring_ઢગલો[tok->ftstring_depth];
    tok->ftstring_depth = next_depth;
    *state = (ftstring_state){0};
    પાછા state;
}

શૂન્ય
Py_Lexer_કાઢો_FTString(સંરચના tok_state *tok)
{
    ftstring_state *state = _PyLexer_CurrentFTString(tok);
    પ્રતિજ્ઞા(state != ખાલી);
    PyMem_Free(state->comments);
    tok->ftstring_depth--;
}

/* Free a tok_state structure */
શૂન્ય
_PyTokenizer_Free(સંરચના tok_state *tok)
{
    Py_XDECREF(tok->filename);
    Py_XDECREF(tok->module);
    _PyTok_ReaderFree(tok);
    _PyTok_SourceClear(&tok->source);
    માટે (પૂર્ણાંક i = 0; i < tok->ftstring_depth; i++) {
        PyMem_Free(tok->ftstring_ઢગલો[i].comments);
    }
    જો (tok->ftstring_ઢગલો != tok->ftstring_ઢગલો_inline) {
        PyMem_Free(tok->ftstring_ઢગલો);
    }
    PyMem_Free(tok);
}

શૂન્ય
_PyToken_Init(સંરચના token *token) {
    *token = (સંરચના token){
        .type = -1,
        .span = {-1, -1},
        .start_loc = {-1, -1},
        .end_loc = {-1, -1},
    };
}

પૂર્ણાંક
_PyLexer_token_setup(સંરચના tok_state *tok, સંરચના token *token, પૂર્ણાંક type, _PyTok_Off start, _PyTok_Off end)
{
    token->level = tok->level;
    token->is_raw = 0;
    પ્રતિજ્ઞા((start == -1 && end == -1) || (start >= 0 && end >= start));
    token->span = (_PyTok_Span){start, end};
    જો (start >= 0) {
        token->start_loc = tok->start_loc;
        token->end_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};
    }
    નહીંતર {
        token->start_loc = token->end_loc = (_PyTok_Loc){tok->lineno, -1};
    }
    પાછા type;
}
