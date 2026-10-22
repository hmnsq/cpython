#جيڪڏهن_وصف_نه_ٿيل _PY_TOKENIZER_HELPERS_H_
#وصف_ڪريو _PY_TOKENIZER_HELPERS_H_

#ضمن "Python.h"

#ضمن "../lexer/state.h"

صحيح _PyTokenizer_syntaxerror(بنية tok_state *tok, ثابت حرف *format, ...);
/* Positive range columns are 1-based byte columns. A start column of -1
   derives the character column from the reporting cursor; an end column of
   -1 uses the start column. */
صحيح Py_Tokenizer_syntaxerror_معلوم_range(بنية tok_state *tok, صحيح کانو_وٿي, صحيح پڇاڙي_کانو_وٿي, ثابت حرف *format, ...);
صحيح _PyTokenizer_syntaxerror_at(
    بنية tok_state *tok, ثابت حرف *سٽ_شروعات, Py_ssize_t cursor_وٿي,
    صحيح lineno, صحيح کانو_وٿي, صحيح پڇاڙي_کانو_وٿي, ثابت حرف *format, ...);
صحيح _PyTokenizer_indenterror(بنية tok_state *tok);
صحيح _PyTokenizer_warn_invalid_escape_sequence(بنية tok_state *tok, صحيح پهريون_invalid_escape_char);
صحيح _PyTokenizer_parser_warn(بنية tok_state *tok, PyObject *category, ثابت حرف *format, ...);

فراغ _PyTokenizer_raise_init_error(PyObject *filename);

صحيح _PyTokenizer_ensure_utf8(ثابت حرف *سٽ, بنية tok_state *tok, صحيح lineno);

#جيڪڏهن_وصف_ٿيل Py_DEBUG
فراغ _PyTokenizer_print_escape(FILE *f, ثابت حرف *s, Py_ssize_t سائيز);
#جيڪڏهن_پورو


#جيڪڏهن_پورو
