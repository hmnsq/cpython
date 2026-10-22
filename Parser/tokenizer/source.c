#ضمن "Python.h"
#ضمن "pycore_pymem.h"  // PYMEM_DEADBYTE

#ضمن "source.h"

#تعریف_کړه سرچینه_بفر_GROWTH_ضریب 2

فراغ
Py_Tok_سرچینه_Init(Py_Tok_سرچینه_متن *سرچینه)
{
    *سرچینه = (Py_Tok_سرچینه_متن){0};
}

فراغ
Py_Tok_سرچینه_Clear(Py_Tok_سرچینه_متن *سرچینه)
{
    PyMem_Free(سرچینه->بایټونه);
    Py_Tok_سرچینه_Init(سرچینه);
}

فراغ
Py_Tok_سرچینه_Discard(Py_Tok_سرچینه_متن *سرچینه)
{
    أكد(سرچینه->بنسټ_دننه_واټن <= PY_SSIZE_T_MAX - سرچینه->len);
    سرچینه->بنسټ_دننه_واټن += سرچینه->len;
    سرچینه->len = 0;
    إذا (سرچینه->بایټونه != عدم) {
        سرچینه->بایټونه[0] = '\0';
    }
    سرچینه->nlines = 0;
}

ساكن صحيح
reserve_بایټونه(Py_Tok_سرچینه_متن *سرچینه, Py_ssize_t needed)
{
    إذا (needed <= سرچینه->cap) {
        أرجع 0;
    }
    Py_ssize_t cap = سرچینه->cap > 0 ? سرچینه->cap : BUFSIZ;
    طالما (cap < needed) {
        إذا (cap > PY_SSIZE_T_MAX / سرچینه_بفر_GROWTH_ضریب) {
            cap = needed;
            اكسر;
        }
        cap *= سرچینه_بفر_GROWTH_ضریب;
    }
    حرف *بایټونه;
#که_تعریف_شوی Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    بایټونه = PyMem_Malloc(cap);
    إذا (بایټونه != عدم && سرچینه->len > 0) {
        حافظه_نقل(بایټونه, سرچینه->بایټونه, سرچینه->len);
    }
#وإلا
    بایټونه = PyMem_Realloc(سرچینه->بایټونه, cap);
#د_که_پای
    إذا (بایټونه == عدم) {
        Py_Err_No_حافظه();
        أرجع -1;
    }
#که_تعریف_شوی Py_DEBUG
    إذا (سرچینه->بایټونه != عدم) {
        memset(سرچینه->بایټونه, PYMEM_DEADBYTE, سرچینه->cap);
        PyMem_Free(سرچینه->بایټونه);
    }
#د_که_پای
    سرچینه->بایټونه = بایټونه;
    سرچینه->cap = cap;
    أرجع 0;
}

ساكن صحيح
validate_کرښه(ثابت Py_Tok_سرچینه_متن *سرچینه, ثابت حرف *بایټونه,
              Py_ssize_t len, صحيح append_newline)
{
    إذا (len < 0 || (len == 0 && !append_newline) || بایټونه == عدم ||
            (سرچینه->nlines > 0 &&
             (سرچینه->len == 0 || سرچینه->بایټونه[سرچینه->len - 1] != '\n'))) {
        Py_Err_سېټ_String(PyExc_SystemError, "invalid tokenizer source line");
        أرجع -1;
    }
    ثابت حرف *newline = memchr(بایټونه, '\n', len);
    إذا (newline != عدم && (append_newline || newline != بایټونه + len - 1)) {
        Py_Err_سېټ_String(PyExc_SystemError, "invalid tokenizer source line");
        أرجع -1;
    }
    إذا (سرچینه->nlines == لوی_عدد ||
            (سرچینه->nlines == لوی_عدد - 1 && (newline != عدم || append_newline))) {
        Py_Err_سېټ_String(Py_Exc_ډکېدنه_Error, "too many tokenizer source lines");
        أرجع -1;
    }
    أرجع 0;
}

ساكن _PyTok_Off
append_کرښه(Py_Tok_سرچینه_متن *سرچینه, ثابت حرف *بایټونه,
            Py_ssize_t len, صحيح append_newline)
{
    إذا (validate_کرښه(سرچینه, بایټونه, len, append_newline) < 0) {
        أرجع -1;
    }
    إذا (سرچینه->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            سرچینه->بنسټ_دننه_واټن > PY_SSIZE_T_MAX - سرچینه->len - len - append_newline) {
        Py_Err_No_حافظه();
        أرجع -1;
    }
    إذا (reserve_بایټونه(سرچینه, سرچینه->len + len + append_newline + 1) < 0) {
        أرجع -1;
    }

    _PyTok_Off پیل = سرچینه->len;
    حافظه_نقل(سرچینه->بایټونه + پیل, بایټونه, len);
    سرچینه->len += len;
    إذا (append_newline) {
        سرچینه->بایټونه[سرچینه->len++] = '\n';
    }
    سرچینه->بایټونه[سرچینه->len] = '\0';
    سرچینه->nlines++;
    أرجع سرچینه->بنسټ_دننه_واټن + پیل;
}

_PyTok_Off
Py_Tok_سرچینه_Append_کرښه(Py_Tok_سرچینه_متن *سرچینه, ثابت حرف *بایټونه,
                        Py_ssize_t len)
{
    أرجع append_کرښه(سرچینه, بایټونه, len, 0);
}

_PyTok_Off
Py_Tok_سرچینه_Append_کرښه_With_Newline(Py_Tok_سرچینه_متن *سرچینه, ثابت حرف *بایټونه,
                                   Py_ssize_t len)
{
    أرجع append_کرښه(سرچینه, بایټونه, len, 1);
}

ثابت حرف *
Py_Tok_سرچینه_کرښه_View(ثابت Py_Tok_سرچینه_متن *سرچینه, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    أكد(len != عدم);
    ثابت حرف *کرښه = Py_Tok_سرچینه_معلومات(سرچینه);
    ثابت حرف *پای_ټکی = کرښه + سرچینه->len;
    طالما (lineno > 1) {
        ثابت حرف *newline = memchr(کرښه, '\n', پای_ټکی - کرښه);
        إذا (newline == عدم) {
            اكسر;
        }
        کرښه = newline + 1;
        lineno--;
    }
    ثابت حرف *newline = memchr(کرښه, '\n', پای_ټکی - کرښه);
    *len = (newline != عدم ? newline : پای_ټکی) - کرښه;
    أرجع کرښه;
}
