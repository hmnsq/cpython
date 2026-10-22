#ضمن "Python.h"
#ضمن "pycore_fileutils.h"
#ضمن "pycore_pystate.h"
#ضمن "pycore_runtime.h"

#ضمن "errcode.h"
#ضمن "helpers.h"
#ضمن "reader.h"
#ضمن "reader_internal.h"
#ضمن "../lexer/state.h"

#تعریف_کرو READER_بفر_GROWTH_ضارب 2

#جے_تعریف_شدہ HAVE_UNISTD_H
#  include <unistd.h>
#جے_انت

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
         i < (صحيح)Py_ARRAY_لمائی(reader->prefetched_سطریں); i++) {
        _PyTok_ChunkClear(&reader->prefetched_سطریں[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_بفر);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = عدم;
}

صحيح
_PyTokenizer_Traverse(بنية tok_state *tok, visitproc ویکھو, فراغ *arg)
{
    Py_ویکھو(tok->filename);
    Py_ویکھو(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_ویکھو(reader->readline);
    Py_ویکھو(reader->decoder);
    لكل (صحيح i = 0;
         i < (صحيح)Py_ARRAY_لمائی(reader->prefetched_سطریں); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_سطریں[i];
        إذا (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_ویکھو(chunk->owner);
        }
    }
    أرجع 0;
}

صحيح
Py_Tok_Reserve_بفر(حرف **بفر, Py_ssize_t *سمرتھا, Py_ssize_t needed,
                     Py_ssize_t initial_سمرتھا)
{
    أكد(initial_سمرتھا > 0);
    إذا (needed <= *سمرتھا) {
        أرجع 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *سمرتھا > 0 ? *سمرتھا : initial_سمرتھا;
    طالما (cap < needed) {
        إذا (cap > PY_SSIZE_T_MAX / READER_بفر_GROWTH_ضارب) {
            cap = needed;
            اكسر;
        }
        cap *= READER_بفر_GROWTH_ضارب;
    }
    حرف *resized = PyMem_Realloc(*بفر, cap);
    إذا (resized == عدم) {
        Py_Err_No_حافظہ();
        أرجع -1;
    }
    *بفر = resized;
    *سمرتھا = cap;
    أرجع 0;
}

ساكن صحيح
append_decoded(_PyTok_Reader *reader, ثابت حرف *انکڑے, Py_ssize_t len)
{
    إذا (len == 0) {
        أرجع 0;
    }
    إذا (reader->decoded_pos > 0) {
        Py_ssize_t بچیا = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (ناپ_t)بچیا);
        reader->decoded_جانچ -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = بچیا;
    }
    إذا (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            Py_Tok_Reserve_بفر(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        Py_Err_No_حافظہ();
        أرجع -1;
    }
    حافظہ_نقل(reader->decoded + reader->decoded_len, انکڑے, (ناپ_t)len);
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
    reader->decoded_پوچھ_is_implicit = 1;
    أرجع 0;
}

ساكن صحيح
نکالو_decoded_سطر(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    أكد(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    أكد(reader->decoded_جانچ >= reader->decoded_pos &&
           reader->decoded_جانچ <= reader->decoded_len);
    إذا (reader->decoded_pos == reader->decoded_len) {
        أرجع 0;
    }
    حرف *شروع = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    حرف *newline = memchr(reader->decoded + reader->decoded_جانچ, '\n',
                           reader->decoded_len - reader->decoded_جانچ);
    إذا (newline == عدم) {
        reader->decoded_جانچ = reader->decoded_len;
        أرجع 0;
    }
    Py_ssize_t len = newline - شروع + 1;
    chunk->انکڑے = شروع;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_جانچ = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_پوچھ_is_implicit;
    إذا (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_جانچ = 0;
        reader->decoded_پوچھ_is_implicit = 0;
    }
    أرجع 1;
}

ساكن صحيح
chunk_is_سطر(ثابت _PyTok_Chunk *chunk)
{
    إذا (chunk->len == 0 || chunk->انکڑے[chunk->len - 1] != '\n') {
        أرجع 0;
    }
    أرجع memchr(chunk->انکڑے, '\n', chunk->len - 1) == عدم;
}

ساكن Py_Tok_Read_نتیجہ
اگلا_prepared(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    إذا (tok->lineno >= tok->سروت.nlines) {
        أرجع _PYTOK_READ_EOF;
    }
    Py_Tok_وسعت پوچھ = {tok->inp, tok->سروت.ادھار_وتھ + tok->سروت.len};
    _PyTok_Off newline = Py_Tok_سروت_لبھو_Byte(&tok->سروت, پوچھ, '\n');
    Py_Tok_وسعت سطر = {پوچھ.شروع, newline >= 0 ? newline + 1 : پوچھ.سرا};
    chunk->انکڑے = (حرف *)Py_Tok_سروت_وسعت_View(
        &tok->سروت, سطر, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = سطر.سرا == پوچھ.سرا
        ? tok->reader->prepared_final_newline : 0;
    أرجع PYTOK_READ_سطر;
}

ساكن Py_Tok_Read_نتیجہ
read_file_سطر(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    لكل (;;) {
        إذا (len > PY_SSIZE_T_MAX - BUFSIZ) {
            Py_Err_No_حافظہ();
            أرجع _PYTOK_READ_ERROR;
        }
        إذا (Py_Tok_Reserve_بفر(&reader->file_بفر, &reader->file_بفر_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            أرجع _PYTOK_READ_ERROR;
        }
        صحيح دستیاب = (صحيح)Py_MIN(reader->file_بفر_cap - len, ودھ_توں_ودھ_عدد);
        ناپ_t اقرأ = 0;
        حرف *نتیجہ = Py_Universal_Newline_Fgets_With_ناپ(
            reader->file_بفر + len, دستیاب, reader->fp, عدم, &اقرأ);
        إذا (نتیجہ == عدم) {
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
    chunk->انکڑے = reader->file_بفر;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    أرجع PYTOK_READ_سطر;
}

ساكن صحيح
initialize_file(بنية tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    إذا (reader->encoding != عدم) {
        أرجع Py_Tok_شروع_Decoder(tok, "strict");
    }

    Py_Tok_Read_نتیجہ نتیجہ = read_file_سطر(
        tok, &reader->prefetched_سطریں[0]);
    إذا (نتیجہ == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        أرجع 0;
    }
    إذا (نتیجہ != PYTOK_READ_سطر) {
        أرجع -1;
    }
    Py_ssize_t bom_len;
    Py_Tok_Encoding_نتیجہ detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_سطریں[0], عدم, 0, &bom_len);
    إذا (detection == _PYTOK_ENCODING_ERROR) {
        أرجع -1;
    }
    إذا (detection == PYTOK_ENCODING_NEED_دوجا_سطر) {
        حرف *پہلا = Py_Tok_Copy_بائٹاں(
            reader->prefetched_سطریں[0].انکڑے,
            reader->prefetched_سطریں[0].len);
        إذا (پہلا == عدم) {
            tok->پورا = E_NOMEM;
            أرجع -1;
        }
        reader->prefetched_سطریں[0].انکڑے = پہلا;
        reader->prefetched_سطریں[0].ownership = _PYTOK_CHUNK_PYMEM;
        نتیجہ = read_file_سطر(tok, &reader->prefetched_سطریں[1]);
        إذا (نتیجہ == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        وإلا إذا (نتیجہ != PYTOK_READ_سطر) {
            أرجع -1;
        }
        _PyTok_Chunk *دوجا = reader->prefetched_سطریں[1].انکڑے != عدم
            ? &reader->prefetched_سطریں[1] : عدم;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_سطریں[0], دوجا, 1, &bom_len);
        إذا (detection == _PYTOK_ENCODING_ERROR) {
            أرجع -1;
        }
    }
    إذا (bom_len != 0) {
        _PyTok_Chunk *پہلا = &reader->prefetched_سطریں[0];
        إذا (پہلا->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(پہلا->انکڑے, پہلا->انکڑے + bom_len,
                    (ناپ_t)(پہلا->len - bom_len));
            پہلا->انکڑے[پہلا->len - bom_len] = '\0';
        }
        وإلا {
            پہلا->انکڑے += bom_len;
        }
        پہلا->len -= bom_len;
    }
    إذا (Py_Tok_شروع_Decoder(tok, "strict") < 0) {
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
        _PyTok_Chunk داخلہ = {
            .انکڑے = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        صحيح decoded = _PyTok_DecodeChunk(tok, &داخلہ, 1);
        إذا (decoded == 0 &&
                append_decoded(reader, داخلہ.انکڑے, داخلہ.len) < 0) {
            tok->پورا = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&داخلہ);
        إذا (decoded < 0) {
            أرجع -1;
        }
    }
    إذا (append_implicit_newline(reader) < 0) {
        tok->پورا = E_NOMEM;
        أرجع -1;
    }
    أرجع 0;
}

ساكن Py_Tok_Read_نتیجہ
اگلا_file(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    إذا (!reader->file_initialized && initialize_file(tok) < 0) {
        أرجع _PYTOK_READ_ERROR;
    }
    لكل (;;) {
        إذا (نکالو_decoded_سطر(reader, chunk)) {
            أرجع PYTOK_READ_سطر;
        }
        _PyTok_Chunk داخلہ = {0};
        إذا (reader->prefetched_سطریں[0].انکڑے != عدم) {
            داخلہ = reader->prefetched_سطریں[0];
            reader->prefetched_سطریں[0] = (_PyTok_Chunk){0};
        }
        وإلا إذا (reader->prefetched_سطریں[1].انکڑے != عدم) {
            داخلہ = reader->prefetched_سطریں[1];
            reader->prefetched_سطریں[1] = (_PyTok_Chunk){0};
        }
        وإلا إذا (!reader->file_eof) {
            Py_Tok_Read_نتیجہ نتیجہ = read_file_سطر(tok, &داخلہ);
            إذا (نتیجہ == _PYTOK_READ_ERROR) {
                أرجع نتیجہ;
            }
            إذا (نتیجہ == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        إذا (داخلہ.انکڑے != عدم) {
            صحيح implicit = داخلہ.implicit_newline;
            إذا (reader->decoder == عدم && !implicit) {
                *chunk = داخلہ;
                أرجع PYTOK_READ_سطر;
            }
            صحيح decoded = _PyTok_DecodeChunk(tok, &داخلہ, 0);
            إذا (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_سطر(&داخلہ)) {
                *chunk = داخلہ;
                أرجع PYTOK_READ_سطر;
            }
            إذا (decoded == 0 &&
                    append_decoded(reader, داخلہ.انکڑے, داخلہ.len) < 0) {
                tok->پورا = E_NOMEM;
                decoded = -1;
            }
            إذا (decoded == 0 && implicit) {
                reader->decoded_پوچھ_is_implicit = 1;
            }
            _PyTok_ChunkClear(&داخلہ);
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

ساكن Py_Tok_Read_نتیجہ
اگلا_readline(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    لكل (;;) {
        إذا (نکالو_decoded_سطر(reader, chunk)) {
            أرجع PYTOK_READ_سطر;
        }
        إذا (reader->decoder_finalized) {
            أرجع _PYTOK_READ_EOF;
        }

        PyObject *raw = Py_Object_Call_No_دلیلاں(reader->readline);
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

        _PyTok_Chunk داخلہ = {0};
        إذا (reader->encoding != عدم) {
            إذا (!Py_بائٹاں_Check(raw)) {
                Py_Err_سیٹ_String(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                أرجع _PYTOK_READ_ERROR;
            }
            إذا (Py_بائٹاں_GET_ناپ(raw) == 0) {
                Py_DECREF(raw);
                إذا (Py_Tok_شروع_Decoder(tok, "replace") < 0) {
                    أرجع _PYTOK_READ_ERROR;
                }
                إذا (finalize_decoding(tok) < 0) {
                    أرجع _PYTOK_READ_ERROR;
                }
                استمر;
            }
            داخلہ.owner = raw;
            داخلہ.انکڑے = Py_بائٹاں_AS_STRING(raw);
            داخلہ.len = Py_بائٹاں_GET_ناپ(raw);
            داخلہ.ownership = _PYTOK_CHUNK_PYOBJECT;
            صحيح decoded;
            إذا (reader->decoder == عدم &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_سطر(&داخلہ)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &داخلہ, "utf-8", "replace");
            }
            وإلا {
                decoded = Py_Tok_شروع_Decoder(tok, "replace");
                إذا (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &داخلہ, 0);
                }
            }
            إذا (decoded < 0) {
                _PyTok_ChunkClear(&داخلہ);
                أرجع _PYTOK_READ_ERROR;
            }
        }
        وإلا {
            إذا (!PyUnicode_Check(raw)) {
                Py_Err_سیٹ_String(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                أرجع _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            ثابت حرف *utf8 = Py_Unicode_As_UTF8_And_ناپ(raw, &utf8_len);
            إذا (utf8 == عدم) {
                Py_DECREF(raw);
                أرجع _PYTOK_READ_ERROR;
            }
            داخلہ.owner = raw;
            داخلہ.انکڑے = (حرف *)utf8;
            داخلہ.len = utf8_len;
            داخلہ.ownership = _PYTOK_CHUNK_PYOBJECT;
            إذا (داخلہ.len == 0) {
                _PyTok_ChunkClear(&داخلہ);
                إذا (finalize_decoding(tok) < 0) {
                    أرجع _PYTOK_READ_ERROR;
                }
                استمر;
            }
        }

        إذا (reader->decoded_pos == reader->decoded_len &&
                chunk_is_سطر(&داخلہ)) {
            *chunk = داخلہ;
            أرجع PYTOK_READ_سطر;
        }

        إذا (append_decoded(reader, داخلہ.انکڑے, داخلہ.len) < 0) {
            _PyTok_ChunkClear(&داخلہ);
            tok->پورا = E_NOMEM;
            أرجع _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&داخلہ);
        إذا (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            صحيح pending = Py_Tok_Decoder_Has_Buffered_داخلہ(tok);
            إذا (pending < 0) {
                أرجع _PYTOK_READ_ERROR;
            }
            إذا (!pending && append_implicit_newline(reader) < 0) {
                tok->پورا = E_NOMEM;
                أرجع _PYTOK_READ_ERROR;
            }
        }
    }
}

ساكن Py_Tok_Read_نتیجہ
اگلا_interactive(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    إذا (reader->stop_interactive) {
        أرجع _PYTOK_READ_STOPPED;
    }
    حرف *داخلہ = PyOS_Readline(
        reader->fp != عدم ? reader->fp : stdin, stdout, reader->prompt);
    إذا (reader->nextprompt != عدم) {
        reader->prompt = reader->nextprompt;
    }
    إذا (داخلہ == عدم) {
        أرجع PYTOK_READ_رکاوٹ;
    }
    Py_ssize_t len = لڑی_لمبائی(داخلہ);
    إذا (len == 0) {
        PyMem_Free(داخلہ);
        أرجع _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .انکڑے = داخلہ,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    إذا (reader->encoding != عدم &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, عدم) < 0) {
        _PyTok_ChunkClear(&decoded);
        أرجع _PYTOK_READ_ERROR;
    }
    إذا (memchr(decoded.انکڑے, '\r', decoded.len) == عدم) {
        *chunk = decoded;
        أرجع PYTOK_READ_سطر;
    }
    chunk->انکڑے = _PyTok_NormalizeNewlines(
        decoded.انکڑے, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    إذا (chunk->انکڑے == عدم) {
        tok->پورا = E_NOMEM;
        أرجع _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    أرجع PYTOK_READ_سطر;
}

صحيح
_PyTok_ReaderIsInteractive(ثابت بنية tok_state *tok)
{
    أرجع tok->reader->قسم == _PYTOK_READER_INTERACTIVE;
}

ثابت حرف *
Py_Tokenizer_Retained_سروت(ثابت بنية tok_state *tok)
{
    إذا (tok->reader->قسم == _PYTOK_READER_PREPARED) {
        أرجع Py_Tok_سروت_انکڑے(&tok->سروت);
    }
    إذا (tok->reader->قسم == _PYTOK_READER_INTERACTIVE) {
        أرجع tok->سروت.بائٹاں;
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

ساكن Py_Tok_Read_نتیجہ
reader_اگلا(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    بدل (tok->reader->قسم) {
        حالة _PYTOK_READER_PREPARED:
            أرجع اگلا_prepared(tok, chunk);
        حالة _PYTOK_READER_FILE:
            أرجع اگلا_file(tok, chunk);
        حالة _PYTOK_READER_READLINE:
            أرجع اگلا_readline(tok, chunk);
        حالة _PYTOK_READER_INTERACTIVE:
            أرجع اگلا_interactive(tok, chunk);
    }
    Py_UNREACHABLE();
}

ساكن فراغ
reset_streaming_بفر(بنية tok_state *tok)
{
    أكد(tok->cur >= tok->buf_وتھ && tok->cur <= tok->inp);
    Py_Tok_سروت_Discard(&tok->سروت);
    tok->buf_وتھ = tok->سروت.ادھار_وتھ;
    tok->cur = tok->inp = tok->سروت.ادھار_وتھ;
    tok->سطر_شروع = tok->buf_وتھ;
}

صحيح
_PyTok_ReaderUnderflow(بنية tok_state *tok)
{
    أكد(tok->cur >= tok->buf_وتھ && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    Py_Tok_Reader_قسم قسم = reader->قسم;
    صحيح prepared = قسم == _PYTOK_READER_PREPARED;
    صحيح streaming = reader_is_streaming(قسم);
    صحيح reset_بفر = tok->شروع < 0 &&
        _PyLexer_CurrentFTString(tok) == عدم;

    _PyTok_Chunk chunk;
    Py_Tok_Read_نتیجہ نتیجہ = reader_اگلا(tok, &chunk);
    إذا (نتیجہ != PYTOK_READ_سطر) {
        إذا (reset_بفر && streaming) {
            reset_streaming_بفر(tok);
        }
        إذا (نتیجہ == _PYTOK_READ_EOF) {
            tok->پورا = E_EOF;
        }
        وإلا إذا (نتیجہ == _PYTOK_READ_STOPPED) {
            tok->پورا = E_INTERACT_STOP;
        }
        وإلا إذا (نتیجہ == PYTOK_READ_رکاوٹ) {
            tok->پورا = E_INTR;
        }
        وإلا {
            إذا (tok->پورا == E_OK) {
                tok->پورا = PyErr_ExceptionMatches(Py_Exc_حافظہ_Error)
                    ? E_NOMEM : E_ERROR;
            }
        }
        إذا (قسم == _PYTOK_READER_INTERACTIVE &&
                نتیجہ != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        أرجع 0;
    }
    إذا (tok->lineno == ودھ_توں_ودھ_عدد) {
        Py_Err_سیٹ_String(Py_Exc_حد_توں_ودھ_Error, "too many tokenizer source lines");
        tok->پورا = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        أرجع 0;
    }

    إذا (!prepared) {
        إذا (streaming && reset_بفر) {
            reset_streaming_بفر(tok);
        }
        _PyTok_Off سروت_شروع = Py_Tok_سروت_Append_سطر(
            &tok->سروت, chunk.انکڑے, chunk.len);
        إذا (سروت_شروع < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->پورا = PyErr_ExceptionMatches(Py_Exc_حافظہ_Error)
                ? E_NOMEM : E_ERROR;
            أرجع 0;
        }
        إذا (reset_بفر) {
            tok->cur = سروت_شروع;
            tok->buf_وتھ = سروت_شروع;
            tok->سطر_شروع = tok->buf_وتھ;
        }
        tok->inp = سروت_شروع + chunk.len;
    }
    وإلا {
        إذا (reset_بفر) {
            tok->buf_وتھ = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    إذا (قسم == _PYTOK_READER_FILE &&
            (reader->encoding == عدم ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                Py_Tok_سروت_اشارہ_گر(&tok->سروت, tok->cur),
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
        Py_Err_No_حافظہ();
        أرجع عدم;
    }
    tok->شروع = tok->سطر_شروع = -1;
    Py_Tok_سروت_Init(&tok->سروت);
    tok->پورا = E_OK;
    tok->layout.at_bol = 1;
    tok->شروع_loc = (_PyTok_Loc){-1, -1};
    tok->ftstring_ڈھیر = tok->ftstring_ڈھیر_inline;
    tok->ftstring_سمرتھا = FTSTRING_ڈھیر_INLINE_سمرتھا;
#جے_تعریف_شدہ Py_DEBUG
    tok->debug = _Py_GetConfig()->parser_debug;
#جے_انت
    tok->reader = PyMem_Calloc(1, حجم(*tok->reader));
    إذا (tok->reader == عدم) {
        Py_Err_No_حافظہ();
        _PyTokenizer_Free(tok);
        أرجع عدم;
    }
    tok->reader->قسم = قسم;
    إذا (قسم == _PYTOK_READER_PREPARED) {
        أرجع tok;
    }
    إذا (reader_is_streaming(قسم)) {
        tok->cur = tok->inp = tok->سروت.ادھار_وتھ;
        tok->سطر_شروع = tok->buf_وتھ;
    }
    أرجع tok;
}

ساكن بنية tok_state *
tokenizer_from_string(ثابت حرف *داخلہ, صحيح utf8_only, صحيح exec_داخلہ,
                      صحيح preserve_crlf)
{
    بنية tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    إذا (tok == عدم) {
        أرجع عدم;
    }
    إذا (_PyTok_PrepareString(
            tok, داخلہ, utf8_only, exec_داخلہ, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        أرجع عدم;
    }
    tok->cur = tok->inp = tok->سروت.ادھار_وتھ;
    tok->سطر_شروع = tok->سروت.ادھار_وتھ;
    أرجع tok;
}

بنية tok_state *
_PyTokenizer_FromString(ثابت حرف *داخلہ, صحيح exec_داخلہ, صحيح preserve_crlf)
{
    أرجع tokenizer_from_string(داخلہ, 0, exec_داخلہ, preserve_crlf);
}

بنية tok_state *
_PyTokenizer_FromUTF8(ثابت حرف *داخلہ, صحيح exec_داخلہ, صحيح preserve_crlf)
{
    أرجع tokenizer_from_string(داخلہ, 1, exec_داخلہ, preserve_crlf);
}

بنية tok_state *
_PyTokenizer_FromReadline(PyObject *readline, ثابت حرف *encoding)
{
    بنية tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    إذا (tok == عدم) {
        أرجع عدم;
    }
    إذا (encoding != عدم && Py_Tok_سیٹ_Encoding(tok, encoding) < 0) {
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
    إذا (encoding != عدم && Py_Tok_سیٹ_Encoding(tok, encoding) < 0) {
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
borrow_read(فراغ *cookie, حرف *بفر, ناپ_t ناپ)
{
    borrowed_fd borrowed = {.cookie = cookie};
    أرجع اقرأ(borrowed.fd, بفر, ناپ);
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
#جے_انت

حرف *
Py_Tokenizer_لبھو_Encoding_Filename(صحيح fd, PyObject *filename)
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
