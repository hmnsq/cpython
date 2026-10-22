#അന്തര്ഭാവ "Python.h"
#അന്തര്ഭാവ "pycore_fileutils.h"
#അന്തര്ഭാവ "pycore_pystate.h"
#അന്തര്ഭാവ "pycore_runtime.h"

#അന്തര്ഭാവ "errcode.h"
#അന്തര്ഭാവ "helpers.h"
#അന്തര്ഭാവ "reader.h"
#അന്തര്ഭാവ "reader_internal.h"
#അന്തര്ഭാവ "../lexer/state.h"

#define READER_BUFFER_GROWTH_FACTOR 2

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ
reader_is_streaming(_PyTok_ReaderKind kind)
{
    തിരികെ kind == _PYTOK_READER_FILE || kind == _PYTOK_READER_READLINE;
}

ശൂന്യം
_PyTok_ReaderFree(ഘടന tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    എങ്കിൽ (reader == ശൂന്യ) {
        തിരികെ;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0;
         i < (പൂർണ്ണസംഖ്യ)Py_ARRAY_LENGTH(reader->prefetched_വരികൾ); i++) {
        _PyTok_ChunkClear(&reader->prefetched_വരികൾ[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_buffer);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = ശൂന്യ;
}

പൂർണ്ണസംഖ്യ
_PyTokenizer_Traverse(ഘടന tok_state *tok, visitproc visit, ശൂന്യം *arg)
{
    Py_VISIT(tok->filename);
    Py_VISIT(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_VISIT(reader->readline);
    Py_VISIT(reader->decoder);
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0;
         i < (പൂർണ്ണസംഖ്യ)Py_ARRAY_LENGTH(reader->prefetched_വരികൾ); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_വരികൾ[i];
        എങ്കിൽ (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_VISIT(chunk->owner);
        }
    }
    തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
_PyTok_ReserveBuffer(അക്ഷരം **buffer, Py_ssize_t *capacity, Py_ssize_t needed,
                     Py_ssize_t initial_capacity)
{
    പ്രതിജ്ഞാ(initial_capacity > 0);
    എങ്കിൽ (needed <= *capacity) {
        തിരികെ 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *capacity > 0 ? *capacity : initial_capacity;
    ആകുമ്പോൾ (cap < needed) {
        എങ്കിൽ (cap > PY_SSIZE_T_MAX / READER_BUFFER_GROWTH_FACTOR) {
            cap = needed;
            നിർത്തുക;
        }
        cap *= READER_BUFFER_GROWTH_FACTOR;
    }
    അക്ഷരം *resized = PyMem_Realloc(*buffer, cap);
    എങ്കിൽ (resized == ശൂന്യ) {
        PyErr_NoMemory();
        തിരികെ -1;
    }
    *buffer = resized;
    *capacity = cap;
    തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
append_decoded(_PyTok_Reader *reader, സ്ഥിരം അക്ഷരം *data, Py_ssize_t len)
{
    എങ്കിൽ (len == 0) {
        തിരികെ 0;
    }
    എങ്കിൽ (reader->decoded_pos > 0) {
        Py_ssize_t remaining = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (size_t)remaining);
        reader->decoded_scan -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = remaining;
    }
    എങ്കിൽ (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            _PyTok_ReserveBuffer(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        PyErr_NoMemory();
        തിരികെ -1;
    }
    memcpy(reader->decoded + reader->decoded_len, data, (size_t)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
append_implicit_newline(_PyTok_Reader *reader)
{
    എങ്കിൽ (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        തിരികെ 0;
    }
    എങ്കിൽ (append_decoded(reader, "\n", 1) < 0) {
        തിരികെ -1;
    }
    reader->decoded_tail_is_implicit = 1;
    തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
എടുക്കുക_decoded_വരി(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    പ്രതിജ്ഞാ(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    പ്രതിജ്ഞാ(reader->decoded_scan >= reader->decoded_pos &&
           reader->decoded_scan <= reader->decoded_len);
    എങ്കിൽ (reader->decoded_pos == reader->decoded_len) {
        തിരികെ 0;
    }
    അക്ഷരം *start = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    അക്ഷരം *newline = memchr(reader->decoded + reader->decoded_scan, '\n',
                           reader->decoded_len - reader->decoded_scan);
    എങ്കിൽ (newline == ശൂന്യ) {
        reader->decoded_scan = reader->decoded_len;
        തിരികെ 0;
    }
    Py_ssize_t len = newline - start + 1;
    chunk->data = start;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_scan = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_tail_is_implicit;
    എങ്കിൽ (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_scan = 0;
        reader->decoded_tail_is_implicit = 0;
    }
    തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
chunk_is_വരി(സ്ഥിരം _PyTok_Chunk *chunk)
{
    എങ്കിൽ (chunk->len == 0 || chunk->data[chunk->len - 1] != '\n') {
        തിരികെ 0;
    }
    തിരികെ memchr(chunk->data, '\n', chunk->len - 1) == ശൂന്യ;
}

സ്ഥിതം _PyTok_ReadResult
next_prepared(ഘടന tok_state *tok, _PyTok_Chunk *chunk)
{
    എങ്കിൽ (tok->lineno >= tok->source.nlines) {
        തിരികെ _PYTOK_READ_EOF;
    }
    _PyTok_Span tail = {tok->inp, tok->source.base_offset + tok->source.len};
    _PyTok_Off newline = _PyTok_SourceFindByte(&tok->source, tail, '\n');
    _PyTok_Span വരി = {tail.start, newline >= 0 ? newline + 1 : tail.end};
    chunk->data = (അക്ഷരം *)_PyTok_SourceSpanView(
        &tok->source, വരി, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = വരി.end == tail.end
        ? tok->reader->prepared_final_newline : 0;
    തിരികെ PYTOK_READ_വരി;
}

സ്ഥിതം _PyTok_ReadResult
read_file_വരി(ഘടന tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    വേണ്ടി (;;) {
        എങ്കിൽ (len > PY_SSIZE_T_MAX - BUFSIZ) {
            PyErr_NoMemory();
            തിരികെ _PYTOK_READ_ERROR;
        }
        എങ്കിൽ (_PyTok_ReserveBuffer(&reader->file_buffer, &reader->file_buffer_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            തിരികെ _PYTOK_READ_ERROR;
        }
        പൂർണ്ണസംഖ്യ available = (പൂർണ്ണസംഖ്യ)Py_MIN(reader->file_buffer_cap - len, INT_MAX);
        size_t പഠ = 0;
        അക്ഷരം *result = _Py_UniversalNewlineFgetsWithSize(
            reader->file_buffer + len, available, reader->fp, ശൂന്യ, &പഠ);
        എങ്കിൽ (result == ശൂന്യ) {
            എങ്കിൽ (len == 0) {
                തിരികെ _PYTOK_READ_EOF;
            }
            നിർത്തുക;
        }
        len += (Py_ssize_t)പഠ;
        എങ്കിൽ (len > 0 && reader->file_buffer[len - 1] == '\n') {
            നിർത്തുക;
        }
    }
    പൂർണ്ണസംഖ്യ implicit = reader->file_buffer[len - 1] != '\n';
    chunk->data = reader->file_buffer;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    തിരികെ PYTOK_READ_വരി;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
initialize_file(ഘടന tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    എങ്കിൽ (reader->encoding != ശൂന്യ) {
        തിരികെ _PyTok_StartDecoder(tok, "strict");
    }

    _PyTok_ReadResult result = read_file_വരി(
        tok, &reader->prefetched_വരികൾ[0]);
    എങ്കിൽ (result == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        തിരികെ 0;
    }
    എങ്കിൽ (result != PYTOK_READ_വരി) {
        തിരികെ -1;
    }
    Py_ssize_t bom_len;
    _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_വരികൾ[0], ശൂന്യ, 0, &bom_len);
    എങ്കിൽ (detection == _PYTOK_ENCODING_ERROR) {
        തിരികെ -1;
    }
    എങ്കിൽ (detection == PYTOK_ENCODING_NEED_SECOND_വരി) {
        അക്ഷരം *first = _PyTok_CopyBytes(
            reader->prefetched_വരികൾ[0].data,
            reader->prefetched_വരികൾ[0].len);
        എങ്കിൽ (first == ശൂന്യ) {
            tok->done = E_NOMEM;
            തിരികെ -1;
        }
        reader->prefetched_വരികൾ[0].data = first;
        reader->prefetched_വരികൾ[0].ownership = _PYTOK_CHUNK_PYMEM;
        result = read_file_വരി(tok, &reader->prefetched_വരികൾ[1]);
        എങ്കിൽ (result == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        അല്ലെങ്കിൽ എങ്കിൽ (result != PYTOK_READ_വരി) {
            തിരികെ -1;
        }
        _PyTok_Chunk *second = reader->prefetched_വരികൾ[1].data != ശൂന്യ
            ? &reader->prefetched_വരികൾ[1] : ശൂന്യ;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_വരികൾ[0], second, 1, &bom_len);
        എങ്കിൽ (detection == _PYTOK_ENCODING_ERROR) {
            തിരികെ -1;
        }
    }
    എങ്കിൽ (bom_len != 0) {
        _PyTok_Chunk *first = &reader->prefetched_വരികൾ[0];
        എങ്കിൽ (first->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(first->data, first->data + bom_len,
                    (size_t)(first->len - bom_len));
            first->data[first->len - bom_len] = '\0';
        }
        അല്ലെങ്കിൽ {
            first->data += bom_len;
        }
        first->len -= bom_len;
    }
    എങ്കിൽ (_PyTok_StartDecoder(tok, "strict") < 0) {
        തിരികെ -1;
    }
    തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
finalize_decoding(ഘടന tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    എങ്കിൽ (reader->decoder_finalized) {
        തിരികെ 0;
    }
    reader->decoder_finalized = 1;
    എങ്കിൽ (reader->decoder != ശൂന്യ) {
        _PyTok_Chunk നിവേശം = {
            .data = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        പൂർണ്ണസംഖ്യ decoded = _PyTok_DecodeChunk(tok, &നിവേശം, 1);
        എങ്കിൽ (decoded == 0 &&
                append_decoded(reader, നിവേശം.data, നിവേശം.len) < 0) {
            tok->done = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&നിവേശം);
        എങ്കിൽ (decoded < 0) {
            തിരികെ -1;
        }
    }
    എങ്കിൽ (append_implicit_newline(reader) < 0) {
        tok->done = E_NOMEM;
        തിരികെ -1;
    }
    തിരികെ 0;
}

സ്ഥിതം _PyTok_ReadResult
next_file(ഘടന tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    എങ്കിൽ (!reader->file_initialized && initialize_file(tok) < 0) {
        തിരികെ _PYTOK_READ_ERROR;
    }
    വേണ്ടി (;;) {
        എങ്കിൽ (എടുക്കുക_decoded_വരി(reader, chunk)) {
            തിരികെ PYTOK_READ_വരി;
        }
        _PyTok_Chunk നിവേശം = {0};
        എങ്കിൽ (reader->prefetched_വരികൾ[0].data != ശൂന്യ) {
            നിവേശം = reader->prefetched_വരികൾ[0];
            reader->prefetched_വരികൾ[0] = (_PyTok_Chunk){0};
        }
        അല്ലെങ്കിൽ എങ്കിൽ (reader->prefetched_വരികൾ[1].data != ശൂന്യ) {
            നിവേശം = reader->prefetched_വരികൾ[1];
            reader->prefetched_വരികൾ[1] = (_PyTok_Chunk){0};
        }
        അല്ലെങ്കിൽ എങ്കിൽ (!reader->file_eof) {
            _PyTok_ReadResult result = read_file_വരി(tok, &നിവേശം);
            എങ്കിൽ (result == _PYTOK_READ_ERROR) {
                തിരികെ result;
            }
            എങ്കിൽ (result == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        എങ്കിൽ (നിവേശം.data != ശൂന്യ) {
            പൂർണ്ണസംഖ്യ implicit = നിവേശം.implicit_newline;
            എങ്കിൽ (reader->decoder == ശൂന്യ && !implicit) {
                *chunk = നിവേശം;
                തിരികെ PYTOK_READ_വരി;
            }
            പൂർണ്ണസംഖ്യ decoded = _PyTok_DecodeChunk(tok, &നിവേശം, 0);
            എങ്കിൽ (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_വരി(&നിവേശം)) {
                *chunk = നിവേശം;
                തിരികെ PYTOK_READ_വരി;
            }
            എങ്കിൽ (decoded == 0 &&
                    append_decoded(reader, നിവേശം.data, നിവേശം.len) < 0) {
                tok->done = E_NOMEM;
                decoded = -1;
            }
            എങ്കിൽ (decoded == 0 && implicit) {
                reader->decoded_tail_is_implicit = 1;
            }
            _PyTok_ChunkClear(&നിവേശം);
            എങ്കിൽ (decoded < 0) {
                തിരികെ _PYTOK_READ_ERROR;
            }
            തുടരുക;
        }
        എങ്കിൽ (!reader->decoder_finalized) {
            എങ്കിൽ (finalize_decoding(tok) < 0) {
                തിരികെ _PYTOK_READ_ERROR;
            }
            തുടരുക;
        }
        തിരികെ _PYTOK_READ_EOF;
    }
}

സ്ഥിതം _PyTok_ReadResult
next_readline(ഘടന tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    വേണ്ടി (;;) {
        എങ്കിൽ (എടുക്കുക_decoded_വരി(reader, chunk)) {
            തിരികെ PYTOK_READ_വരി;
        }
        എങ്കിൽ (reader->decoder_finalized) {
            തിരികെ _PYTOK_READ_EOF;
        }

        PyObject *raw = PyObject_CallNoArgs(reader->readline);
        എങ്കിൽ (raw == ശൂന്യ) {
            എങ്കിൽ (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                എങ്കിൽ (finalize_decoding(tok) < 0) {
                    തിരികെ _PYTOK_READ_ERROR;
                }
                തുടരുക;
            }
            തിരികെ _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk നിവേശം = {0};
        എങ്കിൽ (reader->encoding != ശൂന്യ) {
            എങ്കിൽ (!PyBytes_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                തിരികെ _PYTOK_READ_ERROR;
            }
            എങ്കിൽ (PyBytes_GET_SIZE(raw) == 0) {
                Py_DECREF(raw);
                എങ്കിൽ (_PyTok_StartDecoder(tok, "replace") < 0) {
                    തിരികെ _PYTOK_READ_ERROR;
                }
                എങ്കിൽ (finalize_decoding(tok) < 0) {
                    തിരികെ _PYTOK_READ_ERROR;
                }
                തുടരുക;
            }
            നിവേശം.owner = raw;
            നിവേശം.data = PyBytes_AS_STRING(raw);
            നിവേശം.len = PyBytes_GET_SIZE(raw);
            നിവേശം.ownership = _PYTOK_CHUNK_PYOBJECT;
            പൂർണ്ണസംഖ്യ decoded;
            എങ്കിൽ (reader->decoder == ശൂന്യ &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_വരി(&നിവേശം)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &നിവേശം, "utf-8", "replace");
            }
            അല്ലെങ്കിൽ {
                decoded = _PyTok_StartDecoder(tok, "replace");
                എങ്കിൽ (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &നിവേശം, 0);
                }
            }
            എങ്കിൽ (decoded < 0) {
                _PyTok_ChunkClear(&നിവേശം);
                തിരികെ _PYTOK_READ_ERROR;
            }
        }
        അല്ലെങ്കിൽ {
            എങ്കിൽ (!PyUnicode_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                തിരികെ _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            സ്ഥിരം അക്ഷരം *utf8 = PyUnicode_AsUTF8AndSize(raw, &utf8_len);
            എങ്കിൽ (utf8 == ശൂന്യ) {
                Py_DECREF(raw);
                തിരികെ _PYTOK_READ_ERROR;
            }
            നിവേശം.owner = raw;
            നിവേശം.data = (അക്ഷരം *)utf8;
            നിവേശം.len = utf8_len;
            നിവേശം.ownership = _PYTOK_CHUNK_PYOBJECT;
            എങ്കിൽ (നിവേശം.len == 0) {
                _PyTok_ChunkClear(&നിവേശം);
                എങ്കിൽ (finalize_decoding(tok) < 0) {
                    തിരികെ _PYTOK_READ_ERROR;
                }
                തുടരുക;
            }
        }

        എങ്കിൽ (reader->decoded_pos == reader->decoded_len &&
                chunk_is_വരി(&നിവേശം)) {
            *chunk = നിവേശം;
            തിരികെ PYTOK_READ_വരി;
        }

        എങ്കിൽ (append_decoded(reader, നിവേശം.data, നിവേശം.len) < 0) {
            _PyTok_ChunkClear(&നിവേശം);
            tok->done = E_NOMEM;
            തിരികെ _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&നിവേശം);
        എങ്കിൽ (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            പൂർണ്ണസംഖ്യ pending = Py_Tok_Decoder_Has_Buffered_നിവേശം(tok);
            എങ്കിൽ (pending < 0) {
                തിരികെ _PYTOK_READ_ERROR;
            }
            എങ്കിൽ (!pending && append_implicit_newline(reader) < 0) {
                tok->done = E_NOMEM;
                തിരികെ _PYTOK_READ_ERROR;
            }
        }
    }
}

സ്ഥിതം _PyTok_ReadResult
next_interactive(ഘടന tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    എങ്കിൽ (reader->stop_interactive) {
        തിരികെ _PYTOK_READ_STOPPED;
    }
    അക്ഷരം *നിവേശം = PyOS_Readline(
        reader->fp != ശൂന്യ ? reader->fp : stdin, stdout, reader->prompt);
    എങ്കിൽ (reader->nextprompt != ശൂന്യ) {
        reader->prompt = reader->nextprompt;
    }
    എങ്കിൽ (നിവേശം == ശൂന്യ) {
        തിരികെ _PYTOK_READ_INTERRUPT;
    }
    Py_ssize_t len = strlen(നിവേശം);
    എങ്കിൽ (len == 0) {
        PyMem_Free(നിവേശം);
        തിരികെ _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .data = നിവേശം,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    എങ്കിൽ (reader->encoding != ശൂന്യ &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, ശൂന്യ) < 0) {
        _PyTok_ChunkClear(&decoded);
        തിരികെ _PYTOK_READ_ERROR;
    }
    എങ്കിൽ (memchr(decoded.data, '\r', decoded.len) == ശൂന്യ) {
        *chunk = decoded;
        തിരികെ PYTOK_READ_വരി;
    }
    chunk->data = _PyTok_NormalizeNewlines(
        decoded.data, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    എങ്കിൽ (chunk->data == ശൂന്യ) {
        tok->done = E_NOMEM;
        തിരികെ _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    തിരികെ PYTOK_READ_വരി;
}

പൂർണ്ണസംഖ്യ
_PyTok_ReaderIsInteractive(സ്ഥിരം ഘടന tok_state *tok)
{
    തിരികെ tok->reader->kind == _PYTOK_READER_INTERACTIVE;
}

സ്ഥിരം അക്ഷരം *
_PyTokenizer_RetainedSource(സ്ഥിരം ഘടന tok_state *tok)
{
    എങ്കിൽ (tok->reader->kind == _PYTOK_READER_PREPARED) {
        തിരികെ _PyTok_SourceData(&tok->source);
    }
    എങ്കിൽ (tok->reader->kind == _PYTOK_READER_INTERACTIVE) {
        തിരികെ tok->source.bytes;
    }
    തിരികെ ശൂന്യ;
}

ശൂന്യം
_PyTok_ReaderStopInteractive(ഘടന tok_state *tok)
{
    എങ്കിൽ (_PyTok_ReaderIsInteractive(tok)) {
        tok->reader->stop_interactive = 1;
    }
}

സ്ഥിതം _PyTok_ReadResult
reader_next(ഘടന tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    തിരഞ്ഞെടുപ്പ് (tok->reader->kind) {
        അവസ്ഥ _PYTOK_READER_PREPARED:
            തിരികെ next_prepared(tok, chunk);
        അവസ്ഥ _PYTOK_READER_FILE:
            തിരികെ next_file(tok, chunk);
        അവസ്ഥ _PYTOK_READER_READLINE:
            തിരികെ next_readline(tok, chunk);
        അവസ്ഥ _PYTOK_READER_INTERACTIVE:
            തിരികെ next_interactive(tok, chunk);
    }
    Py_UNREACHABLE();
}

സ്ഥിതം ശൂന്യം
reset_streaming_buffer(ഘടന tok_state *tok)
{
    പ്രതിജ്ഞാ(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_SourceDiscard(&tok->source);
    tok->buf_offset = tok->source.base_offset;
    tok->cur = tok->inp = tok->source.base_offset;
    tok->വരി_start = tok->buf_offset;
}

പൂർണ്ണസംഖ്യ
_PyTok_ReaderUnderflow(ഘടന tok_state *tok)
{
    പ്രതിജ്ഞാ(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    _PyTok_ReaderKind kind = reader->kind;
    പൂർണ്ണസംഖ്യ prepared = kind == _PYTOK_READER_PREPARED;
    പൂർണ്ണസംഖ്യ streaming = reader_is_streaming(kind);
    പൂർണ്ണസംഖ്യ reset_buffer = tok->start < 0 &&
        _PyLexer_CurrentFTString(tok) == ശൂന്യ;

    _PyTok_Chunk chunk;
    _PyTok_ReadResult result = reader_next(tok, &chunk);
    എങ്കിൽ (result != PYTOK_READ_വരി) {
        എങ്കിൽ (reset_buffer && streaming) {
            reset_streaming_buffer(tok);
        }
        എങ്കിൽ (result == _PYTOK_READ_EOF) {
            tok->done = E_EOF;
        }
        അല്ലെങ്കിൽ എങ്കിൽ (result == _PYTOK_READ_STOPPED) {
            tok->done = E_INTERACT_STOP;
        }
        അല്ലെങ്കിൽ എങ്കിൽ (result == _PYTOK_READ_INTERRUPT) {
            tok->done = E_INTR;
        }
        അല്ലെങ്കിൽ {
            എങ്കിൽ (tok->done == E_OK) {
                tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                    ? E_NOMEM : E_ERROR;
            }
        }
        എങ്കിൽ (kind == _PYTOK_READER_INTERACTIVE &&
                result != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        തിരികെ 0;
    }
    എങ്കിൽ (tok->lineno == INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        tok->done = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        തിരികെ 0;
    }

    എങ്കിൽ (!prepared) {
        എങ്കിൽ (streaming && reset_buffer) {
            reset_streaming_buffer(tok);
        }
        _PyTok_Off source_start = Py_Tok_Source_Append_വരി(
            &tok->source, chunk.data, chunk.len);
        എങ്കിൽ (source_start < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            തിരികെ 0;
        }
        എങ്കിൽ (reset_buffer) {
            tok->cur = source_start;
            tok->buf_offset = source_start;
            tok->വരി_start = tok->buf_offset;
        }
        tok->inp = source_start + chunk.len;
    }
    അല്ലെങ്കിൽ {
        എങ്കിൽ (reset_buffer) {
            tok->buf_offset = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    എങ്കിൽ (kind == _PYTOK_READER_FILE &&
            (reader->encoding == ശൂന്യ ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                _PyTok_SourcePointer(&tok->source, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        തിരികെ 0;
    }
    _PyTok_ChunkClear(&chunk);
    തിരികെ 1;
}

സ്ഥിതം ഘടന tok_state *
tokenizer_new_with_reader(_PyTok_ReaderKind kind)
{
    ഘടന tok_state *tok = PyMem_Calloc(1, വലുപ്പം(*tok));
    എങ്കിൽ (tok == ശൂന്യ) {
        PyErr_NoMemory();
        തിരികെ ശൂന്യ;
    }
    tok->start = tok->വരി_start = -1;
    _PyTok_SourceInit(&tok->source);
    tok->done = E_OK;
    tok->layout.at_bol = 1;
    tok->start_loc = (_PyTok_Loc){-1, -1};
    tok->ftstring_അട്ടി = tok->ftstring_അട്ടി_inline;
    tok->ftstring_capacity = FTSTRING_അട്ടി_INLINE_CAPACITY;
#ifdef Py_DEBUG
    tok->debug = _Py_GetConfig()->parser_debug;
#endif
    tok->reader = PyMem_Calloc(1, വലുപ്പം(*tok->reader));
    എങ്കിൽ (tok->reader == ശൂന്യ) {
        PyErr_NoMemory();
        _PyTokenizer_Free(tok);
        തിരികെ ശൂന്യ;
    }
    tok->reader->kind = kind;
    എങ്കിൽ (kind == _PYTOK_READER_PREPARED) {
        തിരികെ tok;
    }
    എങ്കിൽ (reader_is_streaming(kind)) {
        tok->cur = tok->inp = tok->source.base_offset;
        tok->വരി_start = tok->buf_offset;
    }
    തിരികെ tok;
}

സ്ഥിതം ഘടന tok_state *
tokenizer_from_string(സ്ഥിരം അക്ഷരം *നിവേശം, പൂർണ്ണസംഖ്യ utf8_only, പൂർണ്ണസംഖ്യ exec_നിവേശം,
                      പൂർണ്ണസംഖ്യ preserve_crlf)
{
    ഘടന tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    എങ്കിൽ (tok == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (_PyTok_PrepareString(
            tok, നിവേശം, utf8_only, exec_നിവേശം, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        തിരികെ ശൂന്യ;
    }
    tok->cur = tok->inp = tok->source.base_offset;
    tok->വരി_start = tok->source.base_offset;
    തിരികെ tok;
}

ഘടന tok_state *
_PyTokenizer_FromString(സ്ഥിരം അക്ഷരം *നിവേശം, പൂർണ്ണസംഖ്യ exec_നിവേശം, പൂർണ്ണസംഖ്യ preserve_crlf)
{
    തിരികെ tokenizer_from_string(നിവേശം, 0, exec_നിവേശം, preserve_crlf);
}

ഘടന tok_state *
_PyTokenizer_FromUTF8(സ്ഥിരം അക്ഷരം *നിവേശം, പൂർണ്ണസംഖ്യ exec_നിവേശം, പൂർണ്ണസംഖ്യ preserve_crlf)
{
    തിരികെ tokenizer_from_string(നിവേശം, 1, exec_നിവേശം, preserve_crlf);
}

ഘടന tok_state *
_PyTokenizer_FromReadline(PyObject *readline, സ്ഥിരം അക്ഷരം *encoding)
{
    ഘടന tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    എങ്കിൽ (tok == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (encoding != ശൂന്യ && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        തിരികെ ശൂന്യ;
    }
    tok->reader->readline = Py_NewRef(readline);
    തിരികെ tok;
}

ഘടന tok_state *
_PyTokenizer_FromFile(FILE *fp, സ്ഥിരം അക്ഷരം *encoding,
                      സ്ഥിരം അക്ഷരം *ps1, സ്ഥിരം അക്ഷരം *ps2)
{
    _PyTok_ReaderKind kind = ps1 != ശൂന്യ || ps2 != ശൂന്യ
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    ഘടന tok_state *tok = tokenizer_new_with_reader(kind);
    എങ്കിൽ (tok == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (encoding != ശൂന്യ && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        തിരികെ ശൂന്യ;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    തിരികെ tok;
}

#എങ്കിൽ defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
തരനിർവചനം സംഘം {
    ശൂന്യം *cookie;
    പൂർണ്ണസംഖ്യ fd;
} borrowed_fd;

സ്ഥിതം ssize_t
borrow_read(ശൂന്യം *cookie, അക്ഷരം *buffer, size_t size)
{
    borrowed_fd borrowed = {.cookie = cookie};
    തിരികെ പഠ(borrowed.fd, buffer, size);
}

സ്ഥിതം FILE *
fdopen_borrow(പൂർണ്ണസംഖ്യ fd)
{
    cookie_io_functions_t callbacks = {borrow_read, ശൂന്യ, ശൂന്യ, ശൂന്യ};
    borrowed_fd borrowed = {.fd = fd};
    തിരികെ fopencookie(borrowed.cookie, "r", callbacks);
}
#അല്ലെങ്കിൽ
സ്ഥിതം FILE *
fdopen_borrow(പൂർണ്ണസംഖ്യ fd)
{
    പൂർണ്ണസംഖ്യ copy = _Py_dup(fd);
    എങ്കിൽ (copy < 0) {
        തിരികെ ശൂന്യ;
    }
    FILE *fp = fdopen(copy, "r");
    എങ്കിൽ (fp == ശൂന്യ) {
        പൂർണ്ണസംഖ്യ saved_errno = errno;
        close(copy);
        errno = saved_errno;
    }
    തിരികെ fp;
}
#endif

അക്ഷരം *
_PyTokenizer_FindEncodingFilename(പൂർണ്ണസംഖ്യ fd, PyObject *filename)
{
    FILE *fp = fdopen_borrow(fd);
    എങ്കിൽ (fp == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }
    ഘടന tok_state *tok = _PyTokenizer_FromFile(fp, ശൂന്യ, ശൂന്യ, ശൂന്യ);
    എങ്കിൽ (tok == ശൂന്യ) {
        fclose(fp);
        തിരികെ ശൂന്യ;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != ശൂന്യ ? filename : &_Py_STR(anon_string));
    അക്ഷരം *encoding = ശൂന്യ;
    എങ്കിൽ (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = ശൂന്യ;
    }
    fclose(fp);
    _PyTokenizer_Free(tok);
    തിരികെ encoding;
}
