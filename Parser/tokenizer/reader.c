#ಅನ್ತರ್ಭಾವ "Python.h"
#ಅನ್ತರ್ಭಾವ "pycore_fileutils.h"
#ಅನ್ತರ್ಭಾವ "pycore_pystate.h"
#ಅನ್ತರ್ಭಾವ "pycore_runtime.h"

#ಅನ್ತರ್ಭಾವ "errcode.h"
#ಅನ್ತರ್ಭಾವ "helpers.h"
#ಅನ್ತರ್ಭಾವ "reader.h"
#ಅನ್ತರ್ಭಾವ "reader_internal.h"
#ಅನ್ತರ್ಭಾವ "../lexer/state.h"

#ವ್ಯಾಖ್ಯಾನಿಸು READER_ಬಫರ್_GROWTH_ಗುಣಕ 2

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_UNISTD_H
#  include <unistd.h>
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ
reader_is_streaming(Py_Tok_Reader_ಬಗೆ ಬಗೆ)
{
    ಹಿಂತಿರುಗಿ ಬಗೆ == _PYTOK_READER_FILE || ಬಗೆ == _PYTOK_READER_READLINE;
}

ಶೂನ್ಯ
_PyTok_ReaderFree(ರಚನೆ tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    ಆದರೆ (reader == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0;
         i < (ಪೂರ್ಣಾಂಕ)Py_ARRAY_ಉದ್ದಳತೆ(reader->prefetched_ಸಾಲುಗಳು); i++) {
        _PyTok_ChunkClear(&reader->prefetched_ಸಾಲುಗಳು[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_ಬಫರ್);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = ಖಾಲಿ;
}

ಪೂರ್ಣಾಂಕ
_PyTokenizer_Traverse(ರಚನೆ tok_state *tok, visitproc ಭೇಟಿ, ಶೂನ್ಯ *arg)
{
    Py_ಭೇಟಿ(tok->filename);
    Py_ಭೇಟಿ(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_ಭೇಟಿ(reader->readline);
    Py_ಭೇಟಿ(reader->decoder);
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0;
         i < (ಪೂರ್ಣಾಂಕ)Py_ARRAY_ಉದ್ದಳತೆ(reader->prefetched_ಸಾಲುಗಳು); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_ಸಾಲುಗಳು[i];
        ಆದರೆ (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_ಭೇಟಿ(chunk->owner);
        }
    }
    ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ
Py_Tok_Reserve_ಬಫರ್(ಅಕ್ಷರ **ಬಫರ್, Py_ssize_t *ಸಾಮರ್ಥ್ಯ, Py_ssize_t needed,
                     Py_ssize_t initial_ಸಾಮರ್ಥ್ಯ)
{
    ಪ್ರತಿಜ್ಞಾ(initial_ಸಾಮರ್ಥ್ಯ > 0);
    ಆದರೆ (needed <= *ಸಾಮರ್ಥ್ಯ) {
        ಹಿಂತಿರುಗಿ 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *ಸಾಮರ್ಥ್ಯ > 0 ? *ಸಾಮರ್ಥ್ಯ : initial_ಸಾಮರ್ಥ್ಯ;
    ಆಗ (cap < needed) {
        ಆದರೆ (cap > PY_SSIZE_T_MAX / READER_ಬಫರ್_GROWTH_ಗುಣಕ) {
            cap = needed;
            ನಿಲ್ಲಿಸು;
        }
        cap *= READER_ಬಫರ್_GROWTH_ಗುಣಕ;
    }
    ಅಕ್ಷರ *resized = PyMem_Realloc(*ಬಫರ್, cap);
    ಆದರೆ (resized == ಖಾಲಿ) {
        Py_Err_No_ಸ್ಮರಣೆ();
        ಹಿಂತಿರುಗಿ -1;
    }
    *ಬಫರ್ = resized;
    *ಸಾಮರ್ಥ್ಯ = cap;
    ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
append_decoded(_PyTok_Reader *reader, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ, Py_ssize_t len)
{
    ಆದರೆ (len == 0) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (reader->decoded_pos > 0) {
        Py_ssize_t ಉಳಿದ = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (ಗಾತ್ರ_t)ಉಳಿದ);
        reader->decoded_ಪರಿಶೀಲನೆ -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = ಉಳಿದ;
    }
    ಆದರೆ (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            Py_Tok_Reserve_ಬಫರ್(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        Py_Err_No_ಸ್ಮರಣೆ();
        ಹಿಂತಿರುಗಿ -1;
    }
    ಸ್ಮರಣೆ_ನಕಲು(reader->decoded + reader->decoded_len, ದತ್ತಾಂಶ, (ಗಾತ್ರ_t)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
append_implicit_newline(_PyTok_Reader *reader)
{
    ಆದರೆ (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (append_decoded(reader, "\n", 1) < 0) {
        ಹಿಂತಿರುಗಿ -1;
    }
    reader->decoded_ಬಾಲ_is_implicit = 1;
    ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ತೆಗೆ_decoded_ಸಾಲು(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    ಪ್ರತಿಜ್ಞಾ(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    ಪ್ರತಿಜ್ಞಾ(reader->decoded_ಪರಿಶೀಲನೆ >= reader->decoded_pos &&
           reader->decoded_ಪರಿಶೀಲನೆ <= reader->decoded_len);
    ಆದರೆ (reader->decoded_pos == reader->decoded_len) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಅಕ್ಷರ *ಆರಂಭ = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    ಅಕ್ಷರ *newline = memchr(reader->decoded + reader->decoded_ಪರಿಶೀಲನೆ, '\n',
                           reader->decoded_len - reader->decoded_ಪರಿಶೀಲನೆ);
    ಆದರೆ (newline == ಖಾಲಿ) {
        reader->decoded_ಪರಿಶೀಲನೆ = reader->decoded_len;
        ಹಿಂತಿರುಗಿ 0;
    }
    Py_ssize_t len = newline - ಆರಂಭ + 1;
    chunk->ದತ್ತಾಂಶ = ಆರಂಭ;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_ಪರಿಶೀಲನೆ = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_ಬಾಲ_is_implicit;
    ಆದರೆ (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_ಪರಿಶೀಲನೆ = 0;
        reader->decoded_ಬಾಲ_is_implicit = 0;
    }
    ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
chunk_is_ಸಾಲು(ಸ್ಥಿರಾಂಕ _PyTok_Chunk *chunk)
{
    ಆದರೆ (chunk->len == 0 || chunk->ದತ್ತಾಂಶ[chunk->len - 1] != '\n') {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ memchr(chunk->ದತ್ತಾಂಶ, '\n', chunk->len - 1) == ಖಾಲಿ;
}

ಸ್ಥಿರ Py_Tok_Read_ಫಲಿತಾಂಶ
ಮುಂದಿನ_prepared(ರಚನೆ tok_state *tok, _PyTok_Chunk *chunk)
{
    ಆದರೆ (tok->lineno >= tok->ಮೂಲ.nlines) {
        ಹಿಂತಿರುಗಿ _PYTOK_READ_EOF;
    }
    Py_Tok_ಹರವು ಬಾಲ = {tok->inp, tok->ಮೂಲ.ತಳ_ಸ್ಥಾನಾಂತರ + tok->ಮೂಲ.len};
    _PyTok_Off newline = Py_Tok_ಮೂಲ_ಕಂಡುಹಿಡಿ_Byte(&tok->ಮೂಲ, ಬಾಲ, '\n');
    Py_Tok_ಹರವು ಸಾಲು = {ಬಾಲ.ಆರಂಭ, newline >= 0 ? newline + 1 : ಬಾಲ.ಕೊನೆ};
    chunk->ದತ್ತಾಂಶ = (ಅಕ್ಷರ *)Py_Tok_ಮೂಲ_ಹರವು_View(
        &tok->ಮೂಲ, ಸಾಲು, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = ಸಾಲು.ಕೊನೆ == ಬಾಲ.ಕೊನೆ
        ? tok->reader->prepared_final_newline : 0;
    ಹಿಂತಿರುಗಿ PYTOK_READ_ಸಾಲು;
}

ಸ್ಥಿರ Py_Tok_Read_ಫಲಿತಾಂಶ
read_file_ಸಾಲು(ರಚನೆ tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    ಗಾಗಿ (;;) {
        ಆದರೆ (len > PY_SSIZE_T_MAX - BUFSIZ) {
            Py_Err_No_ಸ್ಮರಣೆ();
            ಹಿಂತಿರುಗಿ _PYTOK_READ_ERROR;
        }
        ಆದರೆ (Py_Tok_Reserve_ಬಫರ್(&reader->file_ಬಫರ್, &reader->file_ಬಫರ್_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            ಹಿಂತಿರುಗಿ _PYTOK_READ_ERROR;
        }
        ಪೂರ್ಣಾಂಕ ಲಭ್ಯ = (ಪೂರ್ಣಾಂಕ)Py_MIN(reader->file_ಬಫರ್_cap - len, ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ);
        ಗಾತ್ರ_t ಪಠ = 0;
        ಅಕ್ಷರ *ಫಲಿತಾಂಶ = Py_Universal_Newline_Fgets_With_ಗಾತ್ರ(
            reader->file_ಬಫರ್ + len, ಲಭ್ಯ, reader->fp, ಖಾಲಿ, &ಪಠ);
        ಆದರೆ (ಫಲಿತಾಂಶ == ಖಾಲಿ) {
            ಆದರೆ (len == 0) {
                ಹಿಂತಿರುಗಿ _PYTOK_READ_EOF;
            }
            ನಿಲ್ಲಿಸು;
        }
        len += (Py_ssize_t)ಪಠ;
        ಆದರೆ (len > 0 && reader->file_ಬಫರ್[len - 1] == '\n') {
            ನಿಲ್ಲಿಸು;
        }
    }
    ಪೂರ್ಣಾಂಕ implicit = reader->file_ಬಫರ್[len - 1] != '\n';
    chunk->ದತ್ತಾಂಶ = reader->file_ಬಫರ್;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    ಹಿಂತಿರುಗಿ PYTOK_READ_ಸಾಲು;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
initialize_file(ರಚನೆ tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    ಆದರೆ (reader->encoding != ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ Py_Tok_ಆರಂಭ_Decoder(tok, "strict");
    }

    Py_Tok_Read_ಫಲಿತಾಂಶ ಫಲಿತಾಂಶ = read_file_ಸಾಲು(
        tok, &reader->prefetched_ಸಾಲುಗಳು[0]);
    ಆದರೆ (ಫಲಿತಾಂಶ == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (ಫಲಿತಾಂಶ != PYTOK_READ_ಸಾಲು) {
        ಹಿಂತಿರುಗಿ -1;
    }
    Py_ssize_t bom_len;
    Py_Tok_Encoding_ಫಲಿತಾಂಶ detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_ಸಾಲುಗಳು[0], ಖಾಲಿ, 0, &bom_len);
    ಆದರೆ (detection == _PYTOK_ENCODING_ERROR) {
        ಹಿಂತಿರುಗಿ -1;
    }
    ಆದರೆ (detection == PYTOK_ENCODING_NEED_ಎರಡನೇ_ಸಾಲು) {
        ಅಕ್ಷರ *ಮೊದಲ = Py_Tok_Copy_ಬೈಟುಗಳು(
            reader->prefetched_ಸಾಲುಗಳು[0].ದತ್ತಾಂಶ,
            reader->prefetched_ಸಾಲುಗಳು[0].len);
        ಆದರೆ (ಮೊದಲ == ಖಾಲಿ) {
            tok->ಮುಗಿದ = E_NOMEM;
            ಹಿಂತಿರುಗಿ -1;
        }
        reader->prefetched_ಸಾಲುಗಳು[0].ದತ್ತಾಂಶ = ಮೊದಲ;
        reader->prefetched_ಸಾಲುಗಳು[0].ownership = _PYTOK_CHUNK_PYMEM;
        ಫಲಿತಾಂಶ = read_file_ಸಾಲು(tok, &reader->prefetched_ಸಾಲುಗಳು[1]);
        ಆದರೆ (ಫಲಿತಾಂಶ == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಫಲಿತಾಂಶ != PYTOK_READ_ಸಾಲು) {
            ಹಿಂತಿರುಗಿ -1;
        }
        _PyTok_Chunk *ಎರಡನೇ = reader->prefetched_ಸಾಲುಗಳು[1].ದತ್ತಾಂಶ != ಖಾಲಿ
            ? &reader->prefetched_ಸಾಲುಗಳು[1] : ಖಾಲಿ;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_ಸಾಲುಗಳು[0], ಎರಡನೇ, 1, &bom_len);
        ಆದರೆ (detection == _PYTOK_ENCODING_ERROR) {
            ಹಿಂತಿರುಗಿ -1;
        }
    }
    ಆದರೆ (bom_len != 0) {
        _PyTok_Chunk *ಮೊದಲ = &reader->prefetched_ಸಾಲುಗಳು[0];
        ಆದರೆ (ಮೊದಲ->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(ಮೊದಲ->ದತ್ತಾಂಶ, ಮೊದಲ->ದತ್ತಾಂಶ + bom_len,
                    (ಗಾತ್ರ_t)(ಮೊದಲ->len - bom_len));
            ಮೊದಲ->ದತ್ತಾಂಶ[ಮೊದಲ->len - bom_len] = '\0';
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            ಮೊದಲ->ದತ್ತಾಂಶ += bom_len;
        }
        ಮೊದಲ->len -= bom_len;
    }
    ಆದರೆ (Py_Tok_ಆರಂಭ_Decoder(tok, "strict") < 0) {
        ಹಿಂತಿರುಗಿ -1;
    }
    ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
finalize_decoding(ರಚನೆ tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    ಆದರೆ (reader->decoder_finalized) {
        ಹಿಂತಿರುಗಿ 0;
    }
    reader->decoder_finalized = 1;
    ಆದರೆ (reader->decoder != ಖಾಲಿ) {
        _PyTok_Chunk ಒಳಹರಿವು = {
            .ದತ್ತಾಂಶ = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        ಪೂರ್ಣಾಂಕ decoded = _PyTok_DecodeChunk(tok, &ಒಳಹರಿವು, 1);
        ಆದರೆ (decoded == 0 &&
                append_decoded(reader, ಒಳಹರಿವು.ದತ್ತಾಂಶ, ಒಳಹರಿವು.len) < 0) {
            tok->ಮುಗಿದ = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&ಒಳಹರಿವು);
        ಆದರೆ (decoded < 0) {
            ಹಿಂತಿರುಗಿ -1;
        }
    }
    ಆದರೆ (append_implicit_newline(reader) < 0) {
        tok->ಮುಗಿದ = E_NOMEM;
        ಹಿಂತಿರುಗಿ -1;
    }
    ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ Py_Tok_Read_ಫಲಿತಾಂಶ
ಮುಂದಿನ_file(ರಚನೆ tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    ಆದರೆ (!reader->file_initialized && initialize_file(tok) < 0) {
        ಹಿಂತಿರುಗಿ _PYTOK_READ_ERROR;
    }
    ಗಾಗಿ (;;) {
        ಆದರೆ (ತೆಗೆ_decoded_ಸಾಲು(reader, chunk)) {
            ಹಿಂತಿರುಗಿ PYTOK_READ_ಸಾಲು;
        }
        _PyTok_Chunk ಒಳಹರಿವು = {0};
        ಆದರೆ (reader->prefetched_ಸಾಲುಗಳು[0].ದತ್ತಾಂಶ != ಖಾಲಿ) {
            ಒಳಹರಿವು = reader->prefetched_ಸಾಲುಗಳು[0];
            reader->prefetched_ಸಾಲುಗಳು[0] = (_PyTok_Chunk){0};
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (reader->prefetched_ಸಾಲುಗಳು[1].ದತ್ತಾಂಶ != ಖಾಲಿ) {
            ಒಳಹರಿವು = reader->prefetched_ಸಾಲುಗಳು[1];
            reader->prefetched_ಸಾಲುಗಳು[1] = (_PyTok_Chunk){0};
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!reader->file_eof) {
            Py_Tok_Read_ಫಲಿತಾಂಶ ಫಲಿತಾಂಶ = read_file_ಸಾಲು(tok, &ಒಳಹರಿವು);
            ಆದರೆ (ಫಲಿತಾಂಶ == _PYTOK_READ_ERROR) {
                ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
            }
            ಆದರೆ (ಫಲಿತಾಂಶ == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        ಆದರೆ (ಒಳಹರಿವು.ದತ್ತಾಂಶ != ಖಾಲಿ) {
            ಪೂರ್ಣಾಂಕ implicit = ಒಳಹರಿವು.implicit_newline;
            ಆದರೆ (reader->decoder == ಖಾಲಿ && !implicit) {
                *chunk = ಒಳಹರಿವು;
                ಹಿಂತಿರುಗಿ PYTOK_READ_ಸಾಲು;
            }
            ಪೂರ್ಣಾಂಕ decoded = _PyTok_DecodeChunk(tok, &ಒಳಹರಿವು, 0);
            ಆದರೆ (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_ಸಾಲು(&ಒಳಹರಿವು)) {
                *chunk = ಒಳಹರಿವು;
                ಹಿಂತಿರುಗಿ PYTOK_READ_ಸಾಲು;
            }
            ಆದರೆ (decoded == 0 &&
                    append_decoded(reader, ಒಳಹರಿವು.ದತ್ತಾಂಶ, ಒಳಹರಿವು.len) < 0) {
                tok->ಮುಗಿದ = E_NOMEM;
                decoded = -1;
            }
            ಆದರೆ (decoded == 0 && implicit) {
                reader->decoded_ಬಾಲ_is_implicit = 1;
            }
            _PyTok_ChunkClear(&ಒಳಹರಿವು);
            ಆದರೆ (decoded < 0) {
                ಹಿಂತಿರುಗಿ _PYTOK_READ_ERROR;
            }
            ಮುಂದುವರಿ;
        }
        ಆದರೆ (!reader->decoder_finalized) {
            ಆದರೆ (finalize_decoding(tok) < 0) {
                ಹಿಂತಿರುಗಿ _PYTOK_READ_ERROR;
            }
            ಮುಂದುವರಿ;
        }
        ಹಿಂತಿರುಗಿ _PYTOK_READ_EOF;
    }
}

ಸ್ಥಿರ Py_Tok_Read_ಫಲಿತಾಂಶ
ಮುಂದಿನ_readline(ರಚನೆ tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    ಗಾಗಿ (;;) {
        ಆದರೆ (ತೆಗೆ_decoded_ಸಾಲು(reader, chunk)) {
            ಹಿಂತಿರುಗಿ PYTOK_READ_ಸಾಲು;
        }
        ಆದರೆ (reader->decoder_finalized) {
            ಹಿಂತಿರುಗಿ _PYTOK_READ_EOF;
        }

        PyObject *raw = Py_Object_Call_No_ವಾದಗಳು(reader->readline);
        ಆದರೆ (raw == ಖಾಲಿ) {
            ಆದರೆ (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                ಆದರೆ (finalize_decoding(tok) < 0) {
                    ಹಿಂತಿರುಗಿ _PYTOK_READ_ERROR;
                }
                ಮುಂದುವರಿ;
            }
            ಹಿಂತಿರುಗಿ _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk ಒಳಹರಿವು = {0};
        ಆದರೆ (reader->encoding != ಖಾಲಿ) {
            ಆದರೆ (!Py_ಬೈಟುಗಳು_Check(raw)) {
                Py_Err_ಗಣ_String(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                ಹಿಂತಿರುಗಿ _PYTOK_READ_ERROR;
            }
            ಆದರೆ (Py_ಬೈಟುಗಳು_GET_ಗಾತ್ರ(raw) == 0) {
                Py_DECREF(raw);
                ಆದರೆ (Py_Tok_ಆರಂಭ_Decoder(tok, "replace") < 0) {
                    ಹಿಂತಿರುಗಿ _PYTOK_READ_ERROR;
                }
                ಆದರೆ (finalize_decoding(tok) < 0) {
                    ಹಿಂತಿರುಗಿ _PYTOK_READ_ERROR;
                }
                ಮುಂದುವರಿ;
            }
            ಒಳಹರಿವು.owner = raw;
            ಒಳಹರಿವು.ದತ್ತಾಂಶ = Py_ಬೈಟುಗಳು_AS_STRING(raw);
            ಒಳಹರಿವು.len = Py_ಬೈಟುಗಳು_GET_ಗಾತ್ರ(raw);
            ಒಳಹರಿವು.ownership = _PYTOK_CHUNK_PYOBJECT;
            ಪೂರ್ಣಾಂಕ decoded;
            ಆದರೆ (reader->decoder == ಖಾಲಿ &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_ಸಾಲು(&ಒಳಹರಿವು)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &ಒಳಹರಿವು, "utf-8", "replace");
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                decoded = Py_Tok_ಆರಂಭ_Decoder(tok, "replace");
                ಆದರೆ (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &ಒಳಹರಿವು, 0);
                }
            }
            ಆದರೆ (decoded < 0) {
                _PyTok_ChunkClear(&ಒಳಹರಿವು);
                ಹಿಂತಿರುಗಿ _PYTOK_READ_ERROR;
            }
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ (!PyUnicode_Check(raw)) {
                Py_Err_ಗಣ_String(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                ಹಿಂತಿರುಗಿ _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *utf8 = Py_Unicode_As_UTF8_And_ಗಾತ್ರ(raw, &utf8_len);
            ಆದರೆ (utf8 == ಖಾಲಿ) {
                Py_DECREF(raw);
                ಹಿಂತಿರುಗಿ _PYTOK_READ_ERROR;
            }
            ಒಳಹರಿವು.owner = raw;
            ಒಳಹರಿವು.ದತ್ತಾಂಶ = (ಅಕ್ಷರ *)utf8;
            ಒಳಹರಿವು.len = utf8_len;
            ಒಳಹರಿವು.ownership = _PYTOK_CHUNK_PYOBJECT;
            ಆದರೆ (ಒಳಹರಿವು.len == 0) {
                _PyTok_ChunkClear(&ಒಳಹರಿವು);
                ಆದರೆ (finalize_decoding(tok) < 0) {
                    ಹಿಂತಿರುಗಿ _PYTOK_READ_ERROR;
                }
                ಮುಂದುವರಿ;
            }
        }

        ಆದರೆ (reader->decoded_pos == reader->decoded_len &&
                chunk_is_ಸಾಲು(&ಒಳಹರಿವು)) {
            *chunk = ಒಳಹರಿವು;
            ಹಿಂತಿರುಗಿ PYTOK_READ_ಸಾಲು;
        }

        ಆದರೆ (append_decoded(reader, ಒಳಹರಿವು.ದತ್ತಾಂಶ, ಒಳಹರಿವು.len) < 0) {
            _PyTok_ChunkClear(&ಒಳಹರಿವು);
            tok->ಮುಗಿದ = E_NOMEM;
            ಹಿಂತಿರುಗಿ _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&ಒಳಹರಿವು);
        ಆದರೆ (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            ಪೂರ್ಣಾಂಕ pending = Py_Tok_Decoder_Has_Buffered_ಒಳಹರಿವು(tok);
            ಆದರೆ (pending < 0) {
                ಹಿಂತಿರುಗಿ _PYTOK_READ_ERROR;
            }
            ಆದರೆ (!pending && append_implicit_newline(reader) < 0) {
                tok->ಮುಗಿದ = E_NOMEM;
                ಹಿಂತಿರುಗಿ _PYTOK_READ_ERROR;
            }
        }
    }
}

ಸ್ಥಿರ Py_Tok_Read_ಫಲಿತಾಂಶ
ಮುಂದಿನ_interactive(ರಚನೆ tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    ಆದರೆ (reader->stop_interactive) {
        ಹಿಂತಿರುಗಿ _PYTOK_READ_STOPPED;
    }
    ಅಕ್ಷರ *ಒಳಹರಿವು = PyOS_Readline(
        reader->fp != ಖಾಲಿ ? reader->fp : stdin, stdout, reader->prompt);
    ಆದರೆ (reader->nextprompt != ಖಾಲಿ) {
        reader->prompt = reader->nextprompt;
    }
    ಆದರೆ (ಒಳಹರಿವು == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ PYTOK_READ_ಅಡಚಣೆ;
    }
    Py_ssize_t len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಒಳಹರಿವು);
    ಆದರೆ (len == 0) {
        PyMem_Free(ಒಳಹರಿವು);
        ಹಿಂತಿರುಗಿ _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .ದತ್ತಾಂಶ = ಒಳಹರಿವು,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    ಆದರೆ (reader->encoding != ಖಾಲಿ &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, ಖಾಲಿ) < 0) {
        _PyTok_ChunkClear(&decoded);
        ಹಿಂತಿರುಗಿ _PYTOK_READ_ERROR;
    }
    ಆದರೆ (memchr(decoded.ದತ್ತಾಂಶ, '\r', decoded.len) == ಖಾಲಿ) {
        *chunk = decoded;
        ಹಿಂತಿರುಗಿ PYTOK_READ_ಸಾಲು;
    }
    chunk->ದತ್ತಾಂಶ = _PyTok_NormalizeNewlines(
        decoded.ದತ್ತಾಂಶ, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    ಆದರೆ (chunk->ದತ್ತಾಂಶ == ಖಾಲಿ) {
        tok->ಮುಗಿದ = E_NOMEM;
        ಹಿಂತಿರುಗಿ _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    ಹಿಂತಿರುಗಿ PYTOK_READ_ಸಾಲು;
}

ಪೂರ್ಣಾಂಕ
_PyTok_ReaderIsInteractive(ಸ್ಥಿರಾಂಕ ರಚನೆ tok_state *tok)
{
    ಹಿಂತಿರುಗಿ tok->reader->ಬಗೆ == _PYTOK_READER_INTERACTIVE;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *
Py_Tokenizer_Retained_ಮೂಲ(ಸ್ಥಿರಾಂಕ ರಚನೆ tok_state *tok)
{
    ಆದರೆ (tok->reader->ಬಗೆ == _PYTOK_READER_PREPARED) {
        ಹಿಂತಿರುಗಿ Py_Tok_ಮೂಲ_ದತ್ತಾಂಶ(&tok->ಮೂಲ);
    }
    ಆದರೆ (tok->reader->ಬಗೆ == _PYTOK_READER_INTERACTIVE) {
        ಹಿಂತಿರುಗಿ tok->ಮೂಲ.ಬೈಟುಗಳು;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಶೂನ್ಯ
_PyTok_ReaderStopInteractive(ರಚನೆ tok_state *tok)
{
    ಆದರೆ (_PyTok_ReaderIsInteractive(tok)) {
        tok->reader->stop_interactive = 1;
    }
}

ಸ್ಥಿರ Py_Tok_Read_ಫಲಿತಾಂಶ
reader_ಮುಂದಿನ(ರಚನೆ tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    ಆಯ್ಕೆ (tok->reader->ಬಗೆ) {
        ಸ್ಥಿತಿ _PYTOK_READER_PREPARED:
            ಹಿಂತಿರುಗಿ ಮುಂದಿನ_prepared(tok, chunk);
        ಸ್ಥಿತಿ _PYTOK_READER_FILE:
            ಹಿಂತಿರುಗಿ ಮುಂದಿನ_file(tok, chunk);
        ಸ್ಥಿತಿ _PYTOK_READER_READLINE:
            ಹಿಂತಿರುಗಿ ಮುಂದಿನ_readline(tok, chunk);
        ಸ್ಥಿತಿ _PYTOK_READER_INTERACTIVE:
            ಹಿಂತಿರುಗಿ ಮುಂದಿನ_interactive(tok, chunk);
    }
    Py_UNREACHABLE();
}

ಸ್ಥಿರ ಶೂನ್ಯ
reset_streaming_ಬಫರ್(ರಚನೆ tok_state *tok)
{
    ಪ್ರತಿಜ್ಞಾ(tok->cur >= tok->buf_ಸ್ಥಾನಾಂತರ && tok->cur <= tok->inp);
    Py_Tok_ಮೂಲ_Discard(&tok->ಮೂಲ);
    tok->buf_ಸ್ಥಾನಾಂತರ = tok->ಮೂಲ.ತಳ_ಸ್ಥಾನಾಂತರ;
    tok->cur = tok->inp = tok->ಮೂಲ.ತಳ_ಸ್ಥಾನಾಂತರ;
    tok->ಸಾಲು_ಆರಂಭ = tok->buf_ಸ್ಥಾನಾಂತರ;
}

ಪೂರ್ಣಾಂಕ
_PyTok_ReaderUnderflow(ರಚನೆ tok_state *tok)
{
    ಪ್ರತಿಜ್ಞಾ(tok->cur >= tok->buf_ಸ್ಥಾನಾಂತರ && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    Py_Tok_Reader_ಬಗೆ ಬಗೆ = reader->ಬಗೆ;
    ಪೂರ್ಣಾಂಕ prepared = ಬಗೆ == _PYTOK_READER_PREPARED;
    ಪೂರ್ಣಾಂಕ streaming = reader_is_streaming(ಬಗೆ);
    ಪೂರ್ಣಾಂಕ reset_ಬಫರ್ = tok->ಆರಂಭ < 0 &&
        _PyLexer_CurrentFTString(tok) == ಖಾಲಿ;

    _PyTok_Chunk chunk;
    Py_Tok_Read_ಫಲಿತಾಂಶ ಫಲಿತಾಂಶ = reader_ಮುಂದಿನ(tok, &chunk);
    ಆದರೆ (ಫಲಿತಾಂಶ != PYTOK_READ_ಸಾಲು) {
        ಆದರೆ (reset_ಬಫರ್ && streaming) {
            reset_streaming_ಬಫರ್(tok);
        }
        ಆದರೆ (ಫಲಿತಾಂಶ == _PYTOK_READ_EOF) {
            tok->ಮುಗಿದ = E_EOF;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಫಲಿತಾಂಶ == _PYTOK_READ_STOPPED) {
            tok->ಮುಗಿದ = E_INTERACT_STOP;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಫಲಿತಾಂಶ == PYTOK_READ_ಅಡಚಣೆ) {
            tok->ಮುಗಿದ = E_INTR;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ (tok->ಮುಗಿದ == E_OK) {
                tok->ಮುಗಿದ = PyErr_ExceptionMatches(Py_Exc_ಸ್ಮರಣೆ_Error)
                    ? E_NOMEM : E_ERROR;
            }
        }
        ಆದರೆ (ಬಗೆ == _PYTOK_READER_INTERACTIVE &&
                ಫಲಿತಾಂಶ != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (tok->lineno == ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) {
        Py_Err_ಗಣ_String(Py_Exc_ಅತಿಪ್ರವಾಹ_Error, "too many tokenizer source lines");
        tok->ಮುಗಿದ = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ (!prepared) {
        ಆದರೆ (streaming && reset_ಬಫರ್) {
            reset_streaming_ಬಫರ್(tok);
        }
        _PyTok_Off ಮೂಲ_ಆರಂಭ = Py_Tok_ಮೂಲ_Append_ಸಾಲು(
            &tok->ಮೂಲ, chunk.ದತ್ತಾಂಶ, chunk.len);
        ಆದರೆ (ಮೂಲ_ಆರಂಭ < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->ಮುಗಿದ = PyErr_ExceptionMatches(Py_Exc_ಸ್ಮರಣೆ_Error)
                ? E_NOMEM : E_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
        ಆದರೆ (reset_ಬಫರ್) {
            tok->cur = ಮೂಲ_ಆರಂಭ;
            tok->buf_ಸ್ಥಾನಾಂತರ = ಮೂಲ_ಆರಂಭ;
            tok->ಸಾಲು_ಆರಂಭ = tok->buf_ಸ್ಥಾನಾಂತರ;
        }
        tok->inp = ಮೂಲ_ಆರಂಭ + chunk.len;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ (reset_ಬಫರ್) {
            tok->buf_ಸ್ಥಾನಾಂತರ = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    ಆದರೆ (ಬಗೆ == _PYTOK_READER_FILE &&
            (reader->encoding == ಖಾಲಿ ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                Py_Tok_ಮೂಲ_ಸೂಚಕ(&tok->ಮೂಲ, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        ಹಿಂತಿರುಗಿ 0;
    }
    _PyTok_ChunkClear(&chunk);
    ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ರಚನೆ tok_state *
tokenizer_new_with_reader(Py_Tok_Reader_ಬಗೆ ಬಗೆ)
{
    ರಚನೆ tok_state *tok = PyMem_Calloc(1, ಗಾತ್ರ(*tok));
    ಆದರೆ (tok == ಖಾಲಿ) {
        Py_Err_No_ಸ್ಮರಣೆ();
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    tok->ಆರಂಭ = tok->ಸಾಲು_ಆರಂಭ = -1;
    Py_Tok_ಮೂಲ_Init(&tok->ಮೂಲ);
    tok->ಮುಗಿದ = E_OK;
    tok->layout.at_bol = 1;
    tok->ಆರಂಭ_loc = (_PyTok_Loc){-1, -1};
    tok->ftstring_ರಾಶಿ = tok->ftstring_ರಾಶಿ_inline;
    tok->ftstring_ಸಾಮರ್ಥ್ಯ = FTSTRING_ರಾಶಿ_INLINE_ಸಾಮರ್ಥ್ಯ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ Py_DEBUG
    tok->debug = _Py_GetConfig()->parser_debug;
#ಆದರೆ_ಅಂತ್ಯ
    tok->reader = PyMem_Calloc(1, ಗಾತ್ರ(*tok->reader));
    ಆದರೆ (tok->reader == ಖಾಲಿ) {
        Py_Err_No_ಸ್ಮರಣೆ();
        _PyTokenizer_Free(tok);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    tok->reader->ಬಗೆ = ಬಗೆ;
    ಆದರೆ (ಬಗೆ == _PYTOK_READER_PREPARED) {
        ಹಿಂತಿರುಗಿ tok;
    }
    ಆದರೆ (reader_is_streaming(ಬಗೆ)) {
        tok->cur = tok->inp = tok->ಮೂಲ.ತಳ_ಸ್ಥಾನಾಂತರ;
        tok->ಸಾಲು_ಆರಂಭ = tok->buf_ಸ್ಥಾನಾಂತರ;
    }
    ಹಿಂತಿರುಗಿ tok;
}

ಸ್ಥಿರ ರಚನೆ tok_state *
tokenizer_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳಹರಿವು, ಪೂರ್ಣಾಂಕ utf8_only, ಪೂರ್ಣಾಂಕ exec_ಒಳಹರಿವು,
                      ಪೂರ್ಣಾಂಕ preserve_crlf)
{
    ರಚನೆ tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    ಆದರೆ (tok == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (_PyTok_PrepareString(
            tok, ಒಳಹರಿವು, utf8_only, exec_ಒಳಹರಿವು, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    tok->cur = tok->inp = tok->ಮೂಲ.ತಳ_ಸ್ಥಾನಾಂತರ;
    tok->ಸಾಲು_ಆರಂಭ = tok->ಮೂಲ.ತಳ_ಸ್ಥಾನಾಂತರ;
    ಹಿಂತಿರುಗಿ tok;
}

ರಚನೆ tok_state *
_PyTokenizer_FromString(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳಹರಿವು, ಪೂರ್ಣಾಂಕ exec_ಒಳಹರಿವು, ಪೂರ್ಣಾಂಕ preserve_crlf)
{
    ಹಿಂತಿರುಗಿ tokenizer_from_string(ಒಳಹರಿವು, 0, exec_ಒಳಹರಿವು, preserve_crlf);
}

ರಚನೆ tok_state *
_PyTokenizer_FromUTF8(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳಹರಿವು, ಪೂರ್ಣಾಂಕ exec_ಒಳಹರಿವು, ಪೂರ್ಣಾಂಕ preserve_crlf)
{
    ಹಿಂತಿರುಗಿ tokenizer_from_string(ಒಳಹರಿವು, 1, exec_ಒಳಹರಿವು, preserve_crlf);
}

ರಚನೆ tok_state *
_PyTokenizer_FromReadline(PyObject *readline, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *encoding)
{
    ರಚನೆ tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    ಆದರೆ (tok == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (encoding != ಖಾಲಿ && Py_Tok_ಗಣ_Encoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    tok->reader->readline = Py_NewRef(readline);
    ಹಿಂತಿರುಗಿ tok;
}

ರಚನೆ tok_state *
_PyTokenizer_FromFile(FILE *fp, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *encoding,
                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ps1, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ps2)
{
    Py_Tok_Reader_ಬಗೆ ಬಗೆ = ps1 != ಖಾಲಿ || ps2 != ಖಾಲಿ
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    ರಚನೆ tok_state *tok = tokenizer_new_with_reader(ಬಗೆ);
    ಆದರೆ (tok == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (encoding != ಖಾಲಿ && Py_Tok_ಗಣ_Encoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    ಹಿಂತಿರುಗಿ tok;
}

#ಆದರೆ defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಒಕ್ಕೂಟ {
    ಶೂನ್ಯ *cookie;
    ಪೂರ್ಣಾಂಕ fd;
} borrowed_fd;

ಸ್ಥಿರ ssize_t
borrow_read(ಶೂನ್ಯ *cookie, ಅಕ್ಷರ *ಬಫರ್, ಗಾತ್ರ_t ಗಾತ್ರ_2)
{
    borrowed_fd borrowed = {.cookie = cookie};
    ಹಿಂತಿರುಗಿ ಪಠ(borrowed.fd, ಬಫರ್, ಗಾತ್ರ_2);
}

ಸ್ಥಿರ FILE *
fdopen_borrow(ಪೂರ್ಣಾಂಕ fd)
{
    cookie_io_functions_t callbacks = {borrow_read, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ};
    borrowed_fd borrowed = {.fd = fd};
    ಹಿಂತಿರುಗಿ fopencookie(borrowed.cookie, "r", callbacks);
}
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ FILE *
fdopen_borrow(ಪೂರ್ಣಾಂಕ fd)
{
    ಪೂರ್ಣಾಂಕ copy = _Py_dup(fd);
    ಆದರೆ (copy < 0) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    FILE *fp = fdopen(copy, "r");
    ಆದರೆ (fp == ಖಾಲಿ) {
        ಪೂರ್ಣಾಂಕ saved_errno = errno;
        close(copy);
        errno = saved_errno;
    }
    ಹಿಂತಿರುಗಿ fp;
}
#ಆದರೆ_ಅಂತ್ಯ

ಅಕ್ಷರ *
Py_Tokenizer_ಕಂಡುಹಿಡಿ_Encoding_Filename(ಪೂರ್ಣಾಂಕ fd, PyObject *filename)
{
    FILE *fp = fdopen_borrow(fd);
    ಆದರೆ (fp == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ರಚನೆ tok_state *tok = _PyTokenizer_FromFile(fp, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
    ಆದರೆ (tok == ಖಾಲಿ) {
        fclose(fp);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != ಖಾಲಿ ? filename : &_Py_STR(anon_string));
    ಅಕ್ಷರ *encoding = ಖಾಲಿ;
    ಆದರೆ (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = ಖಾಲಿ;
    }
    fclose(fp);
    _PyTokenizer_Free(tok);
    ಹಿಂತಿರುಗಿ encoding;
}
