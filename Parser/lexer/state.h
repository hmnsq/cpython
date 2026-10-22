#ifndef _PY_LEXER_H_
#define _PY_LEXER_H_

#ضمن "object.h"
#ضمن "../tokenizer/source.h"
#ضمن "../tokenizer/tokenizer.h"

#define MAXINDENT 100       /* Max indentation level */
#define MAXLEVEL 200        /* Max parentheses level */
#define MAXFTSTRINGLEVEL 150
#define FTSTRING_دۆۋە_INLINE_CAPACITY 1

تعريفنوع تعداد {
    FTSTRING_MODE_ئوتتۇرا,
    FTSTRING_MODE_EXPRESSION,
    FTSTRING_MODE_FORMAT_SPEC,
} ftstring_mode;

تعريفنوع تعداد {
    FSTRING,
    RAW_FSTRING,
    TSTRING,
    RAW_TSTRING,
} ftstring_kind;

#define MAX_ئىپادە_NESTING 3

تعريفنوع بنية _tokenizer_comments {
    Py_ssize_t سانى;
    Py_ssize_t capacity;
    _PyTok_Span spans[];
} tokenizer_comments;

تعريفنوع بنية _ftstring_state {
    ftstring_mode mode;
    ftstring_kind kind;
    حرف quote;
    unsigned حرف quote_size;
    unsigned حرف debug_ئىپادە;
    unsigned حرف replacement_depth;
    صحيح paren_level;
    _PyTok_Off start;
    _PyTok_Loc start_loc;
    _PyTok_Span ئىپادە_span;
    tokenizer_comments *comments;
} ftstring_state;

ساكن مضمن صحيح
_PyLexer_IsTString(ftstring_kind kind)
{
    أرجع kind == TSTRING || kind == RAW_TSTRING;
}

ساكن مضمن صحيح
_PyLexer_IsRawString(ftstring_kind kind)
{
    أرجع kind == RAW_FSTRING || kind == RAW_TSTRING;
}

تعريفنوع بنية {
    صحيح column;
    صحيح alternate_column;
} indentation_level;

تعريفنوع بنية {
    صحيح depth;
    صحيح pending;
    صحيح at_bol;
    صحيح comment_newline;
    indentation_level دۆۋە[MAXINDENT];
} lexer_layout_state;

/* Tokenizer state */
بنية tok_state {
    _PyTok_Off buf_offset;
    _PyTok_Off cur;
    _PyTok_Off inp;
    _PyTok_Off start;
    _PyTok_Off قۇر_start;
    _PyTok_SourceText source;
    صحيح done;           /* E_OK normally, E_EOF at EOF, otherwise error code */
    /* NB If done != E_OK, cur must be == inp!!! */
    lexer_layout_state layout;
    صحيح lineno;         /* Current line number */
    _PyTok_Loc start_loc;
    _PyTokenizer_Diagnostic diagnostic;
    صحيح level;          /* () [] {} Parentheses nesting level */
            /* Used to allow free continuations inside them */
    حرف parenstack[MAXLEVEL];
    صحيح parenlinenostack[MAXLEVEL];
    صحيح parencolstack[MAXLEVEL];
    PyObject *filename;
    PyObject *module;
    بنية _PyTok_Reader *reader;

    صحيح type_comments;      /* Whether to look for type comments */

    ftstring_state *ftstring_دۆۋە;
    ftstring_state ftstring_دۆۋە_inline[FTSTRING_دۆۋە_INLINE_CAPACITY];
    صحيح ftstring_depth;
    صحيح ftstring_capacity;
    صحيح tok_extra_tokens;
    صحيح implicit_newline;
#ifdef Py_DEBUG
    صحيح debug;
#endif
    صحيح barry_as_bdfl;
    unsigned حرف allow_incomplete_كىرگۈزۈش;
    unsigned حرف needs_newline;
};

ساكن مضمن ftstring_state *
_PyLexer_CurrentFTString(بنية tok_state *tok)
{
    أكد(tok->ftstring_دۆۋە != عدم);
    أكد(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    إذا (tok->ftstring_depth == 0) {
        أرجع عدم;
    }
    أرجع &tok->ftstring_دۆۋە[tok->ftstring_depth - 1];
}

ساكن مضمن حرف
_PyLexer_StringPrefix(ftstring_kind kind)
{
    أرجع _PyLexer_IsTString(kind) ? 't' : 'f';
}

ساكن مضمن صحيح
_PyLexer_FTStringBracketDepth(ثابت بنية tok_state *tok,
                              ثابت ftstring_state *state)
{
    أرجع tok->level - state->paren_level;
}

ساكن مضمن صحيح
_PyLexer_ByteColumn(ثابت بنية tok_state *tok)
{
    أكد(tok->قۇر_start >= 0);
    أكد(tok->cur >= tok->قۇر_start);
    Py_ssize_t column = tok->cur - tok->قۇر_start;
    أكد(column <= INT_MAX);
    أرجع (صحيح)column;
}

صحيح _PyLexer_token_setup(بنية tok_state *tok, بنية token *token, صحيح type, _PyTok_Off start, _PyTok_Off end);

فراغ _PyLexer_ImplyDedents(بنية tok_state *);

فراغ _PyTokenizer_Free(بنية tok_state *);
ftstring_state *Py_Lexer_ئىتتىرىش_FTString(بنية tok_state *);
فراغ Py_Lexer_چىقىرىش_FTString(بنية tok_state *);


#endif
