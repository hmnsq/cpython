#ifndef _PY_LEXER_H_
#define _PY_LEXER_H_

#अन्तर्भाव "object.h"
#अन्तर्भाव "../tokenizer/source.h"
#अन्तर्भाव "../tokenizer/tokenizer.h"

#define MAXINDENT 100       /* Max indentation level */
#define MAXLEVEL 200        /* Max parentheses level */
#define MAXFTSTRINGLEVEL 150
#define FTSTRING_थाक_INLINE_CAPACITY 1

प्रकारनिर्देश गणना {
    FTSTRING_MODE_बीच,
    FTSTRING_MODE_EXPRESSION,
    FTSTRING_MODE_FORMAT_SPEC,
} ftstring_mode;

प्रकारनिर्देश गणना {
    FSTRING,
    RAW_FSTRING,
    TSTRING,
    RAW_TSTRING,
} ftstring_kind;

#define MAX_अभिव्यक्ति_NESTING 3

प्रकारनिर्देश संरचना _tokenizer_comments {
    Py_ssize_t गन्ती;
    Py_ssize_t capacity;
    _PyTok_Span spans[];
} tokenizer_comments;

प्रकारनिर्देश संरचना _ftstring_state {
    ftstring_mode mode;
    ftstring_kind kind;
    वर्ण quote;
    अचिन्हित वर्ण quote_size;
    अचिन्हित वर्ण debug_अभिव्यक्ति;
    अचिन्हित वर्ण replacement_depth;
    पूर्णाङ्क paren_level;
    _PyTok_Off start;
    _PyTok_Loc start_loc;
    _PyTok_Span अभिव्यक्ति_span;
    tokenizer_comments *comments;
} ftstring_state;

स्थिर अन्तःस्थ पूर्णाङ्क
_PyLexer_IsTString(ftstring_kind kind)
{
    फर्काऊ kind == TSTRING || kind == RAW_TSTRING;
}

स्थिर अन्तःस्थ पूर्णाङ्क
_PyLexer_IsRawString(ftstring_kind kind)
{
    फर्काऊ kind == RAW_FSTRING || kind == RAW_TSTRING;
}

प्रकारनिर्देश संरचना {
    पूर्णाङ्क column;
    पूर्णाङ्क alternate_column;
} indentation_level;

प्रकारनिर्देश संरचना {
    पूर्णाङ्क depth;
    पूर्णाङ्क pending;
    पूर्णाङ्क at_bol;
    पूर्णाङ्क comment_newline;
    indentation_level थाक[MAXINDENT];
} lexer_layout_state;

/* Tokenizer state */
संरचना tok_state {
    _PyTok_Off buf_offset;
    _PyTok_Off cur;
    _PyTok_Off inp;
    _PyTok_Off start;
    _PyTok_Off पङ्क्ति_start;
    _PyTok_SourceText source;
    पूर्णाङ्क done;           /* E_OK normally, E_EOF at EOF, otherwise error code */
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

    ftstring_state *ftstring_थाक;
    ftstring_state ftstring_थाक_inline[FTSTRING_थाक_INLINE_CAPACITY];
    पूर्णाङ्क ftstring_depth;
    पूर्णाङ्क ftstring_capacity;
    पूर्णाङ्क tok_extra_tokens;
    पूर्णाङ्क implicit_newline;
#ifdef Py_DEBUG
    पूर्णाङ्क debug;
#endif
    पूर्णाङ्क barry_as_bdfl;
    अचिन्हित वर्ण allow_incomplete_आगत;
    अचिन्हित वर्ण needs_newline;
};

स्थिर अन्तःस्थ ftstring_state *
_PyLexer_CurrentFTString(संरचना tok_state *tok)
{
    प्रतिज्ञा(tok->ftstring_थाक != रिक्त);
    प्रतिज्ञा(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    यदि (tok->ftstring_depth == 0) {
        फर्काऊ रिक्त;
    }
    फर्काऊ &tok->ftstring_थाक[tok->ftstring_depth - 1];
}

स्थिर अन्तःस्थ वर्ण
_PyLexer_StringPrefix(ftstring_kind kind)
{
    फर्काऊ _PyLexer_IsTString(kind) ? 't' : 'f';
}

स्थिर अन्तःस्थ पूर्णाङ्क
_PyLexer_FTStringBracketDepth(अचर संरचना tok_state *tok,
                              अचर ftstring_state *state)
{
    फर्काऊ tok->level - state->paren_level;
}

स्थिर अन्तःस्थ पूर्णाङ्क
_PyLexer_ByteColumn(अचर संरचना tok_state *tok)
{
    प्रतिज्ञा(tok->पङ्क्ति_start >= 0);
    प्रतिज्ञा(tok->cur >= tok->पङ्क्ति_start);
    Py_ssize_t column = tok->cur - tok->पङ्क्ति_start;
    प्रतिज्ञा(column <= पूर्णांक_उच्च);
    फर्काऊ (पूर्णाङ्क)column;
}

पूर्णाङ्क _PyLexer_token_setup(संरचना tok_state *tok, संरचना token *token, पूर्णाङ्क type, _PyTok_Off start, _PyTok_Off end);

शून्य _PyLexer_ImplyDedents(संरचना tok_state *);

शून्य _PyTokenizer_Free(संरचना tok_state *);
ftstring_state *Py_Lexer_थप_FTString(संरचना tok_state *);
शून्य Py_Lexer_झिक_FTString(संरचना tok_state *);


#endif
