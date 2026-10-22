#अन्तर्भाव <Python.h>
#अन्तर्भाव "pycore_ast.h"           // _PyAST_Validate(),
#अन्तर्भाव "pycore_pystate.h"       // _PyThreadState_GET()
#अन्तर्भाव "pycore_parser.h"        // _PYPEGEN_NSTATISTICS
#अन्तर्भाव "pycore_pyerrors.h"      // PyExc_IncompleteInputError
#अन्तर्भाव "pycore_runtime.h"       // _PyRuntime
#अन्तर्भाव "pycore_unicodeobject.h" // _PyUnicode_InternImmortal
#अन्तर्भाव <errcode.h>

#अन्तर्भाव "lexer/state.h"
#अन्तर्भाव "tokenizer/tokenizer.h"
#अन्तर्भाव "pegen.h"

#define IDENTIFIER_CACHE_SIZE 2048  // Must be a power of two.
#define IDENTIFIER_CACHE_MAX_PROBES 8
#define PEGEN_ARRAY_GROWTH_FACTOR 2
#define TYPE_IGNORE_INITIAL_CAPACITY 10

संरचना _identifier_cache_entry {
    ध्रुव वर्ण *कुञ्चिका;   // Borrowed from arena-owned token bytes.
    Py_ssize_t len;
    Py_hash_t hash;
    PyObject *मानम्;   // Borrowed from an arena-owned identifier.
};

// Internal parser functions

asdl_stmt_seq*
_PyPegen_interactive_exit(Parser *p)
{
    सचे (p->errcode) {
        *(p->errcode) = E_EOF;
    }
    पटिनिवत्त रिक्त;
}

Py_ssize_t
Py_Pegen_byte_offset_to_character_offset_पङ्क्तिः(PyObject *पङ्क्तिः, Py_ssize_t col_offset, Py_ssize_t end_col_offset)
{
    ध्रुव unsigned वर्ण *data = (ध्रुव unsigned वर्ण*)PyUnicode_AsUTF8(पङ्क्तिः);
    सचे (data == रिक्त) {
        पटिनिवत्त -1;
    }

    Py_ssize_t len = 0;
    याव (col_offset < end_col_offset) {
        Py_UCS4 ch = data[col_offset];
        सचे (ch < 0x80) {
            col_offset += 1;
        } अञ्ञथा सचे ((ch & 0xe0) == 0xc0) {
            col_offset += 2;
        } अञ्ञथा सचे ((ch & 0xf0) == 0xe0) {
            col_offset += 3;
        } अञ्ञथा सचे ((ch & 0xf8) == 0xf0) {
            col_offset += 4;
        } अञ्ञथा {
            PyErr_SetString(Py_Exc_मानम्_Error, "Invalid UTF-8 sequence");
            पटिनिवत्त -1;
        }
        len++;
    }
    पटिनिवत्त len;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset_raw(ध्रुव वर्ण* str, Py_ssize_t col_offset)
{
    Py_ssize_t len = (Py_ssize_t)strlen(str);
    सचे (col_offset > len + 1) {
        col_offset = len + 1;
    }
    प्रतिज्ञा(col_offset >= 0);
    PyObject *text = PyUnicode_DecodeUTF8(str, col_offset, "replace");
    सचे (!text) {
        पटिनिवत्त -1;
    }
    Py_ssize_t size = PyUnicode_GET_LENGTH(text);
    Py_DECREF(text);
    पटिनिवत्त size;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset(PyObject *पङ्क्तिः, Py_ssize_t col_offset)
{
    ध्रुव वर्ण *str = PyUnicode_AsUTF8(पङ्क्तिः);
    सचे (!str) {
        पटिनिवत्त -1;
    }
    पटिनिवत्त _PyPegen_byte_offset_to_character_offset_raw(str, col_offset);
}

// Here, mark is the start of the node, while p->mark is the end.
// If node==NULL, they should be the same.
पूर्णाङ्क
Py_Pegen_निवेशय_memo(Parser *p, पूर्णाङ्क mark, पूर्णाङ्क type, शून्य *ग्रन्थिः)
{
    // Insert in front
    Memo *m = _PyArena_Malloc(p->arena, परिमाण(Memo));
    सचे (m == रिक्त) {
        पटिनिवत्त -1;
    }
    m->type = type;
    m->ग्रन्थिः = ग्रन्थिः;
    m->mark = p->mark;
    m->next = p->tokens[mark]->memo;
    p->tokens[mark]->memo = m;
    p->tokens[mark]->memo_mask |= 1ULL << (type & 63);
    पटिनिवत्त 0;
}

// Like _PyPegen_insert_memo(), but returns the inserted Memo so callers
// can update it in place without re-walking the token's memo list.
Memo *
Py_Pegen_निवेशय_memo_direct(Parser *p, पूर्णाङ्क mark, पूर्णाङ्क type)
{
    सचे (Py_Pegen_निवेशय_memo(p, mark, type, रिक्त) < 0) {
        पटिनिवत्त रिक्त;
    }
    पटिनिवत्त p->tokens[mark]->memo;
}

// Like _PyPegen_insert_memo(), but updates an existing node if found.
पूर्णाङ्क
_PyPegen_update_memo(Parser *p, पूर्णाङ्क mark, पूर्णाङ्क type, शून्य *ग्रन्थिः)
{
    अत्थाय (Memo *m = p->tokens[mark]->memo; m != रिक्त; m = m->next) {
        सचे (m->type == type) {
            // Update existing node.
            m->ग्रन्थिः = ग्रन्थिः;
            m->mark = p->mark;
            पटिनिवत्त 0;
        }
    }
    // Insert new node.
    पटिनिवत्त Py_Pegen_निवेशय_memo(p, mark, type, ग्रन्थिः);
}

स्थिर पूर्णाङ्क
init_normalization(Parser *p)
{
    सचे (p->normalize) {
        पटिनिवत्त 1;
    }
    p->normalize = PyImport_ImportModuleAttrString("unicodedata", "normalize");
    सचे (!p->normalize)
    {
        पटिनिवत्त 0;
    }
    पटिनिवत्त 1;
}

पूर्णाङ्क
_PyPegen_grow_loop_buffer(शून्य ***buffer, Py_ssize_t *capacity)
{
    सचे ((size_t)*capacity > (size_t)PY_SSIZE_T_MAX /
            (PEGEN_ARRAY_GROWTH_FACTOR * परिमाण(**buffer))) {
        पटिनिवत्त -1;
    }
    Py_ssize_t new_capacity = *capacity == 0
        ? 1 : *capacity * PEGEN_ARRAY_GROWTH_FACTOR;
    शून्य **new_buffer = PyMem_Realloc(*buffer, new_capacity * परिमाण(**buffer));
    सचे (new_buffer == रिक्त) {
        पटिनिवत्त -1;
    }
    *buffer = new_buffer;
    *capacity = new_capacity;
    पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क
growable_comment_array_add(growable_comment_array *arr, पूर्णाङ्क lineno, वर्ण *comment)
{
    सचे (arr->अङ्कः_वस्तूनि >= arr->size) {
        सचे (arr->size > (size_t)PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * परिमाण(*arr->वस्तूनि))) {
            पटिनिवत्त 0;
        }
        size_t new_size = arr->size == 0
            ? TYPE_IGNORE_INITIAL_CAPACITY
            : arr->size * PEGEN_ARRAY_GROWTH_FACTOR;
        शून्य *new_वस्तूनि_array = PyMem_Realloc(
            arr->वस्तूनि, new_size * परिमाण(*arr->वस्तूनि));
        सचे (!new_वस्तूनि_array) {
            पटिनिवत्त 0;
        }
        arr->वस्तूनि = new_वस्तूनि_array;
        arr->size = new_size;
    }

    arr->वस्तूनि[arr->अङ्कः_वस्तूनि].lineno = lineno;
    arr->वस्तूनि[arr->अङ्कः_वस्तूनि].comment = comment;  // Take ownership
    arr->अङ्कः_वस्तूनि++;
    पटिनिवत्त 1;
}

स्थिर शून्य
growable_comment_array_deallocate(growable_comment_array *arr)
{
    अत्थाय (size_t i = 0; i < arr->अङ्कः_वस्तूनि; i++) {
        PyMem_Free(arr->वस्तूनि[i].comment);
    }
    PyMem_Free(arr->वस्तूनि);
}

स्थिर पूर्णाङ्क
_get_keyword_or_name_type(Parser *p, ध्रुव वर्ण *text, Py_ssize_t length)
{
    प्रतिज्ञा(length > 0);

    सचे (length >= p->n_keyword_lists ||
        p->keywords[length] == रिक्त ||
        p->keywords[length]->type == -1) {
        पटिनिवत्त NAME;
    }
    अत्थाय (KeywordToken *k = p->keywords[length]; k != रिक्त && k->type != -1; k++) {
        सचे (memcmp(k->str, text, (size_t)length) == 0) {
            पटिनिवत्त k->type;
        }
    }
    पटिनिवत्त NAME;
}

// Keep text only where grammar actions or helpers read it. Keyword error
// actions use fixed spellings, so keywords do not need their text.
स्थिर अन्तःस्थ पूर्णाङ्क
token_needs_text(पूर्णाङ्क type)
{
    विकल्प (type) {
        पक्ष NAME:
        पक्ष सङ्ख्या:
        पक्ष STRING:
        पक्ष FSTRING_START:
        पक्ष FSTRING_मध्यः:
        पक्ष FSTRING_END:
        पक्ष TSTRING_START:
        पक्ष TSTRING_मध्यः:
        पक्ष TSTRING_END:
        पक्ष TYPE_COMMENT:
        पक्ष NOTEQUAL:  // _PyPegen_check_barry_as_flufl() distinguishes != and <>.
            पटिनिवत्त 1;
        स्वभाव:
            पटिनिवत्त 0;
    }
}

स्थिर पूर्णाङ्क
initialize_token(Parser *p, Token *parser_token, संरचना token *new_token, पूर्णाङ्क token_type) {
    प्रतिज्ञा(parser_token != रिक्त);

    parser_token->type = token_type;
    parser_token->bytes = रिक्त;
    सचे (token_needs_text(token_type) && new_token->span.start >= 0) {
        Py_ssize_t length;
        ध्रुव वर्ण *text = _PyToken_TextView(p->tok, new_token, &length);
        सचे (token_type == NAME) {
            parser_token->type = _get_keyword_or_name_type(p, text, length);
        }
        // A name reclassified as a keyword does not need its text.
        सचे (parser_token->type == token_type) {
            parser_token->bytes = PyBytes_FromStringAndSize(text, length);
            सचे (parser_token->bytes == रिक्त) {
                पटिनिवत्त -1;
            }
            सचे (_PyArena_AddPyObject(p->arena, parser_token->bytes) < 0) {
                Py_DECREF(parser_token->bytes);
                पटिनिवत्त -1;
            }
        }
    }

    parser_token->metadata = रिक्त;
    सचे (new_token->metadata != रिक्त) {
        सचे (_PyArena_AddPyObject(p->arena, new_token->metadata) < 0) {
            पटिनिवत्त -1;
        }
        parser_token->metadata = new_token->metadata;
        new_token->metadata = रिक्त;
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

    सचे (token_type == ERRORTOKEN && _PyTokenizer_GetStatus(p->tok) == E_DECODE) {
        पटिनिवत्त _Pypegen_raise_decode_error(p);
    }

    पटिनिवत्त (token_type == ERRORTOKEN ? _Pypegen_tokenizer_error(p) : 0);
}

स्थिर पूर्णाङ्क
_resize_tokens_array(Parser *p) {
    सचे (p->size > INT_MAX / PEGEN_ARRAY_GROWTH_FACTOR ||
            (size_t)p->size > PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * परिमाण(*p->tokens))) {
        PyErr_NoMemory();
        पटिनिवत्त -1;
    }
    पूर्णाङ्क newsize = p->size * PEGEN_ARRAY_GROWTH_FACTOR;
    Token **new_tokens = PyMem_Realloc(p->tokens, (size_t)newsize * परिमाण(Token *));
    सचे (new_tokens == रिक्त) {
        PyErr_NoMemory();
        पटिनिवत्त -1;
    }
    p->tokens = new_tokens;

    // Allocate new tokens together without moving existing ones: parser rules
    // may still hold pointers into earlier blocks. Parser_Free frees each block.
    Token *chunk = PyMem_Calloc((size_t)(newsize - p->size), परिमाण(Token));
    सचे (chunk == रिक्त) {
        PyErr_NoMemory();
        पटिनिवत्त -1;
    }
    अत्थाय (पूर्णाङ्क i = p->size; i < newsize; i++) {
        p->tokens[i] = &chunk[i - p->size];
    }
    p->size = newsize;
    पटिनिवत्त 0;
}

पूर्णाङ्क
_PyPegen_fill_token(Parser *p)
{
    संरचना token new_token;
    _PyToken_Init(&new_token);
    _PyTokenizer_Get(p->tok, &new_token);

    // Record and skip '# type: ignore' comments
    याव (new_token.type == TYPE_IGNORE) {
        Py_ssize_t len;
        ध्रुव वर्ण *text = _PyToken_TextView(p->tok, &new_token, &len);
        वर्ण *tag = PyMem_Malloc((size_t)len + 1);
        सचे (tag == रिक्त) {
            PyErr_NoMemory();
            गच्छ error;
        }
        memcpy(tag, text, (size_t)len);
        tag[len] = '\0';
        // Ownership of tag passes to the growable array
        सचे (!growable_comment_array_add(&p->type_ignore_comments, new_token.end_loc.lineno, tag)) {
            PyMem_Free(tag);
            PyErr_NoMemory();
            गच्छ error;
        }
        _PyTokenizer_Get(p->tok, &new_token);
    }

    पूर्णाङ्क type = new_token.type;
    // EOF can finish an ordinary token. Only a boundary token means that
    // the grammar requested input beyond the last physical line.
    सचे (type == ENDMARKER ||
        (type == DEDENT && (p->flags & Py_PARSE_ALLOW_INCOMPLETE_निवेशः) &&
         _PyTokenizer_GetStatus(p->tok) == E_EOF)) {
        p->निवेशः_exhausted = 1;
    }

    // If we have reached the end and we are in single input mode we need to insert a newline and reset the parsing
    सचे (p->start_rule == Py_single_निवेशः && type == ENDMARKER && p->parsing_started) {
        type = NEWLINE; /* Add an extra newline */
        p->parsing_started = 0;

        सचे (!(p->flags & PyPARSE_DONT_IMPLY_DEDENT)) {
            _PyTokenizer_ImplyDedents(p->tok);
        }
    }
    अञ्ञथा {
        p->parsing_started = 1;
    }

    // Check if we are at the limit of the token array capacity and resize if needed
    सचे ((p->fill == p->size) && (_resize_tokens_array(p) != 0)) {
        गच्छ error;
    }

    Token *t = p->tokens[p->fill];
    पूर्णाङ्क result = initialize_token(p, t, &new_token, type);
    _PyToken_Free(&new_token);
    पटिनिवत्त result;
error:
    _PyToken_Free(&new_token);
    पटिनिवत्त -1;
}

#सचे defined(Py_DEBUG)
// Instrumentation to count the effectiveness of memoization.
// The array counts the number of tokens skipped by memoization,
// indexed by type.

#define NSTATISTICS _PYPEGEN_NSTATISTICS
#define memo_statistics _PyRuntime.parser.memo_statistics

शून्य
_PyPegen_clear_memo_statistics(शून्य)
{
    PyMutex_Lock(&_PyRuntime.parser.mutex);
    अत्थाय (पूर्णाङ्क i = 0; i < NSTATISTICS; i++) {
        memo_statistics[i] = 0;
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
}

PyObject *
_PyPegen_get_memo_statistics(शून्य)
{
    PyObject *ret = PyList_New(NSTATISTICS);
    सचे (ret == रिक्त) {
        पटिनिवत्त रिक्त;
    }

    PyMutex_Lock(&_PyRuntime.parser.mutex);
    अत्थाय (पूर्णाङ्क i = 0; i < NSTATISTICS; i++) {
        PyObject *मानम् = PyLong_FromLong(memo_statistics[i]);
        सचे (मानम् == रिक्त) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            पटिनिवत्त रिक्त;
        }
        // PyList_SetItem borrows a reference to value.
        सचे (PyList_SetItem(ret, i, मानम्) < 0) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            पटिनिवत्त रिक्त;
        }
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
    पटिनिवत्त ret;
}
#endif

पूर्णाङ्क  // bool
_PyPegen_is_memoized(Parser *p, पूर्णाङ्क type, शून्य *pres)
{
    सचे (p->mark == p->fill) {
        सचे (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            पटिनिवत्त -1;
        }
    }

    Token *t = p->tokens[p->mark];

    सचे (!(t->memo_mask & (1ULL << (type & 63)))) {
        पटिनिवत्त 0;
    }

    अत्थाय (Memo *m = t->memo; m != रिक्त; m = m->next) {
        सचे (m->type == type) {
#सचे defined(Py_DEBUG)
            सचे (0 <= type && type < NSTATISTICS) {
                long गणनम् = m->mark - p->mark;
                // A memoized negative result counts for one.
                सचे (गणनम् <= 0) {
                    गणनम् = 1;
                }
                PyMutex_Lock(&_PyRuntime.parser.mutex);
                memo_statistics[type] += गणनम्;
                PyMutex_Unlock(&_PyRuntime.parser.mutex);
            }
#endif
            p->mark = m->mark;
            *(शून्य **)(pres) = m->ग्रन्थिः;
            पटिनिवत्त 1;
        }
    }
    पटिनिवत्त 0;
}

#define LOOKAHEAD1(NAME, RES_TYPE)                                  \
    पूर्णाङ्क                                                             \
    NAME (पूर्णाङ्क positive, RES_TYPE (func)(Parser *), Parser *p)       \
    {                                                               \
        पूर्णाङ्क mark = p->mark;                                         \
        शून्य *res = func(p);                                        \
        p->mark = mark;                                             \
        पटिनिवत्त (res != रिक्त) == positive;                           \
    }

LOOKAHEAD1(_PyPegen_lookahead, शून्य *)
LOOKAHEAD1(Py_Pegen_lookahead_for_व्यञ्जकम्, व्यञ्जकम्_ty)
LOOKAHEAD1(_PyPegen_lookahead_for_stmt, stmt_ty)
#undef LOOKAHEAD1

#define LOOKAHEAD2(NAME, RES_TYPE, T)                                   \
    पूर्णाङ्क                                                                 \
    NAME (पूर्णाङ्क positive, RES_TYPE (func)(Parser *, T), Parser *p, T arg) \
    {                                                                   \
        पूर्णाङ्क mark = p->mark;                                             \
        शून्य *res = func(p, arg);                                       \
        p->mark = mark;                                                 \
        पटिनिवत्त (res != रिक्त) == positive;                               \
    }

LOOKAHEAD2(_PyPegen_lookahead_with_int, Token *, पूर्णाङ्क)
LOOKAHEAD2(_PyPegen_lookahead_with_string, व्यञ्जकम्_ty, ध्रुव वर्ण *)
#undef LOOKAHEAD2

Token *
_PyPegen_expect_token(Parser *p, पूर्णाङ्क type)
{
    सचे (p->mark == p->fill) {
        सचे (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            पटिनिवत्त रिक्त;
        }
    }
    Token *t = p->tokens[p->mark];
    सचे (t->type != type) {
       पटिनिवत्त रिक्त;
    }
    p->mark += 1;
    पटिनिवत्त t;
}

शून्य*
_PyPegen_expect_forced_result(Parser *p, शून्य* result, ध्रुव वर्ण* expected) {

    सचे (p->error_indicator == 1) {
        पटिनिवत्त रिक्त;
    }
    सचे (result == रिक्त) {
        RAISE_SYNTAX_ERROR("expected (%s)", expected);
        पटिनिवत्त रिक्त;
    }
    पटिनिवत्त result;
}

Token *
_PyPegen_expect_forced_token(Parser *p, पूर्णाङ्क type, ध्रुव वर्ण* expected) {

    सचे (p->error_indicator == 1) {
        पटिनिवत्त रिक्त;
    }

    सचे (p->mark == p->fill) {
        सचे (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            पटिनिवत्त रिक्त;
        }
    }
    Token *t = p->tokens[p->mark];
    सचे (t->type != type) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(t, "expected '%s'", expected);
        पटिनिवत्त रिक्त;
    }
    p->mark += 1;
    पटिनिवत्त t;
}

व्यञ्जकम्_ty
_PyPegen_expect_soft_keyword(Parser *p, ध्रुव वर्ण *keyword)
{
    सचे (p->mark == p->fill) {
        सचे (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            पटिनिवत्त रिक्त;
        }
    }
    Token *t = p->tokens[p->mark];
    सचे (t->type != NAME) {
        पटिनिवत्त रिक्त;
    }
    ध्रुव वर्ण *s = PyBytes_AsString(t->bytes);
    सचे (!s) {
        p->error_indicator = 1;
        पटिनिवत्त रिक्त;
    }
    सचे (strcmp(s, keyword) != 0) {
        पटिनिवत्त रिक्त;
    }
    पटिनिवत्त _PyPegen_name_token(p);
}

Token *
_PyPegen_get_last_nonnwhitespace_token(Parser *p)
{
    प्रतिज्ञा(p->mark >= 0);
    Token *token = रिक्त;
    अत्थाय (पूर्णाङ्क m = p->mark - 1; m >= 0; m--) {
        token = p->tokens[m];
        सचे (token->type != ENDMARKER && (token->type < NEWLINE || token->type > DEDENT)) {
            भङ्ग;
        }
    }
    पटिनिवत्त token;
}

PyObject *
_PyPegen_new_identifier(Parser *p, ध्रुव वर्ण *n)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, (Py_ssize_t)strlen(n), रिक्त);
    सचे (!id) {
        गच्छ error;
    }
    /* Check whether there are non-ASCII characters in the
       identifier; if so, normalize to NFKC. */
    सचे (!PyUnicode_IS_ASCII(id))
    {
        सचे (!init_normalization(p))
        {
            Py_DECREF(id);
            गच्छ error;
        }
        PyObject *form = PyUnicode_InternFromString("NFKC");
        सचे (form == रिक्त)
        {
            Py_DECREF(id);
            गच्छ error;
        }
        PyObject *args[2] = {form, id};
        PyObject *id2 = PyObject_Vectorcall(p->normalize, args, 2, रिक्त);
        Py_DECREF(id);
        Py_DECREF(form);
        सचे (!id2) {
            गच्छ error;
        }

        सचे (!PyUnicode_Check(id2))
        {
            PyErr_Format(PyExc_TypeError,
                         "unicodedata.normalize() must return a string, not "
                         "%.200s",
                         _PyType_Name(Py_TYPE(id2)));
            Py_DECREF(id2);
            गच्छ error;
        }
        id = id2;
    }
    स्थिर ध्रुव वर्ण * ध्रुव forbidden[] = {
        "None",
        "True",
        "False",
        रिक्त
    };
    अत्थाय (पूर्णाङ्क i = 0; forbidden[i] != रिक्त; i++) {
        सचे (_PyUnicode_EqualToASCIIString(id, forbidden[i])) {
            PyErr_Format(Py_Exc_मानम्_Error,
                         "identifier field can't represent '%s' constant",
                         forbidden[i]);
            Py_DECREF(id);
            गच्छ error;
        }
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &id);
    सचे (_PyArena_AddPyObject(p->arena, id) < 0)
    {
        Py_DECREF(id);
        गच्छ error;
    }
    पटिनिवत्त id;

error:
    p->error_indicator = 1;
    पटिनिवत्त रिक्त;
}

// Return an arena-owned identifier; callers borrow the reference.
स्थिर PyObject *
get_cached_identifier(Parser *p, PyObject *bytes)
{
    ध्रुव वर्ण *s = PyBytes_AsString(bytes);
    सचे (!s) {
        p->error_indicator = 1;
        पटिनिवत्त रिक्त;
    }
    // Identifiers repeat constantly; a small span-keyed cache skips the
    // UTF-8 decode + intern for repeated occurrences. Keys point into
    // arena-owned token bytes and values are arena-owned interned strings,
    // so borrowed references are valid for the lifetime of the parse
    // (including the second error pass, which reuses parser and arena).
    Py_ssize_t len = PyBytes_GET_SIZE(bytes);
    Py_hash_t hash = PyObject_Hash(bytes);
    सचे (hash == -1) {
        p->error_indicator = 1;
        पटिनिवत्त रिक्त;
    }
    // Parses without identifiers do not need this cache.
    सचे (p->identifier_cache == रिक्त) {
        p->identifier_cache = PyMem_Calloc(
            IDENTIFIER_CACHE_SIZE, परिमाण(*p->identifier_cache));
        सचे (p->identifier_cache == रिक्त) {
            p->error_indicator = 1;
            पटिनिवत्त PyErr_NoMemory();
        }
    }
    IdentifierCacheEntry *free_slot = रिक्त;
    size_t idx = (size_t)hash & (IDENTIFIER_CACHE_SIZE - 1);
    अत्थाय (पूर्णाङ्क probe = 0; probe < IDENTIFIER_CACHE_MAX_PROBES; probe++) {
        IdentifierCacheEntry *entry = &p->identifier_cache[
            (idx + probe) & (IDENTIFIER_CACHE_SIZE - 1)];
        सचे (entry->कुञ्चिका == रिक्त) {
            free_slot = entry;
            भङ्ग;
        }
        सचे (entry->hash == hash && entry->len == len &&
            memcmp(entry->कुञ्चिका, s, len) == 0)
        {
            पटिनिवत्त entry->मानम्;
        }
    }
    PyObject *id = _PyPegen_new_identifier(p, s);
    सचे (id == रिक्त) {
        p->error_indicator = 1;
        पटिनिवत्त रिक्त;
    }
    सचे (free_slot != रिक्त) {
        free_slot->कुञ्चिका = s;
        free_slot->len = len;
        free_slot->hash = hash;
        free_slot->मानम् = id;
    }
    पटिनिवत्त id;
}

स्थिर व्यञ्जकम्_ty
_PyPegen_name_from_token(Parser *p, Token* t)
{
    सचे (t == रिक्त) {
        पटिनिवत्त रिक्त;
    }
    // Reuse the AST node when backtracking revisits this token. Memo lookup
    // starts before the token and restores the position after it on a hit.
    // Token kinds can be memo keys: generated grammar rule IDs start at 1000.
    पूर्णाङ्क mark = p->mark - 1;
    p->mark = mark;
    व्यञ्जकम्_ty cached = रिक्त;
    सचे (_PyPegen_is_memoized(p, NAME, &cached)) {
        पटिनिवत्त cached;
    }
    p->mark = mark + 1;
    PyObject *id = get_cached_identifier(p, t->bytes);
    सचे (id == रिक्त) {
        पटिनिवत्त रिक्त;
    }
    व्यञ्जकम्_ty result = _PyAST_Name(id, Load, t->lineno, t->col_offset,
                                t->end_lineno, t->end_col_offset, p->arena);
    सचे (result != रिक्त && Py_Pegen_निवेशय_memo(p, mark, NAME, result) < 0) {
        p->error_indicator = 1;
        पटिनिवत्त रिक्त;
    }
    पटिनिवत्त result;
}

व्यञ्जकम्_ty
_PyPegen_name_token(Parser *p)
{
    Token *t = _PyPegen_expect_token(p, NAME);
    पटिनिवत्त _PyPegen_name_from_token(p, t);
}

शून्य *
_PyPegen_string_token(Parser *p)
{
    पटिनिवत्त _PyPegen_expect_token(p, STRING);
}

व्यञ्जकम्_ty _PyPegen_soft_keyword_token(Parser *p) {
    Token *t = _PyPegen_expect_token(p, NAME);
    सचे (t == रिक्त) {
        पटिनिवत्त रिक्त;
    }
    वर्ण *the_token;
    Py_ssize_t size;
    PyBytes_AsStringAndSize(t->bytes, &the_token, &size);
    अत्थाय (वर्ण **keyword = p->soft_keywords; *keyword != रिक्त; keyword++) {
        सचे (strlen(*keyword) == (size_t)size &&
            strncmp(*keyword, the_token, (size_t)size) == 0) {
            पटिनिवत्त _PyPegen_name_from_token(p, t);
        }
    }
    पटिनिवत्त रिक्त;
}

स्थिर PyObject *
parsenumber_raw(ध्रुव वर्ण *s)
{
    ध्रुव वर्ण *end;
    long x;
    द्विगुण dx;
    Py_complex compl;
    पूर्णाङ्क imflag;

    प्रतिज्ञा(s != रिक्त);
    errno = 0;
    end = s + strlen(s) - 1;
    imflag = *end == 'j' || *end == 'J';
    सचे (s[0] == '0') {
        x = (long)PyOS_strtoul(s, (वर्ण **)&end, 0);
        सचे (x < 0 && errno == 0) {
            पटिनिवत्त PyLong_FromString(s, (वर्ण **)0, 0);
        }
    }
    अञ्ञथा {
        x = PyOS_strtol(s, (वर्ण **)&end, 0);
    }
    सचे (*end == '\0') {
        सचे (errno != 0) {
            पटिनिवत्त PyLong_FromString(s, (वर्ण **)0, 0);
        }
        पटिनिवत्त PyLong_FromLong(x);
    }
    /* XXX Huge floats may silently fail */
    सचे (imflag) {
        compl.real = 0.;
        compl.imag = PyOS_string_to_double(s, (वर्ण **)&end, रिक्त);
        सचे (compl.imag == -1.0 && PyErr_Occurred()) {
            पटिनिवत्त रिक्त;
        }
        पटिनिवत्त PyComplex_FromCComplex(compl);
    }
    dx = PyOS_string_to_double(s, रिक्त, रिक्त);
    सचे (dx == -1.0 && PyErr_Occurred()) {
        पटिनिवत्त रिक्त;
    }
    पटिनिवत्त PyFloat_FromDouble(dx);
}

स्थिर PyObject *
parsenumber(ध्रुव वर्ण *s)
{
    वर्ण *dup;
    वर्ण *end;
    PyObject *res = रिक्त;

    प्रतिज्ञा(s != रिक्त);

    सचे (strchr(s, '_') == रिक्त) {
        पटिनिवत्त parsenumber_raw(s);
    }
    /* Create a duplicate without underscores. */
    dup = PyMem_Malloc(strlen(s) + 1);
    सचे (dup == रिक्त) {
        पटिनिवत्त PyErr_NoMemory();
    }
    end = dup;
    अत्थाय (; *s; s++) {
        सचे (*s != '_') {
            *end++ = *s;
        }
    }
    *end = '\0';
    res = parsenumber_raw(dup);
    PyMem_Free(dup);
    पटिनिवत्त res;
}

व्यञ्जकम्_ty
Py_Pegen_सङ्ख्या_token(Parser *p)
{
    पूर्णाङ्क mark = p->mark;
    Token *t = _PyPegen_expect_token(p, सङ्ख्या);
    सचे (t == रिक्त) {
        पटिनिवत्त रिक्त;
    }

    // Look up the node at the token's start, as for NAME tokens. A memo hit
    // advances the mark; on a miss, restore the position after the token.
    p->mark = mark;
    व्यञ्जकम्_ty cached = रिक्त;
    सचे (_PyPegen_is_memoized(p, सङ्ख्या, &cached)) {
        पटिनिवत्त cached;
    }
    p->mark = mark + 1;

    ध्रुव वर्ण *अङ्कः_raw = PyBytes_AsString(t->bytes);
    सचे (अङ्कः_raw == रिक्त) {
        p->error_indicator = 1;
        पटिनिवत्त रिक्त;
    }

    सचे (p->feature_version < 6 && strchr(अङ्कः_raw, '_') != रिक्त) {
        p->error_indicator = 1;
        पटिनिवत्त RAISE_SYNTAX_ERROR("Underscores in numeric literals are only supported "
                                  "in Python 3.6 and greater");
    }

    PyObject *c = parsenumber(अङ्कः_raw);

    सचे (c == रिक्त) {
        p->error_indicator = 1;
        PyThreadState *tstate = _PyThreadState_GET();
        // The only way a ValueError should happen in _this_ code is via
        // PyLong_FromString hitting a length limit.
        सचे (tstate->current_exception != रिक्त &&
            Py_TYPE(tstate->current_exception) == (PyTypeObject *)Py_Exc_मानम्_Error
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
        पटिनिवत्त रिक्त;
    }

    सचे (_PyArena_AddPyObject(p->arena, c) < 0) {
        Py_DECREF(c);
        p->error_indicator = 1;
        पटिनिवत्त रिक्त;
    }

    व्यञ्जकम्_ty result = _PyAST_Constant(c, रिक्त, t->lineno, t->col_offset,
                                    t->end_lineno, t->end_col_offset, p->arena);
    सचे (result != रिक्त && Py_Pegen_निवेशय_memo(p, mark, सङ्ख्या, result) < 0) {
        p->error_indicator = 1;
        पटिनिवत्त रिक्त;
    }
    पटिनिवत्त result;
}



स्थिर पूर्णाङ्क
compute_parser_flags(PyCompilerFlags *flags)
{
    पूर्णाङ्क parser_flags = 0;
    सचे (!flags) {
        पटिनिवत्त 0;
    }
    सचे (flags->cf_flags & PyCF_DONT_IMPLY_DEDENT) {
        parser_flags |= PyPARSE_DONT_IMPLY_DEDENT;
    }
    सचे (flags->cf_flags & PyCF_IGNORE_COOKIE) {
        parser_flags |= PyPARSE_IGNORE_COOKIE;
    }
    सचे (flags->cf_flags & CO_FUTURE_BARRY_AS_BDFL) {
        parser_flags |= PyPARSE_BARRY_AS_BDFL;
    }
    सचे (flags->cf_flags & PyCF_TYPE_COMMENTS) {
        parser_flags |= PyPARSE_TYPE_COMMENTS;
    }
    सचे (flags->cf_flags & Py_CF_ALLOW_INCOMPLETE_निवेशः) {
        parser_flags |= Py_PARSE_ALLOW_INCOMPLETE_निवेशः;
    }
    पटिनिवत्त parser_flags;
}

// Parser API

Parser *
_PyPegen_Parser_New(संरचना tok_state *tok, पूर्णाङ्क start_rule, पूर्णाङ्क flags,
                    पूर्णाङ्क feature_version, पूर्णाङ्क *errcode, ध्रुव वर्ण* source, PyArena *arena)
{
    Parser *p = PyMem_Malloc(परिमाण(Parser));
    सचे (p == रिक्त) {
        पटिनिवत्त (Parser *) PyErr_NoMemory();
    }
    प्रतिज्ञा(tok != रिक्त);
    _PyTokenizer_SetOptions(tok, 0, (flags & PyPARSE_TYPE_COMMENTS) > 0);
    Py_Tokenizer_Allow_Incomplete_निवेशः(tok, flags & Py_PARSE_ALLOW_INCOMPLETE_निवेशः);
    p->tok = tok;
    p->keywords = रिक्त;
    p->n_keyword_lists = -1;
    p->soft_keywords = रिक्त;
    p->tokens = PyMem_Malloc(परिमाण(Token *));
    सचे (!p->tokens) {
        PyMem_Free(p);
        पटिनिवत्त (Parser *) PyErr_NoMemory();
    }
    p->tokens[0] = PyMem_Calloc(1, परिमाण(Token));
    सचे (!p->tokens[0]) {
        PyMem_Free(p->tokens);
        PyMem_Free(p);
        पटिनिवत्त (Parser *) PyErr_NoMemory();
    }
    p->type_ignore_comments = (growable_comment_array){0};

    p->mark = 0;
    p->fill = 0;
    p->size = 1;

    p->errcode = errcode;
    p->arena = arena;
    p->start_rule = start_rule;
    p->parsing_started = 0;
    p->normalize = रिक्त;
    p->error_indicator = 0;

    p->starting_lineno = 0;
    p->starting_col_offset = 0;
    p->flags = flags;
    p->feature_version = feature_version;
    p->known_err_token = रिक्त;
    p->identifier_cache = रिक्त;
    p->tstate = PyThreadState_Get();
    // Stack limits are initialized when the thread state is attached.
    प्रतिज्ञा(((_PyThreadStateImpl *)p->tstate)->c_राशिः_hard_limit != 0);
    p->राशिः_soft_limit = ((_PyThreadStateImpl *)p->tstate)->c_राशिः_soft_limit;
#सचे Py_राशिः_GROWS_DOWN
    p->राशिः_soft_limit += Py_OS_राशिः_MARGIN_BYTES;
#अञ्ञथा
    p->राशिः_soft_limit -= Py_OS_राशिः_MARGIN_BYTES;
#endif
    p->level = 0;
    p->call_invalid_rules = 0;
    p->निवेशः_exhausted = 0;
    p->last_stmt_location.lineno = 0;
    p->last_stmt_location.col_offset = 0;
    p->last_stmt_location.end_lineno = 0;
    p->last_stmt_location.end_col_offset = 0;
#ifdef Py_DEBUG
    p->debug = _Py_GetConfig()->parser_debug;
#endif
    पटिनिवत्त p;
}

शून्य
_PyPegen_Parser_Free(Parser *p)
{
    PyMem_Free(p->identifier_cache);
    Py_XDECREF(p->normalize);
    // Resizes allocate blocks starting at indices 1, 2, 4, and so on.
    PyMem_Free(p->tokens[0]);
    अत्थाय (पूर्णाङ्क i = 1; i < p->size; i *= PEGEN_ARRAY_GROWTH_FACTOR) {
        PyMem_Free(p->tokens[i]);
    }
    PyMem_Free(p->tokens);
    growable_comment_array_deallocate(&p->type_ignore_comments);
    PyMem_Free(p);
}

स्थिर शून्य
reset_parser_state_for_error_pass(Parser *p)
{
    p->last_stmt_location.lineno = 0;
    p->last_stmt_location.col_offset = 0;
    p->last_stmt_location.end_lineno = 0;
    p->last_stmt_location.end_col_offset = 0;
    अत्थाय (पूर्णाङ्क i = 0; i < p->fill; i++) {
        p->tokens[i]->memo = रिक्त;
        p->tokens[i]->memo_mask = 0;
    }
    p->mark = 0;
    p->flags &= ~Py_PARSE_ALLOW_INCOMPLETE_निवेशः;
    Py_Tokenizer_Allow_Incomplete_निवेशः(p->tok, 0);
    p->call_invalid_rules = 1;
    _PyTokenizer_StopInteractive(p->tok);
}

स्थिर शून्य
_PyPegen_set_syntax_error_metadata(Parser *p) {
    PyObject *exc = PyErr_GetRaisedException();
    सचे (!exc || !PyObject_TypeCheck(exc, (PyTypeObject *)PyExc_SyntaxError)) {
        PyErr_SetRaisedException(exc);
        पटिनिवत्त;
    }
    ध्रुव वर्ण *source = _PyTokenizer_RetainedSource(p->tok);
    ध्रुव वर्ण *encoding = _PyTokenizer_GetInfo(p->tok).encoding;
    PyObject* the_source = रिक्त;
    सचे (source) {
        सचे (encoding == रिक्त) {
            the_source = PyUnicode_FromString(source);
        } अञ्ञथा {
            the_source = PyUnicode_Decode(source, strlen(source), encoding, रिक्त);
        }
    }
    सचे (!the_source) {
        PyErr_Clear();
        the_source = Py_None;
        Py_INCREF(the_source);
    }
    PyObject* metadata = Py_Build_मानम्(
        "(iiN)",
        p->last_stmt_location.lineno,
        p->last_stmt_location.col_offset,
        the_source // N gives ownership to metadata
    );
    सचे (!metadata) {
        PyErr_Clear();
        PyErr_SetRaisedException(exc);
        पटिनिवत्त;
    }
    PySyntaxErrorObject *syntax_error = (PySyntaxErrorObject *)exc;

    Py_XDECREF(syntax_error->metadata);
    syntax_error->metadata = metadata;
    PyErr_SetRaisedException(exc);
}

शून्य *
_PyPegen_run_parser(Parser *p)
{
    शून्य *res = _PyPegen_parse(p);
    प्रतिज्ञा(p->level == 0);
    सचे (res != रिक्त && PyErr_Occurred()) {
        // Discard a result returned with an exception still pending
        // (e.g. a MemoryError from a recovered-from allocation failure).
        पटिनिवत्त रिक्त;
    }
    सचे (res == रिक्त) {
        सचे (PyErr_Occurred() &&
            (PyErr_ExceptionMatches(Py_Exc_Incomplete_निवेशः_Error) ||
             !PyErr_ExceptionMatches(PyExc_SyntaxError))) {
            पटिनिवत्त रिक्त;
        }
        सचे ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_निवेशः) &&
            p->निवेशः_exhausted && !PyErr_Occurred()) {
            पटिनिवत्त Py_Pegen_raise_incomplete_निवेशः(p);
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
        सचे (PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_set_syntax_error_metadata(p);
        }
       पटिनिवत्त रिक्त;
    }

    सचे (p->start_rule == Py_single_निवेशः && _PyTokenizer_HasTrailingStatement(p->tok)) {
        पटिनिवत्त RAISE_SYNTAX_ERROR("multiple statements found while compiling a single statement");
    }

    // test_peg_generator defines _Py_TEST_PEGEN to not call PyAST_Validate()
#सचे defined(Py_DEBUG) && !defined(_Py_TEST_PEGEN)
    सचे (p->start_rule == Py_single_निवेशः ||
        p->start_rule == Py_file_निवेशः ||
        p->start_rule == Py_eval_निवेशः)
    {
        सचे (!_PyAST_Validate(res)) {
            पटिनिवत्त रिक्त;
        }
    }
#endif
    पटिनिवत्त res;
}

mod_ty
_PyPegen_run_parser_from_file_pointer(FILE *fp, पूर्णाङ्क start_rule, PyObject *filename_ob,
                             ध्रुव वर्ण *enc, ध्रुव वर्ण *ps1, ध्रुव वर्ण *ps2,
                             PyCompilerFlags *flags, पूर्णाङ्क *errcode,
                             PyObject **interactive_src, PyArena *arena)
{
    पूर्णाङ्क parser_flags = compute_parser_flags(flags);
    संरचना tok_state *tok = _PyTokenizer_FromFile(fp, enc, ps1, ps2);
    सचे (tok == रिक्त) {
        सचे (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        अञ्ञथा {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        पटिनिवत्त रिक्त;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    // From here on we need to clean up even if there's an error
    mod_ty result = रिक्त;

    PyObject *module = PyUnicode_FromString("__main__");
    सचे (module == रिक्त) {
        गच्छ error;
    }
    _PyTokenizer_SetContext(tok, filename_ob, module);
    Py_DECREF(module);

    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, PY_MINOR_VERSION,
                                    errcode, रिक्त, arena);
    सचे (p == रिक्त) {
        गच्छ error;
    }

    result = _PyPegen_run_parser(p);
    _PyPegen_Parser_Free(p);

    ध्रुव वर्ण *source = _PyTokenizer_RetainedSource(tok);
    सचे (source != रिक्त && result && interactive_src != रिक्त) {
        *interactive_src = PyUnicode_FromString(source);
        सचे (!*interactive_src || _PyArena_AddPyObject(arena, *interactive_src) < 0) {
            Py_XDECREF(*interactive_src);
            result = रिक्त;
            गच्छ error;
        }
    }

error:
    _PyTokenizer_Free(tok);
    पटिनिवत्त result;
}

mod_ty
_PyPegen_run_parser_from_string(ध्रुव वर्ण *str, पूर्णाङ्क start_rule, PyObject *filename_ob,
                       PyCompilerFlags *flags, PyArena *arena, PyObject *module,
                       PyObject **incomplete)
{
    _PyTokenizer_Newline newline_mode = start_rule == Py_file_निवेशः
        ? _PyTokenizer_IMPLY_NEWLINE : _PyTokenizer_KEEP_NEWLINE;
    पूर्णाङ्क parser_flags = compute_parser_flags(flags);
    सचे (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_निवेशः) {
        newline_mode = _PyTokenizer_APPEND_NEWLINE;
    }

    संरचना tok_state *tok;
    सचे (flags != रिक्त && flags->cf_flags & PyCF_IGNORE_COOKIE) {
        tok = _PyTokenizer_FromUTF8(str, newline_mode, 0);
    } अञ्ञथा {
        tok = _PyTokenizer_FromString(str, newline_mode, 0);
    }
    सचे (tok == रिक्त) {
        सचे (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        अञ्ञथा {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        पटिनिवत्त रिक्त;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    _PyTokenizer_SetContext(tok, filename_ob, module);

    // We need to clear up from here on
    mod_ty result = रिक्त;

    पूर्णाङ्क feature_version = flags && (flags->cf_flags & PyCF_ONLY_AST) ?
        flags->cf_feature_version : PY_MINOR_VERSION;
    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, feature_version,
                                    रिक्त, str, arena);
    सचे (p == रिक्त) {
        गच्छ error;
    }

    result = _PyPegen_run_parser(p);
    सचे (result != रिक्त && (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_निवेशः)) {
        पूर्णाङ्क needs_newline = _PyTokenizer_GetNewlineRequirements(tok);
        सचे ((needs_newline & _PyTokenizer_NEWLINE_CONTINUATION) ||
            (start_rule != Py_file_निवेशः &&
             (needs_newline & _PyTokenizer_NEWLINE_INDENT)) ||
            (start_rule == Py_single_निवेशः && p->निवेशः_exhausted &&
             (needs_newline & _PyTokenizer_NEWLINE_STATEMENT))) {
            Py_Pegen_raise_incomplete_निवेशः(p);
            सचे (incomplete != रिक्त && PyErr_ExceptionMatches(Py_Exc_Incomplete_निवेशः_Error)) {
                *incomplete = PyErr_GetRaisedException();
            }
            अञ्ञथा {
                result = रिक्त;
            }
        }
    }
    _PyPegen_Parser_Free(p);

error:
    _PyTokenizer_Free(tok);
    पटिनिवत्त result;
}
