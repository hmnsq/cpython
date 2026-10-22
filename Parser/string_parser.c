#अन्तर्भाव <Python.h>
#अन्तर्भाव "pycore_bytesobject.h"   // _PyBytes_DecodeEscape()
#अन्तर्भाव "pycore_unicodeobject.h" // _PyUnicode_DecodeUnicodeEscapeInternal()

#अन्तर्भाव "pegen.h"
#अन्तर्भाव "string_parser.h"

#अन्तर्भाव <stdbool.h>

//// STRING HANDLING FUNCTIONS ////

स्थिर पूर्णांक
warn_invalid_escape_sequence(Parser *p, स्थिरांक अक्षर* buffer, स्थिरांक अक्षर *first_invalid_escape, Token *t)
{
    जर (p->call_invalid_rules) {
        // Do not report warnings if we are in the second pass of the parser
        // to avoid showing the warning twice.
        परत 0;
    }
    अचिन्हित अक्षर c = (अचिन्हित अक्षर)*first_invalid_escape;
    जर ((t->type == FSTRING_मधला || t->type == FSTRING_END || t->type == TSTRING_मधला || t->type == TSTRING_END)
            && (c == '{' || c == '}')) {
        // in this case the tokenizer has already emitted a warning,
        // see Parser/tokenizer/helpers.c:warn_invalid_escape_sequence
        परत 0;
    }

    पूर्णांक octal = ('4' <= c && c <= '7');
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
    जर (संदेश == रिक्त) {
        परत -1;
    }
    PyObject *category;
    जर (p->feature_version >= 12) {
        category = PyExc_SyntaxWarning;
    }
    अन्यथा {
        category = PyExc_DeprecationWarning;
    }

    // Calculate the lineno and the col_offset of the invalid escape sequence
    स्थिरांक अक्षर *start = buffer;
    स्थिरांक अक्षर *end = first_invalid_escape;
    पूर्णांक lineno = t->lineno;
    पूर्णांक col_offset = t->col_offset;
    पर्यंत (start < end) {
        जर (*start == '\n') {
            lineno++;
            col_offset = 0;
        }
        अन्यथा {
            col_offset++;
        }
        start++;
    }

    // Count the number of quotes in the token
    अक्षर first_quote = 0;
    जर (lineno == t->lineno) {
        पूर्णांक quote_मोजणी = 0;
        स्थिरांक अक्षर* tok = PyBytes_AsString(t->bytes);
        साठी (पूर्णांक i = 0; i < PyBytes_Size(t->bytes); i++) {
            जर (tok[i] == '\'' || tok[i] == '\"') {
                जर (quote_मोजणी == 0) {
                    first_quote = tok[i];
                }
                जर (tok[i] == first_quote) {
                    quote_मोजणी++;
                }
            } अन्यथा {
                थांबा;
            }
        }

        col_offset += quote_मोजणी;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    जर (PyErr_WarnExplicitObject(category, संदेश, info.filename,
                                 lineno, info.module, रिक्त) < 0) {
        जर (PyErr_ExceptionMatches(category)) {
            /* Replace the Syntax/DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            /* This is needed, in order for the SyntaxError to point to the token t,
               since _PyPegen_raise_error uses p->tokens[p->fill - 1] for the
               error location, if p->known_err_token is not set. */
            p->known_err_token = t;
            जर (octal) {
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
        परत -1;
    }
    Py_DECREF(संदेश);
    परत 0;
}

स्थिर PyObject *
decode_utf8(स्थिरांक अक्षर **sPtr, स्थिरांक अक्षर *end)
{
    स्थिरांक अक्षर *s;
    स्थिरांक अक्षर *t;
    t = s = *sPtr;
    पर्यंत (s < end && (*s & 0x80)) {
        s++;
    }
    *sPtr = s;
    परत PyUnicode_DecodeUTF8(t, s - t, रिक्त);
}

स्थिर PyObject *
decode_unicode_with_escapes(Parser *parser, स्थिरांक अक्षर *s, माप_प्रकार len, Token *t)
{
    PyObject *v;
    अक्षर *p;
    स्थिरांक अक्षर *end;

    /* check for integer overflow */
    जर (len > (माप_प्रकार)PY_SSIZE_T_MAX / 6) {
        परत रिक्त;
    }
    /* "ä" (2 bytes) may become "\U000000E4" (10 bytes), or 1:5.
     * "\ä" (3 bytes) may become "\u005c\U000000E4" (16 bytes), or ~1:6. */
    Py_ssize_t alloc = (Py_ssize_t)len * 6;
    अक्षर *buf = PyMem_Malloc(alloc);
    जर (buf == रिक्त) {
        परत PyErr_NoMemory();
    }
    p = buf;
    end = s + len;
    पर्यंत (s < end) {
        जर (*s == '\\') {
            *p++ = *s++;
            जर (s >= end || *s & 0x80) {
                स_नकल(p, "u005c", 5);
                p += 5;
                जर (s >= end) {
                    थांबा;
                }
            }
        }
        जर (*s & 0x80) {
            PyObject *w;
            पूर्णांक kind;
            स्थिरांक शून्य *data;
            Py_ssize_t w_len;
            Py_ssize_t i;
            w = decode_utf8(&s, end);
            जर (w == रिक्त) {
                PyMem_Free(buf);
                परत रिक्त;
            }
            kind = PyUnicode_KIND(w);
            data = PyUnicode_DATA(w);
            w_len = PyUnicode_GET_LENGTH(w);
            साठी (i = 0; i < w_len; i++) {
                // sprintf() writes a null byte: the buffer is large enough
                // for that thanks to the overallocation.
                प्रतिज्ञा((p + 11 - buf) <= alloc);
                Py_UCS4 chr = PyUnicode_READ(kind, data, i);
                प्र_लिखो(p, "\\U%08x", chr);
                p += 10;
            }
            Py_DECREF(w);
        }
        अन्यथा {
            *p++ = *s++;
        }
    }
    len = (माप_प्रकार)(p - buf);

    पूर्णांक first_invalid_escape_char;
    स्थिरांक अक्षर *first_invalid_escape_ptr;
    v = _PyUnicode_DecodeUnicodeEscapeInternal2(buf, (Py_ssize_t)len, रिक्त, रिक्त,
                                                &first_invalid_escape_char,
                                                &first_invalid_escape_ptr);

    // HACK: later we can simply pass the line no, since we don't preserve the tokens
    // when we are decoding the string but we preserve the line numbers.
    जर (v != रिक्त && first_invalid_escape_ptr != रिक्त && t != रिक्त) {
        जर (warn_invalid_escape_sequence(parser, buf, first_invalid_escape_ptr, t) < 0) {
            /* We have not deallocated the buffer before because
             * first_invalid_escape_ptr points inside buf. */
            PyMem_Free(buf);
            Py_DECREF(v);
            परत रिक्त;
        }
    }
    PyMem_Free(buf);
    परत v;
}

स्थिर PyObject *
decode_bytes_with_escapes(Parser *p, स्थिरांक अक्षर *s, Py_ssize_t len, Token *t)
{
    पूर्णांक first_invalid_escape_char;
    स्थिरांक अक्षर *first_invalid_escape_ptr;
    PyObject *result = _PyBytes_DecodeEscape2(s, len, रिक्त,
                                              &first_invalid_escape_char,
                                              &first_invalid_escape_ptr);
    जर (result == रिक्त) {
        परत रिक्त;
    }

    जर (first_invalid_escape_ptr != रिक्त) {
        जर (warn_invalid_escape_sequence(p, s, first_invalid_escape_ptr, t) < 0) {
            Py_DECREF(result);
            परत रिक्त;
        }
    }
    परत result;
}

PyObject *
_PyPegen_decode_string(Parser *p, पूर्णांक raw, स्थिरांक अक्षर *s, माप_प्रकार len, Token *t)
{
    जर (raw) {
        परत PyUnicode_DecodeUTF8Stateful(s, (Py_ssize_t)len, रिक्त, रिक्त);
    }
    परत decode_unicode_with_escapes(p, s, len, t);
}

/* s must include the bracketing quote characters, and r, b &/or f prefixes
    (if any), and embedded escape sequences (if any). (f-strings are handled by the parser)
   _PyPegen_parse_string parses it, and returns the decoded Python string object. */
PyObject *
_PyPegen_parse_string(Parser *p, Token *t)
{
    स्थिरांक अक्षर *s = PyBytes_AsString(t->bytes);
    जर (s == रिक्त) {
        परत रिक्त;
    }

    माप_प्रकार len;
    पूर्णांक quote = Py_CHARMASK(*s);
    पूर्णांक bytesmode = 0;
    पूर्णांक rawmode = 0;

    जर (Py_ISALPHA(quote)) {
        पर्यंत (!bytesmode || !rawmode) {
            जर (quote == 'b' || quote == 'B') {
                quote =(अचिन्हित अक्षर)*++s;
                bytesmode = 1;
            }
            अन्यथा जर (quote == 'u' || quote == 'U') {
                quote = (अचिन्हित अक्षर)*++s;
            }
            अन्यथा जर (quote == 'r' || quote == 'R') {
                quote = (अचिन्हित अक्षर)*++s;
                rawmode = 1;
            }
            अन्यथा {
                थांबा;
            }
        }
    }

    जर (quote != '\'' && quote != '\"') {
        PyErr_BadInternalCall();
        परत रिक्त;
    }

    /* Skip the leading quote char. */
    s++;
    len = म_माप(s);
    // gh-120155: 's' contains at least the trailing quote,
    // so the code '--len' below is safe.
    प्रतिज्ञा(len >= 1);

    जर (len > पूर्णांक_उच्च) {
        PyErr_SetString(PyExc_OverflowError, "string to parse is too long");
        परत रिक्त;
    }
    जर (s[--len] != quote) {
        /* Last quote char must match the first. */
        PyErr_BadInternalCall();
        परत रिक्त;
    }
    जर (len >= 4 && s[0] == quote && s[1] == quote) {
        /* A triple quoted string. We've already skipped one quote at
           the start and one at the end of the string. Now skip the
           two at the start. */
        s += 2;
        len -= 2;
        /* And check that the last two match. */
        जर (s[--len] != quote || s[--len] != quote) {
            PyErr_BadInternalCall();
            परत रिक्त;
        }
    }

    /* Avoid invoking escape decoding routines if possible. */
    rawmode = rawmode || म_अक्षर(s, '\\') == रिक्त;
    जर (bytesmode) {
        /* Disallow non-ASCII characters. */
        स्थिरांक अक्षर *ch;
        साठी (ch = s; *ch; ch++) {
            जर (Py_CHARMASK(*ch) >= 0x80) {
                RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
                                   t,
                                   "bytes can only contain ASCII "
                                   "literal characters");
                परत रिक्त;
            }
        }
        जर (rawmode) {
            परत PyBytes_FromStringAndSize(s, (Py_ssize_t)len);
        }
        परत decode_bytes_with_escapes(p, s, (Py_ssize_t)len, t);
    }
    परत _PyPegen_decode_string(p, rawmode, s, len, t);
}
