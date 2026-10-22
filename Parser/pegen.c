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
    အကယ်၍ (p->errcode) {
        *(p->errcode) = E_EOF;
    }
    ပြန်ပေး NULL;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset_line(PyObject *line, Py_ssize_t col_offset, Py_ssize_t end_col_offset)
{
    const unsigned var_na *data = (const unsigned var_na*)PyUnicode_AsUTF8(line);
    အကယ်၍ (data == NULL) {
        ပြန်ပေး -1;
    }

    Py_ssize_t len = 0;
    နေစဉ် (col_offset < end_col_offset) {
        Py_UCS4 ch = data[col_offset];
        အကယ်၍ (ch < 0x80) {
            col_offset += 1;
        } မဟုတ်ပါက အကယ်၍ ((ch & 0xe0) == 0xc0) {
            col_offset += 2;
        } မဟုတ်ပါက အကယ်၍ ((ch & 0xf0) == 0xe0) {
            col_offset += 3;
        } မဟုတ်ပါက အကယ်၍ ((ch & 0xf8) == 0xf0) {
            col_offset += 4;
        } မဟုတ်ပါက {
            PyErr_SetString(PyExc_ValueError, "Invalid UTF-8 sequence");
            ပြန်ပေး -1;
        }
        len++;
    }
    ပြန်ပေး len;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset_raw(const var_na* str, Py_ssize_t col_offset)
{
    Py_ssize_t len = (Py_ssize_t)strlen(str);
    အကယ်၍ (col_offset > len + 1) {
        col_offset = len + 1;
    }
    assert(col_offset >= 0);
    PyObject *text = PyUnicode_DecodeUTF8(str, col_offset, "replace");
    အကယ်၍ (!text) {
        ပြန်ပေး -1;
    }
    Py_ssize_t size = PyUnicode_GET_LENGTH(text);
    Py_DECREF(text);
    ပြန်ပေး size;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset(PyObject *line, Py_ssize_t col_offset)
{
    const var_na *str = PyUnicode_AsUTF8(line);
    အကယ်၍ (!str) {
        ပြန်ပေး -1;
    }
    ပြန်ပေး _PyPegen_byte_offset_to_character_offset_raw(str, col_offset);
}

// Here, mark is the start of the node, while p->mark is the end.
// If node==NULL, they should be the same.
ကိန်းပြည့်
_PyPegen_insert_memo(Parser *p, ကိန်းပြည့် mark, ကိန်းပြည့် type, void *node)
{
    // Insert in front
    Memo *m = _PyArena_Malloc(p->arena, parimaa_na(Memo));
    အကယ်၍ (m == NULL) {
        ပြန်ပေး -1;
    }
    m->type = type;
    m->node = node;
    m->mark = p->mark;
    m->next = p->tokens[mark]->memo;
    p->tokens[mark]->memo = m;
    p->tokens[mark]->memo_mask |= 1ULL << (type & 63);
    ပြန်ပေး 0;
}

// Like _PyPegen_insert_memo(), but returns the inserted Memo so callers
// can update it in place without re-walking the token's memo list.
Memo *
_PyPegen_insert_memo_direct(Parser *p, ကိန်းပြည့် mark, ကိန်းပြည့် type)
{
    အကယ်၍ (_PyPegen_insert_memo(p, mark, type, NULL) < 0) {
        ပြန်ပေး NULL;
    }
    ပြန်ပေး p->tokens[mark]->memo;
}

// Like _PyPegen_insert_memo(), but updates an existing node if found.
ကိန်းပြည့်
_PyPegen_update_memo(Parser *p, ကိန်းပြည့် mark, ကိန်းပြည့် type, void *node)
{
    အတွက် (Memo *m = p->tokens[mark]->memo; m != NULL; m = m->next) {
        အကယ်၍ (m->type == type) {
            // Update existing node.
            m->node = node;
            m->mark = p->mark;
            ပြန်ပေး 0;
        }
    }
    // Insert new node.
    ပြန်ပေး _PyPegen_insert_memo(p, mark, type, node);
}

static ကိန်းပြည့်
init_normalization(Parser *p)
{
    အကယ်၍ (p->normalize) {
        ပြန်ပေး 1;
    }
    p->normalize = PyImport_ImportModuleAttrString("unicodedata", "normalize");
    အကယ်၍ (!p->normalize)
    {
        ပြန်ပေး 0;
    }
    ပြန်ပေး 1;
}

ကိန်းပြည့်
_PyPegen_grow_loop_buffer(void ***buffer, Py_ssize_t *capacity)
{
    အကယ်၍ ((size_t)*capacity > (size_t)PY_SSIZE_T_MAX /
            (PEGEN_ARRAY_GROWTH_FACTOR * parimaa_na(**buffer))) {
        ပြန်ပေး -1;
    }
    Py_ssize_t new_capacity = *capacity == 0
        ? 1 : *capacity * PEGEN_ARRAY_GROWTH_FACTOR;
    void **new_buffer = PyMem_Realloc(*buffer, new_capacity * parimaa_na(**buffer));
    အကယ်၍ (new_buffer == NULL) {
        ပြန်ပေး -1;
    }
    *buffer = new_buffer;
    *capacity = new_capacity;
    ပြန်ပေး 0;
}

static ကိန်းပြည့်
growable_comment_array_add(growable_comment_array *arr, ကိန်းပြည့် lineno, var_na *comment)
{
    အကယ်၍ (arr->num_items >= arr->size) {
        အကယ်၍ (arr->size > (size_t)PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * parimaa_na(*arr->items))) {
            ပြန်ပေး 0;
        }
        size_t new_size = arr->size == 0
            ? TYPE_IGNORE_INITIAL_CAPACITY
            : arr->size * PEGEN_ARRAY_GROWTH_FACTOR;
        void *new_items_array = PyMem_Realloc(
            arr->items, new_size * parimaa_na(*arr->items));
        အကယ်၍ (!new_items_array) {
            ပြန်ပေး 0;
        }
        arr->items = new_items_array;
        arr->size = new_size;
    }

    arr->items[arr->num_items].lineno = lineno;
    arr->items[arr->num_items].comment = comment;  // Take ownership
    arr->num_items++;
    ပြန်ပေး 1;
}

static void
growable_comment_array_deallocate(growable_comment_array *arr)
{
    အတွက် (size_t i = 0; i < arr->num_items; i++) {
        PyMem_Free(arr->items[i].comment);
    }
    PyMem_Free(arr->items);
}

static ကိန်းပြည့်
_get_keyword_or_name_type(Parser *p, const var_na *text, Py_ssize_t length)
{
    assert(length > 0);

    အကယ်၍ (length >= p->n_keyword_lists ||
        p->keywords[length] == NULL ||
        p->keywords[length]->type == -1) {
        ပြန်ပေး NAME;
    }
    အတွက် (KeywordToken *k = p->keywords[length]; k != NULL && k->type != -1; k++) {
        အကယ်၍ (memcmp(k->str, text, (size_t)length) == 0) {
            ပြန်ပေး k->type;
        }
    }
    ပြန်ပေး NAME;
}

// Keep text only where grammar actions or helpers read it. Keyword error
// actions use fixed spellings, so keywords do not need their text.
static anta_hstha ကိန်းပြည့်
token_needs_text(ကိန်းပြည့် type)
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
            ပြန်ပေး 1;
        default:
            ပြန်ပေး 0;
    }
}

static ကိန်းပြည့်
initialize_token(Parser *p, Token *parser_token, sa_nracanaa token *new_token, ကိန်းပြည့် token_type) {
    assert(parser_token != NULL);

    parser_token->type = token_type;
    parser_token->bytes = NULL;
    အကယ်၍ (token_needs_text(token_type) && new_token->span.start >= 0) {
        Py_ssize_t length;
        const var_na *text = _PyToken_TextView(p->tok, new_token, &length);
        အကယ်၍ (token_type == NAME) {
            parser_token->type = _get_keyword_or_name_type(p, text, length);
        }
        // A name reclassified as a keyword does not need its text.
        အကယ်၍ (parser_token->type == token_type) {
            parser_token->bytes = PyBytes_FromStringAndSize(text, length);
            အကယ်၍ (parser_token->bytes == NULL) {
                ပြန်ပေး -1;
            }
            အကယ်၍ (_PyArena_AddPyObject(p->arena, parser_token->bytes) < 0) {
                Py_DECREF(parser_token->bytes);
                ပြန်ပေး -1;
            }
        }
    }

    parser_token->metadata = NULL;
    အကယ်၍ (new_token->metadata != NULL) {
        အကယ်၍ (_PyArena_AddPyObject(p->arena, new_token->metadata) < 0) {
            ပြန်ပေး -1;
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

    အကယ်၍ (token_type == ERRORTOKEN && _PyTokenizer_GetStatus(p->tok) == E_DECODE) {
        ပြန်ပေး _Pypegen_raise_decode_error(p);
    }

    ပြန်ပေး (token_type == ERRORTOKEN ? _Pypegen_tokenizer_error(p) : 0);
}

static ကိန်းပြည့်
_resize_tokens_array(Parser *p) {
    အကယ်၍ (p->size > INT_MAX / PEGEN_ARRAY_GROWTH_FACTOR ||
            (size_t)p->size > PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * parimaa_na(*p->tokens))) {
        PyErr_NoMemory();
        ပြန်ပေး -1;
    }
    ကိန်းပြည့် newsize = p->size * PEGEN_ARRAY_GROWTH_FACTOR;
    Token **new_tokens = PyMem_Realloc(p->tokens, (size_t)newsize * parimaa_na(Token *));
    အကယ်၍ (new_tokens == NULL) {
        PyErr_NoMemory();
        ပြန်ပေး -1;
    }
    p->tokens = new_tokens;

    // Allocate new tokens together without moving existing ones: parser rules
    // may still hold pointers into earlier blocks. Parser_Free frees each block.
    Token *chunk = PyMem_Calloc((size_t)(newsize - p->size), parimaa_na(Token));
    အကယ်၍ (chunk == NULL) {
        PyErr_NoMemory();
        ပြန်ပေး -1;
    }
    အတွက် (ကိန်းပြည့် i = p->size; i < newsize; i++) {
        p->tokens[i] = &chunk[i - p->size];
    }
    p->size = newsize;
    ပြန်ပေး 0;
}

ကိန်းပြည့်
_PyPegen_fill_token(Parser *p)
{
    sa_nracanaa token new_token;
    _PyToken_Init(&new_token);
    _PyTokenizer_Get(p->tok, &new_token);

    // Record and skip '# type: ignore' comments
    နေစဉ် (new_token.type == TYPE_IGNORE) {
        Py_ssize_t len;
        const var_na *text = _PyToken_TextView(p->tok, &new_token, &len);
        var_na *tag = PyMem_Malloc((size_t)len + 1);
        အကယ်၍ (tag == NULL) {
            PyErr_NoMemory();
            goto error;
        }
        memcpy(tag, text, (size_t)len);
        tag[len] = '\0';
        // Ownership of tag passes to the growable array
        အကယ်၍ (!growable_comment_array_add(&p->type_ignore_comments, new_token.end_loc.lineno, tag)) {
            PyMem_Free(tag);
            PyErr_NoMemory();
            goto error;
        }
        _PyTokenizer_Get(p->tok, &new_token);
    }

    ကိန်းပြည့် type = new_token.type;
    // EOF can finish an ordinary token. Only a boundary token means that
    // the grammar requested input beyond the last physical line.
    အကယ်၍ (type == ENDMARKER ||
        (type == DEDENT && (p->flags & PyPARSE_ALLOW_INCOMPLETE_INPUT) &&
         _PyTokenizer_GetStatus(p->tok) == E_EOF)) {
        p->input_exhausted = 1;
    }

    // If we have reached the end and we are in single input mode we need to insert a newline and reset the parsing
    အကယ်၍ (p->start_rule == Py_single_input && type == ENDMARKER && p->parsing_started) {
        type = NEWLINE; /* Add an extra newline */
        p->parsing_started = 0;

        အကယ်၍ (!(p->flags & PyPARSE_DONT_IMPLY_DEDENT)) {
            _PyTokenizer_ImplyDedents(p->tok);
        }
    }
    မဟုတ်ပါက {
        p->parsing_started = 1;
    }

    // Check if we are at the limit of the token array capacity and resize if needed
    အကယ်၍ ((p->fill == p->size) && (_resize_tokens_array(p) != 0)) {
        goto error;
    }

    Token *t = p->tokens[p->fill];
    ကိန်းပြည့် result = initialize_token(p, t, &new_token, type);
    _PyToken_Free(&new_token);
    ပြန်ပေး result;
error:
    _PyToken_Free(&new_token);
    ပြန်ပေး -1;
}

#အကယ်၍ defined(Py_DEBUG)
// Instrumentation to count the effectiveness of memoization.
// The array counts the number of tokens skipped by memoization,
// indexed by type.

#define NSTATISTICS _PYPEGEN_NSTATISTICS
#define memo_statistics _PyRuntime.parser.memo_statistics

void
_PyPegen_clear_memo_statistics(void)
{
    PyMutex_Lock(&_PyRuntime.parser.mutex);
    အတွက် (ကိန်းပြည့် i = 0; i < NSTATISTICS; i++) {
        memo_statistics[i] = 0;
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
}

PyObject *
_PyPegen_get_memo_statistics(void)
{
    PyObject *ret = PyList_New(NSTATISTICS);
    အကယ်၍ (ret == NULL) {
        ပြန်ပေး NULL;
    }

    PyMutex_Lock(&_PyRuntime.parser.mutex);
    အတွက် (ကိန်းပြည့် i = 0; i < NSTATISTICS; i++) {
        PyObject *value = PyLong_FromLong(memo_statistics[i]);
        အကယ်၍ (value == NULL) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            ပြန်ပေး NULL;
        }
        // PyList_SetItem borrows a reference to value.
        အကယ်၍ (PyList_SetItem(ret, i, value) < 0) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            ပြန်ပေး NULL;
        }
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
    ပြန်ပေး ret;
}
#endif

ကိန်းပြည့်  // bool
_PyPegen_is_memoized(Parser *p, ကိန်းပြည့် type, void *pres)
{
    အကယ်၍ (p->mark == p->fill) {
        အကယ်၍ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ပြန်ပေး -1;
        }
    }

    Token *t = p->tokens[p->mark];

    အကယ်၍ (!(t->memo_mask & (1ULL << (type & 63)))) {
        ပြန်ပေး 0;
    }

    အတွက် (Memo *m = t->memo; m != NULL; m = m->next) {
        အကယ်၍ (m->type == type) {
#အကယ်၍ defined(Py_DEBUG)
            အကယ်၍ (0 <= type && type < NSTATISTICS) {
                long count = m->mark - p->mark;
                // A memoized negative result counts for one.
                အကယ်၍ (count <= 0) {
                    count = 1;
                }
                PyMutex_Lock(&_PyRuntime.parser.mutex);
                memo_statistics[type] += count;
                PyMutex_Unlock(&_PyRuntime.parser.mutex);
            }
#endif
            p->mark = m->mark;
            *(void **)(pres) = m->node;
            ပြန်ပေး 1;
        }
    }
    ပြန်ပေး 0;
}

#define LOOKAHEAD1(NAME, RES_TYPE)                                  \
    ကိန်းပြည့်                                                             \
    NAME (ကိန်းပြည့် positive, RES_TYPE (func)(Parser *), Parser *p)       \
    {                                                               \
        ကိန်းပြည့် mark = p->mark;                                         \
        void *res = func(p);                                        \
        p->mark = mark;                                             \
        ပြန်ပေး (res != NULL) == positive;                           \
    }

LOOKAHEAD1(_PyPegen_lookahead, void *)
LOOKAHEAD1(_PyPegen_lookahead_for_expr, expr_ty)
LOOKAHEAD1(_PyPegen_lookahead_for_stmt, stmt_ty)
#undef LOOKAHEAD1

#define LOOKAHEAD2(NAME, RES_TYPE, T)                                   \
    ကိန်းပြည့်                                                                 \
    NAME (ကိန်းပြည့် positive, RES_TYPE (func)(Parser *, T), Parser *p, T arg) \
    {                                                                   \
        ကိန်းပြည့် mark = p->mark;                                             \
        void *res = func(p, arg);                                       \
        p->mark = mark;                                                 \
        ပြန်ပေး (res != NULL) == positive;                               \
    }

LOOKAHEAD2(_PyPegen_lookahead_with_int, Token *, ကိန်းပြည့်)
LOOKAHEAD2(_PyPegen_lookahead_with_string, expr_ty, const var_na *)
#undef LOOKAHEAD2

Token *
_PyPegen_expect_token(Parser *p, ကိန်းပြည့် type)
{
    အကယ်၍ (p->mark == p->fill) {
        အကယ်၍ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ပြန်ပေး NULL;
        }
    }
    Token *t = p->tokens[p->mark];
    အကယ်၍ (t->type != type) {
       ပြန်ပေး NULL;
    }
    p->mark += 1;
    ပြန်ပေး t;
}

void*
_PyPegen_expect_forced_result(Parser *p, void* result, const var_na* expected) {

    အကယ်၍ (p->error_indicator == 1) {
        ပြန်ပေး NULL;
    }
    အကယ်၍ (result == NULL) {
        RAISE_SYNTAX_ERROR("expected (%s)", expected);
        ပြန်ပေး NULL;
    }
    ပြန်ပေး result;
}

Token *
_PyPegen_expect_forced_token(Parser *p, ကိန်းပြည့် type, const var_na* expected) {

    အကယ်၍ (p->error_indicator == 1) {
        ပြန်ပေး NULL;
    }

    အကယ်၍ (p->mark == p->fill) {
        အကယ်၍ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ပြန်ပေး NULL;
        }
    }
    Token *t = p->tokens[p->mark];
    အကယ်၍ (t->type != type) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(t, "expected '%s'", expected);
        ပြန်ပေး NULL;
    }
    p->mark += 1;
    ပြန်ပေး t;
}

expr_ty
_PyPegen_expect_soft_keyword(Parser *p, const var_na *keyword)
{
    အကယ်၍ (p->mark == p->fill) {
        အကယ်၍ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ပြန်ပေး NULL;
        }
    }
    Token *t = p->tokens[p->mark];
    အကယ်၍ (t->type != NAME) {
        ပြန်ပေး NULL;
    }
    const var_na *s = PyBytes_AsString(t->bytes);
    အကယ်၍ (!s) {
        p->error_indicator = 1;
        ပြန်ပေး NULL;
    }
    အကယ်၍ (strcmp(s, keyword) != 0) {
        ပြန်ပေး NULL;
    }
    ပြန်ပေး _PyPegen_name_token(p);
}

Token *
_PyPegen_get_last_nonnwhitespace_token(Parser *p)
{
    assert(p->mark >= 0);
    Token *token = NULL;
    အတွက် (ကိန်းပြည့် m = p->mark - 1; m >= 0; m--) {
        token = p->tokens[m];
        အကယ်၍ (token->type != ENDMARKER && (token->type < NEWLINE || token->type > DEDENT)) {
            bha_nga;
        }
    }
    ပြန်ပေး token;
}

PyObject *
_PyPegen_new_identifier(Parser *p, const var_na *n)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, (Py_ssize_t)strlen(n), NULL);
    အကယ်၍ (!id) {
        goto error;
    }
    /* Check whether there are non-ASCII characters in the
       identifier; if so, normalize to NFKC. */
    အကယ်၍ (!PyUnicode_IS_ASCII(id))
    {
        အကယ်၍ (!init_normalization(p))
        {
            Py_DECREF(id);
            goto error;
        }
        PyObject *form = PyUnicode_InternFromString("NFKC");
        အကယ်၍ (form == NULL)
        {
            Py_DECREF(id);
            goto error;
        }
        PyObject *args[2] = {form, id};
        PyObject *id2 = PyObject_Vectorcall(p->normalize, args, 2, NULL);
        Py_DECREF(id);
        Py_DECREF(form);
        အကယ်၍ (!id2) {
            goto error;
        }

        အကယ်၍ (!PyUnicode_Check(id2))
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
    အတွက် (ကိန်းပြည့် i = 0; forbidden[i] != NULL; i++) {
        အကယ်၍ (_PyUnicode_EqualToASCIIString(id, forbidden[i])) {
            PyErr_Format(PyExc_ValueError,
                         "identifier field can't represent '%s' constant",
                         forbidden[i]);
            Py_DECREF(id);
            goto error;
        }
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &id);
    အကယ်၍ (_PyArena_AddPyObject(p->arena, id) < 0)
    {
        Py_DECREF(id);
        goto error;
    }
    ပြန်ပေး id;

error:
    p->error_indicator = 1;
    ပြန်ပေး NULL;
}

// Return an arena-owned identifier; callers borrow the reference.
static PyObject *
get_cached_identifier(Parser *p, PyObject *bytes)
{
    const var_na *s = PyBytes_AsString(bytes);
    အကယ်၍ (!s) {
        p->error_indicator = 1;
        ပြန်ပေး NULL;
    }
    // Identifiers repeat constantly; a small span-keyed cache skips the
    // UTF-8 decode + intern for repeated occurrences. Keys point into
    // arena-owned token bytes and values are arena-owned interned strings,
    // so borrowed references are valid for the lifetime of the parse
    // (including the second error pass, which reuses parser and arena).
    Py_ssize_t len = PyBytes_GET_SIZE(bytes);
    Py_hash_t hash = PyObject_Hash(bytes);
    အကယ်၍ (hash == -1) {
        p->error_indicator = 1;
        ပြန်ပေး NULL;
    }
    // Parses without identifiers do not need this cache.
    အကယ်၍ (p->identifier_cache == NULL) {
        p->identifier_cache = PyMem_Calloc(
            IDENTIFIER_CACHE_SIZE, parimaa_na(*p->identifier_cache));
        အကယ်၍ (p->identifier_cache == NULL) {
            p->error_indicator = 1;
            ပြန်ပေး PyErr_NoMemory();
        }
    }
    IdentifierCacheEntry *free_slot = NULL;
    size_t idx = (size_t)hash & (IDENTIFIER_CACHE_SIZE - 1);
    အတွက် (ကိန်းပြည့် probe = 0; probe < IDENTIFIER_CACHE_MAX_PROBES; probe++) {
        IdentifierCacheEntry *entry = &p->identifier_cache[
            (idx + probe) & (IDENTIFIER_CACHE_SIZE - 1)];
        အကယ်၍ (entry->key == NULL) {
            free_slot = entry;
            bha_nga;
        }
        အကယ်၍ (entry->hash == hash && entry->len == len &&
            memcmp(entry->key, s, len) == 0)
        {
            ပြန်ပေး entry->value;
        }
    }
    PyObject *id = _PyPegen_new_identifier(p, s);
    အကယ်၍ (id == NULL) {
        p->error_indicator = 1;
        ပြန်ပေး NULL;
    }
    အကယ်၍ (free_slot != NULL) {
        free_slot->key = s;
        free_slot->len = len;
        free_slot->hash = hash;
        free_slot->value = id;
    }
    ပြန်ပေး id;
}

static expr_ty
_PyPegen_name_from_token(Parser *p, Token* t)
{
    အကယ်၍ (t == NULL) {
        ပြန်ပေး NULL;
    }
    // Reuse the AST node when backtracking revisits this token. Memo lookup
    // starts before the token and restores the position after it on a hit.
    // Token kinds can be memo keys: generated grammar rule IDs start at 1000.
    ကိန်းပြည့် mark = p->mark - 1;
    p->mark = mark;
    expr_ty cached = NULL;
    အကယ်၍ (_PyPegen_is_memoized(p, NAME, &cached)) {
        ပြန်ပေး cached;
    }
    p->mark = mark + 1;
    PyObject *id = get_cached_identifier(p, t->bytes);
    အကယ်၍ (id == NULL) {
        ပြန်ပေး NULL;
    }
    expr_ty result = _PyAST_Name(id, Load, t->lineno, t->col_offset,
                                t->end_lineno, t->end_col_offset, p->arena);
    အကယ်၍ (result != NULL && _PyPegen_insert_memo(p, mark, NAME, result) < 0) {
        p->error_indicator = 1;
        ပြန်ပေး NULL;
    }
    ပြန်ပေး result;
}

expr_ty
_PyPegen_name_token(Parser *p)
{
    Token *t = _PyPegen_expect_token(p, NAME);
    ပြန်ပေး _PyPegen_name_from_token(p, t);
}

void *
_PyPegen_string_token(Parser *p)
{
    ပြန်ပေး _PyPegen_expect_token(p, STRING);
}

expr_ty _PyPegen_soft_keyword_token(Parser *p) {
    Token *t = _PyPegen_expect_token(p, NAME);
    အကယ်၍ (t == NULL) {
        ပြန်ပေး NULL;
    }
    var_na *the_token;
    Py_ssize_t size;
    PyBytes_AsStringAndSize(t->bytes, &the_token, &size);
    အတွက် (var_na **keyword = p->soft_keywords; *keyword != NULL; keyword++) {
        အကယ်၍ (strlen(*keyword) == (size_t)size &&
            strncmp(*keyword, the_token, (size_t)size) == 0) {
            ပြန်ပေး _PyPegen_name_from_token(p, t);
        }
    }
    ပြန်ပေး NULL;
}

static PyObject *
parsenumber_raw(const var_na *s)
{
    const var_na *end;
    long x;
    dviguu_na dx;
    Py_complex compl;
    ကိန်းပြည့် imflag;

    assert(s != NULL);
    errno = 0;
    end = s + strlen(s) - 1;
    imflag = *end == 'j' || *end == 'J';
    အကယ်၍ (s[0] == '0') {
        x = (long)PyOS_strtoul(s, (var_na **)&end, 0);
        အကယ်၍ (x < 0 && errno == 0) {
            ပြန်ပေး PyLong_FromString(s, (var_na **)0, 0);
        }
    }
    မဟုတ်ပါက {
        x = PyOS_strtol(s, (var_na **)&end, 0);
    }
    အကယ်၍ (*end == '\0') {
        အကယ်၍ (errno != 0) {
            ပြန်ပေး PyLong_FromString(s, (var_na **)0, 0);
        }
        ပြန်ပေး PyLong_FromLong(x);
    }
    /* XXX Huge floats may silently fail */
    အကယ်၍ (imflag) {
        compl.real = 0.;
        compl.imag = PyOS_string_to_double(s, (var_na **)&end, NULL);
        အကယ်၍ (compl.imag == -1.0 && PyErr_Occurred()) {
            ပြန်ပေး NULL;
        }
        ပြန်ပေး PyComplex_FromCComplex(compl);
    }
    dx = PyOS_string_to_double(s, NULL, NULL);
    အကယ်၍ (dx == -1.0 && PyErr_Occurred()) {
        ပြန်ပေး NULL;
    }
    ပြန်ပေး PyFloat_FromDouble(dx);
}

static PyObject *
parsenumber(const var_na *s)
{
    var_na *dup;
    var_na *end;
    PyObject *res = NULL;

    assert(s != NULL);

    အကယ်၍ (strchr(s, '_') == NULL) {
        ပြန်ပေး parsenumber_raw(s);
    }
    /* Create a duplicate without underscores. */
    dup = PyMem_Malloc(strlen(s) + 1);
    အကယ်၍ (dup == NULL) {
        ပြန်ပေး PyErr_NoMemory();
    }
    end = dup;
    အတွက် (; *s; s++) {
        အကယ်၍ (*s != '_') {
            *end++ = *s;
        }
    }
    *end = '\0';
    res = parsenumber_raw(dup);
    PyMem_Free(dup);
    ပြန်ပေး res;
}

expr_ty
_PyPegen_number_token(Parser *p)
{
    ကိန်းပြည့် mark = p->mark;
    Token *t = _PyPegen_expect_token(p, NUMBER);
    အကယ်၍ (t == NULL) {
        ပြန်ပေး NULL;
    }

    // Look up the node at the token's start, as for NAME tokens. A memo hit
    // advances the mark; on a miss, restore the position after the token.
    p->mark = mark;
    expr_ty cached = NULL;
    အကယ်၍ (_PyPegen_is_memoized(p, NUMBER, &cached)) {
        ပြန်ပေး cached;
    }
    p->mark = mark + 1;

    const var_na *num_raw = PyBytes_AsString(t->bytes);
    အကယ်၍ (num_raw == NULL) {
        p->error_indicator = 1;
        ပြန်ပေး NULL;
    }

    အကယ်၍ (p->feature_version < 6 && strchr(num_raw, '_') != NULL) {
        p->error_indicator = 1;
        ပြန်ပေး RAISE_SYNTAX_ERROR("Underscores in numeric literals are only supported "
                                  "in Python 3.6 and greater");
    }

    PyObject *c = parsenumber(num_raw);

    အကယ်၍ (c == NULL) {
        p->error_indicator = 1;
        PyThreadState *tstate = _PyThreadState_GET();
        // The only way a ValueError should happen in _this_ code is via
        // PyLong_FromString hitting a length limit.
        အကယ်၍ (tstate->current_exception != NULL &&
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
        ပြန်ပေး NULL;
    }

    အကယ်၍ (_PyArena_AddPyObject(p->arena, c) < 0) {
        Py_DECREF(c);
        p->error_indicator = 1;
        ပြန်ပေး NULL;
    }

    expr_ty result = _PyAST_Constant(c, NULL, t->lineno, t->col_offset,
                                    t->end_lineno, t->end_col_offset, p->arena);
    အကယ်၍ (result != NULL && _PyPegen_insert_memo(p, mark, NUMBER, result) < 0) {
        p->error_indicator = 1;
        ပြန်ပေး NULL;
    }
    ပြန်ပေး result;
}



static ကိန်းပြည့်
compute_parser_flags(PyCompilerFlags *flags)
{
    ကိန်းပြည့် parser_flags = 0;
    အကယ်၍ (!flags) {
        ပြန်ပေး 0;
    }
    အကယ်၍ (flags->cf_flags & PyCF_DONT_IMPLY_DEDENT) {
        parser_flags |= PyPARSE_DONT_IMPLY_DEDENT;
    }
    အကယ်၍ (flags->cf_flags & PyCF_IGNORE_COOKIE) {
        parser_flags |= PyPARSE_IGNORE_COOKIE;
    }
    အကယ်၍ (flags->cf_flags & CO_FUTURE_BARRY_AS_BDFL) {
        parser_flags |= PyPARSE_BARRY_AS_BDFL;
    }
    အကယ်၍ (flags->cf_flags & PyCF_TYPE_COMMENTS) {
        parser_flags |= PyPARSE_TYPE_COMMENTS;
    }
    အကယ်၍ (flags->cf_flags & PyCF_ALLOW_INCOMPLETE_INPUT) {
        parser_flags |= PyPARSE_ALLOW_INCOMPLETE_INPUT;
    }
    ပြန်ပေး parser_flags;
}

// Parser API

Parser *
_PyPegen_Parser_New(sa_nracanaa tok_state *tok, ကိန်းပြည့် start_rule, ကိန်းပြည့် flags,
                    ကိန်းပြည့် feature_version, ကိန်းပြည့် *errcode, const var_na* source, PyArena *arena)
{
    Parser *p = PyMem_Malloc(parimaa_na(Parser));
    အကယ်၍ (p == NULL) {
        ပြန်ပေး (Parser *) PyErr_NoMemory();
    }
    assert(tok != NULL);
    _PyTokenizer_SetOptions(tok, 0, (flags & PyPARSE_TYPE_COMMENTS) > 0);
    _PyTokenizer_AllowIncompleteInput(tok, flags & PyPARSE_ALLOW_INCOMPLETE_INPUT);
    p->tok = tok;
    p->keywords = NULL;
    p->n_keyword_lists = -1;
    p->soft_keywords = NULL;
    p->tokens = PyMem_Malloc(parimaa_na(Token *));
    အကယ်၍ (!p->tokens) {
        PyMem_Free(p);
        ပြန်ပေး (Parser *) PyErr_NoMemory();
    }
    p->tokens[0] = PyMem_Calloc(1, parimaa_na(Token));
    အကယ်၍ (!p->tokens[0]) {
        PyMem_Free(p->tokens);
        PyMem_Free(p);
        ပြန်ပေး (Parser *) PyErr_NoMemory();
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
#အကယ်၍ _Py_STACK_GROWS_DOWN
    p->stack_soft_limit += _PyOS_STACK_MARGIN_BYTES;
#မဟုတ်ပါက
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
    ပြန်ပေး p;
}

void
_PyPegen_Parser_Free(Parser *p)
{
    PyMem_Free(p->identifier_cache);
    Py_XDECREF(p->normalize);
    // Resizes allocate blocks starting at indices 1, 2, 4, and so on.
    PyMem_Free(p->tokens[0]);
    အတွက် (ကိန်းပြည့် i = 1; i < p->size; i *= PEGEN_ARRAY_GROWTH_FACTOR) {
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
    အတွက် (ကိန်းပြည့် i = 0; i < p->fill; i++) {
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
    အကယ်၍ (!exc || !PyObject_TypeCheck(exc, (PyTypeObject *)PyExc_SyntaxError)) {
        PyErr_SetRaisedException(exc);
        ပြန်ပေး;
    }
    const var_na *source = _PyTokenizer_RetainedSource(p->tok);
    const var_na *encoding = _PyTokenizer_GetInfo(p->tok).encoding;
    PyObject* the_source = NULL;
    အကယ်၍ (source) {
        အကယ်၍ (encoding == NULL) {
            the_source = PyUnicode_FromString(source);
        } မဟုတ်ပါက {
            the_source = PyUnicode_Decode(source, strlen(source), encoding, NULL);
        }
    }
    အကယ်၍ (!the_source) {
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
    အကယ်၍ (!metadata) {
        PyErr_Clear();
        PyErr_SetRaisedException(exc);
        ပြန်ပေး;
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
    အကယ်၍ (res != NULL && PyErr_Occurred()) {
        // Discard a result returned with an exception still pending
        // (e.g. a MemoryError from a recovered-from allocation failure).
        ပြန်ပေး NULL;
    }
    အကယ်၍ (res == NULL) {
        အကယ်၍ (PyErr_Occurred() &&
            (PyErr_ExceptionMatches(PyExc_IncompleteInputError) ||
             !PyErr_ExceptionMatches(PyExc_SyntaxError))) {
            ပြန်ပေး NULL;
        }
        အကယ်၍ ((p->flags & PyPARSE_ALLOW_INCOMPLETE_INPUT) &&
            p->input_exhausted && !PyErr_Occurred()) {
            ပြန်ပေး _PyPegen_raise_incomplete_input(p);
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
        အကယ်၍ (PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_set_syntax_error_metadata(p);
        }
       ပြန်ပေး NULL;
    }

    အကယ်၍ (p->start_rule == Py_single_input && _PyTokenizer_HasTrailingStatement(p->tok)) {
        ပြန်ပေး RAISE_SYNTAX_ERROR("multiple statements found while compiling a single statement");
    }

    // test_peg_generator defines _Py_TEST_PEGEN to not call PyAST_Validate()
#အကယ်၍ defined(Py_DEBUG) && !defined(_Py_TEST_PEGEN)
    အကယ်၍ (p->start_rule == Py_single_input ||
        p->start_rule == Py_file_input ||
        p->start_rule == Py_eval_input)
    {
        အကယ်၍ (!_PyAST_Validate(res)) {
            ပြန်ပေး NULL;
        }
    }
#endif
    ပြန်ပေး res;
}

mod_ty
_PyPegen_run_parser_from_file_pointer(FILE *fp, ကိန်းပြည့် start_rule, PyObject *filename_ob,
                             const var_na *enc, const var_na *ps1, const var_na *ps2,
                             PyCompilerFlags *flags, ကိန်းပြည့် *errcode,
                             PyObject **interactive_src, PyArena *arena)
{
    ကိန်းပြည့် parser_flags = compute_parser_flags(flags);
    sa_nracanaa tok_state *tok = _PyTokenizer_FromFile(fp, enc, ps1, ps2);
    အကယ်၍ (tok == NULL) {
        အကယ်၍ (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        မဟုတ်ပါက {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        ပြန်ပေး NULL;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    // From here on we need to clean up even if there's an error
    mod_ty result = NULL;

    PyObject *module = PyUnicode_FromString("__main__");
    အကယ်၍ (module == NULL) {
        goto error;
    }
    _PyTokenizer_SetContext(tok, filename_ob, module);
    Py_DECREF(module);

    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, PY_MINOR_VERSION,
                                    errcode, NULL, arena);
    အကယ်၍ (p == NULL) {
        goto error;
    }

    result = _PyPegen_run_parser(p);
    _PyPegen_Parser_Free(p);

    const var_na *source = _PyTokenizer_RetainedSource(tok);
    အကယ်၍ (source != NULL && result && interactive_src != NULL) {
        *interactive_src = PyUnicode_FromString(source);
        အကယ်၍ (!*interactive_src || _PyArena_AddPyObject(arena, *interactive_src) < 0) {
            Py_XDECREF(*interactive_src);
            result = NULL;
            goto error;
        }
    }

error:
    _PyTokenizer_Free(tok);
    ပြန်ပေး result;
}

mod_ty
_PyPegen_run_parser_from_string(const var_na *str, ကိန်းပြည့် start_rule, PyObject *filename_ob,
                       PyCompilerFlags *flags, PyArena *arena, PyObject *module,
                       PyObject **incomplete)
{
    _PyTokenizer_Newline newline_mode = start_rule == Py_file_input
        ? _PyTokenizer_IMPLY_NEWLINE : _PyTokenizer_KEEP_NEWLINE;
    ကိန်းပြည့် parser_flags = compute_parser_flags(flags);
    အကယ်၍ (parser_flags & PyPARSE_ALLOW_INCOMPLETE_INPUT) {
        newline_mode = _PyTokenizer_APPEND_NEWLINE;
    }

    sa_nracanaa tok_state *tok;
    အကယ်၍ (flags != NULL && flags->cf_flags & PyCF_IGNORE_COOKIE) {
        tok = _PyTokenizer_FromUTF8(str, newline_mode, 0);
    } မဟုတ်ပါက {
        tok = _PyTokenizer_FromString(str, newline_mode, 0);
    }
    အကယ်၍ (tok == NULL) {
        အကယ်၍ (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        မဟုတ်ပါက {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        ပြန်ပေး NULL;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    _PyTokenizer_SetContext(tok, filename_ob, module);

    // We need to clear up from here on
    mod_ty result = NULL;

    ကိန်းပြည့် feature_version = flags && (flags->cf_flags & PyCF_ONLY_AST) ?
        flags->cf_feature_version : PY_MINOR_VERSION;
    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, feature_version,
                                    NULL, str, arena);
    အကယ်၍ (p == NULL) {
        goto error;
    }

    result = _PyPegen_run_parser(p);
    အကယ်၍ (result != NULL && (parser_flags & PyPARSE_ALLOW_INCOMPLETE_INPUT)) {
        ကိန်းပြည့် needs_newline = _PyTokenizer_GetNewlineRequirements(tok);
        အကယ်၍ ((needs_newline & _PyTokenizer_NEWLINE_CONTINUATION) ||
            (start_rule != Py_file_input &&
             (needs_newline & _PyTokenizer_NEWLINE_INDENT)) ||
            (start_rule == Py_single_input && p->input_exhausted &&
             (needs_newline & _PyTokenizer_NEWLINE_STATEMENT))) {
            _PyPegen_raise_incomplete_input(p);
            အကယ်၍ (incomplete != NULL && PyErr_ExceptionMatches(PyExc_IncompleteInputError)) {
                *incomplete = PyErr_GetRaisedException();
            }
            မဟုတ်ပါက {
                result = NULL;
            }
        }
    }
    _PyPegen_Parser_Free(p);

error:
    _PyTokenizer_Free(tok);
    ပြန်ပေး result;
}
