#అన్తర్భావ "Python.h"
#అన్తర్భావ "errcode.h"
#అన్తర్భావ "pycore_runtime.h"       // _Py_ID()
#అన్తర్భావ "pycore_token.h"
#అన్తర్భావ "pycore_tuple.h"         // _PyTuple_FromPair

#అన్తర్భావ "../lexer/state.h"


/* ############## ERRORS ############## */

/* Convert a 1-based column in bytes into a 1-based column in characters.
   The line is UTF-8 encoded, so it is enough to skip continuation bytes. */
స్థిర పూర్ణాంకం
byte_నిలువు_గమ్యం_char_నిలువు(స్థిరరాశి అక్షరం *పంక్తి, పూర్ణాంకం byte_నిలువు)
{
    పూర్ణాంకం char_నిలువు = 1;
    కొరకు (పూర్ణాంకం i = 0; i < byte_నిలువు - 1; i++) {
        ఒకవేళ ((పంక్తి[i] & 0xC0) != 0x80) {
            char_నిలువు++;
        }
    }
    తిరిగి char_నిలువు;
}

స్థిర పూర్ణాంకం
_syntaxerror_range(నిర్మాణం tok_state *tok, స్థిరరాశి అక్షరం *పంక్తి,
                   Py_ssize_t cursor, పూర్ణాంకం lineno, స్థిరరాశి అక్షరం *format,
                   పూర్ణాంకం నిలువు_స్థానభ్రంశం, పూర్ణాంకం చివర_నిలువు_స్థానభ్రంశం,
                   va_list vargs)
{
    // In release builds, we don't want to overwrite a previous error, but in debug builds we
    // want to fail if we are not doing it so we can fix it.
    ప్రతిజ్ఞా(tok->పూర్తి != E_ERROR);
    ఒకవేళ (tok->పూర్తి == E_ERROR) {
        తిరిగి ERRORTOKEN;
    }
    PyObject *errmsg, *errtext, *వాదనలు;
    errmsg = PyUnicode_FromFormatV(format, vargs);
    ఒకవేళ (!errmsg) {
        వెళ్ళు error;
    }

    errtext = PyUnicode_DecodeUTF8(పంక్తి, cursor,
                                   "replace");
    ఒకవేళ (!errtext) {
        వెళ్ళు error;
    }

    ఒకవేళ (నిలువు_స్థానభ్రంశం == -1) {
        నిలువు_స్థానభ్రంశం = (పూర్ణాంకం)Py_Unicode_GET_నిడివి(errtext);
    }
    లేకపోతే ఒకవేళ (నిలువు_స్థానభ్రంశం > 0) {
        నిలువు_స్థానభ్రంశం = byte_నిలువు_గమ్యం_char_నిలువు(పంక్తి, నిలువు_స్థానభ్రంశం);
    }
    ఒకవేళ (చివర_నిలువు_స్థానభ్రంశం == -1) {
        చివర_నిలువు_స్థానభ్రంశం = నిలువు_స్థానభ్రంశం;
    }
    లేకపోతే ఒకవేళ (చివర_నిలువు_స్థానభ్రంశం > 0) {
        చివర_నిలువు_స్థానభ్రంశం = byte_నిలువు_గమ్యం_char_నిలువు(పంక్తి, చివర_నిలువు_స్థానభ్రంశం);
    }

    Py_ssize_t పంక్తి_len = strcspn(పంక్తి, "\n");
    ఒకవేళ (పంక్తి_len != cursor) {
        Py_DECREF(errtext);
        errtext = PyUnicode_DecodeUTF8(పంక్తి, పంక్తి_len,
                                       "replace");
    }
    ఒకవేళ (!errtext) {
        వెళ్ళు error;
    }

    వాదనలు = Py_Build_విలువ("(O(OiiNii))", errmsg,
                         tok->filename ? tok->filename : Py_None,
                         lineno, నిలువు_స్థానభ్రంశం, errtext,
                         lineno, చివర_నిలువు_స్థానభ్రంశం);
    ఒకవేళ (వాదనలు) {
        Py_Err_సమితి_Object(PyExc_SyntaxError, వాదనలు);
        Py_DECREF(వాదనలు);
    }

error:
    Py_XDECREF(errmsg);
    tok->పూర్తి = E_ERROR;
    తిరిగి ERRORTOKEN;
}

పూర్ణాంకం
_PyTokenizer_syntaxerror(నిర్మాణం tok_state *tok, స్థిరరాశి అక్షరం *format, ...)
{
    // These errors are cleaned on startup. Todo: Fix it.
    va_list vargs;
    va_ప్రారంభం(vargs, format);
    పూర్ణాంకం ret = _syntaxerror_range(
        tok, Py_Tok_ఆధారం_సూచిక(&tok->ఆధారం, tok->పంక్తి_ప్రారంభం),
        tok->cur - tok->పంక్తి_ప్రారంభం, tok->lineno, format, -1, -1, vargs);
    va_చివర(vargs);
    తిరిగి ret;
}

పూర్ణాంకం
Py_Tokenizer_syntaxerror_తెలిసిన_range(నిర్మాణం tok_state *tok,
                        పూర్ణాంకం నిలువు_స్థానభ్రంశం, పూర్ణాంకం చివర_నిలువు_స్థానభ్రంశం,
                        స్థిరరాశి అక్షరం *format, ...)
{
    va_list vargs;
    va_ప్రారంభం(vargs, format);
    పూర్ణాంకం ret = _syntaxerror_range(
        tok, Py_Tok_ఆధారం_సూచిక(&tok->ఆధారం, tok->పంక్తి_ప్రారంభం),
        tok->cur - tok->పంక్తి_ప్రారంభం, tok->lineno,
        format, నిలువు_స్థానభ్రంశం, చివర_నిలువు_స్థానభ్రంశం, vargs);
    va_చివర(vargs);
    తిరిగి ret;
}

పూర్ణాంకం
_PyTokenizer_syntaxerror_at(నిర్మాణం tok_state *tok, స్థిరరాశి అక్షరం *పంక్తి,
                             Py_ssize_t cursor, పూర్ణాంకం lineno,
                             పూర్ణాంకం నిలువు_స్థానభ్రంశం, పూర్ణాంకం చివర_నిలువు_స్థానభ్రంశం,
                             స్థిరరాశి అక్షరం *format, ...)
{
    va_list vargs;
    va_ప్రారంభం(vargs, format);
    పూర్ణాంకం ret = _syntaxerror_range(tok, పంక్తి, cursor, lineno, format,
                                నిలువు_స్థానభ్రంశం, చివర_నిలువు_స్థానభ్రంశం, vargs);
    va_చివర(vargs);
    తిరిగి ret;
}

పూర్ణాంకం
_PyTokenizer_indenterror(నిర్మాణం tok_state *tok)
{
    tok->పూర్తి = E_TABSPACE;
    tok->cur = tok->inp;
    తిరిగి ERRORTOKEN;
}

పూర్ణాంకం
_PyTokenizer_warn_invalid_escape_sequence(నిర్మాణం tok_state *tok, పూర్ణాంకం మొదటి_invalid_escape_char)
{
    PyObject *సందేశం = PyUnicode_FromFormat(
        "\"\\%c\" is an invalid escape sequence. "
        "Such sequences will not work in the future. "
        "Did you mean \"\\\\%c\"? A raw string is also an option.",
        (అక్షరం) మొదటి_invalid_escape_char,
        (అక్షరం) మొదటి_invalid_escape_char
    );

    ఒకవేళ (సందేశం == ఖాళీ) {
        తిరిగి -1;
    }

    ఒకవేళ (PyErr_WarnExplicitObject(PyExc_SyntaxWarning, సందేశం, tok->filename,
                                 tok->lineno, tok->module, ఖాళీ) < 0) {
        Py_DECREF(సందేశం);

        ఒకవేళ (PyErr_ExceptionMatches(PyExc_SyntaxWarning)) {
            /* Replace the SyntaxWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            తిరిగి _PyTokenizer_syntaxerror(tok,
                "\"\\%c\" is an invalid escape sequence. "
                "Did you mean \"\\\\%c\"? A raw string is also an option.",
                (అక్షరం) మొదటి_invalid_escape_char,
                (అక్షరం) మొదటి_invalid_escape_char);
        }

        తిరిగి -1;
    }

    Py_DECREF(సందేశం);
    తిరిగి 0;
}

శూన్య
_PyTokenizer_raise_init_error(PyObject *filename)
{
    ఒకవేళ (!(PyErr_ExceptionMatches(PyExc_LookupError)
          || PyErr_ExceptionMatches(PyExc_SyntaxError)
          || PyErr_ExceptionMatches(Py_Exc_విలువ_Error)
          || PyErr_ExceptionMatches(PyExc_UnicodeDecodeError))) {
        తిరిగి;
    }
    PyObject *errstr = ఖాళీ;
    PyObject *tuple = ఖాళీ;
    PyObject *type;
    PyObject *విలువ;
    PyObject *tback;
    PyErr_Fetch(&type, &విలువ, &tback);
    ఒకవేళ (PyErr_GivenExceptionMatches(విలువ, PyExc_SyntaxError)) {
        ఒకవేళ (Py_Object_సమితి_Attr(విలువ, &_Py_ID(filename), filename)) {
            వెళ్ళు error;
        }
        PyErr_Restore(type, విలువ, tback);
        తిరిగి;
    }
    errstr = PyObject_Str(విలువ);
    ఒకవేళ (!errstr) {
        వెళ్ళు error;
    }

    PyObject *tmp = Py_Build_విలువ("(OiiO)", filename, 0, -1, Py_None);
    ఒకవేళ (!tmp) {
        వెళ్ళు error;
    }

    tuple = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    ఒకవేళ (!tuple) {
        వెళ్ళు error;
    }
    Py_Err_సమితి_Object(PyExc_SyntaxError, tuple);

error:
    Py_XDECREF(type);
    Py_XDECREF(విలువ);
    Py_XDECREF(tback);
    Py_XDECREF(errstr);
    Py_XDECREF(tuple);
}

పూర్ణాంకం
_PyTokenizer_parser_warn(నిర్మాణం tok_state *tok, PyObject *category, స్థిరరాశి అక్షరం *format, ...)
{
    PyObject *errmsg;
    va_list vargs;
    va_ప్రారంభం(vargs, format);
    errmsg = PyUnicode_FromFormatV(format, vargs);
    va_చివర(vargs);
    ఒకవేళ (!errmsg) {
        వెళ్ళు error;
    }

    ఒకవేళ (PyErr_WarnExplicitObject(category, errmsg, tok->filename,
                                 tok->lineno, tok->module, ఖాళీ) < 0) {
        ఒకవేళ (PyErr_ExceptionMatches(category)) {
            /* Replace the DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();
            _PyTokenizer_syntaxerror(tok, "%U", errmsg);
        }
        వెళ్ళు error;
    }
    Py_DECREF(errmsg);
    తిరిగి 0;

error:
    Py_XDECREF(errmsg);
    tok->పూర్తి = E_ERROR;
    తిరిగి -1;
}


/* Check whether the characters at s start a valid
   UTF-8 sequence. Return the number of characters forming
   the sequence if yes, 0 if not.  The special cases match
   those in stringlib/codecs.h:utf8_decode.
*/
స్థిర పూర్ణాంకం
valid_utf8(స్థిరరాశి గుర్తులేని అక్షరం* s)
{
    పూర్ణాంకం expected = 0;
    పూర్ణాంకం నిడివి;
    ఒకవేళ (*s < 0x80) {
        /* single-byte code */
        తిరిగి 1;
    }
    లేకపోతే ఒకవేళ (*s < 0xE0) {
        /* \xC2\x80-\xDF\xBF -- 0080-07FF */
        ఒకవేళ (*s < 0xC2) {
            /* invalid sequence
               \x80-\xBF -- continuation byte
               \xC0-\xC1 -- fake 0000-007F */
            తిరిగి 0;
        }
        expected = 1;
    }
    లేకపోతే ఒకవేళ (*s < 0xF0) {
        /* \xE0\xA0\x80-\xEF\xBF\xBF -- 0800-FFFF */
        ఒకవేళ (*s == 0xE0 && *(s + 1) < 0xA0) {
            /* invalid sequence
               \xE0\x80\x80-\xE0\x9F\xBF -- fake 0000-0800 */
            తిరిగి 0;
        }
        లేకపోతే ఒకవేళ (*s == 0xED && *(s + 1) >= 0xA0) {
            /* Decoding UTF-8 sequences in range \xED\xA0\x80-\xED\xBF\xBF
               will result in surrogates in range D800-DFFF. Surrogates are
               not valid UTF-8 so they are rejected.
               See https://www.unicode.org/versions/Unicode5.2.0/ch03.pdf
               (table 3-7) and http://www.rfc-editor.org/rfc/rfc3629.txt */
            తిరిగి 0;
        }
        expected = 2;
    }
    లేకపోతే ఒకవేళ (*s < 0xF5) {
        /* \xF0\x90\x80\x80-\xF4\x8F\xBF\xBF -- 10000-10FFFF */
        ఒకవేళ (*(s + 1) < 0x90 ? *s == 0xF0 : *s == 0xF4) {
            /* invalid sequence -- one of:
               \xF0\x80\x80\x80-\xF0\x8F\xBF\xBF -- fake 0000-FFFF
               \xF4\x90\x80\x80- -- 110000- overflow */
            తిరిగి 0;
        }
        expected = 3;
    }
    లేకపోతే {
        /* invalid start byte */
        తిరిగి 0;
    }
    నిడివి = expected + 1;
    కొరకు (పూర్ణాంకం i = 1; i <= expected; i++) {
        ఒకవేళ (s[i] < 0x80 || s[i] >= 0xC0) {
            తిరిగి 0;
        }
    }
    తిరిగి నిడివి;
}

పూర్ణాంకం
_PyTokenizer_ensure_utf8(స్థిరరాశి అక్షరం *పంక్తి, నిర్మాణం tok_state *tok, పూర్ణాంకం lineno)
{
    స్థిరరాశి అక్షరం *badchar = ఖాళీ;
    స్థిరరాశి అక్షరం *c;
    పూర్ణాంకం నిడివి;
    స్థిరరాశి అక్షరం *పంక్తి_ప్రారంభం = పంక్తి;
    కొరకు (c = పంక్తి; *c; c += నిడివి) {
        ఒకవేళ (!(నిడివి = valid_utf8((స్థిరరాశి గుర్తులేని అక్షరం *)c))) {
            badchar = c;
            ఆపు;
        }
        ఒకవేళ (*c == '\n') {
            lineno++;
            పంక్తి_ప్రారంభం = c + 1;
        }
    }
    ఒకవేళ (badchar) {
        _PyTokenizer_syntaxerror_at(
                tok, పంక్తి_ప్రారంభం, badchar - పంక్తి_ప్రారంభం + 1, lineno,
                -1, -1,
                "Non-UTF-8 code starting with '\\x%.2x'"
                "%s%V on line %i, "
                "but no encoding declared; "
                "see https://peps.python.org/pep-0263/ for details",
                (గుర్తులేని అక్షరం)*badchar,
                tok->filename ? " in file " : "", tok->filename, "",
                lineno);
        తిరిగి 0;
    }
    తిరిగి 1;
}


/* ############## DEBUGGING STUFF ############## */

#నిర్వచితమైతే Py_DEBUG
శూన్య
_PyTokenizer_print_escape(FILE *f, స్థిరరాశి అక్షరం *s, Py_ssize_t కొలత)
{
    ఒకవేళ (s == ఖాళీ) {
        fputs("NULL", f);
        తిరిగి;
    }
    putc('"', f);
    అయితే (కొలత-- > 0) {
        గుర్తులేని అక్షరం c = *s++;
        ఎంపిక (c) {
            స్థితి '\n': fputs("\\n", f); ఆపు;
            స్థితి '\r': fputs("\\r", f); ఆపు;
            స్థితి '\t': fputs("\\t", f); ఆపు;
            స్థితి '\f': fputs("\\f", f); ఆపు;
            స్థితి '\'': fputs("\\'", f); ఆపు;
            స్థితి '"': fputs("\\\"", f); ఆపు;
            అప్రమేయ:
                ఒకవేళ (0x20 <= c && c <= 0x7f)
                    putc(c, f);
                లేకపోతే
                    fprintf(f, "\\x%02x", c);
        }
    }
    putc('"', f);
}
#అయితే_ముగింపు
