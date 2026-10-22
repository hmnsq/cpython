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
    फर्काऊ kind == _PYTOK_READER_FILE || kind == _PYTOK_READER_READLINE;
}

शून्य
_PyTok_ReaderFree(संरचना tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    यदि (reader == रिक्त) {
        फर्काऊ;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    लागि (पूर्णाङ्क i = 0;
         i < (पूर्णाङ्क)Py_ARRAY_LENGTH(reader->prefetched_पङ्क्तिहरू); i++) {
        _PyTok_ChunkClear(&reader->prefetched_पङ्क्तिहरू[i]);
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
    लागि (पूर्णाङ्क i = 0;
         i < (पूर्णाङ्क)Py_ARRAY_LENGTH(reader->prefetched_पङ्क्तिहरू); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_पङ्क्तिहरू[i];
        यदि (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_VISIT(chunk->owner);
        }
    }
    फर्काऊ 0;
}

पूर्णाङ्क
_PyTok_ReserveBuffer(वर्ण **buffer, Py_ssize_t *capacity, Py_ssize_t needed,
                     Py_ssize_t initial_capacity)
{
    प्रतिज्ञा(initial_capacity > 0);
    यदि (needed <= *capacity) {
        फर्काऊ 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *capacity > 0 ? *capacity : initial_capacity;
    जबसम्म (cap < needed) {
        यदि (cap > PY_SSIZE_T_MAX / READER_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            रोक;
        }
        cap *= READER_BUFFER_GROWTH_FACTOR;
    }
    वर्ण *resized = PyMem_Realloc(*buffer, cap);
    यदि (resized == रिक्त) {
        PyErr_NoMemory();
        फर्काऊ -1;
    }
    *buffer = resized;
    *capacity = cap;
    फर्काऊ 0;
}

स्थिर पूर्णाङ्क
append_decoded(_PyTok_Reader *reader, अचर वर्ण *data, Py_ssize_t len)
{
    यदि (len == 0) {
        फर्काऊ 0;
    }
    यदि (reader->decoded_pos > 0) {
        Py_ssize_t remaining = reader->decoded_len - reader->decoded_pos;
        स_हटाओ(reader->decoded, reader->decoded + reader->decoded_pos,
                (माप_प्रकार)remaining);
        reader->decoded_scan -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = remaining;
    }
    यदि (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            _PyTok_ReserveBuffer(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, बफ_मान) < 0) {
        PyErr_NoMemory();
        फर्काऊ -1;
    }
    स_नकल(reader->decoded + reader->decoded_len, data, (माप_प्रकार)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    फर्काऊ 0;
}

स्थिर पूर्णाङ्क
append_implicit_newline(_PyTok_Reader *reader)
{
    यदि (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        फर्काऊ 0;
    }
    यदि (append_decoded(reader, "\n", 1) < 0) {
        फर्काऊ -1;
    }
    reader->decoded_tail_is_implicit = 1;
    फर्काऊ 0;
}

स्थिर पूर्णाङ्क
झिक_decoded_पङ्क्ति(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    प्रतिज्ञा(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    प्रतिज्ञा(reader->decoded_scan >= reader->decoded_pos &&
           reader->decoded_scan <= reader->decoded_len);
    यदि (reader->decoded_pos == reader->decoded_len) {
        फर्काऊ 0;
    }
    वर्ण *start = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    वर्ण *newline = स_प्रथम(reader->decoded + reader->decoded_scan, '\n',
                           reader->decoded_len - reader->decoded_scan);
    यदि (newline == रिक्त) {
        reader->decoded_scan = reader->decoded_len;
        फर्काऊ 0;
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
    फर्काऊ 1;
}

स्थिर पूर्णाङ्क
chunk_is_पङ्क्ति(अचर _PyTok_Chunk *chunk)
{
    यदि (chunk->len == 0 || chunk->data[chunk->len - 1] != '\n') {
        फर्काऊ 0;
    }
    फर्काऊ स_प्रथम(chunk->data, '\n', chunk->len - 1) == रिक्त;
}

स्थिर _PyTok_ReadResult
next_prepared(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    यदि (tok->lineno >= tok->source.nlines) {
        फर्काऊ _PYTOK_READ_EOF;
    }
    _PyTok_Span tail = {tok->inp, tok->source.base_offset + tok->source.len};
    _PyTok_Off newline = _PyTok_SourceFindByte(&tok->source, tail, '\n');
    _PyTok_Span पङ्क्ति = {tail.start, newline >= 0 ? newline + 1 : tail.end};
    chunk->data = (वर्ण *)_PyTok_SourceSpanView(
        &tok->source, पङ्क्ति, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = पङ्क्ति.end == tail.end
        ? tok->reader->prepared_final_newline : 0;
    फर्काऊ PYTOK_READ_पङ्क्ति;
}

स्थिर _PyTok_ReadResult
read_file_पङ्क्ति(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    लागि (;;) {
        यदि (len > PY_SSIZE_T_MAX - बफ_मान) {
            PyErr_NoMemory();
            फर्काऊ _PYTOK_READ_ERROR;
        }
        यदि (_PyTok_ReserveBuffer(&reader->file_buffer, &reader->file_buffer_cap,
                                len + बफ_मान, बफ_मान) < 0) {
            फर्काऊ _PYTOK_READ_ERROR;
        }
        पूर्णाङ्क available = (पूर्णाङ्क)Py_MIN(reader->file_buffer_cap - len, पूर्णांक_उच्च);
        माप_प्रकार पठ = 0;
        वर्ण *result = _Py_UniversalNewlineFgetsWithSize(
            reader->file_buffer + len, available, reader->fp, रिक्त, &पठ);
        यदि (result == रिक्त) {
            यदि (len == 0) {
                फर्काऊ _PYTOK_READ_EOF;
            }
            रोक;
        }
        len += (Py_ssize_t)पठ;
        यदि (len > 0 && reader->file_buffer[len - 1] == '\n') {
            रोक;
        }
    }
    पूर्णाङ्क implicit = reader->file_buffer[len - 1] != '\n';
    chunk->data = reader->file_buffer;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    फर्काऊ PYTOK_READ_पङ्क्ति;
}

स्थिर पूर्णाङ्क
initialize_file(संरचना tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    यदि (reader->encoding != रिक्त) {
        फर्काऊ _PyTok_StartDecoder(tok, "strict");
    }

    _PyTok_ReadResult result = read_file_पङ्क्ति(
        tok, &reader->prefetched_पङ्क्तिहरू[0]);
    यदि (result == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        फर्काऊ 0;
    }
    यदि (result != PYTOK_READ_पङ्क्ति) {
        फर्काऊ -1;
    }
    Py_ssize_t bom_len;
    _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_पङ्क्तिहरू[0], रिक्त, 0, &bom_len);
    यदि (detection == _PYTOK_ENCODING_ERROR) {
        फर्काऊ -1;
    }
    यदि (detection == PYTOK_ENCODING_NEED_SECOND_पङ्क्ति) {
        वर्ण *first = _PyTok_CopyBytes(
            reader->prefetched_पङ्क्तिहरू[0].data,
            reader->prefetched_पङ्क्तिहरू[0].len);
        यदि (first == रिक्त) {
            tok->done = E_NOMEM;
            फर्काऊ -1;
        }
        reader->prefetched_पङ्क्तिहरू[0].data = first;
        reader->prefetched_पङ्क्तिहरू[0].ownership = _PYTOK_CHUNK_PYMEM;
        result = read_file_पङ्क्ति(tok, &reader->prefetched_पङ्क्तिहरू[1]);
        यदि (result == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        अन्यथा यदि (result != PYTOK_READ_पङ्क्ति) {
            फर्काऊ -1;
        }
        _PyTok_Chunk *second = reader->prefetched_पङ्क्तिहरू[1].data != रिक्त
            ? &reader->prefetched_पङ्क्तिहरू[1] : रिक्त;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_पङ्क्तिहरू[0], second, 1, &bom_len);
        यदि (detection == _PYTOK_ENCODING_ERROR) {
            फर्काऊ -1;
        }
    }
    यदि (bom_len != 0) {
        _PyTok_Chunk *first = &reader->prefetched_पङ्क्तिहरू[0];
        यदि (first->ownership == _PYTOK_CHUNK_PYMEM) {
            स_हटाओ(first->data, first->data + bom_len,
                    (माप_प्रकार)(first->len - bom_len));
            first->data[first->len - bom_len] = '\0';
        }
        अन्यथा {
            first->data += bom_len;
        }
        first->len -= bom_len;
    }
    यदि (_PyTok_StartDecoder(tok, "strict") < 0) {
        फर्काऊ -1;
    }
    फर्काऊ 0;
}

स्थिर पूर्णाङ्क
finalize_decoding(संरचना tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    यदि (reader->decoder_finalized) {
        फर्काऊ 0;
    }
    reader->decoder_finalized = 1;
    यदि (reader->decoder != रिक्त) {
        _PyTok_Chunk आगत = {
            .data = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        पूर्णाङ्क decoded = _PyTok_DecodeChunk(tok, &आगत, 1);
        यदि (decoded == 0 &&
                append_decoded(reader, आगत.data, आगत.len) < 0) {
            tok->done = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&आगत);
        यदि (decoded < 0) {
            फर्काऊ -1;
        }
    }
    यदि (append_implicit_newline(reader) < 0) {
        tok->done = E_NOMEM;
        फर्काऊ -1;
    }
    फर्काऊ 0;
}

स्थिर _PyTok_ReadResult
next_file(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    यदि (!reader->file_initialized && initialize_file(tok) < 0) {
        फर्काऊ _PYTOK_READ_ERROR;
    }
    लागि (;;) {
        यदि (झिक_decoded_पङ्क्ति(reader, chunk)) {
            फर्काऊ PYTOK_READ_पङ्क्ति;
        }
        _PyTok_Chunk आगत = {0};
        यदि (reader->prefetched_पङ्क्तिहरू[0].data != रिक्त) {
            आगत = reader->prefetched_पङ्क्तिहरू[0];
            reader->prefetched_पङ्क्तिहरू[0] = (_PyTok_Chunk){0};
        }
        अन्यथा यदि (reader->prefetched_पङ्क्तिहरू[1].data != रिक्त) {
            आगत = reader->prefetched_पङ्क्तिहरू[1];
            reader->prefetched_पङ्क्तिहरू[1] = (_PyTok_Chunk){0};
        }
        अन्यथा यदि (!reader->file_eof) {
            _PyTok_ReadResult result = read_file_पङ्क्ति(tok, &आगत);
            यदि (result == _PYTOK_READ_ERROR) {
                फर्काऊ result;
            }
            यदि (result == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        यदि (आगत.data != रिक्त) {
            पूर्णाङ्क implicit = आगत.implicit_newline;
            यदि (reader->decoder == रिक्त && !implicit) {
                *chunk = आगत;
                फर्काऊ PYTOK_READ_पङ्क्ति;
            }
            पूर्णाङ्क decoded = _PyTok_DecodeChunk(tok, &आगत, 0);
            यदि (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_पङ्क्ति(&आगत)) {
                *chunk = आगत;
                फर्काऊ PYTOK_READ_पङ्क्ति;
            }
            यदि (decoded == 0 &&
                    append_decoded(reader, आगत.data, आगत.len) < 0) {
                tok->done = E_NOMEM;
                decoded = -1;
            }
            यदि (decoded == 0 && implicit) {
                reader->decoded_tail_is_implicit = 1;
            }
            _PyTok_ChunkClear(&आगत);
            यदि (decoded < 0) {
                फर्काऊ _PYTOK_READ_ERROR;
            }
            जारी;
        }
        यदि (!reader->decoder_finalized) {
            यदि (finalize_decoding(tok) < 0) {
                फर्काऊ _PYTOK_READ_ERROR;
            }
            जारी;
        }
        फर्काऊ _PYTOK_READ_EOF;
    }
}

स्थिर _PyTok_ReadResult
next_readline(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    लागि (;;) {
        यदि (झिक_decoded_पङ्क्ति(reader, chunk)) {
            फर्काऊ PYTOK_READ_पङ्क्ति;
        }
        यदि (reader->decoder_finalized) {
            फर्काऊ _PYTOK_READ_EOF;
        }

        PyObject *raw = PyObject_CallNoArgs(reader->readline);
        यदि (raw == रिक्त) {
            यदि (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                यदि (finalize_decoding(tok) < 0) {
                    फर्काऊ _PYTOK_READ_ERROR;
                }
                जारी;
            }
            फर्काऊ _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk आगत = {0};
        यदि (reader->encoding != रिक्त) {
            यदि (!PyBytes_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                फर्काऊ _PYTOK_READ_ERROR;
            }
            यदि (PyBytes_GET_SIZE(raw) == 0) {
                Py_DECREF(raw);
                यदि (_PyTok_StartDecoder(tok, "replace") < 0) {
                    फर्काऊ _PYTOK_READ_ERROR;
                }
                यदि (finalize_decoding(tok) < 0) {
                    फर्काऊ _PYTOK_READ_ERROR;
                }
                जारी;
            }
            आगत.owner = raw;
            आगत.data = PyBytes_AS_STRING(raw);
            आगत.len = PyBytes_GET_SIZE(raw);
            आगत.ownership = _PYTOK_CHUNK_PYOBJECT;
            पूर्णाङ्क decoded;
            यदि (reader->decoder == रिक्त &&
                    म_भेद(reader->encoding, "utf-8") == 0 &&
                    chunk_is_पङ्क्ति(&आगत)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &आगत, "utf-8", "replace");
            }
            अन्यथा {
                decoded = _PyTok_StartDecoder(tok, "replace");
                यदि (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &आगत, 0);
                }
            }
            यदि (decoded < 0) {
                _PyTok_ChunkClear(&आगत);
                फर्काऊ _PYTOK_READ_ERROR;
            }
        }
        अन्यथा {
            यदि (!PyUnicode_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                फर्काऊ _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            अचर वर्ण *utf8 = PyUnicode_AsUTF8AndSize(raw, &utf8_len);
            यदि (utf8 == रिक्त) {
                Py_DECREF(raw);
                फर्काऊ _PYTOK_READ_ERROR;
            }
            आगत.owner = raw;
            आगत.data = (वर्ण *)utf8;
            आगत.len = utf8_len;
            आगत.ownership = _PYTOK_CHUNK_PYOBJECT;
            यदि (आगत.len == 0) {
                _PyTok_ChunkClear(&आगत);
                यदि (finalize_decoding(tok) < 0) {
                    फर्काऊ _PYTOK_READ_ERROR;
                }
                जारी;
            }
        }

        यदि (reader->decoded_pos == reader->decoded_len &&
                chunk_is_पङ्क्ति(&आगत)) {
            *chunk = आगत;
            फर्काऊ PYTOK_READ_पङ्क्ति;
        }

        यदि (append_decoded(reader, आगत.data, आगत.len) < 0) {
            _PyTok_ChunkClear(&आगत);
            tok->done = E_NOMEM;
            फर्काऊ _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&आगत);
        यदि (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            पूर्णाङ्क pending = Py_Tok_Decoder_Has_Buffered_आगत(tok);
            यदि (pending < 0) {
                फर्काऊ _PYTOK_READ_ERROR;
            }
            यदि (!pending && append_implicit_newline(reader) < 0) {
                tok->done = E_NOMEM;
                फर्काऊ _PYTOK_READ_ERROR;
            }
        }
    }
}

स्थिर _PyTok_ReadResult
next_interactive(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    यदि (reader->stop_interactive) {
        फर्काऊ _PYTOK_READ_STOPPED;
    }
    वर्ण *आगत = PyOS_Readline(
        reader->fp != रिक्त ? reader->fp : मानक_निवेश, मानक_निकास, reader->prompt);
    यदि (reader->nextprompt != रिक्त) {
        reader->prompt = reader->nextprompt;
    }
    यदि (आगत == रिक्त) {
        फर्काऊ _PYTOK_READ_INTERRUPT;
    }
    Py_ssize_t len = म_माप(आगत);
    यदि (len == 0) {
        PyMem_Free(आगत);
        फर्काऊ _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .data = आगत,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    यदि (reader->encoding != रिक्त &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, रिक्त) < 0) {
        _PyTok_ChunkClear(&decoded);
        फर्काऊ _PYTOK_READ_ERROR;
    }
    यदि (स_प्रथम(decoded.data, '\r', decoded.len) == रिक्त) {
        *chunk = decoded;
        फर्काऊ PYTOK_READ_पङ्क्ति;
    }
    chunk->data = _PyTok_NormalizeNewlines(
        decoded.data, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    यदि (chunk->data == रिक्त) {
        tok->done = E_NOMEM;
        फर्काऊ _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    फर्काऊ PYTOK_READ_पङ्क्ति;
}

पूर्णाङ्क
_PyTok_ReaderIsInteractive(अचर संरचना tok_state *tok)
{
    फर्काऊ tok->reader->kind == _PYTOK_READER_INTERACTIVE;
}

अचर वर्ण *
_PyTokenizer_RetainedSource(अचर संरचना tok_state *tok)
{
    यदि (tok->reader->kind == _PYTOK_READER_PREPARED) {
        फर्काऊ _PyTok_SourceData(&tok->source);
    }
    यदि (tok->reader->kind == _PYTOK_READER_INTERACTIVE) {
        फर्काऊ tok->source.bytes;
    }
    फर्काऊ रिक्त;
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
            फर्काऊ next_prepared(tok, chunk);
        पक्ष _PYTOK_READER_FILE:
            फर्काऊ next_file(tok, chunk);
        पक्ष _PYTOK_READER_READLINE:
            फर्काऊ next_readline(tok, chunk);
        पक्ष _PYTOK_READER_INTERACTIVE:
            फर्काऊ next_interactive(tok, chunk);
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
    tok->पङ्क्ति_start = tok->buf_offset;
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
    यदि (result != PYTOK_READ_पङ्क्ति) {
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
        फर्काऊ 0;
    }
    यदि (tok->lineno == पूर्णांक_उच्च) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        tok->done = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        फर्काऊ 0;
    }

    यदि (!prepared) {
        यदि (streaming && reset_buffer) {
            reset_streaming_buffer(tok);
        }
        _PyTok_Off source_start = Py_Tok_Source_Append_पङ्क्ति(
            &tok->source, chunk.data, chunk.len);
        यदि (source_start < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            फर्काऊ 0;
        }
        यदि (reset_buffer) {
            tok->cur = source_start;
            tok->buf_offset = source_start;
            tok->पङ्क्ति_start = tok->buf_offset;
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
             म_भेद(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                _PyTok_SourcePointer(&tok->source, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        फर्काऊ 0;
    }
    _PyTok_ChunkClear(&chunk);
    फर्काऊ 1;
}

स्थिर संरचना tok_state *
tokenizer_new_with_reader(_PyTok_ReaderKind kind)
{
    संरचना tok_state *tok = PyMem_Calloc(1, परिमाण(*tok));
    यदि (tok == रिक्त) {
        PyErr_NoMemory();
        फर्काऊ रिक्त;
    }
    tok->start = tok->पङ्क्ति_start = -1;
    _PyTok_SourceInit(&tok->source);
    tok->done = E_OK;
    tok->layout.at_bol = 1;
    tok->start_loc = (_PyTok_Loc){-1, -1};
    tok->ftstring_थाक = tok->ftstring_थाक_inline;
    tok->ftstring_capacity = FTSTRING_थाक_INLINE_CAPACITY;
#ifdef Py_DEBUG
    tok->debug = _Py_GetConfig()->parser_debug;
#endif
    tok->reader = PyMem_Calloc(1, परिमाण(*tok->reader));
    यदि (tok->reader == रिक्त) {
        PyErr_NoMemory();
        _PyTokenizer_Free(tok);
        फर्काऊ रिक्त;
    }
    tok->reader->kind = kind;
    यदि (kind == _PYTOK_READER_PREPARED) {
        फर्काऊ tok;
    }
    यदि (reader_is_streaming(kind)) {
        tok->cur = tok->inp = tok->source.base_offset;
        tok->पङ्क्ति_start = tok->buf_offset;
    }
    फर्काऊ tok;
}

स्थिर संरचना tok_state *
tokenizer_from_string(अचर वर्ण *आगत, पूर्णाङ्क utf8_only, पूर्णाङ्क exec_आगत,
                      पूर्णाङ्क preserve_crlf)
{
    संरचना tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    यदि (tok == रिक्त) {
        फर्काऊ रिक्त;
    }
    यदि (_PyTok_PrepareString(
            tok, आगत, utf8_only, exec_आगत, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        फर्काऊ रिक्त;
    }
    tok->cur = tok->inp = tok->source.base_offset;
    tok->पङ्क्ति_start = tok->source.base_offset;
    फर्काऊ tok;
}

संरचना tok_state *
_PyTokenizer_FromString(अचर वर्ण *आगत, पूर्णाङ्क exec_आगत, पूर्णाङ्क preserve_crlf)
{
    फर्काऊ tokenizer_from_string(आगत, 0, exec_आगत, preserve_crlf);
}

संरचना tok_state *
_PyTokenizer_FromUTF8(अचर वर्ण *आगत, पूर्णाङ्क exec_आगत, पूर्णाङ्क preserve_crlf)
{
    फर्काऊ tokenizer_from_string(आगत, 1, exec_आगत, preserve_crlf);
}

संरचना tok_state *
_PyTokenizer_FromReadline(PyObject *readline, अचर वर्ण *encoding)
{
    संरचना tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    यदि (tok == रिक्त) {
        फर्काऊ रिक्त;
    }
    यदि (encoding != रिक्त && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        फर्काऊ रिक्त;
    }
    tok->reader->readline = Py_NewRef(readline);
    फर्काऊ tok;
}

संरचना tok_state *
_PyTokenizer_FromFile(खाता *fp, अचर वर्ण *encoding,
                      अचर वर्ण *ps1, अचर वर्ण *ps2)
{
    _PyTok_ReaderKind kind = ps1 != रिक्त || ps2 != रिक्त
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    संरचना tok_state *tok = tokenizer_new_with_reader(kind);
    यदि (tok == रिक्त) {
        फर्काऊ रिक्त;
    }
    यदि (encoding != रिक्त && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        फर्काऊ रिक्त;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    फर्काऊ tok;
}

#यदि defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
प्रकारनिर्देश सङ्घ {
    शून्य *cookie;
    पूर्णाङ्क fd;
} borrowed_fd;

स्थिर ssize_t
borrow_read(शून्य *cookie, वर्ण *buffer, माप_प्रकार size)
{
    borrowed_fd borrowed = {.cookie = cookie};
    फर्काऊ पठ(borrowed.fd, buffer, size);
}

स्थिर खाता *
fdopen_borrow(पूर्णाङ्क fd)
{
    cookie_io_functions_t callbacks = {borrow_read, रिक्त, रिक्त, रिक्त};
    borrowed_fd borrowed = {.fd = fd};
    फर्काऊ fopencookie(borrowed.cookie, "r", callbacks);
}
#अन्यथा
स्थिर खाता *
fdopen_borrow(पूर्णाङ्क fd)
{
    पूर्णाङ्क copy = _Py_dup(fd);
    यदि (copy < 0) {
        फर्काऊ रिक्त;
    }
    खाता *fp = fdopen(copy, "r");
    यदि (fp == रिक्त) {
        पूर्णाङ्क saved_errno = त्रुटि_सं;
        बंद(copy);
        त्रुटि_सं = saved_errno;
    }
    फर्काऊ fp;
}
#endif

वर्ण *
_PyTokenizer_FindEncodingFilename(पूर्णाङ्क fd, PyObject *filename)
{
    खाता *fp = fdopen_borrow(fd);
    यदि (fp == रिक्त) {
        फर्काऊ रिक्त;
    }
    संरचना tok_state *tok = _PyTokenizer_FromFile(fp, रिक्त, रिक्त, रिक्त);
    यदि (tok == रिक्त) {
        ख_बंद(fp);
        फर्काऊ रिक्त;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != रिक्त ? filename : &_Py_STR(anon_string));
    वर्ण *encoding = रिक्त;
    यदि (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = रिक्त;
    }
    ख_बंद(fp);
    _PyTokenizer_Free(tok);
    फर्काऊ encoding;
}
