#شامل <Python.h>
#شامل "pycore_bytesobject.h"   // _PyBytes_DecodeEscape()
#شامل "pycore_unicodeobject.h" // _PyUnicode_DecodeUnicodeEscapeInternal()

#شامل "pegen.h"
#شامل "string_parser.h"

#شامل <stdbool.h>

//// STRING HANDLING FUNCTIONS ////

ایستا صحیح
warn_invalid_escape_sequence(Parser *p, ثابت نویسه* buffer, ثابت نویسه *first_invalid_escape, Token *t)
{
    اگر (p->call_invalid_rules) {
        // Do not report warnings if we are in the second pass of the parser
        // to avoid showing the warning twice.
        بازگشت 0;
    }
    unsigned نویسه c = (unsigned نویسه)*first_invalid_escape;
    اگر ((t->type == FSTRING_میانه || t->type == FSTRING_END || t->type == TSTRING_میانه || t->type == TSTRING_END)
            && (c == '{' || c == '}')) {
        // in this case the tokenizer has already emitted a warning,
        // see Parser/tokenizer/helpers.c:warn_invalid_escape_sequence
        بازگشت 0;
    }

    صحیح octal = ('4' <= c && c <= '7');
    PyObject *پیام =
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
    اگر (پیام == پوچ) {
        بازگشت -1;
    }
    PyObject *category;
    اگر (p->feature_version >= 12) {
        category = PyExc_SyntaxWarning;
    }
    وگرنه {
        category = PyExc_DeprecationWarning;
    }

    // Calculate the lineno and the col_offset of the invalid escape sequence
    ثابت نویسه *start = buffer;
    ثابت نویسه *end = first_invalid_escape;
    صحیح lineno = t->lineno;
    صحیح col_offset = t->col_offset;
    وقتی (start < end) {
        اگر (*start == '\n') {
            lineno++;
            col_offset = 0;
        }
        وگرنه {
            col_offset++;
        }
        start++;
    }

    // Count the number of quotes in the token
    نویسه first_quote = 0;
    اگر (lineno == t->lineno) {
        صحیح quote_شمار = 0;
        ثابت نویسه* tok = PyBytes_AsString(t->bytes);
        برای (صحیح i = 0; i < PyBytes_Size(t->bytes); i++) {
            اگر (tok[i] == '\'' || tok[i] == '\"') {
                اگر (quote_شمار == 0) {
                    first_quote = tok[i];
                }
                اگر (tok[i] == first_quote) {
                    quote_شمار++;
                }
            } وگرنه {
                بشکن;
            }
        }

        col_offset += quote_شمار;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    اگر (PyErr_WarnExplicitObject(category, پیام, info.filename,
                                 lineno, info.module, پوچ) < 0) {
        اگر (PyErr_ExceptionMatches(category)) {
            /* Replace the Syntax/DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            /* This is needed, in order for the SyntaxError to point to the token t,
               since _PyPegen_raise_error uses p->tokens[p->fill - 1] for the
               error location, if p->known_err_token is not set. */
            p->known_err_token = t;
            اگر (octal) {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%.3s\" is an invalid octal escape sequence. "
                    "Did you mean \"\\\\%.3s\"? A raw string is also an option.",
                    first_invalid_escape, first_invalid_escape);
            }
            وگرنه {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%c\" is an invalid escape sequence. "
                    "Did you mean \"\\\\%c\"? A raw string is also an option.",
                    c, c);
            }
        }
        Py_DECREF(پیام);
        بازگشت -1;
    }
    Py_DECREF(پیام);
    بازگشت 0;
}

ایستا PyObject *
decode_utf8(ثابت نویسه **sPtr, ثابت نویسه *end)
{
    ثابت نویسه *s;
    ثابت نویسه *t;
    t = s = *sPtr;
    وقتی (s < end && (*s & 0x80)) {
        s++;
    }
    *sPtr = s;
    بازگشت PyUnicode_DecodeUTF8(t, s - t, پوچ);
}

ایستا PyObject *
decode_unicode_with_escapes(Parser *parser, ثابت نویسه *s, size_t len, Token *t)
{
    PyObject *v;
    نویسه *p;
    ثابت نویسه *end;

    /* check for integer overflow */
    اگر (len > (size_t)PY_SSIZE_T_MAX / 6) {
        بازگشت پوچ;
    }
    /* "ä" (2 bytes) may become "\U000000E4" (10 bytes), or 1:5.
     * "\ä" (3 bytes) may become "\u005c\U000000E4" (16 bytes), or ~1:6. */
    Py_ssize_t alloc = (Py_ssize_t)len * 6;
    نویسه *buf = PyMem_Malloc(alloc);
    اگر (buf == پوچ) {
        بازگشت PyErr_NoMemory();
    }
    p = buf;
    end = s + len;
    وقتی (s < end) {
        اگر (*s == '\\') {
            *p++ = *s++;
            اگر (s >= end || *s & 0x80) {
                memcpy(p, "u005c", 5);
                p += 5;
                اگر (s >= end) {
                    بشکن;
                }
            }
        }
        اگر (*s & 0x80) {
            PyObject *w;
            صحیح kind;
            ثابت تهی *data;
            Py_ssize_t w_len;
            Py_ssize_t i;
            w = decode_utf8(&s, end);
            اگر (w == پوچ) {
                PyMem_Free(buf);
                بازگشت پوچ;
            }
            kind = PyUnicode_KIND(w);
            data = PyUnicode_DATA(w);
            w_len = PyUnicode_GET_LENGTH(w);
            برای (i = 0; i < w_len; i++) {
                // sprintf() writes a null byte: the buffer is large enough
                // for that thanks to the overallocation.
                أكد((p + 11 - buf) <= alloc);
                Py_UCS4 chr = PyUnicode_READ(kind, data, i);
                sprintf(p, "\\U%08x", chr);
                p += 10;
            }
            Py_DECREF(w);
        }
        وگرنه {
            *p++ = *s++;
        }
    }
    len = (size_t)(p - buf);

    صحیح first_invalid_escape_char;
    ثابت نویسه *first_invalid_escape_ptr;
    v = _PyUnicode_DecodeUnicodeEscapeInternal2(buf, (Py_ssize_t)len, پوچ, پوچ,
                                                &first_invalid_escape_char,
                                                &first_invalid_escape_ptr);

    // HACK: later we can simply pass the line no, since we don't preserve the tokens
    // when we are decoding the string but we preserve the line numbers.
    اگر (v != پوچ && first_invalid_escape_ptr != پوچ && t != پوچ) {
        اگر (warn_invalid_escape_sequence(parser, buf, first_invalid_escape_ptr, t) < 0) {
            /* We have not deallocated the buffer before because
             * first_invalid_escape_ptr points inside buf. */
            PyMem_Free(buf);
            Py_DECREF(v);
            بازگشت پوچ;
        }
    }
    PyMem_Free(buf);
    بازگشت v;
}

ایستا PyObject *
decode_bytes_with_escapes(Parser *p, ثابت نویسه *s, Py_ssize_t len, Token *t)
{
    صحیح first_invalid_escape_char;
    ثابت نویسه *first_invalid_escape_ptr;
    PyObject *result = _PyBytes_DecodeEscape2(s, len, پوچ,
                                              &first_invalid_escape_char,
                                              &first_invalid_escape_ptr);
    اگر (result == پوچ) {
        بازگشت پوچ;
    }

    اگر (first_invalid_escape_ptr != پوچ) {
        اگر (warn_invalid_escape_sequence(p, s, first_invalid_escape_ptr, t) < 0) {
            Py_DECREF(result);
            بازگشت پوچ;
        }
    }
    بازگشت result;
}

PyObject *
_PyPegen_decode_string(Parser *p, صحیح raw, ثابت نویسه *s, size_t len, Token *t)
{
    اگر (raw) {
        بازگشت PyUnicode_DecodeUTF8Stateful(s, (Py_ssize_t)len, پوچ, پوچ);
    }
    بازگشت decode_unicode_with_escapes(p, s, len, t);
}

/* s must include the bracketing quote characters, and r, b &/or f prefixes
    (if any), and embedded escape sequences (if any). (f-strings are handled by the parser)
   _PyPegen_parse_string parses it, and returns the decoded Python string object. */
PyObject *
_PyPegen_parse_string(Parser *p, Token *t)
{
    ثابت نویسه *s = PyBytes_AsString(t->bytes);
    اگر (s == پوچ) {
        بازگشت پوچ;
    }

    size_t len;
    صحیح quote = Py_CHARMASK(*s);
    صحیح bytesmode = 0;
    صحیح rawmode = 0;

    اگر (Py_ISALPHA(quote)) {
        وقتی (!bytesmode || !rawmode) {
            اگر (quote == 'b' || quote == 'B') {
                quote =(unsigned نویسه)*++s;
                bytesmode = 1;
            }
            وگرنه اگر (quote == 'u' || quote == 'U') {
                quote = (unsigned نویسه)*++s;
            }
            وگرنه اگر (quote == 'r' || quote == 'R') {
                quote = (unsigned نویسه)*++s;
                rawmode = 1;
            }
            وگرنه {
                بشکن;
            }
        }
    }

    اگر (quote != '\'' && quote != '\"') {
        PyErr_BadInternalCall();
        بازگشت پوچ;
    }

    /* Skip the leading quote char. */
    s++;
    len = strlen(s);
    // gh-120155: 's' contains at least the trailing quote,
    // so the code '--len' below is safe.
    أكد(len >= 1);

    اگر (len > INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "string to parse is too long");
        بازگشت پوچ;
    }
    اگر (s[--len] != quote) {
        /* Last quote char must match the first. */
        PyErr_BadInternalCall();
        بازگشت پوچ;
    }
    اگر (len >= 4 && s[0] == quote && s[1] == quote) {
        /* A triple quoted string. We've already skipped one quote at
           the start and one at the end of the string. Now skip the
           two at the start. */
        s += 2;
        len -= 2;
        /* And check that the last two match. */
        اگر (s[--len] != quote || s[--len] != quote) {
            PyErr_BadInternalCall();
            بازگشت پوچ;
        }
    }

    /* Avoid invoking escape decoding routines if possible. */
    rawmode = rawmode || strchr(s, '\\') == پوچ;
    اگر (bytesmode) {
        /* Disallow non-ASCII characters. */
        ثابت نویسه *ch;
        برای (ch = s; *ch; ch++) {
            اگر (Py_CHARMASK(*ch) >= 0x80) {
                RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
                                   t,
                                   "bytes can only contain ASCII "
                                   "literal characters");
                بازگشت پوچ;
            }
        }
        اگر (rawmode) {
            بازگشت PyBytes_FromStringAndSize(s, (Py_ssize_t)len);
        }
        بازگشت decode_bytes_with_escapes(p, s, (Py_ssize_t)len, t);
    }
    بازگشت _PyPegen_decode_string(p, rawmode, s, len, t);
}
