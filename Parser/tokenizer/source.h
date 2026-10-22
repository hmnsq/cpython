#ifndef Py_TOKENIZER_SOURCE_H
#define Py_TOKENIZER_SOURCE_H

#അന്തര്ഭാവ "Python.h"

#അന്തര്ഭാവ "types.h"

തരനിർവചനം ഘടന {
    അക്ഷരം *bytes;
    _PyTok_Off base_offset;
    _PyTok_Off len;
    _PyTok_Off cap;
    പൂർണ്ണസംഖ്യ nlines;
} _PyTok_SourceText;

സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം *
_PyTok_SourceData(സ്ഥിരം _PyTok_SourceText *source)
{
    തിരികെ source->bytes != ശൂന്യ ? source->bytes : "";
}

/* Convert positions within the retained source window. Pointers and views
   are borrowed; append, discard, and clear invalidate them. */
സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം *
_PyTok_SourcePointer(സ്ഥിരം _PyTok_SourceText *source, _PyTok_Off offset)
{
    പ്രതിജ്ഞാ(offset >= source->base_offset);
    പ്രതിജ്ഞാ(offset - source->base_offset <= source->len);
    തിരികെ _PyTok_SourceData(source) + (offset - source->base_offset);
}

സ്ഥിതം വരിയിൽ unsigned അക്ഷരം
_PyTok_SourceByte(സ്ഥിരം _PyTok_SourceText *source, _PyTok_Off offset)
{
    പ്രതിജ്ഞാ(offset >= source->base_offset);
    പ്രതിജ്ഞാ(offset - source->base_offset < source->len);
    തിരികെ (unsigned അക്ഷരം)source->bytes[offset - source->base_offset];
}

സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം *
_PyTok_SourceSpanView(സ്ഥിരം _PyTok_SourceText *source, _PyTok_Span span,
                      Py_ssize_t *length)
{
    പ്രതിജ്ഞാ(length != ശൂന്യ);
    പ്രതിജ്ഞാ(_PyTok_SpanIsValid(span));
    പ്രതിജ്ഞാ(span.end - source->base_offset <= source->len);
    *length = span.end - span.start;
    തിരികെ _PyTok_SourcePointer(source, span.start);
}

/* Return the first matching offset within span, or -1 if absent. */
സ്ഥിതം വരിയിൽ _PyTok_Off
_PyTok_SourceFindByte(സ്ഥിരം _PyTok_SourceText *source, _PyTok_Span span,
                      പൂർണ്ണസംഖ്യ byte)
{
    Py_ssize_t length;
    സ്ഥിരം അക്ഷരം *data = _PyTok_SourceSpanView(source, span, &length);
    സ്ഥിരം അക്ഷരം *found = memchr(data, byte, length);
    തിരികെ found != ശൂന്യ ? span.start + (found - data) : -1;
}

PyAPI_FUNC(ശൂന്യം) _PyTok_SourceInit(_PyTok_SourceText *);
/* Clear invalidates all spans and views for the source. */
PyAPI_FUNC(ശൂന്യം) _PyTok_SourceClear(_PyTok_SourceText *);
/* Discard the retained window and invalidate its spans and views.
   Keep its allocation and advance the logical base to the end of the window. */
PyAPI_FUNC(ശൂന്യം) _PyTok_SourceDiscard(_PyTok_SourceText *);
/* Append one nonempty logical line and return its start offset. The input may
   contain one newline, as its final byte. An unterminated line must be the
   final line. The input must not point into source storage. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_Source_Append_വരി(
    _PyTok_SourceText *source, സ്ഥിരം അക്ഷരം *bytes, Py_ssize_t len);
/* Append a line payload followed by '\n'. The payload may be empty and must
   contain no newline. Other SourceAppendLine constraints still apply. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_Source_Append_വരി_With_Newline(
    _PyTok_SourceText *source, സ്ഥിരം അക്ഷരം *bytes, Py_ssize_t len);
/* Return borrowed bytes excluding '\n', writing the byte length to *len.
   Line numbers are 1-based and clamp to the first or final line; a trailing
   '\n' adds an empty final line. The view need not be NUL-terminated.
   This does not set an exception. Append, discard, and clear invalidate the view. */
PyAPI_FUNC(സ്ഥിരം അക്ഷരം *) Py_Tok_Source_വരി_View(
    സ്ഥിരം _PyTok_SourceText *source, Py_ssize_t lineno, Py_ssize_t *len);

#endif
