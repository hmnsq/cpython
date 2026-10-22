#اگر_تعریف_نشده Py_TOKENIZER_READER_H
#تعریف_کن Py_TOKENIZER_READER_H

بنية tok_state;

فراغ _PyTok_ReaderFree(بنية tok_state *);
صحيح _PyTok_ReaderUnderflow(بنية tok_state *);
صحيح _PyTok_ReaderIsInteractive(ثابت بنية tok_state *);
فراغ _PyTok_ReaderStopInteractive(بنية tok_state *);

#پایان_اگر
