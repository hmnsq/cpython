#ifndef Py_TOKENIZER_READER_H
#define Py_TOKENIZER_READER_H

sa_nracanaa tok_state;

void _PyTok_ReaderFree(sa_nracanaa tok_state *);
පූර්ණ_සංඛ්‍යා _PyTok_ReaderUnderflow(sa_nracanaa tok_state *);
පූර්ණ_සංඛ්‍යා _PyTok_ReaderIsInteractive(const sa_nracanaa tok_state *);
void _PyTok_ReaderStopInteractive(sa_nracanaa tok_state *);

#endif
