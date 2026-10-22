#अन्तर्भाव "Python.h"
#अन्तर्भाव "errcode.h"
#अन्तर्भाव "pycore_runtime.h"       // _Py_ID()
#अन्तर्भाव "pycore_token.h"
#अन्तर्भाव "pycore_tuple.h"         // _PyTuple_FromPair

#अन्तर्भाव "../lexer/state.h"


/* ############## ERRORS ############## */

/* Convert a 1-based column in bytes into a 1-based column in characters.
   The line is UTF-8 encoded, so it is enough to skip continuation bytes. */
स्थिर पूर्णांक
byte_col_to_char_col(स्थिरांक अक्षर *ओळ, पूर्णांक byte_col)
{
    पूर्णांक char_col = 1;
    साठी (पूर्णांक i = 0; i < byte_col - 1; i++) {
        जर ((ओळ[i] & 0xC0) != 0x80) {
            char_col++;
        }
    }
    परत char_col;
}

स्थिर पूर्णांक
_syntaxerror_range(संरचना tok_state *tok, स्थिरांक अक्षर *ओळ,
                   Py_ssize_t cursor, पूर्णांक lineno, स्थिरांक अक्षर *format,
                   पूर्णांक col_offset, पूर्णांक end_col_offset,
                   बहु_सूची vargs)
{
    // In release builds, we don't want to overwrite a previous error, but in debug builds we
    // want to fail if we are not doing it so we can fix it.
    प्रतिज्ञा(tok->done != E_ERROR);
    जर (tok->done == E_ERROR) {
        परत ERRORTOKEN;
    }
    PyObject *errmsg, *errtext, *args;
    errmsg = PyUnicode_FromFormatV(format, vargs);
    जर (!errmsg) {
        जा error;
    }

    errtext = PyUnicode_DecodeUTF8(ओळ, cursor,
                                   "replace");
    जर (!errtext) {
        जा error;
    }

    जर (col_offset == -1) {
        col_offset = (पूर्णांक)PyUnicode_GET_LENGTH(errtext);
    }
    अन्यथा जर (col_offset > 0) {
        col_offset = byte_col_to_char_col(ओळ, col_offset);
    }
    जर (end_col_offset == -1) {
        end_col_offset = col_offset;
    }
    अन्यथा जर (end_col_offset > 0) {
        end_col_offset = byte_col_to_char_col(ओळ, end_col_offset);
    }

    Py_ssize_t ओळ_len = म_खोज(ओळ, "\n");
    जर (ओळ_len != cursor) {
        Py_DECREF(errtext);
        errtext = PyUnicode_DecodeUTF8(ओळ, ओळ_len,
                                       "replace");
    }
    जर (!errtext) {
        जा error;
    }

    args = Py_Build_मूल्य("(O(OiiNii))", errmsg,
                         tok->filename ? tok->filename : Py_None,
                         lineno, col_offset, errtext,
                         lineno, end_col_offset);
    जर (args) {
        PyErr_SetObject(PyExc_SyntaxError, args);
        Py_DECREF(args);
    }

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    परत ERRORTOKEN;
}

पूर्णांक
_PyTokenizer_syntaxerror(संरचना tok_state *tok, स्थिरांक अक्षर *format, ...)
{
    // These errors are cleaned on startup. Todo: Fix it.
    बहु_सूची vargs;
    बहु_शुरू(vargs, format);
    पूर्णांक ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->ओळ_start),
        tok->cur - tok->ओळ_start, tok->lineno, format, -1, -1, vargs);
    बहु_पूर्ण(vargs);
    परत ret;
}

पूर्णांक
_PyTokenizer_syntaxerror_known_range(संरचना tok_state *tok,
                        पूर्णांक col_offset, पूर्णांक end_col_offset,
                        स्थिरांक अक्षर *format, ...)
{
    बहु_सूची vargs;
    बहु_शुरू(vargs, format);
    पूर्णांक ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->ओळ_start),
        tok->cur - tok->ओळ_start, tok->lineno,
        format, col_offset, end_col_offset, vargs);
    बहु_पूर्ण(vargs);
    परत ret;
}

पूर्णांक
_PyTokenizer_syntaxerror_at(संरचना tok_state *tok, स्थिरांक अक्षर *ओळ,
                             Py_ssize_t cursor, पूर्णांक lineno,
                             पूर्णांक col_offset, पूर्णांक end_col_offset,
                             स्थिरांक अक्षर *format, ...)
{
    बहु_सूची vargs;
    बहु_शुरू(vargs, format);
    पूर्णांक ret = _syntaxerror_range(tok, ओळ, cursor, lineno, format,
                                col_offset, end_col_offset, vargs);
    बहु_पूर्ण(vargs);
    परत ret;
}

पूर्णांक
_PyTokenizer_indenterror(संरचना tok_state *tok)
{
    tok->done = E_TABSPACE;
    tok->cur = tok->inp;
    परत ERRORTOKEN;
}

पूर्णांक
_PyTokenizer_warn_invalid_escape_sequence(संरचना tok_state *tok, पूर्णांक first_invalid_escape_char)
{
    PyObject *संदेश = PyUnicode_FromFormat(
        "\"\\%c\" is an invalid escape sequence. "
        "Such sequences will not work in the future. "
        "Did you mean \"\\\\%c\"? A raw string is also an option.",
        (अक्षर) first_invalid_escape_char,
        (अक्षर) first_invalid_escape_char
    );

    जर (संदेश == रिक्त) {
        परत -1;
    }

    जर (PyErr_WarnExplicitObject(PyExc_SyntaxWarning, संदेश, tok->filename,
                                 tok->lineno, tok->module, रिक्त) < 0) {
        Py_DECREF(संदेश);

        जर (PyErr_ExceptionMatches(PyExc_SyntaxWarning)) {
            /* Replace the SyntaxWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            परत _PyTokenizer_syntaxerror(tok,
                "\"\\%c\" is an invalid escape sequence. "
                "Did you mean \"\\\\%c\"? A raw string is also an option.",
                (अक्षर) first_invalid_escape_char,
                (अक्षर) first_invalid_escape_char);
        }

        परत -1;
    }

    Py_DECREF(संदेश);
    परत 0;
}

शून्य
_PyTokenizer_raise_init_error(PyObject *filename)
{
    जर (!(PyErr_ExceptionMatches(PyExc_LookupError)
          || PyErr_ExceptionMatches(PyExc_SyntaxError)
          || PyErr_ExceptionMatches(Py_Exc_मूल्य_Error)
          || PyErr_ExceptionMatches(PyExc_UnicodeDecodeError))) {
        परत;
    }
    PyObject *errstr = रिक्त;
    PyObject *tuple = रिक्त;
    PyObject *type;
    PyObject *मूल्य;
    PyObject *tback;
    PyErr_Fetch(&type, &मूल्य, &tback);
    जर (PyErr_GivenExceptionMatches(मूल्य, PyExc_SyntaxError)) {
        जर (PyObject_SetAttr(मूल्य, &_Py_ID(filename), filename)) {
            जा error;
        }
        PyErr_Restore(type, मूल्य, tback);
        परत;
    }
    errstr = PyObject_Str(मूल्य);
    जर (!errstr) {
        जा error;
    }

    PyObject *tmp = Py_Build_मूल्य("(OiiO)", filename, 0, -1, Py_None);
    जर (!tmp) {
        जा error;
    }

    tuple = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    जर (!tuple) {
        जा error;
    }
    PyErr_SetObject(PyExc_SyntaxError, tuple);

error:
    Py_XDECREF(type);
    Py_XDECREF(मूल्य);
    Py_XDECREF(tback);
    Py_XDECREF(errstr);
    Py_XDECREF(tuple);
}

पूर्णांक
_PyTokenizer_parser_warn(संरचना tok_state *tok, PyObject *category, स्थिरांक अक्षर *format, ...)
{
    PyObject *errmsg;
    बहु_सूची vargs;
    बहु_शुरू(vargs, format);
    errmsg = PyUnicode_FromFormatV(format, vargs);
    बहु_पूर्ण(vargs);
    जर (!errmsg) {
        जा error;
    }

    जर (PyErr_WarnExplicitObject(category, errmsg, tok->filename,
                                 tok->lineno, tok->module, रिक्त) < 0) {
        जर (PyErr_ExceptionMatches(category)) {
            /* Replace the DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();
            _PyTokenizer_syntaxerror(tok, "%U", errmsg);
        }
        जा error;
    }
    Py_DECREF(errmsg);
    परत 0;

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    परत -1;
}


/* Check whether the characters at s start a valid
   UTF-8 sequence. Return the number of characters forming
   the sequence if yes, 0 if not.  The special cases match
   those in stringlib/codecs.h:utf8_decode.
*/
स्थिर पूर्णांक
valid_utf8(स्थिरांक अचिन्हित अक्षर* s)
{
    पूर्णांक expected = 0;
    पूर्णांक length;
    जर (*s < 0x80) {
        /* single-byte code */
        परत 1;
    }
    अन्यथा जर (*s < 0xE0) {
        /* \xC2\x80-\xDF\xBF -- 0080-07FF */
        जर (*s < 0xC2) {
            /* invalid sequence
               \x80-\xBF -- continuation byte
               \xC0-\xC1 -- fake 0000-007F */
            परत 0;
        }
        expected = 1;
    }
    अन्यथा जर (*s < 0xF0) {
        /* \xE0\xA0\x80-\xEF\xBF\xBF -- 0800-FFFF */
        जर (*s == 0xE0 && *(s + 1) < 0xA0) {
            /* invalid sequence
               \xE0\x80\x80-\xE0\x9F\xBF -- fake 0000-0800 */
            परत 0;
        }
        अन्यथा जर (*s == 0xED && *(s + 1) >= 0xA0) {
            /* Decoding UTF-8 sequences in range \xED\xA0\x80-\xED\xBF\xBF
               will result in surrogates in range D800-DFFF. Surrogates are
               not valid UTF-8 so they are rejected.
               See https://www.unicode.org/versions/Unicode5.2.0/ch03.pdf
               (table 3-7) and http://www.rfc-editor.org/rfc/rfc3629.txt */
            परत 0;
        }
        expected = 2;
    }
    अन्यथा जर (*s < 0xF5) {
        /* \xF0\x90\x80\x80-\xF4\x8F\xBF\xBF -- 10000-10FFFF */
        जर (*(s + 1) < 0x90 ? *s == 0xF0 : *s == 0xF4) {
            /* invalid sequence -- one of:
               \xF0\x80\x80\x80-\xF0\x8F\xBF\xBF -- fake 0000-FFFF
               \xF4\x90\x80\x80- -- 110000- overflow */
            परत 0;
        }
        expected = 3;
    }
    अन्यथा {
        /* invalid start byte */
        परत 0;
    }
    length = expected + 1;
    साठी (पूर्णांक i = 1; i <= expected; i++) {
        जर (s[i] < 0x80 || s[i] >= 0xC0) {
            परत 0;
        }
    }
    परत length;
}

पूर्णांक
_PyTokenizer_ensure_utf8(स्थिरांक अक्षर *ओळ, संरचना tok_state *tok, पूर्णांक lineno)
{
    स्थिरांक अक्षर *badchar = रिक्त;
    स्थिरांक अक्षर *c;
    पूर्णांक length;
    स्थिरांक अक्षर *ओळ_start = ओळ;
    साठी (c = ओळ; *c; c += length) {
        जर (!(length = valid_utf8((स्थिरांक अचिन्हित अक्षर *)c))) {
            badchar = c;
            थांबा;
        }
        जर (*c == '\n') {
            lineno++;
            ओळ_start = c + 1;
        }
    }
    जर (badchar) {
        _PyTokenizer_syntaxerror_at(
                tok, ओळ_start, badchar - ओळ_start + 1, lineno,
                -1, -1,
                "Non-UTF-8 code starting with '\\x%.2x'"
                "%s%V on line %i, "
                "but no encoding declared; "
                "see https://peps.python.org/pep-0263/ for details",
                (अचिन्हित अक्षर)*badchar,
                tok->filename ? " in file " : "", tok->filename, "",
                lineno);
        परत 0;
    }
    परत 1;
}


/* ############## DEBUGGING STUFF ############## */

#ifdef Py_DEBUG
शून्य
_PyTokenizer_print_escape(खाता *f, स्थिरांक अक्षर *s, Py_ssize_t size)
{
    जर (s == रिक्त) {
        ख_माला_दो("NULL", f);
        परत;
    }
    अ_दो('"', f);
    पर्यंत (size-- > 0) {
        अचिन्हित अक्षर c = *s++;
        निवड (c) {
            स्थिती '\n': ख_माला_दो("\\n", f); थांबा;
            स्थिती '\r': ख_माला_दो("\\r", f); थांबा;
            स्थिती '\t': ख_माला_दो("\\t", f); थांबा;
            स्थिती '\f': ख_माला_दो("\\f", f); थांबा;
            स्थिती '\'': ख_माला_दो("\\'", f); थांबा;
            स्थिती '"': ख_माला_दो("\\\"", f); थांबा;
            पूर्वनिर्धारित:
                जर (0x20 <= c && c <= 0x7f)
                    अ_दो(c, f);
                अन्यथा
                    ख_लिखो(f, "\\x%02x", c);
        }
    }
    अ_दो('"', f);
}
#endif
