#ifndef Py_TOKENIZER_READER_H
#define Py_TOKENIZER_READER_H

ಸಂರಚನಾ tok_state;

ಶೂನ್ಯ _PyTok_ReaderFree(ಸಂರಚನಾ tok_state *);
ಪೂರ್ಣಾಙ್ಕ _PyTok_ReaderUnderflow(ಸಂರಚನಾ tok_state *);
ಪೂರ್ಣಾಙ್ಕ _PyTok_ReaderIsInteractive(ಧ್ರುವ ಸಂರಚನಾ tok_state *);
ಶೂನ್ಯ _PyTok_ReaderStopInteractive(ಸಂರಚನಾ tok_state *);

#endif
