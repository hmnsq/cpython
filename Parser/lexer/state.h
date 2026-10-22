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
    整数 paren_level;
    _PyTok_Off start;
    _PyTok_Loc start_loc;
    _PyTok_Span expr_span;
    tokenizer_comments *comments;
} ftstring_state;

static inline 整数
_PyLexer_IsTString(ftstring_kind kind)
{
    戻る kind == TSTRING || kind == RAW_TSTRING;
}

static inline 整数
_PyLexer_IsRawString(ftstring_kind kind)
{
    戻る kind == RAW_FSTRING || kind == RAW_TSTRING;
}

typedef struct {
    整数 column;
    整数 alternate_column;
} indentation_level;

typedef struct {
    整数 depth;
    整数 pending;
    整数 at_bol;
    整数 comment_newline;
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
    整数 done;           /* E_OK normally, E_EOF at EOF, otherwise error code */
    /* NB If done != E_OK, cur must be == inp!!! */
    lexer_layout_state layout;
    整数 lineno;         /* Current line number */
    _PyTok_Loc start_loc;
    _PyTokenizer_Diagnostic diagnostic;
    整数 level;          /* () [] {} Parentheses nesting level */
            /* Used to allow free continuations inside them */
    char parenstack[MAXLEVEL];
    整数 parenlinenostack[MAXLEVEL];
    整数 parencolstack[MAXLEVEL];
    PyObject *filename;
    PyObject *module;
    struct _PyTok_Reader *reader;

    整数 type_comments;      /* Whether to look for type comments */

    ftstring_state *ftstring_stack;
    ftstring_state ftstring_stack_inline[FTSTRING_STACK_INLINE_CAPACITY];
    整数 ftstring_depth;
    整数 ftstring_capacity;
    整数 tok_extra_tokens;
    整数 implicit_newline;
#ifdef Py_DEBUG
    整数 debug;
#endif
    整数 barry_as_bdfl;
    unsigned char allow_incomplete_input;
    unsigned char needs_newline;
};

static inline ftstring_state *
_PyLexer_CurrentFTString(struct tok_state *tok)
{
    assert(tok->ftstring_stack != NULL);
    assert(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    もし (tok->ftstring_depth == 0) {
        戻る NULL;
    }
    戻る &tok->ftstring_stack[tok->ftstring_depth - 1];
}

static inline char
_PyLexer_StringPrefix(ftstring_kind kind)
{
    戻る _PyLexer_IsTString(kind) ? 't' : 'f';
}

static inline 整数
_PyLexer_FTStringBracketDepth(const struct tok_state *tok,
                              const ftstring_state *state)
{
    戻る tok->level - state->paren_level;
}

static inline 整数
_PyLexer_ByteColumn(const struct tok_state *tok)
{
    assert(tok->line_start >= 0);
    assert(tok->cur >= tok->line_start);
    Py_ssize_t column = tok->cur - tok->line_start;
    assert(column <= INT_MAX);
    戻る (整数)column;
}

整数 _PyLexer_token_setup(struct tok_state *tok, struct token *token, 整数 type, _PyTok_Off start, _PyTok_Off end);

void _PyLexer_ImplyDedents(struct tok_state *);

void _PyTokenizer_Free(struct tok_state *);
ftstring_state *_PyLexer_PushFTString(struct tok_state *);
void _PyLexer_PopFTString(struct tok_state *);


#endif
