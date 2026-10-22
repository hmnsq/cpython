#অন্তর্ভাব "Python.h"
#অন্তর্ভাব "pycore_pymem.h"  // PYMEM_DEADBYTE

#অন্তর্ভাব "source.h"

#define SOURCE_বাফার_GROWTH_FACTOR 2

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
    যদি (source->bytes != ফাঁকা) {
        source->bytes[0] = '\0';
    }
    source->nlines = 0;
}

স্থির পূর্ণসংখ্যা
reserve_bytes(_PyTok_SourceText *source, Py_ssize_t needed)
{
    যদি (needed <= source->cap) {
        ফেরত 0;
    }
    Py_ssize_t cap = source->cap > 0 ? source->cap : BUFSIZ;
    যখন (cap < needed) {
        যদি (cap > PY_SSIZE_T_MAX / SOURCE_বাফার_GROWTH_FACTOR) {
            cap = needed;
            বিরতি;
        }
        cap *= SOURCE_বাফার_GROWTH_FACTOR;
    }
    অক্ষর *bytes;
#ifdef Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    bytes = PyMem_Malloc(cap);
    যদি (bytes != ফাঁকা && source->len > 0) {
        memcpy(bytes, source->bytes, source->len);
    }
#অন্যথা
    bytes = PyMem_Realloc(source->bytes, cap);
#endif
    যদি (bytes == ফাঁকা) {
        PyErr_NoMemory();
        ফেরত -1;
    }
#ifdef Py_DEBUG
    যদি (source->bytes != ফাঁকা) {
        memset(source->bytes, PYMEM_DEADBYTE, source->cap);
        PyMem_Free(source->bytes);
    }
#endif
    source->bytes = bytes;
    source->cap = cap;
    ফেরত 0;
}

স্থির পূর্ণসংখ্যা
validate_সারি(ধ্রুবক _PyTok_SourceText *source, ধ্রুবক অক্ষর *bytes,
              Py_ssize_t len, পূর্ণসংখ্যা append_newline)
{
    যদি (len < 0 || (len == 0 && !append_newline) || bytes == ফাঁকা ||
            (source->nlines > 0 &&
             (source->len == 0 || source->bytes[source->len - 1] != '\n'))) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        ফেরত -1;
    }
    ধ্রুবক অক্ষর *newline = memchr(bytes, '\n', len);
    যদি (newline != ফাঁকা && (append_newline || newline != bytes + len - 1)) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        ফেরত -1;
    }
    যদি (source->nlines == INT_MAX ||
            (source->nlines == INT_MAX - 1 && (newline != ফাঁকা || append_newline))) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        ফেরত -1;
    }
    ফেরত 0;
}

স্থির _PyTok_Off
append_সারি(_PyTok_SourceText *source, ধ্রুবক অক্ষর *bytes,
            Py_ssize_t len, পূর্ণসংখ্যা append_newline)
{
    যদি (validate_সারি(source, bytes, len, append_newline) < 0) {
        ফেরত -1;
    }
    যদি (source->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            source->base_offset > PY_SSIZE_T_MAX - source->len - len - append_newline) {
        PyErr_NoMemory();
        ফেরত -1;
    }
    যদি (reserve_bytes(source, source->len + len + append_newline + 1) < 0) {
        ফেরত -1;
    }

    _PyTok_Off start = source->len;
    memcpy(source->bytes + start, bytes, len);
    source->len += len;
    যদি (append_newline) {
        source->bytes[source->len++] = '\n';
    }
    source->bytes[source->len] = '\0';
    source->nlines++;
    ফেরত source->base_offset + start;
}

_PyTok_Off
Py_Tok_Source_Append_সারি(_PyTok_SourceText *source, ধ্রুবক অক্ষর *bytes,
                        Py_ssize_t len)
{
    ফেরত append_সারি(source, bytes, len, 0);
}

_PyTok_Off
Py_Tok_Source_Append_সারি_With_Newline(_PyTok_SourceText *source, ধ্রুবক অক্ষর *bytes,
                                   Py_ssize_t len)
{
    ফেরত append_সারি(source, bytes, len, 1);
}

ধ্রুবক অক্ষর *
Py_Tok_Source_সারি_View(ধ্রুবক _PyTok_SourceText *source, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    প্রতিজ্ঞা(len != ফাঁকা);
    ধ্রুবক অক্ষর *সারি = Py_Tok_Source_তথ্য(source);
    ধ্রুবক অক্ষর *end = সারি + source->len;
    যখন (lineno > 1) {
        ধ্রুবক অক্ষর *newline = memchr(সারি, '\n', end - সারি);
        যদি (newline == ফাঁকা) {
            বিরতি;
        }
        সারি = newline + 1;
        lineno--;
    }
    ধ্রুবক অক্ষর *newline = memchr(সারি, '\n', end - সারি);
    *len = (newline != ফাঁকা ? newline : end) - সারি;
    ফেরত সারি;
}
