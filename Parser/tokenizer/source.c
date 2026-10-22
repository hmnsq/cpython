#అన్తర్భావ "Python.h"
#అన్తర్భావ "pycore_pymem.h"  // PYMEM_DEADBYTE

#అన్తర్భావ "source.h"

#నిర్వచించు ఆధారం_బఫరు_GROWTH_గుణకం 2

శూన్య
Py_Tok_ఆధారం_Init(Py_Tok_ఆధారం_పాఠ్యం *ఆధారం)
{
    *ఆధారం = (Py_Tok_ఆధారం_పాఠ్యం){0};
}

శూన్య
Py_Tok_ఆధారం_Clear(Py_Tok_ఆధారం_పాఠ్యం *ఆధారం)
{
    PyMem_Free(ఆధారం->బైట్లు);
    Py_Tok_ఆధారం_Init(ఆధారం);
}

శూన్య
Py_Tok_ఆధారం_Discard(Py_Tok_ఆధారం_పాఠ్యం *ఆధారం)
{
    ప్రతిజ్ఞా(ఆధారం->పునాది_స్థానభ్రంశం <= PY_SSIZE_T_MAX - ఆధారం->len);
    ఆధారం->పునాది_స్థానభ్రంశం += ఆధారం->len;
    ఆధారం->len = 0;
    ఒకవేళ (ఆధారం->బైట్లు != ఖాళీ) {
        ఆధారం->బైట్లు[0] = '\0';
    }
    ఆధారం->nlines = 0;
}

స్థిర పూర్ణాంకం
reserve_బైట్లు(Py_Tok_ఆధారం_పాఠ్యం *ఆధారం, Py_ssize_t needed)
{
    ఒకవేళ (needed <= ఆధారం->cap) {
        తిరిగి 0;
    }
    Py_ssize_t cap = ఆధారం->cap > 0 ? ఆధారం->cap : BUFSIZ;
    అయితే (cap < needed) {
        ఒకవేళ (cap > PY_SSIZE_T_MAX / ఆధారం_బఫరు_GROWTH_గుణకం) {
            cap = needed;
            ఆపు;
        }
        cap *= ఆధారం_బఫరు_GROWTH_గుణకం;
    }
    అక్షరం *బైట్లు;
#నిర్వచితమైతే Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    బైట్లు = PyMem_Malloc(cap);
    ఒకవేళ (బైట్లు != ఖాళీ && ఆధారం->len > 0) {
        జ్ఞాపకం_నకలు(బైట్లు, ఆధారం->బైట్లు, ఆధారం->len);
    }
#లేకపోతే
    బైట్లు = PyMem_Realloc(ఆధారం->బైట్లు, cap);
#అయితే_ముగింపు
    ఒకవేళ (బైట్లు == ఖాళీ) {
        Py_Err_No_జ్ఞాపకం();
        తిరిగి -1;
    }
#నిర్వచితమైతే Py_DEBUG
    ఒకవేళ (ఆధారం->బైట్లు != ఖాళీ) {
        memset(ఆధారం->బైట్లు, PYMEM_DEADBYTE, ఆధారం->cap);
        PyMem_Free(ఆధారం->బైట్లు);
    }
#అయితే_ముగింపు
    ఆధారం->బైట్లు = బైట్లు;
    ఆధారం->cap = cap;
    తిరిగి 0;
}

స్థిర పూర్ణాంకం
validate_పంక్తి(స్థిరరాశి Py_Tok_ఆధారం_పాఠ్యం *ఆధారం, స్థిరరాశి అక్షరం *బైట్లు,
              Py_ssize_t len, పూర్ణాంకం append_newline)
{
    ఒకవేళ (len < 0 || (len == 0 && !append_newline) || బైట్లు == ఖాళీ ||
            (ఆధారం->nlines > 0 &&
             (ఆధారం->len == 0 || ఆధారం->బైట్లు[ఆధారం->len - 1] != '\n'))) {
        Py_Err_సమితి_String(PyExc_SystemError, "invalid tokenizer source line");
        తిరిగి -1;
    }
    స్థిరరాశి అక్షరం *newline = memchr(బైట్లు, '\n', len);
    ఒకవేళ (newline != ఖాళీ && (append_newline || newline != బైట్లు + len - 1)) {
        Py_Err_సమితి_String(PyExc_SystemError, "invalid tokenizer source line");
        తిరిగి -1;
    }
    ఒకవేళ (ఆధారం->nlines == గరిష్ఠ_పూర్ణాంకం ||
            (ఆధారం->nlines == గరిష్ఠ_పూర్ణాంకం - 1 && (newline != ఖాళీ || append_newline))) {
        Py_Err_సమితి_String(Py_Exc_అధికప్రవాహం_Error, "too many tokenizer source lines");
        తిరిగి -1;
    }
    తిరిగి 0;
}

స్థిర _PyTok_Off
append_పంక్తి(Py_Tok_ఆధారం_పాఠ్యం *ఆధారం, స్థిరరాశి అక్షరం *బైట్లు,
            Py_ssize_t len, పూర్ణాంకం append_newline)
{
    ఒకవేళ (validate_పంక్తి(ఆధారం, బైట్లు, len, append_newline) < 0) {
        తిరిగి -1;
    }
    ఒకవేళ (ఆధారం->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            ఆధారం->పునాది_స్థానభ్రంశం > PY_SSIZE_T_MAX - ఆధారం->len - len - append_newline) {
        Py_Err_No_జ్ఞాపకం();
        తిరిగి -1;
    }
    ఒకవేళ (reserve_బైట్లు(ఆధారం, ఆధారం->len + len + append_newline + 1) < 0) {
        తిరిగి -1;
    }

    _PyTok_Off ప్రారంభం = ఆధారం->len;
    జ్ఞాపకం_నకలు(ఆధారం->బైట్లు + ప్రారంభం, బైట్లు, len);
    ఆధారం->len += len;
    ఒకవేళ (append_newline) {
        ఆధారం->బైట్లు[ఆధారం->len++] = '\n';
    }
    ఆధారం->బైట్లు[ఆధారం->len] = '\0';
    ఆధారం->nlines++;
    తిరిగి ఆధారం->పునాది_స్థానభ్రంశం + ప్రారంభం;
}

_PyTok_Off
Py_Tok_ఆధారం_Append_పంక్తి(Py_Tok_ఆధారం_పాఠ్యం *ఆధారం, స్థిరరాశి అక్షరం *బైట్లు,
                        Py_ssize_t len)
{
    తిరిగి append_పంక్తి(ఆధారం, బైట్లు, len, 0);
}

_PyTok_Off
Py_Tok_ఆధారం_Append_పంక్తి_With_Newline(Py_Tok_ఆధారం_పాఠ్యం *ఆధారం, స్థిరరాశి అక్షరం *బైట్లు,
                                   Py_ssize_t len)
{
    తిరిగి append_పంక్తి(ఆధారం, బైట్లు, len, 1);
}

స్థిరరాశి అక్షరం *
Py_Tok_ఆధారం_పంక్తి_View(స్థిరరాశి Py_Tok_ఆధారం_పాఠ్యం *ఆధారం, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    ప్రతిజ్ఞా(len != ఖాళీ);
    స్థిరరాశి అక్షరం *పంక్తి = Py_Tok_ఆధారం_దత్తాంశం(ఆధారం);
    స్థిరరాశి అక్షరం *చివర = పంక్తి + ఆధారం->len;
    అయితే (lineno > 1) {
        స్థిరరాశి అక్షరం *newline = memchr(పంక్తి, '\n', చివర - పంక్తి);
        ఒకవేళ (newline == ఖాళీ) {
            ఆపు;
        }
        పంక్తి = newline + 1;
        lineno--;
    }
    స్థిరరాశి అక్షరం *newline = memchr(పంక్తి, '\n', చివర - పంక్తి);
    *len = (newline != ఖాళీ ? newline : చివర) - పంక్తి;
    తిరిగి పంక్తి;
}
