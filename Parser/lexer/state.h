#که_نه_تعریف_شوی _PY_LEXER_H_
#تعریف_کړه _PY_LEXER_H_

#ضمن "object.h"
#ضمن "../tokenizer/source.h"
#ضمن "../tokenizer/tokenizer.h"

#تعریف_کړه MAXINDENT 100       /* Max indentation level */
#تعریف_کړه MAXLEVEL 200        /* Max parentheses level */
#تعریف_کړه MAXFTSTRINGLEVEL 150
#تعریف_کړه FTSTRING_ډېری_INLINE_ظرفیت 1

تعريفنوع تعداد {
    FTSTRING_ډېرپېښ_منځ,
    FTSTRING_ډېرپېښ_افاده,
    FTSTRING_ډېرپېښ_FORMAT_SPEC,
} ftstring_ډېرپېښ;

تعريفنوع تعداد {
    FSTRING,
    RAW_FSTRING,
    TSTRING,
    RAW_TSTRING,
} ftstring_ډول;

#تعریف_کړه MAX_بیان_NESTING 3

تعريفنوع بنية _tokenizer_comments {
    Py_ssize_t شمېر;
    Py_ssize_t ظرفیت;
    Py_Tok_پراختیا spans[];
} tokenizer_comments;

تعريفنوع بنية _ftstring_state {
    ftstring_ډېرپېښ ډېرپېښ;
    ftstring_ډول ډول;
    حرف quote;
    بینښه حرف quote_اندازه;
    بینښه حرف debug_بیان;
    بینښه حرف replacement_ژوروالی;
    صحيح paren_level;
    _PyTok_Off پیل;
    _PyTok_Loc پیل_loc;
    Py_Tok_پراختیا بیان_پراختیا;
    tokenizer_comments *comments;
} ftstring_state;

ساكن مضمن صحيح
_PyLexer_IsTString(ftstring_ډول ډول)
{
    أرجع ډول == TSTRING || ډول == RAW_TSTRING;
}

ساكن مضمن صحيح
_PyLexer_IsRawString(ftstring_ډول ډول)
{
    أرجع ډول == RAW_FSTRING || ډول == RAW_TSTRING;
}

تعريفنوع بنية {
    صحيح ستنه;
    صحيح alternate_ستنه;
} indentation_level;

تعريفنوع بنية {
    صحيح ژوروالی;
    صحيح pending;
    صحيح at_bol;
    صحيح comment_newline;
    indentation_level ډېری[MAXINDENT];
} lexer_layout_state;

/* Tokenizer state */
بنية tok_state {
    _PyTok_Off buf_دننه_واټن;
    _PyTok_Off cur;
    _PyTok_Off inp;
    _PyTok_Off پیل;
    _PyTok_Off کرښه_پیل;
    Py_Tok_سرچینه_متن سرچینه;
    صحيح بشپړ;           /* E_OK normally, E_EOF at EOF, otherwise error code */
    /* NB If done != E_OK, cur must be == inp!!! */
    lexer_layout_state layout;
    صحيح lineno;         /* Current line number */
    _PyTok_Loc پیل_loc;
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

    ftstring_state *ftstring_ډېری;
    ftstring_state ftstring_ډېری_inline[FTSTRING_ډېری_INLINE_ظرفیت];
    صحيح ftstring_ژوروالی;
    صحيح ftstring_ظرفیت;
    صحيح tok_extra_نښې;
    صحيح implicit_newline;
#که_تعریف_شوی Py_DEBUG
    صحيح debug;
#د_که_پای
    صحيح barry_as_bdfl;
    بینښه حرف allow_incomplete_ننوت;
    بینښه حرف needs_newline;
};

ساكن مضمن ftstring_state *
_PyLexer_CurrentFTString(بنية tok_state *tok)
{
    أكد(tok->ftstring_ډېری != عدم);
    أكد(tok->ftstring_ژوروالی >= 0 && tok->ftstring_ژوروالی <= tok->ftstring_ظرفیت);
    إذا (tok->ftstring_ژوروالی == 0) {
        أرجع عدم;
    }
    أرجع &tok->ftstring_ډېری[tok->ftstring_ژوروالی - 1];
}

ساكن مضمن حرف
Py_Lexer_String_مختاړی(ftstring_ډول ډول)
{
    أرجع _PyLexer_IsTString(ډول) ? 't' : 'f';
}

ساكن مضمن صحيح
Py_Lexer_FTString_Bracket_ژوروالی(ثابت بنية tok_state *tok,
                              ثابت ftstring_state *state)
{
    أرجع tok->level - state->paren_level;
}

ساكن مضمن صحيح
Py_Lexer_Byte_ستنه(ثابت بنية tok_state *tok)
{
    أكد(tok->کرښه_پیل >= 0);
    أكد(tok->cur >= tok->کرښه_پیل);
    Py_ssize_t ستنه = tok->cur - tok->کرښه_پیل;
    أكد(ستنه <= لوی_عدد);
    أرجع (صحيح)ستنه;
}

صحيح Py_Lexer_token_تیاری(بنية tok_state *tok, بنية token *token, صحيح type, _PyTok_Off پیل, _PyTok_Off پای_ټکی);

فراغ _PyLexer_ImplyDedents(بنية tok_state *);

فراغ _PyTokenizer_Free(بنية tok_state *);
ftstring_state *Py_Lexer_ټېل_FTString(بنية tok_state *);
فراغ Py_Lexer_راوباسه_FTString(بنية tok_state *);


#د_که_پای
