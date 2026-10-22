#ifndef _PY_LEXER_H_
#define _PY_LEXER_H_

#અન્તર્ભાવ "object.h"
#અન્તર્ભાવ "../tokenizer/source.h"
#અન્તર્ભાવ "../tokenizer/tokenizer.h"

#define MAXINDENT 100       /* Max indentation level */
#define MAXLEVEL 200        /* Max parentheses level */
#define MAXFTSTRINGLEVEL 150
#define FTSTRING_ઢગલો_INLINE_CAPACITY 1

પ્રકારવ્યાખ્યા ગણના {
    FTSTRING_MODE_મધ્ય,
    FTSTRING_MODE_EXPRESSION,
    FTSTRING_MODE_FORMAT_SPEC,
} ftstring_mode;

પ્રકારવ્યાખ્યા ગણના {
    FSTRING,
    RAW_FSTRING,
    TSTRING,
    RAW_TSTRING,
} ftstring_kind;

#define MAX_પદાવલિ_NESTING 3

પ્રકારવ્યાખ્યા સંરચના _tokenizer_comments {
    Py_ssize_t ગણતરી;
    Py_ssize_t capacity;
    _PyTok_Span spans[];
} tokenizer_comments;

પ્રકારવ્યાખ્યા સંરચના _ftstring_state {
    ftstring_mode mode;
    ftstring_kind kind;
    અક્ષર quote;
    unsigned અક્ષર quote_size;
    unsigned અક્ષર debug_પદાવલિ;
    unsigned અક્ષર replacement_depth;
    પૂર્ણાંક paren_level;
    _PyTok_Off start;
    _PyTok_Loc start_loc;
    _PyTok_Span પદાવલિ_span;
    tokenizer_comments *comments;
} ftstring_state;

સ્થિર અંતઃરેખ પૂર્ણાંક
_PyLexer_IsTString(ftstring_kind kind)
{
    પાછા kind == TSTRING || kind == RAW_TSTRING;
}

સ્થિર અંતઃરેખ પૂર્ણાંક
_PyLexer_IsRawString(ftstring_kind kind)
{
    પાછા kind == RAW_FSTRING || kind == RAW_TSTRING;
}

પ્રકારવ્યાખ્યા સંરચના {
    પૂર્ણાંક column;
    પૂર્ણાંક alternate_column;
} indentation_level;

પ્રકારવ્યાખ્યા સંરચના {
    પૂર્ણાંક depth;
    પૂર્ણાંક pending;
    પૂર્ણાંક at_bol;
    પૂર્ણાંક comment_newline;
    indentation_level ઢગલો[MAXINDENT];
} lexer_layout_state;

/* Tokenizer state */
સંરચના tok_state {
    _PyTok_Off buf_offset;
    _PyTok_Off cur;
    _PyTok_Off inp;
    _PyTok_Off start;
    _PyTok_Off લીટી_start;
    _PyTok_SourceText source;
    પૂર્ણાંક done;           /* E_OK normally, E_EOF at EOF, otherwise error code */
    /* NB If done != E_OK, cur must be == inp!!! */
    lexer_layout_state layout;
    પૂર્ણાંક lineno;         /* Current line number */
    _PyTok_Loc start_loc;
    _PyTokenizer_Diagnostic diagnostic;
    પૂર્ણાંક level;          /* () [] {} Parentheses nesting level */
            /* Used to allow free continuations inside them */
    અક્ષર parenstack[MAXLEVEL];
    પૂર્ણાંક parenlinenostack[MAXLEVEL];
    પૂર્ણાંક parencolstack[MAXLEVEL];
    PyObject *filename;
    PyObject *module;
    સંરચના _PyTok_Reader *reader;

    પૂર્ણાંક type_comments;      /* Whether to look for type comments */

    ftstring_state *ftstring_ઢગલો;
    ftstring_state ftstring_ઢગલો_inline[FTSTRING_ઢગલો_INLINE_CAPACITY];
    પૂર્ણાંક ftstring_depth;
    પૂર્ણાંક ftstring_capacity;
    પૂર્ણાંક tok_extra_tokens;
    પૂર્ણાંક implicit_newline;
#ifdef Py_DEBUG
    પૂર્ણાંક debug;
#endif
    પૂર્ણાંક barry_as_bdfl;
    unsigned અક્ષર allow_incomplete_આગત;
    unsigned અક્ષર needs_newline;
};

સ્થિર અંતઃરેખ ftstring_state *
_PyLexer_CurrentFTString(સંરચના tok_state *tok)
{
    પ્રતિજ્ઞા(tok->ftstring_ઢગલો != ખાલી);
    પ્રતિજ્ઞા(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    જો (tok->ftstring_depth == 0) {
        પાછા ખાલી;
    }
    પાછા &tok->ftstring_ઢગલો[tok->ftstring_depth - 1];
}

સ્થિર અંતઃરેખ અક્ષર
_PyLexer_StringPrefix(ftstring_kind kind)
{
    પાછા _PyLexer_IsTString(kind) ? 't' : 'f';
}

સ્થિર અંતઃરેખ પૂર્ણાંક
_PyLexer_FTStringBracketDepth(અચલ સંરચના tok_state *tok,
                              અચલ ftstring_state *state)
{
    પાછા tok->level - state->paren_level;
}

સ્થિર અંતઃરેખ પૂર્ણાંક
_PyLexer_ByteColumn(અચલ સંરચના tok_state *tok)
{
    પ્રતિજ્ઞા(tok->લીટી_start >= 0);
    પ્રતિજ્ઞા(tok->cur >= tok->લીટી_start);
    Py_ssize_t column = tok->cur - tok->લીટી_start;
    પ્રતિજ્ઞા(column <= INT_MAX);
    પાછા (પૂર્ણાંક)column;
}

પૂર્ણાંક _PyLexer_token_setup(સંરચના tok_state *tok, સંરચના token *token, પૂર્ણાંક type, _PyTok_Off start, _PyTok_Off end);

શૂન્ય _PyLexer_ImplyDedents(સંરચના tok_state *);

શૂન્ય _PyTokenizer_Free(સંરચના tok_state *);
ftstring_state *Py_Lexer_ધકેલો_FTString(સંરચના tok_state *);
શૂન્ય Py_Lexer_કાઢો_FTString(સંરચના tok_state *);


#endif
