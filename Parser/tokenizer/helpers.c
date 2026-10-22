#شامل "Python.h"
#شامل "errcode.h"
#شامل "pycore_runtime.h"       // _Py_ID()
#شامل "pycore_token.h"
#شامل "pycore_tuple.h"         // _PyTuple_FromPair

#شامل "../lexer/state.h"


/* ############## ERRORS ############## */

/* Convert a 1-based column in bytes into a 1-based column in characters.
   The line is UTF-8 encoded, so it is enough to skip continuation bytes. */
ایستا صحیح
byte_col_to_char_col(ثابت نویسه *سطر, صحیح byte_col)
{
    صحیح char_col = 1;
    برای (صحیح i = 0; i < byte_col - 1; i++) {
        اگر ((سطر[i] & 0xC0) != 0x80) {
            char_col++;
        }
    }
    بازگشت char_col;
}

ایستا صحیح
_syntaxerror_range(ساختار tok_state *tok, ثابت نویسه *سطر,
                   Py_ssize_t cursor, صحیح lineno, ثابت نویسه *format,
                   صحیح col_offset, صحیح end_col_offset,
                   va_list vargs)
{
    // In release builds, we don't want to overwrite a previous error, but in debug builds we
    // want to fail if we are not doing it so we can fix it.
    أكد(tok->done != E_ERROR);
    اگر (tok->done == E_ERROR) {
        بازگشت ERRORTOKEN;
    }
    PyObject *errmsg, *errtext, *args;
    errmsg = PyUnicode_FromFormatV(format, vargs);
    اگر (!errmsg) {
        برو error;
    }

    errtext = PyUnicode_DecodeUTF8(سطر, cursor,
                                   "replace");
    اگر (!errtext) {
        برو error;
    }

    اگر (col_offset == -1) {
        col_offset = (صحیح)PyUnicode_GET_LENGTH(errtext);
    }
    وگرنه اگر (col_offset > 0) {
        col_offset = byte_col_to_char_col(سطر, col_offset);
    }
    اگر (end_col_offset == -1) {
        end_col_offset = col_offset;
    }
    وگرنه اگر (end_col_offset > 0) {
        end_col_offset = byte_col_to_char_col(سطر, end_col_offset);
    }

    Py_ssize_t سطر_len = strcspn(سطر, "\n");
    اگر (سطر_len != cursor) {
        Py_DECREF(errtext);
        errtext = PyUnicode_DecodeUTF8(سطر, سطر_len,
                                       "replace");
    }
    اگر (!errtext) {
        برو error;
    }

    args = Py_Build_مقدار("(O(OiiNii))", errmsg,
                         tok->filename ? tok->filename : Py_None,
                         lineno, col_offset, errtext,
                         lineno, end_col_offset);
    اگر (args) {
        PyErr_SetObject(PyExc_SyntaxError, args);
        Py_DECREF(args);
    }

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    بازگشت ERRORTOKEN;
}

صحیح
_PyTokenizer_syntaxerror(ساختار tok_state *tok, ثابت نویسه *format, ...)
{
    // These errors are cleaned on startup. Todo: Fix it.
    va_list vargs;
    va_start(vargs, format);
    صحیح ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->سطر_start),
        tok->cur - tok->سطر_start, tok->lineno, format, -1, -1, vargs);
    va_end(vargs);
    بازگشت ret;
}

صحیح
_PyTokenizer_syntaxerror_known_range(ساختار tok_state *tok,
                        صحیح col_offset, صحیح end_col_offset,
                        ثابت نویسه *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    صحیح ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->سطر_start),
        tok->cur - tok->سطر_start, tok->lineno,
        format, col_offset, end_col_offset, vargs);
    va_end(vargs);
    بازگشت ret;
}

صحیح
_PyTokenizer_syntaxerror_at(ساختار tok_state *tok, ثابت نویسه *سطر,
                             Py_ssize_t cursor, صحیح lineno,
                             صحیح col_offset, صحیح end_col_offset,
                             ثابت نویسه *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    صحیح ret = _syntaxerror_range(tok, سطر, cursor, lineno, format,
                                col_offset, end_col_offset, vargs);
    va_end(vargs);
    بازگشت ret;
}

صحیح
_PyTokenizer_indenterror(ساختار tok_state *tok)
{
    tok->done = E_TABSPACE;
    tok->cur = tok->inp;
    بازگشت ERRORTOKEN;
}

صحیح
_PyTokenizer_warn_invalid_escape_sequence(ساختار tok_state *tok, صحیح first_invalid_escape_char)
{
    PyObject *پیام = PyUnicode_FromFormat(
        "\"\\%c\" is an invalid escape sequence. "
        "Such sequences will not work in the future. "
        "Did you mean \"\\\\%c\"? A raw string is also an option.",
        (نویسه) first_invalid_escape_char,
        (نویسه) first_invalid_escape_char
    );

    اگر (پیام == پوچ) {
        بازگشت -1;
    }

    اگر (PyErr_WarnExplicitObject(PyExc_SyntaxWarning, پیام, tok->filename,
                                 tok->lineno, tok->module, پوچ) < 0) {
        Py_DECREF(پیام);

        اگر (PyErr_ExceptionMatches(PyExc_SyntaxWarning)) {
            /* Replace the SyntaxWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            بازگشت _PyTokenizer_syntaxerror(tok,
                "\"\\%c\" is an invalid escape sequence. "
                "Did you mean \"\\\\%c\"? A raw string is also an option.",
                (نویسه) first_invalid_escape_char,
                (نویسه) first_invalid_escape_char);
        }

        بازگشت -1;
    }

    Py_DECREF(پیام);
    بازگشت 0;
}

تهی
_PyTokenizer_raise_init_error(PyObject *filename)
{
    اگر (!(PyErr_ExceptionMatches(PyExc_LookupError)
          || PyErr_ExceptionMatches(PyExc_SyntaxError)
          || PyErr_ExceptionMatches(Py_Exc_مقدار_Error)
          || PyErr_ExceptionMatches(PyExc_UnicodeDecodeError))) {
        بازگشت;
    }
    PyObject *errstr = پوچ;
    PyObject *tuple = پوچ;
    PyObject *type;
    PyObject *مقدار;
    PyObject *tback;
    PyErr_Fetch(&type, &مقدار, &tback);
    اگر (PyErr_GivenExceptionMatches(مقدار, PyExc_SyntaxError)) {
        اگر (PyObject_SetAttr(مقدار, &_Py_ID(filename), filename)) {
            برو error;
        }
        PyErr_Restore(type, مقدار, tback);
        بازگشت;
    }
    errstr = PyObject_Str(مقدار);
    اگر (!errstr) {
        برو error;
    }

    PyObject *tmp = Py_Build_مقدار("(OiiO)", filename, 0, -1, Py_None);
    اگر (!tmp) {
        برو error;
    }

    tuple = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    اگر (!tuple) {
        برو error;
    }
    PyErr_SetObject(PyExc_SyntaxError, tuple);

error:
    Py_XDECREF(type);
    Py_XDECREF(مقدار);
    Py_XDECREF(tback);
    Py_XDECREF(errstr);
    Py_XDECREF(tuple);
}

صحیح
_PyTokenizer_parser_warn(ساختار tok_state *tok, PyObject *category, ثابت نویسه *format, ...)
{
    PyObject *errmsg;
    va_list vargs;
    va_start(vargs, format);
    errmsg = PyUnicode_FromFormatV(format, vargs);
    va_end(vargs);
    اگر (!errmsg) {
        برو error;
    }

    اگر (PyErr_WarnExplicitObject(category, errmsg, tok->filename,
                                 tok->lineno, tok->module, پوچ) < 0) {
        اگر (PyErr_ExceptionMatches(category)) {
            /* Replace the DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();
            _PyTokenizer_syntaxerror(tok, "%U", errmsg);
        }
        برو error;
    }
    Py_DECREF(errmsg);
    بازگشت 0;

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    بازگشت -1;
}


/* Check whether the characters at s start a valid
   UTF-8 sequence. Return the number of characters forming
   the sequence if yes, 0 if not.  The special cases match
   those in stringlib/codecs.h:utf8_decode.
*/
ایستا صحیح
valid_utf8(ثابت unsigned نویسه* s)
{
    صحیح expected = 0;
    صحیح length;
    اگر (*s < 0x80) {
        /* single-byte code */
        بازگشت 1;
    }
    وگرنه اگر (*s < 0xE0) {
        /* \xC2\x80-\xDF\xBF -- 0080-07FF */
        اگر (*s < 0xC2) {
            /* invalid sequence
               \x80-\xBF -- continuation byte
               \xC0-\xC1 -- fake 0000-007F */
            بازگشت 0;
        }
        expected = 1;
    }
    وگرنه اگر (*s < 0xF0) {
        /* \xE0\xA0\x80-\xEF\xBF\xBF -- 0800-FFFF */
        اگر (*s == 0xE0 && *(s + 1) < 0xA0) {
            /* invalid sequence
               \xE0\x80\x80-\xE0\x9F\xBF -- fake 0000-0800 */
            بازگشت 0;
        }
        وگرنه اگر (*s == 0xED && *(s + 1) >= 0xA0) {
            /* Decoding UTF-8 sequences in range \xED\xA0\x80-\xED\xBF\xBF
               will result in surrogates in range D800-DFFF. Surrogates are
               not valid UTF-8 so they are rejected.
               See https://www.unicode.org/versions/Unicode5.2.0/ch03.pdf
               (table 3-7) and http://www.rfc-editor.org/rfc/rfc3629.txt */
            بازگشت 0;
        }
        expected = 2;
    }
    وگرنه اگر (*s < 0xF5) {
        /* \xF0\x90\x80\x80-\xF4\x8F\xBF\xBF -- 10000-10FFFF */
        اگر (*(s + 1) < 0x90 ? *s == 0xF0 : *s == 0xF4) {
            /* invalid sequence -- one of:
               \xF0\x80\x80\x80-\xF0\x8F\xBF\xBF -- fake 0000-FFFF
               \xF4\x90\x80\x80- -- 110000- overflow */
            بازگشت 0;
        }
        expected = 3;
    }
    وگرنه {
        /* invalid start byte */
        بازگشت 0;
    }
    length = expected + 1;
    برای (صحیح i = 1; i <= expected; i++) {
        اگر (s[i] < 0x80 || s[i] >= 0xC0) {
            بازگشت 0;
        }
    }
    بازگشت length;
}

صحیح
_PyTokenizer_ensure_utf8(ثابت نویسه *سطر, ساختار tok_state *tok, صحیح lineno)
{
    ثابت نویسه *badchar = پوچ;
    ثابت نویسه *c;
    صحیح length;
    ثابت نویسه *سطر_start = سطر;
    برای (c = سطر; *c; c += length) {
        اگر (!(length = valid_utf8((ثابت unsigned نویسه *)c))) {
            badchar = c;
            بشکن;
        }
        اگر (*c == '\n') {
            lineno++;
            سطر_start = c + 1;
        }
    }
    اگر (badchar) {
        _PyTokenizer_syntaxerror_at(
                tok, سطر_start, badchar - سطر_start + 1, lineno,
                -1, -1,
                "Non-UTF-8 code starting with '\\x%.2x'"
                "%s%V on line %i, "
                "but no encoding declared; "
                "see https://peps.python.org/pep-0263/ for details",
                (unsigned نویسه)*badchar,
                tok->filename ? " in file " : "", tok->filename, "",
                lineno);
        بازگشت 0;
    }
    بازگشت 1;
}


/* ############## DEBUGGING STUFF ############## */

#ifdef Py_DEBUG
تهی
_PyTokenizer_print_escape(FILE *f, ثابت نویسه *s, Py_ssize_t size)
{
    اگر (s == پوچ) {
        fputs("NULL", f);
        بازگشت;
    }
    putc('"', f);
    وقتی (size-- > 0) {
        unsigned نویسه c = *s++;
        انتخاب (c) {
            حالت '\n': fputs("\\n", f); بشکن;
            حالت '\r': fputs("\\r", f); بشکن;
            حالت '\t': fputs("\\t", f); بشکن;
            حالت '\f': fputs("\\f", f); بشکن;
            حالت '\'': fputs("\\'", f); بشکن;
            حالت '"': fputs("\\\"", f); بشکن;
            پیشفرض:
                اگر (0x20 <= c && c <= 0x7f)
                    putc(c, f);
                وگرنه
                    fprintf(f, "\\x%02x", c);
        }
    }
    putc('"', f);
}
#endif
