#അന്തര്ഭാവ "Python.h"
#അന്തര്ഭാവ "pycore_codecs.h"
#അന്തര്ഭാവ "pycore_global_strings.h"
#അന്തര്ഭാവ "pycore_runtime.h"
#അന്തര്ഭാവ "errcode.h"

#അന്തര്ഭാവ "reader_internal.h"
#അന്തര്ഭാവ "helpers.h"
#അന്തര്ഭാവ "../lexer/state.h"

#define UNICODE_BOM 0xFEFF
#define UTF16_LE_BOM "\xff\xfe"
#define UTF16_BE_BOM "\xfe\xff"
#define UTF16_BOM_LENGTH ((Py_ssize_t)(വലുപ്പം(UTF16_LE_BOM) - 1))
#define UTF8_BOM "\xef\xbb\xbf"
#define UTF8_BOM_LENGTH ((Py_ssize_t)(വലുപ്പം(UTF8_BOM) - 1))

അക്ഷരം *
_PyTok_CopyBytes(സ്ഥിരം അക്ഷരം *data, Py_ssize_t len)
{
    എങ്കിൽ (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        തിരികെ ശൂന്യ;
    }
    അക്ഷരം *copy = PyMem_Malloc((size_t)len + 1);
    എങ്കിൽ (copy == ശൂന്യ) {
        PyErr_NoMemory();
        തിരികെ ശൂന്യ;
    }
    memcpy(copy, data, (size_t)len);
    copy[len] = '\0';
    തിരികെ copy;
}

സ്ഥിതം ശൂന്യം
chunk_release_data(_PyTok_Chunk *chunk)
{
    തിരഞ്ഞെടുപ്പ് (chunk->ownership) {
        അവസ്ഥ _PYTOK_CHUNK_BORROWED:
            നിർത്തുക;
        അവസ്ഥ _PYTOK_CHUNK_PYMEM:
            PyMem_Free(chunk->data);
            നിർത്തുക;
        അവസ്ഥ _PYTOK_CHUNK_PYOBJECT:
            Py_DECREF(chunk->owner);
            നിർത്തുക;
    }
}

ശൂന്യം
_PyTok_ChunkClear(_PyTok_Chunk *chunk)
{
    chunk_release_data(chunk);
    *chunk = (_PyTok_Chunk){0};
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
chunk_set_unicode(ഘടന tok_state *tok, _PyTok_Chunk *chunk,
                  PyObject *unicode, പൂർണ്ണസംഖ്യ strip_bom)
{
    Py_ssize_t utf8_len;
    സ്ഥിരം അക്ഷരം *utf8 = PyUnicode_AsUTF8AndSize(unicode, &utf8_len);
    എങ്കിൽ (utf8 == ശൂന്യ) {
        Py_DECREF(unicode);
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        തിരികെ -1;
    }
    എങ്കിൽ (strip_bom && PyUnicode_GET_LENGTH(unicode) > 0 &&
            PyUnicode_ReadChar(unicode, 0) == UNICODE_BOM) {
        utf8 += UTF8_BOM_LENGTH;
        utf8_len -= UTF8_BOM_LENGTH;
    }
    chunk_release_data(chunk);
    chunk->owner = unicode;
    chunk->data = (അക്ഷരം *)utf8;
    chunk->len = utf8_len;
    chunk->ownership = _PYTOK_CHUNK_PYOBJECT;
    തിരികെ 0;
}

അക്ഷരം *
_PyTok_NormalizeNewlines(സ്ഥിരം അക്ഷരം *data, Py_ssize_t len,
                         Py_ssize_t *out_len)
{
    എങ്കിൽ (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        തിരികെ ശൂന്യ;
    }
    അക്ഷരം *result = PyMem_Malloc((size_t)len + 1);
    എങ്കിൽ (result == ശൂന്യ) {
        PyErr_NoMemory();
        തിരികെ ശൂന്യ;
    }
    Py_ssize_t ലിഖ = 0;
    വേണ്ടി (Py_ssize_t പഠ = 0; പഠ < len; പഠ++) {
        അക്ഷരം c = data[പഠ];
        എങ്കിൽ (c == '\r') {
            എങ്കിൽ (പഠ + 1 < len && data[പഠ + 1] == '\n') {
                പഠ++;
            }
            c = '\n';
        }
        result[ലിഖ++] = c;
    }
    result[ലിഖ] = '\0';
    *out_len = ലിഖ;
    തിരികെ result;
}

പൂർണ്ണസംഖ്യ
_PyTok_SetEncoding(ഘടന tok_state *tok, സ്ഥിരം അക്ഷരം *encoding)
{
    അക്ഷരം *copy = _PyTok_CopyBytes(encoding, strlen(encoding));
    എങ്കിൽ (copy == ശൂന്യ) {
        tok->done = E_NOMEM;
        തിരികെ -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
find_cookie(സ്ഥിരം അക്ഷരം *വരി, Py_ssize_t len, അക്ഷരം **encoding, പൂർണ്ണസംഖ്യ *scan_next)
{
    Py_ssize_t i = 0;
    *encoding = ശൂന്യ;
    *scan_next = 1;
    വേണ്ടി (; i < len; i++) {
        എങ്കിൽ (വരി[i] == '#') {
            നിർത്തുക;
        }
        എങ്കിൽ (വരി[i] == '\n' || വരി[i] == '\r') {
            തിരികെ 0;
        }
        എങ്കിൽ (വരി[i] != ' ' && വരി[i] != '\t' && വരി[i] != '\f') {
            *scan_next = 0;
            തിരികെ 0;
        }
    }
    സ്ഥിരം Py_ssize_t marker_len = വലുപ്പം("coding") - 1;
    വേണ്ടി (; i < len - marker_len; i++) {
        എങ്കിൽ (memcmp(വരി + i, "coding", marker_len) != 0) {
            തുടരുക;
        }
        സ്ഥിരം അക്ഷരം *cursor = വരി + i + marker_len;
        എങ്കിൽ (*cursor != ':' && *cursor != '=') {
            തുടരുക;
        }
        ചെയ്യുക {
            cursor++;
        } ആകുമ്പോൾ (cursor < വരി + len &&
                 (*cursor == ' ' || *cursor == '\t'));
        സ്ഥിരം അക്ഷരം *start = cursor;
        ആകുമ്പോൾ (cursor < വരി + len &&
                (Py_ISALNUM(*cursor) || *cursor == '-' ||
                 *cursor == '_' || *cursor == '.')) {
            cursor++;
        }
        എങ്കിൽ (cursor == start) {
            തുടരുക;
        }
        Py_ssize_t encoding_len = cursor - start;
        // Include the longest recognized prefix and its NUL terminator.
        അക്ഷരം normalized[വലുപ്പം("iso-latin-1-")];
        സ്ഥിരം Py_ssize_t prefix_len = Py_MIN(
            encoding_len, (Py_ssize_t)വലുപ്പം(normalized) - 1);
        Py_ssize_t n;
        വേണ്ടി (n = 0; n < prefix_len; n++) {
            normalized[n] = start[n] == '_' ? '-' : Py_TOLOWER(start[n]);
        }
        normalized[n] = '\0';
        സ്ഥിരം അക്ഷരം *canonical = start;
        എങ്കിൽ (strcmp(normalized, "utf-8") == 0 ||
                strncmp(normalized, "utf-8-", വലുപ്പം("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        അല്ലെങ്കിൽ എങ്കിൽ (strcmp(normalized, "latin-1") == 0 ||
                 strcmp(normalized, "iso-8859-1") == 0 ||
                 strcmp(normalized, "iso-latin-1") == 0 ||
                 strncmp(normalized, "latin-1-", വലുപ്പം("latin-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-8859-1-", വലുപ്പം("iso-8859-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-latin-1-", വലുപ്പം("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        എങ്കിൽ (canonical != start) {
            encoding_len = strlen(canonical);
        }
        *encoding = _PyTok_CopyBytes(canonical, encoding_len);
        എങ്കിൽ (*encoding == ശൂന്യ) {
            തിരികെ -1;
        }
        *scan_next = 0;
        തിരികെ 0;
    }
    തിരികെ 0;
}

_PyTok_EncodingResult
_PyTok_DetectEncoding(ഘടന tok_state *tok, സ്ഥിരം _PyTok_Chunk *first,
                      സ്ഥിരം _PyTok_Chunk *second, പൂർണ്ണസംഖ്യ final,
                      Py_ssize_t *bom_len)
{
    പൂർണ്ണസംഖ്യ bom = first->len >= UTF8_BOM_LENGTH &&
        memcmp(first->data, UTF8_BOM, UTF8_BOM_LENGTH) == 0;
    *bom_len = bom ? UTF8_BOM_LENGTH : 0;

    അക്ഷരം *cookie = ശൂന്യ;
    പൂർണ്ണസംഖ്യ scan_next = 0;
    പൂർണ്ണസംഖ്യ cookie_വരി = 1;
    സ്ഥിരം അക്ഷരം *first_data = first->data + *bom_len;
    Py_ssize_t first_len = first->len - *bom_len;
    എങ്കിൽ (find_cookie(first_data, first_len, &cookie, &scan_next) < 0) {
        തിരികെ _PYTOK_ENCODING_ERROR;
    }
    എങ്കിൽ (cookie == ശൂന്യ && scan_next && second != ശൂന്യ) {
        എങ്കിൽ (find_cookie(second->data, second->len, &cookie, &scan_next) < 0) {
            തിരികെ _PYTOK_ENCODING_ERROR;
        }
        cookie_വരി = 2;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (cookie == ശൂന്യ && scan_next && !final) {
        തിരികെ PYTOK_ENCODING_NEED_SECOND_വരി;
    }

    എങ്കിൽ (cookie == ശൂന്യ) {
        എങ്കിൽ (bom && _PyTok_SetEncoding(tok, "utf-8") < 0) {
            തിരികെ _PYTOK_ENCODING_ERROR;
        }
        തിരികെ _PYTOK_ENCODING_DONE;
    }
    എങ്കിൽ (bom && strcmp(cookie, "utf-8") != 0) {
        സ്ഥിരം _PyTok_Chunk *വരി = cookie_വരി == 2 ? second : first;
        സ്ഥിരം അക്ഷരം *വരി_data = വരി->data + (cookie_വരി == 1 ? *bom_len : 0);
        Py_ssize_t വരി_len = വരി->len - (cookie_വരി == 1 ? *bom_len : 0);
        പൂർണ്ണസംഖ്യ end_col = (പൂർണ്ണസംഖ്യ)Py_MIN(വരി_len, INT_MAX);
        എങ്കിൽ (end_col > 0 && (വരി_data[end_col - 1] == '\n' ||
                            വരി_data[end_col - 1] == '\r')) {
            end_col--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, വരി_data, 0, cookie_വരി, 0, end_col,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        തിരികെ _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    തിരികെ _PYTOK_ENCODING_DONE;
}

പൂർണ്ണസംഖ്യ
_PyTok_DecodeOnce(ഘടന tok_state *tok, _PyTok_Chunk *chunk,
                  സ്ഥിരം അക്ഷരം *encoding, സ്ഥിരം അക്ഷരം *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->data, chunk->len, encoding, errors);
    എങ്കിൽ (unicode == ശൂന്യ) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        തിരികെ -1;
    }
    തിരികെ chunk_set_unicode(tok, chunk, unicode, 0);
}

സ്ഥിതം Py_ssize_t
raw_വരി_length(സ്ഥിരം അക്ഷരം *data, Py_ssize_t len)
{
    വേണ്ടി (Py_ssize_t i = 0; i < len; i++) {
        എങ്കിൽ (data[i] == '\n') {
            തിരികെ i + 1;
        }
        എങ്കിൽ (data[i] == '\r') {
            തിരികെ i + 1 < len && data[i + 1] == '\n' ? i + 2 : i + 1;
        }
    }
    തിരികെ len;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
store_prepared_source(ഘടന tok_state *tok, സ്ഥിരം അക്ഷരം *data, Py_ssize_t len,
                      പൂർണ്ണസംഖ്യ preserve_crlf, പൂർണ്ണസംഖ്യ add_final_newline)
{
    Py_ssize_t pos = 0;
    ആകുമ്പോൾ (pos < len) {
        Py_ssize_t raw_വരി_len;
        എങ്കിൽ (preserve_crlf) {
            സ്ഥിരം അക്ഷരം *newline = memchr(data + pos, '\n', len - pos);
            raw_വരി_len = newline == ശൂന്യ
                ? len - pos : newline - data - pos + 1;
        }
        അല്ലെങ്കിൽ {
            raw_വരി_len = raw_വരി_length(data + pos, len - pos);
        }
        പൂർണ്ണസംഖ്യ terminated = preserve_crlf
            ? data[pos + raw_വരി_len - 1] == '\n'
            : data[pos + raw_വരി_len - 1] == '\n' ||
              data[pos + raw_വരി_len - 1] == '\r';
        പൂർണ്ണസംഖ്യ add_newline = add_final_newline &&
            pos + raw_വരി_len == len && !terminated;
        സ്ഥിരം അക്ഷരം *വരി = data + pos;
        // raw_line_length stops at the first CR or LF, so any CR is in
        // the line terminator.
        Py_ssize_t terminator_len = 0;
        എങ്കിൽ (!preserve_crlf) {
            എങ്കിൽ (വരി[raw_വരി_len - 1] == '\r') {
                terminator_len = 1;
            }
            അല്ലെങ്കിൽ എങ്കിൽ (raw_വരി_len > 1 && വരി[raw_വരി_len - 2] == '\r') {
                terminator_len = 2;
            }
        }
        _PyTok_Off appended;
        എങ്കിൽ (terminator_len || add_newline) {
            appended = Py_Tok_Source_Append_വരി_With_Newline(
                &tok->source, വരി, raw_വരി_len - terminator_len);
        }
        അല്ലെങ്കിൽ {
            appended = Py_Tok_Source_Append_വരി(&tok->source, വരി, raw_വരി_len);
        }
        എങ്കിൽ (appended < 0) {
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            തിരികെ -1;
        }
        tok->reader->prepared_final_newline = add_newline;
        pos += raw_വരി_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    എങ്കിൽ (add_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || data[len - 1] != '\r'))
    {
        എങ്കിൽ (Py_Tok_Source_Append_വരി(&tok->source, "\n", 1) < 0) {
            തിരികെ -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
_PyTok_PrepareString(ഘടന tok_state *tok, സ്ഥിരം അക്ഷരം *നിവേശം, പൂർണ്ണസംഖ്യ utf8_only,
                     പൂർണ്ണസംഖ്യ exec_നിവേശം, പൂർണ്ണസംഖ്യ preserve_crlf)
{
    Py_ssize_t raw_len = strlen(നിവേശം);
    അക്ഷരം *raw = (അക്ഷരം *)നിവേശം;

    എങ്കിൽ (utf8_only) {
        എങ്കിൽ (_PyTok_SetEncoding(tok, "utf-8") < 0) {
            തിരികെ -1;
        }
    }
    അല്ലെങ്കിൽ {
        Py_ssize_t first_original_len = raw_വരി_length(raw, raw_len);
        _PyTok_Chunk first = {
            .data = raw,
            .len = first_original_len,
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        _PyTok_Chunk second = {0};
        പൂർണ്ണസംഖ്യ have_second = first_original_len < raw_len;
        എങ്കിൽ (have_second) {
            second.data = raw + first_original_len;
            second.len = raw_വരി_length(second.data,
                                         raw_len - first_original_len);
        }
        Py_ssize_t bom_len;
        _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
            tok, &first, have_second ? &second : ശൂന്യ, 1, &bom_len);
        എങ്കിൽ (detection == _PYTOK_ENCODING_ERROR) {
            തിരികെ -1;
        }
        raw += bom_len;
        raw_len -= bom_len;
    }

    _PyTok_Chunk decoded = {
        .data = raw,
        .len = raw_len,
        .ownership = _PYTOK_CHUNK_BORROWED,
    };
    സ്ഥിരം അക്ഷരം *encoding = tok->reader->encoding;
    എങ്കിൽ (encoding != ശൂന്യ && strcmp(encoding, "utf-8") != 0) {
        എങ്കിൽ (_PyTok_DecodeOnce(
                tok, &decoded, encoding, ശൂന്യ) < 0) {
            തിരികെ -1;
        }
    }

    പൂർണ്ണസംഖ്യ stored = store_prepared_source(
        tok, decoded.data, decoded.len, preserve_crlf, exec_നിവേശം);
    _PyTok_ChunkClear(&decoded);
    എങ്കിൽ (stored < 0) {
        തിരികെ -1;
    }
    എങ്കിൽ (!utf8_only &&
            (encoding == ശൂന്യ || strcmp(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(_PyTok_SourceData(&tok->source), tok, 1)) {
        തിരികെ -1;
    }
    തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
_PyTok_StartDecoder(ഘടന tok_state *tok, സ്ഥിരം അക്ഷരം *errors)
{
    _PyTok_Reader *reader = tok->reader;
    എങ്കിൽ (reader->encoding == ശൂന്യ || reader->decoder != ശൂന്യ) {
        തിരികെ 0;
    }
    എങ്കിൽ (reader->kind == _PYTOK_READER_FILE &&
            strcmp(reader->encoding, "utf-8") == 0) {
        തിരികെ 0;
    }

    PyObject *codec = _PyCodec_LookupTextEncoding(reader->encoding, ശൂന്യ);
    എങ്കിൽ (codec != ശൂന്യ) {
        reader->decoder = _PyCodecInfo_GetIncrementalDecoder(codec, errors);
        Py_DECREF(codec);
    }
    എങ്കിൽ (reader->decoder == ശൂന്യ) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        എങ്കിൽ (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != ശൂന്യ ? tok->filename : Py_None);
        }
        തിരികെ -1;
    }
    തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
_PyTok_DecodeChunk(ഘടന tok_state *tok, _PyTok_Chunk *chunk, പൂർണ്ണസംഖ്യ final)
{
    _PyTok_Reader *reader = tok->reader;
    എങ്കിൽ (reader->decoder == ശൂന്യ) {
        തിരികെ 0;
    }
    പൂർണ്ണസംഖ്യ strip_bom = reader->kind == _PYTOK_READER_READLINE &&
        chunk->len >= UTF16_BOM_LENGTH &&
        (memcmp(chunk->data, UTF16_LE_BOM, UTF16_BOM_LENGTH) == 0 ||
         memcmp(chunk->data, UTF16_BE_BOM, UTF16_BOM_LENGTH) == 0);
    PyObject *നിവേശം;
    എങ്കിൽ (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            PyBytes_Check(chunk->owner) &&
            chunk->data == PyBytes_AS_STRING(chunk->owner)) {
        നിവേശം = Py_NewRef(chunk->owner);
    }
    അല്ലെങ്കിൽ {
        നിവേശം = PyBytes_FromStringAndSize(chunk->data, chunk->len);
    }
    എങ്കിൽ (നിവേശം == ശൂന്യ) {
        tok->done = E_NOMEM;
        തിരികെ -1;
    }
    PyObject *unicode = PyObject_CallMethodObjArgs(
        reader->decoder, &_Py_ID(decode), നിവേശം,
        final ? Py_True : Py_False, ശൂന്യ);
    Py_DECREF(നിവേശം);
    എങ്കിൽ (unicode == ശൂന്യ) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        എങ്കിൽ (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != ശൂന്യ ? tok->filename : Py_None);
        }
        തിരികെ -1;
    }
    എങ്കിൽ (!PyUnicode_Check(unicode)) {
        PyErr_Format(PyExc_TypeError,
                     "decoder should return a string result, not '%.200s'",
                     Py_TYPE(unicode)->tp_name);
        Py_DECREF(unicode);
        tok->done = E_DECODE;
        തിരികെ -1;
    }
    തിരികെ chunk_set_unicode(tok, chunk, unicode, strip_bom);
}

പൂർണ്ണസംഖ്യ
Py_Tok_Decoder_Has_Buffered_നിവേശം(ഘടന tok_state *tok)
{
    എങ്കിൽ (tok->reader->decoder == ശൂന്യ) {
        തിരികെ 0;
    }
    PyObject *state = PyObject_CallMethodNoArgs(
        tok->reader->decoder, &_Py_ID(getstate));
    എങ്കിൽ (state == ശൂന്യ) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        തിരികെ -1;
    }
    എങ്കിൽ (!PyTuple_Check(state) || PyTuple_GET_SIZE(state) != 2 ||
            !PyBytes_Check(PyTuple_GET_ITEM(state, 0)) ||
            !PyLong_Check(PyTuple_GET_ITEM(state, 1))) {
        Py_DECREF(state);
        PyErr_SetString(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->done = E_DECODE;
        തിരികെ -1;
    }
    പൂർണ്ണസംഖ്യ pending = PyBytes_GET_SIZE(PyTuple_GET_ITEM(state, 0)) != 0;
    Py_DECREF(state);
    തിരികെ pending;
}
