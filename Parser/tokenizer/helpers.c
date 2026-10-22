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
byte_col_to_char_col(अचर वर्ण *पङ्क्ति, पूर्णाङ्क byte_col)
{
    पूर्णाङ्क char_col = 1;
    लागि (पूर्णाङ्क i = 0; i < byte_col - 1; i++) {
        यदि ((पङ्क्ति[i] & 0xC0) != 0x80) {
            char_col++;
        }
    }
    फर्काऊ char_col;
}

स्थिर पूर्णाङ्क
_syntaxerror_range(संरचना tok_state *tok, अचर वर्ण *पङ्क्ति,
                   Py_ssize_t cursor, पूर्णाङ्क lineno, अचर वर्ण *format,
                   पूर्णाङ्क col_offset, पूर्णाङ्क end_col_offset,
                   बहु_सूची vargs)
{
    // In release builds, we don't want to overwrite a previous error, but in debug builds we
    // want to fail if we are not doing it so we can fix it.
    प्रतिज्ञा(tok->done != E_ERROR);
    यदि (tok->done == E_ERROR) {
        फर्काऊ ERRORTOKEN;
    }
    PyObject *errmsg, *errtext, *args;
    errmsg = PyUnicode_FromFormatV(format, vargs);
    यदि (!errmsg) {
        गच्छ error;
    }

    errtext = PyUnicode_DecodeUTF8(पङ्क्ति, cursor,
                                   "replace");
    यदि (!errtext) {
        गच्छ error;
    }

    यदि (col_offset == -1) {
        col_offset = (पूर्णाङ्क)PyUnicode_GET_LENGTH(errtext);
    }
    अन्यथा यदि (col_offset > 0) {
        col_offset = byte_col_to_char_col(पङ्क्ति, col_offset);
    }
    यदि (end_col_offset == -1) {
        end_col_offset = col_offset;
    }
    अन्यथा यदि (end_col_offset > 0) {
        end_col_offset = byte_col_to_char_col(पङ्क्ति, end_col_offset);
    }

    Py_ssize_t पङ्क्ति_len = म_खोज(पङ्क्ति, "\n");
    यदि (पङ्क्ति_len != cursor) {
        Py_DECREF(errtext);
        errtext = PyUnicode_DecodeUTF8(पङ्क्ति, पङ्क्ति_len,
                                       "replace");
    }
    यदि (!errtext) {
        गच्छ error;
    }

    args = Py_Build_मान("(O(OiiNii))", errmsg,
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
    फर्काऊ ERRORTOKEN;
}

पूर्णाङ्क
_PyTokenizer_syntaxerror(संरचना tok_state *tok, अचर वर्ण *format, ...)
{
    // These errors are cleaned on startup. Todo: Fix it.
    बहु_सूची vargs;
    बहु_शुरू(vargs, format);
    पूर्णाङ्क ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->पङ्क्ति_start),
        tok->cur - tok->पङ्क्ति_start, tok->lineno, format, -1, -1, vargs);
    बहु_पूर्ण(vargs);
    फर्काऊ ret;
}

पूर्णाङ्क
_PyTokenizer_syntaxerror_known_range(संरचना tok_state *tok,
                        पूर्णाङ्क col_offset, पूर्णाङ्क end_col_offset,
                        अचर वर्ण *format, ...)
{
    बहु_सूची vargs;
    बहु_शुरू(vargs, format);
    पूर्णाङ्क ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->पङ्क्ति_start),
        tok->cur - tok->पङ्क्ति_start, tok->lineno,
        format, col_offset, end_col_offset, vargs);
    बहु_पूर्ण(vargs);
    फर्काऊ ret;
}

पूर्णाङ्क
_PyTokenizer_syntaxerror_at(संरचना tok_state *tok, अचर वर्ण *पङ्क्ति,
                             Py_ssize_t cursor, पूर्णाङ्क lineno,
                             पूर्णाङ्क col_offset, पूर्णाङ्क end_col_offset,
                             अचर वर्ण *format, ...)
{
    बहु_सूची vargs;
    बहु_शुरू(vargs, format);
    पूर्णाङ्क ret = _syntaxerror_range(tok, पङ्क्ति, cursor, lineno, format,
                                col_offset, end_col_offset, vargs);
    बहु_पूर्ण(vargs);
    फर्काऊ ret;
}

पूर्णाङ्क
_PyTokenizer_indenterror(संरचना tok_state *tok)
{
    tok->done = E_TABSPACE;
    tok->cur = tok->inp;
    फर्काऊ ERRORTOKEN;
}

पूर्णाङ्क
_PyTokenizer_warn_invalid_escape_sequence(संरचना tok_state *tok, पूर्णाङ्क first_invalid_escape_char)
{
    PyObject *सन्देश = PyUnicode_FromFormat(
        "\"\\%c\" is an invalid escape sequence. "
        "Such sequences will not work in the future. "
        "Did you mean \"\\\\%c\"? A raw string is also an option.",
        (वर्ण) first_invalid_escape_char,
        (वर्ण) first_invalid_escape_char
    );

    यदि (सन्देश == रिक्त) {
        फर्काऊ -1;
    }

    यदि (PyErr_WarnExplicitObject(PyExc_SyntaxWarning, सन्देश, tok->filename,
                                 tok->lineno, tok->module, रिक्त) < 0) {
        Py_DECREF(सन्देश);

        यदि (PyErr_ExceptionMatches(PyExc_SyntaxWarning)) {
            /* Replace the SyntaxWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            फर्काऊ _PyTokenizer_syntaxerror(tok,
                "\"\\%c\" is an invalid escape sequence. "
                "Did you mean \"\\\\%c\"? A raw string is also an option.",
                (वर्ण) first_invalid_escape_char,
                (वर्ण) first_invalid_escape_char);
        }

        फर्काऊ -1;
    }

    Py_DECREF(सन्देश);
    फर्काऊ 0;
}

शून्य
_PyTokenizer_raise_init_error(PyObject *filename)
{
    यदि (!(PyErr_ExceptionMatches(PyExc_LookupError)
          || PyErr_ExceptionMatches(PyExc_SyntaxError)
          || PyErr_ExceptionMatches(Py_Exc_मान_Error)
          || PyErr_ExceptionMatches(PyExc_UnicodeDecodeError))) {
        फर्काऊ;
    }
    PyObject *errstr = रिक्त;
    PyObject *tuple = रिक्त;
    PyObject *type;
    PyObject *मान;
    PyObject *tback;
    PyErr_Fetch(&type, &मान, &tback);
    यदि (PyErr_GivenExceptionMatches(मान, PyExc_SyntaxError)) {
        यदि (PyObject_SetAttr(मान, &_Py_ID(filename), filename)) {
            गच्छ error;
        }
        PyErr_Restore(type, मान, tback);
        फर्काऊ;
    }
    errstr = PyObject_Str(मान);
    यदि (!errstr) {
        गच्छ error;
    }

    PyObject *tmp = Py_Build_मान("(OiiO)", filename, 0, -1, Py_None);
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
    Py_XDECREF(मान);
    Py_XDECREF(tback);
    Py_XDECREF(errstr);
    Py_XDECREF(tuple);
}

पूर्णाङ्क
_PyTokenizer_parser_warn(संरचना tok_state *tok, PyObject *category, अचर वर्ण *format, ...)
{
    PyObject *errmsg;
    बहु_सूची vargs;
    बहु_शुरू(vargs, format);
    errmsg = PyUnicode_FromFormatV(format, vargs);
    बहु_पूर्ण(vargs);
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
    फर्काऊ 0;

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    फर्काऊ -1;
}


/* Check whether the characters at s start a valid
   UTF-8 sequence. Return the number of characters forming
   the sequence if yes, 0 if not.  The special cases match
   those in stringlib/codecs.h:utf8_decode.
*/
स्थिर पूर्णाङ्क
valid_utf8(अचर अचिन्हित वर्ण* s)
{
    पूर्णाङ्क expected = 0;
    पूर्णाङ्क length;
    यदि (*s < 0x80) {
        /* single-byte code */
        फर्काऊ 1;
    }
    अन्यथा यदि (*s < 0xE0) {
        /* \xC2\x80-\xDF\xBF -- 0080-07FF */
        यदि (*s < 0xC2) {
            /* invalid sequence
               \x80-\xBF -- continuation byte
               \xC0-\xC1 -- fake 0000-007F */
            फर्काऊ 0;
        }
        expected = 1;
    }
    अन्यथा यदि (*s < 0xF0) {
        /* \xE0\xA0\x80-\xEF\xBF\xBF -- 0800-FFFF */
        यदि (*s == 0xE0 && *(s + 1) < 0xA0) {
            /* invalid sequence
               \xE0\x80\x80-\xE0\x9F\xBF -- fake 0000-0800 */
            फर्काऊ 0;
        }
        अन्यथा यदि (*s == 0xED && *(s + 1) >= 0xA0) {
            /* Decoding UTF-8 sequences in range \xED\xA0\x80-\xED\xBF\xBF
               will result in surrogates in range D800-DFFF. Surrogates are
               not valid UTF-8 so they are rejected.
               See https://www.unicode.org/versions/Unicode5.2.0/ch03.pdf
               (table 3-7) and http://www.rfc-editor.org/rfc/rfc3629.txt */
            फर्काऊ 0;
        }
        expected = 2;
    }
    अन्यथा यदि (*s < 0xF5) {
        /* \xF0\x90\x80\x80-\xF4\x8F\xBF\xBF -- 10000-10FFFF */
        यदि (*(s + 1) < 0x90 ? *s == 0xF0 : *s == 0xF4) {
            /* invalid sequence -- one of:
               \xF0\x80\x80\x80-\xF0\x8F\xBF\xBF -- fake 0000-FFFF
               \xF4\x90\x80\x80- -- 110000- overflow */
            फर्काऊ 0;
        }
        expected = 3;
    }
    अन्यथा {
        /* invalid start byte */
        फर्काऊ 0;
    }
    length = expected + 1;
    लागि (पूर्णाङ्क i = 1; i <= expected; i++) {
        यदि (s[i] < 0x80 || s[i] >= 0xC0) {
            फर्काऊ 0;
        }
    }
    फर्काऊ length;
}

पूर्णाङ्क
_PyTokenizer_ensure_utf8(अचर वर्ण *पङ्क्ति, संरचना tok_state *tok, पूर्णाङ्क lineno)
{
    अचर वर्ण *badchar = रिक्त;
    अचर वर्ण *c;
    पूर्णाङ्क length;
    अचर वर्ण *पङ्क्ति_start = पङ्क्ति;
    लागि (c = पङ्क्ति; *c; c += length) {
        यदि (!(length = valid_utf8((अचर अचिन्हित वर्ण *)c))) {
            badchar = c;
            रोक;
        }
        यदि (*c == '\n') {
            lineno++;
            पङ्क्ति_start = c + 1;
        }
    }
    यदि (badchar) {
        _PyTokenizer_syntaxerror_at(
                tok, पङ्क्ति_start, badchar - पङ्क्ति_start + 1, lineno,
                -1, -1,
                "Non-UTF-8 code starting with '\\x%.2x'"
                "%s%V on line %i, "
                "but no encoding declared; "
                "see https://peps.python.org/pep-0263/ for details",
                (अचिन्हित वर्ण)*badchar,
                tok->filename ? " in file " : "", tok->filename, "",
                lineno);
        फर्काऊ 0;
    }
    फर्काऊ 1;
}


/* ############## DEBUGGING STUFF ############## */

#ifdef Py_DEBUG
शून्य
_PyTokenizer_print_escape(खाता *f, अचर वर्ण *s, Py_ssize_t size)
{
    यदि (s == रिक्त) {
        ख_माला_दो("NULL", f);
        फर्काऊ;
    }
    अ_दो('"', f);
    जबसम्म (size-- > 0) {
        अचिन्हित वर्ण c = *s++;
        विकल्प (c) {
            पक्ष '\n': ख_माला_दो("\\n", f); रोक;
            पक्ष '\r': ख_माला_दो("\\r", f); रोक;
            पक्ष '\t': ख_माला_दो("\\t", f); रोक;
            पक्ष '\f': ख_माला_दो("\\f", f); रोक;
            पक्ष '\'': ख_माला_दो("\\'", f); रोक;
            पक्ष '"': ख_माला_दो("\\\"", f); रोक;
            स्वभाव:
                यदि (0x20 <= c && c <= 0x7f)
                    अ_दो(c, f);
                अन्यथा
                    ख_लिखो(f, "\\x%02x", c);
        }
    }
    अ_दो('"', f);
}
#endif
