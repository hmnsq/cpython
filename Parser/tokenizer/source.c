#ಅನ್ತರ್ಭಾವ "Python.h"
#ಅನ್ತರ್ಭಾವ "pycore_pymem.h"  // PYMEM_DEADBYTE

#ಅನ್ತರ್ಭಾವ "source.h"

#ವ್ಯಾಖ್ಯಾನಿಸು ಮೂಲ_ಬಫರ್_GROWTH_ಗುಣಕ 2

ಶೂನ್ಯ
Py_Tok_ಮೂಲ_Init(Py_Tok_ಮೂಲ_ಪಠ್ಯ *ಮೂಲ)
{
    *ಮೂಲ = (Py_Tok_ಮೂಲ_ಪಠ್ಯ){0};
}

ಶೂನ್ಯ
Py_Tok_ಮೂಲ_Clear(Py_Tok_ಮೂಲ_ಪಠ್ಯ *ಮೂಲ)
{
    PyMem_Free(ಮೂಲ->ಬೈಟುಗಳು);
    Py_Tok_ಮೂಲ_Init(ಮೂಲ);
}

ಶೂನ್ಯ
Py_Tok_ಮೂಲ_Discard(Py_Tok_ಮೂಲ_ಪಠ್ಯ *ಮೂಲ)
{
    ಪ್ರತಿಜ್ಞಾ(ಮೂಲ->ತಳ_ಸ್ಥಾನಾಂತರ <= PY_SSIZE_T_MAX - ಮೂಲ->len);
    ಮೂಲ->ತಳ_ಸ್ಥಾನಾಂತರ += ಮೂಲ->len;
    ಮೂಲ->len = 0;
    ಆದರೆ (ಮೂಲ->ಬೈಟುಗಳು != ಖಾಲಿ) {
        ಮೂಲ->ಬೈಟುಗಳು[0] = '\0';
    }
    ಮೂಲ->nlines = 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
reserve_ಬೈಟುಗಳು(Py_Tok_ಮೂಲ_ಪಠ್ಯ *ಮೂಲ, Py_ssize_t needed)
{
    ಆದರೆ (needed <= ಮೂಲ->cap) {
        ಹಿಂತಿರುಗಿ 0;
    }
    Py_ssize_t cap = ಮೂಲ->cap > 0 ? ಮೂಲ->cap : BUFSIZ;
    ಆಗ (cap < needed) {
        ಆದರೆ (cap > PY_SSIZE_T_MAX / ಮೂಲ_ಬಫರ್_GROWTH_ಗುಣಕ) {
            cap = needed;
            ನಿಲ್ಲಿಸು;
        }
        cap *= ಮೂಲ_ಬಫರ್_GROWTH_ಗುಣಕ;
    }
    ಅಕ್ಷರ *ಬೈಟುಗಳು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ Py_DEBUG
    /* Moving on growth makes stale interior pointers fail in debug builds. */
    ಬೈಟುಗಳು = PyMem_Malloc(cap);
    ಆದರೆ (ಬೈಟುಗಳು != ಖಾಲಿ && ಮೂಲ->len > 0) {
        ಸ್ಮರಣೆ_ನಕಲು(ಬೈಟುಗಳು, ಮೂಲ->ಬೈಟುಗಳು, ಮೂಲ->len);
    }
#ಇಲ್ಲದಿದ್ದರೆ
    ಬೈಟುಗಳು = PyMem_Realloc(ಮೂಲ->ಬೈಟುಗಳು, cap);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (ಬೈಟುಗಳು == ಖಾಲಿ) {
        Py_Err_No_ಸ್ಮರಣೆ();
        ಹಿಂತಿರುಗಿ -1;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ Py_DEBUG
    ಆದರೆ (ಮೂಲ->ಬೈಟುಗಳು != ಖಾಲಿ) {
        memset(ಮೂಲ->ಬೈಟುಗಳು, PYMEM_DEADBYTE, ಮೂಲ->cap);
        PyMem_Free(ಮೂಲ->ಬೈಟುಗಳು);
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಮೂಲ->ಬೈಟುಗಳು = ಬೈಟುಗಳು;
    ಮೂಲ->cap = cap;
    ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
validate_ಸಾಲು(ಸ್ಥಿರಾಂಕ Py_Tok_ಮೂಲ_ಪಠ್ಯ *ಮೂಲ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೈಟುಗಳು,
              Py_ssize_t len, ಪೂರ್ಣಾಂಕ append_newline)
{
    ಆದರೆ (len < 0 || (len == 0 && !append_newline) || ಬೈಟುಗಳು == ಖಾಲಿ ||
            (ಮೂಲ->nlines > 0 &&
             (ಮೂಲ->len == 0 || ಮೂಲ->ಬೈಟುಗಳು[ಮೂಲ->len - 1] != '\n'))) {
        Py_Err_ಗಣ_String(PyExc_SystemError, "invalid tokenizer source line");
        ಹಿಂತಿರುಗಿ -1;
    }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *newline = memchr(ಬೈಟುಗಳು, '\n', len);
    ಆದರೆ (newline != ಖಾಲಿ && (append_newline || newline != ಬೈಟುಗಳು + len - 1)) {
        Py_Err_ಗಣ_String(PyExc_SystemError, "invalid tokenizer source line");
        ಹಿಂತಿರುಗಿ -1;
    }
    ಆದರೆ (ಮೂಲ->nlines == ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ ||
            (ಮೂಲ->nlines == ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ - 1 && (newline != ಖಾಲಿ || append_newline))) {
        Py_Err_ಗಣ_String(Py_Exc_ಅತಿಪ್ರವಾಹ_Error, "too many tokenizer source lines");
        ಹಿಂತಿರುಗಿ -1;
    }
    ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ _PyTok_Off
append_ಸಾಲು(Py_Tok_ಮೂಲ_ಪಠ್ಯ *ಮೂಲ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೈಟುಗಳು,
            Py_ssize_t len, ಪೂರ್ಣಾಂಕ append_newline)
{
    ಆದರೆ (validate_ಸಾಲು(ಮೂಲ, ಬೈಟುಗಳು, len, append_newline) < 0) {
        ಹಿಂತಿರುಗಿ -1;
    }
    ಆದರೆ (ಮೂಲ->len > PY_SSIZE_T_MAX - len - append_newline - 1 ||
            ಮೂಲ->ತಳ_ಸ್ಥಾನಾಂತರ > PY_SSIZE_T_MAX - ಮೂಲ->len - len - append_newline) {
        Py_Err_No_ಸ್ಮರಣೆ();
        ಹಿಂತಿರುಗಿ -1;
    }
    ಆದರೆ (reserve_ಬೈಟುಗಳು(ಮೂಲ, ಮೂಲ->len + len + append_newline + 1) < 0) {
        ಹಿಂತಿರುಗಿ -1;
    }

    _PyTok_Off ಆರಂಭ = ಮೂಲ->len;
    ಸ್ಮರಣೆ_ನಕಲು(ಮೂಲ->ಬೈಟುಗಳು + ಆರಂಭ, ಬೈಟುಗಳು, len);
    ಮೂಲ->len += len;
    ಆದರೆ (append_newline) {
        ಮೂಲ->ಬೈಟುಗಳು[ಮೂಲ->len++] = '\n';
    }
    ಮೂಲ->ಬೈಟುಗಳು[ಮೂಲ->len] = '\0';
    ಮೂಲ->nlines++;
    ಹಿಂತಿರುಗಿ ಮೂಲ->ತಳ_ಸ್ಥಾನಾಂತರ + ಆರಂಭ;
}

_PyTok_Off
Py_Tok_ಮೂಲ_Append_ಸಾಲು(Py_Tok_ಮೂಲ_ಪಠ್ಯ *ಮೂಲ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೈಟುಗಳು,
                        Py_ssize_t len)
{
    ಹಿಂತಿರುಗಿ append_ಸಾಲು(ಮೂಲ, ಬೈಟುಗಳು, len, 0);
}

_PyTok_Off
Py_Tok_ಮೂಲ_Append_ಸಾಲು_With_Newline(Py_Tok_ಮೂಲ_ಪಠ್ಯ *ಮೂಲ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೈಟುಗಳು,
                                   Py_ssize_t len)
{
    ಹಿಂತಿರುಗಿ append_ಸಾಲು(ಮೂಲ, ಬೈಟುಗಳು, len, 1);
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *
Py_Tok_ಮೂಲ_ಸಾಲು_View(ಸ್ಥಿರಾಂಕ Py_Tok_ಮೂಲ_ಪಠ್ಯ *ಮೂಲ, Py_ssize_t lineno,
                      Py_ssize_t *len)
{
    ಪ್ರತಿಜ್ಞಾ(len != ಖಾಲಿ);
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು = Py_Tok_ಮೂಲ_ದತ್ತಾಂಶ(ಮೂಲ);
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆ = ಸಾಲು + ಮೂಲ->len;
    ಆಗ (lineno > 1) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *newline = memchr(ಸಾಲು, '\n', ಕೊನೆ - ಸಾಲು);
        ಆದರೆ (newline == ಖಾಲಿ) {
            ನಿಲ್ಲಿಸು;
        }
        ಸಾಲು = newline + 1;
        lineno--;
    }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *newline = memchr(ಸಾಲು, '\n', ಕೊನೆ - ಸಾಲು);
    *len = (newline != ಖಾಲಿ ? newline : ಕೊನೆ) - ಸಾಲು;
    ಹಿಂತಿರುಗಿ ಸಾಲು;
}
