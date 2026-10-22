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
byte_کانو_منزل_char_کانو(ثابت حرف *سٽ, صحيح byte_کانو)
{
    صحيح char_کانو = 1;
    لكل (صحيح i = 0; i < byte_کانو - 1; i++) {
        إذا ((سٽ[i] & 0xC0) != 0x80) {
            char_کانو++;
        }
    }
    أرجع char_کانو;
}

ساكن صحيح
_syntaxerror_range(بنية tok_state *tok, ثابت حرف *سٽ,
                   Py_ssize_t cursor, صحيح lineno, ثابت حرف *format,
                   صحيح کانو_وٿي, صحيح پڇاڙي_کانو_وٿي,
                   va_list vargs)
{
    // In release builds, we don't want to overwrite a previous error, but in debug builds we
    // want to fail if we are not doing it so we can fix it.
    أكد(tok->پورو != E_ERROR);
    إذا (tok->پورو == E_ERROR) {
        أرجع ERRORTOKEN;
    }
    PyObject *errmsg, *errtext, *دليلون;
    errmsg = PyUnicode_FromFormatV(format, vargs);
    إذا (!errmsg) {
        اذهب error;
    }

    errtext = PyUnicode_DecodeUTF8(سٽ, cursor,
                                   "replace");
    إذا (!errtext) {
        اذهب error;
    }

    إذا (کانو_وٿي == -1) {
        کانو_وٿي = (صحيح)Py_Unicode_GET_ڊيگهائي(errtext);
    }
    وإلا إذا (کانو_وٿي > 0) {
        کانو_وٿي = byte_کانو_منزل_char_کانو(سٽ, کانو_وٿي);
    }
    إذا (پڇاڙي_کانو_وٿي == -1) {
        پڇاڙي_کانو_وٿي = کانو_وٿي;
    }
    وإلا إذا (پڇاڙي_کانو_وٿي > 0) {
        پڇاڙي_کانو_وٿي = byte_کانو_منزل_char_کانو(سٽ, پڇاڙي_کانو_وٿي);
    }

    Py_ssize_t سٽ_len = strcspn(سٽ, "\n");
    إذا (سٽ_len != cursor) {
        Py_DECREF(errtext);
        errtext = PyUnicode_DecodeUTF8(سٽ, سٽ_len,
                                       "replace");
    }
    إذا (!errtext) {
        اذهب error;
    }

    دليلون = Py_Build_قدر("(O(OiiNii))", errmsg,
                         tok->filename ? tok->filename : Py_None,
                         lineno, کانو_وٿي, errtext,
                         lineno, پڇاڙي_کانو_وٿي);
    إذا (دليلون) {
        Py_Err_سيٽ_Object(PyExc_SyntaxError, دليلون);
        Py_DECREF(دليلون);
    }

error:
    Py_XDECREF(errmsg);
    tok->پورو = E_ERROR;
    أرجع ERRORTOKEN;
}

صحيح
_PyTokenizer_syntaxerror(بنية tok_state *tok, ثابت حرف *format, ...)
{
    // These errors are cleaned on startup. Todo: Fix it.
    va_list vargs;
    va_شروعات(vargs, format);
    صحيح ret = _syntaxerror_range(
        tok, Py_Tok_ذريعو_اشارو(&tok->ذريعو, tok->سٽ_شروعات),
        tok->cur - tok->سٽ_شروعات, tok->lineno, format, -1, -1, vargs);
    va_پڇاڙي(vargs);
    أرجع ret;
}

صحيح
Py_Tokenizer_syntaxerror_معلوم_range(بنية tok_state *tok,
                        صحيح کانو_وٿي, صحيح پڇاڙي_کانو_وٿي,
                        ثابت حرف *format, ...)
{
    va_list vargs;
    va_شروعات(vargs, format);
    صحيح ret = _syntaxerror_range(
        tok, Py_Tok_ذريعو_اشارو(&tok->ذريعو, tok->سٽ_شروعات),
        tok->cur - tok->سٽ_شروعات, tok->lineno,
        format, کانو_وٿي, پڇاڙي_کانو_وٿي, vargs);
    va_پڇاڙي(vargs);
    أرجع ret;
}

صحيح
_PyTokenizer_syntaxerror_at(بنية tok_state *tok, ثابت حرف *سٽ,
                             Py_ssize_t cursor, صحيح lineno,
                             صحيح کانو_وٿي, صحيح پڇاڙي_کانو_وٿي,
                             ثابت حرف *format, ...)
{
    va_list vargs;
    va_شروعات(vargs, format);
    صحيح ret = _syntaxerror_range(tok, سٽ, cursor, lineno, format,
                                کانو_وٿي, پڇاڙي_کانو_وٿي, vargs);
    va_پڇاڙي(vargs);
    أرجع ret;
}

صحيح
_PyTokenizer_indenterror(بنية tok_state *tok)
{
    tok->پورو = E_TABSPACE;
    tok->cur = tok->inp;
    أرجع ERRORTOKEN;
}

صحيح
_PyTokenizer_warn_invalid_escape_sequence(بنية tok_state *tok, صحيح پهريون_invalid_escape_char)
{
    PyObject *نياپو = PyUnicode_FromFormat(
        "\"\\%c\" is an invalid escape sequence. "
        "Such sequences will not work in the future. "
        "Did you mean \"\\\\%c\"? A raw string is also an option.",
        (حرف) پهريون_invalid_escape_char,
        (حرف) پهريون_invalid_escape_char
    );

    إذا (نياپو == عدم) {
        أرجع -1;
    }

    إذا (PyErr_WarnExplicitObject(PyExc_SyntaxWarning, نياپو, tok->filename,
                                 tok->lineno, tok->module, عدم) < 0) {
        Py_DECREF(نياپو);

        إذا (PyErr_ExceptionMatches(PyExc_SyntaxWarning)) {
            /* Replace the SyntaxWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            أرجع _PyTokenizer_syntaxerror(tok,
                "\"\\%c\" is an invalid escape sequence. "
                "Did you mean \"\\\\%c\"? A raw string is also an option.",
                (حرف) پهريون_invalid_escape_char,
                (حرف) پهريون_invalid_escape_char);
        }

        أرجع -1;
    }

    Py_DECREF(نياپو);
    أرجع 0;
}

فراغ
_PyTokenizer_raise_init_error(PyObject *filename)
{
    إذا (!(PyErr_ExceptionMatches(PyExc_LookupError)
          || PyErr_ExceptionMatches(PyExc_SyntaxError)
          || PyErr_ExceptionMatches(Py_Exc_قدر_Error)
          || PyErr_ExceptionMatches(PyExc_UnicodeDecodeError))) {
        أرجع;
    }
    PyObject *errstr = عدم;
    PyObject *tuple = عدم;
    PyObject *type;
    PyObject *قدر;
    PyObject *tback;
    PyErr_Fetch(&type, &قدر, &tback);
    إذا (PyErr_GivenExceptionMatches(قدر, PyExc_SyntaxError)) {
        إذا (Py_Object_سيٽ_Attr(قدر, &_Py_ID(filename), filename)) {
            اذهب error;
        }
        PyErr_Restore(type, قدر, tback);
        أرجع;
    }
    errstr = PyObject_Str(قدر);
    إذا (!errstr) {
        اذهب error;
    }

    PyObject *tmp = Py_Build_قدر("(OiiO)", filename, 0, -1, Py_None);
    إذا (!tmp) {
        اذهب error;
    }

    tuple = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    إذا (!tuple) {
        اذهب error;
    }
    Py_Err_سيٽ_Object(PyExc_SyntaxError, tuple);

error:
    Py_XDECREF(type);
    Py_XDECREF(قدر);
    Py_XDECREF(tback);
    Py_XDECREF(errstr);
    Py_XDECREF(tuple);
}

صحيح
_PyTokenizer_parser_warn(بنية tok_state *tok, PyObject *category, ثابت حرف *format, ...)
{
    PyObject *errmsg;
    va_list vargs;
    va_شروعات(vargs, format);
    errmsg = PyUnicode_FromFormatV(format, vargs);
    va_پڇاڙي(vargs);
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
    أرجع 0;

error:
    Py_XDECREF(errmsg);
    tok->پورو = E_ERROR;
    أرجع -1;
}


/* Check whether the characters at s start a valid
   UTF-8 sequence. Return the number of characters forming
   the sequence if yes, 0 if not.  The special cases match
   those in stringlib/codecs.h:utf8_decode.
*/
ساكن صحيح
valid_utf8(ثابت بنانشان حرف* s)
{
    صحيح expected = 0;
    صحيح ڊيگهائي;
    إذا (*s < 0x80) {
        /* single-byte code */
        أرجع 1;
    }
    وإلا إذا (*s < 0xE0) {
        /* \xC2\x80-\xDF\xBF -- 0080-07FF */
        إذا (*s < 0xC2) {
            /* invalid sequence
               \x80-\xBF -- continuation byte
               \xC0-\xC1 -- fake 0000-007F */
            أرجع 0;
        }
        expected = 1;
    }
    وإلا إذا (*s < 0xF0) {
        /* \xE0\xA0\x80-\xEF\xBF\xBF -- 0800-FFFF */
        إذا (*s == 0xE0 && *(s + 1) < 0xA0) {
            /* invalid sequence
               \xE0\x80\x80-\xE0\x9F\xBF -- fake 0000-0800 */
            أرجع 0;
        }
        وإلا إذا (*s == 0xED && *(s + 1) >= 0xA0) {
            /* Decoding UTF-8 sequences in range \xED\xA0\x80-\xED\xBF\xBF
               will result in surrogates in range D800-DFFF. Surrogates are
               not valid UTF-8 so they are rejected.
               See https://www.unicode.org/versions/Unicode5.2.0/ch03.pdf
               (table 3-7) and http://www.rfc-editor.org/rfc/rfc3629.txt */
            أرجع 0;
        }
        expected = 2;
    }
    وإلا إذا (*s < 0xF5) {
        /* \xF0\x90\x80\x80-\xF4\x8F\xBF\xBF -- 10000-10FFFF */
        إذا (*(s + 1) < 0x90 ? *s == 0xF0 : *s == 0xF4) {
            /* invalid sequence -- one of:
               \xF0\x80\x80\x80-\xF0\x8F\xBF\xBF -- fake 0000-FFFF
               \xF4\x90\x80\x80- -- 110000- overflow */
            أرجع 0;
        }
        expected = 3;
    }
    وإلا {
        /* invalid start byte */
        أرجع 0;
    }
    ڊيگهائي = expected + 1;
    لكل (صحيح i = 1; i <= expected; i++) {
        إذا (s[i] < 0x80 || s[i] >= 0xC0) {
            أرجع 0;
        }
    }
    أرجع ڊيگهائي;
}

صحيح
_PyTokenizer_ensure_utf8(ثابت حرف *سٽ, بنية tok_state *tok, صحيح lineno)
{
    ثابت حرف *badchar = عدم;
    ثابت حرف *c;
    صحيح ڊيگهائي;
    ثابت حرف *سٽ_شروعات = سٽ;
    لكل (c = سٽ; *c; c += ڊيگهائي) {
        إذا (!(ڊيگهائي = valid_utf8((ثابت بنانشان حرف *)c))) {
            badchar = c;
            اكسر;
        }
        إذا (*c == '\n') {
            lineno++;
            سٽ_شروعات = c + 1;
        }
    }
    إذا (badchar) {
        _PyTokenizer_syntaxerror_at(
                tok, سٽ_شروعات, badchar - سٽ_شروعات + 1, lineno,
                -1, -1,
                "Non-UTF-8 code starting with '\\x%.2x'"
                "%s%V on line %i, "
                "but no encoding declared; "
                "see https://peps.python.org/pep-0263/ for details",
                (بنانشان حرف)*badchar,
                tok->filename ? " in file " : "", tok->filename, "",
                lineno);
        أرجع 0;
    }
    أرجع 1;
}


/* ############## DEBUGGING STUFF ############## */

#جيڪڏهن_وصف_ٿيل Py_DEBUG
فراغ
_PyTokenizer_print_escape(FILE *f, ثابت حرف *s, Py_ssize_t سائيز)
{
    إذا (s == عدم) {
        fputs("NULL", f);
        أرجع;
    }
    putc('"', f);
    طالما (سائيز-- > 0) {
        بنانشان حرف c = *s++;
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
#جيڪڏهن_پورو
