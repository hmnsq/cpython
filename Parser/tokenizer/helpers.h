#جے_بنا_تعریف _PY_TOKENIZER_HELPERS_H_
#تعریف_کرو _PY_TOKENIZER_HELPERS_H_

#ضمن "Python.h"

#ضمن "../lexer/state.h"

صحيح _PyTokenizer_syntaxerror(بنية tok_state *tok, ثابت حرف *format, ...);
/* Positive range columns are 1-based byte columns. A start column of -1
   derives the character column from the reporting cursor; an end column of
   -1 uses the start column. */
صحيح Py_Tokenizer_syntaxerror_معلوم_range(بنية tok_state *tok, صحيح خانہ_وتھ, صحيح سرا_خانہ_وتھ, ثابت حرف *format, ...);
صحيح _PyTokenizer_syntaxerror_at(
    بنية tok_state *tok, ثابت حرف *سطر_شروع, Py_ssize_t cursor_وتھ,
    صحيح lineno, صحيح خانہ_وتھ, صحيح سرا_خانہ_وتھ, ثابت حرف *format, ...);
صحيح _PyTokenizer_indenterror(بنية tok_state *tok);
صحيح _PyTokenizer_warn_invalid_escape_sequence(بنية tok_state *tok, صحيح پہلا_invalid_escape_char);
صحيح _PyTokenizer_parser_warn(بنية tok_state *tok, PyObject *category, ثابت حرف *format, ...);

فراغ _PyTokenizer_raise_init_error(PyObject *filename);

صحيح _PyTokenizer_ensure_utf8(ثابت حرف *سطر, بنية tok_state *tok, صحيح lineno);

#جے_تعریف_شدہ Py_DEBUG
فراغ _PyTokenizer_print_escape(FILE *f, ثابت حرف *s, Py_ssize_t ناپ);
#جے_انت


#جے_انت
