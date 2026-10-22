#ifndef _PY_LEXER_INTERNAL_H_
#define _PY_LEXER_INTERNAL_H_

#ಅನ್ತರ್ಭಾವ "errcode.h"
#ಅನ್ತರ್ಭಾವ "state.h"

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

#define FTSTRING_ಮಧ್ಯ(state) \
    (_PyLexer_IsTString((state)->kind) ? TSTRING_ಮಧ್ಯ : FSTRING_ಮಧ್ಯ)
#define FTSTRING_END(state) \
    (_PyLexer_IsTString((state)->kind) ? TSTRING_END : FSTRING_END)
#define tok_backup _PyLexer_backup

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ
tok_failed(ಧ್ರುವ ಸಂರಚನಾ tok_state *tok)
{
    ಪ್ರತ್ಯಾಗಮ tok->done != E_OK && tok->done != E_EOF &&
           tok->done != E_INTERACT_STOP;
}

ಪೂರ್ಣಾಙ್ಕ _PyLexer_refill(ಸಂರಚನಾ tok_state *);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ
tok_nextc(ಸಂರಚನಾ tok_state *tok)
{
    ಯಾವತ್ (tok->cur == tok->inp) {
        ಯದಿ (!_PyLexer_refill(tok)) {
            ಪ್ರತ್ಯಾಗಮ EOF;
        }
    }
    ಯದಿ (tok->cur - tok->ಸಾಲು_start >= INT_MAX) {
        tok->done = E_COLUMNOVERFLOW;
        ಪ್ರತ್ಯಾಗಮ EOF;
    }
    ಪ್ರತ್ಯಾಗಮ _PyTok_SourceByte(&tok->source, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
ಪೂರ್ಣಾಙ್ಕ Py_Lexer_Begin_ಸಾಲು(ಸಂರಚನಾ tok_state *);
ಪೂರ್ಣಾಙ್ಕ Py_Lexer_Continue_ಸಾಲು(ಸಂರಚನಾ tok_state *);
ಪೂರ್ಣಾಙ್ಕ _PyLexer_IndentationToken(ಸಂರಚನಾ tok_state *, ಸಂರಚನಾ token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
ಪೂರ್ಣಾಙ್ಕ _PyLexer_Newline(ಸಂರಚನಾ tok_state *, ಸಂರಚನಾ token *, ಪೂರ್ಣಾಙ್ಕ);
ಶೂನ್ಯ _PyLexer_backup(ಸಂರಚನಾ tok_state *, ಪೂರ್ಣಾಙ್ಕ);
ಪೂರ್ಣಾಙ್ಕ _PyLexer_record_ftstring_comment(
    ಸಂರಚನಾ tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
ಪೂರ್ಣಾಙ್ಕ _PyLexer_ftstring_punctuation(
    ಸಂರಚನಾ tok_state *, ftstring_state *, ಸಂರಚನಾ token *, ಪೂರ್ಣಾಙ್ಕ);
ಪೂರ್ಣಾಙ್ಕ Py_Lexer_close_ftstring_ಅಭಿವ್ಯಕ್ತಿ(
    ಸಂರಚನಾ tok_state *, ftstring_state *, ಪೂರ್ಣಾಙ್ಕ);
ಶೂನ್ಯ _PyLexer_mark_ftstring_debug(ಸಂರಚನಾ tok_state *, ftstring_state *);
ಪೂರ್ಣಾಙ್ಕ _PyLexer_check_string_prefixes(ಸಂರಚನಾ tok_state *, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ);
ಪೂರ್ಣಾಙ್ಕ Py_Lexer_scan_ಸಂಖ್ಯೆ(ಸಂರಚನಾ tok_state *, ಸಂರಚನಾ token *, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ);
ಪೂರ್ಣಾಙ್ಕ _PyLexer_scan_fstring_start(
    ಸಂರಚನಾ tok_state *, ಸಂರಚನಾ token *, ಪೂರ್ಣಾಙ್ಕ, ftstring_kind);
ಪೂರ್ಣಾಙ್ಕ _PyLexer_scan_string(ಸಂರಚನಾ tok_state *, ಸಂರಚನಾ token *, ಪೂರ್ಣಾಙ್ಕ);
ಪೂರ್ಣಾಙ್ಕ _PyLexer_get_normal(ಸಂರಚನಾ tok_state *, ftstring_state *, ಸಂರಚನಾ token *);
ಪೂರ್ಣಾಙ್ಕ _PyLexer_get_ftstring(ಸಂರಚನಾ tok_state *, ftstring_state *, ಸಂರಚನಾ token *);

#endif
