#அந்தர்பாவ <Python.h>
#அந்தர்பாவ "pycore_bytesobject.h"   // _PyBytes_DecodeEscape()
#அந்தர்பாவ "pycore_unicodeobject.h" // _PyUnicode_DecodeUnicodeEscapeInternal()

#அந்தர்பாவ "pegen.h"
#அந்தர்பாவ "string_parser.h"

#அந்தர்பாவ <stdbool.h>

//// STRING HANDLING FUNCTIONS ////

நிலையான முழு
warn_invalid_escape_sequence(Parser *p, மாறிலி எழுத்து* buffer, மாறிலி எழுத்து *first_invalid_escape, Token *t)
{
    எனில் (p->call_invalid_rules) {
        // Do not report warnings if we are in the second pass of the parser
        // to avoid showing the warning twice.
        திரும்பு 0;
    }
    unsigned எழுத்து c = (unsigned எழுத்து)*first_invalid_escape;
    எனில் ((t->type == FSTRING_நடு || t->type == FSTRING_END || t->type == TSTRING_நடு || t->type == TSTRING_END)
            && (c == '{' || c == '}')) {
        // in this case the tokenizer has already emitted a warning,
        // see Parser/tokenizer/helpers.c:warn_invalid_escape_sequence
        திரும்பு 0;
    }

    முழு octal = ('4' <= c && c <= '7');
    PyObject *செய்தி =
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
    எனில் (செய்தி == வெற்று) {
        திரும்பு -1;
    }
    PyObject *category;
    எனில் (p->feature_version >= 12) {
        category = PyExc_SyntaxWarning;
    }
    இல்லையெனில் {
        category = PyExc_DeprecationWarning;
    }

    // Calculate the lineno and the col_offset of the invalid escape sequence
    மாறிலி எழுத்து *start = buffer;
    மாறிலி எழுத்து *end = first_invalid_escape;
    முழு lineno = t->lineno;
    முழு col_offset = t->col_offset;
    போது (start < end) {
        எனில் (*start == '\n') {
            lineno++;
            col_offset = 0;
        }
        இல்லையெனில் {
            col_offset++;
        }
        start++;
    }

    // Count the number of quotes in the token
    எழுத்து first_quote = 0;
    எனில் (lineno == t->lineno) {
        முழு quote_மொத்தம் = 0;
        மாறிலி எழுத்து* tok = PyBytes_AsString(t->bytes);
        க்கு (முழு i = 0; i < PyBytes_Size(t->bytes); i++) {
            எனில் (tok[i] == '\'' || tok[i] == '\"') {
                எனில் (quote_மொத்தம் == 0) {
                    first_quote = tok[i];
                }
                எனில் (tok[i] == first_quote) {
                    quote_மொத்தம்++;
                }
            } இல்லையெனில் {
                நிறுத்து;
            }
        }

        col_offset += quote_மொத்தம்;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    எனில் (PyErr_WarnExplicitObject(category, செய்தி, info.filename,
                                 lineno, info.module, வெற்று) < 0) {
        எனில் (PyErr_ExceptionMatches(category)) {
            /* Replace the Syntax/DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            /* This is needed, in order for the SyntaxError to point to the token t,
               since _PyPegen_raise_error uses p->tokens[p->fill - 1] for the
               error location, if p->known_err_token is not set. */
            p->known_err_token = t;
            எனில் (octal) {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%.3s\" is an invalid octal escape sequence. "
                    "Did you mean \"\\\\%.3s\"? A raw string is also an option.",
                    first_invalid_escape, first_invalid_escape);
            }
            இல்லையெனில் {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%c\" is an invalid escape sequence. "
                    "Did you mean \"\\\\%c\"? A raw string is also an option.",
                    c, c);
            }
        }
        Py_DECREF(செய்தி);
        திரும்பு -1;
    }
    Py_DECREF(செய்தி);
    திரும்பு 0;
}

நிலையான PyObject *
decode_utf8(மாறிலி எழுத்து **sPtr, மாறிலி எழுத்து *end)
{
    மாறிலி எழுத்து *s;
    மாறிலி எழுத்து *t;
    t = s = *sPtr;
    போது (s < end && (*s & 0x80)) {
        s++;
    }
    *sPtr = s;
    திரும்பு PyUnicode_DecodeUTF8(t, s - t, வெற்று);
}

நிலையான PyObject *
decode_unicode_with_escapes(Parser *parser, மாறிலி எழுத்து *s, size_t len, Token *t)
{
    PyObject *v;
    எழுத்து *p;
    மாறிலி எழுத்து *end;

    /* check for integer overflow */
    எனில் (len > (size_t)PY_SSIZE_T_MAX / 6) {
        திரும்பு வெற்று;
    }
    /* "ä" (2 bytes) may become "\U000000E4" (10 bytes), or 1:5.
     * "\ä" (3 bytes) may become "\u005c\U000000E4" (16 bytes), or ~1:6. */
    Py_ssize_t alloc = (Py_ssize_t)len * 6;
    எழுத்து *buf = PyMem_Malloc(alloc);
    எனில் (buf == வெற்று) {
        திரும்பு PyErr_NoMemory();
    }
    p = buf;
    end = s + len;
    போது (s < end) {
        எனில் (*s == '\\') {
            *p++ = *s++;
            எனில் (s >= end || *s & 0x80) {
                memcpy(p, "u005c", 5);
                p += 5;
                எனில் (s >= end) {
                    நிறுத்து;
                }
            }
        }
        எனில் (*s & 0x80) {
            PyObject *w;
            முழு kind;
            மாறிலி வெறுமை *data;
            Py_ssize_t w_len;
            Py_ssize_t i;
            w = decode_utf8(&s, end);
            எனில் (w == வெற்று) {
                PyMem_Free(buf);
                திரும்பு வெற்று;
            }
            kind = PyUnicode_KIND(w);
            data = PyUnicode_DATA(w);
            w_len = PyUnicode_GET_LENGTH(w);
            க்கு (i = 0; i < w_len; i++) {
                // sprintf() writes a null byte: the buffer is large enough
                // for that thanks to the overallocation.
                ப்ரதிஜ்ஞா((p + 11 - buf) <= alloc);
                Py_UCS4 chr = PyUnicode_READ(kind, data, i);
                sprintf(p, "\\U%08x", chr);
                p += 10;
            }
            Py_DECREF(w);
        }
        இல்லையெனில் {
            *p++ = *s++;
        }
    }
    len = (size_t)(p - buf);

    முழு first_invalid_escape_char;
    மாறிலி எழுத்து *first_invalid_escape_ptr;
    v = _PyUnicode_DecodeUnicodeEscapeInternal2(buf, (Py_ssize_t)len, வெற்று, வெற்று,
                                                &first_invalid_escape_char,
                                                &first_invalid_escape_ptr);

    // HACK: later we can simply pass the line no, since we don't preserve the tokens
    // when we are decoding the string but we preserve the line numbers.
    எனில் (v != வெற்று && first_invalid_escape_ptr != வெற்று && t != வெற்று) {
        எனில் (warn_invalid_escape_sequence(parser, buf, first_invalid_escape_ptr, t) < 0) {
            /* We have not deallocated the buffer before because
             * first_invalid_escape_ptr points inside buf. */
            PyMem_Free(buf);
            Py_DECREF(v);
            திரும்பு வெற்று;
        }
    }
    PyMem_Free(buf);
    திரும்பு v;
}

நிலையான PyObject *
decode_bytes_with_escapes(Parser *p, மாறிலி எழுத்து *s, Py_ssize_t len, Token *t)
{
    முழு first_invalid_escape_char;
    மாறிலி எழுத்து *first_invalid_escape_ptr;
    PyObject *result = _PyBytes_DecodeEscape2(s, len, வெற்று,
                                              &first_invalid_escape_char,
                                              &first_invalid_escape_ptr);
    எனில் (result == வெற்று) {
        திரும்பு வெற்று;
    }

    எனில் (first_invalid_escape_ptr != வெற்று) {
        எனில் (warn_invalid_escape_sequence(p, s, first_invalid_escape_ptr, t) < 0) {
            Py_DECREF(result);
            திரும்பு வெற்று;
        }
    }
    திரும்பு result;
}

PyObject *
_PyPegen_decode_string(Parser *p, முழு raw, மாறிலி எழுத்து *s, size_t len, Token *t)
{
    எனில் (raw) {
        திரும்பு PyUnicode_DecodeUTF8Stateful(s, (Py_ssize_t)len, வெற்று, வெற்று);
    }
    திரும்பு decode_unicode_with_escapes(p, s, len, t);
}

/* s must include the bracketing quote characters, and r, b &/or f prefixes
    (if any), and embedded escape sequences (if any). (f-strings are handled by the parser)
   _PyPegen_parse_string parses it, and returns the decoded Python string object. */
PyObject *
_PyPegen_parse_string(Parser *p, Token *t)
{
    மாறிலி எழுத்து *s = PyBytes_AsString(t->bytes);
    எனில் (s == வெற்று) {
        திரும்பு வெற்று;
    }

    size_t len;
    முழு quote = Py_CHARMASK(*s);
    முழு bytesmode = 0;
    முழு rawmode = 0;

    எனில் (Py_ISALPHA(quote)) {
        போது (!bytesmode || !rawmode) {
            எனில் (quote == 'b' || quote == 'B') {
                quote =(unsigned எழுத்து)*++s;
                bytesmode = 1;
            }
            இல்லையெனில் எனில் (quote == 'u' || quote == 'U') {
                quote = (unsigned எழுத்து)*++s;
            }
            இல்லையெனில் எனில் (quote == 'r' || quote == 'R') {
                quote = (unsigned எழுத்து)*++s;
                rawmode = 1;
            }
            இல்லையெனில் {
                நிறுத்து;
            }
        }
    }

    எனில் (quote != '\'' && quote != '\"') {
        PyErr_BadInternalCall();
        திரும்பு வெற்று;
    }

    /* Skip the leading quote char. */
    s++;
    len = strlen(s);
    // gh-120155: 's' contains at least the trailing quote,
    // so the code '--len' below is safe.
    ப்ரதிஜ்ஞா(len >= 1);

    எனில் (len > INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "string to parse is too long");
        திரும்பு வெற்று;
    }
    எனில் (s[--len] != quote) {
        /* Last quote char must match the first. */
        PyErr_BadInternalCall();
        திரும்பு வெற்று;
    }
    எனில் (len >= 4 && s[0] == quote && s[1] == quote) {
        /* A triple quoted string. We've already skipped one quote at
           the start and one at the end of the string. Now skip the
           two at the start. */
        s += 2;
        len -= 2;
        /* And check that the last two match. */
        எனில் (s[--len] != quote || s[--len] != quote) {
            PyErr_BadInternalCall();
            திரும்பு வெற்று;
        }
    }

    /* Avoid invoking escape decoding routines if possible. */
    rawmode = rawmode || strchr(s, '\\') == வெற்று;
    எனில் (bytesmode) {
        /* Disallow non-ASCII characters. */
        மாறிலி எழுத்து *ch;
        க்கு (ch = s; *ch; ch++) {
            எனில் (Py_CHARMASK(*ch) >= 0x80) {
                RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
                                   t,
                                   "bytes can only contain ASCII "
                                   "literal characters");
                திரும்பு வெற்று;
            }
        }
        எனில் (rawmode) {
            திரும்பு PyBytes_FromStringAndSize(s, (Py_ssize_t)len);
        }
        திரும்பு decode_bytes_with_escapes(p, s, (Py_ssize_t)len, t);
    }
    திரும்பு _PyPegen_decode_string(p, rawmode, s, len, t);
}
