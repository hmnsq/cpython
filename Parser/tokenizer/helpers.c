#ಅನ್ತರ್ಭಾವ "Python.h"
#ಅನ್ತರ್ಭಾವ "errcode.h"
#ಅನ್ತರ್ಭಾವ "pycore_runtime.h"       // _Py_ID()
#ಅನ್ತರ್ಭಾವ "pycore_token.h"
#ಅನ್ತರ್ಭಾವ "pycore_tuple.h"         // _PyTuple_FromPair

#ಅನ್ತರ್ಭಾವ "../lexer/state.h"


/* ############## ERRORS ############## */

/* Convert a 1-based column in bytes into a 1-based column in characters.
   The line is UTF-8 encoded, so it is enough to skip continuation bytes. */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
byte_ಲಂಬಸಾಲು_ಗಮ್ಯ_char_ಲಂಬಸಾಲು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು, ಪೂರ್ಣಾಂಕ byte_ಲಂಬಸಾಲು)
{
    ಪೂರ್ಣಾಂಕ char_ಲಂಬಸಾಲು = 1;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < byte_ಲಂಬಸಾಲು - 1; i++) {
        ಆದರೆ ((ಸಾಲು[i] & 0xC0) != 0x80) {
            char_ಲಂಬಸಾಲು++;
        }
    }
    ಹಿಂತಿರುಗಿ char_ಲಂಬಸಾಲು;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
_syntaxerror_range(ರಚನೆ tok_state *tok, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು,
                   Py_ssize_t cursor, ಪೂರ್ಣಾಂಕ lineno, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format,
                   ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                   va_list vargs)
{
    // In release builds, we don't want to overwrite a previous error, but in debug builds we
    // want to fail if we are not doing it so we can fix it.
    ಪ್ರತಿಜ್ಞಾ(tok->ಮುಗಿದ != E_ERROR);
    ಆದರೆ (tok->ಮುಗಿದ == E_ERROR) {
        ಹಿಂತಿರುಗಿ ERRORTOKEN;
    }
    PyObject *errmsg, *errtext, *ವಾದಗಳು;
    errmsg = PyUnicode_FromFormatV(format, vargs);
    ಆದರೆ (!errmsg) {
        ಹೋಗು error;
    }

    errtext = PyUnicode_DecodeUTF8(ಸಾಲು, cursor,
                                   "replace");
    ಆದರೆ (!errtext) {
        ಹೋಗು error;
    }

    ಆದರೆ (ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ == -1) {
        ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = (ಪೂರ್ಣಾಂಕ)Py_Unicode_GET_ಉದ್ದಳತೆ(errtext);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ > 0) {
        ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = byte_ಲಂಬಸಾಲು_ಗಮ್ಯ_char_ಲಂಬಸಾಲು(ಸಾಲು, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ);
    }
    ಆದರೆ (ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ == -1) {
        ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ > 0) {
        ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = byte_ಲಂಬಸಾಲು_ಗಮ್ಯ_char_ಲಂಬಸಾಲು(ಸಾಲು, ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ);
    }

    Py_ssize_t ಸಾಲು_len = strcspn(ಸಾಲು, "\n");
    ಆದರೆ (ಸಾಲು_len != cursor) {
        Py_DECREF(errtext);
        errtext = PyUnicode_DecodeUTF8(ಸಾಲು, ಸಾಲು_len,
                                       "replace");
    }
    ಆದರೆ (!errtext) {
        ಹೋಗು error;
    }

    ವಾದಗಳು = Py_Build_ಮೌಲ್ಯ("(O(OiiNii))", errmsg,
                         tok->filename ? tok->filename : Py_None,
                         lineno, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, errtext,
                         lineno, ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ);
    ಆದರೆ (ವಾದಗಳು) {
        Py_Err_ಗಣ_Object(PyExc_SyntaxError, ವಾದಗಳು);
        Py_DECREF(ವಾದಗಳು);
    }

error:
    Py_XDECREF(errmsg);
    tok->ಮುಗಿದ = E_ERROR;
    ಹಿಂತಿರುಗಿ ERRORTOKEN;
}

ಪೂರ್ಣಾಂಕ
_PyTokenizer_syntaxerror(ರಚನೆ tok_state *tok, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...)
{
    // These errors are cleaned on startup. Todo: Fix it.
    va_list vargs;
    va_ಆರಂಭ(vargs, format);
    ಪೂರ್ಣಾಂಕ ret = _syntaxerror_range(
        tok, Py_Tok_ಮೂಲ_ಸೂಚಕ(&tok->ಮೂಲ, tok->ಸಾಲು_ಆರಂಭ),
        tok->cur - tok->ಸಾಲು_ಆರಂಭ, tok->lineno, format, -1, -1, vargs);
    va_ಕೊನೆ(vargs);
    ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ
Py_Tokenizer_syntaxerror_ತಿಳಿದ_range(ರಚನೆ tok_state *tok,
                        ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...)
{
    va_list vargs;
    va_ಆರಂಭ(vargs, format);
    ಪೂರ್ಣಾಂಕ ret = _syntaxerror_range(
        tok, Py_Tok_ಮೂಲ_ಸೂಚಕ(&tok->ಮೂಲ, tok->ಸಾಲು_ಆರಂಭ),
        tok->cur - tok->ಸಾಲು_ಆರಂಭ, tok->lineno,
        format, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, vargs);
    va_ಕೊನೆ(vargs);
    ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ
_PyTokenizer_syntaxerror_at(ರಚನೆ tok_state *tok, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು,
                             Py_ssize_t cursor, ಪೂರ್ಣಾಂಕ lineno,
                             ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...)
{
    va_list vargs;
    va_ಆರಂಭ(vargs, format);
    ಪೂರ್ಣಾಂಕ ret = _syntaxerror_range(tok, ಸಾಲು, cursor, lineno, format,
                                ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, vargs);
    va_ಕೊನೆ(vargs);
    ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ
_PyTokenizer_indenterror(ರಚನೆ tok_state *tok)
{
    tok->ಮುಗಿದ = E_TABSPACE;
    tok->cur = tok->inp;
    ಹಿಂತಿರುಗಿ ERRORTOKEN;
}

ಪೂರ್ಣಾಂಕ
_PyTokenizer_warn_invalid_escape_sequence(ರಚನೆ tok_state *tok, ಪೂರ್ಣಾಂಕ ಮೊದಲ_invalid_escape_char)
{
    PyObject *ಸಂದೇಶ = PyUnicode_FromFormat(
        "\"\\%c\" is an invalid escape sequence. "
        "Such sequences will not work in the future. "
        "Did you mean \"\\\\%c\"? A raw string is also an option.",
        (ಅಕ್ಷರ) ಮೊದಲ_invalid_escape_char,
        (ಅಕ್ಷರ) ಮೊದಲ_invalid_escape_char
    );

    ಆದರೆ (ಸಂದೇಶ == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ (PyErr_WarnExplicitObject(PyExc_SyntaxWarning, ಸಂದೇಶ, tok->filename,
                                 tok->lineno, tok->module, ಖಾಲಿ) < 0) {
        Py_DECREF(ಸಂದೇಶ);

        ಆದರೆ (PyErr_ExceptionMatches(PyExc_SyntaxWarning)) {
            /* Replace the SyntaxWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            ಹಿಂತಿರುಗಿ _PyTokenizer_syntaxerror(tok,
                "\"\\%c\" is an invalid escape sequence. "
                "Did you mean \"\\\\%c\"? A raw string is also an option.",
                (ಅಕ್ಷರ) ಮೊದಲ_invalid_escape_char,
                (ಅಕ್ಷರ) ಮೊದಲ_invalid_escape_char);
        }

        ಹಿಂತಿರುಗಿ -1;
    }

    Py_DECREF(ಸಂದೇಶ);
    ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ
_PyTokenizer_raise_init_error(PyObject *filename)
{
    ಆದರೆ (!(PyErr_ExceptionMatches(PyExc_LookupError)
          || PyErr_ExceptionMatches(PyExc_SyntaxError)
          || PyErr_ExceptionMatches(Py_Exc_ಮೌಲ್ಯ_Error)
          || PyErr_ExceptionMatches(PyExc_UnicodeDecodeError))) {
        ಹಿಂತಿರುಗಿ;
    }
    PyObject *errstr = ಖಾಲಿ;
    PyObject *tuple = ಖಾಲಿ;
    PyObject *type;
    PyObject *ಮೌಲ್ಯ;
    PyObject *tback;
    PyErr_Fetch(&type, &ಮೌಲ್ಯ, &tback);
    ಆದರೆ (PyErr_GivenExceptionMatches(ಮೌಲ್ಯ, PyExc_SyntaxError)) {
        ಆದರೆ (Py_Object_ಗಣ_Attr(ಮೌಲ್ಯ, &_Py_ID(filename), filename)) {
            ಹೋಗು error;
        }
        PyErr_Restore(type, ಮೌಲ್ಯ, tback);
        ಹಿಂತಿರುಗಿ;
    }
    errstr = PyObject_Str(ಮೌಲ್ಯ);
    ಆದರೆ (!errstr) {
        ಹೋಗು error;
    }

    PyObject *tmp = Py_Build_ಮೌಲ್ಯ("(OiiO)", filename, 0, -1, Py_None);
    ಆದರೆ (!tmp) {
        ಹೋಗು error;
    }

    tuple = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    ಆದರೆ (!tuple) {
        ಹೋಗು error;
    }
    Py_Err_ಗಣ_Object(PyExc_SyntaxError, tuple);

error:
    Py_XDECREF(type);
    Py_XDECREF(ಮೌಲ್ಯ);
    Py_XDECREF(tback);
    Py_XDECREF(errstr);
    Py_XDECREF(tuple);
}

ಪೂರ್ಣಾಂಕ
_PyTokenizer_parser_warn(ರಚನೆ tok_state *tok, PyObject *category, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...)
{
    PyObject *errmsg;
    va_list vargs;
    va_ಆರಂಭ(vargs, format);
    errmsg = PyUnicode_FromFormatV(format, vargs);
    va_ಕೊನೆ(vargs);
    ಆದರೆ (!errmsg) {
        ಹೋಗು error;
    }

    ಆದರೆ (PyErr_WarnExplicitObject(category, errmsg, tok->filename,
                                 tok->lineno, tok->module, ಖಾಲಿ) < 0) {
        ಆದರೆ (PyErr_ExceptionMatches(category)) {
            /* Replace the DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();
            _PyTokenizer_syntaxerror(tok, "%U", errmsg);
        }
        ಹೋಗು error;
    }
    Py_DECREF(errmsg);
    ಹಿಂತಿರುಗಿ 0;

error:
    Py_XDECREF(errmsg);
    tok->ಮುಗಿದ = E_ERROR;
    ಹಿಂತಿರುಗಿ -1;
}


/* Check whether the characters at s start a valid
   UTF-8 sequence. Return the number of characters forming
   the sequence if yes, 0 if not.  The special cases match
   those in stringlib/codecs.h:utf8_decode.
*/
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
valid_utf8(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ* s)
{
    ಪೂರ್ಣಾಂಕ expected = 0;
    ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ;
    ಆದರೆ (*s < 0x80) {
        /* single-byte code */
        ಹಿಂತಿರುಗಿ 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s < 0xE0) {
        /* \xC2\x80-\xDF\xBF -- 0080-07FF */
        ಆದರೆ (*s < 0xC2) {
            /* invalid sequence
               \x80-\xBF -- continuation byte
               \xC0-\xC1 -- fake 0000-007F */
            ಹಿಂತಿರುಗಿ 0;
        }
        expected = 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s < 0xF0) {
        /* \xE0\xA0\x80-\xEF\xBF\xBF -- 0800-FFFF */
        ಆದರೆ (*s == 0xE0 && *(s + 1) < 0xA0) {
            /* invalid sequence
               \xE0\x80\x80-\xE0\x9F\xBF -- fake 0000-0800 */
            ಹಿಂತಿರುಗಿ 0;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s == 0xED && *(s + 1) >= 0xA0) {
            /* Decoding UTF-8 sequences in range \xED\xA0\x80-\xED\xBF\xBF
               will result in surrogates in range D800-DFFF. Surrogates are
               not valid UTF-8 so they are rejected.
               See https://www.unicode.org/versions/Unicode5.2.0/ch03.pdf
               (table 3-7) and http://www.rfc-editor.org/rfc/rfc3629.txt */
            ಹಿಂತಿರುಗಿ 0;
        }
        expected = 2;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s < 0xF5) {
        /* \xF0\x90\x80\x80-\xF4\x8F\xBF\xBF -- 10000-10FFFF */
        ಆದರೆ (*(s + 1) < 0x90 ? *s == 0xF0 : *s == 0xF4) {
            /* invalid sequence -- one of:
               \xF0\x80\x80\x80-\xF0\x8F\xBF\xBF -- fake 0000-FFFF
               \xF4\x90\x80\x80- -- 110000- overflow */
            ಹಿಂತಿರುಗಿ 0;
        }
        expected = 3;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        /* invalid start byte */
        ಹಿಂತಿರುಗಿ 0;
    }
    ಉದ್ದಳತೆ = expected + 1;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i <= expected; i++) {
        ಆದರೆ (s[i] < 0x80 || s[i] >= 0xC0) {
            ಹಿಂತಿರುಗಿ 0;
        }
    }
    ಹಿಂತಿರುಗಿ ಉದ್ದಳತೆ;
}

ಪೂರ್ಣಾಂಕ
_PyTokenizer_ensure_utf8(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು, ರಚನೆ tok_state *tok, ಪೂರ್ಣಾಂಕ lineno)
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *badchar = ಖಾಲಿ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *c;
    ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು_ಆರಂಭ = ಸಾಲು;
    ಗಾಗಿ (c = ಸಾಲು; *c; c += ಉದ್ದಳತೆ) {
        ಆದರೆ (!(ಉದ್ದಳತೆ = valid_utf8((ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)c))) {
            badchar = c;
            ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ (*c == '\n') {
            lineno++;
            ಸಾಲು_ಆರಂಭ = c + 1;
        }
    }
    ಆದರೆ (badchar) {
        _PyTokenizer_syntaxerror_at(
                tok, ಸಾಲು_ಆರಂಭ, badchar - ಸಾಲು_ಆರಂಭ + 1, lineno,
                -1, -1,
                "Non-UTF-8 code starting with '\\x%.2x'"
                "%s%V on line %i, "
                "but no encoding declared; "
                "see https://peps.python.org/pep-0263/ for details",
                (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)*badchar,
                tok->filename ? " in file " : "", tok->filename, "",
                lineno);
        ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ 1;
}


/* ############## DEBUGGING STUFF ############## */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ Py_DEBUG
ಶೂನ್ಯ
_PyTokenizer_print_escape(FILE *f, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, Py_ssize_t ಗಾತ್ರ_2)
{
    ಆದರೆ (s == ಖಾಲಿ) {
        fputs("NULL", f);
        ಹಿಂತಿರುಗಿ;
    }
    putc('"', f);
    ಆಗ (ಗಾತ್ರ_2-- > 0) {
        ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c = *s++;
        ಆಯ್ಕೆ (c) {
            ಸ್ಥಿತಿ '\n': fputs("\\n", f); ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ '\r': fputs("\\r", f); ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ '\t': fputs("\\t", f); ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ '\f': fputs("\\f", f); ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ '\'': fputs("\\'", f); ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ '"': fputs("\\\"", f); ನಿಲ್ಲಿಸು;
            ಪೂರ್ವನಿಯೋಜಿತ:
                ಆದರೆ (0x20 <= c && c <= 0x7f)
                    putc(c, f);
                ಇಲ್ಲದಿದ್ದರೆ
                    fprintf(f, "\\x%02x", c);
        }
    }
    putc('"', f);
}
#ಆದರೆ_ಅಂತ್ಯ
