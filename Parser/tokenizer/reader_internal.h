#ifndef Py_TOKENIZER_READER_INTERNAL_H
#define Py_TOKENIZER_READER_INTERNAL_H

#অন্তর্ভাব "Python.h"

#define PYTOK_ENCODING_COOKIE_MAX_সারিগুলি 2

প্রকারলেখ গণনা {
    _PYTOK_READER_PREPARED,
    _PYTOK_READER_FILE,
    _PYTOK_READER_READLINE,
    _PYTOK_READER_INTERACTIVE,
} _PyTok_ReaderKind;

প্রকারলেখ গণনা {
    PYTOK_READ_সারি,
    _PYTOK_READ_EOF,
    _PYTOK_READ_STOPPED,
    _PYTOK_READ_INTERRUPT,
    _PYTOK_READ_ERROR,
} Py_Tok_Read_ফলাফল;

প্রকারলেখ গণনা {
    _PYTOK_ENCODING_ERROR = -1,
    _PYTOK_ENCODING_DONE,
    PYTOK_ENCODING_NEED_SECOND_সারি,
} Py_Tok_Encoding_ফলাফল;

প্রকারলেখ গণনা {
    _PYTOK_CHUNK_BORROWED,
    _PYTOK_CHUNK_PYMEM,
    _PYTOK_CHUNK_PYOBJECT,
} _PyTok_ChunkOwnership;

প্রকারলেখ কাঠামো {
    অক্ষর *তথ্য;
    PyObject *owner;
    Py_ssize_t len;
    _PyTok_ChunkOwnership ownership;
    unsigned অক্ষর implicit_newline;
} _PyTok_Chunk;

প্রকারলেখ কাঠামো _PyTok_Reader {
    FILE *fp;  // Borrowed input stream; NULL for string and readline input.
    অক্ষর *encoding;  // Owned source encoding.
    PyObject *readline;
    PyObject *decoder;
    ধ্রুবক অক্ষর *prompt;
    ধ্রুবক অক্ষর *nextprompt;

    অক্ষর *file_বাফার;
    Py_ssize_t file_বাফার_cap;
    _PyTok_Chunk prefetched_সারিগুলি[PYTOK_ENCODING_COOKIE_MAX_সারিগুলি];

    অক্ষর *decoded;
    Py_ssize_t decoded_pos;
    Py_ssize_t decoded_scan;  // First byte not yet scanned for a newline.
    Py_ssize_t decoded_len;
    Py_ssize_t decoded_cap;
    _PyTok_ReaderKind kind;
    unsigned অক্ষর prepared_final_newline;  // _PyTokenizer_Newline
    unsigned অক্ষর decoded_লেজ_is_implicit;
    unsigned অক্ষর file_initialized;
    unsigned অক্ষর file_eof;
    unsigned অক্ষর decoder_finalized;
    unsigned অক্ষর stop_interactive;
} _PyTok_Reader;

কাঠামো tok_state;

// On failure, the buffer and capacity are unchanged.
পূর্ণসংখ্যা Py_Tok_Reserve_বাফার(অক্ষর **, Py_ssize_t *, Py_ssize_t, Py_ssize_t);
অক্ষর *_PyTok_CopyBytes(ধ্রুবক অক্ষর *, Py_ssize_t);
পূর্ণসংখ্যা _PyTok_DecodeOnce(
    কাঠামো tok_state *, _PyTok_Chunk *, ধ্রুবক অক্ষর *, ধ্রুবক অক্ষর *);
অক্ষর *_PyTok_NormalizeNewlines(ধ্রুবক অক্ষর *, Py_ssize_t, Py_ssize_t *);
শূন্য _PyTok_ChunkClear(_PyTok_Chunk *);
পূর্ণসংখ্যা _PyTok_SetEncoding(কাঠামো tok_state *, ধ্রুবক অক্ষর *);
Py_Tok_Encoding_ফলাফল _PyTok_DetectEncoding(
    কাঠামো tok_state *, ধ্রুবক _PyTok_Chunk *, ধ্রুবক _PyTok_Chunk *, পূর্ণসংখ্যা,
    Py_ssize_t *);
পূর্ণসংখ্যা _PyTok_PrepareString(কাঠামো tok_state *, ধ্রুবক অক্ষর *, পূর্ণসংখ্যা, পূর্ণসংখ্যা, পূর্ণসংখ্যা);
পূর্ণসংখ্যা _PyTok_StartDecoder(কাঠামো tok_state *, ধ্রুবক অক্ষর *);
পূর্ণসংখ্যা _PyTok_DecodeChunk(কাঠামো tok_state *, _PyTok_Chunk *, পূর্ণসংখ্যা);
পূর্ণসংখ্যা Py_Tok_Decoder_Has_Buffered_প্রবেশ(কাঠামো tok_state *);

#endif
