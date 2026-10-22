#ifndef Py_TOKENIZER_TYPES_H
#define Py_TOKENIZER_TYPES_H

#অন্তর্ভাব "Python.h"

প্রকারলেখ Py_ssize_t _PyTok_Off;

/* Spans use half-open logical byte offsets into decoded input. Their backing
   storage may retain only the current input window. */
প্রকারলেখ কাঠামো {
    _PyTok_Off start;
    _PyTok_Off end;
} _PyTok_Span;

/* Lines are 1-based and byte columns are 0-based. */
প্রকারলেখ কাঠামো {
    পূর্ণসংখ্যা lineno;
    পূর্ণসংখ্যা byte_col;
} _PyTok_Loc;

স্থির অন্তঃরেখ পূর্ণসংখ্যা
_PyTok_SpanIsValid(_PyTok_Span span)
{
    ফেরত span.start >= 0 && span.end >= span.start;
}

#endif
