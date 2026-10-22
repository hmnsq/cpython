#که_نه_تعریف_شوی PY_LEXER_دننی_H
#تعریف_کړه PY_LEXER_دننی_H

#ضمن "errcode.h"
#ضمن "state.h"

#تعریف_کړه is_potential_identifier_پیل(c) (\
              (c >= 'a' && c <= 'z')\
               || (c >= 'A' && c <= 'Z')\
               || c == '_'\
               || (c >= 128))

#تعریف_کړه is_potential_identifier_char(c) (\
              (c >= 'a' && c <= 'z')\
               || (c >= 'A' && c <= 'Z')\
               || (c >= '0' && c <= '9')\
               || c == '_'\
               || (c >= 128))

#تعریف_کړه FTSTRING_منځ(state) \
    (_PyLexer_IsTString((state)->ډول) ? TSTRING_منځ : FSTRING_منځ)
#تعریف_کړه FTSTRING_پای_ټکی(state) \
    (_PyLexer_IsTString((state)->ډول) ? TSTRING_پای_ټکی : FSTRING_پای_ټکی)
#تعریف_کړه tok_backup _PyLexer_backup

ساكن مضمن صحيح
tok_failed(ثابت بنية tok_state *tok)
{
    أرجع tok->بشپړ != E_OK && tok->بشپړ != E_EOF &&
           tok->بشپړ != E_INTERACT_STOP;
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
    إذا (tok->cur - tok->کرښه_پیل >= لوی_عدد) {
        tok->بشپړ = E_COLUMNOVERFLOW;
        أرجع EOF;
    }
    أرجع Py_Tok_سرچینه_Byte(&tok->سرچینه, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
صحيح Py_Lexer_Begin_کرښه(بنية tok_state *);
صحيح Py_Lexer_Continue_کرښه(بنية tok_state *);
صحيح _PyLexer_IndentationToken(بنية tok_state *, بنية token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
صحيح _PyLexer_Newline(بنية tok_state *, بنية token *, صحيح);
فراغ _PyLexer_backup(بنية tok_state *, صحيح);
صحيح Py_Lexer_ریکارډ_ftstring_comment(
    بنية tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
صحيح _PyLexer_ftstring_punctuation(
    بنية tok_state *, ftstring_state *, بنية token *, صحيح);
صحيح Py_Lexer_close_ftstring_بیان(
    بنية tok_state *, ftstring_state *, صحيح);
فراغ _PyLexer_mark_ftstring_debug(بنية tok_state *, ftstring_state *);
صحيح _PyLexer_check_string_prefixes(بنية tok_state *, صحيح, صحيح, صحيح, صحيح, صحيح);
صحيح Py_Lexer_لټون_شمېره(بنية tok_state *, بنية token *, صحيح, صحيح);
صحيح Py_Lexer_لټون_fstring_پیل(
    بنية tok_state *, بنية token *, صحيح, ftstring_ډول);
صحيح Py_Lexer_لټون_string(بنية tok_state *, بنية token *, صحيح);
صحيح Py_Lexer_get_نورمال(بنية tok_state *, ftstring_state *, بنية token *);
صحيح _PyLexer_get_ftstring(بنية tok_state *, ftstring_state *, بنية token *);

#د_که_پای
