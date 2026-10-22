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

static inline ціле
reader_is_streaming(_PyTok_ReaderKind kind)
{
    повернути kind == _PYTOK_READER_FILE || kind == _PYTOK_READER_READLINE;
}

void
_PyTok_ReaderFree(struct tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    якщо (reader == NULL) {
        повернути;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    для (ціле i = 0;
         i < (ціле)Py_ARRAY_LENGTH(reader->prefetched_lines); i++) {
        _PyTok_ChunkClear(&reader->prefetched_lines[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_buffer);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = NULL;
}

ціле
_PyTokenizer_Traverse(struct tok_state *tok, visitproc visit, void *arg)
{
    Py_VISIT(tok->filename);
    Py_VISIT(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_VISIT(reader->readline);
    Py_VISIT(reader->decoder);
    для (ціле i = 0;
         i < (ціле)Py_ARRAY_LENGTH(reader->prefetched_lines); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_lines[i];
        якщо (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_VISIT(chunk->owner);
        }
    }
    повернути 0;
}

ціле
_PyTok_ReserveBuffer(char **buffer, Py_ssize_t *capacity, Py_ssize_t needed,
                     Py_ssize_t initial_capacity)
{
    assert(initial_capacity > 0);
    якщо (needed <= *capacity) {
        повернути 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *capacity > 0 ? *capacity : initial_capacity;
    поки (cap < needed) {
        якщо (cap > PY_SSIZE_T_MAX / READER_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            break;
        }
        cap *= READER_BUFFER_GROWTH_FACTOR;
    }
    char *resized = PyMem_Realloc(*buffer, cap);
    якщо (resized == NULL) {
        PyErr_NoMemory();
        повернути -1;
    }
    *buffer = resized;
    *capacity = cap;
    повернути 0;
}

static ціле
append_decoded(_PyTok_Reader *reader, const char *data, Py_ssize_t len)
{
    якщо (len == 0) {
        повернути 0;
    }
    якщо (reader->decoded_pos > 0) {
        Py_ssize_t remaining = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (size_t)remaining);
        reader->decoded_scan -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = remaining;
    }
    якщо (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            _PyTok_ReserveBuffer(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        PyErr_NoMemory();
        повернути -1;
    }
    memcpy(reader->decoded + reader->decoded_len, data, (size_t)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    повернути 0;
}

static ціле
append_implicit_newline(_PyTok_Reader *reader)
{
    якщо (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        повернути 0;
    }
    якщо (append_decoded(reader, "\n", 1) < 0) {
        повернути -1;
    }
    reader->decoded_tail_is_implicit = 1;
    повернути 0;
}

static ціле
pop_decoded_line(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    assert(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    assert(reader->decoded_scan >= reader->decoded_pos &&
           reader->decoded_scan <= reader->decoded_len);
    якщо (reader->decoded_pos == reader->decoded_len) {
        повернути 0;
    }
    char *start = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    char *newline = memchr(reader->decoded + reader->decoded_scan, '\n',
                           reader->decoded_len - reader->decoded_scan);
    якщо (newline == NULL) {
        reader->decoded_scan = reader->decoded_len;
        повернути 0;
    }
    Py_ssize_t len = newline - start + 1;
    chunk->data = start;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_scan = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_tail_is_implicit;
    якщо (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_scan = 0;
        reader->decoded_tail_is_implicit = 0;
    }
    повернути 1;
}

static ціле
chunk_is_line(const _PyTok_Chunk *chunk)
{
    якщо (chunk->len == 0 || chunk->data[chunk->len - 1] != '\n') {
        повернути 0;
    }
    повернути memchr(chunk->data, '\n', chunk->len - 1) == NULL;
}

static _PyTok_ReadResult
next_prepared(struct tok_state *tok, _PyTok_Chunk *chunk)
{
    якщо (tok->lineno >= tok->source.nlines) {
        повернути _PYTOK_READ_EOF;
    }
    _PyTok_Span tail = {tok->inp, tok->source.base_offset + tok->source.len};
    _PyTok_Off newline = _PyTok_SourceFindByte(&tok->source, tail, '\n');
    _PyTok_Span line = {tail.start, newline >= 0 ? newline + 1 : tail.end};
    chunk->data = (char *)_PyTok_SourceSpanView(
        &tok->source, line, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = line.end == tail.end
        ? tok->reader->prepared_final_newline : 0;
    повернути _PYTOK_READ_LINE;
}

static _PyTok_ReadResult
read_file_line(struct tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    для (;;) {
        якщо (len > PY_SSIZE_T_MAX - BUFSIZ) {
            PyErr_NoMemory();
            повернути _PYTOK_READ_ERROR;
        }
        якщо (_PyTok_ReserveBuffer(&reader->file_buffer, &reader->file_buffer_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            повернути _PYTOK_READ_ERROR;
        }
        ціле available = (ціле)Py_MIN(reader->file_buffer_cap - len, INT_MAX);
        size_t read = 0;
        char *result = _Py_UniversalNewlineFgetsWithSize(
            reader->file_buffer + len, available, reader->fp, NULL, &read);
        якщо (result == NULL) {
            якщо (len == 0) {
                повернути _PYTOK_READ_EOF;
            }
            break;
        }
        len += (Py_ssize_t)read;
        якщо (len > 0 && reader->file_buffer[len - 1] == '\n') {
            break;
        }
    }
    ціле implicit = reader->file_buffer[len - 1] != '\n';
    chunk->data = reader->file_buffer;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    повернути _PYTOK_READ_LINE;
}

static ціле
initialize_file(struct tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    якщо (reader->encoding != NULL) {
        повернути _PyTok_StartDecoder(tok, "strict");
    }

    _PyTok_ReadResult result = read_file_line(
        tok, &reader->prefetched_lines[0]);
    якщо (result == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        повернути 0;
    }
    якщо (result != _PYTOK_READ_LINE) {
        повернути -1;
    }
    Py_ssize_t bom_len;
    _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_lines[0], NULL, 0, &bom_len);
    якщо (detection == _PYTOK_ENCODING_ERROR) {
        повернути -1;
    }
    якщо (detection == _PYTOK_ENCODING_NEED_SECOND_LINE) {
        char *first = _PyTok_CopyBytes(
            reader->prefetched_lines[0].data,
            reader->prefetched_lines[0].len);
        якщо (first == NULL) {
            tok->done = E_NOMEM;
            повернути -1;
        }
        reader->prefetched_lines[0].data = first;
        reader->prefetched_lines[0].ownership = _PYTOK_CHUNK_PYMEM;
        result = read_file_line(tok, &reader->prefetched_lines[1]);
        якщо (result == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        інакше якщо (result != _PYTOK_READ_LINE) {
            повернути -1;
        }
        _PyTok_Chunk *second = reader->prefetched_lines[1].data != NULL
            ? &reader->prefetched_lines[1] : NULL;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_lines[0], second, 1, &bom_len);
        якщо (detection == _PYTOK_ENCODING_ERROR) {
            повернути -1;
        }
    }
    якщо (bom_len != 0) {
        _PyTok_Chunk *first = &reader->prefetched_lines[0];
        якщо (first->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(first->data, first->data + bom_len,
                    (size_t)(first->len - bom_len));
            first->data[first->len - bom_len] = '\0';
        }
        інакше {
            first->data += bom_len;
        }
        first->len -= bom_len;
    }
    якщо (_PyTok_StartDecoder(tok, "strict") < 0) {
        повернути -1;
    }
    повернути 0;
}

static ціле
finalize_decoding(struct tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    якщо (reader->decoder_finalized) {
        повернути 0;
    }
    reader->decoder_finalized = 1;
    якщо (reader->decoder != NULL) {
        _PyTok_Chunk input = {
            .data = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        ціле decoded = _PyTok_DecodeChunk(tok, &input, 1);
        якщо (decoded == 0 &&
                append_decoded(reader, input.data, input.len) < 0) {
            tok->done = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&input);
        якщо (decoded < 0) {
            повернути -1;
        }
    }
    якщо (append_implicit_newline(reader) < 0) {
        tok->done = E_NOMEM;
        повернути -1;
    }
    повернути 0;
}

static _PyTok_ReadResult
next_file(struct tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    якщо (!reader->file_initialized && initialize_file(tok) < 0) {
        повернути _PYTOK_READ_ERROR;
    }
    для (;;) {
        якщо (pop_decoded_line(reader, chunk)) {
            повернути _PYTOK_READ_LINE;
        }
        _PyTok_Chunk input = {0};
        якщо (reader->prefetched_lines[0].data != NULL) {
            input = reader->prefetched_lines[0];
            reader->prefetched_lines[0] = (_PyTok_Chunk){0};
        }
        інакше якщо (reader->prefetched_lines[1].data != NULL) {
            input = reader->prefetched_lines[1];
            reader->prefetched_lines[1] = (_PyTok_Chunk){0};
        }
        інакше якщо (!reader->file_eof) {
            _PyTok_ReadResult result = read_file_line(tok, &input);
            якщо (result == _PYTOK_READ_ERROR) {
                повернути result;
            }
            якщо (result == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        якщо (input.data != NULL) {
            ціле implicit = input.implicit_newline;
            якщо (reader->decoder == NULL && !implicit) {
                *chunk = input;
                повернути _PYTOK_READ_LINE;
            }
            ціле decoded = _PyTok_DecodeChunk(tok, &input, 0);
            якщо (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_line(&input)) {
                *chunk = input;
                повернути _PYTOK_READ_LINE;
            }
            якщо (decoded == 0 &&
                    append_decoded(reader, input.data, input.len) < 0) {
                tok->done = E_NOMEM;
                decoded = -1;
            }
            якщо (decoded == 0 && implicit) {
                reader->decoded_tail_is_implicit = 1;
            }
            _PyTok_ChunkClear(&input);
            якщо (decoded < 0) {
                повернути _PYTOK_READ_ERROR;
            }
            continue;
        }
        якщо (!reader->decoder_finalized) {
            якщо (finalize_decoding(tok) < 0) {
                повернути _PYTOK_READ_ERROR;
            }
            continue;
        }
        повернути _PYTOK_READ_EOF;
    }
}

static _PyTok_ReadResult
next_readline(struct tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    для (;;) {
        якщо (pop_decoded_line(reader, chunk)) {
            повернути _PYTOK_READ_LINE;
        }
        якщо (reader->decoder_finalized) {
            повернути _PYTOK_READ_EOF;
        }

        PyObject *raw = PyObject_CallNoArgs(reader->readline);
        якщо (raw == NULL) {
            якщо (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                якщо (finalize_decoding(tok) < 0) {
                    повернути _PYTOK_READ_ERROR;
                }
                continue;
            }
            повернути _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk input = {0};
        якщо (reader->encoding != NULL) {
            якщо (!PyBytes_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                повернути _PYTOK_READ_ERROR;
            }
            якщо (PyBytes_GET_SIZE(raw) == 0) {
                Py_DECREF(raw);
                якщо (_PyTok_StartDecoder(tok, "replace") < 0) {
                    повернути _PYTOK_READ_ERROR;
                }
                якщо (finalize_decoding(tok) < 0) {
                    повернути _PYTOK_READ_ERROR;
                }
                continue;
            }
            input.owner = raw;
            input.data = PyBytes_AS_STRING(raw);
            input.len = PyBytes_GET_SIZE(raw);
            input.ownership = _PYTOK_CHUNK_PYOBJECT;
            ціле decoded;
            якщо (reader->decoder == NULL &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_line(&input)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &input, "utf-8", "replace");
            }
            інакше {
                decoded = _PyTok_StartDecoder(tok, "replace");
                якщо (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &input, 0);
                }
            }
            якщо (decoded < 0) {
                _PyTok_ChunkClear(&input);
                повернути _PYTOK_READ_ERROR;
            }
        }
        інакше {
            якщо (!PyUnicode_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                повернути _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            const char *utf8 = PyUnicode_AsUTF8AndSize(raw, &utf8_len);
            якщо (utf8 == NULL) {
                Py_DECREF(raw);
                повернути _PYTOK_READ_ERROR;
            }
            input.owner = raw;
            input.data = (char *)utf8;
            input.len = utf8_len;
            input.ownership = _PYTOK_CHUNK_PYOBJECT;
            якщо (input.len == 0) {
                _PyTok_ChunkClear(&input);
                якщо (finalize_decoding(tok) < 0) {
                    повернути _PYTOK_READ_ERROR;
                }
                continue;
            }
        }

        якщо (reader->decoded_pos == reader->decoded_len &&
                chunk_is_line(&input)) {
            *chunk = input;
            повернути _PYTOK_READ_LINE;
        }

        якщо (append_decoded(reader, input.data, input.len) < 0) {
            _PyTok_ChunkClear(&input);
            tok->done = E_NOMEM;
            повернути _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&input);
        якщо (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            ціле pending = _PyTok_DecoderHasBufferedInput(tok);
            якщо (pending < 0) {
                повернути _PYTOK_READ_ERROR;
            }
            якщо (!pending && append_implicit_newline(reader) < 0) {
                tok->done = E_NOMEM;
                повернути _PYTOK_READ_ERROR;
            }
        }
    }
}

static _PyTok_ReadResult
next_interactive(struct tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    якщо (reader->stop_interactive) {
        повернути _PYTOK_READ_STOPPED;
    }
    char *input = PyOS_Readline(
        reader->fp != NULL ? reader->fp : stdin, stdout, reader->prompt);
    якщо (reader->nextprompt != NULL) {
        reader->prompt = reader->nextprompt;
    }
    якщо (input == NULL) {
        повернути _PYTOK_READ_INTERRUPT;
    }
    Py_ssize_t len = strlen(input);
    якщо (len == 0) {
        PyMem_Free(input);
        повернути _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .data = input,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    якщо (reader->encoding != NULL &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, NULL) < 0) {
        _PyTok_ChunkClear(&decoded);
        повернути _PYTOK_READ_ERROR;
    }
    якщо (memchr(decoded.data, '\r', decoded.len) == NULL) {
        *chunk = decoded;
        повернути _PYTOK_READ_LINE;
    }
    chunk->data = _PyTok_NormalizeNewlines(
        decoded.data, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    якщо (chunk->data == NULL) {
        tok->done = E_NOMEM;
        повернути _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    повернути _PYTOK_READ_LINE;
}

ціле
_PyTok_ReaderIsInteractive(const struct tok_state *tok)
{
    повернути tok->reader->kind == _PYTOK_READER_INTERACTIVE;
}

const char *
_PyTokenizer_RetainedSource(const struct tok_state *tok)
{
    якщо (tok->reader->kind == _PYTOK_READER_PREPARED) {
        повернути _PyTok_SourceData(&tok->source);
    }
    якщо (tok->reader->kind == _PYTOK_READER_INTERACTIVE) {
        повернути tok->source.bytes;
    }
    повернути NULL;
}

void
_PyTok_ReaderStopInteractive(struct tok_state *tok)
{
    якщо (_PyTok_ReaderIsInteractive(tok)) {
        tok->reader->stop_interactive = 1;
    }
}

static _PyTok_ReadResult
reader_next(struct tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    switch (tok->reader->kind) {
        case _PYTOK_READER_PREPARED:
            повернути next_prepared(tok, chunk);
        case _PYTOK_READER_FILE:
            повернути next_file(tok, chunk);
        case _PYTOK_READER_READLINE:
            повернути next_readline(tok, chunk);
        case _PYTOK_READER_INTERACTIVE:
            повернути next_interactive(tok, chunk);
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

ціле
_PyTok_ReaderUnderflow(struct tok_state *tok)
{
    assert(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    _PyTok_ReaderKind kind = reader->kind;
    ціле prepared = kind == _PYTOK_READER_PREPARED;
    ціле streaming = reader_is_streaming(kind);
    ціле reset_buffer = tok->start < 0 &&
        _PyLexer_CurrentFTString(tok) == NULL;

    _PyTok_Chunk chunk;
    _PyTok_ReadResult result = reader_next(tok, &chunk);
    якщо (result != _PYTOK_READ_LINE) {
        якщо (reset_buffer && streaming) {
            reset_streaming_buffer(tok);
        }
        якщо (result == _PYTOK_READ_EOF) {
            tok->done = E_EOF;
        }
        інакше якщо (result == _PYTOK_READ_STOPPED) {
            tok->done = E_INTERACT_STOP;
        }
        інакше якщо (result == _PYTOK_READ_INTERRUPT) {
            tok->done = E_INTR;
        }
        інакше {
            якщо (tok->done == E_OK) {
                tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                    ? E_NOMEM : E_ERROR;
            }
        }
        якщо (kind == _PYTOK_READER_INTERACTIVE &&
                result != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        повернути 0;
    }
    якщо (tok->lineno == INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        tok->done = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        повернути 0;
    }

    якщо (!prepared) {
        якщо (streaming && reset_buffer) {
            reset_streaming_buffer(tok);
        }
        _PyTok_Off source_start = _PyTok_SourceAppendLine(
            &tok->source, chunk.data, chunk.len);
        якщо (source_start < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            повернути 0;
        }
        якщо (reset_buffer) {
            tok->cur = source_start;
            tok->buf_offset = source_start;
            tok->line_start = tok->buf_offset;
        }
        tok->inp = source_start + chunk.len;
    }
    інакше {
        якщо (reset_buffer) {
            tok->buf_offset = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    якщо (kind == _PYTOK_READER_FILE &&
            (reader->encoding == NULL ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                _PyTok_SourcePointer(&tok->source, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        повернути 0;
    }
    _PyTok_ChunkClear(&chunk);
    повернути 1;
}

static struct tok_state *
tokenizer_new_with_reader(_PyTok_ReaderKind kind)
{
    struct tok_state *tok = PyMem_Calloc(1, sizeof(*tok));
    якщо (tok == NULL) {
        PyErr_NoMemory();
        повернути NULL;
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
    якщо (tok->reader == NULL) {
        PyErr_NoMemory();
        _PyTokenizer_Free(tok);
        повернути NULL;
    }
    tok->reader->kind = kind;
    якщо (kind == _PYTOK_READER_PREPARED) {
        повернути tok;
    }
    якщо (reader_is_streaming(kind)) {
        tok->cur = tok->inp = tok->source.base_offset;
        tok->line_start = tok->buf_offset;
    }
    повернути tok;
}

static struct tok_state *
tokenizer_from_string(const char *input, ціле utf8_only, ціле exec_input,
                      ціле preserve_crlf)
{
    struct tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    якщо (tok == NULL) {
        повернути NULL;
    }
    якщо (_PyTok_PrepareString(
            tok, input, utf8_only, exec_input, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        повернути NULL;
    }
    tok->cur = tok->inp = tok->source.base_offset;
    tok->line_start = tok->source.base_offset;
    повернути tok;
}

struct tok_state *
_PyTokenizer_FromString(const char *input, ціле exec_input, ціле preserve_crlf)
{
    повернути tokenizer_from_string(input, 0, exec_input, preserve_crlf);
}

struct tok_state *
_PyTokenizer_FromUTF8(const char *input, ціле exec_input, ціле preserve_crlf)
{
    повернути tokenizer_from_string(input, 1, exec_input, preserve_crlf);
}

struct tok_state *
_PyTokenizer_FromReadline(PyObject *readline, const char *encoding)
{
    struct tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    якщо (tok == NULL) {
        повернути NULL;
    }
    якщо (encoding != NULL && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        повернути NULL;
    }
    tok->reader->readline = Py_NewRef(readline);
    повернути tok;
}

struct tok_state *
_PyTokenizer_FromFile(FILE *fp, const char *encoding,
                      const char *ps1, const char *ps2)
{
    _PyTok_ReaderKind kind = ps1 != NULL || ps2 != NULL
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    struct tok_state *tok = tokenizer_new_with_reader(kind);
    якщо (tok == NULL) {
        повернути NULL;
    }
    якщо (encoding != NULL && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        повернути NULL;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    повернути tok;
}

#якщо defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
typedef union {
    void *cookie;
    ціле fd;
} borrowed_fd;

static ssize_t
borrow_read(void *cookie, char *buffer, size_t size)
{
    borrowed_fd borrowed = {.cookie = cookie};
    повернути read(borrowed.fd, buffer, size);
}

static FILE *
fdopen_borrow(ціле fd)
{
    cookie_io_functions_t callbacks = {borrow_read, NULL, NULL, NULL};
    borrowed_fd borrowed = {.fd = fd};
    повернути fopencookie(borrowed.cookie, "r", callbacks);
}
#інакше
static FILE *
fdopen_borrow(ціле fd)
{
    ціле copy = _Py_dup(fd);
    якщо (copy < 0) {
        повернути NULL;
    }
    FILE *fp = fdopen(copy, "r");
    якщо (fp == NULL) {
        ціле saved_errno = errno;
        close(copy);
        errno = saved_errno;
    }
    повернути fp;
}
#endif

char *
_PyTokenizer_FindEncodingFilename(ціле fd, PyObject *filename)
{
    FILE *fp = fdopen_borrow(fd);
    якщо (fp == NULL) {
        повернути NULL;
    }
    struct tok_state *tok = _PyTokenizer_FromFile(fp, NULL, NULL, NULL);
    якщо (tok == NULL) {
        fclose(fp);
        повернути NULL;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != NULL ? filename : &_Py_STR(anon_string));
    char *encoding = NULL;
    якщо (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = NULL;
    }
    fclose(fp);
    _PyTokenizer_Free(tok);
    повернути encoding;
}
