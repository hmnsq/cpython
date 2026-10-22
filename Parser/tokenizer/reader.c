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
    पटिनिवत्त kind == _PYTOK_READER_FILE || kind == _PYTOK_READER_READLINE;
}

शून्य
_PyTok_ReaderFree(संरचना tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    सचे (reader == रिक्त) {
        पटिनिवत्त;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    अत्थाय (पूर्णाङ्क i = 0;
         i < (पूर्णाङ्क)Py_ARRAY_LENGTH(reader->prefetched_पङ्क्तयः); i++) {
        _PyTok_ChunkClear(&reader->prefetched_पङ्क्तयः[i]);
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
    अत्थाय (पूर्णाङ्क i = 0;
         i < (पूर्णाङ्क)Py_ARRAY_LENGTH(reader->prefetched_पङ्क्तयः); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_पङ्क्तयः[i];
        सचे (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_VISIT(chunk->owner);
        }
    }
    पटिनिवत्त 0;
}

पूर्णाङ्क
_PyTok_ReserveBuffer(वर्ण **buffer, Py_ssize_t *capacity, Py_ssize_t needed,
                     Py_ssize_t initial_capacity)
{
    प्रतिज्ञा(initial_capacity > 0);
    सचे (needed <= *capacity) {
        पटिनिवत्त 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *capacity > 0 ? *capacity : initial_capacity;
    याव (cap < needed) {
        सचे (cap > PY_SSIZE_T_MAX / READER_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            भङ्ग;
        }
        cap *= READER_BUFFER_GROWTH_FACTOR;
    }
    वर्ण *resized = PyMem_Realloc(*buffer, cap);
    सचे (resized == रिक्त) {
        PyErr_NoMemory();
        पटिनिवत्त -1;
    }
    *buffer = resized;
    *capacity = cap;
    पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क
append_decoded(_PyTok_Reader *reader, ध्रुव वर्ण *data, Py_ssize_t len)
{
    सचे (len == 0) {
        पटिनिवत्त 0;
    }
    सचे (reader->decoded_pos > 0) {
        Py_ssize_t remaining = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (size_t)remaining);
        reader->decoded_scan -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = remaining;
    }
    सचे (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            _PyTok_ReserveBuffer(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        PyErr_NoMemory();
        पटिनिवत्त -1;
    }
    memcpy(reader->decoded + reader->decoded_len, data, (size_t)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क
append_implicit_newline(_PyTok_Reader *reader)
{
    सचे (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        पटिनिवत्त 0;
    }
    सचे (append_decoded(reader, "\n", 1) < 0) {
        पटिनिवत्त -1;
    }
    reader->decoded_tail_is_implicit = 1;
    पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क
निष्कासय_decoded_पङ्क्तिः(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    प्रतिज्ञा(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    प्रतिज्ञा(reader->decoded_scan >= reader->decoded_pos &&
           reader->decoded_scan <= reader->decoded_len);
    सचे (reader->decoded_pos == reader->decoded_len) {
        पटिनिवत्त 0;
    }
    वर्ण *start = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    वर्ण *newline = memchr(reader->decoded + reader->decoded_scan, '\n',
                           reader->decoded_len - reader->decoded_scan);
    सचे (newline == रिक्त) {
        reader->decoded_scan = reader->decoded_len;
        पटिनिवत्त 0;
    }
    Py_ssize_t len = newline - start + 1;
    chunk->data = start;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_scan = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_tail_is_implicit;
    सचे (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_scan = 0;
        reader->decoded_tail_is_implicit = 0;
    }
    पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क
chunk_is_पङ्क्तिः(ध्रुव _PyTok_Chunk *chunk)
{
    सचे (chunk->len == 0 || chunk->data[chunk->len - 1] != '\n') {
        पटिनिवत्त 0;
    }
    पटिनिवत्त memchr(chunk->data, '\n', chunk->len - 1) == रिक्त;
}

स्थिर _PyTok_ReadResult
next_prepared(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    सचे (tok->lineno >= tok->source.nlines) {
        पटिनिवत्त _PYTOK_READ_EOF;
    }
    _PyTok_Span tail = {tok->inp, tok->source.base_offset + tok->source.len};
    _PyTok_Off newline = _PyTok_SourceFindByte(&tok->source, tail, '\n');
    _PyTok_Span पङ्क्तिः = {tail.start, newline >= 0 ? newline + 1 : tail.end};
    chunk->data = (वर्ण *)_PyTok_SourceSpanView(
        &tok->source, पङ्क्तिः, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = पङ्क्तिः.end == tail.end
        ? tok->reader->prepared_final_newline : 0;
    पटिनिवत्त PYTOK_READ_पङ्क्तिः;
}

स्थिर _PyTok_ReadResult
read_file_पङ्क्तिः(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    अत्थाय (;;) {
        सचे (len > PY_SSIZE_T_MAX - BUFSIZ) {
            PyErr_NoMemory();
            पटिनिवत्त _PYTOK_READ_ERROR;
        }
        सचे (_PyTok_ReserveBuffer(&reader->file_buffer, &reader->file_buffer_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            पटिनिवत्त _PYTOK_READ_ERROR;
        }
        पूर्णाङ्क available = (पूर्णाङ्क)Py_MIN(reader->file_buffer_cap - len, INT_MAX);
        size_t पठ = 0;
        वर्ण *result = _Py_UniversalNewlineFgetsWithSize(
            reader->file_buffer + len, available, reader->fp, रिक्त, &पठ);
        सचे (result == रिक्त) {
            सचे (len == 0) {
                पटिनिवत्त _PYTOK_READ_EOF;
            }
            भङ्ग;
        }
        len += (Py_ssize_t)पठ;
        सचे (len > 0 && reader->file_buffer[len - 1] == '\n') {
            भङ्ग;
        }
    }
    पूर्णाङ्क implicit = reader->file_buffer[len - 1] != '\n';
    chunk->data = reader->file_buffer;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    पटिनिवत्त PYTOK_READ_पङ्क्तिः;
}

स्थिर पूर्णाङ्क
initialize_file(संरचना tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    सचे (reader->encoding != रिक्त) {
        पटिनिवत्त _PyTok_StartDecoder(tok, "strict");
    }

    _PyTok_ReadResult result = read_file_पङ्क्तिः(
        tok, &reader->prefetched_पङ्क्तयः[0]);
    सचे (result == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        पटिनिवत्त 0;
    }
    सचे (result != PYTOK_READ_पङ्क्तिः) {
        पटिनिवत्त -1;
    }
    Py_ssize_t bom_len;
    _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_पङ्क्तयः[0], रिक्त, 0, &bom_len);
    सचे (detection == _PYTOK_ENCODING_ERROR) {
        पटिनिवत्त -1;
    }
    सचे (detection == PYTOK_ENCODING_NEED_SECOND_पङ्क्तिः) {
        वर्ण *first = _PyTok_CopyBytes(
            reader->prefetched_पङ्क्तयः[0].data,
            reader->prefetched_पङ्क्तयः[0].len);
        सचे (first == रिक्त) {
            tok->done = E_NOMEM;
            पटिनिवत्त -1;
        }
        reader->prefetched_पङ्क्तयः[0].data = first;
        reader->prefetched_पङ्क्तयः[0].ownership = _PYTOK_CHUNK_PYMEM;
        result = read_file_पङ्क्तिः(tok, &reader->prefetched_पङ्क्तयः[1]);
        सचे (result == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        अञ्ञथा सचे (result != PYTOK_READ_पङ्क्तिः) {
            पटिनिवत्त -1;
        }
        _PyTok_Chunk *second = reader->prefetched_पङ्क्तयः[1].data != रिक्त
            ? &reader->prefetched_पङ्क्तयः[1] : रिक्त;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_पङ्क्तयः[0], second, 1, &bom_len);
        सचे (detection == _PYTOK_ENCODING_ERROR) {
            पटिनिवत्त -1;
        }
    }
    सचे (bom_len != 0) {
        _PyTok_Chunk *first = &reader->prefetched_पङ्क्तयः[0];
        सचे (first->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(first->data, first->data + bom_len,
                    (size_t)(first->len - bom_len));
            first->data[first->len - bom_len] = '\0';
        }
        अञ्ञथा {
            first->data += bom_len;
        }
        first->len -= bom_len;
    }
    सचे (_PyTok_StartDecoder(tok, "strict") < 0) {
        पटिनिवत्त -1;
    }
    पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क
finalize_decoding(संरचना tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    सचे (reader->decoder_finalized) {
        पटिनिवत्त 0;
    }
    reader->decoder_finalized = 1;
    सचे (reader->decoder != रिक्त) {
        _PyTok_Chunk निवेशः = {
            .data = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        पूर्णाङ्क decoded = _PyTok_DecodeChunk(tok, &निवेशः, 1);
        सचे (decoded == 0 &&
                append_decoded(reader, निवेशः.data, निवेशः.len) < 0) {
            tok->done = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&निवेशः);
        सचे (decoded < 0) {
            पटिनिवत्त -1;
        }
    }
    सचे (append_implicit_newline(reader) < 0) {
        tok->done = E_NOMEM;
        पटिनिवत्त -1;
    }
    पटिनिवत्त 0;
}

स्थिर _PyTok_ReadResult
next_file(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    सचे (!reader->file_initialized && initialize_file(tok) < 0) {
        पटिनिवत्त _PYTOK_READ_ERROR;
    }
    अत्थाय (;;) {
        सचे (निष्कासय_decoded_पङ्क्तिः(reader, chunk)) {
            पटिनिवत्त PYTOK_READ_पङ्क्तिः;
        }
        _PyTok_Chunk निवेशः = {0};
        सचे (reader->prefetched_पङ्क्तयः[0].data != रिक्त) {
            निवेशः = reader->prefetched_पङ्क्तयः[0];
            reader->prefetched_पङ्क्तयः[0] = (_PyTok_Chunk){0};
        }
        अञ्ञथा सचे (reader->prefetched_पङ्क्तयः[1].data != रिक्त) {
            निवेशः = reader->prefetched_पङ्क्तयः[1];
            reader->prefetched_पङ्क्तयः[1] = (_PyTok_Chunk){0};
        }
        अञ्ञथा सचे (!reader->file_eof) {
            _PyTok_ReadResult result = read_file_पङ्क्तिः(tok, &निवेशः);
            सचे (result == _PYTOK_READ_ERROR) {
                पटिनिवत्त result;
            }
            सचे (result == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        सचे (निवेशः.data != रिक्त) {
            पूर्णाङ्क implicit = निवेशः.implicit_newline;
            सचे (reader->decoder == रिक्त && !implicit) {
                *chunk = निवेशः;
                पटिनिवत्त PYTOK_READ_पङ्क्तिः;
            }
            पूर्णाङ्क decoded = _PyTok_DecodeChunk(tok, &निवेशः, 0);
            सचे (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_पङ्क्तिः(&निवेशः)) {
                *chunk = निवेशः;
                पटिनिवत्त PYTOK_READ_पङ्क्तिः;
            }
            सचे (decoded == 0 &&
                    append_decoded(reader, निवेशः.data, निवेशः.len) < 0) {
                tok->done = E_NOMEM;
                decoded = -1;
            }
            सचे (decoded == 0 && implicit) {
                reader->decoded_tail_is_implicit = 1;
            }
            _PyTok_ChunkClear(&निवेशः);
            सचे (decoded < 0) {
                पटिनिवत्त _PYTOK_READ_ERROR;
            }
            अनुवर्त;
        }
        सचे (!reader->decoder_finalized) {
            सचे (finalize_decoding(tok) < 0) {
                पटिनिवत्त _PYTOK_READ_ERROR;
            }
            अनुवर्त;
        }
        पटिनिवत्त _PYTOK_READ_EOF;
    }
}

स्थिर _PyTok_ReadResult
next_readline(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    अत्थाय (;;) {
        सचे (निष्कासय_decoded_पङ्क्तिः(reader, chunk)) {
            पटिनिवत्त PYTOK_READ_पङ्क्तिः;
        }
        सचे (reader->decoder_finalized) {
            पटिनिवत्त _PYTOK_READ_EOF;
        }

        PyObject *raw = PyObject_CallNoArgs(reader->readline);
        सचे (raw == रिक्त) {
            सचे (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                सचे (finalize_decoding(tok) < 0) {
                    पटिनिवत्त _PYTOK_READ_ERROR;
                }
                अनुवर्त;
            }
            पटिनिवत्त _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk निवेशः = {0};
        सचे (reader->encoding != रिक्त) {
            सचे (!PyBytes_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                पटिनिवत्त _PYTOK_READ_ERROR;
            }
            सचे (PyBytes_GET_SIZE(raw) == 0) {
                Py_DECREF(raw);
                सचे (_PyTok_StartDecoder(tok, "replace") < 0) {
                    पटिनिवत्त _PYTOK_READ_ERROR;
                }
                सचे (finalize_decoding(tok) < 0) {
                    पटिनिवत्त _PYTOK_READ_ERROR;
                }
                अनुवर्त;
            }
            निवेशः.owner = raw;
            निवेशः.data = PyBytes_AS_STRING(raw);
            निवेशः.len = PyBytes_GET_SIZE(raw);
            निवेशः.ownership = _PYTOK_CHUNK_PYOBJECT;
            पूर्णाङ्क decoded;
            सचे (reader->decoder == रिक्त &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_पङ्क्तिः(&निवेशः)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &निवेशः, "utf-8", "replace");
            }
            अञ्ञथा {
                decoded = _PyTok_StartDecoder(tok, "replace");
                सचे (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &निवेशः, 0);
                }
            }
            सचे (decoded < 0) {
                _PyTok_ChunkClear(&निवेशः);
                पटिनिवत्त _PYTOK_READ_ERROR;
            }
        }
        अञ्ञथा {
            सचे (!PyUnicode_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                पटिनिवत्त _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            ध्रुव वर्ण *utf8 = PyUnicode_AsUTF8AndSize(raw, &utf8_len);
            सचे (utf8 == रिक्त) {
                Py_DECREF(raw);
                पटिनिवत्त _PYTOK_READ_ERROR;
            }
            निवेशः.owner = raw;
            निवेशः.data = (वर्ण *)utf8;
            निवेशः.len = utf8_len;
            निवेशः.ownership = _PYTOK_CHUNK_PYOBJECT;
            सचे (निवेशः.len == 0) {
                _PyTok_ChunkClear(&निवेशः);
                सचे (finalize_decoding(tok) < 0) {
                    पटिनिवत्त _PYTOK_READ_ERROR;
                }
                अनुवर्त;
            }
        }

        सचे (reader->decoded_pos == reader->decoded_len &&
                chunk_is_पङ्क्तिः(&निवेशः)) {
            *chunk = निवेशः;
            पटिनिवत्त PYTOK_READ_पङ्क्तिः;
        }

        सचे (append_decoded(reader, निवेशः.data, निवेशः.len) < 0) {
            _PyTok_ChunkClear(&निवेशः);
            tok->done = E_NOMEM;
            पटिनिवत्त _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&निवेशः);
        सचे (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            पूर्णाङ्क pending = Py_Tok_Decoder_Has_Buffered_निवेशः(tok);
            सचे (pending < 0) {
                पटिनिवत्त _PYTOK_READ_ERROR;
            }
            सचे (!pending && append_implicit_newline(reader) < 0) {
                tok->done = E_NOMEM;
                पटिनिवत्त _PYTOK_READ_ERROR;
            }
        }
    }
}

स्थिर _PyTok_ReadResult
next_interactive(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    सचे (reader->stop_interactive) {
        पटिनिवत्त _PYTOK_READ_STOPPED;
    }
    वर्ण *निवेशः = PyOS_Readline(
        reader->fp != रिक्त ? reader->fp : stdin, stdout, reader->prompt);
    सचे (reader->nextprompt != रिक्त) {
        reader->prompt = reader->nextprompt;
    }
    सचे (निवेशः == रिक्त) {
        पटिनिवत्त _PYTOK_READ_INTERRUPT;
    }
    Py_ssize_t len = strlen(निवेशः);
    सचे (len == 0) {
        PyMem_Free(निवेशः);
        पटिनिवत्त _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .data = निवेशः,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    सचे (reader->encoding != रिक्त &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, रिक्त) < 0) {
        _PyTok_ChunkClear(&decoded);
        पटिनिवत्त _PYTOK_READ_ERROR;
    }
    सचे (memchr(decoded.data, '\r', decoded.len) == रिक्त) {
        *chunk = decoded;
        पटिनिवत्त PYTOK_READ_पङ्क्तिः;
    }
    chunk->data = _PyTok_NormalizeNewlines(
        decoded.data, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    सचे (chunk->data == रिक्त) {
        tok->done = E_NOMEM;
        पटिनिवत्त _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    पटिनिवत्त PYTOK_READ_पङ्क्तिः;
}

पूर्णाङ्क
_PyTok_ReaderIsInteractive(ध्रुव संरचना tok_state *tok)
{
    पटिनिवत्त tok->reader->kind == _PYTOK_READER_INTERACTIVE;
}

ध्रुव वर्ण *
_PyTokenizer_RetainedSource(ध्रुव संरचना tok_state *tok)
{
    सचे (tok->reader->kind == _PYTOK_READER_PREPARED) {
        पटिनिवत्त _PyTok_SourceData(&tok->source);
    }
    सचे (tok->reader->kind == _PYTOK_READER_INTERACTIVE) {
        पटिनिवत्त tok->source.bytes;
    }
    पटिनिवत्त रिक्त;
}

शून्य
_PyTok_ReaderStopInteractive(संरचना tok_state *tok)
{
    सचे (_PyTok_ReaderIsInteractive(tok)) {
        tok->reader->stop_interactive = 1;
    }
}

स्थिर _PyTok_ReadResult
reader_next(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    विकल्प (tok->reader->kind) {
        पक्ष _PYTOK_READER_PREPARED:
            पटिनिवत्त next_prepared(tok, chunk);
        पक्ष _PYTOK_READER_FILE:
            पटिनिवत्त next_file(tok, chunk);
        पक्ष _PYTOK_READER_READLINE:
            पटिनिवत्त next_readline(tok, chunk);
        पक्ष _PYTOK_READER_INTERACTIVE:
            पटिनिवत्त next_interactive(tok, chunk);
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
    tok->पङ्क्तिः_start = tok->buf_offset;
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
    सचे (result != PYTOK_READ_पङ्क्तिः) {
        सचे (reset_buffer && streaming) {
            reset_streaming_buffer(tok);
        }
        सचे (result == _PYTOK_READ_EOF) {
            tok->done = E_EOF;
        }
        अञ्ञथा सचे (result == _PYTOK_READ_STOPPED) {
            tok->done = E_INTERACT_STOP;
        }
        अञ्ञथा सचे (result == _PYTOK_READ_INTERRUPT) {
            tok->done = E_INTR;
        }
        अञ्ञथा {
            सचे (tok->done == E_OK) {
                tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                    ? E_NOMEM : E_ERROR;
            }
        }
        सचे (kind == _PYTOK_READER_INTERACTIVE &&
                result != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        पटिनिवत्त 0;
    }
    सचे (tok->lineno == INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        tok->done = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        पटिनिवत्त 0;
    }

    सचे (!prepared) {
        सचे (streaming && reset_buffer) {
            reset_streaming_buffer(tok);
        }
        _PyTok_Off source_start = Py_Tok_Source_Append_पङ्क्तिः(
            &tok->source, chunk.data, chunk.len);
        सचे (source_start < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            पटिनिवत्त 0;
        }
        सचे (reset_buffer) {
            tok->cur = source_start;
            tok->buf_offset = source_start;
            tok->पङ्क्तिः_start = tok->buf_offset;
        }
        tok->inp = source_start + chunk.len;
    }
    अञ्ञथा {
        सचे (reset_buffer) {
            tok->buf_offset = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    सचे (kind == _PYTOK_READER_FILE &&
            (reader->encoding == रिक्त ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                _PyTok_SourcePointer(&tok->source, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        पटिनिवत्त 0;
    }
    _PyTok_ChunkClear(&chunk);
    पटिनिवत्त 1;
}

स्थिर संरचना tok_state *
tokenizer_new_with_reader(_PyTok_ReaderKind kind)
{
    संरचना tok_state *tok = PyMem_Calloc(1, परिमाण(*tok));
    सचे (tok == रिक्त) {
        PyErr_NoMemory();
        पटिनिवत्त रिक्त;
    }
    tok->start = tok->पङ्क्तिः_start = -1;
    _PyTok_SourceInit(&tok->source);
    tok->done = E_OK;
    tok->layout.at_bol = 1;
    tok->start_loc = (_PyTok_Loc){-1, -1};
    tok->ftstring_राशिः = tok->ftstring_राशिः_inline;
    tok->ftstring_capacity = FTSTRING_राशिः_INLINE_CAPACITY;
#ifdef Py_DEBUG
    tok->debug = _Py_GetConfig()->parser_debug;
#endif
    tok->reader = PyMem_Calloc(1, परिमाण(*tok->reader));
    सचे (tok->reader == रिक्त) {
        PyErr_NoMemory();
        _PyTokenizer_Free(tok);
        पटिनिवत्त रिक्त;
    }
    tok->reader->kind = kind;
    सचे (kind == _PYTOK_READER_PREPARED) {
        पटिनिवत्त tok;
    }
    सचे (reader_is_streaming(kind)) {
        tok->cur = tok->inp = tok->source.base_offset;
        tok->पङ्क्तिः_start = tok->buf_offset;
    }
    पटिनिवत्त tok;
}

स्थिर संरचना tok_state *
tokenizer_from_string(ध्रुव वर्ण *निवेशः, पूर्णाङ्क utf8_only, पूर्णाङ्क exec_निवेशः,
                      पूर्णाङ्क preserve_crlf)
{
    संरचना tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    सचे (tok == रिक्त) {
        पटिनिवत्त रिक्त;
    }
    सचे (_PyTok_PrepareString(
            tok, निवेशः, utf8_only, exec_निवेशः, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        पटिनिवत्त रिक्त;
    }
    tok->cur = tok->inp = tok->source.base_offset;
    tok->पङ्क्तिः_start = tok->source.base_offset;
    पटिनिवत्त tok;
}

संरचना tok_state *
_PyTokenizer_FromString(ध्रुव वर्ण *निवेशः, पूर्णाङ्क exec_निवेशः, पूर्णाङ्क preserve_crlf)
{
    पटिनिवत्त tokenizer_from_string(निवेशः, 0, exec_निवेशः, preserve_crlf);
}

संरचना tok_state *
_PyTokenizer_FromUTF8(ध्रुव वर्ण *निवेशः, पूर्णाङ्क exec_निवेशः, पूर्णाङ्क preserve_crlf)
{
    पटिनिवत्त tokenizer_from_string(निवेशः, 1, exec_निवेशः, preserve_crlf);
}

संरचना tok_state *
_PyTokenizer_FromReadline(PyObject *readline, ध्रुव वर्ण *encoding)
{
    संरचना tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    सचे (tok == रिक्त) {
        पटिनिवत्त रिक्त;
    }
    सचे (encoding != रिक्त && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        पटिनिवत्त रिक्त;
    }
    tok->reader->readline = Py_NewRef(readline);
    पटिनिवत्त tok;
}

संरचना tok_state *
_PyTokenizer_FromFile(FILE *fp, ध्रुव वर्ण *encoding,
                      ध्रुव वर्ण *ps1, ध्रुव वर्ण *ps2)
{
    _PyTok_ReaderKind kind = ps1 != रिक्त || ps2 != रिक्त
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    संरचना tok_state *tok = tokenizer_new_with_reader(kind);
    सचे (tok == रिक्त) {
        पटिनिवत्त रिक्त;
    }
    सचे (encoding != रिक्त && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        पटिनिवत्त रिक्त;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    पटिनिवत्त tok;
}

#सचे defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
प्रकारनिर्देश सङ्घ {
    शून्य *cookie;
    पूर्णाङ्क fd;
} borrowed_fd;

स्थिर ssize_t
borrow_read(शून्य *cookie, वर्ण *buffer, size_t size)
{
    borrowed_fd borrowed = {.cookie = cookie};
    पटिनिवत्त पठ(borrowed.fd, buffer, size);
}

स्थिर FILE *
fdopen_borrow(पूर्णाङ्क fd)
{
    cookie_io_functions_t callbacks = {borrow_read, रिक्त, रिक्त, रिक्त};
    borrowed_fd borrowed = {.fd = fd};
    पटिनिवत्त fopencookie(borrowed.cookie, "r", callbacks);
}
#अञ्ञथा
स्थिर FILE *
fdopen_borrow(पूर्णाङ्क fd)
{
    पूर्णाङ्क copy = _Py_dup(fd);
    सचे (copy < 0) {
        पटिनिवत्त रिक्त;
    }
    FILE *fp = fdopen(copy, "r");
    सचे (fp == रिक्त) {
        पूर्णाङ्क saved_errno = errno;
        close(copy);
        errno = saved_errno;
    }
    पटिनिवत्त fp;
}
#endif

वर्ण *
_PyTokenizer_FindEncodingFilename(पूर्णाङ्क fd, PyObject *filename)
{
    FILE *fp = fdopen_borrow(fd);
    सचे (fp == रिक्त) {
        पटिनिवत्त रिक्त;
    }
    संरचना tok_state *tok = _PyTokenizer_FromFile(fp, रिक्त, रिक्त, रिक्त);
    सचे (tok == रिक्त) {
        fclose(fp);
        पटिनिवत्त रिक्त;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != रिक्त ? filename : &_Py_STR(anon_string));
    वर्ण *encoding = रिक्त;
    सचे (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = रिक्त;
    }
    fclose(fp);
    _PyTokenizer_Free(tok);
    पटिनिवत्त encoding;
}
