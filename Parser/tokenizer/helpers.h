#ifndef _PY_TOKENIZER_HELPERS_H_
#define _PY_TOKENIZER_HELPERS_H_

#ضمن "Python.h"

#ضمن "../lexer/state.h"

صحيح _PyTokenizer_syntaxerror(بنية tok_state *tok, ثابت حرف *format, ...);
/* Positive range columns are 1-based byte columns. A start column of -1
   derives the character column from the reporting cursor; an end column of
   -1 uses the start column. */
صحيح _PyTokenizer_syntaxerror_known_range(بنية tok_state *tok, صحيح col_offset, صحيح end_col_offset, ثابت حرف *format, ...);
صحيح _PyTokenizer_syntaxerror_at(
    بنية tok_state *tok, ثابت حرف *لي_start, Py_ssize_t cursor_offset,
    صحيح lineno, صحيح col_offset, صحيح end_col_offset, ثابت حرف *format, ...);
صحيح _PyTokenizer_indenterror(بنية tok_state *tok);
صحيح _PyTokenizer_warn_invalid_escape_sequence(بنية tok_state *tok, صحيح first_invalid_escape_char);
صحيح _PyTokenizer_parser_warn(بنية tok_state *tok, PyObject *category, ثابت حرف *format, ...);

فراغ _PyTokenizer_raise_init_error(PyObject *filename);

صحيح _PyTokenizer_ensure_utf8(ثابت حرف *لي, بنية tok_state *tok, صحيح lineno);

#ifdef Py_DEBUG
فراغ Py_Tokenizer_بغ_escape(FILE *f, ثابت حرف *s, Py_ssize_t size);
#endif


#endif
