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
    सचे (source->bytes != रिक्त) {
        source->bytes[0] = '\0';
    }
    source->nlines = 0;
}

स्थिर पूर्णाङ्क
reserve_bytes(_PyTok_SourceText *source, Py_ssize_t needed)
{
    सचे (needed <= source->cap) {
        पटिनिवत्त 0;
    }
    Py_ssize_t cap = source->cap > 0 ? source->cap : BUFSIZ;
    याव (cap < needed) {
        सचे (cap > PY_SSIZE_T_MAX / SOURCE_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            भङ्ग;
        }
        cap *= SOURCE_BUFFER_GROWTH_FACTOR;
    }
    वर्ण *bytes;
#ifdef Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    bytes = PyMem_Malloc(cap);
    सचे (bytes != रिक्त && source->len > 0) {
        memcpy(bytes, source->bytes, source->len);
    }
#अञ्ञथा
    bytes = PyMem_Realloc(source->bytes, cap);
#endif
    सचे (bytes == रिक्त) {
        PyErr_NoMemory();
        पटिनिवत्त -1;
    }
#ifdef Py_DEBUG
    सचे (source->bytes != रिक्त) {
        memset(source->bytes, PYMEM_DEADBYTE, source->cap);
        PyMem_Free(source->bytes);
    }
#endif
    source->bytes = bytes;
    source->cap = cap;
    पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क
validate_पङ्क्तिः(ध्रुव _PyTok_SourceText *source, ध्रुव वर्ण *bytes,
              Py_ssize_t len, पूर्णाङ्क append_newline)
{
    सचे (len < 0 || (len == 0 && !append_newline) || bytes == रिक्त ||
            (source->nlines > 0 &&
             (source->len == 0 || source->bytes[source->len - 1] != '\n'))) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        पटिनिवत्त -1;
    }
    ध्रुव वर्ण *newline = memchr(bytes, '\n', len);
    सचे (newline != रिक्त && (append_newline || newline != bytes + len - 1)) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        पटिनिवत्त -1;
    }
    सचे (source->nlines == INT_MAX ||
            (source->nlines == INT_MAX - 1 && (newline != रिक्त || append_newline))) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        पटिनिवत्त -1;
    }
    पटिनिवत्त 0;
}

स्थिर _PyTok_Off
append_पङ्क्तिः(_PyTok_SourceText *source, ध्रुव वर्ण *bytes,
            Py_ssize_t len, पूर्णाङ्क append_newline)
{
    सचे (validate_पङ्क्तिः(source, bytes, len, append_newline) < 0) {
        पटिनिवत्त -1;
    }
    सचे (source->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            source->base_offset > PY_SSIZE_T_MAX - source->len - len - append_newline) {
        PyErr_NoMemory();
        पटिनिवत्त -1;
    }
    सचे (reserve_bytes(source, source->len + len + append_newline + 1) < 0) {
        पटिनिवत्त -1;
    }

    _PyTok_Off start = source->len;
    memcpy(source->bytes + start, bytes, len);
    source->len += len;
    सचे (append_newline) {
        source->bytes[source->len++] = '\n';
    }
    source->bytes[source->len] = '\0';
    source->nlines++;
    पटिनिवत्त source->base_offset + start;
}

_PyTok_Off
Py_Tok_Source_Append_पङ्क्तिः(_PyTok_SourceText *source, ध्रुव वर्ण *bytes,
                        Py_ssize_t len)
{
    पटिनिवत्त append_पङ्क्तिः(source, bytes, len, 0);
}

_PyTok_Off
Py_Tok_Source_Append_पङ्क्तिः_With_Newline(_PyTok_SourceText *source, ध्रुव वर्ण *bytes,
                                   Py_ssize_t len)
{
    पटिनिवत्त append_पङ्क्तिः(source, bytes, len, 1);
}

ध्रुव वर्ण *
Py_Tok_Source_पङ्क्तिः_View(ध्रुव _PyTok_SourceText *source, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    प्रतिज्ञा(len != रिक्त);
    ध्रुव वर्ण *पङ्क्तिः = _PyTok_SourceData(source);
    ध्रुव वर्ण *end = पङ्क्तिः + source->len;
    याव (lineno > 1) {
        ध्रुव वर्ण *newline = memchr(पङ्क्तिः, '\n', end - पङ्क्तिः);
        सचे (newline == रिक्त) {
            भङ्ग;
        }
        पङ्क्तिः = newline + 1;
        lineno--;
    }
    ध्रुव वर्ण *newline = memchr(पङ्क्तिः, '\n', end - पङ्क्तिः);
    *len = (newline != रिक्त ? newline : end) - पङ्क्तिः;
    पटिनिवत्त पङ्क्तिः;
}
