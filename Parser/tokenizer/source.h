#ifndef Py_TOKENIZER_SOURCE_H
#define Py_TOKENIZER_SOURCE_H

#அந்தர்பாவ "Python.h"

#அந்தர்பாவ "types.h"

வகைவரை கட்டமைப்பு {
    எழுத்து *bytes;
    _PyTok_Off base_offset;
    _PyTok_Off len;
    _PyTok_Off cap;
    முழு nlines;
} _PyTok_SourceText;

நிலையான உள்வரி மாறிலி எழுத்து *
_PyTok_SourceData(மாறிலி _PyTok_SourceText *source)
{
    திரும்பு source->bytes != வெற்று ? source->bytes : "";
}

/* Convert positions within the retained source window. Pointers and views
   are borrowed; append, discard, and clear invalidate them. */
நிலையான உள்வரி மாறிலி எழுத்து *
_PyTok_SourcePointer(மாறிலி _PyTok_SourceText *source, _PyTok_Off offset)
{
    ப்ரதிஜ்ஞா(offset >= source->base_offset);
    ப்ரதிஜ்ஞா(offset - source->base_offset <= source->len);
    திரும்பு _PyTok_SourceData(source) + (offset - source->base_offset);
}

நிலையான உள்வரி unsigned எழுத்து
_PyTok_SourceByte(மாறிலி _PyTok_SourceText *source, _PyTok_Off offset)
{
    ப்ரதிஜ்ஞா(offset >= source->base_offset);
    ப்ரதிஜ்ஞா(offset - source->base_offset < source->len);
    திரும்பு (unsigned எழுத்து)source->bytes[offset - source->base_offset];
}

நிலையான உள்வரி மாறிலி எழுத்து *
_PyTok_SourceSpanView(மாறிலி _PyTok_SourceText *source, _PyTok_Span span,
                      Py_ssize_t *length)
{
    ப்ரதிஜ்ஞா(length != வெற்று);
    ப்ரதிஜ்ஞா(_PyTok_SpanIsValid(span));
    ப்ரதிஜ்ஞா(span.end - source->base_offset <= source->len);
    *length = span.end - span.start;
    திரும்பு _PyTok_SourcePointer(source, span.start);
}

/* Return the first matching offset within span, or -1 if absent. */
நிலையான உள்வரி _PyTok_Off
_PyTok_SourceFindByte(மாறிலி _PyTok_SourceText *source, _PyTok_Span span,
                      முழு byte)
{
    Py_ssize_t length;
    மாறிலி எழுத்து *data = _PyTok_SourceSpanView(source, span, &length);
    மாறிலி எழுத்து *found = memchr(data, byte, length);
    திரும்பு found != வெற்று ? span.start + (found - data) : -1;
}

PyAPI_FUNC(வெறுமை) _PyTok_SourceInit(_PyTok_SourceText *);
/* Clear invalidates all spans and views for the source. */
PyAPI_FUNC(வெறுமை) _PyTok_SourceClear(_PyTok_SourceText *);
/* Discard the retained window and invalidate its spans and views.
   Keep its allocation and advance the logical base to the end of the window. */
PyAPI_FUNC(வெறுமை) _PyTok_SourceDiscard(_PyTok_SourceText *);
/* Append one nonempty logical line and return its start offset. The input may
   contain one newline, as its final byte. An unterminated line must be the
   final line. The input must not point into source storage. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_Source_Append_வரி(
    _PyTok_SourceText *source, மாறிலி எழுத்து *bytes, Py_ssize_t len);
/* Append a line payload followed by '\n'. The payload may be empty and must
   contain no newline. Other SourceAppendLine constraints still apply. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_Source_Append_வரி_With_Newline(
    _PyTok_SourceText *source, மாறிலி எழுத்து *bytes, Py_ssize_t len);
/* Return borrowed bytes excluding '\n', writing the byte length to *len.
   Line numbers are 1-based and clamp to the first or final line; a trailing
   '\n' adds an empty final line. The view need not be NUL-terminated.
   This does not set an exception. Append, discard, and clear invalidate the view. */
PyAPI_FUNC(மாறிலி எழுத்து *) Py_Tok_Source_வரி_View(
    மாறிலி _PyTok_SourceText *source, Py_ssize_t lineno, Py_ssize_t *len);

#endif
