#ifndef Py_TOKENIZER_READER_H
#define Py_TOKENIZER_READER_H

ସଂରଚନା tok_state;

ଶୂନ୍ୟ _PyTok_ReaderFree(ସଂରଚନା tok_state *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyTok_ReaderUnderflow(ସଂରଚନା tok_state *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyTok_ReaderIsInteractive(ଅଚଳ ସଂରଚନା tok_state *);
ଶୂନ୍ୟ _PyTok_ReaderStopInteractive(ସଂରଚନା tok_state *);

#endif
