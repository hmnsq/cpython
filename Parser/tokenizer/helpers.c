#ਅਨ੍ਤਰ੍ਭਾਵ "Python.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errcode.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_runtime.h"       // _Py_ID()
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_token.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_tuple.h"         // _PyTuple_FromPair

#ਅਨ੍ਤਰ੍ਭਾਵ "../lexer/state.h"


/* ############## ERRORS ############## */

/* Convert a 1-based column in bytes into a 1-based column in characters.
   The line is UTF-8 encoded, so it is enough to skip continuation bytes. */
ਸਥਿਰ ਪੂਰਨ
byte_ਖਾਨਾ_ਮੰਜ਼ਿਲ_char_ਖਾਨਾ(ਅਚਲ ਅੱਖਰ *ਸਤਰ, ਪੂਰਨ byte_ਖਾਨਾ)
{
    ਪੂਰਨ char_ਖਾਨਾ = 1;
    ਲਈ (ਪੂਰਨ i = 0; i < byte_ਖਾਨਾ - 1; i++) {
        ਜੇ ((ਸਤਰ[i] & 0xC0) != 0x80) {
            char_ਖਾਨਾ++;
        }
    }
    ਵਾਪਸ char_ਖਾਨਾ;
}

ਸਥਿਰ ਪੂਰਨ
_syntaxerror_range(ਬਣਤਰ tok_state *tok, ਅਚਲ ਅੱਖਰ *ਸਤਰ,
                   Py_ssize_t cursor, ਪੂਰਨ lineno, ਅਚਲ ਅੱਖਰ *format,
                   ਪੂਰਨ ਖਾਨਾ_ਵਿੱਥ, ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ,
                   va_list vargs)
{
    // In release builds, we don't want to overwrite a previous error, but in debug builds we
    // want to fail if we are not doing it so we can fix it.
    ਪ੍ਰਤਿਜ੍ਞਾ(tok->ਪੂਰਾ != E_ERROR);
    ਜੇ (tok->ਪੂਰਾ == E_ERROR) {
        ਵਾਪਸ ERRORTOKEN;
    }
    PyObject *errmsg, *errtext, *ਦਲੀਲਾਂ;
    errmsg = PyUnicode_FromFormatV(format, vargs);
    ਜੇ (!errmsg) {
        ਜਾਓ error;
    }

    errtext = PyUnicode_DecodeUTF8(ਸਤਰ, cursor,
                                   "replace");
    ਜੇ (!errtext) {
        ਜਾਓ error;
    }

    ਜੇ (ਖਾਨਾ_ਵਿੱਥ == -1) {
        ਖਾਨਾ_ਵਿੱਥ = (ਪੂਰਨ)Py_Unicode_GET_ਲੰਮਾਈ(errtext);
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਖਾਨਾ_ਵਿੱਥ > 0) {
        ਖਾਨਾ_ਵਿੱਥ = byte_ਖਾਨਾ_ਮੰਜ਼ਿਲ_char_ਖਾਨਾ(ਸਤਰ, ਖਾਨਾ_ਵਿੱਥ);
    }
    ਜੇ (ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ == -1) {
        ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ = ਖਾਨਾ_ਵਿੱਥ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ > 0) {
        ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ = byte_ਖਾਨਾ_ਮੰਜ਼ਿਲ_char_ਖਾਨਾ(ਸਤਰ, ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ);
    }

    Py_ssize_t ਸਤਰ_len = strcspn(ਸਤਰ, "\n");
    ਜੇ (ਸਤਰ_len != cursor) {
        Py_DECREF(errtext);
        errtext = PyUnicode_DecodeUTF8(ਸਤਰ, ਸਤਰ_len,
                                       "replace");
    }
    ਜੇ (!errtext) {
        ਜਾਓ error;
    }

    ਦਲੀਲਾਂ = Py_Build_ਮੁੱਲ("(O(OiiNii))", errmsg,
                         tok->filename ? tok->filename : Py_None,
                         lineno, ਖਾਨਾ_ਵਿੱਥ, errtext,
                         lineno, ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ);
    ਜੇ (ਦਲੀਲਾਂ) {
        Py_Err_ਸੈੱਟ_Object(PyExc_SyntaxError, ਦਲੀਲਾਂ);
        Py_DECREF(ਦਲੀਲਾਂ);
    }

error:
    Py_XDECREF(errmsg);
    tok->ਪੂਰਾ = E_ERROR;
    ਵਾਪਸ ERRORTOKEN;
}

ਪੂਰਨ
_PyTokenizer_syntaxerror(ਬਣਤਰ tok_state *tok, ਅਚਲ ਅੱਖਰ *format, ...)
{
    // These errors are cleaned on startup. Todo: Fix it.
    va_list vargs;
    va_ਸ਼ੁਰੂ(vargs, format);
    ਪੂਰਨ ret = _syntaxerror_range(
        tok, Py_Tok_ਸਰੋਤ_ਸੂਚਕ(&tok->ਸਰੋਤ, tok->ਸਤਰ_ਸ਼ੁਰੂ),
        tok->cur - tok->ਸਤਰ_ਸ਼ੁਰੂ, tok->lineno, format, -1, -1, vargs);
    va_ਸਿਰਾ(vargs);
    ਵਾਪਸ ret;
}

ਪੂਰਨ
Py_Tokenizer_syntaxerror_ਗਿਆਤ_range(ਬਣਤਰ tok_state *tok,
                        ਪੂਰਨ ਖਾਨਾ_ਵਿੱਥ, ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ,
                        ਅਚਲ ਅੱਖਰ *format, ...)
{
    va_list vargs;
    va_ਸ਼ੁਰੂ(vargs, format);
    ਪੂਰਨ ret = _syntaxerror_range(
        tok, Py_Tok_ਸਰੋਤ_ਸੂਚਕ(&tok->ਸਰੋਤ, tok->ਸਤਰ_ਸ਼ੁਰੂ),
        tok->cur - tok->ਸਤਰ_ਸ਼ੁਰੂ, tok->lineno,
        format, ਖਾਨਾ_ਵਿੱਥ, ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, vargs);
    va_ਸਿਰਾ(vargs);
    ਵਾਪਸ ret;
}

ਪੂਰਨ
_PyTokenizer_syntaxerror_at(ਬਣਤਰ tok_state *tok, ਅਚਲ ਅੱਖਰ *ਸਤਰ,
                             Py_ssize_t cursor, ਪੂਰਨ lineno,
                             ਪੂਰਨ ਖਾਨਾ_ਵਿੱਥ, ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ,
                             ਅਚਲ ਅੱਖਰ *format, ...)
{
    va_list vargs;
    va_ਸ਼ੁਰੂ(vargs, format);
    ਪੂਰਨ ret = _syntaxerror_range(tok, ਸਤਰ, cursor, lineno, format,
                                ਖਾਨਾ_ਵਿੱਥ, ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, vargs);
    va_ਸਿਰਾ(vargs);
    ਵਾਪਸ ret;
}

ਪੂਰਨ
_PyTokenizer_indenterror(ਬਣਤਰ tok_state *tok)
{
    tok->ਪੂਰਾ = E_TABSPACE;
    tok->cur = tok->inp;
    ਵਾਪਸ ERRORTOKEN;
}

ਪੂਰਨ
_PyTokenizer_warn_invalid_escape_sequence(ਬਣਤਰ tok_state *tok, ਪੂਰਨ ਪਹਿਲਾ_invalid_escape_char)
{
    PyObject *ਸੁਨੇਹਾ = PyUnicode_FromFormat(
        "\"\\%c\" is an invalid escape sequence. "
        "Such sequences will not work in the future. "
        "Did you mean \"\\\\%c\"? A raw string is also an option.",
        (ਅੱਖਰ) ਪਹਿਲਾ_invalid_escape_char,
        (ਅੱਖਰ) ਪਹਿਲਾ_invalid_escape_char
    );

    ਜੇ (ਸੁਨੇਹਾ == ਨਿਰਾ) {
        ਵਾਪਸ -1;
    }

    ਜੇ (PyErr_WarnExplicitObject(PyExc_SyntaxWarning, ਸੁਨੇਹਾ, tok->filename,
                                 tok->lineno, tok->module, ਨਿਰਾ) < 0) {
        Py_DECREF(ਸੁਨੇਹਾ);

        ਜੇ (PyErr_ExceptionMatches(PyExc_SyntaxWarning)) {
            /* Replace the SyntaxWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            ਵਾਪਸ _PyTokenizer_syntaxerror(tok,
                "\"\\%c\" is an invalid escape sequence. "
                "Did you mean \"\\\\%c\"? A raw string is also an option.",
                (ਅੱਖਰ) ਪਹਿਲਾ_invalid_escape_char,
                (ਅੱਖਰ) ਪਹਿਲਾ_invalid_escape_char);
        }

        ਵਾਪਸ -1;
    }

    Py_DECREF(ਸੁਨੇਹਾ);
    ਵਾਪਸ 0;
}

ਖਾਲੀ
_PyTokenizer_raise_init_error(PyObject *filename)
{
    ਜੇ (!(PyErr_ExceptionMatches(PyExc_LookupError)
          || PyErr_ExceptionMatches(PyExc_SyntaxError)
          || PyErr_ExceptionMatches(Py_Exc_ਮੁੱਲ_Error)
          || PyErr_ExceptionMatches(PyExc_UnicodeDecodeError))) {
        ਵਾਪਸ;
    }
    PyObject *errstr = ਨਿਰਾ;
    PyObject *tuple = ਨਿਰਾ;
    PyObject *type;
    PyObject *ਮੁੱਲ;
    PyObject *tback;
    PyErr_Fetch(&type, &ਮੁੱਲ, &tback);
    ਜੇ (PyErr_GivenExceptionMatches(ਮੁੱਲ, PyExc_SyntaxError)) {
        ਜੇ (Py_Object_ਸੈੱਟ_Attr(ਮੁੱਲ, &_Py_ID(filename), filename)) {
            ਜਾਓ error;
        }
        PyErr_Restore(type, ਮੁੱਲ, tback);
        ਵਾਪਸ;
    }
    errstr = PyObject_Str(ਮੁੱਲ);
    ਜੇ (!errstr) {
        ਜਾਓ error;
    }

    PyObject *tmp = Py_Build_ਮੁੱਲ("(OiiO)", filename, 0, -1, Py_None);
    ਜੇ (!tmp) {
        ਜਾਓ error;
    }

    tuple = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    ਜੇ (!tuple) {
        ਜਾਓ error;
    }
    Py_Err_ਸੈੱਟ_Object(PyExc_SyntaxError, tuple);

error:
    Py_XDECREF(type);
    Py_XDECREF(ਮੁੱਲ);
    Py_XDECREF(tback);
    Py_XDECREF(errstr);
    Py_XDECREF(tuple);
}

ਪੂਰਨ
_PyTokenizer_parser_warn(ਬਣਤਰ tok_state *tok, PyObject *category, ਅਚਲ ਅੱਖਰ *format, ...)
{
    PyObject *errmsg;
    va_list vargs;
    va_ਸ਼ੁਰੂ(vargs, format);
    errmsg = PyUnicode_FromFormatV(format, vargs);
    va_ਸਿਰਾ(vargs);
    ਜੇ (!errmsg) {
        ਜਾਓ error;
    }

    ਜੇ (PyErr_WarnExplicitObject(category, errmsg, tok->filename,
                                 tok->lineno, tok->module, ਨਿਰਾ) < 0) {
        ਜੇ (PyErr_ExceptionMatches(category)) {
            /* Replace the DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();
            _PyTokenizer_syntaxerror(tok, "%U", errmsg);
        }
        ਜਾਓ error;
    }
    Py_DECREF(errmsg);
    ਵਾਪਸ 0;

error:
    Py_XDECREF(errmsg);
    tok->ਪੂਰਾ = E_ERROR;
    ਵਾਪਸ -1;
}


/* Check whether the characters at s start a valid
   UTF-8 sequence. Return the number of characters forming
   the sequence if yes, 0 if not.  The special cases match
   those in stringlib/codecs.h:utf8_decode.
*/
ਸਥਿਰ ਪੂਰਨ
valid_utf8(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ* s)
{
    ਪੂਰਨ expected = 0;
    ਪੂਰਨ ਲੰਮਾਈ;
    ਜੇ (*s < 0x80) {
        /* single-byte code */
        ਵਾਪਸ 1;
    }
    ਨਹੀਂਤਾਂ ਜੇ (*s < 0xE0) {
        /* \xC2\x80-\xDF\xBF -- 0080-07FF */
        ਜੇ (*s < 0xC2) {
            /* invalid sequence
               \x80-\xBF -- continuation byte
               \xC0-\xC1 -- fake 0000-007F */
            ਵਾਪਸ 0;
        }
        expected = 1;
    }
    ਨਹੀਂਤਾਂ ਜੇ (*s < 0xF0) {
        /* \xE0\xA0\x80-\xEF\xBF\xBF -- 0800-FFFF */
        ਜੇ (*s == 0xE0 && *(s + 1) < 0xA0) {
            /* invalid sequence
               \xE0\x80\x80-\xE0\x9F\xBF -- fake 0000-0800 */
            ਵਾਪਸ 0;
        }
        ਨਹੀਂਤਾਂ ਜੇ (*s == 0xED && *(s + 1) >= 0xA0) {
            /* Decoding UTF-8 sequences in range \xED\xA0\x80-\xED\xBF\xBF
               will result in surrogates in range D800-DFFF. Surrogates are
               not valid UTF-8 so they are rejected.
               See https://www.unicode.org/versions/Unicode5.2.0/ch03.pdf
               (table 3-7) and http://www.rfc-editor.org/rfc/rfc3629.txt */
            ਵਾਪਸ 0;
        }
        expected = 2;
    }
    ਨਹੀਂਤਾਂ ਜੇ (*s < 0xF5) {
        /* \xF0\x90\x80\x80-\xF4\x8F\xBF\xBF -- 10000-10FFFF */
        ਜੇ (*(s + 1) < 0x90 ? *s == 0xF0 : *s == 0xF4) {
            /* invalid sequence -- one of:
               \xF0\x80\x80\x80-\xF0\x8F\xBF\xBF -- fake 0000-FFFF
               \xF4\x90\x80\x80- -- 110000- overflow */
            ਵਾਪਸ 0;
        }
        expected = 3;
    }
    ਨਹੀਂਤਾਂ {
        /* invalid start byte */
        ਵਾਪਸ 0;
    }
    ਲੰਮਾਈ = expected + 1;
    ਲਈ (ਪੂਰਨ i = 1; i <= expected; i++) {
        ਜੇ (s[i] < 0x80 || s[i] >= 0xC0) {
            ਵਾਪਸ 0;
        }
    }
    ਵਾਪਸ ਲੰਮਾਈ;
}

ਪੂਰਨ
_PyTokenizer_ensure_utf8(ਅਚਲ ਅੱਖਰ *ਸਤਰ, ਬਣਤਰ tok_state *tok, ਪੂਰਨ lineno)
{
    ਅਚਲ ਅੱਖਰ *badchar = ਨਿਰਾ;
    ਅਚਲ ਅੱਖਰ *c;
    ਪੂਰਨ ਲੰਮਾਈ;
    ਅਚਲ ਅੱਖਰ *ਸਤਰ_ਸ਼ੁਰੂ = ਸਤਰ;
    ਲਈ (c = ਸਤਰ; *c; c += ਲੰਮਾਈ) {
        ਜੇ (!(ਲੰਮਾਈ = valid_utf8((ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)c))) {
            badchar = c;
            ਰੋਕੋ;
        }
        ਜੇ (*c == '\n') {
            lineno++;
            ਸਤਰ_ਸ਼ੁਰੂ = c + 1;
        }
    }
    ਜੇ (badchar) {
        _PyTokenizer_syntaxerror_at(
                tok, ਸਤਰ_ਸ਼ੁਰੂ, badchar - ਸਤਰ_ਸ਼ੁਰੂ + 1, lineno,
                -1, -1,
                "Non-UTF-8 code starting with '\\x%.2x'"
                "%s%V on line %i, "
                "but no encoding declared; "
                "see https://peps.python.org/pep-0263/ for details",
                (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)*badchar,
                tok->filename ? " in file " : "", tok->filename, "",
                lineno);
        ਵਾਪਸ 0;
    }
    ਵਾਪਸ 1;
}


/* ############## DEBUGGING STUFF ############## */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ Py_DEBUG
ਖਾਲੀ
_PyTokenizer_print_escape(FILE *f, ਅਚਲ ਅੱਖਰ *s, Py_ssize_t ਮਾਪ)
{
    ਜੇ (s == ਨਿਰਾ) {
        fputs("NULL", f);
        ਵਾਪਸ;
    }
    putc('"', f);
    ਜਦੋਂ (ਮਾਪ-- > 0) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c = *s++;
        ਚੋਣ (c) {
            ਹਾਲਤ '\n': fputs("\\n", f); ਰੋਕੋ;
            ਹਾਲਤ '\r': fputs("\\r", f); ਰੋਕੋ;
            ਹਾਲਤ '\t': fputs("\\t", f); ਰੋਕੋ;
            ਹਾਲਤ '\f': fputs("\\f", f); ਰੋਕੋ;
            ਹਾਲਤ '\'': fputs("\\'", f); ਰੋਕੋ;
            ਹਾਲਤ '"': fputs("\\\"", f); ਰੋਕੋ;
            ਮੂਲ:
                ਜੇ (0x20 <= c && c <= 0x7f)
                    putc(c, f);
                ਨਹੀਂਤਾਂ
                    fprintf(f, "\\x%02x", c);
        }
    }
    putc('"', f);
}
#ਜੇ_ਅੰਤ
