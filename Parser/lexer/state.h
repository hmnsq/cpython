#यदि_अपरिभाषितम् _PY_LEXER_H_
#परिभाषय _PY_LEXER_H_

#अन्तर्भाव "object.h"
#अन्तर्भाव "../tokenizer/source.h"
#अन्तर्भाव "../tokenizer/tokenizer.h"

#परिभाषय MAXINDENT 100       /* Max indentation level */
#परिभाषय MAXLEVEL 200        /* Max parentheses level */
#परिभाषय MAXFTSTRINGLEVEL 150
#परिभाषय FTSTRING_राशिः_INLINE_क्षमता 1

प्रकारनिर्देश गणना {
    FTSTRING_बहुलकम्_मध्यः,
    FTSTRING_बहुलकम्_EXPRESSION,
    FTSTRING_बहुलकम्_FORMAT_SPEC,
} ftstring_बहुलकम्;

प्रकारनिर्देश गणना {
    FSTRING,
    RAW_FSTRING,
    TSTRING,
    RAW_TSTRING,
} ftstring_विधा;

#परिभाषय MAX_व्यञ्जकम्_NESTING 3

प्रकारनिर्देश संरचना _tokenizer_comments {
    Py_ssize_t गणनम्;
    Py_ssize_t क्षमता;
    Py_Tok_विस्तृतिः spans[];
} tokenizer_comments;

प्रकारनिर्देश संरचना _ftstring_state {
    ftstring_बहुलकम् बहुलकम्;
    ftstring_विधा विधा;
    वर्ण quote;
    अचिह्नित वर्ण quote_आकार;
    अचिह्नित वर्ण debug_व्यञ्जकम्;
    अचिह्नित वर्ण replacement_गभीरता;
    पूर्णाङ्क paren_level;
    _PyTok_Off start;
    _PyTok_Loc start_loc;
    Py_Tok_विस्तृतिः व्यञ्जकम्_विस्तृतिः;
    tokenizer_comments *comments;
} ftstring_state;

स्थिर अन्तःस्थ पूर्णाङ्क
_PyLexer_IsTString(ftstring_विधा विधा)
{
    प्रत्यागम विधा == TSTRING || विधा == RAW_TSTRING;
}

स्थिर अन्तःस्थ पूर्णाङ्क
_PyLexer_IsRawString(ftstring_विधा विधा)
{
    प्रत्यागम विधा == RAW_FSTRING || विधा == RAW_TSTRING;
}

प्रकारनिर्देश संरचना {
    पूर्णाङ्क column;
    पूर्णाङ्क alternate_column;
} indentation_level;

प्रकारनिर्देश संरचना {
    पूर्णाङ्क गभीरता;
    पूर्णाङ्क pending;
    पूर्णाङ्क at_bol;
    पूर्णाङ्क comment_newline;
    indentation_level राशिः[MAXINDENT];
} lexer_layout_state;

/* Tokenizer state */
संरचना tok_state {
    _PyTok_Off buf_विस्थापनम्;
    _PyTok_Off cur;
    _PyTok_Off inp;
    _PyTok_Off start;
    _PyTok_Off पङ्क्तिः_start;
    _PyTok_SourceText source;
    पूर्णाङ्क समाप्तम्;           /* E_OK normally, E_EOF at EOF, otherwise error code */
    /* NB If done != E_OK, cur must be == inp!!! */
    lexer_layout_state layout;
    पूर्णाङ्क lineno;         /* Current line number */
    _PyTok_Loc start_loc;
    _PyTokenizer_Diagnostic diagnostic;
    पूर्णाङ्क level;          /* () [] {} Parentheses nesting level */
            /* Used to allow free continuations inside them */
    वर्ण parenstack[MAXLEVEL];
    पूर्णाङ्क parenlinenostack[MAXLEVEL];
    पूर्णाङ्क parencolstack[MAXLEVEL];
    PyObject *filename;
    PyObject *module;
    संरचना _PyTok_Reader *reader;

    पूर्णाङ्क type_comments;      /* Whether to look for type comments */

    ftstring_state *ftstring_राशिः;
    ftstring_state ftstring_राशिः_inline[FTSTRING_राशिः_INLINE_क्षमता];
    पूर्णाङ्क ftstring_गभीरता;
    पूर्णाङ्क ftstring_क्षमता;
    पूर्णाङ्क tok_extra_tokens;
    पूर्णाङ्क implicit_newline;
#यदि_परिभाषितम् Py_DEBUG
    पूर्णाङ्क debug;
#यदि_अन्तः
    पूर्णाङ्क barry_as_bdfl;
    अचिह्नित वर्ण allow_incomplete_निवेशः;
    अचिह्नित वर्ण needs_newline;
};

स्थिर अन्तःस्थ ftstring_state *
_PyLexer_CurrentFTString(संरचना tok_state *tok)
{
    प्रतिज्ञा(tok->ftstring_राशिः != रिक्त);
    प्रतिज्ञा(tok->ftstring_गभीरता >= 0 && tok->ftstring_गभीरता <= tok->ftstring_क्षमता);
    यदि (tok->ftstring_गभीरता == 0) {
        प्रत्यागम रिक्त;
    }
    प्रत्यागम &tok->ftstring_राशिः[tok->ftstring_गभीरता - 1];
}

स्थिर अन्तःस्थ वर्ण
Py_Lexer_String_उपसर्गः(ftstring_विधा विधा)
{
    प्रत्यागम _PyLexer_IsTString(विधा) ? 't' : 'f';
}

स्थिर अन्तःस्थ पूर्णाङ्क
Py_Lexer_FTString_Bracket_गभीरता(ध्रुव संरचना tok_state *tok,
                              ध्रुव ftstring_state *state)
{
    प्रत्यागम tok->level - state->paren_level;
}

स्थिर अन्तःस्थ पूर्णाङ्क
_PyLexer_ByteColumn(ध्रुव संरचना tok_state *tok)
{
    प्रतिज्ञा(tok->पङ्क्तिः_start >= 0);
    प्रतिज्ञा(tok->cur >= tok->पङ्क्तिः_start);
    Py_ssize_t column = tok->cur - tok->पङ्क्तिः_start;
    प्रतिज्ञा(column <= अधिकतम_पूर्णाङ्कः);
    प्रत्यागम (पूर्णाङ्क)column;
}

पूर्णाङ्क Py_Lexer_token_सज्जता(संरचना tok_state *tok, संरचना token *token, पूर्णाङ्क type, _PyTok_Off start, _PyTok_Off अन्तः_2);

शून्य _PyLexer_ImplyDedents(संरचना tok_state *);

शून्य _PyTokenizer_Free(संरचना tok_state *);
ftstring_state *Py_Lexer_प्रक्षिप_FTString(संरचना tok_state *);
शून्य Py_Lexer_निष्कासय_FTString(संरचना tok_state *);


#यदि_अन्तः
