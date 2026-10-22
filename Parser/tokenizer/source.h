#ifndef Py_TOKENIZER_SOURCE_H
#define Py_TOKENIZER_SOURCE_H

#अन्तर्भाव "Python.h"

#अन्तर्भाव "types.h"

प्रकारनिर्देश संरचना {
    वर्ण *bytes;
    _PyTok_Off base_offset;
    _PyTok_Off len;
    _PyTok_Off cap;
    पूर्णाङ्क nlines;
} _PyTok_SourceText;

स्थिर अन्तःस्थ ध्रुव वर्ण *
_PyTok_SourceData(ध्रुव _PyTok_SourceText *source)
{
    प्रत्यागम source->bytes != रिक्त ? source->bytes : "";
}

/* Convert positions within the retained source window. Pointers and views
   are borrowed; append, discard, and clear invalidate them. */
स्थिर अन्तःस्थ ध्रुव वर्ण *
_PyTok_SourcePointer(ध्रुव _PyTok_SourceText *source, _PyTok_Off offset)
{
    प्रतिज्ञा(offset >= source->base_offset);
    प्रतिज्ञा(offset - source->base_offset <= source->len);
    प्रत्यागम _PyTok_SourceData(source) + (offset - source->base_offset);
}

स्थिर अन्तःस्थ unsigned वर्ण
_PyTok_SourceByte(ध्रुव _PyTok_SourceText *source, _PyTok_Off offset)
{
    प्रतिज्ञा(offset >= source->base_offset);
    प्रतिज्ञा(offset - source->base_offset < source->len);
    प्रत्यागम (unsigned वर्ण)source->bytes[offset - source->base_offset];
}

स्थिर अन्तःस्थ ध्रुव वर्ण *
_PyTok_SourceSpanView(ध्रुव _PyTok_SourceText *source, _PyTok_Span span,
                      Py_ssize_t *length)
{
    प्रतिज्ञा(length != रिक्त);
    प्रतिज्ञा(_PyTok_SpanIsValid(span));
    प्रतिज्ञा(span.end - source->base_offset <= source->len);
    *length = span.end - span.start;
    प्रत्यागम _PyTok_SourcePointer(source, span.start);
}

/* Return the first matching offset within span, or -1 if absent. */
स्थिर अन्तःस्थ _PyTok_Off
_PyTok_SourceFindByte(ध्रुव _PyTok_SourceText *source, _PyTok_Span span,
                      पूर्णाङ्क byte)
{
    Py_ssize_t length;
    ध्रुव वर्ण *data = _PyTok_SourceSpanView(source, span, &length);
    ध्रुव वर्ण *found = memchr(data, byte, length);
    प्रत्यागम found != रिक्त ? span.start + (found - data) : -1;
}

PyAPI_FUNC(शून्य) _PyTok_SourceInit(_PyTok_SourceText *);
/* Clear invalidates all spans and views for the source. */
PyAPI_FUNC(शून्य) _PyTok_SourceClear(_PyTok_SourceText *);
/* Discard the retained window and invalidate its spans and views.
   Keep its allocation and advance the logical base to the end of the window. */
PyAPI_FUNC(शून्य) _PyTok_SourceDiscard(_PyTok_SourceText *);
/* Append one nonempty logical line and return its start offset. The input may
   contain one newline, as its final byte. An unterminated line must be the
   final line. The input must not point into source storage. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_Source_Append_पंक्ति(
    _PyTok_SourceText *source, ध्रुव वर्ण *bytes, Py_ssize_t len);
/* Append a line payload followed by '\n'. The payload may be empty and must
   contain no newline. Other SourceAppendLine constraints still apply. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_Source_Append_पंक्ति_With_Newline(
    _PyTok_SourceText *source, ध्रुव वर्ण *bytes, Py_ssize_t len);
/* Return borrowed bytes excluding '\n', writing the byte length to *len.
   Line numbers are 1-based and clamp to the first or final line; a trailing
   '\n' adds an empty final line. The view need not be NUL-terminated.
   This does not set an exception. Append, discard, and clear invalidate the view. */
PyAPI_FUNC(ध्रुव वर्ण *) Py_Tok_Source_पंक्ति_View(
    ध्रुव _PyTok_SourceText *source, Py_ssize_t lineno, Py_ssize_t *len);

#endif
