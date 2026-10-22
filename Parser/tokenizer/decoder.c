#அந்தர்பாவ "Python.h"
#அந்தர்பாவ "pycore_codecs.h"
#அந்தர்பாவ "pycore_global_strings.h"
#அந்தர்பாவ "pycore_runtime.h"
#அந்தர்பாவ "errcode.h"

#அந்தர்பாவ "reader_internal.h"
#அந்தர்பாவ "helpers.h"
#அந்தர்பாவ "../lexer/state.h"

#define UNICODE_BOM 0xFEFF
#define UTF16_LE_BOM "\xff\xfe"
#define UTF16_BE_BOM "\xfe\xff"
#define UTF16_BOM_LENGTH ((Py_ssize_t)(அளவு(UTF16_LE_BOM) - 1))
#define UTF8_BOM "\xef\xbb\xbf"
#define UTF8_BOM_LENGTH ((Py_ssize_t)(அளவு(UTF8_BOM) - 1))

எழுத்து *
_PyTok_CopyBytes(மாறிலி எழுத்து *data, Py_ssize_t len)
{
    எனில் (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        திரும்பு வெற்று;
    }
    எழுத்து *copy = PyMem_Malloc((size_t)len + 1);
    எனில் (copy == வெற்று) {
        PyErr_NoMemory();
        திரும்பு வெற்று;
    }
    memcpy(copy, data, (size_t)len);
    copy[len] = '\0';
    திரும்பு copy;
}

நிலையான வெறுமை
chunk_release_data(_PyTok_Chunk *chunk)
{
    தேர்வு (chunk->ownership) {
        நிலை _PYTOK_CHUNK_BORROWED:
            நிறுத்து;
        நிலை _PYTOK_CHUNK_PYMEM:
            PyMem_Free(chunk->data);
            நிறுத்து;
        நிலை _PYTOK_CHUNK_PYOBJECT:
            Py_DECREF(chunk->owner);
            நிறுத்து;
    }
}

வெறுமை
_PyTok_ChunkClear(_PyTok_Chunk *chunk)
{
    chunk_release_data(chunk);
    *chunk = (_PyTok_Chunk){0};
}

நிலையான முழு
chunk_set_unicode(கட்டமைப்பு tok_state *tok, _PyTok_Chunk *chunk,
                  PyObject *unicode, முழு strip_bom)
{
    Py_ssize_t utf8_len;
    மாறிலி எழுத்து *utf8 = PyUnicode_AsUTF8AndSize(unicode, &utf8_len);
    எனில் (utf8 == வெற்று) {
        Py_DECREF(unicode);
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        திரும்பு -1;
    }
    எனில் (strip_bom && PyUnicode_GET_LENGTH(unicode) > 0 &&
            PyUnicode_ReadChar(unicode, 0) == UNICODE_BOM) {
        utf8 += UTF8_BOM_LENGTH;
        utf8_len -= UTF8_BOM_LENGTH;
    }
    chunk_release_data(chunk);
    chunk->owner = unicode;
    chunk->data = (எழுத்து *)utf8;
    chunk->len = utf8_len;
    chunk->ownership = _PYTOK_CHUNK_PYOBJECT;
    திரும்பு 0;
}

எழுத்து *
_PyTok_NormalizeNewlines(மாறிலி எழுத்து *data, Py_ssize_t len,
                         Py_ssize_t *out_len)
{
    எனில் (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        திரும்பு வெற்று;
    }
    எழுத்து *result = PyMem_Malloc((size_t)len + 1);
    எனில் (result == வெற்று) {
        PyErr_NoMemory();
        திரும்பு வெற்று;
    }
    Py_ssize_t லிக = 0;
    க்கு (Py_ssize_t பட = 0; பட < len; பட++) {
        எழுத்து c = data[பட];
        எனில் (c == '\r') {
            எனில் (பட + 1 < len && data[பட + 1] == '\n') {
                பட++;
            }
            c = '\n';
        }
        result[லிக++] = c;
    }
    result[லிக] = '\0';
    *out_len = லிக;
    திரும்பு result;
}

முழு
_PyTok_SetEncoding(கட்டமைப்பு tok_state *tok, மாறிலி எழுத்து *encoding)
{
    எழுத்து *copy = _PyTok_CopyBytes(encoding, strlen(encoding));
    எனில் (copy == வெற்று) {
        tok->done = E_NOMEM;
        திரும்பு -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    திரும்பு 0;
}

நிலையான முழு
find_cookie(மாறிலி எழுத்து *வரி, Py_ssize_t len, எழுத்து **encoding, முழு *scan_next)
{
    Py_ssize_t i = 0;
    *encoding = வெற்று;
    *scan_next = 1;
    க்கு (; i < len; i++) {
        எனில் (வரி[i] == '#') {
            நிறுத்து;
        }
        எனில் (வரி[i] == '\n' || வரி[i] == '\r') {
            திரும்பு 0;
        }
        எனில் (வரி[i] != ' ' && வரி[i] != '\t' && வரி[i] != '\f') {
            *scan_next = 0;
            திரும்பு 0;
        }
    }
    மாறிலி Py_ssize_t marker_len = அளவு("coding") - 1;
    க்கு (; i < len - marker_len; i++) {
        எனில் (memcmp(வரி + i, "coding", marker_len) != 0) {
            தொடர்;
        }
        மாறிலி எழுத்து *cursor = வரி + i + marker_len;
        எனில் (*cursor != ':' && *cursor != '=') {
            தொடர்;
        }
        செய் {
            cursor++;
        } போது (cursor < வரி + len &&
                 (*cursor == ' ' || *cursor == '\t'));
        மாறிலி எழுத்து *start = cursor;
        போது (cursor < வரி + len &&
                (Py_ISALNUM(*cursor) || *cursor == '-' ||
                 *cursor == '_' || *cursor == '.')) {
            cursor++;
        }
        எனில் (cursor == start) {
            தொடர்;
        }
        Py_ssize_t encoding_len = cursor - start;
        // Include the longest recognized prefix and its NUL terminator.
        எழுத்து normalized[அளவு("iso-latin-1-")];
        மாறிலி Py_ssize_t prefix_len = Py_MIN(
            encoding_len, (Py_ssize_t)அளவு(normalized) - 1);
        Py_ssize_t n;
        க்கு (n = 0; n < prefix_len; n++) {
            normalized[n] = start[n] == '_' ? '-' : Py_TOLOWER(start[n]);
        }
        normalized[n] = '\0';
        மாறிலி எழுத்து *canonical = start;
        எனில் (strcmp(normalized, "utf-8") == 0 ||
                strncmp(normalized, "utf-8-", அளவு("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        இல்லையெனில் எனில் (strcmp(normalized, "latin-1") == 0 ||
                 strcmp(normalized, "iso-8859-1") == 0 ||
                 strcmp(normalized, "iso-latin-1") == 0 ||
                 strncmp(normalized, "latin-1-", அளவு("latin-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-8859-1-", அளவு("iso-8859-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-latin-1-", அளவு("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        எனில் (canonical != start) {
            encoding_len = strlen(canonical);
        }
        *encoding = _PyTok_CopyBytes(canonical, encoding_len);
        எனில் (*encoding == வெற்று) {
            திரும்பு -1;
        }
        *scan_next = 0;
        திரும்பு 0;
    }
    திரும்பு 0;
}

_PyTok_EncodingResult
_PyTok_DetectEncoding(கட்டமைப்பு tok_state *tok, மாறிலி _PyTok_Chunk *first,
                      மாறிலி _PyTok_Chunk *second, முழு final,
                      Py_ssize_t *bom_len)
{
    முழு bom = first->len >= UTF8_BOM_LENGTH &&
        memcmp(first->data, UTF8_BOM, UTF8_BOM_LENGTH) == 0;
    *bom_len = bom ? UTF8_BOM_LENGTH : 0;

    எழுத்து *cookie = வெற்று;
    முழு scan_next = 0;
    முழு cookie_வரி = 1;
    மாறிலி எழுத்து *first_data = first->data + *bom_len;
    Py_ssize_t first_len = first->len - *bom_len;
    எனில் (find_cookie(first_data, first_len, &cookie, &scan_next) < 0) {
        திரும்பு _PYTOK_ENCODING_ERROR;
    }
    எனில் (cookie == வெற்று && scan_next && second != வெற்று) {
        எனில் (find_cookie(second->data, second->len, &cookie, &scan_next) < 0) {
            திரும்பு _PYTOK_ENCODING_ERROR;
        }
        cookie_வரி = 2;
    }
    இல்லையெனில் எனில் (cookie == வெற்று && scan_next && !final) {
        திரும்பு PYTOK_ENCODING_NEED_SECOND_வரி;
    }

    எனில் (cookie == வெற்று) {
        எனில் (bom && _PyTok_SetEncoding(tok, "utf-8") < 0) {
            திரும்பு _PYTOK_ENCODING_ERROR;
        }
        திரும்பு _PYTOK_ENCODING_DONE;
    }
    எனில் (bom && strcmp(cookie, "utf-8") != 0) {
        மாறிலி _PyTok_Chunk *வரி = cookie_வரி == 2 ? second : first;
        மாறிலி எழுத்து *வரி_data = வரி->data + (cookie_வரி == 1 ? *bom_len : 0);
        Py_ssize_t வரி_len = வரி->len - (cookie_வரி == 1 ? *bom_len : 0);
        முழு end_col = (முழு)Py_MIN(வரி_len, INT_MAX);
        எனில் (end_col > 0 && (வரி_data[end_col - 1] == '\n' ||
                            வரி_data[end_col - 1] == '\r')) {
            end_col--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, வரி_data, 0, cookie_வரி, 0, end_col,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        திரும்பு _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    திரும்பு _PYTOK_ENCODING_DONE;
}

முழு
_PyTok_DecodeOnce(கட்டமைப்பு tok_state *tok, _PyTok_Chunk *chunk,
                  மாறிலி எழுத்து *encoding, மாறிலி எழுத்து *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->data, chunk->len, encoding, errors);
    எனில் (unicode == வெற்று) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        திரும்பு -1;
    }
    திரும்பு chunk_set_unicode(tok, chunk, unicode, 0);
}

நிலையான Py_ssize_t
raw_வரி_length(மாறிலி எழுத்து *data, Py_ssize_t len)
{
    க்கு (Py_ssize_t i = 0; i < len; i++) {
        எனில் (data[i] == '\n') {
            திரும்பு i + 1;
        }
        எனில் (data[i] == '\r') {
            திரும்பு i + 1 < len && data[i + 1] == '\n' ? i + 2 : i + 1;
        }
    }
    திரும்பு len;
}

நிலையான முழு
store_prepared_source(கட்டமைப்பு tok_state *tok, மாறிலி எழுத்து *data, Py_ssize_t len,
                      முழு preserve_crlf, முழு add_final_newline)
{
    Py_ssize_t pos = 0;
    போது (pos < len) {
        Py_ssize_t raw_வரி_len;
        எனில் (preserve_crlf) {
            மாறிலி எழுத்து *newline = memchr(data + pos, '\n', len - pos);
            raw_வரி_len = newline == வெற்று
                ? len - pos : newline - data - pos + 1;
        }
        இல்லையெனில் {
            raw_வரி_len = raw_வரி_length(data + pos, len - pos);
        }
        முழு terminated = preserve_crlf
            ? data[pos + raw_வரி_len - 1] == '\n'
            : data[pos + raw_வரி_len - 1] == '\n' ||
              data[pos + raw_வரி_len - 1] == '\r';
        முழு add_newline = add_final_newline &&
            pos + raw_வரி_len == len && !terminated;
        மாறிலி எழுத்து *வரி = data + pos;
        // raw_line_length stops at the first CR or LF, so any CR is in
        // the line terminator.
        Py_ssize_t terminator_len = 0;
        எனில் (!preserve_crlf) {
            எனில் (வரி[raw_வரி_len - 1] == '\r') {
                terminator_len = 1;
            }
            இல்லையெனில் எனில் (raw_வரி_len > 1 && வரி[raw_வரி_len - 2] == '\r') {
                terminator_len = 2;
            }
        }
        _PyTok_Off appended;
        எனில் (terminator_len || add_newline) {
            appended = Py_Tok_Source_Append_வரி_With_Newline(
                &tok->source, வரி, raw_வரி_len - terminator_len);
        }
        இல்லையெனில் {
            appended = Py_Tok_Source_Append_வரி(&tok->source, வரி, raw_வரி_len);
        }
        எனில் (appended < 0) {
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            திரும்பு -1;
        }
        tok->reader->prepared_final_newline = add_newline;
        pos += raw_வரி_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    எனில் (add_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || data[len - 1] != '\r'))
    {
        எனில் (Py_Tok_Source_Append_வரி(&tok->source, "\n", 1) < 0) {
            திரும்பு -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    திரும்பு 0;
}

முழு
_PyTok_PrepareString(கட்டமைப்பு tok_state *tok, மாறிலி எழுத்து *உள்ளீடு, முழு utf8_only,
                     முழு exec_உள்ளீடு, முழு preserve_crlf)
{
    Py_ssize_t raw_len = strlen(உள்ளீடு);
    எழுத்து *raw = (எழுத்து *)உள்ளீடு;

    எனில் (utf8_only) {
        எனில் (_PyTok_SetEncoding(tok, "utf-8") < 0) {
            திரும்பு -1;
        }
    }
    இல்லையெனில் {
        Py_ssize_t first_original_len = raw_வரி_length(raw, raw_len);
        _PyTok_Chunk first = {
            .data = raw,
            .len = first_original_len,
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        _PyTok_Chunk second = {0};
        முழு have_second = first_original_len < raw_len;
        எனில் (have_second) {
            second.data = raw + first_original_len;
            second.len = raw_வரி_length(second.data,
                                         raw_len - first_original_len);
        }
        Py_ssize_t bom_len;
        _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
            tok, &first, have_second ? &second : வெற்று, 1, &bom_len);
        எனில் (detection == _PYTOK_ENCODING_ERROR) {
            திரும்பு -1;
        }
        raw += bom_len;
        raw_len -= bom_len;
    }

    _PyTok_Chunk decoded = {
        .data = raw,
        .len = raw_len,
        .ownership = _PYTOK_CHUNK_BORROWED,
    };
    மாறிலி எழுத்து *encoding = tok->reader->encoding;
    எனில் (encoding != வெற்று && strcmp(encoding, "utf-8") != 0) {
        எனில் (_PyTok_DecodeOnce(
                tok, &decoded, encoding, வெற்று) < 0) {
            திரும்பு -1;
        }
    }

    முழு stored = store_prepared_source(
        tok, decoded.data, decoded.len, preserve_crlf, exec_உள்ளீடு);
    _PyTok_ChunkClear(&decoded);
    எனில் (stored < 0) {
        திரும்பு -1;
    }
    எனில் (!utf8_only &&
            (encoding == வெற்று || strcmp(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(_PyTok_SourceData(&tok->source), tok, 1)) {
        திரும்பு -1;
    }
    திரும்பு 0;
}

முழு
_PyTok_StartDecoder(கட்டமைப்பு tok_state *tok, மாறிலி எழுத்து *errors)
{
    _PyTok_Reader *reader = tok->reader;
    எனில் (reader->encoding == வெற்று || reader->decoder != வெற்று) {
        திரும்பு 0;
    }
    எனில் (reader->kind == _PYTOK_READER_FILE &&
            strcmp(reader->encoding, "utf-8") == 0) {
        திரும்பு 0;
    }

    PyObject *codec = _PyCodec_LookupTextEncoding(reader->encoding, வெற்று);
    எனில் (codec != வெற்று) {
        reader->decoder = _PyCodecInfo_GetIncrementalDecoder(codec, errors);
        Py_DECREF(codec);
    }
    எனில் (reader->decoder == வெற்று) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        எனில் (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != வெற்று ? tok->filename : Py_None);
        }
        திரும்பு -1;
    }
    திரும்பு 0;
}

முழு
_PyTok_DecodeChunk(கட்டமைப்பு tok_state *tok, _PyTok_Chunk *chunk, முழு final)
{
    _PyTok_Reader *reader = tok->reader;
    எனில் (reader->decoder == வெற்று) {
        திரும்பு 0;
    }
    முழு strip_bom = reader->kind == _PYTOK_READER_READLINE &&
        chunk->len >= UTF16_BOM_LENGTH &&
        (memcmp(chunk->data, UTF16_LE_BOM, UTF16_BOM_LENGTH) == 0 ||
         memcmp(chunk->data, UTF16_BE_BOM, UTF16_BOM_LENGTH) == 0);
    PyObject *உள்ளீடு;
    எனில் (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            PyBytes_Check(chunk->owner) &&
            chunk->data == PyBytes_AS_STRING(chunk->owner)) {
        உள்ளீடு = Py_NewRef(chunk->owner);
    }
    இல்லையெனில் {
        உள்ளீடு = PyBytes_FromStringAndSize(chunk->data, chunk->len);
    }
    எனில் (உள்ளீடு == வெற்று) {
        tok->done = E_NOMEM;
        திரும்பு -1;
    }
    PyObject *unicode = PyObject_CallMethodObjArgs(
        reader->decoder, &_Py_ID(decode), உள்ளீடு,
        final ? Py_True : Py_False, வெற்று);
    Py_DECREF(உள்ளீடு);
    எனில் (unicode == வெற்று) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        எனில் (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != வெற்று ? tok->filename : Py_None);
        }
        திரும்பு -1;
    }
    எனில் (!PyUnicode_Check(unicode)) {
        PyErr_Format(PyExc_TypeError,
                     "decoder should return a string result, not '%.200s'",
                     Py_TYPE(unicode)->tp_name);
        Py_DECREF(unicode);
        tok->done = E_DECODE;
        திரும்பு -1;
    }
    திரும்பு chunk_set_unicode(tok, chunk, unicode, strip_bom);
}

முழு
Py_Tok_Decoder_Has_Buffered_உள்ளீடு(கட்டமைப்பு tok_state *tok)
{
    எனில் (tok->reader->decoder == வெற்று) {
        திரும்பு 0;
    }
    PyObject *state = PyObject_CallMethodNoArgs(
        tok->reader->decoder, &_Py_ID(getstate));
    எனில் (state == வெற்று) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        திரும்பு -1;
    }
    எனில் (!PyTuple_Check(state) || PyTuple_GET_SIZE(state) != 2 ||
            !PyBytes_Check(PyTuple_GET_ITEM(state, 0)) ||
            !PyLong_Check(PyTuple_GET_ITEM(state, 1))) {
        Py_DECREF(state);
        PyErr_SetString(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->done = E_DECODE;
        திரும்பு -1;
    }
    முழு pending = PyBytes_GET_SIZE(PyTuple_GET_ITEM(state, 0)) != 0;
    Py_DECREF(state);
    திரும்பு pending;
}
