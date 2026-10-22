#ifndef Py_TOKENIZER_READER_INTERNAL_H
#define Py_TOKENIZER_READER_INTERNAL_H

#അന്തര്ഭാവ "Python.h"

#define PYTOK_ENCODING_COOKIE_MAX_വരികൾ 2

തരനിർവചനം എണ്ണൽ {
    _PYTOK_READER_PREPARED,
    _PYTOK_READER_FILE,
    _PYTOK_READER_READLINE,
    _PYTOK_READER_INTERACTIVE,
} _PyTok_ReaderKind;

തരനിർവചനം എണ്ണൽ {
    PYTOK_READ_വരി,
    _PYTOK_READ_EOF,
    _PYTOK_READ_STOPPED,
    _PYTOK_READ_INTERRUPT,
    _PYTOK_READ_ERROR,
} _PyTok_ReadResult;

തരനിർവചനം എണ്ണൽ {
    _PYTOK_ENCODING_ERROR = -1,
    _PYTOK_ENCODING_DONE,
    PYTOK_ENCODING_NEED_SECOND_വരി,
} _PyTok_EncodingResult;

തരനിർവചനം എണ്ണൽ {
    _PYTOK_CHUNK_BORROWED,
    _PYTOK_CHUNK_PYMEM,
    _PYTOK_CHUNK_PYOBJECT,
} _PyTok_ChunkOwnership;

തരനിർവചനം ഘടന {
    അക്ഷരം *data;
    PyObject *owner;
    Py_ssize_t len;
    _PyTok_ChunkOwnership ownership;
    unsigned അക്ഷരം implicit_newline;
} _PyTok_Chunk;

തരനിർവചനം ഘടന _PyTok_Reader {
    FILE *fp;  // Borrowed input stream; NULL for string and readline input.
    അക്ഷരം *encoding;  // Owned source encoding.
    PyObject *readline;
    PyObject *decoder;
    സ്ഥിരം അക്ഷരം *prompt;
    സ്ഥിരം അക്ഷരം *nextprompt;

    അക്ഷരം *file_buffer;
    Py_ssize_t file_buffer_cap;
    _PyTok_Chunk prefetched_വരികൾ[PYTOK_ENCODING_COOKIE_MAX_വരികൾ];

    അക്ഷരം *decoded;
    Py_ssize_t decoded_pos;
    Py_ssize_t decoded_scan;  // First byte not yet scanned for a newline.
    Py_ssize_t decoded_len;
    Py_ssize_t decoded_cap;
    _PyTok_ReaderKind kind;
    unsigned അക്ഷരം prepared_final_newline;  // _PyTokenizer_Newline
    unsigned അക്ഷരം decoded_tail_is_implicit;
    unsigned അക്ഷരം file_initialized;
    unsigned അക്ഷരം file_eof;
    unsigned അക്ഷരം decoder_finalized;
    unsigned അക്ഷരം stop_interactive;
} _PyTok_Reader;

ഘടന tok_state;

// On failure, the buffer and capacity are unchanged.
പൂർണ്ണസംഖ്യ _PyTok_ReserveBuffer(അക്ഷരം **, Py_ssize_t *, Py_ssize_t, Py_ssize_t);
അക്ഷരം *_PyTok_CopyBytes(സ്ഥിരം അക്ഷരം *, Py_ssize_t);
പൂർണ്ണസംഖ്യ _PyTok_DecodeOnce(
    ഘടന tok_state *, _PyTok_Chunk *, സ്ഥിരം അക്ഷരം *, സ്ഥിരം അക്ഷരം *);
അക്ഷരം *_PyTok_NormalizeNewlines(സ്ഥിരം അക്ഷരം *, Py_ssize_t, Py_ssize_t *);
ശൂന്യം _PyTok_ChunkClear(_PyTok_Chunk *);
പൂർണ്ണസംഖ്യ _PyTok_SetEncoding(ഘടന tok_state *, സ്ഥിരം അക്ഷരം *);
_PyTok_EncodingResult _PyTok_DetectEncoding(
    ഘടന tok_state *, സ്ഥിരം _PyTok_Chunk *, സ്ഥിരം _PyTok_Chunk *, പൂർണ്ണസംഖ്യ,
    Py_ssize_t *);
പൂർണ്ണസംഖ്യ _PyTok_PrepareString(ഘടന tok_state *, സ്ഥിരം അക്ഷരം *, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ);
പൂർണ്ണസംഖ്യ _PyTok_StartDecoder(ഘടന tok_state *, സ്ഥിരം അക്ഷരം *);
പൂർണ്ണസംഖ്യ _PyTok_DecodeChunk(ഘടന tok_state *, _PyTok_Chunk *, പൂർണ്ണസംഖ്യ);
പൂർണ്ണസംഖ്യ Py_Tok_Decoder_Has_Buffered_നിവേശം(ഘടന tok_state *);

#endif
