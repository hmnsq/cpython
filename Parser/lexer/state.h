#ifndef _PY_LEXER_H_
#define _PY_LEXER_H_

#अन्तर्भाव "object.h"
#अन्तर्भाव "../tokenizer/source.h"
#अन्तर्भाव "../tokenizer/tokenizer.h"

#define MAXINDENT 100       /* Max indentation level */
#define MAXLEVEL 200        /* Max parentheses level */
#define MAXFTSTRINGLEVEL 150
#define FTSTRING_चळत_INLINE_CAPACITY 1

प्रकारव्याख्या गणना {
    FTSTRING_MODE_मधला,
    FTSTRING_MODE_EXPRESSION,
    FTSTRING_MODE_FORMAT_SPEC,
} ftstring_mode;

प्रकारव्याख्या गणना {
    FSTRING,
    RAW_FSTRING,
    TSTRING,
    RAW_TSTRING,
} ftstring_kind;

#define MAX_अभिव्यक्ती_NESTING 3

प्रकारव्याख्या संरचना _tokenizer_comments {
    Py_ssize_t मोजणी;
    Py_ssize_t capacity;
    _PyTok_Span spans[];
} tokenizer_comments;

प्रकारव्याख्या संरचना _ftstring_state {
    ftstring_mode mode;
    ftstring_kind kind;
    अक्षर quote;
    अचिन्हित अक्षर quote_size;
    अचिन्हित अक्षर debug_अभिव्यक्ती;
    अचिन्हित अक्षर replacement_depth;
    पूर्णांक paren_level;
    _PyTok_Off start;
    _PyTok_Loc start_loc;
    _PyTok_Span अभिव्यक्ती_span;
    tokenizer_comments *comments;
} ftstring_state;

स्थिर ओळीत पूर्णांक
_PyLexer_IsTString(ftstring_kind kind)
{
    परत kind == TSTRING || kind == RAW_TSTRING;
}

स्थिर ओळीत पूर्णांक
_PyLexer_IsRawString(ftstring_kind kind)
{
    परत kind == RAW_FSTRING || kind == RAW_TSTRING;
}

प्रकारव्याख्या संरचना {
    पूर्णांक column;
    पूर्णांक alternate_column;
} indentation_level;

प्रकारव्याख्या संरचना {
    पूर्णांक depth;
    पूर्णांक pending;
    पूर्णांक at_bol;
    पूर्णांक comment_newline;
    indentation_level चळत[MAXINDENT];
} lexer_layout_state;

/* Tokenizer state */
संरचना tok_state {
    _PyTok_Off buf_offset;
    _PyTok_Off cur;
    _PyTok_Off inp;
    _PyTok_Off start;
    _PyTok_Off ओळ_start;
    _PyTok_SourceText source;
    पूर्णांक done;           /* E_OK normally, E_EOF at EOF, otherwise error code */
    /* NB If done != E_OK, cur must be == inp!!! */
    lexer_layout_state layout;
    पूर्णांक lineno;         /* Current line number */
    _PyTok_Loc start_loc;
    _PyTokenizer_Diagnostic diagnostic;
    पूर्णांक level;          /* () [] {} Parentheses nesting level */
            /* Used to allow free continuations inside them */
    अक्षर parenstack[MAXLEVEL];
    पूर्णांक parenlinenostack[MAXLEVEL];
    पूर्णांक parencolstack[MAXLEVEL];
    PyObject *filename;
    PyObject *module;
    संरचना _PyTok_Reader *reader;

    पूर्णांक type_comments;      /* Whether to look for type comments */

    ftstring_state *ftstring_चळत;
    ftstring_state ftstring_चळत_inline[FTSTRING_चळत_INLINE_CAPACITY];
    पूर्णांक ftstring_depth;
    पूर्णांक ftstring_capacity;
    पूर्णांक tok_extra_tokens;
    पूर्णांक implicit_newline;
#ifdef Py_DEBUG
    पूर्णांक debug;
#endif
    पूर्णांक barry_as_bdfl;
    अचिन्हित अक्षर allow_incomplete_आदान;
    अचिन्हित अक्षर needs_newline;
};

स्थिर ओळीत ftstring_state *
_PyLexer_CurrentFTString(संरचना tok_state *tok)
{
    प्रतिज्ञा(tok->ftstring_चळत != रिक्त);
    प्रतिज्ञा(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    जर (tok->ftstring_depth == 0) {
        परत रिक्त;
    }
    परत &tok->ftstring_चळत[tok->ftstring_depth - 1];
}

स्थिर ओळीत अक्षर
_PyLexer_StringPrefix(ftstring_kind kind)
{
    परत _PyLexer_IsTString(kind) ? 't' : 'f';
}

स्थिर ओळीत पूर्णांक
_PyLexer_FTStringBracketDepth(स्थिरांक संरचना tok_state *tok,
                              स्थिरांक ftstring_state *state)
{
    परत tok->level - state->paren_level;
}

स्थिर ओळीत पूर्णांक
_PyLexer_ByteColumn(स्थिरांक संरचना tok_state *tok)
{
    प्रतिज्ञा(tok->ओळ_start >= 0);
    प्रतिज्ञा(tok->cur >= tok->ओळ_start);
    Py_ssize_t column = tok->cur - tok->ओळ_start;
    प्रतिज्ञा(column <= पूर्णांक_उच्च);
    परत (पूर्णांक)column;
}

पूर्णांक _PyLexer_token_setup(संरचना tok_state *tok, संरचना token *token, पूर्णांक type, _PyTok_Off start, _PyTok_Off end);

शून्य _PyLexer_ImplyDedents(संरचना tok_state *);

शून्य _PyTokenizer_Free(संरचना tok_state *);
ftstring_state *Py_Lexer_ढकला_FTString(संरचना tok_state *);
शून्य Py_Lexer_काढा_FTString(संरचना tok_state *);


#endif
