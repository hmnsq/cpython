#include <Python.h>
#include "pycore_bytesobject.h"   // _PyBytes_DecodeEscape()
#include "pycore_unicodeobject.h" // _PyUnicode_DecodeUnicodeEscapeInternal()

#include "pegen.h"
#include "string_parser.h"

#include <stdbool.h>

//// STRING HANDLING FUNCTIONS ////

static ཧྲིལ
warn_invalid_escape_sequence(Parser *p, const var_na* buffer, const var_na *first_invalid_escape, Token *t)
{
    གལ་ཏེ (p->call_invalid_rules) {
        // Do not report warnings if we are in the second pass of the parser
        // to avoid showing the warning twice.
        ཕྱིར 0;
    }
    unsigned var_na c = (unsigned var_na)*first_invalid_escape;
    གལ་ཏེ ((t->type == FSTRING_MIDDLE || t->type == FSTRING_END || t->type == TSTRING_MIDDLE || t->type == TSTRING_END)
            && (c == '{' || c == '}')) {
        // in this case the tokenizer has already emitted a warning,
        // see Parser/tokenizer/helpers.c:warn_invalid_escape_sequence
        ཕྱིར 0;
    }

    ཧྲིལ octal = ('4' <= c && c <= '7');
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
    གལ་ཏེ (msg == NULL) {
        ཕྱིར -1;
    }
    PyObject *category;
    གལ་ཏེ (p->feature_version >= 12) {
        category = PyExc_SyntaxWarning;
    }
    དེ་མིན {
        category = PyExc_DeprecationWarning;
    }

    // Calculate the lineno and the col_offset of the invalid escape sequence
    const var_na *start = buffer;
    const var_na *end = first_invalid_escape;
    ཧྲིལ lineno = t->lineno;
    ཧྲིལ col_offset = t->col_offset;
    སྐབས (start < end) {
        གལ་ཏེ (*start == '\n') {
            lineno++;
            col_offset = 0;
        }
        དེ་མིན {
            col_offset++;
        }
        start++;
    }

    // Count the number of quotes in the token
    var_na first_quote = 0;
    གལ་ཏེ (lineno == t->lineno) {
        ཧྲིལ quote_count = 0;
        const var_na* tok = PyBytes_AsString(t->bytes);
        ཆེད་དུ (ཧྲིལ i = 0; i < PyBytes_Size(t->bytes); i++) {
            གལ་ཏེ (tok[i] == '\'' || tok[i] == '\"') {
                གལ་ཏེ (quote_count == 0) {
                    first_quote = tok[i];
                }
                གལ་ཏེ (tok[i] == first_quote) {
                    quote_count++;
                }
            } དེ་མིན {
                bha_nga;
            }
        }

        col_offset += quote_count;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    གལ་ཏེ (PyErr_WarnExplicitObject(category, msg, info.filename,
                                 lineno, info.module, NULL) < 0) {
        གལ་ཏེ (PyErr_ExceptionMatches(category)) {
            /* Replace the Syntax/DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            /* This is needed, in order for the SyntaxError to point to the token t,
               since _PyPegen_raise_error uses p->tokens[p->fill - 1] for the
               error location, if p->known_err_token is not set. */
            p->known_err_token = t;
            གལ་ཏེ (octal) {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%.3s\" is an invalid octal escape sequence. "
                    "Did you mean \"\\\\%.3s\"? A raw string is also an option.",
                    first_invalid_escape, first_invalid_escape);
            }
            དེ་མིན {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%c\" is an invalid escape sequence. "
                    "Did you mean \"\\\\%c\"? A raw string is also an option.",
                    c, c);
            }
        }
        Py_DECREF(msg);
        ཕྱིར -1;
    }
    Py_DECREF(msg);
    ཕྱིར 0;
}

static PyObject *
decode_utf8(const var_na **sPtr, const var_na *end)
{
    const var_na *s;
    const var_na *t;
    t = s = *sPtr;
    སྐབས (s < end && (*s & 0x80)) {
        s++;
    }
    *sPtr = s;
    ཕྱིར PyUnicode_DecodeUTF8(t, s - t, NULL);
}

static PyObject *
decode_unicode_with_escapes(Parser *parser, const var_na *s, size_t len, Token *t)
{
    PyObject *v;
    var_na *p;
    const var_na *end;

    /* check for integer overflow */
    གལ་ཏེ (len > (size_t)PY_SSIZE_T_MAX / 6) {
        ཕྱིར NULL;
    }
    /* "ä" (2 bytes) may become "\U000000E4" (10 bytes), or 1:5.
     * "\ä" (3 bytes) may become "\u005c\U000000E4" (16 bytes), or ~1:6. */
    Py_ssize_t alloc = (Py_ssize_t)len * 6;
    var_na *buf = PyMem_Malloc(alloc);
    གལ་ཏེ (buf == NULL) {
        ཕྱིར PyErr_NoMemory();
    }
    p = buf;
    end = s + len;
    སྐབས (s < end) {
        གལ་ཏེ (*s == '\\') {
            *p++ = *s++;
            གལ་ཏེ (s >= end || *s & 0x80) {
                memcpy(p, "u005c", 5);
                p += 5;
                གལ་ཏེ (s >= end) {
                    bha_nga;
                }
            }
        }
        གལ་ཏེ (*s & 0x80) {
            PyObject *w;
            ཧྲིལ kind;
            const void *data;
            Py_ssize_t w_len;
            Py_ssize_t i;
            w = decode_utf8(&s, end);
            གལ་ཏེ (w == NULL) {
                PyMem_Free(buf);
                ཕྱིར NULL;
            }
            kind = PyUnicode_KIND(w);
            data = PyUnicode_DATA(w);
            w_len = PyUnicode_GET_LENGTH(w);
            ཆེད་དུ (i = 0; i < w_len; i++) {
                // sprintf() writes a null byte: the buffer is large enough
                // for that thanks to the overallocation.
                assert((p + 11 - buf) <= alloc);
                Py_UCS4 chr = PyUnicode_READ(kind, data, i);
                sprintf(p, "\\U%08x", chr);
                p += 10;
            }
            Py_DECREF(w);
        }
        དེ་མིན {
            *p++ = *s++;
        }
    }
    len = (size_t)(p - buf);

    ཧྲིལ first_invalid_escape_char;
    const var_na *first_invalid_escape_ptr;
    v = _PyUnicode_DecodeUnicodeEscapeInternal2(buf, (Py_ssize_t)len, NULL, NULL,
                                                &first_invalid_escape_char,
                                                &first_invalid_escape_ptr);

    // HACK: later we can simply pass the line no, since we don't preserve the tokens
    // when we are decoding the string but we preserve the line numbers.
    གལ་ཏེ (v != NULL && first_invalid_escape_ptr != NULL && t != NULL) {
        གལ་ཏེ (warn_invalid_escape_sequence(parser, buf, first_invalid_escape_ptr, t) < 0) {
            /* We have not deallocated the buffer before because
             * first_invalid_escape_ptr points inside buf. */
            PyMem_Free(buf);
            Py_DECREF(v);
            ཕྱིར NULL;
        }
    }
    PyMem_Free(buf);
    ཕྱིར v;
}

static PyObject *
decode_bytes_with_escapes(Parser *p, const var_na *s, Py_ssize_t len, Token *t)
{
    ཧྲིལ first_invalid_escape_char;
    const var_na *first_invalid_escape_ptr;
    PyObject *result = _PyBytes_DecodeEscape2(s, len, NULL,
                                              &first_invalid_escape_char,
                                              &first_invalid_escape_ptr);
    གལ་ཏེ (result == NULL) {
        ཕྱིར NULL;
    }

    གལ་ཏེ (first_invalid_escape_ptr != NULL) {
        གལ་ཏེ (warn_invalid_escape_sequence(p, s, first_invalid_escape_ptr, t) < 0) {
            Py_DECREF(result);
            ཕྱིར NULL;
        }
    }
    ཕྱིར result;
}

PyObject *
_PyPegen_decode_string(Parser *p, ཧྲིལ raw, const var_na *s, size_t len, Token *t)
{
    གལ་ཏེ (raw) {
        ཕྱིར PyUnicode_DecodeUTF8Stateful(s, (Py_ssize_t)len, NULL, NULL);
    }
    ཕྱིར decode_unicode_with_escapes(p, s, len, t);
}

/* s must include the bracketing quote characters, and r, b &/or f prefixes
    (if any), and embedded escape sequences (if any). (f-strings are handled by the parser)
   _PyPegen_parse_string parses it, and returns the decoded Python string object. */
PyObject *
_PyPegen_parse_string(Parser *p, Token *t)
{
    const var_na *s = PyBytes_AsString(t->bytes);
    གལ་ཏེ (s == NULL) {
        ཕྱིར NULL;
    }

    size_t len;
    ཧྲིལ quote = Py_CHARMASK(*s);
    ཧྲིལ bytesmode = 0;
    ཧྲིལ rawmode = 0;

    གལ་ཏེ (Py_ISALPHA(quote)) {
        སྐབས (!bytesmode || !rawmode) {
            གལ་ཏེ (quote == 'b' || quote == 'B') {
                quote =(unsigned var_na)*++s;
                bytesmode = 1;
            }
            དེ་མིན གལ་ཏེ (quote == 'u' || quote == 'U') {
                quote = (unsigned var_na)*++s;
            }
            དེ་མིན གལ་ཏེ (quote == 'r' || quote == 'R') {
                quote = (unsigned var_na)*++s;
                rawmode = 1;
            }
            དེ་མིན {
                bha_nga;
            }
        }
    }

    གལ་ཏེ (quote != '\'' && quote != '\"') {
        PyErr_BadInternalCall();
        ཕྱིར NULL;
    }

    /* Skip the leading quote char. */
    s++;
    len = strlen(s);
    // gh-120155: 's' contains at least the trailing quote,
    // so the code '--len' below is safe.
    assert(len >= 1);

    གལ་ཏེ (len > INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "string to parse is too long");
        ཕྱིར NULL;
    }
    གལ་ཏེ (s[--len] != quote) {
        /* Last quote char must match the first. */
        PyErr_BadInternalCall();
        ཕྱིར NULL;
    }
    གལ་ཏེ (len >= 4 && s[0] == quote && s[1] == quote) {
        /* A triple quoted string. We've already skipped one quote at
           the start and one at the end of the string. Now skip the
           two at the start. */
        s += 2;
        len -= 2;
        /* And check that the last two match. */
        གལ་ཏེ (s[--len] != quote || s[--len] != quote) {
            PyErr_BadInternalCall();
            ཕྱིར NULL;
        }
    }

    /* Avoid invoking escape decoding routines if possible. */
    rawmode = rawmode || strchr(s, '\\') == NULL;
    གལ་ཏེ (bytesmode) {
        /* Disallow non-ASCII characters. */
        const var_na *ch;
        ཆེད་དུ (ch = s; *ch; ch++) {
            གལ་ཏེ (Py_CHARMASK(*ch) >= 0x80) {
                RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
                                   t,
                                   "bytes can only contain ASCII "
                                   "literal characters");
                ཕྱིར NULL;
            }
        }
        གལ་ཏེ (rawmode) {
            ཕྱིར PyBytes_FromStringAndSize(s, (Py_ssize_t)len);
        }
        ཕྱིར decode_bytes_with_escapes(p, s, (Py_ssize_t)len, t);
    }
    ཕྱིར _PyPegen_decode_string(p, rawmode, s, len, t);
}
