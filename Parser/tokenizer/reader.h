#ifndef Py_TOKENIZER_READER_H
#define Py_TOKENIZER_READER_H

ഘടന tok_state;

ശൂന്യം _PyTok_ReaderFree(ഘടന tok_state *);
പൂർണ്ണസംഖ്യ _PyTok_ReaderUnderflow(ഘടന tok_state *);
പൂർണ്ണസംഖ്യ _PyTok_ReaderIsInteractive(സ്ഥിരം ഘടന tok_state *);
ശൂന്യം _PyTok_ReaderStopInteractive(ഘടന tok_state *);

#endif
