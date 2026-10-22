#ifndef _PY_LEXER_INTERNAL_H_
#define _PY_LEXER_INTERNAL_H_

#અન્તર્ભાવ "errcode.h"
#અન્તર્ભાવ "state.h"

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

#define FTSTRING_મધ્ય(state) \
    (_PyLexer_IsTString((state)->kind) ? TSTRING_મધ્ય : FSTRING_મધ્ય)
#define FTSTRING_END(state) \
    (_PyLexer_IsTString((state)->kind) ? TSTRING_END : FSTRING_END)
#define tok_backup _PyLexer_backup

સ્થિર અંતઃરેખ પૂર્ણાંક
tok_failed(અચલ સંરચના tok_state *tok)
{
    પાછા tok->done != E_OK && tok->done != E_EOF &&
           tok->done != E_INTERACT_STOP;
}

પૂર્ણાંક _PyLexer_refill(સંરચના tok_state *);

સ્થિર અંતઃરેખ પૂર્ણાંક
tok_nextc(સંરચના tok_state *tok)
{
    જ્યારે (tok->cur == tok->inp) {
        જો (!_PyLexer_refill(tok)) {
            પાછા EOF;
        }
    }
    જો (tok->cur - tok->લીટી_start >= INT_MAX) {
        tok->done = E_COLUMNOVERFLOW;
        પાછા EOF;
    }
    પાછા _PyTok_SourceByte(&tok->source, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
પૂર્ણાંક Py_Lexer_Begin_લીટી(સંરચના tok_state *);
પૂર્ણાંક Py_Lexer_Continue_લીટી(સંરચના tok_state *);
પૂર્ણાંક _PyLexer_IndentationToken(સંરચના tok_state *, સંરચના token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
પૂર્ણાંક _PyLexer_Newline(સંરચના tok_state *, સંરચના token *, પૂર્ણાંક);
શૂન્ય _PyLexer_backup(સંરચના tok_state *, પૂર્ણાંક);
પૂર્ણાંક _PyLexer_record_ftstring_comment(
    સંરચના tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
પૂર્ણાંક _PyLexer_ftstring_punctuation(
    સંરચના tok_state *, ftstring_state *, સંરચના token *, પૂર્ણાંક);
પૂર્ણાંક Py_Lexer_close_ftstring_પદાવલિ(
    સંરચના tok_state *, ftstring_state *, પૂર્ણાંક);
શૂન્ય _PyLexer_mark_ftstring_debug(સંરચના tok_state *, ftstring_state *);
પૂર્ણાંક _PyLexer_check_string_prefixes(સંરચના tok_state *, પૂર્ણાંક, પૂર્ણાંક, પૂર્ણાંક, પૂર્ણાંક, પૂર્ણાંક);
પૂર્ણાંક Py_Lexer_scan_સંખ્યા(સંરચના tok_state *, સંરચના token *, પૂર્ણાંક, પૂર્ણાંક);
પૂર્ણાંક _PyLexer_scan_fstring_start(
    સંરચના tok_state *, સંરચના token *, પૂર્ણાંક, ftstring_kind);
પૂર્ણાંક _PyLexer_scan_string(સંરચના tok_state *, સંરચના token *, પૂર્ણાંક);
પૂર્ણાંક _PyLexer_get_normal(સંરચના tok_state *, ftstring_state *, સંરચના token *);
પૂર્ણાંક _PyLexer_get_ftstring(સંરચના tok_state *, ftstring_state *, સંરચના token *);

#endif
