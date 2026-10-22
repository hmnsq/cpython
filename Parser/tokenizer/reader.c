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

स्थिर ओळीत पूर्णांक
reader_is_streaming(_PyTok_ReaderKind kind)
{
    परत kind == _PYTOK_READER_FILE || kind == _PYTOK_READER_READLINE;
}

शून्य
_PyTok_ReaderFree(संरचना tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    जर (reader == रिक्त) {
        परत;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    साठी (पूर्णांक i = 0;
         i < (पूर्णांक)Py_ARRAY_LENGTH(reader->prefetched_ओळी); i++) {
        _PyTok_ChunkClear(&reader->prefetched_ओळी[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_buffer);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = रिक्त;
}

पूर्णांक
_PyTokenizer_Traverse(संरचना tok_state *tok, visitproc visit, शून्य *arg)
{
    Py_VISIT(tok->filename);
    Py_VISIT(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_VISIT(reader->readline);
    Py_VISIT(reader->decoder);
    साठी (पूर्णांक i = 0;
         i < (पूर्णांक)Py_ARRAY_LENGTH(reader->prefetched_ओळी); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_ओळी[i];
        जर (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_VISIT(chunk->owner);
        }
    }
    परत 0;
}

पूर्णांक
_PyTok_ReserveBuffer(अक्षर **buffer, Py_ssize_t *capacity, Py_ssize_t needed,
                     Py_ssize_t initial_capacity)
{
    प्रतिज्ञा(initial_capacity > 0);
    जर (needed <= *capacity) {
        परत 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *capacity > 0 ? *capacity : initial_capacity;
    पर्यंत (cap < needed) {
        जर (cap > PY_SSIZE_T_MAX / READER_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            थांबा;
        }
        cap *= READER_BUFFER_GROWTH_FACTOR;
    }
    अक्षर *resized = PyMem_Realloc(*buffer, cap);
    जर (resized == रिक्त) {
        PyErr_NoMemory();
        परत -1;
    }
    *buffer = resized;
    *capacity = cap;
    परत 0;
}

स्थिर पूर्णांक
append_decoded(_PyTok_Reader *reader, स्थिरांक अक्षर *data, Py_ssize_t len)
{
    जर (len == 0) {
        परत 0;
    }
    जर (reader->decoded_pos > 0) {
        Py_ssize_t remaining = reader->decoded_len - reader->decoded_pos;
        स_हटाओ(reader->decoded, reader->decoded + reader->decoded_pos,
                (माप_प्रकार)remaining);
        reader->decoded_scan -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = remaining;
    }
    जर (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            _PyTok_ReserveBuffer(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, बफ_मान) < 0) {
        PyErr_NoMemory();
        परत -1;
    }
    स_नकल(reader->decoded + reader->decoded_len, data, (माप_प्रकार)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    परत 0;
}

स्थिर पूर्णांक
append_implicit_newline(_PyTok_Reader *reader)
{
    जर (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        परत 0;
    }
    जर (append_decoded(reader, "\n", 1) < 0) {
        परत -1;
    }
    reader->decoded_tail_is_implicit = 1;
    परत 0;
}

स्थिर पूर्णांक
काढा_decoded_ओळ(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    प्रतिज्ञा(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    प्रतिज्ञा(reader->decoded_scan >= reader->decoded_pos &&
           reader->decoded_scan <= reader->decoded_len);
    जर (reader->decoded_pos == reader->decoded_len) {
        परत 0;
    }
    अक्षर *start = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    अक्षर *newline = स_प्रथम(reader->decoded + reader->decoded_scan, '\n',
                           reader->decoded_len - reader->decoded_scan);
    जर (newline == रिक्त) {
        reader->decoded_scan = reader->decoded_len;
        परत 0;
    }
    Py_ssize_t len = newline - start + 1;
    chunk->data = start;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_scan = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_tail_is_implicit;
    जर (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_scan = 0;
        reader->decoded_tail_is_implicit = 0;
    }
    परत 1;
}

स्थिर पूर्णांक
chunk_is_ओळ(स्थिरांक _PyTok_Chunk *chunk)
{
    जर (chunk->len == 0 || chunk->data[chunk->len - 1] != '\n') {
        परत 0;
    }
    परत स_प्रथम(chunk->data, '\n', chunk->len - 1) == रिक्त;
}

स्थिर _PyTok_ReadResult
next_prepared(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    जर (tok->lineno >= tok->source.nlines) {
        परत _PYTOK_READ_EOF;
    }
    _PyTok_Span tail = {tok->inp, tok->source.base_offset + tok->source.len};
    _PyTok_Off newline = _PyTok_SourceFindByte(&tok->source, tail, '\n');
    _PyTok_Span ओळ = {tail.start, newline >= 0 ? newline + 1 : tail.end};
    chunk->data = (अक्षर *)_PyTok_SourceSpanView(
        &tok->source, ओळ, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = ओळ.end == tail.end
        ? tok->reader->prepared_final_newline : 0;
    परत PYTOK_READ_ओळ;
}

स्थिर _PyTok_ReadResult
read_file_ओळ(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    साठी (;;) {
        जर (len > PY_SSIZE_T_MAX - बफ_मान) {
            PyErr_NoMemory();
            परत _PYTOK_READ_ERROR;
        }
        जर (_PyTok_ReserveBuffer(&reader->file_buffer, &reader->file_buffer_cap,
                                len + बफ_मान, बफ_मान) < 0) {
            परत _PYTOK_READ_ERROR;
        }
        पूर्णांक available = (पूर्णांक)Py_MIN(reader->file_buffer_cap - len, पूर्णांक_उच्च);
        माप_प्रकार पठ = 0;
        अक्षर *result = _Py_UniversalNewlineFgetsWithSize(
            reader->file_buffer + len, available, reader->fp, रिक्त, &पठ);
        जर (result == रिक्त) {
            जर (len == 0) {
                परत _PYTOK_READ_EOF;
            }
            थांबा;
        }
        len += (Py_ssize_t)पठ;
        जर (len > 0 && reader->file_buffer[len - 1] == '\n') {
            थांबा;
        }
    }
    पूर्णांक implicit = reader->file_buffer[len - 1] != '\n';
    chunk->data = reader->file_buffer;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    परत PYTOK_READ_ओळ;
}

स्थिर पूर्णांक
initialize_file(संरचना tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    जर (reader->encoding != रिक्त) {
        परत _PyTok_StartDecoder(tok, "strict");
    }

    _PyTok_ReadResult result = read_file_ओळ(
        tok, &reader->prefetched_ओळी[0]);
    जर (result == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        परत 0;
    }
    जर (result != PYTOK_READ_ओळ) {
        परत -1;
    }
    Py_ssize_t bom_len;
    _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_ओळी[0], रिक्त, 0, &bom_len);
    जर (detection == _PYTOK_ENCODING_ERROR) {
        परत -1;
    }
    जर (detection == PYTOK_ENCODING_NEED_SECOND_ओळ) {
        अक्षर *first = _PyTok_CopyBytes(
            reader->prefetched_ओळी[0].data,
            reader->prefetched_ओळी[0].len);
        जर (first == रिक्त) {
            tok->done = E_NOMEM;
            परत -1;
        }
        reader->prefetched_ओळी[0].data = first;
        reader->prefetched_ओळी[0].ownership = _PYTOK_CHUNK_PYMEM;
        result = read_file_ओळ(tok, &reader->prefetched_ओळी[1]);
        जर (result == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        अन्यथा जर (result != PYTOK_READ_ओळ) {
            परत -1;
        }
        _PyTok_Chunk *second = reader->prefetched_ओळी[1].data != रिक्त
            ? &reader->prefetched_ओळी[1] : रिक्त;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_ओळी[0], second, 1, &bom_len);
        जर (detection == _PYTOK_ENCODING_ERROR) {
            परत -1;
        }
    }
    जर (bom_len != 0) {
        _PyTok_Chunk *first = &reader->prefetched_ओळी[0];
        जर (first->ownership == _PYTOK_CHUNK_PYMEM) {
            स_हटाओ(first->data, first->data + bom_len,
                    (माप_प्रकार)(first->len - bom_len));
            first->data[first->len - bom_len] = '\0';
        }
        अन्यथा {
            first->data += bom_len;
        }
        first->len -= bom_len;
    }
    जर (_PyTok_StartDecoder(tok, "strict") < 0) {
        परत -1;
    }
    परत 0;
}

स्थिर पूर्णांक
finalize_decoding(संरचना tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    जर (reader->decoder_finalized) {
        परत 0;
    }
    reader->decoder_finalized = 1;
    जर (reader->decoder != रिक्त) {
        _PyTok_Chunk आदान = {
            .data = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        पूर्णांक decoded = _PyTok_DecodeChunk(tok, &आदान, 1);
        जर (decoded == 0 &&
                append_decoded(reader, आदान.data, आदान.len) < 0) {
            tok->done = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&आदान);
        जर (decoded < 0) {
            परत -1;
        }
    }
    जर (append_implicit_newline(reader) < 0) {
        tok->done = E_NOMEM;
        परत -1;
    }
    परत 0;
}

स्थिर _PyTok_ReadResult
next_file(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    जर (!reader->file_initialized && initialize_file(tok) < 0) {
        परत _PYTOK_READ_ERROR;
    }
    साठी (;;) {
        जर (काढा_decoded_ओळ(reader, chunk)) {
            परत PYTOK_READ_ओळ;
        }
        _PyTok_Chunk आदान = {0};
        जर (reader->prefetched_ओळी[0].data != रिक्त) {
            आदान = reader->prefetched_ओळी[0];
            reader->prefetched_ओळी[0] = (_PyTok_Chunk){0};
        }
        अन्यथा जर (reader->prefetched_ओळी[1].data != रिक्त) {
            आदान = reader->prefetched_ओळी[1];
            reader->prefetched_ओळी[1] = (_PyTok_Chunk){0};
        }
        अन्यथा जर (!reader->file_eof) {
            _PyTok_ReadResult result = read_file_ओळ(tok, &आदान);
            जर (result == _PYTOK_READ_ERROR) {
                परत result;
            }
            जर (result == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        जर (आदान.data != रिक्त) {
            पूर्णांक implicit = आदान.implicit_newline;
            जर (reader->decoder == रिक्त && !implicit) {
                *chunk = आदान;
                परत PYTOK_READ_ओळ;
            }
            पूर्णांक decoded = _PyTok_DecodeChunk(tok, &आदान, 0);
            जर (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_ओळ(&आदान)) {
                *chunk = आदान;
                परत PYTOK_READ_ओळ;
            }
            जर (decoded == 0 &&
                    append_decoded(reader, आदान.data, आदान.len) < 0) {
                tok->done = E_NOMEM;
                decoded = -1;
            }
            जर (decoded == 0 && implicit) {
                reader->decoded_tail_is_implicit = 1;
            }
            _PyTok_ChunkClear(&आदान);
            जर (decoded < 0) {
                परत _PYTOK_READ_ERROR;
            }
            सुरूठेवा;
        }
        जर (!reader->decoder_finalized) {
            जर (finalize_decoding(tok) < 0) {
                परत _PYTOK_READ_ERROR;
            }
            सुरूठेवा;
        }
        परत _PYTOK_READ_EOF;
    }
}

स्थिर _PyTok_ReadResult
next_readline(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    साठी (;;) {
        जर (काढा_decoded_ओळ(reader, chunk)) {
            परत PYTOK_READ_ओळ;
        }
        जर (reader->decoder_finalized) {
            परत _PYTOK_READ_EOF;
        }

        PyObject *raw = PyObject_CallNoArgs(reader->readline);
        जर (raw == रिक्त) {
            जर (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                जर (finalize_decoding(tok) < 0) {
                    परत _PYTOK_READ_ERROR;
                }
                सुरूठेवा;
            }
            परत _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk आदान = {0};
        जर (reader->encoding != रिक्त) {
            जर (!PyBytes_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                परत _PYTOK_READ_ERROR;
            }
            जर (PyBytes_GET_SIZE(raw) == 0) {
                Py_DECREF(raw);
                जर (_PyTok_StartDecoder(tok, "replace") < 0) {
                    परत _PYTOK_READ_ERROR;
                }
                जर (finalize_decoding(tok) < 0) {
                    परत _PYTOK_READ_ERROR;
                }
                सुरूठेवा;
            }
            आदान.owner = raw;
            आदान.data = PyBytes_AS_STRING(raw);
            आदान.len = PyBytes_GET_SIZE(raw);
            आदान.ownership = _PYTOK_CHUNK_PYOBJECT;
            पूर्णांक decoded;
            जर (reader->decoder == रिक्त &&
                    म_भेद(reader->encoding, "utf-8") == 0 &&
                    chunk_is_ओळ(&आदान)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &आदान, "utf-8", "replace");
            }
            अन्यथा {
                decoded = _PyTok_StartDecoder(tok, "replace");
                जर (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &आदान, 0);
                }
            }
            जर (decoded < 0) {
                _PyTok_ChunkClear(&आदान);
                परत _PYTOK_READ_ERROR;
            }
        }
        अन्यथा {
            जर (!PyUnicode_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                परत _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            स्थिरांक अक्षर *utf8 = PyUnicode_AsUTF8AndSize(raw, &utf8_len);
            जर (utf8 == रिक्त) {
                Py_DECREF(raw);
                परत _PYTOK_READ_ERROR;
            }
            आदान.owner = raw;
            आदान.data = (अक्षर *)utf8;
            आदान.len = utf8_len;
            आदान.ownership = _PYTOK_CHUNK_PYOBJECT;
            जर (आदान.len == 0) {
                _PyTok_ChunkClear(&आदान);
                जर (finalize_decoding(tok) < 0) {
                    परत _PYTOK_READ_ERROR;
                }
                सुरूठेवा;
            }
        }

        जर (reader->decoded_pos == reader->decoded_len &&
                chunk_is_ओळ(&आदान)) {
            *chunk = आदान;
            परत PYTOK_READ_ओळ;
        }

        जर (append_decoded(reader, आदान.data, आदान.len) < 0) {
            _PyTok_ChunkClear(&आदान);
            tok->done = E_NOMEM;
            परत _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&आदान);
        जर (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            पूर्णांक pending = Py_Tok_Decoder_Has_Buffered_आदान(tok);
            जर (pending < 0) {
                परत _PYTOK_READ_ERROR;
            }
            जर (!pending && append_implicit_newline(reader) < 0) {
                tok->done = E_NOMEM;
                परत _PYTOK_READ_ERROR;
            }
        }
    }
}

स्थिर _PyTok_ReadResult
next_interactive(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    जर (reader->stop_interactive) {
        परत _PYTOK_READ_STOPPED;
    }
    अक्षर *आदान = PyOS_Readline(
        reader->fp != रिक्त ? reader->fp : मानक_निवेश, मानक_निकास, reader->prompt);
    जर (reader->nextprompt != रिक्त) {
        reader->prompt = reader->nextprompt;
    }
    जर (आदान == रिक्त) {
        परत _PYTOK_READ_INTERRUPT;
    }
    Py_ssize_t len = म_माप(आदान);
    जर (len == 0) {
        PyMem_Free(आदान);
        परत _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .data = आदान,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    जर (reader->encoding != रिक्त &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, रिक्त) < 0) {
        _PyTok_ChunkClear(&decoded);
        परत _PYTOK_READ_ERROR;
    }
    जर (स_प्रथम(decoded.data, '\r', decoded.len) == रिक्त) {
        *chunk = decoded;
        परत PYTOK_READ_ओळ;
    }
    chunk->data = _PyTok_NormalizeNewlines(
        decoded.data, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    जर (chunk->data == रिक्त) {
        tok->done = E_NOMEM;
        परत _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    परत PYTOK_READ_ओळ;
}

पूर्णांक
_PyTok_ReaderIsInteractive(स्थिरांक संरचना tok_state *tok)
{
    परत tok->reader->kind == _PYTOK_READER_INTERACTIVE;
}

स्थिरांक अक्षर *
_PyTokenizer_RetainedSource(स्थिरांक संरचना tok_state *tok)
{
    जर (tok->reader->kind == _PYTOK_READER_PREPARED) {
        परत _PyTok_SourceData(&tok->source);
    }
    जर (tok->reader->kind == _PYTOK_READER_INTERACTIVE) {
        परत tok->source.bytes;
    }
    परत रिक्त;
}

शून्य
_PyTok_ReaderStopInteractive(संरचना tok_state *tok)
{
    जर (_PyTok_ReaderIsInteractive(tok)) {
        tok->reader->stop_interactive = 1;
    }
}

स्थिर _PyTok_ReadResult
reader_next(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    निवड (tok->reader->kind) {
        स्थिती _PYTOK_READER_PREPARED:
            परत next_prepared(tok, chunk);
        स्थिती _PYTOK_READER_FILE:
            परत next_file(tok, chunk);
        स्थिती _PYTOK_READER_READLINE:
            परत next_readline(tok, chunk);
        स्थिती _PYTOK_READER_INTERACTIVE:
            परत next_interactive(tok, chunk);
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
    tok->ओळ_start = tok->buf_offset;
}

पूर्णांक
_PyTok_ReaderUnderflow(संरचना tok_state *tok)
{
    प्रतिज्ञा(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    _PyTok_ReaderKind kind = reader->kind;
    पूर्णांक prepared = kind == _PYTOK_READER_PREPARED;
    पूर्णांक streaming = reader_is_streaming(kind);
    पूर्णांक reset_buffer = tok->start < 0 &&
        _PyLexer_CurrentFTString(tok) == रिक्त;

    _PyTok_Chunk chunk;
    _PyTok_ReadResult result = reader_next(tok, &chunk);
    जर (result != PYTOK_READ_ओळ) {
        जर (reset_buffer && streaming) {
            reset_streaming_buffer(tok);
        }
        जर (result == _PYTOK_READ_EOF) {
            tok->done = E_EOF;
        }
        अन्यथा जर (result == _PYTOK_READ_STOPPED) {
            tok->done = E_INTERACT_STOP;
        }
        अन्यथा जर (result == _PYTOK_READ_INTERRUPT) {
            tok->done = E_INTR;
        }
        अन्यथा {
            जर (tok->done == E_OK) {
                tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                    ? E_NOMEM : E_ERROR;
            }
        }
        जर (kind == _PYTOK_READER_INTERACTIVE &&
                result != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        परत 0;
    }
    जर (tok->lineno == पूर्णांक_उच्च) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        tok->done = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        परत 0;
    }

    जर (!prepared) {
        जर (streaming && reset_buffer) {
            reset_streaming_buffer(tok);
        }
        _PyTok_Off source_start = Py_Tok_Source_Append_ओळ(
            &tok->source, chunk.data, chunk.len);
        जर (source_start < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            परत 0;
        }
        जर (reset_buffer) {
            tok->cur = source_start;
            tok->buf_offset = source_start;
            tok->ओळ_start = tok->buf_offset;
        }
        tok->inp = source_start + chunk.len;
    }
    अन्यथा {
        जर (reset_buffer) {
            tok->buf_offset = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    जर (kind == _PYTOK_READER_FILE &&
            (reader->encoding == रिक्त ||
             म_भेद(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                _PyTok_SourcePointer(&tok->source, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        परत 0;
    }
    _PyTok_ChunkClear(&chunk);
    परत 1;
}

स्थिर संरचना tok_state *
tokenizer_new_with_reader(_PyTok_ReaderKind kind)
{
    संरचना tok_state *tok = PyMem_Calloc(1, आकार(*tok));
    जर (tok == रिक्त) {
        PyErr_NoMemory();
        परत रिक्त;
    }
    tok->start = tok->ओळ_start = -1;
    _PyTok_SourceInit(&tok->source);
    tok->done = E_OK;
    tok->layout.at_bol = 1;
    tok->start_loc = (_PyTok_Loc){-1, -1};
    tok->ftstring_चळत = tok->ftstring_चळत_inline;
    tok->ftstring_capacity = FTSTRING_चळत_INLINE_CAPACITY;
#ifdef Py_DEBUG
    tok->debug = _Py_GetConfig()->parser_debug;
#endif
    tok->reader = PyMem_Calloc(1, आकार(*tok->reader));
    जर (tok->reader == रिक्त) {
        PyErr_NoMemory();
        _PyTokenizer_Free(tok);
        परत रिक्त;
    }
    tok->reader->kind = kind;
    जर (kind == _PYTOK_READER_PREPARED) {
        परत tok;
    }
    जर (reader_is_streaming(kind)) {
        tok->cur = tok->inp = tok->source.base_offset;
        tok->ओळ_start = tok->buf_offset;
    }
    परत tok;
}

स्थिर संरचना tok_state *
tokenizer_from_string(स्थिरांक अक्षर *आदान, पूर्णांक utf8_only, पूर्णांक exec_आदान,
                      पूर्णांक preserve_crlf)
{
    संरचना tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    जर (tok == रिक्त) {
        परत रिक्त;
    }
    जर (_PyTok_PrepareString(
            tok, आदान, utf8_only, exec_आदान, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        परत रिक्त;
    }
    tok->cur = tok->inp = tok->source.base_offset;
    tok->ओळ_start = tok->source.base_offset;
    परत tok;
}

संरचना tok_state *
_PyTokenizer_FromString(स्थिरांक अक्षर *आदान, पूर्णांक exec_आदान, पूर्णांक preserve_crlf)
{
    परत tokenizer_from_string(आदान, 0, exec_आदान, preserve_crlf);
}

संरचना tok_state *
_PyTokenizer_FromUTF8(स्थिरांक अक्षर *आदान, पूर्णांक exec_आदान, पूर्णांक preserve_crlf)
{
    परत tokenizer_from_string(आदान, 1, exec_आदान, preserve_crlf);
}

संरचना tok_state *
_PyTokenizer_FromReadline(PyObject *readline, स्थिरांक अक्षर *encoding)
{
    संरचना tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    जर (tok == रिक्त) {
        परत रिक्त;
    }
    जर (encoding != रिक्त && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        परत रिक्त;
    }
    tok->reader->readline = Py_NewRef(readline);
    परत tok;
}

संरचना tok_state *
_PyTokenizer_FromFile(खाता *fp, स्थिरांक अक्षर *encoding,
                      स्थिरांक अक्षर *ps1, स्थिरांक अक्षर *ps2)
{
    _PyTok_ReaderKind kind = ps1 != रिक्त || ps2 != रिक्त
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    संरचना tok_state *tok = tokenizer_new_with_reader(kind);
    जर (tok == रिक्त) {
        परत रिक्त;
    }
    जर (encoding != रिक्त && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        परत रिक्त;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    परत tok;
}

#जर defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
प्रकारव्याख्या संघ {
    शून्य *cookie;
    पूर्णांक fd;
} borrowed_fd;

स्थिर ssize_t
borrow_read(शून्य *cookie, अक्षर *buffer, माप_प्रकार size)
{
    borrowed_fd borrowed = {.cookie = cookie};
    परत पठ(borrowed.fd, buffer, size);
}

स्थिर खाता *
fdopen_borrow(पूर्णांक fd)
{
    cookie_io_functions_t callbacks = {borrow_read, रिक्त, रिक्त, रिक्त};
    borrowed_fd borrowed = {.fd = fd};
    परत fopencookie(borrowed.cookie, "r", callbacks);
}
#अन्यथा
स्थिर खाता *
fdopen_borrow(पूर्णांक fd)
{
    पूर्णांक copy = _Py_dup(fd);
    जर (copy < 0) {
        परत रिक्त;
    }
    खाता *fp = fdopen(copy, "r");
    जर (fp == रिक्त) {
        पूर्णांक saved_errno = त्रुटि_सं;
        बंद(copy);
        त्रुटि_सं = saved_errno;
    }
    परत fp;
}
#endif

अक्षर *
_PyTokenizer_FindEncodingFilename(पूर्णांक fd, PyObject *filename)
{
    खाता *fp = fdopen_borrow(fd);
    जर (fp == रिक्त) {
        परत रिक्त;
    }
    संरचना tok_state *tok = _PyTokenizer_FromFile(fp, रिक्त, रिक्त, रिक्त);
    जर (tok == रिक्त) {
        ख_बंद(fp);
        परत रिक्त;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != रिक्त ? filename : &_Py_STR(anon_string));
    अक्षर *encoding = रिक्त;
    जर (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = रिक्त;
    }
    ख_बंद(fp);
    _PyTokenizer_Free(tok);
    परत encoding;
}
