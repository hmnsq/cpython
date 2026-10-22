#ಅನ್ತರ್ಭಾವ "Python.h"
#ಅನ್ತರ್ಭಾವ "pycore_codecs.h"
#ಅನ್ತರ್ಭಾವ "pycore_global_strings.h"
#ಅನ್ತರ್ಭಾವ "pycore_runtime.h"
#ಅನ್ತರ್ಭಾವ "errcode.h"

#ಅನ್ತರ್ಭಾವ "reader_internal.h"
#ಅನ್ತರ್ಭಾವ "helpers.h"
#ಅನ್ತರ್ಭಾವ "../lexer/state.h"

#ವ್ಯಾಖ್ಯಾನಿಸು UNICODE_BOM 0xFEFF
#ವ್ಯಾಖ್ಯಾನಿಸು UTF16_LE_BOM "\xff\xfe"
#ವ್ಯಾಖ್ಯಾನಿಸು UTF16_BE_BOM "\xfe\xff"
#ವ್ಯಾಖ್ಯಾನಿಸು UTF16_BOM_ಉದ್ದಳತೆ ((Py_ssize_t)(ಗಾತ್ರ(UTF16_LE_BOM) - 1))
#ವ್ಯಾಖ್ಯಾನಿಸು UTF8_BOM "\xef\xbb\xbf"
#ವ್ಯಾಖ್ಯಾನಿಸು UTF8_BOM_ಉದ್ದಳತೆ ((Py_ssize_t)(ಗಾತ್ರ(UTF8_BOM) - 1))

ಅಕ್ಷರ *
Py_Tok_Copy_ಬೈಟುಗಳು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ, Py_ssize_t len)
{
    ಆದರೆ (len < 0 || len == PY_SSIZE_T_MAX) {
        Py_Err_No_ಸ್ಮರಣೆ();
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಅಕ್ಷರ *copy = PyMem_Malloc((ಗಾತ್ರ_t)len + 1);
    ಆದರೆ (copy == ಖಾಲಿ) {
        Py_Err_No_ಸ್ಮರಣೆ();
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಸ್ಮರಣೆ_ನಕಲು(copy, ದತ್ತಾಂಶ, (ಗಾತ್ರ_t)len);
    copy[len] = '\0';
    ಹಿಂತಿರುಗಿ copy;
}

ಸ್ಥಿರ ಶೂನ್ಯ
chunk_release_ದತ್ತಾಂಶ(_PyTok_Chunk *chunk)
{
    ಆಯ್ಕೆ (chunk->ownership) {
        ಸ್ಥಿತಿ _PYTOK_CHUNK_BORROWED:
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ _PYTOK_CHUNK_PYMEM:
            PyMem_Free(chunk->ದತ್ತಾಂಶ);
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ _PYTOK_CHUNK_PYOBJECT:
            Py_DECREF(chunk->owner);
            ನಿಲ್ಲಿಸು;
    }
}

ಶೂನ್ಯ
_PyTok_ChunkClear(_PyTok_Chunk *chunk)
{
    chunk_release_ದತ್ತಾಂಶ(chunk);
    *chunk = (_PyTok_Chunk){0};
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
chunk_ಗಣ_unicode(ರಚನೆ tok_state *tok, _PyTok_Chunk *chunk,
                  PyObject *unicode, ಪೂರ್ಣಾಂಕ strip_bom)
{
    Py_ssize_t utf8_len;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *utf8 = Py_Unicode_As_UTF8_And_ಗಾತ್ರ(unicode, &utf8_len);
    ಆದರೆ (utf8 == ಖಾಲಿ) {
        Py_DECREF(unicode);
        tok->ಮುಗಿದ = PyErr_ExceptionMatches(Py_Exc_ಸ್ಮರಣೆ_Error)
            ? E_NOMEM : E_DECODE;
        ಹಿಂತಿರುಗಿ -1;
    }
    ಆದರೆ (strip_bom && Py_Unicode_GET_ಉದ್ದಳತೆ(unicode) > 0 &&
            PyUnicode_ReadChar(unicode, 0) == UNICODE_BOM) {
        utf8 += UTF8_BOM_ಉದ್ದಳತೆ;
        utf8_len -= UTF8_BOM_ಉದ್ದಳತೆ;
    }
    chunk_release_ದತ್ತಾಂಶ(chunk);
    chunk->owner = unicode;
    chunk->ದತ್ತಾಂಶ = (ಅಕ್ಷರ *)utf8;
    chunk->len = utf8_len;
    chunk->ownership = _PYTOK_CHUNK_PYOBJECT;
    ಹಿಂತಿರುಗಿ 0;
}

ಅಕ್ಷರ *
_PyTok_NormalizeNewlines(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ, Py_ssize_t len,
                         Py_ssize_t *ಹೊರಹೋಗು_len)
{
    ಆದರೆ (len < 0 || len == PY_SSIZE_T_MAX) {
        Py_Err_No_ಸ್ಮರಣೆ();
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಅಕ್ಷರ *ಫಲಿತಾಂಶ = PyMem_Malloc((ಗಾತ್ರ_t)len + 1);
    ಆದರೆ (ಫಲಿತಾಂಶ == ಖಾಲಿ) {
        Py_Err_No_ಸ್ಮರಣೆ();
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    Py_ssize_t ಲಿಖ = 0;
    ಗಾಗಿ (Py_ssize_t ಪಠ = 0; ಪಠ < len; ಪಠ++) {
        ಅಕ್ಷರ c = ದತ್ತಾಂಶ[ಪಠ];
        ಆದರೆ (c == '\r') {
            ಆದರೆ (ಪಠ + 1 < len && ದತ್ತಾಂಶ[ಪಠ + 1] == '\n') {
                ಪಠ++;
            }
            c = '\n';
        }
        ಫಲಿತಾಂಶ[ಲಿಖ++] = c;
    }
    ಫಲಿತಾಂಶ[ಲಿಖ] = '\0';
    *ಹೊರಹೋಗು_len = ಲಿಖ;
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಪೂರ್ಣಾಂಕ
Py_Tok_ಗಣ_Encoding(ರಚನೆ tok_state *tok, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *encoding)
{
    ಅಕ್ಷರ *copy = Py_Tok_Copy_ಬೈಟುಗಳು(encoding, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(encoding));
    ಆದರೆ (copy == ಖಾಲಿ) {
        tok->ಮುಗಿದ = E_NOMEM;
        ಹಿಂತಿರುಗಿ -1;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = copy;
    ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_cookie(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು, Py_ssize_t len, ಅಕ್ಷರ **encoding, ಪೂರ್ಣಾಂಕ *ಪರಿಶೀಲನೆ_ಮುಂದಿನ)
{
    Py_ssize_t i = 0;
    *encoding = ಖಾಲಿ;
    *ಪರಿಶೀಲನೆ_ಮುಂದಿನ = 1;
    ಗಾಗಿ (; i < len; i++) {
        ಆದರೆ (ಸಾಲು[i] == '#') {
            ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ (ಸಾಲು[i] == '\n' || ಸಾಲು[i] == '\r') {
            ಹಿಂತಿರುಗಿ 0;
        }
        ಆದರೆ (ಸಾಲು[i] != ' ' && ಸಾಲು[i] != '\t' && ಸಾಲು[i] != '\f') {
            *ಪರಿಶೀಲನೆ_ಮುಂದಿನ = 0;
            ಹಿಂತಿರುಗಿ 0;
        }
    }
    ಸ್ಥಿರಾಂಕ Py_ssize_t marker_len = ಗಾತ್ರ("coding") - 1;
    ಗಾಗಿ (; i < len - marker_len; i++) {
        ಆದರೆ (memcmp(ಸಾಲು + i, "coding", marker_len) != 0) {
            ಮುಂದುವರಿ;
        }
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cursor = ಸಾಲು + i + marker_len;
        ಆದರೆ (*cursor != ':' && *cursor != '=') {
            ಮುಂದುವರಿ;
        }
        ಮಾಡು {
            cursor++;
        } ಆಗ (cursor < ಸಾಲು + len &&
                 (*cursor == ' ' || *cursor == '\t'));
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಆರಂಭ = cursor;
        ಆಗ (cursor < ಸಾಲು + len &&
                (Py_ISALNUM(*cursor) || *cursor == '-' ||
                 *cursor == '_' || *cursor == '.')) {
            cursor++;
        }
        ಆದರೆ (cursor == ಆರಂಭ) {
            ಮುಂದುವರಿ;
        }
        Py_ssize_t encoding_len = cursor - ಆರಂಭ;
        // Include the longest recognized prefix and its NUL terminator.
        ಅಕ್ಷರ normalized[ಗಾತ್ರ("iso-latin-1-")];
        ಸ್ಥಿರಾಂಕ Py_ssize_t ಪೂರ್ವಪ್ರತ್ಯಯ_len = Py_MIN(
            encoding_len, (Py_ssize_t)ಗಾತ್ರ(normalized) - 1);
        Py_ssize_t n;
        ಗಾಗಿ (n = 0; n < ಪೂರ್ವಪ್ರತ್ಯಯ_len; n++) {
            normalized[n] = ಆರಂಭ[n] == '_' ? '-' : Py_TOLOWER(ಆರಂಭ[n]);
        }
        normalized[n] = '\0';
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *canonical = ಆರಂಭ;
        ಆದರೆ (strcmp(normalized, "utf-8") == 0 ||
                ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(normalized, "utf-8-", ಗಾತ್ರ("utf-8-") - 1) == 0) {
            canonical = "utf-8";
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(normalized, "latin-1") == 0 ||
                 strcmp(normalized, "iso-8859-1") == 0 ||
                 strcmp(normalized, "iso-latin-1") == 0 ||
                 ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(normalized, "latin-1-", ಗಾತ್ರ("latin-1-") - 1) == 0 ||
                 ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(normalized, "iso-8859-1-", ಗಾತ್ರ("iso-8859-1-") - 1) == 0 ||
                 ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(normalized, "iso-latin-1-", ಗಾತ್ರ("iso-latin-1-") - 1) == 0) {
            canonical = "iso-8859-1";
        }
        ಆದರೆ (canonical != ಆರಂಭ) {
            encoding_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(canonical);
        }
        *encoding = Py_Tok_Copy_ಬೈಟುಗಳು(canonical, encoding_len);
        ಆದರೆ (*encoding == ಖಾಲಿ) {
            ಹಿಂತಿರುಗಿ -1;
        }
        *ಪರಿಶೀಲನೆ_ಮುಂದಿನ = 0;
        ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ 0;
}

Py_Tok_Encoding_ಫಲಿತಾಂಶ
_PyTok_DetectEncoding(ರಚನೆ tok_state *tok, ಸ್ಥಿರಾಂಕ _PyTok_Chunk *ಮೊದಲ,
                      ಸ್ಥಿರಾಂಕ _PyTok_Chunk *ಎರಡನೇ, ಪೂರ್ಣಾಂಕ final,
                      Py_ssize_t *bom_len)
{
    ಪೂರ್ಣಾಂಕ bom = ಮೊದಲ->len >= UTF8_BOM_ಉದ್ದಳತೆ &&
        memcmp(ಮೊದಲ->ದತ್ತಾಂಶ, UTF8_BOM, UTF8_BOM_ಉದ್ದಳತೆ) == 0;
    *bom_len = bom ? UTF8_BOM_ಉದ್ದಳತೆ : 0;

    ಅಕ್ಷರ *cookie = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ ಪರಿಶೀಲನೆ_ಮುಂದಿನ = 0;
    ಪೂರ್ಣಾಂಕ cookie_ಸಾಲು = 1;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೊದಲ_ದತ್ತಾಂಶ = ಮೊದಲ->ದತ್ತಾಂಶ + *bom_len;
    Py_ssize_t ಮೊದಲ_len = ಮೊದಲ->len - *bom_len;
    ಆದರೆ (ಕಂಡುಹಿಡಿ_cookie(ಮೊದಲ_ದತ್ತಾಂಶ, ಮೊದಲ_len, &cookie, &ಪರಿಶೀಲನೆ_ಮುಂದಿನ) < 0) {
        ಹಿಂತಿರುಗಿ _PYTOK_ENCODING_ERROR;
    }
    ಆದರೆ (cookie == ಖಾಲಿ && ಪರಿಶೀಲನೆ_ಮುಂದಿನ && ಎರಡನೇ != ಖಾಲಿ) {
        ಆದರೆ (ಕಂಡುಹಿಡಿ_cookie(ಎರಡನೇ->ದತ್ತಾಂಶ, ಎರಡನೇ->len, &cookie, &ಪರಿಶೀಲನೆ_ಮುಂದಿನ) < 0) {
            ಹಿಂತಿರುಗಿ _PYTOK_ENCODING_ERROR;
        }
        cookie_ಸಾಲು = 2;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cookie == ಖಾಲಿ && ಪರಿಶೀಲನೆ_ಮುಂದಿನ && !final) {
        ಹಿಂತಿರುಗಿ PYTOK_ENCODING_NEED_ಎರಡನೇ_ಸಾಲು;
    }

    ಆದರೆ (cookie == ಖಾಲಿ) {
        ಆದರೆ (bom && Py_Tok_ಗಣ_Encoding(tok, "utf-8") < 0) {
            ಹಿಂತಿರುಗಿ _PYTOK_ENCODING_ERROR;
        }
        ಹಿಂತಿರುಗಿ PYTOK_ENCODING_ಮುಗಿದ;
    }
    ಆದರೆ (bom && strcmp(cookie, "utf-8") != 0) {
        ಸ್ಥಿರಾಂಕ _PyTok_Chunk *ಸಾಲು = cookie_ಸಾಲು == 2 ? ಎರಡನೇ : ಮೊದಲ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು_ದತ್ತಾಂಶ = ಸಾಲು->ದತ್ತಾಂಶ + (cookie_ಸಾಲು == 1 ? *bom_len : 0);
        Py_ssize_t ಸಾಲು_len = ಸಾಲು->len - (cookie_ಸಾಲು == 1 ? *bom_len : 0);
        ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)Py_MIN(ಸಾಲು_len, ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ);
        ಆದರೆ (ಕೊನೆ_ಲಂಬಸಾಲು > 0 && (ಸಾಲು_ದತ್ತಾಂಶ[ಕೊನೆ_ಲಂಬಸಾಲು - 1] == '\n' ||
                            ಸಾಲು_ದತ್ತಾಂಶ[ಕೊನೆ_ಲಂಬಸಾಲು - 1] == '\r')) {
            ಕೊನೆ_ಲಂಬಸಾಲು--;
        }
        _PyTokenizer_syntaxerror_at(
            tok, ಸಾಲು_ದತ್ತಾಂಶ, 0, cookie_ಸಾಲು, 0, ಕೊನೆ_ಲಂಬಸಾಲು,
            "encoding problem: %s with BOM", cookie);
        PyMem_Free(cookie);
        ಹಿಂತಿರುಗಿ _PYTOK_ENCODING_ERROR;
    }
    PyMem_Free(tok->reader->encoding);
    tok->reader->encoding = cookie;
    ಹಿಂತಿರುಗಿ PYTOK_ENCODING_ಮುಗಿದ;
}

ಪೂರ್ಣಾಂಕ
_PyTok_DecodeOnce(ರಚನೆ tok_state *tok, _PyTok_Chunk *chunk,
                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *encoding, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *errors)
{
    PyObject *unicode = PyUnicode_Decode(
        chunk->ದತ್ತಾಂಶ, chunk->len, encoding, errors);
    ಆದರೆ (unicode == ಖಾಲಿ) {
        tok->ಮುಗಿದ = PyErr_ExceptionMatches(Py_Exc_ಸ್ಮರಣೆ_Error)
            ? E_NOMEM : E_DECODE;
        ಹಿಂತಿರುಗಿ -1;
    }
    ಹಿಂತಿರುಗಿ chunk_ಗಣ_unicode(tok, chunk, unicode, 0);
}

ಸ್ಥಿರ Py_ssize_t
raw_ಸಾಲು_ಉದ್ದಳತೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ, Py_ssize_t len)
{
    ಗಾಗಿ (Py_ssize_t i = 0; i < len; i++) {
        ಆದರೆ (ದತ್ತಾಂಶ[i] == '\n') {
            ಹಿಂತಿರುಗಿ i + 1;
        }
        ಆದರೆ (ದತ್ತಾಂಶ[i] == '\r') {
            ಹಿಂತಿರುಗಿ i + 1 < len && ದತ್ತಾಂಶ[i + 1] == '\n' ? i + 2 : i + 1;
        }
    }
    ಹಿಂತಿರುಗಿ len;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
store_prepared_ಮೂಲ(ರಚನೆ tok_state *tok, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ, Py_ssize_t len,
                      ಪೂರ್ಣಾಂಕ preserve_crlf, ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_final_newline)
{
    Py_ssize_t pos = 0;
    ಆಗ (pos < len) {
        Py_ssize_t raw_ಸಾಲು_len;
        ಆದರೆ (preserve_crlf) {
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *newline = memchr(ದತ್ತಾಂಶ + pos, '\n', len - pos);
            raw_ಸಾಲು_len = newline == ಖಾಲಿ
                ? len - pos : newline - ದತ್ತಾಂಶ - pos + 1;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            raw_ಸಾಲು_len = raw_ಸಾಲು_ಉದ್ದಳತೆ(ದತ್ತಾಂಶ + pos, len - pos);
        }
        ಪೂರ್ಣಾಂಕ terminated = preserve_crlf
            ? ದತ್ತಾಂಶ[pos + raw_ಸಾಲು_len - 1] == '\n'
            : ದತ್ತಾಂಶ[pos + raw_ಸಾಲು_len - 1] == '\n' ||
              ದತ್ತಾಂಶ[pos + raw_ಸಾಲು_len - 1] == '\r';
        ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_newline = ಕೂಡಿಸು_final_newline &&
            pos + raw_ಸಾಲು_len == len && !terminated;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು = ದತ್ತಾಂಶ + pos;
        // raw_line_length stops at the first CR or LF, so any CR is in
        // the line terminator.
        Py_ssize_t terminator_len = 0;
        ಆದರೆ (!preserve_crlf) {
            ಆದರೆ (ಸಾಲು[raw_ಸಾಲು_len - 1] == '\r') {
                terminator_len = 1;
            }
            ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (raw_ಸಾಲು_len > 1 && ಸಾಲು[raw_ಸಾಲು_len - 2] == '\r') {
                terminator_len = 2;
            }
        }
        _PyTok_Off appended;
        ಆದರೆ (terminator_len || ಕೂಡಿಸು_newline) {
            appended = Py_Tok_ಮೂಲ_Append_ಸಾಲು_With_Newline(
                &tok->ಮೂಲ, ಸಾಲು, raw_ಸಾಲು_len - terminator_len);
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            appended = Py_Tok_ಮೂಲ_Append_ಸಾಲು(&tok->ಮೂಲ, ಸಾಲು, raw_ಸಾಲು_len);
        }
        ಆದರೆ (appended < 0) {
            tok->ಮುಗಿದ = PyErr_ExceptionMatches(Py_Exc_ಸ್ಮರಣೆ_Error)
                ? E_NOMEM : E_ERROR;
            ಹಿಂತಿರುಗಿ -1;
        }
        tok->reader->prepared_final_newline = ಕೂಡಿಸು_newline;
        pos += raw_ಸಾಲು_len;
    }
    // Partial input includes the newline submitted by the next Enter key.
    // A final CR pairs with that newline instead of starting a blank line.
    ಆದರೆ (ಕೂಡಿಸು_final_newline == _PyTokenizer_APPEND_NEWLINE &&
        !tok->reader->prepared_final_newline &&
        (len == 0 || ದತ್ತಾಂಶ[len - 1] != '\r'))
    {
        ಆದರೆ (Py_Tok_ಮೂಲ_Append_ಸಾಲು(&tok->ಮೂಲ, "\n", 1) < 0) {
            ಹಿಂತಿರುಗಿ -1;
        }
        tok->reader->prepared_final_newline = _PyTokenizer_APPEND_NEWLINE;
    }
    ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ
_PyTok_PrepareString(ರಚನೆ tok_state *tok, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳಹರಿವು, ಪೂರ್ಣಾಂಕ utf8_only,
                     ಪೂರ್ಣಾಂಕ exec_ಒಳಹರಿವು, ಪೂರ್ಣಾಂಕ preserve_crlf)
{
    Py_ssize_t raw_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಒಳಹರಿವು);
    ಅಕ್ಷರ *raw = (ಅಕ್ಷರ *)ಒಳಹರಿವು;

    ಆದರೆ (utf8_only) {
        ಆದರೆ (Py_Tok_ಗಣ_Encoding(tok, "utf-8") < 0) {
            ಹಿಂತಿರುಗಿ -1;
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        Py_ssize_t ಮೊದಲ_original_len = raw_ಸಾಲು_ಉದ್ದಳತೆ(raw, raw_len);
        _PyTok_Chunk ಮೊದಲ = {
            .ದತ್ತಾಂಶ = raw,
            .len = ಮೊದಲ_original_len,
            .ownership = _PYTOK_CHUNK_BORROWED,
        };
        _PyTok_Chunk ಎರಡನೇ = {0};
        ಪೂರ್ಣಾಂಕ have_ಎರಡನೇ = ಮೊದಲ_original_len < raw_len;
        ಆದರೆ (have_ಎರಡನೇ) {
            ಎರಡನೇ.ದತ್ತಾಂಶ = raw + ಮೊದಲ_original_len;
            ಎರಡನೇ.len = raw_ಸಾಲು_ಉದ್ದಳತೆ(ಎರಡನೇ.ದತ್ತಾಂಶ,
                                         raw_len - ಮೊದಲ_original_len);
        }
        Py_ssize_t bom_len;
        Py_Tok_Encoding_ಫಲಿತಾಂಶ detection = _PyTok_DetectEncoding(
            tok, &ಮೊದಲ, have_ಎರಡನೇ ? &ಎರಡನೇ : ಖಾಲಿ, 1, &bom_len);
        ಆದರೆ (detection == _PYTOK_ENCODING_ERROR) {
            ಹಿಂತಿರುಗಿ -1;
        }
        raw += bom_len;
        raw_len -= bom_len;
    }

    _PyTok_Chunk decoded = {
        .ದತ್ತಾಂಶ = raw,
        .len = raw_len,
        .ownership = _PYTOK_CHUNK_BORROWED,
    };
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *encoding = tok->reader->encoding;
    ಆದರೆ (encoding != ಖಾಲಿ && strcmp(encoding, "utf-8") != 0) {
        ಆದರೆ (_PyTok_DecodeOnce(
                tok, &decoded, encoding, ಖಾಲಿ) < 0) {
            ಹಿಂತಿರುಗಿ -1;
        }
    }

    ಪೂರ್ಣಾಂಕ stored = store_prepared_ಮೂಲ(
        tok, decoded.ದತ್ತಾಂಶ, decoded.len, preserve_crlf, exec_ಒಳಹರಿವು);
    _PyTok_ChunkClear(&decoded);
    ಆದರೆ (stored < 0) {
        ಹಿಂತಿರುಗಿ -1;
    }
    ಆದರೆ (!utf8_only &&
            (encoding == ಖಾಲಿ || strcmp(encoding, "utf-8") == 0) &&
            !_PyTokenizer_ensure_utf8(Py_Tok_ಮೂಲ_ದತ್ತಾಂಶ(&tok->ಮೂಲ), tok, 1)) {
        ಹಿಂತಿರುಗಿ -1;
    }
    ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ
Py_Tok_ಆರಂಭ_Decoder(ರಚನೆ tok_state *tok, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *errors)
{
    _PyTok_Reader *reader = tok->reader;
    ಆದರೆ (reader->encoding == ಖಾಲಿ || reader->decoder != ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (reader->ಬಗೆ == _PYTOK_READER_FILE &&
            strcmp(reader->encoding, "utf-8") == 0) {
        ಹಿಂತಿರುಗಿ 0;
    }

    PyObject *codec = Py_Codec_Lookup_ಪಠ್ಯ_Encoding(reader->encoding, ಖಾಲಿ);
    ಆದರೆ (codec != ಖಾಲಿ) {
        reader->decoder = _PyCodecInfo_GetIncrementalDecoder(codec, errors);
        Py_DECREF(codec);
    }
    ಆದರೆ (reader->decoder == ಖಾಲಿ) {
        tok->ಮುಗಿದ = PyErr_ExceptionMatches(Py_Exc_ಸ್ಮರಣೆ_Error)
            ? E_NOMEM : E_DECODE;
        ಆದರೆ (reader->ಬಗೆ == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != ಖಾಲಿ ? tok->filename : Py_None);
        }
        ಹಿಂತಿರುಗಿ -1;
    }
    ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ
_PyTok_DecodeChunk(ರಚನೆ tok_state *tok, _PyTok_Chunk *chunk, ಪೂರ್ಣಾಂಕ final)
{
    _PyTok_Reader *reader = tok->reader;
    ಆದರೆ (reader->decoder == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಪೂರ್ಣಾಂಕ strip_bom = reader->ಬಗೆ == _PYTOK_READER_READLINE &&
        chunk->len >= UTF16_BOM_ಉದ್ದಳತೆ &&
        (memcmp(chunk->ದತ್ತಾಂಶ, UTF16_LE_BOM, UTF16_BOM_ಉದ್ದಳತೆ) == 0 ||
         memcmp(chunk->ದತ್ತಾಂಶ, UTF16_BE_BOM, UTF16_BOM_ಉದ್ದಳತೆ) == 0);
    PyObject *ಒಳಹರಿವು;
    ಆದರೆ (chunk->ownership == _PYTOK_CHUNK_PYOBJECT &&
            Py_ಬೈಟುಗಳು_Check(chunk->owner) &&
            chunk->ದತ್ತಾಂಶ == Py_ಬೈಟುಗಳು_AS_STRING(chunk->owner)) {
        ಒಳಹರಿವು = Py_NewRef(chunk->owner);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        ಒಳಹರಿವು = Py_ಬೈಟುಗಳು_From_String_And_ಗಾತ್ರ(chunk->ದತ್ತಾಂಶ, chunk->len);
    }
    ಆದರೆ (ಒಳಹರಿವು == ಖಾಲಿ) {
        tok->ಮುಗಿದ = E_NOMEM;
        ಹಿಂತಿರುಗಿ -1;
    }
    PyObject *unicode = Py_Object_Call_ವಿಧಾನ_Obj_ವಾದಗಳು(
        reader->decoder, &_Py_ID(decode), ಒಳಹರಿವು,
        final ? Py_True : Py_False, ಖಾಲಿ);
    Py_DECREF(ಒಳಹರಿವು);
    ಆದರೆ (unicode == ಖಾಲಿ) {
        tok->ಮುಗಿದ = PyErr_ExceptionMatches(Py_Exc_ಸ್ಮರಣೆ_Error)
            ? E_NOMEM : E_DECODE;
        ಆದರೆ (reader->ಬಗೆ == _PYTOK_READER_FILE) {
            _PyTokenizer_raise_init_error(
                tok->filename != ಖಾಲಿ ? tok->filename : Py_None);
        }
        ಹಿಂತಿರುಗಿ -1;
    }
    ಆದರೆ (!PyUnicode_Check(unicode)) {
        PyErr_Format(PyExc_TypeError,
                     "decoder should return a string result, not '%.200s'",
                     Py_TYPE(unicode)->tp_name);
        Py_DECREF(unicode);
        tok->ಮುಗಿದ = E_DECODE;
        ಹಿಂತಿರುಗಿ -1;
    }
    ಹಿಂತಿರುಗಿ chunk_ಗಣ_unicode(tok, chunk, unicode, strip_bom);
}

ಪೂರ್ಣಾಂಕ
Py_Tok_Decoder_Has_Buffered_ಒಳಹರಿವು(ರಚನೆ tok_state *tok)
{
    ಆದರೆ (tok->reader->decoder == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ 0;
    }
    PyObject *state = Py_Object_Call_ವಿಧಾನ_No_ವಾದಗಳು(
        tok->reader->decoder, &_Py_ID(getstate));
    ಆದರೆ (state == ಖಾಲಿ) {
        tok->ಮುಗಿದ = PyErr_ExceptionMatches(Py_Exc_ಸ್ಮರಣೆ_Error)
            ? E_NOMEM : E_DECODE;
        ಹಿಂತಿರುಗಿ -1;
    }
    ಆದರೆ (!PyTuple_Check(state) || Py_Tuple_GET_ಗಾತ್ರ(state) != 2 ||
            !Py_ಬೈಟುಗಳು_Check(Py_Tuple_GET_ಅಂಶ(state, 0)) ||
            !PyLong_Check(Py_Tuple_GET_ಅಂಶ(state, 1))) {
        Py_DECREF(state);
        Py_Err_ಗಣ_String(PyExc_TypeError,
                        "incremental decoder getstate() must return (bytes, int)");
        tok->ಮುಗಿದ = E_DECODE;
        ಹಿಂತಿರುಗಿ -1;
    }
    ಪೂರ್ಣಾಂಕ pending = Py_ಬೈಟುಗಳು_GET_ಗಾತ್ರ(Py_Tuple_GET_ಅಂಶ(state, 0)) != 0;
    Py_DECREF(state);
    ಹಿಂತಿರುಗಿ pending;
}
