#અન્તર્ભાવ "Python.h"
#અન્તર્ભાવ "errcode.h"
#અન્તર્ભાવ "pycore_runtime.h"       // _Py_ID()
#અન્તર્ભાવ "pycore_token.h"
#અન્તર્ભાવ "pycore_tuple.h"         // _PyTuple_FromPair

#અન્તર્ભાવ "../lexer/state.h"


/* ############## ERRORS ############## */

/* Convert a 1-based column in bytes into a 1-based column in characters.
   The line is UTF-8 encoded, so it is enough to skip continuation bytes. */
સ્થિર પૂર્ણાંક
byte_col_to_char_col(અચલ અક્ષર *લીટી, પૂર્ણાંક byte_col)
{
    પૂર્ણાંક char_col = 1;
    માટે (પૂર્ણાંક i = 0; i < byte_col - 1; i++) {
        જો ((લીટી[i] & 0xC0) != 0x80) {
            char_col++;
        }
    }
    પાછા char_col;
}

સ્થિર પૂર્ણાંક
_syntaxerror_range(સંરચના tok_state *tok, અચલ અક્ષર *લીટી,
                   Py_ssize_t cursor, પૂર્ણાંક lineno, અચલ અક્ષર *format,
                   પૂર્ણાંક col_offset, પૂર્ણાંક end_col_offset,
                   va_list vargs)
{
    // In release builds, we don't want to overwrite a previous error, but in debug builds we
    // want to fail if we are not doing it so we can fix it.
    પ્રતિજ્ઞા(tok->done != E_ERROR);
    જો (tok->done == E_ERROR) {
        પાછા ERRORTOKEN;
    }
    PyObject *errmsg, *errtext, *args;
    errmsg = PyUnicode_FromFormatV(format, vargs);
    જો (!errmsg) {
        જાઓ error;
    }

    errtext = PyUnicode_DecodeUTF8(લીટી, cursor,
                                   "replace");
    જો (!errtext) {
        જાઓ error;
    }

    જો (col_offset == -1) {
        col_offset = (પૂર્ણાંક)PyUnicode_GET_LENGTH(errtext);
    }
    નહીંતર જો (col_offset > 0) {
        col_offset = byte_col_to_char_col(લીટી, col_offset);
    }
    જો (end_col_offset == -1) {
        end_col_offset = col_offset;
    }
    નહીંતર જો (end_col_offset > 0) {
        end_col_offset = byte_col_to_char_col(લીટી, end_col_offset);
    }

    Py_ssize_t લીટી_len = strcspn(લીટી, "\n");
    જો (લીટી_len != cursor) {
        Py_DECREF(errtext);
        errtext = PyUnicode_DecodeUTF8(લીટી, લીટી_len,
                                       "replace");
    }
    જો (!errtext) {
        જાઓ error;
    }

    args = Py_Build_કિંમત("(O(OiiNii))", errmsg,
                         tok->filename ? tok->filename : Py_None,
                         lineno, col_offset, errtext,
                         lineno, end_col_offset);
    જો (args) {
        PyErr_SetObject(PyExc_SyntaxError, args);
        Py_DECREF(args);
    }

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    પાછા ERRORTOKEN;
}

પૂર્ણાંક
_PyTokenizer_syntaxerror(સંરચના tok_state *tok, અચલ અક્ષર *format, ...)
{
    // These errors are cleaned on startup. Todo: Fix it.
    va_list vargs;
    va_start(vargs, format);
    પૂર્ણાંક ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->લીટી_start),
        tok->cur - tok->લીટી_start, tok->lineno, format, -1, -1, vargs);
    va_end(vargs);
    પાછા ret;
}

પૂર્ણાંક
_PyTokenizer_syntaxerror_known_range(સંરચના tok_state *tok,
                        પૂર્ણાંક col_offset, પૂર્ણાંક end_col_offset,
                        અચલ અક્ષર *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    પૂર્ણાંક ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->લીટી_start),
        tok->cur - tok->લીટી_start, tok->lineno,
        format, col_offset, end_col_offset, vargs);
    va_end(vargs);
    પાછા ret;
}

પૂર્ણાંક
_PyTokenizer_syntaxerror_at(સંરચના tok_state *tok, અચલ અક્ષર *લીટી,
                             Py_ssize_t cursor, પૂર્ણાંક lineno,
                             પૂર્ણાંક col_offset, પૂર્ણાંક end_col_offset,
                             અચલ અક્ષર *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    પૂર્ણાંક ret = _syntaxerror_range(tok, લીટી, cursor, lineno, format,
                                col_offset, end_col_offset, vargs);
    va_end(vargs);
    પાછા ret;
}

પૂર્ણાંક
_PyTokenizer_indenterror(સંરચના tok_state *tok)
{
    tok->done = E_TABSPACE;
    tok->cur = tok->inp;
    પાછા ERRORTOKEN;
}

પૂર્ણાંક
_PyTokenizer_warn_invalid_escape_sequence(સંરચના tok_state *tok, પૂર્ણાંક first_invalid_escape_char)
{
    PyObject *સંદેશ = PyUnicode_FromFormat(
        "\"\\%c\" is an invalid escape sequence. "
        "Such sequences will not work in the future. "
        "Did you mean \"\\\\%c\"? A raw string is also an option.",
        (અક્ષર) first_invalid_escape_char,
        (અક્ષર) first_invalid_escape_char
    );

    જો (સંદેશ == ખાલી) {
        પાછા -1;
    }

    જો (PyErr_WarnExplicitObject(PyExc_SyntaxWarning, સંદેશ, tok->filename,
                                 tok->lineno, tok->module, ખાલી) < 0) {
        Py_DECREF(સંદેશ);

        જો (PyErr_ExceptionMatches(PyExc_SyntaxWarning)) {
            /* Replace the SyntaxWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            પાછા _PyTokenizer_syntaxerror(tok,
                "\"\\%c\" is an invalid escape sequence. "
                "Did you mean \"\\\\%c\"? A raw string is also an option.",
                (અક્ષર) first_invalid_escape_char,
                (અક્ષર) first_invalid_escape_char);
        }

        પાછા -1;
    }

    Py_DECREF(સંદેશ);
    પાછા 0;
}

શૂન્ય
_PyTokenizer_raise_init_error(PyObject *filename)
{
    જો (!(PyErr_ExceptionMatches(PyExc_LookupError)
          || PyErr_ExceptionMatches(PyExc_SyntaxError)
          || PyErr_ExceptionMatches(Py_Exc_કિંમત_Error)
          || PyErr_ExceptionMatches(PyExc_UnicodeDecodeError))) {
        પાછા;
    }
    PyObject *errstr = ખાલી;
    PyObject *tuple = ખાલી;
    PyObject *type;
    PyObject *કિંમત;
    PyObject *tback;
    PyErr_Fetch(&type, &કિંમત, &tback);
    જો (PyErr_GivenExceptionMatches(કિંમત, PyExc_SyntaxError)) {
        જો (PyObject_SetAttr(કિંમત, &_Py_ID(filename), filename)) {
            જાઓ error;
        }
        PyErr_Restore(type, કિંમત, tback);
        પાછા;
    }
    errstr = PyObject_Str(કિંમત);
    જો (!errstr) {
        જાઓ error;
    }

    PyObject *tmp = Py_Build_કિંમત("(OiiO)", filename, 0, -1, Py_None);
    જો (!tmp) {
        જાઓ error;
    }

    tuple = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    જો (!tuple) {
        જાઓ error;
    }
    PyErr_SetObject(PyExc_SyntaxError, tuple);

error:
    Py_XDECREF(type);
    Py_XDECREF(કિંમત);
    Py_XDECREF(tback);
    Py_XDECREF(errstr);
    Py_XDECREF(tuple);
}

પૂર્ણાંક
_PyTokenizer_parser_warn(સંરચના tok_state *tok, PyObject *category, અચલ અક્ષર *format, ...)
{
    PyObject *errmsg;
    va_list vargs;
    va_start(vargs, format);
    errmsg = PyUnicode_FromFormatV(format, vargs);
    va_end(vargs);
    જો (!errmsg) {
        જાઓ error;
    }

    જો (PyErr_WarnExplicitObject(category, errmsg, tok->filename,
                                 tok->lineno, tok->module, ખાલી) < 0) {
        જો (PyErr_ExceptionMatches(category)) {
            /* Replace the DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();
            _PyTokenizer_syntaxerror(tok, "%U", errmsg);
        }
        જાઓ error;
    }
    Py_DECREF(errmsg);
    પાછા 0;

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    પાછા -1;
}


/* Check whether the characters at s start a valid
   UTF-8 sequence. Return the number of characters forming
   the sequence if yes, 0 if not.  The special cases match
   those in stringlib/codecs.h:utf8_decode.
*/
સ્થિર પૂર્ણાંક
valid_utf8(અચલ unsigned અક્ષર* s)
{
    પૂર્ણાંક expected = 0;
    પૂર્ણાંક length;
    જો (*s < 0x80) {
        /* single-byte code */
        પાછા 1;
    }
    નહીંતર જો (*s < 0xE0) {
        /* \xC2\x80-\xDF\xBF -- 0080-07FF */
        જો (*s < 0xC2) {
            /* invalid sequence
               \x80-\xBF -- continuation byte
               \xC0-\xC1 -- fake 0000-007F */
            પાછા 0;
        }
        expected = 1;
    }
    નહીંતર જો (*s < 0xF0) {
        /* \xE0\xA0\x80-\xEF\xBF\xBF -- 0800-FFFF */
        જો (*s == 0xE0 && *(s + 1) < 0xA0) {
            /* invalid sequence
               \xE0\x80\x80-\xE0\x9F\xBF -- fake 0000-0800 */
            પાછા 0;
        }
        નહીંતર જો (*s == 0xED && *(s + 1) >= 0xA0) {
            /* Decoding UTF-8 sequences in range \xED\xA0\x80-\xED\xBF\xBF
               will result in surrogates in range D800-DFFF. Surrogates are
               not valid UTF-8 so they are rejected.
               See https://www.unicode.org/versions/Unicode5.2.0/ch03.pdf
               (table 3-7) and http://www.rfc-editor.org/rfc/rfc3629.txt */
            પાછા 0;
        }
        expected = 2;
    }
    નહીંતર જો (*s < 0xF5) {
        /* \xF0\x90\x80\x80-\xF4\x8F\xBF\xBF -- 10000-10FFFF */
        જો (*(s + 1) < 0x90 ? *s == 0xF0 : *s == 0xF4) {
            /* invalid sequence -- one of:
               \xF0\x80\x80\x80-\xF0\x8F\xBF\xBF -- fake 0000-FFFF
               \xF4\x90\x80\x80- -- 110000- overflow */
            પાછા 0;
        }
        expected = 3;
    }
    નહીંતર {
        /* invalid start byte */
        પાછા 0;
    }
    length = expected + 1;
    માટે (પૂર્ણાંક i = 1; i <= expected; i++) {
        જો (s[i] < 0x80 || s[i] >= 0xC0) {
            પાછા 0;
        }
    }
    પાછા length;
}

પૂર્ણાંક
_PyTokenizer_ensure_utf8(અચલ અક્ષર *લીટી, સંરચના tok_state *tok, પૂર્ણાંક lineno)
{
    અચલ અક્ષર *badchar = ખાલી;
    અચલ અક્ષર *c;
    પૂર્ણાંક length;
    અચલ અક્ષર *લીટી_start = લીટી;
    માટે (c = લીટી; *c; c += length) {
        જો (!(length = valid_utf8((અચલ unsigned અક્ષર *)c))) {
            badchar = c;
            તોડો;
        }
        જો (*c == '\n') {
            lineno++;
            લીટી_start = c + 1;
        }
    }
    જો (badchar) {
        _PyTokenizer_syntaxerror_at(
                tok, લીટી_start, badchar - લીટી_start + 1, lineno,
                -1, -1,
                "Non-UTF-8 code starting with '\\x%.2x'"
                "%s%V on line %i, "
                "but no encoding declared; "
                "see https://peps.python.org/pep-0263/ for details",
                (unsigned અક્ષર)*badchar,
                tok->filename ? " in file " : "", tok->filename, "",
                lineno);
        પાછા 0;
    }
    પાછા 1;
}


/* ############## DEBUGGING STUFF ############## */

#ifdef Py_DEBUG
શૂન્ય
_PyTokenizer_print_escape(FILE *f, અચલ અક્ષર *s, Py_ssize_t size)
{
    જો (s == ખાલી) {
        fputs("NULL", f);
        પાછા;
    }
    putc('"', f);
    જ્યારે (size-- > 0) {
        unsigned અક્ષર c = *s++;
        પસંદગી (c) {
            સ્થિતિ '\n': fputs("\\n", f); તોડો;
            સ્થિતિ '\r': fputs("\\r", f); તોડો;
            સ્થિતિ '\t': fputs("\\t", f); તોડો;
            સ્થિતિ '\f': fputs("\\f", f); તોડો;
            સ્થિતિ '\'': fputs("\\'", f); તોડો;
            સ્થિતિ '"': fputs("\\\"", f); તોડો;
            મૂળભૂત:
                જો (0x20 <= c && c <= 0x7f)
                    putc(c, f);
                નહીંતર
                    fprintf(f, "\\x%02x", c);
        }
    }
    putc('"', f);
}
#endif
