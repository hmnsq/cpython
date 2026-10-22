#ifndef Py_TOKENIZER_SOURCE_H
#define Py_TOKENIZER_SOURCE_H

#شامل "Python.h"

#شامل "types.h"

تعریفنوع ساختار {
    نویسه *bytes;
    _PyTok_Off base_offset;
    _PyTok_Off len;
    _PyTok_Off cap;
    صحیح nlines;
} _PyTok_SourceText;

ایستا درونخطی ثابت نویسه *
_PyTok_SourceData(ثابت _PyTok_SourceText *source)
{
    بازگشت source->bytes != پوچ ? source->bytes : "";
}

/* Convert positions within the retained source window. Pointers and views
   are borrowed; append, discard, and clear invalidate them. */
ایستا درونخطی ثابت نویسه *
_PyTok_SourcePointer(ثابت _PyTok_SourceText *source, _PyTok_Off offset)
{
    أكد(offset >= source->base_offset);
    أكد(offset - source->base_offset <= source->len);
    بازگشت _PyTok_SourceData(source) + (offset - source->base_offset);
}

ایستا درونخطی unsigned نویسه
_PyTok_SourceByte(ثابت _PyTok_SourceText *source, _PyTok_Off offset)
{
    أكد(offset >= source->base_offset);
    أكد(offset - source->base_offset < source->len);
    بازگشت (unsigned نویسه)source->bytes[offset - source->base_offset];
}

ایستا درونخطی ثابت نویسه *
_PyTok_SourceSpanView(ثابت _PyTok_SourceText *source, _PyTok_Span span,
                      Py_ssize_t *length)
{
    أكد(length != پوچ);
    أكد(_PyTok_SpanIsValid(span));
    أكد(span.end - source->base_offset <= source->len);
    *length = span.end - span.start;
    بازگشت _PyTok_SourcePointer(source, span.start);
}

/* Return the first matching offset within span, or -1 if absent. */
ایستا درونخطی _PyTok_Off
_PyTok_SourceFindByte(ثابت _PyTok_SourceText *source, _PyTok_Span span,
                      صحیح byte)
{
    Py_ssize_t length;
    ثابت نویسه *data = _PyTok_SourceSpanView(source, span, &length);
    ثابت نویسه *found = memchr(data, byte, length);
    بازگشت found != پوچ ? span.start + (found - data) : -1;
}

PyAPI_FUNC(تهی) _PyTok_SourceInit(_PyTok_SourceText *);
/* Clear invalidates all spans and views for the source. */
PyAPI_FUNC(تهی) _PyTok_SourceClear(_PyTok_SourceText *);
/* Discard the retained window and invalidate its spans and views.
   Keep its allocation and advance the logical base to the end of the window. */
PyAPI_FUNC(تهی) _PyTok_SourceDiscard(_PyTok_SourceText *);
/* Append one nonempty logical line and return its start offset. The input may
   contain one newline, as its final byte. An unterminated line must be the
   final line. The input must not point into source storage. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_Source_Append_سطر(
    _PyTok_SourceText *source, ثابت نویسه *bytes, Py_ssize_t len);
/* Append a line payload followed by '\n'. The payload may be empty and must
   contain no newline. Other SourceAppendLine constraints still apply. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_Source_Append_سطر_With_Newline(
    _PyTok_SourceText *source, ثابت نویسه *bytes, Py_ssize_t len);
/* Return borrowed bytes excluding '\n', writing the byte length to *len.
   Line numbers are 1-based and clamp to the first or final line; a trailing
   '\n' adds an empty final line. The view need not be NUL-terminated.
   This does not set an exception. Append, discard, and clear invalidate the view. */
PyAPI_FUNC(ثابت نویسه *) Py_Tok_Source_سطر_View(
    ثابت _PyTok_SourceText *source, Py_ssize_t lineno, Py_ssize_t *len);

#endif
