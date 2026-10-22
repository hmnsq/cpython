#شامل "Python.h"
#شامل "pycore_fileutils.h"
#شامل "pycore_pystate.h"
#شامل "pycore_runtime.h"

#شامل "errcode.h"
#شامل "helpers.h"
#شامل "reader.h"
#شامل "reader_internal.h"
#شامل "../lexer/state.h"

#تعریف_کرو READER_بفر_GROWTH_ضربی_عدد 2

#اگر_تعریف_شدہ HAVE_UNISTD_H
#  include <unistd.h>
#اگر_ختم

ساکن اندرونی صحیح
reader_is_streaming(Py_Tok_Reader_قسم قسم)
{
    واپس قسم == _PYTOK_READER_FILE || قسم == _PYTOK_READER_READLINE;
}

خالی
_PyTok_ReaderFree(ساخت tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    اگر (reader == تہی) {
        واپس;
    }
    Py_XDECREF(reader->readline);
    Py_XDECREF(reader->decoder);
    برائے (صحیح i = 0;
         i < (صحیح)Py_ARRAY_لمبائی(reader->prefetched_سطریں); i++) {
        _PyTok_ChunkClear(&reader->prefetched_سطریں[i]);
    }
    PyMem_Free(reader->encoding);
    PyMem_Free(reader->file_بفر);
    PyMem_Free(reader->decoded);
    PyMem_Free(reader);
    tok->reader = تہی;
}

صحیح
_PyTokenizer_Traverse(ساخت tok_state *tok, visitproc دورہ, خالی *arg)
{
    Py_دورہ(tok->filename);
    Py_دورہ(tok->module);
    _PyTok_Reader *reader = tok->reader;
    Py_دورہ(reader->readline);
    Py_دورہ(reader->decoder);
    برائے (صحیح i = 0;
         i < (صحیح)Py_ARRAY_لمبائی(reader->prefetched_سطریں); i++) {
        _PyTok_Chunk *chunk = &reader->prefetched_سطریں[i];
        اگر (chunk->ownership == _PYTOK_CHUNK_PYOBJECT) {
            Py_دورہ(chunk->owner);
        }
    }
    واپس 0;
}

صحیح
Py_Tok_Reserve_بفر(حرف **بفر, Py_ssize_t *استعداد, Py_ssize_t needed,
                     Py_ssize_t initial_استعداد)
{
    أكد(initial_استعداد > 0);
    اگر (needed <= *استعداد) {
        واپس 0;
    }
    // Grow geometrically to avoid reallocating for every longer line.
    Py_ssize_t cap = *استعداد > 0 ? *استعداد : initial_استعداد;
    جب (cap < needed) {
        اگر (cap > PY_SSIZE_T_MAX / READER_بفر_GROWTH_ضربی_عدد) {
            cap = needed;
            توڑو;
        }
        cap *= READER_بفر_GROWTH_ضربی_عدد;
    }
    حرف *resized = PyMem_Realloc(*بفر, cap);
    اگر (resized == تہی) {
        Py_Err_No_حافظہ();
        واپس -1;
    }
    *بفر = resized;
    *استعداد = cap;
    واپس 0;
}

ساکن صحیح
append_decoded(_PyTok_Reader *reader, ثابت حرف *ڈیٹا, Py_ssize_t len)
{
    اگر (len == 0) {
        واپس 0;
    }
    اگر (reader->decoded_pos > 0) {
        Py_ssize_t بقایا = reader->decoded_len - reader->decoded_pos;
        memmove(reader->decoded, reader->decoded + reader->decoded_pos,
                (سائز_t)بقایا);
        reader->decoded_جائزہ -= reader->decoded_pos;
        reader->decoded_pos = 0;
        reader->decoded_len = بقایا;
    }
    اگر (len < 0 || reader->decoded_len > PY_SSIZE_T_MAX - len - 1 ||
            Py_Tok_Reserve_بفر(&reader->decoded, &reader->decoded_cap,
                                reader->decoded_len + len + 1, BUFSIZ) < 0) {
        Py_Err_No_حافظہ();
        واپس -1;
    }
    حافظہ_نقل(reader->decoded + reader->decoded_len, ڈیٹا, (سائز_t)len);
    reader->decoded_len += len;
    reader->decoded[reader->decoded_len] = '\0';
    واپس 0;
}

ساکن صحیح
append_implicit_newline(_PyTok_Reader *reader)
{
    اگر (reader->decoded_pos == reader->decoded_len ||
            reader->decoded[reader->decoded_len - 1] == '\n') {
        واپس 0;
    }
    اگر (append_decoded(reader, "\n", 1) < 0) {
        واپس -1;
    }
    reader->decoded_دم_is_implicit = 1;
    واپس 0;
}

ساکن صحیح
نکالو_decoded_سطر(_PyTok_Reader *reader, _PyTok_Chunk *chunk)
{
    أكد(reader->decoded_pos >= 0 && reader->decoded_pos <= reader->decoded_len);
    أكد(reader->decoded_جائزہ >= reader->decoded_pos &&
           reader->decoded_جائزہ <= reader->decoded_len);
    اگر (reader->decoded_pos == reader->decoded_len) {
        واپس 0;
    }
    حرف *آغاز = reader->decoded + reader->decoded_pos;
    // Previously scanned bytes cannot contain a newline.
    حرف *newline = memchr(reader->decoded + reader->decoded_جائزہ, '\n',
                           reader->decoded_len - reader->decoded_جائزہ);
    اگر (newline == تہی) {
        reader->decoded_جائزہ = reader->decoded_len;
        واپس 0;
    }
    Py_ssize_t len = newline - آغاز + 1;
    chunk->ڈیٹا = آغاز;
    chunk->len = len;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    reader->decoded_pos += len;
    reader->decoded_جائزہ = reader->decoded_pos;
    chunk->implicit_newline = reader->decoded_pos == reader->decoded_len &&
        reader->decoded_دم_is_implicit;
    اگر (reader->decoded_pos == reader->decoded_len) {
        reader->decoded_pos = reader->decoded_len = 0;
        reader->decoded_جائزہ = 0;
        reader->decoded_دم_is_implicit = 0;
    }
    واپس 1;
}

ساکن صحیح
chunk_is_سطر(ثابت _PyTok_Chunk *chunk)
{
    اگر (chunk->len == 0 || chunk->ڈیٹا[chunk->len - 1] != '\n') {
        واپس 0;
    }
    واپس memchr(chunk->ڈیٹا, '\n', chunk->len - 1) == تہی;
}

ساکن Py_Tok_Read_نتیجہ
اگلا_prepared(ساخت tok_state *tok, _PyTok_Chunk *chunk)
{
    اگر (tok->lineno >= tok->ماخذ.nlines) {
        واپس _PYTOK_READ_EOF;
    }
    Py_Tok_وسعت دم = {tok->inp, tok->ماخذ.بنیاد_انحراف + tok->ماخذ.len};
    _PyTok_Off newline = Py_Tok_ماخذ_ڈھونڈو_Byte(&tok->ماخذ, دم, '\n');
    Py_Tok_وسعت سطر = {دم.آغاز, newline >= 0 ? newline + 1 : دم.انتہا};
    chunk->ڈیٹا = (حرف *)Py_Tok_ماخذ_وسعت_View(
        &tok->ماخذ, سطر, &chunk->len);
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    chunk->implicit_newline = سطر.انتہا == دم.انتہا
        ? tok->reader->prepared_final_newline : 0;
    واپس PYTOK_READ_سطر;
}

ساکن Py_Tok_Read_نتیجہ
read_file_سطر(ساخت tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    Py_ssize_t len = 0;
    برائے (;;) {
        اگر (len > PY_SSIZE_T_MAX - BUFSIZ) {
            Py_Err_No_حافظہ();
            واپس _PYTOK_READ_ERROR;
        }
        اگر (Py_Tok_Reserve_بفر(&reader->file_بفر, &reader->file_بفر_cap,
                                len + BUFSIZ, BUFSIZ) < 0) {
            واپس _PYTOK_READ_ERROR;
        }
        صحیح دستیاب = (صحیح)Py_MIN(reader->file_بفر_cap - len, زیادہ_سے_زیادہ_عدد);
        سائز_t اقرأ = 0;
        حرف *نتیجہ = Py_Universal_Newline_Fgets_With_سائز(
            reader->file_بفر + len, دستیاب, reader->fp, تہی, &اقرأ);
        اگر (نتیجہ == تہی) {
            اگر (len == 0) {
                واپس _PYTOK_READ_EOF;
            }
            توڑو;
        }
        len += (Py_ssize_t)اقرأ;
        اگر (len > 0 && reader->file_بفر[len - 1] == '\n') {
            توڑو;
        }
    }
    صحیح implicit = reader->file_بفر[len - 1] != '\n';
    chunk->ڈیٹا = reader->file_بفر;
    chunk->len = len;
    chunk->implicit_newline = implicit;
    chunk->ownership = _PYTOK_CHUNK_BORROWED;
    واپس PYTOK_READ_سطر;
}

ساکن صحیح
initialize_file(ساخت tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    reader->file_initialized = 1;
    اگر (reader->encoding != تہی) {
        واپس Py_Tok_آغاز_Decoder(tok, "strict");
    }

    Py_Tok_Read_نتیجہ نتیجہ = read_file_سطر(
        tok, &reader->prefetched_سطریں[0]);
    اگر (نتیجہ == _PYTOK_READ_EOF) {
        reader->file_eof = 1;
        واپس 0;
    }
    اگر (نتیجہ != PYTOK_READ_سطر) {
        واپس -1;
    }
    Py_ssize_t bom_len;
    Py_Tok_Encoding_نتیجہ detection = _PyTok_DetectEncoding(
        tok, &reader->prefetched_سطریں[0], تہی, 0, &bom_len);
    اگر (detection == _PYTOK_ENCODING_ERROR) {
        واپس -1;
    }
    اگر (detection == PYTOK_ENCODING_NEED_دوسرا_سطر) {
        حرف *پہلا = Py_Tok_Copy_بائٹیں(
            reader->prefetched_سطریں[0].ڈیٹا,
            reader->prefetched_سطریں[0].len);
        اگر (پہلا == تہی) {
            tok->مکمل = E_NOMEM;
            واپس -1;
        }
        reader->prefetched_سطریں[0].ڈیٹا = پہلا;
        reader->prefetched_سطریں[0].ownership = _PYTOK_CHUNK_PYMEM;
        نتیجہ = read_file_سطر(tok, &reader->prefetched_سطریں[1]);
        اگر (نتیجہ == _PYTOK_READ_EOF) {
            reader->file_eof = 1;
        }
        ورنہ اگر (نتیجہ != PYTOK_READ_سطر) {
            واپس -1;
        }
        _PyTok_Chunk *دوسرا = reader->prefetched_سطریں[1].ڈیٹا != تہی
            ? &reader->prefetched_سطریں[1] : تہی;
        detection = _PyTok_DetectEncoding(
            tok, &reader->prefetched_سطریں[0], دوسرا, 1, &bom_len);
        اگر (detection == _PYTOK_ENCODING_ERROR) {
            واپس -1;
        }
    }
    اگر (bom_len != 0) {
        _PyTok_Chunk *پہلا = &reader->prefetched_سطریں[0];
        اگر (پہلا->ownership == _PYTOK_CHUNK_PYMEM) {
            memmove(پہلا->ڈیٹا, پہلا->ڈیٹا + bom_len,
                    (سائز_t)(پہلا->len - bom_len));
            پہلا->ڈیٹا[پہلا->len - bom_len] = '\0';
        }
        ورنہ {
            پہلا->ڈیٹا += bom_len;
        }
        پہلا->len -= bom_len;
    }
    اگر (Py_Tok_آغاز_Decoder(tok, "strict") < 0) {
        واپس -1;
    }
    واپس 0;
}

ساکن صحیح
finalize_decoding(ساخت tok_state *tok)
{
    _PyTok_Reader *reader = tok->reader;
    اگر (reader->decoder_finalized) {
        واپس 0;
    }
    reader->decoder_finalized = 1;
    اگر (reader->decoder != تہی) {
        _PyTok_Chunk داخلہ = {
            .ڈیٹا = "",
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        صحیح decoded = _PyTok_DecodeChunk(tok, &داخلہ, 1);
        اگر (decoded == 0 &&
                append_decoded(reader, داخلہ.ڈیٹا, داخلہ.len) < 0) {
            tok->مکمل = E_NOMEM;
            decoded = -1;
        }
        _PyTok_ChunkClear(&داخلہ);
        اگر (decoded < 0) {
            واپس -1;
        }
    }
    اگر (append_implicit_newline(reader) < 0) {
        tok->مکمل = E_NOMEM;
        واپس -1;
    }
    واپس 0;
}

ساکن Py_Tok_Read_نتیجہ
اگلا_file(ساخت tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    اگر (!reader->file_initialized && initialize_file(tok) < 0) {
        واپس _PYTOK_READ_ERROR;
    }
    برائے (;;) {
        اگر (نکالو_decoded_سطر(reader, chunk)) {
            واپس PYTOK_READ_سطر;
        }
        _PyTok_Chunk داخلہ = {0};
        اگر (reader->prefetched_سطریں[0].ڈیٹا != تہی) {
            داخلہ = reader->prefetched_سطریں[0];
            reader->prefetched_سطریں[0] = (_PyTok_Chunk){0};
        }
        ورنہ اگر (reader->prefetched_سطریں[1].ڈیٹا != تہی) {
            داخلہ = reader->prefetched_سطریں[1];
            reader->prefetched_سطریں[1] = (_PyTok_Chunk){0};
        }
        ورنہ اگر (!reader->file_eof) {
            Py_Tok_Read_نتیجہ نتیجہ = read_file_سطر(tok, &داخلہ);
            اگر (نتیجہ == _PYTOK_READ_ERROR) {
                واپس نتیجہ;
            }
            اگر (نتیجہ == _PYTOK_READ_EOF) {
                reader->file_eof = 1;
            }
        }
        اگر (داخلہ.ڈیٹا != تہی) {
            صحیح implicit = داخلہ.implicit_newline;
            اگر (reader->decoder == تہی && !implicit) {
                *chunk = داخلہ;
                واپس PYTOK_READ_سطر;
            }
            صحیح decoded = _PyTok_DecodeChunk(tok, &داخلہ, 0);
            اگر (decoded == 0 && reader->decoded_pos == reader->decoded_len &&
                    chunk_is_سطر(&داخلہ)) {
                *chunk = داخلہ;
                واپس PYTOK_READ_سطر;
            }
            اگر (decoded == 0 &&
                    append_decoded(reader, داخلہ.ڈیٹا, داخلہ.len) < 0) {
                tok->مکمل = E_NOMEM;
                decoded = -1;
            }
            اگر (decoded == 0 && implicit) {
                reader->decoded_دم_is_implicit = 1;
            }
            _PyTok_ChunkClear(&داخلہ);
            اگر (decoded < 0) {
                واپس _PYTOK_READ_ERROR;
            }
            جاری;
        }
        اگر (!reader->decoder_finalized) {
            اگر (finalize_decoding(tok) < 0) {
                واپس _PYTOK_READ_ERROR;
            }
            جاری;
        }
        واپس _PYTOK_READ_EOF;
    }
}

ساکن Py_Tok_Read_نتیجہ
اگلا_readline(ساخت tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    برائے (;;) {
        اگر (نکالو_decoded_سطر(reader, chunk)) {
            واپس PYTOK_READ_سطر;
        }
        اگر (reader->decoder_finalized) {
            واپس _PYTOK_READ_EOF;
        }

        PyObject *raw = Py_Object_Call_No_دلائل(reader->readline);
        اگر (raw == تہی) {
            اگر (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
                اگر (finalize_decoding(tok) < 0) {
                    واپس _PYTOK_READ_ERROR;
                }
                جاری;
            }
            واپس _PYTOK_READ_ERROR;
        }

        _PyTok_Chunk داخلہ = {0};
        اگر (reader->encoding != تہی) {
            اگر (!Py_بائٹیں_Check(raw)) {
                Py_Err_سیٹ_String(PyExc_TypeError,
                                "readline() returned a non-bytes object");
                Py_DECREF(raw);
                واپس _PYTOK_READ_ERROR;
            }
            اگر (Py_بائٹیں_GET_سائز(raw) == 0) {
                Py_DECREF(raw);
                اگر (Py_Tok_آغاز_Decoder(tok, "replace") < 0) {
                    واپس _PYTOK_READ_ERROR;
                }
                اگر (finalize_decoding(tok) < 0) {
                    واپس _PYTOK_READ_ERROR;
                }
                جاری;
            }
            داخلہ.owner = raw;
            داخلہ.ڈیٹا = Py_بائٹیں_AS_STRING(raw);
            داخلہ.len = Py_بائٹیں_GET_سائز(raw);
            داخلہ.ownership = _PYTOK_CHUNK_PYOBJECT;
            صحیح decoded;
            اگر (reader->decoder == تہی &&
                    strcmp(reader->encoding, "utf-8") == 0 &&
                    chunk_is_سطر(&داخلہ)) {
                decoded = _PyTok_DecodeOnce(
                    tok, &داخلہ, "utf-8", "replace");
            }
            ورنہ {
                decoded = Py_Tok_آغاز_Decoder(tok, "replace");
                اگر (decoded == 0) {
                    decoded = _PyTok_DecodeChunk(tok, &داخلہ, 0);
                }
            }
            اگر (decoded < 0) {
                _PyTok_ChunkClear(&داخلہ);
                واپس _PYTOK_READ_ERROR;
            }
        }
        ورنہ {
            اگر (!PyUnicode_Check(raw)) {
                Py_Err_سیٹ_String(PyExc_TypeError,
                                "readline() returned a non-string object");
                Py_DECREF(raw);
                واپس _PYTOK_READ_ERROR;
            }
            Py_ssize_t utf8_len;
            ثابت حرف *utf8 = Py_Unicode_As_UTF8_And_سائز(raw, &utf8_len);
            اگر (utf8 == تہی) {
                Py_DECREF(raw);
                واپس _PYTOK_READ_ERROR;
            }
            داخلہ.owner = raw;
            داخلہ.ڈیٹا = (حرف *)utf8;
            داخلہ.len = utf8_len;
            داخلہ.ownership = _PYTOK_CHUNK_PYOBJECT;
            اگر (داخلہ.len == 0) {
                _PyTok_ChunkClear(&داخلہ);
                اگر (finalize_decoding(tok) < 0) {
                    واپس _PYTOK_READ_ERROR;
                }
                جاری;
            }
        }

        اگر (reader->decoded_pos == reader->decoded_len &&
                chunk_is_سطر(&داخلہ)) {
            *chunk = داخلہ;
            واپس PYTOK_READ_سطر;
        }

        اگر (append_decoded(reader, داخلہ.ڈیٹا, داخلہ.len) < 0) {
            _PyTok_ChunkClear(&داخلہ);
            tok->مکمل = E_NOMEM;
            واپس _PYTOK_READ_ERROR;
        }
        _PyTok_ChunkClear(&داخلہ);
        اگر (reader->decoded_pos < reader->decoded_len &&
                reader->decoded[reader->decoded_len - 1] != '\n') {
            صحیح pending = Py_Tok_Decoder_Has_Buffered_داخلہ(tok);
            اگر (pending < 0) {
                واپس _PYTOK_READ_ERROR;
            }
            اگر (!pending && append_implicit_newline(reader) < 0) {
                tok->مکمل = E_NOMEM;
                واپس _PYTOK_READ_ERROR;
            }
        }
    }
}

ساکن Py_Tok_Read_نتیجہ
اگلا_interactive(ساخت tok_state *tok, _PyTok_Chunk *chunk)
{
    _PyTok_Reader *reader = tok->reader;
    اگر (reader->stop_interactive) {
        واپس _PYTOK_READ_STOPPED;
    }
    حرف *داخلہ = PyOS_Readline(
        reader->fp != تہی ? reader->fp : stdin, stdout, reader->prompt);
    اگر (reader->nextprompt != تہی) {
        reader->prompt = reader->nextprompt;
    }
    اگر (داخلہ == تہی) {
        واپس PYTOK_READ_مداخلت;
    }
    Py_ssize_t len = لڑی_لمبائی(داخلہ);
    اگر (len == 0) {
        PyMem_Free(داخلہ);
        واپس _PYTOK_READ_EOF;
    }
    _PyTok_Chunk decoded = {
        .ڈیٹا = داخلہ,
        .len = len,
        .ownership = _PYTOK_CHUNK_PYMEM,
    };
    اگر (reader->encoding != تہی &&
            _PyTok_DecodeOnce(
                tok, &decoded, reader->encoding, تہی) < 0) {
        _PyTok_ChunkClear(&decoded);
        واپس _PYTOK_READ_ERROR;
    }
    اگر (memchr(decoded.ڈیٹا, '\r', decoded.len) == تہی) {
        *chunk = decoded;
        واپس PYTOK_READ_سطر;
    }
    chunk->ڈیٹا = _PyTok_NormalizeNewlines(
        decoded.ڈیٹا, decoded.len, &chunk->len);
    _PyTok_ChunkClear(&decoded);
    اگر (chunk->ڈیٹا == تہی) {
        tok->مکمل = E_NOMEM;
        واپس _PYTOK_READ_ERROR;
    }
    chunk->ownership = _PYTOK_CHUNK_PYMEM;
    واپس PYTOK_READ_سطر;
}

صحیح
_PyTok_ReaderIsInteractive(ثابت ساخت tok_state *tok)
{
    واپس tok->reader->قسم == _PYTOK_READER_INTERACTIVE;
}

ثابت حرف *
Py_Tokenizer_Retained_ماخذ(ثابت ساخت tok_state *tok)
{
    اگر (tok->reader->قسم == _PYTOK_READER_PREPARED) {
        واپس Py_Tok_ماخذ_ڈیٹا(&tok->ماخذ);
    }
    اگر (tok->reader->قسم == _PYTOK_READER_INTERACTIVE) {
        واپس tok->ماخذ.بائٹیں;
    }
    واپس تہی;
}

خالی
_PyTok_ReaderStopInteractive(ساخت tok_state *tok)
{
    اگر (_PyTok_ReaderIsInteractive(tok)) {
        tok->reader->stop_interactive = 1;
    }
}

ساکن Py_Tok_Read_نتیجہ
reader_اگلا(ساخت tok_state *tok, _PyTok_Chunk *chunk)
{
    *chunk = (_PyTok_Chunk){0};
    انتخاب (tok->reader->قسم) {
        حالت _PYTOK_READER_PREPARED:
            واپس اگلا_prepared(tok, chunk);
        حالت _PYTOK_READER_FILE:
            واپس اگلا_file(tok, chunk);
        حالت _PYTOK_READER_READLINE:
            واپس اگلا_readline(tok, chunk);
        حالت _PYTOK_READER_INTERACTIVE:
            واپس اگلا_interactive(tok, chunk);
    }
    Py_UNREACHABLE();
}

ساکن خالی
reset_streaming_بفر(ساخت tok_state *tok)
{
    أكد(tok->cur >= tok->buf_انحراف && tok->cur <= tok->inp);
    Py_Tok_ماخذ_Discard(&tok->ماخذ);
    tok->buf_انحراف = tok->ماخذ.بنیاد_انحراف;
    tok->cur = tok->inp = tok->ماخذ.بنیاد_انحراف;
    tok->سطر_آغاز = tok->buf_انحراف;
}

صحیح
_PyTok_ReaderUnderflow(ساخت tok_state *tok)
{
    أكد(tok->cur >= tok->buf_انحراف && tok->cur <= tok->inp);
    _PyTok_Reader *reader = tok->reader;
    Py_Tok_Reader_قسم قسم = reader->قسم;
    صحیح prepared = قسم == _PYTOK_READER_PREPARED;
    صحیح streaming = reader_is_streaming(قسم);
    صحیح reset_بفر = tok->آغاز < 0 &&
        _PyLexer_CurrentFTString(tok) == تہی;

    _PyTok_Chunk chunk;
    Py_Tok_Read_نتیجہ نتیجہ = reader_اگلا(tok, &chunk);
    اگر (نتیجہ != PYTOK_READ_سطر) {
        اگر (reset_بفر && streaming) {
            reset_streaming_بفر(tok);
        }
        اگر (نتیجہ == _PYTOK_READ_EOF) {
            tok->مکمل = E_EOF;
        }
        ورنہ اگر (نتیجہ == _PYTOK_READ_STOPPED) {
            tok->مکمل = E_INTERACT_STOP;
        }
        ورنہ اگر (نتیجہ == PYTOK_READ_مداخلت) {
            tok->مکمل = E_INTR;
        }
        ورنہ {
            اگر (tok->مکمل == E_OK) {
                tok->مکمل = PyErr_ExceptionMatches(Py_Exc_حافظہ_Error)
                    ? E_NOMEM : E_ERROR;
            }
        }
        اگر (قسم == _PYTOK_READER_INTERACTIVE &&
                نتیجہ != _PYTOK_READ_STOPPED) {
            PySys_WriteStderr("\n");
        }
        واپس 0;
    }
    اگر (tok->lineno == زیادہ_سے_زیادہ_عدد) {
        Py_Err_سیٹ_String(Py_Exc_حد_سے_تجاوز_Error, "too many tokenizer source lines");
        tok->مکمل = E_ERROR;
        _PyTok_ChunkClear(&chunk);
        واپس 0;
    }

    اگر (!prepared) {
        اگر (streaming && reset_بفر) {
            reset_streaming_بفر(tok);
        }
        _PyTok_Off ماخذ_آغاز = Py_Tok_ماخذ_Append_سطر(
            &tok->ماخذ, chunk.ڈیٹا, chunk.len);
        اگر (ماخذ_آغاز < 0) {
            _PyTok_ChunkClear(&chunk);
            tok->مکمل = PyErr_ExceptionMatches(Py_Exc_حافظہ_Error)
                ? E_NOMEM : E_ERROR;
            واپس 0;
        }
        اگر (reset_بفر) {
            tok->cur = ماخذ_آغاز;
            tok->buf_انحراف = ماخذ_آغاز;
            tok->سطر_آغاز = tok->buf_انحراف;
        }
        tok->inp = ماخذ_آغاز + chunk.len;
    }
    ورنہ {
        اگر (reset_بفر) {
            tok->buf_انحراف = tok->inp;
        }
        tok->inp += chunk.len;
    }
    tok->implicit_newline = chunk.implicit_newline;

    tok->lineno++;
    اگر (قسم == _PYTOK_READER_FILE &&
            (reader->encoding == تہی ||
             strcmp(reader->encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(
                Py_Tok_ماخذ_اشارہ_گر(&tok->ماخذ, tok->cur),
                tok, tok->lineno)) {
        _PyTok_ChunkClear(&chunk);
        واپس 0;
    }
    _PyTok_ChunkClear(&chunk);
    واپس 1;
}

ساکن ساخت tok_state *
tokenizer_new_with_reader(Py_Tok_Reader_قسم قسم)
{
    ساخت tok_state *tok = PyMem_Calloc(1, حجم(*tok));
    اگر (tok == تہی) {
        Py_Err_No_حافظہ();
        واپس تہی;
    }
    tok->آغاز = tok->سطر_آغاز = -1;
    Py_Tok_ماخذ_Init(&tok->ماخذ);
    tok->مکمل = E_OK;
    tok->layout.at_bol = 1;
    tok->آغاز_loc = (_PyTok_Loc){-1, -1};
    tok->ftstring_ڈھیر = tok->ftstring_ڈھیر_inline;
    tok->ftstring_استعداد = FTSTRING_ڈھیر_INLINE_استعداد;
#اگر_تعریف_شدہ Py_DEBUG
    tok->debug = _Py_GetConfig()->parser_debug;
#اگر_ختم
    tok->reader = PyMem_Calloc(1, حجم(*tok->reader));
    اگر (tok->reader == تہی) {
        Py_Err_No_حافظہ();
        _PyTokenizer_Free(tok);
        واپس تہی;
    }
    tok->reader->قسم = قسم;
    اگر (قسم == _PYTOK_READER_PREPARED) {
        واپس tok;
    }
    اگر (reader_is_streaming(قسم)) {
        tok->cur = tok->inp = tok->ماخذ.بنیاد_انحراف;
        tok->سطر_آغاز = tok->buf_انحراف;
    }
    واپس tok;
}

ساکن ساخت tok_state *
tokenizer_from_string(ثابت حرف *داخلہ, صحیح utf8_only, صحیح exec_داخلہ,
                      صحیح preserve_crlf)
{
    ساخت tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_PREPARED);
    اگر (tok == تہی) {
        واپس تہی;
    }
    اگر (_PyTok_PrepareString(
            tok, داخلہ, utf8_only, exec_داخلہ, preserve_crlf) < 0) {
        _PyTokenizer_Free(tok);
        واپس تہی;
    }
    tok->cur = tok->inp = tok->ماخذ.بنیاد_انحراف;
    tok->سطر_آغاز = tok->ماخذ.بنیاد_انحراف;
    واپس tok;
}

ساخت tok_state *
_PyTokenizer_FromString(ثابت حرف *داخلہ, صحیح exec_داخلہ, صحیح preserve_crlf)
{
    واپس tokenizer_from_string(داخلہ, 0, exec_داخلہ, preserve_crlf);
}

ساخت tok_state *
_PyTokenizer_FromUTF8(ثابت حرف *داخلہ, صحیح exec_داخلہ, صحیح preserve_crlf)
{
    واپس tokenizer_from_string(داخلہ, 1, exec_داخلہ, preserve_crlf);
}

ساخت tok_state *
_PyTokenizer_FromReadline(PyObject *readline, ثابت حرف *encoding)
{
    ساخت tok_state *tok = tokenizer_new_with_reader(_PYTOK_READER_READLINE);
    اگر (tok == تہی) {
        واپس تہی;
    }
    اگر (encoding != تہی && Py_Tok_سیٹ_Encoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        واپس تہی;
    }
    tok->reader->readline = Py_NewRef(readline);
    واپس tok;
}

ساخت tok_state *
_PyTokenizer_FromFile(FILE *fp, ثابت حرف *encoding,
                      ثابت حرف *ps1, ثابت حرف *ps2)
{
    Py_Tok_Reader_قسم قسم = ps1 != تہی || ps2 != تہی
        ? _PYTOK_READER_INTERACTIVE : _PYTOK_READER_FILE;
    ساخت tok_state *tok = tokenizer_new_with_reader(قسم);
    اگر (tok == تہی) {
        واپس تہی;
    }
    اگر (encoding != تہی && Py_Tok_سیٹ_Encoding(tok, encoding) < 0) {
        _PyTokenizer_Free(tok);
        واپس تہی;
    }
    tok->reader->fp = fp;
    tok->reader->prompt = ps1;
    tok->reader->nextprompt = ps2;
    واپس tok;
}

#اگر defined(__wasi__) || (defined(__EMSCRIPTEN__) && (__EMSCRIPTEN_major__ >= 3))
/* WASI has no dup(), and Emscripten's emulation is slow. */
قسمتعریف اتحاد {
    خالی *cookie;
    صحیح fd;
} borrowed_fd;

ساکن ssize_t
borrow_read(خالی *cookie, حرف *بفر, سائز_t سائز)
{
    borrowed_fd borrowed = {.cookie = cookie};
    واپس اقرأ(borrowed.fd, بفر, سائز);
}

ساکن FILE *
fdopen_borrow(صحیح fd)
{
    cookie_io_functions_t callbacks = {borrow_read, تہی, تہی, تہی};
    borrowed_fd borrowed = {.fd = fd};
    واپس fopencookie(borrowed.cookie, "r", callbacks);
}
#ورنہ
ساکن FILE *
fdopen_borrow(صحیح fd)
{
    صحیح copy = _Py_dup(fd);
    اگر (copy < 0) {
        واپس تہی;
    }
    FILE *fp = fdopen(copy, "r");
    اگر (fp == تہی) {
        صحیح saved_errno = errno;
        close(copy);
        errno = saved_errno;
    }
    واپس fp;
}
#اگر_ختم

حرف *
Py_Tokenizer_ڈھونڈو_Encoding_Filename(صحیح fd, PyObject *filename)
{
    FILE *fp = fdopen_borrow(fd);
    اگر (fp == تہی) {
        واپس تہی;
    }
    ساخت tok_state *tok = _PyTokenizer_FromFile(fp, تہی, تہی, تہی);
    اگر (tok == تہی) {
        fclose(fp);
        واپس تہی;
    }
    _Py_DECLARE_STR(anon_string, "<string>");
    tok->filename = Py_NewRef(filename != تہی ? filename : &_Py_STR(anon_string));
    حرف *encoding = تہی;
    اگر (initialize_file(tok) == 0) {
        encoding = tok->reader->encoding;
        tok->reader->encoding = تہی;
    }
    fclose(fp);
    _PyTokenizer_Free(tok);
    واپس encoding;
}
