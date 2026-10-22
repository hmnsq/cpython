#অন্তর্ভাব "Python.h"
#অন্তর্ভাব "pycore_codecs.h"
#অন্তর্ভাব "pycore_global_strings.h"
#অন্তর্ভাব "pycore_runtime.h"
#অন্তর্ভাব "errcode.h"

#অন্তর্ভাব "reader_internal.h"
#অন্তর্ভাব "helpers.h"
#অন্তর্ভাব "../lexer/state.h"

#define UNICODE_BOM 0xFEFF
#define UTF16_LE_BOM "\xff\xfe"
#define UTF16_BE_BOM "\xfe\xff"
#define UTF16_BOM_দৈর্ঘ্য ((Py_ssize_t)(আকার(UTF16_LE_BOM) - 1))
#define UTF8_BOM "\xef\xbb\xbf"
#define UTF8_BOM_দৈর্ঘ্য ((Py_ssize_t)(আকার(UTF8_BOM) - 1))

অক্ষর *
_PyTok_CopyBytes(ধ্রুবক অক্ষর *তথ্য, Py_ssize_t len)
{
    যদি (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        ফেরত ফাঁকা;
    }
    অক্ষর *copy = PyMem_Malloc((আকার_t)len + 1);
    যদি (copy == ফাঁকা) {
        PyErr_NoMemory();
        ফেরত ফাঁকা;
    }
    memcpy(copy, তথ্য, (আকার_t)len);
    copy[len] = '\0';
    ফেরত copy;
}

স্থির শূন্য
chunk_release_তথ্য(_PyTok_Chunk *chunk)
{
    নির্বাচন (chunk->ownership) {
        ক্ষেত্র _PYTOK_CHUNK_BORROWED:
            বিরতি;
        ক্ষেত্র _PYTOK_CHUNK_PYMEM:
            PyMem_Free(chunk->তথ্য);
            বিরতি;
        ক্ষেত্র _PYTOK_CHUNK_PYOBJECT:
            Py_DECREF(chunk->owner);
            বিরতি;
    }
}

শূন্য
_PyTok_ChunkClear(_PyTok_Chunk *chunk)
{
    chunk_release_তথ্য(chunk);
    *chunk = (_PyTok_Chunk){0};
}

স্থির পূর্ণসংখ্যা
chunk_set_unicode(কাঠামো tok_state *tok, _PyTok_Chunk *chunk,
                  PyObject *unicode, পূর্ণসংখ্যা strip_bom)
{
    Py_ssize_t utf8_len;
    ধ্রুবক অক্ষর *utf8 = Py_Unicode_As_UTF8_And_আকার(unicode, &utf8_len);
    যদি (utf8 == ফাঁকা) {
        Py_DECREF(unicode);
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        ফেরত -1;
    }
    যদি (strip_bom && Py_Unicode_GET_দৈর্ঘ্য(unicode) > 0 &&
            PyUnicode_ReadChar(unicode, 0) == UNICODE_BOM) {
        utf8 += UTF8_BOM_দৈর্ঘ্য;
        utf8_len -= UTF8_BOM_দৈর্ঘ্য;
    }
    chunk_release_তথ্য(chunk);
    chunk->owner = unicode;
    chunk->তথ্য = (অক্ষর *)utf8;
    chunk->len = utf8_len;
    chunk->ownership = _PYTOK_CHUNK_PYOBJECT;
    ফেরত 0;
}

অক্ষর *
_PyTok_NormalizeNewlines(ধ্রুবক অক্ষর *তথ্য, Py_ssize_t len,
                         Py_ssize_t *out_len)
{
    যদি (len < 0 || len == PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        ফেরত ফাঁকা;
    }
    অক্ষর *ফলাফল = PyMem_Malloc((আকার_t)len + 1);
    যদি (ফলাফল == ফাঁকা) {
        PyErr_NoMemory();
        ফেরত ফাঁকা;
    }
    Py_ssize_t লিখ = 0;
    জন্য (Py_ssize_t পঠ = 0; পঠ < len; পঠ++) {
        অক্ষর c = তথ্য[পঠ];
        যদি (c == '\r') {
            যদি (পঠ + 1 < len && তথ্য[পঠ + 1] == '\n') {
                পঠ++;
            }
            c = '\n';
        }
        ফলাফল[লিখ++] = c;
    }
    ফলাফল[লিখ] = '\0';
    *out_len = লিখ;
    ফেরত ফলাফল;
}

পূর্ণসংখ্যা
_PyTok_SetEncoding(কাঠামো tok_state *tok, ধ্রুবক অক্ষর *encoding)
{
    অক্ষর *copy = _PyTok_CopyBytes(encoding, strlen(encoding));
    যদি (copy == ফাঁকা) {
        tok->done = E_NOMEM;
        ফেরত -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    ফেরত 0;
}

স্থির পূর্ণসংখ্যা
find_cookie(ধ্রুবক অক্ষর *সারি, Py_ssize_t len, অক্ষর **encoding, পূর্ণসংখ্যা *scan_পরবর্তী)
{
    Py_ssize_t i = 0;
    *encoding = ফাঁকা;
    *scan_পরবর্তী = 1;
    জন্য (; i < len; i++) {
        যদি (সারি[i] == '#') {
            বিরতি;
        }
        যদি (সারি[i] == '\n' || সারি[i] == '\r') {
            ফেরত 0;
        }
        যদি (সারি[i] != ' ' && সারি[i] != '\t' && সারি[i] != '\f') {
            *scan_পরবর্তী = 0;
            ফেরত 0;
        }
    }
    ধ্রুবক Py_ssize_t marker_len = আকার("coding") - 1;
    জন্য (; i < len - marker_len; i++) {
        যদি (memcmp(সারি + i, "coding", marker_len) != 0) {
            চালু;
        }
        ধ্রুবক অক্ষর *cursor = সারি + i + marker_len;
        যদি (*cursor != ':' && *cursor != '=') {
            চালু;
        }
        করো {
            cursor++;
        } যখন (cursor < সারি + len &&
                 (*cursor == ' ' || *cursor == '\t'));
        ধ্রুবক অক্ষর *start = cursor;
        যখন (cursor < সারি + len &&
                (Py_ISALNUM(*cursor) || *cursor == '-' ||
                 *cursor == '_' || *cursor == '.')) {
            cursor++;
        }
        যদি (cursor == start) {
            চালু;
        }
        Py_ssize_t encoding_len = cursor - start;
        // Include the longest recognized prefix and its NUL terminator.
        অক্ষর normalized[আকার("iso-latin-1-")];
        ধ্রুবক Py_ssize_t prefix_len = Py_MIN(
            encoding_len, (Py_ssize_t)আকার(normalized) - 1);
        Py_ssize_t n;
        জন্য (n = 0; n < prefix_len; n++) {
            normalized[n] = start[n] == '_' ? '-' : Py_TOLOWER(start[n]);
        }
        normalized[n] = '\0';
        ধ্রুবক অক্ষর *canonical = start;
        যদি (strcmp(normalized, "utf-8") == 0 ||
                strncmp(normalized, "utf-8-", আকার("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        অন্যথা যদি (strcmp(normalized, "latin-1") == 0 ||
                 strcmp(normalized, "iso-8859-1") == 0 ||
                 strcmp(normalized, "iso-latin-1") == 0 ||
                 strncmp(normalized, "latin-1-", আকার("latin-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-8859-1-", আকার("iso-8859-1-") - 1) == 0 ||
                 strncmp(normalized, "iso-latin-1-", আকার("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        যদি (canonical != start) {
            encoding_len = strlen(canonical);
        }
        *encoding = _PyTok_CopyBytes(canonical, encoding_len);
        যদি (*encoding == ফাঁকা) {
            ফেরত -1;
        }
        *scan_পরবর্তী = 0;
        ফেরত 0;
    }
    ফেরত 0;
}

Py_Tok_Encoding_ফলাফল
_PyTok_DetectEncoding(কাঠামো tok_state *tok, ধ্রুবক _PyTok_Chunk *first,
                      ধ্রুবক _PyTok_Chunk *second, পূর্ণসংখ্যা final,
                      Py_ssize_t *bom_len)
{
    পূর্ণসংখ্যা bom = first->len >= UTF8_BOM_দৈর্ঘ্য &&
        memcmp(first->তথ্য, UTF8_BOM, UTF8_BOM_দৈর্ঘ্য) == 0;
    *bom_len = bom ? UTF8_BOM_দৈর্ঘ্য : 0;

    অক্ষর *cookie = ফাঁকা;
    পূর্ণসংখ্যা scan_পরবর্তী = 0;
    পূর্ণসংখ্যা cookie_সারি = 1;
    ধ্রুবক অক্ষর *first_তথ্য = first->তথ্য + *bom_len;
    Py_ssize_t first_len = first->len - *bom_len;
    যদি (find_cookie(first_তথ্য, first_len, &cookie, &scan_পরবর্তী) < 0) {
        ফেরত _PYTOK_ENCODING_ERROR;
    }
    যদি (cookie == ফাঁকা && scan_পরবর্তী && second != ফাঁকা) {
        যদি (find_cookie(second->তথ্য, second->len, &cookie, &scan_পরবর্তী) < 0) {
            ফেরত _PYTOK_ENCODING_ERROR;
        }
        cookie_সারি = 2;
    }
    অন্যথা যদি (cookie == ফাঁকা && scan_পরবর্তী && !final) {
        ফেরত PYTOK_ENCODING_NEED_SECOND_সারি;
    }

    যদি (cookie == ফাঁকা) {
        যদি (bom && _PyTok_SetEncoding(tok, "utf-8") < 0) {
            ফেরত _PYTOK_ENCODING_ERROR;
        }
        ফেরত _PYTOK_ENCODING_DONE;
    }
    যদি (bom && strcmp(cookie, "utf-8") != 0) {
        ধ্রুবক _PyTok_Chunk *সারি = cookie_সারি == 2 ? second : first;
        ধ্রুবক অক্ষর *সারি_তথ্য = সারি->তথ্য + (cookie_সারি == 1 ? *bom_len : 0);
        Py_ssize_t সারি_len = সারি->len - (cookie_সারি == 1 ? *bom_len : 0);
        পূর্ণসংখ্যা end_col = (পূর্ণসংখ্যা)Py_MIN(সারি_len, INT_MAX);
        যদি (end_col > 0 && (সারি_তথ্য[end_col - 1] == '\n' ||
                            সারি_তথ্য[end_col - 1] == '\r')) {
            end_col--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, সারি_তথ্য, 0, cookie_সারি, 0, end_col,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        ফেরত _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    ফেরত _PYTOK_ENCODING_DONE;
}

পূর্ণসংখ্যা
_PyTok_DecodeOnce(কাঠামো tok_state *tok, _PyTok_Chunk *chunk,
                  ধ্রুবক অক্ষর *encoding, ধ্রুবক অক্ষর *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->তথ্য, chunk->len, encoding, errors);
    যদি (unicode == ফাঁকা) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        ফেরত -1;
    }
    ফেরত chunk_set_unicode(tok, chunk, unicode, 0);
}

স্থির Py_ssize_t
raw_সারি_দৈর্ঘ্য(ধ্রুবক অক্ষর *তথ্য, Py_ssize_t len)
{
    জন্য (Py_ssize_t i = 0; i < len; i++) {
        যদি (তথ্য[i] == '\n') {
            ফেরত i + 1;
        }
        যদি (তথ্য[i] == '\r') {
            ফেরত i + 1 < len && তথ্য[i + 1] == '\n' ? i + 2 : i + 1;
        }
    }
    ফেরত len;
}

স্থির পূর্ণসংখ্যা
store_prepared_source(কাঠামো tok_state *tok, ধ্রুবক অক্ষর *তথ্য, Py_ssize_t len,
                      পূর্ণসংখ্যা preserve_crlf, পূর্ণসংখ্যা add_final_newline)
{
    Py_ssize_t pos = 0;
    যখন (pos < len) {
        Py_ssize_t raw_সারি_len;
        যদি (preserve_crlf) {
            ধ্রুবক অক্ষর *newline = memchr(তথ্য + pos, '\n', len - pos);
            raw_সারি_len = newline == ফাঁকা
                ? len - pos : newline - তথ্য - pos + 1;
        }
        অন্যথা {
            raw_সারি_len = raw_সারি_দৈর্ঘ্য(তথ্য + pos, len - pos);
        }
        পূর্ণসংখ্যা terminated = preserve_crlf
            ? তথ্য[pos + raw_সারি_len - 1] == '\n'
            : তথ্য[pos + raw_সারি_len - 1] == '\n' ||
              তথ্য[pos + raw_সারি_len - 1] == '\r';
        পূর্ণসংখ্যা add_newline = add_final_newline &&
            pos + raw_সারি_len == len && !terminated;
        ধ্রুবক অক্ষর *সারি = তথ্য + pos;
        // raw_line_length stops at the first CR or LF, so any CR is in
        // the line terminator.
        Py_ssize_t terminator_len = 0;
        যদি (!preserve_crlf) {
            যদি (সারি[raw_সারি_len - 1] == '\r') {
                terminator_len = 1;
            }
            অন্যথা যদি (raw_সারি_len > 1 && সারি[raw_সারি_len - 2] == '\r') {
                terminator_len = 2;
            }
        }
        _PyTok_Off appended;
        যদি (terminator_len || add_newline) {
            appended = Py_Tok_Source_Append_সারি_With_Newline(
                &tok->source, সারি, raw_সারি_len - terminator_len);
        }
        অন্যথা {
            appended = Py_Tok_Source_Append_সারি(&tok->source, সারি, raw_সারি_len);
        }
        যদি (appended < 0) {
            tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
                ? E_NOMEM : E_ERROR;
            ফেরত -1;
        }
        tok->reader->prepared_final_newline = add_newline;
        pos += raw_সারি_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    যদি (add_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || তথ্য[len - 1] != '\r'))
    {
        যদি (Py_Tok_Source_Append_সারি(&tok->source, "\n", 1) < 0) {
            ফেরত -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    ফেরত 0;
}

পূর্ণসংখ্যা
_PyTok_PrepareString(কাঠামো tok_state *tok, ধ্রুবক অক্ষর *প্রবেশ, পূর্ণসংখ্যা utf8_only,
                     পূর্ণসংখ্যা exec_প্রবেশ, পূর্ণসংখ্যা preserve_crlf)
{
    Py_ssize_t raw_len = strlen(প্রবেশ);
    অক্ষর *raw = (অক্ষর *)প্রবেশ;

    যদি (utf8_only) {
        যদি (_PyTok_SetEncoding(tok, "utf-8") < 0) {
            ফেরত -1;
        }
    }
    অন্যথা {
        Py_ssize_t first_original_len = raw_সারি_দৈর্ঘ্য(raw, raw_len);
        _PyTok_Chunk first = {
            .তথ্য = raw,
            .len = first_original_len,
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        _PyTok_Chunk second = {0};
        পূর্ণসংখ্যা have_second = first_original_len < raw_len;
        যদি (have_second) {
            second.তথ্য = raw + first_original_len;
            second.len = raw_সারি_দৈর্ঘ্য(second.তথ্য,
                                         raw_len - first_original_len);
        }
        Py_ssize_t bom_len;
        Py_Tok_Encoding_ফলাফল detection = _PyTok_DetectEncoding(
            tok, &first, have_second ? &second : ফাঁকা, 1, &bom_len);
        যদি (detection == _PYTOK_ENCODING_ERROR) {
            ফেরত -1;
        }
        raw += bom_len;
        raw_len -= bom_len;
    }

    _PyTok_Chunk decoded = {
        .তথ্য = raw,
        .len = raw_len,
        .ownership = _PYTOK_CHUNK_BORROWED,
    };
    ধ্রুবক অক্ষর *encoding = tok->reader->encoding;
    যদি (encoding != ফাঁকা && strcmp(encoding, "utf-8") != 0) {
        যদি (_PyTok_DecodeOnce(
                tok, &decoded, encoding, ফাঁকা) < 0) {
            ফেরত -1;
        }
    }

    পূর্ণসংখ্যা stored = store_prepared_source(
        tok, decoded.তথ্য, decoded.len, preserve_crlf, exec_প্রবেশ);
    _PyTok_ChunkClear(&decoded);
    যদি (stored < 0) {
        ফেরত -1;
    }
    যদি (!utf8_only &&
            (encoding == ফাঁকা || strcmp(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(Py_Tok_Source_তথ্য(&tok->source), tok, 1)) {
        ফেরত -1;
    }
    ফেরত 0;
}

পূর্ণসংখ্যা
_PyTok_StartDecoder(কাঠামো tok_state *tok, ধ্রুবক অক্ষর *errors)
{
    _PyTok_Reader *reader = tok->reader;
    যদি (reader->encoding == ফাঁকা || reader->decoder != ফাঁকা) {
        ফেরত 0;
    }
    যদি (reader->kind == _PYTOK_READER_FILE &&
            strcmp(reader->encoding, "utf-8") == 0) {
        ফেরত 0;
    }

    PyObject *codec = _PyCodec_LookupTextEncoding(reader->encoding, ফাঁকা);
    যদি (codec != ফাঁকা) {
        reader->decoder = _PyCodecInfo_GetIncrementalDecoder(codec, errors);
        Py_DECREF(codec);
    }
    যদি (reader->decoder == ফাঁকা) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        যদি (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != ফাঁকা ? tok->filename : Py_None);
        }
        ফেরত -1;
    }
    ফেরত 0;
}

পূর্ণসংখ্যা
_PyTok_DecodeChunk(কাঠামো tok_state *tok, _PyTok_Chunk *chunk, পূর্ণসংখ্যা final)
{
    _PyTok_Reader *reader = tok->reader;
    যদি (reader->decoder == ফাঁকা) {
        ফেরত 0;
    }
    পূর্ণসংখ্যা strip_bom = reader->kind == _PYTOK_READER_READLINE &&
        chunk->len >= UTF16_BOM_দৈর্ঘ্য &&
        (memcmp(chunk->তথ্য, UTF16_LE_BOM, UTF16_BOM_দৈর্ঘ্য) == 0 ||
         memcmp(chunk->তথ্য, UTF16_BE_BOM, UTF16_BOM_দৈর্ঘ্য) == 0);
    PyObject *প্রবেশ;
    যদি (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            PyBytes_Check(chunk->owner) &&
            chunk->তথ্য == PyBytes_AS_STRING(chunk->owner)) {
        প্রবেশ = Py_NewRef(chunk->owner);
    }
    অন্যথা {
        প্রবেশ = Py_Bytes_From_String_And_আকার(chunk->তথ্য, chunk->len);
    }
    যদি (প্রবেশ == ফাঁকা) {
        tok->done = E_NOMEM;
        ফেরত -1;
    }
    PyObject *unicode = PyObject_CallMethodObjArgs(
        reader->decoder, &_Py_ID(decode), প্রবেশ,
        final ? Py_True : Py_False, ফাঁকা);
    Py_DECREF(প্রবেশ);
    যদি (unicode == ফাঁকা) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        যদি (reader->kind == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != ফাঁকা ? tok->filename : Py_None);
        }
        ফেরত -1;
    }
    যদি (!PyUnicode_Check(unicode)) {
        PyErr_Format(PyExc_TypeError,
                     "decoder should return a string result, not '%.200s'",
                     Py_TYPE(unicode)->tp_name);
        Py_DECREF(unicode);
        tok->done = E_DECODE;
        ফেরত -1;
    }
    ফেরত chunk_set_unicode(tok, chunk, unicode, strip_bom);
}

পূর্ণসংখ্যা
Py_Tok_Decoder_Has_Buffered_প্রবেশ(কাঠামো tok_state *tok)
{
    যদি (tok->reader->decoder == ফাঁকা) {
        ফেরত 0;
    }
    PyObject *state = PyObject_CallMethodNoArgs(
        tok->reader->decoder, &_Py_ID(getstate));
    যদি (state == ফাঁকা) {
        tok->done = PyErr_ExceptionMatches(PyExc_MemoryError)
            ? E_NOMEM : E_DECODE;
        ফেরত -1;
    }
    যদি (!PyTuple_Check(state) || Py_Tuple_GET_আকার(state) != 2 ||
            !PyBytes_Check(PyTuple_GET_ITEM(state, 0)) ||
            !PyLong_Check(PyTuple_GET_ITEM(state, 1))) {
        Py_DECREF(state);
        PyErr_SetString(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->done = E_DECODE;
        ফেরত -1;
    }
    পূর্ণসংখ্যা pending = Py_Bytes_GET_আকার(PyTuple_GET_ITEM(state, 0)) != 0;
    Py_DECREF(state);
    ফেরত pending;
}
