#ifndef _PY_TOKENIZER_HELPERS_H_
#define _PY_TOKENIZER_HELPERS_H_

#include "Python.h"

#include "../lexer/state.h"

整数 _PyTokenizer_syntaxerror(struct tok_state *tok, const char *format, ...);
/* Positive range columns are 1-based byte columns. A start column of -1
   derives the character column from the reporting cursor; an end column of
   -1 uses the start column. */
整数 _PyTokenizer_syntaxerror_known_range(struct tok_state *tok, 整数 col_offset, 整数 end_col_offset, const char *format, ...);
整数 _PyTokenizer_syntaxerror_at(
    struct tok_state *tok, const char *line_start, Py_ssize_t cursor_offset,
    整数 lineno, 整数 col_offset, 整数 end_col_offset, const char *format, ...);
整数 _PyTokenizer_indenterror(struct tok_state *tok);
整数 _PyTokenizer_warn_invalid_escape_sequence(struct tok_state *tok, 整数 first_invalid_escape_char);
整数 _PyTokenizer_parser_warn(struct tok_state *tok, PyObject *category, const char *format, ...);

void _PyTokenizer_raise_init_error(PyObject *filename);

整数 _PyTokenizer_ensure_utf8(const char *line, struct tok_state *tok, 整数 lineno);

#ifdef Py_DEBUG
void _PyTokenizer_print_escape(FILE *f, const char *s, Py_ssize_t size);
#endif


#endif
