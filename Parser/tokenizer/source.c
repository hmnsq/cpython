#ضمن "Python.h"
#ضمن "pycore_pymem.h"  // PYMEM_DEADBYTE

#ضمن "source.h"

#define SOURCE_BUFFER_GROWTH_FACTOR 2

فراغ
_PyTok_SourceInit(_PyTok_SourceText *source)
{
    *source = (_PyTok_SourceText){0};
}

فراغ
_PyTok_SourceClear(_PyTok_SourceText *source)
{
    PyMem_Free(source->bytes);
    _PyTok_SourceInit(source);
}

فراغ
_PyTok_SourceDiscard(_PyTok_SourceText *source)
{
    أكد(source->base_offset <= PY_SSIZE_T_MAX - source->len);
    source->base_offset += source->len;
    source->len = 0;
    אם (source->bytes != عدم) {
        source->bytes[0] = '\0';
    }
    source->nlines = 0;
}

ساكن صحيح
reserve_bytes(_PyTok_SourceText *source, Py_ssize_t needed)
{
    אם (needed <= source->cap) {
        החזר 0;
    }
    Py_ssize_t cap = source->cap > 0 ? source->cap : BUFSIZ;
    כלעוד (cap < needed) {
        אם (cap > PY_SSIZE_T_MAX / SOURCE_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            اكسر;
        }
        cap *= SOURCE_BUFFER_GROWTH_FACTOR;
    }
    حرف *bytes;
#ifdef Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    bytes = PyMem_Malloc(cap);
    אם (bytes != عدم && source->len > 0) {
        memcpy(bytes, source->bytes, source->len);
    }
#אחרת
    bytes = PyMem_Realloc(source->bytes, cap);
#endif
    אם (bytes == عدم) {
        PyErr_NoMemory();
        החזר -1;
    }
#ifdef Py_DEBUG
    אם (source->bytes != عدم) {
        memset(source->bytes, PYMEM_DEADBYTE, source->cap);
        PyMem_Free(source->bytes);
    }
#endif
    source->bytes = bytes;
    source->cap = cap;
    החזר 0;
}

ساكن صحيح
validate_line(ثابت _PyTok_SourceText *source, ثابت حرف *bytes,
              Py_ssize_t len, صحيح append_newline)
{
    אם (len < 0 || (len == 0 && !append_newline) || bytes == عدم ||
            (source->nlines > 0 &&
             (source->len == 0 || source->bytes[source->len - 1] != '\n'))) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        החזר -1;
    }
    ثابت حرف *newline = memchr(bytes, '\n', len);
    אם (newline != عدم && (append_newline || newline != bytes + len - 1)) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        החזר -1;
    }
    אם (source->nlines == INT_MAX ||
            (source->nlines == INT_MAX - 1 && (newline != عدم || append_newline))) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        החזר -1;
    }
    החזר 0;
}

ساكن _PyTok_Off
append_line(_PyTok_SourceText *source, ثابت حرف *bytes,
            Py_ssize_t len, صحيح append_newline)
{
    אם (validate_line(source, bytes, len, append_newline) < 0) {
        החזר -1;
    }
    אם (source->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            source->base_offset > PY_SSIZE_T_MAX - source->len - len - append_newline) {
        PyErr_NoMemory();
        החזר -1;
    }
    אם (reserve_bytes(source, source->len + len + append_newline + 1) < 0) {
        החזר -1;
    }

    _PyTok_Off start = source->len;
    memcpy(source->bytes + start, bytes, len);
    source->len += len;
    אם (append_newline) {
        source->bytes[source->len++] = '\n';
    }
    source->bytes[source->len] = '\0';
    source->nlines++;
    החזר source->base_offset + start;
}

_PyTok_Off
_PyTok_SourceAppendLine(_PyTok_SourceText *source, ثابت حرف *bytes,
                        Py_ssize_t len)
{
    החזר append_line(source, bytes, len, 0);
}

_PyTok_Off
_PyTok_SourceAppendLineWithNewline(_PyTok_SourceText *source, ثابت حرف *bytes,
                                   Py_ssize_t len)
{
    החזר append_line(source, bytes, len, 1);
}

ثابت حرف *
_PyTok_SourceLineView(ثابت _PyTok_SourceText *source, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    أكد(len != عدم);
    ثابت حرف *line = _PyTok_SourceData(source);
    ثابت حرف *end = line + source->len;
    כלעוד (lineno > 1) {
        ثابت حرف *newline = memchr(line, '\n', end - line);
        אם (newline == عدم) {
            اكسر;
        }
        line = newline + 1;
        lineno--;
    }
    ثابت حرف *newline = memchr(line, '\n', end - line);
    *len = (newline != عدم ? newline : end) - line;
    החזר line;
}
