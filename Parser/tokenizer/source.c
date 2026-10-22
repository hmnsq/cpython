#अन्तर्भाव "Python.h"
#अन्तर्भाव "pycore_pymem.h"  // PYMEM_DEADBYTE

#अन्तर्भाव "source.h"

#परिभाषय SOURCE_बफर_GROWTH_गुणकः 2

शून्य
_PyTok_SourceInit(_PyTok_SourceText *source)
{
    *source = (_PyTok_SourceText){0};
}

शून्य
_PyTok_SourceClear(_PyTok_SourceText *source)
{
    PyMem_Free(source->अष्टकानि);
    _PyTok_SourceInit(source);
}

शून्य
_PyTok_SourceDiscard(_PyTok_SourceText *source)
{
    प्रतिज्ञा(source->आधारः_विस्थापनम् <= PY_SSIZE_T_MAX - source->len);
    source->आधारः_विस्थापनम् += source->len;
    source->len = 0;
    यदि (source->अष्टकानि != रिक्त) {
        source->अष्टकानि[0] = '\0';
    }
    source->nlines = 0;
}

स्थिर पूर्णाङ्क
reserve_अष्टकानि(_PyTok_SourceText *source, Py_ssize_t needed)
{
    यदि (needed <= source->cap) {
        प्रत्यागम 0;
    }
    Py_ssize_t cap = source->cap > 0 ? source->cap : BUFSIZ;
    यावत् (cap < needed) {
        यदि (cap > PY_SSIZE_T_MAX / SOURCE_बफर_GROWTH_गुणकः) {
            cap = needed;
            भङ्ग;
        }
        cap *= SOURCE_बफर_GROWTH_गुणकः;
    }
    वर्ण *अष्टकानि;
#यदि_परिभाषितम् Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    अष्टकानि = PyMem_Malloc(cap);
    यदि (अष्टकानि != रिक्त && source->len > 0) {
        स्मृतिं_प्रतिलिख(अष्टकानि, source->अष्टकानि, source->len);
    }
#अन्यथा
    अष्टकानि = PyMem_Realloc(source->अष्टकानि, cap);
#यदि_अन्तः
    यदि (अष्टकानि == रिक्त) {
        Py_Err_No_स्मृतिः();
        प्रत्यागम -1;
    }
#यदि_परिभाषितम् Py_DEBUG
    यदि (source->अष्टकानि != रिक्त) {
        स्मृतिं_पूरय(source->अष्टकानि, PYMEM_DEADBYTE, source->cap);
        PyMem_Free(source->अष्टकानि);
    }
#यदि_अन्तः
    source->अष्टकानि = अष्टकानि;
    source->cap = cap;
    प्रत्यागम 0;
}

स्थिर पूर्णाङ्क
validate_पङ्क्तिः(ध्रुव _PyTok_SourceText *source, ध्रुव वर्ण *अष्टकानि,
              Py_ssize_t len, पूर्णाङ्क append_newline)
{
    यदि (len < 0 || (len == 0 && !append_newline) || अष्टकानि == रिक्त ||
            (source->nlines > 0 &&
             (source->len == 0 || source->अष्टकानि[source->len - 1] != '\n'))) {
        Py_Err_समुच्चयः_String(PyExc_SystemError, "invalid tokenizer source line");
        प्रत्यागम -1;
    }
    ध्रुव वर्ण *newline = memchr(अष्टकानि, '\n', len);
    यदि (newline != रिक्त && (append_newline || newline != अष्टकानि + len - 1)) {
        Py_Err_समुच्चयः_String(PyExc_SystemError, "invalid tokenizer source line");
        प्रत्यागम -1;
    }
    यदि (source->nlines == अधिकतम_पूर्णाङ्कः ||
            (source->nlines == अधिकतम_पूर्णाङ्कः - 1 && (newline != रिक्त || append_newline))) {
        Py_Err_समुच्चयः_String(Py_Exc_अतिप्रवाहः_Error, "too many tokenizer source lines");
        प्रत्यागम -1;
    }
    प्रत्यागम 0;
}

स्थिर _PyTok_Off
append_पङ्क्तिः(_PyTok_SourceText *source, ध्रुव वर्ण *अष्टकानि,
            Py_ssize_t len, पूर्णाङ्क append_newline)
{
    यदि (validate_पङ्क्तिः(source, अष्टकानि, len, append_newline) < 0) {
        प्रत्यागम -1;
    }
    यदि (source->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            source->आधारः_विस्थापनम् > PY_SSIZE_T_MAX - source->len - len - append_newline) {
        Py_Err_No_स्मृतिः();
        प्रत्यागम -1;
    }
    यदि (reserve_अष्टकानि(source, source->len + len + append_newline + 1) < 0) {
        प्रत्यागम -1;
    }

    _PyTok_Off start = source->len;
    स्मृतिं_प्रतिलिख(source->अष्टकानि + start, अष्टकानि, len);
    source->len += len;
    यदि (append_newline) {
        source->अष्टकानि[source->len++] = '\n';
    }
    source->अष्टकानि[source->len] = '\0';
    source->nlines++;
    प्रत्यागम source->आधारः_विस्थापनम् + start;
}

_PyTok_Off
Py_Tok_Source_Append_पङ्क्तिः(_PyTok_SourceText *source, ध्रुव वर्ण *अष्टकानि,
                        Py_ssize_t len)
{
    प्रत्यागम append_पङ्क्तिः(source, अष्टकानि, len, 0);
}

_PyTok_Off
Py_Tok_Source_Append_पङ्क्तिः_With_Newline(_PyTok_SourceText *source, ध्रुव वर्ण *अष्टकानि,
                                   Py_ssize_t len)
{
    प्रत्यागम append_पङ्क्तिः(source, अष्टकानि, len, 1);
}

ध्रुव वर्ण *
Py_Tok_Source_पङ्क्तिः_View(ध्रुव _PyTok_SourceText *source, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    प्रतिज्ञा(len != रिक्त);
    ध्रुव वर्ण *पङ्क्तिः = Py_Tok_Source_दत्त(source);
    ध्रुव वर्ण *अन्तः_2 = पङ्क्तिः + source->len;
    यावत् (lineno > 1) {
        ध्रुव वर्ण *newline = memchr(पङ्क्तिः, '\n', अन्तः_2 - पङ्क्तिः);
        यदि (newline == रिक्त) {
            भङ्ग;
        }
        पङ्क्तिः = newline + 1;
        lineno--;
    }
    ध्रुव वर्ण *newline = memchr(पङ्क्तिः, '\n', अन्तः_2 - पङ्क्तिः);
    *len = (newline != रिक्त ? newline : अन्तः_2) - पङ्क्तिः;
    प्रत्यागम पङ्क्तिः;
}
