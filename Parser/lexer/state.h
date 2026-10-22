#ifndef _PY_LEXER_H_
#define _PY_LEXER_H_

#شامل "object.h"
#شامل "../tokenizer/source.h"
#شامل "../tokenizer/tokenizer.h"

#define MAXINDENT 100       /* Max indentation level */
#define MAXLEVEL 200        /* Max parentheses level */
#define MAXFTSTRINGLEVEL 150
#define FTSTRING_پشته_INLINE_CAPACITY 1

تعریفنوع شمارش {
    FTSTRING_MODE_میانه,
    FTSTRING_MODE_EXPRESSION,
    FTSTRING_MODE_FORMAT_SPEC,
} ftstring_mode;

تعریفنوع شمارش {
    FSTRING,
    RAW_FSTRING,
    TSTRING,
    RAW_TSTRING,
} ftstring_kind;

#define MAX_عبارت_NESTING 3

تعریفنوع ساختار _tokenizer_comments {
    Py_ssize_t شمار;
    Py_ssize_t capacity;
    _PyTok_Span spans[];
} tokenizer_comments;

تعریفنوع ساختار _ftstring_state {
    ftstring_mode mode;
    ftstring_kind kind;
    نویسه quote;
    unsigned نویسه quote_size;
    unsigned نویسه debug_عبارت;
    unsigned نویسه replacement_depth;
    صحیح paren_level;
    _PyTok_Off start;
    _PyTok_Loc start_loc;
    _PyTok_Span عبارت_span;
    tokenizer_comments *comments;
} ftstring_state;

ایستا درونخطی صحیح
_PyLexer_IsTString(ftstring_kind kind)
{
    بازگشت kind == TSTRING || kind == RAW_TSTRING;
}

ایستا درونخطی صحیح
_PyLexer_IsRawString(ftstring_kind kind)
{
    بازگشت kind == RAW_FSTRING || kind == RAW_TSTRING;
}

تعریفنوع ساختار {
    صحیح column;
    صحیح alternate_column;
} indentation_level;

تعریفنوع ساختار {
    صحیح depth;
    صحیح pending;
    صحیح at_bol;
    صحیح comment_newline;
    indentation_level پشته[MAXINDENT];
} lexer_layout_state;

/* Tokenizer state */
ساختار tok_state {
    _PyTok_Off buf_offset;
    _PyTok_Off cur;
    _PyTok_Off inp;
    _PyTok_Off start;
    _PyTok_Off سطر_start;
    _PyTok_SourceText source;
    صحیح done;           /* E_OK normally, E_EOF at EOF, otherwise error code */
    /* NB If done != E_OK, cur must be == inp!!! */
    lexer_layout_state layout;
    صحیح lineno;         /* Current line number */
    _PyTok_Loc start_loc;
    _PyTokenizer_Diagnostic diagnostic;
    صحیح level;          /* () [] {} Parentheses nesting level */
            /* Used to allow free continuations inside them */
    نویسه parenstack[MAXLEVEL];
    صحیح parenlinenostack[MAXLEVEL];
    صحیح parencolstack[MAXLEVEL];
    PyObject *filename;
    PyObject *module;
    ساختار _PyTok_Reader *reader;

    صحیح type_comments;      /* Whether to look for type comments */

    ftstring_state *ftstring_پشته;
    ftstring_state ftstring_پشته_inline[FTSTRING_پشته_INLINE_CAPACITY];
    صحیح ftstring_depth;
    صحیح ftstring_capacity;
    صحیح tok_extra_tokens;
    صحیح implicit_newline;
#ifdef Py_DEBUG
    صحیح debug;
#endif
    صحیح barry_as_bdfl;
    unsigned نویسه allow_incomplete_درون_داد;
    unsigned نویسه needs_newline;
};

ایستا درونخطی ftstring_state *
_PyLexer_CurrentFTString(ساختار tok_state *tok)
{
    أكد(tok->ftstring_پشته != پوچ);
    أكد(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    اگر (tok->ftstring_depth == 0) {
        بازگشت پوچ;
    }
    بازگشت &tok->ftstring_پشته[tok->ftstring_depth - 1];
}

ایستا درونخطی نویسه
_PyLexer_StringPrefix(ftstring_kind kind)
{
    بازگشت _PyLexer_IsTString(kind) ? 't' : 'f';
}

ایستا درونخطی صحیح
_PyLexer_FTStringBracketDepth(ثابت ساختار tok_state *tok,
                              ثابت ftstring_state *state)
{
    بازگشت tok->level - state->paren_level;
}

ایستا درونخطی صحیح
_PyLexer_ByteColumn(ثابت ساختار tok_state *tok)
{
    أكد(tok->سطر_start >= 0);
    أكد(tok->cur >= tok->سطر_start);
    Py_ssize_t column = tok->cur - tok->سطر_start;
    أكد(column <= INT_MAX);
    بازگشت (صحیح)column;
}

صحیح _PyLexer_token_setup(ساختار tok_state *tok, ساختار token *token, صحیح type, _PyTok_Off start, _PyTok_Off end);

تهی _PyLexer_ImplyDedents(ساختار tok_state *);

تهی _PyTokenizer_Free(ساختار tok_state *);
ftstring_state *Py_Lexer_افزودن_FTString(ساختار tok_state *);
تهی Py_Lexer_برداشت_FTString(ساختار tok_state *);


#endif
