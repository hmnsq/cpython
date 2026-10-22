#अन्तर्भाव "Python.h"
#अन्तर्भाव "pycore_codecs.h"
#अन्तर्भाव "pycore_global_strings.h"
#अन्तर्भाव "pycore_runtime.h"
#अन्तर्भाव "errcode.h"

#अन्तर्भाव "reader_internal.h"
#अन्तर्भाव "helpers.h"
#अन्तर्भाव "../lexer/state.h"

#परिभाषय UNICODE_BOM 0xFEFF
#परिभाषय UTF16_LE_BOM "\xff\xfe"
#परिभाषय UTF16_BE_BOM "\xfe\xff"
#परिभाषय UTF16_BOM_दीर्घता ((Py_ssize_t)(परिमाण(UTF16_LE_BOM) - 1))
#परिभाषय UTF8_BOM "\xef\xbb\xbf"
#परिभाषय UTF8_BOM_दीर्घता ((Py_ssize_t)(परिमाण(UTF8_BOM) - 1))

वर्ण *
Py_Tok_Copy_अष्टकानि(ध्रुव वर्ण *दत्त, Py_ssize_t len)
{
    यदि (len < 0 || len == PY_SSIZE_T_MAX) {
        Py_Err_No_स्मृतिः();
        प्रत्यागम रिक्त;
    }
    वर्ण *copy = PyMem_Malloc((आकार_t)len + 1);
    यदि (copy == रिक्त) {
        Py_Err_No_स्मृतिः();
        प्रत्यागम रिक्त;
    }
    स्मृतिं_प्रतिलिख(copy, दत्त, (आकार_t)len);
    copy[len] = '\0';
    प्रत्यागम copy;
}

स्थिर शून्य
chunk_release_दत्त(_PyTok_Chunk *chunk)
{
    विकल्प (chunk->ownership) {
        पक्ष _PYTOK_CHUNK_BORROWED:
            भङ्ग;
        पक्ष _PYTOK_CHUNK_PYMEM:
            PyMem_Free(chunk->दत्त);
            भङ्ग;
        पक्ष _PYTOK_CHUNK_PYOBJECT:
            Py_DECREF(chunk->owner);
            भङ्ग;
    }
}

शून्य
_PyTok_ChunkClear(_PyTok_Chunk *chunk)
{
    chunk_release_दत्त(chunk);
    *chunk = (_PyTok_Chunk){0};
}

स्थिर पूर्णाङ्क
chunk_समुच्चयः_unicode(संरचना tok_state *tok, _PyTok_Chunk *chunk,
                  PyObject *unicode, पूर्णाङ्क strip_bom)
{
    Py_ssize_t utf8_len;
    ध्रुव वर्ण *utf8 = Py_Unicode_As_UTF8_And_आकार(unicode, &utf8_len);
    यदि (utf8 == रिक्त) {
        Py_DECREF(unicode);
        tok->समाप्तम् = PyErr_ExceptionMatches(Py_Exc_स्मृतिः_Error)
            ? E_NOMEM : E_DECODE;
        प्रत्यागम -1;
    }
    यदि (strip_bom && Py_Unicode_GET_दीर्घता(unicode) > 0 &&
            PyUnicode_ReadChar(unicode, 0) == UNICODE_BOM) {
        utf8 += UTF8_BOM_दीर्घता;
        utf8_len -= UTF8_BOM_दीर्घता;
    }
    chunk_release_दत्त(chunk);
    chunk->owner = unicode;
    chunk->दत्त = (वर्ण *)utf8;
    chunk->len = utf8_len;
    chunk->ownership = _PYTOK_CHUNK_PYOBJECT;
    प्रत्यागम 0;
}

वर्ण *
_PyTok_NormalizeNewlines(ध्रुव वर्ण *दत्त, Py_ssize_t len,
                         Py_ssize_t *बहिर्गमनम्_len)
{
    यदि (len < 0 || len == PY_SSIZE_T_MAX) {
        Py_Err_No_स्मृतिः();
        प्रत्यागम रिक्त;
    }
    वर्ण *परिणाम = PyMem_Malloc((आकार_t)len + 1);
    यदि (परिणाम == रिक्त) {
        Py_Err_No_स्मृतिः();
        प्रत्यागम रिक्त;
    }
    Py_ssize_t लिख = 0;
    प्रति (Py_ssize_t पठ = 0; पठ < len; पठ++) {
        वर्ण c = दत्त[पठ];
        यदि (c == '\r') {
            यदि (पठ + 1 < len && दत्त[पठ + 1] == '\n') {
                पठ++;
            }
            c = '\n';
        }
        परिणाम[लिख++] = c;
    }
    परिणाम[लिख] = '\0';
    *बहिर्गमनम्_len = लिख;
    प्रत्यागम परिणाम;
}

पूर्णाङ्क
Py_Tok_समुच्चयः_Encoding(संरचना tok_state *tok, ध्रुव वर्ण *encoding)
{
    वर्ण *copy = Py_Tok_Copy_अष्टकानि(encoding, वाक्यदैर्घ्यम्(encoding));
    यदि (copy == रिक्त) {
        tok->समाप्तम् = E_NOMEM;
        प्रत्यागम -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    प्रत्यागम 0;
}

स्थिर पूर्णाङ्क
find_cookie(ध्रुव वर्ण *पङ्क्तिः, Py_ssize_t len, वर्ण **encoding, पूर्णाङ्क *अवलोकनम्_अग्र)
{
    Py_ssize_t i = 0;
    *encoding = रिक्त;
    *अवलोकनम्_अग्र = 1;
    प्रति (; i < len; i++) {
        यदि (पङ्क्तिः[i] == '#') {
            भङ्ग;
        }
        यदि (पङ्क्तिः[i] == '\n' || पङ्क्तिः[i] == '\r') {
            प्रत्यागम 0;
        }
        यदि (पङ्क्तिः[i] != ' ' && पङ्क्तिः[i] != '\t' && पङ्क्तिः[i] != '\f') {
            *अवलोकनम्_अग्र = 0;
            प्रत्यागम 0;
        }
    }
    ध्रुव Py_ssize_t marker_len = परिमाण("coding") - 1;
    प्रति (; i < len - marker_len; i++) {
        यदि (memcmp(पङ्क्तिः + i, "coding", marker_len) != 0) {
            अनुवर्त;
        }
        ध्रुव वर्ण *cursor = पङ्क्तिः + i + marker_len;
        यदि (*cursor != ':' && *cursor != '=') {
            अनुवर्त;
        }
        कुरु {
            cursor++;
        } यावत् (cursor < पङ्क्तिः + len &&
                 (*cursor == ' ' || *cursor == '\t'));
        ध्रुव वर्ण *start = cursor;
        यावत् (cursor < पङ्क्तिः + len &&
                (Py_ISALNUM(*cursor) || *cursor == '-' ||
                 *cursor == '_' || *cursor == '.')) {
            cursor++;
        }
        यदि (cursor == start) {
            अनुवर्त;
        }
        Py_ssize_t encoding_len = cursor - start;
        // Include the longest recognized prefix and its NUL terminator.
        वर्ण normalized[परिमाण("iso-latin-1-")];
        ध्रुव Py_ssize_t उपसर्गः_len = Py_MIN(
            encoding_len, (Py_ssize_t)परिमाण(normalized) - 1);
        Py_ssize_t n;
        प्रति (n = 0; n < उपसर्गः_len; n++) {
            normalized[n] = start[n] == '_' ? '-' : Py_TOLOWER(start[n]);
        }
        normalized[n] = '\0';
        ध्रुव वर्ण *canonical = start;
        यदि (वाक्यं_तुलय(normalized, "utf-8") == 0 ||
                वाक्ये_तुलय(normalized, "utf-8-", परिमाण("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        अन्यथा यदि (वाक्यं_तुलय(normalized, "latin-1") == 0 ||
                 वाक्यं_तुलय(normalized, "iso-8859-1") == 0 ||
                 वाक्यं_तुलय(normalized, "iso-latin-1") == 0 ||
                 वाक्ये_तुलय(normalized, "latin-1-", परिमाण("latin-1-") - 1) == 0 ||
                 वाक्ये_तुलय(normalized, "iso-8859-1-", परिमाण("iso-8859-1-") - 1) == 0 ||
                 वाक्ये_तुलय(normalized, "iso-latin-1-", परिमाण("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        यदि (canonical != start) {
            encoding_len = वाक्यदैर्घ्यम्(canonical);
        }
        *encoding = Py_Tok_Copy_अष्टकानि(canonical, encoding_len);
        यदि (*encoding == रिक्त) {
            प्रत्यागम -1;
        }
        *अवलोकनम्_अग्र = 0;
        प्रत्यागम 0;
    }
    प्रत्यागम 0;
}

Py_Tok_Encoding_परिणाम
_PyTok_DetectEncoding(संरचना tok_state *tok, ध्रुव _PyTok_Chunk *first,
                      ध्रुव _PyTok_Chunk *second, पूर्णाङ्क final,
                      Py_ssize_t *bom_len)
{
    पूर्णाङ्क bom = first->len >= UTF8_BOM_दीर्घता &&
        memcmp(first->दत्त, UTF8_BOM, UTF8_BOM_दीर्घता) == 0;
    *bom_len = bom ? UTF8_BOM_दीर्घता : 0;

    वर्ण *cookie = रिक्त;
    पूर्णाङ्क अवलोकनम्_अग्र = 0;
    पूर्णाङ्क cookie_पङ्क्तिः = 1;
    ध्रुव वर्ण *first_दत्त = first->दत्त + *bom_len;
    Py_ssize_t first_len = first->len - *bom_len;
    यदि (find_cookie(first_दत्त, first_len, &cookie, &अवलोकनम्_अग्र) < 0) {
        प्रत्यागम _PYTOK_ENCODING_ERROR;
    }
    यदि (cookie == रिक्त && अवलोकनम्_अग्र && second != रिक्त) {
        यदि (find_cookie(second->दत्त, second->len, &cookie, &अवलोकनम्_अग्र) < 0) {
            प्रत्यागम _PYTOK_ENCODING_ERROR;
        }
        cookie_पङ्क्तिः = 2;
    }
    अन्यथा यदि (cookie == रिक्त && अवलोकनम्_अग्र && !final) {
        प्रत्यागम PYTOK_ENCODING_NEED_SECOND_पङ्क्तिः;
    }

    यदि (cookie == रिक्त) {
        यदि (bom && Py_Tok_समुच्चयः_Encoding(tok, "utf-8") < 0) {
            प्रत्यागम _PYTOK_ENCODING_ERROR;
        }
        प्रत्यागम PYTOK_ENCODING_समाप्तम्;
    }
    यदि (bom && वाक्यं_तुलय(cookie, "utf-8") != 0) {
        ध्रुव _PyTok_Chunk *पङ्क्तिः = cookie_पङ्क्तिः == 2 ? second : first;
        ध्रुव वर्ण *पङ्क्तिः_दत्त = पङ्क्तिः->दत्त + (cookie_पङ्क्तिः == 1 ? *bom_len : 0);
        Py_ssize_t पङ्क्तिः_len = पङ्क्तिः->len - (cookie_पङ्क्तिः == 1 ? *bom_len : 0);
        पूर्णाङ्क अन्तः_col = (पूर्णाङ्क)Py_MIN(पङ्क्तिः_len, अधिकतम_पूर्णाङ्कः);
        यदि (अन्तः_col > 0 && (पङ्क्तिः_दत्त[अन्तः_col - 1] == '\n' ||
                            पङ्क्तिः_दत्त[अन्तः_col - 1] == '\r')) {
            अन्तः_col--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, पङ्क्तिः_दत्त, 0, cookie_पङ्क्तिः, 0, अन्तः_col,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        प्रत्यागम _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    प्रत्यागम PYTOK_ENCODING_समाप्तम्;
}

पूर्णाङ्क
_PyTok_DecodeOnce(संरचना tok_state *tok, _PyTok_Chunk *chunk,
                  ध्रुव वर्ण *encoding, ध्रुव वर्ण *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->दत्त, chunk->len, encoding, errors);
    यदि (unicode == रिक्त) {
        tok->समाप्तम् = PyErr_ExceptionMatches(Py_Exc_स्मृतिः_Error)
            ? E_NOMEM : E_DECODE;
        प्रत्यागम -1;
    }
    प्रत्यागम chunk_समुच्चयः_unicode(tok, chunk, unicode, 0);
}

स्थिर Py_ssize_t
raw_पङ्क्तिः_दीर्घता(ध्रुव वर्ण *दत्त, Py_ssize_t len)
{
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        यदि (दत्त[i] == '\n') {
            प्रत्यागम i + 1;
        }
        यदि (दत्त[i] == '\r') {
            प्रत्यागम i + 1 < len && दत्त[i + 1] == '\n' ? i + 2 : i + 1;
        }
    }
    प्रत्यागम len;
}

स्थिर पूर्णाङ्क
store_prepared_source(संरचना tok_state *tok, ध्रुव वर्ण *दत्त, Py_ssize_t len,
                      पूर्णाङ्क preserve_crlf, पूर्णाङ्क सङ्कलय_final_newline)
{
    Py_ssize_t pos = 0;
    यावत् (pos < len) {
        Py_ssize_t raw_पङ्क्तिः_len;
        यदि (preserve_crlf) {
            ध्रुव वर्ण *newline = memchr(दत्त + pos, '\n', len - pos);
            raw_पङ्क्तिः_len = newline == रिक्त
                ? len - pos : newline - दत्त - pos + 1;
        }
        अन्यथा {
            raw_पङ्क्तिः_len = raw_पङ्क्तिः_दीर्घता(दत्त + pos, len - pos);
        }
        पूर्णाङ्क terminated = preserve_crlf
            ? दत्त[pos + raw_पङ्क्तिः_len - 1] == '\n'
            : दत्त[pos + raw_पङ्क्तिः_len - 1] == '\n' ||
              दत्त[pos + raw_पङ्क्तिः_len - 1] == '\r';
        पूर्णाङ्क सङ्कलय_newline = सङ्कलय_final_newline &&
            pos + raw_पङ्क्तिः_len == len && !terminated;
        ध्रुव वर्ण *पङ्क्तिः = दत्त + pos;
        // raw_line_length stops at the first CR or LF, so any CR is in
        // the line terminator.
        Py_ssize_t terminator_len = 0;
        यदि (!preserve_crlf) {
            यदि (पङ्क्तिः[raw_पङ्क्तिः_len - 1] == '\r') {
                terminator_len = 1;
            }
            अन्यथा यदि (raw_पङ्क्तिः_len > 1 && पङ्क्तिः[raw_पङ्क्तिः_len - 2] == '\r') {
                terminator_len = 2;
            }
        }
        _PyTok_Off appended;
        यदि (terminator_len || सङ्कलय_newline) {
            appended = Py_Tok_Source_Append_पङ्क्तिः_With_Newline(
                &tok->source, पङ्क्तिः, raw_पङ्क्तिः_len - terminator_len);
        }
        अन्यथा {
            appended = Py_Tok_Source_Append_पङ्क्तिः(&tok->source, पङ्क्तिः, raw_पङ्क्तिः_len);
        }
        यदि (appended < 0) {
            tok->समाप्तम् = PyErr_ExceptionMatches(Py_Exc_स्मृतिः_Error)
                ? E_NOMEM : E_ERROR;
            प्रत्यागम -1;
        }
        tok->reader->prepared_final_newline = सङ्कलय_newline;
        pos += raw_पङ्क्तिः_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    यदि (सङ्कलय_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || दत्त[len - 1] != '\r'))
    {
        यदि (Py_Tok_Source_Append_पङ्क्तिः(&tok->source, "\n", 1) < 0) {
            प्रत्यागम -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    प्रत्यागम 0;
}

पूर्णाङ्क
_PyTok_PrepareString(संरचना tok_state *tok, ध्रुव वर्ण *निवेशः, पूर्णाङ्क utf8_only,
                     पूर्णाङ्क exec_निवेशः, पूर्णाङ्क preserve_crlf)
{
    Py_ssize_t raw_len = वाक्यदैर्घ्यम्(निवेशः);
    वर्ण *raw = (वर्ण *)निवेशः;

    यदि (utf8_only) {
        यदि (Py_Tok_समुच्चयः_Encoding(tok, "utf-8") < 0) {
            प्रत्यागम -1;
        }
    }
    अन्यथा {
        Py_ssize_t first_original_len = raw_पङ्क्तिः_दीर्घता(raw, raw_len);
        _PyTok_Chunk first = {
            .दत्त = raw,
            .len = first_original_len,
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        _PyTok_Chunk second = {0};
        पूर्णाङ्क have_second = first_original_len < raw_len;
        यदि (have_second) {
            second.दत्त = raw + first_original_len;
            second.len = raw_पङ्क्तिः_दीर्घता(second.दत्त,
                                         raw_len - first_original_len);
        }
        Py_ssize_t bom_len;
        Py_Tok_Encoding_परिणाम detection = _PyTok_DetectEncoding(
            tok, &first, have_second ? &second : रिक्त, 1, &bom_len);
        यदि (detection == _PYTOK_ENCODING_ERROR) {
            प्रत्यागम -1;
        }
        raw += bom_len;
        raw_len -= bom_len;
    }

    _PyTok_Chunk decoded = {
        .दत्त = raw,
        .len = raw_len,
        .ownership = _PYTOK_CHUNK_BORROWED,
    };
    ध्रुव वर्ण *encoding = tok->reader->encoding;
    यदि (encoding != रिक्त && वाक्यं_तुलय(encoding, "utf-8") != 0) {
        यदि (_PyTok_DecodeOnce(
                tok, &decoded, encoding, रिक्त) < 0) {
            प्रत्यागम -1;
        }
    }

    पूर्णाङ्क stored = store_prepared_source(
        tok, decoded.दत्त, decoded.len, preserve_crlf, exec_निवेशः);
    _PyTok_ChunkClear(&decoded);
    यदि (stored < 0) {
        प्रत्यागम -1;
    }
    यदि (!utf8_only &&
            (encoding == रिक्त || वाक्यं_तुलय(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(Py_Tok_Source_दत्त(&tok->source), tok, 1)) {
        प्रत्यागम -1;
    }
    प्रत्यागम 0;
}

पूर्णाङ्क
_PyTok_StartDecoder(संरचना tok_state *tok, ध्रुव वर्ण *errors)
{
    _PyTok_Reader *reader = tok->reader;
    यदि (reader->encoding == रिक्त || reader->decoder != रिक्त) {
        प्रत्यागम 0;
    }
    यदि (reader->विधा == _PYTOK_READER_FILE &&
            वाक्यं_तुलय(reader->encoding, "utf-8") == 0) {
        प्रत्यागम 0;
    }

    PyObject *codec = _PyCodec_LookupTextEncoding(reader->encoding, रिक्त);
    यदि (codec != रिक्त) {
        reader->decoder = _PyCodecInfo_GetIncrementalDecoder(codec, errors);
        Py_DECREF(codec);
    }
    यदि (reader->decoder == रिक्त) {
        tok->समाप्तम् = PyErr_ExceptionMatches(Py_Exc_स्मृतिः_Error)
            ? E_NOMEM : E_DECODE;
        यदि (reader->विधा == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != रिक्त ? tok->filename : Py_None);
        }
        प्रत्यागम -1;
    }
    प्रत्यागम 0;
}

पूर्णाङ्क
_PyTok_DecodeChunk(संरचना tok_state *tok, _PyTok_Chunk *chunk, पूर्णाङ्क final)
{
    _PyTok_Reader *reader = tok->reader;
    यदि (reader->decoder == रिक्त) {
        प्रत्यागम 0;
    }
    पूर्णाङ्क strip_bom = reader->विधा == _PYTOK_READER_READLINE &&
        chunk->len >= UTF16_BOM_दीर्घता &&
        (memcmp(chunk->दत्त, UTF16_LE_BOM, UTF16_BOM_दीर्घता) == 0 ||
         memcmp(chunk->दत्त, UTF16_BE_BOM, UTF16_BOM_दीर्घता) == 0);
    PyObject *निवेशः;
    यदि (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            Py_अष्टकानि_Check(chunk->owner) &&
            chunk->दत्त == Py_अष्टकानि_AS_STRING(chunk->owner)) {
        निवेशः = Py_NewRef(chunk->owner);
    }
    अन्यथा {
        निवेशः = Py_अष्टकानि_From_String_And_आकार(chunk->दत्त, chunk->len);
    }
    यदि (निवेशः == रिक्त) {
        tok->समाप्तम् = E_NOMEM;
        प्रत्यागम -1;
    }
    PyObject *unicode = Py_Object_Call_विधानम्_Obj_तर्काः(
        reader->decoder, &_Py_ID(decode), निवेशः,
        final ? Py_True : Py_False, रिक्त);
    Py_DECREF(निवेशः);
    यदि (unicode == रिक्त) {
        tok->समाप्तम् = PyErr_ExceptionMatches(Py_Exc_स्मृतिः_Error)
            ? E_NOMEM : E_DECODE;
        यदि (reader->विधा == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != रिक्त ? tok->filename : Py_None);
        }
        प्रत्यागम -1;
    }
    यदि (!PyUnicode_Check(unicode)) {
        PyErr_Format(PyExc_TypeError,
                     "decoder should return a string result, not '%.200s'",
                     Py_TYPE(unicode)->tp_name);
        Py_DECREF(unicode);
        tok->समाप्तम् = E_DECODE;
        प्रत्यागम -1;
    }
    प्रत्यागम chunk_समुच्चयः_unicode(tok, chunk, unicode, strip_bom);
}

पूर्णाङ्क
Py_Tok_Decoder_Has_Buffered_निवेशः(संरचना tok_state *tok)
{
    यदि (tok->reader->decoder == रिक्त) {
        प्रत्यागम 0;
    }
    PyObject *state = Py_Object_Call_विधानम्_No_तर्काः(
        tok->reader->decoder, &_Py_ID(getstate));
    यदि (state == रिक्त) {
        tok->समाप्तम् = PyErr_ExceptionMatches(Py_Exc_स्मृतिः_Error)
            ? E_NOMEM : E_DECODE;
        प्रत्यागम -1;
    }
    यदि (!PyTuple_Check(state) || Py_Tuple_GET_आकार(state) != 2 ||
            !Py_अष्टकानि_Check(PyTuple_GET_ITEM(state, 0)) ||
            !PyLong_Check(PyTuple_GET_ITEM(state, 1))) {
        Py_DECREF(state);
        Py_Err_समुच्चयः_String(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->समाप्तम् = E_DECODE;
        प्रत्यागम -1;
    }
    पूर्णाङ्क pending = Py_अष्टकानि_GET_आकार(PyTuple_GET_ITEM(state, 0)) != 0;
    Py_DECREF(state);
    प्रत्यागम pending;
}
