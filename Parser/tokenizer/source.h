#ifndef Py_TOKENIZER_SOURCE_H
#define Py_TOKENIZER_SOURCE_H

#ಅನ್ತರ್ಭಾವ "Python.h"

#ಅನ್ತರ್ಭಾವ "types.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ವರ್ಣ *bytes;
    _PyTok_Off base_offset;
    _PyTok_Off len;
    _PyTok_Off cap;
    ಪೂರ್ಣಾಙ್ಕ nlines;
} _PyTok_SourceText;

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ *
_PyTok_SourceData(ಧ್ರುವ _PyTok_SourceText *source)
{
    ಪ್ರತ್ಯಾಗಮ source->bytes != ರಿಕ್ತ ? source->bytes : "";
}

/* Convert positions within the retained source window. Pointers and views
   are borrowed; append, discard, and clear invalidate them. */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ *
_PyTok_SourcePointer(ಧ್ರುವ _PyTok_SourceText *source, _PyTok_Off offset)
{
    ಪ್ರತಿಜ್ಞಾ(offset >= source->base_offset);
    ಪ್ರತಿಜ್ಞಾ(offset - source->base_offset <= source->len);
    ಪ್ರತ್ಯಾಗಮ _PyTok_SourceData(source) + (offset - source->base_offset);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ unsigned ವರ್ಣ
_PyTok_SourceByte(ಧ್ರುವ _PyTok_SourceText *source, _PyTok_Off offset)
{
    ಪ್ರತಿಜ್ಞಾ(offset >= source->base_offset);
    ಪ್ರತಿಜ್ಞಾ(offset - source->base_offset < source->len);
    ಪ್ರತ್ಯಾಗಮ (unsigned ವರ್ಣ)source->bytes[offset - source->base_offset];
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ *
_PyTok_SourceSpanView(ಧ್ರುವ _PyTok_SourceText *source, _PyTok_Span span,
                      Py_ssize_t *length)
{
    ಪ್ರತಿಜ್ಞಾ(length != ರಿಕ್ತ);
    ಪ್ರತಿಜ್ಞಾ(_PyTok_SpanIsValid(span));
    ಪ್ರತಿಜ್ಞಾ(span.end - source->base_offset <= source->len);
    *length = span.end - span.start;
    ಪ್ರತ್ಯಾಗಮ _PyTok_SourcePointer(source, span.start);
}

/* Return the first matching offset within span, or -1 if absent. */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ _PyTok_Off
_PyTok_SourceFindByte(ಧ್ರುವ _PyTok_SourceText *source, _PyTok_Span span,
                      ಪೂರ್ಣಾಙ್ಕ byte)
{
    Py_ssize_t length;
    ಧ್ರುವ ವರ್ಣ *data = _PyTok_SourceSpanView(source, span, &length);
    ಧ್ರುವ ವರ್ಣ *found = memchr(data, byte, length);
    ಪ್ರತ್ಯಾಗಮ found != ರಿಕ್ತ ? span.start + (found - data) : -1;
}

PyAPI_FUNC(ಶೂನ್ಯ) _PyTok_SourceInit(_PyTok_SourceText *);
/* Clear invalidates all spans and views for the source. */
PyAPI_FUNC(ಶೂನ್ಯ) _PyTok_SourceClear(_PyTok_SourceText *);
/* Discard the retained window and invalidate its spans and views.
   Keep its allocation and advance the logical base to the end of the window. */
PyAPI_FUNC(ಶೂನ್ಯ) _PyTok_SourceDiscard(_PyTok_SourceText *);
/* Append one nonempty logical line and return its start offset. The input may
   contain one newline, as its final byte. An unterminated line must be the
   final line. The input must not point into source storage. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_Source_Append_ಸಾಲು(
    _PyTok_SourceText *source, ಧ್ರುವ ವರ್ಣ *bytes, Py_ssize_t len);
/* Append a line payload followed by '\n'. The payload may be empty and must
   contain no newline. Other SourceAppendLine constraints still apply. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_Source_Append_ಸಾಲು_With_Newline(
    _PyTok_SourceText *source, ಧ್ರುವ ವರ್ಣ *bytes, Py_ssize_t len);
/* Return borrowed bytes excluding '\n', writing the byte length to *len.
   Line numbers are 1-based and clamp to the first or final line; a trailing
   '\n' adds an empty final line. The view need not be NUL-terminated.
   This does not set an exception. Append, discard, and clear invalidate the view. */
PyAPI_FUNC(ಧ್ರುವ ವರ್ಣ *) Py_Tok_Source_ಸಾಲು_View(
    ಧ್ರುವ _PyTok_SourceText *source, Py_ssize_t lineno, Py_ssize_t *len);

#endif
