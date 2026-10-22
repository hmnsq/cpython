#ಅನ್ತರ್ಭಾವ <Python.h>
#ಅನ್ತರ್ಭಾವ "pycore_ast.h"           // _PyAST_Validate(),
#ಅನ್ತರ್ಭಾವ "pycore_pystate.h"       // _PyThreadState_GET()
#ಅನ್ತರ್ಭಾವ "pycore_parser.h"        // _PYPEGEN_NSTATISTICS
#ಅನ್ತರ್ಭಾವ "pycore_pyerrors.h"      // PyExc_IncompleteInputError
#ಅನ್ತರ್ಭಾವ "pycore_runtime.h"       // _PyRuntime
#ಅನ್ತರ್ಭಾವ "pycore_unicodeobject.h" // _PyUnicode_InternImmortal
#ಅನ್ತರ್ಭಾವ <errcode.h>

#ಅನ್ತರ್ಭಾವ "lexer/state.h"
#ಅನ್ತರ್ಭಾವ "tokenizer/tokenizer.h"
#ಅನ್ತರ್ಭಾವ "pegen.h"

#ವ್ಯಾಖ್ಯಾನಿಸು IDENTIFIER_CACHE_ಗಾತ್ರ 2048  // Must be a power of two.
#ವ್ಯಾಖ್ಯಾನಿಸು IDENTIFIER_CACHE_MAX_PROBES 8
#ವ್ಯಾಖ್ಯಾನಿಸು PEGEN_ARRAY_GROWTH_ಗುಣಕ 2
#ವ್ಯಾಖ್ಯಾನಿಸು TYPE_IGNORE_INITIAL_ಸಾಮರ್ಥ್ಯ 10

ರಚನೆ identifier_cache_ನಮೂದು {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ;   // Borrowed from arena-owned token bytes.
    Py_ssize_t len;
    Py_hash_t hash;
    PyObject *ಮೌಲ್ಯ;   // Borrowed from an arena-owned identifier.
};

// Internal parser functions

asdl_stmt_seq*
_PyPegen_interactive_exit(Parser *p)
{
    ಆದರೆ (p->errcode) {
        *(p->errcode) = E_EOF;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

Py_ssize_t
Py_Pegen_byte_ಸ್ಥಾನಾಂತರ_ಗಮ್ಯ_character_ಸ್ಥಾನಾಂತರ_ಸಾಲು(PyObject *ಸಾಲು, Py_ssize_t ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, Py_ssize_t ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ)
{
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ದತ್ತಾಂಶ = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ*)PyUnicode_AsUTF8(ಸಾಲು);
    ಆದರೆ (ದತ್ತಾಂಶ == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ -1;
    }

    Py_ssize_t len = 0;
    ಆಗ (ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ < ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ) {
        Py_UCS4 ch = ದತ್ತಾಂಶ[ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ];
        ಆದರೆ (ch < 0x80) {
            ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ += 1;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((ch & 0xe0) == 0xc0) {
            ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ += 2;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((ch & 0xf0) == 0xe0) {
            ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ += 3;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((ch & 0xf8) == 0xf0) {
            ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ += 4;
        } ಇಲ್ಲದಿದ್ದರೆ {
            Py_Err_ಗಣ_String(Py_Exc_ಮೌಲ್ಯ_Error, "Invalid UTF-8 sequence");
            ಹಿಂತಿರುಗಿ -1;
        }
        len++;
    }
    ಹಿಂತಿರುಗಿ len;
}

Py_ssize_t
Py_Pegen_byte_ಸ್ಥಾನಾಂತರ_ಗಮ್ಯ_character_ಸ್ಥಾನಾಂತರ_raw(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* str, Py_ssize_t ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ)
{
    Py_ssize_t len = (Py_ssize_t)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(str);
    ಆದರೆ (ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ > len + 1) {
        ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = len + 1;
    }
    ಪ್ರತಿಜ್ಞಾ(ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ >= 0);
    PyObject *ಪಠ್ಯ = PyUnicode_DecodeUTF8(str, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, "replace");
    ಆದರೆ (!ಪಠ್ಯ) {
        ಹಿಂತಿರುಗಿ -1;
    }
    Py_ssize_t ಗಾತ್ರ_2 = Py_Unicode_GET_ಉದ್ದಳತೆ(ಪಠ್ಯ);
    Py_DECREF(ಪಠ್ಯ);
    ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2;
}

Py_ssize_t
Py_Pegen_byte_ಸ್ಥಾನಾಂತರ_ಗಮ್ಯ_character_ಸ್ಥಾನಾಂತರ(PyObject *ಸಾಲು, Py_ssize_t ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ)
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str = PyUnicode_AsUTF8(ಸಾಲು);
    ಆದರೆ (!str) {
        ಹಿಂತಿರುಗಿ -1;
    }
    ಹಿಂತಿರುಗಿ Py_Pegen_byte_ಸ್ಥಾನಾಂತರ_ಗಮ್ಯ_character_ಸ್ಥಾನಾಂತರ_raw(str, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ);
}

// Here, mark is the start of the node, while p->mark is the end.
// If node==NULL, they should be the same.
ಪೂರ್ಣಾಂಕ
Py_Pegen_ಸೇರಿಸು_ಜ್ಞಾಪನ(Parser *p, ಪೂರ್ಣಾಂಕ mark, ಪೂರ್ಣಾಂಕ type, ಶೂನ್ಯ *ಗಂಟು)
{
    // Insert in front
    ಜ್ಞಾಪನ *m = _PyArena_Malloc(p->arena, ಗಾತ್ರ(ಜ್ಞಾಪನ));
    ಆದರೆ (m == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ -1;
    }
    m->type = type;
    m->ಗಂಟು = ಗಂಟು;
    m->mark = p->mark;
    m->ಮುಂದಿನ = p->ಟೋಕನುಗಳು[mark]->ಜ್ಞಾಪನ_3;
    p->ಟೋಕನುಗಳು[mark]->ಜ್ಞಾಪನ_3 = m;
    p->ಟೋಕನುಗಳು[mark]->ಜ್ಞಾಪನ_ಮುಖವಾಡ |= 1ULL << (type & 63);
    ಹಿಂತಿರುಗಿ 0;
}

// Like _PyPegen_insert_memo(), but returns the inserted Memo so callers
// can update it in place without re-walking the token's memo list.
ಜ್ಞಾಪನ *
Py_Pegen_ಸೇರಿಸು_ಜ್ಞಾಪನ_ನೇರ(Parser *p, ಪೂರ್ಣಾಂಕ mark, ಪೂರ್ಣಾಂಕ type)
{
    ಆದರೆ (Py_Pegen_ಸೇರಿಸು_ಜ್ಞಾಪನ(p, mark, type, ಖಾಲಿ) < 0) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ p->ಟೋಕನುಗಳು[mark]->ಜ್ಞಾಪನ_3;
}

// Like _PyPegen_insert_memo(), but updates an existing node if found.
ಪೂರ್ಣಾಂಕ
Py_Pegen_update_ಜ್ಞಾಪನ(Parser *p, ಪೂರ್ಣಾಂಕ mark, ಪೂರ್ಣಾಂಕ type, ಶೂನ್ಯ *ಗಂಟು)
{
    ಗಾಗಿ (ಜ್ಞಾಪನ *m = p->ಟೋಕನುಗಳು[mark]->ಜ್ಞಾಪನ_3; m != ಖಾಲಿ; m = m->ಮುಂದಿನ) {
        ಆದರೆ (m->type == type) {
            // Update existing node.
            m->ಗಂಟು = ಗಂಟು;
            m->mark = p->mark;
            ಹಿಂತಿರುಗಿ 0;
        }
    }
    // Insert new node.
    ಹಿಂತಿರುಗಿ Py_Pegen_ಸೇರಿಸು_ಜ್ಞಾಪನ(p, mark, type, ಗಂಟು);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
init_normalization(Parser *p)
{
    ಆದರೆ (p->normalize) {
        ಹಿಂತಿರುಗಿ 1;
    }
    p->normalize = PyImport_ImportModuleAttrString("unicodedata", "normalize");
    ಆದರೆ (!p->normalize)
    {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ
Py_Pegen_grow_loop_ಬಫರ್(ಶೂನ್ಯ ***ಬಫರ್, Py_ssize_t *ಸಾಮರ್ಥ್ಯ)
{
    ಆದರೆ ((ಗಾತ್ರ_t)*ಸಾಮರ್ಥ್ಯ > (ಗಾತ್ರ_t)PY_SSIZE_T_MAX /
            (PEGEN_ARRAY_GROWTH_ಗುಣಕ * ಗಾತ್ರ(**ಬಫರ್))) {
        ಹಿಂತಿರುಗಿ -1;
    }
    Py_ssize_t new_ಸಾಮರ್ಥ್ಯ = *ಸಾಮರ್ಥ್ಯ == 0
        ? 1 : *ಸಾಮರ್ಥ್ಯ * PEGEN_ARRAY_GROWTH_ಗುಣಕ;
    ಶೂನ್ಯ **new_ಬಫರ್ = PyMem_Realloc(*ಬಫರ್, new_ಸಾಮರ್ಥ್ಯ * ಗಾತ್ರ(**ಬಫರ್));
    ಆದರೆ (new_ಬಫರ್ == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ -1;
    }
    *ಬಫರ್ = new_ಬಫರ್;
    *ಸಾಮರ್ಥ್ಯ = new_ಸಾಮರ್ಥ್ಯ;
    ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
growable_comment_array_ಕೂಡಿಸು(growable_comment_array *arr, ಪೂರ್ಣಾಂಕ lineno, ಅಕ್ಷರ *comment)
{
    ಆದರೆ (arr->ಅಂಕೆ_ವಸ್ತುಗಳು >= arr->ಗಾತ್ರ_2) {
        ಆದರೆ (arr->ಗಾತ್ರ_2 > (ಗಾತ್ರ_t)PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_ಗುಣಕ * ಗಾತ್ರ(*arr->ವಸ್ತುಗಳು))) {
            ಹಿಂತಿರುಗಿ 0;
        }
        ಗಾತ್ರ_t new_ಗಾತ್ರ = arr->ಗಾತ್ರ_2 == 0
            ? TYPE_IGNORE_INITIAL_ಸಾಮರ್ಥ್ಯ
            : arr->ಗಾತ್ರ_2 * PEGEN_ARRAY_GROWTH_ಗುಣಕ;
        ಶೂನ್ಯ *new_ವಸ್ತುಗಳು_array = PyMem_Realloc(
            arr->ವಸ್ತುಗಳು, new_ಗಾತ್ರ * ಗಾತ್ರ(*arr->ವಸ್ತುಗಳು));
        ಆದರೆ (!new_ವಸ್ತುಗಳು_array) {
            ಹಿಂತಿರುಗಿ 0;
        }
        arr->ವಸ್ತುಗಳು = new_ವಸ್ತುಗಳು_array;
        arr->ಗಾತ್ರ_2 = new_ಗಾತ್ರ;
    }

    arr->ವಸ್ತುಗಳು[arr->ಅಂಕೆ_ವಸ್ತುಗಳು].lineno = lineno;
    arr->ವಸ್ತುಗಳು[arr->ಅಂಕೆ_ವಸ್ತುಗಳು].comment = comment;  // Take ownership
    arr->ಅಂಕೆ_ವಸ್ತುಗಳು++;
    ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಶೂನ್ಯ
growable_comment_array_deallocate(growable_comment_array *arr)
{
    ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < arr->ಅಂಕೆ_ವಸ್ತುಗಳು; i++) {
        PyMem_Free(arr->ವಸ್ತುಗಳು[i].comment);
    }
    PyMem_Free(arr->ವಸ್ತುಗಳು);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
_get_keyword_or_name_type(Parser *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ, Py_ssize_t ಉದ್ದಳತೆ)
{
    ಪ್ರತಿಜ್ಞಾ(ಉದ್ದಳತೆ > 0);

    ಆದರೆ (ಉದ್ದಳತೆ >= p->n_keyword_lists ||
        p->keywords[ಉದ್ದಳತೆ] == ಖಾಲಿ ||
        p->keywords[ಉದ್ದಳತೆ]->type == -1) {
        ಹಿಂತಿರುಗಿ NAME;
    }
    ಗಾಗಿ (KeywordToken *k = p->keywords[ಉದ್ದಳತೆ]; k != ಖಾಲಿ && k->type != -1; k++) {
        ಆದರೆ (memcmp(k->str, ಪಠ್ಯ, (ಗಾತ್ರ_t)ಉದ್ದಳತೆ) == 0) {
            ಹಿಂತಿರುಗಿ k->type;
        }
    }
    ಹಿಂತಿರುಗಿ NAME;
}

// Keep text only where grammar actions or helpers read it. Keyword error
// actions use fixed spellings, so keywords do not need their text.
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ
token_needs_ಪಠ್ಯ(ಪೂರ್ಣಾಂಕ type)
{
    ಆಯ್ಕೆ (type) {
        ಸ್ಥಿತಿ NAME:
        ಸ್ಥಿತಿ ಸಂಖ್ಯೆ:
        ಸ್ಥಿತಿ STRING:
        ಸ್ಥಿತಿ FSTRING_ಆರಂಭ:
        ಸ್ಥಿತಿ FSTRING_ಮಧ್ಯ:
        ಸ್ಥಿತಿ FSTRING_ಕೊನೆ:
        ಸ್ಥಿತಿ TSTRING_ಆರಂಭ:
        ಸ್ಥಿತಿ TSTRING_ಮಧ್ಯ:
        ಸ್ಥಿತಿ TSTRING_ಕೊನೆ:
        ಸ್ಥಿತಿ TYPE_COMMENT:
        ಸ್ಥಿತಿ NOTEQUAL:  // _PyPegen_check_barry_as_flufl() distinguishes != and <>.
            ಹಿಂತಿರುಗಿ 1;
        ಪೂರ್ವನಿಯೋಜಿತ:
            ಹಿಂತಿರುಗಿ 0;
    }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
initialize_token(Parser *p, Token *parser_token, ರಚನೆ token *new_token, ಪೂರ್ಣಾಂಕ token_type) {
    ಪ್ರತಿಜ್ಞಾ(parser_token != ಖಾಲಿ);

    parser_token->type = token_type;
    parser_token->ಬೈಟುಗಳು = ಖಾಲಿ;
    ಆದರೆ (token_needs_ಪಠ್ಯ(token_type) && new_token->ಹರವು.ಆರಂಭ >= 0) {
        Py_ssize_t ಉದ್ದಳತೆ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ = Py_Token_ಪಠ್ಯ_View(p->tok, new_token, &ಉದ್ದಳತೆ);
        ಆದರೆ (token_type == NAME) {
            parser_token->type = _get_keyword_or_name_type(p, ಪಠ್ಯ, ಉದ್ದಳತೆ);
        }
        // A name reclassified as a keyword does not need its text.
        ಆದರೆ (parser_token->type == token_type) {
            parser_token->ಬೈಟುಗಳು = Py_ಬೈಟುಗಳು_From_String_And_ಗಾತ್ರ(ಪಠ್ಯ, ಉದ್ದಳತೆ);
            ಆದರೆ (parser_token->ಬೈಟುಗಳು == ಖಾಲಿ) {
                ಹಿಂತಿರುಗಿ -1;
            }
            ಆದರೆ (Py_Arena_ಕೂಡಿಸು_Py_Object(p->arena, parser_token->ಬೈಟುಗಳು) < 0) {
                Py_DECREF(parser_token->ಬೈಟುಗಳು);
                ಹಿಂತಿರುಗಿ -1;
            }
        }
    }

    parser_token->metadata = ಖಾಲಿ;
    ಆದರೆ (new_token->metadata != ಖಾಲಿ) {
        ಆದರೆ (Py_Arena_ಕೂಡಿಸು_Py_Object(p->arena, new_token->metadata) < 0) {
            ಹಿಂತಿರುಗಿ -1;
        }
        parser_token->metadata = new_token->metadata;
        new_token->metadata = ಖಾಲಿ;
    }

    parser_token->level = new_token->level;
    parser_token->is_raw = new_token->is_raw;
    parser_token->lineno = new_token->ಆರಂಭ_loc.lineno;
    parser_token->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = new_token->ಕೊನೆ_loc.lineno == p->starting_lineno
        ? p->starting_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ + new_token->ಆರಂಭ_loc.byte_ಲಂಬಸಾಲು
        : new_token->ಆರಂಭ_loc.byte_ಲಂಬಸಾಲು;
    parser_token->ಕೊನೆ_lineno = new_token->ಕೊನೆ_loc.lineno;
    parser_token->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = new_token->ಕೊನೆ_loc.lineno == p->starting_lineno
        ? p->starting_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ + new_token->ಕೊನೆ_loc.byte_ಲಂಬಸಾಲು
        : new_token->ಕೊನೆ_loc.byte_ಲಂಬಸಾಲು;

    p->fill += 1;

    ಆದರೆ (token_type == ERRORTOKEN && Py_Tokenizer_Get_ಸ್ಥಿತಿ(p->tok) == E_DECODE) {
        ಹಿಂತಿರುಗಿ _Pypegen_raise_decode_error(p);
    }

    ಹಿಂತಿರುಗಿ (token_type == ERRORTOKEN ? _Pypegen_tokenizer_error(p) : 0);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
resize_ಟೋಕನುಗಳು_array(Parser *p) {
    ಆದರೆ (p->ಗಾತ್ರ_2 > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ / PEGEN_ARRAY_GROWTH_ಗುಣಕ ||
            (ಗಾತ್ರ_t)p->ಗಾತ್ರ_2 > PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_ಗುಣಕ * ಗಾತ್ರ(*p->ಟೋಕನುಗಳು))) {
        Py_Err_No_ಸ್ಮರಣೆ();
        ಹಿಂತಿರುಗಿ -1;
    }
    ಪೂರ್ಣಾಂಕ newsize = p->ಗಾತ್ರ_2 * PEGEN_ARRAY_GROWTH_ಗುಣಕ;
    Token **new_ಟೋಕನುಗಳು = PyMem_Realloc(p->ಟೋಕನುಗಳು, (ಗಾತ್ರ_t)newsize * ಗಾತ್ರ(Token *));
    ಆದರೆ (new_ಟೋಕನುಗಳು == ಖಾಲಿ) {
        Py_Err_No_ಸ್ಮರಣೆ();
        ಹಿಂತಿರುಗಿ -1;
    }
    p->ಟೋಕನುಗಳು = new_ಟೋಕನುಗಳು;

    // Allocate new tokens together without moving existing ones: parser rules
    // may still hold pointers into earlier blocks. Parser_Free frees each block.
    Token *chunk = PyMem_Calloc((ಗಾತ್ರ_t)(newsize - p->ಗಾತ್ರ_2), ಗಾತ್ರ(Token));
    ಆದರೆ (chunk == ಖಾಲಿ) {
        Py_Err_No_ಸ್ಮರಣೆ();
        ಹಿಂತಿರುಗಿ -1;
    }
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = p->ಗಾತ್ರ_2; i < newsize; i++) {
        p->ಟೋಕನುಗಳು[i] = &chunk[i - p->ಗಾತ್ರ_2];
    }
    p->ಗಾತ್ರ_2 = newsize;
    ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ
_PyPegen_fill_token(Parser *p)
{
    ರಚನೆ token new_token;
    _PyToken_Init(&new_token);
    _PyTokenizer_Get(p->tok, &new_token);

    // Record and skip '# type: ignore' comments
    ಆಗ (new_token.type == TYPE_IGNORE) {
        Py_ssize_t len;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ = Py_Token_ಪಠ್ಯ_View(p->tok, &new_token, &len);
        ಅಕ್ಷರ *ಟ್ಯಾಗ್ = PyMem_Malloc((ಗಾತ್ರ_t)len + 1);
        ಆದರೆ (ಟ್ಯಾಗ್ == ಖಾಲಿ) {
            Py_Err_No_ಸ್ಮರಣೆ();
            ಹೋಗು error;
        }
        ಸ್ಮರಣೆ_ನಕಲು(ಟ್ಯಾಗ್, ಪಠ್ಯ, (ಗಾತ್ರ_t)len);
        ಟ್ಯಾಗ್[len] = '\0';
        // Ownership of tag passes to the growable array
        ಆದರೆ (!growable_comment_array_ಕೂಡಿಸು(&p->type_ignore_comments, new_token.ಕೊನೆ_loc.lineno, ಟ್ಯಾಗ್)) {
            PyMem_Free(ಟ್ಯಾಗ್);
            Py_Err_No_ಸ್ಮರಣೆ();
            ಹೋಗು error;
        }
        _PyTokenizer_Get(p->tok, &new_token);
    }

    ಪೂರ್ಣಾಂಕ type = new_token.type;
    // EOF can finish an ordinary token. Only a boundary token means that
    // the grammar requested input beyond the last physical line.
    ಆದರೆ (type == ENDMARKER ||
        (type == DEDENT && (p->flags & Py_PARSE_ALLOW_INCOMPLETE_ಒಳಹರಿವು) &&
         Py_Tokenizer_Get_ಸ್ಥಿತಿ(p->tok) == E_EOF)) {
        p->ಒಳಹರಿವು_exhausted = 1;
    }

    // If we have reached the end and we are in single input mode we need to insert a newline and reset the parsing
    ಆದರೆ (p->ಆರಂಭ_ನಿಯಮ == Py_single_ಒಳಹರಿವು && type == ENDMARKER && p->parsing_started) {
        type = NEWLINE; /* Add an extra newline */
        p->parsing_started = 0;

        ಆದರೆ (!(p->flags & PyPARSE_DONT_IMPLY_DEDENT)) {
            _PyTokenizer_ImplyDedents(p->tok);
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        p->parsing_started = 1;
    }

    // Check if we are at the limit of the token array capacity and resize if needed
    ಆದರೆ ((p->fill == p->ಗಾತ್ರ_2) && (resize_ಟೋಕನುಗಳು_array(p) != 0)) {
        ಹೋಗು error;
    }

    Token *t = p->ಟೋಕನುಗಳು[p->fill];
    ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = initialize_token(p, t, &new_token, type);
    _PyToken_Free(&new_token);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
error:
    _PyToken_Free(&new_token);
    ಹಿಂತಿರುಗಿ -1;
}

#ಆದರೆ defined(Py_DEBUG)
// Instrumentation to count the effectiveness of memoization.
// The array counts the number of tokens skipped by memoization,
// indexed by type.

#ವ್ಯಾಖ್ಯಾನಿಸು NSTATISTICS _PYPEGEN_NSTATISTICS
#ವ್ಯಾಖ್ಯಾನಿಸು ಜ್ಞಾಪನ_statistics _PyRuntime.parser.ಜ್ಞಾಪನ_statistics

ಶೂನ್ಯ
Py_Pegen_clear_ಜ್ಞಾಪನ_statistics(ಶೂನ್ಯ)
{
    Py_Mutex_ಬೀಗ(&_PyRuntime.parser.mutex);
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < NSTATISTICS; i++) {
        ಜ್ಞಾಪನ_statistics[i] = 0;
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
}

PyObject *
Py_Pegen_get_ಜ್ಞಾಪನ_statistics(ಶೂನ್ಯ)
{
    PyObject *ret = PyList_New(NSTATISTICS);
    ಆದರೆ (ret == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    Py_Mutex_ಬೀಗ(&_PyRuntime.parser.mutex);
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < NSTATISTICS; i++) {
        PyObject *ಮೌಲ್ಯ = PyLong_FromLong(ಜ್ಞಾಪನ_statistics[i]);
        ಆದರೆ (ಮೌಲ್ಯ == ಖಾಲಿ) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
        // PyList_SetItem borrows a reference to value.
        ಆದರೆ (Py_List_ಗಣ_ಅಂಶ(ret, i, ಮೌಲ್ಯ) < 0) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
    ಹಿಂತಿರುಗಿ ret;
}
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ  // bool
_PyPegen_is_memoized(Parser *p, ಪೂರ್ಣಾಂಕ type, ಶೂನ್ಯ *pres)
{
    ಆದರೆ (p->mark == p->fill) {
        ಆದರೆ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ಹಿಂತಿರುಗಿ -1;
        }
    }

    Token *t = p->ಟೋಕನುಗಳು[p->mark];

    ಆದರೆ (!(t->ಜ್ಞಾಪನ_ಮುಖವಾಡ & (1ULL << (type & 63)))) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಗಾಗಿ (ಜ್ಞಾಪನ *m = t->ಜ್ಞಾಪನ_3; m != ಖಾಲಿ; m = m->ಮುಂದಿನ) {
        ಆದರೆ (m->type == type) {
#ಆದರೆ defined(Py_DEBUG)
            ಆದರೆ (0 <= type && type < NSTATISTICS) {
                ದೀರ್ಘ ಎಣಿಕೆ_2 = m->mark - p->mark;
                // A memoized negative result counts for one.
                ಆದರೆ (ಎಣಿಕೆ_2 <= 0) {
                    ಎಣಿಕೆ_2 = 1;
                }
                Py_Mutex_ಬೀಗ(&_PyRuntime.parser.mutex);
                ಜ್ಞಾಪನ_statistics[type] += ಎಣಿಕೆ_2;
                PyMutex_Unlock(&_PyRuntime.parser.mutex);
            }
#ಆದರೆ_ಅಂತ್ಯ
            p->mark = m->mark;
            *(ಶೂನ್ಯ **)(pres) = m->ಗಂಟು;
            ಹಿಂತಿರುಗಿ 1;
        }
    }
    ಹಿಂತಿರುಗಿ 0;
}

#ವ್ಯಾಖ್ಯಾನಿಸು LOOKAHEAD1(NAME, RES_TYPE)                                  \
    ಪೂರ್ಣಾಂಕ                                                             \
    NAME (ಪೂರ್ಣಾಂಕ ಧನಾತ್ಮಕ, RES_TYPE (func)(Parser *), Parser *p)       \
    {                                                               \
        ಪೂರ್ಣಾಂಕ mark = p->mark;                                         \
        ಶೂನ್ಯ *res = func(p);                                        \
        p->mark = mark;                                             \
        ಹಿಂತಿರುಗಿ (res != ಖಾಲಿ) == ಧನಾತ್ಮಕ;                           \
    }

LOOKAHEAD1(_PyPegen_lookahead, ಶೂನ್ಯ *)
LOOKAHEAD1(Py_Pegen_lookahead_for_ಅಭಿವ್ಯಕ್ತಿ, ಅಭಿವ್ಯಕ್ತಿ_ty)
LOOKAHEAD1(_PyPegen_lookahead_for_stmt, stmt_ty)
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು LOOKAHEAD1

#ವ್ಯಾಖ್ಯಾನಿಸು LOOKAHEAD2(NAME, RES_TYPE, T)                                   \
    ಪೂರ್ಣಾಂಕ                                                                 \
    NAME (ಪೂರ್ಣಾಂಕ ಧನಾತ್ಮಕ, RES_TYPE (func)(Parser *, T), Parser *p, T arg) \
    {                                                                   \
        ಪೂರ್ಣಾಂಕ mark = p->mark;                                             \
        ಶೂನ್ಯ *res = func(p, arg);                                       \
        p->mark = mark;                                                 \
        ಹಿಂತಿರುಗಿ (res != ಖಾಲಿ) == ಧನಾತ್ಮಕ;                               \
    }

LOOKAHEAD2(_PyPegen_lookahead_with_int, Token *, ಪೂರ್ಣಾಂಕ)
LOOKAHEAD2(_PyPegen_lookahead_with_string, ಅಭಿವ್ಯಕ್ತಿ_ty, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು LOOKAHEAD2

Token *
_PyPegen_expect_token(Parser *p, ಪೂರ್ಣಾಂಕ type)
{
    ಆದರೆ (p->mark == p->fill) {
        ಆದರೆ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
    }
    Token *t = p->ಟೋಕನುಗಳು[p->mark];
    ಆದರೆ (t->type != type) {
       ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    p->mark += 1;
    ಹಿಂತಿರುಗಿ t;
}

ಶೂನ್ಯ*
Py_Pegen_expect_forced_ಫಲಿತಾಂಶ(Parser *p, ಶೂನ್ಯ* ಫಲಿತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* expected) {

    ಆದರೆ (p->error_indicator == 1) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (ಫಲಿತಾಂಶ == ಖಾಲಿ) {
        RAISE_SYNTAX_ERROR("expected (%s)", expected);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

Token *
_PyPegen_expect_forced_token(Parser *p, ಪೂರ್ಣಾಂಕ type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* expected) {

    ಆದರೆ (p->error_indicator == 1) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (p->mark == p->fill) {
        ಆದರೆ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
    }
    Token *t = p->ಟೋಕನುಗಳು[p->mark];
    ಆದರೆ (t->type != type) {
        RAISE_SYNTAX_ERROR_ತಿಳಿದ_LOCATION(t, "expected '%s'", expected);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    p->mark += 1;
    ಹಿಂತಿರುಗಿ t;
}

ಅಭಿವ್ಯಕ್ತಿ_ty
_PyPegen_expect_soft_keyword(Parser *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *keyword)
{
    ಆದರೆ (p->mark == p->fill) {
        ಆದರೆ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
    }
    Token *t = p->ಟೋಕನುಗಳು[p->mark];
    ಆದರೆ (t->type != NAME) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = Py_ಬೈಟುಗಳು_As_String(t->ಬೈಟುಗಳು);
    ಆದರೆ (!s) {
        p->error_indicator = 1;
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (strcmp(s, keyword) != 0) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ _PyPegen_name_token(p);
}

Token *
Py_Pegen_get_ಕೊನೆಯ_nonnwhitespace_token(Parser *p)
{
    ಪ್ರತಿಜ್ಞಾ(p->mark >= 0);
    Token *token = ಖಾಲಿ;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ m = p->mark - 1; m >= 0; m--) {
        token = p->ಟೋಕನುಗಳು[m];
        ಆದರೆ (token->type != ENDMARKER && (token->type < NEWLINE || token->type > DEDENT)) {
            ನಿಲ್ಲಿಸು;
        }
    }
    ಹಿಂತಿರುಗಿ token;
}

PyObject *
_PyPegen_new_identifier(Parser *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *n)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, (Py_ssize_t)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(n), ಖಾಲಿ);
    ಆದರೆ (!id) {
        ಹೋಗು error;
    }
    /* Check whether there are non-ASCII characters in the
       identifier; if so, normalize to NFKC. */
    ಆದರೆ (!PyUnicode_IS_ASCII(id))
    {
        ಆದರೆ (!init_normalization(p))
        {
            Py_DECREF(id);
            ಹೋಗು error;
        }
        PyObject *form = PyUnicode_InternFromString("NFKC");
        ಆದರೆ (form == ಖಾಲಿ)
        {
            Py_DECREF(id);
            ಹೋಗು error;
        }
        PyObject *ವಾದಗಳು[2] = {form, id};
        PyObject *id2 = PyObject_Vectorcall(p->normalize, ವಾದಗಳು, 2, ಖಾಲಿ);
        Py_DECREF(id);
        Py_DECREF(form);
        ಆದರೆ (!id2) {
            ಹೋಗು error;
        }

        ಆದರೆ (!PyUnicode_Check(id2))
        {
            PyErr_Format(PyExc_TypeError,
                         "unicodedata.normalize() must return a string, not "
                         "%.200s",
                         _PyType_Name(Py_TYPE(id2)));
            Py_DECREF(id2);
            ಹೋಗು error;
        }
        id = id2;
    }
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ forbidden[] = {
        "None",
        "True",
        "False",
        ಖಾಲಿ
    };
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; forbidden[i] != ಖಾಲಿ; i++) {
        ಆದರೆ (Py_Unicode_Equal_ಗಮ್ಯ_ASCIIString(id, forbidden[i])) {
            PyErr_Format(Py_Exc_ಮೌಲ್ಯ_Error,
                         "identifier field can't represent '%s' constant",
                         forbidden[i]);
            Py_DECREF(id);
            ಹೋಗು error;
        }
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &id);
    ಆದರೆ (Py_Arena_ಕೂಡಿಸು_Py_Object(p->arena, id) < 0)
    {
        Py_DECREF(id);
        ಹೋಗು error;
    }
    ಹಿಂತಿರುಗಿ id;

error:
    p->error_indicator = 1;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

// Return an arena-owned identifier; callers borrow the reference.
ಸ್ಥಿರ PyObject *
get_cached_identifier(Parser *p, PyObject *ಬೈಟುಗಳು)
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = Py_ಬೈಟುಗಳು_As_String(ಬೈಟುಗಳು);
    ಆದರೆ (!s) {
        p->error_indicator = 1;
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    // Identifiers repeat constantly; a small span-keyed cache skips the
    // UTF-8 decode + intern for repeated occurrences. Keys point into
    // arena-owned token bytes and values are arena-owned interned strings,
    // so borrowed references are valid for the lifetime of the parse
    // (including the second error pass, which reuses parser and arena).
    Py_ssize_t len = Py_ಬೈಟುಗಳು_GET_ಗಾತ್ರ(ಬೈಟುಗಳು);
    Py_hash_t hash = PyObject_Hash(ಬೈಟುಗಳು);
    ಆದರೆ (hash == -1) {
        p->error_indicator = 1;
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    // Parses without identifiers do not need this cache.
    ಆದರೆ (p->identifier_cache == ಖಾಲಿ) {
        p->identifier_cache = PyMem_Calloc(
            IDENTIFIER_CACHE_ಗಾತ್ರ, ಗಾತ್ರ(*p->identifier_cache));
        ಆದರೆ (p->identifier_cache == ಖಾಲಿ) {
            p->error_indicator = 1;
            ಹಿಂತಿರುಗಿ Py_Err_No_ಸ್ಮರಣೆ();
        }
    }
    Identifier_Cache_ನಮೂದು *free_ಸ್ಥಾನ = ಖಾಲಿ;
    ಗಾತ್ರ_t idx = (ಗಾತ್ರ_t)hash & (IDENTIFIER_CACHE_ಗಾತ್ರ - 1);
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ probe = 0; probe < IDENTIFIER_CACHE_MAX_PROBES; probe++) {
        Identifier_Cache_ನಮೂದು *ನಮೂದು = &p->identifier_cache[
            (idx + probe) & (IDENTIFIER_CACHE_ಗಾತ್ರ - 1)];
        ಆದರೆ (ನಮೂದು->ಕೀಲಿ == ಖಾಲಿ) {
            free_ಸ್ಥಾನ = ನಮೂದು;
            ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ (ನಮೂದು->hash == hash && ನಮೂದು->len == len &&
            memcmp(ನಮೂದು->ಕೀಲಿ, s, len) == 0)
        {
            ಹಿಂತಿರುಗಿ ನಮೂದು->ಮೌಲ್ಯ;
        }
    }
    PyObject *id = _PyPegen_new_identifier(p, s);
    ಆದರೆ (id == ಖಾಲಿ) {
        p->error_indicator = 1;
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (free_ಸ್ಥಾನ != ಖಾಲಿ) {
        free_ಸ್ಥಾನ->ಕೀಲಿ = s;
        free_ಸ್ಥಾನ->len = len;
        free_ಸ್ಥಾನ->hash = hash;
        free_ಸ್ಥಾನ->ಮೌಲ್ಯ = id;
    }
    ಹಿಂತಿರುಗಿ id;
}

ಸ್ಥಿರ ಅಭಿವ್ಯಕ್ತಿ_ty
_PyPegen_name_from_token(Parser *p, Token* t)
{
    ಆದರೆ (t == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    // Reuse the AST node when backtracking revisits this token. Memo lookup
    // starts before the token and restores the position after it on a hit.
    // Token kinds can be memo keys: generated grammar rule IDs start at 1000.
    ಪೂರ್ಣಾಂಕ mark = p->mark - 1;
    p->mark = mark;
    ಅಭಿವ್ಯಕ್ತಿ_ty cached = ಖಾಲಿ;
    ಆದರೆ (_PyPegen_is_memoized(p, NAME, &cached)) {
        ಹಿಂತಿರುಗಿ cached;
    }
    p->mark = mark + 1;
    PyObject *id = get_cached_identifier(p, t->ಬೈಟುಗಳು);
    ಆದರೆ (id == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಅಭಿವ್ಯಕ್ತಿ_ty ಫಲಿತಾಂಶ = _PyAST_Name(id, ತುಂಬು, t->lineno, t->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                                t->ಕೊನೆ_lineno, t->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, p->arena);
    ಆದರೆ (ಫಲಿತಾಂಶ != ಖಾಲಿ && Py_Pegen_ಸೇರಿಸು_ಜ್ಞಾಪನ(p, mark, NAME, ಫಲಿತಾಂಶ) < 0) {
        p->error_indicator = 1;
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಅಭಿವ್ಯಕ್ತಿ_ty
_PyPegen_name_token(Parser *p)
{
    Token *t = _PyPegen_expect_token(p, NAME);
    ಹಿಂತಿರುಗಿ _PyPegen_name_from_token(p, t);
}

ಶೂನ್ಯ *
_PyPegen_string_token(Parser *p)
{
    ಹಿಂತಿರುಗಿ _PyPegen_expect_token(p, STRING);
}

ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_soft_keyword_token(Parser *p) {
    Token *t = _PyPegen_expect_token(p, NAME);
    ಆದರೆ (t == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಅಕ್ಷರ *the_token;
    Py_ssize_t ಗಾತ್ರ_2;
    Py_ಬೈಟುಗಳು_As_String_And_ಗಾತ್ರ(t->ಬೈಟುಗಳು, &the_token, &ಗಾತ್ರ_2);
    ಗಾಗಿ (ಅಕ್ಷರ **keyword = p->soft_keywords; *keyword != ಖಾಲಿ; keyword++) {
        ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(*keyword) == (ಗಾತ್ರ_t)ಗಾತ್ರ_2 &&
            ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(*keyword, the_token, (ಗಾತ್ರ_t)ಗಾತ್ರ_2) == 0) {
            ಹಿಂತಿರುಗಿ _PyPegen_name_from_token(p, t);
        }
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ PyObject *
parsenumber_raw(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s)
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆ;
    ದೀರ್ಘ x;
    ದ್ವಿಗುಣ dx;
    Py_complex compl;
    ಪೂರ್ಣಾಂಕ imflag;

    ಪ್ರತಿಜ್ಞಾ(s != ಖಾಲಿ);
    errno = 0;
    ಕೊನೆ = s + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s) - 1;
    imflag = *ಕೊನೆ == 'j' || *ಕೊನೆ == 'J';
    ಆದರೆ (s[0] == '0') {
        x = (ದೀರ್ಘ)PyOS_strtoul(s, (ಅಕ್ಷರ **)&ಕೊನೆ, 0);
        ಆದರೆ (x < 0 && errno == 0) {
            ಹಿಂತಿರುಗಿ PyLong_FromString(s, (ಅಕ್ಷರ **)0, 0);
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        x = PyOS_strtol(s, (ಅಕ್ಷರ **)&ಕೊನೆ, 0);
    }
    ಆದರೆ (*ಕೊನೆ == '\0') {
        ಆದರೆ (errno != 0) {
            ಹಿಂತಿರುಗಿ PyLong_FromString(s, (ಅಕ್ಷರ **)0, 0);
        }
        ಹಿಂತಿರುಗಿ PyLong_FromLong(x);
    }
    /* XXX Huge floats may silently fail */
    ಆದರೆ (imflag) {
        compl.real = 0.;
        compl.imag = Py_OS_string_ಗಮ್ಯ_double(s, (ಅಕ್ಷರ **)&ಕೊನೆ, ಖಾಲಿ);
        ಆದರೆ (compl.imag == -1.0 && PyErr_Occurred()) {
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
        ಹಿಂತಿರುಗಿ PyComplex_FromCComplex(compl);
    }
    dx = Py_OS_string_ಗಮ್ಯ_double(s, ಖಾಲಿ, ಖಾಲಿ);
    ಆದರೆ (dx == -1.0 && PyErr_Occurred()) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ PyFloat_FromDouble(dx);
}

ಸ್ಥಿರ PyObject *
parsenumber(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s)
{
    ಅಕ್ಷರ *dup;
    ಅಕ್ಷರ *ಕೊನೆ;
    PyObject *res = ಖಾಲಿ;

    ಪ್ರತಿಜ್ಞಾ(s != ಖಾಲಿ);

    ಆದರೆ (strchr(s, '_') == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ parsenumber_raw(s);
    }
    /* Create a duplicate without underscores. */
    dup = PyMem_Malloc(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s) + 1);
    ಆದರೆ (dup == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ Py_Err_No_ಸ್ಮರಣೆ();
    }
    ಕೊನೆ = dup;
    ಗಾಗಿ (; *s; s++) {
        ಆದರೆ (*s != '_') {
            *ಕೊನೆ++ = *s;
        }
    }
    *ಕೊನೆ = '\0';
    res = parsenumber_raw(dup);
    PyMem_Free(dup);
    ಹಿಂತಿರುಗಿ res;
}

ಅಭಿವ್ಯಕ್ತಿ_ty
Py_Pegen_ಸಂಖ್ಯೆ_token(Parser *p)
{
    ಪೂರ್ಣಾಂಕ mark = p->mark;
    Token *t = _PyPegen_expect_token(p, ಸಂಖ್ಯೆ);
    ಆದರೆ (t == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // Look up the node at the token's start, as for NAME tokens. A memo hit
    // advances the mark; on a miss, restore the position after the token.
    p->mark = mark;
    ಅಭಿವ್ಯಕ್ತಿ_ty cached = ಖಾಲಿ;
    ಆದರೆ (_PyPegen_is_memoized(p, ಸಂಖ್ಯೆ, &cached)) {
        ಹಿಂತಿರುಗಿ cached;
    }
    p->mark = mark + 1;

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಅಂಕೆ_raw = Py_ಬೈಟುಗಳು_As_String(t->ಬೈಟುಗಳು);
    ಆದರೆ (ಅಂಕೆ_raw == ಖಾಲಿ) {
        p->error_indicator = 1;
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (p->feature_ಆವೃತ್ತಿ < 6 && strchr(ಅಂಕೆ_raw, '_') != ಖಾಲಿ) {
        p->error_indicator = 1;
        ಹಿಂತಿರುಗಿ RAISE_SYNTAX_ERROR("Underscores in numeric literals are only supported "
                                  "in Python 3.6 and greater");
    }

    PyObject *c = parsenumber(ಅಂಕೆ_raw);

    ಆದರೆ (c == ಖಾಲಿ) {
        p->error_indicator = 1;
        PyThreadState *tstate = _PyThreadState_GET();
        // The only way a ValueError should happen in _this_ code is via
        // PyLong_FromString hitting a length limit.
        ಆದರೆ (tstate->current_exception != ಖಾಲಿ &&
            Py_TYPE(tstate->current_exception) == (PyTypeObject *)Py_Exc_ಮೌಲ್ಯ_Error
        ) {
            PyObject *exc = PyErr_GetRaisedException();
            /* Intentionally omitting columns to avoid a wall of 1000s of '^'s
             * on the error message. Nobody is going to overlook their huge
             * numeric literal once given the line. */
            RAISE_ERROR_ತಿಳಿದ_LOCATION(
                p, PyExc_SyntaxError,
                t->lineno, -1 /* col_offset */,
                t->ಕೊನೆ_lineno, -1 /* end_col_offset */,
                "%S - Consider hexadecimal for huge integer literals "
                "to avoid decimal conversion limits.",
                exc);
            Py_DECREF(exc);
        }
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (Py_Arena_ಕೂಡಿಸು_Py_Object(p->arena, c) < 0) {
        Py_DECREF(c);
        p->error_indicator = 1;
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಅಭಿವ್ಯಕ್ತಿ_ty ಫಲಿತಾಂಶ = _PyAST_Constant(c, ಖಾಲಿ, t->lineno, t->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                                    t->ಕೊನೆ_lineno, t->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, p->arena);
    ಆದರೆ (ಫಲಿತಾಂಶ != ಖಾಲಿ && Py_Pegen_ಸೇರಿಸು_ಜ್ಞಾಪನ(p, mark, ಸಂಖ್ಯೆ, ಫಲಿತಾಂಶ) < 0) {
        p->error_indicator = 1;
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}



ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compute_parser_flags(PyCompilerFlags *flags)
{
    ಪೂರ್ಣಾಂಕ parser_flags = 0;
    ಆದರೆ (!flags) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (flags->cf_flags & PyCF_DONT_IMPLY_DEDENT) {
        parser_flags |= PyPARSE_DONT_IMPLY_DEDENT;
    }
    ಆದರೆ (flags->cf_flags & PyCF_IGNORE_COOKIE) {
        parser_flags |= PyPARSE_IGNORE_COOKIE;
    }
    ಆದರೆ (flags->cf_flags & CO_FUTURE_BARRY_AS_BDFL) {
        parser_flags |= PyPARSE_BARRY_AS_BDFL;
    }
    ಆದರೆ (flags->cf_flags & PyCF_TYPE_COMMENTS) {
        parser_flags |= PyPARSE_TYPE_COMMENTS;
    }
    ಆದರೆ (flags->cf_flags & Py_CF_ALLOW_INCOMPLETE_ಒಳಹರಿವು) {
        parser_flags |= Py_PARSE_ALLOW_INCOMPLETE_ಒಳಹರಿವು;
    }
    ಹಿಂತಿರುಗಿ parser_flags;
}

// Parser API

Parser *
_PyPegen_Parser_New(ರಚನೆ tok_state *tok, ಪೂರ್ಣಾಂಕ ಆರಂಭ_ನಿಯಮ, ಪೂರ್ಣಾಂಕ flags,
                    ಪೂರ್ಣಾಂಕ feature_ಆವೃತ್ತಿ, ಪೂರ್ಣಾಂಕ *errcode, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮೂಲ, PyArena *arena)
{
    Parser *p = PyMem_Malloc(ಗಾತ್ರ(Parser));
    ಆದರೆ (p == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ (Parser *) Py_Err_No_ಸ್ಮರಣೆ();
    }
    ಪ್ರತಿಜ್ಞಾ(tok != ಖಾಲಿ);
    Py_Tokenizer_ಗಣ_Options(tok, 0, (flags & PyPARSE_TYPE_COMMENTS) > 0);
    Py_Tokenizer_Allow_Incomplete_ಒಳಹರಿವು(tok, flags & Py_PARSE_ALLOW_INCOMPLETE_ಒಳಹರಿವು);
    p->tok = tok;
    p->keywords = ಖಾಲಿ;
    p->n_keyword_lists = -1;
    p->soft_keywords = ಖಾಲಿ;
    p->ಟೋಕನುಗಳು = PyMem_Malloc(ಗಾತ್ರ(Token *));
    ಆದರೆ (!p->ಟೋಕನುಗಳು) {
        PyMem_Free(p);
        ಹಿಂತಿರುಗಿ (Parser *) Py_Err_No_ಸ್ಮರಣೆ();
    }
    p->ಟೋಕನುಗಳು[0] = PyMem_Calloc(1, ಗಾತ್ರ(Token));
    ಆದರೆ (!p->ಟೋಕನುಗಳು[0]) {
        PyMem_Free(p->ಟೋಕನುಗಳು);
        PyMem_Free(p);
        ಹಿಂತಿರುಗಿ (Parser *) Py_Err_No_ಸ್ಮರಣೆ();
    }
    p->type_ignore_comments = (growable_comment_array){0};

    p->mark = 0;
    p->fill = 0;
    p->ಗಾತ್ರ_2 = 1;

    p->errcode = errcode;
    p->arena = arena;
    p->ಆರಂಭ_ನಿಯಮ = ಆರಂಭ_ನಿಯಮ;
    p->parsing_started = 0;
    p->normalize = ಖಾಲಿ;
    p->error_indicator = 0;

    p->starting_lineno = 0;
    p->starting_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = 0;
    p->flags = flags;
    p->feature_ಆವೃತ್ತಿ = feature_ಆವೃತ್ತಿ;
    p->ತಿಳಿದ_err_token = ಖಾಲಿ;
    p->identifier_cache = ಖಾಲಿ;
    p->tstate = PyThreadState_Get();
    // Stack limits are initialized when the thread state is attached.
    ಪ್ರತಿಜ್ಞಾ(((_PyThreadStateImpl *)p->tstate)->c_ರಾಶಿ_hard_ಎಲ್ಲೆ != 0);
    p->ರಾಶಿ_soft_ಎಲ್ಲೆ = ((_PyThreadStateImpl *)p->tstate)->c_ರಾಶಿ_soft_ಎಲ್ಲೆ;
#ಆದರೆ Py_ರಾಶಿ_GROWS_ಕೆಳಕ್ಕೆ
    p->ರಾಶಿ_soft_ಎಲ್ಲೆ += Py_OS_ರಾಶಿ_MARGIN_ಬೈಟುಗಳು;
#ಇಲ್ಲದಿದ್ದರೆ
    p->ರಾಶಿ_soft_ಎಲ್ಲೆ -= Py_OS_ರಾಶಿ_MARGIN_ಬೈಟುಗಳು;
#ಆದರೆ_ಅಂತ್ಯ
    p->level = 0;
    p->call_invalid_ನಿಯಮಗಳು = 0;
    p->ಒಳಹರಿವು_exhausted = 0;
    p->ಕೊನೆಯ_stmt_location.lineno = 0;
    p->ಕೊನೆಯ_stmt_location.ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = 0;
    p->ಕೊನೆಯ_stmt_location.ಕೊನೆ_lineno = 0;
    p->ಕೊನೆಯ_stmt_location.ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ Py_DEBUG
    p->debug = _Py_GetConfig()->parser_debug;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ p;
}

ಶೂನ್ಯ
_PyPegen_Parser_Free(Parser *p)
{
    PyMem_Free(p->identifier_cache);
    Py_XDECREF(p->normalize);
    // Resizes allocate blocks starting at indices 1, 2, 4, and so on.
    PyMem_Free(p->ಟೋಕನುಗಳು[0]);
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < p->ಗಾತ್ರ_2; i *= PEGEN_ARRAY_GROWTH_ಗುಣಕ) {
        PyMem_Free(p->ಟೋಕನುಗಳು[i]);
    }
    PyMem_Free(p->ಟೋಕನುಗಳು);
    growable_comment_array_deallocate(&p->type_ignore_comments);
    PyMem_Free(p);
}

ಸ್ಥಿರ ಶೂನ್ಯ
reset_parser_state_for_error_pass(Parser *p)
{
    p->ಕೊನೆಯ_stmt_location.lineno = 0;
    p->ಕೊನೆಯ_stmt_location.ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = 0;
    p->ಕೊನೆಯ_stmt_location.ಕೊನೆ_lineno = 0;
    p->ಕೊನೆಯ_stmt_location.ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = 0;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < p->fill; i++) {
        p->ಟೋಕನುಗಳು[i]->ಜ್ಞಾಪನ_3 = ಖಾಲಿ;
        p->ಟೋಕನುಗಳು[i]->ಜ್ಞಾಪನ_ಮುಖವಾಡ = 0;
    }
    p->mark = 0;
    p->flags &= ~Py_PARSE_ALLOW_INCOMPLETE_ಒಳಹರಿವು;
    Py_Tokenizer_Allow_Incomplete_ಒಳಹರಿವು(p->tok, 0);
    p->call_invalid_ನಿಯಮಗಳು = 1;
    _PyTokenizer_StopInteractive(p->tok);
}

ಸ್ಥಿರ ಶೂನ್ಯ
Py_Pegen_ಗಣ_syntax_error_metadata(Parser *p) {
    PyObject *exc = PyErr_GetRaisedException();
    ಆದರೆ (!exc || !PyObject_TypeCheck(exc, (PyTypeObject *)PyExc_SyntaxError)) {
        Py_Err_ಗಣ_Raised_Exception(exc);
        ಹಿಂತಿರುಗಿ;
    }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ = Py_Tokenizer_Retained_ಮೂಲ(p->tok);
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *encoding = _PyTokenizer_GetInfo(p->tok).encoding;
    PyObject* the_ಮೂಲ = ಖಾಲಿ;
    ಆದರೆ (ಮೂಲ) {
        ಆದರೆ (encoding == ಖಾಲಿ) {
            the_ಮೂಲ = PyUnicode_FromString(ಮೂಲ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            the_ಮೂಲ = PyUnicode_Decode(ಮೂಲ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮೂಲ), encoding, ಖಾಲಿ);
        }
    }
    ಆದರೆ (!the_ಮೂಲ) {
        PyErr_Clear();
        the_ಮೂಲ = Py_None;
        Py_INCREF(the_ಮೂಲ);
    }
    PyObject* metadata = Py_Build_ಮೌಲ್ಯ(
        "(iiN)",
        p->ಕೊನೆಯ_stmt_location.lineno,
        p->ಕೊನೆಯ_stmt_location.ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
        the_ಮೂಲ // N gives ownership to metadata
    );
    ಆದರೆ (!metadata) {
        PyErr_Clear();
        Py_Err_ಗಣ_Raised_Exception(exc);
        ಹಿಂತಿರುಗಿ;
    }
    PySyntaxErrorObject *syntax_error = (PySyntaxErrorObject *)exc;

    Py_XDECREF(syntax_error->metadata);
    syntax_error->metadata = metadata;
    Py_Err_ಗಣ_Raised_Exception(exc);
}

ಶೂನ್ಯ *
Py_Pegen_ಸತತ_parser(Parser *p)
{
    ಶೂನ್ಯ *res = _PyPegen_parse(p);
    ಪ್ರತಿಜ್ಞಾ(p->level == 0);
    ಆದರೆ (res != ಖಾಲಿ && PyErr_Occurred()) {
        // Discard a result returned with an exception still pending
        // (e.g. a MemoryError from a recovered-from allocation failure).
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (res == ಖಾಲಿ) {
        ಆದರೆ (PyErr_Occurred() &&
            (PyErr_ExceptionMatches(Py_Exc_Incomplete_ಒಳಹರಿವು_Error) ||
             !PyErr_ExceptionMatches(PyExc_SyntaxError))) {
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
        ಆದರೆ ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_ಒಳಹರಿವು) &&
            p->ಒಳಹರಿವು_exhausted && !PyErr_Occurred()) {
            ಹಿಂತಿರುಗಿ Py_Pegen_raise_incomplete_ಒಳಹರಿವು(p);
        }
       // Make a second parser pass. In this pass we activate heavier and slower checks
        // to produce better error messages and more complete diagnostics. Extra "invalid_*"
        // rules will be active during parsing.
        Token *ಕೊನೆಯ_token = p->ಟೋಕನುಗಳು[p->fill - 1];
        reset_parser_state_for_error_pass(p);
        _PyPegen_parse(p);

        // Set SyntaxErrors accordingly depending on the parser/tokenizer status at the failure
        // point.
        Pypegen_ಗಣ_syntax_error(p, ಕೊನೆಯ_token);

        // Set the metadata in the exception from p->last_stmt_location
        ಆದರೆ (PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            Py_Pegen_ಗಣ_syntax_error_metadata(p);
        }
       ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (p->ಆರಂಭ_ನಿಯಮ == Py_single_ಒಳಹರಿವು && _PyTokenizer_HasTrailingStatement(p->tok)) {
        ಹಿಂತಿರುಗಿ RAISE_SYNTAX_ERROR("multiple statements found while compiling a single statement");
    }

    // test_peg_generator defines _Py_TEST_PEGEN to not call PyAST_Validate()
#ಆದರೆ defined(Py_DEBUG) && !defined(_Py_TEST_PEGEN)
    ಆದರೆ (p->ಆರಂಭ_ನಿಯಮ == Py_single_ಒಳಹರಿವು ||
        p->ಆರಂಭ_ನಿಯಮ == Py_file_ಒಳಹರಿವು ||
        p->ಆರಂಭ_ನಿಯಮ == Py_eval_ಒಳಹರಿವು)
    {
        ಆದರೆ (!_PyAST_Validate(res)) {
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ res;
}

mod_ty
Py_Pegen_ಸತತ_parser_from_file_ಸೂಚಕ(FILE *fp, ಪೂರ್ಣಾಂಕ ಆರಂಭ_ನಿಯಮ, PyObject *filename_ob,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *enc, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ps1, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ps2,
                             PyCompilerFlags *flags, ಪೂರ್ಣಾಂಕ *errcode,
                             PyObject **interactive_src, PyArena *arena)
{
    ಪೂರ್ಣಾಂಕ parser_flags = compute_parser_flags(flags);
    ರಚನೆ tok_state *tok = _PyTokenizer_FromFile(fp, enc, ps1, ps2);
    ಆದರೆ (tok == ಖಾಲಿ) {
        ಆದರೆ (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            // The only silent tokenizer init failure is a failed allocation.
            Py_Err_No_ಸ್ಮರಣೆ();
        }
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    // From here on we need to clean up even if there's an error
    mod_ty ಫಲಿತಾಂಶ = ಖಾಲಿ;

    PyObject *module = PyUnicode_FromString("__main__");
    ಆದರೆ (module == ಖಾಲಿ) {
        ಹೋಗು error;
    }
    Py_Tokenizer_ಗಣ_Context(tok, filename_ob, module);
    Py_DECREF(module);

    Parser *p = _PyPegen_Parser_New(tok, ಆರಂಭ_ನಿಯಮ, parser_flags, PY_MINOR_ಆವೃತ್ತಿ,
                                    errcode, ಖಾಲಿ, arena);
    ಆದರೆ (p == ಖಾಲಿ) {
        ಹೋಗು error;
    }

    ಫಲಿತಾಂಶ = Py_Pegen_ಸತತ_parser(p);
    _PyPegen_Parser_Free(p);

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ = Py_Tokenizer_Retained_ಮೂಲ(tok);
    ಆದರೆ (ಮೂಲ != ಖಾಲಿ && ಫಲಿತಾಂಶ && interactive_src != ಖಾಲಿ) {
        *interactive_src = PyUnicode_FromString(ಮೂಲ);
        ಆದರೆ (!*interactive_src || Py_Arena_ಕೂಡಿಸು_Py_Object(arena, *interactive_src) < 0) {
            Py_XDECREF(*interactive_src);
            ಫಲಿತಾಂಶ = ಖಾಲಿ;
            ಹೋಗು error;
        }
    }

error:
    _PyTokenizer_Free(tok);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

mod_ty
Py_Pegen_ಸತತ_parser_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, ಪೂರ್ಣಾಂಕ ಆರಂಭ_ನಿಯಮ, PyObject *filename_ob,
                       PyCompilerFlags *flags, PyArena *arena, PyObject *module,
                       PyObject **incomplete)
{
    _PyTokenizer_Newline newline_ಬಹುಲಕ = ಆರಂಭ_ನಿಯಮ == Py_file_ಒಳಹರಿವು
        ? _PyTokenizer_IMPLY_NEWLINE : _PyTokenizer_KEEP_NEWLINE;
    ಪೂರ್ಣಾಂಕ parser_flags = compute_parser_flags(flags);
    ಆದರೆ (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_ಒಳಹರಿವು) {
        newline_ಬಹುಲಕ = _PyTokenizer_APPEND_NEWLINE;
    }

    ರಚನೆ tok_state *tok;
    ಆದರೆ (flags != ಖಾಲಿ && flags->cf_flags & PyCF_IGNORE_COOKIE) {
        tok = _PyTokenizer_FromUTF8(str, newline_ಬಹುಲಕ, 0);
    } ಇಲ್ಲದಿದ್ದರೆ {
        tok = _PyTokenizer_FromString(str, newline_ಬಹುಲಕ, 0);
    }
    ಆದರೆ (tok == ಖಾಲಿ) {
        ಆದರೆ (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            // The only silent tokenizer init failure is a failed allocation.
            Py_Err_No_ಸ್ಮರಣೆ();
        }
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    Py_Tokenizer_ಗಣ_Context(tok, filename_ob, module);

    // We need to clear up from here on
    mod_ty ಫಲಿತಾಂಶ = ಖಾಲಿ;

    ಪೂರ್ಣಾಂಕ feature_ಆವೃತ್ತಿ = flags && (flags->cf_flags & PyCF_ONLY_AST) ?
        flags->cf_feature_ಆವೃತ್ತಿ : PY_MINOR_ಆವೃತ್ತಿ;
    Parser *p = _PyPegen_Parser_New(tok, ಆರಂಭ_ನಿಯಮ, parser_flags, feature_ಆವೃತ್ತಿ,
                                    ಖಾಲಿ, str, arena);
    ಆದರೆ (p == ಖಾಲಿ) {
        ಹೋಗು error;
    }

    ಫಲಿತಾಂಶ = Py_Pegen_ಸತತ_parser(p);
    ಆದರೆ (ಫಲಿತಾಂಶ != ಖಾಲಿ && (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_ಒಳಹರಿವು)) {
        ಪೂರ್ಣಾಂಕ needs_newline = _PyTokenizer_GetNewlineRequirements(tok);
        ಆದರೆ ((needs_newline & _PyTokenizer_NEWLINE_CONTINUATION) ||
            (ಆರಂಭ_ನಿಯಮ != Py_file_ಒಳಹರಿವು &&
             (needs_newline & _PyTokenizer_NEWLINE_INDENT)) ||
            (ಆರಂಭ_ನಿಯಮ == Py_single_ಒಳಹರಿವು && p->ಒಳಹರಿವು_exhausted &&
             (needs_newline & _PyTokenizer_NEWLINE_STATEMENT))) {
            Py_Pegen_raise_incomplete_ಒಳಹರಿವು(p);
            ಆದರೆ (incomplete != ಖಾಲಿ && PyErr_ExceptionMatches(Py_Exc_Incomplete_ಒಳಹರಿವು_Error)) {
                *incomplete = PyErr_GetRaisedException();
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                ಫಲಿತಾಂಶ = ಖಾಲಿ;
            }
        }
    }
    _PyPegen_Parser_Free(p);

error:
    _PyTokenizer_Free(tok);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
