#ifndef _PY_LEXER_INTERNAL_H_
#define _PY_LEXER_INTERNAL_H_

#ଅନ୍ତର୍ଭାଵ "errcode.h"
#ଅନ୍ତର୍ଭାଵ "state.h"

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

#define FTSTRING_ମଧ୍ୟ(state) \
    (_PyLexer_IsTString((state)->kind) ? TSTRING_ମଧ୍ୟ : FSTRING_ମଧ୍ୟ)
#define FTSTRING_END(state) \
    (_PyLexer_IsTString((state)->kind) ? TSTRING_END : FSTRING_END)
#define tok_backup _PyLexer_backup

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tok_failed(ଅଚଳ ସଂରଚନା tok_state *tok)
{
    ଫେରନ୍ତୁ tok->done != E_OK && tok->done != E_EOF &&
           tok->done != E_INTERACT_STOP;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyLexer_refill(ସଂରଚନା tok_state *);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tok_nextc(ସଂରଚନା tok_state *tok)
{
    ଯେତେବେଳେ (tok->cur == tok->inp) {
        ଯଦି (!_PyLexer_refill(tok)) {
            ଫେରନ୍ତୁ EOF;
        }
    }
    ଯଦି (tok->cur - tok->ଧାଡ଼ି_start >= INT_MAX) {
        tok->done = E_COLUMNOVERFLOW;
        ଫେରନ୍ତୁ EOF;
    }
    ଫେରନ୍ତୁ _PyTok_SourceByte(&tok->source, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Py_Lexer_Begin_ଧାଡ଼ି(ସଂରଚନା tok_state *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Py_Lexer_Continue_ଧାଡ଼ି(ସଂରଚନା tok_state *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyLexer_IndentationToken(ସଂରଚନା tok_state *, ସଂରଚନା token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyLexer_Newline(ସଂରଚନା tok_state *, ସଂରଚନା token *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ଶୂନ୍ୟ _PyLexer_backup(ସଂରଚନା tok_state *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyLexer_record_ftstring_comment(
    ସଂରଚନା tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyLexer_ftstring_punctuation(
    ସଂରଚନା tok_state *, ftstring_state *, ସଂରଚନା token *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Py_Lexer_close_ftstring_ପଦସମଷ୍ଟି(
    ସଂରଚନା tok_state *, ftstring_state *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ଶୂନ୍ୟ _PyLexer_mark_ftstring_debug(ସଂରଚନା tok_state *, ftstring_state *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyLexer_check_string_prefixes(ସଂରଚନା tok_state *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Py_Lexer_scan_ସଂଖ୍ୟା(ସଂରଚନା tok_state *, ସଂରଚନା token *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyLexer_scan_fstring_start(
    ସଂରଚନା tok_state *, ସଂରଚନା token *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ftstring_kind);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyLexer_scan_string(ସଂରଚନା tok_state *, ସଂରଚନା token *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyLexer_get_normal(ସଂରଚନା tok_state *, ftstring_state *, ସଂରଚନା token *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyLexer_get_ftstring(ସଂରଚନା tok_state *, ftstring_state *, ସଂରଚନା token *);

#endif
