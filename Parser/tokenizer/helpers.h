#ifndef _PY_TOKENIZER_HELPERS_H_
#define _PY_TOKENIZER_HELPERS_H_

#অন্তর্ভাৱ "Python.h"

#অন্তর্ভাৱ "../lexer/state.h"

পূর্ণাঙ্ক _PyTokenizer_syntaxerror(সংরচনা tok_state *tok, ধ্রুৱ ৱর্ণ *format, ...);
/* Positive range columns are 1-based byte columns. A start column of -1
   derives the character column from the reporting cursor; an end column of
   -1 uses the start column. */
পূর্ণাঙ্ক _PyTokenizer_syntaxerror_known_range(সংরচনা tok_state *tok, পূর্ণাঙ্ক col_offset, পূর্ণাঙ্ক end_col_offset, ধ্রুৱ ৱর্ণ *format, ...);
পূর্ণাঙ্ক _PyTokenizer_syntaxerror_at(
    সংরচনা tok_state *tok, ধ্রুৱ ৱর্ণ *সাৰি_start, Py_ssize_t cursor_offset,
    পূর্ণাঙ্ক lineno, পূর্ণাঙ্ক col_offset, পূর্ণাঙ্ক end_col_offset, ধ্রুৱ ৱর্ণ *format, ...);
পূর্ণাঙ্ক _PyTokenizer_indenterror(সংরচনা tok_state *tok);
পূর্ণাঙ্ক _PyTokenizer_warn_invalid_escape_sequence(সংরচনা tok_state *tok, পূর্ণাঙ্ক first_invalid_escape_char);
পূর্ণাঙ্ক _PyTokenizer_parser_warn(সংরচনা tok_state *tok, PyObject *category, ধ্রুৱ ৱর্ণ *format, ...);

শূন্য _PyTokenizer_raise_init_error(PyObject *filename);

পূর্ণাঙ্ক _PyTokenizer_ensure_utf8(ধ্রুৱ ৱর্ণ *সাৰি, সংরচনা tok_state *tok, পূর্ণাঙ্ক lineno);

#ifdef Py_DEBUG
শূন্য _PyTokenizer_print_escape(FILE *f, ধ্রুৱ ৱর্ণ *s, Py_ssize_t size);
#endif


#endif
