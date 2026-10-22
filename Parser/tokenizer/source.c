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
    जर (source->bytes != रिक्त) {
        source->bytes[0] = '\0';
    }
    source->nlines = 0;
}

स्थिर पूर्णांक
reserve_bytes(_PyTok_SourceText *source, Py_ssize_t needed)
{
    जर (needed <= source->cap) {
        परत 0;
    }
    Py_ssize_t cap = source->cap > 0 ? source->cap : बफ_मान;
    पर्यंत (cap < needed) {
        जर (cap > PY_SSIZE_T_MAX / SOURCE_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            थांबा;
        }
        cap *= SOURCE_BUFFER_GROWTH_FACTOR;
    }
    अक्षर *bytes;
#ifdef Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    bytes = PyMem_Malloc(cap);
    जर (bytes != रिक्त && source->len > 0) {
        स_नकल(bytes, source->bytes, source->len);
    }
#अन्यथा
    bytes = PyMem_Realloc(source->bytes, cap);
#endif
    जर (bytes == रिक्त) {
        PyErr_NoMemory();
        परत -1;
    }
#ifdef Py_DEBUG
    जर (source->bytes != रिक्त) {
        स_रखो(source->bytes, PYMEM_DEADBYTE, source->cap);
        PyMem_Free(source->bytes);
    }
#endif
    source->bytes = bytes;
    source->cap = cap;
    परत 0;
}

स्थिर पूर्णांक
validate_ओळ(स्थिरांक _PyTok_SourceText *source, स्थिरांक अक्षर *bytes,
              Py_ssize_t len, पूर्णांक append_newline)
{
    जर (len < 0 || (len == 0 && !append_newline) || bytes == रिक्त ||
            (source->nlines > 0 &&
             (source->len == 0 || source->bytes[source->len - 1] != '\n'))) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        परत -1;
    }
    स्थिरांक अक्षर *newline = स_प्रथम(bytes, '\n', len);
    जर (newline != रिक्त && (append_newline || newline != bytes + len - 1)) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        परत -1;
    }
    जर (source->nlines == पूर्णांक_उच्च ||
            (source->nlines == पूर्णांक_उच्च - 1 && (newline != रिक्त || append_newline))) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        परत -1;
    }
    परत 0;
}

स्थिर _PyTok_Off
append_ओळ(_PyTok_SourceText *source, स्थिरांक अक्षर *bytes,
            Py_ssize_t len, पूर्णांक append_newline)
{
    जर (validate_ओळ(source, bytes, len, append_newline) < 0) {
        परत -1;
    }
    जर (source->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            source->base_offset > PY_SSIZE_T_MAX - source->len - len - append_newline) {
        PyErr_NoMemory();
        परत -1;
    }
    जर (reserve_bytes(source, source->len + len + append_newline + 1) < 0) {
        परत -1;
    }

    _PyTok_Off start = source->len;
    स_नकल(source->bytes + start, bytes, len);
    source->len += len;
    जर (append_newline) {
        source->bytes[source->len++] = '\n';
    }
    source->bytes[source->len] = '\0';
    source->nlines++;
    परत source->base_offset + start;
}

_PyTok_Off
Py_Tok_Source_Append_ओळ(_PyTok_SourceText *source, स्थिरांक अक्षर *bytes,
                        Py_ssize_t len)
{
    परत append_ओळ(source, bytes, len, 0);
}

_PyTok_Off
Py_Tok_Source_Append_ओळ_With_Newline(_PyTok_SourceText *source, स्थिरांक अक्षर *bytes,
                                   Py_ssize_t len)
{
    परत append_ओळ(source, bytes, len, 1);
}

स्थिरांक अक्षर *
Py_Tok_Source_ओळ_View(स्थिरांक _PyTok_SourceText *source, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    प्रतिज्ञा(len != रिक्त);
    स्थिरांक अक्षर *ओळ = _PyTok_SourceData(source);
    स्थिरांक अक्षर *end = ओळ + source->len;
    पर्यंत (lineno > 1) {
        स्थिरांक अक्षर *newline = स_प्रथम(ओळ, '\n', end - ओळ);
        जर (newline == रिक्त) {
            थांबा;
        }
        ओळ = newline + 1;
        lineno--;
    }
    स्थिरांक अक्षर *newline = स_प्रथम(ओळ, '\n', end - ओळ);
    *len = (newline != रिक्त ? newline : end) - ओळ;
    परत ओळ;
}
