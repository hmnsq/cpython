#جے_بنا_تعریف Py_TOKENIZER_READER_اندرونی_H
#تعریف_کرو Py_TOKENIZER_READER_اندرونی_H

#ضمن "Python.h"

#تعریف_کرو PYTOK_ENCODING_COOKIE_MAX_سطریں 2

تعريفنوع تعداد {
    _PYTOK_READER_PREPARED,
    _PYTOK_READER_FILE,
    _PYTOK_READER_READLINE,
    _PYTOK_READER_INTERACTIVE,
} Py_Tok_Reader_قسم;

تعريفنوع تعداد {
    PYTOK_READ_سطر,
    _PYTOK_READ_EOF,
    _PYTOK_READ_STOPPED,
    PYTOK_READ_رکاوٹ,
    _PYTOK_READ_ERROR,
} Py_Tok_Read_نتیجہ;

تعريفنوع تعداد {
    _PYTOK_ENCODING_ERROR = -1,
    PYTOK_ENCODING_پورا,
    PYTOK_ENCODING_NEED_دوجا_سطر,
} Py_Tok_Encoding_نتیجہ;

تعريفنوع تعداد {
    _PYTOK_CHUNK_BORROWED,
    _PYTOK_CHUNK_PYMEM,
    _PYTOK_CHUNK_PYOBJECT,
} _PyTok_ChunkOwnership;

تعريفنوع بنية {
    حرف *انکڑے;
    PyObject *owner;
    Py_ssize_t len;
    _PyTok_ChunkOwnership ownership;
    بنانشانی حرف implicit_newline;
} _PyTok_Chunk;

تعريفنوع بنية _PyTok_Reader {
    FILE *fp;  // Borrowed input stream; NULL for string and readline input.
    حرف *encoding;  // Owned source encoding.
    PyObject *readline;
    PyObject *decoder;
    ثابت حرف *prompt;
    ثابت حرف *nextprompt;

    حرف *file_بفر;
    Py_ssize_t file_بفر_cap;
    _PyTok_Chunk prefetched_سطریں[PYTOK_ENCODING_COOKIE_MAX_سطریں];

    حرف *decoded;
    Py_ssize_t decoded_pos;
    Py_ssize_t decoded_جانچ;  // First byte not yet scanned for a newline.
    Py_ssize_t decoded_len;
    Py_ssize_t decoded_cap;
    Py_Tok_Reader_قسم قسم;
    بنانشانی حرف prepared_final_newline;  // _PyTokenizer_Newline
    بنانشانی حرف decoded_پوچھ_is_implicit;
    بنانشانی حرف file_initialized;
    بنانشانی حرف file_eof;
    بنانشانی حرف decoder_finalized;
    بنانشانی حرف stop_interactive;
} _PyTok_Reader;

بنية tok_state;

// On failure, the buffer and capacity are unchanged.
صحيح Py_Tok_Reserve_بفر(حرف **, Py_ssize_t *, Py_ssize_t, Py_ssize_t);
حرف *Py_Tok_Copy_بائٹاں(ثابت حرف *, Py_ssize_t);
صحيح _PyTok_DecodeOnce(
    بنية tok_state *, _PyTok_Chunk *, ثابت حرف *, ثابت حرف *);
حرف *_PyTok_NormalizeNewlines(ثابت حرف *, Py_ssize_t, Py_ssize_t *);
فراغ _PyTok_ChunkClear(_PyTok_Chunk *);
صحيح Py_Tok_سیٹ_Encoding(بنية tok_state *, ثابت حرف *);
Py_Tok_Encoding_نتیجہ _PyTok_DetectEncoding(
    بنية tok_state *, ثابت _PyTok_Chunk *, ثابت _PyTok_Chunk *, صحيح,
    Py_ssize_t *);
صحيح _PyTok_PrepareString(بنية tok_state *, ثابت حرف *, صحيح, صحيح, صحيح);
صحيح Py_Tok_شروع_Decoder(بنية tok_state *, ثابت حرف *);
صحيح _PyTok_DecodeChunk(بنية tok_state *, _PyTok_Chunk *, صحيح);
صحيح Py_Tok_Decoder_Has_Buffered_داخلہ(بنية tok_state *);

#جے_انت
