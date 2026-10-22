#ifndef Py_TOKENIZER_TYPES_H
#define Py_TOKENIZER_TYPES_H

#ଅନ୍ତର୍ଭାଵ "Python.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା Py_ssize_t _PyTok_Off;

/* Spans use half-open logical byte offsets into decoded input. Their backing
   storage may retain only the current input window. */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    _PyTok_Off start;
    _PyTok_Off end;
} _PyTok_Span;

/* Lines are 1-based and byte columns are 0-based. */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା byte_col;
} _PyTok_Loc;

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyTok_SpanIsValid(_PyTok_Span span)
{
    ଫେରନ୍ତୁ span.start >= 0 && span.end >= span.start;
}

#endif
