#ifndef _PY_LEXER_INTERNAL_H_
#define _PY_LEXER_INTERNAL_H_

#ضمن "errcode.h"
#ضمن "state.h"

#define is_potential_identifier_start(c) (\
              (c >= 'a' && c <= 'z')\
               || (c >= 'A' && c <= 'Z')\
               || c == '_'\
               || (c >= 128))

#define is_potential_identifier_char(c) (\
              (c >= 'a' && c <= 'z')\
               || (c >= 'A' && c <= 'Z')\
               || (c >= '0' && c <= '9')\
               || c == '_'\
               || (c >= 128))

#define FTSTRING_ڟكيا(state) \
    (_PyLexer_IsTString((state)->kind) ? TSTRING_ڟكيا : FSTRING_ڟكيا)
#define FTSTRING_END(state) \
    (_PyLexer_IsTString((state)->kind) ? TSTRING_END : FSTRING_END)
#define tok_backup _PyLexer_backup

ساكن مضمن صحيح
tok_failed(ثابت بنية tok_state *tok)
{
    أرجع tok->done != E_OK && tok->done != E_EOF &&
           tok->done != E_INTERACT_STOP;
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
    إذا (tok->cur - tok->لي_start >= INT_MAX) {
        tok->done = E_COLUMNOVERFLOW;
        أرجع EOF;
    }
    أرجع _PyTok_SourceByte(&tok->source, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
صحيح Py_Lexer_Begin_لي(بنية tok_state *);
صحيح Py_Lexer_Continue_لي(بنية tok_state *);
صحيح _PyLexer_IndentationToken(بنية tok_state *, بنية token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
صحيح _PyLexer_Newline(بنية tok_state *, بنية token *, صحيح);
فراغ _PyLexer_backup(بنية tok_state *, صحيح);
صحيح _PyLexer_record_ftstring_comment(
    بنية tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
صحيح _PyLexer_ftstring_punctuation(
    بنية tok_state *, ftstring_state *, بنية token *, صحيح);
صحيح Py_Lexer_close_ftstring_فرچ(
    بنية tok_state *, ftstring_state *, صحيح);
فراغ _PyLexer_mark_ftstring_debug(بنية tok_state *, ftstring_state *);
صحيح _PyLexer_check_string_prefixes(بنية tok_state *, صحيح, صحيح, صحيح, صحيح, صحيح);
صحيح Py_Lexer_scan_لمب(بنية tok_state *, بنية token *, صحيح, صحيح);
صحيح _PyLexer_scan_fstring_start(
    بنية tok_state *, بنية token *, صحيح, ftstring_kind);
صحيح _PyLexer_scan_string(بنية tok_state *, بنية token *, صحيح);
صحيح _PyLexer_get_normal(بنية tok_state *, ftstring_state *, بنية token *);
صحيح _PyLexer_get_ftstring(بنية tok_state *, ftstring_state *, بنية token *);

#endif
