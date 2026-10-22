#ifndef Py_TOKENIZER_READER_H
#define Py_TOKENIZER_READER_H

સંરચના tok_state;

શૂન્ય _PyTok_ReaderFree(સંરચના tok_state *);
પૂર્ણાંક _PyTok_ReaderUnderflow(સંરચના tok_state *);
પૂર્ણાંક _PyTok_ReaderIsInteractive(અચલ સંરચના tok_state *);
શૂન્ય _PyTok_ReaderStopInteractive(સંરચના tok_state *);

#endif
