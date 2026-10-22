#اگر_غیر_تعریف_شدہ Py_TOKENIZER_TYPES_H
#تعریف_کرو Py_TOKENIZER_TYPES_H

#شامل "Python.h"

قسمتعریف Py_ssize_t _PyTok_Off;

/* Spans use half-open logical byte offsets into decoded input. Their backing
   storage may retain only the current input window. */
قسمتعریف ساخت {
    _PyTok_Off آغاز;
    _PyTok_Off انتہا;
} Py_Tok_وسعت;

/* Lines are 1-based and byte columns are 0-based. */
قسمتعریف ساخت {
    صحیح lineno;
    صحیح byte_ستون;
} _PyTok_Loc;

ساکن اندرونی صحیح
Py_Tok_وسعت_Is_Valid(Py_Tok_وسعت وسعت)
{
    واپس وسعت.آغاز >= 0 && وسعت.انتہا >= وسعت.آغاز;
}

#اگر_ختم
