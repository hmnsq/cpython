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

static anta_hstha puur_naa_nka
tok_failed(const sa_nracanaa tok_state *tok)
{
    ꦮꦭꦶ tok->done != E_OK && tok->done != E_EOF &&
           tok->done != E_INTERACT_STOP;
}

puur_naa_nka _PyLexer_refill(sa_nracanaa tok_state *);

static anta_hstha puur_naa_nka
tok_nextc(sa_nracanaa tok_state *tok)
{
    ꦱꦼꦢꦼꦭ (tok->cur == tok->inp) {
        ꦪꦼꦤ꧀ (!_PyLexer_refill(tok)) {
            ꦮꦭꦶ EOF;
        }
    }
    ꦪꦼꦤ꧀ (tok->cur - tok->line_start >= INT_MAX) {
        tok->done = E_COLUMNOVERFLOW;
        ꦮꦭꦶ EOF;
    }
    ꦮꦭꦶ _PyTok_SourceByte(&tok->source, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
puur_naa_nka _PyLexer_BeginLine(sa_nracanaa tok_state *);
puur_naa_nka _PyLexer_ContinueLine(sa_nracanaa tok_state *);
puur_naa_nka _PyLexer_IndentationToken(sa_nracanaa tok_state *, sa_nracanaa token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
puur_naa_nka _PyLexer_Newline(sa_nracanaa tok_state *, sa_nracanaa token *, puur_naa_nka);
void _PyLexer_backup(sa_nracanaa tok_state *, puur_naa_nka);
puur_naa_nka _PyLexer_record_ftstring_comment(
    sa_nracanaa tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
puur_naa_nka _PyLexer_ftstring_punctuation(
    sa_nracanaa tok_state *, ftstring_state *, sa_nracanaa token *, puur_naa_nka);
puur_naa_nka _PyLexer_close_ftstring_expr(
    sa_nracanaa tok_state *, ftstring_state *, puur_naa_nka);
void _PyLexer_mark_ftstring_debug(sa_nracanaa tok_state *, ftstring_state *);
puur_naa_nka _PyLexer_check_string_prefixes(sa_nracanaa tok_state *, puur_naa_nka, puur_naa_nka, puur_naa_nka, puur_naa_nka, puur_naa_nka);
puur_naa_nka _PyLexer_scan_number(sa_nracanaa tok_state *, sa_nracanaa token *, puur_naa_nka, puur_naa_nka);
puur_naa_nka _PyLexer_scan_fstring_start(
    sa_nracanaa tok_state *, sa_nracanaa token *, puur_naa_nka, ftstring_kind);
puur_naa_nka _PyLexer_scan_string(sa_nracanaa tok_state *, sa_nracanaa token *, puur_naa_nka);
puur_naa_nka _PyLexer_get_normal(sa_nracanaa tok_state *, ftstring_state *, sa_nracanaa token *);
puur_naa_nka _PyLexer_get_ftstring(sa_nracanaa tok_state *, ftstring_state *, sa_nracanaa token *);

#endif
