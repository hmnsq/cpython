#ifndef Py_TOKENIZER_TYPES_H
#define Py_TOKENIZER_TYPES_H

#अन्तर्भाव "Python.h"

प्रकारव्याख्या Py_ssize_t _PyTok_Off;

/* Spans use half-open logical byte offsets into decoded input. Their backing
   storage may retain only the current input window. */
प्रकारव्याख्या संरचना {
    _PyTok_Off start;
    _PyTok_Off end;
} _PyTok_Span;

/* Lines are 1-based and byte columns are 0-based. */
प्रकारव्याख्या संरचना {
    पूर्णांक lineno;
    पूर्णांक byte_col;
} _PyTok_Loc;

स्थिर ओळीत पूर्णांक
_PyTok_SpanIsValid(_PyTok_Span span)
{
    परत span.start >= 0 && span.end >= span.start;
}

#endif
