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

static inline цяло
reader_is_streaming(_PyTok_ReaderKind kind)
{
    върни kind == _PYTOK_READER_FILE || kind == _PYTOK_READER_READLINE;
}

void
_PyTok_ReaderFree(struct tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    ако (reader == NULL) {
        върни;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    за (цяло i = 0;
         i < (цяло)Py_ARRAY_LENGTH(reader->prefetched_lines); i++) {
        _PyTok_ChunkClear(&reader->prefetched_lines[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_buffer);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = NULL;
}

цяло
_PyTokenizer_Traverse(struct tok_state *tok, visitproc visit, void *arg)
{
    Py_VISIT(tok->filename);
    Py_VISIT(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_VISIT(reader->readline);
    Py_VISIT(reader->decoder);
    за (цяло i = 0;
         i < (цяло)Py_ARRAY_LENGTH(reader->prefetched_lines); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_lines[i];
        ако (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_VISIT(chunk->owner);
        }
    }
    върни 0;
}

цяло
_PyTok_ReserveBuffer(char **buffer, Py_ssize_t *capacity, Py_ssize_t needed,
                     Py_ssize_t initial_capacity)
{
    assert(initial_capacity > 0);
    ако (needed <= *capacity) {
        върни 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *capacity > 0 ? *capacity : initial_capacity;
    докато (cap < needed) {
        ако (cap > PY_SSIZE_T_MAX / READER_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            break;
        }
        cap *= READER_BUFFER_GROWTH_FACTOR;
    }
    char *resized = PyMem_Realloc(*buffer, cap);
    ако (resized == NULL) {
        PyErr_NoMemory();
        върни -1;
    }
    *buffer = resized;
    *capacity = cap;
    върни 0;
}

static цяло
append_decoded(_PyTok_Reader *reader, const char *data, Py_ssize_t len)
{
    ако (len == 0) {
        върни 0;
    }
    ако (reader->decoded_pos > 0) {
        Py_ssize_t remaining = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (size_t)remaining);
        reader->decoded_scan -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = remaining;
    }
    ако (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            _PyTok_ReserveBuffer(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        PyErr_NoMemory();
        върни -1;
    }
    memcpy(reader->decoded + reader->decoded_len, data, (size_t)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    върни 0;
}

static цяло
append_implicit_newline(_PyTok_Reader *reader)
{
    ако (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        върни 0;
    }
    ако (append_decoded(reader, "\n", 1) < 0) {
        върни -1;
    }
    reader->decoded_tail_is_implicit = 1;
    върни 0;
}

static цяло
pop_decoded_line(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    assert(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    assert(reader->decoded_scan >= reader->decoded_pos &&
           reader->decoded_scan <= reader->decoded_len);
    ако (reader->decoded_pos == reader->decoded_len) {
        върни 0;
    }
    char *start = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    char *newline = memchr(reader->decoded + reader->decoded_scan, '\n',
                           reader->decoded_len - reader->decoded_scan);
    ако (newline == NULL) {
        reader->decoded_scan = reader->decoded_len;
        върни 0;
    }
    Py_ssize_t len = newline - start + 1;
    chunk->data = start;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_scan = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_tail_is_implicit;
    ако (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_scan = 0;
        reader->decoded_tail_is_implicit = 0;
    }
    върни 1;
}

static цяло
chunk_is_line(const _PyTok_Chunk *chunk)
{
    ако (chunk->len == 0 || chunk->data[chunk->len - 1] != '\n') {
        върни 0;
    }
    върни memchr(chunk->data, '\n', chunk->len - 1) == NULL;
}

static _PyTok_ReadResult
next_prepared(struct tok_state *tok, _PyTok_Chunk *chunk)
{
    ако (tok->lineno >= tok->source.nlines) {
        върни _PYTOK_READ_EOF;
    }
    _PyTok_Span tail = {tok->inp, tok->source.base_offset + tok->source.len};
    _PyTok_Off newline = _PyTok_SourceFindByte(&tok->source, tail, '\n');
    _PyTok_Span line = {tail.start, newline >= 0 ? newline + 1 : tail.end};
    chunk->data = (char *)_PyTok_SourceSpanView(
        &tok->source, line, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = line.end == tail.end
        ? tok->reader->prepared_final_newline : 0;
    върни _PYTOK_READ_LINE;
}

static _PyTok_ReadResult
read_file_line(struct tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    за (;;) {
        ако (len > PY_SSIZE_T_MAX - BUFSIZ) {
            PyErr_NoMemory();
            върни _PYTOK_READ_ERROR;
        }
        ако (_PyTok_ReserveBuffer(&reader->file_buffer, &reader->file_buffer_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            върни _PYTOK_READ_ERROR;
        }
        цяло available = (цяло)Py_MIN(reader->file_buffer_cap - len, INT_MAX);
        size_t read = 0;
        char *result = _Py_UniversalNewlineFgetsWithSize(
            reader->file_buffer + len, available, reader->fp, NULL, &read);
        ако (result == NULL) {
            ако (len == 0) {
                върни _PYTOK_READ_EOF;
            }
            break;
        }
        len += (Py_ssize_t)read;
        ако (len > 0 && reader->file_buffer[len - 1] == '\n') {
            break;
        }
    }
    цяло implicit = reader->file_buffer[len - 1] != '\n';
    chunk->data = reader->file_buffer;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    върни _PYTOK_READ_LINE;
}

static цяло
initialize_file(struct tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    ако (reader->encoding != NULL) {
        върни _PyTok_StartDecoder(tok, "strict");
    }

    _PyTok_ReadResult result = read_file_line(
        tok, &reader->prefetched_lines[0]);
    ако (result == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        върни 0;
    }
    ако (result != _PYTOK_READ_LINE) {
        върни -1;
    }
    Py_ssize_t bom_len;
    _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_lines[0], NULL, 0, &bom_len);
    ако (detection == _PYTOK_ENCODING_ERROR) {
        върни -1;
    }
    ако (detection == _PYTOK_ENCODING_NEED_SECOND_LINE) {
        char *first = _PyTok_CopyBytes(
            reader->prefetched_lines[0].data,
            reader->prefetched_lines[0].len);
        ако (first == NULL) {
            tok->done = E_NOMEM;
            върни -1;
        }
        reader->prefetched_lines[0].data = first;
        reader->prefetched_lines[0].ownership = _PYTOK_CHUNK_PYMEM;
        result = read_file_line(tok, &reader->prefetched_lines[1]);
        ако (result == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        иначе ако (result != _PYTOK_READ_LINE) {
            върни -1;
        }
        _PyTok_Chunk *second = reader->prefetched_lines[1].data != NULL
            ? &reader->prefetched_lines[1] : NULL;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_lines[0], second, 1, &bom_len);
        ако (detection == _PYTOK_ENCODING_ERROR) {
            върни -1;
        }
    }
    ако (bom_len != 0) {
        _PyTok_Chunk *first = &reader->prefetched_lines[0];
        ако (first->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(first->data, first->data + bom_len,
                    (size_t)(first->len - bom_len));
            first->data[first->len - bom_len] = '\0';
        }
        иначе {
            first->data += bom_len;
        }
        first->len -= bom_len;
    }
    ако (_PyTok_StartDecoder(tok, "strict") < 0) {
        върни -1;
    }
    върни 0;
}

static цяло
finalize_decoding(struct tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    ако (reader->decoder_finalized) {
        върни 0;
    }
    reader->decoder_finalized = 1;
    ако (reader->decoder != NULL) {
        _PyTok_Chunk input = {
            .data = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        цяло decoded = _PyTok_DecodeChunk(tok, &input, 1);
        ако (decoded == 0 &&
                append_decoded(reader, input.data, input.len) < 0) {
            tok->done = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&input);
        ако (decoded < 0) {
            върни -1;
        }
    }
    ако (append_implicit_newline(reader) < 0) {
        tok->done = E_NOMEM;
        върни -1;
    }
    върни 0;
}

static _PyTok_ReadResult
next_file(struct tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    ако (!reader->file_initialized && initialize_file(tok) < 0) {
        върни _PYTOK_READ_ERROR;
    }
    за (;;) {
        ако (pop_decoded_line(reader, chunk)) {
            върни _PYTOK_READ_LINE;
        }
        _PyTok_Chunk input = {0};
        ако (reader->prefetched_lines[0].data != NULL) {
            input = reader->prefetched_lines[0];
            reader->prefetched_lines[0] = (_PyTok_Chunk){0};
        }
        иначе ако (reader->prefetched_lines[1].data != NULL) {
            input = reader->prefetched_lines[1];
            reader->prefetched_lines[1] = (_PyTok_Chunk){0};
        }
        иначе ако (!reader->file_eof) {
            _PyTok_ReadResult result = read_file_line(tok, &input);
            ако (result == _PYTOK_READ_ERROR) {
                върни result;
            }
            ако (result == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        ако (input.data != NULL) {
            цяло implicit = input.implicit_newline;
            ако (reader->decoder == NULL && !implicit) {
                *chunk = input;
                върни _PYTOK_READ_LINE;
            }
            цяло decoded = _PyTok_DecodeChunk(tok, &input, 0);
            ако (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_line(&input)) {
                *chunk = input;
                върни _PYTOK_READ_LINE;
            }
            ако (decoded == 0 &&
                    append_decoded(reader, input.data, input.len) < 0) {
                tok->done = E_NOMEM;
                decoded = -1;
            }
            ако (decoded == 0 && implicit) {
                reader->decoded_tail_is_implicit = 1;
            }
            _PyTok_ChunkClear(&input);
            ако (decoded < 0) {
                върни _PYTOK_READ_ERROR;
            }
            continue;
        }
        ако (!reader->decoder_finalized) {
            ако (finalize_decoding(tok) < 0) {
                върни _PYTOK_READ_ERROR;
            }
            continue;
        }
        върни _PYTOK_READ_EOF;
    }
}

static _PyTok_ReadResult
next_readline(struct tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    за (;;) {
        ако (pop_decoded_line(reader, chunk)) {
            върни _PYTOK_READ_LINE;
        }
        ако (reader->decoder_finalized) {
            върни _PYTOK_READ_EOF;
        }

        PyObject *raw = PyObject_CallNoArgs(reader->readline);
        ако (raw == NULL) {
            ако (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                ако (finalize_decoding(tok) < 0) {
                    върни _PYTOK_READ_ERROR;
                }
                continue;
            }
            върни _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk input = {0};
        ако (reader->encoding != NULL) {
            ако (!PyBytes_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                върни _PYTOK_READ_ERROR;
            }
            ако (PyBytes_GET_SIZE(raw) == 0) {
                Py_DECREF(raw);
                ако (_PyTok_StartDecoder(tok, "replace") < 0) {
                    върни _PYTOK_READ_ERROR;
                }
                ако (finalize_decoding(tok) < 0) {
                    върни _PYTOK_READ_ERROR;
                }
                continue;
            }
            input.owner = raw;
            input.data = PyBytes_AS_STRING(raw);
            input.len = PyBytes_GET_SIZE(raw);
            input.ownership = _PYTOK_CHUNK_PYOBJECT;
            цяло decoded;
            ако (reader->decoder == NULL &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_line(&input)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &input, "utf-8", "replace");
            }
            иначе {
                decoded = _PyTok_StartDecoder(tok, "replace");
                ако (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &input, 0);
                }
            }
            ако (decoded < 0) {
                _PyTok_ChunkClear(&input);
                върни _PYTOK_READ_ERROR;
            }
        }
        иначе {
            ако (!PyUnicode_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                върни _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            const char *utf8 = PyUnicode_AsUTF8AndSize(raw, &utf8_len);
            ако (utf8 == NULL) {
                Py_DECREF(raw);
                върни _PYTOK_READ_ERROR;
            }
            input.owner = raw;
            input.data = (char *)utf8;
            input.len = utf8_len;
            input.ownership = _PYTOK_CHUNK_PYOBJECT;
            ако (input.len == 0) {
                _PyTok_ChunkClear(&input);
                ако (finalize_decoding(tok) < 0) {
                    върни _PYTOK_READ_ERROR;
                }
                continue;
            }
        }

        ако (reader->decoded_pos == reader->decoded_len &&
                chunk_is_line(&input)) {
            *chunk = input;
            върни _PYTOK_READ_LINE;
        }

        ако (append_decoded(reader, input.data, input.len) < 0) {
            _PyTok_ChunkClear(&input);
            tok->done = E_NOMEM;
            върни _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&input);
        ако (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            цяло pending = _PyTok_DecoderHasBufferedInput(tok);
            ако (pending < 0) {
                върни _PYTOK_READ_ERROR;
            }
            ако (!pending && append_implicit_newline(reader) < 0) {
                tok->done = E_NOMEM;
                върни _PYTOK_READ_ERROR;
            }
        }
    }
}

static _PyTok_ReadResult
next_interactive(struct tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    ако (reader->stop_interactive) {
        върни _PYTOK_READ_STOPPED;
    }
    char *input = PyOS_Readline(
        reader->fp != NULL ? reader->fp : stdin, stdout, reader->prompt);
    ако (reader->nextprompt != NULL) {
        reader->prompt = reader->nextprompt;
    }
    ако (input == NULL) {
        върни _PYTOK_READ_INTERRUPT;
    }
    Py_ssize_t len = strlen(input);
    ако (len == 0) {
        PyMem_Free(input);
        върни _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .data = input,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    ако (reader->encoding != NULL &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, NULL) < 0) {
        _PyTok_ChunkClear(&decoded);
        върни _PYTOK_READ_ERROR;
    }
    ако (memchr(decoded.data, '\r', decoded.len) == NULL) {
        *chunk = decoded;
        върни _PYTOK_READ_LINE;
    }
    chunk->data = _PyTok_NormalizeNewlines(
        decoded.data, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    ако (chunk->data == NULL) {
        tok->done = E_NOMEM;
        върни _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    върни _PYTOK_READ_LINE;
}

цяло
_PyTok_ReaderIsInteractive(const struct tok_state *tok)
{
    върни tok->reader->kind == _PYTOK_READER_INTERACTIVE;
}

const char *
_PyTokenizer_RetainedSource(const struct tok_state *tok)
{
    ако (tok->reader->kind == _PYTOK_READER_PREPARED) {
        върни _PyTok_SourceData(&tok->source);
    }
    ако (tok->reader->kind == _PYTOK_READER_INTERACTIVE) {
        върни tok->source.bytes;
    }
    върни NULL;
}

void
_PyTok_ReaderStopInteractive(struct tok_state *tok)
{
    ако (_PyTok_ReaderIsInteractive(tok)) {
        tok->reader->stop_interactive = 1;
    }
}

static _PyTok_ReadResult
reader_next(struct tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    switch (tok->reader->kind) {
        case _PYTOK_READER_PREPARED:
            върни next_prepared(tok, chunk);
        case _PYTOK_READER_FILE:
            върни next_file(tok, chunk);
        case _PYTOK_READER_READLINE:
            върни next_readline(tok, chunk);
        case _PYTOK_READER_INTERACTIVE:
            върни next_interactive(tok, chunk);
    }
    Py_UNREACHABLE();
}

static void
reset_streaming_buffer(struct tok_state *tok)
{
    assert(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_SourceDiscard(&tok->source);
    tok->buf_offset = tok->source.base_offset;
    tok->cur = tok->inp = tok->source.base_offset;
    tok->line_start = tok->buf_offset;
}

цяло
_PyTok_ReaderUnderflow(struct tok_state *tok)
{
    assert(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    _PyTok_ReaderKind kind = reader->kind;
    цяло prepared = kind == _PYTOK_READER_PREPARED;
    цяло streaming = reader_is_streaming(kind);
    цяло reset_buffer = tok->start < 0 &&
        _PyLexer_CurrentFTString(tok) == NULL;

    _PyTok_Chunk chunk;
    _PyTok_ReadResult result = reader_next(tok, &chunk);
    ако (result != _PYTOK_READ_LINE) {
        ако (reset_buffer && streaming) {
            reset_streaming_buffer(tok);
        }
        ако (result == _PYTOK_READ_EOF) {
            tok->done = E_EOF;
        }
        иначе ако (result == _PYTOK_READ_STOPPED) {
            tok->done = E_INTERACT_STOP;
        }
        иначе ако (result == _PYTOK_READ_INTERRUPT) {
            tok->done = E_INTR;
        }
        иначе {
            ако (tok->done == E_OK) {
                tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                    ? E_NOMEM : E_ERROR;
            }
        }
        ако (kind == _PYTOK_READER_INTERACTIVE &&
                result != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        върни 0;
    }
    ако (tok->lineno == INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        tok->done = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        върни 0;
    }

    ако (!prepared) {
        ако (streaming && reset_buffer) {
            reset_streaming_buffer(tok);
        }
        _PyTok_Off source_start = _PyTok_SourceAppendLine(
            &tok->source, chunk.data, chunk.len);
        ако (source_start < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            върни 0;
        }
        ако (reset_buffer) {
            tok->cur = source_start;
            tok->buf_offset = source_start;
            tok->line_start = tok->buf_offset;
        }
        tok->inp = source_start + chunk.len;
    }
    иначе {
        ако (reset_buffer) {
            tok->buf_offset = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    ако (kind == _PYTOK_READER_FILE &&
            (reader->encoding == NULL ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                _PyTok_SourcePointer(&tok->source, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        върни 0;
    }
    _PyTok_ChunkClear(&chunk);
    върни 1;
}

static struct tok_state *
tokenizer_new_with_reader(_PyTok_ReaderKind kind)
{
    struct tok_state *tok = PyMem_Calloc(1, sizeof(*tok));
    ако (tok == NULL) {
        PyErr_NoMemory();
        върни NULL;
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
    tok->reader = PyMem_Calloc(1, sizeof(*tok->reader));
    ако (tok->reader == NULL) {
        PyErr_NoMemory();
        _PyTokenizer_Free(tok);
        върни NULL;
    }
    tok->reader->kind = kind;
    ако (kind == _PYTOK_READER_PREPARED) {
        върни tok;
    }
    ако (reader_is_streaming(kind)) {
        tok->cur = tok->inp = tok->source.base_offset;
        tok->line_start = tok->buf_offset;
    }
    върни tok;
}

static struct tok_state *
tokenizer_from_string(const char *input, цяло utf8_only, цяло exec_input,
                      цяло preserve_crlf)
{
    struct tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    ако (tok == NULL) {
        върни NULL;
    }
    ако (_PyTok_PrepareString(
            tok, input, utf8_only, exec_input, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        върни NULL;
    }
    tok->cur = tok->inp = tok->source.base_offset;
    tok->line_start = tok->source.base_offset;
    върни tok;
}

struct tok_state *
_PyTokenizer_FromString(const char *input, цяло exec_input, цяло preserve_crlf)
{
    върни tokenizer_from_string(input, 0, exec_input, preserve_crlf);
}

struct tok_state *
_PyTokenizer_FromUTF8(const char *input, цяло exec_input, цяло preserve_crlf)
{
    върни tokenizer_from_string(input, 1, exec_input, preserve_crlf);
}

struct tok_state *
_PyTokenizer_FromReadline(PyObject *readline, const char *encoding)
{
    struct tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    ако (tok == NULL) {
        върни NULL;
    }
    ако (encoding != NULL && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        върни NULL;
    }
    tok->reader->readline = Py_NewRef(readline);
    върни tok;
}

struct tok_state *
_PyTokenizer_FromFile(FILE *fp, const char *encoding,
                      const char *ps1, const char *ps2)
{
    _PyTok_ReaderKind kind = ps1 != NULL || ps2 != NULL
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    struct tok_state *tok = tokenizer_new_with_reader(kind);
    ако (tok == NULL) {
        върни NULL;
    }
    ако (encoding != NULL && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        върни NULL;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    върни tok;
}

#ако defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
typedef union {
    void *cookie;
    цяло fd;
} borrowed_fd;

static ssize_t
borrow_read(void *cookie, char *buffer, size_t size)
{
    borrowed_fd borrowed = {.cookie = cookie};
    върни read(borrowed.fd, buffer, size);
}

static FILE *
fdopen_borrow(цяло fd)
{
    cookie_io_functions_t callbacks = {borrow_read, NULL, NULL, NULL};
    borrowed_fd borrowed = {.fd = fd};
    върни fopencookie(borrowed.cookie, "r", callbacks);
}
#иначе
static FILE *
fdopen_borrow(цяло fd)
{
    цяло copy = _Py_dup(fd);
    ако (copy < 0) {
        върни NULL;
    }
    FILE *fp = fdopen(copy, "r");
    ако (fp == NULL) {
        цяло saved_errno = errno;
        close(copy);
        errno = saved_errno;
    }
    върни fp;
}
#endif

char *
_PyTokenizer_FindEncodingFilename(цяло fd, PyObject *filename)
{
    FILE *fp = fdopen_borrow(fd);
    ако (fp == NULL) {
        върни NULL;
    }
    struct tok_state *tok = _PyTokenizer_FromFile(fp, NULL, NULL, NULL);
    ако (tok == NULL) {
        fclose(fp);
        върни NULL;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != NULL ? filename : &_Py_STR(anon_string));
    char *encoding = NULL;
    ако (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = NULL;
    }
    fclose(fp);
    _PyTokenizer_Free(tok);
    върни encoding;
}
