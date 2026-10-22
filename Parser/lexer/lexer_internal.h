#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PY_LEXER_ਅੰਦਰੂਨੀ_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PY_LEXER_ਅੰਦਰੂਨੀ_H

#ਅਨ੍ਤਰ੍ਭਾਵ "errcode.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "state.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ is_potential_identifier_ਸ਼ੁਰੂ(c) (\
              (c >= 'a' && c <= 'z')\
               || (c >= 'A' && c <= 'Z')\
               || c == '_'\
               || (c >= 128))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ is_potential_identifier_char(c) (\
              (c >= 'a' && c <= 'z')\
               || (c >= 'A' && c <= 'Z')\
               || (c >= '0' && c <= '9')\
               || c == '_'\
               || (c >= 128))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FTSTRING_ਵਿਚਕਾਰ(state) \
    (_PyLexer_IsTString((state)->ਕਿਸਮ) ? TSTRING_ਵਿਚਕਾਰ : FSTRING_ਵਿਚਕਾਰ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FTSTRING_ਸਿਰਾ(state) \
    (_PyLexer_IsTString((state)->ਕਿਸਮ) ? TSTRING_ਸਿਰਾ : FSTRING_ਸਿਰਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ tok_backup _PyLexer_backup

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ
tok_failed(ਅਚਲ ਬਣਤਰ tok_state *tok)
{
    ਵਾਪਸ tok->ਪੂਰਾ != E_OK && tok->ਪੂਰਾ != E_EOF &&
           tok->ਪੂਰਾ != E_INTERACT_STOP;
}

ਪੂਰਨ _PyLexer_refill(ਬਣਤਰ tok_state *);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ
tok_nextc(ਬਣਤਰ tok_state *tok)
{
    ਜਦੋਂ (tok->cur == tok->inp) {
        ਜੇ (!_PyLexer_refill(tok)) {
            ਵਾਪਸ EOF;
        }
    }
    ਜੇ (tok->cur - tok->ਸਤਰ_ਸ਼ੁਰੂ >= ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) {
        tok->ਪੂਰਾ = E_COLUMNOVERFLOW;
        ਵਾਪਸ EOF;
    }
    ਵਾਪਸ Py_Tok_ਸਰੋਤ_Byte(&tok->ਸਰੋਤ, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
ਪੂਰਨ Py_Lexer_Begin_ਸਤਰ(ਬਣਤਰ tok_state *);
ਪੂਰਨ Py_Lexer_Continue_ਸਤਰ(ਬਣਤਰ tok_state *);
ਪੂਰਨ _PyLexer_IndentationToken(ਬਣਤਰ tok_state *, ਬਣਤਰ token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
ਪੂਰਨ _PyLexer_Newline(ਬਣਤਰ tok_state *, ਬਣਤਰ token *, ਪੂਰਨ);
ਖਾਲੀ _PyLexer_backup(ਬਣਤਰ tok_state *, ਪੂਰਨ);
ਪੂਰਨ Py_Lexer_ਰਿਕਾਰਡ_ftstring_comment(
    ਬਣਤਰ tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
ਪੂਰਨ _PyLexer_ftstring_punctuation(
    ਬਣਤਰ tok_state *, ftstring_state *, ਬਣਤਰ token *, ਪੂਰਨ);
ਪੂਰਨ Py_Lexer_close_ftstring_ਵਿਅੰਜਕ(
    ਬਣਤਰ tok_state *, ftstring_state *, ਪੂਰਨ);
ਖਾਲੀ _PyLexer_mark_ftstring_debug(ਬਣਤਰ tok_state *, ftstring_state *);
ਪੂਰਨ _PyLexer_check_string_prefixes(ਬਣਤਰ tok_state *, ਪੂਰਨ, ਪੂਰਨ, ਪੂਰਨ, ਪੂਰਨ, ਪੂਰਨ);
ਪੂਰਨ Py_Lexer_ਜਾਂਚ_ਸੰਖਿਆ(ਬਣਤਰ tok_state *, ਬਣਤਰ token *, ਪੂਰਨ, ਪੂਰਨ);
ਪੂਰਨ Py_Lexer_ਜਾਂਚ_fstring_ਸ਼ੁਰੂ(
    ਬਣਤਰ tok_state *, ਬਣਤਰ token *, ਪੂਰਨ, ftstring_ਕਿਸਮ);
ਪੂਰਨ Py_Lexer_ਜਾਂਚ_string(ਬਣਤਰ tok_state *, ਬਣਤਰ token *, ਪੂਰਨ);
ਪੂਰਨ Py_Lexer_get_ਸਾਧਾਰਨ(ਬਣਤਰ tok_state *, ftstring_state *, ਬਣਤਰ token *);
ਪੂਰਨ _PyLexer_get_ftstring(ਬਣਤਰ tok_state *, ftstring_state *, ਬਣਤਰ token *);

#ਜੇ_ਅੰਤ
