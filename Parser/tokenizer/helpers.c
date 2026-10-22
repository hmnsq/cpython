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
byte_ستون_منزل_char_ستون(ثابت حرف *کرښه, صحيح byte_ستون)
{
    صحيح char_ستون = 1;
    لكل (صحيح i = 0; i < byte_ستون - 1; i++) {
        إذا ((کرښه[i] & 0xC0) != 0x80) {
            char_ستون++;
        }
    }
    أرجع char_ستون;
}

ساكن صحيح
_syntaxerror_range(بنية tok_state *tok, ثابت حرف *کرښه,
                   Py_ssize_t cursor, صحيح lineno, ثابت حرف *format,
                   صحيح ستون_دننه_واټن, صحيح پای_ټکی_ستون_دننه_واټن,
                   va_list vargs)
{
    // In release builds, we don't want to overwrite a previous error, but in debug builds we
    // want to fail if we are not doing it so we can fix it.
    أكد(tok->بشپړ != E_ERROR);
    إذا (tok->بشپړ == E_ERROR) {
        أرجع ERRORTOKEN;
    }
    PyObject *errmsg, *errtext, *دلیلونه;
    errmsg = PyUnicode_FromFormatV(format, vargs);
    إذا (!errmsg) {
        اذهب error;
    }

    errtext = PyUnicode_DecodeUTF8(کرښه, cursor,
                                   "replace");
    إذا (!errtext) {
        اذهب error;
    }

    إذا (ستون_دننه_واټن == -1) {
        ستون_دننه_واټن = (صحيح)Py_Unicode_GET_اوږدتیا(errtext);
    }
    وإلا إذا (ستون_دننه_واټن > 0) {
        ستون_دننه_واټن = byte_ستون_منزل_char_ستون(کرښه, ستون_دننه_واټن);
    }
    إذا (پای_ټکی_ستون_دننه_واټن == -1) {
        پای_ټکی_ستون_دننه_واټن = ستون_دننه_واټن;
    }
    وإلا إذا (پای_ټکی_ستون_دننه_واټن > 0) {
        پای_ټکی_ستون_دننه_واټن = byte_ستون_منزل_char_ستون(کرښه, پای_ټکی_ستون_دننه_واټن);
    }

    Py_ssize_t کرښه_len = strcspn(کرښه, "\n");
    إذا (کرښه_len != cursor) {
        Py_DECREF(errtext);
        errtext = PyUnicode_DecodeUTF8(کرښه, کرښه_len,
                                       "replace");
    }
    إذا (!errtext) {
        اذهب error;
    }

    دلیلونه = Py_Build_ارزښت("(O(OiiNii))", errmsg,
                         tok->filename ? tok->filename : Py_None,
                         lineno, ستون_دننه_واټن, errtext,
                         lineno, پای_ټکی_ستون_دننه_واټن);
    إذا (دلیلونه) {
        Py_Err_سېټ_Object(PyExc_SyntaxError, دلیلونه);
        Py_DECREF(دلیلونه);
    }

error:
    Py_XDECREF(errmsg);
    tok->بشپړ = E_ERROR;
    أرجع ERRORTOKEN;
}

صحيح
_PyTokenizer_syntaxerror(بنية tok_state *tok, ثابت حرف *format, ...)
{
    // These errors are cleaned on startup. Todo: Fix it.
    va_list vargs;
    va_پیل(vargs, format);
    صحيح ret = _syntaxerror_range(
        tok, Py_Tok_سرچینه_اشاره_ګر(&tok->سرچینه, tok->کرښه_پیل),
        tok->cur - tok->کرښه_پیل, tok->lineno, format, -1, -1, vargs);
    va_پای_ټکی(vargs);
    أرجع ret;
}

صحيح
Py_Tokenizer_syntaxerror_معلوم_range(بنية tok_state *tok,
                        صحيح ستون_دننه_واټن, صحيح پای_ټکی_ستون_دننه_واټن,
                        ثابت حرف *format, ...)
{
    va_list vargs;
    va_پیل(vargs, format);
    صحيح ret = _syntaxerror_range(
        tok, Py_Tok_سرچینه_اشاره_ګر(&tok->سرچینه, tok->کرښه_پیل),
        tok->cur - tok->کرښه_پیل, tok->lineno,
        format, ستون_دننه_واټن, پای_ټکی_ستون_دننه_واټن, vargs);
    va_پای_ټکی(vargs);
    أرجع ret;
}

صحيح
_PyTokenizer_syntaxerror_at(بنية tok_state *tok, ثابت حرف *کرښه,
                             Py_ssize_t cursor, صحيح lineno,
                             صحيح ستون_دننه_واټن, صحيح پای_ټکی_ستون_دننه_واټن,
                             ثابت حرف *format, ...)
{
    va_list vargs;
    va_پیل(vargs, format);
    صحيح ret = _syntaxerror_range(tok, کرښه, cursor, lineno, format,
                                ستون_دننه_واټن, پای_ټکی_ستون_دننه_واټن, vargs);
    va_پای_ټکی(vargs);
    أرجع ret;
}

صحيح
_PyTokenizer_indenterror(بنية tok_state *tok)
{
    tok->بشپړ = E_TABSPACE;
    tok->cur = tok->inp;
    أرجع ERRORTOKEN;
}

صحيح
_PyTokenizer_warn_invalid_escape_sequence(بنية tok_state *tok, صحيح لومړی_invalid_escape_char)
{
    PyObject *پیغام = PyUnicode_FromFormat(
        "\"\\%c\" is an invalid escape sequence. "
        "Such sequences will not work in the future. "
        "Did you mean \"\\\\%c\"? A raw string is also an option.",
        (حرف) لومړی_invalid_escape_char,
        (حرف) لومړی_invalid_escape_char
    );

    إذا (پیغام == عدم) {
        أرجع -1;
    }

    إذا (PyErr_WarnExplicitObject(PyExc_SyntaxWarning, پیغام, tok->filename,
                                 tok->lineno, tok->module, عدم) < 0) {
        Py_DECREF(پیغام);

        إذا (PyErr_ExceptionMatches(PyExc_SyntaxWarning)) {
            /* Replace the SyntaxWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            أرجع _PyTokenizer_syntaxerror(tok,
                "\"\\%c\" is an invalid escape sequence. "
                "Did you mean \"\\\\%c\"? A raw string is also an option.",
                (حرف) لومړی_invalid_escape_char,
                (حرف) لومړی_invalid_escape_char);
        }

        أرجع -1;
    }

    Py_DECREF(پیغام);
    أرجع 0;
}

فراغ
_PyTokenizer_raise_init_error(PyObject *filename)
{
    إذا (!(PyErr_ExceptionMatches(PyExc_LookupError)
          || PyErr_ExceptionMatches(PyExc_SyntaxError)
          || PyErr_ExceptionMatches(Py_Exc_ارزښت_Error)
          || PyErr_ExceptionMatches(PyExc_UnicodeDecodeError))) {
        أرجع;
    }
    PyObject *errstr = عدم;
    PyObject *tuple = عدم;
    PyObject *type;
    PyObject *ارزښت;
    PyObject *tback;
    PyErr_Fetch(&type, &ارزښت, &tback);
    إذا (PyErr_GivenExceptionMatches(ارزښت, PyExc_SyntaxError)) {
        إذا (Py_Object_سېټ_Attr(ارزښت, &_Py_ID(filename), filename)) {
            اذهب error;
        }
        PyErr_Restore(type, ارزښت, tback);
        أرجع;
    }
    errstr = PyObject_Str(ارزښت);
    إذا (!errstr) {
        اذهب error;
    }

    PyObject *tmp = Py_Build_ارزښت("(OiiO)", filename, 0, -1, Py_None);
    إذا (!tmp) {
        اذهب error;
    }

    tuple = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    إذا (!tuple) {
        اذهب error;
    }
    Py_Err_سېټ_Object(PyExc_SyntaxError, tuple);

error:
    Py_XDECREF(type);
    Py_XDECREF(ارزښت);
    Py_XDECREF(tback);
    Py_XDECREF(errstr);
    Py_XDECREF(tuple);
}

صحيح
_PyTokenizer_parser_warn(بنية tok_state *tok, PyObject *category, ثابت حرف *format, ...)
{
    PyObject *errmsg;
    va_list vargs;
    va_پیل(vargs, format);
    errmsg = PyUnicode_FromFormatV(format, vargs);
    va_پای_ټکی(vargs);
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
    tok->بشپړ = E_ERROR;
    أرجع -1;
}


/* Check whether the characters at s start a valid
   UTF-8 sequence. Return the number of characters forming
   the sequence if yes, 0 if not.  The special cases match
   those in stringlib/codecs.h:utf8_decode.
*/
ساكن صحيح
valid_utf8(ثابت بینښه حرف* s)
{
    صحيح expected = 0;
    صحيح اوږدتیا;
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
    اوږدتیا = expected + 1;
    لكل (صحيح i = 1; i <= expected; i++) {
        إذا (s[i] < 0x80 || s[i] >= 0xC0) {
            أرجع 0;
        }
    }
    أرجع اوږدتیا;
}

صحيح
_PyTokenizer_ensure_utf8(ثابت حرف *کرښه, بنية tok_state *tok, صحيح lineno)
{
    ثابت حرف *badchar = عدم;
    ثابت حرف *c;
    صحيح اوږدتیا;
    ثابت حرف *کرښه_پیل = کرښه;
    لكل (c = کرښه; *c; c += اوږدتیا) {
        إذا (!(اوږدتیا = valid_utf8((ثابت بینښه حرف *)c))) {
            badchar = c;
            اكسر;
        }
        إذا (*c == '\n') {
            lineno++;
            کرښه_پیل = c + 1;
        }
    }
    إذا (badchar) {
        _PyTokenizer_syntaxerror_at(
                tok, کرښه_پیل, badchar - کرښه_پیل + 1, lineno,
                -1, -1,
                "Non-UTF-8 code starting with '\\x%.2x'"
                "%s%V on line %i, "
                "but no encoding declared; "
                "see https://peps.python.org/pep-0263/ for details",
                (بینښه حرف)*badchar,
                tok->filename ? " in file " : "", tok->filename, "",
                lineno);
        أرجع 0;
    }
    أرجع 1;
}


/* ############## DEBUGGING STUFF ############## */

#که_تعریف_شوی Py_DEBUG
فراغ
_PyTokenizer_print_escape(FILE *f, ثابت حرف *s, Py_ssize_t اندازه)
{
    إذا (s == عدم) {
        fputs("NULL", f);
        أرجع;
    }
    putc('"', f);
    طالما (اندازه-- > 0) {
        بینښه حرف c = *s++;
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
#د_که_پای
