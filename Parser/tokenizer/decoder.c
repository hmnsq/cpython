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
    यदि (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        प्रत्यागम रिक्त;
    }
    वर्ण *copy = PyMem_Malloc((size_t)len + 1);
    यदि (copy == रिक्त) {
        PyErr_NoMemory();
        प्रत्यागम रिक्त;
    }
    memcpy(copy, data, (size_t)len);
    copy[len] = '\0';
    प्रत्यागम copy;
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
    यदि (utf8 == रिक्त) {
        Py_DECREF(unicode);
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        प्रत्यागम -1;
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
    प्रत्यागम 0;
}

वर्ण *
_PyTok_NormalizeNewlines(ध्रुव वर्ण *data, Py_ssize_t len,
                         Py_ssize_t *out_len)
{
    यदि (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        प्रत्यागम रिक्त;
    }
    वर्ण *result = PyMem_Malloc((size_t)len + 1);
    यदि (result == रिक्त) {
        PyErr_NoMemory();
        प्रत्यागम रिक्त;
    }
    Py_ssize_t लिख = 0;
    प्रति (Py_ssize_t पठ = 0; पठ < len; पठ++) {
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
    प्रत्यागम result;
}

पूर्णाङ्क
_PyTok_SetEncoding(संरचना tok_state *tok, ध्रुव वर्ण *encoding)
{
    वर्ण *copy = _PyTok_CopyBytes(encoding, strlen(encoding));
    यदि (copy == रिक्त) {
        tok->done = E_NOMEM;
        प्रत्यागम -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    प्रत्यागम 0;
}

स्थिर पूर्णाङ्क
find_cookie(ध्रुव वर्ण *ओळ, Py_ssize_t len, वर्ण **encoding, पूर्णाङ्क *scan_next)
{
    Py_ssize_t i = 0;
    *encoding = रिक्त;
    *scan_next = 1;
    प्रति (; i < len; i++) {
        यदि (ओळ[i] == '#') {
            भङ्ग;
        }
        यदि (ओळ[i] == '\n' || ओळ[i] == '\r') {
            प्रत्यागम 0;
        }
        यदि (ओळ[i] != ' ' && ओळ[i] != '\t' && ओळ[i] != '\f') {
            *scan_next = 0;
            प्रत्यागम 0;
        }
    }
    ध्रुव Py_ssize_t marker_len = परिमाण("coding") - 1;
    प्रति (; i < len - marker_len; i++) {
        यदि (memcmp(ओळ + i, "coding", marker_len) != 0) {
            अनुवर्त;
        }
        ध्रुव वर्ण *cursor = ओळ + i + marker_len;
        यदि (*cursor != ':' && *cursor != '=') {
            अनुवर्त;
        }
        कुरु {
            cursor++;
        } यावत् (cursor < ओळ + len &&
                 (*cursor == ' ' || *cursor == '\t'));
        ध्रुव वर्ण *start = cursor;
        यावत् (cursor < ओळ + len &&
                (Py_ISALNUM(*cursor) || *cursor == '-' ||
                 *cursor == '_' || *cursor == '.')) {
            cursor++;
        }
        यदि (cursor == start) {
            अनुवर्त;
        }
        Py_ssize_t encoding_len = cursor - start;
        // Include the longest recognized prefix and its NUL terminator.
        वर्ण normalized[परिमाण("iso-latin-1-")];
        ध्रुव Py_ssize_t prefix_len = Py_MIN(
            encoding_len, (Py_ssize_t)परिमाण(normalized) - 1);
        Py_ssize_t n;
        प्रति (n = 0; n < prefix_len; n++) {
            normalized[n] = start[n] == '_' ? '-' : Py_TOLOWER(start[n]);
        }
        normalized[n] = '\0';
        ध्रुव वर्ण *canonical = start;
        यदि (strcmp(normalized, "utf-8") == 0 ||
                strncmp(normalized, "utf-8-", परिमाण("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        अन्यथा यदि (strcmp(normalized, "latin-1") == 0 ||
                 strcmp(normalized, "iso-8859-1") == 0 ||
                 strcmp(normalized, "iso-latin-1") == 0 ||
                 strncmp(normalized, "latin-1-", परिमाण("latin-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-8859-1-", परिमाण("iso-8859-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-latin-1-", परिमाण("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        यदि (canonical != start) {
            encoding_len = strlen(canonical);
        }
        *encoding = _PyTok_CopyBytes(canonical, encoding_len);
        यदि (*encoding == रिक्त) {
            प्रत्यागम -1;
        }
        *scan_next = 0;
        प्रत्यागम 0;
    }
    प्रत्यागम 0;
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
    पूर्णाङ्क cookie_ओळ = 1;
    ध्रुव वर्ण *first_data = first->data + *bom_len;
    Py_ssize_t first_len = first->len - *bom_len;
    यदि (find_cookie(first_data, first_len, &cookie, &scan_next) < 0) {
        प्रत्यागम _PYTOK_ENCODING_ERROR;
    }
    यदि (cookie == रिक्त && scan_next && second != रिक्त) {
        यदि (find_cookie(second->data, second->len, &cookie, &scan_next) < 0) {
            प्रत्यागम _PYTOK_ENCODING_ERROR;
        }
        cookie_ओळ = 2;
    }
    अन्यथा यदि (cookie == रिक्त && scan_next && !final) {
        प्रत्यागम PYTOK_ENCODING_NEED_SECOND_ओळ;
    }

    यदि (cookie == रिक्त) {
        यदि (bom && _PyTok_SetEncoding(tok, "utf-8") < 0) {
            प्रत्यागम _PYTOK_ENCODING_ERROR;
        }
        प्रत्यागम _PYTOK_ENCODING_DONE;
    }
    यदि (bom && strcmp(cookie, "utf-8") != 0) {
        ध्रुव _PyTok_Chunk *ओळ = cookie_ओळ == 2 ? second : first;
        ध्रुव वर्ण *ओळ_data = ओळ->data + (cookie_ओळ == 1 ? *bom_len : 0);
        Py_ssize_t ओळ_len = ओळ->len - (cookie_ओळ == 1 ? *bom_len : 0);
        पूर्णाङ्क end_col = (पूर्णाङ्क)Py_MIN(ओळ_len, INT_MAX);
        यदि (end_col > 0 && (ओळ_data[end_col - 1] == '\n' ||
                            ओळ_data[end_col - 1] == '\r')) {
            end_col--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, ओळ_data, 0, cookie_ओळ, 0, end_col,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        प्रत्यागम _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    प्रत्यागम _PYTOK_ENCODING_DONE;
}

पूर्णाङ्क
_PyTok_DecodeOnce(संरचना tok_state *tok, _PyTok_Chunk *chunk,
                  ध्रुव वर्ण *encoding, ध्रुव वर्ण *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->data, chunk->len, encoding, errors);
    यदि (unicode == रिक्त) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        प्रत्यागम -1;
    }
    प्रत्यागम chunk_set_unicode(tok, chunk, unicode, 0);
}

स्थिर Py_ssize_t
raw_ओळ_length(ध्रुव वर्ण *data, Py_ssize_t len)
{
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        यदि (data[i] == '\n') {
            प्रत्यागम i + 1;
        }
        यदि (data[i] == '\r') {
            प्रत्यागम i + 1 < len && data[i + 1] == '\n' ? i + 2 : i + 1;
        }
    }
    प्रत्यागम len;
}

स्थिर पूर्णाङ्क
store_prepared_source(संरचना tok_state *tok, ध्रुव वर्ण *data, Py_ssize_t len,
                      पूर्णाङ्क preserve_crlf, पूर्णाङ्क add_final_newline)
{
    Py_ssize_t pos = 0;
    यावत् (pos < len) {
        Py_ssize_t raw_ओळ_len;
        यदि (preserve_crlf) {
            ध्रुव वर्ण *newline = memchr(data + pos, '\n', len - pos);
            raw_ओळ_len = newline == रिक्त
                ? len - pos : newline - data - pos + 1;
        }
        अन्यथा {
            raw_ओळ_len = raw_ओळ_length(data + pos, len - pos);
        }
        पूर्णाङ्क terminated = preserve_crlf
            ? data[pos + raw_ओळ_len - 1] == '\n'
            : data[pos + raw_ओळ_len - 1] == '\n' ||
              data[pos + raw_ओळ_len - 1] == '\r';
        पूर्णाङ्क add_newline = add_final_newline &&
            pos + raw_ओळ_len == len && !terminated;
        ध्रुव वर्ण *ओळ = data + pos;
        // raw_line_length stops at the first CR or LF, so any CR is in
        // the line terminator.
        Py_ssize_t terminator_len = 0;
        यदि (!preserve_crlf) {
            यदि (ओळ[raw_ओळ_len - 1] == '\r') {
                terminator_len = 1;
            }
            अन्यथा यदि (raw_ओळ_len > 1 && ओळ[raw_ओळ_len - 2] == '\r') {
                terminator_len = 2;
            }
        }
        _PyTok_Off appended;
        यदि (terminator_len || add_newline) {
            appended = Py_Tok_Source_Append_ओळ_With_Newline(
                &tok->source, ओळ, raw_ओळ_len - terminator_len);
        }
        अन्यथा {
            appended = Py_Tok_Source_Append_ओळ(&tok->source, ओळ, raw_ओळ_len);
        }
        यदि (appended < 0) {
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            प्रत्यागम -1;
        }
        tok->reader->prepared_final_newline = add_newline;
        pos += raw_ओळ_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    यदि (add_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || data[len - 1] != '\r'))
    {
        यदि (Py_Tok_Source_Append_ओळ(&tok->source, "\n", 1) < 0) {
            प्रत्यागम -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    प्रत्यागम 0;
}

पूर्णाङ्क
_PyTok_PrepareString(संरचना tok_state *tok, ध्रुव वर्ण *आदान, पूर्णाङ्क utf8_only,
                     पूर्णाङ्क exec_आदान, पूर्णाङ्क preserve_crlf)
{
    Py_ssize_t raw_len = strlen(आदान);
    वर्ण *raw = (वर्ण *)आदान;

    यदि (utf8_only) {
        यदि (_PyTok_SetEncoding(tok, "utf-8") < 0) {
            प्रत्यागम -1;
        }
    }
    अन्यथा {
        Py_ssize_t first_original_len = raw_ओळ_length(raw, raw_len);
        _PyTok_Chunk first = {
            .data = raw,
            .len = first_original_len,
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        _PyTok_Chunk second = {0};
        पूर्णाङ्क have_second = first_original_len < raw_len;
        यदि (have_second) {
            second.data = raw + first_original_len;
            second.len = raw_ओळ_length(second.data,
                                         raw_len - first_original_len);
        }
        Py_ssize_t bom_len;
        _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
            tok, &first, have_second ? &second : रिक्त, 1, &bom_len);
        यदि (detection == _PYTOK_ENCODING_ERROR) {
            प्रत्यागम -1;
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
    यदि (encoding != रिक्त && strcmp(encoding, "utf-8") != 0) {
        यदि (_PyTok_DecodeOnce(
                tok, &decoded, encoding, रिक्त) < 0) {
            प्रत्यागम -1;
        }
    }

    पूर्णाङ्क stored = store_prepared_source(
        tok, decoded.data, decoded.len, preserve_crlf, exec_आदान);
    _PyTok_ChunkClear(&decoded);
    यदि (stored < 0) {
        प्रत्यागम -1;
    }
    यदि (!utf8_only &&
            (encoding == रिक्त || strcmp(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(_PyTok_SourceData(&tok->source), tok, 1)) {
        प्रत्यागम -1;
    }
    प्रत्यागम 0;
}

पूर्णाङ्क
_PyTok_StartDecoder(संरचना tok_state *tok, ध्रुव वर्ण *errors)
{
    _PyTok_Reader *reader = tok->reader;
    यदि (reader->encoding == रिक्त || reader->decoder != रिक्त) {
        प्रत्यागम 0;
    }
    यदि (reader->kind == _PYTOK_READER_FILE &&
            strcmp(reader->encoding, "utf-8") == 0) {
        प्रत्यागम 0;
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
        प्रत्यागम -1;
    }
    प्रत्यागम 0;
}

पूर्णाङ्क
_PyTok_DecodeChunk(संरचना tok_state *tok, _PyTok_Chunk *chunk, पूर्णाङ्क final)
{
    _PyTok_Reader *reader = tok->reader;
    यदि (reader->decoder == रिक्त) {
        प्रत्यागम 0;
    }
    पूर्णाङ्क strip_bom = reader->kind == _PYTOK_READER_READLINE &&
        chunk->len >= UTF16_BOM_LENGTH &&
        (memcmp(chunk->data, UTF16_LE_BOM, UTF16_BOM_LENGTH) == 0 ||
         memcmp(chunk->data, UTF16_BE_BOM, UTF16_BOM_LENGTH) == 0);
    PyObject *आदान;
    यदि (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            PyBytes_Check(chunk->owner) &&
            chunk->data == PyBytes_AS_STRING(chunk->owner)) {
        आदान = Py_NewRef(chunk->owner);
    }
    अन्यथा {
        आदान = PyBytes_FromStringAndSize(chunk->data, chunk->len);
    }
    यदि (आदान == रिक्त) {
        tok->done = E_NOMEM;
        प्रत्यागम -1;
    }
    PyObject *unicode = PyObject_CallMethodObjArgs(
        reader->decoder, &_Py_ID(decode), आदान,
        final ? Py_True : Py_False, रिक्त);
    Py_DECREF(आदान);
    यदि (unicode == रिक्त) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        यदि (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != रिक्त ? tok->filename : Py_None);
        }
        प्रत्यागम -1;
    }
    यदि (!PyUnicode_Check(unicode)) {
        PyErr_Format(PyExc_TypeError,
                     "decoder should return a string result, not '%.200s'",
                     Py_TYPE(unicode)->tp_name);
        Py_DECREF(unicode);
        tok->done = E_DECODE;
        प्रत्यागम -1;
    }
    प्रत्यागम chunk_set_unicode(tok, chunk, unicode, strip_bom);
}

पूर्णाङ्क
Py_Tok_Decoder_Has_Buffered_आदान(संरचना tok_state *tok)
{
    यदि (tok->reader->decoder == रिक्त) {
        प्रत्यागम 0;
    }
    PyObject *state = PyObject_CallMethodNoArgs(
        tok->reader->decoder, &_Py_ID(getstate));
    यदि (state == रिक्त) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        प्रत्यागम -1;
    }
    यदि (!PyTuple_Check(state) || PyTuple_GET_SIZE(state) != 2 ||
            !PyBytes_Check(PyTuple_GET_ITEM(state, 0)) ||
            !PyLong_Check(PyTuple_GET_ITEM(state, 1))) {
        Py_DECREF(state);
        PyErr_SetString(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->done = E_DECODE;
        प्रत्यागम -1;
    }
    पूर्णाङ्क pending = PyBytes_GET_SIZE(PyTuple_GET_ITEM(state, 0)) != 0;
    Py_DECREF(state);
    प्रत्यागम pending;
}
