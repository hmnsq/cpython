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
    ако (source->bytes != NULL) {
        source->bytes[0] = '\0';
    }
    source->nlines = 0;
}

static цео
reserve_bytes(_PyTok_SourceText *source, Py_ssize_t needed)
{
    ако (needed <= source->cap) {
        врати 0;
    }
    Py_ssize_t cap = source->cap > 0 ? source->cap : BUFSIZ;
    док (cap < needed) {
        ако (cap > PY_SSIZE_T_MAX / SOURCE_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            break;
        }
        cap *= SOURCE_BUFFER_GROWTH_FACTOR;
    }
    char *bytes;
#ifdef Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    bytes = PyMem_Malloc(cap);
    ако (bytes != NULL && source->len > 0) {
        memcpy(bytes, source->bytes, source->len);
    }
#инако
    bytes = PyMem_Realloc(source->bytes, cap);
#endif
    ако (bytes == NULL) {
        PyErr_NoMemory();
        врати -1;
    }
#ifdef Py_DEBUG
    ако (source->bytes != NULL) {
        memset(source->bytes, PYMEM_DEADBYTE, source->cap);
        PyMem_Free(source->bytes);
    }
#endif
    source->bytes = bytes;
    source->cap = cap;
    врати 0;
}

static цео
validate_line(const _PyTok_SourceText *source, const char *bytes,
              Py_ssize_t len, цео append_newline)
{
    ако (len < 0 || (len == 0 && !append_newline) || bytes == NULL ||
            (source->nlines > 0 &&
             (source->len == 0 || source->bytes[source->len - 1] != '\n'))) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        врати -1;
    }
    const char *newline = memchr(bytes, '\n', len);
    ако (newline != NULL && (append_newline || newline != bytes + len - 1)) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        врати -1;
    }
    ако (source->nlines == INT_MAX ||
            (source->nlines == INT_MAX - 1 && (newline != NULL || append_newline))) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        врати -1;
    }
    врати 0;
}

static _PyTok_Off
append_line(_PyTok_SourceText *source, const char *bytes,
            Py_ssize_t len, цео append_newline)
{
    ако (validate_line(source, bytes, len, append_newline) < 0) {
        врати -1;
    }
    ако (source->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            source->base_offset > PY_SSIZE_T_MAX - source->len - len - append_newline) {
        PyErr_NoMemory();
        врати -1;
    }
    ако (reserve_bytes(source, source->len + len + append_newline + 1) < 0) {
        врати -1;
    }

    _PyTok_Off start = source->len;
    memcpy(source->bytes + start, bytes, len);
    source->len += len;
    ако (append_newline) {
        source->bytes[source->len++] = '\n';
    }
    source->bytes[source->len] = '\0';
    source->nlines++;
    врати source->base_offset + start;
}

_PyTok_Off
_PyTok_SourceAppendLine(_PyTok_SourceText *source, const char *bytes,
                        Py_ssize_t len)
{
    врати append_line(source, bytes, len, 0);
}

_PyTok_Off
_PyTok_SourceAppendLineWithNewline(_PyTok_SourceText *source, const char *bytes,
                                   Py_ssize_t len)
{
    врати append_line(source, bytes, len, 1);
}

const char *
_PyTok_SourceLineView(const _PyTok_SourceText *source, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    assert(len != NULL);
    const char *line = _PyTok_SourceData(source);
    const char *end = line + source->len;
    док (lineno > 1) {
        const char *newline = memchr(line, '\n', end - line);
        ако (newline == NULL) {
            break;
        }
        line = newline + 1;
        lineno--;
    }
    const char *newline = memchr(line, '\n', end - line);
    *len = (newline != NULL ? newline : end) - line;
    врати line;
}
