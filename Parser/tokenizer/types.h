#اگر_تعریف_نشده Py_TOKENIZER_TYPES_H
#تعریف_کن Py_TOKENIZER_TYPES_H

#ضمن "Python.h"

تعريفنوع Py_ssize_t _PyTok_Off;

/* Spans use half-open logical byte offsets into decoded input. Their backing
   storage may retain only the current input window. */
تعريفنوع بنية {
    _PyTok_Off شروع;
    _PyTok_Off انتها;
} Py_Tok_بازه;

/* Lines are 1-based and byte columns are 0-based. */
تعريفنوع بنية {
    صحيح lineno;
    صحيح byte_ستون;
} _PyTok_Loc;

ساكن مضمن صحيح
Py_Tok_بازه_Is_Valid(Py_Tok_بازه بازه)
{
    أرجع بازه.شروع >= 0 && بازه.انتها >= بازه.شروع;
}

#پایان_اگر
