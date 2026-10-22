#ifndef Py_TOKENIZER_TYPES_H
#define Py_TOKENIZER_TYPES_H

#അന്തര്ഭാവ "Python.h"

തരനിർവചനം Py_ssize_t _PyTok_Off;

/* Spans use half-open logical byte offsets into decoded input. Their backing
   storage may retain only the current input window. */
തരനിർവചനം ഘടന {
    _PyTok_Off start;
    _PyTok_Off end;
} _PyTok_Span;

/* Lines are 1-based and byte columns are 0-based. */
തരനിർവചനം ഘടന {
    പൂർണ്ണസംഖ്യ lineno;
    പൂർണ്ണസംഖ്യ byte_col;
} _PyTok_Loc;

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ
_PyTok_SpanIsValid(_PyTok_Span span)
{
    തിരികെ span.start >= 0 && span.end >= span.start;
}

#endif
