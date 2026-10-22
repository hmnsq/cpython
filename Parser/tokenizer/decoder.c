#अन्तर्भाव "Python.h"
#अन्तर्भाव "pycore_codecs.h"
#अन्तर्भाव "pycore_global_strings.h"
#अन्तर्भाव "pycore_runtime.h"
#अन्तर्भाव "errcode.h"

#अन्तर्भाव "reader_internal.h"
#अन्तर्भाव "helpers.h"
#अन्तर्भाव "../lexer/state.h"

#define UNICODE_BOM 0xFEFF
#define UTF16_LE_BOM "\xff\xfe"
#define UTF16_BE_BOM "\xfe\xff"
#define UTF16_BOM_LENGTH ((Py_ssize_t)(परिमाण(UTF16_LE_BOM) - 1))
#define UTF8_BOM "\xef\xbb\xbf"
#define UTF8_BOM_LENGTH ((Py_ssize_t)(परिमाण(UTF8_BOM) - 1))

वर्ण *
_PyTok_CopyBytes(ध्रुव वर्ण *data, Py_ssize_t len)
{
    सचे (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        पटिनिवत्त रिक्त;
    }
    वर्ण *copy = PyMem_Malloc((size_t)len + 1);
    सचे (copy == रिक्त) {
        PyErr_NoMemory();
        पटिनिवत्त रिक्त;
    }
    memcpy(copy, data, (size_t)len);
    copy[len] = '\0';
    पटिनिवत्त copy;
}

स्थिर शून्य
chunk_release_data(_PyTok_Chunk *chunk)
{
    विकल्प (chunk->ownership) {
        पक्ष _PYTOK_CHUNK_BORROWED:
            भङ्ग;
        पक्ष _PYTOK_CHUNK_PYMEM:
            PyMem_Free(chunk->data);
            भङ्ग;
        पक्ष _PYTOK_CHUNK_PYOBJECT:
            Py_DECREF(chunk->owner);
            भङ्ग;
    }
}

शून्य
_PyTok_ChunkClear(_PyTok_Chunk *chunk)
{
    chunk_release_data(chunk);
    *chunk = (_PyTok_Chunk){0};
}

स्थिर पूर्णाङ्क
chunk_set_unicode(संरचना tok_state *tok, _PyTok_Chunk *chunk,
                  PyObject *unicode, पूर्णाङ्क strip_bom)
{
    Py_ssize_t utf8_len;
    ध्रुव वर्ण *utf8 = PyUnicode_AsUTF8AndSize(unicode, &utf8_len);
    सचे (utf8 == रिक्त) {
        Py_DECREF(unicode);
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        पटिनिवत्त -1;
    }
    सचे (strip_bom && PyUnicode_GET_LENGTH(unicode) > 0 &&
            PyUnicode_ReadChar(unicode, 0) == UNICODE_BOM) {
        utf8 += UTF8_BOM_LENGTH;
        utf8_len -= UTF8_BOM_LENGTH;
    }
    chunk_release_data(chunk);
    chunk->owner = unicode;
    chunk->data = (वर्ण *)utf8;
    chunk->len = utf8_len;
    chunk->ownership = _PYTOK_CHUNK_PYOBJECT;
    पटिनिवत्त 0;
}

वर्ण *
_PyTok_NormalizeNewlines(ध्रुव वर्ण *data, Py_ssize_t len,
                         Py_ssize_t *out_len)
{
    सचे (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        पटिनिवत्त रिक्त;
    }
    वर्ण *result = PyMem_Malloc((size_t)len + 1);
    सचे (result == रिक्त) {
        PyErr_NoMemory();
        पटिनिवत्त रिक्त;
    }
    Py_ssize_t लिख = 0;
    अत्थाय (Py_ssize_t पठ = 0; पठ < len; पठ++) {
        वर्ण c = data[पठ];
        सचे (c == '\r') {
            सचे (पठ + 1 < len && data[पठ + 1] == '\n') {
                पठ++;
            }
            c = '\n';
        }
        result[लिख++] = c;
    }
    result[लिख] = '\0';
    *out_len = लिख;
    पटिनिवत्त result;
}

पूर्णाङ्क
_PyTok_SetEncoding(संरचना tok_state *tok, ध्रुव वर्ण *encoding)
{
    वर्ण *copy = _PyTok_CopyBytes(encoding, strlen(encoding));
    सचे (copy == रिक्त) {
        tok->done = E_NOMEM;
        पटिनिवत्त -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क
find_cookie(ध्रुव वर्ण *पङ्क्तिः, Py_ssize_t len, वर्ण **encoding, पूर्णाङ्क *scan_next)
{
    Py_ssize_t i = 0;
    *encoding = रिक्त;
    *scan_next = 1;
    अत्थाय (; i < len; i++) {
        सचे (पङ्क्तिः[i] == '#') {
            भङ्ग;
        }
        सचे (पङ्क्तिः[i] == '\n' || पङ्क्तिः[i] == '\r') {
            पटिनिवत्त 0;
        }
        सचे (पङ्क्तिः[i] != ' ' && पङ्क्तिः[i] != '\t' && पङ्क्तिः[i] != '\f') {
            *scan_next = 0;
            पटिनिवत्त 0;
        }
    }
    ध्रुव Py_ssize_t marker_len = परिमाण("coding") - 1;
    अत्थाय (; i < len - marker_len; i++) {
        सचे (memcmp(पङ्क्तिः + i, "coding", marker_len) != 0) {
            अनुवर्त;
        }
        ध्रुव वर्ण *cursor = पङ्क्तिः + i + marker_len;
        सचे (*cursor != ':' && *cursor != '=') {
            अनुवर्त;
        }
        करोहि {
            cursor++;
        } याव (cursor < पङ्क्तिः + len &&
                 (*cursor == ' ' || *cursor == '\t'));
        ध्रुव वर्ण *start = cursor;
        याव (cursor < पङ्क्तिः + len &&
                (Py_ISALNUM(*cursor) || *cursor == '-' ||
                 *cursor == '_' || *cursor == '.')) {
            cursor++;
        }
        सचे (cursor == start) {
            अनुवर्त;
        }
        Py_ssize_t encoding_len = cursor - start;
        // Include the longest recognized prefix and its NUL terminator.
        वर्ण normalized[परिमाण("iso-latin-1-")];
        ध्रुव Py_ssize_t prefix_len = Py_MIN(
            encoding_len, (Py_ssize_t)परिमाण(normalized) - 1);
        Py_ssize_t n;
        अत्थाय (n = 0; n < prefix_len; n++) {
            normalized[n] = start[n] == '_' ? '-' : Py_TOLOWER(start[n]);
        }
        normalized[n] = '\0';
        ध्रुव वर्ण *canonical = start;
        सचे (strcmp(normalized, "utf-8") == 0 ||
                strncmp(normalized, "utf-8-", परिमाण("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        अञ्ञथा सचे (strcmp(normalized, "latin-1") == 0 ||
                 strcmp(normalized, "iso-8859-1") == 0 ||
                 strcmp(normalized, "iso-latin-1") == 0 ||
                 strncmp(normalized, "latin-1-", परिमाण("latin-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-8859-1-", परिमाण("iso-8859-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-latin-1-", परिमाण("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        सचे (canonical != start) {
            encoding_len = strlen(canonical);
        }
        *encoding = _PyTok_CopyBytes(canonical, encoding_len);
        सचे (*encoding == रिक्त) {
            पटिनिवत्त -1;
        }
        *scan_next = 0;
        पटिनिवत्त 0;
    }
    पटिनिवत्त 0;
}

_PyTok_EncodingResult
_PyTok_DetectEncoding(संरचना tok_state *tok, ध्रुव _PyTok_Chunk *first,
                      ध्रुव _PyTok_Chunk *second, पूर्णाङ्क final,
                      Py_ssize_t *bom_len)
{
    पूर्णाङ्क bom = first->len >= UTF8_BOM_LENGTH &&
        memcmp(first->data, UTF8_BOM, UTF8_BOM_LENGTH) == 0;
    *bom_len = bom ? UTF8_BOM_LENGTH : 0;

    वर्ण *cookie = रिक्त;
    पूर्णाङ्क scan_next = 0;
    पूर्णाङ्क cookie_पङ्क्तिः = 1;
    ध्रुव वर्ण *first_data = first->data + *bom_len;
    Py_ssize_t first_len = first->len - *bom_len;
    सचे (find_cookie(first_data, first_len, &cookie, &scan_next) < 0) {
        पटिनिवत्त _PYTOK_ENCODING_ERROR;
    }
    सचे (cookie == रिक्त && scan_next && second != रिक्त) {
        सचे (find_cookie(second->data, second->len, &cookie, &scan_next) < 0) {
            पटिनिवत्त _PYTOK_ENCODING_ERROR;
        }
        cookie_पङ्क्तिः = 2;
    }
    अञ्ञथा सचे (cookie == रिक्त && scan_next && !final) {
        पटिनिवत्त PYTOK_ENCODING_NEED_SECOND_पङ्क्तिः;
    }

    सचे (cookie == रिक्त) {
        सचे (bom && _PyTok_SetEncoding(tok, "utf-8") < 0) {
            पटिनिवत्त _PYTOK_ENCODING_ERROR;
        }
        पटिनिवत्त _PYTOK_ENCODING_DONE;
    }
    सचे (bom && strcmp(cookie, "utf-8") != 0) {
        ध्रुव _PyTok_Chunk *पङ्क्तिः = cookie_पङ्क्तिः == 2 ? second : first;
        ध्रुव वर्ण *पङ्क्तिः_data = पङ्क्तिः->data + (cookie_पङ्क्तिः == 1 ? *bom_len : 0);
        Py_ssize_t पङ्क्तिः_len = पङ्क्तिः->len - (cookie_पङ्क्तिः == 1 ? *bom_len : 0);
        पूर्णाङ्क end_col = (पूर्णाङ्क)Py_MIN(पङ्क्तिः_len, INT_MAX);
        सचे (end_col > 0 && (पङ्क्तिः_data[end_col - 1] == '\n' ||
                            पङ्क्तिः_data[end_col - 1] == '\r')) {
            end_col--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, पङ्क्तिः_data, 0, cookie_पङ्क्तिः, 0, end_col,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        पटिनिवत्त _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    पटिनिवत्त _PYTOK_ENCODING_DONE;
}

पूर्णाङ्क
_PyTok_DecodeOnce(संरचना tok_state *tok, _PyTok_Chunk *chunk,
                  ध्रुव वर्ण *encoding, ध्रुव वर्ण *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->data, chunk->len, encoding, errors);
    सचे (unicode == रिक्त) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        पटिनिवत्त -1;
    }
    पटिनिवत्त chunk_set_unicode(tok, chunk, unicode, 0);
}

स्थिर Py_ssize_t
raw_पङ्क्तिः_length(ध्रुव वर्ण *data, Py_ssize_t len)
{
    अत्थाय (Py_ssize_t i = 0; i < len; i++) {
        सचे (data[i] == '\n') {
            पटिनिवत्त i + 1;
        }
        सचे (data[i] == '\r') {
            पटिनिवत्त i + 1 < len && data[i + 1] == '\n' ? i + 2 : i + 1;
        }
    }
    पटिनिवत्त len;
}

स्थिर पूर्णाङ्क
store_prepared_source(संरचना tok_state *tok, ध्रुव वर्ण *data, Py_ssize_t len,
                      पूर्णाङ्क preserve_crlf, पूर्णाङ्क add_final_newline)
{
    Py_ssize_t pos = 0;
    याव (pos < len) {
        Py_ssize_t raw_पङ्क्तिः_len;
        सचे (preserve_crlf) {
            ध्रुव वर्ण *newline = memchr(data + pos, '\n', len - pos);
            raw_पङ्क्तिः_len = newline == रिक्त
                ? len - pos : newline - data - pos + 1;
        }
        अञ्ञथा {
            raw_पङ्क्तिः_len = raw_पङ्क्तिः_length(data + pos, len - pos);
        }
        पूर्णाङ्क terminated = preserve_crlf
            ? data[pos + raw_पङ्क्तिः_len - 1] == '\n'
            : data[pos + raw_पङ्क्तिः_len - 1] == '\n' ||
              data[pos + raw_पङ्क्तिः_len - 1] == '\r';
        पूर्णाङ्क add_newline = add_final_newline &&
            pos + raw_पङ्क्तिः_len == len && !terminated;
        ध्रुव वर्ण *पङ्क्तिः = data + pos;
        // raw_line_length stops at the first CR or LF, so any CR is in
        // the line terminator.
        Py_ssize_t terminator_len = 0;
        सचे (!preserve_crlf) {
            सचे (पङ्क्तिः[raw_पङ्क्तिः_len - 1] == '\r') {
                terminator_len = 1;
            }
            अञ्ञथा सचे (raw_पङ्क्तिः_len > 1 && पङ्क्तिः[raw_पङ्क्तिः_len - 2] == '\r') {
                terminator_len = 2;
            }
        }
        _PyTok_Off appended;
        सचे (terminator_len || add_newline) {
            appended = Py_Tok_Source_Append_पङ्क्तिः_With_Newline(
                &tok->source, पङ्क्तिः, raw_पङ्क्तिः_len - terminator_len);
        }
        अञ्ञथा {
            appended = Py_Tok_Source_Append_पङ्क्तिः(&tok->source, पङ्क्तिः, raw_पङ्क्तिः_len);
        }
        सचे (appended < 0) {
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            पटिनिवत्त -1;
        }
        tok->reader->prepared_final_newline = add_newline;
        pos += raw_पङ्क्तिः_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    सचे (add_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || data[len - 1] != '\r'))
    {
        सचे (Py_Tok_Source_Append_पङ्क्तिः(&tok->source, "\n", 1) < 0) {
            पटिनिवत्त -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    पटिनिवत्त 0;
}

पूर्णाङ्क
_PyTok_PrepareString(संरचना tok_state *tok, ध्रुव वर्ण *निवेशः, पूर्णाङ्क utf8_only,
                     पूर्णाङ्क exec_निवेशः, पूर्णाङ्क preserve_crlf)
{
    Py_ssize_t raw_len = strlen(निवेशः);
    वर्ण *raw = (वर्ण *)निवेशः;

    सचे (utf8_only) {
        सचे (_PyTok_SetEncoding(tok, "utf-8") < 0) {
            पटिनिवत्त -1;
        }
    }
    अञ्ञथा {
        Py_ssize_t first_original_len = raw_पङ्क्तिः_length(raw, raw_len);
        _PyTok_Chunk first = {
            .data = raw,
            .len = first_original_len,
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        _PyTok_Chunk second = {0};
        पूर्णाङ्क have_second = first_original_len < raw_len;
        सचे (have_second) {
            second.data = raw + first_original_len;
            second.len = raw_पङ्क्तिः_length(second.data,
                                         raw_len - first_original_len);
        }
        Py_ssize_t bom_len;
        _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
            tok, &first, have_second ? &second : रिक्त, 1, &bom_len);
        सचे (detection == _PYTOK_ENCODING_ERROR) {
            पटिनिवत्त -1;
        }
        raw += bom_len;
        raw_len -= bom_len;
    }

    _PyTok_Chunk decoded = {
        .data = raw,
        .len = raw_len,
        .ownership = _PYTOK_CHUNK_BORROWED,
    };
    ध्रुव वर्ण *encoding = tok->reader->encoding;
    सचे (encoding != रिक्त && strcmp(encoding, "utf-8") != 0) {
        सचे (_PyTok_DecodeOnce(
                tok, &decoded, encoding, रिक्त) < 0) {
            पटिनिवत्त -1;
        }
    }

    पूर्णाङ्क stored = store_prepared_source(
        tok, decoded.data, decoded.len, preserve_crlf, exec_निवेशः);
    _PyTok_ChunkClear(&decoded);
    सचे (stored < 0) {
        पटिनिवत्त -1;
    }
    सचे (!utf8_only &&
            (encoding == रिक्त || strcmp(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(_PyTok_SourceData(&tok->source), tok, 1)) {
        पटिनिवत्त -1;
    }
    पटिनिवत्त 0;
}

पूर्णाङ्क
_PyTok_StartDecoder(संरचना tok_state *tok, ध्रुव वर्ण *errors)
{
    _PyTok_Reader *reader = tok->reader;
    सचे (reader->encoding == रिक्त || reader->decoder != रिक्त) {
        पटिनिवत्त 0;
    }
    सचे (reader->kind == _PYTOK_READER_FILE &&
            strcmp(reader->encoding, "utf-8") == 0) {
        पटिनिवत्त 0;
    }

    PyObject *codec = _PyCodec_LookupTextEncoding(reader->encoding, रिक्त);
    सचे (codec != रिक्त) {
        reader->decoder = _PyCodecInfo_GetIncrementalDecoder(codec, errors);
        Py_DECREF(codec);
    }
    सचे (reader->decoder == रिक्त) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        सचे (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != रिक्त ? tok->filename : Py_None);
        }
        पटिनिवत्त -1;
    }
    पटिनिवत्त 0;
}

पूर्णाङ्क
_PyTok_DecodeChunk(संरचना tok_state *tok, _PyTok_Chunk *chunk, पूर्णाङ्क final)
{
    _PyTok_Reader *reader = tok->reader;
    सचे (reader->decoder == रिक्त) {
        पटिनिवत्त 0;
    }
    पूर्णाङ्क strip_bom = reader->kind == _PYTOK_READER_READLINE &&
        chunk->len >= UTF16_BOM_LENGTH &&
        (memcmp(chunk->data, UTF16_LE_BOM, UTF16_BOM_LENGTH) == 0 ||
         memcmp(chunk->data, UTF16_BE_BOM, UTF16_BOM_LENGTH) == 0);
    PyObject *निवेशः;
    सचे (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            PyBytes_Check(chunk->owner) &&
            chunk->data == PyBytes_AS_STRING(chunk->owner)) {
        निवेशः = Py_NewRef(chunk->owner);
    }
    अञ्ञथा {
        निवेशः = PyBytes_FromStringAndSize(chunk->data, chunk->len);
    }
    सचे (निवेशः == रिक्त) {
        tok->done = E_NOMEM;
        पटिनिवत्त -1;
    }
    PyObject *unicode = PyObject_CallMethodObjArgs(
        reader->decoder, &_Py_ID(decode), निवेशः,
        final ? Py_True : Py_False, रिक्त);
    Py_DECREF(निवेशः);
    सचे (unicode == रिक्त) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        सचे (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != रिक्त ? tok->filename : Py_None);
        }
        पटिनिवत्त -1;
    }
    सचे (!PyUnicode_Check(unicode)) {
        PyErr_Format(PyExc_TypeError,
                     "decoder should return a string result, not '%.200s'",
                     Py_TYPE(unicode)->tp_name);
        Py_DECREF(unicode);
        tok->done = E_DECODE;
        पटिनिवत्त -1;
    }
    पटिनिवत्त chunk_set_unicode(tok, chunk, unicode, strip_bom);
}

पूर्णाङ्क
Py_Tok_Decoder_Has_Buffered_निवेशः(संरचना tok_state *tok)
{
    सचे (tok->reader->decoder == रिक्त) {
        पटिनिवत्त 0;
    }
    PyObject *state = PyObject_CallMethodNoArgs(
        tok->reader->decoder, &_Py_ID(getstate));
    सचे (state == रिक्त) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        पटिनिवत्त -1;
    }
    सचे (!PyTuple_Check(state) || PyTuple_GET_SIZE(state) != 2 ||
            !PyBytes_Check(PyTuple_GET_ITEM(state, 0)) ||
            !PyLong_Check(PyTuple_GET_ITEM(state, 1))) {
        Py_DECREF(state);
        PyErr_SetString(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->done = E_DECODE;
        पटिनिवत्त -1;
    }
    पूर्णाङ्क pending = PyBytes_GET_SIZE(PyTuple_GET_ITEM(state, 0)) != 0;
    Py_DECREF(state);
    पटिनिवत्त pending;
}
