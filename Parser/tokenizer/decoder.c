#અન્તર્ભાવ "Python.h"
#અન્તર્ભાવ "pycore_codecs.h"
#અન્તર્ભાવ "pycore_global_strings.h"
#અન્તર્ભાવ "pycore_runtime.h"
#અન્તર્ભાવ "errcode.h"

#અન્તર્ભાવ "reader_internal.h"
#અન્તર્ભાવ "helpers.h"
#અન્તર્ભાવ "../lexer/state.h"

#define UNICODE_BOM 0xFEFF
#define UTF16_LE_BOM "\xff\xfe"
#define UTF16_BE_BOM "\xfe\xff"
#define UTF16_BOM_LENGTH ((Py_ssize_t)(કદ(UTF16_LE_BOM) - 1))
#define UTF8_BOM "\xef\xbb\xbf"
#define UTF8_BOM_LENGTH ((Py_ssize_t)(કદ(UTF8_BOM) - 1))

અક્ષર *
_PyTok_CopyBytes(અચલ અક્ષર *data, Py_ssize_t len)
{
    જો (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        પાછા ખાલી;
    }
    અક્ષર *copy = PyMem_Malloc((size_t)len + 1);
    જો (copy == ખાલી) {
        PyErr_NoMemory();
        પાછા ખાલી;
    }
    memcpy(copy, data, (size_t)len);
    copy[len] = '\0';
    પાછા copy;
}

સ્થિર શૂન્ય
chunk_release_data(_PyTok_Chunk *chunk)
{
    પસંદગી (chunk->ownership) {
        સ્થિતિ _PYTOK_CHUNK_BORROWED:
            તોડો;
        સ્થિતિ _PYTOK_CHUNK_PYMEM:
            PyMem_Free(chunk->data);
            તોડો;
        સ્થિતિ _PYTOK_CHUNK_PYOBJECT:
            Py_DECREF(chunk->owner);
            તોડો;
    }
}

શૂન્ય
_PyTok_ChunkClear(_PyTok_Chunk *chunk)
{
    chunk_release_data(chunk);
    *chunk = (_PyTok_Chunk){0};
}

સ્થિર પૂર્ણાંક
chunk_set_unicode(સંરચના tok_state *tok, _PyTok_Chunk *chunk,
                  PyObject *unicode, પૂર્ણાંક strip_bom)
{
    Py_ssize_t utf8_len;
    અચલ અક્ષર *utf8 = PyUnicode_AsUTF8AndSize(unicode, &utf8_len);
    જો (utf8 == ખાલી) {
        Py_DECREF(unicode);
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        પાછા -1;
    }
    જો (strip_bom && PyUnicode_GET_LENGTH(unicode) > 0 &&
            PyUnicode_ReadChar(unicode, 0) == UNICODE_BOM) {
        utf8 += UTF8_BOM_LENGTH;
        utf8_len -= UTF8_BOM_LENGTH;
    }
    chunk_release_data(chunk);
    chunk->owner = unicode;
    chunk->data = (અક્ષર *)utf8;
    chunk->len = utf8_len;
    chunk->ownership = _PYTOK_CHUNK_PYOBJECT;
    પાછા 0;
}

અક્ષર *
_PyTok_NormalizeNewlines(અચલ અક્ષર *data, Py_ssize_t len,
                         Py_ssize_t *out_len)
{
    જો (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        પાછા ખાલી;
    }
    અક્ષર *result = PyMem_Malloc((size_t)len + 1);
    જો (result == ખાલી) {
        PyErr_NoMemory();
        પાછા ખાલી;
    }
    Py_ssize_t લિખ = 0;
    માટે (Py_ssize_t પઠ = 0; પઠ < len; પઠ++) {
        અક્ષર c = data[પઠ];
        જો (c == '\r') {
            જો (પઠ + 1 < len && data[પઠ + 1] == '\n') {
                પઠ++;
            }
            c = '\n';
        }
        result[લિખ++] = c;
    }
    result[લિખ] = '\0';
    *out_len = લિખ;
    પાછા result;
}

પૂર્ણાંક
_PyTok_SetEncoding(સંરચના tok_state *tok, અચલ અક્ષર *encoding)
{
    અક્ષર *copy = _PyTok_CopyBytes(encoding, strlen(encoding));
    જો (copy == ખાલી) {
        tok->done = E_NOMEM;
        પાછા -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    પાછા 0;
}

સ્થિર પૂર્ણાંક
find_cookie(અચલ અક્ષર *લીટી, Py_ssize_t len, અક્ષર **encoding, પૂર્ણાંક *scan_next)
{
    Py_ssize_t i = 0;
    *encoding = ખાલી;
    *scan_next = 1;
    માટે (; i < len; i++) {
        જો (લીટી[i] == '#') {
            તોડો;
        }
        જો (લીટી[i] == '\n' || લીટી[i] == '\r') {
            પાછા 0;
        }
        જો (લીટી[i] != ' ' && લીટી[i] != '\t' && લીટી[i] != '\f') {
            *scan_next = 0;
            પાછા 0;
        }
    }
    અચલ Py_ssize_t marker_len = કદ("coding") - 1;
    માટે (; i < len - marker_len; i++) {
        જો (memcmp(લીટી + i, "coding", marker_len) != 0) {
            ચાલુ;
        }
        અચલ અક્ષર *cursor = લીટી + i + marker_len;
        જો (*cursor != ':' && *cursor != '=') {
            ચાલુ;
        }
        કરો {
            cursor++;
        } જ્યારે (cursor < લીટી + len &&
                 (*cursor == ' ' || *cursor == '\t'));
        અચલ અક્ષર *start = cursor;
        જ્યારે (cursor < લીટી + len &&
                (Py_ISALNUM(*cursor) || *cursor == '-' ||
                 *cursor == '_' || *cursor == '.')) {
            cursor++;
        }
        જો (cursor == start) {
            ચાલુ;
        }
        Py_ssize_t encoding_len = cursor - start;
        // Include the longest recognized prefix and its NUL terminator.
        અક્ષર normalized[કદ("iso-latin-1-")];
        અચલ Py_ssize_t prefix_len = Py_MIN(
            encoding_len, (Py_ssize_t)કદ(normalized) - 1);
        Py_ssize_t n;
        માટે (n = 0; n < prefix_len; n++) {
            normalized[n] = start[n] == '_' ? '-' : Py_TOLOWER(start[n]);
        }
        normalized[n] = '\0';
        અચલ અક્ષર *canonical = start;
        જો (strcmp(normalized, "utf-8") == 0 ||
                strncmp(normalized, "utf-8-", કદ("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        નહીંતર જો (strcmp(normalized, "latin-1") == 0 ||
                 strcmp(normalized, "iso-8859-1") == 0 ||
                 strcmp(normalized, "iso-latin-1") == 0 ||
                 strncmp(normalized, "latin-1-", કદ("latin-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-8859-1-", કદ("iso-8859-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-latin-1-", કદ("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        જો (canonical != start) {
            encoding_len = strlen(canonical);
        }
        *encoding = _PyTok_CopyBytes(canonical, encoding_len);
        જો (*encoding == ખાલી) {
            પાછા -1;
        }
        *scan_next = 0;
        પાછા 0;
    }
    પાછા 0;
}

_PyTok_EncodingResult
_PyTok_DetectEncoding(સંરચના tok_state *tok, અચલ _PyTok_Chunk *first,
                      અચલ _PyTok_Chunk *second, પૂર્ણાંક final,
                      Py_ssize_t *bom_len)
{
    પૂર્ણાંક bom = first->len >= UTF8_BOM_LENGTH &&
        memcmp(first->data, UTF8_BOM, UTF8_BOM_LENGTH) == 0;
    *bom_len = bom ? UTF8_BOM_LENGTH : 0;

    અક્ષર *cookie = ખાલી;
    પૂર્ણાંક scan_next = 0;
    પૂર્ણાંક cookie_લીટી = 1;
    અચલ અક્ષર *first_data = first->data + *bom_len;
    Py_ssize_t first_len = first->len - *bom_len;
    જો (find_cookie(first_data, first_len, &cookie, &scan_next) < 0) {
        પાછા _PYTOK_ENCODING_ERROR;
    }
    જો (cookie == ખાલી && scan_next && second != ખાલી) {
        જો (find_cookie(second->data, second->len, &cookie, &scan_next) < 0) {
            પાછા _PYTOK_ENCODING_ERROR;
        }
        cookie_લીટી = 2;
    }
    નહીંતર જો (cookie == ખાલી && scan_next && !final) {
        પાછા PYTOK_ENCODING_NEED_SECOND_લીટી;
    }

    જો (cookie == ખાલી) {
        જો (bom && _PyTok_SetEncoding(tok, "utf-8") < 0) {
            પાછા _PYTOK_ENCODING_ERROR;
        }
        પાછા _PYTOK_ENCODING_DONE;
    }
    જો (bom && strcmp(cookie, "utf-8") != 0) {
        અચલ _PyTok_Chunk *લીટી = cookie_લીટી == 2 ? second : first;
        અચલ અક્ષર *લીટી_data = લીટી->data + (cookie_લીટી == 1 ? *bom_len : 0);
        Py_ssize_t લીટી_len = લીટી->len - (cookie_લીટી == 1 ? *bom_len : 0);
        પૂર્ણાંક end_col = (પૂર્ણાંક)Py_MIN(લીટી_len, INT_MAX);
        જો (end_col > 0 && (લીટી_data[end_col - 1] == '\n' ||
                            લીટી_data[end_col - 1] == '\r')) {
            end_col--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, લીટી_data, 0, cookie_લીટી, 0, end_col,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        પાછા _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    પાછા _PYTOK_ENCODING_DONE;
}

પૂર્ણાંક
_PyTok_DecodeOnce(સંરચના tok_state *tok, _PyTok_Chunk *chunk,
                  અચલ અક્ષર *encoding, અચલ અક્ષર *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->data, chunk->len, encoding, errors);
    જો (unicode == ખાલી) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        પાછા -1;
    }
    પાછા chunk_set_unicode(tok, chunk, unicode, 0);
}

સ્થિર Py_ssize_t
raw_લીટી_length(અચલ અક્ષર *data, Py_ssize_t len)
{
    માટે (Py_ssize_t i = 0; i < len; i++) {
        જો (data[i] == '\n') {
            પાછા i + 1;
        }
        જો (data[i] == '\r') {
            પાછા i + 1 < len && data[i + 1] == '\n' ? i + 2 : i + 1;
        }
    }
    પાછા len;
}

સ્થિર પૂર્ણાંક
store_prepared_source(સંરચના tok_state *tok, અચલ અક્ષર *data, Py_ssize_t len,
                      પૂર્ણાંક preserve_crlf, પૂર્ણાંક add_final_newline)
{
    Py_ssize_t pos = 0;
    જ્યારે (pos < len) {
        Py_ssize_t raw_લીટી_len;
        જો (preserve_crlf) {
            અચલ અક્ષર *newline = memchr(data + pos, '\n', len - pos);
            raw_લીટી_len = newline == ખાલી
                ? len - pos : newline - data - pos + 1;
        }
        નહીંતર {
            raw_લીટી_len = raw_લીટી_length(data + pos, len - pos);
        }
        પૂર્ણાંક terminated = preserve_crlf
            ? data[pos + raw_લીટી_len - 1] == '\n'
            : data[pos + raw_લીટી_len - 1] == '\n' ||
              data[pos + raw_લીટી_len - 1] == '\r';
        પૂર્ણાંક add_newline = add_final_newline &&
            pos + raw_લીટી_len == len && !terminated;
        અચલ અક્ષર *લીટી = data + pos;
        // raw_line_length stops at the first CR or LF, so any CR is in
        // the line terminator.
        Py_ssize_t terminator_len = 0;
        જો (!preserve_crlf) {
            જો (લીટી[raw_લીટી_len - 1] == '\r') {
                terminator_len = 1;
            }
            નહીંતર જો (raw_લીટી_len > 1 && લીટી[raw_લીટી_len - 2] == '\r') {
                terminator_len = 2;
            }
        }
        _PyTok_Off appended;
        જો (terminator_len || add_newline) {
            appended = Py_Tok_Source_Append_લીટી_With_Newline(
                &tok->source, લીટી, raw_લીટી_len - terminator_len);
        }
        નહીંતર {
            appended = Py_Tok_Source_Append_લીટી(&tok->source, લીટી, raw_લીટી_len);
        }
        જો (appended < 0) {
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            પાછા -1;
        }
        tok->reader->prepared_final_newline = add_newline;
        pos += raw_લીટી_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    જો (add_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || data[len - 1] != '\r'))
    {
        જો (Py_Tok_Source_Append_લીટી(&tok->source, "\n", 1) < 0) {
            પાછા -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    પાછા 0;
}

પૂર્ણાંક
_PyTok_PrepareString(સંરચના tok_state *tok, અચલ અક્ષર *આગત, પૂર્ણાંક utf8_only,
                     પૂર્ણાંક exec_આગત, પૂર્ણાંક preserve_crlf)
{
    Py_ssize_t raw_len = strlen(આગત);
    અક્ષર *raw = (અક્ષર *)આગત;

    જો (utf8_only) {
        જો (_PyTok_SetEncoding(tok, "utf-8") < 0) {
            પાછા -1;
        }
    }
    નહીંતર {
        Py_ssize_t first_original_len = raw_લીટી_length(raw, raw_len);
        _PyTok_Chunk first = {
            .data = raw,
            .len = first_original_len,
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        _PyTok_Chunk second = {0};
        પૂર્ણાંક have_second = first_original_len < raw_len;
        જો (have_second) {
            second.data = raw + first_original_len;
            second.len = raw_લીટી_length(second.data,
                                         raw_len - first_original_len);
        }
        Py_ssize_t bom_len;
        _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
            tok, &first, have_second ? &second : ખાલી, 1, &bom_len);
        જો (detection == _PYTOK_ENCODING_ERROR) {
            પાછા -1;
        }
        raw += bom_len;
        raw_len -= bom_len;
    }

    _PyTok_Chunk decoded = {
        .data = raw,
        .len = raw_len,
        .ownership = _PYTOK_CHUNK_BORROWED,
    };
    અચલ અક્ષર *encoding = tok->reader->encoding;
    જો (encoding != ખાલી && strcmp(encoding, "utf-8") != 0) {
        જો (_PyTok_DecodeOnce(
                tok, &decoded, encoding, ખાલી) < 0) {
            પાછા -1;
        }
    }

    પૂર્ણાંક stored = store_prepared_source(
        tok, decoded.data, decoded.len, preserve_crlf, exec_આગત);
    _PyTok_ChunkClear(&decoded);
    જો (stored < 0) {
        પાછા -1;
    }
    જો (!utf8_only &&
            (encoding == ખાલી || strcmp(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(_PyTok_SourceData(&tok->source), tok, 1)) {
        પાછા -1;
    }
    પાછા 0;
}

પૂર્ણાંક
_PyTok_StartDecoder(સંરચના tok_state *tok, અચલ અક્ષર *errors)
{
    _PyTok_Reader *reader = tok->reader;
    જો (reader->encoding == ખાલી || reader->decoder != ખાલી) {
        પાછા 0;
    }
    જો (reader->kind == _PYTOK_READER_FILE &&
            strcmp(reader->encoding, "utf-8") == 0) {
        પાછા 0;
    }

    PyObject *codec = _PyCodec_LookupTextEncoding(reader->encoding, ખાલી);
    જો (codec != ખાલી) {
        reader->decoder = _PyCodecInfo_GetIncrementalDecoder(codec, errors);
        Py_DECREF(codec);
    }
    જો (reader->decoder == ખાલી) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        જો (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != ખાલી ? tok->filename : Py_None);
        }
        પાછા -1;
    }
    પાછા 0;
}

પૂર્ણાંક
_PyTok_DecodeChunk(સંરચના tok_state *tok, _PyTok_Chunk *chunk, પૂર્ણાંક final)
{
    _PyTok_Reader *reader = tok->reader;
    જો (reader->decoder == ખાલી) {
        પાછા 0;
    }
    પૂર્ણાંક strip_bom = reader->kind == _PYTOK_READER_READLINE &&
        chunk->len >= UTF16_BOM_LENGTH &&
        (memcmp(chunk->data, UTF16_LE_BOM, UTF16_BOM_LENGTH) == 0 ||
         memcmp(chunk->data, UTF16_BE_BOM, UTF16_BOM_LENGTH) == 0);
    PyObject *આગત;
    જો (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            PyBytes_Check(chunk->owner) &&
            chunk->data == PyBytes_AS_STRING(chunk->owner)) {
        આગત = Py_NewRef(chunk->owner);
    }
    નહીંતર {
        આગત = PyBytes_FromStringAndSize(chunk->data, chunk->len);
    }
    જો (આગત == ખાલી) {
        tok->done = E_NOMEM;
        પાછા -1;
    }
    PyObject *unicode = PyObject_CallMethodObjArgs(
        reader->decoder, &_Py_ID(decode), આગત,
        final ? Py_True : Py_False, ખાલી);
    Py_DECREF(આગત);
    જો (unicode == ખાલી) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        જો (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != ખાલી ? tok->filename : Py_None);
        }
        પાછા -1;
    }
    જો (!PyUnicode_Check(unicode)) {
        PyErr_Format(PyExc_TypeError,
                     "decoder should return a string result, not '%.200s'",
                     Py_TYPE(unicode)->tp_name);
        Py_DECREF(unicode);
        tok->done = E_DECODE;
        પાછા -1;
    }
    પાછા chunk_set_unicode(tok, chunk, unicode, strip_bom);
}

પૂર્ણાંક
Py_Tok_Decoder_Has_Buffered_આગત(સંરચના tok_state *tok)
{
    જો (tok->reader->decoder == ખાલી) {
        પાછા 0;
    }
    PyObject *state = PyObject_CallMethodNoArgs(
        tok->reader->decoder, &_Py_ID(getstate));
    જો (state == ખાલી) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        પાછા -1;
    }
    જો (!PyTuple_Check(state) || PyTuple_GET_SIZE(state) != 2 ||
            !PyBytes_Check(PyTuple_GET_ITEM(state, 0)) ||
            !PyLong_Check(PyTuple_GET_ITEM(state, 1))) {
        Py_DECREF(state);
        PyErr_SetString(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->done = E_DECODE;
        પાછા -1;
    }
    પૂર્ણાંક pending = PyBytes_GET_SIZE(PyTuple_GET_ITEM(state, 0)) != 0;
    Py_DECREF(state);
    પાછા pending;
}
