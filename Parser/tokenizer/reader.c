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
    ꦮꦭꦶ kind == _PYTOK_READER_FILE || kind == _PYTOK_READER_READLINE;
}

void
_PyTok_ReaderFree(sa_nracanaa tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    ꦪꦼꦤ꧀ (reader == NULL) {
        ꦮꦭꦶ;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0;
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
    ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0;
         i < (puur_naa_nka)Py_ARRAY_LENGTH(reader->prefetched_lines); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_lines[i];
        ꦪꦼꦤ꧀ (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_VISIT(chunk->owner);
        }
    }
    ꦮꦭꦶ 0;
}

puur_naa_nka
_PyTok_ReserveBuffer(var_na **buffer, Py_ssize_t *capacity, Py_ssize_t needed,
                     Py_ssize_t initial_capacity)
{
    assert(initial_capacity > 0);
    ꦪꦼꦤ꧀ (needed <= *capacity) {
        ꦮꦭꦶ 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *capacity > 0 ? *capacity : initial_capacity;
    ꦱꦼꦢꦼꦭ (cap < needed) {
        ꦪꦼꦤ꧀ (cap > PY_SSIZE_T_MAX / READER_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            bha_nga;
        }
        cap *= READER_BUFFER_GROWTH_FACTOR;
    }
    var_na *resized = PyMem_Realloc(*buffer, cap);
    ꦪꦼꦤ꧀ (resized == NULL) {
        PyErr_NoMemory();
        ꦮꦭꦶ -1;
    }
    *buffer = resized;
    *capacity = cap;
    ꦮꦭꦶ 0;
}

static puur_naa_nka
append_decoded(_PyTok_Reader *reader, const var_na *data, Py_ssize_t len)
{
    ꦪꦼꦤ꧀ (len == 0) {
        ꦮꦭꦶ 0;
    }
    ꦪꦼꦤ꧀ (reader->decoded_pos > 0) {
        Py_ssize_t remaining = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (size_t)remaining);
        reader->decoded_scan -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = remaining;
    }
    ꦪꦼꦤ꧀ (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            _PyTok_ReserveBuffer(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        PyErr_NoMemory();
        ꦮꦭꦶ -1;
    }
    memcpy(reader->decoded + reader->decoded_len, data, (size_t)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    ꦮꦭꦶ 0;
}

static puur_naa_nka
append_implicit_newline(_PyTok_Reader *reader)
{
    ꦪꦼꦤ꧀ (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        ꦮꦭꦶ 0;
    }
    ꦪꦼꦤ꧀ (append_decoded(reader, "\n", 1) < 0) {
        ꦮꦭꦶ -1;
    }
    reader->decoded_tail_is_implicit = 1;
    ꦮꦭꦶ 0;
}

static puur_naa_nka
pop_decoded_line(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    assert(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    assert(reader->decoded_scan >= reader->decoded_pos &&
           reader->decoded_scan <= reader->decoded_len);
    ꦪꦼꦤ꧀ (reader->decoded_pos == reader->decoded_len) {
        ꦮꦭꦶ 0;
    }
    var_na *start = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    var_na *newline = memchr(reader->decoded + reader->decoded_scan, '\n',
                           reader->decoded_len - reader->decoded_scan);
    ꦪꦼꦤ꧀ (newline == NULL) {
        reader->decoded_scan = reader->decoded_len;
        ꦮꦭꦶ 0;
    }
    Py_ssize_t len = newline - start + 1;
    chunk->data = start;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_scan = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_tail_is_implicit;
    ꦪꦼꦤ꧀ (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_scan = 0;
        reader->decoded_tail_is_implicit = 0;
    }
    ꦮꦭꦶ 1;
}

static puur_naa_nka
chunk_is_line(const _PyTok_Chunk *chunk)
{
    ꦪꦼꦤ꧀ (chunk->len == 0 || chunk->data[chunk->len - 1] != '\n') {
        ꦮꦭꦶ 0;
    }
    ꦮꦭꦶ memchr(chunk->data, '\n', chunk->len - 1) == NULL;
}

static _PyTok_ReadResult
next_prepared(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk)
{
    ꦪꦼꦤ꧀ (tok->lineno >= tok->source.nlines) {
        ꦮꦭꦶ _PYTOK_READ_EOF;
    }
    _PyTok_Span tail = {tok->inp, tok->source.base_offset + tok->source.len};
    _PyTok_Off newline = _PyTok_SourceFindByte(&tok->source, tail, '\n');
    _PyTok_Span line = {tail.start, newline >= 0 ? newline + 1 : tail.end};
    chunk->data = (var_na *)_PyTok_SourceSpanView(
        &tok->source, line, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = line.end == tail.end
        ? tok->reader->prepared_final_newline : 0;
    ꦮꦭꦶ _PYTOK_READ_LINE;
}

static _PyTok_ReadResult
read_file_line(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    ꦏꦁꦒꦺꦴ (;;) {
        ꦪꦼꦤ꧀ (len > PY_SSIZE_T_MAX - BUFSIZ) {
            PyErr_NoMemory();
            ꦮꦭꦶ _PYTOK_READ_ERROR;
        }
        ꦪꦼꦤ꧀ (_PyTok_ReserveBuffer(&reader->file_buffer, &reader->file_buffer_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            ꦮꦭꦶ _PYTOK_READ_ERROR;
        }
        puur_naa_nka available = (puur_naa_nka)Py_MIN(reader->file_buffer_cap - len, INT_MAX);
        size_t read = 0;
        var_na *result = _Py_UniversalNewlineFgetsWithSize(
            reader->file_buffer + len, available, reader->fp, NULL, &read);
        ꦪꦼꦤ꧀ (result == NULL) {
            ꦪꦼꦤ꧀ (len == 0) {
                ꦮꦭꦶ _PYTOK_READ_EOF;
            }
            bha_nga;
        }
        len += (Py_ssize_t)read;
        ꦪꦼꦤ꧀ (len > 0 && reader->file_buffer[len - 1] == '\n') {
            bha_nga;
        }
    }
    puur_naa_nka implicit = reader->file_buffer[len - 1] != '\n';
    chunk->data = reader->file_buffer;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    ꦮꦭꦶ _PYTOK_READ_LINE;
}

static puur_naa_nka
initialize_file(sa_nracanaa tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    ꦪꦼꦤ꧀ (reader->encoding != NULL) {
        ꦮꦭꦶ _PyTok_StartDecoder(tok, "strict");
    }

    _PyTok_ReadResult result = read_file_line(
        tok, &reader->prefetched_lines[0]);
    ꦪꦼꦤ꧀ (result == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        ꦮꦭꦶ 0;
    }
    ꦪꦼꦤ꧀ (result != _PYTOK_READ_LINE) {
        ꦮꦭꦶ -1;
    }
    Py_ssize_t bom_len;
    _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_lines[0], NULL, 0, &bom_len);
    ꦪꦼꦤ꧀ (detection == _PYTOK_ENCODING_ERROR) {
        ꦮꦭꦶ -1;
    }
    ꦪꦼꦤ꧀ (detection == _PYTOK_ENCODING_NEED_SECOND_LINE) {
        var_na *first = _PyTok_CopyBytes(
            reader->prefetched_lines[0].data,
            reader->prefetched_lines[0].len);
        ꦪꦼꦤ꧀ (first == NULL) {
            tok->done = E_NOMEM;
            ꦮꦭꦶ -1;
        }
        reader->prefetched_lines[0].data = first;
        reader->prefetched_lines[0].ownership = _PYTOK_CHUNK_PYMEM;
        result = read_file_line(tok, &reader->prefetched_lines[1]);
        ꦪꦼꦤ꧀ (result == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        ꦭꦶꦪ ꦪꦼꦤ꧀ (result != _PYTOK_READ_LINE) {
            ꦮꦭꦶ -1;
        }
        _PyTok_Chunk *second = reader->prefetched_lines[1].data != NULL
            ? &reader->prefetched_lines[1] : NULL;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_lines[0], second, 1, &bom_len);
        ꦪꦼꦤ꧀ (detection == _PYTOK_ENCODING_ERROR) {
            ꦮꦭꦶ -1;
        }
    }
    ꦪꦼꦤ꧀ (bom_len != 0) {
        _PyTok_Chunk *first = &reader->prefetched_lines[0];
        ꦪꦼꦤ꧀ (first->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(first->data, first->data + bom_len,
                    (size_t)(first->len - bom_len));
            first->data[first->len - bom_len] = '\0';
        }
        ꦭꦶꦪ {
            first->data += bom_len;
        }
        first->len -= bom_len;
    }
    ꦪꦼꦤ꧀ (_PyTok_StartDecoder(tok, "strict") < 0) {
        ꦮꦭꦶ -1;
    }
    ꦮꦭꦶ 0;
}

static puur_naa_nka
finalize_decoding(sa_nracanaa tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    ꦪꦼꦤ꧀ (reader->decoder_finalized) {
        ꦮꦭꦶ 0;
    }
    reader->decoder_finalized = 1;
    ꦪꦼꦤ꧀ (reader->decoder != NULL) {
        _PyTok_Chunk input = {
            .data = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        puur_naa_nka decoded = _PyTok_DecodeChunk(tok, &input, 1);
        ꦪꦼꦤ꧀ (decoded == 0 &&
                append_decoded(reader, input.data, input.len) < 0) {
            tok->done = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&input);
        ꦪꦼꦤ꧀ (decoded < 0) {
            ꦮꦭꦶ -1;
        }
    }
    ꦪꦼꦤ꧀ (append_implicit_newline(reader) < 0) {
        tok->done = E_NOMEM;
        ꦮꦭꦶ -1;
    }
    ꦮꦭꦶ 0;
}

static _PyTok_ReadResult
next_file(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    ꦪꦼꦤ꧀ (!reader->file_initialized && initialize_file(tok) < 0) {
        ꦮꦭꦶ _PYTOK_READ_ERROR;
    }
    ꦏꦁꦒꦺꦴ (;;) {
        ꦪꦼꦤ꧀ (pop_decoded_line(reader, chunk)) {
            ꦮꦭꦶ _PYTOK_READ_LINE;
        }
        _PyTok_Chunk input = {0};
        ꦪꦼꦤ꧀ (reader->prefetched_lines[0].data != NULL) {
            input = reader->prefetched_lines[0];
            reader->prefetched_lines[0] = (_PyTok_Chunk){0};
        }
        ꦭꦶꦪ ꦪꦼꦤ꧀ (reader->prefetched_lines[1].data != NULL) {
            input = reader->prefetched_lines[1];
            reader->prefetched_lines[1] = (_PyTok_Chunk){0};
        }
        ꦭꦶꦪ ꦪꦼꦤ꧀ (!reader->file_eof) {
            _PyTok_ReadResult result = read_file_line(tok, &input);
            ꦪꦼꦤ꧀ (result == _PYTOK_READ_ERROR) {
                ꦮꦭꦶ result;
            }
            ꦪꦼꦤ꧀ (result == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        ꦪꦼꦤ꧀ (input.data != NULL) {
            puur_naa_nka implicit = input.implicit_newline;
            ꦪꦼꦤ꧀ (reader->decoder == NULL && !implicit) {
                *chunk = input;
                ꦮꦭꦶ _PYTOK_READ_LINE;
            }
            puur_naa_nka decoded = _PyTok_DecodeChunk(tok, &input, 0);
            ꦪꦼꦤ꧀ (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_line(&input)) {
                *chunk = input;
                ꦮꦭꦶ _PYTOK_READ_LINE;
            }
            ꦪꦼꦤ꧀ (decoded == 0 &&
                    append_decoded(reader, input.data, input.len) < 0) {
                tok->done = E_NOMEM;
                decoded = -1;
            }
            ꦪꦼꦤ꧀ (decoded == 0 && implicit) {
                reader->decoded_tail_is_implicit = 1;
            }
            _PyTok_ChunkClear(&input);
            ꦪꦼꦤ꧀ (decoded < 0) {
                ꦮꦭꦶ _PYTOK_READ_ERROR;
            }
            continue;
        }
        ꦪꦼꦤ꧀ (!reader->decoder_finalized) {
            ꦪꦼꦤ꧀ (finalize_decoding(tok) < 0) {
                ꦮꦭꦶ _PYTOK_READ_ERROR;
            }
            continue;
        }
        ꦮꦭꦶ _PYTOK_READ_EOF;
    }
}

static _PyTok_ReadResult
next_readline(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    ꦏꦁꦒꦺꦴ (;;) {
        ꦪꦼꦤ꧀ (pop_decoded_line(reader, chunk)) {
            ꦮꦭꦶ _PYTOK_READ_LINE;
        }
        ꦪꦼꦤ꧀ (reader->decoder_finalized) {
            ꦮꦭꦶ _PYTOK_READ_EOF;
        }

        PyObject *raw = PyObject_CallNoArgs(reader->readline);
        ꦪꦼꦤ꧀ (raw == NULL) {
            ꦪꦼꦤ꧀ (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                ꦪꦼꦤ꧀ (finalize_decoding(tok) < 0) {
                    ꦮꦭꦶ _PYTOK_READ_ERROR;
                }
                continue;
            }
            ꦮꦭꦶ _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk input = {0};
        ꦪꦼꦤ꧀ (reader->encoding != NULL) {
            ꦪꦼꦤ꧀ (!PyBytes_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                ꦮꦭꦶ _PYTOK_READ_ERROR;
            }
            ꦪꦼꦤ꧀ (PyBytes_GET_SIZE(raw) == 0) {
                Py_DECREF(raw);
                ꦪꦼꦤ꧀ (_PyTok_StartDecoder(tok, "replace") < 0) {
                    ꦮꦭꦶ _PYTOK_READ_ERROR;
                }
                ꦪꦼꦤ꧀ (finalize_decoding(tok) < 0) {
                    ꦮꦭꦶ _PYTOK_READ_ERROR;
                }
                continue;
            }
            input.owner = raw;
            input.data = PyBytes_AS_STRING(raw);
            input.len = PyBytes_GET_SIZE(raw);
            input.ownership = _PYTOK_CHUNK_PYOBJECT;
            puur_naa_nka decoded;
            ꦪꦼꦤ꧀ (reader->decoder == NULL &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_line(&input)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &input, "utf-8", "replace");
            }
            ꦭꦶꦪ {
                decoded = _PyTok_StartDecoder(tok, "replace");
                ꦪꦼꦤ꧀ (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &input, 0);
                }
            }
            ꦪꦼꦤ꧀ (decoded < 0) {
                _PyTok_ChunkClear(&input);
                ꦮꦭꦶ _PYTOK_READ_ERROR;
            }
        }
        ꦭꦶꦪ {
            ꦪꦼꦤ꧀ (!PyUnicode_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                ꦮꦭꦶ _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            const var_na *utf8 = PyUnicode_AsUTF8AndSize(raw, &utf8_len);
            ꦪꦼꦤ꧀ (utf8 == NULL) {
                Py_DECREF(raw);
                ꦮꦭꦶ _PYTOK_READ_ERROR;
            }
            input.owner = raw;
            input.data = (var_na *)utf8;
            input.len = utf8_len;
            input.ownership = _PYTOK_CHUNK_PYOBJECT;
            ꦪꦼꦤ꧀ (input.len == 0) {
                _PyTok_ChunkClear(&input);
                ꦪꦼꦤ꧀ (finalize_decoding(tok) < 0) {
                    ꦮꦭꦶ _PYTOK_READ_ERROR;
                }
                continue;
            }
        }

        ꦪꦼꦤ꧀ (reader->decoded_pos == reader->decoded_len &&
                chunk_is_line(&input)) {
            *chunk = input;
            ꦮꦭꦶ _PYTOK_READ_LINE;
        }

        ꦪꦼꦤ꧀ (append_decoded(reader, input.data, input.len) < 0) {
            _PyTok_ChunkClear(&input);
            tok->done = E_NOMEM;
            ꦮꦭꦶ _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&input);
        ꦪꦼꦤ꧀ (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            puur_naa_nka pending = _PyTok_DecoderHasBufferedInput(tok);
            ꦪꦼꦤ꧀ (pending < 0) {
                ꦮꦭꦶ _PYTOK_READ_ERROR;
            }
            ꦪꦼꦤ꧀ (!pending && append_implicit_newline(reader) < 0) {
                tok->done = E_NOMEM;
                ꦮꦭꦶ _PYTOK_READ_ERROR;
            }
        }
    }
}

static _PyTok_ReadResult
next_interactive(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    ꦪꦼꦤ꧀ (reader->stop_interactive) {
        ꦮꦭꦶ _PYTOK_READ_STOPPED;
    }
    var_na *input = PyOS_Readline(
        reader->fp != NULL ? reader->fp : stdin, stdout, reader->prompt);
    ꦪꦼꦤ꧀ (reader->nextprompt != NULL) {
        reader->prompt = reader->nextprompt;
    }
    ꦪꦼꦤ꧀ (input == NULL) {
        ꦮꦭꦶ _PYTOK_READ_INTERRUPT;
    }
    Py_ssize_t len = strlen(input);
    ꦪꦼꦤ꧀ (len == 0) {
        PyMem_Free(input);
        ꦮꦭꦶ _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .data = input,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    ꦪꦼꦤ꧀ (reader->encoding != NULL &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, NULL) < 0) {
        _PyTok_ChunkClear(&decoded);
        ꦮꦭꦶ _PYTOK_READ_ERROR;
    }
    ꦪꦼꦤ꧀ (memchr(decoded.data, '\r', decoded.len) == NULL) {
        *chunk = decoded;
        ꦮꦭꦶ _PYTOK_READ_LINE;
    }
    chunk->data = _PyTok_NormalizeNewlines(
        decoded.data, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    ꦪꦼꦤ꧀ (chunk->data == NULL) {
        tok->done = E_NOMEM;
        ꦮꦭꦶ _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    ꦮꦭꦶ _PYTOK_READ_LINE;
}

puur_naa_nka
_PyTok_ReaderIsInteractive(const sa_nracanaa tok_state *tok)
{
    ꦮꦭꦶ tok->reader->kind == _PYTOK_READER_INTERACTIVE;
}

const var_na *
_PyTokenizer_RetainedSource(const sa_nracanaa tok_state *tok)
{
    ꦪꦼꦤ꧀ (tok->reader->kind == _PYTOK_READER_PREPARED) {
        ꦮꦭꦶ _PyTok_SourceData(&tok->source);
    }
    ꦪꦼꦤ꧀ (tok->reader->kind == _PYTOK_READER_INTERACTIVE) {
        ꦮꦭꦶ tok->source.bytes;
    }
    ꦮꦭꦶ NULL;
}

void
_PyTok_ReaderStopInteractive(sa_nracanaa tok_state *tok)
{
    ꦪꦼꦤ꧀ (_PyTok_ReaderIsInteractive(tok)) {
        tok->reader->stop_interactive = 1;
    }
}

static _PyTok_ReadResult
reader_next(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    switch (tok->reader->kind) {
        pak_sa _PYTOK_READER_PREPARED:
            ꦮꦭꦶ next_prepared(tok, chunk);
        pak_sa _PYTOK_READER_FILE:
            ꦮꦭꦶ next_file(tok, chunk);
        pak_sa _PYTOK_READER_READLINE:
            ꦮꦭꦶ next_readline(tok, chunk);
        pak_sa _PYTOK_READER_INTERACTIVE:
            ꦮꦭꦶ next_interactive(tok, chunk);
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
    ꦪꦼꦤ꧀ (result != _PYTOK_READ_LINE) {
        ꦪꦼꦤ꧀ (reset_buffer && streaming) {
            reset_streaming_buffer(tok);
        }
        ꦪꦼꦤ꧀ (result == _PYTOK_READ_EOF) {
            tok->done = E_EOF;
        }
        ꦭꦶꦪ ꦪꦼꦤ꧀ (result == _PYTOK_READ_STOPPED) {
            tok->done = E_INTERACT_STOP;
        }
        ꦭꦶꦪ ꦪꦼꦤ꧀ (result == _PYTOK_READ_INTERRUPT) {
            tok->done = E_INTR;
        }
        ꦭꦶꦪ {
            ꦪꦼꦤ꧀ (tok->done == E_OK) {
                tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                    ? E_NOMEM : E_ERROR;
            }
        }
        ꦪꦼꦤ꧀ (kind == _PYTOK_READER_INTERACTIVE &&
                result != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        ꦮꦭꦶ 0;
    }
    ꦪꦼꦤ꧀ (tok->lineno == INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        tok->done = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        ꦮꦭꦶ 0;
    }

    ꦪꦼꦤ꧀ (!prepared) {
        ꦪꦼꦤ꧀ (streaming && reset_buffer) {
            reset_streaming_buffer(tok);
        }
        _PyTok_Off source_start = _PyTok_SourceAppendLine(
            &tok->source, chunk.data, chunk.len);
        ꦪꦼꦤ꧀ (source_start < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            ꦮꦭꦶ 0;
        }
        ꦪꦼꦤ꧀ (reset_buffer) {
            tok->cur = source_start;
            tok->buf_offset = source_start;
            tok->line_start = tok->buf_offset;
        }
        tok->inp = source_start + chunk.len;
    }
    ꦭꦶꦪ {
        ꦪꦼꦤ꧀ (reset_buffer) {
            tok->buf_offset = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    ꦪꦼꦤ꧀ (kind == _PYTOK_READER_FILE &&
            (reader->encoding == NULL ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                _PyTok_SourcePointer(&tok->source, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        ꦮꦭꦶ 0;
    }
    _PyTok_ChunkClear(&chunk);
    ꦮꦭꦶ 1;
}

static sa_nracanaa tok_state *
tokenizer_new_with_reader(_PyTok_ReaderKind kind)
{
    sa_nracanaa tok_state *tok = PyMem_Calloc(1, parimaa_na(*tok));
    ꦪꦼꦤ꧀ (tok == NULL) {
        PyErr_NoMemory();
        ꦮꦭꦶ NULL;
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
    ꦪꦼꦤ꧀ (tok->reader == NULL) {
        PyErr_NoMemory();
        _PyTokenizer_Free(tok);
        ꦮꦭꦶ NULL;
    }
    tok->reader->kind = kind;
    ꦪꦼꦤ꧀ (kind == _PYTOK_READER_PREPARED) {
        ꦮꦭꦶ tok;
    }
    ꦪꦼꦤ꧀ (reader_is_streaming(kind)) {
        tok->cur = tok->inp = tok->source.base_offset;
        tok->line_start = tok->buf_offset;
    }
    ꦮꦭꦶ tok;
}

static sa_nracanaa tok_state *
tokenizer_from_string(const var_na *input, puur_naa_nka utf8_only, puur_naa_nka exec_input,
                      puur_naa_nka preserve_crlf)
{
    sa_nracanaa tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    ꦪꦼꦤ꧀ (tok == NULL) {
        ꦮꦭꦶ NULL;
    }
    ꦪꦼꦤ꧀ (_PyTok_PrepareString(
            tok, input, utf8_only, exec_input, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        ꦮꦭꦶ NULL;
    }
    tok->cur = tok->inp = tok->source.base_offset;
    tok->line_start = tok->source.base_offset;
    ꦮꦭꦶ tok;
}

sa_nracanaa tok_state *
_PyTokenizer_FromString(const var_na *input, puur_naa_nka exec_input, puur_naa_nka preserve_crlf)
{
    ꦮꦭꦶ tokenizer_from_string(input, 0, exec_input, preserve_crlf);
}

sa_nracanaa tok_state *
_PyTokenizer_FromUTF8(const var_na *input, puur_naa_nka exec_input, puur_naa_nka preserve_crlf)
{
    ꦮꦭꦶ tokenizer_from_string(input, 1, exec_input, preserve_crlf);
}

sa_nracanaa tok_state *
_PyTokenizer_FromReadline(PyObject *readline, const var_na *encoding)
{
    sa_nracanaa tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    ꦪꦼꦤ꧀ (tok == NULL) {
        ꦮꦭꦶ NULL;
    }
    ꦪꦼꦤ꧀ (encoding != NULL && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        ꦮꦭꦶ NULL;
    }
    tok->reader->readline = Py_NewRef(readline);
    ꦮꦭꦶ tok;
}

sa_nracanaa tok_state *
_PyTokenizer_FromFile(FILE *fp, const var_na *encoding,
                      const var_na *ps1, const var_na *ps2)
{
    _PyTok_ReaderKind kind = ps1 != NULL || ps2 != NULL
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    sa_nracanaa tok_state *tok = tokenizer_new_with_reader(kind);
    ꦪꦼꦤ꧀ (tok == NULL) {
        ꦮꦭꦶ NULL;
    }
    ꦪꦼꦤ꧀ (encoding != NULL && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        ꦮꦭꦶ NULL;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    ꦮꦭꦶ tok;
}

#ꦪꦼꦤ꧀ defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
typedef sa_ngha {
    void *cookie;
    puur_naa_nka fd;
} borrowed_fd;

static ssize_t
borrow_read(void *cookie, var_na *buffer, size_t size)
{
    borrowed_fd borrowed = {.cookie = cookie};
    ꦮꦭꦶ read(borrowed.fd, buffer, size);
}

static FILE *
fdopen_borrow(puur_naa_nka fd)
{
    cookie_io_functions_t callbacks = {borrow_read, NULL, NULL, NULL};
    borrowed_fd borrowed = {.fd = fd};
    ꦮꦭꦶ fopencookie(borrowed.cookie, "r", callbacks);
}
#ꦭꦶꦪ
static FILE *
fdopen_borrow(puur_naa_nka fd)
{
    puur_naa_nka copy = _Py_dup(fd);
    ꦪꦼꦤ꧀ (copy < 0) {
        ꦮꦭꦶ NULL;
    }
    FILE *fp = fdopen(copy, "r");
    ꦪꦼꦤ꧀ (fp == NULL) {
        puur_naa_nka saved_errno = errno;
        close(copy);
        errno = saved_errno;
    }
    ꦮꦭꦶ fp;
}
#endif

var_na *
_PyTokenizer_FindEncodingFilename(puur_naa_nka fd, PyObject *filename)
{
    FILE *fp = fdopen_borrow(fd);
    ꦪꦼꦤ꧀ (fp == NULL) {
        ꦮꦭꦶ NULL;
    }
    sa_nracanaa tok_state *tok = _PyTokenizer_FromFile(fp, NULL, NULL, NULL);
    ꦪꦼꦤ꧀ (tok == NULL) {
        fclose(fp);
        ꦮꦭꦶ NULL;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != NULL ? filename : &_Py_STR(anon_string));
    var_na *encoding = NULL;
    ꦪꦼꦤ꧀ (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = NULL;
    }
    fclose(fp);
    _PyTokenizer_Free(tok);
    ꦮꦭꦶ encoding;
}
