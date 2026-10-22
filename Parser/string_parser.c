#ଅନ୍ତର୍ଭାଵ <Python.h>
#ଅନ୍ତର୍ଭାଵ "pycore_bytesobject.h"   // _PyBytes_DecodeEscape()
#ଅନ୍ତର୍ଭାଵ "pycore_unicodeobject.h" // _PyUnicode_DecodeUnicodeEscapeInternal()

#ଅନ୍ତର୍ଭାଵ "pegen.h"
#ଅନ୍ତର୍ଭାଵ "string_parser.h"

#ଅନ୍ତର୍ଭାଵ <stdbool.h>

//// STRING HANDLING FUNCTIONS ////

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
warn_invalid_escape_sequence(Parser *p, ଅଚଳ ଅକ୍ଷର* buffer, ଅଚଳ ଅକ୍ଷର *first_invalid_escape, Token *t)
{
    ଯଦି (p->call_invalid_rules) {
        // Do not report warnings if we are in the second pass of the parser
        // to avoid showing the warning twice.
        ଫେରନ୍ତୁ 0;
    }
    unsigned ଅକ୍ଷର c = (unsigned ଅକ୍ଷର)*first_invalid_escape;
    ଯଦି ((t->type == FSTRING_ମଧ୍ୟ || t->type == FSTRING_END || t->type == TSTRING_ମଧ୍ୟ || t->type == TSTRING_END)
            && (c == '{' || c == '}')) {
        // in this case the tokenizer has already emitted a warning,
        // see Parser/tokenizer/helpers.c:warn_invalid_escape_sequence
        ଫେରନ୍ତୁ 0;
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା octal = ('4' <= c && c <= '7');
    PyObject *ସନ୍ଦେଶ =
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
    ଯଦି (ସନ୍ଦେଶ == ଖାଲି) {
        ଫେରନ୍ତୁ -1;
    }
    PyObject *category;
    ଯଦି (p->feature_version >= 12) {
        category = PyExc_SyntaxWarning;
    }
    ଅନ୍ୟଥା {
        category = PyExc_DeprecationWarning;
    }

    // Calculate the lineno and the col_offset of the invalid escape sequence
    ଅଚଳ ଅକ୍ଷର *start = buffer;
    ଅଚଳ ଅକ୍ଷର *end = first_invalid_escape;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno = t->lineno;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_offset = t->col_offset;
    ଯେତେବେଳେ (start < end) {
        ଯଦି (*start == '\n') {
            lineno++;
            col_offset = 0;
        }
        ଅନ୍ୟଥା {
            col_offset++;
        }
        start++;
    }

    // Count the number of quotes in the token
    ଅକ୍ଷର first_quote = 0;
    ଯଦି (lineno == t->lineno) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା quote_ଗଣନା = 0;
        ଅଚଳ ଅକ୍ଷର* tok = PyBytes_AsString(t->bytes);
        ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < PyBytes_Size(t->bytes); i++) {
            ଯଦି (tok[i] == '\'' || tok[i] == '\"') {
                ଯଦି (quote_ଗଣନା == 0) {
                    first_quote = tok[i];
                }
                ଯଦି (tok[i] == first_quote) {
                    quote_ଗଣନା++;
                }
            } ଅନ୍ୟଥା {
                ଭାଙ୍ଗ;
            }
        }

        col_offset += quote_ଗଣନା;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    ଯଦି (PyErr_WarnExplicitObject(category, ସନ୍ଦେଶ, info.filename,
                                 lineno, info.module, ଖାଲି) < 0) {
        ଯଦି (PyErr_ExceptionMatches(category)) {
            /* Replace the Syntax/DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            /* This is needed, in order for the SyntaxError to point to the token t,
               since _PyPegen_raise_error uses p->tokens[p->fill - 1] for the
               error location, if p->known_err_token is not set. */
            p->known_err_token = t;
            ଯଦି (octal) {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%.3s\" is an invalid octal escape sequence. "
                    "Did you mean \"\\\\%.3s\"? A raw string is also an option.",
                    first_invalid_escape, first_invalid_escape);
            }
            ଅନ୍ୟଥା {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%c\" is an invalid escape sequence. "
                    "Did you mean \"\\\\%c\"? A raw string is also an option.",
                    c, c);
            }
        }
        Py_DECREF(ସନ୍ଦେଶ);
        ଫେରନ୍ତୁ -1;
    }
    Py_DECREF(ସନ୍ଦେଶ);
    ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର PyObject *
decode_utf8(ଅଚଳ ଅକ୍ଷର **sPtr, ଅଚଳ ଅକ୍ଷର *end)
{
    ଅଚଳ ଅକ୍ଷର *s;
    ଅଚଳ ଅକ୍ଷର *t;
    t = s = *sPtr;
    ଯେତେବେଳେ (s < end && (*s & 0x80)) {
        s++;
    }
    *sPtr = s;
    ଫେରନ୍ତୁ PyUnicode_DecodeUTF8(t, s - t, ଖାଲି);
}

ସ୍ଥିର PyObject *
decode_unicode_with_escapes(Parser *parser, ଅଚଳ ଅକ୍ଷର *s, size_t len, Token *t)
{
    PyObject *v;
    ଅକ୍ଷର *p;
    ଅଚଳ ଅକ୍ଷର *end;

    /* check for integer overflow */
    ଯଦି (len > (size_t)PY_SSIZE_T_MAX / 6) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    /* "ä" (2 bytes) may become "\U000000E4" (10 bytes), or 1:5.
     * "\ä" (3 bytes) may become "\u005c\U000000E4" (16 bytes), or ~1:6. */
    Py_ssize_t alloc = (Py_ssize_t)len * 6;
    ଅକ୍ଷର *buf = PyMem_Malloc(alloc);
    ଯଦି (buf == ଖାଲି) {
        ଫେରନ୍ତୁ PyErr_NoMemory();
    }
    p = buf;
    end = s + len;
    ଯେତେବେଳେ (s < end) {
        ଯଦି (*s == '\\') {
            *p++ = *s++;
            ଯଦି (s >= end || *s & 0x80) {
                memcpy(p, "u005c", 5);
                p += 5;
                ଯଦି (s >= end) {
                    ଭାଙ୍ଗ;
                }
            }
        }
        ଯଦି (*s & 0x80) {
            PyObject *w;
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା kind;
            ଅଚଳ ଶୂନ୍ୟ *data;
            Py_ssize_t w_len;
            Py_ssize_t i;
            w = decode_utf8(&s, end);
            ଯଦି (w == ଖାଲି) {
                PyMem_Free(buf);
                ଫେରନ୍ତୁ ଖାଲି;
            }
            kind = PyUnicode_KIND(w);
            data = PyUnicode_DATA(w);
            w_len = PyUnicode_GET_LENGTH(w);
            ପାଇଁ (i = 0; i < w_len; i++) {
                // sprintf() writes a null byte: the buffer is large enough
                // for that thanks to the overallocation.
                ପ୍ରତିଜ୍ଞା((p + 11 - buf) <= alloc);
                Py_UCS4 chr = PyUnicode_READ(kind, data, i);
                sprintf(p, "\\U%08x", chr);
                p += 10;
            }
            Py_DECREF(w);
        }
        ଅନ୍ୟଥା {
            *p++ = *s++;
        }
    }
    len = (size_t)(p - buf);

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା first_invalid_escape_char;
    ଅଚଳ ଅକ୍ଷର *first_invalid_escape_ptr;
    v = _PyUnicode_DecodeUnicodeEscapeInternal2(buf, (Py_ssize_t)len, ଖାଲି, ଖାଲି,
                                                &first_invalid_escape_char,
                                                &first_invalid_escape_ptr);

    // HACK: later we can simply pass the line no, since we don't preserve the tokens
    // when we are decoding the string but we preserve the line numbers.
    ଯଦି (v != ଖାଲି && first_invalid_escape_ptr != ଖାଲି && t != ଖାଲି) {
        ଯଦି (warn_invalid_escape_sequence(parser, buf, first_invalid_escape_ptr, t) < 0) {
            /* We have not deallocated the buffer before because
             * first_invalid_escape_ptr points inside buf. */
            PyMem_Free(buf);
            Py_DECREF(v);
            ଫେରନ୍ତୁ ଖାଲି;
        }
    }
    PyMem_Free(buf);
    ଫେରନ୍ତୁ v;
}

ସ୍ଥିର PyObject *
decode_bytes_with_escapes(Parser *p, ଅଚଳ ଅକ୍ଷର *s, Py_ssize_t len, Token *t)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା first_invalid_escape_char;
    ଅଚଳ ଅକ୍ଷର *first_invalid_escape_ptr;
    PyObject *result = _PyBytes_DecodeEscape2(s, len, ଖାଲି,
                                              &first_invalid_escape_char,
                                              &first_invalid_escape_ptr);
    ଯଦି (result == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (first_invalid_escape_ptr != ଖାଲି) {
        ଯଦି (warn_invalid_escape_sequence(p, s, first_invalid_escape_ptr, t) < 0) {
            Py_DECREF(result);
            ଫେରନ୍ତୁ ଖାଲି;
        }
    }
    ଫେରନ୍ତୁ result;
}

PyObject *
_PyPegen_decode_string(Parser *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା raw, ଅଚଳ ଅକ୍ଷର *s, size_t len, Token *t)
{
    ଯଦି (raw) {
        ଫେରନ୍ତୁ PyUnicode_DecodeUTF8Stateful(s, (Py_ssize_t)len, ଖାଲି, ଖାଲି);
    }
    ଫେରନ୍ତୁ decode_unicode_with_escapes(p, s, len, t);
}

/* s must include the bracketing quote characters, and r, b &/or f prefixes
    (if any), and embedded escape sequences (if any). (f-strings are handled by the parser)
   _PyPegen_parse_string parses it, and returns the decoded Python string object. */
PyObject *
_PyPegen_parse_string(Parser *p, Token *t)
{
    ଅଚଳ ଅକ୍ଷର *s = PyBytes_AsString(t->bytes);
    ଯଦି (s == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    size_t len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା quote = Py_CHARMASK(*s);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା bytesmode = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rawmode = 0;

    ଯଦି (Py_ISALPHA(quote)) {
        ଯେତେବେଳେ (!bytesmode || !rawmode) {
            ଯଦି (quote == 'b' || quote == 'B') {
                quote =(unsigned ଅକ୍ଷର)*++s;
                bytesmode = 1;
            }
            ଅନ୍ୟଥା ଯଦି (quote == 'u' || quote == 'U') {
                quote = (unsigned ଅକ୍ଷର)*++s;
            }
            ଅନ୍ୟଥା ଯଦି (quote == 'r' || quote == 'R') {
                quote = (unsigned ଅକ୍ଷର)*++s;
                rawmode = 1;
            }
            ଅନ୍ୟଥା {
                ଭାଙ୍ଗ;
            }
        }
    }

    ଯଦି (quote != '\'' && quote != '\"') {
        PyErr_BadInternalCall();
        ଫେରନ୍ତୁ ଖାଲି;
    }

    /* Skip the leading quote char. */
    s++;
    len = strlen(s);
    // gh-120155: 's' contains at least the trailing quote,
    // so the code '--len' below is safe.
    ପ୍ରତିଜ୍ଞା(len >= 1);

    ଯଦି (len > INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "string to parse is too long");
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (s[--len] != quote) {
        /* Last quote char must match the first. */
        PyErr_BadInternalCall();
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (len >= 4 && s[0] == quote && s[1] == quote) {
        /* A triple quoted string. We've already skipped one quote at
           the start and one at the end of the string. Now skip the
           two at the start. */
        s += 2;
        len -= 2;
        /* And check that the last two match. */
        ଯଦି (s[--len] != quote || s[--len] != quote) {
            PyErr_BadInternalCall();
            ଫେରନ୍ତୁ ଖାଲି;
        }
    }

    /* Avoid invoking escape decoding routines if possible. */
    rawmode = rawmode || strchr(s, '\\') == ଖାଲି;
    ଯଦି (bytesmode) {
        /* Disallow non-ASCII characters. */
        ଅଚଳ ଅକ୍ଷର *ch;
        ପାଇଁ (ch = s; *ch; ch++) {
            ଯଦି (Py_CHARMASK(*ch) >= 0x80) {
                RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
                                   t,
                                   "bytes can only contain ASCII "
                                   "literal characters");
                ଫେରନ୍ତୁ ଖାଲି;
            }
        }
        ଯଦି (rawmode) {
            ଫେରନ୍ତୁ PyBytes_FromStringAndSize(s, (Py_ssize_t)len);
        }
        ଫେରନ୍ତୁ decode_bytes_with_escapes(p, s, (Py_ssize_t)len, t);
    }
    ଫେରନ୍ତୁ _PyPegen_decode_string(p, rawmode, s, len, t);
}
