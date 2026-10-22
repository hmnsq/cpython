#ضمن "Python.h"
#ضمن "pycore_pymem.h"  // PYMEM_DEADBYTE

#ضمن "source.h"

#تعریف_کرو سروت_بفر_GROWTH_ضارب 2

فراغ
Py_Tok_سروت_Init(Py_Tok_سروت_متن *سروت)
{
    *سروت = (Py_Tok_سروت_متن){0};
}

فراغ
Py_Tok_سروت_Clear(Py_Tok_سروت_متن *سروت)
{
    PyMem_Free(سروت->بائٹاں);
    Py_Tok_سروت_Init(سروت);
}

فراغ
Py_Tok_سروت_Discard(Py_Tok_سروت_متن *سروت)
{
    أكد(سروت->ادھار_وتھ <= PY_SSIZE_T_MAX - سروت->len);
    سروت->ادھار_وتھ += سروت->len;
    سروت->len = 0;
    إذا (سروت->بائٹاں != عدم) {
        سروت->بائٹاں[0] = '\0';
    }
    سروت->nlines = 0;
}

ساكن صحيح
reserve_بائٹاں(Py_Tok_سروت_متن *سروت, Py_ssize_t needed)
{
    إذا (needed <= سروت->cap) {
        أرجع 0;
    }
    Py_ssize_t cap = سروت->cap > 0 ? سروت->cap : BUFSIZ;
    طالما (cap < needed) {
        إذا (cap > PY_SSIZE_T_MAX / سروت_بفر_GROWTH_ضارب) {
            cap = needed;
            اكسر;
        }
        cap *= سروت_بفر_GROWTH_ضارب;
    }
    حرف *بائٹاں;
#جے_تعریف_شدہ Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    بائٹاں = PyMem_Malloc(cap);
    إذا (بائٹاں != عدم && سروت->len > 0) {
        حافظہ_نقل(بائٹاں, سروت->بائٹاں, سروت->len);
    }
#وإلا
    بائٹاں = PyMem_Realloc(سروت->بائٹاں, cap);
#جے_انت
    إذا (بائٹاں == عدم) {
        Py_Err_No_حافظہ();
        أرجع -1;
    }
#جے_تعریف_شدہ Py_DEBUG
    إذا (سروت->بائٹاں != عدم) {
        memset(سروت->بائٹاں, PYMEM_DEADBYTE, سروت->cap);
        PyMem_Free(سروت->بائٹاں);
    }
#جے_انت
    سروت->بائٹاں = بائٹاں;
    سروت->cap = cap;
    أرجع 0;
}

ساكن صحيح
validate_سطر(ثابت Py_Tok_سروت_متن *سروت, ثابت حرف *بائٹاں,
              Py_ssize_t len, صحيح append_newline)
{
    إذا (len < 0 || (len == 0 && !append_newline) || بائٹاں == عدم ||
            (سروت->nlines > 0 &&
             (سروت->len == 0 || سروت->بائٹاں[سروت->len - 1] != '\n'))) {
        Py_Err_سیٹ_String(PyExc_SystemError, "invalid tokenizer source line");
        أرجع -1;
    }
    ثابت حرف *newline = memchr(بائٹاں, '\n', len);
    إذا (newline != عدم && (append_newline || newline != بائٹاں + len - 1)) {
        Py_Err_سیٹ_String(PyExc_SystemError, "invalid tokenizer source line");
        أرجع -1;
    }
    إذا (سروت->nlines == ودھ_توں_ودھ_عدد ||
            (سروت->nlines == ودھ_توں_ودھ_عدد - 1 && (newline != عدم || append_newline))) {
        Py_Err_سیٹ_String(Py_Exc_حد_توں_ودھ_Error, "too many tokenizer source lines");
        أرجع -1;
    }
    أرجع 0;
}

ساكن _PyTok_Off
append_سطر(Py_Tok_سروت_متن *سروت, ثابت حرف *بائٹاں,
            Py_ssize_t len, صحيح append_newline)
{
    إذا (validate_سطر(سروت, بائٹاں, len, append_newline) < 0) {
        أرجع -1;
    }
    إذا (سروت->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            سروت->ادھار_وتھ > PY_SSIZE_T_MAX - سروت->len - len - append_newline) {
        Py_Err_No_حافظہ();
        أرجع -1;
    }
    إذا (reserve_بائٹاں(سروت, سروت->len + len + append_newline + 1) < 0) {
        أرجع -1;
    }

    _PyTok_Off شروع = سروت->len;
    حافظہ_نقل(سروت->بائٹاں + شروع, بائٹاں, len);
    سروت->len += len;
    إذا (append_newline) {
        سروت->بائٹاں[سروت->len++] = '\n';
    }
    سروت->بائٹاں[سروت->len] = '\0';
    سروت->nlines++;
    أرجع سروت->ادھار_وتھ + شروع;
}

_PyTok_Off
Py_Tok_سروت_Append_سطر(Py_Tok_سروت_متن *سروت, ثابت حرف *بائٹاں,
                        Py_ssize_t len)
{
    أرجع append_سطر(سروت, بائٹاں, len, 0);
}

_PyTok_Off
Py_Tok_سروت_Append_سطر_With_Newline(Py_Tok_سروت_متن *سروت, ثابت حرف *بائٹاں,
                                   Py_ssize_t len)
{
    أرجع append_سطر(سروت, بائٹاں, len, 1);
}

ثابت حرف *
Py_Tok_سروت_سطر_View(ثابت Py_Tok_سروت_متن *سروت, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    أكد(len != عدم);
    ثابت حرف *سطر = Py_Tok_سروت_انکڑے(سروت);
    ثابت حرف *سرا = سطر + سروت->len;
    طالما (lineno > 1) {
        ثابت حرف *newline = memchr(سطر, '\n', سرا - سطر);
        إذا (newline == عدم) {
            اكسر;
        }
        سطر = newline + 1;
        lineno--;
    }
    ثابت حرف *newline = memchr(سطر, '\n', سرا - سطر);
    *len = (newline != عدم ? newline : سرا) - سطر;
    أرجع سطر;
}
