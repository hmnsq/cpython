#அந்தர்பாவ <Python.h>
#அந்தர்பாவ "pycore_ast.h"           // _PyAST_Validate(),
#அந்தர்பாவ "pycore_pystate.h"       // _PyThreadState_GET()
#அந்தர்பாவ "pycore_parser.h"        // _PYPEGEN_NSTATISTICS
#அந்தர்பாவ "pycore_pyerrors.h"      // PyExc_IncompleteInputError
#அந்தர்பாவ "pycore_runtime.h"       // _PyRuntime
#அந்தர்பாவ "pycore_unicodeobject.h" // _PyUnicode_InternImmortal
#அந்தர்பாவ <errcode.h>

#அந்தர்பாவ "lexer/state.h"
#அந்தர்பாவ "tokenizer/tokenizer.h"
#அந்தர்பாவ "pegen.h"

#define IDENTIFIER_CACHE_SIZE 2048  // Must be a power of two.
#define IDENTIFIER_CACHE_MAX_PROBES 8
#define PEGEN_ARRAY_GROWTH_FACTOR 2
#define TYPE_IGNORE_INITIAL_CAPACITY 10

கட்டமைப்பு _identifier_cache_entry {
    மாறிலி எழுத்து *சாவி;   // Borrowed from arena-owned token bytes.
    Py_ssize_t len;
    Py_hash_t hash;
    PyObject *மதிப்பு;   // Borrowed from an arena-owned identifier.
};

// Internal parser functions

asdl_stmt_seq*
_PyPegen_interactive_exit(Parser *p)
{
    எனில் (p->errcode) {
        *(p->errcode) = E_EOF;
    }
    திரும்பு வெற்று;
}

Py_ssize_t
Py_Pegen_byte_offset_to_character_offset_வரி(PyObject *வரி, Py_ssize_t col_offset, Py_ssize_t end_col_offset)
{
    மாறிலி unsigned எழுத்து *data = (மாறிலி unsigned எழுத்து*)PyUnicode_AsUTF8(வரி);
    எனில் (data == வெற்று) {
        திரும்பு -1;
    }

    Py_ssize_t len = 0;
    போது (col_offset < end_col_offset) {
        Py_UCS4 ch = data[col_offset];
        எனில் (ch < 0x80) {
            col_offset += 1;
        } இல்லையெனில் எனில் ((ch & 0xe0) == 0xc0) {
            col_offset += 2;
        } இல்லையெனில் எனில் ((ch & 0xf0) == 0xe0) {
            col_offset += 3;
        } இல்லையெனில் எனில் ((ch & 0xf8) == 0xf0) {
            col_offset += 4;
        } இல்லையெனில் {
            PyErr_SetString(Py_Exc_மதிப்பு_Error, "Invalid UTF-8 sequence");
            திரும்பு -1;
        }
        len++;
    }
    திரும்பு len;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset_raw(மாறிலி எழுத்து* str, Py_ssize_t col_offset)
{
    Py_ssize_t len = (Py_ssize_t)strlen(str);
    எனில் (col_offset > len + 1) {
        col_offset = len + 1;
    }
    ப்ரதிஜ்ஞா(col_offset >= 0);
    PyObject *text = PyUnicode_DecodeUTF8(str, col_offset, "replace");
    எனில் (!text) {
        திரும்பு -1;
    }
    Py_ssize_t size = PyUnicode_GET_LENGTH(text);
    Py_DECREF(text);
    திரும்பு size;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset(PyObject *வரி, Py_ssize_t col_offset)
{
    மாறிலி எழுத்து *str = PyUnicode_AsUTF8(வரி);
    எனில் (!str) {
        திரும்பு -1;
    }
    திரும்பு _PyPegen_byte_offset_to_character_offset_raw(str, col_offset);
}

// Here, mark is the start of the node, while p->mark is the end.
// If node==NULL, they should be the same.
முழு
Py_Pegen_செருகு_memo(Parser *p, முழு mark, முழு type, வெறுமை *முனை)
{
    // Insert in front
    Memo *m = _PyArena_Malloc(p->arena, அளவு(Memo));
    எனில் (m == வெற்று) {
        திரும்பு -1;
    }
    m->type = type;
    m->முனை = முனை;
    m->mark = p->mark;
    m->next = p->tokens[mark]->memo;
    p->tokens[mark]->memo = m;
    p->tokens[mark]->memo_mask |= 1ULL << (type & 63);
    திரும்பு 0;
}

// Like _PyPegen_insert_memo(), but returns the inserted Memo so callers
// can update it in place without re-walking the token's memo list.
Memo *
Py_Pegen_செருகு_memo_direct(Parser *p, முழு mark, முழு type)
{
    எனில் (Py_Pegen_செருகு_memo(p, mark, type, வெற்று) < 0) {
        திரும்பு வெற்று;
    }
    திரும்பு p->tokens[mark]->memo;
}

// Like _PyPegen_insert_memo(), but updates an existing node if found.
முழு
_PyPegen_update_memo(Parser *p, முழு mark, முழு type, வெறுமை *முனை)
{
    க்கு (Memo *m = p->tokens[mark]->memo; m != வெற்று; m = m->next) {
        எனில் (m->type == type) {
            // Update existing node.
            m->முனை = முனை;
            m->mark = p->mark;
            திரும்பு 0;
        }
    }
    // Insert new node.
    திரும்பு Py_Pegen_செருகு_memo(p, mark, type, முனை);
}

நிலையான முழு
init_normalization(Parser *p)
{
    எனில் (p->normalize) {
        திரும்பு 1;
    }
    p->normalize = PyImport_ImportModuleAttrString("unicodedata", "normalize");
    எனில் (!p->normalize)
    {
        திரும்பு 0;
    }
    திரும்பு 1;
}

முழு
_PyPegen_grow_loop_buffer(வெறுமை ***buffer, Py_ssize_t *capacity)
{
    எனில் ((size_t)*capacity > (size_t)PY_SSIZE_T_MAX /
            (PEGEN_ARRAY_GROWTH_FACTOR * அளவு(**buffer))) {
        திரும்பு -1;
    }
    Py_ssize_t new_capacity = *capacity == 0
        ? 1 : *capacity * PEGEN_ARRAY_GROWTH_FACTOR;
    வெறுமை **new_buffer = PyMem_Realloc(*buffer, new_capacity * அளவு(**buffer));
    எனில் (new_buffer == வெற்று) {
        திரும்பு -1;
    }
    *buffer = new_buffer;
    *capacity = new_capacity;
    திரும்பு 0;
}

நிலையான முழு
growable_comment_array_add(growable_comment_array *arr, முழு lineno, எழுத்து *comment)
{
    எனில் (arr->இலக்கம்_பொருட்கள் >= arr->size) {
        எனில் (arr->size > (size_t)PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * அளவு(*arr->பொருட்கள்))) {
            திரும்பு 0;
        }
        size_t new_size = arr->size == 0
            ? TYPE_IGNORE_INITIAL_CAPACITY
            : arr->size * PEGEN_ARRAY_GROWTH_FACTOR;
        வெறுமை *new_பொருட்கள்_array = PyMem_Realloc(
            arr->பொருட்கள், new_size * அளவு(*arr->பொருட்கள்));
        எனில் (!new_பொருட்கள்_array) {
            திரும்பு 0;
        }
        arr->பொருட்கள் = new_பொருட்கள்_array;
        arr->size = new_size;
    }

    arr->பொருட்கள்[arr->இலக்கம்_பொருட்கள்].lineno = lineno;
    arr->பொருட்கள்[arr->இலக்கம்_பொருட்கள்].comment = comment;  // Take ownership
    arr->இலக்கம்_பொருட்கள்++;
    திரும்பு 1;
}

நிலையான வெறுமை
growable_comment_array_deallocate(growable_comment_array *arr)
{
    க்கு (size_t i = 0; i < arr->இலக்கம்_பொருட்கள்; i++) {
        PyMem_Free(arr->பொருட்கள்[i].comment);
    }
    PyMem_Free(arr->பொருட்கள்);
}

நிலையான முழு
_get_keyword_or_name_type(Parser *p, மாறிலி எழுத்து *text, Py_ssize_t length)
{
    ப்ரதிஜ்ஞா(length > 0);

    எனில் (length >= p->n_keyword_lists ||
        p->keywords[length] == வெற்று ||
        p->keywords[length]->type == -1) {
        திரும்பு NAME;
    }
    க்கு (KeywordToken *k = p->keywords[length]; k != வெற்று && k->type != -1; k++) {
        எனில் (memcmp(k->str, text, (size_t)length) == 0) {
            திரும்பு k->type;
        }
    }
    திரும்பு NAME;
}

// Keep text only where grammar actions or helpers read it. Keyword error
// actions use fixed spellings, so keywords do not need their text.
நிலையான உள்வரி முழு
token_needs_text(முழு type)
{
    தேர்வு (type) {
        நிலை NAME:
        நிலை எண்:
        நிலை STRING:
        நிலை FSTRING_START:
        நிலை FSTRING_நடு:
        நிலை FSTRING_END:
        நிலை TSTRING_START:
        நிலை TSTRING_நடு:
        நிலை TSTRING_END:
        நிலை TYPE_COMMENT:
        நிலை NOTEQUAL:  // _PyPegen_check_barry_as_flufl() distinguishes != and <>.
            திரும்பு 1;
        இயல்பு:
            திரும்பு 0;
    }
}

நிலையான முழு
initialize_token(Parser *p, Token *parser_token, கட்டமைப்பு token *new_token, முழு token_type) {
    ப்ரதிஜ்ஞா(parser_token != வெற்று);

    parser_token->type = token_type;
    parser_token->bytes = வெற்று;
    எனில் (token_needs_text(token_type) && new_token->span.start >= 0) {
        Py_ssize_t length;
        மாறிலி எழுத்து *text = _PyToken_TextView(p->tok, new_token, &length);
        எனில் (token_type == NAME) {
            parser_token->type = _get_keyword_or_name_type(p, text, length);
        }
        // A name reclassified as a keyword does not need its text.
        எனில் (parser_token->type == token_type) {
            parser_token->bytes = PyBytes_FromStringAndSize(text, length);
            எனில் (parser_token->bytes == வெற்று) {
                திரும்பு -1;
            }
            எனில் (_PyArena_AddPyObject(p->arena, parser_token->bytes) < 0) {
                Py_DECREF(parser_token->bytes);
                திரும்பு -1;
            }
        }
    }

    parser_token->metadata = வெற்று;
    எனில் (new_token->metadata != வெற்று) {
        எனில் (_PyArena_AddPyObject(p->arena, new_token->metadata) < 0) {
            திரும்பு -1;
        }
        parser_token->metadata = new_token->metadata;
        new_token->metadata = வெற்று;
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

    எனில் (token_type == ERRORTOKEN && _PyTokenizer_GetStatus(p->tok) == E_DECODE) {
        திரும்பு _Pypegen_raise_decode_error(p);
    }

    திரும்பு (token_type == ERRORTOKEN ? _Pypegen_tokenizer_error(p) : 0);
}

நிலையான முழு
_resize_tokens_array(Parser *p) {
    எனில் (p->size > INT_MAX / PEGEN_ARRAY_GROWTH_FACTOR ||
            (size_t)p->size > PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * அளவு(*p->tokens))) {
        PyErr_NoMemory();
        திரும்பு -1;
    }
    முழு newsize = p->size * PEGEN_ARRAY_GROWTH_FACTOR;
    Token **new_tokens = PyMem_Realloc(p->tokens, (size_t)newsize * அளவு(Token *));
    எனில் (new_tokens == வெற்று) {
        PyErr_NoMemory();
        திரும்பு -1;
    }
    p->tokens = new_tokens;

    // Allocate new tokens together without moving existing ones: parser rules
    // may still hold pointers into earlier blocks. Parser_Free frees each block.
    Token *chunk = PyMem_Calloc((size_t)(newsize - p->size), அளவு(Token));
    எனில் (chunk == வெற்று) {
        PyErr_NoMemory();
        திரும்பு -1;
    }
    க்கு (முழு i = p->size; i < newsize; i++) {
        p->tokens[i] = &chunk[i - p->size];
    }
    p->size = newsize;
    திரும்பு 0;
}

முழு
_PyPegen_fill_token(Parser *p)
{
    கட்டமைப்பு token new_token;
    _PyToken_Init(&new_token);
    _PyTokenizer_Get(p->tok, &new_token);

    // Record and skip '# type: ignore' comments
    போது (new_token.type == TYPE_IGNORE) {
        Py_ssize_t len;
        மாறிலி எழுத்து *text = _PyToken_TextView(p->tok, &new_token, &len);
        எழுத்து *tag = PyMem_Malloc((size_t)len + 1);
        எனில் (tag == வெற்று) {
            PyErr_NoMemory();
            செல் error;
        }
        memcpy(tag, text, (size_t)len);
        tag[len] = '\0';
        // Ownership of tag passes to the growable array
        எனில் (!growable_comment_array_add(&p->type_ignore_comments, new_token.end_loc.lineno, tag)) {
            PyMem_Free(tag);
            PyErr_NoMemory();
            செல் error;
        }
        _PyTokenizer_Get(p->tok, &new_token);
    }

    முழு type = new_token.type;
    // EOF can finish an ordinary token. Only a boundary token means that
    // the grammar requested input beyond the last physical line.
    எனில் (type == ENDMARKER ||
        (type == DEDENT && (p->flags & Py_PARSE_ALLOW_INCOMPLETE_உள்ளீடு) &&
         _PyTokenizer_GetStatus(p->tok) == E_EOF)) {
        p->உள்ளீடு_exhausted = 1;
    }

    // If we have reached the end and we are in single input mode we need to insert a newline and reset the parsing
    எனில் (p->start_rule == Py_single_உள்ளீடு && type == ENDMARKER && p->parsing_started) {
        type = NEWLINE; /* Add an extra newline */
        p->parsing_started = 0;

        எனில் (!(p->flags & PyPARSE_DONT_IMPLY_DEDENT)) {
            _PyTokenizer_ImplyDedents(p->tok);
        }
    }
    இல்லையெனில் {
        p->parsing_started = 1;
    }

    // Check if we are at the limit of the token array capacity and resize if needed
    எனில் ((p->fill == p->size) && (_resize_tokens_array(p) != 0)) {
        செல் error;
    }

    Token *t = p->tokens[p->fill];
    முழு result = initialize_token(p, t, &new_token, type);
    _PyToken_Free(&new_token);
    திரும்பு result;
error:
    _PyToken_Free(&new_token);
    திரும்பு -1;
}

#எனில் defined(Py_DEBUG)
// Instrumentation to count the effectiveness of memoization.
// The array counts the number of tokens skipped by memoization,
// indexed by type.

#define NSTATISTICS _PYPEGEN_NSTATISTICS
#define memo_statistics _PyRuntime.parser.memo_statistics

வெறுமை
_PyPegen_clear_memo_statistics(வெறுமை)
{
    PyMutex_Lock(&_PyRuntime.parser.mutex);
    க்கு (முழு i = 0; i < NSTATISTICS; i++) {
        memo_statistics[i] = 0;
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
}

PyObject *
_PyPegen_get_memo_statistics(வெறுமை)
{
    PyObject *ret = PyList_New(NSTATISTICS);
    எனில் (ret == வெற்று) {
        திரும்பு வெற்று;
    }

    PyMutex_Lock(&_PyRuntime.parser.mutex);
    க்கு (முழு i = 0; i < NSTATISTICS; i++) {
        PyObject *மதிப்பு = PyLong_FromLong(memo_statistics[i]);
        எனில் (மதிப்பு == வெற்று) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            திரும்பு வெற்று;
        }
        // PyList_SetItem borrows a reference to value.
        எனில் (PyList_SetItem(ret, i, மதிப்பு) < 0) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            திரும்பு வெற்று;
        }
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
    திரும்பு ret;
}
#endif

முழு  // bool
_PyPegen_is_memoized(Parser *p, முழு type, வெறுமை *pres)
{
    எனில் (p->mark == p->fill) {
        எனில் (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            திரும்பு -1;
        }
    }

    Token *t = p->tokens[p->mark];

    எனில் (!(t->memo_mask & (1ULL << (type & 63)))) {
        திரும்பு 0;
    }

    க்கு (Memo *m = t->memo; m != வெற்று; m = m->next) {
        எனில் (m->type == type) {
#எனில் defined(Py_DEBUG)
            எனில் (0 <= type && type < NSTATISTICS) {
                long மொத்தம் = m->mark - p->mark;
                // A memoized negative result counts for one.
                எனில் (மொத்தம் <= 0) {
                    மொத்தம் = 1;
                }
                PyMutex_Lock(&_PyRuntime.parser.mutex);
                memo_statistics[type] += மொத்தம்;
                PyMutex_Unlock(&_PyRuntime.parser.mutex);
            }
#endif
            p->mark = m->mark;
            *(வெறுமை **)(pres) = m->முனை;
            திரும்பு 1;
        }
    }
    திரும்பு 0;
}

#define LOOKAHEAD1(NAME, RES_TYPE)                                  \
    முழு                                                             \
    NAME (முழு positive, RES_TYPE (func)(Parser *), Parser *p)       \
    {                                                               \
        முழு mark = p->mark;                                         \
        வெறுமை *res = func(p);                                        \
        p->mark = mark;                                             \
        திரும்பு (res != வெற்று) == positive;                           \
    }

LOOKAHEAD1(_PyPegen_lookahead, வெறுமை *)
LOOKAHEAD1(Py_Pegen_lookahead_for_கோவை, கோவை_ty)
LOOKAHEAD1(_PyPegen_lookahead_for_stmt, stmt_ty)
#undef LOOKAHEAD1

#define LOOKAHEAD2(NAME, RES_TYPE, T)                                   \
    முழு                                                                 \
    NAME (முழு positive, RES_TYPE (func)(Parser *, T), Parser *p, T arg) \
    {                                                                   \
        முழு mark = p->mark;                                             \
        வெறுமை *res = func(p, arg);                                       \
        p->mark = mark;                                                 \
        திரும்பு (res != வெற்று) == positive;                               \
    }

LOOKAHEAD2(_PyPegen_lookahead_with_int, Token *, முழு)
LOOKAHEAD2(_PyPegen_lookahead_with_string, கோவை_ty, மாறிலி எழுத்து *)
#undef LOOKAHEAD2

Token *
_PyPegen_expect_token(Parser *p, முழு type)
{
    எனில் (p->mark == p->fill) {
        எனில் (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            திரும்பு வெற்று;
        }
    }
    Token *t = p->tokens[p->mark];
    எனில் (t->type != type) {
       திரும்பு வெற்று;
    }
    p->mark += 1;
    திரும்பு t;
}

வெறுமை*
_PyPegen_expect_forced_result(Parser *p, வெறுமை* result, மாறிலி எழுத்து* expected) {

    எனில் (p->error_indicator == 1) {
        திரும்பு வெற்று;
    }
    எனில் (result == வெற்று) {
        RAISE_SYNTAX_ERROR("expected (%s)", expected);
        திரும்பு வெற்று;
    }
    திரும்பு result;
}

Token *
_PyPegen_expect_forced_token(Parser *p, முழு type, மாறிலி எழுத்து* expected) {

    எனில் (p->error_indicator == 1) {
        திரும்பு வெற்று;
    }

    எனில் (p->mark == p->fill) {
        எனில் (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            திரும்பு வெற்று;
        }
    }
    Token *t = p->tokens[p->mark];
    எனில் (t->type != type) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(t, "expected '%s'", expected);
        திரும்பு வெற்று;
    }
    p->mark += 1;
    திரும்பு t;
}

கோவை_ty
_PyPegen_expect_soft_keyword(Parser *p, மாறிலி எழுத்து *keyword)
{
    எனில் (p->mark == p->fill) {
        எனில் (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            திரும்பு வெற்று;
        }
    }
    Token *t = p->tokens[p->mark];
    எனில் (t->type != NAME) {
        திரும்பு வெற்று;
    }
    மாறிலி எழுத்து *s = PyBytes_AsString(t->bytes);
    எனில் (!s) {
        p->error_indicator = 1;
        திரும்பு வெற்று;
    }
    எனில் (strcmp(s, keyword) != 0) {
        திரும்பு வெற்று;
    }
    திரும்பு _PyPegen_name_token(p);
}

Token *
_PyPegen_get_last_nonnwhitespace_token(Parser *p)
{
    ப்ரதிஜ்ஞா(p->mark >= 0);
    Token *token = வெற்று;
    க்கு (முழு m = p->mark - 1; m >= 0; m--) {
        token = p->tokens[m];
        எனில் (token->type != ENDMARKER && (token->type < NEWLINE || token->type > DEDENT)) {
            நிறுத்து;
        }
    }
    திரும்பு token;
}

PyObject *
_PyPegen_new_identifier(Parser *p, மாறிலி எழுத்து *n)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, (Py_ssize_t)strlen(n), வெற்று);
    எனில் (!id) {
        செல் error;
    }
    /* Check whether there are non-ASCII characters in the
       identifier; if so, normalize to NFKC. */
    எனில் (!PyUnicode_IS_ASCII(id))
    {
        எனில் (!init_normalization(p))
        {
            Py_DECREF(id);
            செல் error;
        }
        PyObject *form = PyUnicode_InternFromString("NFKC");
        எனில் (form == வெற்று)
        {
            Py_DECREF(id);
            செல் error;
        }
        PyObject *args[2] = {form, id};
        PyObject *id2 = PyObject_Vectorcall(p->normalize, args, 2, வெற்று);
        Py_DECREF(id);
        Py_DECREF(form);
        எனில் (!id2) {
            செல் error;
        }

        எனில் (!PyUnicode_Check(id2))
        {
            PyErr_Format(PyExc_TypeError,
                         "unicodedata.normalize() must return a string, not "
                         "%.200s",
                         _PyType_Name(Py_TYPE(id2)));
            Py_DECREF(id2);
            செல் error;
        }
        id = id2;
    }
    நிலையான மாறிலி எழுத்து * மாறிலி forbidden[] = {
        "None",
        "True",
        "False",
        வெற்று
    };
    க்கு (முழு i = 0; forbidden[i] != வெற்று; i++) {
        எனில் (_PyUnicode_EqualToASCIIString(id, forbidden[i])) {
            PyErr_Format(Py_Exc_மதிப்பு_Error,
                         "identifier field can't represent '%s' constant",
                         forbidden[i]);
            Py_DECREF(id);
            செல் error;
        }
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &id);
    எனில் (_PyArena_AddPyObject(p->arena, id) < 0)
    {
        Py_DECREF(id);
        செல் error;
    }
    திரும்பு id;

error:
    p->error_indicator = 1;
    திரும்பு வெற்று;
}

// Return an arena-owned identifier; callers borrow the reference.
நிலையான PyObject *
get_cached_identifier(Parser *p, PyObject *bytes)
{
    மாறிலி எழுத்து *s = PyBytes_AsString(bytes);
    எனில் (!s) {
        p->error_indicator = 1;
        திரும்பு வெற்று;
    }
    // Identifiers repeat constantly; a small span-keyed cache skips the
    // UTF-8 decode + intern for repeated occurrences. Keys point into
    // arena-owned token bytes and values are arena-owned interned strings,
    // so borrowed references are valid for the lifetime of the parse
    // (including the second error pass, which reuses parser and arena).
    Py_ssize_t len = PyBytes_GET_SIZE(bytes);
    Py_hash_t hash = PyObject_Hash(bytes);
    எனில் (hash == -1) {
        p->error_indicator = 1;
        திரும்பு வெற்று;
    }
    // Parses without identifiers do not need this cache.
    எனில் (p->identifier_cache == வெற்று) {
        p->identifier_cache = PyMem_Calloc(
            IDENTIFIER_CACHE_SIZE, அளவு(*p->identifier_cache));
        எனில் (p->identifier_cache == வெற்று) {
            p->error_indicator = 1;
            திரும்பு PyErr_NoMemory();
        }
    }
    IdentifierCacheEntry *free_slot = வெற்று;
    size_t idx = (size_t)hash & (IDENTIFIER_CACHE_SIZE - 1);
    க்கு (முழு probe = 0; probe < IDENTIFIER_CACHE_MAX_PROBES; probe++) {
        IdentifierCacheEntry *entry = &p->identifier_cache[
            (idx + probe) & (IDENTIFIER_CACHE_SIZE - 1)];
        எனில் (entry->சாவி == வெற்று) {
            free_slot = entry;
            நிறுத்து;
        }
        எனில் (entry->hash == hash && entry->len == len &&
            memcmp(entry->சாவி, s, len) == 0)
        {
            திரும்பு entry->மதிப்பு;
        }
    }
    PyObject *id = _PyPegen_new_identifier(p, s);
    எனில் (id == வெற்று) {
        p->error_indicator = 1;
        திரும்பு வெற்று;
    }
    எனில் (free_slot != வெற்று) {
        free_slot->சாவி = s;
        free_slot->len = len;
        free_slot->hash = hash;
        free_slot->மதிப்பு = id;
    }
    திரும்பு id;
}

நிலையான கோவை_ty
_PyPegen_name_from_token(Parser *p, Token* t)
{
    எனில் (t == வெற்று) {
        திரும்பு வெற்று;
    }
    // Reuse the AST node when backtracking revisits this token. Memo lookup
    // starts before the token and restores the position after it on a hit.
    // Token kinds can be memo keys: generated grammar rule IDs start at 1000.
    முழு mark = p->mark - 1;
    p->mark = mark;
    கோவை_ty cached = வெற்று;
    எனில் (_PyPegen_is_memoized(p, NAME, &cached)) {
        திரும்பு cached;
    }
    p->mark = mark + 1;
    PyObject *id = get_cached_identifier(p, t->bytes);
    எனில் (id == வெற்று) {
        திரும்பு வெற்று;
    }
    கோவை_ty result = _PyAST_Name(id, Load, t->lineno, t->col_offset,
                                t->end_lineno, t->end_col_offset, p->arena);
    எனில் (result != வெற்று && Py_Pegen_செருகு_memo(p, mark, NAME, result) < 0) {
        p->error_indicator = 1;
        திரும்பு வெற்று;
    }
    திரும்பு result;
}

கோவை_ty
_PyPegen_name_token(Parser *p)
{
    Token *t = _PyPegen_expect_token(p, NAME);
    திரும்பு _PyPegen_name_from_token(p, t);
}

வெறுமை *
_PyPegen_string_token(Parser *p)
{
    திரும்பு _PyPegen_expect_token(p, STRING);
}

கோவை_ty _PyPegen_soft_keyword_token(Parser *p) {
    Token *t = _PyPegen_expect_token(p, NAME);
    எனில் (t == வெற்று) {
        திரும்பு வெற்று;
    }
    எழுத்து *the_token;
    Py_ssize_t size;
    PyBytes_AsStringAndSize(t->bytes, &the_token, &size);
    க்கு (எழுத்து **keyword = p->soft_keywords; *keyword != வெற்று; keyword++) {
        எனில் (strlen(*keyword) == (size_t)size &&
            strncmp(*keyword, the_token, (size_t)size) == 0) {
            திரும்பு _PyPegen_name_from_token(p, t);
        }
    }
    திரும்பு வெற்று;
}

நிலையான PyObject *
parsenumber_raw(மாறிலி எழுத்து *s)
{
    மாறிலி எழுத்து *end;
    long x;
    இரட்டை dx;
    Py_complex compl;
    முழு imflag;

    ப்ரதிஜ்ஞா(s != வெற்று);
    errno = 0;
    end = s + strlen(s) - 1;
    imflag = *end == 'j' || *end == 'J';
    எனில் (s[0] == '0') {
        x = (long)PyOS_strtoul(s, (எழுத்து **)&end, 0);
        எனில் (x < 0 && errno == 0) {
            திரும்பு PyLong_FromString(s, (எழுத்து **)0, 0);
        }
    }
    இல்லையெனில் {
        x = PyOS_strtol(s, (எழுத்து **)&end, 0);
    }
    எனில் (*end == '\0') {
        எனில் (errno != 0) {
            திரும்பு PyLong_FromString(s, (எழுத்து **)0, 0);
        }
        திரும்பு PyLong_FromLong(x);
    }
    /* XXX Huge floats may silently fail */
    எனில் (imflag) {
        compl.real = 0.;
        compl.imag = PyOS_string_to_double(s, (எழுத்து **)&end, வெற்று);
        எனில் (compl.imag == -1.0 && PyErr_Occurred()) {
            திரும்பு வெற்று;
        }
        திரும்பு PyComplex_FromCComplex(compl);
    }
    dx = PyOS_string_to_double(s, வெற்று, வெற்று);
    எனில் (dx == -1.0 && PyErr_Occurred()) {
        திரும்பு வெற்று;
    }
    திரும்பு PyFloat_FromDouble(dx);
}

நிலையான PyObject *
parsenumber(மாறிலி எழுத்து *s)
{
    எழுத்து *dup;
    எழுத்து *end;
    PyObject *res = வெற்று;

    ப்ரதிஜ்ஞா(s != வெற்று);

    எனில் (strchr(s, '_') == வெற்று) {
        திரும்பு parsenumber_raw(s);
    }
    /* Create a duplicate without underscores. */
    dup = PyMem_Malloc(strlen(s) + 1);
    எனில் (dup == வெற்று) {
        திரும்பு PyErr_NoMemory();
    }
    end = dup;
    க்கு (; *s; s++) {
        எனில் (*s != '_') {
            *end++ = *s;
        }
    }
    *end = '\0';
    res = parsenumber_raw(dup);
    PyMem_Free(dup);
    திரும்பு res;
}

கோவை_ty
Py_Pegen_எண்_token(Parser *p)
{
    முழு mark = p->mark;
    Token *t = _PyPegen_expect_token(p, எண்);
    எனில் (t == வெற்று) {
        திரும்பு வெற்று;
    }

    // Look up the node at the token's start, as for NAME tokens. A memo hit
    // advances the mark; on a miss, restore the position after the token.
    p->mark = mark;
    கோவை_ty cached = வெற்று;
    எனில் (_PyPegen_is_memoized(p, எண், &cached)) {
        திரும்பு cached;
    }
    p->mark = mark + 1;

    மாறிலி எழுத்து *இலக்கம்_raw = PyBytes_AsString(t->bytes);
    எனில் (இலக்கம்_raw == வெற்று) {
        p->error_indicator = 1;
        திரும்பு வெற்று;
    }

    எனில் (p->feature_version < 6 && strchr(இலக்கம்_raw, '_') != வெற்று) {
        p->error_indicator = 1;
        திரும்பு RAISE_SYNTAX_ERROR("Underscores in numeric literals are only supported "
                                  "in Python 3.6 and greater");
    }

    PyObject *c = parsenumber(இலக்கம்_raw);

    எனில் (c == வெற்று) {
        p->error_indicator = 1;
        PyThreadState *tstate = _PyThreadState_GET();
        // The only way a ValueError should happen in _this_ code is via
        // PyLong_FromString hitting a length limit.
        எனில் (tstate->current_exception != வெற்று &&
            Py_TYPE(tstate->current_exception) == (PyTypeObject *)Py_Exc_மதிப்பு_Error
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
        திரும்பு வெற்று;
    }

    எனில் (_PyArena_AddPyObject(p->arena, c) < 0) {
        Py_DECREF(c);
        p->error_indicator = 1;
        திரும்பு வெற்று;
    }

    கோவை_ty result = _PyAST_Constant(c, வெற்று, t->lineno, t->col_offset,
                                    t->end_lineno, t->end_col_offset, p->arena);
    எனில் (result != வெற்று && Py_Pegen_செருகு_memo(p, mark, எண், result) < 0) {
        p->error_indicator = 1;
        திரும்பு வெற்று;
    }
    திரும்பு result;
}



நிலையான முழு
compute_parser_flags(PyCompilerFlags *flags)
{
    முழு parser_flags = 0;
    எனில் (!flags) {
        திரும்பு 0;
    }
    எனில் (flags->cf_flags & PyCF_DONT_IMPLY_DEDENT) {
        parser_flags |= PyPARSE_DONT_IMPLY_DEDENT;
    }
    எனில் (flags->cf_flags & PyCF_IGNORE_COOKIE) {
        parser_flags |= PyPARSE_IGNORE_COOKIE;
    }
    எனில் (flags->cf_flags & CO_FUTURE_BARRY_AS_BDFL) {
        parser_flags |= PyPARSE_BARRY_AS_BDFL;
    }
    எனில் (flags->cf_flags & PyCF_TYPE_COMMENTS) {
        parser_flags |= PyPARSE_TYPE_COMMENTS;
    }
    எனில் (flags->cf_flags & Py_CF_ALLOW_INCOMPLETE_உள்ளீடு) {
        parser_flags |= Py_PARSE_ALLOW_INCOMPLETE_உள்ளீடு;
    }
    திரும்பு parser_flags;
}

// Parser API

Parser *
_PyPegen_Parser_New(கட்டமைப்பு tok_state *tok, முழு start_rule, முழு flags,
                    முழு feature_version, முழு *errcode, மாறிலி எழுத்து* source, PyArena *arena)
{
    Parser *p = PyMem_Malloc(அளவு(Parser));
    எனில் (p == வெற்று) {
        திரும்பு (Parser *) PyErr_NoMemory();
    }
    ப்ரதிஜ்ஞா(tok != வெற்று);
    _PyTokenizer_SetOptions(tok, 0, (flags & PyPARSE_TYPE_COMMENTS) > 0);
    Py_Tokenizer_Allow_Incomplete_உள்ளீடு(tok, flags & Py_PARSE_ALLOW_INCOMPLETE_உள்ளீடு);
    p->tok = tok;
    p->keywords = வெற்று;
    p->n_keyword_lists = -1;
    p->soft_keywords = வெற்று;
    p->tokens = PyMem_Malloc(அளவு(Token *));
    எனில் (!p->tokens) {
        PyMem_Free(p);
        திரும்பு (Parser *) PyErr_NoMemory();
    }
    p->tokens[0] = PyMem_Calloc(1, அளவு(Token));
    எனில் (!p->tokens[0]) {
        PyMem_Free(p->tokens);
        PyMem_Free(p);
        திரும்பு (Parser *) PyErr_NoMemory();
    }
    p->type_ignore_comments = (growable_comment_array){0};

    p->mark = 0;
    p->fill = 0;
    p->size = 1;

    p->errcode = errcode;
    p->arena = arena;
    p->start_rule = start_rule;
    p->parsing_started = 0;
    p->normalize = வெற்று;
    p->error_indicator = 0;

    p->starting_lineno = 0;
    p->starting_col_offset = 0;
    p->flags = flags;
    p->feature_version = feature_version;
    p->known_err_token = வெற்று;
    p->identifier_cache = வெற்று;
    p->tstate = PyThreadState_Get();
    // Stack limits are initialized when the thread state is attached.
    ப்ரதிஜ்ஞா(((_PyThreadStateImpl *)p->tstate)->c_அடுக்கு_hard_limit != 0);
    p->அடுக்கு_soft_limit = ((_PyThreadStateImpl *)p->tstate)->c_அடுக்கு_soft_limit;
#எனில் Py_அடுக்கு_GROWS_DOWN
    p->அடுக்கு_soft_limit += Py_OS_அடுக்கு_MARGIN_BYTES;
#இல்லையெனில்
    p->அடுக்கு_soft_limit -= Py_OS_அடுக்கு_MARGIN_BYTES;
#endif
    p->level = 0;
    p->call_invalid_rules = 0;
    p->உள்ளீடு_exhausted = 0;
    p->last_stmt_location.lineno = 0;
    p->last_stmt_location.col_offset = 0;
    p->last_stmt_location.end_lineno = 0;
    p->last_stmt_location.end_col_offset = 0;
#ifdef Py_DEBUG
    p->debug = _Py_GetConfig()->parser_debug;
#endif
    திரும்பு p;
}

வெறுமை
_PyPegen_Parser_Free(Parser *p)
{
    PyMem_Free(p->identifier_cache);
    Py_XDECREF(p->normalize);
    // Resizes allocate blocks starting at indices 1, 2, 4, and so on.
    PyMem_Free(p->tokens[0]);
    க்கு (முழு i = 1; i < p->size; i *= PEGEN_ARRAY_GROWTH_FACTOR) {
        PyMem_Free(p->tokens[i]);
    }
    PyMem_Free(p->tokens);
    growable_comment_array_deallocate(&p->type_ignore_comments);
    PyMem_Free(p);
}

நிலையான வெறுமை
reset_parser_state_for_error_pass(Parser *p)
{
    p->last_stmt_location.lineno = 0;
    p->last_stmt_location.col_offset = 0;
    p->last_stmt_location.end_lineno = 0;
    p->last_stmt_location.end_col_offset = 0;
    க்கு (முழு i = 0; i < p->fill; i++) {
        p->tokens[i]->memo = வெற்று;
        p->tokens[i]->memo_mask = 0;
    }
    p->mark = 0;
    p->flags &= ~Py_PARSE_ALLOW_INCOMPLETE_உள்ளீடு;
    Py_Tokenizer_Allow_Incomplete_உள்ளீடு(p->tok, 0);
    p->call_invalid_rules = 1;
    _PyTokenizer_StopInteractive(p->tok);
}

நிலையான வெறுமை
_PyPegen_set_syntax_error_metadata(Parser *p) {
    PyObject *exc = PyErr_GetRaisedException();
    எனில் (!exc || !PyObject_TypeCheck(exc, (PyTypeObject *)PyExc_SyntaxError)) {
        PyErr_SetRaisedException(exc);
        திரும்பு;
    }
    மாறிலி எழுத்து *source = _PyTokenizer_RetainedSource(p->tok);
    மாறிலி எழுத்து *encoding = _PyTokenizer_GetInfo(p->tok).encoding;
    PyObject* the_source = வெற்று;
    எனில் (source) {
        எனில் (encoding == வெற்று) {
            the_source = PyUnicode_FromString(source);
        } இல்லையெனில் {
            the_source = PyUnicode_Decode(source, strlen(source), encoding, வெற்று);
        }
    }
    எனில் (!the_source) {
        PyErr_Clear();
        the_source = Py_None;
        Py_INCREF(the_source);
    }
    PyObject* metadata = Py_Build_மதிப்பு(
        "(iiN)",
        p->last_stmt_location.lineno,
        p->last_stmt_location.col_offset,
        the_source // N gives ownership to metadata
    );
    எனில் (!metadata) {
        PyErr_Clear();
        PyErr_SetRaisedException(exc);
        திரும்பு;
    }
    PySyntaxErrorObject *syntax_error = (PySyntaxErrorObject *)exc;

    Py_XDECREF(syntax_error->metadata);
    syntax_error->metadata = metadata;
    PyErr_SetRaisedException(exc);
}

வெறுமை *
_PyPegen_run_parser(Parser *p)
{
    வெறுமை *res = _PyPegen_parse(p);
    ப்ரதிஜ்ஞா(p->level == 0);
    எனில் (res != வெற்று && PyErr_Occurred()) {
        // Discard a result returned with an exception still pending
        // (e.g. a MemoryError from a recovered-from allocation failure).
        திரும்பு வெற்று;
    }
    எனில் (res == வெற்று) {
        எனில் (PyErr_Occurred() &&
            (PyErr_ExceptionMatches(Py_Exc_Incomplete_உள்ளீடு_Error) ||
             !PyErr_ExceptionMatches(PyExc_SyntaxError))) {
            திரும்பு வெற்று;
        }
        எனில் ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_உள்ளீடு) &&
            p->உள்ளீடு_exhausted && !PyErr_Occurred()) {
            திரும்பு Py_Pegen_raise_incomplete_உள்ளீடு(p);
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
        எனில் (PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_set_syntax_error_metadata(p);
        }
       திரும்பு வெற்று;
    }

    எனில் (p->start_rule == Py_single_உள்ளீடு && _PyTokenizer_HasTrailingStatement(p->tok)) {
        திரும்பு RAISE_SYNTAX_ERROR("multiple statements found while compiling a single statement");
    }

    // test_peg_generator defines _Py_TEST_PEGEN to not call PyAST_Validate()
#எனில் defined(Py_DEBUG) && !defined(_Py_TEST_PEGEN)
    எனில் (p->start_rule == Py_single_உள்ளீடு ||
        p->start_rule == Py_file_உள்ளீடு ||
        p->start_rule == Py_eval_உள்ளீடு)
    {
        எனில் (!_PyAST_Validate(res)) {
            திரும்பு வெற்று;
        }
    }
#endif
    திரும்பு res;
}

mod_ty
_PyPegen_run_parser_from_file_pointer(FILE *fp, முழு start_rule, PyObject *filename_ob,
                             மாறிலி எழுத்து *enc, மாறிலி எழுத்து *ps1, மாறிலி எழுத்து *ps2,
                             PyCompilerFlags *flags, முழு *errcode,
                             PyObject **interactive_src, PyArena *arena)
{
    முழு parser_flags = compute_parser_flags(flags);
    கட்டமைப்பு tok_state *tok = _PyTokenizer_FromFile(fp, enc, ps1, ps2);
    எனில் (tok == வெற்று) {
        எனில் (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        இல்லையெனில் {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        திரும்பு வெற்று;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    // From here on we need to clean up even if there's an error
    mod_ty result = வெற்று;

    PyObject *module = PyUnicode_FromString("__main__");
    எனில் (module == வெற்று) {
        செல் error;
    }
    _PyTokenizer_SetContext(tok, filename_ob, module);
    Py_DECREF(module);

    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, PY_MINOR_VERSION,
                                    errcode, வெற்று, arena);
    எனில் (p == வெற்று) {
        செல் error;
    }

    result = _PyPegen_run_parser(p);
    _PyPegen_Parser_Free(p);

    மாறிலி எழுத்து *source = _PyTokenizer_RetainedSource(tok);
    எனில் (source != வெற்று && result && interactive_src != வெற்று) {
        *interactive_src = PyUnicode_FromString(source);
        எனில் (!*interactive_src || _PyArena_AddPyObject(arena, *interactive_src) < 0) {
            Py_XDECREF(*interactive_src);
            result = வெற்று;
            செல் error;
        }
    }

error:
    _PyTokenizer_Free(tok);
    திரும்பு result;
}

mod_ty
_PyPegen_run_parser_from_string(மாறிலி எழுத்து *str, முழு start_rule, PyObject *filename_ob,
                       PyCompilerFlags *flags, PyArena *arena, PyObject *module,
                       PyObject **incomplete)
{
    _PyTokenizer_Newline newline_mode = start_rule == Py_file_உள்ளீடு
        ? _PyTokenizer_IMPLY_NEWLINE : _PyTokenizer_KEEP_NEWLINE;
    முழு parser_flags = compute_parser_flags(flags);
    எனில் (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_உள்ளீடு) {
        newline_mode = _PyTokenizer_APPEND_NEWLINE;
    }

    கட்டமைப்பு tok_state *tok;
    எனில் (flags != வெற்று && flags->cf_flags & PyCF_IGNORE_COOKIE) {
        tok = _PyTokenizer_FromUTF8(str, newline_mode, 0);
    } இல்லையெனில் {
        tok = _PyTokenizer_FromString(str, newline_mode, 0);
    }
    எனில் (tok == வெற்று) {
        எனில் (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        இல்லையெனில் {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        திரும்பு வெற்று;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    _PyTokenizer_SetContext(tok, filename_ob, module);

    // We need to clear up from here on
    mod_ty result = வெற்று;

    முழு feature_version = flags && (flags->cf_flags & PyCF_ONLY_AST) ?
        flags->cf_feature_version : PY_MINOR_VERSION;
    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, feature_version,
                                    வெற்று, str, arena);
    எனில் (p == வெற்று) {
        செல் error;
    }

    result = _PyPegen_run_parser(p);
    எனில் (result != வெற்று && (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_உள்ளீடு)) {
        முழு needs_newline = _PyTokenizer_GetNewlineRequirements(tok);
        எனில் ((needs_newline & _PyTokenizer_NEWLINE_CONTINUATION) ||
            (start_rule != Py_file_உள்ளீடு &&
             (needs_newline & _PyTokenizer_NEWLINE_INDENT)) ||
            (start_rule == Py_single_உள்ளீடு && p->உள்ளீடு_exhausted &&
             (needs_newline & _PyTokenizer_NEWLINE_STATEMENT))) {
            Py_Pegen_raise_incomplete_உள்ளீடு(p);
            எனில் (incomplete != வெற்று && PyErr_ExceptionMatches(Py_Exc_Incomplete_உள்ளீடு_Error)) {
                *incomplete = PyErr_GetRaisedException();
            }
            இல்லையெனில் {
                result = வெற்று;
            }
        }
    }
    _PyPegen_Parser_Free(p);

error:
    _PyTokenizer_Free(tok);
    திரும்பு result;
}
