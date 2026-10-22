#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ Py_TOKENIZER_READER_ಆಂತರಿಕ_H
#ವ್ಯಾಖ್ಯಾನಿಸು Py_TOKENIZER_READER_ಆಂತರಿಕ_H

#ಅನ್ತರ್ಭಾವ "Python.h"

#ವ್ಯಾಖ್ಯಾನಿಸು PYTOK_ENCODING_COOKIE_MAX_ಸಾಲುಗಳು 2

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    _PYTOK_READER_PREPARED,
    _PYTOK_READER_FILE,
    _PYTOK_READER_READLINE,
    _PYTOK_READER_INTERACTIVE,
} Py_Tok_Reader_ಬಗೆ;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    PYTOK_READ_ಸಾಲು,
    _PYTOK_READ_EOF,
    _PYTOK_READ_STOPPED,
    PYTOK_READ_ಅಡಚಣೆ,
    _PYTOK_READ_ERROR,
} Py_Tok_Read_ಫಲಿತಾಂಶ;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    _PYTOK_ENCODING_ERROR = -1,
    PYTOK_ENCODING_ಮುಗಿದ,
    PYTOK_ENCODING_NEED_ಎರಡನೇ_ಸಾಲು,
} Py_Tok_Encoding_ಫಲಿತಾಂಶ;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    _PYTOK_CHUNK_BORROWED,
    _PYTOK_CHUNK_PYMEM,
    _PYTOK_CHUNK_PYOBJECT,
} _PyTok_ChunkOwnership;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಅಕ್ಷರ *ದತ್ತಾಂಶ;
    PyObject *owner;
    Py_ssize_t len;
    _PyTok_ChunkOwnership ownership;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ implicit_newline;
} _PyTok_Chunk;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ _PyTok_Reader {
    FILE *fp;  // Borrowed input stream; NULL for string and readline input.
    ಅಕ್ಷರ *encoding;  // Owned source encoding.
    PyObject *readline;
    PyObject *decoder;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *prompt;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *nextprompt;

    ಅಕ್ಷರ *file_ಬಫರ್;
    Py_ssize_t file_ಬಫರ್_cap;
    _PyTok_Chunk prefetched_ಸಾಲುಗಳು[PYTOK_ENCODING_COOKIE_MAX_ಸಾಲುಗಳು];

    ಅಕ್ಷರ *decoded;
    Py_ssize_t decoded_pos;
    Py_ssize_t decoded_ಪರಿಶೀಲನೆ;  // First byte not yet scanned for a newline.
    Py_ssize_t decoded_len;
    Py_ssize_t decoded_cap;
    Py_Tok_Reader_ಬಗೆ ಬಗೆ;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ prepared_final_newline;  // _PyTokenizer_Newline
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ decoded_ಬಾಲ_is_implicit;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ file_initialized;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ file_eof;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ decoder_finalized;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ stop_interactive;
} _PyTok_Reader;

ರಚನೆ tok_state;

// On failure, the buffer and capacity are unchanged.
ಪೂರ್ಣಾಂಕ Py_Tok_Reserve_ಬಫರ್(ಅಕ್ಷರ **, Py_ssize_t *, Py_ssize_t, Py_ssize_t);
ಅಕ್ಷರ *Py_Tok_Copy_ಬೈಟುಗಳು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, Py_ssize_t);
ಪೂರ್ಣಾಂಕ _PyTok_DecodeOnce(
    ರಚನೆ tok_state *, _PyTok_Chunk *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಅಕ್ಷರ *_PyTok_NormalizeNewlines(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, Py_ssize_t, Py_ssize_t *);
ಶೂನ್ಯ _PyTok_ChunkClear(_PyTok_Chunk *);
ಪೂರ್ಣಾಂಕ Py_Tok_ಗಣ_Encoding(ರಚನೆ tok_state *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
Py_Tok_Encoding_ಫಲಿತಾಂಶ _PyTok_DetectEncoding(
    ರಚನೆ tok_state *, ಸ್ಥಿರಾಂಕ _PyTok_Chunk *, ಸ್ಥಿರಾಂಕ _PyTok_Chunk *, ಪೂರ್ಣಾಂಕ,
    Py_ssize_t *);
ಪೂರ್ಣಾಂಕ _PyTok_PrepareString(ರಚನೆ tok_state *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ);
ಪೂರ್ಣಾಂಕ Py_Tok_ಆರಂಭ_Decoder(ರಚನೆ tok_state *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಪೂರ್ಣಾಂಕ _PyTok_DecodeChunk(ರಚನೆ tok_state *, _PyTok_Chunk *, ಪೂರ್ಣಾಂಕ);
ಪೂರ್ಣಾಂಕ Py_Tok_Decoder_Has_Buffered_ಒಳಹರಿವು(ರಚನೆ tok_state *);

#ಆದರೆ_ಅಂತ್ಯ
