#ifndef Py_TOKENIZER_SOURCE_H
#define Py_TOKENIZER_SOURCE_H

#अन्तर्भाव "Python.h"

#अन्तर्भाव "types.h"

प्रकारव्याख्या संरचना {
    अक्षर *bytes;
    _PyTok_Off base_offset;
    _PyTok_Off len;
    _PyTok_Off cap;
    पूर्णांक nlines;
} _PyTok_SourceText;

स्थिर ओळीत स्थिरांक अक्षर *
_PyTok_SourceData(स्थिरांक _PyTok_SourceText *source)
{
    परत source->bytes != रिक्त ? source->bytes : "";
}

/* Convert positions within the retained source window. Pointers and views
   are borrowed; append, discard, and clear invalidate them. */
स्थिर ओळीत स्थिरांक अक्षर *
_PyTok_SourcePointer(स्थिरांक _PyTok_SourceText *source, _PyTok_Off offset)
{
    प्रतिज्ञा(offset >= source->base_offset);
    प्रतिज्ञा(offset - source->base_offset <= source->len);
    परत _PyTok_SourceData(source) + (offset - source->base_offset);
}

स्थिर ओळीत अचिन्हित अक्षर
_PyTok_SourceByte(स्थिरांक _PyTok_SourceText *source, _PyTok_Off offset)
{
    प्रतिज्ञा(offset >= source->base_offset);
    प्रतिज्ञा(offset - source->base_offset < source->len);
    परत (अचिन्हित अक्षर)source->bytes[offset - source->base_offset];
}

स्थिर ओळीत स्थिरांक अक्षर *
_PyTok_SourceSpanView(स्थिरांक _PyTok_SourceText *source, _PyTok_Span span,
                      Py_ssize_t *length)
{
    प्रतिज्ञा(length != रिक्त);
    प्रतिज्ञा(_PyTok_SpanIsValid(span));
    प्रतिज्ञा(span.end - source->base_offset <= source->len);
    *length = span.end - span.start;
    परत _PyTok_SourcePointer(source, span.start);
}

/* Return the first matching offset within span, or -1 if absent. */
स्थिर ओळीत _PyTok_Off
_PyTok_SourceFindByte(स्थिरांक _PyTok_SourceText *source, _PyTok_Span span,
                      पूर्णांक byte)
{
    Py_ssize_t length;
    स्थिरांक अक्षर *data = _PyTok_SourceSpanView(source, span, &length);
    स्थिरांक अक्षर *found = स_प्रथम(data, byte, length);
    परत found != रिक्त ? span.start + (found - data) : -1;
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
PyAPI_FUNC(_PyTok_Off) Py_Tok_Source_Append_ओळ(
    _PyTok_SourceText *source, स्थिरांक अक्षर *bytes, Py_ssize_t len);
/* Append a line payload followed by '\n'. The payload may be empty and must
   contain no newline. Other SourceAppendLine constraints still apply. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_Source_Append_ओळ_With_Newline(
    _PyTok_SourceText *source, स्थिरांक अक्षर *bytes, Py_ssize_t len);
/* Return borrowed bytes excluding '\n', writing the byte length to *len.
   Line numbers are 1-based and clamp to the first or final line; a trailing
   '\n' adds an empty final line. The view need not be NUL-terminated.
   This does not set an exception. Append, discard, and clear invalidate the view. */
PyAPI_FUNC(स्थिरांक अक्षर *) Py_Tok_Source_ओळ_View(
    स्थिरांक _PyTok_SourceText *source, Py_ssize_t lineno, Py_ssize_t *len);

#endif
