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
_PyTok_CopyBytes(अचर वर्ण *data, Py_ssize_t len)
{
    यदि (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        फर्काऊ रिक्त;
    }
    वर्ण *copy = PyMem_Malloc((माप_प्रकार)len + 1);
    यदि (copy == रिक्त) {
        PyErr_NoMemory();
        फर्काऊ रिक्त;
    }
    स_नकल(copy, data, (माप_प्रकार)len);
    copy[len] = '\0';
    फर्काऊ copy;
}

स्थिर शून्य
chunk_release_data(_PyTok_Chunk *chunk)
{
    विकल्प (chunk->ownership) {
        पक्ष _PYTOK_CHUNK_BORROWED:
            रोक;
        पक्ष _PYTOK_CHUNK_PYMEM:
            PyMem_Free(chunk->data);
            रोक;
        पक्ष _PYTOK_CHUNK_PYOBJECT:
            Py_DECREF(chunk->owner);
            रोक;
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
    अचर वर्ण *utf8 = PyUnicode_AsUTF8AndSize(unicode, &utf8_len);
    यदि (utf8 == रिक्त) {
        Py_DECREF(unicode);
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        फर्काऊ -1;
    }
    यदि (strip_bom && PyUnicode_GET_LENGTH(unicode) > 0 &&
            PyUnicode_ReadChar(unicode, 0) == UNICODE_BOM) {
        utf8 += UTF8_BOM_LENGTH;
        utf8_len -= UTF8_BOM_LENGTH;
    }
    chunk_release_data(chunk);
    chunk->owner = unicode;
    chunk->data = (वर्ण *)utf8;
    chunk->len = utf8_len;
    chunk->ownership = _PYTOK_CHUNK_PYOBJECT;
    फर्काऊ 0;
}

वर्ण *
_PyTok_NormalizeNewlines(अचर वर्ण *data, Py_ssize_t len,
                         Py_ssize_t *out_len)
{
    यदि (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        फर्काऊ रिक्त;
    }
    वर्ण *result = PyMem_Malloc((माप_प्रकार)len + 1);
    यदि (result == रिक्त) {
        PyErr_NoMemory();
        फर्काऊ रिक्त;
    }
    Py_ssize_t लिख = 0;
    लागि (Py_ssize_t पठ = 0; पठ < len; पठ++) {
        वर्ण c = data[पठ];
        यदि (c == '\r') {
            यदि (पठ + 1 < len && data[पठ + 1] == '\n') {
                पठ++;
            }
            c = '\n';
        }
        result[लिख++] = c;
    }
    result[लिख] = '\0';
    *out_len = लिख;
    फर्काऊ result;
}

पूर्णाङ्क
_PyTok_SetEncoding(संरचना tok_state *tok, अचर वर्ण *encoding)
{
    वर्ण *copy = _PyTok_CopyBytes(encoding, म_माप(encoding));
    यदि (copy == रिक्त) {
        tok->done = E_NOMEM;
        फर्काऊ -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    फर्काऊ 0;
}

स्थिर पूर्णाङ्क
find_cookie(अचर वर्ण *पङ्क्ति, Py_ssize_t len, वर्ण **encoding, पूर्णाङ्क *scan_next)
{
    Py_ssize_t i = 0;
    *encoding = रिक्त;
    *scan_next = 1;
    लागि (; i < len; i++) {
        यदि (पङ्क्ति[i] == '#') {
            रोक;
        }
        यदि (पङ्क्ति[i] == '\n' || पङ्क्ति[i] == '\r') {
            फर्काऊ 0;
        }
        यदि (पङ्क्ति[i] != ' ' && पङ्क्ति[i] != '\t' && पङ्क्ति[i] != '\f') {
            *scan_next = 0;
            फर्काऊ 0;
        }
    }
    अचर Py_ssize_t marker_len = परिमाण("coding") - 1;
    लागि (; i < len - marker_len; i++) {
        यदि (स_भेद(पङ्क्ति + i, "coding", marker_len) != 0) {
            जारी;
        }
        अचर वर्ण *cursor = पङ्क्ति + i + marker_len;
        यदि (*cursor != ':' && *cursor != '=') {
            जारी;
        }
        गर {
            cursor++;
        } जबसम्म (cursor < पङ्क्ति + len &&
                 (*cursor == ' ' || *cursor == '\t'));
        अचर वर्ण *start = cursor;
        जबसम्म (cursor < पङ्क्ति + len &&
                (Py_ISALNUM(*cursor) || *cursor == '-' ||
                 *cursor == '_' || *cursor == '.')) {
            cursor++;
        }
        यदि (cursor == start) {
            जारी;
        }
        Py_ssize_t encoding_len = cursor - start;
        // Include the longest recognized prefix and its NUL terminator.
        वर्ण normalized[परिमाण("iso-latin-1-")];
        अचर Py_ssize_t prefix_len = Py_MIN(
            encoding_len, (Py_ssize_t)परिमाण(normalized) - 1);
        Py_ssize_t n;
        लागि (n = 0; n < prefix_len; n++) {
            normalized[n] = start[n] == '_' ? '-' : Py_TOLOWER(start[n]);
        }
        normalized[n] = '\0';
        अचर वर्ण *canonical = start;
        यदि (म_भेद(normalized, "utf-8") == 0 ||
                म_भेदन(normalized, "utf-8-", परिमाण("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        अन्यथा यदि (म_भेद(normalized, "latin-1") == 0 ||
                 म_भेद(normalized, "iso-8859-1") == 0 ||
                 म_भेद(normalized, "iso-latin-1") == 0 ||
                 म_भेदन(normalized, "latin-1-", परिमाण("latin-1-") - 1) == 0 ||
                 म_भेदन(normalized, "iso-8859-1-", परिमाण("iso-8859-1-") - 1) == 0 ||
                 म_भेदन(normalized, "iso-latin-1-", परिमाण("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        यदि (canonical != start) {
            encoding_len = म_माप(canonical);
        }
        *encoding = _PyTok_CopyBytes(canonical, encoding_len);
        यदि (*encoding == रिक्त) {
            फर्काऊ -1;
        }
        *scan_next = 0;
        फर्काऊ 0;
    }
    फर्काऊ 0;
}

_PyTok_EncodingResult
_PyTok_DetectEncoding(संरचना tok_state *tok, अचर _PyTok_Chunk *first,
                      अचर _PyTok_Chunk *second, पूर्णाङ्क final,
                      Py_ssize_t *bom_len)
{
    पूर्णाङ्क bom = first->len >= UTF8_BOM_LENGTH &&
        स_भेद(first->data, UTF8_BOM, UTF8_BOM_LENGTH) == 0;
    *bom_len = bom ? UTF8_BOM_LENGTH : 0;

    वर्ण *cookie = रिक्त;
    पूर्णाङ्क scan_next = 0;
    पूर्णाङ्क cookie_पङ्क्ति = 1;
    अचर वर्ण *first_data = first->data + *bom_len;
    Py_ssize_t first_len = first->len - *bom_len;
    यदि (find_cookie(first_data, first_len, &cookie, &scan_next) < 0) {
        फर्काऊ _PYTOK_ENCODING_ERROR;
    }
    यदि (cookie == रिक्त && scan_next && second != रिक्त) {
        यदि (find_cookie(second->data, second->len, &cookie, &scan_next) < 0) {
            फर्काऊ _PYTOK_ENCODING_ERROR;
        }
        cookie_पङ्क्ति = 2;
    }
    अन्यथा यदि (cookie == रिक्त && scan_next && !final) {
        फर्काऊ PYTOK_ENCODING_NEED_SECOND_पङ्क्ति;
    }

    यदि (cookie == रिक्त) {
        यदि (bom && _PyTok_SetEncoding(tok, "utf-8") < 0) {
            फर्काऊ _PYTOK_ENCODING_ERROR;
        }
        फर्काऊ _PYTOK_ENCODING_DONE;
    }
    यदि (bom && म_भेद(cookie, "utf-8") != 0) {
        अचर _PyTok_Chunk *पङ्क्ति = cookie_पङ्क्ति == 2 ? second : first;
        अचर वर्ण *पङ्क्ति_data = पङ्क्ति->data + (cookie_पङ्क्ति == 1 ? *bom_len : 0);
        Py_ssize_t पङ्क्ति_len = पङ्क्ति->len - (cookie_पङ्क्ति == 1 ? *bom_len : 0);
        पूर्णाङ्क end_col = (पूर्णाङ्क)Py_MIN(पङ्क्ति_len, पूर्णांक_उच्च);
        यदि (end_col > 0 && (पङ्क्ति_data[end_col - 1] == '\n' ||
                            पङ्क्ति_data[end_col - 1] == '\r')) {
            end_col--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, पङ्क्ति_data, 0, cookie_पङ्क्ति, 0, end_col,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        फर्काऊ _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    फर्काऊ _PYTOK_ENCODING_DONE;
}

पूर्णाङ्क
_PyTok_DecodeOnce(संरचना tok_state *tok, _PyTok_Chunk *chunk,
                  अचर वर्ण *encoding, अचर वर्ण *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->data, chunk->len, encoding, errors);
    यदि (unicode == रिक्त) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        फर्काऊ -1;
    }
    फर्काऊ chunk_set_unicode(tok, chunk, unicode, 0);
}

स्थिर Py_ssize_t
raw_पङ्क्ति_length(अचर वर्ण *data, Py_ssize_t len)
{
    लागि (Py_ssize_t i = 0; i < len; i++) {
        यदि (data[i] == '\n') {
            फर्काऊ i + 1;
        }
        यदि (data[i] == '\r') {
            फर्काऊ i + 1 < len && data[i + 1] == '\n' ? i + 2 : i + 1;
        }
    }
    फर्काऊ len;
}

स्थिर पूर्णाङ्क
store_prepared_source(संरचना tok_state *tok, अचर वर्ण *data, Py_ssize_t len,
                      पूर्णाङ्क preserve_crlf, पूर्णाङ्क add_final_newline)
{
    Py_ssize_t pos = 0;
    जबसम्म (pos < len) {
        Py_ssize_t raw_पङ्क्ति_len;
        यदि (preserve_crlf) {
            अचर वर्ण *newline = स_प्रथम(data + pos, '\n', len - pos);
            raw_पङ्क्ति_len = newline == रिक्त
                ? len - pos : newline - data - pos + 1;
        }
        अन्यथा {
            raw_पङ्क्ति_len = raw_पङ्क्ति_length(data + pos, len - pos);
        }
        पूर्णाङ्क terminated = preserve_crlf
            ? data[pos + raw_पङ्क्ति_len - 1] == '\n'
            : data[pos + raw_पङ्क्ति_len - 1] == '\n' ||
              data[pos + raw_पङ्क्ति_len - 1] == '\r';
        पूर्णाङ्क add_newline = add_final_newline &&
            pos + raw_पङ्क्ति_len == len && !terminated;
        अचर वर्ण *पङ्क्ति = data + pos;
        // raw_line_length stops at the first CR or LF, so any CR is in
        // the line terminator.
        Py_ssize_t terminator_len = 0;
        यदि (!preserve_crlf) {
            यदि (पङ्क्ति[raw_पङ्क्ति_len - 1] == '\r') {
                terminator_len = 1;
            }
            अन्यथा यदि (raw_पङ्क्ति_len > 1 && पङ्क्ति[raw_पङ्क्ति_len - 2] == '\r') {
                terminator_len = 2;
            }
        }
        _PyTok_Off appended;
        यदि (terminator_len || add_newline) {
            appended = Py_Tok_Source_Append_पङ्क्ति_With_Newline(
                &tok->source, पङ्क्ति, raw_पङ्क्ति_len - terminator_len);
        }
        अन्यथा {
            appended = Py_Tok_Source_Append_पङ्क्ति(&tok->source, पङ्क्ति, raw_पङ्क्ति_len);
        }
        यदि (appended < 0) {
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            फर्काऊ -1;
        }
        tok->reader->prepared_final_newline = add_newline;
        pos += raw_पङ्क्ति_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    यदि (add_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || data[len - 1] != '\r'))
    {
        यदि (Py_Tok_Source_Append_पङ्क्ति(&tok->source, "\n", 1) < 0) {
            फर्काऊ -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    फर्काऊ 0;
}

पूर्णाङ्क
_PyTok_PrepareString(संरचना tok_state *tok, अचर वर्ण *आगत, पूर्णाङ्क utf8_only,
                     पूर्णाङ्क exec_आगत, पूर्णाङ्क preserve_crlf)
{
    Py_ssize_t raw_len = म_माप(आगत);
    वर्ण *raw = (वर्ण *)आगत;

    यदि (utf8_only) {
        यदि (_PyTok_SetEncoding(tok, "utf-8") < 0) {
            फर्काऊ -1;
        }
    }
    अन्यथा {
        Py_ssize_t first_original_len = raw_पङ्क्ति_length(raw, raw_len);
        _PyTok_Chunk first = {
            .data = raw,
            .len = first_original_len,
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        _PyTok_Chunk second = {0};
        पूर्णाङ्क have_second = first_original_len < raw_len;
        यदि (have_second) {
            second.data = raw + first_original_len;
            second.len = raw_पङ्क्ति_length(second.data,
                                         raw_len - first_original_len);
        }
        Py_ssize_t bom_len;
        _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
            tok, &first, have_second ? &second : रिक्त, 1, &bom_len);
        यदि (detection == _PYTOK_ENCODING_ERROR) {
            फर्काऊ -1;
        }
        raw += bom_len;
        raw_len -= bom_len;
    }

    _PyTok_Chunk decoded = {
        .data = raw,
        .len = raw_len,
        .ownership = _PYTOK_CHUNK_BORROWED,
    };
    अचर वर्ण *encoding = tok->reader->encoding;
    यदि (encoding != रिक्त && म_भेद(encoding, "utf-8") != 0) {
        यदि (_PyTok_DecodeOnce(
                tok, &decoded, encoding, रिक्त) < 0) {
            फर्काऊ -1;
        }
    }

    पूर्णाङ्क stored = store_prepared_source(
        tok, decoded.data, decoded.len, preserve_crlf, exec_आगत);
    _PyTok_ChunkClear(&decoded);
    यदि (stored < 0) {
        फर्काऊ -1;
    }
    यदि (!utf8_only &&
            (encoding == रिक्त || म_भेद(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(_PyTok_SourceData(&tok->source), tok, 1)) {
        फर्काऊ -1;
    }
    फर्काऊ 0;
}

पूर्णाङ्क
_PyTok_StartDecoder(संरचना tok_state *tok, अचर वर्ण *errors)
{
    _PyTok_Reader *reader = tok->reader;
    यदि (reader->encoding == रिक्त || reader->decoder != रिक्त) {
        फर्काऊ 0;
    }
    यदि (reader->kind == _PYTOK_READER_FILE &&
            म_भेद(reader->encoding, "utf-8") == 0) {
        फर्काऊ 0;
    }

    PyObject *codec = _PyCodec_LookupTextEncoding(reader->encoding, रिक्त);
    यदि (codec != रिक्त) {
        reader->decoder = _PyCodecInfo_GetIncrementalDecoder(codec, errors);
        Py_DECREF(codec);
    }
    यदि (reader->decoder == रिक्त) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        यदि (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != रिक्त ? tok->filename : Py_None);
        }
        फर्काऊ -1;
    }
    फर्काऊ 0;
}

पूर्णाङ्क
_PyTok_DecodeChunk(संरचना tok_state *tok, _PyTok_Chunk *chunk, पूर्णाङ्क final)
{
    _PyTok_Reader *reader = tok->reader;
    यदि (reader->decoder == रिक्त) {
        फर्काऊ 0;
    }
    पूर्णाङ्क strip_bom = reader->kind == _PYTOK_READER_READLINE &&
        chunk->len >= UTF16_BOM_LENGTH &&
        (स_भेद(chunk->data, UTF16_LE_BOM, UTF16_BOM_LENGTH) == 0 ||
         स_भेद(chunk->data, UTF16_BE_BOM, UTF16_BOM_LENGTH) == 0);
    PyObject *आगत;
    यदि (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            PyBytes_Check(chunk->owner) &&
            chunk->data == PyBytes_AS_STRING(chunk->owner)) {
        आगत = Py_NewRef(chunk->owner);
    }
    अन्यथा {
        आगत = PyBytes_FromStringAndSize(chunk->data, chunk->len);
    }
    यदि (आगत == रिक्त) {
        tok->done = E_NOMEM;
        फर्काऊ -1;
    }
    PyObject *unicode = PyObject_CallMethodObjArgs(
        reader->decoder, &_Py_ID(decode), आगत,
        final ? Py_True : Py_False, रिक्त);
    Py_DECREF(आगत);
    यदि (unicode == रिक्त) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        यदि (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != रिक्त ? tok->filename : Py_None);
        }
        फर्काऊ -1;
    }
    यदि (!PyUnicode_Check(unicode)) {
        PyErr_Format(PyExc_TypeError,
                     "decoder should return a string result, not '%.200s'",
                     Py_TYPE(unicode)->tp_name);
        Py_DECREF(unicode);
        tok->done = E_DECODE;
        फर्काऊ -1;
    }
    फर्काऊ chunk_set_unicode(tok, chunk, unicode, strip_bom);
}

पूर्णाङ्क
Py_Tok_Decoder_Has_Buffered_आगत(संरचना tok_state *tok)
{
    यदि (tok->reader->decoder == रिक्त) {
        फर्काऊ 0;
    }
    PyObject *state = PyObject_CallMethodNoArgs(
        tok->reader->decoder, &_Py_ID(getstate));
    यदि (state == रिक्त) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        फर्काऊ -1;
    }
    यदि (!PyTuple_Check(state) || PyTuple_GET_SIZE(state) != 2 ||
            !PyBytes_Check(PyTuple_GET_ITEM(state, 0)) ||
            !PyLong_Check(PyTuple_GET_ITEM(state, 1))) {
        Py_DECREF(state);
        PyErr_SetString(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->done = E_DECODE;
        फर्काऊ -1;
    }
    पूर्णाङ्क pending = PyBytes_GET_SIZE(PyTuple_GET_ITEM(state, 0)) != 0;
    Py_DECREF(state);
    फर्काऊ pending;
}
