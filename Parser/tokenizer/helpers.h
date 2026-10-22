#నిర్వచితం_కాకపోతే _PY_TOKENIZER_HELPERS_H_
#నిర్వచించు _PY_TOKENIZER_HELPERS_H_

#అన్తర్భావ "Python.h"

#అన్తర్భావ "../lexer/state.h"

పూర్ణాంకం _PyTokenizer_syntaxerror(నిర్మాణం tok_state *tok, స్థిరరాశి అక్షరం *format, ...);
/* Positive range columns are 1-based byte columns. A start column of -1
   derives the character column from the reporting cursor; an end column of
   -1 uses the start column. */
పూర్ణాంకం Py_Tokenizer_syntaxerror_తెలిసిన_range(నిర్మాణం tok_state *tok, పూర్ణాంకం నిలువు_స్థానభ్రంశం, పూర్ణాంకం చివర_నిలువు_స్థానభ్రంశం, స్థిరరాశి అక్షరం *format, ...);
పూర్ణాంకం _PyTokenizer_syntaxerror_at(
    నిర్మాణం tok_state *tok, స్థిరరాశి అక్షరం *పంక్తి_ప్రారంభం, Py_ssize_t cursor_స్థానభ్రంశం,
    పూర్ణాంకం lineno, పూర్ణాంకం నిలువు_స్థానభ్రంశం, పూర్ణాంకం చివర_నిలువు_స్థానభ్రంశం, స్థిరరాశి అక్షరం *format, ...);
పూర్ణాంకం _PyTokenizer_indenterror(నిర్మాణం tok_state *tok);
పూర్ణాంకం _PyTokenizer_warn_invalid_escape_sequence(నిర్మాణం tok_state *tok, పూర్ణాంకం మొదటి_invalid_escape_char);
పూర్ణాంకం _PyTokenizer_parser_warn(నిర్మాణం tok_state *tok, PyObject *category, స్థిరరాశి అక్షరం *format, ...);

శూన్య _PyTokenizer_raise_init_error(PyObject *filename);

పూర్ణాంకం _PyTokenizer_ensure_utf8(స్థిరరాశి అక్షరం *పంక్తి, నిర్మాణం tok_state *tok, పూర్ణాంకం lineno);

#నిర్వచితమైతే Py_DEBUG
శూన్య _PyTokenizer_print_escape(FILE *f, స్థిరరాశి అక్షరం *s, Py_ssize_t కొలత);
#అయితే_ముగింపు


#అయితే_ముగింపు
