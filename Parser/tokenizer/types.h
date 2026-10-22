#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ Py_TOKENIZER_TYPES_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Py_TOKENIZER_TYPES_H

#ਅਨ੍ਤਰ੍ਭਾਵ "Python.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ Py_ssize_t _PyTok_Off;

/* Spans use half-open logical byte offsets into decoded input. Their backing
   storage may retain only the current input window. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    _PyTok_Off ਸ਼ੁਰੂ;
    _PyTok_Off ਸਿਰਾ;
} Py_Tok_ਵਿਸਤਾਰ;

/* Lines are 1-based and byte columns are 0-based. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ lineno;
    ਪੂਰਨ byte_ਖਾਨਾ;
} _PyTok_Loc;

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ
Py_Tok_ਵਿਸਤਾਰ_Is_Valid(Py_Tok_ਵਿਸਤਾਰ ਵਿਸਤਾਰ)
{
    ਵਾਪਸ ਵਿਸਤਾਰ.ਸ਼ੁਰੂ >= 0 && ਵਿਸਤਾਰ.ਸਿਰਾ >= ਵਿਸਤਾਰ.ਸ਼ੁਰੂ;
}

#ਜੇ_ਅੰਤ
