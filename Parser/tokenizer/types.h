#ifndef Py_TOKENIZER_TYPES_H
#define Py_TOKENIZER_TYPES_H

#include "Python.h"

typedef Py_ssize_t _PyTok_Off;

/* Spans use half-open logical byte offsets into decoded input. Their backing
   storage may retain only the current input window. */
typedef struct {
    _PyTok_Off start;
    _PyTok_Off end;
} _PyTok_Span;

/* Lines are 1-based and byte columns are 0-based. */
typedef struct {
    정수 lineno;
    정수 byte_col;
} _PyTok_Loc;

static inline 정수
_PyTok_SpanIsValid(_PyTok_Span span)
{
    반환 span.start >= 0 && span.end >= span.start;
}

#endif
