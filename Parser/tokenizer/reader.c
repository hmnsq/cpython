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

static anta_hstha ကိန်းပြည့်
reader_is_streaming(_PyTok_ReaderKind kind)
{
    ပြန်ပေး kind == _PYTOK_READER_FILE || kind == _PYTOK_READER_READLINE;
}

void
_PyTok_ReaderFree(sa_nracanaa tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    အကယ်၍ (reader == NULL) {
        ပြန်ပေး;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    အတွက် (ကိန်းပြည့် i = 0;
         i < (ကိန်းပြည့်)Py_ARRAY_LENGTH(reader->prefetched_lines); i++) {
        _PyTok_ChunkClear(&reader->prefetched_lines[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_buffer);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = NULL;
}

ကိန်းပြည့်
_PyTokenizer_Traverse(sa_nracanaa tok_state *tok, visitproc visit, void *arg)
{
    Py_VISIT(tok->filename);
    Py_VISIT(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_VISIT(reader->readline);
    Py_VISIT(reader->decoder);
    အတွက် (ကိန်းပြည့် i = 0;
         i < (ကိန်းပြည့်)Py_ARRAY_LENGTH(reader->prefetched_lines); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_lines[i];
        အကယ်၍ (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_VISIT(chunk->owner);
        }
    }
    ပြန်ပေး 0;
}

ကိန်းပြည့်
_PyTok_ReserveBuffer(var_na **buffer, Py_ssize_t *capacity, Py_ssize_t needed,
                     Py_ssize_t initial_capacity)
{
    assert(initial_capacity > 0);
    အကယ်၍ (needed <= *capacity) {
        ပြန်ပေး 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *capacity > 0 ? *capacity : initial_capacity;
    နေစဉ် (cap < needed) {
        အကယ်၍ (cap > PY_SSIZE_T_MAX / READER_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            bha_nga;
        }
        cap *= READER_BUFFER_GROWTH_FACTOR;
    }
    var_na *resized = PyMem_Realloc(*buffer, cap);
    အကယ်၍ (resized == NULL) {
        PyErr_NoMemory();
        ပြန်ပေး -1;
    }
    *buffer = resized;
    *capacity = cap;
    ပြန်ပေး 0;
}

static ကိန်းပြည့်
append_decoded(_PyTok_Reader *reader, const var_na *data, Py_ssize_t len)
{
    အကယ်၍ (len == 0) {
        ပြန်ပေး 0;
    }
    အကယ်၍ (reader->decoded_pos > 0) {
        Py_ssize_t remaining = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (size_t)remaining);
        reader->decoded_scan -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = remaining;
    }
    အကယ်၍ (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            _PyTok_ReserveBuffer(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        PyErr_NoMemory();
        ပြန်ပေး -1;
    }
    memcpy(reader->decoded + reader->decoded_len, data, (size_t)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    ပြန်ပေး 0;
}

static ကိန်းပြည့်
append_implicit_newline(_PyTok_Reader *reader)
{
    အကယ်၍ (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        ပြန်ပေး 0;
    }
    အကယ်၍ (append_decoded(reader, "\n", 1) < 0) {
        ပြန်ပေး -1;
    }
    reader->decoded_tail_is_implicit = 1;
    ပြန်ပေး 0;
}

static ကိန်းပြည့်
pop_decoded_line(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    assert(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    assert(reader->decoded_scan >= reader->decoded_pos &&
           reader->decoded_scan <= reader->decoded_len);
    အကယ်၍ (reader->decoded_pos == reader->decoded_len) {
        ပြန်ပေး 0;
    }
    var_na *start = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    var_na *newline = memchr(reader->decoded + reader->decoded_scan, '\n',
                           reader->decoded_len - reader->decoded_scan);
    အကယ်၍ (newline == NULL) {
        reader->decoded_scan = reader->decoded_len;
        ပြန်ပေး 0;
    }
    Py_ssize_t len = newline - start + 1;
    chunk->data = start;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_scan = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_tail_is_implicit;
    အကယ်၍ (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_scan = 0;
        reader->decoded_tail_is_implicit = 0;
    }
    ပြန်ပေး 1;
}

static ကိန်းပြည့်
chunk_is_line(const _PyTok_Chunk *chunk)
{
    အကယ်၍ (chunk->len == 0 || chunk->data[chunk->len - 1] != '\n') {
        ပြန်ပေး 0;
    }
    ပြန်ပေး memchr(chunk->data, '\n', chunk->len - 1) == NULL;
}

static _PyTok_ReadResult
next_prepared(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk)
{
    အကယ်၍ (tok->lineno >= tok->source.nlines) {
        ပြန်ပေး _PYTOK_READ_EOF;
    }
    _PyTok_Span tail = {tok->inp, tok->source.base_offset + tok->source.len};
    _PyTok_Off newline = _PyTok_SourceFindByte(&tok->source, tail, '\n');
    _PyTok_Span line = {tail.start, newline >= 0 ? newline + 1 : tail.end};
    chunk->data = (var_na *)_PyTok_SourceSpanView(
        &tok->source, line, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = line.end == tail.end
        ? tok->reader->prepared_final_newline : 0;
    ပြန်ပေး _PYTOK_READ_LINE;
}

static _PyTok_ReadResult
read_file_line(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    အတွက် (;;) {
        အကယ်၍ (len > PY_SSIZE_T_MAX - BUFSIZ) {
            PyErr_NoMemory();
            ပြန်ပေး _PYTOK_READ_ERROR;
        }
        အကယ်၍ (_PyTok_ReserveBuffer(&reader->file_buffer, &reader->file_buffer_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            ပြန်ပေး _PYTOK_READ_ERROR;
        }
        ကိန်းပြည့် available = (ကိန်းပြည့်)Py_MIN(reader->file_buffer_cap - len, INT_MAX);
        size_t read = 0;
        var_na *result = _Py_UniversalNewlineFgetsWithSize(
            reader->file_buffer + len, available, reader->fp, NULL, &read);
        အကယ်၍ (result == NULL) {
            အကယ်၍ (len == 0) {
                ပြန်ပေး _PYTOK_READ_EOF;
            }
            bha_nga;
        }
        len += (Py_ssize_t)read;
        အကယ်၍ (len > 0 && reader->file_buffer[len - 1] == '\n') {
            bha_nga;
        }
    }
    ကိန်းပြည့် implicit = reader->file_buffer[len - 1] != '\n';
    chunk->data = reader->file_buffer;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    ပြန်ပေး _PYTOK_READ_LINE;
}

static ကိန်းပြည့်
initialize_file(sa_nracanaa tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    အကယ်၍ (reader->encoding != NULL) {
        ပြန်ပေး _PyTok_StartDecoder(tok, "strict");
    }

    _PyTok_ReadResult result = read_file_line(
        tok, &reader->prefetched_lines[0]);
    အကယ်၍ (result == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        ပြန်ပေး 0;
    }
    အကယ်၍ (result != _PYTOK_READ_LINE) {
        ပြန်ပေး -1;
    }
    Py_ssize_t bom_len;
    _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_lines[0], NULL, 0, &bom_len);
    အကယ်၍ (detection == _PYTOK_ENCODING_ERROR) {
        ပြန်ပေး -1;
    }
    အကယ်၍ (detection == _PYTOK_ENCODING_NEED_SECOND_LINE) {
        var_na *first = _PyTok_CopyBytes(
            reader->prefetched_lines[0].data,
            reader->prefetched_lines[0].len);
        အကယ်၍ (first == NULL) {
            tok->done = E_NOMEM;
            ပြန်ပေး -1;
        }
        reader->prefetched_lines[0].data = first;
        reader->prefetched_lines[0].ownership = _PYTOK_CHUNK_PYMEM;
        result = read_file_line(tok, &reader->prefetched_lines[1]);
        အကယ်၍ (result == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        မဟုတ်ပါက အကယ်၍ (result != _PYTOK_READ_LINE) {
            ပြန်ပေး -1;
        }
        _PyTok_Chunk *second = reader->prefetched_lines[1].data != NULL
            ? &reader->prefetched_lines[1] : NULL;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_lines[0], second, 1, &bom_len);
        အကယ်၍ (detection == _PYTOK_ENCODING_ERROR) {
            ပြန်ပေး -1;
        }
    }
    အကယ်၍ (bom_len != 0) {
        _PyTok_Chunk *first = &reader->prefetched_lines[0];
        အကယ်၍ (first->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(first->data, first->data + bom_len,
                    (size_t)(first->len - bom_len));
            first->data[first->len - bom_len] = '\0';
        }
        မဟုတ်ပါက {
            first->data += bom_len;
        }
        first->len -= bom_len;
    }
    အကယ်၍ (_PyTok_StartDecoder(tok, "strict") < 0) {
        ပြန်ပေး -1;
    }
    ပြန်ပေး 0;
}

static ကိန်းပြည့်
finalize_decoding(sa_nracanaa tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    အကယ်၍ (reader->decoder_finalized) {
        ပြန်ပေး 0;
    }
    reader->decoder_finalized = 1;
    အကယ်၍ (reader->decoder != NULL) {
        _PyTok_Chunk input = {
            .data = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        ကိန်းပြည့် decoded = _PyTok_DecodeChunk(tok, &input, 1);
        အကယ်၍ (decoded == 0 &&
                append_decoded(reader, input.data, input.len) < 0) {
            tok->done = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&input);
        အကယ်၍ (decoded < 0) {
            ပြန်ပေး -1;
        }
    }
    အကယ်၍ (append_implicit_newline(reader) < 0) {
        tok->done = E_NOMEM;
        ပြန်ပေး -1;
    }
    ပြန်ပေး 0;
}

static _PyTok_ReadResult
next_file(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    အကယ်၍ (!reader->file_initialized && initialize_file(tok) < 0) {
        ပြန်ပေး _PYTOK_READ_ERROR;
    }
    အတွက် (;;) {
        အကယ်၍ (pop_decoded_line(reader, chunk)) {
            ပြန်ပေး _PYTOK_READ_LINE;
        }
        _PyTok_Chunk input = {0};
        အကယ်၍ (reader->prefetched_lines[0].data != NULL) {
            input = reader->prefetched_lines[0];
            reader->prefetched_lines[0] = (_PyTok_Chunk){0};
        }
        မဟုတ်ပါက အကယ်၍ (reader->prefetched_lines[1].data != NULL) {
            input = reader->prefetched_lines[1];
            reader->prefetched_lines[1] = (_PyTok_Chunk){0};
        }
        မဟုတ်ပါက အကယ်၍ (!reader->file_eof) {
            _PyTok_ReadResult result = read_file_line(tok, &input);
            အကယ်၍ (result == _PYTOK_READ_ERROR) {
                ပြန်ပေး result;
            }
            အကယ်၍ (result == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        အကယ်၍ (input.data != NULL) {
            ကိန်းပြည့် implicit = input.implicit_newline;
            အကယ်၍ (reader->decoder == NULL && !implicit) {
                *chunk = input;
                ပြန်ပေး _PYTOK_READ_LINE;
            }
            ကိန်းပြည့် decoded = _PyTok_DecodeChunk(tok, &input, 0);
            အကယ်၍ (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_line(&input)) {
                *chunk = input;
                ပြန်ပေး _PYTOK_READ_LINE;
            }
            အကယ်၍ (decoded == 0 &&
                    append_decoded(reader, input.data, input.len) < 0) {
                tok->done = E_NOMEM;
                decoded = -1;
            }
            အကယ်၍ (decoded == 0 && implicit) {
                reader->decoded_tail_is_implicit = 1;
            }
            _PyTok_ChunkClear(&input);
            အကယ်၍ (decoded < 0) {
                ပြန်ပေး _PYTOK_READ_ERROR;
            }
            continue;
        }
        အကယ်၍ (!reader->decoder_finalized) {
            အကယ်၍ (finalize_decoding(tok) < 0) {
                ပြန်ပေး _PYTOK_READ_ERROR;
            }
            continue;
        }
        ပြန်ပေး _PYTOK_READ_EOF;
    }
}

static _PyTok_ReadResult
next_readline(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    အတွက် (;;) {
        အကယ်၍ (pop_decoded_line(reader, chunk)) {
            ပြန်ပေး _PYTOK_READ_LINE;
        }
        အကယ်၍ (reader->decoder_finalized) {
            ပြန်ပေး _PYTOK_READ_EOF;
        }

        PyObject *raw = PyObject_CallNoArgs(reader->readline);
        အကယ်၍ (raw == NULL) {
            အကယ်၍ (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                အကယ်၍ (finalize_decoding(tok) < 0) {
                    ပြန်ပေး _PYTOK_READ_ERROR;
                }
                continue;
            }
            ပြန်ပေး _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk input = {0};
        အကယ်၍ (reader->encoding != NULL) {
            အကယ်၍ (!PyBytes_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                ပြန်ပေး _PYTOK_READ_ERROR;
            }
            အကယ်၍ (PyBytes_GET_SIZE(raw) == 0) {
                Py_DECREF(raw);
                အကယ်၍ (_PyTok_StartDecoder(tok, "replace") < 0) {
                    ပြန်ပေး _PYTOK_READ_ERROR;
                }
                အကယ်၍ (finalize_decoding(tok) < 0) {
                    ပြန်ပေး _PYTOK_READ_ERROR;
                }
                continue;
            }
            input.owner = raw;
            input.data = PyBytes_AS_STRING(raw);
            input.len = PyBytes_GET_SIZE(raw);
            input.ownership = _PYTOK_CHUNK_PYOBJECT;
            ကိန်းပြည့် decoded;
            အကယ်၍ (reader->decoder == NULL &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_line(&input)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &input, "utf-8", "replace");
            }
            မဟုတ်ပါက {
                decoded = _PyTok_StartDecoder(tok, "replace");
                အကယ်၍ (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &input, 0);
                }
            }
            အကယ်၍ (decoded < 0) {
                _PyTok_ChunkClear(&input);
                ပြန်ပေး _PYTOK_READ_ERROR;
            }
        }
        မဟုတ်ပါက {
            အကယ်၍ (!PyUnicode_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                ပြန်ပေး _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            const var_na *utf8 = PyUnicode_AsUTF8AndSize(raw, &utf8_len);
            အကယ်၍ (utf8 == NULL) {
                Py_DECREF(raw);
                ပြန်ပေး _PYTOK_READ_ERROR;
            }
            input.owner = raw;
            input.data = (var_na *)utf8;
            input.len = utf8_len;
            input.ownership = _PYTOK_CHUNK_PYOBJECT;
            အကယ်၍ (input.len == 0) {
                _PyTok_ChunkClear(&input);
                အကယ်၍ (finalize_decoding(tok) < 0) {
                    ပြန်ပေး _PYTOK_READ_ERROR;
                }
                continue;
            }
        }

        အကယ်၍ (reader->decoded_pos == reader->decoded_len &&
                chunk_is_line(&input)) {
            *chunk = input;
            ပြန်ပေး _PYTOK_READ_LINE;
        }

        အကယ်၍ (append_decoded(reader, input.data, input.len) < 0) {
            _PyTok_ChunkClear(&input);
            tok->done = E_NOMEM;
            ပြန်ပေး _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&input);
        အကယ်၍ (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            ကိန်းပြည့် pending = _PyTok_DecoderHasBufferedInput(tok);
            အကယ်၍ (pending < 0) {
                ပြန်ပေး _PYTOK_READ_ERROR;
            }
            အကယ်၍ (!pending && append_implicit_newline(reader) < 0) {
                tok->done = E_NOMEM;
                ပြန်ပေး _PYTOK_READ_ERROR;
            }
        }
    }
}

static _PyTok_ReadResult
next_interactive(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    အကယ်၍ (reader->stop_interactive) {
        ပြန်ပေး _PYTOK_READ_STOPPED;
    }
    var_na *input = PyOS_Readline(
        reader->fp != NULL ? reader->fp : stdin, stdout, reader->prompt);
    အကယ်၍ (reader->nextprompt != NULL) {
        reader->prompt = reader->nextprompt;
    }
    အကယ်၍ (input == NULL) {
        ပြန်ပေး _PYTOK_READ_INTERRUPT;
    }
    Py_ssize_t len = strlen(input);
    အကယ်၍ (len == 0) {
        PyMem_Free(input);
        ပြန်ပေး _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .data = input,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    အကယ်၍ (reader->encoding != NULL &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, NULL) < 0) {
        _PyTok_ChunkClear(&decoded);
        ပြန်ပေး _PYTOK_READ_ERROR;
    }
    အကယ်၍ (memchr(decoded.data, '\r', decoded.len) == NULL) {
        *chunk = decoded;
        ပြန်ပေး _PYTOK_READ_LINE;
    }
    chunk->data = _PyTok_NormalizeNewlines(
        decoded.data, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    အကယ်၍ (chunk->data == NULL) {
        tok->done = E_NOMEM;
        ပြန်ပေး _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    ပြန်ပေး _PYTOK_READ_LINE;
}

ကိန်းပြည့်
_PyTok_ReaderIsInteractive(const sa_nracanaa tok_state *tok)
{
    ပြန်ပေး tok->reader->kind == _PYTOK_READER_INTERACTIVE;
}

const var_na *
_PyTokenizer_RetainedSource(const sa_nracanaa tok_state *tok)
{
    အကယ်၍ (tok->reader->kind == _PYTOK_READER_PREPARED) {
        ပြန်ပေး _PyTok_SourceData(&tok->source);
    }
    အကယ်၍ (tok->reader->kind == _PYTOK_READER_INTERACTIVE) {
        ပြန်ပေး tok->source.bytes;
    }
    ပြန်ပေး NULL;
}

void
_PyTok_ReaderStopInteractive(sa_nracanaa tok_state *tok)
{
    အကယ်၍ (_PyTok_ReaderIsInteractive(tok)) {
        tok->reader->stop_interactive = 1;
    }
}

static _PyTok_ReadResult
reader_next(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    switch (tok->reader->kind) {
        pak_sa _PYTOK_READER_PREPARED:
            ပြန်ပေး next_prepared(tok, chunk);
        pak_sa _PYTOK_READER_FILE:
            ပြန်ပေး next_file(tok, chunk);
        pak_sa _PYTOK_READER_READLINE:
            ပြန်ပေး next_readline(tok, chunk);
        pak_sa _PYTOK_READER_INTERACTIVE:
            ပြန်ပေး next_interactive(tok, chunk);
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

ကိန်းပြည့်
_PyTok_ReaderUnderflow(sa_nracanaa tok_state *tok)
{
    assert(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    _PyTok_ReaderKind kind = reader->kind;
    ကိန်းပြည့် prepared = kind == _PYTOK_READER_PREPARED;
    ကိန်းပြည့် streaming = reader_is_streaming(kind);
    ကိန်းပြည့် reset_buffer = tok->start < 0 &&
        _PyLexer_CurrentFTString(tok) == NULL;

    _PyTok_Chunk chunk;
    _PyTok_ReadResult result = reader_next(tok, &chunk);
    အကယ်၍ (result != _PYTOK_READ_LINE) {
        အကယ်၍ (reset_buffer && streaming) {
            reset_streaming_buffer(tok);
        }
        အကယ်၍ (result == _PYTOK_READ_EOF) {
            tok->done = E_EOF;
        }
        မဟုတ်ပါက အကယ်၍ (result == _PYTOK_READ_STOPPED) {
            tok->done = E_INTERACT_STOP;
        }
        မဟုတ်ပါက အကယ်၍ (result == _PYTOK_READ_INTERRUPT) {
            tok->done = E_INTR;
        }
        မဟုတ်ပါက {
            အကယ်၍ (tok->done == E_OK) {
                tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                    ? E_NOMEM : E_ERROR;
            }
        }
        အကယ်၍ (kind == _PYTOK_READER_INTERACTIVE &&
                result != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        ပြန်ပေး 0;
    }
    အကယ်၍ (tok->lineno == INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        tok->done = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        ပြန်ပေး 0;
    }

    အကယ်၍ (!prepared) {
        အကယ်၍ (streaming && reset_buffer) {
            reset_streaming_buffer(tok);
        }
        _PyTok_Off source_start = _PyTok_SourceAppendLine(
            &tok->source, chunk.data, chunk.len);
        အကယ်၍ (source_start < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            ပြန်ပေး 0;
        }
        အကယ်၍ (reset_buffer) {
            tok->cur = source_start;
            tok->buf_offset = source_start;
            tok->line_start = tok->buf_offset;
        }
        tok->inp = source_start + chunk.len;
    }
    မဟုတ်ပါက {
        အကယ်၍ (reset_buffer) {
            tok->buf_offset = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    အကယ်၍ (kind == _PYTOK_READER_FILE &&
            (reader->encoding == NULL ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                _PyTok_SourcePointer(&tok->source, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        ပြန်ပေး 0;
    }
    _PyTok_ChunkClear(&chunk);
    ပြန်ပေး 1;
}

static sa_nracanaa tok_state *
tokenizer_new_with_reader(_PyTok_ReaderKind kind)
{
    sa_nracanaa tok_state *tok = PyMem_Calloc(1, parimaa_na(*tok));
    အကယ်၍ (tok == NULL) {
        PyErr_NoMemory();
        ပြန်ပေး NULL;
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
    အကယ်၍ (tok->reader == NULL) {
        PyErr_NoMemory();
        _PyTokenizer_Free(tok);
        ပြန်ပေး NULL;
    }
    tok->reader->kind = kind;
    အကယ်၍ (kind == _PYTOK_READER_PREPARED) {
        ပြန်ပေး tok;
    }
    အကယ်၍ (reader_is_streaming(kind)) {
        tok->cur = tok->inp = tok->source.base_offset;
        tok->line_start = tok->buf_offset;
    }
    ပြန်ပေး tok;
}

static sa_nracanaa tok_state *
tokenizer_from_string(const var_na *input, ကိန်းပြည့် utf8_only, ကိန်းပြည့် exec_input,
                      ကိန်းပြည့် preserve_crlf)
{
    sa_nracanaa tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    အကယ်၍ (tok == NULL) {
        ပြန်ပေး NULL;
    }
    အကယ်၍ (_PyTok_PrepareString(
            tok, input, utf8_only, exec_input, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        ပြန်ပေး NULL;
    }
    tok->cur = tok->inp = tok->source.base_offset;
    tok->line_start = tok->source.base_offset;
    ပြန်ပေး tok;
}

sa_nracanaa tok_state *
_PyTokenizer_FromString(const var_na *input, ကိန်းပြည့် exec_input, ကိန်းပြည့် preserve_crlf)
{
    ပြန်ပေး tokenizer_from_string(input, 0, exec_input, preserve_crlf);
}

sa_nracanaa tok_state *
_PyTokenizer_FromUTF8(const var_na *input, ကိန်းပြည့် exec_input, ကိန်းပြည့် preserve_crlf)
{
    ပြန်ပေး tokenizer_from_string(input, 1, exec_input, preserve_crlf);
}

sa_nracanaa tok_state *
_PyTokenizer_FromReadline(PyObject *readline, const var_na *encoding)
{
    sa_nracanaa tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    အကယ်၍ (tok == NULL) {
        ပြန်ပေး NULL;
    }
    အကယ်၍ (encoding != NULL && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        ပြန်ပေး NULL;
    }
    tok->reader->readline = Py_NewRef(readline);
    ပြန်ပေး tok;
}

sa_nracanaa tok_state *
_PyTokenizer_FromFile(FILE *fp, const var_na *encoding,
                      const var_na *ps1, const var_na *ps2)
{
    _PyTok_ReaderKind kind = ps1 != NULL || ps2 != NULL
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    sa_nracanaa tok_state *tok = tokenizer_new_with_reader(kind);
    အကယ်၍ (tok == NULL) {
        ပြန်ပေး NULL;
    }
    အကယ်၍ (encoding != NULL && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        ပြန်ပေး NULL;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    ပြန်ပေး tok;
}

#အကယ်၍ defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
typedef sa_ngha {
    void *cookie;
    ကိန်းပြည့် fd;
} borrowed_fd;

static ssize_t
borrow_read(void *cookie, var_na *buffer, size_t size)
{
    borrowed_fd borrowed = {.cookie = cookie};
    ပြန်ပေး read(borrowed.fd, buffer, size);
}

static FILE *
fdopen_borrow(ကိန်းပြည့် fd)
{
    cookie_io_functions_t callbacks = {borrow_read, NULL, NULL, NULL};
    borrowed_fd borrowed = {.fd = fd};
    ပြန်ပေး fopencookie(borrowed.cookie, "r", callbacks);
}
#မဟုတ်ပါက
static FILE *
fdopen_borrow(ကိန်းပြည့် fd)
{
    ကိန်းပြည့် copy = _Py_dup(fd);
    အကယ်၍ (copy < 0) {
        ပြန်ပေး NULL;
    }
    FILE *fp = fdopen(copy, "r");
    အကယ်၍ (fp == NULL) {
        ကိန်းပြည့် saved_errno = errno;
        close(copy);
        errno = saved_errno;
    }
    ပြန်ပေး fp;
}
#endif

var_na *
_PyTokenizer_FindEncodingFilename(ကိန်းပြည့် fd, PyObject *filename)
{
    FILE *fp = fdopen_borrow(fd);
    အကယ်၍ (fp == NULL) {
        ပြန်ပေး NULL;
    }
    sa_nracanaa tok_state *tok = _PyTokenizer_FromFile(fp, NULL, NULL, NULL);
    အကယ်၍ (tok == NULL) {
        fclose(fp);
        ပြန်ပေး NULL;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != NULL ? filename : &_Py_STR(anon_string));
    var_na *encoding = NULL;
    အကယ်၍ (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = NULL;
    }
    fclose(fp);
    _PyTokenizer_Free(tok);
    ပြန်ပေး encoding;
}
