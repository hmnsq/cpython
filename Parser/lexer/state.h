#اگر_غیر_تعریف_شدہ _PY_LEXER_H_
#تعریف_کرو _PY_LEXER_H_

#شامل "object.h"
#شامل "../tokenizer/source.h"
#شامل "../tokenizer/tokenizer.h"

#تعریف_کرو MAXINDENT 100       /* Max indentation level */
#تعریف_کرو MAXLEVEL 200        /* Max parentheses level */
#تعریف_کرو MAXFTSTRINGLEVEL 150
#تعریف_کرو FTSTRING_ڈھیر_INLINE_استعداد 1

قسمتعریف شمار {
    FTSTRING_منوال_درمیان,
    FTSTRING_منوال_عبارت,
    FTSTRING_منوال_FORMAT_SPEC,
} ftstring_منوال;

قسمتعریف شمار {
    FSTRING,
    RAW_FSTRING,
    TSTRING,
    RAW_TSTRING,
} ftstring_قسم;

#تعریف_کرو MAX_اظہاریہ_NESTING 3

قسمتعریف ساخت _tokenizer_comments {
    Py_ssize_t گنتی;
    Py_ssize_t استعداد;
    Py_Tok_وسعت spans[];
} tokenizer_comments;

قسمتعریف ساخت _ftstring_state {
    ftstring_منوال منوال;
    ftstring_قسم قسم;
    حرف quote;
    غیرعلامتی حرف quote_سائز;
    غیرعلامتی حرف debug_اظہاریہ;
    غیرعلامتی حرف replacement_گہرائی;
    صحیح paren_level;
    _PyTok_Off آغاز;
    _PyTok_Loc آغاز_loc;
    Py_Tok_وسعت اظہاریہ_وسعت;
    tokenizer_comments *comments;
} ftstring_state;

ساکن اندرونی صحیح
_PyLexer_IsTString(ftstring_قسم قسم)
{
    واپس قسم == TSTRING || قسم == RAW_TSTRING;
}

ساکن اندرونی صحیح
_PyLexer_IsRawString(ftstring_قسم قسم)
{
    واپس قسم == RAW_FSTRING || قسم == RAW_TSTRING;
}

قسمتعریف ساخت {
    صحیح عمود;
    صحیح alternate_عمود;
} indentation_level;

قسمتعریف ساخت {
    صحیح گہرائی;
    صحیح pending;
    صحیح at_bol;
    صحیح comment_newline;
    indentation_level ڈھیر[MAXINDENT];
} lexer_layout_state;

/* Tokenizer state */
ساخت tok_state {
    _PyTok_Off buf_انحراف;
    _PyTok_Off cur;
    _PyTok_Off inp;
    _PyTok_Off آغاز;
    _PyTok_Off سطر_آغاز;
    Py_Tok_ماخذ_متن ماخذ;
    صحیح مکمل;           /* E_OK normally, E_EOF at EOF, otherwise error code */
    /* NB If done != E_OK, cur must be == inp!!! */
    lexer_layout_state layout;
    صحیح lineno;         /* Current line number */
    _PyTok_Loc آغاز_loc;
    _PyTokenizer_Diagnostic diagnostic;
    صحیح level;          /* () [] {} Parentheses nesting level */
            /* Used to allow free continuations inside them */
    حرف parenstack[MAXLEVEL];
    صحیح parenlinenostack[MAXLEVEL];
    صحیح parencolstack[MAXLEVEL];
    PyObject *filename;
    PyObject *module;
    ساخت _PyTok_Reader *reader;

    صحیح type_comments;      /* Whether to look for type comments */

    ftstring_state *ftstring_ڈھیر;
    ftstring_state ftstring_ڈھیر_inline[FTSTRING_ڈھیر_INLINE_استعداد];
    صحیح ftstring_گہرائی;
    صحیح ftstring_استعداد;
    صحیح tok_extra_علامات;
    صحیح implicit_newline;
#اگر_تعریف_شدہ Py_DEBUG
    صحیح debug;
#اگر_ختم
    صحیح barry_as_bdfl;
    غیرعلامتی حرف allow_incomplete_داخلہ;
    غیرعلامتی حرف needs_newline;
};

ساکن اندرونی ftstring_state *
_PyLexer_CurrentFTString(ساخت tok_state *tok)
{
    أكد(tok->ftstring_ڈھیر != تہی);
    أكد(tok->ftstring_گہرائی >= 0 && tok->ftstring_گہرائی <= tok->ftstring_استعداد);
    اگر (tok->ftstring_گہرائی == 0) {
        واپس تہی;
    }
    واپس &tok->ftstring_ڈھیر[tok->ftstring_گہرائی - 1];
}

ساکن اندرونی حرف
Py_Lexer_String_سابقہ(ftstring_قسم قسم)
{
    واپس _PyLexer_IsTString(قسم) ? 't' : 'f';
}

ساکن اندرونی صحیح
Py_Lexer_FTString_Bracket_گہرائی(ثابت ساخت tok_state *tok,
                              ثابت ftstring_state *state)
{
    واپس tok->level - state->paren_level;
}

ساکن اندرونی صحیح
Py_Lexer_Byte_عمود(ثابت ساخت tok_state *tok)
{
    أكد(tok->سطر_آغاز >= 0);
    أكد(tok->cur >= tok->سطر_آغاز);
    Py_ssize_t عمود = tok->cur - tok->سطر_آغاز;
    أكد(عمود <= زیادہ_سے_زیادہ_عدد);
    واپس (صحیح)عمود;
}

صحیح Py_Lexer_token_تیاری(ساخت tok_state *tok, ساخت token *token, صحیح type, _PyTok_Off آغاز, _PyTok_Off انتہا);

خالی _PyLexer_ImplyDedents(ساخت tok_state *);

خالی _PyTokenizer_Free(ساخت tok_state *);
ftstring_state *Py_Lexer_ڈالو_FTString(ساخت tok_state *);
خالی Py_Lexer_نکالو_FTString(ساخت tok_state *);


#اگر_ختم
