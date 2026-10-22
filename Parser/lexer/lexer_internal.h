#ifndef _PY_LEXER_INTERNAL_H_
#define _PY_LEXER_INTERNAL_H_

#അന്തര്ഭാവ "errcode.h"
#അന്തര്ഭാവ "state.h"

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

#define FTSTRING_നടുവ്(state) \
    (_PyLexer_IsTString((state)->kind) ? TSTRING_നടുവ് : FSTRING_നടുവ്)
#define FTSTRING_END(state) \
    (_PyLexer_IsTString((state)->kind) ? TSTRING_END : FSTRING_END)
#define tok_backup _PyLexer_backup

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ
tok_failed(സ്ഥിരം ഘടന tok_state *tok)
{
    തിരികെ tok->done != E_OK && tok->done != E_EOF &&
           tok->done != E_INTERACT_STOP;
}

പൂർണ്ണസംഖ്യ _PyLexer_refill(ഘടന tok_state *);

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ
tok_nextc(ഘടന tok_state *tok)
{
    ആകുമ്പോൾ (tok->cur == tok->inp) {
        എങ്കിൽ (!_PyLexer_refill(tok)) {
            തിരികെ EOF;
        }
    }
    എങ്കിൽ (tok->cur - tok->വരി_start >= INT_MAX) {
        tok->done = E_COLUMNOVERFLOW;
        തിരികെ EOF;
    }
    തിരികെ _PyTok_SourceByte(&tok->source, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
പൂർണ്ണസംഖ്യ Py_Lexer_Begin_വരി(ഘടന tok_state *);
പൂർണ്ണസംഖ്യ Py_Lexer_Continue_വരി(ഘടന tok_state *);
പൂർണ്ണസംഖ്യ _PyLexer_IndentationToken(ഘടന tok_state *, ഘടന token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
പൂർണ്ണസംഖ്യ _PyLexer_Newline(ഘടന tok_state *, ഘടന token *, പൂർണ്ണസംഖ്യ);
ശൂന്യം _PyLexer_backup(ഘടന tok_state *, പൂർണ്ണസംഖ്യ);
പൂർണ്ണസംഖ്യ _PyLexer_record_ftstring_comment(
    ഘടന tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
പൂർണ്ണസംഖ്യ _PyLexer_ftstring_punctuation(
    ഘടന tok_state *, ftstring_state *, ഘടന token *, പൂർണ്ണസംഖ്യ);
പൂർണ്ണസംഖ്യ Py_Lexer_close_ftstring_വ്യഞ്ജകം(
    ഘടന tok_state *, ftstring_state *, പൂർണ്ണസംഖ്യ);
ശൂന്യം _PyLexer_mark_ftstring_debug(ഘടന tok_state *, ftstring_state *);
പൂർണ്ണസംഖ്യ _PyLexer_check_string_prefixes(ഘടന tok_state *, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ);
പൂർണ്ണസംഖ്യ Py_Lexer_scan_സംഖ്യ(ഘടന tok_state *, ഘടന token *, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ);
പൂർണ്ണസംഖ്യ _PyLexer_scan_fstring_start(
    ഘടന tok_state *, ഘടന token *, പൂർണ്ണസംഖ്യ, ftstring_kind);
പൂർണ്ണസംഖ്യ _PyLexer_scan_string(ഘടന tok_state *, ഘടന token *, പൂർണ്ണസംഖ്യ);
പൂർണ്ണസംഖ്യ _PyLexer_get_normal(ഘടന tok_state *, ftstring_state *, ഘടന token *);
പൂർണ്ണസംഖ്യ _PyLexer_get_ftstring(ഘടന tok_state *, ftstring_state *, ഘടന token *);

#endif
