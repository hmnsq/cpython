#اگر_غیر_تعریف_شدہ PY_LEXER_اندرونی_H
#تعریف_کرو PY_LEXER_اندرونی_H

#شامل "errcode.h"
#شامل "state.h"

#تعریف_کرو is_potential_identifier_آغاز(c) (\
              (c >= 'a' && c <= 'z')\
               || (c >= 'A' && c <= 'Z')\
               || c == '_'\
               || (c >= 128))

#تعریف_کرو is_potential_identifier_char(c) (\
              (c >= 'a' && c <= 'z')\
               || (c >= 'A' && c <= 'Z')\
               || (c >= '0' && c <= '9')\
               || c == '_'\
               || (c >= 128))

#تعریف_کرو FTSTRING_درمیان(state) \
    (_PyLexer_IsTString((state)->قسم) ? TSTRING_درمیان : FSTRING_درمیان)
#تعریف_کرو FTSTRING_انتہا(state) \
    (_PyLexer_IsTString((state)->قسم) ? TSTRING_انتہا : FSTRING_انتہا)
#تعریف_کرو tok_backup _PyLexer_backup

ساکن اندرونی صحیح
tok_failed(ثابت ساخت tok_state *tok)
{
    واپس tok->مکمل != E_OK && tok->مکمل != E_EOF &&
           tok->مکمل != E_INTERACT_STOP;
}

صحیح _PyLexer_refill(ساخت tok_state *);

ساکن اندرونی صحیح
tok_nextc(ساخت tok_state *tok)
{
    جب (tok->cur == tok->inp) {
        اگر (!_PyLexer_refill(tok)) {
            واپس EOF;
        }
    }
    اگر (tok->cur - tok->سطر_آغاز >= زیادہ_سے_زیادہ_عدد) {
        tok->مکمل = E_COLUMNOVERFLOW;
        واپس EOF;
    }
    واپس Py_Tok_ماخذ_Byte(&tok->ماخذ, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
صحیح Py_Lexer_Begin_سطر(ساخت tok_state *);
صحیح Py_Lexer_Continue_سطر(ساخت tok_state *);
صحیح _PyLexer_IndentationToken(ساخت tok_state *, ساخت token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
صحیح _PyLexer_Newline(ساخت tok_state *, ساخت token *, صحیح);
خالی _PyLexer_backup(ساخت tok_state *, صحیح);
صحیح Py_Lexer_ریکارڈ_ftstring_comment(
    ساخت tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
صحیح _PyLexer_ftstring_punctuation(
    ساخت tok_state *, ftstring_state *, ساخت token *, صحیح);
صحیح Py_Lexer_close_ftstring_اظہاریہ(
    ساخت tok_state *, ftstring_state *, صحیح);
خالی _PyLexer_mark_ftstring_debug(ساخت tok_state *, ftstring_state *);
صحیح _PyLexer_check_string_prefixes(ساخت tok_state *, صحیح, صحیح, صحیح, صحیح, صحیح);
صحیح Py_Lexer_جائزہ_نمبر(ساخت tok_state *, ساخت token *, صحیح, صحیح);
صحیح Py_Lexer_جائزہ_fstring_آغاز(
    ساخت tok_state *, ساخت token *, صحیح, ftstring_قسم);
صحیح Py_Lexer_جائزہ_string(ساخت tok_state *, ساخت token *, صحیح);
صحیح Py_Lexer_get_معمول(ساخت tok_state *, ftstring_state *, ساخت token *);
صحیح _PyLexer_get_ftstring(ساخت tok_state *, ftstring_state *, ساخت token *);

#اگر_ختم
