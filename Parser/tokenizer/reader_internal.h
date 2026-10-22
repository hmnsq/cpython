#ifndef Py_TOKENIZER_READER_INTERNAL_H
#define Py_TOKENIZER_READER_INTERNAL_H

#include "Python.h"

#define _PYTOK_ENCODING_COOKIE_MAX_LINES 2

typedef ga_nanaa {
    _PYTOK_READER_PREPARED,
    _PYTOK_READER_FILE,
    _PYTOK_READER_READLINE,
    _PYTOK_READER_INTERACTIVE,
} _PyTok_ReaderKind;

typedef ga_nanaa {
    _PYTOK_READ_LINE,
    _PYTOK_READ_EOF,
    _PYTOK_READ_STOPPED,
    _PYTOK_READ_INTERRUPT,
    _PYTOK_READ_ERROR,
} _PyTok_ReadResult;

typedef ga_nanaa {
    _PYTOK_ENCODING_ERROR = -1,
    _PYTOK_ENCODING_DONE,
    _PYTOK_ENCODING_NEED_SECOND_LINE,
} _PyTok_EncodingResult;

typedef ga_nanaa {
    _PYTOK_CHUNK_BORROWED,
    _PYTOK_CHUNK_PYMEM,
    _PYTOK_CHUNK_PYOBJECT,
} _PyTok_ChunkOwnership;

typedef sa_nracanaa {
    var_na *data;
    PyObject *owner;
    Py_ssize_t len;
    _PyTok_ChunkOwnership ownership;
    unsigned var_na implicit_newline;
} _PyTok_Chunk;

typedef sa_nracanaa _PyTok_Reader {
    FILE *fp;  // Borrowed input stream; NULL for string and readline input.
    var_na *encoding;  // Owned source encoding.
    PyObject *readline;
    PyObject *decoder;
    const var_na *prompt;
    const var_na *nextprompt;

    var_na *file_buffer;
    Py_ssize_t file_buffer_cap;
    _PyTok_Chunk prefetched_lines[_PYTOK_ENCODING_COOKIE_MAX_LINES];

    var_na *decoded;
    Py_ssize_t decoded_pos;
    Py_ssize_t decoded_scan;  // First byte not yet scanned for a newline.
    Py_ssize_t decoded_len;
    Py_ssize_t decoded_cap;
    _PyTok_ReaderKind kind;
    unsigned var_na prepared_final_newline;  // _PyTokenizer_Newline
    unsigned var_na decoded_tail_is_implicit;
    unsigned var_na file_initialized;
    unsigned var_na file_eof;
    unsigned var_na decoder_finalized;
    unsigned var_na stop_interactive;
} _PyTok_Reader;

sa_nracanaa tok_state;

// On failure, the buffer and capacity are unchanged.
puur_naa_nka _PyTok_ReserveBuffer(var_na **, Py_ssize_t *, Py_ssize_t, Py_ssize_t);
var_na *_PyTok_CopyBytes(const var_na *, Py_ssize_t);
puur_naa_nka _PyTok_DecodeOnce(
    sa_nracanaa tok_state *, _PyTok_Chunk *, const var_na *, const var_na *);
var_na *_PyTok_NormalizeNewlines(const var_na *, Py_ssize_t, Py_ssize_t *);
void _PyTok_ChunkClear(_PyTok_Chunk *);
puur_naa_nka _PyTok_SetEncoding(sa_nracanaa tok_state *, const var_na *);
_PyTok_EncodingResult _PyTok_DetectEncoding(
    sa_nracanaa tok_state *, const _PyTok_Chunk *, const _PyTok_Chunk *, puur_naa_nka,
    Py_ssize_t *);
puur_naa_nka _PyTok_PrepareString(sa_nracanaa tok_state *, const var_na *, puur_naa_nka, puur_naa_nka, puur_naa_nka);
puur_naa_nka _PyTok_StartDecoder(sa_nracanaa tok_state *, const var_na *);
puur_naa_nka _PyTok_DecodeChunk(sa_nracanaa tok_state *, _PyTok_Chunk *, puur_naa_nka);
puur_naa_nka _PyTok_DecoderHasBufferedInput(sa_nracanaa tok_state *);

#endif
