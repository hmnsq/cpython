#include <Python.h>
#include "pycore_ast.h"           // _PyAST_Validate(),
#include "pycore_pystate.h"       // _PyThreadState_GET()
#include "pycore_parser.h"        // _PYPEGEN_NSTATISTICS
#include "pycore_pyerrors.h"      // PyExc_IncompleteInputError
#include "pycore_runtime.h"       // _PyRuntime
#include "pycore_unicodeobject.h" // _PyUnicode_InternImmortal
#include <errcode.h>

#include "lexer/state.h"
#include "tokenizer/tokenizer.h"
#include "pegen.h"

#define IDENTIFIER_CACHE_SIZE 2048  // Must be a power of two.
#define IDENTIFIER_CACHE_MAX_PROBES 8
#define PEGEN_ARRAY_GROWTH_FACTOR 2
#define TYPE_IGNORE_INITIAL_CAPACITY 10

sa_nracanaa _identifier_cache_entry {
    const var_na *key;   // Borrowed from arena-owned token bytes.
    Py_ssize_t len;
    Py_hash_t hash;
    PyObject *value;   // Borrowed from an arena-owned identifier.
};

// Internal parser functions

asdl_stmt_seq*
_PyPegen_interactive_exit(Parser *p)
{
    ถ้า (p->errcode) {
        *(p->errcode) = E_EOF;
    }
    คืน NULL;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset_line(PyObject *line, Py_ssize_t col_offset, Py_ssize_t end_col_offset)
{
    const unsigned var_na *data = (const unsigned var_na*)PyUnicode_AsUTF8(line);
    ถ้า (data == NULL) {
        คืน -1;
    }

    Py_ssize_t len = 0;
    ขณะ (col_offset < end_col_offset) {
        Py_UCS4 ch = data[col_offset];
        ถ้า (ch < 0x80) {
            col_offset += 1;
        } มิฉะนั้น ถ้า ((ch & 0xe0) == 0xc0) {
            col_offset += 2;
        } มิฉะนั้น ถ้า ((ch & 0xf0) == 0xe0) {
            col_offset += 3;
        } มิฉะนั้น ถ้า ((ch & 0xf8) == 0xf0) {
            col_offset += 4;
        } มิฉะนั้น {
            PyErr_SetString(PyExc_ValueError, "Invalid UTF-8 sequence");
            คืน -1;
        }
        len++;
    }
    คืน len;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset_raw(const var_na* str, Py_ssize_t col_offset)
{
    Py_ssize_t len = (Py_ssize_t)strlen(str);
    ถ้า (col_offset > len + 1) {
        col_offset = len + 1;
    }
    assert(col_offset >= 0);
    PyObject *text = PyUnicode_DecodeUTF8(str, col_offset, "replace");
    ถ้า (!text) {
        คืน -1;
    }
    Py_ssize_t size = PyUnicode_GET_LENGTH(text);
    Py_DECREF(text);
    คืน size;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset(PyObject *line, Py_ssize_t col_offset)
{
    const var_na *str = PyUnicode_AsUTF8(line);
    ถ้า (!str) {
        คืน -1;
    }
    คืน _PyPegen_byte_offset_to_character_offset_raw(str, col_offset);
}

// Here, mark is the start of the node, while p->mark is the end.
// If node==NULL, they should be the same.
จำนวนเต็ม
_PyPegen_insert_memo(Parser *p, จำนวนเต็ม mark, จำนวนเต็ม type, void *node)
{
    // Insert in front
    Memo *m = _PyArena_Malloc(p->arena, parimaa_na(Memo));
    ถ้า (m == NULL) {
        คืน -1;
    }
    m->type = type;
    m->node = node;
    m->mark = p->mark;
    m->next = p->tokens[mark]->memo;
    p->tokens[mark]->memo = m;
    p->tokens[mark]->memo_mask |= 1ULL << (type & 63);
    คืน 0;
}

// Like _PyPegen_insert_memo(), but returns the inserted Memo so callers
// can update it in place without re-walking the token's memo list.
Memo *
_PyPegen_insert_memo_direct(Parser *p, จำนวนเต็ม mark, จำนวนเต็ม type)
{
    ถ้า (_PyPegen_insert_memo(p, mark, type, NULL) < 0) {
        คืน NULL;
    }
    คืน p->tokens[mark]->memo;
}

// Like _PyPegen_insert_memo(), but updates an existing node if found.
จำนวนเต็ม
_PyPegen_update_memo(Parser *p, จำนวนเต็ม mark, จำนวนเต็ม type, void *node)
{
    สำหรับ (Memo *m = p->tokens[mark]->memo; m != NULL; m = m->next) {
        ถ้า (m->type == type) {
            // Update existing node.
            m->node = node;
            m->mark = p->mark;
            คืน 0;
        }
    }
    // Insert new node.
    คืน _PyPegen_insert_memo(p, mark, type, node);
}

static จำนวนเต็ม
init_normalization(Parser *p)
{
    ถ้า (p->normalize) {
        คืน 1;
    }
    p->normalize = PyImport_ImportModuleAttrString("unicodedata", "normalize");
    ถ้า (!p->normalize)
    {
        คืน 0;
    }
    คืน 1;
}

จำนวนเต็ม
_PyPegen_grow_loop_buffer(void ***buffer, Py_ssize_t *capacity)
{
    ถ้า ((size_t)*capacity > (size_t)PY_SSIZE_T_MAX /
            (PEGEN_ARRAY_GROWTH_FACTOR * parimaa_na(**buffer))) {
        คืน -1;
    }
    Py_ssize_t new_capacity = *capacity == 0
        ? 1 : *capacity * PEGEN_ARRAY_GROWTH_FACTOR;
    void **new_buffer = PyMem_Realloc(*buffer, new_capacity * parimaa_na(**buffer));
    ถ้า (new_buffer == NULL) {
        คืน -1;
    }
    *buffer = new_buffer;
    *capacity = new_capacity;
    คืน 0;
}

static จำนวนเต็ม
growable_comment_array_add(growable_comment_array *arr, จำนวนเต็ม lineno, var_na *comment)
{
    ถ้า (arr->num_items >= arr->size) {
        ถ้า (arr->size > (size_t)PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * parimaa_na(*arr->items))) {
            คืน 0;
        }
        size_t new_size = arr->size == 0
            ? TYPE_IGNORE_INITIAL_CAPACITY
            : arr->size * PEGEN_ARRAY_GROWTH_FACTOR;
        void *new_items_array = PyMem_Realloc(
            arr->items, new_size * parimaa_na(*arr->items));
        ถ้า (!new_items_array) {
            คืน 0;
        }
        arr->items = new_items_array;
        arr->size = new_size;
    }

    arr->items[arr->num_items].lineno = lineno;
    arr->items[arr->num_items].comment = comment;  // Take ownership
    arr->num_items++;
    คืน 1;
}

static void
growable_comment_array_deallocate(growable_comment_array *arr)
{
    สำหรับ (size_t i = 0; i < arr->num_items; i++) {
        PyMem_Free(arr->items[i].comment);
    }
    PyMem_Free(arr->items);
}

static จำนวนเต็ม
_get_keyword_or_name_type(Parser *p, const var_na *text, Py_ssize_t length)
{
    assert(length > 0);

    ถ้า (length >= p->n_keyword_lists ||
        p->keywords[length] == NULL ||
        p->keywords[length]->type == -1) {
        คืน NAME;
    }
    สำหรับ (KeywordToken *k = p->keywords[length]; k != NULL && k->type != -1; k++) {
        ถ้า (memcmp(k->str, text, (size_t)length) == 0) {
            คืน k->type;
        }
    }
    คืน NAME;
}

// Keep text only where grammar actions or helpers read it. Keyword error
// actions use fixed spellings, so keywords do not need their text.
static anta_hstha จำนวนเต็ม
token_needs_text(จำนวนเต็ม type)
{
    switch (type) {
        pak_sa NAME:
        pak_sa NUMBER:
        pak_sa STRING:
        pak_sa FSTRING_START:
        pak_sa FSTRING_MIDDLE:
        pak_sa FSTRING_END:
        pak_sa TSTRING_START:
        pak_sa TSTRING_MIDDLE:
        pak_sa TSTRING_END:
        pak_sa TYPE_COMMENT:
        pak_sa NOTEQUAL:  // _PyPegen_check_barry_as_flufl() distinguishes != and <>.
            คืน 1;
        default:
            คืน 0;
    }
}

static จำนวนเต็ม
initialize_token(Parser *p, Token *parser_token, sa_nracanaa token *new_token, จำนวนเต็ม token_type) {
    assert(parser_token != NULL);

    parser_token->type = token_type;
    parser_token->bytes = NULL;
    ถ้า (token_needs_text(token_type) && new_token->span.start >= 0) {
        Py_ssize_t length;
        const var_na *text = _PyToken_TextView(p->tok, new_token, &length);
        ถ้า (token_type == NAME) {
            parser_token->type = _get_keyword_or_name_type(p, text, length);
        }
        // A name reclassified as a keyword does not need its text.
        ถ้า (parser_token->type == token_type) {
            parser_token->bytes = PyBytes_FromStringAndSize(text, length);
            ถ้า (parser_token->bytes == NULL) {
                คืน -1;
            }
            ถ้า (_PyArena_AddPyObject(p->arena, parser_token->bytes) < 0) {
                Py_DECREF(parser_token->bytes);
                คืน -1;
            }
        }
    }

    parser_token->metadata = NULL;
    ถ้า (new_token->metadata != NULL) {
        ถ้า (_PyArena_AddPyObject(p->arena, new_token->metadata) < 0) {
            คืน -1;
        }
        parser_token->metadata = new_token->metadata;
        new_token->metadata = NULL;
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

    ถ้า (token_type == ERRORTOKEN && _PyTokenizer_GetStatus(p->tok) == E_DECODE) {
        คืน _Pypegen_raise_decode_error(p);
    }

    คืน (token_type == ERRORTOKEN ? _Pypegen_tokenizer_error(p) : 0);
}

static จำนวนเต็ม
_resize_tokens_array(Parser *p) {
    ถ้า (p->size > INT_MAX / PEGEN_ARRAY_GROWTH_FACTOR ||
            (size_t)p->size > PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * parimaa_na(*p->tokens))) {
        PyErr_NoMemory();
        คืน -1;
    }
    จำนวนเต็ม newsize = p->size * PEGEN_ARRAY_GROWTH_FACTOR;
    Token **new_tokens = PyMem_Realloc(p->tokens, (size_t)newsize * parimaa_na(Token *));
    ถ้า (new_tokens == NULL) {
        PyErr_NoMemory();
        คืน -1;
    }
    p->tokens = new_tokens;

    // Allocate new tokens together without moving existing ones: parser rules
    // may still hold pointers into earlier blocks. Parser_Free frees each block.
    Token *chunk = PyMem_Calloc((size_t)(newsize - p->size), parimaa_na(Token));
    ถ้า (chunk == NULL) {
        PyErr_NoMemory();
        คืน -1;
    }
    สำหรับ (จำนวนเต็ม i = p->size; i < newsize; i++) {
        p->tokens[i] = &chunk[i - p->size];
    }
    p->size = newsize;
    คืน 0;
}

จำนวนเต็ม
_PyPegen_fill_token(Parser *p)
{
    sa_nracanaa token new_token;
    _PyToken_Init(&new_token);
    _PyTokenizer_Get(p->tok, &new_token);

    // Record and skip '# type: ignore' comments
    ขณะ (new_token.type == TYPE_IGNORE) {
        Py_ssize_t len;
        const var_na *text = _PyToken_TextView(p->tok, &new_token, &len);
        var_na *tag = PyMem_Malloc((size_t)len + 1);
        ถ้า (tag == NULL) {
            PyErr_NoMemory();
            goto error;
        }
        memcpy(tag, text, (size_t)len);
        tag[len] = '\0';
        // Ownership of tag passes to the growable array
        ถ้า (!growable_comment_array_add(&p->type_ignore_comments, new_token.end_loc.lineno, tag)) {
            PyMem_Free(tag);
            PyErr_NoMemory();
            goto error;
        }
        _PyTokenizer_Get(p->tok, &new_token);
    }

    จำนวนเต็ม type = new_token.type;
    // EOF can finish an ordinary token. Only a boundary token means that
    // the grammar requested input beyond the last physical line.
    ถ้า (type == ENDMARKER ||
        (type == DEDENT && (p->flags & PyPARSE_ALLOW_INCOMPLETE_INPUT) &&
         _PyTokenizer_GetStatus(p->tok) == E_EOF)) {
        p->input_exhausted = 1;
    }

    // If we have reached the end and we are in single input mode we need to insert a newline and reset the parsing
    ถ้า (p->start_rule == Py_single_input && type == ENDMARKER && p->parsing_started) {
        type = NEWLINE; /* Add an extra newline */
        p->parsing_started = 0;

        ถ้า (!(p->flags & PyPARSE_DONT_IMPLY_DEDENT)) {
            _PyTokenizer_ImplyDedents(p->tok);
        }
    }
    มิฉะนั้น {
        p->parsing_started = 1;
    }

    // Check if we are at the limit of the token array capacity and resize if needed
    ถ้า ((p->fill == p->size) && (_resize_tokens_array(p) != 0)) {
        goto error;
    }

    Token *t = p->tokens[p->fill];
    จำนวนเต็ม result = initialize_token(p, t, &new_token, type);
    _PyToken_Free(&new_token);
    คืน result;
error:
    _PyToken_Free(&new_token);
    คืน -1;
}

#ถ้า defined(Py_DEBUG)
// Instrumentation to count the effectiveness of memoization.
// The array counts the number of tokens skipped by memoization,
// indexed by type.

#define NSTATISTICS _PYPEGEN_NSTATISTICS
#define memo_statistics _PyRuntime.parser.memo_statistics

void
_PyPegen_clear_memo_statistics(void)
{
    PyMutex_Lock(&_PyRuntime.parser.mutex);
    สำหรับ (จำนวนเต็ม i = 0; i < NSTATISTICS; i++) {
        memo_statistics[i] = 0;
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
}

PyObject *
_PyPegen_get_memo_statistics(void)
{
    PyObject *ret = PyList_New(NSTATISTICS);
    ถ้า (ret == NULL) {
        คืน NULL;
    }

    PyMutex_Lock(&_PyRuntime.parser.mutex);
    สำหรับ (จำนวนเต็ม i = 0; i < NSTATISTICS; i++) {
        PyObject *value = PyLong_FromLong(memo_statistics[i]);
        ถ้า (value == NULL) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            คืน NULL;
        }
        // PyList_SetItem borrows a reference to value.
        ถ้า (PyList_SetItem(ret, i, value) < 0) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            คืน NULL;
        }
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
    คืน ret;
}
#endif

จำนวนเต็ม  // bool
_PyPegen_is_memoized(Parser *p, จำนวนเต็ม type, void *pres)
{
    ถ้า (p->mark == p->fill) {
        ถ้า (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            คืน -1;
        }
    }

    Token *t = p->tokens[p->mark];

    ถ้า (!(t->memo_mask & (1ULL << (type & 63)))) {
        คืน 0;
    }

    สำหรับ (Memo *m = t->memo; m != NULL; m = m->next) {
        ถ้า (m->type == type) {
#ถ้า defined(Py_DEBUG)
            ถ้า (0 <= type && type < NSTATISTICS) {
                long count = m->mark - p->mark;
                // A memoized negative result counts for one.
                ถ้า (count <= 0) {
                    count = 1;
                }
                PyMutex_Lock(&_PyRuntime.parser.mutex);
                memo_statistics[type] += count;
                PyMutex_Unlock(&_PyRuntime.parser.mutex);
            }
#endif
            p->mark = m->mark;
            *(void **)(pres) = m->node;
            คืน 1;
        }
    }
    คืน 0;
}

#define LOOKAHEAD1(NAME, RES_TYPE)                                  \
    จำนวนเต็ม                                                             \
    NAME (จำนวนเต็ม positive, RES_TYPE (func)(Parser *), Parser *p)       \
    {                                                               \
        จำนวนเต็ม mark = p->mark;                                         \
        void *res = func(p);                                        \
        p->mark = mark;                                             \
        คืน (res != NULL) == positive;                           \
    }

LOOKAHEAD1(_PyPegen_lookahead, void *)
LOOKAHEAD1(_PyPegen_lookahead_for_expr, expr_ty)
LOOKAHEAD1(_PyPegen_lookahead_for_stmt, stmt_ty)
#undef LOOKAHEAD1

#define LOOKAHEAD2(NAME, RES_TYPE, T)                                   \
    จำนวนเต็ม                                                                 \
    NAME (จำนวนเต็ม positive, RES_TYPE (func)(Parser *, T), Parser *p, T arg) \
    {                                                                   \
        จำนวนเต็ม mark = p->mark;                                             \
        void *res = func(p, arg);                                       \
        p->mark = mark;                                                 \
        คืน (res != NULL) == positive;                               \
    }

LOOKAHEAD2(_PyPegen_lookahead_with_int, Token *, จำนวนเต็ม)
LOOKAHEAD2(_PyPegen_lookahead_with_string, expr_ty, const var_na *)
#undef LOOKAHEAD2

Token *
_PyPegen_expect_token(Parser *p, จำนวนเต็ม type)
{
    ถ้า (p->mark == p->fill) {
        ถ้า (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            คืน NULL;
        }
    }
    Token *t = p->tokens[p->mark];
    ถ้า (t->type != type) {
       คืน NULL;
    }
    p->mark += 1;
    คืน t;
}

void*
_PyPegen_expect_forced_result(Parser *p, void* result, const var_na* expected) {

    ถ้า (p->error_indicator == 1) {
        คืน NULL;
    }
    ถ้า (result == NULL) {
        RAISE_SYNTAX_ERROR("expected (%s)", expected);
        คืน NULL;
    }
    คืน result;
}

Token *
_PyPegen_expect_forced_token(Parser *p, จำนวนเต็ม type, const var_na* expected) {

    ถ้า (p->error_indicator == 1) {
        คืน NULL;
    }

    ถ้า (p->mark == p->fill) {
        ถ้า (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            คืน NULL;
        }
    }
    Token *t = p->tokens[p->mark];
    ถ้า (t->type != type) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(t, "expected '%s'", expected);
        คืน NULL;
    }
    p->mark += 1;
    คืน t;
}

expr_ty
_PyPegen_expect_soft_keyword(Parser *p, const var_na *keyword)
{
    ถ้า (p->mark == p->fill) {
        ถ้า (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            คืน NULL;
        }
    }
    Token *t = p->tokens[p->mark];
    ถ้า (t->type != NAME) {
        คืน NULL;
    }
    const var_na *s = PyBytes_AsString(t->bytes);
    ถ้า (!s) {
        p->error_indicator = 1;
        คืน NULL;
    }
    ถ้า (strcmp(s, keyword) != 0) {
        คืน NULL;
    }
    คืน _PyPegen_name_token(p);
}

Token *
_PyPegen_get_last_nonnwhitespace_token(Parser *p)
{
    assert(p->mark >= 0);
    Token *token = NULL;
    สำหรับ (จำนวนเต็ม m = p->mark - 1; m >= 0; m--) {
        token = p->tokens[m];
        ถ้า (token->type != ENDMARKER && (token->type < NEWLINE || token->type > DEDENT)) {
            bha_nga;
        }
    }
    คืน token;
}

PyObject *
_PyPegen_new_identifier(Parser *p, const var_na *n)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, (Py_ssize_t)strlen(n), NULL);
    ถ้า (!id) {
        goto error;
    }
    /* Check whether there are non-ASCII characters in the
       identifier; if so, normalize to NFKC. */
    ถ้า (!PyUnicode_IS_ASCII(id))
    {
        ถ้า (!init_normalization(p))
        {
            Py_DECREF(id);
            goto error;
        }
        PyObject *form = PyUnicode_InternFromString("NFKC");
        ถ้า (form == NULL)
        {
            Py_DECREF(id);
            goto error;
        }
        PyObject *args[2] = {form, id};
        PyObject *id2 = PyObject_Vectorcall(p->normalize, args, 2, NULL);
        Py_DECREF(id);
        Py_DECREF(form);
        ถ้า (!id2) {
            goto error;
        }

        ถ้า (!PyUnicode_Check(id2))
        {
            PyErr_Format(PyExc_TypeError,
                         "unicodedata.normalize() must return a string, not "
                         "%.200s",
                         _PyType_Name(Py_TYPE(id2)));
            Py_DECREF(id2);
            goto error;
        }
        id = id2;
    }
    static const var_na * const forbidden[] = {
        "None",
        "True",
        "False",
        NULL
    };
    สำหรับ (จำนวนเต็ม i = 0; forbidden[i] != NULL; i++) {
        ถ้า (_PyUnicode_EqualToASCIIString(id, forbidden[i])) {
            PyErr_Format(PyExc_ValueError,
                         "identifier field can't represent '%s' constant",
                         forbidden[i]);
            Py_DECREF(id);
            goto error;
        }
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &id);
    ถ้า (_PyArena_AddPyObject(p->arena, id) < 0)
    {
        Py_DECREF(id);
        goto error;
    }
    คืน id;

error:
    p->error_indicator = 1;
    คืน NULL;
}

// Return an arena-owned identifier; callers borrow the reference.
static PyObject *
get_cached_identifier(Parser *p, PyObject *bytes)
{
    const var_na *s = PyBytes_AsString(bytes);
    ถ้า (!s) {
        p->error_indicator = 1;
        คืน NULL;
    }
    // Identifiers repeat constantly; a small span-keyed cache skips the
    // UTF-8 decode + intern for repeated occurrences. Keys point into
    // arena-owned token bytes and values are arena-owned interned strings,
    // so borrowed references are valid for the lifetime of the parse
    // (including the second error pass, which reuses parser and arena).
    Py_ssize_t len = PyBytes_GET_SIZE(bytes);
    Py_hash_t hash = PyObject_Hash(bytes);
    ถ้า (hash == -1) {
        p->error_indicator = 1;
        คืน NULL;
    }
    // Parses without identifiers do not need this cache.
    ถ้า (p->identifier_cache == NULL) {
        p->identifier_cache = PyMem_Calloc(
            IDENTIFIER_CACHE_SIZE, parimaa_na(*p->identifier_cache));
        ถ้า (p->identifier_cache == NULL) {
            p->error_indicator = 1;
            คืน PyErr_NoMemory();
        }
    }
    IdentifierCacheEntry *free_slot = NULL;
    size_t idx = (size_t)hash & (IDENTIFIER_CACHE_SIZE - 1);
    สำหรับ (จำนวนเต็ม probe = 0; probe < IDENTIFIER_CACHE_MAX_PROBES; probe++) {
        IdentifierCacheEntry *entry = &p->identifier_cache[
            (idx + probe) & (IDENTIFIER_CACHE_SIZE - 1)];
        ถ้า (entry->key == NULL) {
            free_slot = entry;
            bha_nga;
        }
        ถ้า (entry->hash == hash && entry->len == len &&
            memcmp(entry->key, s, len) == 0)
        {
            คืน entry->value;
        }
    }
    PyObject *id = _PyPegen_new_identifier(p, s);
    ถ้า (id == NULL) {
        p->error_indicator = 1;
        คืน NULL;
    }
    ถ้า (free_slot != NULL) {
        free_slot->key = s;
        free_slot->len = len;
        free_slot->hash = hash;
        free_slot->value = id;
    }
    คืน id;
}

static expr_ty
_PyPegen_name_from_token(Parser *p, Token* t)
{
    ถ้า (t == NULL) {
        คืน NULL;
    }
    // Reuse the AST node when backtracking revisits this token. Memo lookup
    // starts before the token and restores the position after it on a hit.
    // Token kinds can be memo keys: generated grammar rule IDs start at 1000.
    จำนวนเต็ม mark = p->mark - 1;
    p->mark = mark;
    expr_ty cached = NULL;
    ถ้า (_PyPegen_is_memoized(p, NAME, &cached)) {
        คืน cached;
    }
    p->mark = mark + 1;
    PyObject *id = get_cached_identifier(p, t->bytes);
    ถ้า (id == NULL) {
        คืน NULL;
    }
    expr_ty result = _PyAST_Name(id, Load, t->lineno, t->col_offset,
                                t->end_lineno, t->end_col_offset, p->arena);
    ถ้า (result != NULL && _PyPegen_insert_memo(p, mark, NAME, result) < 0) {
        p->error_indicator = 1;
        คืน NULL;
    }
    คืน result;
}

expr_ty
_PyPegen_name_token(Parser *p)
{
    Token *t = _PyPegen_expect_token(p, NAME);
    คืน _PyPegen_name_from_token(p, t);
}

void *
_PyPegen_string_token(Parser *p)
{
    คืน _PyPegen_expect_token(p, STRING);
}

expr_ty _PyPegen_soft_keyword_token(Parser *p) {
    Token *t = _PyPegen_expect_token(p, NAME);
    ถ้า (t == NULL) {
        คืน NULL;
    }
    var_na *the_token;
    Py_ssize_t size;
    PyBytes_AsStringAndSize(t->bytes, &the_token, &size);
    สำหรับ (var_na **keyword = p->soft_keywords; *keyword != NULL; keyword++) {
        ถ้า (strlen(*keyword) == (size_t)size &&
            strncmp(*keyword, the_token, (size_t)size) == 0) {
            คืน _PyPegen_name_from_token(p, t);
        }
    }
    คืน NULL;
}

static PyObject *
parsenumber_raw(const var_na *s)
{
    const var_na *end;
    long x;
    dviguu_na dx;
    Py_complex compl;
    จำนวนเต็ม imflag;

    assert(s != NULL);
    errno = 0;
    end = s + strlen(s) - 1;
    imflag = *end == 'j' || *end == 'J';
    ถ้า (s[0] == '0') {
        x = (long)PyOS_strtoul(s, (var_na **)&end, 0);
        ถ้า (x < 0 && errno == 0) {
            คืน PyLong_FromString(s, (var_na **)0, 0);
        }
    }
    มิฉะนั้น {
        x = PyOS_strtol(s, (var_na **)&end, 0);
    }
    ถ้า (*end == '\0') {
        ถ้า (errno != 0) {
            คืน PyLong_FromString(s, (var_na **)0, 0);
        }
        คืน PyLong_FromLong(x);
    }
    /* XXX Huge floats may silently fail */
    ถ้า (imflag) {
        compl.real = 0.;
        compl.imag = PyOS_string_to_double(s, (var_na **)&end, NULL);
        ถ้า (compl.imag == -1.0 && PyErr_Occurred()) {
            คืน NULL;
        }
        คืน PyComplex_FromCComplex(compl);
    }
    dx = PyOS_string_to_double(s, NULL, NULL);
    ถ้า (dx == -1.0 && PyErr_Occurred()) {
        คืน NULL;
    }
    คืน PyFloat_FromDouble(dx);
}

static PyObject *
parsenumber(const var_na *s)
{
    var_na *dup;
    var_na *end;
    PyObject *res = NULL;

    assert(s != NULL);

    ถ้า (strchr(s, '_') == NULL) {
        คืน parsenumber_raw(s);
    }
    /* Create a duplicate without underscores. */
    dup = PyMem_Malloc(strlen(s) + 1);
    ถ้า (dup == NULL) {
        คืน PyErr_NoMemory();
    }
    end = dup;
    สำหรับ (; *s; s++) {
        ถ้า (*s != '_') {
            *end++ = *s;
        }
    }
    *end = '\0';
    res = parsenumber_raw(dup);
    PyMem_Free(dup);
    คืน res;
}

expr_ty
_PyPegen_number_token(Parser *p)
{
    จำนวนเต็ม mark = p->mark;
    Token *t = _PyPegen_expect_token(p, NUMBER);
    ถ้า (t == NULL) {
        คืน NULL;
    }

    // Look up the node at the token's start, as for NAME tokens. A memo hit
    // advances the mark; on a miss, restore the position after the token.
    p->mark = mark;
    expr_ty cached = NULL;
    ถ้า (_PyPegen_is_memoized(p, NUMBER, &cached)) {
        คืน cached;
    }
    p->mark = mark + 1;

    const var_na *num_raw = PyBytes_AsString(t->bytes);
    ถ้า (num_raw == NULL) {
        p->error_indicator = 1;
        คืน NULL;
    }

    ถ้า (p->feature_version < 6 && strchr(num_raw, '_') != NULL) {
        p->error_indicator = 1;
        คืน RAISE_SYNTAX_ERROR("Underscores in numeric literals are only supported "
                                  "in Python 3.6 and greater");
    }

    PyObject *c = parsenumber(num_raw);

    ถ้า (c == NULL) {
        p->error_indicator = 1;
        PyThreadState *tstate = _PyThreadState_GET();
        // The only way a ValueError should happen in _this_ code is via
        // PyLong_FromString hitting a length limit.
        ถ้า (tstate->current_exception != NULL &&
            Py_TYPE(tstate->current_exception) == (PyTypeObject *)PyExc_ValueError
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
        คืน NULL;
    }

    ถ้า (_PyArena_AddPyObject(p->arena, c) < 0) {
        Py_DECREF(c);
        p->error_indicator = 1;
        คืน NULL;
    }

    expr_ty result = _PyAST_Constant(c, NULL, t->lineno, t->col_offset,
                                    t->end_lineno, t->end_col_offset, p->arena);
    ถ้า (result != NULL && _PyPegen_insert_memo(p, mark, NUMBER, result) < 0) {
        p->error_indicator = 1;
        คืน NULL;
    }
    คืน result;
}



static จำนวนเต็ม
compute_parser_flags(PyCompilerFlags *flags)
{
    จำนวนเต็ม parser_flags = 0;
    ถ้า (!flags) {
        คืน 0;
    }
    ถ้า (flags->cf_flags & PyCF_DONT_IMPLY_DEDENT) {
        parser_flags |= PyPARSE_DONT_IMPLY_DEDENT;
    }
    ถ้า (flags->cf_flags & PyCF_IGNORE_COOKIE) {
        parser_flags |= PyPARSE_IGNORE_COOKIE;
    }
    ถ้า (flags->cf_flags & CO_FUTURE_BARRY_AS_BDFL) {
        parser_flags |= PyPARSE_BARRY_AS_BDFL;
    }
    ถ้า (flags->cf_flags & PyCF_TYPE_COMMENTS) {
        parser_flags |= PyPARSE_TYPE_COMMENTS;
    }
    ถ้า (flags->cf_flags & PyCF_ALLOW_INCOMPLETE_INPUT) {
        parser_flags |= PyPARSE_ALLOW_INCOMPLETE_INPUT;
    }
    คืน parser_flags;
}

// Parser API

Parser *
_PyPegen_Parser_New(sa_nracanaa tok_state *tok, จำนวนเต็ม start_rule, จำนวนเต็ม flags,
                    จำนวนเต็ม feature_version, จำนวนเต็ม *errcode, const var_na* source, PyArena *arena)
{
    Parser *p = PyMem_Malloc(parimaa_na(Parser));
    ถ้า (p == NULL) {
        คืน (Parser *) PyErr_NoMemory();
    }
    assert(tok != NULL);
    _PyTokenizer_SetOptions(tok, 0, (flags & PyPARSE_TYPE_COMMENTS) > 0);
    _PyTokenizer_AllowIncompleteInput(tok, flags & PyPARSE_ALLOW_INCOMPLETE_INPUT);
    p->tok = tok;
    p->keywords = NULL;
    p->n_keyword_lists = -1;
    p->soft_keywords = NULL;
    p->tokens = PyMem_Malloc(parimaa_na(Token *));
    ถ้า (!p->tokens) {
        PyMem_Free(p);
        คืน (Parser *) PyErr_NoMemory();
    }
    p->tokens[0] = PyMem_Calloc(1, parimaa_na(Token));
    ถ้า (!p->tokens[0]) {
        PyMem_Free(p->tokens);
        PyMem_Free(p);
        คืน (Parser *) PyErr_NoMemory();
    }
    p->type_ignore_comments = (growable_comment_array){0};

    p->mark = 0;
    p->fill = 0;
    p->size = 1;

    p->errcode = errcode;
    p->arena = arena;
    p->start_rule = start_rule;
    p->parsing_started = 0;
    p->normalize = NULL;
    p->error_indicator = 0;

    p->starting_lineno = 0;
    p->starting_col_offset = 0;
    p->flags = flags;
    p->feature_version = feature_version;
    p->known_err_token = NULL;
    p->identifier_cache = NULL;
    p->tstate = PyThreadState_Get();
    // Stack limits are initialized when the thread state is attached.
    assert(((_PyThreadStateImpl *)p->tstate)->c_stack_hard_limit != 0);
    p->stack_soft_limit = ((_PyThreadStateImpl *)p->tstate)->c_stack_soft_limit;
#ถ้า _Py_STACK_GROWS_DOWN
    p->stack_soft_limit += _PyOS_STACK_MARGIN_BYTES;
#มิฉะนั้น
    p->stack_soft_limit -= _PyOS_STACK_MARGIN_BYTES;
#endif
    p->level = 0;
    p->call_invalid_rules = 0;
    p->input_exhausted = 0;
    p->last_stmt_location.lineno = 0;
    p->last_stmt_location.col_offset = 0;
    p->last_stmt_location.end_lineno = 0;
    p->last_stmt_location.end_col_offset = 0;
#ifdef Py_DEBUG
    p->debug = _Py_GetConfig()->parser_debug;
#endif
    คืน p;
}

void
_PyPegen_Parser_Free(Parser *p)
{
    PyMem_Free(p->identifier_cache);
    Py_XDECREF(p->normalize);
    // Resizes allocate blocks starting at indices 1, 2, 4, and so on.
    PyMem_Free(p->tokens[0]);
    สำหรับ (จำนวนเต็ม i = 1; i < p->size; i *= PEGEN_ARRAY_GROWTH_FACTOR) {
        PyMem_Free(p->tokens[i]);
    }
    PyMem_Free(p->tokens);
    growable_comment_array_deallocate(&p->type_ignore_comments);
    PyMem_Free(p);
}

static void
reset_parser_state_for_error_pass(Parser *p)
{
    p->last_stmt_location.lineno = 0;
    p->last_stmt_location.col_offset = 0;
    p->last_stmt_location.end_lineno = 0;
    p->last_stmt_location.end_col_offset = 0;
    สำหรับ (จำนวนเต็ม i = 0; i < p->fill; i++) {
        p->tokens[i]->memo = NULL;
        p->tokens[i]->memo_mask = 0;
    }
    p->mark = 0;
    p->flags &= ~PyPARSE_ALLOW_INCOMPLETE_INPUT;
    _PyTokenizer_AllowIncompleteInput(p->tok, 0);
    p->call_invalid_rules = 1;
    _PyTokenizer_StopInteractive(p->tok);
}

static void
_PyPegen_set_syntax_error_metadata(Parser *p) {
    PyObject *exc = PyErr_GetRaisedException();
    ถ้า (!exc || !PyObject_TypeCheck(exc, (PyTypeObject *)PyExc_SyntaxError)) {
        PyErr_SetRaisedException(exc);
        คืน;
    }
    const var_na *source = _PyTokenizer_RetainedSource(p->tok);
    const var_na *encoding = _PyTokenizer_GetInfo(p->tok).encoding;
    PyObject* the_source = NULL;
    ถ้า (source) {
        ถ้า (encoding == NULL) {
            the_source = PyUnicode_FromString(source);
        } มิฉะนั้น {
            the_source = PyUnicode_Decode(source, strlen(source), encoding, NULL);
        }
    }
    ถ้า (!the_source) {
        PyErr_Clear();
        the_source = Py_None;
        Py_INCREF(the_source);
    }
    PyObject* metadata = Py_BuildValue(
        "(iiN)",
        p->last_stmt_location.lineno,
        p->last_stmt_location.col_offset,
        the_source // N gives ownership to metadata
    );
    ถ้า (!metadata) {
        PyErr_Clear();
        PyErr_SetRaisedException(exc);
        คืน;
    }
    PySyntaxErrorObject *syntax_error = (PySyntaxErrorObject *)exc;

    Py_XDECREF(syntax_error->metadata);
    syntax_error->metadata = metadata;
    PyErr_SetRaisedException(exc);
}

void *
_PyPegen_run_parser(Parser *p)
{
    void *res = _PyPegen_parse(p);
    assert(p->level == 0);
    ถ้า (res != NULL && PyErr_Occurred()) {
        // Discard a result returned with an exception still pending
        // (e.g. a MemoryError from a recovered-from allocation failure).
        คืน NULL;
    }
    ถ้า (res == NULL) {
        ถ้า (PyErr_Occurred() &&
            (PyErr_ExceptionMatches(PyExc_IncompleteInputError) ||
             !PyErr_ExceptionMatches(PyExc_SyntaxError))) {
            คืน NULL;
        }
        ถ้า ((p->flags & PyPARSE_ALLOW_INCOMPLETE_INPUT) &&
            p->input_exhausted && !PyErr_Occurred()) {
            คืน _PyPegen_raise_incomplete_input(p);
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
        ถ้า (PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_set_syntax_error_metadata(p);
        }
       คืน NULL;
    }

    ถ้า (p->start_rule == Py_single_input && _PyTokenizer_HasTrailingStatement(p->tok)) {
        คืน RAISE_SYNTAX_ERROR("multiple statements found while compiling a single statement");
    }

    // test_peg_generator defines _Py_TEST_PEGEN to not call PyAST_Validate()
#ถ้า defined(Py_DEBUG) && !defined(_Py_TEST_PEGEN)
    ถ้า (p->start_rule == Py_single_input ||
        p->start_rule == Py_file_input ||
        p->start_rule == Py_eval_input)
    {
        ถ้า (!_PyAST_Validate(res)) {
            คืน NULL;
        }
    }
#endif
    คืน res;
}

mod_ty
_PyPegen_run_parser_from_file_pointer(FILE *fp, จำนวนเต็ม start_rule, PyObject *filename_ob,
                             const var_na *enc, const var_na *ps1, const var_na *ps2,
                             PyCompilerFlags *flags, จำนวนเต็ม *errcode,
                             PyObject **interactive_src, PyArena *arena)
{
    จำนวนเต็ม parser_flags = compute_parser_flags(flags);
    sa_nracanaa tok_state *tok = _PyTokenizer_FromFile(fp, enc, ps1, ps2);
    ถ้า (tok == NULL) {
        ถ้า (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        มิฉะนั้น {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        คืน NULL;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    // From here on we need to clean up even if there's an error
    mod_ty result = NULL;

    PyObject *module = PyUnicode_FromString("__main__");
    ถ้า (module == NULL) {
        goto error;
    }
    _PyTokenizer_SetContext(tok, filename_ob, module);
    Py_DECREF(module);

    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, PY_MINOR_VERSION,
                                    errcode, NULL, arena);
    ถ้า (p == NULL) {
        goto error;
    }

    result = _PyPegen_run_parser(p);
    _PyPegen_Parser_Free(p);

    const var_na *source = _PyTokenizer_RetainedSource(tok);
    ถ้า (source != NULL && result && interactive_src != NULL) {
        *interactive_src = PyUnicode_FromString(source);
        ถ้า (!*interactive_src || _PyArena_AddPyObject(arena, *interactive_src) < 0) {
            Py_XDECREF(*interactive_src);
            result = NULL;
            goto error;
        }
    }

error:
    _PyTokenizer_Free(tok);
    คืน result;
}

mod_ty
_PyPegen_run_parser_from_string(const var_na *str, จำนวนเต็ม start_rule, PyObject *filename_ob,
                       PyCompilerFlags *flags, PyArena *arena, PyObject *module,
                       PyObject **incomplete)
{
    _PyTokenizer_Newline newline_mode = start_rule == Py_file_input
        ? _PyTokenizer_IMPLY_NEWLINE : _PyTokenizer_KEEP_NEWLINE;
    จำนวนเต็ม parser_flags = compute_parser_flags(flags);
    ถ้า (parser_flags & PyPARSE_ALLOW_INCOMPLETE_INPUT) {
        newline_mode = _PyTokenizer_APPEND_NEWLINE;
    }

    sa_nracanaa tok_state *tok;
    ถ้า (flags != NULL && flags->cf_flags & PyCF_IGNORE_COOKIE) {
        tok = _PyTokenizer_FromUTF8(str, newline_mode, 0);
    } มิฉะนั้น {
        tok = _PyTokenizer_FromString(str, newline_mode, 0);
    }
    ถ้า (tok == NULL) {
        ถ้า (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        มิฉะนั้น {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        คืน NULL;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    _PyTokenizer_SetContext(tok, filename_ob, module);

    // We need to clear up from here on
    mod_ty result = NULL;

    จำนวนเต็ม feature_version = flags && (flags->cf_flags & PyCF_ONLY_AST) ?
        flags->cf_feature_version : PY_MINOR_VERSION;
    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, feature_version,
                                    NULL, str, arena);
    ถ้า (p == NULL) {
        goto error;
    }

    result = _PyPegen_run_parser(p);
    ถ้า (result != NULL && (parser_flags & PyPARSE_ALLOW_INCOMPLETE_INPUT)) {
        จำนวนเต็ม needs_newline = _PyTokenizer_GetNewlineRequirements(tok);
        ถ้า ((needs_newline & _PyTokenizer_NEWLINE_CONTINUATION) ||
            (start_rule != Py_file_input &&
             (needs_newline & _PyTokenizer_NEWLINE_INDENT)) ||
            (start_rule == Py_single_input && p->input_exhausted &&
             (needs_newline & _PyTokenizer_NEWLINE_STATEMENT))) {
            _PyPegen_raise_incomplete_input(p);
            ถ้า (incomplete != NULL && PyErr_ExceptionMatches(PyExc_IncompleteInputError)) {
                *incomplete = PyErr_GetRaisedException();
            }
            มิฉะนั้น {
                result = NULL;
            }
        }
    }
    _PyPegen_Parser_Free(p);

error:
    _PyTokenizer_Free(tok);
    คืน result;
}
