#ifndef Py_TOKENIZER_SOURCE_H
#define Py_TOKENIZER_SOURCE_H

#ଅନ୍ତର୍ଭାଵ "Python.h"

#ଅନ୍ତର୍ଭାଵ "types.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ଅକ୍ଷର *bytes;
    _PyTok_Off base_offset;
    _PyTok_Off len;
    _PyTok_Off cap;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା nlines;
} _PyTok_SourceText;

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର *
_PyTok_SourceData(ଅଚଳ _PyTok_SourceText *source)
{
    ଫେରନ୍ତୁ source->bytes != ଖାଲି ? source->bytes : "";
}

/* Convert positions within the retained source window. Pointers and views
   are borrowed; append, discard, and clear invalidate them. */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର *
_PyTok_SourcePointer(ଅଚଳ _PyTok_SourceText *source, _PyTok_Off offset)
{
    ପ୍ରତିଜ୍ଞା(offset >= source->base_offset);
    ପ୍ରତିଜ୍ଞା(offset - source->base_offset <= source->len);
    ଫେରନ୍ତୁ _PyTok_SourceData(source) + (offset - source->base_offset);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ unsigned ଅକ୍ଷର
_PyTok_SourceByte(ଅଚଳ _PyTok_SourceText *source, _PyTok_Off offset)
{
    ପ୍ରତିଜ୍ଞା(offset >= source->base_offset);
    ପ୍ରତିଜ୍ଞା(offset - source->base_offset < source->len);
    ଫେରନ୍ତୁ (unsigned ଅକ୍ଷର)source->bytes[offset - source->base_offset];
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର *
_PyTok_SourceSpanView(ଅଚଳ _PyTok_SourceText *source, _PyTok_Span span,
                      Py_ssize_t *length)
{
    ପ୍ରତିଜ୍ଞା(length != ଖାଲି);
    ପ୍ରତିଜ୍ଞା(_PyTok_SpanIsValid(span));
    ପ୍ରତିଜ୍ଞା(span.end - source->base_offset <= source->len);
    *length = span.end - span.start;
    ଫେରନ୍ତୁ _PyTok_SourcePointer(source, span.start);
}

/* Return the first matching offset within span, or -1 if absent. */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ _PyTok_Off
_PyTok_SourceFindByte(ଅଚଳ _PyTok_SourceText *source, _PyTok_Span span,
                      ପୂର୍ଣ୍ଣସଂଖ୍ୟା byte)
{
    Py_ssize_t length;
    ଅଚଳ ଅକ୍ଷର *data = _PyTok_SourceSpanView(source, span, &length);
    ଅଚଳ ଅକ୍ଷର *found = memchr(data, byte, length);
    ଫେରନ୍ତୁ found != ଖାଲି ? span.start + (found - data) : -1;
}

PyAPI_FUNC(ଶୂନ୍ୟ) _PyTok_SourceInit(_PyTok_SourceText *);
/* Clear invalidates all spans and views for the source. */
PyAPI_FUNC(ଶୂନ୍ୟ) _PyTok_SourceClear(_PyTok_SourceText *);
/* Discard the retained window and invalidate its spans and views.
   Keep its allocation and advance the logical base to the end of the window. */
PyAPI_FUNC(ଶୂନ୍ୟ) _PyTok_SourceDiscard(_PyTok_SourceText *);
/* Append one nonempty logical line and return its start offset. The input may
   contain one newline, as its final byte. An unterminated line must be the
   final line. The input must not point into source storage. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_Source_Append_ଧାଡ଼ି(
    _PyTok_SourceText *source, ଅଚଳ ଅକ୍ଷର *bytes, Py_ssize_t len);
/* Append a line payload followed by '\n'. The payload may be empty and must
   contain no newline. Other SourceAppendLine constraints still apply. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_Source_Append_ଧାଡ଼ି_With_Newline(
    _PyTok_SourceText *source, ଅଚଳ ଅକ୍ଷର *bytes, Py_ssize_t len);
/* Return borrowed bytes excluding '\n', writing the byte length to *len.
   Line numbers are 1-based and clamp to the first or final line; a trailing
   '\n' adds an empty final line. The view need not be NUL-terminated.
   This does not set an exception. Append, discard, and clear invalidate the view. */
PyAPI_FUNC(ଅଚଳ ଅକ୍ଷର *) Py_Tok_Source_ଧାଡ଼ି_View(
    ଅଚଳ _PyTok_SourceText *source, Py_ssize_t lineno, Py_ssize_t *len);

#endif
