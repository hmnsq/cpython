#اگر_تعریف_نشده _PY_LEXER_H_
#تعریف_کن _PY_LEXER_H_

#ضمن "object.h"
#ضمن "../tokenizer/source.h"
#ضمن "../tokenizer/tokenizer.h"

#تعریف_کن MAXINDENT 100       /* Max indentation level */
#تعریف_کن MAXLEVEL 200        /* Max parentheses level */
#تعریف_کن MAXFTSTRINGLEVEL 150
#تعریف_کن FTSTRING_پشته_INLINE_ظرفیت 1

تعريفنوع تعداد {
    FTSTRING_نما_میانه,
    FTSTRING_نما_عبارت,
    FTSTRING_نما_FORMAT_SPEC,
} ftstring_نما;

تعريفنوع تعداد {
    FSTRING,
    RAW_FSTRING,
    TSTRING,
    RAW_TSTRING,
} ftstring_نوع;

#تعریف_کن MAX_عبارت_NESTING 3

تعريفنوع بنية _tokenizer_comments {
    Py_ssize_t شمار;
    Py_ssize_t ظرفیت;
    Py_Tok_بازه spans[];
} tokenizer_comments;

تعريفنوع بنية _ftstring_state {
    ftstring_نما نما;
    ftstring_نوع نوع;
    حرف quote;
    بیعلامت حرف quote_اندازه;
    بیعلامت حرف debug_عبارت;
    بیعلامت حرف replacement_عمق;
    صحيح paren_level;
    _PyTok_Off شروع;
    _PyTok_Loc شروع_loc;
    Py_Tok_بازه عبارت_بازه;
    tokenizer_comments *comments;
} ftstring_state;

ساكن مضمن صحيح
_PyLexer_IsTString(ftstring_نوع نوع)
{
    أرجع نوع == TSTRING || نوع == RAW_TSTRING;
}

ساكن مضمن صحيح
_PyLexer_IsRawString(ftstring_نوع نوع)
{
    أرجع نوع == RAW_FSTRING || نوع == RAW_TSTRING;
}

تعريفنوع بنية {
    صحيح ستونی;
    صحيح alternate_ستونی;
} indentation_level;

تعريفنوع بنية {
    صحيح عمق;
    صحيح pending;
    صحيح at_bol;
    صحيح comment_newline;
    indentation_level پشته[MAXINDENT];
} lexer_layout_state;

/* Tokenizer state */
بنية tok_state {
    _PyTok_Off buf_جابه_جایی;
    _PyTok_Off cur;
    _PyTok_Off inp;
    _PyTok_Off شروع;
    _PyTok_Off سطر_شروع;
    Py_Tok_منبع_متن منبع;
    صحيح انجامشده;           /* E_OK normally, E_EOF at EOF, otherwise error code */
    /* NB If done != E_OK, cur must be == inp!!! */
    lexer_layout_state layout;
    صحيح lineno;         /* Current line number */
    _PyTok_Loc شروع_loc;
    _PyTokenizer_Diagnostic diagnostic;
    صحيح level;          /* () [] {} Parentheses nesting level */
            /* Used to allow free continuations inside them */
    حرف parenstack[MAXLEVEL];
    صحيح parenlinenostack[MAXLEVEL];
    صحيح parencolstack[MAXLEVEL];
    PyObject *filename;
    PyObject *module;
    بنية _PyTok_Reader *reader;

    صحيح type_comments;      /* Whether to look for type comments */

    ftstring_state *ftstring_پشته;
    ftstring_state ftstring_پشته_inline[FTSTRING_پشته_INLINE_ظرفیت];
    صحيح ftstring_عمق;
    صحيح ftstring_ظرفیت;
    صحيح tok_extra_نشانهها;
    صحيح implicit_newline;
#اگر_تعریف_شده Py_DEBUG
    صحيح debug;
#پایان_اگر
    صحيح barry_as_bdfl;
    بیعلامت حرف allow_incomplete_درون_داد;
    بیعلامت حرف needs_newline;
};

ساكن مضمن ftstring_state *
_PyLexer_CurrentFTString(بنية tok_state *tok)
{
    أكد(tok->ftstring_پشته != عدم);
    أكد(tok->ftstring_عمق >= 0 && tok->ftstring_عمق <= tok->ftstring_ظرفیت);
    إذا (tok->ftstring_عمق == 0) {
        أرجع عدم;
    }
    أرجع &tok->ftstring_پشته[tok->ftstring_عمق - 1];
}

ساكن مضمن حرف
Py_Lexer_String_پیشوند(ftstring_نوع نوع)
{
    أرجع _PyLexer_IsTString(نوع) ? 't' : 'f';
}

ساكن مضمن صحيح
Py_Lexer_FTString_Bracket_عمق(ثابت بنية tok_state *tok,
                              ثابت ftstring_state *state)
{
    أرجع tok->level - state->paren_level;
}

ساكن مضمن صحيح
Py_Lexer_Byte_ستونی(ثابت بنية tok_state *tok)
{
    أكد(tok->سطر_شروع >= 0);
    أكد(tok->cur >= tok->سطر_شروع);
    Py_ssize_t ستونی = tok->cur - tok->سطر_شروع;
    أكد(ستونی <= بیشترین_صحیح);
    أرجع (صحيح)ستونی;
}

صحيح Py_Lexer_token_راهاندازی(بنية tok_state *tok, بنية token *token, صحيح type, _PyTok_Off شروع, _PyTok_Off انتها);

فراغ _PyLexer_ImplyDedents(بنية tok_state *);

فراغ _PyTokenizer_Free(بنية tok_state *);
ftstring_state *Py_Lexer_افزودن_FTString(بنية tok_state *);
فراغ Py_Lexer_برداشت_FTString(بنية tok_state *);


#پایان_اگر
