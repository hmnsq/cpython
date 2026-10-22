#اگر_تعریف_نشده PY_LEXER_داخلی_H
#تعریف_کن PY_LEXER_داخلی_H

#ضمن "errcode.h"
#ضمن "state.h"

#تعریف_کن is_potential_identifier_شروع(c) (\
              (c >= 'a' && c <= 'z')\
               || (c >= 'A' && c <= 'Z')\
               || c == '_'\
               || (c >= 128))

#تعریف_کن is_potential_identifier_char(c) (\
              (c >= 'a' && c <= 'z')\
               || (c >= 'A' && c <= 'Z')\
               || (c >= '0' && c <= '9')\
               || c == '_'\
               || (c >= 128))

#تعریف_کن FTSTRING_میانه(state) \
    (_PyLexer_IsTString((state)->نوع) ? TSTRING_میانه : FSTRING_میانه)
#تعریف_کن FTSTRING_انتها(state) \
    (_PyLexer_IsTString((state)->نوع) ? TSTRING_انتها : FSTRING_انتها)
#تعریف_کن tok_backup _PyLexer_backup

ساكن مضمن صحيح
tok_failed(ثابت بنية tok_state *tok)
{
    أرجع tok->انجامشده != E_OK && tok->انجامشده != E_EOF &&
           tok->انجامشده != E_INTERACT_STOP;
}

صحيح _PyLexer_refill(بنية tok_state *);

ساكن مضمن صحيح
tok_nextc(بنية tok_state *tok)
{
    طالما (tok->cur == tok->inp) {
        إذا (!_PyLexer_refill(tok)) {
            أرجع EOF;
        }
    }
    إذا (tok->cur - tok->سطر_شروع >= بیشترین_صحیح) {
        tok->انجامشده = E_COLUMNOVERFLOW;
        أرجع EOF;
    }
    أرجع Py_Tok_منبع_Byte(&tok->منبع, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
صحيح Py_Lexer_Begin_سطر(بنية tok_state *);
صحيح Py_Lexer_Continue_سطر(بنية tok_state *);
صحيح _PyLexer_IndentationToken(بنية tok_state *, بنية token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
صحيح _PyLexer_Newline(بنية tok_state *, بنية token *, صحيح);
فراغ _PyLexer_backup(بنية tok_state *, صحيح);
صحيح Py_Lexer_رکورد_ftstring_comment(
    بنية tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
صحيح _PyLexer_ftstring_punctuation(
    بنية tok_state *, ftstring_state *, بنية token *, صحيح);
صحيح Py_Lexer_close_ftstring_عبارت(
    بنية tok_state *, ftstring_state *, صحيح);
فراغ _PyLexer_mark_ftstring_debug(بنية tok_state *, ftstring_state *);
صحيح _PyLexer_check_string_prefixes(بنية tok_state *, صحيح, صحيح, صحيح, صحيح, صحيح);
صحيح Py_Lexer_پیمایش_عدد(بنية tok_state *, بنية token *, صحيح, صحيح);
صحيح Py_Lexer_پیمایش_fstring_شروع(
    بنية tok_state *, بنية token *, صحيح, ftstring_نوع);
صحيح Py_Lexer_پیمایش_string(بنية tok_state *, بنية token *, صحيح);
صحيح Py_Lexer_get_نرمال(بنية tok_state *, ftstring_state *, بنية token *);
صحيح _PyLexer_get_ftstring(بنية tok_state *, ftstring_state *, بنية token *);

#پایان_اگر
