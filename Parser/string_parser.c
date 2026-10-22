#ضمن <Python.h>
#ضمن "pycore_bytesobject.h"   // _PyBytes_DecodeEscape()
#ضمن "pycore_unicodeobject.h" // _PyUnicode_DecodeUnicodeEscapeInternal()

#ضمن "pegen.h"
#ضمن "string_parser.h"

#ضمن <stdbool.h>

//// STRING HANDLING FUNCTIONS ////

ساكن صحيح
warn_invalid_escape_sequence(Parser *p, ثابت حرف* buffer, ثابت حرف *first_invalid_escape, Token *t)
{
    إذا (p->call_invalid_rules) {
        // Do not report warnings if we are in the second pass of the parser
        // to avoid showing the warning twice.
        أرجع 0;
    }
    unsigned حرف c = (unsigned حرف)*first_invalid_escape;
    إذا ((t->type == FSTRING_ڟكيا || t->type == FSTRING_END || t->type == TSTRING_ڟكيا || t->type == TSTRING_END)
            && (c == '{' || c == '}')) {
        // in this case the tokenizer has already emitted a warning,
        // see Parser/tokenizer/helpers.c:warn_invalid_escape_sequence
        أرجع 0;
    }

    صحيح octal = ('4' <= c && c <= '7');
    PyObject *سكو =
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
    إذا (سكو == عدم) {
        أرجع -1;
    }
    PyObject *category;
    إذا (p->feature_version >= 12) {
        category = PyExc_SyntaxWarning;
    }
    وإلا {
        category = PyExc_DeprecationWarning;
    }

    // Calculate the lineno and the col_offset of the invalid escape sequence
    ثابت حرف *start = buffer;
    ثابت حرف *end = first_invalid_escape;
    صحيح lineno = t->lineno;
    صحيح col_offset = t->col_offset;
    طالما (start < end) {
        إذا (*start == '\n') {
            lineno++;
            col_offset = 0;
        }
        وإلا {
            col_offset++;
        }
        start++;
    }

    // Count the number of quotes in the token
    حرف first_quote = 0;
    إذا (lineno == t->lineno) {
        صحيح quote_كدي = 0;
        ثابت حرف* tok = PyBytes_AsString(t->bytes);
        لكل (صحيح i = 0; i < PyBytes_Size(t->bytes); i++) {
            إذا (tok[i] == '\'' || tok[i] == '\"') {
                إذا (quote_كدي == 0) {
                    first_quote = tok[i];
                }
                إذا (tok[i] == first_quote) {
                    quote_كدي++;
                }
            } وإلا {
                اكسر;
            }
        }

        col_offset += quote_كدي;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    إذا (PyErr_WarnExplicitObject(category, سكو, info.filename,
                                 lineno, info.module, عدم) < 0) {
        إذا (PyErr_ExceptionMatches(category)) {
            /* Replace the Syntax/DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            /* This is needed, in order for the SyntaxError to point to the token t,
               since _PyPegen_raise_error uses p->tokens[p->fill - 1] for the
               error location, if p->known_err_token is not set. */
            p->known_err_token = t;
            إذا (octal) {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%.3s\" is an invalid octal escape sequence. "
                    "Did you mean \"\\\\%.3s\"? A raw string is also an option.",
                    first_invalid_escape, first_invalid_escape);
            }
            وإلا {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%c\" is an invalid escape sequence. "
                    "Did you mean \"\\\\%c\"? A raw string is also an option.",
                    c, c);
            }
        }
        Py_DECREF(سكو);
        أرجع -1;
    }
    Py_DECREF(سكو);
    أرجع 0;
}

ساكن PyObject *
decode_utf8(ثابت حرف **sPtr, ثابت حرف *end)
{
    ثابت حرف *s;
    ثابت حرف *t;
    t = s = *sPtr;
    طالما (s < end && (*s & 0x80)) {
        s++;
    }
    *sPtr = s;
    أرجع PyUnicode_DecodeUTF8(t, s - t, عدم);
}

ساكن PyObject *
decode_unicode_with_escapes(Parser *parser, ثابت حرف *s, size_t len, Token *t)
{
    PyObject *v;
    حرف *p;
    ثابت حرف *end;

    /* check for integer overflow */
    إذا (len > (size_t)PY_SSIZE_T_MAX / 6) {
        أرجع عدم;
    }
    /* "ä" (2 bytes) may become "\U000000E4" (10 bytes), or 1:5.
     * "\ä" (3 bytes) may become "\u005c\U000000E4" (16 bytes), or ~1:6. */
    Py_ssize_t alloc = (Py_ssize_t)len * 6;
    حرف *buf = PyMem_Malloc(alloc);
    إذا (buf == عدم) {
        أرجع PyErr_NoMemory();
    }
    p = buf;
    end = s + len;
    طالما (s < end) {
        إذا (*s == '\\') {
            *p++ = *s++;
            إذا (s >= end || *s & 0x80) {
                memcpy(p, "u005c", 5);
                p += 5;
                إذا (s >= end) {
                    اكسر;
                }
            }
        }
        إذا (*s & 0x80) {
            PyObject *w;
            صحيح kind;
            ثابت فراغ *data;
            Py_ssize_t w_len;
            Py_ssize_t i;
            w = decode_utf8(&s, end);
            إذا (w == عدم) {
                PyMem_Free(buf);
                أرجع عدم;
            }
            kind = PyUnicode_KIND(w);
            data = PyUnicode_DATA(w);
            w_len = PyUnicode_GET_LENGTH(w);
            لكل (i = 0; i < w_len; i++) {
                // sprintf() writes a null byte: the buffer is large enough
                // for that thanks to the overallocation.
                أكد((p + 11 - buf) <= alloc);
                Py_UCS4 chr = PyUnicode_READ(kind, data, i);
                sprintf(p, "\\U%08x", chr);
                p += 10;
            }
            Py_DECREF(w);
        }
        وإلا {
            *p++ = *s++;
        }
    }
    len = (size_t)(p - buf);

    صحيح first_invalid_escape_char;
    ثابت حرف *first_invalid_escape_ptr;
    v = _PyUnicode_DecodeUnicodeEscapeInternal2(buf, (Py_ssize_t)len, عدم, عدم,
                                                &first_invalid_escape_char,
                                                &first_invalid_escape_ptr);

    // HACK: later we can simply pass the line no, since we don't preserve the tokens
    // when we are decoding the string but we preserve the line numbers.
    إذا (v != عدم && first_invalid_escape_ptr != عدم && t != عدم) {
        إذا (warn_invalid_escape_sequence(parser, buf, first_invalid_escape_ptr, t) < 0) {
            /* We have not deallocated the buffer before because
             * first_invalid_escape_ptr points inside buf. */
            PyMem_Free(buf);
            Py_DECREF(v);
            أرجع عدم;
        }
    }
    PyMem_Free(buf);
    أرجع v;
}

ساكن PyObject *
decode_bytes_with_escapes(Parser *p, ثابت حرف *s, Py_ssize_t len, Token *t)
{
    صحيح first_invalid_escape_char;
    ثابت حرف *first_invalid_escape_ptr;
    PyObject *result = _PyBytes_DecodeEscape2(s, len, عدم,
                                              &first_invalid_escape_char,
                                              &first_invalid_escape_ptr);
    إذا (result == عدم) {
        أرجع عدم;
    }

    إذا (first_invalid_escape_ptr != عدم) {
        إذا (warn_invalid_escape_sequence(p, s, first_invalid_escape_ptr, t) < 0) {
            Py_DECREF(result);
            أرجع عدم;
        }
    }
    أرجع result;
}

PyObject *
_PyPegen_decode_string(Parser *p, صحيح raw, ثابت حرف *s, size_t len, Token *t)
{
    إذا (raw) {
        أرجع PyUnicode_DecodeUTF8Stateful(s, (Py_ssize_t)len, عدم, عدم);
    }
    أرجع decode_unicode_with_escapes(p, s, len, t);
}

/* s must include the bracketing quote characters, and r, b &/or f prefixes
    (if any), and embedded escape sequences (if any). (f-strings are handled by the parser)
   _PyPegen_parse_string parses it, and returns the decoded Python string object. */
PyObject *
_PyPegen_parse_string(Parser *p, Token *t)
{
    ثابت حرف *s = PyBytes_AsString(t->bytes);
    إذا (s == عدم) {
        أرجع عدم;
    }

    size_t len;
    صحيح quote = Py_CHARMASK(*s);
    صحيح bytesmode = 0;
    صحيح rawmode = 0;

    إذا (Py_ISALPHA(quote)) {
        طالما (!bytesmode || !rawmode) {
            إذا (quote == 'b' || quote == 'B') {
                quote =(unsigned حرف)*++s;
                bytesmode = 1;
            }
            وإلا إذا (quote == 'u' || quote == 'U') {
                quote = (unsigned حرف)*++s;
            }
            وإلا إذا (quote == 'r' || quote == 'R') {
                quote = (unsigned حرف)*++s;
                rawmode = 1;
            }
            وإلا {
                اكسر;
            }
        }
    }

    إذا (quote != '\'' && quote != '\"') {
        PyErr_BadInternalCall();
        أرجع عدم;
    }

    /* Skip the leading quote char. */
    s++;
    len = strlen(s);
    // gh-120155: 's' contains at least the trailing quote,
    // so the code '--len' below is safe.
    أكد(len >= 1);

    إذا (len > INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "string to parse is too long");
        أرجع عدم;
    }
    إذا (s[--len] != quote) {
        /* Last quote char must match the first. */
        PyErr_BadInternalCall();
        أرجع عدم;
    }
    إذا (len >= 4 && s[0] == quote && s[1] == quote) {
        /* A triple quoted string. We've already skipped one quote at
           the start and one at the end of the string. Now skip the
           two at the start. */
        s += 2;
        len -= 2;
        /* And check that the last two match. */
        إذا (s[--len] != quote || s[--len] != quote) {
            PyErr_BadInternalCall();
            أرجع عدم;
        }
    }

    /* Avoid invoking escape decoding routines if possible. */
    rawmode = rawmode || strchr(s, '\\') == عدم;
    إذا (bytesmode) {
        /* Disallow non-ASCII characters. */
        ثابت حرف *ch;
        لكل (ch = s; *ch; ch++) {
            إذا (Py_CHARMASK(*ch) >= 0x80) {
                RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
                                   t,
                                   "bytes can only contain ASCII "
                                   "literal characters");
                أرجع عدم;
            }
        }
        إذا (rawmode) {
            أرجع PyBytes_FromStringAndSize(s, (Py_ssize_t)len);
        }
        أرجع decode_bytes_with_escapes(p, s, (Py_ssize_t)len, t);
    }
    أرجع _PyPegen_decode_string(p, rawmode, s, len, t);
}
