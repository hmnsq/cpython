#ifndef Py_TOKENIZER_READER_H
#define Py_TOKENIZER_READER_H

কাঠামো tok_state;

শূন্য _PyTok_ReaderFree(কাঠামো tok_state *);
পূর্ণসংখ্যা _PyTok_ReaderUnderflow(কাঠামো tok_state *);
পূর্ণসংখ্যা _PyTok_ReaderIsInteractive(ধ্রুবক কাঠামো tok_state *);
শূন্য _PyTok_ReaderStopInteractive(কাঠামো tok_state *);

#endif
