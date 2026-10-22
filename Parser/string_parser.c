#અન્તર્ભાવ <Python.h>
#અન્તર્ભાવ "pycore_bytesobject.h"   // _PyBytes_DecodeEscape()
#અન્તર્ભાવ "pycore_unicodeobject.h" // _PyUnicode_DecodeUnicodeEscapeInternal()

#અન્તર્ભાવ "pegen.h"
#અન્તર્ભાવ "string_parser.h"

#અન્તર્ભાવ <stdbool.h>

//// STRING HANDLING FUNCTIONS ////

સ્થિર પૂર્ણાંક
warn_invalid_escape_sequence(Parser *p, અચલ અક્ષર* buffer, અચલ અક્ષર *first_invalid_escape, Token *t)
{
    જો (p->call_invalid_rules) {
        // Do not report warnings if we are in the second pass of the parser
        // to avoid showing the warning twice.
        પાછા 0;
    }
    unsigned અક્ષર c = (unsigned અક્ષર)*first_invalid_escape;
    જો ((t->type == FSTRING_મધ્ય || t->type == FSTRING_END || t->type == TSTRING_મધ્ય || t->type == TSTRING_END)
            && (c == '{' || c == '}')) {
        // in this case the tokenizer has already emitted a warning,
        // see Parser/tokenizer/helpers.c:warn_invalid_escape_sequence
        પાછા 0;
    }

    પૂર્ણાંક octal = ('4' <= c && c <= '7');
    PyObject *સંદેશ =
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
    જો (સંદેશ == ખાલી) {
        પાછા -1;
    }
    PyObject *category;
    જો (p->feature_version >= 12) {
        category = PyExc_SyntaxWarning;
    }
    નહીંતર {
        category = PyExc_DeprecationWarning;
    }

    // Calculate the lineno and the col_offset of the invalid escape sequence
    અચલ અક્ષર *start = buffer;
    અચલ અક્ષર *end = first_invalid_escape;
    પૂર્ણાંક lineno = t->lineno;
    પૂર્ણાંક col_offset = t->col_offset;
    જ્યારે (start < end) {
        જો (*start == '\n') {
            lineno++;
            col_offset = 0;
        }
        નહીંતર {
            col_offset++;
        }
        start++;
    }

    // Count the number of quotes in the token
    અક્ષર first_quote = 0;
    જો (lineno == t->lineno) {
        પૂર્ણાંક quote_ગણતરી = 0;
        અચલ અક્ષર* tok = PyBytes_AsString(t->bytes);
        માટે (પૂર્ણાંક i = 0; i < PyBytes_Size(t->bytes); i++) {
            જો (tok[i] == '\'' || tok[i] == '\"') {
                જો (quote_ગણતરી == 0) {
                    first_quote = tok[i];
                }
                જો (tok[i] == first_quote) {
                    quote_ગણતરી++;
                }
            } નહીંતર {
                તોડો;
            }
        }

        col_offset += quote_ગણતરી;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    જો (PyErr_WarnExplicitObject(category, સંદેશ, info.filename,
                                 lineno, info.module, ખાલી) < 0) {
        જો (PyErr_ExceptionMatches(category)) {
            /* Replace the Syntax/DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            /* This is needed, in order for the SyntaxError to point to the token t,
               since _PyPegen_raise_error uses p->tokens[p->fill - 1] for the
               error location, if p->known_err_token is not set. */
            p->known_err_token = t;
            જો (octal) {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%.3s\" is an invalid octal escape sequence. "
                    "Did you mean \"\\\\%.3s\"? A raw string is also an option.",
                    first_invalid_escape, first_invalid_escape);
            }
            નહીંતર {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%c\" is an invalid escape sequence. "
                    "Did you mean \"\\\\%c\"? A raw string is also an option.",
                    c, c);
            }
        }
        Py_DECREF(સંદેશ);
        પાછા -1;
    }
    Py_DECREF(સંદેશ);
    પાછા 0;
}

સ્થિર PyObject *
decode_utf8(અચલ અક્ષર **sPtr, અચલ અક્ષર *end)
{
    અચલ અક્ષર *s;
    અચલ અક્ષર *t;
    t = s = *sPtr;
    જ્યારે (s < end && (*s & 0x80)) {
        s++;
    }
    *sPtr = s;
    પાછા PyUnicode_DecodeUTF8(t, s - t, ખાલી);
}

સ્થિર PyObject *
decode_unicode_with_escapes(Parser *parser, અચલ અક્ષર *s, size_t len, Token *t)
{
    PyObject *v;
    અક્ષર *p;
    અચલ અક્ષર *end;

    /* check for integer overflow */
    જો (len > (size_t)PY_SSIZE_T_MAX / 6) {
        પાછા ખાલી;
    }
    /* "ä" (2 bytes) may become "\U000000E4" (10 bytes), or 1:5.
     * "\ä" (3 bytes) may become "\u005c\U000000E4" (16 bytes), or ~1:6. */
    Py_ssize_t alloc = (Py_ssize_t)len * 6;
    અક્ષર *buf = PyMem_Malloc(alloc);
    જો (buf == ખાલી) {
        પાછા PyErr_NoMemory();
    }
    p = buf;
    end = s + len;
    જ્યારે (s < end) {
        જો (*s == '\\') {
            *p++ = *s++;
            જો (s >= end || *s & 0x80) {
                memcpy(p, "u005c", 5);
                p += 5;
                જો (s >= end) {
                    તોડો;
                }
            }
        }
        જો (*s & 0x80) {
            PyObject *w;
            પૂર્ણાંક kind;
            અચલ શૂન્ય *data;
            Py_ssize_t w_len;
            Py_ssize_t i;
            w = decode_utf8(&s, end);
            જો (w == ખાલી) {
                PyMem_Free(buf);
                પાછા ખાલી;
            }
            kind = PyUnicode_KIND(w);
            data = PyUnicode_DATA(w);
            w_len = PyUnicode_GET_LENGTH(w);
            માટે (i = 0; i < w_len; i++) {
                // sprintf() writes a null byte: the buffer is large enough
                // for that thanks to the overallocation.
                પ્રતિજ્ઞા((p + 11 - buf) <= alloc);
                Py_UCS4 chr = PyUnicode_READ(kind, data, i);
                sprintf(p, "\\U%08x", chr);
                p += 10;
            }
            Py_DECREF(w);
        }
        નહીંતર {
            *p++ = *s++;
        }
    }
    len = (size_t)(p - buf);

    પૂર્ણાંક first_invalid_escape_char;
    અચલ અક્ષર *first_invalid_escape_ptr;
    v = _PyUnicode_DecodeUnicodeEscapeInternal2(buf, (Py_ssize_t)len, ખાલી, ખાલી,
                                                &first_invalid_escape_char,
                                                &first_invalid_escape_ptr);

    // HACK: later we can simply pass the line no, since we don't preserve the tokens
    // when we are decoding the string but we preserve the line numbers.
    જો (v != ખાલી && first_invalid_escape_ptr != ખાલી && t != ખાલી) {
        જો (warn_invalid_escape_sequence(parser, buf, first_invalid_escape_ptr, t) < 0) {
            /* We have not deallocated the buffer before because
             * first_invalid_escape_ptr points inside buf. */
            PyMem_Free(buf);
            Py_DECREF(v);
            પાછા ખાલી;
        }
    }
    PyMem_Free(buf);
    પાછા v;
}

સ્થિર PyObject *
decode_bytes_with_escapes(Parser *p, અચલ અક્ષર *s, Py_ssize_t len, Token *t)
{
    પૂર્ણાંક first_invalid_escape_char;
    અચલ અક્ષર *first_invalid_escape_ptr;
    PyObject *result = _PyBytes_DecodeEscape2(s, len, ખાલી,
                                              &first_invalid_escape_char,
                                              &first_invalid_escape_ptr);
    જો (result == ખાલી) {
        પાછા ખાલી;
    }

    જો (first_invalid_escape_ptr != ખાલી) {
        જો (warn_invalid_escape_sequence(p, s, first_invalid_escape_ptr, t) < 0) {
            Py_DECREF(result);
            પાછા ખાલી;
        }
    }
    પાછા result;
}

PyObject *
_PyPegen_decode_string(Parser *p, પૂર્ણાંક raw, અચલ અક્ષર *s, size_t len, Token *t)
{
    જો (raw) {
        પાછા PyUnicode_DecodeUTF8Stateful(s, (Py_ssize_t)len, ખાલી, ખાલી);
    }
    પાછા decode_unicode_with_escapes(p, s, len, t);
}

/* s must include the bracketing quote characters, and r, b &/or f prefixes
    (if any), and embedded escape sequences (if any). (f-strings are handled by the parser)
   _PyPegen_parse_string parses it, and returns the decoded Python string object. */
PyObject *
_PyPegen_parse_string(Parser *p, Token *t)
{
    અચલ અક્ષર *s = PyBytes_AsString(t->bytes);
    જો (s == ખાલી) {
        પાછા ખાલી;
    }

    size_t len;
    પૂર્ણાંક quote = Py_CHARMASK(*s);
    પૂર્ણાંક bytesmode = 0;
    પૂર્ણાંક rawmode = 0;

    જો (Py_ISALPHA(quote)) {
        જ્યારે (!bytesmode || !rawmode) {
            જો (quote == 'b' || quote == 'B') {
                quote =(unsigned અક્ષર)*++s;
                bytesmode = 1;
            }
            નહીંતર જો (quote == 'u' || quote == 'U') {
                quote = (unsigned અક્ષર)*++s;
            }
            નહીંતર જો (quote == 'r' || quote == 'R') {
                quote = (unsigned અક્ષર)*++s;
                rawmode = 1;
            }
            નહીંતર {
                તોડો;
            }
        }
    }

    જો (quote != '\'' && quote != '\"') {
        PyErr_BadInternalCall();
        પાછા ખાલી;
    }

    /* Skip the leading quote char. */
    s++;
    len = strlen(s);
    // gh-120155: 's' contains at least the trailing quote,
    // so the code '--len' below is safe.
    પ્રતિજ્ઞા(len >= 1);

    જો (len > INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "string to parse is too long");
        પાછા ખાલી;
    }
    જો (s[--len] != quote) {
        /* Last quote char must match the first. */
        PyErr_BadInternalCall();
        પાછા ખાલી;
    }
    જો (len >= 4 && s[0] == quote && s[1] == quote) {
        /* A triple quoted string. We've already skipped one quote at
           the start and one at the end of the string. Now skip the
           two at the start. */
        s += 2;
        len -= 2;
        /* And check that the last two match. */
        જો (s[--len] != quote || s[--len] != quote) {
            PyErr_BadInternalCall();
            પાછા ખાલી;
        }
    }

    /* Avoid invoking escape decoding routines if possible. */
    rawmode = rawmode || strchr(s, '\\') == ખાલી;
    જો (bytesmode) {
        /* Disallow non-ASCII characters. */
        અચલ અક્ષર *ch;
        માટે (ch = s; *ch; ch++) {
            જો (Py_CHARMASK(*ch) >= 0x80) {
                RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
                                   t,
                                   "bytes can only contain ASCII "
                                   "literal characters");
                પાછા ખાલી;
            }
        }
        જો (rawmode) {
            પાછા PyBytes_FromStringAndSize(s, (Py_ssize_t)len);
        }
        પાછા decode_bytes_with_escapes(p, s, (Py_ssize_t)len, t);
    }
    પાછા _PyPegen_decode_string(p, rawmode, s, len, t);
}
