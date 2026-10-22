#అన్తర్భావ "Python.h"
#అన్తర్భావ "pycore_fileutils.h"
#అన్తర్భావ "pycore_pystate.h"
#అన్తర్భావ "pycore_runtime.h"

#అన్తర్భావ "errcode.h"
#అన్తర్భావ "helpers.h"
#అన్తర్భావ "reader.h"
#అన్తర్భావ "reader_internal.h"
#అన్తర్భావ "../lexer/state.h"

#నిర్వచించు READER_బఫరు_GROWTH_గుణకం 2

#నిర్వచితమైతే HAVE_UNISTD_H
#  include <unistd.h>
#అయితే_ముగింపు

స్థిర అంతఃవరుస పూర్ణాంకం
reader_is_streaming(Py_Tok_Reader_రకం రకం)
{
    తిరిగి రకం == _PYTOK_READER_FILE || రకం == _PYTOK_READER_READLINE;
}

శూన్య
_PyTok_ReaderFree(నిర్మాణం tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    ఒకవేళ (reader == ఖాళీ) {
        తిరిగి;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    కొరకు (పూర్ణాంకం i = 0;
         i < (పూర్ణాంకం)Py_ARRAY_నిడివి(reader->prefetched_పంక్తులు); i++) {
        _PyTok_ChunkClear(&reader->prefetched_పంక్తులు[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_బఫరు);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = ఖాళీ;
}

పూర్ణాంకం
_PyTokenizer_Traverse(నిర్మాణం tok_state *tok, visitproc సందర్శించు, శూన్య *arg)
{
    Py_సందర్శించు(tok->filename);
    Py_సందర్శించు(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_సందర్శించు(reader->readline);
    Py_సందర్శించు(reader->decoder);
    కొరకు (పూర్ణాంకం i = 0;
         i < (పూర్ణాంకం)Py_ARRAY_నిడివి(reader->prefetched_పంక్తులు); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_పంక్తులు[i];
        ఒకవేళ (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_సందర్శించు(chunk->owner);
        }
    }
    తిరిగి 0;
}

పూర్ణాంకం
Py_Tok_Reserve_బఫరు(అక్షరం **బఫరు, Py_ssize_t *సామర్థ్యం, Py_ssize_t needed,
                     Py_ssize_t initial_సామర్థ్యం)
{
    ప్రతిజ్ఞా(initial_సామర్థ్యం > 0);
    ఒకవేళ (needed <= *సామర్థ్యం) {
        తిరిగి 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *సామర్థ్యం > 0 ? *సామర్థ్యం : initial_సామర్థ్యం;
    అయితే (cap < needed) {
        ఒకవేళ (cap > PY_SSIZE_T_MAX / READER_బఫరు_GROWTH_గుణకం) {
            cap = needed;
            ఆపు;
        }
        cap *= READER_బఫరు_GROWTH_గుణకం;
    }
    అక్షరం *resized = PyMem_Realloc(*బఫరు, cap);
    ఒకవేళ (resized == ఖాళీ) {
        Py_Err_No_జ్ఞాపకం();
        తిరిగి -1;
    }
    *బఫరు = resized;
    *సామర్థ్యం = cap;
    తిరిగి 0;
}

స్థిర పూర్ణాంకం
append_decoded(_PyTok_Reader *reader, స్థిరరాశి అక్షరం *దత్తాంశం, Py_ssize_t len)
{
    ఒకవేళ (len == 0) {
        తిరిగి 0;
    }
    ఒకవేళ (reader->decoded_pos > 0) {
        Py_ssize_t మిగిలిన = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (కొలత_t)మిగిలిన);
        reader->decoded_పరిశీలన -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = మిగిలిన;
    }
    ఒకవేళ (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            Py_Tok_Reserve_బఫరు(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        Py_Err_No_జ్ఞాపకం();
        తిరిగి -1;
    }
    జ్ఞాపకం_నకలు(reader->decoded + reader->decoded_len, దత్తాంశం, (కొలత_t)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    తిరిగి 0;
}

స్థిర పూర్ణాంకం
append_implicit_newline(_PyTok_Reader *reader)
{
    ఒకవేళ (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        తిరిగి 0;
    }
    ఒకవేళ (append_decoded(reader, "\n", 1) < 0) {
        తిరిగి -1;
    }
    reader->decoded_తోక_is_implicit = 1;
    తిరిగి 0;
}

స్థిర పూర్ణాంకం
తీయి_decoded_పంక్తి(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    ప్రతిజ్ఞా(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    ప్రతిజ్ఞా(reader->decoded_పరిశీలన >= reader->decoded_pos &&
           reader->decoded_పరిశీలన <= reader->decoded_len);
    ఒకవేళ (reader->decoded_pos == reader->decoded_len) {
        తిరిగి 0;
    }
    అక్షరం *ప్రారంభం = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    అక్షరం *newline = memchr(reader->decoded + reader->decoded_పరిశీలన, '\n',
                           reader->decoded_len - reader->decoded_పరిశీలన);
    ఒకవేళ (newline == ఖాళీ) {
        reader->decoded_పరిశీలన = reader->decoded_len;
        తిరిగి 0;
    }
    Py_ssize_t len = newline - ప్రారంభం + 1;
    chunk->దత్తాంశం = ప్రారంభం;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_పరిశీలన = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_తోక_is_implicit;
    ఒకవేళ (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_పరిశీలన = 0;
        reader->decoded_తోక_is_implicit = 0;
    }
    తిరిగి 1;
}

స్థిర పూర్ణాంకం
chunk_is_పంక్తి(స్థిరరాశి _PyTok_Chunk *chunk)
{
    ఒకవేళ (chunk->len == 0 || chunk->దత్తాంశం[chunk->len - 1] != '\n') {
        తిరిగి 0;
    }
    తిరిగి memchr(chunk->దత్తాంశం, '\n', chunk->len - 1) == ఖాళీ;
}

స్థిర Py_Tok_Read_ఫలితం
తదుపరి_prepared(నిర్మాణం tok_state *tok, _PyTok_Chunk *chunk)
{
    ఒకవేళ (tok->lineno >= tok->ఆధారం.nlines) {
        తిరిగి _PYTOK_READ_EOF;
    }
    Py_Tok_పరిధి తోక = {tok->inp, tok->ఆధారం.పునాది_స్థానభ్రంశం + tok->ఆధారం.len};
    _PyTok_Off newline = Py_Tok_ఆధారం_కనుగొను_Byte(&tok->ఆధారం, తోక, '\n');
    Py_Tok_పరిధి పంక్తి = {తోక.ప్రారంభం, newline >= 0 ? newline + 1 : తోక.చివర};
    chunk->దత్తాంశం = (అక్షరం *)Py_Tok_ఆధారం_పరిధి_View(
        &tok->ఆధారం, పంక్తి, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = పంక్తి.చివర == తోక.చివర
        ? tok->reader->prepared_final_newline : 0;
    తిరిగి PYTOK_READ_పంక్తి;
}

స్థిర Py_Tok_Read_ఫలితం
read_file_పంక్తి(నిర్మాణం tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    కొరకు (;;) {
        ఒకవేళ (len > PY_SSIZE_T_MAX - BUFSIZ) {
            Py_Err_No_జ్ఞాపకం();
            తిరిగి _PYTOK_READ_ERROR;
        }
        ఒకవేళ (Py_Tok_Reserve_బఫరు(&reader->file_బఫరు, &reader->file_బఫరు_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            తిరిగి _PYTOK_READ_ERROR;
        }
        పూర్ణాంకం అందుబాటు = (పూర్ణాంకం)Py_MIN(reader->file_బఫరు_cap - len, గరిష్ఠ_పూర్ణాంకం);
        కొలత_t పఠ = 0;
        అక్షరం *ఫలితం = Py_Universal_Newline_Fgets_With_కొలత(
            reader->file_బఫరు + len, అందుబాటు, reader->fp, ఖాళీ, &పఠ);
        ఒకవేళ (ఫలితం == ఖాళీ) {
            ఒకవేళ (len == 0) {
                తిరిగి _PYTOK_READ_EOF;
            }
            ఆపు;
        }
        len += (Py_ssize_t)పఠ;
        ఒకవేళ (len > 0 && reader->file_బఫరు[len - 1] == '\n') {
            ఆపు;
        }
    }
    పూర్ణాంకం implicit = reader->file_బఫరు[len - 1] != '\n';
    chunk->దత్తాంశం = reader->file_బఫరు;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    తిరిగి PYTOK_READ_పంక్తి;
}

స్థిర పూర్ణాంకం
initialize_file(నిర్మాణం tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    ఒకవేళ (reader->encoding != ఖాళీ) {
        తిరిగి Py_Tok_ప్రారంభం_Decoder(tok, "strict");
    }

    Py_Tok_Read_ఫలితం ఫలితం = read_file_పంక్తి(
        tok, &reader->prefetched_పంక్తులు[0]);
    ఒకవేళ (ఫలితం == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        తిరిగి 0;
    }
    ఒకవేళ (ఫలితం != PYTOK_READ_పంక్తి) {
        తిరిగి -1;
    }
    Py_ssize_t bom_len;
    Py_Tok_Encoding_ఫలితం detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_పంక్తులు[0], ఖాళీ, 0, &bom_len);
    ఒకవేళ (detection == _PYTOK_ENCODING_ERROR) {
        తిరిగి -1;
    }
    ఒకవేళ (detection == PYTOK_ENCODING_NEED_రెండవ_పంక్తి) {
        అక్షరం *మొదటి = Py_Tok_Copy_బైట్లు(
            reader->prefetched_పంక్తులు[0].దత్తాంశం,
            reader->prefetched_పంక్తులు[0].len);
        ఒకవేళ (మొదటి == ఖాళీ) {
            tok->పూర్తి = E_NOMEM;
            తిరిగి -1;
        }
        reader->prefetched_పంక్తులు[0].దత్తాంశం = మొదటి;
        reader->prefetched_పంక్తులు[0].ownership = _PYTOK_CHUNK_PYMEM;
        ఫలితం = read_file_పంక్తి(tok, &reader->prefetched_పంక్తులు[1]);
        ఒకవేళ (ఫలితం == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        లేకపోతే ఒకవేళ (ఫలితం != PYTOK_READ_పంక్తి) {
            తిరిగి -1;
        }
        _PyTok_Chunk *రెండవ = reader->prefetched_పంక్తులు[1].దత్తాంశం != ఖాళీ
            ? &reader->prefetched_పంక్తులు[1] : ఖాళీ;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_పంక్తులు[0], రెండవ, 1, &bom_len);
        ఒకవేళ (detection == _PYTOK_ENCODING_ERROR) {
            తిరిగి -1;
        }
    }
    ఒకవేళ (bom_len != 0) {
        _PyTok_Chunk *మొదటి = &reader->prefetched_పంక్తులు[0];
        ఒకవేళ (మొదటి->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(మొదటి->దత్తాంశం, మొదటి->దత్తాంశం + bom_len,
                    (కొలత_t)(మొదటి->len - bom_len));
            మొదటి->దత్తాంశం[మొదటి->len - bom_len] = '\0';
        }
        లేకపోతే {
            మొదటి->దత్తాంశం += bom_len;
        }
        మొదటి->len -= bom_len;
    }
    ఒకవేళ (Py_Tok_ప్రారంభం_Decoder(tok, "strict") < 0) {
        తిరిగి -1;
    }
    తిరిగి 0;
}

స్థిర పూర్ణాంకం
finalize_decoding(నిర్మాణం tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    ఒకవేళ (reader->decoder_finalized) {
        తిరిగి 0;
    }
    reader->decoder_finalized = 1;
    ఒకవేళ (reader->decoder != ఖాళీ) {
        _PyTok_Chunk ఆదానం = {
            .దత్తాంశం = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        పూర్ణాంకం decoded = _PyTok_DecodeChunk(tok, &ఆదానం, 1);
        ఒకవేళ (decoded == 0 &&
                append_decoded(reader, ఆదానం.దత్తాంశం, ఆదానం.len) < 0) {
            tok->పూర్తి = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&ఆదానం);
        ఒకవేళ (decoded < 0) {
            తిరిగి -1;
        }
    }
    ఒకవేళ (append_implicit_newline(reader) < 0) {
        tok->పూర్తి = E_NOMEM;
        తిరిగి -1;
    }
    తిరిగి 0;
}

స్థిర Py_Tok_Read_ఫలితం
తదుపరి_file(నిర్మాణం tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    ఒకవేళ (!reader->file_initialized && initialize_file(tok) < 0) {
        తిరిగి _PYTOK_READ_ERROR;
    }
    కొరకు (;;) {
        ఒకవేళ (తీయి_decoded_పంక్తి(reader, chunk)) {
            తిరిగి PYTOK_READ_పంక్తి;
        }
        _PyTok_Chunk ఆదానం = {0};
        ఒకవేళ (reader->prefetched_పంక్తులు[0].దత్తాంశం != ఖాళీ) {
            ఆదానం = reader->prefetched_పంక్తులు[0];
            reader->prefetched_పంక్తులు[0] = (_PyTok_Chunk){0};
        }
        లేకపోతే ఒకవేళ (reader->prefetched_పంక్తులు[1].దత్తాంశం != ఖాళీ) {
            ఆదానం = reader->prefetched_పంక్తులు[1];
            reader->prefetched_పంక్తులు[1] = (_PyTok_Chunk){0};
        }
        లేకపోతే ఒకవేళ (!reader->file_eof) {
            Py_Tok_Read_ఫలితం ఫలితం = read_file_పంక్తి(tok, &ఆదానం);
            ఒకవేళ (ఫలితం == _PYTOK_READ_ERROR) {
                తిరిగి ఫలితం;
            }
            ఒకవేళ (ఫలితం == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        ఒకవేళ (ఆదానం.దత్తాంశం != ఖాళీ) {
            పూర్ణాంకం implicit = ఆదానం.implicit_newline;
            ఒకవేళ (reader->decoder == ఖాళీ && !implicit) {
                *chunk = ఆదానం;
                తిరిగి PYTOK_READ_పంక్తి;
            }
            పూర్ణాంకం decoded = _PyTok_DecodeChunk(tok, &ఆదానం, 0);
            ఒకవేళ (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_పంక్తి(&ఆదానం)) {
                *chunk = ఆదానం;
                తిరిగి PYTOK_READ_పంక్తి;
            }
            ఒకవేళ (decoded == 0 &&
                    append_decoded(reader, ఆదానం.దత్తాంశం, ఆదానం.len) < 0) {
                tok->పూర్తి = E_NOMEM;
                decoded = -1;
            }
            ఒకవేళ (decoded == 0 && implicit) {
                reader->decoded_తోక_is_implicit = 1;
            }
            _PyTok_ChunkClear(&ఆదానం);
            ఒకవేళ (decoded < 0) {
                తిరిగి _PYTOK_READ_ERROR;
            }
            కొనసాగు;
        }
        ఒకవేళ (!reader->decoder_finalized) {
            ఒకవేళ (finalize_decoding(tok) < 0) {
                తిరిగి _PYTOK_READ_ERROR;
            }
            కొనసాగు;
        }
        తిరిగి _PYTOK_READ_EOF;
    }
}

స్థిర Py_Tok_Read_ఫలితం
తదుపరి_readline(నిర్మాణం tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    కొరకు (;;) {
        ఒకవేళ (తీయి_decoded_పంక్తి(reader, chunk)) {
            తిరిగి PYTOK_READ_పంక్తి;
        }
        ఒకవేళ (reader->decoder_finalized) {
            తిరిగి _PYTOK_READ_EOF;
        }

        PyObject *raw = Py_Object_Call_No_వాదనలు(reader->readline);
        ఒకవేళ (raw == ఖాళీ) {
            ఒకవేళ (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                ఒకవేళ (finalize_decoding(tok) < 0) {
                    తిరిగి _PYTOK_READ_ERROR;
                }
                కొనసాగు;
            }
            తిరిగి _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk ఆదానం = {0};
        ఒకవేళ (reader->encoding != ఖాళీ) {
            ఒకవేళ (!Py_బైట్లు_Check(raw)) {
                Py_Err_సమితి_String(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                తిరిగి _PYTOK_READ_ERROR;
            }
            ఒకవేళ (Py_బైట్లు_GET_కొలత(raw) == 0) {
                Py_DECREF(raw);
                ఒకవేళ (Py_Tok_ప్రారంభం_Decoder(tok, "replace") < 0) {
                    తిరిగి _PYTOK_READ_ERROR;
                }
                ఒకవేళ (finalize_decoding(tok) < 0) {
                    తిరిగి _PYTOK_READ_ERROR;
                }
                కొనసాగు;
            }
            ఆదానం.owner = raw;
            ఆదానం.దత్తాంశం = Py_బైట్లు_AS_STRING(raw);
            ఆదానం.len = Py_బైట్లు_GET_కొలత(raw);
            ఆదానం.ownership = _PYTOK_CHUNK_PYOBJECT;
            పూర్ణాంకం decoded;
            ఒకవేళ (reader->decoder == ఖాళీ &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_పంక్తి(&ఆదానం)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &ఆదానం, "utf-8", "replace");
            }
            లేకపోతే {
                decoded = Py_Tok_ప్రారంభం_Decoder(tok, "replace");
                ఒకవేళ (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &ఆదానం, 0);
                }
            }
            ఒకవేళ (decoded < 0) {
                _PyTok_ChunkClear(&ఆదానం);
                తిరిగి _PYTOK_READ_ERROR;
            }
        }
        లేకపోతే {
            ఒకవేళ (!PyUnicode_Check(raw)) {
                Py_Err_సమితి_String(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                తిరిగి _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            స్థిరరాశి అక్షరం *utf8 = Py_Unicode_As_UTF8_And_కొలత(raw, &utf8_len);
            ఒకవేళ (utf8 == ఖాళీ) {
                Py_DECREF(raw);
                తిరిగి _PYTOK_READ_ERROR;
            }
            ఆదానం.owner = raw;
            ఆదానం.దత్తాంశం = (అక్షరం *)utf8;
            ఆదానం.len = utf8_len;
            ఆదానం.ownership = _PYTOK_CHUNK_PYOBJECT;
            ఒకవేళ (ఆదానం.len == 0) {
                _PyTok_ChunkClear(&ఆదానం);
                ఒకవేళ (finalize_decoding(tok) < 0) {
                    తిరిగి _PYTOK_READ_ERROR;
                }
                కొనసాగు;
            }
        }

        ఒకవేళ (reader->decoded_pos == reader->decoded_len &&
                chunk_is_పంక్తి(&ఆదానం)) {
            *chunk = ఆదానం;
            తిరిగి PYTOK_READ_పంక్తి;
        }

        ఒకవేళ (append_decoded(reader, ఆదానం.దత్తాంశం, ఆదానం.len) < 0) {
            _PyTok_ChunkClear(&ఆదానం);
            tok->పూర్తి = E_NOMEM;
            తిరిగి _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&ఆదానం);
        ఒకవేళ (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            పూర్ణాంకం pending = Py_Tok_Decoder_Has_Buffered_ఆదానం(tok);
            ఒకవేళ (pending < 0) {
                తిరిగి _PYTOK_READ_ERROR;
            }
            ఒకవేళ (!pending && append_implicit_newline(reader) < 0) {
                tok->పూర్తి = E_NOMEM;
                తిరిగి _PYTOK_READ_ERROR;
            }
        }
    }
}

స్థిర Py_Tok_Read_ఫలితం
తదుపరి_interactive(నిర్మాణం tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    ఒకవేళ (reader->stop_interactive) {
        తిరిగి _PYTOK_READ_STOPPED;
    }
    అక్షరం *ఆదానం = PyOS_Readline(
        reader->fp != ఖాళీ ? reader->fp : stdin, stdout, reader->prompt);
    ఒకవేళ (reader->nextprompt != ఖాళీ) {
        reader->prompt = reader->nextprompt;
    }
    ఒకవేళ (ఆదానం == ఖాళీ) {
        తిరిగి PYTOK_READ_అంతరాయం;
    }
    Py_ssize_t len = అక్షరశ్రేణి_పొడవు(ఆదానం);
    ఒకవేళ (len == 0) {
        PyMem_Free(ఆదానం);
        తిరిగి _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .దత్తాంశం = ఆదానం,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    ఒకవేళ (reader->encoding != ఖాళీ &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, ఖాళీ) < 0) {
        _PyTok_ChunkClear(&decoded);
        తిరిగి _PYTOK_READ_ERROR;
    }
    ఒకవేళ (memchr(decoded.దత్తాంశం, '\r', decoded.len) == ఖాళీ) {
        *chunk = decoded;
        తిరిగి PYTOK_READ_పంక్తి;
    }
    chunk->దత్తాంశం = _PyTok_NormalizeNewlines(
        decoded.దత్తాంశం, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    ఒకవేళ (chunk->దత్తాంశం == ఖాళీ) {
        tok->పూర్తి = E_NOMEM;
        తిరిగి _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    తిరిగి PYTOK_READ_పంక్తి;
}

పూర్ణాంకం
_PyTok_ReaderIsInteractive(స్థిరరాశి నిర్మాణం tok_state *tok)
{
    తిరిగి tok->reader->రకం == _PYTOK_READER_INTERACTIVE;
}

స్థిరరాశి అక్షరం *
Py_Tokenizer_Retained_ఆధారం(స్థిరరాశి నిర్మాణం tok_state *tok)
{
    ఒకవేళ (tok->reader->రకం == _PYTOK_READER_PREPARED) {
        తిరిగి Py_Tok_ఆధారం_దత్తాంశం(&tok->ఆధారం);
    }
    ఒకవేళ (tok->reader->రకం == _PYTOK_READER_INTERACTIVE) {
        తిరిగి tok->ఆధారం.బైట్లు;
    }
    తిరిగి ఖాళీ;
}

శూన్య
_PyTok_ReaderStopInteractive(నిర్మాణం tok_state *tok)
{
    ఒకవేళ (_PyTok_ReaderIsInteractive(tok)) {
        tok->reader->stop_interactive = 1;
    }
}

స్థిర Py_Tok_Read_ఫలితం
reader_తదుపరి(నిర్మాణం tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    ఎంపిక (tok->reader->రకం) {
        స్థితి _PYTOK_READER_PREPARED:
            తిరిగి తదుపరి_prepared(tok, chunk);
        స్థితి _PYTOK_READER_FILE:
            తిరిగి తదుపరి_file(tok, chunk);
        స్థితి _PYTOK_READER_READLINE:
            తిరిగి తదుపరి_readline(tok, chunk);
        స్థితి _PYTOK_READER_INTERACTIVE:
            తిరిగి తదుపరి_interactive(tok, chunk);
    }
    Py_UNREACHABLE();
}

స్థిర శూన్య
reset_streaming_బఫరు(నిర్మాణం tok_state *tok)
{
    ప్రతిజ్ఞా(tok->cur >= tok->buf_స్థానభ్రంశం && tok->cur <= tok->inp);
    Py_Tok_ఆధారం_Discard(&tok->ఆధారం);
    tok->buf_స్థానభ్రంశం = tok->ఆధారం.పునాది_స్థానభ్రంశం;
    tok->cur = tok->inp = tok->ఆధారం.పునాది_స్థానభ్రంశం;
    tok->పంక్తి_ప్రారంభం = tok->buf_స్థానభ్రంశం;
}

పూర్ణాంకం
_PyTok_ReaderUnderflow(నిర్మాణం tok_state *tok)
{
    ప్రతిజ్ఞా(tok->cur >= tok->buf_స్థానభ్రంశం && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    Py_Tok_Reader_రకం రకం = reader->రకం;
    పూర్ణాంకం prepared = రకం == _PYTOK_READER_PREPARED;
    పూర్ణాంకం streaming = reader_is_streaming(రకం);
    పూర్ణాంకం reset_బఫరు = tok->ప్రారంభం < 0 &&
        _PyLexer_CurrentFTString(tok) == ఖాళీ;

    _PyTok_Chunk chunk;
    Py_Tok_Read_ఫలితం ఫలితం = reader_తదుపరి(tok, &chunk);
    ఒకవేళ (ఫలితం != PYTOK_READ_పంక్తి) {
        ఒకవేళ (reset_బఫరు && streaming) {
            reset_streaming_బఫరు(tok);
        }
        ఒకవేళ (ఫలితం == _PYTOK_READ_EOF) {
            tok->పూర్తి = E_EOF;
        }
        లేకపోతే ఒకవేళ (ఫలితం == _PYTOK_READ_STOPPED) {
            tok->పూర్తి = E_INTERACT_STOP;
        }
        లేకపోతే ఒకవేళ (ఫలితం == PYTOK_READ_అంతరాయం) {
            tok->పూర్తి = E_INTR;
        }
        లేకపోతే {
            ఒకవేళ (tok->పూర్తి == E_OK) {
                tok->పూర్తి = PyErr_ExceptionMatches(Py_Exc_జ్ఞాపకం_Error)
                    ? E_NOMEM : E_ERROR;
            }
        }
        ఒకవేళ (రకం == _PYTOK_READER_INTERACTIVE &&
                ఫలితం != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        తిరిగి 0;
    }
    ఒకవేళ (tok->lineno == గరిష్ఠ_పూర్ణాంకం) {
        Py_Err_సమితి_String(Py_Exc_అధికప్రవాహం_Error, "too many tokenizer source lines");
        tok->పూర్తి = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        తిరిగి 0;
    }

    ఒకవేళ (!prepared) {
        ఒకవేళ (streaming && reset_బఫరు) {
            reset_streaming_బఫరు(tok);
        }
        _PyTok_Off ఆధారం_ప్రారంభం = Py_Tok_ఆధారం_Append_పంక్తి(
            &tok->ఆధారం, chunk.దత్తాంశం, chunk.len);
        ఒకవేళ (ఆధారం_ప్రారంభం < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->పూర్తి = PyErr_ExceptionMatches(Py_Exc_జ్ఞాపకం_Error)
                ? E_NOMEM : E_ERROR;
            తిరిగి 0;
        }
        ఒకవేళ (reset_బఫరు) {
            tok->cur = ఆధారం_ప్రారంభం;
            tok->buf_స్థానభ్రంశం = ఆధారం_ప్రారంభం;
            tok->పంక్తి_ప్రారంభం = tok->buf_స్థానభ్రంశం;
        }
        tok->inp = ఆధారం_ప్రారంభం + chunk.len;
    }
    లేకపోతే {
        ఒకవేళ (reset_బఫరు) {
            tok->buf_స్థానభ్రంశం = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    ఒకవేళ (రకం == _PYTOK_READER_FILE &&
            (reader->encoding == ఖాళీ ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                Py_Tok_ఆధారం_సూచిక(&tok->ఆధారం, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        తిరిగి 0;
    }
    _PyTok_ChunkClear(&chunk);
    తిరిగి 1;
}

స్థిర నిర్మాణం tok_state *
tokenizer_new_with_reader(Py_Tok_Reader_రకం రకం)
{
    నిర్మాణం tok_state *tok = PyMem_Calloc(1, పరిమాణం(*tok));
    ఒకవేళ (tok == ఖాళీ) {
        Py_Err_No_జ్ఞాపకం();
        తిరిగి ఖాళీ;
    }
    tok->ప్రారంభం = tok->పంక్తి_ప్రారంభం = -1;
    Py_Tok_ఆధారం_Init(&tok->ఆధారం);
    tok->పూర్తి = E_OK;
    tok->layout.at_bol = 1;
    tok->ప్రారంభం_loc = (_PyTok_Loc){-1, -1};
    tok->ftstring_దొంతర = tok->ftstring_దొంతర_inline;
    tok->ftstring_సామర్థ్యం = FTSTRING_దొంతర_INLINE_సామర్థ్యం;
#నిర్వచితమైతే Py_DEBUG
    tok->debug = _Py_GetConfig()->parser_debug;
#అయితే_ముగింపు
    tok->reader = PyMem_Calloc(1, పరిమాణం(*tok->reader));
    ఒకవేళ (tok->reader == ఖాళీ) {
        Py_Err_No_జ్ఞాపకం();
        _PyTokenizer_Free(tok);
        తిరిగి ఖాళీ;
    }
    tok->reader->రకం = రకం;
    ఒకవేళ (రకం == _PYTOK_READER_PREPARED) {
        తిరిగి tok;
    }
    ఒకవేళ (reader_is_streaming(రకం)) {
        tok->cur = tok->inp = tok->ఆధారం.పునాది_స్థానభ్రంశం;
        tok->పంక్తి_ప్రారంభం = tok->buf_స్థానభ్రంశం;
    }
    తిరిగి tok;
}

స్థిర నిర్మాణం tok_state *
tokenizer_from_string(స్థిరరాశి అక్షరం *ఆదానం, పూర్ణాంకం utf8_only, పూర్ణాంకం exec_ఆదానం,
                      పూర్ణాంకం preserve_crlf)
{
    నిర్మాణం tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    ఒకవేళ (tok == ఖాళీ) {
        తిరిగి ఖాళీ;
    }
    ఒకవేళ (_PyTok_PrepareString(
            tok, ఆదానం, utf8_only, exec_ఆదానం, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        తిరిగి ఖాళీ;
    }
    tok->cur = tok->inp = tok->ఆధారం.పునాది_స్థానభ్రంశం;
    tok->పంక్తి_ప్రారంభం = tok->ఆధారం.పునాది_స్థానభ్రంశం;
    తిరిగి tok;
}

నిర్మాణం tok_state *
_PyTokenizer_FromString(స్థిరరాశి అక్షరం *ఆదానం, పూర్ణాంకం exec_ఆదానం, పూర్ణాంకం preserve_crlf)
{
    తిరిగి tokenizer_from_string(ఆదానం, 0, exec_ఆదానం, preserve_crlf);
}

నిర్మాణం tok_state *
_PyTokenizer_FromUTF8(స్థిరరాశి అక్షరం *ఆదానం, పూర్ణాంకం exec_ఆదానం, పూర్ణాంకం preserve_crlf)
{
    తిరిగి tokenizer_from_string(ఆదానం, 1, exec_ఆదానం, preserve_crlf);
}

నిర్మాణం tok_state *
_PyTokenizer_FromReadline(PyObject *readline, స్థిరరాశి అక్షరం *encoding)
{
    నిర్మాణం tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    ఒకవేళ (tok == ఖాళీ) {
        తిరిగి ఖాళీ;
    }
    ఒకవేళ (encoding != ఖాళీ && Py_Tok_సమితి_Encoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        తిరిగి ఖాళీ;
    }
    tok->reader->readline = Py_NewRef(readline);
    తిరిగి tok;
}

నిర్మాణం tok_state *
_PyTokenizer_FromFile(FILE *fp, స్థిరరాశి అక్షరం *encoding,
                      స్థిరరాశి అక్షరం *ps1, స్థిరరాశి అక్షరం *ps2)
{
    Py_Tok_Reader_రకం రకం = ps1 != ఖాళీ || ps2 != ఖాళీ
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    నిర్మాణం tok_state *tok = tokenizer_new_with_reader(రకం);
    ఒకవేళ (tok == ఖాళీ) {
        తిరిగి ఖాళీ;
    }
    ఒకవేళ (encoding != ఖాళీ && Py_Tok_సమితి_Encoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        తిరిగి ఖాళీ;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    తిరిగి tok;
}

#ఒకవేళ defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
రకనిర్వచనం సంఘం {
    శూన్య *cookie;
    పూర్ణాంకం fd;
} borrowed_fd;

స్థిర ssize_t
borrow_read(శూన్య *cookie, అక్షరం *బఫరు, కొలత_t కొలత)
{
    borrowed_fd borrowed = {.cookie = cookie};
    తిరిగి పఠ(borrowed.fd, బఫరు, కొలత);
}

స్థిర FILE *
fdopen_borrow(పూర్ణాంకం fd)
{
    cookie_io_functions_t callbacks = {borrow_read, ఖాళీ, ఖాళీ, ఖాళీ};
    borrowed_fd borrowed = {.fd = fd};
    తిరిగి fopencookie(borrowed.cookie, "r", callbacks);
}
#లేకపోతే
స్థిర FILE *
fdopen_borrow(పూర్ణాంకం fd)
{
    పూర్ణాంకం copy = _Py_dup(fd);
    ఒకవేళ (copy < 0) {
        తిరిగి ఖాళీ;
    }
    FILE *fp = fdopen(copy, "r");
    ఒకవేళ (fp == ఖాళీ) {
        పూర్ణాంకం saved_errno = errno;
        close(copy);
        errno = saved_errno;
    }
    తిరిగి fp;
}
#అయితే_ముగింపు

అక్షరం *
Py_Tokenizer_కనుగొను_Encoding_Filename(పూర్ణాంకం fd, PyObject *filename)
{
    FILE *fp = fdopen_borrow(fd);
    ఒకవేళ (fp == ఖాళీ) {
        తిరిగి ఖాళీ;
    }
    నిర్మాణం tok_state *tok = _PyTokenizer_FromFile(fp, ఖాళీ, ఖాళీ, ఖాళీ);
    ఒకవేళ (tok == ఖాళీ) {
        fclose(fp);
        తిరిగి ఖాళీ;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != ఖాళీ ? filename : &_Py_STR(anon_string));
    అక్షరం *encoding = ఖాళీ;
    ఒకవేళ (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = ఖాళీ;
    }
    fclose(fp);
    _PyTokenizer_Free(tok);
    తిరిగి encoding;
}
