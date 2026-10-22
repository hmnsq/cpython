#ଅନ୍ତର୍ଭାଵ "Python.h"
#ଅନ୍ତର୍ଭାଵ "errcode.h"
#ଅନ୍ତର୍ଭାଵ "pycore_runtime.h"       // _Py_ID()
#ଅନ୍ତର୍ଭାଵ "pycore_token.h"
#ଅନ୍ତର୍ଭାଵ "pycore_tuple.h"         // _PyTuple_FromPair

#ଅନ୍ତର୍ଭାଵ "../lexer/state.h"


/* ############## ERRORS ############## */

/* Convert a 1-based column in bytes into a 1-based column in characters.
   The line is UTF-8 encoded, so it is enough to skip continuation bytes. */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
byte_col_to_char_col(ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା byte_col)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା char_col = 1;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < byte_col - 1; i++) {
        ଯଦି ((ଧାଡ଼ି[i] & 0xC0) != 0x80) {
            char_col++;
        }
    }
    ଫେରନ୍ତୁ char_col;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_syntaxerror_range(ସଂରଚନା tok_state *tok, ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି,
                   Py_ssize_t cursor, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno, ଅଚଳ ଅକ୍ଷର *format,
                   ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col_offset,
                   va_list vargs)
{
    // In release builds, we don't want to overwrite a previous error, but in debug builds we
    // want to fail if we are not doing it so we can fix it.
    ପ୍ରତିଜ୍ଞା(tok->done != E_ERROR);
    ଯଦି (tok->done == E_ERROR) {
        ଫେରନ୍ତୁ ERRORTOKEN;
    }
    PyObject *errmsg, *errtext, *args;
    errmsg = PyUnicode_FromFormatV(format, vargs);
    ଯଦି (!errmsg) {
        ଯାଅ error;
    }

    errtext = PyUnicode_DecodeUTF8(ଧାଡ଼ି, cursor,
                                   "replace");
    ଯଦି (!errtext) {
        ଯାଅ error;
    }

    ଯଦି (col_offset == -1) {
        col_offset = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)PyUnicode_GET_LENGTH(errtext);
    }
    ଅନ୍ୟଥା ଯଦି (col_offset > 0) {
        col_offset = byte_col_to_char_col(ଧାଡ଼ି, col_offset);
    }
    ଯଦି (end_col_offset == -1) {
        end_col_offset = col_offset;
    }
    ଅନ୍ୟଥା ଯଦି (end_col_offset > 0) {
        end_col_offset = byte_col_to_char_col(ଧାଡ଼ି, end_col_offset);
    }

    Py_ssize_t ଧାଡ଼ି_len = strcspn(ଧାଡ଼ି, "\n");
    ଯଦି (ଧାଡ଼ି_len != cursor) {
        Py_DECREF(errtext);
        errtext = PyUnicode_DecodeUTF8(ଧାଡ଼ି, ଧାଡ଼ି_len,
                                       "replace");
    }
    ଯଦି (!errtext) {
        ଯାଅ error;
    }

    args = Py_Build_ମୂଲ୍ୟ("(O(OiiNii))", errmsg,
                         tok->filename ? tok->filename : Py_None,
                         lineno, col_offset, errtext,
                         lineno, end_col_offset);
    ଯଦି (args) {
        PyErr_SetObject(PyExc_SyntaxError, args);
        Py_DECREF(args);
    }

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    ଫେରନ୍ତୁ ERRORTOKEN;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyTokenizer_syntaxerror(ସଂରଚନା tok_state *tok, ଅଚଳ ଅକ୍ଷର *format, ...)
{
    // These errors are cleaned on startup. Todo: Fix it.
    va_list vargs;
    va_start(vargs, format);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->ଧାଡ଼ି_start),
        tok->cur - tok->ଧାଡ଼ି_start, tok->lineno, format, -1, -1, vargs);
    va_end(vargs);
    ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyTokenizer_syntaxerror_known_range(ସଂରଚନା tok_state *tok,
                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col_offset,
                        ଅଚଳ ଅକ୍ଷର *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->ଧାଡ଼ି_start),
        tok->cur - tok->ଧାଡ଼ି_start, tok->lineno,
        format, col_offset, end_col_offset, vargs);
    va_end(vargs);
    ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyTokenizer_syntaxerror_at(ସଂରଚନା tok_state *tok, ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି,
                             Py_ssize_t cursor, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno,
                             ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col_offset,
                             ଅଚଳ ଅକ୍ଷର *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = _syntaxerror_range(tok, ଧାଡ଼ି, cursor, lineno, format,
                                col_offset, end_col_offset, vargs);
    va_end(vargs);
    ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyTokenizer_indenterror(ସଂରଚନା tok_state *tok)
{
    tok->done = E_TABSPACE;
    tok->cur = tok->inp;
    ଫେରନ୍ତୁ ERRORTOKEN;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyTokenizer_warn_invalid_escape_sequence(ସଂରଚନା tok_state *tok, ପୂର୍ଣ୍ଣସଂଖ୍ୟା first_invalid_escape_char)
{
    PyObject *ସନ୍ଦେଶ = PyUnicode_FromFormat(
        "\"\\%c\" is an invalid escape sequence. "
        "Such sequences will not work in the future. "
        "Did you mean \"\\\\%c\"? A raw string is also an option.",
        (ଅକ୍ଷର) first_invalid_escape_char,
        (ଅକ୍ଷର) first_invalid_escape_char
    );

    ଯଦି (ସନ୍ଦେଶ == ଖାଲି) {
        ଫେରନ୍ତୁ -1;
    }

    ଯଦି (PyErr_WarnExplicitObject(PyExc_SyntaxWarning, ସନ୍ଦେଶ, tok->filename,
                                 tok->lineno, tok->module, ଖାଲି) < 0) {
        Py_DECREF(ସନ୍ଦେଶ);

        ଯଦି (PyErr_ExceptionMatches(PyExc_SyntaxWarning)) {
            /* Replace the SyntaxWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            ଫେରନ୍ତୁ _PyTokenizer_syntaxerror(tok,
                "\"\\%c\" is an invalid escape sequence. "
                "Did you mean \"\\\\%c\"? A raw string is also an option.",
                (ଅକ୍ଷର) first_invalid_escape_char,
                (ଅକ୍ଷର) first_invalid_escape_char);
        }

        ଫେରନ୍ତୁ -1;
    }

    Py_DECREF(ସନ୍ଦେଶ);
    ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ
_PyTokenizer_raise_init_error(PyObject *filename)
{
    ଯଦି (!(PyErr_ExceptionMatches(PyExc_LookupError)
          || PyErr_ExceptionMatches(PyExc_SyntaxError)
          || PyErr_ExceptionMatches(Py_Exc_ମୂଲ୍ୟ_Error)
          || PyErr_ExceptionMatches(PyExc_UnicodeDecodeError))) {
        ଫେରନ୍ତୁ;
    }
    PyObject *errstr = ଖାଲି;
    PyObject *tuple = ଖାଲି;
    PyObject *type;
    PyObject *ମୂଲ୍ୟ;
    PyObject *tback;
    PyErr_Fetch(&type, &ମୂଲ୍ୟ, &tback);
    ଯଦି (PyErr_GivenExceptionMatches(ମୂଲ୍ୟ, PyExc_SyntaxError)) {
        ଯଦି (PyObject_SetAttr(ମୂଲ୍ୟ, &_Py_ID(filename), filename)) {
            ଯାଅ error;
        }
        PyErr_Restore(type, ମୂଲ୍ୟ, tback);
        ଫେରନ୍ତୁ;
    }
    errstr = PyObject_Str(ମୂଲ୍ୟ);
    ଯଦି (!errstr) {
        ଯାଅ error;
    }

    PyObject *tmp = Py_Build_ମୂଲ୍ୟ("(OiiO)", filename, 0, -1, Py_None);
    ଯଦି (!tmp) {
        ଯାଅ error;
    }

    tuple = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    ଯଦି (!tuple) {
        ଯାଅ error;
    }
    PyErr_SetObject(PyExc_SyntaxError, tuple);

error:
    Py_XDECREF(type);
    Py_XDECREF(ମୂଲ୍ୟ);
    Py_XDECREF(tback);
    Py_XDECREF(errstr);
    Py_XDECREF(tuple);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyTokenizer_parser_warn(ସଂରଚନା tok_state *tok, PyObject *category, ଅଚଳ ଅକ୍ଷର *format, ...)
{
    PyObject *errmsg;
    va_list vargs;
    va_start(vargs, format);
    errmsg = PyUnicode_FromFormatV(format, vargs);
    va_end(vargs);
    ଯଦି (!errmsg) {
        ଯାଅ error;
    }

    ଯଦି (PyErr_WarnExplicitObject(category, errmsg, tok->filename,
                                 tok->lineno, tok->module, ଖାଲି) < 0) {
        ଯଦି (PyErr_ExceptionMatches(category)) {
            /* Replace the DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();
            _PyTokenizer_syntaxerror(tok, "%U", errmsg);
        }
        ଯାଅ error;
    }
    Py_DECREF(errmsg);
    ଫେରନ୍ତୁ 0;

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    ଫେରନ୍ତୁ -1;
}


/* Check whether the characters at s start a valid
   UTF-8 sequence. Return the number of characters forming
   the sequence if yes, 0 if not.  The special cases match
   those in stringlib/codecs.h:utf8_decode.
*/
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
valid_utf8(ଅଚଳ unsigned ଅକ୍ଷର* s)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା expected = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା length;
    ଯଦି (*s < 0x80) {
        /* single-byte code */
        ଫେରନ୍ତୁ 1;
    }
    ଅନ୍ୟଥା ଯଦି (*s < 0xE0) {
        /* \xC2\x80-\xDF\xBF -- 0080-07FF */
        ଯଦି (*s < 0xC2) {
            /* invalid sequence
               \x80-\xBF -- continuation byte
               \xC0-\xC1 -- fake 0000-007F */
            ଫେରନ୍ତୁ 0;
        }
        expected = 1;
    }
    ଅନ୍ୟଥା ଯଦି (*s < 0xF0) {
        /* \xE0\xA0\x80-\xEF\xBF\xBF -- 0800-FFFF */
        ଯଦି (*s == 0xE0 && *(s + 1) < 0xA0) {
            /* invalid sequence
               \xE0\x80\x80-\xE0\x9F\xBF -- fake 0000-0800 */
            ଫେରନ୍ତୁ 0;
        }
        ଅନ୍ୟଥା ଯଦି (*s == 0xED && *(s + 1) >= 0xA0) {
            /* Decoding UTF-8 sequences in range \xED\xA0\x80-\xED\xBF\xBF
               will result in surrogates in range D800-DFFF. Surrogates are
               not valid UTF-8 so they are rejected.
               See https://www.unicode.org/versions/Unicode5.2.0/ch03.pdf
               (table 3-7) and http://www.rfc-editor.org/rfc/rfc3629.txt */
            ଫେରନ୍ତୁ 0;
        }
        expected = 2;
    }
    ଅନ୍ୟଥା ଯଦି (*s < 0xF5) {
        /* \xF0\x90\x80\x80-\xF4\x8F\xBF\xBF -- 10000-10FFFF */
        ଯଦି (*(s + 1) < 0x90 ? *s == 0xF0 : *s == 0xF4) {
            /* invalid sequence -- one of:
               \xF0\x80\x80\x80-\xF0\x8F\xBF\xBF -- fake 0000-FFFF
               \xF4\x90\x80\x80- -- 110000- overflow */
            ଫେରନ୍ତୁ 0;
        }
        expected = 3;
    }
    ଅନ୍ୟଥା {
        /* invalid start byte */
        ଫେରନ୍ତୁ 0;
    }
    length = expected + 1;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i <= expected; i++) {
        ଯଦି (s[i] < 0x80 || s[i] >= 0xC0) {
            ଫେରନ୍ତୁ 0;
        }
    }
    ଫେରନ୍ତୁ length;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyTokenizer_ensure_utf8(ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି, ସଂରଚନା tok_state *tok, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno)
{
    ଅଚଳ ଅକ୍ଷର *badchar = ଖାଲି;
    ଅଚଳ ଅକ୍ଷର *c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା length;
    ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି_start = ଧାଡ଼ି;
    ପାଇଁ (c = ଧାଡ଼ି; *c; c += length) {
        ଯଦି (!(length = valid_utf8((ଅଚଳ unsigned ଅକ୍ଷର *)c))) {
            badchar = c;
            ଭାଙ୍ଗ;
        }
        ଯଦି (*c == '\n') {
            lineno++;
            ଧାଡ଼ି_start = c + 1;
        }
    }
    ଯଦି (badchar) {
        _PyTokenizer_syntaxerror_at(
                tok, ଧାଡ଼ି_start, badchar - ଧାଡ଼ି_start + 1, lineno,
                -1, -1,
                "Non-UTF-8 code starting with '\\x%.2x'"
                "%s%V on line %i, "
                "but no encoding declared; "
                "see https://peps.python.org/pep-0263/ for details",
                (unsigned ଅକ୍ଷର)*badchar,
                tok->filename ? " in file " : "", tok->filename, "",
                lineno);
        ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ 1;
}


/* ############## DEBUGGING STUFF ############## */

#ifdef Py_DEBUG
ଶୂନ୍ୟ
_PyTokenizer_print_escape(FILE *f, ଅଚଳ ଅକ୍ଷର *s, Py_ssize_t size)
{
    ଯଦି (s == ଖାଲି) {
        fputs("NULL", f);
        ଫେରନ୍ତୁ;
    }
    putc('"', f);
    ଯେତେବେଳେ (size-- > 0) {
        unsigned ଅକ୍ଷର c = *s++;
        ବାଛ (c) {
            ସ୍ଥିତି '\n': fputs("\\n", f); ଭାଙ୍ଗ;
            ସ୍ଥିତି '\r': fputs("\\r", f); ଭାଙ୍ଗ;
            ସ୍ଥିତି '\t': fputs("\\t", f); ଭାଙ୍ଗ;
            ସ୍ଥିତି '\f': fputs("\\f", f); ଭାଙ୍ଗ;
            ସ୍ଥିତି '\'': fputs("\\'", f); ଭାଙ୍ଗ;
            ସ୍ଥିତି '"': fputs("\\\"", f); ଭାଙ୍ଗ;
            ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଯଦି (0x20 <= c && c <= 0x7f)
                    putc(c, f);
                ଅନ୍ୟଥା
                    fprintf(f, "\\x%02x", c);
        }
    }
    putc('"', f);
}
#endif
