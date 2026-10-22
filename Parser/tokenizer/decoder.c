#ضمن "Python.h"
#ضمن "pycore_codecs.h"
#ضمن "pycore_global_strings.h"
#ضمن "pycore_runtime.h"
#ضمن "errcode.h"

#ضمن "reader_internal.h"
#ضمن "helpers.h"
#ضمن "../lexer/state.h"

#تعریف_کړه UNICODE_BOM 0xFEFF
#تعریف_کړه UTF16_LE_BOM "\xff\xfe"
#تعریف_کړه UTF16_BE_BOM "\xfe\xff"
#تعریف_کړه UTF16_BOM_اوږدتیا ((Py_ssize_t)(حجم(UTF16_LE_BOM) - 1))
#تعریف_کړه UTF8_BOM "\xef\xbb\xbf"
#تعریف_کړه UTF8_BOM_اوږدتیا ((Py_ssize_t)(حجم(UTF8_BOM) - 1))

حرف *
Py_Tok_Copy_بایټونه(ثابت حرف *معلومات, Py_ssize_t len)
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
    حافظه_نقل(copy, معلومات, (اندازه_t)len);
    copy[len] = '\0';
    أرجع copy;
}

ساكن فراغ
chunk_release_معلومات(_PyTok_Chunk *chunk)
{
    بدل (chunk->ownership) {
        حالة _PYTOK_CHUNK_BORROWED:
            اكسر;
        حالة _PYTOK_CHUNK_PYMEM:
            PyMem_Free(chunk->معلومات);
            اكسر;
        حالة _PYTOK_CHUNK_PYOBJECT:
            Py_DECREF(chunk->owner);
            اكسر;
    }
}

فراغ
_PyTok_ChunkClear(_PyTok_Chunk *chunk)
{
    chunk_release_معلومات(chunk);
    *chunk = (_PyTok_Chunk){0};
}

ساكن صحيح
chunk_سېټ_unicode(بنية tok_state *tok, _PyTok_Chunk *chunk,
                  PyObject *unicode, صحيح strip_bom)
{
    Py_ssize_t utf8_len;
    ثابت حرف *utf8 = Py_Unicode_As_UTF8_And_اندازه(unicode, &utf8_len);
    إذا (utf8 == عدم) {
        Py_DECREF(unicode);
        tok->بشپړ = PyErr_ExceptionMatches(Py_Exc_حافظه_Error)
            ? E_NOMEM : E_DECODE;
        أرجع -1;
    }
    إذا (strip_bom && Py_Unicode_GET_اوږدتیا(unicode) > 0 &&
            PyUnicode_ReadChar(unicode, 0) == UNICODE_BOM) {
        utf8 += UTF8_BOM_اوږدتیا;
        utf8_len -= UTF8_BOM_اوږدتیا;
    }
    chunk_release_معلومات(chunk);
    chunk->owner = unicode;
    chunk->معلومات = (حرف *)utf8;
    chunk->len = utf8_len;
    chunk->ownership = _PYTOK_CHUNK_PYOBJECT;
    أرجع 0;
}

حرف *
_PyTok_NormalizeNewlines(ثابت حرف *معلومات, Py_ssize_t len,
                         Py_ssize_t *بهر_len)
{
    إذا (len < 0 || len == PY_SSIZE_T_MAX) {
        Py_Err_No_حافظه();
        أرجع عدم;
    }
    حرف *پایله = PyMem_Malloc((اندازه_t)len + 1);
    إذا (پایله == عدم) {
        Py_Err_No_حافظه();
        أرجع عدم;
    }
    Py_ssize_t اكتب = 0;
    لكل (Py_ssize_t اقرأ = 0; اقرأ < len; اقرأ++) {
        حرف c = معلومات[اقرأ];
        إذا (c == '\r') {
            إذا (اقرأ + 1 < len && معلومات[اقرأ + 1] == '\n') {
                اقرأ++;
            }
            c = '\n';
        }
        پایله[اكتب++] = c;
    }
    پایله[اكتب] = '\0';
    *بهر_len = اكتب;
    أرجع پایله;
}

صحيح
Py_Tok_سېټ_Encoding(بنية tok_state *tok, ثابت حرف *encoding)
{
    حرف *copy = Py_Tok_Copy_بایټونه(encoding, مزي_اوږدوالی(encoding));
    إذا (copy == عدم) {
        tok->بشپړ = E_NOMEM;
        أرجع -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    أرجع 0;
}

ساكن صحيح
پیدا_کړه_cookie(ثابت حرف *کرښه, Py_ssize_t len, حرف **encoding, صحيح *لټون_بل)
{
    Py_ssize_t i = 0;
    *encoding = عدم;
    *لټون_بل = 1;
    لكل (; i < len; i++) {
        إذا (کرښه[i] == '#') {
            اكسر;
        }
        إذا (کرښه[i] == '\n' || کرښه[i] == '\r') {
            أرجع 0;
        }
        إذا (کرښه[i] != ' ' && کرښه[i] != '\t' && کرښه[i] != '\f') {
            *لټون_بل = 0;
            أرجع 0;
        }
    }
    ثابت Py_ssize_t marker_len = حجم("coding") - 1;
    لكل (; i < len - marker_len; i++) {
        إذا (memcmp(کرښه + i, "coding", marker_len) != 0) {
            استمر;
        }
        ثابت حرف *cursor = کرښه + i + marker_len;
        إذا (*cursor != ':' && *cursor != '=') {
            استمر;
        }
        افعل {
            cursor++;
        } طالما (cursor < کرښه + len &&
                 (*cursor == ' ' || *cursor == '\t'));
        ثابت حرف *پیل = cursor;
        طالما (cursor < کرښه + len &&
                (Py_ISALNUM(*cursor) || *cursor == '-' ||
                 *cursor == '_' || *cursor == '.')) {
            cursor++;
        }
        إذا (cursor == پیل) {
            استمر;
        }
        Py_ssize_t encoding_len = cursor - پیل;
        // Include the longest recognized prefix and its NUL terminator.
        حرف normalized[حجم("iso-latin-1-")];
        ثابت Py_ssize_t مختاړی_len = Py_MIN(
            encoding_len, (Py_ssize_t)حجم(normalized) - 1);
        Py_ssize_t n;
        لكل (n = 0; n < مختاړی_len; n++) {
            normalized[n] = پیل[n] == '_' ? '-' : Py_TOLOWER(پیل[n]);
        }
        normalized[n] = '\0';
        ثابت حرف *canonical = پیل;
        إذا (strcmp(normalized, "utf-8") == 0 ||
                مزي_پرتله(normalized, "utf-8-", حجم("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        وإلا إذا (strcmp(normalized, "latin-1") == 0 ||
                 strcmp(normalized, "iso-8859-1") == 0 ||
                 strcmp(normalized, "iso-latin-1") == 0 ||
                 مزي_پرتله(normalized, "latin-1-", حجم("latin-1-") - 1) == 0 ||
                 مزي_پرتله(normalized, "iso-8859-1-", حجم("iso-8859-1-") - 1) == 0 ||
                 مزي_پرتله(normalized, "iso-latin-1-", حجم("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        إذا (canonical != پیل) {
            encoding_len = مزي_اوږدوالی(canonical);
        }
        *encoding = Py_Tok_Copy_بایټونه(canonical, encoding_len);
        إذا (*encoding == عدم) {
            أرجع -1;
        }
        *لټون_بل = 0;
        أرجع 0;
    }
    أرجع 0;
}

Py_Tok_Encoding_پایله
_PyTok_DetectEncoding(بنية tok_state *tok, ثابت _PyTok_Chunk *لومړی,
                      ثابت _PyTok_Chunk *دوهم, صحيح final,
                      Py_ssize_t *bom_len)
{
    صحيح bom = لومړی->len >= UTF8_BOM_اوږدتیا &&
        memcmp(لومړی->معلومات, UTF8_BOM, UTF8_BOM_اوږدتیا) == 0;
    *bom_len = bom ? UTF8_BOM_اوږدتیا : 0;

    حرف *cookie = عدم;
    صحيح لټون_بل = 0;
    صحيح cookie_کرښه = 1;
    ثابت حرف *لومړی_معلومات = لومړی->معلومات + *bom_len;
    Py_ssize_t لومړی_len = لومړی->len - *bom_len;
    إذا (پیدا_کړه_cookie(لومړی_معلومات, لومړی_len, &cookie, &لټون_بل) < 0) {
        أرجع _PYTOK_ENCODING_ERROR;
    }
    إذا (cookie == عدم && لټون_بل && دوهم != عدم) {
        إذا (پیدا_کړه_cookie(دوهم->معلومات, دوهم->len, &cookie, &لټون_بل) < 0) {
            أرجع _PYTOK_ENCODING_ERROR;
        }
        cookie_کرښه = 2;
    }
    وإلا إذا (cookie == عدم && لټون_بل && !final) {
        أرجع PYTOK_ENCODING_NEED_دوهم_کرښه;
    }

    إذا (cookie == عدم) {
        إذا (bom && Py_Tok_سېټ_Encoding(tok, "utf-8") < 0) {
            أرجع _PYTOK_ENCODING_ERROR;
        }
        أرجع PYTOK_ENCODING_بشپړ;
    }
    إذا (bom && strcmp(cookie, "utf-8") != 0) {
        ثابت _PyTok_Chunk *کرښه = cookie_کرښه == 2 ? دوهم : لومړی;
        ثابت حرف *کرښه_معلومات = کرښه->معلومات + (cookie_کرښه == 1 ? *bom_len : 0);
        Py_ssize_t کرښه_len = کرښه->len - (cookie_کرښه == 1 ? *bom_len : 0);
        صحيح پای_ټکی_ستون = (صحيح)Py_MIN(کرښه_len, لوی_عدد);
        إذا (پای_ټکی_ستون > 0 && (کرښه_معلومات[پای_ټکی_ستون - 1] == '\n' ||
                            کرښه_معلومات[پای_ټکی_ستون - 1] == '\r')) {
            پای_ټکی_ستون--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, کرښه_معلومات, 0, cookie_کرښه, 0, پای_ټکی_ستون,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        أرجع _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    أرجع PYTOK_ENCODING_بشپړ;
}

صحيح
_PyTok_DecodeOnce(بنية tok_state *tok, _PyTok_Chunk *chunk,
                  ثابت حرف *encoding, ثابت حرف *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->معلومات, chunk->len, encoding, errors);
    إذا (unicode == عدم) {
        tok->بشپړ = PyErr_ExceptionMatches(Py_Exc_حافظه_Error)
            ? E_NOMEM : E_DECODE;
        أرجع -1;
    }
    أرجع chunk_سېټ_unicode(tok, chunk, unicode, 0);
}

ساكن Py_ssize_t
raw_کرښه_اوږدتیا(ثابت حرف *معلومات, Py_ssize_t len)
{
    لكل (Py_ssize_t i = 0; i < len; i++) {
        إذا (معلومات[i] == '\n') {
            أرجع i + 1;
        }
        إذا (معلومات[i] == '\r') {
            أرجع i + 1 < len && معلومات[i + 1] == '\n' ? i + 2 : i + 1;
        }
    }
    أرجع len;
}

ساكن صحيح
store_prepared_سرچینه(بنية tok_state *tok, ثابت حرف *معلومات, Py_ssize_t len,
                      صحيح preserve_crlf, صحيح جمع_کړه_final_newline)
{
    Py_ssize_t pos = 0;
    طالما (pos < len) {
        Py_ssize_t raw_کرښه_len;
        إذا (preserve_crlf) {
            ثابت حرف *newline = memchr(معلومات + pos, '\n', len - pos);
            raw_کرښه_len = newline == عدم
                ? len - pos : newline - معلومات - pos + 1;
        }
        وإلا {
            raw_کرښه_len = raw_کرښه_اوږدتیا(معلومات + pos, len - pos);
        }
        صحيح terminated = preserve_crlf
            ? معلومات[pos + raw_کرښه_len - 1] == '\n'
            : معلومات[pos + raw_کرښه_len - 1] == '\n' ||
              معلومات[pos + raw_کرښه_len - 1] == '\r';
        صحيح جمع_کړه_newline = جمع_کړه_final_newline &&
            pos + raw_کرښه_len == len && !terminated;
        ثابت حرف *کرښه = معلومات + pos;
        // raw_line_length stops at the first CR or LF, so any CR is in
        // the line terminator.
        Py_ssize_t terminator_len = 0;
        إذا (!preserve_crlf) {
            إذا (کرښه[raw_کرښه_len - 1] == '\r') {
                terminator_len = 1;
            }
            وإلا إذا (raw_کرښه_len > 1 && کرښه[raw_کرښه_len - 2] == '\r') {
                terminator_len = 2;
            }
        }
        _PyTok_Off appended;
        إذا (terminator_len || جمع_کړه_newline) {
            appended = Py_Tok_سرچینه_Append_کرښه_With_Newline(
                &tok->سرچینه, کرښه, raw_کرښه_len - terminator_len);
        }
        وإلا {
            appended = Py_Tok_سرچینه_Append_کرښه(&tok->سرچینه, کرښه, raw_کرښه_len);
        }
        إذا (appended < 0) {
            tok->بشپړ = PyErr_ExceptionMatches(Py_Exc_حافظه_Error)
                ? E_NOMEM : E_ERROR;
            أرجع -1;
        }
        tok->reader->prepared_final_newline = جمع_کړه_newline;
        pos += raw_کرښه_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    إذا (جمع_کړه_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || معلومات[len - 1] != '\r'))
    {
        إذا (Py_Tok_سرچینه_Append_کرښه(&tok->سرچینه, "\n", 1) < 0) {
            أرجع -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    أرجع 0;
}

صحيح
_PyTok_PrepareString(بنية tok_state *tok, ثابت حرف *ننوت, صحيح utf8_only,
                     صحيح exec_ننوت, صحيح preserve_crlf)
{
    Py_ssize_t raw_len = مزي_اوږدوالی(ننوت);
    حرف *raw = (حرف *)ننوت;

    إذا (utf8_only) {
        إذا (Py_Tok_سېټ_Encoding(tok, "utf-8") < 0) {
            أرجع -1;
        }
    }
    وإلا {
        Py_ssize_t لومړی_original_len = raw_کرښه_اوږدتیا(raw, raw_len);
        _PyTok_Chunk لومړی = {
            .معلومات = raw,
            .len = لومړی_original_len,
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        _PyTok_Chunk دوهم = {0};
        صحيح have_دوهم = لومړی_original_len < raw_len;
        إذا (have_دوهم) {
            دوهم.معلومات = raw + لومړی_original_len;
            دوهم.len = raw_کرښه_اوږدتیا(دوهم.معلومات,
                                         raw_len - لومړی_original_len);
        }
        Py_ssize_t bom_len;
        Py_Tok_Encoding_پایله detection = _PyTok_DetectEncoding(
            tok, &لومړی, have_دوهم ? &دوهم : عدم, 1, &bom_len);
        إذا (detection == _PYTOK_ENCODING_ERROR) {
            أرجع -1;
        }
        raw += bom_len;
        raw_len -= bom_len;
    }

    _PyTok_Chunk decoded = {
        .معلومات = raw,
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

    صحيح stored = store_prepared_سرچینه(
        tok, decoded.معلومات, decoded.len, preserve_crlf, exec_ننوت);
    _PyTok_ChunkClear(&decoded);
    إذا (stored < 0) {
        أرجع -1;
    }
    إذا (!utf8_only &&
            (encoding == عدم || strcmp(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(Py_Tok_سرچینه_معلومات(&tok->سرچینه), tok, 1)) {
        أرجع -1;
    }
    أرجع 0;
}

صحيح
Py_Tok_پیل_Decoder(بنية tok_state *tok, ثابت حرف *errors)
{
    _PyTok_Reader *reader = tok->reader;
    إذا (reader->encoding == عدم || reader->decoder != عدم) {
        أرجع 0;
    }
    إذا (reader->ډول == _PYTOK_READER_FILE &&
            strcmp(reader->encoding, "utf-8") == 0) {
        أرجع 0;
    }

    PyObject *codec = Py_Codec_Lookup_متن_Encoding(reader->encoding, عدم);
    إذا (codec != عدم) {
        reader->decoder = _PyCodecInfo_GetIncrementalDecoder(codec, errors);
        Py_DECREF(codec);
    }
    إذا (reader->decoder == عدم) {
        tok->بشپړ = PyErr_ExceptionMatches(Py_Exc_حافظه_Error)
            ? E_NOMEM : E_DECODE;
        إذا (reader->ډول == _PYTOK_READER_FILE) {
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
    صحيح strip_bom = reader->ډول == _PYTOK_READER_READLINE &&
        chunk->len >= UTF16_BOM_اوږدتیا &&
        (memcmp(chunk->معلومات, UTF16_LE_BOM, UTF16_BOM_اوږدتیا) == 0 ||
         memcmp(chunk->معلومات, UTF16_BE_BOM, UTF16_BOM_اوږدتیا) == 0);
    PyObject *ننوت;
    إذا (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            Py_بایټونه_Check(chunk->owner) &&
            chunk->معلومات == Py_بایټونه_AS_STRING(chunk->owner)) {
        ننوت = Py_NewRef(chunk->owner);
    }
    وإلا {
        ننوت = Py_بایټونه_From_String_And_اندازه(chunk->معلومات, chunk->len);
    }
    إذا (ننوت == عدم) {
        tok->بشپړ = E_NOMEM;
        أرجع -1;
    }
    PyObject *unicode = Py_Object_Call_لاره_Obj_دلیلونه(
        reader->decoder, &_Py_ID(decode), ننوت,
        final ? Py_True : Py_False, عدم);
    Py_DECREF(ننوت);
    إذا (unicode == عدم) {
        tok->بشپړ = PyErr_ExceptionMatches(Py_Exc_حافظه_Error)
            ? E_NOMEM : E_DECODE;
        إذا (reader->ډول == _PYTOK_READER_FILE) {
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
        tok->بشپړ = E_DECODE;
        أرجع -1;
    }
    أرجع chunk_سېټ_unicode(tok, chunk, unicode, strip_bom);
}

صحيح
Py_Tok_Decoder_Has_Buffered_ننوت(بنية tok_state *tok)
{
    إذا (tok->reader->decoder == عدم) {
        أرجع 0;
    }
    PyObject *state = Py_Object_Call_لاره_No_دلیلونه(
        tok->reader->decoder, &_Py_ID(getstate));
    إذا (state == عدم) {
        tok->بشپړ = PyErr_ExceptionMatches(Py_Exc_حافظه_Error)
            ? E_NOMEM : E_DECODE;
        أرجع -1;
    }
    إذا (!PyTuple_Check(state) || Py_Tuple_GET_اندازه(state) != 2 ||
            !Py_بایټونه_Check(Py_Tuple_GET_توکی(state, 0)) ||
            !PyLong_Check(Py_Tuple_GET_توکی(state, 1))) {
        Py_DECREF(state);
        Py_Err_سېټ_String(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->بشپړ = E_DECODE;
        أرجع -1;
    }
    صحيح pending = Py_بایټونه_GET_اندازه(Py_Tuple_GET_توکی(state, 0)) != 0;
    Py_DECREF(state);
    أرجع pending;
}
