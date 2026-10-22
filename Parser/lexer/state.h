#ifndef _PY_LEXER_H_
#define _PY_LEXER_H_

#অন্তর্ভাব "object.h"
#অন্তর্ভাব "../tokenizer/source.h"
#অন্তর্ভাব "../tokenizer/tokenizer.h"

#define MAXINDENT 100       /* Max indentation level */
#define MAXLEVEL 200        /* Max parentheses level */
#define MAXFTSTRINGLEVEL 150
#define FTSTRING_স্তূপ_INLINE_CAPACITY 1

প্রকারলেখ গণনা {
    FTSTRING_MODE_মধ্য,
    FTSTRING_MODE_EXPRESSION,
    FTSTRING_MODE_FORMAT_SPEC,
} ftstring_mode;

প্রকারলেখ গণনা {
    FSTRING,
    RAW_FSTRING,
    TSTRING,
    RAW_TSTRING,
} ftstring_kind;

#define MAX_রাশিমালা_NESTING 3

প্রকারলেখ কাঠামো _tokenizer_comments {
    Py_ssize_t গুনতি;
    Py_ssize_t capacity;
    _PyTok_Span spans[];
} tokenizer_comments;

প্রকারলেখ কাঠামো _ftstring_state {
    ftstring_mode mode;
    ftstring_kind kind;
    অক্ষর quote;
    unsigned অক্ষর quote_আকার;
    unsigned অক্ষর debug_রাশিমালা;
    unsigned অক্ষর replacement_depth;
    পূর্ণসংখ্যা paren_level;
    _PyTok_Off start;
    _PyTok_Loc start_loc;
    _PyTok_Span রাশিমালা_span;
    tokenizer_comments *comments;
} ftstring_state;

স্থির অন্তঃরেখ পূর্ণসংখ্যা
_PyLexer_IsTString(ftstring_kind kind)
{
    ফেরত kind == TSTRING || kind == RAW_TSTRING;
}

স্থির অন্তঃরেখ পূর্ণসংখ্যা
_PyLexer_IsRawString(ftstring_kind kind)
{
    ফেরত kind == RAW_FSTRING || kind == RAW_TSTRING;
}

প্রকারলেখ কাঠামো {
    পূর্ণসংখ্যা column;
    পূর্ণসংখ্যা alternate_column;
} indentation_level;

প্রকারলেখ কাঠামো {
    পূর্ণসংখ্যা depth;
    পূর্ণসংখ্যা pending;
    পূর্ণসংখ্যা at_bol;
    পূর্ণসংখ্যা comment_newline;
    indentation_level স্তূপ[MAXINDENT];
} lexer_layout_state;

/* Tokenizer state */
কাঠামো tok_state {
    _PyTok_Off buf_offset;
    _PyTok_Off cur;
    _PyTok_Off inp;
    _PyTok_Off start;
    _PyTok_Off সারি_start;
    _PyTok_SourceText source;
    পূর্ণসংখ্যা done;           /* E_OK normally, E_EOF at EOF, otherwise error code */
    /* NB If done != E_OK, cur must be == inp!!! */
    lexer_layout_state layout;
    পূর্ণসংখ্যা lineno;         /* Current line number */
    _PyTok_Loc start_loc;
    _PyTokenizer_Diagnostic diagnostic;
    পূর্ণসংখ্যা level;          /* () [] {} Parentheses nesting level */
            /* Used to allow free continuations inside them */
    অক্ষর parenstack[MAXLEVEL];
    পূর্ণসংখ্যা parenlinenostack[MAXLEVEL];
    পূর্ণসংখ্যা parencolstack[MAXLEVEL];
    PyObject *filename;
    PyObject *module;
    কাঠামো _PyTok_Reader *reader;

    পূর্ণসংখ্যা type_comments;      /* Whether to look for type comments */

    ftstring_state *ftstring_স্তূপ;
    ftstring_state ftstring_স্তূপ_inline[FTSTRING_স্তূপ_INLINE_CAPACITY];
    পূর্ণসংখ্যা ftstring_depth;
    পূর্ণসংখ্যা ftstring_capacity;
    পূর্ণসংখ্যা tok_extra_tokens;
    পূর্ণসংখ্যা implicit_newline;
#ifdef Py_DEBUG
    পূর্ণসংখ্যা debug;
#endif
    পূর্ণসংখ্যা barry_as_bdfl;
    unsigned অক্ষর allow_incomplete_প্রবেশ;
    unsigned অক্ষর needs_newline;
};

স্থির অন্তঃরেখ ftstring_state *
_PyLexer_CurrentFTString(কাঠামো tok_state *tok)
{
    প্রতিজ্ঞা(tok->ftstring_স্তূপ != ফাঁকা);
    প্রতিজ্ঞা(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    যদি (tok->ftstring_depth == 0) {
        ফেরত ফাঁকা;
    }
    ফেরত &tok->ftstring_স্তূপ[tok->ftstring_depth - 1];
}

স্থির অন্তঃরেখ অক্ষর
_PyLexer_StringPrefix(ftstring_kind kind)
{
    ফেরত _PyLexer_IsTString(kind) ? 't' : 'f';
}

স্থির অন্তঃরেখ পূর্ণসংখ্যা
_PyLexer_FTStringBracketDepth(ধ্রুবক কাঠামো tok_state *tok,
                              ধ্রুবক ftstring_state *state)
{
    ফেরত tok->level - state->paren_level;
}

স্থির অন্তঃরেখ পূর্ণসংখ্যা
_PyLexer_ByteColumn(ধ্রুবক কাঠামো tok_state *tok)
{
    প্রতিজ্ঞা(tok->সারি_start >= 0);
    প্রতিজ্ঞা(tok->cur >= tok->সারি_start);
    Py_ssize_t column = tok->cur - tok->সারি_start;
    প্রতিজ্ঞা(column <= INT_MAX);
    ফেরত (পূর্ণসংখ্যা)column;
}

পূর্ণসংখ্যা _PyLexer_token_setup(কাঠামো tok_state *tok, কাঠামো token *token, পূর্ণসংখ্যা type, _PyTok_Off start, _PyTok_Off end);

শূন্য _PyLexer_ImplyDedents(কাঠামো tok_state *);

শূন্য _PyTokenizer_Free(কাঠামো tok_state *);
ftstring_state *Py_Lexer_ঠেলো_FTString(কাঠামো tok_state *);
শূন্য Py_Lexer_বের_করো_FTString(কাঠামো tok_state *);


#endif
