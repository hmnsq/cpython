#அந்தர்பாவ "Python.h"
#அந்தர்பாவ "pycore_token.h"
#அந்தர்பாவ "errcode.h"

#அந்தர்பாவ "state.h"
#அந்தர்பாவ "../tokenizer/helpers.h"
#அந்தர்பாவ "../tokenizer/reader.h"

ftstring_state *
Py_Lexer_தள்ளு_FTString(கட்டமைப்பு tok_state *tok)
{
    ப்ரதிஜ்ஞா(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    முழு next_depth = tok->ftstring_depth + 1;
    எனில் (next_depth >= MAXFTSTRINGLEVEL) {
        _PyTokenizer_syntaxerror(
            tok, "too many nested f-strings or t-strings");
        திரும்பு வெற்று;
    }
    எனில் (tok->ftstring_depth == tok->ftstring_capacity) {
        முழு capacity = Py_MIN(Py_MAX(tok->ftstring_capacity * 2, 4),
                              MAXFTSTRINGLEVEL);
        size_t size = (size_t)capacity * அளவு(*tok->ftstring_அடுக்கு);
        ftstring_state *அடுக்கு;
        எனில் (tok->ftstring_அடுக்கு == tok->ftstring_அடுக்கு_inline) {
            அடுக்கு = PyMem_Malloc(size);
            எனில் (அடுக்கு != வெற்று) {
                memcpy(அடுக்கு, tok->ftstring_அடுக்கு,
                       (size_t)tok->ftstring_depth * அளவு(*அடுக்கு));
            }
        }
        இல்லையெனில் {
            அடுக்கு = PyMem_Realloc(tok->ftstring_அடுக்கு, size);
        }
        எனில் (அடுக்கு == வெற்று) {
            PyErr_NoMemory();
            tok->done = E_NOMEM;
            திரும்பு வெற்று;
        }
        tok->ftstring_அடுக்கு = அடுக்கு;
        tok->ftstring_capacity = capacity;
    }
    ftstring_state *state = &tok->ftstring_அடுக்கு[tok->ftstring_depth];
    tok->ftstring_depth = next_depth;
    *state = (ftstring_state){0};
    திரும்பு state;
}

வெறுமை
Py_Lexer_எடு_FTString(கட்டமைப்பு tok_state *tok)
{
    ftstring_state *state = _PyLexer_CurrentFTString(tok);
    ப்ரதிஜ்ஞா(state != வெற்று);
    PyMem_Free(state->comments);
    tok->ftstring_depth--;
}

/* Free a tok_state structure */
வெறுமை
_PyTokenizer_Free(கட்டமைப்பு tok_state *tok)
{
    Py_XDECREF(tok->filename);
    Py_XDECREF(tok->module);
    _PyTok_ReaderFree(tok);
    _PyTok_SourceClear(&tok->source);
    க்கு (முழு i = 0; i < tok->ftstring_depth; i++) {
        PyMem_Free(tok->ftstring_அடுக்கு[i].comments);
    }
    எனில் (tok->ftstring_அடுக்கு != tok->ftstring_அடுக்கு_inline) {
        PyMem_Free(tok->ftstring_அடுக்கு);
    }
    PyMem_Free(tok);
}

வெறுமை
_PyToken_Init(கட்டமைப்பு token *token) {
    *token = (கட்டமைப்பு token){
        .type = -1,
        .span = {-1, -1},
        .start_loc = {-1, -1},
        .end_loc = {-1, -1},
    };
}

முழு
_PyLexer_token_setup(கட்டமைப்பு tok_state *tok, கட்டமைப்பு token *token, முழு type, _PyTok_Off start, _PyTok_Off end)
{
    token->level = tok->level;
    token->is_raw = 0;
    ப்ரதிஜ்ஞா((start == -1 && end == -1) || (start >= 0 && end >= start));
    token->span = (_PyTok_Span){start, end};
    எனில் (start >= 0) {
        token->start_loc = tok->start_loc;
        token->end_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};
    }
    இல்லையெனில் {
        token->start_loc = token->end_loc = (_PyTok_Loc){tok->lineno, -1};
    }
    திரும்பு type;
}
