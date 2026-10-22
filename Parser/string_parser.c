#అన్తర్భావ <Python.h>
#అన్తర్భావ "pycore_bytesobject.h"   // _PyBytes_DecodeEscape()
#అన్తర్భావ "pycore_unicodeobject.h" // _PyUnicode_DecodeUnicodeEscapeInternal()

#అన్తర్భావ "pegen.h"
#అన్తర్భావ "string_parser.h"

#అన్తర్భావ <stdbool.h>

//// STRING HANDLING FUNCTIONS ////

స్థిర పూర్ణాంకం
warn_invalid_escape_sequence(Parser *p, స్థిరరాశి అక్షరం* బఫరు, స్థిరరాశి అక్షరం *మొదటి_invalid_escape, Token *t)
{
    ఒకవేళ (p->call_invalid_నియమాలు) {
        // Do not report warnings if we are in the second pass of the parser
        // to avoid showing the warning twice.
        తిరిగి 0;
    }
    గుర్తులేని అక్షరం c = (గుర్తులేని అక్షరం)*మొదటి_invalid_escape;
    ఒకవేళ ((t->type == FSTRING_మధ్య || t->type == FSTRING_చివర || t->type == TSTRING_మధ్య || t->type == TSTRING_చివర)
            && (c == '{' || c == '}')) {
        // in this case the tokenizer has already emitted a warning,
        // see Parser/tokenizer/helpers.c:warn_invalid_escape_sequence
        తిరిగి 0;
    }

    పూర్ణాంకం octal = ('4' <= c && c <= '7');
    PyObject *సందేశం =
        octal
        ? PyUnicode_FromFormat(
              "\"\\%.3s\" is an invalid octal escape sequence. "
              "Such sequences will not work in the future. "
              "Did you mean \"\\\\%.3s\"? A raw string is also an option.",
              మొదటి_invalid_escape, మొదటి_invalid_escape)
        : PyUnicode_FromFormat(
              "\"\\%c\" is an invalid escape sequence. "
              "Such sequences will not work in the future. "
              "Did you mean \"\\\\%c\"? A raw string is also an option.",
              c, c);
    ఒకవేళ (సందేశం == ఖాళీ) {
        తిరిగి -1;
    }
    PyObject *category;
    ఒకవేళ (p->feature_సంస్కరణ >= 12) {
        category = PyExc_SyntaxWarning;
    }
    లేకపోతే {
        category = PyExc_DeprecationWarning;
    }

    // Calculate the lineno and the col_offset of the invalid escape sequence
    స్థిరరాశి అక్షరం *ప్రారంభం = బఫరు;
    స్థిరరాశి అక్షరం *చివర = మొదటి_invalid_escape;
    పూర్ణాంకం lineno = t->lineno;
    పూర్ణాంకం నిలువు_స్థానభ్రంశం = t->నిలువు_స్థానభ్రంశం;
    అయితే (ప్రారంభం < చివర) {
        ఒకవేళ (*ప్రారంభం == '\n') {
            lineno++;
            నిలువు_స్థానభ్రంశం = 0;
        }
        లేకపోతే {
            నిలువు_స్థానభ్రంశం++;
        }
        ప్రారంభం++;
    }

    // Count the number of quotes in the token
    అక్షరం మొదటి_quote = 0;
    ఒకవేళ (lineno == t->lineno) {
        పూర్ణాంకం quote_లెక్క = 0;
        స్థిరరాశి అక్షరం* tok = Py_బైట్లు_As_String(t->బైట్లు);
        కొరకు (పూర్ణాంకం i = 0; i < Py_బైట్లు_కొలత(t->బైట్లు); i++) {
            ఒకవేళ (tok[i] == '\'' || tok[i] == '\"') {
                ఒకవేళ (quote_లెక్క == 0) {
                    మొదటి_quote = tok[i];
                }
                ఒకవేళ (tok[i] == మొదటి_quote) {
                    quote_లెక్క++;
                }
            } లేకపోతే {
                ఆపు;
            }
        }

        నిలువు_స్థానభ్రంశం += quote_లెక్క;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    ఒకవేళ (PyErr_WarnExplicitObject(category, సందేశం, info.filename,
                                 lineno, info.module, ఖాళీ) < 0) {
        ఒకవేళ (PyErr_ExceptionMatches(category)) {
            /* Replace the Syntax/DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            /* This is needed, in order for the SyntaxError to point to the token t,
               since _PyPegen_raise_error uses p->tokens[p->fill - 1] for the
               error location, if p->known_err_token is not set. */
            p->తెలిసిన_err_token = t;
            ఒకవేళ (octal) {
                RAISE_ERROR_తెలిసిన_LOCATION(p, PyExc_SyntaxError, lineno, నిలువు_స్థానభ్రంశం-1, lineno, నిలువు_స్థానభ్రంశం+1,
                    "\"\\%.3s\" is an invalid octal escape sequence. "
                    "Did you mean \"\\\\%.3s\"? A raw string is also an option.",
                    మొదటి_invalid_escape, మొదటి_invalid_escape);
            }
            లేకపోతే {
                RAISE_ERROR_తెలిసిన_LOCATION(p, PyExc_SyntaxError, lineno, నిలువు_స్థానభ్రంశం-1, lineno, నిలువు_స్థానభ్రంశం+1,
                    "\"\\%c\" is an invalid escape sequence. "
                    "Did you mean \"\\\\%c\"? A raw string is also an option.",
                    c, c);
            }
        }
        Py_DECREF(సందేశం);
        తిరిగి -1;
    }
    Py_DECREF(సందేశం);
    తిరిగి 0;
}

స్థిర PyObject *
decode_utf8(స్థిరరాశి అక్షరం **sPtr, స్థిరరాశి అక్షరం *చివర)
{
    స్థిరరాశి అక్షరం *s;
    స్థిరరాశి అక్షరం *t;
    t = s = *sPtr;
    అయితే (s < చివర && (*s & 0x80)) {
        s++;
    }
    *sPtr = s;
    తిరిగి PyUnicode_DecodeUTF8(t, s - t, ఖాళీ);
}

స్థిర PyObject *
decode_unicode_with_escapes(Parser *parser, స్థిరరాశి అక్షరం *s, కొలత_t len, Token *t)
{
    PyObject *v;
    అక్షరం *p;
    స్థిరరాశి అక్షరం *చివర;

    /* check for integer overflow */
    ఒకవేళ (len > (కొలత_t)PY_SSIZE_T_MAX / 6) {
        తిరిగి ఖాళీ;
    }
    /* "ä" (2 bytes) may become "\U000000E4" (10 bytes), or 1:5.
     * "\ä" (3 bytes) may become "\u005c\U000000E4" (16 bytes), or ~1:6. */
    Py_ssize_t alloc = (Py_ssize_t)len * 6;
    అక్షరం *buf = PyMem_Malloc(alloc);
    ఒకవేళ (buf == ఖాళీ) {
        తిరిగి Py_Err_No_జ్ఞాపకం();
    }
    p = buf;
    చివర = s + len;
    అయితే (s < చివర) {
        ఒకవేళ (*s == '\\') {
            *p++ = *s++;
            ఒకవేళ (s >= చివర || *s & 0x80) {
                జ్ఞాపకం_నకలు(p, "u005c", 5);
                p += 5;
                ఒకవేళ (s >= చివర) {
                    ఆపు;
                }
            }
        }
        ఒకవేళ (*s & 0x80) {
            PyObject *w;
            పూర్ణాంకం రకం;
            స్థిరరాశి శూన్య *దత్తాంశం;
            Py_ssize_t w_len;
            Py_ssize_t i;
            w = decode_utf8(&s, చివర);
            ఒకవేళ (w == ఖాళీ) {
                PyMem_Free(buf);
                తిరిగి ఖాళీ;
            }
            రకం = Py_Unicode_రకం(w);
            దత్తాంశం = Py_Unicode_దత్తాంశం(w);
            w_len = Py_Unicode_GET_నిడివి(w);
            కొరకు (i = 0; i < w_len; i++) {
                // sprintf() writes a null byte: the buffer is large enough
                // for that thanks to the overallocation.
                ప్రతిజ్ఞా((p + 11 - buf) <= alloc);
                Py_UCS4 chr = PyUnicode_READ(రకం, దత్తాంశం, i);
                sprintf(p, "\\U%08x", chr);
                p += 10;
            }
            Py_DECREF(w);
        }
        లేకపోతే {
            *p++ = *s++;
        }
    }
    len = (కొలత_t)(p - buf);

    పూర్ణాంకం మొదటి_invalid_escape_char;
    స్థిరరాశి అక్షరం *మొదటి_invalid_escape_ptr;
    v = _PyUnicode_DecodeUnicodeEscapeInternal2(buf, (Py_ssize_t)len, ఖాళీ, ఖాళీ,
                                                &మొదటి_invalid_escape_char,
                                                &మొదటి_invalid_escape_ptr);

    // HACK: later we can simply pass the line no, since we don't preserve the tokens
    // when we are decoding the string but we preserve the line numbers.
    ఒకవేళ (v != ఖాళీ && మొదటి_invalid_escape_ptr != ఖాళీ && t != ఖాళీ) {
        ఒకవేళ (warn_invalid_escape_sequence(parser, buf, మొదటి_invalid_escape_ptr, t) < 0) {
            /* We have not deallocated the buffer before because
             * first_invalid_escape_ptr points inside buf. */
            PyMem_Free(buf);
            Py_DECREF(v);
            తిరిగి ఖాళీ;
        }
    }
    PyMem_Free(buf);
    తిరిగి v;
}

స్థిర PyObject *
decode_బైట్లు_with_escapes(Parser *p, స్థిరరాశి అక్షరం *s, Py_ssize_t len, Token *t)
{
    పూర్ణాంకం మొదటి_invalid_escape_char;
    స్థిరరాశి అక్షరం *మొదటి_invalid_escape_ptr;
    PyObject *ఫలితం = Py_బైట్లు_Decode_Escape2(s, len, ఖాళీ,
                                              &మొదటి_invalid_escape_char,
                                              &మొదటి_invalid_escape_ptr);
    ఒకవేళ (ఫలితం == ఖాళీ) {
        తిరిగి ఖాళీ;
    }

    ఒకవేళ (మొదటి_invalid_escape_ptr != ఖాళీ) {
        ఒకవేళ (warn_invalid_escape_sequence(p, s, మొదటి_invalid_escape_ptr, t) < 0) {
            Py_DECREF(ఫలితం);
            తిరిగి ఖాళీ;
        }
    }
    తిరిగి ఫలితం;
}

PyObject *
_PyPegen_decode_string(Parser *p, పూర్ణాంకం raw, స్థిరరాశి అక్షరం *s, కొలత_t len, Token *t)
{
    ఒకవేళ (raw) {
        తిరిగి PyUnicode_DecodeUTF8Stateful(s, (Py_ssize_t)len, ఖాళీ, ఖాళీ);
    }
    తిరిగి decode_unicode_with_escapes(p, s, len, t);
}

/* s must include the bracketing quote characters, and r, b &/or f prefixes
    (if any), and embedded escape sequences (if any). (f-strings are handled by the parser)
   _PyPegen_parse_string parses it, and returns the decoded Python string object. */
PyObject *
_PyPegen_parse_string(Parser *p, Token *t)
{
    స్థిరరాశి అక్షరం *s = Py_బైట్లు_As_String(t->బైట్లు);
    ఒకవేళ (s == ఖాళీ) {
        తిరిగి ఖాళీ;
    }

    కొలత_t len;
    పూర్ణాంకం quote = Py_CHARMASK(*s);
    పూర్ణాంకం bytesmode = 0;
    పూర్ణాంకం rawmode = 0;

    ఒకవేళ (Py_ISALPHA(quote)) {
        అయితే (!bytesmode || !rawmode) {
            ఒకవేళ (quote == 'b' || quote == 'B') {
                quote =(గుర్తులేని అక్షరం)*++s;
                bytesmode = 1;
            }
            లేకపోతే ఒకవేళ (quote == 'u' || quote == 'U') {
                quote = (గుర్తులేని అక్షరం)*++s;
            }
            లేకపోతే ఒకవేళ (quote == 'r' || quote == 'R') {
                quote = (గుర్తులేని అక్షరం)*++s;
                rawmode = 1;
            }
            లేకపోతే {
                ఆపు;
            }
        }
    }

    ఒకవేళ (quote != '\'' && quote != '\"') {
        Py_Err_Bad_అంతర్గత_Call();
        తిరిగి ఖాళీ;
    }

    /* Skip the leading quote char. */
    s++;
    len = అక్షరశ్రేణి_పొడవు(s);
    // gh-120155: 's' contains at least the trailing quote,
    // so the code '--len' below is safe.
    ప్రతిజ్ఞా(len >= 1);

    ఒకవేళ (len > గరిష్ఠ_పూర్ణాంకం) {
        Py_Err_సమితి_String(Py_Exc_అధికప్రవాహం_Error, "string to parse is too long");
        తిరిగి ఖాళీ;
    }
    ఒకవేళ (s[--len] != quote) {
        /* Last quote char must match the first. */
        Py_Err_Bad_అంతర్గత_Call();
        తిరిగి ఖాళీ;
    }
    ఒకవేళ (len >= 4 && s[0] == quote && s[1] == quote) {
        /* A triple quoted string. We've already skipped one quote at
           the start and one at the end of the string. Now skip the
           two at the start. */
        s += 2;
        len -= 2;
        /* And check that the last two match. */
        ఒకవేళ (s[--len] != quote || s[--len] != quote) {
            Py_Err_Bad_అంతర్గత_Call();
            తిరిగి ఖాళీ;
        }
    }

    /* Avoid invoking escape decoding routines if possible. */
    rawmode = rawmode || strchr(s, '\\') == ఖాళీ;
    ఒకవేళ (bytesmode) {
        /* Disallow non-ASCII characters. */
        స్థిరరాశి అక్షరం *ch;
        కొరకు (ch = s; *ch; ch++) {
            ఒకవేళ (Py_CHARMASK(*ch) >= 0x80) {
                RAISE_SYNTAX_ERROR_తెలిసిన_LOCATION(
                                   t,
                                   "bytes can only contain ASCII "
                                   "literal characters");
                తిరిగి ఖాళీ;
            }
        }
        ఒకవేళ (rawmode) {
            తిరిగి Py_బైట్లు_From_String_And_కొలత(s, (Py_ssize_t)len);
        }
        తిరిగి decode_బైట్లు_with_escapes(p, s, (Py_ssize_t)len, t);
    }
    తిరిగి _PyPegen_decode_string(p, rawmode, s, len, t);
}
