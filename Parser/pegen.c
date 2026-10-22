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
    если (p->errcode) {
        *(p->errcode) = E_EOF;
    }
    вернуть NULL;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset_line(PyObject *line, Py_ssize_t col_offset, Py_ssize_t end_col_offset)
{
    const unsigned char *data = (const unsigned char*)PyUnicode_AsUTF8(line);
    если (data == NULL) {
        вернуть -1;
    }

    Py_ssize_t len = 0;
    пока (col_offset < end_col_offset) {
        Py_UCS4 ch = data[col_offset];
        если (ch < 0x80) {
            col_offset += 1;
        } иначе если ((ch & 0xe0) == 0xc0) {
            col_offset += 2;
        } иначе если ((ch & 0xf0) == 0xe0) {
            col_offset += 3;
        } иначе если ((ch & 0xf8) == 0xf0) {
            col_offset += 4;
        } иначе {
            PyErr_SetString(PyExc_ValueError, "Invalid UTF-8 sequence");
            вернуть -1;
        }
        len++;
    }
    вернуть len;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset_raw(const char* str, Py_ssize_t col_offset)
{
    Py_ssize_t len = (Py_ssize_t)strlen(str);
    если (col_offset > len + 1) {
        col_offset = len + 1;
    }
    assert(col_offset >= 0);
    PyObject *text = PyUnicode_DecodeUTF8(str, col_offset, "replace");
    если (!text) {
        вернуть -1;
    }
    Py_ssize_t size = PyUnicode_GET_LENGTH(text);
    Py_DECREF(text);
    вернуть size;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset(PyObject *line, Py_ssize_t col_offset)
{
    const char *str = PyUnicode_AsUTF8(line);
    если (!str) {
        вернуть -1;
    }
    вернуть _PyPegen_byte_offset_to_character_offset_raw(str, col_offset);
}

// Here, mark is the start of the node, while p->mark is the end.
// If node==NULL, they should be the same.
целое
_PyPegen_insert_memo(Parser *p, целое mark, целое type, void *node)
{
    // Insert in front
    Memo *m = _PyArena_Malloc(p->arena, sizeof(Memo));
    если (m == NULL) {
        вернуть -1;
    }
    m->type = type;
    m->node = node;
    m->mark = p->mark;
    m->next = p->tokens[mark]->memo;
    p->tokens[mark]->memo = m;
    p->tokens[mark]->memo_mask |= 1ULL << (type & 63);
    вернуть 0;
}

// Like _PyPegen_insert_memo(), but returns the inserted Memo so callers
// can update it in place without re-walking the token's memo list.
Memo *
_PyPegen_insert_memo_direct(Parser *p, целое mark, целое type)
{
    если (_PyPegen_insert_memo(p, mark, type, NULL) < 0) {
        вернуть NULL;
    }
    вернуть p->tokens[mark]->memo;
}

// Like _PyPegen_insert_memo(), but updates an existing node if found.
целое
_PyPegen_update_memo(Parser *p, целое mark, целое type, void *node)
{
    для (Memo *m = p->tokens[mark]->memo; m != NULL; m = m->next) {
        если (m->type == type) {
            // Update existing node.
            m->node = node;
            m->mark = p->mark;
            вернуть 0;
        }
    }
    // Insert new node.
    вернуть _PyPegen_insert_memo(p, mark, type, node);
}

static целое
init_normalization(Parser *p)
{
    если (p->normalize) {
        вернуть 1;
    }
    p->normalize = PyImport_ImportModuleAttrString("unicodedata", "normalize");
    если (!p->normalize)
    {
        вернуть 0;
    }
    вернуть 1;
}

целое
_PyPegen_grow_loop_buffer(void ***buffer, Py_ssize_t *capacity)
{
    если ((size_t)*capacity > (size_t)PY_SSIZE_T_MAX /
            (PEGEN_ARRAY_GROWTH_FACTOR * sizeof(**buffer))) {
        вернуть -1;
    }
    Py_ssize_t new_capacity = *capacity == 0
        ? 1 : *capacity * PEGEN_ARRAY_GROWTH_FACTOR;
    void **new_buffer = PyMem_Realloc(*buffer, new_capacity * sizeof(**buffer));
    если (new_buffer == NULL) {
        вернуть -1;
    }
    *buffer = new_buffer;
    *capacity = new_capacity;
    вернуть 0;
}

static целое
growable_comment_array_add(growable_comment_array *arr, целое lineno, char *comment)
{
    если (arr->num_items >= arr->size) {
        если (arr->size > (size_t)PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * sizeof(*arr->items))) {
            вернуть 0;
        }
        size_t new_size = arr->size == 0
            ? TYPE_IGNORE_INITIAL_CAPACITY
            : arr->size * PEGEN_ARRAY_GROWTH_FACTOR;
        void *new_items_array = PyMem_Realloc(
            arr->items, new_size * sizeof(*arr->items));
        если (!new_items_array) {
            вернуть 0;
        }
        arr->items = new_items_array;
        arr->size = new_size;
    }

    arr->items[arr->num_items].lineno = lineno;
    arr->items[arr->num_items].comment = comment;  // Take ownership
    arr->num_items++;
    вернуть 1;
}

static void
growable_comment_array_deallocate(growable_comment_array *arr)
{
    для (size_t i = 0; i < arr->num_items; i++) {
        PyMem_Free(arr->items[i].comment);
    }
    PyMem_Free(arr->items);
}

static целое
_get_keyword_or_name_type(Parser *p, const char *text, Py_ssize_t length)
{
    assert(length > 0);

    если (length >= p->n_keyword_lists ||
        p->keywords[length] == NULL ||
        p->keywords[length]->type == -1) {
        вернуть NAME;
    }
    для (KeywordToken *k = p->keywords[length]; k != NULL && k->type != -1; k++) {
        если (memcmp(k->str, text, (size_t)length) == 0) {
            вернуть k->type;
        }
    }
    вернуть NAME;
}

// Keep text only where grammar actions or helpers read it. Keyword error
// actions use fixed spellings, so keywords do not need their text.
static inline целое
token_needs_text(целое type)
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
            вернуть 1;
        default:
            вернуть 0;
    }
}

static целое
initialize_token(Parser *p, Token *parser_token, struct token *new_token, целое token_type) {
    assert(parser_token != NULL);

    parser_token->type = token_type;
    parser_token->bytes = NULL;
    если (token_needs_text(token_type) && new_token->span.start >= 0) {
        Py_ssize_t length;
        const char *text = _PyToken_TextView(p->tok, new_token, &length);
        если (token_type == NAME) {
            parser_token->type = _get_keyword_or_name_type(p, text, length);
        }
        // A name reclassified as a keyword does not need its text.
        если (parser_token->type == token_type) {
            parser_token->bytes = PyBytes_FromStringAndSize(text, length);
            если (parser_token->bytes == NULL) {
                вернуть -1;
            }
            если (_PyArena_AddPyObject(p->arena, parser_token->bytes) < 0) {
                Py_DECREF(parser_token->bytes);
                вернуть -1;
            }
        }
    }

    parser_token->metadata = NULL;
    если (new_token->metadata != NULL) {
        если (_PyArena_AddPyObject(p->arena, new_token->metadata) < 0) {
            вернуть -1;
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

    если (token_type == ERRORTOKEN && _PyTokenizer_GetStatus(p->tok) == E_DECODE) {
        вернуть _Pypegen_raise_decode_error(p);
    }

    вернуть (token_type == ERRORTOKEN ? _Pypegen_tokenizer_error(p) : 0);
}

static целое
_resize_tokens_array(Parser *p) {
    если (p->size > INT_MAX / PEGEN_ARRAY_GROWTH_FACTOR ||
            (size_t)p->size > PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * sizeof(*p->tokens))) {
        PyErr_NoMemory();
        вернуть -1;
    }
    целое newsize = p->size * PEGEN_ARRAY_GROWTH_FACTOR;
    Token **new_tokens = PyMem_Realloc(p->tokens, (size_t)newsize * sizeof(Token *));
    если (new_tokens == NULL) {
        PyErr_NoMemory();
        вернуть -1;
    }
    p->tokens = new_tokens;

    // Allocate new tokens together without moving existing ones: parser rules
    // may still hold pointers into earlier blocks. Parser_Free frees each block.
    Token *chunk = PyMem_Calloc((size_t)(newsize - p->size), sizeof(Token));
    если (chunk == NULL) {
        PyErr_NoMemory();
        вернуть -1;
    }
    для (целое i = p->size; i < newsize; i++) {
        p->tokens[i] = &chunk[i - p->size];
    }
    p->size = newsize;
    вернуть 0;
}

целое
_PyPegen_fill_token(Parser *p)
{
    struct token new_token;
    _PyToken_Init(&new_token);
    _PyTokenizer_Get(p->tok, &new_token);

    // Record and skip '# type: ignore' comments
    пока (new_token.type == TYPE_IGNORE) {
        Py_ssize_t len;
        const char *text = _PyToken_TextView(p->tok, &new_token, &len);
        char *tag = PyMem_Malloc((size_t)len + 1);
        если (tag == NULL) {
            PyErr_NoMemory();
            goto error;
        }
        memcpy(tag, text, (size_t)len);
        tag[len] = '\0';
        // Ownership of tag passes to the growable array
        если (!growable_comment_array_add(&p->type_ignore_comments, new_token.end_loc.lineno, tag)) {
            PyMem_Free(tag);
            PyErr_NoMemory();
            goto error;
        }
        _PyTokenizer_Get(p->tok, &new_token);
    }

    целое type = new_token.type;
    // EOF can finish an ordinary token. Only a boundary token means that
    // the grammar requested input beyond the last physical line.
    если (type == ENDMARKER ||
        (type == DEDENT && (p->flags & PyPARSE_ALLOW_INCOMPLETE_INPUT) &&
         _PyTokenizer_GetStatus(p->tok) == E_EOF)) {
        p->input_exhausted = 1;
    }

    // If we have reached the end and we are in single input mode we need to insert a newline and reset the parsing
    если (p->start_rule == Py_single_input && type == ENDMARKER && p->parsing_started) {
        type = NEWLINE; /* Add an extra newline */
        p->parsing_started = 0;

        если (!(p->flags & PyPARSE_DONT_IMPLY_DEDENT)) {
            _PyTokenizer_ImplyDedents(p->tok);
        }
    }
    иначе {
        p->parsing_started = 1;
    }

    // Check if we are at the limit of the token array capacity and resize if needed
    если ((p->fill == p->size) && (_resize_tokens_array(p) != 0)) {
        goto error;
    }

    Token *t = p->tokens[p->fill];
    целое result = initialize_token(p, t, &new_token, type);
    _PyToken_Free(&new_token);
    вернуть result;
error:
    _PyToken_Free(&new_token);
    вернуть -1;
}

#если defined(Py_DEBUG)
// Instrumentation to count the effectiveness of memoization.
// The array counts the number of tokens skipped by memoization,
// indexed by type.

#define NSTATISTICS _PYPEGEN_NSTATISTICS
#define memo_statistics _PyRuntime.parser.memo_statistics

void
_PyPegen_clear_memo_statistics(void)
{
    PyMutex_Lock(&_PyRuntime.parser.mutex);
    для (целое i = 0; i < NSTATISTICS; i++) {
        memo_statistics[i] = 0;
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
}

PyObject *
_PyPegen_get_memo_statistics(void)
{
    PyObject *ret = PyList_New(NSTATISTICS);
    если (ret == NULL) {
        вернуть NULL;
    }

    PyMutex_Lock(&_PyRuntime.parser.mutex);
    для (целое i = 0; i < NSTATISTICS; i++) {
        PyObject *value = PyLong_FromLong(memo_statistics[i]);
        если (value == NULL) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            вернуть NULL;
        }
        // PyList_SetItem borrows a reference to value.
        если (PyList_SetItem(ret, i, value) < 0) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            вернуть NULL;
        }
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
    вернуть ret;
}
#endif

целое  // bool
_PyPegen_is_memoized(Parser *p, целое type, void *pres)
{
    если (p->mark == p->fill) {
        если (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            вернуть -1;
        }
    }

    Token *t = p->tokens[p->mark];

    если (!(t->memo_mask & (1ULL << (type & 63)))) {
        вернуть 0;
    }

    для (Memo *m = t->memo; m != NULL; m = m->next) {
        если (m->type == type) {
#если defined(Py_DEBUG)
            если (0 <= type && type < NSTATISTICS) {
                long count = m->mark - p->mark;
                // A memoized negative result counts for one.
                если (count <= 0) {
                    count = 1;
                }
                PyMutex_Lock(&_PyRuntime.parser.mutex);
                memo_statistics[type] += count;
                PyMutex_Unlock(&_PyRuntime.parser.mutex);
            }
#endif
            p->mark = m->mark;
            *(void **)(pres) = m->node;
            вернуть 1;
        }
    }
    вернуть 0;
}

#define LOOKAHEAD1(NAME, RES_TYPE)                                  \
    целое                                                             \
    NAME (целое positive, RES_TYPE (func)(Parser *), Parser *p)       \
    {                                                               \
        целое mark = p->mark;                                         \
        void *res = func(p);                                        \
        p->mark = mark;                                             \
        вернуть (res != NULL) == positive;                           \
    }

LOOKAHEAD1(_PyPegen_lookahead, void *)
LOOKAHEAD1(_PyPegen_lookahead_for_expr, expr_ty)
LOOKAHEAD1(_PyPegen_lookahead_for_stmt, stmt_ty)
#undef LOOKAHEAD1

#define LOOKAHEAD2(NAME, RES_TYPE, T)                                   \
    целое                                                                 \
    NAME (целое positive, RES_TYPE (func)(Parser *, T), Parser *p, T arg) \
    {                                                                   \
        целое mark = p->mark;                                             \
        void *res = func(p, arg);                                       \
        p->mark = mark;                                                 \
        вернуть (res != NULL) == positive;                               \
    }

LOOKAHEAD2(_PyPegen_lookahead_with_int, Token *, целое)
LOOKAHEAD2(_PyPegen_lookahead_with_string, expr_ty, const char *)
#undef LOOKAHEAD2

Token *
_PyPegen_expect_token(Parser *p, целое type)
{
    если (p->mark == p->fill) {
        если (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            вернуть NULL;
        }
    }
    Token *t = p->tokens[p->mark];
    если (t->type != type) {
       вернуть NULL;
    }
    p->mark += 1;
    вернуть t;
}

void*
_PyPegen_expect_forced_result(Parser *p, void* result, const char* expected) {

    если (p->error_indicator == 1) {
        вернуть NULL;
    }
    если (result == NULL) {
        RAISE_SYNTAX_ERROR("expected (%s)", expected);
        вернуть NULL;
    }
    вернуть result;
}

Token *
_PyPegen_expect_forced_token(Parser *p, целое type, const char* expected) {

    если (p->error_indicator == 1) {
        вернуть NULL;
    }

    если (p->mark == p->fill) {
        если (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            вернуть NULL;
        }
    }
    Token *t = p->tokens[p->mark];
    если (t->type != type) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(t, "expected '%s'", expected);
        вернуть NULL;
    }
    p->mark += 1;
    вернуть t;
}

expr_ty
_PyPegen_expect_soft_keyword(Parser *p, const char *keyword)
{
    если (p->mark == p->fill) {
        если (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            вернуть NULL;
        }
    }
    Token *t = p->tokens[p->mark];
    если (t->type != NAME) {
        вернуть NULL;
    }
    const char *s = PyBytes_AsString(t->bytes);
    если (!s) {
        p->error_indicator = 1;
        вернуть NULL;
    }
    если (strcmp(s, keyword) != 0) {
        вернуть NULL;
    }
    вернуть _PyPegen_name_token(p);
}

Token *
_PyPegen_get_last_nonnwhitespace_token(Parser *p)
{
    assert(p->mark >= 0);
    Token *token = NULL;
    для (целое m = p->mark - 1; m >= 0; m--) {
        token = p->tokens[m];
        если (token->type != ENDMARKER && (token->type < NEWLINE || token->type > DEDENT)) {
            break;
        }
    }
    вернуть token;
}

PyObject *
_PyPegen_new_identifier(Parser *p, const char *n)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, (Py_ssize_t)strlen(n), NULL);
    если (!id) {
        goto error;
    }
    /* Check whether there are non-ASCII characters in the
       identifier; if so, normalize to NFKC. */
    если (!PyUnicode_IS_ASCII(id))
    {
        если (!init_normalization(p))
        {
            Py_DECREF(id);
            goto error;
        }
        PyObject *form = PyUnicode_InternFromString("NFKC");
        если (form == NULL)
        {
            Py_DECREF(id);
            goto error;
        }
        PyObject *args[2] = {form, id};
        PyObject *id2 = PyObject_Vectorcall(p->normalize, args, 2, NULL);
        Py_DECREF(id);
        Py_DECREF(form);
        если (!id2) {
            goto error;
        }

        если (!PyUnicode_Check(id2))
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
    для (целое i = 0; forbidden[i] != NULL; i++) {
        если (_PyUnicode_EqualToASCIIString(id, forbidden[i])) {
            PyErr_Format(PyExc_ValueError,
                         "identifier field can't represent '%s' constant",
                         forbidden[i]);
            Py_DECREF(id);
            goto error;
        }
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &id);
    если (_PyArena_AddPyObject(p->arena, id) < 0)
    {
        Py_DECREF(id);
        goto error;
    }
    вернуть id;

error:
    p->error_indicator = 1;
    вернуть NULL;
}

// Return an arena-owned identifier; callers borrow the reference.
static PyObject *
get_cached_identifier(Parser *p, PyObject *bytes)
{
    const char *s = PyBytes_AsString(bytes);
    если (!s) {
        p->error_indicator = 1;
        вернуть NULL;
    }
    // Identifiers repeat constantly; a small span-keyed cache skips the
    // UTF-8 decode + intern for repeated occurrences. Keys point into
    // arena-owned token bytes and values are arena-owned interned strings,
    // so borrowed references are valid for the lifetime of the parse
    // (including the second error pass, which reuses parser and arena).
    Py_ssize_t len = PyBytes_GET_SIZE(bytes);
    Py_hash_t hash = PyObject_Hash(bytes);
    если (hash == -1) {
        p->error_indicator = 1;
        вернуть NULL;
    }
    // Parses without identifiers do not need this cache.
    если (p->identifier_cache == NULL) {
        p->identifier_cache = PyMem_Calloc(
            IDENTIFIER_CACHE_SIZE, sizeof(*p->identifier_cache));
        если (p->identifier_cache == NULL) {
            p->error_indicator = 1;
            вернуть PyErr_NoMemory();
        }
    }
    IdentifierCacheEntry *free_slot = NULL;
    size_t idx = (size_t)hash & (IDENTIFIER_CACHE_SIZE - 1);
    для (целое probe = 0; probe < IDENTIFIER_CACHE_MAX_PROBES; probe++) {
        IdentifierCacheEntry *entry = &p->identifier_cache[
            (idx + probe) & (IDENTIFIER_CACHE_SIZE - 1)];
        если (entry->key == NULL) {
            free_slot = entry;
            break;
        }
        если (entry->hash == hash && entry->len == len &&
            memcmp(entry->key, s, len) == 0)
        {
            вернуть entry->value;
        }
    }
    PyObject *id = _PyPegen_new_identifier(p, s);
    если (id == NULL) {
        p->error_indicator = 1;
        вернуть NULL;
    }
    если (free_slot != NULL) {
        free_slot->key = s;
        free_slot->len = len;
        free_slot->hash = hash;
        free_slot->value = id;
    }
    вернуть id;
}

static expr_ty
_PyPegen_name_from_token(Parser *p, Token* t)
{
    если (t == NULL) {
        вернуть NULL;
    }
    // Reuse the AST node when backtracking revisits this token. Memo lookup
    // starts before the token and restores the position after it on a hit.
    // Token kinds can be memo keys: generated grammar rule IDs start at 1000.
    целое mark = p->mark - 1;
    p->mark = mark;
    expr_ty cached = NULL;
    если (_PyPegen_is_memoized(p, NAME, &cached)) {
        вернуть cached;
    }
    p->mark = mark + 1;
    PyObject *id = get_cached_identifier(p, t->bytes);
    если (id == NULL) {
        вернуть NULL;
    }
    expr_ty result = _PyAST_Name(id, Load, t->lineno, t->col_offset,
                                t->end_lineno, t->end_col_offset, p->arena);
    если (result != NULL && _PyPegen_insert_memo(p, mark, NAME, result) < 0) {
        p->error_indicator = 1;
        вернуть NULL;
    }
    вернуть result;
}

expr_ty
_PyPegen_name_token(Parser *p)
{
    Token *t = _PyPegen_expect_token(p, NAME);
    вернуть _PyPegen_name_from_token(p, t);
}

void *
_PyPegen_string_token(Parser *p)
{
    вернуть _PyPegen_expect_token(p, STRING);
}

expr_ty _PyPegen_soft_keyword_token(Parser *p) {
    Token *t = _PyPegen_expect_token(p, NAME);
    если (t == NULL) {
        вернуть NULL;
    }
    char *the_token;
    Py_ssize_t size;
    PyBytes_AsStringAndSize(t->bytes, &the_token, &size);
    для (char **keyword = p->soft_keywords; *keyword != NULL; keyword++) {
        если (strlen(*keyword) == (size_t)size &&
            strncmp(*keyword, the_token, (size_t)size) == 0) {
            вернуть _PyPegen_name_from_token(p, t);
        }
    }
    вернуть NULL;
}

static PyObject *
parsenumber_raw(const char *s)
{
    const char *end;
    long x;
    double dx;
    Py_complex compl;
    целое imflag;

    assert(s != NULL);
    errno = 0;
    end = s + strlen(s) - 1;
    imflag = *end == 'j' || *end == 'J';
    если (s[0] == '0') {
        x = (long)PyOS_strtoul(s, (char **)&end, 0);
        если (x < 0 && errno == 0) {
            вернуть PyLong_FromString(s, (char **)0, 0);
        }
    }
    иначе {
        x = PyOS_strtol(s, (char **)&end, 0);
    }
    если (*end == '\0') {
        если (errno != 0) {
            вернуть PyLong_FromString(s, (char **)0, 0);
        }
        вернуть PyLong_FromLong(x);
    }
    /* XXX Huge floats may silently fail */
    если (imflag) {
        compl.real = 0.;
        compl.imag = PyOS_string_to_double(s, (char **)&end, NULL);
        если (compl.imag == -1.0 && PyErr_Occurred()) {
            вернуть NULL;
        }
        вернуть PyComplex_FromCComplex(compl);
    }
    dx = PyOS_string_to_double(s, NULL, NULL);
    если (dx == -1.0 && PyErr_Occurred()) {
        вернуть NULL;
    }
    вернуть PyFloat_FromDouble(dx);
}

static PyObject *
parsenumber(const char *s)
{
    char *dup;
    char *end;
    PyObject *res = NULL;

    assert(s != NULL);

    если (strchr(s, '_') == NULL) {
        вернуть parsenumber_raw(s);
    }
    /* Create a duplicate without underscores. */
    dup = PyMem_Malloc(strlen(s) + 1);
    если (dup == NULL) {
        вернуть PyErr_NoMemory();
    }
    end = dup;
    для (; *s; s++) {
        если (*s != '_') {
            *end++ = *s;
        }
    }
    *end = '\0';
    res = parsenumber_raw(dup);
    PyMem_Free(dup);
    вернуть res;
}

expr_ty
_PyPegen_number_token(Parser *p)
{
    целое mark = p->mark;
    Token *t = _PyPegen_expect_token(p, NUMBER);
    если (t == NULL) {
        вернуть NULL;
    }

    // Look up the node at the token's start, as for NAME tokens. A memo hit
    // advances the mark; on a miss, restore the position after the token.
    p->mark = mark;
    expr_ty cached = NULL;
    если (_PyPegen_is_memoized(p, NUMBER, &cached)) {
        вернуть cached;
    }
    p->mark = mark + 1;

    const char *num_raw = PyBytes_AsString(t->bytes);
    если (num_raw == NULL) {
        p->error_indicator = 1;
        вернуть NULL;
    }

    если (p->feature_version < 6 && strchr(num_raw, '_') != NULL) {
        p->error_indicator = 1;
        вернуть RAISE_SYNTAX_ERROR("Underscores in numeric literals are only supported "
                                  "in Python 3.6 and greater");
    }

    PyObject *c = parsenumber(num_raw);

    если (c == NULL) {
        p->error_indicator = 1;
        PyThreadState *tstate = _PyThreadState_GET();
        // The only way a ValueError should happen in _this_ code is via
        // PyLong_FromString hitting a length limit.
        если (tstate->current_exception != NULL &&
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
        вернуть NULL;
    }

    если (_PyArena_AddPyObject(p->arena, c) < 0) {
        Py_DECREF(c);
        p->error_indicator = 1;
        вернуть NULL;
    }

    expr_ty result = _PyAST_Constant(c, NULL, t->lineno, t->col_offset,
                                    t->end_lineno, t->end_col_offset, p->arena);
    если (result != NULL && _PyPegen_insert_memo(p, mark, NUMBER, result) < 0) {
        p->error_indicator = 1;
        вернуть NULL;
    }
    вернуть result;
}



static целое
compute_parser_flags(PyCompilerFlags *flags)
{
    целое parser_flags = 0;
    если (!flags) {
        вернуть 0;
    }
    если (flags->cf_flags & PyCF_DONT_IMPLY_DEDENT) {
        parser_flags |= PyPARSE_DONT_IMPLY_DEDENT;
    }
    если (flags->cf_flags & PyCF_IGNORE_COOKIE) {
        parser_flags |= PyPARSE_IGNORE_COOKIE;
    }
    если (flags->cf_flags & CO_FUTURE_BARRY_AS_BDFL) {
        parser_flags |= PyPARSE_BARRY_AS_BDFL;
    }
    если (flags->cf_flags & PyCF_TYPE_COMMENTS) {
        parser_flags |= PyPARSE_TYPE_COMMENTS;
    }
    если (flags->cf_flags & PyCF_ALLOW_INCOMPLETE_INPUT) {
        parser_flags |= PyPARSE_ALLOW_INCOMPLETE_INPUT;
    }
    вернуть parser_flags;
}

// Parser API

Parser *
_PyPegen_Parser_New(struct tok_state *tok, целое start_rule, целое flags,
                    целое feature_version, целое *errcode, const char* source, PyArena *arena)
{
    Parser *p = PyMem_Malloc(sizeof(Parser));
    если (p == NULL) {
        вернуть (Parser *) PyErr_NoMemory();
    }
    assert(tok != NULL);
    _PyTokenizer_SetOptions(tok, 0, (flags & PyPARSE_TYPE_COMMENTS) > 0);
    _PyTokenizer_AllowIncompleteInput(tok, flags & PyPARSE_ALLOW_INCOMPLETE_INPUT);
    p->tok = tok;
    p->keywords = NULL;
    p->n_keyword_lists = -1;
    p->soft_keywords = NULL;
    p->tokens = PyMem_Malloc(sizeof(Token *));
    если (!p->tokens) {
        PyMem_Free(p);
        вернуть (Parser *) PyErr_NoMemory();
    }
    p->tokens[0] = PyMem_Calloc(1, sizeof(Token));
    если (!p->tokens[0]) {
        PyMem_Free(p->tokens);
        PyMem_Free(p);
        вернуть (Parser *) PyErr_NoMemory();
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
#если _Py_STACK_GROWS_DOWN
    p->stack_soft_limit += _PyOS_STACK_MARGIN_BYTES;
#иначе
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
    вернуть p;
}

void
_PyPegen_Parser_Free(Parser *p)
{
    PyMem_Free(p->identifier_cache);
    Py_XDECREF(p->normalize);
    // Resizes allocate blocks starting at indices 1, 2, 4, and so on.
    PyMem_Free(p->tokens[0]);
    для (целое i = 1; i < p->size; i *= PEGEN_ARRAY_GROWTH_FACTOR) {
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
    для (целое i = 0; i < p->fill; i++) {
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
    если (!exc || !PyObject_TypeCheck(exc, (PyTypeObject *)PyExc_SyntaxError)) {
        PyErr_SetRaisedException(exc);
        вернуть;
    }
    const char *source = _PyTokenizer_RetainedSource(p->tok);
    const char *encoding = _PyTokenizer_GetInfo(p->tok).encoding;
    PyObject* the_source = NULL;
    если (source) {
        если (encoding == NULL) {
            the_source = PyUnicode_FromString(source);
        } иначе {
            the_source = PyUnicode_Decode(source, strlen(source), encoding, NULL);
        }
    }
    если (!the_source) {
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
    если (!metadata) {
        PyErr_Clear();
        PyErr_SetRaisedException(exc);
        вернуть;
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
    если (res != NULL && PyErr_Occurred()) {
        // Discard a result returned with an exception still pending
        // (e.g. a MemoryError from a recovered-from allocation failure).
        вернуть NULL;
    }
    если (res == NULL) {
        если (PyErr_Occurred() &&
            (PyErr_ExceptionMatches(PyExc_IncompleteInputError) ||
             !PyErr_ExceptionMatches(PyExc_SyntaxError))) {
            вернуть NULL;
        }
        если ((p->flags & PyPARSE_ALLOW_INCOMPLETE_INPUT) &&
            p->input_exhausted && !PyErr_Occurred()) {
            вернуть _PyPegen_raise_incomplete_input(p);
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
        если (PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_set_syntax_error_metadata(p);
        }
       вернуть NULL;
    }

    если (p->start_rule == Py_single_input && _PyTokenizer_HasTrailingStatement(p->tok)) {
        вернуть RAISE_SYNTAX_ERROR("multiple statements found while compiling a single statement");
    }

    // test_peg_generator defines _Py_TEST_PEGEN to not call PyAST_Validate()
#если defined(Py_DEBUG) && !defined(_Py_TEST_PEGEN)
    если (p->start_rule == Py_single_input ||
        p->start_rule == Py_file_input ||
        p->start_rule == Py_eval_input)
    {
        если (!_PyAST_Validate(res)) {
            вернуть NULL;
        }
    }
#endif
    вернуть res;
}

mod_ty
_PyPegen_run_parser_from_file_pointer(FILE *fp, целое start_rule, PyObject *filename_ob,
                             const char *enc, const char *ps1, const char *ps2,
                             PyCompilerFlags *flags, целое *errcode,
                             PyObject **interactive_src, PyArena *arena)
{
    целое parser_flags = compute_parser_flags(flags);
    struct tok_state *tok = _PyTokenizer_FromFile(fp, enc, ps1, ps2);
    если (tok == NULL) {
        если (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        иначе {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        вернуть NULL;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    // From here on we need to clean up even if there's an error
    mod_ty result = NULL;

    PyObject *module = PyUnicode_FromString("__main__");
    если (module == NULL) {
        goto error;
    }
    _PyTokenizer_SetContext(tok, filename_ob, module);
    Py_DECREF(module);

    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, PY_MINOR_VERSION,
                                    errcode, NULL, arena);
    если (p == NULL) {
        goto error;
    }

    result = _PyPegen_run_parser(p);
    _PyPegen_Parser_Free(p);

    const char *source = _PyTokenizer_RetainedSource(tok);
    если (source != NULL && result && interactive_src != NULL) {
        *interactive_src = PyUnicode_FromString(source);
        если (!*interactive_src || _PyArena_AddPyObject(arena, *interactive_src) < 0) {
            Py_XDECREF(*interactive_src);
            result = NULL;
            goto error;
        }
    }

error:
    _PyTokenizer_Free(tok);
    вернуть result;
}

mod_ty
_PyPegen_run_parser_from_string(const char *str, целое start_rule, PyObject *filename_ob,
                       PyCompilerFlags *flags, PyArena *arena, PyObject *module,
                       PyObject **incomplete)
{
    _PyTokenizer_Newline newline_mode = start_rule == Py_file_input
        ? _PyTokenizer_IMPLY_NEWLINE : _PyTokenizer_KEEP_NEWLINE;
    целое parser_flags = compute_parser_flags(flags);
    если (parser_flags & PyPARSE_ALLOW_INCOMPLETE_INPUT) {
        newline_mode = _PyTokenizer_APPEND_NEWLINE;
    }

    struct tok_state *tok;
    если (flags != NULL && flags->cf_flags & PyCF_IGNORE_COOKIE) {
        tok = _PyTokenizer_FromUTF8(str, newline_mode, 0);
    } иначе {
        tok = _PyTokenizer_FromString(str, newline_mode, 0);
    }
    если (tok == NULL) {
        если (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        иначе {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        вернуть NULL;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    _PyTokenizer_SetContext(tok, filename_ob, module);

    // We need to clear up from here on
    mod_ty result = NULL;

    целое feature_version = flags && (flags->cf_flags & PyCF_ONLY_AST) ?
        flags->cf_feature_version : PY_MINOR_VERSION;
    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, feature_version,
                                    NULL, str, arena);
    если (p == NULL) {
        goto error;
    }

    result = _PyPegen_run_parser(p);
    если (result != NULL && (parser_flags & PyPARSE_ALLOW_INCOMPLETE_INPUT)) {
        целое needs_newline = _PyTokenizer_GetNewlineRequirements(tok);
        если ((needs_newline & _PyTokenizer_NEWLINE_CONTINUATION) ||
            (start_rule != Py_file_input &&
             (needs_newline & _PyTokenizer_NEWLINE_INDENT)) ||
            (start_rule == Py_single_input && p->input_exhausted &&
             (needs_newline & _PyTokenizer_NEWLINE_STATEMENT))) {
            _PyPegen_raise_incomplete_input(p);
            если (incomplete != NULL && PyErr_ExceptionMatches(PyExc_IncompleteInputError)) {
                *incomplete = PyErr_GetRaisedException();
            }
            иначе {
                result = NULL;
            }
        }
    }
    _PyPegen_Parser_Free(p);

error:
    _PyTokenizer_Free(tok);
    вернуть result;
}
