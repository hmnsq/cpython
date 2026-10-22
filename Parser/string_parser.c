#ಅನ್ತರ್ಭಾವ <Python.h>
#ಅನ್ತರ್ಭಾವ "pycore_bytesobject.h"   // _PyBytes_DecodeEscape()
#ಅನ್ತರ್ಭಾವ "pycore_unicodeobject.h" // _PyUnicode_DecodeUnicodeEscapeInternal()

#ಅನ್ತರ್ಭಾವ "pegen.h"
#ಅನ್ತರ್ಭಾವ "string_parser.h"

#ಅನ್ತರ್ಭಾವ <stdbool.h>

//// STRING HANDLING FUNCTIONS ////

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
warn_invalid_escape_sequence(Parser *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಬಫರ್, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೊದಲ_invalid_escape, Token *t)
{
    ಆದರೆ (p->call_invalid_ನಿಯಮಗಳು) {
        // Do not report warnings if we are in the second pass of the parser
        // to avoid showing the warning twice.
        ಹಿಂತಿರುಗಿ 0;
    }
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)*ಮೊದಲ_invalid_escape;
    ಆದರೆ ((t->type == FSTRING_ಮಧ್ಯ || t->type == FSTRING_ಕೊನೆ || t->type == TSTRING_ಮಧ್ಯ || t->type == TSTRING_ಕೊನೆ)
            && (c == '{' || c == '}')) {
        // in this case the tokenizer has already emitted a warning,
        // see Parser/tokenizer/helpers.c:warn_invalid_escape_sequence
        ಹಿಂತಿರುಗಿ 0;
    }

    ಪೂರ್ಣಾಂಕ octal = ('4' <= c && c <= '7');
    PyObject *ಸಂದೇಶ =
        octal
        ? PyUnicode_FromFormat(
              "\"\\%.3s\" is an invalid octal escape sequence. "
              "Such sequences will not work in the future. "
              "Did you mean \"\\\\%.3s\"? A raw string is also an option.",
              ಮೊದಲ_invalid_escape, ಮೊದಲ_invalid_escape)
        : PyUnicode_FromFormat(
              "\"\\%c\" is an invalid escape sequence. "
              "Such sequences will not work in the future. "
              "Did you mean \"\\\\%c\"? A raw string is also an option.",
              c, c);
    ಆದರೆ (ಸಂದೇಶ == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ -1;
    }
    PyObject *category;
    ಆದರೆ (p->feature_ಆವೃತ್ತಿ >= 12) {
        category = PyExc_SyntaxWarning;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        category = PyExc_DeprecationWarning;
    }

    // Calculate the lineno and the col_offset of the invalid escape sequence
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಆರಂಭ = ಬಫರ್;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆ = ಮೊದಲ_invalid_escape;
    ಪೂರ್ಣಾಂಕ lineno = t->lineno;
    ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = t->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;
    ಆಗ (ಆರಂಭ < ಕೊನೆ) {
        ಆದರೆ (*ಆರಂಭ == '\n') {
            lineno++;
            ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = 0;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ++;
        }
        ಆರಂಭ++;
    }

    // Count the number of quotes in the token
    ಅಕ್ಷರ ಮೊದಲ_quote = 0;
    ಆದರೆ (lineno == t->lineno) {
        ಪೂರ್ಣಾಂಕ quote_ಎಣಿಕೆ = 0;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* tok = Py_ಬೈಟುಗಳು_As_String(t->ಬೈಟುಗಳು);
        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Py_ಬೈಟುಗಳು_ಗಾತ್ರ(t->ಬೈಟುಗಳು); i++) {
            ಆದರೆ (tok[i] == '\'' || tok[i] == '\"') {
                ಆದರೆ (quote_ಎಣಿಕೆ == 0) {
                    ಮೊದಲ_quote = tok[i];
                }
                ಆದರೆ (tok[i] == ಮೊದಲ_quote) {
                    quote_ಎಣಿಕೆ++;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                ನಿಲ್ಲಿಸು;
            }
        }

        ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ += quote_ಎಣಿಕೆ;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    ಆದರೆ (PyErr_WarnExplicitObject(category, ಸಂದೇಶ, info.filename,
                                 lineno, info.module, ಖಾಲಿ) < 0) {
        ಆದರೆ (PyErr_ExceptionMatches(category)) {
            /* Replace the Syntax/DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            /* This is needed, in order for the SyntaxError to point to the token t,
               since _PyPegen_raise_error uses p->tokens[p->fill - 1] for the
               error location, if p->known_err_token is not set. */
            p->ತಿಳಿದ_err_token = t;
            ಆದರೆ (octal) {
                RAISE_ERROR_ತಿಳಿದ_LOCATION(p, PyExc_SyntaxError, lineno, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ-1, lineno, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ+1,
                    "\"\\%.3s\" is an invalid octal escape sequence. "
                    "Did you mean \"\\\\%.3s\"? A raw string is also an option.",
                    ಮೊದಲ_invalid_escape, ಮೊದಲ_invalid_escape);
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                RAISE_ERROR_ತಿಳಿದ_LOCATION(p, PyExc_SyntaxError, lineno, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ-1, lineno, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ+1,
                    "\"\\%c\" is an invalid escape sequence. "
                    "Did you mean \"\\\\%c\"? A raw string is also an option.",
                    c, c);
            }
        }
        Py_DECREF(ಸಂದೇಶ);
        ಹಿಂತಿರುಗಿ -1;
    }
    Py_DECREF(ಸಂದೇಶ);
    ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ PyObject *
decode_utf8(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **sPtr, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆ)
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t;
    t = s = *sPtr;
    ಆಗ (s < ಕೊನೆ && (*s & 0x80)) {
        s++;
    }
    *sPtr = s;
    ಹಿಂತಿರುಗಿ PyUnicode_DecodeUTF8(t, s - t, ಖಾಲಿ);
}

ಸ್ಥಿರ PyObject *
decode_unicode_with_escapes(Parser *parser, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಗಾತ್ರ_t len, Token *t)
{
    PyObject *v;
    ಅಕ್ಷರ *p;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆ;

    /* check for integer overflow */
    ಆದರೆ (len > (ಗಾತ್ರ_t)PY_SSIZE_T_MAX / 6) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    /* "ä" (2 bytes) may become "\U000000E4" (10 bytes), or 1:5.
     * "\ä" (3 bytes) may become "\u005c\U000000E4" (16 bytes), or ~1:6. */
    Py_ssize_t alloc = (Py_ssize_t)len * 6;
    ಅಕ್ಷರ *buf = PyMem_Malloc(alloc);
    ಆದರೆ (buf == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ Py_Err_No_ಸ್ಮರಣೆ();
    }
    p = buf;
    ಕೊನೆ = s + len;
    ಆಗ (s < ಕೊನೆ) {
        ಆದರೆ (*s == '\\') {
            *p++ = *s++;
            ಆದರೆ (s >= ಕೊನೆ || *s & 0x80) {
                ಸ್ಮರಣೆ_ನಕಲು(p, "u005c", 5);
                p += 5;
                ಆದರೆ (s >= ಕೊನೆ) {
                    ನಿಲ್ಲಿಸು;
                }
            }
        }
        ಆದರೆ (*s & 0x80) {
            PyObject *w;
            ಪೂರ್ಣಾಂಕ ಬಗೆ;
            ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ;
            Py_ssize_t w_len;
            Py_ssize_t i;
            w = decode_utf8(&s, ಕೊನೆ);
            ಆದರೆ (w == ಖಾಲಿ) {
                PyMem_Free(buf);
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
            }
            ಬಗೆ = Py_Unicode_ಬಗೆ(w);
            ದತ್ತಾಂಶ = Py_Unicode_ದತ್ತಾಂಶ(w);
            w_len = Py_Unicode_GET_ಉದ್ದಳತೆ(w);
            ಗಾಗಿ (i = 0; i < w_len; i++) {
                // sprintf() writes a null byte: the buffer is large enough
                // for that thanks to the overallocation.
                ಪ್ರತಿಜ್ಞಾ((p + 11 - buf) <= alloc);
                Py_UCS4 chr = PyUnicode_READ(ಬಗೆ, ದತ್ತಾಂಶ, i);
                sprintf(p, "\\U%08x", chr);
                p += 10;
            }
            Py_DECREF(w);
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            *p++ = *s++;
        }
    }
    len = (ಗಾತ್ರ_t)(p - buf);

    ಪೂರ್ಣಾಂಕ ಮೊದಲ_invalid_escape_char;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೊದಲ_invalid_escape_ptr;
    v = _PyUnicode_DecodeUnicodeEscapeInternal2(buf, (Py_ssize_t)len, ಖಾಲಿ, ಖಾಲಿ,
                                                &ಮೊದಲ_invalid_escape_char,
                                                &ಮೊದಲ_invalid_escape_ptr);

    // HACK: later we can simply pass the line no, since we don't preserve the tokens
    // when we are decoding the string but we preserve the line numbers.
    ಆದರೆ (v != ಖಾಲಿ && ಮೊದಲ_invalid_escape_ptr != ಖಾಲಿ && t != ಖಾಲಿ) {
        ಆದರೆ (warn_invalid_escape_sequence(parser, buf, ಮೊದಲ_invalid_escape_ptr, t) < 0) {
            /* We have not deallocated the buffer before because
             * first_invalid_escape_ptr points inside buf. */
            PyMem_Free(buf);
            Py_DECREF(v);
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
    }
    PyMem_Free(buf);
    ಹಿಂತಿರುಗಿ v;
}

ಸ್ಥಿರ PyObject *
decode_ಬೈಟುಗಳು_with_escapes(Parser *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, Py_ssize_t len, Token *t)
{
    ಪೂರ್ಣಾಂಕ ಮೊದಲ_invalid_escape_char;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೊದಲ_invalid_escape_ptr;
    PyObject *ಫಲಿತಾಂಶ = Py_ಬೈಟುಗಳು_Decode_Escape2(s, len, ಖಾಲಿ,
                                              &ಮೊದಲ_invalid_escape_char,
                                              &ಮೊದಲ_invalid_escape_ptr);
    ಆದರೆ (ಫಲಿತಾಂಶ == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (ಮೊದಲ_invalid_escape_ptr != ಖಾಲಿ) {
        ಆದರೆ (warn_invalid_escape_sequence(p, s, ಮೊದಲ_invalid_escape_ptr, t) < 0) {
            Py_DECREF(ಫಲಿತಾಂಶ);
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

PyObject *
_PyPegen_decode_string(Parser *p, ಪೂರ್ಣಾಂಕ raw, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಗಾತ್ರ_t len, Token *t)
{
    ಆದರೆ (raw) {
        ಹಿಂತಿರುಗಿ PyUnicode_DecodeUTF8Stateful(s, (Py_ssize_t)len, ಖಾಲಿ, ಖಾಲಿ);
    }
    ಹಿಂತಿರುಗಿ decode_unicode_with_escapes(p, s, len, t);
}

/* s must include the bracketing quote characters, and r, b &/or f prefixes
    (if any), and embedded escape sequences (if any). (f-strings are handled by the parser)
   _PyPegen_parse_string parses it, and returns the decoded Python string object. */
PyObject *
_PyPegen_parse_string(Parser *p, Token *t)
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = Py_ಬೈಟುಗಳು_As_String(t->ಬೈಟುಗಳು);
    ಆದರೆ (s == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಗಾತ್ರ_t len;
    ಪೂರ್ಣಾಂಕ quote = Py_CHARMASK(*s);
    ಪೂರ್ಣಾಂಕ bytesmode = 0;
    ಪೂರ್ಣಾಂಕ rawmode = 0;

    ಆದರೆ (Py_ISALPHA(quote)) {
        ಆಗ (!bytesmode || !rawmode) {
            ಆದರೆ (quote == 'b' || quote == 'B') {
                quote =(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)*++s;
                bytesmode = 1;
            }
            ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (quote == 'u' || quote == 'U') {
                quote = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)*++s;
            }
            ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (quote == 'r' || quote == 'R') {
                quote = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)*++s;
                rawmode = 1;
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                ನಿಲ್ಲಿಸು;
            }
        }
    }

    ಆದರೆ (quote != '\'' && quote != '\"') {
        Py_Err_Bad_ಆಂತರಿಕ_Call();
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    /* Skip the leading quote char. */
    s++;
    len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s);
    // gh-120155: 's' contains at least the trailing quote,
    // so the code '--len' below is safe.
    ಪ್ರತಿಜ್ಞಾ(len >= 1);

    ಆದರೆ (len > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) {
        Py_Err_ಗಣ_String(Py_Exc_ಅತಿಪ್ರವಾಹ_Error, "string to parse is too long");
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (s[--len] != quote) {
        /* Last quote char must match the first. */
        Py_Err_Bad_ಆಂತರಿಕ_Call();
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (len >= 4 && s[0] == quote && s[1] == quote) {
        /* A triple quoted string. We've already skipped one quote at
           the start and one at the end of the string. Now skip the
           two at the start. */
        s += 2;
        len -= 2;
        /* And check that the last two match. */
        ಆದರೆ (s[--len] != quote || s[--len] != quote) {
            Py_Err_Bad_ಆಂತರಿಕ_Call();
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
    }

    /* Avoid invoking escape decoding routines if possible. */
    rawmode = rawmode || strchr(s, '\\') == ಖಾಲಿ;
    ಆದರೆ (bytesmode) {
        /* Disallow non-ASCII characters. */
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ch;
        ಗಾಗಿ (ch = s; *ch; ch++) {
            ಆದರೆ (Py_CHARMASK(*ch) >= 0x80) {
                RAISE_SYNTAX_ERROR_ತಿಳಿದ_LOCATION(
                                   t,
                                   "bytes can only contain ASCII "
                                   "literal characters");
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
            }
        }
        ಆದರೆ (rawmode) {
            ಹಿಂತಿರುಗಿ Py_ಬೈಟುಗಳು_From_String_And_ಗಾತ್ರ(s, (Py_ssize_t)len);
        }
        ಹಿಂತಿರುಗಿ decode_ಬೈಟುಗಳು_with_escapes(p, s, (Py_ssize_t)len, t);
    }
    ಹಿಂತಿರುಗಿ _PyPegen_decode_string(p, rawmode, s, len, t);
}
