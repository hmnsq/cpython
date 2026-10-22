#అన్తర్భావ "Python.h"
#అన్తర్భావ "pycore_token.h"
#అన్తర్భావ "errcode.h"

#అన్తర్భావ "state.h"
#అన్తర్భావ "../tokenizer/helpers.h"
#అన్తర్భావ "../tokenizer/reader.h"

ftstring_state *
Py_Lexer_నెట్టు_FTString(నిర్మాణం tok_state *tok)
{
    ప్రతిజ్ఞా(tok->ftstring_లోతు >= 0 && tok->ftstring_లోతు <= tok->ftstring_సామర్థ్యం);
    పూర్ణాంకం తదుపరి_లోతు = tok->ftstring_లోతు + 1;
    ఒకవేళ (తదుపరి_లోతు >= MAXFTSTRINGLEVEL) {
        _PyTokenizer_syntaxerror(
            tok, "too many nested f-strings or t-strings");
        తిరిగి ఖాళీ;
    }
    ఒకవేళ (tok->ftstring_లోతు == tok->ftstring_సామర్థ్యం) {
        పూర్ణాంకం సామర్థ్యం = Py_MIN(Py_MAX(tok->ftstring_సామర్థ్యం * 2, 4),
                              MAXFTSTRINGLEVEL);
        కొలత_t కొలత = (కొలత_t)సామర్థ్యం * పరిమాణం(*tok->ftstring_దొంతర);
        ftstring_state *దొంతర;
        ఒకవేళ (tok->ftstring_దొంతర == tok->ftstring_దొంతర_inline) {
            దొంతర = PyMem_Malloc(కొలత);
            ఒకవేళ (దొంతర != ఖాళీ) {
                జ్ఞాపకం_నకలు(దొంతర, tok->ftstring_దొంతర,
                       (కొలత_t)tok->ftstring_లోతు * పరిమాణం(*దొంతర));
            }
        }
        లేకపోతే {
            దొంతర = PyMem_Realloc(tok->ftstring_దొంతర, కొలత);
        }
        ఒకవేళ (దొంతర == ఖాళీ) {
            Py_Err_No_జ్ఞాపకం();
            tok->పూర్తి = E_NOMEM;
            తిరిగి ఖాళీ;
        }
        tok->ftstring_దొంతర = దొంతర;
        tok->ftstring_సామర్థ్యం = సామర్థ్యం;
    }
    ftstring_state *state = &tok->ftstring_దొంతర[tok->ftstring_లోతు];
    tok->ftstring_లోతు = తదుపరి_లోతు;
    *state = (ftstring_state){0};
    తిరిగి state;
}

శూన్య
Py_Lexer_తీయి_FTString(నిర్మాణం tok_state *tok)
{
    ftstring_state *state = _PyLexer_CurrentFTString(tok);
    ప్రతిజ్ఞా(state != ఖాళీ);
    PyMem_Free(state->comments);
    tok->ftstring_లోతు--;
}

/* Free a tok_state structure */
శూన్య
_PyTokenizer_Free(నిర్మాణం tok_state *tok)
{
    Py_XDECREF(tok->filename);
    Py_XDECREF(tok->module);
    _PyTok_ReaderFree(tok);
    Py_Tok_ఆధారం_Clear(&tok->ఆధారం);
    కొరకు (పూర్ణాంకం i = 0; i < tok->ftstring_లోతు; i++) {
        PyMem_Free(tok->ftstring_దొంతర[i].comments);
    }
    ఒకవేళ (tok->ftstring_దొంతర != tok->ftstring_దొంతర_inline) {
        PyMem_Free(tok->ftstring_దొంతర);
    }
    PyMem_Free(tok);
}

శూన్య
_PyToken_Init(నిర్మాణం token *token) {
    *token = (నిర్మాణం token){
        .type = -1,
        .పరిధి = {-1, -1},
        .ప్రారంభం_loc = {-1, -1},
        .చివర_loc = {-1, -1},
    };
}

పూర్ణాంకం
Py_Lexer_token_అమరిక(నిర్మాణం tok_state *tok, నిర్మాణం token *token, పూర్ణాంకం type, _PyTok_Off ప్రారంభం, _PyTok_Off చివర)
{
    token->level = tok->level;
    token->is_raw = 0;
    ప్రతిజ్ఞా((ప్రారంభం == -1 && చివర == -1) || (ప్రారంభం >= 0 && చివర >= ప్రారంభం));
    token->పరిధి = (Py_Tok_పరిధి){ప్రారంభం, చివర};
    ఒకవేళ (ప్రారంభం >= 0) {
        token->ప్రారంభం_loc = tok->ప్రారంభం_loc;
        token->చివర_loc = (_PyTok_Loc){tok->lineno, Py_Lexer_Byte_స్తంభం(tok)};
    }
    లేకపోతే {
        token->ప్రారంభం_loc = token->చివర_loc = (_PyTok_Loc){tok->lineno, -1};
    }
    తిరిగి type;
}
