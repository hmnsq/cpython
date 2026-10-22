#include "Python.h"
#include "pycore_pymem.h"  // PYMEM_DEADBYTE

#include "source.h"

#define SOURCE_BUFFER_GROWTH_FACTOR 2

void
_PyTok_SourceInit(_PyTok_SourceText *source)
{
    *source = (_PyTok_SourceText){0};
}

void
_PyTok_SourceClear(_PyTok_SourceText *source)
{
    PyMem_Free(source->bytes);
    _PyTok_SourceInit(source);
}

void
_PyTok_SourceDiscard(_PyTok_SourceText *source)
{
    assert(source->base_offset <= PY_SSIZE_T_MAX - source->len);
    source->base_offset += source->len;
    source->len = 0;
    if (source->bytes != NULL) {
        source->bytes[0] = '\0';
    }
    source->nlines = 0;
}

static int
reserve_bytes(_PyTok_SourceText *source, Py_ssize_t needed)
{
    if (needed <= source->cap) {
        return 0;
    }
    Py_ssize_t cap = source->cap > 0 ? source->cap : BUFSIZ;
    while (cap < needed) {
        if (cap > PY_SSIZE_T_MAX / SOURCE_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            break;
        }
        cap *= SOURCE_BUFFER_GROWTH_FACTOR;
    }
    char *bytes;
#ifdef Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    bytes = PyMem_Malloc(cap);
    if (bytes != NULL && source->len > 0) {
        memcpy(bytes, source->bytes, source->len);
    }
#else
    bytes = PyMem_Realloc(source->bytes, cap);
#endif
    if (bytes == NULL) {
        PyErr_NoMemory();
        return -1;
    }
#ifdef Py_DEBUG
    if (source->bytes != NULL) {
        memset(source->bytes, PYMEM_DEADBYTE, source->cap);
        PyMem_Free(source->bytes);
    }
#endif
    source->bytes = bytes;
    source->cap = cap;
    return 0;
}

static int
validate_लिन्या(const _PyTok_SourceText *source, const char *bytes,
              Py_ssize_t len, int append_newline)
{
    if (len < 0 || (len == 0 && !append_newline) || bytes == NULL ||
            (source->nlines > 0 &&
             (source->len == 0 || source->bytes[source->len - 1] != '\n'))) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        return -1;
    }
    const char *newline = memchr(bytes, '\n', len);
    if (newline != NULL && (append_newline || newline != bytes + len - 1)) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        return -1;
    }
    if (source->nlines == INT_MAX ||
            (source->nlines == INT_MAX - 1 && (newline != NULL || append_newline))) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        return -1;
    }
    return 0;
}

static _PyTok_Off
append_लिन्या(_PyTok_SourceText *source, const char *bytes,
            Py_ssize_t len, int append_newline)
{
    if (validate_लिन्या(source, bytes, len, append_newline) < 0) {
        return -1;
    }
    if (source->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            source->base_offset > PY_SSIZE_T_MAX - source->len - len - append_newline) {
        PyErr_NoMemory();
        return -1;
    }
    if (reserve_bytes(source, source->len + len + append_newline + 1) < 0) {
        return -1;
    }

    _PyTok_Off start = source->len;
    memcpy(source->bytes + start, bytes, len);
    source->len += len;
    if (append_newline) {
        source->bytes[source->len++] = '\n';
    }
    source->bytes[source->len] = '\0';
    source->nlines++;
    return source->base_offset + start;
}

_PyTok_Off
Py_Tok_Source_Append_लिन्या(_PyTok_SourceText *source, const char *bytes,
                        Py_ssize_t len)
{
    return append_लिन्या(source, bytes, len, 0);
}

_PyTok_Off
Py_Tok_Source_Append_लिन्या_With_Newline(_PyTok_SourceText *source, const char *bytes,
                                   Py_ssize_t len)
{
    return append_लिन्या(source, bytes, len, 1);
}

const char *
Py_Tok_Source_लिन्या_View(const _PyTok_SourceText *source, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    assert(len != NULL);
    const char *लिन्या = _PyTok_SourceData(source);
    const char *end = लिन्या + source->len;
    while (lineno > 1) {
        const char *newline = memchr(लिन्या, '\n', end - लिन्या);
        if (newline == NULL) {
            break;
        }
        लिन्या = newline + 1;
        lineno--;
    }
    const char *newline = memchr(लिन्या, '\n', end - लिन्या);
    *len = (newline != NULL ? newline : end) - लिन्या;
    return लिन्या;
}
