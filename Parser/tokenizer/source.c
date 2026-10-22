#ਅਨ੍ਤਰ੍ਭਾਵ "Python.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_pymem.h"  // PYMEM_DEADBYTE

#ਅਨ੍ਤਰ੍ਭਾਵ "source.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਰੋਤ_ਬਫਰ_GROWTH_ਗੁਣਕ 2

ਖਾਲੀ
Py_Tok_ਸਰੋਤ_Init(Py_Tok_ਸਰੋਤ_ਪਾਠ *ਸਰੋਤ)
{
    *ਸਰੋਤ = (Py_Tok_ਸਰੋਤ_ਪਾਠ){0};
}

ਖਾਲੀ
Py_Tok_ਸਰੋਤ_Clear(Py_Tok_ਸਰੋਤ_ਪਾਠ *ਸਰੋਤ)
{
    PyMem_Free(ਸਰੋਤ->ਬਾਈਟਾਂ);
    Py_Tok_ਸਰੋਤ_Init(ਸਰੋਤ);
}

ਖਾਲੀ
Py_Tok_ਸਰੋਤ_Discard(Py_Tok_ਸਰੋਤ_ਪਾਠ *ਸਰੋਤ)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(ਸਰੋਤ->ਅਧਾਰ_ਵਿੱਥ <= PY_SSIZE_T_MAX - ਸਰੋਤ->len);
    ਸਰੋਤ->ਅਧਾਰ_ਵਿੱਥ += ਸਰੋਤ->len;
    ਸਰੋਤ->len = 0;
    ਜੇ (ਸਰੋਤ->ਬਾਈਟਾਂ != ਨਿਰਾ) {
        ਸਰੋਤ->ਬਾਈਟਾਂ[0] = '\0';
    }
    ਸਰੋਤ->nlines = 0;
}

ਸਥਿਰ ਪੂਰਨ
reserve_ਬਾਈਟਾਂ(Py_Tok_ਸਰੋਤ_ਪਾਠ *ਸਰੋਤ, Py_ssize_t needed)
{
    ਜੇ (needed <= ਸਰੋਤ->cap) {
        ਵਾਪਸ 0;
    }
    Py_ssize_t cap = ਸਰੋਤ->cap > 0 ? ਸਰੋਤ->cap : BUFSIZ;
    ਜਦੋਂ (cap < needed) {
        ਜੇ (cap > PY_SSIZE_T_MAX / ਸਰੋਤ_ਬਫਰ_GROWTH_ਗੁਣਕ) {
            cap = needed;
            ਰੋਕੋ;
        }
        cap *= ਸਰੋਤ_ਬਫਰ_GROWTH_ਗੁਣਕ;
    }
    ਅੱਖਰ *ਬਾਈਟਾਂ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    ਬਾਈਟਾਂ = PyMem_Malloc(cap);
    ਜੇ (ਬਾਈਟਾਂ != ਨਿਰਾ && ਸਰੋਤ->len > 0) {
        ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਬਾਈਟਾਂ, ਸਰੋਤ->ਬਾਈਟਾਂ, ਸਰੋਤ->len);
    }
#ਨਹੀਂਤਾਂ
    ਬਾਈਟਾਂ = PyMem_Realloc(ਸਰੋਤ->ਬਾਈਟਾਂ, cap);
#ਜੇ_ਅੰਤ
    ਜੇ (ਬਾਈਟਾਂ == ਨਿਰਾ) {
        Py_Err_No_ਸਮ੍ਰਿਤੀ();
        ਵਾਪਸ -1;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ Py_DEBUG
    ਜੇ (ਸਰੋਤ->ਬਾਈਟਾਂ != ਨਿਰਾ) {
        memset(ਸਰੋਤ->ਬਾਈਟਾਂ, PYMEM_DEADBYTE, ਸਰੋਤ->cap);
        PyMem_Free(ਸਰੋਤ->ਬਾਈਟਾਂ);
    }
#ਜੇ_ਅੰਤ
    ਸਰੋਤ->ਬਾਈਟਾਂ = ਬਾਈਟਾਂ;
    ਸਰੋਤ->cap = cap;
    ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ
validate_ਸਤਰ(ਅਚਲ Py_Tok_ਸਰੋਤ_ਪਾਠ *ਸਰੋਤ, ਅਚਲ ਅੱਖਰ *ਬਾਈਟਾਂ,
              Py_ssize_t len, ਪੂਰਨ append_newline)
{
    ਜੇ (len < 0 || (len == 0 && !append_newline) || ਬਾਈਟਾਂ == ਨਿਰਾ ||
            (ਸਰੋਤ->nlines > 0 &&
             (ਸਰੋਤ->len == 0 || ਸਰੋਤ->ਬਾਈਟਾਂ[ਸਰੋਤ->len - 1] != '\n'))) {
        Py_Err_ਸੈੱਟ_String(PyExc_SystemError, "invalid tokenizer source line");
        ਵਾਪਸ -1;
    }
    ਅਚਲ ਅੱਖਰ *newline = memchr(ਬਾਈਟਾਂ, '\n', len);
    ਜੇ (newline != ਨਿਰਾ && (append_newline || newline != ਬਾਈਟਾਂ + len - 1)) {
        Py_Err_ਸੈੱਟ_String(PyExc_SystemError, "invalid tokenizer source line");
        ਵਾਪਸ -1;
    }
    ਜੇ (ਸਰੋਤ->nlines == ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ ||
            (ਸਰੋਤ->nlines == ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ - 1 && (newline != ਨਿਰਾ || append_newline))) {
        Py_Err_ਸੈੱਟ_String(Py_Exc_ਉਛਾਲ_Error, "too many tokenizer source lines");
        ਵਾਪਸ -1;
    }
    ਵਾਪਸ 0;
}

ਸਥਿਰ _PyTok_Off
append_ਸਤਰ(Py_Tok_ਸਰੋਤ_ਪਾਠ *ਸਰੋਤ, ਅਚਲ ਅੱਖਰ *ਬਾਈਟਾਂ,
            Py_ssize_t len, ਪੂਰਨ append_newline)
{
    ਜੇ (validate_ਸਤਰ(ਸਰੋਤ, ਬਾਈਟਾਂ, len, append_newline) < 0) {
        ਵਾਪਸ -1;
    }
    ਜੇ (ਸਰੋਤ->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            ਸਰੋਤ->ਅਧਾਰ_ਵਿੱਥ > PY_SSIZE_T_MAX - ਸਰੋਤ->len - len - append_newline) {
        Py_Err_No_ਸਮ੍ਰਿਤੀ();
        ਵਾਪਸ -1;
    }
    ਜੇ (reserve_ਬਾਈਟਾਂ(ਸਰੋਤ, ਸਰੋਤ->len + len + append_newline + 1) < 0) {
        ਵਾਪਸ -1;
    }

    _PyTok_Off ਸ਼ੁਰੂ = ਸਰੋਤ->len;
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਸਰੋਤ->ਬਾਈਟਾਂ + ਸ਼ੁਰੂ, ਬਾਈਟਾਂ, len);
    ਸਰੋਤ->len += len;
    ਜੇ (append_newline) {
        ਸਰੋਤ->ਬਾਈਟਾਂ[ਸਰੋਤ->len++] = '\n';
    }
    ਸਰੋਤ->ਬਾਈਟਾਂ[ਸਰੋਤ->len] = '\0';
    ਸਰੋਤ->nlines++;
    ਵਾਪਸ ਸਰੋਤ->ਅਧਾਰ_ਵਿੱਥ + ਸ਼ੁਰੂ;
}

_PyTok_Off
Py_Tok_ਸਰੋਤ_Append_ਸਤਰ(Py_Tok_ਸਰੋਤ_ਪਾਠ *ਸਰੋਤ, ਅਚਲ ਅੱਖਰ *ਬਾਈਟਾਂ,
                        Py_ssize_t len)
{
    ਵਾਪਸ append_ਸਤਰ(ਸਰੋਤ, ਬਾਈਟਾਂ, len, 0);
}

_PyTok_Off
Py_Tok_ਸਰੋਤ_Append_ਸਤਰ_With_Newline(Py_Tok_ਸਰੋਤ_ਪਾਠ *ਸਰੋਤ, ਅਚਲ ਅੱਖਰ *ਬਾਈਟਾਂ,
                                   Py_ssize_t len)
{
    ਵਾਪਸ append_ਸਤਰ(ਸਰੋਤ, ਬਾਈਟਾਂ, len, 1);
}

ਅਚਲ ਅੱਖਰ *
Py_Tok_ਸਰੋਤ_ਸਤਰ_View(ਅਚਲ Py_Tok_ਸਰੋਤ_ਪਾਠ *ਸਰੋਤ, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(len != ਨਿਰਾ);
    ਅਚਲ ਅੱਖਰ *ਸਤਰ = Py_Tok_ਸਰੋਤ_ਅੰਕੜੇ(ਸਰੋਤ);
    ਅਚਲ ਅੱਖਰ *ਸਿਰਾ = ਸਤਰ + ਸਰੋਤ->len;
    ਜਦੋਂ (lineno > 1) {
        ਅਚਲ ਅੱਖਰ *newline = memchr(ਸਤਰ, '\n', ਸਿਰਾ - ਸਤਰ);
        ਜੇ (newline == ਨਿਰਾ) {
            ਰੋਕੋ;
        }
        ਸਤਰ = newline + 1;
        lineno--;
    }
    ਅਚਲ ਅੱਖਰ *newline = memchr(ਸਤਰ, '\n', ਸਿਰਾ - ਸਤਰ);
    *len = (newline != ਨਿਰਾ ? newline : ਸਿਰਾ) - ਸਤਰ;
    ਵਾਪਸ ਸਤਰ;
}
