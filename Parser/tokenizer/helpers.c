#അന്തര്ഭാവ "Python.h"
#അന്തര്ഭാവ "errcode.h"
#അന്തര്ഭാവ "pycore_runtime.h"       // _Py_ID()
#അന്തര്ഭാവ "pycore_token.h"
#അന്തര്ഭാവ "pycore_tuple.h"         // _PyTuple_FromPair

#അന്തര്ഭാവ "../lexer/state.h"


/* ############## ERRORS ############## */

/* Convert a 1-based column in bytes into a 1-based column in characters.
   The line is UTF-8 encoded, so it is enough to skip continuation bytes. */
സ്ഥിതം പൂർണ്ണസംഖ്യ
byte_col_to_char_col(സ്ഥിരം അക്ഷരം *വരി, പൂർണ്ണസംഖ്യ byte_col)
{
    പൂർണ്ണസംഖ്യ char_col = 1;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < byte_col - 1; i++) {
        എങ്കിൽ ((വരി[i] & 0xC0) != 0x80) {
            char_col++;
        }
    }
    തിരികെ char_col;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
_syntaxerror_range(ഘടന tok_state *tok, സ്ഥിരം അക്ഷരം *വരി,
                   Py_ssize_t cursor, പൂർണ്ണസംഖ്യ lineno, സ്ഥിരം അക്ഷരം *format,
                   പൂർണ്ണസംഖ്യ col_offset, പൂർണ്ണസംഖ്യ end_col_offset,
                   va_list vargs)
{
    // In release builds, we don't want to overwrite a previous error, but in debug builds we
    // want to fail if we are not doing it so we can fix it.
    പ്രതിജ്ഞാ(tok->done != E_ERROR);
    എങ്കിൽ (tok->done == E_ERROR) {
        തിരികെ ERRORTOKEN;
    }
    PyObject *errmsg, *errtext, *args;
    errmsg = PyUnicode_FromFormatV(format, vargs);
    എങ്കിൽ (!errmsg) {
        പോകുക error;
    }

    errtext = PyUnicode_DecodeUTF8(വരി, cursor,
                                   "replace");
    എങ്കിൽ (!errtext) {
        പോകുക error;
    }

    എങ്കിൽ (col_offset == -1) {
        col_offset = (പൂർണ്ണസംഖ്യ)PyUnicode_GET_LENGTH(errtext);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (col_offset > 0) {
        col_offset = byte_col_to_char_col(വരി, col_offset);
    }
    എങ്കിൽ (end_col_offset == -1) {
        end_col_offset = col_offset;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (end_col_offset > 0) {
        end_col_offset = byte_col_to_char_col(വരി, end_col_offset);
    }

    Py_ssize_t വരി_len = strcspn(വരി, "\n");
    എങ്കിൽ (വരി_len != cursor) {
        Py_DECREF(errtext);
        errtext = PyUnicode_DecodeUTF8(വരി, വരി_len,
                                       "replace");
    }
    എങ്കിൽ (!errtext) {
        പോകുക error;
    }

    args = Py_Build_മൂല്യം("(O(OiiNii))", errmsg,
                         tok->filename ? tok->filename : Py_None,
                         lineno, col_offset, errtext,
                         lineno, end_col_offset);
    എങ്കിൽ (args) {
        PyErr_SetObject(PyExc_SyntaxError, args);
        Py_DECREF(args);
    }

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    തിരികെ ERRORTOKEN;
}

പൂർണ്ണസംഖ്യ
_PyTokenizer_syntaxerror(ഘടന tok_state *tok, സ്ഥിരം അക്ഷരം *format, ...)
{
    // These errors are cleaned on startup. Todo: Fix it.
    va_list vargs;
    va_start(vargs, format);
    പൂർണ്ണസംഖ്യ ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->വരി_start),
        tok->cur - tok->വരി_start, tok->lineno, format, -1, -1, vargs);
    va_end(vargs);
    തിരികെ ret;
}

പൂർണ്ണസംഖ്യ
_PyTokenizer_syntaxerror_known_range(ഘടന tok_state *tok,
                        പൂർണ്ണസംഖ്യ col_offset, പൂർണ്ണസംഖ്യ end_col_offset,
                        സ്ഥിരം അക്ഷരം *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    പൂർണ്ണസംഖ്യ ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->വരി_start),
        tok->cur - tok->വരി_start, tok->lineno,
        format, col_offset, end_col_offset, vargs);
    va_end(vargs);
    തിരികെ ret;
}

പൂർണ്ണസംഖ്യ
_PyTokenizer_syntaxerror_at(ഘടന tok_state *tok, സ്ഥിരം അക്ഷരം *വരി,
                             Py_ssize_t cursor, പൂർണ്ണസംഖ്യ lineno,
                             പൂർണ്ണസംഖ്യ col_offset, പൂർണ്ണസംഖ്യ end_col_offset,
                             സ്ഥിരം അക്ഷരം *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    പൂർണ്ണസംഖ്യ ret = _syntaxerror_range(tok, വരി, cursor, lineno, format,
                                col_offset, end_col_offset, vargs);
    va_end(vargs);
    തിരികെ ret;
}

പൂർണ്ണസംഖ്യ
_PyTokenizer_indenterror(ഘടന tok_state *tok)
{
    tok->done = E_TABSPACE;
    tok->cur = tok->inp;
    തിരികെ ERRORTOKEN;
}

പൂർണ്ണസംഖ്യ
_PyTokenizer_warn_invalid_escape_sequence(ഘടന tok_state *tok, പൂർണ്ണസംഖ്യ first_invalid_escape_char)
{
    PyObject *സന്ദേശം = PyUnicode_FromFormat(
        "\"\\%c\" is an invalid escape sequence. "
        "Such sequences will not work in the future. "
        "Did you mean \"\\\\%c\"? A raw string is also an option.",
        (അക്ഷരം) first_invalid_escape_char,
        (അക്ഷരം) first_invalid_escape_char
    );

    എങ്കിൽ (സന്ദേശം == ശൂന്യ) {
        തിരികെ -1;
    }

    എങ്കിൽ (PyErr_WarnExplicitObject(PyExc_SyntaxWarning, സന്ദേശം, tok->filename,
                                 tok->lineno, tok->module, ശൂന്യ) < 0) {
        Py_DECREF(സന്ദേശം);

        എങ്കിൽ (PyErr_ExceptionMatches(PyExc_SyntaxWarning)) {
            /* Replace the SyntaxWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            തിരികെ _PyTokenizer_syntaxerror(tok,
                "\"\\%c\" is an invalid escape sequence. "
                "Did you mean \"\\\\%c\"? A raw string is also an option.",
                (അക്ഷരം) first_invalid_escape_char,
                (അക്ഷരം) first_invalid_escape_char);
        }

        തിരികെ -1;
    }

    Py_DECREF(സന്ദേശം);
    തിരികെ 0;
}

ശൂന്യം
_PyTokenizer_raise_init_error(PyObject *filename)
{
    എങ്കിൽ (!(PyErr_ExceptionMatches(PyExc_LookupError)
          || PyErr_ExceptionMatches(PyExc_SyntaxError)
          || PyErr_ExceptionMatches(Py_Exc_മൂല്യം_Error)
          || PyErr_ExceptionMatches(PyExc_UnicodeDecodeError))) {
        തിരികെ;
    }
    PyObject *errstr = ശൂന്യ;
    PyObject *tuple = ശൂന്യ;
    PyObject *type;
    PyObject *മൂല്യം;
    PyObject *tback;
    PyErr_Fetch(&type, &മൂല്യം, &tback);
    എങ്കിൽ (PyErr_GivenExceptionMatches(മൂല്യം, PyExc_SyntaxError)) {
        എങ്കിൽ (PyObject_SetAttr(മൂല്യം, &_Py_ID(filename), filename)) {
            പോകുക error;
        }
        PyErr_Restore(type, മൂല്യം, tback);
        തിരികെ;
    }
    errstr = PyObject_Str(മൂല്യം);
    എങ്കിൽ (!errstr) {
        പോകുക error;
    }

    PyObject *tmp = Py_Build_മൂല്യം("(OiiO)", filename, 0, -1, Py_None);
    എങ്കിൽ (!tmp) {
        പോകുക error;
    }

    tuple = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    എങ്കിൽ (!tuple) {
        പോകുക error;
    }
    PyErr_SetObject(PyExc_SyntaxError, tuple);

error:
    Py_XDECREF(type);
    Py_XDECREF(മൂല്യം);
    Py_XDECREF(tback);
    Py_XDECREF(errstr);
    Py_XDECREF(tuple);
}

പൂർണ്ണസംഖ്യ
_PyTokenizer_parser_warn(ഘടന tok_state *tok, PyObject *category, സ്ഥിരം അക്ഷരം *format, ...)
{
    PyObject *errmsg;
    va_list vargs;
    va_start(vargs, format);
    errmsg = PyUnicode_FromFormatV(format, vargs);
    va_end(vargs);
    എങ്കിൽ (!errmsg) {
        പോകുക error;
    }

    എങ്കിൽ (PyErr_WarnExplicitObject(category, errmsg, tok->filename,
                                 tok->lineno, tok->module, ശൂന്യ) < 0) {
        എങ്കിൽ (PyErr_ExceptionMatches(category)) {
            /* Replace the DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();
            _PyTokenizer_syntaxerror(tok, "%U", errmsg);
        }
        പോകുക error;
    }
    Py_DECREF(errmsg);
    തിരികെ 0;

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    തിരികെ -1;
}


/* Check whether the characters at s start a valid
   UTF-8 sequence. Return the number of characters forming
   the sequence if yes, 0 if not.  The special cases match
   those in stringlib/codecs.h:utf8_decode.
*/
സ്ഥിതം പൂർണ്ണസംഖ്യ
valid_utf8(സ്ഥിരം unsigned അക്ഷരം* s)
{
    പൂർണ്ണസംഖ്യ expected = 0;
    പൂർണ്ണസംഖ്യ length;
    എങ്കിൽ (*s < 0x80) {
        /* single-byte code */
        തിരികെ 1;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*s < 0xE0) {
        /* \xC2\x80-\xDF\xBF -- 0080-07FF */
        എങ്കിൽ (*s < 0xC2) {
            /* invalid sequence
               \x80-\xBF -- continuation byte
               \xC0-\xC1 -- fake 0000-007F */
            തിരികെ 0;
        }
        expected = 1;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*s < 0xF0) {
        /* \xE0\xA0\x80-\xEF\xBF\xBF -- 0800-FFFF */
        എങ്കിൽ (*s == 0xE0 && *(s + 1) < 0xA0) {
            /* invalid sequence
               \xE0\x80\x80-\xE0\x9F\xBF -- fake 0000-0800 */
            തിരികെ 0;
        }
        അല്ലെങ്കിൽ എങ്കിൽ (*s == 0xED && *(s + 1) >= 0xA0) {
            /* Decoding UTF-8 sequences in range \xED\xA0\x80-\xED\xBF\xBF
               will result in surrogates in range D800-DFFF. Surrogates are
               not valid UTF-8 so they are rejected.
               See https://www.unicode.org/versions/Unicode5.2.0/ch03.pdf
               (table 3-7) and http://www.rfc-editor.org/rfc/rfc3629.txt */
            തിരികെ 0;
        }
        expected = 2;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*s < 0xF5) {
        /* \xF0\x90\x80\x80-\xF4\x8F\xBF\xBF -- 10000-10FFFF */
        എങ്കിൽ (*(s + 1) < 0x90 ? *s == 0xF0 : *s == 0xF4) {
            /* invalid sequence -- one of:
               \xF0\x80\x80\x80-\xF0\x8F\xBF\xBF -- fake 0000-FFFF
               \xF4\x90\x80\x80- -- 110000- overflow */
            തിരികെ 0;
        }
        expected = 3;
    }
    അല്ലെങ്കിൽ {
        /* invalid start byte */
        തിരികെ 0;
    }
    length = expected + 1;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i <= expected; i++) {
        എങ്കിൽ (s[i] < 0x80 || s[i] >= 0xC0) {
            തിരികെ 0;
        }
    }
    തിരികെ length;
}

പൂർണ്ണസംഖ്യ
_PyTokenizer_ensure_utf8(സ്ഥിരം അക്ഷരം *വരി, ഘടന tok_state *tok, പൂർണ്ണസംഖ്യ lineno)
{
    സ്ഥിരം അക്ഷരം *badchar = ശൂന്യ;
    സ്ഥിരം അക്ഷരം *c;
    പൂർണ്ണസംഖ്യ length;
    സ്ഥിരം അക്ഷരം *വരി_start = വരി;
    വേണ്ടി (c = വരി; *c; c += length) {
        എങ്കിൽ (!(length = valid_utf8((സ്ഥിരം unsigned അക്ഷരം *)c))) {
            badchar = c;
            നിർത്തുക;
        }
        എങ്കിൽ (*c == '\n') {
            lineno++;
            വരി_start = c + 1;
        }
    }
    എങ്കിൽ (badchar) {
        _PyTokenizer_syntaxerror_at(
                tok, വരി_start, badchar - വരി_start + 1, lineno,
                -1, -1,
                "Non-UTF-8 code starting with '\\x%.2x'"
                "%s%V on line %i, "
                "but no encoding declared; "
                "see https://peps.python.org/pep-0263/ for details",
                (unsigned അക്ഷരം)*badchar,
                tok->filename ? " in file " : "", tok->filename, "",
                lineno);
        തിരികെ 0;
    }
    തിരികെ 1;
}


/* ############## DEBUGGING STUFF ############## */

#ifdef Py_DEBUG
ശൂന്യം
_PyTokenizer_print_escape(FILE *f, സ്ഥിരം അക്ഷരം *s, Py_ssize_t size)
{
    എങ്കിൽ (s == ശൂന്യ) {
        fputs("NULL", f);
        തിരികെ;
    }
    putc('"', f);
    ആകുമ്പോൾ (size-- > 0) {
        unsigned അക്ഷരം c = *s++;
        തിരഞ്ഞെടുപ്പ് (c) {
            അവസ്ഥ '\n': fputs("\\n", f); നിർത്തുക;
            അവസ്ഥ '\r': fputs("\\r", f); നിർത്തുക;
            അവസ്ഥ '\t': fputs("\\t", f); നിർത്തുക;
            അവസ്ഥ '\f': fputs("\\f", f); നിർത്തുക;
            അവസ്ഥ '\'': fputs("\\'", f); നിർത്തുക;
            അവസ്ഥ '"': fputs("\\\"", f); നിർത്തുക;
            സ്ഥിരസ്ഥിതി:
                എങ്കിൽ (0x20 <= c && c <= 0x7f)
                    putc(c, f);
                അല്ലെങ്കിൽ
                    fprintf(f, "\\x%02x", c);
        }
    }
    putc('"', f);
}
#endif
