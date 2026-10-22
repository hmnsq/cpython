#ifndef _PY_TOKENIZER_HELPERS_H_
#define _PY_TOKENIZER_HELPERS_H_

#अन्तर्भाव "Python.h"

#अन्तर्भाव "../lexer/state.h"

पूर्णांक _PyTokenizer_syntaxerror(संरचना tok_state *tok, स्थिरांक अक्षर *format, ...);
/* Positive range columns are 1-based byte columns. A start column of -1
   derives the character column from the reporting cursor; an end column of
   -1 uses the start column. */
पूर्णांक _PyTokenizer_syntaxerror_known_range(संरचना tok_state *tok, पूर्णांक col_offset, पूर्णांक end_col_offset, स्थिरांक अक्षर *format, ...);
पूर्णांक _PyTokenizer_syntaxerror_at(
    संरचना tok_state *tok, स्थिरांक अक्षर *ओळ_start, Py_ssize_t cursor_offset,
    पूर्णांक lineno, पूर्णांक col_offset, पूर्णांक end_col_offset, स्थिरांक अक्षर *format, ...);
पूर्णांक _PyTokenizer_indenterror(संरचना tok_state *tok);
पूर्णांक _PyTokenizer_warn_invalid_escape_sequence(संरचना tok_state *tok, पूर्णांक first_invalid_escape_char);
पूर्णांक _PyTokenizer_parser_warn(संरचना tok_state *tok, PyObject *category, स्थिरांक अक्षर *format, ...);

शून्य _PyTokenizer_raise_init_error(PyObject *filename);

पूर्णांक _PyTokenizer_ensure_utf8(स्थिरांक अक्षर *ओळ, संरचना tok_state *tok, पूर्णांक lineno);

#ifdef Py_DEBUG
शून्य _PyTokenizer_print_escape(खाता *f, स्थिरांक अक्षर *s, Py_ssize_t size);
#endif


#endif
