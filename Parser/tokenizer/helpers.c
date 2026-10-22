#شامل "Python.h"
#شامل "errcode.h"
#شامل "pycore_runtime.h"       // _Py_ID()
#شامل "pycore_token.h"
#شامل "pycore_tuple.h"         // _PyTuple_FromPair

#شامل "../lexer/state.h"


/* ############## ERRORS ############## */

/* Convert a 1-based column in bytes into a 1-based column in characters.
   The line is UTF-8 encoded, so it is enough to skip continuation bytes. */
ساکن صحیح
byte_ستون_مقصد_char_ستون(ثابت حرف *سطر, صحیح byte_ستون)
{
    صحیح char_ستون = 1;
    برائے (صحیح i = 0; i < byte_ستون - 1; i++) {
        اگر ((سطر[i] & 0xC0) != 0x80) {
            char_ستون++;
        }
    }
    واپس char_ستون;
}

ساکن صحیح
_syntaxerror_range(ساخت tok_state *tok, ثابت حرف *سطر,
                   Py_ssize_t cursor, صحیح lineno, ثابت حرف *format,
                   صحیح ستون_انحراف, صحیح انتہا_ستون_انحراف,
                   va_list vargs)
{
    // In release builds, we don't want to overwrite a previous error, but in debug builds we
    // want to fail if we are not doing it so we can fix it.
    أكد(tok->مکمل != E_ERROR);
    اگر (tok->مکمل == E_ERROR) {
        واپس ERRORTOKEN;
    }
    PyObject *errmsg, *errtext, *دلائل;
    errmsg = PyUnicode_FromFormatV(format, vargs);
    اگر (!errmsg) {
        جاؤ error;
    }

    errtext = PyUnicode_DecodeUTF8(سطر, cursor,
                                   "replace");
    اگر (!errtext) {
        جاؤ error;
    }

    اگر (ستون_انحراف == -1) {
        ستون_انحراف = (صحیح)Py_Unicode_GET_لمبائی(errtext);
    }
    ورنہ اگر (ستون_انحراف > 0) {
        ستون_انحراف = byte_ستون_مقصد_char_ستون(سطر, ستون_انحراف);
    }
    اگر (انتہا_ستون_انحراف == -1) {
        انتہا_ستون_انحراف = ستون_انحراف;
    }
    ورنہ اگر (انتہا_ستون_انحراف > 0) {
        انتہا_ستون_انحراف = byte_ستون_مقصد_char_ستون(سطر, انتہا_ستون_انحراف);
    }

    Py_ssize_t سطر_len = strcspn(سطر, "\n");
    اگر (سطر_len != cursor) {
        Py_DECREF(errtext);
        errtext = PyUnicode_DecodeUTF8(سطر, سطر_len,
                                       "replace");
    }
    اگر (!errtext) {
        جاؤ error;
    }

    دلائل = Py_Build_قدر("(O(OiiNii))", errmsg,
                         tok->filename ? tok->filename : Py_None,
                         lineno, ستون_انحراف, errtext,
                         lineno, انتہا_ستون_انحراف);
    اگر (دلائل) {
        Py_Err_سیٹ_Object(PyExc_SyntaxError, دلائل);
        Py_DECREF(دلائل);
    }

error:
    Py_XDECREF(errmsg);
    tok->مکمل = E_ERROR;
    واپس ERRORTOKEN;
}

صحیح
_PyTokenizer_syntaxerror(ساخت tok_state *tok, ثابت حرف *format, ...)
{
    // These errors are cleaned on startup. Todo: Fix it.
    va_list vargs;
    va_آغاز(vargs, format);
    صحیح ret = _syntaxerror_range(
        tok, Py_Tok_ماخذ_اشارہ_گر(&tok->ماخذ, tok->سطر_آغاز),
        tok->cur - tok->سطر_آغاز, tok->lineno, format, -1, -1, vargs);
    va_انتہا(vargs);
    واپس ret;
}

صحیح
Py_Tokenizer_syntaxerror_معلوم_range(ساخت tok_state *tok,
                        صحیح ستون_انحراف, صحیح انتہا_ستون_انحراف,
                        ثابت حرف *format, ...)
{
    va_list vargs;
    va_آغاز(vargs, format);
    صحیح ret = _syntaxerror_range(
        tok, Py_Tok_ماخذ_اشارہ_گر(&tok->ماخذ, tok->سطر_آغاز),
        tok->cur - tok->سطر_آغاز, tok->lineno,
        format, ستون_انحراف, انتہا_ستون_انحراف, vargs);
    va_انتہا(vargs);
    واپس ret;
}

صحیح
_PyTokenizer_syntaxerror_at(ساخت tok_state *tok, ثابت حرف *سطر,
                             Py_ssize_t cursor, صحیح lineno,
                             صحیح ستون_انحراف, صحیح انتہا_ستون_انحراف,
                             ثابت حرف *format, ...)
{
    va_list vargs;
    va_آغاز(vargs, format);
    صحیح ret = _syntaxerror_range(tok, سطر, cursor, lineno, format,
                                ستون_انحراف, انتہا_ستون_انحراف, vargs);
    va_انتہا(vargs);
    واپس ret;
}

صحیح
_PyTokenizer_indenterror(ساخت tok_state *tok)
{
    tok->مکمل = E_TABSPACE;
    tok->cur = tok->inp;
    واپس ERRORTOKEN;
}

صحیح
_PyTokenizer_warn_invalid_escape_sequence(ساخت tok_state *tok, صحیح پہلا_invalid_escape_char)
{
    PyObject *پیغام = PyUnicode_FromFormat(
        "\"\\%c\" is an invalid escape sequence. "
        "Such sequences will not work in the future. "
        "Did you mean \"\\\\%c\"? A raw string is also an option.",
        (حرف) پہلا_invalid_escape_char,
        (حرف) پہلا_invalid_escape_char
    );

    اگر (پیغام == تہی) {
        واپس -1;
    }

    اگر (PyErr_WarnExplicitObject(PyExc_SyntaxWarning, پیغام, tok->filename,
                                 tok->lineno, tok->module, تہی) < 0) {
        Py_DECREF(پیغام);

        اگر (PyErr_ExceptionMatches(PyExc_SyntaxWarning)) {
            /* Replace the SyntaxWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            واپس _PyTokenizer_syntaxerror(tok,
                "\"\\%c\" is an invalid escape sequence. "
                "Did you mean \"\\\\%c\"? A raw string is also an option.",
                (حرف) پہلا_invalid_escape_char,
                (حرف) پہلا_invalid_escape_char);
        }

        واپس -1;
    }

    Py_DECREF(پیغام);
    واپس 0;
}

خالی
_PyTokenizer_raise_init_error(PyObject *filename)
{
    اگر (!(PyErr_ExceptionMatches(PyExc_LookupError)
          || PyErr_ExceptionMatches(PyExc_SyntaxError)
          || PyErr_ExceptionMatches(Py_Exc_قدر_Error)
          || PyErr_ExceptionMatches(PyExc_UnicodeDecodeError))) {
        واپس;
    }
    PyObject *errstr = تہی;
    PyObject *tuple = تہی;
    PyObject *type;
    PyObject *قدر;
    PyObject *tback;
    PyErr_Fetch(&type, &قدر, &tback);
    اگر (PyErr_GivenExceptionMatches(قدر, PyExc_SyntaxError)) {
        اگر (Py_Object_سیٹ_Attr(قدر, &_Py_ID(filename), filename)) {
            جاؤ error;
        }
        PyErr_Restore(type, قدر, tback);
        واپس;
    }
    errstr = PyObject_Str(قدر);
    اگر (!errstr) {
        جاؤ error;
    }

    PyObject *tmp = Py_Build_قدر("(OiiO)", filename, 0, -1, Py_None);
    اگر (!tmp) {
        جاؤ error;
    }

    tuple = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    اگر (!tuple) {
        جاؤ error;
    }
    Py_Err_سیٹ_Object(PyExc_SyntaxError, tuple);

error:
    Py_XDECREF(type);
    Py_XDECREF(قدر);
    Py_XDECREF(tback);
    Py_XDECREF(errstr);
    Py_XDECREF(tuple);
}

صحیح
_PyTokenizer_parser_warn(ساخت tok_state *tok, PyObject *category, ثابت حرف *format, ...)
{
    PyObject *errmsg;
    va_list vargs;
    va_آغاز(vargs, format);
    errmsg = PyUnicode_FromFormatV(format, vargs);
    va_انتہا(vargs);
    اگر (!errmsg) {
        جاؤ error;
    }

    اگر (PyErr_WarnExplicitObject(category, errmsg, tok->filename,
                                 tok->lineno, tok->module, تہی) < 0) {
        اگر (PyErr_ExceptionMatches(category)) {
            /* Replace the DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();
            _PyTokenizer_syntaxerror(tok, "%U", errmsg);
        }
        جاؤ error;
    }
    Py_DECREF(errmsg);
    واپس 0;

error:
    Py_XDECREF(errmsg);
    tok->مکمل = E_ERROR;
    واپس -1;
}


/* Check whether the characters at s start a valid
   UTF-8 sequence. Return the number of characters forming
   the sequence if yes, 0 if not.  The special cases match
   those in stringlib/codecs.h:utf8_decode.
*/
ساکن صحیح
valid_utf8(ثابت غیرعلامتی حرف* s)
{
    صحیح expected = 0;
    صحیح لمبائی_2;
    اگر (*s < 0x80) {
        /* single-byte code */
        واپس 1;
    }
    ورنہ اگر (*s < 0xE0) {
        /* \xC2\x80-\xDF\xBF -- 0080-07FF */
        اگر (*s < 0xC2) {
            /* invalid sequence
               \x80-\xBF -- continuation byte
               \xC0-\xC1 -- fake 0000-007F */
            واپس 0;
        }
        expected = 1;
    }
    ورنہ اگر (*s < 0xF0) {
        /* \xE0\xA0\x80-\xEF\xBF\xBF -- 0800-FFFF */
        اگر (*s == 0xE0 && *(s + 1) < 0xA0) {
            /* invalid sequence
               \xE0\x80\x80-\xE0\x9F\xBF -- fake 0000-0800 */
            واپس 0;
        }
        ورنہ اگر (*s == 0xED && *(s + 1) >= 0xA0) {
            /* Decoding UTF-8 sequences in range \xED\xA0\x80-\xED\xBF\xBF
               will result in surrogates in range D800-DFFF. Surrogates are
               not valid UTF-8 so they are rejected.
               See https://www.unicode.org/versions/Unicode5.2.0/ch03.pdf
               (table 3-7) and http://www.rfc-editor.org/rfc/rfc3629.txt */
            واپس 0;
        }
        expected = 2;
    }
    ورنہ اگر (*s < 0xF5) {
        /* \xF0\x90\x80\x80-\xF4\x8F\xBF\xBF -- 10000-10FFFF */
        اگر (*(s + 1) < 0x90 ? *s == 0xF0 : *s == 0xF4) {
            /* invalid sequence -- one of:
               \xF0\x80\x80\x80-\xF0\x8F\xBF\xBF -- fake 0000-FFFF
               \xF4\x90\x80\x80- -- 110000- overflow */
            واپس 0;
        }
        expected = 3;
    }
    ورنہ {
        /* invalid start byte */
        واپس 0;
    }
    لمبائی_2 = expected + 1;
    برائے (صحیح i = 1; i <= expected; i++) {
        اگر (s[i] < 0x80 || s[i] >= 0xC0) {
            واپس 0;
        }
    }
    واپس لمبائی_2;
}

صحیح
_PyTokenizer_ensure_utf8(ثابت حرف *سطر, ساخت tok_state *tok, صحیح lineno)
{
    ثابت حرف *badchar = تہی;
    ثابت حرف *c;
    صحیح لمبائی_2;
    ثابت حرف *سطر_آغاز = سطر;
    برائے (c = سطر; *c; c += لمبائی_2) {
        اگر (!(لمبائی_2 = valid_utf8((ثابت غیرعلامتی حرف *)c))) {
            badchar = c;
            توڑو;
        }
        اگر (*c == '\n') {
            lineno++;
            سطر_آغاز = c + 1;
        }
    }
    اگر (badchar) {
        _PyTokenizer_syntaxerror_at(
                tok, سطر_آغاز, badchar - سطر_آغاز + 1, lineno,
                -1, -1,
                "Non-UTF-8 code starting with '\\x%.2x'"
                "%s%V on line %i, "
                "but no encoding declared; "
                "see https://peps.python.org/pep-0263/ for details",
                (غیرعلامتی حرف)*badchar,
                tok->filename ? " in file " : "", tok->filename, "",
                lineno);
        واپس 0;
    }
    واپس 1;
}


/* ############## DEBUGGING STUFF ############## */

#اگر_تعریف_شدہ Py_DEBUG
خالی
_PyTokenizer_print_escape(FILE *f, ثابت حرف *s, Py_ssize_t سائز)
{
    اگر (s == تہی) {
        fputs("NULL", f);
        واپس;
    }
    putc('"', f);
    جب (سائز-- > 0) {
        غیرعلامتی حرف c = *s++;
        انتخاب (c) {
            حالت '\n': fputs("\\n", f); توڑو;
            حالت '\r': fputs("\\r", f); توڑو;
            حالت '\t': fputs("\\t", f); توڑو;
            حالت '\f': fputs("\\f", f); توڑو;
            حالت '\'': fputs("\\'", f); توڑو;
            حالت '"': fputs("\\\"", f); توڑو;
            طےشدہ:
                اگر (0x20 <= c && c <= 0x7f)
                    putc(c, f);
                ورنہ
                    fprintf(f, "\\x%02x", c);
        }
    }
    putc('"', f);
}
#اگر_ختم
