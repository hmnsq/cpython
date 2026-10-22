#ifndef Py_TOKENIZER_READER_INTERNAL_H
#define Py_TOKENIZER_READER_INTERNAL_H

#அந்தர்பாவ "Python.h"

#define PYTOK_ENCODING_COOKIE_MAX_வரிகள் 2

வகைவரை எண்ணிக்கை {
    _PYTOK_READER_PREPARED,
    _PYTOK_READER_FILE,
    _PYTOK_READER_READLINE,
    _PYTOK_READER_INTERACTIVE,
} _PyTok_ReaderKind;

வகைவரை எண்ணிக்கை {
    PYTOK_READ_வரி,
    _PYTOK_READ_EOF,
    _PYTOK_READ_STOPPED,
    _PYTOK_READ_INTERRUPT,
    _PYTOK_READ_ERROR,
} _PyTok_ReadResult;

வகைவரை எண்ணிக்கை {
    _PYTOK_ENCODING_ERROR = -1,
    _PYTOK_ENCODING_DONE,
    PYTOK_ENCODING_NEED_SECOND_வரி,
} _PyTok_EncodingResult;

வகைவரை எண்ணிக்கை {
    _PYTOK_CHUNK_BORROWED,
    _PYTOK_CHUNK_PYMEM,
    _PYTOK_CHUNK_PYOBJECT,
} _PyTok_ChunkOwnership;

வகைவரை கட்டமைப்பு {
    எழுத்து *data;
    PyObject *owner;
    Py_ssize_t len;
    _PyTok_ChunkOwnership ownership;
    unsigned எழுத்து implicit_newline;
} _PyTok_Chunk;

வகைவரை கட்டமைப்பு _PyTok_Reader {
    FILE *fp;  // Borrowed input stream; NULL for string and readline input.
    எழுத்து *encoding;  // Owned source encoding.
    PyObject *readline;
    PyObject *decoder;
    மாறிலி எழுத்து *prompt;
    மாறிலி எழுத்து *nextprompt;

    எழுத்து *file_buffer;
    Py_ssize_t file_buffer_cap;
    _PyTok_Chunk prefetched_வரிகள்[PYTOK_ENCODING_COOKIE_MAX_வரிகள்];

    எழுத்து *decoded;
    Py_ssize_t decoded_pos;
    Py_ssize_t decoded_scan;  // First byte not yet scanned for a newline.
    Py_ssize_t decoded_len;
    Py_ssize_t decoded_cap;
    _PyTok_ReaderKind kind;
    unsigned எழுத்து prepared_final_newline;  // _PyTokenizer_Newline
    unsigned எழுத்து decoded_tail_is_implicit;
    unsigned எழுத்து file_initialized;
    unsigned எழுத்து file_eof;
    unsigned எழுத்து decoder_finalized;
    unsigned எழுத்து stop_interactive;
} _PyTok_Reader;

கட்டமைப்பு tok_state;

// On failure, the buffer and capacity are unchanged.
முழு _PyTok_ReserveBuffer(எழுத்து **, Py_ssize_t *, Py_ssize_t, Py_ssize_t);
எழுத்து *_PyTok_CopyBytes(மாறிலி எழுத்து *, Py_ssize_t);
முழு _PyTok_DecodeOnce(
    கட்டமைப்பு tok_state *, _PyTok_Chunk *, மாறிலி எழுத்து *, மாறிலி எழுத்து *);
எழுத்து *_PyTok_NormalizeNewlines(மாறிலி எழுத்து *, Py_ssize_t, Py_ssize_t *);
வெறுமை _PyTok_ChunkClear(_PyTok_Chunk *);
முழு _PyTok_SetEncoding(கட்டமைப்பு tok_state *, மாறிலி எழுத்து *);
_PyTok_EncodingResult _PyTok_DetectEncoding(
    கட்டமைப்பு tok_state *, மாறிலி _PyTok_Chunk *, மாறிலி _PyTok_Chunk *, முழு,
    Py_ssize_t *);
முழு _PyTok_PrepareString(கட்டமைப்பு tok_state *, மாறிலி எழுத்து *, முழு, முழு, முழு);
முழு _PyTok_StartDecoder(கட்டமைப்பு tok_state *, மாறிலி எழுத்து *);
முழு _PyTok_DecodeChunk(கட்டமைப்பு tok_state *, _PyTok_Chunk *, முழு);
முழு Py_Tok_Decoder_Has_Buffered_உள்ளீடு(கட்டமைப்பு tok_state *);

#endif
