#که_نه_تعریف_شوی _PY_TOKENIZER_HELPERS_H_
#تعریف_کړه _PY_TOKENIZER_HELPERS_H_

#ضمن "Python.h"

#ضمن "../lexer/state.h"

صحيح _PyTokenizer_syntaxerror(بنية tok_state *tok, ثابت حرف *format, ...);
/* Positive range columns are 1-based byte columns. A start column of -1
   derives the character column from the reporting cursor; an end column of
   -1 uses the start column. */
صحيح Py_Tokenizer_syntaxerror_معلوم_range(بنية tok_state *tok, صحيح ستون_دننه_واټن, صحيح پای_ټکی_ستون_دننه_واټن, ثابت حرف *format, ...);
صحيح _PyTokenizer_syntaxerror_at(
    بنية tok_state *tok, ثابت حرف *کرښه_پیل, Py_ssize_t cursor_دننه_واټن,
    صحيح lineno, صحيح ستون_دننه_واټن, صحيح پای_ټکی_ستون_دننه_واټن, ثابت حرف *format, ...);
صحيح _PyTokenizer_indenterror(بنية tok_state *tok);
صحيح _PyTokenizer_warn_invalid_escape_sequence(بنية tok_state *tok, صحيح لومړی_invalid_escape_char);
صحيح _PyTokenizer_parser_warn(بنية tok_state *tok, PyObject *category, ثابت حرف *format, ...);

فراغ _PyTokenizer_raise_init_error(PyObject *filename);

صحيح _PyTokenizer_ensure_utf8(ثابت حرف *کرښه, بنية tok_state *tok, صحيح lineno);

#که_تعریف_شوی Py_DEBUG
فراغ _PyTokenizer_print_escape(FILE *f, ثابت حرف *s, Py_ssize_t اندازه);
#د_که_پای


#د_که_پای
