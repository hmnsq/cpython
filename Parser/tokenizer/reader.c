#ضمن "Python.h"
#ضمن "pycore_fileutils.h"
#ضمن "pycore_pystate.h"
#ضمن "pycore_runtime.h"

#ضمن "errcode.h"
#ضمن "helpers.h"
#ضمن "reader.h"
#ضمن "reader_internal.h"
#ضمن "../lexer/state.h"

#تعریف_کړه READER_بفر_GROWTH_ضریب 2

#که_تعریف_شوی HAVE_UNISTD_H
#  include <unistd.h>
#د_که_پای

ساكن مضمن صحيح
reader_is_streaming(Py_Tok_Reader_ډول ډول)
{
    أرجع ډول == _PYTOK_READER_FILE || ډول == _PYTOK_READER_READLINE;
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
         i < (صحيح)Py_ARRAY_اوږدتیا(reader->prefetched_کرښې); i++) {
        _PyTok_ChunkClear(&reader->prefetched_کرښې[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_بفر);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = عدم;
}

صحيح
_PyTokenizer_Traverse(بنية tok_state *tok, visitproc لیدنه, فراغ *arg)
{
    Py_لیدنه(tok->filename);
    Py_لیدنه(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_لیدنه(reader->readline);
    Py_لیدنه(reader->decoder);
    لكل (صحيح i = 0;
         i < (صحيح)Py_ARRAY_اوږدتیا(reader->prefetched_کرښې); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_کرښې[i];
        إذا (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_لیدنه(chunk->owner);
        }
    }
    أرجع 0;
}

صحيح
Py_Tok_Reserve_بفر(حرف **بفر, Py_ssize_t *ظرفیت, Py_ssize_t needed,
                     Py_ssize_t initial_ظرفیت)
{
    أكد(initial_ظرفیت > 0);
    إذا (needed <= *ظرفیت) {
        أرجع 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *ظرفیت > 0 ? *ظرفیت : initial_ظرفیت;
    طالما (cap < needed) {
        إذا (cap > PY_SSIZE_T_MAX / READER_بفر_GROWTH_ضریب) {
            cap = needed;
            اكسر;
        }
        cap *= READER_بفر_GROWTH_ضریب;
    }
    حرف *resized = PyMem_Realloc(*بفر, cap);
    إذا (resized == عدم) {
        Py_Err_No_حافظه();
        أرجع -1;
    }
    *بفر = resized;
    *ظرفیت = cap;
    أرجع 0;
}

ساكن صحيح
append_decoded(_PyTok_Reader *reader, ثابت حرف *معلومات, Py_ssize_t len)
{
    إذا (len == 0) {
        أرجع 0;
    }
    إذا (reader->decoded_pos > 0) {
        Py_ssize_t پاتې_شوی = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (اندازه_t)پاتې_شوی);
        reader->decoded_لټون -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = پاتې_شوی;
    }
    إذا (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            Py_Tok_Reserve_بفر(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        Py_Err_No_حافظه();
        أرجع -1;
    }
    حافظه_نقل(reader->decoded + reader->decoded_len, معلومات, (اندازه_t)len);
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
    reader->decoded_لکۍ_is_implicit = 1;
    أرجع 0;
}

ساكن صحيح
راوباسه_decoded_کرښه(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    أكد(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    أكد(reader->decoded_لټون >= reader->decoded_pos &&
           reader->decoded_لټون <= reader->decoded_len);
    إذا (reader->decoded_pos == reader->decoded_len) {
        أرجع 0;
    }
    حرف *پیل = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    حرف *newline = memchr(reader->decoded + reader->decoded_لټون, '\n',
                           reader->decoded_len - reader->decoded_لټون);
    إذا (newline == عدم) {
        reader->decoded_لټون = reader->decoded_len;
        أرجع 0;
    }
    Py_ssize_t len = newline - پیل + 1;
    chunk->معلومات = پیل;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_لټون = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_لکۍ_is_implicit;
    إذا (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_لټون = 0;
        reader->decoded_لکۍ_is_implicit = 0;
    }
    أرجع 1;
}

ساكن صحيح
chunk_is_کرښه(ثابت _PyTok_Chunk *chunk)
{
    إذا (chunk->len == 0 || chunk->معلومات[chunk->len - 1] != '\n') {
        أرجع 0;
    }
    أرجع memchr(chunk->معلومات, '\n', chunk->len - 1) == عدم;
}

ساكن Py_Tok_Read_پایله
بل_prepared(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    إذا (tok->lineno >= tok->سرچینه.nlines) {
        أرجع _PYTOK_READ_EOF;
    }
    Py_Tok_پراختیا لکۍ = {tok->inp, tok->سرچینه.بنسټ_دننه_واټن + tok->سرچینه.len};
    _PyTok_Off newline = Py_Tok_سرچینه_پیدا_کړه_Byte(&tok->سرچینه, لکۍ, '\n');
    Py_Tok_پراختیا کرښه = {لکۍ.پیل, newline >= 0 ? newline + 1 : لکۍ.پای_ټکی};
    chunk->معلومات = (حرف *)Py_Tok_سرچینه_پراختیا_View(
        &tok->سرچینه, کرښه, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = کرښه.پای_ټکی == لکۍ.پای_ټکی
        ? tok->reader->prepared_final_newline : 0;
    أرجع PYTOK_READ_کرښه;
}

ساكن Py_Tok_Read_پایله
read_file_کرښه(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    لكل (;;) {
        إذا (len > PY_SSIZE_T_MAX - BUFSIZ) {
            Py_Err_No_حافظه();
            أرجع _PYTOK_READ_ERROR;
        }
        إذا (Py_Tok_Reserve_بفر(&reader->file_بفر, &reader->file_بفر_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            أرجع _PYTOK_READ_ERROR;
        }
        صحيح شته = (صحيح)Py_MIN(reader->file_بفر_cap - len, لوی_عدد);
        اندازه_t اقرأ = 0;
        حرف *پایله = Py_Universal_Newline_Fgets_With_اندازه(
            reader->file_بفر + len, شته, reader->fp, عدم, &اقرأ);
        إذا (پایله == عدم) {
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
    أرجع PYTOK_READ_کرښه;
}

ساكن صحيح
initialize_file(بنية tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    إذا (reader->encoding != عدم) {
        أرجع Py_Tok_پیل_Decoder(tok, "strict");
    }

    Py_Tok_Read_پایله پایله = read_file_کرښه(
        tok, &reader->prefetched_کرښې[0]);
    إذا (پایله == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        أرجع 0;
    }
    إذا (پایله != PYTOK_READ_کرښه) {
        أرجع -1;
    }
    Py_ssize_t bom_len;
    Py_Tok_Encoding_پایله detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_کرښې[0], عدم, 0, &bom_len);
    إذا (detection == _PYTOK_ENCODING_ERROR) {
        أرجع -1;
    }
    إذا (detection == PYTOK_ENCODING_NEED_دوهم_کرښه) {
        حرف *لومړی = Py_Tok_Copy_بایټونه(
            reader->prefetched_کرښې[0].معلومات,
            reader->prefetched_کرښې[0].len);
        إذا (لومړی == عدم) {
            tok->بشپړ = E_NOMEM;
            أرجع -1;
        }
        reader->prefetched_کرښې[0].معلومات = لومړی;
        reader->prefetched_کرښې[0].ownership = _PYTOK_CHUNK_PYMEM;
        پایله = read_file_کرښه(tok, &reader->prefetched_کرښې[1]);
        إذا (پایله == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        وإلا إذا (پایله != PYTOK_READ_کرښه) {
            أرجع -1;
        }
        _PyTok_Chunk *دوهم = reader->prefetched_کرښې[1].معلومات != عدم
            ? &reader->prefetched_کرښې[1] : عدم;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_کرښې[0], دوهم, 1, &bom_len);
        إذا (detection == _PYTOK_ENCODING_ERROR) {
            أرجع -1;
        }
    }
    إذا (bom_len != 0) {
        _PyTok_Chunk *لومړی = &reader->prefetched_کرښې[0];
        إذا (لومړی->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(لومړی->معلومات, لومړی->معلومات + bom_len,
                    (اندازه_t)(لومړی->len - bom_len));
            لومړی->معلومات[لومړی->len - bom_len] = '\0';
        }
        وإلا {
            لومړی->معلومات += bom_len;
        }
        لومړی->len -= bom_len;
    }
    إذا (Py_Tok_پیل_Decoder(tok, "strict") < 0) {
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
        _PyTok_Chunk ننوت = {
            .معلومات = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        صحيح decoded = _PyTok_DecodeChunk(tok, &ننوت, 1);
        إذا (decoded == 0 &&
                append_decoded(reader, ننوت.معلومات, ننوت.len) < 0) {
            tok->بشپړ = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&ننوت);
        إذا (decoded < 0) {
            أرجع -1;
        }
    }
    إذا (append_implicit_newline(reader) < 0) {
        tok->بشپړ = E_NOMEM;
        أرجع -1;
    }
    أرجع 0;
}

ساكن Py_Tok_Read_پایله
بل_file(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    إذا (!reader->file_initialized && initialize_file(tok) < 0) {
        أرجع _PYTOK_READ_ERROR;
    }
    لكل (;;) {
        إذا (راوباسه_decoded_کرښه(reader, chunk)) {
            أرجع PYTOK_READ_کرښه;
        }
        _PyTok_Chunk ننوت = {0};
        إذا (reader->prefetched_کرښې[0].معلومات != عدم) {
            ننوت = reader->prefetched_کرښې[0];
            reader->prefetched_کرښې[0] = (_PyTok_Chunk){0};
        }
        وإلا إذا (reader->prefetched_کرښې[1].معلومات != عدم) {
            ننوت = reader->prefetched_کرښې[1];
            reader->prefetched_کرښې[1] = (_PyTok_Chunk){0};
        }
        وإلا إذا (!reader->file_eof) {
            Py_Tok_Read_پایله پایله = read_file_کرښه(tok, &ننوت);
            إذا (پایله == _PYTOK_READ_ERROR) {
                أرجع پایله;
            }
            إذا (پایله == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        إذا (ننوت.معلومات != عدم) {
            صحيح implicit = ننوت.implicit_newline;
            إذا (reader->decoder == عدم && !implicit) {
                *chunk = ننوت;
                أرجع PYTOK_READ_کرښه;
            }
            صحيح decoded = _PyTok_DecodeChunk(tok, &ننوت, 0);
            إذا (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_کرښه(&ننوت)) {
                *chunk = ننوت;
                أرجع PYTOK_READ_کرښه;
            }
            إذا (decoded == 0 &&
                    append_decoded(reader, ننوت.معلومات, ننوت.len) < 0) {
                tok->بشپړ = E_NOMEM;
                decoded = -1;
            }
            إذا (decoded == 0 && implicit) {
                reader->decoded_لکۍ_is_implicit = 1;
            }
            _PyTok_ChunkClear(&ننوت);
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

ساكن Py_Tok_Read_پایله
بل_readline(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    لكل (;;) {
        إذا (راوباسه_decoded_کرښه(reader, chunk)) {
            أرجع PYTOK_READ_کرښه;
        }
        إذا (reader->decoder_finalized) {
            أرجع _PYTOK_READ_EOF;
        }

        PyObject *raw = Py_Object_Call_No_دلیلونه(reader->readline);
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

        _PyTok_Chunk ننوت = {0};
        إذا (reader->encoding != عدم) {
            إذا (!Py_بایټونه_Check(raw)) {
                Py_Err_سېټ_String(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                أرجع _PYTOK_READ_ERROR;
            }
            إذا (Py_بایټونه_GET_اندازه(raw) == 0) {
                Py_DECREF(raw);
                إذا (Py_Tok_پیل_Decoder(tok, "replace") < 0) {
                    أرجع _PYTOK_READ_ERROR;
                }
                إذا (finalize_decoding(tok) < 0) {
                    أرجع _PYTOK_READ_ERROR;
                }
                استمر;
            }
            ننوت.owner = raw;
            ننوت.معلومات = Py_بایټونه_AS_STRING(raw);
            ننوت.len = Py_بایټونه_GET_اندازه(raw);
            ننوت.ownership = _PYTOK_CHUNK_PYOBJECT;
            صحيح decoded;
            إذا (reader->decoder == عدم &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_کرښه(&ننوت)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &ننوت, "utf-8", "replace");
            }
            وإلا {
                decoded = Py_Tok_پیل_Decoder(tok, "replace");
                إذا (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &ننوت, 0);
                }
            }
            إذا (decoded < 0) {
                _PyTok_ChunkClear(&ننوت);
                أرجع _PYTOK_READ_ERROR;
            }
        }
        وإلا {
            إذا (!PyUnicode_Check(raw)) {
                Py_Err_سېټ_String(PyExc_TypeError,
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
            ننوت.owner = raw;
            ننوت.معلومات = (حرف *)utf8;
            ننوت.len = utf8_len;
            ننوت.ownership = _PYTOK_CHUNK_PYOBJECT;
            إذا (ننوت.len == 0) {
                _PyTok_ChunkClear(&ننوت);
                إذا (finalize_decoding(tok) < 0) {
                    أرجع _PYTOK_READ_ERROR;
                }
                استمر;
            }
        }

        إذا (reader->decoded_pos == reader->decoded_len &&
                chunk_is_کرښه(&ننوت)) {
            *chunk = ننوت;
            أرجع PYTOK_READ_کرښه;
        }

        إذا (append_decoded(reader, ننوت.معلومات, ننوت.len) < 0) {
            _PyTok_ChunkClear(&ننوت);
            tok->بشپړ = E_NOMEM;
            أرجع _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&ننوت);
        إذا (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            صحيح pending = Py_Tok_Decoder_Has_Buffered_ننوت(tok);
            إذا (pending < 0) {
                أرجع _PYTOK_READ_ERROR;
            }
            إذا (!pending && append_implicit_newline(reader) < 0) {
                tok->بشپړ = E_NOMEM;
                أرجع _PYTOK_READ_ERROR;
            }
        }
    }
}

ساكن Py_Tok_Read_پایله
بل_interactive(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    إذا (reader->stop_interactive) {
        أرجع _PYTOK_READ_STOPPED;
    }
    حرف *ننوت = PyOS_Readline(
        reader->fp != عدم ? reader->fp : stdin, stdout, reader->prompt);
    إذا (reader->nextprompt != عدم) {
        reader->prompt = reader->nextprompt;
    }
    إذا (ننوت == عدم) {
        أرجع PYTOK_READ_مداخله;
    }
    Py_ssize_t len = مزي_اوږدوالی(ننوت);
    إذا (len == 0) {
        PyMem_Free(ننوت);
        أرجع _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .معلومات = ننوت,
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
        أرجع PYTOK_READ_کرښه;
    }
    chunk->معلومات = _PyTok_NormalizeNewlines(
        decoded.معلومات, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    إذا (chunk->معلومات == عدم) {
        tok->بشپړ = E_NOMEM;
        أرجع _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    أرجع PYTOK_READ_کرښه;
}

صحيح
_PyTok_ReaderIsInteractive(ثابت بنية tok_state *tok)
{
    أرجع tok->reader->ډول == _PYTOK_READER_INTERACTIVE;
}

ثابت حرف *
Py_Tokenizer_Retained_سرچینه(ثابت بنية tok_state *tok)
{
    إذا (tok->reader->ډول == _PYTOK_READER_PREPARED) {
        أرجع Py_Tok_سرچینه_معلومات(&tok->سرچینه);
    }
    إذا (tok->reader->ډول == _PYTOK_READER_INTERACTIVE) {
        أرجع tok->سرچینه.بایټونه;
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

ساكن Py_Tok_Read_پایله
reader_بل(بنية tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    بدل (tok->reader->ډول) {
        حالة _PYTOK_READER_PREPARED:
            أرجع بل_prepared(tok, chunk);
        حالة _PYTOK_READER_FILE:
            أرجع بل_file(tok, chunk);
        حالة _PYTOK_READER_READLINE:
            أرجع بل_readline(tok, chunk);
        حالة _PYTOK_READER_INTERACTIVE:
            أرجع بل_interactive(tok, chunk);
    }
    Py_UNREACHABLE();
}

ساكن فراغ
reset_streaming_بفر(بنية tok_state *tok)
{
    أكد(tok->cur >= tok->buf_دننه_واټن && tok->cur <= tok->inp);
    Py_Tok_سرچینه_Discard(&tok->سرچینه);
    tok->buf_دننه_واټن = tok->سرچینه.بنسټ_دننه_واټن;
    tok->cur = tok->inp = tok->سرچینه.بنسټ_دننه_واټن;
    tok->کرښه_پیل = tok->buf_دننه_واټن;
}

صحيح
_PyTok_ReaderUnderflow(بنية tok_state *tok)
{
    أكد(tok->cur >= tok->buf_دننه_واټن && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    Py_Tok_Reader_ډول ډول = reader->ډول;
    صحيح prepared = ډول == _PYTOK_READER_PREPARED;
    صحيح streaming = reader_is_streaming(ډول);
    صحيح reset_بفر = tok->پیل < 0 &&
        _PyLexer_CurrentFTString(tok) == عدم;

    _PyTok_Chunk chunk;
    Py_Tok_Read_پایله پایله = reader_بل(tok, &chunk);
    إذا (پایله != PYTOK_READ_کرښه) {
        إذا (reset_بفر && streaming) {
            reset_streaming_بفر(tok);
        }
        إذا (پایله == _PYTOK_READ_EOF) {
            tok->بشپړ = E_EOF;
        }
        وإلا إذا (پایله == _PYTOK_READ_STOPPED) {
            tok->بشپړ = E_INTERACT_STOP;
        }
        وإلا إذا (پایله == PYTOK_READ_مداخله) {
            tok->بشپړ = E_INTR;
        }
        وإلا {
            إذا (tok->بشپړ == E_OK) {
                tok->بشپړ = PyErr_ExceptionMatches(Py_Exc_حافظه_Error)
                    ? E_NOMEM : E_ERROR;
            }
        }
        إذا (ډول == _PYTOK_READER_INTERACTIVE &&
                پایله != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        أرجع 0;
    }
    إذا (tok->lineno == لوی_عدد) {
        Py_Err_سېټ_String(Py_Exc_ډکېدنه_Error, "too many tokenizer source lines");
        tok->بشپړ = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        أرجع 0;
    }

    إذا (!prepared) {
        إذا (streaming && reset_بفر) {
            reset_streaming_بفر(tok);
        }
        _PyTok_Off سرچینه_پیل = Py_Tok_سرچینه_Append_کرښه(
            &tok->سرچینه, chunk.معلومات, chunk.len);
        إذا (سرچینه_پیل < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->بشپړ = PyErr_ExceptionMatches(Py_Exc_حافظه_Error)
                ? E_NOMEM : E_ERROR;
            أرجع 0;
        }
        إذا (reset_بفر) {
            tok->cur = سرچینه_پیل;
            tok->buf_دننه_واټن = سرچینه_پیل;
            tok->کرښه_پیل = tok->buf_دننه_واټن;
        }
        tok->inp = سرچینه_پیل + chunk.len;
    }
    وإلا {
        إذا (reset_بفر) {
            tok->buf_دننه_واټن = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    إذا (ډول == _PYTOK_READER_FILE &&
            (reader->encoding == عدم ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                Py_Tok_سرچینه_اشاره_ګر(&tok->سرچینه, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        أرجع 0;
    }
    _PyTok_ChunkClear(&chunk);
    أرجع 1;
}

ساكن بنية tok_state *
tokenizer_new_with_reader(Py_Tok_Reader_ډول ډول)
{
    بنية tok_state *tok = PyMem_Calloc(1, حجم(*tok));
    إذا (tok == عدم) {
        Py_Err_No_حافظه();
        أرجع عدم;
    }
    tok->پیل = tok->کرښه_پیل = -1;
    Py_Tok_سرچینه_Init(&tok->سرچینه);
    tok->بشپړ = E_OK;
    tok->layout.at_bol = 1;
    tok->پیل_loc = (_PyTok_Loc){-1, -1};
    tok->ftstring_ډېری = tok->ftstring_ډېری_inline;
    tok->ftstring_ظرفیت = FTSTRING_ډېری_INLINE_ظرفیت;
#که_تعریف_شوی Py_DEBUG
    tok->debug = _Py_GetConfig()->parser_debug;
#د_که_پای
    tok->reader = PyMem_Calloc(1, حجم(*tok->reader));
    إذا (tok->reader == عدم) {
        Py_Err_No_حافظه();
        _PyTokenizer_Free(tok);
        أرجع عدم;
    }
    tok->reader->ډول = ډول;
    إذا (ډول == _PYTOK_READER_PREPARED) {
        أرجع tok;
    }
    إذا (reader_is_streaming(ډول)) {
        tok->cur = tok->inp = tok->سرچینه.بنسټ_دننه_واټن;
        tok->کرښه_پیل = tok->buf_دننه_واټن;
    }
    أرجع tok;
}

ساكن بنية tok_state *
tokenizer_from_string(ثابت حرف *ننوت, صحيح utf8_only, صحيح exec_ننوت,
                      صحيح preserve_crlf)
{
    بنية tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    إذا (tok == عدم) {
        أرجع عدم;
    }
    إذا (_PyTok_PrepareString(
            tok, ننوت, utf8_only, exec_ننوت, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        أرجع عدم;
    }
    tok->cur = tok->inp = tok->سرچینه.بنسټ_دننه_واټن;
    tok->کرښه_پیل = tok->سرچینه.بنسټ_دننه_واټن;
    أرجع tok;
}

بنية tok_state *
_PyTokenizer_FromString(ثابت حرف *ننوت, صحيح exec_ننوت, صحيح preserve_crlf)
{
    أرجع tokenizer_from_string(ننوت, 0, exec_ننوت, preserve_crlf);
}

بنية tok_state *
_PyTokenizer_FromUTF8(ثابت حرف *ننوت, صحيح exec_ننوت, صحيح preserve_crlf)
{
    أرجع tokenizer_from_string(ننوت, 1, exec_ننوت, preserve_crlf);
}

بنية tok_state *
_PyTokenizer_FromReadline(PyObject *readline, ثابت حرف *encoding)
{
    بنية tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    إذا (tok == عدم) {
        أرجع عدم;
    }
    إذا (encoding != عدم && Py_Tok_سېټ_Encoding(tok, encoding) < 0) {
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
    Py_Tok_Reader_ډول ډول = ps1 != عدم || ps2 != عدم
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    بنية tok_state *tok = tokenizer_new_with_reader(ډول);
    إذا (tok == عدم) {
        أرجع عدم;
    }
    إذا (encoding != عدم && Py_Tok_سېټ_Encoding(tok, encoding) < 0) {
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
borrow_read(فراغ *cookie, حرف *بفر, اندازه_t اندازه)
{
    borrowed_fd borrowed = {.cookie = cookie};
    أرجع اقرأ(borrowed.fd, بفر, اندازه);
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
#د_که_پای

حرف *
Py_Tokenizer_پیدا_کړه_Encoding_Filename(صحيح fd, PyObject *filename)
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
