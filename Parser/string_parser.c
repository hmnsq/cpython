#ಅನ್ತರ್ಭಾವ <Python.h>
#ಅನ್ತರ್ಭಾವ "pycore_bytesobject.h"   // _PyBytes_DecodeEscape()
#ಅನ್ತರ್ಭಾವ "pycore_unicodeobject.h" // _PyUnicode_DecodeUnicodeEscapeInternal()

#ಅನ್ತರ್ಭಾವ "pegen.h"
#ಅನ್ತರ್ಭಾವ "string_parser.h"

#ಅನ್ತರ್ಭಾವ <stdbool.h>

//// STRING HANDLING FUNCTIONS ////

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
warn_invalid_escape_sequence(Parser *p, ಧ್ರುವ ವರ್ಣ* buffer, ಧ್ರುವ ವರ್ಣ *first_invalid_escape, Token *t)
{
    ಯದಿ (p->call_invalid_rules) {
        // Do not report warnings if we are in the second pass of the parser
        // to avoid showing the warning twice.
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    unsigned ವರ್ಣ c = (unsigned ವರ್ಣ)*first_invalid_escape;
    ಯದಿ ((t->type == FSTRING_ಮಧ್ಯ || t->type == FSTRING_END || t->type == TSTRING_ಮಧ್ಯ || t->type == TSTRING_END)
            && (c == '{' || c == '}')) {
        // in this case the tokenizer has already emitted a warning,
        // see Parser/tokenizer/helpers.c:warn_invalid_escape_sequence
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಪೂರ್ಣಾಙ್ಕ octal = ('4' <= c && c <= '7');
    PyObject *ಸಂದೇಶ =
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
    ಯದಿ (ಸಂದೇಶ == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    PyObject *category;
    ಯದಿ (p->feature_version >= 12) {
        category = PyExc_SyntaxWarning;
    }
    ಅನ್ಯಥಾ {
        category = PyExc_DeprecationWarning;
    }

    // Calculate the lineno and the col_offset of the invalid escape sequence
    ಧ್ರುವ ವರ್ಣ *start = buffer;
    ಧ್ರುವ ವರ್ಣ *end = first_invalid_escape;
    ಪೂರ್ಣಾಙ್ಕ lineno = t->lineno;
    ಪೂರ್ಣಾಙ್ಕ col_offset = t->col_offset;
    ಯಾವತ್ (start < end) {
        ಯದಿ (*start == '\n') {
            lineno++;
            col_offset = 0;
        }
        ಅನ್ಯಥಾ {
            col_offset++;
        }
        start++;
    }

    // Count the number of quotes in the token
    ವರ್ಣ first_quote = 0;
    ಯದಿ (lineno == t->lineno) {
        ಪೂರ್ಣಾಙ್ಕ quote_ಎಣಿಕೆ = 0;
        ಧ್ರುವ ವರ್ಣ* tok = PyBytes_AsString(t->bytes);
        ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < PyBytes_Size(t->bytes); i++) {
            ಯದಿ (tok[i] == '\'' || tok[i] == '\"') {
                ಯದಿ (quote_ಎಣಿಕೆ == 0) {
                    first_quote = tok[i];
                }
                ಯದಿ (tok[i] == first_quote) {
                    quote_ಎಣಿಕೆ++;
                }
            } ಅನ್ಯಥಾ {
                ಭಙ್ಗ;
            }
        }

        col_offset += quote_ಎಣಿಕೆ;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    ಯದಿ (PyErr_WarnExplicitObject(category, ಸಂದೇಶ, info.filename,
                                 lineno, info.module, ರಿಕ್ತ) < 0) {
        ಯದಿ (PyErr_ExceptionMatches(category)) {
            /* Replace the Syntax/DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            /* This is needed, in order for the SyntaxError to point to the token t,
               since _PyPegen_raise_error uses p->tokens[p->fill - 1] for the
               error location, if p->known_err_token is not set. */
            p->known_err_token = t;
            ಯದಿ (octal) {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%.3s\" is an invalid octal escape sequence. "
                    "Did you mean \"\\\\%.3s\"? A raw string is also an option.",
                    first_invalid_escape, first_invalid_escape);
            }
            ಅನ್ಯಥಾ {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%c\" is an invalid escape sequence. "
                    "Did you mean \"\\\\%c\"? A raw string is also an option.",
                    c, c);
            }
        }
        Py_DECREF(ಸಂದೇಶ);
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    Py_DECREF(ಸಂದೇಶ);
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ PyObject *
decode_utf8(ಧ್ರುವ ವರ್ಣ **sPtr, ಧ್ರುವ ವರ್ಣ *end)
{
    ಧ್ರುವ ವರ್ಣ *s;
    ಧ್ರುವ ವರ್ಣ *t;
    t = s = *sPtr;
    ಯಾವತ್ (s < end && (*s & 0x80)) {
        s++;
    }
    *sPtr = s;
    ಪ್ರತ್ಯಾಗಮ PyUnicode_DecodeUTF8(t, s - t, ರಿಕ್ತ);
}

ಸ್ಥಿರ PyObject *
decode_unicode_with_escapes(Parser *parser, ಧ್ರುವ ವರ್ಣ *s, size_t len, Token *t)
{
    PyObject *v;
    ವರ್ಣ *p;
    ಧ್ರುವ ವರ್ಣ *end;

    /* check for integer overflow */
    ಯದಿ (len > (size_t)PY_SSIZE_T_MAX / 6) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    /* "ä" (2 bytes) may become "\U000000E4" (10 bytes), or 1:5.
     * "\ä" (3 bytes) may become "\u005c\U000000E4" (16 bytes), or ~1:6. */
    Py_ssize_t alloc = (Py_ssize_t)len * 6;
    ವರ್ಣ *buf = PyMem_Malloc(alloc);
    ಯದಿ (buf == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ PyErr_NoMemory();
    }
    p = buf;
    end = s + len;
    ಯಾವತ್ (s < end) {
        ಯದಿ (*s == '\\') {
            *p++ = *s++;
            ಯದಿ (s >= end || *s & 0x80) {
                memcpy(p, "u005c", 5);
                p += 5;
                ಯದಿ (s >= end) {
                    ಭಙ್ಗ;
                }
            }
        }
        ಯದಿ (*s & 0x80) {
            PyObject *w;
            ಪೂರ್ಣಾಙ್ಕ kind;
            ಧ್ರುವ ಶೂನ್ಯ *data;
            Py_ssize_t w_len;
            Py_ssize_t i;
            w = decode_utf8(&s, end);
            ಯದಿ (w == ರಿಕ್ತ) {
                PyMem_Free(buf);
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
            }
            kind = PyUnicode_KIND(w);
            data = PyUnicode_DATA(w);
            w_len = PyUnicode_GET_LENGTH(w);
            ಪ್ರತಿ (i = 0; i < w_len; i++) {
                // sprintf() writes a null byte: the buffer is large enough
                // for that thanks to the overallocation.
                ಪ್ರತಿಜ್ಞಾ((p + 11 - buf) <= alloc);
                Py_UCS4 chr = PyUnicode_READ(kind, data, i);
                sprintf(p, "\\U%08x", chr);
                p += 10;
            }
            Py_DECREF(w);
        }
        ಅನ್ಯಥಾ {
            *p++ = *s++;
        }
    }
    len = (size_t)(p - buf);

    ಪೂರ್ಣಾಙ್ಕ first_invalid_escape_char;
    ಧ್ರುವ ವರ್ಣ *first_invalid_escape_ptr;
    v = _PyUnicode_DecodeUnicodeEscapeInternal2(buf, (Py_ssize_t)len, ರಿಕ್ತ, ರಿಕ್ತ,
                                                &first_invalid_escape_char,
                                                &first_invalid_escape_ptr);

    // HACK: later we can simply pass the line no, since we don't preserve the tokens
    // when we are decoding the string but we preserve the line numbers.
    ಯದಿ (v != ರಿಕ್ತ && first_invalid_escape_ptr != ರಿಕ್ತ && t != ರಿಕ್ತ) {
        ಯದಿ (warn_invalid_escape_sequence(parser, buf, first_invalid_escape_ptr, t) < 0) {
            /* We have not deallocated the buffer before because
             * first_invalid_escape_ptr points inside buf. */
            PyMem_Free(buf);
            Py_DECREF(v);
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
    }
    PyMem_Free(buf);
    ಪ್ರತ್ಯಾಗಮ v;
}

ಸ್ಥಿರ PyObject *
decode_bytes_with_escapes(Parser *p, ಧ್ರುವ ವರ್ಣ *s, Py_ssize_t len, Token *t)
{
    ಪೂರ್ಣಾಙ್ಕ first_invalid_escape_char;
    ಧ್ರುವ ವರ್ಣ *first_invalid_escape_ptr;
    PyObject *result = _PyBytes_DecodeEscape2(s, len, ರಿಕ್ತ,
                                              &first_invalid_escape_char,
                                              &first_invalid_escape_ptr);
    ಯದಿ (result == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (first_invalid_escape_ptr != ರಿಕ್ತ) {
        ಯದಿ (warn_invalid_escape_sequence(p, s, first_invalid_escape_ptr, t) < 0) {
            Py_DECREF(result);
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

PyObject *
_PyPegen_decode_string(Parser *p, ಪೂರ್ಣಾಙ್ಕ raw, ಧ್ರುವ ವರ್ಣ *s, size_t len, Token *t)
{
    ಯದಿ (raw) {
        ಪ್ರತ್ಯಾಗಮ PyUnicode_DecodeUTF8Stateful(s, (Py_ssize_t)len, ರಿಕ್ತ, ರಿಕ್ತ);
    }
    ಪ್ರತ್ಯಾಗಮ decode_unicode_with_escapes(p, s, len, t);
}

/* s must include the bracketing quote characters, and r, b &/or f prefixes
    (if any), and embedded escape sequences (if any). (f-strings are handled by the parser)
   _PyPegen_parse_string parses it, and returns the decoded Python string object. */
PyObject *
_PyPegen_parse_string(Parser *p, Token *t)
{
    ಧ್ರುವ ವರ್ಣ *s = PyBytes_AsString(t->bytes);
    ಯದಿ (s == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    size_t len;
    ಪೂರ್ಣಾಙ್ಕ quote = Py_CHARMASK(*s);
    ಪೂರ್ಣಾಙ್ಕ bytesmode = 0;
    ಪೂರ್ಣಾಙ್ಕ rawmode = 0;

    ಯದಿ (Py_ISALPHA(quote)) {
        ಯಾವತ್ (!bytesmode || !rawmode) {
            ಯದಿ (quote == 'b' || quote == 'B') {
                quote =(unsigned ವರ್ಣ)*++s;
                bytesmode = 1;
            }
            ಅನ್ಯಥಾ ಯದಿ (quote == 'u' || quote == 'U') {
                quote = (unsigned ವರ್ಣ)*++s;
            }
            ಅನ್ಯಥಾ ಯದಿ (quote == 'r' || quote == 'R') {
                quote = (unsigned ವರ್ಣ)*++s;
                rawmode = 1;
            }
            ಅನ್ಯಥಾ {
                ಭಙ್ಗ;
            }
        }
    }

    ಯದಿ (quote != '\'' && quote != '\"') {
        PyErr_BadInternalCall();
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    /* Skip the leading quote char. */
    s++;
    len = strlen(s);
    // gh-120155: 's' contains at least the trailing quote,
    // so the code '--len' below is safe.
    ಪ್ರತಿಜ್ಞಾ(len >= 1);

    ಯದಿ (len > INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "string to parse is too long");
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (s[--len] != quote) {
        /* Last quote char must match the first. */
        PyErr_BadInternalCall();
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (len >= 4 && s[0] == quote && s[1] == quote) {
        /* A triple quoted string. We've already skipped one quote at
           the start and one at the end of the string. Now skip the
           two at the start. */
        s += 2;
        len -= 2;
        /* And check that the last two match. */
        ಯದಿ (s[--len] != quote || s[--len] != quote) {
            PyErr_BadInternalCall();
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
    }

    /* Avoid invoking escape decoding routines if possible. */
    rawmode = rawmode || strchr(s, '\\') == ರಿಕ್ತ;
    ಯದಿ (bytesmode) {
        /* Disallow non-ASCII characters. */
        ಧ್ರುವ ವರ್ಣ *ch;
        ಪ್ರತಿ (ch = s; *ch; ch++) {
            ಯದಿ (Py_CHARMASK(*ch) >= 0x80) {
                RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
                                   t,
                                   "bytes can only contain ASCII "
                                   "literal characters");
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
            }
        }
        ಯದಿ (rawmode) {
            ಪ್ರತ್ಯಾಗಮ PyBytes_FromStringAndSize(s, (Py_ssize_t)len);
        }
        ಪ್ರತ್ಯಾಗಮ decode_bytes_with_escapes(p, s, (Py_ssize_t)len, t);
    }
    ಪ್ರತ್ಯಾಗಮ _PyPegen_decode_string(p, rawmode, s, len, t);
}
