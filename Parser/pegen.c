#ضمن <Python.h>
#ضمن "pycore_ast.h"           // _PyAST_Validate(),
#ضمن "pycore_pystate.h"       // _PyThreadState_GET()
#ضمن "pycore_parser.h"        // _PYPEGEN_NSTATISTICS
#ضمن "pycore_pyerrors.h"      // PyExc_IncompleteInputError
#ضمن "pycore_runtime.h"       // _PyRuntime
#ضمن "pycore_unicodeobject.h" // _PyUnicode_InternImmortal
#ضمن <errcode.h>

#ضمن "lexer/state.h"
#ضمن "tokenizer/tokenizer.h"
#ضمن "pegen.h"

#define IDENTIFIER_CACHE_SIZE 2048  // Must be a power of two.
#define IDENTIFIER_CACHE_MAX_PROBES 8
#define PEGEN_ARRAY_GROWTH_FACTOR 2
#define TYPE_IGNORE_INITIAL_CAPACITY 10

بنية _identifier_cache_entry {
    ثابت حرف *key;   // Borrowed from arena-owned token bytes.
    Py_ssize_t len;
    Py_hash_t hash;
    PyObject *value;   // Borrowed from an arena-owned identifier.
};

// Internal parser functions

asdl_stmt_seq*
_PyPegen_interactive_exit(Parser *p)
{
    ܐܢ (p->errcode) {
        *(p->errcode) = E_EOF;
    }
    ܦܢܐ عدم;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset_line(PyObject *line, Py_ssize_t col_offset, Py_ssize_t end_col_offset)
{
    ثابت unsigned حرف *data = (ثابت unsigned حرف*)PyUnicode_AsUTF8(line);
    ܐܢ (data == عدم) {
        ܦܢܐ -1;
    }

    Py_ssize_t len = 0;
    ܟܡܐ (col_offset < end_col_offset) {
        Py_UCS4 ch = data[col_offset];
        ܐܢ (ch < 0x80) {
            col_offset += 1;
        } ܐܚܪܢܐ ܐܢ ((ch & 0xe0) == 0xc0) {
            col_offset += 2;
        } ܐܚܪܢܐ ܐܢ ((ch & 0xf0) == 0xe0) {
            col_offset += 3;
        } ܐܚܪܢܐ ܐܢ ((ch & 0xf8) == 0xf0) {
            col_offset += 4;
        } ܐܚܪܢܐ {
            PyErr_SetString(PyExc_ValueError, "Invalid UTF-8 sequence");
            ܦܢܐ -1;
        }
        len++;
    }
    ܦܢܐ len;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset_raw(ثابت حرف* str, Py_ssize_t col_offset)
{
    Py_ssize_t len = (Py_ssize_t)strlen(str);
    ܐܢ (col_offset > len + 1) {
        col_offset = len + 1;
    }
    أكد(col_offset >= 0);
    PyObject *text = PyUnicode_DecodeUTF8(str, col_offset, "replace");
    ܐܢ (!text) {
        ܦܢܐ -1;
    }
    Py_ssize_t size = PyUnicode_GET_LENGTH(text);
    Py_DECREF(text);
    ܦܢܐ size;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset(PyObject *line, Py_ssize_t col_offset)
{
    ثابت حرف *str = PyUnicode_AsUTF8(line);
    ܐܢ (!str) {
        ܦܢܐ -1;
    }
    ܦܢܐ _PyPegen_byte_offset_to_character_offset_raw(str, col_offset);
}

// Here, mark is the start of the node, while p->mark is the end.
// If node==NULL, they should be the same.
صحيح
_PyPegen_insert_memo(Parser *p, صحيح mark, صحيح type, فراغ *node)
{
    // Insert in front
    Memo *m = _PyArena_Malloc(p->arena, حجم(Memo));
    ܐܢ (m == عدم) {
        ܦܢܐ -1;
    }
    m->type = type;
    m->node = node;
    m->mark = p->mark;
    m->next = p->tokens[mark]->memo;
    p->tokens[mark]->memo = m;
    p->tokens[mark]->memo_mask |= 1ULL << (type & 63);
    ܦܢܐ 0;
}

// Like _PyPegen_insert_memo(), but returns the inserted Memo so callers
// can update it in place without re-walking the token's memo list.
Memo *
_PyPegen_insert_memo_direct(Parser *p, صحيح mark, صحيح type)
{
    ܐܢ (_PyPegen_insert_memo(p, mark, type, عدم) < 0) {
        ܦܢܐ عدم;
    }
    ܦܢܐ p->tokens[mark]->memo;
}

// Like _PyPegen_insert_memo(), but updates an existing node if found.
صحيح
_PyPegen_update_memo(Parser *p, صحيح mark, صحيح type, فراغ *node)
{
    لكل (Memo *m = p->tokens[mark]->memo; m != عدم; m = m->next) {
        ܐܢ (m->type == type) {
            // Update existing node.
            m->node = node;
            m->mark = p->mark;
            ܦܢܐ 0;
        }
    }
    // Insert new node.
    ܦܢܐ _PyPegen_insert_memo(p, mark, type, node);
}

ساكن صحيح
init_normalization(Parser *p)
{
    ܐܢ (p->normalize) {
        ܦܢܐ 1;
    }
    p->normalize = PyImport_ImportModuleAttrString("unicodedata", "normalize");
    ܐܢ (!p->normalize)
    {
        ܦܢܐ 0;
    }
    ܦܢܐ 1;
}

صحيح
_PyPegen_grow_loop_buffer(فراغ ***buffer, Py_ssize_t *capacity)
{
    ܐܢ ((size_t)*capacity > (size_t)PY_SSIZE_T_MAX /
            (PEGEN_ARRAY_GROWTH_FACTOR * حجم(**buffer))) {
        ܦܢܐ -1;
    }
    Py_ssize_t new_capacity = *capacity == 0
        ? 1 : *capacity * PEGEN_ARRAY_GROWTH_FACTOR;
    فراغ **new_buffer = PyMem_Realloc(*buffer, new_capacity * حجم(**buffer));
    ܐܢ (new_buffer == عدم) {
        ܦܢܐ -1;
    }
    *buffer = new_buffer;
    *capacity = new_capacity;
    ܦܢܐ 0;
}

ساكن صحيح
growable_comment_array_add(growable_comment_array *arr, صحيح lineno, حرف *comment)
{
    ܐܢ (arr->num_items >= arr->size) {
        ܐܢ (arr->size > (size_t)PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * حجم(*arr->items))) {
            ܦܢܐ 0;
        }
        size_t new_size = arr->size == 0
            ? TYPE_IGNORE_INITIAL_CAPACITY
            : arr->size * PEGEN_ARRAY_GROWTH_FACTOR;
        فراغ *new_items_array = PyMem_Realloc(
            arr->items, new_size * حجم(*arr->items));
        ܐܢ (!new_items_array) {
            ܦܢܐ 0;
        }
        arr->items = new_items_array;
        arr->size = new_size;
    }

    arr->items[arr->num_items].lineno = lineno;
    arr->items[arr->num_items].comment = comment;  // Take ownership
    arr->num_items++;
    ܦܢܐ 1;
}

ساكن فراغ
growable_comment_array_deallocate(growable_comment_array *arr)
{
    لكل (size_t i = 0; i < arr->num_items; i++) {
        PyMem_Free(arr->items[i].comment);
    }
    PyMem_Free(arr->items);
}

ساكن صحيح
_get_keyword_or_name_type(Parser *p, ثابت حرف *text, Py_ssize_t length)
{
    أكد(length > 0);

    ܐܢ (length >= p->n_keyword_lists ||
        p->keywords[length] == عدم ||
        p->keywords[length]->type == -1) {
        ܦܢܐ NAME;
    }
    لكل (KeywordToken *k = p->keywords[length]; k != عدم && k->type != -1; k++) {
        ܐܢ (memcmp(k->str, text, (size_t)length) == 0) {
            ܦܢܐ k->type;
        }
    }
    ܦܢܐ NAME;
}

// Keep text only where grammar actions or helpers read it. Keyword error
// actions use fixed spellings, so keywords do not need their text.
ساكن مضمن صحيح
token_needs_text(صحيح type)
{
    بدل (type) {
        حالة NAME:
        حالة NUMBER:
        حالة STRING:
        حالة FSTRING_START:
        حالة FSTRING_MIDDLE:
        حالة FSTRING_END:
        حالة TSTRING_START:
        حالة TSTRING_MIDDLE:
        حالة TSTRING_END:
        حالة TYPE_COMMENT:
        حالة NOTEQUAL:  // _PyPegen_check_barry_as_flufl() distinguishes != and <>.
            ܦܢܐ 1;
        افتراضي:
            ܦܢܐ 0;
    }
}

ساكن صحيح
initialize_token(Parser *p, Token *parser_token, بنية token *new_token, صحيح token_type) {
    أكد(parser_token != عدم);

    parser_token->type = token_type;
    parser_token->bytes = عدم;
    ܐܢ (token_needs_text(token_type) && new_token->span.start >= 0) {
        Py_ssize_t length;
        ثابت حرف *text = _PyToken_TextView(p->tok, new_token, &length);
        ܐܢ (token_type == NAME) {
            parser_token->type = _get_keyword_or_name_type(p, text, length);
        }
        // A name reclassified as a keyword does not need its text.
        ܐܢ (parser_token->type == token_type) {
            parser_token->bytes = PyBytes_FromStringAndSize(text, length);
            ܐܢ (parser_token->bytes == عدم) {
                ܦܢܐ -1;
            }
            ܐܢ (_PyArena_AddPyObject(p->arena, parser_token->bytes) < 0) {
                Py_DECREF(parser_token->bytes);
                ܦܢܐ -1;
            }
        }
    }

    parser_token->metadata = عدم;
    ܐܢ (new_token->metadata != عدم) {
        ܐܢ (_PyArena_AddPyObject(p->arena, new_token->metadata) < 0) {
            ܦܢܐ -1;
        }
        parser_token->metadata = new_token->metadata;
        new_token->metadata = عدم;
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

    ܐܢ (token_type == ERRORTOKEN && _PyTokenizer_GetStatus(p->tok) == E_DECODE) {
        ܦܢܐ _Pypegen_raise_decode_error(p);
    }

    ܦܢܐ (token_type == ERRORTOKEN ? _Pypegen_tokenizer_error(p) : 0);
}

ساكن صحيح
_resize_tokens_array(Parser *p) {
    ܐܢ (p->size > INT_MAX / PEGEN_ARRAY_GROWTH_FACTOR ||
            (size_t)p->size > PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * حجم(*p->tokens))) {
        PyErr_NoMemory();
        ܦܢܐ -1;
    }
    صحيح newsize = p->size * PEGEN_ARRAY_GROWTH_FACTOR;
    Token **new_tokens = PyMem_Realloc(p->tokens, (size_t)newsize * حجم(Token *));
    ܐܢ (new_tokens == عدم) {
        PyErr_NoMemory();
        ܦܢܐ -1;
    }
    p->tokens = new_tokens;

    // Allocate new tokens together without moving existing ones: parser rules
    // may still hold pointers into earlier blocks. Parser_Free frees each block.
    Token *chunk = PyMem_Calloc((size_t)(newsize - p->size), حجم(Token));
    ܐܢ (chunk == عدم) {
        PyErr_NoMemory();
        ܦܢܐ -1;
    }
    لكل (صحيح i = p->size; i < newsize; i++) {
        p->tokens[i] = &chunk[i - p->size];
    }
    p->size = newsize;
    ܦܢܐ 0;
}

صحيح
_PyPegen_fill_token(Parser *p)
{
    بنية token new_token;
    _PyToken_Init(&new_token);
    _PyTokenizer_Get(p->tok, &new_token);

    // Record and skip '# type: ignore' comments
    ܟܡܐ (new_token.type == TYPE_IGNORE) {
        Py_ssize_t len;
        ثابت حرف *text = _PyToken_TextView(p->tok, &new_token, &len);
        حرف *tag = PyMem_Malloc((size_t)len + 1);
        ܐܢ (tag == عدم) {
            PyErr_NoMemory();
            اذهب error;
        }
        memcpy(tag, text, (size_t)len);
        tag[len] = '\0';
        // Ownership of tag passes to the growable array
        ܐܢ (!growable_comment_array_add(&p->type_ignore_comments, new_token.end_loc.lineno, tag)) {
            PyMem_Free(tag);
            PyErr_NoMemory();
            اذهب error;
        }
        _PyTokenizer_Get(p->tok, &new_token);
    }

    صحيح type = new_token.type;
    // EOF can finish an ordinary token. Only a boundary token means that
    // the grammar requested input beyond the last physical line.
    ܐܢ (type == ENDMARKER ||
        (type == DEDENT && (p->flags & PyPARSE_ALLOW_INCOMPLETE_INPUT) &&
         _PyTokenizer_GetStatus(p->tok) == E_EOF)) {
        p->input_exhausted = 1;
    }

    // If we have reached the end and we are in single input mode we need to insert a newline and reset the parsing
    ܐܢ (p->start_rule == Py_single_input && type == ENDMARKER && p->parsing_started) {
        type = NEWLINE; /* Add an extra newline */
        p->parsing_started = 0;

        ܐܢ (!(p->flags & PyPARSE_DONT_IMPLY_DEDENT)) {
            _PyTokenizer_ImplyDedents(p->tok);
        }
    }
    ܐܚܪܢܐ {
        p->parsing_started = 1;
    }

    // Check if we are at the limit of the token array capacity and resize if needed
    ܐܢ ((p->fill == p->size) && (_resize_tokens_array(p) != 0)) {
        اذهب error;
    }

    Token *t = p->tokens[p->fill];
    صحيح result = initialize_token(p, t, &new_token, type);
    _PyToken_Free(&new_token);
    ܦܢܐ result;
error:
    _PyToken_Free(&new_token);
    ܦܢܐ -1;
}

#ܐܢ defined(Py_DEBUG)
// Instrumentation to count the effectiveness of memoization.
// The array counts the number of tokens skipped by memoization,
// indexed by type.

#define NSTATISTICS _PYPEGEN_NSTATISTICS
#define memo_statistics _PyRuntime.parser.memo_statistics

فراغ
_PyPegen_clear_memo_statistics(فراغ)
{
    PyMutex_Lock(&_PyRuntime.parser.mutex);
    لكل (صحيح i = 0; i < NSTATISTICS; i++) {
        memo_statistics[i] = 0;
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
}

PyObject *
_PyPegen_get_memo_statistics(فراغ)
{
    PyObject *ret = PyList_New(NSTATISTICS);
    ܐܢ (ret == عدم) {
        ܦܢܐ عدم;
    }

    PyMutex_Lock(&_PyRuntime.parser.mutex);
    لكل (صحيح i = 0; i < NSTATISTICS; i++) {
        PyObject *value = PyLong_FromLong(memo_statistics[i]);
        ܐܢ (value == عدم) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            ܦܢܐ عدم;
        }
        // PyList_SetItem borrows a reference to value.
        ܐܢ (PyList_SetItem(ret, i, value) < 0) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            ܦܢܐ عدم;
        }
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
    ܦܢܐ ret;
}
#endif

صحيح  // bool
_PyPegen_is_memoized(Parser *p, صحيح type, فراغ *pres)
{
    ܐܢ (p->mark == p->fill) {
        ܐܢ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ܦܢܐ -1;
        }
    }

    Token *t = p->tokens[p->mark];

    ܐܢ (!(t->memo_mask & (1ULL << (type & 63)))) {
        ܦܢܐ 0;
    }

    لكل (Memo *m = t->memo; m != عدم; m = m->next) {
        ܐܢ (m->type == type) {
#ܐܢ defined(Py_DEBUG)
            ܐܢ (0 <= type && type < NSTATISTICS) {
                long count = m->mark - p->mark;
                // A memoized negative result counts for one.
                ܐܢ (count <= 0) {
                    count = 1;
                }
                PyMutex_Lock(&_PyRuntime.parser.mutex);
                memo_statistics[type] += count;
                PyMutex_Unlock(&_PyRuntime.parser.mutex);
            }
#endif
            p->mark = m->mark;
            *(فراغ **)(pres) = m->node;
            ܦܢܐ 1;
        }
    }
    ܦܢܐ 0;
}

#define LOOKAHEAD1(NAME, RES_TYPE)                                  \
    صحيح                                                             \
    NAME (صحيح positive, RES_TYPE (func)(Parser *), Parser *p)       \
    {                                                               \
        صحيح mark = p->mark;                                         \
        فراغ *res = func(p);                                        \
        p->mark = mark;                                             \
        ܦܢܐ (res != عدم) == positive;                           \
    }

LOOKAHEAD1(_PyPegen_lookahead, فراغ *)
LOOKAHEAD1(_PyPegen_lookahead_for_expr, expr_ty)
LOOKAHEAD1(_PyPegen_lookahead_for_stmt, stmt_ty)
#undef LOOKAHEAD1

#define LOOKAHEAD2(NAME, RES_TYPE, T)                                   \
    صحيح                                                                 \
    NAME (صحيح positive, RES_TYPE (func)(Parser *, T), Parser *p, T arg) \
    {                                                                   \
        صحيح mark = p->mark;                                             \
        فراغ *res = func(p, arg);                                       \
        p->mark = mark;                                                 \
        ܦܢܐ (res != عدم) == positive;                               \
    }

LOOKAHEAD2(_PyPegen_lookahead_with_int, Token *, صحيح)
LOOKAHEAD2(_PyPegen_lookahead_with_string, expr_ty, ثابت حرف *)
#undef LOOKAHEAD2

Token *
_PyPegen_expect_token(Parser *p, صحيح type)
{
    ܐܢ (p->mark == p->fill) {
        ܐܢ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ܦܢܐ عدم;
        }
    }
    Token *t = p->tokens[p->mark];
    ܐܢ (t->type != type) {
       ܦܢܐ عدم;
    }
    p->mark += 1;
    ܦܢܐ t;
}

فراغ*
_PyPegen_expect_forced_result(Parser *p, فراغ* result, ثابت حرف* expected) {

    ܐܢ (p->error_indicator == 1) {
        ܦܢܐ عدم;
    }
    ܐܢ (result == عدم) {
        RAISE_SYNTAX_ERROR("expected (%s)", expected);
        ܦܢܐ عدم;
    }
    ܦܢܐ result;
}

Token *
_PyPegen_expect_forced_token(Parser *p, صحيح type, ثابت حرف* expected) {

    ܐܢ (p->error_indicator == 1) {
        ܦܢܐ عدم;
    }

    ܐܢ (p->mark == p->fill) {
        ܐܢ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ܦܢܐ عدم;
        }
    }
    Token *t = p->tokens[p->mark];
    ܐܢ (t->type != type) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(t, "expected '%s'", expected);
        ܦܢܐ عدم;
    }
    p->mark += 1;
    ܦܢܐ t;
}

expr_ty
_PyPegen_expect_soft_keyword(Parser *p, ثابت حرف *keyword)
{
    ܐܢ (p->mark == p->fill) {
        ܐܢ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ܦܢܐ عدم;
        }
    }
    Token *t = p->tokens[p->mark];
    ܐܢ (t->type != NAME) {
        ܦܢܐ عدم;
    }
    ثابت حرف *s = PyBytes_AsString(t->bytes);
    ܐܢ (!s) {
        p->error_indicator = 1;
        ܦܢܐ عدم;
    }
    ܐܢ (strcmp(s, keyword) != 0) {
        ܦܢܐ عدم;
    }
    ܦܢܐ _PyPegen_name_token(p);
}

Token *
_PyPegen_get_last_nonnwhitespace_token(Parser *p)
{
    أكد(p->mark >= 0);
    Token *token = عدم;
    لكل (صحيح m = p->mark - 1; m >= 0; m--) {
        token = p->tokens[m];
        ܐܢ (token->type != ENDMARKER && (token->type < NEWLINE || token->type > DEDENT)) {
            اكسر;
        }
    }
    ܦܢܐ token;
}

PyObject *
_PyPegen_new_identifier(Parser *p, ثابت حرف *n)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, (Py_ssize_t)strlen(n), عدم);
    ܐܢ (!id) {
        اذهب error;
    }
    /* Check whether there are non-ASCII characters in the
       identifier; if so, normalize to NFKC. */
    ܐܢ (!PyUnicode_IS_ASCII(id))
    {
        ܐܢ (!init_normalization(p))
        {
            Py_DECREF(id);
            اذهب error;
        }
        PyObject *form = PyUnicode_InternFromString("NFKC");
        ܐܢ (form == عدم)
        {
            Py_DECREF(id);
            اذهب error;
        }
        PyObject *args[2] = {form, id};
        PyObject *id2 = PyObject_Vectorcall(p->normalize, args, 2, عدم);
        Py_DECREF(id);
        Py_DECREF(form);
        ܐܢ (!id2) {
            اذهب error;
        }

        ܐܢ (!PyUnicode_Check(id2))
        {
            PyErr_Format(PyExc_TypeError,
                         "unicodedata.normalize() must return a string, not "
                         "%.200s",
                         _PyType_Name(Py_TYPE(id2)));
            Py_DECREF(id2);
            اذهب error;
        }
        id = id2;
    }
    ساكن ثابت حرف * ثابت forbidden[] = {
        "None",
        "True",
        "False",
        عدم
    };
    لكل (صحيح i = 0; forbidden[i] != عدم; i++) {
        ܐܢ (_PyUnicode_EqualToASCIIString(id, forbidden[i])) {
            PyErr_Format(PyExc_ValueError,
                         "identifier field can't represent '%s' constant",
                         forbidden[i]);
            Py_DECREF(id);
            اذهب error;
        }
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &id);
    ܐܢ (_PyArena_AddPyObject(p->arena, id) < 0)
    {
        Py_DECREF(id);
        اذهب error;
    }
    ܦܢܐ id;

error:
    p->error_indicator = 1;
    ܦܢܐ عدم;
}

// Return an arena-owned identifier; callers borrow the reference.
ساكن PyObject *
get_cached_identifier(Parser *p, PyObject *bytes)
{
    ثابت حرف *s = PyBytes_AsString(bytes);
    ܐܢ (!s) {
        p->error_indicator = 1;
        ܦܢܐ عدم;
    }
    // Identifiers repeat constantly; a small span-keyed cache skips the
    // UTF-8 decode + intern for repeated occurrences. Keys point into
    // arena-owned token bytes and values are arena-owned interned strings,
    // so borrowed references are valid for the lifetime of the parse
    // (including the second error pass, which reuses parser and arena).
    Py_ssize_t len = PyBytes_GET_SIZE(bytes);
    Py_hash_t hash = PyObject_Hash(bytes);
    ܐܢ (hash == -1) {
        p->error_indicator = 1;
        ܦܢܐ عدم;
    }
    // Parses without identifiers do not need this cache.
    ܐܢ (p->identifier_cache == عدم) {
        p->identifier_cache = PyMem_Calloc(
            IDENTIFIER_CACHE_SIZE, حجم(*p->identifier_cache));
        ܐܢ (p->identifier_cache == عدم) {
            p->error_indicator = 1;
            ܦܢܐ PyErr_NoMemory();
        }
    }
    IdentifierCacheEntry *free_slot = عدم;
    size_t idx = (size_t)hash & (IDENTIFIER_CACHE_SIZE - 1);
    لكل (صحيح probe = 0; probe < IDENTIFIER_CACHE_MAX_PROBES; probe++) {
        IdentifierCacheEntry *entry = &p->identifier_cache[
            (idx + probe) & (IDENTIFIER_CACHE_SIZE - 1)];
        ܐܢ (entry->key == عدم) {
            free_slot = entry;
            اكسر;
        }
        ܐܢ (entry->hash == hash && entry->len == len &&
            memcmp(entry->key, s, len) == 0)
        {
            ܦܢܐ entry->value;
        }
    }
    PyObject *id = _PyPegen_new_identifier(p, s);
    ܐܢ (id == عدم) {
        p->error_indicator = 1;
        ܦܢܐ عدم;
    }
    ܐܢ (free_slot != عدم) {
        free_slot->key = s;
        free_slot->len = len;
        free_slot->hash = hash;
        free_slot->value = id;
    }
    ܦܢܐ id;
}

ساكن expr_ty
_PyPegen_name_from_token(Parser *p, Token* t)
{
    ܐܢ (t == عدم) {
        ܦܢܐ عدم;
    }
    // Reuse the AST node when backtracking revisits this token. Memo lookup
    // starts before the token and restores the position after it on a hit.
    // Token kinds can be memo keys: generated grammar rule IDs start at 1000.
    صحيح mark = p->mark - 1;
    p->mark = mark;
    expr_ty cached = عدم;
    ܐܢ (_PyPegen_is_memoized(p, NAME, &cached)) {
        ܦܢܐ cached;
    }
    p->mark = mark + 1;
    PyObject *id = get_cached_identifier(p, t->bytes);
    ܐܢ (id == عدم) {
        ܦܢܐ عدم;
    }
    expr_ty result = _PyAST_Name(id, Load, t->lineno, t->col_offset,
                                t->end_lineno, t->end_col_offset, p->arena);
    ܐܢ (result != عدم && _PyPegen_insert_memo(p, mark, NAME, result) < 0) {
        p->error_indicator = 1;
        ܦܢܐ عدم;
    }
    ܦܢܐ result;
}

expr_ty
_PyPegen_name_token(Parser *p)
{
    Token *t = _PyPegen_expect_token(p, NAME);
    ܦܢܐ _PyPegen_name_from_token(p, t);
}

فراغ *
_PyPegen_string_token(Parser *p)
{
    ܦܢܐ _PyPegen_expect_token(p, STRING);
}

expr_ty _PyPegen_soft_keyword_token(Parser *p) {
    Token *t = _PyPegen_expect_token(p, NAME);
    ܐܢ (t == عدم) {
        ܦܢܐ عدم;
    }
    حرف *the_token;
    Py_ssize_t size;
    PyBytes_AsStringAndSize(t->bytes, &the_token, &size);
    لكل (حرف **keyword = p->soft_keywords; *keyword != عدم; keyword++) {
        ܐܢ (strlen(*keyword) == (size_t)size &&
            strncmp(*keyword, the_token, (size_t)size) == 0) {
            ܦܢܐ _PyPegen_name_from_token(p, t);
        }
    }
    ܦܢܐ عدم;
}

ساكن PyObject *
parsenumber_raw(ثابت حرف *s)
{
    ثابت حرف *end;
    long x;
    مزدوج dx;
    Py_complex compl;
    صحيح imflag;

    أكد(s != عدم);
    errno = 0;
    end = s + strlen(s) - 1;
    imflag = *end == 'j' || *end == 'J';
    ܐܢ (s[0] == '0') {
        x = (long)PyOS_strtoul(s, (حرف **)&end, 0);
        ܐܢ (x < 0 && errno == 0) {
            ܦܢܐ PyLong_FromString(s, (حرف **)0, 0);
        }
    }
    ܐܚܪܢܐ {
        x = PyOS_strtol(s, (حرف **)&end, 0);
    }
    ܐܢ (*end == '\0') {
        ܐܢ (errno != 0) {
            ܦܢܐ PyLong_FromString(s, (حرف **)0, 0);
        }
        ܦܢܐ PyLong_FromLong(x);
    }
    /* XXX Huge floats may silently fail */
    ܐܢ (imflag) {
        compl.real = 0.;
        compl.imag = PyOS_string_to_double(s, (حرف **)&end, عدم);
        ܐܢ (compl.imag == -1.0 && PyErr_Occurred()) {
            ܦܢܐ عدم;
        }
        ܦܢܐ PyComplex_FromCComplex(compl);
    }
    dx = PyOS_string_to_double(s, عدم, عدم);
    ܐܢ (dx == -1.0 && PyErr_Occurred()) {
        ܦܢܐ عدم;
    }
    ܦܢܐ PyFloat_FromDouble(dx);
}

ساكن PyObject *
parsenumber(ثابت حرف *s)
{
    حرف *dup;
    حرف *end;
    PyObject *res = عدم;

    أكد(s != عدم);

    ܐܢ (strchr(s, '_') == عدم) {
        ܦܢܐ parsenumber_raw(s);
    }
    /* Create a duplicate without underscores. */
    dup = PyMem_Malloc(strlen(s) + 1);
    ܐܢ (dup == عدم) {
        ܦܢܐ PyErr_NoMemory();
    }
    end = dup;
    لكل (; *s; s++) {
        ܐܢ (*s != '_') {
            *end++ = *s;
        }
    }
    *end = '\0';
    res = parsenumber_raw(dup);
    PyMem_Free(dup);
    ܦܢܐ res;
}

expr_ty
_PyPegen_number_token(Parser *p)
{
    صحيح mark = p->mark;
    Token *t = _PyPegen_expect_token(p, NUMBER);
    ܐܢ (t == عدم) {
        ܦܢܐ عدم;
    }

    // Look up the node at the token's start, as for NAME tokens. A memo hit
    // advances the mark; on a miss, restore the position after the token.
    p->mark = mark;
    expr_ty cached = عدم;
    ܐܢ (_PyPegen_is_memoized(p, NUMBER, &cached)) {
        ܦܢܐ cached;
    }
    p->mark = mark + 1;

    ثابت حرف *num_raw = PyBytes_AsString(t->bytes);
    ܐܢ (num_raw == عدم) {
        p->error_indicator = 1;
        ܦܢܐ عدم;
    }

    ܐܢ (p->feature_version < 6 && strchr(num_raw, '_') != عدم) {
        p->error_indicator = 1;
        ܦܢܐ RAISE_SYNTAX_ERROR("Underscores in numeric literals are only supported "
                                  "in Python 3.6 and greater");
    }

    PyObject *c = parsenumber(num_raw);

    ܐܢ (c == عدم) {
        p->error_indicator = 1;
        PyThreadState *tstate = _PyThreadState_GET();
        // The only way a ValueError should happen in _this_ code is via
        // PyLong_FromString hitting a length limit.
        ܐܢ (tstate->current_exception != عدم &&
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
        ܦܢܐ عدم;
    }

    ܐܢ (_PyArena_AddPyObject(p->arena, c) < 0) {
        Py_DECREF(c);
        p->error_indicator = 1;
        ܦܢܐ عدم;
    }

    expr_ty result = _PyAST_Constant(c, عدم, t->lineno, t->col_offset,
                                    t->end_lineno, t->end_col_offset, p->arena);
    ܐܢ (result != عدم && _PyPegen_insert_memo(p, mark, NUMBER, result) < 0) {
        p->error_indicator = 1;
        ܦܢܐ عدم;
    }
    ܦܢܐ result;
}



ساكن صحيح
compute_parser_flags(PyCompilerFlags *flags)
{
    صحيح parser_flags = 0;
    ܐܢ (!flags) {
        ܦܢܐ 0;
    }
    ܐܢ (flags->cf_flags & PyCF_DONT_IMPLY_DEDENT) {
        parser_flags |= PyPARSE_DONT_IMPLY_DEDENT;
    }
    ܐܢ (flags->cf_flags & PyCF_IGNORE_COOKIE) {
        parser_flags |= PyPARSE_IGNORE_COOKIE;
    }
    ܐܢ (flags->cf_flags & CO_FUTURE_BARRY_AS_BDFL) {
        parser_flags |= PyPARSE_BARRY_AS_BDFL;
    }
    ܐܢ (flags->cf_flags & PyCF_TYPE_COMMENTS) {
        parser_flags |= PyPARSE_TYPE_COMMENTS;
    }
    ܐܢ (flags->cf_flags & PyCF_ALLOW_INCOMPLETE_INPUT) {
        parser_flags |= PyPARSE_ALLOW_INCOMPLETE_INPUT;
    }
    ܦܢܐ parser_flags;
}

// Parser API

Parser *
_PyPegen_Parser_New(بنية tok_state *tok, صحيح start_rule, صحيح flags,
                    صحيح feature_version, صحيح *errcode, ثابت حرف* source, PyArena *arena)
{
    Parser *p = PyMem_Malloc(حجم(Parser));
    ܐܢ (p == عدم) {
        ܦܢܐ (Parser *) PyErr_NoMemory();
    }
    أكد(tok != عدم);
    _PyTokenizer_SetOptions(tok, 0, (flags & PyPARSE_TYPE_COMMENTS) > 0);
    _PyTokenizer_AllowIncompleteInput(tok, flags & PyPARSE_ALLOW_INCOMPLETE_INPUT);
    p->tok = tok;
    p->keywords = عدم;
    p->n_keyword_lists = -1;
    p->soft_keywords = عدم;
    p->tokens = PyMem_Malloc(حجم(Token *));
    ܐܢ (!p->tokens) {
        PyMem_Free(p);
        ܦܢܐ (Parser *) PyErr_NoMemory();
    }
    p->tokens[0] = PyMem_Calloc(1, حجم(Token));
    ܐܢ (!p->tokens[0]) {
        PyMem_Free(p->tokens);
        PyMem_Free(p);
        ܦܢܐ (Parser *) PyErr_NoMemory();
    }
    p->type_ignore_comments = (growable_comment_array){0};

    p->mark = 0;
    p->fill = 0;
    p->size = 1;

    p->errcode = errcode;
    p->arena = arena;
    p->start_rule = start_rule;
    p->parsing_started = 0;
    p->normalize = عدم;
    p->error_indicator = 0;

    p->starting_lineno = 0;
    p->starting_col_offset = 0;
    p->flags = flags;
    p->feature_version = feature_version;
    p->known_err_token = عدم;
    p->identifier_cache = عدم;
    p->tstate = PyThreadState_Get();
    // Stack limits are initialized when the thread state is attached.
    أكد(((_PyThreadStateImpl *)p->tstate)->c_stack_hard_limit != 0);
    p->stack_soft_limit = ((_PyThreadStateImpl *)p->tstate)->c_stack_soft_limit;
#ܐܢ _Py_STACK_GROWS_DOWN
    p->stack_soft_limit += _PyOS_STACK_MARGIN_BYTES;
#ܐܚܪܢܐ
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
    ܦܢܐ p;
}

فراغ
_PyPegen_Parser_Free(Parser *p)
{
    PyMem_Free(p->identifier_cache);
    Py_XDECREF(p->normalize);
    // Resizes allocate blocks starting at indices 1, 2, 4, and so on.
    PyMem_Free(p->tokens[0]);
    لكل (صحيح i = 1; i < p->size; i *= PEGEN_ARRAY_GROWTH_FACTOR) {
        PyMem_Free(p->tokens[i]);
    }
    PyMem_Free(p->tokens);
    growable_comment_array_deallocate(&p->type_ignore_comments);
    PyMem_Free(p);
}

ساكن فراغ
reset_parser_state_for_error_pass(Parser *p)
{
    p->last_stmt_location.lineno = 0;
    p->last_stmt_location.col_offset = 0;
    p->last_stmt_location.end_lineno = 0;
    p->last_stmt_location.end_col_offset = 0;
    لكل (صحيح i = 0; i < p->fill; i++) {
        p->tokens[i]->memo = عدم;
        p->tokens[i]->memo_mask = 0;
    }
    p->mark = 0;
    p->flags &= ~PyPARSE_ALLOW_INCOMPLETE_INPUT;
    _PyTokenizer_AllowIncompleteInput(p->tok, 0);
    p->call_invalid_rules = 1;
    _PyTokenizer_StopInteractive(p->tok);
}

ساكن فراغ
_PyPegen_set_syntax_error_metadata(Parser *p) {
    PyObject *exc = PyErr_GetRaisedException();
    ܐܢ (!exc || !PyObject_TypeCheck(exc, (PyTypeObject *)PyExc_SyntaxError)) {
        PyErr_SetRaisedException(exc);
        ܦܢܐ;
    }
    ثابت حرف *source = _PyTokenizer_RetainedSource(p->tok);
    ثابت حرف *encoding = _PyTokenizer_GetInfo(p->tok).encoding;
    PyObject* the_source = عدم;
    ܐܢ (source) {
        ܐܢ (encoding == عدم) {
            the_source = PyUnicode_FromString(source);
        } ܐܚܪܢܐ {
            the_source = PyUnicode_Decode(source, strlen(source), encoding, عدم);
        }
    }
    ܐܢ (!the_source) {
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
    ܐܢ (!metadata) {
        PyErr_Clear();
        PyErr_SetRaisedException(exc);
        ܦܢܐ;
    }
    PySyntaxErrorObject *syntax_error = (PySyntaxErrorObject *)exc;

    Py_XDECREF(syntax_error->metadata);
    syntax_error->metadata = metadata;
    PyErr_SetRaisedException(exc);
}

فراغ *
_PyPegen_run_parser(Parser *p)
{
    فراغ *res = _PyPegen_parse(p);
    أكد(p->level == 0);
    ܐܢ (res != عدم && PyErr_Occurred()) {
        // Discard a result returned with an exception still pending
        // (e.g. a MemoryError from a recovered-from allocation failure).
        ܦܢܐ عدم;
    }
    ܐܢ (res == عدم) {
        ܐܢ (PyErr_Occurred() &&
            (PyErr_ExceptionMatches(PyExc_IncompleteInputError) ||
             !PyErr_ExceptionMatches(PyExc_SyntaxError))) {
            ܦܢܐ عدم;
        }
        ܐܢ ((p->flags & PyPARSE_ALLOW_INCOMPLETE_INPUT) &&
            p->input_exhausted && !PyErr_Occurred()) {
            ܦܢܐ _PyPegen_raise_incomplete_input(p);
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
        ܐܢ (PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_set_syntax_error_metadata(p);
        }
       ܦܢܐ عدم;
    }

    ܐܢ (p->start_rule == Py_single_input && _PyTokenizer_HasTrailingStatement(p->tok)) {
        ܦܢܐ RAISE_SYNTAX_ERROR("multiple statements found while compiling a single statement");
    }

    // test_peg_generator defines _Py_TEST_PEGEN to not call PyAST_Validate()
#ܐܢ defined(Py_DEBUG) && !defined(_Py_TEST_PEGEN)
    ܐܢ (p->start_rule == Py_single_input ||
        p->start_rule == Py_file_input ||
        p->start_rule == Py_eval_input)
    {
        ܐܢ (!_PyAST_Validate(res)) {
            ܦܢܐ عدم;
        }
    }
#endif
    ܦܢܐ res;
}

mod_ty
_PyPegen_run_parser_from_file_pointer(FILE *fp, صحيح start_rule, PyObject *filename_ob,
                             ثابت حرف *enc, ثابت حرف *ps1, ثابت حرف *ps2,
                             PyCompilerFlags *flags, صحيح *errcode,
                             PyObject **interactive_src, PyArena *arena)
{
    صحيح parser_flags = compute_parser_flags(flags);
    بنية tok_state *tok = _PyTokenizer_FromFile(fp, enc, ps1, ps2);
    ܐܢ (tok == عدم) {
        ܐܢ (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        ܐܚܪܢܐ {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        ܦܢܐ عدم;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    // From here on we need to clean up even if there's an error
    mod_ty result = عدم;

    PyObject *module = PyUnicode_FromString("__main__");
    ܐܢ (module == عدم) {
        اذهب error;
    }
    _PyTokenizer_SetContext(tok, filename_ob, module);
    Py_DECREF(module);

    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, PY_MINOR_VERSION,
                                    errcode, عدم, arena);
    ܐܢ (p == عدم) {
        اذهب error;
    }

    result = _PyPegen_run_parser(p);
    _PyPegen_Parser_Free(p);

    ثابت حرف *source = _PyTokenizer_RetainedSource(tok);
    ܐܢ (source != عدم && result && interactive_src != عدم) {
        *interactive_src = PyUnicode_FromString(source);
        ܐܢ (!*interactive_src || _PyArena_AddPyObject(arena, *interactive_src) < 0) {
            Py_XDECREF(*interactive_src);
            result = عدم;
            اذهب error;
        }
    }

error:
    _PyTokenizer_Free(tok);
    ܦܢܐ result;
}

mod_ty
_PyPegen_run_parser_from_string(ثابت حرف *str, صحيح start_rule, PyObject *filename_ob,
                       PyCompilerFlags *flags, PyArena *arena, PyObject *module,
                       PyObject **incomplete)
{
    _PyTokenizer_Newline newline_mode = start_rule == Py_file_input
        ? _PyTokenizer_IMPLY_NEWLINE : _PyTokenizer_KEEP_NEWLINE;
    صحيح parser_flags = compute_parser_flags(flags);
    ܐܢ (parser_flags & PyPARSE_ALLOW_INCOMPLETE_INPUT) {
        newline_mode = _PyTokenizer_APPEND_NEWLINE;
    }

    بنية tok_state *tok;
    ܐܢ (flags != عدم && flags->cf_flags & PyCF_IGNORE_COOKIE) {
        tok = _PyTokenizer_FromUTF8(str, newline_mode, 0);
    } ܐܚܪܢܐ {
        tok = _PyTokenizer_FromString(str, newline_mode, 0);
    }
    ܐܢ (tok == عدم) {
        ܐܢ (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        ܐܚܪܢܐ {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        ܦܢܐ عدم;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    _PyTokenizer_SetContext(tok, filename_ob, module);

    // We need to clear up from here on
    mod_ty result = عدم;

    صحيح feature_version = flags && (flags->cf_flags & PyCF_ONLY_AST) ?
        flags->cf_feature_version : PY_MINOR_VERSION;
    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, feature_version,
                                    عدم, str, arena);
    ܐܢ (p == عدم) {
        اذهب error;
    }

    result = _PyPegen_run_parser(p);
    ܐܢ (result != عدم && (parser_flags & PyPARSE_ALLOW_INCOMPLETE_INPUT)) {
        صحيح needs_newline = _PyTokenizer_GetNewlineRequirements(tok);
        ܐܢ ((needs_newline & _PyTokenizer_NEWLINE_CONTINUATION) ||
            (start_rule != Py_file_input &&
             (needs_newline & _PyTokenizer_NEWLINE_INDENT)) ||
            (start_rule == Py_single_input && p->input_exhausted &&
             (needs_newline & _PyTokenizer_NEWLINE_STATEMENT))) {
            _PyPegen_raise_incomplete_input(p);
            ܐܢ (incomplete != عدم && PyErr_ExceptionMatches(PyExc_IncompleteInputError)) {
                *incomplete = PyErr_GetRaisedException();
            }
            ܐܚܪܢܐ {
                result = عدم;
            }
        }
    }
    _PyPegen_Parser_Free(p);

error:
    _PyTokenizer_Free(tok);
    ܦܢܐ result;
}
