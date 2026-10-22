#ifndef _PY_LEXER_INTERNAL_H_
#define _PY_LEXER_INTERNAL_H_

#অন্তর্ভাব "errcode.h"
#অন্তর্ভাব "state.h"

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

#define FTSTRING_মধ্য(state) \
    (_PyLexer_IsTString((state)->kind) ? TSTRING_মধ্য : FSTRING_মধ্য)
#define FTSTRING_END(state) \
    (_PyLexer_IsTString((state)->kind) ? TSTRING_END : FSTRING_END)
#define tok_backup _PyLexer_backup

স্থির অন্তঃরেখ পূর্ণসংখ্যা
tok_failed(ধ্রুবক কাঠামো tok_state *tok)
{
    ফেরত tok->done != E_OK && tok->done != E_EOF &&
           tok->done != E_INTERACT_STOP;
}

পূর্ণসংখ্যা _PyLexer_refill(কাঠামো tok_state *);

স্থির অন্তঃরেখ পূর্ণসংখ্যা
tok_nextc(কাঠামো tok_state *tok)
{
    যখন (tok->cur == tok->inp) {
        যদি (!_PyLexer_refill(tok)) {
            ফেরত EOF;
        }
    }
    যদি (tok->cur - tok->সারি_start >= INT_MAX) {
        tok->done = E_COLUMNOVERFLOW;
        ফেরত EOF;
    }
    ফেরত _PyTok_SourceByte(&tok->source, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
পূর্ণসংখ্যা Py_Lexer_Begin_সারি(কাঠামো tok_state *);
পূর্ণসংখ্যা Py_Lexer_Continue_সারি(কাঠামো tok_state *);
পূর্ণসংখ্যা _PyLexer_IndentationToken(কাঠামো tok_state *, কাঠামো token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
পূর্ণসংখ্যা _PyLexer_Newline(কাঠামো tok_state *, কাঠামো token *, পূর্ণসংখ্যা);
শূন্য _PyLexer_backup(কাঠামো tok_state *, পূর্ণসংখ্যা);
পূর্ণসংখ্যা _PyLexer_record_ftstring_comment(
    কাঠামো tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
পূর্ণসংখ্যা _PyLexer_ftstring_punctuation(
    কাঠামো tok_state *, ftstring_state *, কাঠামো token *, পূর্ণসংখ্যা);
পূর্ণসংখ্যা Py_Lexer_close_ftstring_রাশিমালা(
    কাঠামো tok_state *, ftstring_state *, পূর্ণসংখ্যা);
শূন্য _PyLexer_mark_ftstring_debug(কাঠামো tok_state *, ftstring_state *);
পূর্ণসংখ্যা _PyLexer_check_string_prefixes(কাঠামো tok_state *, পূর্ণসংখ্যা, পূর্ণসংখ্যা, পূর্ণসংখ্যা, পূর্ণসংখ্যা, পূর্ণসংখ্যা);
পূর্ণসংখ্যা Py_Lexer_scan_সংখ্যা(কাঠামো tok_state *, কাঠামো token *, পূর্ণসংখ্যা, পূর্ণসংখ্যা);
পূর্ণসংখ্যা _PyLexer_scan_fstring_start(
    কাঠামো tok_state *, কাঠামো token *, পূর্ণসংখ্যা, ftstring_kind);
পূর্ণসংখ্যা _PyLexer_scan_string(কাঠামো tok_state *, কাঠামো token *, পূর্ণসংখ্যা);
পূর্ণসংখ্যা _PyLexer_get_normal(কাঠামো tok_state *, ftstring_state *, কাঠামো token *);
পূর্ণসংখ্যা _PyLexer_get_ftstring(কাঠামো tok_state *, ftstring_state *, কাঠামো token *);

#endif
