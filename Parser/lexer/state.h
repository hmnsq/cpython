#ifndef _PY_LEXER_H_
#define _PY_LEXER_H_

#include "object.h"
#include "../tokenizer/source.h"
#include "../tokenizer/tokenizer.h"

#define MAXINDENT 100       /* Max indentation level */
#define MAXLEVEL 200        /* Max parentheses level */
#define MAXFTSTRINGLEVEL 150
#define FTSTRING_STACK_INLINE_CAPACITY 1

typedef enum {
    FTSTRING_MODE_MIDDLE,
    FTSTRING_MODE_EXPRESSION,
    FTSTRING_MODE_FORMAT_SPEC,
} ftstring_mode;

typedef enum {
    FSTRING,
    RAW_FSTRING,
    TSTRING,
    RAW_TSTRING,
} ftstring_kind;

#define MAX_EXPR_NESTING 3

typedef struct _tokenizer_comments {
    Py_ssize_t count;
    Py_ssize_t capacity;
    _PyTok_Span spans[];
} tokenizer_comments;

typedef struct _ftstring_state {
    ftstring_mode mode;
    ftstring_kind kind;
    char quote;
    unsigned char quote_size;
    unsigned char debug_expr;
    unsigned char replacement_depth;
    số_nguyên paren_level;
    _PyTok_Off start;
    _PyTok_Loc start_loc;
    _PyTok_Span expr_span;
    tokenizer_comments *comments;
} ftstring_state;

static inline số_nguyên
_PyLexer_IsTString(ftstring_kind kind)
{
    trả_về kind == TSTRING || kind == RAW_TSTRING;
}

static inline số_nguyên
_PyLexer_IsRawString(ftstring_kind kind)
{
    trả_về kind == RAW_FSTRING || kind == RAW_TSTRING;
}

typedef struct {
    số_nguyên column;
    số_nguyên alternate_column;
} indentation_level;

typedef struct {
    số_nguyên depth;
    số_nguyên pending;
    số_nguyên at_bol;
    số_nguyên comment_newline;
    indentation_level stack[MAXINDENT];
} lexer_layout_state;

/* Tokenizer state */
struct tok_state {
    _PyTok_Off buf_offset;
    _PyTok_Off cur;
    _PyTok_Off inp;
    _PyTok_Off start;
    _PyTok_Off line_start;
    _PyTok_SourceText source;
    số_nguyên done;           /* E_OK normally, E_EOF at EOF, otherwise error code */
    /* NB If done != E_OK, cur must be == inp!!! */
    lexer_layout_state layout;
    số_nguyên lineno;         /* Current line number */
    _PyTok_Loc start_loc;
    _PyTokenizer_Diagnostic diagnostic;
    số_nguyên level;          /* () [] {} Parentheses nesting level */
            /* Used to allow free continuations inside them */
    char parenstack[MAXLEVEL];
    số_nguyên parenlinenostack[MAXLEVEL];
    số_nguyên parencolstack[MAXLEVEL];
    PyObject *filename;
    PyObject *module;
    struct _PyTok_Reader *reader;

    số_nguyên type_comments;      /* Whether to look for type comments */

    ftstring_state *ftstring_stack;
    ftstring_state ftstring_stack_inline[FTSTRING_STACK_INLINE_CAPACITY];
    số_nguyên ftstring_depth;
    số_nguyên ftstring_capacity;
    số_nguyên tok_extra_tokens;
    số_nguyên implicit_newline;
#ifdef Py_DEBUG
    số_nguyên debug;
#endif
    số_nguyên barry_as_bdfl;
    unsigned char allow_incomplete_input;
    unsigned char needs_newline;
};

static inline ftstring_state *
_PyLexer_CurrentFTString(struct tok_state *tok)
{
    assert(tok->ftstring_stack != NULL);
    assert(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    nếu (tok->ftstring_depth == 0) {
        trả_về NULL;
    }
    trả_về &tok->ftstring_stack[tok->ftstring_depth - 1];
}

static inline char
_PyLexer_StringPrefix(ftstring_kind kind)
{
    trả_về _PyLexer_IsTString(kind) ? 't' : 'f';
}

static inline số_nguyên
_PyLexer_FTStringBracketDepth(const struct tok_state *tok,
                              const ftstring_state *state)
{
    trả_về tok->level - state->paren_level;
}

static inline số_nguyên
_PyLexer_ByteColumn(const struct tok_state *tok)
{
    assert(tok->line_start >= 0);
    assert(tok->cur >= tok->line_start);
    Py_ssize_t column = tok->cur - tok->line_start;
    assert(column <= INT_MAX);
    trả_về (số_nguyên)column;
}

số_nguyên _PyLexer_token_setup(struct tok_state *tok, struct token *token, số_nguyên type, _PyTok_Off start, _PyTok_Off end);

void _PyLexer_ImplyDedents(struct tok_state *);

void _PyTokenizer_Free(struct tok_state *);
ftstring_state *_PyLexer_PushFTString(struct tok_state *);
void _PyLexer_PopFTString(struct tok_state *);


#endif
