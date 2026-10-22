#اگر_غیر_تعریف_شدہ _PY_TOKENIZER_HELPERS_H_
#تعریف_کرو _PY_TOKENIZER_HELPERS_H_

#شامل "Python.h"

#شامل "../lexer/state.h"

صحیح _PyTokenizer_syntaxerror(ساخت tok_state *tok, ثابت حرف *format, ...);
/* Positive range columns are 1-based byte columns. A start column of -1
   derives the character column from the reporting cursor; an end column of
   -1 uses the start column. */
صحیح Py_Tokenizer_syntaxerror_معلوم_range(ساخت tok_state *tok, صحیح ستون_انحراف, صحیح انتہا_ستون_انحراف, ثابت حرف *format, ...);
صحیح _PyTokenizer_syntaxerror_at(
    ساخت tok_state *tok, ثابت حرف *سطر_آغاز, Py_ssize_t cursor_انحراف,
    صحیح lineno, صحیح ستون_انحراف, صحیح انتہا_ستون_انحراف, ثابت حرف *format, ...);
صحیح _PyTokenizer_indenterror(ساخت tok_state *tok);
صحیح _PyTokenizer_warn_invalid_escape_sequence(ساخت tok_state *tok, صحیح پہلا_invalid_escape_char);
صحیح _PyTokenizer_parser_warn(ساخت tok_state *tok, PyObject *category, ثابت حرف *format, ...);

خالی _PyTokenizer_raise_init_error(PyObject *filename);

صحیح _PyTokenizer_ensure_utf8(ثابت حرف *سطر, ساخت tok_state *tok, صحیح lineno);

#اگر_تعریف_شدہ Py_DEBUG
خالی _PyTokenizer_print_escape(FILE *f, ثابت حرف *s, Py_ssize_t سائز);
#اگر_ختم


#اگر_ختم
