#അന്തര്ഭാവ <Python.h>
#അന്തര്ഭാവ "pycore_ast.h"           // _PyAST_Validate(),
#അന്തര്ഭാവ "pycore_pystate.h"       // _PyThreadState_GET()
#അന്തര്ഭാവ "pycore_parser.h"        // _PYPEGEN_NSTATISTICS
#അന്തര്ഭാവ "pycore_pyerrors.h"      // PyExc_IncompleteInputError
#അന്തര്ഭാവ "pycore_runtime.h"       // _PyRuntime
#അന്തര്ഭാവ "pycore_unicodeobject.h" // _PyUnicode_InternImmortal
#അന്തര്ഭാവ <errcode.h>

#അന്തര്ഭാവ "lexer/state.h"
#അന്തര്ഭാവ "tokenizer/tokenizer.h"
#അന്തര്ഭാവ "pegen.h"

#define IDENTIFIER_CACHE_SIZE 2048  // Must be a power of two.
#define IDENTIFIER_CACHE_MAX_PROBES 8
#define PEGEN_ARRAY_GROWTH_FACTOR 2
#define TYPE_IGNORE_INITIAL_CAPACITY 10

ഘടന _identifier_cache_entry {
    സ്ഥിരം അക്ഷരം *താക്കോൽ;   // Borrowed from arena-owned token bytes.
    Py_ssize_t len;
    Py_hash_t hash;
    PyObject *മൂല്യം;   // Borrowed from an arena-owned identifier.
};

// Internal parser functions

asdl_stmt_seq*
_PyPegen_interactive_exit(Parser *p)
{
    എങ്കിൽ (p->errcode) {
        *(p->errcode) = E_EOF;
    }
    തിരികെ ശൂന്യ;
}

Py_ssize_t
Py_Pegen_byte_offset_to_character_offset_വരി(PyObject *വരി, Py_ssize_t col_offset, Py_ssize_t end_col_offset)
{
    സ്ഥിരം unsigned അക്ഷരം *data = (സ്ഥിരം unsigned അക്ഷരം*)PyUnicode_AsUTF8(വരി);
    എങ്കിൽ (data == ശൂന്യ) {
        തിരികെ -1;
    }

    Py_ssize_t len = 0;
    ആകുമ്പോൾ (col_offset < end_col_offset) {
        Py_UCS4 ch = data[col_offset];
        എങ്കിൽ (ch < 0x80) {
            col_offset += 1;
        } അല്ലെങ്കിൽ എങ്കിൽ ((ch & 0xe0) == 0xc0) {
            col_offset += 2;
        } അല്ലെങ്കിൽ എങ്കിൽ ((ch & 0xf0) == 0xe0) {
            col_offset += 3;
        } അല്ലെങ്കിൽ എങ്കിൽ ((ch & 0xf8) == 0xf0) {
            col_offset += 4;
        } അല്ലെങ്കിൽ {
            PyErr_SetString(Py_Exc_മൂല്യം_Error, "Invalid UTF-8 sequence");
            തിരികെ -1;
        }
        len++;
    }
    തിരികെ len;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset_raw(സ്ഥിരം അക്ഷരം* str, Py_ssize_t col_offset)
{
    Py_ssize_t len = (Py_ssize_t)strlen(str);
    എങ്കിൽ (col_offset > len + 1) {
        col_offset = len + 1;
    }
    പ്രതിജ്ഞാ(col_offset >= 0);
    PyObject *text = PyUnicode_DecodeUTF8(str, col_offset, "replace");
    എങ്കിൽ (!text) {
        തിരികെ -1;
    }
    Py_ssize_t size = PyUnicode_GET_LENGTH(text);
    Py_DECREF(text);
    തിരികെ size;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset(PyObject *വരി, Py_ssize_t col_offset)
{
    സ്ഥിരം അക്ഷരം *str = PyUnicode_AsUTF8(വരി);
    എങ്കിൽ (!str) {
        തിരികെ -1;
    }
    തിരികെ _PyPegen_byte_offset_to_character_offset_raw(str, col_offset);
}

// Here, mark is the start of the node, while p->mark is the end.
// If node==NULL, they should be the same.
പൂർണ്ണസംഖ്യ
Py_Pegen_തിരുകുക_memo(Parser *p, പൂർണ്ണസംഖ്യ mark, പൂർണ്ണസംഖ്യ type, ശൂന്യം *കണ്ണി)
{
    // Insert in front
    Memo *m = _PyArena_Malloc(p->arena, വലുപ്പം(Memo));
    എങ്കിൽ (m == ശൂന്യ) {
        തിരികെ -1;
    }
    m->type = type;
    m->കണ്ണി = കണ്ണി;
    m->mark = p->mark;
    m->next = p->tokens[mark]->memo;
    p->tokens[mark]->memo = m;
    p->tokens[mark]->memo_mask |= 1ULL << (type & 63);
    തിരികെ 0;
}

// Like _PyPegen_insert_memo(), but returns the inserted Memo so callers
// can update it in place without re-walking the token's memo list.
Memo *
Py_Pegen_തിരുകുക_memo_direct(Parser *p, പൂർണ്ണസംഖ്യ mark, പൂർണ്ണസംഖ്യ type)
{
    എങ്കിൽ (Py_Pegen_തിരുകുക_memo(p, mark, type, ശൂന്യ) < 0) {
        തിരികെ ശൂന്യ;
    }
    തിരികെ p->tokens[mark]->memo;
}

// Like _PyPegen_insert_memo(), but updates an existing node if found.
പൂർണ്ണസംഖ്യ
_PyPegen_update_memo(Parser *p, പൂർണ്ണസംഖ്യ mark, പൂർണ്ണസംഖ്യ type, ശൂന്യം *കണ്ണി)
{
    വേണ്ടി (Memo *m = p->tokens[mark]->memo; m != ശൂന്യ; m = m->next) {
        എങ്കിൽ (m->type == type) {
            // Update existing node.
            m->കണ്ണി = കണ്ണി;
            m->mark = p->mark;
            തിരികെ 0;
        }
    }
    // Insert new node.
    തിരികെ Py_Pegen_തിരുകുക_memo(p, mark, type, കണ്ണി);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
init_normalization(Parser *p)
{
    എങ്കിൽ (p->normalize) {
        തിരികെ 1;
    }
    p->normalize = PyImport_ImportModuleAttrString("unicodedata", "normalize");
    എങ്കിൽ (!p->normalize)
    {
        തിരികെ 0;
    }
    തിരികെ 1;
}

പൂർണ്ണസംഖ്യ
_PyPegen_grow_loop_buffer(ശൂന്യം ***buffer, Py_ssize_t *capacity)
{
    എങ്കിൽ ((size_t)*capacity > (size_t)PY_SSIZE_T_MAX /
            (PEGEN_ARRAY_GROWTH_FACTOR * വലുപ്പം(**buffer))) {
        തിരികെ -1;
    }
    Py_ssize_t new_capacity = *capacity == 0
        ? 1 : *capacity * PEGEN_ARRAY_GROWTH_FACTOR;
    ശൂന്യം **new_buffer = PyMem_Realloc(*buffer, new_capacity * വലുപ്പം(**buffer));
    എങ്കിൽ (new_buffer == ശൂന്യ) {
        തിരികെ -1;
    }
    *buffer = new_buffer;
    *capacity = new_capacity;
    തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
growable_comment_array_add(growable_comment_array *arr, പൂർണ്ണസംഖ്യ lineno, അക്ഷരം *comment)
{
    എങ്കിൽ (arr->അക്കം_ഇനങ്ങൾ >= arr->size) {
        എങ്കിൽ (arr->size > (size_t)PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * വലുപ്പം(*arr->ഇനങ്ങൾ))) {
            തിരികെ 0;
        }
        size_t new_size = arr->size == 0
            ? TYPE_IGNORE_INITIAL_CAPACITY
            : arr->size * PEGEN_ARRAY_GROWTH_FACTOR;
        ശൂന്യം *new_ഇനങ്ങൾ_array = PyMem_Realloc(
            arr->ഇനങ്ങൾ, new_size * വലുപ്പം(*arr->ഇനങ്ങൾ));
        എങ്കിൽ (!new_ഇനങ്ങൾ_array) {
            തിരികെ 0;
        }
        arr->ഇനങ്ങൾ = new_ഇനങ്ങൾ_array;
        arr->size = new_size;
    }

    arr->ഇനങ്ങൾ[arr->അക്കം_ഇനങ്ങൾ].lineno = lineno;
    arr->ഇനങ്ങൾ[arr->അക്കം_ഇനങ്ങൾ].comment = comment;  // Take ownership
    arr->അക്കം_ഇനങ്ങൾ++;
    തിരികെ 1;
}

സ്ഥിതം ശൂന്യം
growable_comment_array_deallocate(growable_comment_array *arr)
{
    വേണ്ടി (size_t i = 0; i < arr->അക്കം_ഇനങ്ങൾ; i++) {
        PyMem_Free(arr->ഇനങ്ങൾ[i].comment);
    }
    PyMem_Free(arr->ഇനങ്ങൾ);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
_get_keyword_or_name_type(Parser *p, സ്ഥിരം അക്ഷരം *text, Py_ssize_t length)
{
    പ്രതിജ്ഞാ(length > 0);

    എങ്കിൽ (length >= p->n_keyword_lists ||
        p->keywords[length] == ശൂന്യ ||
        p->keywords[length]->type == -1) {
        തിരികെ NAME;
    }
    വേണ്ടി (KeywordToken *k = p->keywords[length]; k != ശൂന്യ && k->type != -1; k++) {
        എങ്കിൽ (memcmp(k->str, text, (size_t)length) == 0) {
            തിരികെ k->type;
        }
    }
    തിരികെ NAME;
}

// Keep text only where grammar actions or helpers read it. Keyword error
// actions use fixed spellings, so keywords do not need their text.
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ
token_needs_text(പൂർണ്ണസംഖ്യ type)
{
    തിരഞ്ഞെടുപ്പ് (type) {
        അവസ്ഥ NAME:
        അവസ്ഥ സംഖ്യ:
        അവസ്ഥ STRING:
        അവസ്ഥ FSTRING_START:
        അവസ്ഥ FSTRING_നടുവ്:
        അവസ്ഥ FSTRING_END:
        അവസ്ഥ TSTRING_START:
        അവസ്ഥ TSTRING_നടുവ്:
        അവസ്ഥ TSTRING_END:
        അവസ്ഥ TYPE_COMMENT:
        അവസ്ഥ NOTEQUAL:  // _PyPegen_check_barry_as_flufl() distinguishes != and <>.
            തിരികെ 1;
        സ്ഥിരസ്ഥിതി:
            തിരികെ 0;
    }
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
initialize_token(Parser *p, Token *parser_token, ഘടന token *new_token, പൂർണ്ണസംഖ്യ token_type) {
    പ്രതിജ്ഞാ(parser_token != ശൂന്യ);

    parser_token->type = token_type;
    parser_token->bytes = ശൂന്യ;
    എങ്കിൽ (token_needs_text(token_type) && new_token->span.start >= 0) {
        Py_ssize_t length;
        സ്ഥിരം അക്ഷരം *text = _PyToken_TextView(p->tok, new_token, &length);
        എങ്കിൽ (token_type == NAME) {
            parser_token->type = _get_keyword_or_name_type(p, text, length);
        }
        // A name reclassified as a keyword does not need its text.
        എങ്കിൽ (parser_token->type == token_type) {
            parser_token->bytes = PyBytes_FromStringAndSize(text, length);
            എങ്കിൽ (parser_token->bytes == ശൂന്യ) {
                തിരികെ -1;
            }
            എങ്കിൽ (_PyArena_AddPyObject(p->arena, parser_token->bytes) < 0) {
                Py_DECREF(parser_token->bytes);
                തിരികെ -1;
            }
        }
    }

    parser_token->metadata = ശൂന്യ;
    എങ്കിൽ (new_token->metadata != ശൂന്യ) {
        എങ്കിൽ (_PyArena_AddPyObject(p->arena, new_token->metadata) < 0) {
            തിരികെ -1;
        }
        parser_token->metadata = new_token->metadata;
        new_token->metadata = ശൂന്യ;
    }

    parser_token->level = new_token->level;
    parser_token->is_raw = new_token->is_raw;
    parser_token->lineno = new_token->start_loc.lineno;
    parser_token->col_offset = new_token->end_loc.lineno == p->starting_lineno
        ? p->starting_col_offset + new_token->start_loc.byte_col
        : new_token->start_loc.byte_col;
    parser_token->end_lineno = new_token->end_loc.lineno;
    parser_token->end_col_offset = new_token->end_loc.lineno == p->starting_lineno
        ? p->starting_col_offset + new_token->end_loc.byte_col
        : new_token->end_loc.byte_col;

    p->fill += 1;

    എങ്കിൽ (token_type == ERRORTOKEN && _PyTokenizer_GetStatus(p->tok) == E_DECODE) {
        തിരികെ _Pypegen_raise_decode_error(p);
    }

    തിരികെ (token_type == ERRORTOKEN ? _Pypegen_tokenizer_error(p) : 0);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
_resize_tokens_array(Parser *p) {
    എങ്കിൽ (p->size > INT_MAX / PEGEN_ARRAY_GROWTH_FACTOR ||
            (size_t)p->size > PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * വലുപ്പം(*p->tokens))) {
        PyErr_NoMemory();
        തിരികെ -1;
    }
    പൂർണ്ണസംഖ്യ newsize = p->size * PEGEN_ARRAY_GROWTH_FACTOR;
    Token **new_tokens = PyMem_Realloc(p->tokens, (size_t)newsize * വലുപ്പം(Token *));
    എങ്കിൽ (new_tokens == ശൂന്യ) {
        PyErr_NoMemory();
        തിരികെ -1;
    }
    p->tokens = new_tokens;

    // Allocate new tokens together without moving existing ones: parser rules
    // may still hold pointers into earlier blocks. Parser_Free frees each block.
    Token *chunk = PyMem_Calloc((size_t)(newsize - p->size), വലുപ്പം(Token));
    എങ്കിൽ (chunk == ശൂന്യ) {
        PyErr_NoMemory();
        തിരികെ -1;
    }
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = p->size; i < newsize; i++) {
        p->tokens[i] = &chunk[i - p->size];
    }
    p->size = newsize;
    തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
_PyPegen_fill_token(Parser *p)
{
    ഘടന token new_token;
    _PyToken_Init(&new_token);
    _PyTokenizer_Get(p->tok, &new_token);

    // Record and skip '# type: ignore' comments
    ആകുമ്പോൾ (new_token.type == TYPE_IGNORE) {
        Py_ssize_t len;
        സ്ഥിരം അക്ഷരം *text = _PyToken_TextView(p->tok, &new_token, &len);
        അക്ഷരം *tag = PyMem_Malloc((size_t)len + 1);
        എങ്കിൽ (tag == ശൂന്യ) {
            PyErr_NoMemory();
            പോകുക error;
        }
        memcpy(tag, text, (size_t)len);
        tag[len] = '\0';
        // Ownership of tag passes to the growable array
        എങ്കിൽ (!growable_comment_array_add(&p->type_ignore_comments, new_token.end_loc.lineno, tag)) {
            PyMem_Free(tag);
            PyErr_NoMemory();
            പോകുക error;
        }
        _PyTokenizer_Get(p->tok, &new_token);
    }

    പൂർണ്ണസംഖ്യ type = new_token.type;
    // EOF can finish an ordinary token. Only a boundary token means that
    // the grammar requested input beyond the last physical line.
    എങ്കിൽ (type == ENDMARKER ||
        (type == DEDENT && (p->flags & Py_PARSE_ALLOW_INCOMPLETE_നിവേശം) &&
         _PyTokenizer_GetStatus(p->tok) == E_EOF)) {
        p->നിവേശം_exhausted = 1;
    }

    // If we have reached the end and we are in single input mode we need to insert a newline and reset the parsing
    എങ്കിൽ (p->start_rule == Py_single_നിവേശം && type == ENDMARKER && p->parsing_started) {
        type = NEWLINE; /* Add an extra newline */
        p->parsing_started = 0;

        എങ്കിൽ (!(p->flags & PyPARSE_DONT_IMPLY_DEDENT)) {
            _PyTokenizer_ImplyDedents(p->tok);
        }
    }
    അല്ലെങ്കിൽ {
        p->parsing_started = 1;
    }

    // Check if we are at the limit of the token array capacity and resize if needed
    എങ്കിൽ ((p->fill == p->size) && (_resize_tokens_array(p) != 0)) {
        പോകുക error;
    }

    Token *t = p->tokens[p->fill];
    പൂർണ്ണസംഖ്യ result = initialize_token(p, t, &new_token, type);
    _PyToken_Free(&new_token);
    തിരികെ result;
error:
    _PyToken_Free(&new_token);
    തിരികെ -1;
}

#എങ്കിൽ defined(Py_DEBUG)
// Instrumentation to count the effectiveness of memoization.
// The array counts the number of tokens skipped by memoization,
// indexed by type.

#define NSTATISTICS _PYPEGEN_NSTATISTICS
#define memo_statistics _PyRuntime.parser.memo_statistics

ശൂന്യം
_PyPegen_clear_memo_statistics(ശൂന്യം)
{
    PyMutex_Lock(&_PyRuntime.parser.mutex);
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < NSTATISTICS; i++) {
        memo_statistics[i] = 0;
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
}

PyObject *
_PyPegen_get_memo_statistics(ശൂന്യം)
{
    PyObject *ret = PyList_New(NSTATISTICS);
    എങ്കിൽ (ret == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }

    PyMutex_Lock(&_PyRuntime.parser.mutex);
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < NSTATISTICS; i++) {
        PyObject *മൂല്യം = PyLong_FromLong(memo_statistics[i]);
        എങ്കിൽ (മൂല്യം == ശൂന്യ) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            തിരികെ ശൂന്യ;
        }
        // PyList_SetItem borrows a reference to value.
        എങ്കിൽ (PyList_SetItem(ret, i, മൂല്യം) < 0) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            തിരികെ ശൂന്യ;
        }
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
    തിരികെ ret;
}
#endif

പൂർണ്ണസംഖ്യ  // bool
_PyPegen_is_memoized(Parser *p, പൂർണ്ണസംഖ്യ type, ശൂന്യം *pres)
{
    എങ്കിൽ (p->mark == p->fill) {
        എങ്കിൽ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            തിരികെ -1;
        }
    }

    Token *t = p->tokens[p->mark];

    എങ്കിൽ (!(t->memo_mask & (1ULL << (type & 63)))) {
        തിരികെ 0;
    }

    വേണ്ടി (Memo *m = t->memo; m != ശൂന്യ; m = m->next) {
        എങ്കിൽ (m->type == type) {
#എങ്കിൽ defined(Py_DEBUG)
            എങ്കിൽ (0 <= type && type < NSTATISTICS) {
                long എണ്ണം = m->mark - p->mark;
                // A memoized negative result counts for one.
                എങ്കിൽ (എണ്ണം <= 0) {
                    എണ്ണം = 1;
                }
                PyMutex_Lock(&_PyRuntime.parser.mutex);
                memo_statistics[type] += എണ്ണം;
                PyMutex_Unlock(&_PyRuntime.parser.mutex);
            }
#endif
            p->mark = m->mark;
            *(ശൂന്യം **)(pres) = m->കണ്ണി;
            തിരികെ 1;
        }
    }
    തിരികെ 0;
}

#define LOOKAHEAD1(NAME, RES_TYPE)                                  \
    പൂർണ്ണസംഖ്യ                                                             \
    NAME (പൂർണ്ണസംഖ്യ positive, RES_TYPE (func)(Parser *), Parser *p)       \
    {                                                               \
        പൂർണ്ണസംഖ്യ mark = p->mark;                                         \
        ശൂന്യം *res = func(p);                                        \
        p->mark = mark;                                             \
        തിരികെ (res != ശൂന്യ) == positive;                           \
    }

LOOKAHEAD1(_PyPegen_lookahead, ശൂന്യം *)
LOOKAHEAD1(Py_Pegen_lookahead_for_വ്യഞ്ജകം, വ്യഞ്ജകം_ty)
LOOKAHEAD1(_PyPegen_lookahead_for_stmt, stmt_ty)
#undef LOOKAHEAD1

#define LOOKAHEAD2(NAME, RES_TYPE, T)                                   \
    പൂർണ്ണസംഖ്യ                                                                 \
    NAME (പൂർണ്ണസംഖ്യ positive, RES_TYPE (func)(Parser *, T), Parser *p, T arg) \
    {                                                                   \
        പൂർണ്ണസംഖ്യ mark = p->mark;                                             \
        ശൂന്യം *res = func(p, arg);                                       \
        p->mark = mark;                                                 \
        തിരികെ (res != ശൂന്യ) == positive;                               \
    }

LOOKAHEAD2(_PyPegen_lookahead_with_int, Token *, പൂർണ്ണസംഖ്യ)
LOOKAHEAD2(_PyPegen_lookahead_with_string, വ്യഞ്ജകം_ty, സ്ഥിരം അക്ഷരം *)
#undef LOOKAHEAD2

Token *
_PyPegen_expect_token(Parser *p, പൂർണ്ണസംഖ്യ type)
{
    എങ്കിൽ (p->mark == p->fill) {
        എങ്കിൽ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            തിരികെ ശൂന്യ;
        }
    }
    Token *t = p->tokens[p->mark];
    എങ്കിൽ (t->type != type) {
       തിരികെ ശൂന്യ;
    }
    p->mark += 1;
    തിരികെ t;
}

ശൂന്യം*
_PyPegen_expect_forced_result(Parser *p, ശൂന്യം* result, സ്ഥിരം അക്ഷരം* expected) {

    എങ്കിൽ (p->error_indicator == 1) {
        തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (result == ശൂന്യ) {
        RAISE_SYNTAX_ERROR("expected (%s)", expected);
        തിരികെ ശൂന്യ;
    }
    തിരികെ result;
}

Token *
_PyPegen_expect_forced_token(Parser *p, പൂർണ്ണസംഖ്യ type, സ്ഥിരം അക്ഷരം* expected) {

    എങ്കിൽ (p->error_indicator == 1) {
        തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (p->mark == p->fill) {
        എങ്കിൽ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            തിരികെ ശൂന്യ;
        }
    }
    Token *t = p->tokens[p->mark];
    എങ്കിൽ (t->type != type) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(t, "expected '%s'", expected);
        തിരികെ ശൂന്യ;
    }
    p->mark += 1;
    തിരികെ t;
}

വ്യഞ്ജകം_ty
_PyPegen_expect_soft_keyword(Parser *p, സ്ഥിരം അക്ഷരം *keyword)
{
    എങ്കിൽ (p->mark == p->fill) {
        എങ്കിൽ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            തിരികെ ശൂന്യ;
        }
    }
    Token *t = p->tokens[p->mark];
    എങ്കിൽ (t->type != NAME) {
        തിരികെ ശൂന്യ;
    }
    സ്ഥിരം അക്ഷരം *s = PyBytes_AsString(t->bytes);
    എങ്കിൽ (!s) {
        p->error_indicator = 1;
        തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (strcmp(s, keyword) != 0) {
        തിരികെ ശൂന്യ;
    }
    തിരികെ _PyPegen_name_token(p);
}

Token *
_PyPegen_get_last_nonnwhitespace_token(Parser *p)
{
    പ്രതിജ്ഞാ(p->mark >= 0);
    Token *token = ശൂന്യ;
    വേണ്ടി (പൂർണ്ണസംഖ്യ m = p->mark - 1; m >= 0; m--) {
        token = p->tokens[m];
        എങ്കിൽ (token->type != ENDMARKER && (token->type < NEWLINE || token->type > DEDENT)) {
            നിർത്തുക;
        }
    }
    തിരികെ token;
}

PyObject *
_PyPegen_new_identifier(Parser *p, സ്ഥിരം അക്ഷരം *n)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, (Py_ssize_t)strlen(n), ശൂന്യ);
    എങ്കിൽ (!id) {
        പോകുക error;
    }
    /* Check whether there are non-ASCII characters in the
       identifier; if so, normalize to NFKC. */
    എങ്കിൽ (!PyUnicode_IS_ASCII(id))
    {
        എങ്കിൽ (!init_normalization(p))
        {
            Py_DECREF(id);
            പോകുക error;
        }
        PyObject *form = PyUnicode_InternFromString("NFKC");
        എങ്കിൽ (form == ശൂന്യ)
        {
            Py_DECREF(id);
            പോകുക error;
        }
        PyObject *args[2] = {form, id};
        PyObject *id2 = PyObject_Vectorcall(p->normalize, args, 2, ശൂന്യ);
        Py_DECREF(id);
        Py_DECREF(form);
        എങ്കിൽ (!id2) {
            പോകുക error;
        }

        എങ്കിൽ (!PyUnicode_Check(id2))
        {
            PyErr_Format(PyExc_TypeError,
                         "unicodedata.normalize() must return a string, not "
                         "%.200s",
                         _PyType_Name(Py_TYPE(id2)));
            Py_DECREF(id2);
            പോകുക error;
        }
        id = id2;
    }
    സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം forbidden[] = {
        "None",
        "True",
        "False",
        ശൂന്യ
    };
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; forbidden[i] != ശൂന്യ; i++) {
        എങ്കിൽ (_PyUnicode_EqualToASCIIString(id, forbidden[i])) {
            PyErr_Format(Py_Exc_മൂല്യം_Error,
                         "identifier field can't represent '%s' constant",
                         forbidden[i]);
            Py_DECREF(id);
            പോകുക error;
        }
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &id);
    എങ്കിൽ (_PyArena_AddPyObject(p->arena, id) < 0)
    {
        Py_DECREF(id);
        പോകുക error;
    }
    തിരികെ id;

error:
    p->error_indicator = 1;
    തിരികെ ശൂന്യ;
}

// Return an arena-owned identifier; callers borrow the reference.
സ്ഥിതം PyObject *
get_cached_identifier(Parser *p, PyObject *bytes)
{
    സ്ഥിരം അക്ഷരം *s = PyBytes_AsString(bytes);
    എങ്കിൽ (!s) {
        p->error_indicator = 1;
        തിരികെ ശൂന്യ;
    }
    // Identifiers repeat constantly; a small span-keyed cache skips the
    // UTF-8 decode + intern for repeated occurrences. Keys point into
    // arena-owned token bytes and values are arena-owned interned strings,
    // so borrowed references are valid for the lifetime of the parse
    // (including the second error pass, which reuses parser and arena).
    Py_ssize_t len = PyBytes_GET_SIZE(bytes);
    Py_hash_t hash = PyObject_Hash(bytes);
    എങ്കിൽ (hash == -1) {
        p->error_indicator = 1;
        തിരികെ ശൂന്യ;
    }
    // Parses without identifiers do not need this cache.
    എങ്കിൽ (p->identifier_cache == ശൂന്യ) {
        p->identifier_cache = PyMem_Calloc(
            IDENTIFIER_CACHE_SIZE, വലുപ്പം(*p->identifier_cache));
        എങ്കിൽ (p->identifier_cache == ശൂന്യ) {
            p->error_indicator = 1;
            തിരികെ PyErr_NoMemory();
        }
    }
    IdentifierCacheEntry *free_slot = ശൂന്യ;
    size_t idx = (size_t)hash & (IDENTIFIER_CACHE_SIZE - 1);
    വേണ്ടി (പൂർണ്ണസംഖ്യ probe = 0; probe < IDENTIFIER_CACHE_MAX_PROBES; probe++) {
        IdentifierCacheEntry *entry = &p->identifier_cache[
            (idx + probe) & (IDENTIFIER_CACHE_SIZE - 1)];
        എങ്കിൽ (entry->താക്കോൽ == ശൂന്യ) {
            free_slot = entry;
            നിർത്തുക;
        }
        എങ്കിൽ (entry->hash == hash && entry->len == len &&
            memcmp(entry->താക്കോൽ, s, len) == 0)
        {
            തിരികെ entry->മൂല്യം;
        }
    }
    PyObject *id = _PyPegen_new_identifier(p, s);
    എങ്കിൽ (id == ശൂന്യ) {
        p->error_indicator = 1;
        തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (free_slot != ശൂന്യ) {
        free_slot->താക്കോൽ = s;
        free_slot->len = len;
        free_slot->hash = hash;
        free_slot->മൂല്യം = id;
    }
    തിരികെ id;
}

സ്ഥിതം വ്യഞ്ജകം_ty
_PyPegen_name_from_token(Parser *p, Token* t)
{
    എങ്കിൽ (t == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }
    // Reuse the AST node when backtracking revisits this token. Memo lookup
    // starts before the token and restores the position after it on a hit.
    // Token kinds can be memo keys: generated grammar rule IDs start at 1000.
    പൂർണ്ണസംഖ്യ mark = p->mark - 1;
    p->mark = mark;
    വ്യഞ്ജകം_ty cached = ശൂന്യ;
    എങ്കിൽ (_PyPegen_is_memoized(p, NAME, &cached)) {
        തിരികെ cached;
    }
    p->mark = mark + 1;
    PyObject *id = get_cached_identifier(p, t->bytes);
    എങ്കിൽ (id == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }
    വ്യഞ്ജകം_ty result = _PyAST_Name(id, Load, t->lineno, t->col_offset,
                                t->end_lineno, t->end_col_offset, p->arena);
    എങ്കിൽ (result != ശൂന്യ && Py_Pegen_തിരുകുക_memo(p, mark, NAME, result) < 0) {
        p->error_indicator = 1;
        തിരികെ ശൂന്യ;
    }
    തിരികെ result;
}

വ്യഞ്ജകം_ty
_PyPegen_name_token(Parser *p)
{
    Token *t = _PyPegen_expect_token(p, NAME);
    തിരികെ _PyPegen_name_from_token(p, t);
}

ശൂന്യം *
_PyPegen_string_token(Parser *p)
{
    തിരികെ _PyPegen_expect_token(p, STRING);
}

വ്യഞ്ജകം_ty _PyPegen_soft_keyword_token(Parser *p) {
    Token *t = _PyPegen_expect_token(p, NAME);
    എങ്കിൽ (t == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }
    അക്ഷരം *the_token;
    Py_ssize_t size;
    PyBytes_AsStringAndSize(t->bytes, &the_token, &size);
    വേണ്ടി (അക്ഷരം **keyword = p->soft_keywords; *keyword != ശൂന്യ; keyword++) {
        എങ്കിൽ (strlen(*keyword) == (size_t)size &&
            strncmp(*keyword, the_token, (size_t)size) == 0) {
            തിരികെ _PyPegen_name_from_token(p, t);
        }
    }
    തിരികെ ശൂന്യ;
}

സ്ഥിതം PyObject *
parsenumber_raw(സ്ഥിരം അക്ഷരം *s)
{
    സ്ഥിരം അക്ഷരം *end;
    long x;
    ഇരട്ട dx;
    Py_complex compl;
    പൂർണ്ണസംഖ്യ imflag;

    പ്രതിജ്ഞാ(s != ശൂന്യ);
    errno = 0;
    end = s + strlen(s) - 1;
    imflag = *end == 'j' || *end == 'J';
    എങ്കിൽ (s[0] == '0') {
        x = (long)PyOS_strtoul(s, (അക്ഷരം **)&end, 0);
        എങ്കിൽ (x < 0 && errno == 0) {
            തിരികെ PyLong_FromString(s, (അക്ഷരം **)0, 0);
        }
    }
    അല്ലെങ്കിൽ {
        x = PyOS_strtol(s, (അക്ഷരം **)&end, 0);
    }
    എങ്കിൽ (*end == '\0') {
        എങ്കിൽ (errno != 0) {
            തിരികെ PyLong_FromString(s, (അക്ഷരം **)0, 0);
        }
        തിരികെ PyLong_FromLong(x);
    }
    /* XXX Huge floats may silently fail */
    എങ്കിൽ (imflag) {
        compl.real = 0.;
        compl.imag = PyOS_string_to_double(s, (അക്ഷരം **)&end, ശൂന്യ);
        എങ്കിൽ (compl.imag == -1.0 && PyErr_Occurred()) {
            തിരികെ ശൂന്യ;
        }
        തിരികെ PyComplex_FromCComplex(compl);
    }
    dx = PyOS_string_to_double(s, ശൂന്യ, ശൂന്യ);
    എങ്കിൽ (dx == -1.0 && PyErr_Occurred()) {
        തിരികെ ശൂന്യ;
    }
    തിരികെ PyFloat_FromDouble(dx);
}

സ്ഥിതം PyObject *
parsenumber(സ്ഥിരം അക്ഷരം *s)
{
    അക്ഷരം *dup;
    അക്ഷരം *end;
    PyObject *res = ശൂന്യ;

    പ്രതിജ്ഞാ(s != ശൂന്യ);

    എങ്കിൽ (strchr(s, '_') == ശൂന്യ) {
        തിരികെ parsenumber_raw(s);
    }
    /* Create a duplicate without underscores. */
    dup = PyMem_Malloc(strlen(s) + 1);
    എങ്കിൽ (dup == ശൂന്യ) {
        തിരികെ PyErr_NoMemory();
    }
    end = dup;
    വേണ്ടി (; *s; s++) {
        എങ്കിൽ (*s != '_') {
            *end++ = *s;
        }
    }
    *end = '\0';
    res = parsenumber_raw(dup);
    PyMem_Free(dup);
    തിരികെ res;
}

വ്യഞ്ജകം_ty
Py_Pegen_സംഖ്യ_token(Parser *p)
{
    പൂർണ്ണസംഖ്യ mark = p->mark;
    Token *t = _PyPegen_expect_token(p, സംഖ്യ);
    എങ്കിൽ (t == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }

    // Look up the node at the token's start, as for NAME tokens. A memo hit
    // advances the mark; on a miss, restore the position after the token.
    p->mark = mark;
    വ്യഞ്ജകം_ty cached = ശൂന്യ;
    എങ്കിൽ (_PyPegen_is_memoized(p, സംഖ്യ, &cached)) {
        തിരികെ cached;
    }
    p->mark = mark + 1;

    സ്ഥിരം അക്ഷരം *അക്കം_raw = PyBytes_AsString(t->bytes);
    എങ്കിൽ (അക്കം_raw == ശൂന്യ) {
        p->error_indicator = 1;
        തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (p->feature_version < 6 && strchr(അക്കം_raw, '_') != ശൂന്യ) {
        p->error_indicator = 1;
        തിരികെ RAISE_SYNTAX_ERROR("Underscores in numeric literals are only supported "
                                  "in Python 3.6 and greater");
    }

    PyObject *c = parsenumber(അക്കം_raw);

    എങ്കിൽ (c == ശൂന്യ) {
        p->error_indicator = 1;
        PyThreadState *tstate = _PyThreadState_GET();
        // The only way a ValueError should happen in _this_ code is via
        // PyLong_FromString hitting a length limit.
        എങ്കിൽ (tstate->current_exception != ശൂന്യ &&
            Py_TYPE(tstate->current_exception) == (PyTypeObject *)Py_Exc_മൂല്യം_Error
        ) {
            PyObject *exc = PyErr_GetRaisedException();
            /* Intentionally omitting columns to avoid a wall of 1000s of '^'s
             * on the error message. Nobody is going to overlook their huge
             * numeric literal once given the line. */
            RAISE_ERROR_KNOWN_LOCATION(
                p, PyExc_SyntaxError,
                t->lineno, -1 /* col_offset */,
                t->end_lineno, -1 /* end_col_offset */,
                "%S - Consider hexadecimal for huge integer literals "
                "to avoid decimal conversion limits.",
                exc);
            Py_DECREF(exc);
        }
        തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (_PyArena_AddPyObject(p->arena, c) < 0) {
        Py_DECREF(c);
        p->error_indicator = 1;
        തിരികെ ശൂന്യ;
    }

    വ്യഞ്ജകം_ty result = _PyAST_Constant(c, ശൂന്യ, t->lineno, t->col_offset,
                                    t->end_lineno, t->end_col_offset, p->arena);
    എങ്കിൽ (result != ശൂന്യ && Py_Pegen_തിരുകുക_memo(p, mark, സംഖ്യ, result) < 0) {
        p->error_indicator = 1;
        തിരികെ ശൂന്യ;
    }
    തിരികെ result;
}



സ്ഥിതം പൂർണ്ണസംഖ്യ
compute_parser_flags(PyCompilerFlags *flags)
{
    പൂർണ്ണസംഖ്യ parser_flags = 0;
    എങ്കിൽ (!flags) {
        തിരികെ 0;
    }
    എങ്കിൽ (flags->cf_flags & PyCF_DONT_IMPLY_DEDENT) {
        parser_flags |= PyPARSE_DONT_IMPLY_DEDENT;
    }
    എങ്കിൽ (flags->cf_flags & PyCF_IGNORE_COOKIE) {
        parser_flags |= PyPARSE_IGNORE_COOKIE;
    }
    എങ്കിൽ (flags->cf_flags & CO_FUTURE_BARRY_AS_BDFL) {
        parser_flags |= PyPARSE_BARRY_AS_BDFL;
    }
    എങ്കിൽ (flags->cf_flags & PyCF_TYPE_COMMENTS) {
        parser_flags |= PyPARSE_TYPE_COMMENTS;
    }
    എങ്കിൽ (flags->cf_flags & Py_CF_ALLOW_INCOMPLETE_നിവേശം) {
        parser_flags |= Py_PARSE_ALLOW_INCOMPLETE_നിവേശം;
    }
    തിരികെ parser_flags;
}

// Parser API

Parser *
_PyPegen_Parser_New(ഘടന tok_state *tok, പൂർണ്ണസംഖ്യ start_rule, പൂർണ്ണസംഖ്യ flags,
                    പൂർണ്ണസംഖ്യ feature_version, പൂർണ്ണസംഖ്യ *errcode, സ്ഥിരം അക്ഷരം* source, PyArena *arena)
{
    Parser *p = PyMem_Malloc(വലുപ്പം(Parser));
    എങ്കിൽ (p == ശൂന്യ) {
        തിരികെ (Parser *) PyErr_NoMemory();
    }
    പ്രതിജ്ഞാ(tok != ശൂന്യ);
    _PyTokenizer_SetOptions(tok, 0, (flags & PyPARSE_TYPE_COMMENTS) > 0);
    Py_Tokenizer_Allow_Incomplete_നിവേശം(tok, flags & Py_PARSE_ALLOW_INCOMPLETE_നിവേശം);
    p->tok = tok;
    p->keywords = ശൂന്യ;
    p->n_keyword_lists = -1;
    p->soft_keywords = ശൂന്യ;
    p->tokens = PyMem_Malloc(വലുപ്പം(Token *));
    എങ്കിൽ (!p->tokens) {
        PyMem_Free(p);
        തിരികെ (Parser *) PyErr_NoMemory();
    }
    p->tokens[0] = PyMem_Calloc(1, വലുപ്പം(Token));
    എങ്കിൽ (!p->tokens[0]) {
        PyMem_Free(p->tokens);
        PyMem_Free(p);
        തിരികെ (Parser *) PyErr_NoMemory();
    }
    p->type_ignore_comments = (growable_comment_array){0};

    p->mark = 0;
    p->fill = 0;
    p->size = 1;

    p->errcode = errcode;
    p->arena = arena;
    p->start_rule = start_rule;
    p->parsing_started = 0;
    p->normalize = ശൂന്യ;
    p->error_indicator = 0;

    p->starting_lineno = 0;
    p->starting_col_offset = 0;
    p->flags = flags;
    p->feature_version = feature_version;
    p->known_err_token = ശൂന്യ;
    p->identifier_cache = ശൂന്യ;
    p->tstate = PyThreadState_Get();
    // Stack limits are initialized when the thread state is attached.
    പ്രതിജ്ഞാ(((_PyThreadStateImpl *)p->tstate)->c_അട്ടി_hard_limit != 0);
    p->അട്ടി_soft_limit = ((_PyThreadStateImpl *)p->tstate)->c_അട്ടി_soft_limit;
#എങ്കിൽ Py_അട്ടി_GROWS_DOWN
    p->അട്ടി_soft_limit += Py_OS_അട്ടി_MARGIN_BYTES;
#അല്ലെങ്കിൽ
    p->അട്ടി_soft_limit -= Py_OS_അട്ടി_MARGIN_BYTES;
#endif
    p->level = 0;
    p->call_invalid_rules = 0;
    p->നിവേശം_exhausted = 0;
    p->last_stmt_location.lineno = 0;
    p->last_stmt_location.col_offset = 0;
    p->last_stmt_location.end_lineno = 0;
    p->last_stmt_location.end_col_offset = 0;
#ifdef Py_DEBUG
    p->debug = _Py_GetConfig()->parser_debug;
#endif
    തിരികെ p;
}

ശൂന്യം
_PyPegen_Parser_Free(Parser *p)
{
    PyMem_Free(p->identifier_cache);
    Py_XDECREF(p->normalize);
    // Resizes allocate blocks starting at indices 1, 2, 4, and so on.
    PyMem_Free(p->tokens[0]);
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < p->size; i *= PEGEN_ARRAY_GROWTH_FACTOR) {
        PyMem_Free(p->tokens[i]);
    }
    PyMem_Free(p->tokens);
    growable_comment_array_deallocate(&p->type_ignore_comments);
    PyMem_Free(p);
}

സ്ഥിതം ശൂന്യം
reset_parser_state_for_error_pass(Parser *p)
{
    p->last_stmt_location.lineno = 0;
    p->last_stmt_location.col_offset = 0;
    p->last_stmt_location.end_lineno = 0;
    p->last_stmt_location.end_col_offset = 0;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < p->fill; i++) {
        p->tokens[i]->memo = ശൂന്യ;
        p->tokens[i]->memo_mask = 0;
    }
    p->mark = 0;
    p->flags &= ~Py_PARSE_ALLOW_INCOMPLETE_നിവേശം;
    Py_Tokenizer_Allow_Incomplete_നിവേശം(p->tok, 0);
    p->call_invalid_rules = 1;
    _PyTokenizer_StopInteractive(p->tok);
}

സ്ഥിതം ശൂന്യം
_PyPegen_set_syntax_error_metadata(Parser *p) {
    PyObject *exc = PyErr_GetRaisedException();
    എങ്കിൽ (!exc || !PyObject_TypeCheck(exc, (PyTypeObject *)PyExc_SyntaxError)) {
        PyErr_SetRaisedException(exc);
        തിരികെ;
    }
    സ്ഥിരം അക്ഷരം *source = _PyTokenizer_RetainedSource(p->tok);
    സ്ഥിരം അക്ഷരം *encoding = _PyTokenizer_GetInfo(p->tok).encoding;
    PyObject* the_source = ശൂന്യ;
    എങ്കിൽ (source) {
        എങ്കിൽ (encoding == ശൂന്യ) {
            the_source = PyUnicode_FromString(source);
        } അല്ലെങ്കിൽ {
            the_source = PyUnicode_Decode(source, strlen(source), encoding, ശൂന്യ);
        }
    }
    എങ്കിൽ (!the_source) {
        PyErr_Clear();
        the_source = Py_None;
        Py_INCREF(the_source);
    }
    PyObject* metadata = Py_Build_മൂല്യം(
        "(iiN)",
        p->last_stmt_location.lineno,
        p->last_stmt_location.col_offset,
        the_source // N gives ownership to metadata
    );
    എങ്കിൽ (!metadata) {
        PyErr_Clear();
        PyErr_SetRaisedException(exc);
        തിരികെ;
    }
    PySyntaxErrorObject *syntax_error = (PySyntaxErrorObject *)exc;

    Py_XDECREF(syntax_error->metadata);
    syntax_error->metadata = metadata;
    PyErr_SetRaisedException(exc);
}

ശൂന്യം *
_PyPegen_run_parser(Parser *p)
{
    ശൂന്യം *res = _PyPegen_parse(p);
    പ്രതിജ്ഞാ(p->level == 0);
    എങ്കിൽ (res != ശൂന്യ && PyErr_Occurred()) {
        // Discard a result returned with an exception still pending
        // (e.g. a MemoryError from a recovered-from allocation failure).
        തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (res == ശൂന്യ) {
        എങ്കിൽ (PyErr_Occurred() &&
            (PyErr_ExceptionMatches(Py_Exc_Incomplete_നിവേശം_Error) ||
             !PyErr_ExceptionMatches(PyExc_SyntaxError))) {
            തിരികെ ശൂന്യ;
        }
        എങ്കിൽ ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_നിവേശം) &&
            p->നിവേശം_exhausted && !PyErr_Occurred()) {
            തിരികെ Py_Pegen_raise_incomplete_നിവേശം(p);
        }
       // Make a second parser pass. In this pass we activate heavier and slower checks
        // to produce better error messages and more complete diagnostics. Extra "invalid_*"
        // rules will be active during parsing.
        Token *last_token = p->tokens[p->fill - 1];
        reset_parser_state_for_error_pass(p);
        _PyPegen_parse(p);

        // Set SyntaxErrors accordingly depending on the parser/tokenizer status at the failure
        // point.
        _Pypegen_set_syntax_error(p, last_token);

        // Set the metadata in the exception from p->last_stmt_location
        എങ്കിൽ (PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_set_syntax_error_metadata(p);
        }
       തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (p->start_rule == Py_single_നിവേശം && _PyTokenizer_HasTrailingStatement(p->tok)) {
        തിരികെ RAISE_SYNTAX_ERROR("multiple statements found while compiling a single statement");
    }

    // test_peg_generator defines _Py_TEST_PEGEN to not call PyAST_Validate()
#എങ്കിൽ defined(Py_DEBUG) && !defined(_Py_TEST_PEGEN)
    എങ്കിൽ (p->start_rule == Py_single_നിവേശം ||
        p->start_rule == Py_file_നിവേശം ||
        p->start_rule == Py_eval_നിവേശം)
    {
        എങ്കിൽ (!_PyAST_Validate(res)) {
            തിരികെ ശൂന്യ;
        }
    }
#endif
    തിരികെ res;
}

mod_ty
_PyPegen_run_parser_from_file_pointer(FILE *fp, പൂർണ്ണസംഖ്യ start_rule, PyObject *filename_ob,
                             സ്ഥിരം അക്ഷരം *enc, സ്ഥിരം അക്ഷരം *ps1, സ്ഥിരം അക്ഷരം *ps2,
                             PyCompilerFlags *flags, പൂർണ്ണസംഖ്യ *errcode,
                             PyObject **interactive_src, PyArena *arena)
{
    പൂർണ്ണസംഖ്യ parser_flags = compute_parser_flags(flags);
    ഘടന tok_state *tok = _PyTokenizer_FromFile(fp, enc, ps1, ps2);
    എങ്കിൽ (tok == ശൂന്യ) {
        എങ്കിൽ (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        അല്ലെങ്കിൽ {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        തിരികെ ശൂന്യ;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    // From here on we need to clean up even if there's an error
    mod_ty result = ശൂന്യ;

    PyObject *module = PyUnicode_FromString("__main__");
    എങ്കിൽ (module == ശൂന്യ) {
        പോകുക error;
    }
    _PyTokenizer_SetContext(tok, filename_ob, module);
    Py_DECREF(module);

    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, PY_MINOR_VERSION,
                                    errcode, ശൂന്യ, arena);
    എങ്കിൽ (p == ശൂന്യ) {
        പോകുക error;
    }

    result = _PyPegen_run_parser(p);
    _PyPegen_Parser_Free(p);

    സ്ഥിരം അക്ഷരം *source = _PyTokenizer_RetainedSource(tok);
    എങ്കിൽ (source != ശൂന്യ && result && interactive_src != ശൂന്യ) {
        *interactive_src = PyUnicode_FromString(source);
        എങ്കിൽ (!*interactive_src || _PyArena_AddPyObject(arena, *interactive_src) < 0) {
            Py_XDECREF(*interactive_src);
            result = ശൂന്യ;
            പോകുക error;
        }
    }

error:
    _PyTokenizer_Free(tok);
    തിരികെ result;
}

mod_ty
_PyPegen_run_parser_from_string(സ്ഥിരം അക്ഷരം *str, പൂർണ്ണസംഖ്യ start_rule, PyObject *filename_ob,
                       PyCompilerFlags *flags, PyArena *arena, PyObject *module,
                       PyObject **incomplete)
{
    _PyTokenizer_Newline newline_mode = start_rule == Py_file_നിവേശം
        ? _PyTokenizer_IMPLY_NEWLINE : _PyTokenizer_KEEP_NEWLINE;
    പൂർണ്ണസംഖ്യ parser_flags = compute_parser_flags(flags);
    എങ്കിൽ (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_നിവേശം) {
        newline_mode = _PyTokenizer_APPEND_NEWLINE;
    }

    ഘടന tok_state *tok;
    എങ്കിൽ (flags != ശൂന്യ && flags->cf_flags & PyCF_IGNORE_COOKIE) {
        tok = _PyTokenizer_FromUTF8(str, newline_mode, 0);
    } അല്ലെങ്കിൽ {
        tok = _PyTokenizer_FromString(str, newline_mode, 0);
    }
    എങ്കിൽ (tok == ശൂന്യ) {
        എങ്കിൽ (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        അല്ലെങ്കിൽ {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        തിരികെ ശൂന്യ;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    _PyTokenizer_SetContext(tok, filename_ob, module);

    // We need to clear up from here on
    mod_ty result = ശൂന്യ;

    പൂർണ്ണസംഖ്യ feature_version = flags && (flags->cf_flags & PyCF_ONLY_AST) ?
        flags->cf_feature_version : PY_MINOR_VERSION;
    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, feature_version,
                                    ശൂന്യ, str, arena);
    എങ്കിൽ (p == ശൂന്യ) {
        പോകുക error;
    }

    result = _PyPegen_run_parser(p);
    എങ്കിൽ (result != ശൂന്യ && (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_നിവേശം)) {
        പൂർണ്ണസംഖ്യ needs_newline = _PyTokenizer_GetNewlineRequirements(tok);
        എങ്കിൽ ((needs_newline & _PyTokenizer_NEWLINE_CONTINUATION) ||
            (start_rule != Py_file_നിവേശം &&
             (needs_newline & _PyTokenizer_NEWLINE_INDENT)) ||
            (start_rule == Py_single_നിവേശം && p->നിവേശം_exhausted &&
             (needs_newline & _PyTokenizer_NEWLINE_STATEMENT))) {
            Py_Pegen_raise_incomplete_നിവേശം(p);
            എങ്കിൽ (incomplete != ശൂന്യ && PyErr_ExceptionMatches(Py_Exc_Incomplete_നിവേശം_Error)) {
                *incomplete = PyErr_GetRaisedException();
            }
            അല്ലെങ്കിൽ {
                result = ശൂന്യ;
            }
        }
    }
    _PyPegen_Parser_Free(p);

error:
    _PyTokenizer_Free(tok);
    തിരികെ result;
}
