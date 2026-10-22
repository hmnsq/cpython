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

static anta_hstha පූර්ණ_සංඛ්‍යා
tok_failed(const sa_nracanaa tok_state *tok)
{
    ආපසු tok->done != E_OK && tok->done != E_EOF &&
           tok->done != E_INTERACT_STOP;
}

පූර්ණ_සංඛ්‍යා _PyLexer_refill(sa_nracanaa tok_state *);

static anta_hstha පූර්ණ_සංඛ්‍යා
tok_nextc(sa_nracanaa tok_state *tok)
{
    අතරතුර (tok->cur == tok->inp) {
        නම් (!_PyLexer_refill(tok)) {
            ආපසු EOF;
        }
    }
    නම් (tok->cur - tok->line_start >= INT_MAX) {
        tok->done = E_COLUMNOVERFLOW;
        ආපසු EOF;
    }
    ආපසු _PyTok_SourceByte(&tok->source, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
පූර්ණ_සංඛ්‍යා _PyLexer_BeginLine(sa_nracanaa tok_state *);
පූර්ණ_සංඛ්‍යා _PyLexer_ContinueLine(sa_nracanaa tok_state *);
පූර්ණ_සංඛ්‍යා _PyLexer_IndentationToken(sa_nracanaa tok_state *, sa_nracanaa token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
පූර්ණ_සංඛ්‍යා _PyLexer_Newline(sa_nracanaa tok_state *, sa_nracanaa token *, පූර්ණ_සංඛ්‍යා);
void _PyLexer_backup(sa_nracanaa tok_state *, පූර්ණ_සංඛ්‍යා);
පූර්ණ_සංඛ්‍යා _PyLexer_record_ftstring_comment(
    sa_nracanaa tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
පූර්ණ_සංඛ්‍යා _PyLexer_ftstring_punctuation(
    sa_nracanaa tok_state *, ftstring_state *, sa_nracanaa token *, පූර්ණ_සංඛ්‍යා);
පූර්ණ_සංඛ්‍යා _PyLexer_close_ftstring_expr(
    sa_nracanaa tok_state *, ftstring_state *, පූර්ණ_සංඛ්‍යා);
void _PyLexer_mark_ftstring_debug(sa_nracanaa tok_state *, ftstring_state *);
පූර්ණ_සංඛ්‍යා _PyLexer_check_string_prefixes(sa_nracanaa tok_state *, පූර්ණ_සංඛ්‍යා, පූර්ණ_සංඛ්‍යා, පූර්ණ_සංඛ්‍යා, පූර්ණ_සංඛ්‍යා, පූර්ණ_සංඛ්‍යා);
පූර්ණ_සංඛ්‍යා _PyLexer_scan_number(sa_nracanaa tok_state *, sa_nracanaa token *, පූර්ණ_සංඛ්‍යා, පූර්ණ_සංඛ්‍යා);
පූර්ණ_සංඛ්‍යා _PyLexer_scan_fstring_start(
    sa_nracanaa tok_state *, sa_nracanaa token *, පූර්ණ_සංඛ්‍යා, ftstring_kind);
පූර්ණ_සංඛ්‍යා _PyLexer_scan_string(sa_nracanaa tok_state *, sa_nracanaa token *, පූර්ණ_සංඛ්‍යා);
පූර්ණ_සංඛ්‍යා _PyLexer_get_normal(sa_nracanaa tok_state *, ftstring_state *, sa_nracanaa token *);
පූර්ණ_සංඛ්‍යා _PyLexer_get_ftstring(sa_nracanaa tok_state *, ftstring_state *, sa_nracanaa token *);

#endif
