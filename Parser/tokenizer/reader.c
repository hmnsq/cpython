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
    أرجع kind == _PYTOK_READER_FILE || kind == _PYTOK_READER_READLINE;
}

فراغ
_PyTok_ReaderFree(بنية tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    إذا (reader == عدم) {
        أرجع;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    لكل (صحيح i = 0;
         i < (صحيح)Py_ARRAY_LENGTH(reader->prefetched_قۇرلار); i++) {
        _PyTok_ChunkClear(&reader->prefetched_قۇرلار[i]);
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
         i < (صحيح)Py_ARRAY_LENGTH(reader->prefetched_قۇرلار); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_قۇرلار[i];
        إذا (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_VISIT(chunk->owner);
        }
    }
    أرجع 0;
}

صحيح
_PyTok_ReserveBuffer(حرف **buffer, Py_ssize_t *capacity, Py_ssize_t needed,
                     Py_ssize_t initial_capacity)
{
    أكد(initial_capacity > 0);
    إذا (needed <= *capacity) {
        أرجع 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *capacity > 0 ? *capacity : initial_capacity;
    طالما (cap < needed) {
        إذا (cap > PY_SSIZE_T_MAX / READER_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            اكسر;
        }
        cap *= READER_BUFFER_GROWTH_FACTOR;
    }
    حرف *resized = PyMem_Realloc(*buffer, cap);
    إذا (resized == عدم) {
        PyErr_NoMemory();
        أرجع -1;
    }
    *buffer = resized;
    *capacity = cap;
    أرجع 0;
}

ساكن صحيح
append_decoded(_PyTok_Reader *reader, ثابت حرف *data, Py_ssize_t len)
{
    إذا (len == 0) {
        أرجع 0;
    }
    إذا (reader->decoded_pos > 0) {
        Py_ssize_t remaining = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (size_t)remaining);
        reader->decoded_scan -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = remaining;
    }
    إذا (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            _PyTok_ReserveBuffer(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        PyErr_NoMemory();
        أرجع -1;
    }
    memcpy(reader->decoded + reader->decoded_len, data, (size_t)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    أرجع 0;
}

ساكن صحيح
append_implicit_newline(_PyTok_Reader *reader)
{
    إذا (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        أرجع 0;
    }
    إذا (append_decoded(reader, "\n", 1) < 0) {
        أرجع -1;
    }
    reader->decoded_tail_is_implicit = 1;
    أرجع 0;
}

ساكن صحيح
چىقىرىش_decoded_قۇر(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    أكد(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    أكد(reader->decoded_scan >= reader->decoded_pos &&
           reader->decoded_scan <= reader->decoded_len);
    إذا (reader->decoded_pos == reader->decoded_len) {
        أرجع 0;
    }
    حرف *start = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    حرف *newline = memchr(reader->decoded + reader->decoded_scan, '\n',
                           reader->decoded_len - reader->decoded_scan);
    إذا (newline == عدم) {
        reader->decoded_scan = reader->decoded_len;
        أرجع 0;
    }
    Py_ssize_t len = newline - start + 1;
    chunk->data = start;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_scan = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_tail_is_implicit;
    إذا (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_scan = 0;
        reader->decoded_tail_is_implicit = 0;
    }
    أرجع 1;
}

ساكن صحيح
chunk_is_قۇر(ثابت _PyTok_Chunk *chunk)
{
    إذا (chunk->len == 0 || chunk->data[chunk->len - 1] != '\n') {
        أرجع 0;
    }
    أرجع memchr(chunk->data, '\n', chunk->len - 1) == عدم;
}

ساكن _PyTok_ReadResult
next_prepared(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    إذا (tok->lineno >= tok->source.nlines) {
        أرجع _PYTOK_READ_EOF;
    }
    _PyTok_Span tail = {tok->inp, tok->source.base_offset + tok->source.len};
    _PyTok_Off newline = _PyTok_SourceFindByte(&tok->source, tail, '\n');
    _PyTok_Span قۇر = {tail.start, newline >= 0 ? newline + 1 : tail.end};
    chunk->data = (حرف *)_PyTok_SourceSpanView(
        &tok->source, قۇر, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = قۇر.end == tail.end
        ? tok->reader->prepared_final_newline : 0;
    أرجع PYTOK_READ_قۇر;
}

ساكن _PyTok_ReadResult
read_file_قۇر(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    لكل (;;) {
        إذا (len > PY_SSIZE_T_MAX - BUFSIZ) {
            PyErr_NoMemory();
            أرجع _PYTOK_READ_ERROR;
        }
        إذا (_PyTok_ReserveBuffer(&reader->file_buffer, &reader->file_buffer_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            أرجع _PYTOK_READ_ERROR;
        }
        صحيح available = (صحيح)Py_MIN(reader->file_buffer_cap - len, INT_MAX);
        size_t اقرأ = 0;
        حرف *result = _Py_UniversalNewlineFgetsWithSize(
            reader->file_buffer + len, available, reader->fp, عدم, &اقرأ);
        إذا (result == عدم) {
            إذا (len == 0) {
                أرجع _PYTOK_READ_EOF;
            }
            اكسر;
        }
        len += (Py_ssize_t)اقرأ;
        إذا (len > 0 && reader->file_buffer[len - 1] == '\n') {
            اكسر;
        }
    }
    صحيح implicit = reader->file_buffer[len - 1] != '\n';
    chunk->data = reader->file_buffer;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    أرجع PYTOK_READ_قۇر;
}

ساكن صحيح
initialize_file(بنية tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    إذا (reader->encoding != عدم) {
        أرجع _PyTok_StartDecoder(tok, "strict");
    }

    _PyTok_ReadResult result = read_file_قۇر(
        tok, &reader->prefetched_قۇرلار[0]);
    إذا (result == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        أرجع 0;
    }
    إذا (result != PYTOK_READ_قۇر) {
        أرجع -1;
    }
    Py_ssize_t bom_len;
    _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_قۇرلار[0], عدم, 0, &bom_len);
    إذا (detection == _PYTOK_ENCODING_ERROR) {
        أرجع -1;
    }
    إذا (detection == PYTOK_ENCODING_NEED_SECOND_قۇر) {
        حرف *first = _PyTok_CopyBytes(
            reader->prefetched_قۇرلار[0].data,
            reader->prefetched_قۇرلار[0].len);
        إذا (first == عدم) {
            tok->done = E_NOMEM;
            أرجع -1;
        }
        reader->prefetched_قۇرلار[0].data = first;
        reader->prefetched_قۇرلار[0].ownership = _PYTOK_CHUNK_PYMEM;
        result = read_file_قۇر(tok, &reader->prefetched_قۇرلار[1]);
        إذا (result == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        وإلا إذا (result != PYTOK_READ_قۇر) {
            أرجع -1;
        }
        _PyTok_Chunk *second = reader->prefetched_قۇرلار[1].data != عدم
            ? &reader->prefetched_قۇرلار[1] : عدم;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_قۇرلار[0], second, 1, &bom_len);
        إذا (detection == _PYTOK_ENCODING_ERROR) {
            أرجع -1;
        }
    }
    إذا (bom_len != 0) {
        _PyTok_Chunk *first = &reader->prefetched_قۇرلار[0];
        إذا (first->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(first->data, first->data + bom_len,
                    (size_t)(first->len - bom_len));
            first->data[first->len - bom_len] = '\0';
        }
        وإلا {
            first->data += bom_len;
        }
        first->len -= bom_len;
    }
    إذا (_PyTok_StartDecoder(tok, "strict") < 0) {
        أرجع -1;
    }
    أرجع 0;
}

ساكن صحيح
finalize_decoding(بنية tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    إذا (reader->decoder_finalized) {
        أرجع 0;
    }
    reader->decoder_finalized = 1;
    إذا (reader->decoder != عدم) {
        _PyTok_Chunk كىرگۈزۈش = {
            .data = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        صحيح decoded = _PyTok_DecodeChunk(tok, &كىرگۈزۈش, 1);
        إذا (decoded == 0 &&
                append_decoded(reader, كىرگۈزۈش.data, كىرگۈزۈش.len) < 0) {
            tok->done = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&كىرگۈزۈش);
        إذا (decoded < 0) {
            أرجع -1;
        }
    }
    إذا (append_implicit_newline(reader) < 0) {
        tok->done = E_NOMEM;
        أرجع -1;
    }
    أرجع 0;
}

ساكن _PyTok_ReadResult
next_file(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    إذا (!reader->file_initialized && initialize_file(tok) < 0) {
        أرجع _PYTOK_READ_ERROR;
    }
    لكل (;;) {
        إذا (چىقىرىش_decoded_قۇر(reader, chunk)) {
            أرجع PYTOK_READ_قۇر;
        }
        _PyTok_Chunk كىرگۈزۈش = {0};
        إذا (reader->prefetched_قۇرلار[0].data != عدم) {
            كىرگۈزۈش = reader->prefetched_قۇرلار[0];
            reader->prefetched_قۇرلار[0] = (_PyTok_Chunk){0};
        }
        وإلا إذا (reader->prefetched_قۇرلار[1].data != عدم) {
            كىرگۈزۈش = reader->prefetched_قۇرلار[1];
            reader->prefetched_قۇرلار[1] = (_PyTok_Chunk){0};
        }
        وإلا إذا (!reader->file_eof) {
            _PyTok_ReadResult result = read_file_قۇر(tok, &كىرگۈزۈش);
            إذا (result == _PYTOK_READ_ERROR) {
                أرجع result;
            }
            إذا (result == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        إذا (كىرگۈزۈش.data != عدم) {
            صحيح implicit = كىرگۈزۈش.implicit_newline;
            إذا (reader->decoder == عدم && !implicit) {
                *chunk = كىرگۈزۈش;
                أرجع PYTOK_READ_قۇر;
            }
            صحيح decoded = _PyTok_DecodeChunk(tok, &كىرگۈزۈش, 0);
            إذا (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_قۇر(&كىرگۈزۈش)) {
                *chunk = كىرگۈزۈش;
                أرجع PYTOK_READ_قۇر;
            }
            إذا (decoded == 0 &&
                    append_decoded(reader, كىرگۈزۈش.data, كىرگۈزۈش.len) < 0) {
                tok->done = E_NOMEM;
                decoded = -1;
            }
            إذا (decoded == 0 && implicit) {
                reader->decoded_tail_is_implicit = 1;
            }
            _PyTok_ChunkClear(&كىرگۈزۈش);
            إذا (decoded < 0) {
                أرجع _PYTOK_READ_ERROR;
            }
            استمر;
        }
        إذا (!reader->decoder_finalized) {
            إذا (finalize_decoding(tok) < 0) {
                أرجع _PYTOK_READ_ERROR;
            }
            استمر;
        }
        أرجع _PYTOK_READ_EOF;
    }
}

ساكن _PyTok_ReadResult
next_readline(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    لكل (;;) {
        إذا (چىقىرىش_decoded_قۇر(reader, chunk)) {
            أرجع PYTOK_READ_قۇر;
        }
        إذا (reader->decoder_finalized) {
            أرجع _PYTOK_READ_EOF;
        }

        PyObject *raw = PyObject_CallNoArgs(reader->readline);
        إذا (raw == عدم) {
            إذا (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                إذا (finalize_decoding(tok) < 0) {
                    أرجع _PYTOK_READ_ERROR;
                }
                استمر;
            }
            أرجع _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk كىرگۈزۈش = {0};
        إذا (reader->encoding != عدم) {
            إذا (!PyBytes_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                أرجع _PYTOK_READ_ERROR;
            }
            إذا (PyBytes_GET_SIZE(raw) == 0) {
                Py_DECREF(raw);
                إذا (_PyTok_StartDecoder(tok, "replace") < 0) {
                    أرجع _PYTOK_READ_ERROR;
                }
                إذا (finalize_decoding(tok) < 0) {
                    أرجع _PYTOK_READ_ERROR;
                }
                استمر;
            }
            كىرگۈزۈش.owner = raw;
            كىرگۈزۈش.data = PyBytes_AS_STRING(raw);
            كىرگۈزۈش.len = PyBytes_GET_SIZE(raw);
            كىرگۈزۈش.ownership = _PYTOK_CHUNK_PYOBJECT;
            صحيح decoded;
            إذا (reader->decoder == عدم &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_قۇر(&كىرگۈزۈش)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &كىرگۈزۈش, "utf-8", "replace");
            }
            وإلا {
                decoded = _PyTok_StartDecoder(tok, "replace");
                إذا (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &كىرگۈزۈش, 0);
                }
            }
            إذا (decoded < 0) {
                _PyTok_ChunkClear(&كىرگۈزۈش);
                أرجع _PYTOK_READ_ERROR;
            }
        }
        وإلا {
            إذا (!PyUnicode_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                أرجع _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            ثابت حرف *utf8 = PyUnicode_AsUTF8AndSize(raw, &utf8_len);
            إذا (utf8 == عدم) {
                Py_DECREF(raw);
                أرجع _PYTOK_READ_ERROR;
            }
            كىرگۈزۈش.owner = raw;
            كىرگۈزۈش.data = (حرف *)utf8;
            كىرگۈزۈش.len = utf8_len;
            كىرگۈزۈش.ownership = _PYTOK_CHUNK_PYOBJECT;
            إذا (كىرگۈزۈش.len == 0) {
                _PyTok_ChunkClear(&كىرگۈزۈش);
                إذا (finalize_decoding(tok) < 0) {
                    أرجع _PYTOK_READ_ERROR;
                }
                استمر;
            }
        }

        إذا (reader->decoded_pos == reader->decoded_len &&
                chunk_is_قۇر(&كىرگۈزۈش)) {
            *chunk = كىرگۈزۈش;
            أرجع PYTOK_READ_قۇر;
        }

        إذا (append_decoded(reader, كىرگۈزۈش.data, كىرگۈزۈش.len) < 0) {
            _PyTok_ChunkClear(&كىرگۈزۈش);
            tok->done = E_NOMEM;
            أرجع _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&كىرگۈزۈش);
        إذا (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            صحيح pending = Py_Tok_Decoder_Has_Buffered_كىرگۈزۈش(tok);
            إذا (pending < 0) {
                أرجع _PYTOK_READ_ERROR;
            }
            إذا (!pending && append_implicit_newline(reader) < 0) {
                tok->done = E_NOMEM;
                أرجع _PYTOK_READ_ERROR;
            }
        }
    }
}

ساكن _PyTok_ReadResult
next_interactive(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    إذا (reader->stop_interactive) {
        أرجع _PYTOK_READ_STOPPED;
    }
    حرف *كىرگۈزۈش = PyOS_Readline(
        reader->fp != عدم ? reader->fp : stdin, stdout, reader->prompt);
    إذا (reader->nextprompt != عدم) {
        reader->prompt = reader->nextprompt;
    }
    إذا (كىرگۈزۈش == عدم) {
        أرجع _PYTOK_READ_INTERRUPT;
    }
    Py_ssize_t len = strlen(كىرگۈزۈش);
    إذا (len == 0) {
        PyMem_Free(كىرگۈزۈش);
        أرجع _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .data = كىرگۈزۈش,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    إذا (reader->encoding != عدم &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, عدم) < 0) {
        _PyTok_ChunkClear(&decoded);
        أرجع _PYTOK_READ_ERROR;
    }
    إذا (memchr(decoded.data, '\r', decoded.len) == عدم) {
        *chunk = decoded;
        أرجع PYTOK_READ_قۇر;
    }
    chunk->data = _PyTok_NormalizeNewlines(
        decoded.data, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    إذا (chunk->data == عدم) {
        tok->done = E_NOMEM;
        أرجع _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    أرجع PYTOK_READ_قۇر;
}

صحيح
_PyTok_ReaderIsInteractive(ثابت بنية tok_state *tok)
{
    أرجع tok->reader->kind == _PYTOK_READER_INTERACTIVE;
}

ثابت حرف *
_PyTokenizer_RetainedSource(ثابت بنية tok_state *tok)
{
    إذا (tok->reader->kind == _PYTOK_READER_PREPARED) {
        أرجع _PyTok_SourceData(&tok->source);
    }
    إذا (tok->reader->kind == _PYTOK_READER_INTERACTIVE) {
        أرجع tok->source.bytes;
    }
    أرجع عدم;
}

فراغ
_PyTok_ReaderStopInteractive(بنية tok_state *tok)
{
    إذا (_PyTok_ReaderIsInteractive(tok)) {
        tok->reader->stop_interactive = 1;
    }
}

ساكن _PyTok_ReadResult
reader_next(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    بدل (tok->reader->kind) {
        حالة _PYTOK_READER_PREPARED:
            أرجع next_prepared(tok, chunk);
        حالة _PYTOK_READER_FILE:
            أرجع next_file(tok, chunk);
        حالة _PYTOK_READER_READLINE:
            أرجع next_readline(tok, chunk);
        حالة _PYTOK_READER_INTERACTIVE:
            أرجع next_interactive(tok, chunk);
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
    tok->قۇر_start = tok->buf_offset;
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
    إذا (result != PYTOK_READ_قۇر) {
        إذا (reset_buffer && streaming) {
            reset_streaming_buffer(tok);
        }
        إذا (result == _PYTOK_READ_EOF) {
            tok->done = E_EOF;
        }
        وإلا إذا (result == _PYTOK_READ_STOPPED) {
            tok->done = E_INTERACT_STOP;
        }
        وإلا إذا (result == _PYTOK_READ_INTERRUPT) {
            tok->done = E_INTR;
        }
        وإلا {
            إذا (tok->done == E_OK) {
                tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                    ? E_NOMEM : E_ERROR;
            }
        }
        إذا (kind == _PYTOK_READER_INTERACTIVE &&
                result != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        أرجع 0;
    }
    إذا (tok->lineno == INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        tok->done = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        أرجع 0;
    }

    إذا (!prepared) {
        إذا (streaming && reset_buffer) {
            reset_streaming_buffer(tok);
        }
        _PyTok_Off source_start = Py_Tok_Source_Append_قۇر(
            &tok->source, chunk.data, chunk.len);
        إذا (source_start < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            أرجع 0;
        }
        إذا (reset_buffer) {
            tok->cur = source_start;
            tok->buf_offset = source_start;
            tok->قۇر_start = tok->buf_offset;
        }
        tok->inp = source_start + chunk.len;
    }
    وإلا {
        إذا (reset_buffer) {
            tok->buf_offset = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    إذا (kind == _PYTOK_READER_FILE &&
            (reader->encoding == عدم ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                _PyTok_SourcePointer(&tok->source, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        أرجع 0;
    }
    _PyTok_ChunkClear(&chunk);
    أرجع 1;
}

ساكن بنية tok_state *
tokenizer_new_with_reader(_PyTok_ReaderKind kind)
{
    بنية tok_state *tok = PyMem_Calloc(1, حجم(*tok));
    إذا (tok == عدم) {
        PyErr_NoMemory();
        أرجع عدم;
    }
    tok->start = tok->قۇر_start = -1;
    _PyTok_SourceInit(&tok->source);
    tok->done = E_OK;
    tok->layout.at_bol = 1;
    tok->start_loc = (_PyTok_Loc){-1, -1};
    tok->ftstring_دۆۋە = tok->ftstring_دۆۋە_inline;
    tok->ftstring_capacity = FTSTRING_دۆۋە_INLINE_CAPACITY;
#ifdef Py_DEBUG
    tok->debug = _Py_GetConfig()->parser_debug;
#endif
    tok->reader = PyMem_Calloc(1, حجم(*tok->reader));
    إذا (tok->reader == عدم) {
        PyErr_NoMemory();
        _PyTokenizer_Free(tok);
        أرجع عدم;
    }
    tok->reader->kind = kind;
    إذا (kind == _PYTOK_READER_PREPARED) {
        أرجع tok;
    }
    إذا (reader_is_streaming(kind)) {
        tok->cur = tok->inp = tok->source.base_offset;
        tok->قۇر_start = tok->buf_offset;
    }
    أرجع tok;
}

ساكن بنية tok_state *
tokenizer_from_string(ثابت حرف *كىرگۈزۈش, صحيح utf8_only, صحيح exec_كىرگۈزۈش,
                      صحيح preserve_crlf)
{
    بنية tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    إذا (tok == عدم) {
        أرجع عدم;
    }
    إذا (_PyTok_PrepareString(
            tok, كىرگۈزۈش, utf8_only, exec_كىرگۈزۈش, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        أرجع عدم;
    }
    tok->cur = tok->inp = tok->source.base_offset;
    tok->قۇر_start = tok->source.base_offset;
    أرجع tok;
}

بنية tok_state *
_PyTokenizer_FromString(ثابت حرف *كىرگۈزۈش, صحيح exec_كىرگۈزۈش, صحيح preserve_crlf)
{
    أرجع tokenizer_from_string(كىرگۈزۈش, 0, exec_كىرگۈزۈش, preserve_crlf);
}

بنية tok_state *
_PyTokenizer_FromUTF8(ثابت حرف *كىرگۈزۈش, صحيح exec_كىرگۈزۈش, صحيح preserve_crlf)
{
    أرجع tokenizer_from_string(كىرگۈزۈش, 1, exec_كىرگۈزۈش, preserve_crlf);
}

بنية tok_state *
_PyTokenizer_FromReadline(PyObject *readline, ثابت حرف *encoding)
{
    بنية tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    إذا (tok == عدم) {
        أرجع عدم;
    }
    إذا (encoding != عدم && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        أرجع عدم;
    }
    tok->reader->readline = Py_NewRef(readline);
    أرجع tok;
}

بنية tok_state *
_PyTokenizer_FromFile(FILE *fp, ثابت حرف *encoding,
                      ثابت حرف *ps1, ثابت حرف *ps2)
{
    _PyTok_ReaderKind kind = ps1 != عدم || ps2 != عدم
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    بنية tok_state *tok = tokenizer_new_with_reader(kind);
    إذا (tok == عدم) {
        أرجع عدم;
    }
    إذا (encoding != عدم && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        أرجع عدم;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    أرجع tok;
}

#إذا defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
تعريفنوع اتحاد {
    فراغ *cookie;
    صحيح fd;
} borrowed_fd;

ساكن ssize_t
borrow_read(فراغ *cookie, حرف *buffer, size_t size)
{
    borrowed_fd borrowed = {.cookie = cookie};
    أرجع اقرأ(borrowed.fd, buffer, size);
}

ساكن FILE *
fdopen_borrow(صحيح fd)
{
    cookie_io_functions_t callbacks = {borrow_read, عدم, عدم, عدم};
    borrowed_fd borrowed = {.fd = fd};
    أرجع fopencookie(borrowed.cookie, "r", callbacks);
}
#وإلا
ساكن FILE *
fdopen_borrow(صحيح fd)
{
    صحيح copy = _Py_dup(fd);
    إذا (copy < 0) {
        أرجع عدم;
    }
    FILE *fp = fdopen(copy, "r");
    إذا (fp == عدم) {
        صحيح saved_errno = errno;
        close(copy);
        errno = saved_errno;
    }
    أرجع fp;
}
#endif

حرف *
_PyTokenizer_FindEncodingFilename(صحيح fd, PyObject *filename)
{
    FILE *fp = fdopen_borrow(fd);
    إذا (fp == عدم) {
        أرجع عدم;
    }
    بنية tok_state *tok = _PyTokenizer_FromFile(fp, عدم, عدم, عدم);
    إذا (tok == عدم) {
        fclose(fp);
        أرجع عدم;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != عدم ? filename : &_Py_STR(anon_string));
    حرف *encoding = عدم;
    إذا (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = عدم;
    }
    fclose(fp);
    _PyTokenizer_Free(tok);
    أرجع encoding;
}
