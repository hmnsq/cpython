#अन्तर्भाव "Python.h"
#अन्तर्भाव "pycore_pymem.h"  // PYMEM_DEADBYTE

#अन्तर्भाव "source.h"

#define SOURCE_BUFFER_GROWTH_FACTOR 2

शून्य
_PyTok_SourceInit(_PyTok_SourceText *source)
{
    *source = (_PyTok_SourceText){0};
}

शून्य
_PyTok_SourceClear(_PyTok_SourceText *source)
{
    PyMem_Free(source->bytes);
    _PyTok_SourceInit(source);
}

शून्य
_PyTok_SourceDiscard(_PyTok_SourceText *source)
{
    प्रतिज्ञा(source->base_offset <= PY_SSIZE_T_MAX - source->len);
    source->base_offset += source->len;
    source->len = 0;
    जइ (source->bytes != रिक्त) {
        source->bytes[0] = '\0';
    }
    source->nlines = 0;
}

स्थिर पूर्णाङ्क
reserve_bytes(_PyTok_SourceText *source, Py_ssize_t needed)
{
    जइ (needed <= source->cap) {
        प्रत्यागम 0;
    }
    Py_ssize_t cap = source->cap > 0 ? source->cap : BUFSIZ;
    जाव (cap < needed) {
        जइ (cap > PY_SSIZE_T_MAX / SOURCE_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            भङ्ग;
        }
        cap *= SOURCE_BUFFER_GROWTH_FACTOR;
    }
    वर्ण *bytes;
#ifdef Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    bytes = PyMem_Malloc(cap);
    जइ (bytes != रिक्त && source->len > 0) {
        memcpy(bytes, source->bytes, source->len);
    }
#अण्णहा
    bytes = PyMem_Realloc(source->bytes, cap);
#endif
    जइ (bytes == रिक्त) {
        PyErr_NoMemory();
        प्रत्यागम -1;
    }
#ifdef Py_DEBUG
    जइ (source->bytes != रिक्त) {
        memset(source->bytes, PYMEM_DEADBYTE, source->cap);
        PyMem_Free(source->bytes);
    }
#endif
    source->bytes = bytes;
    source->cap = cap;
    प्रत्यागम 0;
}

स्थिर पूर्णाङ्क
validate_पङ्क्तिः(ध्रुव _PyTok_SourceText *source, ध्रुव वर्ण *bytes,
              Py_ssize_t len, पूर्णाङ्क append_newline)
{
    जइ (len < 0 || (len == 0 && !append_newline) || bytes == रिक्त ||
            (source->nlines > 0 &&
             (source->len == 0 || source->bytes[source->len - 1] != '\n'))) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        प्रत्यागम -1;
    }
    ध्रुव वर्ण *newline = memchr(bytes, '\n', len);
    जइ (newline != रिक्त && (append_newline || newline != bytes + len - 1)) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        प्रत्यागम -1;
    }
    जइ (source->nlines == INT_MAX ||
            (source->nlines == INT_MAX - 1 && (newline != रिक्त || append_newline))) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        प्रत्यागम -1;
    }
    प्रत्यागम 0;
}

स्थिर _PyTok_Off
append_पङ्क्तिः(_PyTok_SourceText *source, ध्रुव वर्ण *bytes,
            Py_ssize_t len, पूर्णाङ्क append_newline)
{
    जइ (validate_पङ्क्तिः(source, bytes, len, append_newline) < 0) {
        प्रत्यागम -1;
    }
    जइ (source->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            source->base_offset > PY_SSIZE_T_MAX - source->len - len - append_newline) {
        PyErr_NoMemory();
        प्रत्यागम -1;
    }
    जइ (reserve_bytes(source, source->len + len + append_newline + 1) < 0) {
        प्रत्यागम -1;
    }

    _PyTok_Off start = source->len;
    memcpy(source->bytes + start, bytes, len);
    source->len += len;
    जइ (append_newline) {
        source->bytes[source->len++] = '\n';
    }
    source->bytes[source->len] = '\0';
    source->nlines++;
    प्रत्यागम source->base_offset + start;
}

_PyTok_Off
Py_Tok_Source_Append_पङ्क्तिः(_PyTok_SourceText *source, ध्रुव वर्ण *bytes,
                        Py_ssize_t len)
{
    प्रत्यागम append_पङ्क्तिः(source, bytes, len, 0);
}

_PyTok_Off
Py_Tok_Source_Append_पङ्क्तिः_With_Newline(_PyTok_SourceText *source, ध्रुव वर्ण *bytes,
                                   Py_ssize_t len)
{
    प्रत्यागम append_पङ्क्तिः(source, bytes, len, 1);
}

ध्रुव वर्ण *
Py_Tok_Source_पङ्क्तिः_View(ध्रुव _PyTok_SourceText *source, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    प्रतिज्ञा(len != रिक्त);
    ध्रुव वर्ण *पङ्क्तिः = _PyTok_SourceData(source);
    ध्रुव वर्ण *end = पङ्क्तिः + source->len;
    जाव (lineno > 1) {
        ध्रुव वर्ण *newline = memchr(पङ्क्तिः, '\n', end - पङ्क्तिः);
        जइ (newline == रिक्त) {
            भङ्ग;
        }
        पङ्क्तिः = newline + 1;
        lineno--;
    }
    ध्रुव वर्ण *newline = memchr(पङ्क्तिः, '\n', end - पङ्क्तिः);
    *len = (newline != रिक्त ? newline : end) - पङ्क्तिः;
    प्रत्यागम पङ्क्तिः;
}
