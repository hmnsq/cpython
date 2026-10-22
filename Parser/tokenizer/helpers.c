#அந்தர்பாவ "Python.h"
#அந்தர்பாவ "errcode.h"
#அந்தர்பாவ "pycore_runtime.h"       // _Py_ID()
#அந்தர்பாவ "pycore_token.h"
#அந்தர்பாவ "pycore_tuple.h"         // _PyTuple_FromPair

#அந்தர்பாவ "../lexer/state.h"


/* ############## ERRORS ############## */

/* Convert a 1-based column in bytes into a 1-based column in characters.
   The line is UTF-8 encoded, so it is enough to skip continuation bytes. */
நிலையான முழு
byte_col_to_char_col(மாறிலி எழுத்து *வரி, முழு byte_col)
{
    முழு char_col = 1;
    க்கு (முழு i = 0; i < byte_col - 1; i++) {
        எனில் ((வரி[i] & 0xC0) != 0x80) {
            char_col++;
        }
    }
    திரும்பு char_col;
}

நிலையான முழு
_syntaxerror_range(கட்டமைப்பு tok_state *tok, மாறிலி எழுத்து *வரி,
                   Py_ssize_t cursor, முழு lineno, மாறிலி எழுத்து *format,
                   முழு col_offset, முழு end_col_offset,
                   va_list vargs)
{
    // In release builds, we don't want to overwrite a previous error, but in debug builds we
    // want to fail if we are not doing it so we can fix it.
    ப்ரதிஜ்ஞா(tok->done != E_ERROR);
    எனில் (tok->done == E_ERROR) {
        திரும்பு ERRORTOKEN;
    }
    PyObject *errmsg, *errtext, *args;
    errmsg = PyUnicode_FromFormatV(format, vargs);
    எனில் (!errmsg) {
        செல் error;
    }

    errtext = PyUnicode_DecodeUTF8(வரி, cursor,
                                   "replace");
    எனில் (!errtext) {
        செல் error;
    }

    எனில் (col_offset == -1) {
        col_offset = (முழு)PyUnicode_GET_LENGTH(errtext);
    }
    இல்லையெனில் எனில் (col_offset > 0) {
        col_offset = byte_col_to_char_col(வரி, col_offset);
    }
    எனில் (end_col_offset == -1) {
        end_col_offset = col_offset;
    }
    இல்லையெனில் எனில் (end_col_offset > 0) {
        end_col_offset = byte_col_to_char_col(வரி, end_col_offset);
    }

    Py_ssize_t வரி_len = strcspn(வரி, "\n");
    எனில் (வரி_len != cursor) {
        Py_DECREF(errtext);
        errtext = PyUnicode_DecodeUTF8(வரி, வரி_len,
                                       "replace");
    }
    எனில் (!errtext) {
        செல் error;
    }

    args = Py_Build_மதிப்பு("(O(OiiNii))", errmsg,
                         tok->filename ? tok->filename : Py_None,
                         lineno, col_offset, errtext,
                         lineno, end_col_offset);
    எனில் (args) {
        PyErr_SetObject(PyExc_SyntaxError, args);
        Py_DECREF(args);
    }

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    திரும்பு ERRORTOKEN;
}

முழு
_PyTokenizer_syntaxerror(கட்டமைப்பு tok_state *tok, மாறிலி எழுத்து *format, ...)
{
    // These errors are cleaned on startup. Todo: Fix it.
    va_list vargs;
    va_start(vargs, format);
    முழு ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->வரி_start),
        tok->cur - tok->வரி_start, tok->lineno, format, -1, -1, vargs);
    va_end(vargs);
    திரும்பு ret;
}

முழு
_PyTokenizer_syntaxerror_known_range(கட்டமைப்பு tok_state *tok,
                        முழு col_offset, முழு end_col_offset,
                        மாறிலி எழுத்து *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    முழு ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->வரி_start),
        tok->cur - tok->வரி_start, tok->lineno,
        format, col_offset, end_col_offset, vargs);
    va_end(vargs);
    திரும்பு ret;
}

முழு
_PyTokenizer_syntaxerror_at(கட்டமைப்பு tok_state *tok, மாறிலி எழுத்து *வரி,
                             Py_ssize_t cursor, முழு lineno,
                             முழு col_offset, முழு end_col_offset,
                             மாறிலி எழுத்து *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    முழு ret = _syntaxerror_range(tok, வரி, cursor, lineno, format,
                                col_offset, end_col_offset, vargs);
    va_end(vargs);
    திரும்பு ret;
}

முழு
_PyTokenizer_indenterror(கட்டமைப்பு tok_state *tok)
{
    tok->done = E_TABSPACE;
    tok->cur = tok->inp;
    திரும்பு ERRORTOKEN;
}

முழு
_PyTokenizer_warn_invalid_escape_sequence(கட்டமைப்பு tok_state *tok, முழு first_invalid_escape_char)
{
    PyObject *செய்தி = PyUnicode_FromFormat(
        "\"\\%c\" is an invalid escape sequence. "
        "Such sequences will not work in the future. "
        "Did you mean \"\\\\%c\"? A raw string is also an option.",
        (எழுத்து) first_invalid_escape_char,
        (எழுத்து) first_invalid_escape_char
    );

    எனில் (செய்தி == வெற்று) {
        திரும்பு -1;
    }

    எனில் (PyErr_WarnExplicitObject(PyExc_SyntaxWarning, செய்தி, tok->filename,
                                 tok->lineno, tok->module, வெற்று) < 0) {
        Py_DECREF(செய்தி);

        எனில் (PyErr_ExceptionMatches(PyExc_SyntaxWarning)) {
            /* Replace the SyntaxWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            திரும்பு _PyTokenizer_syntaxerror(tok,
                "\"\\%c\" is an invalid escape sequence. "
                "Did you mean \"\\\\%c\"? A raw string is also an option.",
                (எழுத்து) first_invalid_escape_char,
                (எழுத்து) first_invalid_escape_char);
        }

        திரும்பு -1;
    }

    Py_DECREF(செய்தி);
    திரும்பு 0;
}

வெறுமை
_PyTokenizer_raise_init_error(PyObject *filename)
{
    எனில் (!(PyErr_ExceptionMatches(PyExc_LookupError)
          || PyErr_ExceptionMatches(PyExc_SyntaxError)
          || PyErr_ExceptionMatches(Py_Exc_மதிப்பு_Error)
          || PyErr_ExceptionMatches(PyExc_UnicodeDecodeError))) {
        திரும்பு;
    }
    PyObject *errstr = வெற்று;
    PyObject *tuple = வெற்று;
    PyObject *type;
    PyObject *மதிப்பு;
    PyObject *tback;
    PyErr_Fetch(&type, &மதிப்பு, &tback);
    எனில் (PyErr_GivenExceptionMatches(மதிப்பு, PyExc_SyntaxError)) {
        எனில் (PyObject_SetAttr(மதிப்பு, &_Py_ID(filename), filename)) {
            செல் error;
        }
        PyErr_Restore(type, மதிப்பு, tback);
        திரும்பு;
    }
    errstr = PyObject_Str(மதிப்பு);
    எனில் (!errstr) {
        செல் error;
    }

    PyObject *tmp = Py_Build_மதிப்பு("(OiiO)", filename, 0, -1, Py_None);
    எனில் (!tmp) {
        செல் error;
    }

    tuple = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    எனில் (!tuple) {
        செல் error;
    }
    PyErr_SetObject(PyExc_SyntaxError, tuple);

error:
    Py_XDECREF(type);
    Py_XDECREF(மதிப்பு);
    Py_XDECREF(tback);
    Py_XDECREF(errstr);
    Py_XDECREF(tuple);
}

முழு
_PyTokenizer_parser_warn(கட்டமைப்பு tok_state *tok, PyObject *category, மாறிலி எழுத்து *format, ...)
{
    PyObject *errmsg;
    va_list vargs;
    va_start(vargs, format);
    errmsg = PyUnicode_FromFormatV(format, vargs);
    va_end(vargs);
    எனில் (!errmsg) {
        செல் error;
    }

    எனில் (PyErr_WarnExplicitObject(category, errmsg, tok->filename,
                                 tok->lineno, tok->module, வெற்று) < 0) {
        எனில் (PyErr_ExceptionMatches(category)) {
            /* Replace the DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();
            _PyTokenizer_syntaxerror(tok, "%U", errmsg);
        }
        செல் error;
    }
    Py_DECREF(errmsg);
    திரும்பு 0;

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    திரும்பு -1;
}


/* Check whether the characters at s start a valid
   UTF-8 sequence. Return the number of characters forming
   the sequence if yes, 0 if not.  The special cases match
   those in stringlib/codecs.h:utf8_decode.
*/
நிலையான முழு
valid_utf8(மாறிலி unsigned எழுத்து* s)
{
    முழு expected = 0;
    முழு length;
    எனில் (*s < 0x80) {
        /* single-byte code */
        திரும்பு 1;
    }
    இல்லையெனில் எனில் (*s < 0xE0) {
        /* \xC2\x80-\xDF\xBF -- 0080-07FF */
        எனில் (*s < 0xC2) {
            /* invalid sequence
               \x80-\xBF -- continuation byte
               \xC0-\xC1 -- fake 0000-007F */
            திரும்பு 0;
        }
        expected = 1;
    }
    இல்லையெனில் எனில் (*s < 0xF0) {
        /* \xE0\xA0\x80-\xEF\xBF\xBF -- 0800-FFFF */
        எனில் (*s == 0xE0 && *(s + 1) < 0xA0) {
            /* invalid sequence
               \xE0\x80\x80-\xE0\x9F\xBF -- fake 0000-0800 */
            திரும்பு 0;
        }
        இல்லையெனில் எனில் (*s == 0xED && *(s + 1) >= 0xA0) {
            /* Decoding UTF-8 sequences in range \xED\xA0\x80-\xED\xBF\xBF
               will result in surrogates in range D800-DFFF. Surrogates are
               not valid UTF-8 so they are rejected.
               See https://www.unicode.org/versions/Unicode5.2.0/ch03.pdf
               (table 3-7) and http://www.rfc-editor.org/rfc/rfc3629.txt */
            திரும்பு 0;
        }
        expected = 2;
    }
    இல்லையெனில் எனில் (*s < 0xF5) {
        /* \xF0\x90\x80\x80-\xF4\x8F\xBF\xBF -- 10000-10FFFF */
        எனில் (*(s + 1) < 0x90 ? *s == 0xF0 : *s == 0xF4) {
            /* invalid sequence -- one of:
               \xF0\x80\x80\x80-\xF0\x8F\xBF\xBF -- fake 0000-FFFF
               \xF4\x90\x80\x80- -- 110000- overflow */
            திரும்பு 0;
        }
        expected = 3;
    }
    இல்லையெனில் {
        /* invalid start byte */
        திரும்பு 0;
    }
    length = expected + 1;
    க்கு (முழு i = 1; i <= expected; i++) {
        எனில் (s[i] < 0x80 || s[i] >= 0xC0) {
            திரும்பு 0;
        }
    }
    திரும்பு length;
}

முழு
_PyTokenizer_ensure_utf8(மாறிலி எழுத்து *வரி, கட்டமைப்பு tok_state *tok, முழு lineno)
{
    மாறிலி எழுத்து *badchar = வெற்று;
    மாறிலி எழுத்து *c;
    முழு length;
    மாறிலி எழுத்து *வரி_start = வரி;
    க்கு (c = வரி; *c; c += length) {
        எனில் (!(length = valid_utf8((மாறிலி unsigned எழுத்து *)c))) {
            badchar = c;
            நிறுத்து;
        }
        எனில் (*c == '\n') {
            lineno++;
            வரி_start = c + 1;
        }
    }
    எனில் (badchar) {
        _PyTokenizer_syntaxerror_at(
                tok, வரி_start, badchar - வரி_start + 1, lineno,
                -1, -1,
                "Non-UTF-8 code starting with '\\x%.2x'"
                "%s%V on line %i, "
                "but no encoding declared; "
                "see https://peps.python.org/pep-0263/ for details",
                (unsigned எழுத்து)*badchar,
                tok->filename ? " in file " : "", tok->filename, "",
                lineno);
        திரும்பு 0;
    }
    திரும்பு 1;
}


/* ############## DEBUGGING STUFF ############## */

#ifdef Py_DEBUG
வெறுமை
_PyTokenizer_print_escape(FILE *f, மாறிலி எழுத்து *s, Py_ssize_t size)
{
    எனில் (s == வெற்று) {
        fputs("NULL", f);
        திரும்பு;
    }
    putc('"', f);
    போது (size-- > 0) {
        unsigned எழுத்து c = *s++;
        தேர்வு (c) {
            நிலை '\n': fputs("\\n", f); நிறுத்து;
            நிலை '\r': fputs("\\r", f); நிறுத்து;
            நிலை '\t': fputs("\\t", f); நிறுத்து;
            நிலை '\f': fputs("\\f", f); நிறுத்து;
            நிலை '\'': fputs("\\'", f); நிறுத்து;
            நிலை '"': fputs("\\\"", f); நிறுத்து;
            இயல்பு:
                எனில் (0x20 <= c && c <= 0x7f)
                    putc(c, f);
                இல்லையெனில்
                    fprintf(f, "\\x%02x", c);
        }
    }
    putc('"', f);
}
#endif
