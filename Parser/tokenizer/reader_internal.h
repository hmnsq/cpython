#اگر_غیر_تعریف_شدہ Py_TOKENIZER_READER_اندرونی_H
#تعریف_کرو Py_TOKENIZER_READER_اندرونی_H

#شامل "Python.h"

#تعریف_کرو PYTOK_ENCODING_COOKIE_MAX_سطریں 2

قسمتعریف شمار {
    _PYTOK_READER_PREPARED,
    _PYTOK_READER_FILE,
    _PYTOK_READER_READLINE,
    _PYTOK_READER_INTERACTIVE,
} Py_Tok_Reader_قسم;

قسمتعریف شمار {
    PYTOK_READ_سطر,
    _PYTOK_READ_EOF,
    _PYTOK_READ_STOPPED,
    PYTOK_READ_مداخلت,
    _PYTOK_READ_ERROR,
} Py_Tok_Read_نتیجہ;

قسمتعریف شمار {
    _PYTOK_ENCODING_ERROR = -1,
    PYTOK_ENCODING_مکمل,
    PYTOK_ENCODING_NEED_دوسرا_سطر,
} Py_Tok_Encoding_نتیجہ;

قسمتعریف شمار {
    _PYTOK_CHUNK_BORROWED,
    _PYTOK_CHUNK_PYMEM,
    _PYTOK_CHUNK_PYOBJECT,
} _PyTok_ChunkOwnership;

قسمتعریف ساخت {
    حرف *ڈیٹا;
    PyObject *owner;
    Py_ssize_t len;
    _PyTok_ChunkOwnership ownership;
    غیرعلامتی حرف implicit_newline;
} _PyTok_Chunk;

قسمتعریف ساخت _PyTok_Reader {
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
    Py_ssize_t decoded_جائزہ;  // First byte not yet scanned for a newline.
    Py_ssize_t decoded_len;
    Py_ssize_t decoded_cap;
    Py_Tok_Reader_قسم قسم;
    غیرعلامتی حرف prepared_final_newline;  // _PyTokenizer_Newline
    غیرعلامتی حرف decoded_دم_is_implicit;
    غیرعلامتی حرف file_initialized;
    غیرعلامتی حرف file_eof;
    غیرعلامتی حرف decoder_finalized;
    غیرعلامتی حرف stop_interactive;
} _PyTok_Reader;

ساخت tok_state;

// On failure, the buffer and capacity are unchanged.
صحیح Py_Tok_Reserve_بفر(حرف **, Py_ssize_t *, Py_ssize_t, Py_ssize_t);
حرف *Py_Tok_Copy_بائٹیں(ثابت حرف *, Py_ssize_t);
صحیح _PyTok_DecodeOnce(
    ساخت tok_state *, _PyTok_Chunk *, ثابت حرف *, ثابت حرف *);
حرف *_PyTok_NormalizeNewlines(ثابت حرف *, Py_ssize_t, Py_ssize_t *);
خالی _PyTok_ChunkClear(_PyTok_Chunk *);
صحیح Py_Tok_سیٹ_Encoding(ساخت tok_state *, ثابت حرف *);
Py_Tok_Encoding_نتیجہ _PyTok_DetectEncoding(
    ساخت tok_state *, ثابت _PyTok_Chunk *, ثابت _PyTok_Chunk *, صحیح,
    Py_ssize_t *);
صحیح _PyTok_PrepareString(ساخت tok_state *, ثابت حرف *, صحیح, صحیح, صحیح);
صحیح Py_Tok_آغاز_Decoder(ساخت tok_state *, ثابت حرف *);
صحیح _PyTok_DecodeChunk(ساخت tok_state *, _PyTok_Chunk *, صحیح);
صحیح Py_Tok_Decoder_Has_Buffered_داخلہ(ساخت tok_state *);

#اگر_ختم
