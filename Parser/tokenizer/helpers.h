#ifndef _PY_TOKENIZER_HELPERS_H_
#define _PY_TOKENIZER_HELPERS_H_

#अन्तर्भाव "Python.h"

#अन्तर्भाव "../lexer/state.h"

पूर्णाङ्क _PyTokenizer_syntaxerror(संरचना tok_state *tok, ध्रुव वर्ण *format, ...);
/* Positive range columns are 1-based byte columns. A start column of -1
   derives the character column from the reporting cursor; an end column of
   -1 uses the start column. */
पूर्णाङ्क _PyTokenizer_syntaxerror_known_range(संरचना tok_state *tok, पूर्णाङ्क col_offset, पूर्णाङ्क end_col_offset, ध्रुव वर्ण *format, ...);
पूर्णाङ्क _PyTokenizer_syntaxerror_at(
    संरचना tok_state *tok, ध्रुव वर्ण *पङ्क्तिः_start, Py_ssize_t cursor_offset,
    पूर्णाङ्क lineno, पूर्णाङ्क col_offset, पूर्णाङ्क end_col_offset, ध्रुव वर्ण *format, ...);
पूर्णाङ्क _PyTokenizer_indenterror(संरचना tok_state *tok);
पूर्णाङ्क _PyTokenizer_warn_invalid_escape_sequence(संरचना tok_state *tok, पूर्णाङ्क first_invalid_escape_char);
पूर्णाङ्क _PyTokenizer_parser_warn(संरचना tok_state *tok, PyObject *category, ध्रुव वर्ण *format, ...);

शून्य _PyTokenizer_raise_init_error(PyObject *filename);

पूर्णाङ्क _PyTokenizer_ensure_utf8(ध्रुव वर्ण *पङ्क्तिः, संरचना tok_state *tok, पूर्णाङ्क lineno);

#ifdef Py_DEBUG
शून्य _PyTokenizer_print_escape(FILE *f, ध्रुव वर्ण *s, Py_ssize_t size);
#endif


#endif
