#که_نه_تعریف_شوی Py_TOKENIZER_READER_H
#تعریف_کړه Py_TOKENIZER_READER_H

بنية tok_state;

فراغ _PyTok_ReaderFree(بنية tok_state *);
صحيح _PyTok_ReaderUnderflow(بنية tok_state *);
صحيح _PyTok_ReaderIsInteractive(ثابت بنية tok_state *);
فراغ _PyTok_ReaderStopInteractive(بنية tok_state *);

#د_که_پای
