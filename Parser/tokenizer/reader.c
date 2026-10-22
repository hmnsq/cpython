#अन्तर्भाव "Python.h"
#अन्तर्भाव "pycore_fileutils.h"
#अन्तर्भाव "pycore_pystate.h"
#अन्तर्भाव "pycore_runtime.h"

#अन्तर्भाव "errcode.h"
#अन्तर्भाव "helpers.h"
#अन्तर्भाव "reader.h"
#अन्तर्भाव "reader_internal.h"
#अन्तर्भाव "../lexer/state.h"

#परिभाषय READER_बफर_GROWTH_गुणकः 2

#यदि_परिभाषितम् HAVE_UNISTD_H
#  include <unistd.h>
#यदि_अन्तः

स्थिर अन्तःस्थ पूर्णाङ्क
reader_is_streaming(Py_Tok_Reader_विधा विधा)
{
    प्रत्यागम विधा == _PYTOK_READER_FILE || विधा == _PYTOK_READER_READLINE;
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
         i < (पूर्णाङ्क)Py_ARRAY_दीर्घता(reader->prefetched_पङ्क्तयः); i++) {
        _PyTok_ChunkClear(&reader->prefetched_पङ्क्तयः[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_बफर);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = रिक्त;
}

पूर्णाङ्क
_PyTokenizer_Traverse(संरचना tok_state *tok, visitproc दर्शय, शून्य *arg)
{
    Py_दर्शय(tok->filename);
    Py_दर्शय(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_दर्शय(reader->readline);
    Py_दर्शय(reader->decoder);
    प्रति (पूर्णाङ्क i = 0;
         i < (पूर्णाङ्क)Py_ARRAY_दीर्घता(reader->prefetched_पङ्क्तयः); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_पङ्क्तयः[i];
        यदि (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_दर्शय(chunk->owner);
        }
    }
    प्रत्यागम 0;
}

पूर्णाङ्क
Py_Tok_Reserve_बफर(वर्ण **बफर, Py_ssize_t *क्षमता, Py_ssize_t needed,
                     Py_ssize_t initial_क्षमता)
{
    प्रतिज्ञा(initial_क्षमता > 0);
    यदि (needed <= *क्षमता) {
        प्रत्यागम 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *क्षमता > 0 ? *क्षमता : initial_क्षमता;
    यावत् (cap < needed) {
        यदि (cap > PY_SSIZE_T_MAX / READER_बफर_GROWTH_गुणकः) {
            cap = needed;
            भङ्ग;
        }
        cap *= READER_बफर_GROWTH_गुणकः;
    }
    वर्ण *resized = PyMem_Realloc(*बफर, cap);
    यदि (resized == रिक्त) {
        Py_Err_No_स्मृतिः();
        प्रत्यागम -1;
    }
    *बफर = resized;
    *क्षमता = cap;
    प्रत्यागम 0;
}

स्थिर पूर्णाङ्क
append_decoded(_PyTok_Reader *reader, ध्रुव वर्ण *दत्त, Py_ssize_t len)
{
    यदि (len == 0) {
        प्रत्यागम 0;
    }
    यदि (reader->decoded_pos > 0) {
        Py_ssize_t remaining = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (आकार_t)remaining);
        reader->decoded_अवलोकनम् -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = remaining;
    }
    यदि (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            Py_Tok_Reserve_बफर(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        Py_Err_No_स्मृतिः();
        प्रत्यागम -1;
    }
    स्मृतिं_प्रतिलिख(reader->decoded + reader->decoded_len, दत्त, (आकार_t)len);
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
    reader->decoded_पुच्छ_is_implicit = 1;
    प्रत्यागम 0;
}

स्थिर पूर्णाङ्क
निष्कासय_decoded_पङ्क्तिः(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    प्रतिज्ञा(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    प्रतिज्ञा(reader->decoded_अवलोकनम् >= reader->decoded_pos &&
           reader->decoded_अवलोकनम् <= reader->decoded_len);
    यदि (reader->decoded_pos == reader->decoded_len) {
        प्रत्यागम 0;
    }
    वर्ण *start = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    वर्ण *newline = memchr(reader->decoded + reader->decoded_अवलोकनम्, '\n',
                           reader->decoded_len - reader->decoded_अवलोकनम्);
    यदि (newline == रिक्त) {
        reader->decoded_अवलोकनम् = reader->decoded_len;
        प्रत्यागम 0;
    }
    Py_ssize_t len = newline - start + 1;
    chunk->दत्त = start;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_अवलोकनम् = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_पुच्छ_is_implicit;
    यदि (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_अवलोकनम् = 0;
        reader->decoded_पुच्छ_is_implicit = 0;
    }
    प्रत्यागम 1;
}

स्थिर पूर्णाङ्क
chunk_is_पङ्क्तिः(ध्रुव _PyTok_Chunk *chunk)
{
    यदि (chunk->len == 0 || chunk->दत्त[chunk->len - 1] != '\n') {
        प्रत्यागम 0;
    }
    प्रत्यागम memchr(chunk->दत्त, '\n', chunk->len - 1) == रिक्त;
}

स्थिर Py_Tok_Read_परिणाम
अग्र_prepared(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    यदि (tok->lineno >= tok->source.nlines) {
        प्रत्यागम _PYTOK_READ_EOF;
    }
    Py_Tok_विस्तृतिः पुच्छ = {tok->inp, tok->source.आधारः_विस्थापनम् + tok->source.len};
    _PyTok_Off newline = _PyTok_SourceFindByte(&tok->source, पुच्छ, '\n');
    Py_Tok_विस्तृतिः पङ्क्तिः = {पुच्छ.start, newline >= 0 ? newline + 1 : पुच्छ.अन्तः_2};
    chunk->दत्त = (वर्ण *)Py_Tok_Source_विस्तृतिः_View(
        &tok->source, पङ्क्तिः, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = पङ्क्तिः.अन्तः_2 == पुच्छ.अन्तः_2
        ? tok->reader->prepared_final_newline : 0;
    प्रत्यागम PYTOK_READ_पङ्क्तिः;
}

स्थिर Py_Tok_Read_परिणाम
read_file_पङ्क्तिः(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    प्रति (;;) {
        यदि (len > PY_SSIZE_T_MAX - BUFSIZ) {
            Py_Err_No_स्मृतिः();
            प्रत्यागम _PYTOK_READ_ERROR;
        }
        यदि (Py_Tok_Reserve_बफर(&reader->file_बफर, &reader->file_बफर_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            प्रत्यागम _PYTOK_READ_ERROR;
        }
        पूर्णाङ्क उपलब्धम् = (पूर्णाङ्क)Py_MIN(reader->file_बफर_cap - len, अधिकतम_पूर्णाङ्कः);
        आकार_t पठ = 0;
        वर्ण *परिणाम = Py_Universal_Newline_Fgets_With_आकार(
            reader->file_बफर + len, उपलब्धम्, reader->fp, रिक्त, &पठ);
        यदि (परिणाम == रिक्त) {
            यदि (len == 0) {
                प्रत्यागम _PYTOK_READ_EOF;
            }
            भङ्ग;
        }
        len += (Py_ssize_t)पठ;
        यदि (len > 0 && reader->file_बफर[len - 1] == '\n') {
            भङ्ग;
        }
    }
    पूर्णाङ्क implicit = reader->file_बफर[len - 1] != '\n';
    chunk->दत्त = reader->file_बफर;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    प्रत्यागम PYTOK_READ_पङ्क्तिः;
}

स्थिर पूर्णाङ्क
initialize_file(संरचना tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    यदि (reader->encoding != रिक्त) {
        प्रत्यागम _PyTok_StartDecoder(tok, "strict");
    }

    Py_Tok_Read_परिणाम परिणाम = read_file_पङ्क्तिः(
        tok, &reader->prefetched_पङ्क्तयः[0]);
    यदि (परिणाम == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        प्रत्यागम 0;
    }
    यदि (परिणाम != PYTOK_READ_पङ्क्तिः) {
        प्रत्यागम -1;
    }
    Py_ssize_t bom_len;
    Py_Tok_Encoding_परिणाम detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_पङ्क्तयः[0], रिक्त, 0, &bom_len);
    यदि (detection == _PYTOK_ENCODING_ERROR) {
        प्रत्यागम -1;
    }
    यदि (detection == PYTOK_ENCODING_NEED_SECOND_पङ्क्तिः) {
        वर्ण *first = Py_Tok_Copy_अष्टकानि(
            reader->prefetched_पङ्क्तयः[0].दत्त,
            reader->prefetched_पङ्क्तयः[0].len);
        यदि (first == रिक्त) {
            tok->समाप्तम् = E_NOMEM;
            प्रत्यागम -1;
        }
        reader->prefetched_पङ्क्तयः[0].दत्त = first;
        reader->prefetched_पङ्क्तयः[0].ownership = _PYTOK_CHUNK_PYMEM;
        परिणाम = read_file_पङ्क्तिः(tok, &reader->prefetched_पङ्क्तयः[1]);
        यदि (परिणाम == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        अन्यथा यदि (परिणाम != PYTOK_READ_पङ्क्तिः) {
            प्रत्यागम -1;
        }
        _PyTok_Chunk *second = reader->prefetched_पङ्क्तयः[1].दत्त != रिक्त
            ? &reader->prefetched_पङ्क्तयः[1] : रिक्त;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_पङ्क्तयः[0], second, 1, &bom_len);
        यदि (detection == _PYTOK_ENCODING_ERROR) {
            प्रत्यागम -1;
        }
    }
    यदि (bom_len != 0) {
        _PyTok_Chunk *first = &reader->prefetched_पङ्क्तयः[0];
        यदि (first->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(first->दत्त, first->दत्त + bom_len,
                    (आकार_t)(first->len - bom_len));
            first->दत्त[first->len - bom_len] = '\0';
        }
        अन्यथा {
            first->दत्त += bom_len;
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
        _PyTok_Chunk निवेशः = {
            .दत्त = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        पूर्णाङ्क decoded = _PyTok_DecodeChunk(tok, &निवेशः, 1);
        यदि (decoded == 0 &&
                append_decoded(reader, निवेशः.दत्त, निवेशः.len) < 0) {
            tok->समाप्तम् = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&निवेशः);
        यदि (decoded < 0) {
            प्रत्यागम -1;
        }
    }
    यदि (append_implicit_newline(reader) < 0) {
        tok->समाप्तम् = E_NOMEM;
        प्रत्यागम -1;
    }
    प्रत्यागम 0;
}

स्थिर Py_Tok_Read_परिणाम
अग्र_file(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    यदि (!reader->file_initialized && initialize_file(tok) < 0) {
        प्रत्यागम _PYTOK_READ_ERROR;
    }
    प्रति (;;) {
        यदि (निष्कासय_decoded_पङ्क्तिः(reader, chunk)) {
            प्रत्यागम PYTOK_READ_पङ्क्तिः;
        }
        _PyTok_Chunk निवेशः = {0};
        यदि (reader->prefetched_पङ्क्तयः[0].दत्त != रिक्त) {
            निवेशः = reader->prefetched_पङ्क्तयः[0];
            reader->prefetched_पङ्क्तयः[0] = (_PyTok_Chunk){0};
        }
        अन्यथा यदि (reader->prefetched_पङ्क्तयः[1].दत्त != रिक्त) {
            निवेशः = reader->prefetched_पङ्क्तयः[1];
            reader->prefetched_पङ्क्तयः[1] = (_PyTok_Chunk){0};
        }
        अन्यथा यदि (!reader->file_eof) {
            Py_Tok_Read_परिणाम परिणाम = read_file_पङ्क्तिः(tok, &निवेशः);
            यदि (परिणाम == _PYTOK_READ_ERROR) {
                प्रत्यागम परिणाम;
            }
            यदि (परिणाम == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        यदि (निवेशः.दत्त != रिक्त) {
            पूर्णाङ्क implicit = निवेशः.implicit_newline;
            यदि (reader->decoder == रिक्त && !implicit) {
                *chunk = निवेशः;
                प्रत्यागम PYTOK_READ_पङ्क्तिः;
            }
            पूर्णाङ्क decoded = _PyTok_DecodeChunk(tok, &निवेशः, 0);
            यदि (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_पङ्क्तिः(&निवेशः)) {
                *chunk = निवेशः;
                प्रत्यागम PYTOK_READ_पङ्क्तिः;
            }
            यदि (decoded == 0 &&
                    append_decoded(reader, निवेशः.दत्त, निवेशः.len) < 0) {
                tok->समाप्तम् = E_NOMEM;
                decoded = -1;
            }
            यदि (decoded == 0 && implicit) {
                reader->decoded_पुच्छ_is_implicit = 1;
            }
            _PyTok_ChunkClear(&निवेशः);
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

स्थिर Py_Tok_Read_परिणाम
अग्र_readline(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    प्रति (;;) {
        यदि (निष्कासय_decoded_पङ्क्तिः(reader, chunk)) {
            प्रत्यागम PYTOK_READ_पङ्क्तिः;
        }
        यदि (reader->decoder_finalized) {
            प्रत्यागम _PYTOK_READ_EOF;
        }

        PyObject *raw = Py_Object_Call_No_तर्काः(reader->readline);
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

        _PyTok_Chunk निवेशः = {0};
        यदि (reader->encoding != रिक्त) {
            यदि (!Py_अष्टकानि_Check(raw)) {
                Py_Err_समुच्चयः_String(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                प्रत्यागम _PYTOK_READ_ERROR;
            }
            यदि (Py_अष्टकानि_GET_आकार(raw) == 0) {
                Py_DECREF(raw);
                यदि (_PyTok_StartDecoder(tok, "replace") < 0) {
                    प्रत्यागम _PYTOK_READ_ERROR;
                }
                यदि (finalize_decoding(tok) < 0) {
                    प्रत्यागम _PYTOK_READ_ERROR;
                }
                अनुवर्त;
            }
            निवेशः.owner = raw;
            निवेशः.दत्त = Py_अष्टकानि_AS_STRING(raw);
            निवेशः.len = Py_अष्टकानि_GET_आकार(raw);
            निवेशः.ownership = _PYTOK_CHUNK_PYOBJECT;
            पूर्णाङ्क decoded;
            यदि (reader->decoder == रिक्त &&
                    वाक्यं_तुलय(reader->encoding, "utf-8") == 0 &&
                    chunk_is_पङ्क्तिः(&निवेशः)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &निवेशः, "utf-8", "replace");
            }
            अन्यथा {
                decoded = _PyTok_StartDecoder(tok, "replace");
                यदि (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &निवेशः, 0);
                }
            }
            यदि (decoded < 0) {
                _PyTok_ChunkClear(&निवेशः);
                प्रत्यागम _PYTOK_READ_ERROR;
            }
        }
        अन्यथा {
            यदि (!PyUnicode_Check(raw)) {
                Py_Err_समुच्चयः_String(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                प्रत्यागम _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            ध्रुव वर्ण *utf8 = Py_Unicode_As_UTF8_And_आकार(raw, &utf8_len);
            यदि (utf8 == रिक्त) {
                Py_DECREF(raw);
                प्रत्यागम _PYTOK_READ_ERROR;
            }
            निवेशः.owner = raw;
            निवेशः.दत्त = (वर्ण *)utf8;
            निवेशः.len = utf8_len;
            निवेशः.ownership = _PYTOK_CHUNK_PYOBJECT;
            यदि (निवेशः.len == 0) {
                _PyTok_ChunkClear(&निवेशः);
                यदि (finalize_decoding(tok) < 0) {
                    प्रत्यागम _PYTOK_READ_ERROR;
                }
                अनुवर्त;
            }
        }

        यदि (reader->decoded_pos == reader->decoded_len &&
                chunk_is_पङ्क्तिः(&निवेशः)) {
            *chunk = निवेशः;
            प्रत्यागम PYTOK_READ_पङ्क्तिः;
        }

        यदि (append_decoded(reader, निवेशः.दत्त, निवेशः.len) < 0) {
            _PyTok_ChunkClear(&निवेशः);
            tok->समाप्तम् = E_NOMEM;
            प्रत्यागम _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&निवेशः);
        यदि (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            पूर्णाङ्क pending = Py_Tok_Decoder_Has_Buffered_निवेशः(tok);
            यदि (pending < 0) {
                प्रत्यागम _PYTOK_READ_ERROR;
            }
            यदि (!pending && append_implicit_newline(reader) < 0) {
                tok->समाप्तम् = E_NOMEM;
                प्रत्यागम _PYTOK_READ_ERROR;
            }
        }
    }
}

स्थिर Py_Tok_Read_परिणाम
अग्र_interactive(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    यदि (reader->stop_interactive) {
        प्रत्यागम _PYTOK_READ_STOPPED;
    }
    वर्ण *निवेशः = PyOS_Readline(
        reader->fp != रिक्त ? reader->fp : stdin, stdout, reader->prompt);
    यदि (reader->nextprompt != रिक्त) {
        reader->prompt = reader->nextprompt;
    }
    यदि (निवेशः == रिक्त) {
        प्रत्यागम PYTOK_READ_व्यवधानम्;
    }
    Py_ssize_t len = वाक्यदैर्घ्यम्(निवेशः);
    यदि (len == 0) {
        PyMem_Free(निवेशः);
        प्रत्यागम _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .दत्त = निवेशः,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    यदि (reader->encoding != रिक्त &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, रिक्त) < 0) {
        _PyTok_ChunkClear(&decoded);
        प्रत्यागम _PYTOK_READ_ERROR;
    }
    यदि (memchr(decoded.दत्त, '\r', decoded.len) == रिक्त) {
        *chunk = decoded;
        प्रत्यागम PYTOK_READ_पङ्क्तिः;
    }
    chunk->दत्त = _PyTok_NormalizeNewlines(
        decoded.दत्त, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    यदि (chunk->दत्त == रिक्त) {
        tok->समाप्तम् = E_NOMEM;
        प्रत्यागम _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    प्रत्यागम PYTOK_READ_पङ्क्तिः;
}

पूर्णाङ्क
_PyTok_ReaderIsInteractive(ध्रुव संरचना tok_state *tok)
{
    प्रत्यागम tok->reader->विधा == _PYTOK_READER_INTERACTIVE;
}

ध्रुव वर्ण *
_PyTokenizer_RetainedSource(ध्रुव संरचना tok_state *tok)
{
    यदि (tok->reader->विधा == _PYTOK_READER_PREPARED) {
        प्रत्यागम Py_Tok_Source_दत्त(&tok->source);
    }
    यदि (tok->reader->विधा == _PYTOK_READER_INTERACTIVE) {
        प्रत्यागम tok->source.अष्टकानि;
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

स्थिर Py_Tok_Read_परिणाम
reader_अग्र(संरचना tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    विकल्प (tok->reader->विधा) {
        पक्ष _PYTOK_READER_PREPARED:
            प्रत्यागम अग्र_prepared(tok, chunk);
        पक्ष _PYTOK_READER_FILE:
            प्रत्यागम अग्र_file(tok, chunk);
        पक्ष _PYTOK_READER_READLINE:
            प्रत्यागम अग्र_readline(tok, chunk);
        पक्ष _PYTOK_READER_INTERACTIVE:
            प्रत्यागम अग्र_interactive(tok, chunk);
    }
    Py_UNREACHABLE();
}

स्थिर शून्य
reset_streaming_बफर(संरचना tok_state *tok)
{
    प्रतिज्ञा(tok->cur >= tok->buf_विस्थापनम् && tok->cur <= tok->inp);
    _PyTok_SourceDiscard(&tok->source);
    tok->buf_विस्थापनम् = tok->source.आधारः_विस्थापनम्;
    tok->cur = tok->inp = tok->source.आधारः_विस्थापनम्;
    tok->पङ्क्तिः_start = tok->buf_विस्थापनम्;
}

पूर्णाङ्क
_PyTok_ReaderUnderflow(संरचना tok_state *tok)
{
    प्रतिज्ञा(tok->cur >= tok->buf_विस्थापनम् && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    Py_Tok_Reader_विधा विधा = reader->विधा;
    पूर्णाङ्क prepared = विधा == _PYTOK_READER_PREPARED;
    पूर्णाङ्क streaming = reader_is_streaming(विधा);
    पूर्णाङ्क reset_बफर = tok->start < 0 &&
        _PyLexer_CurrentFTString(tok) == रिक्त;

    _PyTok_Chunk chunk;
    Py_Tok_Read_परिणाम परिणाम = reader_अग्र(tok, &chunk);
    यदि (परिणाम != PYTOK_READ_पङ्क्तिः) {
        यदि (reset_बफर && streaming) {
            reset_streaming_बफर(tok);
        }
        यदि (परिणाम == _PYTOK_READ_EOF) {
            tok->समाप्तम् = E_EOF;
        }
        अन्यथा यदि (परिणाम == _PYTOK_READ_STOPPED) {
            tok->समाप्तम् = E_INTERACT_STOP;
        }
        अन्यथा यदि (परिणाम == PYTOK_READ_व्यवधानम्) {
            tok->समाप्तम् = E_INTR;
        }
        अन्यथा {
            यदि (tok->समाप्तम् == E_OK) {
                tok->समाप्तम् = PyErr_ExceptionMatches(Py_Exc_स्मृतिः_Error)
                    ? E_NOMEM : E_ERROR;
            }
        }
        यदि (विधा == _PYTOK_READER_INTERACTIVE &&
                परिणाम != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        प्रत्यागम 0;
    }
    यदि (tok->lineno == अधिकतम_पूर्णाङ्कः) {
        Py_Err_समुच्चयः_String(Py_Exc_अतिप्रवाहः_Error, "too many tokenizer source lines");
        tok->समाप्तम् = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        प्रत्यागम 0;
    }

    यदि (!prepared) {
        यदि (streaming && reset_बफर) {
            reset_streaming_बफर(tok);
        }
        _PyTok_Off source_start = Py_Tok_Source_Append_पङ्क्तिः(
            &tok->source, chunk.दत्त, chunk.len);
        यदि (source_start < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->समाप्तम् = PyErr_ExceptionMatches(Py_Exc_स्मृतिः_Error)
                ? E_NOMEM : E_ERROR;
            प्रत्यागम 0;
        }
        यदि (reset_बफर) {
            tok->cur = source_start;
            tok->buf_विस्थापनम् = source_start;
            tok->पङ्क्तिः_start = tok->buf_विस्थापनम्;
        }
        tok->inp = source_start + chunk.len;
    }
    अन्यथा {
        यदि (reset_बफर) {
            tok->buf_विस्थापनम् = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    यदि (विधा == _PYTOK_READER_FILE &&
            (reader->encoding == रिक्त ||
             वाक्यं_तुलय(reader->encoding, "utf-8") == 0) &&
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
tokenizer_new_with_reader(Py_Tok_Reader_विधा विधा)
{
    संरचना tok_state *tok = PyMem_Calloc(1, परिमाण(*tok));
    यदि (tok == रिक्त) {
        Py_Err_No_स्मृतिः();
        प्रत्यागम रिक्त;
    }
    tok->start = tok->पङ्क्तिः_start = -1;
    _PyTok_SourceInit(&tok->source);
    tok->समाप्तम् = E_OK;
    tok->layout.at_bol = 1;
    tok->start_loc = (_PyTok_Loc){-1, -1};
    tok->ftstring_राशिः = tok->ftstring_राशिः_inline;
    tok->ftstring_क्षमता = FTSTRING_राशिः_INLINE_क्षमता;
#यदि_परिभाषितम् Py_DEBUG
    tok->debug = _Py_GetConfig()->parser_debug;
#यदि_अन्तः
    tok->reader = PyMem_Calloc(1, परिमाण(*tok->reader));
    यदि (tok->reader == रिक्त) {
        Py_Err_No_स्मृतिः();
        _PyTokenizer_Free(tok);
        प्रत्यागम रिक्त;
    }
    tok->reader->विधा = विधा;
    यदि (विधा == _PYTOK_READER_PREPARED) {
        प्रत्यागम tok;
    }
    यदि (reader_is_streaming(विधा)) {
        tok->cur = tok->inp = tok->source.आधारः_विस्थापनम्;
        tok->पङ्क्तिः_start = tok->buf_विस्थापनम्;
    }
    प्रत्यागम tok;
}

स्थिर संरचना tok_state *
tokenizer_from_string(ध्रुव वर्ण *निवेशः, पूर्णाङ्क utf8_only, पूर्णाङ्क exec_निवेशः,
                      पूर्णाङ्क preserve_crlf)
{
    संरचना tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    यदि (tok == रिक्त) {
        प्रत्यागम रिक्त;
    }
    यदि (_PyTok_PrepareString(
            tok, निवेशः, utf8_only, exec_निवेशः, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        प्रत्यागम रिक्त;
    }
    tok->cur = tok->inp = tok->source.आधारः_विस्थापनम्;
    tok->पङ्क्तिः_start = tok->source.आधारः_विस्थापनम्;
    प्रत्यागम tok;
}

संरचना tok_state *
_PyTokenizer_FromString(ध्रुव वर्ण *निवेशः, पूर्णाङ्क exec_निवेशः, पूर्णाङ्क preserve_crlf)
{
    प्रत्यागम tokenizer_from_string(निवेशः, 0, exec_निवेशः, preserve_crlf);
}

संरचना tok_state *
_PyTokenizer_FromUTF8(ध्रुव वर्ण *निवेशः, पूर्णाङ्क exec_निवेशः, पूर्णाङ्क preserve_crlf)
{
    प्रत्यागम tokenizer_from_string(निवेशः, 1, exec_निवेशः, preserve_crlf);
}

संरचना tok_state *
_PyTokenizer_FromReadline(PyObject *readline, ध्रुव वर्ण *encoding)
{
    संरचना tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    यदि (tok == रिक्त) {
        प्रत्यागम रिक्त;
    }
    यदि (encoding != रिक्त && Py_Tok_समुच्चयः_Encoding(tok, encoding) < 0) {
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
    Py_Tok_Reader_विधा विधा = ps1 != रिक्त || ps2 != रिक्त
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    संरचना tok_state *tok = tokenizer_new_with_reader(विधा);
    यदि (tok == रिक्त) {
        प्रत्यागम रिक्त;
    }
    यदि (encoding != रिक्त && Py_Tok_समुच्चयः_Encoding(tok, encoding) < 0) {
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
borrow_read(शून्य *cookie, वर्ण *बफर, आकार_t आकार)
{
    borrowed_fd borrowed = {.cookie = cookie};
    प्रत्यागम पठ(borrowed.fd, बफर, आकार);
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
#यदि_अन्तः

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
