#اگر_تعریف_نشده Py_TOKENIZER_READER_داخلی_H
#تعریف_کن Py_TOKENIZER_READER_داخلی_H

#ضمن "Python.h"

#تعریف_کن PYTOK_ENCODING_COOKIE_MAX_سطرها 2

تعريفنوع تعداد {
    _PYTOK_READER_PREPARED,
    _PYTOK_READER_FILE,
    _PYTOK_READER_READLINE,
    _PYTOK_READER_INTERACTIVE,
} Py_Tok_Reader_نوع;

تعريفنوع تعداد {
    PYTOK_READ_سطر,
    _PYTOK_READ_EOF,
    _PYTOK_READ_STOPPED,
    PYTOK_READ_وقفه,
    _PYTOK_READ_ERROR,
} Py_Tok_Read_نتیجه;

تعريفنوع تعداد {
    _PYTOK_ENCODING_ERROR = -1,
    PYTOK_ENCODING_انجامشده,
    PYTOK_ENCODING_NEED_دومی_سطر,
} Py_Tok_Encoding_نتیجه;

تعريفنوع تعداد {
    _PYTOK_CHUNK_BORROWED,
    _PYTOK_CHUNK_PYMEM,
    _PYTOK_CHUNK_PYOBJECT,
} _PyTok_ChunkOwnership;

تعريفنوع بنية {
    حرف *داده;
    PyObject *owner;
    Py_ssize_t len;
    _PyTok_ChunkOwnership ownership;
    بیعلامت حرف implicit_newline;
} _PyTok_Chunk;

تعريفنوع بنية _PyTok_Reader {
    FILE *fp;  // Borrowed input stream; NULL for string and readline input.
    حرف *encoding;  // Owned source encoding.
    PyObject *readline;
    PyObject *decoder;
    ثابت حرف *prompt;
    ثابت حرف *nextprompt;

    حرف *file_میانگیر;
    Py_ssize_t file_میانگیر_cap;
    _PyTok_Chunk prefetched_سطرها[PYTOK_ENCODING_COOKIE_MAX_سطرها];

    حرف *decoded;
    Py_ssize_t decoded_pos;
    Py_ssize_t decoded_پیمایش;  // First byte not yet scanned for a newline.
    Py_ssize_t decoded_len;
    Py_ssize_t decoded_cap;
    Py_Tok_Reader_نوع نوع;
    بیعلامت حرف prepared_final_newline;  // _PyTokenizer_Newline
    بیعلامت حرف decoded_دم_is_implicit;
    بیعلامت حرف file_initialized;
    بیعلامت حرف file_eof;
    بیعلامت حرف decoder_finalized;
    بیعلامت حرف stop_interactive;
} _PyTok_Reader;

بنية tok_state;

// On failure, the buffer and capacity are unchanged.
صحيح Py_Tok_Reserve_میانگیر(حرف **, Py_ssize_t *, Py_ssize_t, Py_ssize_t);
حرف *Py_Tok_Copy_بایتها(ثابت حرف *, Py_ssize_t);
صحيح _PyTok_DecodeOnce(
    بنية tok_state *, _PyTok_Chunk *, ثابت حرف *, ثابت حرف *);
حرف *_PyTok_NormalizeNewlines(ثابت حرف *, Py_ssize_t, Py_ssize_t *);
فراغ _PyTok_ChunkClear(_PyTok_Chunk *);
صحيح Py_Tok_مجموعه_Encoding(بنية tok_state *, ثابت حرف *);
Py_Tok_Encoding_نتیجه _PyTok_DetectEncoding(
    بنية tok_state *, ثابت _PyTok_Chunk *, ثابت _PyTok_Chunk *, صحيح,
    Py_ssize_t *);
صحيح _PyTok_PrepareString(بنية tok_state *, ثابت حرف *, صحيح, صحيح, صحيح);
صحيح Py_Tok_شروع_Decoder(بنية tok_state *, ثابت حرف *);
صحيح _PyTok_DecodeChunk(بنية tok_state *, _PyTok_Chunk *, صحيح);
صحيح Py_Tok_Decoder_Has_Buffered_درون_داد(بنية tok_state *);

#پایان_اگر
