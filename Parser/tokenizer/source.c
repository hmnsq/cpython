#شامل "Python.h"
#شامل "pycore_pymem.h"  // PYMEM_DEADBYTE

#شامل "source.h"

#تعریف_کرو ماخذ_بفر_GROWTH_ضربی_عدد 2

خالی
Py_Tok_ماخذ_Init(Py_Tok_ماخذ_متن *ماخذ)
{
    *ماخذ = (Py_Tok_ماخذ_متن){0};
}

خالی
Py_Tok_ماخذ_Clear(Py_Tok_ماخذ_متن *ماخذ)
{
    PyMem_Free(ماخذ->بائٹیں);
    Py_Tok_ماخذ_Init(ماخذ);
}

خالی
Py_Tok_ماخذ_Discard(Py_Tok_ماخذ_متن *ماخذ)
{
    أكد(ماخذ->بنیاد_انحراف <= PY_SSIZE_T_MAX - ماخذ->len);
    ماخذ->بنیاد_انحراف += ماخذ->len;
    ماخذ->len = 0;
    اگر (ماخذ->بائٹیں != تہی) {
        ماخذ->بائٹیں[0] = '\0';
    }
    ماخذ->nlines = 0;
}

ساکن صحیح
reserve_بائٹیں(Py_Tok_ماخذ_متن *ماخذ, Py_ssize_t needed)
{
    اگر (needed <= ماخذ->cap) {
        واپس 0;
    }
    Py_ssize_t cap = ماخذ->cap > 0 ? ماخذ->cap : BUFSIZ;
    جب (cap < needed) {
        اگر (cap > PY_SSIZE_T_MAX / ماخذ_بفر_GROWTH_ضربی_عدد) {
            cap = needed;
            توڑو;
        }
        cap *= ماخذ_بفر_GROWTH_ضربی_عدد;
    }
    حرف *بائٹیں;
#اگر_تعریف_شدہ Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    بائٹیں = PyMem_Malloc(cap);
    اگر (بائٹیں != تہی && ماخذ->len > 0) {
        حافظہ_نقل(بائٹیں, ماخذ->بائٹیں, ماخذ->len);
    }
#ورنہ
    بائٹیں = PyMem_Realloc(ماخذ->بائٹیں, cap);
#اگر_ختم
    اگر (بائٹیں == تہی) {
        Py_Err_No_حافظہ();
        واپس -1;
    }
#اگر_تعریف_شدہ Py_DEBUG
    اگر (ماخذ->بائٹیں != تہی) {
        memset(ماخذ->بائٹیں, PYMEM_DEADBYTE, ماخذ->cap);
        PyMem_Free(ماخذ->بائٹیں);
    }
#اگر_ختم
    ماخذ->بائٹیں = بائٹیں;
    ماخذ->cap = cap;
    واپس 0;
}

ساکن صحیح
validate_سطر(ثابت Py_Tok_ماخذ_متن *ماخذ, ثابت حرف *بائٹیں,
              Py_ssize_t len, صحیح append_newline)
{
    اگر (len < 0 || (len == 0 && !append_newline) || بائٹیں == تہی ||
            (ماخذ->nlines > 0 &&
             (ماخذ->len == 0 || ماخذ->بائٹیں[ماخذ->len - 1] != '\n'))) {
        Py_Err_سیٹ_String(PyExc_SystemError, "invalid tokenizer source line");
        واپس -1;
    }
    ثابت حرف *newline = memchr(بائٹیں, '\n', len);
    اگر (newline != تہی && (append_newline || newline != بائٹیں + len - 1)) {
        Py_Err_سیٹ_String(PyExc_SystemError, "invalid tokenizer source line");
        واپس -1;
    }
    اگر (ماخذ->nlines == زیادہ_سے_زیادہ_عدد ||
            (ماخذ->nlines == زیادہ_سے_زیادہ_عدد - 1 && (newline != تہی || append_newline))) {
        Py_Err_سیٹ_String(Py_Exc_حد_سے_تجاوز_Error, "too many tokenizer source lines");
        واپس -1;
    }
    واپس 0;
}

ساکن _PyTok_Off
append_سطر(Py_Tok_ماخذ_متن *ماخذ, ثابت حرف *بائٹیں,
            Py_ssize_t len, صحیح append_newline)
{
    اگر (validate_سطر(ماخذ, بائٹیں, len, append_newline) < 0) {
        واپس -1;
    }
    اگر (ماخذ->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            ماخذ->بنیاد_انحراف > PY_SSIZE_T_MAX - ماخذ->len - len - append_newline) {
        Py_Err_No_حافظہ();
        واپس -1;
    }
    اگر (reserve_بائٹیں(ماخذ, ماخذ->len + len + append_newline + 1) < 0) {
        واپس -1;
    }

    _PyTok_Off آغاز = ماخذ->len;
    حافظہ_نقل(ماخذ->بائٹیں + آغاز, بائٹیں, len);
    ماخذ->len += len;
    اگر (append_newline) {
        ماخذ->بائٹیں[ماخذ->len++] = '\n';
    }
    ماخذ->بائٹیں[ماخذ->len] = '\0';
    ماخذ->nlines++;
    واپس ماخذ->بنیاد_انحراف + آغاز;
}

_PyTok_Off
Py_Tok_ماخذ_Append_سطر(Py_Tok_ماخذ_متن *ماخذ, ثابت حرف *بائٹیں,
                        Py_ssize_t len)
{
    واپس append_سطر(ماخذ, بائٹیں, len, 0);
}

_PyTok_Off
Py_Tok_ماخذ_Append_سطر_With_Newline(Py_Tok_ماخذ_متن *ماخذ, ثابت حرف *بائٹیں,
                                   Py_ssize_t len)
{
    واپس append_سطر(ماخذ, بائٹیں, len, 1);
}

ثابت حرف *
Py_Tok_ماخذ_سطر_View(ثابت Py_Tok_ماخذ_متن *ماخذ, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    أكد(len != تہی);
    ثابت حرف *سطر = Py_Tok_ماخذ_ڈیٹا(ماخذ);
    ثابت حرف *انتہا = سطر + ماخذ->len;
    جب (lineno > 1) {
        ثابت حرف *newline = memchr(سطر, '\n', انتہا - سطر);
        اگر (newline == تہی) {
            توڑو;
        }
        سطر = newline + 1;
        lineno--;
    }
    ثابت حرف *newline = memchr(سطر, '\n', انتہا - سطر);
    *len = (newline != تہی ? newline : انتہا) - سطر;
    واپس سطر;
}
