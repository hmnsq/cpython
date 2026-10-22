#ifndef _PY_LEXER_H_
#define _PY_LEXER_H_

#ଅନ୍ତର୍ଭାଵ "object.h"
#ଅନ୍ତର୍ଭାଵ "../tokenizer/source.h"
#ଅନ୍ତର୍ଭାଵ "../tokenizer/tokenizer.h"

#define MAXINDENT 100       /* Max indentation level */
#define MAXLEVEL 200        /* Max parentheses level */
#define MAXFTSTRINGLEVEL 150
#define FTSTRING_ଗଦା_INLINE_CAPACITY 1

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
    FTSTRING_MODE_ମଧ୍ୟ,
    FTSTRING_MODE_EXPRESSION,
    FTSTRING_MODE_FORMAT_SPEC,
} ftstring_mode;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
    FSTRING,
    RAW_FSTRING,
    TSTRING,
    RAW_TSTRING,
} ftstring_kind;

#define MAX_ପଦସମଷ୍ଟି_NESTING 3

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା _tokenizer_comments {
    Py_ssize_t ଗଣନା_2;
    Py_ssize_t capacity;
    _PyTok_Span spans[];
} tokenizer_comments;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା _ftstring_state {
    ftstring_mode mode;
    ftstring_kind kind;
    ଅକ୍ଷର quote;
    unsigned ଅକ୍ଷର quote_size;
    unsigned ଅକ୍ଷର debug_ପଦସମଷ୍ଟି;
    unsigned ଅକ୍ଷର replacement_depth;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା paren_level;
    _PyTok_Off start;
    _PyTok_Loc start_loc;
    _PyTok_Span ପଦସମଷ୍ଟି_span;
    tokenizer_comments *comments;
} ftstring_state;

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyLexer_IsTString(ftstring_kind kind)
{
    ଫେରନ୍ତୁ kind == TSTRING || kind == RAW_TSTRING;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyLexer_IsRawString(ftstring_kind kind)
{
    ଫେରନ୍ତୁ kind == RAW_FSTRING || kind == RAW_TSTRING;
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା column;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା alternate_column;
} indentation_level;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pending;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_bol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା comment_newline;
    indentation_level ଗଦା[MAXINDENT];
} lexer_layout_state;

/* Tokenizer state */
ସଂରଚନା tok_state {
    _PyTok_Off buf_offset;
    _PyTok_Off cur;
    _PyTok_Off inp;
    _PyTok_Off start;
    _PyTok_Off ଧାଡ଼ି_start;
    _PyTok_SourceText source;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା done;           /* E_OK normally, E_EOF at EOF, otherwise error code */
    /* NB If done != E_OK, cur must be == inp!!! */
    lexer_layout_state layout;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno;         /* Current line number */
    _PyTok_Loc start_loc;
    _PyTokenizer_Diagnostic diagnostic;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା level;          /* () [] {} Parentheses nesting level */
            /* Used to allow free continuations inside them */
    ଅକ୍ଷର parenstack[MAXLEVEL];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା parenlinenostack[MAXLEVEL];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା parencolstack[MAXLEVEL];
    PyObject *filename;
    PyObject *module;
    ସଂରଚନା _PyTok_Reader *reader;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା type_comments;      /* Whether to look for type comments */

    ftstring_state *ftstring_ଗଦା;
    ftstring_state ftstring_ଗଦା_inline[FTSTRING_ଗଦା_INLINE_CAPACITY];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ftstring_depth;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ftstring_capacity;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା tok_extra_tokens;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା implicit_newline;
#ifdef Py_DEBUG
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା debug;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା barry_as_bdfl;
    unsigned ଅକ୍ଷର allow_incomplete_ନିବେଶ;
    unsigned ଅକ୍ଷର needs_newline;
};

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ftstring_state *
_PyLexer_CurrentFTString(ସଂରଚନା tok_state *tok)
{
    ପ୍ରତିଜ୍ଞା(tok->ftstring_ଗଦା != ଖାଲି);
    ପ୍ରତିଜ୍ଞା(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    ଯଦି (tok->ftstring_depth == 0) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ &tok->ftstring_ଗଦା[tok->ftstring_depth - 1];
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର
_PyLexer_StringPrefix(ftstring_kind kind)
{
    ଫେରନ୍ତୁ _PyLexer_IsTString(kind) ? 't' : 'f';
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyLexer_FTStringBracketDepth(ଅଚଳ ସଂରଚନା tok_state *tok,
                              ଅଚଳ ftstring_state *state)
{
    ଫେରନ୍ତୁ tok->level - state->paren_level;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyLexer_ByteColumn(ଅଚଳ ସଂରଚନା tok_state *tok)
{
    ପ୍ରତିଜ୍ଞା(tok->ଧାଡ଼ି_start >= 0);
    ପ୍ରତିଜ୍ଞା(tok->cur >= tok->ଧାଡ଼ି_start);
    Py_ssize_t column = tok->cur - tok->ଧାଡ଼ି_start;
    ପ୍ରତିଜ୍ଞା(column <= INT_MAX);
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)column;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyLexer_token_setup(ସଂରଚନା tok_state *tok, ସଂରଚନା token *token, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, _PyTok_Off start, _PyTok_Off end);

ଶୂନ୍ୟ _PyLexer_ImplyDedents(ସଂରଚନା tok_state *);

ଶୂନ୍ୟ _PyTokenizer_Free(ସଂରଚନା tok_state *);
ftstring_state *Py_Lexer_ଠେଲ_FTString(ସଂରଚନା tok_state *);
ଶୂନ୍ୟ Py_Lexer_କାଢ଼_FTString(ସଂରଚନା tok_state *);


#endif
