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
    puur_naa_nka paren_level;
    _PyTok_Off start;
    _PyTok_Loc start_loc;
    _PyTok_Span expr_span;
    tokenizer_comments *comments;
} ftstring_state;

static anta_hstha puur_naa_nka
_PyLexer_IsTString(ftstring_kind kind)
{
    ᰀᰧᰌ kind == TSTRING || kind == RAW_TSTRING;
}

static anta_hstha puur_naa_nka
_PyLexer_IsRawString(ftstring_kind kind)
{
    ᰀᰧᰌ kind == RAW_FSTRING || kind == RAW_TSTRING;
}

typedef sa_nracanaa {
    puur_naa_nka column;
    puur_naa_nka alternate_column;
} indentation_level;

typedef sa_nracanaa {
    puur_naa_nka depth;
    puur_naa_nka pending;
    puur_naa_nka at_bol;
    puur_naa_nka comment_newline;
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
    puur_naa_nka done;           /* E_OK normally, E_EOF at EOF, otherwise error code */
    /* NB If done != E_OK, cur must be == inp!!! */
    lexer_layout_state layout;
    puur_naa_nka lineno;         /* Current line number */
    _PyTok_Loc start_loc;
    _PyTokenizer_Diagnostic diagnostic;
    puur_naa_nka level;          /* () [] {} Parentheses nesting level */
            /* Used to allow free continuations inside them */
    var_na parenstack[MAXLEVEL];
    puur_naa_nka parenlinenostack[MAXLEVEL];
    puur_naa_nka parencolstack[MAXLEVEL];
    PyObject *filename;
    PyObject *module;
    sa_nracanaa _PyTok_Reader *reader;

    puur_naa_nka type_comments;      /* Whether to look for type comments */

    ftstring_state *ftstring_stack;
    ftstring_state ftstring_stack_inline[FTSTRING_STACK_INLINE_CAPACITY];
    puur_naa_nka ftstring_depth;
    puur_naa_nka ftstring_capacity;
    puur_naa_nka tok_extra_tokens;
    puur_naa_nka implicit_newline;
#ifdef Py_DEBUG
    puur_naa_nka debug;
#endif
    puur_naa_nka barry_as_bdfl;
    unsigned var_na allow_incomplete_input;
    unsigned var_na needs_newline;
};

static anta_hstha ftstring_state *
_PyLexer_CurrentFTString(sa_nracanaa tok_state *tok)
{
    assert(tok->ftstring_stack != NULL);
    assert(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    ᰀᰧ (tok->ftstring_depth == 0) {
        ᰀᰧᰌ NULL;
    }
    ᰀᰧᰌ &tok->ftstring_stack[tok->ftstring_depth - 1];
}

static anta_hstha var_na
_PyLexer_StringPrefix(ftstring_kind kind)
{
    ᰀᰧᰌ _PyLexer_IsTString(kind) ? 't' : 'f';
}

static anta_hstha puur_naa_nka
_PyLexer_FTStringBracketDepth(const sa_nracanaa tok_state *tok,
                              const ftstring_state *state)
{
    ᰀᰧᰌ tok->level - state->paren_level;
}

static anta_hstha puur_naa_nka
_PyLexer_ByteColumn(const sa_nracanaa tok_state *tok)
{
    assert(tok->line_start >= 0);
    assert(tok->cur >= tok->line_start);
    Py_ssize_t column = tok->cur - tok->line_start;
    assert(column <= INT_MAX);
    ᰀᰧᰌ (puur_naa_nka)column;
}

puur_naa_nka _PyLexer_token_setup(sa_nracanaa tok_state *tok, sa_nracanaa token *token, puur_naa_nka type, _PyTok_Off start, _PyTok_Off end);

void _PyLexer_ImplyDedents(sa_nracanaa tok_state *);

void _PyTokenizer_Free(sa_nracanaa tok_state *);
ftstring_state *_PyLexer_PushFTString(sa_nracanaa tok_state *);
void _PyLexer_PopFTString(sa_nracanaa tok_state *);


#endif
