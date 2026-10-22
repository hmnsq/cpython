#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PY_LEXER_ಆಂತರಿಕ_H
#ವ್ಯಾಖ್ಯಾನಿಸು PY_LEXER_ಆಂತರಿಕ_H

#ಅನ್ತರ್ಭಾವ "errcode.h"
#ಅನ್ತರ್ಭಾವ "state.h"

#ವ್ಯಾಖ್ಯಾನಿಸು is_potential_identifier_ಆರಂಭ(c) (\
              (c >= 'a' && c <= 'z')\
               || (c >= 'A' && c <= 'Z')\
               || c == '_'\
               || (c >= 128))

#ವ್ಯಾಖ್ಯಾನಿಸು is_potential_identifier_char(c) (\
              (c >= 'a' && c <= 'z')\
               || (c >= 'A' && c <= 'Z')\
               || (c >= '0' && c <= '9')\
               || c == '_'\
               || (c >= 128))

#ವ್ಯಾಖ್ಯಾನಿಸು FTSTRING_ಮಧ್ಯ(state) \
    (_PyLexer_IsTString((state)->ಬಗೆ) ? TSTRING_ಮಧ್ಯ : FSTRING_ಮಧ್ಯ)
#ವ್ಯಾಖ್ಯಾನಿಸು FTSTRING_ಕೊನೆ(state) \
    (_PyLexer_IsTString((state)->ಬಗೆ) ? TSTRING_ಕೊನೆ : FSTRING_ಕೊನೆ)
#ವ್ಯಾಖ್ಯಾನಿಸು tok_backup _PyLexer_backup

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ
tok_failed(ಸ್ಥಿರಾಂಕ ರಚನೆ tok_state *tok)
{
    ಹಿಂತಿರುಗಿ tok->ಮುಗಿದ != E_OK && tok->ಮುಗಿದ != E_EOF &&
           tok->ಮುಗಿದ != E_INTERACT_STOP;
}

ಪೂರ್ಣಾಂಕ _PyLexer_refill(ರಚನೆ tok_state *);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ
tok_nextc(ರಚನೆ tok_state *tok)
{
    ಆಗ (tok->cur == tok->inp) {
        ಆದರೆ (!_PyLexer_refill(tok)) {
            ಹಿಂತಿರುಗಿ EOF;
        }
    }
    ಆದರೆ (tok->cur - tok->ಸಾಲು_ಆರಂಭ >= ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) {
        tok->ಮುಗಿದ = E_COLUMNOVERFLOW;
        ಹಿಂತಿರುಗಿ EOF;
    }
    ಹಿಂತಿರುಗಿ Py_Tok_ಮೂಲ_Byte(&tok->ಮೂಲ, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
ಪೂರ್ಣಾಂಕ Py_Lexer_Begin_ಸಾಲು(ರಚನೆ tok_state *);
ಪೂರ್ಣಾಂಕ Py_Lexer_Continue_ಸಾಲು(ರಚನೆ tok_state *);
ಪೂರ್ಣಾಂಕ _PyLexer_IndentationToken(ರಚನೆ tok_state *, ರಚನೆ token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
ಪೂರ್ಣಾಂಕ _PyLexer_Newline(ರಚನೆ tok_state *, ರಚನೆ token *, ಪೂರ್ಣಾಂಕ);
ಶೂನ್ಯ _PyLexer_backup(ರಚನೆ tok_state *, ಪೂರ್ಣಾಂಕ);
ಪೂರ್ಣಾಂಕ Py_Lexer_ದಾಖಲೆ_ftstring_comment(
    ರಚನೆ tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
ಪೂರ್ಣಾಂಕ _PyLexer_ftstring_punctuation(
    ರಚನೆ tok_state *, ftstring_state *, ರಚನೆ token *, ಪೂರ್ಣಾಂಕ);
ಪೂರ್ಣಾಂಕ Py_Lexer_close_ftstring_ಅಭಿವ್ಯಕ್ತಿ(
    ರಚನೆ tok_state *, ftstring_state *, ಪೂರ್ಣಾಂಕ);
ಶೂನ್ಯ _PyLexer_mark_ftstring_debug(ರಚನೆ tok_state *, ftstring_state *);
ಪೂರ್ಣಾಂಕ _PyLexer_check_string_prefixes(ರಚನೆ tok_state *, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ);
ಪೂರ್ಣಾಂಕ Py_Lexer_ಪರಿಶೀಲನೆ_ಸಂಖ್ಯೆ(ರಚನೆ tok_state *, ರಚನೆ token *, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ);
ಪೂರ್ಣಾಂಕ Py_Lexer_ಪರಿಶೀಲನೆ_fstring_ಆರಂಭ(
    ರಚನೆ tok_state *, ರಚನೆ token *, ಪೂರ್ಣಾಂಕ, ftstring_ಬಗೆ);
ಪೂರ್ಣಾಂಕ Py_Lexer_ಪರಿಶೀಲನೆ_string(ರಚನೆ tok_state *, ರಚನೆ token *, ಪೂರ್ಣಾಂಕ);
ಪೂರ್ಣಾಂಕ Py_Lexer_get_ಸಾಮಾನ್ಯ(ರಚನೆ tok_state *, ftstring_state *, ರಚನೆ token *);
ಪೂರ್ಣಾಂಕ _PyLexer_get_ftstring(ರಚನೆ tok_state *, ftstring_state *, ರಚನೆ token *);

#ಆದರೆ_ಅಂತ್ಯ
