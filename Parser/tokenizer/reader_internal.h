#ifndef Py_TOKENIZER_READER_INTERNAL_H
#define Py_TOKENIZER_READER_INTERNAL_H

#ଅନ୍ତର୍ଭାଵ "Python.h"

#define PYTOK_ENCODING_COOKIE_MAX_ଧାଡ଼ିଗୁଡ଼ିକ 2

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
    _PYTOK_READER_PREPARED,
    _PYTOK_READER_FILE,
    _PYTOK_READER_READLINE,
    _PYTOK_READER_INTERACTIVE,
} _PyTok_ReaderKind;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
    PYTOK_READ_ଧାଡ଼ି,
    _PYTOK_READ_EOF,
    _PYTOK_READ_STOPPED,
    _PYTOK_READ_INTERRUPT,
    _PYTOK_READ_ERROR,
} _PyTok_ReadResult;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
    _PYTOK_ENCODING_ERROR = -1,
    _PYTOK_ENCODING_DONE,
    PYTOK_ENCODING_NEED_SECOND_ଧାଡ଼ି,
} _PyTok_EncodingResult;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
    _PYTOK_CHUNK_BORROWED,
    _PYTOK_CHUNK_PYMEM,
    _PYTOK_CHUNK_PYOBJECT,
} _PyTok_ChunkOwnership;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ଅକ୍ଷର *data;
    PyObject *owner;
    Py_ssize_t len;
    _PyTok_ChunkOwnership ownership;
    unsigned ଅକ୍ଷର implicit_newline;
} _PyTok_Chunk;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା _PyTok_Reader {
    FILE *fp;  // Borrowed input stream; NULL for string and readline input.
    ଅକ୍ଷର *encoding;  // Owned source encoding.
    PyObject *readline;
    PyObject *decoder;
    ଅଚଳ ଅକ୍ଷର *prompt;
    ଅଚଳ ଅକ୍ଷର *nextprompt;

    ଅକ୍ଷର *file_buffer;
    Py_ssize_t file_buffer_cap;
    _PyTok_Chunk prefetched_ଧାଡ଼ିଗୁଡ଼ିକ[PYTOK_ENCODING_COOKIE_MAX_ଧାଡ଼ିଗୁଡ଼ିକ];

    ଅକ୍ଷର *decoded;
    Py_ssize_t decoded_pos;
    Py_ssize_t decoded_scan;  // First byte not yet scanned for a newline.
    Py_ssize_t decoded_len;
    Py_ssize_t decoded_cap;
    _PyTok_ReaderKind kind;
    unsigned ଅକ୍ଷର prepared_final_newline;  // _PyTokenizer_Newline
    unsigned ଅକ୍ଷର decoded_tail_is_implicit;
    unsigned ଅକ୍ଷର file_initialized;
    unsigned ଅକ୍ଷର file_eof;
    unsigned ଅକ୍ଷର decoder_finalized;
    unsigned ଅକ୍ଷର stop_interactive;
} _PyTok_Reader;

ସଂରଚନା tok_state;

// On failure, the buffer and capacity are unchanged.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyTok_ReserveBuffer(ଅକ୍ଷର **, Py_ssize_t *, Py_ssize_t, Py_ssize_t);
ଅକ୍ଷର *_PyTok_CopyBytes(ଅଚଳ ଅକ୍ଷର *, Py_ssize_t);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyTok_DecodeOnce(
    ସଂରଚନା tok_state *, _PyTok_Chunk *, ଅଚଳ ଅକ୍ଷର *, ଅଚଳ ଅକ୍ଷର *);
ଅକ୍ଷର *_PyTok_NormalizeNewlines(ଅଚଳ ଅକ୍ଷର *, Py_ssize_t, Py_ssize_t *);
ଶୂନ୍ୟ _PyTok_ChunkClear(_PyTok_Chunk *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyTok_SetEncoding(ସଂରଚନା tok_state *, ଅଚଳ ଅକ୍ଷର *);
_PyTok_EncodingResult _PyTok_DetectEncoding(
    ସଂରଚନା tok_state *, ଅଚଳ _PyTok_Chunk *, ଅଚଳ _PyTok_Chunk *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା,
    Py_ssize_t *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyTok_PrepareString(ସଂରଚନା tok_state *, ଅଚଳ ଅକ୍ଷର *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyTok_StartDecoder(ସଂରଚନା tok_state *, ଅଚଳ ଅକ୍ଷର *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyTok_DecodeChunk(ସଂରଚନା tok_state *, _PyTok_Chunk *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Py_Tok_Decoder_Has_Buffered_ନିବେଶ(ସଂରଚନା tok_state *);

#endif
