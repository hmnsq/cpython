#جے_بنا_تعریف PY_LEXER_اندرونی_H
#تعریف_کرو PY_LEXER_اندرونی_H

#ضمن "errcode.h"
#ضمن "state.h"

#تعریف_کرو is_potential_identifier_شروع(c) (\
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
#تعریف_کرو FTSTRING_سرا(state) \
    (_PyLexer_IsTString((state)->قسم) ? TSTRING_سرا : FSTRING_سرا)
#تعریف_کرو tok_backup _PyLexer_backup

ساكن مضمن صحيح
tok_failed(ثابت بنية tok_state *tok)
{
    أرجع tok->پورا != E_OK && tok->پورا != E_EOF &&
           tok->پورا != E_INTERACT_STOP;
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
    إذا (tok->cur - tok->سطر_شروع >= ودھ_توں_ودھ_عدد) {
        tok->پورا = E_COLUMNOVERFLOW;
        أرجع EOF;
    }
    أرجع Py_Tok_سروت_Byte(&tok->سروت, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
صحيح Py_Lexer_Begin_سطر(بنية tok_state *);
صحيح Py_Lexer_Continue_سطر(بنية tok_state *);
صحيح _PyLexer_IndentationToken(بنية tok_state *, بنية token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
صحيح _PyLexer_Newline(بنية tok_state *, بنية token *, صحيح);
فراغ _PyLexer_backup(بنية tok_state *, صحيح);
صحيح Py_Lexer_ریکارڈ_ftstring_comment(
    بنية tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
صحيح _PyLexer_ftstring_punctuation(
    بنية tok_state *, ftstring_state *, بنية token *, صحيح);
صحيح Py_Lexer_close_ftstring_اظہاریہ(
    بنية tok_state *, ftstring_state *, صحيح);
فراغ _PyLexer_mark_ftstring_debug(بنية tok_state *, ftstring_state *);
صحيح _PyLexer_check_string_prefixes(بنية tok_state *, صحيح, صحيح, صحيح, صحيح, صحيح);
صحيح Py_Lexer_جانچ_نمبر(بنية tok_state *, بنية token *, صحيح, صحيح);
صحيح Py_Lexer_جانچ_fstring_شروع(
    بنية tok_state *, بنية token *, صحيح, ftstring_قسم);
صحيح Py_Lexer_جانچ_string(بنية tok_state *, بنية token *, صحيح);
صحيح Py_Lexer_get_عام(بنية tok_state *, ftstring_state *, بنية token *);
صحيح _PyLexer_get_ftstring(بنية tok_state *, ftstring_state *, بنية token *);

#جے_انت
