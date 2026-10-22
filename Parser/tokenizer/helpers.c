#অন্তর্ভাব "Python.h"
#অন্তর্ভাব "errcode.h"
#অন্তর্ভাব "pycore_runtime.h"       // _Py_ID()
#অন্তর্ভাব "pycore_token.h"
#অন্তর্ভাব "pycore_tuple.h"         // _PyTuple_FromPair

#অন্তর্ভাব "../lexer/state.h"


/* ############## ERRORS ############## */

/* Convert a 1-based column in bytes into a 1-based column in characters.
   The line is UTF-8 encoded, so it is enough to skip continuation bytes. */
স্থির পূর্ণাঙ্ক
byte_col_to_char_col(ধ্রুব বর্ণ *সারি, পূর্ণাঙ্ক byte_col)
{
    পূর্ণাঙ্ক char_col = 1;
    প্রতি (পূর্ণাঙ্ক i = 0; i < byte_col - 1; i++) {
        যদি ((সারি[i] & 0xC0) != 0x80) {
            char_col++;
        }
    }
    প্রত্যাগম char_col;
}

স্থির পূর্ণাঙ্ক
_syntaxerror_range(সংরচনা tok_state *tok, ধ্রুব বর্ণ *সারি,
                   Py_ssize_t cursor, পূর্ণাঙ্ক lineno, ধ্রুব বর্ণ *format,
                   পূর্ণাঙ্ক col_offset, পূর্ণাঙ্ক end_col_offset,
                   va_list vargs)
{
    // In release builds, we don't want to overwrite a previous error, but in debug builds we
    // want to fail if we are not doing it so we can fix it.
    প্রতিজ্ঞা(tok->done != E_ERROR);
    যদি (tok->done == E_ERROR) {
        প্রত্যাগম ERRORTOKEN;
    }
    PyObject *errmsg, *errtext, *args;
    errmsg = PyUnicode_FromFormatV(format, vargs);
    যদি (!errmsg) {
        গচ্ছ error;
    }

    errtext = PyUnicode_DecodeUTF8(সারি, cursor,
                                   "replace");
    যদি (!errtext) {
        গচ্ছ error;
    }

    যদি (col_offset == -1) {
        col_offset = (পূর্ণাঙ্ক)PyUnicode_GET_LENGTH(errtext);
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
        গচ্ছ error;
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
    প্রত্যাগম ERRORTOKEN;
}

পূর্ণাঙ্ক
_PyTokenizer_syntaxerror(সংরচনা tok_state *tok, ধ্রুব বর্ণ *format, ...)
{
    // These errors are cleaned on startup. Todo: Fix it.
    va_list vargs;
    va_start(vargs, format);
    পূর্ণাঙ্ক ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->সারি_start),
        tok->cur - tok->সারি_start, tok->lineno, format, -1, -1, vargs);
    va_end(vargs);
    প্রত্যাগম ret;
}

পূর্ণাঙ্ক
_PyTokenizer_syntaxerror_known_range(সংরচনা tok_state *tok,
                        পূর্ণাঙ্ক col_offset, পূর্ণাঙ্ক end_col_offset,
                        ধ্রুব বর্ণ *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    পূর্ণাঙ্ক ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->সারি_start),
        tok->cur - tok->সারি_start, tok->lineno,
        format, col_offset, end_col_offset, vargs);
    va_end(vargs);
    প্রত্যাগম ret;
}

পূর্ণাঙ্ক
_PyTokenizer_syntaxerror_at(সংরচনা tok_state *tok, ধ্রুব বর্ণ *সারি,
                             Py_ssize_t cursor, পূর্ণাঙ্ক lineno,
                             পূর্ণাঙ্ক col_offset, পূর্ণাঙ্ক end_col_offset,
                             ধ্রুব বর্ণ *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    পূর্ণাঙ্ক ret = _syntaxerror_range(tok, সারি, cursor, lineno, format,
                                col_offset, end_col_offset, vargs);
    va_end(vargs);
    প্রত্যাগম ret;
}

পূর্ণাঙ্ক
_PyTokenizer_indenterror(সংরচনা tok_state *tok)
{
    tok->done = E_TABSPACE;
    tok->cur = tok->inp;
    প্রত্যাগম ERRORTOKEN;
}

পূর্ণাঙ্ক
_PyTokenizer_warn_invalid_escape_sequence(সংরচনা tok_state *tok, পূর্ণাঙ্ক first_invalid_escape_char)
{
    PyObject *বার্তা = PyUnicode_FromFormat(
        "\"\\%c\" is an invalid escape sequence. "
        "Such sequences will not work in the future. "
        "Did you mean \"\\\\%c\"? A raw string is also an option.",
        (বর্ণ) first_invalid_escape_char,
        (বর্ণ) first_invalid_escape_char
    );

    যদি (বার্তা == রিক্ত) {
        প্রত্যাগম -1;
    }

    যদি (PyErr_WarnExplicitObject(PyExc_SyntaxWarning, বার্তা, tok->filename,
                                 tok->lineno, tok->module, রিক্ত) < 0) {
        Py_DECREF(বার্তা);

        যদি (PyErr_ExceptionMatches(PyExc_SyntaxWarning)) {
            /* Replace the SyntaxWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            প্রত্যাগম _PyTokenizer_syntaxerror(tok,
                "\"\\%c\" is an invalid escape sequence. "
                "Did you mean \"\\\\%c\"? A raw string is also an option.",
                (বর্ণ) first_invalid_escape_char,
                (বর্ণ) first_invalid_escape_char);
        }

        প্রত্যাগম -1;
    }

    Py_DECREF(বার্তা);
    প্রত্যাগম 0;
}

শূন্য
_PyTokenizer_raise_init_error(PyObject *filename)
{
    যদি (!(PyErr_ExceptionMatches(PyExc_LookupError)
          || PyErr_ExceptionMatches(PyExc_SyntaxError)
          || PyErr_ExceptionMatches(Py_Exc_মান_Error)
          || PyErr_ExceptionMatches(PyExc_UnicodeDecodeError))) {
        প্রত্যাগম;
    }
    PyObject *errstr = রিক্ত;
    PyObject *tuple = রিক্ত;
    PyObject *type;
    PyObject *মান;
    PyObject *tback;
    PyErr_Fetch(&type, &মান, &tback);
    যদি (PyErr_GivenExceptionMatches(মান, PyExc_SyntaxError)) {
        যদি (PyObject_SetAttr(মান, &_Py_ID(filename), filename)) {
            গচ্ছ error;
        }
        PyErr_Restore(type, মান, tback);
        প্রত্যাগম;
    }
    errstr = PyObject_Str(মান);
    যদি (!errstr) {
        গচ্ছ error;
    }

    PyObject *tmp = Py_Build_মান("(OiiO)", filename, 0, -1, Py_None);
    যদি (!tmp) {
        গচ্ছ error;
    }

    tuple = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    যদি (!tuple) {
        গচ্ছ error;
    }
    PyErr_SetObject(PyExc_SyntaxError, tuple);

error:
    Py_XDECREF(type);
    Py_XDECREF(মান);
    Py_XDECREF(tback);
    Py_XDECREF(errstr);
    Py_XDECREF(tuple);
}

পূর্ণাঙ্ক
_PyTokenizer_parser_warn(সংরচনা tok_state *tok, PyObject *category, ধ্রুব বর্ণ *format, ...)
{
    PyObject *errmsg;
    va_list vargs;
    va_start(vargs, format);
    errmsg = PyUnicode_FromFormatV(format, vargs);
    va_end(vargs);
    যদি (!errmsg) {
        গচ্ছ error;
    }

    যদি (PyErr_WarnExplicitObject(category, errmsg, tok->filename,
                                 tok->lineno, tok->module, রিক্ত) < 0) {
        যদি (PyErr_ExceptionMatches(category)) {
            /* Replace the DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();
            _PyTokenizer_syntaxerror(tok, "%U", errmsg);
        }
        গচ্ছ error;
    }
    Py_DECREF(errmsg);
    প্রত্যাগম 0;

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    প্রত্যাগম -1;
}


/* Check whether the characters at s start a valid
   UTF-8 sequence. Return the number of characters forming
   the sequence if yes, 0 if not.  The special cases match
   those in stringlib/codecs.h:utf8_decode.
*/
স্থির পূর্ণাঙ্ক
valid_utf8(ধ্রুব unsigned বর্ণ* s)
{
    পূর্ণাঙ্ক expected = 0;
    পূর্ণাঙ্ক length;
    যদি (*s < 0x80) {
        /* single-byte code */
        প্রত্যাগম 1;
    }
    অন্যথা যদি (*s < 0xE0) {
        /* \xC2\x80-\xDF\xBF -- 0080-07FF */
        যদি (*s < 0xC2) {
            /* invalid sequence
               \x80-\xBF -- continuation byte
               \xC0-\xC1 -- fake 0000-007F */
            প্রত্যাগম 0;
        }
        expected = 1;
    }
    অন্যথা যদি (*s < 0xF0) {
        /* \xE0\xA0\x80-\xEF\xBF\xBF -- 0800-FFFF */
        যদি (*s == 0xE0 && *(s + 1) < 0xA0) {
            /* invalid sequence
               \xE0\x80\x80-\xE0\x9F\xBF -- fake 0000-0800 */
            প্রত্যাগম 0;
        }
        অন্যথা যদি (*s == 0xED && *(s + 1) >= 0xA0) {
            /* Decoding UTF-8 sequences in range \xED\xA0\x80-\xED\xBF\xBF
               will result in surrogates in range D800-DFFF. Surrogates are
               not valid UTF-8 so they are rejected.
               See https://www.unicode.org/versions/Unicode5.2.0/ch03.pdf
               (table 3-7) and http://www.rfc-editor.org/rfc/rfc3629.txt */
            প্রত্যাগম 0;
        }
        expected = 2;
    }
    অন্যথা যদি (*s < 0xF5) {
        /* \xF0\x90\x80\x80-\xF4\x8F\xBF\xBF -- 10000-10FFFF */
        যদি (*(s + 1) < 0x90 ? *s == 0xF0 : *s == 0xF4) {
            /* invalid sequence -- one of:
               \xF0\x80\x80\x80-\xF0\x8F\xBF\xBF -- fake 0000-FFFF
               \xF4\x90\x80\x80- -- 110000- overflow */
            প্রত্যাগম 0;
        }
        expected = 3;
    }
    অন্যথা {
        /* invalid start byte */
        প্রত্যাগম 0;
    }
    length = expected + 1;
    প্রতি (পূর্ণাঙ্ক i = 1; i <= expected; i++) {
        যদি (s[i] < 0x80 || s[i] >= 0xC0) {
            প্রত্যাগম 0;
        }
    }
    প্রত্যাগম length;
}

পূর্ণাঙ্ক
_PyTokenizer_ensure_utf8(ধ্রুব বর্ণ *সারি, সংরচনা tok_state *tok, পূর্ণাঙ্ক lineno)
{
    ধ্রুব বর্ণ *badchar = রিক্ত;
    ধ্রুব বর্ণ *c;
    পূর্ণাঙ্ক length;
    ধ্রুব বর্ণ *সারি_start = সারি;
    প্রতি (c = সারি; *c; c += length) {
        যদি (!(length = valid_utf8((ধ্রুব unsigned বর্ণ *)c))) {
            badchar = c;
            ভঙ্গ;
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
                (unsigned বর্ণ)*badchar,
                tok->filename ? " in file " : "", tok->filename, "",
                lineno);
        প্রত্যাগম 0;
    }
    প্রত্যাগম 1;
}


/* ############## DEBUGGING STUFF ############## */

#ifdef Py_DEBUG
শূন্য
_PyTokenizer_print_escape(FILE *f, ধ্রুব বর্ণ *s, Py_ssize_t size)
{
    যদি (s == রিক্ত) {
        fputs("NULL", f);
        প্রত্যাগম;
    }
    putc('"', f);
    যাবত্ (size-- > 0) {
        unsigned বর্ণ c = *s++;
        বিকল্প (c) {
            পক্ষ '\n': fputs("\\n", f); ভঙ্গ;
            পক্ষ '\r': fputs("\\r", f); ভঙ্গ;
            পক্ষ '\t': fputs("\\t", f); ভঙ্গ;
            পক্ষ '\f': fputs("\\f", f); ভঙ্গ;
            পক্ষ '\'': fputs("\\'", f); ভঙ্গ;
            পক্ষ '"': fputs("\\\"", f); ভঙ্গ;
            স্বভাব:
                যদি (0x20 <= c && c <= 0x7f)
                    putc(c, f);
                অন্যথা
                    fprintf(f, "\\x%02x", c);
        }
    }
    putc('"', f);
}
#endif
