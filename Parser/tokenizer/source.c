#ضمن "Python.h"
#ضمن "pycore_pymem.h"  // PYMEM_DEADBYTE

#ضمن "source.h"

#تعریف_کن منبع_میانگیر_GROWTH_ضریب 2

فراغ
Py_Tok_منبع_Init(Py_Tok_منبع_متن *منبع)
{
    *منبع = (Py_Tok_منبع_متن){0};
}

فراغ
Py_Tok_منبع_Clear(Py_Tok_منبع_متن *منبع)
{
    PyMem_Free(منبع->بایتها);
    Py_Tok_منبع_Init(منبع);
}

فراغ
Py_Tok_منبع_Discard(Py_Tok_منبع_متن *منبع)
{
    أكد(منبع->پایه_جابه_جایی <= PY_SSIZE_T_MAX - منبع->len);
    منبع->پایه_جابه_جایی += منبع->len;
    منبع->len = 0;
    إذا (منبع->بایتها != عدم) {
        منبع->بایتها[0] = '\0';
    }
    منبع->nlines = 0;
}

ساكن صحيح
reserve_بایتها(Py_Tok_منبع_متن *منبع, Py_ssize_t needed)
{
    إذا (needed <= منبع->cap) {
        أرجع 0;
    }
    Py_ssize_t cap = منبع->cap > 0 ? منبع->cap : BUFSIZ;
    طالما (cap < needed) {
        إذا (cap > PY_SSIZE_T_MAX / منبع_میانگیر_GROWTH_ضریب) {
            cap = needed;
            اكسر;
        }
        cap *= منبع_میانگیر_GROWTH_ضریب;
    }
    حرف *بایتها;
#اگر_تعریف_شده Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    بایتها = PyMem_Malloc(cap);
    إذا (بایتها != عدم && منبع->len > 0) {
        کپی_حافظه(بایتها, منبع->بایتها, منبع->len);
    }
#وإلا
    بایتها = PyMem_Realloc(منبع->بایتها, cap);
#پایان_اگر
    إذا (بایتها == عدم) {
        Py_Err_No_حافظه();
        أرجع -1;
    }
#اگر_تعریف_شده Py_DEBUG
    إذا (منبع->بایتها != عدم) {
        memset(منبع->بایتها, PYMEM_DEADBYTE, منبع->cap);
        PyMem_Free(منبع->بایتها);
    }
#پایان_اگر
    منبع->بایتها = بایتها;
    منبع->cap = cap;
    أرجع 0;
}

ساكن صحيح
validate_سطر(ثابت Py_Tok_منبع_متن *منبع, ثابت حرف *بایتها,
              Py_ssize_t len, صحيح append_newline)
{
    إذا (len < 0 || (len == 0 && !append_newline) || بایتها == عدم ||
            (منبع->nlines > 0 &&
             (منبع->len == 0 || منبع->بایتها[منبع->len - 1] != '\n'))) {
        Py_Err_مجموعه_String(PyExc_SystemError, "invalid tokenizer source line");
        أرجع -1;
    }
    ثابت حرف *newline = memchr(بایتها, '\n', len);
    إذا (newline != عدم && (append_newline || newline != بایتها + len - 1)) {
        Py_Err_مجموعه_String(PyExc_SystemError, "invalid tokenizer source line");
        أرجع -1;
    }
    إذا (منبع->nlines == بیشترین_صحیح ||
            (منبع->nlines == بیشترین_صحیح - 1 && (newline != عدم || append_newline))) {
        Py_Err_مجموعه_String(Py_Exc_سرریز_Error, "too many tokenizer source lines");
        أرجع -1;
    }
    أرجع 0;
}

ساكن _PyTok_Off
append_سطر(Py_Tok_منبع_متن *منبع, ثابت حرف *بایتها,
            Py_ssize_t len, صحيح append_newline)
{
    إذا (validate_سطر(منبع, بایتها, len, append_newline) < 0) {
        أرجع -1;
    }
    إذا (منبع->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            منبع->پایه_جابه_جایی > PY_SSIZE_T_MAX - منبع->len - len - append_newline) {
        Py_Err_No_حافظه();
        أرجع -1;
    }
    إذا (reserve_بایتها(منبع, منبع->len + len + append_newline + 1) < 0) {
        أرجع -1;
    }

    _PyTok_Off شروع = منبع->len;
    کپی_حافظه(منبع->بایتها + شروع, بایتها, len);
    منبع->len += len;
    إذا (append_newline) {
        منبع->بایتها[منبع->len++] = '\n';
    }
    منبع->بایتها[منبع->len] = '\0';
    منبع->nlines++;
    أرجع منبع->پایه_جابه_جایی + شروع;
}

_PyTok_Off
Py_Tok_منبع_Append_سطر(Py_Tok_منبع_متن *منبع, ثابت حرف *بایتها,
                        Py_ssize_t len)
{
    أرجع append_سطر(منبع, بایتها, len, 0);
}

_PyTok_Off
Py_Tok_منبع_Append_سطر_With_Newline(Py_Tok_منبع_متن *منبع, ثابت حرف *بایتها,
                                   Py_ssize_t len)
{
    أرجع append_سطر(منبع, بایتها, len, 1);
}

ثابت حرف *
Py_Tok_منبع_سطر_View(ثابت Py_Tok_منبع_متن *منبع, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    أكد(len != عدم);
    ثابت حرف *سطر = Py_Tok_منبع_داده(منبع);
    ثابت حرف *انتها = سطر + منبع->len;
    طالما (lineno > 1) {
        ثابت حرف *newline = memchr(سطر, '\n', انتها - سطر);
        إذا (newline == عدم) {
            اكسر;
        }
        سطر = newline + 1;
        lineno--;
    }
    ثابت حرف *newline = memchr(سطر, '\n', انتها - سطر);
    *len = (newline != عدم ? newline : انتها) - سطر;
    أرجع سطر;
}
