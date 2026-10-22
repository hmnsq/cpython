#অন্তর্ভাব "Python.h"
#অন্তর্ভাব "pycore_fileutils.h"
#অন্তর্ভাব "pycore_pystate.h"
#অন্তর্ভাব "pycore_runtime.h"

#অন্তর্ভাব "errcode.h"
#অন্তর্ভাব "helpers.h"
#অন্তর্ভাব "reader.h"
#অন্তর্ভাব "reader_internal.h"
#অন্তর্ভাব "../lexer/state.h"

#define READER_বাফার_GROWTH_FACTOR 2

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

স্থির অন্তঃরেখ পূর্ণসংখ্যা
reader_is_streaming(_PyTok_ReaderKind kind)
{
    ফেরত kind == _PYTOK_READER_FILE || kind == _PYTOK_READER_READLINE;
}

শূন্য
_PyTok_ReaderFree(কাঠামো tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    যদি (reader == ফাঁকা) {
        ফেরত;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    জন্য (পূর্ণসংখ্যা i = 0;
         i < (পূর্ণসংখ্যা)Py_ARRAY_দৈর্ঘ্য(reader->prefetched_সারিগুলি); i++) {
        _PyTok_ChunkClear(&reader->prefetched_সারিগুলি[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_বাফার);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = ফাঁকা;
}

পূর্ণসংখ্যা
_PyTokenizer_Traverse(কাঠামো tok_state *tok, visitproc visit, শূন্য *arg)
{
    Py_VISIT(tok->filename);
    Py_VISIT(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_VISIT(reader->readline);
    Py_VISIT(reader->decoder);
    জন্য (পূর্ণসংখ্যা i = 0;
         i < (পূর্ণসংখ্যা)Py_ARRAY_দৈর্ঘ্য(reader->prefetched_সারিগুলি); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_সারিগুলি[i];
        যদি (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_VISIT(chunk->owner);
        }
    }
    ফেরত 0;
}

পূর্ণসংখ্যা
Py_Tok_Reserve_বাফার(অক্ষর **বাফার, Py_ssize_t *capacity, Py_ssize_t needed,
                     Py_ssize_t initial_capacity)
{
    প্রতিজ্ঞা(initial_capacity > 0);
    যদি (needed <= *capacity) {
        ফেরত 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *capacity > 0 ? *capacity : initial_capacity;
    যখন (cap < needed) {
        যদি (cap > PY_SSIZE_T_MAX / READER_বাফার_GROWTH_FACTOR) {
            cap = needed;
            বিরতি;
        }
        cap *= READER_বাফার_GROWTH_FACTOR;
    }
    অক্ষর *resized = PyMem_Realloc(*বাফার, cap);
    যদি (resized == ফাঁকা) {
        PyErr_NoMemory();
        ফেরত -1;
    }
    *বাফার = resized;
    *capacity = cap;
    ফেরত 0;
}

স্থির পূর্ণসংখ্যা
append_decoded(_PyTok_Reader *reader, ধ্রুবক অক্ষর *তথ্য, Py_ssize_t len)
{
    যদি (len == 0) {
        ফেরত 0;
    }
    যদি (reader->decoded_pos > 0) {
        Py_ssize_t remaining = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (আকার_t)remaining);
        reader->decoded_scan -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = remaining;
    }
    যদি (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            Py_Tok_Reserve_বাফার(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        PyErr_NoMemory();
        ফেরত -1;
    }
    memcpy(reader->decoded + reader->decoded_len, তথ্য, (আকার_t)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    ফেরত 0;
}

স্থির পূর্ণসংখ্যা
append_implicit_newline(_PyTok_Reader *reader)
{
    যদি (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        ফেরত 0;
    }
    যদি (append_decoded(reader, "\n", 1) < 0) {
        ফেরত -1;
    }
    reader->decoded_লেজ_is_implicit = 1;
    ফেরত 0;
}

স্থির পূর্ণসংখ্যা
বের_করো_decoded_সারি(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    প্রতিজ্ঞা(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    প্রতিজ্ঞা(reader->decoded_scan >= reader->decoded_pos &&
           reader->decoded_scan <= reader->decoded_len);
    যদি (reader->decoded_pos == reader->decoded_len) {
        ফেরত 0;
    }
    অক্ষর *start = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    অক্ষর *newline = memchr(reader->decoded + reader->decoded_scan, '\n',
                           reader->decoded_len - reader->decoded_scan);
    যদি (newline == ফাঁকা) {
        reader->decoded_scan = reader->decoded_len;
        ফেরত 0;
    }
    Py_ssize_t len = newline - start + 1;
    chunk->তথ্য = start;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_scan = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_লেজ_is_implicit;
    যদি (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_scan = 0;
        reader->decoded_লেজ_is_implicit = 0;
    }
    ফেরত 1;
}

স্থির পূর্ণসংখ্যা
chunk_is_সারি(ধ্রুবক _PyTok_Chunk *chunk)
{
    যদি (chunk->len == 0 || chunk->তথ্য[chunk->len - 1] != '\n') {
        ফেরত 0;
    }
    ফেরত memchr(chunk->তথ্য, '\n', chunk->len - 1) == ফাঁকা;
}

স্থির Py_Tok_Read_ফলাফল
পরবর্তী_prepared(কাঠামো tok_state *tok, _PyTok_Chunk *chunk)
{
    যদি (tok->lineno >= tok->source.nlines) {
        ফেরত _PYTOK_READ_EOF;
    }
    _PyTok_Span লেজ = {tok->inp, tok->source.base_offset + tok->source.len};
    _PyTok_Off newline = _PyTok_SourceFindByte(&tok->source, লেজ, '\n');
    _PyTok_Span সারি = {লেজ.start, newline >= 0 ? newline + 1 : লেজ.end};
    chunk->তথ্য = (অক্ষর *)_PyTok_SourceSpanView(
        &tok->source, সারি, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = সারি.end == লেজ.end
        ? tok->reader->prepared_final_newline : 0;
    ফেরত PYTOK_READ_সারি;
}

স্থির Py_Tok_Read_ফলাফল
read_file_সারি(কাঠামো tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    জন্য (;;) {
        যদি (len > PY_SSIZE_T_MAX - BUFSIZ) {
            PyErr_NoMemory();
            ফেরত _PYTOK_READ_ERROR;
        }
        যদি (Py_Tok_Reserve_বাফার(&reader->file_বাফার, &reader->file_বাফার_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            ফেরত _PYTOK_READ_ERROR;
        }
        পূর্ণসংখ্যা available = (পূর্ণসংখ্যা)Py_MIN(reader->file_বাফার_cap - len, INT_MAX);
        আকার_t পঠ = 0;
        অক্ষর *ফলাফল = Py_Universal_Newline_Fgets_With_আকার(
            reader->file_বাফার + len, available, reader->fp, ফাঁকা, &পঠ);
        যদি (ফলাফল == ফাঁকা) {
            যদি (len == 0) {
                ফেরত _PYTOK_READ_EOF;
            }
            বিরতি;
        }
        len += (Py_ssize_t)পঠ;
        যদি (len > 0 && reader->file_বাফার[len - 1] == '\n') {
            বিরতি;
        }
    }
    পূর্ণসংখ্যা implicit = reader->file_বাফার[len - 1] != '\n';
    chunk->তথ্য = reader->file_বাফার;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    ফেরত PYTOK_READ_সারি;
}

স্থির পূর্ণসংখ্যা
initialize_file(কাঠামো tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    যদি (reader->encoding != ফাঁকা) {
        ফেরত _PyTok_StartDecoder(tok, "strict");
    }

    Py_Tok_Read_ফলাফল ফলাফল = read_file_সারি(
        tok, &reader->prefetched_সারিগুলি[0]);
    যদি (ফলাফল == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        ফেরত 0;
    }
    যদি (ফলাফল != PYTOK_READ_সারি) {
        ফেরত -1;
    }
    Py_ssize_t bom_len;
    Py_Tok_Encoding_ফলাফল detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_সারিগুলি[0], ফাঁকা, 0, &bom_len);
    যদি (detection == _PYTOK_ENCODING_ERROR) {
        ফেরত -1;
    }
    যদি (detection == PYTOK_ENCODING_NEED_SECOND_সারি) {
        অক্ষর *first = _PyTok_CopyBytes(
            reader->prefetched_সারিগুলি[0].তথ্য,
            reader->prefetched_সারিগুলি[0].len);
        যদি (first == ফাঁকা) {
            tok->done = E_NOMEM;
            ফেরত -1;
        }
        reader->prefetched_সারিগুলি[0].তথ্য = first;
        reader->prefetched_সারিগুলি[0].ownership = _PYTOK_CHUNK_PYMEM;
        ফলাফল = read_file_সারি(tok, &reader->prefetched_সারিগুলি[1]);
        যদি (ফলাফল == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        অন্যথা যদি (ফলাফল != PYTOK_READ_সারি) {
            ফেরত -1;
        }
        _PyTok_Chunk *second = reader->prefetched_সারিগুলি[1].তথ্য != ফাঁকা
            ? &reader->prefetched_সারিগুলি[1] : ফাঁকা;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_সারিগুলি[0], second, 1, &bom_len);
        যদি (detection == _PYTOK_ENCODING_ERROR) {
            ফেরত -1;
        }
    }
    যদি (bom_len != 0) {
        _PyTok_Chunk *first = &reader->prefetched_সারিগুলি[0];
        যদি (first->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(first->তথ্য, first->তথ্য + bom_len,
                    (আকার_t)(first->len - bom_len));
            first->তথ্য[first->len - bom_len] = '\0';
        }
        অন্যথা {
            first->তথ্য += bom_len;
        }
        first->len -= bom_len;
    }
    যদি (_PyTok_StartDecoder(tok, "strict") < 0) {
        ফেরত -1;
    }
    ফেরত 0;
}

স্থির পূর্ণসংখ্যা
finalize_decoding(কাঠামো tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    যদি (reader->decoder_finalized) {
        ফেরত 0;
    }
    reader->decoder_finalized = 1;
    যদি (reader->decoder != ফাঁকা) {
        _PyTok_Chunk প্রবেশ = {
            .তথ্য = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        পূর্ণসংখ্যা decoded = _PyTok_DecodeChunk(tok, &প্রবেশ, 1);
        যদি (decoded == 0 &&
                append_decoded(reader, প্রবেশ.তথ্য, প্রবেশ.len) < 0) {
            tok->done = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&প্রবেশ);
        যদি (decoded < 0) {
            ফেরত -1;
        }
    }
    যদি (append_implicit_newline(reader) < 0) {
        tok->done = E_NOMEM;
        ফেরত -1;
    }
    ফেরত 0;
}

স্থির Py_Tok_Read_ফলাফল
পরবর্তী_file(কাঠামো tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    যদি (!reader->file_initialized && initialize_file(tok) < 0) {
        ফেরত _PYTOK_READ_ERROR;
    }
    জন্য (;;) {
        যদি (বের_করো_decoded_সারি(reader, chunk)) {
            ফেরত PYTOK_READ_সারি;
        }
        _PyTok_Chunk প্রবেশ = {0};
        যদি (reader->prefetched_সারিগুলি[0].তথ্য != ফাঁকা) {
            প্রবেশ = reader->prefetched_সারিগুলি[0];
            reader->prefetched_সারিগুলি[0] = (_PyTok_Chunk){0};
        }
        অন্যথা যদি (reader->prefetched_সারিগুলি[1].তথ্য != ফাঁকা) {
            প্রবেশ = reader->prefetched_সারিগুলি[1];
            reader->prefetched_সারিগুলি[1] = (_PyTok_Chunk){0};
        }
        অন্যথা যদি (!reader->file_eof) {
            Py_Tok_Read_ফলাফল ফলাফল = read_file_সারি(tok, &প্রবেশ);
            যদি (ফলাফল == _PYTOK_READ_ERROR) {
                ফেরত ফলাফল;
            }
            যদি (ফলাফল == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        যদি (প্রবেশ.তথ্য != ফাঁকা) {
            পূর্ণসংখ্যা implicit = প্রবেশ.implicit_newline;
            যদি (reader->decoder == ফাঁকা && !implicit) {
                *chunk = প্রবেশ;
                ফেরত PYTOK_READ_সারি;
            }
            পূর্ণসংখ্যা decoded = _PyTok_DecodeChunk(tok, &প্রবেশ, 0);
            যদি (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_সারি(&প্রবেশ)) {
                *chunk = প্রবেশ;
                ফেরত PYTOK_READ_সারি;
            }
            যদি (decoded == 0 &&
                    append_decoded(reader, প্রবেশ.তথ্য, প্রবেশ.len) < 0) {
                tok->done = E_NOMEM;
                decoded = -1;
            }
            যদি (decoded == 0 && implicit) {
                reader->decoded_লেজ_is_implicit = 1;
            }
            _PyTok_ChunkClear(&প্রবেশ);
            যদি (decoded < 0) {
                ফেরত _PYTOK_READ_ERROR;
            }
            চালু;
        }
        যদি (!reader->decoder_finalized) {
            যদি (finalize_decoding(tok) < 0) {
                ফেরত _PYTOK_READ_ERROR;
            }
            চালু;
        }
        ফেরত _PYTOK_READ_EOF;
    }
}

স্থির Py_Tok_Read_ফলাফল
পরবর্তী_readline(কাঠামো tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    জন্য (;;) {
        যদি (বের_করো_decoded_সারি(reader, chunk)) {
            ফেরত PYTOK_READ_সারি;
        }
        যদি (reader->decoder_finalized) {
            ফেরত _PYTOK_READ_EOF;
        }

        PyObject *raw = PyObject_CallNoArgs(reader->readline);
        যদি (raw == ফাঁকা) {
            যদি (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                যদি (finalize_decoding(tok) < 0) {
                    ফেরত _PYTOK_READ_ERROR;
                }
                চালু;
            }
            ফেরত _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk প্রবেশ = {0};
        যদি (reader->encoding != ফাঁকা) {
            যদি (!PyBytes_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                ফেরত _PYTOK_READ_ERROR;
            }
            যদি (Py_Bytes_GET_আকার(raw) == 0) {
                Py_DECREF(raw);
                যদি (_PyTok_StartDecoder(tok, "replace") < 0) {
                    ফেরত _PYTOK_READ_ERROR;
                }
                যদি (finalize_decoding(tok) < 0) {
                    ফেরত _PYTOK_READ_ERROR;
                }
                চালু;
            }
            প্রবেশ.owner = raw;
            প্রবেশ.তথ্য = PyBytes_AS_STRING(raw);
            প্রবেশ.len = Py_Bytes_GET_আকার(raw);
            প্রবেশ.ownership = _PYTOK_CHUNK_PYOBJECT;
            পূর্ণসংখ্যা decoded;
            যদি (reader->decoder == ফাঁকা &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_সারি(&প্রবেশ)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &প্রবেশ, "utf-8", "replace");
            }
            অন্যথা {
                decoded = _PyTok_StartDecoder(tok, "replace");
                যদি (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &প্রবেশ, 0);
                }
            }
            যদি (decoded < 0) {
                _PyTok_ChunkClear(&প্রবেশ);
                ফেরত _PYTOK_READ_ERROR;
            }
        }
        অন্যথা {
            যদি (!PyUnicode_Check(raw)) {
                PyErr_SetString(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                ফেরত _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            ধ্রুবক অক্ষর *utf8 = Py_Unicode_As_UTF8_And_আকার(raw, &utf8_len);
            যদি (utf8 == ফাঁকা) {
                Py_DECREF(raw);
                ফেরত _PYTOK_READ_ERROR;
            }
            প্রবেশ.owner = raw;
            প্রবেশ.তথ্য = (অক্ষর *)utf8;
            প্রবেশ.len = utf8_len;
            প্রবেশ.ownership = _PYTOK_CHUNK_PYOBJECT;
            যদি (প্রবেশ.len == 0) {
                _PyTok_ChunkClear(&প্রবেশ);
                যদি (finalize_decoding(tok) < 0) {
                    ফেরত _PYTOK_READ_ERROR;
                }
                চালু;
            }
        }

        যদি (reader->decoded_pos == reader->decoded_len &&
                chunk_is_সারি(&প্রবেশ)) {
            *chunk = প্রবেশ;
            ফেরত PYTOK_READ_সারি;
        }

        যদি (append_decoded(reader, প্রবেশ.তথ্য, প্রবেশ.len) < 0) {
            _PyTok_ChunkClear(&প্রবেশ);
            tok->done = E_NOMEM;
            ফেরত _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&প্রবেশ);
        যদি (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            পূর্ণসংখ্যা pending = Py_Tok_Decoder_Has_Buffered_প্রবেশ(tok);
            যদি (pending < 0) {
                ফেরত _PYTOK_READ_ERROR;
            }
            যদি (!pending && append_implicit_newline(reader) < 0) {
                tok->done = E_NOMEM;
                ফেরত _PYTOK_READ_ERROR;
            }
        }
    }
}

স্থির Py_Tok_Read_ফলাফল
পরবর্তী_interactive(কাঠামো tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    যদি (reader->stop_interactive) {
        ফেরত _PYTOK_READ_STOPPED;
    }
    অক্ষর *প্রবেশ = PyOS_Readline(
        reader->fp != ফাঁকা ? reader->fp : stdin, stdout, reader->prompt);
    যদি (reader->nextprompt != ফাঁকা) {
        reader->prompt = reader->nextprompt;
    }
    যদি (প্রবেশ == ফাঁকা) {
        ফেরত _PYTOK_READ_INTERRUPT;
    }
    Py_ssize_t len = strlen(প্রবেশ);
    যদি (len == 0) {
        PyMem_Free(প্রবেশ);
        ফেরত _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .তথ্য = প্রবেশ,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    যদি (reader->encoding != ফাঁকা &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, ফাঁকা) < 0) {
        _PyTok_ChunkClear(&decoded);
        ফেরত _PYTOK_READ_ERROR;
    }
    যদি (memchr(decoded.তথ্য, '\r', decoded.len) == ফাঁকা) {
        *chunk = decoded;
        ফেরত PYTOK_READ_সারি;
    }
    chunk->তথ্য = _PyTok_NormalizeNewlines(
        decoded.তথ্য, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    যদি (chunk->তথ্য == ফাঁকা) {
        tok->done = E_NOMEM;
        ফেরত _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    ফেরত PYTOK_READ_সারি;
}

পূর্ণসংখ্যা
_PyTok_ReaderIsInteractive(ধ্রুবক কাঠামো tok_state *tok)
{
    ফেরত tok->reader->kind == _PYTOK_READER_INTERACTIVE;
}

ধ্রুবক অক্ষর *
_PyTokenizer_RetainedSource(ধ্রুবক কাঠামো tok_state *tok)
{
    যদি (tok->reader->kind == _PYTOK_READER_PREPARED) {
        ফেরত Py_Tok_Source_তথ্য(&tok->source);
    }
    যদি (tok->reader->kind == _PYTOK_READER_INTERACTIVE) {
        ফেরত tok->source.bytes;
    }
    ফেরত ফাঁকা;
}

শূন্য
_PyTok_ReaderStopInteractive(কাঠামো tok_state *tok)
{
    যদি (_PyTok_ReaderIsInteractive(tok)) {
        tok->reader->stop_interactive = 1;
    }
}

স্থির Py_Tok_Read_ফলাফল
reader_পরবর্তী(কাঠামো tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    নির্বাচন (tok->reader->kind) {
        ক্ষেত্র _PYTOK_READER_PREPARED:
            ফেরত পরবর্তী_prepared(tok, chunk);
        ক্ষেত্র _PYTOK_READER_FILE:
            ফেরত পরবর্তী_file(tok, chunk);
        ক্ষেত্র _PYTOK_READER_READLINE:
            ফেরত পরবর্তী_readline(tok, chunk);
        ক্ষেত্র _PYTOK_READER_INTERACTIVE:
            ফেরত পরবর্তী_interactive(tok, chunk);
    }
    Py_UNREACHABLE();
}

স্থির শূন্য
reset_streaming_বাফার(কাঠামো tok_state *tok)
{
    প্রতিজ্ঞা(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_SourceDiscard(&tok->source);
    tok->buf_offset = tok->source.base_offset;
    tok->cur = tok->inp = tok->source.base_offset;
    tok->সারি_start = tok->buf_offset;
}

পূর্ণসংখ্যা
_PyTok_ReaderUnderflow(কাঠামো tok_state *tok)
{
    প্রতিজ্ঞা(tok->cur >= tok->buf_offset && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    _PyTok_ReaderKind kind = reader->kind;
    পূর্ণসংখ্যা prepared = kind == _PYTOK_READER_PREPARED;
    পূর্ণসংখ্যা streaming = reader_is_streaming(kind);
    পূর্ণসংখ্যা reset_বাফার = tok->start < 0 &&
        _PyLexer_CurrentFTString(tok) == ফাঁকা;

    _PyTok_Chunk chunk;
    Py_Tok_Read_ফলাফল ফলাফল = reader_পরবর্তী(tok, &chunk);
    যদি (ফলাফল != PYTOK_READ_সারি) {
        যদি (reset_বাফার && streaming) {
            reset_streaming_বাফার(tok);
        }
        যদি (ফলাফল == _PYTOK_READ_EOF) {
            tok->done = E_EOF;
        }
        অন্যথা যদি (ফলাফল == _PYTOK_READ_STOPPED) {
            tok->done = E_INTERACT_STOP;
        }
        অন্যথা যদি (ফলাফল == _PYTOK_READ_INTERRUPT) {
            tok->done = E_INTR;
        }
        অন্যথা {
            যদি (tok->done == E_OK) {
                tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                    ? E_NOMEM : E_ERROR;
            }
        }
        যদি (kind == _PYTOK_READER_INTERACTIVE &&
                ফলাফল != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        ফেরত 0;
    }
    যদি (tok->lineno == INT_MAX) {
        PyErr_SetString(PyExc_OverflowError, "too many tokenizer source lines");
        tok->done = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        ফেরত 0;
    }

    যদি (!prepared) {
        যদি (streaming && reset_বাফার) {
            reset_streaming_বাফার(tok);
        }
        _PyTok_Off source_start = Py_Tok_Source_Append_সারি(
            &tok->source, chunk.তথ্য, chunk.len);
        যদি (source_start < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            ফেরত 0;
        }
        যদি (reset_বাফার) {
            tok->cur = source_start;
            tok->buf_offset = source_start;
            tok->সারি_start = tok->buf_offset;
        }
        tok->inp = source_start + chunk.len;
    }
    অন্যথা {
        যদি (reset_বাফার) {
            tok->buf_offset = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    যদি (kind == _PYTOK_READER_FILE &&
            (reader->encoding == ফাঁকা ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                _PyTok_SourcePointer(&tok->source, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        ফেরত 0;
    }
    _PyTok_ChunkClear(&chunk);
    ফেরত 1;
}

স্থির কাঠামো tok_state *
tokenizer_new_with_reader(_PyTok_ReaderKind kind)
{
    কাঠামো tok_state *tok = PyMem_Calloc(1, আকার(*tok));
    যদি (tok == ফাঁকা) {
        PyErr_NoMemory();
        ফেরত ফাঁকা;
    }
    tok->start = tok->সারি_start = -1;
    _PyTok_SourceInit(&tok->source);
    tok->done = E_OK;
    tok->layout.at_bol = 1;
    tok->start_loc = (_PyTok_Loc){-1, -1};
    tok->ftstring_স্তূপ = tok->ftstring_স্তূপ_inline;
    tok->ftstring_capacity = FTSTRING_স্তূপ_INLINE_CAPACITY;
#ifdef Py_DEBUG
    tok->debug = _Py_GetConfig()->parser_debug;
#endif
    tok->reader = PyMem_Calloc(1, আকার(*tok->reader));
    যদি (tok->reader == ফাঁকা) {
        PyErr_NoMemory();
        _PyTokenizer_Free(tok);
        ফেরত ফাঁকা;
    }
    tok->reader->kind = kind;
    যদি (kind == _PYTOK_READER_PREPARED) {
        ফেরত tok;
    }
    যদি (reader_is_streaming(kind)) {
        tok->cur = tok->inp = tok->source.base_offset;
        tok->সারি_start = tok->buf_offset;
    }
    ফেরত tok;
}

স্থির কাঠামো tok_state *
tokenizer_from_string(ধ্রুবক অক্ষর *প্রবেশ, পূর্ণসংখ্যা utf8_only, পূর্ণসংখ্যা exec_প্রবেশ,
                      পূর্ণসংখ্যা preserve_crlf)
{
    কাঠামো tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    যদি (tok == ফাঁকা) {
        ফেরত ফাঁকা;
    }
    যদি (_PyTok_PrepareString(
            tok, প্রবেশ, utf8_only, exec_প্রবেশ, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        ফেরত ফাঁকা;
    }
    tok->cur = tok->inp = tok->source.base_offset;
    tok->সারি_start = tok->source.base_offset;
    ফেরত tok;
}

কাঠামো tok_state *
_PyTokenizer_FromString(ধ্রুবক অক্ষর *প্রবেশ, পূর্ণসংখ্যা exec_প্রবেশ, পূর্ণসংখ্যা preserve_crlf)
{
    ফেরত tokenizer_from_string(প্রবেশ, 0, exec_প্রবেশ, preserve_crlf);
}

কাঠামো tok_state *
_PyTokenizer_FromUTF8(ধ্রুবক অক্ষর *প্রবেশ, পূর্ণসংখ্যা exec_প্রবেশ, পূর্ণসংখ্যা preserve_crlf)
{
    ফেরত tokenizer_from_string(প্রবেশ, 1, exec_প্রবেশ, preserve_crlf);
}

কাঠামো tok_state *
_PyTokenizer_FromReadline(PyObject *readline, ধ্রুবক অক্ষর *encoding)
{
    কাঠামো tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    যদি (tok == ফাঁকা) {
        ফেরত ফাঁকা;
    }
    যদি (encoding != ফাঁকা && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        ফেরত ফাঁকা;
    }
    tok->reader->readline = Py_NewRef(readline);
    ফেরত tok;
}

কাঠামো tok_state *
_PyTokenizer_FromFile(FILE *fp, ধ্রুবক অক্ষর *encoding,
                      ধ্রুবক অক্ষর *ps1, ধ্রুবক অক্ষর *ps2)
{
    _PyTok_ReaderKind kind = ps1 != ফাঁকা || ps2 != ফাঁকা
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    কাঠামো tok_state *tok = tokenizer_new_with_reader(kind);
    যদি (tok == ফাঁকা) {
        ফেরত ফাঁকা;
    }
    যদি (encoding != ফাঁকা && _PyTok_SetEncoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        ফেরত ফাঁকা;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    ফেরত tok;
}

#যদি defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
প্রকারলেখ সংঘ {
    শূন্য *cookie;
    পূর্ণসংখ্যা fd;
} borrowed_fd;

স্থির ssize_t
borrow_read(শূন্য *cookie, অক্ষর *বাফার, আকার_t আকার_2)
{
    borrowed_fd borrowed = {.cookie = cookie};
    ফেরত পঠ(borrowed.fd, বাফার, আকার_2);
}

স্থির FILE *
fdopen_borrow(পূর্ণসংখ্যা fd)
{
    cookie_io_functions_t callbacks = {borrow_read, ফাঁকা, ফাঁকা, ফাঁকা};
    borrowed_fd borrowed = {.fd = fd};
    ফেরত fopencookie(borrowed.cookie, "r", callbacks);
}
#অন্যথা
স্থির FILE *
fdopen_borrow(পূর্ণসংখ্যা fd)
{
    পূর্ণসংখ্যা copy = _Py_dup(fd);
    যদি (copy < 0) {
        ফেরত ফাঁকা;
    }
    FILE *fp = fdopen(copy, "r");
    যদি (fp == ফাঁকা) {
        পূর্ণসংখ্যা saved_errno = errno;
        close(copy);
        errno = saved_errno;
    }
    ফেরত fp;
}
#endif

অক্ষর *
_PyTokenizer_FindEncodingFilename(পূর্ণসংখ্যা fd, PyObject *filename)
{
    FILE *fp = fdopen_borrow(fd);
    যদি (fp == ফাঁকা) {
        ফেরত ফাঁকা;
    }
    কাঠামো tok_state *tok = _PyTokenizer_FromFile(fp, ফাঁকা, ফাঁকা, ফাঁকা);
    যদি (tok == ফাঁকা) {
        fclose(fp);
        ফেরত ফাঁকা;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != ফাঁকা ? filename : &_Py_STR(anon_string));
    অক্ষর *encoding = ফাঁকা;
    যদি (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = ফাঁকা;
    }
    fclose(fp);
    _PyTokenizer_Free(tok);
    ফেরত encoding;
}
