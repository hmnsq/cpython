#include <Python.h>
#include "pycore_bytesobject.h"   // _PyBytes_DecodeEscape()
#include "pycore_unicodeobject.h" // _PyUnicode_DecodeUnicodeEscapeInternal()

#include "pegen.h"
#include "string_parser.h"

#include <stdbool.h>

//// STRING HANDLING FUNCTIONS ////

static цяло
warn_invalid_escape_sequence(Parser *p, const char* buffer, const char *first_invalid_escape, Token *t)
{
    ако (p->call_invalid_rules) {
        // Do not report warnings if we are in the second pass of the parser
        // to avoid showing the warning twice.
        върни 0;
    }
    unsigned char c = (unsigned char)*first_invalid_escape;
    ако ((t->type == FSTRING_MIDDLE || t->type == FSTRING_END || t->type == TSTRING_MIDDLE || t->type == TSTRING_END)
            && (c == '{' || c == '}')) {
        // in this case the tokenizer has already emitted a warning,
        // see Parser/tokenizer/helpers.c:warn_invalid_escape_sequence
        върни 0;
    }

    цяло octal = ('4' <= c && c <= '7');
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
    ако (msg == NULL) {
        върни -1;
    }
    PyObject *category;
    ако (p->feature_version >= 12) {
        category = PyExc_SyntaxWarning;
    }
    иначе {
        category = PyExc_DeprecationWarning;
    }

    // Calculate the lineno and the col_offset of the invalid escape sequence
    const char *start = buffer;
    const char *end = first_invalid_escape;
    цяло lineno = t->lineno;
    цяло col_offset = t->col_offset;
    докато (start < end) {
        ако (*start == '\n') {
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
    ако (lineno == t->lineno) {
        цяло quote_count = 0;
        const char* tok = PyBytes_AsString(t->bytes);
        за (цяло i = 0; i < PyBytes_Size(t->bytes); i++) {
            ако (tok[i] == '\'' || tok[i] == '\"') {
                ако (quote_count == 0) {
                    first_quote = tok[i];
                }
                ако (tok[i] == first_quote) {
                    quote_count++;
                }
            } иначе {
                break;
            }
        }

        col_offset += quote_count;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    ако (PyErr_WarnExplicitObject(category, msg, info.filename,
                                 lineno, info.module, NULL) < 0) {
        ако (PyErr_ExceptionMatches(category)) {
            /* Replace the Syntax/DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            /* This is needed, in order for the SyntaxError to point to the token t,
               since _PyPegen_raise_error uses p->tokens[p->fill - 1] for the
               error location, if p->known_err_token is not set. */
            p->known_err_token = t;
            ако (octal) {
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
        върни -1;
    }
    Py_DECREF(msg);
    върни 0;
}

static PyObject *
decode_utf8(const char **sPtr, const char *end)
{
    const char *s;
    const char *t;
    t = s = *sPtr;
    докато (s < end && (*s & 0x80)) {
        s++;
    }
    *sPtr = s;
    върни PyUnicode_DecodeUTF8(t, s - t, NULL);
}

static PyObject *
decode_unicode_with_escapes(Parser *parser, const char *s, size_t len, Token *t)
{
    PyObject *v;
    char *p;
    const char *end;

    /* check for integer overflow */
    ако (len > (size_t)PY_SSIZE_T_MAX / 6) {
        върни NULL;
    }
    /* "ä" (2 bytes) may become "\U000000E4" (10 bytes), or 1:5.
     * "\ä" (3 bytes) may become "\u005c\U000000E4" (16 bytes), or ~1:6. */
    Py_ssize_t alloc = (Py_ssize_t)len * 6;
    char *buf = PyMem_Malloc(alloc);
    ако (buf == NULL) {
        върни PyErr_NoMemory();
    }
    p = buf;
    end = s + len;
    докато (s < end) {
        ако (*s == '\\') {
            *p++ = *s++;
            ако (s >= end || *s & 0x80) {
                memcpy(p, "u005c", 5);
                p += 5;
                ако (s >= end) {
                    break;
                }
            }
        }
        ако (*s & 0x80) {
            PyObject *w;
            цяло kind;
            const void *data;
            Py_ssize_t w_len;
            Py_ssize_t i;
            w = decode_utf8(&s, end);
            ако (w == NULL) {
                PyMem_Free(buf);
                върни NULL;
            }
            kind = PyUnicode_KIND(w);
            data = PyUnicode_DATA(w);
            w_len = PyUnicode_GET_LENGTH(w);
            за (i = 0; i < w_len; i++) {
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

    цяло first_invalid_escape_char;
    const char *first_invalid_escape_ptr;
    v = _PyUnicode_DecodeUnicodeEscapeInternal2(buf, (Py_ssize_t)len, NULL, NULL,
                                                &first_invalid_escape_char,
                                                &first_invalid_escape_ptr);

    // HACK: later we can simply pass the line no, since we don't preserve the tokens
    // when we are decoding the string but we preserve the line numbers.
    ако (v != NULL && first_invalid_escape_ptr != NULL && t != NULL) {
        ако (warn_invalid_escape_sequence(parser, buf, first_invalid_escape_ptr, t) < 0) {
            /* We have not deallocated the buffer before because
             * first_invalid_escape_ptr points inside buf. */
            PyMem_Free(buf);
            Py_DECREF(v);
            върни NULL;
        }
    }
    PyMem_Free(buf);
    върни v;
}

static PyObject *
decode_bytes_with_escapes(Parser *p, const char *s, Py_ssize_t len, Token *t)
{
    цяло first_invalid_escape_char;
    const char *first_invalid_escape_ptr;
    PyObject *result = _PyBytes_DecodeEscape2(s, len, NULL,
                                              &first_invalid_escape_char,
                                              &first_invalid_escape_ptr);
    ако (result == NULL) {
        върни NULL;
    }

    ако (first_invalid_escape_ptr != NULL) {
        ако (warn_invalid_escape_sequence(p, s, first_invalid_escape_ptr, t) < 0) {
            Py_DECREF(result);
            върни NULL;
        }
    }
    върни result;
}

PyObject *
_PyPegen_decode_string(Parser *p, цяло raw, const char *s, size_t len, Token *t)
{
    ако (raw) {
        върни PyUnicode_DecodeUTF8Stateful(s, (Py_ssize_t)len, NULL, NULL);
    }
    върни decode_unicode_with_escapes(p, s, len, t);
}

/* s must include the bracketing quote characters, and r, b &/or f prefixes
    (if any), and embedded escape sequences (if any). (f-strings are handled by the parser)
   _PyPegen_parse_string parses it, and returns the decoded Python string object. */
PyObject *
_PyPegen_parse_string(Parser *p, Token *t)
{
    const char *s = PyBytes_AsString(t->bytes);
    ако (s == NULL) {
        върни NULL;
    }

    size_t len;
    цяло quote = Py_CHARMASK(*s);
    цяло bytesmode = 0;
    цяло rawmode = 0;

    ако (Py_ISALPHA(quote)) {
        докато (!bytesmode || !rawmode) {
            ако (quote == 'b' || quote == 'B') {
                quote =(unsigned char)*++s;
                bytesmode = 1;
            }
            иначе ако (quote == 'u' || quote == 'U') {
                quote = (unsigned char)*++s;
            }
            иначе ако (quote == 'r' || quote == 'R') {
                quote = (unsigned char)*++s;
                rawmode = 1;
            }
            иначе {
                break;
            }
        }
    }

    ако (quote != '\'' && quote != '\"') {
        PyErr_BadInternalCall();
        върни NULL;
    }

    /* Skip the leading quote char. */
    s++;
    len = strlen(s);
    // gh-120155: 's' contains at least the trailing quote,
    // so the code '--len' below is safe.
    assert(len >= 1);

    ако (len > INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "string to parse is too long");
        върни NULL;
    }
    ако (s[--len] != quote) {
        /* Last quote char must match the first. */
        PyErr_BadInternalCall();
        върни NULL;
    }
    ако (len >= 4 && s[0] == quote && s[1] == quote) {
        /* A triple quoted string. We've already skipped one quote at
           the start and one at the end of the string. Now skip the
           two at the start. */
        s += 2;
        len -= 2;
        /* And check that the last two match. */
        ако (s[--len] != quote || s[--len] != quote) {
            PyErr_BadInternalCall();
            върни NULL;
        }
    }

    /* Avoid invoking escape decoding routines if possible. */
    rawmode = rawmode || strchr(s, '\\') == NULL;
    ако (bytesmode) {
        /* Disallow non-ASCII characters. */
        const char *ch;
        за (ch = s; *ch; ch++) {
            ако (Py_CHARMASK(*ch) >= 0x80) {
                RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
                                   t,
                                   "bytes can only contain ASCII "
                                   "literal characters");
                върни NULL;
            }
        }
        ако (rawmode) {
            върни PyBytes_FromStringAndSize(s, (Py_ssize_t)len);
        }
        върни decode_bytes_with_escapes(p, s, (Py_ssize_t)len, t);
    }
    върни _PyPegen_decode_string(p, rawmode, s, len, t);
}
