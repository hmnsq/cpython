#ଅନ୍ତର୍ଭାଵ "Python.h"
#ଅନ୍ତର୍ଭାଵ "pycore_pymem.h"  // PYMEM_DEADBYTE

#ଅନ୍ତର୍ଭାଵ "source.h"

#define SOURCE_BUFFER_GROWTH_FACTOR 2

ଶୂନ୍ୟ
_PyTok_SourceInit(_PyTok_SourceText *source)
{
    *source = (_PyTok_SourceText){0};
}

ଶୂନ୍ୟ
_PyTok_SourceClear(_PyTok_SourceText *source)
{
    PyMem_Free(source->bytes);
    _PyTok_SourceInit(source);
}

ଶୂନ୍ୟ
_PyTok_SourceDiscard(_PyTok_SourceText *source)
{
    ପ୍ରତିଜ୍ଞା(source->base_offset <= PY_SSIZE_T_MAX - source->len);
    source->base_offset += source->len;
    source->len = 0;
    ଯଦି (source->bytes != ଖାଲି) {
        source->bytes[0] = '\0';
    }
    source->nlines = 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
reserve_bytes(_PyTok_SourceText *source, Py_ssize_t needed)
{
    ଯଦି (needed <= source->cap) {
        ଫେରନ୍ତୁ 0;
    }
    Py_ssize_t cap = source->cap > 0 ? source->cap : BUFSIZ;
    ଯେତେବେଳେ (cap < needed) {
        ଯଦି (cap > PY_SSIZE_T_MAX / SOURCE_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            ଭାଙ୍ଗ;
        }
        cap *= SOURCE_BUFFER_GROWTH_FACTOR;
    }
    ଅକ୍ଷର *bytes;
#ifdef Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    bytes = PyMem_Malloc(cap);
    ଯଦି (bytes != ଖାଲି && source->len > 0) {
        memcpy(bytes, source->bytes, source->len);
    }
#ଅନ୍ୟଥା
    bytes = PyMem_Realloc(source->bytes, cap);
#endif
    ଯଦି (bytes == ଖାଲି) {
        PyErr_NoMemory();
        ଫେରନ୍ତୁ -1;
    }
#ifdef Py_DEBUG
    ଯଦି (source->bytes != ଖାଲି) {
        memset(source->bytes, PYMEM_DEADBYTE, source->cap);
        PyMem_Free(source->bytes);
    }
#endif
    source->bytes = bytes;
    source->cap = cap;
    ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
validate_ଧାଡ଼ି(ଅଚଳ _PyTok_SourceText *source, ଅଚଳ ଅକ୍ଷର *bytes,
              Py_ssize_t len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା append_newline)
{
    ଯଦି (len < 0 || (len == 0 && !append_newline) || bytes == ଖାଲି ||
            (source->nlines > 0 &&
             (source->len == 0 || source->bytes[source->len - 1] != '\n'))) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        ଫେରନ୍ତୁ -1;
    }
    ଅଚଳ ଅକ୍ଷର *newline = memchr(bytes, '\n', len);
    ଯଦି (newline != ଖାଲି && (append_newline || newline != bytes + len - 1)) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        ଫେରନ୍ତୁ -1;
    }
    ଯଦି (source->nlines == INT_MAX ||
            (source->nlines == INT_MAX - 1 && (newline != ଖାଲି || append_newline))) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        ଫେରନ୍ତୁ -1;
    }
    ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର _PyTok_Off
append_ଧାଡ଼ି(_PyTok_SourceText *source, ଅଚଳ ଅକ୍ଷର *bytes,
            Py_ssize_t len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା append_newline)
{
    ଯଦି (validate_ଧାଡ଼ି(source, bytes, len, append_newline) < 0) {
        ଫେରନ୍ତୁ -1;
    }
    ଯଦି (source->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            source->base_offset > PY_SSIZE_T_MAX - source->len - len - append_newline) {
        PyErr_NoMemory();
        ଫେରନ୍ତୁ -1;
    }
    ଯଦି (reserve_bytes(source, source->len + len + append_newline + 1) < 0) {
        ଫେରନ୍ତୁ -1;
    }

    _PyTok_Off start = source->len;
    memcpy(source->bytes + start, bytes, len);
    source->len += len;
    ଯଦି (append_newline) {
        source->bytes[source->len++] = '\n';
    }
    source->bytes[source->len] = '\0';
    source->nlines++;
    ଫେରନ୍ତୁ source->base_offset + start;
}

_PyTok_Off
Py_Tok_Source_Append_ଧାଡ଼ି(_PyTok_SourceText *source, ଅଚଳ ଅକ୍ଷର *bytes,
                        Py_ssize_t len)
{
    ଫେରନ୍ତୁ append_ଧାଡ଼ି(source, bytes, len, 0);
}

_PyTok_Off
Py_Tok_Source_Append_ଧାଡ଼ି_With_Newline(_PyTok_SourceText *source, ଅଚଳ ଅକ୍ଷର *bytes,
                                   Py_ssize_t len)
{
    ଫେରନ୍ତୁ append_ଧାଡ଼ି(source, bytes, len, 1);
}

ଅଚଳ ଅକ୍ଷର *
Py_Tok_Source_ଧାଡ଼ି_View(ଅଚଳ _PyTok_SourceText *source, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    ପ୍ରତିଜ୍ଞା(len != ଖାଲି);
    ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି = _PyTok_SourceData(source);
    ଅଚଳ ଅକ୍ଷର *end = ଧାଡ଼ି + source->len;
    ଯେତେବେଳେ (lineno > 1) {
        ଅଚଳ ଅକ୍ଷର *newline = memchr(ଧାଡ଼ି, '\n', end - ଧାଡ଼ି);
        ଯଦି (newline == ଖାଲି) {
            ଭାଙ୍ଗ;
        }
        ଧାଡ଼ି = newline + 1;
        lineno--;
    }
    ଅଚଳ ଅକ୍ଷର *newline = memchr(ଧାଡ଼ି, '\n', end - ଧାଡ଼ି);
    *len = (newline != ଖାଲି ? newline : end) - ଧାଡ଼ି;
    ଫେରନ୍ତୁ ଧାଡ଼ି;
}
