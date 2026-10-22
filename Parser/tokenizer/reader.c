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

static inline 정수
reader_is_streaming(_PyTok_ReaderKind kind)
{
    반환 kind == _PYTOK_READER_FILE || kind == _PYTOK_READER_READLINE;
}

void
_PyTok_ReaderFree(struct tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    만약 (reader == NULL) {
        반환;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    위해 (정수 i = 0;
         i < (정수)Py_ARRAY_LENGTH(reader->prefetched_lines); i++) {
        _PyTok_ChunkClear(&reader->prefetched_lines[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_buffer);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = NULL;
}

정수
_PyTokenizer_Traverse(struct tok_state *tok, visitproc visit, void *arg)
{
    Py_VISIT(tok->filename);
    Py_VISIT(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_VISIT(reader->readline);
    Py_VISIT(reader->decoder);
    위해 (정수 i = 0;
         i < (정수)Py_ARRAY_LENGTH(reader->prefetched_lines); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_lines[i];
        만약 (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_VISIT(chunk->owner);
        }
    }
    반환 0;
}

정수
_PyTok_ReserveBuffer(char **buffer, Py_ssize_t *capacity, Py_ssize_t needed,
                     Py_ssize_t initial_capacity)
{
    assert(initial_capacity > 0);
    만약 (needed <= *capacity) {
        반환 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *capacity > 0 ? *capacity : initial_capacity;
    동안 (cap < needed) {
        만약 (cap > PY_SSIZE_T_MAX / READER_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            break;
        }
        cap *= READER_BUFFER_GROWTH_FACTOR;
    }
    char *resized = PyMem_Realloc(*buffer, cap);
    만약 (resized == NULL) {
        PyErr_NoMemory();
        반환 -1;
    }
    *buffer = resized;
    *capacity = cap;
    반환 0;
}

static 정수
append_decoded(_PyTok_Reader *reader, const char *data, Py_ssize_t len)
{
    만약 (len == 0) {
        반환 0;
    }
    만약 (reader->decoded_pos > 0) {
        Py_ssize_t remaining = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (size_t)remaining);
        reader->decoded_scan -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = remaining;
    }
    만약 (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            _PyTok_ReserveBuffer(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        PyErr_NoMemory();
        반환 -1;
    }
    memcpy(reader->decoded + reader->decoded_len, data, (size_t)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    반환 0;
}

static 정수
append_implicit_newline(_PyTok_Reader *reader)
{
    만약 (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        반환 0;
    }
    만약 (append_decoded(reader, "\n", 1) < 0) {
        반환 -1;
    }
    reader->decoded_tail_is_implicit = 1;
    반환 0;
}

static 정수
pop_decoded_line(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    assert(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    assert(reader->decoded_scan >= reader->decoded_pos &&
           reader->decoded_scan <= reader->decoded_len);
    만약 (reader->decoded_pos == reader->decoded_len) {
        반환 0;
    }
    char *start = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    char *newline = memchr(reader->decoded + reader->decoded_scan, '\n',
                           reader->decoded_len - reader->decoded_scan);
    만약 (newline == NULL) {
        reader->decoded_scan = reader->decoded_len;
        반환 0;
    }
    Py_ssize_t len = newline - start + 1;
    chunk->data = start;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_scan = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_tail_is_implicit;
    만약 (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_scan = 0;
        reader->decoded_tail_is_implicit = 0;
    }
    반환 1;
}

static 정수
chunk_is_line(const _PyTok_Chunk *chunk)
{
    만약 (chunk->len == 0 || chunk->data[chunk->len - 1] != '\n') {
        반환 0;
    }
    반환 memchr(chunk->data, '\n', chunk->len - 1) == NULL;
}

static _PyTok_ReadResult
next_prepared(struct tok_state *tok, _PyTok_Chunk *chunk)
{
    만약 (tok->lineno >= tok->source.nlines) {
        반환 _PYTOK_READ_EOF;
    }
    _PyTok_Span tail = {tok->inp, tok->source.base_offset + tok->source.len};
    _PyTok_Off newline = _PyTok_SourceFindByte(&tok->source, tail, '\n');
    _PyTok_Span line = {tail.start, newline >= 0 ? newline + 1 : tail.end};
    chunk->data = (char *)_PyTok_SourceSpanView(
        &tok->source, line, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = line.end == tail.end
        ? tok->reader->prepared_final_newline : 0;
    반환 _PYTOK_READ_LINE;
}

static _PyTok_ReadResult
read_file_line(struct tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    위해 (;;) {
        만약 (len > PY_SSIZE_T_MAX - BUFSIZ) {
            PyErr_NoMemory();
            반환 _PYTOK_READ_ERROR;
        }
        만약 (_PyTok_ReserveBuffer(&reader->file_buffer, &reader->file_buffer_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            반환 _PYTOK_READ_ERROR;
        }
        정수 available = (정수)Py_MIN(reader->file_buffer_cap - len, INT_MAX);
        size_t read = 0;
        char *result = _Py_UniversalNewlineFgetsWithSize(
            reader->file_buffer + len, available, reader->fp, NULL, &read);
        만약 (result == NULL) {
            만약 (len == 0) {
                반환 _PYTOK_READ_EOF;
            }
            break;
        }
        len += (Py_ssize_t)read;
        만약 (len > 0 && reader->file_buffer[len - 1] == '\n') {
            break;
        }
    }
    정수 implicit = reader->file_buffer[len - 1] != '\n';
    chunk->data = reader->file_buffer;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    반환 _PYTOK_READ_LINE;
}

static 정수
initialize_file(struct tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    만약 (reader->encoding != NULL) {
        반환 _PyTok_StartDecoder(tok, "strict");
    }

    _PyTok_ReadResult result = read_file_line(
        tok, &reader->prefetched_lines[0]);
    만약 (result == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        반환 0;
    }
    만약 (result != _PYTOK_READ_LINE) {
        반환 -1;
    }
    Py_ssize_t bom_len;
    _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_lines[0], NULL, 0, &bom_len);
    만약 (detection == _PYTOK_ENCODING_ERROR) {
        반환 -1;
    }
    만약 (detection == _PYTOK_ENCODING_NEED_SECOND_LINE) {
        char *first = _PyTok_CopyBytes(
            reader->prefetched_lines[0].data,
            reader->prefetched_lines[0].len);
        만약 (first == NULL) {
            tok->done = E_NOMEM;
            반환 -1;
        }
        reader->prefetched_lines[0].data = first;
        reader->prefetched_lines[0].ownership = _PYTOK_CHUNK_PYMEM;
        result = read_file_line(tok, &reader->prefetched_lines[1]);
        만약 (result == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        아니면 만약 (result != _PYTOK_READ_LINE) {
            반환 -1;
        }
        _PyTok_Chunk *second = reader->prefetched_lines[1].data != NULL
            ? &reader->prefetched_lines[1] : NULL;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_lines[0], second, 1, &bom_len);
        만약 (detection == _PYTOK_ENCODING_ERROR) {
            반환 -1;
        }
    }
    만약 (bom_len != 0) {
        _PyTok_Chunk *first = &reader->prefetched_lines[0];
        만약 (first->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(first->data, first->data + bom_len,
                    (size_t)(first->len - bom_len));
            first->data[first->len - bom_len] = '\0';
        }
        아니면 {
            first->data += bom_len;
        }
        first->len -= bom_len;
    }
    만약 (_PyTok_StartDecoder(tok, "strict") < 0) {
        반환 -1;
    }
    반환 0;
}

static 정수
finalize_decoding(struct tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    만약 (reader->decoder_finalized) {
        반환 0;
    }
    reader->decoder_finalized = 1;
    만약 (reader->decoder != NULL) {
        _PyTok_Chunk input = {
            .data = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        정수 decoded = _PyTok_DecodeChunk(tok, &input, 1);
        만약 (decoded == 0 &&
                append_decoded(reader, input.data, input.len) < 0) {
            tok->done = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&input);
        만약 (decoded < 0) {
            반환 -1;
        }
    }
    만약 (append_implicit_newline(reader) < 0) {
        tok->done = E_NOMEM;
        반환 -1;
    }
    반환 0;
}

static _PyTok_ReadResult
next_file(struct tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    만약 (!reader->file_initialized && initialize_file(tok) < 0) {
        반환 _PYTOK_READ_ERROR;
    }
    위해 (;;) {
        만약 (pop_decoded_line(reader, chunk)) {
            반환 _PYTOK_READ_LINE;
        }
        _PyTok_Chunk input = {0};
        만약 (reader->prefetched_lines[0].data != NULL) {
            input = reader->prefetched_lines[0];
            reader->prefetched_lines[0] = (_PyTok_Chunk){0};
        }
        아니면 만약 (reader->prefetched_lines[1].data != NULL) {
            input = reader->prefetched_lines[1];
            reader->prefetched_lines[1] = (_PyTok_Chunk){0};
        }
        아니면 만약 (!reader->file_eof) {
            _PyTok_ReadResult result = read_file_line(tok, &input);
            만약 (result == _PYTOK_READ_ERROR) {
                반환 result;
            }
            만약 (result == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        만약 (input.data != NULL) {
            정수 implicit = input.implicit_newline;
            만약 (reader->decoder == NULL && !implicit) {
                *chunk = input;
                반환 _PYTOK_READ_LINE;
            }
            정수 decoded = _PyTok_DecodeChunk(tok, &input, 0);
            만약 (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_line(&input)) {
                *chunk = input;
                반환 _PYTOK_READ_LINE;
            }
            만약 (decoded == 0 &&
                    append_decoded(reader, input.data, input.len) < 0) {
                tok->done = E_NOMEM;
                decoded = -1;
            }
            만약 (decoded == 0 && implicit) {
                reader->decoded_tail_is_implicit = 1;
            }
            _PyTok_ChunkClear(&input);
            만약 (decoded < 0) {
                반환 _PYTOK_READ_ERROR;
            }
            continue;
        }
        만약 (!reader->decoder_finalized) {
            만약 (finalize_decoding(tok) < 0) {
                반환 _PYTOK_READ_ERROR;
            }
            continue;
        }
        반환 _PYTOK_READ_EOF;
    }
}

static _PyTok_ReadResult
next_readline(struct tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    위해 (;;) {
        만약 (pop_decoded_line(reader, chunk)) {
            반환 _PYTOK_READ_LINE;
        }
        만약 (reader->decoder_finalized) {
            반환 _PYTOK_READ_EOF;
        }

        PyObject *raw = PyObject_CallNoArgs(reader->readline);
        만약 (raw == NULL) {
            만약 (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                만약 (finalize_decoding(tok) < 0) {
                    반환 _PYTOK_READ_ERROR;
                }
                continue;
            }
            반환 _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk input = {0};
        만약 (reader->encoding != NULL) {
            만약 (!PyBytes_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                반환 _PYTOK_READ_ERROR;
            }
            만약 (PyBytes_GET_SIZE(raw) == 0) {
                Py_DECREF(raw);
                만약 (_PyTok_StartDecoder(tok, "replace") < 0) {
                    반환 _PYTOK_READ_ERROR;
                }
                만약 (finalize_decoding(tok) < 0) {
                    반환 _PYTOK_READ_ERROR;
                }
                continue;
            }
            input.owner = raw;
            input.data = PyBytes_AS_STRING(raw);
            input.len = PyBytes_GET_SIZE(raw);
            input.ownership = _PYTOK_CHUNK_PYOBJECT;
            정수 decoded;
            만약 (reader->decoder == NULL &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_line(&input)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &input, "utf-8", "replace");
            }
            아니면 {
                decoded = _PyTok_StartDecoder(tok, "replace");
                만약 (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &input, 0);
                }
            }
            만약 (decoded < 0) {
                _PyTok_ChunkClear(&input);
                반환 _PYTOK_READ_ERROR;
            }
        }
        아니면 {
            만약 (!PyUnicode_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                반환 _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            const char *utf8 = PyUnicode_AsUTF8AndSize(raw, &utf8_len);
            만약 (utf8 == NULL) {
                Py_DECREF(raw);
                반환 _PYTOK_READ_ERROR;
            }
            input.owner = raw;
            input.data = (char *)utf8;
            input.len = utf8_len;
            input.ownership = _PYTOK_CHUNK_PYOBJECT;
            만약 (input.len == 0) {
                _PyTok_ChunkClear(&input);
                만약 (finalize_decoding(tok) < 0) {
                    반환 _PYTOK_READ_ERROR;
                }
                continue;
            }
        }

        만약 (reader->decoded_pos == reader->decoded_len &&
                chunk_is_line(&input)) {
            *chunk = input;
            반환 _PYTOK_READ_LINE;
        }

        만약 (append_decoded(reader, input.data, input.len) < 0) {
            _PyTok_ChunkClear(&input);
            tok->done = E_NOMEM;
            반환 _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&input);
        만약 (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            정수 pending = _PyTok_DecoderHasBufferedInput(tok);
            만약 (pending < 0) {
                반환 _PYTOK_READ_ERROR;
            }
            만약 (!pending && append_implicit_newline(reader) < 0) {
                tok->done = E_NOMEM;
                반환 _PYTOK_READ_ERROR;
            }
        }
    }
}

static _PyTok_ReadResult
next_interactive(struct tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    만약 (reader->stop_interactive) {
        반환 _PYTOK_READ_STOPPED;
    }
    char *input = PyOS_Readline(
        reader->fp != NULL ? reader->fp : stdin, stdout, reader->prompt);
    만약 (reader->nextprompt != NULL) {
        reader->prompt = reader->nextprompt;
    }
    만약 (input == NULL) {
        반환 _PYTOK_READ_INTERRUPT;
    }
    Py_ssize_t len = strlen(input);
    만약 (len == 0) {
        PyMem_Free(input);
        반환 _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .data = input,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    만약 (reader->encoding != NULL &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, NULL) < 0) {
        _PyTok_ChunkClear(&decoded);
        반환 _PYTOK_READ_ERROR;
    }
    만약 (memchr(decoded.data, '\r', decoded.len) == NULL) {
        *chunk = decoded;
        반환 _PYTOK_READ_LINE;
    }
    chunk->data = _PyTok_NormalizeNewlines(
        decoded.data, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    만약 (chunk->data == NULL) {
        tok->done = E_NOMEM;
        반환 _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    반환 _PYTOK_READ_LINE;
}

정수
_PyTok_ReaderIsInteractive(const struct tok_state *tok)
{
    반환 tok->reader->kind == _PYTOK_READER_INTERACTIVE;
}

const char *
_PyTokenizer_RetainedSource(const struct tok_state *tok)
{
    만약 (tok->reader->kind == _PYTOK_READER_PREPARED) {
        반환 _PyTok_SourceData(&tok->source);
    }
    만약 (tok->reader->kind == _PYTOK_READER_INTERACTIVE) {
        반환 tok->source.bytes;
    }
    반환 NULL;
}

void
_PyTok_ReaderStopInteractive(struct tok_state *tok)
{
    만약 (_PyTok_ReaderIsInteractive(tok)) {
        tok->reader->stop_interactive = 1;
    }
}

static _PyTok_ReadResult
reader_next(struct tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    switch (tok->reader->kind) {
        case _PYTOK_READER_PREPARED:
            반환 next_prepared(tok, chunk);
        case _PYTOK_READER_FILE:
            반환 next_file(tok, chunk);
        case _PYTOK_READER_READLINE:
            반환 next_readline(tok, chunk);
        case _PYTOK_READER_INTERACTIVE:
            반환 next_interactive(tok, chunk);
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

정수
_PyTok_ReaderUnderflow(struct tok_state *tok)
{
    assert(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    _PyTok_ReaderKind kind = reader->kind;
    정수 prepared = kind == _PYTOK_READER_PREPARED;
    정수 streaming = reader_is_streaming(kind);
    정수 reset_buffer = tok->start < 0 &&
        _PyLexer_CurrentFTString(tok) == NULL;

    _PyTok_Chunk chunk;
    _PyTok_ReadResult result = reader_next(tok, &chunk);
    만약 (result != _PYTOK_READ_LINE) {
        만약 (reset_buffer && streaming) {
            reset_streaming_buffer(tok);
        }
        만약 (result == _PYTOK_READ_EOF) {
            tok->done = E_EOF;
        }
        아니면 만약 (result == _PYTOK_READ_STOPPED) {
            tok->done = E_INTERACT_STOP;
        }
        아니면 만약 (result == _PYTOK_READ_INTERRUPT) {
            tok->done = E_INTR;
        }
        아니면 {
            만약 (tok->done == E_OK) {
                tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                    ? E_NOMEM : E_ERROR;
            }
        }
        만약 (kind == _PYTOK_READER_INTERACTIVE &&
                result != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        반환 0;
    }
    만약 (tok->lineno == INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        tok->done = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        반환 0;
    }

    만약 (!prepared) {
        만약 (streaming && reset_buffer) {
            reset_streaming_buffer(tok);
        }
        _PyTok_Off source_start = _PyTok_SourceAppendLine(
            &tok->source, chunk.data, chunk.len);
        만약 (source_start < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            반환 0;
        }
        만약 (reset_buffer) {
            tok->cur = source_start;
            tok->buf_offset = source_start;
            tok->line_start = tok->buf_offset;
        }
        tok->inp = source_start + chunk.len;
    }
    아니면 {
        만약 (reset_buffer) {
            tok->buf_offset = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    만약 (kind == _PYTOK_READER_FILE &&
            (reader->encoding == NULL ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                _PyTok_SourcePointer(&tok->source, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        반환 0;
    }
    _PyTok_ChunkClear(&chunk);
    반환 1;
}

static struct tok_state *
tokenizer_new_with_reader(_PyTok_ReaderKind kind)
{
    struct tok_state *tok = PyMem_Calloc(1, sizeof(*tok));
    만약 (tok == NULL) {
        PyErr_NoMemory();
        반환 NULL;
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
    만약 (tok->reader == NULL) {
        PyErr_NoMemory();
        _PyTokenizer_Free(tok);
        반환 NULL;
    }
    tok->reader->kind = kind;
    만약 (kind == _PYTOK_READER_PREPARED) {
        반환 tok;
    }
    만약 (reader_is_streaming(kind)) {
        tok->cur = tok->inp = tok->source.base_offset;
        tok->line_start = tok->buf_offset;
    }
    반환 tok;
}

static struct tok_state *
tokenizer_from_string(const char *input, 정수 utf8_only, 정수 exec_input,
                      정수 preserve_crlf)
{
    struct tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    만약 (tok == NULL) {
        반환 NULL;
    }
    만약 (_PyTok_PrepareString(
            tok, input, utf8_only, exec_input, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        반환 NULL;
    }
    tok->cur = tok->inp = tok->source.base_offset;
    tok->line_start = tok->source.base_offset;
    반환 tok;
}

struct tok_state *
_PyTokenizer_FromString(const char *input, 정수 exec_input, 정수 preserve_crlf)
{
    반환 tokenizer_from_string(input, 0, exec_input, preserve_crlf);
}

struct tok_state *
_PyTokenizer_FromUTF8(const char *input, 정수 exec_input, 정수 preserve_crlf)
{
    반환 tokenizer_from_string(input, 1, exec_input, preserve_crlf);
}

struct tok_state *
_PyTokenizer_FromReadline(PyObject *readline, const char *encoding)
{
    struct tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    만약 (tok == NULL) {
        반환 NULL;
    }
    만약 (encoding != NULL && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        반환 NULL;
    }
    tok->reader->readline = Py_NewRef(readline);
    반환 tok;
}

struct tok_state *
_PyTokenizer_FromFile(FILE *fp, const char *encoding,
                      const char *ps1, const char *ps2)
{
    _PyTok_ReaderKind kind = ps1 != NULL || ps2 != NULL
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    struct tok_state *tok = tokenizer_new_with_reader(kind);
    만약 (tok == NULL) {
        반환 NULL;
    }
    만약 (encoding != NULL && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        반환 NULL;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    반환 tok;
}

#만약 defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
typedef union {
    void *cookie;
    정수 fd;
} borrowed_fd;

static ssize_t
borrow_read(void *cookie, char *buffer, size_t size)
{
    borrowed_fd borrowed = {.cookie = cookie};
    반환 read(borrowed.fd, buffer, size);
}

static FILE *
fdopen_borrow(정수 fd)
{
    cookie_io_functions_t callbacks = {borrow_read, NULL, NULL, NULL};
    borrowed_fd borrowed = {.fd = fd};
    반환 fopencookie(borrowed.cookie, "r", callbacks);
}
#아니면
static FILE *
fdopen_borrow(정수 fd)
{
    정수 copy = _Py_dup(fd);
    만약 (copy < 0) {
        반환 NULL;
    }
    FILE *fp = fdopen(copy, "r");
    만약 (fp == NULL) {
        정수 saved_errno = errno;
        close(copy);
        errno = saved_errno;
    }
    반환 fp;
}
#endif

char *
_PyTokenizer_FindEncodingFilename(정수 fd, PyObject *filename)
{
    FILE *fp = fdopen_borrow(fd);
    만약 (fp == NULL) {
        반환 NULL;
    }
    struct tok_state *tok = _PyTokenizer_FromFile(fp, NULL, NULL, NULL);
    만약 (tok == NULL) {
        fclose(fp);
        반환 NULL;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != NULL ? filename : &_Py_STR(anon_string));
    char *encoding = NULL;
    만약 (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = NULL;
    }
    fclose(fp);
    _PyTokenizer_Free(tok);
    반환 encoding;
}
