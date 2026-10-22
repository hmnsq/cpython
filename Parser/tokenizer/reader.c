#ಅನ್ತರ್ಭಾವ "Python.h"
#ಅನ್ತರ್ಭಾವ "pycore_fileutils.h"
#ಅನ್ತರ್ಭಾವ "pycore_pystate.h"
#ಅನ್ತರ್ಭಾವ "pycore_runtime.h"

#ಅನ್ತರ್ಭಾವ "errcode.h"
#ಅನ್ತರ್ಭಾವ "helpers.h"
#ಅನ್ತರ್ಭಾವ "reader.h"
#ಅನ್ತರ್ಭಾವ "reader_internal.h"
#ಅನ್ತರ್ಭಾವ "../lexer/state.h"

#define READER_BUFFER_GROWTH_FACTOR 2

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ
reader_is_streaming(_PyTok_ReaderKind kind)
{
    ಪ್ರತ್ಯಾಗಮ kind == _PYTOK_READER_FILE || kind == _PYTOK_READER_READLINE;
}

ಶೂನ್ಯ
_PyTok_ReaderFree(ಸಂರಚನಾ tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    ಯದಿ (reader == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0;
         i < (ಪೂರ್ಣಾಙ್ಕ)Py_ARRAY_LENGTH(reader->prefetched_ಸಾಲುಗಳು); i++) {
        _PyTok_ChunkClear(&reader->prefetched_ಸಾಲುಗಳು[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_buffer);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = ರಿಕ್ತ;
}

ಪೂರ್ಣಾಙ್ಕ
_PyTokenizer_Traverse(ಸಂರಚನಾ tok_state *tok, visitproc visit, ಶೂನ್ಯ *arg)
{
    Py_VISIT(tok->filename);
    Py_VISIT(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_VISIT(reader->readline);
    Py_VISIT(reader->decoder);
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0;
         i < (ಪೂರ್ಣಾಙ್ಕ)Py_ARRAY_LENGTH(reader->prefetched_ಸಾಲುಗಳು); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_ಸಾಲುಗಳು[i];
        ಯದಿ (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_VISIT(chunk->owner);
        }
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ
_PyTok_ReserveBuffer(ವರ್ಣ **buffer, Py_ssize_t *capacity, Py_ssize_t needed,
                     Py_ssize_t initial_capacity)
{
    ಪ್ರತಿಜ್ಞಾ(initial_capacity > 0);
    ಯದಿ (needed <= *capacity) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *capacity > 0 ? *capacity : initial_capacity;
    ಯಾವತ್ (cap < needed) {
        ಯದಿ (cap > PY_SSIZE_T_MAX / READER_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            ಭಙ್ಗ;
        }
        cap *= READER_BUFFER_GROWTH_FACTOR;
    }
    ವರ್ಣ *resized = PyMem_Realloc(*buffer, cap);
    ಯದಿ (resized == ರಿಕ್ತ) {
        PyErr_NoMemory();
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    *buffer = resized;
    *capacity = cap;
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
append_decoded(_PyTok_Reader *reader, ಧ್ರುವ ವರ್ಣ *data, Py_ssize_t len)
{
    ಯದಿ (len == 0) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ (reader->decoded_pos > 0) {
        Py_ssize_t remaining = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (size_t)remaining);
        reader->decoded_scan -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = remaining;
    }
    ಯದಿ (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            _PyTok_ReserveBuffer(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        PyErr_NoMemory();
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    memcpy(reader->decoded + reader->decoded_len, data, (size_t)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
append_implicit_newline(_PyTok_Reader *reader)
{
    ಯದಿ (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ (append_decoded(reader, "\n", 1) < 0) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    reader->decoded_tail_is_implicit = 1;
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ತೆಗೆ_decoded_ಸಾಲು(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    ಪ್ರತಿಜ್ಞಾ(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    ಪ್ರತಿಜ್ಞಾ(reader->decoded_scan >= reader->decoded_pos &&
           reader->decoded_scan <= reader->decoded_len);
    ಯದಿ (reader->decoded_pos == reader->decoded_len) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ವರ್ಣ *start = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    ವರ್ಣ *newline = memchr(reader->decoded + reader->decoded_scan, '\n',
                           reader->decoded_len - reader->decoded_scan);
    ಯದಿ (newline == ರಿಕ್ತ) {
        reader->decoded_scan = reader->decoded_len;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    Py_ssize_t len = newline - start + 1;
    chunk->data = start;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_scan = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_tail_is_implicit;
    ಯದಿ (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_scan = 0;
        reader->decoded_tail_is_implicit = 0;
    }
    ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
chunk_is_ಸಾಲು(ಧ್ರುವ _PyTok_Chunk *chunk)
{
    ಯದಿ (chunk->len == 0 || chunk->data[chunk->len - 1] != '\n') {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತ್ಯಾಗಮ memchr(chunk->data, '\n', chunk->len - 1) == ರಿಕ್ತ;
}

ಸ್ಥಿರ _PyTok_ReadResult
next_prepared(ಸಂರಚನಾ tok_state *tok, _PyTok_Chunk *chunk)
{
    ಯದಿ (tok->lineno >= tok->source.nlines) {
        ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_EOF;
    }
    _PyTok_Span tail = {tok->inp, tok->source.base_offset + tok->source.len};
    _PyTok_Off newline = _PyTok_SourceFindByte(&tok->source, tail, '\n');
    _PyTok_Span ಸಾಲು = {tail.start, newline >= 0 ? newline + 1 : tail.end};
    chunk->data = (ವರ್ಣ *)_PyTok_SourceSpanView(
        &tok->source, ಸಾಲು, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = ಸಾಲು.end == tail.end
        ? tok->reader->prepared_final_newline : 0;
    ಪ್ರತ್ಯಾಗಮ PYTOK_READ_ಸಾಲು;
}

ಸ್ಥಿರ _PyTok_ReadResult
read_file_ಸಾಲು(ಸಂರಚನಾ tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    ಪ್ರತಿ (;;) {
        ಯದಿ (len > PY_SSIZE_T_MAX - BUFSIZ) {
            PyErr_NoMemory();
            ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_ERROR;
        }
        ಯದಿ (_PyTok_ReserveBuffer(&reader->file_buffer, &reader->file_buffer_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_ERROR;
        }
        ಪೂರ್ಣಾಙ್ಕ available = (ಪೂರ್ಣಾಙ್ಕ)Py_MIN(reader->file_buffer_cap - len, INT_MAX);
        size_t ಪಠ = 0;
        ವರ್ಣ *result = _Py_UniversalNewlineFgetsWithSize(
            reader->file_buffer + len, available, reader->fp, ರಿಕ್ತ, &ಪಠ);
        ಯದಿ (result == ರಿಕ್ತ) {
            ಯದಿ (len == 0) {
                ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_EOF;
            }
            ಭಙ್ಗ;
        }
        len += (Py_ssize_t)ಪಠ;
        ಯದಿ (len > 0 && reader->file_buffer[len - 1] == '\n') {
            ಭಙ್ಗ;
        }
    }
    ಪೂರ್ಣಾಙ್ಕ implicit = reader->file_buffer[len - 1] != '\n';
    chunk->data = reader->file_buffer;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    ಪ್ರತ್ಯಾಗಮ PYTOK_READ_ಸಾಲು;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
initialize_file(ಸಂರಚನಾ tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    ಯದಿ (reader->encoding != ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ _PyTok_StartDecoder(tok, "strict");
    }

    _PyTok_ReadResult result = read_file_ಸಾಲು(
        tok, &reader->prefetched_ಸಾಲುಗಳು[0]);
    ಯದಿ (result == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ (result != PYTOK_READ_ಸಾಲು) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    Py_ssize_t bom_len;
    _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_ಸಾಲುಗಳು[0], ರಿಕ್ತ, 0, &bom_len);
    ಯದಿ (detection == _PYTOK_ENCODING_ERROR) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಯದಿ (detection == PYTOK_ENCODING_NEED_SECOND_ಸಾಲು) {
        ವರ್ಣ *first = _PyTok_CopyBytes(
            reader->prefetched_ಸಾಲುಗಳು[0].data,
            reader->prefetched_ಸಾಲುಗಳು[0].len);
        ಯದಿ (first == ರಿಕ್ತ) {
            tok->done = E_NOMEM;
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        reader->prefetched_ಸಾಲುಗಳು[0].data = first;
        reader->prefetched_ಸಾಲುಗಳು[0].ownership = _PYTOK_CHUNK_PYMEM;
        result = read_file_ಸಾಲು(tok, &reader->prefetched_ಸಾಲುಗಳು[1]);
        ಯದಿ (result == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        ಅನ್ಯಥಾ ಯದಿ (result != PYTOK_READ_ಸಾಲು) {
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        _PyTok_Chunk *second = reader->prefetched_ಸಾಲುಗಳು[1].data != ರಿಕ್ತ
            ? &reader->prefetched_ಸಾಲುಗಳು[1] : ರಿಕ್ತ;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_ಸಾಲುಗಳು[0], second, 1, &bom_len);
        ಯದಿ (detection == _PYTOK_ENCODING_ERROR) {
            ಪ್ರತ್ಯಾಗಮ -1;
        }
    }
    ಯದಿ (bom_len != 0) {
        _PyTok_Chunk *first = &reader->prefetched_ಸಾಲುಗಳು[0];
        ಯದಿ (first->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(first->data, first->data + bom_len,
                    (size_t)(first->len - bom_len));
            first->data[first->len - bom_len] = '\0';
        }
        ಅನ್ಯಥಾ {
            first->data += bom_len;
        }
        first->len -= bom_len;
    }
    ಯದಿ (_PyTok_StartDecoder(tok, "strict") < 0) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
finalize_decoding(ಸಂರಚನಾ tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    ಯದಿ (reader->decoder_finalized) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    reader->decoder_finalized = 1;
    ಯದಿ (reader->decoder != ರಿಕ್ತ) {
        _PyTok_Chunk ಒಳಹರಿವು = {
            .data = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        ಪೂರ್ಣಾಙ್ಕ decoded = _PyTok_DecodeChunk(tok, &ಒಳಹರಿವು, 1);
        ಯದಿ (decoded == 0 &&
                append_decoded(reader, ಒಳಹರಿವು.data, ಒಳಹರಿವು.len) < 0) {
            tok->done = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&ಒಳಹರಿವು);
        ಯದಿ (decoded < 0) {
            ಪ್ರತ್ಯಾಗಮ -1;
        }
    }
    ಯದಿ (append_implicit_newline(reader) < 0) {
        tok->done = E_NOMEM;
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ _PyTok_ReadResult
next_file(ಸಂರಚನಾ tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    ಯದಿ (!reader->file_initialized && initialize_file(tok) < 0) {
        ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_ERROR;
    }
    ಪ್ರತಿ (;;) {
        ಯದಿ (ತೆಗೆ_decoded_ಸಾಲು(reader, chunk)) {
            ಪ್ರತ್ಯಾಗಮ PYTOK_READ_ಸಾಲು;
        }
        _PyTok_Chunk ಒಳಹರಿವು = {0};
        ಯದಿ (reader->prefetched_ಸಾಲುಗಳು[0].data != ರಿಕ್ತ) {
            ಒಳಹರಿವು = reader->prefetched_ಸಾಲುಗಳು[0];
            reader->prefetched_ಸಾಲುಗಳು[0] = (_PyTok_Chunk){0};
        }
        ಅನ್ಯಥಾ ಯದಿ (reader->prefetched_ಸಾಲುಗಳು[1].data != ರಿಕ್ತ) {
            ಒಳಹರಿವು = reader->prefetched_ಸಾಲುಗಳು[1];
            reader->prefetched_ಸಾಲುಗಳು[1] = (_PyTok_Chunk){0};
        }
        ಅನ್ಯಥಾ ಯದಿ (!reader->file_eof) {
            _PyTok_ReadResult result = read_file_ಸಾಲು(tok, &ಒಳಹರಿವು);
            ಯದಿ (result == _PYTOK_READ_ERROR) {
                ಪ್ರತ್ಯಾಗಮ result;
            }
            ಯದಿ (result == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        ಯದಿ (ಒಳಹರಿವು.data != ರಿಕ್ತ) {
            ಪೂರ್ಣಾಙ್ಕ implicit = ಒಳಹರಿವು.implicit_newline;
            ಯದಿ (reader->decoder == ರಿಕ್ತ && !implicit) {
                *chunk = ಒಳಹರಿವು;
                ಪ್ರತ್ಯಾಗಮ PYTOK_READ_ಸಾಲು;
            }
            ಪೂರ್ಣಾಙ್ಕ decoded = _PyTok_DecodeChunk(tok, &ಒಳಹರಿವು, 0);
            ಯದಿ (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_ಸಾಲು(&ಒಳಹರಿವು)) {
                *chunk = ಒಳಹರಿವು;
                ಪ್ರತ್ಯಾಗಮ PYTOK_READ_ಸಾಲು;
            }
            ಯದಿ (decoded == 0 &&
                    append_decoded(reader, ಒಳಹರಿವು.data, ಒಳಹರಿವು.len) < 0) {
                tok->done = E_NOMEM;
                decoded = -1;
            }
            ಯದಿ (decoded == 0 && implicit) {
                reader->decoded_tail_is_implicit = 1;
            }
            _PyTok_ChunkClear(&ಒಳಹರಿವು);
            ಯದಿ (decoded < 0) {
                ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_ERROR;
            }
            ಅನುವರ್ತ;
        }
        ಯದಿ (!reader->decoder_finalized) {
            ಯದಿ (finalize_decoding(tok) < 0) {
                ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_ERROR;
            }
            ಅನುವರ್ತ;
        }
        ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_EOF;
    }
}

ಸ್ಥಿರ _PyTok_ReadResult
next_readline(ಸಂರಚನಾ tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    ಪ್ರತಿ (;;) {
        ಯದಿ (ತೆಗೆ_decoded_ಸಾಲು(reader, chunk)) {
            ಪ್ರತ್ಯಾಗಮ PYTOK_READ_ಸಾಲು;
        }
        ಯದಿ (reader->decoder_finalized) {
            ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_EOF;
        }

        PyObject *raw = PyObject_CallNoArgs(reader->readline);
        ಯದಿ (raw == ರಿಕ್ತ) {
            ಯದಿ (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                ಯದಿ (finalize_decoding(tok) < 0) {
                    ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_ERROR;
                }
                ಅನುವರ್ತ;
            }
            ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk ಒಳಹರಿವು = {0};
        ಯದಿ (reader->encoding != ರಿಕ್ತ) {
            ಯದಿ (!PyBytes_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_ERROR;
            }
            ಯದಿ (PyBytes_GET_SIZE(raw) == 0) {
                Py_DECREF(raw);
                ಯದಿ (_PyTok_StartDecoder(tok, "replace") < 0) {
                    ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_ERROR;
                }
                ಯದಿ (finalize_decoding(tok) < 0) {
                    ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_ERROR;
                }
                ಅನುವರ್ತ;
            }
            ಒಳಹರಿವು.owner = raw;
            ಒಳಹರಿವು.data = PyBytes_AS_STRING(raw);
            ಒಳಹರಿವು.len = PyBytes_GET_SIZE(raw);
            ಒಳಹರಿವು.ownership = _PYTOK_CHUNK_PYOBJECT;
            ಪೂರ್ಣಾಙ್ಕ decoded;
            ಯದಿ (reader->decoder == ರಿಕ್ತ &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_ಸಾಲು(&ಒಳಹರಿವು)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &ಒಳಹರಿವು, "utf-8", "replace");
            }
            ಅನ್ಯಥಾ {
                decoded = _PyTok_StartDecoder(tok, "replace");
                ಯದಿ (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &ಒಳಹರಿವು, 0);
                }
            }
            ಯದಿ (decoded < 0) {
                _PyTok_ChunkClear(&ಒಳಹರಿವು);
                ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_ERROR;
            }
        }
        ಅನ್ಯಥಾ {
            ಯದಿ (!PyUnicode_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            ಧ್ರುವ ವರ್ಣ *utf8 = PyUnicode_AsUTF8AndSize(raw, &utf8_len);
            ಯದಿ (utf8 == ರಿಕ್ತ) {
                Py_DECREF(raw);
                ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_ERROR;
            }
            ಒಳಹರಿವು.owner = raw;
            ಒಳಹರಿವು.data = (ವರ್ಣ *)utf8;
            ಒಳಹರಿವು.len = utf8_len;
            ಒಳಹರಿವು.ownership = _PYTOK_CHUNK_PYOBJECT;
            ಯದಿ (ಒಳಹರಿವು.len == 0) {
                _PyTok_ChunkClear(&ಒಳಹರಿವು);
                ಯದಿ (finalize_decoding(tok) < 0) {
                    ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_ERROR;
                }
                ಅನುವರ್ತ;
            }
        }

        ಯದಿ (reader->decoded_pos == reader->decoded_len &&
                chunk_is_ಸಾಲು(&ಒಳಹರಿವು)) {
            *chunk = ಒಳಹರಿವು;
            ಪ್ರತ್ಯಾಗಮ PYTOK_READ_ಸಾಲು;
        }

        ಯದಿ (append_decoded(reader, ಒಳಹರಿವು.data, ಒಳಹರಿವು.len) < 0) {
            _PyTok_ChunkClear(&ಒಳಹರಿವು);
            tok->done = E_NOMEM;
            ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&ಒಳಹರಿವು);
        ಯದಿ (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            ಪೂರ್ಣಾಙ್ಕ pending = Py_Tok_Decoder_Has_Buffered_ಒಳಹರಿವು(tok);
            ಯದಿ (pending < 0) {
                ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_ERROR;
            }
            ಯದಿ (!pending && append_implicit_newline(reader) < 0) {
                tok->done = E_NOMEM;
                ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_ERROR;
            }
        }
    }
}

ಸ್ಥಿರ _PyTok_ReadResult
next_interactive(ಸಂರಚನಾ tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    ಯದಿ (reader->stop_interactive) {
        ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_STOPPED;
    }
    ವರ್ಣ *ಒಳಹರಿವು = PyOS_Readline(
        reader->fp != ರಿಕ್ತ ? reader->fp : stdin, stdout, reader->prompt);
    ಯದಿ (reader->nextprompt != ರಿಕ್ತ) {
        reader->prompt = reader->nextprompt;
    }
    ಯದಿ (ಒಳಹರಿವು == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_INTERRUPT;
    }
    Py_ssize_t len = strlen(ಒಳಹರಿವು);
    ಯದಿ (len == 0) {
        PyMem_Free(ಒಳಹರಿವು);
        ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .data = ಒಳಹರಿವು,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    ಯದಿ (reader->encoding != ರಿಕ್ತ &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, ರಿಕ್ತ) < 0) {
        _PyTok_ChunkClear(&decoded);
        ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_ERROR;
    }
    ಯದಿ (memchr(decoded.data, '\r', decoded.len) == ರಿಕ್ತ) {
        *chunk = decoded;
        ಪ್ರತ್ಯಾಗಮ PYTOK_READ_ಸಾಲು;
    }
    chunk->data = _PyTok_NormalizeNewlines(
        decoded.data, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    ಯದಿ (chunk->data == ರಿಕ್ತ) {
        tok->done = E_NOMEM;
        ಪ್ರತ್ಯಾಗಮ _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    ಪ್ರತ್ಯಾಗಮ PYTOK_READ_ಸಾಲು;
}

ಪೂರ್ಣಾಙ್ಕ
_PyTok_ReaderIsInteractive(ಧ್ರುವ ಸಂರಚನಾ tok_state *tok)
{
    ಪ್ರತ್ಯಾಗಮ tok->reader->kind == _PYTOK_READER_INTERACTIVE;
}

ಧ್ರುವ ವರ್ಣ *
_PyTokenizer_RetainedSource(ಧ್ರುವ ಸಂರಚನಾ tok_state *tok)
{
    ಯದಿ (tok->reader->kind == _PYTOK_READER_PREPARED) {
        ಪ್ರತ್ಯಾಗಮ _PyTok_SourceData(&tok->source);
    }
    ಯದಿ (tok->reader->kind == _PYTOK_READER_INTERACTIVE) {
        ಪ್ರತ್ಯಾಗಮ tok->source.bytes;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಶೂನ್ಯ
_PyTok_ReaderStopInteractive(ಸಂರಚನಾ tok_state *tok)
{
    ಯದಿ (_PyTok_ReaderIsInteractive(tok)) {
        tok->reader->stop_interactive = 1;
    }
}

ಸ್ಥಿರ _PyTok_ReadResult
reader_next(ಸಂರಚನಾ tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    ವಿಕಲ್ಪ (tok->reader->kind) {
        ಪಕ್ಷ _PYTOK_READER_PREPARED:
            ಪ್ರತ್ಯಾಗಮ next_prepared(tok, chunk);
        ಪಕ್ಷ _PYTOK_READER_FILE:
            ಪ್ರತ್ಯಾಗಮ next_file(tok, chunk);
        ಪಕ್ಷ _PYTOK_READER_READLINE:
            ಪ್ರತ್ಯಾಗಮ next_readline(tok, chunk);
        ಪಕ್ಷ _PYTOK_READER_INTERACTIVE:
            ಪ್ರತ್ಯಾಗಮ next_interactive(tok, chunk);
    }
    Py_UNREACHABLE();
}

ಸ್ಥಿರ ಶೂನ್ಯ
reset_streaming_buffer(ಸಂರಚನಾ tok_state *tok)
{
    ಪ್ರತಿಜ್ಞಾ(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_SourceDiscard(&tok->source);
    tok->buf_offset = tok->source.base_offset;
    tok->cur = tok->inp = tok->source.base_offset;
    tok->ಸಾಲು_start = tok->buf_offset;
}

ಪೂರ್ಣಾಙ್ಕ
_PyTok_ReaderUnderflow(ಸಂರಚನಾ tok_state *tok)
{
    ಪ್ರತಿಜ್ಞಾ(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    _PyTok_ReaderKind kind = reader->kind;
    ಪೂರ್ಣಾಙ್ಕ prepared = kind == _PYTOK_READER_PREPARED;
    ಪೂರ್ಣಾಙ್ಕ streaming = reader_is_streaming(kind);
    ಪೂರ್ಣಾಙ್ಕ reset_buffer = tok->start < 0 &&
        _PyLexer_CurrentFTString(tok) == ರಿಕ್ತ;

    _PyTok_Chunk chunk;
    _PyTok_ReadResult result = reader_next(tok, &chunk);
    ಯದಿ (result != PYTOK_READ_ಸಾಲು) {
        ಯದಿ (reset_buffer && streaming) {
            reset_streaming_buffer(tok);
        }
        ಯದಿ (result == _PYTOK_READ_EOF) {
            tok->done = E_EOF;
        }
        ಅನ್ಯಥಾ ಯದಿ (result == _PYTOK_READ_STOPPED) {
            tok->done = E_INTERACT_STOP;
        }
        ಅನ್ಯಥಾ ಯದಿ (result == _PYTOK_READ_INTERRUPT) {
            tok->done = E_INTR;
        }
        ಅನ್ಯಥಾ {
            ಯದಿ (tok->done == E_OK) {
                tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                    ? E_NOMEM : E_ERROR;
            }
        }
        ಯದಿ (kind == _PYTOK_READER_INTERACTIVE &&
                result != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ (tok->lineno == INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        tok->done = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ (!prepared) {
        ಯದಿ (streaming && reset_buffer) {
            reset_streaming_buffer(tok);
        }
        _PyTok_Off source_start = Py_Tok_Source_Append_ಸಾಲು(
            &tok->source, chunk.data, chunk.len);
        ಯದಿ (source_start < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        ಯದಿ (reset_buffer) {
            tok->cur = source_start;
            tok->buf_offset = source_start;
            tok->ಸಾಲು_start = tok->buf_offset;
        }
        tok->inp = source_start + chunk.len;
    }
    ಅನ್ಯಥಾ {
        ಯದಿ (reset_buffer) {
            tok->buf_offset = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    ಯದಿ (kind == _PYTOK_READER_FILE &&
            (reader->encoding == ರಿಕ್ತ ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                _PyTok_SourcePointer(&tok->source, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    _PyTok_ChunkClear(&chunk);
    ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಸಂರಚನಾ tok_state *
tokenizer_new_with_reader(_PyTok_ReaderKind kind)
{
    ಸಂರಚನಾ tok_state *tok = PyMem_Calloc(1, ಪರಿಮಾಣ(*tok));
    ಯದಿ (tok == ರಿಕ್ತ) {
        PyErr_NoMemory();
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    tok->start = tok->ಸಾಲು_start = -1;
    _PyTok_SourceInit(&tok->source);
    tok->done = E_OK;
    tok->layout.at_bol = 1;
    tok->start_loc = (_PyTok_Loc){-1, -1};
    tok->ftstring_ರಾಶಿ = tok->ftstring_ರಾಶಿ_inline;
    tok->ftstring_capacity = FTSTRING_ರಾಶಿ_INLINE_CAPACITY;
#ifdef Py_DEBUG
    tok->debug = _Py_GetConfig()->parser_debug;
#endif
    tok->reader = PyMem_Calloc(1, ಪರಿಮಾಣ(*tok->reader));
    ಯದಿ (tok->reader == ರಿಕ್ತ) {
        PyErr_NoMemory();
        _PyTokenizer_Free(tok);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    tok->reader->kind = kind;
    ಯದಿ (kind == _PYTOK_READER_PREPARED) {
        ಪ್ರತ್ಯಾಗಮ tok;
    }
    ಯದಿ (reader_is_streaming(kind)) {
        tok->cur = tok->inp = tok->source.base_offset;
        tok->ಸಾಲು_start = tok->buf_offset;
    }
    ಪ್ರತ್ಯಾಗಮ tok;
}

ಸ್ಥಿರ ಸಂರಚನಾ tok_state *
tokenizer_from_string(ಧ್ರುವ ವರ್ಣ *ಒಳಹರಿವು, ಪೂರ್ಣಾಙ್ಕ utf8_only, ಪೂರ್ಣಾಙ್ಕ exec_ಒಳಹರಿವು,
                      ಪೂರ್ಣಾಙ್ಕ preserve_crlf)
{
    ಸಂರಚನಾ tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    ಯದಿ (tok == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (_PyTok_PrepareString(
            tok, ಒಳಹರಿವು, utf8_only, exec_ಒಳಹರಿವು, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    tok->cur = tok->inp = tok->source.base_offset;
    tok->ಸಾಲು_start = tok->source.base_offset;
    ಪ್ರತ್ಯಾಗಮ tok;
}

ಸಂರಚನಾ tok_state *
_PyTokenizer_FromString(ಧ್ರುವ ವರ್ಣ *ಒಳಹರಿವು, ಪೂರ್ಣಾಙ್ಕ exec_ಒಳಹರಿವು, ಪೂರ್ಣಾಙ್ಕ preserve_crlf)
{
    ಪ್ರತ್ಯಾಗಮ tokenizer_from_string(ಒಳಹರಿವು, 0, exec_ಒಳಹರಿವು, preserve_crlf);
}

ಸಂರಚನಾ tok_state *
_PyTokenizer_FromUTF8(ಧ್ರುವ ವರ್ಣ *ಒಳಹರಿವು, ಪೂರ್ಣಾಙ್ಕ exec_ಒಳಹರಿವು, ಪೂರ್ಣಾಙ್ಕ preserve_crlf)
{
    ಪ್ರತ್ಯಾಗಮ tokenizer_from_string(ಒಳಹರಿವು, 1, exec_ಒಳಹರಿವು, preserve_crlf);
}

ಸಂರಚನಾ tok_state *
_PyTokenizer_FromReadline(PyObject *readline, ಧ್ರುವ ವರ್ಣ *encoding)
{
    ಸಂರಚನಾ tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    ಯದಿ (tok == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (encoding != ರಿಕ್ತ && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    tok->reader->readline = Py_NewRef(readline);
    ಪ್ರತ್ಯಾಗಮ tok;
}

ಸಂರಚನಾ tok_state *
_PyTokenizer_FromFile(FILE *fp, ಧ್ರುವ ವರ್ಣ *encoding,
                      ಧ್ರುವ ವರ್ಣ *ps1, ಧ್ರುವ ವರ್ಣ *ps2)
{
    _PyTok_ReaderKind kind = ps1 != ರಿಕ್ತ || ps2 != ರಿಕ್ತ
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    ಸಂರಚನಾ tok_state *tok = tokenizer_new_with_reader(kind);
    ಯದಿ (tok == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (encoding != ರಿಕ್ತ && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    ಪ್ರತ್ಯಾಗಮ tok;
}

#ಯದಿ defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಙ್ಘ {
    ಶೂನ್ಯ *cookie;
    ಪೂರ್ಣಾಙ್ಕ fd;
} borrowed_fd;

ಸ್ಥಿರ ssize_t
borrow_read(ಶೂನ್ಯ *cookie, ವರ್ಣ *buffer, size_t size)
{
    borrowed_fd borrowed = {.cookie = cookie};
    ಪ್ರತ್ಯಾಗಮ ಪಠ(borrowed.fd, buffer, size);
}

ಸ್ಥಿರ FILE *
fdopen_borrow(ಪೂರ್ಣಾಙ್ಕ fd)
{
    cookie_io_functions_t callbacks = {borrow_read, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ};
    borrowed_fd borrowed = {.fd = fd};
    ಪ್ರತ್ಯಾಗಮ fopencookie(borrowed.cookie, "r", callbacks);
}
#ಅನ್ಯಥಾ
ಸ್ಥಿರ FILE *
fdopen_borrow(ಪೂರ್ಣಾಙ್ಕ fd)
{
    ಪೂರ್ಣಾಙ್ಕ copy = _Py_dup(fd);
    ಯದಿ (copy < 0) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    FILE *fp = fdopen(copy, "r");
    ಯದಿ (fp == ರಿಕ್ತ) {
        ಪೂರ್ಣಾಙ್ಕ saved_errno = errno;
        close(copy);
        errno = saved_errno;
    }
    ಪ್ರತ್ಯಾಗಮ fp;
}
#endif

ವರ್ಣ *
_PyTokenizer_FindEncodingFilename(ಪೂರ್ಣಾಙ್ಕ fd, PyObject *filename)
{
    FILE *fp = fdopen_borrow(fd);
    ಯದಿ (fp == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಸಂರಚನಾ tok_state *tok = _PyTokenizer_FromFile(fp, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
    ಯದಿ (tok == ರಿಕ್ತ) {
        fclose(fp);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != ರಿಕ್ತ ? filename : &_Py_STR(anon_string));
    ವರ್ಣ *encoding = ರಿಕ್ತ;
    ಯದಿ (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = ರಿಕ್ತ;
    }
    fclose(fp);
    _PyTokenizer_Free(tok);
    ಪ್ರತ್ಯಾಗಮ encoding;
}
