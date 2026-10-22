#ifndef Py_TOKENIZER_READER_INTERNAL_H
#define Py_TOKENIZER_READER_INTERNAL_H

#شامل "Python.h"

#define PYTOK_ENCODING_COOKIE_MAX_سطرها 2

تعریفنوع شمارش {
    _PYTOK_READER_PREPARED,
    _PYTOK_READER_FILE,
    _PYTOK_READER_READLINE,
    _PYTOK_READER_INTERACTIVE,
} _PyTok_ReaderKind;

تعریفنوع شمارش {
    PYTOK_READ_سطر,
    _PYTOK_READ_EOF,
    _PYTOK_READ_STOPPED,
    _PYTOK_READ_INTERRUPT,
    _PYTOK_READ_ERROR,
} _PyTok_ReadResult;

تعریفنوع شمارش {
    _PYTOK_ENCODING_ERROR = -1,
    _PYTOK_ENCODING_DONE,
    PYTOK_ENCODING_NEED_SECOND_سطر,
} _PyTok_EncodingResult;

تعریفنوع شمارش {
    _PYTOK_CHUNK_BORROWED,
    _PYTOK_CHUNK_PYMEM,
    _PYTOK_CHUNK_PYOBJECT,
} _PyTok_ChunkOwnership;

تعریفنوع ساختار {
    نویسه *data;
    PyObject *owner;
    Py_ssize_t len;
    _PyTok_ChunkOwnership ownership;
    unsigned نویسه implicit_newline;
} _PyTok_Chunk;

تعریفنوع ساختار _PyTok_Reader {
    FILE *fp;  // Borrowed input stream; NULL for string and readline input.
    نویسه *encoding;  // Owned source encoding.
    PyObject *readline;
    PyObject *decoder;
    ثابت نویسه *prompt;
    ثابت نویسه *nextprompt;

    نویسه *file_buffer;
    Py_ssize_t file_buffer_cap;
    _PyTok_Chunk prefetched_سطرها[PYTOK_ENCODING_COOKIE_MAX_سطرها];

    نویسه *decoded;
    Py_ssize_t decoded_pos;
    Py_ssize_t decoded_scan;  // First byte not yet scanned for a newline.
    Py_ssize_t decoded_len;
    Py_ssize_t decoded_cap;
    _PyTok_ReaderKind kind;
    unsigned نویسه prepared_final_newline;  // _PyTokenizer_Newline
    unsigned نویسه decoded_tail_is_implicit;
    unsigned نویسه file_initialized;
    unsigned نویسه file_eof;
    unsigned نویسه decoder_finalized;
    unsigned نویسه stop_interactive;
} _PyTok_Reader;

ساختار tok_state;

// On failure, the buffer and capacity are unchanged.
صحیح _PyTok_ReserveBuffer(نویسه **, Py_ssize_t *, Py_ssize_t, Py_ssize_t);
نویسه *_PyTok_CopyBytes(ثابت نویسه *, Py_ssize_t);
صحیح _PyTok_DecodeOnce(
    ساختار tok_state *, _PyTok_Chunk *, ثابت نویسه *, ثابت نویسه *);
نویسه *_PyTok_NormalizeNewlines(ثابت نویسه *, Py_ssize_t, Py_ssize_t *);
تهی _PyTok_ChunkClear(_PyTok_Chunk *);
صحیح _PyTok_SetEncoding(ساختار tok_state *, ثابت نویسه *);
_PyTok_EncodingResult _PyTok_DetectEncoding(
    ساختار tok_state *, ثابت _PyTok_Chunk *, ثابت _PyTok_Chunk *, صحیح,
    Py_ssize_t *);
صحیح _PyTok_PrepareString(ساختار tok_state *, ثابت نویسه *, صحیح, صحیح, صحیح);
صحیح _PyTok_StartDecoder(ساختار tok_state *, ثابت نویسه *);
صحیح _PyTok_DecodeChunk(ساختار tok_state *, _PyTok_Chunk *, صحیح);
صحیح Py_Tok_Decoder_Has_Buffered_درون_داد(ساختار tok_state *);

#endif
