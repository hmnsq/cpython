#ਅਨ੍ਤਰ੍ਭਾਵ <Python.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_bytesobject.h"   // _PyBytes_DecodeEscape()
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_unicodeobject.h" // _PyUnicode_DecodeUnicodeEscapeInternal()

#ਅਨ੍ਤਰ੍ਭਾਵ "pegen.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string_parser.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdbool.h>

//// STRING HANDLING FUNCTIONS ////

ਸਥਿਰ ਪੂਰਨ
warn_invalid_escape_sequence(Parser *p, ਅਚਲ ਅੱਖਰ* ਬਫਰ, ਅਚਲ ਅੱਖਰ *ਪਹਿਲਾ_invalid_escape, Token *t)
{
    ਜੇ (p->call_invalid_ਨਿਯਮਾਂ) {
        // Do not report warnings if we are in the second pass of the parser
        // to avoid showing the warning twice.
        ਵਾਪਸ 0;
    }
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)*ਪਹਿਲਾ_invalid_escape;
    ਜੇ ((t->type == FSTRING_ਵਿਚਕਾਰ || t->type == FSTRING_ਸਿਰਾ || t->type == TSTRING_ਵਿਚਕਾਰ || t->type == TSTRING_ਸਿਰਾ)
            && (c == '{' || c == '}')) {
        // in this case the tokenizer has already emitted a warning,
        // see Parser/tokenizer/helpers.c:warn_invalid_escape_sequence
        ਵਾਪਸ 0;
    }

    ਪੂਰਨ octal = ('4' <= c && c <= '7');
    PyObject *ਸੁਨੇਹਾ =
        octal
        ? PyUnicode_FromFormat(
              "\"\\%.3s\" is an invalid octal escape sequence. "
              "Such sequences will not work in the future. "
              "Did you mean \"\\\\%.3s\"? A raw string is also an option.",
              ਪਹਿਲਾ_invalid_escape, ਪਹਿਲਾ_invalid_escape)
        : PyUnicode_FromFormat(
              "\"\\%c\" is an invalid escape sequence. "
              "Such sequences will not work in the future. "
              "Did you mean \"\\\\%c\"? A raw string is also an option.",
              c, c);
    ਜੇ (ਸੁਨੇਹਾ == ਨਿਰਾ) {
        ਵਾਪਸ -1;
    }
    PyObject *category;
    ਜੇ (p->feature_ਸੰਸਕਰਣ >= 12) {
        category = PyExc_SyntaxWarning;
    }
    ਨਹੀਂਤਾਂ {
        category = PyExc_DeprecationWarning;
    }

    // Calculate the lineno and the col_offset of the invalid escape sequence
    ਅਚਲ ਅੱਖਰ *ਸ਼ੁਰੂ = ਬਫਰ;
    ਅਚਲ ਅੱਖਰ *ਸਿਰਾ = ਪਹਿਲਾ_invalid_escape;
    ਪੂਰਨ lineno = t->lineno;
    ਪੂਰਨ ਖਾਨਾ_ਵਿੱਥ = t->ਖਾਨਾ_ਵਿੱਥ;
    ਜਦੋਂ (ਸ਼ੁਰੂ < ਸਿਰਾ) {
        ਜੇ (*ਸ਼ੁਰੂ == '\n') {
            lineno++;
            ਖਾਨਾ_ਵਿੱਥ = 0;
        }
        ਨਹੀਂਤਾਂ {
            ਖਾਨਾ_ਵਿੱਥ++;
        }
        ਸ਼ੁਰੂ++;
    }

    // Count the number of quotes in the token
    ਅੱਖਰ ਪਹਿਲਾ_quote = 0;
    ਜੇ (lineno == t->lineno) {
        ਪੂਰਨ quote_ਗਿਣਤੀ = 0;
        ਅਚਲ ਅੱਖਰ* tok = Py_ਬਾਈਟਾਂ_As_String(t->ਬਾਈਟਾਂ);
        ਲਈ (ਪੂਰਨ i = 0; i < Py_ਬਾਈਟਾਂ_ਮਾਪ(t->ਬਾਈਟਾਂ); i++) {
            ਜੇ (tok[i] == '\'' || tok[i] == '\"') {
                ਜੇ (quote_ਗਿਣਤੀ == 0) {
                    ਪਹਿਲਾ_quote = tok[i];
                }
                ਜੇ (tok[i] == ਪਹਿਲਾ_quote) {
                    quote_ਗਿਣਤੀ++;
                }
            } ਨਹੀਂਤਾਂ {
                ਰੋਕੋ;
            }
        }

        ਖਾਨਾ_ਵਿੱਥ += quote_ਗਿਣਤੀ;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    ਜੇ (PyErr_WarnExplicitObject(category, ਸੁਨੇਹਾ, info.filename,
                                 lineno, info.module, ਨਿਰਾ) < 0) {
        ਜੇ (PyErr_ExceptionMatches(category)) {
            /* Replace the Syntax/DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            /* This is needed, in order for the SyntaxError to point to the token t,
               since _PyPegen_raise_error uses p->tokens[p->fill - 1] for the
               error location, if p->known_err_token is not set. */
            p->ਗਿਆਤ_err_token = t;
            ਜੇ (octal) {
                RAISE_ERROR_ਗਿਆਤ_LOCATION(p, PyExc_SyntaxError, lineno, ਖਾਨਾ_ਵਿੱਥ-1, lineno, ਖਾਨਾ_ਵਿੱਥ+1,
                    "\"\\%.3s\" is an invalid octal escape sequence. "
                    "Did you mean \"\\\\%.3s\"? A raw string is also an option.",
                    ਪਹਿਲਾ_invalid_escape, ਪਹਿਲਾ_invalid_escape);
            }
            ਨਹੀਂਤਾਂ {
                RAISE_ERROR_ਗਿਆਤ_LOCATION(p, PyExc_SyntaxError, lineno, ਖਾਨਾ_ਵਿੱਥ-1, lineno, ਖਾਨਾ_ਵਿੱਥ+1,
                    "\"\\%c\" is an invalid escape sequence. "
                    "Did you mean \"\\\\%c\"? A raw string is also an option.",
                    c, c);
            }
        }
        Py_DECREF(ਸੁਨੇਹਾ);
        ਵਾਪਸ -1;
    }
    Py_DECREF(ਸੁਨੇਹਾ);
    ਵਾਪਸ 0;
}

ਸਥਿਰ PyObject *
decode_utf8(ਅਚਲ ਅੱਖਰ **sPtr, ਅਚਲ ਅੱਖਰ *ਸਿਰਾ)
{
    ਅਚਲ ਅੱਖਰ *s;
    ਅਚਲ ਅੱਖਰ *t;
    t = s = *sPtr;
    ਜਦੋਂ (s < ਸਿਰਾ && (*s & 0x80)) {
        s++;
    }
    *sPtr = s;
    ਵਾਪਸ PyUnicode_DecodeUTF8(t, s - t, ਨਿਰਾ);
}

ਸਥਿਰ PyObject *
decode_unicode_with_escapes(Parser *parser, ਅਚਲ ਅੱਖਰ *s, ਮਾਪ_t len, Token *t)
{
    PyObject *v;
    ਅੱਖਰ *p;
    ਅਚਲ ਅੱਖਰ *ਸਿਰਾ;

    /* check for integer overflow */
    ਜੇ (len > (ਮਾਪ_t)PY_SSIZE_T_MAX / 6) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    /* "ä" (2 bytes) may become "\U000000E4" (10 bytes), or 1:5.
     * "\ä" (3 bytes) may become "\u005c\U000000E4" (16 bytes), or ~1:6. */
    Py_ssize_t alloc = (Py_ssize_t)len * 6;
    ਅੱਖਰ *buf = PyMem_Malloc(alloc);
    ਜੇ (buf == ਨਿਰਾ) {
        ਵਾਪਸ Py_Err_No_ਸਮ੍ਰਿਤੀ();
    }
    p = buf;
    ਸਿਰਾ = s + len;
    ਜਦੋਂ (s < ਸਿਰਾ) {
        ਜੇ (*s == '\\') {
            *p++ = *s++;
            ਜੇ (s >= ਸਿਰਾ || *s & 0x80) {
                ਸਮ੍ਰਿਤੀ_ਨਕਲ(p, "u005c", 5);
                p += 5;
                ਜੇ (s >= ਸਿਰਾ) {
                    ਰੋਕੋ;
                }
            }
        }
        ਜੇ (*s & 0x80) {
            PyObject *w;
            ਪੂਰਨ ਕਿਸਮ;
            ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ;
            Py_ssize_t w_len;
            Py_ssize_t i;
            w = decode_utf8(&s, ਸਿਰਾ);
            ਜੇ (w == ਨਿਰਾ) {
                PyMem_Free(buf);
                ਵਾਪਸ ਨਿਰਾ;
            }
            ਕਿਸਮ = Py_Unicode_ਕਿਸਮ(w);
            ਅੰਕੜੇ = Py_Unicode_ਅੰਕੜੇ(w);
            w_len = Py_Unicode_GET_ਲੰਮਾਈ(w);
            ਲਈ (i = 0; i < w_len; i++) {
                // sprintf() writes a null byte: the buffer is large enough
                // for that thanks to the overallocation.
                ਪ੍ਰਤਿਜ੍ਞਾ((p + 11 - buf) <= alloc);
                Py_UCS4 chr = PyUnicode_READ(ਕਿਸਮ, ਅੰਕੜੇ, i);
                sprintf(p, "\\U%08x", chr);
                p += 10;
            }
            Py_DECREF(w);
        }
        ਨਹੀਂਤਾਂ {
            *p++ = *s++;
        }
    }
    len = (ਮਾਪ_t)(p - buf);

    ਪੂਰਨ ਪਹਿਲਾ_invalid_escape_char;
    ਅਚਲ ਅੱਖਰ *ਪਹਿਲਾ_invalid_escape_ptr;
    v = _PyUnicode_DecodeUnicodeEscapeInternal2(buf, (Py_ssize_t)len, ਨਿਰਾ, ਨਿਰਾ,
                                                &ਪਹਿਲਾ_invalid_escape_char,
                                                &ਪਹਿਲਾ_invalid_escape_ptr);

    // HACK: later we can simply pass the line no, since we don't preserve the tokens
    // when we are decoding the string but we preserve the line numbers.
    ਜੇ (v != ਨਿਰਾ && ਪਹਿਲਾ_invalid_escape_ptr != ਨਿਰਾ && t != ਨਿਰਾ) {
        ਜੇ (warn_invalid_escape_sequence(parser, buf, ਪਹਿਲਾ_invalid_escape_ptr, t) < 0) {
            /* We have not deallocated the buffer before because
             * first_invalid_escape_ptr points inside buf. */
            PyMem_Free(buf);
            Py_DECREF(v);
            ਵਾਪਸ ਨਿਰਾ;
        }
    }
    PyMem_Free(buf);
    ਵਾਪਸ v;
}

ਸਥਿਰ PyObject *
decode_ਬਾਈਟਾਂ_with_escapes(Parser *p, ਅਚਲ ਅੱਖਰ *s, Py_ssize_t len, Token *t)
{
    ਪੂਰਨ ਪਹਿਲਾ_invalid_escape_char;
    ਅਚਲ ਅੱਖਰ *ਪਹਿਲਾ_invalid_escape_ptr;
    PyObject *ਨਤੀਜਾ = Py_ਬਾਈਟਾਂ_Decode_Escape2(s, len, ਨਿਰਾ,
                                              &ਪਹਿਲਾ_invalid_escape_char,
                                              &ਪਹਿਲਾ_invalid_escape_ptr);
    ਜੇ (ਨਤੀਜਾ == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (ਪਹਿਲਾ_invalid_escape_ptr != ਨਿਰਾ) {
        ਜੇ (warn_invalid_escape_sequence(p, s, ਪਹਿਲਾ_invalid_escape_ptr, t) < 0) {
            Py_DECREF(ਨਤੀਜਾ);
            ਵਾਪਸ ਨਿਰਾ;
        }
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

PyObject *
_PyPegen_decode_string(Parser *p, ਪੂਰਨ raw, ਅਚਲ ਅੱਖਰ *s, ਮਾਪ_t len, Token *t)
{
    ਜੇ (raw) {
        ਵਾਪਸ PyUnicode_DecodeUTF8Stateful(s, (Py_ssize_t)len, ਨਿਰਾ, ਨਿਰਾ);
    }
    ਵਾਪਸ decode_unicode_with_escapes(p, s, len, t);
}

/* s must include the bracketing quote characters, and r, b &/or f prefixes
    (if any), and embedded escape sequences (if any). (f-strings are handled by the parser)
   _PyPegen_parse_string parses it, and returns the decoded Python string object. */
PyObject *
_PyPegen_parse_string(Parser *p, Token *t)
{
    ਅਚਲ ਅੱਖਰ *s = Py_ਬਾਈਟਾਂ_As_String(t->ਬਾਈਟਾਂ);
    ਜੇ (s == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    ਮਾਪ_t len;
    ਪੂਰਨ quote = Py_CHARMASK(*s);
    ਪੂਰਨ bytesmode = 0;
    ਪੂਰਨ rawmode = 0;

    ਜੇ (Py_ISALPHA(quote)) {
        ਜਦੋਂ (!bytesmode || !rawmode) {
            ਜੇ (quote == 'b' || quote == 'B') {
                quote =(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)*++s;
                bytesmode = 1;
            }
            ਨਹੀਂਤਾਂ ਜੇ (quote == 'u' || quote == 'U') {
                quote = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)*++s;
            }
            ਨਹੀਂਤਾਂ ਜੇ (quote == 'r' || quote == 'R') {
                quote = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)*++s;
                rawmode = 1;
            }
            ਨਹੀਂਤਾਂ {
                ਰੋਕੋ;
            }
        }
    }

    ਜੇ (quote != '\'' && quote != '\"') {
        Py_Err_Bad_ਅੰਦਰੂਨੀ_Call();
        ਵਾਪਸ ਨਿਰਾ;
    }

    /* Skip the leading quote char. */
    s++;
    len = ਲੜੀ_ਲੰਬਾਈ(s);
    // gh-120155: 's' contains at least the trailing quote,
    // so the code '--len' below is safe.
    ਪ੍ਰਤਿਜ੍ਞਾ(len >= 1);

    ਜੇ (len > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) {
        Py_Err_ਸੈੱਟ_String(Py_Exc_ਉਛਾਲ_Error, "string to parse is too long");
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (s[--len] != quote) {
        /* Last quote char must match the first. */
        Py_Err_Bad_ਅੰਦਰੂਨੀ_Call();
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (len >= 4 && s[0] == quote && s[1] == quote) {
        /* A triple quoted string. We've already skipped one quote at
           the start and one at the end of the string. Now skip the
           two at the start. */
        s += 2;
        len -= 2;
        /* And check that the last two match. */
        ਜੇ (s[--len] != quote || s[--len] != quote) {
            Py_Err_Bad_ਅੰਦਰੂਨੀ_Call();
            ਵਾਪਸ ਨਿਰਾ;
        }
    }

    /* Avoid invoking escape decoding routines if possible. */
    rawmode = rawmode || strchr(s, '\\') == ਨਿਰਾ;
    ਜੇ (bytesmode) {
        /* Disallow non-ASCII characters. */
        ਅਚਲ ਅੱਖਰ *ch;
        ਲਈ (ch = s; *ch; ch++) {
            ਜੇ (Py_CHARMASK(*ch) >= 0x80) {
                RAISE_SYNTAX_ERROR_ਗਿਆਤ_LOCATION(
                                   t,
                                   "bytes can only contain ASCII "
                                   "literal characters");
                ਵਾਪਸ ਨਿਰਾ;
            }
        }
        ਜੇ (rawmode) {
            ਵਾਪਸ Py_ਬਾਈਟਾਂ_From_String_And_ਮਾਪ(s, (Py_ssize_t)len);
        }
        ਵਾਪਸ decode_ਬਾਈਟਾਂ_with_escapes(p, s, (Py_ssize_t)len, t);
    }
    ਵਾਪਸ _PyPegen_decode_string(p, rawmode, s, len, t);
}
