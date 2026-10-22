#ifndef Py_TOKENIZER_READER_INTERNAL_H
#define Py_TOKENIZER_READER_INTERNAL_H

#অন্তর্ভাৱ "Python.h"

#define PYTOK_ENCODING_COOKIE_MAX_সাৰিগুলি 2

প্রকারনির্দেশ গণনা {
    _PYTOK_READER_PREPARED,
    _PYTOK_READER_FILE,
    _PYTOK_READER_READLINE,
    _PYTOK_READER_INTERACTIVE,
} _PyTok_ReaderKind;

প্রকারনির্দেশ গণনা {
    PYTOK_READ_সাৰি,
    _PYTOK_READ_EOF,
    _PYTOK_READ_STOPPED,
    _PYTOK_READ_INTERRUPT,
    _PYTOK_READ_ERROR,
} _PyTok_ReadResult;

প্রকারনির্দেশ গণনা {
    _PYTOK_ENCODING_ERROR = -1,
    _PYTOK_ENCODING_DONE,
    PYTOK_ENCODING_NEED_SECOND_সাৰি,
} _PyTok_EncodingResult;

প্রকারনির্দেশ গণনা {
    _PYTOK_CHUNK_BORROWED,
    _PYTOK_CHUNK_PYMEM,
    _PYTOK_CHUNK_PYOBJECT,
} _PyTok_ChunkOwnership;

প্রকারনির্দেশ সংরচনা {
    ৱর্ণ *data;
    PyObject *owner;
    Py_ssize_t len;
    _PyTok_ChunkOwnership ownership;
    unsigned ৱর্ণ implicit_newline;
} _PyTok_Chunk;

প্রকারনির্দেশ সংরচনা _PyTok_Reader {
    FILE *fp;  // Borrowed input stream; NULL for string and readline input.
    ৱর্ণ *encoding;  // Owned source encoding.
    PyObject *readline;
    PyObject *decoder;
    ধ্রুৱ ৱর্ণ *prompt;
    ধ্রুৱ ৱর্ণ *nextprompt;

    ৱর্ণ *file_buffer;
    Py_ssize_t file_buffer_cap;
    _PyTok_Chunk prefetched_সাৰিগুলি[PYTOK_ENCODING_COOKIE_MAX_সাৰিগুলি];

    ৱর্ণ *decoded;
    Py_ssize_t decoded_pos;
    Py_ssize_t decoded_scan;  // First byte not yet scanned for a newline.
    Py_ssize_t decoded_len;
    Py_ssize_t decoded_cap;
    _PyTok_ReaderKind kind;
    unsigned ৱর্ণ prepared_final_newline;  // _PyTokenizer_Newline
    unsigned ৱর্ণ decoded_tail_is_implicit;
    unsigned ৱর্ণ file_initialized;
    unsigned ৱর্ণ file_eof;
    unsigned ৱর্ণ decoder_finalized;
    unsigned ৱর্ণ stop_interactive;
} _PyTok_Reader;

সংরচনা tok_state;

// On failure, the buffer and capacity are unchanged.
পূর্ণাঙ্ক _PyTok_ReserveBuffer(ৱর্ণ **, Py_ssize_t *, Py_ssize_t, Py_ssize_t);
ৱর্ণ *_PyTok_CopyBytes(ধ্রুৱ ৱর্ণ *, Py_ssize_t);
পূর্ণাঙ্ক _PyTok_DecodeOnce(
    সংরচনা tok_state *, _PyTok_Chunk *, ধ্রুৱ ৱর্ণ *, ধ্রুৱ ৱর্ণ *);
ৱর্ণ *_PyTok_NormalizeNewlines(ধ্রুৱ ৱর্ণ *, Py_ssize_t, Py_ssize_t *);
শূন্য _PyTok_ChunkClear(_PyTok_Chunk *);
পূর্ণাঙ্ক _PyTok_SetEncoding(সংরচনা tok_state *, ধ্রুৱ ৱর্ণ *);
_PyTok_EncodingResult _PyTok_DetectEncoding(
    সংরচনা tok_state *, ধ্রুৱ _PyTok_Chunk *, ধ্রুৱ _PyTok_Chunk *, পূর্ণাঙ্ক,
    Py_ssize_t *);
পূর্ণাঙ্ক _PyTok_PrepareString(সংরচনা tok_state *, ধ্রুৱ ৱর্ণ *, পূর্ণাঙ্ক, পূর্ণাঙ্ক, পূর্ণাঙ্ক);
পূর্ণাঙ্ক _PyTok_StartDecoder(সংরচনা tok_state *, ধ্রুৱ ৱর্ণ *);
পূর্ণাঙ্ক _PyTok_DecodeChunk(সংরচনা tok_state *, _PyTok_Chunk *, পূর্ণাঙ্ক);
পূর্ণাঙ্ক Py_Tok_Decoder_Has_Buffered_প্ৰবেশ(সংরচনা tok_state *);

#endif
