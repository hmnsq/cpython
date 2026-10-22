#അന്തര്ഭാവ <Python.h>
#അന്തര്ഭാവ "pycore_bytesobject.h"   // _PyBytes_DecodeEscape()
#അന്തര്ഭാവ "pycore_unicodeobject.h" // _PyUnicode_DecodeUnicodeEscapeInternal()

#അന്തര്ഭാവ "pegen.h"
#അന്തര്ഭാവ "string_parser.h"

#അന്തര്ഭാവ <stdbool.h>

//// STRING HANDLING FUNCTIONS ////

സ്ഥിതം പൂർണ്ണസംഖ്യ
warn_invalid_escape_sequence(Parser *p, സ്ഥിരം അക്ഷരം* buffer, സ്ഥിരം അക്ഷരം *first_invalid_escape, Token *t)
{
    എങ്കിൽ (p->call_invalid_rules) {
        // Do not report warnings if we are in the second pass of the parser
        // to avoid showing the warning twice.
        തിരികെ 0;
    }
    unsigned അക്ഷരം c = (unsigned അക്ഷരം)*first_invalid_escape;
    എങ്കിൽ ((t->type == FSTRING_നടുവ് || t->type == FSTRING_END || t->type == TSTRING_നടുവ് || t->type == TSTRING_END)
            && (c == '{' || c == '}')) {
        // in this case the tokenizer has already emitted a warning,
        // see Parser/tokenizer/helpers.c:warn_invalid_escape_sequence
        തിരികെ 0;
    }

    പൂർണ്ണസംഖ്യ octal = ('4' <= c && c <= '7');
    PyObject *സന്ദേശം =
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
    എങ്കിൽ (സന്ദേശം == ശൂന്യ) {
        തിരികെ -1;
    }
    PyObject *category;
    എങ്കിൽ (p->feature_version >= 12) {
        category = PyExc_SyntaxWarning;
    }
    അല്ലെങ്കിൽ {
        category = PyExc_DeprecationWarning;
    }

    // Calculate the lineno and the col_offset of the invalid escape sequence
    സ്ഥിരം അക്ഷരം *start = buffer;
    സ്ഥിരം അക്ഷരം *end = first_invalid_escape;
    പൂർണ്ണസംഖ്യ lineno = t->lineno;
    പൂർണ്ണസംഖ്യ col_offset = t->col_offset;
    ആകുമ്പോൾ (start < end) {
        എങ്കിൽ (*start == '\n') {
            lineno++;
            col_offset = 0;
        }
        അല്ലെങ്കിൽ {
            col_offset++;
        }
        start++;
    }

    // Count the number of quotes in the token
    അക്ഷരം first_quote = 0;
    എങ്കിൽ (lineno == t->lineno) {
        പൂർണ്ണസംഖ്യ quote_എണ്ണം = 0;
        സ്ഥിരം അക്ഷരം* tok = PyBytes_AsString(t->bytes);
        വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < PyBytes_Size(t->bytes); i++) {
            എങ്കിൽ (tok[i] == '\'' || tok[i] == '\"') {
                എങ്കിൽ (quote_എണ്ണം == 0) {
                    first_quote = tok[i];
                }
                എങ്കിൽ (tok[i] == first_quote) {
                    quote_എണ്ണം++;
                }
            } അല്ലെങ്കിൽ {
                നിർത്തുക;
            }
        }

        col_offset += quote_എണ്ണം;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    എങ്കിൽ (PyErr_WarnExplicitObject(category, സന്ദേശം, info.filename,
                                 lineno, info.module, ശൂന്യ) < 0) {
        എങ്കിൽ (PyErr_ExceptionMatches(category)) {
            /* Replace the Syntax/DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            /* This is needed, in order for the SyntaxError to point to the token t,
               since _PyPegen_raise_error uses p->tokens[p->fill - 1] for the
               error location, if p->known_err_token is not set. */
            p->known_err_token = t;
            എങ്കിൽ (octal) {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%.3s\" is an invalid octal escape sequence. "
                    "Did you mean \"\\\\%.3s\"? A raw string is also an option.",
                    first_invalid_escape, first_invalid_escape);
            }
            അല്ലെങ്കിൽ {
                RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, lineno, col_offset-1, lineno, col_offset+1,
                    "\"\\%c\" is an invalid escape sequence. "
                    "Did you mean \"\\\\%c\"? A raw string is also an option.",
                    c, c);
            }
        }
        Py_DECREF(സന്ദേശം);
        തിരികെ -1;
    }
    Py_DECREF(സന്ദേശം);
    തിരികെ 0;
}

സ്ഥിതം PyObject *
decode_utf8(സ്ഥിരം അക്ഷരം **sPtr, സ്ഥിരം അക്ഷരം *end)
{
    സ്ഥിരം അക്ഷരം *s;
    സ്ഥിരം അക്ഷരം *t;
    t = s = *sPtr;
    ആകുമ്പോൾ (s < end && (*s & 0x80)) {
        s++;
    }
    *sPtr = s;
    തിരികെ PyUnicode_DecodeUTF8(t, s - t, ശൂന്യ);
}

സ്ഥിതം PyObject *
decode_unicode_with_escapes(Parser *parser, സ്ഥിരം അക്ഷരം *s, size_t len, Token *t)
{
    PyObject *v;
    അക്ഷരം *p;
    സ്ഥിരം അക്ഷരം *end;

    /* check for integer overflow */
    എങ്കിൽ (len > (size_t)PY_SSIZE_T_MAX / 6) {
        തിരികെ ശൂന്യ;
    }
    /* "ä" (2 bytes) may become "\U000000E4" (10 bytes), or 1:5.
     * "\ä" (3 bytes) may become "\u005c\U000000E4" (16 bytes), or ~1:6. */
    Py_ssize_t alloc = (Py_ssize_t)len * 6;
    അക്ഷരം *buf = PyMem_Malloc(alloc);
    എങ്കിൽ (buf == ശൂന്യ) {
        തിരികെ PyErr_NoMemory();
    }
    p = buf;
    end = s + len;
    ആകുമ്പോൾ (s < end) {
        എങ്കിൽ (*s == '\\') {
            *p++ = *s++;
            എങ്കിൽ (s >= end || *s & 0x80) {
                memcpy(p, "u005c", 5);
                p += 5;
                എങ്കിൽ (s >= end) {
                    നിർത്തുക;
                }
            }
        }
        എങ്കിൽ (*s & 0x80) {
            PyObject *w;
            പൂർണ്ണസംഖ്യ kind;
            സ്ഥിരം ശൂന്യം *data;
            Py_ssize_t w_len;
            Py_ssize_t i;
            w = decode_utf8(&s, end);
            എങ്കിൽ (w == ശൂന്യ) {
                PyMem_Free(buf);
                തിരികെ ശൂന്യ;
            }
            kind = PyUnicode_KIND(w);
            data = PyUnicode_DATA(w);
            w_len = PyUnicode_GET_LENGTH(w);
            വേണ്ടി (i = 0; i < w_len; i++) {
                // sprintf() writes a null byte: the buffer is large enough
                // for that thanks to the overallocation.
                പ്രതിജ്ഞാ((p + 11 - buf) <= alloc);
                Py_UCS4 chr = PyUnicode_READ(kind, data, i);
                sprintf(p, "\\U%08x", chr);
                p += 10;
            }
            Py_DECREF(w);
        }
        അല്ലെങ്കിൽ {
            *p++ = *s++;
        }
    }
    len = (size_t)(p - buf);

    പൂർണ്ണസംഖ്യ first_invalid_escape_char;
    സ്ഥിരം അക്ഷരം *first_invalid_escape_ptr;
    v = _PyUnicode_DecodeUnicodeEscapeInternal2(buf, (Py_ssize_t)len, ശൂന്യ, ശൂന്യ,
                                                &first_invalid_escape_char,
                                                &first_invalid_escape_ptr);

    // HACK: later we can simply pass the line no, since we don't preserve the tokens
    // when we are decoding the string but we preserve the line numbers.
    എങ്കിൽ (v != ശൂന്യ && first_invalid_escape_ptr != ശൂന്യ && t != ശൂന്യ) {
        എങ്കിൽ (warn_invalid_escape_sequence(parser, buf, first_invalid_escape_ptr, t) < 0) {
            /* We have not deallocated the buffer before because
             * first_invalid_escape_ptr points inside buf. */
            PyMem_Free(buf);
            Py_DECREF(v);
            തിരികെ ശൂന്യ;
        }
    }
    PyMem_Free(buf);
    തിരികെ v;
}

സ്ഥിതം PyObject *
decode_bytes_with_escapes(Parser *p, സ്ഥിരം അക്ഷരം *s, Py_ssize_t len, Token *t)
{
    പൂർണ്ണസംഖ്യ first_invalid_escape_char;
    സ്ഥിരം അക്ഷരം *first_invalid_escape_ptr;
    PyObject *result = _PyBytes_DecodeEscape2(s, len, ശൂന്യ,
                                              &first_invalid_escape_char,
                                              &first_invalid_escape_ptr);
    എങ്കിൽ (result == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (first_invalid_escape_ptr != ശൂന്യ) {
        എങ്കിൽ (warn_invalid_escape_sequence(p, s, first_invalid_escape_ptr, t) < 0) {
            Py_DECREF(result);
            തിരികെ ശൂന്യ;
        }
    }
    തിരികെ result;
}

PyObject *
_PyPegen_decode_string(Parser *p, പൂർണ്ണസംഖ്യ raw, സ്ഥിരം അക്ഷരം *s, size_t len, Token *t)
{
    എങ്കിൽ (raw) {
        തിരികെ PyUnicode_DecodeUTF8Stateful(s, (Py_ssize_t)len, ശൂന്യ, ശൂന്യ);
    }
    തിരികെ decode_unicode_with_escapes(p, s, len, t);
}

/* s must include the bracketing quote characters, and r, b &/or f prefixes
    (if any), and embedded escape sequences (if any). (f-strings are handled by the parser)
   _PyPegen_parse_string parses it, and returns the decoded Python string object. */
PyObject *
_PyPegen_parse_string(Parser *p, Token *t)
{
    സ്ഥിരം അക്ഷരം *s = PyBytes_AsString(t->bytes);
    എങ്കിൽ (s == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }

    size_t len;
    പൂർണ്ണസംഖ്യ quote = Py_CHARMASK(*s);
    പൂർണ്ണസംഖ്യ bytesmode = 0;
    പൂർണ്ണസംഖ്യ rawmode = 0;

    എങ്കിൽ (Py_ISALPHA(quote)) {
        ആകുമ്പോൾ (!bytesmode || !rawmode) {
            എങ്കിൽ (quote == 'b' || quote == 'B') {
                quote =(unsigned അക്ഷരം)*++s;
                bytesmode = 1;
            }
            അല്ലെങ്കിൽ എങ്കിൽ (quote == 'u' || quote == 'U') {
                quote = (unsigned അക്ഷരം)*++s;
            }
            അല്ലെങ്കിൽ എങ്കിൽ (quote == 'r' || quote == 'R') {
                quote = (unsigned അക്ഷരം)*++s;
                rawmode = 1;
            }
            അല്ലെങ്കിൽ {
                നിർത്തുക;
            }
        }
    }

    എങ്കിൽ (quote != '\'' && quote != '\"') {
        PyErr_BadInternalCall();
        തിരികെ ശൂന്യ;
    }

    /* Skip the leading quote char. */
    s++;
    len = strlen(s);
    // gh-120155: 's' contains at least the trailing quote,
    // so the code '--len' below is safe.
    പ്രതിജ്ഞാ(len >= 1);

    എങ്കിൽ (len > INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "string to parse is too long");
        തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (s[--len] != quote) {
        /* Last quote char must match the first. */
        PyErr_BadInternalCall();
        തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (len >= 4 && s[0] == quote && s[1] == quote) {
        /* A triple quoted string. We've already skipped one quote at
           the start and one at the end of the string. Now skip the
           two at the start. */
        s += 2;
        len -= 2;
        /* And check that the last two match. */
        എങ്കിൽ (s[--len] != quote || s[--len] != quote) {
            PyErr_BadInternalCall();
            തിരികെ ശൂന്യ;
        }
    }

    /* Avoid invoking escape decoding routines if possible. */
    rawmode = rawmode || strchr(s, '\\') == ശൂന്യ;
    എങ്കിൽ (bytesmode) {
        /* Disallow non-ASCII characters. */
        സ്ഥിരം അക്ഷരം *ch;
        വേണ്ടി (ch = s; *ch; ch++) {
            എങ്കിൽ (Py_CHARMASK(*ch) >= 0x80) {
                RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
                                   t,
                                   "bytes can only contain ASCII "
                                   "literal characters");
                തിരികെ ശൂന്യ;
            }
        }
        എങ്കിൽ (rawmode) {
            തിരികെ PyBytes_FromStringAndSize(s, (Py_ssize_t)len);
        }
        തിരികെ decode_bytes_with_escapes(p, s, (Py_ssize_t)len, t);
    }
    തിരികെ _PyPegen_decode_string(p, rawmode, s, len, t);
}
