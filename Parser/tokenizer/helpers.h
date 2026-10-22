#ifndef _PY_TOKENIZER_HELPERS_H_
#define _PY_TOKENIZER_HELPERS_H_

#include "Python.h"

#include "../lexer/state.h"

целое _PyTokenizer_syntaxerror(struct tok_state *tok, const char *format, ...);
/* Positive range columns are 1-based byte columns. A start column of -1
   derives the character column from the reporting cursor; an end column of
   -1 uses the start column. */
целое _PyTokenizer_syntaxerror_known_range(struct tok_state *tok, целое col_offset, целое end_col_offset, const char *format, ...);
целое _PyTokenizer_syntaxerror_at(
    struct tok_state *tok, const char *line_start, Py_ssize_t cursor_offset,
    целое lineno, целое col_offset, целое end_col_offset, const char *format, ...);
целое _PyTokenizer_indenterror(struct tok_state *tok);
целое _PyTokenizer_warn_invalid_escape_sequence(struct tok_state *tok, целое first_invalid_escape_char);
целое _PyTokenizer_parser_warn(struct tok_state *tok, PyObject *category, const char *format, ...);

void _PyTokenizer_raise_init_error(PyObject *filename);

целое _PyTokenizer_ensure_utf8(const char *line, struct tok_state *tok, целое lineno);

#ifdef Py_DEBUG
void _PyTokenizer_print_escape(FILE *f, const char *s, Py_ssize_t size);
#endif


#endif
