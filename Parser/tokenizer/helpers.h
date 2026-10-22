#ifndef _PY_TOKENIZER_HELPERS_H_
#define _PY_TOKENIZER_HELPERS_H_

#ಅನ್ತರ್ಭಾವ "Python.h"

#ಅನ್ತರ್ಭಾವ "../lexer/state.h"

ಪೂರ್ಣಾಙ್ಕ _PyTokenizer_syntaxerror(ಸಂರಚನಾ tok_state *tok, ಧ್ರುವ ವರ್ಣ *format, ...);
/* Positive range columns are 1-based byte columns. A start column of -1
   derives the character column from the reporting cursor; an end column of
   -1 uses the start column. */
ಪೂರ್ಣಾಙ್ಕ _PyTokenizer_syntaxerror_known_range(ಸಂರಚನಾ tok_state *tok, ಪೂರ್ಣಾಙ್ಕ col_offset, ಪೂರ್ಣಾಙ್ಕ end_col_offset, ಧ್ರುವ ವರ್ಣ *format, ...);
ಪೂರ್ಣಾಙ್ಕ _PyTokenizer_syntaxerror_at(
    ಸಂರಚನಾ tok_state *tok, ಧ್ರುವ ವರ್ಣ *ಸಾಲು_start, Py_ssize_t cursor_offset,
    ಪೂರ್ಣಾಙ್ಕ lineno, ಪೂರ್ಣಾಙ್ಕ col_offset, ಪೂರ್ಣಾಙ್ಕ end_col_offset, ಧ್ರುವ ವರ್ಣ *format, ...);
ಪೂರ್ಣಾಙ್ಕ _PyTokenizer_indenterror(ಸಂರಚನಾ tok_state *tok);
ಪೂರ್ಣಾಙ್ಕ _PyTokenizer_warn_invalid_escape_sequence(ಸಂರಚನಾ tok_state *tok, ಪೂರ್ಣಾಙ್ಕ first_invalid_escape_char);
ಪೂರ್ಣಾಙ್ಕ _PyTokenizer_parser_warn(ಸಂರಚನಾ tok_state *tok, PyObject *category, ಧ್ರುವ ವರ್ಣ *format, ...);

ಶೂನ್ಯ _PyTokenizer_raise_init_error(PyObject *filename);

ಪೂರ್ಣಾಙ್ಕ _PyTokenizer_ensure_utf8(ಧ್ರುವ ವರ್ಣ *ಸಾಲು, ಸಂರಚನಾ tok_state *tok, ಪೂರ್ಣಾಙ್ಕ lineno);

#ifdef Py_DEBUG
ಶೂನ್ಯ _PyTokenizer_print_escape(FILE *f, ಧ್ರುವ ವರ್ಣ *s, Py_ssize_t size);
#endif


#endif
