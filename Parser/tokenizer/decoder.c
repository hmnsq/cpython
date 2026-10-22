#ضمن "Python.h"
#ضمن "pycore_codecs.h"
#ضمن "pycore_global_strings.h"
#ضمن "pycore_runtime.h"
#ضمن "errcode.h"

#ضمن "reader_internal.h"
#ضمن "helpers.h"
#ضمن "../lexer/state.h"

#تعریف_کرو UNICODE_BOM 0xFEFF
#تعریف_کرو UTF16_LE_BOM "\xff\xfe"
#تعریف_کرو UTF16_BE_BOM "\xfe\xff"
#تعریف_کرو UTF16_BOM_لمائی ((Py_ssize_t)(حجم(UTF16_LE_BOM) - 1))
#تعریف_کرو UTF8_BOM "\xef\xbb\xbf"
#تعریف_کرو UTF8_BOM_لمائی ((Py_ssize_t)(حجم(UTF8_BOM) - 1))

حرف *
Py_Tok_Copy_بائٹاں(ثابت حرف *انکڑے, Py_ssize_t len)
{
    إذا (len < 0 || len == PY_SSIZE_T_MAX) {
        Py_Err_No_حافظہ();
        أرجع عدم;
    }
    حرف *copy = PyMem_Malloc((ناپ_t)len + 1);
    إذا (copy == عدم) {
        Py_Err_No_حافظہ();
        أرجع عدم;
    }
    حافظہ_نقل(copy, انکڑے, (ناپ_t)len);
    copy[len] = '\0';
    أرجع copy;
}

ساكن فراغ
chunk_release_انکڑے(_PyTok_Chunk *chunk)
{
    بدل (chunk->ownership) {
        حالة _PYTOK_CHUNK_BORROWED:
            اكسر;
        حالة _PYTOK_CHUNK_PYMEM:
            PyMem_Free(chunk->انکڑے);
            اكسر;
        حالة _PYTOK_CHUNK_PYOBJECT:
            Py_DECREF(chunk->owner);
            اكسر;
    }
}

فراغ
_PyTok_ChunkClear(_PyTok_Chunk *chunk)
{
    chunk_release_انکڑے(chunk);
    *chunk = (_PyTok_Chunk){0};
}

ساكن صحيح
chunk_سیٹ_unicode(بنية tok_state *tok, _PyTok_Chunk *chunk,
                  PyObject *unicode, صحيح strip_bom)
{
    Py_ssize_t utf8_len;
    ثابت حرف *utf8 = Py_Unicode_As_UTF8_And_ناپ(unicode, &utf8_len);
    إذا (utf8 == عدم) {
        Py_DECREF(unicode);
        tok->پورا = PyErr_ExceptionMatches(Py_Exc_حافظہ_Error)
            ? E_NOMEM : E_DECODE;
        أرجع -1;
    }
    إذا (strip_bom && Py_Unicode_GET_لمائی(unicode) > 0 &&
            PyUnicode_ReadChar(unicode, 0) == UNICODE_BOM) {
        utf8 += UTF8_BOM_لمائی;
        utf8_len -= UTF8_BOM_لمائی;
    }
    chunk_release_انکڑے(chunk);
    chunk->owner = unicode;
    chunk->انکڑے = (حرف *)utf8;
    chunk->len = utf8_len;
    chunk->ownership = _PYTOK_CHUNK_PYOBJECT;
    أرجع 0;
}

حرف *
_PyTok_NormalizeNewlines(ثابت حرف *انکڑے, Py_ssize_t len,
                         Py_ssize_t *باہرلا_len)
{
    إذا (len < 0 || len == PY_SSIZE_T_MAX) {
        Py_Err_No_حافظہ();
        أرجع عدم;
    }
    حرف *نتیجہ = PyMem_Malloc((ناپ_t)len + 1);
    إذا (نتیجہ == عدم) {
        Py_Err_No_حافظہ();
        أرجع عدم;
    }
    Py_ssize_t اكتب = 0;
    لكل (Py_ssize_t اقرأ = 0; اقرأ < len; اقرأ++) {
        حرف c = انکڑے[اقرأ];
        إذا (c == '\r') {
            إذا (اقرأ + 1 < len && انکڑے[اقرأ + 1] == '\n') {
                اقرأ++;
            }
            c = '\n';
        }
        نتیجہ[اكتب++] = c;
    }
    نتیجہ[اكتب] = '\0';
    *باہرلا_len = اكتب;
    أرجع نتیجہ;
}

صحيح
Py_Tok_سیٹ_Encoding(بنية tok_state *tok, ثابت حرف *encoding)
{
    حرف *copy = Py_Tok_Copy_بائٹاں(encoding, لڑی_لمبائی(encoding));
    إذا (copy == عدم) {
        tok->پورا = E_NOMEM;
        أرجع -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    أرجع 0;
}

ساكن صحيح
لبھو_cookie(ثابت حرف *سطر, Py_ssize_t len, حرف **encoding, صحيح *جانچ_اگلا)
{
    Py_ssize_t i = 0;
    *encoding = عدم;
    *جانچ_اگلا = 1;
    لكل (; i < len; i++) {
        إذا (سطر[i] == '#') {
            اكسر;
        }
        إذا (سطر[i] == '\n' || سطر[i] == '\r') {
            أرجع 0;
        }
        إذا (سطر[i] != ' ' && سطر[i] != '\t' && سطر[i] != '\f') {
            *جانچ_اگلا = 0;
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
        ثابت Py_ssize_t اگیتر_len = Py_MIN(
            encoding_len, (Py_ssize_t)حجم(normalized) - 1);
        Py_ssize_t n;
        لكل (n = 0; n < اگیتر_len; n++) {
            normalized[n] = شروع[n] == '_' ? '-' : Py_TOLOWER(شروع[n]);
        }
        normalized[n] = '\0';
        ثابت حرف *canonical = شروع;
        إذا (strcmp(normalized, "utf-8") == 0 ||
                لڑی_مقابلہ(normalized, "utf-8-", حجم("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        وإلا إذا (strcmp(normalized, "latin-1") == 0 ||
                 strcmp(normalized, "iso-8859-1") == 0 ||
                 strcmp(normalized, "iso-latin-1") == 0 ||
                 لڑی_مقابلہ(normalized, "latin-1-", حجم("latin-1-") - 1) == 0 ||
                 لڑی_مقابلہ(normalized, "iso-8859-1-", حجم("iso-8859-1-") - 1) == 0 ||
                 لڑی_مقابلہ(normalized, "iso-latin-1-", حجم("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        إذا (canonical != شروع) {
            encoding_len = لڑی_لمبائی(canonical);
        }
        *encoding = Py_Tok_Copy_بائٹاں(canonical, encoding_len);
        إذا (*encoding == عدم) {
            أرجع -1;
        }
        *جانچ_اگلا = 0;
        أرجع 0;
    }
    أرجع 0;
}

Py_Tok_Encoding_نتیجہ
_PyTok_DetectEncoding(بنية tok_state *tok, ثابت _PyTok_Chunk *پہلا,
                      ثابت _PyTok_Chunk *دوجا, صحيح final,
                      Py_ssize_t *bom_len)
{
    صحيح bom = پہلا->len >= UTF8_BOM_لمائی &&
        memcmp(پہلا->انکڑے, UTF8_BOM, UTF8_BOM_لمائی) == 0;
    *bom_len = bom ? UTF8_BOM_لمائی : 0;

    حرف *cookie = عدم;
    صحيح جانچ_اگلا = 0;
    صحيح cookie_سطر = 1;
    ثابت حرف *پہلا_انکڑے = پہلا->انکڑے + *bom_len;
    Py_ssize_t پہلا_len = پہلا->len - *bom_len;
    إذا (لبھو_cookie(پہلا_انکڑے, پہلا_len, &cookie, &جانچ_اگلا) < 0) {
        أرجع _PYTOK_ENCODING_ERROR;
    }
    إذا (cookie == عدم && جانچ_اگلا && دوجا != عدم) {
        إذا (لبھو_cookie(دوجا->انکڑے, دوجا->len, &cookie, &جانچ_اگلا) < 0) {
            أرجع _PYTOK_ENCODING_ERROR;
        }
        cookie_سطر = 2;
    }
    وإلا إذا (cookie == عدم && جانچ_اگلا && !final) {
        أرجع PYTOK_ENCODING_NEED_دوجا_سطر;
    }

    إذا (cookie == عدم) {
        إذا (bom && Py_Tok_سیٹ_Encoding(tok, "utf-8") < 0) {
            أرجع _PYTOK_ENCODING_ERROR;
        }
        أرجع PYTOK_ENCODING_پورا;
    }
    إذا (bom && strcmp(cookie, "utf-8") != 0) {
        ثابت _PyTok_Chunk *سطر = cookie_سطر == 2 ? دوجا : پہلا;
        ثابت حرف *سطر_انکڑے = سطر->انکڑے + (cookie_سطر == 1 ? *bom_len : 0);
        Py_ssize_t سطر_len = سطر->len - (cookie_سطر == 1 ? *bom_len : 0);
        صحيح سرا_خانہ = (صحيح)Py_MIN(سطر_len, ودھ_توں_ودھ_عدد);
        إذا (سرا_خانہ > 0 && (سطر_انکڑے[سرا_خانہ - 1] == '\n' ||
                            سطر_انکڑے[سرا_خانہ - 1] == '\r')) {
            سرا_خانہ--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, سطر_انکڑے, 0, cookie_سطر, 0, سرا_خانہ,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        أرجع _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    أرجع PYTOK_ENCODING_پورا;
}

صحيح
_PyTok_DecodeOnce(بنية tok_state *tok, _PyTok_Chunk *chunk,
                  ثابت حرف *encoding, ثابت حرف *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->انکڑے, chunk->len, encoding, errors);
    إذا (unicode == عدم) {
        tok->پورا = PyErr_ExceptionMatches(Py_Exc_حافظہ_Error)
            ? E_NOMEM : E_DECODE;
        أرجع -1;
    }
    أرجع chunk_سیٹ_unicode(tok, chunk, unicode, 0);
}

ساكن Py_ssize_t
raw_سطر_لمائی(ثابت حرف *انکڑے, Py_ssize_t len)
{
    لكل (Py_ssize_t i = 0; i < len; i++) {
        إذا (انکڑے[i] == '\n') {
            أرجع i + 1;
        }
        إذا (انکڑے[i] == '\r') {
            أرجع i + 1 < len && انکڑے[i + 1] == '\n' ? i + 2 : i + 1;
        }
    }
    أرجع len;
}

ساكن صحيح
store_prepared_سروت(بنية tok_state *tok, ثابت حرف *انکڑے, Py_ssize_t len,
                      صحيح preserve_crlf, صحيح جوڑو_final_newline)
{
    Py_ssize_t pos = 0;
    طالما (pos < len) {
        Py_ssize_t raw_سطر_len;
        إذا (preserve_crlf) {
            ثابت حرف *newline = memchr(انکڑے + pos, '\n', len - pos);
            raw_سطر_len = newline == عدم
                ? len - pos : newline - انکڑے - pos + 1;
        }
        وإلا {
            raw_سطر_len = raw_سطر_لمائی(انکڑے + pos, len - pos);
        }
        صحيح terminated = preserve_crlf
            ? انکڑے[pos + raw_سطر_len - 1] == '\n'
            : انکڑے[pos + raw_سطر_len - 1] == '\n' ||
              انکڑے[pos + raw_سطر_len - 1] == '\r';
        صحيح جوڑو_newline = جوڑو_final_newline &&
            pos + raw_سطر_len == len && !terminated;
        ثابت حرف *سطر = انکڑے + pos;
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
        إذا (terminator_len || جوڑو_newline) {
            appended = Py_Tok_سروت_Append_سطر_With_Newline(
                &tok->سروت, سطر, raw_سطر_len - terminator_len);
        }
        وإلا {
            appended = Py_Tok_سروت_Append_سطر(&tok->سروت, سطر, raw_سطر_len);
        }
        إذا (appended < 0) {
            tok->پورا = PyErr_ExceptionMatches(Py_Exc_حافظہ_Error)
                ? E_NOMEM : E_ERROR;
            أرجع -1;
        }
        tok->reader->prepared_final_newline = جوڑو_newline;
        pos += raw_سطر_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    إذا (جوڑو_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || انکڑے[len - 1] != '\r'))
    {
        إذا (Py_Tok_سروت_Append_سطر(&tok->سروت, "\n", 1) < 0) {
            أرجع -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    أرجع 0;
}

صحيح
_PyTok_PrepareString(بنية tok_state *tok, ثابت حرف *داخلہ, صحيح utf8_only,
                     صحيح exec_داخلہ, صحيح preserve_crlf)
{
    Py_ssize_t raw_len = لڑی_لمبائی(داخلہ);
    حرف *raw = (حرف *)داخلہ;

    إذا (utf8_only) {
        إذا (Py_Tok_سیٹ_Encoding(tok, "utf-8") < 0) {
            أرجع -1;
        }
    }
    وإلا {
        Py_ssize_t پہلا_original_len = raw_سطر_لمائی(raw, raw_len);
        _PyTok_Chunk پہلا = {
            .انکڑے = raw,
            .len = پہلا_original_len,
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        _PyTok_Chunk دوجا = {0};
        صحيح have_دوجا = پہلا_original_len < raw_len;
        إذا (have_دوجا) {
            دوجا.انکڑے = raw + پہلا_original_len;
            دوجا.len = raw_سطر_لمائی(دوجا.انکڑے,
                                         raw_len - پہلا_original_len);
        }
        Py_ssize_t bom_len;
        Py_Tok_Encoding_نتیجہ detection = _PyTok_DetectEncoding(
            tok, &پہلا, have_دوجا ? &دوجا : عدم, 1, &bom_len);
        إذا (detection == _PYTOK_ENCODING_ERROR) {
            أرجع -1;
        }
        raw += bom_len;
        raw_len -= bom_len;
    }

    _PyTok_Chunk decoded = {
        .انکڑے = raw,
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

    صحيح stored = store_prepared_سروت(
        tok, decoded.انکڑے, decoded.len, preserve_crlf, exec_داخلہ);
    _PyTok_ChunkClear(&decoded);
    إذا (stored < 0) {
        أرجع -1;
    }
    إذا (!utf8_only &&
            (encoding == عدم || strcmp(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(Py_Tok_سروت_انکڑے(&tok->سروت), tok, 1)) {
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
        tok->پورا = PyErr_ExceptionMatches(Py_Exc_حافظہ_Error)
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
        chunk->len >= UTF16_BOM_لمائی &&
        (memcmp(chunk->انکڑے, UTF16_LE_BOM, UTF16_BOM_لمائی) == 0 ||
         memcmp(chunk->انکڑے, UTF16_BE_BOM, UTF16_BOM_لمائی) == 0);
    PyObject *داخلہ;
    إذا (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            Py_بائٹاں_Check(chunk->owner) &&
            chunk->انکڑے == Py_بائٹاں_AS_STRING(chunk->owner)) {
        داخلہ = Py_NewRef(chunk->owner);
    }
    وإلا {
        داخلہ = Py_بائٹاں_From_String_And_ناپ(chunk->انکڑے, chunk->len);
    }
    إذا (داخلہ == عدم) {
        tok->پورا = E_NOMEM;
        أرجع -1;
    }
    PyObject *unicode = Py_Object_Call_طریقہ_Obj_دلیلاں(
        reader->decoder, &_Py_ID(decode), داخلہ,
        final ? Py_True : Py_False, عدم);
    Py_DECREF(داخلہ);
    إذا (unicode == عدم) {
        tok->پورا = PyErr_ExceptionMatches(Py_Exc_حافظہ_Error)
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
        tok->پورا = E_DECODE;
        أرجع -1;
    }
    أرجع chunk_سیٹ_unicode(tok, chunk, unicode, strip_bom);
}

صحيح
Py_Tok_Decoder_Has_Buffered_داخلہ(بنية tok_state *tok)
{
    إذا (tok->reader->decoder == عدم) {
        أرجع 0;
    }
    PyObject *state = Py_Object_Call_طریقہ_No_دلیلاں(
        tok->reader->decoder, &_Py_ID(getstate));
    إذا (state == عدم) {
        tok->پورا = PyErr_ExceptionMatches(Py_Exc_حافظہ_Error)
            ? E_NOMEM : E_DECODE;
        أرجع -1;
    }
    إذا (!PyTuple_Check(state) || Py_Tuple_GET_ناپ(state) != 2 ||
            !Py_بائٹاں_Check(Py_Tuple_GET_شے(state, 0)) ||
            !PyLong_Check(Py_Tuple_GET_شے(state, 1))) {
        Py_DECREF(state);
        Py_Err_سیٹ_String(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->پورا = E_DECODE;
        أرجع -1;
    }
    صحيح pending = Py_بائٹاں_GET_ناپ(Py_Tuple_GET_شے(state, 0)) != 0;
    Py_DECREF(state);
    أرجع pending;
}
