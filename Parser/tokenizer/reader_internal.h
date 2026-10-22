#ifndef Py_TOKENIZER_READER_INTERNAL_H
#define Py_TOKENIZER_READER_INTERNAL_H

#अन्तर्भाव "Python.h"

#define PYTOK_ENCODING_COOKIE_MAX_पङ्क्तिहरू 2

प्रकारनिर्देश गणना {
    _PYTOK_READER_PREPARED,
    _PYTOK_READER_FILE,
    _PYTOK_READER_READLINE,
    _PYTOK_READER_INTERACTIVE,
} _PyTok_ReaderKind;

प्रकारनिर्देश गणना {
    PYTOK_READ_पङ्क्ति,
    _PYTOK_READ_EOF,
    _PYTOK_READ_STOPPED,
    _PYTOK_READ_INTERRUPT,
    _PYTOK_READ_ERROR,
} _PyTok_ReadResult;

प्रकारनिर्देश गणना {
    _PYTOK_ENCODING_ERROR = -1,
    _PYTOK_ENCODING_DONE,
    PYTOK_ENCODING_NEED_SECOND_पङ्क्ति,
} _PyTok_EncodingResult;

प्रकारनिर्देश गणना {
    _PYTOK_CHUNK_BORROWED,
    _PYTOK_CHUNK_PYMEM,
    _PYTOK_CHUNK_PYOBJECT,
} _PyTok_ChunkOwnership;

प्रकारनिर्देश संरचना {
    वर्ण *data;
    PyObject *owner;
    Py_ssize_t len;
    _PyTok_ChunkOwnership ownership;
    अचिन्हित वर्ण implicit_newline;
} _PyTok_Chunk;

प्रकारनिर्देश संरचना _PyTok_Reader {
    खाता *fp;  // Borrowed input stream; NULL for string and readline input.
    वर्ण *encoding;  // Owned source encoding.
    PyObject *readline;
    PyObject *decoder;
    अचर वर्ण *prompt;
    अचर वर्ण *nextprompt;

    वर्ण *file_buffer;
    Py_ssize_t file_buffer_cap;
    _PyTok_Chunk prefetched_पङ्क्तिहरू[PYTOK_ENCODING_COOKIE_MAX_पङ्क्तिहरू];

    वर्ण *decoded;
    Py_ssize_t decoded_pos;
    Py_ssize_t decoded_scan;  // First byte not yet scanned for a newline.
    Py_ssize_t decoded_len;
    Py_ssize_t decoded_cap;
    _PyTok_ReaderKind kind;
    अचिन्हित वर्ण prepared_final_newline;  // _PyTokenizer_Newline
    अचिन्हित वर्ण decoded_tail_is_implicit;
    अचिन्हित वर्ण file_initialized;
    अचिन्हित वर्ण file_eof;
    अचिन्हित वर्ण decoder_finalized;
    अचिन्हित वर्ण stop_interactive;
} _PyTok_Reader;

संरचना tok_state;

// On failure, the buffer and capacity are unchanged.
पूर्णाङ्क _PyTok_ReserveBuffer(वर्ण **, Py_ssize_t *, Py_ssize_t, Py_ssize_t);
वर्ण *_PyTok_CopyBytes(अचर वर्ण *, Py_ssize_t);
पूर्णाङ्क _PyTok_DecodeOnce(
    संरचना tok_state *, _PyTok_Chunk *, अचर वर्ण *, अचर वर्ण *);
वर्ण *_PyTok_NormalizeNewlines(अचर वर्ण *, Py_ssize_t, Py_ssize_t *);
शून्य _PyTok_ChunkClear(_PyTok_Chunk *);
पूर्णाङ्क _PyTok_SetEncoding(संरचना tok_state *, अचर वर्ण *);
_PyTok_EncodingResult _PyTok_DetectEncoding(
    संरचना tok_state *, अचर _PyTok_Chunk *, अचर _PyTok_Chunk *, पूर्णाङ्क,
    Py_ssize_t *);
पूर्णाङ्क _PyTok_PrepareString(संरचना tok_state *, अचर वर्ण *, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क);
पूर्णाङ्क _PyTok_StartDecoder(संरचना tok_state *, अचर वर्ण *);
पूर्णाङ्क _PyTok_DecodeChunk(संरचना tok_state *, _PyTok_Chunk *, पूर्णाङ्क);
पूर्णाङ्क Py_Tok_Decoder_Has_Buffered_आगत(संरचना tok_state *);

#endif
