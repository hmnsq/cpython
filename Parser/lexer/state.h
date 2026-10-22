#ifndef _PY_LEXER_H_
#define _PY_LEXER_H_

#ಅನ್ತರ್ಭಾವ "object.h"
#ಅನ್ತರ್ಭಾವ "../tokenizer/source.h"
#ಅನ್ತರ್ಭಾವ "../tokenizer/tokenizer.h"

#define MAXINDENT 100       /* Max indentation level */
#define MAXLEVEL 200        /* Max parentheses level */
#define MAXFTSTRINGLEVEL 150
#define FTSTRING_ರಾಶಿ_INLINE_CAPACITY 1

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
    FTSTRING_MODE_ಮಧ್ಯ,
    FTSTRING_MODE_EXPRESSION,
    FTSTRING_MODE_FORMAT_SPEC,
} ftstring_mode;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
    FSTRING,
    RAW_FSTRING,
    TSTRING,
    RAW_TSTRING,
} ftstring_kind;

#define MAX_ಅಭಿವ್ಯಕ್ತಿ_NESTING 3

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ _tokenizer_comments {
    Py_ssize_t ಎಣಿಕೆ;
    Py_ssize_t capacity;
    _PyTok_Span spans[];
} tokenizer_comments;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ _ftstring_state {
    ftstring_mode mode;
    ftstring_kind kind;
    ವರ್ಣ quote;
    unsigned ವರ್ಣ quote_size;
    unsigned ವರ್ಣ debug_ಅಭಿವ್ಯಕ್ತಿ;
    unsigned ವರ್ಣ replacement_depth;
    ಪೂರ್ಣಾಙ್ಕ paren_level;
    _PyTok_Off start;
    _PyTok_Loc start_loc;
    _PyTok_Span ಅಭಿವ್ಯಕ್ತಿ_span;
    tokenizer_comments *comments;
} ftstring_state;

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ
_PyLexer_IsTString(ftstring_kind kind)
{
    ಪ್ರತ್ಯಾಗಮ kind == TSTRING || kind == RAW_TSTRING;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ
_PyLexer_IsRawString(ftstring_kind kind)
{
    ಪ್ರತ್ಯಾಗಮ kind == RAW_FSTRING || kind == RAW_TSTRING;
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಪೂರ್ಣಾಙ್ಕ column;
    ಪೂರ್ಣಾಙ್ಕ alternate_column;
} indentation_level;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಪೂರ್ಣಾಙ್ಕ depth;
    ಪೂರ್ಣಾಙ್ಕ pending;
    ಪೂರ್ಣಾಙ್ಕ at_bol;
    ಪೂರ್ಣಾಙ್ಕ comment_newline;
    indentation_level ರಾಶಿ[MAXINDENT];
} lexer_layout_state;

/* Tokenizer state */
ಸಂರಚನಾ tok_state {
    _PyTok_Off buf_offset;
    _PyTok_Off cur;
    _PyTok_Off inp;
    _PyTok_Off start;
    _PyTok_Off ಸಾಲು_start;
    _PyTok_SourceText source;
    ಪೂರ್ಣಾಙ್ಕ done;           /* E_OK normally, E_EOF at EOF, otherwise error code */
    /* NB If done != E_OK, cur must be == inp!!! */
    lexer_layout_state layout;
    ಪೂರ್ಣಾಙ್ಕ lineno;         /* Current line number */
    _PyTok_Loc start_loc;
    _PyTokenizer_Diagnostic diagnostic;
    ಪೂರ್ಣಾಙ್ಕ level;          /* () [] {} Parentheses nesting level */
            /* Used to allow free continuations inside them */
    ವರ್ಣ parenstack[MAXLEVEL];
    ಪೂರ್ಣಾಙ್ಕ parenlinenostack[MAXLEVEL];
    ಪೂರ್ಣಾಙ್ಕ parencolstack[MAXLEVEL];
    PyObject *filename;
    PyObject *module;
    ಸಂರಚನಾ _PyTok_Reader *reader;

    ಪೂರ್ಣಾಙ್ಕ type_comments;      /* Whether to look for type comments */

    ftstring_state *ftstring_ರಾಶಿ;
    ftstring_state ftstring_ರಾಶಿ_inline[FTSTRING_ರಾಶಿ_INLINE_CAPACITY];
    ಪೂರ್ಣಾಙ್ಕ ftstring_depth;
    ಪೂರ್ಣಾಙ್ಕ ftstring_capacity;
    ಪೂರ್ಣಾಙ್ಕ tok_extra_tokens;
    ಪೂರ್ಣಾಙ್ಕ implicit_newline;
#ifdef Py_DEBUG
    ಪೂರ್ಣಾಙ್ಕ debug;
#endif
    ಪೂರ್ಣಾಙ್ಕ barry_as_bdfl;
    unsigned ವರ್ಣ allow_incomplete_ಒಳಹರಿವು;
    unsigned ವರ್ಣ needs_newline;
};

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ftstring_state *
_PyLexer_CurrentFTString(ಸಂರಚನಾ tok_state *tok)
{
    ಪ್ರತಿಜ್ಞಾ(tok->ftstring_ರಾಶಿ != ರಿಕ್ತ);
    ಪ್ರತಿಜ್ಞಾ(tok->ftstring_depth >= 0 && tok->ftstring_depth <= tok->ftstring_capacity);
    ಯದಿ (tok->ftstring_depth == 0) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ &tok->ftstring_ರಾಶಿ[tok->ftstring_depth - 1];
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ
_PyLexer_StringPrefix(ftstring_kind kind)
{
    ಪ್ರತ್ಯಾಗಮ _PyLexer_IsTString(kind) ? 't' : 'f';
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ
_PyLexer_FTStringBracketDepth(ಧ್ರುವ ಸಂರಚನಾ tok_state *tok,
                              ಧ್ರುವ ftstring_state *state)
{
    ಪ್ರತ್ಯಾಗಮ tok->level - state->paren_level;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ
_PyLexer_ByteColumn(ಧ್ರುವ ಸಂರಚನಾ tok_state *tok)
{
    ಪ್ರತಿಜ್ಞಾ(tok->ಸಾಲು_start >= 0);
    ಪ್ರತಿಜ್ಞಾ(tok->cur >= tok->ಸಾಲು_start);
    Py_ssize_t column = tok->cur - tok->ಸಾಲು_start;
    ಪ್ರತಿಜ್ಞಾ(column <= INT_MAX);
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)column;
}

ಪೂರ್ಣಾಙ್ಕ _PyLexer_token_setup(ಸಂರಚನಾ tok_state *tok, ಸಂರಚನಾ token *token, ಪೂರ್ಣಾಙ್ಕ type, _PyTok_Off start, _PyTok_Off end);

ಶೂನ್ಯ _PyLexer_ImplyDedents(ಸಂರಚನಾ tok_state *);

ಶೂನ್ಯ _PyTokenizer_Free(ಸಂರಚನಾ tok_state *);
ftstring_state *Py_Lexer_ತಳ್ಳು_FTString(ಸಂರಚನಾ tok_state *);
ಶೂನ್ಯ Py_Lexer_ತೆಗೆ_FTString(ಸಂರಚನಾ tok_state *);


#endif
