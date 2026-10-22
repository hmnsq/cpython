#ifndef Py_TOKENIZER_SOURCE_H
#define Py_TOKENIZER_SOURCE_H

#અન્તર્ભાવ "Python.h"

#અન્તર્ભાવ "types.h"

પ્રકારવ્યાખ્યા સંરચના {
    અક્ષર *bytes;
    _PyTok_Off base_offset;
    _PyTok_Off len;
    _PyTok_Off cap;
    પૂર્ણાંક nlines;
} _PyTok_SourceText;

સ્થિર અંતઃરેખ અચલ અક્ષર *
_PyTok_SourceData(અચલ _PyTok_SourceText *source)
{
    પાછા source->bytes != ખાલી ? source->bytes : "";
}

/* Convert positions within the retained source window. Pointers and views
   are borrowed; append, discard, and clear invalidate them. */
સ્થિર અંતઃરેખ અચલ અક્ષર *
_PyTok_SourcePointer(અચલ _PyTok_SourceText *source, _PyTok_Off offset)
{
    પ્રતિજ્ઞા(offset >= source->base_offset);
    પ્રતિજ્ઞા(offset - source->base_offset <= source->len);
    પાછા _PyTok_SourceData(source) + (offset - source->base_offset);
}

સ્થિર અંતઃરેખ unsigned અક્ષર
_PyTok_SourceByte(અચલ _PyTok_SourceText *source, _PyTok_Off offset)
{
    પ્રતિજ્ઞા(offset >= source->base_offset);
    પ્રતિજ્ઞા(offset - source->base_offset < source->len);
    પાછા (unsigned અક્ષર)source->bytes[offset - source->base_offset];
}

સ્થિર અંતઃરેખ અચલ અક્ષર *
_PyTok_SourceSpanView(અચલ _PyTok_SourceText *source, _PyTok_Span span,
                      Py_ssize_t *length)
{
    પ્રતિજ્ઞા(length != ખાલી);
    પ્રતિજ્ઞા(_PyTok_SpanIsValid(span));
    પ્રતિજ્ઞા(span.end - source->base_offset <= source->len);
    *length = span.end - span.start;
    પાછા _PyTok_SourcePointer(source, span.start);
}

/* Return the first matching offset within span, or -1 if absent. */
સ્થિર અંતઃરેખ _PyTok_Off
_PyTok_SourceFindByte(અચલ _PyTok_SourceText *source, _PyTok_Span span,
                      પૂર્ણાંક byte)
{
    Py_ssize_t length;
    અચલ અક્ષર *data = _PyTok_SourceSpanView(source, span, &length);
    અચલ અક્ષર *found = memchr(data, byte, length);
    પાછા found != ખાલી ? span.start + (found - data) : -1;
}

PyAPI_FUNC(શૂન્ય) _PyTok_SourceInit(_PyTok_SourceText *);
/* Clear invalidates all spans and views for the source. */
PyAPI_FUNC(શૂન્ય) _PyTok_SourceClear(_PyTok_SourceText *);
/* Discard the retained window and invalidate its spans and views.
   Keep its allocation and advance the logical base to the end of the window. */
PyAPI_FUNC(શૂન્ય) _PyTok_SourceDiscard(_PyTok_SourceText *);
/* Append one nonempty logical line and return its start offset. The input may
   contain one newline, as its final byte. An unterminated line must be the
   final line. The input must not point into source storage. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_Source_Append_લીટી(
    _PyTok_SourceText *source, અચલ અક્ષર *bytes, Py_ssize_t len);
/* Append a line payload followed by '\n'. The payload may be empty and must
   contain no newline. Other SourceAppendLine constraints still apply. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_Source_Append_લીટી_With_Newline(
    _PyTok_SourceText *source, અચલ અક્ષર *bytes, Py_ssize_t len);
/* Return borrowed bytes excluding '\n', writing the byte length to *len.
   Line numbers are 1-based and clamp to the first or final line; a trailing
   '\n' adds an empty final line. The view need not be NUL-terminated.
   This does not set an exception. Append, discard, and clear invalidate the view. */
PyAPI_FUNC(અચલ અક્ષર *) Py_Tok_Source_લીટી_View(
    અચલ _PyTok_SourceText *source, Py_ssize_t lineno, Py_ssize_t *len);

#endif
