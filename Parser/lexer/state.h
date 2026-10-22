#جيڪڏهن_وصف_نه_ٿيل _PY_LEXER_H_
#وصف_ڪريو _PY_LEXER_H_

#ضمن "object.h"
#ضمن "../tokenizer/source.h"
#ضمن "../tokenizer/tokenizer.h"

#وصف_ڪريو MAXINDENT 100       /* Max indentation level */
#وصف_ڪريو MAXLEVEL 200        /* Max parentheses level */
#وصف_ڪريو MAXFTSTRINGLEVEL 150
#وصف_ڪريو FTSTRING_ڍير_INLINE_صلاحيت 1

تعريفنوع تعداد {
    FTSTRING_گهڻائي_وچ,
    FTSTRING_گهڻائي_اظهار,
    FTSTRING_گهڻائي_FORMAT_SPEC,
} ftstring_گهڻائي;

تعريفنوع تعداد {
    FSTRING,
    RAW_FSTRING,
    TSTRING,
    RAW_TSTRING,
} ftstring_قسم;

#وصف_ڪريو MAX_اظهار_NESTING 3

تعريفنوع بنية _tokenizer_comments {
    Py_ssize_t ڳڻپ;
    Py_ssize_t صلاحيت;
    Py_Tok_وسعت spans[];
} tokenizer_comments;

تعريفنوع بنية _ftstring_state {
    ftstring_گهڻائي گهڻائي;
    ftstring_قسم قسم;
    حرف quote;
    بنانشان حرف quote_سائيز;
    بنانشان حرف debug_اظهار;
    بنانشان حرف replacement_اونهائي;
    صحيح paren_level;
    _PyTok_Off شروعات;
    _PyTok_Loc شروعات_loc;
    Py_Tok_وسعت اظهار_وسعت;
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
    صحيح ڪالم;
    صحيح alternate_ڪالم;
} indentation_level;

تعريفنوع بنية {
    صحيح اونهائي;
    صحيح pending;
    صحيح at_bol;
    صحيح comment_newline;
    indentation_level ڍير[MAXINDENT];
} lexer_layout_state;

/* Tokenizer state */
بنية tok_state {
    _PyTok_Off buf_وٿي;
    _PyTok_Off cur;
    _PyTok_Off inp;
    _PyTok_Off شروعات;
    _PyTok_Off سٽ_شروعات;
    Py_Tok_ذريعو_متن ذريعو;
    صحيح پورو;           /* E_OK normally, E_EOF at EOF, otherwise error code */
    /* NB If done != E_OK, cur must be == inp!!! */
    lexer_layout_state layout;
    صحيح lineno;         /* Current line number */
    _PyTok_Loc شروعات_loc;
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

    ftstring_state *ftstring_ڍير;
    ftstring_state ftstring_ڍير_inline[FTSTRING_ڍير_INLINE_صلاحيت];
    صحيح ftstring_اونهائي;
    صحيح ftstring_صلاحيت;
    صحيح tok_extra_ٽوڪن;
    صحيح implicit_newline;
#جيڪڏهن_وصف_ٿيل Py_DEBUG
    صحيح debug;
#جيڪڏهن_پورو
    صحيح barry_as_bdfl;
    بنانشان حرف allow_incomplete_داخلا;
    بنانشان حرف needs_newline;
};

ساكن مضمن ftstring_state *
_PyLexer_CurrentFTString(بنية tok_state *tok)
{
    أكد(tok->ftstring_ڍير != عدم);
    أكد(tok->ftstring_اونهائي >= 0 && tok->ftstring_اونهائي <= tok->ftstring_صلاحيت);
    إذا (tok->ftstring_اونهائي == 0) {
        أرجع عدم;
    }
    أرجع &tok->ftstring_ڍير[tok->ftstring_اونهائي - 1];
}

ساكن مضمن حرف
Py_Lexer_String_اڳياڙي(ftstring_قسم قسم)
{
    أرجع _PyLexer_IsTString(قسم) ? 't' : 'f';
}

ساكن مضمن صحيح
Py_Lexer_FTString_Bracket_اونهائي(ثابت بنية tok_state *tok,
                              ثابت ftstring_state *state)
{
    أرجع tok->level - state->paren_level;
}

ساكن مضمن صحيح
Py_Lexer_Byte_ڪالم(ثابت بنية tok_state *tok)
{
    أكد(tok->سٽ_شروعات >= 0);
    أكد(tok->cur >= tok->سٽ_شروعات);
    Py_ssize_t ڪالم = tok->cur - tok->سٽ_شروعات;
    أكد(ڪالم <= وڌ_کان_وڌ_عدد);
    أرجع (صحيح)ڪالم;
}

صحيح Py_Lexer_token_تياري(بنية tok_state *tok, بنية token *token, صحيح type, _PyTok_Off شروعات, _PyTok_Off پڇاڙي);

فراغ _PyLexer_ImplyDedents(بنية tok_state *);

فراغ _PyTokenizer_Free(بنية tok_state *);
ftstring_state *Py_Lexer_ڌڪو_FTString(بنية tok_state *);
فراغ Py_Lexer_ڪڍو_FTString(بنية tok_state *);


#جيڪڏهن_پورو
