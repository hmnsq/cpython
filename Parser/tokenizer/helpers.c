#अन्तर्भाव "Python.h"
#अन्तर्भाव "errcode.h"
#अन्तर्भाव "pycore_runtime.h"       // _Py_ID()
#अन्तर्भाव "pycore_token.h"
#अन्तर्भाव "pycore_tuple.h"         // _PyTuple_FromPair

#अन्तर्भाव "../lexer/state.h"


/* ############## ERRORS ############## */

/* Convert a 1-based column in bytes into a 1-based column in characters.
   The line is UTF-8 encoded, so it is enough to skip continuation bytes. */
स्थिर पूर्णाङ्क
byte_col_to_char_col(ध्रुव वर्ण *ओळ, पूर्णाङ्क byte_col)
{
    पूर्णाङ्क char_col = 1;
    प्रति (पूर्णाङ्क i = 0; i < byte_col - 1; i++) {
        यदि ((ओळ[i] & 0xC0) != 0x80) {
            char_col++;
        }
    }
    प्रत्यागम char_col;
}

स्थिर पूर्णाङ्क
_syntaxerror_range(संरचना tok_state *tok, ध्रुव वर्ण *ओळ,
                   Py_ssize_t cursor, पूर्णाङ्क lineno, ध्रुव वर्ण *format,
                   पूर्णाङ्क col_offset, पूर्णाङ्क end_col_offset,
                   va_list vargs)
{
    // In release builds, we don't want to overwrite a previous error, but in debug builds we
    // want to fail if we are not doing it so we can fix it.
    प्रतिज्ञा(tok->done != E_ERROR);
    यदि (tok->done == E_ERROR) {
        प्रत्यागम ERRORTOKEN;
    }
    PyObject *errmsg, *errtext, *args;
    errmsg = PyUnicode_FromFormatV(format, vargs);
    यदि (!errmsg) {
        गच्छ error;
    }

    errtext = PyUnicode_DecodeUTF8(ओळ, cursor,
                                   "replace");
    यदि (!errtext) {
        गच्छ error;
    }

    यदि (col_offset == -1) {
        col_offset = (पूर्णाङ्क)PyUnicode_GET_LENGTH(errtext);
    }
    अन्यथा यदि (col_offset > 0) {
        col_offset = byte_col_to_char_col(ओळ, col_offset);
    }
    यदि (end_col_offset == -1) {
        end_col_offset = col_offset;
    }
    अन्यथा यदि (end_col_offset > 0) {
        end_col_offset = byte_col_to_char_col(ओळ, end_col_offset);
    }

    Py_ssize_t ओळ_len = strcspn(ओळ, "\n");
    यदि (ओळ_len != cursor) {
        Py_DECREF(errtext);
        errtext = PyUnicode_DecodeUTF8(ओळ, ओळ_len,
                                       "replace");
    }
    यदि (!errtext) {
        गच्छ error;
    }

    args = Py_Build_मूल्य("(O(OiiNii))", errmsg,
                         tok->filename ? tok->filename : Py_None,
                         lineno, col_offset, errtext,
                         lineno, end_col_offset);
    यदि (args) {
        PyErr_SetObject(PyExc_SyntaxError, args);
        Py_DECREF(args);
    }

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    प्रत्यागम ERRORTOKEN;
}

पूर्णाङ्क
_PyTokenizer_syntaxerror(संरचना tok_state *tok, ध्रुव वर्ण *format, ...)
{
    // These errors are cleaned on startup. Todo: Fix it.
    va_list vargs;
    va_start(vargs, format);
    पूर्णाङ्क ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->ओळ_start),
        tok->cur - tok->ओळ_start, tok->lineno, format, -1, -1, vargs);
    va_end(vargs);
    प्रत्यागम ret;
}

पूर्णाङ्क
_PyTokenizer_syntaxerror_known_range(संरचना tok_state *tok,
                        पूर्णाङ्क col_offset, पूर्णाङ्क end_col_offset,
                        ध्रुव वर्ण *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    पूर्णाङ्क ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->ओळ_start),
        tok->cur - tok->ओळ_start, tok->lineno,
        format, col_offset, end_col_offset, vargs);
    va_end(vargs);
    प्रत्यागम ret;
}

पूर्णाङ्क
_PyTokenizer_syntaxerror_at(संरचना tok_state *tok, ध्रुव वर्ण *ओळ,
                             Py_ssize_t cursor, पूर्णाङ्क lineno,
                             पूर्णाङ्क col_offset, पूर्णाङ्क end_col_offset,
                             ध्रुव वर्ण *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    पूर्णाङ्क ret = _syntaxerror_range(tok, ओळ, cursor, lineno, format,
                                col_offset, end_col_offset, vargs);
    va_end(vargs);
    प्रत्यागम ret;
}

पूर्णाङ्क
_PyTokenizer_indenterror(संरचना tok_state *tok)
{
    tok->done = E_TABSPACE;
    tok->cur = tok->inp;
    प्रत्यागम ERRORTOKEN;
}

पूर्णाङ्क
_PyTokenizer_warn_invalid_escape_sequence(संरचना tok_state *tok, पूर्णाङ्क first_invalid_escape_char)
{
    PyObject *संदेश = PyUnicode_FromFormat(
        "\"\\%c\" is an invalid escape sequence. "
        "Such sequences will not work in the future. "
        "Did you mean \"\\\\%c\"? A raw string is also an option.",
        (वर्ण) first_invalid_escape_char,
        (वर्ण) first_invalid_escape_char
    );

    यदि (संदेश == रिक्त) {
        प्रत्यागम -1;
    }

    यदि (PyErr_WarnExplicitObject(PyExc_SyntaxWarning, संदेश, tok->filename,
                                 tok->lineno, tok->module, रिक्त) < 0) {
        Py_DECREF(संदेश);

        यदि (PyErr_ExceptionMatches(PyExc_SyntaxWarning)) {
            /* Replace the SyntaxWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            प्रत्यागम _PyTokenizer_syntaxerror(tok,
                "\"\\%c\" is an invalid escape sequence. "
                "Did you mean \"\\\\%c\"? A raw string is also an option.",
                (वर्ण) first_invalid_escape_char,
                (वर्ण) first_invalid_escape_char);
        }

        प्रत्यागम -1;
    }

    Py_DECREF(संदेश);
    प्रत्यागम 0;
}

शून्य
_PyTokenizer_raise_init_error(PyObject *filename)
{
    यदि (!(PyErr_ExceptionMatches(PyExc_LookupError)
          || PyErr_ExceptionMatches(PyExc_SyntaxError)
          || PyErr_ExceptionMatches(Py_Exc_मूल्य_Error)
          || PyErr_ExceptionMatches(PyExc_UnicodeDecodeError))) {
        प्रत्यागम;
    }
    PyObject *errstr = रिक्त;
    PyObject *tuple = रिक्त;
    PyObject *type;
    PyObject *मूल्य;
    PyObject *tback;
    PyErr_Fetch(&type, &मूल्य, &tback);
    यदि (PyErr_GivenExceptionMatches(मूल्य, PyExc_SyntaxError)) {
        यदि (PyObject_SetAttr(मूल्य, &_Py_ID(filename), filename)) {
            गच्छ error;
        }
        PyErr_Restore(type, मूल्य, tback);
        प्रत्यागम;
    }
    errstr = PyObject_Str(मूल्य);
    यदि (!errstr) {
        गच्छ error;
    }

    PyObject *tmp = Py_Build_मूल्य("(OiiO)", filename, 0, -1, Py_None);
    यदि (!tmp) {
        गच्छ error;
    }

    tuple = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    यदि (!tuple) {
        गच्छ error;
    }
    PyErr_SetObject(PyExc_SyntaxError, tuple);

error:
    Py_XDECREF(type);
    Py_XDECREF(मूल्य);
    Py_XDECREF(tback);
    Py_XDECREF(errstr);
    Py_XDECREF(tuple);
}

पूर्णाङ्क
_PyTokenizer_parser_warn(संरचना tok_state *tok, PyObject *category, ध्रुव वर्ण *format, ...)
{
    PyObject *errmsg;
    va_list vargs;
    va_start(vargs, format);
    errmsg = PyUnicode_FromFormatV(format, vargs);
    va_end(vargs);
    यदि (!errmsg) {
        गच्छ error;
    }

    यदि (PyErr_WarnExplicitObject(category, errmsg, tok->filename,
                                 tok->lineno, tok->module, रिक्त) < 0) {
        यदि (PyErr_ExceptionMatches(category)) {
            /* Replace the DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();
            _PyTokenizer_syntaxerror(tok, "%U", errmsg);
        }
        गच्छ error;
    }
    Py_DECREF(errmsg);
    प्रत्यागम 0;

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    प्रत्यागम -1;
}


/* Check whether the characters at s start a valid
   UTF-8 sequence. Return the number of characters forming
   the sequence if yes, 0 if not.  The special cases match
   those in stringlib/codecs.h:utf8_decode.
*/
स्थिर पूर्णाङ्क
valid_utf8(ध्रुव unsigned वर्ण* s)
{
    पूर्णाङ्क expected = 0;
    पूर्णाङ्क length;
    यदि (*s < 0x80) {
        /* single-byte code */
        प्रत्यागम 1;
    }
    अन्यथा यदि (*s < 0xE0) {
        /* \xC2\x80-\xDF\xBF -- 0080-07FF */
        यदि (*s < 0xC2) {
            /* invalid sequence
               \x80-\xBF -- continuation byte
               \xC0-\xC1 -- fake 0000-007F */
            प्रत्यागम 0;
        }
        expected = 1;
    }
    अन्यथा यदि (*s < 0xF0) {
        /* \xE0\xA0\x80-\xEF\xBF\xBF -- 0800-FFFF */
        यदि (*s == 0xE0 && *(s + 1) < 0xA0) {
            /* invalid sequence
               \xE0\x80\x80-\xE0\x9F\xBF -- fake 0000-0800 */
            प्रत्यागम 0;
        }
        अन्यथा यदि (*s == 0xED && *(s + 1) >= 0xA0) {
            /* Decoding UTF-8 sequences in range \xED\xA0\x80-\xED\xBF\xBF
               will result in surrogates in range D800-DFFF. Surrogates are
               not valid UTF-8 so they are rejected.
               See https://www.unicode.org/versions/Unicode5.2.0/ch03.pdf
               (table 3-7) and http://www.rfc-editor.org/rfc/rfc3629.txt */
            प्रत्यागम 0;
        }
        expected = 2;
    }
    अन्यथा यदि (*s < 0xF5) {
        /* \xF0\x90\x80\x80-\xF4\x8F\xBF\xBF -- 10000-10FFFF */
        यदि (*(s + 1) < 0x90 ? *s == 0xF0 : *s == 0xF4) {
            /* invalid sequence -- one of:
               \xF0\x80\x80\x80-\xF0\x8F\xBF\xBF -- fake 0000-FFFF
               \xF4\x90\x80\x80- -- 110000- overflow */
            प्रत्यागम 0;
        }
        expected = 3;
    }
    अन्यथा {
        /* invalid start byte */
        प्रत्यागम 0;
    }
    length = expected + 1;
    प्रति (पूर्णाङ्क i = 1; i <= expected; i++) {
        यदि (s[i] < 0x80 || s[i] >= 0xC0) {
            प्रत्यागम 0;
        }
    }
    प्रत्यागम length;
}

पूर्णाङ्क
_PyTokenizer_ensure_utf8(ध्रुव वर्ण *ओळ, संरचना tok_state *tok, पूर्णाङ्क lineno)
{
    ध्रुव वर्ण *badchar = रिक्त;
    ध्रुव वर्ण *c;
    पूर्णाङ्क length;
    ध्रुव वर्ण *ओळ_start = ओळ;
    प्रति (c = ओळ; *c; c += length) {
        यदि (!(length = valid_utf8((ध्रुव unsigned वर्ण *)c))) {
            badchar = c;
            भङ्ग;
        }
        यदि (*c == '\n') {
            lineno++;
            ओळ_start = c + 1;
        }
    }
    यदि (badchar) {
        _PyTokenizer_syntaxerror_at(
                tok, ओळ_start, badchar - ओळ_start + 1, lineno,
                -1, -1,
                "Non-UTF-8 code starting with '\\x%.2x'"
                "%s%V on line %i, "
                "but no encoding declared; "
                "see https://peps.python.org/pep-0263/ for details",
                (unsigned वर्ण)*badchar,
                tok->filename ? " in file " : "", tok->filename, "",
                lineno);
        प्रत्यागम 0;
    }
    प्रत्यागम 1;
}


/* ############## DEBUGGING STUFF ############## */

#ifdef Py_DEBUG
शून्य
_PyTokenizer_print_escape(FILE *f, ध्रुव वर्ण *s, Py_ssize_t size)
{
    यदि (s == रिक्त) {
        fputs("NULL", f);
        प्रत्यागम;
    }
    putc('"', f);
    यावत् (size-- > 0) {
        unsigned वर्ण c = *s++;
        विकल्प (c) {
            पक्ष '\n': fputs("\\n", f); भङ्ग;
            पक्ष '\r': fputs("\\r", f); भङ्ग;
            पक्ष '\t': fputs("\\t", f); भङ्ग;
            पक्ष '\f': fputs("\\f", f); भङ्ग;
            पक्ष '\'': fputs("\\'", f); भङ्ग;
            पक्ष '"': fputs("\\\"", f); भङ्ग;
            स्वभाव:
                यदि (0x20 <= c && c <= 0x7f)
                    putc(c, f);
                अन्यथा
                    fprintf(f, "\\x%02x", c);
        }
    }
    putc('"', f);
}
#endif
