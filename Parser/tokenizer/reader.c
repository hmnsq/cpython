#अन्तर्भाव "Python.h"
#अन्तर्भाव "pycore_fileutils.h"
#अन्तर्भाव "pycore_pystate.h"
#अन्तर्भाव "pycore_runtime.h"

#अन्तर्भाव "errcode.h"
#अन्तर्भाव "helpers.h"
#अन्तर्भाव "reader.h"
#अन्तर्भाव "reader_internal.h"
#अन्तर्भाव "../lexer/state.h"

#define READER_BUFFER_GROWTH_FACTOR 2

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

स्थिर अन्तःस्थ पूर्णाङ्क
reader_is_streaming(_PyTok_ReaderKind kind)
{
    प्रत्यागम kind == _PYTOK_READER_FILE || kind == _PYTOK_READER_READLINE;
}

शून्य
_PyTok_ReaderFree(संरचना tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    यदि (reader == रिक्त) {
        प्रत्यागम;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    प्रति (पूर्णाङ्क i = 0;
         i < (पूर्णाङ्क)Py_ARRAY_LENGTH(reader->prefetched_पंक्तियाँ); i++) {
        _PyTok_ChunkClear(&reader->prefetched_पंक्तियाँ[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_buffer);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = रिक्त;
}

पूर्णाङ्क
_PyTokenizer_Traverse(संरचना tok_state *tok, visitproc visit, शून्य *arg)
{
    Py_VISIT(tok->filename);
    Py_VISIT(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_VISIT(reader->readline);
    Py_VISIT(reader->decoder);
    प्रति (पूर्णाङ्क i = 0;
         i < (पूर्णाङ्क)Py_ARRAY_LENGTH(reader->prefetched_पंक्तियाँ); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_पंक्तियाँ[i];
        यदि (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_VISIT(chunk->owner);
        }
    }
    प्रत्यागम 0;
}

पूर्णाङ्क
_PyTok_ReserveBuffer(वर्ण **buffer, Py_ssize_t *capacity, Py_ssize_t needed,
                     Py_ssize_t initial_capacity)
{
    प्रतिज्ञा(initial_capacity > 0);
    यदि (needed <= *capacity) {
        प्रत्यागम 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *capacity > 0 ? *capacity : initial_capacity;
    यावत् (cap < needed) {
        यदि (cap > PY_SSIZE_T_MAX / READER_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            भङ्ग;
        }
        cap *= READER_BUFFER_GROWTH_FACTOR;
    }
    वर्ण *resized = PyMem_Realloc(*buffer, cap);
    यदि (resized == रिक्त) {
        PyErr_NoMemory();
        प्रत्यागम -1;
    }
    *buffer = resized;
    *capacity = cap;
    प्रत्यागम 0;
}

स्थिर पूर्णाङ्क
append_decoded(_PyTok_Reader *reader, ध्रुव वर्ण *data, Py_ssize_t len)
{
    यदि (len == 0) {
        प्रत्यागम 0;
    }
    यदि (reader->decoded_pos > 0) {
        Py_ssize_t remaining = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (size_t)remaining);
        reader->decoded_scan -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = remaining;
    }
    यदि (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            _PyTok_ReserveBuffer(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        PyErr_NoMemory();
        प्रत्यागम -1;
    }
    memcpy(reader->decoded + reader->decoded_len, data, (size_t)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    प्रत्यागम 0;
}

स्थिर पूर्णाङ्क
append_implicit_newline(_PyTok_Reader *reader)
{
    यदि (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        प्रत्यागम 0;
    }
    यदि (append_decoded(reader, "\n", 1) < 0) {
        प्रत्यागम -1;
    }
    reader->decoded_tail_is_implicit = 1;
    प्रत्यागम 0;
}

स्थिर पूर्णाङ्क
निकालो_decoded_पंक्ति(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    प्रतिज्ञा(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    प्रतिज्ञा(reader->decoded_scan >= reader->decoded_pos &&
           reader->decoded_scan <= reader->decoded_len);
    यदि (reader->decoded_pos == reader->decoded_len) {
        प्रत्यागम 0;
    }
    वर्ण *start = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    वर्ण *newline = memchr(reader->decoded + reader->decoded_scan, '\n',
                           reader->decoded_len - reader->decoded_scan);
    यदि (newline == रिक्त) {
        reader->decoded_scan = reader->decoded_len;
        प्रत्यागम 0;
    }
    Py_ssize_t len = newline - start + 1;
    chunk->data = start;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_scan = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_tail_is_implicit;
    यदि (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_scan = 0;
        reader->decoded_tail_is_implicit = 0;
    }
    प्रत्यागम 1;
}

स्थिर पूर्णाङ्क
chunk_is_पंक्ति(ध्रुव _PyTok_Chunk *chunk)
{
    यदि (chunk->len == 0 || chunk->data[chunk->len - 1] != '\n') {
        प्रत्यागम 0;
    }
    प्रत्यागम memchr(chunk->data, '\n', chunk->len - 1) == रिक्त;
}

स्थिर _PyTok_ReadResult
next_prepared(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    यदि (tok->lineno >= tok->source.nlines) {
        प्रत्यागम _PYTOK_READ_EOF;
    }
    _PyTok_Span tail = {tok->inp, tok->source.base_offset + tok->source.len};
    _PyTok_Off newline = _PyTok_SourceFindByte(&tok->source, tail, '\n');
    _PyTok_Span पंक्ति = {tail.start, newline >= 0 ? newline + 1 : tail.end};
    chunk->data = (वर्ण *)_PyTok_SourceSpanView(
        &tok->source, पंक्ति, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = पंक्ति.end == tail.end
        ? tok->reader->prepared_final_newline : 0;
    प्रत्यागम PYTOK_READ_पंक्ति;
}

स्थिर _PyTok_ReadResult
read_file_पंक्ति(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    प्रति (;;) {
        यदि (len > PY_SSIZE_T_MAX - BUFSIZ) {
            PyErr_NoMemory();
            प्रत्यागम _PYTOK_READ_ERROR;
        }
        यदि (_PyTok_ReserveBuffer(&reader->file_buffer, &reader->file_buffer_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            प्रत्यागम _PYTOK_READ_ERROR;
        }
        पूर्णाङ्क available = (पूर्णाङ्क)Py_MIN(reader->file_buffer_cap - len, INT_MAX);
        size_t पठ = 0;
        वर्ण *result = _Py_UniversalNewlineFgetsWithSize(
            reader->file_buffer + len, available, reader->fp, रिक्त, &पठ);
        यदि (result == रिक्त) {
            यदि (len == 0) {
                प्रत्यागम _PYTOK_READ_EOF;
            }
            भङ्ग;
        }
        len += (Py_ssize_t)पठ;
        यदि (len > 0 && reader->file_buffer[len - 1] == '\n') {
            भङ्ग;
        }
    }
    पूर्णाङ्क implicit = reader->file_buffer[len - 1] != '\n';
    chunk->data = reader->file_buffer;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    प्रत्यागम PYTOK_READ_पंक्ति;
}

स्थिर पूर्णाङ्क
initialize_file(संरचना tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    यदि (reader->encoding != रिक्त) {
        प्रत्यागम _PyTok_StartDecoder(tok, "strict");
    }

    _PyTok_ReadResult result = read_file_पंक्ति(
        tok, &reader->prefetched_पंक्तियाँ[0]);
    यदि (result == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        प्रत्यागम 0;
    }
    यदि (result != PYTOK_READ_पंक्ति) {
        प्रत्यागम -1;
    }
    Py_ssize_t bom_len;
    _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_पंक्तियाँ[0], रिक्त, 0, &bom_len);
    यदि (detection == _PYTOK_ENCODING_ERROR) {
        प्रत्यागम -1;
    }
    यदि (detection == PYTOK_ENCODING_NEED_SECOND_पंक्ति) {
        वर्ण *first = _PyTok_CopyBytes(
            reader->prefetched_पंक्तियाँ[0].data,
            reader->prefetched_पंक्तियाँ[0].len);
        यदि (first == रिक्त) {
            tok->done = E_NOMEM;
            प्रत्यागम -1;
        }
        reader->prefetched_पंक्तियाँ[0].data = first;
        reader->prefetched_पंक्तियाँ[0].ownership = _PYTOK_CHUNK_PYMEM;
        result = read_file_पंक्ति(tok, &reader->prefetched_पंक्तियाँ[1]);
        यदि (result == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        अन्यथा यदि (result != PYTOK_READ_पंक्ति) {
            प्रत्यागम -1;
        }
        _PyTok_Chunk *second = reader->prefetched_पंक्तियाँ[1].data != रिक्त
            ? &reader->prefetched_पंक्तियाँ[1] : रिक्त;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_पंक्तियाँ[0], second, 1, &bom_len);
        यदि (detection == _PYTOK_ENCODING_ERROR) {
            प्रत्यागम -1;
        }
    }
    यदि (bom_len != 0) {
        _PyTok_Chunk *first = &reader->prefetched_पंक्तियाँ[0];
        यदि (first->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(first->data, first->data + bom_len,
                    (size_t)(first->len - bom_len));
            first->data[first->len - bom_len] = '\0';
        }
        अन्यथा {
            first->data += bom_len;
        }
        first->len -= bom_len;
    }
    यदि (_PyTok_StartDecoder(tok, "strict") < 0) {
        प्रत्यागम -1;
    }
    प्रत्यागम 0;
}

स्थिर पूर्णाङ्क
finalize_decoding(संरचना tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    यदि (reader->decoder_finalized) {
        प्रत्यागम 0;
    }
    reader->decoder_finalized = 1;
    यदि (reader->decoder != रिक्त) {
        _PyTok_Chunk निवेश = {
            .data = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        पूर्णाङ्क decoded = _PyTok_DecodeChunk(tok, &निवेश, 1);
        यदि (decoded == 0 &&
                append_decoded(reader, निवेश.data, निवेश.len) < 0) {
            tok->done = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&निवेश);
        यदि (decoded < 0) {
            प्रत्यागम -1;
        }
    }
    यदि (append_implicit_newline(reader) < 0) {
        tok->done = E_NOMEM;
        प्रत्यागम -1;
    }
    प्रत्यागम 0;
}

स्थिर _PyTok_ReadResult
next_file(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    यदि (!reader->file_initialized && initialize_file(tok) < 0) {
        प्रत्यागम _PYTOK_READ_ERROR;
    }
    प्रति (;;) {
        यदि (निकालो_decoded_पंक्ति(reader, chunk)) {
            प्रत्यागम PYTOK_READ_पंक्ति;
        }
        _PyTok_Chunk निवेश = {0};
        यदि (reader->prefetched_पंक्तियाँ[0].data != रिक्त) {
            निवेश = reader->prefetched_पंक्तियाँ[0];
            reader->prefetched_पंक्तियाँ[0] = (_PyTok_Chunk){0};
        }
        अन्यथा यदि (reader->prefetched_पंक्तियाँ[1].data != रिक्त) {
            निवेश = reader->prefetched_पंक्तियाँ[1];
            reader->prefetched_पंक्तियाँ[1] = (_PyTok_Chunk){0};
        }
        अन्यथा यदि (!reader->file_eof) {
            _PyTok_ReadResult result = read_file_पंक्ति(tok, &निवेश);
            यदि (result == _PYTOK_READ_ERROR) {
                प्रत्यागम result;
            }
            यदि (result == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        यदि (निवेश.data != रिक्त) {
            पूर्णाङ्क implicit = निवेश.implicit_newline;
            यदि (reader->decoder == रिक्त && !implicit) {
                *chunk = निवेश;
                प्रत्यागम PYTOK_READ_पंक्ति;
            }
            पूर्णाङ्क decoded = _PyTok_DecodeChunk(tok, &निवेश, 0);
            यदि (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_पंक्ति(&निवेश)) {
                *chunk = निवेश;
                प्रत्यागम PYTOK_READ_पंक्ति;
            }
            यदि (decoded == 0 &&
                    append_decoded(reader, निवेश.data, निवेश.len) < 0) {
                tok->done = E_NOMEM;
                decoded = -1;
            }
            यदि (decoded == 0 && implicit) {
                reader->decoded_tail_is_implicit = 1;
            }
            _PyTok_ChunkClear(&निवेश);
            यदि (decoded < 0) {
                प्रत्यागम _PYTOK_READ_ERROR;
            }
            अनुवर्त;
        }
        यदि (!reader->decoder_finalized) {
            यदि (finalize_decoding(tok) < 0) {
                प्रत्यागम _PYTOK_READ_ERROR;
            }
            अनुवर्त;
        }
        प्रत्यागम _PYTOK_READ_EOF;
    }
}

स्थिर _PyTok_ReadResult
next_readline(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    प्रति (;;) {
        यदि (निकालो_decoded_पंक्ति(reader, chunk)) {
            प्रत्यागम PYTOK_READ_पंक्ति;
        }
        यदि (reader->decoder_finalized) {
            प्रत्यागम _PYTOK_READ_EOF;
        }

        PyObject *raw = PyObject_CallNoArgs(reader->readline);
        यदि (raw == रिक्त) {
            यदि (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                यदि (finalize_decoding(tok) < 0) {
                    प्रत्यागम _PYTOK_READ_ERROR;
                }
                अनुवर्त;
            }
            प्रत्यागम _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk निवेश = {0};
        यदि (reader->encoding != रिक्त) {
            यदि (!PyBytes_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                प्रत्यागम _PYTOK_READ_ERROR;
            }
            यदि (PyBytes_GET_SIZE(raw) == 0) {
                Py_DECREF(raw);
                यदि (_PyTok_StartDecoder(tok, "replace") < 0) {
                    प्रत्यागम _PYTOK_READ_ERROR;
                }
                यदि (finalize_decoding(tok) < 0) {
                    प्रत्यागम _PYTOK_READ_ERROR;
                }
                अनुवर्त;
            }
            निवेश.owner = raw;
            निवेश.data = PyBytes_AS_STRING(raw);
            निवेश.len = PyBytes_GET_SIZE(raw);
            निवेश.ownership = _PYTOK_CHUNK_PYOBJECT;
            पूर्णाङ्क decoded;
            यदि (reader->decoder == रिक्त &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_पंक्ति(&निवेश)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &निवेश, "utf-8", "replace");
            }
            अन्यथा {
                decoded = _PyTok_StartDecoder(tok, "replace");
                यदि (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &निवेश, 0);
                }
            }
            यदि (decoded < 0) {
                _PyTok_ChunkClear(&निवेश);
                प्रत्यागम _PYTOK_READ_ERROR;
            }
        }
        अन्यथा {
            यदि (!PyUnicode_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                प्रत्यागम _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            ध्रुव वर्ण *utf8 = PyUnicode_AsUTF8AndSize(raw, &utf8_len);
            यदि (utf8 == रिक्त) {
                Py_DECREF(raw);
                प्रत्यागम _PYTOK_READ_ERROR;
            }
            निवेश.owner = raw;
            निवेश.data = (वर्ण *)utf8;
            निवेश.len = utf8_len;
            निवेश.ownership = _PYTOK_CHUNK_PYOBJECT;
            यदि (निवेश.len == 0) {
                _PyTok_ChunkClear(&निवेश);
                यदि (finalize_decoding(tok) < 0) {
                    प्रत्यागम _PYTOK_READ_ERROR;
                }
                अनुवर्त;
            }
        }

        यदि (reader->decoded_pos == reader->decoded_len &&
                chunk_is_पंक्ति(&निवेश)) {
            *chunk = निवेश;
            प्रत्यागम PYTOK_READ_पंक्ति;
        }

        यदि (append_decoded(reader, निवेश.data, निवेश.len) < 0) {
            _PyTok_ChunkClear(&निवेश);
            tok->done = E_NOMEM;
            प्रत्यागम _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&निवेश);
        यदि (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            पूर्णाङ्क pending = Py_Tok_Decoder_Has_Buffered_निवेश(tok);
            यदि (pending < 0) {
                प्रत्यागम _PYTOK_READ_ERROR;
            }
            यदि (!pending && append_implicit_newline(reader) < 0) {
                tok->done = E_NOMEM;
                प्रत्यागम _PYTOK_READ_ERROR;
            }
        }
    }
}

स्थिर _PyTok_ReadResult
next_interactive(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    यदि (reader->stop_interactive) {
        प्रत्यागम _PYTOK_READ_STOPPED;
    }
    वर्ण *निवेश = PyOS_Readline(
        reader->fp != रिक्त ? reader->fp : stdin, stdout, reader->prompt);
    यदि (reader->nextprompt != रिक्त) {
        reader->prompt = reader->nextprompt;
    }
    यदि (निवेश == रिक्त) {
        प्रत्यागम _PYTOK_READ_INTERRUPT;
    }
    Py_ssize_t len = strlen(निवेश);
    यदि (len == 0) {
        PyMem_Free(निवेश);
        प्रत्यागम _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .data = निवेश,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    यदि (reader->encoding != रिक्त &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, रिक्त) < 0) {
        _PyTok_ChunkClear(&decoded);
        प्रत्यागम _PYTOK_READ_ERROR;
    }
    यदि (memchr(decoded.data, '\r', decoded.len) == रिक्त) {
        *chunk = decoded;
        प्रत्यागम PYTOK_READ_पंक्ति;
    }
    chunk->data = _PyTok_NormalizeNewlines(
        decoded.data, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    यदि (chunk->data == रिक्त) {
        tok->done = E_NOMEM;
        प्रत्यागम _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    प्रत्यागम PYTOK_READ_पंक्ति;
}

पूर्णाङ्क
_PyTok_ReaderIsInteractive(ध्रुव संरचना tok_state *tok)
{
    प्रत्यागम tok->reader->kind == _PYTOK_READER_INTERACTIVE;
}

ध्रुव वर्ण *
_PyTokenizer_RetainedSource(ध्रुव संरचना tok_state *tok)
{
    यदि (tok->reader->kind == _PYTOK_READER_PREPARED) {
        प्रत्यागम _PyTok_SourceData(&tok->source);
    }
    यदि (tok->reader->kind == _PYTOK_READER_INTERACTIVE) {
        प्रत्यागम tok->source.bytes;
    }
    प्रत्यागम रिक्त;
}

शून्य
_PyTok_ReaderStopInteractive(संरचना tok_state *tok)
{
    यदि (_PyTok_ReaderIsInteractive(tok)) {
        tok->reader->stop_interactive = 1;
    }
}

स्थिर _PyTok_ReadResult
reader_next(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    विकल्प (tok->reader->kind) {
        पक्ष _PYTOK_READER_PREPARED:
            प्रत्यागम next_prepared(tok, chunk);
        पक्ष _PYTOK_READER_FILE:
            प्रत्यागम next_file(tok, chunk);
        पक्ष _PYTOK_READER_READLINE:
            प्रत्यागम next_readline(tok, chunk);
        पक्ष _PYTOK_READER_INTERACTIVE:
            प्रत्यागम next_interactive(tok, chunk);
    }
    Py_UNREACHABLE();
}

स्थिर शून्य
reset_streaming_buffer(संरचना tok_state *tok)
{
    प्रतिज्ञा(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_SourceDiscard(&tok->source);
    tok->buf_offset = tok->source.base_offset;
    tok->cur = tok->inp = tok->source.base_offset;
    tok->पंक्ति_start = tok->buf_offset;
}

पूर्णाङ्क
_PyTok_ReaderUnderflow(संरचना tok_state *tok)
{
    प्रतिज्ञा(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    _PyTok_ReaderKind kind = reader->kind;
    पूर्णाङ्क prepared = kind == _PYTOK_READER_PREPARED;
    पूर्णाङ्क streaming = reader_is_streaming(kind);
    पूर्णाङ्क reset_buffer = tok->start < 0 &&
        _PyLexer_CurrentFTString(tok) == रिक्त;

    _PyTok_Chunk chunk;
    _PyTok_ReadResult result = reader_next(tok, &chunk);
    यदि (result != PYTOK_READ_पंक्ति) {
        यदि (reset_buffer && streaming) {
            reset_streaming_buffer(tok);
        }
        यदि (result == _PYTOK_READ_EOF) {
            tok->done = E_EOF;
        }
        अन्यथा यदि (result == _PYTOK_READ_STOPPED) {
            tok->done = E_INTERACT_STOP;
        }
        अन्यथा यदि (result == _PYTOK_READ_INTERRUPT) {
            tok->done = E_INTR;
        }
        अन्यथा {
            यदि (tok->done == E_OK) {
                tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                    ? E_NOMEM : E_ERROR;
            }
        }
        यदि (kind == _PYTOK_READER_INTERACTIVE &&
                result != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        प्रत्यागम 0;
    }
    यदि (tok->lineno == INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        tok->done = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        प्रत्यागम 0;
    }

    यदि (!prepared) {
        यदि (streaming && reset_buffer) {
            reset_streaming_buffer(tok);
        }
        _PyTok_Off source_start = Py_Tok_Source_Append_पंक्ति(
            &tok->source, chunk.data, chunk.len);
        यदि (source_start < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            प्रत्यागम 0;
        }
        यदि (reset_buffer) {
            tok->cur = source_start;
            tok->buf_offset = source_start;
            tok->पंक्ति_start = tok->buf_offset;
        }
        tok->inp = source_start + chunk.len;
    }
    अन्यथा {
        यदि (reset_buffer) {
            tok->buf_offset = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    यदि (kind == _PYTOK_READER_FILE &&
            (reader->encoding == रिक्त ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                _PyTok_SourcePointer(&tok->source, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        प्रत्यागम 0;
    }
    _PyTok_ChunkClear(&chunk);
    प्रत्यागम 1;
}

स्थिर संरचना tok_state *
tokenizer_new_with_reader(_PyTok_ReaderKind kind)
{
    संरचना tok_state *tok = PyMem_Calloc(1, परिमाण(*tok));
    यदि (tok == रिक्त) {
        PyErr_NoMemory();
        प्रत्यागम रिक्त;
    }
    tok->start = tok->पंक्ति_start = -1;
    _PyTok_SourceInit(&tok->source);
    tok->done = E_OK;
    tok->layout.at_bol = 1;
    tok->start_loc = (_PyTok_Loc){-1, -1};
    tok->ftstring_ढेर = tok->ftstring_ढेर_inline;
    tok->ftstring_capacity = FTSTRING_ढेर_INLINE_CAPACITY;
#ifdef Py_DEBUG
    tok->debug = _Py_GetConfig()->parser_debug;
#endif
    tok->reader = PyMem_Calloc(1, परिमाण(*tok->reader));
    यदि (tok->reader == रिक्त) {
        PyErr_NoMemory();
        _PyTokenizer_Free(tok);
        प्रत्यागम रिक्त;
    }
    tok->reader->kind = kind;
    यदि (kind == _PYTOK_READER_PREPARED) {
        प्रत्यागम tok;
    }
    यदि (reader_is_streaming(kind)) {
        tok->cur = tok->inp = tok->source.base_offset;
        tok->पंक्ति_start = tok->buf_offset;
    }
    प्रत्यागम tok;
}

स्थिर संरचना tok_state *
tokenizer_from_string(ध्रुव वर्ण *निवेश, पूर्णाङ्क utf8_only, पूर्णाङ्क exec_निवेश,
                      पूर्णाङ्क preserve_crlf)
{
    संरचना tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    यदि (tok == रिक्त) {
        प्रत्यागम रिक्त;
    }
    यदि (_PyTok_PrepareString(
            tok, निवेश, utf8_only, exec_निवेश, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        प्रत्यागम रिक्त;
    }
    tok->cur = tok->inp = tok->source.base_offset;
    tok->पंक्ति_start = tok->source.base_offset;
    प्रत्यागम tok;
}

संरचना tok_state *
_PyTokenizer_FromString(ध्रुव वर्ण *निवेश, पूर्णाङ्क exec_निवेश, पूर्णाङ्क preserve_crlf)
{
    प्रत्यागम tokenizer_from_string(निवेश, 0, exec_निवेश, preserve_crlf);
}

संरचना tok_state *
_PyTokenizer_FromUTF8(ध्रुव वर्ण *निवेश, पूर्णाङ्क exec_निवेश, पूर्णाङ्क preserve_crlf)
{
    प्रत्यागम tokenizer_from_string(निवेश, 1, exec_निवेश, preserve_crlf);
}

संरचना tok_state *
_PyTokenizer_FromReadline(PyObject *readline, ध्रुव वर्ण *encoding)
{
    संरचना tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    यदि (tok == रिक्त) {
        प्रत्यागम रिक्त;
    }
    यदि (encoding != रिक्त && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        प्रत्यागम रिक्त;
    }
    tok->reader->readline = Py_NewRef(readline);
    प्रत्यागम tok;
}

संरचना tok_state *
_PyTokenizer_FromFile(FILE *fp, ध्रुव वर्ण *encoding,
                      ध्रुव वर्ण *ps1, ध्रुव वर्ण *ps2)
{
    _PyTok_ReaderKind kind = ps1 != रिक्त || ps2 != रिक्त
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    संरचना tok_state *tok = tokenizer_new_with_reader(kind);
    यदि (tok == रिक्त) {
        प्रत्यागम रिक्त;
    }
    यदि (encoding != रिक्त && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        प्रत्यागम रिक्त;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    प्रत्यागम tok;
}

#यदि defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
प्रकारनिर्देश सङ्घ {
    शून्य *cookie;
    पूर्णाङ्क fd;
} borrowed_fd;

स्थिर ssize_t
borrow_read(शून्य *cookie, वर्ण *buffer, size_t size)
{
    borrowed_fd borrowed = {.cookie = cookie};
    प्रत्यागम पठ(borrowed.fd, buffer, size);
}

स्थिर FILE *
fdopen_borrow(पूर्णाङ्क fd)
{
    cookie_io_functions_t callbacks = {borrow_read, रिक्त, रिक्त, रिक्त};
    borrowed_fd borrowed = {.fd = fd};
    प्रत्यागम fopencookie(borrowed.cookie, "r", callbacks);
}
#अन्यथा
स्थिर FILE *
fdopen_borrow(पूर्णाङ्क fd)
{
    पूर्णाङ्क copy = _Py_dup(fd);
    यदि (copy < 0) {
        प्रत्यागम रिक्त;
    }
    FILE *fp = fdopen(copy, "r");
    यदि (fp == रिक्त) {
        पूर्णाङ्क saved_errno = errno;
        close(copy);
        errno = saved_errno;
    }
    प्रत्यागम fp;
}
#endif

वर्ण *
_PyTokenizer_FindEncodingFilename(पूर्णाङ्क fd, PyObject *filename)
{
    FILE *fp = fdopen_borrow(fd);
    यदि (fp == रिक्त) {
        प्रत्यागम रिक्त;
    }
    संरचना tok_state *tok = _PyTokenizer_FromFile(fp, रिक्त, रिक्त, रिक्त);
    यदि (tok == रिक्त) {
        fclose(fp);
        प्रत्यागम रिक्त;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != रिक्त ? filename : &_Py_STR(anon_string));
    वर्ण *encoding = रिक्त;
    यदि (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = रिक्त;
    }
    fclose(fp);
    _PyTokenizer_Free(tok);
    प्रत्यागम encoding;
}
