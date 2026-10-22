#ifndef _PY_LEXER_INTERNAL_H_
#define _PY_LEXER_INTERNAL_H_

#अन्तर्भाव "errcode.h"
#अन्तर्भाव "state.h"

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

#define FTSTRING_मध्य(state) \
    (_PyLexer_IsTString((state)->kind) ? TSTRING_मध्य : FSTRING_मध्य)
#define FTSTRING_END(state) \
    (_PyLexer_IsTString((state)->kind) ? TSTRING_END : FSTRING_END)
#define tok_backup _PyLexer_backup

स्थिर अन्तःस्थ पूर्णाङ्क
tok_failed(ध्रुव संरचना tok_state *tok)
{
    प्रत्यागम tok->done != E_OK && tok->done != E_EOF &&
           tok->done != E_INTERACT_STOP;
}

पूर्णाङ्क _PyLexer_refill(संरचना tok_state *);

स्थिर अन्तःस्थ पूर्णाङ्क
tok_nextc(संरचना tok_state *tok)
{
    यावत् (tok->cur == tok->inp) {
        यदि (!_PyLexer_refill(tok)) {
            प्रत्यागम EOF;
        }
    }
    यदि (tok->cur - tok->पंक्ति_start >= INT_MAX) {
        tok->done = E_COLUMNOVERFLOW;
        प्रत्यागम EOF;
    }
    प्रत्यागम _PyTok_SourceByte(&tok->source, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
पूर्णाङ्क Py_Lexer_Begin_पंक्ति(संरचना tok_state *);
पूर्णाङ्क Py_Lexer_Continue_पंक्ति(संरचना tok_state *);
पूर्णाङ्क _PyLexer_IndentationToken(संरचना tok_state *, संरचना token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
पूर्णाङ्क _PyLexer_Newline(संरचना tok_state *, संरचना token *, पूर्णाङ्क);
शून्य _PyLexer_backup(संरचना tok_state *, पूर्णाङ्क);
पूर्णाङ्क _PyLexer_record_ftstring_comment(
    संरचना tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
पूर्णाङ्क _PyLexer_ftstring_punctuation(
    संरचना tok_state *, ftstring_state *, संरचना token *, पूर्णाङ्क);
पूर्णाङ्क Py_Lexer_close_ftstring_व्यंजक(
    संरचना tok_state *, ftstring_state *, पूर्णाङ्क);
शून्य _PyLexer_mark_ftstring_debug(संरचना tok_state *, ftstring_state *);
पूर्णाङ्क _PyLexer_check_string_prefixes(संरचना tok_state *, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क);
पूर्णाङ्क Py_Lexer_scan_संख्या(संरचना tok_state *, संरचना token *, पूर्णाङ्क, पूर्णाङ्क);
पूर्णाङ्क _PyLexer_scan_fstring_start(
    संरचना tok_state *, संरचना token *, पूर्णाङ्क, ftstring_kind);
पूर्णाङ्क _PyLexer_scan_string(संरचना tok_state *, संरचना token *, पूर्णाङ्क);
पूर्णाङ्क _PyLexer_get_normal(संरचना tok_state *, ftstring_state *, संरचना token *);
पूर्णाङ्क _PyLexer_get_ftstring(संरचना tok_state *, ftstring_state *, संरचना token *);

#endif
