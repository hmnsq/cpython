#ifndef _PY_LEXER_H_
#define _PY_LEXER_H_

#അന്തര്ഭാവ "object.h"
#അന്തര്ഭാവ "../tokenizer/source.h"
#അന്തര്ഭാവ "../tokenizer/tokenizer.h"

#define MAXINDENT 100       /* Max indentation level */
#define MAXLEVEL 200        /* Max parentheses level */
#define MAXFTSTRINGLEVEL 150
#define FTSTRING_അട്ടി_INLINE_CAPACITY 1

തരനിർവചനം എണ്ണൽ {
    FTSTRING_MODE_നടുവ്,
    FTSTRING_MODE_EXPRESSION,
    FTSTRING_MODE_FORMAT_SPEC,
} ftstring_mode;

തരനിർവചനം എണ്ണൽ {
    FSTRING,
    RAW_FSTRING,
    TSTRING,
    RAW_TSTRING,
} ftstring_kind;

#define MAX_വ്യഞ്ജകം_NESTING 3

തരനിർവചനം ഘടന _tokenizer_comments {
    Py_ssize_t എണ്ണം;
    Py_ssize_t capacity;
    _PyTok_Span spans[];
} tokenizer_comments;

തരനിർവചനം ഘടന _ftstring_state {
    ftstring_mode mode;
    ftstring_kind kind;
    അക്ഷരം quote;
    unsigned അക്ഷരം quote_size;
    unsigned അക്ഷരം debug_വ്യഞ്ജകം;
    unsigned അക്ഷരം replacement_depth;
    പൂർണ്ണസംഖ്യ paren_level;
    _PyTok_Off start;
    _PyTok_Loc start_loc;
    _PyTok_Span വ്യഞ്ജകം_span;
    tokenizer_comments *comments;
} ftstring_state;

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ
_PyLexer_IsTString(ftstring_kind kind)
{
    തിരികെ kind == TSTRING || kind == RAW_TSTRING;
}

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ
_PyLexer_IsRawString(ftstring_kind kind)
{
    തിരികെ kind == RAW_FSTRING || kind == RAW_TSTRING;
}

തരനിർവചനം ഘടന {
    പൂർണ്ണസംഖ്യ column;
    പൂർണ്ണസംഖ്യ alternate_column;
} indentation_level;

തരനിർവചനം ഘടന {
    പൂർണ്ണസംഖ്യ depth;
    പൂർണ്ണസംഖ്യ pending;
    പൂർണ്ണസംഖ്യ at_bol;
    പൂർണ്ണസംഖ്യ comment_newline;
    indentation_level അട്ടി[MAXINDENT];
} lexer_layout_state;

/* Tokenizer state */
ഘടന tok_state {
    _PyTok_Off buf_offset;
    _PyTok_Off cur;
    _PyTok_Off inp;
    _PyTok_Off start;
    _PyTok_Off വരി_start;
    _PyTok_SourceText source;
    പൂർണ്ണസംഖ്യ done;           /* E_OK normally, E_EOF at EOF, otherwise error code */
    /* NB If done != E_OK, cur must be == inp!!! */
    lexer_layout_state layout;
    പൂർണ്ണസംഖ്യ lineno;         /* Current line number */
    _PyTok_Loc start_loc;
    _PyTokenizer_Diagnostic diagnostic;
    പൂർണ്ണസംഖ്യ level;          /* () [] {} Parentheses nesting level */
            /* Used to allow free continuations inside them */
    അക്ഷരം parenstack[MAXLEVEL];
    പൂർണ്ണസംഖ്യ parenlinenostack[MAXLEVEL];
    പൂർണ്ണസംഖ്യ parencolstack[MAXLEVEL];
    PyObject *filename;
    PyObject *module;
    ഘടന _PyTok_Reader *reader;

    പൂർണ്ണസംഖ്യ type_comments;      /* Whether to look for type comments */

    ftstring_state *ftstring_അട്ടി;
    ftstring_state ftstring_അട്ടി_inline[FTSTRING_അട്ടി_INLINE_CAPACITY];
    പൂർണ്ണസംഖ്യ ftstring_depth;
    പൂർണ്ണസംഖ്യ ftstring_capacity;
    പൂർണ്ണസംഖ്യ tok_extra_tokens;
    പൂർണ്ണസംഖ്യ implicit_newline;
#ifdef Py_DEBUG
    പൂർണ്ണസംഖ്യ debug;
#endif
    പൂർണ്ണസംഖ്യ barry_as_bdfl;
    unsigned അക്ഷരം allow_incomplete_നിവേശം;
    unsigned അക്ഷരം needs_newline;
};

സ്ഥിതം വരിയിൽ ftstring_state *
_PyLexer_CurrentFTString(ഘടന tok_state *tok)
{
    പ്രതിജ്ഞാ(tok->ftstring_അട്ടി != ശൂന്യ);
    പ്രതിജ്ഞാ(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    എങ്കിൽ (tok->ftstring_depth == 0) {
        തിരികെ ശൂന്യ;
    }
    തിരികെ &tok->ftstring_അട്ടി[tok->ftstring_depth - 1];
}

സ്ഥിതം വരിയിൽ അക്ഷരം
_PyLexer_StringPrefix(ftstring_kind kind)
{
    തിരികെ _PyLexer_IsTString(kind) ? 't' : 'f';
}

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ
_PyLexer_FTStringBracketDepth(സ്ഥിരം ഘടന tok_state *tok,
                              സ്ഥിരം ftstring_state *state)
{
    തിരികെ tok->level - state->paren_level;
}

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ
_PyLexer_ByteColumn(സ്ഥിരം ഘടന tok_state *tok)
{
    പ്രതിജ്ഞാ(tok->വരി_start >= 0);
    പ്രതിജ്ഞാ(tok->cur >= tok->വരി_start);
    Py_ssize_t column = tok->cur - tok->വരി_start;
    പ്രതിജ്ഞാ(column <= INT_MAX);
    തിരികെ (പൂർണ്ണസംഖ്യ)column;
}

പൂർണ്ണസംഖ്യ _PyLexer_token_setup(ഘടന tok_state *tok, ഘടന token *token, പൂർണ്ണസംഖ്യ type, _PyTok_Off start, _PyTok_Off end);

ശൂന്യം _PyLexer_ImplyDedents(ഘടന tok_state *);

ശൂന്യം _PyTokenizer_Free(ഘടന tok_state *);
ftstring_state *Py_Lexer_തള്ളുക_FTString(ഘടന tok_state *);
ശൂന്യം Py_Lexer_എടുക്കുക_FTString(ഘടന tok_state *);


#endif
