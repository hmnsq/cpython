#ifndef Py_TOKENIZER_TYPES_H
#define Py_TOKENIZER_TYPES_H

#شامل "Python.h"

تعریفنوع Py_ssize_t _PyTok_Off;

/* Spans use half-open logical byte offsets into decoded input. Their backing
   storage may retain only the current input window. */
تعریفنوع ساختار {
    _PyTok_Off start;
    _PyTok_Off end;
} _PyTok_Span;

/* Lines are 1-based and byte columns are 0-based. */
تعریفنوع ساختار {
    صحیح lineno;
    صحیح byte_col;
} _PyTok_Loc;

ایستا درونخطی صحیح
_PyTok_SpanIsValid(_PyTok_Span span)
{
    بازگشت span.start >= 0 && span.end >= span.start;
}

#endif
