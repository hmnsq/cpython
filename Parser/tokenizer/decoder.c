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
#define UTF16_BOM_LENGTH ((Py_ssize_t)(parimaa_na(UTF16_LE_BOM) - 1))
#define UTF8_BOM "\xef\xbb\xbf"
#define UTF8_BOM_LENGTH ((Py_ssize_t)(parimaa_na(UTF8_BOM) - 1))

var_na *
_PyTok_CopyBytes(const var_na *data, Py_ssize_t len)
{
    ប្រសិនបើ (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        ត្រឡប់ NULL;
    }
    var_na *copy = PyMem_Malloc((size_t)len + 1);
    ប្រសិនបើ (copy == NULL) {
        PyErr_NoMemory();
        ត្រឡប់ NULL;
    }
    memcpy(copy, data, (size_t)len);
    copy[len] = '\0';
    ត្រឡប់ copy;
}

static void
chunk_release_data(_PyTok_Chunk *chunk)
{
    switch (chunk->ownership) {
        pak_sa _PYTOK_CHUNK_BORROWED:
            bha_nga;
        pak_sa _PYTOK_CHUNK_PYMEM:
            PyMem_Free(chunk->data);
            bha_nga;
        pak_sa _PYTOK_CHUNK_PYOBJECT:
            Py_DECREF(chunk->owner);
            bha_nga;
    }
}

void
_PyTok_ChunkClear(_PyTok_Chunk *chunk)
{
    chunk_release_data(chunk);
    *chunk = (_PyTok_Chunk){0};
}

static ចំនួនគត់
chunk_set_unicode(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk,
                  PyObject *unicode, ចំនួនគត់ strip_bom)
{
    Py_ssize_t utf8_len;
    const var_na *utf8 = PyUnicode_AsUTF8AndSize(unicode, &utf8_len);
    ប្រសិនបើ (utf8 == NULL) {
        Py_DECREF(unicode);
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        ត្រឡប់ -1;
    }
    ប្រសិនបើ (strip_bom && PyUnicode_GET_LENGTH(unicode) > 0 &&
            PyUnicode_ReadChar(unicode, 0) == UNICODE_BOM) {
        utf8 += UTF8_BOM_LENGTH;
        utf8_len -= UTF8_BOM_LENGTH;
    }
    chunk_release_data(chunk);
    chunk->owner = unicode;
    chunk->data = (var_na *)utf8;
    chunk->len = utf8_len;
    chunk->ownership = _PYTOK_CHUNK_PYOBJECT;
    ត្រឡប់ 0;
}

var_na *
_PyTok_NormalizeNewlines(const var_na *data, Py_ssize_t len,
                         Py_ssize_t *out_len)
{
    ប្រសិនបើ (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        ត្រឡប់ NULL;
    }
    var_na *result = PyMem_Malloc((size_t)len + 1);
    ប្រសិនបើ (result == NULL) {
        PyErr_NoMemory();
        ត្រឡប់ NULL;
    }
    Py_ssize_t write = 0;
    សម្រាប់ (Py_ssize_t read = 0; read < len; read++) {
        var_na c = data[read];
        ប្រសិនបើ (c == '\r') {
            ប្រសិនបើ (read + 1 < len && data[read + 1] == '\n') {
                read++;
            }
            c = '\n';
        }
        result[write++] = c;
    }
    result[write] = '\0';
    *out_len = write;
    ត្រឡប់ result;
}

ចំនួនគត់
_PyTok_SetEncoding(sa_nracanaa tok_state *tok, const var_na *encoding)
{
    var_na *copy = _PyTok_CopyBytes(encoding, strlen(encoding));
    ប្រសិនបើ (copy == NULL) {
        tok->done = E_NOMEM;
        ត្រឡប់ -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    ត្រឡប់ 0;
}

static ចំនួនគត់
find_cookie(const var_na *line, Py_ssize_t len, var_na **encoding, ចំនួនគត់ *scan_next)
{
    Py_ssize_t i = 0;
    *encoding = NULL;
    *scan_next = 1;
    សម្រាប់ (; i < len; i++) {
        ប្រសិនបើ (line[i] == '#') {
            bha_nga;
        }
        ប្រសិនបើ (line[i] == '\n' || line[i] == '\r') {
            ត្រឡប់ 0;
        }
        ប្រសិនបើ (line[i] != ' ' && line[i] != '\t' && line[i] != '\f') {
            *scan_next = 0;
            ត្រឡប់ 0;
        }
    }
    const Py_ssize_t marker_len = parimaa_na("coding") - 1;
    សម្រាប់ (; i < len - marker_len; i++) {
        ប្រសិនបើ (memcmp(line + i, "coding", marker_len) != 0) {
            continue;
        }
        const var_na *cursor = line + i + marker_len;
        ប្រសិនបើ (*cursor != ':' && *cursor != '=') {
            continue;
        }
        do {
            cursor++;
        } ខណៈពេល (cursor < line + len &&
                 (*cursor == ' ' || *cursor == '\t'));
        const var_na *start = cursor;
        ខណៈពេល (cursor < line + len &&
                (Py_ISALNUM(*cursor) || *cursor == '-' ||
                 *cursor == '_' || *cursor == '.')) {
            cursor++;
        }
        ប្រសិនបើ (cursor == start) {
            continue;
        }
        Py_ssize_t encoding_len = cursor - start;
        // Include the longest recognized prefix and its NUL terminator.
        var_na normalized[parimaa_na("iso-latin-1-")];
        const Py_ssize_t prefix_len = Py_MIN(
            encoding_len, (Py_ssize_t)parimaa_na(normalized) - 1);
        Py_ssize_t n;
        សម្រាប់ (n = 0; n < prefix_len; n++) {
            normalized[n] = start[n] == '_' ? '-' : Py_TOLOWER(start[n]);
        }
        normalized[n] = '\0';
        const var_na *canonical = start;
        ប្រសិនបើ (strcmp(normalized, "utf-8") == 0 ||
                strncmp(normalized, "utf-8-", parimaa_na("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        បើមិនដូច្នេះ ប្រសិនបើ (strcmp(normalized, "latin-1") == 0 ||
                 strcmp(normalized, "iso-8859-1") == 0 ||
                 strcmp(normalized, "iso-latin-1") == 0 ||
                 strncmp(normalized, "latin-1-", parimaa_na("latin-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-8859-1-", parimaa_na("iso-8859-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-latin-1-", parimaa_na("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        ប្រសិនបើ (canonical != start) {
            encoding_len = strlen(canonical);
        }
        *encoding = _PyTok_CopyBytes(canonical, encoding_len);
        ប្រសិនបើ (*encoding == NULL) {
            ត្រឡប់ -1;
        }
        *scan_next = 0;
        ត្រឡប់ 0;
    }
    ត្រឡប់ 0;
}

_PyTok_EncodingResult
_PyTok_DetectEncoding(sa_nracanaa tok_state *tok, const _PyTok_Chunk *first,
                      const _PyTok_Chunk *second, ចំនួនគត់ final,
                      Py_ssize_t *bom_len)
{
    ចំនួនគត់ bom = first->len >= UTF8_BOM_LENGTH &&
        memcmp(first->data, UTF8_BOM, UTF8_BOM_LENGTH) == 0;
    *bom_len = bom ? UTF8_BOM_LENGTH : 0;

    var_na *cookie = NULL;
    ចំនួនគត់ scan_next = 0;
    ចំនួនគត់ cookie_line = 1;
    const var_na *first_data = first->data + *bom_len;
    Py_ssize_t first_len = first->len - *bom_len;
    ប្រសិនបើ (find_cookie(first_data, first_len, &cookie, &scan_next) < 0) {
        ត្រឡប់ _PYTOK_ENCODING_ERROR;
    }
    ប្រសិនបើ (cookie == NULL && scan_next && second != NULL) {
        ប្រសិនបើ (find_cookie(second->data, second->len, &cookie, &scan_next) < 0) {
            ត្រឡប់ _PYTOK_ENCODING_ERROR;
        }
        cookie_line = 2;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (cookie == NULL && scan_next && !final) {
        ត្រឡប់ _PYTOK_ENCODING_NEED_SECOND_LINE;
    }

    ប្រសិនបើ (cookie == NULL) {
        ប្រសិនបើ (bom && _PyTok_SetEncoding(tok, "utf-8") < 0) {
            ត្រឡប់ _PYTOK_ENCODING_ERROR;
        }
        ត្រឡប់ _PYTOK_ENCODING_DONE;
    }
    ប្រសិនបើ (bom && strcmp(cookie, "utf-8") != 0) {
        const _PyTok_Chunk *line = cookie_line == 2 ? second : first;
        const var_na *line_data = line->data + (cookie_line == 1 ? *bom_len : 0);
        Py_ssize_t line_len = line->len - (cookie_line == 1 ? *bom_len : 0);
        ចំនួនគត់ end_col = (ចំនួនគត់)Py_MIN(line_len, INT_MAX);
        ប្រសិនបើ (end_col > 0 && (line_data[end_col - 1] == '\n' ||
                            line_data[end_col - 1] == '\r')) {
            end_col--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, line_data, 0, cookie_line, 0, end_col,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        ត្រឡប់ _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    ត្រឡប់ _PYTOK_ENCODING_DONE;
}

ចំនួនគត់
_PyTok_DecodeOnce(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk,
                  const var_na *encoding, const var_na *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->data, chunk->len, encoding, errors);
    ប្រសិនបើ (unicode == NULL) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        ត្រឡប់ -1;
    }
    ត្រឡប់ chunk_set_unicode(tok, chunk, unicode, 0);
}

static Py_ssize_t
raw_line_length(const var_na *data, Py_ssize_t len)
{
    សម្រាប់ (Py_ssize_t i = 0; i < len; i++) {
        ប្រសិនបើ (data[i] == '\n') {
            ត្រឡប់ i + 1;
        }
        ប្រសិនបើ (data[i] == '\r') {
            ត្រឡប់ i + 1 < len && data[i + 1] == '\n' ? i + 2 : i + 1;
        }
    }
    ត្រឡប់ len;
}

static ចំនួនគត់
store_prepared_source(sa_nracanaa tok_state *tok, const var_na *data, Py_ssize_t len,
                      ចំនួនគត់ preserve_crlf, ចំនួនគត់ add_final_newline)
{
    Py_ssize_t pos = 0;
    ខណៈពេល (pos < len) {
        Py_ssize_t raw_line_len;
        ប្រសិនបើ (preserve_crlf) {
            const var_na *newline = memchr(data + pos, '\n', len - pos);
            raw_line_len = newline == NULL
                ? len - pos : newline - data - pos + 1;
        }
        បើមិនដូច្នេះ {
            raw_line_len = raw_line_length(data + pos, len - pos);
        }
        ចំនួនគត់ terminated = preserve_crlf
            ? data[pos + raw_line_len - 1] == '\n'
            : data[pos + raw_line_len - 1] == '\n' ||
              data[pos + raw_line_len - 1] == '\r';
        ចំនួនគត់ add_newline = add_final_newline &&
            pos + raw_line_len == len && !terminated;
        const var_na *line = data + pos;
        // raw_line_length stops at the first CR or LF, so any CR is in
        // the line terminator.
        Py_ssize_t terminator_len = 0;
        ប្រសិនបើ (!preserve_crlf) {
            ប្រសិនបើ (line[raw_line_len - 1] == '\r') {
                terminator_len = 1;
            }
            បើមិនដូច្នេះ ប្រសិនបើ (raw_line_len > 1 && line[raw_line_len - 2] == '\r') {
                terminator_len = 2;
            }
        }
        _PyTok_Off appended;
        ប្រសិនបើ (terminator_len || add_newline) {
            appended = _PyTok_SourceAppendLineWithNewline(
                &tok->source, line, raw_line_len - terminator_len);
        }
        បើមិនដូច្នេះ {
            appended = _PyTok_SourceAppendLine(&tok->source, line, raw_line_len);
        }
        ប្រសិនបើ (appended < 0) {
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            ត្រឡប់ -1;
        }
        tok->reader->prepared_final_newline = add_newline;
        pos += raw_line_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    ប្រសិនបើ (add_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || data[len - 1] != '\r'))
    {
        ប្រសិនបើ (_PyTok_SourceAppendLine(&tok->source, "\n", 1) < 0) {
            ត្រឡប់ -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    ត្រឡប់ 0;
}

ចំនួនគត់
_PyTok_PrepareString(sa_nracanaa tok_state *tok, const var_na *input, ចំនួនគត់ utf8_only,
                     ចំនួនគត់ exec_input, ចំនួនគត់ preserve_crlf)
{
    Py_ssize_t raw_len = strlen(input);
    var_na *raw = (var_na *)input;

    ប្រសិនបើ (utf8_only) {
        ប្រសិនបើ (_PyTok_SetEncoding(tok, "utf-8") < 0) {
            ត្រឡប់ -1;
        }
    }
    បើមិនដូច្នេះ {
        Py_ssize_t first_original_len = raw_line_length(raw, raw_len);
        _PyTok_Chunk first = {
            .data = raw,
            .len = first_original_len,
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        _PyTok_Chunk second = {0};
        ចំនួនគត់ have_second = first_original_len < raw_len;
        ប្រសិនបើ (have_second) {
            second.data = raw + first_original_len;
            second.len = raw_line_length(second.data,
                                         raw_len - first_original_len);
        }
        Py_ssize_t bom_len;
        _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
            tok, &first, have_second ? &second : NULL, 1, &bom_len);
        ប្រសិនបើ (detection == _PYTOK_ENCODING_ERROR) {
            ត្រឡប់ -1;
        }
        raw += bom_len;
        raw_len -= bom_len;
    }

    _PyTok_Chunk decoded = {
        .data = raw,
        .len = raw_len,
        .ownership = _PYTOK_CHUNK_BORROWED,
    };
    const var_na *encoding = tok->reader->encoding;
    ប្រសិនបើ (encoding != NULL && strcmp(encoding, "utf-8") != 0) {
        ប្រសិនបើ (_PyTok_DecodeOnce(
                tok, &decoded, encoding, NULL) < 0) {
            ត្រឡប់ -1;
        }
    }

    ចំនួនគត់ stored = store_prepared_source(
        tok, decoded.data, decoded.len, preserve_crlf, exec_input);
    _PyTok_ChunkClear(&decoded);
    ប្រសិនបើ (stored < 0) {
        ត្រឡប់ -1;
    }
    ប្រសិនបើ (!utf8_only &&
            (encoding == NULL || strcmp(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(_PyTok_SourceData(&tok->source), tok, 1)) {
        ត្រឡប់ -1;
    }
    ត្រឡប់ 0;
}

ចំនួនគត់
_PyTok_StartDecoder(sa_nracanaa tok_state *tok, const var_na *errors)
{
    _PyTok_Reader *reader = tok->reader;
    ប្រសិនបើ (reader->encoding == NULL || reader->decoder != NULL) {
        ត្រឡប់ 0;
    }
    ប្រសិនបើ (reader->kind == _PYTOK_READER_FILE &&
            strcmp(reader->encoding, "utf-8") == 0) {
        ត្រឡប់ 0;
    }

    PyObject *codec = _PyCodec_LookupTextEncoding(reader->encoding, NULL);
    ប្រសិនបើ (codec != NULL) {
        reader->decoder = _PyCodecInfo_GetIncrementalDecoder(codec, errors);
        Py_DECREF(codec);
    }
    ប្រសិនបើ (reader->decoder == NULL) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        ប្រសិនបើ (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != NULL ? tok->filename : Py_None);
        }
        ត្រឡប់ -1;
    }
    ត្រឡប់ 0;
}

ចំនួនគត់
_PyTok_DecodeChunk(sa_nracanaa tok_state *tok, _PyTok_Chunk *chunk, ចំនួនគត់ final)
{
    _PyTok_Reader *reader = tok->reader;
    ប្រសិនបើ (reader->decoder == NULL) {
        ត្រឡប់ 0;
    }
    ចំនួនគត់ strip_bom = reader->kind == _PYTOK_READER_READLINE &&
        chunk->len >= UTF16_BOM_LENGTH &&
        (memcmp(chunk->data, UTF16_LE_BOM, UTF16_BOM_LENGTH) == 0 ||
         memcmp(chunk->data, UTF16_BE_BOM, UTF16_BOM_LENGTH) == 0);
    PyObject *input;
    ប្រសិនបើ (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            PyBytes_Check(chunk->owner) &&
            chunk->data == PyBytes_AS_STRING(chunk->owner)) {
        input = Py_NewRef(chunk->owner);
    }
    បើមិនដូច្នេះ {
        input = PyBytes_FromStringAndSize(chunk->data, chunk->len);
    }
    ប្រសិនបើ (input == NULL) {
        tok->done = E_NOMEM;
        ត្រឡប់ -1;
    }
    PyObject *unicode = PyObject_CallMethodObjArgs(
        reader->decoder, &_Py_ID(decode), input,
        final ? Py_True : Py_False, NULL);
    Py_DECREF(input);
    ប្រសិនបើ (unicode == NULL) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        ប្រសិនបើ (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != NULL ? tok->filename : Py_None);
        }
        ត្រឡប់ -1;
    }
    ប្រសិនបើ (!PyUnicode_Check(unicode)) {
        PyErr_Format(PyExc_TypeError,
                     "decoder should return a string result, not '%.200s'",
                     Py_TYPE(unicode)->tp_name);
        Py_DECREF(unicode);
        tok->done = E_DECODE;
        ត្រឡប់ -1;
    }
    ត្រឡប់ chunk_set_unicode(tok, chunk, unicode, strip_bom);
}

ចំនួនគត់
_PyTok_DecoderHasBufferedInput(sa_nracanaa tok_state *tok)
{
    ប្រសិនបើ (tok->reader->decoder == NULL) {
        ត្រឡប់ 0;
    }
    PyObject *state = PyObject_CallMethodNoArgs(
        tok->reader->decoder, &_Py_ID(getstate));
    ប្រសិនបើ (state == NULL) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        ត្រឡប់ -1;
    }
    ប្រសិនបើ (!PyTuple_Check(state) || PyTuple_GET_SIZE(state) != 2 ||
            !PyBytes_Check(PyTuple_GET_ITEM(state, 0)) ||
            !PyLong_Check(PyTuple_GET_ITEM(state, 1))) {
        Py_DECREF(state);
        PyErr_SetString(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->done = E_DECODE;
        ត្រឡប់ -1;
    }
    ចំនួនគត់ pending = PyBytes_GET_SIZE(PyTuple_GET_ITEM(state, 0)) != 0;
    Py_DECREF(state);
    ត្រឡប់ pending;
}
