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

static inline số_nguyên
tok_failed(const struct tok_state *tok)
{
    trả_về tok->done != E_OK && tok->done != E_EOF &&
           tok->done != E_INTERACT_STOP;
}

số_nguyên _PyLexer_refill(struct tok_state *);

static inline số_nguyên
tok_nextc(struct tok_state *tok)
{
    trong_khi (tok->cur == tok->inp) {
        nếu (!_PyLexer_refill(tok)) {
            trả_về EOF;
        }
    }
    nếu (tok->cur - tok->line_start >= INT_MAX) {
        tok->done = E_COLUMNOVERFLOW;
        trả_về EOF;
    }
    trả_về _PyTok_SourceByte(&tok->source, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
số_nguyên _PyLexer_BeginLine(struct tok_state *);
số_nguyên _PyLexer_ContinueLine(struct tok_state *);
số_nguyên _PyLexer_IndentationToken(struct tok_state *, struct token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
số_nguyên _PyLexer_Newline(struct tok_state *, struct token *, số_nguyên);
void _PyLexer_backup(struct tok_state *, số_nguyên);
số_nguyên _PyLexer_record_ftstring_comment(
    struct tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
số_nguyên _PyLexer_ftstring_punctuation(
    struct tok_state *, ftstring_state *, struct token *, số_nguyên);
số_nguyên _PyLexer_close_ftstring_expr(
    struct tok_state *, ftstring_state *, số_nguyên);
void _PyLexer_mark_ftstring_debug(struct tok_state *, ftstring_state *);
số_nguyên _PyLexer_check_string_prefixes(struct tok_state *, số_nguyên, số_nguyên, số_nguyên, số_nguyên, số_nguyên);
số_nguyên _PyLexer_scan_number(struct tok_state *, struct token *, số_nguyên, số_nguyên);
số_nguyên _PyLexer_scan_fstring_start(
    struct tok_state *, struct token *, số_nguyên, ftstring_kind);
số_nguyên _PyLexer_scan_string(struct tok_state *, struct token *, số_nguyên);
số_nguyên _PyLexer_get_normal(struct tok_state *, ftstring_state *, struct token *);
số_nguyên _PyLexer_get_ftstring(struct tok_state *, ftstring_state *, struct token *);

#endif
