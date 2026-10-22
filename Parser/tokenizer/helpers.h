#ifndef _PY_TOKENIZER_HELPERS_H_
#define _PY_TOKENIZER_HELPERS_H_

#شامل "Python.h"

#شامل "../lexer/state.h"

صحیح _PyTokenizer_syntaxerror(ساختار tok_state *tok, ثابت نویسه *format, ...);
/* Positive range columns are 1-based byte columns. A start column of -1
   derives the character column from the reporting cursor; an end column of
   -1 uses the start column. */
صحیح _PyTokenizer_syntaxerror_known_range(ساختار tok_state *tok, صحیح col_offset, صحیح end_col_offset, ثابت نویسه *format, ...);
صحیح _PyTokenizer_syntaxerror_at(
    ساختار tok_state *tok, ثابت نویسه *سطر_start, Py_ssize_t cursor_offset,
    صحیح lineno, صحیح col_offset, صحیح end_col_offset, ثابت نویسه *format, ...);
صحیح _PyTokenizer_indenterror(ساختار tok_state *tok);
صحیح _PyTokenizer_warn_invalid_escape_sequence(ساختار tok_state *tok, صحیح first_invalid_escape_char);
صحیح _PyTokenizer_parser_warn(ساختار tok_state *tok, PyObject *category, ثابت نویسه *format, ...);

تهی _PyTokenizer_raise_init_error(PyObject *filename);

صحیح _PyTokenizer_ensure_utf8(ثابت نویسه *سطر, ساختار tok_state *tok, صحیح lineno);

#ifdef Py_DEBUG
تهی _PyTokenizer_print_escape(FILE *f, ثابت نویسه *s, Py_ssize_t size);
#endif


#endif
