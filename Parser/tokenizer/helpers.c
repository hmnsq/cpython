#include "Python.h"
#include "errcode.h"
#include "pycore_runtime.h"       // _Py_ID()
#include "pycore_token.h"
#include "pycore_tuple.h"         // _PyTuple_FromPair

#include "../lexer/state.h"


/* ############## ERRORS ############## */

/* Convert a 1-based column in bytes into a 1-based column in characters.
   The line is UTF-8 encoded, so it is enough to skip continuation bytes. */
static 정수
byte_col_to_char_col(const char *line, 정수 byte_col)
{
    정수 char_col = 1;
    위해 (정수 i = 0; i < byte_col - 1; i++) {
        만약 ((line[i] & 0xC0) != 0x80) {
            char_col++;
        }
    }
    반환 char_col;
}

static 정수
_syntaxerror_range(struct tok_state *tok, const char *line,
                   Py_ssize_t cursor, 정수 lineno, const char *format,
                   정수 col_offset, 정수 end_col_offset,
                   va_list vargs)
{
    // In release builds, we don't want to overwrite a previous error, but in debug builds we
    // want to fail if we are not doing it so we can fix it.
    assert(tok->done != E_ERROR);
    만약 (tok->done == E_ERROR) {
        반환 ERRORTOKEN;
    }
    PyObject *errmsg, *errtext, *args;
    errmsg = PyUnicode_FromFormatV(format, vargs);
    만약 (!errmsg) {
        goto error;
    }

    errtext = PyUnicode_DecodeUTF8(line, cursor,
                                   "replace");
    만약 (!errtext) {
        goto error;
    }

    만약 (col_offset == -1) {
        col_offset = (정수)PyUnicode_GET_LENGTH(errtext);
    }
    아니면 만약 (col_offset > 0) {
        col_offset = byte_col_to_char_col(line, col_offset);
    }
    만약 (end_col_offset == -1) {
        end_col_offset = col_offset;
    }
    아니면 만약 (end_col_offset > 0) {
        end_col_offset = byte_col_to_char_col(line, end_col_offset);
    }

    Py_ssize_t line_len = strcspn(line, "\n");
    만약 (line_len != cursor) {
        Py_DECREF(errtext);
        errtext = PyUnicode_DecodeUTF8(line, line_len,
                                       "replace");
    }
    만약 (!errtext) {
        goto error;
    }

    args = Py_BuildValue("(O(OiiNii))", errmsg,
                         tok->filename ? tok->filename : Py_None,
                         lineno, col_offset, errtext,
                         lineno, end_col_offset);
    만약 (args) {
        PyErr_SetObject(PyExc_SyntaxError, args);
        Py_DECREF(args);
    }

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    반환 ERRORTOKEN;
}

정수
_PyTokenizer_syntaxerror(struct tok_state *tok, const char *format, ...)
{
    // These errors are cleaned on startup. Todo: Fix it.
    va_list vargs;
    va_start(vargs, format);
    정수 ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
        tok->cur - tok->line_start, tok->lineno, format, -1, -1, vargs);
    va_end(vargs);
    반환 ret;
}

정수
_PyTokenizer_syntaxerror_known_range(struct tok_state *tok,
                        정수 col_offset, 정수 end_col_offset,
                        const char *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    정수 ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
        tok->cur - tok->line_start, tok->lineno,
        format, col_offset, end_col_offset, vargs);
    va_end(vargs);
    반환 ret;
}

정수
_PyTokenizer_syntaxerror_at(struct tok_state *tok, const char *line,
                             Py_ssize_t cursor, 정수 lineno,
                             정수 col_offset, 정수 end_col_offset,
                             const char *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    정수 ret = _syntaxerror_range(tok, line, cursor, lineno, format,
                                col_offset, end_col_offset, vargs);
    va_end(vargs);
    반환 ret;
}

정수
_PyTokenizer_indenterror(struct tok_state *tok)
{
    tok->done = E_TABSPACE;
    tok->cur = tok->inp;
    반환 ERRORTOKEN;
}

정수
_PyTokenizer_warn_invalid_escape_sequence(struct tok_state *tok, 정수 first_invalid_escape_char)
{
    PyObject *msg = PyUnicode_FromFormat(
        "\"\\%c\" is an invalid escape sequence. "
        "Such sequences will not work in the future. "
        "Did you mean \"\\\\%c\"? A raw string is also an option.",
        (char) first_invalid_escape_char,
        (char) first_invalid_escape_char
    );

    만약 (msg == NULL) {
        반환 -1;
    }

    만약 (PyErr_WarnExplicitObject(PyExc_SyntaxWarning, msg, tok->filename,
                                 tok->lineno, tok->module, NULL) < 0) {
        Py_DECREF(msg);

        만약 (PyErr_ExceptionMatches(PyExc_SyntaxWarning)) {
            /* Replace the SyntaxWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            반환 _PyTokenizer_syntaxerror(tok,
                "\"\\%c\" is an invalid escape sequence. "
                "Did you mean \"\\\\%c\"? A raw string is also an option.",
                (char) first_invalid_escape_char,
                (char) first_invalid_escape_char);
        }

        반환 -1;
    }

    Py_DECREF(msg);
    반환 0;
}

void
_PyTokenizer_raise_init_error(PyObject *filename)
{
    만약 (!(PyErr_ExceptionMatches(PyExc_LookupError)
          || PyErr_ExceptionMatches(PyExc_SyntaxError)
          || PyErr_ExceptionMatches(PyExc_ValueError)
          || PyErr_ExceptionMatches(PyExc_UnicodeDecodeError))) {
        반환;
    }
    PyObject *errstr = NULL;
    PyObject *tuple = NULL;
    PyObject *type;
    PyObject *value;
    PyObject *tback;
    PyErr_Fetch(&type, &value, &tback);
    만약 (PyErr_GivenExceptionMatches(value, PyExc_SyntaxError)) {
        만약 (PyObject_SetAttr(value, &_Py_ID(filename), filename)) {
            goto error;
        }
        PyErr_Restore(type, value, tback);
        반환;
    }
    errstr = PyObject_Str(value);
    만약 (!errstr) {
        goto error;
    }

    PyObject *tmp = Py_BuildValue("(OiiO)", filename, 0, -1, Py_None);
    만약 (!tmp) {
        goto error;
    }

    tuple = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    만약 (!tuple) {
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

정수
_PyTokenizer_parser_warn(struct tok_state *tok, PyObject *category, const char *format, ...)
{
    PyObject *errmsg;
    va_list vargs;
    va_start(vargs, format);
    errmsg = PyUnicode_FromFormatV(format, vargs);
    va_end(vargs);
    만약 (!errmsg) {
        goto error;
    }

    만약 (PyErr_WarnExplicitObject(category, errmsg, tok->filename,
                                 tok->lineno, tok->module, NULL) < 0) {
        만약 (PyErr_ExceptionMatches(category)) {
            /* Replace the DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();
            _PyTokenizer_syntaxerror(tok, "%U", errmsg);
        }
        goto error;
    }
    Py_DECREF(errmsg);
    반환 0;

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    반환 -1;
}


/* Check whether the characters at s start a valid
   UTF-8 sequence. Return the number of characters forming
   the sequence if yes, 0 if not.  The special cases match
   those in stringlib/codecs.h:utf8_decode.
*/
static 정수
valid_utf8(const unsigned char* s)
{
    정수 expected = 0;
    정수 length;
    만약 (*s < 0x80) {
        /* single-byte code */
        반환 1;
    }
    아니면 만약 (*s < 0xE0) {
        /* \xC2\x80-\xDF\xBF -- 0080-07FF */
        만약 (*s < 0xC2) {
            /* invalid sequence
               \x80-\xBF -- continuation byte
               \xC0-\xC1 -- fake 0000-007F */
            반환 0;
        }
        expected = 1;
    }
    아니면 만약 (*s < 0xF0) {
        /* \xE0\xA0\x80-\xEF\xBF\xBF -- 0800-FFFF */
        만약 (*s == 0xE0 && *(s + 1) < 0xA0) {
            /* invalid sequence
               \xE0\x80\x80-\xE0\x9F\xBF -- fake 0000-0800 */
            반환 0;
        }
        아니면 만약 (*s == 0xED && *(s + 1) >= 0xA0) {
            /* Decoding UTF-8 sequences in range \xED\xA0\x80-\xED\xBF\xBF
               will result in surrogates in range D800-DFFF. Surrogates are
               not valid UTF-8 so they are rejected.
               See https://www.unicode.org/versions/Unicode5.2.0/ch03.pdf
               (table 3-7) and http://www.rfc-editor.org/rfc/rfc3629.txt */
            반환 0;
        }
        expected = 2;
    }
    아니면 만약 (*s < 0xF5) {
        /* \xF0\x90\x80\x80-\xF4\x8F\xBF\xBF -- 10000-10FFFF */
        만약 (*(s + 1) < 0x90 ? *s == 0xF0 : *s == 0xF4) {
            /* invalid sequence -- one of:
               \xF0\x80\x80\x80-\xF0\x8F\xBF\xBF -- fake 0000-FFFF
               \xF4\x90\x80\x80- -- 110000- overflow */
            반환 0;
        }
        expected = 3;
    }
    아니면 {
        /* invalid start byte */
        반환 0;
    }
    length = expected + 1;
    위해 (정수 i = 1; i <= expected; i++) {
        만약 (s[i] < 0x80 || s[i] >= 0xC0) {
            반환 0;
        }
    }
    반환 length;
}

정수
_PyTokenizer_ensure_utf8(const char *line, struct tok_state *tok, 정수 lineno)
{
    const char *badchar = NULL;
    const char *c;
    정수 length;
    const char *line_start = line;
    위해 (c = line; *c; c += length) {
        만약 (!(length = valid_utf8((const unsigned char *)c))) {
            badchar = c;
            break;
        }
        만약 (*c == '\n') {
            lineno++;
            line_start = c + 1;
        }
    }
    만약 (badchar) {
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
        반환 0;
    }
    반환 1;
}


/* ############## DEBUGGING STUFF ############## */

#ifdef Py_DEBUG
void
_PyTokenizer_print_escape(FILE *f, const char *s, Py_ssize_t size)
{
    만약 (s == NULL) {
        fputs("NULL", f);
        반환;
    }
    putc('"', f);
    동안 (size-- > 0) {
        unsigned char c = *s++;
        switch (c) {
            case '\n': fputs("\\n", f); break;
            case '\r': fputs("\\r", f); break;
            case '\t': fputs("\\t", f); break;
            case '\f': fputs("\\f", f); break;
            case '\'': fputs("\\'", f); break;
            case '"': fputs("\\\"", f); break;
            default:
                만약 (0x20 <= c && c <= 0x7f)
                    putc(c, f);
                아니면
                    fprintf(f, "\\x%02x", c);
        }
    }
    putc('"', f);
}
#endif
