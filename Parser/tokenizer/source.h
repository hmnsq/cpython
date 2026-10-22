#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ Py_TOKENIZER_ಮೂಲ_H
#ವ್ಯಾಖ್ಯಾನಿಸು Py_TOKENIZER_ಮೂಲ_H

#ಅನ್ತರ್ಭಾವ "Python.h"

#ಅನ್ತರ್ಭಾವ "types.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಅಕ್ಷರ *ಬೈಟುಗಳು;
    _PyTok_Off ತಳ_ಸ್ಥಾನಾಂತರ;
    _PyTok_Off len;
    _PyTok_Off cap;
    ಪೂರ್ಣಾಂಕ nlines;
} Py_Tok_ಮೂಲ_ಪಠ್ಯ;

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *
Py_Tok_ಮೂಲ_ದತ್ತಾಂಶ(ಸ್ಥಿರಾಂಕ Py_Tok_ಮೂಲ_ಪಠ್ಯ *ಮೂಲ)
{
    ಹಿಂತಿರುಗಿ ಮೂಲ->ಬೈಟುಗಳು != ಖಾಲಿ ? ಮೂಲ->ಬೈಟುಗಳು : "";
}

/* Convert positions within the retained source window. Pointers and views
   are borrowed; append, discard, and clear invalidate them. */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *
Py_Tok_ಮೂಲ_ಸೂಚಕ(ಸ್ಥಿರಾಂಕ Py_Tok_ಮೂಲ_ಪಠ್ಯ *ಮೂಲ, _PyTok_Off ಸ್ಥಾನಾಂತರ)
{
    ಪ್ರತಿಜ್ಞಾ(ಸ್ಥಾನಾಂತರ >= ಮೂಲ->ತಳ_ಸ್ಥಾನಾಂತರ);
    ಪ್ರತಿಜ್ಞಾ(ಸ್ಥಾನಾಂತರ - ಮೂಲ->ತಳ_ಸ್ಥಾನಾಂತರ <= ಮೂಲ->len);
    ಹಿಂತಿರುಗಿ Py_Tok_ಮೂಲ_ದತ್ತಾಂಶ(ಮೂಲ) + (ಸ್ಥಾನಾಂತರ - ಮೂಲ->ತಳ_ಸ್ಥಾನಾಂತರ);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ
Py_Tok_ಮೂಲ_Byte(ಸ್ಥಿರಾಂಕ Py_Tok_ಮೂಲ_ಪಠ್ಯ *ಮೂಲ, _PyTok_Off ಸ್ಥಾನಾಂತರ)
{
    ಪ್ರತಿಜ್ಞಾ(ಸ್ಥಾನಾಂತರ >= ಮೂಲ->ತಳ_ಸ್ಥಾನಾಂತರ);
    ಪ್ರತಿಜ್ಞಾ(ಸ್ಥಾನಾಂತರ - ಮೂಲ->ತಳ_ಸ್ಥಾನಾಂತರ < ಮೂಲ->len);
    ಹಿಂತಿರುಗಿ (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)ಮೂಲ->ಬೈಟುಗಳು[ಸ್ಥಾನಾಂತರ - ಮೂಲ->ತಳ_ಸ್ಥಾನಾಂತರ];
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *
Py_Tok_ಮೂಲ_ಹರವು_View(ಸ್ಥಿರಾಂಕ Py_Tok_ಮೂಲ_ಪಠ್ಯ *ಮೂಲ, Py_Tok_ಹರವು ಹರವು,
                      Py_ssize_t *ಉದ್ದಳತೆ)
{
    ಪ್ರತಿಜ್ಞಾ(ಉದ್ದಳತೆ != ಖಾಲಿ);
    ಪ್ರತಿಜ್ಞಾ(Py_Tok_ಹರವು_Is_Valid(ಹರವು));
    ಪ್ರತಿಜ್ಞಾ(ಹರವು.ಕೊನೆ - ಮೂಲ->ತಳ_ಸ್ಥಾನಾಂತರ <= ಮೂಲ->len);
    *ಉದ್ದಳತೆ = ಹರವು.ಕೊನೆ - ಹರವು.ಆರಂಭ;
    ಹಿಂತಿರುಗಿ Py_Tok_ಮೂಲ_ಸೂಚಕ(ಮೂಲ, ಹರವು.ಆರಂಭ);
}

/* Return the first matching offset within span, or -1 if absent. */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ _PyTok_Off
Py_Tok_ಮೂಲ_ಕಂಡುಹಿಡಿ_Byte(ಸ್ಥಿರಾಂಕ Py_Tok_ಮೂಲ_ಪಠ್ಯ *ಮೂಲ, Py_Tok_ಹರವು ಹರವು,
                      ಪೂರ್ಣಾಂಕ byte)
{
    Py_ssize_t ಉದ್ದಳತೆ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ = Py_Tok_ಮೂಲ_ಹರವು_View(ಮೂಲ, ಹರವು, &ಉದ್ದಳತೆ);
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಿಕ್ಕಿತು = memchr(ದತ್ತಾಂಶ, byte, ಉದ್ದಳತೆ);
    ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು != ಖಾಲಿ ? ಹರವು.ಆರಂಭ + (ಸಿಕ್ಕಿತು - ದತ್ತಾಂಶ) : -1;
}

PyAPI_FUNC(ಶೂನ್ಯ) Py_Tok_ಮೂಲ_Init(Py_Tok_ಮೂಲ_ಪಠ್ಯ *);
/* Clear invalidates all spans and views for the source. */
PyAPI_FUNC(ಶೂನ್ಯ) Py_Tok_ಮೂಲ_Clear(Py_Tok_ಮೂಲ_ಪಠ್ಯ *);
/* Discard the retained window and invalidate its spans and views.
   Keep its allocation and advance the logical base to the end of the window. */
PyAPI_FUNC(ಶೂನ್ಯ) Py_Tok_ಮೂಲ_Discard(Py_Tok_ಮೂಲ_ಪಠ್ಯ *);
/* Append one nonempty logical line and return its start offset. The input may
   contain one newline, as its final byte. An unterminated line must be the
   final line. The input must not point into source storage. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_ಮೂಲ_Append_ಸಾಲು(
    Py_Tok_ಮೂಲ_ಪಠ್ಯ *ಮೂಲ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೈಟುಗಳು, Py_ssize_t len);
/* Append a line payload followed by '\n'. The payload may be empty and must
   contain no newline. Other SourceAppendLine constraints still apply. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_ಮೂಲ_Append_ಸಾಲು_With_Newline(
    Py_Tok_ಮೂಲ_ಪಠ್ಯ *ಮೂಲ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೈಟುಗಳು, Py_ssize_t len);
/* Return borrowed bytes excluding '\n', writing the byte length to *len.
   Line numbers are 1-based and clamp to the first or final line; a trailing
   '\n' adds an empty final line. The view need not be NUL-terminated.
   This does not set an exception. Append, discard, and clear invalidate the view. */
PyAPI_FUNC(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *) Py_Tok_ಮೂಲ_ಸಾಲು_View(
    ಸ್ಥಿರಾಂಕ Py_Tok_ಮೂಲ_ಪಠ್ಯ *ಮೂಲ, Py_ssize_t lineno, Py_ssize_t *len);

#ಆದರೆ_ಅಂತ್ಯ
