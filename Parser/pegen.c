#অন্তর্ভাব <Python.h>
#অন্তর্ভাব "pycore_ast.h"           // _PyAST_Validate(),
#অন্তর্ভাব "pycore_pystate.h"       // _PyThreadState_GET()
#অন্তর্ভাব "pycore_parser.h"        // _PYPEGEN_NSTATISTICS
#অন্তর্ভাব "pycore_pyerrors.h"      // PyExc_IncompleteInputError
#অন্তর্ভাব "pycore_runtime.h"       // _PyRuntime
#অন্তর্ভাব "pycore_unicodeobject.h" // _PyUnicode_InternImmortal
#অন্তর্ভাব <errcode.h>

#অন্তর্ভাব "lexer/state.h"
#অন্তর্ভাব "tokenizer/tokenizer.h"
#অন্তর্ভাব "pegen.h"

#define IDENTIFIER_CACHE_আকার 2048  // Must be a power of two.
#define IDENTIFIER_CACHE_MAX_PROBES 8
#define PEGEN_ARRAY_GROWTH_FACTOR 2
#define TYPE_IGNORE_INITIAL_CAPACITY 10

কাঠামো _identifier_cache_entry {
    ধ্রুবক অক্ষর *চাবি;   // Borrowed from arena-owned token bytes.
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
    ফেরত ফাঁকা;
}

Py_ssize_t
Py_Pegen_byte_offset_to_character_offset_সারি(PyObject *সারি, Py_ssize_t col_offset, Py_ssize_t end_col_offset)
{
    ধ্রুবক unsigned অক্ষর *তথ্য = (ধ্রুবক unsigned অক্ষর*)PyUnicode_AsUTF8(সারি);
    যদি (তথ্য == ফাঁকা) {
        ফেরত -1;
    }

    Py_ssize_t len = 0;
    যখন (col_offset < end_col_offset) {
        Py_UCS4 ch = তথ্য[col_offset];
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
            ফেরত -1;
        }
        len++;
    }
    ফেরত len;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset_raw(ধ্রুবক অক্ষর* str, Py_ssize_t col_offset)
{
    Py_ssize_t len = (Py_ssize_t)strlen(str);
    যদি (col_offset > len + 1) {
        col_offset = len + 1;
    }
    প্রতিজ্ঞা(col_offset >= 0);
    PyObject *text = PyUnicode_DecodeUTF8(str, col_offset, "replace");
    যদি (!text) {
        ফেরত -1;
    }
    Py_ssize_t আকার_2 = Py_Unicode_GET_দৈর্ঘ্য(text);
    Py_DECREF(text);
    ফেরত আকার_2;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset(PyObject *সারি, Py_ssize_t col_offset)
{
    ধ্রুবক অক্ষর *str = PyUnicode_AsUTF8(সারি);
    যদি (!str) {
        ফেরত -1;
    }
    ফেরত _PyPegen_byte_offset_to_character_offset_raw(str, col_offset);
}

// Here, mark is the start of the node, while p->mark is the end.
// If node==NULL, they should be the same.
পূর্ণসংখ্যা
Py_Pegen_ঢোকাও_memo(Parser *p, পূর্ণসংখ্যা mark, পূর্ণসংখ্যা type, শূন্য *গ্রন্থি)
{
    // Insert in front
    Memo *m = _PyArena_Malloc(p->arena, আকার(Memo));
    যদি (m == ফাঁকা) {
        ফেরত -1;
    }
    m->type = type;
    m->গ্রন্থি = গ্রন্থি;
    m->mark = p->mark;
    m->পরবর্তী = p->tokens[mark]->memo;
    p->tokens[mark]->memo = m;
    p->tokens[mark]->memo_mask |= 1ULL << (type & 63);
    ফেরত 0;
}

// Like _PyPegen_insert_memo(), but returns the inserted Memo so callers
// can update it in place without re-walking the token's memo list.
Memo *
Py_Pegen_ঢোকাও_memo_direct(Parser *p, পূর্ণসংখ্যা mark, পূর্ণসংখ্যা type)
{
    যদি (Py_Pegen_ঢোকাও_memo(p, mark, type, ফাঁকা) < 0) {
        ফেরত ফাঁকা;
    }
    ফেরত p->tokens[mark]->memo;
}

// Like _PyPegen_insert_memo(), but updates an existing node if found.
পূর্ণসংখ্যা
_PyPegen_update_memo(Parser *p, পূর্ণসংখ্যা mark, পূর্ণসংখ্যা type, শূন্য *গ্রন্থি)
{
    জন্য (Memo *m = p->tokens[mark]->memo; m != ফাঁকা; m = m->পরবর্তী) {
        যদি (m->type == type) {
            // Update existing node.
            m->গ্রন্থি = গ্রন্থি;
            m->mark = p->mark;
            ফেরত 0;
        }
    }
    // Insert new node.
    ফেরত Py_Pegen_ঢোকাও_memo(p, mark, type, গ্রন্থি);
}

স্থির পূর্ণসংখ্যা
init_normalization(Parser *p)
{
    যদি (p->normalize) {
        ফেরত 1;
    }
    p->normalize = PyImport_ImportModuleAttrString("unicodedata", "normalize");
    যদি (!p->normalize)
    {
        ফেরত 0;
    }
    ফেরত 1;
}

পূর্ণসংখ্যা
Py_Pegen_grow_loop_বাফার(শূন্য ***বাফার, Py_ssize_t *capacity)
{
    যদি ((আকার_t)*capacity > (আকার_t)PY_SSIZE_T_MAX /
            (PEGEN_ARRAY_GROWTH_FACTOR * আকার(**বাফার))) {
        ফেরত -1;
    }
    Py_ssize_t new_capacity = *capacity == 0
        ? 1 : *capacity * PEGEN_ARRAY_GROWTH_FACTOR;
    শূন্য **new_বাফার = PyMem_Realloc(*বাফার, new_capacity * আকার(**বাফার));
    যদি (new_বাফার == ফাঁকা) {
        ফেরত -1;
    }
    *বাফার = new_বাফার;
    *capacity = new_capacity;
    ফেরত 0;
}

স্থির পূর্ণসংখ্যা
growable_comment_array_add(growable_comment_array *arr, পূর্ণসংখ্যা lineno, অক্ষর *comment)
{
    যদি (arr->অঙ্ক_বস্তুসমূহ >= arr->আকার_2) {
        যদি (arr->আকার_2 > (আকার_t)PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * আকার(*arr->বস্তুসমূহ))) {
            ফেরত 0;
        }
        আকার_t new_আকার = arr->আকার_2 == 0
            ? TYPE_IGNORE_INITIAL_CAPACITY
            : arr->আকার_2 * PEGEN_ARRAY_GROWTH_FACTOR;
        শূন্য *new_বস্তুসমূহ_array = PyMem_Realloc(
            arr->বস্তুসমূহ, new_আকার * আকার(*arr->বস্তুসমূহ));
        যদি (!new_বস্তুসমূহ_array) {
            ফেরত 0;
        }
        arr->বস্তুসমূহ = new_বস্তুসমূহ_array;
        arr->আকার_2 = new_আকার;
    }

    arr->বস্তুসমূহ[arr->অঙ্ক_বস্তুসমূহ].lineno = lineno;
    arr->বস্তুসমূহ[arr->অঙ্ক_বস্তুসমূহ].comment = comment;  // Take ownership
    arr->অঙ্ক_বস্তুসমূহ++;
    ফেরত 1;
}

স্থির শূন্য
growable_comment_array_deallocate(growable_comment_array *arr)
{
    জন্য (আকার_t i = 0; i < arr->অঙ্ক_বস্তুসমূহ; i++) {
        PyMem_Free(arr->বস্তুসমূহ[i].comment);
    }
    PyMem_Free(arr->বস্তুসমূহ);
}

স্থির পূর্ণসংখ্যা
_get_keyword_or_name_type(Parser *p, ধ্রুবক অক্ষর *text, Py_ssize_t দৈর্ঘ্য)
{
    প্রতিজ্ঞা(দৈর্ঘ্য > 0);

    যদি (দৈর্ঘ্য >= p->n_keyword_lists ||
        p->keywords[দৈর্ঘ্য] == ফাঁকা ||
        p->keywords[দৈর্ঘ্য]->type == -1) {
        ফেরত NAME;
    }
    জন্য (KeywordToken *k = p->keywords[দৈর্ঘ্য]; k != ফাঁকা && k->type != -1; k++) {
        যদি (memcmp(k->str, text, (আকার_t)দৈর্ঘ্য) == 0) {
            ফেরত k->type;
        }
    }
    ফেরত NAME;
}

// Keep text only where grammar actions or helpers read it. Keyword error
// actions use fixed spellings, so keywords do not need their text.
স্থির অন্তঃরেখ পূর্ণসংখ্যা
token_needs_text(পূর্ণসংখ্যা type)
{
    নির্বাচন (type) {
        ক্ষেত্র NAME:
        ক্ষেত্র সংখ্যা:
        ক্ষেত্র STRING:
        ক্ষেত্র FSTRING_START:
        ক্ষেত্র FSTRING_মধ্য:
        ক্ষেত্র FSTRING_END:
        ক্ষেত্র TSTRING_START:
        ক্ষেত্র TSTRING_মধ্য:
        ক্ষেত্র TSTRING_END:
        ক্ষেত্র TYPE_COMMENT:
        ক্ষেত্র NOTEQUAL:  // _PyPegen_check_barry_as_flufl() distinguishes != and <>.
            ফেরত 1;
        পূর্বনির্ধারিত:
            ফেরত 0;
    }
}

স্থির পূর্ণসংখ্যা
initialize_token(Parser *p, Token *parser_token, কাঠামো token *new_token, পূর্ণসংখ্যা token_type) {
    প্রতিজ্ঞা(parser_token != ফাঁকা);

    parser_token->type = token_type;
    parser_token->bytes = ফাঁকা;
    যদি (token_needs_text(token_type) && new_token->span.start >= 0) {
        Py_ssize_t দৈর্ঘ্য;
        ধ্রুবক অক্ষর *text = _PyToken_TextView(p->tok, new_token, &দৈর্ঘ্য);
        যদি (token_type == NAME) {
            parser_token->type = _get_keyword_or_name_type(p, text, দৈর্ঘ্য);
        }
        // A name reclassified as a keyword does not need its text.
        যদি (parser_token->type == token_type) {
            parser_token->bytes = Py_Bytes_From_String_And_আকার(text, দৈর্ঘ্য);
            যদি (parser_token->bytes == ফাঁকা) {
                ফেরত -1;
            }
            যদি (_PyArena_AddPyObject(p->arena, parser_token->bytes) < 0) {
                Py_DECREF(parser_token->bytes);
                ফেরত -1;
            }
        }
    }

    parser_token->metadata = ফাঁকা;
    যদি (new_token->metadata != ফাঁকা) {
        যদি (_PyArena_AddPyObject(p->arena, new_token->metadata) < 0) {
            ফেরত -1;
        }
        parser_token->metadata = new_token->metadata;
        new_token->metadata = ফাঁকা;
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
        ফেরত _Pypegen_raise_decode_error(p);
    }

    ফেরত (token_type == ERRORTOKEN ? _Pypegen_tokenizer_error(p) : 0);
}

স্থির পূর্ণসংখ্যা
_resize_tokens_array(Parser *p) {
    যদি (p->আকার_2 > INT_MAX / PEGEN_ARRAY_GROWTH_FACTOR ||
            (আকার_t)p->আকার_2 > PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * আকার(*p->tokens))) {
        PyErr_NoMemory();
        ফেরত -1;
    }
    পূর্ণসংখ্যা newsize = p->আকার_2 * PEGEN_ARRAY_GROWTH_FACTOR;
    Token **new_tokens = PyMem_Realloc(p->tokens, (আকার_t)newsize * আকার(Token *));
    যদি (new_tokens == ফাঁকা) {
        PyErr_NoMemory();
        ফেরত -1;
    }
    p->tokens = new_tokens;

    // Allocate new tokens together without moving existing ones: parser rules
    // may still hold pointers into earlier blocks. Parser_Free frees each block.
    Token *chunk = PyMem_Calloc((আকার_t)(newsize - p->আকার_2), আকার(Token));
    যদি (chunk == ফাঁকা) {
        PyErr_NoMemory();
        ফেরত -1;
    }
    জন্য (পূর্ণসংখ্যা i = p->আকার_2; i < newsize; i++) {
        p->tokens[i] = &chunk[i - p->আকার_2];
    }
    p->আকার_2 = newsize;
    ফেরত 0;
}

পূর্ণসংখ্যা
_PyPegen_fill_token(Parser *p)
{
    কাঠামো token new_token;
    _PyToken_Init(&new_token);
    _PyTokenizer_Get(p->tok, &new_token);

    // Record and skip '# type: ignore' comments
    যখন (new_token.type == TYPE_IGNORE) {
        Py_ssize_t len;
        ধ্রুবক অক্ষর *text = _PyToken_TextView(p->tok, &new_token, &len);
        অক্ষর *tag = PyMem_Malloc((আকার_t)len + 1);
        যদি (tag == ফাঁকা) {
            PyErr_NoMemory();
            যাও error;
        }
        memcpy(tag, text, (আকার_t)len);
        tag[len] = '\0';
        // Ownership of tag passes to the growable array
        যদি (!growable_comment_array_add(&p->type_ignore_comments, new_token.end_loc.lineno, tag)) {
            PyMem_Free(tag);
            PyErr_NoMemory();
            যাও error;
        }
        _PyTokenizer_Get(p->tok, &new_token);
    }

    পূর্ণসংখ্যা type = new_token.type;
    // EOF can finish an ordinary token. Only a boundary token means that
    // the grammar requested input beyond the last physical line.
    যদি (type == ENDMARKER ||
        (type == DEDENT && (p->flags & Py_PARSE_ALLOW_INCOMPLETE_প্রবেশ) &&
         _PyTokenizer_GetStatus(p->tok) == E_EOF)) {
        p->প্রবেশ_exhausted = 1;
    }

    // If we have reached the end and we are in single input mode we need to insert a newline and reset the parsing
    যদি (p->start_rule == Py_single_প্রবেশ && type == ENDMARKER && p->parsing_started) {
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
    যদি ((p->fill == p->আকার_2) && (_resize_tokens_array(p) != 0)) {
        যাও error;
    }

    Token *t = p->tokens[p->fill];
    পূর্ণসংখ্যা ফলাফল = initialize_token(p, t, &new_token, type);
    _PyToken_Free(&new_token);
    ফেরত ফলাফল;
error:
    _PyToken_Free(&new_token);
    ফেরত -1;
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
    জন্য (পূর্ণসংখ্যা i = 0; i < NSTATISTICS; i++) {
        memo_statistics[i] = 0;
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
}

PyObject *
_PyPegen_get_memo_statistics(শূন্য)
{
    PyObject *ret = PyList_New(NSTATISTICS);
    যদি (ret == ফাঁকা) {
        ফেরত ফাঁকা;
    }

    PyMutex_Lock(&_PyRuntime.parser.mutex);
    জন্য (পূর্ণসংখ্যা i = 0; i < NSTATISTICS; i++) {
        PyObject *মান = PyLong_FromLong(memo_statistics[i]);
        যদি (মান == ফাঁকা) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            ফেরত ফাঁকা;
        }
        // PyList_SetItem borrows a reference to value.
        যদি (PyList_SetItem(ret, i, মান) < 0) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            ফেরত ফাঁকা;
        }
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
    ফেরত ret;
}
#endif

পূর্ণসংখ্যা  // bool
_PyPegen_is_memoized(Parser *p, পূর্ণসংখ্যা type, শূন্য *pres)
{
    যদি (p->mark == p->fill) {
        যদি (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ফেরত -1;
        }
    }

    Token *t = p->tokens[p->mark];

    যদি (!(t->memo_mask & (1ULL << (type & 63)))) {
        ফেরত 0;
    }

    জন্য (Memo *m = t->memo; m != ফাঁকা; m = m->পরবর্তী) {
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
            *(শূন্য **)(pres) = m->গ্রন্থি;
            ফেরত 1;
        }
    }
    ফেরত 0;
}

#define LOOKAHEAD1(NAME, RES_TYPE)                                  \
    পূর্ণসংখ্যা                                                             \
    NAME (পূর্ণসংখ্যা positive, RES_TYPE (func)(Parser *), Parser *p)       \
    {                                                               \
        পূর্ণসংখ্যা mark = p->mark;                                         \
        শূন্য *res = func(p);                                        \
        p->mark = mark;                                             \
        ফেরত (res != ফাঁকা) == positive;                           \
    }

LOOKAHEAD1(_PyPegen_lookahead, শূন্য *)
LOOKAHEAD1(Py_Pegen_lookahead_for_রাশিমালা, রাশিমালা_ty)
LOOKAHEAD1(_PyPegen_lookahead_for_stmt, stmt_ty)
#undef LOOKAHEAD1

#define LOOKAHEAD2(NAME, RES_TYPE, T)                                   \
    পূর্ণসংখ্যা                                                                 \
    NAME (পূর্ণসংখ্যা positive, RES_TYPE (func)(Parser *, T), Parser *p, T arg) \
    {                                                                   \
        পূর্ণসংখ্যা mark = p->mark;                                             \
        শূন্য *res = func(p, arg);                                       \
        p->mark = mark;                                                 \
        ফেরত (res != ফাঁকা) == positive;                               \
    }

LOOKAHEAD2(_PyPegen_lookahead_with_int, Token *, পূর্ণসংখ্যা)
LOOKAHEAD2(_PyPegen_lookahead_with_string, রাশিমালা_ty, ধ্রুবক অক্ষর *)
#undef LOOKAHEAD2

Token *
_PyPegen_expect_token(Parser *p, পূর্ণসংখ্যা type)
{
    যদি (p->mark == p->fill) {
        যদি (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ফেরত ফাঁকা;
        }
    }
    Token *t = p->tokens[p->mark];
    যদি (t->type != type) {
       ফেরত ফাঁকা;
    }
    p->mark += 1;
    ফেরত t;
}

শূন্য*
Py_Pegen_expect_forced_ফলাফল(Parser *p, শূন্য* ফলাফল, ধ্রুবক অক্ষর* expected) {

    যদি (p->error_indicator == 1) {
        ফেরত ফাঁকা;
    }
    যদি (ফলাফল == ফাঁকা) {
        RAISE_SYNTAX_ERROR("expected (%s)", expected);
        ফেরত ফাঁকা;
    }
    ফেরত ফলাফল;
}

Token *
_PyPegen_expect_forced_token(Parser *p, পূর্ণসংখ্যা type, ধ্রুবক অক্ষর* expected) {

    যদি (p->error_indicator == 1) {
        ফেরত ফাঁকা;
    }

    যদি (p->mark == p->fill) {
        যদি (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ফেরত ফাঁকা;
        }
    }
    Token *t = p->tokens[p->mark];
    যদি (t->type != type) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(t, "expected '%s'", expected);
        ফেরত ফাঁকা;
    }
    p->mark += 1;
    ফেরত t;
}

রাশিমালা_ty
_PyPegen_expect_soft_keyword(Parser *p, ধ্রুবক অক্ষর *keyword)
{
    যদি (p->mark == p->fill) {
        যদি (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ফেরত ফাঁকা;
        }
    }
    Token *t = p->tokens[p->mark];
    যদি (t->type != NAME) {
        ফেরত ফাঁকা;
    }
    ধ্রুবক অক্ষর *s = PyBytes_AsString(t->bytes);
    যদি (!s) {
        p->error_indicator = 1;
        ফেরত ফাঁকা;
    }
    যদি (strcmp(s, keyword) != 0) {
        ফেরত ফাঁকা;
    }
    ফেরত _PyPegen_name_token(p);
}

Token *
_PyPegen_get_last_nonnwhitespace_token(Parser *p)
{
    প্রতিজ্ঞা(p->mark >= 0);
    Token *token = ফাঁকা;
    জন্য (পূর্ণসংখ্যা m = p->mark - 1; m >= 0; m--) {
        token = p->tokens[m];
        যদি (token->type != ENDMARKER && (token->type < NEWLINE || token->type > DEDENT)) {
            বিরতি;
        }
    }
    ফেরত token;
}

PyObject *
_PyPegen_new_identifier(Parser *p, ধ্রুবক অক্ষর *n)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, (Py_ssize_t)strlen(n), ফাঁকা);
    যদি (!id) {
        যাও error;
    }
    /* Check whether there are non-ASCII characters in the
       identifier; if so, normalize to NFKC. */
    যদি (!PyUnicode_IS_ASCII(id))
    {
        যদি (!init_normalization(p))
        {
            Py_DECREF(id);
            যাও error;
        }
        PyObject *form = PyUnicode_InternFromString("NFKC");
        যদি (form == ফাঁকা)
        {
            Py_DECREF(id);
            যাও error;
        }
        PyObject *args[2] = {form, id};
        PyObject *id2 = PyObject_Vectorcall(p->normalize, args, 2, ফাঁকা);
        Py_DECREF(id);
        Py_DECREF(form);
        যদি (!id2) {
            যাও error;
        }

        যদি (!PyUnicode_Check(id2))
        {
            PyErr_Format(PyExc_TypeError,
                         "unicodedata.normalize() must return a string, not "
                         "%.200s",
                         _PyType_Name(Py_TYPE(id2)));
            Py_DECREF(id2);
            যাও error;
        }
        id = id2;
    }
    স্থির ধ্রুবক অক্ষর * ধ্রুবক forbidden[] = {
        "None",
        "True",
        "False",
        ফাঁকা
    };
    জন্য (পূর্ণসংখ্যা i = 0; forbidden[i] != ফাঁকা; i++) {
        যদি (_PyUnicode_EqualToASCIIString(id, forbidden[i])) {
            PyErr_Format(Py_Exc_মান_Error,
                         "identifier field can't represent '%s' constant",
                         forbidden[i]);
            Py_DECREF(id);
            যাও error;
        }
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &id);
    যদি (_PyArena_AddPyObject(p->arena, id) < 0)
    {
        Py_DECREF(id);
        যাও error;
    }
    ফেরত id;

error:
    p->error_indicator = 1;
    ফেরত ফাঁকা;
}

// Return an arena-owned identifier; callers borrow the reference.
স্থির PyObject *
get_cached_identifier(Parser *p, PyObject *bytes)
{
    ধ্রুবক অক্ষর *s = PyBytes_AsString(bytes);
    যদি (!s) {
        p->error_indicator = 1;
        ফেরত ফাঁকা;
    }
    // Identifiers repeat constantly; a small span-keyed cache skips the
    // UTF-8 decode + intern for repeated occurrences. Keys point into
    // arena-owned token bytes and values are arena-owned interned strings,
    // so borrowed references are valid for the lifetime of the parse
    // (including the second error pass, which reuses parser and arena).
    Py_ssize_t len = Py_Bytes_GET_আকার(bytes);
    Py_hash_t hash = PyObject_Hash(bytes);
    যদি (hash == -1) {
        p->error_indicator = 1;
        ফেরত ফাঁকা;
    }
    // Parses without identifiers do not need this cache.
    যদি (p->identifier_cache == ফাঁকা) {
        p->identifier_cache = PyMem_Calloc(
            IDENTIFIER_CACHE_আকার, আকার(*p->identifier_cache));
        যদি (p->identifier_cache == ফাঁকা) {
            p->error_indicator = 1;
            ফেরত PyErr_NoMemory();
        }
    }
    IdentifierCacheEntry *free_slot = ফাঁকা;
    আকার_t idx = (আকার_t)hash & (IDENTIFIER_CACHE_আকার - 1);
    জন্য (পূর্ণসংখ্যা probe = 0; probe < IDENTIFIER_CACHE_MAX_PROBES; probe++) {
        IdentifierCacheEntry *entry = &p->identifier_cache[
            (idx + probe) & (IDENTIFIER_CACHE_আকার - 1)];
        যদি (entry->চাবি == ফাঁকা) {
            free_slot = entry;
            বিরতি;
        }
        যদি (entry->hash == hash && entry->len == len &&
            memcmp(entry->চাবি, s, len) == 0)
        {
            ফেরত entry->মান;
        }
    }
    PyObject *id = _PyPegen_new_identifier(p, s);
    যদি (id == ফাঁকা) {
        p->error_indicator = 1;
        ফেরত ফাঁকা;
    }
    যদি (free_slot != ফাঁকা) {
        free_slot->চাবি = s;
        free_slot->len = len;
        free_slot->hash = hash;
        free_slot->মান = id;
    }
    ফেরত id;
}

স্থির রাশিমালা_ty
_PyPegen_name_from_token(Parser *p, Token* t)
{
    যদি (t == ফাঁকা) {
        ফেরত ফাঁকা;
    }
    // Reuse the AST node when backtracking revisits this token. Memo lookup
    // starts before the token and restores the position after it on a hit.
    // Token kinds can be memo keys: generated grammar rule IDs start at 1000.
    পূর্ণসংখ্যা mark = p->mark - 1;
    p->mark = mark;
    রাশিমালা_ty cached = ফাঁকা;
    যদি (_PyPegen_is_memoized(p, NAME, &cached)) {
        ফেরত cached;
    }
    p->mark = mark + 1;
    PyObject *id = get_cached_identifier(p, t->bytes);
    যদি (id == ফাঁকা) {
        ফেরত ফাঁকা;
    }
    রাশিমালা_ty ফলাফল = _PyAST_Name(id, Load, t->lineno, t->col_offset,
                                t->end_lineno, t->end_col_offset, p->arena);
    যদি (ফলাফল != ফাঁকা && Py_Pegen_ঢোকাও_memo(p, mark, NAME, ফলাফল) < 0) {
        p->error_indicator = 1;
        ফেরত ফাঁকা;
    }
    ফেরত ফলাফল;
}

রাশিমালা_ty
_PyPegen_name_token(Parser *p)
{
    Token *t = _PyPegen_expect_token(p, NAME);
    ফেরত _PyPegen_name_from_token(p, t);
}

শূন্য *
_PyPegen_string_token(Parser *p)
{
    ফেরত _PyPegen_expect_token(p, STRING);
}

রাশিমালা_ty _PyPegen_soft_keyword_token(Parser *p) {
    Token *t = _PyPegen_expect_token(p, NAME);
    যদি (t == ফাঁকা) {
        ফেরত ফাঁকা;
    }
    অক্ষর *the_token;
    Py_ssize_t আকার_2;
    Py_Bytes_As_String_And_আকার(t->bytes, &the_token, &আকার_2);
    জন্য (অক্ষর **keyword = p->soft_keywords; *keyword != ফাঁকা; keyword++) {
        যদি (strlen(*keyword) == (আকার_t)আকার_2 &&
            strncmp(*keyword, the_token, (আকার_t)আকার_2) == 0) {
            ফেরত _PyPegen_name_from_token(p, t);
        }
    }
    ফেরত ফাঁকা;
}

স্থির PyObject *
parsenumber_raw(ধ্রুবক অক্ষর *s)
{
    ধ্রুবক অক্ষর *end;
    long x;
    দ্বিগুণ dx;
    Py_complex compl;
    পূর্ণসংখ্যা imflag;

    প্রতিজ্ঞা(s != ফাঁকা);
    errno = 0;
    end = s + strlen(s) - 1;
    imflag = *end == 'j' || *end == 'J';
    যদি (s[0] == '0') {
        x = (long)PyOS_strtoul(s, (অক্ষর **)&end, 0);
        যদি (x < 0 && errno == 0) {
            ফেরত PyLong_FromString(s, (অক্ষর **)0, 0);
        }
    }
    অন্যথা {
        x = PyOS_strtol(s, (অক্ষর **)&end, 0);
    }
    যদি (*end == '\0') {
        যদি (errno != 0) {
            ফেরত PyLong_FromString(s, (অক্ষর **)0, 0);
        }
        ফেরত PyLong_FromLong(x);
    }
    /* XXX Huge floats may silently fail */
    যদি (imflag) {
        compl.real = 0.;
        compl.imag = PyOS_string_to_double(s, (অক্ষর **)&end, ফাঁকা);
        যদি (compl.imag == -1.0 && PyErr_Occurred()) {
            ফেরত ফাঁকা;
        }
        ফেরত PyComplex_FromCComplex(compl);
    }
    dx = PyOS_string_to_double(s, ফাঁকা, ফাঁকা);
    যদি (dx == -1.0 && PyErr_Occurred()) {
        ফেরত ফাঁকা;
    }
    ফেরত PyFloat_FromDouble(dx);
}

স্থির PyObject *
parsenumber(ধ্রুবক অক্ষর *s)
{
    অক্ষর *dup;
    অক্ষর *end;
    PyObject *res = ফাঁকা;

    প্রতিজ্ঞা(s != ফাঁকা);

    যদি (strchr(s, '_') == ফাঁকা) {
        ফেরত parsenumber_raw(s);
    }
    /* Create a duplicate without underscores. */
    dup = PyMem_Malloc(strlen(s) + 1);
    যদি (dup == ফাঁকা) {
        ফেরত PyErr_NoMemory();
    }
    end = dup;
    জন্য (; *s; s++) {
        যদি (*s != '_') {
            *end++ = *s;
        }
    }
    *end = '\0';
    res = parsenumber_raw(dup);
    PyMem_Free(dup);
    ফেরত res;
}

রাশিমালা_ty
Py_Pegen_সংখ্যা_token(Parser *p)
{
    পূর্ণসংখ্যা mark = p->mark;
    Token *t = _PyPegen_expect_token(p, সংখ্যা);
    যদি (t == ফাঁকা) {
        ফেরত ফাঁকা;
    }

    // Look up the node at the token's start, as for NAME tokens. A memo hit
    // advances the mark; on a miss, restore the position after the token.
    p->mark = mark;
    রাশিমালা_ty cached = ফাঁকা;
    যদি (_PyPegen_is_memoized(p, সংখ্যা, &cached)) {
        ফেরত cached;
    }
    p->mark = mark + 1;

    ধ্রুবক অক্ষর *অঙ্ক_raw = PyBytes_AsString(t->bytes);
    যদি (অঙ্ক_raw == ফাঁকা) {
        p->error_indicator = 1;
        ফেরত ফাঁকা;
    }

    যদি (p->feature_version < 6 && strchr(অঙ্ক_raw, '_') != ফাঁকা) {
        p->error_indicator = 1;
        ফেরত RAISE_SYNTAX_ERROR("Underscores in numeric literals are only supported "
                                  "in Python 3.6 and greater");
    }

    PyObject *c = parsenumber(অঙ্ক_raw);

    যদি (c == ফাঁকা) {
        p->error_indicator = 1;
        PyThreadState *tstate = _PyThreadState_GET();
        // The only way a ValueError should happen in _this_ code is via
        // PyLong_FromString hitting a length limit.
        যদি (tstate->current_exception != ফাঁকা &&
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
        ফেরত ফাঁকা;
    }

    যদি (_PyArena_AddPyObject(p->arena, c) < 0) {
        Py_DECREF(c);
        p->error_indicator = 1;
        ফেরত ফাঁকা;
    }

    রাশিমালা_ty ফলাফল = _PyAST_Constant(c, ফাঁকা, t->lineno, t->col_offset,
                                    t->end_lineno, t->end_col_offset, p->arena);
    যদি (ফলাফল != ফাঁকা && Py_Pegen_ঢোকাও_memo(p, mark, সংখ্যা, ফলাফল) < 0) {
        p->error_indicator = 1;
        ফেরত ফাঁকা;
    }
    ফেরত ফলাফল;
}



স্থির পূর্ণসংখ্যা
compute_parser_flags(PyCompilerFlags *flags)
{
    পূর্ণসংখ্যা parser_flags = 0;
    যদি (!flags) {
        ফেরত 0;
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
    যদি (flags->cf_flags & Py_CF_ALLOW_INCOMPLETE_প্রবেশ) {
        parser_flags |= Py_PARSE_ALLOW_INCOMPLETE_প্রবেশ;
    }
    ফেরত parser_flags;
}

// Parser API

Parser *
_PyPegen_Parser_New(কাঠামো tok_state *tok, পূর্ণসংখ্যা start_rule, পূর্ণসংখ্যা flags,
                    পূর্ণসংখ্যা feature_version, পূর্ণসংখ্যা *errcode, ধ্রুবক অক্ষর* source, PyArena *arena)
{
    Parser *p = PyMem_Malloc(আকার(Parser));
    যদি (p == ফাঁকা) {
        ফেরত (Parser *) PyErr_NoMemory();
    }
    প্রতিজ্ঞা(tok != ফাঁকা);
    _PyTokenizer_SetOptions(tok, 0, (flags & PyPARSE_TYPE_COMMENTS) > 0);
    Py_Tokenizer_Allow_Incomplete_প্রবেশ(tok, flags & Py_PARSE_ALLOW_INCOMPLETE_প্রবেশ);
    p->tok = tok;
    p->keywords = ফাঁকা;
    p->n_keyword_lists = -1;
    p->soft_keywords = ফাঁকা;
    p->tokens = PyMem_Malloc(আকার(Token *));
    যদি (!p->tokens) {
        PyMem_Free(p);
        ফেরত (Parser *) PyErr_NoMemory();
    }
    p->tokens[0] = PyMem_Calloc(1, আকার(Token));
    যদি (!p->tokens[0]) {
        PyMem_Free(p->tokens);
        PyMem_Free(p);
        ফেরত (Parser *) PyErr_NoMemory();
    }
    p->type_ignore_comments = (growable_comment_array){0};

    p->mark = 0;
    p->fill = 0;
    p->আকার_2 = 1;

    p->errcode = errcode;
    p->arena = arena;
    p->start_rule = start_rule;
    p->parsing_started = 0;
    p->normalize = ফাঁকা;
    p->error_indicator = 0;

    p->starting_lineno = 0;
    p->starting_col_offset = 0;
    p->flags = flags;
    p->feature_version = feature_version;
    p->known_err_token = ফাঁকা;
    p->identifier_cache = ফাঁকা;
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
    p->প্রবেশ_exhausted = 0;
    p->last_stmt_location.lineno = 0;
    p->last_stmt_location.col_offset = 0;
    p->last_stmt_location.end_lineno = 0;
    p->last_stmt_location.end_col_offset = 0;
#ifdef Py_DEBUG
    p->debug = _Py_GetConfig()->parser_debug;
#endif
    ফেরত p;
}

শূন্য
_PyPegen_Parser_Free(Parser *p)
{
    PyMem_Free(p->identifier_cache);
    Py_XDECREF(p->normalize);
    // Resizes allocate blocks starting at indices 1, 2, 4, and so on.
    PyMem_Free(p->tokens[0]);
    জন্য (পূর্ণসংখ্যা i = 1; i < p->আকার_2; i *= PEGEN_ARRAY_GROWTH_FACTOR) {
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
    জন্য (পূর্ণসংখ্যা i = 0; i < p->fill; i++) {
        p->tokens[i]->memo = ফাঁকা;
        p->tokens[i]->memo_mask = 0;
    }
    p->mark = 0;
    p->flags &= ~Py_PARSE_ALLOW_INCOMPLETE_প্রবেশ;
    Py_Tokenizer_Allow_Incomplete_প্রবেশ(p->tok, 0);
    p->call_invalid_rules = 1;
    _PyTokenizer_StopInteractive(p->tok);
}

স্থির শূন্য
_PyPegen_set_syntax_error_metadata(Parser *p) {
    PyObject *exc = PyErr_GetRaisedException();
    যদি (!exc || !PyObject_TypeCheck(exc, (PyTypeObject *)PyExc_SyntaxError)) {
        PyErr_SetRaisedException(exc);
        ফেরত;
    }
    ধ্রুবক অক্ষর *source = _PyTokenizer_RetainedSource(p->tok);
    ধ্রুবক অক্ষর *encoding = _PyTokenizer_GetInfo(p->tok).encoding;
    PyObject* the_source = ফাঁকা;
    যদি (source) {
        যদি (encoding == ফাঁকা) {
            the_source = PyUnicode_FromString(source);
        } অন্যথা {
            the_source = PyUnicode_Decode(source, strlen(source), encoding, ফাঁকা);
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
        ফেরত;
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
    যদি (res != ফাঁকা && PyErr_Occurred()) {
        // Discard a result returned with an exception still pending
        // (e.g. a MemoryError from a recovered-from allocation failure).
        ফেরত ফাঁকা;
    }
    যদি (res == ফাঁকা) {
        যদি (PyErr_Occurred() &&
            (PyErr_ExceptionMatches(Py_Exc_Incomplete_প্রবেশ_Error) ||
             !PyErr_ExceptionMatches(PyExc_SyntaxError))) {
            ফেরত ফাঁকা;
        }
        যদি ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_প্রবেশ) &&
            p->প্রবেশ_exhausted && !PyErr_Occurred()) {
            ফেরত Py_Pegen_raise_incomplete_প্রবেশ(p);
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
       ফেরত ফাঁকা;
    }

    যদি (p->start_rule == Py_single_প্রবেশ && _PyTokenizer_HasTrailingStatement(p->tok)) {
        ফেরত RAISE_SYNTAX_ERROR("multiple statements found while compiling a single statement");
    }

    // test_peg_generator defines _Py_TEST_PEGEN to not call PyAST_Validate()
#যদি defined(Py_DEBUG) && !defined(_Py_TEST_PEGEN)
    যদি (p->start_rule == Py_single_প্রবেশ ||
        p->start_rule == Py_file_প্রবেশ ||
        p->start_rule == Py_eval_প্রবেশ)
    {
        যদি (!_PyAST_Validate(res)) {
            ফেরত ফাঁকা;
        }
    }
#endif
    ফেরত res;
}

mod_ty
_PyPegen_run_parser_from_file_pointer(FILE *fp, পূর্ণসংখ্যা start_rule, PyObject *filename_ob,
                             ধ্রুবক অক্ষর *enc, ধ্রুবক অক্ষর *ps1, ধ্রুবক অক্ষর *ps2,
                             PyCompilerFlags *flags, পূর্ণসংখ্যা *errcode,
                             PyObject **interactive_src, PyArena *arena)
{
    পূর্ণসংখ্যা parser_flags = compute_parser_flags(flags);
    কাঠামো tok_state *tok = _PyTokenizer_FromFile(fp, enc, ps1, ps2);
    যদি (tok == ফাঁকা) {
        যদি (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        অন্যথা {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        ফেরত ফাঁকা;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    // From here on we need to clean up even if there's an error
    mod_ty ফলাফল = ফাঁকা;

    PyObject *module = PyUnicode_FromString("__main__");
    যদি (module == ফাঁকা) {
        যাও error;
    }
    _PyTokenizer_SetContext(tok, filename_ob, module);
    Py_DECREF(module);

    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, PY_MINOR_VERSION,
                                    errcode, ফাঁকা, arena);
    যদি (p == ফাঁকা) {
        যাও error;
    }

    ফলাফল = _PyPegen_run_parser(p);
    _PyPegen_Parser_Free(p);

    ধ্রুবক অক্ষর *source = _PyTokenizer_RetainedSource(tok);
    যদি (source != ফাঁকা && ফলাফল && interactive_src != ফাঁকা) {
        *interactive_src = PyUnicode_FromString(source);
        যদি (!*interactive_src || _PyArena_AddPyObject(arena, *interactive_src) < 0) {
            Py_XDECREF(*interactive_src);
            ফলাফল = ফাঁকা;
            যাও error;
        }
    }

error:
    _PyTokenizer_Free(tok);
    ফেরত ফলাফল;
}

mod_ty
_PyPegen_run_parser_from_string(ধ্রুবক অক্ষর *str, পূর্ণসংখ্যা start_rule, PyObject *filename_ob,
                       PyCompilerFlags *flags, PyArena *arena, PyObject *module,
                       PyObject **incomplete)
{
    _PyTokenizer_Newline newline_mode = start_rule == Py_file_প্রবেশ
        ? _PyTokenizer_IMPLY_NEWLINE : _PyTokenizer_KEEP_NEWLINE;
    পূর্ণসংখ্যা parser_flags = compute_parser_flags(flags);
    যদি (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_প্রবেশ) {
        newline_mode = _PyTokenizer_APPEND_NEWLINE;
    }

    কাঠামো tok_state *tok;
    যদি (flags != ফাঁকা && flags->cf_flags & PyCF_IGNORE_COOKIE) {
        tok = _PyTokenizer_FromUTF8(str, newline_mode, 0);
    } অন্যথা {
        tok = _PyTokenizer_FromString(str, newline_mode, 0);
    }
    যদি (tok == ফাঁকা) {
        যদি (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        অন্যথা {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        ফেরত ফাঁকা;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    _PyTokenizer_SetContext(tok, filename_ob, module);

    // We need to clear up from here on
    mod_ty ফলাফল = ফাঁকা;

    পূর্ণসংখ্যা feature_version = flags && (flags->cf_flags & PyCF_ONLY_AST) ?
        flags->cf_feature_version : PY_MINOR_VERSION;
    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, feature_version,
                                    ফাঁকা, str, arena);
    যদি (p == ফাঁকা) {
        যাও error;
    }

    ফলাফল = _PyPegen_run_parser(p);
    যদি (ফলাফল != ফাঁকা && (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_প্রবেশ)) {
        পূর্ণসংখ্যা needs_newline = _PyTokenizer_GetNewlineRequirements(tok);
        যদি ((needs_newline & _PyTokenizer_NEWLINE_CONTINUATION) ||
            (start_rule != Py_file_প্রবেশ &&
             (needs_newline & _PyTokenizer_NEWLINE_INDENT)) ||
            (start_rule == Py_single_প্রবেশ && p->প্রবেশ_exhausted &&
             (needs_newline & _PyTokenizer_NEWLINE_STATEMENT))) {
            Py_Pegen_raise_incomplete_প্রবেশ(p);
            যদি (incomplete != ফাঁকা && PyErr_ExceptionMatches(Py_Exc_Incomplete_প্রবেশ_Error)) {
                *incomplete = PyErr_GetRaisedException();
            }
            অন্যথা {
                ফলাফল = ফাঁকা;
            }
        }
    }
    _PyPegen_Parser_Free(p);

error:
    _PyTokenizer_Free(tok);
    ফেরত ফলাফল;
}
