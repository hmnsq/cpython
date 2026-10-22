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
    ܐܢ (p->call_invalid_rules) {
        // Do not report warnings if we are in the second pass of the parser
        // to avoid showing the warning twice.
        ܦܢܐ 0;
    }
    unsigned حرف c = (unsigned حرف)*first_invalid_escape;
    ܐܢ ((t->type == FSTRING_MIDDLE || t->type == FSTRING_END || t->type == TSTRING_MIDDLE || t->type == TSTRING_END)
            && (c == '{' || c == '}')) {
        // in this case the tokenizer has already emitted a warning,
        // see Parser/tokenizer/helpers.c:warn_invalid_escape_sequence
        ܦܢܐ 0;
    }

    صحيح octal = ('4' <= c && c <= '7');
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
    ܐܢ (msg == عدم) {
        ܦܢܐ -1;
    }
    PyObject *category;
    ܐܢ (p->feature_version >= 12) {
        category = PyExc_SyntaxWarning;
    }
    ܐܚܪܢܐ {
        category = PyExc_DeprecationWarning;
    }

    // Calculate the lineno and the col_offset of the invalid escape sequence
    ثابت حرف *start = buffer;
    ثابت حرف *end = first_invalid_escape;
    صحيح lineno = t->lineno;
    صحيح col_offset = t->col_offset;
    ܟܡܐ (start < end) {
        ܐܢ (*start == '\n') {
            lineno++;
            col_offset = 0;
        }
        ܐܚܪܢܐ {
            col_offset++;
        }
        start++;
    }

    // Count the number of quotes in the token
    حرف first_quote = 0;
    ܐܢ (lineno == t->lineno) {
        صحيح quote_count = 0;
        ثابت حرف* tok = PyBytes_AsString(t->bytes);
        لكل (صحيح i = 0; i < PyBytes_Size(t->bytes); i++) {
            ܐܢ (tok[i] == '\'' || tok[i] == '\"') {
                ܐܢ (quote_count == 0) {
                    first_quote = tok[i];
                }
                ܐܢ (tok[i] == first_quote) {
                    quote_count++;
                }
            } ܐܚܪܢܐ {
                اكسر;
            }
        }

        col_offset += quote_count;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    ܐܢ (PyErr_WarnExplicitObject(category, msg, info.filename,
                                 lineno, info.module, عدم) < 0) {
        ܐܢ (PyErr_ExceptionMatches(category)) {
            /* Replace the Syntax/DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            /* This is needed, in order for the SyntaxError to point to the token t,
               since _PyPegen_raise_error uses p->tokens[p->fill - 1] for the
               error location, if p->known_err_token is not set. */
            p->known_err_token = t;
            ܐܢ (octal) {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%.3s\" is an invalid octal escape sequence. "
                    "Did you mean \"\\\\%.3s\"? A raw string is also an option.",
                    first_invalid_escape, first_invalid_escape);
            }
            ܐܚܪܢܐ {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%c\" is an invalid escape sequence. "
                    "Did you mean \"\\\\%c\"? A raw string is also an option.",
                    c, c);
            }
        }
        Py_DECREF(msg);
        ܦܢܐ -1;
    }
    Py_DECREF(msg);
    ܦܢܐ 0;
}

ساكن PyObject *
decode_utf8(ثابت حرف **sPtr, ثابت حرف *end)
{
    ثابت حرف *s;
    ثابت حرف *t;
    t = s = *sPtr;
    ܟܡܐ (s < end && (*s & 0x80)) {
        s++;
    }
    *sPtr = s;
    ܦܢܐ PyUnicode_DecodeUTF8(t, s - t, عدم);
}

ساكن PyObject *
decode_unicode_with_escapes(Parser *parser, ثابت حرف *s, size_t len, Token *t)
{
    PyObject *v;
    حرف *p;
    ثابت حرف *end;

    /* check for integer overflow */
    ܐܢ (len > (size_t)PY_SSIZE_T_MAX / 6) {
        ܦܢܐ عدم;
    }
    /* "ä" (2 bytes) may become "\U000000E4" (10 bytes), or 1:5.
     * "\ä" (3 bytes) may become "\u005c\U000000E4" (16 bytes), or ~1:6. */
    Py_ssize_t alloc = (Py_ssize_t)len * 6;
    حرف *buf = PyMem_Malloc(alloc);
    ܐܢ (buf == عدم) {
        ܦܢܐ PyErr_NoMemory();
    }
    p = buf;
    end = s + len;
    ܟܡܐ (s < end) {
        ܐܢ (*s == '\\') {
            *p++ = *s++;
            ܐܢ (s >= end || *s & 0x80) {
                memcpy(p, "u005c", 5);
                p += 5;
                ܐܢ (s >= end) {
                    اكسر;
                }
            }
        }
        ܐܢ (*s & 0x80) {
            PyObject *w;
            صحيح kind;
            ثابت فراغ *data;
            Py_ssize_t w_len;
            Py_ssize_t i;
            w = decode_utf8(&s, end);
            ܐܢ (w == عدم) {
                PyMem_Free(buf);
                ܦܢܐ عدم;
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
        ܐܚܪܢܐ {
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
    ܐܢ (v != عدم && first_invalid_escape_ptr != عدم && t != عدم) {
        ܐܢ (warn_invalid_escape_sequence(parser, buf, first_invalid_escape_ptr, t) < 0) {
            /* We have not deallocated the buffer before because
             * first_invalid_escape_ptr points inside buf. */
            PyMem_Free(buf);
            Py_DECREF(v);
            ܦܢܐ عدم;
        }
    }
    PyMem_Free(buf);
    ܦܢܐ v;
}

ساكن PyObject *
decode_bytes_with_escapes(Parser *p, ثابت حرف *s, Py_ssize_t len, Token *t)
{
    صحيح first_invalid_escape_char;
    ثابت حرف *first_invalid_escape_ptr;
    PyObject *result = _PyBytes_DecodeEscape2(s, len, عدم,
                                              &first_invalid_escape_char,
                                              &first_invalid_escape_ptr);
    ܐܢ (result == عدم) {
        ܦܢܐ عدم;
    }

    ܐܢ (first_invalid_escape_ptr != عدم) {
        ܐܢ (warn_invalid_escape_sequence(p, s, first_invalid_escape_ptr, t) < 0) {
            Py_DECREF(result);
            ܦܢܐ عدم;
        }
    }
    ܦܢܐ result;
}

PyObject *
_PyPegen_decode_string(Parser *p, صحيح raw, ثابت حرف *s, size_t len, Token *t)
{
    ܐܢ (raw) {
        ܦܢܐ PyUnicode_DecodeUTF8Stateful(s, (Py_ssize_t)len, عدم, عدم);
    }
    ܦܢܐ decode_unicode_with_escapes(p, s, len, t);
}

/* s must include the bracketing quote characters, and r, b &/or f prefixes
    (if any), and embedded escape sequences (if any). (f-strings are handled by the parser)
   _PyPegen_parse_string parses it, and returns the decoded Python string object. */
PyObject *
_PyPegen_parse_string(Parser *p, Token *t)
{
    ثابت حرف *s = PyBytes_AsString(t->bytes);
    ܐܢ (s == عدم) {
        ܦܢܐ عدم;
    }

    size_t len;
    صحيح quote = Py_CHARMASK(*s);
    صحيح bytesmode = 0;
    صحيح rawmode = 0;

    ܐܢ (Py_ISALPHA(quote)) {
        ܟܡܐ (!bytesmode || !rawmode) {
            ܐܢ (quote == 'b' || quote == 'B') {
                quote =(unsigned حرف)*++s;
                bytesmode = 1;
            }
            ܐܚܪܢܐ ܐܢ (quote == 'u' || quote == 'U') {
                quote = (unsigned حرف)*++s;
            }
            ܐܚܪܢܐ ܐܢ (quote == 'r' || quote == 'R') {
                quote = (unsigned حرف)*++s;
                rawmode = 1;
            }
            ܐܚܪܢܐ {
                اكسر;
            }
        }
    }

    ܐܢ (quote != '\'' && quote != '\"') {
        PyErr_BadInternalCall();
        ܦܢܐ عدم;
    }

    /* Skip the leading quote char. */
    s++;
    len = strlen(s);
    // gh-120155: 's' contains at least the trailing quote,
    // so the code '--len' below is safe.
    أكد(len >= 1);

    ܐܢ (len > INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "string to parse is too long");
        ܦܢܐ عدم;
    }
    ܐܢ (s[--len] != quote) {
        /* Last quote char must match the first. */
        PyErr_BadInternalCall();
        ܦܢܐ عدم;
    }
    ܐܢ (len >= 4 && s[0] == quote && s[1] == quote) {
        /* A triple quoted string. We've already skipped one quote at
           the start and one at the end of the string. Now skip the
           two at the start. */
        s += 2;
        len -= 2;
        /* And check that the last two match. */
        ܐܢ (s[--len] != quote || s[--len] != quote) {
            PyErr_BadInternalCall();
            ܦܢܐ عدم;
        }
    }

    /* Avoid invoking escape decoding routines if possible. */
    rawmode = rawmode || strchr(s, '\\') == عدم;
    ܐܢ (bytesmode) {
        /* Disallow non-ASCII characters. */
        ثابت حرف *ch;
        لكل (ch = s; *ch; ch++) {
            ܐܢ (Py_CHARMASK(*ch) >= 0x80) {
                RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
                                   t,
                                   "bytes can only contain ASCII "
                                   "literal characters");
                ܦܢܐ عدم;
            }
        }
        ܐܢ (rawmode) {
            ܦܢܐ PyBytes_FromStringAndSize(s, (Py_ssize_t)len);
        }
        ܦܢܐ decode_bytes_with_escapes(p, s, (Py_ssize_t)len, t);
    }
    ܦܢܐ _PyPegen_decode_string(p, rawmode, s, len, t);
}
