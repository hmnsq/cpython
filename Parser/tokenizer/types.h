#ifndef Py_TOKENIZER_TYPES_H
#define Py_TOKENIZER_TYPES_H

#ಅನ್ತರ್ಭಾವ "Python.h"

ಪ್ರಕಾರನಿರ್ದೇಶ Py_ssize_t _PyTok_Off;

/* Spans use half-open logical byte offsets into decoded input. Their backing
   storage may retain only the current input window. */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    _PyTok_Off start;
    _PyTok_Off end;
} _PyTok_Span;

/* Lines are 1-based and byte columns are 0-based. */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಪೂರ್ಣಾಙ್ಕ lineno;
    ಪೂರ್ಣಾಙ್ಕ byte_col;
} _PyTok_Loc;

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ
_PyTok_SpanIsValid(_PyTok_Span span)
{
    ಪ್ರತ್ಯಾಗಮ span.start >= 0 && span.end >= span.start;
}

#endif
