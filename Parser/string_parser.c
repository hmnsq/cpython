#شامل <Python.h>
#شامل "pycore_bytesobject.h"   // _PyBytes_DecodeEscape()
#شامل "pycore_unicodeobject.h" // _PyUnicode_DecodeUnicodeEscapeInternal()

#شامل "pegen.h"
#شامل "string_parser.h"

#شامل <stdbool.h>

//// STRING HANDLING FUNCTIONS ////

ساکن صحیح
warn_invalid_escape_sequence(Parser *p, ثابت حرف* بفر, ثابت حرف *پہلا_invalid_escape, Token *t)
{
    اگر (p->call_invalid_قواعد) {
        // Do not report warnings if we are in the second pass of the parser
        // to avoid showing the warning twice.
        واپس 0;
    }
    غیرعلامتی حرف c = (غیرعلامتی حرف)*پہلا_invalid_escape;
    اگر ((t->type == FSTRING_درمیان || t->type == FSTRING_انتہا || t->type == TSTRING_درمیان || t->type == TSTRING_انتہا)
            && (c == '{' || c == '}')) {
        // in this case the tokenizer has already emitted a warning,
        // see Parser/tokenizer/helpers.c:warn_invalid_escape_sequence
        واپس 0;
    }

    صحیح octal = ('4' <= c && c <= '7');
    PyObject *پیغام =
        octal
        ? PyUnicode_FromFormat(
              "\"\\%.3s\" is an invalid octal escape sequence. "
              "Such sequences will not work in the future. "
              "Did you mean \"\\\\%.3s\"? A raw string is also an option.",
              پہلا_invalid_escape, پہلا_invalid_escape)
        : PyUnicode_FromFormat(
              "\"\\%c\" is an invalid escape sequence. "
              "Such sequences will not work in the future. "
              "Did you mean \"\\\\%c\"? A raw string is also an option.",
              c, c);
    اگر (پیغام == تہی) {
        واپس -1;
    }
    PyObject *category;
    اگر (p->feature_نسخہ >= 12) {
        category = PyExc_SyntaxWarning;
    }
    ورنہ {
        category = PyExc_DeprecationWarning;
    }

    // Calculate the lineno and the col_offset of the invalid escape sequence
    ثابت حرف *آغاز = بفر;
    ثابت حرف *انتہا = پہلا_invalid_escape;
    صحیح lineno = t->lineno;
    صحیح ستون_انحراف = t->ستون_انحراف;
    جب (آغاز < انتہا) {
        اگر (*آغاز == '\n') {
            lineno++;
            ستون_انحراف = 0;
        }
        ورنہ {
            ستون_انحراف++;
        }
        آغاز++;
    }

    // Count the number of quotes in the token
    حرف پہلا_quote = 0;
    اگر (lineno == t->lineno) {
        صحیح quote_گنتی = 0;
        ثابت حرف* tok = Py_بائٹیں_As_String(t->بائٹیں);
        برائے (صحیح i = 0; i < Py_بائٹیں_سائز(t->بائٹیں); i++) {
            اگر (tok[i] == '\'' || tok[i] == '\"') {
                اگر (quote_گنتی == 0) {
                    پہلا_quote = tok[i];
                }
                اگر (tok[i] == پہلا_quote) {
                    quote_گنتی++;
                }
            } ورنہ {
                توڑو;
            }
        }

        ستون_انحراف += quote_گنتی;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    اگر (PyErr_WarnExplicitObject(category, پیغام, info.filename,
                                 lineno, info.module, تہی) < 0) {
        اگر (PyErr_ExceptionMatches(category)) {
            /* Replace the Syntax/DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            /* This is needed, in order for the SyntaxError to point to the token t,
               since _PyPegen_raise_error uses p->tokens[p->fill - 1] for the
               error location, if p->known_err_token is not set. */
            p->معلوم_err_token = t;
            اگر (octal) {
                RAISE_ERROR_معلوم_LOCATION(p, PyExc_SyntaxError, lineno, ستون_انحراف-1, lineno, ستون_انحراف+1,
                    "\"\\%.3s\" is an invalid octal escape sequence. "
                    "Did you mean \"\\\\%.3s\"? A raw string is also an option.",
                    پہلا_invalid_escape, پہلا_invalid_escape);
            }
            ورنہ {
                RAISE_ERROR_معلوم_LOCATION(p, PyExc_SyntaxError, lineno, ستون_انحراف-1, lineno, ستون_انحراف+1,
                    "\"\\%c\" is an invalid escape sequence. "
                    "Did you mean \"\\\\%c\"? A raw string is also an option.",
                    c, c);
            }
        }
        Py_DECREF(پیغام);
        واپس -1;
    }
    Py_DECREF(پیغام);
    واپس 0;
}

ساکن PyObject *
decode_utf8(ثابت حرف **sPtr, ثابت حرف *انتہا)
{
    ثابت حرف *s;
    ثابت حرف *t;
    t = s = *sPtr;
    جب (s < انتہا && (*s & 0x80)) {
        s++;
    }
    *sPtr = s;
    واپس PyUnicode_DecodeUTF8(t, s - t, تہی);
}

ساکن PyObject *
decode_unicode_with_escapes(Parser *parser, ثابت حرف *s, سائز_t len, Token *t)
{
    PyObject *v;
    حرف *p;
    ثابت حرف *انتہا;

    /* check for integer overflow */
    اگر (len > (سائز_t)PY_SSIZE_T_MAX / 6) {
        واپس تہی;
    }
    /* "ä" (2 bytes) may become "\U000000E4" (10 bytes), or 1:5.
     * "\ä" (3 bytes) may become "\u005c\U000000E4" (16 bytes), or ~1:6. */
    Py_ssize_t alloc = (Py_ssize_t)len * 6;
    حرف *buf = PyMem_Malloc(alloc);
    اگر (buf == تہی) {
        واپس Py_Err_No_حافظہ();
    }
    p = buf;
    انتہا = s + len;
    جب (s < انتہا) {
        اگر (*s == '\\') {
            *p++ = *s++;
            اگر (s >= انتہا || *s & 0x80) {
                حافظہ_نقل(p, "u005c", 5);
                p += 5;
                اگر (s >= انتہا) {
                    توڑو;
                }
            }
        }
        اگر (*s & 0x80) {
            PyObject *w;
            صحیح قسم;
            ثابت خالی *ڈیٹا;
            Py_ssize_t w_len;
            Py_ssize_t i;
            w = decode_utf8(&s, انتہا);
            اگر (w == تہی) {
                PyMem_Free(buf);
                واپس تہی;
            }
            قسم = Py_Unicode_قسم(w);
            ڈیٹا = Py_Unicode_ڈیٹا(w);
            w_len = Py_Unicode_GET_لمبائی(w);
            برائے (i = 0; i < w_len; i++) {
                // sprintf() writes a null byte: the buffer is large enough
                // for that thanks to the overallocation.
                أكد((p + 11 - buf) <= alloc);
                Py_UCS4 chr = PyUnicode_READ(قسم, ڈیٹا, i);
                sprintf(p, "\\U%08x", chr);
                p += 10;
            }
            Py_DECREF(w);
        }
        ورنہ {
            *p++ = *s++;
        }
    }
    len = (سائز_t)(p - buf);

    صحیح پہلا_invalid_escape_char;
    ثابت حرف *پہلا_invalid_escape_ptr;
    v = _PyUnicode_DecodeUnicodeEscapeInternal2(buf, (Py_ssize_t)len, تہی, تہی,
                                                &پہلا_invalid_escape_char,
                                                &پہلا_invalid_escape_ptr);

    // HACK: later we can simply pass the line no, since we don't preserve the tokens
    // when we are decoding the string but we preserve the line numbers.
    اگر (v != تہی && پہلا_invalid_escape_ptr != تہی && t != تہی) {
        اگر (warn_invalid_escape_sequence(parser, buf, پہلا_invalid_escape_ptr, t) < 0) {
            /* We have not deallocated the buffer before because
             * first_invalid_escape_ptr points inside buf. */
            PyMem_Free(buf);
            Py_DECREF(v);
            واپس تہی;
        }
    }
    PyMem_Free(buf);
    واپس v;
}

ساکن PyObject *
decode_بائٹیں_with_escapes(Parser *p, ثابت حرف *s, Py_ssize_t len, Token *t)
{
    صحیح پہلا_invalid_escape_char;
    ثابت حرف *پہلا_invalid_escape_ptr;
    PyObject *نتیجہ = Py_بائٹیں_Decode_Escape2(s, len, تہی,
                                              &پہلا_invalid_escape_char,
                                              &پہلا_invalid_escape_ptr);
    اگر (نتیجہ == تہی) {
        واپس تہی;
    }

    اگر (پہلا_invalid_escape_ptr != تہی) {
        اگر (warn_invalid_escape_sequence(p, s, پہلا_invalid_escape_ptr, t) < 0) {
            Py_DECREF(نتیجہ);
            واپس تہی;
        }
    }
    واپس نتیجہ;
}

PyObject *
_PyPegen_decode_string(Parser *p, صحیح raw, ثابت حرف *s, سائز_t len, Token *t)
{
    اگر (raw) {
        واپس PyUnicode_DecodeUTF8Stateful(s, (Py_ssize_t)len, تہی, تہی);
    }
    واپس decode_unicode_with_escapes(p, s, len, t);
}

/* s must include the bracketing quote characters, and r, b &/or f prefixes
    (if any), and embedded escape sequences (if any). (f-strings are handled by the parser)
   _PyPegen_parse_string parses it, and returns the decoded Python string object. */
PyObject *
_PyPegen_parse_string(Parser *p, Token *t)
{
    ثابت حرف *s = Py_بائٹیں_As_String(t->بائٹیں);
    اگر (s == تہی) {
        واپس تہی;
    }

    سائز_t len;
    صحیح quote = Py_CHARMASK(*s);
    صحیح bytesmode = 0;
    صحیح rawmode = 0;

    اگر (Py_ISALPHA(quote)) {
        جب (!bytesmode || !rawmode) {
            اگر (quote == 'b' || quote == 'B') {
                quote =(غیرعلامتی حرف)*++s;
                bytesmode = 1;
            }
            ورنہ اگر (quote == 'u' || quote == 'U') {
                quote = (غیرعلامتی حرف)*++s;
            }
            ورنہ اگر (quote == 'r' || quote == 'R') {
                quote = (غیرعلامتی حرف)*++s;
                rawmode = 1;
            }
            ورنہ {
                توڑو;
            }
        }
    }

    اگر (quote != '\'' && quote != '\"') {
        Py_Err_Bad_اندرونی_Call();
        واپس تہی;
    }

    /* Skip the leading quote char. */
    s++;
    len = لڑی_لمبائی(s);
    // gh-120155: 's' contains at least the trailing quote,
    // so the code '--len' below is safe.
    أكد(len >= 1);

    اگر (len > زیادہ_سے_زیادہ_عدد) {
        Py_Err_سیٹ_String(Py_Exc_حد_سے_تجاوز_Error, "string to parse is too long");
        واپس تہی;
    }
    اگر (s[--len] != quote) {
        /* Last quote char must match the first. */
        Py_Err_Bad_اندرونی_Call();
        واپس تہی;
    }
    اگر (len >= 4 && s[0] == quote && s[1] == quote) {
        /* A triple quoted string. We've already skipped one quote at
           the start and one at the end of the string. Now skip the
           two at the start. */
        s += 2;
        len -= 2;
        /* And check that the last two match. */
        اگر (s[--len] != quote || s[--len] != quote) {
            Py_Err_Bad_اندرونی_Call();
            واپس تہی;
        }
    }

    /* Avoid invoking escape decoding routines if possible. */
    rawmode = rawmode || strchr(s, '\\') == تہی;
    اگر (bytesmode) {
        /* Disallow non-ASCII characters. */
        ثابت حرف *ch;
        برائے (ch = s; *ch; ch++) {
            اگر (Py_CHARMASK(*ch) >= 0x80) {
                RAISE_SYNTAX_ERROR_معلوم_LOCATION(
                                   t,
                                   "bytes can only contain ASCII "
                                   "literal characters");
                واپس تہی;
            }
        }
        اگر (rawmode) {
            واپس Py_بائٹیں_From_String_And_سائز(s, (Py_ssize_t)len);
        }
        واپس decode_بائٹیں_with_escapes(p, s, (Py_ssize_t)len, t);
    }
    واپس _PyPegen_decode_string(p, rawmode, s, len, t);
}
