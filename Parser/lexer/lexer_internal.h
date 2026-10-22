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

#define FTSTRING_बीच(state) \
    (_PyLexer_IsTString((state)->kind) ? TSTRING_बीच : FSTRING_बीच)
#define FTSTRING_END(state) \
    (_PyLexer_IsTString((state)->kind) ? TSTRING_END : FSTRING_END)
#define tok_backup _PyLexer_backup

स्थिर अन्तःस्थ पूर्णाङ्क
tok_failed(अचर संरचना tok_state *tok)
{
    फर्काऊ tok->done != E_OK && tok->done != E_EOF &&
           tok->done != E_INTERACT_STOP;
}

पूर्णाङ्क _PyLexer_refill(संरचना tok_state *);

स्थिर अन्तःस्थ पूर्णाङ्क
tok_nextc(संरचना tok_state *tok)
{
    जबसम्म (tok->cur == tok->inp) {
        यदि (!_PyLexer_refill(tok)) {
            फर्काऊ खातापूर्ण;
        }
    }
    यदि (tok->cur - tok->पङ्क्ति_start >= पूर्णांक_उच्च) {
        tok->done = E_COLUMNOVERFLOW;
        फर्काऊ खातापूर्ण;
    }
    फर्काऊ _PyTok_SourceByte(&tok->source, tok->cur++);
}

/* Return -1 on error, otherwise whether the line is blank. */
पूर्णाङ्क Py_Lexer_Begin_पङ्क्ति(संरचना tok_state *);
पूर्णाङ्क Py_Lexer_Continue_पङ्क्ति(संरचना tok_state *);
पूर्णाङ्क _PyLexer_IndentationToken(संरचना tok_state *, संरचना token *);
/* Return zero when the newline is suppressed, otherwise its token type. */
पूर्णाङ्क _PyLexer_Newline(संरचना tok_state *, संरचना token *, पूर्णाङ्क);
शून्य _PyLexer_backup(संरचना tok_state *, पूर्णाङ्क);
पूर्णाङ्क _PyLexer_record_ftstring_comment(
    संरचना tok_state *, ftstring_state *, _PyTok_Off, _PyTok_Off);
पूर्णाङ्क _PyLexer_ftstring_punctuation(
    संरचना tok_state *, ftstring_state *, संरचना token *, पूर्णाङ्क);
पूर्णाङ्क Py_Lexer_close_ftstring_अभिव्यक्ति(
    संरचना tok_state *, ftstring_state *, पूर्णाङ्क);
शून्य _PyLexer_mark_ftstring_debug(संरचना tok_state *, ftstring_state *);
पूर्णाङ्क _PyLexer_check_string_prefixes(संरचना tok_state *, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क);
पूर्णाङ्क Py_Lexer_scan_सङ्ख्या(संरचना tok_state *, संरचना token *, पूर्णाङ्क, पूर्णाङ्क);
पूर्णाङ्क _PyLexer_scan_fstring_start(
    संरचना tok_state *, संरचना token *, पूर्णाङ्क, ftstring_kind);
पूर्णाङ्क _PyLexer_scan_string(संरचना tok_state *, संरचना token *, पूर्णाङ्क);
पूर्णाङ्क _PyLexer_get_normal(संरचना tok_state *, ftstring_state *, संरचना token *);
पूर्णाङ्क _PyLexer_get_ftstring(संरचना tok_state *, ftstring_state *, संरचना token *);

#endif
