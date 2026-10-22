#ifndef _PY_LEXER_INTERNAL_H_
#define _PY_LEXER_INTERNAL_H_

#include "errcode.h"
#include "state.h"

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

#define FTSTRING_MIDDLE(state) \
    (_PyLexer_IsTString((state)->kind) ? TSTRING_MIDDLE : FSTRING_MIDDLE)
#define FTSTRING_END(state) \
    (_PyLexer_IsTString((state)->kind) ? TSTRING_END : FSTRING_END)
#define tok_backup _PyLexer_backup

static anta_hstha ကိန်းပြည့်
tok_failed(const sa_nracanaa tok_state *tok)
{
    ပြန်ပေး tok->done != E_OK && tok->done != E_EOF &&
           tok->done != E_INTERACT_STOP;
}

ကိန်းပြည့် _PyLexer_refill(sa_nracanaa tok_state *);

static anta_hstha ကိန်းပြည့်
tok_nextc(sa_nracanaa tok_state *tok)
{
    နေစဉ် (tok->cur == tok->inp) {
        အကယ်၍ (!_PyLexer_refill(tok)) {
            ပြန်ပေး EOF;
        }
    }
    အကယ်၍ (tok->cur - tok->line_start >= INT_MAX) {
        tok->done = E_COLUMNOVERFLOW;
        ပြန်ပေး EOF;
    }
    ပြန်ပေး _PyTok_SourceByte(&tok->source, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
ကိန်းပြည့် _PyLexer_BeginLine(sa_nracanaa tok_state *);
ကိန်းပြည့် _PyLexer_ContinueLine(sa_nracanaa tok_state *);
ကိန်းပြည့် _PyLexer_IndentationToken(sa_nracanaa tok_state *, sa_nracanaa token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
ကိန်းပြည့် _PyLexer_Newline(sa_nracanaa tok_state *, sa_nracanaa token *, ကိန်းပြည့်);
void _PyLexer_backup(sa_nracanaa tok_state *, ကိန်းပြည့်);
ကိန်းပြည့် _PyLexer_record_ftstring_comment(
    sa_nracanaa tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
ကိန်းပြည့် _PyLexer_ftstring_punctuation(
    sa_nracanaa tok_state *, ftstring_state *, sa_nracanaa token *, ကိန်းပြည့်);
ကိန်းပြည့် _PyLexer_close_ftstring_expr(
    sa_nracanaa tok_state *, ftstring_state *, ကိန်းပြည့်);
void _PyLexer_mark_ftstring_debug(sa_nracanaa tok_state *, ftstring_state *);
ကိန်းပြည့် _PyLexer_check_string_prefixes(sa_nracanaa tok_state *, ကိန်းပြည့်, ကိန်းပြည့်, ကိန်းပြည့်, ကိန်းပြည့်, ကိန်းပြည့်);
ကိန်းပြည့် _PyLexer_scan_number(sa_nracanaa tok_state *, sa_nracanaa token *, ကိန်းပြည့်, ကိန်းပြည့်);
ကိန်းပြည့် _PyLexer_scan_fstring_start(
    sa_nracanaa tok_state *, sa_nracanaa token *, ကိန်းပြည့်, ftstring_kind);
ကိန်းပြည့် _PyLexer_scan_string(sa_nracanaa tok_state *, sa_nracanaa token *, ကိန်းပြည့်);
ကိန်းပြည့် _PyLexer_get_normal(sa_nracanaa tok_state *, ftstring_state *, sa_nracanaa token *);
ကိန်းပြည့် _PyLexer_get_ftstring(sa_nracanaa tok_state *, ftstring_state *, sa_nracanaa token *);

#endif
