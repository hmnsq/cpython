#ifndef Py_TOKENIZER_SOURCE_H
#define Py_TOKENIZER_SOURCE_H

#অন্তর্ভাব "Python.h"

#অন্তর্ভাব "types.h"

প্রকারলেখ কাঠামো {
    অক্ষর *bytes;
    _PyTok_Off base_offset;
    _PyTok_Off len;
    _PyTok_Off cap;
    পূর্ণসংখ্যা nlines;
} _PyTok_SourceText;

স্থির অন্তঃরেখ ধ্রুবক অক্ষর *
Py_Tok_Source_তথ্য(ধ্রুবক _PyTok_SourceText *source)
{
    ফেরত source->bytes != ফাঁকা ? source->bytes : "";
}

/* Convert positions within the retained source window. Pointers and views
   are borrowed; append, discard, and clear invalidate them. */
স্থির অন্তঃরেখ ধ্রুবক অক্ষর *
_PyTok_SourcePointer(ধ্রুবক _PyTok_SourceText *source, _PyTok_Off offset)
{
    প্রতিজ্ঞা(offset >= source->base_offset);
    প্রতিজ্ঞা(offset - source->base_offset <= source->len);
    ফেরত Py_Tok_Source_তথ্য(source) + (offset - source->base_offset);
}

স্থির অন্তঃরেখ unsigned অক্ষর
_PyTok_SourceByte(ধ্রুবক _PyTok_SourceText *source, _PyTok_Off offset)
{
    প্রতিজ্ঞা(offset >= source->base_offset);
    প্রতিজ্ঞা(offset - source->base_offset < source->len);
    ফেরত (unsigned অক্ষর)source->bytes[offset - source->base_offset];
}

স্থির অন্তঃরেখ ধ্রুবক অক্ষর *
_PyTok_SourceSpanView(ধ্রুবক _PyTok_SourceText *source, _PyTok_Span span,
                      Py_ssize_t *দৈর্ঘ্য)
{
    প্রতিজ্ঞা(দৈর্ঘ্য != ফাঁকা);
    প্রতিজ্ঞা(_PyTok_SpanIsValid(span));
    প্রতিজ্ঞা(span.end - source->base_offset <= source->len);
    *দৈর্ঘ্য = span.end - span.start;
    ফেরত _PyTok_SourcePointer(source, span.start);
}

/* Return the first matching offset within span, or -1 if absent. */
স্থির অন্তঃরেখ _PyTok_Off
_PyTok_SourceFindByte(ধ্রুবক _PyTok_SourceText *source, _PyTok_Span span,
                      পূর্ণসংখ্যা byte)
{
    Py_ssize_t দৈর্ঘ্য;
    ধ্রুবক অক্ষর *তথ্য = _PyTok_SourceSpanView(source, span, &দৈর্ঘ্য);
    ধ্রুবক অক্ষর *found = memchr(তথ্য, byte, দৈর্ঘ্য);
    ফেরত found != ফাঁকা ? span.start + (found - তথ্য) : -1;
}

PyAPI_FUNC(শূন্য) _PyTok_SourceInit(_PyTok_SourceText *);
/* Clear invalidates all spans and views for the source. */
PyAPI_FUNC(শূন্য) _PyTok_SourceClear(_PyTok_SourceText *);
/* Discard the retained window and invalidate its spans and views.
   Keep its allocation and advance the logical base to the end of the window. */
PyAPI_FUNC(শূন্য) _PyTok_SourceDiscard(_PyTok_SourceText *);
/* Append one nonempty logical line and return its start offset. The input may
   contain one newline, as its final byte. An unterminated line must be the
   final line. The input must not point into source storage. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_Source_Append_সারি(
    _PyTok_SourceText *source, ধ্রুবক অক্ষর *bytes, Py_ssize_t len);
/* Append a line payload followed by '\n'. The payload may be empty and must
   contain no newline. Other SourceAppendLine constraints still apply. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_Source_Append_সারি_With_Newline(
    _PyTok_SourceText *source, ধ্রুবক অক্ষর *bytes, Py_ssize_t len);
/* Return borrowed bytes excluding '\n', writing the byte length to *len.
   Line numbers are 1-based and clamp to the first or final line; a trailing
   '\n' adds an empty final line. The view need not be NUL-terminated.
   This does not set an exception. Append, discard, and clear invalidate the view. */
PyAPI_FUNC(ধ্রুবক অক্ষর *) Py_Tok_Source_সারি_View(
    ধ্রুবক _PyTok_SourceText *source, Py_ssize_t lineno, Py_ssize_t *len);

#endif
