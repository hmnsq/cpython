#شامل "Python.h"
#شامل "pycore_fileutils.h"
#شامل "pycore_pystate.h"
#شامل "pycore_runtime.h"

#شامل "errcode.h"
#شامل "helpers.h"
#شامل "reader.h"
#شامل "reader_internal.h"
#شامل "../lexer/state.h"

#define READER_BUFFER_GROWTH_FACTOR 2

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

ایستا درونخطی صحیح
reader_is_streaming(_PyTok_ReaderKind kind)
{
    بازگشت kind == _PYTOK_READER_FILE || kind == _PYTOK_READER_READLINE;
}

تهی
_PyTok_ReaderFree(ساختار tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    اگر (reader == پوچ) {
        بازگشت;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    برای (صحیح i = 0;
         i < (صحیح)Py_ARRAY_LENGTH(reader->prefetched_سطرها); i++) {
        _PyTok_ChunkClear(&reader->prefetched_سطرها[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_buffer);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = پوچ;
}

صحیح
_PyTokenizer_Traverse(ساختار tok_state *tok, visitproc visit, تهی *arg)
{
    Py_VISIT(tok->filename);
    Py_VISIT(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_VISIT(reader->readline);
    Py_VISIT(reader->decoder);
    برای (صحیح i = 0;
         i < (صحیح)Py_ARRAY_LENGTH(reader->prefetched_سطرها); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_سطرها[i];
        اگر (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_VISIT(chunk->owner);
        }
    }
    بازگشت 0;
}

صحیح
_PyTok_ReserveBuffer(نویسه **buffer, Py_ssize_t *capacity, Py_ssize_t needed,
                     Py_ssize_t initial_capacity)
{
    أكد(initial_capacity > 0);
    اگر (needed <= *capacity) {
        بازگشت 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *capacity > 0 ? *capacity : initial_capacity;
    وقتی (cap < needed) {
        اگر (cap > PY_SSIZE_T_MAX / READER_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            بشکن;
        }
        cap *= READER_BUFFER_GROWTH_FACTOR;
    }
    نویسه *resized = PyMem_Realloc(*buffer, cap);
    اگر (resized == پوچ) {
        PyErr_NoMemory();
        بازگشت -1;
    }
    *buffer = resized;
    *capacity = cap;
    بازگشت 0;
}

ایستا صحیح
append_decoded(_PyTok_Reader *reader, ثابت نویسه *data, Py_ssize_t len)
{
    اگر (len == 0) {
        بازگشت 0;
    }
    اگر (reader->decoded_pos > 0) {
        Py_ssize_t remaining = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (size_t)remaining);
        reader->decoded_scan -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = remaining;
    }
    اگر (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            _PyTok_ReserveBuffer(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        PyErr_NoMemory();
        بازگشت -1;
    }
    memcpy(reader->decoded + reader->decoded_len, data, (size_t)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    بازگشت 0;
}

ایستا صحیح
append_implicit_newline(_PyTok_Reader *reader)
{
    اگر (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        بازگشت 0;
    }
    اگر (append_decoded(reader, "\n", 1) < 0) {
        بازگشت -1;
    }
    reader->decoded_tail_is_implicit = 1;
    بازگشت 0;
}

ایستا صحیح
برداشت_decoded_سطر(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    أكد(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    أكد(reader->decoded_scan >= reader->decoded_pos &&
           reader->decoded_scan <= reader->decoded_len);
    اگر (reader->decoded_pos == reader->decoded_len) {
        بازگشت 0;
    }
    نویسه *start = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    نویسه *newline = memchr(reader->decoded + reader->decoded_scan, '\n',
                           reader->decoded_len - reader->decoded_scan);
    اگر (newline == پوچ) {
        reader->decoded_scan = reader->decoded_len;
        بازگشت 0;
    }
    Py_ssize_t len = newline - start + 1;
    chunk->data = start;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_scan = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_tail_is_implicit;
    اگر (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_scan = 0;
        reader->decoded_tail_is_implicit = 0;
    }
    بازگشت 1;
}

ایستا صحیح
chunk_is_سطر(ثابت _PyTok_Chunk *chunk)
{
    اگر (chunk->len == 0 || chunk->data[chunk->len - 1] != '\n') {
        بازگشت 0;
    }
    بازگشت memchr(chunk->data, '\n', chunk->len - 1) == پوچ;
}

ایستا _PyTok_ReadResult
next_prepared(ساختار tok_state *tok, _PyTok_Chunk *chunk)
{
    اگر (tok->lineno >= tok->source.nlines) {
        بازگشت _PYTOK_READ_EOF;
    }
    _PyTok_Span tail = {tok->inp, tok->source.base_offset + tok->source.len};
    _PyTok_Off newline = _PyTok_SourceFindByte(&tok->source, tail, '\n');
    _PyTok_Span سطر = {tail.start, newline >= 0 ? newline + 1 : tail.end};
    chunk->data = (نویسه *)_PyTok_SourceSpanView(
        &tok->source, سطر, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = سطر.end == tail.end
        ? tok->reader->prepared_final_newline : 0;
    بازگشت PYTOK_READ_سطر;
}

ایستا _PyTok_ReadResult
read_file_سطر(ساختار tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    برای (;;) {
        اگر (len > PY_SSIZE_T_MAX - BUFSIZ) {
            PyErr_NoMemory();
            بازگشت _PYTOK_READ_ERROR;
        }
        اگر (_PyTok_ReserveBuffer(&reader->file_buffer, &reader->file_buffer_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            بازگشت _PYTOK_READ_ERROR;
        }
        صحیح available = (صحیح)Py_MIN(reader->file_buffer_cap - len, INT_MAX);
        size_t اقرأ = 0;
        نویسه *result = _Py_UniversalNewlineFgetsWithSize(
            reader->file_buffer + len, available, reader->fp, پوچ, &اقرأ);
        اگر (result == پوچ) {
            اگر (len == 0) {
                بازگشت _PYTOK_READ_EOF;
            }
            بشکن;
        }
        len += (Py_ssize_t)اقرأ;
        اگر (len > 0 && reader->file_buffer[len - 1] == '\n') {
            بشکن;
        }
    }
    صحیح implicit = reader->file_buffer[len - 1] != '\n';
    chunk->data = reader->file_buffer;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    بازگشت PYTOK_READ_سطر;
}

ایستا صحیح
initialize_file(ساختار tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    اگر (reader->encoding != پوچ) {
        بازگشت _PyTok_StartDecoder(tok, "strict");
    }

    _PyTok_ReadResult result = read_file_سطر(
        tok, &reader->prefetched_سطرها[0]);
    اگر (result == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        بازگشت 0;
    }
    اگر (result != PYTOK_READ_سطر) {
        بازگشت -1;
    }
    Py_ssize_t bom_len;
    _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_سطرها[0], پوچ, 0, &bom_len);
    اگر (detection == _PYTOK_ENCODING_ERROR) {
        بازگشت -1;
    }
    اگر (detection == PYTOK_ENCODING_NEED_SECOND_سطر) {
        نویسه *first = _PyTok_CopyBytes(
            reader->prefetched_سطرها[0].data,
            reader->prefetched_سطرها[0].len);
        اگر (first == پوچ) {
            tok->done = E_NOMEM;
            بازگشت -1;
        }
        reader->prefetched_سطرها[0].data = first;
        reader->prefetched_سطرها[0].ownership = _PYTOK_CHUNK_PYMEM;
        result = read_file_سطر(tok, &reader->prefetched_سطرها[1]);
        اگر (result == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        وگرنه اگر (result != PYTOK_READ_سطر) {
            بازگشت -1;
        }
        _PyTok_Chunk *second = reader->prefetched_سطرها[1].data != پوچ
            ? &reader->prefetched_سطرها[1] : پوچ;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_سطرها[0], second, 1, &bom_len);
        اگر (detection == _PYTOK_ENCODING_ERROR) {
            بازگشت -1;
        }
    }
    اگر (bom_len != 0) {
        _PyTok_Chunk *first = &reader->prefetched_سطرها[0];
        اگر (first->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(first->data, first->data + bom_len,
                    (size_t)(first->len - bom_len));
            first->data[first->len - bom_len] = '\0';
        }
        وگرنه {
            first->data += bom_len;
        }
        first->len -= bom_len;
    }
    اگر (_PyTok_StartDecoder(tok, "strict") < 0) {
        بازگشت -1;
    }
    بازگشت 0;
}

ایستا صحیح
finalize_decoding(ساختار tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    اگر (reader->decoder_finalized) {
        بازگشت 0;
    }
    reader->decoder_finalized = 1;
    اگر (reader->decoder != پوچ) {
        _PyTok_Chunk درون_داد = {
            .data = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        صحیح decoded = _PyTok_DecodeChunk(tok, &درون_داد, 1);
        اگر (decoded == 0 &&
                append_decoded(reader, درون_داد.data, درون_داد.len) < 0) {
            tok->done = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&درون_داد);
        اگر (decoded < 0) {
            بازگشت -1;
        }
    }
    اگر (append_implicit_newline(reader) < 0) {
        tok->done = E_NOMEM;
        بازگشت -1;
    }
    بازگشت 0;
}

ایستا _PyTok_ReadResult
next_file(ساختار tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    اگر (!reader->file_initialized && initialize_file(tok) < 0) {
        بازگشت _PYTOK_READ_ERROR;
    }
    برای (;;) {
        اگر (برداشت_decoded_سطر(reader, chunk)) {
            بازگشت PYTOK_READ_سطر;
        }
        _PyTok_Chunk درون_داد = {0};
        اگر (reader->prefetched_سطرها[0].data != پوچ) {
            درون_داد = reader->prefetched_سطرها[0];
            reader->prefetched_سطرها[0] = (_PyTok_Chunk){0};
        }
        وگرنه اگر (reader->prefetched_سطرها[1].data != پوچ) {
            درون_داد = reader->prefetched_سطرها[1];
            reader->prefetched_سطرها[1] = (_PyTok_Chunk){0};
        }
        وگرنه اگر (!reader->file_eof) {
            _PyTok_ReadResult result = read_file_سطر(tok, &درون_داد);
            اگر (result == _PYTOK_READ_ERROR) {
                بازگشت result;
            }
            اگر (result == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        اگر (درون_داد.data != پوچ) {
            صحیح implicit = درون_داد.implicit_newline;
            اگر (reader->decoder == پوچ && !implicit) {
                *chunk = درون_داد;
                بازگشت PYTOK_READ_سطر;
            }
            صحیح decoded = _PyTok_DecodeChunk(tok, &درون_داد, 0);
            اگر (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_سطر(&درون_داد)) {
                *chunk = درون_داد;
                بازگشت PYTOK_READ_سطر;
            }
            اگر (decoded == 0 &&
                    append_decoded(reader, درون_داد.data, درون_داد.len) < 0) {
                tok->done = E_NOMEM;
                decoded = -1;
            }
            اگر (decoded == 0 && implicit) {
                reader->decoded_tail_is_implicit = 1;
            }
            _PyTok_ChunkClear(&درون_داد);
            اگر (decoded < 0) {
                بازگشت _PYTOK_READ_ERROR;
            }
            ادامه;
        }
        اگر (!reader->decoder_finalized) {
            اگر (finalize_decoding(tok) < 0) {
                بازگشت _PYTOK_READ_ERROR;
            }
            ادامه;
        }
        بازگشت _PYTOK_READ_EOF;
    }
}

ایستا _PyTok_ReadResult
next_readline(ساختار tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    برای (;;) {
        اگر (برداشت_decoded_سطر(reader, chunk)) {
            بازگشت PYTOK_READ_سطر;
        }
        اگر (reader->decoder_finalized) {
            بازگشت _PYTOK_READ_EOF;
        }

        PyObject *raw = PyObject_CallNoArgs(reader->readline);
        اگر (raw == پوچ) {
            اگر (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                اگر (finalize_decoding(tok) < 0) {
                    بازگشت _PYTOK_READ_ERROR;
                }
                ادامه;
            }
            بازگشت _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk درون_داد = {0};
        اگر (reader->encoding != پوچ) {
            اگر (!PyBytes_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                بازگشت _PYTOK_READ_ERROR;
            }
            اگر (PyBytes_GET_SIZE(raw) == 0) {
                Py_DECREF(raw);
                اگر (_PyTok_StartDecoder(tok, "replace") < 0) {
                    بازگشت _PYTOK_READ_ERROR;
                }
                اگر (finalize_decoding(tok) < 0) {
                    بازگشت _PYTOK_READ_ERROR;
                }
                ادامه;
            }
            درون_داد.owner = raw;
            درون_داد.data = PyBytes_AS_STRING(raw);
            درون_داد.len = PyBytes_GET_SIZE(raw);
            درون_داد.ownership = _PYTOK_CHUNK_PYOBJECT;
            صحیح decoded;
            اگر (reader->decoder == پوچ &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_سطر(&درون_داد)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &درون_داد, "utf-8", "replace");
            }
            وگرنه {
                decoded = _PyTok_StartDecoder(tok, "replace");
                اگر (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &درون_داد, 0);
                }
            }
            اگر (decoded < 0) {
                _PyTok_ChunkClear(&درون_داد);
                بازگشت _PYTOK_READ_ERROR;
            }
        }
        وگرنه {
            اگر (!PyUnicode_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                بازگشت _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            ثابت نویسه *utf8 = PyUnicode_AsUTF8AndSize(raw, &utf8_len);
            اگر (utf8 == پوچ) {
                Py_DECREF(raw);
                بازگشت _PYTOK_READ_ERROR;
            }
            درون_داد.owner = raw;
            درون_داد.data = (نویسه *)utf8;
            درون_داد.len = utf8_len;
            درون_داد.ownership = _PYTOK_CHUNK_PYOBJECT;
            اگر (درون_داد.len == 0) {
                _PyTok_ChunkClear(&درون_داد);
                اگر (finalize_decoding(tok) < 0) {
                    بازگشت _PYTOK_READ_ERROR;
                }
                ادامه;
            }
        }

        اگر (reader->decoded_pos == reader->decoded_len &&
                chunk_is_سطر(&درون_داد)) {
            *chunk = درون_داد;
            بازگشت PYTOK_READ_سطر;
        }

        اگر (append_decoded(reader, درون_داد.data, درون_داد.len) < 0) {
            _PyTok_ChunkClear(&درون_داد);
            tok->done = E_NOMEM;
            بازگشت _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&درون_داد);
        اگر (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            صحیح pending = Py_Tok_Decoder_Has_Buffered_درون_داد(tok);
            اگر (pending < 0) {
                بازگشت _PYTOK_READ_ERROR;
            }
            اگر (!pending && append_implicit_newline(reader) < 0) {
                tok->done = E_NOMEM;
                بازگشت _PYTOK_READ_ERROR;
            }
        }
    }
}

ایستا _PyTok_ReadResult
next_interactive(ساختار tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    اگر (reader->stop_interactive) {
        بازگشت _PYTOK_READ_STOPPED;
    }
    نویسه *درون_داد = PyOS_Readline(
        reader->fp != پوچ ? reader->fp : stdin, stdout, reader->prompt);
    اگر (reader->nextprompt != پوچ) {
        reader->prompt = reader->nextprompt;
    }
    اگر (درون_داد == پوچ) {
        بازگشت _PYTOK_READ_INTERRUPT;
    }
    Py_ssize_t len = strlen(درون_داد);
    اگر (len == 0) {
        PyMem_Free(درون_داد);
        بازگشت _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .data = درون_داد,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    اگر (reader->encoding != پوچ &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, پوچ) < 0) {
        _PyTok_ChunkClear(&decoded);
        بازگشت _PYTOK_READ_ERROR;
    }
    اگر (memchr(decoded.data, '\r', decoded.len) == پوچ) {
        *chunk = decoded;
        بازگشت PYTOK_READ_سطر;
    }
    chunk->data = _PyTok_NormalizeNewlines(
        decoded.data, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    اگر (chunk->data == پوچ) {
        tok->done = E_NOMEM;
        بازگشت _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    بازگشت PYTOK_READ_سطر;
}

صحیح
_PyTok_ReaderIsInteractive(ثابت ساختار tok_state *tok)
{
    بازگشت tok->reader->kind == _PYTOK_READER_INTERACTIVE;
}

ثابت نویسه *
_PyTokenizer_RetainedSource(ثابت ساختار tok_state *tok)
{
    اگر (tok->reader->kind == _PYTOK_READER_PREPARED) {
        بازگشت _PyTok_SourceData(&tok->source);
    }
    اگر (tok->reader->kind == _PYTOK_READER_INTERACTIVE) {
        بازگشت tok->source.bytes;
    }
    بازگشت پوچ;
}

تهی
_PyTok_ReaderStopInteractive(ساختار tok_state *tok)
{
    اگر (_PyTok_ReaderIsInteractive(tok)) {
        tok->reader->stop_interactive = 1;
    }
}

ایستا _PyTok_ReadResult
reader_next(ساختار tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    انتخاب (tok->reader->kind) {
        حالت _PYTOK_READER_PREPARED:
            بازگشت next_prepared(tok, chunk);
        حالت _PYTOK_READER_FILE:
            بازگشت next_file(tok, chunk);
        حالت _PYTOK_READER_READLINE:
            بازگشت next_readline(tok, chunk);
        حالت _PYTOK_READER_INTERACTIVE:
            بازگشت next_interactive(tok, chunk);
    }
    Py_UNREACHABLE();
}

ایستا تهی
reset_streaming_buffer(ساختار tok_state *tok)
{
    أكد(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_SourceDiscard(&tok->source);
    tok->buf_offset = tok->source.base_offset;
    tok->cur = tok->inp = tok->source.base_offset;
    tok->سطر_start = tok->buf_offset;
}

صحیح
_PyTok_ReaderUnderflow(ساختار tok_state *tok)
{
    أكد(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    _PyTok_ReaderKind kind = reader->kind;
    صحیح prepared = kind == _PYTOK_READER_PREPARED;
    صحیح streaming = reader_is_streaming(kind);
    صحیح reset_buffer = tok->start < 0 &&
        _PyLexer_CurrentFTString(tok) == پوچ;

    _PyTok_Chunk chunk;
    _PyTok_ReadResult result = reader_next(tok, &chunk);
    اگر (result != PYTOK_READ_سطر) {
        اگر (reset_buffer && streaming) {
            reset_streaming_buffer(tok);
        }
        اگر (result == _PYTOK_READ_EOF) {
            tok->done = E_EOF;
        }
        وگرنه اگر (result == _PYTOK_READ_STOPPED) {
            tok->done = E_INTERACT_STOP;
        }
        وگرنه اگر (result == _PYTOK_READ_INTERRUPT) {
            tok->done = E_INTR;
        }
        وگرنه {
            اگر (tok->done == E_OK) {
                tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                    ? E_NOMEM : E_ERROR;
            }
        }
        اگر (kind == _PYTOK_READER_INTERACTIVE &&
                result != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        بازگشت 0;
    }
    اگر (tok->lineno == INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        tok->done = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        بازگشت 0;
    }

    اگر (!prepared) {
        اگر (streaming && reset_buffer) {
            reset_streaming_buffer(tok);
        }
        _PyTok_Off source_start = Py_Tok_Source_Append_سطر(
            &tok->source, chunk.data, chunk.len);
        اگر (source_start < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            بازگشت 0;
        }
        اگر (reset_buffer) {
            tok->cur = source_start;
            tok->buf_offset = source_start;
            tok->سطر_start = tok->buf_offset;
        }
        tok->inp = source_start + chunk.len;
    }
    وگرنه {
        اگر (reset_buffer) {
            tok->buf_offset = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    اگر (kind == _PYTOK_READER_FILE &&
            (reader->encoding == پوچ ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                _PyTok_SourcePointer(&tok->source, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        بازگشت 0;
    }
    _PyTok_ChunkClear(&chunk);
    بازگشت 1;
}

ایستا ساختار tok_state *
tokenizer_new_with_reader(_PyTok_ReaderKind kind)
{
    ساختار tok_state *tok = PyMem_Calloc(1, اندازه(*tok));
    اگر (tok == پوچ) {
        PyErr_NoMemory();
        بازگشت پوچ;
    }
    tok->start = tok->سطر_start = -1;
    _PyTok_SourceInit(&tok->source);
    tok->done = E_OK;
    tok->layout.at_bol = 1;
    tok->start_loc = (_PyTok_Loc){-1, -1};
    tok->ftstring_پشته = tok->ftstring_پشته_inline;
    tok->ftstring_capacity = FTSTRING_پشته_INLINE_CAPACITY;
#ifdef Py_DEBUG
    tok->debug = _Py_GetConfig()->parser_debug;
#endif
    tok->reader = PyMem_Calloc(1, اندازه(*tok->reader));
    اگر (tok->reader == پوچ) {
        PyErr_NoMemory();
        _PyTokenizer_Free(tok);
        بازگشت پوچ;
    }
    tok->reader->kind = kind;
    اگر (kind == _PYTOK_READER_PREPARED) {
        بازگشت tok;
    }
    اگر (reader_is_streaming(kind)) {
        tok->cur = tok->inp = tok->source.base_offset;
        tok->سطر_start = tok->buf_offset;
    }
    بازگشت tok;
}

ایستا ساختار tok_state *
tokenizer_from_string(ثابت نویسه *درون_داد, صحیح utf8_only, صحیح exec_درون_داد,
                      صحیح preserve_crlf)
{
    ساختار tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    اگر (tok == پوچ) {
        بازگشت پوچ;
    }
    اگر (_PyTok_PrepareString(
            tok, درون_داد, utf8_only, exec_درون_داد, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        بازگشت پوچ;
    }
    tok->cur = tok->inp = tok->source.base_offset;
    tok->سطر_start = tok->source.base_offset;
    بازگشت tok;
}

ساختار tok_state *
_PyTokenizer_FromString(ثابت نویسه *درون_داد, صحیح exec_درون_داد, صحیح preserve_crlf)
{
    بازگشت tokenizer_from_string(درون_داد, 0, exec_درون_داد, preserve_crlf);
}

ساختار tok_state *
_PyTokenizer_FromUTF8(ثابت نویسه *درون_داد, صحیح exec_درون_داد, صحیح preserve_crlf)
{
    بازگشت tokenizer_from_string(درون_داد, 1, exec_درون_داد, preserve_crlf);
}

ساختار tok_state *
_PyTokenizer_FromReadline(PyObject *readline, ثابت نویسه *encoding)
{
    ساختار tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    اگر (tok == پوچ) {
        بازگشت پوچ;
    }
    اگر (encoding != پوچ && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        بازگشت پوچ;
    }
    tok->reader->readline = Py_NewRef(readline);
    بازگشت tok;
}

ساختار tok_state *
_PyTokenizer_FromFile(FILE *fp, ثابت نویسه *encoding,
                      ثابت نویسه *ps1, ثابت نویسه *ps2)
{
    _PyTok_ReaderKind kind = ps1 != پوچ || ps2 != پوچ
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    ساختار tok_state *tok = tokenizer_new_with_reader(kind);
    اگر (tok == پوچ) {
        بازگشت پوچ;
    }
    اگر (encoding != پوچ && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        بازگشت پوچ;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    بازگشت tok;
}

#اگر defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
تعریفنوع اجتماع {
    تهی *cookie;
    صحیح fd;
} borrowed_fd;

ایستا ssize_t
borrow_read(تهی *cookie, نویسه *buffer, size_t size)
{
    borrowed_fd borrowed = {.cookie = cookie};
    بازگشت اقرأ(borrowed.fd, buffer, size);
}

ایستا FILE *
fdopen_borrow(صحیح fd)
{
    cookie_io_functions_t callbacks = {borrow_read, پوچ, پوچ, پوچ};
    borrowed_fd borrowed = {.fd = fd};
    بازگشت fopencookie(borrowed.cookie, "r", callbacks);
}
#وگرنه
ایستا FILE *
fdopen_borrow(صحیح fd)
{
    صحیح copy = _Py_dup(fd);
    اگر (copy < 0) {
        بازگشت پوچ;
    }
    FILE *fp = fdopen(copy, "r");
    اگر (fp == پوچ) {
        صحیح saved_errno = errno;
        close(copy);
        errno = saved_errno;
    }
    بازگشت fp;
}
#endif

نویسه *
_PyTokenizer_FindEncodingFilename(صحیح fd, PyObject *filename)
{
    FILE *fp = fdopen_borrow(fd);
    اگر (fp == پوچ) {
        بازگشت پوچ;
    }
    ساختار tok_state *tok = _PyTokenizer_FromFile(fp, پوچ, پوچ, پوچ);
    اگر (tok == پوچ) {
        fclose(fp);
        بازگشت پوچ;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != پوچ ? filename : &_Py_STR(anon_string));
    نویسه *encoding = پوچ;
    اگر (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = پوچ;
    }
    fclose(fp);
    _PyTokenizer_Free(tok);
    بازگشت encoding;
}
