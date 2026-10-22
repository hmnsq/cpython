#include "Python.h"
#include "errcode.h"
#include "pycore_runtime.h"       // _Py_ID()
#include "pycore_token.h"
#include "pycore_tuple.h"         // _PyTuple_FromPair

#include "../lexer/state.h"


/* ############## ERRORS ############## */

/* Convert a 1-based column in bytes into a 1-based column in characters.
   The line is UTF-8 encoded, so it is enough to skip continuation bytes. */
static ціле
byte_col_to_char_col(const char *line, ціле byte_col)
{
    ціле char_col = 1;
    для (ціле i = 0; i < byte_col - 1; i++) {
        якщо ((line[i] & 0xC0) != 0x80) {
            char_col++;
        }
    }
    повернути char_col;
}

static ціле
_syntaxerror_range(struct tok_state *tok, const char *line,
                   Py_ssize_t cursor, ціле lineno, const char *format,
                   ціле col_offset, ціле end_col_offset,
                   va_list vargs)
{
    // In release builds, we don't want to overwrite a previous error, but in debug builds we
    // want to fail if we are not doing it so we can fix it.
    assert(tok->done != E_ERROR);
    якщо (tok->done == E_ERROR) {
        повернути ERRORTOKEN;
    }
    PyObject *errmsg, *errtext, *args;
    errmsg = PyUnicode_FromFormatV(format, vargs);
    якщо (!errmsg) {
        goto error;
    }

    errtext = PyUnicode_DecodeUTF8(line, cursor,
                                   "replace");
    якщо (!errtext) {
        goto error;
    }

    якщо (col_offset == -1) {
        col_offset = (ціле)PyUnicode_GET_LENGTH(errtext);
    }
    інакше якщо (col_offset > 0) {
        col_offset = byte_col_to_char_col(line, col_offset);
    }
    якщо (end_col_offset == -1) {
        end_col_offset = col_offset;
    }
    інакше якщо (end_col_offset > 0) {
        end_col_offset = byte_col_to_char_col(line, end_col_offset);
    }

    Py_ssize_t line_len = strcspn(line, "\n");
    якщо (line_len != cursor) {
        Py_DECREF(errtext);
        errtext = PyUnicode_DecodeUTF8(line, line_len,
                                       "replace");
    }
    якщо (!errtext) {
        goto error;
    }

    args = Py_BuildValue("(O(OiiNii))", errmsg,
                         tok->filename ? tok->filename : Py_None,
                         lineno, col_offset, errtext,
                         lineno, end_col_offset);
    якщо (args) {
        PyErr_SetObject(PyExc_SyntaxError, args);
        Py_DECREF(args);
    }

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    повернути ERRORTOKEN;
}

ціле
_PyTokenizer_syntaxerror(struct tok_state *tok, const char *format, ...)
{
    // These errors are cleaned on startup. Todo: Fix it.
    va_list vargs;
    va_start(vargs, format);
    ціле ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
        tok->cur - tok->line_start, tok->lineno, format, -1, -1, vargs);
    va_end(vargs);
    повернути ret;
}

ціле
_PyTokenizer_syntaxerror_known_range(struct tok_state *tok,
                        ціле col_offset, ціле end_col_offset,
                        const char *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    ціле ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
        tok->cur - tok->line_start, tok->lineno,
        format, col_offset, end_col_offset, vargs);
    va_end(vargs);
    повернути ret;
}

ціле
_PyTokenizer_syntaxerror_at(struct tok_state *tok, const char *line,
                             Py_ssize_t cursor, ціле lineno,
                             ціле col_offset, ціле end_col_offset,
                             const char *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    ціле ret = _syntaxerror_range(tok, line, cursor, lineno, format,
                                col_offset, end_col_offset, vargs);
    va_end(vargs);
    повернути ret;
}

ціле
_PyTokenizer_indenterror(struct tok_state *tok)
{
    tok->done = E_TABSPACE;
    tok->cur = tok->inp;
    повернути ERRORTOKEN;
}

ціле
_PyTokenizer_warn_invalid_escape_sequence(struct tok_state *tok, ціле first_invalid_escape_char)
{
    PyObject *msg = PyUnicode_FromFormat(
        "\"\\%c\" is an invalid escape sequence. "
        "Such sequences will not work in the future. "
        "Did you mean \"\\\\%c\"? A raw string is also an option.",
        (char) first_invalid_escape_char,
        (char) first_invalid_escape_char
    );

    якщо (msg == NULL) {
        повернути -1;
    }

    якщо (PyErr_WarnExplicitObject(PyExc_SyntaxWarning, msg, tok->filename,
                                 tok->lineno, tok->module, NULL) < 0) {
        Py_DECREF(msg);

        якщо (PyErr_ExceptionMatches(PyExc_SyntaxWarning)) {
            /* Replace the SyntaxWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            повернути _PyTokenizer_syntaxerror(tok,
                "\"\\%c\" is an invalid escape sequence. "
                "Did you mean \"\\\\%c\"? A raw string is also an option.",
                (char) first_invalid_escape_char,
                (char) first_invalid_escape_char);
        }

        повернути -1;
    }

    Py_DECREF(msg);
    повернути 0;
}

void
_PyTokenizer_raise_init_error(PyObject *filename)
{
    якщо (!(PyErr_ExceptionMatches(PyExc_LookupError)
          || PyErr_ExceptionMatches(PyExc_SyntaxError)
          || PyErr_ExceptionMatches(PyExc_ValueError)
          || PyErr_ExceptionMatches(PyExc_UnicodeDecodeError))) {
        повернути;
    }
    PyObject *errstr = NULL;
    PyObject *tuple = NULL;
    PyObject *type;
    PyObject *value;
    PyObject *tback;
    PyErr_Fetch(&type, &value, &tback);
    якщо (PyErr_GivenExceptionMatches(value, PyExc_SyntaxError)) {
        якщо (PyObject_SetAttr(value, &_Py_ID(filename), filename)) {
            goto error;
        }
        PyErr_Restore(type, value, tback);
        повернути;
    }
    errstr = PyObject_Str(value);
    якщо (!errstr) {
        goto error;
    }

    PyObject *tmp = Py_BuildValue("(OiiO)", filename, 0, -1, Py_None);
    якщо (!tmp) {
        goto error;
    }

    tuple = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    якщо (!tuple) {
        goto error;
    }
    PyErr_SetObject(PyExc_SyntaxError, tuple);

error:
    Py_XDECREF(type);
    Py_XDECREF(value);
    Py_XDECREF(tback);
    Py_XDECREF(errstr);
    Py_XDECREF(tuple);
}

ціле
_PyTokenizer_parser_warn(struct tok_state *tok, PyObject *category, const char *format, ...)
{
    PyObject *errmsg;
    va_list vargs;
    va_start(vargs, format);
    errmsg = PyUnicode_FromFormatV(format, vargs);
    va_end(vargs);
    якщо (!errmsg) {
        goto error;
    }

    якщо (PyErr_WarnExplicitObject(category, errmsg, tok->filename,
                                 tok->lineno, tok->module, NULL) < 0) {
        якщо (PyErr_ExceptionMatches(category)) {
            /* Replace the DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();
            _PyTokenizer_syntaxerror(tok, "%U", errmsg);
        }
        goto error;
    }
    Py_DECREF(errmsg);
    повернути 0;

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    повернути -1;
}


/* Check whether the characters at s start a valid
   UTF-8 sequence. Return the number of characters forming
   the sequence if yes, 0 if not.  The special cases match
   those in stringlib/codecs.h:utf8_decode.
*/
static ціле
valid_utf8(const unsigned char* s)
{
    ціле expected = 0;
    ціле length;
    якщо (*s < 0x80) {
        /* single-byte code */
        повернути 1;
    }
    інакше якщо (*s < 0xE0) {
        /* \xC2\x80-\xDF\xBF -- 0080-07FF */
        якщо (*s < 0xC2) {
            /* invalid sequence
               \x80-\xBF -- continuation byte
               \xC0-\xC1 -- fake 0000-007F */
            повернути 0;
        }
        expected = 1;
    }
    інакше якщо (*s < 0xF0) {
        /* \xE0\xA0\x80-\xEF\xBF\xBF -- 0800-FFFF */
        якщо (*s == 0xE0 && *(s + 1) < 0xA0) {
            /* invalid sequence
               \xE0\x80\x80-\xE0\x9F\xBF -- fake 0000-0800 */
            повернути 0;
        }
        інакше якщо (*s == 0xED && *(s + 1) >= 0xA0) {
            /* Decoding UTF-8 sequences in range \xED\xA0\x80-\xED\xBF\xBF
               will result in surrogates in range D800-DFFF. Surrogates are
               not valid UTF-8 so they are rejected.
               See https://www.unicode.org/versions/Unicode5.2.0/ch03.pdf
               (table 3-7) and http://www.rfc-editor.org/rfc/rfc3629.txt */
            повернути 0;
        }
        expected = 2;
    }
    інакше якщо (*s < 0xF5) {
        /* \xF0\x90\x80\x80-\xF4\x8F\xBF\xBF -- 10000-10FFFF */
        якщо (*(s + 1) < 0x90 ? *s == 0xF0 : *s == 0xF4) {
            /* invalid sequence -- one of:
               \xF0\x80\x80\x80-\xF0\x8F\xBF\xBF -- fake 0000-FFFF
               \xF4\x90\x80\x80- -- 110000- overflow */
            повернути 0;
        }
        expected = 3;
    }
    інакше {
        /* invalid start byte */
        повернути 0;
    }
    length = expected + 1;
    для (ціле i = 1; i <= expected; i++) {
        якщо (s[i] < 0x80 || s[i] >= 0xC0) {
            повернути 0;
        }
    }
    повернути length;
}

ціле
_PyTokenizer_ensure_utf8(const char *line, struct tok_state *tok, ціле lineno)
{
    const char *badchar = NULL;
    const char *c;
    ціле length;
    const char *line_start = line;
    для (c = line; *c; c += length) {
        якщо (!(length = valid_utf8((const unsigned char *)c))) {
            badchar = c;
            break;
        }
        якщо (*c == '\n') {
            lineno++;
            line_start = c + 1;
        }
    }
    якщо (badchar) {
        _PyTokenizer_syntaxerror_at(
                tok, line_start, badchar - line_start + 1, lineno,
                -1, -1,
                "Non-UTF-8 code starting with '\\x%.2x'"
                "%s%V on line %i, "
                "but no encoding declared; "
                "see https://peps.python.org/pep-0263/ for details",
                (unsigned char)*badchar,
                tok->filename ? " in file " : "", tok->filename, "",
                lineno);
        повернути 0;
    }
    повернути 1;
}


/* ############## DEBUGGING STUFF ############## */

#ifdef Py_DEBUG
void
_PyTokenizer_print_escape(FILE *f, const char *s, Py_ssize_t size)
{
    якщо (s == NULL) {
        fputs("NULL", f);
        повернути;
    }
    putc('"', f);
    поки (size-- > 0) {
        unsigned char c = *s++;
        switch (c) {
            case '\n': fputs("\\n", f); break;
            case '\r': fputs("\\r", f); break;
            case '\t': fputs("\\t", f); break;
            case '\f': fputs("\\f", f); break;
            case '\'': fputs("\\'", f); break;
            case '"': fputs("\\\"", f); break;
            default:
                якщо (0x20 <= c && c <= 0x7f)
                    putc(c, f);
                інакше
                    fprintf(f, "\\x%02x", c);
        }
    }
    putc('"', f);
}
#endif
