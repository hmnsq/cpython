#ifndef Py_TOKENIZER_READER_INTERNAL_H
#define Py_TOKENIZER_READER_INTERNAL_H

#ಅನ್ತರ್ಭಾವ "Python.h"

#define PYTOK_ENCODING_COOKIE_MAX_ಸಾಲುಗಳು 2

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
    _PYTOK_READER_PREPARED,
    _PYTOK_READER_FILE,
    _PYTOK_READER_READLINE,
    _PYTOK_READER_INTERACTIVE,
} _PyTok_ReaderKind;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
    PYTOK_READ_ಸಾಲು,
    _PYTOK_READ_EOF,
    _PYTOK_READ_STOPPED,
    _PYTOK_READ_INTERRUPT,
    _PYTOK_READ_ERROR,
} _PyTok_ReadResult;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
    _PYTOK_ENCODING_ERROR = -1,
    _PYTOK_ENCODING_DONE,
    PYTOK_ENCODING_NEED_SECOND_ಸಾಲು,
} _PyTok_EncodingResult;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
    _PYTOK_CHUNK_BORROWED,
    _PYTOK_CHUNK_PYMEM,
    _PYTOK_CHUNK_PYOBJECT,
} _PyTok_ChunkOwnership;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ವರ್ಣ *data;
    PyObject *owner;
    Py_ssize_t len;
    _PyTok_ChunkOwnership ownership;
    unsigned ವರ್ಣ implicit_newline;
} _PyTok_Chunk;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ _PyTok_Reader {
    FILE *fp;  // Borrowed input stream; NULL for string and readline input.
    ವರ್ಣ *encoding;  // Owned source encoding.
    PyObject *readline;
    PyObject *decoder;
    ಧ್ರುವ ವರ್ಣ *prompt;
    ಧ್ರುವ ವರ್ಣ *nextprompt;

    ವರ್ಣ *file_buffer;
    Py_ssize_t file_buffer_cap;
    _PyTok_Chunk prefetched_ಸಾಲುಗಳು[PYTOK_ENCODING_COOKIE_MAX_ಸಾಲುಗಳು];

    ವರ್ಣ *decoded;
    Py_ssize_t decoded_pos;
    Py_ssize_t decoded_scan;  // First byte not yet scanned for a newline.
    Py_ssize_t decoded_len;
    Py_ssize_t decoded_cap;
    _PyTok_ReaderKind kind;
    unsigned ವರ್ಣ prepared_final_newline;  // _PyTokenizer_Newline
    unsigned ವರ್ಣ decoded_tail_is_implicit;
    unsigned ವರ್ಣ file_initialized;
    unsigned ವರ್ಣ file_eof;
    unsigned ವರ್ಣ decoder_finalized;
    unsigned ವರ್ಣ stop_interactive;
} _PyTok_Reader;

ಸಂರಚನಾ tok_state;

// On failure, the buffer and capacity are unchanged.
ಪೂರ್ಣಾಙ್ಕ _PyTok_ReserveBuffer(ವರ್ಣ **, Py_ssize_t *, Py_ssize_t, Py_ssize_t);
ವರ್ಣ *_PyTok_CopyBytes(ಧ್ರುವ ವರ್ಣ *, Py_ssize_t);
ಪೂರ್ಣಾಙ್ಕ _PyTok_DecodeOnce(
    ಸಂರಚನಾ tok_state *, _PyTok_Chunk *, ಧ್ರುವ ವರ್ಣ *, ಧ್ರುವ ವರ್ಣ *);
ವರ್ಣ *_PyTok_NormalizeNewlines(ಧ್ರುವ ವರ್ಣ *, Py_ssize_t, Py_ssize_t *);
ಶೂನ್ಯ _PyTok_ChunkClear(_PyTok_Chunk *);
ಪೂರ್ಣಾಙ್ಕ _PyTok_SetEncoding(ಸಂರಚನಾ tok_state *, ಧ್ರುವ ವರ್ಣ *);
_PyTok_EncodingResult _PyTok_DetectEncoding(
    ಸಂರಚನಾ tok_state *, ಧ್ರುವ _PyTok_Chunk *, ಧ್ರುವ _PyTok_Chunk *, ಪೂರ್ಣಾಙ್ಕ,
    Py_ssize_t *);
ಪೂರ್ಣಾಙ್ಕ _PyTok_PrepareString(ಸಂರಚನಾ tok_state *, ಧ್ರುವ ವರ್ಣ *, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ);
ಪೂರ್ಣಾಙ್ಕ _PyTok_StartDecoder(ಸಂರಚನಾ tok_state *, ಧ್ರುವ ವರ್ಣ *);
ಪೂರ್ಣಾಙ್ಕ _PyTok_DecodeChunk(ಸಂರಚನಾ tok_state *, _PyTok_Chunk *, ಪೂರ್ಣಾಙ್ಕ);
ಪೂರ್ಣಾಙ್ಕ Py_Tok_Decoder_Has_Buffered_ಒಳಹರಿವು(ಸಂರಚನಾ tok_state *);

#endif
