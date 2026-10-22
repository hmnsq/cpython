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

static anta_hstha ཧྲིལ
tok_failed(const sa_nracanaa tok_state *tok)
{
    ཕྱིར tok->done != E_OK && tok->done != E_EOF &&
           tok->done != E_INTERACT_STOP;
}

ཧྲིལ _PyLexer_refill(sa_nracanaa tok_state *);

static anta_hstha ཧྲིལ
tok_nextc(sa_nracanaa tok_state *tok)
{
    སྐབས (tok->cur == tok->inp) {
        གལ་ཏེ (!_PyLexer_refill(tok)) {
            ཕྱིར EOF;
        }
    }
    གལ་ཏེ (tok->cur - tok->line_start >= INT_MAX) {
        tok->done = E_COLUMNOVERFLOW;
        ཕྱིར EOF;
    }
    ཕྱིར _PyTok_SourceByte(&tok->source, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
ཧྲིལ _PyLexer_BeginLine(sa_nracanaa tok_state *);
ཧྲིལ _PyLexer_ContinueLine(sa_nracanaa tok_state *);
ཧྲིལ _PyLexer_IndentationToken(sa_nracanaa tok_state *, sa_nracanaa token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
ཧྲིལ _PyLexer_Newline(sa_nracanaa tok_state *, sa_nracanaa token *, ཧྲིལ);
void _PyLexer_backup(sa_nracanaa tok_state *, ཧྲིལ);
ཧྲིལ _PyLexer_record_ftstring_comment(
    sa_nracanaa tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
ཧྲིལ _PyLexer_ftstring_punctuation(
    sa_nracanaa tok_state *, ftstring_state *, sa_nracanaa token *, ཧྲིལ);
ཧྲིལ _PyLexer_close_ftstring_expr(
    sa_nracanaa tok_state *, ftstring_state *, ཧྲིལ);
void _PyLexer_mark_ftstring_debug(sa_nracanaa tok_state *, ftstring_state *);
ཧྲིལ _PyLexer_check_string_prefixes(sa_nracanaa tok_state *, ཧྲིལ, ཧྲིལ, ཧྲིལ, ཧྲིལ, ཧྲིལ);
ཧྲིལ _PyLexer_scan_number(sa_nracanaa tok_state *, sa_nracanaa token *, ཧྲིལ, ཧྲིལ);
ཧྲིལ _PyLexer_scan_fstring_start(
    sa_nracanaa tok_state *, sa_nracanaa token *, ཧྲིལ, ftstring_kind);
ཧྲིལ _PyLexer_scan_string(sa_nracanaa tok_state *, sa_nracanaa token *, ཧྲིལ);
ཧྲིལ _PyLexer_get_normal(sa_nracanaa tok_state *, ftstring_state *, sa_nracanaa token *);
ཧྲིལ _PyLexer_get_ftstring(sa_nracanaa tok_state *, ftstring_state *, sa_nracanaa token *);

#endif
