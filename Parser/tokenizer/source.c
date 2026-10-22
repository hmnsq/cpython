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
    ᯀᯮᯝ (source->bytes != NULL) {
        source->bytes[0] = '\0';
    }
    source->nlines = 0;
}

static puur_naa_nka
reserve_bytes(_PyTok_SourceText *source, Py_ssize_t needed)
{
    ᯀᯮᯝ (needed <= source->cap) {
        ᯀᯮᯞ 0;
    }
    Py_ssize_t cap = source->cap > 0 ? source->cap : BUFSIZ;
    ᯀᯬᯝ (cap < needed) {
        ᯀᯮᯝ (cap > PY_SSIZE_T_MAX / SOURCE_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            bha_nga;
        }
        cap *= SOURCE_BUFFER_GROWTH_FACTOR;
    }
    var_na *bytes;
#ifdef Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    bytes = PyMem_Malloc(cap);
    ᯀᯮᯝ (bytes != NULL && source->len > 0) {
        memcpy(bytes, source->bytes, source->len);
    }
#ᯀᯮᯝᯂᯪ
    bytes = PyMem_Realloc(source->bytes, cap);
#endif
    ᯀᯮᯝ (bytes == NULL) {
        PyErr_NoMemory();
        ᯀᯮᯞ -1;
    }
#ifdef Py_DEBUG
    ᯀᯮᯝ (source->bytes != NULL) {
        memset(source->bytes, PYMEM_DEADBYTE, source->cap);
        PyMem_Free(source->bytes);
    }
#endif
    source->bytes = bytes;
    source->cap = cap;
    ᯀᯮᯞ 0;
}

static puur_naa_nka
validate_line(const _PyTok_SourceText *source, const var_na *bytes,
              Py_ssize_t len, puur_naa_nka append_newline)
{
    ᯀᯮᯝ (len < 0 || (len == 0 && !append_newline) || bytes == NULL ||
            (source->nlines > 0 &&
             (source->len == 0 || source->bytes[source->len - 1] != '\n'))) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        ᯀᯮᯞ -1;
    }
    const var_na *newline = memchr(bytes, '\n', len);
    ᯀᯮᯝ (newline != NULL && (append_newline || newline != bytes + len - 1)) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        ᯀᯮᯞ -1;
    }
    ᯀᯮᯝ (source->nlines == INT_MAX ||
            (source->nlines == INT_MAX - 1 && (newline != NULL || append_newline))) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        ᯀᯮᯞ -1;
    }
    ᯀᯮᯞ 0;
}

static _PyTok_Off
append_line(_PyTok_SourceText *source, const var_na *bytes,
            Py_ssize_t len, puur_naa_nka append_newline)
{
    ᯀᯮᯝ (validate_line(source, bytes, len, append_newline) < 0) {
        ᯀᯮᯞ -1;
    }
    ᯀᯮᯝ (source->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            source->base_offset > PY_SSIZE_T_MAX - source->len - len - append_newline) {
        PyErr_NoMemory();
        ᯀᯮᯞ -1;
    }
    ᯀᯮᯝ (reserve_bytes(source, source->len + len + append_newline + 1) < 0) {
        ᯀᯮᯞ -1;
    }

    _PyTok_Off start = source->len;
    memcpy(source->bytes + start, bytes, len);
    source->len += len;
    ᯀᯮᯝ (append_newline) {
        source->bytes[source->len++] = '\n';
    }
    source->bytes[source->len] = '\0';
    source->nlines++;
    ᯀᯮᯞ source->base_offset + start;
}

_PyTok_Off
_PyTok_SourceAppendLine(_PyTok_SourceText *source, const var_na *bytes,
                        Py_ssize_t len)
{
    ᯀᯮᯞ append_line(source, bytes, len, 0);
}

_PyTok_Off
_PyTok_SourceAppendLineWithNewline(_PyTok_SourceText *source, const var_na *bytes,
                                   Py_ssize_t len)
{
    ᯀᯮᯞ append_line(source, bytes, len, 1);
}

const var_na *
_PyTok_SourceLineView(const _PyTok_SourceText *source, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    assert(len != NULL);
    const var_na *line = _PyTok_SourceData(source);
    const var_na *end = line + source->len;
    ᯀᯬᯝ (lineno > 1) {
        const var_na *newline = memchr(line, '\n', end - line);
        ᯀᯮᯝ (newline == NULL) {
            bha_nga;
        }
        line = newline + 1;
        lineno--;
    }
    const var_na *newline = memchr(line, '\n', end - line);
    *len = (newline != NULL ? newline : end) - line;
    ᯀᯮᯞ line;
}
