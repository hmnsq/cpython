#ifndef _PY_TOKENIZER_HELPERS_H_
#define _PY_TOKENIZER_HELPERS_H_

#અન્તર્ભાવ "Python.h"

#અન્તર્ભાવ "../lexer/state.h"

પૂર્ણાંક _PyTokenizer_syntaxerror(સંરચના tok_state *tok, અચલ અક્ષર *format, ...);
/* Positive range columns are 1-based byte columns. A start column of -1
   derives the character column from the reporting cursor; an end column of
   -1 uses the start column. */
પૂર્ણાંક _PyTokenizer_syntaxerror_known_range(સંરચના tok_state *tok, પૂર્ણાંક col_offset, પૂર્ણાંક end_col_offset, અચલ અક્ષર *format, ...);
પૂર્ણાંક _PyTokenizer_syntaxerror_at(
    સંરચના tok_state *tok, અચલ અક્ષર *લીટી_start, Py_ssize_t cursor_offset,
    પૂર્ણાંક lineno, પૂર્ણાંક col_offset, પૂર્ણાંક end_col_offset, અચલ અક્ષર *format, ...);
પૂર્ણાંક _PyTokenizer_indenterror(સંરચના tok_state *tok);
પૂર્ણાંક _PyTokenizer_warn_invalid_escape_sequence(સંરચના tok_state *tok, પૂર્ણાંક first_invalid_escape_char);
પૂર્ણાંક _PyTokenizer_parser_warn(સંરચના tok_state *tok, PyObject *category, અચલ અક્ષર *format, ...);

શૂન્ય _PyTokenizer_raise_init_error(PyObject *filename);

પૂર્ણાંક _PyTokenizer_ensure_utf8(અચલ અક્ષર *લીટી, સંરચના tok_state *tok, પૂર્ણાંક lineno);

#ifdef Py_DEBUG
શૂન્ય _PyTokenizer_print_escape(FILE *f, અચલ અક્ષર *s, Py_ssize_t size);
#endif


#endif
