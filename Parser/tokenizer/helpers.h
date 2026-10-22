#ifndef _PY_TOKENIZER_HELPERS_H_
#define _PY_TOKENIZER_HELPERS_H_

#include "Python.h"

#include "../lexer/state.h"

puur_naa_nka _PyTokenizer_syntaxerror(sa_nracanaa tok_state *tok, const var_na *format, ...);
/* Positive range columns are 1-based byte columns. A start column of -1
   derives the character column from the reporting cursor; an end column of
   -1 uses the start column. */
puur_naa_nka _PyTokenizer_syntaxerror_known_range(sa_nracanaa tok_state *tok, puur_naa_nka col_offset, puur_naa_nka end_col_offset, const var_na *format, ...);
puur_naa_nka _PyTokenizer_syntaxerror_at(
    sa_nracanaa tok_state *tok, const var_na *line_start, Py_ssize_t cursor_offset,
    puur_naa_nka lineno, puur_naa_nka col_offset, puur_naa_nka end_col_offset, const var_na *format, ...);
puur_naa_nka _PyTokenizer_indenterror(sa_nracanaa tok_state *tok);
puur_naa_nka _PyTokenizer_warn_invalid_escape_sequence(sa_nracanaa tok_state *tok, puur_naa_nka first_invalid_escape_char);
puur_naa_nka _PyTokenizer_parser_warn(sa_nracanaa tok_state *tok, PyObject *category, const var_na *format, ...);

void _PyTokenizer_raise_init_error(PyObject *filename);

puur_naa_nka _PyTokenizer_ensure_utf8(const var_na *line, sa_nracanaa tok_state *tok, puur_naa_nka lineno);

#ifdef Py_DEBUG
void _PyTokenizer_print_escape(FILE *f, const var_na *s, Py_ssize_t size);
#endif


#endif
