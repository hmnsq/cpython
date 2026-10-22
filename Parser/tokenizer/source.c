#ضمن "Python.h"
#ضمن "pycore_pymem.h"  // PYMEM_DEADBYTE

#ضمن "source.h"

#وصف_ڪريو ذريعو_بفر_GROWTH_ضرب_ڪندڙ 2

فراغ
Py_Tok_ذريعو_Init(Py_Tok_ذريعو_متن *ذريعو)
{
    *ذريعو = (Py_Tok_ذريعو_متن){0};
}

فراغ
Py_Tok_ذريعو_Clear(Py_Tok_ذريعو_متن *ذريعو)
{
    PyMem_Free(ذريعو->بائيٽون);
    Py_Tok_ذريعو_Init(ذريعو);
}

فراغ
Py_Tok_ذريعو_Discard(Py_Tok_ذريعو_متن *ذريعو)
{
    أكد(ذريعو->بنياد_وٿي <= PY_SSIZE_T_MAX - ذريعو->len);
    ذريعو->بنياد_وٿي += ذريعو->len;
    ذريعو->len = 0;
    إذا (ذريعو->بائيٽون != عدم) {
        ذريعو->بائيٽون[0] = '\0';
    }
    ذريعو->nlines = 0;
}

ساكن صحيح
reserve_بائيٽون(Py_Tok_ذريعو_متن *ذريعو, Py_ssize_t needed)
{
    إذا (needed <= ذريعو->cap) {
        أرجع 0;
    }
    Py_ssize_t cap = ذريعو->cap > 0 ? ذريعو->cap : BUFSIZ;
    طالما (cap < needed) {
        إذا (cap > PY_SSIZE_T_MAX / ذريعو_بفر_GROWTH_ضرب_ڪندڙ) {
            cap = needed;
            اكسر;
        }
        cap *= ذريعو_بفر_GROWTH_ضرب_ڪندڙ;
    }
    حرف *بائيٽون;
#جيڪڏهن_وصف_ٿيل Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    بائيٽون = PyMem_Malloc(cap);
    إذا (بائيٽون != عدم && ذريعو->len > 0) {
        يادگيري_نقل(بائيٽون, ذريعو->بائيٽون, ذريعو->len);
    }
#وإلا
    بائيٽون = PyMem_Realloc(ذريعو->بائيٽون, cap);
#جيڪڏهن_پورو
    إذا (بائيٽون == عدم) {
        Py_Err_No_يادگيري();
        أرجع -1;
    }
#جيڪڏهن_وصف_ٿيل Py_DEBUG
    إذا (ذريعو->بائيٽون != عدم) {
        memset(ذريعو->بائيٽون, PYMEM_DEADBYTE, ذريعو->cap);
        PyMem_Free(ذريعو->بائيٽون);
    }
#جيڪڏهن_پورو
    ذريعو->بائيٽون = بائيٽون;
    ذريعو->cap = cap;
    أرجع 0;
}

ساكن صحيح
validate_سٽ(ثابت Py_Tok_ذريعو_متن *ذريعو, ثابت حرف *بائيٽون,
              Py_ssize_t len, صحيح append_newline)
{
    إذا (len < 0 || (len == 0 && !append_newline) || بائيٽون == عدم ||
            (ذريعو->nlines > 0 &&
             (ذريعو->len == 0 || ذريعو->بائيٽون[ذريعو->len - 1] != '\n'))) {
        Py_Err_سيٽ_String(PyExc_SystemError, "invalid tokenizer source line");
        أرجع -1;
    }
    ثابت حرف *newline = memchr(بائيٽون, '\n', len);
    إذا (newline != عدم && (append_newline || newline != بائيٽون + len - 1)) {
        Py_Err_سيٽ_String(PyExc_SystemError, "invalid tokenizer source line");
        أرجع -1;
    }
    إذا (ذريعو->nlines == وڌ_کان_وڌ_عدد ||
            (ذريعو->nlines == وڌ_کان_وڌ_عدد - 1 && (newline != عدم || append_newline))) {
        Py_Err_سيٽ_String(Py_Exc_حد_کان_مٿي_Error, "too many tokenizer source lines");
        أرجع -1;
    }
    أرجع 0;
}

ساكن _PyTok_Off
append_سٽ(Py_Tok_ذريعو_متن *ذريعو, ثابت حرف *بائيٽون,
            Py_ssize_t len, صحيح append_newline)
{
    إذا (validate_سٽ(ذريعو, بائيٽون, len, append_newline) < 0) {
        أرجع -1;
    }
    إذا (ذريعو->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            ذريعو->بنياد_وٿي > PY_SSIZE_T_MAX - ذريعو->len - len - append_newline) {
        Py_Err_No_يادگيري();
        أرجع -1;
    }
    إذا (reserve_بائيٽون(ذريعو, ذريعو->len + len + append_newline + 1) < 0) {
        أرجع -1;
    }

    _PyTok_Off شروعات = ذريعو->len;
    يادگيري_نقل(ذريعو->بائيٽون + شروعات, بائيٽون, len);
    ذريعو->len += len;
    إذا (append_newline) {
        ذريعو->بائيٽون[ذريعو->len++] = '\n';
    }
    ذريعو->بائيٽون[ذريعو->len] = '\0';
    ذريعو->nlines++;
    أرجع ذريعو->بنياد_وٿي + شروعات;
}

_PyTok_Off
Py_Tok_ذريعو_Append_سٽ(Py_Tok_ذريعو_متن *ذريعو, ثابت حرف *بائيٽون,
                        Py_ssize_t len)
{
    أرجع append_سٽ(ذريعو, بائيٽون, len, 0);
}

_PyTok_Off
Py_Tok_ذريعو_Append_سٽ_With_Newline(Py_Tok_ذريعو_متن *ذريعو, ثابت حرف *بائيٽون,
                                   Py_ssize_t len)
{
    أرجع append_سٽ(ذريعو, بائيٽون, len, 1);
}

ثابت حرف *
Py_Tok_ذريعو_سٽ_View(ثابت Py_Tok_ذريعو_متن *ذريعو, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    أكد(len != عدم);
    ثابت حرف *سٽ = Py_Tok_ذريعو_معلومات(ذريعو);
    ثابت حرف *پڇاڙي = سٽ + ذريعو->len;
    طالما (lineno > 1) {
        ثابت حرف *newline = memchr(سٽ, '\n', پڇاڙي - سٽ);
        إذا (newline == عدم) {
            اكسر;
        }
        سٽ = newline + 1;
        lineno--;
    }
    ثابت حرف *newline = memchr(سٽ, '\n', پڇاڙي - سٽ);
    *len = (newline != عدم ? newline : پڇاڙي) - سٽ;
    أرجع سٽ;
}
