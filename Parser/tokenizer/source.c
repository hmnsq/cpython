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
    만약 (source->bytes != NULL) {
        source->bytes[0] = '\0';
    }
    source->nlines = 0;
}

static 정수
reserve_bytes(_PyTok_SourceText *source, Py_ssize_t needed)
{
    만약 (needed <= source->cap) {
        반환 0;
    }
    Py_ssize_t cap = source->cap > 0 ? source->cap : BUFSIZ;
    동안 (cap < needed) {
        만약 (cap > PY_SSIZE_T_MAX / SOURCE_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            break;
        }
        cap *= SOURCE_BUFFER_GROWTH_FACTOR;
    }
    char *bytes;
#ifdef Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    bytes = PyMem_Malloc(cap);
    만약 (bytes != NULL && source->len > 0) {
        memcpy(bytes, source->bytes, source->len);
    }
#아니면
    bytes = PyMem_Realloc(source->bytes, cap);
#endif
    만약 (bytes == NULL) {
        PyErr_NoMemory();
        반환 -1;
    }
#ifdef Py_DEBUG
    만약 (source->bytes != NULL) {
        memset(source->bytes, PYMEM_DEADBYTE, source->cap);
        PyMem_Free(source->bytes);
    }
#endif
    source->bytes = bytes;
    source->cap = cap;
    반환 0;
}

static 정수
validate_line(const _PyTok_SourceText *source, const char *bytes,
              Py_ssize_t len, 정수 append_newline)
{
    만약 (len < 0 || (len == 0 && !append_newline) || bytes == NULL ||
            (source->nlines > 0 &&
             (source->len == 0 || source->bytes[source->len - 1] != '\n'))) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        반환 -1;
    }
    const char *newline = memchr(bytes, '\n', len);
    만약 (newline != NULL && (append_newline || newline != bytes + len - 1)) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        반환 -1;
    }
    만약 (source->nlines == INT_MAX ||
            (source->nlines == INT_MAX - 1 && (newline != NULL || append_newline))) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        반환 -1;
    }
    반환 0;
}

static _PyTok_Off
append_line(_PyTok_SourceText *source, const char *bytes,
            Py_ssize_t len, 정수 append_newline)
{
    만약 (validate_line(source, bytes, len, append_newline) < 0) {
        반환 -1;
    }
    만약 (source->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            source->base_offset > PY_SSIZE_T_MAX - source->len - len - append_newline) {
        PyErr_NoMemory();
        반환 -1;
    }
    만약 (reserve_bytes(source, source->len + len + append_newline + 1) < 0) {
        반환 -1;
    }

    _PyTok_Off start = source->len;
    memcpy(source->bytes + start, bytes, len);
    source->len += len;
    만약 (append_newline) {
        source->bytes[source->len++] = '\n';
    }
    source->bytes[source->len] = '\0';
    source->nlines++;
    반환 source->base_offset + start;
}

_PyTok_Off
_PyTok_SourceAppendLine(_PyTok_SourceText *source, const char *bytes,
                        Py_ssize_t len)
{
    반환 append_line(source, bytes, len, 0);
}

_PyTok_Off
_PyTok_SourceAppendLineWithNewline(_PyTok_SourceText *source, const char *bytes,
                                   Py_ssize_t len)
{
    반환 append_line(source, bytes, len, 1);
}

const char *
_PyTok_SourceLineView(const _PyTok_SourceText *source, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    assert(len != NULL);
    const char *line = _PyTok_SourceData(source);
    const char *end = line + source->len;
    동안 (lineno > 1) {
        const char *newline = memchr(line, '\n', end - line);
        만약 (newline == NULL) {
            break;
        }
        line = newline + 1;
        lineno--;
    }
    const char *newline = memchr(line, '\n', end - line);
    *len = (newline != NULL ? newline : end) - line;
    반환 line;
}
