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
    ຖ້າ (p->errcode) {
        *(p->errcode) = E_EOF;
    }
    ສົ່ງຄືນ NULL;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset_line(PyObject *line, Py_ssize_t col_offset, Py_ssize_t end_col_offset)
{
    const unsigned var_na *data = (const unsigned var_na*)PyUnicode_AsUTF8(line);
    ຖ້າ (data == NULL) {
        ສົ່ງຄືນ -1;
    }

    Py_ssize_t len = 0;
    ໃນຂະນະທີ່ (col_offset < end_col_offset) {
        Py_UCS4 ch = data[col_offset];
        ຖ້າ (ch < 0x80) {
            col_offset += 1;
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ ((ch & 0xe0) == 0xc0) {
            col_offset += 2;
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ ((ch & 0xf0) == 0xe0) {
            col_offset += 3;
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ ((ch & 0xf8) == 0xf0) {
            col_offset += 4;
        } ບໍ່ດັ່ງນັ້ນ {
            PyErr_SetString(PyExc_ValueError, "Invalid UTF-8 sequence");
            ສົ່ງຄືນ -1;
        }
        len++;
    }
    ສົ່ງຄືນ len;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset_raw(const var_na* str, Py_ssize_t col_offset)
{
    Py_ssize_t len = (Py_ssize_t)strlen(str);
    ຖ້າ (col_offset > len + 1) {
        col_offset = len + 1;
    }
    assert(col_offset >= 0);
    PyObject *text = PyUnicode_DecodeUTF8(str, col_offset, "replace");
    ຖ້າ (!text) {
        ສົ່ງຄືນ -1;
    }
    Py_ssize_t size = PyUnicode_GET_LENGTH(text);
    Py_DECREF(text);
    ສົ່ງຄືນ size;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset(PyObject *line, Py_ssize_t col_offset)
{
    const var_na *str = PyUnicode_AsUTF8(line);
    ຖ້າ (!str) {
        ສົ່ງຄືນ -1;
    }
    ສົ່ງຄືນ _PyPegen_byte_offset_to_character_offset_raw(str, col_offset);
}

// Here, mark is the start of the node, while p->mark is the end.
// If node==NULL, they should be the same.
ຈຳນວນເຕັມ
_PyPegen_insert_memo(Parser *p, ຈຳນວນເຕັມ mark, ຈຳນວນເຕັມ type, void *node)
{
    // Insert in front
    Memo *m = _PyArena_Malloc(p->arena, parimaa_na(Memo));
    ຖ້າ (m == NULL) {
        ສົ່ງຄືນ -1;
    }
    m->type = type;
    m->node = node;
    m->mark = p->mark;
    m->next = p->tokens[mark]->memo;
    p->tokens[mark]->memo = m;
    p->tokens[mark]->memo_mask |= 1ULL << (type & 63);
    ສົ່ງຄືນ 0;
}

// Like _PyPegen_insert_memo(), but returns the inserted Memo so callers
// can update it in place without re-walking the token's memo list.
Memo *
_PyPegen_insert_memo_direct(Parser *p, ຈຳນວນເຕັມ mark, ຈຳນວນເຕັມ type)
{
    ຖ້າ (_PyPegen_insert_memo(p, mark, type, NULL) < 0) {
        ສົ່ງຄືນ NULL;
    }
    ສົ່ງຄືນ p->tokens[mark]->memo;
}

// Like _PyPegen_insert_memo(), but updates an existing node if found.
ຈຳນວນເຕັມ
_PyPegen_update_memo(Parser *p, ຈຳນວນເຕັມ mark, ຈຳນວນເຕັມ type, void *node)
{
    ສຳລັບ (Memo *m = p->tokens[mark]->memo; m != NULL; m = m->next) {
        ຖ້າ (m->type == type) {
            // Update existing node.
            m->node = node;
            m->mark = p->mark;
            ສົ່ງຄືນ 0;
        }
    }
    // Insert new node.
    ສົ່ງຄືນ _PyPegen_insert_memo(p, mark, type, node);
}

static ຈຳນວນເຕັມ
init_normalization(Parser *p)
{
    ຖ້າ (p->normalize) {
        ສົ່ງຄືນ 1;
    }
    p->normalize = PyImport_ImportModuleAttrString("unicodedata", "normalize");
    ຖ້າ (!p->normalize)
    {
        ສົ່ງຄືນ 0;
    }
    ສົ່ງຄືນ 1;
}

ຈຳນວນເຕັມ
_PyPegen_grow_loop_buffer(void ***buffer, Py_ssize_t *capacity)
{
    ຖ້າ ((size_t)*capacity > (size_t)PY_SSIZE_T_MAX /
            (PEGEN_ARRAY_GROWTH_FACTOR * parimaa_na(**buffer))) {
        ສົ່ງຄືນ -1;
    }
    Py_ssize_t new_capacity = *capacity == 0
        ? 1 : *capacity * PEGEN_ARRAY_GROWTH_FACTOR;
    void **new_buffer = PyMem_Realloc(*buffer, new_capacity * parimaa_na(**buffer));
    ຖ້າ (new_buffer == NULL) {
        ສົ່ງຄືນ -1;
    }
    *buffer = new_buffer;
    *capacity = new_capacity;
    ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ
growable_comment_array_add(growable_comment_array *arr, ຈຳນວນເຕັມ lineno, var_na *comment)
{
    ຖ້າ (arr->num_items >= arr->size) {
        ຖ້າ (arr->size > (size_t)PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * parimaa_na(*arr->items))) {
            ສົ່ງຄືນ 0;
        }
        size_t new_size = arr->size == 0
            ? TYPE_IGNORE_INITIAL_CAPACITY
            : arr->size * PEGEN_ARRAY_GROWTH_FACTOR;
        void *new_items_array = PyMem_Realloc(
            arr->items, new_size * parimaa_na(*arr->items));
        ຖ້າ (!new_items_array) {
            ສົ່ງຄືນ 0;
        }
        arr->items = new_items_array;
        arr->size = new_size;
    }

    arr->items[arr->num_items].lineno = lineno;
    arr->items[arr->num_items].comment = comment;  // Take ownership
    arr->num_items++;
    ສົ່ງຄືນ 1;
}

static void
growable_comment_array_deallocate(growable_comment_array *arr)
{
    ສຳລັບ (size_t i = 0; i < arr->num_items; i++) {
        PyMem_Free(arr->items[i].comment);
    }
    PyMem_Free(arr->items);
}

static ຈຳນວນເຕັມ
_get_keyword_or_name_type(Parser *p, const var_na *text, Py_ssize_t length)
{
    assert(length > 0);

    ຖ້າ (length >= p->n_keyword_lists ||
        p->keywords[length] == NULL ||
        p->keywords[length]->type == -1) {
        ສົ່ງຄືນ NAME;
    }
    ສຳລັບ (KeywordToken *k = p->keywords[length]; k != NULL && k->type != -1; k++) {
        ຖ້າ (memcmp(k->str, text, (size_t)length) == 0) {
            ສົ່ງຄືນ k->type;
        }
    }
    ສົ່ງຄືນ NAME;
}

// Keep text only where grammar actions or helpers read it. Keyword error
// actions use fixed spellings, so keywords do not need their text.
static anta_hstha ຈຳນວນເຕັມ
token_needs_text(ຈຳນວນເຕັມ type)
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
            ສົ່ງຄືນ 1;
        default:
            ສົ່ງຄືນ 0;
    }
}

static ຈຳນວນເຕັມ
initialize_token(Parser *p, Token *parser_token, sa_nracanaa token *new_token, ຈຳນວນເຕັມ token_type) {
    assert(parser_token != NULL);

    parser_token->type = token_type;
    parser_token->bytes = NULL;
    ຖ້າ (token_needs_text(token_type) && new_token->span.start >= 0) {
        Py_ssize_t length;
        const var_na *text = _PyToken_TextView(p->tok, new_token, &length);
        ຖ້າ (token_type == NAME) {
            parser_token->type = _get_keyword_or_name_type(p, text, length);
        }
        // A name reclassified as a keyword does not need its text.
        ຖ້າ (parser_token->type == token_type) {
            parser_token->bytes = PyBytes_FromStringAndSize(text, length);
            ຖ້າ (parser_token->bytes == NULL) {
                ສົ່ງຄືນ -1;
            }
            ຖ້າ (_PyArena_AddPyObject(p->arena, parser_token->bytes) < 0) {
                Py_DECREF(parser_token->bytes);
                ສົ່ງຄືນ -1;
            }
        }
    }

    parser_token->metadata = NULL;
    ຖ້າ (new_token->metadata != NULL) {
        ຖ້າ (_PyArena_AddPyObject(p->arena, new_token->metadata) < 0) {
            ສົ່ງຄືນ -1;
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

    ຖ້າ (token_type == ERRORTOKEN && _PyTokenizer_GetStatus(p->tok) == E_DECODE) {
        ສົ່ງຄືນ _Pypegen_raise_decode_error(p);
    }

    ສົ່ງຄືນ (token_type == ERRORTOKEN ? _Pypegen_tokenizer_error(p) : 0);
}

static ຈຳນວນເຕັມ
_resize_tokens_array(Parser *p) {
    ຖ້າ (p->size > INT_MAX / PEGEN_ARRAY_GROWTH_FACTOR ||
            (size_t)p->size > PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * parimaa_na(*p->tokens))) {
        PyErr_NoMemory();
        ສົ່ງຄືນ -1;
    }
    ຈຳນວນເຕັມ newsize = p->size * PEGEN_ARRAY_GROWTH_FACTOR;
    Token **new_tokens = PyMem_Realloc(p->tokens, (size_t)newsize * parimaa_na(Token *));
    ຖ້າ (new_tokens == NULL) {
        PyErr_NoMemory();
        ສົ່ງຄືນ -1;
    }
    p->tokens = new_tokens;

    // Allocate new tokens together without moving existing ones: parser rules
    // may still hold pointers into earlier blocks. Parser_Free frees each block.
    Token *chunk = PyMem_Calloc((size_t)(newsize - p->size), parimaa_na(Token));
    ຖ້າ (chunk == NULL) {
        PyErr_NoMemory();
        ສົ່ງຄືນ -1;
    }
    ສຳລັບ (ຈຳນວນເຕັມ i = p->size; i < newsize; i++) {
        p->tokens[i] = &chunk[i - p->size];
    }
    p->size = newsize;
    ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ
_PyPegen_fill_token(Parser *p)
{
    sa_nracanaa token new_token;
    _PyToken_Init(&new_token);
    _PyTokenizer_Get(p->tok, &new_token);

    // Record and skip '# type: ignore' comments
    ໃນຂະນະທີ່ (new_token.type == TYPE_IGNORE) {
        Py_ssize_t len;
        const var_na *text = _PyToken_TextView(p->tok, &new_token, &len);
        var_na *tag = PyMem_Malloc((size_t)len + 1);
        ຖ້າ (tag == NULL) {
            PyErr_NoMemory();
            goto error;
        }
        memcpy(tag, text, (size_t)len);
        tag[len] = '\0';
        // Ownership of tag passes to the growable array
        ຖ້າ (!growable_comment_array_add(&p->type_ignore_comments, new_token.end_loc.lineno, tag)) {
            PyMem_Free(tag);
            PyErr_NoMemory();
            goto error;
        }
        _PyTokenizer_Get(p->tok, &new_token);
    }

    ຈຳນວນເຕັມ type = new_token.type;
    // EOF can finish an ordinary token. Only a boundary token means that
    // the grammar requested input beyond the last physical line.
    ຖ້າ (type == ENDMARKER ||
        (type == DEDENT && (p->flags & PyPARSE_ALLOW_INCOMPLETE_INPUT) &&
         _PyTokenizer_GetStatus(p->tok) == E_EOF)) {
        p->input_exhausted = 1;
    }

    // If we have reached the end and we are in single input mode we need to insert a newline and reset the parsing
    ຖ້າ (p->start_rule == Py_single_input && type == ENDMARKER && p->parsing_started) {
        type = NEWLINE; /* Add an extra newline */
        p->parsing_started = 0;

        ຖ້າ (!(p->flags & PyPARSE_DONT_IMPLY_DEDENT)) {
            _PyTokenizer_ImplyDedents(p->tok);
        }
    }
    ບໍ່ດັ່ງນັ້ນ {
        p->parsing_started = 1;
    }

    // Check if we are at the limit of the token array capacity and resize if needed
    ຖ້າ ((p->fill == p->size) && (_resize_tokens_array(p) != 0)) {
        goto error;
    }

    Token *t = p->tokens[p->fill];
    ຈຳນວນເຕັມ result = initialize_token(p, t, &new_token, type);
    _PyToken_Free(&new_token);
    ສົ່ງຄືນ result;
error:
    _PyToken_Free(&new_token);
    ສົ່ງຄືນ -1;
}

#ຖ້າ defined(Py_DEBUG)
// Instrumentation to count the effectiveness of memoization.
// The array counts the number of tokens skipped by memoization,
// indexed by type.

#define NSTATISTICS _PYPEGEN_NSTATISTICS
#define memo_statistics _PyRuntime.parser.memo_statistics

void
_PyPegen_clear_memo_statistics(void)
{
    PyMutex_Lock(&_PyRuntime.parser.mutex);
    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < NSTATISTICS; i++) {
        memo_statistics[i] = 0;
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
}

PyObject *
_PyPegen_get_memo_statistics(void)
{
    PyObject *ret = PyList_New(NSTATISTICS);
    ຖ້າ (ret == NULL) {
        ສົ່ງຄືນ NULL;
    }

    PyMutex_Lock(&_PyRuntime.parser.mutex);
    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < NSTATISTICS; i++) {
        PyObject *value = PyLong_FromLong(memo_statistics[i]);
        ຖ້າ (value == NULL) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            ສົ່ງຄືນ NULL;
        }
        // PyList_SetItem borrows a reference to value.
        ຖ້າ (PyList_SetItem(ret, i, value) < 0) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            ສົ່ງຄືນ NULL;
        }
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
    ສົ່ງຄືນ ret;
}
#endif

ຈຳນວນເຕັມ  // bool
_PyPegen_is_memoized(Parser *p, ຈຳນວນເຕັມ type, void *pres)
{
    ຖ້າ (p->mark == p->fill) {
        ຖ້າ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ສົ່ງຄືນ -1;
        }
    }

    Token *t = p->tokens[p->mark];

    ຖ້າ (!(t->memo_mask & (1ULL << (type & 63)))) {
        ສົ່ງຄືນ 0;
    }

    ສຳລັບ (Memo *m = t->memo; m != NULL; m = m->next) {
        ຖ້າ (m->type == type) {
#ຖ້າ defined(Py_DEBUG)
            ຖ້າ (0 <= type && type < NSTATISTICS) {
                long count = m->mark - p->mark;
                // A memoized negative result counts for one.
                ຖ້າ (count <= 0) {
                    count = 1;
                }
                PyMutex_Lock(&_PyRuntime.parser.mutex);
                memo_statistics[type] += count;
                PyMutex_Unlock(&_PyRuntime.parser.mutex);
            }
#endif
            p->mark = m->mark;
            *(void **)(pres) = m->node;
            ສົ່ງຄືນ 1;
        }
    }
    ສົ່ງຄືນ 0;
}

#define LOOKAHEAD1(NAME, RES_TYPE)                                  \
    ຈຳນວນເຕັມ                                                             \
    NAME (ຈຳນວນເຕັມ positive, RES_TYPE (func)(Parser *), Parser *p)       \
    {                                                               \
        ຈຳນວນເຕັມ mark = p->mark;                                         \
        void *res = func(p);                                        \
        p->mark = mark;                                             \
        ສົ່ງຄືນ (res != NULL) == positive;                           \
    }

LOOKAHEAD1(_PyPegen_lookahead, void *)
LOOKAHEAD1(_PyPegen_lookahead_for_expr, expr_ty)
LOOKAHEAD1(_PyPegen_lookahead_for_stmt, stmt_ty)
#undef LOOKAHEAD1

#define LOOKAHEAD2(NAME, RES_TYPE, T)                                   \
    ຈຳນວນເຕັມ                                                                 \
    NAME (ຈຳນວນເຕັມ positive, RES_TYPE (func)(Parser *, T), Parser *p, T arg) \
    {                                                                   \
        ຈຳນວນເຕັມ mark = p->mark;                                             \
        void *res = func(p, arg);                                       \
        p->mark = mark;                                                 \
        ສົ່ງຄືນ (res != NULL) == positive;                               \
    }

LOOKAHEAD2(_PyPegen_lookahead_with_int, Token *, ຈຳນວນເຕັມ)
LOOKAHEAD2(_PyPegen_lookahead_with_string, expr_ty, const var_na *)
#undef LOOKAHEAD2

Token *
_PyPegen_expect_token(Parser *p, ຈຳນວນເຕັມ type)
{
    ຖ້າ (p->mark == p->fill) {
        ຖ້າ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ສົ່ງຄືນ NULL;
        }
    }
    Token *t = p->tokens[p->mark];
    ຖ້າ (t->type != type) {
       ສົ່ງຄືນ NULL;
    }
    p->mark += 1;
    ສົ່ງຄືນ t;
}

void*
_PyPegen_expect_forced_result(Parser *p, void* result, const var_na* expected) {

    ຖ້າ (p->error_indicator == 1) {
        ສົ່ງຄືນ NULL;
    }
    ຖ້າ (result == NULL) {
        RAISE_SYNTAX_ERROR("expected (%s)", expected);
        ສົ່ງຄືນ NULL;
    }
    ສົ່ງຄືນ result;
}

Token *
_PyPegen_expect_forced_token(Parser *p, ຈຳນວນເຕັມ type, const var_na* expected) {

    ຖ້າ (p->error_indicator == 1) {
        ສົ່ງຄືນ NULL;
    }

    ຖ້າ (p->mark == p->fill) {
        ຖ້າ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ສົ່ງຄືນ NULL;
        }
    }
    Token *t = p->tokens[p->mark];
    ຖ້າ (t->type != type) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(t, "expected '%s'", expected);
        ສົ່ງຄືນ NULL;
    }
    p->mark += 1;
    ສົ່ງຄືນ t;
}

expr_ty
_PyPegen_expect_soft_keyword(Parser *p, const var_na *keyword)
{
    ຖ້າ (p->mark == p->fill) {
        ຖ້າ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ສົ່ງຄືນ NULL;
        }
    }
    Token *t = p->tokens[p->mark];
    ຖ້າ (t->type != NAME) {
        ສົ່ງຄືນ NULL;
    }
    const var_na *s = PyBytes_AsString(t->bytes);
    ຖ້າ (!s) {
        p->error_indicator = 1;
        ສົ່ງຄືນ NULL;
    }
    ຖ້າ (strcmp(s, keyword) != 0) {
        ສົ່ງຄືນ NULL;
    }
    ສົ່ງຄືນ _PyPegen_name_token(p);
}

Token *
_PyPegen_get_last_nonnwhitespace_token(Parser *p)
{
    assert(p->mark >= 0);
    Token *token = NULL;
    ສຳລັບ (ຈຳນວນເຕັມ m = p->mark - 1; m >= 0; m--) {
        token = p->tokens[m];
        ຖ້າ (token->type != ENDMARKER && (token->type < NEWLINE || token->type > DEDENT)) {
            bha_nga;
        }
    }
    ສົ່ງຄືນ token;
}

PyObject *
_PyPegen_new_identifier(Parser *p, const var_na *n)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, (Py_ssize_t)strlen(n), NULL);
    ຖ້າ (!id) {
        goto error;
    }
    /* Check whether there are non-ASCII characters in the
       identifier; if so, normalize to NFKC. */
    ຖ້າ (!PyUnicode_IS_ASCII(id))
    {
        ຖ້າ (!init_normalization(p))
        {
            Py_DECREF(id);
            goto error;
        }
        PyObject *form = PyUnicode_InternFromString("NFKC");
        ຖ້າ (form == NULL)
        {
            Py_DECREF(id);
            goto error;
        }
        PyObject *args[2] = {form, id};
        PyObject *id2 = PyObject_Vectorcall(p->normalize, args, 2, NULL);
        Py_DECREF(id);
        Py_DECREF(form);
        ຖ້າ (!id2) {
            goto error;
        }

        ຖ້າ (!PyUnicode_Check(id2))
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
    ສຳລັບ (ຈຳນວນເຕັມ i = 0; forbidden[i] != NULL; i++) {
        ຖ້າ (_PyUnicode_EqualToASCIIString(id, forbidden[i])) {
            PyErr_Format(PyExc_ValueError,
                         "identifier field can't represent '%s' constant",
                         forbidden[i]);
            Py_DECREF(id);
            goto error;
        }
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &id);
    ຖ້າ (_PyArena_AddPyObject(p->arena, id) < 0)
    {
        Py_DECREF(id);
        goto error;
    }
    ສົ່ງຄືນ id;

error:
    p->error_indicator = 1;
    ສົ່ງຄືນ NULL;
}

// Return an arena-owned identifier; callers borrow the reference.
static PyObject *
get_cached_identifier(Parser *p, PyObject *bytes)
{
    const var_na *s = PyBytes_AsString(bytes);
    ຖ້າ (!s) {
        p->error_indicator = 1;
        ສົ່ງຄືນ NULL;
    }
    // Identifiers repeat constantly; a small span-keyed cache skips the
    // UTF-8 decode + intern for repeated occurrences. Keys point into
    // arena-owned token bytes and values are arena-owned interned strings,
    // so borrowed references are valid for the lifetime of the parse
    // (including the second error pass, which reuses parser and arena).
    Py_ssize_t len = PyBytes_GET_SIZE(bytes);
    Py_hash_t hash = PyObject_Hash(bytes);
    ຖ້າ (hash == -1) {
        p->error_indicator = 1;
        ສົ່ງຄືນ NULL;
    }
    // Parses without identifiers do not need this cache.
    ຖ້າ (p->identifier_cache == NULL) {
        p->identifier_cache = PyMem_Calloc(
            IDENTIFIER_CACHE_SIZE, parimaa_na(*p->identifier_cache));
        ຖ້າ (p->identifier_cache == NULL) {
            p->error_indicator = 1;
            ສົ່ງຄືນ PyErr_NoMemory();
        }
    }
    IdentifierCacheEntry *free_slot = NULL;
    size_t idx = (size_t)hash & (IDENTIFIER_CACHE_SIZE - 1);
    ສຳລັບ (ຈຳນວນເຕັມ probe = 0; probe < IDENTIFIER_CACHE_MAX_PROBES; probe++) {
        IdentifierCacheEntry *entry = &p->identifier_cache[
            (idx + probe) & (IDENTIFIER_CACHE_SIZE - 1)];
        ຖ້າ (entry->key == NULL) {
            free_slot = entry;
            bha_nga;
        }
        ຖ້າ (entry->hash == hash && entry->len == len &&
            memcmp(entry->key, s, len) == 0)
        {
            ສົ່ງຄືນ entry->value;
        }
    }
    PyObject *id = _PyPegen_new_identifier(p, s);
    ຖ້າ (id == NULL) {
        p->error_indicator = 1;
        ສົ່ງຄືນ NULL;
    }
    ຖ້າ (free_slot != NULL) {
        free_slot->key = s;
        free_slot->len = len;
        free_slot->hash = hash;
        free_slot->value = id;
    }
    ສົ່ງຄືນ id;
}

static expr_ty
_PyPegen_name_from_token(Parser *p, Token* t)
{
    ຖ້າ (t == NULL) {
        ສົ່ງຄືນ NULL;
    }
    // Reuse the AST node when backtracking revisits this token. Memo lookup
    // starts before the token and restores the position after it on a hit.
    // Token kinds can be memo keys: generated grammar rule IDs start at 1000.
    ຈຳນວນເຕັມ mark = p->mark - 1;
    p->mark = mark;
    expr_ty cached = NULL;
    ຖ້າ (_PyPegen_is_memoized(p, NAME, &cached)) {
        ສົ່ງຄືນ cached;
    }
    p->mark = mark + 1;
    PyObject *id = get_cached_identifier(p, t->bytes);
    ຖ້າ (id == NULL) {
        ສົ່ງຄືນ NULL;
    }
    expr_ty result = _PyAST_Name(id, Load, t->lineno, t->col_offset,
                                t->end_lineno, t->end_col_offset, p->arena);
    ຖ້າ (result != NULL && _PyPegen_insert_memo(p, mark, NAME, result) < 0) {
        p->error_indicator = 1;
        ສົ່ງຄືນ NULL;
    }
    ສົ່ງຄືນ result;
}

expr_ty
_PyPegen_name_token(Parser *p)
{
    Token *t = _PyPegen_expect_token(p, NAME);
    ສົ່ງຄືນ _PyPegen_name_from_token(p, t);
}

void *
_PyPegen_string_token(Parser *p)
{
    ສົ່ງຄືນ _PyPegen_expect_token(p, STRING);
}

expr_ty _PyPegen_soft_keyword_token(Parser *p) {
    Token *t = _PyPegen_expect_token(p, NAME);
    ຖ້າ (t == NULL) {
        ສົ່ງຄືນ NULL;
    }
    var_na *the_token;
    Py_ssize_t size;
    PyBytes_AsStringAndSize(t->bytes, &the_token, &size);
    ສຳລັບ (var_na **keyword = p->soft_keywords; *keyword != NULL; keyword++) {
        ຖ້າ (strlen(*keyword) == (size_t)size &&
            strncmp(*keyword, the_token, (size_t)size) == 0) {
            ສົ່ງຄືນ _PyPegen_name_from_token(p, t);
        }
    }
    ສົ່ງຄືນ NULL;
}

static PyObject *
parsenumber_raw(const var_na *s)
{
    const var_na *end;
    long x;
    dviguu_na dx;
    Py_complex compl;
    ຈຳນວນເຕັມ imflag;

    assert(s != NULL);
    errno = 0;
    end = s + strlen(s) - 1;
    imflag = *end == 'j' || *end == 'J';
    ຖ້າ (s[0] == '0') {
        x = (long)PyOS_strtoul(s, (var_na **)&end, 0);
        ຖ້າ (x < 0 && errno == 0) {
            ສົ່ງຄືນ PyLong_FromString(s, (var_na **)0, 0);
        }
    }
    ບໍ່ດັ່ງນັ້ນ {
        x = PyOS_strtol(s, (var_na **)&end, 0);
    }
    ຖ້າ (*end == '\0') {
        ຖ້າ (errno != 0) {
            ສົ່ງຄືນ PyLong_FromString(s, (var_na **)0, 0);
        }
        ສົ່ງຄືນ PyLong_FromLong(x);
    }
    /* XXX Huge floats may silently fail */
    ຖ້າ (imflag) {
        compl.real = 0.;
        compl.imag = PyOS_string_to_double(s, (var_na **)&end, NULL);
        ຖ້າ (compl.imag == -1.0 && PyErr_Occurred()) {
            ສົ່ງຄືນ NULL;
        }
        ສົ່ງຄືນ PyComplex_FromCComplex(compl);
    }
    dx = PyOS_string_to_double(s, NULL, NULL);
    ຖ້າ (dx == -1.0 && PyErr_Occurred()) {
        ສົ່ງຄືນ NULL;
    }
    ສົ່ງຄືນ PyFloat_FromDouble(dx);
}

static PyObject *
parsenumber(const var_na *s)
{
    var_na *dup;
    var_na *end;
    PyObject *res = NULL;

    assert(s != NULL);

    ຖ້າ (strchr(s, '_') == NULL) {
        ສົ່ງຄືນ parsenumber_raw(s);
    }
    /* Create a duplicate without underscores. */
    dup = PyMem_Malloc(strlen(s) + 1);
    ຖ້າ (dup == NULL) {
        ສົ່ງຄືນ PyErr_NoMemory();
    }
    end = dup;
    ສຳລັບ (; *s; s++) {
        ຖ້າ (*s != '_') {
            *end++ = *s;
        }
    }
    *end = '\0';
    res = parsenumber_raw(dup);
    PyMem_Free(dup);
    ສົ່ງຄືນ res;
}

expr_ty
_PyPegen_number_token(Parser *p)
{
    ຈຳນວນເຕັມ mark = p->mark;
    Token *t = _PyPegen_expect_token(p, NUMBER);
    ຖ້າ (t == NULL) {
        ສົ່ງຄືນ NULL;
    }

    // Look up the node at the token's start, as for NAME tokens. A memo hit
    // advances the mark; on a miss, restore the position after the token.
    p->mark = mark;
    expr_ty cached = NULL;
    ຖ້າ (_PyPegen_is_memoized(p, NUMBER, &cached)) {
        ສົ່ງຄືນ cached;
    }
    p->mark = mark + 1;

    const var_na *num_raw = PyBytes_AsString(t->bytes);
    ຖ້າ (num_raw == NULL) {
        p->error_indicator = 1;
        ສົ່ງຄືນ NULL;
    }

    ຖ້າ (p->feature_version < 6 && strchr(num_raw, '_') != NULL) {
        p->error_indicator = 1;
        ສົ່ງຄືນ RAISE_SYNTAX_ERROR("Underscores in numeric literals are only supported "
                                  "in Python 3.6 and greater");
    }

    PyObject *c = parsenumber(num_raw);

    ຖ້າ (c == NULL) {
        p->error_indicator = 1;
        PyThreadState *tstate = _PyThreadState_GET();
        // The only way a ValueError should happen in _this_ code is via
        // PyLong_FromString hitting a length limit.
        ຖ້າ (tstate->current_exception != NULL &&
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
        ສົ່ງຄືນ NULL;
    }

    ຖ້າ (_PyArena_AddPyObject(p->arena, c) < 0) {
        Py_DECREF(c);
        p->error_indicator = 1;
        ສົ່ງຄືນ NULL;
    }

    expr_ty result = _PyAST_Constant(c, NULL, t->lineno, t->col_offset,
                                    t->end_lineno, t->end_col_offset, p->arena);
    ຖ້າ (result != NULL && _PyPegen_insert_memo(p, mark, NUMBER, result) < 0) {
        p->error_indicator = 1;
        ສົ່ງຄືນ NULL;
    }
    ສົ່ງຄືນ result;
}



static ຈຳນວນເຕັມ
compute_parser_flags(PyCompilerFlags *flags)
{
    ຈຳນວນເຕັມ parser_flags = 0;
    ຖ້າ (!flags) {
        ສົ່ງຄືນ 0;
    }
    ຖ້າ (flags->cf_flags & PyCF_DONT_IMPLY_DEDENT) {
        parser_flags |= PyPARSE_DONT_IMPLY_DEDENT;
    }
    ຖ້າ (flags->cf_flags & PyCF_IGNORE_COOKIE) {
        parser_flags |= PyPARSE_IGNORE_COOKIE;
    }
    ຖ້າ (flags->cf_flags & CO_FUTURE_BARRY_AS_BDFL) {
        parser_flags |= PyPARSE_BARRY_AS_BDFL;
    }
    ຖ້າ (flags->cf_flags & PyCF_TYPE_COMMENTS) {
        parser_flags |= PyPARSE_TYPE_COMMENTS;
    }
    ຖ້າ (flags->cf_flags & PyCF_ALLOW_INCOMPLETE_INPUT) {
        parser_flags |= PyPARSE_ALLOW_INCOMPLETE_INPUT;
    }
    ສົ່ງຄືນ parser_flags;
}

// Parser API

Parser *
_PyPegen_Parser_New(sa_nracanaa tok_state *tok, ຈຳນວນເຕັມ start_rule, ຈຳນວນເຕັມ flags,
                    ຈຳນວນເຕັມ feature_version, ຈຳນວນເຕັມ *errcode, const var_na* source, PyArena *arena)
{
    Parser *p = PyMem_Malloc(parimaa_na(Parser));
    ຖ້າ (p == NULL) {
        ສົ່ງຄືນ (Parser *) PyErr_NoMemory();
    }
    assert(tok != NULL);
    _PyTokenizer_SetOptions(tok, 0, (flags & PyPARSE_TYPE_COMMENTS) > 0);
    _PyTokenizer_AllowIncompleteInput(tok, flags & PyPARSE_ALLOW_INCOMPLETE_INPUT);
    p->tok = tok;
    p->keywords = NULL;
    p->n_keyword_lists = -1;
    p->soft_keywords = NULL;
    p->tokens = PyMem_Malloc(parimaa_na(Token *));
    ຖ້າ (!p->tokens) {
        PyMem_Free(p);
        ສົ່ງຄືນ (Parser *) PyErr_NoMemory();
    }
    p->tokens[0] = PyMem_Calloc(1, parimaa_na(Token));
    ຖ້າ (!p->tokens[0]) {
        PyMem_Free(p->tokens);
        PyMem_Free(p);
        ສົ່ງຄືນ (Parser *) PyErr_NoMemory();
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
#ຖ້າ _Py_STACK_GROWS_DOWN
    p->stack_soft_limit += _PyOS_STACK_MARGIN_BYTES;
#ບໍ່ດັ່ງນັ້ນ
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
    ສົ່ງຄືນ p;
}

void
_PyPegen_Parser_Free(Parser *p)
{
    PyMem_Free(p->identifier_cache);
    Py_XDECREF(p->normalize);
    // Resizes allocate blocks starting at indices 1, 2, 4, and so on.
    PyMem_Free(p->tokens[0]);
    ສຳລັບ (ຈຳນວນເຕັມ i = 1; i < p->size; i *= PEGEN_ARRAY_GROWTH_FACTOR) {
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
    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < p->fill; i++) {
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
    ຖ້າ (!exc || !PyObject_TypeCheck(exc, (PyTypeObject *)PyExc_SyntaxError)) {
        PyErr_SetRaisedException(exc);
        ສົ່ງຄືນ;
    }
    const var_na *source = _PyTokenizer_RetainedSource(p->tok);
    const var_na *encoding = _PyTokenizer_GetInfo(p->tok).encoding;
    PyObject* the_source = NULL;
    ຖ້າ (source) {
        ຖ້າ (encoding == NULL) {
            the_source = PyUnicode_FromString(source);
        } ບໍ່ດັ່ງນັ້ນ {
            the_source = PyUnicode_Decode(source, strlen(source), encoding, NULL);
        }
    }
    ຖ້າ (!the_source) {
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
    ຖ້າ (!metadata) {
        PyErr_Clear();
        PyErr_SetRaisedException(exc);
        ສົ່ງຄືນ;
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
    ຖ້າ (res != NULL && PyErr_Occurred()) {
        // Discard a result returned with an exception still pending
        // (e.g. a MemoryError from a recovered-from allocation failure).
        ສົ່ງຄືນ NULL;
    }
    ຖ້າ (res == NULL) {
        ຖ້າ (PyErr_Occurred() &&
            (PyErr_ExceptionMatches(PyExc_IncompleteInputError) ||
             !PyErr_ExceptionMatches(PyExc_SyntaxError))) {
            ສົ່ງຄືນ NULL;
        }
        ຖ້າ ((p->flags & PyPARSE_ALLOW_INCOMPLETE_INPUT) &&
            p->input_exhausted && !PyErr_Occurred()) {
            ສົ່ງຄືນ _PyPegen_raise_incomplete_input(p);
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
        ຖ້າ (PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_set_syntax_error_metadata(p);
        }
       ສົ່ງຄືນ NULL;
    }

    ຖ້າ (p->start_rule == Py_single_input && _PyTokenizer_HasTrailingStatement(p->tok)) {
        ສົ່ງຄືນ RAISE_SYNTAX_ERROR("multiple statements found while compiling a single statement");
    }

    // test_peg_generator defines _Py_TEST_PEGEN to not call PyAST_Validate()
#ຖ້າ defined(Py_DEBUG) && !defined(_Py_TEST_PEGEN)
    ຖ້າ (p->start_rule == Py_single_input ||
        p->start_rule == Py_file_input ||
        p->start_rule == Py_eval_input)
    {
        ຖ້າ (!_PyAST_Validate(res)) {
            ສົ່ງຄືນ NULL;
        }
    }
#endif
    ສົ່ງຄືນ res;
}

mod_ty
_PyPegen_run_parser_from_file_pointer(FILE *fp, ຈຳນວນເຕັມ start_rule, PyObject *filename_ob,
                             const var_na *enc, const var_na *ps1, const var_na *ps2,
                             PyCompilerFlags *flags, ຈຳນວນເຕັມ *errcode,
                             PyObject **interactive_src, PyArena *arena)
{
    ຈຳນວນເຕັມ parser_flags = compute_parser_flags(flags);
    sa_nracanaa tok_state *tok = _PyTokenizer_FromFile(fp, enc, ps1, ps2);
    ຖ້າ (tok == NULL) {
        ຖ້າ (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        ບໍ່ດັ່ງນັ້ນ {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        ສົ່ງຄືນ NULL;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    // From here on we need to clean up even if there's an error
    mod_ty result = NULL;

    PyObject *module = PyUnicode_FromString("__main__");
    ຖ້າ (module == NULL) {
        goto error;
    }
    _PyTokenizer_SetContext(tok, filename_ob, module);
    Py_DECREF(module);

    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, PY_MINOR_VERSION,
                                    errcode, NULL, arena);
    ຖ້າ (p == NULL) {
        goto error;
    }

    result = _PyPegen_run_parser(p);
    _PyPegen_Parser_Free(p);

    const var_na *source = _PyTokenizer_RetainedSource(tok);
    ຖ້າ (source != NULL && result && interactive_src != NULL) {
        *interactive_src = PyUnicode_FromString(source);
        ຖ້າ (!*interactive_src || _PyArena_AddPyObject(arena, *interactive_src) < 0) {
            Py_XDECREF(*interactive_src);
            result = NULL;
            goto error;
        }
    }

error:
    _PyTokenizer_Free(tok);
    ສົ່ງຄືນ result;
}

mod_ty
_PyPegen_run_parser_from_string(const var_na *str, ຈຳນວນເຕັມ start_rule, PyObject *filename_ob,
                       PyCompilerFlags *flags, PyArena *arena, PyObject *module,
                       PyObject **incomplete)
{
    _PyTokenizer_Newline newline_mode = start_rule == Py_file_input
        ? _PyTokenizer_IMPLY_NEWLINE : _PyTokenizer_KEEP_NEWLINE;
    ຈຳນວນເຕັມ parser_flags = compute_parser_flags(flags);
    ຖ້າ (parser_flags & PyPARSE_ALLOW_INCOMPLETE_INPUT) {
        newline_mode = _PyTokenizer_APPEND_NEWLINE;
    }

    sa_nracanaa tok_state *tok;
    ຖ້າ (flags != NULL && flags->cf_flags & PyCF_IGNORE_COOKIE) {
        tok = _PyTokenizer_FromUTF8(str, newline_mode, 0);
    } ບໍ່ດັ່ງນັ້ນ {
        tok = _PyTokenizer_FromString(str, newline_mode, 0);
    }
    ຖ້າ (tok == NULL) {
        ຖ້າ (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        ບໍ່ດັ່ງນັ້ນ {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        ສົ່ງຄືນ NULL;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    _PyTokenizer_SetContext(tok, filename_ob, module);

    // We need to clear up from here on
    mod_ty result = NULL;

    ຈຳນວນເຕັມ feature_version = flags && (flags->cf_flags & PyCF_ONLY_AST) ?
        flags->cf_feature_version : PY_MINOR_VERSION;
    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, feature_version,
                                    NULL, str, arena);
    ຖ້າ (p == NULL) {
        goto error;
    }

    result = _PyPegen_run_parser(p);
    ຖ້າ (result != NULL && (parser_flags & PyPARSE_ALLOW_INCOMPLETE_INPUT)) {
        ຈຳນວນເຕັມ needs_newline = _PyTokenizer_GetNewlineRequirements(tok);
        ຖ້າ ((needs_newline & _PyTokenizer_NEWLINE_CONTINUATION) ||
            (start_rule != Py_file_input &&
             (needs_newline & _PyTokenizer_NEWLINE_INDENT)) ||
            (start_rule == Py_single_input && p->input_exhausted &&
             (needs_newline & _PyTokenizer_NEWLINE_STATEMENT))) {
            _PyPegen_raise_incomplete_input(p);
            ຖ້າ (incomplete != NULL && PyErr_ExceptionMatches(PyExc_IncompleteInputError)) {
                *incomplete = PyErr_GetRaisedException();
            }
            ບໍ່ດັ່ງນັ້ນ {
                result = NULL;
            }
        }
    }
    _PyPegen_Parser_Free(p);

error:
    _PyTokenizer_Free(tok);
    ສົ່ງຄືນ result;
}
