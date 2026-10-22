#ifndef Py_TOKENIZER_TYPES_H
#define Py_TOKENIZER_TYPES_H

#অন্তর্ভাব "Python.h"

প্রকারনির্দেশ Py_ssize_t _PyTok_Off;

/* Spans use half-open logical byte offsets into decoded input. Their backing
   storage may retain only the current input window. */
প্রকারনির্দেশ সংরচনা {
    _PyTok_Off start;
    _PyTok_Off end;
} _PyTok_Span;

/* Lines are 1-based and byte columns are 0-based. */
প্রকারনির্দেশ সংরচনা {
    পূর্ণাঙ্ক lineno;
    পূর্ণাঙ্ক byte_col;
} _PyTok_Loc;

স্থির অন্তঃস্থ পূর্ণাঙ্ক
_PyTok_SpanIsValid(_PyTok_Span span)
{
    প্রত্যাগম span.start >= 0 && span.end >= span.start;
}

#endif
