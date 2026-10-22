#ifndef Py_TOKENIZER_READER_INTERNAL_H
#define Py_TOKENIZER_READER_INTERNAL_H

#અન્તર્ભાવ "Python.h"

#define PYTOK_ENCODING_COOKIE_MAX_લીટીઓ 2

પ્રકારવ્યાખ્યા ગણના {
    _PYTOK_READER_PREPARED,
    _PYTOK_READER_FILE,
    _PYTOK_READER_READLINE,
    _PYTOK_READER_INTERACTIVE,
} _PyTok_ReaderKind;

પ્રકારવ્યાખ્યા ગણના {
    PYTOK_READ_લીટી,
    _PYTOK_READ_EOF,
    _PYTOK_READ_STOPPED,
    _PYTOK_READ_INTERRUPT,
    _PYTOK_READ_ERROR,
} _PyTok_ReadResult;

પ્રકારવ્યાખ્યા ગણના {
    _PYTOK_ENCODING_ERROR = -1,
    _PYTOK_ENCODING_DONE,
    PYTOK_ENCODING_NEED_SECOND_લીટી,
} _PyTok_EncodingResult;

પ્રકારવ્યાખ્યા ગણના {
    _PYTOK_CHUNK_BORROWED,
    _PYTOK_CHUNK_PYMEM,
    _PYTOK_CHUNK_PYOBJECT,
} _PyTok_ChunkOwnership;

પ્રકારવ્યાખ્યા સંરચના {
    અક્ષર *data;
    PyObject *owner;
    Py_ssize_t len;
    _PyTok_ChunkOwnership ownership;
    unsigned અક્ષર implicit_newline;
} _PyTok_Chunk;

પ્રકારવ્યાખ્યા સંરચના _PyTok_Reader {
    FILE *fp;  // Borrowed input stream; NULL for string and readline input.
    અક્ષર *encoding;  // Owned source encoding.
    PyObject *readline;
    PyObject *decoder;
    અચલ અક્ષર *prompt;
    અચલ અક્ષર *nextprompt;

    અક્ષર *file_buffer;
    Py_ssize_t file_buffer_cap;
    _PyTok_Chunk prefetched_લીટીઓ[PYTOK_ENCODING_COOKIE_MAX_લીટીઓ];

    અક્ષર *decoded;
    Py_ssize_t decoded_pos;
    Py_ssize_t decoded_scan;  // First byte not yet scanned for a newline.
    Py_ssize_t decoded_len;
    Py_ssize_t decoded_cap;
    _PyTok_ReaderKind kind;
    unsigned અક્ષર prepared_final_newline;  // _PyTokenizer_Newline
    unsigned અક્ષર decoded_tail_is_implicit;
    unsigned અક્ષર file_initialized;
    unsigned અક્ષર file_eof;
    unsigned અક્ષર decoder_finalized;
    unsigned અક્ષર stop_interactive;
} _PyTok_Reader;

સંરચના tok_state;

// On failure, the buffer and capacity are unchanged.
પૂર્ણાંક _PyTok_ReserveBuffer(અક્ષર **, Py_ssize_t *, Py_ssize_t, Py_ssize_t);
અક્ષર *_PyTok_CopyBytes(અચલ અક્ષર *, Py_ssize_t);
પૂર્ણાંક _PyTok_DecodeOnce(
    સંરચના tok_state *, _PyTok_Chunk *, અચલ અક્ષર *, અચલ અક્ષર *);
અક્ષર *_PyTok_NormalizeNewlines(અચલ અક્ષર *, Py_ssize_t, Py_ssize_t *);
શૂન્ય _PyTok_ChunkClear(_PyTok_Chunk *);
પૂર્ણાંક _PyTok_SetEncoding(સંરચના tok_state *, અચલ અક્ષર *);
_PyTok_EncodingResult _PyTok_DetectEncoding(
    સંરચના tok_state *, અચલ _PyTok_Chunk *, અચલ _PyTok_Chunk *, પૂર્ણાંક,
    Py_ssize_t *);
પૂર્ણાંક _PyTok_PrepareString(સંરચના tok_state *, અચલ અક્ષર *, પૂર્ણાંક, પૂર્ણાંક, પૂર્ણાંક);
પૂર્ણાંક _PyTok_StartDecoder(સંરચના tok_state *, અચલ અક્ષર *);
પૂર્ણાંક _PyTok_DecodeChunk(સંરચના tok_state *, _PyTok_Chunk *, પૂર્ણાંક);
પૂર્ણાંક Py_Tok_Decoder_Has_Buffered_આગત(સંરચના tok_state *);

#endif
