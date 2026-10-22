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

static anta_hstha จำนวนเต็ม
reader_is_streaming(_PyTok_ReaderKind kind)
{
    คืน kind == _PYTOK_READER_FILE || kind == _PYTOK_READER_READLINE;
}

void
_PyTok_ReaderFree(sa_nracanaa tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    ถ้า (reader == NULL) {
        คืน;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    สำหรับ (จำนวนเต็ม i = 0;
         i < (จำนวนเต็ม)Py_ARRAY_LENGTH(reader->prefetched_lines); i++) {
        _PyTok_ChunkClear(&reader->prefetched_lines[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_buffer);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = NULL;
}

จำนวนเต็ม
_PyTokenizer_Traverse(sa_nracanaa tok_state *tok, visitproc visit, void *arg)
{
    Py_VISIT(tok->filename);
    Py_VISIT(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_VISIT(reader->readline);
    Py_VISIT(reader->decoder);
    สำหรับ (จำนวนเต็ม i = 0;
         i < (จำนวนเต็ม)Py_ARRAY_LENGTH(reader->prefetched_lines); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_lines[i];
        ถ้า (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_VISIT(chunk->owner);
        }
    }
    คืน 0;
}

จำนวนเต็ม
_PyTok_ReserveBuffer(var_na **buffer, Py_ssize_t *capacity, Py_ssize_t needed,
                     Py_ssize_t initial_capacity)
{
    assert(initial_capacity > 0);
    ถ้า (needed <= *capacity) {
        คืน 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *capacity > 0 ? *capacity : initial_capacity;
    ขณะ (cap < needed) {
        ถ้า (cap > PY_SSIZE_T_MAX / READER_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            bha_nga;
        }
        cap *= READER_BUFFER_GROWTH_FACTOR;
    }
    var_na *resized = PyMem_Realloc(*buffer, cap);
    ถ้า (resized == NULL) {
        PyErr_NoMemory();
        คืน -1;
    }
    *buffer = resized;
    *capacity = cap;
    คืน 0;
}

static จำนวนเต็ม
append_decoded(_PyTok_Reader *reader, const var_na *data, Py_ssize_t len)
{
    ถ้า (len == 0) {
        คืน 0;
    }
    ถ้า (reader->decoded_pos > 0) {
        Py_ssize_t remaining = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (size_t)remaining);
        reader->decoded_scan -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = remaining;
    }
    ถ้า (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            _PyTok_ReserveBuffer(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        PyErr_NoMemory();
        คืน -1;
    }
    memcpy(reader->decoded + reader->decoded_len, data, (size_t)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    คืน 0;
}

static จำนวนเต็ม
append_implicit_newline(_PyTok_Reader *reader)
{
    ถ้า (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        คืน 0;
    }
    ถ้า (append_decoded(reader, "\n", 1) < 0) {
        คืน -1;
    }
    reader->decoded_tail_is_implicit = 1;
    คืน 0;
}

static จำนวนเต็ม
pop_decoded_line(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    assert(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    assert(reader->decoded_scan >= reader->decoded_pos &&
           reader->decoded_scan <= reader->decoded_len);
    ถ้า (reader->decoded_pos == reader->decoded_len) {
        คืน 0;
    }
    var_na *start = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    var_na *newline = memchr(reader->decoded + reader->decoded_scan, '\n',
                           reader->decoded_len - reader->decoded_scan);
    ถ้า (newline == NULL) {
        reader->decoded_scan = reader->decoded_len;
        คืน 0;
    }
    Py_ssize_t len = newline - start + 1;
    chunk->data = start;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_scan = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_tail_is_implicit;
    ถ้า (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_scan = 0;
        reader->decoded_tail_is_implicit = 0;
    }
    คืน 1;
}

static จำนวนเต็ม
chunk_is_line(const _PyTok_Chunk *chunk)
{
    ถ้า (chunk->len == 0 || chunk->data[chunk->len - 1] != '\n') {
        คืน 0;
    }
    คืน memchr(chunk->data, '\n', chunk->len - 1) == NULL;
}

static _PyTok_ReadResult
next_prepared(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk)
{
    ถ้า (tok->lineno >= tok->source.nlines) {
        คืน _PYTOK_READ_EOF;
    }
    _PyTok_Span tail = {tok->inp, tok->source.base_offset + tok->source.len};
    _PyTok_Off newline = _PyTok_SourceFindByte(&tok->source, tail, '\n');
    _PyTok_Span line = {tail.start, newline >= 0 ? newline + 1 : tail.end};
    chunk->data = (var_na *)_PyTok_SourceSpanView(
        &tok->source, line, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = line.end == tail.end
        ? tok->reader->prepared_final_newline : 0;
    คืน _PYTOK_READ_LINE;
}

static _PyTok_ReadResult
read_file_line(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    สำหรับ (;;) {
        ถ้า (len > PY_SSIZE_T_MAX - BUFSIZ) {
            PyErr_NoMemory();
            คืน _PYTOK_READ_ERROR;
        }
        ถ้า (_PyTok_ReserveBuffer(&reader->file_buffer, &reader->file_buffer_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            คืน _PYTOK_READ_ERROR;
        }
        จำนวนเต็ม available = (จำนวนเต็ม)Py_MIN(reader->file_buffer_cap - len, INT_MAX);
        size_t read = 0;
        var_na *result = _Py_UniversalNewlineFgetsWithSize(
            reader->file_buffer + len, available, reader->fp, NULL, &read);
        ถ้า (result == NULL) {
            ถ้า (len == 0) {
                คืน _PYTOK_READ_EOF;
            }
            bha_nga;
        }
        len += (Py_ssize_t)read;
        ถ้า (len > 0 && reader->file_buffer[len - 1] == '\n') {
            bha_nga;
        }
    }
    จำนวนเต็ม implicit = reader->file_buffer[len - 1] != '\n';
    chunk->data = reader->file_buffer;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    คืน _PYTOK_READ_LINE;
}

static จำนวนเต็ม
initialize_file(sa_nracanaa tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    ถ้า (reader->encoding != NULL) {
        คืน _PyTok_StartDecoder(tok, "strict");
    }

    _PyTok_ReadResult result = read_file_line(
        tok, &reader->prefetched_lines[0]);
    ถ้า (result == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        คืน 0;
    }
    ถ้า (result != _PYTOK_READ_LINE) {
        คืน -1;
    }
    Py_ssize_t bom_len;
    _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_lines[0], NULL, 0, &bom_len);
    ถ้า (detection == _PYTOK_ENCODING_ERROR) {
        คืน -1;
    }
    ถ้า (detection == _PYTOK_ENCODING_NEED_SECOND_LINE) {
        var_na *first = _PyTok_CopyBytes(
            reader->prefetched_lines[0].data,
            reader->prefetched_lines[0].len);
        ถ้า (first == NULL) {
            tok->done = E_NOMEM;
            คืน -1;
        }
        reader->prefetched_lines[0].data = first;
        reader->prefetched_lines[0].ownership = _PYTOK_CHUNK_PYMEM;
        result = read_file_line(tok, &reader->prefetched_lines[1]);
        ถ้า (result == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        มิฉะนั้น ถ้า (result != _PYTOK_READ_LINE) {
            คืน -1;
        }
        _PyTok_Chunk *second = reader->prefetched_lines[1].data != NULL
            ? &reader->prefetched_lines[1] : NULL;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_lines[0], second, 1, &bom_len);
        ถ้า (detection == _PYTOK_ENCODING_ERROR) {
            คืน -1;
        }
    }
    ถ้า (bom_len != 0) {
        _PyTok_Chunk *first = &reader->prefetched_lines[0];
        ถ้า (first->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(first->data, first->data + bom_len,
                    (size_t)(first->len - bom_len));
            first->data[first->len - bom_len] = '\0';
        }
        มิฉะนั้น {
            first->data += bom_len;
        }
        first->len -= bom_len;
    }
    ถ้า (_PyTok_StartDecoder(tok, "strict") < 0) {
        คืน -1;
    }
    คืน 0;
}

static จำนวนเต็ม
finalize_decoding(sa_nracanaa tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    ถ้า (reader->decoder_finalized) {
        คืน 0;
    }
    reader->decoder_finalized = 1;
    ถ้า (reader->decoder != NULL) {
        _PyTok_Chunk input = {
            .data = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        จำนวนเต็ม decoded = _PyTok_DecodeChunk(tok, &input, 1);
        ถ้า (decoded == 0 &&
                append_decoded(reader, input.data, input.len) < 0) {
            tok->done = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&input);
        ถ้า (decoded < 0) {
            คืน -1;
        }
    }
    ถ้า (append_implicit_newline(reader) < 0) {
        tok->done = E_NOMEM;
        คืน -1;
    }
    คืน 0;
}

static _PyTok_ReadResult
next_file(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    ถ้า (!reader->file_initialized && initialize_file(tok) < 0) {
        คืน _PYTOK_READ_ERROR;
    }
    สำหรับ (;;) {
        ถ้า (pop_decoded_line(reader, chunk)) {
            คืน _PYTOK_READ_LINE;
        }
        _PyTok_Chunk input = {0};
        ถ้า (reader->prefetched_lines[0].data != NULL) {
            input = reader->prefetched_lines[0];
            reader->prefetched_lines[0] = (_PyTok_Chunk){0};
        }
        มิฉะนั้น ถ้า (reader->prefetched_lines[1].data != NULL) {
            input = reader->prefetched_lines[1];
            reader->prefetched_lines[1] = (_PyTok_Chunk){0};
        }
        มิฉะนั้น ถ้า (!reader->file_eof) {
            _PyTok_ReadResult result = read_file_line(tok, &input);
            ถ้า (result == _PYTOK_READ_ERROR) {
                คืน result;
            }
            ถ้า (result == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        ถ้า (input.data != NULL) {
            จำนวนเต็ม implicit = input.implicit_newline;
            ถ้า (reader->decoder == NULL && !implicit) {
                *chunk = input;
                คืน _PYTOK_READ_LINE;
            }
            จำนวนเต็ม decoded = _PyTok_DecodeChunk(tok, &input, 0);
            ถ้า (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_line(&input)) {
                *chunk = input;
                คืน _PYTOK_READ_LINE;
            }
            ถ้า (decoded == 0 &&
                    append_decoded(reader, input.data, input.len) < 0) {
                tok->done = E_NOMEM;
                decoded = -1;
            }
            ถ้า (decoded == 0 && implicit) {
                reader->decoded_tail_is_implicit = 1;
            }
            _PyTok_ChunkClear(&input);
            ถ้า (decoded < 0) {
                คืน _PYTOK_READ_ERROR;
            }
            continue;
        }
        ถ้า (!reader->decoder_finalized) {
            ถ้า (finalize_decoding(tok) < 0) {
                คืน _PYTOK_READ_ERROR;
            }
            continue;
        }
        คืน _PYTOK_READ_EOF;
    }
}

static _PyTok_ReadResult
next_readline(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    สำหรับ (;;) {
        ถ้า (pop_decoded_line(reader, chunk)) {
            คืน _PYTOK_READ_LINE;
        }
        ถ้า (reader->decoder_finalized) {
            คืน _PYTOK_READ_EOF;
        }

        PyObject *raw = PyObject_CallNoArgs(reader->readline);
        ถ้า (raw == NULL) {
            ถ้า (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                ถ้า (finalize_decoding(tok) < 0) {
                    คืน _PYTOK_READ_ERROR;
                }
                continue;
            }
            คืน _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk input = {0};
        ถ้า (reader->encoding != NULL) {
            ถ้า (!PyBytes_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                คืน _PYTOK_READ_ERROR;
            }
            ถ้า (PyBytes_GET_SIZE(raw) == 0) {
                Py_DECREF(raw);
                ถ้า (_PyTok_StartDecoder(tok, "replace") < 0) {
                    คืน _PYTOK_READ_ERROR;
                }
                ถ้า (finalize_decoding(tok) < 0) {
                    คืน _PYTOK_READ_ERROR;
                }
                continue;
            }
            input.owner = raw;
            input.data = PyBytes_AS_STRING(raw);
            input.len = PyBytes_GET_SIZE(raw);
            input.ownership = _PYTOK_CHUNK_PYOBJECT;
            จำนวนเต็ม decoded;
            ถ้า (reader->decoder == NULL &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_line(&input)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &input, "utf-8", "replace");
            }
            มิฉะนั้น {
                decoded = _PyTok_StartDecoder(tok, "replace");
                ถ้า (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &input, 0);
                }
            }
            ถ้า (decoded < 0) {
                _PyTok_ChunkClear(&input);
                คืน _PYTOK_READ_ERROR;
            }
        }
        มิฉะนั้น {
            ถ้า (!PyUnicode_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                คืน _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            const var_na *utf8 = PyUnicode_AsUTF8AndSize(raw, &utf8_len);
            ถ้า (utf8 == NULL) {
                Py_DECREF(raw);
                คืน _PYTOK_READ_ERROR;
            }
            input.owner = raw;
            input.data = (var_na *)utf8;
            input.len = utf8_len;
            input.ownership = _PYTOK_CHUNK_PYOBJECT;
            ถ้า (input.len == 0) {
                _PyTok_ChunkClear(&input);
                ถ้า (finalize_decoding(tok) < 0) {
                    คืน _PYTOK_READ_ERROR;
                }
                continue;
            }
        }

        ถ้า (reader->decoded_pos == reader->decoded_len &&
                chunk_is_line(&input)) {
            *chunk = input;
            คืน _PYTOK_READ_LINE;
        }

        ถ้า (append_decoded(reader, input.data, input.len) < 0) {
            _PyTok_ChunkClear(&input);
            tok->done = E_NOMEM;
            คืน _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&input);
        ถ้า (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            จำนวนเต็ม pending = _PyTok_DecoderHasBufferedInput(tok);
            ถ้า (pending < 0) {
                คืน _PYTOK_READ_ERROR;
            }
            ถ้า (!pending && append_implicit_newline(reader) < 0) {
                tok->done = E_NOMEM;
                คืน _PYTOK_READ_ERROR;
            }
        }
    }
}

static _PyTok_ReadResult
next_interactive(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    ถ้า (reader->stop_interactive) {
        คืน _PYTOK_READ_STOPPED;
    }
    var_na *input = PyOS_Readline(
        reader->fp != NULL ? reader->fp : stdin, stdout, reader->prompt);
    ถ้า (reader->nextprompt != NULL) {
        reader->prompt = reader->nextprompt;
    }
    ถ้า (input == NULL) {
        คืน _PYTOK_READ_INTERRUPT;
    }
    Py_ssize_t len = strlen(input);
    ถ้า (len == 0) {
        PyMem_Free(input);
        คืน _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .data = input,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    ถ้า (reader->encoding != NULL &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, NULL) < 0) {
        _PyTok_ChunkClear(&decoded);
        คืน _PYTOK_READ_ERROR;
    }
    ถ้า (memchr(decoded.data, '\r', decoded.len) == NULL) {
        *chunk = decoded;
        คืน _PYTOK_READ_LINE;
    }
    chunk->data = _PyTok_NormalizeNewlines(
        decoded.data, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    ถ้า (chunk->data == NULL) {
        tok->done = E_NOMEM;
        คืน _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    คืน _PYTOK_READ_LINE;
}

จำนวนเต็ม
_PyTok_ReaderIsInteractive(const sa_nracanaa tok_state *tok)
{
    คืน tok->reader->kind == _PYTOK_READER_INTERACTIVE;
}

const var_na *
_PyTokenizer_RetainedSource(const sa_nracanaa tok_state *tok)
{
    ถ้า (tok->reader->kind == _PYTOK_READER_PREPARED) {
        คืน _PyTok_SourceData(&tok->source);
    }
    ถ้า (tok->reader->kind == _PYTOK_READER_INTERACTIVE) {
        คืน tok->source.bytes;
    }
    คืน NULL;
}

void
_PyTok_ReaderStopInteractive(sa_nracanaa tok_state *tok)
{
    ถ้า (_PyTok_ReaderIsInteractive(tok)) {
        tok->reader->stop_interactive = 1;
    }
}

static _PyTok_ReadResult
reader_next(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    switch (tok->reader->kind) {
        pak_sa _PYTOK_READER_PREPARED:
            คืน next_prepared(tok, chunk);
        pak_sa _PYTOK_READER_FILE:
            คืน next_file(tok, chunk);
        pak_sa _PYTOK_READER_READLINE:
            คืน next_readline(tok, chunk);
        pak_sa _PYTOK_READER_INTERACTIVE:
            คืน next_interactive(tok, chunk);
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

จำนวนเต็ม
_PyTok_ReaderUnderflow(sa_nracanaa tok_state *tok)
{
    assert(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    _PyTok_ReaderKind kind = reader->kind;
    จำนวนเต็ม prepared = kind == _PYTOK_READER_PREPARED;
    จำนวนเต็ม streaming = reader_is_streaming(kind);
    จำนวนเต็ม reset_buffer = tok->start < 0 &&
        _PyLexer_CurrentFTString(tok) == NULL;

    _PyTok_Chunk chunk;
    _PyTok_ReadResult result = reader_next(tok, &chunk);
    ถ้า (result != _PYTOK_READ_LINE) {
        ถ้า (reset_buffer && streaming) {
            reset_streaming_buffer(tok);
        }
        ถ้า (result == _PYTOK_READ_EOF) {
            tok->done = E_EOF;
        }
        มิฉะนั้น ถ้า (result == _PYTOK_READ_STOPPED) {
            tok->done = E_INTERACT_STOP;
        }
        มิฉะนั้น ถ้า (result == _PYTOK_READ_INTERRUPT) {
            tok->done = E_INTR;
        }
        มิฉะนั้น {
            ถ้า (tok->done == E_OK) {
                tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                    ? E_NOMEM : E_ERROR;
            }
        }
        ถ้า (kind == _PYTOK_READER_INTERACTIVE &&
                result != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        คืน 0;
    }
    ถ้า (tok->lineno == INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        tok->done = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        คืน 0;
    }

    ถ้า (!prepared) {
        ถ้า (streaming && reset_buffer) {
            reset_streaming_buffer(tok);
        }
        _PyTok_Off source_start = _PyTok_SourceAppendLine(
            &tok->source, chunk.data, chunk.len);
        ถ้า (source_start < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            คืน 0;
        }
        ถ้า (reset_buffer) {
            tok->cur = source_start;
            tok->buf_offset = source_start;
            tok->line_start = tok->buf_offset;
        }
        tok->inp = source_start + chunk.len;
    }
    มิฉะนั้น {
        ถ้า (reset_buffer) {
            tok->buf_offset = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    ถ้า (kind == _PYTOK_READER_FILE &&
            (reader->encoding == NULL ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                _PyTok_SourcePointer(&tok->source, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        คืน 0;
    }
    _PyTok_ChunkClear(&chunk);
    คืน 1;
}

static sa_nracanaa tok_state *
tokenizer_new_with_reader(_PyTok_ReaderKind kind)
{
    sa_nracanaa tok_state *tok = PyMem_Calloc(1, parimaa_na(*tok));
    ถ้า (tok == NULL) {
        PyErr_NoMemory();
        คืน NULL;
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
    ถ้า (tok->reader == NULL) {
        PyErr_NoMemory();
        _PyTokenizer_Free(tok);
        คืน NULL;
    }
    tok->reader->kind = kind;
    ถ้า (kind == _PYTOK_READER_PREPARED) {
        คืน tok;
    }
    ถ้า (reader_is_streaming(kind)) {
        tok->cur = tok->inp = tok->source.base_offset;
        tok->line_start = tok->buf_offset;
    }
    คืน tok;
}

static sa_nracanaa tok_state *
tokenizer_from_string(const var_na *input, จำนวนเต็ม utf8_only, จำนวนเต็ม exec_input,
                      จำนวนเต็ม preserve_crlf)
{
    sa_nracanaa tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    ถ้า (tok == NULL) {
        คืน NULL;
    }
    ถ้า (_PyTok_PrepareString(
            tok, input, utf8_only, exec_input, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        คืน NULL;
    }
    tok->cur = tok->inp = tok->source.base_offset;
    tok->line_start = tok->source.base_offset;
    คืน tok;
}

sa_nracanaa tok_state *
_PyTokenizer_FromString(const var_na *input, จำนวนเต็ม exec_input, จำนวนเต็ม preserve_crlf)
{
    คืน tokenizer_from_string(input, 0, exec_input, preserve_crlf);
}

sa_nracanaa tok_state *
_PyTokenizer_FromUTF8(const var_na *input, จำนวนเต็ม exec_input, จำนวนเต็ม preserve_crlf)
{
    คืน tokenizer_from_string(input, 1, exec_input, preserve_crlf);
}

sa_nracanaa tok_state *
_PyTokenizer_FromReadline(PyObject *readline, const var_na *encoding)
{
    sa_nracanaa tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    ถ้า (tok == NULL) {
        คืน NULL;
    }
    ถ้า (encoding != NULL && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        คืน NULL;
    }
    tok->reader->readline = Py_NewRef(readline);
    คืน tok;
}

sa_nracanaa tok_state *
_PyTokenizer_FromFile(FILE *fp, const var_na *encoding,
                      const var_na *ps1, const var_na *ps2)
{
    _PyTok_ReaderKind kind = ps1 != NULL || ps2 != NULL
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    sa_nracanaa tok_state *tok = tokenizer_new_with_reader(kind);
    ถ้า (tok == NULL) {
        คืน NULL;
    }
    ถ้า (encoding != NULL && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        คืน NULL;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    คืน tok;
}

#ถ้า defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
typedef sa_ngha {
    void *cookie;
    จำนวนเต็ม fd;
} borrowed_fd;

static ssize_t
borrow_read(void *cookie, var_na *buffer, size_t size)
{
    borrowed_fd borrowed = {.cookie = cookie};
    คืน read(borrowed.fd, buffer, size);
}

static FILE *
fdopen_borrow(จำนวนเต็ม fd)
{
    cookie_io_functions_t callbacks = {borrow_read, NULL, NULL, NULL};
    borrowed_fd borrowed = {.fd = fd};
    คืน fopencookie(borrowed.cookie, "r", callbacks);
}
#มิฉะนั้น
static FILE *
fdopen_borrow(จำนวนเต็ม fd)
{
    จำนวนเต็ม copy = _Py_dup(fd);
    ถ้า (copy < 0) {
        คืน NULL;
    }
    FILE *fp = fdopen(copy, "r");
    ถ้า (fp == NULL) {
        จำนวนเต็ม saved_errno = errno;
        close(copy);
        errno = saved_errno;
    }
    คืน fp;
}
#endif

var_na *
_PyTokenizer_FindEncodingFilename(จำนวนเต็ม fd, PyObject *filename)
{
    FILE *fp = fdopen_borrow(fd);
    ถ้า (fp == NULL) {
        คืน NULL;
    }
    sa_nracanaa tok_state *tok = _PyTokenizer_FromFile(fp, NULL, NULL, NULL);
    ถ้า (tok == NULL) {
        fclose(fp);
        คืน NULL;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != NULL ? filename : &_Py_STR(anon_string));
    var_na *encoding = NULL;
    ถ้า (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = NULL;
    }
    fclose(fp);
    _PyTokenizer_Free(tok);
    คืน encoding;
}
