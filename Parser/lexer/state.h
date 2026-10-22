#جے_بنا_تعریف _PY_LEXER_H_
#تعریف_کرو _PY_LEXER_H_

#ضمن "object.h"
#ضمن "../tokenizer/source.h"
#ضمن "../tokenizer/tokenizer.h"

#تعریف_کرو MAXINDENT 100       /* Max indentation level */
#تعریف_کرو MAXLEVEL 200        /* Max parentheses level */
#تعریف_کرو MAXFTSTRINGLEVEL 150
#تعریف_کرو FTSTRING_ڈھیر_INLINE_سمرتھا 1

تعريفنوع تعداد {
    FTSTRING_منوال_درمیان,
    FTSTRING_منوال_عبارت,
    FTSTRING_منوال_FORMAT_SPEC,
} ftstring_منوال;

تعريفنوع تعداد {
    FSTRING,
    RAW_FSTRING,
    TSTRING,
    RAW_TSTRING,
} ftstring_قسم;

#تعریف_کرو MAX_اظہاریہ_NESTING 3

تعريفنوع بنية _tokenizer_comments {
    Py_ssize_t گنتی;
    Py_ssize_t سمرتھا;
    Py_Tok_وسعت spans[];
} tokenizer_comments;

تعريفنوع بنية _ftstring_state {
    ftstring_منوال منوال;
    ftstring_قسم قسم;
    حرف quote;
    بنانشانی حرف quote_ناپ;
    بنانشانی حرف debug_اظہاریہ;
    بنانشانی حرف replacement_ڈونگھائی;
    صحيح paren_level;
    _PyTok_Off شروع;
    _PyTok_Loc شروع_loc;
    Py_Tok_وسعت اظہاریہ_وسعت;
    tokenizer_comments *comments;
} ftstring_state;

ساكن مضمن صحيح
_PyLexer_IsTString(ftstring_قسم قسم)
{
    أرجع قسم == TSTRING || قسم == RAW_TSTRING;
}

ساكن مضمن صحيح
_PyLexer_IsRawString(ftstring_قسم قسم)
{
    أرجع قسم == RAW_FSTRING || قسم == RAW_TSTRING;
}

تعريفنوع بنية {
    صحيح کالم;
    صحيح alternate_کالم;
} indentation_level;

تعريفنوع بنية {
    صحيح ڈونگھائی;
    صحيح pending;
    صحيح at_bol;
    صحيح comment_newline;
    indentation_level ڈھیر[MAXINDENT];
} lexer_layout_state;

/* Tokenizer state */
بنية tok_state {
    _PyTok_Off buf_وتھ;
    _PyTok_Off cur;
    _PyTok_Off inp;
    _PyTok_Off شروع;
    _PyTok_Off سطر_شروع;
    Py_Tok_سروت_متن سروت;
    صحيح پورا;           /* E_OK normally, E_EOF at EOF, otherwise error code */
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

    ftstring_state *ftstring_ڈھیر;
    ftstring_state ftstring_ڈھیر_inline[FTSTRING_ڈھیر_INLINE_سمرتھا];
    صحيح ftstring_ڈونگھائی;
    صحيح ftstring_سمرتھا;
    صحيح tok_extra_ٹوکن;
    صحيح implicit_newline;
#جے_تعریف_شدہ Py_DEBUG
    صحيح debug;
#جے_انت
    صحيح barry_as_bdfl;
    بنانشانی حرف allow_incomplete_داخلہ;
    بنانشانی حرف needs_newline;
};

ساكن مضمن ftstring_state *
_PyLexer_CurrentFTString(بنية tok_state *tok)
{
    أكد(tok->ftstring_ڈھیر != عدم);
    أكد(tok->ftstring_ڈونگھائی >= 0 && tok->ftstring_ڈونگھائی <= tok->ftstring_سمرتھا);
    إذا (tok->ftstring_ڈونگھائی == 0) {
        أرجع عدم;
    }
    أرجع &tok->ftstring_ڈھیر[tok->ftstring_ڈونگھائی - 1];
}

ساكن مضمن حرف
Py_Lexer_String_اگیتر(ftstring_قسم قسم)
{
    أرجع _PyLexer_IsTString(قسم) ? 't' : 'f';
}

ساكن مضمن صحيح
Py_Lexer_FTString_Bracket_ڈونگھائی(ثابت بنية tok_state *tok,
                              ثابت ftstring_state *state)
{
    أرجع tok->level - state->paren_level;
}

ساكن مضمن صحيح
Py_Lexer_Byte_کالم(ثابت بنية tok_state *tok)
{
    أكد(tok->سطر_شروع >= 0);
    أكد(tok->cur >= tok->سطر_شروع);
    Py_ssize_t کالم = tok->cur - tok->سطر_شروع;
    أكد(کالم <= ودھ_توں_ودھ_عدد);
    أرجع (صحيح)کالم;
}

صحيح Py_Lexer_token_تیاری(بنية tok_state *tok, بنية token *token, صحيح type, _PyTok_Off شروع, _PyTok_Off سرا);

فراغ _PyLexer_ImplyDedents(بنية tok_state *);

فراغ _PyTokenizer_Free(بنية tok_state *);
ftstring_state *Py_Lexer_ڈالو_FTString(بنية tok_state *);
فراغ Py_Lexer_نکالو_FTString(بنية tok_state *);


#جے_انت
