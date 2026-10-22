#ଅନ୍ତର୍ଭାଵ "Python.h"
#ଅନ୍ତର୍ଭାଵ "pycore_codecs.h"
#ଅନ୍ତର୍ଭାଵ "pycore_global_strings.h"
#ଅନ୍ତର୍ଭାଵ "pycore_runtime.h"
#ଅନ୍ତର୍ଭାଵ "errcode.h"

#ଅନ୍ତର୍ଭାଵ "reader_internal.h"
#ଅନ୍ତର୍ଭାଵ "helpers.h"
#ଅନ୍ତର୍ଭାଵ "../lexer/state.h"

#define UNICODE_BOM 0xFEFF
#define UTF16_LE_BOM "\xff\xfe"
#define UTF16_BE_BOM "\xfe\xff"
#define UTF16_BOM_LENGTH ((Py_ssize_t)(ଆକାର(UTF16_LE_BOM) - 1))
#define UTF8_BOM "\xef\xbb\xbf"
#define UTF8_BOM_LENGTH ((Py_ssize_t)(ଆକାର(UTF8_BOM) - 1))

ଅକ୍ଷର *
_PyTok_CopyBytes(ଅଚଳ ଅକ୍ଷର *data, Py_ssize_t len)
{
    ଯଦି (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଅକ୍ଷର *copy = PyMem_Malloc((size_t)len + 1);
    ଯଦି (copy == ଖାଲି) {
        PyErr_NoMemory();
        ଫେରନ୍ତୁ ଖାଲି;
    }
    memcpy(copy, data, (size_t)len);
    copy[len] = '\0';
    ଫେରନ୍ତୁ copy;
}

ସ୍ଥିର ଶୂନ୍ୟ
chunk_release_data(_PyTok_Chunk *chunk)
{
    ବାଛ (chunk->ownership) {
        ସ୍ଥିତି _PYTOK_CHUNK_BORROWED:
            ଭାଙ୍ଗ;
        ସ୍ଥିତି _PYTOK_CHUNK_PYMEM:
            PyMem_Free(chunk->data);
            ଭାଙ୍ଗ;
        ସ୍ଥିତି _PYTOK_CHUNK_PYOBJECT:
            Py_DECREF(chunk->owner);
            ଭାଙ୍ଗ;
    }
}

ଶୂନ୍ୟ
_PyTok_ChunkClear(_PyTok_Chunk *chunk)
{
    chunk_release_data(chunk);
    *chunk = (_PyTok_Chunk){0};
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
chunk_set_unicode(ସଂରଚନା tok_state *tok, _PyTok_Chunk *chunk,
                  PyObject *unicode, ପୂର୍ଣ୍ଣସଂଖ୍ୟା strip_bom)
{
    Py_ssize_t utf8_len;
    ଅଚଳ ଅକ୍ଷର *utf8 = PyUnicode_AsUTF8AndSize(unicode, &utf8_len);
    ଯଦି (utf8 == ଖାଲି) {
        Py_DECREF(unicode);
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        ଫେରନ୍ତୁ -1;
    }
    ଯଦି (strip_bom && PyUnicode_GET_LENGTH(unicode) > 0 &&
            PyUnicode_ReadChar(unicode, 0) == UNICODE_BOM) {
        utf8 += UTF8_BOM_LENGTH;
        utf8_len -= UTF8_BOM_LENGTH;
    }
    chunk_release_data(chunk);
    chunk->owner = unicode;
    chunk->data = (ଅକ୍ଷର *)utf8;
    chunk->len = utf8_len;
    chunk->ownership = _PYTOK_CHUNK_PYOBJECT;
    ଫେରନ୍ତୁ 0;
}

ଅକ୍ଷର *
_PyTok_NormalizeNewlines(ଅଚଳ ଅକ୍ଷର *data, Py_ssize_t len,
                         Py_ssize_t *out_len)
{
    ଯଦି (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଅକ୍ଷର *result = PyMem_Malloc((size_t)len + 1);
    ଯଦି (result == ଖାଲି) {
        PyErr_NoMemory();
        ଫେରନ୍ତୁ ଖାଲି;
    }
    Py_ssize_t ଲିଖ = 0;
    ପାଇଁ (Py_ssize_t ପଠ = 0; ପଠ < len; ପଠ++) {
        ଅକ୍ଷର c = data[ପଠ];
        ଯଦି (c == '\r') {
            ଯଦି (ପଠ + 1 < len && data[ପଠ + 1] == '\n') {
                ପଠ++;
            }
            c = '\n';
        }
        result[ଲିଖ++] = c;
    }
    result[ଲିଖ] = '\0';
    *out_len = ଲିଖ;
    ଫେରନ୍ତୁ result;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyTok_SetEncoding(ସଂରଚନା tok_state *tok, ଅଚଳ ଅକ୍ଷର *encoding)
{
    ଅକ୍ଷର *copy = _PyTok_CopyBytes(encoding, strlen(encoding));
    ଯଦି (copy == ଖାଲି) {
        tok->done = E_NOMEM;
        ଫେରନ୍ତୁ -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
find_cookie(ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି, Py_ssize_t len, ଅକ୍ଷର **encoding, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *scan_next)
{
    Py_ssize_t i = 0;
    *encoding = ଖାଲି;
    *scan_next = 1;
    ପାଇଁ (; i < len; i++) {
        ଯଦି (ଧାଡ଼ି[i] == '#') {
            ଭାଙ୍ଗ;
        }
        ଯଦି (ଧାଡ଼ି[i] == '\n' || ଧାଡ଼ି[i] == '\r') {
            ଫେରନ୍ତୁ 0;
        }
        ଯଦି (ଧାଡ଼ି[i] != ' ' && ଧାଡ଼ି[i] != '\t' && ଧାଡ଼ି[i] != '\f') {
            *scan_next = 0;
            ଫେରନ୍ତୁ 0;
        }
    }
    ଅଚଳ Py_ssize_t marker_len = ଆକାର("coding") - 1;
    ପାଇଁ (; i < len - marker_len; i++) {
        ଯଦି (memcmp(ଧାଡ଼ି + i, "coding", marker_len) != 0) {
            ଜାରି;
        }
        ଅଚଳ ଅକ୍ଷର *cursor = ଧାଡ଼ି + i + marker_len;
        ଯଦି (*cursor != ':' && *cursor != '=') {
            ଜାରି;
        }
        କର {
            cursor++;
        } ଯେତେବେଳେ (cursor < ଧାଡ଼ି + len &&
                 (*cursor == ' ' || *cursor == '\t'));
        ଅଚଳ ଅକ୍ଷର *start = cursor;
        ଯେତେବେଳେ (cursor < ଧାଡ଼ି + len &&
                (Py_ISALNUM(*cursor) || *cursor == '-' ||
                 *cursor == '_' || *cursor == '.')) {
            cursor++;
        }
        ଯଦି (cursor == start) {
            ଜାରି;
        }
        Py_ssize_t encoding_len = cursor - start;
        // Include the longest recognized prefix and its NUL terminator.
        ଅକ୍ଷର normalized[ଆକାର("iso-latin-1-")];
        ଅଚଳ Py_ssize_t prefix_len = Py_MIN(
            encoding_len, (Py_ssize_t)ଆକାର(normalized) - 1);
        Py_ssize_t n;
        ପାଇଁ (n = 0; n < prefix_len; n++) {
            normalized[n] = start[n] == '_' ? '-' : Py_TOLOWER(start[n]);
        }
        normalized[n] = '\0';
        ଅଚଳ ଅକ୍ଷର *canonical = start;
        ଯଦି (strcmp(normalized, "utf-8") == 0 ||
                strncmp(normalized, "utf-8-", ଆକାର("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        ଅନ୍ୟଥା ଯଦି (strcmp(normalized, "latin-1") == 0 ||
                 strcmp(normalized, "iso-8859-1") == 0 ||
                 strcmp(normalized, "iso-latin-1") == 0 ||
                 strncmp(normalized, "latin-1-", ଆକାର("latin-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-8859-1-", ଆକାର("iso-8859-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-latin-1-", ଆକାର("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        ଯଦି (canonical != start) {
            encoding_len = strlen(canonical);
        }
        *encoding = _PyTok_CopyBytes(canonical, encoding_len);
        ଯଦି (*encoding == ଖାଲି) {
            ଫେରନ୍ତୁ -1;
        }
        *scan_next = 0;
        ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ 0;
}

_PyTok_EncodingResult
_PyTok_DetectEncoding(ସଂରଚନା tok_state *tok, ଅଚଳ _PyTok_Chunk *first,
                      ଅଚଳ _PyTok_Chunk *second, ପୂର୍ଣ୍ଣସଂଖ୍ୟା final,
                      Py_ssize_t *bom_len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା bom = first->len >= UTF8_BOM_LENGTH &&
        memcmp(first->data, UTF8_BOM, UTF8_BOM_LENGTH) == 0;
    *bom_len = bom ? UTF8_BOM_LENGTH : 0;

    ଅକ୍ଷର *cookie = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା scan_next = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cookie_ଧାଡ଼ି = 1;
    ଅଚଳ ଅକ୍ଷର *first_data = first->data + *bom_len;
    Py_ssize_t first_len = first->len - *bom_len;
    ଯଦି (find_cookie(first_data, first_len, &cookie, &scan_next) < 0) {
        ଫେରନ୍ତୁ _PYTOK_ENCODING_ERROR;
    }
    ଯଦି (cookie == ଖାଲି && scan_next && second != ଖାଲି) {
        ଯଦି (find_cookie(second->data, second->len, &cookie, &scan_next) < 0) {
            ଫେରନ୍ତୁ _PYTOK_ENCODING_ERROR;
        }
        cookie_ଧାଡ଼ି = 2;
    }
    ଅନ୍ୟଥା ଯଦି (cookie == ଖାଲି && scan_next && !final) {
        ଫେରନ୍ତୁ PYTOK_ENCODING_NEED_SECOND_ଧାଡ଼ି;
    }

    ଯଦି (cookie == ଖାଲି) {
        ଯଦି (bom && _PyTok_SetEncoding(tok, "utf-8") < 0) {
            ଫେରନ୍ତୁ _PYTOK_ENCODING_ERROR;
        }
        ଫେରନ୍ତୁ _PYTOK_ENCODING_DONE;
    }
    ଯଦି (bom && strcmp(cookie, "utf-8") != 0) {
        ଅଚଳ _PyTok_Chunk *ଧାଡ଼ି = cookie_ଧାଡ଼ି == 2 ? second : first;
        ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି_data = ଧାଡ଼ି->data + (cookie_ଧାଡ଼ି == 1 ? *bom_len : 0);
        Py_ssize_t ଧାଡ଼ି_len = ଧାଡ଼ି->len - (cookie_ଧାଡ଼ି == 1 ? *bom_len : 0);
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Py_MIN(ଧାଡ଼ି_len, INT_MAX);
        ଯଦି (end_col > 0 && (ଧାଡ଼ି_data[end_col - 1] == '\n' ||
                            ଧାଡ଼ି_data[end_col - 1] == '\r')) {
            end_col--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, ଧାଡ଼ି_data, 0, cookie_ଧାଡ଼ି, 0, end_col,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        ଫେରନ୍ତୁ _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    ଫେରନ୍ତୁ _PYTOK_ENCODING_DONE;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyTok_DecodeOnce(ସଂରଚନା tok_state *tok, _PyTok_Chunk *chunk,
                  ଅଚଳ ଅକ୍ଷର *encoding, ଅଚଳ ଅକ୍ଷର *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->data, chunk->len, encoding, errors);
    ଯଦି (unicode == ଖାଲି) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        ଫେରନ୍ତୁ -1;
    }
    ଫେରନ୍ତୁ chunk_set_unicode(tok, chunk, unicode, 0);
}

ସ୍ଥିର Py_ssize_t
raw_ଧାଡ଼ି_length(ଅଚଳ ଅକ୍ଷର *data, Py_ssize_t len)
{
    ପାଇଁ (Py_ssize_t i = 0; i < len; i++) {
        ଯଦି (data[i] == '\n') {
            ଫେରନ୍ତୁ i + 1;
        }
        ଯଦି (data[i] == '\r') {
            ଫେରନ୍ତୁ i + 1 < len && data[i + 1] == '\n' ? i + 2 : i + 1;
        }
    }
    ଫେରନ୍ତୁ len;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
store_prepared_source(ସଂରଚନା tok_state *tok, ଅଚଳ ଅକ୍ଷର *data, Py_ssize_t len,
                      ପୂର୍ଣ୍ଣସଂଖ୍ୟା preserve_crlf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା add_final_newline)
{
    Py_ssize_t pos = 0;
    ଯେତେବେଳେ (pos < len) {
        Py_ssize_t raw_ଧାଡ଼ି_len;
        ଯଦି (preserve_crlf) {
            ଅଚଳ ଅକ୍ଷର *newline = memchr(data + pos, '\n', len - pos);
            raw_ଧାଡ଼ି_len = newline == ଖାଲି
                ? len - pos : newline - data - pos + 1;
        }
        ଅନ୍ୟଥା {
            raw_ଧାଡ଼ି_len = raw_ଧାଡ଼ି_length(data + pos, len - pos);
        }
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminated = preserve_crlf
            ? data[pos + raw_ଧାଡ଼ି_len - 1] == '\n'
            : data[pos + raw_ଧାଡ଼ି_len - 1] == '\n' ||
              data[pos + raw_ଧାଡ଼ି_len - 1] == '\r';
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା add_newline = add_final_newline &&
            pos + raw_ଧାଡ଼ି_len == len && !terminated;
        ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି = data + pos;
        // raw_line_length stops at the first CR or LF, so any CR is in
        // the line terminator.
        Py_ssize_t terminator_len = 0;
        ଯଦି (!preserve_crlf) {
            ଯଦି (ଧାଡ଼ି[raw_ଧାଡ଼ି_len - 1] == '\r') {
                terminator_len = 1;
            }
            ଅନ୍ୟଥା ଯଦି (raw_ଧାଡ଼ି_len > 1 && ଧାଡ଼ି[raw_ଧାଡ଼ି_len - 2] == '\r') {
                terminator_len = 2;
            }
        }
        _PyTok_Off appended;
        ଯଦି (terminator_len || add_newline) {
            appended = Py_Tok_Source_Append_ଧାଡ଼ି_With_Newline(
                &tok->source, ଧାଡ଼ି, raw_ଧାଡ଼ି_len - terminator_len);
        }
        ଅନ୍ୟଥା {
            appended = Py_Tok_Source_Append_ଧାଡ଼ି(&tok->source, ଧାଡ଼ି, raw_ଧାଡ଼ି_len);
        }
        ଯଦି (appended < 0) {
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            ଫେରନ୍ତୁ -1;
        }
        tok->reader->prepared_final_newline = add_newline;
        pos += raw_ଧାଡ଼ି_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    ଯଦି (add_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || data[len - 1] != '\r'))
    {
        ଯଦି (Py_Tok_Source_Append_ଧାଡ଼ି(&tok->source, "\n", 1) < 0) {
            ଫେରନ୍ତୁ -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyTok_PrepareString(ସଂରଚନା tok_state *tok, ଅଚଳ ଅକ୍ଷର *ନିବେଶ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା utf8_only,
                     ପୂର୍ଣ୍ଣସଂଖ୍ୟା exec_ନିବେଶ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା preserve_crlf)
{
    Py_ssize_t raw_len = strlen(ନିବେଶ);
    ଅକ୍ଷର *raw = (ଅକ୍ଷର *)ନିବେଶ;

    ଯଦି (utf8_only) {
        ଯଦି (_PyTok_SetEncoding(tok, "utf-8") < 0) {
            ଫେରନ୍ତୁ -1;
        }
    }
    ଅନ୍ୟଥା {
        Py_ssize_t first_original_len = raw_ଧାଡ଼ି_length(raw, raw_len);
        _PyTok_Chunk first = {
            .data = raw,
            .len = first_original_len,
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        _PyTok_Chunk second = {0};
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା have_second = first_original_len < raw_len;
        ଯଦି (have_second) {
            second.data = raw + first_original_len;
            second.len = raw_ଧାଡ଼ି_length(second.data,
                                         raw_len - first_original_len);
        }
        Py_ssize_t bom_len;
        _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
            tok, &first, have_second ? &second : ଖାଲି, 1, &bom_len);
        ଯଦି (detection == _PYTOK_ENCODING_ERROR) {
            ଫେରନ୍ତୁ -1;
        }
        raw += bom_len;
        raw_len -= bom_len;
    }

    _PyTok_Chunk decoded = {
        .data = raw,
        .len = raw_len,
        .ownership = _PYTOK_CHUNK_BORROWED,
    };
    ଅଚଳ ଅକ୍ଷର *encoding = tok->reader->encoding;
    ଯଦି (encoding != ଖାଲି && strcmp(encoding, "utf-8") != 0) {
        ଯଦି (_PyTok_DecodeOnce(
                tok, &decoded, encoding, ଖାଲି) < 0) {
            ଫେରନ୍ତୁ -1;
        }
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା stored = store_prepared_source(
        tok, decoded.data, decoded.len, preserve_crlf, exec_ନିବେଶ);
    _PyTok_ChunkClear(&decoded);
    ଯଦି (stored < 0) {
        ଫେରନ୍ତୁ -1;
    }
    ଯଦି (!utf8_only &&
            (encoding == ଖାଲି || strcmp(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(_PyTok_SourceData(&tok->source), tok, 1)) {
        ଫେରନ୍ତୁ -1;
    }
    ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyTok_StartDecoder(ସଂରଚନା tok_state *tok, ଅଚଳ ଅକ୍ଷର *errors)
{
    _PyTok_Reader *reader = tok->reader;
    ଯଦି (reader->encoding == ଖାଲି || reader->decoder != ଖାଲି) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି (reader->kind == _PYTOK_READER_FILE &&
            strcmp(reader->encoding, "utf-8") == 0) {
        ଫେରନ୍ତୁ 0;
    }

    PyObject *codec = _PyCodec_LookupTextEncoding(reader->encoding, ଖାଲି);
    ଯଦି (codec != ଖାଲି) {
        reader->decoder = _PyCodecInfo_GetIncrementalDecoder(codec, errors);
        Py_DECREF(codec);
    }
    ଯଦି (reader->decoder == ଖାଲି) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        ଯଦି (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != ଖାଲି ? tok->filename : Py_None);
        }
        ଫେରନ୍ତୁ -1;
    }
    ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyTok_DecodeChunk(ସଂରଚନା tok_state *tok, _PyTok_Chunk *chunk, ପୂର୍ଣ୍ଣସଂଖ୍ୟା final)
{
    _PyTok_Reader *reader = tok->reader;
    ଯଦି (reader->decoder == ଖାଲି) {
        ଫେରନ୍ତୁ 0;
    }
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା strip_bom = reader->kind == _PYTOK_READER_READLINE &&
        chunk->len >= UTF16_BOM_LENGTH &&
        (memcmp(chunk->data, UTF16_LE_BOM, UTF16_BOM_LENGTH) == 0 ||
         memcmp(chunk->data, UTF16_BE_BOM, UTF16_BOM_LENGTH) == 0);
    PyObject *ନିବେଶ;
    ଯଦି (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            PyBytes_Check(chunk->owner) &&
            chunk->data == PyBytes_AS_STRING(chunk->owner)) {
        ନିବେଶ = Py_NewRef(chunk->owner);
    }
    ଅନ୍ୟଥା {
        ନିବେଶ = PyBytes_FromStringAndSize(chunk->data, chunk->len);
    }
    ଯଦି (ନିବେଶ == ଖାଲି) {
        tok->done = E_NOMEM;
        ଫେରନ୍ତୁ -1;
    }
    PyObject *unicode = PyObject_CallMethodObjArgs(
        reader->decoder, &_Py_ID(decode), ନିବେଶ,
        final ? Py_True : Py_False, ଖାଲି);
    Py_DECREF(ନିବେଶ);
    ଯଦି (unicode == ଖାଲି) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        ଯଦି (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != ଖାଲି ? tok->filename : Py_None);
        }
        ଫେରନ୍ତୁ -1;
    }
    ଯଦି (!PyUnicode_Check(unicode)) {
        PyErr_Format(PyExc_TypeError,
                     "decoder should return a string result, not '%.200s'",
                     Py_TYPE(unicode)->tp_name);
        Py_DECREF(unicode);
        tok->done = E_DECODE;
        ଫେରନ୍ତୁ -1;
    }
    ଫେରନ୍ତୁ chunk_set_unicode(tok, chunk, unicode, strip_bom);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
Py_Tok_Decoder_Has_Buffered_ନିବେଶ(ସଂରଚନା tok_state *tok)
{
    ଯଦି (tok->reader->decoder == ଖାଲି) {
        ଫେରନ୍ତୁ 0;
    }
    PyObject *state = PyObject_CallMethodNoArgs(
        tok->reader->decoder, &_Py_ID(getstate));
    ଯଦି (state == ଖାଲି) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        ଫେରନ୍ତୁ -1;
    }
    ଯଦି (!PyTuple_Check(state) || PyTuple_GET_SIZE(state) != 2 ||
            !PyBytes_Check(PyTuple_GET_ITEM(state, 0)) ||
            !PyLong_Check(PyTuple_GET_ITEM(state, 1))) {
        Py_DECREF(state);
        PyErr_SetString(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->done = E_DECODE;
        ଫେରନ୍ତୁ -1;
    }
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pending = PyBytes_GET_SIZE(PyTuple_GET_ITEM(state, 0)) != 0;
    Py_DECREF(state);
    ଫେରନ୍ତୁ pending;
}
