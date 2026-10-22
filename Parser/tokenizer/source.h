#ifndef Py_TOKENIZER_SOURCE_H
#define Py_TOKENIZER_SOURCE_H

#অন্তর্ভাৱ "Python.h"

#অন্তর্ভাৱ "types.h"

প্রকারনির্দেশ সংরচনা {
    ৱর্ণ *bytes;
    _PyTok_Off base_offset;
    _PyTok_Off len;
    _PyTok_Off cap;
    পূর্ণাঙ্ক nlines;
} _PyTok_SourceText;

স্থির অন্তঃস্থ ধ্রুৱ ৱর্ণ *
_PyTok_SourceData(ধ্রুৱ _PyTok_SourceText *source)
{
    ঘূৰাই source->bytes != রিক্ত ? source->bytes : "";
}

/* Convert positions within the retained source window. Pointers and views
   are borrowed; append, discard, and clear invalidate them. */
স্থির অন্তঃস্থ ধ্রুৱ ৱর্ণ *
_PyTok_SourcePointer(ধ্রুৱ _PyTok_SourceText *source, _PyTok_Off offset)
{
    প্রতিজ্ঞা(offset >= source->base_offset);
    প্রতিজ্ঞা(offset - source->base_offset <= source->len);
    ঘূৰাই _PyTok_SourceData(source) + (offset - source->base_offset);
}

স্থির অন্তঃস্থ unsigned ৱর্ণ
_PyTok_SourceByte(ধ্রুৱ _PyTok_SourceText *source, _PyTok_Off offset)
{
    প্রতিজ্ঞা(offset >= source->base_offset);
    প্রতিজ্ঞা(offset - source->base_offset < source->len);
    ঘূৰাই (unsigned ৱর্ণ)source->bytes[offset - source->base_offset];
}

স্থির অন্তঃস্থ ধ্রুৱ ৱর্ণ *
_PyTok_SourceSpanView(ধ্রুৱ _PyTok_SourceText *source, _PyTok_Span span,
                      Py_ssize_t *length)
{
    প্রতিজ্ঞা(length != রিক্ত);
    প্রতিজ্ঞা(_PyTok_SpanIsValid(span));
    প্রতিজ্ঞা(span.end - source->base_offset <= source->len);
    *length = span.end - span.start;
    ঘূৰাই _PyTok_SourcePointer(source, span.start);
}

/* Return the first matching offset within span, or -1 if absent. */
স্থির অন্তঃস্থ _PyTok_Off
_PyTok_SourceFindByte(ধ্রুৱ _PyTok_SourceText *source, _PyTok_Span span,
                      পূর্ণাঙ্ক byte)
{
    Py_ssize_t length;
    ধ্রুৱ ৱর্ণ *data = _PyTok_SourceSpanView(source, span, &length);
    ধ্রুৱ ৱর্ণ *found = memchr(data, byte, length);
    ঘূৰাই found != রিক্ত ? span.start + (found - data) : -1;
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
PyAPI_FUNC(_PyTok_Off) Py_Tok_Source_Append_সাৰি(
    _PyTok_SourceText *source, ধ্রুৱ ৱর্ণ *bytes, Py_ssize_t len);
/* Append a line payload followed by '\n'. The payload may be empty and must
   contain no newline. Other SourceAppendLine constraints still apply. */
PyAPI_FUNC(_PyTok_Off) Py_Tok_Source_Append_সাৰি_With_Newline(
    _PyTok_SourceText *source, ধ্রুৱ ৱর্ণ *bytes, Py_ssize_t len);
/* Return borrowed bytes excluding '\n', writing the byte length to *len.
   Line numbers are 1-based and clamp to the first or final line; a trailing
   '\n' adds an empty final line. The view need not be NUL-terminated.
   This does not set an exception. Append, discard, and clear invalidate the view. */
PyAPI_FUNC(ধ্রুৱ ৱর্ণ *) Py_Tok_Source_সাৰি_View(
    ধ্রুৱ _PyTok_SourceText *source, Py_ssize_t lineno, Py_ssize_t *len);

#endif
