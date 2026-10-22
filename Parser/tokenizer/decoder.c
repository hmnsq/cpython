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
    𐡄𐡍 (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        أرجع عدم;
    }
    حرف *copy = PyMem_Malloc((size_t)len + 1);
    𐡄𐡍 (copy == عدم) {
        PyErr_NoMemory();
        أرجع عدم;
    }
    memcpy(copy, data, (size_t)len);
    copy[len] = '\0';
    أرجع copy;
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
    𐡄𐡍 (utf8 == عدم) {
        Py_DECREF(unicode);
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        أرجع -1;
    }
    𐡄𐡍 (strip_bom && PyUnicode_GET_LENGTH(unicode) > 0 &&
            PyUnicode_ReadChar(unicode, 0) == UNICODE_BOM) {
        utf8 += UTF8_BOM_LENGTH;
        utf8_len -= UTF8_BOM_LENGTH;
    }
    chunk_release_data(chunk);
    chunk->owner = unicode;
    chunk->data = (حرف *)utf8;
    chunk->len = utf8_len;
    chunk->ownership = _PYTOK_CHUNK_PYOBJECT;
    أرجع 0;
}

حرف *
_PyTok_NormalizeNewlines(ثابت حرف *data, Py_ssize_t len,
                         Py_ssize_t *out_len)
{
    𐡄𐡍 (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        أرجع عدم;
    }
    حرف *result = PyMem_Malloc((size_t)len + 1);
    𐡄𐡍 (result == عدم) {
        PyErr_NoMemory();
        أرجع عدم;
    }
    Py_ssize_t اكتب = 0;
    لكل (Py_ssize_t اقرأ = 0; اقرأ < len; اقرأ++) {
        حرف c = data[اقرأ];
        𐡄𐡍 (c == '\r') {
            𐡄𐡍 (اقرأ + 1 < len && data[اقرأ + 1] == '\n') {
                اقرأ++;
            }
            c = '\n';
        }
        result[اكتب++] = c;
    }
    result[اكتب] = '\0';
    *out_len = اكتب;
    أرجع result;
}

صحيح
_PyTok_SetEncoding(بنية tok_state *tok, ثابت حرف *encoding)
{
    حرف *copy = _PyTok_CopyBytes(encoding, strlen(encoding));
    𐡄𐡍 (copy == عدم) {
        tok->done = E_NOMEM;
        أرجع -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    أرجع 0;
}

ساكن صحيح
find_cookie(ثابت حرف *line, Py_ssize_t len, حرف **encoding, صحيح *scan_next)
{
    Py_ssize_t i = 0;
    *encoding = عدم;
    *scan_next = 1;
    لكل (; i < len; i++) {
        𐡄𐡍 (line[i] == '#') {
            اكسر;
        }
        𐡄𐡍 (line[i] == '\n' || line[i] == '\r') {
            أرجع 0;
        }
        𐡄𐡍 (line[i] != ' ' && line[i] != '\t' && line[i] != '\f') {
            *scan_next = 0;
            أرجع 0;
        }
    }
    ثابت Py_ssize_t marker_len = حجم("coding") - 1;
    لكل (; i < len - marker_len; i++) {
        𐡄𐡍 (memcmp(line + i, "coding", marker_len) != 0) {
            استمر;
        }
        ثابت حرف *cursor = line + i + marker_len;
        𐡄𐡍 (*cursor != ':' && *cursor != '=') {
            استمر;
        }
        𐡏𐡁𐡃 {
            cursor++;
        } طالما (cursor < line + len &&
                 (*cursor == ' ' || *cursor == '\t'));
        ثابت حرف *start = cursor;
        طالما (cursor < line + len &&
                (Py_ISALNUM(*cursor) || *cursor == '-' ||
                 *cursor == '_' || *cursor == '.')) {
            cursor++;
        }
        𐡄𐡍 (cursor == start) {
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
        𐡄𐡍 (strcmp(normalized, "utf-8") == 0 ||
                strncmp(normalized, "utf-8-", حجم("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        وإلا 𐡄𐡍 (strcmp(normalized, "latin-1") == 0 ||
                 strcmp(normalized, "iso-8859-1") == 0 ||
                 strcmp(normalized, "iso-latin-1") == 0 ||
                 strncmp(normalized, "latin-1-", حجم("latin-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-8859-1-", حجم("iso-8859-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-latin-1-", حجم("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        𐡄𐡍 (canonical != start) {
            encoding_len = strlen(canonical);
        }
        *encoding = _PyTok_CopyBytes(canonical, encoding_len);
        𐡄𐡍 (*encoding == عدم) {
            أرجع -1;
        }
        *scan_next = 0;
        أرجع 0;
    }
    أرجع 0;
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
    صحيح cookie_line = 1;
    ثابت حرف *first_data = first->data + *bom_len;
    Py_ssize_t first_len = first->len - *bom_len;
    𐡄𐡍 (find_cookie(first_data, first_len, &cookie, &scan_next) < 0) {
        أرجع _PYTOK_ENCODING_ERROR;
    }
    𐡄𐡍 (cookie == عدم && scan_next && second != عدم) {
        𐡄𐡍 (find_cookie(second->data, second->len, &cookie, &scan_next) < 0) {
            أرجع _PYTOK_ENCODING_ERROR;
        }
        cookie_line = 2;
    }
    وإلا 𐡄𐡍 (cookie == عدم && scan_next && !final) {
        أرجع _PYTOK_ENCODING_NEED_SECOND_LINE;
    }

    𐡄𐡍 (cookie == عدم) {
        𐡄𐡍 (bom && _PyTok_SetEncoding(tok, "utf-8") < 0) {
            أرجع _PYTOK_ENCODING_ERROR;
        }
        أرجع _PYTOK_ENCODING_DONE;
    }
    𐡄𐡍 (bom && strcmp(cookie, "utf-8") != 0) {
        ثابت _PyTok_Chunk *line = cookie_line == 2 ? second : first;
        ثابت حرف *line_data = line->data + (cookie_line == 1 ? *bom_len : 0);
        Py_ssize_t line_len = line->len - (cookie_line == 1 ? *bom_len : 0);
        صحيح end_col = (صحيح)Py_MIN(line_len, INT_MAX);
        𐡄𐡍 (end_col > 0 && (line_data[end_col - 1] == '\n' ||
                            line_data[end_col - 1] == '\r')) {
            end_col--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, line_data, 0, cookie_line, 0, end_col,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        أرجع _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    أرجع _PYTOK_ENCODING_DONE;
}

صحيح
_PyTok_DecodeOnce(بنية tok_state *tok, _PyTok_Chunk *chunk,
                  ثابت حرف *encoding, ثابت حرف *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->data, chunk->len, encoding, errors);
    𐡄𐡍 (unicode == عدم) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        أرجع -1;
    }
    أرجع chunk_set_unicode(tok, chunk, unicode, 0);
}

ساكن Py_ssize_t
raw_line_length(ثابت حرف *data, Py_ssize_t len)
{
    لكل (Py_ssize_t i = 0; i < len; i++) {
        𐡄𐡍 (data[i] == '\n') {
            أرجع i + 1;
        }
        𐡄𐡍 (data[i] == '\r') {
            أرجع i + 1 < len && data[i + 1] == '\n' ? i + 2 : i + 1;
        }
    }
    أرجع len;
}

ساكن صحيح
store_prepared_source(بنية tok_state *tok, ثابت حرف *data, Py_ssize_t len,
                      صحيح preserve_crlf, صحيح add_final_newline)
{
    Py_ssize_t pos = 0;
    طالما (pos < len) {
        Py_ssize_t raw_line_len;
        𐡄𐡍 (preserve_crlf) {
            ثابت حرف *newline = memchr(data + pos, '\n', len - pos);
            raw_line_len = newline == عدم
                ? len - pos : newline - data - pos + 1;
        }
        وإلا {
            raw_line_len = raw_line_length(data + pos, len - pos);
        }
        صحيح terminated = preserve_crlf
            ? data[pos + raw_line_len - 1] == '\n'
            : data[pos + raw_line_len - 1] == '\n' ||
              data[pos + raw_line_len - 1] == '\r';
        صحيح add_newline = add_final_newline &&
            pos + raw_line_len == len && !terminated;
        ثابت حرف *line = data + pos;
        // raw_line_length stops at the first CR or LF, so any CR is in
        // the line terminator.
        Py_ssize_t terminator_len = 0;
        𐡄𐡍 (!preserve_crlf) {
            𐡄𐡍 (line[raw_line_len - 1] == '\r') {
                terminator_len = 1;
            }
            وإلا 𐡄𐡍 (raw_line_len > 1 && line[raw_line_len - 2] == '\r') {
                terminator_len = 2;
            }
        }
        _PyTok_Off appended;
        𐡄𐡍 (terminator_len || add_newline) {
            appended = _PyTok_SourceAppendLineWithNewline(
                &tok->source, line, raw_line_len - terminator_len);
        }
        وإلا {
            appended = _PyTok_SourceAppendLine(&tok->source, line, raw_line_len);
        }
        𐡄𐡍 (appended < 0) {
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            أرجع -1;
        }
        tok->reader->prepared_final_newline = add_newline;
        pos += raw_line_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    𐡄𐡍 (add_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || data[len - 1] != '\r'))
    {
        𐡄𐡍 (_PyTok_SourceAppendLine(&tok->source, "\n", 1) < 0) {
            أرجع -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    أرجع 0;
}

صحيح
_PyTok_PrepareString(بنية tok_state *tok, ثابت حرف *input, صحيح utf8_only,
                     صحيح exec_input, صحيح preserve_crlf)
{
    Py_ssize_t raw_len = strlen(input);
    حرف *raw = (حرف *)input;

    𐡄𐡍 (utf8_only) {
        𐡄𐡍 (_PyTok_SetEncoding(tok, "utf-8") < 0) {
            أرجع -1;
        }
    }
    وإلا {
        Py_ssize_t first_original_len = raw_line_length(raw, raw_len);
        _PyTok_Chunk first = {
            .data = raw,
            .len = first_original_len,
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        _PyTok_Chunk second = {0};
        صحيح have_second = first_original_len < raw_len;
        𐡄𐡍 (have_second) {
            second.data = raw + first_original_len;
            second.len = raw_line_length(second.data,
                                         raw_len - first_original_len);
        }
        Py_ssize_t bom_len;
        _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
            tok, &first, have_second ? &second : عدم, 1, &bom_len);
        𐡄𐡍 (detection == _PYTOK_ENCODING_ERROR) {
            أرجع -1;
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
    𐡄𐡍 (encoding != عدم && strcmp(encoding, "utf-8") != 0) {
        𐡄𐡍 (_PyTok_DecodeOnce(
                tok, &decoded, encoding, عدم) < 0) {
            أرجع -1;
        }
    }

    صحيح stored = store_prepared_source(
        tok, decoded.data, decoded.len, preserve_crlf, exec_input);
    _PyTok_ChunkClear(&decoded);
    𐡄𐡍 (stored < 0) {
        أرجع -1;
    }
    𐡄𐡍 (!utf8_only &&
            (encoding == عدم || strcmp(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(_PyTok_SourceData(&tok->source), tok, 1)) {
        أرجع -1;
    }
    أرجع 0;
}

صحيح
_PyTok_StartDecoder(بنية tok_state *tok, ثابت حرف *errors)
{
    _PyTok_Reader *reader = tok->reader;
    𐡄𐡍 (reader->encoding == عدم || reader->decoder != عدم) {
        أرجع 0;
    }
    𐡄𐡍 (reader->kind == _PYTOK_READER_FILE &&
            strcmp(reader->encoding, "utf-8") == 0) {
        أرجع 0;
    }

    PyObject *codec = _PyCodec_LookupTextEncoding(reader->encoding, عدم);
    𐡄𐡍 (codec != عدم) {
        reader->decoder = _PyCodecInfo_GetIncrementalDecoder(codec, errors);
        Py_DECREF(codec);
    }
    𐡄𐡍 (reader->decoder == عدم) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        𐡄𐡍 (reader->kind == _PYTOK_READER_FILE) {
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
    𐡄𐡍 (reader->decoder == عدم) {
        أرجع 0;
    }
    صحيح strip_bom = reader->kind == _PYTOK_READER_READLINE &&
        chunk->len >= UTF16_BOM_LENGTH &&
        (memcmp(chunk->data, UTF16_LE_BOM, UTF16_BOM_LENGTH) == 0 ||
         memcmp(chunk->data, UTF16_BE_BOM, UTF16_BOM_LENGTH) == 0);
    PyObject *input;
    𐡄𐡍 (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            PyBytes_Check(chunk->owner) &&
            chunk->data == PyBytes_AS_STRING(chunk->owner)) {
        input = Py_NewRef(chunk->owner);
    }
    وإلا {
        input = PyBytes_FromStringAndSize(chunk->data, chunk->len);
    }
    𐡄𐡍 (input == عدم) {
        tok->done = E_NOMEM;
        أرجع -1;
    }
    PyObject *unicode = PyObject_CallMethodObjArgs(
        reader->decoder, &_Py_ID(decode), input,
        final ? Py_True : Py_False, عدم);
    Py_DECREF(input);
    𐡄𐡍 (unicode == عدم) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        𐡄𐡍 (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != عدم ? tok->filename : Py_None);
        }
        أرجع -1;
    }
    𐡄𐡍 (!PyUnicode_Check(unicode)) {
        PyErr_Format(PyExc_TypeError,
                     "decoder should return a string result, not '%.200s'",
                     Py_TYPE(unicode)->tp_name);
        Py_DECREF(unicode);
        tok->done = E_DECODE;
        أرجع -1;
    }
    أرجع chunk_set_unicode(tok, chunk, unicode, strip_bom);
}

صحيح
_PyTok_DecoderHasBufferedInput(بنية tok_state *tok)
{
    𐡄𐡍 (tok->reader->decoder == عدم) {
        أرجع 0;
    }
    PyObject *state = PyObject_CallMethodNoArgs(
        tok->reader->decoder, &_Py_ID(getstate));
    𐡄𐡍 (state == عدم) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        أرجع -1;
    }
    𐡄𐡍 (!PyTuple_Check(state) || PyTuple_GET_SIZE(state) != 2 ||
            !PyBytes_Check(PyTuple_GET_ITEM(state, 0)) ||
            !PyLong_Check(PyTuple_GET_ITEM(state, 1))) {
        Py_DECREF(state);
        PyErr_SetString(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->done = E_DECODE;
        أرجع -1;
    }
    صحيح pending = PyBytes_GET_SIZE(PyTuple_GET_ITEM(state, 0)) != 0;
    Py_DECREF(state);
    أرجع pending;
}
