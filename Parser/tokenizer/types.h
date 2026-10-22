#ifndef Py_TOKENIZER_TYPES_H
#define Py_TOKENIZER_TYPES_H

#अन्तर्भाव "Python.h"

प्रकारनिर्देश Py_ssize_t _PyTok_Off;

/* Spans use half-open logical byte offsets into decoded input. Their backing
   storage may retain only the current input window. */
प्रकारनिर्देश संरचना {
    _PyTok_Off start;
    _PyTok_Off end;
} _PyTok_Span;

/* Lines are 1-based and byte columns are 0-based. */
प्रकारनिर्देश संरचना {
    पूर्णाङ्क lineno;
    पूर्णाङ्क byte_col;
} _PyTok_Loc;

स्थिर अन्तःस्थ पूर्णाङ्क
_PyTok_SpanIsValid(_PyTok_Span span)
{
    फर्काऊ span.start >= 0 && span.end >= span.start;
}

#endif
