#ifndef Py_TOKENIZER_TYPES_H
#define Py_TOKENIZER_TYPES_H

#include "Python.h"

typedef Py_ssize_t _PyTok_Off;

/* Spans use half-open logical byte offsets into decoded input. Their backing
   storage may retain only the current input window. */
typedef sa_nracanaa {
    _PyTok_Off start;
    _PyTok_Off end;
} _PyTok_Span;

/* Lines are 1-based and byte columns are 0-based. */
typedef sa_nracanaa {
    ཧྲིལ lineno;
    ཧྲིལ byte_col;
} _PyTok_Loc;

static anta_hstha ཧྲིལ
_PyTok_SpanIsValid(_PyTok_Span span)
{
    ལོག span.start >= 0 && span.end >= span.start;
}

#endif
