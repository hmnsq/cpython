#अन्तर्भाव "Python.h"
#अन्तर्भाव "pycore_token.h"
#अन्तर्भाव "errcode.h"

#अन्तर्भाव "state.h"
#अन्तर्भाव "../tokenizer/helpers.h"
#अन्तर्भाव "../tokenizer/reader.h"

ftstring_state *
Py_Lexer_प्रक्षिप_FTString(संरचना tok_state *tok)
{
    प्रतिज्ञा(tok->ftstring_गभीरता >= 0 && tok->ftstring_गभीरता <= tok->ftstring_क्षमता);
    पूर्णाङ्क अग्र_गभीरता = tok->ftstring_गभीरता + 1;
    यदि (अग्र_गभीरता >= MAXFTSTRINGLEVEL) {
        _PyTokenizer_syntaxerror(
            tok, "too many nested f-strings or t-strings");
        प्रत्यागम रिक्त;
    }
    यदि (tok->ftstring_गभीरता == tok->ftstring_क्षमता) {
        पूर्णाङ्क क्षमता = Py_MIN(Py_MAX(tok->ftstring_क्षमता * 2, 4),
                              MAXFTSTRINGLEVEL);
        आकार_t आकार = (आकार_t)क्षमता * परिमाण(*tok->ftstring_राशिः);
        ftstring_state *राशिः;
        यदि (tok->ftstring_राशिः == tok->ftstring_राशिः_inline) {
            राशिः = PyMem_Malloc(आकार);
            यदि (राशिः != रिक्त) {
                स्मृतिं_प्रतिलिख(राशिः, tok->ftstring_राशिः,
                       (आकार_t)tok->ftstring_गभीरता * परिमाण(*राशिः));
            }
        }
        अन्यथा {
            राशिः = PyMem_Realloc(tok->ftstring_राशिः, आकार);
        }
        यदि (राशिः == रिक्त) {
            Py_Err_No_स्मृतिः();
            tok->समाप्तम् = E_NOMEM;
            प्रत्यागम रिक्त;
        }
        tok->ftstring_राशिः = राशिः;
        tok->ftstring_क्षमता = क्षमता;
    }
    ftstring_state *state = &tok->ftstring_राशिः[tok->ftstring_गभीरता];
    tok->ftstring_गभीरता = अग्र_गभीरता;
    *state = (ftstring_state){0};
    प्रत्यागम state;
}

शून्य
Py_Lexer_निष्कासय_FTString(संरचना tok_state *tok)
{
    ftstring_state *state = _PyLexer_CurrentFTString(tok);
    प्रतिज्ञा(state != रिक्त);
    PyMem_Free(state->comments);
    tok->ftstring_गभीरता--;
}

/* Free a tok_state structure */
शून्य
_PyTokenizer_Free(संरचना tok_state *tok)
{
    Py_XDECREF(tok->filename);
    Py_XDECREF(tok->module);
    _PyTok_ReaderFree(tok);
    _PyTok_SourceClear(&tok->source);
    प्रति (पूर्णाङ्क i = 0; i < tok->ftstring_गभीरता; i++) {
        PyMem_Free(tok->ftstring_राशिः[i].comments);
    }
    यदि (tok->ftstring_राशिः != tok->ftstring_राशिः_inline) {
        PyMem_Free(tok->ftstring_राशिः);
    }
    PyMem_Free(tok);
}

शून्य
_PyToken_Init(संरचना token *token) {
    *token = (संरचना token){
        .type = -1,
        .विस्तृतिः = {-1, -1},
        .start_loc = {-1, -1},
        .अन्तः_loc = {-1, -1},
    };
}

पूर्णाङ्क
Py_Lexer_token_सज्जता(संरचना tok_state *tok, संरचना token *token, पूर्णाङ्क type, _PyTok_Off start, _PyTok_Off अन्तः_2)
{
    token->level = tok->level;
    token->is_raw = 0;
    प्रतिज्ञा((start == -1 && अन्तः_2 == -1) || (start >= 0 && अन्तः_2 >= start));
    token->विस्तृतिः = (Py_Tok_विस्तृतिः){start, अन्तः_2};
    यदि (start >= 0) {
        token->start_loc = tok->start_loc;
        token->अन्तः_loc = (_PyTok_Loc){tok->lineno, _PyLexer_ByteColumn(tok)};
    }
    अन्यथा {
        token->start_loc = token->अन्तः_loc = (_PyTok_Loc){tok->lineno, -1};
    }
    प्रत्यागम type;
}
