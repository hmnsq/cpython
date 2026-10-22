#ifndef _PY_TOKENIZER_HELPERS_H_
#define _PY_TOKENIZER_HELPERS_H_

#அந்தர்பாவ "Python.h"

#அந்தர்பாவ "../lexer/state.h"

முழு _PyTokenizer_syntaxerror(கட்டமைப்பு tok_state *tok, மாறிலி எழுத்து *format, ...);
/* Positive range columns are 1-based byte columns. A start column of -1
   derives the character column from the reporting cursor; an end column of
   -1 uses the start column. */
முழு _PyTokenizer_syntaxerror_known_range(கட்டமைப்பு tok_state *tok, முழு col_offset, முழு end_col_offset, மாறிலி எழுத்து *format, ...);
முழு _PyTokenizer_syntaxerror_at(
    கட்டமைப்பு tok_state *tok, மாறிலி எழுத்து *வரி_start, Py_ssize_t cursor_offset,
    முழு lineno, முழு col_offset, முழு end_col_offset, மாறிலி எழுத்து *format, ...);
முழு _PyTokenizer_indenterror(கட்டமைப்பு tok_state *tok);
முழு _PyTokenizer_warn_invalid_escape_sequence(கட்டமைப்பு tok_state *tok, முழு first_invalid_escape_char);
முழு _PyTokenizer_parser_warn(கட்டமைப்பு tok_state *tok, PyObject *category, மாறிலி எழுத்து *format, ...);

வெறுமை _PyTokenizer_raise_init_error(PyObject *filename);

முழு _PyTokenizer_ensure_utf8(மாறிலி எழுத்து *வரி, கட்டமைப்பு tok_state *tok, முழு lineno);

#ifdef Py_DEBUG
வெறுமை _PyTokenizer_print_escape(FILE *f, மாறிலி எழுத்து *s, Py_ssize_t size);
#endif


#endif
