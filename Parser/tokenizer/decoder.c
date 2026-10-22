#شامل "Python.h"
#شامل "pycore_codecs.h"
#شامل "pycore_global_strings.h"
#شامل "pycore_runtime.h"
#شامل "errcode.h"

#شامل "reader_internal.h"
#شامل "helpers.h"
#شامل "../lexer/state.h"

#تعریف_کرو UNICODE_BOM 0xFEFF
#تعریف_کرو UTF16_LE_BOM "\xff\xfe"
#تعریف_کرو UTF16_BE_BOM "\xfe\xff"
#تعریف_کرو UTF16_BOM_لمبائی ((Py_ssize_t)(حجم(UTF16_LE_BOM) - 1))
#تعریف_کرو UTF8_BOM "\xef\xbb\xbf"
#تعریف_کرو UTF8_BOM_لمبائی ((Py_ssize_t)(حجم(UTF8_BOM) - 1))

حرف *
Py_Tok_Copy_بائٹیں(ثابت حرف *ڈیٹا, Py_ssize_t len)
{
    اگر (len < 0 || len == PY_SSIZE_T_MAX) {
        Py_Err_No_حافظہ();
        واپس تہی;
    }
    حرف *copy = PyMem_Malloc((سائز_t)len + 1);
    اگر (copy == تہی) {
        Py_Err_No_حافظہ();
        واپس تہی;
    }
    حافظہ_نقل(copy, ڈیٹا, (سائز_t)len);
    copy[len] = '\0';
    واپس copy;
}

ساکن خالی
chunk_release_ڈیٹا(_PyTok_Chunk *chunk)
{
    انتخاب (chunk->ownership) {
        حالت _PYTOK_CHUNK_BORROWED:
            توڑو;
        حالت _PYTOK_CHUNK_PYMEM:
            PyMem_Free(chunk->ڈیٹا);
            توڑو;
        حالت _PYTOK_CHUNK_PYOBJECT:
            Py_DECREF(chunk->owner);
            توڑو;
    }
}

خالی
_PyTok_ChunkClear(_PyTok_Chunk *chunk)
{
    chunk_release_ڈیٹا(chunk);
    *chunk = (_PyTok_Chunk){0};
}

ساکن صحیح
chunk_سیٹ_unicode(ساخت tok_state *tok, _PyTok_Chunk *chunk,
                  PyObject *unicode, صحیح strip_bom)
{
    Py_ssize_t utf8_len;
    ثابت حرف *utf8 = Py_Unicode_As_UTF8_And_سائز(unicode, &utf8_len);
    اگر (utf8 == تہی) {
        Py_DECREF(unicode);
        tok->مکمل = PyErr_ExceptionMatches(Py_Exc_حافظہ_Error)
            ? E_NOMEM : E_DECODE;
        واپس -1;
    }
    اگر (strip_bom && Py_Unicode_GET_لمبائی(unicode) > 0 &&
            PyUnicode_ReadChar(unicode, 0) == UNICODE_BOM) {
        utf8 += UTF8_BOM_لمبائی;
        utf8_len -= UTF8_BOM_لمبائی;
    }
    chunk_release_ڈیٹا(chunk);
    chunk->owner = unicode;
    chunk->ڈیٹا = (حرف *)utf8;
    chunk->len = utf8_len;
    chunk->ownership = _PYTOK_CHUNK_PYOBJECT;
    واپس 0;
}

حرف *
_PyTok_NormalizeNewlines(ثابت حرف *ڈیٹا, Py_ssize_t len,
                         Py_ssize_t *برآمد_len)
{
    اگر (len < 0 || len == PY_SSIZE_T_MAX) {
        Py_Err_No_حافظہ();
        واپس تہی;
    }
    حرف *نتیجہ = PyMem_Malloc((سائز_t)len + 1);
    اگر (نتیجہ == تہی) {
        Py_Err_No_حافظہ();
        واپس تہی;
    }
    Py_ssize_t اكتب = 0;
    برائے (Py_ssize_t اقرأ = 0; اقرأ < len; اقرأ++) {
        حرف c = ڈیٹا[اقرأ];
        اگر (c == '\r') {
            اگر (اقرأ + 1 < len && ڈیٹا[اقرأ + 1] == '\n') {
                اقرأ++;
            }
            c = '\n';
        }
        نتیجہ[اكتب++] = c;
    }
    نتیجہ[اكتب] = '\0';
    *برآمد_len = اكتب;
    واپس نتیجہ;
}

صحیح
Py_Tok_سیٹ_Encoding(ساخت tok_state *tok, ثابت حرف *encoding)
{
    حرف *copy = Py_Tok_Copy_بائٹیں(encoding, لڑی_لمبائی(encoding));
    اگر (copy == تہی) {
        tok->مکمل = E_NOMEM;
        واپس -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    واپس 0;
}

ساکن صحیح
ڈھونڈو_cookie(ثابت حرف *سطر, Py_ssize_t len, حرف **encoding, صحیح *جائزہ_اگلا)
{
    Py_ssize_t i = 0;
    *encoding = تہی;
    *جائزہ_اگلا = 1;
    برائے (; i < len; i++) {
        اگر (سطر[i] == '#') {
            توڑو;
        }
        اگر (سطر[i] == '\n' || سطر[i] == '\r') {
            واپس 0;
        }
        اگر (سطر[i] != ' ' && سطر[i] != '\t' && سطر[i] != '\f') {
            *جائزہ_اگلا = 0;
            واپس 0;
        }
    }
    ثابت Py_ssize_t marker_len = حجم("coding") - 1;
    برائے (; i < len - marker_len; i++) {
        اگر (memcmp(سطر + i, "coding", marker_len) != 0) {
            جاری;
        }
        ثابت حرف *cursor = سطر + i + marker_len;
        اگر (*cursor != ':' && *cursor != '=') {
            جاری;
        }
        کرو {
            cursor++;
        } جب (cursor < سطر + len &&
                 (*cursor == ' ' || *cursor == '\t'));
        ثابت حرف *آغاز = cursor;
        جب (cursor < سطر + len &&
                (Py_ISALNUM(*cursor) || *cursor == '-' ||
                 *cursor == '_' || *cursor == '.')) {
            cursor++;
        }
        اگر (cursor == آغاز) {
            جاری;
        }
        Py_ssize_t encoding_len = cursor - آغاز;
        // Include the longest recognized prefix and its NUL terminator.
        حرف normalized[حجم("iso-latin-1-")];
        ثابت Py_ssize_t سابقہ_len = Py_MIN(
            encoding_len, (Py_ssize_t)حجم(normalized) - 1);
        Py_ssize_t n;
        برائے (n = 0; n < سابقہ_len; n++) {
            normalized[n] = آغاز[n] == '_' ? '-' : Py_TOLOWER(آغاز[n]);
        }
        normalized[n] = '\0';
        ثابت حرف *canonical = آغاز;
        اگر (strcmp(normalized, "utf-8") == 0 ||
                لڑی_موازنہ(normalized, "utf-8-", حجم("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        ورنہ اگر (strcmp(normalized, "latin-1") == 0 ||
                 strcmp(normalized, "iso-8859-1") == 0 ||
                 strcmp(normalized, "iso-latin-1") == 0 ||
                 لڑی_موازنہ(normalized, "latin-1-", حجم("latin-1-") - 1) == 0 ||
                 لڑی_موازنہ(normalized, "iso-8859-1-", حجم("iso-8859-1-") - 1) == 0 ||
                 لڑی_موازنہ(normalized, "iso-latin-1-", حجم("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        اگر (canonical != آغاز) {
            encoding_len = لڑی_لمبائی(canonical);
        }
        *encoding = Py_Tok_Copy_بائٹیں(canonical, encoding_len);
        اگر (*encoding == تہی) {
            واپس -1;
        }
        *جائزہ_اگلا = 0;
        واپس 0;
    }
    واپس 0;
}

Py_Tok_Encoding_نتیجہ
_PyTok_DetectEncoding(ساخت tok_state *tok, ثابت _PyTok_Chunk *پہلا,
                      ثابت _PyTok_Chunk *دوسرا, صحیح final,
                      Py_ssize_t *bom_len)
{
    صحیح bom = پہلا->len >= UTF8_BOM_لمبائی &&
        memcmp(پہلا->ڈیٹا, UTF8_BOM, UTF8_BOM_لمبائی) == 0;
    *bom_len = bom ? UTF8_BOM_لمبائی : 0;

    حرف *cookie = تہی;
    صحیح جائزہ_اگلا = 0;
    صحیح cookie_سطر = 1;
    ثابت حرف *پہلا_ڈیٹا = پہلا->ڈیٹا + *bom_len;
    Py_ssize_t پہلا_len = پہلا->len - *bom_len;
    اگر (ڈھونڈو_cookie(پہلا_ڈیٹا, پہلا_len, &cookie, &جائزہ_اگلا) < 0) {
        واپس _PYTOK_ENCODING_ERROR;
    }
    اگر (cookie == تہی && جائزہ_اگلا && دوسرا != تہی) {
        اگر (ڈھونڈو_cookie(دوسرا->ڈیٹا, دوسرا->len, &cookie, &جائزہ_اگلا) < 0) {
            واپس _PYTOK_ENCODING_ERROR;
        }
        cookie_سطر = 2;
    }
    ورنہ اگر (cookie == تہی && جائزہ_اگلا && !final) {
        واپس PYTOK_ENCODING_NEED_دوسرا_سطر;
    }

    اگر (cookie == تہی) {
        اگر (bom && Py_Tok_سیٹ_Encoding(tok, "utf-8") < 0) {
            واپس _PYTOK_ENCODING_ERROR;
        }
        واپس PYTOK_ENCODING_مکمل;
    }
    اگر (bom && strcmp(cookie, "utf-8") != 0) {
        ثابت _PyTok_Chunk *سطر = cookie_سطر == 2 ? دوسرا : پہلا;
        ثابت حرف *سطر_ڈیٹا = سطر->ڈیٹا + (cookie_سطر == 1 ? *bom_len : 0);
        Py_ssize_t سطر_len = سطر->len - (cookie_سطر == 1 ? *bom_len : 0);
        صحیح انتہا_ستون = (صحیح)Py_MIN(سطر_len, زیادہ_سے_زیادہ_عدد);
        اگر (انتہا_ستون > 0 && (سطر_ڈیٹا[انتہا_ستون - 1] == '\n' ||
                            سطر_ڈیٹا[انتہا_ستون - 1] == '\r')) {
            انتہا_ستون--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, سطر_ڈیٹا, 0, cookie_سطر, 0, انتہا_ستون,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        واپس _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    واپس PYTOK_ENCODING_مکمل;
}

صحیح
_PyTok_DecodeOnce(ساخت tok_state *tok, _PyTok_Chunk *chunk,
                  ثابت حرف *encoding, ثابت حرف *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->ڈیٹا, chunk->len, encoding, errors);
    اگر (unicode == تہی) {
        tok->مکمل = PyErr_ExceptionMatches(Py_Exc_حافظہ_Error)
            ? E_NOMEM : E_DECODE;
        واپس -1;
    }
    واپس chunk_سیٹ_unicode(tok, chunk, unicode, 0);
}

ساکن Py_ssize_t
raw_سطر_لمبائی(ثابت حرف *ڈیٹا, Py_ssize_t len)
{
    برائے (Py_ssize_t i = 0; i < len; i++) {
        اگر (ڈیٹا[i] == '\n') {
            واپس i + 1;
        }
        اگر (ڈیٹا[i] == '\r') {
            واپس i + 1 < len && ڈیٹا[i + 1] == '\n' ? i + 2 : i + 1;
        }
    }
    واپس len;
}

ساکن صحیح
store_prepared_ماخذ(ساخت tok_state *tok, ثابت حرف *ڈیٹا, Py_ssize_t len,
                      صحیح preserve_crlf, صحیح جوڑو_final_newline)
{
    Py_ssize_t pos = 0;
    جب (pos < len) {
        Py_ssize_t raw_سطر_len;
        اگر (preserve_crlf) {
            ثابت حرف *newline = memchr(ڈیٹا + pos, '\n', len - pos);
            raw_سطر_len = newline == تہی
                ? len - pos : newline - ڈیٹا - pos + 1;
        }
        ورنہ {
            raw_سطر_len = raw_سطر_لمبائی(ڈیٹا + pos, len - pos);
        }
        صحیح terminated = preserve_crlf
            ? ڈیٹا[pos + raw_سطر_len - 1] == '\n'
            : ڈیٹا[pos + raw_سطر_len - 1] == '\n' ||
              ڈیٹا[pos + raw_سطر_len - 1] == '\r';
        صحیح جوڑو_newline = جوڑو_final_newline &&
            pos + raw_سطر_len == len && !terminated;
        ثابت حرف *سطر = ڈیٹا + pos;
        // raw_line_length stops at the first CR or LF, so any CR is in
        // the line terminator.
        Py_ssize_t terminator_len = 0;
        اگر (!preserve_crlf) {
            اگر (سطر[raw_سطر_len - 1] == '\r') {
                terminator_len = 1;
            }
            ورنہ اگر (raw_سطر_len > 1 && سطر[raw_سطر_len - 2] == '\r') {
                terminator_len = 2;
            }
        }
        _PyTok_Off appended;
        اگر (terminator_len || جوڑو_newline) {
            appended = Py_Tok_ماخذ_Append_سطر_With_Newline(
                &tok->ماخذ, سطر, raw_سطر_len - terminator_len);
        }
        ورنہ {
            appended = Py_Tok_ماخذ_Append_سطر(&tok->ماخذ, سطر, raw_سطر_len);
        }
        اگر (appended < 0) {
            tok->مکمل = PyErr_ExceptionMatches(Py_Exc_حافظہ_Error)
                ? E_NOMEM : E_ERROR;
            واپس -1;
        }
        tok->reader->prepared_final_newline = جوڑو_newline;
        pos += raw_سطر_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    اگر (جوڑو_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || ڈیٹا[len - 1] != '\r'))
    {
        اگر (Py_Tok_ماخذ_Append_سطر(&tok->ماخذ, "\n", 1) < 0) {
            واپس -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    واپس 0;
}

صحیح
_PyTok_PrepareString(ساخت tok_state *tok, ثابت حرف *داخلہ, صحیح utf8_only,
                     صحیح exec_داخلہ, صحیح preserve_crlf)
{
    Py_ssize_t raw_len = لڑی_لمبائی(داخلہ);
    حرف *raw = (حرف *)داخلہ;

    اگر (utf8_only) {
        اگر (Py_Tok_سیٹ_Encoding(tok, "utf-8") < 0) {
            واپس -1;
        }
    }
    ورنہ {
        Py_ssize_t پہلا_original_len = raw_سطر_لمبائی(raw, raw_len);
        _PyTok_Chunk پہلا = {
            .ڈیٹا = raw,
            .len = پہلا_original_len,
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        _PyTok_Chunk دوسرا = {0};
        صحیح have_دوسرا = پہلا_original_len < raw_len;
        اگر (have_دوسرا) {
            دوسرا.ڈیٹا = raw + پہلا_original_len;
            دوسرا.len = raw_سطر_لمبائی(دوسرا.ڈیٹا,
                                         raw_len - پہلا_original_len);
        }
        Py_ssize_t bom_len;
        Py_Tok_Encoding_نتیجہ detection = _PyTok_DetectEncoding(
            tok, &پہلا, have_دوسرا ? &دوسرا : تہی, 1, &bom_len);
        اگر (detection == _PYTOK_ENCODING_ERROR) {
            واپس -1;
        }
        raw += bom_len;
        raw_len -= bom_len;
    }

    _PyTok_Chunk decoded = {
        .ڈیٹا = raw,
        .len = raw_len,
        .ownership = _PYTOK_CHUNK_BORROWED,
    };
    ثابت حرف *encoding = tok->reader->encoding;
    اگر (encoding != تہی && strcmp(encoding, "utf-8") != 0) {
        اگر (_PyTok_DecodeOnce(
                tok, &decoded, encoding, تہی) < 0) {
            واپس -1;
        }
    }

    صحیح stored = store_prepared_ماخذ(
        tok, decoded.ڈیٹا, decoded.len, preserve_crlf, exec_داخلہ);
    _PyTok_ChunkClear(&decoded);
    اگر (stored < 0) {
        واپس -1;
    }
    اگر (!utf8_only &&
            (encoding == تہی || strcmp(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(Py_Tok_ماخذ_ڈیٹا(&tok->ماخذ), tok, 1)) {
        واپس -1;
    }
    واپس 0;
}

صحیح
Py_Tok_آغاز_Decoder(ساخت tok_state *tok, ثابت حرف *errors)
{
    _PyTok_Reader *reader = tok->reader;
    اگر (reader->encoding == تہی || reader->decoder != تہی) {
        واپس 0;
    }
    اگر (reader->قسم == _PYTOK_READER_FILE &&
            strcmp(reader->encoding, "utf-8") == 0) {
        واپس 0;
    }

    PyObject *codec = Py_Codec_Lookup_متن_Encoding(reader->encoding, تہی);
    اگر (codec != تہی) {
        reader->decoder = _PyCodecInfo_GetIncrementalDecoder(codec, errors);
        Py_DECREF(codec);
    }
    اگر (reader->decoder == تہی) {
        tok->مکمل = PyErr_ExceptionMatches(Py_Exc_حافظہ_Error)
            ? E_NOMEM : E_DECODE;
        اگر (reader->قسم == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != تہی ? tok->filename : Py_None);
        }
        واپس -1;
    }
    واپس 0;
}

صحیح
_PyTok_DecodeChunk(ساخت tok_state *tok, _PyTok_Chunk *chunk, صحیح final)
{
    _PyTok_Reader *reader = tok->reader;
    اگر (reader->decoder == تہی) {
        واپس 0;
    }
    صحیح strip_bom = reader->قسم == _PYTOK_READER_READLINE &&
        chunk->len >= UTF16_BOM_لمبائی &&
        (memcmp(chunk->ڈیٹا, UTF16_LE_BOM, UTF16_BOM_لمبائی) == 0 ||
         memcmp(chunk->ڈیٹا, UTF16_BE_BOM, UTF16_BOM_لمبائی) == 0);
    PyObject *داخلہ;
    اگر (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            Py_بائٹیں_Check(chunk->owner) &&
            chunk->ڈیٹا == Py_بائٹیں_AS_STRING(chunk->owner)) {
        داخلہ = Py_NewRef(chunk->owner);
    }
    ورنہ {
        داخلہ = Py_بائٹیں_From_String_And_سائز(chunk->ڈیٹا, chunk->len);
    }
    اگر (داخلہ == تہی) {
        tok->مکمل = E_NOMEM;
        واپس -1;
    }
    PyObject *unicode = Py_Object_Call_طریقہ_Obj_دلائل(
        reader->decoder, &_Py_ID(decode), داخلہ,
        final ? Py_True : Py_False, تہی);
    Py_DECREF(داخلہ);
    اگر (unicode == تہی) {
        tok->مکمل = PyErr_ExceptionMatches(Py_Exc_حافظہ_Error)
            ? E_NOMEM : E_DECODE;
        اگر (reader->قسم == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != تہی ? tok->filename : Py_None);
        }
        واپس -1;
    }
    اگر (!PyUnicode_Check(unicode)) {
        PyErr_Format(PyExc_TypeError,
                     "decoder should return a string result, not '%.200s'",
                     Py_TYPE(unicode)->tp_name);
        Py_DECREF(unicode);
        tok->مکمل = E_DECODE;
        واپس -1;
    }
    واپس chunk_سیٹ_unicode(tok, chunk, unicode, strip_bom);
}

صحیح
Py_Tok_Decoder_Has_Buffered_داخلہ(ساخت tok_state *tok)
{
    اگر (tok->reader->decoder == تہی) {
        واپس 0;
    }
    PyObject *state = Py_Object_Call_طریقہ_No_دلائل(
        tok->reader->decoder, &_Py_ID(getstate));
    اگر (state == تہی) {
        tok->مکمل = PyErr_ExceptionMatches(Py_Exc_حافظہ_Error)
            ? E_NOMEM : E_DECODE;
        واپس -1;
    }
    اگر (!PyTuple_Check(state) || Py_Tuple_GET_سائز(state) != 2 ||
            !Py_بائٹیں_Check(Py_Tuple_GET_شے(state, 0)) ||
            !PyLong_Check(Py_Tuple_GET_شے(state, 1))) {
        Py_DECREF(state);
        Py_Err_سیٹ_String(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->مکمل = E_DECODE;
        واپس -1;
    }
    صحیح pending = Py_بائٹیں_GET_سائز(Py_Tuple_GET_شے(state, 0)) != 0;
    Py_DECREF(state);
    واپس pending;
}
