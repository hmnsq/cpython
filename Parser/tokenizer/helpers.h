#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _PY_TOKENIZER_HELPERS_H_
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _PY_TOKENIZER_HELPERS_H_

#ਅਨ੍ਤਰ੍ਭਾਵ "Python.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "../lexer/state.h"

ਪੂਰਨ _PyTokenizer_syntaxerror(ਬਣਤਰ tok_state *tok, ਅਚਲ ਅੱਖਰ *format, ...);
/* Positive range columns are 1-based byte columns. A start column of -1
   derives the character column from the reporting cursor; an end column of
   -1 uses the start column. */
ਪੂਰਨ Py_Tokenizer_syntaxerror_ਗਿਆਤ_range(ਬਣਤਰ tok_state *tok, ਪੂਰਨ ਖਾਨਾ_ਵਿੱਥ, ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, ਅਚਲ ਅੱਖਰ *format, ...);
ਪੂਰਨ _PyTokenizer_syntaxerror_at(
    ਬਣਤਰ tok_state *tok, ਅਚਲ ਅੱਖਰ *ਸਤਰ_ਸ਼ੁਰੂ, Py_ssize_t cursor_ਵਿੱਥ,
    ਪੂਰਨ lineno, ਪੂਰਨ ਖਾਨਾ_ਵਿੱਥ, ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, ਅਚਲ ਅੱਖਰ *format, ...);
ਪੂਰਨ _PyTokenizer_indenterror(ਬਣਤਰ tok_state *tok);
ਪੂਰਨ _PyTokenizer_warn_invalid_escape_sequence(ਬਣਤਰ tok_state *tok, ਪੂਰਨ ਪਹਿਲਾ_invalid_escape_char);
ਪੂਰਨ _PyTokenizer_parser_warn(ਬਣਤਰ tok_state *tok, PyObject *category, ਅਚਲ ਅੱਖਰ *format, ...);

ਖਾਲੀ _PyTokenizer_raise_init_error(PyObject *filename);

ਪੂਰਨ _PyTokenizer_ensure_utf8(ਅਚਲ ਅੱਖਰ *ਸਤਰ, ਬਣਤਰ tok_state *tok, ਪੂਰਨ lineno);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ Py_DEBUG
ਖਾਲੀ _PyTokenizer_print_escape(FILE *f, ਅਚਲ ਅੱਖਰ *s, Py_ssize_t ਮਾਪ);
#ਜੇ_ਅੰਤ


#ਜੇ_ਅੰਤ
