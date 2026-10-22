#ಅನ್ತರ್ಭಾವ "Python.h"
#ಅನ್ತರ್ಭಾವ "errcode.h"
#ಅನ್ತರ್ಭಾವ "pycore_runtime.h"       // _Py_ID()
#ಅನ್ತರ್ಭಾವ "pycore_token.h"
#ಅನ್ತರ್ಭಾವ "pycore_tuple.h"         // _PyTuple_FromPair

#ಅನ್ತರ್ಭಾವ "../lexer/state.h"


/* ############## ERRORS ############## */

/* Convert a 1-based column in bytes into a 1-based column in characters.
   The line is UTF-8 encoded, so it is enough to skip continuation bytes. */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
byte_col_to_char_col(ಧ್ರುವ ವರ್ಣ *ಸಾಲು, ಪೂರ್ಣಾಙ್ಕ byte_col)
{
    ಪೂರ್ಣಾಙ್ಕ char_col = 1;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < byte_col - 1; i++) {
        ಯದಿ ((ಸಾಲು[i] & 0xC0) != 0x80) {
            char_col++;
        }
    }
    ಪ್ರತ್ಯಾಗಮ char_col;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
_syntaxerror_range(ಸಂರಚನಾ tok_state *tok, ಧ್ರುವ ವರ್ಣ *ಸಾಲು,
                   Py_ssize_t cursor, ಪೂರ್ಣಾಙ್ಕ lineno, ಧ್ರುವ ವರ್ಣ *format,
                   ಪೂರ್ಣಾಙ್ಕ col_offset, ಪೂರ್ಣಾಙ್ಕ end_col_offset,
                   va_list vargs)
{
    // In release builds, we don't want to overwrite a previous error, but in debug builds we
    // want to fail if we are not doing it so we can fix it.
    ಪ್ರತಿಜ್ಞಾ(tok->done != E_ERROR);
    ಯದಿ (tok->done == E_ERROR) {
        ಪ್ರತ್ಯಾಗಮ ERRORTOKEN;
    }
    PyObject *errmsg, *errtext, *args;
    errmsg = PyUnicode_FromFormatV(format, vargs);
    ಯದಿ (!errmsg) {
        ಗಚ್ಛ error;
    }

    errtext = PyUnicode_DecodeUTF8(ಸಾಲು, cursor,
                                   "replace");
    ಯದಿ (!errtext) {
        ಗಚ್ಛ error;
    }

    ಯದಿ (col_offset == -1) {
        col_offset = (ಪೂರ್ಣಾಙ್ಕ)PyUnicode_GET_LENGTH(errtext);
    }
    ಅನ್ಯಥಾ ಯದಿ (col_offset > 0) {
        col_offset = byte_col_to_char_col(ಸಾಲು, col_offset);
    }
    ಯದಿ (end_col_offset == -1) {
        end_col_offset = col_offset;
    }
    ಅನ್ಯಥಾ ಯದಿ (end_col_offset > 0) {
        end_col_offset = byte_col_to_char_col(ಸಾಲು, end_col_offset);
    }

    Py_ssize_t ಸಾಲು_len = strcspn(ಸಾಲು, "\n");
    ಯದಿ (ಸಾಲು_len != cursor) {
        Py_DECREF(errtext);
        errtext = PyUnicode_DecodeUTF8(ಸಾಲು, ಸಾಲು_len,
                                       "replace");
    }
    ಯದಿ (!errtext) {
        ಗಚ್ಛ error;
    }

    args = Py_Build_ಮೌಲ್ಯ("(O(OiiNii))", errmsg,
                         tok->filename ? tok->filename : Py_None,
                         lineno, col_offset, errtext,
                         lineno, end_col_offset);
    ಯದಿ (args) {
        PyErr_SetObject(PyExc_SyntaxError, args);
        Py_DECREF(args);
    }

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    ಪ್ರತ್ಯಾಗಮ ERRORTOKEN;
}

ಪೂರ್ಣಾಙ್ಕ
_PyTokenizer_syntaxerror(ಸಂರಚನಾ tok_state *tok, ಧ್ರುವ ವರ್ಣ *format, ...)
{
    // These errors are cleaned on startup. Todo: Fix it.
    va_list vargs;
    va_start(vargs, format);
    ಪೂರ್ಣಾಙ್ಕ ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->ಸಾಲು_start),
        tok->cur - tok->ಸಾಲು_start, tok->lineno, format, -1, -1, vargs);
    va_end(vargs);
    ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ
_PyTokenizer_syntaxerror_known_range(ಸಂರಚನಾ tok_state *tok,
                        ಪೂರ್ಣಾಙ್ಕ col_offset, ಪೂರ್ಣಾಙ್ಕ end_col_offset,
                        ಧ್ರುವ ವರ್ಣ *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    ಪೂರ್ಣಾಙ್ಕ ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->ಸಾಲು_start),
        tok->cur - tok->ಸಾಲು_start, tok->lineno,
        format, col_offset, end_col_offset, vargs);
    va_end(vargs);
    ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ
_PyTokenizer_syntaxerror_at(ಸಂರಚನಾ tok_state *tok, ಧ್ರುವ ವರ್ಣ *ಸಾಲು,
                             Py_ssize_t cursor, ಪೂರ್ಣಾಙ್ಕ lineno,
                             ಪೂರ್ಣಾಙ್ಕ col_offset, ಪೂರ್ಣಾಙ್ಕ end_col_offset,
                             ಧ್ರುವ ವರ್ಣ *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    ಪೂರ್ಣಾಙ್ಕ ret = _syntaxerror_range(tok, ಸಾಲು, cursor, lineno, format,
                                col_offset, end_col_offset, vargs);
    va_end(vargs);
    ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ
_PyTokenizer_indenterror(ಸಂರಚನಾ tok_state *tok)
{
    tok->done = E_TABSPACE;
    tok->cur = tok->inp;
    ಪ್ರತ್ಯಾಗಮ ERRORTOKEN;
}

ಪೂರ್ಣಾಙ್ಕ
_PyTokenizer_warn_invalid_escape_sequence(ಸಂರಚನಾ tok_state *tok, ಪೂರ್ಣಾಙ್ಕ first_invalid_escape_char)
{
    PyObject *ಸಂದೇಶ = PyUnicode_FromFormat(
        "\"\\%c\" is an invalid escape sequence. "
        "Such sequences will not work in the future. "
        "Did you mean \"\\\\%c\"? A raw string is also an option.",
        (ವರ್ಣ) first_invalid_escape_char,
        (ವರ್ಣ) first_invalid_escape_char
    );

    ಯದಿ (ಸಂದೇಶ == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಯದಿ (PyErr_WarnExplicitObject(PyExc_SyntaxWarning, ಸಂದೇಶ, tok->filename,
                                 tok->lineno, tok->module, ರಿಕ್ತ) < 0) {
        Py_DECREF(ಸಂದೇಶ);

        ಯದಿ (PyErr_ExceptionMatches(PyExc_SyntaxWarning)) {
            /* Replace the SyntaxWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            ಪ್ರತ್ಯಾಗಮ _PyTokenizer_syntaxerror(tok,
                "\"\\%c\" is an invalid escape sequence. "
                "Did you mean \"\\\\%c\"? A raw string is also an option.",
                (ವರ್ಣ) first_invalid_escape_char,
                (ವರ್ಣ) first_invalid_escape_char);
        }

        ಪ್ರತ್ಯಾಗಮ -1;
    }

    Py_DECREF(ಸಂದೇಶ);
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ
_PyTokenizer_raise_init_error(PyObject *filename)
{
    ಯದಿ (!(PyErr_ExceptionMatches(PyExc_LookupError)
          || PyErr_ExceptionMatches(PyExc_SyntaxError)
          || PyErr_ExceptionMatches(Py_Exc_ಮೌಲ್ಯ_Error)
          || PyErr_ExceptionMatches(PyExc_UnicodeDecodeError))) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    PyObject *errstr = ರಿಕ್ತ;
    PyObject *tuple = ರಿಕ್ತ;
    PyObject *type;
    PyObject *ಮೌಲ್ಯ;
    PyObject *tback;
    PyErr_Fetch(&type, &ಮೌಲ್ಯ, &tback);
    ಯದಿ (PyErr_GivenExceptionMatches(ಮೌಲ್ಯ, PyExc_SyntaxError)) {
        ಯದಿ (PyObject_SetAttr(ಮೌಲ್ಯ, &_Py_ID(filename), filename)) {
            ಗಚ್ಛ error;
        }
        PyErr_Restore(type, ಮೌಲ್ಯ, tback);
        ಪ್ರತ್ಯಾಗಮ;
    }
    errstr = PyObject_Str(ಮೌಲ್ಯ);
    ಯದಿ (!errstr) {
        ಗಚ್ಛ error;
    }

    PyObject *tmp = Py_Build_ಮೌಲ್ಯ("(OiiO)", filename, 0, -1, Py_None);
    ಯದಿ (!tmp) {
        ಗಚ್ಛ error;
    }

    tuple = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    ಯದಿ (!tuple) {
        ಗಚ್ಛ error;
    }
    PyErr_SetObject(PyExc_SyntaxError, tuple);

error:
    Py_XDECREF(type);
    Py_XDECREF(ಮೌಲ್ಯ);
    Py_XDECREF(tback);
    Py_XDECREF(errstr);
    Py_XDECREF(tuple);
}

ಪೂರ್ಣಾಙ್ಕ
_PyTokenizer_parser_warn(ಸಂರಚನಾ tok_state *tok, PyObject *category, ಧ್ರುವ ವರ್ಣ *format, ...)
{
    PyObject *errmsg;
    va_list vargs;
    va_start(vargs, format);
    errmsg = PyUnicode_FromFormatV(format, vargs);
    va_end(vargs);
    ಯದಿ (!errmsg) {
        ಗಚ್ಛ error;
    }

    ಯದಿ (PyErr_WarnExplicitObject(category, errmsg, tok->filename,
                                 tok->lineno, tok->module, ರಿಕ್ತ) < 0) {
        ಯದಿ (PyErr_ExceptionMatches(category)) {
            /* Replace the DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();
            _PyTokenizer_syntaxerror(tok, "%U", errmsg);
        }
        ಗಚ್ಛ error;
    }
    Py_DECREF(errmsg);
    ಪ್ರತ್ಯಾಗಮ 0;

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    ಪ್ರತ್ಯಾಗಮ -1;
}


/* Check whether the characters at s start a valid
   UTF-8 sequence. Return the number of characters forming
   the sequence if yes, 0 if not.  The special cases match
   those in stringlib/codecs.h:utf8_decode.
*/
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
valid_utf8(ಧ್ರುವ unsigned ವರ್ಣ* s)
{
    ಪೂರ್ಣಾಙ್ಕ expected = 0;
    ಪೂರ್ಣಾಙ್ಕ length;
    ಯದಿ (*s < 0x80) {
        /* single-byte code */
        ಪ್ರತ್ಯಾಗಮ 1;
    }
    ಅನ್ಯಥಾ ಯದಿ (*s < 0xE0) {
        /* \xC2\x80-\xDF\xBF -- 0080-07FF */
        ಯದಿ (*s < 0xC2) {
            /* invalid sequence
               \x80-\xBF -- continuation byte
               \xC0-\xC1 -- fake 0000-007F */
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        expected = 1;
    }
    ಅನ್ಯಥಾ ಯದಿ (*s < 0xF0) {
        /* \xE0\xA0\x80-\xEF\xBF\xBF -- 0800-FFFF */
        ಯದಿ (*s == 0xE0 && *(s + 1) < 0xA0) {
            /* invalid sequence
               \xE0\x80\x80-\xE0\x9F\xBF -- fake 0000-0800 */
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        ಅನ್ಯಥಾ ಯದಿ (*s == 0xED && *(s + 1) >= 0xA0) {
            /* Decoding UTF-8 sequences in range \xED\xA0\x80-\xED\xBF\xBF
               will result in surrogates in range D800-DFFF. Surrogates are
               not valid UTF-8 so they are rejected.
               See https://www.unicode.org/versions/Unicode5.2.0/ch03.pdf
               (table 3-7) and http://www.rfc-editor.org/rfc/rfc3629.txt */
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        expected = 2;
    }
    ಅನ್ಯಥಾ ಯದಿ (*s < 0xF5) {
        /* \xF0\x90\x80\x80-\xF4\x8F\xBF\xBF -- 10000-10FFFF */
        ಯದಿ (*(s + 1) < 0x90 ? *s == 0xF0 : *s == 0xF4) {
            /* invalid sequence -- one of:
               \xF0\x80\x80\x80-\xF0\x8F\xBF\xBF -- fake 0000-FFFF
               \xF4\x90\x80\x80- -- 110000- overflow */
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        expected = 3;
    }
    ಅನ್ಯಥಾ {
        /* invalid start byte */
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    length = expected + 1;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i <= expected; i++) {
        ಯದಿ (s[i] < 0x80 || s[i] >= 0xC0) {
            ಪ್ರತ್ಯಾಗಮ 0;
        }
    }
    ಪ್ರತ್ಯಾಗಮ length;
}

ಪೂರ್ಣಾಙ್ಕ
_PyTokenizer_ensure_utf8(ಧ್ರುವ ವರ್ಣ *ಸಾಲು, ಸಂರಚನಾ tok_state *tok, ಪೂರ್ಣಾಙ್ಕ lineno)
{
    ಧ್ರುವ ವರ್ಣ *badchar = ರಿಕ್ತ;
    ಧ್ರುವ ವರ್ಣ *c;
    ಪೂರ್ಣಾಙ್ಕ length;
    ಧ್ರುವ ವರ್ಣ *ಸಾಲು_start = ಸಾಲು;
    ಪ್ರತಿ (c = ಸಾಲು; *c; c += length) {
        ಯದಿ (!(length = valid_utf8((ಧ್ರುವ unsigned ವರ್ಣ *)c))) {
            badchar = c;
            ಭಙ್ಗ;
        }
        ಯದಿ (*c == '\n') {
            lineno++;
            ಸಾಲು_start = c + 1;
        }
    }
    ಯದಿ (badchar) {
        _PyTokenizer_syntaxerror_at(
                tok, ಸಾಲು_start, badchar - ಸಾಲು_start + 1, lineno,
                -1, -1,
                "Non-UTF-8 code starting with '\\x%.2x'"
                "%s%V on line %i, "
                "but no encoding declared; "
                "see https://peps.python.org/pep-0263/ for details",
                (unsigned ವರ್ಣ)*badchar,
                tok->filename ? " in file " : "", tok->filename, "",
                lineno);
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತ್ಯಾಗಮ 1;
}


/* ############## DEBUGGING STUFF ############## */

#ifdef Py_DEBUG
ಶೂನ್ಯ
_PyTokenizer_print_escape(FILE *f, ಧ್ರುವ ವರ್ಣ *s, Py_ssize_t size)
{
    ಯದಿ (s == ರಿಕ್ತ) {
        fputs("NULL", f);
        ಪ್ರತ್ಯಾಗಮ;
    }
    putc('"', f);
    ಯಾವತ್ (size-- > 0) {
        unsigned ವರ್ಣ c = *s++;
        ವಿಕಲ್ಪ (c) {
            ಪಕ್ಷ '\n': fputs("\\n", f); ಭಙ್ಗ;
            ಪಕ್ಷ '\r': fputs("\\r", f); ಭಙ್ಗ;
            ಪಕ್ಷ '\t': fputs("\\t", f); ಭಙ್ಗ;
            ಪಕ್ಷ '\f': fputs("\\f", f); ಭಙ್ಗ;
            ಪಕ್ಷ '\'': fputs("\\'", f); ಭಙ್ಗ;
            ಪಕ್ಷ '"': fputs("\\\"", f); ಭಙ್ಗ;
            ಸ್ವಭಾವ:
                ಯದಿ (0x20 <= c && c <= 0x7f)
                    putc(c, f);
                ಅನ್ಯಥಾ
                    fprintf(f, "\\x%02x", c);
        }
    }
    putc('"', f);
}
#endif
