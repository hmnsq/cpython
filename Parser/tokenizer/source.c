#شامل "Python.h"
#شامل "pycore_pymem.h"  // PYMEM_DEADBYTE

#شامل "source.h"

#define SOURCE_BUFFER_GROWTH_FACTOR 2

تهی
_PyTok_SourceInit(_PyTok_SourceText *source)
{
    *source = (_PyTok_SourceText){0};
}

تهی
_PyTok_SourceClear(_PyTok_SourceText *source)
{
    PyMem_Free(source->bytes);
    _PyTok_SourceInit(source);
}

تهی
_PyTok_SourceDiscard(_PyTok_SourceText *source)
{
    أكد(source->base_offset <= PY_SSIZE_T_MAX - source->len);
    source->base_offset += source->len;
    source->len = 0;
    اگر (source->bytes != پوچ) {
        source->bytes[0] = '\0';
    }
    source->nlines = 0;
}

ایستا صحیح
reserve_bytes(_PyTok_SourceText *source, Py_ssize_t needed)
{
    اگر (needed <= source->cap) {
        بازگشت 0;
    }
    Py_ssize_t cap = source->cap > 0 ? source->cap : BUFSIZ;
    وقتی (cap < needed) {
        اگر (cap > PY_SSIZE_T_MAX / SOURCE_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            بشکن;
        }
        cap *= SOURCE_BUFFER_GROWTH_FACTOR;
    }
    نویسه *bytes;
#ifdef Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    bytes = PyMem_Malloc(cap);
    اگر (bytes != پوچ && source->len > 0) {
        memcpy(bytes, source->bytes, source->len);
    }
#وگرنه
    bytes = PyMem_Realloc(source->bytes, cap);
#endif
    اگر (bytes == پوچ) {
        PyErr_NoMemory();
        بازگشت -1;
    }
#ifdef Py_DEBUG
    اگر (source->bytes != پوچ) {
        memset(source->bytes, PYMEM_DEADBYTE, source->cap);
        PyMem_Free(source->bytes);
    }
#endif
    source->bytes = bytes;
    source->cap = cap;
    بازگشت 0;
}

ایستا صحیح
validate_سطر(ثابت _PyTok_SourceText *source, ثابت نویسه *bytes,
              Py_ssize_t len, صحیح append_newline)
{
    اگر (len < 0 || (len == 0 && !append_newline) || bytes == پوچ ||
            (source->nlines > 0 &&
             (source->len == 0 || source->bytes[source->len - 1] != '\n'))) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        بازگشت -1;
    }
    ثابت نویسه *newline = memchr(bytes, '\n', len);
    اگر (newline != پوچ && (append_newline || newline != bytes + len - 1)) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        بازگشت -1;
    }
    اگر (source->nlines == INT_MAX ||
            (source->nlines == INT_MAX - 1 && (newline != پوچ || append_newline))) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        بازگشت -1;
    }
    بازگشت 0;
}

ایستا _PyTok_Off
append_سطر(_PyTok_SourceText *source, ثابت نویسه *bytes,
            Py_ssize_t len, صحیح append_newline)
{
    اگر (validate_سطر(source, bytes, len, append_newline) < 0) {
        بازگشت -1;
    }
    اگر (source->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            source->base_offset > PY_SSIZE_T_MAX - source->len - len - append_newline) {
        PyErr_NoMemory();
        بازگشت -1;
    }
    اگر (reserve_bytes(source, source->len + len + append_newline + 1) < 0) {
        بازگشت -1;
    }

    _PyTok_Off start = source->len;
    memcpy(source->bytes + start, bytes, len);
    source->len += len;
    اگر (append_newline) {
        source->bytes[source->len++] = '\n';
    }
    source->bytes[source->len] = '\0';
    source->nlines++;
    بازگشت source->base_offset + start;
}

_PyTok_Off
Py_Tok_Source_Append_سطر(_PyTok_SourceText *source, ثابت نویسه *bytes,
                        Py_ssize_t len)
{
    بازگشت append_سطر(source, bytes, len, 0);
}

_PyTok_Off
Py_Tok_Source_Append_سطر_With_Newline(_PyTok_SourceText *source, ثابت نویسه *bytes,
                                   Py_ssize_t len)
{
    بازگشت append_سطر(source, bytes, len, 1);
}

ثابت نویسه *
Py_Tok_Source_سطر_View(ثابت _PyTok_SourceText *source, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    أكد(len != پوچ);
    ثابت نویسه *سطر = _PyTok_SourceData(source);
    ثابت نویسه *end = سطر + source->len;
    وقتی (lineno > 1) {
        ثابت نویسه *newline = memchr(سطر, '\n', end - سطر);
        اگر (newline == پوچ) {
            بشکن;
        }
        سطر = newline + 1;
        lineno--;
    }
    ثابت نویسه *newline = memchr(سطر, '\n', end - سطر);
    *len = (newline != پوچ ? newline : end) - سطر;
    بازگشت سطر;
}
