#ifndef _PY_TOKENIZER_HELPERS_H_
#define _PY_TOKENIZER_HELPERS_H_

#অন্তর্ভাব "Python.h"

#অন্তর্ভাব "../lexer/state.h"

পূর্ণসংখ্যা _PyTokenizer_syntaxerror(কাঠামো tok_state *tok, ধ্রুবক অক্ষর *format, ...);
/* Positive range columns are 1-based byte columns. A start column of -1
   derives the character column from the reporting cursor; an end column of
   -1 uses the start column. */
পূর্ণসংখ্যা _PyTokenizer_syntaxerror_known_range(কাঠামো tok_state *tok, পূর্ণসংখ্যা col_offset, পূর্ণসংখ্যা end_col_offset, ধ্রুবক অক্ষর *format, ...);
পূর্ণসংখ্যা _PyTokenizer_syntaxerror_at(
    কাঠামো tok_state *tok, ধ্রুবক অক্ষর *সারি_start, Py_ssize_t cursor_offset,
    পূর্ণসংখ্যা lineno, পূর্ণসংখ্যা col_offset, পূর্ণসংখ্যা end_col_offset, ধ্রুবক অক্ষর *format, ...);
পূর্ণসংখ্যা _PyTokenizer_indenterror(কাঠামো tok_state *tok);
পূর্ণসংখ্যা _PyTokenizer_warn_invalid_escape_sequence(কাঠামো tok_state *tok, পূর্ণসংখ্যা first_invalid_escape_char);
পূর্ণসংখ্যা _PyTokenizer_parser_warn(কাঠামো tok_state *tok, PyObject *category, ধ্রুবক অক্ষর *format, ...);

শূন্য _PyTokenizer_raise_init_error(PyObject *filename);

পূর্ণসংখ্যা _PyTokenizer_ensure_utf8(ধ্রুবক অক্ষর *সারি, কাঠামো tok_state *tok, পূর্ণসংখ্যা lineno);

#ifdef Py_DEBUG
শূন্য _PyTokenizer_print_escape(FILE *f, ধ্রুবক অক্ষর *s, Py_ssize_t আকার_2);
#endif


#endif
