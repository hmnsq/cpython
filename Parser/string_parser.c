#अन्तर्भाव <Python.h>
#अन्तर्भाव "pycore_bytesobject.h"   // _PyBytes_DecodeEscape()
#अन्तर्भाव "pycore_unicodeobject.h" // _PyUnicode_DecodeUnicodeEscapeInternal()

#अन्तर्भाव "pegen.h"
#अन्तर्भाव "string_parser.h"

#अन्तर्भाव <stdbool.h>

//// STRING HANDLING FUNCTIONS ////

स्थिर पूर्णाङ्क
warn_invalid_escape_sequence(Parser *p, ध्रुव वर्ण* बफर, ध्रुव वर्ण *first_invalid_escape, Token *t)
{
    यदि (p->call_invalid_नियमाः) {
        // Do not report warnings if we are in the second pass of the parser
        // to avoid showing the warning twice.
        प्रत्यागम 0;
    }
    अचिह्नित वर्ण c = (अचिह्नित वर्ण)*first_invalid_escape;
    यदि ((t->type == FSTRING_मध्यः || t->type == FSTRING_अन्तः || t->type == TSTRING_मध्यः || t->type == TSTRING_अन्तः)
            && (c == '{' || c == '}')) {
        // in this case the tokenizer has already emitted a warning,
        // see Parser/tokenizer/helpers.c:warn_invalid_escape_sequence
        प्रत्यागम 0;
    }

    पूर्णाङ्क octal = ('4' <= c && c <= '7');
    PyObject *सन्देशः =
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
    यदि (सन्देशः == रिक्त) {
        प्रत्यागम -1;
    }
    PyObject *category;
    यदि (p->feature_संस्करणम् >= 12) {
        category = PyExc_SyntaxWarning;
    }
    अन्यथा {
        category = PyExc_DeprecationWarning;
    }

    // Calculate the lineno and the col_offset of the invalid escape sequence
    ध्रुव वर्ण *start = बफर;
    ध्रुव वर्ण *अन्तः_2 = first_invalid_escape;
    पूर्णाङ्क lineno = t->lineno;
    पूर्णाङ्क col_विस्थापनम् = t->col_विस्थापनम्;
    यावत् (start < अन्तः_2) {
        यदि (*start == '\n') {
            lineno++;
            col_विस्थापनम् = 0;
        }
        अन्यथा {
            col_विस्थापनम्++;
        }
        start++;
    }

    // Count the number of quotes in the token
    वर्ण first_quote = 0;
    यदि (lineno == t->lineno) {
        पूर्णाङ्क quote_गणनम् = 0;
        ध्रुव वर्ण* tok = Py_अष्टकानि_As_String(t->अष्टकानि);
        प्रति (पूर्णाङ्क i = 0; i < Py_अष्टकानि_आकार(t->अष्टकानि); i++) {
            यदि (tok[i] == '\'' || tok[i] == '\"') {
                यदि (quote_गणनम् == 0) {
                    first_quote = tok[i];
                }
                यदि (tok[i] == first_quote) {
                    quote_गणनम्++;
                }
            } अन्यथा {
                भङ्ग;
            }
        }

        col_विस्थापनम् += quote_गणनम्;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    यदि (PyErr_WarnExplicitObject(category, सन्देशः, info.filename,
                                 lineno, info.module, रिक्त) < 0) {
        यदि (PyErr_ExceptionMatches(category)) {
            /* Replace the Syntax/DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            /* This is needed, in order for the SyntaxError to point to the token t,
               since _PyPegen_raise_error uses p->tokens[p->fill - 1] for the
               error location, if p->known_err_token is not set. */
            p->ज्ञातम्_err_token = t;
            यदि (octal) {
                RAISE_ERROR_ज्ञातम्_LOCATION(p, PyExc_SyntaxError, lineno, col_विस्थापनम्-1, lineno, col_विस्थापनम्+1,
                    "\"\\%.3s\" is an invalid octal escape sequence. "
                    "Did you mean \"\\\\%.3s\"? A raw string is also an option.",
                    first_invalid_escape, first_invalid_escape);
            }
            अन्यथा {
                RAISE_ERROR_ज्ञातम्_LOCATION(p, PyExc_SyntaxError, lineno, col_विस्थापनम्-1, lineno, col_विस्थापनम्+1,
                    "\"\\%c\" is an invalid escape sequence. "
                    "Did you mean \"\\\\%c\"? A raw string is also an option.",
                    c, c);
            }
        }
        Py_DECREF(सन्देशः);
        प्रत्यागम -1;
    }
    Py_DECREF(सन्देशः);
    प्रत्यागम 0;
}

स्थिर PyObject *
decode_utf8(ध्रुव वर्ण **sPtr, ध्रुव वर्ण *अन्तः_2)
{
    ध्रुव वर्ण *s;
    ध्रुव वर्ण *t;
    t = s = *sPtr;
    यावत् (s < अन्तः_2 && (*s & 0x80)) {
        s++;
    }
    *sPtr = s;
    प्रत्यागम PyUnicode_DecodeUTF8(t, s - t, रिक्त);
}

स्थिर PyObject *
decode_unicode_with_escapes(Parser *parser, ध्रुव वर्ण *s, आकार_t len, Token *t)
{
    PyObject *v;
    वर्ण *p;
    ध्रुव वर्ण *अन्तः_2;

    /* check for integer overflow */
    यदि (len > (आकार_t)PY_SSIZE_T_MAX / 6) {
        प्रत्यागम रिक्त;
    }
    /* "ä" (2 bytes) may become "\U000000E4" (10 bytes), or 1:5.
     * "\ä" (3 bytes) may become "\u005c\U000000E4" (16 bytes), or ~1:6. */
    Py_ssize_t alloc = (Py_ssize_t)len * 6;
    वर्ण *buf = PyMem_Malloc(alloc);
    यदि (buf == रिक्त) {
        प्रत्यागम Py_Err_No_स्मृतिः();
    }
    p = buf;
    अन्तः_2 = s + len;
    यावत् (s < अन्तः_2) {
        यदि (*s == '\\') {
            *p++ = *s++;
            यदि (s >= अन्तः_2 || *s & 0x80) {
                स्मृतिं_प्रतिलिख(p, "u005c", 5);
                p += 5;
                यदि (s >= अन्तः_2) {
                    भङ्ग;
                }
            }
        }
        यदि (*s & 0x80) {
            PyObject *w;
            पूर्णाङ्क विधा;
            ध्रुव शून्य *दत्त;
            Py_ssize_t w_len;
            Py_ssize_t i;
            w = decode_utf8(&s, अन्तः_2);
            यदि (w == रिक्त) {
                PyMem_Free(buf);
                प्रत्यागम रिक्त;
            }
            विधा = Py_Unicode_विधा(w);
            दत्त = Py_Unicode_दत्त(w);
            w_len = Py_Unicode_GET_दीर्घता(w);
            प्रति (i = 0; i < w_len; i++) {
                // sprintf() writes a null byte: the buffer is large enough
                // for that thanks to the overallocation.
                प्रतिज्ञा((p + 11 - buf) <= alloc);
                Py_UCS4 chr = PyUnicode_READ(विधा, दत्त, i);
                sprintf(p, "\\U%08x", chr);
                p += 10;
            }
            Py_DECREF(w);
        }
        अन्यथा {
            *p++ = *s++;
        }
    }
    len = (आकार_t)(p - buf);

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
decode_अष्टकानि_with_escapes(Parser *p, ध्रुव वर्ण *s, Py_ssize_t len, Token *t)
{
    पूर्णाङ्क first_invalid_escape_char;
    ध्रुव वर्ण *first_invalid_escape_ptr;
    PyObject *परिणाम = Py_अष्टकानि_Decode_Escape2(s, len, रिक्त,
                                              &first_invalid_escape_char,
                                              &first_invalid_escape_ptr);
    यदि (परिणाम == रिक्त) {
        प्रत्यागम रिक्त;
    }

    यदि (first_invalid_escape_ptr != रिक्त) {
        यदि (warn_invalid_escape_sequence(p, s, first_invalid_escape_ptr, t) < 0) {
            Py_DECREF(परिणाम);
            प्रत्यागम रिक्त;
        }
    }
    प्रत्यागम परिणाम;
}

PyObject *
_PyPegen_decode_string(Parser *p, पूर्णाङ्क raw, ध्रुव वर्ण *s, आकार_t len, Token *t)
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
    ध्रुव वर्ण *s = Py_अष्टकानि_As_String(t->अष्टकानि);
    यदि (s == रिक्त) {
        प्रत्यागम रिक्त;
    }

    आकार_t len;
    पूर्णाङ्क quote = Py_CHARMASK(*s);
    पूर्णाङ्क bytesmode = 0;
    पूर्णाङ्क rawmode = 0;

    यदि (Py_ISALPHA(quote)) {
        यावत् (!bytesmode || !rawmode) {
            यदि (quote == 'b' || quote == 'B') {
                quote =(अचिह्नित वर्ण)*++s;
                bytesmode = 1;
            }
            अन्यथा यदि (quote == 'u' || quote == 'U') {
                quote = (अचिह्नित वर्ण)*++s;
            }
            अन्यथा यदि (quote == 'r' || quote == 'R') {
                quote = (अचिह्नित वर्ण)*++s;
                rawmode = 1;
            }
            अन्यथा {
                भङ्ग;
            }
        }
    }

    यदि (quote != '\'' && quote != '\"') {
        Py_Err_Bad_आन्तरिकम्_Call();
        प्रत्यागम रिक्त;
    }

    /* Skip the leading quote char. */
    s++;
    len = वाक्यदैर्घ्यम्(s);
    // gh-120155: 's' contains at least the trailing quote,
    // so the code '--len' below is safe.
    प्रतिज्ञा(len >= 1);

    यदि (len > अधिकतम_पूर्णाङ्कः) {
        Py_Err_समुच्चयः_String(Py_Exc_अतिप्रवाहः_Error, "string to parse is too long");
        प्रत्यागम रिक्त;
    }
    यदि (s[--len] != quote) {
        /* Last quote char must match the first. */
        Py_Err_Bad_आन्तरिकम्_Call();
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
            Py_Err_Bad_आन्तरिकम्_Call();
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
                RAISE_SYNTAX_ERROR_ज्ञातम्_LOCATION(
                                   t,
                                   "bytes can only contain ASCII "
                                   "literal characters");
                प्रत्यागम रिक्त;
            }
        }
        यदि (rawmode) {
            प्रत्यागम Py_अष्टकानि_From_String_And_आकार(s, (Py_ssize_t)len);
        }
        प्रत्यागम decode_अष्टकानि_with_escapes(p, s, (Py_ssize_t)len, t);
    }
    प्रत्यागम _PyPegen_decode_string(p, rawmode, s, len, t);
}
