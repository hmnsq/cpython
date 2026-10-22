#ifndef Py_TOKENIZER_SOURCE_H
#define Py_TOKENIZER_SOURCE_H

#ضمن "Python.h"

#ضمن "types.h"

تعريفنوع بنية {
    حرف *bytes;
    _PyTok_Off base_offset;
    _PyTok_Off len;
    _PyTok_Off cap;
    صحيح nlines;
} _PyTok_SourceText;

ساكن مضمن ثابت حرف *
_PyTok_SourceData(ثابت _PyTok_SourceText *source)
{
    إرجاع source->bytes != عدم ? source->bytes : "";
}

/* Convert positions within the retained source window. Pointers and views
   are borrowed; append, discard, and clear invalidate them. */
ساكن مضمن ثابت حرف *
_PyTok_SourcePointer(ثابت _PyTok_SourceText *source, _PyTok_Off offset)
{
    أكد(offset >= source->base_offset);
    أكد(offset - source->base_offset <= source->len);
    إرجاع _PyTok_SourceData(source) + (offset - source->base_offset);
}

ساكن مضمن unsigned حرف
_PyTok_SourceByte(ثابت _PyTok_SourceText *source, _PyTok_Off offset)
{
    أكد(offset >= source->base_offset);
    أكد(offset - source->base_offset < source->len);
    إرجاع (unsigned حرف)source->bytes[offset - source->base_offset];
}

ساكن مضمن ثابت حرف *
_PyTok_SourceSpanView(ثابت _PyTok_SourceText *source, _PyTok_Span span,
                      Py_ssize_t *length)
{
    أكد(length != عدم);
    أكد(_PyTok_SpanIsValid(span));
    أكد(span.end - source->base_offset <= source->len);
    *length = span.end - span.start;
    إرجاع _PyTok_SourcePointer(source, span.start);
}

/* Return the first matching offset within span, or -1 if absent. */
ساكن مضمن _PyTok_Off
_PyTok_SourceFindByte(ثابت _PyTok_SourceText *source, _PyTok_Span span,
                      صحيح byte)
{
    Py_ssize_t length;
    ثابت حرف *data = _PyTok_SourceSpanView(source, span, &length);
    ثابت حرف *found = memchr(data, byte, length);
    إرجاع found != عدم ? span.start + (found - data) : -1;
}

PyAPI_FUNC(فراغ) _PyTok_SourceInit(_PyTok_SourceText *);
/* Clear invalidates all spans and views for the source. */
PyAPI_FUNC(فراغ) _PyTok_SourceClear(_PyTok_SourceText *);
/* Discard the retained window and invalidate its spans and views.
   Keep its allocation and advance the logical base to the end of the window. */
PyAPI_FUNC(فراغ) _PyTok_SourceDiscard(_PyTok_SourceText *);
/* Append one nonempty logical line and return its start offset. The input may
   contain one newline, as its final byte. An unterminated line must be the
   final line. The input must not point into source storage. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_Source_Append_سطر(
    _PyTok_SourceText *source, ثابت حرف *bytes, Py_ssize_t len);
/* Append a line payload followed by '\n'. The payload may be empty and must
   contain no newline. Other SourceAppendLine constraints still apply. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_Source_Append_سطر_With_Newline(
    _PyTok_SourceText *source, ثابت حرف *bytes, Py_ssize_t len);
/* Return borrowed bytes excluding '\n', writing the byte length to *len.
   Line numbers are 1-based and clamp to the first or final line; a trailing
   '\n' adds an empty final line. The view need not be NUL-terminated.
   This does not set an exception. Append, discard, and clear invalidate the view. */
PyAPI_FUNC(ثابت حرف *) Py_Tok_Source_سطر_View(
    ثابت _PyTok_SourceText *source, Py_ssize_t lineno, Py_ssize_t *len);

#endif
