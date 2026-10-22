#నిర్వచితం_కాకపోతే Py_TOKENIZER_TYPES_H
#నిర్వచించు Py_TOKENIZER_TYPES_H

#అన్తర్భావ "Python.h"

రకనిర్వచనం Py_ssize_t _PyTok_Off;

/* Spans use half-open logical byte offsets into decoded input. Their backing
   storage may retain only the current input window. */
రకనిర్వచనం నిర్మాణం {
    _PyTok_Off ప్రారంభం;
    _PyTok_Off చివర;
} Py_Tok_పరిధి;

/* Lines are 1-based and byte columns are 0-based. */
రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం lineno;
    పూర్ణాంకం byte_నిలువు;
} _PyTok_Loc;

స్థిర అంతఃవరుస పూర్ణాంకం
Py_Tok_పరిధి_Is_Valid(Py_Tok_పరిధి పరిధి)
{
    తిరిగి పరిధి.ప్రారంభం >= 0 && పరిధి.చివర >= పరిధి.ప్రారంభం;
}

#అయితే_ముగింపు
