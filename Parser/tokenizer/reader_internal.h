#నిర్వచితం_కాకపోతే Py_TOKENIZER_READER_అంతర్గత_H
#నిర్వచించు Py_TOKENIZER_READER_అంతర్గత_H

#అన్తర్భావ "Python.h"

#నిర్వచించు PYTOK_ENCODING_COOKIE_MAX_పంక్తులు 2

రకనిర్వచనం గణన {
    _PYTOK_READER_PREPARED,
    _PYTOK_READER_FILE,
    _PYTOK_READER_READLINE,
    _PYTOK_READER_INTERACTIVE,
} Py_Tok_Reader_రకం;

రకనిర్వచనం గణన {
    PYTOK_READ_పంక్తి,
    _PYTOK_READ_EOF,
    _PYTOK_READ_STOPPED,
    PYTOK_READ_అంతరాయం,
    _PYTOK_READ_ERROR,
} Py_Tok_Read_ఫలితం;

రకనిర్వచనం గణన {
    _PYTOK_ENCODING_ERROR = -1,
    PYTOK_ENCODING_పూర్తి,
    PYTOK_ENCODING_NEED_రెండవ_పంక్తి,
} Py_Tok_Encoding_ఫలితం;

రకనిర్వచనం గణన {
    _PYTOK_CHUNK_BORROWED,
    _PYTOK_CHUNK_PYMEM,
    _PYTOK_CHUNK_PYOBJECT,
} _PyTok_ChunkOwnership;

రకనిర్వచనం నిర్మాణం {
    అక్షరం *దత్తాంశం;
    PyObject *owner;
    Py_ssize_t len;
    _PyTok_ChunkOwnership ownership;
    గుర్తులేని అక్షరం implicit_newline;
} _PyTok_Chunk;

రకనిర్వచనం నిర్మాణం _PyTok_Reader {
    FILE *fp;  // Borrowed input stream; NULL for string and readline input.
    అక్షరం *encoding;  // Owned source encoding.
    PyObject *readline;
    PyObject *decoder;
    స్థిరరాశి అక్షరం *prompt;
    స్థిరరాశి అక్షరం *nextprompt;

    అక్షరం *file_బఫరు;
    Py_ssize_t file_బఫరు_cap;
    _PyTok_Chunk prefetched_పంక్తులు[PYTOK_ENCODING_COOKIE_MAX_పంక్తులు];

    అక్షరం *decoded;
    Py_ssize_t decoded_pos;
    Py_ssize_t decoded_పరిశీలన;  // First byte not yet scanned for a newline.
    Py_ssize_t decoded_len;
    Py_ssize_t decoded_cap;
    Py_Tok_Reader_రకం రకం;
    గుర్తులేని అక్షరం prepared_final_newline;  // _PyTokenizer_Newline
    గుర్తులేని అక్షరం decoded_తోక_is_implicit;
    గుర్తులేని అక్షరం file_initialized;
    గుర్తులేని అక్షరం file_eof;
    గుర్తులేని అక్షరం decoder_finalized;
    గుర్తులేని అక్షరం stop_interactive;
} _PyTok_Reader;

నిర్మాణం tok_state;

// On failure, the buffer and capacity are unchanged.
పూర్ణాంకం Py_Tok_Reserve_బఫరు(అక్షరం **, Py_ssize_t *, Py_ssize_t, Py_ssize_t);
అక్షరం *Py_Tok_Copy_బైట్లు(స్థిరరాశి అక్షరం *, Py_ssize_t);
పూర్ణాంకం _PyTok_DecodeOnce(
    నిర్మాణం tok_state *, _PyTok_Chunk *, స్థిరరాశి అక్షరం *, స్థిరరాశి అక్షరం *);
అక్షరం *_PyTok_NormalizeNewlines(స్థిరరాశి అక్షరం *, Py_ssize_t, Py_ssize_t *);
శూన్య _PyTok_ChunkClear(_PyTok_Chunk *);
పూర్ణాంకం Py_Tok_సమితి_Encoding(నిర్మాణం tok_state *, స్థిరరాశి అక్షరం *);
Py_Tok_Encoding_ఫలితం _PyTok_DetectEncoding(
    నిర్మాణం tok_state *, స్థిరరాశి _PyTok_Chunk *, స్థిరరాశి _PyTok_Chunk *, పూర్ణాంకం,
    Py_ssize_t *);
పూర్ణాంకం _PyTok_PrepareString(నిర్మాణం tok_state *, స్థిరరాశి అక్షరం *, పూర్ణాంకం, పూర్ణాంకం, పూర్ణాంకం);
పూర్ణాంకం Py_Tok_ప్రారంభం_Decoder(నిర్మాణం tok_state *, స్థిరరాశి అక్షరం *);
పూర్ణాంకం _PyTok_DecodeChunk(నిర్మాణం tok_state *, _PyTok_Chunk *, పూర్ణాంకం);
పూర్ణాంకం Py_Tok_Decoder_Has_Buffered_ఆదానం(నిర్మాణం tok_state *);

#అయితే_ముగింపు
