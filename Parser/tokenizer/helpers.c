#include "Python.h"
#include "errcode.h"
#include "pycore_runtime.h"       // _Py_ID()
#include "pycore_token.h"
#include "pycore_tuple.h"         // _PyTuple_FromPair

#include "../lexer/state.h"


/* ############## ERRORS ############## */

/* Convert a 1-based column in bytes into a 1-based column in characters.
   The line is UTF-8 encoded, so it is enough to skip continuation bytes. */
static số_nguyên
byte_col_to_char_col(const char *line, số_nguyên byte_col)
{
    số_nguyên char_col = 1;
    cho (số_nguyên i = 0; i < byte_col - 1; i++) {
        nếu ((line[i] & 0xC0) != 0x80) {
            char_col++;
        }
    }
    trả_về char_col;
}

static số_nguyên
_syntaxerror_range(struct tok_state *tok, const char *line,
                   Py_ssize_t cursor, số_nguyên lineno, const char *format,
                   số_nguyên col_offset, số_nguyên end_col_offset,
                   va_list vargs)
{
    // In release builds, we don't want to overwrite a previous error, but in debug builds we
    // want to fail if we are not doing it so we can fix it.
    assert(tok->done != E_ERROR);
    nếu (tok->done == E_ERROR) {
        trả_về ERRORTOKEN;
    }
    PyObject *errmsg, *errtext, *args;
    errmsg = PyUnicode_FromFormatV(format, vargs);
    nếu (!errmsg) {
        goto error;
    }

    errtext = PyUnicode_DecodeUTF8(line, cursor,
                                   "replace");
    nếu (!errtext) {
        goto error;
    }

    nếu (col_offset == -1) {
        col_offset = (số_nguyên)PyUnicode_GET_LENGTH(errtext);
    }
    không_thì nếu (col_offset > 0) {
        col_offset = byte_col_to_char_col(line, col_offset);
    }
    nếu (end_col_offset == -1) {
        end_col_offset = col_offset;
    }
    không_thì nếu (end_col_offset > 0) {
        end_col_offset = byte_col_to_char_col(line, end_col_offset);
    }

    Py_ssize_t line_len = strcspn(line, "\n");
    nếu (line_len != cursor) {
        Py_DECREF(errtext);
        errtext = PyUnicode_DecodeUTF8(line, line_len,
                                       "replace");
    }
    nếu (!errtext) {
        goto error;
    }

    args = Py_BuildValue("(O(OiiNii))", errmsg,
                         tok->filename ? tok->filename : Py_None,
                         lineno, col_offset, errtext,
                         lineno, end_col_offset);
    nếu (args) {
        PyErr_SetObject(PyExc_SyntaxError, args);
        Py_DECREF(args);
    }

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    trả_về ERRORTOKEN;
}

số_nguyên
_PyTokenizer_syntaxerror(struct tok_state *tok, const char *format, ...)
{
    // These errors are cleaned on startup. Todo: Fix it.
    va_list vargs;
    va_start(vargs, format);
    số_nguyên ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
        tok->cur - tok->line_start, tok->lineno, format, -1, -1, vargs);
    va_end(vargs);
    trả_về ret;
}

số_nguyên
_PyTokenizer_syntaxerror_known_range(struct tok_state *tok,
                        số_nguyên col_offset, số_nguyên end_col_offset,
                        const char *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    số_nguyên ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
        tok->cur - tok->line_start, tok->lineno,
        format, col_offset, end_col_offset, vargs);
    va_end(vargs);
    trả_về ret;
}

số_nguyên
_PyTokenizer_syntaxerror_at(struct tok_state *tok, const char *line,
                             Py_ssize_t cursor, số_nguyên lineno,
                             số_nguyên col_offset, số_nguyên end_col_offset,
                             const char *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    số_nguyên ret = _syntaxerror_range(tok, line, cursor, lineno, format,
                                col_offset, end_col_offset, vargs);
    va_end(vargs);
    trả_về ret;
}

số_nguyên
_PyTokenizer_indenterror(struct tok_state *tok)
{
    tok->done = E_TABSPACE;
    tok->cur = tok->inp;
    trả_về ERRORTOKEN;
}

số_nguyên
_PyTokenizer_warn_invalid_escape_sequence(struct tok_state *tok, số_nguyên first_invalid_escape_char)
{
    PyObject *msg = PyUnicode_FromFormat(
        "\"\\%c\" is an invalid escape sequence. "
        "Such sequences will not work in the future. "
        "Did you mean \"\\\\%c\"? A raw string is also an option.",
        (char) first_invalid_escape_char,
        (char) first_invalid_escape_char
    );

    nếu (msg == NULL) {
        trả_về -1;
    }

    nếu (PyErr_WarnExplicitObject(PyExc_SyntaxWarning, msg, tok->filename,
                                 tok->lineno, tok->module, NULL) < 0) {
        Py_DECREF(msg);

        nếu (PyErr_ExceptionMatches(PyExc_SyntaxWarning)) {
            /* Replace the SyntaxWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            trả_về _PyTokenizer_syntaxerror(tok,
                "\"\\%c\" is an invalid escape sequence. "
                "Did you mean \"\\\\%c\"? A raw string is also an option.",
                (char) first_invalid_escape_char,
                (char) first_invalid_escape_char);
        }

        trả_về -1;
    }

    Py_DECREF(msg);
    trả_về 0;
}

void
_PyTokenizer_raise_init_error(PyObject *filename)
{
    nếu (!(PyErr_ExceptionMatches(PyExc_LookupError)
          || PyErr_ExceptionMatches(PyExc_SyntaxError)
          || PyErr_ExceptionMatches(PyExc_ValueError)
          || PyErr_ExceptionMatches(PyExc_UnicodeDecodeError))) {
        trả_về;
    }
    PyObject *errstr = NULL;
    PyObject *tuple = NULL;
    PyObject *type;
    PyObject *value;
    PyObject *tback;
    PyErr_Fetch(&type, &value, &tback);
    nếu (PyErr_GivenExceptionMatches(value, PyExc_SyntaxError)) {
        nếu (PyObject_SetAttr(value, &_Py_ID(filename), filename)) {
            goto error;
        }
        PyErr_Restore(type, value, tback);
        trả_về;
    }
    errstr = PyObject_Str(value);
    nếu (!errstr) {
        goto error;
    }

    PyObject *tmp = Py_BuildValue("(OiiO)", filename, 0, -1, Py_None);
    nếu (!tmp) {
        goto error;
    }

    tuple = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    nếu (!tuple) {
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

số_nguyên
_PyTokenizer_parser_warn(struct tok_state *tok, PyObject *category, const char *format, ...)
{
    PyObject *errmsg;
    va_list vargs;
    va_start(vargs, format);
    errmsg = PyUnicode_FromFormatV(format, vargs);
    va_end(vargs);
    nếu (!errmsg) {
        goto error;
    }

    nếu (PyErr_WarnExplicitObject(category, errmsg, tok->filename,
                                 tok->lineno, tok->module, NULL) < 0) {
        nếu (PyErr_ExceptionMatches(category)) {
            /* Replace the DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();
            _PyTokenizer_syntaxerror(tok, "%U", errmsg);
        }
        goto error;
    }
    Py_DECREF(errmsg);
    trả_về 0;

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    trả_về -1;
}


/* Check whether the characters at s start a valid
   UTF-8 sequence. Return the number of characters forming
   the sequence if yes, 0 if not.  The special cases match
   those in stringlib/codecs.h:utf8_decode.
*/
static số_nguyên
valid_utf8(const unsigned char* s)
{
    số_nguyên expected = 0;
    số_nguyên length;
    nếu (*s < 0x80) {
        /* single-byte code */
        trả_về 1;
    }
    không_thì nếu (*s < 0xE0) {
        /* \xC2\x80-\xDF\xBF -- 0080-07FF */
        nếu (*s < 0xC2) {
            /* invalid sequence
               \x80-\xBF -- continuation byte
               \xC0-\xC1 -- fake 0000-007F */
            trả_về 0;
        }
        expected = 1;
    }
    không_thì nếu (*s < 0xF0) {
        /* \xE0\xA0\x80-\xEF\xBF\xBF -- 0800-FFFF */
        nếu (*s == 0xE0 && *(s + 1) < 0xA0) {
            /* invalid sequence
               \xE0\x80\x80-\xE0\x9F\xBF -- fake 0000-0800 */
            trả_về 0;
        }
        không_thì nếu (*s == 0xED && *(s + 1) >= 0xA0) {
            /* Decoding UTF-8 sequences in range \xED\xA0\x80-\xED\xBF\xBF
               will result in surrogates in range D800-DFFF. Surrogates are
               not valid UTF-8 so they are rejected.
               See https://www.unicode.org/versions/Unicode5.2.0/ch03.pdf
               (table 3-7) and http://www.rfc-editor.org/rfc/rfc3629.txt */
            trả_về 0;
        }
        expected = 2;
    }
    không_thì nếu (*s < 0xF5) {
        /* \xF0\x90\x80\x80-\xF4\x8F\xBF\xBF -- 10000-10FFFF */
        nếu (*(s + 1) < 0x90 ? *s == 0xF0 : *s == 0xF4) {
            /* invalid sequence -- one of:
               \xF0\x80\x80\x80-\xF0\x8F\xBF\xBF -- fake 0000-FFFF
               \xF4\x90\x80\x80- -- 110000- overflow */
            trả_về 0;
        }
        expected = 3;
    }
    không_thì {
        /* invalid start byte */
        trả_về 0;
    }
    length = expected + 1;
    cho (số_nguyên i = 1; i <= expected; i++) {
        nếu (s[i] < 0x80 || s[i] >= 0xC0) {
            trả_về 0;
        }
    }
    trả_về length;
}

số_nguyên
_PyTokenizer_ensure_utf8(const char *line, struct tok_state *tok, số_nguyên lineno)
{
    const char *badchar = NULL;
    const char *c;
    số_nguyên length;
    const char *line_start = line;
    cho (c = line; *c; c += length) {
        nếu (!(length = valid_utf8((const unsigned char *)c))) {
            badchar = c;
            break;
        }
        nếu (*c == '\n') {
            lineno++;
            line_start = c + 1;
        }
    }
    nếu (badchar) {
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
        trả_về 0;
    }
    trả_về 1;
}


/* ############## DEBUGGING STUFF ############## */

#ifdef Py_DEBUG
void
_PyTokenizer_print_escape(FILE *f, const char *s, Py_ssize_t size)
{
    nếu (s == NULL) {
        fputs("NULL", f);
        trả_về;
    }
    putc('"', f);
    trong_khi (size-- > 0) {
        unsigned char c = *s++;
        switch (c) {
            case '\n': fputs("\\n", f); break;
            case '\r': fputs("\\r", f); break;
            case '\t': fputs("\\t", f); break;
            case '\f': fputs("\\f", f); break;
            case '\'': fputs("\\'", f); break;
            case '"': fputs("\\\"", f); break;
            default:
                nếu (0x20 <= c && c <= 0x7f)
                    putc(c, f);
                không_thì
                    fprintf(f, "\\x%02x", c);
        }
    }
    putc('"', f);
}
#endif
