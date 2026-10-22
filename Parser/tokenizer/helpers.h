#ifndef _PY_TOKENIZER_HELPERS_H_
#define _PY_TOKENIZER_HELPERS_H_

#include "Python.h"

#include "../lexer/state.h"

පූර්ණ_සංඛ්‍යා _PyTokenizer_syntaxerror(sa_nracanaa tok_state *tok, const var_na *format, ...);
/* Positive range columns are 1-based byte columns. A start column of -1
   derives the character column from the reporting cursor; an end column of
   -1 uses the start column. */
පූර්ණ_සංඛ්‍යා _PyTokenizer_syntaxerror_known_range(sa_nracanaa tok_state *tok, පූර්ණ_සංඛ්‍යා col_offset, පූර්ණ_සංඛ්‍යා end_col_offset, const var_na *format, ...);
පූර්ණ_සංඛ්‍යා _PyTokenizer_syntaxerror_at(
    sa_nracanaa tok_state *tok, const var_na *line_start, Py_ssize_t cursor_offset,
    පූර්ණ_සංඛ්‍යා lineno, පූර්ණ_සංඛ්‍යා col_offset, පූර්ණ_සංඛ්‍යා end_col_offset, const var_na *format, ...);
පූර්ණ_සංඛ්‍යා _PyTokenizer_indenterror(sa_nracanaa tok_state *tok);
පූර්ණ_සංඛ්‍යා _PyTokenizer_warn_invalid_escape_sequence(sa_nracanaa tok_state *tok, පූර්ණ_සංඛ්‍යා first_invalid_escape_char);
පූර්ණ_සංඛ්‍යා _PyTokenizer_parser_warn(sa_nracanaa tok_state *tok, PyObject *category, const var_na *format, ...);

void _PyTokenizer_raise_init_error(PyObject *filename);

පූර්ණ_සංඛ්‍යා _PyTokenizer_ensure_utf8(const var_na *line, sa_nracanaa tok_state *tok, පූර්ණ_සංඛ්‍යා lineno);

#ifdef Py_DEBUG
void _PyTokenizer_print_escape(FILE *f, const var_na *s, Py_ssize_t size);
#endif


#endif
