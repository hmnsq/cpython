#ضمن "Python.h"
#ضمن "errcode.h"
#ضمن "pycore_runtime.h"       // _Py_ID()
#ضمن "pycore_token.h"
#ضمن "pycore_tuple.h"         // _PyTuple_FromPair

#ضمن "../lexer/state.h"


/* ############## ERRORS ############## */

/* Convert a 1-based column in bytes into a 1-based column in characters.
   The line is UTF-8 encoded, so it is enough to skip continuation bytes. */
ساكن صحيح
byte_col_to_char_col(ثابت حرف *سطر, صحيح byte_col)
{
    صحيح char_col = 1;
    لكل (صحيح i = 0; i < byte_col - 1; i++) {
        إذا ((سطر[i] & 0xC0) != 0x80) {
            char_col++;
        }
    }
    إرجاع char_col;
}

ساكن صحيح
_syntaxerror_range(بنية tok_state *tok, ثابت حرف *سطر,
                   Py_ssize_t cursor, صحيح lineno, ثابت حرف *format,
                   صحيح col_offset, صحيح end_col_offset,
                   va_list vargs)
{
    // In release builds, we don't want to overwrite a previous error, but in debug builds we
    // want to fail if we are not doing it so we can fix it.
    أكد(tok->done != E_ERROR);
    إذا (tok->done == E_ERROR) {
        إرجاع ERRORTOKEN;
    }
    PyObject *errmsg, *errtext, *args;
    errmsg = PyUnicode_FromFormatV(format, vargs);
    إذا (!errmsg) {
        اذهب error;
    }

    errtext = PyUnicode_DecodeUTF8(سطر, cursor,
                                   "replace");
    إذا (!errtext) {
        اذهب error;
    }

    إذا (col_offset == -1) {
        col_offset = (صحيح)PyUnicode_GET_LENGTH(errtext);
    }
    وإلا إذا (col_offset > 0) {
        col_offset = byte_col_to_char_col(سطر, col_offset);
    }
    إذا (end_col_offset == -1) {
        end_col_offset = col_offset;
    }
    وإلا إذا (end_col_offset > 0) {
        end_col_offset = byte_col_to_char_col(سطر, end_col_offset);
    }

    Py_ssize_t سطر_len = strcspn(سطر, "\n");
    إذا (سطر_len != cursor) {
        Py_DECREF(errtext);
        errtext = PyUnicode_DecodeUTF8(سطر, سطر_len,
                                       "replace");
    }
    إذا (!errtext) {
        اذهب error;
    }

    args = Py_Build_قيمة("(O(OiiNii))", errmsg,
                         tok->filename ? tok->filename : Py_None,
                         lineno, col_offset, errtext,
                         lineno, end_col_offset);
    إذا (args) {
        PyErr_SetObject(PyExc_SyntaxError, args);
        Py_DECREF(args);
    }

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    إرجاع ERRORTOKEN;
}

صحيح
_PyTokenizer_syntaxerror(بنية tok_state *tok, ثابت حرف *format, ...)
{
    // These errors are cleaned on startup. Todo: Fix it.
    va_list vargs;
    va_start(vargs, format);
    صحيح ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->سطر_start),
        tok->cur - tok->سطر_start, tok->lineno, format, -1, -1, vargs);
    va_end(vargs);
    إرجاع ret;
}

صحيح
_PyTokenizer_syntaxerror_known_range(بنية tok_state *tok,
                        صحيح col_offset, صحيح end_col_offset,
                        ثابت حرف *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    صحيح ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->سطر_start),
        tok->cur - tok->سطر_start, tok->lineno,
        format, col_offset, end_col_offset, vargs);
    va_end(vargs);
    إرجاع ret;
}

صحيح
_PyTokenizer_syntaxerror_at(بنية tok_state *tok, ثابت حرف *سطر,
                             Py_ssize_t cursor, صحيح lineno,
                             صحيح col_offset, صحيح end_col_offset,
                             ثابت حرف *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    صحيح ret = _syntaxerror_range(tok, سطر, cursor, lineno, format,
                                col_offset, end_col_offset, vargs);
    va_end(vargs);
    إرجاع ret;
}

صحيح
_PyTokenizer_indenterror(بنية tok_state *tok)
{
    tok->done = E_TABSPACE;
    tok->cur = tok->inp;
    إرجاع ERRORTOKEN;
}

صحيح
_PyTokenizer_warn_invalid_escape_sequence(بنية tok_state *tok, صحيح first_invalid_escape_char)
{
    PyObject *رسالة = PyUnicode_FromFormat(
        "\"\\%c\" is an invalid escape sequence. "
        "Such sequences will not work in the future. "
        "Did you mean \"\\\\%c\"? A raw string is also an option.",
        (حرف) first_invalid_escape_char,
        (حرف) first_invalid_escape_char
    );

    إذا (رسالة == عدم) {
        إرجاع -1;
    }

    إذا (PyErr_WarnExplicitObject(PyExc_SyntaxWarning, رسالة, tok->filename,
                                 tok->lineno, tok->module, عدم) < 0) {
        Py_DECREF(رسالة);

        إذا (PyErr_ExceptionMatches(PyExc_SyntaxWarning)) {
            /* Replace the SyntaxWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            إرجاع _PyTokenizer_syntaxerror(tok,
                "\"\\%c\" is an invalid escape sequence. "
                "Did you mean \"\\\\%c\"? A raw string is also an option.",
                (حرف) first_invalid_escape_char,
                (حرف) first_invalid_escape_char);
        }

        إرجاع -1;
    }

    Py_DECREF(رسالة);
    إرجاع 0;
}

فراغ
_PyTokenizer_raise_init_error(PyObject *filename)
{
    إذا (!(PyErr_ExceptionMatches(PyExc_LookupError)
          || PyErr_ExceptionMatches(PyExc_SyntaxError)
          || PyErr_ExceptionMatches(Py_Exc_قيمة_Error)
          || PyErr_ExceptionMatches(PyExc_UnicodeDecodeError))) {
        إرجاع;
    }
    PyObject *errstr = عدم;
    PyObject *tuple = عدم;
    PyObject *type;
    PyObject *قيمة;
    PyObject *tback;
    PyErr_Fetch(&type, &قيمة, &tback);
    إذا (PyErr_GivenExceptionMatches(قيمة, PyExc_SyntaxError)) {
        إذا (PyObject_SetAttr(قيمة, &_Py_ID(filename), filename)) {
            اذهب error;
        }
        PyErr_Restore(type, قيمة, tback);
        إرجاع;
    }
    errstr = PyObject_Str(قيمة);
    إذا (!errstr) {
        اذهب error;
    }

    PyObject *tmp = Py_Build_قيمة("(OiiO)", filename, 0, -1, Py_None);
    إذا (!tmp) {
        اذهب error;
    }

    tuple = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    إذا (!tuple) {
        اذهب error;
    }
    PyErr_SetObject(PyExc_SyntaxError, tuple);

error:
    Py_XDECREF(type);
    Py_XDECREF(قيمة);
    Py_XDECREF(tback);
    Py_XDECREF(errstr);
    Py_XDECREF(tuple);
}

صحيح
_PyTokenizer_parser_warn(بنية tok_state *tok, PyObject *category, ثابت حرف *format, ...)
{
    PyObject *errmsg;
    va_list vargs;
    va_start(vargs, format);
    errmsg = PyUnicode_FromFormatV(format, vargs);
    va_end(vargs);
    إذا (!errmsg) {
        اذهب error;
    }

    إذا (PyErr_WarnExplicitObject(category, errmsg, tok->filename,
                                 tok->lineno, tok->module, عدم) < 0) {
        إذا (PyErr_ExceptionMatches(category)) {
            /* Replace the DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();
            _PyTokenizer_syntaxerror(tok, "%U", errmsg);
        }
        اذهب error;
    }
    Py_DECREF(errmsg);
    إرجاع 0;

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    إرجاع -1;
}


/* Check whether the characters at s start a valid
   UTF-8 sequence. Return the number of characters forming
   the sequence if yes, 0 if not.  The special cases match
   those in stringlib/codecs.h:utf8_decode.
*/
ساكن صحيح
valid_utf8(ثابت unsigned حرف* s)
{
    صحيح expected = 0;
    صحيح length;
    إذا (*s < 0x80) {
        /* single-byte code */
        إرجاع 1;
    }
    وإلا إذا (*s < 0xE0) {
        /* \xC2\x80-\xDF\xBF -- 0080-07FF */
        إذا (*s < 0xC2) {
            /* invalid sequence
               \x80-\xBF -- continuation byte
               \xC0-\xC1 -- fake 0000-007F */
            إرجاع 0;
        }
        expected = 1;
    }
    وإلا إذا (*s < 0xF0) {
        /* \xE0\xA0\x80-\xEF\xBF\xBF -- 0800-FFFF */
        إذا (*s == 0xE0 && *(s + 1) < 0xA0) {
            /* invalid sequence
               \xE0\x80\x80-\xE0\x9F\xBF -- fake 0000-0800 */
            إرجاع 0;
        }
        وإلا إذا (*s == 0xED && *(s + 1) >= 0xA0) {
            /* Decoding UTF-8 sequences in range \xED\xA0\x80-\xED\xBF\xBF
               will result in surrogates in range D800-DFFF. Surrogates are
               not valid UTF-8 so they are rejected.
               See https://www.unicode.org/versions/Unicode5.2.0/ch03.pdf
               (table 3-7) and http://www.rfc-editor.org/rfc/rfc3629.txt */
            إرجاع 0;
        }
        expected = 2;
    }
    وإلا إذا (*s < 0xF5) {
        /* \xF0\x90\x80\x80-\xF4\x8F\xBF\xBF -- 10000-10FFFF */
        إذا (*(s + 1) < 0x90 ? *s == 0xF0 : *s == 0xF4) {
            /* invalid sequence -- one of:
               \xF0\x80\x80\x80-\xF0\x8F\xBF\xBF -- fake 0000-FFFF
               \xF4\x90\x80\x80- -- 110000- overflow */
            إرجاع 0;
        }
        expected = 3;
    }
    وإلا {
        /* invalid start byte */
        إرجاع 0;
    }
    length = expected + 1;
    لكل (صحيح i = 1; i <= expected; i++) {
        إذا (s[i] < 0x80 || s[i] >= 0xC0) {
            إرجاع 0;
        }
    }
    إرجاع length;
}

صحيح
_PyTokenizer_ensure_utf8(ثابت حرف *سطر, بنية tok_state *tok, صحيح lineno)
{
    ثابت حرف *badchar = عدم;
    ثابت حرف *c;
    صحيح length;
    ثابت حرف *سطر_start = سطر;
    لكل (c = سطر; *c; c += length) {
        إذا (!(length = valid_utf8((ثابت unsigned حرف *)c))) {
            badchar = c;
            اكسر;
        }
        إذا (*c == '\n') {
            lineno++;
            سطر_start = c + 1;
        }
    }
    إذا (badchar) {
        _PyTokenizer_syntaxerror_at(
                tok, سطر_start, badchar - سطر_start + 1, lineno,
                -1, -1,
                "Non-UTF-8 code starting with '\\x%.2x'"
                "%s%V on line %i, "
                "but no encoding declared; "
                "see https://peps.python.org/pep-0263/ for details",
                (unsigned حرف)*badchar,
                tok->filename ? " in file " : "", tok->filename, "",
                lineno);
        إرجاع 0;
    }
    إرجاع 1;
}


/* ############## DEBUGGING STUFF ############## */

#ifdef Py_DEBUG
فراغ
_PyTokenizer_print_escape(FILE *f, ثابت حرف *s, Py_ssize_t size)
{
    إذا (s == عدم) {
        fputs("NULL", f);
        إرجاع;
    }
    putc('"', f);
    بينما (size-- > 0) {
        unsigned حرف c = *s++;
        بدل (c) {
            حالة '\n': fputs("\\n", f); اكسر;
            حالة '\r': fputs("\\r", f); اكسر;
            حالة '\t': fputs("\\t", f); اكسر;
            حالة '\f': fputs("\\f", f); اكسر;
            حالة '\'': fputs("\\'", f); اكسر;
            حالة '"': fputs("\\\"", f); اكسر;
            افتراضي:
                إذا (0x20 <= c && c <= 0x7f)
                    putc(c, f);
                وإلا
                    fprintf(f, "\\x%02x", c);
        }
    }
    putc('"', f);
}
#endif
