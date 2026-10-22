#અન્તર્ભાવ "Python.h"
#અન્તર્ભાવ "pycore_pymem.h"  // PYMEM_DEADBYTE

#અન્તર્ભાવ "source.h"

#define SOURCE_BUFFER_GROWTH_FACTOR 2

શૂન્ય
_PyTok_SourceInit(_PyTok_SourceText *source)
{
    *source = (_PyTok_SourceText){0};
}

શૂન્ય
_PyTok_SourceClear(_PyTok_SourceText *source)
{
    PyMem_Free(source->bytes);
    _PyTok_SourceInit(source);
}

શૂન્ય
_PyTok_SourceDiscard(_PyTok_SourceText *source)
{
    પ્રતિજ્ઞા(source->base_offset <= PY_SSIZE_T_MAX - source->len);
    source->base_offset += source->len;
    source->len = 0;
    જો (source->bytes != ખાલી) {
        source->bytes[0] = '\0';
    }
    source->nlines = 0;
}

સ્થિર પૂર્ણાંક
reserve_bytes(_PyTok_SourceText *source, Py_ssize_t needed)
{
    જો (needed <= source->cap) {
        પાછા 0;
    }
    Py_ssize_t cap = source->cap > 0 ? source->cap : BUFSIZ;
    જ્યારે (cap < needed) {
        જો (cap > PY_SSIZE_T_MAX / SOURCE_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            તોડો;
        }
        cap *= SOURCE_BUFFER_GROWTH_FACTOR;
    }
    અક્ષર *bytes;
#ifdef Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    bytes = PyMem_Malloc(cap);
    જો (bytes != ખાલી && source->len > 0) {
        memcpy(bytes, source->bytes, source->len);
    }
#નહીંતર
    bytes = PyMem_Realloc(source->bytes, cap);
#endif
    જો (bytes == ખાલી) {
        PyErr_NoMemory();
        પાછા -1;
    }
#ifdef Py_DEBUG
    જો (source->bytes != ખાલી) {
        memset(source->bytes, PYMEM_DEADBYTE, source->cap);
        PyMem_Free(source->bytes);
    }
#endif
    source->bytes = bytes;
    source->cap = cap;
    પાછા 0;
}

સ્થિર પૂર્ણાંક
validate_લીટી(અચલ _PyTok_SourceText *source, અચલ અક્ષર *bytes,
              Py_ssize_t len, પૂર્ણાંક append_newline)
{
    જો (len < 0 || (len == 0 && !append_newline) || bytes == ખાલી ||
            (source->nlines > 0 &&
             (source->len == 0 || source->bytes[source->len - 1] != '\n'))) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        પાછા -1;
    }
    અચલ અક્ષર *newline = memchr(bytes, '\n', len);
    જો (newline != ખાલી && (append_newline || newline != bytes + len - 1)) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        પાછા -1;
    }
    જો (source->nlines == INT_MAX ||
            (source->nlines == INT_MAX - 1 && (newline != ખાલી || append_newline))) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        પાછા -1;
    }
    પાછા 0;
}

સ્થિર _PyTok_Off
append_લીટી(_PyTok_SourceText *source, અચલ અક્ષર *bytes,
            Py_ssize_t len, પૂર્ણાંક append_newline)
{
    જો (validate_લીટી(source, bytes, len, append_newline) < 0) {
        પાછા -1;
    }
    જો (source->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            source->base_offset > PY_SSIZE_T_MAX - source->len - len - append_newline) {
        PyErr_NoMemory();
        પાછા -1;
    }
    જો (reserve_bytes(source, source->len + len + append_newline + 1) < 0) {
        પાછા -1;
    }

    _PyTok_Off start = source->len;
    memcpy(source->bytes + start, bytes, len);
    source->len += len;
    જો (append_newline) {
        source->bytes[source->len++] = '\n';
    }
    source->bytes[source->len] = '\0';
    source->nlines++;
    પાછા source->base_offset + start;
}

_PyTok_Off
Py_Tok_Source_Append_લીટી(_PyTok_SourceText *source, અચલ અક્ષર *bytes,
                        Py_ssize_t len)
{
    પાછા append_લીટી(source, bytes, len, 0);
}

_PyTok_Off
Py_Tok_Source_Append_લીટી_With_Newline(_PyTok_SourceText *source, અચલ અક્ષર *bytes,
                                   Py_ssize_t len)
{
    પાછા append_લીટી(source, bytes, len, 1);
}

અચલ અક્ષર *
Py_Tok_Source_લીટી_View(અચલ _PyTok_SourceText *source, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    પ્રતિજ્ઞા(len != ખાલી);
    અચલ અક્ષર *લીટી = _PyTok_SourceData(source);
    અચલ અક્ષર *end = લીટી + source->len;
    જ્યારે (lineno > 1) {
        અચલ અક્ષર *newline = memchr(લીટી, '\n', end - લીટી);
        જો (newline == ખાલી) {
            તોડો;
        }
        લીટી = newline + 1;
        lineno--;
    }
    અચલ અક્ષર *newline = memchr(લીટી, '\n', end - લીટી);
    *len = (newline != ખાલી ? newline : end) - લીટી;
    પાછા લીટી;
}
