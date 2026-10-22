#ifndef Py_TOKENIZER_READER_INTERNAL_H
#define Py_TOKENIZER_READER_INTERNAL_H

#अन्तर्भाव "Python.h"

#define PYTOK_ENCODING_COOKIE_MAX_ओळी 2

प्रकारव्याख्या गणना {
    _PYTOK_READER_PREPARED,
    _PYTOK_READER_FILE,
    _PYTOK_READER_READLINE,
    _PYTOK_READER_INTERACTIVE,
} _PyTok_ReaderKind;

प्रकारव्याख्या गणना {
    PYTOK_READ_ओळ,
    _PYTOK_READ_EOF,
    _PYTOK_READ_STOPPED,
    _PYTOK_READ_INTERRUPT,
    _PYTOK_READ_ERROR,
} _PyTok_ReadResult;

प्रकारव्याख्या गणना {
    _PYTOK_ENCODING_ERROR = -1,
    _PYTOK_ENCODING_DONE,
    PYTOK_ENCODING_NEED_SECOND_ओळ,
} _PyTok_EncodingResult;

प्रकारव्याख्या गणना {
    _PYTOK_CHUNK_BORROWED,
    _PYTOK_CHUNK_PYMEM,
    _PYTOK_CHUNK_PYOBJECT,
} _PyTok_ChunkOwnership;

प्रकारव्याख्या संरचना {
    अक्षर *data;
    PyObject *owner;
    Py_ssize_t len;
    _PyTok_ChunkOwnership ownership;
    अचिन्हित अक्षर implicit_newline;
} _PyTok_Chunk;

प्रकारव्याख्या संरचना _PyTok_Reader {
    खाता *fp;  // Borrowed input stream; NULL for string and readline input.
    अक्षर *encoding;  // Owned source encoding.
    PyObject *readline;
    PyObject *decoder;
    स्थिरांक अक्षर *prompt;
    स्थिरांक अक्षर *nextprompt;

    अक्षर *file_buffer;
    Py_ssize_t file_buffer_cap;
    _PyTok_Chunk prefetched_ओळी[PYTOK_ENCODING_COOKIE_MAX_ओळी];

    अक्षर *decoded;
    Py_ssize_t decoded_pos;
    Py_ssize_t decoded_scan;  // First byte not yet scanned for a newline.
    Py_ssize_t decoded_len;
    Py_ssize_t decoded_cap;
    _PyTok_ReaderKind kind;
    अचिन्हित अक्षर prepared_final_newline;  // _PyTokenizer_Newline
    अचिन्हित अक्षर decoded_tail_is_implicit;
    अचिन्हित अक्षर file_initialized;
    अचिन्हित अक्षर file_eof;
    अचिन्हित अक्षर decoder_finalized;
    अचिन्हित अक्षर stop_interactive;
} _PyTok_Reader;

संरचना tok_state;

// On failure, the buffer and capacity are unchanged.
पूर्णांक _PyTok_ReserveBuffer(अक्षर **, Py_ssize_t *, Py_ssize_t, Py_ssize_t);
अक्षर *_PyTok_CopyBytes(स्थिरांक अक्षर *, Py_ssize_t);
पूर्णांक _PyTok_DecodeOnce(
    संरचना tok_state *, _PyTok_Chunk *, स्थिरांक अक्षर *, स्थिरांक अक्षर *);
अक्षर *_PyTok_NormalizeNewlines(स्थिरांक अक्षर *, Py_ssize_t, Py_ssize_t *);
शून्य _PyTok_ChunkClear(_PyTok_Chunk *);
पूर्णांक _PyTok_SetEncoding(संरचना tok_state *, स्थिरांक अक्षर *);
_PyTok_EncodingResult _PyTok_DetectEncoding(
    संरचना tok_state *, स्थिरांक _PyTok_Chunk *, स्थिरांक _PyTok_Chunk *, पूर्णांक,
    Py_ssize_t *);
पूर्णांक _PyTok_PrepareString(संरचना tok_state *, स्थिरांक अक्षर *, पूर्णांक, पूर्णांक, पूर्णांक);
पूर्णांक _PyTok_StartDecoder(संरचना tok_state *, स्थिरांक अक्षर *);
पूर्णांक _PyTok_DecodeChunk(संरचना tok_state *, _PyTok_Chunk *, पूर्णांक);
पूर्णांक Py_Tok_Decoder_Has_Buffered_आदान(संरचना tok_state *);

#endif
