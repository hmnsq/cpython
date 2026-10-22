#ضمن "Python.h"
#ضمن "pycore_codecs.h"
#ضمن "pycore_global_strings.h"
#ضمن "pycore_runtime.h"
#ضمن "errcode.h"

#ضمن "reader_internal.h"
#ضمن "helpers.h"
#ضمن "../lexer/state.h"

#وصف_ڪريو UNICODE_BOM 0xFEFF
#وصف_ڪريو UTF16_LE_BOM "\xff\xfe"
#وصف_ڪريو UTF16_BE_BOM "\xfe\xff"
#وصف_ڪريو UTF16_BOM_ڊيگهائي ((Py_ssize_t)(حجم(UTF16_LE_BOM) - 1))
#وصف_ڪريو UTF8_BOM "\xef\xbb\xbf"
#وصف_ڪريو UTF8_BOM_ڊيگهائي ((Py_ssize_t)(حجم(UTF8_BOM) - 1))

حرف *
Py_Tok_Copy_بائيٽون(ثابت حرف *معلومات, Py_ssize_t len)
{
    إذا (len < 0 || len == PY_SSIZE_T_MAX) {
        Py_Err_No_يادگيري();
        أرجع عدم;
    }
    حرف *copy = PyMem_Malloc((سائيز_t)len + 1);
    إذا (copy == عدم) {
        Py_Err_No_يادگيري();
        أرجع عدم;
    }
    يادگيري_نقل(copy, معلومات, (سائيز_t)len);
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
chunk_سيٽ_unicode(بنية tok_state *tok, _PyTok_Chunk *chunk,
                  PyObject *unicode, صحيح strip_bom)
{
    Py_ssize_t utf8_len;
    ثابت حرف *utf8 = Py_Unicode_As_UTF8_And_سائيز(unicode, &utf8_len);
    إذا (utf8 == عدم) {
        Py_DECREF(unicode);
        tok->پورو = PyErr_ExceptionMatches(Py_Exc_يادگيري_Error)
            ? E_NOMEM : E_DECODE;
        أرجع -1;
    }
    إذا (strip_bom && Py_Unicode_GET_ڊيگهائي(unicode) > 0 &&
            PyUnicode_ReadChar(unicode, 0) == UNICODE_BOM) {
        utf8 += UTF8_BOM_ڊيگهائي;
        utf8_len -= UTF8_BOM_ڊيگهائي;
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
                         Py_ssize_t *ٻاهريون_len)
{
    إذا (len < 0 || len == PY_SSIZE_T_MAX) {
        Py_Err_No_يادگيري();
        أرجع عدم;
    }
    حرف *نتيجو = PyMem_Malloc((سائيز_t)len + 1);
    إذا (نتيجو == عدم) {
        Py_Err_No_يادگيري();
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
        نتيجو[اكتب++] = c;
    }
    نتيجو[اكتب] = '\0';
    *ٻاهريون_len = اكتب;
    أرجع نتيجو;
}

صحيح
Py_Tok_سيٽ_Encoding(بنية tok_state *tok, ثابت حرف *encoding)
{
    حرف *copy = Py_Tok_Copy_بائيٽون(encoding, لڙي_ڊيگهه(encoding));
    إذا (copy == عدم) {
        tok->پورو = E_NOMEM;
        أرجع -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    أرجع 0;
}

ساكن صحيح
ڳوليو_cookie(ثابت حرف *سٽ, Py_ssize_t len, حرف **encoding, صحيح *جاچ_ايندڙ)
{
    Py_ssize_t i = 0;
    *encoding = عدم;
    *جاچ_ايندڙ = 1;
    لكل (; i < len; i++) {
        إذا (سٽ[i] == '#') {
            اكسر;
        }
        إذا (سٽ[i] == '\n' || سٽ[i] == '\r') {
            أرجع 0;
        }
        إذا (سٽ[i] != ' ' && سٽ[i] != '\t' && سٽ[i] != '\f') {
            *جاچ_ايندڙ = 0;
            أرجع 0;
        }
    }
    ثابت Py_ssize_t marker_len = حجم("coding") - 1;
    لكل (; i < len - marker_len; i++) {
        إذا (memcmp(سٽ + i, "coding", marker_len) != 0) {
            استمر;
        }
        ثابت حرف *cursor = سٽ + i + marker_len;
        إذا (*cursor != ':' && *cursor != '=') {
            استمر;
        }
        افعل {
            cursor++;
        } طالما (cursor < سٽ + len &&
                 (*cursor == ' ' || *cursor == '\t'));
        ثابت حرف *شروعات = cursor;
        طالما (cursor < سٽ + len &&
                (Py_ISALNUM(*cursor) || *cursor == '-' ||
                 *cursor == '_' || *cursor == '.')) {
            cursor++;
        }
        إذا (cursor == شروعات) {
            استمر;
        }
        Py_ssize_t encoding_len = cursor - شروعات;
        // Include the longest recognized prefix and its NUL terminator.
        حرف normalized[حجم("iso-latin-1-")];
        ثابت Py_ssize_t اڳياڙي_len = Py_MIN(
            encoding_len, (Py_ssize_t)حجم(normalized) - 1);
        Py_ssize_t n;
        لكل (n = 0; n < اڳياڙي_len; n++) {
            normalized[n] = شروعات[n] == '_' ? '-' : Py_TOLOWER(شروعات[n]);
        }
        normalized[n] = '\0';
        ثابت حرف *canonical = شروعات;
        إذا (strcmp(normalized, "utf-8") == 0 ||
                لڙي_ڀيٽ(normalized, "utf-8-", حجم("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        وإلا إذا (strcmp(normalized, "latin-1") == 0 ||
                 strcmp(normalized, "iso-8859-1") == 0 ||
                 strcmp(normalized, "iso-latin-1") == 0 ||
                 لڙي_ڀيٽ(normalized, "latin-1-", حجم("latin-1-") - 1) == 0 ||
                 لڙي_ڀيٽ(normalized, "iso-8859-1-", حجم("iso-8859-1-") - 1) == 0 ||
                 لڙي_ڀيٽ(normalized, "iso-latin-1-", حجم("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        إذا (canonical != شروعات) {
            encoding_len = لڙي_ڊيگهه(canonical);
        }
        *encoding = Py_Tok_Copy_بائيٽون(canonical, encoding_len);
        إذا (*encoding == عدم) {
            أرجع -1;
        }
        *جاچ_ايندڙ = 0;
        أرجع 0;
    }
    أرجع 0;
}

Py_Tok_Encoding_نتيجو
_PyTok_DetectEncoding(بنية tok_state *tok, ثابت _PyTok_Chunk *پهريون,
                      ثابت _PyTok_Chunk *ٻيو, صحيح final,
                      Py_ssize_t *bom_len)
{
    صحيح bom = پهريون->len >= UTF8_BOM_ڊيگهائي &&
        memcmp(پهريون->معلومات, UTF8_BOM, UTF8_BOM_ڊيگهائي) == 0;
    *bom_len = bom ? UTF8_BOM_ڊيگهائي : 0;

    حرف *cookie = عدم;
    صحيح جاچ_ايندڙ = 0;
    صحيح cookie_سٽ = 1;
    ثابت حرف *پهريون_معلومات = پهريون->معلومات + *bom_len;
    Py_ssize_t پهريون_len = پهريون->len - *bom_len;
    إذا (ڳوليو_cookie(پهريون_معلومات, پهريون_len, &cookie, &جاچ_ايندڙ) < 0) {
        أرجع _PYTOK_ENCODING_ERROR;
    }
    إذا (cookie == عدم && جاچ_ايندڙ && ٻيو != عدم) {
        إذا (ڳوليو_cookie(ٻيو->معلومات, ٻيو->len, &cookie, &جاچ_ايندڙ) < 0) {
            أرجع _PYTOK_ENCODING_ERROR;
        }
        cookie_سٽ = 2;
    }
    وإلا إذا (cookie == عدم && جاچ_ايندڙ && !final) {
        أرجع PYTOK_ENCODING_NEED_ٻيو_سٽ;
    }

    إذا (cookie == عدم) {
        إذا (bom && Py_Tok_سيٽ_Encoding(tok, "utf-8") < 0) {
            أرجع _PYTOK_ENCODING_ERROR;
        }
        أرجع PYTOK_ENCODING_پورو;
    }
    إذا (bom && strcmp(cookie, "utf-8") != 0) {
        ثابت _PyTok_Chunk *سٽ = cookie_سٽ == 2 ? ٻيو : پهريون;
        ثابت حرف *سٽ_معلومات = سٽ->معلومات + (cookie_سٽ == 1 ? *bom_len : 0);
        Py_ssize_t سٽ_len = سٽ->len - (cookie_سٽ == 1 ? *bom_len : 0);
        صحيح پڇاڙي_کانو = (صحيح)Py_MIN(سٽ_len, وڌ_کان_وڌ_عدد);
        إذا (پڇاڙي_کانو > 0 && (سٽ_معلومات[پڇاڙي_کانو - 1] == '\n' ||
                            سٽ_معلومات[پڇاڙي_کانو - 1] == '\r')) {
            پڇاڙي_کانو--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, سٽ_معلومات, 0, cookie_سٽ, 0, پڇاڙي_کانو,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        أرجع _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    أرجع PYTOK_ENCODING_پورو;
}

صحيح
_PyTok_DecodeOnce(بنية tok_state *tok, _PyTok_Chunk *chunk,
                  ثابت حرف *encoding, ثابت حرف *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->معلومات, chunk->len, encoding, errors);
    إذا (unicode == عدم) {
        tok->پورو = PyErr_ExceptionMatches(Py_Exc_يادگيري_Error)
            ? E_NOMEM : E_DECODE;
        أرجع -1;
    }
    أرجع chunk_سيٽ_unicode(tok, chunk, unicode, 0);
}

ساكن Py_ssize_t
raw_سٽ_ڊيگهائي(ثابت حرف *معلومات, Py_ssize_t len)
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
store_prepared_ذريعو(بنية tok_state *tok, ثابت حرف *معلومات, Py_ssize_t len,
                      صحيح preserve_crlf, صحيح جوڙيو_final_newline)
{
    Py_ssize_t pos = 0;
    طالما (pos < len) {
        Py_ssize_t raw_سٽ_len;
        إذا (preserve_crlf) {
            ثابت حرف *newline = memchr(معلومات + pos, '\n', len - pos);
            raw_سٽ_len = newline == عدم
                ? len - pos : newline - معلومات - pos + 1;
        }
        وإلا {
            raw_سٽ_len = raw_سٽ_ڊيگهائي(معلومات + pos, len - pos);
        }
        صحيح terminated = preserve_crlf
            ? معلومات[pos + raw_سٽ_len - 1] == '\n'
            : معلومات[pos + raw_سٽ_len - 1] == '\n' ||
              معلومات[pos + raw_سٽ_len - 1] == '\r';
        صحيح جوڙيو_newline = جوڙيو_final_newline &&
            pos + raw_سٽ_len == len && !terminated;
        ثابت حرف *سٽ = معلومات + pos;
        // raw_line_length stops at the first CR or LF, so any CR is in
        // the line terminator.
        Py_ssize_t terminator_len = 0;
        إذا (!preserve_crlf) {
            إذا (سٽ[raw_سٽ_len - 1] == '\r') {
                terminator_len = 1;
            }
            وإلا إذا (raw_سٽ_len > 1 && سٽ[raw_سٽ_len - 2] == '\r') {
                terminator_len = 2;
            }
        }
        _PyTok_Off appended;
        إذا (terminator_len || جوڙيو_newline) {
            appended = Py_Tok_ذريعو_Append_سٽ_With_Newline(
                &tok->ذريعو, سٽ, raw_سٽ_len - terminator_len);
        }
        وإلا {
            appended = Py_Tok_ذريعو_Append_سٽ(&tok->ذريعو, سٽ, raw_سٽ_len);
        }
        إذا (appended < 0) {
            tok->پورو = PyErr_ExceptionMatches(Py_Exc_يادگيري_Error)
                ? E_NOMEM : E_ERROR;
            أرجع -1;
        }
        tok->reader->prepared_final_newline = جوڙيو_newline;
        pos += raw_سٽ_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    إذا (جوڙيو_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || معلومات[len - 1] != '\r'))
    {
        إذا (Py_Tok_ذريعو_Append_سٽ(&tok->ذريعو, "\n", 1) < 0) {
            أرجع -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    أرجع 0;
}

صحيح
_PyTok_PrepareString(بنية tok_state *tok, ثابت حرف *داخلا_2, صحيح utf8_only,
                     صحيح exec_داخلا, صحيح preserve_crlf)
{
    Py_ssize_t raw_len = لڙي_ڊيگهه(داخلا_2);
    حرف *raw = (حرف *)داخلا_2;

    إذا (utf8_only) {
        إذا (Py_Tok_سيٽ_Encoding(tok, "utf-8") < 0) {
            أرجع -1;
        }
    }
    وإلا {
        Py_ssize_t پهريون_original_len = raw_سٽ_ڊيگهائي(raw, raw_len);
        _PyTok_Chunk پهريون = {
            .معلومات = raw,
            .len = پهريون_original_len,
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        _PyTok_Chunk ٻيو = {0};
        صحيح have_ٻيو = پهريون_original_len < raw_len;
        إذا (have_ٻيو) {
            ٻيو.معلومات = raw + پهريون_original_len;
            ٻيو.len = raw_سٽ_ڊيگهائي(ٻيو.معلومات,
                                         raw_len - پهريون_original_len);
        }
        Py_ssize_t bom_len;
        Py_Tok_Encoding_نتيجو detection = _PyTok_DetectEncoding(
            tok, &پهريون, have_ٻيو ? &ٻيو : عدم, 1, &bom_len);
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

    صحيح stored = store_prepared_ذريعو(
        tok, decoded.معلومات, decoded.len, preserve_crlf, exec_داخلا);
    _PyTok_ChunkClear(&decoded);
    إذا (stored < 0) {
        أرجع -1;
    }
    إذا (!utf8_only &&
            (encoding == عدم || strcmp(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(Py_Tok_ذريعو_معلومات(&tok->ذريعو), tok, 1)) {
        أرجع -1;
    }
    أرجع 0;
}

صحيح
Py_Tok_شروعات_Decoder(بنية tok_state *tok, ثابت حرف *errors)
{
    _PyTok_Reader *reader = tok->reader;
    إذا (reader->encoding == عدم || reader->decoder != عدم) {
        أرجع 0;
    }
    إذا (reader->قسم == _PYTOK_READER_FILE &&
            strcmp(reader->encoding, "utf-8") == 0) {
        أرجع 0;
    }

    PyObject *codec = Py_Codec_Lookup_متن_Encoding(reader->encoding, عدم);
    إذا (codec != عدم) {
        reader->decoder = _PyCodecInfo_GetIncrementalDecoder(codec, errors);
        Py_DECREF(codec);
    }
    إذا (reader->decoder == عدم) {
        tok->پورو = PyErr_ExceptionMatches(Py_Exc_يادگيري_Error)
            ? E_NOMEM : E_DECODE;
        إذا (reader->قسم == _PYTOK_READER_FILE) {
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
    صحيح strip_bom = reader->قسم == _PYTOK_READER_READLINE &&
        chunk->len >= UTF16_BOM_ڊيگهائي &&
        (memcmp(chunk->معلومات, UTF16_LE_BOM, UTF16_BOM_ڊيگهائي) == 0 ||
         memcmp(chunk->معلومات, UTF16_BE_BOM, UTF16_BOM_ڊيگهائي) == 0);
    PyObject *داخلا_2;
    إذا (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            Py_بائيٽون_Check(chunk->owner) &&
            chunk->معلومات == Py_بائيٽون_AS_STRING(chunk->owner)) {
        داخلا_2 = Py_NewRef(chunk->owner);
    }
    وإلا {
        داخلا_2 = Py_بائيٽون_From_String_And_سائيز(chunk->معلومات, chunk->len);
    }
    إذا (داخلا_2 == عدم) {
        tok->پورو = E_NOMEM;
        أرجع -1;
    }
    PyObject *unicode = Py_Object_Call_طريقو_Obj_دليلون(
        reader->decoder, &_Py_ID(decode), داخلا_2,
        final ? Py_True : Py_False, عدم);
    Py_DECREF(داخلا_2);
    إذا (unicode == عدم) {
        tok->پورو = PyErr_ExceptionMatches(Py_Exc_يادگيري_Error)
            ? E_NOMEM : E_DECODE;
        إذا (reader->قسم == _PYTOK_READER_FILE) {
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
        tok->پورو = E_DECODE;
        أرجع -1;
    }
    أرجع chunk_سيٽ_unicode(tok, chunk, unicode, strip_bom);
}

صحيح
Py_Tok_Decoder_Has_Buffered_داخلا(بنية tok_state *tok)
{
    إذا (tok->reader->decoder == عدم) {
        أرجع 0;
    }
    PyObject *state = Py_Object_Call_طريقو_No_دليلون(
        tok->reader->decoder, &_Py_ID(getstate));
    إذا (state == عدم) {
        tok->پورو = PyErr_ExceptionMatches(Py_Exc_يادگيري_Error)
            ? E_NOMEM : E_DECODE;
        أرجع -1;
    }
    إذا (!PyTuple_Check(state) || Py_Tuple_GET_سائيز(state) != 2 ||
            !Py_بائيٽون_Check(Py_Tuple_GET_شي(state, 0)) ||
            !PyLong_Check(Py_Tuple_GET_شي(state, 1))) {
        Py_DECREF(state);
        Py_Err_سيٽ_String(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->پورو = E_DECODE;
        أرجع -1;
    }
    صحيح pending = Py_بائيٽون_GET_سائيز(Py_Tuple_GET_شي(state, 0)) != 0;
    Py_DECREF(state);
    أرجع pending;
}
