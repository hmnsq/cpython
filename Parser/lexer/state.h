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
    цео paren_level;
    _PyTok_Off start;
    _PyTok_Loc start_loc;
    _PyTok_Span expr_span;
    tokenizer_comments *comments;
} ftstring_state;

static inline цео
_PyLexer_IsTString(ftstring_kind kind)
{
    врати kind == TSTRING || kind == RAW_TSTRING;
}

static inline цео
_PyLexer_IsRawString(ftstring_kind kind)
{
    врати kind == RAW_FSTRING || kind == RAW_TSTRING;
}

typedef struct {
    цео column;
    цео alternate_column;
} indentation_level;

typedef struct {
    цео depth;
    цео pending;
    цео at_bol;
    цео comment_newline;
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
    цео done;           /* E_OK normally, E_EOF at EOF, otherwise error code */
    /* NB If done != E_OK, cur must be == inp!!! */
    lexer_layout_state layout;
    цео lineno;         /* Current line number */
    _PyTok_Loc start_loc;
    _PyTokenizer_Diagnostic diagnostic;
    цео level;          /* () [] {} Parentheses nesting level */
            /* Used to allow free continuations inside them */
    char parenstack[MAXLEVEL];
    цео parenlinenostack[MAXLEVEL];
    цео parencolstack[MAXLEVEL];
    PyObject *filename;
    PyObject *module;
    struct _PyTok_Reader *reader;

    цео type_comments;      /* Whether to look for type comments */

    ftstring_state *ftstring_stack;
    ftstring_state ftstring_stack_inline[FTSTRING_STACK_INLINE_CAPACITY];
    цео ftstring_depth;
    цео ftstring_capacity;
    цео tok_extra_tokens;
    цео implicit_newline;
#ifdef Py_DEBUG
    цео debug;
#endif
    цео barry_as_bdfl;
    unsigned char allow_incomplete_input;
    unsigned char needs_newline;
};

static inline ftstring_state *
_PyLexer_CurrentFTString(struct tok_state *tok)
{
    assert(tok->ftstring_stack != NULL);
    assert(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    ако (tok->ftstring_depth == 0) {
        врати NULL;
    }
    врати &tok->ftstring_stack[tok->ftstring_depth - 1];
}

static inline char
_PyLexer_StringPrefix(ftstring_kind kind)
{
    врати _PyLexer_IsTString(kind) ? 't' : 'f';
}

static inline цео
_PyLexer_FTStringBracketDepth(const struct tok_state *tok,
                              const ftstring_state *state)
{
    врати tok->level - state->paren_level;
}

static inline цео
_PyLexer_ByteColumn(const struct tok_state *tok)
{
    assert(tok->line_start >= 0);
    assert(tok->cur >= tok->line_start);
    Py_ssize_t column = tok->cur - tok->line_start;
    assert(column <= INT_MAX);
    врати (цео)column;
}

цео _PyLexer_token_setup(struct tok_state *tok, struct token *token, цео type, _PyTok_Off start, _PyTok_Off end);

void _PyLexer_ImplyDedents(struct tok_state *);

void _PyTokenizer_Free(struct tok_state *);
ftstring_state *_PyLexer_PushFTString(struct tok_state *);
void _PyLexer_PopFTString(struct tok_state *);


#endif
