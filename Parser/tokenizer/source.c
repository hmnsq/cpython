#അന്തര്ഭാവ "Python.h"
#അന്തര്ഭാവ "pycore_pymem.h"  // PYMEM_DEADBYTE

#അന്തര്ഭാവ "source.h"

#define SOURCE_BUFFER_GROWTH_FACTOR 2

ശൂന്യം
_PyTok_SourceInit(_PyTok_SourceText *source)
{
    *source = (_PyTok_SourceText){0};
}

ശൂന്യം
_PyTok_SourceClear(_PyTok_SourceText *source)
{
    PyMem_Free(source->bytes);
    _PyTok_SourceInit(source);
}

ശൂന്യം
_PyTok_SourceDiscard(_PyTok_SourceText *source)
{
    പ്രതിജ്ഞാ(source->base_offset <= PY_SSIZE_T_MAX - source->len);
    source->base_offset += source->len;
    source->len = 0;
    എങ്കിൽ (source->bytes != ശൂന്യ) {
        source->bytes[0] = '\0';
    }
    source->nlines = 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
reserve_bytes(_PyTok_SourceText *source, Py_ssize_t needed)
{
    എങ്കിൽ (needed <= source->cap) {
        തിരികെ 0;
    }
    Py_ssize_t cap = source->cap > 0 ? source->cap : BUFSIZ;
    ആകുമ്പോൾ (cap < needed) {
        എങ്കിൽ (cap > PY_SSIZE_T_MAX / SOURCE_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            നിർത്തുക;
        }
        cap *= SOURCE_BUFFER_GROWTH_FACTOR;
    }
    അക്ഷരം *bytes;
#ifdef Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    bytes = PyMem_Malloc(cap);
    എങ്കിൽ (bytes != ശൂന്യ && source->len > 0) {
        memcpy(bytes, source->bytes, source->len);
    }
#അല്ലെങ്കിൽ
    bytes = PyMem_Realloc(source->bytes, cap);
#endif
    എങ്കിൽ (bytes == ശൂന്യ) {
        PyErr_NoMemory();
        തിരികെ -1;
    }
#ifdef Py_DEBUG
    എങ്കിൽ (source->bytes != ശൂന്യ) {
        memset(source->bytes, PYMEM_DEADBYTE, source->cap);
        PyMem_Free(source->bytes);
    }
#endif
    source->bytes = bytes;
    source->cap = cap;
    തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
validate_വരി(സ്ഥിരം _PyTok_SourceText *source, സ്ഥിരം അക്ഷരം *bytes,
              Py_ssize_t len, പൂർണ്ണസംഖ്യ append_newline)
{
    എങ്കിൽ (len < 0 || (len == 0 && !append_newline) || bytes == ശൂന്യ ||
            (source->nlines > 0 &&
             (source->len == 0 || source->bytes[source->len - 1] != '\n'))) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        തിരികെ -1;
    }
    സ്ഥിരം അക്ഷരം *newline = memchr(bytes, '\n', len);
    എങ്കിൽ (newline != ശൂന്യ && (append_newline || newline != bytes + len - 1)) {
        PyErr_SetString(PyExc_SystemError, "invalid tokenizer source line");
        തിരികെ -1;
    }
    എങ്കിൽ (source->nlines == INT_MAX ||
            (source->nlines == INT_MAX - 1 && (newline != ശൂന്യ || append_newline))) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        തിരികെ -1;
    }
    തിരികെ 0;
}

സ്ഥിതം _PyTok_Off
append_വരി(_PyTok_SourceText *source, സ്ഥിരം അക്ഷരം *bytes,
            Py_ssize_t len, പൂർണ്ണസംഖ്യ append_newline)
{
    എങ്കിൽ (validate_വരി(source, bytes, len, append_newline) < 0) {
        തിരികെ -1;
    }
    എങ്കിൽ (source->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            source->base_offset > PY_SSIZE_T_MAX - source->len - len - append_newline) {
        PyErr_NoMemory();
        തിരികെ -1;
    }
    എങ്കിൽ (reserve_bytes(source, source->len + len + append_newline + 1) < 0) {
        തിരികെ -1;
    }

    _PyTok_Off start = source->len;
    memcpy(source->bytes + start, bytes, len);
    source->len += len;
    എങ്കിൽ (append_newline) {
        source->bytes[source->len++] = '\n';
    }
    source->bytes[source->len] = '\0';
    source->nlines++;
    തിരികെ source->base_offset + start;
}

_PyTok_Off
Py_Tok_Source_Append_വരി(_PyTok_SourceText *source, സ്ഥിരം അക്ഷരം *bytes,
                        Py_ssize_t len)
{
    തിരികെ append_വരി(source, bytes, len, 0);
}

_PyTok_Off
Py_Tok_Source_Append_വരി_With_Newline(_PyTok_SourceText *source, സ്ഥിരം അക്ഷരം *bytes,
                                   Py_ssize_t len)
{
    തിരികെ append_വരി(source, bytes, len, 1);
}

സ്ഥിരം അക്ഷരം *
Py_Tok_Source_വരി_View(സ്ഥിരം _PyTok_SourceText *source, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    പ്രതിജ്ഞാ(len != ശൂന്യ);
    സ്ഥിരം അക്ഷരം *വരി = _PyTok_SourceData(source);
    സ്ഥിരം അക്ഷരം *end = വരി + source->len;
    ആകുമ്പോൾ (lineno > 1) {
        സ്ഥിരം അക്ഷരം *newline = memchr(വരി, '\n', end - വരി);
        എങ്കിൽ (newline == ശൂന്യ) {
            നിർത്തുക;
        }
        വരി = newline + 1;
        lineno--;
    }
    സ്ഥിരം അക്ഷരം *newline = memchr(വരി, '\n', end - വരി);
    *len = (newline != ശൂന്യ ? newline : end) - വരി;
    തിരികെ വരി;
}
