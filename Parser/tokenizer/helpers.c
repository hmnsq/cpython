#include "Python.h"
#include "errcode.h"
#include "pycore_runtime.h"       // _Py_ID()
#include "pycore_token.h"
#include "pycore_tuple.h"         // _PyTuple_FromPair

#include "../lexer/state.h"


/* ############## ERRORS ############## */

/* Convert a 1-based column in bytes into a 1-based column in characters.
   The line is UTF-8 encoded, so it is enough to skip continuation bytes. */
static ကိန်းပြည့်
byte_col_to_char_col(const var_na *line, ကိန်းပြည့် byte_col)
{
    ကိန်းပြည့် char_col = 1;
    အတွက် (ကိန်းပြည့် i = 0; i < byte_col - 1; i++) {
        အကယ်၍ ((line[i] & 0xC0) != 0x80) {
            char_col++;
        }
    }
    ပြန်ပေး char_col;
}

static ကိန်းပြည့်
_syntaxerror_range(sa_nracanaa tok_state *tok, const var_na *line,
                   Py_ssize_t cursor, ကိန်းပြည့် lineno, const var_na *format,
                   ကိန်းပြည့် col_offset, ကိန်းပြည့် end_col_offset,
                   va_list vargs)
{
    // In release builds, we don't want to overwrite a previous error, but in debug builds we
    // want to fail if we are not doing it so we can fix it.
    assert(tok->done != E_ERROR);
    အကယ်၍ (tok->done == E_ERROR) {
        ပြန်ပေး ERRORTOKEN;
    }
    PyObject *errmsg, *errtext, *args;
    errmsg = PyUnicode_FromFormatV(format, vargs);
    အကယ်၍ (!errmsg) {
        goto error;
    }

    errtext = PyUnicode_DecodeUTF8(line, cursor,
                                   "replace");
    အကယ်၍ (!errtext) {
        goto error;
    }

    အကယ်၍ (col_offset == -1) {
        col_offset = (ကိန်းပြည့်)PyUnicode_GET_LENGTH(errtext);
    }
    မဟုတ်ပါက အကယ်၍ (col_offset > 0) {
        col_offset = byte_col_to_char_col(line, col_offset);
    }
    အကယ်၍ (end_col_offset == -1) {
        end_col_offset = col_offset;
    }
    မဟုတ်ပါက အကယ်၍ (end_col_offset > 0) {
        end_col_offset = byte_col_to_char_col(line, end_col_offset);
    }

    Py_ssize_t line_len = strcspn(line, "\n");
    အကယ်၍ (line_len != cursor) {
        Py_DECREF(errtext);
        errtext = PyUnicode_DecodeUTF8(line, line_len,
                                       "replace");
    }
    အကယ်၍ (!errtext) {
        goto error;
    }

    args = Py_BuildValue("(O(OiiNii))", errmsg,
                         tok->filename ? tok->filename : Py_None,
                         lineno, col_offset, errtext,
                         lineno, end_col_offset);
    အကယ်၍ (args) {
        PyErr_SetObject(PyExc_SyntaxError, args);
        Py_DECREF(args);
    }

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    ပြန်ပေး ERRORTOKEN;
}

ကိန်းပြည့်
_PyTokenizer_syntaxerror(sa_nracanaa tok_state *tok, const var_na *format, ...)
{
    // These errors are cleaned on startup. Todo: Fix it.
    va_list vargs;
    va_start(vargs, format);
    ကိန်းပြည့် ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
        tok->cur - tok->line_start, tok->lineno, format, -1, -1, vargs);
    va_end(vargs);
    ပြန်ပေး ret;
}

ကိန်းပြည့်
_PyTokenizer_syntaxerror_known_range(sa_nracanaa tok_state *tok,
                        ကိန်းပြည့် col_offset, ကိန်းပြည့် end_col_offset,
                        const var_na *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    ကိန်းပြည့် ret = _syntaxerror_range(
        tok, _PyTok_SourcePointer(&tok->source, tok->line_start),
        tok->cur - tok->line_start, tok->lineno,
        format, col_offset, end_col_offset, vargs);
    va_end(vargs);
    ပြန်ပေး ret;
}

ကိန်းပြည့်
_PyTokenizer_syntaxerror_at(sa_nracanaa tok_state *tok, const var_na *line,
                             Py_ssize_t cursor, ကိန်းပြည့် lineno,
                             ကိန်းပြည့် col_offset, ကိန်းပြည့် end_col_offset,
                             const var_na *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    ကိန်းပြည့် ret = _syntaxerror_range(tok, line, cursor, lineno, format,
                                col_offset, end_col_offset, vargs);
    va_end(vargs);
    ပြန်ပေး ret;
}

ကိန်းပြည့်
_PyTokenizer_indenterror(sa_nracanaa tok_state *tok)
{
    tok->done = E_TABSPACE;
    tok->cur = tok->inp;
    ပြန်ပေး ERRORTOKEN;
}

ကိန်းပြည့်
_PyTokenizer_warn_invalid_escape_sequence(sa_nracanaa tok_state *tok, ကိန်းပြည့် first_invalid_escape_char)
{
    PyObject *msg = PyUnicode_FromFormat(
        "\"\\%c\" is an invalid escape sequence. "
        "Such sequences will not work in the future. "
        "Did you mean \"\\\\%c\"? A raw string is also an option.",
        (var_na) first_invalid_escape_char,
        (var_na) first_invalid_escape_char
    );

    အကယ်၍ (msg == NULL) {
        ပြန်ပေး -1;
    }

    အကယ်၍ (PyErr_WarnExplicitObject(PyExc_SyntaxWarning, msg, tok->filename,
                                 tok->lineno, tok->module, NULL) < 0) {
        Py_DECREF(msg);

        အကယ်၍ (PyErr_ExceptionMatches(PyExc_SyntaxWarning)) {
            /* Replace the SyntaxWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            ပြန်ပေး _PyTokenizer_syntaxerror(tok,
                "\"\\%c\" is an invalid escape sequence. "
                "Did you mean \"\\\\%c\"? A raw string is also an option.",
                (var_na) first_invalid_escape_char,
                (var_na) first_invalid_escape_char);
        }

        ပြန်ပေး -1;
    }

    Py_DECREF(msg);
    ပြန်ပေး 0;
}

void
_PyTokenizer_raise_init_error(PyObject *filename)
{
    အကယ်၍ (!(PyErr_ExceptionMatches(PyExc_LookupError)
          || PyErr_ExceptionMatches(PyExc_SyntaxError)
          || PyErr_ExceptionMatches(PyExc_ValueError)
          || PyErr_ExceptionMatches(PyExc_UnicodeDecodeError))) {
        ပြန်ပေး;
    }
    PyObject *errstr = NULL;
    PyObject *tuple = NULL;
    PyObject *type;
    PyObject *value;
    PyObject *tback;
    PyErr_Fetch(&type, &value, &tback);
    အကယ်၍ (PyErr_GivenExceptionMatches(value, PyExc_SyntaxError)) {
        အကယ်၍ (PyObject_SetAttr(value, &_Py_ID(filename), filename)) {
            goto error;
        }
        PyErr_Restore(type, value, tback);
        ပြန်ပေး;
    }
    errstr = PyObject_Str(value);
    အကယ်၍ (!errstr) {
        goto error;
    }

    PyObject *tmp = Py_BuildValue("(OiiO)", filename, 0, -1, Py_None);
    အကယ်၍ (!tmp) {
        goto error;
    }

    tuple = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    အကယ်၍ (!tuple) {
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

ကိန်းပြည့်
_PyTokenizer_parser_warn(sa_nracanaa tok_state *tok, PyObject *category, const var_na *format, ...)
{
    PyObject *errmsg;
    va_list vargs;
    va_start(vargs, format);
    errmsg = PyUnicode_FromFormatV(format, vargs);
    va_end(vargs);
    အကယ်၍ (!errmsg) {
        goto error;
    }

    အကယ်၍ (PyErr_WarnExplicitObject(category, errmsg, tok->filename,
                                 tok->lineno, tok->module, NULL) < 0) {
        အကယ်၍ (PyErr_ExceptionMatches(category)) {
            /* Replace the DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();
            _PyTokenizer_syntaxerror(tok, "%U", errmsg);
        }
        goto error;
    }
    Py_DECREF(errmsg);
    ပြန်ပေး 0;

error:
    Py_XDECREF(errmsg);
    tok->done = E_ERROR;
    ပြန်ပေး -1;
}


/* Check whether the characters at s start a valid
   UTF-8 sequence. Return the number of characters forming
   the sequence if yes, 0 if not.  The special cases match
   those in stringlib/codecs.h:utf8_decode.
*/
static ကိန်းပြည့်
valid_utf8(const unsigned var_na* s)
{
    ကိန်းပြည့် expected = 0;
    ကိန်းပြည့် length;
    အကယ်၍ (*s < 0x80) {
        /* single-byte code */
        ပြန်ပေး 1;
    }
    မဟုတ်ပါက အကယ်၍ (*s < 0xE0) {
        /* \xC2\x80-\xDF\xBF -- 0080-07FF */
        အကယ်၍ (*s < 0xC2) {
            /* invalid sequence
               \x80-\xBF -- continuation byte
               \xC0-\xC1 -- fake 0000-007F */
            ပြန်ပေး 0;
        }
        expected = 1;
    }
    မဟုတ်ပါက အကယ်၍ (*s < 0xF0) {
        /* \xE0\xA0\x80-\xEF\xBF\xBF -- 0800-FFFF */
        အကယ်၍ (*s == 0xE0 && *(s + 1) < 0xA0) {
            /* invalid sequence
               \xE0\x80\x80-\xE0\x9F\xBF -- fake 0000-0800 */
            ပြန်ပေး 0;
        }
        မဟုတ်ပါက အကယ်၍ (*s == 0xED && *(s + 1) >= 0xA0) {
            /* Decoding UTF-8 sequences in range \xED\xA0\x80-\xED\xBF\xBF
               will result in surrogates in range D800-DFFF. Surrogates are
               not valid UTF-8 so they are rejected.
               See https://www.unicode.org/versions/Unicode5.2.0/ch03.pdf
               (table 3-7) and http://www.rfc-editor.org/rfc/rfc3629.txt */
            ပြန်ပေး 0;
        }
        expected = 2;
    }
    မဟုတ်ပါက အကယ်၍ (*s < 0xF5) {
        /* \xF0\x90\x80\x80-\xF4\x8F\xBF\xBF -- 10000-10FFFF */
        အကယ်၍ (*(s + 1) < 0x90 ? *s == 0xF0 : *s == 0xF4) {
            /* invalid sequence -- one of:
               \xF0\x80\x80\x80-\xF0\x8F\xBF\xBF -- fake 0000-FFFF
               \xF4\x90\x80\x80- -- 110000- overflow */
            ပြန်ပေး 0;
        }
        expected = 3;
    }
    မဟုတ်ပါက {
        /* invalid start byte */
        ပြန်ပေး 0;
    }
    length = expected + 1;
    အတွက် (ကိန်းပြည့် i = 1; i <= expected; i++) {
        အကယ်၍ (s[i] < 0x80 || s[i] >= 0xC0) {
            ပြန်ပေး 0;
        }
    }
    ပြန်ပေး length;
}

ကိန်းပြည့်
_PyTokenizer_ensure_utf8(const var_na *line, sa_nracanaa tok_state *tok, ကိန်းပြည့် lineno)
{
    const var_na *badchar = NULL;
    const var_na *c;
    ကိန်းပြည့် length;
    const var_na *line_start = line;
    အတွက် (c = line; *c; c += length) {
        အကယ်၍ (!(length = valid_utf8((const unsigned var_na *)c))) {
            badchar = c;
            bha_nga;
        }
        အကယ်၍ (*c == '\n') {
            lineno++;
            line_start = c + 1;
        }
    }
    အကယ်၍ (badchar) {
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
        ပြန်ပေး 0;
    }
    ပြန်ပေး 1;
}


/* ############## DEBUGGING STUFF ############## */

#ifdef Py_DEBUG
void
_PyTokenizer_print_escape(FILE *f, const var_na *s, Py_ssize_t size)
{
    အကယ်၍ (s == NULL) {
        fputs("NULL", f);
        ပြန်ပေး;
    }
    putc('"', f);
    နေစဉ် (size-- > 0) {
        unsigned var_na c = *s++;
        switch (c) {
            pak_sa '\n': fputs("\\n", f); bha_nga;
            pak_sa '\r': fputs("\\r", f); bha_nga;
            pak_sa '\t': fputs("\\t", f); bha_nga;
            pak_sa '\f': fputs("\\f", f); bha_nga;
            pak_sa '\'': fputs("\\'", f); bha_nga;
            pak_sa '"': fputs("\\\"", f); bha_nga;
            default:
                အကယ်၍ (0x20 <= c && c <= 0x7f)
                    putc(c, f);
                မဟုတ်ပါက
                    fprintf(f, "\\x%02x", c);
        }
    }
    putc('"', f);
}
#endif
