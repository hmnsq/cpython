#अन्तर्भाव "Python.h"
#अन्तर्भाव "pycore_token.h"
#अन्तर्भाव "errcode.h"

#अन्तर्भाव "state.h"
#अन्तर्भाव "../tokenizer/helpers.h"
#अन्तर्भाव "../tokenizer/reader.h"

ftstring_state *
Py_Lexer_थप_FTString(संरचना tok_state *tok)
{
    प्रतिज्ञा(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    पूर्णाङ्क next_depth = tok->ftstring_depth + 1;
    यदि (next_depth >= MAXFTSTRINGLEVEL) {
        _PyTokenizer_syntaxerror(
            tok, "too many nested f-strings or t-strings");
        फर्काऊ रिक्त;
    }
    यदि (tok->ftstring_depth == tok->ftstring_capacity) {
        पूर्णाङ्क capacity = Py_MIN(Py_MAX(tok->ftstring_capacity * 2, 4),
                              MAXFTSTRINGLEVEL);
        माप_प्रकार size = (माप_प्रकार)capacity * परिमाण(*tok->ftstring_थाक);
        ftstring_state *थाक;
        यदि (tok->ftstring_थाक == tok->ftstring_थाक_inline) {
            थाक = PyMem_Malloc(size);
            यदि (थाक != रिक्त) {
                स_नकल(थाक, tok->ftstring_थाक,
                       (माप_प्रकार)tok->ftstring_depth * परिमाण(*थाक));
            }
        }
        अन्यथा {
            थाक = PyMem_Realloc(tok->ftstring_थाक, size);
        }
        यदि (थाक == रिक्त) {
            PyErr_NoMemory();
            tok->done = E_NOMEM;
            फर्काऊ रिक्त;
        }
        tok->ftstring_थाक = थाक;
        tok->ftstring_capacity = capacity;
    }
    ftstring_state *state = &tok->ftstring_थाक[tok->ftstring_depth];
    tok->ftstring_depth = next_depth;
    *state = (ftstring_state){0};
    फर्काऊ state;
}

शून्य
Py_Lexer_झिक_FTString(संरचना tok_state *tok)
{
    ftstring_state *state = _PyLexer_CurrentFTString(tok);
    प्रतिज्ञा(state != रिक्त);
    PyMem_Free(state->comments);
    tok->ftstring_depth--;
}

/* Free a tok_state structure */
शून्य
_PyTokenizer_Free(संरचना tok_state *tok)
{
    Py_XDECREF(tok->filename);
    Py_XDECREF(tok->module);
    _PyTok_ReaderFree(tok);
    _PyTok_SourceClear(&tok->source);
    लागि (पूर्णाङ्क i = 0; i < tok->ftstring_depth; i++) {
        PyMem_Free(tok->ftstring_थाक[i].comments);
    }
    यदि (tok->ftstring_थाक != tok->ftstring_थाक_inline) {
        PyMem_Free(tok->ftstring_थाक);
    }
    PyMem_Free(tok);
}

शून्य
_PyToken_Init(संरचना token *token) {
    *token = (संरचना token){
        .type = -1,
        .span = {-1, -1},
        .start_loc = {-1, -1},
        .end_loc = {-1, -1},
    };
}

पूर्णाङ्क
_PyLexer_token_setup(संरचना tok_state *tok, संरचना token *token, पूर्णाङ्क type, _PyTok_Off start, _PyTok_Off end)
{
    token->level = tok->level;
    token->is_raw = 0;
    प्रतिज्ञा((start == -1 && end == -1) || (start >= 0 && end >= start));
    token->span = (_PyTok_Span){start, end};
    यदि (start >= 0) {
        token->start_loc = tok->start_loc;
        token->end_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};
    }
    अन्यथा {
        token->start_loc = token->end_loc = (_PyTok_Loc){tok->lineno, -1};
    }
    फर्काऊ type;
}
