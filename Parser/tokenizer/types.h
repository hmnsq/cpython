#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ Py_TOKENIZER_TYPES_H
#ವ್ಯಾಖ್ಯಾನಿಸು Py_TOKENIZER_TYPES_H

#ಅನ್ತರ್ಭಾವ "Python.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ Py_ssize_t _PyTok_Off;

/* Spans use half-open logical byte offsets into decoded input. Their backing
   storage may retain only the current input window. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    _PyTok_Off ಆರಂಭ;
    _PyTok_Off ಕೊನೆ;
} Py_Tok_ಹರವು;

/* Lines are 1-based and byte columns are 0-based. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ lineno;
    ಪೂರ್ಣಾಂಕ byte_ಲಂಬಸಾಲು;
} _PyTok_Loc;

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ
Py_Tok_ಹರವು_Is_Valid(Py_Tok_ಹರವು ಹರವು)
{
    ಹಿಂತಿರುಗಿ ಹರವು.ಆರಂಭ >= 0 && ಹರವು.ಕೊನೆ >= ಹರವು.ಆರಂಭ;
}

#ಆದರೆ_ಅಂತ್ಯ
