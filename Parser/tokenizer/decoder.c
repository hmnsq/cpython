#ضمن "Python.h"
#ضمن "pycore_codecs.h"
#ضمن "pycore_global_strings.h"
#ضمن "pycore_runtime.h"
#ضمن "errcode.h"

#ضمن "reader_internal.h"
#ضمن "helpers.h"
#ضمن "../lexer/state.h"

#تعریف_کن UNICODE_BOM 0xFEFF
#تعریف_کن UTF16_LE_BOM "\xff\xfe"
#تعریف_کن UTF16_BE_BOM "\xfe\xff"
#تعریف_کن UTF16_BOM_درازی ((Py_ssize_t)(حجم(UTF16_LE_BOM) - 1))
#تعریف_کن UTF8_BOM "\xef\xbb\xbf"
#تعریف_کن UTF8_BOM_درازی ((Py_ssize_t)(حجم(UTF8_BOM) - 1))

حرف *
Py_Tok_Copy_بایتها(ثابت حرف *داده, Py_ssize_t len)
{
    إذا (len < 0 || len == PY_SSIZE_T_MAX) {
        Py_Err_No_حافظه();
        أرجع عدم;
    }
    حرف *copy = PyMem_Malloc((اندازه_t)len + 1);
    إذا (copy == عدم) {
        Py_Err_No_حافظه();
        أرجع عدم;
    }
    کپی_حافظه(copy, داده, (اندازه_t)len);
    copy[len] = '\0';
    أرجع copy;
}

ساكن فراغ
chunk_release_داده(_PyTok_Chunk *chunk)
{
    بدل (chunk->ownership) {
        حالة _PYTOK_CHUNK_BORROWED:
            اكسر;
        حالة _PYTOK_CHUNK_PYMEM:
            PyMem_Free(chunk->داده);
            اكسر;
        حالة _PYTOK_CHUNK_PYOBJECT:
            Py_DECREF(chunk->owner);
            اكسر;
    }
}

فراغ
_PyTok_ChunkClear(_PyTok_Chunk *chunk)
{
    chunk_release_داده(chunk);
    *chunk = (_PyTok_Chunk){0};
}

ساكن صحيح
chunk_مجموعه_unicode(بنية tok_state *tok, _PyTok_Chunk *chunk,
                  PyObject *unicode, صحيح strip_bom)
{
    Py_ssize_t utf8_len;
    ثابت حرف *utf8 = Py_Unicode_As_UTF8_And_اندازه(unicode, &utf8_len);
    إذا (utf8 == عدم) {
        Py_DECREF(unicode);
        tok->انجامشده = PyErr_ExceptionMatches(Py_Exc_حافظه_Error)
            ? E_NOMEM : E_DECODE;
        أرجع -1;
    }
    إذا (strip_bom && Py_Unicode_GET_درازی(unicode) > 0 &&
            PyUnicode_ReadChar(unicode, 0) == UNICODE_BOM) {
        utf8 += UTF8_BOM_درازی;
        utf8_len -= UTF8_BOM_درازی;
    }
    chunk_release_داده(chunk);
    chunk->owner = unicode;
    chunk->داده = (حرف *)utf8;
    chunk->len = utf8_len;
    chunk->ownership = _PYTOK_CHUNK_PYOBJECT;
    أرجع 0;
}

حرف *
_PyTok_NormalizeNewlines(ثابت حرف *داده, Py_ssize_t len,
                         Py_ssize_t *بیرون_len)
{
    إذا (len < 0 || len == PY_SSIZE_T_MAX) {
        Py_Err_No_حافظه();
        أرجع عدم;
    }
    حرف *نتیجه = PyMem_Malloc((اندازه_t)len + 1);
    إذا (نتیجه == عدم) {
        Py_Err_No_حافظه();
        أرجع عدم;
    }
    Py_ssize_t اكتب = 0;
    لكل (Py_ssize_t اقرأ = 0; اقرأ < len; اقرأ++) {
        حرف c = داده[اقرأ];
        إذا (c == '\r') {
            إذا (اقرأ + 1 < len && داده[اقرأ + 1] == '\n') {
                اقرأ++;
            }
            c = '\n';
        }
        نتیجه[اكتب++] = c;
    }
    نتیجه[اكتب] = '\0';
    *بیرون_len = اكتب;
    أرجع نتیجه;
}

صحيح
Py_Tok_مجموعه_Encoding(بنية tok_state *tok, ثابت حرف *encoding)
{
    حرف *copy = Py_Tok_Copy_بایتها(encoding, طول_رشته(encoding));
    إذا (copy == عدم) {
        tok->انجامشده = E_NOMEM;
        أرجع -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    أرجع 0;
}

ساكن صحيح
پیدا_کن_cookie(ثابت حرف *سطر, Py_ssize_t len, حرف **encoding, صحيح *پیمایش_بعدی)
{
    Py_ssize_t i = 0;
    *encoding = عدم;
    *پیمایش_بعدی = 1;
    لكل (; i < len; i++) {
        إذا (سطر[i] == '#') {
            اكسر;
        }
        إذا (سطر[i] == '\n' || سطر[i] == '\r') {
            أرجع 0;
        }
        إذا (سطر[i] != ' ' && سطر[i] != '\t' && سطر[i] != '\f') {
            *پیمایش_بعدی = 0;
            أرجع 0;
        }
    }
    ثابت Py_ssize_t marker_len = حجم("coding") - 1;
    لكل (; i < len - marker_len; i++) {
        إذا (memcmp(سطر + i, "coding", marker_len) != 0) {
            استمر;
        }
        ثابت حرف *cursor = سطر + i + marker_len;
        إذا (*cursor != ':' && *cursor != '=') {
            استمر;
        }
        افعل {
            cursor++;
        } طالما (cursor < سطر + len &&
                 (*cursor == ' ' || *cursor == '\t'));
        ثابت حرف *شروع = cursor;
        طالما (cursor < سطر + len &&
                (Py_ISALNUM(*cursor) || *cursor == '-' ||
                 *cursor == '_' || *cursor == '.')) {
            cursor++;
        }
        إذا (cursor == شروع) {
            استمر;
        }
        Py_ssize_t encoding_len = cursor - شروع;
        // Include the longest recognized prefix and its NUL terminator.
        حرف normalized[حجم("iso-latin-1-")];
        ثابت Py_ssize_t پیشوند_len = Py_MIN(
            encoding_len, (Py_ssize_t)حجم(normalized) - 1);
        Py_ssize_t n;
        لكل (n = 0; n < پیشوند_len; n++) {
            normalized[n] = شروع[n] == '_' ? '-' : Py_TOLOWER(شروع[n]);
        }
        normalized[n] = '\0';
        ثابت حرف *canonical = شروع;
        إذا (strcmp(normalized, "utf-8") == 0 ||
                مقایسه_رشته(normalized, "utf-8-", حجم("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        وإلا إذا (strcmp(normalized, "latin-1") == 0 ||
                 strcmp(normalized, "iso-8859-1") == 0 ||
                 strcmp(normalized, "iso-latin-1") == 0 ||
                 مقایسه_رشته(normalized, "latin-1-", حجم("latin-1-") - 1) == 0 ||
                 مقایسه_رشته(normalized, "iso-8859-1-", حجم("iso-8859-1-") - 1) == 0 ||
                 مقایسه_رشته(normalized, "iso-latin-1-", حجم("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        إذا (canonical != شروع) {
            encoding_len = طول_رشته(canonical);
        }
        *encoding = Py_Tok_Copy_بایتها(canonical, encoding_len);
        إذا (*encoding == عدم) {
            أرجع -1;
        }
        *پیمایش_بعدی = 0;
        أرجع 0;
    }
    أرجع 0;
}

Py_Tok_Encoding_نتیجه
_PyTok_DetectEncoding(بنية tok_state *tok, ثابت _PyTok_Chunk *اولی,
                      ثابت _PyTok_Chunk *دومی, صحيح final,
                      Py_ssize_t *bom_len)
{
    صحيح bom = اولی->len >= UTF8_BOM_درازی &&
        memcmp(اولی->داده, UTF8_BOM, UTF8_BOM_درازی) == 0;
    *bom_len = bom ? UTF8_BOM_درازی : 0;

    حرف *cookie = عدم;
    صحيح پیمایش_بعدی = 0;
    صحيح cookie_سطر = 1;
    ثابت حرف *اولی_داده = اولی->داده + *bom_len;
    Py_ssize_t اولی_len = اولی->len - *bom_len;
    إذا (پیدا_کن_cookie(اولی_داده, اولی_len, &cookie, &پیمایش_بعدی) < 0) {
        أرجع _PYTOK_ENCODING_ERROR;
    }
    إذا (cookie == عدم && پیمایش_بعدی && دومی != عدم) {
        إذا (پیدا_کن_cookie(دومی->داده, دومی->len, &cookie, &پیمایش_بعدی) < 0) {
            أرجع _PYTOK_ENCODING_ERROR;
        }
        cookie_سطر = 2;
    }
    وإلا إذا (cookie == عدم && پیمایش_بعدی && !final) {
        أرجع PYTOK_ENCODING_NEED_دومی_سطر;
    }

    إذا (cookie == عدم) {
        إذا (bom && Py_Tok_مجموعه_Encoding(tok, "utf-8") < 0) {
            أرجع _PYTOK_ENCODING_ERROR;
        }
        أرجع PYTOK_ENCODING_انجامشده;
    }
    إذا (bom && strcmp(cookie, "utf-8") != 0) {
        ثابت _PyTok_Chunk *سطر = cookie_سطر == 2 ? دومی : اولی;
        ثابت حرف *سطر_داده = سطر->داده + (cookie_سطر == 1 ? *bom_len : 0);
        Py_ssize_t سطر_len = سطر->len - (cookie_سطر == 1 ? *bom_len : 0);
        صحيح انتها_ستون = (صحيح)Py_MIN(سطر_len, بیشترین_صحیح);
        إذا (انتها_ستون > 0 && (سطر_داده[انتها_ستون - 1] == '\n' ||
                            سطر_داده[انتها_ستون - 1] == '\r')) {
            انتها_ستون--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, سطر_داده, 0, cookie_سطر, 0, انتها_ستون,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        أرجع _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    أرجع PYTOK_ENCODING_انجامشده;
}

صحيح
_PyTok_DecodeOnce(بنية tok_state *tok, _PyTok_Chunk *chunk,
                  ثابت حرف *encoding, ثابت حرف *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->داده, chunk->len, encoding, errors);
    إذا (unicode == عدم) {
        tok->انجامشده = PyErr_ExceptionMatches(Py_Exc_حافظه_Error)
            ? E_NOMEM : E_DECODE;
        أرجع -1;
    }
    أرجع chunk_مجموعه_unicode(tok, chunk, unicode, 0);
}

ساكن Py_ssize_t
raw_سطر_درازی(ثابت حرف *داده, Py_ssize_t len)
{
    لكل (Py_ssize_t i = 0; i < len; i++) {
        إذا (داده[i] == '\n') {
            أرجع i + 1;
        }
        إذا (داده[i] == '\r') {
            أرجع i + 1 < len && داده[i + 1] == '\n' ? i + 2 : i + 1;
        }
    }
    أرجع len;
}

ساكن صحيح
store_prepared_منبع(بنية tok_state *tok, ثابت حرف *داده, Py_ssize_t len,
                      صحيح preserve_crlf, صحيح جمع_کن_final_newline)
{
    Py_ssize_t pos = 0;
    طالما (pos < len) {
        Py_ssize_t raw_سطر_len;
        إذا (preserve_crlf) {
            ثابت حرف *newline = memchr(داده + pos, '\n', len - pos);
            raw_سطر_len = newline == عدم
                ? len - pos : newline - داده - pos + 1;
        }
        وإلا {
            raw_سطر_len = raw_سطر_درازی(داده + pos, len - pos);
        }
        صحيح terminated = preserve_crlf
            ? داده[pos + raw_سطر_len - 1] == '\n'
            : داده[pos + raw_سطر_len - 1] == '\n' ||
              داده[pos + raw_سطر_len - 1] == '\r';
        صحيح جمع_کن_newline = جمع_کن_final_newline &&
            pos + raw_سطر_len == len && !terminated;
        ثابت حرف *سطر = داده + pos;
        // raw_line_length stops at the first CR or LF, so any CR is in
        // the line terminator.
        Py_ssize_t terminator_len = 0;
        إذا (!preserve_crlf) {
            إذا (سطر[raw_سطر_len - 1] == '\r') {
                terminator_len = 1;
            }
            وإلا إذا (raw_سطر_len > 1 && سطر[raw_سطر_len - 2] == '\r') {
                terminator_len = 2;
            }
        }
        _PyTok_Off appended;
        إذا (terminator_len || جمع_کن_newline) {
            appended = Py_Tok_منبع_Append_سطر_With_Newline(
                &tok->منبع, سطر, raw_سطر_len - terminator_len);
        }
        وإلا {
            appended = Py_Tok_منبع_Append_سطر(&tok->منبع, سطر, raw_سطر_len);
        }
        إذا (appended < 0) {
            tok->انجامشده = PyErr_ExceptionMatches(Py_Exc_حافظه_Error)
                ? E_NOMEM : E_ERROR;
            أرجع -1;
        }
        tok->reader->prepared_final_newline = جمع_کن_newline;
        pos += raw_سطر_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    إذا (جمع_کن_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || داده[len - 1] != '\r'))
    {
        إذا (Py_Tok_منبع_Append_سطر(&tok->منبع, "\n", 1) < 0) {
            أرجع -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    أرجع 0;
}

صحيح
_PyTok_PrepareString(بنية tok_state *tok, ثابت حرف *درون_داد, صحيح utf8_only,
                     صحيح exec_درون_داد, صحيح preserve_crlf)
{
    Py_ssize_t raw_len = طول_رشته(درون_داد);
    حرف *raw = (حرف *)درون_داد;

    إذا (utf8_only) {
        إذا (Py_Tok_مجموعه_Encoding(tok, "utf-8") < 0) {
            أرجع -1;
        }
    }
    وإلا {
        Py_ssize_t اولی_original_len = raw_سطر_درازی(raw, raw_len);
        _PyTok_Chunk اولی = {
            .داده = raw,
            .len = اولی_original_len,
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        _PyTok_Chunk دومی = {0};
        صحيح have_دومی = اولی_original_len < raw_len;
        إذا (have_دومی) {
            دومی.داده = raw + اولی_original_len;
            دومی.len = raw_سطر_درازی(دومی.داده,
                                         raw_len - اولی_original_len);
        }
        Py_ssize_t bom_len;
        Py_Tok_Encoding_نتیجه detection = _PyTok_DetectEncoding(
            tok, &اولی, have_دومی ? &دومی : عدم, 1, &bom_len);
        إذا (detection == _PYTOK_ENCODING_ERROR) {
            أرجع -1;
        }
        raw += bom_len;
        raw_len -= bom_len;
    }

    _PyTok_Chunk decoded = {
        .داده = raw,
        .len = raw_len,
        .ownership = _PYTOK_CHUNK_BORROWED,
    };
    ثابت حرف *encoding = tok->reader->encoding;
    إذا (encoding != عدم && strcmp(encoding, "utf-8") != 0) {
        إذا (_PyTok_DecodeOnce(
                tok, &decoded, encoding, عدم) < 0) {
            أرجع -1;
        }
    }

    صحيح stored = store_prepared_منبع(
        tok, decoded.داده, decoded.len, preserve_crlf, exec_درون_داد);
    _PyTok_ChunkClear(&decoded);
    إذا (stored < 0) {
        أرجع -1;
    }
    إذا (!utf8_only &&
            (encoding == عدم || strcmp(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(Py_Tok_منبع_داده(&tok->منبع), tok, 1)) {
        أرجع -1;
    }
    أرجع 0;
}

صحيح
Py_Tok_شروع_Decoder(بنية tok_state *tok, ثابت حرف *errors)
{
    _PyTok_Reader *reader = tok->reader;
    إذا (reader->encoding == عدم || reader->decoder != عدم) {
        أرجع 0;
    }
    إذا (reader->نوع == _PYTOK_READER_FILE &&
            strcmp(reader->encoding, "utf-8") == 0) {
        أرجع 0;
    }

    PyObject *codec = Py_Codec_Lookup_متن_Encoding(reader->encoding, عدم);
    إذا (codec != عدم) {
        reader->decoder = _PyCodecInfo_GetIncrementalDecoder(codec, errors);
        Py_DECREF(codec);
    }
    إذا (reader->decoder == عدم) {
        tok->انجامشده = PyErr_ExceptionMatches(Py_Exc_حافظه_Error)
            ? E_NOMEM : E_DECODE;
        إذا (reader->نوع == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != عدم ? tok->filename : Py_None);
        }
        أرجع -1;
    }
    أرجع 0;
}

صحيح
_PyTok_DecodeChunk(بنية tok_state *tok, _PyTok_Chunk *chunk, صحيح final)
{
    _PyTok_Reader *reader = tok->reader;
    إذا (reader->decoder == عدم) {
        أرجع 0;
    }
    صحيح strip_bom = reader->نوع == _PYTOK_READER_READLINE &&
        chunk->len >= UTF16_BOM_درازی &&
        (memcmp(chunk->داده, UTF16_LE_BOM, UTF16_BOM_درازی) == 0 ||
         memcmp(chunk->داده, UTF16_BE_BOM, UTF16_BOM_درازی) == 0);
    PyObject *درون_داد;
    إذا (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            Py_بایتها_Check(chunk->owner) &&
            chunk->داده == Py_بایتها_AS_STRING(chunk->owner)) {
        درون_داد = Py_NewRef(chunk->owner);
    }
    وإلا {
        درون_داد = Py_بایتها_From_String_And_اندازه(chunk->داده, chunk->len);
    }
    إذا (درون_داد == عدم) {
        tok->انجامشده = E_NOMEM;
        أرجع -1;
    }
    PyObject *unicode = Py_Object_Call_روش_Obj_آرگومانها(
        reader->decoder, &_Py_ID(decode), درون_داد,
        final ? Py_True : Py_False, عدم);
    Py_DECREF(درون_داد);
    إذا (unicode == عدم) {
        tok->انجامشده = PyErr_ExceptionMatches(Py_Exc_حافظه_Error)
            ? E_NOMEM : E_DECODE;
        إذا (reader->نوع == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != عدم ? tok->filename : Py_None);
        }
        أرجع -1;
    }
    إذا (!PyUnicode_Check(unicode)) {
        PyErr_Format(PyExc_TypeError,
                     "decoder should return a string result, not '%.200s'",
                     Py_TYPE(unicode)->tp_name);
        Py_DECREF(unicode);
        tok->انجامشده = E_DECODE;
        أرجع -1;
    }
    أرجع chunk_مجموعه_unicode(tok, chunk, unicode, strip_bom);
}

صحيح
Py_Tok_Decoder_Has_Buffered_درون_داد(بنية tok_state *tok)
{
    إذا (tok->reader->decoder == عدم) {
        أرجع 0;
    }
    PyObject *state = Py_Object_Call_روش_No_آرگومانها(
        tok->reader->decoder, &_Py_ID(getstate));
    إذا (state == عدم) {
        tok->انجامشده = PyErr_ExceptionMatches(Py_Exc_حافظه_Error)
            ? E_NOMEM : E_DECODE;
        أرجع -1;
    }
    إذا (!PyTuple_Check(state) || Py_Tuple_GET_اندازه(state) != 2 ||
            !Py_بایتها_Check(Py_Tuple_GET_قلم(state, 0)) ||
            !PyLong_Check(Py_Tuple_GET_قلم(state, 1))) {
        Py_DECREF(state);
        Py_Err_مجموعه_String(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->انجامشده = E_DECODE;
        أرجع -1;
    }
    صحيح pending = Py_بایتها_GET_اندازه(Py_Tuple_GET_قلم(state, 0)) != 0;
    Py_DECREF(state);
    أرجع pending;
}
