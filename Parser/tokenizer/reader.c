#ଅନ୍ତର୍ଭାଵ "Python.h"
#ଅନ୍ତର୍ଭାଵ "pycore_fileutils.h"
#ଅନ୍ତର୍ଭାଵ "pycore_pystate.h"
#ଅନ୍ତର୍ଭାଵ "pycore_runtime.h"

#ଅନ୍ତର୍ଭାଵ "errcode.h"
#ଅନ୍ତର୍ଭାଵ "helpers.h"
#ଅନ୍ତର୍ଭାଵ "reader.h"
#ଅନ୍ତର୍ଭାଵ "reader_internal.h"
#ଅନ୍ତର୍ଭାଵ "../lexer/state.h"

#define READER_BUFFER_GROWTH_FACTOR 2

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା
reader_is_streaming(_PyTok_ReaderKind kind)
{
    ଫେରନ୍ତୁ kind == _PYTOK_READER_FILE || kind == _PYTOK_READER_READLINE;
}

ଶୂନ୍ୟ
_PyTok_ReaderFree(ସଂରଚନା tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    ଯଦି (reader == ଖାଲି) {
        ଫେରନ୍ତୁ;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;
         i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Py_ARRAY_LENGTH(reader->prefetched_ଧାଡ଼ିଗୁଡ଼ିକ); i++) {
        _PyTok_ChunkClear(&reader->prefetched_ଧାଡ଼ିଗୁଡ଼ିକ[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_buffer);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = ଖାଲି;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyTokenizer_Traverse(ସଂରଚନା tok_state *tok, visitproc visit, ଶୂନ୍ୟ *arg)
{
    Py_VISIT(tok->filename);
    Py_VISIT(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_VISIT(reader->readline);
    Py_VISIT(reader->decoder);
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;
         i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Py_ARRAY_LENGTH(reader->prefetched_ଧାଡ଼ିଗୁଡ଼ିକ); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_ଧାଡ଼ିଗୁଡ଼ିକ[i];
        ଯଦି (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_VISIT(chunk->owner);
        }
    }
    ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyTok_ReserveBuffer(ଅକ୍ଷର **buffer, Py_ssize_t *capacity, Py_ssize_t needed,
                     Py_ssize_t initial_capacity)
{
    ପ୍ରତିଜ୍ଞା(initial_capacity > 0);
    ଯଦି (needed <= *capacity) {
        ଫେରନ୍ତୁ 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *capacity > 0 ? *capacity : initial_capacity;
    ଯେତେବେଳେ (cap < needed) {
        ଯଦି (cap > PY_SSIZE_T_MAX / READER_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            ଭାଙ୍ଗ;
        }
        cap *= READER_BUFFER_GROWTH_FACTOR;
    }
    ଅକ୍ଷର *resized = PyMem_Realloc(*buffer, cap);
    ଯଦି (resized == ଖାଲି) {
        PyErr_NoMemory();
        ଫେରନ୍ତୁ -1;
    }
    *buffer = resized;
    *capacity = cap;
    ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
append_decoded(_PyTok_Reader *reader, ଅଚଳ ଅକ୍ଷର *data, Py_ssize_t len)
{
    ଯଦି (len == 0) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି (reader->decoded_pos > 0) {
        Py_ssize_t remaining = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (size_t)remaining);
        reader->decoded_scan -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = remaining;
    }
    ଯଦି (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            _PyTok_ReserveBuffer(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        PyErr_NoMemory();
        ଫେରନ୍ତୁ -1;
    }
    memcpy(reader->decoded + reader->decoded_len, data, (size_t)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
append_implicit_newline(_PyTok_Reader *reader)
{
    ଯଦି (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି (append_decoded(reader, "\n", 1) < 0) {
        ଫେରନ୍ତୁ -1;
    }
    reader->decoded_tail_is_implicit = 1;
    ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
କାଢ଼_decoded_ଧାଡ଼ି(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    ପ୍ରତିଜ୍ଞା(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    ପ୍ରତିଜ୍ଞା(reader->decoded_scan >= reader->decoded_pos &&
           reader->decoded_scan <= reader->decoded_len);
    ଯଦି (reader->decoded_pos == reader->decoded_len) {
        ଫେରନ୍ତୁ 0;
    }
    ଅକ୍ଷର *start = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    ଅକ୍ଷର *newline = memchr(reader->decoded + reader->decoded_scan, '\n',
                           reader->decoded_len - reader->decoded_scan);
    ଯଦି (newline == ଖାଲି) {
        reader->decoded_scan = reader->decoded_len;
        ଫେରନ୍ତୁ 0;
    }
    Py_ssize_t len = newline - start + 1;
    chunk->data = start;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_scan = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_tail_is_implicit;
    ଯଦି (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_scan = 0;
        reader->decoded_tail_is_implicit = 0;
    }
    ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
chunk_is_ଧାଡ଼ି(ଅଚଳ _PyTok_Chunk *chunk)
{
    ଯଦି (chunk->len == 0 || chunk->data[chunk->len - 1] != '\n') {
        ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ memchr(chunk->data, '\n', chunk->len - 1) == ଖାଲି;
}

ସ୍ଥିର _PyTok_ReadResult
next_prepared(ସଂରଚନା tok_state *tok, _PyTok_Chunk *chunk)
{
    ଯଦି (tok->lineno >= tok->source.nlines) {
        ଫେରନ୍ତୁ _PYTOK_READ_EOF;
    }
    _PyTok_Span tail = {tok->inp, tok->source.base_offset + tok->source.len};
    _PyTok_Off newline = _PyTok_SourceFindByte(&tok->source, tail, '\n');
    _PyTok_Span ଧାଡ଼ି = {tail.start, newline >= 0 ? newline + 1 : tail.end};
    chunk->data = (ଅକ୍ଷର *)_PyTok_SourceSpanView(
        &tok->source, ଧାଡ଼ି, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = ଧାଡ଼ି.end == tail.end
        ? tok->reader->prepared_final_newline : 0;
    ଫେରନ୍ତୁ PYTOK_READ_ଧାଡ଼ି;
}

ସ୍ଥିର _PyTok_ReadResult
read_file_ଧାଡ଼ି(ସଂରଚନା tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    ପାଇଁ (;;) {
        ଯଦି (len > PY_SSIZE_T_MAX - BUFSIZ) {
            PyErr_NoMemory();
            ଫେରନ୍ତୁ _PYTOK_READ_ERROR;
        }
        ଯଦି (_PyTok_ReserveBuffer(&reader->file_buffer, &reader->file_buffer_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            ଫେରନ୍ତୁ _PYTOK_READ_ERROR;
        }
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା available = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Py_MIN(reader->file_buffer_cap - len, INT_MAX);
        size_t ପଠ = 0;
        ଅକ୍ଷର *result = _Py_UniversalNewlineFgetsWithSize(
            reader->file_buffer + len, available, reader->fp, ଖାଲି, &ପଠ);
        ଯଦି (result == ଖାଲି) {
            ଯଦି (len == 0) {
                ଫେରନ୍ତୁ _PYTOK_READ_EOF;
            }
            ଭାଙ୍ଗ;
        }
        len += (Py_ssize_t)ପଠ;
        ଯଦି (len > 0 && reader->file_buffer[len - 1] == '\n') {
            ଭାଙ୍ଗ;
        }
    }
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା implicit = reader->file_buffer[len - 1] != '\n';
    chunk->data = reader->file_buffer;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    ଫେରନ୍ତୁ PYTOK_READ_ଧାଡ଼ି;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
initialize_file(ସଂରଚନା tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    ଯଦି (reader->encoding != ଖାଲି) {
        ଫେରନ୍ତୁ _PyTok_StartDecoder(tok, "strict");
    }

    _PyTok_ReadResult result = read_file_ଧାଡ଼ି(
        tok, &reader->prefetched_ଧାଡ଼ିଗୁଡ଼ିକ[0]);
    ଯଦି (result == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି (result != PYTOK_READ_ଧାଡ଼ି) {
        ଫେରନ୍ତୁ -1;
    }
    Py_ssize_t bom_len;
    _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_ଧାଡ଼ିଗୁଡ଼ିକ[0], ଖାଲି, 0, &bom_len);
    ଯଦି (detection == _PYTOK_ENCODING_ERROR) {
        ଫେରନ୍ତୁ -1;
    }
    ଯଦି (detection == PYTOK_ENCODING_NEED_SECOND_ଧାଡ଼ି) {
        ଅକ୍ଷର *first = _PyTok_CopyBytes(
            reader->prefetched_ଧାଡ଼ିଗୁଡ଼ିକ[0].data,
            reader->prefetched_ଧାଡ଼ିଗୁଡ଼ିକ[0].len);
        ଯଦି (first == ଖାଲି) {
            tok->done = E_NOMEM;
            ଫେରନ୍ତୁ -1;
        }
        reader->prefetched_ଧାଡ଼ିଗୁଡ଼ିକ[0].data = first;
        reader->prefetched_ଧାଡ଼ିଗୁଡ଼ିକ[0].ownership = _PYTOK_CHUNK_PYMEM;
        result = read_file_ଧାଡ଼ି(tok, &reader->prefetched_ଧାଡ଼ିଗୁଡ଼ିକ[1]);
        ଯଦି (result == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        ଅନ୍ୟଥା ଯଦି (result != PYTOK_READ_ଧାଡ଼ି) {
            ଫେରନ୍ତୁ -1;
        }
        _PyTok_Chunk *second = reader->prefetched_ଧାଡ଼ିଗୁଡ଼ିକ[1].data != ଖାଲି
            ? &reader->prefetched_ଧାଡ଼ିଗୁଡ଼ିକ[1] : ଖାଲି;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_ଧାଡ଼ିଗୁଡ଼ିକ[0], second, 1, &bom_len);
        ଯଦି (detection == _PYTOK_ENCODING_ERROR) {
            ଫେରନ୍ତୁ -1;
        }
    }
    ଯଦି (bom_len != 0) {
        _PyTok_Chunk *first = &reader->prefetched_ଧାଡ଼ିଗୁଡ଼ିକ[0];
        ଯଦି (first->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(first->data, first->data + bom_len,
                    (size_t)(first->len - bom_len));
            first->data[first->len - bom_len] = '\0';
        }
        ଅନ୍ୟଥା {
            first->data += bom_len;
        }
        first->len -= bom_len;
    }
    ଯଦି (_PyTok_StartDecoder(tok, "strict") < 0) {
        ଫେରନ୍ତୁ -1;
    }
    ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
finalize_decoding(ସଂରଚନା tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    ଯଦି (reader->decoder_finalized) {
        ଫେରନ୍ତୁ 0;
    }
    reader->decoder_finalized = 1;
    ଯଦି (reader->decoder != ଖାଲି) {
        _PyTok_Chunk ନିବେଶ = {
            .data = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା decoded = _PyTok_DecodeChunk(tok, &ନିବେଶ, 1);
        ଯଦି (decoded == 0 &&
                append_decoded(reader, ନିବେଶ.data, ନିବେଶ.len) < 0) {
            tok->done = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&ନିବେଶ);
        ଯଦି (decoded < 0) {
            ଫେରନ୍ତୁ -1;
        }
    }
    ଯଦି (append_implicit_newline(reader) < 0) {
        tok->done = E_NOMEM;
        ଫେରନ୍ତୁ -1;
    }
    ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର _PyTok_ReadResult
next_file(ସଂରଚନା tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    ଯଦି (!reader->file_initialized && initialize_file(tok) < 0) {
        ଫେରନ୍ତୁ _PYTOK_READ_ERROR;
    }
    ପାଇଁ (;;) {
        ଯଦି (କାଢ଼_decoded_ଧାଡ଼ି(reader, chunk)) {
            ଫେରନ୍ତୁ PYTOK_READ_ଧାଡ଼ି;
        }
        _PyTok_Chunk ନିବେଶ = {0};
        ଯଦି (reader->prefetched_ଧାଡ଼ିଗୁଡ଼ିକ[0].data != ଖାଲି) {
            ନିବେଶ = reader->prefetched_ଧାଡ଼ିଗୁଡ଼ିକ[0];
            reader->prefetched_ଧାଡ଼ିଗୁଡ଼ିକ[0] = (_PyTok_Chunk){0};
        }
        ଅନ୍ୟଥା ଯଦି (reader->prefetched_ଧାଡ଼ିଗୁଡ଼ିକ[1].data != ଖାଲି) {
            ନିବେଶ = reader->prefetched_ଧାଡ଼ିଗୁଡ଼ିକ[1];
            reader->prefetched_ଧାଡ଼ିଗୁଡ଼ିକ[1] = (_PyTok_Chunk){0};
        }
        ଅନ୍ୟଥା ଯଦି (!reader->file_eof) {
            _PyTok_ReadResult result = read_file_ଧାଡ଼ି(tok, &ନିବେଶ);
            ଯଦି (result == _PYTOK_READ_ERROR) {
                ଫେରନ୍ତୁ result;
            }
            ଯଦି (result == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        ଯଦି (ନିବେଶ.data != ଖାଲି) {
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା implicit = ନିବେଶ.implicit_newline;
            ଯଦି (reader->decoder == ଖାଲି && !implicit) {
                *chunk = ନିବେଶ;
                ଫେରନ୍ତୁ PYTOK_READ_ଧାଡ଼ି;
            }
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା decoded = _PyTok_DecodeChunk(tok, &ନିବେଶ, 0);
            ଯଦି (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_ଧାଡ଼ି(&ନିବେଶ)) {
                *chunk = ନିବେଶ;
                ଫେରନ୍ତୁ PYTOK_READ_ଧାଡ଼ି;
            }
            ଯଦି (decoded == 0 &&
                    append_decoded(reader, ନିବେଶ.data, ନିବେଶ.len) < 0) {
                tok->done = E_NOMEM;
                decoded = -1;
            }
            ଯଦି (decoded == 0 && implicit) {
                reader->decoded_tail_is_implicit = 1;
            }
            _PyTok_ChunkClear(&ନିବେଶ);
            ଯଦି (decoded < 0) {
                ଫେରନ୍ତୁ _PYTOK_READ_ERROR;
            }
            ଜାରି;
        }
        ଯଦି (!reader->decoder_finalized) {
            ଯଦି (finalize_decoding(tok) < 0) {
                ଫେରନ୍ତୁ _PYTOK_READ_ERROR;
            }
            ଜାରି;
        }
        ଫେରନ୍ତୁ _PYTOK_READ_EOF;
    }
}

ସ୍ଥିର _PyTok_ReadResult
next_readline(ସଂରଚନା tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    ପାଇଁ (;;) {
        ଯଦି (କାଢ଼_decoded_ଧାଡ଼ି(reader, chunk)) {
            ଫେରନ୍ତୁ PYTOK_READ_ଧାଡ଼ି;
        }
        ଯଦି (reader->decoder_finalized) {
            ଫେରନ୍ତୁ _PYTOK_READ_EOF;
        }

        PyObject *raw = PyObject_CallNoArgs(reader->readline);
        ଯଦି (raw == ଖାଲି) {
            ଯଦି (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                ଯଦି (finalize_decoding(tok) < 0) {
                    ଫେରନ୍ତୁ _PYTOK_READ_ERROR;
                }
                ଜାରି;
            }
            ଫେରନ୍ତୁ _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk ନିବେଶ = {0};
        ଯଦି (reader->encoding != ଖାଲି) {
            ଯଦି (!PyBytes_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                ଫେରନ୍ତୁ _PYTOK_READ_ERROR;
            }
            ଯଦି (PyBytes_GET_SIZE(raw) == 0) {
                Py_DECREF(raw);
                ଯଦି (_PyTok_StartDecoder(tok, "replace") < 0) {
                    ଫେରନ୍ତୁ _PYTOK_READ_ERROR;
                }
                ଯଦି (finalize_decoding(tok) < 0) {
                    ଫେରନ୍ତୁ _PYTOK_READ_ERROR;
                }
                ଜାରି;
            }
            ନିବେଶ.owner = raw;
            ନିବେଶ.data = PyBytes_AS_STRING(raw);
            ନିବେଶ.len = PyBytes_GET_SIZE(raw);
            ନିବେଶ.ownership = _PYTOK_CHUNK_PYOBJECT;
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା decoded;
            ଯଦି (reader->decoder == ଖାଲି &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_ଧାଡ଼ି(&ନିବେଶ)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &ନିବେଶ, "utf-8", "replace");
            }
            ଅନ୍ୟଥା {
                decoded = _PyTok_StartDecoder(tok, "replace");
                ଯଦି (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &ନିବେଶ, 0);
                }
            }
            ଯଦି (decoded < 0) {
                _PyTok_ChunkClear(&ନିବେଶ);
                ଫେରନ୍ତୁ _PYTOK_READ_ERROR;
            }
        }
        ଅନ୍ୟଥା {
            ଯଦି (!PyUnicode_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                ଫେରନ୍ତୁ _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            ଅଚଳ ଅକ୍ଷର *utf8 = PyUnicode_AsUTF8AndSize(raw, &utf8_len);
            ଯଦି (utf8 == ଖାଲି) {
                Py_DECREF(raw);
                ଫେରନ୍ତୁ _PYTOK_READ_ERROR;
            }
            ନିବେଶ.owner = raw;
            ନିବେଶ.data = (ଅକ୍ଷର *)utf8;
            ନିବେଶ.len = utf8_len;
            ନିବେଶ.ownership = _PYTOK_CHUNK_PYOBJECT;
            ଯଦି (ନିବେଶ.len == 0) {
                _PyTok_ChunkClear(&ନିବେଶ);
                ଯଦି (finalize_decoding(tok) < 0) {
                    ଫେରନ୍ତୁ _PYTOK_READ_ERROR;
                }
                ଜାରି;
            }
        }

        ଯଦି (reader->decoded_pos == reader->decoded_len &&
                chunk_is_ଧାଡ଼ି(&ନିବେଶ)) {
            *chunk = ନିବେଶ;
            ଫେରନ୍ତୁ PYTOK_READ_ଧାଡ଼ି;
        }

        ଯଦି (append_decoded(reader, ନିବେଶ.data, ନିବେଶ.len) < 0) {
            _PyTok_ChunkClear(&ନିବେଶ);
            tok->done = E_NOMEM;
            ଫେରନ୍ତୁ _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&ନିବେଶ);
        ଯଦି (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା pending = Py_Tok_Decoder_Has_Buffered_ନିବେଶ(tok);
            ଯଦି (pending < 0) {
                ଫେରନ୍ତୁ _PYTOK_READ_ERROR;
            }
            ଯଦି (!pending && append_implicit_newline(reader) < 0) {
                tok->done = E_NOMEM;
                ଫେରନ୍ତୁ _PYTOK_READ_ERROR;
            }
        }
    }
}

ସ୍ଥିର _PyTok_ReadResult
next_interactive(ସଂରଚନା tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    ଯଦି (reader->stop_interactive) {
        ଫେରନ୍ତୁ _PYTOK_READ_STOPPED;
    }
    ଅକ୍ଷର *ନିବେଶ = PyOS_Readline(
        reader->fp != ଖାଲି ? reader->fp : stdin, stdout, reader->prompt);
    ଯଦି (reader->nextprompt != ଖାଲି) {
        reader->prompt = reader->nextprompt;
    }
    ଯଦି (ନିବେଶ == ଖାଲି) {
        ଫେରନ୍ତୁ _PYTOK_READ_INTERRUPT;
    }
    Py_ssize_t len = strlen(ନିବେଶ);
    ଯଦି (len == 0) {
        PyMem_Free(ନିବେଶ);
        ଫେରନ୍ତୁ _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .data = ନିବେଶ,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    ଯଦି (reader->encoding != ଖାଲି &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, ଖାଲି) < 0) {
        _PyTok_ChunkClear(&decoded);
        ଫେରନ୍ତୁ _PYTOK_READ_ERROR;
    }
    ଯଦି (memchr(decoded.data, '\r', decoded.len) == ଖାଲି) {
        *chunk = decoded;
        ଫେରନ୍ତୁ PYTOK_READ_ଧାଡ଼ି;
    }
    chunk->data = _PyTok_NormalizeNewlines(
        decoded.data, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    ଯଦି (chunk->data == ଖାଲି) {
        tok->done = E_NOMEM;
        ଫେରନ୍ତୁ _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    ଫେରନ୍ତୁ PYTOK_READ_ଧାଡ଼ି;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyTok_ReaderIsInteractive(ଅଚଳ ସଂରଚନା tok_state *tok)
{
    ଫେରନ୍ତୁ tok->reader->kind == _PYTOK_READER_INTERACTIVE;
}

ଅଚଳ ଅକ୍ଷର *
_PyTokenizer_RetainedSource(ଅଚଳ ସଂରଚନା tok_state *tok)
{
    ଯଦି (tok->reader->kind == _PYTOK_READER_PREPARED) {
        ଫେରନ୍ତୁ _PyTok_SourceData(&tok->source);
    }
    ଯଦି (tok->reader->kind == _PYTOK_READER_INTERACTIVE) {
        ଫେରନ୍ତୁ tok->source.bytes;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

ଶୂନ୍ୟ
_PyTok_ReaderStopInteractive(ସଂରଚନା tok_state *tok)
{
    ଯଦି (_PyTok_ReaderIsInteractive(tok)) {
        tok->reader->stop_interactive = 1;
    }
}

ସ୍ଥିର _PyTok_ReadResult
reader_next(ସଂରଚନା tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    ବାଛ (tok->reader->kind) {
        ସ୍ଥିତି _PYTOK_READER_PREPARED:
            ଫେରନ୍ତୁ next_prepared(tok, chunk);
        ସ୍ଥିତି _PYTOK_READER_FILE:
            ଫେରନ୍ତୁ next_file(tok, chunk);
        ସ୍ଥିତି _PYTOK_READER_READLINE:
            ଫେରନ୍ତୁ next_readline(tok, chunk);
        ସ୍ଥିତି _PYTOK_READER_INTERACTIVE:
            ଫେରନ୍ତୁ next_interactive(tok, chunk);
    }
    Py_UNREACHABLE();
}

ସ୍ଥିର ଶୂନ୍ୟ
reset_streaming_buffer(ସଂରଚନା tok_state *tok)
{
    ପ୍ରତିଜ୍ଞା(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_SourceDiscard(&tok->source);
    tok->buf_offset = tok->source.base_offset;
    tok->cur = tok->inp = tok->source.base_offset;
    tok->ଧାଡ଼ି_start = tok->buf_offset;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyTok_ReaderUnderflow(ସଂରଚନା tok_state *tok)
{
    ପ୍ରତିଜ୍ଞା(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    _PyTok_ReaderKind kind = reader->kind;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା prepared = kind == _PYTOK_READER_PREPARED;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା streaming = reader_is_streaming(kind);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା reset_buffer = tok->start < 0 &&
        _PyLexer_CurrentFTString(tok) == ଖାଲି;

    _PyTok_Chunk chunk;
    _PyTok_ReadResult result = reader_next(tok, &chunk);
    ଯଦି (result != PYTOK_READ_ଧାଡ଼ି) {
        ଯଦି (reset_buffer && streaming) {
            reset_streaming_buffer(tok);
        }
        ଯଦି (result == _PYTOK_READ_EOF) {
            tok->done = E_EOF;
        }
        ଅନ୍ୟଥା ଯଦି (result == _PYTOK_READ_STOPPED) {
            tok->done = E_INTERACT_STOP;
        }
        ଅନ୍ୟଥା ଯଦି (result == _PYTOK_READ_INTERRUPT) {
            tok->done = E_INTR;
        }
        ଅନ୍ୟଥା {
            ଯଦି (tok->done == E_OK) {
                tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                    ? E_NOMEM : E_ERROR;
            }
        }
        ଯଦି (kind == _PYTOK_READER_INTERACTIVE &&
                result != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି (tok->lineno == INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        tok->done = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        ଫେରନ୍ତୁ 0;
    }

    ଯଦି (!prepared) {
        ଯଦି (streaming && reset_buffer) {
            reset_streaming_buffer(tok);
        }
        _PyTok_Off source_start = Py_Tok_Source_Append_ଧାଡ଼ି(
            &tok->source, chunk.data, chunk.len);
        ଯଦି (source_start < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            ଫେରନ୍ତୁ 0;
        }
        ଯଦି (reset_buffer) {
            tok->cur = source_start;
            tok->buf_offset = source_start;
            tok->ଧାଡ଼ି_start = tok->buf_offset;
        }
        tok->inp = source_start + chunk.len;
    }
    ଅନ୍ୟଥା {
        ଯଦି (reset_buffer) {
            tok->buf_offset = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    ଯଦି (kind == _PYTOK_READER_FILE &&
            (reader->encoding == ଖାଲି ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                _PyTok_SourcePointer(&tok->source, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        ଫେରନ୍ତୁ 0;
    }
    _PyTok_ChunkClear(&chunk);
    ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ସଂରଚନା tok_state *
tokenizer_new_with_reader(_PyTok_ReaderKind kind)
{
    ସଂରଚନା tok_state *tok = PyMem_Calloc(1, ଆକାର(*tok));
    ଯଦି (tok == ଖାଲି) {
        PyErr_NoMemory();
        ଫେରନ୍ତୁ ଖାଲି;
    }
    tok->start = tok->ଧାଡ଼ି_start = -1;
    _PyTok_SourceInit(&tok->source);
    tok->done = E_OK;
    tok->layout.at_bol = 1;
    tok->start_loc = (_PyTok_Loc){-1, -1};
    tok->ftstring_ଗଦା = tok->ftstring_ଗଦା_inline;
    tok->ftstring_capacity = FTSTRING_ଗଦା_INLINE_CAPACITY;
#ifdef Py_DEBUG
    tok->debug = _Py_GetConfig()->parser_debug;
#endif
    tok->reader = PyMem_Calloc(1, ଆକାର(*tok->reader));
    ଯଦି (tok->reader == ଖାଲି) {
        PyErr_NoMemory();
        _PyTokenizer_Free(tok);
        ଫେରନ୍ତୁ ଖାଲି;
    }
    tok->reader->kind = kind;
    ଯଦି (kind == _PYTOK_READER_PREPARED) {
        ଫେରନ୍ତୁ tok;
    }
    ଯଦି (reader_is_streaming(kind)) {
        tok->cur = tok->inp = tok->source.base_offset;
        tok->ଧାଡ଼ି_start = tok->buf_offset;
    }
    ଫେରନ୍ତୁ tok;
}

ସ୍ଥିର ସଂରଚନା tok_state *
tokenizer_from_string(ଅଚଳ ଅକ୍ଷର *ନିବେଶ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା utf8_only, ପୂର୍ଣ୍ଣସଂଖ୍ୟା exec_ନିବେଶ,
                      ପୂର୍ଣ୍ଣସଂଖ୍ୟା preserve_crlf)
{
    ସଂରଚନା tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    ଯଦି (tok == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (_PyTok_PrepareString(
            tok, ନିବେଶ, utf8_only, exec_ନିବେଶ, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        ଫେରନ୍ତୁ ଖାଲି;
    }
    tok->cur = tok->inp = tok->source.base_offset;
    tok->ଧାଡ଼ି_start = tok->source.base_offset;
    ଫେରନ୍ତୁ tok;
}

ସଂରଚନା tok_state *
_PyTokenizer_FromString(ଅଚଳ ଅକ୍ଷର *ନିବେଶ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା exec_ନିବେଶ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା preserve_crlf)
{
    ଫେରନ୍ତୁ tokenizer_from_string(ନିବେଶ, 0, exec_ନିବେଶ, preserve_crlf);
}

ସଂରଚନା tok_state *
_PyTokenizer_FromUTF8(ଅଚଳ ଅକ୍ଷର *ନିବେଶ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା exec_ନିବେଶ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା preserve_crlf)
{
    ଫେରନ୍ତୁ tokenizer_from_string(ନିବେଶ, 1, exec_ନିବେଶ, preserve_crlf);
}

ସଂରଚନା tok_state *
_PyTokenizer_FromReadline(PyObject *readline, ଅଚଳ ଅକ୍ଷର *encoding)
{
    ସଂରଚନା tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    ଯଦି (tok == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (encoding != ଖାଲି && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        ଫେରନ୍ତୁ ଖାଲି;
    }
    tok->reader->readline = Py_NewRef(readline);
    ଫେରନ୍ତୁ tok;
}

ସଂରଚନା tok_state *
_PyTokenizer_FromFile(FILE *fp, ଅଚଳ ଅକ୍ଷର *encoding,
                      ଅଚଳ ଅକ୍ଷର *ps1, ଅଚଳ ଅକ୍ଷର *ps2)
{
    _PyTok_ReaderKind kind = ps1 != ଖାଲି || ps2 != ଖାଲି
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    ସଂରଚନା tok_state *tok = tokenizer_new_with_reader(kind);
    ଯଦି (tok == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (encoding != ଖାଲି && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        ଫେରନ୍ତୁ ଖାଲି;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    ଫେରନ୍ତୁ tok;
}

#ଯଦି defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂଘ {
    ଶୂନ୍ୟ *cookie;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;
} borrowed_fd;

ସ୍ଥିର ssize_t
borrow_read(ଶୂନ୍ୟ *cookie, ଅକ୍ଷର *buffer, size_t size)
{
    borrowed_fd borrowed = {.cookie = cookie};
    ଫେରନ୍ତୁ ପଠ(borrowed.fd, buffer, size);
}

ସ୍ଥିର FILE *
fdopen_borrow(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd)
{
    cookie_io_functions_t callbacks = {borrow_read, ଖାଲି, ଖାଲି, ଖାଲି};
    borrowed_fd borrowed = {.fd = fd};
    ଫେରନ୍ତୁ fopencookie(borrowed.cookie, "r", callbacks);
}
#ଅନ୍ୟଥା
ସ୍ଥିର FILE *
fdopen_borrow(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା copy = _Py_dup(fd);
    ଯଦି (copy < 0) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    FILE *fp = fdopen(copy, "r");
    ଯଦି (fp == ଖାଲି) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା saved_errno = errno;
        close(copy);
        errno = saved_errno;
    }
    ଫେରନ୍ତୁ fp;
}
#endif

ଅକ୍ଷର *
_PyTokenizer_FindEncodingFilename(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, PyObject *filename)
{
    FILE *fp = fdopen_borrow(fd);
    ଯଦି (fp == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ସଂରଚନା tok_state *tok = _PyTokenizer_FromFile(fp, ଖାଲି, ଖାଲି, ଖାଲି);
    ଯଦି (tok == ଖାଲି) {
        fclose(fp);
        ଫେରନ୍ତୁ ଖାଲି;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != ଖାଲି ? filename : &_Py_STR(anon_string));
    ଅକ୍ଷର *encoding = ଖାଲି;
    ଯଦି (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = ଖାଲି;
    }
    fclose(fp);
    _PyTokenizer_Free(tok);
    ଫେରନ୍ତୁ encoding;
}
