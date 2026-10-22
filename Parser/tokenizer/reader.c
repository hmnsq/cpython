#ضمن "Python.h"
#ضمن "pycore_fileutils.h"
#ضمن "pycore_pystate.h"
#ضمن "pycore_runtime.h"

#ضمن "errcode.h"
#ضمن "helpers.h"
#ضمن "reader.h"
#ضمن "reader_internal.h"
#ضمن "../lexer/state.h"

#تعریف_کن READER_میانگیر_GROWTH_ضریب 2

#اگر_تعریف_شده HAVE_UNISTD_H
#  include <unistd.h>
#پایان_اگر

ساكن مضمن صحيح
reader_is_streaming(Py_Tok_Reader_نوع نوع)
{
    أرجع نوع == _PYTOK_READER_FILE || نوع == _PYTOK_READER_READLINE;
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
         i < (صحيح)Py_ARRAY_درازی(reader->prefetched_سطرها); i++) {
        _PyTok_ChunkClear(&reader->prefetched_سطرها[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_میانگیر);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = عدم;
}

صحيح
_PyTokenizer_Traverse(بنية tok_state *tok, visitproc بازدید, فراغ *arg)
{
    Py_بازدید(tok->filename);
    Py_بازدید(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_بازدید(reader->readline);
    Py_بازدید(reader->decoder);
    لكل (صحيح i = 0;
         i < (صحيح)Py_ARRAY_درازی(reader->prefetched_سطرها); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_سطرها[i];
        إذا (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_بازدید(chunk->owner);
        }
    }
    أرجع 0;
}

صحيح
Py_Tok_Reserve_میانگیر(حرف **میانگیر, Py_ssize_t *ظرفیت, Py_ssize_t needed,
                     Py_ssize_t initial_ظرفیت)
{
    أكد(initial_ظرفیت > 0);
    إذا (needed <= *ظرفیت) {
        أرجع 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *ظرفیت > 0 ? *ظرفیت : initial_ظرفیت;
    طالما (cap < needed) {
        إذا (cap > PY_SSIZE_T_MAX / READER_میانگیر_GROWTH_ضریب) {
            cap = needed;
            اكسر;
        }
        cap *= READER_میانگیر_GROWTH_ضریب;
    }
    حرف *resized = PyMem_Realloc(*میانگیر, cap);
    إذا (resized == عدم) {
        Py_Err_No_حافظه();
        أرجع -1;
    }
    *میانگیر = resized;
    *ظرفیت = cap;
    أرجع 0;
}

ساكن صحيح
append_decoded(_PyTok_Reader *reader, ثابت حرف *داده, Py_ssize_t len)
{
    إذا (len == 0) {
        أرجع 0;
    }
    إذا (reader->decoded_pos > 0) {
        Py_ssize_t مانده = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (اندازه_t)مانده);
        reader->decoded_پیمایش -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = مانده;
    }
    إذا (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            Py_Tok_Reserve_میانگیر(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        Py_Err_No_حافظه();
        أرجع -1;
    }
    کپی_حافظه(reader->decoded + reader->decoded_len, داده, (اندازه_t)len);
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
    reader->decoded_دم_is_implicit = 1;
    أرجع 0;
}

ساكن صحيح
برداشت_decoded_سطر(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    أكد(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    أكد(reader->decoded_پیمایش >= reader->decoded_pos &&
           reader->decoded_پیمایش <= reader->decoded_len);
    إذا (reader->decoded_pos == reader->decoded_len) {
        أرجع 0;
    }
    حرف *شروع = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    حرف *newline = memchr(reader->decoded + reader->decoded_پیمایش, '\n',
                           reader->decoded_len - reader->decoded_پیمایش);
    إذا (newline == عدم) {
        reader->decoded_پیمایش = reader->decoded_len;
        أرجع 0;
    }
    Py_ssize_t len = newline - شروع + 1;
    chunk->داده = شروع;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_پیمایش = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_دم_is_implicit;
    إذا (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_پیمایش = 0;
        reader->decoded_دم_is_implicit = 0;
    }
    أرجع 1;
}

ساكن صحيح
chunk_is_سطر(ثابت _PyTok_Chunk *chunk)
{
    إذا (chunk->len == 0 || chunk->داده[chunk->len - 1] != '\n') {
        أرجع 0;
    }
    أرجع memchr(chunk->داده, '\n', chunk->len - 1) == عدم;
}

ساكن Py_Tok_Read_نتیجه
بعدی_prepared(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    إذا (tok->lineno >= tok->منبع.nlines) {
        أرجع _PYTOK_READ_EOF;
    }
    Py_Tok_بازه دم = {tok->inp, tok->منبع.پایه_جابه_جایی + tok->منبع.len};
    _PyTok_Off newline = Py_Tok_منبع_پیدا_کن_Byte(&tok->منبع, دم, '\n');
    Py_Tok_بازه سطر = {دم.شروع, newline >= 0 ? newline + 1 : دم.انتها};
    chunk->داده = (حرف *)Py_Tok_منبع_بازه_View(
        &tok->منبع, سطر, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = سطر.انتها == دم.انتها
        ? tok->reader->prepared_final_newline : 0;
    أرجع PYTOK_READ_سطر;
}

ساكن Py_Tok_Read_نتیجه
read_file_سطر(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    لكل (;;) {
        إذا (len > PY_SSIZE_T_MAX - BUFSIZ) {
            Py_Err_No_حافظه();
            أرجع _PYTOK_READ_ERROR;
        }
        إذا (Py_Tok_Reserve_میانگیر(&reader->file_میانگیر, &reader->file_میانگیر_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            أرجع _PYTOK_READ_ERROR;
        }
        صحيح موجود = (صحيح)Py_MIN(reader->file_میانگیر_cap - len, بیشترین_صحیح);
        اندازه_t اقرأ = 0;
        حرف *نتیجه = Py_Universal_Newline_Fgets_With_اندازه(
            reader->file_میانگیر + len, موجود, reader->fp, عدم, &اقرأ);
        إذا (نتیجه == عدم) {
            إذا (len == 0) {
                أرجع _PYTOK_READ_EOF;
            }
            اكسر;
        }
        len += (Py_ssize_t)اقرأ;
        إذا (len > 0 && reader->file_میانگیر[len - 1] == '\n') {
            اكسر;
        }
    }
    صحيح implicit = reader->file_میانگیر[len - 1] != '\n';
    chunk->داده = reader->file_میانگیر;
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

    Py_Tok_Read_نتیجه نتیجه = read_file_سطر(
        tok, &reader->prefetched_سطرها[0]);
    إذا (نتیجه == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        أرجع 0;
    }
    إذا (نتیجه != PYTOK_READ_سطر) {
        أرجع -1;
    }
    Py_ssize_t bom_len;
    Py_Tok_Encoding_نتیجه detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_سطرها[0], عدم, 0, &bom_len);
    إذا (detection == _PYTOK_ENCODING_ERROR) {
        أرجع -1;
    }
    إذا (detection == PYTOK_ENCODING_NEED_دومی_سطر) {
        حرف *اولی = Py_Tok_Copy_بایتها(
            reader->prefetched_سطرها[0].داده,
            reader->prefetched_سطرها[0].len);
        إذا (اولی == عدم) {
            tok->انجامشده = E_NOMEM;
            أرجع -1;
        }
        reader->prefetched_سطرها[0].داده = اولی;
        reader->prefetched_سطرها[0].ownership = _PYTOK_CHUNK_PYMEM;
        نتیجه = read_file_سطر(tok, &reader->prefetched_سطرها[1]);
        إذا (نتیجه == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        وإلا إذا (نتیجه != PYTOK_READ_سطر) {
            أرجع -1;
        }
        _PyTok_Chunk *دومی = reader->prefetched_سطرها[1].داده != عدم
            ? &reader->prefetched_سطرها[1] : عدم;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_سطرها[0], دومی, 1, &bom_len);
        إذا (detection == _PYTOK_ENCODING_ERROR) {
            أرجع -1;
        }
    }
    إذا (bom_len != 0) {
        _PyTok_Chunk *اولی = &reader->prefetched_سطرها[0];
        إذا (اولی->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(اولی->داده, اولی->داده + bom_len,
                    (اندازه_t)(اولی->len - bom_len));
            اولی->داده[اولی->len - bom_len] = '\0';
        }
        وإلا {
            اولی->داده += bom_len;
        }
        اولی->len -= bom_len;
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
        _PyTok_Chunk درون_داد = {
            .داده = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        صحيح decoded = _PyTok_DecodeChunk(tok, &درون_داد, 1);
        إذا (decoded == 0 &&
                append_decoded(reader, درون_داد.داده, درون_داد.len) < 0) {
            tok->انجامشده = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&درون_داد);
        إذا (decoded < 0) {
            أرجع -1;
        }
    }
    إذا (append_implicit_newline(reader) < 0) {
        tok->انجامشده = E_NOMEM;
        أرجع -1;
    }
    أرجع 0;
}

ساكن Py_Tok_Read_نتیجه
بعدی_file(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    إذا (!reader->file_initialized && initialize_file(tok) < 0) {
        أرجع _PYTOK_READ_ERROR;
    }
    لكل (;;) {
        إذا (برداشت_decoded_سطر(reader, chunk)) {
            أرجع PYTOK_READ_سطر;
        }
        _PyTok_Chunk درون_داد = {0};
        إذا (reader->prefetched_سطرها[0].داده != عدم) {
            درون_داد = reader->prefetched_سطرها[0];
            reader->prefetched_سطرها[0] = (_PyTok_Chunk){0};
        }
        وإلا إذا (reader->prefetched_سطرها[1].داده != عدم) {
            درون_داد = reader->prefetched_سطرها[1];
            reader->prefetched_سطرها[1] = (_PyTok_Chunk){0};
        }
        وإلا إذا (!reader->file_eof) {
            Py_Tok_Read_نتیجه نتیجه = read_file_سطر(tok, &درون_داد);
            إذا (نتیجه == _PYTOK_READ_ERROR) {
                أرجع نتیجه;
            }
            إذا (نتیجه == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        إذا (درون_داد.داده != عدم) {
            صحيح implicit = درون_داد.implicit_newline;
            إذا (reader->decoder == عدم && !implicit) {
                *chunk = درون_داد;
                أرجع PYTOK_READ_سطر;
            }
            صحيح decoded = _PyTok_DecodeChunk(tok, &درون_داد, 0);
            إذا (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_سطر(&درون_داد)) {
                *chunk = درون_داد;
                أرجع PYTOK_READ_سطر;
            }
            إذا (decoded == 0 &&
                    append_decoded(reader, درون_داد.داده, درون_داد.len) < 0) {
                tok->انجامشده = E_NOMEM;
                decoded = -1;
            }
            إذا (decoded == 0 && implicit) {
                reader->decoded_دم_is_implicit = 1;
            }
            _PyTok_ChunkClear(&درون_داد);
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

ساكن Py_Tok_Read_نتیجه
بعدی_readline(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    لكل (;;) {
        إذا (برداشت_decoded_سطر(reader, chunk)) {
            أرجع PYTOK_READ_سطر;
        }
        إذا (reader->decoder_finalized) {
            أرجع _PYTOK_READ_EOF;
        }

        PyObject *raw = Py_Object_Call_No_آرگومانها(reader->readline);
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

        _PyTok_Chunk درون_داد = {0};
        إذا (reader->encoding != عدم) {
            إذا (!Py_بایتها_Check(raw)) {
                Py_Err_مجموعه_String(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                أرجع _PYTOK_READ_ERROR;
            }
            إذا (Py_بایتها_GET_اندازه(raw) == 0) {
                Py_DECREF(raw);
                إذا (Py_Tok_شروع_Decoder(tok, "replace") < 0) {
                    أرجع _PYTOK_READ_ERROR;
                }
                إذا (finalize_decoding(tok) < 0) {
                    أرجع _PYTOK_READ_ERROR;
                }
                استمر;
            }
            درون_داد.owner = raw;
            درون_داد.داده = Py_بایتها_AS_STRING(raw);
            درون_داد.len = Py_بایتها_GET_اندازه(raw);
            درون_داد.ownership = _PYTOK_CHUNK_PYOBJECT;
            صحيح decoded;
            إذا (reader->decoder == عدم &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_سطر(&درون_داد)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &درون_داد, "utf-8", "replace");
            }
            وإلا {
                decoded = Py_Tok_شروع_Decoder(tok, "replace");
                إذا (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &درون_داد, 0);
                }
            }
            إذا (decoded < 0) {
                _PyTok_ChunkClear(&درون_داد);
                أرجع _PYTOK_READ_ERROR;
            }
        }
        وإلا {
            إذا (!PyUnicode_Check(raw)) {
                Py_Err_مجموعه_String(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                أرجع _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            ثابت حرف *utf8 = Py_Unicode_As_UTF8_And_اندازه(raw, &utf8_len);
            إذا (utf8 == عدم) {
                Py_DECREF(raw);
                أرجع _PYTOK_READ_ERROR;
            }
            درون_داد.owner = raw;
            درون_داد.داده = (حرف *)utf8;
            درون_داد.len = utf8_len;
            درون_داد.ownership = _PYTOK_CHUNK_PYOBJECT;
            إذا (درون_داد.len == 0) {
                _PyTok_ChunkClear(&درون_داد);
                إذا (finalize_decoding(tok) < 0) {
                    أرجع _PYTOK_READ_ERROR;
                }
                استمر;
            }
        }

        إذا (reader->decoded_pos == reader->decoded_len &&
                chunk_is_سطر(&درون_داد)) {
            *chunk = درون_داد;
            أرجع PYTOK_READ_سطر;
        }

        إذا (append_decoded(reader, درون_داد.داده, درون_داد.len) < 0) {
            _PyTok_ChunkClear(&درون_داد);
            tok->انجامشده = E_NOMEM;
            أرجع _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&درون_داد);
        إذا (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            صحيح pending = Py_Tok_Decoder_Has_Buffered_درون_داد(tok);
            إذا (pending < 0) {
                أرجع _PYTOK_READ_ERROR;
            }
            إذا (!pending && append_implicit_newline(reader) < 0) {
                tok->انجامشده = E_NOMEM;
                أرجع _PYTOK_READ_ERROR;
            }
        }
    }
}

ساكن Py_Tok_Read_نتیجه
بعدی_interactive(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    إذا (reader->stop_interactive) {
        أرجع _PYTOK_READ_STOPPED;
    }
    حرف *درون_داد = PyOS_Readline(
        reader->fp != عدم ? reader->fp : stdin, stdout, reader->prompt);
    إذا (reader->nextprompt != عدم) {
        reader->prompt = reader->nextprompt;
    }
    إذا (درون_داد == عدم) {
        أرجع PYTOK_READ_وقفه;
    }
    Py_ssize_t len = طول_رشته(درون_داد);
    إذا (len == 0) {
        PyMem_Free(درون_داد);
        أرجع _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .داده = درون_داد,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    إذا (reader->encoding != عدم &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, عدم) < 0) {
        _PyTok_ChunkClear(&decoded);
        أرجع _PYTOK_READ_ERROR;
    }
    إذا (memchr(decoded.داده, '\r', decoded.len) == عدم) {
        *chunk = decoded;
        أرجع PYTOK_READ_سطر;
    }
    chunk->داده = _PyTok_NormalizeNewlines(
        decoded.داده, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    إذا (chunk->داده == عدم) {
        tok->انجامشده = E_NOMEM;
        أرجع _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    أرجع PYTOK_READ_سطر;
}

صحيح
_PyTok_ReaderIsInteractive(ثابت بنية tok_state *tok)
{
    أرجع tok->reader->نوع == _PYTOK_READER_INTERACTIVE;
}

ثابت حرف *
Py_Tokenizer_Retained_منبع(ثابت بنية tok_state *tok)
{
    إذا (tok->reader->نوع == _PYTOK_READER_PREPARED) {
        أرجع Py_Tok_منبع_داده(&tok->منبع);
    }
    إذا (tok->reader->نوع == _PYTOK_READER_INTERACTIVE) {
        أرجع tok->منبع.بایتها;
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

ساكن Py_Tok_Read_نتیجه
reader_بعدی(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    بدل (tok->reader->نوع) {
        حالة _PYTOK_READER_PREPARED:
            أرجع بعدی_prepared(tok, chunk);
        حالة _PYTOK_READER_FILE:
            أرجع بعدی_file(tok, chunk);
        حالة _PYTOK_READER_READLINE:
            أرجع بعدی_readline(tok, chunk);
        حالة _PYTOK_READER_INTERACTIVE:
            أرجع بعدی_interactive(tok, chunk);
    }
    Py_UNREACHABLE();
}

ساكن فراغ
reset_streaming_میانگیر(بنية tok_state *tok)
{
    أكد(tok->cur >= tok->buf_جابه_جایی && tok->cur <= tok->inp);
    Py_Tok_منبع_Discard(&tok->منبع);
    tok->buf_جابه_جایی = tok->منبع.پایه_جابه_جایی;
    tok->cur = tok->inp = tok->منبع.پایه_جابه_جایی;
    tok->سطر_شروع = tok->buf_جابه_جایی;
}

صحيح
_PyTok_ReaderUnderflow(بنية tok_state *tok)
{
    أكد(tok->cur >= tok->buf_جابه_جایی && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    Py_Tok_Reader_نوع نوع = reader->نوع;
    صحيح prepared = نوع == _PYTOK_READER_PREPARED;
    صحيح streaming = reader_is_streaming(نوع);
    صحيح reset_میانگیر = tok->شروع < 0 &&
        _PyLexer_CurrentFTString(tok) == عدم;

    _PyTok_Chunk chunk;
    Py_Tok_Read_نتیجه نتیجه = reader_بعدی(tok, &chunk);
    إذا (نتیجه != PYTOK_READ_سطر) {
        إذا (reset_میانگیر && streaming) {
            reset_streaming_میانگیر(tok);
        }
        إذا (نتیجه == _PYTOK_READ_EOF) {
            tok->انجامشده = E_EOF;
        }
        وإلا إذا (نتیجه == _PYTOK_READ_STOPPED) {
            tok->انجامشده = E_INTERACT_STOP;
        }
        وإلا إذا (نتیجه == PYTOK_READ_وقفه) {
            tok->انجامشده = E_INTR;
        }
        وإلا {
            إذا (tok->انجامشده == E_OK) {
                tok->انجامشده = PyErr_ExceptionMatches(Py_Exc_حافظه_Error)
                    ? E_NOMEM : E_ERROR;
            }
        }
        إذا (نوع == _PYTOK_READER_INTERACTIVE &&
                نتیجه != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        أرجع 0;
    }
    إذا (tok->lineno == بیشترین_صحیح) {
        Py_Err_مجموعه_String(Py_Exc_سرریز_Error, "too many tokenizer source lines");
        tok->انجامشده = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        أرجع 0;
    }

    إذا (!prepared) {
        إذا (streaming && reset_میانگیر) {
            reset_streaming_میانگیر(tok);
        }
        _PyTok_Off منبع_شروع = Py_Tok_منبع_Append_سطر(
            &tok->منبع, chunk.داده, chunk.len);
        إذا (منبع_شروع < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->انجامشده = PyErr_ExceptionMatches(Py_Exc_حافظه_Error)
                ? E_NOMEM : E_ERROR;
            أرجع 0;
        }
        إذا (reset_میانگیر) {
            tok->cur = منبع_شروع;
            tok->buf_جابه_جایی = منبع_شروع;
            tok->سطر_شروع = tok->buf_جابه_جایی;
        }
        tok->inp = منبع_شروع + chunk.len;
    }
    وإلا {
        إذا (reset_میانگیر) {
            tok->buf_جابه_جایی = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    إذا (نوع == _PYTOK_READER_FILE &&
            (reader->encoding == عدم ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                Py_Tok_منبع_اشاره_گر(&tok->منبع, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        أرجع 0;
    }
    _PyTok_ChunkClear(&chunk);
    أرجع 1;
}

ساكن بنية tok_state *
tokenizer_new_with_reader(Py_Tok_Reader_نوع نوع)
{
    بنية tok_state *tok = PyMem_Calloc(1, حجم(*tok));
    إذا (tok == عدم) {
        Py_Err_No_حافظه();
        أرجع عدم;
    }
    tok->شروع = tok->سطر_شروع = -1;
    Py_Tok_منبع_Init(&tok->منبع);
    tok->انجامشده = E_OK;
    tok->layout.at_bol = 1;
    tok->شروع_loc = (_PyTok_Loc){-1, -1};
    tok->ftstring_پشته = tok->ftstring_پشته_inline;
    tok->ftstring_ظرفیت = FTSTRING_پشته_INLINE_ظرفیت;
#اگر_تعریف_شده Py_DEBUG
    tok->debug = _Py_GetConfig()->parser_debug;
#پایان_اگر
    tok->reader = PyMem_Calloc(1, حجم(*tok->reader));
    إذا (tok->reader == عدم) {
        Py_Err_No_حافظه();
        _PyTokenizer_Free(tok);
        أرجع عدم;
    }
    tok->reader->نوع = نوع;
    إذا (نوع == _PYTOK_READER_PREPARED) {
        أرجع tok;
    }
    إذا (reader_is_streaming(نوع)) {
        tok->cur = tok->inp = tok->منبع.پایه_جابه_جایی;
        tok->سطر_شروع = tok->buf_جابه_جایی;
    }
    أرجع tok;
}

ساكن بنية tok_state *
tokenizer_from_string(ثابت حرف *درون_داد, صحيح utf8_only, صحيح exec_درون_داد,
                      صحيح preserve_crlf)
{
    بنية tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    إذا (tok == عدم) {
        أرجع عدم;
    }
    إذا (_PyTok_PrepareString(
            tok, درون_داد, utf8_only, exec_درون_داد, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        أرجع عدم;
    }
    tok->cur = tok->inp = tok->منبع.پایه_جابه_جایی;
    tok->سطر_شروع = tok->منبع.پایه_جابه_جایی;
    أرجع tok;
}

بنية tok_state *
_PyTokenizer_FromString(ثابت حرف *درون_داد, صحيح exec_درون_داد, صحيح preserve_crlf)
{
    أرجع tokenizer_from_string(درون_داد, 0, exec_درون_داد, preserve_crlf);
}

بنية tok_state *
_PyTokenizer_FromUTF8(ثابت حرف *درون_داد, صحيح exec_درون_داد, صحيح preserve_crlf)
{
    أرجع tokenizer_from_string(درون_داد, 1, exec_درون_داد, preserve_crlf);
}

بنية tok_state *
_PyTokenizer_FromReadline(PyObject *readline, ثابت حرف *encoding)
{
    بنية tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    إذا (tok == عدم) {
        أرجع عدم;
    }
    إذا (encoding != عدم && Py_Tok_مجموعه_Encoding(tok, encoding) < 0) {
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
    Py_Tok_Reader_نوع نوع = ps1 != عدم || ps2 != عدم
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    بنية tok_state *tok = tokenizer_new_with_reader(نوع);
    إذا (tok == عدم) {
        أرجع عدم;
    }
    إذا (encoding != عدم && Py_Tok_مجموعه_Encoding(tok, encoding) < 0) {
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
borrow_read(فراغ *cookie, حرف *میانگیر, اندازه_t اندازه)
{
    borrowed_fd borrowed = {.cookie = cookie};
    أرجع اقرأ(borrowed.fd, میانگیر, اندازه);
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
#پایان_اگر

حرف *
Py_Tokenizer_پیدا_کن_Encoding_Filename(صحيح fd, PyObject *filename)
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
