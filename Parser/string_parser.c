#অন্তর্ভাব <Python.h>
#অন্তর্ভাব "pycore_bytesobject.h"   // _PyBytes_DecodeEscape()
#অন্তর্ভাব "pycore_unicodeobject.h" // _PyUnicode_DecodeUnicodeEscapeInternal()

#অন্তর্ভাব "pegen.h"
#অন্তর্ভাব "string_parser.h"

#অন্তর্ভাব <stdbool.h>

//// STRING HANDLING FUNCTIONS ////

স্থির পূর্ণাঙ্ক
warn_invalid_escape_sequence(Parser *p, ধ্রুব বর্ণ* buffer, ধ্রুব বর্ণ *first_invalid_escape, Token *t)
{
    যদি (p->call_invalid_rules) {
        // Do not report warnings if we are in the second pass of the parser
        // to avoid showing the warning twice.
        প্রত্যাগম 0;
    }
    unsigned বর্ণ c = (unsigned বর্ণ)*first_invalid_escape;
    যদি ((t->type == FSTRING_মধ্য || t->type == FSTRING_END || t->type == TSTRING_মধ্য || t->type == TSTRING_END)
            && (c == '{' || c == '}')) {
        // in this case the tokenizer has already emitted a warning,
        // see Parser/tokenizer/helpers.c:warn_invalid_escape_sequence
        প্রত্যাগম 0;
    }

    পূর্ণাঙ্ক octal = ('4' <= c && c <= '7');
    PyObject *বার্তা =
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
    যদি (বার্তা == রিক্ত) {
        প্রত্যাগম -1;
    }
    PyObject *category;
    যদি (p->feature_version >= 12) {
        category = PyExc_SyntaxWarning;
    }
    অন্যথা {
        category = PyExc_DeprecationWarning;
    }

    // Calculate the lineno and the col_offset of the invalid escape sequence
    ধ্রুব বর্ণ *start = buffer;
    ধ্রুব বর্ণ *end = first_invalid_escape;
    পূর্ণাঙ্ক lineno = t->lineno;
    পূর্ণাঙ্ক col_offset = t->col_offset;
    যাবত্ (start < end) {
        যদি (*start == '\n') {
            lineno++;
            col_offset = 0;
        }
        অন্যথা {
            col_offset++;
        }
        start++;
    }

    // Count the number of quotes in the token
    বর্ণ first_quote = 0;
    যদি (lineno == t->lineno) {
        পূর্ণাঙ্ক quote_গুনতি = 0;
        ধ্রুব বর্ণ* tok = PyBytes_AsString(t->bytes);
        প্রতি (পূর্ণাঙ্ক i = 0; i < PyBytes_Size(t->bytes); i++) {
            যদি (tok[i] == '\'' || tok[i] == '\"') {
                যদি (quote_গুনতি == 0) {
                    first_quote = tok[i];
                }
                যদি (tok[i] == first_quote) {
                    quote_গুনতি++;
                }
            } অন্যথা {
                ভঙ্গ;
            }
        }

        col_offset += quote_গুনতি;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    যদি (PyErr_WarnExplicitObject(category, বার্তা, info.filename,
                                 lineno, info.module, রিক্ত) < 0) {
        যদি (PyErr_ExceptionMatches(category)) {
            /* Replace the Syntax/DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            /* This is needed, in order for the SyntaxError to point to the token t,
               since _PyPegen_raise_error uses p->tokens[p->fill - 1] for the
               error location, if p->known_err_token is not set. */
            p->known_err_token = t;
            যদি (octal) {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%.3s\" is an invalid octal escape sequence. "
                    "Did you mean \"\\\\%.3s\"? A raw string is also an option.",
                    first_invalid_escape, first_invalid_escape);
            }
            অন্যথা {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%c\" is an invalid escape sequence. "
                    "Did you mean \"\\\\%c\"? A raw string is also an option.",
                    c, c);
            }
        }
        Py_DECREF(বার্তা);
        প্রত্যাগম -1;
    }
    Py_DECREF(বার্তা);
    প্রত্যাগম 0;
}

স্থির PyObject *
decode_utf8(ধ্রুব বর্ণ **sPtr, ধ্রুব বর্ণ *end)
{
    ধ্রুব বর্ণ *s;
    ধ্রুব বর্ণ *t;
    t = s = *sPtr;
    যাবত্ (s < end && (*s & 0x80)) {
        s++;
    }
    *sPtr = s;
    প্রত্যাগম PyUnicode_DecodeUTF8(t, s - t, রিক্ত);
}

স্থির PyObject *
decode_unicode_with_escapes(Parser *parser, ধ্রুব বর্ণ *s, size_t len, Token *t)
{
    PyObject *v;
    বর্ণ *p;
    ধ্রুব বর্ণ *end;

    /* check for integer overflow */
    যদি (len > (size_t)PY_SSIZE_T_MAX / 6) {
        প্রত্যাগম রিক্ত;
    }
    /* "ä" (2 bytes) may become "\U000000E4" (10 bytes), or 1:5.
     * "\ä" (3 bytes) may become "\u005c\U000000E4" (16 bytes), or ~1:6. */
    Py_ssize_t alloc = (Py_ssize_t)len * 6;
    বর্ণ *buf = PyMem_Malloc(alloc);
    যদি (buf == রিক্ত) {
        প্রত্যাগম PyErr_NoMemory();
    }
    p = buf;
    end = s + len;
    যাবত্ (s < end) {
        যদি (*s == '\\') {
            *p++ = *s++;
            যদি (s >= end || *s & 0x80) {
                memcpy(p, "u005c", 5);
                p += 5;
                যদি (s >= end) {
                    ভঙ্গ;
                }
            }
        }
        যদি (*s & 0x80) {
            PyObject *w;
            পূর্ণাঙ্ক kind;
            ধ্রুব শূন্য *data;
            Py_ssize_t w_len;
            Py_ssize_t i;
            w = decode_utf8(&s, end);
            যদি (w == রিক্ত) {
                PyMem_Free(buf);
                প্রত্যাগম রিক্ত;
            }
            kind = PyUnicode_KIND(w);
            data = PyUnicode_DATA(w);
            w_len = PyUnicode_GET_LENGTH(w);
            প্রতি (i = 0; i < w_len; i++) {
                // sprintf() writes a null byte: the buffer is large enough
                // for that thanks to the overallocation.
                প্রতিজ্ঞা((p + 11 - buf) <= alloc);
                Py_UCS4 chr = PyUnicode_READ(kind, data, i);
                sprintf(p, "\\U%08x", chr);
                p += 10;
            }
            Py_DECREF(w);
        }
        অন্যথা {
            *p++ = *s++;
        }
    }
    len = (size_t)(p - buf);

    পূর্ণাঙ্ক first_invalid_escape_char;
    ধ্রুব বর্ণ *first_invalid_escape_ptr;
    v = _PyUnicode_DecodeUnicodeEscapeInternal2(buf, (Py_ssize_t)len, রিক্ত, রিক্ত,
                                                &first_invalid_escape_char,
                                                &first_invalid_escape_ptr);

    // HACK: later we can simply pass the line no, since we don't preserve the tokens
    // when we are decoding the string but we preserve the line numbers.
    যদি (v != রিক্ত && first_invalid_escape_ptr != রিক্ত && t != রিক্ত) {
        যদি (warn_invalid_escape_sequence(parser, buf, first_invalid_escape_ptr, t) < 0) {
            /* We have not deallocated the buffer before because
             * first_invalid_escape_ptr points inside buf. */
            PyMem_Free(buf);
            Py_DECREF(v);
            প্রত্যাগম রিক্ত;
        }
    }
    PyMem_Free(buf);
    প্রত্যাগম v;
}

স্থির PyObject *
decode_bytes_with_escapes(Parser *p, ধ্রুব বর্ণ *s, Py_ssize_t len, Token *t)
{
    পূর্ণাঙ্ক first_invalid_escape_char;
    ধ্রুব বর্ণ *first_invalid_escape_ptr;
    PyObject *result = _PyBytes_DecodeEscape2(s, len, রিক্ত,
                                              &first_invalid_escape_char,
                                              &first_invalid_escape_ptr);
    যদি (result == রিক্ত) {
        প্রত্যাগম রিক্ত;
    }

    যদি (first_invalid_escape_ptr != রিক্ত) {
        যদি (warn_invalid_escape_sequence(p, s, first_invalid_escape_ptr, t) < 0) {
            Py_DECREF(result);
            প্রত্যাগম রিক্ত;
        }
    }
    প্রত্যাগম result;
}

PyObject *
_PyPegen_decode_string(Parser *p, পূর্ণাঙ্ক raw, ধ্রুব বর্ণ *s, size_t len, Token *t)
{
    যদি (raw) {
        প্রত্যাগম PyUnicode_DecodeUTF8Stateful(s, (Py_ssize_t)len, রিক্ত, রিক্ত);
    }
    প্রত্যাগম decode_unicode_with_escapes(p, s, len, t);
}

/* s must include the bracketing quote characters, and r, b &/or f prefixes
    (if any), and embedded escape sequences (if any). (f-strings are handled by the parser)
   _PyPegen_parse_string parses it, and returns the decoded Python string object. */
PyObject *
_PyPegen_parse_string(Parser *p, Token *t)
{
    ধ্রুব বর্ণ *s = PyBytes_AsString(t->bytes);
    যদি (s == রিক্ত) {
        প্রত্যাগম রিক্ত;
    }

    size_t len;
    পূর্ণাঙ্ক quote = Py_CHARMASK(*s);
    পূর্ণাঙ্ক bytesmode = 0;
    পূর্ণাঙ্ক rawmode = 0;

    যদি (Py_ISALPHA(quote)) {
        যাবত্ (!bytesmode || !rawmode) {
            যদি (quote == 'b' || quote == 'B') {
                quote =(unsigned বর্ণ)*++s;
                bytesmode = 1;
            }
            অন্যথা যদি (quote == 'u' || quote == 'U') {
                quote = (unsigned বর্ণ)*++s;
            }
            অন্যথা যদি (quote == 'r' || quote == 'R') {
                quote = (unsigned বর্ণ)*++s;
                rawmode = 1;
            }
            অন্যথা {
                ভঙ্গ;
            }
        }
    }

    যদি (quote != '\'' && quote != '\"') {
        PyErr_BadInternalCall();
        প্রত্যাগম রিক্ত;
    }

    /* Skip the leading quote char. */
    s++;
    len = strlen(s);
    // gh-120155: 's' contains at least the trailing quote,
    // so the code '--len' below is safe.
    প্রতিজ্ঞা(len >= 1);

    যদি (len > INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "string to parse is too long");
        প্রত্যাগম রিক্ত;
    }
    যদি (s[--len] != quote) {
        /* Last quote char must match the first. */
        PyErr_BadInternalCall();
        প্রত্যাগম রিক্ত;
    }
    যদি (len >= 4 && s[0] == quote && s[1] == quote) {
        /* A triple quoted string. We've already skipped one quote at
           the start and one at the end of the string. Now skip the
           two at the start. */
        s += 2;
        len -= 2;
        /* And check that the last two match. */
        যদি (s[--len] != quote || s[--len] != quote) {
            PyErr_BadInternalCall();
            প্রত্যাগম রিক্ত;
        }
    }

    /* Avoid invoking escape decoding routines if possible. */
    rawmode = rawmode || strchr(s, '\\') == রিক্ত;
    যদি (bytesmode) {
        /* Disallow non-ASCII characters. */
        ধ্রুব বর্ণ *ch;
        প্রতি (ch = s; *ch; ch++) {
            যদি (Py_CHARMASK(*ch) >= 0x80) {
                RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
                                   t,
                                   "bytes can only contain ASCII "
                                   "literal characters");
                প্রত্যাগম রিক্ত;
            }
        }
        যদি (rawmode) {
            প্রত্যাগম PyBytes_FromStringAndSize(s, (Py_ssize_t)len);
        }
        প্রত্যাগম decode_bytes_with_escapes(p, s, (Py_ssize_t)len, t);
    }
    প্রত্যাগম _PyPegen_decode_string(p, rawmode, s, len, t);
}
