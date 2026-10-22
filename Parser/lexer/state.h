#ifndef _PY_LEXER_H_
#define _PY_LEXER_H_

#include "object.h"
#include "../tokenizer/source.h"
#include "../tokenizer/tokenizer.h"

#define MAXINDENT 100       /* Max indentation level */
#define MAXLEVEL 200        /* Max parentheses level */
#define MAXFTSTRINGLEVEL 150
#define FTSTRING_STACK_INLINE_CAPACITY 1

typedef ga_nanaa {
    FTSTRING_MODE_MIDDLE,
    FTSTRING_MODE_EXPRESSION,
    FTSTRING_MODE_FORMAT_SPEC,
} ftstring_mode;

typedef ga_nanaa {
    FSTRING,
    RAW_FSTRING,
    TSTRING,
    RAW_TSTRING,
} ftstring_kind;

#define MAX_EXPR_NESTING 3

typedef sa_nracanaa _tokenizer_comments {
    Py_ssize_t count;
    Py_ssize_t capacity;
    _PyTok_Span spans[];
} tokenizer_comments;

typedef sa_nracanaa _ftstring_state {
    ftstring_mode mode;
    ftstring_kind kind;
    var_na quote;
    unsigned var_na quote_size;
    unsigned var_na debug_expr;
    unsigned var_na replacement_depth;
    ကိန်းပြည့် paren_level;
    _PyTok_Off start;
    _PyTok_Loc start_loc;
    _PyTok_Span expr_span;
    tokenizer_comments *comments;
} ftstring_state;

static anta_hstha ကိန်းပြည့်
_PyLexer_IsTString(ftstring_kind kind)
{
    ပြန်ပေး kind == TSTRING || kind == RAW_TSTRING;
}

static anta_hstha ကိန်းပြည့်
_PyLexer_IsRawString(ftstring_kind kind)
{
    ပြန်ပေး kind == RAW_FSTRING || kind == RAW_TSTRING;
}

typedef sa_nracanaa {
    ကိန်းပြည့် column;
    ကိန်းပြည့် alternate_column;
} indentation_level;

typedef sa_nracanaa {
    ကိန်းပြည့် depth;
    ကိန်းပြည့် pending;
    ကိန်းပြည့် at_bol;
    ကိန်းပြည့် comment_newline;
    indentation_level stack[MAXINDENT];
} lexer_layout_state;

/* Tokenizer state */
sa_nracanaa tok_state {
    _PyTok_Off buf_offset;
    _PyTok_Off cur;
    _PyTok_Off inp;
    _PyTok_Off start;
    _PyTok_Off line_start;
    _PyTok_SourceText source;
    ကိန်းပြည့် done;           /* E_OK normally, E_EOF at EOF, otherwise error code */
    /* NB If done != E_OK, cur must be == inp!!! */
    lexer_layout_state layout;
    ကိန်းပြည့် lineno;         /* Current line number */
    _PyTok_Loc start_loc;
    _PyTokenizer_Diagnostic diagnostic;
    ကိန်းပြည့် level;          /* () [] {} Parentheses nesting level */
            /* Used to allow free continuations inside them */
    var_na parenstack[MAXLEVEL];
    ကိန်းပြည့် parenlinenostack[MAXLEVEL];
    ကိန်းပြည့် parencolstack[MAXLEVEL];
    PyObject *filename;
    PyObject *module;
    sa_nracanaa _PyTok_Reader *reader;

    ကိန်းပြည့် type_comments;      /* Whether to look for type comments */

    ftstring_state *ftstring_stack;
    ftstring_state ftstring_stack_inline[FTSTRING_STACK_INLINE_CAPACITY];
    ကိန်းပြည့် ftstring_depth;
    ကိန်းပြည့် ftstring_capacity;
    ကိန်းပြည့် tok_extra_tokens;
    ကိန်းပြည့် implicit_newline;
#ifdef Py_DEBUG
    ကိန်းပြည့် debug;
#endif
    ကိန်းပြည့် barry_as_bdfl;
    unsigned var_na allow_incomplete_input;
    unsigned var_na needs_newline;
};

static anta_hstha ftstring_state *
_PyLexer_CurrentFTString(sa_nracanaa tok_state *tok)
{
    assert(tok->ftstring_stack != NULL);
    assert(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    အကယ်၍ (tok->ftstring_depth == 0) {
        ပြန်ပေး NULL;
    }
    ပြန်ပေး &tok->ftstring_stack[tok->ftstring_depth - 1];
}

static anta_hstha var_na
_PyLexer_StringPrefix(ftstring_kind kind)
{
    ပြန်ပေး _PyLexer_IsTString(kind) ? 't' : 'f';
}

static anta_hstha ကိန်းပြည့်
_PyLexer_FTStringBracketDepth(const sa_nracanaa tok_state *tok,
                              const ftstring_state *state)
{
    ပြန်ပေး tok->level - state->paren_level;
}

static anta_hstha ကိန်းပြည့်
_PyLexer_ByteColumn(const sa_nracanaa tok_state *tok)
{
    assert(tok->line_start >= 0);
    assert(tok->cur >= tok->line_start);
    Py_ssize_t column = tok->cur - tok->line_start;
    assert(column <= INT_MAX);
    ပြန်ပေး (ကိန်းပြည့်)column;
}

ကိန်းပြည့် _PyLexer_token_setup(sa_nracanaa tok_state *tok, sa_nracanaa token *token, ကိန်းပြည့် type, _PyTok_Off start, _PyTok_Off end);

void _PyLexer_ImplyDedents(sa_nracanaa tok_state *);

void _PyTokenizer_Free(sa_nracanaa tok_state *);
ftstring_state *_PyLexer_PushFTString(sa_nracanaa tok_state *);
void _PyLexer_PopFTString(sa_nracanaa tok_state *);


#endif
