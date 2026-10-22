#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ Py_TOKENIZER_READER_ਅੰਦਰੂਨੀ_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Py_TOKENIZER_READER_ਅੰਦਰੂਨੀ_H

#ਅਨ੍ਤਰ੍ਭਾਵ "Python.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PYTOK_ENCODING_COOKIE_MAX_ਸਤਰਾਂ 2

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    _PYTOK_READER_PREPARED,
    _PYTOK_READER_FILE,
    _PYTOK_READER_READLINE,
    _PYTOK_READER_INTERACTIVE,
} Py_Tok_Reader_ਕਿਸਮ;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    PYTOK_READ_ਸਤਰ,
    _PYTOK_READ_EOF,
    _PYTOK_READ_STOPPED,
    PYTOK_READ_ਰੁਕਾਵਟ,
    _PYTOK_READ_ERROR,
} Py_Tok_Read_ਨਤੀਜਾ;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    _PYTOK_ENCODING_ERROR = -1,
    PYTOK_ENCODING_ਪੂਰਾ,
    PYTOK_ENCODING_NEED_ਦੂਜਾ_ਸਤਰ,
} Py_Tok_Encoding_ਨਤੀਜਾ;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    _PYTOK_CHUNK_BORROWED,
    _PYTOK_CHUNK_PYMEM,
    _PYTOK_CHUNK_PYOBJECT,
} _PyTok_ChunkOwnership;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਅੱਖਰ *ਅੰਕੜੇ;
    PyObject *owner;
    Py_ssize_t len;
    _PyTok_ChunkOwnership ownership;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ implicit_newline;
} _PyTok_Chunk;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ _PyTok_Reader {
    FILE *fp;  // Borrowed input stream; NULL for string and readline input.
    ਅੱਖਰ *encoding;  // Owned source encoding.
    PyObject *readline;
    PyObject *decoder;
    ਅਚਲ ਅੱਖਰ *prompt;
    ਅਚਲ ਅੱਖਰ *nextprompt;

    ਅੱਖਰ *file_ਬਫਰ;
    Py_ssize_t file_ਬਫਰ_cap;
    _PyTok_Chunk prefetched_ਸਤਰਾਂ[PYTOK_ENCODING_COOKIE_MAX_ਸਤਰਾਂ];

    ਅੱਖਰ *decoded;
    Py_ssize_t decoded_pos;
    Py_ssize_t decoded_ਜਾਂਚ;  // First byte not yet scanned for a newline.
    Py_ssize_t decoded_len;
    Py_ssize_t decoded_cap;
    Py_Tok_Reader_ਕਿਸਮ ਕਿਸਮ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ prepared_final_newline;  // _PyTokenizer_Newline
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ decoded_ਪੂਛ_is_implicit;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ file_initialized;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ file_eof;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ decoder_finalized;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ stop_interactive;
} _PyTok_Reader;

ਬਣਤਰ tok_state;

// On failure, the buffer and capacity are unchanged.
ਪੂਰਨ Py_Tok_Reserve_ਬਫਰ(ਅੱਖਰ **, Py_ssize_t *, Py_ssize_t, Py_ssize_t);
ਅੱਖਰ *Py_Tok_Copy_ਬਾਈਟਾਂ(ਅਚਲ ਅੱਖਰ *, Py_ssize_t);
ਪੂਰਨ _PyTok_DecodeOnce(
    ਬਣਤਰ tok_state *, _PyTok_Chunk *, ਅਚਲ ਅੱਖਰ *, ਅਚਲ ਅੱਖਰ *);
ਅੱਖਰ *_PyTok_NormalizeNewlines(ਅਚਲ ਅੱਖਰ *, Py_ssize_t, Py_ssize_t *);
ਖਾਲੀ _PyTok_ChunkClear(_PyTok_Chunk *);
ਪੂਰਨ Py_Tok_ਸੈੱਟ_Encoding(ਬਣਤਰ tok_state *, ਅਚਲ ਅੱਖਰ *);
Py_Tok_Encoding_ਨਤੀਜਾ _PyTok_DetectEncoding(
    ਬਣਤਰ tok_state *, ਅਚਲ _PyTok_Chunk *, ਅਚਲ _PyTok_Chunk *, ਪੂਰਨ,
    Py_ssize_t *);
ਪੂਰਨ _PyTok_PrepareString(ਬਣਤਰ tok_state *, ਅਚਲ ਅੱਖਰ *, ਪੂਰਨ, ਪੂਰਨ, ਪੂਰਨ);
ਪੂਰਨ Py_Tok_ਸ਼ੁਰੂ_Decoder(ਬਣਤਰ tok_state *, ਅਚਲ ਅੱਖਰ *);
ਪੂਰਨ _PyTok_DecodeChunk(ਬਣਤਰ tok_state *, _PyTok_Chunk *, ਪੂਰਨ);
ਪੂਰਨ Py_Tok_Decoder_Has_Buffered_ਨਿਵੇਸ਼(ਬਣਤਰ tok_state *);

#ਜੇ_ਅੰਤ
