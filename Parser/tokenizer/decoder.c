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
    если (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        вернуть NULL;
    }
    char *copy = PyMem_Malloc((size_t)len + 1);
    если (copy == NULL) {
        PyErr_NoMemory();
        вернуть NULL;
    }
    memcpy(copy, data, (size_t)len);
    copy[len] = '\0';
    вернуть copy;
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

static целое
chunk_set_unicode(struct tok_state *tok, _PyTok_Chunk *chunk,
                  PyObject *unicode, целое strip_bom)
{
    Py_ssize_t utf8_len;
    const char *utf8 = PyUnicode_AsUTF8AndSize(unicode, &utf8_len);
    если (utf8 == NULL) {
        Py_DECREF(unicode);
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        вернуть -1;
    }
    если (strip_bom && PyUnicode_GET_LENGTH(unicode) > 0 &&
            PyUnicode_ReadChar(unicode, 0) == UNICODE_BOM) {
        utf8 += UTF8_BOM_LENGTH;
        utf8_len -= UTF8_BOM_LENGTH;
    }
    chunk_release_data(chunk);
    chunk->owner = unicode;
    chunk->data = (char *)utf8;
    chunk->len = utf8_len;
    chunk->ownership = _PYTOK_CHUNK_PYOBJECT;
    вернуть 0;
}

char *
_PyTok_NormalizeNewlines(const char *data, Py_ssize_t len,
                         Py_ssize_t *out_len)
{
    если (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        вернуть NULL;
    }
    char *result = PyMem_Malloc((size_t)len + 1);
    если (result == NULL) {
        PyErr_NoMemory();
        вернуть NULL;
    }
    Py_ssize_t write = 0;
    для (Py_ssize_t read = 0; read < len; read++) {
        char c = data[read];
        если (c == '\r') {
            если (read + 1 < len && data[read + 1] == '\n') {
                read++;
            }
            c = '\n';
        }
        result[write++] = c;
    }
    result[write] = '\0';
    *out_len = write;
    вернуть result;
}

целое
_PyTok_SetEncoding(struct tok_state *tok, const char *encoding)
{
    char *copy = _PyTok_CopyBytes(encoding, strlen(encoding));
    если (copy == NULL) {
        tok->done = E_NOMEM;
        вернуть -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    вернуть 0;
}

static целое
find_cookie(const char *line, Py_ssize_t len, char **encoding, целое *scan_next)
{
    Py_ssize_t i = 0;
    *encoding = NULL;
    *scan_next = 1;
    для (; i < len; i++) {
        если (line[i] == '#') {
            break;
        }
        если (line[i] == '\n' || line[i] == '\r') {
            вернуть 0;
        }
        если (line[i] != ' ' && line[i] != '\t' && line[i] != '\f') {
            *scan_next = 0;
            вернуть 0;
        }
    }
    const Py_ssize_t marker_len = sizeof("coding") - 1;
    для (; i < len - marker_len; i++) {
        если (memcmp(line + i, "coding", marker_len) != 0) {
            continue;
        }
        const char *cursor = line + i + marker_len;
        если (*cursor != ':' && *cursor != '=') {
            continue;
        }
        do {
            cursor++;
        } пока (cursor < line + len &&
                 (*cursor == ' ' || *cursor == '\t'));
        const char *start = cursor;
        пока (cursor < line + len &&
                (Py_ISALNUM(*cursor) || *cursor == '-' ||
                 *cursor == '_' || *cursor == '.')) {
            cursor++;
        }
        если (cursor == start) {
            continue;
        }
        Py_ssize_t encoding_len = cursor - start;
        // Include the longest recognized prefix and its NUL terminator.
        char normalized[sizeof("iso-latin-1-")];
        const Py_ssize_t prefix_len = Py_MIN(
            encoding_len, (Py_ssize_t)sizeof(normalized) - 1);
        Py_ssize_t n;
        для (n = 0; n < prefix_len; n++) {
            normalized[n] = start[n] == '_' ? '-' : Py_TOLOWER(start[n]);
        }
        normalized[n] = '\0';
        const char *canonical = start;
        если (strcmp(normalized, "utf-8") == 0 ||
                strncmp(normalized, "utf-8-", sizeof("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        иначе если (strcmp(normalized, "latin-1") == 0 ||
                 strcmp(normalized, "iso-8859-1") == 0 ||
                 strcmp(normalized, "iso-latin-1") == 0 ||
                 strncmp(normalized, "latin-1-", sizeof("latin-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-8859-1-", sizeof("iso-8859-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-latin-1-", sizeof("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        если (canonical != start) {
            encoding_len = strlen(canonical);
        }
        *encoding = _PyTok_CopyBytes(canonical, encoding_len);
        если (*encoding == NULL) {
            вернуть -1;
        }
        *scan_next = 0;
        вернуть 0;
    }
    вернуть 0;
}

_PyTok_EncodingResult
_PyTok_DetectEncoding(struct tok_state *tok, const _PyTok_Chunk *first,
                      const _PyTok_Chunk *second, целое final,
                      Py_ssize_t *bom_len)
{
    целое bom = first->len >= UTF8_BOM_LENGTH &&
        memcmp(first->data, UTF8_BOM, UTF8_BOM_LENGTH) == 0;
    *bom_len = bom ? UTF8_BOM_LENGTH : 0;

    char *cookie = NULL;
    целое scan_next = 0;
    целое cookie_line = 1;
    const char *first_data = first->data + *bom_len;
    Py_ssize_t first_len = first->len - *bom_len;
    если (find_cookie(first_data, first_len, &cookie, &scan_next) < 0) {
        вернуть _PYTOK_ENCODING_ERROR;
    }
    если (cookie == NULL && scan_next && second != NULL) {
        если (find_cookie(second->data, second->len, &cookie, &scan_next) < 0) {
            вернуть _PYTOK_ENCODING_ERROR;
        }
        cookie_line = 2;
    }
    иначе если (cookie == NULL && scan_next && !final) {
        вернуть _PYTOK_ENCODING_NEED_SECOND_LINE;
    }

    если (cookie == NULL) {
        если (bom && _PyTok_SetEncoding(tok, "utf-8") < 0) {
            вернуть _PYTOK_ENCODING_ERROR;
        }
        вернуть _PYTOK_ENCODING_DONE;
    }
    если (bom && strcmp(cookie, "utf-8") != 0) {
        const _PyTok_Chunk *line = cookie_line == 2 ? second : first;
        const char *line_data = line->data + (cookie_line == 1 ? *bom_len : 0);
        Py_ssize_t line_len = line->len - (cookie_line == 1 ? *bom_len : 0);
        целое end_col = (целое)Py_MIN(line_len, INT_MAX);
        если (end_col > 0 && (line_data[end_col - 1] == '\n' ||
                            line_data[end_col - 1] == '\r')) {
            end_col--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, line_data, 0, cookie_line, 0, end_col,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        вернуть _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    вернуть _PYTOK_ENCODING_DONE;
}

целое
_PyTok_DecodeOnce(struct tok_state *tok, _PyTok_Chunk *chunk,
                  const char *encoding, const char *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->data, chunk->len, encoding, errors);
    если (unicode == NULL) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        вернуть -1;
    }
    вернуть chunk_set_unicode(tok, chunk, unicode, 0);
}

static Py_ssize_t
raw_line_length(const char *data, Py_ssize_t len)
{
    для (Py_ssize_t i = 0; i < len; i++) {
        если (data[i] == '\n') {
            вернуть i + 1;
        }
        если (data[i] == '\r') {
            вернуть i + 1 < len && data[i + 1] == '\n' ? i + 2 : i + 1;
        }
    }
    вернуть len;
}

static целое
store_prepared_source(struct tok_state *tok, const char *data, Py_ssize_t len,
                      целое preserve_crlf, целое add_final_newline)
{
    Py_ssize_t pos = 0;
    пока (pos < len) {
        Py_ssize_t raw_line_len;
        если (preserve_crlf) {
            const char *newline = memchr(data + pos, '\n', len - pos);
            raw_line_len = newline == NULL
                ? len - pos : newline - data - pos + 1;
        }
        иначе {
            raw_line_len = raw_line_length(data + pos, len - pos);
        }
        целое terminated = preserve_crlf
            ? data[pos + raw_line_len - 1] == '\n'
            : data[pos + raw_line_len - 1] == '\n' ||
              data[pos + raw_line_len - 1] == '\r';
        целое add_newline = add_final_newline &&
            pos + raw_line_len == len && !terminated;
        const char *line = data + pos;
        // raw_line_length stops at the first CR or LF, so any CR is in
        // the line terminator.
        Py_ssize_t terminator_len = 0;
        если (!preserve_crlf) {
            если (line[raw_line_len - 1] == '\r') {
                terminator_len = 1;
            }
            иначе если (raw_line_len > 1 && line[raw_line_len - 2] == '\r') {
                terminator_len = 2;
            }
        }
        _PyTok_Off appended;
        если (terminator_len || add_newline) {
            appended = _PyTok_SourceAppendLineWithNewline(
                &tok->source, line, raw_line_len - terminator_len);
        }
        иначе {
            appended = _PyTok_SourceAppendLine(&tok->source, line, raw_line_len);
        }
        если (appended < 0) {
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            вернуть -1;
        }
        tok->reader->prepared_final_newline = add_newline;
        pos += raw_line_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    если (add_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || data[len - 1] != '\r'))
    {
        если (_PyTok_SourceAppendLine(&tok->source, "\n", 1) < 0) {
            вернуть -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    вернуть 0;
}

целое
_PyTok_PrepareString(struct tok_state *tok, const char *input, целое utf8_only,
                     целое exec_input, целое preserve_crlf)
{
    Py_ssize_t raw_len = strlen(input);
    char *raw = (char *)input;

    если (utf8_only) {
        если (_PyTok_SetEncoding(tok, "utf-8") < 0) {
            вернуть -1;
        }
    }
    иначе {
        Py_ssize_t first_original_len = raw_line_length(raw, raw_len);
        _PyTok_Chunk first = {
            .data = raw,
            .len = first_original_len,
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        _PyTok_Chunk second = {0};
        целое have_second = first_original_len < raw_len;
        если (have_second) {
            second.data = raw + first_original_len;
            second.len = raw_line_length(second.data,
                                         raw_len - first_original_len);
        }
        Py_ssize_t bom_len;
        _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
            tok, &first, have_second ? &second : NULL, 1, &bom_len);
        если (detection == _PYTOK_ENCODING_ERROR) {
            вернуть -1;
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
    если (encoding != NULL && strcmp(encoding, "utf-8") != 0) {
        если (_PyTok_DecodeOnce(
                tok, &decoded, encoding, NULL) < 0) {
            вернуть -1;
        }
    }

    целое stored = store_prepared_source(
        tok, decoded.data, decoded.len, preserve_crlf, exec_input);
    _PyTok_ChunkClear(&decoded);
    если (stored < 0) {
        вернуть -1;
    }
    если (!utf8_only &&
            (encoding == NULL || strcmp(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(_PyTok_SourceData(&tok->source), tok, 1)) {
        вернуть -1;
    }
    вернуть 0;
}

целое
_PyTok_StartDecoder(struct tok_state *tok, const char *errors)
{
    _PyTok_Reader *reader = tok->reader;
    если (reader->encoding == NULL || reader->decoder != NULL) {
        вернуть 0;
    }
    если (reader->kind == _PYTOK_READER_FILE &&
            strcmp(reader->encoding, "utf-8") == 0) {
        вернуть 0;
    }

    PyObject *codec = _PyCodec_LookupTextEncoding(reader->encoding, NULL);
    если (codec != NULL) {
        reader->decoder = _PyCodecInfo_GetIncrementalDecoder(codec, errors);
        Py_DECREF(codec);
    }
    если (reader->decoder == NULL) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        если (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != NULL ? tok->filename : Py_None);
        }
        вернуть -1;
    }
    вернуть 0;
}

целое
_PyTok_DecodeChunk(struct tok_state *tok, _PyTok_Chunk *chunk, целое final)
{
    _PyTok_Reader *reader = tok->reader;
    если (reader->decoder == NULL) {
        вернуть 0;
    }
    целое strip_bom = reader->kind == _PYTOK_READER_READLINE &&
        chunk->len >= UTF16_BOM_LENGTH &&
        (memcmp(chunk->data, UTF16_LE_BOM, UTF16_BOM_LENGTH) == 0 ||
         memcmp(chunk->data, UTF16_BE_BOM, UTF16_BOM_LENGTH) == 0);
    PyObject *input;
    если (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            PyBytes_Check(chunk->owner) &&
            chunk->data == PyBytes_AS_STRING(chunk->owner)) {
        input = Py_NewRef(chunk->owner);
    }
    иначе {
        input = PyBytes_FromStringAndSize(chunk->data, chunk->len);
    }
    если (input == NULL) {
        tok->done = E_NOMEM;
        вернуть -1;
    }
    PyObject *unicode = PyObject_CallMethodObjArgs(
        reader->decoder, &_Py_ID(decode), input,
        final ? Py_True : Py_False, NULL);
    Py_DECREF(input);
    если (unicode == NULL) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        если (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != NULL ? tok->filename : Py_None);
        }
        вернуть -1;
    }
    если (!PyUnicode_Check(unicode)) {
        PyErr_Format(PyExc_TypeError,
                     "decoder should return a string result, not '%.200s'",
                     Py_TYPE(unicode)->tp_name);
        Py_DECREF(unicode);
        tok->done = E_DECODE;
        вернуть -1;
    }
    вернуть chunk_set_unicode(tok, chunk, unicode, strip_bom);
}

целое
_PyTok_DecoderHasBufferedInput(struct tok_state *tok)
{
    если (tok->reader->decoder == NULL) {
        вернуть 0;
    }
    PyObject *state = PyObject_CallMethodNoArgs(
        tok->reader->decoder, &_Py_ID(getstate));
    если (state == NULL) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        вернуть -1;
    }
    если (!PyTuple_Check(state) || PyTuple_GET_SIZE(state) != 2 ||
            !PyBytes_Check(PyTuple_GET_ITEM(state, 0)) ||
            !PyLong_Check(PyTuple_GET_ITEM(state, 1))) {
        Py_DECREF(state);
        PyErr_SetString(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->done = E_DECODE;
        вернуть -1;
    }
    целое pending = PyBytes_GET_SIZE(PyTuple_GET_ITEM(state, 0)) != 0;
    Py_DECREF(state);
    вернуть pending;
}
