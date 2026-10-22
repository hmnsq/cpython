#ಅನ್ತರ್ಭಾವ "Python.h"
#ಅನ್ತರ್ಭಾವ "pycore_pymem.h"  // PYMEM_DEADBYTE

#ಅನ್ತರ್ಭಾವ "source.h"

#define SOURCE_BUFFER_GROWTH_FACTOR 2

ಶೂನ್ಯ
_PyTok_SourceInit(_PyTok_SourceText *source)
{
    *source = (_PyTok_SourceText){0};
}

ಶೂನ್ಯ
_PyTok_SourceClear(_PyTok_SourceText *source)
{
    PyMem_Free(source->bytes);
    _PyTok_SourceInit(source);
}

ಶೂನ್ಯ
_PyTok_SourceDiscard(_PyTok_SourceText *source)
{
    ಪ್ರತಿಜ್ಞಾ(source->base_offset <= PY_SSIZE_T_MAX - source->len);
    source->base_offset += source->len;
    source->len = 0;
    ಯದಿ (source->bytes != ರಿಕ್ತ) {
        source->bytes[0] = '\0';
    }
    source->nlines = 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
reserve_bytes(_PyTok_SourceText *source, Py_ssize_t needed)
{
    ಯದಿ (needed <= source->cap) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    Py_ssize_t cap = source->cap > 0 ? source->cap : BUFSIZ;
    ಯಾವತ್ (cap < needed) {
        ಯದಿ (cap > PY_SSIZE_T_MAX / SOURCE_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            ಭಙ್ಗ;
        }
        cap *= SOURCE_BUFFER_GROWTH_FACTOR;
    }
    ವರ್ಣ *bytes;
#ifdef Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    bytes = PyMem_Malloc(cap);
    ಯದಿ (bytes != ರಿಕ್ತ && source->len > 0) {
        memcpy(bytes, source->bytes, source->len);
    }
#ಅನ್ಯಥಾ
    bytes = PyMem_Realloc(source->bytes, cap);
#endif
    ಯದಿ (bytes == ರಿಕ್ತ) {
        PyErr_NoMemory();
        ಪ್ರತ್ಯಾಗಮ -1;
    }
#ifdef Py_DEBUG
    ಯದಿ (source->bytes != ರಿಕ್ತ) {
        memset(source->bytes, PYMEM_DEADBYTE, source->cap);
        PyMem_Free(source->bytes);
    }
#endif
    source->bytes = bytes;
    source->cap = cap;
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
validate_ಸಾಲು(ಧ್ರುವ _PyTok_SourceText *source, ಧ್ರುವ ವರ್ಣ *bytes,
              Py_ssize_t len, ಪೂರ್ಣಾಙ್ಕ append_newline)
{
    ಯದಿ (len < 0 || (len == 0 && !append_newline) || bytes == ರಿಕ್ತ ||
            (source->nlines > 0 &&
             (source->len == 0 || source->bytes[source->len - 1] != '\n'))) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಧ್ರುವ ವರ್ಣ *newline = memchr(bytes, '\n', len);
    ಯದಿ (newline != ರಿಕ್ತ && (append_newline || newline != bytes + len - 1)) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಯದಿ (source->nlines == INT_MAX ||
            (source->nlines == INT_MAX - 1 && (newline != ರಿಕ್ತ || append_newline))) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ _PyTok_Off
append_ಸಾಲು(_PyTok_SourceText *source, ಧ್ರುವ ವರ್ಣ *bytes,
            Py_ssize_t len, ಪೂರ್ಣಾಙ್ಕ append_newline)
{
    ಯದಿ (validate_ಸಾಲು(source, bytes, len, append_newline) < 0) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಯದಿ (source->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            source->base_offset > PY_SSIZE_T_MAX - source->len - len - append_newline) {
        PyErr_NoMemory();
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಯದಿ (reserve_bytes(source, source->len + len + append_newline + 1) < 0) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }

    _PyTok_Off start = source->len;
    memcpy(source->bytes + start, bytes, len);
    source->len += len;
    ಯದಿ (append_newline) {
        source->bytes[source->len++] = '\n';
    }
    source->bytes[source->len] = '\0';
    source->nlines++;
    ಪ್ರತ್ಯಾಗಮ source->base_offset + start;
}

_PyTok_Off
Py_Tok_Source_Append_ಸಾಲು(_PyTok_SourceText *source, ಧ್ರುವ ವರ್ಣ *bytes,
                        Py_ssize_t len)
{
    ಪ್ರತ್ಯಾಗಮ append_ಸಾಲು(source, bytes, len, 0);
}

_PyTok_Off
Py_Tok_Source_Append_ಸಾಲು_With_Newline(_PyTok_SourceText *source, ಧ್ರುವ ವರ್ಣ *bytes,
                                   Py_ssize_t len)
{
    ಪ್ರತ್ಯಾಗಮ append_ಸಾಲು(source, bytes, len, 1);
}

ಧ್ರುವ ವರ್ಣ *
Py_Tok_Source_ಸಾಲು_View(ಧ್ರುವ _PyTok_SourceText *source, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    ಪ್ರತಿಜ್ಞಾ(len != ರಿಕ್ತ);
    ಧ್ರುವ ವರ್ಣ *ಸಾಲು = _PyTok_SourceData(source);
    ಧ್ರುವ ವರ್ಣ *end = ಸಾಲು + source->len;
    ಯಾವತ್ (lineno > 1) {
        ಧ್ರುವ ವರ್ಣ *newline = memchr(ಸಾಲು, '\n', end - ಸಾಲು);
        ಯದಿ (newline == ರಿಕ್ತ) {
            ಭಙ್ಗ;
        }
        ಸಾಲು = newline + 1;
        lineno--;
    }
    ಧ್ರುವ ವರ್ಣ *newline = memchr(ಸಾಲು, '\n', end - ಸಾಲು);
    *len = (newline != ರಿಕ್ತ ? newline : end) - ಸಾಲು;
    ಪ್ರತ್ಯಾಗಮ ಸಾಲು;
}
