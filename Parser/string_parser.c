#ضمن <Python.h>
#ضمن "pycore_bytesobject.h"   // _PyBytes_DecodeEscape()
#ضمن "pycore_unicodeobject.h" // _PyUnicode_DecodeUnicodeEscapeInternal()

#ضمن "pegen.h"
#ضمن "string_parser.h"

#ضمن <stdbool.h>

//// STRING HANDLING FUNCTIONS ////

ساكن صحيح
warn_invalid_escape_sequence(Parser *p, ثابت حرف* بفر, ثابت حرف *پهريون_invalid_escape, Token *t)
{
    إذا (p->call_invalid_قاعدا) {
        // Do not report warnings if we are in the second pass of the parser
        // to avoid showing the warning twice.
        أرجع 0;
    }
    بنانشان حرف c = (بنانشان حرف)*پهريون_invalid_escape;
    إذا ((t->type == FSTRING_وچ || t->type == FSTRING_پڇاڙي || t->type == TSTRING_وچ || t->type == TSTRING_پڇاڙي)
            && (c == '{' || c == '}')) {
        // in this case the tokenizer has already emitted a warning,
        // see Parser/tokenizer/helpers.c:warn_invalid_escape_sequence
        أرجع 0;
    }

    صحيح octal = ('4' <= c && c <= '7');
    PyObject *نياپو =
        octal
        ? PyUnicode_FromFormat(
              "\"\\%.3s\" is an invalid octal escape sequence. "
              "Such sequences will not work in the future. "
              "Did you mean \"\\\\%.3s\"? A raw string is also an option.",
              پهريون_invalid_escape, پهريون_invalid_escape)
        : PyUnicode_FromFormat(
              "\"\\%c\" is an invalid escape sequence. "
              "Such sequences will not work in the future. "
              "Did you mean \"\\\\%c\"? A raw string is also an option.",
              c, c);
    إذا (نياپو == عدم) {
        أرجع -1;
    }
    PyObject *category;
    إذا (p->feature_نسخو >= 12) {
        category = PyExc_SyntaxWarning;
    }
    وإلا {
        category = PyExc_DeprecationWarning;
    }

    // Calculate the lineno and the col_offset of the invalid escape sequence
    ثابت حرف *شروعات = بفر;
    ثابت حرف *پڇاڙي = پهريون_invalid_escape;
    صحيح lineno = t->lineno;
    صحيح کانو_وٿي = t->کانو_وٿي;
    طالما (شروعات < پڇاڙي) {
        إذا (*شروعات == '\n') {
            lineno++;
            کانو_وٿي = 0;
        }
        وإلا {
            کانو_وٿي++;
        }
        شروعات++;
    }

    // Count the number of quotes in the token
    حرف پهريون_quote = 0;
    إذا (lineno == t->lineno) {
        صحيح quote_ڳڻپ = 0;
        ثابت حرف* tok = Py_بائيٽون_As_String(t->بائيٽون);
        لكل (صحيح i = 0; i < Py_بائيٽون_سائيز(t->بائيٽون); i++) {
            إذا (tok[i] == '\'' || tok[i] == '\"') {
                إذا (quote_ڳڻپ == 0) {
                    پهريون_quote = tok[i];
                }
                إذا (tok[i] == پهريون_quote) {
                    quote_ڳڻپ++;
                }
            } وإلا {
                اكسر;
            }
        }

        کانو_وٿي += quote_ڳڻپ;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    إذا (PyErr_WarnExplicitObject(category, نياپو, info.filename,
                                 lineno, info.module, عدم) < 0) {
        إذا (PyErr_ExceptionMatches(category)) {
            /* Replace the Syntax/DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            /* This is needed, in order for the SyntaxError to point to the token t,
               since _PyPegen_raise_error uses p->tokens[p->fill - 1] for the
               error location, if p->known_err_token is not set. */
            p->معلوم_err_token = t;
            إذا (octal) {
                RAISE_ERROR_معلوم_LOCATION(p, PyExc_SyntaxError, lineno, کانو_وٿي-1, lineno, کانو_وٿي+1,
                    "\"\\%.3s\" is an invalid octal escape sequence. "
                    "Did you mean \"\\\\%.3s\"? A raw string is also an option.",
                    پهريون_invalid_escape, پهريون_invalid_escape);
            }
            وإلا {
                RAISE_ERROR_معلوم_LOCATION(p, PyExc_SyntaxError, lineno, کانو_وٿي-1, lineno, کانو_وٿي+1,
                    "\"\\%c\" is an invalid escape sequence. "
                    "Did you mean \"\\\\%c\"? A raw string is also an option.",
                    c, c);
            }
        }
        Py_DECREF(نياپو);
        أرجع -1;
    }
    Py_DECREF(نياپو);
    أرجع 0;
}

ساكن PyObject *
decode_utf8(ثابت حرف **sPtr, ثابت حرف *پڇاڙي)
{
    ثابت حرف *s;
    ثابت حرف *t;
    t = s = *sPtr;
    طالما (s < پڇاڙي && (*s & 0x80)) {
        s++;
    }
    *sPtr = s;
    أرجع PyUnicode_DecodeUTF8(t, s - t, عدم);
}

ساكن PyObject *
decode_unicode_with_escapes(Parser *parser, ثابت حرف *s, سائيز_t len, Token *t)
{
    PyObject *v;
    حرف *p;
    ثابت حرف *پڇاڙي;

    /* check for integer overflow */
    إذا (len > (سائيز_t)PY_SSIZE_T_MAX / 6) {
        أرجع عدم;
    }
    /* "ä" (2 bytes) may become "\U000000E4" (10 bytes), or 1:5.
     * "\ä" (3 bytes) may become "\u005c\U000000E4" (16 bytes), or ~1:6. */
    Py_ssize_t alloc = (Py_ssize_t)len * 6;
    حرف *buf = PyMem_Malloc(alloc);
    إذا (buf == عدم) {
        أرجع Py_Err_No_يادگيري();
    }
    p = buf;
    پڇاڙي = s + len;
    طالما (s < پڇاڙي) {
        إذا (*s == '\\') {
            *p++ = *s++;
            إذا (s >= پڇاڙي || *s & 0x80) {
                يادگيري_نقل(p, "u005c", 5);
                p += 5;
                إذا (s >= پڇاڙي) {
                    اكسر;
                }
            }
        }
        إذا (*s & 0x80) {
            PyObject *w;
            صحيح قسم;
            ثابت فراغ *معلومات;
            Py_ssize_t w_len;
            Py_ssize_t i;
            w = decode_utf8(&s, پڇاڙي);
            إذا (w == عدم) {
                PyMem_Free(buf);
                أرجع عدم;
            }
            قسم = Py_Unicode_قسم(w);
            معلومات = Py_Unicode_معلومات(w);
            w_len = Py_Unicode_GET_ڊيگهائي(w);
            لكل (i = 0; i < w_len; i++) {
                // sprintf() writes a null byte: the buffer is large enough
                // for that thanks to the overallocation.
                أكد((p + 11 - buf) <= alloc);
                Py_UCS4 chr = PyUnicode_READ(قسم, معلومات, i);
                sprintf(p, "\\U%08x", chr);
                p += 10;
            }
            Py_DECREF(w);
        }
        وإلا {
            *p++ = *s++;
        }
    }
    len = (سائيز_t)(p - buf);

    صحيح پهريون_invalid_escape_char;
    ثابت حرف *پهريون_invalid_escape_ptr;
    v = _PyUnicode_DecodeUnicodeEscapeInternal2(buf, (Py_ssize_t)len, عدم, عدم,
                                                &پهريون_invalid_escape_char,
                                                &پهريون_invalid_escape_ptr);

    // HACK: later we can simply pass the line no, since we don't preserve the tokens
    // when we are decoding the string but we preserve the line numbers.
    إذا (v != عدم && پهريون_invalid_escape_ptr != عدم && t != عدم) {
        إذا (warn_invalid_escape_sequence(parser, buf, پهريون_invalid_escape_ptr, t) < 0) {
            /* We have not deallocated the buffer before because
             * first_invalid_escape_ptr points inside buf. */
            PyMem_Free(buf);
            Py_DECREF(v);
            أرجع عدم;
        }
    }
    PyMem_Free(buf);
    أرجع v;
}

ساكن PyObject *
decode_بائيٽون_with_escapes(Parser *p, ثابت حرف *s, Py_ssize_t len, Token *t)
{
    صحيح پهريون_invalid_escape_char;
    ثابت حرف *پهريون_invalid_escape_ptr;
    PyObject *نتيجو = Py_بائيٽون_Decode_Escape2(s, len, عدم,
                                              &پهريون_invalid_escape_char,
                                              &پهريون_invalid_escape_ptr);
    إذا (نتيجو == عدم) {
        أرجع عدم;
    }

    إذا (پهريون_invalid_escape_ptr != عدم) {
        إذا (warn_invalid_escape_sequence(p, s, پهريون_invalid_escape_ptr, t) < 0) {
            Py_DECREF(نتيجو);
            أرجع عدم;
        }
    }
    أرجع نتيجو;
}

PyObject *
_PyPegen_decode_string(Parser *p, صحيح raw, ثابت حرف *s, سائيز_t len, Token *t)
{
    إذا (raw) {
        أرجع PyUnicode_DecodeUTF8Stateful(s, (Py_ssize_t)len, عدم, عدم);
    }
    أرجع decode_unicode_with_escapes(p, s, len, t);
}

/* s must include the bracketing quote characters, and r, b &/or f prefixes
    (if any), and embedded escape sequences (if any). (f-strings are handled by the parser)
   _PyPegen_parse_string parses it, and returns the decoded Python string object. */
PyObject *
_PyPegen_parse_string(Parser *p, Token *t)
{
    ثابت حرف *s = Py_بائيٽون_As_String(t->بائيٽون);
    إذا (s == عدم) {
        أرجع عدم;
    }

    سائيز_t len;
    صحيح quote = Py_CHARMASK(*s);
    صحيح bytesmode = 0;
    صحيح rawmode = 0;

    إذا (Py_ISALPHA(quote)) {
        طالما (!bytesmode || !rawmode) {
            إذا (quote == 'b' || quote == 'B') {
                quote =(بنانشان حرف)*++s;
                bytesmode = 1;
            }
            وإلا إذا (quote == 'u' || quote == 'U') {
                quote = (بنانشان حرف)*++s;
            }
            وإلا إذا (quote == 'r' || quote == 'R') {
                quote = (بنانشان حرف)*++s;
                rawmode = 1;
            }
            وإلا {
                اكسر;
            }
        }
    }

    إذا (quote != '\'' && quote != '\"') {
        Py_Err_Bad_اندروني_Call();
        أرجع عدم;
    }

    /* Skip the leading quote char. */
    s++;
    len = لڙي_ڊيگهه(s);
    // gh-120155: 's' contains at least the trailing quote,
    // so the code '--len' below is safe.
    أكد(len >= 1);

    إذا (len > وڌ_کان_وڌ_عدد) {
        Py_Err_سيٽ_String(Py_Exc_حد_کان_مٿي_Error, "string to parse is too long");
        أرجع عدم;
    }
    إذا (s[--len] != quote) {
        /* Last quote char must match the first. */
        Py_Err_Bad_اندروني_Call();
        أرجع عدم;
    }
    إذا (len >= 4 && s[0] == quote && s[1] == quote) {
        /* A triple quoted string. We've already skipped one quote at
           the start and one at the end of the string. Now skip the
           two at the start. */
        s += 2;
        len -= 2;
        /* And check that the last two match. */
        إذا (s[--len] != quote || s[--len] != quote) {
            Py_Err_Bad_اندروني_Call();
            أرجع عدم;
        }
    }

    /* Avoid invoking escape decoding routines if possible. */
    rawmode = rawmode || strchr(s, '\\') == عدم;
    إذا (bytesmode) {
        /* Disallow non-ASCII characters. */
        ثابت حرف *ch;
        لكل (ch = s; *ch; ch++) {
            إذا (Py_CHARMASK(*ch) >= 0x80) {
                RAISE_SYNTAX_ERROR_معلوم_LOCATION(
                                   t,
                                   "bytes can only contain ASCII "
                                   "literal characters");
                أرجع عدم;
            }
        }
        إذا (rawmode) {
            أرجع Py_بائيٽون_From_String_And_سائيز(s, (Py_ssize_t)len);
        }
        أرجع decode_بائيٽون_with_escapes(p, s, (Py_ssize_t)len, t);
    }
    أرجع _PyPegen_decode_string(p, rawmode, s, len, t);
}
