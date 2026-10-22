#अन्तर्भाव "Python.h"
#अन्तर्भाव "pycore_token.h"
#अन्तर्भाव "errcode.h"

#अन्तर्भाव "state.h"
#अन्तर्भाव "../tokenizer/helpers.h"
#अन्तर्भाव "../tokenizer/reader.h"

ftstring_state *
Py_Lexer_ढकला_FTString(संरचना tok_state *tok)
{
    प्रतिज्ञा(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    पूर्णाङ्क next_depth = tok->ftstring_depth + 1;
    यदि (next_depth >= MAXFTSTRINGLEVEL) {
        _PyTokenizer_syntaxerror(
            tok, "too many nested f-strings or t-strings");
        प्रत्यागम रिक्त;
    }
    यदि (tok->ftstring_depth == tok->ftstring_capacity) {
        पूर्णाङ्क capacity = Py_MIN(Py_MAX(tok->ftstring_capacity * 2, 4),
                              MAXFTSTRINGLEVEL);
        size_t size = (size_t)capacity * परिमाण(*tok->ftstring_चळत);
        ftstring_state *चळत;
        यदि (tok->ftstring_चळत == tok->ftstring_चळत_inline) {
            चळत = PyMem_Malloc(size);
            यदि (चळत != रिक्त) {
                memcpy(चळत, tok->ftstring_चळत,
                       (size_t)tok->ftstring_depth * परिमाण(*चळत));
            }
        }
        अन्यथा {
            चळत = PyMem_Realloc(tok->ftstring_चळत, size);
        }
        यदि (चळत == रिक्त) {
            PyErr_NoMemory();
            tok->done = E_NOMEM;
            प्रत्यागम रिक्त;
        }
        tok->ftstring_चळत = चळत;
        tok->ftstring_capacity = capacity;
    }
    ftstring_state *state = &tok->ftstring_चळत[tok->ftstring_depth];
    tok->ftstring_depth = next_depth;
    *state = (ftstring_state){0};
    प्रत्यागम state;
}

शून्य
Py_Lexer_काढा_FTString(संरचना tok_state *tok)
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
    प्रति (पूर्णाङ्क i = 0; i < tok->ftstring_depth; i++) {
        PyMem_Free(tok->ftstring_चळत[i].comments);
    }
    यदि (tok->ftstring_चळत != tok->ftstring_चळत_inline) {
        PyMem_Free(tok->ftstring_चळत);
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
    प्रत्यागम type;
}
