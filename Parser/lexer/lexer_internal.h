#నిర్వచితం_కాకపోతే PY_LEXER_అంతర్గత_H
#నిర్వచించు PY_LEXER_అంతర్గత_H

#అన్తర్భావ "errcode.h"
#అన్తర్భావ "state.h"

#నిర్వచించు is_potential_identifier_ప్రారంభం(c) (\
              (c >= 'a' && c <= 'z')\
               || (c >= 'A' && c <= 'Z')\
               || c == '_'\
               || (c >= 128))

#నిర్వచించు is_potential_identifier_char(c) (\
              (c >= 'a' && c <= 'z')\
               || (c >= 'A' && c <= 'Z')\
               || (c >= '0' && c <= '9')\
               || c == '_'\
               || (c >= 128))

#నిర్వచించు FTSTRING_మధ్య(state) \
    (_PyLexer_IsTString((state)->రకం) ? TSTRING_మధ్య : FSTRING_మధ్య)
#నిర్వచించు FTSTRING_చివర(state) \
    (_PyLexer_IsTString((state)->రకం) ? TSTRING_చివర : FSTRING_చివర)
#నిర్వచించు tok_backup _PyLexer_backup

స్థిర అంతఃవరుస పూర్ణాంకం
tok_failed(స్థిరరాశి నిర్మాణం tok_state *tok)
{
    తిరిగి tok->పూర్తి != E_OK && tok->పూర్తి != E_EOF &&
           tok->పూర్తి != E_INTERACT_STOP;
}

పూర్ణాంకం _PyLexer_refill(నిర్మాణం tok_state *);

స్థిర అంతఃవరుస పూర్ణాంకం
tok_nextc(నిర్మాణం tok_state *tok)
{
    అయితే (tok->cur == tok->inp) {
        ఒకవేళ (!_PyLexer_refill(tok)) {
            తిరిగి EOF;
        }
    }
    ఒకవేళ (tok->cur - tok->పంక్తి_ప్రారంభం >= గరిష్ఠ_పూర్ణాంకం) {
        tok->పూర్తి = E_COLUMNOVERFLOW;
        తిరిగి EOF;
    }
    తిరిగి Py_Tok_ఆధారం_Byte(&tok->ఆధారం, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
పూర్ణాంకం Py_Lexer_Begin_పంక్తి(నిర్మాణం tok_state *);
పూర్ణాంకం Py_Lexer_Continue_పంక్తి(నిర్మాణం tok_state *);
పూర్ణాంకం _PyLexer_IndentationToken(నిర్మాణం tok_state *, నిర్మాణం token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
పూర్ణాంకం _PyLexer_Newline(నిర్మాణం tok_state *, నిర్మాణం token *, పూర్ణాంకం);
శూన్య _PyLexer_backup(నిర్మాణం tok_state *, పూర్ణాంకం);
పూర్ణాంకం Py_Lexer_రికార్డు_ftstring_comment(
    నిర్మాణం tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
పూర్ణాంకం _PyLexer_ftstring_punctuation(
    నిర్మాణం tok_state *, ftstring_state *, నిర్మాణం token *, పూర్ణాంకం);
పూర్ణాంకం Py_Lexer_close_ftstring_సమాసం(
    నిర్మాణం tok_state *, ftstring_state *, పూర్ణాంకం);
శూన్య _PyLexer_mark_ftstring_debug(నిర్మాణం tok_state *, ftstring_state *);
పూర్ణాంకం _PyLexer_check_string_prefixes(నిర్మాణం tok_state *, పూర్ణాంకం, పూర్ణాంకం, పూర్ణాంకం, పూర్ణాంకం, పూర్ణాంకం);
పూర్ణాంకం Py_Lexer_పరిశీలన_సంఖ్య(నిర్మాణం tok_state *, నిర్మాణం token *, పూర్ణాంకం, పూర్ణాంకం);
పూర్ణాంకం Py_Lexer_పరిశీలన_fstring_ప్రారంభం(
    నిర్మాణం tok_state *, నిర్మాణం token *, పూర్ణాంకం, ftstring_రకం);
పూర్ణాంకం Py_Lexer_పరిశీలన_string(నిర్మాణం tok_state *, నిర్మాణం token *, పూర్ణాంకం);
పూర్ణాంకం Py_Lexer_get_సాధారణ(నిర్మాణం tok_state *, ftstring_state *, నిర్మాణం token *);
పూర్ణాంకం _PyLexer_get_ftstring(నిర్మాణం tok_state *, ftstring_state *, నిర్మాణం token *);

#అయితే_ముగింపు
