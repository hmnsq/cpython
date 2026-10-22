#شامل <Python.h>
#شامل "pycore_ast.h"           // _PyAST_Validate(),
#شامل "pycore_pystate.h"       // _PyThreadState_GET()
#شامل "pycore_parser.h"        // _PYPEGEN_NSTATISTICS
#شامل "pycore_pyerrors.h"      // PyExc_IncompleteInputError
#شامل "pycore_runtime.h"       // _PyRuntime
#شامل "pycore_unicodeobject.h" // _PyUnicode_InternImmortal
#شامل <errcode.h>

#شامل "lexer/state.h"
#شامل "tokenizer/tokenizer.h"
#شامل "pegen.h"

#define IDENTIFIER_CACHE_SIZE 2048  // Must be a power of two.
#define IDENTIFIER_CACHE_MAX_PROBES 8
#define PEGEN_ARRAY_GROWTH_FACTOR 2
#define TYPE_IGNORE_INITIAL_CAPACITY 10

ساختار _identifier_cache_entry {
    ثابت نویسه *کلید;   // Borrowed from arena-owned token bytes.
    Py_ssize_t len;
    Py_hash_t hash;
    PyObject *مقدار;   // Borrowed from an arena-owned identifier.
};

// Internal parser functions

asdl_stmt_seq*
_PyPegen_interactive_exit(Parser *p)
{
    اگر (p->errcode) {
        *(p->errcode) = E_EOF;
    }
    بازگشت پوچ;
}

Py_ssize_t
Py_Pegen_byte_offset_to_character_offset_سطر(PyObject *سطر, Py_ssize_t col_offset, Py_ssize_t end_col_offset)
{
    ثابت unsigned نویسه *data = (ثابت unsigned نویسه*)PyUnicode_AsUTF8(سطر);
    اگر (data == پوچ) {
        بازگشت -1;
    }

    Py_ssize_t len = 0;
    وقتی (col_offset < end_col_offset) {
        Py_UCS4 ch = data[col_offset];
        اگر (ch < 0x80) {
            col_offset += 1;
        } وگرنه اگر ((ch & 0xe0) == 0xc0) {
            col_offset += 2;
        } وگرنه اگر ((ch & 0xf0) == 0xe0) {
            col_offset += 3;
        } وگرنه اگر ((ch & 0xf8) == 0xf0) {
            col_offset += 4;
        } وگرنه {
            PyErr_SetString(Py_Exc_مقدار_Error, "Invalid UTF-8 sequence");
            بازگشت -1;
        }
        len++;
    }
    بازگشت len;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset_raw(ثابت نویسه* str, Py_ssize_t col_offset)
{
    Py_ssize_t len = (Py_ssize_t)strlen(str);
    اگر (col_offset > len + 1) {
        col_offset = len + 1;
    }
    أكد(col_offset >= 0);
    PyObject *text = PyUnicode_DecodeUTF8(str, col_offset, "replace");
    اگر (!text) {
        بازگشت -1;
    }
    Py_ssize_t size = PyUnicode_GET_LENGTH(text);
    Py_DECREF(text);
    بازگشت size;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset(PyObject *سطر, Py_ssize_t col_offset)
{
    ثابت نویسه *str = PyUnicode_AsUTF8(سطر);
    اگر (!str) {
        بازگشت -1;
    }
    بازگشت _PyPegen_byte_offset_to_character_offset_raw(str, col_offset);
}

// Here, mark is the start of the node, while p->mark is the end.
// If node==NULL, they should be the same.
صحیح
Py_Pegen_درج_memo(Parser *p, صحیح mark, صحیح type, تهی *گره)
{
    // Insert in front
    Memo *m = _PyArena_Malloc(p->arena, اندازه(Memo));
    اگر (m == پوچ) {
        بازگشت -1;
    }
    m->type = type;
    m->گره = گره;
    m->mark = p->mark;
    m->next = p->tokens[mark]->memo;
    p->tokens[mark]->memo = m;
    p->tokens[mark]->memo_mask |= 1ULL << (type & 63);
    بازگشت 0;
}

// Like _PyPegen_insert_memo(), but returns the inserted Memo so callers
// can update it in place without re-walking the token's memo list.
Memo *
Py_Pegen_درج_memo_direct(Parser *p, صحیح mark, صحیح type)
{
    اگر (Py_Pegen_درج_memo(p, mark, type, پوچ) < 0) {
        بازگشت پوچ;
    }
    بازگشت p->tokens[mark]->memo;
}

// Like _PyPegen_insert_memo(), but updates an existing node if found.
صحیح
_PyPegen_update_memo(Parser *p, صحیح mark, صحیح type, تهی *گره)
{
    برای (Memo *m = p->tokens[mark]->memo; m != پوچ; m = m->next) {
        اگر (m->type == type) {
            // Update existing node.
            m->گره = گره;
            m->mark = p->mark;
            بازگشت 0;
        }
    }
    // Insert new node.
    بازگشت Py_Pegen_درج_memo(p, mark, type, گره);
}

ایستا صحیح
init_normalization(Parser *p)
{
    اگر (p->normalize) {
        بازگشت 1;
    }
    p->normalize = PyImport_ImportModuleAttrString("unicodedata", "normalize");
    اگر (!p->normalize)
    {
        بازگشت 0;
    }
    بازگشت 1;
}

صحیح
_PyPegen_grow_loop_buffer(تهی ***buffer, Py_ssize_t *capacity)
{
    اگر ((size_t)*capacity > (size_t)PY_SSIZE_T_MAX /
            (PEGEN_ARRAY_GROWTH_FACTOR * اندازه(**buffer))) {
        بازگشت -1;
    }
    Py_ssize_t new_capacity = *capacity == 0
        ? 1 : *capacity * PEGEN_ARRAY_GROWTH_FACTOR;
    تهی **new_buffer = PyMem_Realloc(*buffer, new_capacity * اندازه(**buffer));
    اگر (new_buffer == پوچ) {
        بازگشت -1;
    }
    *buffer = new_buffer;
    *capacity = new_capacity;
    بازگشت 0;
}

ایستا صحیح
growable_comment_array_add(growable_comment_array *arr, صحیح lineno, نویسه *comment)
{
    اگر (arr->رقم_اقلام >= arr->size) {
        اگر (arr->size > (size_t)PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * اندازه(*arr->اقلام))) {
            بازگشت 0;
        }
        size_t new_size = arr->size == 0
            ? TYPE_IGNORE_INITIAL_CAPACITY
            : arr->size * PEGEN_ARRAY_GROWTH_FACTOR;
        تهی *new_اقلام_array = PyMem_Realloc(
            arr->اقلام, new_size * اندازه(*arr->اقلام));
        اگر (!new_اقلام_array) {
            بازگشت 0;
        }
        arr->اقلام = new_اقلام_array;
        arr->size = new_size;
    }

    arr->اقلام[arr->رقم_اقلام].lineno = lineno;
    arr->اقلام[arr->رقم_اقلام].comment = comment;  // Take ownership
    arr->رقم_اقلام++;
    بازگشت 1;
}

ایستا تهی
growable_comment_array_deallocate(growable_comment_array *arr)
{
    برای (size_t i = 0; i < arr->رقم_اقلام; i++) {
        PyMem_Free(arr->اقلام[i].comment);
    }
    PyMem_Free(arr->اقلام);
}

ایستا صحیح
_get_keyword_or_name_type(Parser *p, ثابت نویسه *text, Py_ssize_t length)
{
    أكد(length > 0);

    اگر (length >= p->n_keyword_lists ||
        p->keywords[length] == پوچ ||
        p->keywords[length]->type == -1) {
        بازگشت NAME;
    }
    برای (KeywordToken *k = p->keywords[length]; k != پوچ && k->type != -1; k++) {
        اگر (memcmp(k->str, text, (size_t)length) == 0) {
            بازگشت k->type;
        }
    }
    بازگشت NAME;
}

// Keep text only where grammar actions or helpers read it. Keyword error
// actions use fixed spellings, so keywords do not need their text.
ایستا درونخطی صحیح
token_needs_text(صحیح type)
{
    انتخاب (type) {
        حالت NAME:
        حالت عدد:
        حالت STRING:
        حالت FSTRING_START:
        حالت FSTRING_میانه:
        حالت FSTRING_END:
        حالت TSTRING_START:
        حالت TSTRING_میانه:
        حالت TSTRING_END:
        حالت TYPE_COMMENT:
        حالت NOTEQUAL:  // _PyPegen_check_barry_as_flufl() distinguishes != and <>.
            بازگشت 1;
        پیشفرض:
            بازگشت 0;
    }
}

ایستا صحیح
initialize_token(Parser *p, Token *parser_token, ساختار token *new_token, صحیح token_type) {
    أكد(parser_token != پوچ);

    parser_token->type = token_type;
    parser_token->bytes = پوچ;
    اگر (token_needs_text(token_type) && new_token->span.start >= 0) {
        Py_ssize_t length;
        ثابت نویسه *text = _PyToken_TextView(p->tok, new_token, &length);
        اگر (token_type == NAME) {
            parser_token->type = _get_keyword_or_name_type(p, text, length);
        }
        // A name reclassified as a keyword does not need its text.
        اگر (parser_token->type == token_type) {
            parser_token->bytes = PyBytes_FromStringAndSize(text, length);
            اگر (parser_token->bytes == پوچ) {
                بازگشت -1;
            }
            اگر (_PyArena_AddPyObject(p->arena, parser_token->bytes) < 0) {
                Py_DECREF(parser_token->bytes);
                بازگشت -1;
            }
        }
    }

    parser_token->metadata = پوچ;
    اگر (new_token->metadata != پوچ) {
        اگر (_PyArena_AddPyObject(p->arena, new_token->metadata) < 0) {
            بازگشت -1;
        }
        parser_token->metadata = new_token->metadata;
        new_token->metadata = پوچ;
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

    اگر (token_type == ERRORTOKEN && _PyTokenizer_GetStatus(p->tok) == E_DECODE) {
        بازگشت _Pypegen_raise_decode_error(p);
    }

    بازگشت (token_type == ERRORTOKEN ? _Pypegen_tokenizer_error(p) : 0);
}

ایستا صحیح
_resize_tokens_array(Parser *p) {
    اگر (p->size > INT_MAX / PEGEN_ARRAY_GROWTH_FACTOR ||
            (size_t)p->size > PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * اندازه(*p->tokens))) {
        PyErr_NoMemory();
        بازگشت -1;
    }
    صحیح newsize = p->size * PEGEN_ARRAY_GROWTH_FACTOR;
    Token **new_tokens = PyMem_Realloc(p->tokens, (size_t)newsize * اندازه(Token *));
    اگر (new_tokens == پوچ) {
        PyErr_NoMemory();
        بازگشت -1;
    }
    p->tokens = new_tokens;

    // Allocate new tokens together without moving existing ones: parser rules
    // may still hold pointers into earlier blocks. Parser_Free frees each block.
    Token *chunk = PyMem_Calloc((size_t)(newsize - p->size), اندازه(Token));
    اگر (chunk == پوچ) {
        PyErr_NoMemory();
        بازگشت -1;
    }
    برای (صحیح i = p->size; i < newsize; i++) {
        p->tokens[i] = &chunk[i - p->size];
    }
    p->size = newsize;
    بازگشت 0;
}

صحیح
_PyPegen_fill_token(Parser *p)
{
    ساختار token new_token;
    _PyToken_Init(&new_token);
    _PyTokenizer_Get(p->tok, &new_token);

    // Record and skip '# type: ignore' comments
    وقتی (new_token.type == TYPE_IGNORE) {
        Py_ssize_t len;
        ثابت نویسه *text = _PyToken_TextView(p->tok, &new_token, &len);
        نویسه *tag = PyMem_Malloc((size_t)len + 1);
        اگر (tag == پوچ) {
            PyErr_NoMemory();
            برو error;
        }
        memcpy(tag, text, (size_t)len);
        tag[len] = '\0';
        // Ownership of tag passes to the growable array
        اگر (!growable_comment_array_add(&p->type_ignore_comments, new_token.end_loc.lineno, tag)) {
            PyMem_Free(tag);
            PyErr_NoMemory();
            برو error;
        }
        _PyTokenizer_Get(p->tok, &new_token);
    }

    صحیح type = new_token.type;
    // EOF can finish an ordinary token. Only a boundary token means that
    // the grammar requested input beyond the last physical line.
    اگر (type == ENDMARKER ||
        (type == DEDENT && (p->flags & Py_PARSE_ALLOW_INCOMPLETE_درون_داد) &&
         _PyTokenizer_GetStatus(p->tok) == E_EOF)) {
        p->درون_داد_exhausted = 1;
    }

    // If we have reached the end and we are in single input mode we need to insert a newline and reset the parsing
    اگر (p->start_rule == Py_single_درون_داد && type == ENDMARKER && p->parsing_started) {
        type = NEWLINE; /* Add an extra newline */
        p->parsing_started = 0;

        اگر (!(p->flags & PyPARSE_DONT_IMPLY_DEDENT)) {
            _PyTokenizer_ImplyDedents(p->tok);
        }
    }
    وگرنه {
        p->parsing_started = 1;
    }

    // Check if we are at the limit of the token array capacity and resize if needed
    اگر ((p->fill == p->size) && (_resize_tokens_array(p) != 0)) {
        برو error;
    }

    Token *t = p->tokens[p->fill];
    صحیح result = initialize_token(p, t, &new_token, type);
    _PyToken_Free(&new_token);
    بازگشت result;
error:
    _PyToken_Free(&new_token);
    بازگشت -1;
}

#اگر defined(Py_DEBUG)
// Instrumentation to count the effectiveness of memoization.
// The array counts the number of tokens skipped by memoization,
// indexed by type.

#define NSTATISTICS _PYPEGEN_NSTATISTICS
#define memo_statistics _PyRuntime.parser.memo_statistics

تهی
_PyPegen_clear_memo_statistics(تهی)
{
    PyMutex_Lock(&_PyRuntime.parser.mutex);
    برای (صحیح i = 0; i < NSTATISTICS; i++) {
        memo_statistics[i] = 0;
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
}

PyObject *
_PyPegen_get_memo_statistics(تهی)
{
    PyObject *ret = PyList_New(NSTATISTICS);
    اگر (ret == پوچ) {
        بازگشت پوچ;
    }

    PyMutex_Lock(&_PyRuntime.parser.mutex);
    برای (صحیح i = 0; i < NSTATISTICS; i++) {
        PyObject *مقدار = PyLong_FromLong(memo_statistics[i]);
        اگر (مقدار == پوچ) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            بازگشت پوچ;
        }
        // PyList_SetItem borrows a reference to value.
        اگر (PyList_SetItem(ret, i, مقدار) < 0) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            بازگشت پوچ;
        }
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
    بازگشت ret;
}
#endif

صحیح  // bool
_PyPegen_is_memoized(Parser *p, صحیح type, تهی *pres)
{
    اگر (p->mark == p->fill) {
        اگر (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            بازگشت -1;
        }
    }

    Token *t = p->tokens[p->mark];

    اگر (!(t->memo_mask & (1ULL << (type & 63)))) {
        بازگشت 0;
    }

    برای (Memo *m = t->memo; m != پوچ; m = m->next) {
        اگر (m->type == type) {
#اگر defined(Py_DEBUG)
            اگر (0 <= type && type < NSTATISTICS) {
                long شمار = m->mark - p->mark;
                // A memoized negative result counts for one.
                اگر (شمار <= 0) {
                    شمار = 1;
                }
                PyMutex_Lock(&_PyRuntime.parser.mutex);
                memo_statistics[type] += شمار;
                PyMutex_Unlock(&_PyRuntime.parser.mutex);
            }
#endif
            p->mark = m->mark;
            *(تهی **)(pres) = m->گره;
            بازگشت 1;
        }
    }
    بازگشت 0;
}

#define LOOKAHEAD1(NAME, RES_TYPE)                                  \
    صحیح                                                             \
    NAME (صحیح positive, RES_TYPE (func)(Parser *), Parser *p)       \
    {                                                               \
        صحیح mark = p->mark;                                         \
        تهی *res = func(p);                                        \
        p->mark = mark;                                             \
        بازگشت (res != پوچ) == positive;                           \
    }

LOOKAHEAD1(_PyPegen_lookahead, تهی *)
LOOKAHEAD1(Py_Pegen_lookahead_for_عبارت, عبارت_ty)
LOOKAHEAD1(_PyPegen_lookahead_for_stmt, stmt_ty)
#undef LOOKAHEAD1

#define LOOKAHEAD2(NAME, RES_TYPE, T)                                   \
    صحیح                                                                 \
    NAME (صحیح positive, RES_TYPE (func)(Parser *, T), Parser *p, T arg) \
    {                                                                   \
        صحیح mark = p->mark;                                             \
        تهی *res = func(p, arg);                                       \
        p->mark = mark;                                                 \
        بازگشت (res != پوچ) == positive;                               \
    }

LOOKAHEAD2(_PyPegen_lookahead_with_int, Token *, صحیح)
LOOKAHEAD2(_PyPegen_lookahead_with_string, عبارت_ty, ثابت نویسه *)
#undef LOOKAHEAD2

Token *
_PyPegen_expect_token(Parser *p, صحیح type)
{
    اگر (p->mark == p->fill) {
        اگر (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            بازگشت پوچ;
        }
    }
    Token *t = p->tokens[p->mark];
    اگر (t->type != type) {
       بازگشت پوچ;
    }
    p->mark += 1;
    بازگشت t;
}

تهی*
_PyPegen_expect_forced_result(Parser *p, تهی* result, ثابت نویسه* expected) {

    اگر (p->error_indicator == 1) {
        بازگشت پوچ;
    }
    اگر (result == پوچ) {
        RAISE_SYNTAX_ERROR("expected (%s)", expected);
        بازگشت پوچ;
    }
    بازگشت result;
}

Token *
_PyPegen_expect_forced_token(Parser *p, صحیح type, ثابت نویسه* expected) {

    اگر (p->error_indicator == 1) {
        بازگشت پوچ;
    }

    اگر (p->mark == p->fill) {
        اگر (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            بازگشت پوچ;
        }
    }
    Token *t = p->tokens[p->mark];
    اگر (t->type != type) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(t, "expected '%s'", expected);
        بازگشت پوچ;
    }
    p->mark += 1;
    بازگشت t;
}

عبارت_ty
_PyPegen_expect_soft_keyword(Parser *p, ثابت نویسه *keyword)
{
    اگر (p->mark == p->fill) {
        اگر (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            بازگشت پوچ;
        }
    }
    Token *t = p->tokens[p->mark];
    اگر (t->type != NAME) {
        بازگشت پوچ;
    }
    ثابت نویسه *s = PyBytes_AsString(t->bytes);
    اگر (!s) {
        p->error_indicator = 1;
        بازگشت پوچ;
    }
    اگر (strcmp(s, keyword) != 0) {
        بازگشت پوچ;
    }
    بازگشت _PyPegen_name_token(p);
}

Token *
_PyPegen_get_last_nonnwhitespace_token(Parser *p)
{
    أكد(p->mark >= 0);
    Token *token = پوچ;
    برای (صحیح m = p->mark - 1; m >= 0; m--) {
        token = p->tokens[m];
        اگر (token->type != ENDMARKER && (token->type < NEWLINE || token->type > DEDENT)) {
            بشکن;
        }
    }
    بازگشت token;
}

PyObject *
_PyPegen_new_identifier(Parser *p, ثابت نویسه *n)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, (Py_ssize_t)strlen(n), پوچ);
    اگر (!id) {
        برو error;
    }
    /* Check whether there are non-ASCII characters in the
       identifier; if so, normalize to NFKC. */
    اگر (!PyUnicode_IS_ASCII(id))
    {
        اگر (!init_normalization(p))
        {
            Py_DECREF(id);
            برو error;
        }
        PyObject *form = PyUnicode_InternFromString("NFKC");
        اگر (form == پوچ)
        {
            Py_DECREF(id);
            برو error;
        }
        PyObject *args[2] = {form, id};
        PyObject *id2 = PyObject_Vectorcall(p->normalize, args, 2, پوچ);
        Py_DECREF(id);
        Py_DECREF(form);
        اگر (!id2) {
            برو error;
        }

        اگر (!PyUnicode_Check(id2))
        {
            PyErr_Format(PyExc_TypeError,
                         "unicodedata.normalize() must return a string, not "
                         "%.200s",
                         _PyType_Name(Py_TYPE(id2)));
            Py_DECREF(id2);
            برو error;
        }
        id = id2;
    }
    ایستا ثابت نویسه * ثابت forbidden[] = {
        "None",
        "True",
        "False",
        پوچ
    };
    برای (صحیح i = 0; forbidden[i] != پوچ; i++) {
        اگر (_PyUnicode_EqualToASCIIString(id, forbidden[i])) {
            PyErr_Format(Py_Exc_مقدار_Error,
                         "identifier field can't represent '%s' constant",
                         forbidden[i]);
            Py_DECREF(id);
            برو error;
        }
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &id);
    اگر (_PyArena_AddPyObject(p->arena, id) < 0)
    {
        Py_DECREF(id);
        برو error;
    }
    بازگشت id;

error:
    p->error_indicator = 1;
    بازگشت پوچ;
}

// Return an arena-owned identifier; callers borrow the reference.
ایستا PyObject *
get_cached_identifier(Parser *p, PyObject *bytes)
{
    ثابت نویسه *s = PyBytes_AsString(bytes);
    اگر (!s) {
        p->error_indicator = 1;
        بازگشت پوچ;
    }
    // Identifiers repeat constantly; a small span-keyed cache skips the
    // UTF-8 decode + intern for repeated occurrences. Keys point into
    // arena-owned token bytes and values are arena-owned interned strings,
    // so borrowed references are valid for the lifetime of the parse
    // (including the second error pass, which reuses parser and arena).
    Py_ssize_t len = PyBytes_GET_SIZE(bytes);
    Py_hash_t hash = PyObject_Hash(bytes);
    اگر (hash == -1) {
        p->error_indicator = 1;
        بازگشت پوچ;
    }
    // Parses without identifiers do not need this cache.
    اگر (p->identifier_cache == پوچ) {
        p->identifier_cache = PyMem_Calloc(
            IDENTIFIER_CACHE_SIZE, اندازه(*p->identifier_cache));
        اگر (p->identifier_cache == پوچ) {
            p->error_indicator = 1;
            بازگشت PyErr_NoMemory();
        }
    }
    IdentifierCacheEntry *free_slot = پوچ;
    size_t idx = (size_t)hash & (IDENTIFIER_CACHE_SIZE - 1);
    برای (صحیح probe = 0; probe < IDENTIFIER_CACHE_MAX_PROBES; probe++) {
        IdentifierCacheEntry *entry = &p->identifier_cache[
            (idx + probe) & (IDENTIFIER_CACHE_SIZE - 1)];
        اگر (entry->کلید == پوچ) {
            free_slot = entry;
            بشکن;
        }
        اگر (entry->hash == hash && entry->len == len &&
            memcmp(entry->کلید, s, len) == 0)
        {
            بازگشت entry->مقدار;
        }
    }
    PyObject *id = _PyPegen_new_identifier(p, s);
    اگر (id == پوچ) {
        p->error_indicator = 1;
        بازگشت پوچ;
    }
    اگر (free_slot != پوچ) {
        free_slot->کلید = s;
        free_slot->len = len;
        free_slot->hash = hash;
        free_slot->مقدار = id;
    }
    بازگشت id;
}

ایستا عبارت_ty
_PyPegen_name_from_token(Parser *p, Token* t)
{
    اگر (t == پوچ) {
        بازگشت پوچ;
    }
    // Reuse the AST node when backtracking revisits this token. Memo lookup
    // starts before the token and restores the position after it on a hit.
    // Token kinds can be memo keys: generated grammar rule IDs start at 1000.
    صحیح mark = p->mark - 1;
    p->mark = mark;
    عبارت_ty cached = پوچ;
    اگر (_PyPegen_is_memoized(p, NAME, &cached)) {
        بازگشت cached;
    }
    p->mark = mark + 1;
    PyObject *id = get_cached_identifier(p, t->bytes);
    اگر (id == پوچ) {
        بازگشت پوچ;
    }
    عبارت_ty result = _PyAST_Name(id, Load, t->lineno, t->col_offset,
                                t->end_lineno, t->end_col_offset, p->arena);
    اگر (result != پوچ && Py_Pegen_درج_memo(p, mark, NAME, result) < 0) {
        p->error_indicator = 1;
        بازگشت پوچ;
    }
    بازگشت result;
}

عبارت_ty
_PyPegen_name_token(Parser *p)
{
    Token *t = _PyPegen_expect_token(p, NAME);
    بازگشت _PyPegen_name_from_token(p, t);
}

تهی *
_PyPegen_string_token(Parser *p)
{
    بازگشت _PyPegen_expect_token(p, STRING);
}

عبارت_ty _PyPegen_soft_keyword_token(Parser *p) {
    Token *t = _PyPegen_expect_token(p, NAME);
    اگر (t == پوچ) {
        بازگشت پوچ;
    }
    نویسه *the_token;
    Py_ssize_t size;
    PyBytes_AsStringAndSize(t->bytes, &the_token, &size);
    برای (نویسه **keyword = p->soft_keywords; *keyword != پوچ; keyword++) {
        اگر (strlen(*keyword) == (size_t)size &&
            strncmp(*keyword, the_token, (size_t)size) == 0) {
            بازگشت _PyPegen_name_from_token(p, t);
        }
    }
    بازگشت پوچ;
}

ایستا PyObject *
parsenumber_raw(ثابت نویسه *s)
{
    ثابت نویسه *end;
    long x;
    دوگانه dx;
    Py_complex compl;
    صحیح imflag;

    أكد(s != پوچ);
    errno = 0;
    end = s + strlen(s) - 1;
    imflag = *end == 'j' || *end == 'J';
    اگر (s[0] == '0') {
        x = (long)PyOS_strtoul(s, (نویسه **)&end, 0);
        اگر (x < 0 && errno == 0) {
            بازگشت PyLong_FromString(s, (نویسه **)0, 0);
        }
    }
    وگرنه {
        x = PyOS_strtol(s, (نویسه **)&end, 0);
    }
    اگر (*end == '\0') {
        اگر (errno != 0) {
            بازگشت PyLong_FromString(s, (نویسه **)0, 0);
        }
        بازگشت PyLong_FromLong(x);
    }
    /* XXX Huge floats may silently fail */
    اگر (imflag) {
        compl.real = 0.;
        compl.imag = PyOS_string_to_double(s, (نویسه **)&end, پوچ);
        اگر (compl.imag == -1.0 && PyErr_Occurred()) {
            بازگشت پوچ;
        }
        بازگشت PyComplex_FromCComplex(compl);
    }
    dx = PyOS_string_to_double(s, پوچ, پوچ);
    اگر (dx == -1.0 && PyErr_Occurred()) {
        بازگشت پوچ;
    }
    بازگشت PyFloat_FromDouble(dx);
}

ایستا PyObject *
parsenumber(ثابت نویسه *s)
{
    نویسه *dup;
    نویسه *end;
    PyObject *res = پوچ;

    أكد(s != پوچ);

    اگر (strchr(s, '_') == پوچ) {
        بازگشت parsenumber_raw(s);
    }
    /* Create a duplicate without underscores. */
    dup = PyMem_Malloc(strlen(s) + 1);
    اگر (dup == پوچ) {
        بازگشت PyErr_NoMemory();
    }
    end = dup;
    برای (; *s; s++) {
        اگر (*s != '_') {
            *end++ = *s;
        }
    }
    *end = '\0';
    res = parsenumber_raw(dup);
    PyMem_Free(dup);
    بازگشت res;
}

عبارت_ty
Py_Pegen_عدد_token(Parser *p)
{
    صحیح mark = p->mark;
    Token *t = _PyPegen_expect_token(p, عدد);
    اگر (t == پوچ) {
        بازگشت پوچ;
    }

    // Look up the node at the token's start, as for NAME tokens. A memo hit
    // advances the mark; on a miss, restore the position after the token.
    p->mark = mark;
    عبارت_ty cached = پوچ;
    اگر (_PyPegen_is_memoized(p, عدد, &cached)) {
        بازگشت cached;
    }
    p->mark = mark + 1;

    ثابت نویسه *رقم_raw = PyBytes_AsString(t->bytes);
    اگر (رقم_raw == پوچ) {
        p->error_indicator = 1;
        بازگشت پوچ;
    }

    اگر (p->feature_version < 6 && strchr(رقم_raw, '_') != پوچ) {
        p->error_indicator = 1;
        بازگشت RAISE_SYNTAX_ERROR("Underscores in numeric literals are only supported "
                                  "in Python 3.6 and greater");
    }

    PyObject *c = parsenumber(رقم_raw);

    اگر (c == پوچ) {
        p->error_indicator = 1;
        PyThreadState *tstate = _PyThreadState_GET();
        // The only way a ValueError should happen in _this_ code is via
        // PyLong_FromString hitting a length limit.
        اگر (tstate->current_exception != پوچ &&
            Py_TYPE(tstate->current_exception) == (PyTypeObject *)Py_Exc_مقدار_Error
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
        بازگشت پوچ;
    }

    اگر (_PyArena_AddPyObject(p->arena, c) < 0) {
        Py_DECREF(c);
        p->error_indicator = 1;
        بازگشت پوچ;
    }

    عبارت_ty result = _PyAST_Constant(c, پوچ, t->lineno, t->col_offset,
                                    t->end_lineno, t->end_col_offset, p->arena);
    اگر (result != پوچ && Py_Pegen_درج_memo(p, mark, عدد, result) < 0) {
        p->error_indicator = 1;
        بازگشت پوچ;
    }
    بازگشت result;
}



ایستا صحیح
compute_parser_flags(PyCompilerFlags *flags)
{
    صحیح parser_flags = 0;
    اگر (!flags) {
        بازگشت 0;
    }
    اگر (flags->cf_flags & PyCF_DONT_IMPLY_DEDENT) {
        parser_flags |= PyPARSE_DONT_IMPLY_DEDENT;
    }
    اگر (flags->cf_flags & PyCF_IGNORE_COOKIE) {
        parser_flags |= PyPARSE_IGNORE_COOKIE;
    }
    اگر (flags->cf_flags & CO_FUTURE_BARRY_AS_BDFL) {
        parser_flags |= PyPARSE_BARRY_AS_BDFL;
    }
    اگر (flags->cf_flags & PyCF_TYPE_COMMENTS) {
        parser_flags |= PyPARSE_TYPE_COMMENTS;
    }
    اگر (flags->cf_flags & Py_CF_ALLOW_INCOMPLETE_درون_داد) {
        parser_flags |= Py_PARSE_ALLOW_INCOMPLETE_درون_داد;
    }
    بازگشت parser_flags;
}

// Parser API

Parser *
_PyPegen_Parser_New(ساختار tok_state *tok, صحیح start_rule, صحیح flags,
                    صحیح feature_version, صحیح *errcode, ثابت نویسه* source, PyArena *arena)
{
    Parser *p = PyMem_Malloc(اندازه(Parser));
    اگر (p == پوچ) {
        بازگشت (Parser *) PyErr_NoMemory();
    }
    أكد(tok != پوچ);
    _PyTokenizer_SetOptions(tok, 0, (flags & PyPARSE_TYPE_COMMENTS) > 0);
    Py_Tokenizer_Allow_Incomplete_درون_داد(tok, flags & Py_PARSE_ALLOW_INCOMPLETE_درون_داد);
    p->tok = tok;
    p->keywords = پوچ;
    p->n_keyword_lists = -1;
    p->soft_keywords = پوچ;
    p->tokens = PyMem_Malloc(اندازه(Token *));
    اگر (!p->tokens) {
        PyMem_Free(p);
        بازگشت (Parser *) PyErr_NoMemory();
    }
    p->tokens[0] = PyMem_Calloc(1, اندازه(Token));
    اگر (!p->tokens[0]) {
        PyMem_Free(p->tokens);
        PyMem_Free(p);
        بازگشت (Parser *) PyErr_NoMemory();
    }
    p->type_ignore_comments = (growable_comment_array){0};

    p->mark = 0;
    p->fill = 0;
    p->size = 1;

    p->errcode = errcode;
    p->arena = arena;
    p->start_rule = start_rule;
    p->parsing_started = 0;
    p->normalize = پوچ;
    p->error_indicator = 0;

    p->starting_lineno = 0;
    p->starting_col_offset = 0;
    p->flags = flags;
    p->feature_version = feature_version;
    p->known_err_token = پوچ;
    p->identifier_cache = پوچ;
    p->tstate = PyThreadState_Get();
    // Stack limits are initialized when the thread state is attached.
    أكد(((_PyThreadStateImpl *)p->tstate)->c_پشته_hard_limit != 0);
    p->پشته_soft_limit = ((_PyThreadStateImpl *)p->tstate)->c_پشته_soft_limit;
#اگر Py_پشته_GROWS_DOWN
    p->پشته_soft_limit += Py_OS_پشته_MARGIN_BYTES;
#وگرنه
    p->پشته_soft_limit -= Py_OS_پشته_MARGIN_BYTES;
#endif
    p->level = 0;
    p->call_invalid_rules = 0;
    p->درون_داد_exhausted = 0;
    p->last_stmt_location.lineno = 0;
    p->last_stmt_location.col_offset = 0;
    p->last_stmt_location.end_lineno = 0;
    p->last_stmt_location.end_col_offset = 0;
#ifdef Py_DEBUG
    p->debug = _Py_GetConfig()->parser_debug;
#endif
    بازگشت p;
}

تهی
_PyPegen_Parser_Free(Parser *p)
{
    PyMem_Free(p->identifier_cache);
    Py_XDECREF(p->normalize);
    // Resizes allocate blocks starting at indices 1, 2, 4, and so on.
    PyMem_Free(p->tokens[0]);
    برای (صحیح i = 1; i < p->size; i *= PEGEN_ARRAY_GROWTH_FACTOR) {
        PyMem_Free(p->tokens[i]);
    }
    PyMem_Free(p->tokens);
    growable_comment_array_deallocate(&p->type_ignore_comments);
    PyMem_Free(p);
}

ایستا تهی
reset_parser_state_for_error_pass(Parser *p)
{
    p->last_stmt_location.lineno = 0;
    p->last_stmt_location.col_offset = 0;
    p->last_stmt_location.end_lineno = 0;
    p->last_stmt_location.end_col_offset = 0;
    برای (صحیح i = 0; i < p->fill; i++) {
        p->tokens[i]->memo = پوچ;
        p->tokens[i]->memo_mask = 0;
    }
    p->mark = 0;
    p->flags &= ~Py_PARSE_ALLOW_INCOMPLETE_درون_داد;
    Py_Tokenizer_Allow_Incomplete_درون_داد(p->tok, 0);
    p->call_invalid_rules = 1;
    _PyTokenizer_StopInteractive(p->tok);
}

ایستا تهی
_PyPegen_set_syntax_error_metadata(Parser *p) {
    PyObject *exc = PyErr_GetRaisedException();
    اگر (!exc || !PyObject_TypeCheck(exc, (PyTypeObject *)PyExc_SyntaxError)) {
        PyErr_SetRaisedException(exc);
        بازگشت;
    }
    ثابت نویسه *source = _PyTokenizer_RetainedSource(p->tok);
    ثابت نویسه *encoding = _PyTokenizer_GetInfo(p->tok).encoding;
    PyObject* the_source = پوچ;
    اگر (source) {
        اگر (encoding == پوچ) {
            the_source = PyUnicode_FromString(source);
        } وگرنه {
            the_source = PyUnicode_Decode(source, strlen(source), encoding, پوچ);
        }
    }
    اگر (!the_source) {
        PyErr_Clear();
        the_source = Py_None;
        Py_INCREF(the_source);
    }
    PyObject* metadata = Py_Build_مقدار(
        "(iiN)",
        p->last_stmt_location.lineno,
        p->last_stmt_location.col_offset,
        the_source // N gives ownership to metadata
    );
    اگر (!metadata) {
        PyErr_Clear();
        PyErr_SetRaisedException(exc);
        بازگشت;
    }
    PySyntaxErrorObject *syntax_error = (PySyntaxErrorObject *)exc;

    Py_XDECREF(syntax_error->metadata);
    syntax_error->metadata = metadata;
    PyErr_SetRaisedException(exc);
}

تهی *
_PyPegen_run_parser(Parser *p)
{
    تهی *res = _PyPegen_parse(p);
    أكد(p->level == 0);
    اگر (res != پوچ && PyErr_Occurred()) {
        // Discard a result returned with an exception still pending
        // (e.g. a MemoryError from a recovered-from allocation failure).
        بازگشت پوچ;
    }
    اگر (res == پوچ) {
        اگر (PyErr_Occurred() &&
            (PyErr_ExceptionMatches(Py_Exc_Incomplete_درون_داد_Error) ||
             !PyErr_ExceptionMatches(PyExc_SyntaxError))) {
            بازگشت پوچ;
        }
        اگر ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_درون_داد) &&
            p->درون_داد_exhausted && !PyErr_Occurred()) {
            بازگشت Py_Pegen_raise_incomplete_درون_داد(p);
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
        اگر (PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_set_syntax_error_metadata(p);
        }
       بازگشت پوچ;
    }

    اگر (p->start_rule == Py_single_درون_داد && _PyTokenizer_HasTrailingStatement(p->tok)) {
        بازگشت RAISE_SYNTAX_ERROR("multiple statements found while compiling a single statement");
    }

    // test_peg_generator defines _Py_TEST_PEGEN to not call PyAST_Validate()
#اگر defined(Py_DEBUG) && !defined(_Py_TEST_PEGEN)
    اگر (p->start_rule == Py_single_درون_داد ||
        p->start_rule == Py_file_درون_داد ||
        p->start_rule == Py_eval_درون_داد)
    {
        اگر (!_PyAST_Validate(res)) {
            بازگشت پوچ;
        }
    }
#endif
    بازگشت res;
}

mod_ty
_PyPegen_run_parser_from_file_pointer(FILE *fp, صحیح start_rule, PyObject *filename_ob,
                             ثابت نویسه *enc, ثابت نویسه *ps1, ثابت نویسه *ps2,
                             PyCompilerFlags *flags, صحیح *errcode,
                             PyObject **interactive_src, PyArena *arena)
{
    صحیح parser_flags = compute_parser_flags(flags);
    ساختار tok_state *tok = _PyTokenizer_FromFile(fp, enc, ps1, ps2);
    اگر (tok == پوچ) {
        اگر (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        وگرنه {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        بازگشت پوچ;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    // From here on we need to clean up even if there's an error
    mod_ty result = پوچ;

    PyObject *module = PyUnicode_FromString("__main__");
    اگر (module == پوچ) {
        برو error;
    }
    _PyTokenizer_SetContext(tok, filename_ob, module);
    Py_DECREF(module);

    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, PY_MINOR_VERSION,
                                    errcode, پوچ, arena);
    اگر (p == پوچ) {
        برو error;
    }

    result = _PyPegen_run_parser(p);
    _PyPegen_Parser_Free(p);

    ثابت نویسه *source = _PyTokenizer_RetainedSource(tok);
    اگر (source != پوچ && result && interactive_src != پوچ) {
        *interactive_src = PyUnicode_FromString(source);
        اگر (!*interactive_src || _PyArena_AddPyObject(arena, *interactive_src) < 0) {
            Py_XDECREF(*interactive_src);
            result = پوچ;
            برو error;
        }
    }

error:
    _PyTokenizer_Free(tok);
    بازگشت result;
}

mod_ty
_PyPegen_run_parser_from_string(ثابت نویسه *str, صحیح start_rule, PyObject *filename_ob,
                       PyCompilerFlags *flags, PyArena *arena, PyObject *module,
                       PyObject **incomplete)
{
    _PyTokenizer_Newline newline_mode = start_rule == Py_file_درون_داد
        ? _PyTokenizer_IMPLY_NEWLINE : _PyTokenizer_KEEP_NEWLINE;
    صحیح parser_flags = compute_parser_flags(flags);
    اگر (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_درون_داد) {
        newline_mode = _PyTokenizer_APPEND_NEWLINE;
    }

    ساختار tok_state *tok;
    اگر (flags != پوچ && flags->cf_flags & PyCF_IGNORE_COOKIE) {
        tok = _PyTokenizer_FromUTF8(str, newline_mode, 0);
    } وگرنه {
        tok = _PyTokenizer_FromString(str, newline_mode, 0);
    }
    اگر (tok == پوچ) {
        اگر (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        وگرنه {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        بازگشت پوچ;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    _PyTokenizer_SetContext(tok, filename_ob, module);

    // We need to clear up from here on
    mod_ty result = پوچ;

    صحیح feature_version = flags && (flags->cf_flags & PyCF_ONLY_AST) ?
        flags->cf_feature_version : PY_MINOR_VERSION;
    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, feature_version,
                                    پوچ, str, arena);
    اگر (p == پوچ) {
        برو error;
    }

    result = _PyPegen_run_parser(p);
    اگر (result != پوچ && (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_درون_داد)) {
        صحیح needs_newline = _PyTokenizer_GetNewlineRequirements(tok);
        اگر ((needs_newline & _PyTokenizer_NEWLINE_CONTINUATION) ||
            (start_rule != Py_file_درون_داد &&
             (needs_newline & _PyTokenizer_NEWLINE_INDENT)) ||
            (start_rule == Py_single_درون_داد && p->درون_داد_exhausted &&
             (needs_newline & _PyTokenizer_NEWLINE_STATEMENT))) {
            Py_Pegen_raise_incomplete_درون_داد(p);
            اگر (incomplete != پوچ && PyErr_ExceptionMatches(Py_Exc_Incomplete_درون_داد_Error)) {
                *incomplete = PyErr_GetRaisedException();
            }
            وگرنه {
                result = پوچ;
            }
        }
    }
    _PyPegen_Parser_Free(p);

error:
    _PyTokenizer_Free(tok);
    بازگشت result;
}
