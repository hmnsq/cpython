#ಅನ್ತರ್ಭಾವ "Python.h"
#ಅನ್ತರ್ಭಾವ "pycore_token.h"
#ಅನ್ತರ್ಭಾವ "errcode.h"

#ಅನ್ತರ್ಭಾವ "state.h"
#ಅನ್ತರ್ಭಾವ "../tokenizer/helpers.h"
#ಅನ್ತರ್ಭಾವ "../tokenizer/reader.h"

ftstring_state *
Py_Lexer_ತಳ್ಳು_FTString(ಸಂರಚನಾ tok_state *tok)
{
    ಪ್ರತಿಜ್ಞಾ(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    ಪೂರ್ಣಾಙ್ಕ next_depth = tok->ftstring_depth + 1;
    ಯದಿ (next_depth >= MAXFTSTRINGLEVEL) {
        _PyTokenizer_syntaxerror(
            tok, "too many nested f-strings or t-strings");
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (tok->ftstring_depth == tok->ftstring_capacity) {
        ಪೂರ್ಣಾಙ್ಕ capacity = Py_MIN(Py_MAX(tok->ftstring_capacity * 2, 4),
                              MAXFTSTRINGLEVEL);
        size_t size = (size_t)capacity * ಪರಿಮಾಣ(*tok->ftstring_ರಾಶಿ);
        ftstring_state *ರಾಶಿ;
        ಯದಿ (tok->ftstring_ರಾಶಿ == tok->ftstring_ರಾಶಿ_inline) {
            ರಾಶಿ = PyMem_Malloc(size);
            ಯದಿ (ರಾಶಿ != ರಿಕ್ತ) {
                memcpy(ರಾಶಿ, tok->ftstring_ರಾಶಿ,
                       (size_t)tok->ftstring_depth * ಪರಿಮಾಣ(*ರಾಶಿ));
            }
        }
        ಅನ್ಯಥಾ {
            ರಾಶಿ = PyMem_Realloc(tok->ftstring_ರಾಶಿ, size);
        }
        ಯದಿ (ರಾಶಿ == ರಿಕ್ತ) {
            PyErr_NoMemory();
            tok->done = E_NOMEM;
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
        tok->ftstring_ರಾಶಿ = ರಾಶಿ;
        tok->ftstring_capacity = capacity;
    }
    ftstring_state *state = &tok->ftstring_ರಾಶಿ[tok->ftstring_depth];
    tok->ftstring_depth = next_depth;
    *state = (ftstring_state){0};
    ಪ್ರತ್ಯಾಗಮ state;
}

ಶೂನ್ಯ
Py_Lexer_ತೆಗೆ_FTString(ಸಂರಚನಾ tok_state *tok)
{
    ftstring_state *state = _PyLexer_CurrentFTString(tok);
    ಪ್ರತಿಜ್ಞಾ(state != ರಿಕ್ತ);
    PyMem_Free(state->comments);
    tok->ftstring_depth--;
}

/* Free a tok_state structure */
ಶೂನ್ಯ
_PyTokenizer_Free(ಸಂರಚನಾ tok_state *tok)
{
    Py_XDECREF(tok->filename);
    Py_XDECREF(tok->module);
    _PyTok_ReaderFree(tok);
    _PyTok_SourceClear(&tok->source);
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < tok->ftstring_depth; i++) {
        PyMem_Free(tok->ftstring_ರಾಶಿ[i].comments);
    }
    ಯದಿ (tok->ftstring_ರಾಶಿ != tok->ftstring_ರಾಶಿ_inline) {
        PyMem_Free(tok->ftstring_ರಾಶಿ);
    }
    PyMem_Free(tok);
}

ಶೂನ್ಯ
_PyToken_Init(ಸಂರಚನಾ token *token) {
    *token = (ಸಂರಚನಾ token){
        .type = -1,
        .span = {-1, -1},
        .start_loc = {-1, -1},
        .end_loc = {-1, -1},
    };
}

ಪೂರ್ಣಾಙ್ಕ
_PyLexer_token_setup(ಸಂರಚನಾ tok_state *tok, ಸಂರಚನಾ token *token, ಪೂರ್ಣಾಙ್ಕ type, _PyTok_Off start, _PyTok_Off end)
{
    token->level = tok->level;
    token->is_raw = 0;
    ಪ್ರತಿಜ್ಞಾ((start == -1 && end == -1) || (start >= 0 && end >= start));
    token->span = (_PyTok_Span){start, end};
    ಯದಿ (start >= 0) {
        token->start_loc = tok->start_loc;
        token->end_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};
    }
    ಅನ್ಯಥಾ {
        token->start_loc = token->end_loc = (_PyTok_Loc){tok->lineno, -1};
    }
    ಪ್ರತ್ಯಾಗಮ type;
}
