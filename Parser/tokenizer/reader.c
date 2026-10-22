#ਅਨ੍ਤਰ੍ਭਾਵ "Python.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_fileutils.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_pystate.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_runtime.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "errcode.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "helpers.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "reader.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "reader_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "../lexer/state.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ READER_ਬਫਰ_GROWTH_ਗੁਣਕ 2

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_UNISTD_H
#  include <unistd.h>
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ
reader_is_streaming(Py_Tok_Reader_ਕਿਸਮ ਕਿਸਮ)
{
    ਵਾਪਸ ਕਿਸਮ == _PYTOK_READER_FILE || ਕਿਸਮ == _PYTOK_READER_READLINE;
}

ਖਾਲੀ
_PyTok_ReaderFree(ਬਣਤਰ tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    ਜੇ (reader == ਨਿਰਾ) {
        ਵਾਪਸ;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    ਲਈ (ਪੂਰਨ i = 0;
         i < (ਪੂਰਨ)Py_ARRAY_ਲੰਮਾਈ(reader->prefetched_ਸਤਰਾਂ); i++) {
        _PyTok_ChunkClear(&reader->prefetched_ਸਤਰਾਂ[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_ਬਫਰ);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = ਨਿਰਾ;
}

ਪੂਰਨ
_PyTokenizer_Traverse(ਬਣਤਰ tok_state *tok, visitproc ਦੇਖੋ, ਖਾਲੀ *arg)
{
    Py_ਦੇਖੋ(tok->filename);
    Py_ਦੇਖੋ(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_ਦੇਖੋ(reader->readline);
    Py_ਦੇਖੋ(reader->decoder);
    ਲਈ (ਪੂਰਨ i = 0;
         i < (ਪੂਰਨ)Py_ARRAY_ਲੰਮਾਈ(reader->prefetched_ਸਤਰਾਂ); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_ਸਤਰਾਂ[i];
        ਜੇ (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_ਦੇਖੋ(chunk->owner);
        }
    }
    ਵਾਪਸ 0;
}

ਪੂਰਨ
Py_Tok_Reserve_ਬਫਰ(ਅੱਖਰ **ਬਫਰ, Py_ssize_t *ਸਮਰੱਥਾ, Py_ssize_t needed,
                     Py_ssize_t initial_ਸਮਰੱਥਾ)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(initial_ਸਮਰੱਥਾ > 0);
    ਜੇ (needed <= *ਸਮਰੱਥਾ) {
        ਵਾਪਸ 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *ਸਮਰੱਥਾ > 0 ? *ਸਮਰੱਥਾ : initial_ਸਮਰੱਥਾ;
    ਜਦੋਂ (cap < needed) {
        ਜੇ (cap > PY_SSIZE_T_MAX / READER_ਬਫਰ_GROWTH_ਗੁਣਕ) {
            cap = needed;
            ਰੋਕੋ;
        }
        cap *= READER_ਬਫਰ_GROWTH_ਗੁਣਕ;
    }
    ਅੱਖਰ *resized = PyMem_Realloc(*ਬਫਰ, cap);
    ਜੇ (resized == ਨਿਰਾ) {
        Py_Err_No_ਸਮ੍ਰਿਤੀ();
        ਵਾਪਸ -1;
    }
    *ਬਫਰ = resized;
    *ਸਮਰੱਥਾ = cap;
    ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ
append_decoded(_PyTok_Reader *reader, ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ, Py_ssize_t len)
{
    ਜੇ (len == 0) {
        ਵਾਪਸ 0;
    }
    ਜੇ (reader->decoded_pos > 0) {
        Py_ssize_t ਬਚਿਆ = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (ਮਾਪ_t)ਬਚਿਆ);
        reader->decoded_ਜਾਂਚ -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = ਬਚਿਆ;
    }
    ਜੇ (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            Py_Tok_Reserve_ਬਫਰ(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        Py_Err_No_ਸਮ੍ਰਿਤੀ();
        ਵਾਪਸ -1;
    }
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(reader->decoded + reader->decoded_len, ਅੰਕੜੇ, (ਮਾਪ_t)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ
append_implicit_newline(_PyTok_Reader *reader)
{
    ਜੇ (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        ਵਾਪਸ 0;
    }
    ਜੇ (append_decoded(reader, "\n", 1) < 0) {
        ਵਾਪਸ -1;
    }
    reader->decoded_ਪੂਛ_is_implicit = 1;
    ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ
ਕੱਢੋ_decoded_ਸਤਰ(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    ਪ੍ਰਤਿਜ੍ਞਾ(reader->decoded_ਜਾਂਚ >= reader->decoded_pos &&
           reader->decoded_ਜਾਂਚ <= reader->decoded_len);
    ਜੇ (reader->decoded_pos == reader->decoded_len) {
        ਵਾਪਸ 0;
    }
    ਅੱਖਰ *ਸ਼ੁਰੂ = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    ਅੱਖਰ *newline = memchr(reader->decoded + reader->decoded_ਜਾਂਚ, '\n',
                           reader->decoded_len - reader->decoded_ਜਾਂਚ);
    ਜੇ (newline == ਨਿਰਾ) {
        reader->decoded_ਜਾਂਚ = reader->decoded_len;
        ਵਾਪਸ 0;
    }
    Py_ssize_t len = newline - ਸ਼ੁਰੂ + 1;
    chunk->ਅੰਕੜੇ = ਸ਼ੁਰੂ;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_ਜਾਂਚ = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_ਪੂਛ_is_implicit;
    ਜੇ (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_ਜਾਂਚ = 0;
        reader->decoded_ਪੂਛ_is_implicit = 0;
    }
    ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ
chunk_is_ਸਤਰ(ਅਚਲ _PyTok_Chunk *chunk)
{
    ਜੇ (chunk->len == 0 || chunk->ਅੰਕੜੇ[chunk->len - 1] != '\n') {
        ਵਾਪਸ 0;
    }
    ਵਾਪਸ memchr(chunk->ਅੰਕੜੇ, '\n', chunk->len - 1) == ਨਿਰਾ;
}

ਸਥਿਰ Py_Tok_Read_ਨਤੀਜਾ
ਅਗਲਾ_prepared(ਬਣਤਰ tok_state *tok, _PyTok_Chunk *chunk)
{
    ਜੇ (tok->lineno >= tok->ਸਰੋਤ.nlines) {
        ਵਾਪਸ _PYTOK_READ_EOF;
    }
    Py_Tok_ਵਿਸਤਾਰ ਪੂਛ = {tok->inp, tok->ਸਰੋਤ.ਅਧਾਰ_ਵਿੱਥ + tok->ਸਰੋਤ.len};
    _PyTok_Off newline = Py_Tok_ਸਰੋਤ_ਲੱਭੋ_Byte(&tok->ਸਰੋਤ, ਪੂਛ, '\n');
    Py_Tok_ਵਿਸਤਾਰ ਸਤਰ = {ਪੂਛ.ਸ਼ੁਰੂ, newline >= 0 ? newline + 1 : ਪੂਛ.ਸਿਰਾ};
    chunk->ਅੰਕੜੇ = (ਅੱਖਰ *)Py_Tok_ਸਰੋਤ_ਵਿਸਤਾਰ_View(
        &tok->ਸਰੋਤ, ਸਤਰ, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = ਸਤਰ.ਸਿਰਾ == ਪੂਛ.ਸਿਰਾ
        ? tok->reader->prepared_final_newline : 0;
    ਵਾਪਸ PYTOK_READ_ਸਤਰ;
}

ਸਥਿਰ Py_Tok_Read_ਨਤੀਜਾ
read_file_ਸਤਰ(ਬਣਤਰ tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    ਲਈ (;;) {
        ਜੇ (len > PY_SSIZE_T_MAX - BUFSIZ) {
            Py_Err_No_ਸਮ੍ਰਿਤੀ();
            ਵਾਪਸ _PYTOK_READ_ERROR;
        }
        ਜੇ (Py_Tok_Reserve_ਬਫਰ(&reader->file_ਬਫਰ, &reader->file_ਬਫਰ_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            ਵਾਪਸ _PYTOK_READ_ERROR;
        }
        ਪੂਰਨ ਉਪਲਬਧ = (ਪੂਰਨ)Py_MIN(reader->file_ਬਫਰ_cap - len, ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ);
        ਮਾਪ_t ਪਠ = 0;
        ਅੱਖਰ *ਨਤੀਜਾ = Py_Universal_Newline_Fgets_With_ਮਾਪ(
            reader->file_ਬਫਰ + len, ਉਪਲਬਧ, reader->fp, ਨਿਰਾ, &ਪਠ);
        ਜੇ (ਨਤੀਜਾ == ਨਿਰਾ) {
            ਜੇ (len == 0) {
                ਵਾਪਸ _PYTOK_READ_EOF;
            }
            ਰੋਕੋ;
        }
        len += (Py_ssize_t)ਪਠ;
        ਜੇ (len > 0 && reader->file_ਬਫਰ[len - 1] == '\n') {
            ਰੋਕੋ;
        }
    }
    ਪੂਰਨ implicit = reader->file_ਬਫਰ[len - 1] != '\n';
    chunk->ਅੰਕੜੇ = reader->file_ਬਫਰ;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    ਵਾਪਸ PYTOK_READ_ਸਤਰ;
}

ਸਥਿਰ ਪੂਰਨ
initialize_file(ਬਣਤਰ tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    ਜੇ (reader->encoding != ਨਿਰਾ) {
        ਵਾਪਸ Py_Tok_ਸ਼ੁਰੂ_Decoder(tok, "strict");
    }

    Py_Tok_Read_ਨਤੀਜਾ ਨਤੀਜਾ = read_file_ਸਤਰ(
        tok, &reader->prefetched_ਸਤਰਾਂ[0]);
    ਜੇ (ਨਤੀਜਾ == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        ਵਾਪਸ 0;
    }
    ਜੇ (ਨਤੀਜਾ != PYTOK_READ_ਸਤਰ) {
        ਵਾਪਸ -1;
    }
    Py_ssize_t bom_len;
    Py_Tok_Encoding_ਨਤੀਜਾ detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_ਸਤਰਾਂ[0], ਨਿਰਾ, 0, &bom_len);
    ਜੇ (detection == _PYTOK_ENCODING_ERROR) {
        ਵਾਪਸ -1;
    }
    ਜੇ (detection == PYTOK_ENCODING_NEED_ਦੂਜਾ_ਸਤਰ) {
        ਅੱਖਰ *ਪਹਿਲਾ = Py_Tok_Copy_ਬਾਈਟਾਂ(
            reader->prefetched_ਸਤਰਾਂ[0].ਅੰਕੜੇ,
            reader->prefetched_ਸਤਰਾਂ[0].len);
        ਜੇ (ਪਹਿਲਾ == ਨਿਰਾ) {
            tok->ਪੂਰਾ = E_NOMEM;
            ਵਾਪਸ -1;
        }
        reader->prefetched_ਸਤਰਾਂ[0].ਅੰਕੜੇ = ਪਹਿਲਾ;
        reader->prefetched_ਸਤਰਾਂ[0].ownership = _PYTOK_CHUNK_PYMEM;
        ਨਤੀਜਾ = read_file_ਸਤਰ(tok, &reader->prefetched_ਸਤਰਾਂ[1]);
        ਜੇ (ਨਤੀਜਾ == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        ਨਹੀਂਤਾਂ ਜੇ (ਨਤੀਜਾ != PYTOK_READ_ਸਤਰ) {
            ਵਾਪਸ -1;
        }
        _PyTok_Chunk *ਦੂਜਾ = reader->prefetched_ਸਤਰਾਂ[1].ਅੰਕੜੇ != ਨਿਰਾ
            ? &reader->prefetched_ਸਤਰਾਂ[1] : ਨਿਰਾ;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_ਸਤਰਾਂ[0], ਦੂਜਾ, 1, &bom_len);
        ਜੇ (detection == _PYTOK_ENCODING_ERROR) {
            ਵਾਪਸ -1;
        }
    }
    ਜੇ (bom_len != 0) {
        _PyTok_Chunk *ਪਹਿਲਾ = &reader->prefetched_ਸਤਰਾਂ[0];
        ਜੇ (ਪਹਿਲਾ->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(ਪਹਿਲਾ->ਅੰਕੜੇ, ਪਹਿਲਾ->ਅੰਕੜੇ + bom_len,
                    (ਮਾਪ_t)(ਪਹਿਲਾ->len - bom_len));
            ਪਹਿਲਾ->ਅੰਕੜੇ[ਪਹਿਲਾ->len - bom_len] = '\0';
        }
        ਨਹੀਂਤਾਂ {
            ਪਹਿਲਾ->ਅੰਕੜੇ += bom_len;
        }
        ਪਹਿਲਾ->len -= bom_len;
    }
    ਜੇ (Py_Tok_ਸ਼ੁਰੂ_Decoder(tok, "strict") < 0) {
        ਵਾਪਸ -1;
    }
    ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ
finalize_decoding(ਬਣਤਰ tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    ਜੇ (reader->decoder_finalized) {
        ਵਾਪਸ 0;
    }
    reader->decoder_finalized = 1;
    ਜੇ (reader->decoder != ਨਿਰਾ) {
        _PyTok_Chunk ਨਿਵੇਸ਼ = {
            .ਅੰਕੜੇ = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        ਪੂਰਨ decoded = _PyTok_DecodeChunk(tok, &ਨਿਵੇਸ਼, 1);
        ਜੇ (decoded == 0 &&
                append_decoded(reader, ਨਿਵੇਸ਼.ਅੰਕੜੇ, ਨਿਵੇਸ਼.len) < 0) {
            tok->ਪੂਰਾ = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&ਨਿਵੇਸ਼);
        ਜੇ (decoded < 0) {
            ਵਾਪਸ -1;
        }
    }
    ਜੇ (append_implicit_newline(reader) < 0) {
        tok->ਪੂਰਾ = E_NOMEM;
        ਵਾਪਸ -1;
    }
    ਵਾਪਸ 0;
}

ਸਥਿਰ Py_Tok_Read_ਨਤੀਜਾ
ਅਗਲਾ_file(ਬਣਤਰ tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    ਜੇ (!reader->file_initialized && initialize_file(tok) < 0) {
        ਵਾਪਸ _PYTOK_READ_ERROR;
    }
    ਲਈ (;;) {
        ਜੇ (ਕੱਢੋ_decoded_ਸਤਰ(reader, chunk)) {
            ਵਾਪਸ PYTOK_READ_ਸਤਰ;
        }
        _PyTok_Chunk ਨਿਵੇਸ਼ = {0};
        ਜੇ (reader->prefetched_ਸਤਰਾਂ[0].ਅੰਕੜੇ != ਨਿਰਾ) {
            ਨਿਵੇਸ਼ = reader->prefetched_ਸਤਰਾਂ[0];
            reader->prefetched_ਸਤਰਾਂ[0] = (_PyTok_Chunk){0};
        }
        ਨਹੀਂਤਾਂ ਜੇ (reader->prefetched_ਸਤਰਾਂ[1].ਅੰਕੜੇ != ਨਿਰਾ) {
            ਨਿਵੇਸ਼ = reader->prefetched_ਸਤਰਾਂ[1];
            reader->prefetched_ਸਤਰਾਂ[1] = (_PyTok_Chunk){0};
        }
        ਨਹੀਂਤਾਂ ਜੇ (!reader->file_eof) {
            Py_Tok_Read_ਨਤੀਜਾ ਨਤੀਜਾ = read_file_ਸਤਰ(tok, &ਨਿਵੇਸ਼);
            ਜੇ (ਨਤੀਜਾ == _PYTOK_READ_ERROR) {
                ਵਾਪਸ ਨਤੀਜਾ;
            }
            ਜੇ (ਨਤੀਜਾ == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        ਜੇ (ਨਿਵੇਸ਼.ਅੰਕੜੇ != ਨਿਰਾ) {
            ਪੂਰਨ implicit = ਨਿਵੇਸ਼.implicit_newline;
            ਜੇ (reader->decoder == ਨਿਰਾ && !implicit) {
                *chunk = ਨਿਵੇਸ਼;
                ਵਾਪਸ PYTOK_READ_ਸਤਰ;
            }
            ਪੂਰਨ decoded = _PyTok_DecodeChunk(tok, &ਨਿਵੇਸ਼, 0);
            ਜੇ (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_ਸਤਰ(&ਨਿਵੇਸ਼)) {
                *chunk = ਨਿਵੇਸ਼;
                ਵਾਪਸ PYTOK_READ_ਸਤਰ;
            }
            ਜੇ (decoded == 0 &&
                    append_decoded(reader, ਨਿਵੇਸ਼.ਅੰਕੜੇ, ਨਿਵੇਸ਼.len) < 0) {
                tok->ਪੂਰਾ = E_NOMEM;
                decoded = -1;
            }
            ਜੇ (decoded == 0 && implicit) {
                reader->decoded_ਪੂਛ_is_implicit = 1;
            }
            _PyTok_ChunkClear(&ਨਿਵੇਸ਼);
            ਜੇ (decoded < 0) {
                ਵਾਪਸ _PYTOK_READ_ERROR;
            }
            ਜਾਰੀ;
        }
        ਜੇ (!reader->decoder_finalized) {
            ਜੇ (finalize_decoding(tok) < 0) {
                ਵਾਪਸ _PYTOK_READ_ERROR;
            }
            ਜਾਰੀ;
        }
        ਵਾਪਸ _PYTOK_READ_EOF;
    }
}

ਸਥਿਰ Py_Tok_Read_ਨਤੀਜਾ
ਅਗਲਾ_readline(ਬਣਤਰ tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    ਲਈ (;;) {
        ਜੇ (ਕੱਢੋ_decoded_ਸਤਰ(reader, chunk)) {
            ਵਾਪਸ PYTOK_READ_ਸਤਰ;
        }
        ਜੇ (reader->decoder_finalized) {
            ਵਾਪਸ _PYTOK_READ_EOF;
        }

        PyObject *raw = Py_Object_Call_No_ਦਲੀਲਾਂ(reader->readline);
        ਜੇ (raw == ਨਿਰਾ) {
            ਜੇ (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                ਜੇ (finalize_decoding(tok) < 0) {
                    ਵਾਪਸ _PYTOK_READ_ERROR;
                }
                ਜਾਰੀ;
            }
            ਵਾਪਸ _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk ਨਿਵੇਸ਼ = {0};
        ਜੇ (reader->encoding != ਨਿਰਾ) {
            ਜੇ (!Py_ਬਾਈਟਾਂ_Check(raw)) {
                Py_Err_ਸੈੱਟ_String(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                ਵਾਪਸ _PYTOK_READ_ERROR;
            }
            ਜੇ (Py_ਬਾਈਟਾਂ_GET_ਮਾਪ(raw) == 0) {
                Py_DECREF(raw);
                ਜੇ (Py_Tok_ਸ਼ੁਰੂ_Decoder(tok, "replace") < 0) {
                    ਵਾਪਸ _PYTOK_READ_ERROR;
                }
                ਜੇ (finalize_decoding(tok) < 0) {
                    ਵਾਪਸ _PYTOK_READ_ERROR;
                }
                ਜਾਰੀ;
            }
            ਨਿਵੇਸ਼.owner = raw;
            ਨਿਵੇਸ਼.ਅੰਕੜੇ = Py_ਬਾਈਟਾਂ_AS_STRING(raw);
            ਨਿਵੇਸ਼.len = Py_ਬਾਈਟਾਂ_GET_ਮਾਪ(raw);
            ਨਿਵੇਸ਼.ownership = _PYTOK_CHUNK_PYOBJECT;
            ਪੂਰਨ decoded;
            ਜੇ (reader->decoder == ਨਿਰਾ &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_ਸਤਰ(&ਨਿਵੇਸ਼)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &ਨਿਵੇਸ਼, "utf-8", "replace");
            }
            ਨਹੀਂਤਾਂ {
                decoded = Py_Tok_ਸ਼ੁਰੂ_Decoder(tok, "replace");
                ਜੇ (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &ਨਿਵੇਸ਼, 0);
                }
            }
            ਜੇ (decoded < 0) {
                _PyTok_ChunkClear(&ਨਿਵੇਸ਼);
                ਵਾਪਸ _PYTOK_READ_ERROR;
            }
        }
        ਨਹੀਂਤਾਂ {
            ਜੇ (!PyUnicode_Check(raw)) {
                Py_Err_ਸੈੱਟ_String(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                ਵਾਪਸ _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            ਅਚਲ ਅੱਖਰ *utf8 = Py_Unicode_As_UTF8_And_ਮਾਪ(raw, &utf8_len);
            ਜੇ (utf8 == ਨਿਰਾ) {
                Py_DECREF(raw);
                ਵਾਪਸ _PYTOK_READ_ERROR;
            }
            ਨਿਵੇਸ਼.owner = raw;
            ਨਿਵੇਸ਼.ਅੰਕੜੇ = (ਅੱਖਰ *)utf8;
            ਨਿਵੇਸ਼.len = utf8_len;
            ਨਿਵੇਸ਼.ownership = _PYTOK_CHUNK_PYOBJECT;
            ਜੇ (ਨਿਵੇਸ਼.len == 0) {
                _PyTok_ChunkClear(&ਨਿਵੇਸ਼);
                ਜੇ (finalize_decoding(tok) < 0) {
                    ਵਾਪਸ _PYTOK_READ_ERROR;
                }
                ਜਾਰੀ;
            }
        }

        ਜੇ (reader->decoded_pos == reader->decoded_len &&
                chunk_is_ਸਤਰ(&ਨਿਵੇਸ਼)) {
            *chunk = ਨਿਵੇਸ਼;
            ਵਾਪਸ PYTOK_READ_ਸਤਰ;
        }

        ਜੇ (append_decoded(reader, ਨਿਵੇਸ਼.ਅੰਕੜੇ, ਨਿਵੇਸ਼.len) < 0) {
            _PyTok_ChunkClear(&ਨਿਵੇਸ਼);
            tok->ਪੂਰਾ = E_NOMEM;
            ਵਾਪਸ _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&ਨਿਵੇਸ਼);
        ਜੇ (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            ਪੂਰਨ pending = Py_Tok_Decoder_Has_Buffered_ਨਿਵੇਸ਼(tok);
            ਜੇ (pending < 0) {
                ਵਾਪਸ _PYTOK_READ_ERROR;
            }
            ਜੇ (!pending && append_implicit_newline(reader) < 0) {
                tok->ਪੂਰਾ = E_NOMEM;
                ਵਾਪਸ _PYTOK_READ_ERROR;
            }
        }
    }
}

ਸਥਿਰ Py_Tok_Read_ਨਤੀਜਾ
ਅਗਲਾ_interactive(ਬਣਤਰ tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    ਜੇ (reader->stop_interactive) {
        ਵਾਪਸ _PYTOK_READ_STOPPED;
    }
    ਅੱਖਰ *ਨਿਵੇਸ਼ = PyOS_Readline(
        reader->fp != ਨਿਰਾ ? reader->fp : stdin, stdout, reader->prompt);
    ਜੇ (reader->nextprompt != ਨਿਰਾ) {
        reader->prompt = reader->nextprompt;
    }
    ਜੇ (ਨਿਵੇਸ਼ == ਨਿਰਾ) {
        ਵਾਪਸ PYTOK_READ_ਰੁਕਾਵਟ;
    }
    Py_ssize_t len = ਲੜੀ_ਲੰਬਾਈ(ਨਿਵੇਸ਼);
    ਜੇ (len == 0) {
        PyMem_Free(ਨਿਵੇਸ਼);
        ਵਾਪਸ _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .ਅੰਕੜੇ = ਨਿਵੇਸ਼,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    ਜੇ (reader->encoding != ਨਿਰਾ &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, ਨਿਰਾ) < 0) {
        _PyTok_ChunkClear(&decoded);
        ਵਾਪਸ _PYTOK_READ_ERROR;
    }
    ਜੇ (memchr(decoded.ਅੰਕੜੇ, '\r', decoded.len) == ਨਿਰਾ) {
        *chunk = decoded;
        ਵਾਪਸ PYTOK_READ_ਸਤਰ;
    }
    chunk->ਅੰਕੜੇ = _PyTok_NormalizeNewlines(
        decoded.ਅੰਕੜੇ, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    ਜੇ (chunk->ਅੰਕੜੇ == ਨਿਰਾ) {
        tok->ਪੂਰਾ = E_NOMEM;
        ਵਾਪਸ _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    ਵਾਪਸ PYTOK_READ_ਸਤਰ;
}

ਪੂਰਨ
_PyTok_ReaderIsInteractive(ਅਚਲ ਬਣਤਰ tok_state *tok)
{
    ਵਾਪਸ tok->reader->ਕਿਸਮ == _PYTOK_READER_INTERACTIVE;
}

ਅਚਲ ਅੱਖਰ *
Py_Tokenizer_Retained_ਸਰੋਤ(ਅਚਲ ਬਣਤਰ tok_state *tok)
{
    ਜੇ (tok->reader->ਕਿਸਮ == _PYTOK_READER_PREPARED) {
        ਵਾਪਸ Py_Tok_ਸਰੋਤ_ਅੰਕੜੇ(&tok->ਸਰੋਤ);
    }
    ਜੇ (tok->reader->ਕਿਸਮ == _PYTOK_READER_INTERACTIVE) {
        ਵਾਪਸ tok->ਸਰੋਤ.ਬਾਈਟਾਂ;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

ਖਾਲੀ
_PyTok_ReaderStopInteractive(ਬਣਤਰ tok_state *tok)
{
    ਜੇ (_PyTok_ReaderIsInteractive(tok)) {
        tok->reader->stop_interactive = 1;
    }
}

ਸਥਿਰ Py_Tok_Read_ਨਤੀਜਾ
reader_ਅਗਲਾ(ਬਣਤਰ tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    ਚੋਣ (tok->reader->ਕਿਸਮ) {
        ਹਾਲਤ _PYTOK_READER_PREPARED:
            ਵਾਪਸ ਅਗਲਾ_prepared(tok, chunk);
        ਹਾਲਤ _PYTOK_READER_FILE:
            ਵਾਪਸ ਅਗਲਾ_file(tok, chunk);
        ਹਾਲਤ _PYTOK_READER_READLINE:
            ਵਾਪਸ ਅਗਲਾ_readline(tok, chunk);
        ਹਾਲਤ _PYTOK_READER_INTERACTIVE:
            ਵਾਪਸ ਅਗਲਾ_interactive(tok, chunk);
    }
    Py_UNREACHABLE();
}

ਸਥਿਰ ਖਾਲੀ
reset_streaming_ਬਫਰ(ਬਣਤਰ tok_state *tok)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(tok->cur >= tok->buf_ਵਿੱਥ && tok->cur <= tok->inp);
    Py_Tok_ਸਰੋਤ_Discard(&tok->ਸਰੋਤ);
    tok->buf_ਵਿੱਥ = tok->ਸਰੋਤ.ਅਧਾਰ_ਵਿੱਥ;
    tok->cur = tok->inp = tok->ਸਰੋਤ.ਅਧਾਰ_ਵਿੱਥ;
    tok->ਸਤਰ_ਸ਼ੁਰੂ = tok->buf_ਵਿੱਥ;
}

ਪੂਰਨ
_PyTok_ReaderUnderflow(ਬਣਤਰ tok_state *tok)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(tok->cur >= tok->buf_ਵਿੱਥ && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    Py_Tok_Reader_ਕਿਸਮ ਕਿਸਮ = reader->ਕਿਸਮ;
    ਪੂਰਨ prepared = ਕਿਸਮ == _PYTOK_READER_PREPARED;
    ਪੂਰਨ streaming = reader_is_streaming(ਕਿਸਮ);
    ਪੂਰਨ reset_ਬਫਰ = tok->ਸ਼ੁਰੂ < 0 &&
        _PyLexer_CurrentFTString(tok) == ਨਿਰਾ;

    _PyTok_Chunk chunk;
    Py_Tok_Read_ਨਤੀਜਾ ਨਤੀਜਾ = reader_ਅਗਲਾ(tok, &chunk);
    ਜੇ (ਨਤੀਜਾ != PYTOK_READ_ਸਤਰ) {
        ਜੇ (reset_ਬਫਰ && streaming) {
            reset_streaming_ਬਫਰ(tok);
        }
        ਜੇ (ਨਤੀਜਾ == _PYTOK_READ_EOF) {
            tok->ਪੂਰਾ = E_EOF;
        }
        ਨਹੀਂਤਾਂ ਜੇ (ਨਤੀਜਾ == _PYTOK_READ_STOPPED) {
            tok->ਪੂਰਾ = E_INTERACT_STOP;
        }
        ਨਹੀਂਤਾਂ ਜੇ (ਨਤੀਜਾ == PYTOK_READ_ਰੁਕਾਵਟ) {
            tok->ਪੂਰਾ = E_INTR;
        }
        ਨਹੀਂਤਾਂ {
            ਜੇ (tok->ਪੂਰਾ == E_OK) {
                tok->ਪੂਰਾ = PyErr_ExceptionMatches(Py_Exc_ਸਮ੍ਰਿਤੀ_Error)
                    ? E_NOMEM : E_ERROR;
            }
        }
        ਜੇ (ਕਿਸਮ == _PYTOK_READER_INTERACTIVE &&
                ਨਤੀਜਾ != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        ਵਾਪਸ 0;
    }
    ਜੇ (tok->lineno == ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) {
        Py_Err_ਸੈੱਟ_String(Py_Exc_ਉਛਾਲ_Error, "too many tokenizer source lines");
        tok->ਪੂਰਾ = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        ਵਾਪਸ 0;
    }

    ਜੇ (!prepared) {
        ਜੇ (streaming && reset_ਬਫਰ) {
            reset_streaming_ਬਫਰ(tok);
        }
        _PyTok_Off ਸਰੋਤ_ਸ਼ੁਰੂ = Py_Tok_ਸਰੋਤ_Append_ਸਤਰ(
            &tok->ਸਰੋਤ, chunk.ਅੰਕੜੇ, chunk.len);
        ਜੇ (ਸਰੋਤ_ਸ਼ੁਰੂ < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->ਪੂਰਾ = PyErr_ExceptionMatches(Py_Exc_ਸਮ੍ਰਿਤੀ_Error)
                ? E_NOMEM : E_ERROR;
            ਵਾਪਸ 0;
        }
        ਜੇ (reset_ਬਫਰ) {
            tok->cur = ਸਰੋਤ_ਸ਼ੁਰੂ;
            tok->buf_ਵਿੱਥ = ਸਰੋਤ_ਸ਼ੁਰੂ;
            tok->ਸਤਰ_ਸ਼ੁਰੂ = tok->buf_ਵਿੱਥ;
        }
        tok->inp = ਸਰੋਤ_ਸ਼ੁਰੂ + chunk.len;
    }
    ਨਹੀਂਤਾਂ {
        ਜੇ (reset_ਬਫਰ) {
            tok->buf_ਵਿੱਥ = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    ਜੇ (ਕਿਸਮ == _PYTOK_READER_FILE &&
            (reader->encoding == ਨਿਰਾ ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                Py_Tok_ਸਰੋਤ_ਸੂਚਕ(&tok->ਸਰੋਤ, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        ਵਾਪਸ 0;
    }
    _PyTok_ChunkClear(&chunk);
    ਵਾਪਸ 1;
}

ਸਥਿਰ ਬਣਤਰ tok_state *
tokenizer_new_with_reader(Py_Tok_Reader_ਕਿਸਮ ਕਿਸਮ)
{
    ਬਣਤਰ tok_state *tok = PyMem_Calloc(1, ਆਕਾਰ(*tok));
    ਜੇ (tok == ਨਿਰਾ) {
        Py_Err_No_ਸਮ੍ਰਿਤੀ();
        ਵਾਪਸ ਨਿਰਾ;
    }
    tok->ਸ਼ੁਰੂ = tok->ਸਤਰ_ਸ਼ੁਰੂ = -1;
    Py_Tok_ਸਰੋਤ_Init(&tok->ਸਰੋਤ);
    tok->ਪੂਰਾ = E_OK;
    tok->layout.at_bol = 1;
    tok->ਸ਼ੁਰੂ_loc = (_PyTok_Loc){-1, -1};
    tok->ftstring_ਢੇਰ = tok->ftstring_ਢੇਰ_inline;
    tok->ftstring_ਸਮਰੱਥਾ = FTSTRING_ਢੇਰ_INLINE_ਸਮਰੱਥਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ Py_DEBUG
    tok->debug = _Py_GetConfig()->parser_debug;
#ਜੇ_ਅੰਤ
    tok->reader = PyMem_Calloc(1, ਆਕਾਰ(*tok->reader));
    ਜੇ (tok->reader == ਨਿਰਾ) {
        Py_Err_No_ਸਮ੍ਰਿਤੀ();
        _PyTokenizer_Free(tok);
        ਵਾਪਸ ਨਿਰਾ;
    }
    tok->reader->ਕਿਸਮ = ਕਿਸਮ;
    ਜੇ (ਕਿਸਮ == _PYTOK_READER_PREPARED) {
        ਵਾਪਸ tok;
    }
    ਜੇ (reader_is_streaming(ਕਿਸਮ)) {
        tok->cur = tok->inp = tok->ਸਰੋਤ.ਅਧਾਰ_ਵਿੱਥ;
        tok->ਸਤਰ_ਸ਼ੁਰੂ = tok->buf_ਵਿੱਥ;
    }
    ਵਾਪਸ tok;
}

ਸਥਿਰ ਬਣਤਰ tok_state *
tokenizer_from_string(ਅਚਲ ਅੱਖਰ *ਨਿਵੇਸ਼, ਪੂਰਨ utf8_only, ਪੂਰਨ exec_ਨਿਵੇਸ਼,
                      ਪੂਰਨ preserve_crlf)
{
    ਬਣਤਰ tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    ਜੇ (tok == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (_PyTok_PrepareString(
            tok, ਨਿਵੇਸ਼, utf8_only, exec_ਨਿਵੇਸ਼, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        ਵਾਪਸ ਨਿਰਾ;
    }
    tok->cur = tok->inp = tok->ਸਰੋਤ.ਅਧਾਰ_ਵਿੱਥ;
    tok->ਸਤਰ_ਸ਼ੁਰੂ = tok->ਸਰੋਤ.ਅਧਾਰ_ਵਿੱਥ;
    ਵਾਪਸ tok;
}

ਬਣਤਰ tok_state *
_PyTokenizer_FromString(ਅਚਲ ਅੱਖਰ *ਨਿਵੇਸ਼, ਪੂਰਨ exec_ਨਿਵੇਸ਼, ਪੂਰਨ preserve_crlf)
{
    ਵਾਪਸ tokenizer_from_string(ਨਿਵੇਸ਼, 0, exec_ਨਿਵੇਸ਼, preserve_crlf);
}

ਬਣਤਰ tok_state *
_PyTokenizer_FromUTF8(ਅਚਲ ਅੱਖਰ *ਨਿਵੇਸ਼, ਪੂਰਨ exec_ਨਿਵੇਸ਼, ਪੂਰਨ preserve_crlf)
{
    ਵਾਪਸ tokenizer_from_string(ਨਿਵੇਸ਼, 1, exec_ਨਿਵੇਸ਼, preserve_crlf);
}

ਬਣਤਰ tok_state *
_PyTokenizer_FromReadline(PyObject *readline, ਅਚਲ ਅੱਖਰ *encoding)
{
    ਬਣਤਰ tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    ਜੇ (tok == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (encoding != ਨਿਰਾ && Py_Tok_ਸੈੱਟ_Encoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        ਵਾਪਸ ਨਿਰਾ;
    }
    tok->reader->readline = Py_NewRef(readline);
    ਵਾਪਸ tok;
}

ਬਣਤਰ tok_state *
_PyTokenizer_FromFile(FILE *fp, ਅਚਲ ਅੱਖਰ *encoding,
                      ਅਚਲ ਅੱਖਰ *ps1, ਅਚਲ ਅੱਖਰ *ps2)
{
    Py_Tok_Reader_ਕਿਸਮ ਕਿਸਮ = ps1 != ਨਿਰਾ || ps2 != ਨਿਰਾ
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    ਬਣਤਰ tok_state *tok = tokenizer_new_with_reader(ਕਿਸਮ);
    ਜੇ (tok == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (encoding != ਨਿਰਾ && Py_Tok_ਸੈੱਟ_Encoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        ਵਾਪਸ ਨਿਰਾ;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    ਵਾਪਸ tok;
}

#ਜੇ defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਸੰਘ {
    ਖਾਲੀ *cookie;
    ਪੂਰਨ fd;
} borrowed_fd;

ਸਥਿਰ ssize_t
borrow_read(ਖਾਲੀ *cookie, ਅੱਖਰ *ਬਫਰ, ਮਾਪ_t ਮਾਪ)
{
    borrowed_fd borrowed = {.cookie = cookie};
    ਵਾਪਸ ਪਠ(borrowed.fd, ਬਫਰ, ਮਾਪ);
}

ਸਥਿਰ FILE *
fdopen_borrow(ਪੂਰਨ fd)
{
    cookie_io_functions_t callbacks = {borrow_read, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ};
    borrowed_fd borrowed = {.fd = fd};
    ਵਾਪਸ fopencookie(borrowed.cookie, "r", callbacks);
}
#ਨਹੀਂਤਾਂ
ਸਥਿਰ FILE *
fdopen_borrow(ਪੂਰਨ fd)
{
    ਪੂਰਨ copy = _Py_dup(fd);
    ਜੇ (copy < 0) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    FILE *fp = fdopen(copy, "r");
    ਜੇ (fp == ਨਿਰਾ) {
        ਪੂਰਨ saved_errno = errno;
        close(copy);
        errno = saved_errno;
    }
    ਵਾਪਸ fp;
}
#ਜੇ_ਅੰਤ

ਅੱਖਰ *
Py_Tokenizer_ਲੱਭੋ_Encoding_Filename(ਪੂਰਨ fd, PyObject *filename)
{
    FILE *fp = fdopen_borrow(fd);
    ਜੇ (fp == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਬਣਤਰ tok_state *tok = _PyTokenizer_FromFile(fp, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
    ਜੇ (tok == ਨਿਰਾ) {
        fclose(fp);
        ਵਾਪਸ ਨਿਰਾ;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != ਨਿਰਾ ? filename : &_Py_STR(anon_string));
    ਅੱਖਰ *encoding = ਨਿਰਾ;
    ਜੇ (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = ਨਿਰਾ;
    }
    fclose(fp);
    _PyTokenizer_Free(tok);
    ਵਾਪਸ encoding;
}
