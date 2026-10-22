#यदि_अपरिभाषितम् PY_LEXER_आन्तरिकम्_H
#परिभाषय PY_LEXER_आन्तरिकम्_H

#अन्तर्भाव "errcode.h"
#अन्तर्भाव "state.h"

#परिभाषय is_potential_identifier_start(c) (\
              (c >= 'a' && c <= 'z')\
               || (c >= 'A' && c <= 'Z')\
               || c == '_'\
               || (c >= 128))

#परिभाषय is_potential_identifier_char(c) (\
              (c >= 'a' && c <= 'z')\
               || (c >= 'A' && c <= 'Z')\
               || (c >= '0' && c <= '9')\
               || c == '_'\
               || (c >= 128))

#परिभाषय FTSTRING_मध्यः(state) \
    (_PyLexer_IsTString((state)->विधा) ? TSTRING_मध्यः : FSTRING_मध्यः)
#परिभाषय FTSTRING_अन्तः(state) \
    (_PyLexer_IsTString((state)->विधा) ? TSTRING_अन्तः : FSTRING_अन्तः)
#परिभाषय tok_backup _PyLexer_backup

स्थिर अन्तःस्थ पूर्णाङ्क
tok_failed(ध्रुव संरचना tok_state *tok)
{
    प्रत्यागम tok->समाप्तम् != E_OK && tok->समाप्तम् != E_EOF &&
           tok->समाप्तम् != E_INTERACT_STOP;
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
    यदि (tok->cur - tok->पङ्क्तिः_start >= अधिकतम_पूर्णाङ्कः) {
        tok->समाप्तम् = E_COLUMNOVERFLOW;
        प्रत्यागम EOF;
    }
    प्रत्यागम _PyTok_SourceByte(&tok->source, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
पूर्णाङ्क Py_Lexer_Begin_पङ्क्तिः(संरचना tok_state *);
पूर्णाङ्क Py_Lexer_Continue_पङ्क्तिः(संरचना tok_state *);
पूर्णाङ्क _PyLexer_IndentationToken(संरचना tok_state *, संरचना token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
पूर्णाङ्क _PyLexer_Newline(संरचना tok_state *, संरचना token *, पूर्णाङ्क);
शून्य _PyLexer_backup(संरचना tok_state *, पूर्णाङ्क);
पूर्णाङ्क Py_Lexer_लेखः_ftstring_comment(
    संरचना tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
पूर्णाङ्क _PyLexer_ftstring_punctuation(
    संरचना tok_state *, ftstring_state *, संरचना token *, पूर्णाङ्क);
पूर्णाङ्क Py_Lexer_close_ftstring_व्यञ्जकम्(
    संरचना tok_state *, ftstring_state *, पूर्णाङ्क);
शून्य _PyLexer_mark_ftstring_debug(संरचना tok_state *, ftstring_state *);
पूर्णाङ्क _PyLexer_check_string_prefixes(संरचना tok_state *, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क);
पूर्णाङ्क Py_Lexer_अवलोकनम्_सङ्ख्या(संरचना tok_state *, संरचना token *, पूर्णाङ्क, पूर्णाङ्क);
पूर्णाङ्क Py_Lexer_अवलोकनम्_fstring_start(
    संरचना tok_state *, संरचना token *, पूर्णाङ्क, ftstring_विधा);
पूर्णाङ्क Py_Lexer_अवलोकनम्_string(संरचना tok_state *, संरचना token *, पूर्णाङ्क);
पूर्णाङ्क Py_Lexer_get_प्रसामान्यम्(संरचना tok_state *, ftstring_state *, संरचना token *);
पूर्णाङ्क _PyLexer_get_ftstring(संरचना tok_state *, ftstring_state *, संरचना token *);

#यदि_अन्तः
