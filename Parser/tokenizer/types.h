#ifndef Py_TOKENIZER_TYPES_H
#define Py_TOKENIZER_TYPES_H

#அந்தர்பாவ "Python.h"

வகைவரை Py_ssize_t _PyTok_Off;

/* Spans use half-open logical byte offsets into decoded input. Their backing
   storage may retain only the current input window. */
வகைவரை கட்டமைப்பு {
    _PyTok_Off start;
    _PyTok_Off end;
} _PyTok_Span;

/* Lines are 1-based and byte columns are 0-based. */
வகைவரை கட்டமைப்பு {
    முழு lineno;
    முழு byte_col;
} _PyTok_Loc;

நிலையான உள்வரி முழு
_PyTok_SpanIsValid(_PyTok_Span span)
{
    திரும்பு span.start >= 0 && span.end >= span.start;
}

#endif
