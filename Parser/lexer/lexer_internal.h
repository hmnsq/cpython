#جيڪڏهن_وصف_نه_ٿيل PY_LEXER_اندروني_H
#وصف_ڪريو PY_LEXER_اندروني_H

#ضمن "errcode.h"
#ضمن "state.h"

#وصف_ڪريو is_potential_identifier_شروعات(c) (\
              (c >= 'a' && c <= 'z')\
               || (c >= 'A' && c <= 'Z')\
               || c == '_'\
               || (c >= 128))

#وصف_ڪريو is_potential_identifier_char(c) (\
              (c >= 'a' && c <= 'z')\
               || (c >= 'A' && c <= 'Z')\
               || (c >= '0' && c <= '9')\
               || c == '_'\
               || (c >= 128))

#وصف_ڪريو FTSTRING_وچ(state) \
    (_PyLexer_IsTString((state)->قسم) ? TSTRING_وچ : FSTRING_وچ)
#وصف_ڪريو FTSTRING_پڇاڙي(state) \
    (_PyLexer_IsTString((state)->قسم) ? TSTRING_پڇاڙي : FSTRING_پڇاڙي)
#وصف_ڪريو tok_backup _PyLexer_backup

ساكن مضمن صحيح
tok_failed(ثابت بنية tok_state *tok)
{
    أرجع tok->پورو != E_OK && tok->پورو != E_EOF &&
           tok->پورو != E_INTERACT_STOP;
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
    إذا (tok->cur - tok->سٽ_شروعات >= وڌ_کان_وڌ_عدد) {
        tok->پورو = E_COLUMNOVERFLOW;
        أرجع EOF;
    }
    أرجع Py_Tok_ذريعو_Byte(&tok->ذريعو, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
صحيح Py_Lexer_Begin_سٽ(بنية tok_state *);
صحيح Py_Lexer_Continue_سٽ(بنية tok_state *);
صحيح _PyLexer_IndentationToken(بنية tok_state *, بنية token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
صحيح _PyLexer_Newline(بنية tok_state *, بنية token *, صحيح);
فراغ _PyLexer_backup(بنية tok_state *, صحيح);
صحيح Py_Lexer_رڪارڊ_ftstring_comment(
    بنية tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
صحيح _PyLexer_ftstring_punctuation(
    بنية tok_state *, ftstring_state *, بنية token *, صحيح);
صحيح Py_Lexer_close_ftstring_اظهار(
    بنية tok_state *, ftstring_state *, صحيح);
فراغ _PyLexer_mark_ftstring_debug(بنية tok_state *, ftstring_state *);
صحيح _PyLexer_check_string_prefixes(بنية tok_state *, صحيح, صحيح, صحيح, صحيح, صحيح);
صحيح Py_Lexer_جاچ_عدد(بنية tok_state *, بنية token *, صحيح, صحيح);
صحيح Py_Lexer_جاچ_fstring_شروعات(
    بنية tok_state *, بنية token *, صحيح, ftstring_قسم);
صحيح Py_Lexer_جاچ_string(بنية tok_state *, بنية token *, صحيح);
صحيح Py_Lexer_get_عام(بنية tok_state *, ftstring_state *, بنية token *);
صحيح _PyLexer_get_ftstring(بنية tok_state *, ftstring_state *, بنية token *);

#جيڪڏهن_پورو
