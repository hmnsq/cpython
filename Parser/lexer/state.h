#ifndef _PY_LEXER_H_
#define _PY_LEXER_H_

#অন্তর্ভাৱ "object.h"
#অন্তর্ভাৱ "../tokenizer/source.h"
#অন্তর্ভাৱ "../tokenizer/tokenizer.h"

#define MAXINDENT 100       /* Max indentation level */
#define MAXLEVEL 200        /* Max parentheses level */
#define MAXFTSTRINGLEVEL 150
#define FTSTRING_স্তূপ_INLINE_CAPACITY 1

প্রকারনির্দেশ গণনা {
    FTSTRING_MODE_মধ্য,
    FTSTRING_MODE_EXPRESSION,
    FTSTRING_MODE_FORMAT_SPEC,
} ftstring_mode;

প্রকারনির্দেশ গণনা {
    FSTRING,
    RAW_FSTRING,
    TSTRING,
    RAW_TSTRING,
} ftstring_kind;

#define MAX_ৰাশিমালা_NESTING 3

প্রকারনির্দেশ সংরচনা _tokenizer_comments {
    Py_ssize_t গুনতি;
    Py_ssize_t capacity;
    _PyTok_Span spans[];
} tokenizer_comments;

প্রকারনির্দেশ সংরচনা _ftstring_state {
    ftstring_mode mode;
    ftstring_kind kind;
    ৱর্ণ quote;
    unsigned ৱর্ণ quote_size;
    unsigned ৱর্ণ debug_ৰাশিমালা;
    unsigned ৱর্ণ replacement_depth;
    পূর্ণাঙ্ক paren_level;
    _PyTok_Off start;
    _PyTok_Loc start_loc;
    _PyTok_Span ৰাশিমালা_span;
    tokenizer_comments *comments;
} ftstring_state;

স্থির অন্তঃস্থ পূর্ণাঙ্ক
_PyLexer_IsTString(ftstring_kind kind)
{
    ঘূৰাই kind == TSTRING || kind == RAW_TSTRING;
}

স্থির অন্তঃস্থ পূর্ণাঙ্ক
_PyLexer_IsRawString(ftstring_kind kind)
{
    ঘূৰাই kind == RAW_FSTRING || kind == RAW_TSTRING;
}

প্রকারনির্দেশ সংরচনা {
    পূর্ণাঙ্ক column;
    পূর্ণাঙ্ক alternate_column;
} indentation_level;

প্রকারনির্দেশ সংরচনা {
    পূর্ণাঙ্ক depth;
    পূর্ণাঙ্ক pending;
    পূর্ণাঙ্ক at_bol;
    পূর্ণাঙ্ক comment_newline;
    indentation_level স্তূপ[MAXINDENT];
} lexer_layout_state;

/* Tokenizer state */
সংরচনা tok_state {
    _PyTok_Off buf_offset;
    _PyTok_Off cur;
    _PyTok_Off inp;
    _PyTok_Off start;
    _PyTok_Off সাৰি_start;
    _PyTok_SourceText source;
    পূর্ণাঙ্ক done;           /* E_OK normally, E_EOF at EOF, otherwise error code */
    /* NB If done != E_OK, cur must be == inp!!! */
    lexer_layout_state layout;
    পূর্ণাঙ্ক lineno;         /* Current line number */
    _PyTok_Loc start_loc;
    _PyTokenizer_Diagnostic diagnostic;
    পূর্ণাঙ্ক level;          /* () [] {} Parentheses nesting level */
            /* Used to allow free continuations inside them */
    ৱর্ণ parenstack[MAXLEVEL];
    পূর্ণাঙ্ক parenlinenostack[MAXLEVEL];
    পূর্ণাঙ্ক parencolstack[MAXLEVEL];
    PyObject *filename;
    PyObject *module;
    সংরচনা _PyTok_Reader *reader;

    পূর্ণাঙ্ক type_comments;      /* Whether to look for type comments */

    ftstring_state *ftstring_স্তূপ;
    ftstring_state ftstring_স্তূপ_inline[FTSTRING_স্তূপ_INLINE_CAPACITY];
    পূর্ণাঙ্ক ftstring_depth;
    পূর্ণাঙ্ক ftstring_capacity;
    পূর্ণাঙ্ক tok_extra_tokens;
    পূর্ণাঙ্ক implicit_newline;
#ifdef Py_DEBUG
    পূর্ণাঙ্ক debug;
#endif
    পূর্ণাঙ্ক barry_as_bdfl;
    unsigned ৱর্ণ allow_incomplete_প্ৰবেশ;
    unsigned ৱর্ণ needs_newline;
};

স্থির অন্তঃস্থ ftstring_state *
_PyLexer_CurrentFTString(সংরচনা tok_state *tok)
{
    প্রতিজ্ঞা(tok->ftstring_স্তূপ != রিক্ত);
    প্রতিজ্ঞা(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    যদি (tok->ftstring_depth == 0) {
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই &tok->ftstring_স্তূপ[tok->ftstring_depth - 1];
}

স্থির অন্তঃস্থ ৱর্ণ
_PyLexer_StringPrefix(ftstring_kind kind)
{
    ঘূৰাই _PyLexer_IsTString(kind) ? 't' : 'f';
}

স্থির অন্তঃস্থ পূর্ণাঙ্ক
_PyLexer_FTStringBracketDepth(ধ্রুৱ সংরচনা tok_state *tok,
                              ধ্রুৱ ftstring_state *state)
{
    ঘূৰাই tok->level - state->paren_level;
}

স্থির অন্তঃস্থ পূর্ণাঙ্ক
_PyLexer_ByteColumn(ধ্রুৱ সংরচনা tok_state *tok)
{
    প্রতিজ্ঞা(tok->সাৰি_start >= 0);
    প্রতিজ্ঞা(tok->cur >= tok->সাৰি_start);
    Py_ssize_t column = tok->cur - tok->সাৰি_start;
    প্রতিজ্ঞা(column <= INT_MAX);
    ঘূৰাই (পূর্ণাঙ্ক)column;
}

পূর্ণাঙ্ক _PyLexer_token_setup(সংরচনা tok_state *tok, সংরচনা token *token, পূর্ণাঙ্ক type, _PyTok_Off start, _PyTok_Off end);

শূন্য _PyLexer_ImplyDedents(সংরচনা tok_state *);

শূন্য _PyTokenizer_Free(সংরচনা tok_state *);
ftstring_state *Py_Lexer_ঠেলো_FTString(সংরচনা tok_state *);
শূন্য Py_Lexer_বেৰ_কৰো_FTString(সংরচনা tok_state *);


#endif
