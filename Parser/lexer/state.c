#ଅନ୍ତର୍ଭାଵ "Python.h"
#ଅନ୍ତର୍ଭାଵ "pycore_token.h"
#ଅନ୍ତର୍ଭାଵ "errcode.h"

#ଅନ୍ତର୍ଭାଵ "state.h"
#ଅନ୍ତର୍ଭାଵ "../tokenizer/helpers.h"
#ଅନ୍ତର୍ଭାଵ "../tokenizer/reader.h"

ftstring_state *
Py_Lexer_ଠେଲ_FTString(ସଂରଚନା tok_state *tok)
{
    ପ୍ରତିଜ୍ଞା(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା next_depth = tok->ftstring_depth + 1;
    ଯଦି (next_depth >= MAXFTSTRINGLEVEL) {
        _PyTokenizer_syntaxerror(
            tok, "too many nested f-strings or t-strings");
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (tok->ftstring_depth == tok->ftstring_capacity) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା capacity = Py_MIN(Py_MAX(tok->ftstring_capacity * 2, 4),
                              MAXFTSTRINGLEVEL);
        size_t size = (size_t)capacity * ଆକାର(*tok->ftstring_ଗଦା);
        ftstring_state *ଗଦା;
        ଯଦି (tok->ftstring_ଗଦା == tok->ftstring_ଗଦା_inline) {
            ଗଦା = PyMem_Malloc(size);
            ଯଦି (ଗଦା != ଖାଲି) {
                memcpy(ଗଦା, tok->ftstring_ଗଦା,
                       (size_t)tok->ftstring_depth * ଆକାର(*ଗଦା));
            }
        }
        ଅନ୍ୟଥା {
            ଗଦା = PyMem_Realloc(tok->ftstring_ଗଦା, size);
        }
        ଯଦି (ଗଦା == ଖାଲି) {
            PyErr_NoMemory();
            tok->done = E_NOMEM;
            ଫେରନ୍ତୁ ଖାଲି;
        }
        tok->ftstring_ଗଦା = ଗଦା;
        tok->ftstring_capacity = capacity;
    }
    ftstring_state *state = &tok->ftstring_ଗଦା[tok->ftstring_depth];
    tok->ftstring_depth = next_depth;
    *state = (ftstring_state){0};
    ଫେରନ୍ତୁ state;
}

ଶୂନ୍ୟ
Py_Lexer_କାଢ଼_FTString(ସଂରଚନା tok_state *tok)
{
    ftstring_state *state = _PyLexer_CurrentFTString(tok);
    ପ୍ରତିଜ୍ଞା(state != ଖାଲି);
    PyMem_Free(state->comments);
    tok->ftstring_depth--;
}

/* Free a tok_state structure */
ଶୂନ୍ୟ
_PyTokenizer_Free(ସଂରଚନା tok_state *tok)
{
    Py_XDECREF(tok->filename);
    Py_XDECREF(tok->module);
    _PyTok_ReaderFree(tok);
    _PyTok_SourceClear(&tok->source);
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < tok->ftstring_depth; i++) {
        PyMem_Free(tok->ftstring_ଗଦା[i].comments);
    }
    ଯଦି (tok->ftstring_ଗଦା != tok->ftstring_ଗଦା_inline) {
        PyMem_Free(tok->ftstring_ଗଦା);
    }
    PyMem_Free(tok);
}

ଶୂନ୍ୟ
_PyToken_Init(ସଂରଚନା token *token) {
    *token = (ସଂରଚନା token){
        .type = -1,
        .span = {-1, -1},
        .start_loc = {-1, -1},
        .end_loc = {-1, -1},
    };
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyLexer_token_setup(ସଂରଚନା tok_state *tok, ସଂରଚନା token *token, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, _PyTok_Off start, _PyTok_Off end)
{
    token->level = tok->level;
    token->is_raw = 0;
    ପ୍ରତିଜ୍ଞା((start == -1 && end == -1) || (start >= 0 && end >= start));
    token->span = (_PyTok_Span){start, end};
    ଯଦି (start >= 0) {
        token->start_loc = tok->start_loc;
        token->end_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};
    }
    ଅନ୍ୟଥା {
        token->start_loc = token->end_loc = (_PyTok_Loc){tok->lineno, -1};
    }
    ଫେରନ୍ତୁ type;
}
