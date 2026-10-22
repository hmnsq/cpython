#ifndef _PY_TOKENIZER_HELPERS_H_
#define _PY_TOKENIZER_HELPERS_H_

#അന്തര്ഭാവ "Python.h"

#അന്തര്ഭാവ "../lexer/state.h"

പൂർണ്ണസംഖ്യ _PyTokenizer_syntaxerror(ഘടന tok_state *tok, സ്ഥിരം അക്ഷരം *format, ...);
/* Positive range columns are 1-based byte columns. A start column of -1
   derives the character column from the reporting cursor; an end column of
   -1 uses the start column. */
പൂർണ്ണസംഖ്യ _PyTokenizer_syntaxerror_known_range(ഘടന tok_state *tok, പൂർണ്ണസംഖ്യ col_offset, പൂർണ്ണസംഖ്യ end_col_offset, സ്ഥിരം അക്ഷരം *format, ...);
പൂർണ്ണസംഖ്യ _PyTokenizer_syntaxerror_at(
    ഘടന tok_state *tok, സ്ഥിരം അക്ഷരം *വരി_start, Py_ssize_t cursor_offset,
    പൂർണ്ണസംഖ്യ lineno, പൂർണ്ണസംഖ്യ col_offset, പൂർണ്ണസംഖ്യ end_col_offset, സ്ഥിരം അക്ഷരം *format, ...);
പൂർണ്ണസംഖ്യ _PyTokenizer_indenterror(ഘടന tok_state *tok);
പൂർണ്ണസംഖ്യ _PyTokenizer_warn_invalid_escape_sequence(ഘടന tok_state *tok, പൂർണ്ണസംഖ്യ first_invalid_escape_char);
പൂർണ്ണസംഖ്യ _PyTokenizer_parser_warn(ഘടന tok_state *tok, PyObject *category, സ്ഥിരം അക്ഷരം *format, ...);

ശൂന്യം _PyTokenizer_raise_init_error(PyObject *filename);

പൂർണ്ണസംഖ്യ _PyTokenizer_ensure_utf8(സ്ഥിരം അക്ഷരം *വരി, ഘടന tok_state *tok, പൂർണ്ണസംഖ്യ lineno);

#ifdef Py_DEBUG
ശൂന്യം _PyTokenizer_print_escape(FILE *f, സ്ഥിരം അക്ഷരം *s, Py_ssize_t size);
#endif


#endif
