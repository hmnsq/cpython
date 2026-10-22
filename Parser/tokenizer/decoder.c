#అన్తర్భావ "Python.h"
#అన్తర్భావ "pycore_codecs.h"
#అన్తర్భావ "pycore_global_strings.h"
#అన్తర్భావ "pycore_runtime.h"
#అన్తర్భావ "errcode.h"

#అన్తర్భావ "reader_internal.h"
#అన్తర్భావ "helpers.h"
#అన్తర్భావ "../lexer/state.h"

#నిర్వచించు UNICODE_BOM 0xFEFF
#నిర్వచించు UTF16_LE_BOM "\xff\xfe"
#నిర్వచించు UTF16_BE_BOM "\xfe\xff"
#నిర్వచించు UTF16_BOM_నిడివి ((Py_ssize_t)(పరిమాణం(UTF16_LE_BOM) - 1))
#నిర్వచించు UTF8_BOM "\xef\xbb\xbf"
#నిర్వచించు UTF8_BOM_నిడివి ((Py_ssize_t)(పరిమాణం(UTF8_BOM) - 1))

అక్షరం *
Py_Tok_Copy_బైట్లు(స్థిరరాశి అక్షరం *దత్తాంశం, Py_ssize_t len)
{
    ఒకవేళ (len < 0 || len == PY_SSIZE_T_MAX) {
        Py_Err_No_జ్ఞాపకం();
        తిరిగి ఖాళీ;
    }
    అక్షరం *copy = PyMem_Malloc((కొలత_t)len + 1);
    ఒకవేళ (copy == ఖాళీ) {
        Py_Err_No_జ్ఞాపకం();
        తిరిగి ఖాళీ;
    }
    జ్ఞాపకం_నకలు(copy, దత్తాంశం, (కొలత_t)len);
    copy[len] = '\0';
    తిరిగి copy;
}

స్థిర శూన్య
chunk_release_దత్తాంశం(_PyTok_Chunk *chunk)
{
    ఎంపిక (chunk->ownership) {
        స్థితి _PYTOK_CHUNK_BORROWED:
            ఆపు;
        స్థితి _PYTOK_CHUNK_PYMEM:
            PyMem_Free(chunk->దత్తాంశం);
            ఆపు;
        స్థితి _PYTOK_CHUNK_PYOBJECT:
            Py_DECREF(chunk->owner);
            ఆపు;
    }
}

శూన్య
_PyTok_ChunkClear(_PyTok_Chunk *chunk)
{
    chunk_release_దత్తాంశం(chunk);
    *chunk = (_PyTok_Chunk){0};
}

స్థిర పూర్ణాంకం
chunk_సమితి_unicode(నిర్మాణం tok_state *tok, _PyTok_Chunk *chunk,
                  PyObject *unicode, పూర్ణాంకం strip_bom)
{
    Py_ssize_t utf8_len;
    స్థిరరాశి అక్షరం *utf8 = Py_Unicode_As_UTF8_And_కొలత(unicode, &utf8_len);
    ఒకవేళ (utf8 == ఖాళీ) {
        Py_DECREF(unicode);
        tok->పూర్తి = PyErr_ExceptionMatches(Py_Exc_జ్ఞాపకం_Error)
            ? E_NOMEM : E_DECODE;
        తిరిగి -1;
    }
    ఒకవేళ (strip_bom && Py_Unicode_GET_నిడివి(unicode) > 0 &&
            PyUnicode_ReadChar(unicode, 0) == UNICODE_BOM) {
        utf8 += UTF8_BOM_నిడివి;
        utf8_len -= UTF8_BOM_నిడివి;
    }
    chunk_release_దత్తాంశం(chunk);
    chunk->owner = unicode;
    chunk->దత్తాంశం = (అక్షరం *)utf8;
    chunk->len = utf8_len;
    chunk->ownership = _PYTOK_CHUNK_PYOBJECT;
    తిరిగి 0;
}

అక్షరం *
_PyTok_NormalizeNewlines(స్థిరరాశి అక్షరం *దత్తాంశం, Py_ssize_t len,
                         Py_ssize_t *బహిర్గతం_len)
{
    ఒకవేళ (len < 0 || len == PY_SSIZE_T_MAX) {
        Py_Err_No_జ్ఞాపకం();
        తిరిగి ఖాళీ;
    }
    అక్షరం *ఫలితం = PyMem_Malloc((కొలత_t)len + 1);
    ఒకవేళ (ఫలితం == ఖాళీ) {
        Py_Err_No_జ్ఞాపకం();
        తిరిగి ఖాళీ;
    }
    Py_ssize_t లిఖ = 0;
    కొరకు (Py_ssize_t పఠ = 0; పఠ < len; పఠ++) {
        అక్షరం c = దత్తాంశం[పఠ];
        ఒకవేళ (c == '\r') {
            ఒకవేళ (పఠ + 1 < len && దత్తాంశం[పఠ + 1] == '\n') {
                పఠ++;
            }
            c = '\n';
        }
        ఫలితం[లిఖ++] = c;
    }
    ఫలితం[లిఖ] = '\0';
    *బహిర్గతం_len = లిఖ;
    తిరిగి ఫలితం;
}

పూర్ణాంకం
Py_Tok_సమితి_Encoding(నిర్మాణం tok_state *tok, స్థిరరాశి అక్షరం *encoding)
{
    అక్షరం *copy = Py_Tok_Copy_బైట్లు(encoding, అక్షరశ్రేణి_పొడవు(encoding));
    ఒకవేళ (copy == ఖాళీ) {
        tok->పూర్తి = E_NOMEM;
        తిరిగి -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    తిరిగి 0;
}

స్థిర పూర్ణాంకం
కనుగొను_cookie(స్థిరరాశి అక్షరం *పంక్తి, Py_ssize_t len, అక్షరం **encoding, పూర్ణాంకం *పరిశీలన_తదుపరి)
{
    Py_ssize_t i = 0;
    *encoding = ఖాళీ;
    *పరిశీలన_తదుపరి = 1;
    కొరకు (; i < len; i++) {
        ఒకవేళ (పంక్తి[i] == '#') {
            ఆపు;
        }
        ఒకవేళ (పంక్తి[i] == '\n' || పంక్తి[i] == '\r') {
            తిరిగి 0;
        }
        ఒకవేళ (పంక్తి[i] != ' ' && పంక్తి[i] != '\t' && పంక్తి[i] != '\f') {
            *పరిశీలన_తదుపరి = 0;
            తిరిగి 0;
        }
    }
    స్థిరరాశి Py_ssize_t marker_len = పరిమాణం("coding") - 1;
    కొరకు (; i < len - marker_len; i++) {
        ఒకవేళ (memcmp(పంక్తి + i, "coding", marker_len) != 0) {
            కొనసాగు;
        }
        స్థిరరాశి అక్షరం *cursor = పంక్తి + i + marker_len;
        ఒకవేళ (*cursor != ':' && *cursor != '=') {
            కొనసాగు;
        }
        చేయి {
            cursor++;
        } అయితే (cursor < పంక్తి + len &&
                 (*cursor == ' ' || *cursor == '\t'));
        స్థిరరాశి అక్షరం *ప్రారంభం = cursor;
        అయితే (cursor < పంక్తి + len &&
                (Py_ISALNUM(*cursor) || *cursor == '-' ||
                 *cursor == '_' || *cursor == '.')) {
            cursor++;
        }
        ఒకవేళ (cursor == ప్రారంభం) {
            కొనసాగు;
        }
        Py_ssize_t encoding_len = cursor - ప్రారంభం;
        // Include the longest recognized prefix and its NUL terminator.
        అక్షరం normalized[పరిమాణం("iso-latin-1-")];
        స్థిరరాశి Py_ssize_t ఉపసర్గ_len = Py_MIN(
            encoding_len, (Py_ssize_t)పరిమాణం(normalized) - 1);
        Py_ssize_t n;
        కొరకు (n = 0; n < ఉపసర్గ_len; n++) {
            normalized[n] = ప్రారంభం[n] == '_' ? '-' : Py_TOLOWER(ప్రారంభం[n]);
        }
        normalized[n] = '\0';
        స్థిరరాశి అక్షరం *canonical = ప్రారంభం;
        ఒకవేళ (strcmp(normalized, "utf-8") == 0 ||
                అక్షరశ్రేణి_పోలిక(normalized, "utf-8-", పరిమాణం("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        లేకపోతే ఒకవేళ (strcmp(normalized, "latin-1") == 0 ||
                 strcmp(normalized, "iso-8859-1") == 0 ||
                 strcmp(normalized, "iso-latin-1") == 0 ||
                 అక్షరశ్రేణి_పోలిక(normalized, "latin-1-", పరిమాణం("latin-1-") - 1) == 0 ||
                 అక్షరశ్రేణి_పోలిక(normalized, "iso-8859-1-", పరిమాణం("iso-8859-1-") - 1) == 0 ||
                 అక్షరశ్రేణి_పోలిక(normalized, "iso-latin-1-", పరిమాణం("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        ఒకవేళ (canonical != ప్రారంభం) {
            encoding_len = అక్షరశ్రేణి_పొడవు(canonical);
        }
        *encoding = Py_Tok_Copy_బైట్లు(canonical, encoding_len);
        ఒకవేళ (*encoding == ఖాళీ) {
            తిరిగి -1;
        }
        *పరిశీలన_తదుపరి = 0;
        తిరిగి 0;
    }
    తిరిగి 0;
}

Py_Tok_Encoding_ఫలితం
_PyTok_DetectEncoding(నిర్మాణం tok_state *tok, స్థిరరాశి _PyTok_Chunk *మొదటి,
                      స్థిరరాశి _PyTok_Chunk *రెండవ, పూర్ణాంకం final,
                      Py_ssize_t *bom_len)
{
    పూర్ణాంకం bom = మొదటి->len >= UTF8_BOM_నిడివి &&
        memcmp(మొదటి->దత్తాంశం, UTF8_BOM, UTF8_BOM_నిడివి) == 0;
    *bom_len = bom ? UTF8_BOM_నిడివి : 0;

    అక్షరం *cookie = ఖాళీ;
    పూర్ణాంకం పరిశీలన_తదుపరి = 0;
    పూర్ణాంకం cookie_పంక్తి = 1;
    స్థిరరాశి అక్షరం *మొదటి_దత్తాంశం = మొదటి->దత్తాంశం + *bom_len;
    Py_ssize_t మొదటి_len = మొదటి->len - *bom_len;
    ఒకవేళ (కనుగొను_cookie(మొదటి_దత్తాంశం, మొదటి_len, &cookie, &పరిశీలన_తదుపరి) < 0) {
        తిరిగి _PYTOK_ENCODING_ERROR;
    }
    ఒకవేళ (cookie == ఖాళీ && పరిశీలన_తదుపరి && రెండవ != ఖాళీ) {
        ఒకవేళ (కనుగొను_cookie(రెండవ->దత్తాంశం, రెండవ->len, &cookie, &పరిశీలన_తదుపరి) < 0) {
            తిరిగి _PYTOK_ENCODING_ERROR;
        }
        cookie_పంక్తి = 2;
    }
    లేకపోతే ఒకవేళ (cookie == ఖాళీ && పరిశీలన_తదుపరి && !final) {
        తిరిగి PYTOK_ENCODING_NEED_రెండవ_పంక్తి;
    }

    ఒకవేళ (cookie == ఖాళీ) {
        ఒకవేళ (bom && Py_Tok_సమితి_Encoding(tok, "utf-8") < 0) {
            తిరిగి _PYTOK_ENCODING_ERROR;
        }
        తిరిగి PYTOK_ENCODING_పూర్తి;
    }
    ఒకవేళ (bom && strcmp(cookie, "utf-8") != 0) {
        స్థిరరాశి _PyTok_Chunk *పంక్తి = cookie_పంక్తి == 2 ? రెండవ : మొదటి;
        స్థిరరాశి అక్షరం *పంక్తి_దత్తాంశం = పంక్తి->దత్తాంశం + (cookie_పంక్తి == 1 ? *bom_len : 0);
        Py_ssize_t పంక్తి_len = పంక్తి->len - (cookie_పంక్తి == 1 ? *bom_len : 0);
        పూర్ణాంకం చివర_నిలువు = (పూర్ణాంకం)Py_MIN(పంక్తి_len, గరిష్ఠ_పూర్ణాంకం);
        ఒకవేళ (చివర_నిలువు > 0 && (పంక్తి_దత్తాంశం[చివర_నిలువు - 1] == '\n' ||
                            పంక్తి_దత్తాంశం[చివర_నిలువు - 1] == '\r')) {
            చివర_నిలువు--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, పంక్తి_దత్తాంశం, 0, cookie_పంక్తి, 0, చివర_నిలువు,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        తిరిగి _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    తిరిగి PYTOK_ENCODING_పూర్తి;
}

పూర్ణాంకం
_PyTok_DecodeOnce(నిర్మాణం tok_state *tok, _PyTok_Chunk *chunk,
                  స్థిరరాశి అక్షరం *encoding, స్థిరరాశి అక్షరం *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->దత్తాంశం, chunk->len, encoding, errors);
    ఒకవేళ (unicode == ఖాళీ) {
        tok->పూర్తి = PyErr_ExceptionMatches(Py_Exc_జ్ఞాపకం_Error)
            ? E_NOMEM : E_DECODE;
        తిరిగి -1;
    }
    తిరిగి chunk_సమితి_unicode(tok, chunk, unicode, 0);
}

స్థిర Py_ssize_t
raw_పంక్తి_నిడివి(స్థిరరాశి అక్షరం *దత్తాంశం, Py_ssize_t len)
{
    కొరకు (Py_ssize_t i = 0; i < len; i++) {
        ఒకవేళ (దత్తాంశం[i] == '\n') {
            తిరిగి i + 1;
        }
        ఒకవేళ (దత్తాంశం[i] == '\r') {
            తిరిగి i + 1 < len && దత్తాంశం[i + 1] == '\n' ? i + 2 : i + 1;
        }
    }
    తిరిగి len;
}

స్థిర పూర్ణాంకం
store_prepared_ఆధారం(నిర్మాణం tok_state *tok, స్థిరరాశి అక్షరం *దత్తాంశం, Py_ssize_t len,
                      పూర్ణాంకం preserve_crlf, పూర్ణాంకం కూడు_final_newline)
{
    Py_ssize_t pos = 0;
    అయితే (pos < len) {
        Py_ssize_t raw_పంక్తి_len;
        ఒకవేళ (preserve_crlf) {
            స్థిరరాశి అక్షరం *newline = memchr(దత్తాంశం + pos, '\n', len - pos);
            raw_పంక్తి_len = newline == ఖాళీ
                ? len - pos : newline - దత్తాంశం - pos + 1;
        }
        లేకపోతే {
            raw_పంక్తి_len = raw_పంక్తి_నిడివి(దత్తాంశం + pos, len - pos);
        }
        పూర్ణాంకం terminated = preserve_crlf
            ? దత్తాంశం[pos + raw_పంక్తి_len - 1] == '\n'
            : దత్తాంశం[pos + raw_పంక్తి_len - 1] == '\n' ||
              దత్తాంశం[pos + raw_పంక్తి_len - 1] == '\r';
        పూర్ణాంకం కూడు_newline = కూడు_final_newline &&
            pos + raw_పంక్తి_len == len && !terminated;
        స్థిరరాశి అక్షరం *పంక్తి = దత్తాంశం + pos;
        // raw_line_length stops at the first CR or LF, so any CR is in
        // the line terminator.
        Py_ssize_t terminator_len = 0;
        ఒకవేళ (!preserve_crlf) {
            ఒకవేళ (పంక్తి[raw_పంక్తి_len - 1] == '\r') {
                terminator_len = 1;
            }
            లేకపోతే ఒకవేళ (raw_పంక్తి_len > 1 && పంక్తి[raw_పంక్తి_len - 2] == '\r') {
                terminator_len = 2;
            }
        }
        _PyTok_Off appended;
        ఒకవేళ (terminator_len || కూడు_newline) {
            appended = Py_Tok_ఆధారం_Append_పంక్తి_With_Newline(
                &tok->ఆధారం, పంక్తి, raw_పంక్తి_len - terminator_len);
        }
        లేకపోతే {
            appended = Py_Tok_ఆధారం_Append_పంక్తి(&tok->ఆధారం, పంక్తి, raw_పంక్తి_len);
        }
        ఒకవేళ (appended < 0) {
            tok->పూర్తి = PyErr_ExceptionMatches(Py_Exc_జ్ఞాపకం_Error)
                ? E_NOMEM : E_ERROR;
            తిరిగి -1;
        }
        tok->reader->prepared_final_newline = కూడు_newline;
        pos += raw_పంక్తి_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    ఒకవేళ (కూడు_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || దత్తాంశం[len - 1] != '\r'))
    {
        ఒకవేళ (Py_Tok_ఆధారం_Append_పంక్తి(&tok->ఆధారం, "\n", 1) < 0) {
            తిరిగి -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    తిరిగి 0;
}

పూర్ణాంకం
_PyTok_PrepareString(నిర్మాణం tok_state *tok, స్థిరరాశి అక్షరం *ఆదానం, పూర్ణాంకం utf8_only,
                     పూర్ణాంకం exec_ఆదానం, పూర్ణాంకం preserve_crlf)
{
    Py_ssize_t raw_len = అక్షరశ్రేణి_పొడవు(ఆదానం);
    అక్షరం *raw = (అక్షరం *)ఆదానం;

    ఒకవేళ (utf8_only) {
        ఒకవేళ (Py_Tok_సమితి_Encoding(tok, "utf-8") < 0) {
            తిరిగి -1;
        }
    }
    లేకపోతే {
        Py_ssize_t మొదటి_original_len = raw_పంక్తి_నిడివి(raw, raw_len);
        _PyTok_Chunk మొదటి = {
            .దత్తాంశం = raw,
            .len = మొదటి_original_len,
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        _PyTok_Chunk రెండవ = {0};
        పూర్ణాంకం have_రెండవ = మొదటి_original_len < raw_len;
        ఒకవేళ (have_రెండవ) {
            రెండవ.దత్తాంశం = raw + మొదటి_original_len;
            రెండవ.len = raw_పంక్తి_నిడివి(రెండవ.దత్తాంశం,
                                         raw_len - మొదటి_original_len);
        }
        Py_ssize_t bom_len;
        Py_Tok_Encoding_ఫలితం detection = _PyTok_DetectEncoding(
            tok, &మొదటి, have_రెండవ ? &రెండవ : ఖాళీ, 1, &bom_len);
        ఒకవేళ (detection == _PYTOK_ENCODING_ERROR) {
            తిరిగి -1;
        }
        raw += bom_len;
        raw_len -= bom_len;
    }

    _PyTok_Chunk decoded = {
        .దత్తాంశం = raw,
        .len = raw_len,
        .ownership = _PYTOK_CHUNK_BORROWED,
    };
    స్థిరరాశి అక్షరం *encoding = tok->reader->encoding;
    ఒకవేళ (encoding != ఖాళీ && strcmp(encoding, "utf-8") != 0) {
        ఒకవేళ (_PyTok_DecodeOnce(
                tok, &decoded, encoding, ఖాళీ) < 0) {
            తిరిగి -1;
        }
    }

    పూర్ణాంకం stored = store_prepared_ఆధారం(
        tok, decoded.దత్తాంశం, decoded.len, preserve_crlf, exec_ఆదానం);
    _PyTok_ChunkClear(&decoded);
    ఒకవేళ (stored < 0) {
        తిరిగి -1;
    }
    ఒకవేళ (!utf8_only &&
            (encoding == ఖాళీ || strcmp(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(Py_Tok_ఆధారం_దత్తాంశం(&tok->ఆధారం), tok, 1)) {
        తిరిగి -1;
    }
    తిరిగి 0;
}

పూర్ణాంకం
Py_Tok_ప్రారంభం_Decoder(నిర్మాణం tok_state *tok, స్థిరరాశి అక్షరం *errors)
{
    _PyTok_Reader *reader = tok->reader;
    ఒకవేళ (reader->encoding == ఖాళీ || reader->decoder != ఖాళీ) {
        తిరిగి 0;
    }
    ఒకవేళ (reader->రకం == _PYTOK_READER_FILE &&
            strcmp(reader->encoding, "utf-8") == 0) {
        తిరిగి 0;
    }

    PyObject *codec = Py_Codec_Lookup_పాఠ్యం_Encoding(reader->encoding, ఖాళీ);
    ఒకవేళ (codec != ఖాళీ) {
        reader->decoder = _PyCodecInfo_GetIncrementalDecoder(codec, errors);
        Py_DECREF(codec);
    }
    ఒకవేళ (reader->decoder == ఖాళీ) {
        tok->పూర్తి = PyErr_ExceptionMatches(Py_Exc_జ్ఞాపకం_Error)
            ? E_NOMEM : E_DECODE;
        ఒకవేళ (reader->రకం == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != ఖాళీ ? tok->filename : Py_None);
        }
        తిరిగి -1;
    }
    తిరిగి 0;
}

పూర్ణాంకం
_PyTok_DecodeChunk(నిర్మాణం tok_state *tok, _PyTok_Chunk *chunk, పూర్ణాంకం final)
{
    _PyTok_Reader *reader = tok->reader;
    ఒకవేళ (reader->decoder == ఖాళీ) {
        తిరిగి 0;
    }
    పూర్ణాంకం strip_bom = reader->రకం == _PYTOK_READER_READLINE &&
        chunk->len >= UTF16_BOM_నిడివి &&
        (memcmp(chunk->దత్తాంశం, UTF16_LE_BOM, UTF16_BOM_నిడివి) == 0 ||
         memcmp(chunk->దత్తాంశం, UTF16_BE_BOM, UTF16_BOM_నిడివి) == 0);
    PyObject *ఆదానం;
    ఒకవేళ (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            Py_బైట్లు_Check(chunk->owner) &&
            chunk->దత్తాంశం == Py_బైట్లు_AS_STRING(chunk->owner)) {
        ఆదానం = Py_NewRef(chunk->owner);
    }
    లేకపోతే {
        ఆదానం = Py_బైట్లు_From_String_And_కొలత(chunk->దత్తాంశం, chunk->len);
    }
    ఒకవేళ (ఆదానం == ఖాళీ) {
        tok->పూర్తి = E_NOMEM;
        తిరిగి -1;
    }
    PyObject *unicode = Py_Object_Call_పద్ధతి_Obj_వాదనలు(
        reader->decoder, &_Py_ID(decode), ఆదానం,
        final ? Py_True : Py_False, ఖాళీ);
    Py_DECREF(ఆదానం);
    ఒకవేళ (unicode == ఖాళీ) {
        tok->పూర్తి = PyErr_ExceptionMatches(Py_Exc_జ్ఞాపకం_Error)
            ? E_NOMEM : E_DECODE;
        ఒకవేళ (reader->రకం == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != ఖాళీ ? tok->filename : Py_None);
        }
        తిరిగి -1;
    }
    ఒకవేళ (!PyUnicode_Check(unicode)) {
        PyErr_Format(PyExc_TypeError,
                     "decoder should return a string result, not '%.200s'",
                     Py_TYPE(unicode)->tp_name);
        Py_DECREF(unicode);
        tok->పూర్తి = E_DECODE;
        తిరిగి -1;
    }
    తిరిగి chunk_సమితి_unicode(tok, chunk, unicode, strip_bom);
}

పూర్ణాంకం
Py_Tok_Decoder_Has_Buffered_ఆదానం(నిర్మాణం tok_state *tok)
{
    ఒకవేళ (tok->reader->decoder == ఖాళీ) {
        తిరిగి 0;
    }
    PyObject *state = Py_Object_Call_పద్ధతి_No_వాదనలు(
        tok->reader->decoder, &_Py_ID(getstate));
    ఒకవేళ (state == ఖాళీ) {
        tok->పూర్తి = PyErr_ExceptionMatches(Py_Exc_జ్ఞాపకం_Error)
            ? E_NOMEM : E_DECODE;
        తిరిగి -1;
    }
    ఒకవేళ (!PyTuple_Check(state) || Py_Tuple_GET_కొలత(state) != 2 ||
            !Py_బైట్లు_Check(Py_Tuple_GET_అంశం(state, 0)) ||
            !PyLong_Check(Py_Tuple_GET_అంశం(state, 1))) {
        Py_DECREF(state);
        Py_Err_సమితి_String(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->పూర్తి = E_DECODE;
        తిరిగి -1;
    }
    పూర్ణాంకం pending = Py_బైట్లు_GET_కొలత(Py_Tuple_GET_అంశం(state, 0)) != 0;
    Py_DECREF(state);
    తిరిగి pending;
}
