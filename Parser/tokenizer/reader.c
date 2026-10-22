#ضمن "Python.h"
#ضمن "pycore_fileutils.h"
#ضمن "pycore_pystate.h"
#ضمن "pycore_runtime.h"

#ضمن "errcode.h"
#ضمن "helpers.h"
#ضمن "reader.h"
#ضمن "reader_internal.h"
#ضمن "../lexer/state.h"

#وصف_ڪريو READER_بفر_GROWTH_ضرب_ڪندڙ 2

#جيڪڏهن_وصف_ٿيل HAVE_UNISTD_H
#  include <unistd.h>
#جيڪڏهن_پورو

ساكن مضمن صحيح
reader_is_streaming(Py_Tok_Reader_قسم قسم)
{
    أرجع قسم == _PYTOK_READER_FILE || قسم == _PYTOK_READER_READLINE;
}

فراغ
_PyTok_ReaderFree(بنية tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    إذا (reader == عدم) {
        أرجع;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    لكل (صحيح i = 0;
         i < (صحيح)Py_ARRAY_ڊيگهائي(reader->prefetched_سٽون); i++) {
        _PyTok_ChunkClear(&reader->prefetched_سٽون[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_بفر);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = عدم;
}

صحيح
_PyTokenizer_Traverse(بنية tok_state *tok, visitproc گهمو, فراغ *arg)
{
    Py_گهمو(tok->filename);
    Py_گهمو(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_گهمو(reader->readline);
    Py_گهمو(reader->decoder);
    لكل (صحيح i = 0;
         i < (صحيح)Py_ARRAY_ڊيگهائي(reader->prefetched_سٽون); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_سٽون[i];
        إذا (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_گهمو(chunk->owner);
        }
    }
    أرجع 0;
}

صحيح
Py_Tok_Reserve_بفر(حرف **بفر, Py_ssize_t *صلاحيت, Py_ssize_t needed,
                     Py_ssize_t initial_صلاحيت)
{
    أكد(initial_صلاحيت > 0);
    إذا (needed <= *صلاحيت) {
        أرجع 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *صلاحيت > 0 ? *صلاحيت : initial_صلاحيت;
    طالما (cap < needed) {
        إذا (cap > PY_SSIZE_T_MAX / READER_بفر_GROWTH_ضرب_ڪندڙ) {
            cap = needed;
            اكسر;
        }
        cap *= READER_بفر_GROWTH_ضرب_ڪندڙ;
    }
    حرف *resized = PyMem_Realloc(*بفر, cap);
    إذا (resized == عدم) {
        Py_Err_No_يادگيري();
        أرجع -1;
    }
    *بفر = resized;
    *صلاحيت = cap;
    أرجع 0;
}

ساكن صحيح
append_decoded(_PyTok_Reader *reader, ثابت حرف *معلومات, Py_ssize_t len)
{
    إذا (len == 0) {
        أرجع 0;
    }
    إذا (reader->decoded_pos > 0) {
        Py_ssize_t بچيل = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (سائيز_t)بچيل);
        reader->decoded_جاچ -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = بچيل;
    }
    إذا (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            Py_Tok_Reserve_بفر(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        Py_Err_No_يادگيري();
        أرجع -1;
    }
    يادگيري_نقل(reader->decoded + reader->decoded_len, معلومات, (سائيز_t)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    أرجع 0;
}

ساكن صحيح
append_implicit_newline(_PyTok_Reader *reader)
{
    إذا (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        أرجع 0;
    }
    إذا (append_decoded(reader, "\n", 1) < 0) {
        أرجع -1;
    }
    reader->decoded_پڇ_is_implicit = 1;
    أرجع 0;
}

ساكن صحيح
ڪڍو_decoded_سٽ(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    أكد(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    أكد(reader->decoded_جاچ >= reader->decoded_pos &&
           reader->decoded_جاچ <= reader->decoded_len);
    إذا (reader->decoded_pos == reader->decoded_len) {
        أرجع 0;
    }
    حرف *شروعات = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    حرف *newline = memchr(reader->decoded + reader->decoded_جاچ, '\n',
                           reader->decoded_len - reader->decoded_جاچ);
    إذا (newline == عدم) {
        reader->decoded_جاچ = reader->decoded_len;
        أرجع 0;
    }
    Py_ssize_t len = newline - شروعات + 1;
    chunk->معلومات = شروعات;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_جاچ = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_پڇ_is_implicit;
    إذا (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_جاچ = 0;
        reader->decoded_پڇ_is_implicit = 0;
    }
    أرجع 1;
}

ساكن صحيح
chunk_is_سٽ(ثابت _PyTok_Chunk *chunk)
{
    إذا (chunk->len == 0 || chunk->معلومات[chunk->len - 1] != '\n') {
        أرجع 0;
    }
    أرجع memchr(chunk->معلومات, '\n', chunk->len - 1) == عدم;
}

ساكن Py_Tok_Read_نتيجو
ايندڙ_prepared(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    إذا (tok->lineno >= tok->ذريعو.nlines) {
        أرجع _PYTOK_READ_EOF;
    }
    Py_Tok_وسعت پڇ = {tok->inp, tok->ذريعو.بنياد_وٿي + tok->ذريعو.len};
    _PyTok_Off newline = Py_Tok_ذريعو_ڳوليو_Byte(&tok->ذريعو, پڇ, '\n');
    Py_Tok_وسعت سٽ = {پڇ.شروعات, newline >= 0 ? newline + 1 : پڇ.پڇاڙي};
    chunk->معلومات = (حرف *)Py_Tok_ذريعو_وسعت_View(
        &tok->ذريعو, سٽ, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = سٽ.پڇاڙي == پڇ.پڇاڙي
        ? tok->reader->prepared_final_newline : 0;
    أرجع PYTOK_READ_سٽ;
}

ساكن Py_Tok_Read_نتيجو
read_file_سٽ(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    لكل (;;) {
        إذا (len > PY_SSIZE_T_MAX - BUFSIZ) {
            Py_Err_No_يادگيري();
            أرجع _PYTOK_READ_ERROR;
        }
        إذا (Py_Tok_Reserve_بفر(&reader->file_بفر, &reader->file_بفر_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            أرجع _PYTOK_READ_ERROR;
        }
        صحيح موجود = (صحيح)Py_MIN(reader->file_بفر_cap - len, وڌ_کان_وڌ_عدد);
        سائيز_t اقرأ = 0;
        حرف *نتيجو = Py_Universal_Newline_Fgets_With_سائيز(
            reader->file_بفر + len, موجود, reader->fp, عدم, &اقرأ);
        إذا (نتيجو == عدم) {
            إذا (len == 0) {
                أرجع _PYTOK_READ_EOF;
            }
            اكسر;
        }
        len += (Py_ssize_t)اقرأ;
        إذا (len > 0 && reader->file_بفر[len - 1] == '\n') {
            اكسر;
        }
    }
    صحيح implicit = reader->file_بفر[len - 1] != '\n';
    chunk->معلومات = reader->file_بفر;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    أرجع PYTOK_READ_سٽ;
}

ساكن صحيح
initialize_file(بنية tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    إذا (reader->encoding != عدم) {
        أرجع Py_Tok_شروعات_Decoder(tok, "strict");
    }

    Py_Tok_Read_نتيجو نتيجو = read_file_سٽ(
        tok, &reader->prefetched_سٽون[0]);
    إذا (نتيجو == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        أرجع 0;
    }
    إذا (نتيجو != PYTOK_READ_سٽ) {
        أرجع -1;
    }
    Py_ssize_t bom_len;
    Py_Tok_Encoding_نتيجو detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_سٽون[0], عدم, 0, &bom_len);
    إذا (detection == _PYTOK_ENCODING_ERROR) {
        أرجع -1;
    }
    إذا (detection == PYTOK_ENCODING_NEED_ٻيو_سٽ) {
        حرف *پهريون = Py_Tok_Copy_بائيٽون(
            reader->prefetched_سٽون[0].معلومات,
            reader->prefetched_سٽون[0].len);
        إذا (پهريون == عدم) {
            tok->پورو = E_NOMEM;
            أرجع -1;
        }
        reader->prefetched_سٽون[0].معلومات = پهريون;
        reader->prefetched_سٽون[0].ownership = _PYTOK_CHUNK_PYMEM;
        نتيجو = read_file_سٽ(tok, &reader->prefetched_سٽون[1]);
        إذا (نتيجو == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        وإلا إذا (نتيجو != PYTOK_READ_سٽ) {
            أرجع -1;
        }
        _PyTok_Chunk *ٻيو = reader->prefetched_سٽون[1].معلومات != عدم
            ? &reader->prefetched_سٽون[1] : عدم;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_سٽون[0], ٻيو, 1, &bom_len);
        إذا (detection == _PYTOK_ENCODING_ERROR) {
            أرجع -1;
        }
    }
    إذا (bom_len != 0) {
        _PyTok_Chunk *پهريون = &reader->prefetched_سٽون[0];
        إذا (پهريون->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(پهريون->معلومات, پهريون->معلومات + bom_len,
                    (سائيز_t)(پهريون->len - bom_len));
            پهريون->معلومات[پهريون->len - bom_len] = '\0';
        }
        وإلا {
            پهريون->معلومات += bom_len;
        }
        پهريون->len -= bom_len;
    }
    إذا (Py_Tok_شروعات_Decoder(tok, "strict") < 0) {
        أرجع -1;
    }
    أرجع 0;
}

ساكن صحيح
finalize_decoding(بنية tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    إذا (reader->decoder_finalized) {
        أرجع 0;
    }
    reader->decoder_finalized = 1;
    إذا (reader->decoder != عدم) {
        _PyTok_Chunk داخلا_2 = {
            .معلومات = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        صحيح decoded = _PyTok_DecodeChunk(tok, &داخلا_2, 1);
        إذا (decoded == 0 &&
                append_decoded(reader, داخلا_2.معلومات, داخلا_2.len) < 0) {
            tok->پورو = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&داخلا_2);
        إذا (decoded < 0) {
            أرجع -1;
        }
    }
    إذا (append_implicit_newline(reader) < 0) {
        tok->پورو = E_NOMEM;
        أرجع -1;
    }
    أرجع 0;
}

ساكن Py_Tok_Read_نتيجو
ايندڙ_file(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    إذا (!reader->file_initialized && initialize_file(tok) < 0) {
        أرجع _PYTOK_READ_ERROR;
    }
    لكل (;;) {
        إذا (ڪڍو_decoded_سٽ(reader, chunk)) {
            أرجع PYTOK_READ_سٽ;
        }
        _PyTok_Chunk داخلا_2 = {0};
        إذا (reader->prefetched_سٽون[0].معلومات != عدم) {
            داخلا_2 = reader->prefetched_سٽون[0];
            reader->prefetched_سٽون[0] = (_PyTok_Chunk){0};
        }
        وإلا إذا (reader->prefetched_سٽون[1].معلومات != عدم) {
            داخلا_2 = reader->prefetched_سٽون[1];
            reader->prefetched_سٽون[1] = (_PyTok_Chunk){0};
        }
        وإلا إذا (!reader->file_eof) {
            Py_Tok_Read_نتيجو نتيجو = read_file_سٽ(tok, &داخلا_2);
            إذا (نتيجو == _PYTOK_READ_ERROR) {
                أرجع نتيجو;
            }
            إذا (نتيجو == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        إذا (داخلا_2.معلومات != عدم) {
            صحيح implicit = داخلا_2.implicit_newline;
            إذا (reader->decoder == عدم && !implicit) {
                *chunk = داخلا_2;
                أرجع PYTOK_READ_سٽ;
            }
            صحيح decoded = _PyTok_DecodeChunk(tok, &داخلا_2, 0);
            إذا (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_سٽ(&داخلا_2)) {
                *chunk = داخلا_2;
                أرجع PYTOK_READ_سٽ;
            }
            إذا (decoded == 0 &&
                    append_decoded(reader, داخلا_2.معلومات, داخلا_2.len) < 0) {
                tok->پورو = E_NOMEM;
                decoded = -1;
            }
            إذا (decoded == 0 && implicit) {
                reader->decoded_پڇ_is_implicit = 1;
            }
            _PyTok_ChunkClear(&داخلا_2);
            إذا (decoded < 0) {
                أرجع _PYTOK_READ_ERROR;
            }
            استمر;
        }
        إذا (!reader->decoder_finalized) {
            إذا (finalize_decoding(tok) < 0) {
                أرجع _PYTOK_READ_ERROR;
            }
            استمر;
        }
        أرجع _PYTOK_READ_EOF;
    }
}

ساكن Py_Tok_Read_نتيجو
ايندڙ_readline(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    لكل (;;) {
        إذا (ڪڍو_decoded_سٽ(reader, chunk)) {
            أرجع PYTOK_READ_سٽ;
        }
        إذا (reader->decoder_finalized) {
            أرجع _PYTOK_READ_EOF;
        }

        PyObject *raw = Py_Object_Call_No_دليلون(reader->readline);
        إذا (raw == عدم) {
            إذا (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                إذا (finalize_decoding(tok) < 0) {
                    أرجع _PYTOK_READ_ERROR;
                }
                استمر;
            }
            أرجع _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk داخلا_2 = {0};
        إذا (reader->encoding != عدم) {
            إذا (!Py_بائيٽون_Check(raw)) {
                Py_Err_سيٽ_String(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                أرجع _PYTOK_READ_ERROR;
            }
            إذا (Py_بائيٽون_GET_سائيز(raw) == 0) {
                Py_DECREF(raw);
                إذا (Py_Tok_شروعات_Decoder(tok, "replace") < 0) {
                    أرجع _PYTOK_READ_ERROR;
                }
                إذا (finalize_decoding(tok) < 0) {
                    أرجع _PYTOK_READ_ERROR;
                }
                استمر;
            }
            داخلا_2.owner = raw;
            داخلا_2.معلومات = Py_بائيٽون_AS_STRING(raw);
            داخلا_2.len = Py_بائيٽون_GET_سائيز(raw);
            داخلا_2.ownership = _PYTOK_CHUNK_PYOBJECT;
            صحيح decoded;
            إذا (reader->decoder == عدم &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_سٽ(&داخلا_2)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &داخلا_2, "utf-8", "replace");
            }
            وإلا {
                decoded = Py_Tok_شروعات_Decoder(tok, "replace");
                إذا (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &داخلا_2, 0);
                }
            }
            إذا (decoded < 0) {
                _PyTok_ChunkClear(&داخلا_2);
                أرجع _PYTOK_READ_ERROR;
            }
        }
        وإلا {
            إذا (!PyUnicode_Check(raw)) {
                Py_Err_سيٽ_String(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                أرجع _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            ثابت حرف *utf8 = Py_Unicode_As_UTF8_And_سائيز(raw, &utf8_len);
            إذا (utf8 == عدم) {
                Py_DECREF(raw);
                أرجع _PYTOK_READ_ERROR;
            }
            داخلا_2.owner = raw;
            داخلا_2.معلومات = (حرف *)utf8;
            داخلا_2.len = utf8_len;
            داخلا_2.ownership = _PYTOK_CHUNK_PYOBJECT;
            إذا (داخلا_2.len == 0) {
                _PyTok_ChunkClear(&داخلا_2);
                إذا (finalize_decoding(tok) < 0) {
                    أرجع _PYTOK_READ_ERROR;
                }
                استمر;
            }
        }

        إذا (reader->decoded_pos == reader->decoded_len &&
                chunk_is_سٽ(&داخلا_2)) {
            *chunk = داخلا_2;
            أرجع PYTOK_READ_سٽ;
        }

        إذا (append_decoded(reader, داخلا_2.معلومات, داخلا_2.len) < 0) {
            _PyTok_ChunkClear(&داخلا_2);
            tok->پورو = E_NOMEM;
            أرجع _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&داخلا_2);
        إذا (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            صحيح pending = Py_Tok_Decoder_Has_Buffered_داخلا(tok);
            إذا (pending < 0) {
                أرجع _PYTOK_READ_ERROR;
            }
            إذا (!pending && append_implicit_newline(reader) < 0) {
                tok->پورو = E_NOMEM;
                أرجع _PYTOK_READ_ERROR;
            }
        }
    }
}

ساكن Py_Tok_Read_نتيجو
ايندڙ_interactive(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    إذا (reader->stop_interactive) {
        أرجع _PYTOK_READ_STOPPED;
    }
    حرف *داخلا_2 = PyOS_Readline(
        reader->fp != عدم ? reader->fp : stdin, stdout, reader->prompt);
    إذا (reader->nextprompt != عدم) {
        reader->prompt = reader->nextprompt;
    }
    إذا (داخلا_2 == عدم) {
        أرجع PYTOK_READ_مداخلت;
    }
    Py_ssize_t len = لڙي_ڊيگهه(داخلا_2);
    إذا (len == 0) {
        PyMem_Free(داخلا_2);
        أرجع _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .معلومات = داخلا_2,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    إذا (reader->encoding != عدم &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, عدم) < 0) {
        _PyTok_ChunkClear(&decoded);
        أرجع _PYTOK_READ_ERROR;
    }
    إذا (memchr(decoded.معلومات, '\r', decoded.len) == عدم) {
        *chunk = decoded;
        أرجع PYTOK_READ_سٽ;
    }
    chunk->معلومات = _PyTok_NormalizeNewlines(
        decoded.معلومات, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    إذا (chunk->معلومات == عدم) {
        tok->پورو = E_NOMEM;
        أرجع _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    أرجع PYTOK_READ_سٽ;
}

صحيح
_PyTok_ReaderIsInteractive(ثابت بنية tok_state *tok)
{
    أرجع tok->reader->قسم == _PYTOK_READER_INTERACTIVE;
}

ثابت حرف *
Py_Tokenizer_Retained_ذريعو(ثابت بنية tok_state *tok)
{
    إذا (tok->reader->قسم == _PYTOK_READER_PREPARED) {
        أرجع Py_Tok_ذريعو_معلومات(&tok->ذريعو);
    }
    إذا (tok->reader->قسم == _PYTOK_READER_INTERACTIVE) {
        أرجع tok->ذريعو.بائيٽون;
    }
    أرجع عدم;
}

فراغ
_PyTok_ReaderStopInteractive(بنية tok_state *tok)
{
    إذا (_PyTok_ReaderIsInteractive(tok)) {
        tok->reader->stop_interactive = 1;
    }
}

ساكن Py_Tok_Read_نتيجو
reader_ايندڙ(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    بدل (tok->reader->قسم) {
        حالة _PYTOK_READER_PREPARED:
            أرجع ايندڙ_prepared(tok, chunk);
        حالة _PYTOK_READER_FILE:
            أرجع ايندڙ_file(tok, chunk);
        حالة _PYTOK_READER_READLINE:
            أرجع ايندڙ_readline(tok, chunk);
        حالة _PYTOK_READER_INTERACTIVE:
            أرجع ايندڙ_interactive(tok, chunk);
    }
    Py_UNREACHABLE();
}

ساكن فراغ
reset_streaming_بفر(بنية tok_state *tok)
{
    أكد(tok->cur >= tok->buf_وٿي && tok->cur <= tok->inp);
    Py_Tok_ذريعو_Discard(&tok->ذريعو);
    tok->buf_وٿي = tok->ذريعو.بنياد_وٿي;
    tok->cur = tok->inp = tok->ذريعو.بنياد_وٿي;
    tok->سٽ_شروعات = tok->buf_وٿي;
}

صحيح
_PyTok_ReaderUnderflow(بنية tok_state *tok)
{
    أكد(tok->cur >= tok->buf_وٿي && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    Py_Tok_Reader_قسم قسم = reader->قسم;
    صحيح prepared = قسم == _PYTOK_READER_PREPARED;
    صحيح streaming = reader_is_streaming(قسم);
    صحيح reset_بفر = tok->شروعات < 0 &&
        _PyLexer_CurrentFTString(tok) == عدم;

    _PyTok_Chunk chunk;
    Py_Tok_Read_نتيجو نتيجو = reader_ايندڙ(tok, &chunk);
    إذا (نتيجو != PYTOK_READ_سٽ) {
        إذا (reset_بفر && streaming) {
            reset_streaming_بفر(tok);
        }
        إذا (نتيجو == _PYTOK_READ_EOF) {
            tok->پورو = E_EOF;
        }
        وإلا إذا (نتيجو == _PYTOK_READ_STOPPED) {
            tok->پورو = E_INTERACT_STOP;
        }
        وإلا إذا (نتيجو == PYTOK_READ_مداخلت) {
            tok->پورو = E_INTR;
        }
        وإلا {
            إذا (tok->پورو == E_OK) {
                tok->پورو = PyErr_ExceptionMatches(Py_Exc_يادگيري_Error)
                    ? E_NOMEM : E_ERROR;
            }
        }
        إذا (قسم == _PYTOK_READER_INTERACTIVE &&
                نتيجو != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        أرجع 0;
    }
    إذا (tok->lineno == وڌ_کان_وڌ_عدد) {
        Py_Err_سيٽ_String(Py_Exc_حد_کان_مٿي_Error, "too many tokenizer source lines");
        tok->پورو = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        أرجع 0;
    }

    إذا (!prepared) {
        إذا (streaming && reset_بفر) {
            reset_streaming_بفر(tok);
        }
        _PyTok_Off ذريعو_شروعات = Py_Tok_ذريعو_Append_سٽ(
            &tok->ذريعو, chunk.معلومات, chunk.len);
        إذا (ذريعو_شروعات < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->پورو = PyErr_ExceptionMatches(Py_Exc_يادگيري_Error)
                ? E_NOMEM : E_ERROR;
            أرجع 0;
        }
        إذا (reset_بفر) {
            tok->cur = ذريعو_شروعات;
            tok->buf_وٿي = ذريعو_شروعات;
            tok->سٽ_شروعات = tok->buf_وٿي;
        }
        tok->inp = ذريعو_شروعات + chunk.len;
    }
    وإلا {
        إذا (reset_بفر) {
            tok->buf_وٿي = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    إذا (قسم == _PYTOK_READER_FILE &&
            (reader->encoding == عدم ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                Py_Tok_ذريعو_اشارو(&tok->ذريعو, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        أرجع 0;
    }
    _PyTok_ChunkClear(&chunk);
    أرجع 1;
}

ساكن بنية tok_state *
tokenizer_new_with_reader(Py_Tok_Reader_قسم قسم)
{
    بنية tok_state *tok = PyMem_Calloc(1, حجم(*tok));
    إذا (tok == عدم) {
        Py_Err_No_يادگيري();
        أرجع عدم;
    }
    tok->شروعات = tok->سٽ_شروعات = -1;
    Py_Tok_ذريعو_Init(&tok->ذريعو);
    tok->پورو = E_OK;
    tok->layout.at_bol = 1;
    tok->شروعات_loc = (_PyTok_Loc){-1, -1};
    tok->ftstring_ڍير = tok->ftstring_ڍير_inline;
    tok->ftstring_صلاحيت = FTSTRING_ڍير_INLINE_صلاحيت;
#جيڪڏهن_وصف_ٿيل Py_DEBUG
    tok->debug = _Py_GetConfig()->parser_debug;
#جيڪڏهن_پورو
    tok->reader = PyMem_Calloc(1, حجم(*tok->reader));
    إذا (tok->reader == عدم) {
        Py_Err_No_يادگيري();
        _PyTokenizer_Free(tok);
        أرجع عدم;
    }
    tok->reader->قسم = قسم;
    إذا (قسم == _PYTOK_READER_PREPARED) {
        أرجع tok;
    }
    إذا (reader_is_streaming(قسم)) {
        tok->cur = tok->inp = tok->ذريعو.بنياد_وٿي;
        tok->سٽ_شروعات = tok->buf_وٿي;
    }
    أرجع tok;
}

ساكن بنية tok_state *
tokenizer_from_string(ثابت حرف *داخلا_2, صحيح utf8_only, صحيح exec_داخلا,
                      صحيح preserve_crlf)
{
    بنية tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    إذا (tok == عدم) {
        أرجع عدم;
    }
    إذا (_PyTok_PrepareString(
            tok, داخلا_2, utf8_only, exec_داخلا, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        أرجع عدم;
    }
    tok->cur = tok->inp = tok->ذريعو.بنياد_وٿي;
    tok->سٽ_شروعات = tok->ذريعو.بنياد_وٿي;
    أرجع tok;
}

بنية tok_state *
_PyTokenizer_FromString(ثابت حرف *داخلا_2, صحيح exec_داخلا, صحيح preserve_crlf)
{
    أرجع tokenizer_from_string(داخلا_2, 0, exec_داخلا, preserve_crlf);
}

بنية tok_state *
_PyTokenizer_FromUTF8(ثابت حرف *داخلا_2, صحيح exec_داخلا, صحيح preserve_crlf)
{
    أرجع tokenizer_from_string(داخلا_2, 1, exec_داخلا, preserve_crlf);
}

بنية tok_state *
_PyTokenizer_FromReadline(PyObject *readline, ثابت حرف *encoding)
{
    بنية tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    إذا (tok == عدم) {
        أرجع عدم;
    }
    إذا (encoding != عدم && Py_Tok_سيٽ_Encoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        أرجع عدم;
    }
    tok->reader->readline = Py_NewRef(readline);
    أرجع tok;
}

بنية tok_state *
_PyTokenizer_FromFile(FILE *fp, ثابت حرف *encoding,
                      ثابت حرف *ps1, ثابت حرف *ps2)
{
    Py_Tok_Reader_قسم قسم = ps1 != عدم || ps2 != عدم
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    بنية tok_state *tok = tokenizer_new_with_reader(قسم);
    إذا (tok == عدم) {
        أرجع عدم;
    }
    إذا (encoding != عدم && Py_Tok_سيٽ_Encoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        أرجع عدم;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    أرجع tok;
}

#إذا defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
تعريفنوع اتحاد {
    فراغ *cookie;
    صحيح fd;
} borrowed_fd;

ساكن ssize_t
borrow_read(فراغ *cookie, حرف *بفر, سائيز_t سائيز)
{
    borrowed_fd borrowed = {.cookie = cookie};
    أرجع اقرأ(borrowed.fd, بفر, سائيز);
}

ساكن FILE *
fdopen_borrow(صحيح fd)
{
    cookie_io_functions_t callbacks = {borrow_read, عدم, عدم, عدم};
    borrowed_fd borrowed = {.fd = fd};
    أرجع fopencookie(borrowed.cookie, "r", callbacks);
}
#وإلا
ساكن FILE *
fdopen_borrow(صحيح fd)
{
    صحيح copy = _Py_dup(fd);
    إذا (copy < 0) {
        أرجع عدم;
    }
    FILE *fp = fdopen(copy, "r");
    إذا (fp == عدم) {
        صحيح saved_errno = errno;
        close(copy);
        errno = saved_errno;
    }
    أرجع fp;
}
#جيڪڏهن_پورو

حرف *
Py_Tokenizer_ڳوليو_Encoding_Filename(صحيح fd, PyObject *filename)
{
    FILE *fp = fdopen_borrow(fd);
    إذا (fp == عدم) {
        أرجع عدم;
    }
    بنية tok_state *tok = _PyTokenizer_FromFile(fp, عدم, عدم, عدم);
    إذا (tok == عدم) {
        fclose(fp);
        أرجع عدم;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != عدم ? filename : &_Py_STR(anon_string));
    حرف *encoding = عدم;
    إذا (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = عدم;
    }
    fclose(fp);
    _PyTokenizer_Free(tok);
    أرجع encoding;
}
