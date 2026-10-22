#அந்தர்பாவ "Python.h"
#அந்தர்பாவ "pycore_pymem.h"  // PYMEM_DEADBYTE

#அந்தர்பாவ "source.h"

#define SOURCE_BUFFER_GROWTH_FACTOR 2

வெறுமை
_PyTok_SourceInit(_PyTok_SourceText *source)
{
    *source = (_PyTok_SourceText){0};
}

வெறுமை
_PyTok_SourceClear(_PyTok_SourceText *source)
{
    PyMem_Free(source->bytes);
    _PyTok_SourceInit(source);
}

வெறுமை
_PyTok_SourceDiscard(_PyTok_SourceText *source)
{
    ப்ரதிஜ்ஞா(source->base_offset <= PY_SSIZE_T_MAX - source->len);
    source->base_offset += source->len;
    source->len = 0;
    எனில் (source->bytes != வெற்று) {
        source->bytes[0] = '\0';
    }
    source->nlines = 0;
}

நிலையான முழு
reserve_bytes(_PyTok_SourceText *source, Py_ssize_t needed)
{
    எனில் (needed <= source->cap) {
        திரும்பு 0;
    }
    Py_ssize_t cap = source->cap > 0 ? source->cap : BUFSIZ;
    போது (cap < needed) {
        எனில் (cap > PY_SSIZE_T_MAX / SOURCE_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            நிறுத்து;
        }
        cap *= SOURCE_BUFFER_GROWTH_FACTOR;
    }
    எழுத்து *bytes;
#ifdef Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    bytes = PyMem_Malloc(cap);
    எனில் (bytes != வெற்று && source->len > 0) {
        memcpy(bytes, source->bytes, source->len);
    }
#இல்லையெனில்
    bytes = PyMem_Realloc(source->bytes, cap);
#endif
    எனில் (bytes == வெற்று) {
        PyErr_NoMemory();
        திரும்பு -1;
    }
#ifdef Py_DEBUG
    எனில் (source->bytes != வெற்று) {
        memset(source->bytes, PYMEM_DEADBYTE, source->cap);
        PyMem_Free(source->bytes);
    }
#endif
    source->bytes = bytes;
    source->cap = cap;
    திரும்பு 0;
}

நிலையான முழு
validate_வரி(மாறிலி _PyTok_SourceText *source, மாறிலி எழுத்து *bytes,
              Py_ssize_t len, முழு append_newline)
{
    எனில் (len < 0 || (len == 0 && !append_newline) || bytes == வெற்று ||
            (source->nlines > 0 &&
             (source->len == 0 || source->bytes[source->len - 1] != '\n'))) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        திரும்பு -1;
    }
    மாறிலி எழுத்து *newline = memchr(bytes, '\n', len);
    எனில் (newline != வெற்று && (append_newline || newline != bytes + len - 1)) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        திரும்பு -1;
    }
    எனில் (source->nlines == INT_MAX ||
            (source->nlines == INT_MAX - 1 && (newline != வெற்று || append_newline))) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        திரும்பு -1;
    }
    திரும்பு 0;
}

நிலையான _PyTok_Off
append_வரி(_PyTok_SourceText *source, மாறிலி எழுத்து *bytes,
            Py_ssize_t len, முழு append_newline)
{
    எனில் (validate_வரி(source, bytes, len, append_newline) < 0) {
        திரும்பு -1;
    }
    எனில் (source->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            source->base_offset > PY_SSIZE_T_MAX - source->len - len - append_newline) {
        PyErr_NoMemory();
        திரும்பு -1;
    }
    எனில் (reserve_bytes(source, source->len + len + append_newline + 1) < 0) {
        திரும்பு -1;
    }

    _PyTok_Off start = source->len;
    memcpy(source->bytes + start, bytes, len);
    source->len += len;
    எனில் (append_newline) {
        source->bytes[source->len++] = '\n';
    }
    source->bytes[source->len] = '\0';
    source->nlines++;
    திரும்பு source->base_offset + start;
}

_PyTok_Off
Py_Tok_Source_Append_வரி(_PyTok_SourceText *source, மாறிலி எழுத்து *bytes,
                        Py_ssize_t len)
{
    திரும்பு append_வரி(source, bytes, len, 0);
}

_PyTok_Off
Py_Tok_Source_Append_வரி_With_Newline(_PyTok_SourceText *source, மாறிலி எழுத்து *bytes,
                                   Py_ssize_t len)
{
    திரும்பு append_வரி(source, bytes, len, 1);
}

மாறிலி எழுத்து *
Py_Tok_Source_வரி_View(மாறிலி _PyTok_SourceText *source, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    ப்ரதிஜ்ஞா(len != வெற்று);
    மாறிலி எழுத்து *வரி = _PyTok_SourceData(source);
    மாறிலி எழுத்து *end = வரி + source->len;
    போது (lineno > 1) {
        மாறிலி எழுத்து *newline = memchr(வரி, '\n', end - வரி);
        எனில் (newline == வெற்று) {
            நிறுத்து;
        }
        வரி = newline + 1;
        lineno--;
    }
    மாறிலி எழுத்து *newline = memchr(வரி, '\n', end - வரி);
    *len = (newline != வெற்று ? newline : end) - வரி;
    திரும்பு வரி;
}
