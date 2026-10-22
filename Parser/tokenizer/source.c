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
    ᮚᮦᮔ᮪ (source->bytes != NULL) {
        source->bytes[0] = '\0';
    }
    source->nlines = 0;
}

static puur_naa_nka
reserve_bytes(_PyTok_SourceText *source, Py_ssize_t needed)
{
    ᮚᮦᮔ᮪ (needed <= source->cap) {
        ᮝᮜᮤ 0;
    }
    Py_ssize_t cap = source->cap > 0 ? source->cap : BUFSIZ;
    ᮞᮨᮓᮨᮜ (cap < needed) {
        ᮚᮦᮔ᮪ (cap > PY_SSIZE_T_MAX / SOURCE_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            bha_nga;
        }
        cap *= SOURCE_BUFFER_GROWTH_FACTOR;
    }
    var_na *bytes;
#ifdef Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    bytes = PyMem_Malloc(cap);
    ᮚᮦᮔ᮪ (bytes != NULL && source->len > 0) {
        memcpy(bytes, source->bytes, source->len);
    }
#ᮜᮤᮚ
    bytes = PyMem_Realloc(source->bytes, cap);
#endif
    ᮚᮦᮔ᮪ (bytes == NULL) {
        PyErr_NoMemory();
        ᮝᮜᮤ -1;
    }
#ifdef Py_DEBUG
    ᮚᮦᮔ᮪ (source->bytes != NULL) {
        memset(source->bytes, PYMEM_DEADBYTE, source->cap);
        PyMem_Free(source->bytes);
    }
#endif
    source->bytes = bytes;
    source->cap = cap;
    ᮝᮜᮤ 0;
}

static puur_naa_nka
validate_line(const _PyTok_SourceText *source, const var_na *bytes,
              Py_ssize_t len, puur_naa_nka append_newline)
{
    ᮚᮦᮔ᮪ (len < 0 || (len == 0 && !append_newline) || bytes == NULL ||
            (source->nlines > 0 &&
             (source->len == 0 || source->bytes[source->len - 1] != '\n'))) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        ᮝᮜᮤ -1;
    }
    const var_na *newline = memchr(bytes, '\n', len);
    ᮚᮦᮔ᮪ (newline != NULL && (append_newline || newline != bytes + len - 1)) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        ᮝᮜᮤ -1;
    }
    ᮚᮦᮔ᮪ (source->nlines == INT_MAX ||
            (source->nlines == INT_MAX - 1 && (newline != NULL || append_newline))) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        ᮝᮜᮤ -1;
    }
    ᮝᮜᮤ 0;
}

static _PyTok_Off
append_line(_PyTok_SourceText *source, const var_na *bytes,
            Py_ssize_t len, puur_naa_nka append_newline)
{
    ᮚᮦᮔ᮪ (validate_line(source, bytes, len, append_newline) < 0) {
        ᮝᮜᮤ -1;
    }
    ᮚᮦᮔ᮪ (source->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            source->base_offset > PY_SSIZE_T_MAX - source->len - len - append_newline) {
        PyErr_NoMemory();
        ᮝᮜᮤ -1;
    }
    ᮚᮦᮔ᮪ (reserve_bytes(source, source->len + len + append_newline + 1) < 0) {
        ᮝᮜᮤ -1;
    }

    _PyTok_Off start = source->len;
    memcpy(source->bytes + start, bytes, len);
    source->len += len;
    ᮚᮦᮔ᮪ (append_newline) {
        source->bytes[source->len++] = '\n';
    }
    source->bytes[source->len] = '\0';
    source->nlines++;
    ᮝᮜᮤ source->base_offset + start;
}

_PyTok_Off
_PyTok_SourceAppendLine(_PyTok_SourceText *source, const var_na *bytes,
                        Py_ssize_t len)
{
    ᮝᮜᮤ append_line(source, bytes, len, 0);
}

_PyTok_Off
_PyTok_SourceAppendLineWithNewline(_PyTok_SourceText *source, const var_na *bytes,
                                   Py_ssize_t len)
{
    ᮝᮜᮤ append_line(source, bytes, len, 1);
}

const var_na *
_PyTok_SourceLineView(const _PyTok_SourceText *source, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    assert(len != NULL);
    const var_na *line = _PyTok_SourceData(source);
    const var_na *end = line + source->len;
    ᮞᮨᮓᮨᮜ (lineno > 1) {
        const var_na *newline = memchr(line, '\n', end - line);
        ᮚᮦᮔ᮪ (newline == NULL) {
            bha_nga;
        }
        line = newline + 1;
        lineno--;
    }
    const var_na *newline = memchr(line, '\n', end - line);
    *len = (newline != NULL ? newline : end) - line;
    ᮝᮜᮤ line;
}
