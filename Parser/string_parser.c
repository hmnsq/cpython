#include <Python.h>
#include "pycore_bytesobject.h"   // _PyBytes_DecodeEscape()
#include "pycore_unicodeobject.h" // _PyUnicode_DecodeUnicodeEscapeInternal()

#include "pegen.h"
#include "string_parser.h"

#include <stdbool.h>

//// STRING HANDLING FUNCTIONS ////

static 정수
warn_invalid_escape_sequence(Parser *p, const char* buffer, const char *first_invalid_escape, Token *t)
{
    만약 (p->call_invalid_rules) {
        // Do not report warnings if we are in the second pass of the parser
        // to avoid showing the warning twice.
        반환 0;
    }
    unsigned char c = (unsigned char)*first_invalid_escape;
    만약 ((t->type == FSTRING_MIDDLE || t->type == FSTRING_END || t->type == TSTRING_MIDDLE || t->type == TSTRING_END)
            && (c == '{' || c == '}')) {
        // in this case the tokenizer has already emitted a warning,
        // see Parser/tokenizer/helpers.c:warn_invalid_escape_sequence
        반환 0;
    }

    정수 octal = ('4' <= c && c <= '7');
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
    만약 (msg == NULL) {
        반환 -1;
    }
    PyObject *category;
    만약 (p->feature_version >= 12) {
        category = PyExc_SyntaxWarning;
    }
    아니면 {
        category = PyExc_DeprecationWarning;
    }

    // Calculate the lineno and the col_offset of the invalid escape sequence
    const char *start = buffer;
    const char *end = first_invalid_escape;
    정수 lineno = t->lineno;
    정수 col_offset = t->col_offset;
    동안 (start < end) {
        만약 (*start == '\n') {
            lineno++;
            col_offset = 0;
        }
        아니면 {
            col_offset++;
        }
        start++;
    }

    // Count the number of quotes in the token
    char first_quote = 0;
    만약 (lineno == t->lineno) {
        정수 quote_count = 0;
        const char* tok = PyBytes_AsString(t->bytes);
        위해 (정수 i = 0; i < PyBytes_Size(t->bytes); i++) {
            만약 (tok[i] == '\'' || tok[i] == '\"') {
                만약 (quote_count == 0) {
                    first_quote = tok[i];
                }
                만약 (tok[i] == first_quote) {
                    quote_count++;
                }
            } 아니면 {
                break;
            }
        }

        col_offset += quote_count;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    만약 (PyErr_WarnExplicitObject(category, msg, info.filename,
                                 lineno, info.module, NULL) < 0) {
        만약 (PyErr_ExceptionMatches(category)) {
            /* Replace the Syntax/DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            /* This is needed, in order for the SyntaxError to point to the token t,
               since _PyPegen_raise_error uses p->tokens[p->fill - 1] for the
               error location, if p->known_err_token is not set. */
            p->known_err_token = t;
            만약 (octal) {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%.3s\" is an invalid octal escape sequence. "
                    "Did you mean \"\\\\%.3s\"? A raw string is also an option.",
                    first_invalid_escape, first_invalid_escape);
            }
            아니면 {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%c\" is an invalid escape sequence. "
                    "Did you mean \"\\\\%c\"? A raw string is also an option.",
                    c, c);
            }
        }
        Py_DECREF(msg);
        반환 -1;
    }
    Py_DECREF(msg);
    반환 0;
}

static PyObject *
decode_utf8(const char **sPtr, const char *end)
{
    const char *s;
    const char *t;
    t = s = *sPtr;
    동안 (s < end && (*s & 0x80)) {
        s++;
    }
    *sPtr = s;
    반환 PyUnicode_DecodeUTF8(t, s - t, NULL);
}

static PyObject *
decode_unicode_with_escapes(Parser *parser, const char *s, size_t len, Token *t)
{
    PyObject *v;
    char *p;
    const char *end;

    /* check for integer overflow */
    만약 (len > (size_t)PY_SSIZE_T_MAX / 6) {
        반환 NULL;
    }
    /* "ä" (2 bytes) may become "\U000000E4" (10 bytes), or 1:5.
     * "\ä" (3 bytes) may become "\u005c\U000000E4" (16 bytes), or ~1:6. */
    Py_ssize_t alloc = (Py_ssize_t)len * 6;
    char *buf = PyMem_Malloc(alloc);
    만약 (buf == NULL) {
        반환 PyErr_NoMemory();
    }
    p = buf;
    end = s + len;
    동안 (s < end) {
        만약 (*s == '\\') {
            *p++ = *s++;
            만약 (s >= end || *s & 0x80) {
                memcpy(p, "u005c", 5);
                p += 5;
                만약 (s >= end) {
                    break;
                }
            }
        }
        만약 (*s & 0x80) {
            PyObject *w;
            정수 kind;
            const void *data;
            Py_ssize_t w_len;
            Py_ssize_t i;
            w = decode_utf8(&s, end);
            만약 (w == NULL) {
                PyMem_Free(buf);
                반환 NULL;
            }
            kind = PyUnicode_KIND(w);
            data = PyUnicode_DATA(w);
            w_len = PyUnicode_GET_LENGTH(w);
            위해 (i = 0; i < w_len; i++) {
                // sprintf() writes a null byte: the buffer is large enough
                // for that thanks to the overallocation.
                assert((p + 11 - buf) <= alloc);
                Py_UCS4 chr = PyUnicode_READ(kind, data, i);
                sprintf(p, "\\U%08x", chr);
                p += 10;
            }
            Py_DECREF(w);
        }
        아니면 {
            *p++ = *s++;
        }
    }
    len = (size_t)(p - buf);

    정수 first_invalid_escape_char;
    const char *first_invalid_escape_ptr;
    v = _PyUnicode_DecodeUnicodeEscapeInternal2(buf, (Py_ssize_t)len, NULL, NULL,
                                                &first_invalid_escape_char,
                                                &first_invalid_escape_ptr);

    // HACK: later we can simply pass the line no, since we don't preserve the tokens
    // when we are decoding the string but we preserve the line numbers.
    만약 (v != NULL && first_invalid_escape_ptr != NULL && t != NULL) {
        만약 (warn_invalid_escape_sequence(parser, buf, first_invalid_escape_ptr, t) < 0) {
            /* We have not deallocated the buffer before because
             * first_invalid_escape_ptr points inside buf. */
            PyMem_Free(buf);
            Py_DECREF(v);
            반환 NULL;
        }
    }
    PyMem_Free(buf);
    반환 v;
}

static PyObject *
decode_bytes_with_escapes(Parser *p, const char *s, Py_ssize_t len, Token *t)
{
    정수 first_invalid_escape_char;
    const char *first_invalid_escape_ptr;
    PyObject *result = _PyBytes_DecodeEscape2(s, len, NULL,
                                              &first_invalid_escape_char,
                                              &first_invalid_escape_ptr);
    만약 (result == NULL) {
        반환 NULL;
    }

    만약 (first_invalid_escape_ptr != NULL) {
        만약 (warn_invalid_escape_sequence(p, s, first_invalid_escape_ptr, t) < 0) {
            Py_DECREF(result);
            반환 NULL;
        }
    }
    반환 result;
}

PyObject *
_PyPegen_decode_string(Parser *p, 정수 raw, const char *s, size_t len, Token *t)
{
    만약 (raw) {
        반환 PyUnicode_DecodeUTF8Stateful(s, (Py_ssize_t)len, NULL, NULL);
    }
    반환 decode_unicode_with_escapes(p, s, len, t);
}

/* s must include the bracketing quote characters, and r, b &/or f prefixes
    (if any), and embedded escape sequences (if any). (f-strings are handled by the parser)
   _PyPegen_parse_string parses it, and returns the decoded Python string object. */
PyObject *
_PyPegen_parse_string(Parser *p, Token *t)
{
    const char *s = PyBytes_AsString(t->bytes);
    만약 (s == NULL) {
        반환 NULL;
    }

    size_t len;
    정수 quote = Py_CHARMASK(*s);
    정수 bytesmode = 0;
    정수 rawmode = 0;

    만약 (Py_ISALPHA(quote)) {
        동안 (!bytesmode || !rawmode) {
            만약 (quote == 'b' || quote == 'B') {
                quote =(unsigned char)*++s;
                bytesmode = 1;
            }
            아니면 만약 (quote == 'u' || quote == 'U') {
                quote = (unsigned char)*++s;
            }
            아니면 만약 (quote == 'r' || quote == 'R') {
                quote = (unsigned char)*++s;
                rawmode = 1;
            }
            아니면 {
                break;
            }
        }
    }

    만약 (quote != '\'' && quote != '\"') {
        PyErr_BadInternalCall();
        반환 NULL;
    }

    /* Skip the leading quote char. */
    s++;
    len = strlen(s);
    // gh-120155: 's' contains at least the trailing quote,
    // so the code '--len' below is safe.
    assert(len >= 1);

    만약 (len > INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "string to parse is too long");
        반환 NULL;
    }
    만약 (s[--len] != quote) {
        /* Last quote char must match the first. */
        PyErr_BadInternalCall();
        반환 NULL;
    }
    만약 (len >= 4 && s[0] == quote && s[1] == quote) {
        /* A triple quoted string. We've already skipped one quote at
           the start and one at the end of the string. Now skip the
           two at the start. */
        s += 2;
        len -= 2;
        /* And check that the last two match. */
        만약 (s[--len] != quote || s[--len] != quote) {
            PyErr_BadInternalCall();
            반환 NULL;
        }
    }

    /* Avoid invoking escape decoding routines if possible. */
    rawmode = rawmode || strchr(s, '\\') == NULL;
    만약 (bytesmode) {
        /* Disallow non-ASCII characters. */
        const char *ch;
        위해 (ch = s; *ch; ch++) {
            만약 (Py_CHARMASK(*ch) >= 0x80) {
                RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
                                   t,
                                   "bytes can only contain ASCII "
                                   "literal characters");
                반환 NULL;
            }
        }
        만약 (rawmode) {
            반환 PyBytes_FromStringAndSize(s, (Py_ssize_t)len);
        }
        반환 decode_bytes_with_escapes(p, s, (Py_ssize_t)len, t);
    }
    반환 _PyPegen_decode_string(p, rawmode, s, len, t);
}
