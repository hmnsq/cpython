#यदि_अपरिभाषितम् Py_TOKENIZER_READER_आन्तरिकम्_H
#परिभाषय Py_TOKENIZER_READER_आन्तरिकम्_H

#अन्तर्भाव "Python.h"

#परिभाषय PYTOK_ENCODING_COOKIE_MAX_पङ्क्तयः 2

प्रकारनिर्देश गणना {
    _PYTOK_READER_PREPARED,
    _PYTOK_READER_FILE,
    _PYTOK_READER_READLINE,
    _PYTOK_READER_INTERACTIVE,
} Py_Tok_Reader_विधा;

प्रकारनिर्देश गणना {
    PYTOK_READ_पङ्क्तिः,
    _PYTOK_READ_EOF,
    _PYTOK_READ_STOPPED,
    PYTOK_READ_व्यवधानम्,
    _PYTOK_READ_ERROR,
} Py_Tok_Read_परिणाम;

प्रकारनिर्देश गणना {
    _PYTOK_ENCODING_ERROR = -1,
    PYTOK_ENCODING_समाप्तम्,
    PYTOK_ENCODING_NEED_SECOND_पङ्क्तिः,
} Py_Tok_Encoding_परिणाम;

प्रकारनिर्देश गणना {
    _PYTOK_CHUNK_BORROWED,
    _PYTOK_CHUNK_PYMEM,
    _PYTOK_CHUNK_PYOBJECT,
} _PyTok_ChunkOwnership;

प्रकारनिर्देश संरचना {
    वर्ण *दत्त;
    PyObject *owner;
    Py_ssize_t len;
    _PyTok_ChunkOwnership ownership;
    अचिह्नित वर्ण implicit_newline;
} _PyTok_Chunk;

प्रकारनिर्देश संरचना _PyTok_Reader {
    FILE *fp;  // Borrowed input stream; NULL for string and readline input.
    वर्ण *encoding;  // Owned source encoding.
    PyObject *readline;
    PyObject *decoder;
    ध्रुव वर्ण *prompt;
    ध्रुव वर्ण *nextprompt;

    वर्ण *file_बफर;
    Py_ssize_t file_बफर_cap;
    _PyTok_Chunk prefetched_पङ्क्तयः[PYTOK_ENCODING_COOKIE_MAX_पङ्क्तयः];

    वर्ण *decoded;
    Py_ssize_t decoded_pos;
    Py_ssize_t decoded_अवलोकनम्;  // First byte not yet scanned for a newline.
    Py_ssize_t decoded_len;
    Py_ssize_t decoded_cap;
    Py_Tok_Reader_विधा विधा;
    अचिह्नित वर्ण prepared_final_newline;  // _PyTokenizer_Newline
    अचिह्नित वर्ण decoded_पुच्छ_is_implicit;
    अचिह्नित वर्ण file_initialized;
    अचिह्नित वर्ण file_eof;
    अचिह्नित वर्ण decoder_finalized;
    अचिह्नित वर्ण stop_interactive;
} _PyTok_Reader;

संरचना tok_state;

// On failure, the buffer and capacity are unchanged.
पूर्णाङ्क Py_Tok_Reserve_बफर(वर्ण **, Py_ssize_t *, Py_ssize_t, Py_ssize_t);
वर्ण *Py_Tok_Copy_अष्टकानि(ध्रुव वर्ण *, Py_ssize_t);
पूर्णाङ्क _PyTok_DecodeOnce(
    संरचना tok_state *, _PyTok_Chunk *, ध्रुव वर्ण *, ध्रुव वर्ण *);
वर्ण *_PyTok_NormalizeNewlines(ध्रुव वर्ण *, Py_ssize_t, Py_ssize_t *);
शून्य _PyTok_ChunkClear(_PyTok_Chunk *);
पूर्णाङ्क Py_Tok_समुच्चयः_Encoding(संरचना tok_state *, ध्रुव वर्ण *);
Py_Tok_Encoding_परिणाम _PyTok_DetectEncoding(
    संरचना tok_state *, ध्रुव _PyTok_Chunk *, ध्रुव _PyTok_Chunk *, पूर्णाङ्क,
    Py_ssize_t *);
पूर्णाङ्क _PyTok_PrepareString(संरचना tok_state *, ध्रुव वर्ण *, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क);
पूर्णाङ्क _PyTok_StartDecoder(संरचना tok_state *, ध्रुव वर्ण *);
पूर्णाङ्क _PyTok_DecodeChunk(संरचना tok_state *, _PyTok_Chunk *, पूर्णाङ्क);
पूर्णाङ्क Py_Tok_Decoder_Has_Buffered_निवेशः(संरचना tok_state *);

#यदि_अन्तः
