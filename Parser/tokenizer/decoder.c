#include "Python.h"
#include "pycore_codecs.h"
#include "pycore_global_strings.h"
#include "pycore_runtime.h"
#include "errcode.h"

#include "reader_internal.h"
#include "helpers.h"
#include "../lexer/state.h"

#define UNICODE_BOM 0xFEFF
#define UTF16_LE_BOM "\xff\xfe"
#define UTF16_BE_BOM "\xfe\xff"
#define UTF16_BOM_LENGTH ((Py_ssize_t)(sizeof(UTF16_LE_BOM) - 1))
#define UTF8_BOM "\xef\xbb\xbf"
#define UTF8_BOM_LENGTH ((Py_ssize_t)(sizeof(UTF8_BOM) - 1))

char *
_PyTok_CopyBytes(const char *data, Py_ssize_t len)
{
    nếu (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        trả_về NULL;
    }
    char *copy = PyMem_Malloc((size_t)len + 1);
    nếu (copy == NULL) {
        PyErr_NoMemory();
        trả_về NULL;
    }
    memcpy(copy, data, (size_t)len);
    copy[len] = '\0';
    trả_về copy;
}

static void
chunk_release_data(_PyTok_Chunk *chunk)
{
    switch (chunk->ownership) {
        case _PYTOK_CHUNK_BORROWED:
            break;
        case _PYTOK_CHUNK_PYMEM:
            PyMem_Free(chunk->data);
            break;
        case _PYTOK_CHUNK_PYOBJECT:
            Py_DECREF(chunk->owner);
            break;
    }
}

void
_PyTok_ChunkClear(_PyTok_Chunk *chunk)
{
    chunk_release_data(chunk);
    *chunk = (_PyTok_Chunk){0};
}

static số_nguyên
chunk_set_unicode(struct tok_state *tok, _PyTok_Chunk *chunk,
                  PyObject *unicode, số_nguyên strip_bom)
{
    Py_ssize_t utf8_len;
    const char *utf8 = PyUnicode_AsUTF8AndSize(unicode, &utf8_len);
    nếu (utf8 == NULL) {
        Py_DECREF(unicode);
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        trả_về -1;
    }
    nếu (strip_bom && PyUnicode_GET_LENGTH(unicode) > 0 &&
            PyUnicode_ReadChar(unicode, 0) == UNICODE_BOM) {
        utf8 += UTF8_BOM_LENGTH;
        utf8_len -= UTF8_BOM_LENGTH;
    }
    chunk_release_data(chunk);
    chunk->owner = unicode;
    chunk->data = (char *)utf8;
    chunk->len = utf8_len;
    chunk->ownership = _PYTOK_CHUNK_PYOBJECT;
    trả_về 0;
}

char *
_PyTok_NormalizeNewlines(const char *data, Py_ssize_t len,
                         Py_ssize_t *out_len)
{
    nếu (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        trả_về NULL;
    }
    char *result = PyMem_Malloc((size_t)len + 1);
    nếu (result == NULL) {
        PyErr_NoMemory();
        trả_về NULL;
    }
    Py_ssize_t write = 0;
    cho (Py_ssize_t read = 0; read < len; read++) {
        char c = data[read];
        nếu (c == '\r') {
            nếu (read + 1 < len && data[read + 1] == '\n') {
                read++;
            }
            c = '\n';
        }
        result[write++] = c;
    }
    result[write] = '\0';
    *out_len = write;
    trả_về result;
}

số_nguyên
_PyTok_SetEncoding(struct tok_state *tok, const char *encoding)
{
    char *copy = _PyTok_CopyBytes(encoding, strlen(encoding));
    nếu (copy == NULL) {
        tok->done = E_NOMEM;
        trả_về -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    trả_về 0;
}

static số_nguyên
find_cookie(const char *line, Py_ssize_t len, char **encoding, số_nguyên *scan_next)
{
    Py_ssize_t i = 0;
    *encoding = NULL;
    *scan_next = 1;
    cho (; i < len; i++) {
        nếu (line[i] == '#') {
            break;
        }
        nếu (line[i] == '\n' || line[i] == '\r') {
            trả_về 0;
        }
        nếu (line[i] != ' ' && line[i] != '\t' && line[i] != '\f') {
            *scan_next = 0;
            trả_về 0;
        }
    }
    const Py_ssize_t marker_len = sizeof("coding") - 1;
    cho (; i < len - marker_len; i++) {
        nếu (memcmp(line + i, "coding", marker_len) != 0) {
            continue;
        }
        const char *cursor = line + i + marker_len;
        nếu (*cursor != ':' && *cursor != '=') {
            continue;
        }
        do {
            cursor++;
        } trong_khi (cursor < line + len &&
                 (*cursor == ' ' || *cursor == '\t'));
        const char *start = cursor;
        trong_khi (cursor < line + len &&
                (Py_ISALNUM(*cursor) || *cursor == '-' ||
                 *cursor == '_' || *cursor == '.')) {
            cursor++;
        }
        nếu (cursor == start) {
            continue;
        }
        Py_ssize_t encoding_len = cursor - start;
        // Include the longest recognized prefix and its NUL terminator.
        char normalized[sizeof("iso-latin-1-")];
        const Py_ssize_t prefix_len = Py_MIN(
            encoding_len, (Py_ssize_t)sizeof(normalized) - 1);
        Py_ssize_t n;
        cho (n = 0; n < prefix_len; n++) {
            normalized[n] = start[n] == '_' ? '-' : Py_TOLOWER(start[n]);
        }
        normalized[n] = '\0';
        const char *canonical = start;
        nếu (strcmp(normalized, "utf-8") == 0 ||
                strncmp(normalized, "utf-8-", sizeof("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        không_thì nếu (strcmp(normalized, "latin-1") == 0 ||
                 strcmp(normalized, "iso-8859-1") == 0 ||
                 strcmp(normalized, "iso-latin-1") == 0 ||
                 strncmp(normalized, "latin-1-", sizeof("latin-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-8859-1-", sizeof("iso-8859-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-latin-1-", sizeof("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        nếu (canonical != start) {
            encoding_len = strlen(canonical);
        }
        *encoding = _PyTok_CopyBytes(canonical, encoding_len);
        nếu (*encoding == NULL) {
            trả_về -1;
        }
        *scan_next = 0;
        trả_về 0;
    }
    trả_về 0;
}

_PyTok_EncodingResult
_PyTok_DetectEncoding(struct tok_state *tok, const _PyTok_Chunk *first,
                      const _PyTok_Chunk *second, số_nguyên final,
                      Py_ssize_t *bom_len)
{
    số_nguyên bom = first->len >= UTF8_BOM_LENGTH &&
        memcmp(first->data, UTF8_BOM, UTF8_BOM_LENGTH) == 0;
    *bom_len = bom ? UTF8_BOM_LENGTH : 0;

    char *cookie = NULL;
    số_nguyên scan_next = 0;
    số_nguyên cookie_line = 1;
    const char *first_data = first->data + *bom_len;
    Py_ssize_t first_len = first->len - *bom_len;
    nếu (find_cookie(first_data, first_len, &cookie, &scan_next) < 0) {
        trả_về _PYTOK_ENCODING_ERROR;
    }
    nếu (cookie == NULL && scan_next && second != NULL) {
        nếu (find_cookie(second->data, second->len, &cookie, &scan_next) < 0) {
            trả_về _PYTOK_ENCODING_ERROR;
        }
        cookie_line = 2;
    }
    không_thì nếu (cookie == NULL && scan_next && !final) {
        trả_về _PYTOK_ENCODING_NEED_SECOND_LINE;
    }

    nếu (cookie == NULL) {
        nếu (bom && _PyTok_SetEncoding(tok, "utf-8") < 0) {
            trả_về _PYTOK_ENCODING_ERROR;
        }
        trả_về _PYTOK_ENCODING_DONE;
    }
    nếu (bom && strcmp(cookie, "utf-8") != 0) {
        const _PyTok_Chunk *line = cookie_line == 2 ? second : first;
        const char *line_data = line->data + (cookie_line == 1 ? *bom_len : 0);
        Py_ssize_t line_len = line->len - (cookie_line == 1 ? *bom_len : 0);
        số_nguyên end_col = (số_nguyên)Py_MIN(line_len, INT_MAX);
        nếu (end_col > 0 && (line_data[end_col - 1] == '\n' ||
                            line_data[end_col - 1] == '\r')) {
            end_col--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, line_data, 0, cookie_line, 0, end_col,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        trả_về _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    trả_về _PYTOK_ENCODING_DONE;
}

số_nguyên
_PyTok_DecodeOnce(struct tok_state *tok, _PyTok_Chunk *chunk,
                  const char *encoding, const char *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->data, chunk->len, encoding, errors);
    nếu (unicode == NULL) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        trả_về -1;
    }
    trả_về chunk_set_unicode(tok, chunk, unicode, 0);
}

static Py_ssize_t
raw_line_length(const char *data, Py_ssize_t len)
{
    cho (Py_ssize_t i = 0; i < len; i++) {
        nếu (data[i] == '\n') {
            trả_về i + 1;
        }
        nếu (data[i] == '\r') {
            trả_về i + 1 < len && data[i + 1] == '\n' ? i + 2 : i + 1;
        }
    }
    trả_về len;
}

static số_nguyên
store_prepared_source(struct tok_state *tok, const char *data, Py_ssize_t len,
                      số_nguyên preserve_crlf, số_nguyên add_final_newline)
{
    Py_ssize_t pos = 0;
    trong_khi (pos < len) {
        Py_ssize_t raw_line_len;
        nếu (preserve_crlf) {
            const char *newline = memchr(data + pos, '\n', len - pos);
            raw_line_len = newline == NULL
                ? len - pos : newline - data - pos + 1;
        }
        không_thì {
            raw_line_len = raw_line_length(data + pos, len - pos);
        }
        số_nguyên terminated = preserve_crlf
            ? data[pos + raw_line_len - 1] == '\n'
            : data[pos + raw_line_len - 1] == '\n' ||
              data[pos + raw_line_len - 1] == '\r';
        số_nguyên add_newline = add_final_newline &&
            pos + raw_line_len == len && !terminated;
        const char *line = data + pos;
        // raw_line_length stops at the first CR or LF, so any CR is in
        // the line terminator.
        Py_ssize_t terminator_len = 0;
        nếu (!preserve_crlf) {
            nếu (line[raw_line_len - 1] == '\r') {
                terminator_len = 1;
            }
            không_thì nếu (raw_line_len > 1 && line[raw_line_len - 2] == '\r') {
                terminator_len = 2;
            }
        }
        _PyTok_Off appended;
        nếu (terminator_len || add_newline) {
            appended = _PyTok_SourceAppendLineWithNewline(
                &tok->source, line, raw_line_len - terminator_len);
        }
        không_thì {
            appended = _PyTok_SourceAppendLine(&tok->source, line, raw_line_len);
        }
        nếu (appended < 0) {
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            trả_về -1;
        }
        tok->reader->prepared_final_newline = add_newline;
        pos += raw_line_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    nếu (add_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || data[len - 1] != '\r'))
    {
        nếu (_PyTok_SourceAppendLine(&tok->source, "\n", 1) < 0) {
            trả_về -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    trả_về 0;
}

số_nguyên
_PyTok_PrepareString(struct tok_state *tok, const char *input, số_nguyên utf8_only,
                     số_nguyên exec_input, số_nguyên preserve_crlf)
{
    Py_ssize_t raw_len = strlen(input);
    char *raw = (char *)input;

    nếu (utf8_only) {
        nếu (_PyTok_SetEncoding(tok, "utf-8") < 0) {
            trả_về -1;
        }
    }
    không_thì {
        Py_ssize_t first_original_len = raw_line_length(raw, raw_len);
        _PyTok_Chunk first = {
            .data = raw,
            .len = first_original_len,
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        _PyTok_Chunk second = {0};
        số_nguyên have_second = first_original_len < raw_len;
        nếu (have_second) {
            second.data = raw + first_original_len;
            second.len = raw_line_length(second.data,
                                         raw_len - first_original_len);
        }
        Py_ssize_t bom_len;
        _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
            tok, &first, have_second ? &second : NULL, 1, &bom_len);
        nếu (detection == _PYTOK_ENCODING_ERROR) {
            trả_về -1;
        }
        raw += bom_len;
        raw_len -= bom_len;
    }

    _PyTok_Chunk decoded = {
        .data = raw,
        .len = raw_len,
        .ownership = _PYTOK_CHUNK_BORROWED,
    };
    const char *encoding = tok->reader->encoding;
    nếu (encoding != NULL && strcmp(encoding, "utf-8") != 0) {
        nếu (_PyTok_DecodeOnce(
                tok, &decoded, encoding, NULL) < 0) {
            trả_về -1;
        }
    }

    số_nguyên stored = store_prepared_source(
        tok, decoded.data, decoded.len, preserve_crlf, exec_input);
    _PyTok_ChunkClear(&decoded);
    nếu (stored < 0) {
        trả_về -1;
    }
    nếu (!utf8_only &&
            (encoding == NULL || strcmp(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(_PyTok_SourceData(&tok->source), tok, 1)) {
        trả_về -1;
    }
    trả_về 0;
}

số_nguyên
_PyTok_StartDecoder(struct tok_state *tok, const char *errors)
{
    _PyTok_Reader *reader = tok->reader;
    nếu (reader->encoding == NULL || reader->decoder != NULL) {
        trả_về 0;
    }
    nếu (reader->kind == _PYTOK_READER_FILE &&
            strcmp(reader->encoding, "utf-8") == 0) {
        trả_về 0;
    }

    PyObject *codec = _PyCodec_LookupTextEncoding(reader->encoding, NULL);
    nếu (codec != NULL) {
        reader->decoder = _PyCodecInfo_GetIncrementalDecoder(codec, errors);
        Py_DECREF(codec);
    }
    nếu (reader->decoder == NULL) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        nếu (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != NULL ? tok->filename : Py_None);
        }
        trả_về -1;
    }
    trả_về 0;
}

số_nguyên
_PyTok_DecodeChunk(struct tok_state *tok, _PyTok_Chunk *chunk, số_nguyên final)
{
    _PyTok_Reader *reader = tok->reader;
    nếu (reader->decoder == NULL) {
        trả_về 0;
    }
    số_nguyên strip_bom = reader->kind == _PYTOK_READER_READLINE &&
        chunk->len >= UTF16_BOM_LENGTH &&
        (memcmp(chunk->data, UTF16_LE_BOM, UTF16_BOM_LENGTH) == 0 ||
         memcmp(chunk->data, UTF16_BE_BOM, UTF16_BOM_LENGTH) == 0);
    PyObject *input;
    nếu (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            PyBytes_Check(chunk->owner) &&
            chunk->data == PyBytes_AS_STRING(chunk->owner)) {
        input = Py_NewRef(chunk->owner);
    }
    không_thì {
        input = PyBytes_FromStringAndSize(chunk->data, chunk->len);
    }
    nếu (input == NULL) {
        tok->done = E_NOMEM;
        trả_về -1;
    }
    PyObject *unicode = PyObject_CallMethodObjArgs(
        reader->decoder, &_Py_ID(decode), input,
        final ? Py_True : Py_False, NULL);
    Py_DECREF(input);
    nếu (unicode == NULL) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        nếu (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != NULL ? tok->filename : Py_None);
        }
        trả_về -1;
    }
    nếu (!PyUnicode_Check(unicode)) {
        PyErr_Format(PyExc_TypeError,
                     "decoder should return a string result, not '%.200s'",
                     Py_TYPE(unicode)->tp_name);
        Py_DECREF(unicode);
        tok->done = E_DECODE;
        trả_về -1;
    }
    trả_về chunk_set_unicode(tok, chunk, unicode, strip_bom);
}

số_nguyên
_PyTok_DecoderHasBufferedInput(struct tok_state *tok)
{
    nếu (tok->reader->decoder == NULL) {
        trả_về 0;
    }
    PyObject *state = PyObject_CallMethodNoArgs(
        tok->reader->decoder, &_Py_ID(getstate));
    nếu (state == NULL) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        trả_về -1;
    }
    nếu (!PyTuple_Check(state) || PyTuple_GET_SIZE(state) != 2 ||
            !PyBytes_Check(PyTuple_GET_ITEM(state, 0)) ||
            !PyLong_Check(PyTuple_GET_ITEM(state, 1))) {
        Py_DECREF(state);
        PyErr_SetString(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->done = E_DECODE;
        trả_về -1;
    }
    số_nguyên pending = PyBytes_GET_SIZE(PyTuple_GET_ITEM(state, 0)) != 0;
    Py_DECREF(state);
    trả_về pending;
}
