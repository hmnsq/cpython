#ifndef Py_TOKENIZER_READER_H
#define Py_TOKENIZER_READER_H

সংরচনা tok_state;

শূন্য _PyTok_ReaderFree(সংরচনা tok_state *);
পূর্ণাঙ্ক _PyTok_ReaderUnderflow(সংরচনা tok_state *);
পূর্ণাঙ্ক _PyTok_ReaderIsInteractive(ধ্রুৱ সংরচনা tok_state *);
শূন্য _PyTok_ReaderStopInteractive(সংরচনা tok_state *);

#endif
