#ifndef _PY_LEXER_INTERNAL_H_
#define _PY_LEXER_INTERNAL_H_

#அந்தர்பாவ "errcode.h"
#அந்தர்பாவ "state.h"

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

#define FTSTRING_நடு(state) \
    (_PyLexer_IsTString((state)->kind) ? TSTRING_நடு : FSTRING_நடு)
#define FTSTRING_END(state) \
    (_PyLexer_IsTString((state)->kind) ? TSTRING_END : FSTRING_END)
#define tok_backup _PyLexer_backup

நிலையான உள்வரி முழு
tok_failed(மாறிலி கட்டமைப்பு tok_state *tok)
{
    திரும்பு tok->done != E_OK && tok->done != E_EOF &&
           tok->done != E_INTERACT_STOP;
}

முழு _PyLexer_refill(கட்டமைப்பு tok_state *);

நிலையான உள்வரி முழு
tok_nextc(கட்டமைப்பு tok_state *tok)
{
    போது (tok->cur == tok->inp) {
        எனில் (!_PyLexer_refill(tok)) {
            திரும்பு EOF;
        }
    }
    எனில் (tok->cur - tok->வரி_start >= INT_MAX) {
        tok->done = E_COLUMNOVERFLOW;
        திரும்பு EOF;
    }
    திரும்பு _PyTok_SourceByte(&tok->source, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
முழு Py_Lexer_Begin_வரி(கட்டமைப்பு tok_state *);
முழு Py_Lexer_Continue_வரி(கட்டமைப்பு tok_state *);
முழு _PyLexer_IndentationToken(கட்டமைப்பு tok_state *, கட்டமைப்பு token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
முழு _PyLexer_Newline(கட்டமைப்பு tok_state *, கட்டமைப்பு token *, முழு);
வெறுமை _PyLexer_backup(கட்டமைப்பு tok_state *, முழு);
முழு _PyLexer_record_ftstring_comment(
    கட்டமைப்பு tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
முழு _PyLexer_ftstring_punctuation(
    கட்டமைப்பு tok_state *, ftstring_state *, கட்டமைப்பு token *, முழு);
முழு Py_Lexer_close_ftstring_கோவை(
    கட்டமைப்பு tok_state *, ftstring_state *, முழு);
வெறுமை _PyLexer_mark_ftstring_debug(கட்டமைப்பு tok_state *, ftstring_state *);
முழு _PyLexer_check_string_prefixes(கட்டமைப்பு tok_state *, முழு, முழு, முழு, முழு, முழு);
முழு Py_Lexer_scan_எண்(கட்டமைப்பு tok_state *, கட்டமைப்பு token *, முழு, முழு);
முழு _PyLexer_scan_fstring_start(
    கட்டமைப்பு tok_state *, கட்டமைப்பு token *, முழு, ftstring_kind);
முழு _PyLexer_scan_string(கட்டமைப்பு tok_state *, கட்டமைப்பு token *, முழு);
முழு _PyLexer_get_normal(கட்டமைப்பு tok_state *, ftstring_state *, கட்டமைப்பு token *);
முழு _PyLexer_get_ftstring(கட்டமைப்பு tok_state *, ftstring_state *, கட்டமைப்பு token *);

#endif
