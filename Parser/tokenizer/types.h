#جے_بنا_تعریف Py_TOKENIZER_TYPES_H
#تعریف_کرو Py_TOKENIZER_TYPES_H

#ضمن "Python.h"

تعريفنوع Py_ssize_t _PyTok_Off;

/* Spans use half-open logical byte offsets into decoded input. Their backing
   storage may retain only the current input window. */
تعريفنوع بنية {
    _PyTok_Off شروع;
    _PyTok_Off سرا;
} Py_Tok_وسعت;

/* Lines are 1-based and byte columns are 0-based. */
تعريفنوع بنية {
    صحيح lineno;
    صحيح byte_خانہ;
} _PyTok_Loc;

ساكن مضمن صحيح
Py_Tok_وسعت_Is_Valid(Py_Tok_وسعت وسعت)
{
    أرجع وسعت.شروع >= 0 && وسعت.سرا >= وسعت.شروع;
}

#جے_انت
