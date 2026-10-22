#ಅನ್ತರ್ಭಾವ "Python.h"
#ಅನ್ತರ್ಭಾವ "pycore_codecs.h"
#ಅನ್ತರ್ಭಾವ "pycore_global_strings.h"
#ಅನ್ತರ್ಭಾವ "pycore_runtime.h"
#ಅನ್ತರ್ಭಾವ "errcode.h"

#ಅನ್ತರ್ಭಾವ "reader_internal.h"
#ಅನ್ತರ್ಭಾವ "helpers.h"
#ಅನ್ತರ್ಭಾವ "../lexer/state.h"

#define UNICODE_BOM 0xFEFF
#define UTF16_LE_BOM "\xff\xfe"
#define UTF16_BE_BOM "\xfe\xff"
#define UTF16_BOM_LENGTH ((Py_ssize_t)(ಪರಿಮಾಣ(UTF16_LE_BOM) - 1))
#define UTF8_BOM "\xef\xbb\xbf"
#define UTF8_BOM_LENGTH ((Py_ssize_t)(ಪರಿಮಾಣ(UTF8_BOM) - 1))

ವರ್ಣ *
_PyTok_CopyBytes(ಧ್ರುವ ವರ್ಣ *data, Py_ssize_t len)
{
    ಯದಿ (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ವರ್ಣ *copy = PyMem_Malloc((size_t)len + 1);
    ಯದಿ (copy == ರಿಕ್ತ) {
        PyErr_NoMemory();
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    memcpy(copy, data, (size_t)len);
    copy[len] = '\0';
    ಪ್ರತ್ಯಾಗಮ copy;
}

ಸ್ಥಿರ ಶೂನ್ಯ
chunk_release_data(_PyTok_Chunk *chunk)
{
    ವಿಕಲ್ಪ (chunk->ownership) {
        ಪಕ್ಷ _PYTOK_CHUNK_BORROWED:
            ಭಙ್ಗ;
        ಪಕ್ಷ _PYTOK_CHUNK_PYMEM:
            PyMem_Free(chunk->data);
            ಭಙ್ಗ;
        ಪಕ್ಷ _PYTOK_CHUNK_PYOBJECT:
            Py_DECREF(chunk->owner);
            ಭಙ್ಗ;
    }
}

ಶೂನ್ಯ
_PyTok_ChunkClear(_PyTok_Chunk *chunk)
{
    chunk_release_data(chunk);
    *chunk = (_PyTok_Chunk){0};
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
chunk_set_unicode(ಸಂರಚನಾ tok_state *tok, _PyTok_Chunk *chunk,
                  PyObject *unicode, ಪೂರ್ಣಾಙ್ಕ strip_bom)
{
    Py_ssize_t utf8_len;
    ಧ್ರುವ ವರ್ಣ *utf8 = PyUnicode_AsUTF8AndSize(unicode, &utf8_len);
    ಯದಿ (utf8 == ರಿಕ್ತ) {
        Py_DECREF(unicode);
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಯದಿ (strip_bom && PyUnicode_GET_LENGTH(unicode) > 0 &&
            PyUnicode_ReadChar(unicode, 0) == UNICODE_BOM) {
        utf8 += UTF8_BOM_LENGTH;
        utf8_len -= UTF8_BOM_LENGTH;
    }
    chunk_release_data(chunk);
    chunk->owner = unicode;
    chunk->data = (ವರ್ಣ *)utf8;
    chunk->len = utf8_len;
    chunk->ownership = _PYTOK_CHUNK_PYOBJECT;
    ಪ್ರತ್ಯಾಗಮ 0;
}

ವರ್ಣ *
_PyTok_NormalizeNewlines(ಧ್ರುವ ವರ್ಣ *data, Py_ssize_t len,
                         Py_ssize_t *out_len)
{
    ಯದಿ (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ವರ್ಣ *result = PyMem_Malloc((size_t)len + 1);
    ಯದಿ (result == ರಿಕ್ತ) {
        PyErr_NoMemory();
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    Py_ssize_t ಲಿಖ = 0;
    ಪ್ರತಿ (Py_ssize_t ಪಠ = 0; ಪಠ < len; ಪಠ++) {
        ವರ್ಣ c = data[ಪಠ];
        ಯದಿ (c == '\r') {
            ಯದಿ (ಪಠ + 1 < len && data[ಪಠ + 1] == '\n') {
                ಪಠ++;
            }
            c = '\n';
        }
        result[ಲಿಖ++] = c;
    }
    result[ಲಿಖ] = '\0';
    *out_len = ಲಿಖ;
    ಪ್ರತ್ಯಾಗಮ result;
}

ಪೂರ್ಣಾಙ್ಕ
_PyTok_SetEncoding(ಸಂರಚನಾ tok_state *tok, ಧ್ರುವ ವರ್ಣ *encoding)
{
    ವರ್ಣ *copy = _PyTok_CopyBytes(encoding, strlen(encoding));
    ಯದಿ (copy == ರಿಕ್ತ) {
        tok->done = E_NOMEM;
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
find_cookie(ಧ್ರುವ ವರ್ಣ *ಸಾಲು, Py_ssize_t len, ವರ್ಣ **encoding, ಪೂರ್ಣಾಙ್ಕ *scan_next)
{
    Py_ssize_t i = 0;
    *encoding = ರಿಕ್ತ;
    *scan_next = 1;
    ಪ್ರತಿ (; i < len; i++) {
        ಯದಿ (ಸಾಲು[i] == '#') {
            ಭಙ್ಗ;
        }
        ಯದಿ (ಸಾಲು[i] == '\n' || ಸಾಲು[i] == '\r') {
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        ಯದಿ (ಸಾಲು[i] != ' ' && ಸಾಲು[i] != '\t' && ಸಾಲು[i] != '\f') {
            *scan_next = 0;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
    }
    ಧ್ರುವ Py_ssize_t marker_len = ಪರಿಮಾಣ("coding") - 1;
    ಪ್ರತಿ (; i < len - marker_len; i++) {
        ಯದಿ (memcmp(ಸಾಲು + i, "coding", marker_len) != 0) {
            ಅನುವರ್ತ;
        }
        ಧ್ರುವ ವರ್ಣ *cursor = ಸಾಲು + i + marker_len;
        ಯದಿ (*cursor != ':' && *cursor != '=') {
            ಅನುವರ್ತ;
        }
        ಕುರು {
            cursor++;
        } ಯಾವತ್ (cursor < ಸಾಲು + len &&
                 (*cursor == ' ' || *cursor == '\t'));
        ಧ್ರುವ ವರ್ಣ *start = cursor;
        ಯಾವತ್ (cursor < ಸಾಲು + len &&
                (Py_ISALNUM(*cursor) || *cursor == '-' ||
                 *cursor == '_' || *cursor == '.')) {
            cursor++;
        }
        ಯದಿ (cursor == start) {
            ಅನುವರ್ತ;
        }
        Py_ssize_t encoding_len = cursor - start;
        // Include the longest recognized prefix and its NUL terminator.
        ವರ್ಣ normalized[ಪರಿಮಾಣ("iso-latin-1-")];
        ಧ್ರುವ Py_ssize_t prefix_len = Py_MIN(
            encoding_len, (Py_ssize_t)ಪರಿಮಾಣ(normalized) - 1);
        Py_ssize_t n;
        ಪ್ರತಿ (n = 0; n < prefix_len; n++) {
            normalized[n] = start[n] == '_' ? '-' : Py_TOLOWER(start[n]);
        }
        normalized[n] = '\0';
        ಧ್ರುವ ವರ್ಣ *canonical = start;
        ಯದಿ (strcmp(normalized, "utf-8") == 0 ||
                strncmp(normalized, "utf-8-", ಪರಿಮಾಣ("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        ಅನ್ಯಥಾ ಯದಿ (strcmp(normalized, "latin-1") == 0 ||
                 strcmp(normalized, "iso-8859-1") == 0 ||
                 strcmp(normalized, "iso-latin-1") == 0 ||
                 strncmp(normalized, "latin-1-", ಪರಿಮಾಣ("latin-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-8859-1-", ಪರಿಮಾಣ("iso-8859-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-latin-1-", ಪರಿಮಾಣ("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        ಯದಿ (canonical != start) {
            encoding_len = strlen(canonical);
        }
        *encoding = _PyTok_CopyBytes(canonical, encoding_len);
        ಯದಿ (*encoding == ರಿಕ್ತ) {
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        *scan_next = 0;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

_PyTok_EncodingResult
_PyTok_DetectEncoding(ಸಂರಚನಾ tok_state *tok, ಧ್ರುವ _PyTok_Chunk *first,
                      ಧ್ರುವ _PyTok_Chunk *second, ಪೂರ್ಣಾಙ್ಕ final,
                      Py_ssize_t *bom_len)
{
    ಪೂರ್ಣಾಙ್ಕ bom = first->len >= UTF8_BOM_LENGTH &&
        memcmp(first->data, UTF8_BOM, UTF8_BOM_LENGTH) == 0;
    *bom_len = bom ? UTF8_BOM_LENGTH : 0;

    ವರ್ಣ *cookie = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ scan_next = 0;
    ಪೂರ್ಣಾಙ್ಕ cookie_ಸಾಲು = 1;
    ಧ್ರುವ ವರ್ಣ *first_data = first->data + *bom_len;
    Py_ssize_t first_len = first->len - *bom_len;
    ಯದಿ (find_cookie(first_data, first_len, &cookie, &scan_next) < 0) {
        ಪ್ರತ್ಯಾಗಮ _PYTOK_ENCODING_ERROR;
    }
    ಯದಿ (cookie == ರಿಕ್ತ && scan_next && second != ರಿಕ್ತ) {
        ಯದಿ (find_cookie(second->data, second->len, &cookie, &scan_next) < 0) {
            ಪ್ರತ್ಯಾಗಮ _PYTOK_ENCODING_ERROR;
        }
        cookie_ಸಾಲು = 2;
    }
    ಅನ್ಯಥಾ ಯದಿ (cookie == ರಿಕ್ತ && scan_next && !final) {
        ಪ್ರತ್ಯಾಗಮ PYTOK_ENCODING_NEED_SECOND_ಸಾಲು;
    }

    ಯದಿ (cookie == ರಿಕ್ತ) {
        ಯದಿ (bom && _PyTok_SetEncoding(tok, "utf-8") < 0) {
            ಪ್ರತ್ಯಾಗಮ _PYTOK_ENCODING_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ _PYTOK_ENCODING_DONE;
    }
    ಯದಿ (bom && strcmp(cookie, "utf-8") != 0) {
        ಧ್ರುವ _PyTok_Chunk *ಸಾಲು = cookie_ಸಾಲು == 2 ? second : first;
        ಧ್ರುವ ವರ್ಣ *ಸಾಲು_data = ಸಾಲು->data + (cookie_ಸಾಲು == 1 ? *bom_len : 0);
        Py_ssize_t ಸಾಲು_len = ಸಾಲು->len - (cookie_ಸಾಲು == 1 ? *bom_len : 0);
        ಪೂರ್ಣಾಙ್ಕ end_col = (ಪೂರ್ಣಾಙ್ಕ)Py_MIN(ಸಾಲು_len, INT_MAX);
        ಯದಿ (end_col > 0 && (ಸಾಲು_data[end_col - 1] == '\n' ||
                            ಸಾಲು_data[end_col - 1] == '\r')) {
            end_col--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, ಸಾಲು_data, 0, cookie_ಸಾಲು, 0, end_col,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        ಪ್ರತ್ಯಾಗಮ _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    ಪ್ರತ್ಯಾಗಮ _PYTOK_ENCODING_DONE;
}

ಪೂರ್ಣಾಙ್ಕ
_PyTok_DecodeOnce(ಸಂರಚನಾ tok_state *tok, _PyTok_Chunk *chunk,
                  ಧ್ರುವ ವರ್ಣ *encoding, ಧ್ರುವ ವರ್ಣ *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->data, chunk->len, encoding, errors);
    ಯದಿ (unicode == ರಿಕ್ತ) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಪ್ರತ್ಯಾಗಮ chunk_set_unicode(tok, chunk, unicode, 0);
}

ಸ್ಥಿರ Py_ssize_t
raw_ಸಾಲು_length(ಧ್ರುವ ವರ್ಣ *data, Py_ssize_t len)
{
    ಪ್ರತಿ (Py_ssize_t i = 0; i < len; i++) {
        ಯದಿ (data[i] == '\n') {
            ಪ್ರತ್ಯಾಗಮ i + 1;
        }
        ಯದಿ (data[i] == '\r') {
            ಪ್ರತ್ಯಾಗಮ i + 1 < len && data[i + 1] == '\n' ? i + 2 : i + 1;
        }
    }
    ಪ್ರತ್ಯಾಗಮ len;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
store_prepared_source(ಸಂರಚನಾ tok_state *tok, ಧ್ರುವ ವರ್ಣ *data, Py_ssize_t len,
                      ಪೂರ್ಣಾಙ್ಕ preserve_crlf, ಪೂರ್ಣಾಙ್ಕ add_final_newline)
{
    Py_ssize_t pos = 0;
    ಯಾವತ್ (pos < len) {
        Py_ssize_t raw_ಸಾಲು_len;
        ಯದಿ (preserve_crlf) {
            ಧ್ರುವ ವರ್ಣ *newline = memchr(data + pos, '\n', len - pos);
            raw_ಸಾಲು_len = newline == ರಿಕ್ತ
                ? len - pos : newline - data - pos + 1;
        }
        ಅನ್ಯಥಾ {
            raw_ಸಾಲು_len = raw_ಸಾಲು_length(data + pos, len - pos);
        }
        ಪೂರ್ಣಾಙ್ಕ terminated = preserve_crlf
            ? data[pos + raw_ಸಾಲು_len - 1] == '\n'
            : data[pos + raw_ಸಾಲು_len - 1] == '\n' ||
              data[pos + raw_ಸಾಲು_len - 1] == '\r';
        ಪೂರ್ಣಾಙ್ಕ add_newline = add_final_newline &&
            pos + raw_ಸಾಲು_len == len && !terminated;
        ಧ್ರುವ ವರ್ಣ *ಸಾಲು = data + pos;
        // raw_line_length stops at the first CR or LF, so any CR is in
        // the line terminator.
        Py_ssize_t terminator_len = 0;
        ಯದಿ (!preserve_crlf) {
            ಯದಿ (ಸಾಲು[raw_ಸಾಲು_len - 1] == '\r') {
                terminator_len = 1;
            }
            ಅನ್ಯಥಾ ಯದಿ (raw_ಸಾಲು_len > 1 && ಸಾಲು[raw_ಸಾಲು_len - 2] == '\r') {
                terminator_len = 2;
            }
        }
        _PyTok_Off appended;
        ಯದಿ (terminator_len || add_newline) {
            appended = Py_Tok_Source_Append_ಸಾಲು_With_Newline(
                &tok->source, ಸಾಲು, raw_ಸಾಲು_len - terminator_len);
        }
        ಅನ್ಯಥಾ {
            appended = Py_Tok_Source_Append_ಸಾಲು(&tok->source, ಸಾಲು, raw_ಸಾಲು_len);
        }
        ಯದಿ (appended < 0) {
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        tok->reader->prepared_final_newline = add_newline;
        pos += raw_ಸಾಲು_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    ಯದಿ (add_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || data[len - 1] != '\r'))
    {
        ಯದಿ (Py_Tok_Source_Append_ಸಾಲು(&tok->source, "\n", 1) < 0) {
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ
_PyTok_PrepareString(ಸಂರಚನಾ tok_state *tok, ಧ್ರುವ ವರ್ಣ *ಒಳಹರಿವು, ಪೂರ್ಣಾಙ್ಕ utf8_only,
                     ಪೂರ್ಣಾಙ್ಕ exec_ಒಳಹರಿವು, ಪೂರ್ಣಾಙ್ಕ preserve_crlf)
{
    Py_ssize_t raw_len = strlen(ಒಳಹರಿವು);
    ವರ್ಣ *raw = (ವರ್ಣ *)ಒಳಹರಿವು;

    ಯದಿ (utf8_only) {
        ಯದಿ (_PyTok_SetEncoding(tok, "utf-8") < 0) {
            ಪ್ರತ್ಯಾಗಮ -1;
        }
    }
    ಅನ್ಯಥಾ {
        Py_ssize_t first_original_len = raw_ಸಾಲು_length(raw, raw_len);
        _PyTok_Chunk first = {
            .data = raw,
            .len = first_original_len,
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        _PyTok_Chunk second = {0};
        ಪೂರ್ಣಾಙ್ಕ have_second = first_original_len < raw_len;
        ಯದಿ (have_second) {
            second.data = raw + first_original_len;
            second.len = raw_ಸಾಲು_length(second.data,
                                         raw_len - first_original_len);
        }
        Py_ssize_t bom_len;
        _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
            tok, &first, have_second ? &second : ರಿಕ್ತ, 1, &bom_len);
        ಯದಿ (detection == _PYTOK_ENCODING_ERROR) {
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        raw += bom_len;
        raw_len -= bom_len;
    }

    _PyTok_Chunk decoded = {
        .data = raw,
        .len = raw_len,
        .ownership = _PYTOK_CHUNK_BORROWED,
    };
    ಧ್ರುವ ವರ್ಣ *encoding = tok->reader->encoding;
    ಯದಿ (encoding != ರಿಕ್ತ && strcmp(encoding, "utf-8") != 0) {
        ಯದಿ (_PyTok_DecodeOnce(
                tok, &decoded, encoding, ರಿಕ್ತ) < 0) {
            ಪ್ರತ್ಯಾಗಮ -1;
        }
    }

    ಪೂರ್ಣಾಙ್ಕ stored = store_prepared_source(
        tok, decoded.data, decoded.len, preserve_crlf, exec_ಒಳಹರಿವು);
    _PyTok_ChunkClear(&decoded);
    ಯದಿ (stored < 0) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಯದಿ (!utf8_only &&
            (encoding == ರಿಕ್ತ || strcmp(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(_PyTok_SourceData(&tok->source), tok, 1)) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ
_PyTok_StartDecoder(ಸಂರಚನಾ tok_state *tok, ಧ್ರುವ ವರ್ಣ *errors)
{
    _PyTok_Reader *reader = tok->reader;
    ಯದಿ (reader->encoding == ರಿಕ್ತ || reader->decoder != ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ (reader->kind == _PYTOK_READER_FILE &&
            strcmp(reader->encoding, "utf-8") == 0) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    PyObject *codec = _PyCodec_LookupTextEncoding(reader->encoding, ರಿಕ್ತ);
    ಯದಿ (codec != ರಿಕ್ತ) {
        reader->decoder = _PyCodecInfo_GetIncrementalDecoder(codec, errors);
        Py_DECREF(codec);
    }
    ಯದಿ (reader->decoder == ರಿಕ್ತ) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        ಯದಿ (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != ರಿಕ್ತ ? tok->filename : Py_None);
        }
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ
_PyTok_DecodeChunk(ಸಂರಚನಾ tok_state *tok, _PyTok_Chunk *chunk, ಪೂರ್ಣಾಙ್ಕ final)
{
    _PyTok_Reader *reader = tok->reader;
    ಯದಿ (reader->decoder == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪೂರ್ಣಾಙ್ಕ strip_bom = reader->kind == _PYTOK_READER_READLINE &&
        chunk->len >= UTF16_BOM_LENGTH &&
        (memcmp(chunk->data, UTF16_LE_BOM, UTF16_BOM_LENGTH) == 0 ||
         memcmp(chunk->data, UTF16_BE_BOM, UTF16_BOM_LENGTH) == 0);
    PyObject *ಒಳಹರಿವು;
    ಯದಿ (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            PyBytes_Check(chunk->owner) &&
            chunk->data == PyBytes_AS_STRING(chunk->owner)) {
        ಒಳಹರಿವು = Py_NewRef(chunk->owner);
    }
    ಅನ್ಯಥಾ {
        ಒಳಹರಿವು = PyBytes_FromStringAndSize(chunk->data, chunk->len);
    }
    ಯದಿ (ಒಳಹರಿವು == ರಿಕ್ತ) {
        tok->done = E_NOMEM;
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    PyObject *unicode = PyObject_CallMethodObjArgs(
        reader->decoder, &_Py_ID(decode), ಒಳಹರಿವು,
        final ? Py_True : Py_False, ರಿಕ್ತ);
    Py_DECREF(ಒಳಹರಿವು);
    ಯದಿ (unicode == ರಿಕ್ತ) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        ಯದಿ (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != ರಿಕ್ತ ? tok->filename : Py_None);
        }
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಯದಿ (!PyUnicode_Check(unicode)) {
        PyErr_Format(PyExc_TypeError,
                     "decoder should return a string result, not '%.200s'",
                     Py_TYPE(unicode)->tp_name);
        Py_DECREF(unicode);
        tok->done = E_DECODE;
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಪ್ರತ್ಯಾಗಮ chunk_set_unicode(tok, chunk, unicode, strip_bom);
}

ಪೂರ್ಣಾಙ್ಕ
Py_Tok_Decoder_Has_Buffered_ಒಳಹರಿವು(ಸಂರಚನಾ tok_state *tok)
{
    ಯದಿ (tok->reader->decoder == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    PyObject *state = PyObject_CallMethodNoArgs(
        tok->reader->decoder, &_Py_ID(getstate));
    ಯದಿ (state == ರಿಕ್ತ) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಯದಿ (!PyTuple_Check(state) || PyTuple_GET_SIZE(state) != 2 ||
            !PyBytes_Check(PyTuple_GET_ITEM(state, 0)) ||
            !PyLong_Check(PyTuple_GET_ITEM(state, 1))) {
        Py_DECREF(state);
        PyErr_SetString(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->done = E_DECODE;
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಪೂರ್ಣಾಙ್ಕ pending = PyBytes_GET_SIZE(PyTuple_GET_ITEM(state, 0)) != 0;
    Py_DECREF(state);
    ಪ್ರತ್ಯಾಗಮ pending;
}
