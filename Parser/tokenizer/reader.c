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
    إرجاع kind == _PYTOK_READER_FILE || kind == _PYTOK_READER_READLINE;
}

فراغ
_PyTok_ReaderFree(بنية tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    إذا (reader == عدم) {
        إرجاع;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    لكل (صحيح i = 0;
         i < (صحيح)Py_ARRAY_LENGTH(reader->prefetched_أسطر); i++) {
        _PyTok_ChunkClear(&reader->prefetched_أسطر[i]);
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
         i < (صحيح)Py_ARRAY_LENGTH(reader->prefetched_أسطر); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_أسطر[i];
        إذا (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_VISIT(chunk->owner);
        }
    }
    إرجاع 0;
}

صحيح
_PyTok_ReserveBuffer(حرف **buffer, Py_ssize_t *capacity, Py_ssize_t needed,
                     Py_ssize_t initial_capacity)
{
    أكد(initial_capacity > 0);
    إذا (needed <= *capacity) {
        إرجاع 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *capacity > 0 ? *capacity : initial_capacity;
    بينما (cap < needed) {
        إذا (cap > PY_SSIZE_T_MAX / READER_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            اكسر;
        }
        cap *= READER_BUFFER_GROWTH_FACTOR;
    }
    حرف *resized = PyMem_Realloc(*buffer, cap);
    إذا (resized == عدم) {
        PyErr_NoMemory();
        إرجاع -1;
    }
    *buffer = resized;
    *capacity = cap;
    إرجاع 0;
}

ساكن صحيح
append_decoded(_PyTok_Reader *reader, ثابت حرف *data, Py_ssize_t len)
{
    إذا (len == 0) {
        إرجاع 0;
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
        إرجاع -1;
    }
    memcpy(reader->decoded + reader->decoded_len, data, (size_t)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    إرجاع 0;
}

ساكن صحيح
append_implicit_newline(_PyTok_Reader *reader)
{
    إذا (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        إرجاع 0;
    }
    إذا (append_decoded(reader, "\n", 1) < 0) {
        إرجاع -1;
    }
    reader->decoded_tail_is_implicit = 1;
    إرجاع 0;
}

ساكن صحيح
اسحب_decoded_سطر(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    أكد(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    أكد(reader->decoded_scan >= reader->decoded_pos &&
           reader->decoded_scan <= reader->decoded_len);
    إذا (reader->decoded_pos == reader->decoded_len) {
        إرجاع 0;
    }
    حرف *start = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    حرف *newline = memchr(reader->decoded + reader->decoded_scan, '\n',
                           reader->decoded_len - reader->decoded_scan);
    إذا (newline == عدم) {
        reader->decoded_scan = reader->decoded_len;
        إرجاع 0;
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
    إرجاع 1;
}

ساكن صحيح
chunk_is_سطر(ثابت _PyTok_Chunk *chunk)
{
    إذا (chunk->len == 0 || chunk->data[chunk->len - 1] != '\n') {
        إرجاع 0;
    }
    إرجاع memchr(chunk->data, '\n', chunk->len - 1) == عدم;
}

ساكن _PyTok_ReadResult
next_prepared(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    إذا (tok->lineno >= tok->source.nlines) {
        إرجاع _PYTOK_READ_EOF;
    }
    _PyTok_Span tail = {tok->inp, tok->source.base_offset + tok->source.len};
    _PyTok_Off newline = _PyTok_SourceFindByte(&tok->source, tail, '\n');
    _PyTok_Span سطر = {tail.start, newline >= 0 ? newline + 1 : tail.end};
    chunk->data = (حرف *)_PyTok_SourceSpanView(
        &tok->source, سطر, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = سطر.end == tail.end
        ? tok->reader->prepared_final_newline : 0;
    إرجاع PYTOK_READ_سطر;
}

ساكن _PyTok_ReadResult
read_file_سطر(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    لكل (;;) {
        إذا (len > PY_SSIZE_T_MAX - BUFSIZ) {
            PyErr_NoMemory();
            إرجاع _PYTOK_READ_ERROR;
        }
        إذا (_PyTok_ReserveBuffer(&reader->file_buffer, &reader->file_buffer_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            إرجاع _PYTOK_READ_ERROR;
        }
        صحيح available = (صحيح)Py_MIN(reader->file_buffer_cap - len, INT_MAX);
        size_t اقرأ = 0;
        حرف *result = _Py_UniversalNewlineFgetsWithSize(
            reader->file_buffer + len, available, reader->fp, عدم, &اقرأ);
        إذا (result == عدم) {
            إذا (len == 0) {
                إرجاع _PYTOK_READ_EOF;
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
    إرجاع PYTOK_READ_سطر;
}

ساكن صحيح
initialize_file(بنية tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    إذا (reader->encoding != عدم) {
        إرجاع _PyTok_StartDecoder(tok, "strict");
    }

    _PyTok_ReadResult result = read_file_سطر(
        tok, &reader->prefetched_أسطر[0]);
    إذا (result == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        إرجاع 0;
    }
    إذا (result != PYTOK_READ_سطر) {
        إرجاع -1;
    }
    Py_ssize_t bom_len;
    _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_أسطر[0], عدم, 0, &bom_len);
    إذا (detection == _PYTOK_ENCODING_ERROR) {
        إرجاع -1;
    }
    إذا (detection == PYTOK_ENCODING_NEED_SECOND_سطر) {
        حرف *first = _PyTok_CopyBytes(
            reader->prefetched_أسطر[0].data,
            reader->prefetched_أسطر[0].len);
        إذا (first == عدم) {
            tok->done = E_NOMEM;
            إرجاع -1;
        }
        reader->prefetched_أسطر[0].data = first;
        reader->prefetched_أسطر[0].ownership = _PYTOK_CHUNK_PYMEM;
        result = read_file_سطر(tok, &reader->prefetched_أسطر[1]);
        إذا (result == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        وإلا إذا (result != PYTOK_READ_سطر) {
            إرجاع -1;
        }
        _PyTok_Chunk *second = reader->prefetched_أسطر[1].data != عدم
            ? &reader->prefetched_أسطر[1] : عدم;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_أسطر[0], second, 1, &bom_len);
        إذا (detection == _PYTOK_ENCODING_ERROR) {
            إرجاع -1;
        }
    }
    إذا (bom_len != 0) {
        _PyTok_Chunk *first = &reader->prefetched_أسطر[0];
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
        إرجاع -1;
    }
    إرجاع 0;
}

ساكن صحيح
finalize_decoding(بنية tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    إذا (reader->decoder_finalized) {
        إرجاع 0;
    }
    reader->decoder_finalized = 1;
    إذا (reader->decoder != عدم) {
        _PyTok_Chunk إدخال = {
            .data = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        صحيح decoded = _PyTok_DecodeChunk(tok, &إدخال, 1);
        إذا (decoded == 0 &&
                append_decoded(reader, إدخال.data, إدخال.len) < 0) {
            tok->done = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&إدخال);
        إذا (decoded < 0) {
            إرجاع -1;
        }
    }
    إذا (append_implicit_newline(reader) < 0) {
        tok->done = E_NOMEM;
        إرجاع -1;
    }
    إرجاع 0;
}

ساكن _PyTok_ReadResult
next_file(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    إذا (!reader->file_initialized && initialize_file(tok) < 0) {
        إرجاع _PYTOK_READ_ERROR;
    }
    لكل (;;) {
        إذا (اسحب_decoded_سطر(reader, chunk)) {
            إرجاع PYTOK_READ_سطر;
        }
        _PyTok_Chunk إدخال = {0};
        إذا (reader->prefetched_أسطر[0].data != عدم) {
            إدخال = reader->prefetched_أسطر[0];
            reader->prefetched_أسطر[0] = (_PyTok_Chunk){0};
        }
        وإلا إذا (reader->prefetched_أسطر[1].data != عدم) {
            إدخال = reader->prefetched_أسطر[1];
            reader->prefetched_أسطر[1] = (_PyTok_Chunk){0};
        }
        وإلا إذا (!reader->file_eof) {
            _PyTok_ReadResult result = read_file_سطر(tok, &إدخال);
            إذا (result == _PYTOK_READ_ERROR) {
                إرجاع result;
            }
            إذا (result == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        إذا (إدخال.data != عدم) {
            صحيح implicit = إدخال.implicit_newline;
            إذا (reader->decoder == عدم && !implicit) {
                *chunk = إدخال;
                إرجاع PYTOK_READ_سطر;
            }
            صحيح decoded = _PyTok_DecodeChunk(tok, &إدخال, 0);
            إذا (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_سطر(&إدخال)) {
                *chunk = إدخال;
                إرجاع PYTOK_READ_سطر;
            }
            إذا (decoded == 0 &&
                    append_decoded(reader, إدخال.data, إدخال.len) < 0) {
                tok->done = E_NOMEM;
                decoded = -1;
            }
            إذا (decoded == 0 && implicit) {
                reader->decoded_tail_is_implicit = 1;
            }
            _PyTok_ChunkClear(&إدخال);
            إذا (decoded < 0) {
                إرجاع _PYTOK_READ_ERROR;
            }
            استمر;
        }
        إذا (!reader->decoder_finalized) {
            إذا (finalize_decoding(tok) < 0) {
                إرجاع _PYTOK_READ_ERROR;
            }
            استمر;
        }
        إرجاع _PYTOK_READ_EOF;
    }
}

ساكن _PyTok_ReadResult
next_readline(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    لكل (;;) {
        إذا (اسحب_decoded_سطر(reader, chunk)) {
            إرجاع PYTOK_READ_سطر;
        }
        إذا (reader->decoder_finalized) {
            إرجاع _PYTOK_READ_EOF;
        }

        PyObject *raw = PyObject_CallNoArgs(reader->readline);
        إذا (raw == عدم) {
            إذا (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                إذا (finalize_decoding(tok) < 0) {
                    إرجاع _PYTOK_READ_ERROR;
                }
                استمر;
            }
            إرجاع _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk إدخال = {0};
        إذا (reader->encoding != عدم) {
            إذا (!PyBytes_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                إرجاع _PYTOK_READ_ERROR;
            }
            إذا (PyBytes_GET_SIZE(raw) == 0) {
                Py_DECREF(raw);
                إذا (_PyTok_StartDecoder(tok, "replace") < 0) {
                    إرجاع _PYTOK_READ_ERROR;
                }
                إذا (finalize_decoding(tok) < 0) {
                    إرجاع _PYTOK_READ_ERROR;
                }
                استمر;
            }
            إدخال.owner = raw;
            إدخال.data = PyBytes_AS_STRING(raw);
            إدخال.len = PyBytes_GET_SIZE(raw);
            إدخال.ownership = _PYTOK_CHUNK_PYOBJECT;
            صحيح decoded;
            إذا (reader->decoder == عدم &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_سطر(&إدخال)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &إدخال, "utf-8", "replace");
            }
            وإلا {
                decoded = _PyTok_StartDecoder(tok, "replace");
                إذا (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &إدخال, 0);
                }
            }
            إذا (decoded < 0) {
                _PyTok_ChunkClear(&إدخال);
                إرجاع _PYTOK_READ_ERROR;
            }
        }
        وإلا {
            إذا (!PyUnicode_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                إرجاع _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            ثابت حرف *utf8 = PyUnicode_AsUTF8AndSize(raw, &utf8_len);
            إذا (utf8 == عدم) {
                Py_DECREF(raw);
                إرجاع _PYTOK_READ_ERROR;
            }
            إدخال.owner = raw;
            إدخال.data = (حرف *)utf8;
            إدخال.len = utf8_len;
            إدخال.ownership = _PYTOK_CHUNK_PYOBJECT;
            إذا (إدخال.len == 0) {
                _PyTok_ChunkClear(&إدخال);
                إذا (finalize_decoding(tok) < 0) {
                    إرجاع _PYTOK_READ_ERROR;
                }
                استمر;
            }
        }

        إذا (reader->decoded_pos == reader->decoded_len &&
                chunk_is_سطر(&إدخال)) {
            *chunk = إدخال;
            إرجاع PYTOK_READ_سطر;
        }

        إذا (append_decoded(reader, إدخال.data, إدخال.len) < 0) {
            _PyTok_ChunkClear(&إدخال);
            tok->done = E_NOMEM;
            إرجاع _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&إدخال);
        إذا (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            صحيح pending = Py_Tok_Decoder_Has_Buffered_إدخال(tok);
            إذا (pending < 0) {
                إرجاع _PYTOK_READ_ERROR;
            }
            إذا (!pending && append_implicit_newline(reader) < 0) {
                tok->done = E_NOMEM;
                إرجاع _PYTOK_READ_ERROR;
            }
        }
    }
}

ساكن _PyTok_ReadResult
next_interactive(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    إذا (reader->stop_interactive) {
        إرجاع _PYTOK_READ_STOPPED;
    }
    حرف *إدخال = PyOS_Readline(
        reader->fp != عدم ? reader->fp : stdin, stdout, reader->prompt);
    إذا (reader->nextprompt != عدم) {
        reader->prompt = reader->nextprompt;
    }
    إذا (إدخال == عدم) {
        إرجاع _PYTOK_READ_INTERRUPT;
    }
    Py_ssize_t len = strlen(إدخال);
    إذا (len == 0) {
        PyMem_Free(إدخال);
        إرجاع _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .data = إدخال,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    إذا (reader->encoding != عدم &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, عدم) < 0) {
        _PyTok_ChunkClear(&decoded);
        إرجاع _PYTOK_READ_ERROR;
    }
    إذا (memchr(decoded.data, '\r', decoded.len) == عدم) {
        *chunk = decoded;
        إرجاع PYTOK_READ_سطر;
    }
    chunk->data = _PyTok_NormalizeNewlines(
        decoded.data, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    إذا (chunk->data == عدم) {
        tok->done = E_NOMEM;
        إرجاع _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    إرجاع PYTOK_READ_سطر;
}

صحيح
_PyTok_ReaderIsInteractive(ثابت بنية tok_state *tok)
{
    إرجاع tok->reader->kind == _PYTOK_READER_INTERACTIVE;
}

ثابت حرف *
_PyTokenizer_RetainedSource(ثابت بنية tok_state *tok)
{
    إذا (tok->reader->kind == _PYTOK_READER_PREPARED) {
        إرجاع _PyTok_SourceData(&tok->source);
    }
    إذا (tok->reader->kind == _PYTOK_READER_INTERACTIVE) {
        إرجاع tok->source.bytes;
    }
    إرجاع عدم;
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
            إرجاع next_prepared(tok, chunk);
        حالة _PYTOK_READER_FILE:
            إرجاع next_file(tok, chunk);
        حالة _PYTOK_READER_READLINE:
            إرجاع next_readline(tok, chunk);
        حالة _PYTOK_READER_INTERACTIVE:
            إرجاع next_interactive(tok, chunk);
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
    tok->سطر_start = tok->buf_offset;
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
    إذا (result != PYTOK_READ_سطر) {
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
        إرجاع 0;
    }
    إذا (tok->lineno == INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        tok->done = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        إرجاع 0;
    }

    إذا (!prepared) {
        إذا (streaming && reset_buffer) {
            reset_streaming_buffer(tok);
        }
        _PyTok_Off source_start = Py_Tok_Source_Append_سطر(
            &tok->source, chunk.data, chunk.len);
        إذا (source_start < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            إرجاع 0;
        }
        إذا (reset_buffer) {
            tok->cur = source_start;
            tok->buf_offset = source_start;
            tok->سطر_start = tok->buf_offset;
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
        إرجاع 0;
    }
    _PyTok_ChunkClear(&chunk);
    إرجاع 1;
}

ساكن بنية tok_state *
tokenizer_new_with_reader(_PyTok_ReaderKind kind)
{
    بنية tok_state *tok = PyMem_Calloc(1, حجم(*tok));
    إذا (tok == عدم) {
        PyErr_NoMemory();
        إرجاع عدم;
    }
    tok->start = tok->سطر_start = -1;
    _PyTok_SourceInit(&tok->source);
    tok->done = E_OK;
    tok->layout.at_bol = 1;
    tok->start_loc = (_PyTok_Loc){-1, -1};
    tok->ftstring_مكدس = tok->ftstring_مكدس_inline;
    tok->ftstring_capacity = FTSTRING_مكدس_INLINE_CAPACITY;
#ifdef Py_DEBUG
    tok->debug = _Py_GetConfig()->parser_debug;
#endif
    tok->reader = PyMem_Calloc(1, حجم(*tok->reader));
    إذا (tok->reader == عدم) {
        PyErr_NoMemory();
        _PyTokenizer_Free(tok);
        إرجاع عدم;
    }
    tok->reader->kind = kind;
    إذا (kind == _PYTOK_READER_PREPARED) {
        إرجاع tok;
    }
    إذا (reader_is_streaming(kind)) {
        tok->cur = tok->inp = tok->source.base_offset;
        tok->سطر_start = tok->buf_offset;
    }
    إرجاع tok;
}

ساكن بنية tok_state *
tokenizer_from_string(ثابت حرف *إدخال, صحيح utf8_only, صحيح exec_إدخال,
                      صحيح preserve_crlf)
{
    بنية tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    إذا (tok == عدم) {
        إرجاع عدم;
    }
    إذا (_PyTok_PrepareString(
            tok, إدخال, utf8_only, exec_إدخال, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        إرجاع عدم;
    }
    tok->cur = tok->inp = tok->source.base_offset;
    tok->سطر_start = tok->source.base_offset;
    إرجاع tok;
}

بنية tok_state *
_PyTokenizer_FromString(ثابت حرف *إدخال, صحيح exec_إدخال, صحيح preserve_crlf)
{
    إرجاع tokenizer_from_string(إدخال, 0, exec_إدخال, preserve_crlf);
}

بنية tok_state *
_PyTokenizer_FromUTF8(ثابت حرف *إدخال, صحيح exec_إدخال, صحيح preserve_crlf)
{
    إرجاع tokenizer_from_string(إدخال, 1, exec_إدخال, preserve_crlf);
}

بنية tok_state *
_PyTokenizer_FromReadline(PyObject *readline, ثابت حرف *encoding)
{
    بنية tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    إذا (tok == عدم) {
        إرجاع عدم;
    }
    إذا (encoding != عدم && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        إرجاع عدم;
    }
    tok->reader->readline = Py_NewRef(readline);
    إرجاع tok;
}

بنية tok_state *
_PyTokenizer_FromFile(FILE *fp, ثابت حرف *encoding,
                      ثابت حرف *ps1, ثابت حرف *ps2)
{
    _PyTok_ReaderKind kind = ps1 != عدم || ps2 != عدم
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    بنية tok_state *tok = tokenizer_new_with_reader(kind);
    إذا (tok == عدم) {
        إرجاع عدم;
    }
    إذا (encoding != عدم && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        إرجاع عدم;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    إرجاع tok;
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
    إرجاع اقرأ(borrowed.fd, buffer, size);
}

ساكن FILE *
fdopen_borrow(صحيح fd)
{
    cookie_io_functions_t callbacks = {borrow_read, عدم, عدم, عدم};
    borrowed_fd borrowed = {.fd = fd};
    إرجاع fopencookie(borrowed.cookie, "r", callbacks);
}
#وإلا
ساكن FILE *
fdopen_borrow(صحيح fd)
{
    صحيح copy = _Py_dup(fd);
    إذا (copy < 0) {
        إرجاع عدم;
    }
    FILE *fp = fdopen(copy, "r");
    إذا (fp == عدم) {
        صحيح saved_errno = errno;
        close(copy);
        errno = saved_errno;
    }
    إرجاع fp;
}
#endif

حرف *
_PyTokenizer_FindEncodingFilename(صحيح fd, PyObject *filename)
{
    FILE *fp = fdopen_borrow(fd);
    إذا (fp == عدم) {
        إرجاع عدم;
    }
    بنية tok_state *tok = _PyTokenizer_FromFile(fp, عدم, عدم, عدم);
    إذا (tok == عدم) {
        fclose(fp);
        إرجاع عدم;
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
    إرجاع encoding;
}
