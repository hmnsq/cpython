#جيڪڏهن_وصف_نه_ٿيل Py_TOKENIZER_TYPES_H
#وصف_ڪريو Py_TOKENIZER_TYPES_H

#ضمن "Python.h"

تعريفنوع Py_ssize_t _PyTok_Off;

/* Spans use half-open logical byte offsets into decoded input. Their backing
   storage may retain only the current input window. */
تعريفنوع بنية {
    _PyTok_Off شروعات;
    _PyTok_Off پڇاڙي;
} Py_Tok_وسعت;

/* Lines are 1-based and byte columns are 0-based. */
تعريفنوع بنية {
    صحيح lineno;
    صحيح byte_کانو;
} _PyTok_Loc;

ساكن مضمن صحيح
Py_Tok_وسعت_Is_Valid(Py_Tok_وسعت وسعت)
{
    أرجع وسعت.شروعات >= 0 && وسعت.پڇاڙي >= وسعت.شروعات;
}

#جيڪڏهن_پورو
