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

স্থির অন্তঃস্থ পূর্ণাঙ্ক
tok_failed(ধ্রুব সংরচনা tok_state *tok)
{
    প্রত্যাগম tok->done != E_OK && tok->done != E_EOF &&
           tok->done != E_INTERACT_STOP;
}

পূর্ণাঙ্ক _PyLexer_refill(সংরচনা tok_state *);

স্থির অন্তঃস্থ পূর্ণাঙ্ক
tok_nextc(সংরচনা tok_state *tok)
{
    যাবত্ (tok->cur == tok->inp) {
        যদি (!_PyLexer_refill(tok)) {
            প্রত্যাগম EOF;
        }
    }
    যদি (tok->cur - tok->সারি_start >= INT_MAX) {
        tok->done = E_COLUMNOVERFLOW;
        প্রত্যাগম EOF;
    }
    প্রত্যাগম _PyTok_SourceByte(&tok->source, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
পূর্ণাঙ্ক Py_Lexer_Begin_সারি(সংরচনা tok_state *);
পূর্ণাঙ্ক Py_Lexer_Continue_সারি(সংরচনা tok_state *);
পূর্ণাঙ্ক _PyLexer_IndentationToken(সংরচনা tok_state *, সংরচনা token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
পূর্ণাঙ্ক _PyLexer_Newline(সংরচনা tok_state *, সংরচনা token *, পূর্ণাঙ্ক);
শূন্য _PyLexer_backup(সংরচনা tok_state *, পূর্ণাঙ্ক);
পূর্ণাঙ্ক _PyLexer_record_ftstring_comment(
    সংরচনা tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
পূর্ণাঙ্ক _PyLexer_ftstring_punctuation(
    সংরচনা tok_state *, ftstring_state *, সংরচনা token *, পূর্ণাঙ্ক);
পূর্ণাঙ্ক Py_Lexer_close_ftstring_রাশিমালা(
    সংরচনা tok_state *, ftstring_state *, পূর্ণাঙ্ক);
শূন্য _PyLexer_mark_ftstring_debug(সংরচনা tok_state *, ftstring_state *);
পূর্ণাঙ্ক _PyLexer_check_string_prefixes(সংরচনা tok_state *, পূর্ণাঙ্ক, পূর্ণাঙ্ক, পূর্ণাঙ্ক, পূর্ণাঙ্ক, পূর্ণাঙ্ক);
পূর্ণাঙ্ক Py_Lexer_scan_সংখ্যা(সংরচনা tok_state *, সংরচনা token *, পূর্ণাঙ্ক, পূর্ণাঙ্ক);
পূর্ণাঙ্ক _PyLexer_scan_fstring_start(
    সংরচনা tok_state *, সংরচনা token *, পূর্ণাঙ্ক, ftstring_kind);
পূর্ণাঙ্ক _PyLexer_scan_string(সংরচনা tok_state *, সংরচনা token *, পূর্ণাঙ্ক);
পূর্ণাঙ্ক _PyLexer_get_normal(সংরচনা tok_state *, ftstring_state *, সংরচনা token *);
পূর্ণাঙ্ক _PyLexer_get_ftstring(সংরচনা tok_state *, ftstring_state *, সংরচনা token *);

#endif
