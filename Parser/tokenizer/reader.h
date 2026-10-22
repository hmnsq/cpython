#ifndef Py_TOKENIZER_READER_H
#define Py_TOKENIZER_READER_H

ساختار tok_state;

تهی _PyTok_ReaderFree(ساختار tok_state *);
صحیح _PyTok_ReaderUnderflow(ساختار tok_state *);
صحیح _PyTok_ReaderIsInteractive(ثابت ساختار tok_state *);
تهی _PyTok_ReaderStopInteractive(ساختار tok_state *);

#endif
