#অন্তর্ভাৱ <Python.h>
#অন্তর্ভাৱ "pycore_ast.h"           // _PyAST_Validate(),
#অন্তর্ভাৱ "pycore_pystate.h"       // _PyThreadState_GET()
#অন্তর্ভাৱ "pycore_parser.h"        // _PYPEGEN_NSTATISTICS
#অন্তর্ভাৱ "pycore_pyerrors.h"      // PyExc_IncompleteInputError
#অন্তর্ভাৱ "pycore_runtime.h"       // _PyRuntime
#অন্তর্ভাৱ "pycore_unicodeobject.h" // _PyUnicode_InternImmortal
#অন্তর্ভাৱ <errcode.h>

#অন্তর্ভাৱ "lexer/state.h"
#অন্তর্ভাৱ "tokenizer/tokenizer.h"
#অন্তর্ভাৱ "pegen.h"

#define IDENTIFIER_CACHE_SIZE 2048  // Must be a power of two.
#define IDENTIFIER_CACHE_MAX_PROBES 8
#define PEGEN_ARRAY_GROWTH_FACTOR 2
#define TYPE_IGNORE_INITIAL_CAPACITY 10

সংরচনা _identifier_cache_entry {
    ধ্রুৱ ৱর্ণ *চাবি;   // Borrowed from arena-owned token bytes.
    Py_ssize_t len;
    Py_hash_t hash;
    PyObject *মান;   // Borrowed from an arena-owned identifier.
};

// Internal parser functions

asdl_stmt_seq*
_PyPegen_interactive_exit(Parser *p)
{
    যদি (p->errcode) {
        *(p->errcode) = E_EOF;
    }
    ঘূৰাই রিক্ত;
}

Py_ssize_t
Py_Pegen_byte_offset_to_character_offset_সাৰি(PyObject *সাৰি, Py_ssize_t col_offset, Py_ssize_t end_col_offset)
{
    ধ্রুৱ unsigned ৱর্ণ *data = (ধ্রুৱ unsigned ৱর্ণ*)PyUnicode_AsUTF8(সাৰি);
    যদি (data == রিক্ত) {
        ঘূৰাই -1;
    }

    Py_ssize_t len = 0;
    যেতিয়ালৈ (col_offset < end_col_offset) {
        Py_UCS4 ch = data[col_offset];
        যদি (ch < 0x80) {
            col_offset += 1;
        } অন্যথা যদি ((ch & 0xe0) == 0xc0) {
            col_offset += 2;
        } অন্যথা যদি ((ch & 0xf0) == 0xe0) {
            col_offset += 3;
        } অন্যথা যদি ((ch & 0xf8) == 0xf0) {
            col_offset += 4;
        } অন্যথা {
            PyErr_SetString(Py_Exc_মান_Error, "Invalid UTF-8 sequence");
            ঘূৰাই -1;
        }
        len++;
    }
    ঘূৰাই len;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset_raw(ধ্রুৱ ৱর্ণ* str, Py_ssize_t col_offset)
{
    Py_ssize_t len = (Py_ssize_t)strlen(str);
    যদি (col_offset > len + 1) {
        col_offset = len + 1;
    }
    প্রতিজ্ঞা(col_offset >= 0);
    PyObject *text = PyUnicode_DecodeUTF8(str, col_offset, "replace");
    যদি (!text) {
        ঘূৰাই -1;
    }
    Py_ssize_t size = PyUnicode_GET_LENGTH(text);
    Py_DECREF(text);
    ঘূৰাই size;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset(PyObject *সাৰি, Py_ssize_t col_offset)
{
    ধ্রুৱ ৱর্ণ *str = PyUnicode_AsUTF8(সাৰি);
    যদি (!str) {
        ঘূৰাই -1;
    }
    ঘূৰাই _PyPegen_byte_offset_to_character_offset_raw(str, col_offset);
}

// Here, mark is the start of the node, while p->mark is the end.
// If node==NULL, they should be the same.
পূর্ণাঙ্ক
Py_Pegen_ঢোকাও_memo(Parser *p, পূর্ণাঙ্ক mark, পূর্ণাঙ্ক type, শূন্য *গ্ৰন্থি)
{
    // Insert in front
    Memo *m = _PyArena_Malloc(p->arena, পরিমাণ(Memo));
    যদি (m == রিক্ত) {
        ঘূৰাই -1;
    }
    m->type = type;
    m->গ্ৰন্থি = গ্ৰন্থি;
    m->mark = p->mark;
    m->next = p->tokens[mark]->memo;
    p->tokens[mark]->memo = m;
    p->tokens[mark]->memo_mask |= 1ULL << (type & 63);
    ঘূৰাই 0;
}

// Like _PyPegen_insert_memo(), but returns the inserted Memo so callers
// can update it in place without re-walking the token's memo list.
Memo *
Py_Pegen_ঢোকাও_memo_direct(Parser *p, পূর্ণাঙ্ক mark, পূর্ণাঙ্ক type)
{
    যদি (Py_Pegen_ঢোকাও_memo(p, mark, type, রিক্ত) < 0) {
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই p->tokens[mark]->memo;
}

// Like _PyPegen_insert_memo(), but updates an existing node if found.
পূর্ণাঙ্ক
_PyPegen_update_memo(Parser *p, পূর্ণাঙ্ক mark, পূর্ণাঙ্ক type, শূন্য *গ্ৰন্থি)
{
    বাবে (Memo *m = p->tokens[mark]->memo; m != রিক্ত; m = m->next) {
        যদি (m->type == type) {
            // Update existing node.
            m->গ্ৰন্থি = গ্ৰন্থি;
            m->mark = p->mark;
            ঘূৰাই 0;
        }
    }
    // Insert new node.
    ঘূৰাই Py_Pegen_ঢোকাও_memo(p, mark, type, গ্ৰন্থি);
}

স্থির পূর্ণাঙ্ক
init_normalization(Parser *p)
{
    যদি (p->normalize) {
        ঘূৰাই 1;
    }
    p->normalize = PyImport_ImportModuleAttrString("unicodedata", "normalize");
    যদি (!p->normalize)
    {
        ঘূৰাই 0;
    }
    ঘূৰাই 1;
}

পূর্ণাঙ্ক
_PyPegen_grow_loop_buffer(শূন্য ***buffer, Py_ssize_t *capacity)
{
    যদি ((size_t)*capacity > (size_t)PY_SSIZE_T_MAX /
            (PEGEN_ARRAY_GROWTH_FACTOR * পরিমাণ(**buffer))) {
        ঘূৰাই -1;
    }
    Py_ssize_t new_capacity = *capacity == 0
        ? 1 : *capacity * PEGEN_ARRAY_GROWTH_FACTOR;
    শূন্য **new_buffer = PyMem_Realloc(*buffer, new_capacity * পরিমাণ(**buffer));
    যদি (new_buffer == রিক্ত) {
        ঘূৰাই -1;
    }
    *buffer = new_buffer;
    *capacity = new_capacity;
    ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক
growable_comment_array_add(growable_comment_array *arr, পূর্ণাঙ্ক lineno, ৱর্ণ *comment)
{
    যদি (arr->অঙ্ক_বস্তুসমূহ >= arr->size) {
        যদি (arr->size > (size_t)PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * পরিমাণ(*arr->বস্তুসমূহ))) {
            ঘূৰাই 0;
        }
        size_t new_size = arr->size == 0
            ? TYPE_IGNORE_INITIAL_CAPACITY
            : arr->size * PEGEN_ARRAY_GROWTH_FACTOR;
        শূন্য *new_বস্তুসমূহ_array = PyMem_Realloc(
            arr->বস্তুসমূহ, new_size * পরিমাণ(*arr->বস্তুসমূহ));
        যদি (!new_বস্তুসমূহ_array) {
            ঘূৰাই 0;
        }
        arr->বস্তুসমূহ = new_বস্তুসমূহ_array;
        arr->size = new_size;
    }

    arr->বস্তুসমূহ[arr->অঙ্ক_বস্তুসমূহ].lineno = lineno;
    arr->বস্তুসমূহ[arr->অঙ্ক_বস্তুসমূহ].comment = comment;  // Take ownership
    arr->অঙ্ক_বস্তুসমূহ++;
    ঘূৰাই 1;
}

স্থির শূন্য
growable_comment_array_deallocate(growable_comment_array *arr)
{
    বাবে (size_t i = 0; i < arr->অঙ্ক_বস্তুসমূহ; i++) {
        PyMem_Free(arr->বস্তুসমূহ[i].comment);
    }
    PyMem_Free(arr->বস্তুসমূহ);
}

স্থির পূর্ণাঙ্ক
_get_keyword_or_name_type(Parser *p, ধ্রুৱ ৱর্ণ *text, Py_ssize_t length)
{
    প্রতিজ্ঞা(length > 0);

    যদি (length >= p->n_keyword_lists ||
        p->keywords[length] == রিক্ত ||
        p->keywords[length]->type == -1) {
        ঘূৰাই NAME;
    }
    বাবে (KeywordToken *k = p->keywords[length]; k != রিক্ত && k->type != -1; k++) {
        যদি (memcmp(k->str, text, (size_t)length) == 0) {
            ঘূৰাই k->type;
        }
    }
    ঘূৰাই NAME;
}

// Keep text only where grammar actions or helpers read it. Keyword error
// actions use fixed spellings, so keywords do not need their text.
স্থির অন্তঃস্থ পূর্ণাঙ্ক
token_needs_text(পূর্ণাঙ্ক type)
{
    ৱিকল্প (type) {
        পক্ষ NAME:
        পক্ষ সংখ্যা:
        পক্ষ STRING:
        পক্ষ FSTRING_START:
        পক্ষ FSTRING_মধ্য:
        পক্ষ FSTRING_END:
        পক্ষ TSTRING_START:
        পক্ষ TSTRING_মধ্য:
        পক্ষ TSTRING_END:
        পক্ষ TYPE_COMMENT:
        পক্ষ NOTEQUAL:  // _PyPegen_check_barry_as_flufl() distinguishes != and <>.
            ঘূৰাই 1;
        স্ৱভাৱ:
            ঘূৰাই 0;
    }
}

স্থির পূর্ণাঙ্ক
initialize_token(Parser *p, Token *parser_token, সংরচনা token *new_token, পূর্ণাঙ্ক token_type) {
    প্রতিজ্ঞা(parser_token != রিক্ত);

    parser_token->type = token_type;
    parser_token->bytes = রিক্ত;
    যদি (token_needs_text(token_type) && new_token->span.start >= 0) {
        Py_ssize_t length;
        ধ্রুৱ ৱর্ণ *text = _PyToken_TextView(p->tok, new_token, &length);
        যদি (token_type == NAME) {
            parser_token->type = _get_keyword_or_name_type(p, text, length);
        }
        // A name reclassified as a keyword does not need its text.
        যদি (parser_token->type == token_type) {
            parser_token->bytes = PyBytes_FromStringAndSize(text, length);
            যদি (parser_token->bytes == রিক্ত) {
                ঘূৰাই -1;
            }
            যদি (_PyArena_AddPyObject(p->arena, parser_token->bytes) < 0) {
                Py_DECREF(parser_token->bytes);
                ঘূৰাই -1;
            }
        }
    }

    parser_token->metadata = রিক্ত;
    যদি (new_token->metadata != রিক্ত) {
        যদি (_PyArena_AddPyObject(p->arena, new_token->metadata) < 0) {
            ঘূৰাই -1;
        }
        parser_token->metadata = new_token->metadata;
        new_token->metadata = রিক্ত;
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

    যদি (token_type == ERRORTOKEN && _PyTokenizer_GetStatus(p->tok) == E_DECODE) {
        ঘূৰাই _Pypegen_raise_decode_error(p);
    }

    ঘূৰাই (token_type == ERRORTOKEN ? _Pypegen_tokenizer_error(p) : 0);
}

স্থির পূর্ণাঙ্ক
_resize_tokens_array(Parser *p) {
    যদি (p->size > INT_MAX / PEGEN_ARRAY_GROWTH_FACTOR ||
            (size_t)p->size > PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * পরিমাণ(*p->tokens))) {
        PyErr_NoMemory();
        ঘূৰাই -1;
    }
    পূর্ণাঙ্ক newsize = p->size * PEGEN_ARRAY_GROWTH_FACTOR;
    Token **new_tokens = PyMem_Realloc(p->tokens, (size_t)newsize * পরিমাণ(Token *));
    যদি (new_tokens == রিক্ত) {
        PyErr_NoMemory();
        ঘূৰাই -1;
    }
    p->tokens = new_tokens;

    // Allocate new tokens together without moving existing ones: parser rules
    // may still hold pointers into earlier blocks. Parser_Free frees each block.
    Token *chunk = PyMem_Calloc((size_t)(newsize - p->size), পরিমাণ(Token));
    যদি (chunk == রিক্ত) {
        PyErr_NoMemory();
        ঘূৰাই -1;
    }
    বাবে (পূর্ণাঙ্ক i = p->size; i < newsize; i++) {
        p->tokens[i] = &chunk[i - p->size];
    }
    p->size = newsize;
    ঘূৰাই 0;
}

পূর্ণাঙ্ক
_PyPegen_fill_token(Parser *p)
{
    সংরচনা token new_token;
    _PyToken_Init(&new_token);
    _PyTokenizer_Get(p->tok, &new_token);

    // Record and skip '# type: ignore' comments
    যেতিয়ালৈ (new_token.type == TYPE_IGNORE) {
        Py_ssize_t len;
        ধ্রুৱ ৱর্ণ *text = _PyToken_TextView(p->tok, &new_token, &len);
        ৱর্ণ *tag = PyMem_Malloc((size_t)len + 1);
        যদি (tag == রিক্ত) {
            PyErr_NoMemory();
            গচ্ছ error;
        }
        memcpy(tag, text, (size_t)len);
        tag[len] = '\0';
        // Ownership of tag passes to the growable array
        যদি (!growable_comment_array_add(&p->type_ignore_comments, new_token.end_loc.lineno, tag)) {
            PyMem_Free(tag);
            PyErr_NoMemory();
            গচ্ছ error;
        }
        _PyTokenizer_Get(p->tok, &new_token);
    }

    পূর্ণাঙ্ক type = new_token.type;
    // EOF can finish an ordinary token. Only a boundary token means that
    // the grammar requested input beyond the last physical line.
    যদি (type == ENDMARKER ||
        (type == DEDENT && (p->flags & Py_PARSE_ALLOW_INCOMPLETE_প্ৰবেশ) &&
         _PyTokenizer_GetStatus(p->tok) == E_EOF)) {
        p->প্ৰবেশ_exhausted = 1;
    }

    // If we have reached the end and we are in single input mode we need to insert a newline and reset the parsing
    যদি (p->start_rule == Py_single_প্ৰবেশ && type == ENDMARKER && p->parsing_started) {
        type = NEWLINE; /* Add an extra newline */
        p->parsing_started = 0;

        যদি (!(p->flags & PyPARSE_DONT_IMPLY_DEDENT)) {
            _PyTokenizer_ImplyDedents(p->tok);
        }
    }
    অন্যথা {
        p->parsing_started = 1;
    }

    // Check if we are at the limit of the token array capacity and resize if needed
    যদি ((p->fill == p->size) && (_resize_tokens_array(p) != 0)) {
        গচ্ছ error;
    }

    Token *t = p->tokens[p->fill];
    পূর্ণাঙ্ক result = initialize_token(p, t, &new_token, type);
    _PyToken_Free(&new_token);
    ঘূৰাই result;
error:
    _PyToken_Free(&new_token);
    ঘূৰাই -1;
}

#যদি defined(Py_DEBUG)
// Instrumentation to count the effectiveness of memoization.
// The array counts the number of tokens skipped by memoization,
// indexed by type.

#define NSTATISTICS _PYPEGEN_NSTATISTICS
#define memo_statistics _PyRuntime.parser.memo_statistics

শূন্য
_PyPegen_clear_memo_statistics(শূন্য)
{
    PyMutex_Lock(&_PyRuntime.parser.mutex);
    বাবে (পূর্ণাঙ্ক i = 0; i < NSTATISTICS; i++) {
        memo_statistics[i] = 0;
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
}

PyObject *
_PyPegen_get_memo_statistics(শূন্য)
{
    PyObject *ret = PyList_New(NSTATISTICS);
    যদি (ret == রিক্ত) {
        ঘূৰাই রিক্ত;
    }

    PyMutex_Lock(&_PyRuntime.parser.mutex);
    বাবে (পূর্ণাঙ্ক i = 0; i < NSTATISTICS; i++) {
        PyObject *মান = PyLong_FromLong(memo_statistics[i]);
        যদি (মান == রিক্ত) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            ঘূৰাই রিক্ত;
        }
        // PyList_SetItem borrows a reference to value.
        যদি (PyList_SetItem(ret, i, মান) < 0) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            ঘূৰাই রিক্ত;
        }
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
    ঘূৰাই ret;
}
#endif

পূর্ণাঙ্ক  // bool
_PyPegen_is_memoized(Parser *p, পূর্ণাঙ্ক type, শূন্য *pres)
{
    যদি (p->mark == p->fill) {
        যদি (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ঘূৰাই -1;
        }
    }

    Token *t = p->tokens[p->mark];

    যদি (!(t->memo_mask & (1ULL << (type & 63)))) {
        ঘূৰাই 0;
    }

    বাবে (Memo *m = t->memo; m != রিক্ত; m = m->next) {
        যদি (m->type == type) {
#যদি defined(Py_DEBUG)
            যদি (0 <= type && type < NSTATISTICS) {
                long গুনতি = m->mark - p->mark;
                // A memoized negative result counts for one.
                যদি (গুনতি <= 0) {
                    গুনতি = 1;
                }
                PyMutex_Lock(&_PyRuntime.parser.mutex);
                memo_statistics[type] += গুনতি;
                PyMutex_Unlock(&_PyRuntime.parser.mutex);
            }
#endif
            p->mark = m->mark;
            *(শূন্য **)(pres) = m->গ্ৰন্থি;
            ঘূৰাই 1;
        }
    }
    ঘূৰাই 0;
}

#define LOOKAHEAD1(NAME, RES_TYPE)                                  \
    পূর্ণাঙ্ক                                                             \
    NAME (পূর্ণাঙ্ক positive, RES_TYPE (func)(Parser *), Parser *p)       \
    {                                                               \
        পূর্ণাঙ্ক mark = p->mark;                                         \
        শূন্য *res = func(p);                                        \
        p->mark = mark;                                             \
        ঘূৰাই (res != রিক্ত) == positive;                           \
    }

LOOKAHEAD1(_PyPegen_lookahead, শূন্য *)
LOOKAHEAD1(Py_Pegen_lookahead_for_ৰাশিমালা, ৰাশিমালা_ty)
LOOKAHEAD1(_PyPegen_lookahead_for_stmt, stmt_ty)
#undef LOOKAHEAD1

#define LOOKAHEAD2(NAME, RES_TYPE, T)                                   \
    পূর্ণাঙ্ক                                                                 \
    NAME (পূর্ণাঙ্ক positive, RES_TYPE (func)(Parser *, T), Parser *p, T arg) \
    {                                                                   \
        পূর্ণাঙ্ক mark = p->mark;                                             \
        শূন্য *res = func(p, arg);                                       \
        p->mark = mark;                                                 \
        ঘূৰাই (res != রিক্ত) == positive;                               \
    }

LOOKAHEAD2(_PyPegen_lookahead_with_int, Token *, পূর্ণাঙ্ক)
LOOKAHEAD2(_PyPegen_lookahead_with_string, ৰাশিমালা_ty, ধ্রুৱ ৱর্ণ *)
#undef LOOKAHEAD2

Token *
_PyPegen_expect_token(Parser *p, পূর্ণাঙ্ক type)
{
    যদি (p->mark == p->fill) {
        যদি (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ঘূৰাই রিক্ত;
        }
    }
    Token *t = p->tokens[p->mark];
    যদি (t->type != type) {
       ঘূৰাই রিক্ত;
    }
    p->mark += 1;
    ঘূৰাই t;
}

শূন্য*
_PyPegen_expect_forced_result(Parser *p, শূন্য* result, ধ্রুৱ ৱর্ণ* expected) {

    যদি (p->error_indicator == 1) {
        ঘূৰাই রিক্ত;
    }
    যদি (result == রিক্ত) {
        RAISE_SYNTAX_ERROR("expected (%s)", expected);
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই result;
}

Token *
_PyPegen_expect_forced_token(Parser *p, পূর্ণাঙ্ক type, ধ্রুৱ ৱর্ণ* expected) {

    যদি (p->error_indicator == 1) {
        ঘূৰাই রিক্ত;
    }

    যদি (p->mark == p->fill) {
        যদি (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ঘূৰাই রিক্ত;
        }
    }
    Token *t = p->tokens[p->mark];
    যদি (t->type != type) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(t, "expected '%s'", expected);
        ঘূৰাই রিক্ত;
    }
    p->mark += 1;
    ঘূৰাই t;
}

ৰাশিমালা_ty
_PyPegen_expect_soft_keyword(Parser *p, ধ্রুৱ ৱর্ণ *keyword)
{
    যদি (p->mark == p->fill) {
        যদি (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ঘূৰাই রিক্ত;
        }
    }
    Token *t = p->tokens[p->mark];
    যদি (t->type != NAME) {
        ঘূৰাই রিক্ত;
    }
    ধ্রুৱ ৱর্ণ *s = PyBytes_AsString(t->bytes);
    যদি (!s) {
        p->error_indicator = 1;
        ঘূৰাই রিক্ত;
    }
    যদি (strcmp(s, keyword) != 0) {
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই _PyPegen_name_token(p);
}

Token *
_PyPegen_get_last_nonnwhitespace_token(Parser *p)
{
    প্রতিজ্ঞা(p->mark >= 0);
    Token *token = রিক্ত;
    বাবে (পূর্ণাঙ্ক m = p->mark - 1; m >= 0; m--) {
        token = p->tokens[m];
        যদি (token->type != ENDMARKER && (token->type < NEWLINE || token->type > DEDENT)) {
            ভঙ্গ;
        }
    }
    ঘূৰাই token;
}

PyObject *
_PyPegen_new_identifier(Parser *p, ধ্রুৱ ৱর্ণ *n)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, (Py_ssize_t)strlen(n), রিক্ত);
    যদি (!id) {
        গচ্ছ error;
    }
    /* Check whether there are non-ASCII characters in the
       identifier; if so, normalize to NFKC. */
    যদি (!PyUnicode_IS_ASCII(id))
    {
        যদি (!init_normalization(p))
        {
            Py_DECREF(id);
            গচ্ছ error;
        }
        PyObject *form = PyUnicode_InternFromString("NFKC");
        যদি (form == রিক্ত)
        {
            Py_DECREF(id);
            গচ্ছ error;
        }
        PyObject *args[2] = {form, id};
        PyObject *id2 = PyObject_Vectorcall(p->normalize, args, 2, রিক্ত);
        Py_DECREF(id);
        Py_DECREF(form);
        যদি (!id2) {
            গচ্ছ error;
        }

        যদি (!PyUnicode_Check(id2))
        {
            PyErr_Format(PyExc_TypeError,
                         "unicodedata.normalize() must return a string, not "
                         "%.200s",
                         _PyType_Name(Py_TYPE(id2)));
            Py_DECREF(id2);
            গচ্ছ error;
        }
        id = id2;
    }
    স্থির ধ্রুৱ ৱর্ণ * ধ্রুৱ forbidden[] = {
        "None",
        "True",
        "False",
        রিক্ত
    };
    বাবে (পূর্ণাঙ্ক i = 0; forbidden[i] != রিক্ত; i++) {
        যদি (_PyUnicode_EqualToASCIIString(id, forbidden[i])) {
            PyErr_Format(Py_Exc_মান_Error,
                         "identifier field can't represent '%s' constant",
                         forbidden[i]);
            Py_DECREF(id);
            গচ্ছ error;
        }
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &id);
    যদি (_PyArena_AddPyObject(p->arena, id) < 0)
    {
        Py_DECREF(id);
        গচ্ছ error;
    }
    ঘূৰাই id;

error:
    p->error_indicator = 1;
    ঘূৰাই রিক্ত;
}

// Return an arena-owned identifier; callers borrow the reference.
স্থির PyObject *
get_cached_identifier(Parser *p, PyObject *bytes)
{
    ধ্রুৱ ৱর্ণ *s = PyBytes_AsString(bytes);
    যদি (!s) {
        p->error_indicator = 1;
        ঘূৰাই রিক্ত;
    }
    // Identifiers repeat constantly; a small span-keyed cache skips the
    // UTF-8 decode + intern for repeated occurrences. Keys point into
    // arena-owned token bytes and values are arena-owned interned strings,
    // so borrowed references are valid for the lifetime of the parse
    // (including the second error pass, which reuses parser and arena).
    Py_ssize_t len = PyBytes_GET_SIZE(bytes);
    Py_hash_t hash = PyObject_Hash(bytes);
    যদি (hash == -1) {
        p->error_indicator = 1;
        ঘূৰাই রিক্ত;
    }
    // Parses without identifiers do not need this cache.
    যদি (p->identifier_cache == রিক্ত) {
        p->identifier_cache = PyMem_Calloc(
            IDENTIFIER_CACHE_SIZE, পরিমাণ(*p->identifier_cache));
        যদি (p->identifier_cache == রিক্ত) {
            p->error_indicator = 1;
            ঘূৰাই PyErr_NoMemory();
        }
    }
    IdentifierCacheEntry *free_slot = রিক্ত;
    size_t idx = (size_t)hash & (IDENTIFIER_CACHE_SIZE - 1);
    বাবে (পূর্ণাঙ্ক probe = 0; probe < IDENTIFIER_CACHE_MAX_PROBES; probe++) {
        IdentifierCacheEntry *entry = &p->identifier_cache[
            (idx + probe) & (IDENTIFIER_CACHE_SIZE - 1)];
        যদি (entry->চাবি == রিক্ত) {
            free_slot = entry;
            ভঙ্গ;
        }
        যদি (entry->hash == hash && entry->len == len &&
            memcmp(entry->চাবি, s, len) == 0)
        {
            ঘূৰাই entry->মান;
        }
    }
    PyObject *id = _PyPegen_new_identifier(p, s);
    যদি (id == রিক্ত) {
        p->error_indicator = 1;
        ঘূৰাই রিক্ত;
    }
    যদি (free_slot != রিক্ত) {
        free_slot->চাবি = s;
        free_slot->len = len;
        free_slot->hash = hash;
        free_slot->মান = id;
    }
    ঘূৰাই id;
}

স্থির ৰাশিমালা_ty
_PyPegen_name_from_token(Parser *p, Token* t)
{
    যদি (t == রিক্ত) {
        ঘূৰাই রিক্ত;
    }
    // Reuse the AST node when backtracking revisits this token. Memo lookup
    // starts before the token and restores the position after it on a hit.
    // Token kinds can be memo keys: generated grammar rule IDs start at 1000.
    পূর্ণাঙ্ক mark = p->mark - 1;
    p->mark = mark;
    ৰাশিমালা_ty cached = রিক্ত;
    যদি (_PyPegen_is_memoized(p, NAME, &cached)) {
        ঘূৰাই cached;
    }
    p->mark = mark + 1;
    PyObject *id = get_cached_identifier(p, t->bytes);
    যদি (id == রিক্ত) {
        ঘূৰাই রিক্ত;
    }
    ৰাশিমালা_ty result = _PyAST_Name(id, Load, t->lineno, t->col_offset,
                                t->end_lineno, t->end_col_offset, p->arena);
    যদি (result != রিক্ত && Py_Pegen_ঢোকাও_memo(p, mark, NAME, result) < 0) {
        p->error_indicator = 1;
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই result;
}

ৰাশিমালা_ty
_PyPegen_name_token(Parser *p)
{
    Token *t = _PyPegen_expect_token(p, NAME);
    ঘূৰাই _PyPegen_name_from_token(p, t);
}

শূন্য *
_PyPegen_string_token(Parser *p)
{
    ঘূৰাই _PyPegen_expect_token(p, STRING);
}

ৰাশিমালা_ty _PyPegen_soft_keyword_token(Parser *p) {
    Token *t = _PyPegen_expect_token(p, NAME);
    যদি (t == রিক্ত) {
        ঘূৰাই রিক্ত;
    }
    ৱর্ণ *the_token;
    Py_ssize_t size;
    PyBytes_AsStringAndSize(t->bytes, &the_token, &size);
    বাবে (ৱর্ণ **keyword = p->soft_keywords; *keyword != রিক্ত; keyword++) {
        যদি (strlen(*keyword) == (size_t)size &&
            strncmp(*keyword, the_token, (size_t)size) == 0) {
            ঘূৰাই _PyPegen_name_from_token(p, t);
        }
    }
    ঘূৰাই রিক্ত;
}

স্থির PyObject *
parsenumber_raw(ধ্রুৱ ৱর্ণ *s)
{
    ধ্রুৱ ৱর্ণ *end;
    long x;
    দ্ৱিগুণ dx;
    Py_complex compl;
    পূর্ণাঙ্ক imflag;

    প্রতিজ্ঞা(s != রিক্ত);
    errno = 0;
    end = s + strlen(s) - 1;
    imflag = *end == 'j' || *end == 'J';
    যদি (s[0] == '0') {
        x = (long)PyOS_strtoul(s, (ৱর্ণ **)&end, 0);
        যদি (x < 0 && errno == 0) {
            ঘূৰাই PyLong_FromString(s, (ৱর্ণ **)0, 0);
        }
    }
    অন্যথা {
        x = PyOS_strtol(s, (ৱর্ণ **)&end, 0);
    }
    যদি (*end == '\0') {
        যদি (errno != 0) {
            ঘূৰাই PyLong_FromString(s, (ৱর্ণ **)0, 0);
        }
        ঘূৰাই PyLong_FromLong(x);
    }
    /* XXX Huge floats may silently fail */
    যদি (imflag) {
        compl.real = 0.;
        compl.imag = PyOS_string_to_double(s, (ৱর্ণ **)&end, রিক্ত);
        যদি (compl.imag == -1.0 && PyErr_Occurred()) {
            ঘূৰাই রিক্ত;
        }
        ঘূৰাই PyComplex_FromCComplex(compl);
    }
    dx = PyOS_string_to_double(s, রিক্ত, রিক্ত);
    যদি (dx == -1.0 && PyErr_Occurred()) {
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই PyFloat_FromDouble(dx);
}

স্থির PyObject *
parsenumber(ধ্রুৱ ৱর্ণ *s)
{
    ৱর্ণ *dup;
    ৱর্ণ *end;
    PyObject *res = রিক্ত;

    প্রতিজ্ঞা(s != রিক্ত);

    যদি (strchr(s, '_') == রিক্ত) {
        ঘূৰাই parsenumber_raw(s);
    }
    /* Create a duplicate without underscores. */
    dup = PyMem_Malloc(strlen(s) + 1);
    যদি (dup == রিক্ত) {
        ঘূৰাই PyErr_NoMemory();
    }
    end = dup;
    বাবে (; *s; s++) {
        যদি (*s != '_') {
            *end++ = *s;
        }
    }
    *end = '\0';
    res = parsenumber_raw(dup);
    PyMem_Free(dup);
    ঘূৰাই res;
}

ৰাশিমালা_ty
Py_Pegen_সংখ্যা_token(Parser *p)
{
    পূর্ণাঙ্ক mark = p->mark;
    Token *t = _PyPegen_expect_token(p, সংখ্যা);
    যদি (t == রিক্ত) {
        ঘূৰাই রিক্ত;
    }

    // Look up the node at the token's start, as for NAME tokens. A memo hit
    // advances the mark; on a miss, restore the position after the token.
    p->mark = mark;
    ৰাশিমালা_ty cached = রিক্ত;
    যদি (_PyPegen_is_memoized(p, সংখ্যা, &cached)) {
        ঘূৰাই cached;
    }
    p->mark = mark + 1;

    ধ্রুৱ ৱর্ণ *অঙ্ক_raw = PyBytes_AsString(t->bytes);
    যদি (অঙ্ক_raw == রিক্ত) {
        p->error_indicator = 1;
        ঘূৰাই রিক্ত;
    }

    যদি (p->feature_version < 6 && strchr(অঙ্ক_raw, '_') != রিক্ত) {
        p->error_indicator = 1;
        ঘূৰাই RAISE_SYNTAX_ERROR("Underscores in numeric literals are only supported "
                                  "in Python 3.6 and greater");
    }

    PyObject *c = parsenumber(অঙ্ক_raw);

    যদি (c == রিক্ত) {
        p->error_indicator = 1;
        PyThreadState *tstate = _PyThreadState_GET();
        // The only way a ValueError should happen in _this_ code is via
        // PyLong_FromString hitting a length limit.
        যদি (tstate->current_exception != রিক্ত &&
            Py_TYPE(tstate->current_exception) == (PyTypeObject *)Py_Exc_মান_Error
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
        ঘূৰাই রিক্ত;
    }

    যদি (_PyArena_AddPyObject(p->arena, c) < 0) {
        Py_DECREF(c);
        p->error_indicator = 1;
        ঘূৰাই রিক্ত;
    }

    ৰাশিমালা_ty result = _PyAST_Constant(c, রিক্ত, t->lineno, t->col_offset,
                                    t->end_lineno, t->end_col_offset, p->arena);
    যদি (result != রিক্ত && Py_Pegen_ঢোকাও_memo(p, mark, সংখ্যা, result) < 0) {
        p->error_indicator = 1;
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই result;
}



স্থির পূর্ণাঙ্ক
compute_parser_flags(PyCompilerFlags *flags)
{
    পূর্ণাঙ্ক parser_flags = 0;
    যদি (!flags) {
        ঘূৰাই 0;
    }
    যদি (flags->cf_flags & PyCF_DONT_IMPLY_DEDENT) {
        parser_flags |= PyPARSE_DONT_IMPLY_DEDENT;
    }
    যদি (flags->cf_flags & PyCF_IGNORE_COOKIE) {
        parser_flags |= PyPARSE_IGNORE_COOKIE;
    }
    যদি (flags->cf_flags & CO_FUTURE_BARRY_AS_BDFL) {
        parser_flags |= PyPARSE_BARRY_AS_BDFL;
    }
    যদি (flags->cf_flags & PyCF_TYPE_COMMENTS) {
        parser_flags |= PyPARSE_TYPE_COMMENTS;
    }
    যদি (flags->cf_flags & Py_CF_ALLOW_INCOMPLETE_প্ৰবেশ) {
        parser_flags |= Py_PARSE_ALLOW_INCOMPLETE_প্ৰবেশ;
    }
    ঘূৰাই parser_flags;
}

// Parser API

Parser *
_PyPegen_Parser_New(সংরচনা tok_state *tok, পূর্ণাঙ্ক start_rule, পূর্ণাঙ্ক flags,
                    পূর্ণাঙ্ক feature_version, পূর্ণাঙ্ক *errcode, ধ্রুৱ ৱর্ণ* source, PyArena *arena)
{
    Parser *p = PyMem_Malloc(পরিমাণ(Parser));
    যদি (p == রিক্ত) {
        ঘূৰাই (Parser *) PyErr_NoMemory();
    }
    প্রতিজ্ঞা(tok != রিক্ত);
    _PyTokenizer_SetOptions(tok, 0, (flags & PyPARSE_TYPE_COMMENTS) > 0);
    Py_Tokenizer_Allow_Incomplete_প্ৰবেশ(tok, flags & Py_PARSE_ALLOW_INCOMPLETE_প্ৰবেশ);
    p->tok = tok;
    p->keywords = রিক্ত;
    p->n_keyword_lists = -1;
    p->soft_keywords = রিক্ত;
    p->tokens = PyMem_Malloc(পরিমাণ(Token *));
    যদি (!p->tokens) {
        PyMem_Free(p);
        ঘূৰাই (Parser *) PyErr_NoMemory();
    }
    p->tokens[0] = PyMem_Calloc(1, পরিমাণ(Token));
    যদি (!p->tokens[0]) {
        PyMem_Free(p->tokens);
        PyMem_Free(p);
        ঘূৰাই (Parser *) PyErr_NoMemory();
    }
    p->type_ignore_comments = (growable_comment_array){0};

    p->mark = 0;
    p->fill = 0;
    p->size = 1;

    p->errcode = errcode;
    p->arena = arena;
    p->start_rule = start_rule;
    p->parsing_started = 0;
    p->normalize = রিক্ত;
    p->error_indicator = 0;

    p->starting_lineno = 0;
    p->starting_col_offset = 0;
    p->flags = flags;
    p->feature_version = feature_version;
    p->known_err_token = রিক্ত;
    p->identifier_cache = রিক্ত;
    p->tstate = PyThreadState_Get();
    // Stack limits are initialized when the thread state is attached.
    প্রতিজ্ঞা(((_PyThreadStateImpl *)p->tstate)->c_স্তূপ_hard_limit != 0);
    p->স্তূপ_soft_limit = ((_PyThreadStateImpl *)p->tstate)->c_স্তূপ_soft_limit;
#যদি Py_স্তূপ_GROWS_DOWN
    p->স্তূপ_soft_limit += Py_OS_স্তূপ_MARGIN_BYTES;
#অন্যথা
    p->স্তূপ_soft_limit -= Py_OS_স্তূপ_MARGIN_BYTES;
#endif
    p->level = 0;
    p->call_invalid_rules = 0;
    p->প্ৰবেশ_exhausted = 0;
    p->last_stmt_location.lineno = 0;
    p->last_stmt_location.col_offset = 0;
    p->last_stmt_location.end_lineno = 0;
    p->last_stmt_location.end_col_offset = 0;
#ifdef Py_DEBUG
    p->debug = _Py_GetConfig()->parser_debug;
#endif
    ঘূৰাই p;
}

শূন্য
_PyPegen_Parser_Free(Parser *p)
{
    PyMem_Free(p->identifier_cache);
    Py_XDECREF(p->normalize);
    // Resizes allocate blocks starting at indices 1, 2, 4, and so on.
    PyMem_Free(p->tokens[0]);
    বাবে (পূর্ণাঙ্ক i = 1; i < p->size; i *= PEGEN_ARRAY_GROWTH_FACTOR) {
        PyMem_Free(p->tokens[i]);
    }
    PyMem_Free(p->tokens);
    growable_comment_array_deallocate(&p->type_ignore_comments);
    PyMem_Free(p);
}

স্থির শূন্য
reset_parser_state_for_error_pass(Parser *p)
{
    p->last_stmt_location.lineno = 0;
    p->last_stmt_location.col_offset = 0;
    p->last_stmt_location.end_lineno = 0;
    p->last_stmt_location.end_col_offset = 0;
    বাবে (পূর্ণাঙ্ক i = 0; i < p->fill; i++) {
        p->tokens[i]->memo = রিক্ত;
        p->tokens[i]->memo_mask = 0;
    }
    p->mark = 0;
    p->flags &= ~Py_PARSE_ALLOW_INCOMPLETE_প্ৰবেশ;
    Py_Tokenizer_Allow_Incomplete_প্ৰবেশ(p->tok, 0);
    p->call_invalid_rules = 1;
    _PyTokenizer_StopInteractive(p->tok);
}

স্থির শূন্য
_PyPegen_set_syntax_error_metadata(Parser *p) {
    PyObject *exc = PyErr_GetRaisedException();
    যদি (!exc || !PyObject_TypeCheck(exc, (PyTypeObject *)PyExc_SyntaxError)) {
        PyErr_SetRaisedException(exc);
        ঘূৰাই;
    }
    ধ্রুৱ ৱর্ণ *source = _PyTokenizer_RetainedSource(p->tok);
    ধ্রুৱ ৱর্ণ *encoding = _PyTokenizer_GetInfo(p->tok).encoding;
    PyObject* the_source = রিক্ত;
    যদি (source) {
        যদি (encoding == রিক্ত) {
            the_source = PyUnicode_FromString(source);
        } অন্যথা {
            the_source = PyUnicode_Decode(source, strlen(source), encoding, রিক্ত);
        }
    }
    যদি (!the_source) {
        PyErr_Clear();
        the_source = Py_None;
        Py_INCREF(the_source);
    }
    PyObject* metadata = Py_Build_মান(
        "(iiN)",
        p->last_stmt_location.lineno,
        p->last_stmt_location.col_offset,
        the_source // N gives ownership to metadata
    );
    যদি (!metadata) {
        PyErr_Clear();
        PyErr_SetRaisedException(exc);
        ঘূৰাই;
    }
    PySyntaxErrorObject *syntax_error = (PySyntaxErrorObject *)exc;

    Py_XDECREF(syntax_error->metadata);
    syntax_error->metadata = metadata;
    PyErr_SetRaisedException(exc);
}

শূন্য *
_PyPegen_run_parser(Parser *p)
{
    শূন্য *res = _PyPegen_parse(p);
    প্রতিজ্ঞা(p->level == 0);
    যদি (res != রিক্ত && PyErr_Occurred()) {
        // Discard a result returned with an exception still pending
        // (e.g. a MemoryError from a recovered-from allocation failure).
        ঘূৰাই রিক্ত;
    }
    যদি (res == রিক্ত) {
        যদি (PyErr_Occurred() &&
            (PyErr_ExceptionMatches(Py_Exc_Incomplete_প্ৰবেশ_Error) ||
             !PyErr_ExceptionMatches(PyExc_SyntaxError))) {
            ঘূৰাই রিক্ত;
        }
        যদি ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_প্ৰবেশ) &&
            p->প্ৰবেশ_exhausted && !PyErr_Occurred()) {
            ঘূৰাই Py_Pegen_raise_incomplete_প্ৰবেশ(p);
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
        যদি (PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_set_syntax_error_metadata(p);
        }
       ঘূৰাই রিক্ত;
    }

    যদি (p->start_rule == Py_single_প্ৰবেশ && _PyTokenizer_HasTrailingStatement(p->tok)) {
        ঘূৰাই RAISE_SYNTAX_ERROR("multiple statements found while compiling a single statement");
    }

    // test_peg_generator defines _Py_TEST_PEGEN to not call PyAST_Validate()
#যদি defined(Py_DEBUG) && !defined(_Py_TEST_PEGEN)
    যদি (p->start_rule == Py_single_প্ৰবেশ ||
        p->start_rule == Py_file_প্ৰবেশ ||
        p->start_rule == Py_eval_প্ৰবেশ)
    {
        যদি (!_PyAST_Validate(res)) {
            ঘূৰাই রিক্ত;
        }
    }
#endif
    ঘূৰাই res;
}

mod_ty
_PyPegen_run_parser_from_file_pointer(FILE *fp, পূর্ণাঙ্ক start_rule, PyObject *filename_ob,
                             ধ্রুৱ ৱর্ণ *enc, ধ্রুৱ ৱর্ণ *ps1, ধ্রুৱ ৱর্ণ *ps2,
                             PyCompilerFlags *flags, পূর্ণাঙ্ক *errcode,
                             PyObject **interactive_src, PyArena *arena)
{
    পূর্ণাঙ্ক parser_flags = compute_parser_flags(flags);
    সংরচনা tok_state *tok = _PyTokenizer_FromFile(fp, enc, ps1, ps2);
    যদি (tok == রিক্ত) {
        যদি (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        অন্যথা {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        ঘূৰাই রিক্ত;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    // From here on we need to clean up even if there's an error
    mod_ty result = রিক্ত;

    PyObject *module = PyUnicode_FromString("__main__");
    যদি (module == রিক্ত) {
        গচ্ছ error;
    }
    _PyTokenizer_SetContext(tok, filename_ob, module);
    Py_DECREF(module);

    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, PY_MINOR_VERSION,
                                    errcode, রিক্ত, arena);
    যদি (p == রিক্ত) {
        গচ্ছ error;
    }

    result = _PyPegen_run_parser(p);
    _PyPegen_Parser_Free(p);

    ধ্রুৱ ৱর্ণ *source = _PyTokenizer_RetainedSource(tok);
    যদি (source != রিক্ত && result && interactive_src != রিক্ত) {
        *interactive_src = PyUnicode_FromString(source);
        যদি (!*interactive_src || _PyArena_AddPyObject(arena, *interactive_src) < 0) {
            Py_XDECREF(*interactive_src);
            result = রিক্ত;
            গচ্ছ error;
        }
    }

error:
    _PyTokenizer_Free(tok);
    ঘূৰাই result;
}

mod_ty
_PyPegen_run_parser_from_string(ধ্রুৱ ৱর্ণ *str, পূর্ণাঙ্ক start_rule, PyObject *filename_ob,
                       PyCompilerFlags *flags, PyArena *arena, PyObject *module,
                       PyObject **incomplete)
{
    _PyTokenizer_Newline newline_mode = start_rule == Py_file_প্ৰবেশ
        ? _PyTokenizer_IMPLY_NEWLINE : _PyTokenizer_KEEP_NEWLINE;
    পূর্ণাঙ্ক parser_flags = compute_parser_flags(flags);
    যদি (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_প্ৰবেশ) {
        newline_mode = _PyTokenizer_APPEND_NEWLINE;
    }

    সংরচনা tok_state *tok;
    যদি (flags != রিক্ত && flags->cf_flags & PyCF_IGNORE_COOKIE) {
        tok = _PyTokenizer_FromUTF8(str, newline_mode, 0);
    } অন্যথা {
        tok = _PyTokenizer_FromString(str, newline_mode, 0);
    }
    যদি (tok == রিক্ত) {
        যদি (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        অন্যথা {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        ঘূৰাই রিক্ত;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    _PyTokenizer_SetContext(tok, filename_ob, module);

    // We need to clear up from here on
    mod_ty result = রিক্ত;

    পূর্ণাঙ্ক feature_version = flags && (flags->cf_flags & PyCF_ONLY_AST) ?
        flags->cf_feature_version : PY_MINOR_VERSION;
    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, feature_version,
                                    রিক্ত, str, arena);
    যদি (p == রিক্ত) {
        গচ্ছ error;
    }

    result = _PyPegen_run_parser(p);
    যদি (result != রিক্ত && (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_প্ৰবেশ)) {
        পূর্ণাঙ্ক needs_newline = _PyTokenizer_GetNewlineRequirements(tok);
        যদি ((needs_newline & _PyTokenizer_NEWLINE_CONTINUATION) ||
            (start_rule != Py_file_প্ৰবেশ &&
             (needs_newline & _PyTokenizer_NEWLINE_INDENT)) ||
            (start_rule == Py_single_প্ৰবেশ && p->প্ৰবেশ_exhausted &&
             (needs_newline & _PyTokenizer_NEWLINE_STATEMENT))) {
            Py_Pegen_raise_incomplete_প্ৰবেশ(p);
            যদি (incomplete != রিক্ত && PyErr_ExceptionMatches(Py_Exc_Incomplete_প্ৰবেশ_Error)) {
                *incomplete = PyErr_GetRaisedException();
            }
            অন্যথা {
                result = রিক্ত;
            }
        }
    }
    _PyPegen_Parser_Free(p);

error:
    _PyTokenizer_Free(tok);
    ঘূৰাই result;
}
