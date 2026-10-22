#ifndef Py_TOKENIZER_READER_INTERNAL_H
#define Py_TOKENIZER_READER_INTERNAL_H

#ضمن "Python.h"

#define PYTOK_ENCODING_COOKIE_MAX_سطریں 2

تعريفنوع تعداد {
    _PYTOK_READER_PREPARED,
    _PYTOK_READER_FILE,
    _PYTOK_READER_READLINE,
    _PYTOK_READER_INTERACTIVE,
} _PyTok_ReaderKind;

تعريفنوع تعداد {
    PYTOK_READ_سطر,
    _PYTOK_READ_EOF,
    _PYTOK_READ_STOPPED,
    _PYTOK_READ_INTERRUPT,
    _PYTOK_READ_ERROR,
} _PyTok_ReadResult;

تعريفنوع تعداد {
    _PYTOK_ENCODING_ERROR = -1,
    _PYTOK_ENCODING_DONE,
    PYTOK_ENCODING_NEED_SECOND_سطر,
} _PyTok_EncodingResult;

تعريفنوع تعداد {
    _PYTOK_CHUNK_BORROWED,
    _PYTOK_CHUNK_PYMEM,
    _PYTOK_CHUNK_PYOBJECT,
} _PyTok_ChunkOwnership;

تعريفنوع بنية {
    حرف *data;
    PyObject *owner;
    Py_ssize_t len;
    _PyTok_ChunkOwnership ownership;
    unsigned حرف implicit_newline;
} _PyTok_Chunk;

تعريفنوع بنية _PyTok_Reader {
    FILE *fp;  // Borrowed input stream; NULL for string and readline input.
    حرف *encoding;  // Owned source encoding.
    PyObject *readline;
    PyObject *decoder;
    ثابت حرف *prompt;
    ثابت حرف *nextprompt;

    حرف *file_buffer;
    Py_ssize_t file_buffer_cap;
    _PyTok_Chunk prefetched_سطریں[PYTOK_ENCODING_COOKIE_MAX_سطریں];

    حرف *decoded;
    Py_ssize_t decoded_pos;
    Py_ssize_t decoded_scan;  // First byte not yet scanned for a newline.
    Py_ssize_t decoded_len;
    Py_ssize_t decoded_cap;
    _PyTok_ReaderKind kind;
    unsigned حرف prepared_final_newline;  // _PyTokenizer_Newline
    unsigned حرف decoded_tail_is_implicit;
    unsigned حرف file_initialized;
    unsigned حرف file_eof;
    unsigned حرف decoder_finalized;
    unsigned حرف stop_interactive;
} _PyTok_Reader;

بنية tok_state;

// On failure, the buffer and capacity are unchanged.
صحيح _PyTok_ReserveBuffer(حرف **, Py_ssize_t *, Py_ssize_t, Py_ssize_t);
حرف *_PyTok_CopyBytes(ثابت حرف *, Py_ssize_t);
صحيح _PyTok_DecodeOnce(
    بنية tok_state *, _PyTok_Chunk *, ثابت حرف *, ثابت حرف *);
حرف *_PyTok_NormalizeNewlines(ثابت حرف *, Py_ssize_t, Py_ssize_t *);
فراغ _PyTok_ChunkClear(_PyTok_Chunk *);
صحيح _PyTok_SetEncoding(بنية tok_state *, ثابت حرف *);
_PyTok_EncodingResult _PyTok_DetectEncoding(
    بنية tok_state *, ثابت _PyTok_Chunk *, ثابت _PyTok_Chunk *, صحيح,
    Py_ssize_t *);
صحيح _PyTok_PrepareString(بنية tok_state *, ثابت حرف *, صحيح, صحيح, صحيح);
صحيح _PyTok_StartDecoder(بنية tok_state *, ثابت حرف *);
صحيح _PyTok_DecodeChunk(بنية tok_state *, _PyTok_Chunk *, صحيح);
صحيح Py_Tok_Decoder_Has_Buffered_داخلہ(بنية tok_state *);

#endif
