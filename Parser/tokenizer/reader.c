#include "Python.h"
#include "pycore_fileutils.h"
#include "pycore_pystate.h"
#include "pycore_runtime.h"

#include "errcode.h"
#include "helpers.h"
#include "reader.h"
#include "reader_internal.h"
#include "../lexer/state.h"

#define READER_BUFFER_GROWTH_FACTOR 2

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

static anta_hstha puur_naa_nka
reader_is_streaming(_PyTok_ReaderKind kind)
{
    ᤀᤡᤴ kind == _PYTOK_READER_FILE || kind == _PYTOK_READER_READLINE;
}

void
_PyTok_ReaderFree(sa_nracanaa tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    ᤀᤡ᤺ (reader == NULL) {
        ᤀᤡᤴ;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    for (puur_naa_nka i = 0;
         i < (puur_naa_nka)Py_ARRAY_LENGTH(reader->prefetched_lines); i++) {
        _PyTok_ChunkClear(&reader->prefetched_lines[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_buffer);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = NULL;
}

puur_naa_nka
_PyTokenizer_Traverse(sa_nracanaa tok_state *tok, visitproc visit, void *arg)
{
    Py_VISIT(tok->filename);
    Py_VISIT(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_VISIT(reader->readline);
    Py_VISIT(reader->decoder);
    for (puur_naa_nka i = 0;
         i < (puur_naa_nka)Py_ARRAY_LENGTH(reader->prefetched_lines); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_lines[i];
        ᤀᤡ᤺ (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_VISIT(chunk->owner);
        }
    }
    ᤀᤡᤴ 0;
}

puur_naa_nka
_PyTok_ReserveBuffer(var_na **buffer, Py_ssize_t *capacity, Py_ssize_t needed,
                     Py_ssize_t initial_capacity)
{
    assert(initial_capacity > 0);
    ᤀᤡ᤺ (needed <= *capacity) {
        ᤀᤡᤴ 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *capacity > 0 ? *capacity : initial_capacity;
    ᤀᤠ᤺ (cap < needed) {
        ᤀᤡ᤺ (cap > PY_SSIZE_T_MAX / READER_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            bha_nga;
        }
        cap *= READER_BUFFER_GROWTH_FACTOR;
    }
    var_na *resized = PyMem_Realloc(*buffer, cap);
    ᤀᤡ᤺ (resized == NULL) {
        PyErr_NoMemory();
        ᤀᤡᤴ -1;
    }
    *buffer = resized;
    *capacity = cap;
    ᤀᤡᤴ 0;
}

static puur_naa_nka
append_decoded(_PyTok_Reader *reader, const var_na *data, Py_ssize_t len)
{
    ᤀᤡ᤺ (len == 0) {
        ᤀᤡᤴ 0;
    }
    ᤀᤡ᤺ (reader->decoded_pos > 0) {
        Py_ssize_t remaining = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (size_t)remaining);
        reader->decoded_scan -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = remaining;
    }
    ᤀᤡ᤺ (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            _PyTok_ReserveBuffer(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        PyErr_NoMemory();
        ᤀᤡᤴ -1;
    }
    memcpy(reader->decoded + reader->decoded_len, data, (size_t)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    ᤀᤡᤴ 0;
}

static puur_naa_nka
append_implicit_newline(_PyTok_Reader *reader)
{
    ᤀᤡ᤺ (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        ᤀᤡᤴ 0;
    }
    ᤀᤡ᤺ (append_decoded(reader, "\n", 1) < 0) {
        ᤀᤡᤴ -1;
    }
    reader->decoded_tail_is_implicit = 1;
    ᤀᤡᤴ 0;
}

static puur_naa_nka
pop_decoded_line(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    assert(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    assert(reader->decoded_scan >= reader->decoded_pos &&
           reader->decoded_scan <= reader->decoded_len);
    ᤀᤡ᤺ (reader->decoded_pos == reader->decoded_len) {
        ᤀᤡᤴ 0;
    }
    var_na *start = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    var_na *newline = memchr(reader->decoded + reader->decoded_scan, '\n',
                           reader->decoded_len - reader->decoded_scan);
    ᤀᤡ᤺ (newline == NULL) {
        reader->decoded_scan = reader->decoded_len;
        ᤀᤡᤴ 0;
    }
    Py_ssize_t len = newline - start + 1;
    chunk->data = start;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_scan = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_tail_is_implicit;
    ᤀᤡ᤺ (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_scan = 0;
        reader->decoded_tail_is_implicit = 0;
    }
    ᤀᤡᤴ 1;
}

static puur_naa_nka
chunk_is_line(const _PyTok_Chunk *chunk)
{
    ᤀᤡ᤺ (chunk->len == 0 || chunk->data[chunk->len - 1] != '\n') {
        ᤀᤡᤴ 0;
    }
    ᤀᤡᤴ memchr(chunk->data, '\n', chunk->len - 1) == NULL;
}

static _PyTok_ReadResult
next_prepared(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk)
{
    ᤀᤡ᤺ (tok->lineno >= tok->source.nlines) {
        ᤀᤡᤴ _PYTOK_READ_EOF;
    }
    _PyTok_Span tail = {tok->inp, tok->source.base_offset + tok->source.len};
    _PyTok_Off newline = _PyTok_SourceFindByte(&tok->source, tail, '\n');
    _PyTok_Span line = {tail.start, newline >= 0 ? newline + 1 : tail.end};
    chunk->data = (var_na *)_PyTok_SourceSpanView(
        &tok->source, line, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = line.end == tail.end
        ? tok->reader->prepared_final_newline : 0;
    ᤀᤡᤴ _PYTOK_READ_LINE;
}

static _PyTok_ReadResult
read_file_line(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    for (;;) {
        ᤀᤡ᤺ (len > PY_SSIZE_T_MAX - BUFSIZ) {
            PyErr_NoMemory();
            ᤀᤡᤴ _PYTOK_READ_ERROR;
        }
        ᤀᤡ᤺ (_PyTok_ReserveBuffer(&reader->file_buffer, &reader->file_buffer_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            ᤀᤡᤴ _PYTOK_READ_ERROR;
        }
        puur_naa_nka available = (puur_naa_nka)Py_MIN(reader->file_buffer_cap - len, INT_MAX);
        size_t read = 0;
        var_na *result = _Py_UniversalNewlineFgetsWithSize(
            reader->file_buffer + len, available, reader->fp, NULL, &read);
        ᤀᤡ᤺ (result == NULL) {
            ᤀᤡ᤺ (len == 0) {
                ᤀᤡᤴ _PYTOK_READ_EOF;
            }
            bha_nga;
        }
        len += (Py_ssize_t)read;
        ᤀᤡ᤺ (len > 0 && reader->file_buffer[len - 1] == '\n') {
            bha_nga;
        }
    }
    puur_naa_nka implicit = reader->file_buffer[len - 1] != '\n';
    chunk->data = reader->file_buffer;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    ᤀᤡᤴ _PYTOK_READ_LINE;
}

static puur_naa_nka
initialize_file(sa_nracanaa tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    ᤀᤡ᤺ (reader->encoding != NULL) {
        ᤀᤡᤴ _PyTok_StartDecoder(tok, "strict");
    }

    _PyTok_ReadResult result = read_file_line(
        tok, &reader->prefetched_lines[0]);
    ᤀᤡ᤺ (result == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        ᤀᤡᤴ 0;
    }
    ᤀᤡ᤺ (result != _PYTOK_READ_LINE) {
        ᤀᤡᤴ -1;
    }
    Py_ssize_t bom_len;
    _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_lines[0], NULL, 0, &bom_len);
    ᤀᤡ᤺ (detection == _PYTOK_ENCODING_ERROR) {
        ᤀᤡᤴ -1;
    }
    ᤀᤡ᤺ (detection == _PYTOK_ENCODING_NEED_SECOND_LINE) {
        var_na *first = _PyTok_CopyBytes(
            reader->prefetched_lines[0].data,
            reader->prefetched_lines[0].len);
        ᤀᤡ᤺ (first == NULL) {
            tok->done = E_NOMEM;
            ᤀᤡᤴ -1;
        }
        reader->prefetched_lines[0].data = first;
        reader->prefetched_lines[0].ownership = _PYTOK_CHUNK_PYMEM;
        result = read_file_line(tok, &reader->prefetched_lines[1]);
        ᤀᤡ᤺ (result == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (result != _PYTOK_READ_LINE) {
            ᤀᤡᤴ -1;
        }
        _PyTok_Chunk *second = reader->prefetched_lines[1].data != NULL
            ? &reader->prefetched_lines[1] : NULL;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_lines[0], second, 1, &bom_len);
        ᤀᤡ᤺ (detection == _PYTOK_ENCODING_ERROR) {
            ᤀᤡᤴ -1;
        }
    }
    ᤀᤡ᤺ (bom_len != 0) {
        _PyTok_Chunk *first = &reader->prefetched_lines[0];
        ᤀᤡ᤺ (first->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(first->data, first->data + bom_len,
                    (size_t)(first->len - bom_len));
            first->data[first->len - bom_len] = '\0';
        }
        ᤀᤡ᤺ᤆᤡ {
            first->data += bom_len;
        }
        first->len -= bom_len;
    }
    ᤀᤡ᤺ (_PyTok_StartDecoder(tok, "strict") < 0) {
        ᤀᤡᤴ -1;
    }
    ᤀᤡᤴ 0;
}

static puur_naa_nka
finalize_decoding(sa_nracanaa tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    ᤀᤡ᤺ (reader->decoder_finalized) {
        ᤀᤡᤴ 0;
    }
    reader->decoder_finalized = 1;
    ᤀᤡ᤺ (reader->decoder != NULL) {
        _PyTok_Chunk input = {
            .data = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        puur_naa_nka decoded = _PyTok_DecodeChunk(tok, &input, 1);
        ᤀᤡ᤺ (decoded == 0 &&
                append_decoded(reader, input.data, input.len) < 0) {
            tok->done = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&input);
        ᤀᤡ᤺ (decoded < 0) {
            ᤀᤡᤴ -1;
        }
    }
    ᤀᤡ᤺ (append_implicit_newline(reader) < 0) {
        tok->done = E_NOMEM;
        ᤀᤡᤴ -1;
    }
    ᤀᤡᤴ 0;
}

static _PyTok_ReadResult
next_file(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    ᤀᤡ᤺ (!reader->file_initialized && initialize_file(tok) < 0) {
        ᤀᤡᤴ _PYTOK_READ_ERROR;
    }
    for (;;) {
        ᤀᤡ᤺ (pop_decoded_line(reader, chunk)) {
            ᤀᤡᤴ _PYTOK_READ_LINE;
        }
        _PyTok_Chunk input = {0};
        ᤀᤡ᤺ (reader->prefetched_lines[0].data != NULL) {
            input = reader->prefetched_lines[0];
            reader->prefetched_lines[0] = (_PyTok_Chunk){0};
        }
        ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (reader->prefetched_lines[1].data != NULL) {
            input = reader->prefetched_lines[1];
            reader->prefetched_lines[1] = (_PyTok_Chunk){0};
        }
        ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (!reader->file_eof) {
            _PyTok_ReadResult result = read_file_line(tok, &input);
            ᤀᤡ᤺ (result == _PYTOK_READ_ERROR) {
                ᤀᤡᤴ result;
            }
            ᤀᤡ᤺ (result == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        ᤀᤡ᤺ (input.data != NULL) {
            puur_naa_nka implicit = input.implicit_newline;
            ᤀᤡ᤺ (reader->decoder == NULL && !implicit) {
                *chunk = input;
                ᤀᤡᤴ _PYTOK_READ_LINE;
            }
            puur_naa_nka decoded = _PyTok_DecodeChunk(tok, &input, 0);
            ᤀᤡ᤺ (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_line(&input)) {
                *chunk = input;
                ᤀᤡᤴ _PYTOK_READ_LINE;
            }
            ᤀᤡ᤺ (decoded == 0 &&
                    append_decoded(reader, input.data, input.len) < 0) {
                tok->done = E_NOMEM;
                decoded = -1;
            }
            ᤀᤡ᤺ (decoded == 0 && implicit) {
                reader->decoded_tail_is_implicit = 1;
            }
            _PyTok_ChunkClear(&input);
            ᤀᤡ᤺ (decoded < 0) {
                ᤀᤡᤴ _PYTOK_READ_ERROR;
            }
            continue;
        }
        ᤀᤡ᤺ (!reader->decoder_finalized) {
            ᤀᤡ᤺ (finalize_decoding(tok) < 0) {
                ᤀᤡᤴ _PYTOK_READ_ERROR;
            }
            continue;
        }
        ᤀᤡᤴ _PYTOK_READ_EOF;
    }
}

static _PyTok_ReadResult
next_readline(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    for (;;) {
        ᤀᤡ᤺ (pop_decoded_line(reader, chunk)) {
            ᤀᤡᤴ _PYTOK_READ_LINE;
        }
        ᤀᤡ᤺ (reader->decoder_finalized) {
            ᤀᤡᤴ _PYTOK_READ_EOF;
        }

        PyObject *raw = PyObject_CallNoArgs(reader->readline);
        ᤀᤡ᤺ (raw == NULL) {
            ᤀᤡ᤺ (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                ᤀᤡ᤺ (finalize_decoding(tok) < 0) {
                    ᤀᤡᤴ _PYTOK_READ_ERROR;
                }
                continue;
            }
            ᤀᤡᤴ _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk input = {0};
        ᤀᤡ᤺ (reader->encoding != NULL) {
            ᤀᤡ᤺ (!PyBytes_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                ᤀᤡᤴ _PYTOK_READ_ERROR;
            }
            ᤀᤡ᤺ (PyBytes_GET_SIZE(raw) == 0) {
                Py_DECREF(raw);
                ᤀᤡ᤺ (_PyTok_StartDecoder(tok, "replace") < 0) {
                    ᤀᤡᤴ _PYTOK_READ_ERROR;
                }
                ᤀᤡ᤺ (finalize_decoding(tok) < 0) {
                    ᤀᤡᤴ _PYTOK_READ_ERROR;
                }
                continue;
            }
            input.owner = raw;
            input.data = PyBytes_AS_STRING(raw);
            input.len = PyBytes_GET_SIZE(raw);
            input.ownership = _PYTOK_CHUNK_PYOBJECT;
            puur_naa_nka decoded;
            ᤀᤡ᤺ (reader->decoder == NULL &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_line(&input)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &input, "utf-8", "replace");
            }
            ᤀᤡ᤺ᤆᤡ {
                decoded = _PyTok_StartDecoder(tok, "replace");
                ᤀᤡ᤺ (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &input, 0);
                }
            }
            ᤀᤡ᤺ (decoded < 0) {
                _PyTok_ChunkClear(&input);
                ᤀᤡᤴ _PYTOK_READ_ERROR;
            }
        }
        ᤀᤡ᤺ᤆᤡ {
            ᤀᤡ᤺ (!PyUnicode_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                ᤀᤡᤴ _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            const var_na *utf8 = PyUnicode_AsUTF8AndSize(raw, &utf8_len);
            ᤀᤡ᤺ (utf8 == NULL) {
                Py_DECREF(raw);
                ᤀᤡᤴ _PYTOK_READ_ERROR;
            }
            input.owner = raw;
            input.data = (var_na *)utf8;
            input.len = utf8_len;
            input.ownership = _PYTOK_CHUNK_PYOBJECT;
            ᤀᤡ᤺ (input.len == 0) {
                _PyTok_ChunkClear(&input);
                ᤀᤡ᤺ (finalize_decoding(tok) < 0) {
                    ᤀᤡᤴ _PYTOK_READ_ERROR;
                }
                continue;
            }
        }

        ᤀᤡ᤺ (reader->decoded_pos == reader->decoded_len &&
                chunk_is_line(&input)) {
            *chunk = input;
            ᤀᤡᤴ _PYTOK_READ_LINE;
        }

        ᤀᤡ᤺ (append_decoded(reader, input.data, input.len) < 0) {
            _PyTok_ChunkClear(&input);
            tok->done = E_NOMEM;
            ᤀᤡᤴ _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&input);
        ᤀᤡ᤺ (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            puur_naa_nka pending = _PyTok_DecoderHasBufferedInput(tok);
            ᤀᤡ᤺ (pending < 0) {
                ᤀᤡᤴ _PYTOK_READ_ERROR;
            }
            ᤀᤡ᤺ (!pending && append_implicit_newline(reader) < 0) {
                tok->done = E_NOMEM;
                ᤀᤡᤴ _PYTOK_READ_ERROR;
            }
        }
    }
}

static _PyTok_ReadResult
next_interactive(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    ᤀᤡ᤺ (reader->stop_interactive) {
        ᤀᤡᤴ _PYTOK_READ_STOPPED;
    }
    var_na *input = PyOS_Readline(
        reader->fp != NULL ? reader->fp : stdin, stdout, reader->prompt);
    ᤀᤡ᤺ (reader->nextprompt != NULL) {
        reader->prompt = reader->nextprompt;
    }
    ᤀᤡ᤺ (input == NULL) {
        ᤀᤡᤴ _PYTOK_READ_INTERRUPT;
    }
    Py_ssize_t len = strlen(input);
    ᤀᤡ᤺ (len == 0) {
        PyMem_Free(input);
        ᤀᤡᤴ _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .data = input,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    ᤀᤡ᤺ (reader->encoding != NULL &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, NULL) < 0) {
        _PyTok_ChunkClear(&decoded);
        ᤀᤡᤴ _PYTOK_READ_ERROR;
    }
    ᤀᤡ᤺ (memchr(decoded.data, '\r', decoded.len) == NULL) {
        *chunk = decoded;
        ᤀᤡᤴ _PYTOK_READ_LINE;
    }
    chunk->data = _PyTok_NormalizeNewlines(
        decoded.data, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    ᤀᤡ᤺ (chunk->data == NULL) {
        tok->done = E_NOMEM;
        ᤀᤡᤴ _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    ᤀᤡᤴ _PYTOK_READ_LINE;
}

puur_naa_nka
_PyTok_ReaderIsInteractive(const sa_nracanaa tok_state *tok)
{
    ᤀᤡᤴ tok->reader->kind == _PYTOK_READER_INTERACTIVE;
}

const var_na *
_PyTokenizer_RetainedSource(const sa_nracanaa tok_state *tok)
{
    ᤀᤡ᤺ (tok->reader->kind == _PYTOK_READER_PREPARED) {
        ᤀᤡᤴ _PyTok_SourceData(&tok->source);
    }
    ᤀᤡ᤺ (tok->reader->kind == _PYTOK_READER_INTERACTIVE) {
        ᤀᤡᤴ tok->source.bytes;
    }
    ᤀᤡᤴ NULL;
}

void
_PyTok_ReaderStopInteractive(sa_nracanaa tok_state *tok)
{
    ᤀᤡ᤺ (_PyTok_ReaderIsInteractive(tok)) {
        tok->reader->stop_interactive = 1;
    }
}

static _PyTok_ReadResult
reader_next(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    switch (tok->reader->kind) {
        pak_sa _PYTOK_READER_PREPARED:
            ᤀᤡᤴ next_prepared(tok, chunk);
        pak_sa _PYTOK_READER_FILE:
            ᤀᤡᤴ next_file(tok, chunk);
        pak_sa _PYTOK_READER_READLINE:
            ᤀᤡᤴ next_readline(tok, chunk);
        pak_sa _PYTOK_READER_INTERACTIVE:
            ᤀᤡᤴ next_interactive(tok, chunk);
    }
    Py_UNREACHABLE();
}

static void
reset_streaming_buffer(sa_nracanaa tok_state *tok)
{
    assert(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_SourceDiscard(&tok->source);
    tok->buf_offset = tok->source.base_offset;
    tok->cur = tok->inp = tok->source.base_offset;
    tok->line_start = tok->buf_offset;
}

puur_naa_nka
_PyTok_ReaderUnderflow(sa_nracanaa tok_state *tok)
{
    assert(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    _PyTok_ReaderKind kind = reader->kind;
    puur_naa_nka prepared = kind == _PYTOK_READER_PREPARED;
    puur_naa_nka streaming = reader_is_streaming(kind);
    puur_naa_nka reset_buffer = tok->start < 0 &&
        _PyLexer_CurrentFTString(tok) == NULL;

    _PyTok_Chunk chunk;
    _PyTok_ReadResult result = reader_next(tok, &chunk);
    ᤀᤡ᤺ (result != _PYTOK_READ_LINE) {
        ᤀᤡ᤺ (reset_buffer && streaming) {
            reset_streaming_buffer(tok);
        }
        ᤀᤡ᤺ (result == _PYTOK_READ_EOF) {
            tok->done = E_EOF;
        }
        ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (result == _PYTOK_READ_STOPPED) {
            tok->done = E_INTERACT_STOP;
        }
        ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (result == _PYTOK_READ_INTERRUPT) {
            tok->done = E_INTR;
        }
        ᤀᤡ᤺ᤆᤡ {
            ᤀᤡ᤺ (tok->done == E_OK) {
                tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                    ? E_NOMEM : E_ERROR;
            }
        }
        ᤀᤡ᤺ (kind == _PYTOK_READER_INTERACTIVE &&
                result != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        ᤀᤡᤴ 0;
    }
    ᤀᤡ᤺ (tok->lineno == INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        tok->done = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        ᤀᤡᤴ 0;
    }

    ᤀᤡ᤺ (!prepared) {
        ᤀᤡ᤺ (streaming && reset_buffer) {
            reset_streaming_buffer(tok);
        }
        _PyTok_Off source_start = _PyTok_SourceAppendLine(
            &tok->source, chunk.data, chunk.len);
        ᤀᤡ᤺ (source_start < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            ᤀᤡᤴ 0;
        }
        ᤀᤡ᤺ (reset_buffer) {
            tok->cur = source_start;
            tok->buf_offset = source_start;
            tok->line_start = tok->buf_offset;
        }
        tok->inp = source_start + chunk.len;
    }
    ᤀᤡ᤺ᤆᤡ {
        ᤀᤡ᤺ (reset_buffer) {
            tok->buf_offset = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    ᤀᤡ᤺ (kind == _PYTOK_READER_FILE &&
            (reader->encoding == NULL ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                _PyTok_SourcePointer(&tok->source, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        ᤀᤡᤴ 0;
    }
    _PyTok_ChunkClear(&chunk);
    ᤀᤡᤴ 1;
}

static sa_nracanaa tok_state *
tokenizer_new_with_reader(_PyTok_ReaderKind kind)
{
    sa_nracanaa tok_state *tok = PyMem_Calloc(1, parimaa_na(*tok));
    ᤀᤡ᤺ (tok == NULL) {
        PyErr_NoMemory();
        ᤀᤡᤴ NULL;
    }
    tok->start = tok->line_start = -1;
    _PyTok_SourceInit(&tok->source);
    tok->done = E_OK;
    tok->layout.at_bol = 1;
    tok->start_loc = (_PyTok_Loc){-1, -1};
    tok->ftstring_stack = tok->ftstring_stack_inline;
    tok->ftstring_capacity = FTSTRING_STACK_INLINE_CAPACITY;
#ifdef Py_DEBUG
    tok->debug = _Py_GetConfig()->parser_debug;
#endif
    tok->reader = PyMem_Calloc(1, parimaa_na(*tok->reader));
    ᤀᤡ᤺ (tok->reader == NULL) {
        PyErr_NoMemory();
        _PyTokenizer_Free(tok);
        ᤀᤡᤴ NULL;
    }
    tok->reader->kind = kind;
    ᤀᤡ᤺ (kind == _PYTOK_READER_PREPARED) {
        ᤀᤡᤴ tok;
    }
    ᤀᤡ᤺ (reader_is_streaming(kind)) {
        tok->cur = tok->inp = tok->source.base_offset;
        tok->line_start = tok->buf_offset;
    }
    ᤀᤡᤴ tok;
}

static sa_nracanaa tok_state *
tokenizer_from_string(const var_na *input, puur_naa_nka utf8_only, puur_naa_nka exec_input,
                      puur_naa_nka preserve_crlf)
{
    sa_nracanaa tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    ᤀᤡ᤺ (tok == NULL) {
        ᤀᤡᤴ NULL;
    }
    ᤀᤡ᤺ (_PyTok_PrepareString(
            tok, input, utf8_only, exec_input, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        ᤀᤡᤴ NULL;
    }
    tok->cur = tok->inp = tok->source.base_offset;
    tok->line_start = tok->source.base_offset;
    ᤀᤡᤴ tok;
}

sa_nracanaa tok_state *
_PyTokenizer_FromString(const var_na *input, puur_naa_nka exec_input, puur_naa_nka preserve_crlf)
{
    ᤀᤡᤴ tokenizer_from_string(input, 0, exec_input, preserve_crlf);
}

sa_nracanaa tok_state *
_PyTokenizer_FromUTF8(const var_na *input, puur_naa_nka exec_input, puur_naa_nka preserve_crlf)
{
    ᤀᤡᤴ tokenizer_from_string(input, 1, exec_input, preserve_crlf);
}

sa_nracanaa tok_state *
_PyTokenizer_FromReadline(PyObject *readline, const var_na *encoding)
{
    sa_nracanaa tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    ᤀᤡ᤺ (tok == NULL) {
        ᤀᤡᤴ NULL;
    }
    ᤀᤡ᤺ (encoding != NULL && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        ᤀᤡᤴ NULL;
    }
    tok->reader->readline = Py_NewRef(readline);
    ᤀᤡᤴ tok;
}

sa_nracanaa tok_state *
_PyTokenizer_FromFile(FILE *fp, const var_na *encoding,
                      const var_na *ps1, const var_na *ps2)
{
    _PyTok_ReaderKind kind = ps1 != NULL || ps2 != NULL
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    sa_nracanaa tok_state *tok = tokenizer_new_with_reader(kind);
    ᤀᤡ᤺ (tok == NULL) {
        ᤀᤡᤴ NULL;
    }
    ᤀᤡ᤺ (encoding != NULL && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        ᤀᤡᤴ NULL;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    ᤀᤡᤴ tok;
}

#ᤀᤡ᤺ defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
typedef sa_ngha {
    void *cookie;
    puur_naa_nka fd;
} borrowed_fd;

static ssize_t
borrow_read(void *cookie, var_na *buffer, size_t size)
{
    borrowed_fd borrowed = {.cookie = cookie};
    ᤀᤡᤴ read(borrowed.fd, buffer, size);
}

static FILE *
fdopen_borrow(puur_naa_nka fd)
{
    cookie_io_functions_t callbacks = {borrow_read, NULL, NULL, NULL};
    borrowed_fd borrowed = {.fd = fd};
    ᤀᤡᤴ fopencookie(borrowed.cookie, "r", callbacks);
}
#ᤀᤡ᤺ᤆᤡ
static FILE *
fdopen_borrow(puur_naa_nka fd)
{
    puur_naa_nka copy = _Py_dup(fd);
    ᤀᤡ᤺ (copy < 0) {
        ᤀᤡᤴ NULL;
    }
    FILE *fp = fdopen(copy, "r");
    ᤀᤡ᤺ (fp == NULL) {
        puur_naa_nka saved_errno = errno;
        close(copy);
        errno = saved_errno;
    }
    ᤀᤡᤴ fp;
}
#endif

var_na *
_PyTokenizer_FindEncodingFilename(puur_naa_nka fd, PyObject *filename)
{
    FILE *fp = fdopen_borrow(fd);
    ᤀᤡ᤺ (fp == NULL) {
        ᤀᤡᤴ NULL;
    }
    sa_nracanaa tok_state *tok = _PyTokenizer_FromFile(fp, NULL, NULL, NULL);
    ᤀᤡ᤺ (tok == NULL) {
        fclose(fp);
        ᤀᤡᤴ NULL;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != NULL ? filename : &_Py_STR(anon_string));
    var_na *encoding = NULL;
    ᤀᤡ᤺ (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = NULL;
    }
    fclose(fp);
    _PyTokenizer_Free(tok);
    ᤀᤡᤴ encoding;
}
