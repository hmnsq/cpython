#ifndef _PY_LEXER_INTERNAL_H_
#define _PY_LEXER_INTERNAL_H_

#شامل "errcode.h"
#شامل "state.h"

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

#define FTSTRING_میانه(state) \
    (_PyLexer_IsTString((state)->kind) ? TSTRING_میانه : FSTRING_میانه)
#define FTSTRING_END(state) \
    (_PyLexer_IsTString((state)->kind) ? TSTRING_END : FSTRING_END)
#define tok_backup _PyLexer_backup

ایستا درونخطی صحیح
tok_failed(ثابت ساختار tok_state *tok)
{
    بازگشت tok->done != E_OK && tok->done != E_EOF &&
           tok->done != E_INTERACT_STOP;
}

صحیح _PyLexer_refill(ساختار tok_state *);

ایستا درونخطی صحیح
tok_nextc(ساختار tok_state *tok)
{
    وقتی (tok->cur == tok->inp) {
        اگر (!_PyLexer_refill(tok)) {
            بازگشت EOF;
        }
    }
    اگر (tok->cur - tok->سطر_start >= INT_MAX) {
        tok->done = E_COLUMNOVERFLOW;
        بازگشت EOF;
    }
    بازگشت _PyTok_SourceByte(&tok->source, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
صحیح Py_Lexer_Begin_سطر(ساختار tok_state *);
صحیح Py_Lexer_Continue_سطر(ساختار tok_state *);
صحیح _PyLexer_IndentationToken(ساختار tok_state *, ساختار token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
صحیح _PyLexer_Newline(ساختار tok_state *, ساختار token *, صحیح);
تهی _PyLexer_backup(ساختار tok_state *, صحیح);
صحیح _PyLexer_record_ftstring_comment(
    ساختار tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
صحیح _PyLexer_ftstring_punctuation(
    ساختار tok_state *, ftstring_state *, ساختار token *, صحیح);
صحیح Py_Lexer_close_ftstring_عبارت(
    ساختار tok_state *, ftstring_state *, صحیح);
تهی _PyLexer_mark_ftstring_debug(ساختار tok_state *, ftstring_state *);
صحیح _PyLexer_check_string_prefixes(ساختار tok_state *, صحیح, صحیح, صحیح, صحیح, صحیح);
صحیح Py_Lexer_scan_عدد(ساختار tok_state *, ساختار token *, صحیح, صحیح);
صحیح _PyLexer_scan_fstring_start(
    ساختار tok_state *, ساختار token *, صحیح, ftstring_kind);
صحیح _PyLexer_scan_string(ساختار tok_state *, ساختار token *, صحیح);
صحیح _PyLexer_get_normal(ساختار tok_state *, ftstring_state *, ساختار token *);
صحیح _PyLexer_get_ftstring(ساختار tok_state *, ftstring_state *, ساختار token *);

#endif
