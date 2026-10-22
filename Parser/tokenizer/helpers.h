#ifndef _PY_TOKENIZER_HELPERS_H_
#define _PY_TOKENIZER_HELPERS_H_

#ଅନ୍ତର୍ଭାଵ "Python.h"

#ଅନ୍ତର୍ଭାଵ "../lexer/state.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyTokenizer_syntaxerror(ସଂରଚନା tok_state *tok, ଅଚଳ ଅକ୍ଷର *format, ...);
/* Positive range columns are 1-based byte columns. A start column of -1
   derives the character column from the reporting cursor; an end column of
   -1 uses the start column. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyTokenizer_syntaxerror_known_range(ସଂରଚନା tok_state *tok, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col_offset, ଅଚଳ ଅକ୍ଷର *format, ...);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyTokenizer_syntaxerror_at(
    ସଂରଚନା tok_state *tok, ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି_start, Py_ssize_t cursor_offset,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col_offset, ଅଚଳ ଅକ୍ଷର *format, ...);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyTokenizer_indenterror(ସଂରଚନା tok_state *tok);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyTokenizer_warn_invalid_escape_sequence(ସଂରଚନା tok_state *tok, ପୂର୍ଣ୍ଣସଂଖ୍ୟା first_invalid_escape_char);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyTokenizer_parser_warn(ସଂରଚନା tok_state *tok, PyObject *category, ଅଚଳ ଅକ୍ଷର *format, ...);

ଶୂନ୍ୟ _PyTokenizer_raise_init_error(PyObject *filename);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyTokenizer_ensure_utf8(ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି, ସଂରଚନା tok_state *tok, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno);

#ifdef Py_DEBUG
ଶୂନ୍ୟ _PyTokenizer_print_escape(FILE *f, ଅଚଳ ଅକ୍ଷର *s, Py_ssize_t size);
#endif


#endif
