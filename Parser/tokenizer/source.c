#অন্তর্ভাব "Python.h"
#অন্তর্ভাব "pycore_pymem.h"  // PYMEM_DEADBYTE

#অন্তর্ভাব "source.h"

#define SOURCE_BUFFER_GROWTH_FACTOR 2

শূন্য
_PyTok_SourceInit(_PyTok_SourceText *source)
{
    *source = (_PyTok_SourceText){0};
}

শূন্য
_PyTok_SourceClear(_PyTok_SourceText *source)
{
    PyMem_Free(source->bytes);
    _PyTok_SourceInit(source);
}

শূন্য
_PyTok_SourceDiscard(_PyTok_SourceText *source)
{
    প্রতিজ্ঞা(source->base_offset <= PY_SSIZE_T_MAX - source->len);
    source->base_offset += source->len;
    source->len = 0;
    যদি (source->bytes != রিক্ত) {
        source->bytes[0] = '\0';
    }
    source->nlines = 0;
}

স্থির পূর্ণাঙ্ক
reserve_bytes(_PyTok_SourceText *source, Py_ssize_t needed)
{
    যদি (needed <= source->cap) {
        প্রত্যাগম 0;
    }
    Py_ssize_t cap = source->cap > 0 ? source->cap : BUFSIZ;
    যাবত্ (cap < needed) {
        যদি (cap > PY_SSIZE_T_MAX / SOURCE_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            ভঙ্গ;
        }
        cap *= SOURCE_BUFFER_GROWTH_FACTOR;
    }
    বর্ণ *bytes;
#ifdef Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    bytes = PyMem_Malloc(cap);
    যদি (bytes != রিক্ত && source->len > 0) {
        memcpy(bytes, source->bytes, source->len);
    }
#অন্যথা
    bytes = PyMem_Realloc(source->bytes, cap);
#endif
    যদি (bytes == রিক্ত) {
        PyErr_NoMemory();
        প্রত্যাগম -1;
    }
#ifdef Py_DEBUG
    যদি (source->bytes != রিক্ত) {
        memset(source->bytes, PYMEM_DEADBYTE, source->cap);
        PyMem_Free(source->bytes);
    }
#endif
    source->bytes = bytes;
    source->cap = cap;
    প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক
validate_সারি(ধ্রুব _PyTok_SourceText *source, ধ্রুব বর্ণ *bytes,
              Py_ssize_t len, পূর্ণাঙ্ক append_newline)
{
    যদি (len < 0 || (len == 0 && !append_newline) || bytes == রিক্ত ||
            (source->nlines > 0 &&
             (source->len == 0 || source->bytes[source->len - 1] != '\n'))) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        প্রত্যাগম -1;
    }
    ধ্রুব বর্ণ *newline = memchr(bytes, '\n', len);
    যদি (newline != রিক্ত && (append_newline || newline != bytes + len - 1)) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        প্রত্যাগম -1;
    }
    যদি (source->nlines == INT_MAX ||
            (source->nlines == INT_MAX - 1 && (newline != রিক্ত || append_newline))) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        প্রত্যাগম -1;
    }
    প্রত্যাগম 0;
}

স্থির _PyTok_Off
append_সারি(_PyTok_SourceText *source, ধ্রুব বর্ণ *bytes,
            Py_ssize_t len, পূর্ণাঙ্ক append_newline)
{
    যদি (validate_সারি(source, bytes, len, append_newline) < 0) {
        প্রত্যাগম -1;
    }
    যদি (source->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            source->base_offset > PY_SSIZE_T_MAX - source->len - len - append_newline) {
        PyErr_NoMemory();
        প্রত্যাগম -1;
    }
    যদি (reserve_bytes(source, source->len + len + append_newline + 1) < 0) {
        প্রত্যাগম -1;
    }

    _PyTok_Off start = source->len;
    memcpy(source->bytes + start, bytes, len);
    source->len += len;
    যদি (append_newline) {
        source->bytes[source->len++] = '\n';
    }
    source->bytes[source->len] = '\0';
    source->nlines++;
    প্রত্যাগম source->base_offset + start;
}

_PyTok_Off
Py_Tok_Source_Append_সারি(_PyTok_SourceText *source, ধ্রুব বর্ণ *bytes,
                        Py_ssize_t len)
{
    প্রত্যাগম append_সারি(source, bytes, len, 0);
}

_PyTok_Off
Py_Tok_Source_Append_সারি_With_Newline(_PyTok_SourceText *source, ধ্রুব বর্ণ *bytes,
                                   Py_ssize_t len)
{
    প্রত্যাগম append_সারি(source, bytes, len, 1);
}

ধ্রুব বর্ণ *
Py_Tok_Source_সারি_View(ধ্রুব _PyTok_SourceText *source, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    প্রতিজ্ঞা(len != রিক্ত);
    ধ্রুব বর্ণ *সারি = _PyTok_SourceData(source);
    ধ্রুব বর্ণ *end = সারি + source->len;
    যাবত্ (lineno > 1) {
        ধ্রুব বর্ণ *newline = memchr(সারি, '\n', end - সারি);
        যদি (newline == রিক্ত) {
            ভঙ্গ;
        }
        সারি = newline + 1;
        lineno--;
    }
    ধ্রুব বর্ণ *newline = memchr(সারি, '\n', end - সারি);
    *len = (newline != রিক্ত ? newline : end) - সারি;
    প্রত্যাগম সারি;
}
