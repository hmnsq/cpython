#अन्तर्भाव <Python.h>
#अन्तर्भाव "pycore_bytesobject.h"   // _PyBytes_DecodeEscape()
#अन्तर्भाव "pycore_unicodeobject.h" // _PyUnicode_DecodeUnicodeEscapeInternal()

#अन्तर्भाव "pegen.h"
#अन्तर्भाव "string_parser.h"

#अन्तर्भाव <stdbool.h>

//// STRING HANDLING FUNCTIONS ////

स्थिर पूर्णाङ्क
warn_invalid_escape_sequence(Parser *p, ध्रुव वर्ण* buffer, ध्रुव वर्ण *first_invalid_escape, Token *t)
{
    यदि (p->call_invalid_rules) {
        // Do not report warnings if we are in the second pass of the parser
        // to avoid showing the warning twice.
        प्रत्यागम 0;
    }
    unsigned वर्ण c = (unsigned वर्ण)*first_invalid_escape;
    यदि ((t->type == FSTRING_मधला || t->type == FSTRING_END || t->type == TSTRING_मधला || t->type == TSTRING_END)
            && (c == '{' || c == '}')) {
        // in this case the tokenizer has already emitted a warning,
        // see Parser/tokenizer/helpers.c:warn_invalid_escape_sequence
        प्रत्यागम 0;
    }

    पूर्णाङ्क octal = ('4' <= c && c <= '7');
    PyObject *संदेश =
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
    यदि (संदेश == रिक्त) {
        प्रत्यागम -1;
    }
    PyObject *category;
    यदि (p->feature_version >= 12) {
        category = PyExc_SyntaxWarning;
    }
    अन्यथा {
        category = PyExc_DeprecationWarning;
    }

    // Calculate the lineno and the col_offset of the invalid escape sequence
    ध्रुव वर्ण *start = buffer;
    ध्रुव वर्ण *end = first_invalid_escape;
    पूर्णाङ्क lineno = t->lineno;
    पूर्णाङ्क col_offset = t->col_offset;
    यावत् (start < end) {
        यदि (*start == '\n') {
            lineno++;
            col_offset = 0;
        }
        अन्यथा {
            col_offset++;
        }
        start++;
    }

    // Count the number of quotes in the token
    वर्ण first_quote = 0;
    यदि (lineno == t->lineno) {
        पूर्णाङ्क quote_मोजणी = 0;
        ध्रुव वर्ण* tok = PyBytes_AsString(t->bytes);
        प्रति (पूर्णाङ्क i = 0; i < PyBytes_Size(t->bytes); i++) {
            यदि (tok[i] == '\'' || tok[i] == '\"') {
                यदि (quote_मोजणी == 0) {
                    first_quote = tok[i];
                }
                यदि (tok[i] == first_quote) {
                    quote_मोजणी++;
                }
            } अन्यथा {
                भङ्ग;
            }
        }

        col_offset += quote_मोजणी;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    यदि (PyErr_WarnExplicitObject(category, संदेश, info.filename,
                                 lineno, info.module, रिक्त) < 0) {
        यदि (PyErr_ExceptionMatches(category)) {
            /* Replace the Syntax/DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            /* This is needed, in order for the SyntaxError to point to the token t,
               since _PyPegen_raise_error uses p->tokens[p->fill - 1] for the
               error location, if p->known_err_token is not set. */
            p->known_err_token = t;
            यदि (octal) {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%.3s\" is an invalid octal escape sequence. "
                    "Did you mean \"\\\\%.3s\"? A raw string is also an option.",
                    first_invalid_escape, first_invalid_escape);
            }
            अन्यथा {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%c\" is an invalid escape sequence. "
                    "Did you mean \"\\\\%c\"? A raw string is also an option.",
                    c, c);
            }
        }
        Py_DECREF(संदेश);
        प्रत्यागम -1;
    }
    Py_DECREF(संदेश);
    प्रत्यागम 0;
}

स्थिर PyObject *
decode_utf8(ध्रुव वर्ण **sPtr, ध्रुव वर्ण *end)
{
    ध्रुव वर्ण *s;
    ध्रुव वर्ण *t;
    t = s = *sPtr;
    यावत् (s < end && (*s & 0x80)) {
        s++;
    }
    *sPtr = s;
    प्रत्यागम PyUnicode_DecodeUTF8(t, s - t, रिक्त);
}

स्थिर PyObject *
decode_unicode_with_escapes(Parser *parser, ध्रुव वर्ण *s, size_t len, Token *t)
{
    PyObject *v;
    वर्ण *p;
    ध्रुव वर्ण *end;

    /* check for integer overflow */
    यदि (len > (size_t)PY_SSIZE_T_MAX / 6) {
        प्रत्यागम रिक्त;
    }
    /* "ä" (2 bytes) may become "\U000000E4" (10 bytes), or 1:5.
     * "\ä" (3 bytes) may become "\u005c\U000000E4" (16 bytes), or ~1:6. */
    Py_ssize_t alloc = (Py_ssize_t)len * 6;
    वर्ण *buf = PyMem_Malloc(alloc);
    यदि (buf == रिक्त) {
        प्रत्यागम PyErr_NoMemory();
    }
    p = buf;
    end = s + len;
    यावत् (s < end) {
        यदि (*s == '\\') {
            *p++ = *s++;
            यदि (s >= end || *s & 0x80) {
                memcpy(p, "u005c", 5);
                p += 5;
                यदि (s >= end) {
                    भङ्ग;
                }
            }
        }
        यदि (*s & 0x80) {
            PyObject *w;
            पूर्णाङ्क kind;
            ध्रुव शून्य *data;
            Py_ssize_t w_len;
            Py_ssize_t i;
            w = decode_utf8(&s, end);
            यदि (w == रिक्त) {
                PyMem_Free(buf);
                प्रत्यागम रिक्त;
            }
            kind = PyUnicode_KIND(w);
            data = PyUnicode_DATA(w);
            w_len = PyUnicode_GET_LENGTH(w);
            प्रति (i = 0; i < w_len; i++) {
                // sprintf() writes a null byte: the buffer is large enough
                // for that thanks to the overallocation.
                प्रतिज्ञा((p + 11 - buf) <= alloc);
                Py_UCS4 chr = PyUnicode_READ(kind, data, i);
                sprintf(p, "\\U%08x", chr);
                p += 10;
            }
            Py_DECREF(w);
        }
        अन्यथा {
            *p++ = *s++;
        }
    }
    len = (size_t)(p - buf);

    पूर्णाङ्क first_invalid_escape_char;
    ध्रुव वर्ण *first_invalid_escape_ptr;
    v = _PyUnicode_DecodeUnicodeEscapeInternal2(buf, (Py_ssize_t)len, रिक्त, रिक्त,
                                                &first_invalid_escape_char,
                                                &first_invalid_escape_ptr);

    // HACK: later we can simply pass the line no, since we don't preserve the tokens
    // when we are decoding the string but we preserve the line numbers.
    यदि (v != रिक्त && first_invalid_escape_ptr != रिक्त && t != रिक्त) {
        यदि (warn_invalid_escape_sequence(parser, buf, first_invalid_escape_ptr, t) < 0) {
            /* We have not deallocated the buffer before because
             * first_invalid_escape_ptr points inside buf. */
            PyMem_Free(buf);
            Py_DECREF(v);
            प्रत्यागम रिक्त;
        }
    }
    PyMem_Free(buf);
    प्रत्यागम v;
}

स्थिर PyObject *
decode_bytes_with_escapes(Parser *p, ध्रुव वर्ण *s, Py_ssize_t len, Token *t)
{
    पूर्णाङ्क first_invalid_escape_char;
    ध्रुव वर्ण *first_invalid_escape_ptr;
    PyObject *result = _PyBytes_DecodeEscape2(s, len, रिक्त,
                                              &first_invalid_escape_char,
                                              &first_invalid_escape_ptr);
    यदि (result == रिक्त) {
        प्रत्यागम रिक्त;
    }

    यदि (first_invalid_escape_ptr != रिक्त) {
        यदि (warn_invalid_escape_sequence(p, s, first_invalid_escape_ptr, t) < 0) {
            Py_DECREF(result);
            प्रत्यागम रिक्त;
        }
    }
    प्रत्यागम result;
}

PyObject *
_PyPegen_decode_string(Parser *p, पूर्णाङ्क raw, ध्रुव वर्ण *s, size_t len, Token *t)
{
    यदि (raw) {
        प्रत्यागम PyUnicode_DecodeUTF8Stateful(s, (Py_ssize_t)len, रिक्त, रिक्त);
    }
    प्रत्यागम decode_unicode_with_escapes(p, s, len, t);
}

/* s must include the bracketing quote characters, and r, b &/or f prefixes
    (if any), and embedded escape sequences (if any). (f-strings are handled by the parser)
   _PyPegen_parse_string parses it, and returns the decoded Python string object. */
PyObject *
_PyPegen_parse_string(Parser *p, Token *t)
{
    ध्रुव वर्ण *s = PyBytes_AsString(t->bytes);
    यदि (s == रिक्त) {
        प्रत्यागम रिक्त;
    }

    size_t len;
    पूर्णाङ्क quote = Py_CHARMASK(*s);
    पूर्णाङ्क bytesmode = 0;
    पूर्णाङ्क rawmode = 0;

    यदि (Py_ISALPHA(quote)) {
        यावत् (!bytesmode || !rawmode) {
            यदि (quote == 'b' || quote == 'B') {
                quote =(unsigned वर्ण)*++s;
                bytesmode = 1;
            }
            अन्यथा यदि (quote == 'u' || quote == 'U') {
                quote = (unsigned वर्ण)*++s;
            }
            अन्यथा यदि (quote == 'r' || quote == 'R') {
                quote = (unsigned वर्ण)*++s;
                rawmode = 1;
            }
            अन्यथा {
                भङ्ग;
            }
        }
    }

    यदि (quote != '\'' && quote != '\"') {
        PyErr_BadInternalCall();
        प्रत्यागम रिक्त;
    }

    /* Skip the leading quote char. */
    s++;
    len = strlen(s);
    // gh-120155: 's' contains at least the trailing quote,
    // so the code '--len' below is safe.
    प्रतिज्ञा(len >= 1);

    यदि (len > INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "string to parse is too long");
        प्रत्यागम रिक्त;
    }
    यदि (s[--len] != quote) {
        /* Last quote char must match the first. */
        PyErr_BadInternalCall();
        प्रत्यागम रिक्त;
    }
    यदि (len >= 4 && s[0] == quote && s[1] == quote) {
        /* A triple quoted string. We've already skipped one quote at
           the start and one at the end of the string. Now skip the
           two at the start. */
        s += 2;
        len -= 2;
        /* And check that the last two match. */
        यदि (s[--len] != quote || s[--len] != quote) {
            PyErr_BadInternalCall();
            प्रत्यागम रिक्त;
        }
    }

    /* Avoid invoking escape decoding routines if possible. */
    rawmode = rawmode || strchr(s, '\\') == रिक्त;
    यदि (bytesmode) {
        /* Disallow non-ASCII characters. */
        ध्रुव वर्ण *ch;
        प्रति (ch = s; *ch; ch++) {
            यदि (Py_CHARMASK(*ch) >= 0x80) {
                RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
                                   t,
                                   "bytes can only contain ASCII "
                                   "literal characters");
                प्रत्यागम रिक्त;
            }
        }
        यदि (rawmode) {
            प्रत्यागम PyBytes_FromStringAndSize(s, (Py_ssize_t)len);
        }
        प्रत्यागम decode_bytes_with_escapes(p, s, (Py_ssize_t)len, t);
    }
    प्रत्यागम _PyPegen_decode_string(p, rawmode, s, len, t);
}
