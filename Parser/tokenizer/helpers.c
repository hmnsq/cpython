#অন্তর্ভাব "Python.h"
#অন্তর্ভাব "errcode.h"
#অন্তর্ভাব "pycore_runtime.h"       // _Py_ID()
#অন্তর্ভাব "pycore_token.h"
#অন্তর্ভাব "pycore_tuple.h"         // _PyTuple_FromPair

#অন্তর্ভাব "../lexer/state.h"


/* ############## ERRORS ############## */

/* Convert a 1-based column in bytes into a 1-based column in characters.
   The line is UTF-8 encoded, so it is enough to skip continuation bytes. */
স্থির পূর্ণসংখ্যা
byte_col_to_char_col(ধ্রুবক অক্ষর *সারি, পূর্ণসংখ্যা byte_col)
{
    পূর্ণসংখ্যা char_col = 1;
    জন্য (পূর্ণসংখ্যা i = 0; i < byte_col - 1; i++) {
        যদি ((সারি[i] & 0xC0) != 0x80) {
            char_col++;
        }
    }
    ফেরত char_col;
}

স্থির পূর্ণসংখ্যা
_syntaxerror_range(কাঠামো tok_state *tok, ধ্রুবক অক্ষর *সারি,
                   Py_ssize_t cursor, পূর্ণসংখ্যা lineno, ধ্রুবক অক্ষর *format,
                   পূর্ণসংখ্যা col_offset, পূর্ণসংখ্যা end_col_offset,
                   va_list vargs)
{
    // In release builds, we don't want to overwrite a previous error, but in debug builds we
    // want to fail if we are not doing it so we can fix it.
    প্রতিজ্ঞা(tok->done != E_ERROR);
    যদি (tok->done == E_ERROR) {
        ফেরত ERRORTOKEN;
    }
    PyObject *errmsg, *errtext, *args;
    errmsg = PyUnicode_FromFormatV(format, vargs);
    যদি (!errmsg) {
        যাও error;
    }

    errtext = PyUnicode_DecodeUTF8(সারি, cursor,
                                   "replace");
    যদি (!errtext) {
        যাও error;
    }

    যদি (col_offset == -1) {
        col_offset = (পূর্ণসংখ্যা)Py_Unicode_GET_দৈর্ঘ্য(errtext);
    }
    অন্যথা যদি (col_offset > 0) {
        col_offset = byte_col_to_char_col(সারি, col_offset);
    }
    যদি (end_col_offset == -1) {
        end_col_offset = col_offset;
    }
    অন্যথা যদি (end_col_offset > 0) {
        end_col_offset = byte_col_to_char_col(সারি, end_col_offset);
    }

    Py_ssize_t সারি_len = strcspn(সারি, "\n");
    যদি (সারি_len != cursor) {
        Py_DECREF(errtext);
        errtext = PyUnicode_DecodeUTF8(সারি, সারি_len,
                                       "replace");
    }
    যদি (!errtext) {
        যাও error;
    }

    args = Py_Build_মান("(O(OiiNii))", errmsg,
                         tok->filename ? tok->filename : Py_None,
                         lineno, col_offset, errtext,
                         lineno, end_col_offset);
    যদি (args) {
        PyErr_SetObject(PyExc_SyntaxError, args);
        Py_DECREF(args);
    }

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    ফেরত ERRORTOKEN;
}

পূর্ণসংখ্যা
_PyTokenizer_syntaxerror(কাঠামো tok_state *tok, ধ্রুবক অক্ষর *format, ...)
{
    // These errors are cleaned on startup. Todo: Fix it.
    va_list vargs;
    va_start(vargs, format);
    পূর্ণসংখ্যা ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->সারি_start),
        tok->cur - tok->সারি_start, tok->lineno, format, -1, -1, vargs);
    va_end(vargs);
    ফেরত ret;
}

পূর্ণসংখ্যা
_PyTokenizer_syntaxerror_known_range(কাঠামো tok_state *tok,
                        পূর্ণসংখ্যা col_offset, পূর্ণসংখ্যা end_col_offset,
                        ধ্রুবক অক্ষর *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    পূর্ণসংখ্যা ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->সারি_start),
        tok->cur - tok->সারি_start, tok->lineno,
        format, col_offset, end_col_offset, vargs);
    va_end(vargs);
    ফেরত ret;
}

পূর্ণসংখ্যা
_PyTokenizer_syntaxerror_at(কাঠামো tok_state *tok, ধ্রুবক অক্ষর *সারি,
                             Py_ssize_t cursor, পূর্ণসংখ্যা lineno,
                             পূর্ণসংখ্যা col_offset, পূর্ণসংখ্যা end_col_offset,
                             ধ্রুবক অক্ষর *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    পূর্ণসংখ্যা ret = _syntaxerror_range(tok, সারি, cursor, lineno, format,
                                col_offset, end_col_offset, vargs);
    va_end(vargs);
    ফেরত ret;
}

পূর্ণসংখ্যা
_PyTokenizer_indenterror(কাঠামো tok_state *tok)
{
    tok->done = E_TABSPACE;
    tok->cur = tok->inp;
    ফেরত ERRORTOKEN;
}

পূর্ণসংখ্যা
_PyTokenizer_warn_invalid_escape_sequence(কাঠামো tok_state *tok, পূর্ণসংখ্যা first_invalid_escape_char)
{
    PyObject *বার্তা = PyUnicode_FromFormat(
        "\"\\%c\" is an invalid escape sequence. "
        "Such sequences will not work in the future. "
        "Did you mean \"\\\\%c\"? A raw string is also an option.",
        (অক্ষর) first_invalid_escape_char,
        (অক্ষর) first_invalid_escape_char
    );

    যদি (বার্তা == ফাঁকা) {
        ফেরত -1;
    }

    যদি (PyErr_WarnExplicitObject(PyExc_SyntaxWarning, বার্তা, tok->filename,
                                 tok->lineno, tok->module, ফাঁকা) < 0) {
        Py_DECREF(বার্তা);

        যদি (PyErr_ExceptionMatches(PyExc_SyntaxWarning)) {
            /* Replace the SyntaxWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            ফেরত _PyTokenizer_syntaxerror(tok,
                "\"\\%c\" is an invalid escape sequence. "
                "Did you mean \"\\\\%c\"? A raw string is also an option.",
                (অক্ষর) first_invalid_escape_char,
                (অক্ষর) first_invalid_escape_char);
        }

        ফেরত -1;
    }

    Py_DECREF(বার্তা);
    ফেরত 0;
}

শূন্য
_PyTokenizer_raise_init_error(PyObject *filename)
{
    যদি (!(PyErr_ExceptionMatches(PyExc_LookupError)
          || PyErr_ExceptionMatches(PyExc_SyntaxError)
          || PyErr_ExceptionMatches(Py_Exc_মান_Error)
          || PyErr_ExceptionMatches(PyExc_UnicodeDecodeError))) {
        ফেরত;
    }
    PyObject *errstr = ফাঁকা;
    PyObject *tuple = ফাঁকা;
    PyObject *type;
    PyObject *মান;
    PyObject *tback;
    PyErr_Fetch(&type, &মান, &tback);
    যদি (PyErr_GivenExceptionMatches(মান, PyExc_SyntaxError)) {
        যদি (PyObject_SetAttr(মান, &_Py_ID(filename), filename)) {
            যাও error;
        }
        PyErr_Restore(type, মান, tback);
        ফেরত;
    }
    errstr = PyObject_Str(মান);
    যদি (!errstr) {
        যাও error;
    }

    PyObject *tmp = Py_Build_মান("(OiiO)", filename, 0, -1, Py_None);
    যদি (!tmp) {
        যাও error;
    }

    tuple = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    যদি (!tuple) {
        যাও error;
    }
    PyErr_SetObject(PyExc_SyntaxError, tuple);

error:
    Py_XDECREF(type);
    Py_XDECREF(মান);
    Py_XDECREF(tback);
    Py_XDECREF(errstr);
    Py_XDECREF(tuple);
}

পূর্ণসংখ্যা
_PyTokenizer_parser_warn(কাঠামো tok_state *tok, PyObject *category, ধ্রুবক অক্ষর *format, ...)
{
    PyObject *errmsg;
    va_list vargs;
    va_start(vargs, format);
    errmsg = PyUnicode_FromFormatV(format, vargs);
    va_end(vargs);
    যদি (!errmsg) {
        যাও error;
    }

    যদি (PyErr_WarnExplicitObject(category, errmsg, tok->filename,
                                 tok->lineno, tok->module, ফাঁকা) < 0) {
        যদি (PyErr_ExceptionMatches(category)) {
            /* Replace the DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();
            _PyTokenizer_syntaxerror(tok, "%U", errmsg);
        }
        যাও error;
    }
    Py_DECREF(errmsg);
    ফেরত 0;

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    ফেরত -1;
}


/* Check whether the characters at s start a valid
   UTF-8 sequence. Return the number of characters forming
   the sequence if yes, 0 if not.  The special cases match
   those in stringlib/codecs.h:utf8_decode.
*/
স্থির পূর্ণসংখ্যা
valid_utf8(ধ্রুবক unsigned অক্ষর* s)
{
    পূর্ণসংখ্যা expected = 0;
    পূর্ণসংখ্যা দৈর্ঘ্য;
    যদি (*s < 0x80) {
        /* single-byte code */
        ফেরত 1;
    }
    অন্যথা যদি (*s < 0xE0) {
        /* \xC2\x80-\xDF\xBF -- 0080-07FF */
        যদি (*s < 0xC2) {
            /* invalid sequence
               \x80-\xBF -- continuation byte
               \xC0-\xC1 -- fake 0000-007F */
            ফেরত 0;
        }
        expected = 1;
    }
    অন্যথা যদি (*s < 0xF0) {
        /* \xE0\xA0\x80-\xEF\xBF\xBF -- 0800-FFFF */
        যদি (*s == 0xE0 && *(s + 1) < 0xA0) {
            /* invalid sequence
               \xE0\x80\x80-\xE0\x9F\xBF -- fake 0000-0800 */
            ফেরত 0;
        }
        অন্যথা যদি (*s == 0xED && *(s + 1) >= 0xA0) {
            /* Decoding UTF-8 sequences in range \xED\xA0\x80-\xED\xBF\xBF
               will result in surrogates in range D800-DFFF. Surrogates are
               not valid UTF-8 so they are rejected.
               See https://www.unicode.org/versions/Unicode5.2.0/ch03.pdf
               (table 3-7) and http://www.rfc-editor.org/rfc/rfc3629.txt */
            ফেরত 0;
        }
        expected = 2;
    }
    অন্যথা যদি (*s < 0xF5) {
        /* \xF0\x90\x80\x80-\xF4\x8F\xBF\xBF -- 10000-10FFFF */
        যদি (*(s + 1) < 0x90 ? *s == 0xF0 : *s == 0xF4) {
            /* invalid sequence -- one of:
               \xF0\x80\x80\x80-\xF0\x8F\xBF\xBF -- fake 0000-FFFF
               \xF4\x90\x80\x80- -- 110000- overflow */
            ফেরত 0;
        }
        expected = 3;
    }
    অন্যথা {
        /* invalid start byte */
        ফেরত 0;
    }
    দৈর্ঘ্য = expected + 1;
    জন্য (পূর্ণসংখ্যা i = 1; i <= expected; i++) {
        যদি (s[i] < 0x80 || s[i] >= 0xC0) {
            ফেরত 0;
        }
    }
    ফেরত দৈর্ঘ্য;
}

পূর্ণসংখ্যা
_PyTokenizer_ensure_utf8(ধ্রুবক অক্ষর *সারি, কাঠামো tok_state *tok, পূর্ণসংখ্যা lineno)
{
    ধ্রুবক অক্ষর *badchar = ফাঁকা;
    ধ্রুবক অক্ষর *c;
    পূর্ণসংখ্যা দৈর্ঘ্য;
    ধ্রুবক অক্ষর *সারি_start = সারি;
    জন্য (c = সারি; *c; c += দৈর্ঘ্য) {
        যদি (!(দৈর্ঘ্য = valid_utf8((ধ্রুবক unsigned অক্ষর *)c))) {
            badchar = c;
            বিরতি;
        }
        যদি (*c == '\n') {
            lineno++;
            সারি_start = c + 1;
        }
    }
    যদি (badchar) {
        _PyTokenizer_syntaxerror_at(
                tok, সারি_start, badchar - সারি_start + 1, lineno,
                -1, -1,
                "Non-UTF-8 code starting with '\\x%.2x'"
                "%s%V on line %i, "
                "but no encoding declared; "
                "see https://peps.python.org/pep-0263/ for details",
                (unsigned অক্ষর)*badchar,
                tok->filename ? " in file " : "", tok->filename, "",
                lineno);
        ফেরত 0;
    }
    ফেরত 1;
}


/* ############## DEBUGGING STUFF ############## */

#ifdef Py_DEBUG
শূন্য
_PyTokenizer_print_escape(FILE *f, ধ্রুবক অক্ষর *s, Py_ssize_t আকার_2)
{
    যদি (s == ফাঁকা) {
        fputs("NULL", f);
        ফেরত;
    }
    putc('"', f);
    যখন (আকার_2-- > 0) {
        unsigned অক্ষর c = *s++;
        নির্বাচন (c) {
            ক্ষেত্র '\n': fputs("\\n", f); বিরতি;
            ক্ষেত্র '\r': fputs("\\r", f); বিরতি;
            ক্ষেত্র '\t': fputs("\\t", f); বিরতি;
            ক্ষেত্র '\f': fputs("\\f", f); বিরতি;
            ক্ষেত্র '\'': fputs("\\'", f); বিরতি;
            ক্ষেত্র '"': fputs("\\\"", f); বিরতি;
            পূর্বনির্ধারিত:
                যদি (0x20 <= c && c <= 0x7f)
                    putc(c, f);
                অন্যথা
                    fprintf(f, "\\x%02x", c);
        }
    }
    putc('"', f);
}
#endif
