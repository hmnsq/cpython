#ਅਨ੍ਤਰ੍ਭਾਵ "Python.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_codecs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_global_strings.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_runtime.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errcode.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "reader_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "helpers.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "../lexer/state.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNICODE_BOM 0xFEFF
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UTF16_LE_BOM "\xff\xfe"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UTF16_BE_BOM "\xfe\xff"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UTF16_BOM_ਲੰਮਾਈ ((Py_ssize_t)(ਆਕਾਰ(UTF16_LE_BOM) - 1))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UTF8_BOM "\xef\xbb\xbf"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UTF8_BOM_ਲੰਮਾਈ ((Py_ssize_t)(ਆਕਾਰ(UTF8_BOM) - 1))

ਅੱਖਰ *
Py_Tok_Copy_ਬਾਈਟਾਂ(ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ, Py_ssize_t len)
{
    ਜੇ (len < 0 || len == PY_SSIZE_T_MAX) {
        Py_Err_No_ਸਮ੍ਰਿਤੀ();
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਅੱਖਰ *copy = PyMem_Malloc((ਮਾਪ_t)len + 1);
    ਜੇ (copy == ਨਿਰਾ) {
        Py_Err_No_ਸਮ੍ਰਿਤੀ();
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(copy, ਅੰਕੜੇ, (ਮਾਪ_t)len);
    copy[len] = '\0';
    ਵਾਪਸ copy;
}

ਸਥਿਰ ਖਾਲੀ
chunk_release_ਅੰਕੜੇ(_PyTok_Chunk *chunk)
{
    ਚੋਣ (chunk->ownership) {
        ਹਾਲਤ _PYTOK_CHUNK_BORROWED:
            ਰੋਕੋ;
        ਹਾਲਤ _PYTOK_CHUNK_PYMEM:
            PyMem_Free(chunk->ਅੰਕੜੇ);
            ਰੋਕੋ;
        ਹਾਲਤ _PYTOK_CHUNK_PYOBJECT:
            Py_DECREF(chunk->owner);
            ਰੋਕੋ;
    }
}

ਖਾਲੀ
_PyTok_ChunkClear(_PyTok_Chunk *chunk)
{
    chunk_release_ਅੰਕੜੇ(chunk);
    *chunk = (_PyTok_Chunk){0};
}

ਸਥਿਰ ਪੂਰਨ
chunk_ਸੈੱਟ_unicode(ਬਣਤਰ tok_state *tok, _PyTok_Chunk *chunk,
                  PyObject *unicode, ਪੂਰਨ strip_bom)
{
    Py_ssize_t utf8_len;
    ਅਚਲ ਅੱਖਰ *utf8 = Py_Unicode_As_UTF8_And_ਮਾਪ(unicode, &utf8_len);
    ਜੇ (utf8 == ਨਿਰਾ) {
        Py_DECREF(unicode);
        tok->ਪੂਰਾ = PyErr_ExceptionMatches(Py_Exc_ਸਮ੍ਰਿਤੀ_Error)
            ? E_NOMEM : E_DECODE;
        ਵਾਪਸ -1;
    }
    ਜੇ (strip_bom && Py_Unicode_GET_ਲੰਮਾਈ(unicode) > 0 &&
            PyUnicode_ReadChar(unicode, 0) == UNICODE_BOM) {
        utf8 += UTF8_BOM_ਲੰਮਾਈ;
        utf8_len -= UTF8_BOM_ਲੰਮਾਈ;
    }
    chunk_release_ਅੰਕੜੇ(chunk);
    chunk->owner = unicode;
    chunk->ਅੰਕੜੇ = (ਅੱਖਰ *)utf8;
    chunk->len = utf8_len;
    chunk->ownership = _PYTOK_CHUNK_PYOBJECT;
    ਵਾਪਸ 0;
}

ਅੱਖਰ *
_PyTok_NormalizeNewlines(ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ, Py_ssize_t len,
                         Py_ssize_t *ਬਾਹਰਲਾ_len)
{
    ਜੇ (len < 0 || len == PY_SSIZE_T_MAX) {
        Py_Err_No_ਸਮ੍ਰਿਤੀ();
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਅੱਖਰ *ਨਤੀਜਾ = PyMem_Malloc((ਮਾਪ_t)len + 1);
    ਜੇ (ਨਤੀਜਾ == ਨਿਰਾ) {
        Py_Err_No_ਸਮ੍ਰਿਤੀ();
        ਵਾਪਸ ਨਿਰਾ;
    }
    Py_ssize_t ਲਿਖ = 0;
    ਲਈ (Py_ssize_t ਪਠ = 0; ਪਠ < len; ਪਠ++) {
        ਅੱਖਰ c = ਅੰਕੜੇ[ਪਠ];
        ਜੇ (c == '\r') {
            ਜੇ (ਪਠ + 1 < len && ਅੰਕੜੇ[ਪਠ + 1] == '\n') {
                ਪਠ++;
            }
            c = '\n';
        }
        ਨਤੀਜਾ[ਲਿਖ++] = c;
    }
    ਨਤੀਜਾ[ਲਿਖ] = '\0';
    *ਬਾਹਰਲਾ_len = ਲਿਖ;
    ਵਾਪਸ ਨਤੀਜਾ;
}

ਪੂਰਨ
Py_Tok_ਸੈੱਟ_Encoding(ਬਣਤਰ tok_state *tok, ਅਚਲ ਅੱਖਰ *encoding)
{
    ਅੱਖਰ *copy = Py_Tok_Copy_ਬਾਈਟਾਂ(encoding, ਲੜੀ_ਲੰਬਾਈ(encoding));
    ਜੇ (copy == ਨਿਰਾ) {
        tok->ਪੂਰਾ = E_NOMEM;
        ਵਾਪਸ -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ
ਲੱਭੋ_cookie(ਅਚਲ ਅੱਖਰ *ਸਤਰ, Py_ssize_t len, ਅੱਖਰ **encoding, ਪੂਰਨ *ਜਾਂਚ_ਅਗਲਾ)
{
    Py_ssize_t i = 0;
    *encoding = ਨਿਰਾ;
    *ਜਾਂਚ_ਅਗਲਾ = 1;
    ਲਈ (; i < len; i++) {
        ਜੇ (ਸਤਰ[i] == '#') {
            ਰੋਕੋ;
        }
        ਜੇ (ਸਤਰ[i] == '\n' || ਸਤਰ[i] == '\r') {
            ਵਾਪਸ 0;
        }
        ਜੇ (ਸਤਰ[i] != ' ' && ਸਤਰ[i] != '\t' && ਸਤਰ[i] != '\f') {
            *ਜਾਂਚ_ਅਗਲਾ = 0;
            ਵਾਪਸ 0;
        }
    }
    ਅਚਲ Py_ssize_t marker_len = ਆਕਾਰ("coding") - 1;
    ਲਈ (; i < len - marker_len; i++) {
        ਜੇ (memcmp(ਸਤਰ + i, "coding", marker_len) != 0) {
            ਜਾਰੀ;
        }
        ਅਚਲ ਅੱਖਰ *cursor = ਸਤਰ + i + marker_len;
        ਜੇ (*cursor != ':' && *cursor != '=') {
            ਜਾਰੀ;
        }
        ਕਰੋ {
            cursor++;
        } ਜਦੋਂ (cursor < ਸਤਰ + len &&
                 (*cursor == ' ' || *cursor == '\t'));
        ਅਚਲ ਅੱਖਰ *ਸ਼ੁਰੂ = cursor;
        ਜਦੋਂ (cursor < ਸਤਰ + len &&
                (Py_ISALNUM(*cursor) || *cursor == '-' ||
                 *cursor == '_' || *cursor == '.')) {
            cursor++;
        }
        ਜੇ (cursor == ਸ਼ੁਰੂ) {
            ਜਾਰੀ;
        }
        Py_ssize_t encoding_len = cursor - ਸ਼ੁਰੂ;
        // Include the longest recognized prefix and its NUL terminator.
        ਅੱਖਰ normalized[ਆਕਾਰ("iso-latin-1-")];
        ਅਚਲ Py_ssize_t ਅਗੇਤਰ_len = Py_MIN(
            encoding_len, (Py_ssize_t)ਆਕਾਰ(normalized) - 1);
        Py_ssize_t n;
        ਲਈ (n = 0; n < ਅਗੇਤਰ_len; n++) {
            normalized[n] = ਸ਼ੁਰੂ[n] == '_' ? '-' : Py_TOLOWER(ਸ਼ੁਰੂ[n]);
        }
        normalized[n] = '\0';
        ਅਚਲ ਅੱਖਰ *canonical = ਸ਼ੁਰੂ;
        ਜੇ (strcmp(normalized, "utf-8") == 0 ||
                ਲੜੀ_ਤੁਲਨਾ(normalized, "utf-8-", ਆਕਾਰ("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        ਨਹੀਂਤਾਂ ਜੇ (strcmp(normalized, "latin-1") == 0 ||
                 strcmp(normalized, "iso-8859-1") == 0 ||
                 strcmp(normalized, "iso-latin-1") == 0 ||
                 ਲੜੀ_ਤੁਲਨਾ(normalized, "latin-1-", ਆਕਾਰ("latin-1-") - 1) == 0 ||
                 ਲੜੀ_ਤੁਲਨਾ(normalized, "iso-8859-1-", ਆਕਾਰ("iso-8859-1-") - 1) == 0 ||
                 ਲੜੀ_ਤੁਲਨਾ(normalized, "iso-latin-1-", ਆਕਾਰ("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        ਜੇ (canonical != ਸ਼ੁਰੂ) {
            encoding_len = ਲੜੀ_ਲੰਬਾਈ(canonical);
        }
        *encoding = Py_Tok_Copy_ਬਾਈਟਾਂ(canonical, encoding_len);
        ਜੇ (*encoding == ਨਿਰਾ) {
            ਵਾਪਸ -1;
        }
        *ਜਾਂਚ_ਅਗਲਾ = 0;
        ਵਾਪਸ 0;
    }
    ਵਾਪਸ 0;
}

Py_Tok_Encoding_ਨਤੀਜਾ
_PyTok_DetectEncoding(ਬਣਤਰ tok_state *tok, ਅਚਲ _PyTok_Chunk *ਪਹਿਲਾ,
                      ਅਚਲ _PyTok_Chunk *ਦੂਜਾ, ਪੂਰਨ final,
                      Py_ssize_t *bom_len)
{
    ਪੂਰਨ bom = ਪਹਿਲਾ->len >= UTF8_BOM_ਲੰਮਾਈ &&
        memcmp(ਪਹਿਲਾ->ਅੰਕੜੇ, UTF8_BOM, UTF8_BOM_ਲੰਮਾਈ) == 0;
    *bom_len = bom ? UTF8_BOM_ਲੰਮਾਈ : 0;

    ਅੱਖਰ *cookie = ਨਿਰਾ;
    ਪੂਰਨ ਜਾਂਚ_ਅਗਲਾ = 0;
    ਪੂਰਨ cookie_ਸਤਰ = 1;
    ਅਚਲ ਅੱਖਰ *ਪਹਿਲਾ_ਅੰਕੜੇ = ਪਹਿਲਾ->ਅੰਕੜੇ + *bom_len;
    Py_ssize_t ਪਹਿਲਾ_len = ਪਹਿਲਾ->len - *bom_len;
    ਜੇ (ਲੱਭੋ_cookie(ਪਹਿਲਾ_ਅੰਕੜੇ, ਪਹਿਲਾ_len, &cookie, &ਜਾਂਚ_ਅਗਲਾ) < 0) {
        ਵਾਪਸ _PYTOK_ENCODING_ERROR;
    }
    ਜੇ (cookie == ਨਿਰਾ && ਜਾਂਚ_ਅਗਲਾ && ਦੂਜਾ != ਨਿਰਾ) {
        ਜੇ (ਲੱਭੋ_cookie(ਦੂਜਾ->ਅੰਕੜੇ, ਦੂਜਾ->len, &cookie, &ਜਾਂਚ_ਅਗਲਾ) < 0) {
            ਵਾਪਸ _PYTOK_ENCODING_ERROR;
        }
        cookie_ਸਤਰ = 2;
    }
    ਨਹੀਂਤਾਂ ਜੇ (cookie == ਨਿਰਾ && ਜਾਂਚ_ਅਗਲਾ && !final) {
        ਵਾਪਸ PYTOK_ENCODING_NEED_ਦੂਜਾ_ਸਤਰ;
    }

    ਜੇ (cookie == ਨਿਰਾ) {
        ਜੇ (bom && Py_Tok_ਸੈੱਟ_Encoding(tok, "utf-8") < 0) {
            ਵਾਪਸ _PYTOK_ENCODING_ERROR;
        }
        ਵਾਪਸ PYTOK_ENCODING_ਪੂਰਾ;
    }
    ਜੇ (bom && strcmp(cookie, "utf-8") != 0) {
        ਅਚਲ _PyTok_Chunk *ਸਤਰ = cookie_ਸਤਰ == 2 ? ਦੂਜਾ : ਪਹਿਲਾ;
        ਅਚਲ ਅੱਖਰ *ਸਤਰ_ਅੰਕੜੇ = ਸਤਰ->ਅੰਕੜੇ + (cookie_ਸਤਰ == 1 ? *bom_len : 0);
        Py_ssize_t ਸਤਰ_len = ਸਤਰ->len - (cookie_ਸਤਰ == 1 ? *bom_len : 0);
        ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ = (ਪੂਰਨ)Py_MIN(ਸਤਰ_len, ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ);
        ਜੇ (ਸਿਰਾ_ਖਾਨਾ > 0 && (ਸਤਰ_ਅੰਕੜੇ[ਸਿਰਾ_ਖਾਨਾ - 1] == '\n' ||
                            ਸਤਰ_ਅੰਕੜੇ[ਸਿਰਾ_ਖਾਨਾ - 1] == '\r')) {
            ਸਿਰਾ_ਖਾਨਾ--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, ਸਤਰ_ਅੰਕੜੇ, 0, cookie_ਸਤਰ, 0, ਸਿਰਾ_ਖਾਨਾ,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        ਵਾਪਸ _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    ਵਾਪਸ PYTOK_ENCODING_ਪੂਰਾ;
}

ਪੂਰਨ
_PyTok_DecodeOnce(ਬਣਤਰ tok_state *tok, _PyTok_Chunk *chunk,
                  ਅਚਲ ਅੱਖਰ *encoding, ਅਚਲ ਅੱਖਰ *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->ਅੰਕੜੇ, chunk->len, encoding, errors);
    ਜੇ (unicode == ਨਿਰਾ) {
        tok->ਪੂਰਾ = PyErr_ExceptionMatches(Py_Exc_ਸਮ੍ਰਿਤੀ_Error)
            ? E_NOMEM : E_DECODE;
        ਵਾਪਸ -1;
    }
    ਵਾਪਸ chunk_ਸੈੱਟ_unicode(tok, chunk, unicode, 0);
}

ਸਥਿਰ Py_ssize_t
raw_ਸਤਰ_ਲੰਮਾਈ(ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ, Py_ssize_t len)
{
    ਲਈ (Py_ssize_t i = 0; i < len; i++) {
        ਜੇ (ਅੰਕੜੇ[i] == '\n') {
            ਵਾਪਸ i + 1;
        }
        ਜੇ (ਅੰਕੜੇ[i] == '\r') {
            ਵਾਪਸ i + 1 < len && ਅੰਕੜੇ[i + 1] == '\n' ? i + 2 : i + 1;
        }
    }
    ਵਾਪਸ len;
}

ਸਥਿਰ ਪੂਰਨ
store_prepared_ਸਰੋਤ(ਬਣਤਰ tok_state *tok, ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ, Py_ssize_t len,
                      ਪੂਰਨ preserve_crlf, ਪੂਰਨ ਜੋੜੋ_final_newline)
{
    Py_ssize_t pos = 0;
    ਜਦੋਂ (pos < len) {
        Py_ssize_t raw_ਸਤਰ_len;
        ਜੇ (preserve_crlf) {
            ਅਚਲ ਅੱਖਰ *newline = memchr(ਅੰਕੜੇ + pos, '\n', len - pos);
            raw_ਸਤਰ_len = newline == ਨਿਰਾ
                ? len - pos : newline - ਅੰਕੜੇ - pos + 1;
        }
        ਨਹੀਂਤਾਂ {
            raw_ਸਤਰ_len = raw_ਸਤਰ_ਲੰਮਾਈ(ਅੰਕੜੇ + pos, len - pos);
        }
        ਪੂਰਨ terminated = preserve_crlf
            ? ਅੰਕੜੇ[pos + raw_ਸਤਰ_len - 1] == '\n'
            : ਅੰਕੜੇ[pos + raw_ਸਤਰ_len - 1] == '\n' ||
              ਅੰਕੜੇ[pos + raw_ਸਤਰ_len - 1] == '\r';
        ਪੂਰਨ ਜੋੜੋ_newline = ਜੋੜੋ_final_newline &&
            pos + raw_ਸਤਰ_len == len && !terminated;
        ਅਚਲ ਅੱਖਰ *ਸਤਰ = ਅੰਕੜੇ + pos;
        // raw_line_length stops at the first CR or LF, so any CR is in
        // the line terminator.
        Py_ssize_t terminator_len = 0;
        ਜੇ (!preserve_crlf) {
            ਜੇ (ਸਤਰ[raw_ਸਤਰ_len - 1] == '\r') {
                terminator_len = 1;
            }
            ਨਹੀਂਤਾਂ ਜੇ (raw_ਸਤਰ_len > 1 && ਸਤਰ[raw_ਸਤਰ_len - 2] == '\r') {
                terminator_len = 2;
            }
        }
        _PyTok_Off appended;
        ਜੇ (terminator_len || ਜੋੜੋ_newline) {
            appended = Py_Tok_ਸਰੋਤ_Append_ਸਤਰ_With_Newline(
                &tok->ਸਰੋਤ, ਸਤਰ, raw_ਸਤਰ_len - terminator_len);
        }
        ਨਹੀਂਤਾਂ {
            appended = Py_Tok_ਸਰੋਤ_Append_ਸਤਰ(&tok->ਸਰੋਤ, ਸਤਰ, raw_ਸਤਰ_len);
        }
        ਜੇ (appended < 0) {
            tok->ਪੂਰਾ = PyErr_ExceptionMatches(Py_Exc_ਸਮ੍ਰਿਤੀ_Error)
                ? E_NOMEM : E_ERROR;
            ਵਾਪਸ -1;
        }
        tok->reader->prepared_final_newline = ਜੋੜੋ_newline;
        pos += raw_ਸਤਰ_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    ਜੇ (ਜੋੜੋ_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || ਅੰਕੜੇ[len - 1] != '\r'))
    {
        ਜੇ (Py_Tok_ਸਰੋਤ_Append_ਸਤਰ(&tok->ਸਰੋਤ, "\n", 1) < 0) {
            ਵਾਪਸ -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    ਵਾਪਸ 0;
}

ਪੂਰਨ
_PyTok_PrepareString(ਬਣਤਰ tok_state *tok, ਅਚਲ ਅੱਖਰ *ਨਿਵੇਸ਼, ਪੂਰਨ utf8_only,
                     ਪੂਰਨ exec_ਨਿਵੇਸ਼, ਪੂਰਨ preserve_crlf)
{
    Py_ssize_t raw_len = ਲੜੀ_ਲੰਬਾਈ(ਨਿਵੇਸ਼);
    ਅੱਖਰ *raw = (ਅੱਖਰ *)ਨਿਵੇਸ਼;

    ਜੇ (utf8_only) {
        ਜੇ (Py_Tok_ਸੈੱਟ_Encoding(tok, "utf-8") < 0) {
            ਵਾਪਸ -1;
        }
    }
    ਨਹੀਂਤਾਂ {
        Py_ssize_t ਪਹਿਲਾ_original_len = raw_ਸਤਰ_ਲੰਮਾਈ(raw, raw_len);
        _PyTok_Chunk ਪਹਿਲਾ = {
            .ਅੰਕੜੇ = raw,
            .len = ਪਹਿਲਾ_original_len,
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        _PyTok_Chunk ਦੂਜਾ = {0};
        ਪੂਰਨ have_ਦੂਜਾ = ਪਹਿਲਾ_original_len < raw_len;
        ਜੇ (have_ਦੂਜਾ) {
            ਦੂਜਾ.ਅੰਕੜੇ = raw + ਪਹਿਲਾ_original_len;
            ਦੂਜਾ.len = raw_ਸਤਰ_ਲੰਮਾਈ(ਦੂਜਾ.ਅੰਕੜੇ,
                                         raw_len - ਪਹਿਲਾ_original_len);
        }
        Py_ssize_t bom_len;
        Py_Tok_Encoding_ਨਤੀਜਾ detection = _PyTok_DetectEncoding(
            tok, &ਪਹਿਲਾ, have_ਦੂਜਾ ? &ਦੂਜਾ : ਨਿਰਾ, 1, &bom_len);
        ਜੇ (detection == _PYTOK_ENCODING_ERROR) {
            ਵਾਪਸ -1;
        }
        raw += bom_len;
        raw_len -= bom_len;
    }

    _PyTok_Chunk decoded = {
        .ਅੰਕੜੇ = raw,
        .len = raw_len,
        .ownership = _PYTOK_CHUNK_BORROWED,
    };
    ਅਚਲ ਅੱਖਰ *encoding = tok->reader->encoding;
    ਜੇ (encoding != ਨਿਰਾ && strcmp(encoding, "utf-8") != 0) {
        ਜੇ (_PyTok_DecodeOnce(
                tok, &decoded, encoding, ਨਿਰਾ) < 0) {
            ਵਾਪਸ -1;
        }
    }

    ਪੂਰਨ stored = store_prepared_ਸਰੋਤ(
        tok, decoded.ਅੰਕੜੇ, decoded.len, preserve_crlf, exec_ਨਿਵੇਸ਼);
    _PyTok_ChunkClear(&decoded);
    ਜੇ (stored < 0) {
        ਵਾਪਸ -1;
    }
    ਜੇ (!utf8_only &&
            (encoding == ਨਿਰਾ || strcmp(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(Py_Tok_ਸਰੋਤ_ਅੰਕੜੇ(&tok->ਸਰੋਤ), tok, 1)) {
        ਵਾਪਸ -1;
    }
    ਵਾਪਸ 0;
}

ਪੂਰਨ
Py_Tok_ਸ਼ੁਰੂ_Decoder(ਬਣਤਰ tok_state *tok, ਅਚਲ ਅੱਖਰ *errors)
{
    _PyTok_Reader *reader = tok->reader;
    ਜੇ (reader->encoding == ਨਿਰਾ || reader->decoder != ਨਿਰਾ) {
        ਵਾਪਸ 0;
    }
    ਜੇ (reader->ਕਿਸਮ == _PYTOK_READER_FILE &&
            strcmp(reader->encoding, "utf-8") == 0) {
        ਵਾਪਸ 0;
    }

    PyObject *codec = Py_Codec_Lookup_ਪਾਠ_Encoding(reader->encoding, ਨਿਰਾ);
    ਜੇ (codec != ਨਿਰਾ) {
        reader->decoder = _PyCodecInfo_GetIncrementalDecoder(codec, errors);
        Py_DECREF(codec);
    }
    ਜੇ (reader->decoder == ਨਿਰਾ) {
        tok->ਪੂਰਾ = PyErr_ExceptionMatches(Py_Exc_ਸਮ੍ਰਿਤੀ_Error)
            ? E_NOMEM : E_DECODE;
        ਜੇ (reader->ਕਿਸਮ == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != ਨਿਰਾ ? tok->filename : Py_None);
        }
        ਵਾਪਸ -1;
    }
    ਵਾਪਸ 0;
}

ਪੂਰਨ
_PyTok_DecodeChunk(ਬਣਤਰ tok_state *tok, _PyTok_Chunk *chunk, ਪੂਰਨ final)
{
    _PyTok_Reader *reader = tok->reader;
    ਜੇ (reader->decoder == ਨਿਰਾ) {
        ਵਾਪਸ 0;
    }
    ਪੂਰਨ strip_bom = reader->ਕਿਸਮ == _PYTOK_READER_READLINE &&
        chunk->len >= UTF16_BOM_ਲੰਮਾਈ &&
        (memcmp(chunk->ਅੰਕੜੇ, UTF16_LE_BOM, UTF16_BOM_ਲੰਮਾਈ) == 0 ||
         memcmp(chunk->ਅੰਕੜੇ, UTF16_BE_BOM, UTF16_BOM_ਲੰਮਾਈ) == 0);
    PyObject *ਨਿਵੇਸ਼;
    ਜੇ (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            Py_ਬਾਈਟਾਂ_Check(chunk->owner) &&
            chunk->ਅੰਕੜੇ == Py_ਬਾਈਟਾਂ_AS_STRING(chunk->owner)) {
        ਨਿਵੇਸ਼ = Py_NewRef(chunk->owner);
    }
    ਨਹੀਂਤਾਂ {
        ਨਿਵੇਸ਼ = Py_ਬਾਈਟਾਂ_From_String_And_ਮਾਪ(chunk->ਅੰਕੜੇ, chunk->len);
    }
    ਜੇ (ਨਿਵੇਸ਼ == ਨਿਰਾ) {
        tok->ਪੂਰਾ = E_NOMEM;
        ਵਾਪਸ -1;
    }
    PyObject *unicode = Py_Object_Call_ਵਿਧੀ_Obj_ਦਲੀਲਾਂ(
        reader->decoder, &_Py_ID(decode), ਨਿਵੇਸ਼,
        final ? Py_True : Py_False, ਨਿਰਾ);
    Py_DECREF(ਨਿਵੇਸ਼);
    ਜੇ (unicode == ਨਿਰਾ) {
        tok->ਪੂਰਾ = PyErr_ExceptionMatches(Py_Exc_ਸਮ੍ਰਿਤੀ_Error)
            ? E_NOMEM : E_DECODE;
        ਜੇ (reader->ਕਿਸਮ == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != ਨਿਰਾ ? tok->filename : Py_None);
        }
        ਵਾਪਸ -1;
    }
    ਜੇ (!PyUnicode_Check(unicode)) {
        PyErr_Format(PyExc_TypeError,
                     "decoder should return a string result, not '%.200s'",
                     Py_TYPE(unicode)->tp_name);
        Py_DECREF(unicode);
        tok->ਪੂਰਾ = E_DECODE;
        ਵਾਪਸ -1;
    }
    ਵਾਪਸ chunk_ਸੈੱਟ_unicode(tok, chunk, unicode, strip_bom);
}

ਪੂਰਨ
Py_Tok_Decoder_Has_Buffered_ਨਿਵੇਸ਼(ਬਣਤਰ tok_state *tok)
{
    ਜੇ (tok->reader->decoder == ਨਿਰਾ) {
        ਵਾਪਸ 0;
    }
    PyObject *state = Py_Object_Call_ਵਿਧੀ_No_ਦਲੀਲਾਂ(
        tok->reader->decoder, &_Py_ID(getstate));
    ਜੇ (state == ਨਿਰਾ) {
        tok->ਪੂਰਾ = PyErr_ExceptionMatches(Py_Exc_ਸਮ੍ਰਿਤੀ_Error)
            ? E_NOMEM : E_DECODE;
        ਵਾਪਸ -1;
    }
    ਜੇ (!PyTuple_Check(state) || Py_Tuple_GET_ਮਾਪ(state) != 2 ||
            !Py_ਬਾਈਟਾਂ_Check(Py_Tuple_GET_ਵਸਤੂ(state, 0)) ||
            !PyLong_Check(Py_Tuple_GET_ਵਸਤੂ(state, 1))) {
        Py_DECREF(state);
        Py_Err_ਸੈੱਟ_String(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->ਪੂਰਾ = E_DECODE;
        ਵਾਪਸ -1;
    }
    ਪੂਰਨ pending = Py_ਬਾਈਟਾਂ_GET_ਮਾਪ(Py_Tuple_GET_ਵਸਤੂ(state, 0)) != 0;
    Py_DECREF(state);
    ਵਾਪਸ pending;
}
