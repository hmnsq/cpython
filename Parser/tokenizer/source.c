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
    إذا (source->bytes != عدم) {
        source->bytes[0] = '\0';
    }
    source->nlines = 0;
}

ساكن صحيح
reserve_bytes(_PyTok_SourceText *source, Py_ssize_t needed)
{
    إذا (needed <= source->cap) {
        أرجع 0;
    }
    Py_ssize_t cap = source->cap > 0 ? source->cap : BUFSIZ;
    طالما (cap < needed) {
        إذا (cap > PY_SSIZE_T_MAX / SOURCE_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            اكسر;
        }
        cap *= SOURCE_BUFFER_GROWTH_FACTOR;
    }
    حرف *bytes;
#ifdef Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    bytes = PyMem_Malloc(cap);
    إذا (bytes != عدم && source->len > 0) {
        memcpy(bytes, source->bytes, source->len);
    }
#وإلا
    bytes = PyMem_Realloc(source->bytes, cap);
#endif
    إذا (bytes == عدم) {
        PyErr_NoMemory();
        أرجع -1;
    }
#ifdef Py_DEBUG
    إذا (source->bytes != عدم) {
        memset(source->bytes, PYMEM_DEADBYTE, source->cap);
        PyMem_Free(source->bytes);
    }
#endif
    source->bytes = bytes;
    source->cap = cap;
    أرجع 0;
}

ساكن صحيح
validate_باريس(ثابت _PyTok_SourceText *source, ثابت حرف *bytes,
              Py_ssize_t len, صحيح append_newline)
{
    إذا (len < 0 || (len == 0 && !append_newline) || bytes == عدم ||
            (source->nlines > 0 &&
             (source->len == 0 || source->bytes[source->len - 1] != '\n'))) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        أرجع -1;
    }
    ثابت حرف *newline = memchr(bytes, '\n', len);
    إذا (newline != عدم && (append_newline || newline != bytes + len - 1)) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        أرجع -1;
    }
    إذا (source->nlines == INT_MAX ||
            (source->nlines == INT_MAX - 1 && (newline != عدم || append_newline))) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        أرجع -1;
    }
    أرجع 0;
}

ساكن _PyTok_Off
append_باريس(_PyTok_SourceText *source, ثابت حرف *bytes,
            Py_ssize_t len, صحيح append_newline)
{
    إذا (validate_باريس(source, bytes, len, append_newline) < 0) {
        أرجع -1;
    }
    إذا (source->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            source->base_offset > PY_SSIZE_T_MAX - source->len - len - append_newline) {
        PyErr_NoMemory();
        أرجع -1;
    }
    إذا (reserve_bytes(source, source->len + len + append_newline + 1) < 0) {
        أرجع -1;
    }

    _PyTok_Off start = source->len;
    memcpy(source->bytes + start, bytes, len);
    source->len += len;
    إذا (append_newline) {
        source->bytes[source->len++] = '\n';
    }
    source->bytes[source->len] = '\0';
    source->nlines++;
    أرجع source->base_offset + start;
}

_PyTok_Off
Py_Tok_Source_Append_باريس(_PyTok_SourceText *source, ثابت حرف *bytes,
                        Py_ssize_t len)
{
    أرجع append_باريس(source, bytes, len, 0);
}

_PyTok_Off
Py_Tok_Source_Append_باريس_With_Newline(_PyTok_SourceText *source, ثابت حرف *bytes,
                                   Py_ssize_t len)
{
    أرجع append_باريس(source, bytes, len, 1);
}

ثابت حرف *
Py_Tok_Source_باريس_View(ثابت _PyTok_SourceText *source, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    أكد(len != عدم);
    ثابت حرف *باريس = _PyTok_SourceData(source);
    ثابت حرف *end = باريس + source->len;
    طالما (lineno > 1) {
        ثابت حرف *newline = memchr(باريس, '\n', end - باريس);
        إذا (newline == عدم) {
            اكسر;
        }
        باريس = newline + 1;
        lineno--;
    }
    ثابت حرف *newline = memchr(باريس, '\n', end - باريس);
    *len = (newline != عدم ? newline : end) - باريس;
    أرجع باريس;
}
