#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _PY_LEXER_H_
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _PY_LEXER_H_

#ਅਨ੍ਤਰ੍ਭਾਵ "object.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "../tokenizer/source.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "../tokenizer/tokenizer.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAXINDENT 100       /* Max indentation level */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAXLEVEL 200        /* Max parentheses level */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAXFTSTRINGLEVEL 150
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FTSTRING_ਢੇਰ_INLINE_ਸਮਰੱਥਾ 1

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    FTSTRING_ਬਹੁਲਕ_ਵਿਚਕਾਰ,
    FTSTRING_ਬਹੁਲਕ_ਵਿਅੰਜਕ,
    FTSTRING_ਬਹੁਲਕ_FORMAT_SPEC,
} ftstring_ਬਹੁਲਕ;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    FSTRING,
    RAW_FSTRING,
    TSTRING,
    RAW_TSTRING,
} ftstring_ਕਿਸਮ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ਵਿਅੰਜਕ_NESTING 3

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ _tokenizer_comments {
    Py_ssize_t ਗਿਣਤੀ_2;
    Py_ssize_t ਸਮਰੱਥਾ;
    Py_Tok_ਵਿਸਤਾਰ spans[];
} tokenizer_comments;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ _ftstring_state {
    ftstring_ਬਹੁਲਕ ਬਹੁਲਕ;
    ftstring_ਕਿਸਮ ਕਿਸਮ;
    ਅੱਖਰ quote;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ quote_ਮਾਪ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ debug_ਵਿਅੰਜਕ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ replacement_ਡੂੰਘਾਈ;
    ਪੂਰਨ paren_level;
    _PyTok_Off ਸ਼ੁਰੂ;
    _PyTok_Loc ਸ਼ੁਰੂ_loc;
    Py_Tok_ਵਿਸਤਾਰ ਵਿਅੰਜਕ_ਵਿਸਤਾਰ;
    tokenizer_comments *comments;
} ftstring_state;

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ
_PyLexer_IsTString(ftstring_ਕਿਸਮ ਕਿਸਮ)
{
    ਵਾਪਸ ਕਿਸਮ == TSTRING || ਕਿਸਮ == RAW_TSTRING;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ
_PyLexer_IsRawString(ftstring_ਕਿਸਮ ਕਿਸਮ)
{
    ਵਾਪਸ ਕਿਸਮ == RAW_FSTRING || ਕਿਸਮ == RAW_TSTRING;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ ਕਾਲਮ;
    ਪੂਰਨ alternate_ਕਾਲਮ;
} indentation_level;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ ਡੂੰਘਾਈ;
    ਪੂਰਨ pending;
    ਪੂਰਨ at_bol;
    ਪੂਰਨ comment_newline;
    indentation_level ਢੇਰ[MAXINDENT];
} lexer_layout_state;

/* Tokenizer state */
ਬਣਤਰ tok_state {
    _PyTok_Off buf_ਵਿੱਥ;
    _PyTok_Off cur;
    _PyTok_Off inp;
    _PyTok_Off ਸ਼ੁਰੂ;
    _PyTok_Off ਸਤਰ_ਸ਼ੁਰੂ;
    Py_Tok_ਸਰੋਤ_ਪਾਠ ਸਰੋਤ;
    ਪੂਰਨ ਪੂਰਾ;           /* E_OK normally, E_EOF at EOF, otherwise error code */
    /* NB If done != E_OK, cur must be == inp!!! */
    lexer_layout_state layout;
    ਪੂਰਨ lineno;         /* Current line number */
    _PyTok_Loc ਸ਼ੁਰੂ_loc;
    _PyTokenizer_Diagnostic diagnostic;
    ਪੂਰਨ level;          /* () [] {} Parentheses nesting level */
            /* Used to allow free continuations inside them */
    ਅੱਖਰ parenstack[MAXLEVEL];
    ਪੂਰਨ parenlinenostack[MAXLEVEL];
    ਪੂਰਨ parencolstack[MAXLEVEL];
    PyObject *filename;
    PyObject *module;
    ਬਣਤਰ _PyTok_Reader *reader;

    ਪੂਰਨ type_comments;      /* Whether to look for type comments */

    ftstring_state *ftstring_ਢੇਰ;
    ftstring_state ftstring_ਢੇਰ_inline[FTSTRING_ਢੇਰ_INLINE_ਸਮਰੱਥਾ];
    ਪੂਰਨ ftstring_ਡੂੰਘਾਈ;
    ਪੂਰਨ ftstring_ਸਮਰੱਥਾ;
    ਪੂਰਨ tok_extra_ਟੋਕਨ;
    ਪੂਰਨ implicit_newline;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ Py_DEBUG
    ਪੂਰਨ debug;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ barry_as_bdfl;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ allow_incomplete_ਨਿਵੇਸ਼;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ needs_newline;
};

ਸਥਿਰ ਸਤਰਅੰਦਰ ftstring_state *
_PyLexer_CurrentFTString(ਬਣਤਰ tok_state *tok)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(tok->ftstring_ਢੇਰ != ਨਿਰਾ);
    ਪ੍ਰਤਿਜ੍ਞਾ(tok->ftstring_ਡੂੰਘਾਈ >= 0 && tok->ftstring_ਡੂੰਘਾਈ <= tok->ftstring_ਸਮਰੱਥਾ);
    ਜੇ (tok->ftstring_ਡੂੰਘਾਈ == 0) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ &tok->ftstring_ਢੇਰ[tok->ftstring_ਡੂੰਘਾਈ - 1];
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ
Py_Lexer_String_ਅਗੇਤਰ(ftstring_ਕਿਸਮ ਕਿਸਮ)
{
    ਵਾਪਸ _PyLexer_IsTString(ਕਿਸਮ) ? 't' : 'f';
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ
Py_Lexer_FTString_Bracket_ਡੂੰਘਾਈ(ਅਚਲ ਬਣਤਰ tok_state *tok,
                              ਅਚਲ ftstring_state *state)
{
    ਵਾਪਸ tok->level - state->paren_level;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ
Py_Lexer_Byte_ਕਾਲਮ(ਅਚਲ ਬਣਤਰ tok_state *tok)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(tok->ਸਤਰ_ਸ਼ੁਰੂ >= 0);
    ਪ੍ਰਤਿਜ੍ਞਾ(tok->cur >= tok->ਸਤਰ_ਸ਼ੁਰੂ);
    Py_ssize_t ਕਾਲਮ = tok->cur - tok->ਸਤਰ_ਸ਼ੁਰੂ;
    ਪ੍ਰਤਿਜ੍ਞਾ(ਕਾਲਮ <= ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ);
    ਵਾਪਸ (ਪੂਰਨ)ਕਾਲਮ;
}

ਪੂਰਨ Py_Lexer_token_ਤਿਆਰੀ(ਬਣਤਰ tok_state *tok, ਬਣਤਰ token *token, ਪੂਰਨ type, _PyTok_Off ਸ਼ੁਰੂ, _PyTok_Off ਸਿਰਾ);

ਖਾਲੀ _PyLexer_ImplyDedents(ਬਣਤਰ tok_state *);

ਖਾਲੀ _PyTokenizer_Free(ਬਣਤਰ tok_state *);
ftstring_state *Py_Lexer_ਧੱਕੋ_FTString(ਬਣਤਰ tok_state *);
ਖਾਲੀ Py_Lexer_ਕੱਢੋ_FTString(ਬਣਤਰ tok_state *);


#ਜੇ_ਅੰਤ
