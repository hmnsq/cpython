#ifndef Py_TOKENIZER_TYPES_H
#define Py_TOKENIZER_TYPES_H

#અન્તર્ભાવ "Python.h"

પ્રકારવ્યાખ્યા Py_ssize_t _PyTok_Off;

/* Spans use half-open logical byte offsets into decoded input. Their backing
   storage may retain only the current input window. */
પ્રકારવ્યાખ્યા સંરચના {
    _PyTok_Off start;
    _PyTok_Off end;
} _PyTok_Span;

/* Lines are 1-based and byte columns are 0-based. */
પ્રકારવ્યાખ્યા સંરચના {
    પૂર્ણાંક lineno;
    પૂર્ણાંક byte_col;
} _PyTok_Loc;

સ્થિર અંતઃરેખ પૂર્ણાંક
_PyTok_SpanIsValid(_PyTok_Span span)
{
    પાછા span.start >= 0 && span.end >= span.start;
}

#endif
