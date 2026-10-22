#include <Python.h>
#include "pycore_bytesobject.h"   // _PyBytes_DecodeEscape()
#include "pycore_unicodeobject.h" // _PyUnicode_DecodeUnicodeEscapeInternal()

#include "pegen.h"
#include "string_parser.h"

#include <stdbool.h>

//// STRING HANDLING FUNCTIONS ////

static целое
warn_invalid_escape_sequence(Parser *p, const char* buffer, const char *first_invalid_escape, Token *t)
{
    если (p->call_invalid_rules) {
        // Do not report warnings if we are in the second pass of the parser
        // to avoid showing the warning twice.
        вернуть 0;
    }
    unsigned char c = (unsigned char)*first_invalid_escape;
    если ((t->type == FSTRING_MIDDLE || t->type == FSTRING_END || t->type == TSTRING_MIDDLE || t->type == TSTRING_END)
            && (c == '{' || c == '}')) {
        // in this case the tokenizer has already emitted a warning,
        // see Parser/tokenizer/helpers.c:warn_invalid_escape_sequence
        вернуть 0;
    }

    целое octal = ('4' <= c && c <= '7');
    PyObject *msg =
        octal
        ? PyUnicode_FromFormat(
              "\"\\%.3s\" is an invalid octal escape sequence. "
              "Such sequences will not work in the future. "
              "Did you mean \"\\\\%.3s\"? A raw string is also an option.",
              first_invalid_escape, first_invalid_escape)
        : PyUnicode_FromFormat(
              "\"\\%c\" is an invalid escape sequence. "
              "Such sequences will not work in the future. "
              "Did you mean \"\\\\%c\"? A raw string is also an option.",
              c, c);
    если (msg == NULL) {
        вернуть -1;
    }
    PyObject *category;
    если (p->feature_version >= 12) {
        category = PyExc_SyntaxWarning;
    }
    иначе {
        category = PyExc_DeprecationWarning;
    }

    // Calculate the lineno and the col_offset of the invalid escape sequence
    const char *start = buffer;
    const char *end = first_invalid_escape;
    целое lineno = t->lineno;
    целое col_offset = t->col_offset;
    пока (start < end) {
        если (*start == '\n') {
            lineno++;
            col_offset = 0;
        }
        иначе {
            col_offset++;
        }
        start++;
    }

    // Count the number of quotes in the token
    char first_quote = 0;
    если (lineno == t->lineno) {
        целое quote_count = 0;
        const char* tok = PyBytes_AsString(t->bytes);
        для (целое i = 0; i < PyBytes_Size(t->bytes); i++) {
            если (tok[i] == '\'' || tok[i] == '\"') {
                если (quote_count == 0) {
                    first_quote = tok[i];
                }
                если (tok[i] == first_quote) {
                    quote_count++;
                }
            } иначе {
                break;
            }
        }

        col_offset += quote_count;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    если (PyErr_WarnExplicitObject(category, msg, info.filename,
                                 lineno, info.module, NULL) < 0) {
        если (PyErr_ExceptionMatches(category)) {
            /* Replace the Syntax/DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            /* This is needed, in order for the SyntaxError to point to the token t,
               since _PyPegen_raise_error uses p->tokens[p->fill - 1] for the
               error location, if p->known_err_token is not set. */
            p->known_err_token = t;
            если (octal) {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%.3s\" is an invalid octal escape sequence. "
                    "Did you mean \"\\\\%.3s\"? A raw string is also an option.",
                    first_invalid_escape, first_invalid_escape);
            }
            иначе {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%c\" is an invalid escape sequence. "
                    "Did you mean \"\\\\%c\"? A raw string is also an option.",
                    c, c);
            }
        }
        Py_DECREF(msg);
        вернуть -1;
    }
    Py_DECREF(msg);
    вернуть 0;
}

static PyObject *
decode_utf8(const char **sPtr, const char *end)
{
    const char *s;
    const char *t;
    t = s = *sPtr;
    пока (s < end && (*s & 0x80)) {
        s++;
    }
    *sPtr = s;
    вернуть PyUnicode_DecodeUTF8(t, s - t, NULL);
}

static PyObject *
decode_unicode_with_escapes(Parser *parser, const char *s, size_t len, Token *t)
{
    PyObject *v;
    char *p;
    const char *end;

    /* check for integer overflow */
    если (len > (size_t)PY_SSIZE_T_MAX / 6) {
        вернуть NULL;
    }
    /* "ä" (2 bytes) may become "\U000000E4" (10 bytes), or 1:5.
     * "\ä" (3 bytes) may become "\u005c\U000000E4" (16 bytes), or ~1:6. */
    Py_ssize_t alloc = (Py_ssize_t)len * 6;
    char *buf = PyMem_Malloc(alloc);
    если (buf == NULL) {
        вернуть PyErr_NoMemory();
    }
    p = buf;
    end = s + len;
    пока (s < end) {
        если (*s == '\\') {
            *p++ = *s++;
            если (s >= end || *s & 0x80) {
                memcpy(p, "u005c", 5);
                p += 5;
                если (s >= end) {
                    break;
                }
            }
        }
        если (*s & 0x80) {
            PyObject *w;
            целое kind;
            const void *data;
            Py_ssize_t w_len;
            Py_ssize_t i;
            w = decode_utf8(&s, end);
            если (w == NULL) {
                PyMem_Free(buf);
                вернуть NULL;
            }
            kind = PyUnicode_KIND(w);
            data = PyUnicode_DATA(w);
            w_len = PyUnicode_GET_LENGTH(w);
            для (i = 0; i < w_len; i++) {
                // sprintf() writes a null byte: the buffer is large enough
                // for that thanks to the overallocation.
                assert((p + 11 - buf) <= alloc);
                Py_UCS4 chr = PyUnicode_READ(kind, data, i);
                sprintf(p, "\\U%08x", chr);
                p += 10;
            }
            Py_DECREF(w);
        }
        иначе {
            *p++ = *s++;
        }
    }
    len = (size_t)(p - buf);

    целое first_invalid_escape_char;
    const char *first_invalid_escape_ptr;
    v = _PyUnicode_DecodeUnicodeEscapeInternal2(buf, (Py_ssize_t)len, NULL, NULL,
                                                &first_invalid_escape_char,
                                                &first_invalid_escape_ptr);

    // HACK: later we can simply pass the line no, since we don't preserve the tokens
    // when we are decoding the string but we preserve the line numbers.
    если (v != NULL && first_invalid_escape_ptr != NULL && t != NULL) {
        если (warn_invalid_escape_sequence(parser, buf, first_invalid_escape_ptr, t) < 0) {
            /* We have not deallocated the buffer before because
             * first_invalid_escape_ptr points inside buf. */
            PyMem_Free(buf);
            Py_DECREF(v);
            вернуть NULL;
        }
    }
    PyMem_Free(buf);
    вернуть v;
}

static PyObject *
decode_bytes_with_escapes(Parser *p, const char *s, Py_ssize_t len, Token *t)
{
    целое first_invalid_escape_char;
    const char *first_invalid_escape_ptr;
    PyObject *result = _PyBytes_DecodeEscape2(s, len, NULL,
                                              &first_invalid_escape_char,
                                              &first_invalid_escape_ptr);
    если (result == NULL) {
        вернуть NULL;
    }

    если (first_invalid_escape_ptr != NULL) {
        если (warn_invalid_escape_sequence(p, s, first_invalid_escape_ptr, t) < 0) {
            Py_DECREF(result);
            вернуть NULL;
        }
    }
    вернуть result;
}

PyObject *
_PyPegen_decode_string(Parser *p, целое raw, const char *s, size_t len, Token *t)
{
    если (raw) {
        вернуть PyUnicode_DecodeUTF8Stateful(s, (Py_ssize_t)len, NULL, NULL);
    }
    вернуть decode_unicode_with_escapes(p, s, len, t);
}

/* s must include the bracketing quote characters, and r, b &/or f prefixes
    (if any), and embedded escape sequences (if any). (f-strings are handled by the parser)
   _PyPegen_parse_string parses it, and returns the decoded Python string object. */
PyObject *
_PyPegen_parse_string(Parser *p, Token *t)
{
    const char *s = PyBytes_AsString(t->bytes);
    если (s == NULL) {
        вернуть NULL;
    }

    size_t len;
    целое quote = Py_CHARMASK(*s);
    целое bytesmode = 0;
    целое rawmode = 0;

    если (Py_ISALPHA(quote)) {
        пока (!bytesmode || !rawmode) {
            если (quote == 'b' || quote == 'B') {
                quote =(unsigned char)*++s;
                bytesmode = 1;
            }
            иначе если (quote == 'u' || quote == 'U') {
                quote = (unsigned char)*++s;
            }
            иначе если (quote == 'r' || quote == 'R') {
                quote = (unsigned char)*++s;
                rawmode = 1;
            }
            иначе {
                break;
            }
        }
    }

    если (quote != '\'' && quote != '\"') {
        PyErr_BadInternalCall();
        вернуть NULL;
    }

    /* Skip the leading quote char. */
    s++;
    len = strlen(s);
    // gh-120155: 's' contains at least the trailing quote,
    // so the code '--len' below is safe.
    assert(len >= 1);

    если (len > INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "string to parse is too long");
        вернуть NULL;
    }
    если (s[--len] != quote) {
        /* Last quote char must match the first. */
        PyErr_BadInternalCall();
        вернуть NULL;
    }
    если (len >= 4 && s[0] == quote && s[1] == quote) {
        /* A triple quoted string. We've already skipped one quote at
           the start and one at the end of the string. Now skip the
           two at the start. */
        s += 2;
        len -= 2;
        /* And check that the last two match. */
        если (s[--len] != quote || s[--len] != quote) {
            PyErr_BadInternalCall();
            вернуть NULL;
        }
    }

    /* Avoid invoking escape decoding routines if possible. */
    rawmode = rawmode || strchr(s, '\\') == NULL;
    если (bytesmode) {
        /* Disallow non-ASCII characters. */
        const char *ch;
        для (ch = s; *ch; ch++) {
            если (Py_CHARMASK(*ch) >= 0x80) {
                RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
                                   t,
                                   "bytes can only contain ASCII "
                                   "literal characters");
                вернуть NULL;
            }
        }
        если (rawmode) {
            вернуть PyBytes_FromStringAndSize(s, (Py_ssize_t)len);
        }
        вернуть decode_bytes_with_escapes(p, s, (Py_ssize_t)len, t);
    }
    вернуть _PyPegen_decode_string(p, rawmode, s, len, t);
}
