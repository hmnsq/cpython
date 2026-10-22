#അന്തര്ഭാവ "Python.h"
#അന്തര്ഭാവ "pycore_token.h"
#അന്തര്ഭാവ "errcode.h"

#അന്തര്ഭാവ "state.h"
#അന്തര്ഭാവ "../tokenizer/helpers.h"
#അന്തര്ഭാവ "../tokenizer/reader.h"

ftstring_state *
Py_Lexer_തള്ളുക_FTString(ഘടന tok_state *tok)
{
    പ്രതിജ്ഞാ(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    പൂർണ്ണസംഖ്യ next_depth = tok->ftstring_depth + 1;
    എങ്കിൽ (next_depth >= MAXFTSTRINGLEVEL) {
        _PyTokenizer_syntaxerror(
            tok, "too many nested f-strings or t-strings");
        തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (tok->ftstring_depth == tok->ftstring_capacity) {
        പൂർണ്ണസംഖ്യ capacity = Py_MIN(Py_MAX(tok->ftstring_capacity * 2, 4),
                              MAXFTSTRINGLEVEL);
        size_t size = (size_t)capacity * വലുപ്പം(*tok->ftstring_അട്ടി);
        ftstring_state *അട്ടി;
        എങ്കിൽ (tok->ftstring_അട്ടി == tok->ftstring_അട്ടി_inline) {
            അട്ടി = PyMem_Malloc(size);
            എങ്കിൽ (അട്ടി != ശൂന്യ) {
                memcpy(അട്ടി, tok->ftstring_അട്ടി,
                       (size_t)tok->ftstring_depth * വലുപ്പം(*അട്ടി));
            }
        }
        അല്ലെങ്കിൽ {
            അട്ടി = PyMem_Realloc(tok->ftstring_അട്ടി, size);
        }
        എങ്കിൽ (അട്ടി == ശൂന്യ) {
            PyErr_NoMemory();
            tok->done = E_NOMEM;
            തിരികെ ശൂന്യ;
        }
        tok->ftstring_അട്ടി = അട്ടി;
        tok->ftstring_capacity = capacity;
    }
    ftstring_state *state = &tok->ftstring_അട്ടി[tok->ftstring_depth];
    tok->ftstring_depth = next_depth;
    *state = (ftstring_state){0};
    തിരികെ state;
}

ശൂന്യം
Py_Lexer_എടുക്കുക_FTString(ഘടന tok_state *tok)
{
    ftstring_state *state = _PyLexer_CurrentFTString(tok);
    പ്രതിജ്ഞാ(state != ശൂന്യ);
    PyMem_Free(state->comments);
    tok->ftstring_depth--;
}

/* Free a tok_state structure */
ശൂന്യം
_PyTokenizer_Free(ഘടന tok_state *tok)
{
    Py_XDECREF(tok->filename);
    Py_XDECREF(tok->module);
    _PyTok_ReaderFree(tok);
    _PyTok_SourceClear(&tok->source);
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < tok->ftstring_depth; i++) {
        PyMem_Free(tok->ftstring_അട്ടി[i].comments);
    }
    എങ്കിൽ (tok->ftstring_അട്ടി != tok->ftstring_അട്ടി_inline) {
        PyMem_Free(tok->ftstring_അട്ടി);
    }
    PyMem_Free(tok);
}

ശൂന്യം
_PyToken_Init(ഘടന token *token) {
    *token = (ഘടന token){
        .type = -1,
        .span = {-1, -1},
        .start_loc = {-1, -1},
        .end_loc = {-1, -1},
    };
}

പൂർണ്ണസംഖ്യ
_PyLexer_token_setup(ഘടന tok_state *tok, ഘടന token *token, പൂർണ്ണസംഖ്യ type, _PyTok_Off start, _PyTok_Off end)
{
    token->level = tok->level;
    token->is_raw = 0;
    പ്രതിജ്ഞാ((start == -1 && end == -1) || (start >= 0 && end >= start));
    token->span = (_PyTok_Span){start, end};
    എങ്കിൽ (start >= 0) {
        token->start_loc = tok->start_loc;
        token->end_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};
    }
    അല്ലെങ്കിൽ {
        token->start_loc = token->end_loc = (_PyTok_Loc){tok->lineno, -1};
    }
    തിരികെ type;
}
