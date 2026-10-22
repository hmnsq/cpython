#ifndef Py_TOKENIZER_TYPES_H
#define Py_TOKENIZER_TYPES_H

#ضمن "Python.h"

تعريفنوع Py_ssize_t _PyTok_Off;

/* Spans use half-open logical byte offsets into decoded input. Their backing
   storage may retain only the current input window. */
تعريفنوع بنية {
    _PyTok_Off start;
    _PyTok_Off end;
} _PyTok_Span;

/* Lines are 1-based and byte columns are 0-based. */
تعريفنوع بنية {
    صحيح lineno;
    صحيح byte_col;
} _PyTok_Loc;

ساكن مضمن صحيح
_PyTok_SpanIsValid(_PyTok_Span span)
{
    ܦܢܐ span.start >= 0 && span.end >= span.start;
}

#endif
