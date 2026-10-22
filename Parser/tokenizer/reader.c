#அந்தர்பாவ "Python.h"
#அந்தர்பாவ "pycore_fileutils.h"
#அந்தர்பாவ "pycore_pystate.h"
#அந்தர்பாவ "pycore_runtime.h"

#அந்தர்பாவ "errcode.h"
#அந்தர்பாவ "helpers.h"
#அந்தர்பாவ "reader.h"
#அந்தர்பாவ "reader_internal.h"
#அந்தர்பாவ "../lexer/state.h"

#define READER_BUFFER_GROWTH_FACTOR 2

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

நிலையான உள்வரி முழு
reader_is_streaming(_PyTok_ReaderKind kind)
{
    திரும்பு kind == _PYTOK_READER_FILE || kind == _PYTOK_READER_READLINE;
}

வெறுமை
_PyTok_ReaderFree(கட்டமைப்பு tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    எனில் (reader == வெற்று) {
        திரும்பு;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    க்கு (முழு i = 0;
         i < (முழு)Py_ARRAY_LENGTH(reader->prefetched_வரிகள்); i++) {
        _PyTok_ChunkClear(&reader->prefetched_வரிகள்[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_buffer);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = வெற்று;
}

முழு
_PyTokenizer_Traverse(கட்டமைப்பு tok_state *tok, visitproc visit, வெறுமை *arg)
{
    Py_VISIT(tok->filename);
    Py_VISIT(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_VISIT(reader->readline);
    Py_VISIT(reader->decoder);
    க்கு (முழு i = 0;
         i < (முழு)Py_ARRAY_LENGTH(reader->prefetched_வரிகள்); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_வரிகள்[i];
        எனில் (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_VISIT(chunk->owner);
        }
    }
    திரும்பு 0;
}

முழு
_PyTok_ReserveBuffer(எழுத்து **buffer, Py_ssize_t *capacity, Py_ssize_t needed,
                     Py_ssize_t initial_capacity)
{
    ப்ரதிஜ்ஞா(initial_capacity > 0);
    எனில் (needed <= *capacity) {
        திரும்பு 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *capacity > 0 ? *capacity : initial_capacity;
    போது (cap < needed) {
        எனில் (cap > PY_SSIZE_T_MAX / READER_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            நிறுத்து;
        }
        cap *= READER_BUFFER_GROWTH_FACTOR;
    }
    எழுத்து *resized = PyMem_Realloc(*buffer, cap);
    எனில் (resized == வெற்று) {
        PyErr_NoMemory();
        திரும்பு -1;
    }
    *buffer = resized;
    *capacity = cap;
    திரும்பு 0;
}

நிலையான முழு
append_decoded(_PyTok_Reader *reader, மாறிலி எழுத்து *data, Py_ssize_t len)
{
    எனில் (len == 0) {
        திரும்பு 0;
    }
    எனில் (reader->decoded_pos > 0) {
        Py_ssize_t remaining = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (size_t)remaining);
        reader->decoded_scan -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = remaining;
    }
    எனில் (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            _PyTok_ReserveBuffer(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        PyErr_NoMemory();
        திரும்பு -1;
    }
    memcpy(reader->decoded + reader->decoded_len, data, (size_t)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    திரும்பு 0;
}

நிலையான முழு
append_implicit_newline(_PyTok_Reader *reader)
{
    எனில் (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        திரும்பு 0;
    }
    எனில் (append_decoded(reader, "\n", 1) < 0) {
        திரும்பு -1;
    }
    reader->decoded_tail_is_implicit = 1;
    திரும்பு 0;
}

நிலையான முழு
எடு_decoded_வரி(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    ப்ரதிஜ்ஞா(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    ப்ரதிஜ்ஞா(reader->decoded_scan >= reader->decoded_pos &&
           reader->decoded_scan <= reader->decoded_len);
    எனில் (reader->decoded_pos == reader->decoded_len) {
        திரும்பு 0;
    }
    எழுத்து *start = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    எழுத்து *newline = memchr(reader->decoded + reader->decoded_scan, '\n',
                           reader->decoded_len - reader->decoded_scan);
    எனில் (newline == வெற்று) {
        reader->decoded_scan = reader->decoded_len;
        திரும்பு 0;
    }
    Py_ssize_t len = newline - start + 1;
    chunk->data = start;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_scan = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_tail_is_implicit;
    எனில் (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_scan = 0;
        reader->decoded_tail_is_implicit = 0;
    }
    திரும்பு 1;
}

நிலையான முழு
chunk_is_வரி(மாறிலி _PyTok_Chunk *chunk)
{
    எனில் (chunk->len == 0 || chunk->data[chunk->len - 1] != '\n') {
        திரும்பு 0;
    }
    திரும்பு memchr(chunk->data, '\n', chunk->len - 1) == வெற்று;
}

நிலையான _PyTok_ReadResult
next_prepared(கட்டமைப்பு tok_state *tok, _PyTok_Chunk *chunk)
{
    எனில் (tok->lineno >= tok->source.nlines) {
        திரும்பு _PYTOK_READ_EOF;
    }
    _PyTok_Span tail = {tok->inp, tok->source.base_offset + tok->source.len};
    _PyTok_Off newline = _PyTok_SourceFindByte(&tok->source, tail, '\n');
    _PyTok_Span வரி = {tail.start, newline >= 0 ? newline + 1 : tail.end};
    chunk->data = (எழுத்து *)_PyTok_SourceSpanView(
        &tok->source, வரி, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = வரி.end == tail.end
        ? tok->reader->prepared_final_newline : 0;
    திரும்பு PYTOK_READ_வரி;
}

நிலையான _PyTok_ReadResult
read_file_வரி(கட்டமைப்பு tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    க்கு (;;) {
        எனில் (len > PY_SSIZE_T_MAX - BUFSIZ) {
            PyErr_NoMemory();
            திரும்பு _PYTOK_READ_ERROR;
        }
        எனில் (_PyTok_ReserveBuffer(&reader->file_buffer, &reader->file_buffer_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            திரும்பு _PYTOK_READ_ERROR;
        }
        முழு available = (முழு)Py_MIN(reader->file_buffer_cap - len, INT_MAX);
        size_t பட = 0;
        எழுத்து *result = _Py_UniversalNewlineFgetsWithSize(
            reader->file_buffer + len, available, reader->fp, வெற்று, &பட);
        எனில் (result == வெற்று) {
            எனில் (len == 0) {
                திரும்பு _PYTOK_READ_EOF;
            }
            நிறுத்து;
        }
        len += (Py_ssize_t)பட;
        எனில் (len > 0 && reader->file_buffer[len - 1] == '\n') {
            நிறுத்து;
        }
    }
    முழு implicit = reader->file_buffer[len - 1] != '\n';
    chunk->data = reader->file_buffer;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    திரும்பு PYTOK_READ_வரி;
}

நிலையான முழு
initialize_file(கட்டமைப்பு tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    எனில் (reader->encoding != வெற்று) {
        திரும்பு _PyTok_StartDecoder(tok, "strict");
    }

    _PyTok_ReadResult result = read_file_வரி(
        tok, &reader->prefetched_வரிகள்[0]);
    எனில் (result == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        திரும்பு 0;
    }
    எனில் (result != PYTOK_READ_வரி) {
        திரும்பு -1;
    }
    Py_ssize_t bom_len;
    _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_வரிகள்[0], வெற்று, 0, &bom_len);
    எனில் (detection == _PYTOK_ENCODING_ERROR) {
        திரும்பு -1;
    }
    எனில் (detection == PYTOK_ENCODING_NEED_SECOND_வரி) {
        எழுத்து *first = _PyTok_CopyBytes(
            reader->prefetched_வரிகள்[0].data,
            reader->prefetched_வரிகள்[0].len);
        எனில் (first == வெற்று) {
            tok->done = E_NOMEM;
            திரும்பு -1;
        }
        reader->prefetched_வரிகள்[0].data = first;
        reader->prefetched_வரிகள்[0].ownership = _PYTOK_CHUNK_PYMEM;
        result = read_file_வரி(tok, &reader->prefetched_வரிகள்[1]);
        எனில் (result == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        இல்லையெனில் எனில் (result != PYTOK_READ_வரி) {
            திரும்பு -1;
        }
        _PyTok_Chunk *second = reader->prefetched_வரிகள்[1].data != வெற்று
            ? &reader->prefetched_வரிகள்[1] : வெற்று;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_வரிகள்[0], second, 1, &bom_len);
        எனில் (detection == _PYTOK_ENCODING_ERROR) {
            திரும்பு -1;
        }
    }
    எனில் (bom_len != 0) {
        _PyTok_Chunk *first = &reader->prefetched_வரிகள்[0];
        எனில் (first->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(first->data, first->data + bom_len,
                    (size_t)(first->len - bom_len));
            first->data[first->len - bom_len] = '\0';
        }
        இல்லையெனில் {
            first->data += bom_len;
        }
        first->len -= bom_len;
    }
    எனில் (_PyTok_StartDecoder(tok, "strict") < 0) {
        திரும்பு -1;
    }
    திரும்பு 0;
}

நிலையான முழு
finalize_decoding(கட்டமைப்பு tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    எனில் (reader->decoder_finalized) {
        திரும்பு 0;
    }
    reader->decoder_finalized = 1;
    எனில் (reader->decoder != வெற்று) {
        _PyTok_Chunk உள்ளீடு = {
            .data = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        முழு decoded = _PyTok_DecodeChunk(tok, &உள்ளீடு, 1);
        எனில் (decoded == 0 &&
                append_decoded(reader, உள்ளீடு.data, உள்ளீடு.len) < 0) {
            tok->done = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&உள்ளீடு);
        எனில் (decoded < 0) {
            திரும்பு -1;
        }
    }
    எனில் (append_implicit_newline(reader) < 0) {
        tok->done = E_NOMEM;
        திரும்பு -1;
    }
    திரும்பு 0;
}

நிலையான _PyTok_ReadResult
next_file(கட்டமைப்பு tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    எனில் (!reader->file_initialized && initialize_file(tok) < 0) {
        திரும்பு _PYTOK_READ_ERROR;
    }
    க்கு (;;) {
        எனில் (எடு_decoded_வரி(reader, chunk)) {
            திரும்பு PYTOK_READ_வரி;
        }
        _PyTok_Chunk உள்ளீடு = {0};
        எனில் (reader->prefetched_வரிகள்[0].data != வெற்று) {
            உள்ளீடு = reader->prefetched_வரிகள்[0];
            reader->prefetched_வரிகள்[0] = (_PyTok_Chunk){0};
        }
        இல்லையெனில் எனில் (reader->prefetched_வரிகள்[1].data != வெற்று) {
            உள்ளீடு = reader->prefetched_வரிகள்[1];
            reader->prefetched_வரிகள்[1] = (_PyTok_Chunk){0};
        }
        இல்லையெனில் எனில் (!reader->file_eof) {
            _PyTok_ReadResult result = read_file_வரி(tok, &உள்ளீடு);
            எனில் (result == _PYTOK_READ_ERROR) {
                திரும்பு result;
            }
            எனில் (result == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        எனில் (உள்ளீடு.data != வெற்று) {
            முழு implicit = உள்ளீடு.implicit_newline;
            எனில் (reader->decoder == வெற்று && !implicit) {
                *chunk = உள்ளீடு;
                திரும்பு PYTOK_READ_வரி;
            }
            முழு decoded = _PyTok_DecodeChunk(tok, &உள்ளீடு, 0);
            எனில் (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_வரி(&உள்ளீடு)) {
                *chunk = உள்ளீடு;
                திரும்பு PYTOK_READ_வரி;
            }
            எனில் (decoded == 0 &&
                    append_decoded(reader, உள்ளீடு.data, உள்ளீடு.len) < 0) {
                tok->done = E_NOMEM;
                decoded = -1;
            }
            எனில் (decoded == 0 && implicit) {
                reader->decoded_tail_is_implicit = 1;
            }
            _PyTok_ChunkClear(&உள்ளீடு);
            எனில் (decoded < 0) {
                திரும்பு _PYTOK_READ_ERROR;
            }
            தொடர்;
        }
        எனில் (!reader->decoder_finalized) {
            எனில் (finalize_decoding(tok) < 0) {
                திரும்பு _PYTOK_READ_ERROR;
            }
            தொடர்;
        }
        திரும்பு _PYTOK_READ_EOF;
    }
}

நிலையான _PyTok_ReadResult
next_readline(கட்டமைப்பு tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    க்கு (;;) {
        எனில் (எடு_decoded_வரி(reader, chunk)) {
            திரும்பு PYTOK_READ_வரி;
        }
        எனில் (reader->decoder_finalized) {
            திரும்பு _PYTOK_READ_EOF;
        }

        PyObject *raw = PyObject_CallNoArgs(reader->readline);
        எனில் (raw == வெற்று) {
            எனில் (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                எனில் (finalize_decoding(tok) < 0) {
                    திரும்பு _PYTOK_READ_ERROR;
                }
                தொடர்;
            }
            திரும்பு _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk உள்ளீடு = {0};
        எனில் (reader->encoding != வெற்று) {
            எனில் (!PyBytes_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                திரும்பு _PYTOK_READ_ERROR;
            }
            எனில் (PyBytes_GET_SIZE(raw) == 0) {
                Py_DECREF(raw);
                எனில் (_PyTok_StartDecoder(tok, "replace") < 0) {
                    திரும்பு _PYTOK_READ_ERROR;
                }
                எனில் (finalize_decoding(tok) < 0) {
                    திரும்பு _PYTOK_READ_ERROR;
                }
                தொடர்;
            }
            உள்ளீடு.owner = raw;
            உள்ளீடு.data = PyBytes_AS_STRING(raw);
            உள்ளீடு.len = PyBytes_GET_SIZE(raw);
            உள்ளீடு.ownership = _PYTOK_CHUNK_PYOBJECT;
            முழு decoded;
            எனில் (reader->decoder == வெற்று &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_வரி(&உள்ளீடு)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &உள்ளீடு, "utf-8", "replace");
            }
            இல்லையெனில் {
                decoded = _PyTok_StartDecoder(tok, "replace");
                எனில் (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &உள்ளீடு, 0);
                }
            }
            எனில் (decoded < 0) {
                _PyTok_ChunkClear(&உள்ளீடு);
                திரும்பு _PYTOK_READ_ERROR;
            }
        }
        இல்லையெனில் {
            எனில் (!PyUnicode_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                திரும்பு _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            மாறிலி எழுத்து *utf8 = PyUnicode_AsUTF8AndSize(raw, &utf8_len);
            எனில் (utf8 == வெற்று) {
                Py_DECREF(raw);
                திரும்பு _PYTOK_READ_ERROR;
            }
            உள்ளீடு.owner = raw;
            உள்ளீடு.data = (எழுத்து *)utf8;
            உள்ளீடு.len = utf8_len;
            உள்ளீடு.ownership = _PYTOK_CHUNK_PYOBJECT;
            எனில் (உள்ளீடு.len == 0) {
                _PyTok_ChunkClear(&உள்ளீடு);
                எனில் (finalize_decoding(tok) < 0) {
                    திரும்பு _PYTOK_READ_ERROR;
                }
                தொடர்;
            }
        }

        எனில் (reader->decoded_pos == reader->decoded_len &&
                chunk_is_வரி(&உள்ளீடு)) {
            *chunk = உள்ளீடு;
            திரும்பு PYTOK_READ_வரி;
        }

        எனில் (append_decoded(reader, உள்ளீடு.data, உள்ளீடு.len) < 0) {
            _PyTok_ChunkClear(&உள்ளீடு);
            tok->done = E_NOMEM;
            திரும்பு _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&உள்ளீடு);
        எனில் (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            முழு pending = Py_Tok_Decoder_Has_Buffered_உள்ளீடு(tok);
            எனில் (pending < 0) {
                திரும்பு _PYTOK_READ_ERROR;
            }
            எனில் (!pending && append_implicit_newline(reader) < 0) {
                tok->done = E_NOMEM;
                திரும்பு _PYTOK_READ_ERROR;
            }
        }
    }
}

நிலையான _PyTok_ReadResult
next_interactive(கட்டமைப்பு tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    எனில் (reader->stop_interactive) {
        திரும்பு _PYTOK_READ_STOPPED;
    }
    எழுத்து *உள்ளீடு = PyOS_Readline(
        reader->fp != வெற்று ? reader->fp : stdin, stdout, reader->prompt);
    எனில் (reader->nextprompt != வெற்று) {
        reader->prompt = reader->nextprompt;
    }
    எனில் (உள்ளீடு == வெற்று) {
        திரும்பு _PYTOK_READ_INTERRUPT;
    }
    Py_ssize_t len = strlen(உள்ளீடு);
    எனில் (len == 0) {
        PyMem_Free(உள்ளீடு);
        திரும்பு _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .data = உள்ளீடு,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    எனில் (reader->encoding != வெற்று &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, வெற்று) < 0) {
        _PyTok_ChunkClear(&decoded);
        திரும்பு _PYTOK_READ_ERROR;
    }
    எனில் (memchr(decoded.data, '\r', decoded.len) == வெற்று) {
        *chunk = decoded;
        திரும்பு PYTOK_READ_வரி;
    }
    chunk->data = _PyTok_NormalizeNewlines(
        decoded.data, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    எனில் (chunk->data == வெற்று) {
        tok->done = E_NOMEM;
        திரும்பு _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    திரும்பு PYTOK_READ_வரி;
}

முழு
_PyTok_ReaderIsInteractive(மாறிலி கட்டமைப்பு tok_state *tok)
{
    திரும்பு tok->reader->kind == _PYTOK_READER_INTERACTIVE;
}

மாறிலி எழுத்து *
_PyTokenizer_RetainedSource(மாறிலி கட்டமைப்பு tok_state *tok)
{
    எனில் (tok->reader->kind == _PYTOK_READER_PREPARED) {
        திரும்பு _PyTok_SourceData(&tok->source);
    }
    எனில் (tok->reader->kind == _PYTOK_READER_INTERACTIVE) {
        திரும்பு tok->source.bytes;
    }
    திரும்பு வெற்று;
}

வெறுமை
_PyTok_ReaderStopInteractive(கட்டமைப்பு tok_state *tok)
{
    எனில் (_PyTok_ReaderIsInteractive(tok)) {
        tok->reader->stop_interactive = 1;
    }
}

நிலையான _PyTok_ReadResult
reader_next(கட்டமைப்பு tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    தேர்வு (tok->reader->kind) {
        நிலை _PYTOK_READER_PREPARED:
            திரும்பு next_prepared(tok, chunk);
        நிலை _PYTOK_READER_FILE:
            திரும்பு next_file(tok, chunk);
        நிலை _PYTOK_READER_READLINE:
            திரும்பு next_readline(tok, chunk);
        நிலை _PYTOK_READER_INTERACTIVE:
            திரும்பு next_interactive(tok, chunk);
    }
    Py_UNREACHABLE();
}

நிலையான வெறுமை
reset_streaming_buffer(கட்டமைப்பு tok_state *tok)
{
    ப்ரதிஜ்ஞா(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_SourceDiscard(&tok->source);
    tok->buf_offset = tok->source.base_offset;
    tok->cur = tok->inp = tok->source.base_offset;
    tok->வரி_start = tok->buf_offset;
}

முழு
_PyTok_ReaderUnderflow(கட்டமைப்பு tok_state *tok)
{
    ப்ரதிஜ்ஞா(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    _PyTok_ReaderKind kind = reader->kind;
    முழு prepared = kind == _PYTOK_READER_PREPARED;
    முழு streaming = reader_is_streaming(kind);
    முழு reset_buffer = tok->start < 0 &&
        _PyLexer_CurrentFTString(tok) == வெற்று;

    _PyTok_Chunk chunk;
    _PyTok_ReadResult result = reader_next(tok, &chunk);
    எனில் (result != PYTOK_READ_வரி) {
        எனில் (reset_buffer && streaming) {
            reset_streaming_buffer(tok);
        }
        எனில் (result == _PYTOK_READ_EOF) {
            tok->done = E_EOF;
        }
        இல்லையெனில் எனில் (result == _PYTOK_READ_STOPPED) {
            tok->done = E_INTERACT_STOP;
        }
        இல்லையெனில் எனில் (result == _PYTOK_READ_INTERRUPT) {
            tok->done = E_INTR;
        }
        இல்லையெனில் {
            எனில் (tok->done == E_OK) {
                tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                    ? E_NOMEM : E_ERROR;
            }
        }
        எனில் (kind == _PYTOK_READER_INTERACTIVE &&
                result != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        திரும்பு 0;
    }
    எனில் (tok->lineno == INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        tok->done = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        திரும்பு 0;
    }

    எனில் (!prepared) {
        எனில் (streaming && reset_buffer) {
            reset_streaming_buffer(tok);
        }
        _PyTok_Off source_start = Py_Tok_Source_Append_வரி(
            &tok->source, chunk.data, chunk.len);
        எனில் (source_start < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            திரும்பு 0;
        }
        எனில் (reset_buffer) {
            tok->cur = source_start;
            tok->buf_offset = source_start;
            tok->வரி_start = tok->buf_offset;
        }
        tok->inp = source_start + chunk.len;
    }
    இல்லையெனில் {
        எனில் (reset_buffer) {
            tok->buf_offset = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    எனில் (kind == _PYTOK_READER_FILE &&
            (reader->encoding == வெற்று ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                _PyTok_SourcePointer(&tok->source, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        திரும்பு 0;
    }
    _PyTok_ChunkClear(&chunk);
    திரும்பு 1;
}

நிலையான கட்டமைப்பு tok_state *
tokenizer_new_with_reader(_PyTok_ReaderKind kind)
{
    கட்டமைப்பு tok_state *tok = PyMem_Calloc(1, அளவு(*tok));
    எனில் (tok == வெற்று) {
        PyErr_NoMemory();
        திரும்பு வெற்று;
    }
    tok->start = tok->வரி_start = -1;
    _PyTok_SourceInit(&tok->source);
    tok->done = E_OK;
    tok->layout.at_bol = 1;
    tok->start_loc = (_PyTok_Loc){-1, -1};
    tok->ftstring_அடுக்கு = tok->ftstring_அடுக்கு_inline;
    tok->ftstring_capacity = FTSTRING_அடுக்கு_INLINE_CAPACITY;
#ifdef Py_DEBUG
    tok->debug = _Py_GetConfig()->parser_debug;
#endif
    tok->reader = PyMem_Calloc(1, அளவு(*tok->reader));
    எனில் (tok->reader == வெற்று) {
        PyErr_NoMemory();
        _PyTokenizer_Free(tok);
        திரும்பு வெற்று;
    }
    tok->reader->kind = kind;
    எனில் (kind == _PYTOK_READER_PREPARED) {
        திரும்பு tok;
    }
    எனில் (reader_is_streaming(kind)) {
        tok->cur = tok->inp = tok->source.base_offset;
        tok->வரி_start = tok->buf_offset;
    }
    திரும்பு tok;
}

நிலையான கட்டமைப்பு tok_state *
tokenizer_from_string(மாறிலி எழுத்து *உள்ளீடு, முழு utf8_only, முழு exec_உள்ளீடு,
                      முழு preserve_crlf)
{
    கட்டமைப்பு tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    எனில் (tok == வெற்று) {
        திரும்பு வெற்று;
    }
    எனில் (_PyTok_PrepareString(
            tok, உள்ளீடு, utf8_only, exec_உள்ளீடு, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        திரும்பு வெற்று;
    }
    tok->cur = tok->inp = tok->source.base_offset;
    tok->வரி_start = tok->source.base_offset;
    திரும்பு tok;
}

கட்டமைப்பு tok_state *
_PyTokenizer_FromString(மாறிலி எழுத்து *உள்ளீடு, முழு exec_உள்ளீடு, முழு preserve_crlf)
{
    திரும்பு tokenizer_from_string(உள்ளீடு, 0, exec_உள்ளீடு, preserve_crlf);
}

கட்டமைப்பு tok_state *
_PyTokenizer_FromUTF8(மாறிலி எழுத்து *உள்ளீடு, முழு exec_உள்ளீடு, முழு preserve_crlf)
{
    திரும்பு tokenizer_from_string(உள்ளீடு, 1, exec_உள்ளீடு, preserve_crlf);
}

கட்டமைப்பு tok_state *
_PyTokenizer_FromReadline(PyObject *readline, மாறிலி எழுத்து *encoding)
{
    கட்டமைப்பு tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    எனில் (tok == வெற்று) {
        திரும்பு வெற்று;
    }
    எனில் (encoding != வெற்று && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        திரும்பு வெற்று;
    }
    tok->reader->readline = Py_NewRef(readline);
    திரும்பு tok;
}

கட்டமைப்பு tok_state *
_PyTokenizer_FromFile(FILE *fp, மாறிலி எழுத்து *encoding,
                      மாறிலி எழுத்து *ps1, மாறிலி எழுத்து *ps2)
{
    _PyTok_ReaderKind kind = ps1 != வெற்று || ps2 != வெற்று
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    கட்டமைப்பு tok_state *tok = tokenizer_new_with_reader(kind);
    எனில் (tok == வெற்று) {
        திரும்பு வெற்று;
    }
    எனில் (encoding != வெற்று && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        திரும்பு வெற்று;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    திரும்பு tok;
}

#எனில் defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
வகைவரை ஒன்றிப்பு {
    வெறுமை *cookie;
    முழு fd;
} borrowed_fd;

நிலையான ssize_t
borrow_read(வெறுமை *cookie, எழுத்து *buffer, size_t size)
{
    borrowed_fd borrowed = {.cookie = cookie};
    திரும்பு பட(borrowed.fd, buffer, size);
}

நிலையான FILE *
fdopen_borrow(முழு fd)
{
    cookie_io_functions_t callbacks = {borrow_read, வெற்று, வெற்று, வெற்று};
    borrowed_fd borrowed = {.fd = fd};
    திரும்பு fopencookie(borrowed.cookie, "r", callbacks);
}
#இல்லையெனில்
நிலையான FILE *
fdopen_borrow(முழு fd)
{
    முழு copy = _Py_dup(fd);
    எனில் (copy < 0) {
        திரும்பு வெற்று;
    }
    FILE *fp = fdopen(copy, "r");
    எனில் (fp == வெற்று) {
        முழு saved_errno = errno;
        close(copy);
        errno = saved_errno;
    }
    திரும்பு fp;
}
#endif

எழுத்து *
_PyTokenizer_FindEncodingFilename(முழு fd, PyObject *filename)
{
    FILE *fp = fdopen_borrow(fd);
    எனில் (fp == வெற்று) {
        திரும்பு வெற்று;
    }
    கட்டமைப்பு tok_state *tok = _PyTokenizer_FromFile(fp, வெற்று, வெற்று, வெற்று);
    எனில் (tok == வெற்று) {
        fclose(fp);
        திரும்பு வெற்று;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != வெற்று ? filename : &_Py_STR(anon_string));
    எழுத்து *encoding = வெற்று;
    எனில் (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = வெற்று;
    }
    fclose(fp);
    _PyTokenizer_Free(tok);
    திரும்பு encoding;
}
