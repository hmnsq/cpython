#ضمن "Python.h"
#ضمن "pycore_fileutils.h"
#ضمن "pycore_pystate.h"
#ضمن "pycore_runtime.h"

#ضمن "errcode.h"
#ضمن "helpers.h"
#ضمن "reader.h"
#ضمن "reader_internal.h"
#ضمن "../lexer/state.h"

#define READER_BUFFER_GROWTH_FACTOR 2

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

ساكن مضمن صحيح
reader_is_streaming(_PyTok_ReaderKind kind)
{
    ܦܢܐ kind == _PYTOK_READER_FILE || kind == _PYTOK_READER_READLINE;
}

فراغ
_PyTok_ReaderFree(بنية tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    ܐܢ (reader == عدم) {
        ܦܢܐ;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    لكل (صحيح i = 0;
         i < (صحيح)Py_ARRAY_LENGTH(reader->prefetched_lines); i++) {
        _PyTok_ChunkClear(&reader->prefetched_lines[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_buffer);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = عدم;
}

صحيح
_PyTokenizer_Traverse(بنية tok_state *tok, visitproc visit, فراغ *arg)
{
    Py_VISIT(tok->filename);
    Py_VISIT(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_VISIT(reader->readline);
    Py_VISIT(reader->decoder);
    لكل (صحيح i = 0;
         i < (صحيح)Py_ARRAY_LENGTH(reader->prefetched_lines); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_lines[i];
        ܐܢ (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_VISIT(chunk->owner);
        }
    }
    ܦܢܐ 0;
}

صحيح
_PyTok_ReserveBuffer(حرف **buffer, Py_ssize_t *capacity, Py_ssize_t needed,
                     Py_ssize_t initial_capacity)
{
    أكد(initial_capacity > 0);
    ܐܢ (needed <= *capacity) {
        ܦܢܐ 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *capacity > 0 ? *capacity : initial_capacity;
    ܟܡܐ (cap < needed) {
        ܐܢ (cap > PY_SSIZE_T_MAX / READER_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            اكسر;
        }
        cap *= READER_BUFFER_GROWTH_FACTOR;
    }
    حرف *resized = PyMem_Realloc(*buffer, cap);
    ܐܢ (resized == عدم) {
        PyErr_NoMemory();
        ܦܢܐ -1;
    }
    *buffer = resized;
    *capacity = cap;
    ܦܢܐ 0;
}

ساكن صحيح
append_decoded(_PyTok_Reader *reader, ثابت حرف *data, Py_ssize_t len)
{
    ܐܢ (len == 0) {
        ܦܢܐ 0;
    }
    ܐܢ (reader->decoded_pos > 0) {
        Py_ssize_t remaining = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (size_t)remaining);
        reader->decoded_scan -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = remaining;
    }
    ܐܢ (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            _PyTok_ReserveBuffer(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        PyErr_NoMemory();
        ܦܢܐ -1;
    }
    memcpy(reader->decoded + reader->decoded_len, data, (size_t)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    ܦܢܐ 0;
}

ساكن صحيح
append_implicit_newline(_PyTok_Reader *reader)
{
    ܐܢ (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        ܦܢܐ 0;
    }
    ܐܢ (append_decoded(reader, "\n", 1) < 0) {
        ܦܢܐ -1;
    }
    reader->decoded_tail_is_implicit = 1;
    ܦܢܐ 0;
}

ساكن صحيح
pop_decoded_line(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    أكد(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    أكد(reader->decoded_scan >= reader->decoded_pos &&
           reader->decoded_scan <= reader->decoded_len);
    ܐܢ (reader->decoded_pos == reader->decoded_len) {
        ܦܢܐ 0;
    }
    حرف *start = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    حرف *newline = memchr(reader->decoded + reader->decoded_scan, '\n',
                           reader->decoded_len - reader->decoded_scan);
    ܐܢ (newline == عدم) {
        reader->decoded_scan = reader->decoded_len;
        ܦܢܐ 0;
    }
    Py_ssize_t len = newline - start + 1;
    chunk->data = start;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_scan = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_tail_is_implicit;
    ܐܢ (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_scan = 0;
        reader->decoded_tail_is_implicit = 0;
    }
    ܦܢܐ 1;
}

ساكن صحيح
chunk_is_line(ثابت _PyTok_Chunk *chunk)
{
    ܐܢ (chunk->len == 0 || chunk->data[chunk->len - 1] != '\n') {
        ܦܢܐ 0;
    }
    ܦܢܐ memchr(chunk->data, '\n', chunk->len - 1) == عدم;
}

ساكن _PyTok_ReadResult
next_prepared(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    ܐܢ (tok->lineno >= tok->source.nlines) {
        ܦܢܐ _PYTOK_READ_EOF;
    }
    _PyTok_Span tail = {tok->inp, tok->source.base_offset + tok->source.len};
    _PyTok_Off newline = _PyTok_SourceFindByte(&tok->source, tail, '\n');
    _PyTok_Span line = {tail.start, newline >= 0 ? newline + 1 : tail.end};
    chunk->data = (حرف *)_PyTok_SourceSpanView(
        &tok->source, line, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = line.end == tail.end
        ? tok->reader->prepared_final_newline : 0;
    ܦܢܐ _PYTOK_READ_LINE;
}

ساكن _PyTok_ReadResult
read_file_line(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    لكل (;;) {
        ܐܢ (len > PY_SSIZE_T_MAX - BUFSIZ) {
            PyErr_NoMemory();
            ܦܢܐ _PYTOK_READ_ERROR;
        }
        ܐܢ (_PyTok_ReserveBuffer(&reader->file_buffer, &reader->file_buffer_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            ܦܢܐ _PYTOK_READ_ERROR;
        }
        صحيح available = (صحيح)Py_MIN(reader->file_buffer_cap - len, INT_MAX);
        size_t اقرأ = 0;
        حرف *result = _Py_UniversalNewlineFgetsWithSize(
            reader->file_buffer + len, available, reader->fp, عدم, &اقرأ);
        ܐܢ (result == عدم) {
            ܐܢ (len == 0) {
                ܦܢܐ _PYTOK_READ_EOF;
            }
            اكسر;
        }
        len += (Py_ssize_t)اقرأ;
        ܐܢ (len > 0 && reader->file_buffer[len - 1] == '\n') {
            اكسر;
        }
    }
    صحيح implicit = reader->file_buffer[len - 1] != '\n';
    chunk->data = reader->file_buffer;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    ܦܢܐ _PYTOK_READ_LINE;
}

ساكن صحيح
initialize_file(بنية tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    ܐܢ (reader->encoding != عدم) {
        ܦܢܐ _PyTok_StartDecoder(tok, "strict");
    }

    _PyTok_ReadResult result = read_file_line(
        tok, &reader->prefetched_lines[0]);
    ܐܢ (result == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        ܦܢܐ 0;
    }
    ܐܢ (result != _PYTOK_READ_LINE) {
        ܦܢܐ -1;
    }
    Py_ssize_t bom_len;
    _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_lines[0], عدم, 0, &bom_len);
    ܐܢ (detection == _PYTOK_ENCODING_ERROR) {
        ܦܢܐ -1;
    }
    ܐܢ (detection == _PYTOK_ENCODING_NEED_SECOND_LINE) {
        حرف *first = _PyTok_CopyBytes(
            reader->prefetched_lines[0].data,
            reader->prefetched_lines[0].len);
        ܐܢ (first == عدم) {
            tok->done = E_NOMEM;
            ܦܢܐ -1;
        }
        reader->prefetched_lines[0].data = first;
        reader->prefetched_lines[0].ownership = _PYTOK_CHUNK_PYMEM;
        result = read_file_line(tok, &reader->prefetched_lines[1]);
        ܐܢ (result == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        ܐܚܪܢܐ ܐܢ (result != _PYTOK_READ_LINE) {
            ܦܢܐ -1;
        }
        _PyTok_Chunk *second = reader->prefetched_lines[1].data != عدم
            ? &reader->prefetched_lines[1] : عدم;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_lines[0], second, 1, &bom_len);
        ܐܢ (detection == _PYTOK_ENCODING_ERROR) {
            ܦܢܐ -1;
        }
    }
    ܐܢ (bom_len != 0) {
        _PyTok_Chunk *first = &reader->prefetched_lines[0];
        ܐܢ (first->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(first->data, first->data + bom_len,
                    (size_t)(first->len - bom_len));
            first->data[first->len - bom_len] = '\0';
        }
        ܐܚܪܢܐ {
            first->data += bom_len;
        }
        first->len -= bom_len;
    }
    ܐܢ (_PyTok_StartDecoder(tok, "strict") < 0) {
        ܦܢܐ -1;
    }
    ܦܢܐ 0;
}

ساكن صحيح
finalize_decoding(بنية tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    ܐܢ (reader->decoder_finalized) {
        ܦܢܐ 0;
    }
    reader->decoder_finalized = 1;
    ܐܢ (reader->decoder != عدم) {
        _PyTok_Chunk input = {
            .data = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        صحيح decoded = _PyTok_DecodeChunk(tok, &input, 1);
        ܐܢ (decoded == 0 &&
                append_decoded(reader, input.data, input.len) < 0) {
            tok->done = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&input);
        ܐܢ (decoded < 0) {
            ܦܢܐ -1;
        }
    }
    ܐܢ (append_implicit_newline(reader) < 0) {
        tok->done = E_NOMEM;
        ܦܢܐ -1;
    }
    ܦܢܐ 0;
}

ساكن _PyTok_ReadResult
next_file(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    ܐܢ (!reader->file_initialized && initialize_file(tok) < 0) {
        ܦܢܐ _PYTOK_READ_ERROR;
    }
    لكل (;;) {
        ܐܢ (pop_decoded_line(reader, chunk)) {
            ܦܢܐ _PYTOK_READ_LINE;
        }
        _PyTok_Chunk input = {0};
        ܐܢ (reader->prefetched_lines[0].data != عدم) {
            input = reader->prefetched_lines[0];
            reader->prefetched_lines[0] = (_PyTok_Chunk){0};
        }
        ܐܚܪܢܐ ܐܢ (reader->prefetched_lines[1].data != عدم) {
            input = reader->prefetched_lines[1];
            reader->prefetched_lines[1] = (_PyTok_Chunk){0};
        }
        ܐܚܪܢܐ ܐܢ (!reader->file_eof) {
            _PyTok_ReadResult result = read_file_line(tok, &input);
            ܐܢ (result == _PYTOK_READ_ERROR) {
                ܦܢܐ result;
            }
            ܐܢ (result == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        ܐܢ (input.data != عدم) {
            صحيح implicit = input.implicit_newline;
            ܐܢ (reader->decoder == عدم && !implicit) {
                *chunk = input;
                ܦܢܐ _PYTOK_READ_LINE;
            }
            صحيح decoded = _PyTok_DecodeChunk(tok, &input, 0);
            ܐܢ (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_line(&input)) {
                *chunk = input;
                ܦܢܐ _PYTOK_READ_LINE;
            }
            ܐܢ (decoded == 0 &&
                    append_decoded(reader, input.data, input.len) < 0) {
                tok->done = E_NOMEM;
                decoded = -1;
            }
            ܐܢ (decoded == 0 && implicit) {
                reader->decoded_tail_is_implicit = 1;
            }
            _PyTok_ChunkClear(&input);
            ܐܢ (decoded < 0) {
                ܦܢܐ _PYTOK_READ_ERROR;
            }
            استمر;
        }
        ܐܢ (!reader->decoder_finalized) {
            ܐܢ (finalize_decoding(tok) < 0) {
                ܦܢܐ _PYTOK_READ_ERROR;
            }
            استمر;
        }
        ܦܢܐ _PYTOK_READ_EOF;
    }
}

ساكن _PyTok_ReadResult
next_readline(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    لكل (;;) {
        ܐܢ (pop_decoded_line(reader, chunk)) {
            ܦܢܐ _PYTOK_READ_LINE;
        }
        ܐܢ (reader->decoder_finalized) {
            ܦܢܐ _PYTOK_READ_EOF;
        }

        PyObject *raw = PyObject_CallNoArgs(reader->readline);
        ܐܢ (raw == عدم) {
            ܐܢ (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                ܐܢ (finalize_decoding(tok) < 0) {
                    ܦܢܐ _PYTOK_READ_ERROR;
                }
                استمر;
            }
            ܦܢܐ _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk input = {0};
        ܐܢ (reader->encoding != عدم) {
            ܐܢ (!PyBytes_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                ܦܢܐ _PYTOK_READ_ERROR;
            }
            ܐܢ (PyBytes_GET_SIZE(raw) == 0) {
                Py_DECREF(raw);
                ܐܢ (_PyTok_StartDecoder(tok, "replace") < 0) {
                    ܦܢܐ _PYTOK_READ_ERROR;
                }
                ܐܢ (finalize_decoding(tok) < 0) {
                    ܦܢܐ _PYTOK_READ_ERROR;
                }
                استمر;
            }
            input.owner = raw;
            input.data = PyBytes_AS_STRING(raw);
            input.len = PyBytes_GET_SIZE(raw);
            input.ownership = _PYTOK_CHUNK_PYOBJECT;
            صحيح decoded;
            ܐܢ (reader->decoder == عدم &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_line(&input)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &input, "utf-8", "replace");
            }
            ܐܚܪܢܐ {
                decoded = _PyTok_StartDecoder(tok, "replace");
                ܐܢ (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &input, 0);
                }
            }
            ܐܢ (decoded < 0) {
                _PyTok_ChunkClear(&input);
                ܦܢܐ _PYTOK_READ_ERROR;
            }
        }
        ܐܚܪܢܐ {
            ܐܢ (!PyUnicode_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                ܦܢܐ _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            ثابت حرف *utf8 = PyUnicode_AsUTF8AndSize(raw, &utf8_len);
            ܐܢ (utf8 == عدم) {
                Py_DECREF(raw);
                ܦܢܐ _PYTOK_READ_ERROR;
            }
            input.owner = raw;
            input.data = (حرف *)utf8;
            input.len = utf8_len;
            input.ownership = _PYTOK_CHUNK_PYOBJECT;
            ܐܢ (input.len == 0) {
                _PyTok_ChunkClear(&input);
                ܐܢ (finalize_decoding(tok) < 0) {
                    ܦܢܐ _PYTOK_READ_ERROR;
                }
                استمر;
            }
        }

        ܐܢ (reader->decoded_pos == reader->decoded_len &&
                chunk_is_line(&input)) {
            *chunk = input;
            ܦܢܐ _PYTOK_READ_LINE;
        }

        ܐܢ (append_decoded(reader, input.data, input.len) < 0) {
            _PyTok_ChunkClear(&input);
            tok->done = E_NOMEM;
            ܦܢܐ _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&input);
        ܐܢ (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            صحيح pending = _PyTok_DecoderHasBufferedInput(tok);
            ܐܢ (pending < 0) {
                ܦܢܐ _PYTOK_READ_ERROR;
            }
            ܐܢ (!pending && append_implicit_newline(reader) < 0) {
                tok->done = E_NOMEM;
                ܦܢܐ _PYTOK_READ_ERROR;
            }
        }
    }
}

ساكن _PyTok_ReadResult
next_interactive(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    ܐܢ (reader->stop_interactive) {
        ܦܢܐ _PYTOK_READ_STOPPED;
    }
    حرف *input = PyOS_Readline(
        reader->fp != عدم ? reader->fp : stdin, stdout, reader->prompt);
    ܐܢ (reader->nextprompt != عدم) {
        reader->prompt = reader->nextprompt;
    }
    ܐܢ (input == عدم) {
        ܦܢܐ _PYTOK_READ_INTERRUPT;
    }
    Py_ssize_t len = strlen(input);
    ܐܢ (len == 0) {
        PyMem_Free(input);
        ܦܢܐ _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .data = input,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    ܐܢ (reader->encoding != عدم &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, عدم) < 0) {
        _PyTok_ChunkClear(&decoded);
        ܦܢܐ _PYTOK_READ_ERROR;
    }
    ܐܢ (memchr(decoded.data, '\r', decoded.len) == عدم) {
        *chunk = decoded;
        ܦܢܐ _PYTOK_READ_LINE;
    }
    chunk->data = _PyTok_NormalizeNewlines(
        decoded.data, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    ܐܢ (chunk->data == عدم) {
        tok->done = E_NOMEM;
        ܦܢܐ _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    ܦܢܐ _PYTOK_READ_LINE;
}

صحيح
_PyTok_ReaderIsInteractive(ثابت بنية tok_state *tok)
{
    ܦܢܐ tok->reader->kind == _PYTOK_READER_INTERACTIVE;
}

ثابت حرف *
_PyTokenizer_RetainedSource(ثابت بنية tok_state *tok)
{
    ܐܢ (tok->reader->kind == _PYTOK_READER_PREPARED) {
        ܦܢܐ _PyTok_SourceData(&tok->source);
    }
    ܐܢ (tok->reader->kind == _PYTOK_READER_INTERACTIVE) {
        ܦܢܐ tok->source.bytes;
    }
    ܦܢܐ عدم;
}

فراغ
_PyTok_ReaderStopInteractive(بنية tok_state *tok)
{
    ܐܢ (_PyTok_ReaderIsInteractive(tok)) {
        tok->reader->stop_interactive = 1;
    }
}

ساكن _PyTok_ReadResult
reader_next(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    بدل (tok->reader->kind) {
        حالة _PYTOK_READER_PREPARED:
            ܦܢܐ next_prepared(tok, chunk);
        حالة _PYTOK_READER_FILE:
            ܦܢܐ next_file(tok, chunk);
        حالة _PYTOK_READER_READLINE:
            ܦܢܐ next_readline(tok, chunk);
        حالة _PYTOK_READER_INTERACTIVE:
            ܦܢܐ next_interactive(tok, chunk);
    }
    Py_UNREACHABLE();
}

ساكن فراغ
reset_streaming_buffer(بنية tok_state *tok)
{
    أكد(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_SourceDiscard(&tok->source);
    tok->buf_offset = tok->source.base_offset;
    tok->cur = tok->inp = tok->source.base_offset;
    tok->line_start = tok->buf_offset;
}

صحيح
_PyTok_ReaderUnderflow(بنية tok_state *tok)
{
    أكد(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    _PyTok_ReaderKind kind = reader->kind;
    صحيح prepared = kind == _PYTOK_READER_PREPARED;
    صحيح streaming = reader_is_streaming(kind);
    صحيح reset_buffer = tok->start < 0 &&
        _PyLexer_CurrentFTString(tok) == عدم;

    _PyTok_Chunk chunk;
    _PyTok_ReadResult result = reader_next(tok, &chunk);
    ܐܢ (result != _PYTOK_READ_LINE) {
        ܐܢ (reset_buffer && streaming) {
            reset_streaming_buffer(tok);
        }
        ܐܢ (result == _PYTOK_READ_EOF) {
            tok->done = E_EOF;
        }
        ܐܚܪܢܐ ܐܢ (result == _PYTOK_READ_STOPPED) {
            tok->done = E_INTERACT_STOP;
        }
        ܐܚܪܢܐ ܐܢ (result == _PYTOK_READ_INTERRUPT) {
            tok->done = E_INTR;
        }
        ܐܚܪܢܐ {
            ܐܢ (tok->done == E_OK) {
                tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                    ? E_NOMEM : E_ERROR;
            }
        }
        ܐܢ (kind == _PYTOK_READER_INTERACTIVE &&
                result != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        ܦܢܐ 0;
    }
    ܐܢ (tok->lineno == INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        tok->done = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        ܦܢܐ 0;
    }

    ܐܢ (!prepared) {
        ܐܢ (streaming && reset_buffer) {
            reset_streaming_buffer(tok);
        }
        _PyTok_Off source_start = _PyTok_SourceAppendLine(
            &tok->source, chunk.data, chunk.len);
        ܐܢ (source_start < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            ܦܢܐ 0;
        }
        ܐܢ (reset_buffer) {
            tok->cur = source_start;
            tok->buf_offset = source_start;
            tok->line_start = tok->buf_offset;
        }
        tok->inp = source_start + chunk.len;
    }
    ܐܚܪܢܐ {
        ܐܢ (reset_buffer) {
            tok->buf_offset = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    ܐܢ (kind == _PYTOK_READER_FILE &&
            (reader->encoding == عدم ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                _PyTok_SourcePointer(&tok->source, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        ܦܢܐ 0;
    }
    _PyTok_ChunkClear(&chunk);
    ܦܢܐ 1;
}

ساكن بنية tok_state *
tokenizer_new_with_reader(_PyTok_ReaderKind kind)
{
    بنية tok_state *tok = PyMem_Calloc(1, حجم(*tok));
    ܐܢ (tok == عدم) {
        PyErr_NoMemory();
        ܦܢܐ عدم;
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
    tok->reader = PyMem_Calloc(1, حجم(*tok->reader));
    ܐܢ (tok->reader == عدم) {
        PyErr_NoMemory();
        _PyTokenizer_Free(tok);
        ܦܢܐ عدم;
    }
    tok->reader->kind = kind;
    ܐܢ (kind == _PYTOK_READER_PREPARED) {
        ܦܢܐ tok;
    }
    ܐܢ (reader_is_streaming(kind)) {
        tok->cur = tok->inp = tok->source.base_offset;
        tok->line_start = tok->buf_offset;
    }
    ܦܢܐ tok;
}

ساكن بنية tok_state *
tokenizer_from_string(ثابت حرف *input, صحيح utf8_only, صحيح exec_input,
                      صحيح preserve_crlf)
{
    بنية tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    ܐܢ (tok == عدم) {
        ܦܢܐ عدم;
    }
    ܐܢ (_PyTok_PrepareString(
            tok, input, utf8_only, exec_input, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        ܦܢܐ عدم;
    }
    tok->cur = tok->inp = tok->source.base_offset;
    tok->line_start = tok->source.base_offset;
    ܦܢܐ tok;
}

بنية tok_state *
_PyTokenizer_FromString(ثابت حرف *input, صحيح exec_input, صحيح preserve_crlf)
{
    ܦܢܐ tokenizer_from_string(input, 0, exec_input, preserve_crlf);
}

بنية tok_state *
_PyTokenizer_FromUTF8(ثابت حرف *input, صحيح exec_input, صحيح preserve_crlf)
{
    ܦܢܐ tokenizer_from_string(input, 1, exec_input, preserve_crlf);
}

بنية tok_state *
_PyTokenizer_FromReadline(PyObject *readline, ثابت حرف *encoding)
{
    بنية tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    ܐܢ (tok == عدم) {
        ܦܢܐ عدم;
    }
    ܐܢ (encoding != عدم && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        ܦܢܐ عدم;
    }
    tok->reader->readline = Py_NewRef(readline);
    ܦܢܐ tok;
}

بنية tok_state *
_PyTokenizer_FromFile(FILE *fp, ثابت حرف *encoding,
                      ثابت حرف *ps1, ثابت حرف *ps2)
{
    _PyTok_ReaderKind kind = ps1 != عدم || ps2 != عدم
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    بنية tok_state *tok = tokenizer_new_with_reader(kind);
    ܐܢ (tok == عدم) {
        ܦܢܐ عدم;
    }
    ܐܢ (encoding != عدم && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        ܦܢܐ عدم;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    ܦܢܐ tok;
}

#ܐܢ defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
تعريفنوع اتحاد {
    فراغ *cookie;
    صحيح fd;
} borrowed_fd;

ساكن ssize_t
borrow_read(فراغ *cookie, حرف *buffer, size_t size)
{
    borrowed_fd borrowed = {.cookie = cookie};
    ܦܢܐ اقرأ(borrowed.fd, buffer, size);
}

ساكن FILE *
fdopen_borrow(صحيح fd)
{
    cookie_io_functions_t callbacks = {borrow_read, عدم, عدم, عدم};
    borrowed_fd borrowed = {.fd = fd};
    ܦܢܐ fopencookie(borrowed.cookie, "r", callbacks);
}
#ܐܚܪܢܐ
ساكن FILE *
fdopen_borrow(صحيح fd)
{
    صحيح copy = _Py_dup(fd);
    ܐܢ (copy < 0) {
        ܦܢܐ عدم;
    }
    FILE *fp = fdopen(copy, "r");
    ܐܢ (fp == عدم) {
        صحيح saved_errno = errno;
        close(copy);
        errno = saved_errno;
    }
    ܦܢܐ fp;
}
#endif

حرف *
_PyTokenizer_FindEncodingFilename(صحيح fd, PyObject *filename)
{
    FILE *fp = fdopen_borrow(fd);
    ܐܢ (fp == عدم) {
        ܦܢܐ عدم;
    }
    بنية tok_state *tok = _PyTokenizer_FromFile(fp, عدم, عدم, عدم);
    ܐܢ (tok == عدم) {
        fclose(fp);
        ܦܢܐ عدم;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != عدم ? filename : &_Py_STR(anon_string));
    حرف *encoding = عدم;
    ܐܢ (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = عدم;
    }
    fclose(fp);
    _PyTokenizer_Free(tok);
    ܦܢܐ encoding;
}
