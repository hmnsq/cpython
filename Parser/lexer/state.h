#నిర్వచితం_కాకపోతే _PY_LEXER_H_
#నిర్వచించు _PY_LEXER_H_

#అన్తర్భావ "object.h"
#అన్తర్భావ "../tokenizer/source.h"
#అన్తర్భావ "../tokenizer/tokenizer.h"

#నిర్వచించు MAXINDENT 100       /* Max indentation level */
#నిర్వచించు MAXLEVEL 200        /* Max parentheses level */
#నిర్వచించు MAXFTSTRINGLEVEL 150
#నిర్వచించు FTSTRING_దొంతర_INLINE_సామర్థ్యం 1

రకనిర్వచనం గణన {
    FTSTRING_బహుళకం_మధ్య,
    FTSTRING_బహుళకం_వ్యక్తీకరణ,
    FTSTRING_బహుళకం_FORMAT_SPEC,
} ftstring_బహుళకం;

రకనిర్వచనం గణన {
    FSTRING,
    RAW_FSTRING,
    TSTRING,
    RAW_TSTRING,
} ftstring_రకం;

#నిర్వచించు MAX_సమాసం_NESTING 3

రకనిర్వచనం నిర్మాణం _tokenizer_comments {
    Py_ssize_t లెక్క;
    Py_ssize_t సామర్థ్యం;
    Py_Tok_పరిధి spans[];
} tokenizer_comments;

రకనిర్వచనం నిర్మాణం _ftstring_state {
    ftstring_బహుళకం బహుళకం;
    ftstring_రకం రకం;
    అక్షరం quote;
    గుర్తులేని అక్షరం quote_కొలత;
    గుర్తులేని అక్షరం debug_సమాసం;
    గుర్తులేని అక్షరం replacement_లోతు;
    పూర్ణాంకం paren_level;
    _PyTok_Off ప్రారంభం;
    _PyTok_Loc ప్రారంభం_loc;
    Py_Tok_పరిధి సమాసం_పరిధి;
    tokenizer_comments *comments;
} ftstring_state;

స్థిర అంతఃవరుస పూర్ణాంకం
_PyLexer_IsTString(ftstring_రకం రకం)
{
    తిరిగి రకం == TSTRING || రకం == RAW_TSTRING;
}

స్థిర అంతఃవరుస పూర్ణాంకం
_PyLexer_IsRawString(ftstring_రకం రకం)
{
    తిరిగి రకం == RAW_FSTRING || రకం == RAW_TSTRING;
}

రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం స్తంభం;
    పూర్ణాంకం alternate_స్తంభం;
} indentation_level;

రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం లోతు;
    పూర్ణాంకం pending;
    పూర్ణాంకం at_bol;
    పూర్ణాంకం comment_newline;
    indentation_level దొంతర[MAXINDENT];
} lexer_layout_state;

/* Tokenizer state */
నిర్మాణం tok_state {
    _PyTok_Off buf_స్థానభ్రంశం;
    _PyTok_Off cur;
    _PyTok_Off inp;
    _PyTok_Off ప్రారంభం;
    _PyTok_Off పంక్తి_ప్రారంభం;
    Py_Tok_ఆధారం_పాఠ్యం ఆధారం;
    పూర్ణాంకం పూర్తి;           /* E_OK normally, E_EOF at EOF, otherwise error code */
    /* NB If done != E_OK, cur must be == inp!!! */
    lexer_layout_state layout;
    పూర్ణాంకం lineno;         /* Current line number */
    _PyTok_Loc ప్రారంభం_loc;
    _PyTokenizer_Diagnostic diagnostic;
    పూర్ణాంకం level;          /* () [] {} Parentheses nesting level */
            /* Used to allow free continuations inside them */
    అక్షరం parenstack[MAXLEVEL];
    పూర్ణాంకం parenlinenostack[MAXLEVEL];
    పూర్ణాంకం parencolstack[MAXLEVEL];
    PyObject *filename;
    PyObject *module;
    నిర్మాణం _PyTok_Reader *reader;

    పూర్ణాంకం type_comments;      /* Whether to look for type comments */

    ftstring_state *ftstring_దొంతర;
    ftstring_state ftstring_దొంతర_inline[FTSTRING_దొంతర_INLINE_సామర్థ్యం];
    పూర్ణాంకం ftstring_లోతు;
    పూర్ణాంకం ftstring_సామర్థ్యం;
    పూర్ణాంకం tok_extra_టోకెన్లు;
    పూర్ణాంకం implicit_newline;
#నిర్వచితమైతే Py_DEBUG
    పూర్ణాంకం debug;
#అయితే_ముగింపు
    పూర్ణాంకం barry_as_bdfl;
    గుర్తులేని అక్షరం allow_incomplete_ఆదానం;
    గుర్తులేని అక్షరం needs_newline;
};

స్థిర అంతఃవరుస ftstring_state *
_PyLexer_CurrentFTString(నిర్మాణం tok_state *tok)
{
    ప్రతిజ్ఞా(tok->ftstring_దొంతర != ఖాళీ);
    ప్రతిజ్ఞా(tok->ftstring_లోతు >= 0 && tok->ftstring_లోతు <= tok->ftstring_సామర్థ్యం);
    ఒకవేళ (tok->ftstring_లోతు == 0) {
        తిరిగి ఖాళీ;
    }
    తిరిగి &tok->ftstring_దొంతర[tok->ftstring_లోతు - 1];
}

స్థిర అంతఃవరుస అక్షరం
Py_Lexer_String_ఉపసర్గ(ftstring_రకం రకం)
{
    తిరిగి _PyLexer_IsTString(రకం) ? 't' : 'f';
}

స్థిర అంతఃవరుస పూర్ణాంకం
Py_Lexer_FTString_Bracket_లోతు(స్థిరరాశి నిర్మాణం tok_state *tok,
                              స్థిరరాశి ftstring_state *state)
{
    తిరిగి tok->level - state->paren_level;
}

స్థిర అంతఃవరుస పూర్ణాంకం
Py_Lexer_Byte_స్తంభం(స్థిరరాశి నిర్మాణం tok_state *tok)
{
    ప్రతిజ్ఞా(tok->పంక్తి_ప్రారంభం >= 0);
    ప్రతిజ్ఞా(tok->cur >= tok->పంక్తి_ప్రారంభం);
    Py_ssize_t స్తంభం = tok->cur - tok->పంక్తి_ప్రారంభం;
    ప్రతిజ్ఞా(స్తంభం <= గరిష్ఠ_పూర్ణాంకం);
    తిరిగి (పూర్ణాంకం)స్తంభం;
}

పూర్ణాంకం Py_Lexer_token_అమరిక(నిర్మాణం tok_state *tok, నిర్మాణం token *token, పూర్ణాంకం type, _PyTok_Off ప్రారంభం, _PyTok_Off చివర);

శూన్య _PyLexer_ImplyDedents(నిర్మాణం tok_state *);

శూన్య _PyTokenizer_Free(నిర్మాణం tok_state *);
ftstring_state *Py_Lexer_నెట్టు_FTString(నిర్మాణం tok_state *);
శూన్య Py_Lexer_తీయి_FTString(నిర్మాణం tok_state *);


#అయితే_ముగింపు
