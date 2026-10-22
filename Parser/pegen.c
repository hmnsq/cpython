#ਅਨ੍ਤਰ੍ਭਾਵ <Python.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_ast.h"           // _PyAST_Validate(),
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_pystate.h"       // _PyThreadState_GET()
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_parser.h"        // _PYPEGEN_NSTATISTICS
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_pyerrors.h"      // PyExc_IncompleteInputError
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_runtime.h"       // _PyRuntime
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_unicodeobject.h" // _PyUnicode_InternImmortal
#ਅਨ੍ਤਰ੍ਭਾਵ <errcode.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "lexer/state.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "tokenizer/tokenizer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pegen.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IDENTIFIER_CACHE_ਮਾਪ 2048  // Must be a power of two.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IDENTIFIER_CACHE_MAX_PROBES 8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PEGEN_ARRAY_GROWTH_ਗੁਣਕ 2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TYPE_IGNORE_INITIAL_ਸਮਰੱਥਾ 10

ਬਣਤਰ identifier_cache_ਇੰਦਰਾਜ {
    ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ;   // Borrowed from arena-owned token bytes.
    Py_ssize_t len;
    Py_hash_t hash;
    PyObject *ਮੁੱਲ;   // Borrowed from an arena-owned identifier.
};

// Internal parser functions

asdl_stmt_seq*
_PyPegen_interactive_exit(Parser *p)
{
    ਜੇ (p->errcode) {
        *(p->errcode) = E_EOF;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

Py_ssize_t
Py_Pegen_byte_ਵਿੱਥ_ਮੰਜ਼ਿਲ_character_ਵਿੱਥ_ਸਤਰ(PyObject *ਸਤਰ, Py_ssize_t ਖਾਨਾ_ਵਿੱਥ, Py_ssize_t ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ)
{
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਅੰਕੜੇ = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ*)PyUnicode_AsUTF8(ਸਤਰ);
    ਜੇ (ਅੰਕੜੇ == ਨਿਰਾ) {
        ਵਾਪਸ -1;
    }

    Py_ssize_t len = 0;
    ਜਦੋਂ (ਖਾਨਾ_ਵਿੱਥ < ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ) {
        Py_UCS4 ch = ਅੰਕੜੇ[ਖਾਨਾ_ਵਿੱਥ];
        ਜੇ (ch < 0x80) {
            ਖਾਨਾ_ਵਿੱਥ += 1;
        } ਨਹੀਂਤਾਂ ਜੇ ((ch & 0xe0) == 0xc0) {
            ਖਾਨਾ_ਵਿੱਥ += 2;
        } ਨਹੀਂਤਾਂ ਜੇ ((ch & 0xf0) == 0xe0) {
            ਖਾਨਾ_ਵਿੱਥ += 3;
        } ਨਹੀਂਤਾਂ ਜੇ ((ch & 0xf8) == 0xf0) {
            ਖਾਨਾ_ਵਿੱਥ += 4;
        } ਨਹੀਂਤਾਂ {
            Py_Err_ਸੈੱਟ_String(Py_Exc_ਮੁੱਲ_Error, "Invalid UTF-8 sequence");
            ਵਾਪਸ -1;
        }
        len++;
    }
    ਵਾਪਸ len;
}

Py_ssize_t
Py_Pegen_byte_ਵਿੱਥ_ਮੰਜ਼ਿਲ_character_ਵਿੱਥ_raw(ਅਚਲ ਅੱਖਰ* str, Py_ssize_t ਖਾਨਾ_ਵਿੱਥ)
{
    Py_ssize_t len = (Py_ssize_t)ਲੜੀ_ਲੰਬਾਈ(str);
    ਜੇ (ਖਾਨਾ_ਵਿੱਥ > len + 1) {
        ਖਾਨਾ_ਵਿੱਥ = len + 1;
    }
    ਪ੍ਰਤਿਜ੍ਞਾ(ਖਾਨਾ_ਵਿੱਥ >= 0);
    PyObject *ਪਾਠ = PyUnicode_DecodeUTF8(str, ਖਾਨਾ_ਵਿੱਥ, "replace");
    ਜੇ (!ਪਾਠ) {
        ਵਾਪਸ -1;
    }
    Py_ssize_t ਮਾਪ = Py_Unicode_GET_ਲੰਮਾਈ(ਪਾਠ);
    Py_DECREF(ਪਾਠ);
    ਵਾਪਸ ਮਾਪ;
}

Py_ssize_t
Py_Pegen_byte_ਵਿੱਥ_ਮੰਜ਼ਿਲ_character_ਵਿੱਥ(PyObject *ਸਤਰ, Py_ssize_t ਖਾਨਾ_ਵਿੱਥ)
{
    ਅਚਲ ਅੱਖਰ *str = PyUnicode_AsUTF8(ਸਤਰ);
    ਜੇ (!str) {
        ਵਾਪਸ -1;
    }
    ਵਾਪਸ Py_Pegen_byte_ਵਿੱਥ_ਮੰਜ਼ਿਲ_character_ਵਿੱਥ_raw(str, ਖਾਨਾ_ਵਿੱਥ);
}

// Here, mark is the start of the node, while p->mark is the end.
// If node==NULL, they should be the same.
ਪੂਰਨ
Py_Pegen_ਪਾਓ_ਯਾਦ(Parser *p, ਪੂਰਨ mark, ਪੂਰਨ type, ਖਾਲੀ *ਗੰਢ)
{
    // Insert in front
    ਯਾਦ *m = _PyArena_Malloc(p->arena, ਆਕਾਰ(ਯਾਦ));
    ਜੇ (m == ਨਿਰਾ) {
        ਵਾਪਸ -1;
    }
    m->type = type;
    m->ਗੰਢ = ਗੰਢ;
    m->mark = p->mark;
    m->ਅਗਲਾ = p->ਟੋਕਨ[mark]->ਯਾਦ_3;
    p->ਟੋਕਨ[mark]->ਯਾਦ_3 = m;
    p->ਟੋਕਨ[mark]->ਯਾਦ_ਨਕਾਬ |= 1ULL << (type & 63);
    ਵਾਪਸ 0;
}

// Like _PyPegen_insert_memo(), but returns the inserted Memo so callers
// can update it in place without re-walking the token's memo list.
ਯਾਦ *
Py_Pegen_ਪਾਓ_ਯਾਦ_ਸਿੱਧਾ(Parser *p, ਪੂਰਨ mark, ਪੂਰਨ type)
{
    ਜੇ (Py_Pegen_ਪਾਓ_ਯਾਦ(p, mark, type, ਨਿਰਾ) < 0) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ p->ਟੋਕਨ[mark]->ਯਾਦ_3;
}

// Like _PyPegen_insert_memo(), but updates an existing node if found.
ਪੂਰਨ
Py_Pegen_update_ਯਾਦ(Parser *p, ਪੂਰਨ mark, ਪੂਰਨ type, ਖਾਲੀ *ਗੰਢ)
{
    ਲਈ (ਯਾਦ *m = p->ਟੋਕਨ[mark]->ਯਾਦ_3; m != ਨਿਰਾ; m = m->ਅਗਲਾ) {
        ਜੇ (m->type == type) {
            // Update existing node.
            m->ਗੰਢ = ਗੰਢ;
            m->mark = p->mark;
            ਵਾਪਸ 0;
        }
    }
    // Insert new node.
    ਵਾਪਸ Py_Pegen_ਪਾਓ_ਯਾਦ(p, mark, type, ਗੰਢ);
}

ਸਥਿਰ ਪੂਰਨ
init_normalization(Parser *p)
{
    ਜੇ (p->normalize) {
        ਵਾਪਸ 1;
    }
    p->normalize = PyImport_ImportModuleAttrString("unicodedata", "normalize");
    ਜੇ (!p->normalize)
    {
        ਵਾਪਸ 0;
    }
    ਵਾਪਸ 1;
}

ਪੂਰਨ
Py_Pegen_grow_loop_ਬਫਰ(ਖਾਲੀ ***ਬਫਰ, Py_ssize_t *ਸਮਰੱਥਾ)
{
    ਜੇ ((ਮਾਪ_t)*ਸਮਰੱਥਾ > (ਮਾਪ_t)PY_SSIZE_T_MAX /
            (PEGEN_ARRAY_GROWTH_ਗੁਣਕ * ਆਕਾਰ(**ਬਫਰ))) {
        ਵਾਪਸ -1;
    }
    Py_ssize_t new_ਸਮਰੱਥਾ = *ਸਮਰੱਥਾ == 0
        ? 1 : *ਸਮਰੱਥਾ * PEGEN_ARRAY_GROWTH_ਗੁਣਕ;
    ਖਾਲੀ **new_ਬਫਰ = PyMem_Realloc(*ਬਫਰ, new_ਸਮਰੱਥਾ * ਆਕਾਰ(**ਬਫਰ));
    ਜੇ (new_ਬਫਰ == ਨਿਰਾ) {
        ਵਾਪਸ -1;
    }
    *ਬਫਰ = new_ਬਫਰ;
    *ਸਮਰੱਥਾ = new_ਸਮਰੱਥਾ;
    ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ
growable_comment_array_ਜੋੜੋ(growable_comment_array *arr, ਪੂਰਨ lineno, ਅੱਖਰ *comment)
{
    ਜੇ (arr->ਅੰਕ_ਚੀਜ਼ਾਂ >= arr->ਮਾਪ) {
        ਜੇ (arr->ਮਾਪ > (ਮਾਪ_t)PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_ਗੁਣਕ * ਆਕਾਰ(*arr->ਚੀਜ਼ਾਂ))) {
            ਵਾਪਸ 0;
        }
        ਮਾਪ_t new_ਮਾਪ = arr->ਮਾਪ == 0
            ? TYPE_IGNORE_INITIAL_ਸਮਰੱਥਾ
            : arr->ਮਾਪ * PEGEN_ARRAY_GROWTH_ਗੁਣਕ;
        ਖਾਲੀ *new_ਚੀਜ਼ਾਂ_array = PyMem_Realloc(
            arr->ਚੀਜ਼ਾਂ, new_ਮਾਪ * ਆਕਾਰ(*arr->ਚੀਜ਼ਾਂ));
        ਜੇ (!new_ਚੀਜ਼ਾਂ_array) {
            ਵਾਪਸ 0;
        }
        arr->ਚੀਜ਼ਾਂ = new_ਚੀਜ਼ਾਂ_array;
        arr->ਮਾਪ = new_ਮਾਪ;
    }

    arr->ਚੀਜ਼ਾਂ[arr->ਅੰਕ_ਚੀਜ਼ਾਂ].lineno = lineno;
    arr->ਚੀਜ਼ਾਂ[arr->ਅੰਕ_ਚੀਜ਼ਾਂ].comment = comment;  // Take ownership
    arr->ਅੰਕ_ਚੀਜ਼ਾਂ++;
    ਵਾਪਸ 1;
}

ਸਥਿਰ ਖਾਲੀ
growable_comment_array_deallocate(growable_comment_array *arr)
{
    ਲਈ (ਮਾਪ_t i = 0; i < arr->ਅੰਕ_ਚੀਜ਼ਾਂ; i++) {
        PyMem_Free(arr->ਚੀਜ਼ਾਂ[i].comment);
    }
    PyMem_Free(arr->ਚੀਜ਼ਾਂ);
}

ਸਥਿਰ ਪੂਰਨ
_get_keyword_or_name_type(Parser *p, ਅਚਲ ਅੱਖਰ *ਪਾਠ, Py_ssize_t ਲੰਮਾਈ)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(ਲੰਮਾਈ > 0);

    ਜੇ (ਲੰਮਾਈ >= p->n_keyword_lists ||
        p->keywords[ਲੰਮਾਈ] == ਨਿਰਾ ||
        p->keywords[ਲੰਮਾਈ]->type == -1) {
        ਵਾਪਸ NAME;
    }
    ਲਈ (KeywordToken *k = p->keywords[ਲੰਮਾਈ]; k != ਨਿਰਾ && k->type != -1; k++) {
        ਜੇ (memcmp(k->str, ਪਾਠ, (ਮਾਪ_t)ਲੰਮਾਈ) == 0) {
            ਵਾਪਸ k->type;
        }
    }
    ਵਾਪਸ NAME;
}

// Keep text only where grammar actions or helpers read it. Keyword error
// actions use fixed spellings, so keywords do not need their text.
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ
token_needs_ਪਾਠ(ਪੂਰਨ type)
{
    ਚੋਣ (type) {
        ਹਾਲਤ NAME:
        ਹਾਲਤ ਸੰਖਿਆ:
        ਹਾਲਤ STRING:
        ਹਾਲਤ FSTRING_ਸ਼ੁਰੂ:
        ਹਾਲਤ FSTRING_ਵਿਚਕਾਰ:
        ਹਾਲਤ FSTRING_ਸਿਰਾ:
        ਹਾਲਤ TSTRING_ਸ਼ੁਰੂ:
        ਹਾਲਤ TSTRING_ਵਿਚਕਾਰ:
        ਹਾਲਤ TSTRING_ਸਿਰਾ:
        ਹਾਲਤ TYPE_COMMENT:
        ਹਾਲਤ NOTEQUAL:  // _PyPegen_check_barry_as_flufl() distinguishes != and <>.
            ਵਾਪਸ 1;
        ਮੂਲ:
            ਵਾਪਸ 0;
    }
}

ਸਥਿਰ ਪੂਰਨ
initialize_token(Parser *p, Token *parser_token, ਬਣਤਰ token *new_token, ਪੂਰਨ token_type) {
    ਪ੍ਰਤਿਜ੍ਞਾ(parser_token != ਨਿਰਾ);

    parser_token->type = token_type;
    parser_token->ਬਾਈਟਾਂ = ਨਿਰਾ;
    ਜੇ (token_needs_ਪਾਠ(token_type) && new_token->ਵਿਸਤਾਰ.ਸ਼ੁਰੂ >= 0) {
        Py_ssize_t ਲੰਮਾਈ;
        ਅਚਲ ਅੱਖਰ *ਪਾਠ = Py_Token_ਪਾਠ_View(p->tok, new_token, &ਲੰਮਾਈ);
        ਜੇ (token_type == NAME) {
            parser_token->type = _get_keyword_or_name_type(p, ਪਾਠ, ਲੰਮਾਈ);
        }
        // A name reclassified as a keyword does not need its text.
        ਜੇ (parser_token->type == token_type) {
            parser_token->ਬਾਈਟਾਂ = Py_ਬਾਈਟਾਂ_From_String_And_ਮਾਪ(ਪਾਠ, ਲੰਮਾਈ);
            ਜੇ (parser_token->ਬਾਈਟਾਂ == ਨਿਰਾ) {
                ਵਾਪਸ -1;
            }
            ਜੇ (Py_Arena_ਜੋੜੋ_Py_Object(p->arena, parser_token->ਬਾਈਟਾਂ) < 0) {
                Py_DECREF(parser_token->ਬਾਈਟਾਂ);
                ਵਾਪਸ -1;
            }
        }
    }

    parser_token->metadata = ਨਿਰਾ;
    ਜੇ (new_token->metadata != ਨਿਰਾ) {
        ਜੇ (Py_Arena_ਜੋੜੋ_Py_Object(p->arena, new_token->metadata) < 0) {
            ਵਾਪਸ -1;
        }
        parser_token->metadata = new_token->metadata;
        new_token->metadata = ਨਿਰਾ;
    }

    parser_token->level = new_token->level;
    parser_token->is_raw = new_token->is_raw;
    parser_token->lineno = new_token->ਸ਼ੁਰੂ_loc.lineno;
    parser_token->ਖਾਨਾ_ਵਿੱਥ = new_token->ਸਿਰਾ_loc.lineno == p->starting_lineno
        ? p->starting_ਖਾਨਾ_ਵਿੱਥ + new_token->ਸ਼ੁਰੂ_loc.byte_ਖਾਨਾ
        : new_token->ਸ਼ੁਰੂ_loc.byte_ਖਾਨਾ;
    parser_token->ਸਿਰਾ_lineno = new_token->ਸਿਰਾ_loc.lineno;
    parser_token->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ = new_token->ਸਿਰਾ_loc.lineno == p->starting_lineno
        ? p->starting_ਖਾਨਾ_ਵਿੱਥ + new_token->ਸਿਰਾ_loc.byte_ਖਾਨਾ
        : new_token->ਸਿਰਾ_loc.byte_ਖਾਨਾ;

    p->fill += 1;

    ਜੇ (token_type == ERRORTOKEN && Py_Tokenizer_Get_ਸਥਿਤੀ(p->tok) == E_DECODE) {
        ਵਾਪਸ _Pypegen_raise_decode_error(p);
    }

    ਵਾਪਸ (token_type == ERRORTOKEN ? _Pypegen_tokenizer_error(p) : 0);
}

ਸਥਿਰ ਪੂਰਨ
resize_ਟੋਕਨ_array(Parser *p) {
    ਜੇ (p->ਮਾਪ > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ / PEGEN_ARRAY_GROWTH_ਗੁਣਕ ||
            (ਮਾਪ_t)p->ਮਾਪ > PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_ਗੁਣਕ * ਆਕਾਰ(*p->ਟੋਕਨ))) {
        Py_Err_No_ਸਮ੍ਰਿਤੀ();
        ਵਾਪਸ -1;
    }
    ਪੂਰਨ newsize = p->ਮਾਪ * PEGEN_ARRAY_GROWTH_ਗੁਣਕ;
    Token **new_ਟੋਕਨ = PyMem_Realloc(p->ਟੋਕਨ, (ਮਾਪ_t)newsize * ਆਕਾਰ(Token *));
    ਜੇ (new_ਟੋਕਨ == ਨਿਰਾ) {
        Py_Err_No_ਸਮ੍ਰਿਤੀ();
        ਵਾਪਸ -1;
    }
    p->ਟੋਕਨ = new_ਟੋਕਨ;

    // Allocate new tokens together without moving existing ones: parser rules
    // may still hold pointers into earlier blocks. Parser_Free frees each block.
    Token *chunk = PyMem_Calloc((ਮਾਪ_t)(newsize - p->ਮਾਪ), ਆਕਾਰ(Token));
    ਜੇ (chunk == ਨਿਰਾ) {
        Py_Err_No_ਸਮ੍ਰਿਤੀ();
        ਵਾਪਸ -1;
    }
    ਲਈ (ਪੂਰਨ i = p->ਮਾਪ; i < newsize; i++) {
        p->ਟੋਕਨ[i] = &chunk[i - p->ਮਾਪ];
    }
    p->ਮਾਪ = newsize;
    ਵਾਪਸ 0;
}

ਪੂਰਨ
_PyPegen_fill_token(Parser *p)
{
    ਬਣਤਰ token new_token;
    _PyToken_Init(&new_token);
    _PyTokenizer_Get(p->tok, &new_token);

    // Record and skip '# type: ignore' comments
    ਜਦੋਂ (new_token.type == TYPE_IGNORE) {
        Py_ssize_t len;
        ਅਚਲ ਅੱਖਰ *ਪਾਠ = Py_Token_ਪਾਠ_View(p->tok, &new_token, &len);
        ਅੱਖਰ *ਟੈਗ = PyMem_Malloc((ਮਾਪ_t)len + 1);
        ਜੇ (ਟੈਗ == ਨਿਰਾ) {
            Py_Err_No_ਸਮ੍ਰਿਤੀ();
            ਜਾਓ error;
        }
        ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਟੈਗ, ਪਾਠ, (ਮਾਪ_t)len);
        ਟੈਗ[len] = '\0';
        // Ownership of tag passes to the growable array
        ਜੇ (!growable_comment_array_ਜੋੜੋ(&p->type_ignore_comments, new_token.ਸਿਰਾ_loc.lineno, ਟੈਗ)) {
            PyMem_Free(ਟੈਗ);
            Py_Err_No_ਸਮ੍ਰਿਤੀ();
            ਜਾਓ error;
        }
        _PyTokenizer_Get(p->tok, &new_token);
    }

    ਪੂਰਨ type = new_token.type;
    // EOF can finish an ordinary token. Only a boundary token means that
    // the grammar requested input beyond the last physical line.
    ਜੇ (type == ENDMARKER ||
        (type == DEDENT && (p->flags & Py_PARSE_ALLOW_INCOMPLETE_ਨਿਵੇਸ਼) &&
         Py_Tokenizer_Get_ਸਥਿਤੀ(p->tok) == E_EOF)) {
        p->ਨਿਵੇਸ਼_exhausted = 1;
    }

    // If we have reached the end and we are in single input mode we need to insert a newline and reset the parsing
    ਜੇ (p->ਸ਼ੁਰੂ_ਨਿਯਮ == Py_single_ਨਿਵੇਸ਼ && type == ENDMARKER && p->parsing_started) {
        type = NEWLINE; /* Add an extra newline */
        p->parsing_started = 0;

        ਜੇ (!(p->flags & PyPARSE_DONT_IMPLY_DEDENT)) {
            _PyTokenizer_ImplyDedents(p->tok);
        }
    }
    ਨਹੀਂਤਾਂ {
        p->parsing_started = 1;
    }

    // Check if we are at the limit of the token array capacity and resize if needed
    ਜੇ ((p->fill == p->ਮਾਪ) && (resize_ਟੋਕਨ_array(p) != 0)) {
        ਜਾਓ error;
    }

    Token *t = p->ਟੋਕਨ[p->fill];
    ਪੂਰਨ ਨਤੀਜਾ = initialize_token(p, t, &new_token, type);
    _PyToken_Free(&new_token);
    ਵਾਪਸ ਨਤੀਜਾ;
error:
    _PyToken_Free(&new_token);
    ਵਾਪਸ -1;
}

#ਜੇ defined(Py_DEBUG)
// Instrumentation to count the effectiveness of memoization.
// The array counts the number of tokens skipped by memoization,
// indexed by type.

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NSTATISTICS _PYPEGEN_NSTATISTICS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਯਾਦ_statistics _PyRuntime.parser.ਯਾਦ_statistics

ਖਾਲੀ
Py_Pegen_clear_ਯਾਦ_statistics(ਖਾਲੀ)
{
    Py_Mutex_ਤਾਲਾ(&_PyRuntime.parser.mutex);
    ਲਈ (ਪੂਰਨ i = 0; i < NSTATISTICS; i++) {
        ਯਾਦ_statistics[i] = 0;
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
}

PyObject *
Py_Pegen_get_ਯਾਦ_statistics(ਖਾਲੀ)
{
    PyObject *ret = PyList_New(NSTATISTICS);
    ਜੇ (ret == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    Py_Mutex_ਤਾਲਾ(&_PyRuntime.parser.mutex);
    ਲਈ (ਪੂਰਨ i = 0; i < NSTATISTICS; i++) {
        PyObject *ਮੁੱਲ = PyLong_FromLong(ਯਾਦ_statistics[i]);
        ਜੇ (ਮੁੱਲ == ਨਿਰਾ) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            ਵਾਪਸ ਨਿਰਾ;
        }
        // PyList_SetItem borrows a reference to value.
        ਜੇ (Py_List_ਸੈੱਟ_ਵਸਤੂ(ret, i, ਮੁੱਲ) < 0) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            ਵਾਪਸ ਨਿਰਾ;
        }
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
    ਵਾਪਸ ret;
}
#ਜੇ_ਅੰਤ

ਪੂਰਨ  // bool
_PyPegen_is_memoized(Parser *p, ਪੂਰਨ type, ਖਾਲੀ *pres)
{
    ਜੇ (p->mark == p->fill) {
        ਜੇ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ਵਾਪਸ -1;
        }
    }

    Token *t = p->ਟੋਕਨ[p->mark];

    ਜੇ (!(t->ਯਾਦ_ਨਕਾਬ & (1ULL << (type & 63)))) {
        ਵਾਪਸ 0;
    }

    ਲਈ (ਯਾਦ *m = t->ਯਾਦ_3; m != ਨਿਰਾ; m = m->ਅਗਲਾ) {
        ਜੇ (m->type == type) {
#ਜੇ defined(Py_DEBUG)
            ਜੇ (0 <= type && type < NSTATISTICS) {
                ਲੰਮਾ ਗਿਣਤੀ_2 = m->mark - p->mark;
                // A memoized negative result counts for one.
                ਜੇ (ਗਿਣਤੀ_2 <= 0) {
                    ਗਿਣਤੀ_2 = 1;
                }
                Py_Mutex_ਤਾਲਾ(&_PyRuntime.parser.mutex);
                ਯਾਦ_statistics[type] += ਗਿਣਤੀ_2;
                PyMutex_Unlock(&_PyRuntime.parser.mutex);
            }
#ਜੇ_ਅੰਤ
            p->mark = m->mark;
            *(ਖਾਲੀ **)(pres) = m->ਗੰਢ;
            ਵਾਪਸ 1;
        }
    }
    ਵਾਪਸ 0;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOOKAHEAD1(NAME, RES_TYPE)                                  \
    ਪੂਰਨ                                                             \
    NAME (ਪੂਰਨ ਸਕਾਰਾਤਮਕ, RES_TYPE (func)(Parser *), Parser *p)       \
    {                                                               \
        ਪੂਰਨ mark = p->mark;                                         \
        ਖਾਲੀ *res = func(p);                                        \
        p->mark = mark;                                             \
        ਵਾਪਸ (res != ਨਿਰਾ) == ਸਕਾਰਾਤਮਕ;                           \
    }

LOOKAHEAD1(_PyPegen_lookahead, ਖਾਲੀ *)
LOOKAHEAD1(Py_Pegen_lookahead_for_ਵਿਅੰਜਕ, ਵਿਅੰਜਕ_ty)
LOOKAHEAD1(_PyPegen_lookahead_for_stmt, stmt_ty)
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ LOOKAHEAD1

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOOKAHEAD2(NAME, RES_TYPE, T)                                   \
    ਪੂਰਨ                                                                 \
    NAME (ਪੂਰਨ ਸਕਾਰਾਤਮਕ, RES_TYPE (func)(Parser *, T), Parser *p, T arg) \
    {                                                                   \
        ਪੂਰਨ mark = p->mark;                                             \
        ਖਾਲੀ *res = func(p, arg);                                       \
        p->mark = mark;                                                 \
        ਵਾਪਸ (res != ਨਿਰਾ) == ਸਕਾਰਾਤਮਕ;                               \
    }

LOOKAHEAD2(_PyPegen_lookahead_with_int, Token *, ਪੂਰਨ)
LOOKAHEAD2(_PyPegen_lookahead_with_string, ਵਿਅੰਜਕ_ty, ਅਚਲ ਅੱਖਰ *)
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ LOOKAHEAD2

Token *
_PyPegen_expect_token(Parser *p, ਪੂਰਨ type)
{
    ਜੇ (p->mark == p->fill) {
        ਜੇ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ਵਾਪਸ ਨਿਰਾ;
        }
    }
    Token *t = p->ਟੋਕਨ[p->mark];
    ਜੇ (t->type != type) {
       ਵਾਪਸ ਨਿਰਾ;
    }
    p->mark += 1;
    ਵਾਪਸ t;
}

ਖਾਲੀ*
Py_Pegen_expect_forced_ਨਤੀਜਾ(Parser *p, ਖਾਲੀ* ਨਤੀਜਾ, ਅਚਲ ਅੱਖਰ* expected) {

    ਜੇ (p->error_indicator == 1) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (ਨਤੀਜਾ == ਨਿਰਾ) {
        RAISE_SYNTAX_ERROR("expected (%s)", expected);
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

Token *
_PyPegen_expect_forced_token(Parser *p, ਪੂਰਨ type, ਅਚਲ ਅੱਖਰ* expected) {

    ਜੇ (p->error_indicator == 1) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (p->mark == p->fill) {
        ਜੇ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ਵਾਪਸ ਨਿਰਾ;
        }
    }
    Token *t = p->ਟੋਕਨ[p->mark];
    ਜੇ (t->type != type) {
        RAISE_SYNTAX_ERROR_ਗਿਆਤ_LOCATION(t, "expected '%s'", expected);
        ਵਾਪਸ ਨਿਰਾ;
    }
    p->mark += 1;
    ਵਾਪਸ t;
}

ਵਿਅੰਜਕ_ty
_PyPegen_expect_soft_keyword(Parser *p, ਅਚਲ ਅੱਖਰ *keyword)
{
    ਜੇ (p->mark == p->fill) {
        ਜੇ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ਵਾਪਸ ਨਿਰਾ;
        }
    }
    Token *t = p->ਟੋਕਨ[p->mark];
    ਜੇ (t->type != NAME) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਅਚਲ ਅੱਖਰ *s = Py_ਬਾਈਟਾਂ_As_String(t->ਬਾਈਟਾਂ);
    ਜੇ (!s) {
        p->error_indicator = 1;
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (strcmp(s, keyword) != 0) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ _PyPegen_name_token(p);
}

Token *
Py_Pegen_get_ਆਖਰੀ_nonnwhitespace_token(Parser *p)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(p->mark >= 0);
    Token *token = ਨਿਰਾ;
    ਲਈ (ਪੂਰਨ m = p->mark - 1; m >= 0; m--) {
        token = p->ਟੋਕਨ[m];
        ਜੇ (token->type != ENDMARKER && (token->type < NEWLINE || token->type > DEDENT)) {
            ਰੋਕੋ;
        }
    }
    ਵਾਪਸ token;
}

PyObject *
_PyPegen_new_identifier(Parser *p, ਅਚਲ ਅੱਖਰ *n)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, (Py_ssize_t)ਲੜੀ_ਲੰਬਾਈ(n), ਨਿਰਾ);
    ਜੇ (!id) {
        ਜਾਓ error;
    }
    /* Check whether there are non-ASCII characters in the
       identifier; if so, normalize to NFKC. */
    ਜੇ (!PyUnicode_IS_ASCII(id))
    {
        ਜੇ (!init_normalization(p))
        {
            Py_DECREF(id);
            ਜਾਓ error;
        }
        PyObject *form = PyUnicode_InternFromString("NFKC");
        ਜੇ (form == ਨਿਰਾ)
        {
            Py_DECREF(id);
            ਜਾਓ error;
        }
        PyObject *ਦਲੀਲਾਂ[2] = {form, id};
        PyObject *id2 = PyObject_Vectorcall(p->normalize, ਦਲੀਲਾਂ, 2, ਨਿਰਾ);
        Py_DECREF(id);
        Py_DECREF(form);
        ਜੇ (!id2) {
            ਜਾਓ error;
        }

        ਜੇ (!PyUnicode_Check(id2))
        {
            PyErr_Format(PyExc_TypeError,
                         "unicodedata.normalize() must return a string, not "
                         "%.200s",
                         _PyType_Name(Py_TYPE(id2)));
            Py_DECREF(id2);
            ਜਾਓ error;
        }
        id = id2;
    }
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ forbidden[] = {
        "None",
        "True",
        "False",
        ਨਿਰਾ
    };
    ਲਈ (ਪੂਰਨ i = 0; forbidden[i] != ਨਿਰਾ; i++) {
        ਜੇ (Py_Unicode_Equal_ਮੰਜ਼ਿਲ_ASCIIString(id, forbidden[i])) {
            PyErr_Format(Py_Exc_ਮੁੱਲ_Error,
                         "identifier field can't represent '%s' constant",
                         forbidden[i]);
            Py_DECREF(id);
            ਜਾਓ error;
        }
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &id);
    ਜੇ (Py_Arena_ਜੋੜੋ_Py_Object(p->arena, id) < 0)
    {
        Py_DECREF(id);
        ਜਾਓ error;
    }
    ਵਾਪਸ id;

error:
    p->error_indicator = 1;
    ਵਾਪਸ ਨਿਰਾ;
}

// Return an arena-owned identifier; callers borrow the reference.
ਸਥਿਰ PyObject *
get_cached_identifier(Parser *p, PyObject *ਬਾਈਟਾਂ)
{
    ਅਚਲ ਅੱਖਰ *s = Py_ਬਾਈਟਾਂ_As_String(ਬਾਈਟਾਂ);
    ਜੇ (!s) {
        p->error_indicator = 1;
        ਵਾਪਸ ਨਿਰਾ;
    }
    // Identifiers repeat constantly; a small span-keyed cache skips the
    // UTF-8 decode + intern for repeated occurrences. Keys point into
    // arena-owned token bytes and values are arena-owned interned strings,
    // so borrowed references are valid for the lifetime of the parse
    // (including the second error pass, which reuses parser and arena).
    Py_ssize_t len = Py_ਬਾਈਟਾਂ_GET_ਮਾਪ(ਬਾਈਟਾਂ);
    Py_hash_t hash = PyObject_Hash(ਬਾਈਟਾਂ);
    ਜੇ (hash == -1) {
        p->error_indicator = 1;
        ਵਾਪਸ ਨਿਰਾ;
    }
    // Parses without identifiers do not need this cache.
    ਜੇ (p->identifier_cache == ਨਿਰਾ) {
        p->identifier_cache = PyMem_Calloc(
            IDENTIFIER_CACHE_ਮਾਪ, ਆਕਾਰ(*p->identifier_cache));
        ਜੇ (p->identifier_cache == ਨਿਰਾ) {
            p->error_indicator = 1;
            ਵਾਪਸ Py_Err_No_ਸਮ੍ਰਿਤੀ();
        }
    }
    Identifier_Cache_ਇੰਦਰਾਜ *free_ਥਾਂ = ਨਿਰਾ;
    ਮਾਪ_t idx = (ਮਾਪ_t)hash & (IDENTIFIER_CACHE_ਮਾਪ - 1);
    ਲਈ (ਪੂਰਨ probe = 0; probe < IDENTIFIER_CACHE_MAX_PROBES; probe++) {
        Identifier_Cache_ਇੰਦਰਾਜ *ਇੰਦਰਾਜ = &p->identifier_cache[
            (idx + probe) & (IDENTIFIER_CACHE_ਮਾਪ - 1)];
        ਜੇ (ਇੰਦਰਾਜ->ਕੁੰਜੀ == ਨਿਰਾ) {
            free_ਥਾਂ = ਇੰਦਰਾਜ;
            ਰੋਕੋ;
        }
        ਜੇ (ਇੰਦਰਾਜ->hash == hash && ਇੰਦਰਾਜ->len == len &&
            memcmp(ਇੰਦਰਾਜ->ਕੁੰਜੀ, s, len) == 0)
        {
            ਵਾਪਸ ਇੰਦਰਾਜ->ਮੁੱਲ;
        }
    }
    PyObject *id = _PyPegen_new_identifier(p, s);
    ਜੇ (id == ਨਿਰਾ) {
        p->error_indicator = 1;
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (free_ਥਾਂ != ਨਿਰਾ) {
        free_ਥਾਂ->ਕੁੰਜੀ = s;
        free_ਥਾਂ->len = len;
        free_ਥਾਂ->hash = hash;
        free_ਥਾਂ->ਮੁੱਲ = id;
    }
    ਵਾਪਸ id;
}

ਸਥਿਰ ਵਿਅੰਜਕ_ty
_PyPegen_name_from_token(Parser *p, Token* t)
{
    ਜੇ (t == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    // Reuse the AST node when backtracking revisits this token. Memo lookup
    // starts before the token and restores the position after it on a hit.
    // Token kinds can be memo keys: generated grammar rule IDs start at 1000.
    ਪੂਰਨ mark = p->mark - 1;
    p->mark = mark;
    ਵਿਅੰਜਕ_ty cached = ਨਿਰਾ;
    ਜੇ (_PyPegen_is_memoized(p, NAME, &cached)) {
        ਵਾਪਸ cached;
    }
    p->mark = mark + 1;
    PyObject *id = get_cached_identifier(p, t->ਬਾਈਟਾਂ);
    ਜੇ (id == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਿਅੰਜਕ_ty ਨਤੀਜਾ = _PyAST_Name(id, ਲੱਦੋ, t->lineno, t->ਖਾਨਾ_ਵਿੱਥ,
                                t->ਸਿਰਾ_lineno, t->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, p->arena);
    ਜੇ (ਨਤੀਜਾ != ਨਿਰਾ && Py_Pegen_ਪਾਓ_ਯਾਦ(p, mark, NAME, ਨਤੀਜਾ) < 0) {
        p->error_indicator = 1;
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

ਵਿਅੰਜਕ_ty
_PyPegen_name_token(Parser *p)
{
    Token *t = _PyPegen_expect_token(p, NAME);
    ਵਾਪਸ _PyPegen_name_from_token(p, t);
}

ਖਾਲੀ *
_PyPegen_string_token(Parser *p)
{
    ਵਾਪਸ _PyPegen_expect_token(p, STRING);
}

ਵਿਅੰਜਕ_ty _PyPegen_soft_keyword_token(Parser *p) {
    Token *t = _PyPegen_expect_token(p, NAME);
    ਜੇ (t == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਅੱਖਰ *the_token;
    Py_ssize_t ਮਾਪ;
    Py_ਬਾਈਟਾਂ_As_String_And_ਮਾਪ(t->ਬਾਈਟਾਂ, &the_token, &ਮਾਪ);
    ਲਈ (ਅੱਖਰ **keyword = p->soft_keywords; *keyword != ਨਿਰਾ; keyword++) {
        ਜੇ (ਲੜੀ_ਲੰਬਾਈ(*keyword) == (ਮਾਪ_t)ਮਾਪ &&
            ਲੜੀ_ਤੁਲਨਾ(*keyword, the_token, (ਮਾਪ_t)ਮਾਪ) == 0) {
            ਵਾਪਸ _PyPegen_name_from_token(p, t);
        }
    }
    ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ PyObject *
parsenumber_raw(ਅਚਲ ਅੱਖਰ *s)
{
    ਅਚਲ ਅੱਖਰ *ਸਿਰਾ;
    ਲੰਮਾ x;
    ਦੁੱਗਣਾ dx;
    Py_complex compl;
    ਪੂਰਨ imflag;

    ਪ੍ਰਤਿਜ੍ਞਾ(s != ਨਿਰਾ);
    errno = 0;
    ਸਿਰਾ = s + ਲੜੀ_ਲੰਬਾਈ(s) - 1;
    imflag = *ਸਿਰਾ == 'j' || *ਸਿਰਾ == 'J';
    ਜੇ (s[0] == '0') {
        x = (ਲੰਮਾ)PyOS_strtoul(s, (ਅੱਖਰ **)&ਸਿਰਾ, 0);
        ਜੇ (x < 0 && errno == 0) {
            ਵਾਪਸ PyLong_FromString(s, (ਅੱਖਰ **)0, 0);
        }
    }
    ਨਹੀਂਤਾਂ {
        x = PyOS_strtol(s, (ਅੱਖਰ **)&ਸਿਰਾ, 0);
    }
    ਜੇ (*ਸਿਰਾ == '\0') {
        ਜੇ (errno != 0) {
            ਵਾਪਸ PyLong_FromString(s, (ਅੱਖਰ **)0, 0);
        }
        ਵਾਪਸ PyLong_FromLong(x);
    }
    /* XXX Huge floats may silently fail */
    ਜੇ (imflag) {
        compl.real = 0.;
        compl.imag = Py_OS_string_ਮੰਜ਼ਿਲ_double(s, (ਅੱਖਰ **)&ਸਿਰਾ, ਨਿਰਾ);
        ਜੇ (compl.imag == -1.0 && PyErr_Occurred()) {
            ਵਾਪਸ ਨਿਰਾ;
        }
        ਵਾਪਸ PyComplex_FromCComplex(compl);
    }
    dx = Py_OS_string_ਮੰਜ਼ਿਲ_double(s, ਨਿਰਾ, ਨਿਰਾ);
    ਜੇ (dx == -1.0 && PyErr_Occurred()) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ PyFloat_FromDouble(dx);
}

ਸਥਿਰ PyObject *
parsenumber(ਅਚਲ ਅੱਖਰ *s)
{
    ਅੱਖਰ *dup;
    ਅੱਖਰ *ਸਿਰਾ;
    PyObject *res = ਨਿਰਾ;

    ਪ੍ਰਤਿਜ੍ਞਾ(s != ਨਿਰਾ);

    ਜੇ (strchr(s, '_') == ਨਿਰਾ) {
        ਵਾਪਸ parsenumber_raw(s);
    }
    /* Create a duplicate without underscores. */
    dup = PyMem_Malloc(ਲੜੀ_ਲੰਬਾਈ(s) + 1);
    ਜੇ (dup == ਨਿਰਾ) {
        ਵਾਪਸ Py_Err_No_ਸਮ੍ਰਿਤੀ();
    }
    ਸਿਰਾ = dup;
    ਲਈ (; *s; s++) {
        ਜੇ (*s != '_') {
            *ਸਿਰਾ++ = *s;
        }
    }
    *ਸਿਰਾ = '\0';
    res = parsenumber_raw(dup);
    PyMem_Free(dup);
    ਵਾਪਸ res;
}

ਵਿਅੰਜਕ_ty
Py_Pegen_ਸੰਖਿਆ_token(Parser *p)
{
    ਪੂਰਨ mark = p->mark;
    Token *t = _PyPegen_expect_token(p, ਸੰਖਿਆ);
    ਜੇ (t == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    // Look up the node at the token's start, as for NAME tokens. A memo hit
    // advances the mark; on a miss, restore the position after the token.
    p->mark = mark;
    ਵਿਅੰਜਕ_ty cached = ਨਿਰਾ;
    ਜੇ (_PyPegen_is_memoized(p, ਸੰਖਿਆ, &cached)) {
        ਵਾਪਸ cached;
    }
    p->mark = mark + 1;

    ਅਚਲ ਅੱਖਰ *ਅੰਕ_raw = Py_ਬਾਈਟਾਂ_As_String(t->ਬਾਈਟਾਂ);
    ਜੇ (ਅੰਕ_raw == ਨਿਰਾ) {
        p->error_indicator = 1;
        ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (p->feature_ਸੰਸਕਰਣ < 6 && strchr(ਅੰਕ_raw, '_') != ਨਿਰਾ) {
        p->error_indicator = 1;
        ਵਾਪਸ RAISE_SYNTAX_ERROR("Underscores in numeric literals are only supported "
                                  "in Python 3.6 and greater");
    }

    PyObject *c = parsenumber(ਅੰਕ_raw);

    ਜੇ (c == ਨਿਰਾ) {
        p->error_indicator = 1;
        PyThreadState *tstate = _PyThreadState_GET();
        // The only way a ValueError should happen in _this_ code is via
        // PyLong_FromString hitting a length limit.
        ਜੇ (tstate->current_exception != ਨਿਰਾ &&
            Py_TYPE(tstate->current_exception) == (PyTypeObject *)Py_Exc_ਮੁੱਲ_Error
        ) {
            PyObject *exc = PyErr_GetRaisedException();
            /* Intentionally omitting columns to avoid a wall of 1000s of '^'s
             * on the error message. Nobody is going to overlook their huge
             * numeric literal once given the line. */
            RAISE_ERROR_ਗਿਆਤ_LOCATION(
                p, PyExc_SyntaxError,
                t->lineno, -1 /* col_offset */,
                t->ਸਿਰਾ_lineno, -1 /* end_col_offset */,
                "%S - Consider hexadecimal for huge integer literals "
                "to avoid decimal conversion limits.",
                exc);
            Py_DECREF(exc);
        }
        ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (Py_Arena_ਜੋੜੋ_Py_Object(p->arena, c) < 0) {
        Py_DECREF(c);
        p->error_indicator = 1;
        ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਿਅੰਜਕ_ty ਨਤੀਜਾ = _PyAST_Constant(c, ਨਿਰਾ, t->lineno, t->ਖਾਨਾ_ਵਿੱਥ,
                                    t->ਸਿਰਾ_lineno, t->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, p->arena);
    ਜੇ (ਨਤੀਜਾ != ਨਿਰਾ && Py_Pegen_ਪਾਓ_ਯਾਦ(p, mark, ਸੰਖਿਆ, ਨਤੀਜਾ) < 0) {
        p->error_indicator = 1;
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}



ਸਥਿਰ ਪੂਰਨ
compute_parser_flags(PyCompilerFlags *flags)
{
    ਪੂਰਨ parser_flags = 0;
    ਜੇ (!flags) {
        ਵਾਪਸ 0;
    }
    ਜੇ (flags->cf_flags & PyCF_DONT_IMPLY_DEDENT) {
        parser_flags |= PyPARSE_DONT_IMPLY_DEDENT;
    }
    ਜੇ (flags->cf_flags & PyCF_IGNORE_COOKIE) {
        parser_flags |= PyPARSE_IGNORE_COOKIE;
    }
    ਜੇ (flags->cf_flags & CO_FUTURE_BARRY_AS_BDFL) {
        parser_flags |= PyPARSE_BARRY_AS_BDFL;
    }
    ਜੇ (flags->cf_flags & PyCF_TYPE_COMMENTS) {
        parser_flags |= PyPARSE_TYPE_COMMENTS;
    }
    ਜੇ (flags->cf_flags & Py_CF_ALLOW_INCOMPLETE_ਨਿਵੇਸ਼) {
        parser_flags |= Py_PARSE_ALLOW_INCOMPLETE_ਨਿਵੇਸ਼;
    }
    ਵਾਪਸ parser_flags;
}

// Parser API

Parser *
_PyPegen_Parser_New(ਬਣਤਰ tok_state *tok, ਪੂਰਨ ਸ਼ੁਰੂ_ਨਿਯਮ, ਪੂਰਨ flags,
                    ਪੂਰਨ feature_ਸੰਸਕਰਣ, ਪੂਰਨ *errcode, ਅਚਲ ਅੱਖਰ* ਸਰੋਤ, PyArena *arena)
{
    Parser *p = PyMem_Malloc(ਆਕਾਰ(Parser));
    ਜੇ (p == ਨਿਰਾ) {
        ਵਾਪਸ (Parser *) Py_Err_No_ਸਮ੍ਰਿਤੀ();
    }
    ਪ੍ਰਤਿਜ੍ਞਾ(tok != ਨਿਰਾ);
    Py_Tokenizer_ਸੈੱਟ_Options(tok, 0, (flags & PyPARSE_TYPE_COMMENTS) > 0);
    Py_Tokenizer_Allow_Incomplete_ਨਿਵੇਸ਼(tok, flags & Py_PARSE_ALLOW_INCOMPLETE_ਨਿਵੇਸ਼);
    p->tok = tok;
    p->keywords = ਨਿਰਾ;
    p->n_keyword_lists = -1;
    p->soft_keywords = ਨਿਰਾ;
    p->ਟੋਕਨ = PyMem_Malloc(ਆਕਾਰ(Token *));
    ਜੇ (!p->ਟੋਕਨ) {
        PyMem_Free(p);
        ਵਾਪਸ (Parser *) Py_Err_No_ਸਮ੍ਰਿਤੀ();
    }
    p->ਟੋਕਨ[0] = PyMem_Calloc(1, ਆਕਾਰ(Token));
    ਜੇ (!p->ਟੋਕਨ[0]) {
        PyMem_Free(p->ਟੋਕਨ);
        PyMem_Free(p);
        ਵਾਪਸ (Parser *) Py_Err_No_ਸਮ੍ਰਿਤੀ();
    }
    p->type_ignore_comments = (growable_comment_array){0};

    p->mark = 0;
    p->fill = 0;
    p->ਮਾਪ = 1;

    p->errcode = errcode;
    p->arena = arena;
    p->ਸ਼ੁਰੂ_ਨਿਯਮ = ਸ਼ੁਰੂ_ਨਿਯਮ;
    p->parsing_started = 0;
    p->normalize = ਨਿਰਾ;
    p->error_indicator = 0;

    p->starting_lineno = 0;
    p->starting_ਖਾਨਾ_ਵਿੱਥ = 0;
    p->flags = flags;
    p->feature_ਸੰਸਕਰਣ = feature_ਸੰਸਕਰਣ;
    p->ਗਿਆਤ_err_token = ਨਿਰਾ;
    p->identifier_cache = ਨਿਰਾ;
    p->tstate = PyThreadState_Get();
    // Stack limits are initialized when the thread state is attached.
    ਪ੍ਰਤਿਜ੍ਞਾ(((_PyThreadStateImpl *)p->tstate)->c_ਢੇਰ_hard_ਹੱਦਬੰਦੀ != 0);
    p->ਢੇਰ_soft_ਹੱਦਬੰਦੀ = ((_PyThreadStateImpl *)p->tstate)->c_ਢੇਰ_soft_ਹੱਦਬੰਦੀ;
#ਜੇ Py_ਢੇਰ_GROWS_ਥੱਲੇ
    p->ਢੇਰ_soft_ਹੱਦਬੰਦੀ += Py_OS_ਢੇਰ_MARGIN_ਬਾਈਟਾਂ;
#ਨਹੀਂਤਾਂ
    p->ਢੇਰ_soft_ਹੱਦਬੰਦੀ -= Py_OS_ਢੇਰ_MARGIN_ਬਾਈਟਾਂ;
#ਜੇ_ਅੰਤ
    p->level = 0;
    p->call_invalid_ਨਿਯਮਾਂ = 0;
    p->ਨਿਵੇਸ਼_exhausted = 0;
    p->ਆਖਰੀ_stmt_location.lineno = 0;
    p->ਆਖਰੀ_stmt_location.ਖਾਨਾ_ਵਿੱਥ = 0;
    p->ਆਖਰੀ_stmt_location.ਸਿਰਾ_lineno = 0;
    p->ਆਖਰੀ_stmt_location.ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ Py_DEBUG
    p->debug = _Py_GetConfig()->parser_debug;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ p;
}

ਖਾਲੀ
_PyPegen_Parser_Free(Parser *p)
{
    PyMem_Free(p->identifier_cache);
    Py_XDECREF(p->normalize);
    // Resizes allocate blocks starting at indices 1, 2, 4, and so on.
    PyMem_Free(p->ਟੋਕਨ[0]);
    ਲਈ (ਪੂਰਨ i = 1; i < p->ਮਾਪ; i *= PEGEN_ARRAY_GROWTH_ਗੁਣਕ) {
        PyMem_Free(p->ਟੋਕਨ[i]);
    }
    PyMem_Free(p->ਟੋਕਨ);
    growable_comment_array_deallocate(&p->type_ignore_comments);
    PyMem_Free(p);
}

ਸਥਿਰ ਖਾਲੀ
reset_parser_state_for_error_pass(Parser *p)
{
    p->ਆਖਰੀ_stmt_location.lineno = 0;
    p->ਆਖਰੀ_stmt_location.ਖਾਨਾ_ਵਿੱਥ = 0;
    p->ਆਖਰੀ_stmt_location.ਸਿਰਾ_lineno = 0;
    p->ਆਖਰੀ_stmt_location.ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ = 0;
    ਲਈ (ਪੂਰਨ i = 0; i < p->fill; i++) {
        p->ਟੋਕਨ[i]->ਯਾਦ_3 = ਨਿਰਾ;
        p->ਟੋਕਨ[i]->ਯਾਦ_ਨਕਾਬ = 0;
    }
    p->mark = 0;
    p->flags &= ~Py_PARSE_ALLOW_INCOMPLETE_ਨਿਵੇਸ਼;
    Py_Tokenizer_Allow_Incomplete_ਨਿਵੇਸ਼(p->tok, 0);
    p->call_invalid_ਨਿਯਮਾਂ = 1;
    _PyTokenizer_StopInteractive(p->tok);
}

ਸਥਿਰ ਖਾਲੀ
Py_Pegen_ਸੈੱਟ_syntax_error_metadata(Parser *p) {
    PyObject *exc = PyErr_GetRaisedException();
    ਜੇ (!exc || !PyObject_TypeCheck(exc, (PyTypeObject *)PyExc_SyntaxError)) {
        Py_Err_ਸੈੱਟ_Raised_Exception(exc);
        ਵਾਪਸ;
    }
    ਅਚਲ ਅੱਖਰ *ਸਰੋਤ = Py_Tokenizer_Retained_ਸਰੋਤ(p->tok);
    ਅਚਲ ਅੱਖਰ *encoding = _PyTokenizer_GetInfo(p->tok).encoding;
    PyObject* the_ਸਰੋਤ = ਨਿਰਾ;
    ਜੇ (ਸਰੋਤ) {
        ਜੇ (encoding == ਨਿਰਾ) {
            the_ਸਰੋਤ = PyUnicode_FromString(ਸਰੋਤ);
        } ਨਹੀਂਤਾਂ {
            the_ਸਰੋਤ = PyUnicode_Decode(ਸਰੋਤ, ਲੜੀ_ਲੰਬਾਈ(ਸਰੋਤ), encoding, ਨਿਰਾ);
        }
    }
    ਜੇ (!the_ਸਰੋਤ) {
        PyErr_Clear();
        the_ਸਰੋਤ = Py_None;
        Py_INCREF(the_ਸਰੋਤ);
    }
    PyObject* metadata = Py_Build_ਮੁੱਲ(
        "(iiN)",
        p->ਆਖਰੀ_stmt_location.lineno,
        p->ਆਖਰੀ_stmt_location.ਖਾਨਾ_ਵਿੱਥ,
        the_ਸਰੋਤ // N gives ownership to metadata
    );
    ਜੇ (!metadata) {
        PyErr_Clear();
        Py_Err_ਸੈੱਟ_Raised_Exception(exc);
        ਵਾਪਸ;
    }
    PySyntaxErrorObject *syntax_error = (PySyntaxErrorObject *)exc;

    Py_XDECREF(syntax_error->metadata);
    syntax_error->metadata = metadata;
    Py_Err_ਸੈੱਟ_Raised_Exception(exc);
}

ਖਾਲੀ *
Py_Pegen_ਲੜੀਵਾਰ_parser(Parser *p)
{
    ਖਾਲੀ *res = _PyPegen_parse(p);
    ਪ੍ਰਤਿਜ੍ਞਾ(p->level == 0);
    ਜੇ (res != ਨਿਰਾ && PyErr_Occurred()) {
        // Discard a result returned with an exception still pending
        // (e.g. a MemoryError from a recovered-from allocation failure).
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (res == ਨਿਰਾ) {
        ਜੇ (PyErr_Occurred() &&
            (PyErr_ExceptionMatches(Py_Exc_Incomplete_ਨਿਵੇਸ਼_Error) ||
             !PyErr_ExceptionMatches(PyExc_SyntaxError))) {
            ਵਾਪਸ ਨਿਰਾ;
        }
        ਜੇ ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_ਨਿਵੇਸ਼) &&
            p->ਨਿਵੇਸ਼_exhausted && !PyErr_Occurred()) {
            ਵਾਪਸ Py_Pegen_raise_incomplete_ਨਿਵੇਸ਼(p);
        }
       // Make a second parser pass. In this pass we activate heavier and slower checks
        // to produce better error messages and more complete diagnostics. Extra "invalid_*"
        // rules will be active during parsing.
        Token *ਆਖਰੀ_token = p->ਟੋਕਨ[p->fill - 1];
        reset_parser_state_for_error_pass(p);
        _PyPegen_parse(p);

        // Set SyntaxErrors accordingly depending on the parser/tokenizer status at the failure
        // point.
        Pypegen_ਸੈੱਟ_syntax_error(p, ਆਖਰੀ_token);

        // Set the metadata in the exception from p->last_stmt_location
        ਜੇ (PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            Py_Pegen_ਸੈੱਟ_syntax_error_metadata(p);
        }
       ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (p->ਸ਼ੁਰੂ_ਨਿਯਮ == Py_single_ਨਿਵੇਸ਼ && _PyTokenizer_HasTrailingStatement(p->tok)) {
        ਵਾਪਸ RAISE_SYNTAX_ERROR("multiple statements found while compiling a single statement");
    }

    // test_peg_generator defines _Py_TEST_PEGEN to not call PyAST_Validate()
#ਜੇ defined(Py_DEBUG) && !defined(_Py_TEST_PEGEN)
    ਜੇ (p->ਸ਼ੁਰੂ_ਨਿਯਮ == Py_single_ਨਿਵੇਸ਼ ||
        p->ਸ਼ੁਰੂ_ਨਿਯਮ == Py_file_ਨਿਵੇਸ਼ ||
        p->ਸ਼ੁਰੂ_ਨਿਯਮ == Py_eval_ਨਿਵੇਸ਼)
    {
        ਜੇ (!_PyAST_Validate(res)) {
            ਵਾਪਸ ਨਿਰਾ;
        }
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ res;
}

mod_ty
Py_Pegen_ਲੜੀਵਾਰ_parser_from_file_ਸੂਚਕ(FILE *fp, ਪੂਰਨ ਸ਼ੁਰੂ_ਨਿਯਮ, PyObject *filename_ob,
                             ਅਚਲ ਅੱਖਰ *enc, ਅਚਲ ਅੱਖਰ *ps1, ਅਚਲ ਅੱਖਰ *ps2,
                             PyCompilerFlags *flags, ਪੂਰਨ *errcode,
                             PyObject **interactive_src, PyArena *arena)
{
    ਪੂਰਨ parser_flags = compute_parser_flags(flags);
    ਬਣਤਰ tok_state *tok = _PyTokenizer_FromFile(fp, enc, ps1, ps2);
    ਜੇ (tok == ਨਿਰਾ) {
        ਜੇ (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        ਨਹੀਂਤਾਂ {
            // The only silent tokenizer init failure is a failed allocation.
            Py_Err_No_ਸਮ੍ਰਿਤੀ();
        }
        ਵਾਪਸ ਨਿਰਾ;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    // From here on we need to clean up even if there's an error
    mod_ty ਨਤੀਜਾ = ਨਿਰਾ;

    PyObject *module = PyUnicode_FromString("__main__");
    ਜੇ (module == ਨਿਰਾ) {
        ਜਾਓ error;
    }
    Py_Tokenizer_ਸੈੱਟ_Context(tok, filename_ob, module);
    Py_DECREF(module);

    Parser *p = _PyPegen_Parser_New(tok, ਸ਼ੁਰੂ_ਨਿਯਮ, parser_flags, PY_MINOR_ਸੰਸਕਰਣ,
                                    errcode, ਨਿਰਾ, arena);
    ਜੇ (p == ਨਿਰਾ) {
        ਜਾਓ error;
    }

    ਨਤੀਜਾ = Py_Pegen_ਲੜੀਵਾਰ_parser(p);
    _PyPegen_Parser_Free(p);

    ਅਚਲ ਅੱਖਰ *ਸਰੋਤ = Py_Tokenizer_Retained_ਸਰੋਤ(tok);
    ਜੇ (ਸਰੋਤ != ਨਿਰਾ && ਨਤੀਜਾ && interactive_src != ਨਿਰਾ) {
        *interactive_src = PyUnicode_FromString(ਸਰੋਤ);
        ਜੇ (!*interactive_src || Py_Arena_ਜੋੜੋ_Py_Object(arena, *interactive_src) < 0) {
            Py_XDECREF(*interactive_src);
            ਨਤੀਜਾ = ਨਿਰਾ;
            ਜਾਓ error;
        }
    }

error:
    _PyTokenizer_Free(tok);
    ਵਾਪਸ ਨਤੀਜਾ;
}

mod_ty
Py_Pegen_ਲੜੀਵਾਰ_parser_from_string(ਅਚਲ ਅੱਖਰ *str, ਪੂਰਨ ਸ਼ੁਰੂ_ਨਿਯਮ, PyObject *filename_ob,
                       PyCompilerFlags *flags, PyArena *arena, PyObject *module,
                       PyObject **incomplete)
{
    _PyTokenizer_Newline newline_ਬਹੁਲਕ = ਸ਼ੁਰੂ_ਨਿਯਮ == Py_file_ਨਿਵੇਸ਼
        ? _PyTokenizer_IMPLY_NEWLINE : _PyTokenizer_KEEP_NEWLINE;
    ਪੂਰਨ parser_flags = compute_parser_flags(flags);
    ਜੇ (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_ਨਿਵੇਸ਼) {
        newline_ਬਹੁਲਕ = _PyTokenizer_APPEND_NEWLINE;
    }

    ਬਣਤਰ tok_state *tok;
    ਜੇ (flags != ਨਿਰਾ && flags->cf_flags & PyCF_IGNORE_COOKIE) {
        tok = _PyTokenizer_FromUTF8(str, newline_ਬਹੁਲਕ, 0);
    } ਨਹੀਂਤਾਂ {
        tok = _PyTokenizer_FromString(str, newline_ਬਹੁਲਕ, 0);
    }
    ਜੇ (tok == ਨਿਰਾ) {
        ਜੇ (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        ਨਹੀਂਤਾਂ {
            // The only silent tokenizer init failure is a failed allocation.
            Py_Err_No_ਸਮ੍ਰਿਤੀ();
        }
        ਵਾਪਸ ਨਿਰਾ;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    Py_Tokenizer_ਸੈੱਟ_Context(tok, filename_ob, module);

    // We need to clear up from here on
    mod_ty ਨਤੀਜਾ = ਨਿਰਾ;

    ਪੂਰਨ feature_ਸੰਸਕਰਣ = flags && (flags->cf_flags & PyCF_ONLY_AST) ?
        flags->cf_feature_ਸੰਸਕਰਣ : PY_MINOR_ਸੰਸਕਰਣ;
    Parser *p = _PyPegen_Parser_New(tok, ਸ਼ੁਰੂ_ਨਿਯਮ, parser_flags, feature_ਸੰਸਕਰਣ,
                                    ਨਿਰਾ, str, arena);
    ਜੇ (p == ਨਿਰਾ) {
        ਜਾਓ error;
    }

    ਨਤੀਜਾ = Py_Pegen_ਲੜੀਵਾਰ_parser(p);
    ਜੇ (ਨਤੀਜਾ != ਨਿਰਾ && (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_ਨਿਵੇਸ਼)) {
        ਪੂਰਨ needs_newline = _PyTokenizer_GetNewlineRequirements(tok);
        ਜੇ ((needs_newline & _PyTokenizer_NEWLINE_CONTINUATION) ||
            (ਸ਼ੁਰੂ_ਨਿਯਮ != Py_file_ਨਿਵੇਸ਼ &&
             (needs_newline & _PyTokenizer_NEWLINE_INDENT)) ||
            (ਸ਼ੁਰੂ_ਨਿਯਮ == Py_single_ਨਿਵੇਸ਼ && p->ਨਿਵੇਸ਼_exhausted &&
             (needs_newline & _PyTokenizer_NEWLINE_STATEMENT))) {
            Py_Pegen_raise_incomplete_ਨਿਵੇਸ਼(p);
            ਜੇ (incomplete != ਨਿਰਾ && PyErr_ExceptionMatches(Py_Exc_Incomplete_ਨਿਵੇਸ਼_Error)) {
                *incomplete = PyErr_GetRaisedException();
            }
            ਨਹੀਂਤਾਂ {
                ਨਤੀਜਾ = ਨਿਰਾ;
            }
        }
    }
    _PyPegen_Parser_Free(p);

error:
    _PyTokenizer_Free(tok);
    ਵਾਪਸ ਨਤੀਜਾ;
}
