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
    nếu (source->bytes != NULL) {
        source->bytes[0] = '\0';
    }
    source->nlines = 0;
}

static số_nguyên
reserve_bytes(_PyTok_SourceText *source, Py_ssize_t needed)
{
    nếu (needed <= source->cap) {
        trả_về 0;
    }
    Py_ssize_t cap = source->cap > 0 ? source->cap : BUFSIZ;
    trong_khi (cap < needed) {
        nếu (cap > PY_SSIZE_T_MAX / SOURCE_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            break;
        }
        cap *= SOURCE_BUFFER_GROWTH_FACTOR;
    }
    char *bytes;
#ifdef Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    bytes = PyMem_Malloc(cap);
    nếu (bytes != NULL && source->len > 0) {
        memcpy(bytes, source->bytes, source->len);
    }
#không_thì
    bytes = PyMem_Realloc(source->bytes, cap);
#endif
    nếu (bytes == NULL) {
        PyErr_NoMemory();
        trả_về -1;
    }
#ifdef Py_DEBUG
    nếu (source->bytes != NULL) {
        memset(source->bytes, PYMEM_DEADBYTE, source->cap);
        PyMem_Free(source->bytes);
    }
#endif
    source->bytes = bytes;
    source->cap = cap;
    trả_về 0;
}

static số_nguyên
validate_line(const _PyTok_SourceText *source, const char *bytes,
              Py_ssize_t len, số_nguyên append_newline)
{
    nếu (len < 0 || (len == 0 && !append_newline) || bytes == NULL ||
            (source->nlines > 0 &&
             (source->len == 0 || source->bytes[source->len - 1] != '\n'))) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        trả_về -1;
    }
    const char *newline = memchr(bytes, '\n', len);
    nếu (newline != NULL && (append_newline || newline != bytes + len - 1)) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        trả_về -1;
    }
    nếu (source->nlines == INT_MAX ||
            (source->nlines == INT_MAX - 1 && (newline != NULL || append_newline))) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        trả_về -1;
    }
    trả_về 0;
}

static _PyTok_Off
append_line(_PyTok_SourceText *source, const char *bytes,
            Py_ssize_t len, số_nguyên append_newline)
{
    nếu (validate_line(source, bytes, len, append_newline) < 0) {
        trả_về -1;
    }
    nếu (source->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            source->base_offset > PY_SSIZE_T_MAX - source->len - len - append_newline) {
        PyErr_NoMemory();
        trả_về -1;
    }
    nếu (reserve_bytes(source, source->len + len + append_newline + 1) < 0) {
        trả_về -1;
    }

    _PyTok_Off start = source->len;
    memcpy(source->bytes + start, bytes, len);
    source->len += len;
    nếu (append_newline) {
        source->bytes[source->len++] = '\n';
    }
    source->bytes[source->len] = '\0';
    source->nlines++;
    trả_về source->base_offset + start;
}

_PyTok_Off
_PyTok_SourceAppendLine(_PyTok_SourceText *source, const char *bytes,
                        Py_ssize_t len)
{
    trả_về append_line(source, bytes, len, 0);
}

_PyTok_Off
_PyTok_SourceAppendLineWithNewline(_PyTok_SourceText *source, const char *bytes,
                                   Py_ssize_t len)
{
    trả_về append_line(source, bytes, len, 1);
}

const char *
_PyTok_SourceLineView(const _PyTok_SourceText *source, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    assert(len != NULL);
    const char *line = _PyTok_SourceData(source);
    const char *end = line + source->len;
    trong_khi (lineno > 1) {
        const char *newline = memchr(line, '\n', end - line);
        nếu (newline == NULL) {
            break;
        }
        line = newline + 1;
        lineno--;
    }
    const char *newline = memchr(line, '\n', end - line);
    *len = (newline != NULL ? newline : end) - line;
    trả_về line;
}
