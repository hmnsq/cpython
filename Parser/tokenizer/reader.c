#অন্তর্ভাৱ "Python.h"
#অন্তর্ভাৱ "pycore_fileutils.h"
#অন্তর্ভাৱ "pycore_pystate.h"
#অন্তর্ভাৱ "pycore_runtime.h"

#অন্তর্ভাৱ "errcode.h"
#অন্তর্ভাৱ "helpers.h"
#অন্তর্ভাৱ "reader.h"
#অন্তর্ভাৱ "reader_internal.h"
#অন্তর্ভাৱ "../lexer/state.h"

#define READER_BUFFER_GROWTH_FACTOR 2

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

স্থির অন্তঃস্থ পূর্ণাঙ্ক
reader_is_streaming(_PyTok_ReaderKind kind)
{
    ঘূৰাই kind == _PYTOK_READER_FILE || kind == _PYTOK_READER_READLINE;
}

শূন্য
_PyTok_ReaderFree(সংরচনা tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    যদি (reader == রিক্ত) {
        ঘূৰাই;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    বাবে (পূর্ণাঙ্ক i = 0;
         i < (পূর্ণাঙ্ক)Py_ARRAY_LENGTH(reader->prefetched_সাৰিগুলি); i++) {
        _PyTok_ChunkClear(&reader->prefetched_সাৰিগুলি[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_buffer);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = রিক্ত;
}

পূর্ণাঙ্ক
_PyTokenizer_Traverse(সংরচনা tok_state *tok, visitproc visit, শূন্য *arg)
{
    Py_VISIT(tok->filename);
    Py_VISIT(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_VISIT(reader->readline);
    Py_VISIT(reader->decoder);
    বাবে (পূর্ণাঙ্ক i = 0;
         i < (পূর্ণাঙ্ক)Py_ARRAY_LENGTH(reader->prefetched_সাৰিগুলি); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_সাৰিগুলি[i];
        যদি (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_VISIT(chunk->owner);
        }
    }
    ঘূৰাই 0;
}

পূর্ণাঙ্ক
_PyTok_ReserveBuffer(ৱর্ণ **buffer, Py_ssize_t *capacity, Py_ssize_t needed,
                     Py_ssize_t initial_capacity)
{
    প্রতিজ্ঞা(initial_capacity > 0);
    যদি (needed <= *capacity) {
        ঘূৰাই 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *capacity > 0 ? *capacity : initial_capacity;
    যেতিয়ালৈ (cap < needed) {
        যদি (cap > PY_SSIZE_T_MAX / READER_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            ভঙ্গ;
        }
        cap *= READER_BUFFER_GROWTH_FACTOR;
    }
    ৱর্ণ *resized = PyMem_Realloc(*buffer, cap);
    যদি (resized == রিক্ত) {
        PyErr_NoMemory();
        ঘূৰাই -1;
    }
    *buffer = resized;
    *capacity = cap;
    ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক
append_decoded(_PyTok_Reader *reader, ধ্রুৱ ৱর্ণ *data, Py_ssize_t len)
{
    যদি (len == 0) {
        ঘূৰাই 0;
    }
    যদি (reader->decoded_pos > 0) {
        Py_ssize_t remaining = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (size_t)remaining);
        reader->decoded_scan -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = remaining;
    }
    যদি (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            _PyTok_ReserveBuffer(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        PyErr_NoMemory();
        ঘূৰাই -1;
    }
    memcpy(reader->decoded + reader->decoded_len, data, (size_t)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক
append_implicit_newline(_PyTok_Reader *reader)
{
    যদি (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        ঘূৰাই 0;
    }
    যদি (append_decoded(reader, "\n", 1) < 0) {
        ঘূৰাই -1;
    }
    reader->decoded_tail_is_implicit = 1;
    ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক
বেৰ_কৰো_decoded_সাৰি(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    প্রতিজ্ঞা(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    প্রতিজ্ঞা(reader->decoded_scan >= reader->decoded_pos &&
           reader->decoded_scan <= reader->decoded_len);
    যদি (reader->decoded_pos == reader->decoded_len) {
        ঘূৰাই 0;
    }
    ৱর্ণ *start = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    ৱর্ণ *newline = memchr(reader->decoded + reader->decoded_scan, '\n',
                           reader->decoded_len - reader->decoded_scan);
    যদি (newline == রিক্ত) {
        reader->decoded_scan = reader->decoded_len;
        ঘূৰাই 0;
    }
    Py_ssize_t len = newline - start + 1;
    chunk->data = start;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_scan = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_tail_is_implicit;
    যদি (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_scan = 0;
        reader->decoded_tail_is_implicit = 0;
    }
    ঘূৰাই 1;
}

স্থির পূর্ণাঙ্ক
chunk_is_সাৰি(ধ্রুৱ _PyTok_Chunk *chunk)
{
    যদি (chunk->len == 0 || chunk->data[chunk->len - 1] != '\n') {
        ঘূৰাই 0;
    }
    ঘূৰাই memchr(chunk->data, '\n', chunk->len - 1) == রিক্ত;
}

স্থির _PyTok_ReadResult
next_prepared(সংরচনা tok_state *tok, _PyTok_Chunk *chunk)
{
    যদি (tok->lineno >= tok->source.nlines) {
        ঘূৰাই _PYTOK_READ_EOF;
    }
    _PyTok_Span tail = {tok->inp, tok->source.base_offset + tok->source.len};
    _PyTok_Off newline = _PyTok_SourceFindByte(&tok->source, tail, '\n');
    _PyTok_Span সাৰি = {tail.start, newline >= 0 ? newline + 1 : tail.end};
    chunk->data = (ৱর্ণ *)_PyTok_SourceSpanView(
        &tok->source, সাৰি, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = সাৰি.end == tail.end
        ? tok->reader->prepared_final_newline : 0;
    ঘূৰাই PYTOK_READ_সাৰি;
}

স্থির _PyTok_ReadResult
read_file_সাৰি(সংরচনা tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    বাবে (;;) {
        যদি (len > PY_SSIZE_T_MAX - BUFSIZ) {
            PyErr_NoMemory();
            ঘূৰাই _PYTOK_READ_ERROR;
        }
        যদি (_PyTok_ReserveBuffer(&reader->file_buffer, &reader->file_buffer_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            ঘূৰাই _PYTOK_READ_ERROR;
        }
        পূর্ণাঙ্ক available = (পূর্ণাঙ্ক)Py_MIN(reader->file_buffer_cap - len, INT_MAX);
        size_t পঠ = 0;
        ৱর্ণ *result = _Py_UniversalNewlineFgetsWithSize(
            reader->file_buffer + len, available, reader->fp, রিক্ত, &পঠ);
        যদি (result == রিক্ত) {
            যদি (len == 0) {
                ঘূৰাই _PYTOK_READ_EOF;
            }
            ভঙ্গ;
        }
        len += (Py_ssize_t)পঠ;
        যদি (len > 0 && reader->file_buffer[len - 1] == '\n') {
            ভঙ্গ;
        }
    }
    পূর্ণাঙ্ক implicit = reader->file_buffer[len - 1] != '\n';
    chunk->data = reader->file_buffer;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    ঘূৰাই PYTOK_READ_সাৰি;
}

স্থির পূর্ণাঙ্ক
initialize_file(সংরচনা tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    যদি (reader->encoding != রিক্ত) {
        ঘূৰাই _PyTok_StartDecoder(tok, "strict");
    }

    _PyTok_ReadResult result = read_file_সাৰি(
        tok, &reader->prefetched_সাৰিগুলি[0]);
    যদি (result == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        ঘূৰাই 0;
    }
    যদি (result != PYTOK_READ_সাৰি) {
        ঘূৰাই -1;
    }
    Py_ssize_t bom_len;
    _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_সাৰিগুলি[0], রিক্ত, 0, &bom_len);
    যদি (detection == _PYTOK_ENCODING_ERROR) {
        ঘূৰাই -1;
    }
    যদি (detection == PYTOK_ENCODING_NEED_SECOND_সাৰি) {
        ৱর্ণ *first = _PyTok_CopyBytes(
            reader->prefetched_সাৰিগুলি[0].data,
            reader->prefetched_সাৰিগুলি[0].len);
        যদি (first == রিক্ত) {
            tok->done = E_NOMEM;
            ঘূৰাই -1;
        }
        reader->prefetched_সাৰিগুলি[0].data = first;
        reader->prefetched_সাৰিগুলি[0].ownership = _PYTOK_CHUNK_PYMEM;
        result = read_file_সাৰি(tok, &reader->prefetched_সাৰিগুলি[1]);
        যদি (result == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        অন্যথা যদি (result != PYTOK_READ_সাৰি) {
            ঘূৰাই -1;
        }
        _PyTok_Chunk *second = reader->prefetched_সাৰিগুলি[1].data != রিক্ত
            ? &reader->prefetched_সাৰিগুলি[1] : রিক্ত;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_সাৰিগুলি[0], second, 1, &bom_len);
        যদি (detection == _PYTOK_ENCODING_ERROR) {
            ঘূৰাই -1;
        }
    }
    যদি (bom_len != 0) {
        _PyTok_Chunk *first = &reader->prefetched_সাৰিগুলি[0];
        যদি (first->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(first->data, first->data + bom_len,
                    (size_t)(first->len - bom_len));
            first->data[first->len - bom_len] = '\0';
        }
        অন্যথা {
            first->data += bom_len;
        }
        first->len -= bom_len;
    }
    যদি (_PyTok_StartDecoder(tok, "strict") < 0) {
        ঘূৰাই -1;
    }
    ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক
finalize_decoding(সংরচনা tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    যদি (reader->decoder_finalized) {
        ঘূৰাই 0;
    }
    reader->decoder_finalized = 1;
    যদি (reader->decoder != রিক্ত) {
        _PyTok_Chunk প্ৰবেশ = {
            .data = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        পূর্ণাঙ্ক decoded = _PyTok_DecodeChunk(tok, &প্ৰবেশ, 1);
        যদি (decoded == 0 &&
                append_decoded(reader, প্ৰবেশ.data, প্ৰবেশ.len) < 0) {
            tok->done = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&প্ৰবেশ);
        যদি (decoded < 0) {
            ঘূৰাই -1;
        }
    }
    যদি (append_implicit_newline(reader) < 0) {
        tok->done = E_NOMEM;
        ঘূৰাই -1;
    }
    ঘূৰাই 0;
}

স্থির _PyTok_ReadResult
next_file(সংরচনা tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    যদি (!reader->file_initialized && initialize_file(tok) < 0) {
        ঘূৰাই _PYTOK_READ_ERROR;
    }
    বাবে (;;) {
        যদি (বেৰ_কৰো_decoded_সাৰি(reader, chunk)) {
            ঘূৰাই PYTOK_READ_সাৰি;
        }
        _PyTok_Chunk প্ৰবেশ = {0};
        যদি (reader->prefetched_সাৰিগুলি[0].data != রিক্ত) {
            প্ৰবেশ = reader->prefetched_সাৰিগুলি[0];
            reader->prefetched_সাৰিগুলি[0] = (_PyTok_Chunk){0};
        }
        অন্যথা যদি (reader->prefetched_সাৰিগুলি[1].data != রিক্ত) {
            প্ৰবেশ = reader->prefetched_সাৰিগুলি[1];
            reader->prefetched_সাৰিগুলি[1] = (_PyTok_Chunk){0};
        }
        অন্যথা যদি (!reader->file_eof) {
            _PyTok_ReadResult result = read_file_সাৰি(tok, &প্ৰবেশ);
            যদি (result == _PYTOK_READ_ERROR) {
                ঘূৰাই result;
            }
            যদি (result == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        যদি (প্ৰবেশ.data != রিক্ত) {
            পূর্ণাঙ্ক implicit = প্ৰবেশ.implicit_newline;
            যদি (reader->decoder == রিক্ত && !implicit) {
                *chunk = প্ৰবেশ;
                ঘূৰাই PYTOK_READ_সাৰি;
            }
            পূর্ণাঙ্ক decoded = _PyTok_DecodeChunk(tok, &প্ৰবেশ, 0);
            যদি (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_সাৰি(&প্ৰবেশ)) {
                *chunk = প্ৰবেশ;
                ঘূৰাই PYTOK_READ_সাৰি;
            }
            যদি (decoded == 0 &&
                    append_decoded(reader, প্ৰবেশ.data, প্ৰবেশ.len) < 0) {
                tok->done = E_NOMEM;
                decoded = -1;
            }
            যদি (decoded == 0 && implicit) {
                reader->decoded_tail_is_implicit = 1;
            }
            _PyTok_ChunkClear(&প্ৰবেশ);
            যদি (decoded < 0) {
                ঘূৰাই _PYTOK_READ_ERROR;
            }
            অনুৱর্ত;
        }
        যদি (!reader->decoder_finalized) {
            যদি (finalize_decoding(tok) < 0) {
                ঘূৰাই _PYTOK_READ_ERROR;
            }
            অনুৱর্ত;
        }
        ঘূৰাই _PYTOK_READ_EOF;
    }
}

স্থির _PyTok_ReadResult
next_readline(সংরচনা tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    বাবে (;;) {
        যদি (বেৰ_কৰো_decoded_সাৰি(reader, chunk)) {
            ঘূৰাই PYTOK_READ_সাৰি;
        }
        যদি (reader->decoder_finalized) {
            ঘূৰাই _PYTOK_READ_EOF;
        }

        PyObject *raw = PyObject_CallNoArgs(reader->readline);
        যদি (raw == রিক্ত) {
            যদি (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                যদি (finalize_decoding(tok) < 0) {
                    ঘূৰাই _PYTOK_READ_ERROR;
                }
                অনুৱর্ত;
            }
            ঘূৰাই _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk প্ৰবেশ = {0};
        যদি (reader->encoding != রিক্ত) {
            যদি (!PyBytes_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                ঘূৰাই _PYTOK_READ_ERROR;
            }
            যদি (PyBytes_GET_SIZE(raw) == 0) {
                Py_DECREF(raw);
                যদি (_PyTok_StartDecoder(tok, "replace") < 0) {
                    ঘূৰাই _PYTOK_READ_ERROR;
                }
                যদি (finalize_decoding(tok) < 0) {
                    ঘূৰাই _PYTOK_READ_ERROR;
                }
                অনুৱর্ত;
            }
            প্ৰবেশ.owner = raw;
            প্ৰবেশ.data = PyBytes_AS_STRING(raw);
            প্ৰবেশ.len = PyBytes_GET_SIZE(raw);
            প্ৰবেশ.ownership = _PYTOK_CHUNK_PYOBJECT;
            পূর্ণাঙ্ক decoded;
            যদি (reader->decoder == রিক্ত &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_সাৰি(&প্ৰবেশ)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &প্ৰবেশ, "utf-8", "replace");
            }
            অন্যথা {
                decoded = _PyTok_StartDecoder(tok, "replace");
                যদি (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &প্ৰবেশ, 0);
                }
            }
            যদি (decoded < 0) {
                _PyTok_ChunkClear(&প্ৰবেশ);
                ঘূৰাই _PYTOK_READ_ERROR;
            }
        }
        অন্যথা {
            যদি (!PyUnicode_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                ঘূৰাই _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            ধ্রুৱ ৱর্ণ *utf8 = PyUnicode_AsUTF8AndSize(raw, &utf8_len);
            যদি (utf8 == রিক্ত) {
                Py_DECREF(raw);
                ঘূৰাই _PYTOK_READ_ERROR;
            }
            প্ৰবেশ.owner = raw;
            প্ৰবেশ.data = (ৱর্ণ *)utf8;
            প্ৰবেশ.len = utf8_len;
            প্ৰবেশ.ownership = _PYTOK_CHUNK_PYOBJECT;
            যদি (প্ৰবেশ.len == 0) {
                _PyTok_ChunkClear(&প্ৰবেশ);
                যদি (finalize_decoding(tok) < 0) {
                    ঘূৰাই _PYTOK_READ_ERROR;
                }
                অনুৱর্ত;
            }
        }

        যদি (reader->decoded_pos == reader->decoded_len &&
                chunk_is_সাৰি(&প্ৰবেশ)) {
            *chunk = প্ৰবেশ;
            ঘূৰাই PYTOK_READ_সাৰি;
        }

        যদি (append_decoded(reader, প্ৰবেশ.data, প্ৰবেশ.len) < 0) {
            _PyTok_ChunkClear(&প্ৰবেশ);
            tok->done = E_NOMEM;
            ঘূৰাই _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&প্ৰবেশ);
        যদি (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            পূর্ণাঙ্ক pending = Py_Tok_Decoder_Has_Buffered_প্ৰবেশ(tok);
            যদি (pending < 0) {
                ঘূৰাই _PYTOK_READ_ERROR;
            }
            যদি (!pending && append_implicit_newline(reader) < 0) {
                tok->done = E_NOMEM;
                ঘূৰাই _PYTOK_READ_ERROR;
            }
        }
    }
}

স্থির _PyTok_ReadResult
next_interactive(সংরচনা tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    যদি (reader->stop_interactive) {
        ঘূৰাই _PYTOK_READ_STOPPED;
    }
    ৱর্ণ *প্ৰবেশ = PyOS_Readline(
        reader->fp != রিক্ত ? reader->fp : stdin, stdout, reader->prompt);
    যদি (reader->nextprompt != রিক্ত) {
        reader->prompt = reader->nextprompt;
    }
    যদি (প্ৰবেশ == রিক্ত) {
        ঘূৰাই _PYTOK_READ_INTERRUPT;
    }
    Py_ssize_t len = strlen(প্ৰবেশ);
    যদি (len == 0) {
        PyMem_Free(প্ৰবেশ);
        ঘূৰাই _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .data = প্ৰবেশ,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    যদি (reader->encoding != রিক্ত &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, রিক্ত) < 0) {
        _PyTok_ChunkClear(&decoded);
        ঘূৰাই _PYTOK_READ_ERROR;
    }
    যদি (memchr(decoded.data, '\r', decoded.len) == রিক্ত) {
        *chunk = decoded;
        ঘূৰাই PYTOK_READ_সাৰি;
    }
    chunk->data = _PyTok_NormalizeNewlines(
        decoded.data, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    যদি (chunk->data == রিক্ত) {
        tok->done = E_NOMEM;
        ঘূৰাই _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    ঘূৰাই PYTOK_READ_সাৰি;
}

পূর্ণাঙ্ক
_PyTok_ReaderIsInteractive(ধ্রুৱ সংরচনা tok_state *tok)
{
    ঘূৰাই tok->reader->kind == _PYTOK_READER_INTERACTIVE;
}

ধ্রুৱ ৱর্ণ *
_PyTokenizer_RetainedSource(ধ্রুৱ সংরচনা tok_state *tok)
{
    যদি (tok->reader->kind == _PYTOK_READER_PREPARED) {
        ঘূৰাই _PyTok_SourceData(&tok->source);
    }
    যদি (tok->reader->kind == _PYTOK_READER_INTERACTIVE) {
        ঘূৰাই tok->source.bytes;
    }
    ঘূৰাই রিক্ত;
}

শূন্য
_PyTok_ReaderStopInteractive(সংরচনা tok_state *tok)
{
    যদি (_PyTok_ReaderIsInteractive(tok)) {
        tok->reader->stop_interactive = 1;
    }
}

স্থির _PyTok_ReadResult
reader_next(সংরচনা tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    ৱিকল্প (tok->reader->kind) {
        পক্ষ _PYTOK_READER_PREPARED:
            ঘূৰাই next_prepared(tok, chunk);
        পক্ষ _PYTOK_READER_FILE:
            ঘূৰাই next_file(tok, chunk);
        পক্ষ _PYTOK_READER_READLINE:
            ঘূৰাই next_readline(tok, chunk);
        পক্ষ _PYTOK_READER_INTERACTIVE:
            ঘূৰাই next_interactive(tok, chunk);
    }
    Py_UNREACHABLE();
}

স্থির শূন্য
reset_streaming_buffer(সংরচনা tok_state *tok)
{
    প্রতিজ্ঞা(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_SourceDiscard(&tok->source);
    tok->buf_offset = tok->source.base_offset;
    tok->cur = tok->inp = tok->source.base_offset;
    tok->সাৰি_start = tok->buf_offset;
}

পূর্ণাঙ্ক
_PyTok_ReaderUnderflow(সংরচনা tok_state *tok)
{
    প্রতিজ্ঞা(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    _PyTok_ReaderKind kind = reader->kind;
    পূর্ণাঙ্ক prepared = kind == _PYTOK_READER_PREPARED;
    পূর্ণাঙ্ক streaming = reader_is_streaming(kind);
    পূর্ণাঙ্ক reset_buffer = tok->start < 0 &&
        _PyLexer_CurrentFTString(tok) == রিক্ত;

    _PyTok_Chunk chunk;
    _PyTok_ReadResult result = reader_next(tok, &chunk);
    যদি (result != PYTOK_READ_সাৰি) {
        যদি (reset_buffer && streaming) {
            reset_streaming_buffer(tok);
        }
        যদি (result == _PYTOK_READ_EOF) {
            tok->done = E_EOF;
        }
        অন্যথা যদি (result == _PYTOK_READ_STOPPED) {
            tok->done = E_INTERACT_STOP;
        }
        অন্যথা যদি (result == _PYTOK_READ_INTERRUPT) {
            tok->done = E_INTR;
        }
        অন্যথা {
            যদি (tok->done == E_OK) {
                tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                    ? E_NOMEM : E_ERROR;
            }
        }
        যদি (kind == _PYTOK_READER_INTERACTIVE &&
                result != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        ঘূৰাই 0;
    }
    যদি (tok->lineno == INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        tok->done = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        ঘূৰাই 0;
    }

    যদি (!prepared) {
        যদি (streaming && reset_buffer) {
            reset_streaming_buffer(tok);
        }
        _PyTok_Off source_start = Py_Tok_Source_Append_সাৰি(
            &tok->source, chunk.data, chunk.len);
        যদি (source_start < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            ঘূৰাই 0;
        }
        যদি (reset_buffer) {
            tok->cur = source_start;
            tok->buf_offset = source_start;
            tok->সাৰি_start = tok->buf_offset;
        }
        tok->inp = source_start + chunk.len;
    }
    অন্যথা {
        যদি (reset_buffer) {
            tok->buf_offset = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    যদি (kind == _PYTOK_READER_FILE &&
            (reader->encoding == রিক্ত ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                _PyTok_SourcePointer(&tok->source, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        ঘূৰাই 0;
    }
    _PyTok_ChunkClear(&chunk);
    ঘূৰাই 1;
}

স্থির সংরচনা tok_state *
tokenizer_new_with_reader(_PyTok_ReaderKind kind)
{
    সংরচনা tok_state *tok = PyMem_Calloc(1, পরিমাণ(*tok));
    যদি (tok == রিক্ত) {
        PyErr_NoMemory();
        ঘূৰাই রিক্ত;
    }
    tok->start = tok->সাৰি_start = -1;
    _PyTok_SourceInit(&tok->source);
    tok->done = E_OK;
    tok->layout.at_bol = 1;
    tok->start_loc = (_PyTok_Loc){-1, -1};
    tok->ftstring_স্তূপ = tok->ftstring_স্তূপ_inline;
    tok->ftstring_capacity = FTSTRING_স্তূপ_INLINE_CAPACITY;
#ifdef Py_DEBUG
    tok->debug = _Py_GetConfig()->parser_debug;
#endif
    tok->reader = PyMem_Calloc(1, পরিমাণ(*tok->reader));
    যদি (tok->reader == রিক্ত) {
        PyErr_NoMemory();
        _PyTokenizer_Free(tok);
        ঘূৰাই রিক্ত;
    }
    tok->reader->kind = kind;
    যদি (kind == _PYTOK_READER_PREPARED) {
        ঘূৰাই tok;
    }
    যদি (reader_is_streaming(kind)) {
        tok->cur = tok->inp = tok->source.base_offset;
        tok->সাৰি_start = tok->buf_offset;
    }
    ঘূৰাই tok;
}

স্থির সংরচনা tok_state *
tokenizer_from_string(ধ্রুৱ ৱর্ণ *প্ৰবেশ, পূর্ণাঙ্ক utf8_only, পূর্ণাঙ্ক exec_প্ৰবেশ,
                      পূর্ণাঙ্ক preserve_crlf)
{
    সংরচনা tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    যদি (tok == রিক্ত) {
        ঘূৰাই রিক্ত;
    }
    যদি (_PyTok_PrepareString(
            tok, প্ৰবেশ, utf8_only, exec_প্ৰবেশ, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        ঘূৰাই রিক্ত;
    }
    tok->cur = tok->inp = tok->source.base_offset;
    tok->সাৰি_start = tok->source.base_offset;
    ঘূৰাই tok;
}

সংরচনা tok_state *
_PyTokenizer_FromString(ধ্রুৱ ৱর্ণ *প্ৰবেশ, পূর্ণাঙ্ক exec_প্ৰবেশ, পূর্ণাঙ্ক preserve_crlf)
{
    ঘূৰাই tokenizer_from_string(প্ৰবেশ, 0, exec_প্ৰবেশ, preserve_crlf);
}

সংরচনা tok_state *
_PyTokenizer_FromUTF8(ধ্রুৱ ৱর্ণ *প্ৰবেশ, পূর্ণাঙ্ক exec_প্ৰবেশ, পূর্ণাঙ্ক preserve_crlf)
{
    ঘূৰাই tokenizer_from_string(প্ৰবেশ, 1, exec_প্ৰবেশ, preserve_crlf);
}

সংরচনা tok_state *
_PyTokenizer_FromReadline(PyObject *readline, ধ্রুৱ ৱর্ণ *encoding)
{
    সংরচনা tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    যদি (tok == রিক্ত) {
        ঘূৰাই রিক্ত;
    }
    যদি (encoding != রিক্ত && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        ঘূৰাই রিক্ত;
    }
    tok->reader->readline = Py_NewRef(readline);
    ঘূৰাই tok;
}

সংরচনা tok_state *
_PyTokenizer_FromFile(FILE *fp, ধ্রুৱ ৱর্ণ *encoding,
                      ধ্রুৱ ৱর্ণ *ps1, ধ্রুৱ ৱর্ণ *ps2)
{
    _PyTok_ReaderKind kind = ps1 != রিক্ত || ps2 != রিক্ত
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    সংরচনা tok_state *tok = tokenizer_new_with_reader(kind);
    যদি (tok == রিক্ত) {
        ঘূৰাই রিক্ত;
    }
    যদি (encoding != রিক্ত && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        ঘূৰাই রিক্ত;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    ঘূৰাই tok;
}

#যদি defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
প্রকারনির্দেশ সঙ্ঘ {
    শূন্য *cookie;
    পূর্ণাঙ্ক fd;
} borrowed_fd;

স্থির ssize_t
borrow_read(শূন্য *cookie, ৱর্ণ *buffer, size_t size)
{
    borrowed_fd borrowed = {.cookie = cookie};
    ঘূৰাই পঠ(borrowed.fd, buffer, size);
}

স্থির FILE *
fdopen_borrow(পূর্ণাঙ্ক fd)
{
    cookie_io_functions_t callbacks = {borrow_read, রিক্ত, রিক্ত, রিক্ত};
    borrowed_fd borrowed = {.fd = fd};
    ঘূৰাই fopencookie(borrowed.cookie, "r", callbacks);
}
#অন্যথা
স্থির FILE *
fdopen_borrow(পূর্ণাঙ্ক fd)
{
    পূর্ণাঙ্ক copy = _Py_dup(fd);
    যদি (copy < 0) {
        ঘূৰাই রিক্ত;
    }
    FILE *fp = fdopen(copy, "r");
    যদি (fp == রিক্ত) {
        পূর্ণাঙ্ক saved_errno = errno;
        close(copy);
        errno = saved_errno;
    }
    ঘূৰাই fp;
}
#endif

ৱর্ণ *
_PyTokenizer_FindEncodingFilename(পূর্ণাঙ্ক fd, PyObject *filename)
{
    FILE *fp = fdopen_borrow(fd);
    যদি (fp == রিক্ত) {
        ঘূৰাই রিক্ত;
    }
    সংরচনা tok_state *tok = _PyTokenizer_FromFile(fp, রিক্ত, রিক্ত, রিক্ত);
    যদি (tok == রিক্ত) {
        fclose(fp);
        ঘূৰাই রিক্ত;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != রিক্ত ? filename : &_Py_STR(anon_string));
    ৱর্ণ *encoding = রিক্ত;
    যদি (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = রিক্ত;
    }
    fclose(fp);
    _PyTokenizer_Free(tok);
    ঘূৰাই encoding;
}
