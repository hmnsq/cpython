#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _PY_LEXER_H_
#ವ್ಯಾಖ್ಯಾನಿಸು _PY_LEXER_H_

#ಅನ್ತರ್ಭಾವ "object.h"
#ಅನ್ತರ್ಭಾವ "../tokenizer/source.h"
#ಅನ್ತರ್ಭಾವ "../tokenizer/tokenizer.h"

#ವ್ಯಾಖ್ಯಾನಿಸು MAXINDENT 100       /* Max indentation level */
#ವ್ಯಾಖ್ಯಾನಿಸು MAXLEVEL 200        /* Max parentheses level */
#ವ್ಯಾಖ್ಯಾನಿಸು MAXFTSTRINGLEVEL 150
#ವ್ಯಾಖ್ಯಾನಿಸು FTSTRING_ರಾಶಿ_INLINE_ಸಾಮರ್ಥ್ಯ 1

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    FTSTRING_ಬಹುಲಕ_ಮಧ್ಯ,
    FTSTRING_ಬಹುಲಕ_ಅಭಿವ್ಯಕ್ತಿ,
    FTSTRING_ಬಹುಲಕ_FORMAT_SPEC,
} ftstring_ಬಹುಲಕ;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    FSTRING,
    RAW_FSTRING,
    TSTRING,
    RAW_TSTRING,
} ftstring_ಬಗೆ;

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ಅಭಿವ್ಯಕ್ತಿ_NESTING 3

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ _tokenizer_comments {
    Py_ssize_t ಎಣಿಕೆ_2;
    Py_ssize_t ಸಾಮರ್ಥ್ಯ;
    Py_Tok_ಹರವು spans[];
} tokenizer_comments;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ _ftstring_state {
    ftstring_ಬಹುಲಕ ಬಹುಲಕ;
    ftstring_ಬಗೆ ಬಗೆ;
    ಅಕ್ಷರ quote;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ quote_ಗಾತ್ರ;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ debug_ಅಭಿವ್ಯಕ್ತಿ;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ replacement_ಆಳ;
    ಪೂರ್ಣಾಂಕ paren_level;
    _PyTok_Off ಆರಂಭ;
    _PyTok_Loc ಆರಂಭ_loc;
    Py_Tok_ಹರವು ಅಭಿವ್ಯಕ್ತಿ_ಹರವು;
    tokenizer_comments *comments;
} ftstring_state;

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ
_PyLexer_IsTString(ftstring_ಬಗೆ ಬಗೆ)
{
    ಹಿಂತಿರುಗಿ ಬಗೆ == TSTRING || ಬಗೆ == RAW_TSTRING;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ
_PyLexer_IsRawString(ftstring_ಬಗೆ ಬಗೆ)
{
    ಹಿಂತಿರುಗಿ ಬಗೆ == RAW_FSTRING || ಬಗೆ == RAW_TSTRING;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ ಸ್ತಂಭ;
    ಪೂರ್ಣಾಂಕ alternate_ಸ್ತಂಭ;
} indentation_level;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ ಆಳ;
    ಪೂರ್ಣಾಂಕ pending;
    ಪೂರ್ಣಾಂಕ at_bol;
    ಪೂರ್ಣಾಂಕ comment_newline;
    indentation_level ರಾಶಿ[MAXINDENT];
} lexer_layout_state;

/* Tokenizer state */
ರಚನೆ tok_state {
    _PyTok_Off buf_ಸ್ಥಾನಾಂತರ;
    _PyTok_Off cur;
    _PyTok_Off inp;
    _PyTok_Off ಆರಂಭ;
    _PyTok_Off ಸಾಲು_ಆರಂಭ;
    Py_Tok_ಮೂಲ_ಪಠ್ಯ ಮೂಲ;
    ಪೂರ್ಣಾಂಕ ಮುಗಿದ;           /* E_OK normally, E_EOF at EOF, otherwise error code */
    /* NB If done != E_OK, cur must be == inp!!! */
    lexer_layout_state layout;
    ಪೂರ್ಣಾಂಕ lineno;         /* Current line number */
    _PyTok_Loc ಆರಂಭ_loc;
    _PyTokenizer_Diagnostic diagnostic;
    ಪೂರ್ಣಾಂಕ level;          /* () [] {} Parentheses nesting level */
            /* Used to allow free continuations inside them */
    ಅಕ್ಷರ parenstack[MAXLEVEL];
    ಪೂರ್ಣಾಂಕ parenlinenostack[MAXLEVEL];
    ಪೂರ್ಣಾಂಕ parencolstack[MAXLEVEL];
    PyObject *filename;
    PyObject *module;
    ರಚನೆ _PyTok_Reader *reader;

    ಪೂರ್ಣಾಂಕ type_comments;      /* Whether to look for type comments */

    ftstring_state *ftstring_ರಾಶಿ;
    ftstring_state ftstring_ರಾಶಿ_inline[FTSTRING_ರಾಶಿ_INLINE_ಸಾಮರ್ಥ್ಯ];
    ಪೂರ್ಣಾಂಕ ftstring_ಆಳ;
    ಪೂರ್ಣಾಂಕ ftstring_ಸಾಮರ್ಥ್ಯ;
    ಪೂರ್ಣಾಂಕ tok_extra_ಟೋಕನುಗಳು;
    ಪೂರ್ಣಾಂಕ implicit_newline;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ Py_DEBUG
    ಪೂರ್ಣಾಂಕ debug;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ barry_as_bdfl;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ allow_incomplete_ಒಳಹರಿವು;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ needs_newline;
};

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ftstring_state *
_PyLexer_CurrentFTString(ರಚನೆ tok_state *tok)
{
    ಪ್ರತಿಜ್ಞಾ(tok->ftstring_ರಾಶಿ != ಖಾಲಿ);
    ಪ್ರತಿಜ್ಞಾ(tok->ftstring_ಆಳ >= 0 && tok->ftstring_ಆಳ <= tok->ftstring_ಸಾಮರ್ಥ್ಯ);
    ಆದರೆ (tok->ftstring_ಆಳ == 0) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ &tok->ftstring_ರಾಶಿ[tok->ftstring_ಆಳ - 1];
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ
Py_Lexer_String_ಪೂರ್ವಪ್ರತ್ಯಯ(ftstring_ಬಗೆ ಬಗೆ)
{
    ಹಿಂತಿರುಗಿ _PyLexer_IsTString(ಬಗೆ) ? 't' : 'f';
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ
Py_Lexer_FTString_Bracket_ಆಳ(ಸ್ಥಿರಾಂಕ ರಚನೆ tok_state *tok,
                              ಸ್ಥಿರಾಂಕ ftstring_state *state)
{
    ಹಿಂತಿರುಗಿ tok->level - state->paren_level;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ
Py_Lexer_Byte_ಸ್ತಂಭ(ಸ್ಥಿರಾಂಕ ರಚನೆ tok_state *tok)
{
    ಪ್ರತಿಜ್ಞಾ(tok->ಸಾಲು_ಆರಂಭ >= 0);
    ಪ್ರತಿಜ್ಞಾ(tok->cur >= tok->ಸಾಲು_ಆರಂಭ);
    Py_ssize_t ಸ್ತಂಭ = tok->cur - tok->ಸಾಲು_ಆರಂಭ;
    ಪ್ರತಿಜ್ಞಾ(ಸ್ತಂಭ <= ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ);
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)ಸ್ತಂಭ;
}

ಪೂರ್ಣಾಂಕ Py_Lexer_token_ಸಿದ್ಧತೆ(ರಚನೆ tok_state *tok, ರಚನೆ token *token, ಪೂರ್ಣಾಂಕ type, _PyTok_Off ಆರಂಭ, _PyTok_Off ಕೊನೆ);

ಶೂನ್ಯ _PyLexer_ImplyDedents(ರಚನೆ tok_state *);

ಶೂನ್ಯ _PyTokenizer_Free(ರಚನೆ tok_state *);
ftstring_state *Py_Lexer_ತಳ್ಳು_FTString(ರಚನೆ tok_state *);
ಶೂನ್ಯ Py_Lexer_ತೆಗೆ_FTString(ರಚನೆ tok_state *);


#ಆದರೆ_ಅಂತ್ಯ
