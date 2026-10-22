#ضمن "Python.h"
#ضمن "pycore_codecs.h"
#ضمن "pycore_global_strings.h"
#ضمن "pycore_runtime.h"
#ضمن "errcode.h"

#ضمن "reader_internal.h"
#ضمن "helpers.h"
#ضمن "../lexer/state.h"

#define UNICODE_BOM 0xFEFF
#define UTF16_LE_BOM "\xff\xfe"
#define UTF16_BE_BOM "\xfe\xff"
#define UTF16_BOM_LENGTH ((Py_ssize_t)(حجم(UTF16_LE_BOM) - 1))
#define UTF8_BOM "\xef\xbb\xbf"
#define UTF8_BOM_LENGTH ((Py_ssize_t)(حجم(UTF8_BOM) - 1))

حرف *
_PyTok_CopyBytes(ثابت حرف *data, Py_ssize_t len)
{
    إذا (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        إرجاع عدم;
    }
    حرف *copy = PyMem_Malloc((size_t)len + 1);
    إذا (copy == عدم) {
        PyErr_NoMemory();
        إرجاع عدم;
    }
    memcpy(copy, data, (size_t)len);
    copy[len] = '\0';
    إرجاع copy;
}

ساكن فراغ
chunk_release_data(_PyTok_Chunk *chunk)
{
    بدل (chunk->ownership) {
        حالة _PYTOK_CHUNK_BORROWED:
            اكسر;
        حالة _PYTOK_CHUNK_PYMEM:
            PyMem_Free(chunk->data);
            اكسر;
        حالة _PYTOK_CHUNK_PYOBJECT:
            Py_DECREF(chunk->owner);
            اكسر;
    }
}

فراغ
_PyTok_ChunkClear(_PyTok_Chunk *chunk)
{
    chunk_release_data(chunk);
    *chunk = (_PyTok_Chunk){0};
}

ساكن صحيح
chunk_set_unicode(بنية tok_state *tok, _PyTok_Chunk *chunk,
                  PyObject *unicode, صحيح strip_bom)
{
    Py_ssize_t utf8_len;
    ثابت حرف *utf8 = PyUnicode_AsUTF8AndSize(unicode, &utf8_len);
    إذا (utf8 == عدم) {
        Py_DECREF(unicode);
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        إرجاع -1;
    }
    إذا (strip_bom && PyUnicode_GET_LENGTH(unicode) > 0 &&
            PyUnicode_ReadChar(unicode, 0) == UNICODE_BOM) {
        utf8 += UTF8_BOM_LENGTH;
        utf8_len -= UTF8_BOM_LENGTH;
    }
    chunk_release_data(chunk);
    chunk->owner = unicode;
    chunk->data = (حرف *)utf8;
    chunk->len = utf8_len;
    chunk->ownership = _PYTOK_CHUNK_PYOBJECT;
    إرجاع 0;
}

حرف *
_PyTok_NormalizeNewlines(ثابت حرف *data, Py_ssize_t len,
                         Py_ssize_t *out_len)
{
    إذا (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        إرجاع عدم;
    }
    حرف *result = PyMem_Malloc((size_t)len + 1);
    إذا (result == عدم) {
        PyErr_NoMemory();
        إرجاع عدم;
    }
    Py_ssize_t اكتب = 0;
    لكل (Py_ssize_t اقرأ = 0; اقرأ < len; اقرأ++) {
        حرف c = data[اقرأ];
        إذا (c == '\r') {
            إذا (اقرأ + 1 < len && data[اقرأ + 1] == '\n') {
                اقرأ++;
            }
            c = '\n';
        }
        result[اكتب++] = c;
    }
    result[اكتب] = '\0';
    *out_len = اكتب;
    إرجاع result;
}

صحيح
_PyTok_SetEncoding(بنية tok_state *tok, ثابت حرف *encoding)
{
    حرف *copy = _PyTok_CopyBytes(encoding, strlen(encoding));
    إذا (copy == عدم) {
        tok->done = E_NOMEM;
        إرجاع -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    إرجاع 0;
}

ساكن صحيح
find_cookie(ثابت حرف *سطر, Py_ssize_t len, حرف **encoding, صحيح *scan_next)
{
    Py_ssize_t i = 0;
    *encoding = عدم;
    *scan_next = 1;
    لكل (; i < len; i++) {
        إذا (سطر[i] == '#') {
            اكسر;
        }
        إذا (سطر[i] == '\n' || سطر[i] == '\r') {
            إرجاع 0;
        }
        إذا (سطر[i] != ' ' && سطر[i] != '\t' && سطر[i] != '\f') {
            *scan_next = 0;
            إرجاع 0;
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
        } بينما (cursor < سطر + len &&
                 (*cursor == ' ' || *cursor == '\t'));
        ثابت حرف *start = cursor;
        بينما (cursor < سطر + len &&
                (Py_ISALNUM(*cursor) || *cursor == '-' ||
                 *cursor == '_' || *cursor == '.')) {
            cursor++;
        }
        إذا (cursor == start) {
            استمر;
        }
        Py_ssize_t encoding_len = cursor - start;
        // Include the longest recognized prefix and its NUL terminator.
        حرف normalized[حجم("iso-latin-1-")];
        ثابت Py_ssize_t prefix_len = Py_MIN(
            encoding_len, (Py_ssize_t)حجم(normalized) - 1);
        Py_ssize_t n;
        لكل (n = 0; n < prefix_len; n++) {
            normalized[n] = start[n] == '_' ? '-' : Py_TOLOWER(start[n]);
        }
        normalized[n] = '\0';
        ثابت حرف *canonical = start;
        إذا (strcmp(normalized, "utf-8") == 0 ||
                strncmp(normalized, "utf-8-", حجم("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        وإلا إذا (strcmp(normalized, "latin-1") == 0 ||
                 strcmp(normalized, "iso-8859-1") == 0 ||
                 strcmp(normalized, "iso-latin-1") == 0 ||
                 strncmp(normalized, "latin-1-", حجم("latin-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-8859-1-", حجم("iso-8859-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-latin-1-", حجم("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        إذا (canonical != start) {
            encoding_len = strlen(canonical);
        }
        *encoding = _PyTok_CopyBytes(canonical, encoding_len);
        إذا (*encoding == عدم) {
            إرجاع -1;
        }
        *scan_next = 0;
        إرجاع 0;
    }
    إرجاع 0;
}

_PyTok_EncodingResult
_PyTok_DetectEncoding(بنية tok_state *tok, ثابت _PyTok_Chunk *first,
                      ثابت _PyTok_Chunk *second, صحيح final,
                      Py_ssize_t *bom_len)
{
    صحيح bom = first->len >= UTF8_BOM_LENGTH &&
        memcmp(first->data, UTF8_BOM, UTF8_BOM_LENGTH) == 0;
    *bom_len = bom ? UTF8_BOM_LENGTH : 0;

    حرف *cookie = عدم;
    صحيح scan_next = 0;
    صحيح cookie_سطر = 1;
    ثابت حرف *first_data = first->data + *bom_len;
    Py_ssize_t first_len = first->len - *bom_len;
    إذا (find_cookie(first_data, first_len, &cookie, &scan_next) < 0) {
        إرجاع _PYTOK_ENCODING_ERROR;
    }
    إذا (cookie == عدم && scan_next && second != عدم) {
        إذا (find_cookie(second->data, second->len, &cookie, &scan_next) < 0) {
            إرجاع _PYTOK_ENCODING_ERROR;
        }
        cookie_سطر = 2;
    }
    وإلا إذا (cookie == عدم && scan_next && !final) {
        إرجاع PYTOK_ENCODING_NEED_SECOND_سطر;
    }

    إذا (cookie == عدم) {
        إذا (bom && _PyTok_SetEncoding(tok, "utf-8") < 0) {
            إرجاع _PYTOK_ENCODING_ERROR;
        }
        إرجاع _PYTOK_ENCODING_DONE;
    }
    إذا (bom && strcmp(cookie, "utf-8") != 0) {
        ثابت _PyTok_Chunk *سطر = cookie_سطر == 2 ? second : first;
        ثابت حرف *سطر_data = سطر->data + (cookie_سطر == 1 ? *bom_len : 0);
        Py_ssize_t سطر_len = سطر->len - (cookie_سطر == 1 ? *bom_len : 0);
        صحيح end_col = (صحيح)Py_MIN(سطر_len, INT_MAX);
        إذا (end_col > 0 && (سطر_data[end_col - 1] == '\n' ||
                            سطر_data[end_col - 1] == '\r')) {
            end_col--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, سطر_data, 0, cookie_سطر, 0, end_col,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        إرجاع _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    إرجاع _PYTOK_ENCODING_DONE;
}

صحيح
_PyTok_DecodeOnce(بنية tok_state *tok, _PyTok_Chunk *chunk,
                  ثابت حرف *encoding, ثابت حرف *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->data, chunk->len, encoding, errors);
    إذا (unicode == عدم) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        إرجاع -1;
    }
    إرجاع chunk_set_unicode(tok, chunk, unicode, 0);
}

ساكن Py_ssize_t
raw_سطر_length(ثابت حرف *data, Py_ssize_t len)
{
    لكل (Py_ssize_t i = 0; i < len; i++) {
        إذا (data[i] == '\n') {
            إرجاع i + 1;
        }
        إذا (data[i] == '\r') {
            إرجاع i + 1 < len && data[i + 1] == '\n' ? i + 2 : i + 1;
        }
    }
    إرجاع len;
}

ساكن صحيح
store_prepared_source(بنية tok_state *tok, ثابت حرف *data, Py_ssize_t len,
                      صحيح preserve_crlf, صحيح add_final_newline)
{
    Py_ssize_t pos = 0;
    بينما (pos < len) {
        Py_ssize_t raw_سطر_len;
        إذا (preserve_crlf) {
            ثابت حرف *newline = memchr(data + pos, '\n', len - pos);
            raw_سطر_len = newline == عدم
                ? len - pos : newline - data - pos + 1;
        }
        وإلا {
            raw_سطر_len = raw_سطر_length(data + pos, len - pos);
        }
        صحيح terminated = preserve_crlf
            ? data[pos + raw_سطر_len - 1] == '\n'
            : data[pos + raw_سطر_len - 1] == '\n' ||
              data[pos + raw_سطر_len - 1] == '\r';
        صحيح add_newline = add_final_newline &&
            pos + raw_سطر_len == len && !terminated;
        ثابت حرف *سطر = data + pos;
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
        إذا (terminator_len || add_newline) {
            appended = Py_Tok_Source_Append_سطر_With_Newline(
                &tok->source, سطر, raw_سطر_len - terminator_len);
        }
        وإلا {
            appended = Py_Tok_Source_Append_سطر(&tok->source, سطر, raw_سطر_len);
        }
        إذا (appended < 0) {
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            إرجاع -1;
        }
        tok->reader->prepared_final_newline = add_newline;
        pos += raw_سطر_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    إذا (add_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || data[len - 1] != '\r'))
    {
        إذا (Py_Tok_Source_Append_سطر(&tok->source, "\n", 1) < 0) {
            إرجاع -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    إرجاع 0;
}

صحيح
_PyTok_PrepareString(بنية tok_state *tok, ثابت حرف *إدخال, صحيح utf8_only,
                     صحيح exec_إدخال, صحيح preserve_crlf)
{
    Py_ssize_t raw_len = strlen(إدخال);
    حرف *raw = (حرف *)إدخال;

    إذا (utf8_only) {
        إذا (_PyTok_SetEncoding(tok, "utf-8") < 0) {
            إرجاع -1;
        }
    }
    وإلا {
        Py_ssize_t first_original_len = raw_سطر_length(raw, raw_len);
        _PyTok_Chunk first = {
            .data = raw,
            .len = first_original_len,
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        _PyTok_Chunk second = {0};
        صحيح have_second = first_original_len < raw_len;
        إذا (have_second) {
            second.data = raw + first_original_len;
            second.len = raw_سطر_length(second.data,
                                         raw_len - first_original_len);
        }
        Py_ssize_t bom_len;
        _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
            tok, &first, have_second ? &second : عدم, 1, &bom_len);
        إذا (detection == _PYTOK_ENCODING_ERROR) {
            إرجاع -1;
        }
        raw += bom_len;
        raw_len -= bom_len;
    }

    _PyTok_Chunk decoded = {
        .data = raw,
        .len = raw_len,
        .ownership = _PYTOK_CHUNK_BORROWED,
    };
    ثابت حرف *encoding = tok->reader->encoding;
    إذا (encoding != عدم && strcmp(encoding, "utf-8") != 0) {
        إذا (_PyTok_DecodeOnce(
                tok, &decoded, encoding, عدم) < 0) {
            إرجاع -1;
        }
    }

    صحيح stored = store_prepared_source(
        tok, decoded.data, decoded.len, preserve_crlf, exec_إدخال);
    _PyTok_ChunkClear(&decoded);
    إذا (stored < 0) {
        إرجاع -1;
    }
    إذا (!utf8_only &&
            (encoding == عدم || strcmp(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(_PyTok_SourceData(&tok->source), tok, 1)) {
        إرجاع -1;
    }
    إرجاع 0;
}

صحيح
_PyTok_StartDecoder(بنية tok_state *tok, ثابت حرف *errors)
{
    _PyTok_Reader *reader = tok->reader;
    إذا (reader->encoding == عدم || reader->decoder != عدم) {
        إرجاع 0;
    }
    إذا (reader->kind == _PYTOK_READER_FILE &&
            strcmp(reader->encoding, "utf-8") == 0) {
        إرجاع 0;
    }

    PyObject *codec = _PyCodec_LookupTextEncoding(reader->encoding, عدم);
    إذا (codec != عدم) {
        reader->decoder = _PyCodecInfo_GetIncrementalDecoder(codec, errors);
        Py_DECREF(codec);
    }
    إذا (reader->decoder == عدم) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        إذا (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != عدم ? tok->filename : Py_None);
        }
        إرجاع -1;
    }
    إرجاع 0;
}

صحيح
_PyTok_DecodeChunk(بنية tok_state *tok, _PyTok_Chunk *chunk, صحيح final)
{
    _PyTok_Reader *reader = tok->reader;
    إذا (reader->decoder == عدم) {
        إرجاع 0;
    }
    صحيح strip_bom = reader->kind == _PYTOK_READER_READLINE &&
        chunk->len >= UTF16_BOM_LENGTH &&
        (memcmp(chunk->data, UTF16_LE_BOM, UTF16_BOM_LENGTH) == 0 ||
         memcmp(chunk->data, UTF16_BE_BOM, UTF16_BOM_LENGTH) == 0);
    PyObject *إدخال;
    إذا (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            PyBytes_Check(chunk->owner) &&
            chunk->data == PyBytes_AS_STRING(chunk->owner)) {
        إدخال = Py_NewRef(chunk->owner);
    }
    وإلا {
        إدخال = PyBytes_FromStringAndSize(chunk->data, chunk->len);
    }
    إذا (إدخال == عدم) {
        tok->done = E_NOMEM;
        إرجاع -1;
    }
    PyObject *unicode = PyObject_CallMethodObjArgs(
        reader->decoder, &_Py_ID(decode), إدخال,
        final ? Py_True : Py_False, عدم);
    Py_DECREF(إدخال);
    إذا (unicode == عدم) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        إذا (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != عدم ? tok->filename : Py_None);
        }
        إرجاع -1;
    }
    إذا (!PyUnicode_Check(unicode)) {
        PyErr_Format(PyExc_TypeError,
                     "decoder should return a string result, not '%.200s'",
                     Py_TYPE(unicode)->tp_name);
        Py_DECREF(unicode);
        tok->done = E_DECODE;
        إرجاع -1;
    }
    إرجاع chunk_set_unicode(tok, chunk, unicode, strip_bom);
}

صحيح
Py_Tok_Decoder_Has_Buffered_إدخال(بنية tok_state *tok)
{
    إذا (tok->reader->decoder == عدم) {
        إرجاع 0;
    }
    PyObject *state = PyObject_CallMethodNoArgs(
        tok->reader->decoder, &_Py_ID(getstate));
    إذا (state == عدم) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        إرجاع -1;
    }
    إذا (!PyTuple_Check(state) || PyTuple_GET_SIZE(state) != 2 ||
            !PyBytes_Check(PyTuple_GET_ITEM(state, 0)) ||
            !PyLong_Check(PyTuple_GET_ITEM(state, 1))) {
        Py_DECREF(state);
        PyErr_SetString(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->done = E_DECODE;
        إرجاع -1;
    }
    صحيح pending = PyBytes_GET_SIZE(PyTuple_GET_ITEM(state, 0)) != 0;
    Py_DECREF(state);
    إرجاع pending;
}
