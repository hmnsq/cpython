#که_نه_تعریف_شوی Py_TOKENIZER_TYPES_H
#تعریف_کړه Py_TOKENIZER_TYPES_H

#ضمن "Python.h"

تعريفنوع Py_ssize_t _PyTok_Off;

/* Spans use half-open logical byte offsets into decoded input. Their backing
   storage may retain only the current input window. */
تعريفنوع بنية {
    _PyTok_Off پیل;
    _PyTok_Off پای_ټکی;
} Py_Tok_پراختیا;

/* Lines are 1-based and byte columns are 0-based. */
تعريفنوع بنية {
    صحيح lineno;
    صحيح byte_ستون;
} _PyTok_Loc;

ساكن مضمن صحيح
Py_Tok_پراختیا_Is_Valid(Py_Tok_پراختیا پراختیا)
{
    أرجع پراختیا.پیل >= 0 && پراختیا.پای_ټکی >= پراختیا.پیل;
}

#د_که_پای
