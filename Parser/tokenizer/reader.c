#અન્તર્ભાવ "Python.h"
#અન્તર્ભાવ "pycore_fileutils.h"
#અન્તર્ભાવ "pycore_pystate.h"
#અન્તર્ભાવ "pycore_runtime.h"

#અન્તર્ભાવ "errcode.h"
#અન્તર્ભાવ "helpers.h"
#અન્તર્ભાવ "reader.h"
#અન્તર્ભાવ "reader_internal.h"
#અન્તર્ભાવ "../lexer/state.h"

#define READER_BUFFER_GROWTH_FACTOR 2

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

સ્થિર અંતઃરેખ પૂર્ણાંક
reader_is_streaming(_PyTok_ReaderKind kind)
{
    પાછા kind == _PYTOK_READER_FILE || kind == _PYTOK_READER_READLINE;
}

શૂન્ય
_PyTok_ReaderFree(સંરચના tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    જો (reader == ખાલી) {
        પાછા;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    માટે (પૂર્ણાંક i = 0;
         i < (પૂર્ણાંક)Py_ARRAY_LENGTH(reader->prefetched_લીટીઓ); i++) {
        _PyTok_ChunkClear(&reader->prefetched_લીટીઓ[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_buffer);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = ખાલી;
}

પૂર્ણાંક
_PyTokenizer_Traverse(સંરચના tok_state *tok, visitproc visit, શૂન્ય *arg)
{
    Py_VISIT(tok->filename);
    Py_VISIT(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_VISIT(reader->readline);
    Py_VISIT(reader->decoder);
    માટે (પૂર્ણાંક i = 0;
         i < (પૂર્ણાંક)Py_ARRAY_LENGTH(reader->prefetched_લીટીઓ); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_લીટીઓ[i];
        જો (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_VISIT(chunk->owner);
        }
    }
    પાછા 0;
}

પૂર્ણાંક
_PyTok_ReserveBuffer(અક્ષર **buffer, Py_ssize_t *capacity, Py_ssize_t needed,
                     Py_ssize_t initial_capacity)
{
    પ્રતિજ્ઞા(initial_capacity > 0);
    જો (needed <= *capacity) {
        પાછા 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *capacity > 0 ? *capacity : initial_capacity;
    જ્યારે (cap < needed) {
        જો (cap > PY_SSIZE_T_MAX / READER_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            તોડો;
        }
        cap *= READER_BUFFER_GROWTH_FACTOR;
    }
    અક્ષર *resized = PyMem_Realloc(*buffer, cap);
    જો (resized == ખાલી) {
        PyErr_NoMemory();
        પાછા -1;
    }
    *buffer = resized;
    *capacity = cap;
    પાછા 0;
}

સ્થિર પૂર્ણાંક
append_decoded(_PyTok_Reader *reader, અચલ અક્ષર *data, Py_ssize_t len)
{
    જો (len == 0) {
        પાછા 0;
    }
    જો (reader->decoded_pos > 0) {
        Py_ssize_t remaining = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (size_t)remaining);
        reader->decoded_scan -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = remaining;
    }
    જો (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            _PyTok_ReserveBuffer(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        PyErr_NoMemory();
        પાછા -1;
    }
    memcpy(reader->decoded + reader->decoded_len, data, (size_t)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    પાછા 0;
}

સ્થિર પૂર્ણાંક
append_implicit_newline(_PyTok_Reader *reader)
{
    જો (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        પાછા 0;
    }
    જો (append_decoded(reader, "\n", 1) < 0) {
        પાછા -1;
    }
    reader->decoded_tail_is_implicit = 1;
    પાછા 0;
}

સ્થિર પૂર્ણાંક
કાઢો_decoded_લીટી(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    પ્રતિજ્ઞા(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    પ્રતિજ્ઞા(reader->decoded_scan >= reader->decoded_pos &&
           reader->decoded_scan <= reader->decoded_len);
    જો (reader->decoded_pos == reader->decoded_len) {
        પાછા 0;
    }
    અક્ષર *start = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    અક્ષર *newline = memchr(reader->decoded + reader->decoded_scan, '\n',
                           reader->decoded_len - reader->decoded_scan);
    જો (newline == ખાલી) {
        reader->decoded_scan = reader->decoded_len;
        પાછા 0;
    }
    Py_ssize_t len = newline - start + 1;
    chunk->data = start;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_scan = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_tail_is_implicit;
    જો (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_scan = 0;
        reader->decoded_tail_is_implicit = 0;
    }
    પાછા 1;
}

સ્થિર પૂર્ણાંક
chunk_is_લીટી(અચલ _PyTok_Chunk *chunk)
{
    જો (chunk->len == 0 || chunk->data[chunk->len - 1] != '\n') {
        પાછા 0;
    }
    પાછા memchr(chunk->data, '\n', chunk->len - 1) == ખાલી;
}

સ્થિર _PyTok_ReadResult
next_prepared(સંરચના tok_state *tok, _PyTok_Chunk *chunk)
{
    જો (tok->lineno >= tok->source.nlines) {
        પાછા _PYTOK_READ_EOF;
    }
    _PyTok_Span tail = {tok->inp, tok->source.base_offset + tok->source.len};
    _PyTok_Off newline = _PyTok_SourceFindByte(&tok->source, tail, '\n');
    _PyTok_Span લીટી = {tail.start, newline >= 0 ? newline + 1 : tail.end};
    chunk->data = (અક્ષર *)_PyTok_SourceSpanView(
        &tok->source, લીટી, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = લીટી.end == tail.end
        ? tok->reader->prepared_final_newline : 0;
    પાછા PYTOK_READ_લીટી;
}

સ્થિર _PyTok_ReadResult
read_file_લીટી(સંરચના tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    માટે (;;) {
        જો (len > PY_SSIZE_T_MAX - BUFSIZ) {
            PyErr_NoMemory();
            પાછા _PYTOK_READ_ERROR;
        }
        જો (_PyTok_ReserveBuffer(&reader->file_buffer, &reader->file_buffer_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            પાછા _PYTOK_READ_ERROR;
        }
        પૂર્ણાંક available = (પૂર્ણાંક)Py_MIN(reader->file_buffer_cap - len, INT_MAX);
        size_t પઠ = 0;
        અક્ષર *result = _Py_UniversalNewlineFgetsWithSize(
            reader->file_buffer + len, available, reader->fp, ખાલી, &પઠ);
        જો (result == ખાલી) {
            જો (len == 0) {
                પાછા _PYTOK_READ_EOF;
            }
            તોડો;
        }
        len += (Py_ssize_t)પઠ;
        જો (len > 0 && reader->file_buffer[len - 1] == '\n') {
            તોડો;
        }
    }
    પૂર્ણાંક implicit = reader->file_buffer[len - 1] != '\n';
    chunk->data = reader->file_buffer;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    પાછા PYTOK_READ_લીટી;
}

સ્થિર પૂર્ણાંક
initialize_file(સંરચના tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    જો (reader->encoding != ખાલી) {
        પાછા _PyTok_StartDecoder(tok, "strict");
    }

    _PyTok_ReadResult result = read_file_લીટી(
        tok, &reader->prefetched_લીટીઓ[0]);
    જો (result == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        પાછા 0;
    }
    જો (result != PYTOK_READ_લીટી) {
        પાછા -1;
    }
    Py_ssize_t bom_len;
    _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_લીટીઓ[0], ખાલી, 0, &bom_len);
    જો (detection == _PYTOK_ENCODING_ERROR) {
        પાછા -1;
    }
    જો (detection == PYTOK_ENCODING_NEED_SECOND_લીટી) {
        અક્ષર *first = _PyTok_CopyBytes(
            reader->prefetched_લીટીઓ[0].data,
            reader->prefetched_લીટીઓ[0].len);
        જો (first == ખાલી) {
            tok->done = E_NOMEM;
            પાછા -1;
        }
        reader->prefetched_લીટીઓ[0].data = first;
        reader->prefetched_લીટીઓ[0].ownership = _PYTOK_CHUNK_PYMEM;
        result = read_file_લીટી(tok, &reader->prefetched_લીટીઓ[1]);
        જો (result == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        નહીંતર જો (result != PYTOK_READ_લીટી) {
            પાછા -1;
        }
        _PyTok_Chunk *second = reader->prefetched_લીટીઓ[1].data != ખાલી
            ? &reader->prefetched_લીટીઓ[1] : ખાલી;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_લીટીઓ[0], second, 1, &bom_len);
        જો (detection == _PYTOK_ENCODING_ERROR) {
            પાછા -1;
        }
    }
    જો (bom_len != 0) {
        _PyTok_Chunk *first = &reader->prefetched_લીટીઓ[0];
        જો (first->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(first->data, first->data + bom_len,
                    (size_t)(first->len - bom_len));
            first->data[first->len - bom_len] = '\0';
        }
        નહીંતર {
            first->data += bom_len;
        }
        first->len -= bom_len;
    }
    જો (_PyTok_StartDecoder(tok, "strict") < 0) {
        પાછા -1;
    }
    પાછા 0;
}

સ્થિર પૂર્ણાંક
finalize_decoding(સંરચના tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    જો (reader->decoder_finalized) {
        પાછા 0;
    }
    reader->decoder_finalized = 1;
    જો (reader->decoder != ખાલી) {
        _PyTok_Chunk આગત = {
            .data = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        પૂર્ણાંક decoded = _PyTok_DecodeChunk(tok, &આગત, 1);
        જો (decoded == 0 &&
                append_decoded(reader, આગત.data, આગત.len) < 0) {
            tok->done = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&આગત);
        જો (decoded < 0) {
            પાછા -1;
        }
    }
    જો (append_implicit_newline(reader) < 0) {
        tok->done = E_NOMEM;
        પાછા -1;
    }
    પાછા 0;
}

સ્થિર _PyTok_ReadResult
next_file(સંરચના tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    જો (!reader->file_initialized && initialize_file(tok) < 0) {
        પાછા _PYTOK_READ_ERROR;
    }
    માટે (;;) {
        જો (કાઢો_decoded_લીટી(reader, chunk)) {
            પાછા PYTOK_READ_લીટી;
        }
        _PyTok_Chunk આગત = {0};
        જો (reader->prefetched_લીટીઓ[0].data != ખાલી) {
            આગત = reader->prefetched_લીટીઓ[0];
            reader->prefetched_લીટીઓ[0] = (_PyTok_Chunk){0};
        }
        નહીંતર જો (reader->prefetched_લીટીઓ[1].data != ખાલી) {
            આગત = reader->prefetched_લીટીઓ[1];
            reader->prefetched_લીટીઓ[1] = (_PyTok_Chunk){0};
        }
        નહીંતર જો (!reader->file_eof) {
            _PyTok_ReadResult result = read_file_લીટી(tok, &આગત);
            જો (result == _PYTOK_READ_ERROR) {
                પાછા result;
            }
            જો (result == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        જો (આગત.data != ખાલી) {
            પૂર્ણાંક implicit = આગત.implicit_newline;
            જો (reader->decoder == ખાલી && !implicit) {
                *chunk = આગત;
                પાછા PYTOK_READ_લીટી;
            }
            પૂર્ણાંક decoded = _PyTok_DecodeChunk(tok, &આગત, 0);
            જો (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_લીટી(&આગત)) {
                *chunk = આગત;
                પાછા PYTOK_READ_લીટી;
            }
            જો (decoded == 0 &&
                    append_decoded(reader, આગત.data, આગત.len) < 0) {
                tok->done = E_NOMEM;
                decoded = -1;
            }
            જો (decoded == 0 && implicit) {
                reader->decoded_tail_is_implicit = 1;
            }
            _PyTok_ChunkClear(&આગત);
            જો (decoded < 0) {
                પાછા _PYTOK_READ_ERROR;
            }
            ચાલુ;
        }
        જો (!reader->decoder_finalized) {
            જો (finalize_decoding(tok) < 0) {
                પાછા _PYTOK_READ_ERROR;
            }
            ચાલુ;
        }
        પાછા _PYTOK_READ_EOF;
    }
}

સ્થિર _PyTok_ReadResult
next_readline(સંરચના tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    માટે (;;) {
        જો (કાઢો_decoded_લીટી(reader, chunk)) {
            પાછા PYTOK_READ_લીટી;
        }
        જો (reader->decoder_finalized) {
            પાછા _PYTOK_READ_EOF;
        }

        PyObject *raw = PyObject_CallNoArgs(reader->readline);
        જો (raw == ખાલી) {
            જો (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                જો (finalize_decoding(tok) < 0) {
                    પાછા _PYTOK_READ_ERROR;
                }
                ચાલુ;
            }
            પાછા _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk આગત = {0};
        જો (reader->encoding != ખાલી) {
            જો (!PyBytes_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                પાછા _PYTOK_READ_ERROR;
            }
            જો (PyBytes_GET_SIZE(raw) == 0) {
                Py_DECREF(raw);
                જો (_PyTok_StartDecoder(tok, "replace") < 0) {
                    પાછા _PYTOK_READ_ERROR;
                }
                જો (finalize_decoding(tok) < 0) {
                    પાછા _PYTOK_READ_ERROR;
                }
                ચાલુ;
            }
            આગત.owner = raw;
            આગત.data = PyBytes_AS_STRING(raw);
            આગત.len = PyBytes_GET_SIZE(raw);
            આગત.ownership = _PYTOK_CHUNK_PYOBJECT;
            પૂર્ણાંક decoded;
            જો (reader->decoder == ખાલી &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_લીટી(&આગત)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &આગત, "utf-8", "replace");
            }
            નહીંતર {
                decoded = _PyTok_StartDecoder(tok, "replace");
                જો (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &આગત, 0);
                }
            }
            જો (decoded < 0) {
                _PyTok_ChunkClear(&આગત);
                પાછા _PYTOK_READ_ERROR;
            }
        }
        નહીંતર {
            જો (!PyUnicode_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                પાછા _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            અચલ અક્ષર *utf8 = PyUnicode_AsUTF8AndSize(raw, &utf8_len);
            જો (utf8 == ખાલી) {
                Py_DECREF(raw);
                પાછા _PYTOK_READ_ERROR;
            }
            આગત.owner = raw;
            આગત.data = (અક્ષર *)utf8;
            આગત.len = utf8_len;
            આગત.ownership = _PYTOK_CHUNK_PYOBJECT;
            જો (આગત.len == 0) {
                _PyTok_ChunkClear(&આગત);
                જો (finalize_decoding(tok) < 0) {
                    પાછા _PYTOK_READ_ERROR;
                }
                ચાલુ;
            }
        }

        જો (reader->decoded_pos == reader->decoded_len &&
                chunk_is_લીટી(&આગત)) {
            *chunk = આગત;
            પાછા PYTOK_READ_લીટી;
        }

        જો (append_decoded(reader, આગત.data, આગત.len) < 0) {
            _PyTok_ChunkClear(&આગત);
            tok->done = E_NOMEM;
            પાછા _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&આગત);
        જો (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            પૂર્ણાંક pending = Py_Tok_Decoder_Has_Buffered_આગત(tok);
            જો (pending < 0) {
                પાછા _PYTOK_READ_ERROR;
            }
            જો (!pending && append_implicit_newline(reader) < 0) {
                tok->done = E_NOMEM;
                પાછા _PYTOK_READ_ERROR;
            }
        }
    }
}

સ્થિર _PyTok_ReadResult
next_interactive(સંરચના tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    જો (reader->stop_interactive) {
        પાછા _PYTOK_READ_STOPPED;
    }
    અક્ષર *આગત = PyOS_Readline(
        reader->fp != ખાલી ? reader->fp : stdin, stdout, reader->prompt);
    જો (reader->nextprompt != ખાલી) {
        reader->prompt = reader->nextprompt;
    }
    જો (આગત == ખાલી) {
        પાછા _PYTOK_READ_INTERRUPT;
    }
    Py_ssize_t len = strlen(આગત);
    જો (len == 0) {
        PyMem_Free(આગત);
        પાછા _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .data = આગત,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    જો (reader->encoding != ખાલી &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, ખાલી) < 0) {
        _PyTok_ChunkClear(&decoded);
        પાછા _PYTOK_READ_ERROR;
    }
    જો (memchr(decoded.data, '\r', decoded.len) == ખાલી) {
        *chunk = decoded;
        પાછા PYTOK_READ_લીટી;
    }
    chunk->data = _PyTok_NormalizeNewlines(
        decoded.data, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    જો (chunk->data == ખાલી) {
        tok->done = E_NOMEM;
        પાછા _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    પાછા PYTOK_READ_લીટી;
}

પૂર્ણાંક
_PyTok_ReaderIsInteractive(અચલ સંરચના tok_state *tok)
{
    પાછા tok->reader->kind == _PYTOK_READER_INTERACTIVE;
}

અચલ અક્ષર *
_PyTokenizer_RetainedSource(અચલ સંરચના tok_state *tok)
{
    જો (tok->reader->kind == _PYTOK_READER_PREPARED) {
        પાછા _PyTok_SourceData(&tok->source);
    }
    જો (tok->reader->kind == _PYTOK_READER_INTERACTIVE) {
        પાછા tok->source.bytes;
    }
    પાછા ખાલી;
}

શૂન્ય
_PyTok_ReaderStopInteractive(સંરચના tok_state *tok)
{
    જો (_PyTok_ReaderIsInteractive(tok)) {
        tok->reader->stop_interactive = 1;
    }
}

સ્થિર _PyTok_ReadResult
reader_next(સંરચના tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    પસંદગી (tok->reader->kind) {
        સ્થિતિ _PYTOK_READER_PREPARED:
            પાછા next_prepared(tok, chunk);
        સ્થિતિ _PYTOK_READER_FILE:
            પાછા next_file(tok, chunk);
        સ્થિતિ _PYTOK_READER_READLINE:
            પાછા next_readline(tok, chunk);
        સ્થિતિ _PYTOK_READER_INTERACTIVE:
            પાછા next_interactive(tok, chunk);
    }
    Py_UNREACHABLE();
}

સ્થિર શૂન્ય
reset_streaming_buffer(સંરચના tok_state *tok)
{
    પ્રતિજ્ઞા(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_SourceDiscard(&tok->source);
    tok->buf_offset = tok->source.base_offset;
    tok->cur = tok->inp = tok->source.base_offset;
    tok->લીટી_start = tok->buf_offset;
}

પૂર્ણાંક
_PyTok_ReaderUnderflow(સંરચના tok_state *tok)
{
    પ્રતિજ્ઞા(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    _PyTok_ReaderKind kind = reader->kind;
    પૂર્ણાંક prepared = kind == _PYTOK_READER_PREPARED;
    પૂર્ણાંક streaming = reader_is_streaming(kind);
    પૂર્ણાંક reset_buffer = tok->start < 0 &&
        _PyLexer_CurrentFTString(tok) == ખાલી;

    _PyTok_Chunk chunk;
    _PyTok_ReadResult result = reader_next(tok, &chunk);
    જો (result != PYTOK_READ_લીટી) {
        જો (reset_buffer && streaming) {
            reset_streaming_buffer(tok);
        }
        જો (result == _PYTOK_READ_EOF) {
            tok->done = E_EOF;
        }
        નહીંતર જો (result == _PYTOK_READ_STOPPED) {
            tok->done = E_INTERACT_STOP;
        }
        નહીંતર જો (result == _PYTOK_READ_INTERRUPT) {
            tok->done = E_INTR;
        }
        નહીંતર {
            જો (tok->done == E_OK) {
                tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                    ? E_NOMEM : E_ERROR;
            }
        }
        જો (kind == _PYTOK_READER_INTERACTIVE &&
                result != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        પાછા 0;
    }
    જો (tok->lineno == INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        tok->done = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        પાછા 0;
    }

    જો (!prepared) {
        જો (streaming && reset_buffer) {
            reset_streaming_buffer(tok);
        }
        _PyTok_Off source_start = Py_Tok_Source_Append_લીટી(
            &tok->source, chunk.data, chunk.len);
        જો (source_start < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            પાછા 0;
        }
        જો (reset_buffer) {
            tok->cur = source_start;
            tok->buf_offset = source_start;
            tok->લીટી_start = tok->buf_offset;
        }
        tok->inp = source_start + chunk.len;
    }
    નહીંતર {
        જો (reset_buffer) {
            tok->buf_offset = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    જો (kind == _PYTOK_READER_FILE &&
            (reader->encoding == ખાલી ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                _PyTok_SourcePointer(&tok->source, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        પાછા 0;
    }
    _PyTok_ChunkClear(&chunk);
    પાછા 1;
}

સ્થિર સંરચના tok_state *
tokenizer_new_with_reader(_PyTok_ReaderKind kind)
{
    સંરચના tok_state *tok = PyMem_Calloc(1, કદ(*tok));
    જો (tok == ખાલી) {
        PyErr_NoMemory();
        પાછા ખાલી;
    }
    tok->start = tok->લીટી_start = -1;
    _PyTok_SourceInit(&tok->source);
    tok->done = E_OK;
    tok->layout.at_bol = 1;
    tok->start_loc = (_PyTok_Loc){-1, -1};
    tok->ftstring_ઢગલો = tok->ftstring_ઢગલો_inline;
    tok->ftstring_capacity = FTSTRING_ઢગલો_INLINE_CAPACITY;
#ifdef Py_DEBUG
    tok->debug = _Py_GetConfig()->parser_debug;
#endif
    tok->reader = PyMem_Calloc(1, કદ(*tok->reader));
    જો (tok->reader == ખાલી) {
        PyErr_NoMemory();
        _PyTokenizer_Free(tok);
        પાછા ખાલી;
    }
    tok->reader->kind = kind;
    જો (kind == _PYTOK_READER_PREPARED) {
        પાછા tok;
    }
    જો (reader_is_streaming(kind)) {
        tok->cur = tok->inp = tok->source.base_offset;
        tok->લીટી_start = tok->buf_offset;
    }
    પાછા tok;
}

સ્થિર સંરચના tok_state *
tokenizer_from_string(અચલ અક્ષર *આગત, પૂર્ણાંક utf8_only, પૂર્ણાંક exec_આગત,
                      પૂર્ણાંક preserve_crlf)
{
    સંરચના tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    જો (tok == ખાલી) {
        પાછા ખાલી;
    }
    જો (_PyTok_PrepareString(
            tok, આગત, utf8_only, exec_આગત, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        પાછા ખાલી;
    }
    tok->cur = tok->inp = tok->source.base_offset;
    tok->લીટી_start = tok->source.base_offset;
    પાછા tok;
}

સંરચના tok_state *
_PyTokenizer_FromString(અચલ અક્ષર *આગત, પૂર્ણાંક exec_આગત, પૂર્ણાંક preserve_crlf)
{
    પાછા tokenizer_from_string(આગત, 0, exec_આગત, preserve_crlf);
}

સંરચના tok_state *
_PyTokenizer_FromUTF8(અચલ અક્ષર *આગત, પૂર્ણાંક exec_આગત, પૂર્ણાંક preserve_crlf)
{
    પાછા tokenizer_from_string(આગત, 1, exec_આગત, preserve_crlf);
}

સંરચના tok_state *
_PyTokenizer_FromReadline(PyObject *readline, અચલ અક્ષર *encoding)
{
    સંરચના tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    જો (tok == ખાલી) {
        પાછા ખાલી;
    }
    જો (encoding != ખાલી && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        પાછા ખાલી;
    }
    tok->reader->readline = Py_NewRef(readline);
    પાછા tok;
}

સંરચના tok_state *
_PyTokenizer_FromFile(FILE *fp, અચલ અક્ષર *encoding,
                      અચલ અક્ષર *ps1, અચલ અક્ષર *ps2)
{
    _PyTok_ReaderKind kind = ps1 != ખાલી || ps2 != ખાલી
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    સંરચના tok_state *tok = tokenizer_new_with_reader(kind);
    જો (tok == ખાલી) {
        પાછા ખાલી;
    }
    જો (encoding != ખાલી && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        પાછા ખાલી;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    પાછા tok;
}

#જો defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
પ્રકારવ્યાખ્યા સંઘ {
    શૂન્ય *cookie;
    પૂર્ણાંક fd;
} borrowed_fd;

સ્થિર ssize_t
borrow_read(શૂન્ય *cookie, અક્ષર *buffer, size_t size)
{
    borrowed_fd borrowed = {.cookie = cookie};
    પાછા પઠ(borrowed.fd, buffer, size);
}

સ્થિર FILE *
fdopen_borrow(પૂર્ણાંક fd)
{
    cookie_io_functions_t callbacks = {borrow_read, ખાલી, ખાલી, ખાલી};
    borrowed_fd borrowed = {.fd = fd};
    પાછા fopencookie(borrowed.cookie, "r", callbacks);
}
#નહીંતર
સ્થિર FILE *
fdopen_borrow(પૂર્ણાંક fd)
{
    પૂર્ણાંક copy = _Py_dup(fd);
    જો (copy < 0) {
        પાછા ખાલી;
    }
    FILE *fp = fdopen(copy, "r");
    જો (fp == ખાલી) {
        પૂર્ણાંક saved_errno = errno;
        close(copy);
        errno = saved_errno;
    }
    પાછા fp;
}
#endif

અક્ષર *
_PyTokenizer_FindEncodingFilename(પૂર્ણાંક fd, PyObject *filename)
{
    FILE *fp = fdopen_borrow(fd);
    જો (fp == ખાલી) {
        પાછા ખાલી;
    }
    સંરચના tok_state *tok = _PyTokenizer_FromFile(fp, ખાલી, ખાલી, ખાલી);
    જો (tok == ખાલી) {
        fclose(fp);
        પાછા ખાલી;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != ખાલી ? filename : &_Py_STR(anon_string));
    અક્ષર *encoding = ખાલી;
    જો (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = ખાલી;
    }
    fclose(fp);
    _PyTokenizer_Free(tok);
    પાછા encoding;
}
