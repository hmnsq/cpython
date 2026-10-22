#यदि_अपरिभाषितम् Py_TOKENIZER_TYPES_H
#परिभाषय Py_TOKENIZER_TYPES_H

#अन्तर्भाव "Python.h"

प्रकारनिर्देश Py_ssize_t _PyTok_Off;

/* Spans use half-open logical byte offsets into decoded input. Their backing
   storage may retain only the current input window. */
प्रकारनिर्देश संरचना {
    _PyTok_Off start;
    _PyTok_Off अन्तः_2;
} Py_Tok_विस्तृतिः;

/* Lines are 1-based and byte columns are 0-based. */
प्रकारनिर्देश संरचना {
    पूर्णाङ्क lineno;
    पूर्णाङ्क byte_col;
} _PyTok_Loc;

स्थिर अन्तःस्थ पूर्णाङ्क
Py_Tok_विस्तृतिः_Is_Valid(Py_Tok_विस्तृतिः विस्तृतिः)
{
    प्रत्यागम विस्तृतिः.start >= 0 && विस्तृतिः.अन्तः_2 >= विस्तृतिः.start;
}

#यदि_अन्तः
