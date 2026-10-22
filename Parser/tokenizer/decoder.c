#شامل "Python.h"
#شامل "pycore_codecs.h"
#شامل "pycore_global_strings.h"
#شامل "pycore_runtime.h"
#شامل "errcode.h"

#شامل "reader_internal.h"
#شامل "helpers.h"
#شامل "../lexer/state.h"

#define UNICODE_BOM 0xFEFF
#define UTF16_LE_BOM "\xff\xfe"
#define UTF16_BE_BOM "\xfe\xff"
#define UTF16_BOM_LENGTH ((Py_ssize_t)(اندازه(UTF16_LE_BOM) - 1))
#define UTF8_BOM "\xef\xbb\xbf"
#define UTF8_BOM_LENGTH ((Py_ssize_t)(اندازه(UTF8_BOM) - 1))

نویسه *
_PyTok_CopyBytes(ثابت نویسه *data, Py_ssize_t len)
{
    اگر (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        بازگشت پوچ;
    }
    نویسه *copy = PyMem_Malloc((size_t)len + 1);
    اگر (copy == پوچ) {
        PyErr_NoMemory();
        بازگشت پوچ;
    }
    memcpy(copy, data, (size_t)len);
    copy[len] = '\0';
    بازگشت copy;
}

ایستا تهی
chunk_release_data(_PyTok_Chunk *chunk)
{
    انتخاب (chunk->ownership) {
        حالت _PYTOK_CHUNK_BORROWED:
            بشکن;
        حالت _PYTOK_CHUNK_PYMEM:
            PyMem_Free(chunk->data);
            بشکن;
        حالت _PYTOK_CHUNK_PYOBJECT:
            Py_DECREF(chunk->owner);
            بشکن;
    }
}

تهی
_PyTok_ChunkClear(_PyTok_Chunk *chunk)
{
    chunk_release_data(chunk);
    *chunk = (_PyTok_Chunk){0};
}

ایستا صحیح
chunk_set_unicode(ساختار tok_state *tok, _PyTok_Chunk *chunk,
                  PyObject *unicode, صحیح strip_bom)
{
    Py_ssize_t utf8_len;
    ثابت نویسه *utf8 = PyUnicode_AsUTF8AndSize(unicode, &utf8_len);
    اگر (utf8 == پوچ) {
        Py_DECREF(unicode);
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        بازگشت -1;
    }
    اگر (strip_bom && PyUnicode_GET_LENGTH(unicode) > 0 &&
            PyUnicode_ReadChar(unicode, 0) == UNICODE_BOM) {
        utf8 += UTF8_BOM_LENGTH;
        utf8_len -= UTF8_BOM_LENGTH;
    }
    chunk_release_data(chunk);
    chunk->owner = unicode;
    chunk->data = (نویسه *)utf8;
    chunk->len = utf8_len;
    chunk->ownership = _PYTOK_CHUNK_PYOBJECT;
    بازگشت 0;
}

نویسه *
_PyTok_NormalizeNewlines(ثابت نویسه *data, Py_ssize_t len,
                         Py_ssize_t *out_len)
{
    اگر (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        بازگشت پوچ;
    }
    نویسه *result = PyMem_Malloc((size_t)len + 1);
    اگر (result == پوچ) {
        PyErr_NoMemory();
        بازگشت پوچ;
    }
    Py_ssize_t اكتب = 0;
    برای (Py_ssize_t اقرأ = 0; اقرأ < len; اقرأ++) {
        نویسه c = data[اقرأ];
        اگر (c == '\r') {
            اگر (اقرأ + 1 < len && data[اقرأ + 1] == '\n') {
                اقرأ++;
            }
            c = '\n';
        }
        result[اكتب++] = c;
    }
    result[اكتب] = '\0';
    *out_len = اكتب;
    بازگشت result;
}

صحیح
_PyTok_SetEncoding(ساختار tok_state *tok, ثابت نویسه *encoding)
{
    نویسه *copy = _PyTok_CopyBytes(encoding, strlen(encoding));
    اگر (copy == پوچ) {
        tok->done = E_NOMEM;
        بازگشت -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    بازگشت 0;
}

ایستا صحیح
find_cookie(ثابت نویسه *سطر, Py_ssize_t len, نویسه **encoding, صحیح *scan_next)
{
    Py_ssize_t i = 0;
    *encoding = پوچ;
    *scan_next = 1;
    برای (; i < len; i++) {
        اگر (سطر[i] == '#') {
            بشکن;
        }
        اگر (سطر[i] == '\n' || سطر[i] == '\r') {
            بازگشت 0;
        }
        اگر (سطر[i] != ' ' && سطر[i] != '\t' && سطر[i] != '\f') {
            *scan_next = 0;
            بازگشت 0;
        }
    }
    ثابت Py_ssize_t marker_len = اندازه("coding") - 1;
    برای (; i < len - marker_len; i++) {
        اگر (memcmp(سطر + i, "coding", marker_len) != 0) {
            ادامه;
        }
        ثابت نویسه *cursor = سطر + i + marker_len;
        اگر (*cursor != ':' && *cursor != '=') {
            ادامه;
        }
        انجامبده {
            cursor++;
        } وقتی (cursor < سطر + len &&
                 (*cursor == ' ' || *cursor == '\t'));
        ثابت نویسه *start = cursor;
        وقتی (cursor < سطر + len &&
                (Py_ISALNUM(*cursor) || *cursor == '-' ||
                 *cursor == '_' || *cursor == '.')) {
            cursor++;
        }
        اگر (cursor == start) {
            ادامه;
        }
        Py_ssize_t encoding_len = cursor - start;
        // Include the longest recognized prefix and its NUL terminator.
        نویسه normalized[اندازه("iso-latin-1-")];
        ثابت Py_ssize_t prefix_len = Py_MIN(
            encoding_len, (Py_ssize_t)اندازه(normalized) - 1);
        Py_ssize_t n;
        برای (n = 0; n < prefix_len; n++) {
            normalized[n] = start[n] == '_' ? '-' : Py_TOLOWER(start[n]);
        }
        normalized[n] = '\0';
        ثابت نویسه *canonical = start;
        اگر (strcmp(normalized, "utf-8") == 0 ||
                strncmp(normalized, "utf-8-", اندازه("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        وگرنه اگر (strcmp(normalized, "latin-1") == 0 ||
                 strcmp(normalized, "iso-8859-1") == 0 ||
                 strcmp(normalized, "iso-latin-1") == 0 ||
                 strncmp(normalized, "latin-1-", اندازه("latin-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-8859-1-", اندازه("iso-8859-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-latin-1-", اندازه("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        اگر (canonical != start) {
            encoding_len = strlen(canonical);
        }
        *encoding = _PyTok_CopyBytes(canonical, encoding_len);
        اگر (*encoding == پوچ) {
            بازگشت -1;
        }
        *scan_next = 0;
        بازگشت 0;
    }
    بازگشت 0;
}

_PyTok_EncodingResult
_PyTok_DetectEncoding(ساختار tok_state *tok, ثابت _PyTok_Chunk *first,
                      ثابت _PyTok_Chunk *second, صحیح final,
                      Py_ssize_t *bom_len)
{
    صحیح bom = first->len >= UTF8_BOM_LENGTH &&
        memcmp(first->data, UTF8_BOM, UTF8_BOM_LENGTH) == 0;
    *bom_len = bom ? UTF8_BOM_LENGTH : 0;

    نویسه *cookie = پوچ;
    صحیح scan_next = 0;
    صحیح cookie_سطر = 1;
    ثابت نویسه *first_data = first->data + *bom_len;
    Py_ssize_t first_len = first->len - *bom_len;
    اگر (find_cookie(first_data, first_len, &cookie, &scan_next) < 0) {
        بازگشت _PYTOK_ENCODING_ERROR;
    }
    اگر (cookie == پوچ && scan_next && second != پوچ) {
        اگر (find_cookie(second->data, second->len, &cookie, &scan_next) < 0) {
            بازگشت _PYTOK_ENCODING_ERROR;
        }
        cookie_سطر = 2;
    }
    وگرنه اگر (cookie == پوچ && scan_next && !final) {
        بازگشت PYTOK_ENCODING_NEED_SECOND_سطر;
    }

    اگر (cookie == پوچ) {
        اگر (bom && _PyTok_SetEncoding(tok, "utf-8") < 0) {
            بازگشت _PYTOK_ENCODING_ERROR;
        }
        بازگشت _PYTOK_ENCODING_DONE;
    }
    اگر (bom && strcmp(cookie, "utf-8") != 0) {
        ثابت _PyTok_Chunk *سطر = cookie_سطر == 2 ? second : first;
        ثابت نویسه *سطر_data = سطر->data + (cookie_سطر == 1 ? *bom_len : 0);
        Py_ssize_t سطر_len = سطر->len - (cookie_سطر == 1 ? *bom_len : 0);
        صحیح end_col = (صحیح)Py_MIN(سطر_len, INT_MAX);
        اگر (end_col > 0 && (سطر_data[end_col - 1] == '\n' ||
                            سطر_data[end_col - 1] == '\r')) {
            end_col--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, سطر_data, 0, cookie_سطر, 0, end_col,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        بازگشت _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    بازگشت _PYTOK_ENCODING_DONE;
}

صحیح
_PyTok_DecodeOnce(ساختار tok_state *tok, _PyTok_Chunk *chunk,
                  ثابت نویسه *encoding, ثابت نویسه *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->data, chunk->len, encoding, errors);
    اگر (unicode == پوچ) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        بازگشت -1;
    }
    بازگشت chunk_set_unicode(tok, chunk, unicode, 0);
}

ایستا Py_ssize_t
raw_سطر_length(ثابت نویسه *data, Py_ssize_t len)
{
    برای (Py_ssize_t i = 0; i < len; i++) {
        اگر (data[i] == '\n') {
            بازگشت i + 1;
        }
        اگر (data[i] == '\r') {
            بازگشت i + 1 < len && data[i + 1] == '\n' ? i + 2 : i + 1;
        }
    }
    بازگشت len;
}

ایستا صحیح
store_prepared_source(ساختار tok_state *tok, ثابت نویسه *data, Py_ssize_t len,
                      صحیح preserve_crlf, صحیح add_final_newline)
{
    Py_ssize_t pos = 0;
    وقتی (pos < len) {
        Py_ssize_t raw_سطر_len;
        اگر (preserve_crlf) {
            ثابت نویسه *newline = memchr(data + pos, '\n', len - pos);
            raw_سطر_len = newline == پوچ
                ? len - pos : newline - data - pos + 1;
        }
        وگرنه {
            raw_سطر_len = raw_سطر_length(data + pos, len - pos);
        }
        صحیح terminated = preserve_crlf
            ? data[pos + raw_سطر_len - 1] == '\n'
            : data[pos + raw_سطر_len - 1] == '\n' ||
              data[pos + raw_سطر_len - 1] == '\r';
        صحیح add_newline = add_final_newline &&
            pos + raw_سطر_len == len && !terminated;
        ثابت نویسه *سطر = data + pos;
        // raw_line_length stops at the first CR or LF, so any CR is in
        // the line terminator.
        Py_ssize_t terminator_len = 0;
        اگر (!preserve_crlf) {
            اگر (سطر[raw_سطر_len - 1] == '\r') {
                terminator_len = 1;
            }
            وگرنه اگر (raw_سطر_len > 1 && سطر[raw_سطر_len - 2] == '\r') {
                terminator_len = 2;
            }
        }
        _PyTok_Off appended;
        اگر (terminator_len || add_newline) {
            appended = Py_Tok_Source_Append_سطر_With_Newline(
                &tok->source, سطر, raw_سطر_len - terminator_len);
        }
        وگرنه {
            appended = Py_Tok_Source_Append_سطر(&tok->source, سطر, raw_سطر_len);
        }
        اگر (appended < 0) {
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            بازگشت -1;
        }
        tok->reader->prepared_final_newline = add_newline;
        pos += raw_سطر_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    اگر (add_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || data[len - 1] != '\r'))
    {
        اگر (Py_Tok_Source_Append_سطر(&tok->source, "\n", 1) < 0) {
            بازگشت -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    بازگشت 0;
}

صحیح
_PyTok_PrepareString(ساختار tok_state *tok, ثابت نویسه *درون_داد, صحیح utf8_only,
                     صحیح exec_درون_داد, صحیح preserve_crlf)
{
    Py_ssize_t raw_len = strlen(درون_داد);
    نویسه *raw = (نویسه *)درون_داد;

    اگر (utf8_only) {
        اگر (_PyTok_SetEncoding(tok, "utf-8") < 0) {
            بازگشت -1;
        }
    }
    وگرنه {
        Py_ssize_t first_original_len = raw_سطر_length(raw, raw_len);
        _PyTok_Chunk first = {
            .data = raw,
            .len = first_original_len,
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        _PyTok_Chunk second = {0};
        صحیح have_second = first_original_len < raw_len;
        اگر (have_second) {
            second.data = raw + first_original_len;
            second.len = raw_سطر_length(second.data,
                                         raw_len - first_original_len);
        }
        Py_ssize_t bom_len;
        _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
            tok, &first, have_second ? &second : پوچ, 1, &bom_len);
        اگر (detection == _PYTOK_ENCODING_ERROR) {
            بازگشت -1;
        }
        raw += bom_len;
        raw_len -= bom_len;
    }

    _PyTok_Chunk decoded = {
        .data = raw,
        .len = raw_len,
        .ownership = _PYTOK_CHUNK_BORROWED,
    };
    ثابت نویسه *encoding = tok->reader->encoding;
    اگر (encoding != پوچ && strcmp(encoding, "utf-8") != 0) {
        اگر (_PyTok_DecodeOnce(
                tok, &decoded, encoding, پوچ) < 0) {
            بازگشت -1;
        }
    }

    صحیح stored = store_prepared_source(
        tok, decoded.data, decoded.len, preserve_crlf, exec_درون_داد);
    _PyTok_ChunkClear(&decoded);
    اگر (stored < 0) {
        بازگشت -1;
    }
    اگر (!utf8_only &&
            (encoding == پوچ || strcmp(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(_PyTok_SourceData(&tok->source), tok, 1)) {
        بازگشت -1;
    }
    بازگشت 0;
}

صحیح
_PyTok_StartDecoder(ساختار tok_state *tok, ثابت نویسه *errors)
{
    _PyTok_Reader *reader = tok->reader;
    اگر (reader->encoding == پوچ || reader->decoder != پوچ) {
        بازگشت 0;
    }
    اگر (reader->kind == _PYTOK_READER_FILE &&
            strcmp(reader->encoding, "utf-8") == 0) {
        بازگشت 0;
    }

    PyObject *codec = _PyCodec_LookupTextEncoding(reader->encoding, پوچ);
    اگر (codec != پوچ) {
        reader->decoder = _PyCodecInfo_GetIncrementalDecoder(codec, errors);
        Py_DECREF(codec);
    }
    اگر (reader->decoder == پوچ) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        اگر (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != پوچ ? tok->filename : Py_None);
        }
        بازگشت -1;
    }
    بازگشت 0;
}

صحیح
_PyTok_DecodeChunk(ساختار tok_state *tok, _PyTok_Chunk *chunk, صحیح final)
{
    _PyTok_Reader *reader = tok->reader;
    اگر (reader->decoder == پوچ) {
        بازگشت 0;
    }
    صحیح strip_bom = reader->kind == _PYTOK_READER_READLINE &&
        chunk->len >= UTF16_BOM_LENGTH &&
        (memcmp(chunk->data, UTF16_LE_BOM, UTF16_BOM_LENGTH) == 0 ||
         memcmp(chunk->data, UTF16_BE_BOM, UTF16_BOM_LENGTH) == 0);
    PyObject *درون_داد;
    اگر (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            PyBytes_Check(chunk->owner) &&
            chunk->data == PyBytes_AS_STRING(chunk->owner)) {
        درون_داد = Py_NewRef(chunk->owner);
    }
    وگرنه {
        درون_داد = PyBytes_FromStringAndSize(chunk->data, chunk->len);
    }
    اگر (درون_داد == پوچ) {
        tok->done = E_NOMEM;
        بازگشت -1;
    }
    PyObject *unicode = PyObject_CallMethodObjArgs(
        reader->decoder, &_Py_ID(decode), درون_داد,
        final ? Py_True : Py_False, پوچ);
    Py_DECREF(درون_داد);
    اگر (unicode == پوچ) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        اگر (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != پوچ ? tok->filename : Py_None);
        }
        بازگشت -1;
    }
    اگر (!PyUnicode_Check(unicode)) {
        PyErr_Format(PyExc_TypeError,
                     "decoder should return a string result, not '%.200s'",
                     Py_TYPE(unicode)->tp_name);
        Py_DECREF(unicode);
        tok->done = E_DECODE;
        بازگشت -1;
    }
    بازگشت chunk_set_unicode(tok, chunk, unicode, strip_bom);
}

صحیح
Py_Tok_Decoder_Has_Buffered_درون_داد(ساختار tok_state *tok)
{
    اگر (tok->reader->decoder == پوچ) {
        بازگشت 0;
    }
    PyObject *state = PyObject_CallMethodNoArgs(
        tok->reader->decoder, &_Py_ID(getstate));
    اگر (state == پوچ) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        بازگشت -1;
    }
    اگر (!PyTuple_Check(state) || PyTuple_GET_SIZE(state) != 2 ||
            !PyBytes_Check(PyTuple_GET_ITEM(state, 0)) ||
            !PyLong_Check(PyTuple_GET_ITEM(state, 1))) {
        Py_DECREF(state);
        PyErr_SetString(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->done = E_DECODE;
        بازگشت -1;
    }
    صحیح pending = PyBytes_GET_SIZE(PyTuple_GET_ITEM(state, 0)) != 0;
    Py_DECREF(state);
    بازگشت pending;
}
