#অন্তর্ভাব "Python.h"
#অন্তর্ভাব "pycore_token.h"
#অন্তর্ভাব "errcode.h"

#অন্তর্ভাব "state.h"
#অন্তর্ভাব "../tokenizer/helpers.h"
#অন্তর্ভাব "../tokenizer/reader.h"

ftstring_state *
Py_Lexer_ঠেলো_FTString(সংরচনা tok_state *tok)
{
    প্রতিজ্ঞা(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    পূর্ণাঙ্ক next_depth = tok->ftstring_depth + 1;
    যদি (next_depth >= MAXFTSTRINGLEVEL) {
        _PyTokenizer_syntaxerror(
            tok, "too many nested f-strings or t-strings");
        প্রত্যাগম রিক্ত;
    }
    যদি (tok->ftstring_depth == tok->ftstring_capacity) {
        পূর্ণাঙ্ক capacity = Py_MIN(Py_MAX(tok->ftstring_capacity * 2, 4),
                              MAXFTSTRINGLEVEL);
        size_t size = (size_t)capacity * পরিমাণ(*tok->ftstring_স্তূপ);
        ftstring_state *স্তূপ;
        যদি (tok->ftstring_স্তূপ == tok->ftstring_স্তূপ_inline) {
            স্তূপ = PyMem_Malloc(size);
            যদি (স্তূপ != রিক্ত) {
                memcpy(স্তূপ, tok->ftstring_স্তূপ,
                       (size_t)tok->ftstring_depth * পরিমাণ(*স্তূপ));
            }
        }
        অন্যথা {
            স্তূপ = PyMem_Realloc(tok->ftstring_স্তূপ, size);
        }
        যদি (স্তূপ == রিক্ত) {
            PyErr_NoMemory();
            tok->done = E_NOMEM;
            প্রত্যাগম রিক্ত;
        }
        tok->ftstring_স্তূপ = স্তূপ;
        tok->ftstring_capacity = capacity;
    }
    ftstring_state *state = &tok->ftstring_স্তূপ[tok->ftstring_depth];
    tok->ftstring_depth = next_depth;
    *state = (ftstring_state){0};
    প্রত্যাগম state;
}

শূন্য
Py_Lexer_বের_করো_FTString(সংরচনা tok_state *tok)
{
    ftstring_state *state = _PyLexer_CurrentFTString(tok);
    প্রতিজ্ঞা(state != রিক্ত);
    PyMem_Free(state->comments);
    tok->ftstring_depth--;
}

/* Free a tok_state structure */
শূন্য
_PyTokenizer_Free(সংরচনা tok_state *tok)
{
    Py_XDECREF(tok->filename);
    Py_XDECREF(tok->module);
    _PyTok_ReaderFree(tok);
    _PyTok_SourceClear(&tok->source);
    প্রতি (পূর্ণাঙ্ক i = 0; i < tok->ftstring_depth; i++) {
        PyMem_Free(tok->ftstring_স্তূপ[i].comments);
    }
    যদি (tok->ftstring_স্তূপ != tok->ftstring_স্তূপ_inline) {
        PyMem_Free(tok->ftstring_স্তূপ);
    }
    PyMem_Free(tok);
}

শূন্য
_PyToken_Init(সংরচনা token *token) {
    *token = (সংরচনা token){
        .type = -1,
        .span = {-1, -1},
        .start_loc = {-1, -1},
        .end_loc = {-1, -1},
    };
}

পূর্ণাঙ্ক
_PyLexer_token_setup(সংরচনা tok_state *tok, সংরচনা token *token, পূর্ণাঙ্ক type, _PyTok_Off start, _PyTok_Off end)
{
    token->level = tok->level;
    token->is_raw = 0;
    প্রতিজ্ঞা((start == -1 && end == -1) || (start >= 0 && end >= start));
    token->span = (_PyTok_Span){start, end};
    যদি (start >= 0) {
        token->start_loc = tok->start_loc;
        token->end_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};
    }
    অন্যথা {
        token->start_loc = token->end_loc = (_PyTok_Loc){tok->lineno, -1};
    }
    প্রত্যাগম type;
}
