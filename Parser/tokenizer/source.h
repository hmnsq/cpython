#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ Py_TOKENIZER_ਸਰੋਤ_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Py_TOKENIZER_ਸਰੋਤ_H

#ਅਨ੍ਤਰ੍ਭਾਵ "Python.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "types.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਅੱਖਰ *ਬਾਈਟਾਂ;
    _PyTok_Off ਅਧਾਰ_ਵਿੱਥ;
    _PyTok_Off len;
    _PyTok_Off cap;
    ਪੂਰਨ nlines;
} Py_Tok_ਸਰੋਤ_ਪਾਠ;

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ *
Py_Tok_ਸਰੋਤ_ਅੰਕੜੇ(ਅਚਲ Py_Tok_ਸਰੋਤ_ਪਾਠ *ਸਰੋਤ)
{
    ਵਾਪਸ ਸਰੋਤ->ਬਾਈਟਾਂ != ਨਿਰਾ ? ਸਰੋਤ->ਬਾਈਟਾਂ : "";
}

/* Convert positions within the retained source window. Pointers and views
   are borrowed; append, discard, and clear invalidate them. */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ *
Py_Tok_ਸਰੋਤ_ਸੂਚਕ(ਅਚਲ Py_Tok_ਸਰੋਤ_ਪਾਠ *ਸਰੋਤ, _PyTok_Off ਵਿੱਥ)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿੱਥ >= ਸਰੋਤ->ਅਧਾਰ_ਵਿੱਥ);
    ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿੱਥ - ਸਰੋਤ->ਅਧਾਰ_ਵਿੱਥ <= ਸਰੋਤ->len);
    ਵਾਪਸ Py_Tok_ਸਰੋਤ_ਅੰਕੜੇ(ਸਰੋਤ) + (ਵਿੱਥ - ਸਰੋਤ->ਅਧਾਰ_ਵਿੱਥ);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ
Py_Tok_ਸਰੋਤ_Byte(ਅਚਲ Py_Tok_ਸਰੋਤ_ਪਾਠ *ਸਰੋਤ, _PyTok_Off ਵਿੱਥ)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿੱਥ >= ਸਰੋਤ->ਅਧਾਰ_ਵਿੱਥ);
    ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿੱਥ - ਸਰੋਤ->ਅਧਾਰ_ਵਿੱਥ < ਸਰੋਤ->len);
    ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)ਸਰੋਤ->ਬਾਈਟਾਂ[ਵਿੱਥ - ਸਰੋਤ->ਅਧਾਰ_ਵਿੱਥ];
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ *
Py_Tok_ਸਰੋਤ_ਵਿਸਤਾਰ_View(ਅਚਲ Py_Tok_ਸਰੋਤ_ਪਾਠ *ਸਰੋਤ, Py_Tok_ਵਿਸਤਾਰ ਵਿਸਤਾਰ,
                      Py_ssize_t *ਲੰਮਾਈ)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(ਲੰਮਾਈ != ਨਿਰਾ);
    ਪ੍ਰਤਿਜ੍ਞਾ(Py_Tok_ਵਿਸਤਾਰ_Is_Valid(ਵਿਸਤਾਰ));
    ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿਸਤਾਰ.ਸਿਰਾ - ਸਰੋਤ->ਅਧਾਰ_ਵਿੱਥ <= ਸਰੋਤ->len);
    *ਲੰਮਾਈ = ਵਿਸਤਾਰ.ਸਿਰਾ - ਵਿਸਤਾਰ.ਸ਼ੁਰੂ;
    ਵਾਪਸ Py_Tok_ਸਰੋਤ_ਸੂਚਕ(ਸਰੋਤ, ਵਿਸਤਾਰ.ਸ਼ੁਰੂ);
}

/* Return the first matching offset within span, or -1 if absent. */
ਸਥਿਰ ਸਤਰਅੰਦਰ _PyTok_Off
Py_Tok_ਸਰੋਤ_ਲੱਭੋ_Byte(ਅਚਲ Py_Tok_ਸਰੋਤ_ਪਾਠ *ਸਰੋਤ, Py_Tok_ਵਿਸਤਾਰ ਵਿਸਤਾਰ,
                      ਪੂਰਨ byte)
{
    Py_ssize_t ਲੰਮਾਈ;
    ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ = Py_Tok_ਸਰੋਤ_ਵਿਸਤਾਰ_View(ਸਰੋਤ, ਵਿਸਤਾਰ, &ਲੰਮਾਈ);
    ਅਚਲ ਅੱਖਰ *ਮਿਲਿਆ = memchr(ਅੰਕੜੇ, byte, ਲੰਮਾਈ);
    ਵਾਪਸ ਮਿਲਿਆ != ਨਿਰਾ ? ਵਿਸਤਾਰ.ਸ਼ੁਰੂ + (ਮਿਲਿਆ - ਅੰਕੜੇ) : -1;
}

PyAPI_FUNC(ਖਾਲੀ) Py_Tok_ਸਰੋਤ_Init(Py_Tok_ਸਰੋਤ_ਪਾਠ *);
/* Clear invalidates all spans and views for the source. */
PyAPI_FUNC(ਖਾਲੀ) Py_Tok_ਸਰੋਤ_Clear(Py_Tok_ਸਰੋਤ_ਪਾਠ *);
/* Discard the retained window and invalidate its spans and views.
   Keep its allocation and advance the logical base to the end of the window. */
PyAPI_FUNC(ਖਾਲੀ) Py_Tok_ਸਰੋਤ_Discard(Py_Tok_ਸਰੋਤ_ਪਾਠ *);
/* Append one nonempty logical line and return its start offset. The input may
   contain one newline, as its final byte. An unterminated line must be the
   final line. The input must not point into source storage. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_ਸਰੋਤ_Append_ਸਤਰ(
    Py_Tok_ਸਰੋਤ_ਪਾਠ *ਸਰੋਤ, ਅਚਲ ਅੱਖਰ *ਬਾਈਟਾਂ, Py_ssize_t len);
/* Append a line payload followed by '\n'. The payload may be empty and must
   contain no newline. Other SourceAppendLine constraints still apply. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_ਸਰੋਤ_Append_ਸਤਰ_With_Newline(
    Py_Tok_ਸਰੋਤ_ਪਾਠ *ਸਰੋਤ, ਅਚਲ ਅੱਖਰ *ਬਾਈਟਾਂ, Py_ssize_t len);
/* Return borrowed bytes excluding '\n', writing the byte length to *len.
   Line numbers are 1-based and clamp to the first or final line; a trailing
   '\n' adds an empty final line. The view need not be NUL-terminated.
   This does not set an exception. Append, discard, and clear invalidate the view. */
PyAPI_FUNC(ਅਚਲ ਅੱਖਰ *) Py_Tok_ਸਰੋਤ_ਸਤਰ_View(
    ਅਚਲ Py_Tok_ਸਰੋਤ_ਪਾਠ *ਸਰੋਤ, Py_ssize_t lineno, Py_ssize_t *len);

#ਜੇ_ਅੰਤ
