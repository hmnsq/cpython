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

static inline цео
tok_failed(const struct tok_state *tok)
{
    врати tok->done != E_OK && tok->done != E_EOF &&
           tok->done != E_INTERACT_STOP;
}

цео _PyLexer_refill(struct tok_state *);

static inline цео
tok_nextc(struct tok_state *tok)
{
    док (tok->cur == tok->inp) {
        ако (!_PyLexer_refill(tok)) {
            врати EOF;
        }
    }
    ако (tok->cur - tok->line_start >= INT_MAX) {
        tok->done = E_COLUMNOVERFLOW;
        врати EOF;
    }
    врати _PyTok_SourceByte(&tok->source, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
цео _PyLexer_BeginLine(struct tok_state *);
цео _PyLexer_ContinueLine(struct tok_state *);
цео _PyLexer_IndentationToken(struct tok_state *, struct token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
цео _PyLexer_Newline(struct tok_state *, struct token *, цео);
void _PyLexer_backup(struct tok_state *, цео);
цео _PyLexer_record_ftstring_comment(
    struct tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
цео _PyLexer_ftstring_punctuation(
    struct tok_state *, ftstring_state *, struct token *, цео);
цео _PyLexer_close_ftstring_expr(
    struct tok_state *, ftstring_state *, цео);
void _PyLexer_mark_ftstring_debug(struct tok_state *, ftstring_state *);
цео _PyLexer_check_string_prefixes(struct tok_state *, цео, цео, цео, цео, цео);
цео _PyLexer_scan_number(struct tok_state *, struct token *, цео, цео);
цео _PyLexer_scan_fstring_start(
    struct tok_state *, struct token *, цео, ftstring_kind);
цео _PyLexer_scan_string(struct tok_state *, struct token *, цео);
цео _PyLexer_get_normal(struct tok_state *, ftstring_state *, struct token *);
цео _PyLexer_get_ftstring(struct tok_state *, ftstring_state *, struct token *);

#endif
