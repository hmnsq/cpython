#অন্তর্ভাব "Python.h"
#অন্তর্ভাব "pycore_token.h"
#অন্তর্ভাব "errcode.h"

#অন্তর্ভাব "state.h"
#অন্তর্ভাব "../tokenizer/helpers.h"
#অন্তর্ভাব "../tokenizer/reader.h"

ftstring_state *
Py_Lexer_ঠেলো_FTString(কাঠামো tok_state *tok)
{
    প্রতিজ্ঞা(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    পূর্ণসংখ্যা পরবর্তী_depth = tok->ftstring_depth + 1;
    যদি (পরবর্তী_depth >= MAXFTSTRINGLEVEL) {
        _PyTokenizer_syntaxerror(
            tok, "too many nested f-strings or t-strings");
        ফেরত ফাঁকা;
    }
    যদি (tok->ftstring_depth == tok->ftstring_capacity) {
        পূর্ণসংখ্যা capacity = Py_MIN(Py_MAX(tok->ftstring_capacity * 2, 4),
                              MAXFTSTRINGLEVEL);
        আকার_t আকার_2 = (আকার_t)capacity * আকার(*tok->ftstring_স্তূপ);
        ftstring_state *স্তূপ;
        যদি (tok->ftstring_স্তূপ == tok->ftstring_স্তূপ_inline) {
            স্তূপ = PyMem_Malloc(আকার_2);
            যদি (স্তূপ != ফাঁকা) {
                memcpy(স্তূপ, tok->ftstring_স্তূপ,
                       (আকার_t)tok->ftstring_depth * আকার(*স্তূপ));
            }
        }
        অন্যথা {
            স্তূপ = PyMem_Realloc(tok->ftstring_স্তূপ, আকার_2);
        }
        যদি (স্তূপ == ফাঁকা) {
            PyErr_NoMemory();
            tok->done = E_NOMEM;
            ফেরত ফাঁকা;
        }
        tok->ftstring_স্তূপ = স্তূপ;
        tok->ftstring_capacity = capacity;
    }
    ftstring_state *state = &tok->ftstring_স্তূপ[tok->ftstring_depth];
    tok->ftstring_depth = পরবর্তী_depth;
    *state = (ftstring_state){0};
    ফেরত state;
}

শূন্য
Py_Lexer_বের_করো_FTString(কাঠামো tok_state *tok)
{
    ftstring_state *state = _PyLexer_CurrentFTString(tok);
    প্রতিজ্ঞা(state != ফাঁকা);
    PyMem_Free(state->comments);
    tok->ftstring_depth--;
}

/* Free a tok_state structure */
শূন্য
_PyTokenizer_Free(কাঠামো tok_state *tok)
{
    Py_XDECREF(tok->filename);
    Py_XDECREF(tok->module);
    _PyTok_ReaderFree(tok);
    _PyTok_SourceClear(&tok->source);
    জন্য (পূর্ণসংখ্যা i = 0; i < tok->ftstring_depth; i++) {
        PyMem_Free(tok->ftstring_স্তূপ[i].comments);
    }
    যদি (tok->ftstring_স্তূপ != tok->ftstring_স্তূপ_inline) {
        PyMem_Free(tok->ftstring_স্তূপ);
    }
    PyMem_Free(tok);
}

শূন্য
_PyToken_Init(কাঠামো token *token) {
    *token = (কাঠামো token){
        .type = -1,
        .span = {-1, -1},
        .start_loc = {-1, -1},
        .end_loc = {-1, -1},
    };
}

পূর্ণসংখ্যা
_PyLexer_token_setup(কাঠামো tok_state *tok, কাঠামো token *token, পূর্ণসংখ্যা type, _PyTok_Off start, _PyTok_Off end)
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
    ফেরত type;
}
