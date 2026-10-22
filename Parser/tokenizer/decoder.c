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
#define UTF16_BOM_LENGTH ((Py_ssize_t)(आकार(UTF16_LE_BOM) - 1))
#define UTF8_BOM "\xef\xbb\xbf"
#define UTF8_BOM_LENGTH ((Py_ssize_t)(आकार(UTF8_BOM) - 1))

अक्षर *
_PyTok_CopyBytes(स्थिरांक अक्षर *data, Py_ssize_t len)
{
    जर (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        परत रिक्त;
    }
    अक्षर *copy = PyMem_Malloc((माप_प्रकार)len + 1);
    जर (copy == रिक्त) {
        PyErr_NoMemory();
        परत रिक्त;
    }
    स_नकल(copy, data, (माप_प्रकार)len);
    copy[len] = '\0';
    परत copy;
}

स्थिर शून्य
chunk_release_data(_PyTok_Chunk *chunk)
{
    निवड (chunk->ownership) {
        स्थिती _PYTOK_CHUNK_BORROWED:
            थांबा;
        स्थिती _PYTOK_CHUNK_PYMEM:
            PyMem_Free(chunk->data);
            थांबा;
        स्थिती _PYTOK_CHUNK_PYOBJECT:
            Py_DECREF(chunk->owner);
            थांबा;
    }
}

शून्य
_PyTok_ChunkClear(_PyTok_Chunk *chunk)
{
    chunk_release_data(chunk);
    *chunk = (_PyTok_Chunk){0};
}

स्थिर पूर्णांक
chunk_set_unicode(संरचना tok_state *tok, _PyTok_Chunk *chunk,
                  PyObject *unicode, पूर्णांक strip_bom)
{
    Py_ssize_t utf8_len;
    स्थिरांक अक्षर *utf8 = PyUnicode_AsUTF8AndSize(unicode, &utf8_len);
    जर (utf8 == रिक्त) {
        Py_DECREF(unicode);
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        परत -1;
    }
    जर (strip_bom && PyUnicode_GET_LENGTH(unicode) > 0 &&
            PyUnicode_ReadChar(unicode, 0) == UNICODE_BOM) {
        utf8 += UTF8_BOM_LENGTH;
        utf8_len -= UTF8_BOM_LENGTH;
    }
    chunk_release_data(chunk);
    chunk->owner = unicode;
    chunk->data = (अक्षर *)utf8;
    chunk->len = utf8_len;
    chunk->ownership = _PYTOK_CHUNK_PYOBJECT;
    परत 0;
}

अक्षर *
_PyTok_NormalizeNewlines(स्थिरांक अक्षर *data, Py_ssize_t len,
                         Py_ssize_t *out_len)
{
    जर (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        परत रिक्त;
    }
    अक्षर *result = PyMem_Malloc((माप_प्रकार)len + 1);
    जर (result == रिक्त) {
        PyErr_NoMemory();
        परत रिक्त;
    }
    Py_ssize_t लिख = 0;
    साठी (Py_ssize_t पठ = 0; पठ < len; पठ++) {
        अक्षर c = data[पठ];
        जर (c == '\r') {
            जर (पठ + 1 < len && data[पठ + 1] == '\n') {
                पठ++;
            }
            c = '\n';
        }
        result[लिख++] = c;
    }
    result[लिख] = '\0';
    *out_len = लिख;
    परत result;
}

पूर्णांक
_PyTok_SetEncoding(संरचना tok_state *tok, स्थिरांक अक्षर *encoding)
{
    अक्षर *copy = _PyTok_CopyBytes(encoding, म_माप(encoding));
    जर (copy == रिक्त) {
        tok->done = E_NOMEM;
        परत -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    परत 0;
}

स्थिर पूर्णांक
find_cookie(स्थिरांक अक्षर *ओळ, Py_ssize_t len, अक्षर **encoding, पूर्णांक *scan_next)
{
    Py_ssize_t i = 0;
    *encoding = रिक्त;
    *scan_next = 1;
    साठी (; i < len; i++) {
        जर (ओळ[i] == '#') {
            थांबा;
        }
        जर (ओळ[i] == '\n' || ओळ[i] == '\r') {
            परत 0;
        }
        जर (ओळ[i] != ' ' && ओळ[i] != '\t' && ओळ[i] != '\f') {
            *scan_next = 0;
            परत 0;
        }
    }
    स्थिरांक Py_ssize_t marker_len = आकार("coding") - 1;
    साठी (; i < len - marker_len; i++) {
        जर (स_भेद(ओळ + i, "coding", marker_len) != 0) {
            सुरूठेवा;
        }
        स्थिरांक अक्षर *cursor = ओळ + i + marker_len;
        जर (*cursor != ':' && *cursor != '=') {
            सुरूठेवा;
        }
        करा {
            cursor++;
        } पर्यंत (cursor < ओळ + len &&
                 (*cursor == ' ' || *cursor == '\t'));
        स्थिरांक अक्षर *start = cursor;
        पर्यंत (cursor < ओळ + len &&
                (Py_ISALNUM(*cursor) || *cursor == '-' ||
                 *cursor == '_' || *cursor == '.')) {
            cursor++;
        }
        जर (cursor == start) {
            सुरूठेवा;
        }
        Py_ssize_t encoding_len = cursor - start;
        // Include the longest recognized prefix and its NUL terminator.
        अक्षर normalized[आकार("iso-latin-1-")];
        स्थिरांक Py_ssize_t prefix_len = Py_MIN(
            encoding_len, (Py_ssize_t)आकार(normalized) - 1);
        Py_ssize_t n;
        साठी (n = 0; n < prefix_len; n++) {
            normalized[n] = start[n] == '_' ? '-' : Py_TOLOWER(start[n]);
        }
        normalized[n] = '\0';
        स्थिरांक अक्षर *canonical = start;
        जर (म_भेद(normalized, "utf-8") == 0 ||
                म_भेदन(normalized, "utf-8-", आकार("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        अन्यथा जर (म_भेद(normalized, "latin-1") == 0 ||
                 म_भेद(normalized, "iso-8859-1") == 0 ||
                 म_भेद(normalized, "iso-latin-1") == 0 ||
                 म_भेदन(normalized, "latin-1-", आकार("latin-1-") - 1) == 0 ||
                 म_भेदन(normalized, "iso-8859-1-", आकार("iso-8859-1-") - 1) == 0 ||
                 म_भेदन(normalized, "iso-latin-1-", आकार("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        जर (canonical != start) {
            encoding_len = म_माप(canonical);
        }
        *encoding = _PyTok_CopyBytes(canonical, encoding_len);
        जर (*encoding == रिक्त) {
            परत -1;
        }
        *scan_next = 0;
        परत 0;
    }
    परत 0;
}

_PyTok_EncodingResult
_PyTok_DetectEncoding(संरचना tok_state *tok, स्थिरांक _PyTok_Chunk *first,
                      स्थिरांक _PyTok_Chunk *second, पूर्णांक final,
                      Py_ssize_t *bom_len)
{
    पूर्णांक bom = first->len >= UTF8_BOM_LENGTH &&
        स_भेद(first->data, UTF8_BOM, UTF8_BOM_LENGTH) == 0;
    *bom_len = bom ? UTF8_BOM_LENGTH : 0;

    अक्षर *cookie = रिक्त;
    पूर्णांक scan_next = 0;
    पूर्णांक cookie_ओळ = 1;
    स्थिरांक अक्षर *first_data = first->data + *bom_len;
    Py_ssize_t first_len = first->len - *bom_len;
    जर (find_cookie(first_data, first_len, &cookie, &scan_next) < 0) {
        परत _PYTOK_ENCODING_ERROR;
    }
    जर (cookie == रिक्त && scan_next && second != रिक्त) {
        जर (find_cookie(second->data, second->len, &cookie, &scan_next) < 0) {
            परत _PYTOK_ENCODING_ERROR;
        }
        cookie_ओळ = 2;
    }
    अन्यथा जर (cookie == रिक्त && scan_next && !final) {
        परत PYTOK_ENCODING_NEED_SECOND_ओळ;
    }

    जर (cookie == रिक्त) {
        जर (bom && _PyTok_SetEncoding(tok, "utf-8") < 0) {
            परत _PYTOK_ENCODING_ERROR;
        }
        परत _PYTOK_ENCODING_DONE;
    }
    जर (bom && म_भेद(cookie, "utf-8") != 0) {
        स्थिरांक _PyTok_Chunk *ओळ = cookie_ओळ == 2 ? second : first;
        स्थिरांक अक्षर *ओळ_data = ओळ->data + (cookie_ओळ == 1 ? *bom_len : 0);
        Py_ssize_t ओळ_len = ओळ->len - (cookie_ओळ == 1 ? *bom_len : 0);
        पूर्णांक end_col = (पूर्णांक)Py_MIN(ओळ_len, पूर्णांक_उच्च);
        जर (end_col > 0 && (ओळ_data[end_col - 1] == '\n' ||
                            ओळ_data[end_col - 1] == '\r')) {
            end_col--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, ओळ_data, 0, cookie_ओळ, 0, end_col,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        परत _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    परत _PYTOK_ENCODING_DONE;
}

पूर्णांक
_PyTok_DecodeOnce(संरचना tok_state *tok, _PyTok_Chunk *chunk,
                  स्थिरांक अक्षर *encoding, स्थिरांक अक्षर *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->data, chunk->len, encoding, errors);
    जर (unicode == रिक्त) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        परत -1;
    }
    परत chunk_set_unicode(tok, chunk, unicode, 0);
}

स्थिर Py_ssize_t
raw_ओळ_length(स्थिरांक अक्षर *data, Py_ssize_t len)
{
    साठी (Py_ssize_t i = 0; i < len; i++) {
        जर (data[i] == '\n') {
            परत i + 1;
        }
        जर (data[i] == '\r') {
            परत i + 1 < len && data[i + 1] == '\n' ? i + 2 : i + 1;
        }
    }
    परत len;
}

स्थिर पूर्णांक
store_prepared_source(संरचना tok_state *tok, स्थिरांक अक्षर *data, Py_ssize_t len,
                      पूर्णांक preserve_crlf, पूर्णांक add_final_newline)
{
    Py_ssize_t pos = 0;
    पर्यंत (pos < len) {
        Py_ssize_t raw_ओळ_len;
        जर (preserve_crlf) {
            स्थिरांक अक्षर *newline = स_प्रथम(data + pos, '\n', len - pos);
            raw_ओळ_len = newline == रिक्त
                ? len - pos : newline - data - pos + 1;
        }
        अन्यथा {
            raw_ओळ_len = raw_ओळ_length(data + pos, len - pos);
        }
        पूर्णांक terminated = preserve_crlf
            ? data[pos + raw_ओळ_len - 1] == '\n'
            : data[pos + raw_ओळ_len - 1] == '\n' ||
              data[pos + raw_ओळ_len - 1] == '\r';
        पूर्णांक add_newline = add_final_newline &&
            pos + raw_ओळ_len == len && !terminated;
        स्थिरांक अक्षर *ओळ = data + pos;
        // raw_line_length stops at the first CR or LF, so any CR is in
        // the line terminator.
        Py_ssize_t terminator_len = 0;
        जर (!preserve_crlf) {
            जर (ओळ[raw_ओळ_len - 1] == '\r') {
                terminator_len = 1;
            }
            अन्यथा जर (raw_ओळ_len > 1 && ओळ[raw_ओळ_len - 2] == '\r') {
                terminator_len = 2;
            }
        }
        _PyTok_Off appended;
        जर (terminator_len || add_newline) {
            appended = Py_Tok_Source_Append_ओळ_With_Newline(
                &tok->source, ओळ, raw_ओळ_len - terminator_len);
        }
        अन्यथा {
            appended = Py_Tok_Source_Append_ओळ(&tok->source, ओळ, raw_ओळ_len);
        }
        जर (appended < 0) {
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            परत -1;
        }
        tok->reader->prepared_final_newline = add_newline;
        pos += raw_ओळ_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    जर (add_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || data[len - 1] != '\r'))
    {
        जर (Py_Tok_Source_Append_ओळ(&tok->source, "\n", 1) < 0) {
            परत -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    परत 0;
}

पूर्णांक
_PyTok_PrepareString(संरचना tok_state *tok, स्थिरांक अक्षर *आदान, पूर्णांक utf8_only,
                     पूर्णांक exec_आदान, पूर्णांक preserve_crlf)
{
    Py_ssize_t raw_len = म_माप(आदान);
    अक्षर *raw = (अक्षर *)आदान;

    जर (utf8_only) {
        जर (_PyTok_SetEncoding(tok, "utf-8") < 0) {
            परत -1;
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
        पूर्णांक have_second = first_original_len < raw_len;
        जर (have_second) {
            second.data = raw + first_original_len;
            second.len = raw_ओळ_length(second.data,
                                         raw_len - first_original_len);
        }
        Py_ssize_t bom_len;
        _PyTok_EncodingResult detection = _PyTok_DetectEncoding(
            tok, &first, have_second ? &second : रिक्त, 1, &bom_len);
        जर (detection == _PYTOK_ENCODING_ERROR) {
            परत -1;
        }
        raw += bom_len;
        raw_len -= bom_len;
    }

    _PyTok_Chunk decoded = {
        .data = raw,
        .len = raw_len,
        .ownership = _PYTOK_CHUNK_BORROWED,
    };
    स्थिरांक अक्षर *encoding = tok->reader->encoding;
    जर (encoding != रिक्त && म_भेद(encoding, "utf-8") != 0) {
        जर (_PyTok_DecodeOnce(
                tok, &decoded, encoding, रिक्त) < 0) {
            परत -1;
        }
    }

    पूर्णांक stored = store_prepared_source(
        tok, decoded.data, decoded.len, preserve_crlf, exec_आदान);
    _PyTok_ChunkClear(&decoded);
    जर (stored < 0) {
        परत -1;
    }
    जर (!utf8_only &&
            (encoding == रिक्त || म_भेद(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(_PyTok_SourceData(&tok->source), tok, 1)) {
        परत -1;
    }
    परत 0;
}

पूर्णांक
_PyTok_StartDecoder(संरचना tok_state *tok, स्थिरांक अक्षर *errors)
{
    _PyTok_Reader *reader = tok->reader;
    जर (reader->encoding == रिक्त || reader->decoder != रिक्त) {
        परत 0;
    }
    जर (reader->kind == _PYTOK_READER_FILE &&
            म_भेद(reader->encoding, "utf-8") == 0) {
        परत 0;
    }

    PyObject *codec = _PyCodec_LookupTextEncoding(reader->encoding, रिक्त);
    जर (codec != रिक्त) {
        reader->decoder = _PyCodecInfo_GetIncrementalDecoder(codec, errors);
        Py_DECREF(codec);
    }
    जर (reader->decoder == रिक्त) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        जर (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != रिक्त ? tok->filename : Py_None);
        }
        परत -1;
    }
    परत 0;
}

पूर्णांक
_PyTok_DecodeChunk(संरचना tok_state *tok, _PyTok_Chunk *chunk, पूर्णांक final)
{
    _PyTok_Reader *reader = tok->reader;
    जर (reader->decoder == रिक्त) {
        परत 0;
    }
    पूर्णांक strip_bom = reader->kind == _PYTOK_READER_READLINE &&
        chunk->len >= UTF16_BOM_LENGTH &&
        (स_भेद(chunk->data, UTF16_LE_BOM, UTF16_BOM_LENGTH) == 0 ||
         स_भेद(chunk->data, UTF16_BE_BOM, UTF16_BOM_LENGTH) == 0);
    PyObject *आदान;
    जर (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            PyBytes_Check(chunk->owner) &&
            chunk->data == PyBytes_AS_STRING(chunk->owner)) {
        आदान = Py_NewRef(chunk->owner);
    }
    अन्यथा {
        आदान = PyBytes_FromStringAndSize(chunk->data, chunk->len);
    }
    जर (आदान == रिक्त) {
        tok->done = E_NOMEM;
        परत -1;
    }
    PyObject *unicode = PyObject_CallMethodObjArgs(
        reader->decoder, &_Py_ID(decode), आदान,
        final ? Py_True : Py_False, रिक्त);
    Py_DECREF(आदान);
    जर (unicode == रिक्त) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        जर (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != रिक्त ? tok->filename : Py_None);
        }
        परत -1;
    }
    जर (!PyUnicode_Check(unicode)) {
        PyErr_Format(PyExc_TypeError,
                     "decoder should return a string result, not '%.200s'",
                     Py_TYPE(unicode)->tp_name);
        Py_DECREF(unicode);
        tok->done = E_DECODE;
        परत -1;
    }
    परत chunk_set_unicode(tok, chunk, unicode, strip_bom);
}

पूर्णांक
Py_Tok_Decoder_Has_Buffered_आदान(संरचना tok_state *tok)
{
    जर (tok->reader->decoder == रिक्त) {
        परत 0;
    }
    PyObject *state = PyObject_CallMethodNoArgs(
        tok->reader->decoder, &_Py_ID(getstate));
    जर (state == रिक्त) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        परत -1;
    }
    जर (!PyTuple_Check(state) || PyTuple_GET_SIZE(state) != 2 ||
            !PyBytes_Check(PyTuple_GET_ITEM(state, 0)) ||
            !PyLong_Check(PyTuple_GET_ITEM(state, 1))) {
        Py_DECREF(state);
        PyErr_SetString(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->done = E_DECODE;
        परत -1;
    }
    पूर्णांक pending = PyBytes_GET_SIZE(PyTuple_GET_ITEM(state, 0)) != 0;
    Py_DECREF(state);
    परत pending;
}
