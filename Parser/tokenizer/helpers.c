#include "Python.h"
#include "errcode.h"
#include "pycore_runtime.h"       // _Py_ID()
#include "pycore_token.h"
#include "pycore_tuple.h"         // _PyTuple_FromPair

#include "../lexer/state.h"


/* ############## ERRORS ############## */

/* Convert a 1-based column in bytes into a 1-based column in characters.
   The line is UTF-8 encoded, so it is enough to skip continuation bytes. */
static පූර්ණ_සංඛ්‍යා
byte_col_to_char_col(const var_na *line, පූර්ණ_සංඛ්‍යා byte_col)
{
    පූර්ණ_සංඛ්‍යා char_col = 1;
    සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < byte_col - 1; i++) {
        නම් ((line[i] & 0xC0) != 0x80) {
            char_col++;
        }
    }
    ආපසු char_col;
}

static පූර්ණ_සංඛ්‍යා
_syntaxerror_range(sa_nracanaa tok_state *tok, const var_na *line,
                   Py_ssize_t cursor, පූර්ණ_සංඛ්‍යා lineno, const var_na *format,
                   පූර්ණ_සංඛ්‍යා col_offset, පූර්ණ_සංඛ්‍යා end_col_offset,
                   va_list vargs)
{
    // In release builds, we don't want to overwrite a previous error, but in debug builds we
    // want to fail if we are not doing it so we can fix it.
    assert(tok->done != E_ERROR);
    නම් (tok->done == E_ERROR) {
        ආපසු ERRORTOKEN;
    }
    PyObject *errmsg, *errtext, *args;
    errmsg = PyUnicode_FromFormatV(format, vargs);
    නම් (!errmsg) {
        goto error;
    }

    errtext = PyUnicode_DecodeUTF8(line, cursor,
                                   "replace");
    නම් (!errtext) {
        goto error;
    }

    නම් (col_offset == -1) {
        col_offset = (පූර්ණ_සංඛ්‍යා)PyUnicode_GET_LENGTH(errtext);
    }
    නැතහොත් නම් (col_offset > 0) {
        col_offset = byte_col_to_char_col(line, col_offset);
    }
    නම් (end_col_offset == -1) {
        end_col_offset = col_offset;
    }
    නැතහොත් නම් (end_col_offset > 0) {
        end_col_offset = byte_col_to_char_col(line, end_col_offset);
    }

    Py_ssize_t line_len = strcspn(line, "\n");
    නම් (line_len != cursor) {
        Py_DECREF(errtext);
        errtext = PyUnicode_DecodeUTF8(line, line_len,
                                       "replace");
    }
    නම් (!errtext) {
        goto error;
    }

    args = Py_BuildValue("(O(OiiNii))", errmsg,
                         tok->filename ? tok->filename : Py_None,
                         lineno, col_offset, errtext,
                         lineno, end_col_offset);
    නම් (args) {
        PyErr_SetObject(PyExc_SyntaxError, args);
        Py_DECREF(args);
    }

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    ආපසු ERRORTOKEN;
}

පූර්ණ_සංඛ්‍යා
_PyTokenizer_syntaxerror(sa_nracanaa tok_state *tok, const var_na *format, ...)
{
    // These errors are cleaned on startup. Todo: Fix it.
    va_list vargs;
    va_start(vargs, format);
    පූර්ණ_සංඛ්‍යා ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
        tok->cur - tok->line_start, tok->lineno, format, -1, -1, vargs);
    va_end(vargs);
    ආපසු ret;
}

පූර්ණ_සංඛ්‍යා
_PyTokenizer_syntaxerror_known_range(sa_nracanaa tok_state *tok,
                        පූර්ණ_සංඛ්‍යා col_offset, පූර්ණ_සංඛ්‍යා end_col_offset,
                        const var_na *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    පූර්ණ_සංඛ්‍යා ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
        tok->cur - tok->line_start, tok->lineno,
        format, col_offset, end_col_offset, vargs);
    va_end(vargs);
    ආපසු ret;
}

පූර්ණ_සංඛ්‍යා
_PyTokenizer_syntaxerror_at(sa_nracanaa tok_state *tok, const var_na *line,
                             Py_ssize_t cursor, පූර්ණ_සංඛ්‍යා lineno,
                             පූර්ණ_සංඛ්‍යා col_offset, පූර්ණ_සංඛ්‍යා end_col_offset,
                             const var_na *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    පූර්ණ_සංඛ්‍යා ret = _syntaxerror_range(tok, line, cursor, lineno, format,
                                col_offset, end_col_offset, vargs);
    va_end(vargs);
    ආපසු ret;
}

පූර්ණ_සංඛ්‍යා
_PyTokenizer_indenterror(sa_nracanaa tok_state *tok)
{
    tok->done = E_TABSPACE;
    tok->cur = tok->inp;
    ආපසු ERRORTOKEN;
}

පූර්ණ_සංඛ්‍යා
_PyTokenizer_warn_invalid_escape_sequence(sa_nracanaa tok_state *tok, පූර්ණ_සංඛ්‍යා first_invalid_escape_char)
{
    PyObject *msg = PyUnicode_FromFormat(
        "\"\\%c\" is an invalid escape sequence. "
        "Such sequences will not work in the future. "
        "Did you mean \"\\\\%c\"? A raw string is also an option.",
        (var_na) first_invalid_escape_char,
        (var_na) first_invalid_escape_char
    );

    නම් (msg == NULL) {
        ආපසු -1;
    }

    නම් (PyErr_WarnExplicitObject(PyExc_SyntaxWarning, msg, tok->filename,
                                 tok->lineno, tok->module, NULL) < 0) {
        Py_DECREF(msg);

        නම් (PyErr_ExceptionMatches(PyExc_SyntaxWarning)) {
            /* Replace the SyntaxWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            ආපසු _PyTokenizer_syntaxerror(tok,
                "\"\\%c\" is an invalid escape sequence. "
                "Did you mean \"\\\\%c\"? A raw string is also an option.",
                (var_na) first_invalid_escape_char,
                (var_na) first_invalid_escape_char);
        }

        ආපසු -1;
    }

    Py_DECREF(msg);
    ආපසු 0;
}

void
_PyTokenizer_raise_init_error(PyObject *filename)
{
    නම් (!(PyErr_ExceptionMatches(PyExc_LookupError)
          || PyErr_ExceptionMatches(PyExc_SyntaxError)
          || PyErr_ExceptionMatches(PyExc_ValueError)
          || PyErr_ExceptionMatches(PyExc_UnicodeDecodeError))) {
        ආපසු;
    }
    PyObject *errstr = NULL;
    PyObject *tuple = NULL;
    PyObject *type;
    PyObject *value;
    PyObject *tback;
    PyErr_Fetch(&type, &value, &tback);
    නම් (PyErr_GivenExceptionMatches(value, PyExc_SyntaxError)) {
        නම් (PyObject_SetAttr(value, &_Py_ID(filename), filename)) {
            goto error;
        }
        PyErr_Restore(type, value, tback);
        ආපසු;
    }
    errstr = PyObject_Str(value);
    නම් (!errstr) {
        goto error;
    }

    PyObject *tmp = Py_BuildValue("(OiiO)", filename, 0, -1, Py_None);
    නම් (!tmp) {
        goto error;
    }

    tuple = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    නම් (!tuple) {
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

පූර්ණ_සංඛ්‍යා
_PyTokenizer_parser_warn(sa_nracanaa tok_state *tok, PyObject *category, const var_na *format, ...)
{
    PyObject *errmsg;
    va_list vargs;
    va_start(vargs, format);
    errmsg = PyUnicode_FromFormatV(format, vargs);
    va_end(vargs);
    නම් (!errmsg) {
        goto error;
    }

    නම් (PyErr_WarnExplicitObject(category, errmsg, tok->filename,
                                 tok->lineno, tok->module, NULL) < 0) {
        නම් (PyErr_ExceptionMatches(category)) {
            /* Replace the DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();
            _PyTokenizer_syntaxerror(tok, "%U", errmsg);
        }
        goto error;
    }
    Py_DECREF(errmsg);
    ආපසු 0;

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    ආපසු -1;
}


/* Check whether the characters at s start a valid
   UTF-8 sequence. Return the number of characters forming
   the sequence if yes, 0 if not.  The special cases match
   those in stringlib/codecs.h:utf8_decode.
*/
static පූර්ණ_සංඛ්‍යා
valid_utf8(const unsigned var_na* s)
{
    පූර්ණ_සංඛ්‍යා expected = 0;
    පූර්ණ_සංඛ්‍යා length;
    නම් (*s < 0x80) {
        /* single-byte code */
        ආපසු 1;
    }
    නැතහොත් නම් (*s < 0xE0) {
        /* \xC2\x80-\xDF\xBF -- 0080-07FF */
        නම් (*s < 0xC2) {
            /* invalid sequence
               \x80-\xBF -- continuation byte
               \xC0-\xC1 -- fake 0000-007F */
            ආපසු 0;
        }
        expected = 1;
    }
    නැතහොත් නම් (*s < 0xF0) {
        /* \xE0\xA0\x80-\xEF\xBF\xBF -- 0800-FFFF */
        නම් (*s == 0xE0 && *(s + 1) < 0xA0) {
            /* invalid sequence
               \xE0\x80\x80-\xE0\x9F\xBF -- fake 0000-0800 */
            ආපසු 0;
        }
        නැතහොත් නම් (*s == 0xED && *(s + 1) >= 0xA0) {
            /* Decoding UTF-8 sequences in range \xED\xA0\x80-\xED\xBF\xBF
               will result in surrogates in range D800-DFFF. Surrogates are
               not valid UTF-8 so they are rejected.
               See https://www.unicode.org/versions/Unicode5.2.0/ch03.pdf
               (table 3-7) and http://www.rfc-editor.org/rfc/rfc3629.txt */
            ආපසු 0;
        }
        expected = 2;
    }
    නැතහොත් නම් (*s < 0xF5) {
        /* \xF0\x90\x80\x80-\xF4\x8F\xBF\xBF -- 10000-10FFFF */
        නම් (*(s + 1) < 0x90 ? *s == 0xF0 : *s == 0xF4) {
            /* invalid sequence -- one of:
               \xF0\x80\x80\x80-\xF0\x8F\xBF\xBF -- fake 0000-FFFF
               \xF4\x90\x80\x80- -- 110000- overflow */
            ආපසු 0;
        }
        expected = 3;
    }
    නැතහොත් {
        /* invalid start byte */
        ආපසු 0;
    }
    length = expected + 1;
    සඳහා (පූර්ණ_සංඛ්‍යා i = 1; i <= expected; i++) {
        නම් (s[i] < 0x80 || s[i] >= 0xC0) {
            ආපසු 0;
        }
    }
    ආපසු length;
}

පූර්ණ_සංඛ්‍යා
_PyTokenizer_ensure_utf8(const var_na *line, sa_nracanaa tok_state *tok, පූර්ණ_සංඛ්‍යා lineno)
{
    const var_na *badchar = NULL;
    const var_na *c;
    පූර්ණ_සංඛ්‍යා length;
    const var_na *line_start = line;
    සඳහා (c = line; *c; c += length) {
        නම් (!(length = valid_utf8((const unsigned var_na *)c))) {
            badchar = c;
            bha_nga;
        }
        නම් (*c == '\n') {
            lineno++;
            line_start = c + 1;
        }
    }
    නම් (badchar) {
        _PyTokenizer_syntaxerror_at(
                tok, line_start, badchar - line_start + 1, lineno,
                -1, -1,
                "Non-UTF-8 code starting with '\\x%.2x'"
                "%s%V on line %i, "
                "but no encoding declared; "
                "see https://peps.python.org/pep-0263/ for details",
                (unsigned var_na)*badchar,
                tok->filename ? " in file " : "", tok->filename, "",
                lineno);
        ආපසු 0;
    }
    ආපසු 1;
}


/* ############## DEBUGGING STUFF ############## */

#ifdef Py_DEBUG
void
_PyTokenizer_print_escape(FILE *f, const var_na *s, Py_ssize_t size)
{
    නම් (s == NULL) {
        fputs("NULL", f);
        ආපසු;
    }
    putc('"', f);
    අතරතුර (size-- > 0) {
        unsigned var_na c = *s++;
        switch (c) {
            pak_sa '\n': fputs("\\n", f); bha_nga;
            pak_sa '\r': fputs("\\r", f); bha_nga;
            pak_sa '\t': fputs("\\t", f); bha_nga;
            pak_sa '\f': fputs("\\f", f); bha_nga;
            pak_sa '\'': fputs("\\'", f); bha_nga;
            pak_sa '"': fputs("\\\"", f); bha_nga;
            default:
                නම් (0x20 <= c && c <= 0x7f)
                    putc(c, f);
                නැතහොත්
                    fprintf(f, "\\x%02x", c);
        }
    }
    putc('"', f);
}
#endif
