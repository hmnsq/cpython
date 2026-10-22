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

struct _identifier_cache_entry {
    const char *key;   // Borrowed from arena-owned token bytes.
    Py_ssize_t len;
    Py_hash_t hash;
    PyObject *value;   // Borrowed from an arena-owned identifier.
};

// Internal parser functions

asdl_stmt_seq*
_PyPegen_interactive_exit(Parser *p)
{
    만약 (p->errcode) {
        *(p->errcode) = E_EOF;
    }
    반환 NULL;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset_line(PyObject *line, Py_ssize_t col_offset, Py_ssize_t end_col_offset)
{
    const unsigned char *data = (const unsigned char*)PyUnicode_AsUTF8(line);
    만약 (data == NULL) {
        반환 -1;
    }

    Py_ssize_t len = 0;
    동안 (col_offset < end_col_offset) {
        Py_UCS4 ch = data[col_offset];
        만약 (ch < 0x80) {
            col_offset += 1;
        } 아니면 만약 ((ch & 0xe0) == 0xc0) {
            col_offset += 2;
        } 아니면 만약 ((ch & 0xf0) == 0xe0) {
            col_offset += 3;
        } 아니면 만약 ((ch & 0xf8) == 0xf0) {
            col_offset += 4;
        } 아니면 {
            PyErr_SetString(PyExc_ValueError, "Invalid UTF-8 sequence");
            반환 -1;
        }
        len++;
    }
    반환 len;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset_raw(const char* str, Py_ssize_t col_offset)
{
    Py_ssize_t len = (Py_ssize_t)strlen(str);
    만약 (col_offset > len + 1) {
        col_offset = len + 1;
    }
    assert(col_offset >= 0);
    PyObject *text = PyUnicode_DecodeUTF8(str, col_offset, "replace");
    만약 (!text) {
        반환 -1;
    }
    Py_ssize_t size = PyUnicode_GET_LENGTH(text);
    Py_DECREF(text);
    반환 size;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset(PyObject *line, Py_ssize_t col_offset)
{
    const char *str = PyUnicode_AsUTF8(line);
    만약 (!str) {
        반환 -1;
    }
    반환 _PyPegen_byte_offset_to_character_offset_raw(str, col_offset);
}

// Here, mark is the start of the node, while p->mark is the end.
// If node==NULL, they should be the same.
정수
_PyPegen_insert_memo(Parser *p, 정수 mark, 정수 type, void *node)
{
    // Insert in front
    Memo *m = _PyArena_Malloc(p->arena, sizeof(Memo));
    만약 (m == NULL) {
        반환 -1;
    }
    m->type = type;
    m->node = node;
    m->mark = p->mark;
    m->next = p->tokens[mark]->memo;
    p->tokens[mark]->memo = m;
    p->tokens[mark]->memo_mask |= 1ULL << (type & 63);
    반환 0;
}

// Like _PyPegen_insert_memo(), but returns the inserted Memo so callers
// can update it in place without re-walking the token's memo list.
Memo *
_PyPegen_insert_memo_direct(Parser *p, 정수 mark, 정수 type)
{
    만약 (_PyPegen_insert_memo(p, mark, type, NULL) < 0) {
        반환 NULL;
    }
    반환 p->tokens[mark]->memo;
}

// Like _PyPegen_insert_memo(), but updates an existing node if found.
정수
_PyPegen_update_memo(Parser *p, 정수 mark, 정수 type, void *node)
{
    위해 (Memo *m = p->tokens[mark]->memo; m != NULL; m = m->next) {
        만약 (m->type == type) {
            // Update existing node.
            m->node = node;
            m->mark = p->mark;
            반환 0;
        }
    }
    // Insert new node.
    반환 _PyPegen_insert_memo(p, mark, type, node);
}

static 정수
init_normalization(Parser *p)
{
    만약 (p->normalize) {
        반환 1;
    }
    p->normalize = PyImport_ImportModuleAttrString("unicodedata", "normalize");
    만약 (!p->normalize)
    {
        반환 0;
    }
    반환 1;
}

정수
_PyPegen_grow_loop_buffer(void ***buffer, Py_ssize_t *capacity)
{
    만약 ((size_t)*capacity > (size_t)PY_SSIZE_T_MAX /
            (PEGEN_ARRAY_GROWTH_FACTOR * sizeof(**buffer))) {
        반환 -1;
    }
    Py_ssize_t new_capacity = *capacity == 0
        ? 1 : *capacity * PEGEN_ARRAY_GROWTH_FACTOR;
    void **new_buffer = PyMem_Realloc(*buffer, new_capacity * sizeof(**buffer));
    만약 (new_buffer == NULL) {
        반환 -1;
    }
    *buffer = new_buffer;
    *capacity = new_capacity;
    반환 0;
}

static 정수
growable_comment_array_add(growable_comment_array *arr, 정수 lineno, char *comment)
{
    만약 (arr->num_items >= arr->size) {
        만약 (arr->size > (size_t)PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * sizeof(*arr->items))) {
            반환 0;
        }
        size_t new_size = arr->size == 0
            ? TYPE_IGNORE_INITIAL_CAPACITY
            : arr->size * PEGEN_ARRAY_GROWTH_FACTOR;
        void *new_items_array = PyMem_Realloc(
            arr->items, new_size * sizeof(*arr->items));
        만약 (!new_items_array) {
            반환 0;
        }
        arr->items = new_items_array;
        arr->size = new_size;
    }

    arr->items[arr->num_items].lineno = lineno;
    arr->items[arr->num_items].comment = comment;  // Take ownership
    arr->num_items++;
    반환 1;
}

static void
growable_comment_array_deallocate(growable_comment_array *arr)
{
    위해 (size_t i = 0; i < arr->num_items; i++) {
        PyMem_Free(arr->items[i].comment);
    }
    PyMem_Free(arr->items);
}

static 정수
_get_keyword_or_name_type(Parser *p, const char *text, Py_ssize_t length)
{
    assert(length > 0);

    만약 (length >= p->n_keyword_lists ||
        p->keywords[length] == NULL ||
        p->keywords[length]->type == -1) {
        반환 NAME;
    }
    위해 (KeywordToken *k = p->keywords[length]; k != NULL && k->type != -1; k++) {
        만약 (memcmp(k->str, text, (size_t)length) == 0) {
            반환 k->type;
        }
    }
    반환 NAME;
}

// Keep text only where grammar actions or helpers read it. Keyword error
// actions use fixed spellings, so keywords do not need their text.
static inline 정수
token_needs_text(정수 type)
{
    switch (type) {
        case NAME:
        case NUMBER:
        case STRING:
        case FSTRING_START:
        case FSTRING_MIDDLE:
        case FSTRING_END:
        case TSTRING_START:
        case TSTRING_MIDDLE:
        case TSTRING_END:
        case TYPE_COMMENT:
        case NOTEQUAL:  // _PyPegen_check_barry_as_flufl() distinguishes != and <>.
            반환 1;
        default:
            반환 0;
    }
}

static 정수
initialize_token(Parser *p, Token *parser_token, struct token *new_token, 정수 token_type) {
    assert(parser_token != NULL);

    parser_token->type = token_type;
    parser_token->bytes = NULL;
    만약 (token_needs_text(token_type) && new_token->span.start >= 0) {
        Py_ssize_t length;
        const char *text = _PyToken_TextView(p->tok, new_token, &length);
        만약 (token_type == NAME) {
            parser_token->type = _get_keyword_or_name_type(p, text, length);
        }
        // A name reclassified as a keyword does not need its text.
        만약 (parser_token->type == token_type) {
            parser_token->bytes = PyBytes_FromStringAndSize(text, length);
            만약 (parser_token->bytes == NULL) {
                반환 -1;
            }
            만약 (_PyArena_AddPyObject(p->arena, parser_token->bytes) < 0) {
                Py_DECREF(parser_token->bytes);
                반환 -1;
            }
        }
    }

    parser_token->metadata = NULL;
    만약 (new_token->metadata != NULL) {
        만약 (_PyArena_AddPyObject(p->arena, new_token->metadata) < 0) {
            반환 -1;
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

    만약 (token_type == ERRORTOKEN && _PyTokenizer_GetStatus(p->tok) == E_DECODE) {
        반환 _Pypegen_raise_decode_error(p);
    }

    반환 (token_type == ERRORTOKEN ? _Pypegen_tokenizer_error(p) : 0);
}

static 정수
_resize_tokens_array(Parser *p) {
    만약 (p->size > INT_MAX / PEGEN_ARRAY_GROWTH_FACTOR ||
            (size_t)p->size > PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * sizeof(*p->tokens))) {
        PyErr_NoMemory();
        반환 -1;
    }
    정수 newsize = p->size * PEGEN_ARRAY_GROWTH_FACTOR;
    Token **new_tokens = PyMem_Realloc(p->tokens, (size_t)newsize * sizeof(Token *));
    만약 (new_tokens == NULL) {
        PyErr_NoMemory();
        반환 -1;
    }
    p->tokens = new_tokens;

    // Allocate new tokens together without moving existing ones: parser rules
    // may still hold pointers into earlier blocks. Parser_Free frees each block.
    Token *chunk = PyMem_Calloc((size_t)(newsize - p->size), sizeof(Token));
    만약 (chunk == NULL) {
        PyErr_NoMemory();
        반환 -1;
    }
    위해 (정수 i = p->size; i < newsize; i++) {
        p->tokens[i] = &chunk[i - p->size];
    }
    p->size = newsize;
    반환 0;
}

정수
_PyPegen_fill_token(Parser *p)
{
    struct token new_token;
    _PyToken_Init(&new_token);
    _PyTokenizer_Get(p->tok, &new_token);

    // Record and skip '# type: ignore' comments
    동안 (new_token.type == TYPE_IGNORE) {
        Py_ssize_t len;
        const char *text = _PyToken_TextView(p->tok, &new_token, &len);
        char *tag = PyMem_Malloc((size_t)len + 1);
        만약 (tag == NULL) {
            PyErr_NoMemory();
            goto error;
        }
        memcpy(tag, text, (size_t)len);
        tag[len] = '\0';
        // Ownership of tag passes to the growable array
        만약 (!growable_comment_array_add(&p->type_ignore_comments, new_token.end_loc.lineno, tag)) {
            PyMem_Free(tag);
            PyErr_NoMemory();
            goto error;
        }
        _PyTokenizer_Get(p->tok, &new_token);
    }

    정수 type = new_token.type;
    // EOF can finish an ordinary token. Only a boundary token means that
    // the grammar requested input beyond the last physical line.
    만약 (type == ENDMARKER ||
        (type == DEDENT && (p->flags & PyPARSE_ALLOW_INCOMPLETE_INPUT) &&
         _PyTokenizer_GetStatus(p->tok) == E_EOF)) {
        p->input_exhausted = 1;
    }

    // If we have reached the end and we are in single input mode we need to insert a newline and reset the parsing
    만약 (p->start_rule == Py_single_input && type == ENDMARKER && p->parsing_started) {
        type = NEWLINE; /* Add an extra newline */
        p->parsing_started = 0;

        만약 (!(p->flags & PyPARSE_DONT_IMPLY_DEDENT)) {
            _PyTokenizer_ImplyDedents(p->tok);
        }
    }
    아니면 {
        p->parsing_started = 1;
    }

    // Check if we are at the limit of the token array capacity and resize if needed
    만약 ((p->fill == p->size) && (_resize_tokens_array(p) != 0)) {
        goto error;
    }

    Token *t = p->tokens[p->fill];
    정수 result = initialize_token(p, t, &new_token, type);
    _PyToken_Free(&new_token);
    반환 result;
error:
    _PyToken_Free(&new_token);
    반환 -1;
}

#만약 defined(Py_DEBUG)
// Instrumentation to count the effectiveness of memoization.
// The array counts the number of tokens skipped by memoization,
// indexed by type.

#define NSTATISTICS _PYPEGEN_NSTATISTICS
#define memo_statistics _PyRuntime.parser.memo_statistics

void
_PyPegen_clear_memo_statistics(void)
{
    PyMutex_Lock(&_PyRuntime.parser.mutex);
    위해 (정수 i = 0; i < NSTATISTICS; i++) {
        memo_statistics[i] = 0;
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
}

PyObject *
_PyPegen_get_memo_statistics(void)
{
    PyObject *ret = PyList_New(NSTATISTICS);
    만약 (ret == NULL) {
        반환 NULL;
    }

    PyMutex_Lock(&_PyRuntime.parser.mutex);
    위해 (정수 i = 0; i < NSTATISTICS; i++) {
        PyObject *value = PyLong_FromLong(memo_statistics[i]);
        만약 (value == NULL) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            반환 NULL;
        }
        // PyList_SetItem borrows a reference to value.
        만약 (PyList_SetItem(ret, i, value) < 0) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            반환 NULL;
        }
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
    반환 ret;
}
#endif

정수  // bool
_PyPegen_is_memoized(Parser *p, 정수 type, void *pres)
{
    만약 (p->mark == p->fill) {
        만약 (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            반환 -1;
        }
    }

    Token *t = p->tokens[p->mark];

    만약 (!(t->memo_mask & (1ULL << (type & 63)))) {
        반환 0;
    }

    위해 (Memo *m = t->memo; m != NULL; m = m->next) {
        만약 (m->type == type) {
#만약 defined(Py_DEBUG)
            만약 (0 <= type && type < NSTATISTICS) {
                long count = m->mark - p->mark;
                // A memoized negative result counts for one.
                만약 (count <= 0) {
                    count = 1;
                }
                PyMutex_Lock(&_PyRuntime.parser.mutex);
                memo_statistics[type] += count;
                PyMutex_Unlock(&_PyRuntime.parser.mutex);
            }
#endif
            p->mark = m->mark;
            *(void **)(pres) = m->node;
            반환 1;
        }
    }
    반환 0;
}

#define LOOKAHEAD1(NAME, RES_TYPE)                                  \
    정수                                                             \
    NAME (정수 positive, RES_TYPE (func)(Parser *), Parser *p)       \
    {                                                               \
        정수 mark = p->mark;                                         \
        void *res = func(p);                                        \
        p->mark = mark;                                             \
        반환 (res != NULL) == positive;                           \
    }

LOOKAHEAD1(_PyPegen_lookahead, void *)
LOOKAHEAD1(_PyPegen_lookahead_for_expr, expr_ty)
LOOKAHEAD1(_PyPegen_lookahead_for_stmt, stmt_ty)
#undef LOOKAHEAD1

#define LOOKAHEAD2(NAME, RES_TYPE, T)                                   \
    정수                                                                 \
    NAME (정수 positive, RES_TYPE (func)(Parser *, T), Parser *p, T arg) \
    {                                                                   \
        정수 mark = p->mark;                                             \
        void *res = func(p, arg);                                       \
        p->mark = mark;                                                 \
        반환 (res != NULL) == positive;                               \
    }

LOOKAHEAD2(_PyPegen_lookahead_with_int, Token *, 정수)
LOOKAHEAD2(_PyPegen_lookahead_with_string, expr_ty, const char *)
#undef LOOKAHEAD2

Token *
_PyPegen_expect_token(Parser *p, 정수 type)
{
    만약 (p->mark == p->fill) {
        만약 (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            반환 NULL;
        }
    }
    Token *t = p->tokens[p->mark];
    만약 (t->type != type) {
       반환 NULL;
    }
    p->mark += 1;
    반환 t;
}

void*
_PyPegen_expect_forced_result(Parser *p, void* result, const char* expected) {

    만약 (p->error_indicator == 1) {
        반환 NULL;
    }
    만약 (result == NULL) {
        RAISE_SYNTAX_ERROR("expected (%s)", expected);
        반환 NULL;
    }
    반환 result;
}

Token *
_PyPegen_expect_forced_token(Parser *p, 정수 type, const char* expected) {

    만약 (p->error_indicator == 1) {
        반환 NULL;
    }

    만약 (p->mark == p->fill) {
        만약 (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            반환 NULL;
        }
    }
    Token *t = p->tokens[p->mark];
    만약 (t->type != type) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(t, "expected '%s'", expected);
        반환 NULL;
    }
    p->mark += 1;
    반환 t;
}

expr_ty
_PyPegen_expect_soft_keyword(Parser *p, const char *keyword)
{
    만약 (p->mark == p->fill) {
        만약 (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            반환 NULL;
        }
    }
    Token *t = p->tokens[p->mark];
    만약 (t->type != NAME) {
        반환 NULL;
    }
    const char *s = PyBytes_AsString(t->bytes);
    만약 (!s) {
        p->error_indicator = 1;
        반환 NULL;
    }
    만약 (strcmp(s, keyword) != 0) {
        반환 NULL;
    }
    반환 _PyPegen_name_token(p);
}

Token *
_PyPegen_get_last_nonnwhitespace_token(Parser *p)
{
    assert(p->mark >= 0);
    Token *token = NULL;
    위해 (정수 m = p->mark - 1; m >= 0; m--) {
        token = p->tokens[m];
        만약 (token->type != ENDMARKER && (token->type < NEWLINE || token->type > DEDENT)) {
            break;
        }
    }
    반환 token;
}

PyObject *
_PyPegen_new_identifier(Parser *p, const char *n)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, (Py_ssize_t)strlen(n), NULL);
    만약 (!id) {
        goto error;
    }
    /* Check whether there are non-ASCII characters in the
       identifier; if so, normalize to NFKC. */
    만약 (!PyUnicode_IS_ASCII(id))
    {
        만약 (!init_normalization(p))
        {
            Py_DECREF(id);
            goto error;
        }
        PyObject *form = PyUnicode_InternFromString("NFKC");
        만약 (form == NULL)
        {
            Py_DECREF(id);
            goto error;
        }
        PyObject *args[2] = {form, id};
        PyObject *id2 = PyObject_Vectorcall(p->normalize, args, 2, NULL);
        Py_DECREF(id);
        Py_DECREF(form);
        만약 (!id2) {
            goto error;
        }

        만약 (!PyUnicode_Check(id2))
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
    static const char * const forbidden[] = {
        "None",
        "True",
        "False",
        NULL
    };
    위해 (정수 i = 0; forbidden[i] != NULL; i++) {
        만약 (_PyUnicode_EqualToASCIIString(id, forbidden[i])) {
            PyErr_Format(PyExc_ValueError,
                         "identifier field can't represent '%s' constant",
                         forbidden[i]);
            Py_DECREF(id);
            goto error;
        }
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &id);
    만약 (_PyArena_AddPyObject(p->arena, id) < 0)
    {
        Py_DECREF(id);
        goto error;
    }
    반환 id;

error:
    p->error_indicator = 1;
    반환 NULL;
}

// Return an arena-owned identifier; callers borrow the reference.
static PyObject *
get_cached_identifier(Parser *p, PyObject *bytes)
{
    const char *s = PyBytes_AsString(bytes);
    만약 (!s) {
        p->error_indicator = 1;
        반환 NULL;
    }
    // Identifiers repeat constantly; a small span-keyed cache skips the
    // UTF-8 decode + intern for repeated occurrences. Keys point into
    // arena-owned token bytes and values are arena-owned interned strings,
    // so borrowed references are valid for the lifetime of the parse
    // (including the second error pass, which reuses parser and arena).
    Py_ssize_t len = PyBytes_GET_SIZE(bytes);
    Py_hash_t hash = PyObject_Hash(bytes);
    만약 (hash == -1) {
        p->error_indicator = 1;
        반환 NULL;
    }
    // Parses without identifiers do not need this cache.
    만약 (p->identifier_cache == NULL) {
        p->identifier_cache = PyMem_Calloc(
            IDENTIFIER_CACHE_SIZE, sizeof(*p->identifier_cache));
        만약 (p->identifier_cache == NULL) {
            p->error_indicator = 1;
            반환 PyErr_NoMemory();
        }
    }
    IdentifierCacheEntry *free_slot = NULL;
    size_t idx = (size_t)hash & (IDENTIFIER_CACHE_SIZE - 1);
    위해 (정수 probe = 0; probe < IDENTIFIER_CACHE_MAX_PROBES; probe++) {
        IdentifierCacheEntry *entry = &p->identifier_cache[
            (idx + probe) & (IDENTIFIER_CACHE_SIZE - 1)];
        만약 (entry->key == NULL) {
            free_slot = entry;
            break;
        }
        만약 (entry->hash == hash && entry->len == len &&
            memcmp(entry->key, s, len) == 0)
        {
            반환 entry->value;
        }
    }
    PyObject *id = _PyPegen_new_identifier(p, s);
    만약 (id == NULL) {
        p->error_indicator = 1;
        반환 NULL;
    }
    만약 (free_slot != NULL) {
        free_slot->key = s;
        free_slot->len = len;
        free_slot->hash = hash;
        free_slot->value = id;
    }
    반환 id;
}

static expr_ty
_PyPegen_name_from_token(Parser *p, Token* t)
{
    만약 (t == NULL) {
        반환 NULL;
    }
    // Reuse the AST node when backtracking revisits this token. Memo lookup
    // starts before the token and restores the position after it on a hit.
    // Token kinds can be memo keys: generated grammar rule IDs start at 1000.
    정수 mark = p->mark - 1;
    p->mark = mark;
    expr_ty cached = NULL;
    만약 (_PyPegen_is_memoized(p, NAME, &cached)) {
        반환 cached;
    }
    p->mark = mark + 1;
    PyObject *id = get_cached_identifier(p, t->bytes);
    만약 (id == NULL) {
        반환 NULL;
    }
    expr_ty result = _PyAST_Name(id, Load, t->lineno, t->col_offset,
                                t->end_lineno, t->end_col_offset, p->arena);
    만약 (result != NULL && _PyPegen_insert_memo(p, mark, NAME, result) < 0) {
        p->error_indicator = 1;
        반환 NULL;
    }
    반환 result;
}

expr_ty
_PyPegen_name_token(Parser *p)
{
    Token *t = _PyPegen_expect_token(p, NAME);
    반환 _PyPegen_name_from_token(p, t);
}

void *
_PyPegen_string_token(Parser *p)
{
    반환 _PyPegen_expect_token(p, STRING);
}

expr_ty _PyPegen_soft_keyword_token(Parser *p) {
    Token *t = _PyPegen_expect_token(p, NAME);
    만약 (t == NULL) {
        반환 NULL;
    }
    char *the_token;
    Py_ssize_t size;
    PyBytes_AsStringAndSize(t->bytes, &the_token, &size);
    위해 (char **keyword = p->soft_keywords; *keyword != NULL; keyword++) {
        만약 (strlen(*keyword) == (size_t)size &&
            strncmp(*keyword, the_token, (size_t)size) == 0) {
            반환 _PyPegen_name_from_token(p, t);
        }
    }
    반환 NULL;
}

static PyObject *
parsenumber_raw(const char *s)
{
    const char *end;
    long x;
    double dx;
    Py_complex compl;
    정수 imflag;

    assert(s != NULL);
    errno = 0;
    end = s + strlen(s) - 1;
    imflag = *end == 'j' || *end == 'J';
    만약 (s[0] == '0') {
        x = (long)PyOS_strtoul(s, (char **)&end, 0);
        만약 (x < 0 && errno == 0) {
            반환 PyLong_FromString(s, (char **)0, 0);
        }
    }
    아니면 {
        x = PyOS_strtol(s, (char **)&end, 0);
    }
    만약 (*end == '\0') {
        만약 (errno != 0) {
            반환 PyLong_FromString(s, (char **)0, 0);
        }
        반환 PyLong_FromLong(x);
    }
    /* XXX Huge floats may silently fail */
    만약 (imflag) {
        compl.real = 0.;
        compl.imag = PyOS_string_to_double(s, (char **)&end, NULL);
        만약 (compl.imag == -1.0 && PyErr_Occurred()) {
            반환 NULL;
        }
        반환 PyComplex_FromCComplex(compl);
    }
    dx = PyOS_string_to_double(s, NULL, NULL);
    만약 (dx == -1.0 && PyErr_Occurred()) {
        반환 NULL;
    }
    반환 PyFloat_FromDouble(dx);
}

static PyObject *
parsenumber(const char *s)
{
    char *dup;
    char *end;
    PyObject *res = NULL;

    assert(s != NULL);

    만약 (strchr(s, '_') == NULL) {
        반환 parsenumber_raw(s);
    }
    /* Create a duplicate without underscores. */
    dup = PyMem_Malloc(strlen(s) + 1);
    만약 (dup == NULL) {
        반환 PyErr_NoMemory();
    }
    end = dup;
    위해 (; *s; s++) {
        만약 (*s != '_') {
            *end++ = *s;
        }
    }
    *end = '\0';
    res = parsenumber_raw(dup);
    PyMem_Free(dup);
    반환 res;
}

expr_ty
_PyPegen_number_token(Parser *p)
{
    정수 mark = p->mark;
    Token *t = _PyPegen_expect_token(p, NUMBER);
    만약 (t == NULL) {
        반환 NULL;
    }

    // Look up the node at the token's start, as for NAME tokens. A memo hit
    // advances the mark; on a miss, restore the position after the token.
    p->mark = mark;
    expr_ty cached = NULL;
    만약 (_PyPegen_is_memoized(p, NUMBER, &cached)) {
        반환 cached;
    }
    p->mark = mark + 1;

    const char *num_raw = PyBytes_AsString(t->bytes);
    만약 (num_raw == NULL) {
        p->error_indicator = 1;
        반환 NULL;
    }

    만약 (p->feature_version < 6 && strchr(num_raw, '_') != NULL) {
        p->error_indicator = 1;
        반환 RAISE_SYNTAX_ERROR("Underscores in numeric literals are only supported "
                                  "in Python 3.6 and greater");
    }

    PyObject *c = parsenumber(num_raw);

    만약 (c == NULL) {
        p->error_indicator = 1;
        PyThreadState *tstate = _PyThreadState_GET();
        // The only way a ValueError should happen in _this_ code is via
        // PyLong_FromString hitting a length limit.
        만약 (tstate->current_exception != NULL &&
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
        반환 NULL;
    }

    만약 (_PyArena_AddPyObject(p->arena, c) < 0) {
        Py_DECREF(c);
        p->error_indicator = 1;
        반환 NULL;
    }

    expr_ty result = _PyAST_Constant(c, NULL, t->lineno, t->col_offset,
                                    t->end_lineno, t->end_col_offset, p->arena);
    만약 (result != NULL && _PyPegen_insert_memo(p, mark, NUMBER, result) < 0) {
        p->error_indicator = 1;
        반환 NULL;
    }
    반환 result;
}



static 정수
compute_parser_flags(PyCompilerFlags *flags)
{
    정수 parser_flags = 0;
    만약 (!flags) {
        반환 0;
    }
    만약 (flags->cf_flags & PyCF_DONT_IMPLY_DEDENT) {
        parser_flags |= PyPARSE_DONT_IMPLY_DEDENT;
    }
    만약 (flags->cf_flags & PyCF_IGNORE_COOKIE) {
        parser_flags |= PyPARSE_IGNORE_COOKIE;
    }
    만약 (flags->cf_flags & CO_FUTURE_BARRY_AS_BDFL) {
        parser_flags |= PyPARSE_BARRY_AS_BDFL;
    }
    만약 (flags->cf_flags & PyCF_TYPE_COMMENTS) {
        parser_flags |= PyPARSE_TYPE_COMMENTS;
    }
    만약 (flags->cf_flags & PyCF_ALLOW_INCOMPLETE_INPUT) {
        parser_flags |= PyPARSE_ALLOW_INCOMPLETE_INPUT;
    }
    반환 parser_flags;
}

// Parser API

Parser *
_PyPegen_Parser_New(struct tok_state *tok, 정수 start_rule, 정수 flags,
                    정수 feature_version, 정수 *errcode, const char* source, PyArena *arena)
{
    Parser *p = PyMem_Malloc(sizeof(Parser));
    만약 (p == NULL) {
        반환 (Parser *) PyErr_NoMemory();
    }
    assert(tok != NULL);
    _PyTokenizer_SetOptions(tok, 0, (flags & PyPARSE_TYPE_COMMENTS) > 0);
    _PyTokenizer_AllowIncompleteInput(tok, flags & PyPARSE_ALLOW_INCOMPLETE_INPUT);
    p->tok = tok;
    p->keywords = NULL;
    p->n_keyword_lists = -1;
    p->soft_keywords = NULL;
    p->tokens = PyMem_Malloc(sizeof(Token *));
    만약 (!p->tokens) {
        PyMem_Free(p);
        반환 (Parser *) PyErr_NoMemory();
    }
    p->tokens[0] = PyMem_Calloc(1, sizeof(Token));
    만약 (!p->tokens[0]) {
        PyMem_Free(p->tokens);
        PyMem_Free(p);
        반환 (Parser *) PyErr_NoMemory();
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
#만약 _Py_STACK_GROWS_DOWN
    p->stack_soft_limit += _PyOS_STACK_MARGIN_BYTES;
#아니면
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
    반환 p;
}

void
_PyPegen_Parser_Free(Parser *p)
{
    PyMem_Free(p->identifier_cache);
    Py_XDECREF(p->normalize);
    // Resizes allocate blocks starting at indices 1, 2, 4, and so on.
    PyMem_Free(p->tokens[0]);
    위해 (정수 i = 1; i < p->size; i *= PEGEN_ARRAY_GROWTH_FACTOR) {
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
    위해 (정수 i = 0; i < p->fill; i++) {
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
    만약 (!exc || !PyObject_TypeCheck(exc, (PyTypeObject *)PyExc_SyntaxError)) {
        PyErr_SetRaisedException(exc);
        반환;
    }
    const char *source = _PyTokenizer_RetainedSource(p->tok);
    const char *encoding = _PyTokenizer_GetInfo(p->tok).encoding;
    PyObject* the_source = NULL;
    만약 (source) {
        만약 (encoding == NULL) {
            the_source = PyUnicode_FromString(source);
        } 아니면 {
            the_source = PyUnicode_Decode(source, strlen(source), encoding, NULL);
        }
    }
    만약 (!the_source) {
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
    만약 (!metadata) {
        PyErr_Clear();
        PyErr_SetRaisedException(exc);
        반환;
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
    만약 (res != NULL && PyErr_Occurred()) {
        // Discard a result returned with an exception still pending
        // (e.g. a MemoryError from a recovered-from allocation failure).
        반환 NULL;
    }
    만약 (res == NULL) {
        만약 (PyErr_Occurred() &&
            (PyErr_ExceptionMatches(PyExc_IncompleteInputError) ||
             !PyErr_ExceptionMatches(PyExc_SyntaxError))) {
            반환 NULL;
        }
        만약 ((p->flags & PyPARSE_ALLOW_INCOMPLETE_INPUT) &&
            p->input_exhausted && !PyErr_Occurred()) {
            반환 _PyPegen_raise_incomplete_input(p);
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
        만약 (PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_set_syntax_error_metadata(p);
        }
       반환 NULL;
    }

    만약 (p->start_rule == Py_single_input && _PyTokenizer_HasTrailingStatement(p->tok)) {
        반환 RAISE_SYNTAX_ERROR("multiple statements found while compiling a single statement");
    }

    // test_peg_generator defines _Py_TEST_PEGEN to not call PyAST_Validate()
#만약 defined(Py_DEBUG) && !defined(_Py_TEST_PEGEN)
    만약 (p->start_rule == Py_single_input ||
        p->start_rule == Py_file_input ||
        p->start_rule == Py_eval_input)
    {
        만약 (!_PyAST_Validate(res)) {
            반환 NULL;
        }
    }
#endif
    반환 res;
}

mod_ty
_PyPegen_run_parser_from_file_pointer(FILE *fp, 정수 start_rule, PyObject *filename_ob,
                             const char *enc, const char *ps1, const char *ps2,
                             PyCompilerFlags *flags, 정수 *errcode,
                             PyObject **interactive_src, PyArena *arena)
{
    정수 parser_flags = compute_parser_flags(flags);
    struct tok_state *tok = _PyTokenizer_FromFile(fp, enc, ps1, ps2);
    만약 (tok == NULL) {
        만약 (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        아니면 {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        반환 NULL;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    // From here on we need to clean up even if there's an error
    mod_ty result = NULL;

    PyObject *module = PyUnicode_FromString("__main__");
    만약 (module == NULL) {
        goto error;
    }
    _PyTokenizer_SetContext(tok, filename_ob, module);
    Py_DECREF(module);

    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, PY_MINOR_VERSION,
                                    errcode, NULL, arena);
    만약 (p == NULL) {
        goto error;
    }

    result = _PyPegen_run_parser(p);
    _PyPegen_Parser_Free(p);

    const char *source = _PyTokenizer_RetainedSource(tok);
    만약 (source != NULL && result && interactive_src != NULL) {
        *interactive_src = PyUnicode_FromString(source);
        만약 (!*interactive_src || _PyArena_AddPyObject(arena, *interactive_src) < 0) {
            Py_XDECREF(*interactive_src);
            result = NULL;
            goto error;
        }
    }

error:
    _PyTokenizer_Free(tok);
    반환 result;
}

mod_ty
_PyPegen_run_parser_from_string(const char *str, 정수 start_rule, PyObject *filename_ob,
                       PyCompilerFlags *flags, PyArena *arena, PyObject *module,
                       PyObject **incomplete)
{
    _PyTokenizer_Newline newline_mode = start_rule == Py_file_input
        ? _PyTokenizer_IMPLY_NEWLINE : _PyTokenizer_KEEP_NEWLINE;
    정수 parser_flags = compute_parser_flags(flags);
    만약 (parser_flags & PyPARSE_ALLOW_INCOMPLETE_INPUT) {
        newline_mode = _PyTokenizer_APPEND_NEWLINE;
    }

    struct tok_state *tok;
    만약 (flags != NULL && flags->cf_flags & PyCF_IGNORE_COOKIE) {
        tok = _PyTokenizer_FromUTF8(str, newline_mode, 0);
    } 아니면 {
        tok = _PyTokenizer_FromString(str, newline_mode, 0);
    }
    만약 (tok == NULL) {
        만약 (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        아니면 {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        반환 NULL;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    _PyTokenizer_SetContext(tok, filename_ob, module);

    // We need to clear up from here on
    mod_ty result = NULL;

    정수 feature_version = flags && (flags->cf_flags & PyCF_ONLY_AST) ?
        flags->cf_feature_version : PY_MINOR_VERSION;
    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, feature_version,
                                    NULL, str, arena);
    만약 (p == NULL) {
        goto error;
    }

    result = _PyPegen_run_parser(p);
    만약 (result != NULL && (parser_flags & PyPARSE_ALLOW_INCOMPLETE_INPUT)) {
        정수 needs_newline = _PyTokenizer_GetNewlineRequirements(tok);
        만약 ((needs_newline & _PyTokenizer_NEWLINE_CONTINUATION) ||
            (start_rule != Py_file_input &&
             (needs_newline & _PyTokenizer_NEWLINE_INDENT)) ||
            (start_rule == Py_single_input && p->input_exhausted &&
             (needs_newline & _PyTokenizer_NEWLINE_STATEMENT))) {
            _PyPegen_raise_incomplete_input(p);
            만약 (incomplete != NULL && PyErr_ExceptionMatches(PyExc_IncompleteInputError)) {
                *incomplete = PyErr_GetRaisedException();
            }
            아니면 {
                result = NULL;
            }
        }
    }
    _PyPegen_Parser_Free(p);

error:
    _PyTokenizer_Free(tok);
    반환 result;
}
