#অন্তর্ভাৱ "Python.h"
#অন্তর্ভাৱ "pycore_codecs.h"
#অন্তর্ভাৱ "pycore_global_strings.h"
#অন্তর্ভাৱ "pycore_runtime.h"
#অন্তর্ভাৱ "errcode.h"

#অন্তর্ভাৱ "reader_internal.h"
#অন্তর্ভাৱ "helpers.h"
#অন্তর্ভাৱ "../lexer/state.h"

#define UNICODE_BOM 0xFEFF
#define UTF16_LE_BOM "\xff\xfe"
#define UTF16_BE_BOM "\xfe\xff"
#define UTF16_BOM_LENGTH ((Py_ssize_t)(পরিমাণ(UTF16_LE_BOM) - 1))
#define UTF8_BOM "\xef\xbb\xbf"
#define UTF8_BOM_LENGTH ((Py_ssize_t)(পরিমাণ(UTF8_BOM) - 1))

ৱর্ণ *
_PyTok_CopyBytes(ধ্রুৱ ৱর্ণ *data, Py_ssize_t len)
{
    যদি (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        ঘূৰাই রিক্ত;
    }
    ৱর্ণ *copy = PyMem_Malloc((size_t)len + 1);
    যদি (copy == রিক্ত) {
        PyErr_NoMemory();
        ঘূৰাই রিক্ত;
    }
    memcpy(copy, data, (size_t)len);
    copy[len] = '\0';
    ঘূৰাই copy;
}

স্থির শূন্য
chunk_release_data(_PyTok_Chunk *chunk)
{
    ৱিকল্প (chunk->ownership) {
        পক্ষ _PYTOK_CHUNK_BORROWED:
            ভঙ্গ;
        পক্ষ _PYTOK_CHUNK_PYMEM:
            PyMem_Free(chunk->data);
            ভঙ্গ;
        পক্ষ _PYTOK_CHUNK_PYOBJECT:
            Py_DECREF(chunk->owner);
            ভঙ্গ;
    }
}

শূন্য
_PyTok_ChunkClear(_PyTok_Chunk *chunk)
{
    chunk_release_data(chunk);
    *chunk = (_PyTok_Chunk){0};
}

স্থির পূর্ণাঙ্ক
chunk_set_unicode(সংরচনা tok_state *tok, _PyTok_Chunk *chunk,
                  PyObject *unicode, পূর্ণাঙ্ক strip_bom)
{
    Py_ssize_t utf8_len;
    ধ্রুৱ ৱর্ণ *utf8 = PyUnicode_AsUTF8AndSize(unicode, &utf8_len);
    যদি (utf8 == রিক্ত) {
        Py_DECREF(unicode);
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        ঘূৰাই -1;
    }
    যদি (strip_bom && PyUnicode_GET_LENGTH(unicode) > 0 &&
            PyUnicode_ReadChar(unicode, 0) == UNICODE_BOM) {
        utf8 += UTF8_BOM_LENGTH;
        utf8_len -= UTF8_BOM_LENGTH;
    }
    chunk_release_data(chunk);
    chunk->owner = unicode;
    chunk->data = (ৱর্ণ *)utf8;
    chunk->len = utf8_len;
    chunk->ownership = _PYTOK_CHUNK_PYOBJECT;
    ঘূৰাই 0;
}

ৱর্ণ *
_PyTok_NormalizeNewlines(ধ্রুৱ ৱর্ণ *data, Py_ssize_t len,
                         Py_ssize_t *out_len)
{
    যদি (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        ঘূৰাই রিক্ত;
    }
    ৱর্ণ *result = PyMem_Malloc((size_t)len + 1);
    যদি (result == রিক্ত) {
        PyErr_NoMemory();
        ঘূৰাই রিক্ত;
    }
    Py_ssize_t লিখ = 0;
    বাবে (Py_ssize_t পঠ = 0; পঠ < len; পঠ++) {
        ৱর্ণ c = data[পঠ];
        যদি (c == '\r') {
            যদি (পঠ + 1 < len && data[পঠ + 1] == '\n') {
                পঠ++;
            }
            c = '\n';
        }
        result[লিখ++] = c;
    }
    result[লিখ] = '\0';
    *out_len = লিখ;
    ঘূৰাই result;
}

পূর্ণাঙ্ক
_PyTok_SetEncoding(সংরচনা tok_state *tok, ধ্রুৱ ৱর্ণ *encoding)
{
    ৱর্ণ *copy = _PyTok_CopyBytes(encoding, strlen(encoding));
    যদি (copy == রিক্ত) {
        tok->done = E_NOMEM;
        ঘূৰাই -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক
find_cookie(ধ্রুৱ ৱর্ণ *সাৰি, Py_ssize_t len, ৱর্ণ **encoding, পূর্ণাঙ্ক *scan_next)
{
    Py_ssize_t i = 0;
    *encoding = রিক্ত;
    *scan_next = 1;
    বাবে (; i < len; i++) {
        যদি (সাৰি[i] == '#') {
            ভঙ্গ;
        }
        যদি (সাৰি[i] == '\n' || সাৰি[i] == '\r') {
            ঘূৰাই 0;
        }
        যদি (সাৰি[i] != ' ' && সাৰি[i] != '\t' && সাৰি[i] != '\f') {
            *scan_next = 0;
            ঘূৰাই 0;
        }
    }
    ধ্রুৱ Py_ssize_t marker_len = পরিমাণ("coding") - 1;
    বাবে (; i < len - marker_len; i++) {
        যদি (memcmp(সাৰি + i, "coding", marker_len) != 0) {
            অনুৱর্ত;
        }
        ধ্রুৱ ৱর্ণ *cursor = সাৰি + i + marker_len;
        যদি (*cursor != ':' && *cursor != '=') {
            অনুৱর্ত;
        }
        কৰা {
            cursor++;
        } যেতিয়ালৈ (cursor < সাৰি + len &&
                 (*cursor == ' ' || *cursor == '\t'));
        ধ্রুৱ ৱর্ণ *start = cursor;
        যেতিয়ালৈ (cursor < সাৰি + len &&
                (Py_ISALNUM(*cursor) || *cursor == '-' ||
                 *cursor == '_' || *cursor == '.')) {
            cursor++;
        }
        যদি (cursor == start) {
            অনুৱর্ত;
        }
        Py_ssize_t encoding_len = cursor - start;
        // Include the longest recognized prefix and its NUL terminator.
        ৱর্ণ normalized[পরিমাণ("iso-latin-1-")];
        ধ্রুৱ Py_ssize_t prefix_len = Py_MIN(
            encoding_len, (Py_ssize_t)পরিমাণ(normalized) - 1);
        Py_ssize_t n;
        বাবে (n = 0; n < prefix_len; n++) {
            normalized[n] = start[n] == '_' ? '-' : Py_TOLOWER(start[n]);
        }
        normalized[n] = '\0';
        ধ্রুৱ ৱর্ণ *canonical = start;
        যদি (strcmp(normalized, "utf-8") == 0 ||
                strncmp(normalized, "utf-8-", পরিমাণ("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        অন্যথা যদি (strcmp(normalized, "latin-1") == 0 ||
                 strcmp(normalized, "iso-8859-1") == 0 ||
                 strcmp(normalized, "iso-latin-1") == 0 ||
                 strncmp(normalized, "latin-1-", পরিমাণ("latin-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-8859-1-", পরিমাণ("iso-8859-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-latin-1-", পরিমাণ("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        যদি (canonical != start) {
            encoding_len = strlen(canonical);
        }
        *encoding = _PyTok_CopyBytes(canonical, encoding_len);
        যদি (*encoding == রিক্ত) {
            ঘূৰাই -1;
        }
        *scan_next = 0;
        ঘূৰাই 0;
    }
    ঘূৰাই 0;
}

_PyTok_EncodingResult
_PyTok_DetectEncoding(সংরচনা tok_state *tok, ধ্রুৱ _PyTok_Chunk *first,
                      ধ্রুৱ _PyTok_Chunk *second, পূর্ণাঙ্ক final,
                      Py_ssize_t *bom_len)
{
    পূর্ণাঙ্ক bom = first->len >= UTF8_BOM_LENGTH &&
        memcmp(first->data, UTF8_BOM, UTF8_BOM_LENGTH) == 0;
    *bom_len = bom ? UTF8_BOM_LENGTH : 0;

    ৱর্ণ *cookie = রিক্ত;
    পূর্ণাঙ্ক scan_next = 0;
    পূর্ণাঙ্ক cookie_সাৰি = 1;
    ধ্রুৱ ৱর্ণ *first_data = first->data + *bom_len;
    Py_ssize_t first_len = first->len - *bom_len;
    যদি (find_cookie(first_data, first_len, &cookie, &scan_next) < 0) {
        ঘূৰাই _PYTOK_ENCODING_ERROR;
    }
    যদি (cookie == রিক্ত && scan_next && second != রিক্ত) {
        যদি (find_cookie(second->data, second->len, &cookie, &scan_next) < 0) {
            ঘূৰাই _PYTOK_ENCODING_ERROR;
        }
        cookie_সাৰি = 2;
    }
    অন্যথা যদি (cookie == রিক্ত && scan_next && !final) {
        ঘূৰাই PYTOK_ENCODING_NEED_SECOND_সাৰি;
    }

    যদি (cookie == রিক্ত) {
        যদি (bom && _PyTok_SetEncoding(tok, "utf-8") < 0) {
            ঘূৰাই _PYTOK_ENCODING_ERROR;
        }
        ঘূৰাই _PYTOK_ENCODING_DONE;
    }
    যদি (bom && strcmp(cookie, "utf-8") != 0) {
        ধ্রুৱ _PyTok_Chunk *সাৰি = cookie_সাৰি == 2 ? second : first;
        ধ্রুৱ ৱর্ণ *সাৰি_data = সাৰি->data + (cookie_সাৰি == 1 ? *bom_len : 0);
        Py_ssize_t সাৰি_len = সাৰি->len - (cookie_সাৰি == 1 ? *bom_len : 0);
        পূর্ণাঙ্ক end_col = (পূর্ণাঙ্ক)Py_MIN(সাৰি_len, INT_MAX);
        যদি (end_col > 0 && (সাৰি_data[end_col - 1] == '\n' ||
                            সাৰি_data[end_col - 1] == '\r')) {
            end_col--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, সাৰি_data, 0, cookie_সাৰি, 0, end_col,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        ঘূৰাই _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    ঘূৰাই _PYTOK_ENCODING_DONE;
}

পূর্ণাঙ্ক
_PyTok_DecodeOnce(সংরচনা tok_state *tok, _PyTok_Chunk *chunk,
                  ধ্রুৱ ৱর্ণ *encoding, ধ্রুৱ ৱর্ণ *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->data, chunk->len, encoding, errors);
    যদি (unicode == রিক্ত) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        ঘূৰাই -1;
    }
    ঘূৰাই chunk_set_unicode(tok, chunk, unicode, 0);
}

স্থির Py_ssize_t
raw_সাৰি_length(ধ্রুৱ ৱর্ণ *data, Py_ssize_t len)
{
    বাবে (Py_ssize_t i = 0; i < len; i++) {
        যদি (data[i] == '\n') {
            ঘূৰাই i + 1;
        }
        যদি (data[i] == '\r') {
            ঘূৰাই i + 1 < len && data[i + 1] == '\n' ? i + 2 : i + 1;
        }
    }
    ঘূৰাই len;
}

স্থির পূর্ণাঙ্ক
store_prepared_source(সংরচনা tok_state *tok, ধ্রুৱ ৱর্ণ *data, Py_ssize_t len,
                      পূর্ণাঙ্ক preserve_crlf, পূর্ণাঙ্ক add_final_newline)
{
    Py_ssize_t pos = 0;
    যেতিয়ালৈ (pos < len) {
        Py_ssize_t raw_সাৰি_len;
        যদি (preserve_crlf) {
            ধ্রুৱ ৱর্ণ *newline = memchr(data + pos, '\n', len - pos);
            raw_সাৰি_len = newline == রিক্ত
                ? len - pos : newline - data - pos + 1;
        }
        অন্যথা {
            raw_সাৰি_len = raw_সাৰি_length(data + pos, len - pos);
        }
        পূর্ণাঙ্ক terminated = preserve_crlf
            ? data[pos + raw_সাৰি_len - 1] == '\n'
            : data[pos + raw_সাৰি_len - 1] == '\n' ||
              data[pos + raw_সাৰি_len - 1] == '\r';
        পূর্ণাঙ্ক add_newline = add_final_newline &&
            pos + raw_সাৰি_len == len && !terminated;
        ধ্রুৱ ৱর্ণ *সাৰি = data + pos;
        // raw_line_length stops at the first CR or LF, so any CR is in
        // the line terminator.
        Py_ssize_t terminator_len = 0;
        যদি (!preserve_crlf) {
            যদি (সাৰি[raw_সাৰি_len - 1] == '\r') {
                terminator_len = 1;
            }
            অন্যথা যদি (raw_সাৰি_len > 1 && সাৰি[raw_সাৰি_len - 2] == '\r') {
                terminator_len = 2;
            }
        }
        _PyTok_Off appended;
        যদি (terminator_len || add_newline) {
            appended = Py_Tok_Source_Append_সাৰি_With_Newline(
                &tok->source, সাৰি, raw_সাৰি_len - terminator_len);
        }
        অন্যথা {
            appended = Py_Tok_Source_Append_সাৰি(&tok->source, সাৰি, raw_সাৰি_len);
        }
        যদি (appended < 0) {
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            ঘূৰাই -1;
        }
        tok->reader->prepared_final_newline = add_newline;
        pos += raw_সাৰি_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    যদি (add_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || data[len - 1] != '\r'))
    {
        যদি (Py_Tok_Source_Append_সাৰি(&tok->source, "\n", 1) < 0) {
            ঘূৰাই -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    ঘূৰাই 0;
}

পূর্ণাঙ্ক
_PyTok_PrepareString(সংরচনা tok_state *tok, ধ্রুৱ ৱর্ণ *প্ৰবেশ, পূর্ণাঙ্ক utf8_only,
                     পূর্ণাঙ্ক exec_প্ৰবেশ, পূর্ণাঙ্ক preserve_crlf)
{
    Py_ssize_t raw_len = strlen(প্ৰবেশ);
    ৱর্ণ *raw = (ৱর্ণ *)প্ৰবেশ;

    যদি (utf8_only) {
        যদি (_PyTok_SetEncoding(tok, "utf-8") < 0) {
            ঘূৰাই -1;
        }
    }
    অন্যথা {
        Py_ssize_t first_original_len = raw_সাৰি_length(raw, raw_len);
        _PyTok_Chunk first = {
            .data = raw,
            .len = first_original_len,
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        _PyTok_Chunk second = {0};
        পূর্ণাঙ্ক have_second = first_original_len < raw_len;
        যদি (have_second) {
            second.data = raw + first_original_len;
            second.len = raw_সাৰি_length(second.data,
                                         raw_len - first_original_len);
        }
        Py_ssize_t bom_len;
        _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
            tok, &first, have_second ? &second : রিক্ত, 1, &bom_len);
        যদি (detection == _PYTOK_ENCODING_ERROR) {
            ঘূৰাই -1;
        }
        raw += bom_len;
        raw_len -= bom_len;
    }

    _PyTok_Chunk decoded = {
        .data = raw,
        .len = raw_len,
        .ownership = _PYTOK_CHUNK_BORROWED,
    };
    ধ্রুৱ ৱর্ণ *encoding = tok->reader->encoding;
    যদি (encoding != রিক্ত && strcmp(encoding, "utf-8") != 0) {
        যদি (_PyTok_DecodeOnce(
                tok, &decoded, encoding, রিক্ত) < 0) {
            ঘূৰাই -1;
        }
    }

    পূর্ণাঙ্ক stored = store_prepared_source(
        tok, decoded.data, decoded.len, preserve_crlf, exec_প্ৰবেশ);
    _PyTok_ChunkClear(&decoded);
    যদি (stored < 0) {
        ঘূৰাই -1;
    }
    যদি (!utf8_only &&
            (encoding == রিক্ত || strcmp(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(_PyTok_SourceData(&tok->source), tok, 1)) {
        ঘূৰাই -1;
    }
    ঘূৰাই 0;
}

পূর্ণাঙ্ক
_PyTok_StartDecoder(সংরচনা tok_state *tok, ধ্রুৱ ৱর্ণ *errors)
{
    _PyTok_Reader *reader = tok->reader;
    যদি (reader->encoding == রিক্ত || reader->decoder != রিক্ত) {
        ঘূৰাই 0;
    }
    যদি (reader->kind == _PYTOK_READER_FILE &&
            strcmp(reader->encoding, "utf-8") == 0) {
        ঘূৰাই 0;
    }

    PyObject *codec = _PyCodec_LookupTextEncoding(reader->encoding, রিক্ত);
    যদি (codec != রিক্ত) {
        reader->decoder = _PyCodecInfo_GetIncrementalDecoder(codec, errors);
        Py_DECREF(codec);
    }
    যদি (reader->decoder == রিক্ত) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        যদি (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != রিক্ত ? tok->filename : Py_None);
        }
        ঘূৰাই -1;
    }
    ঘূৰাই 0;
}

পূর্ণাঙ্ক
_PyTok_DecodeChunk(সংরচনা tok_state *tok, _PyTok_Chunk *chunk, পূর্ণাঙ্ক final)
{
    _PyTok_Reader *reader = tok->reader;
    যদি (reader->decoder == রিক্ত) {
        ঘূৰাই 0;
    }
    পূর্ণাঙ্ক strip_bom = reader->kind == _PYTOK_READER_READLINE &&
        chunk->len >= UTF16_BOM_LENGTH &&
        (memcmp(chunk->data, UTF16_LE_BOM, UTF16_BOM_LENGTH) == 0 ||
         memcmp(chunk->data, UTF16_BE_BOM, UTF16_BOM_LENGTH) == 0);
    PyObject *প্ৰবেশ;
    যদি (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            PyBytes_Check(chunk->owner) &&
            chunk->data == PyBytes_AS_STRING(chunk->owner)) {
        প্ৰবেশ = Py_NewRef(chunk->owner);
    }
    অন্যথা {
        প্ৰবেশ = PyBytes_FromStringAndSize(chunk->data, chunk->len);
    }
    যদি (প্ৰবেশ == রিক্ত) {
        tok->done = E_NOMEM;
        ঘূৰাই -1;
    }
    PyObject *unicode = PyObject_CallMethodObjArgs(
        reader->decoder, &_Py_ID(decode), প্ৰবেশ,
        final ? Py_True : Py_False, রিক্ত);
    Py_DECREF(প্ৰবেশ);
    যদি (unicode == রিক্ত) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        যদি (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != রিক্ত ? tok->filename : Py_None);
        }
        ঘূৰাই -1;
    }
    যদি (!PyUnicode_Check(unicode)) {
        PyErr_Format(PyExc_TypeError,
                     "decoder should return a string result, not '%.200s'",
                     Py_TYPE(unicode)->tp_name);
        Py_DECREF(unicode);
        tok->done = E_DECODE;
        ঘূৰাই -1;
    }
    ঘূৰাই chunk_set_unicode(tok, chunk, unicode, strip_bom);
}

পূর্ণাঙ্ক
Py_Tok_Decoder_Has_Buffered_প্ৰবেশ(সংরচনা tok_state *tok)
{
    যদি (tok->reader->decoder == রিক্ত) {
        ঘূৰাই 0;
    }
    PyObject *state = PyObject_CallMethodNoArgs(
        tok->reader->decoder, &_Py_ID(getstate));
    যদি (state == রিক্ত) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        ঘূৰাই -1;
    }
    যদি (!PyTuple_Check(state) || PyTuple_GET_SIZE(state) != 2 ||
            !PyBytes_Check(PyTuple_GET_ITEM(state, 0)) ||
            !PyLong_Check(PyTuple_GET_ITEM(state, 1))) {
        Py_DECREF(state);
        PyErr_SetString(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->done = E_DECODE;
        ঘূৰাই -1;
    }
    পূর্ণাঙ্ক pending = PyBytes_GET_SIZE(PyTuple_GET_ITEM(state, 0)) != 0;
    Py_DECREF(state);
    ঘূৰাই pending;
}
