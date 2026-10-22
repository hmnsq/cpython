#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _PY_TOKENIZER_HELPERS_H_
#ವ್ಯಾಖ್ಯಾನಿಸು _PY_TOKENIZER_HELPERS_H_

#ಅನ್ತರ್ಭಾವ "Python.h"

#ಅನ್ತರ್ಭಾವ "../lexer/state.h"

ಪೂರ್ಣಾಂಕ _PyTokenizer_syntaxerror(ರಚನೆ tok_state *tok, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...);
/* Positive range columns are 1-based byte columns. A start column of -1
   derives the character column from the reporting cursor; an end column of
   -1 uses the start column. */
ಪೂರ್ಣಾಂಕ Py_Tokenizer_syntaxerror_ತಿಳಿದ_range(ರಚನೆ tok_state *tok, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...);
ಪೂರ್ಣಾಂಕ _PyTokenizer_syntaxerror_at(
    ರಚನೆ tok_state *tok, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು_ಆರಂಭ, Py_ssize_t cursor_ಸ್ಥಾನಾಂತರ,
    ಪೂರ್ಣಾಂಕ lineno, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...);
ಪೂರ್ಣಾಂಕ _PyTokenizer_indenterror(ರಚನೆ tok_state *tok);
ಪೂರ್ಣಾಂಕ _PyTokenizer_warn_invalid_escape_sequence(ರಚನೆ tok_state *tok, ಪೂರ್ಣಾಂಕ ಮೊದಲ_invalid_escape_char);
ಪೂರ್ಣಾಂಕ _PyTokenizer_parser_warn(ರಚನೆ tok_state *tok, PyObject *category, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...);

ಶೂನ್ಯ _PyTokenizer_raise_init_error(PyObject *filename);

ಪೂರ್ಣಾಂಕ _PyTokenizer_ensure_utf8(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು, ರಚನೆ tok_state *tok, ಪೂರ್ಣಾಂಕ lineno);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ Py_DEBUG
ಶೂನ್ಯ _PyTokenizer_print_escape(FILE *f, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, Py_ssize_t ಗಾತ್ರ_2);
#ಆದರೆ_ಅಂತ್ಯ


#ಆದರೆ_ಅಂತ್ಯ
