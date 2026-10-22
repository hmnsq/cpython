#ifndef Py_TOKENIZER_READER_H
#define Py_TOKENIZER_READER_H

संरचना tok_state;

शून्य _PyTok_ReaderFree(संरचना tok_state *);
पूर्णांक _PyTok_ReaderUnderflow(संरचना tok_state *);
पूर्णांक _PyTok_ReaderIsInteractive(स्थिरांक संरचना tok_state *);
शून्य _PyTok_ReaderStopInteractive(संरचना tok_state *);

#endif
