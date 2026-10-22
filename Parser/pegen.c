#ଅନ୍ତର୍ଭାଵ <Python.h>
#ଅନ୍ତର୍ଭାଵ "pycore_ast.h"           // _PyAST_Validate(),
#ଅନ୍ତର୍ଭାଵ "pycore_pystate.h"       // _PyThreadState_GET()
#ଅନ୍ତର୍ଭାଵ "pycore_parser.h"        // _PYPEGEN_NSTATISTICS
#ଅନ୍ତର୍ଭାଵ "pycore_pyerrors.h"      // PyExc_IncompleteInputError
#ଅନ୍ତର୍ଭାଵ "pycore_runtime.h"       // _PyRuntime
#ଅନ୍ତର୍ଭାଵ "pycore_unicodeobject.h" // _PyUnicode_InternImmortal
#ଅନ୍ତର୍ଭାଵ <errcode.h>

#ଅନ୍ତର୍ଭାଵ "lexer/state.h"
#ଅନ୍ତର୍ଭାଵ "tokenizer/tokenizer.h"
#ଅନ୍ତର୍ଭାଵ "pegen.h"

#define IDENTIFIER_CACHE_SIZE 2048  // Must be a power of two.
#define IDENTIFIER_CACHE_MAX_PROBES 8
#define PEGEN_ARRAY_GROWTH_FACTOR 2
#define TYPE_IGNORE_INITIAL_CAPACITY 10

ସଂରଚନା _identifier_cache_entry {
    ଅଚଳ ଅକ୍ଷର *ଚାବି;   // Borrowed from arena-owned token bytes.
    Py_ssize_t len;
    Py_hash_t hash;
    PyObject *ମୂଲ୍ୟ;   // Borrowed from an arena-owned identifier.
};

// Internal parser functions

asdl_stmt_seq*
_PyPegen_interactive_exit(Parser *p)
{
    ଯଦି (p->errcode) {
        *(p->errcode) = E_EOF;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

Py_ssize_t
Py_Pegen_byte_offset_to_character_offset_ଧାଡ଼ି(PyObject *ଧାଡ଼ି, Py_ssize_t col_offset, Py_ssize_t end_col_offset)
{
    ଅଚଳ unsigned ଅକ୍ଷର *data = (ଅଚଳ unsigned ଅକ୍ଷର*)PyUnicode_AsUTF8(ଧାଡ଼ି);
    ଯଦି (data == ଖାଲି) {
        ଫେରନ୍ତୁ -1;
    }

    Py_ssize_t len = 0;
    ଯେତେବେଳେ (col_offset < end_col_offset) {
        Py_UCS4 ch = data[col_offset];
        ଯଦି (ch < 0x80) {
            col_offset += 1;
        } ଅନ୍ୟଥା ଯଦି ((ch & 0xe0) == 0xc0) {
            col_offset += 2;
        } ଅନ୍ୟଥା ଯଦି ((ch & 0xf0) == 0xe0) {
            col_offset += 3;
        } ଅନ୍ୟଥା ଯଦି ((ch & 0xf8) == 0xf0) {
            col_offset += 4;
        } ଅନ୍ୟଥା {
            PyErr_SetString(Py_Exc_ମୂଲ୍ୟ_Error, "Invalid UTF-8 sequence");
            ଫେରନ୍ତୁ -1;
        }
        len++;
    }
    ଫେରନ୍ତୁ len;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset_raw(ଅଚଳ ଅକ୍ଷର* str, Py_ssize_t col_offset)
{
    Py_ssize_t len = (Py_ssize_t)strlen(str);
    ଯଦି (col_offset > len + 1) {
        col_offset = len + 1;
    }
    ପ୍ରତିଜ୍ଞା(col_offset >= 0);
    PyObject *text = PyUnicode_DecodeUTF8(str, col_offset, "replace");
    ଯଦି (!text) {
        ଫେରନ୍ତୁ -1;
    }
    Py_ssize_t size = PyUnicode_GET_LENGTH(text);
    Py_DECREF(text);
    ଫେରନ୍ତୁ size;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset(PyObject *ଧାଡ଼ି, Py_ssize_t col_offset)
{
    ଅଚଳ ଅକ୍ଷର *str = PyUnicode_AsUTF8(ଧାଡ଼ି);
    ଯଦି (!str) {
        ଫେରନ୍ତୁ -1;
    }
    ଫେରନ୍ତୁ _PyPegen_byte_offset_to_character_offset_raw(str, col_offset);
}

// Here, mark is the start of the node, while p->mark is the end.
// If node==NULL, they should be the same.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
Py_Pegen_ଭର୍ତ୍ତି_memo(Parser *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mark, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, ଶୂନ୍ୟ *ଗଣ୍ଠି)
{
    // Insert in front
    Memo *m = _PyArena_Malloc(p->arena, ଆକାର(Memo));
    ଯଦି (m == ଖାଲି) {
        ଫେରନ୍ତୁ -1;
    }
    m->type = type;
    m->ଗଣ୍ଠି = ଗଣ୍ଠି;
    m->mark = p->mark;
    m->next = p->tokens[mark]->memo;
    p->tokens[mark]->memo = m;
    p->tokens[mark]->memo_mask |= 1ULL << (type & 63);
    ଫେରନ୍ତୁ 0;
}

// Like _PyPegen_insert_memo(), but returns the inserted Memo so callers
// can update it in place without re-walking the token's memo list.
Memo *
Py_Pegen_ଭର୍ତ୍ତି_memo_direct(Parser *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mark, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type)
{
    ଯଦି (Py_Pegen_ଭର୍ତ୍ତି_memo(p, mark, type, ଖାଲି) < 0) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ p->tokens[mark]->memo;
}

// Like _PyPegen_insert_memo(), but updates an existing node if found.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyPegen_update_memo(Parser *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mark, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, ଶୂନ୍ୟ *ଗଣ୍ଠି)
{
    ପାଇଁ (Memo *m = p->tokens[mark]->memo; m != ଖାଲି; m = m->next) {
        ଯଦି (m->type == type) {
            // Update existing node.
            m->ଗଣ୍ଠି = ଗଣ୍ଠି;
            m->mark = p->mark;
            ଫେରନ୍ତୁ 0;
        }
    }
    // Insert new node.
    ଫେରନ୍ତୁ Py_Pegen_ଭର୍ତ୍ତି_memo(p, mark, type, ଗଣ୍ଠି);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
init_normalization(Parser *p)
{
    ଯଦି (p->normalize) {
        ଫେରନ୍ତୁ 1;
    }
    p->normalize = PyImport_ImportModuleAttrString("unicodedata", "normalize");
    ଯଦି (!p->normalize)
    {
        ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyPegen_grow_loop_buffer(ଶୂନ୍ୟ ***buffer, Py_ssize_t *capacity)
{
    ଯଦି ((size_t)*capacity > (size_t)PY_SSIZE_T_MAX /
            (PEGEN_ARRAY_GROWTH_FACTOR * ଆକାର(**buffer))) {
        ଫେରନ୍ତୁ -1;
    }
    Py_ssize_t new_capacity = *capacity == 0
        ? 1 : *capacity * PEGEN_ARRAY_GROWTH_FACTOR;
    ଶୂନ୍ୟ **new_buffer = PyMem_Realloc(*buffer, new_capacity * ଆକାର(**buffer));
    ଯଦି (new_buffer == ଖାଲି) {
        ଫେରନ୍ତୁ -1;
    }
    *buffer = new_buffer;
    *capacity = new_capacity;
    ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
growable_comment_array_add(growable_comment_array *arr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno, ଅକ୍ଷର *comment)
{
    ଯଦି (arr->ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ >= arr->size) {
        ଯଦି (arr->size > (size_t)PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * ଆକାର(*arr->ବସ୍ତୁଗୁଡ଼ିକ))) {
            ଫେରନ୍ତୁ 0;
        }
        size_t new_size = arr->size == 0
            ? TYPE_IGNORE_INITIAL_CAPACITY
            : arr->size * PEGEN_ARRAY_GROWTH_FACTOR;
        ଶୂନ୍ୟ *new_ବସ୍ତୁଗୁଡ଼ିକ_array = PyMem_Realloc(
            arr->ବସ୍ତୁଗୁଡ଼ିକ, new_size * ଆକାର(*arr->ବସ୍ତୁଗୁଡ଼ିକ));
        ଯଦି (!new_ବସ୍ତୁଗୁଡ଼ିକ_array) {
            ଫେରନ୍ତୁ 0;
        }
        arr->ବସ୍ତୁଗୁଡ଼ିକ = new_ବସ୍ତୁଗୁଡ଼ିକ_array;
        arr->size = new_size;
    }

    arr->ବସ୍ତୁଗୁଡ଼ିକ[arr->ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ].lineno = lineno;
    arr->ବସ୍ତୁଗୁଡ଼ିକ[arr->ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ].comment = comment;  // Take ownership
    arr->ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ++;
    ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ଶୂନ୍ୟ
growable_comment_array_deallocate(growable_comment_array *arr)
{
    ପାଇଁ (size_t i = 0; i < arr->ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ; i++) {
        PyMem_Free(arr->ବସ୍ତୁଗୁଡ଼ିକ[i].comment);
    }
    PyMem_Free(arr->ବସ୍ତୁଗୁଡ଼ିକ);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_get_keyword_or_name_type(Parser *p, ଅଚଳ ଅକ୍ଷର *text, Py_ssize_t length)
{
    ପ୍ରତିଜ୍ଞା(length > 0);

    ଯଦି (length >= p->n_keyword_lists ||
        p->keywords[length] == ଖାଲି ||
        p->keywords[length]->type == -1) {
        ଫେରନ୍ତୁ NAME;
    }
    ପାଇଁ (KeywordToken *k = p->keywords[length]; k != ଖାଲି && k->type != -1; k++) {
        ଯଦି (memcmp(k->str, text, (size_t)length) == 0) {
            ଫେରନ୍ତୁ k->type;
        }
    }
    ଫେରନ୍ତୁ NAME;
}

// Keep text only where grammar actions or helpers read it. Keyword error
// actions use fixed spellings, so keywords do not need their text.
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା
token_needs_text(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type)
{
    ବାଛ (type) {
        ସ୍ଥିତି NAME:
        ସ୍ଥିତି ସଂଖ୍ୟା:
        ସ୍ଥିତି STRING:
        ସ୍ଥିତି FSTRING_START:
        ସ୍ଥିତି FSTRING_ମଧ୍ୟ:
        ସ୍ଥିତି FSTRING_END:
        ସ୍ଥିତି TSTRING_START:
        ସ୍ଥିତି TSTRING_ମଧ୍ୟ:
        ସ୍ଥିତି TSTRING_END:
        ସ୍ଥିତି TYPE_COMMENT:
        ସ୍ଥିତି NOTEQUAL:  // _PyPegen_check_barry_as_flufl() distinguishes != and <>.
            ଫେରନ୍ତୁ 1;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            ଫେରନ୍ତୁ 0;
    }
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
initialize_token(Parser *p, Token *parser_token, ସଂରଚନା token *new_token, ପୂର୍ଣ୍ଣସଂଖ୍ୟା token_type) {
    ପ୍ରତିଜ୍ଞା(parser_token != ଖାଲି);

    parser_token->type = token_type;
    parser_token->bytes = ଖାଲି;
    ଯଦି (token_needs_text(token_type) && new_token->span.start >= 0) {
        Py_ssize_t length;
        ଅଚଳ ଅକ୍ଷର *text = _PyToken_TextView(p->tok, new_token, &length);
        ଯଦି (token_type == NAME) {
            parser_token->type = _get_keyword_or_name_type(p, text, length);
        }
        // A name reclassified as a keyword does not need its text.
        ଯଦି (parser_token->type == token_type) {
            parser_token->bytes = PyBytes_FromStringAndSize(text, length);
            ଯଦି (parser_token->bytes == ଖାଲି) {
                ଫେରନ୍ତୁ -1;
            }
            ଯଦି (_PyArena_AddPyObject(p->arena, parser_token->bytes) < 0) {
                Py_DECREF(parser_token->bytes);
                ଫେରନ୍ତୁ -1;
            }
        }
    }

    parser_token->metadata = ଖାଲି;
    ଯଦି (new_token->metadata != ଖାଲି) {
        ଯଦି (_PyArena_AddPyObject(p->arena, new_token->metadata) < 0) {
            ଫେରନ୍ତୁ -1;
        }
        parser_token->metadata = new_token->metadata;
        new_token->metadata = ଖାଲି;
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

    ଯଦି (token_type == ERRORTOKEN && _PyTokenizer_GetStatus(p->tok) == E_DECODE) {
        ଫେରନ୍ତୁ _Pypegen_raise_decode_error(p);
    }

    ଫେରନ୍ତୁ (token_type == ERRORTOKEN ? _Pypegen_tokenizer_error(p) : 0);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_resize_tokens_array(Parser *p) {
    ଯଦି (p->size > INT_MAX / PEGEN_ARRAY_GROWTH_FACTOR ||
            (size_t)p->size > PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * ଆକାର(*p->tokens))) {
        PyErr_NoMemory();
        ଫେରନ୍ତୁ -1;
    }
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା newsize = p->size * PEGEN_ARRAY_GROWTH_FACTOR;
    Token **new_tokens = PyMem_Realloc(p->tokens, (size_t)newsize * ଆକାର(Token *));
    ଯଦି (new_tokens == ଖାଲି) {
        PyErr_NoMemory();
        ଫେରନ୍ତୁ -1;
    }
    p->tokens = new_tokens;

    // Allocate new tokens together without moving existing ones: parser rules
    // may still hold pointers into earlier blocks. Parser_Free frees each block.
    Token *chunk = PyMem_Calloc((size_t)(newsize - p->size), ଆକାର(Token));
    ଯଦି (chunk == ଖାଲି) {
        PyErr_NoMemory();
        ଫେରନ୍ତୁ -1;
    }
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = p->size; i < newsize; i++) {
        p->tokens[i] = &chunk[i - p->size];
    }
    p->size = newsize;
    ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyPegen_fill_token(Parser *p)
{
    ସଂରଚନା token new_token;
    _PyToken_Init(&new_token);
    _PyTokenizer_Get(p->tok, &new_token);

    // Record and skip '# type: ignore' comments
    ଯେତେବେଳେ (new_token.type == TYPE_IGNORE) {
        Py_ssize_t len;
        ଅଚଳ ଅକ୍ଷର *text = _PyToken_TextView(p->tok, &new_token, &len);
        ଅକ୍ଷର *tag = PyMem_Malloc((size_t)len + 1);
        ଯଦି (tag == ଖାଲି) {
            PyErr_NoMemory();
            ଯାଅ error;
        }
        memcpy(tag, text, (size_t)len);
        tag[len] = '\0';
        // Ownership of tag passes to the growable array
        ଯଦି (!growable_comment_array_add(&p->type_ignore_comments, new_token.end_loc.lineno, tag)) {
            PyMem_Free(tag);
            PyErr_NoMemory();
            ଯାଅ error;
        }
        _PyTokenizer_Get(p->tok, &new_token);
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା type = new_token.type;
    // EOF can finish an ordinary token. Only a boundary token means that
    // the grammar requested input beyond the last physical line.
    ଯଦି (type == ENDMARKER ||
        (type == DEDENT && (p->flags & Py_PARSE_ALLOW_INCOMPLETE_ନିବେଶ) &&
         _PyTokenizer_GetStatus(p->tok) == E_EOF)) {
        p->ନିବେଶ_exhausted = 1;
    }

    // If we have reached the end and we are in single input mode we need to insert a newline and reset the parsing
    ଯଦି (p->start_rule == Py_single_ନିବେଶ && type == ENDMARKER && p->parsing_started) {
        type = NEWLINE; /* Add an extra newline */
        p->parsing_started = 0;

        ଯଦି (!(p->flags & PyPARSE_DONT_IMPLY_DEDENT)) {
            _PyTokenizer_ImplyDedents(p->tok);
        }
    }
    ଅନ୍ୟଥା {
        p->parsing_started = 1;
    }

    // Check if we are at the limit of the token array capacity and resize if needed
    ଯଦି ((p->fill == p->size) && (_resize_tokens_array(p) != 0)) {
        ଯାଅ error;
    }

    Token *t = p->tokens[p->fill];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = initialize_token(p, t, &new_token, type);
    _PyToken_Free(&new_token);
    ଫେରନ୍ତୁ result;
error:
    _PyToken_Free(&new_token);
    ଫେରନ୍ତୁ -1;
}

#ଯଦି defined(Py_DEBUG)
// Instrumentation to count the effectiveness of memoization.
// The array counts the number of tokens skipped by memoization,
// indexed by type.

#define NSTATISTICS _PYPEGEN_NSTATISTICS
#define memo_statistics _PyRuntime.parser.memo_statistics

ଶୂନ୍ୟ
_PyPegen_clear_memo_statistics(ଶୂନ୍ୟ)
{
    PyMutex_Lock(&_PyRuntime.parser.mutex);
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < NSTATISTICS; i++) {
        memo_statistics[i] = 0;
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
}

PyObject *
_PyPegen_get_memo_statistics(ଶୂନ୍ୟ)
{
    PyObject *ret = PyList_New(NSTATISTICS);
    ଯଦି (ret == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    PyMutex_Lock(&_PyRuntime.parser.mutex);
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < NSTATISTICS; i++) {
        PyObject *ମୂଲ୍ୟ = PyLong_FromLong(memo_statistics[i]);
        ଯଦି (ମୂଲ୍ୟ == ଖାଲି) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            ଫେରନ୍ତୁ ଖାଲି;
        }
        // PyList_SetItem borrows a reference to value.
        ଯଦି (PyList_SetItem(ret, i, ମୂଲ୍ୟ) < 0) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            ଫେରନ୍ତୁ ଖାଲି;
        }
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
    ଫେରନ୍ତୁ ret;
}
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା  // bool
_PyPegen_is_memoized(Parser *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, ଶୂନ୍ୟ *pres)
{
    ଯଦି (p->mark == p->fill) {
        ଯଦି (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ଫେରନ୍ତୁ -1;
        }
    }

    Token *t = p->tokens[p->mark];

    ଯଦି (!(t->memo_mask & (1ULL << (type & 63)))) {
        ଫେରନ୍ତୁ 0;
    }

    ପାଇଁ (Memo *m = t->memo; m != ଖାଲି; m = m->next) {
        ଯଦି (m->type == type) {
#ଯଦି defined(Py_DEBUG)
            ଯଦି (0 <= type && type < NSTATISTICS) {
                long ଗଣନା_2 = m->mark - p->mark;
                // A memoized negative result counts for one.
                ଯଦି (ଗଣନା_2 <= 0) {
                    ଗଣନା_2 = 1;
                }
                PyMutex_Lock(&_PyRuntime.parser.mutex);
                memo_statistics[type] += ଗଣନା_2;
                PyMutex_Unlock(&_PyRuntime.parser.mutex);
            }
#endif
            p->mark = m->mark;
            *(ଶୂନ୍ୟ **)(pres) = m->ଗଣ୍ଠି;
            ଫେରନ୍ତୁ 1;
        }
    }
    ଫେରନ୍ତୁ 0;
}

#define LOOKAHEAD1(NAME, RES_TYPE)                                  \
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା                                                             \
    NAME (ପୂର୍ଣ୍ଣସଂଖ୍ୟା positive, RES_TYPE (func)(Parser *), Parser *p)       \
    {                                                               \
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା mark = p->mark;                                         \
        ଶୂନ୍ୟ *res = func(p);                                        \
        p->mark = mark;                                             \
        ଫେରନ୍ତୁ (res != ଖାଲି) == positive;                           \
    }

LOOKAHEAD1(_PyPegen_lookahead, ଶୂନ୍ୟ *)
LOOKAHEAD1(Py_Pegen_lookahead_for_ପଦସମଷ୍ଟି, ପଦସମଷ୍ଟି_ty)
LOOKAHEAD1(_PyPegen_lookahead_for_stmt, stmt_ty)
#undef LOOKAHEAD1

#define LOOKAHEAD2(NAME, RES_TYPE, T)                                   \
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା                                                                 \
    NAME (ପୂର୍ଣ୍ଣସଂଖ୍ୟା positive, RES_TYPE (func)(Parser *, T), Parser *p, T arg) \
    {                                                                   \
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା mark = p->mark;                                             \
        ଶୂନ୍ୟ *res = func(p, arg);                                       \
        p->mark = mark;                                                 \
        ଫେରନ୍ତୁ (res != ଖାଲି) == positive;                               \
    }

LOOKAHEAD2(_PyPegen_lookahead_with_int, Token *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
LOOKAHEAD2(_PyPegen_lookahead_with_string, ପଦସମଷ୍ଟି_ty, ଅଚଳ ଅକ୍ଷର *)
#undef LOOKAHEAD2

Token *
_PyPegen_expect_token(Parser *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type)
{
    ଯଦି (p->mark == p->fill) {
        ଯଦି (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ଫେରନ୍ତୁ ଖାଲି;
        }
    }
    Token *t = p->tokens[p->mark];
    ଯଦି (t->type != type) {
       ଫେରନ୍ତୁ ଖାଲି;
    }
    p->mark += 1;
    ଫେରନ୍ତୁ t;
}

ଶୂନ୍ୟ*
_PyPegen_expect_forced_result(Parser *p, ଶୂନ୍ୟ* result, ଅଚଳ ଅକ୍ଷର* expected) {

    ଯଦି (p->error_indicator == 1) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (result == ଖାଲି) {
        RAISE_SYNTAX_ERROR("expected (%s)", expected);
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ result;
}

Token *
_PyPegen_expect_forced_token(Parser *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, ଅଚଳ ଅକ୍ଷର* expected) {

    ଯଦି (p->error_indicator == 1) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (p->mark == p->fill) {
        ଯଦି (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ଫେରନ୍ତୁ ଖାଲି;
        }
    }
    Token *t = p->tokens[p->mark];
    ଯଦି (t->type != type) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(t, "expected '%s'", expected);
        ଫେରନ୍ତୁ ଖାଲି;
    }
    p->mark += 1;
    ଫେରନ୍ତୁ t;
}

ପଦସମଷ୍ଟି_ty
_PyPegen_expect_soft_keyword(Parser *p, ଅଚଳ ଅକ୍ଷର *keyword)
{
    ଯଦି (p->mark == p->fill) {
        ଯଦି (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ଫେରନ୍ତୁ ଖାଲି;
        }
    }
    Token *t = p->tokens[p->mark];
    ଯଦି (t->type != NAME) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଅଚଳ ଅକ୍ଷର *s = PyBytes_AsString(t->bytes);
    ଯଦି (!s) {
        p->error_indicator = 1;
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (strcmp(s, keyword) != 0) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ _PyPegen_name_token(p);
}

Token *
_PyPegen_get_last_nonnwhitespace_token(Parser *p)
{
    ପ୍ରତିଜ୍ଞା(p->mark >= 0);
    Token *token = ଖାଲି;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା m = p->mark - 1; m >= 0; m--) {
        token = p->tokens[m];
        ଯଦି (token->type != ENDMARKER && (token->type < NEWLINE || token->type > DEDENT)) {
            ଭାଙ୍ଗ;
        }
    }
    ଫେରନ୍ତୁ token;
}

PyObject *
_PyPegen_new_identifier(Parser *p, ଅଚଳ ଅକ୍ଷର *n)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, (Py_ssize_t)strlen(n), ଖାଲି);
    ଯଦି (!id) {
        ଯାଅ error;
    }
    /* Check whether there are non-ASCII characters in the
       identifier; if so, normalize to NFKC. */
    ଯଦି (!PyUnicode_IS_ASCII(id))
    {
        ଯଦି (!init_normalization(p))
        {
            Py_DECREF(id);
            ଯାଅ error;
        }
        PyObject *form = PyUnicode_InternFromString("NFKC");
        ଯଦି (form == ଖାଲି)
        {
            Py_DECREF(id);
            ଯାଅ error;
        }
        PyObject *args[2] = {form, id};
        PyObject *id2 = PyObject_Vectorcall(p->normalize, args, 2, ଖାଲି);
        Py_DECREF(id);
        Py_DECREF(form);
        ଯଦି (!id2) {
            ଯାଅ error;
        }

        ଯଦି (!PyUnicode_Check(id2))
        {
            PyErr_Format(PyExc_TypeError,
                         "unicodedata.normalize() must return a string, not "
                         "%.200s",
                         _PyType_Name(Py_TYPE(id2)));
            Py_DECREF(id2);
            ଯାଅ error;
        }
        id = id2;
    }
    ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ forbidden[] = {
        "None",
        "True",
        "False",
        ଖାଲି
    };
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; forbidden[i] != ଖାଲି; i++) {
        ଯଦି (_PyUnicode_EqualToASCIIString(id, forbidden[i])) {
            PyErr_Format(Py_Exc_ମୂଲ୍ୟ_Error,
                         "identifier field can't represent '%s' constant",
                         forbidden[i]);
            Py_DECREF(id);
            ଯାଅ error;
        }
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &id);
    ଯଦି (_PyArena_AddPyObject(p->arena, id) < 0)
    {
        Py_DECREF(id);
        ଯାଅ error;
    }
    ଫେରନ୍ତୁ id;

error:
    p->error_indicator = 1;
    ଫେରନ୍ତୁ ଖାଲି;
}

// Return an arena-owned identifier; callers borrow the reference.
ସ୍ଥିର PyObject *
get_cached_identifier(Parser *p, PyObject *bytes)
{
    ଅଚଳ ଅକ୍ଷର *s = PyBytes_AsString(bytes);
    ଯଦି (!s) {
        p->error_indicator = 1;
        ଫେରନ୍ତୁ ଖାଲି;
    }
    // Identifiers repeat constantly; a small span-keyed cache skips the
    // UTF-8 decode + intern for repeated occurrences. Keys point into
    // arena-owned token bytes and values are arena-owned interned strings,
    // so borrowed references are valid for the lifetime of the parse
    // (including the second error pass, which reuses parser and arena).
    Py_ssize_t len = PyBytes_GET_SIZE(bytes);
    Py_hash_t hash = PyObject_Hash(bytes);
    ଯଦି (hash == -1) {
        p->error_indicator = 1;
        ଫେରନ୍ତୁ ଖାଲି;
    }
    // Parses without identifiers do not need this cache.
    ଯଦି (p->identifier_cache == ଖାଲି) {
        p->identifier_cache = PyMem_Calloc(
            IDENTIFIER_CACHE_SIZE, ଆକାର(*p->identifier_cache));
        ଯଦି (p->identifier_cache == ଖାଲି) {
            p->error_indicator = 1;
            ଫେରନ୍ତୁ PyErr_NoMemory();
        }
    }
    IdentifierCacheEntry *free_slot = ଖାଲି;
    size_t idx = (size_t)hash & (IDENTIFIER_CACHE_SIZE - 1);
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା probe = 0; probe < IDENTIFIER_CACHE_MAX_PROBES; probe++) {
        IdentifierCacheEntry *entry = &p->identifier_cache[
            (idx + probe) & (IDENTIFIER_CACHE_SIZE - 1)];
        ଯଦି (entry->ଚାବି == ଖାଲି) {
            free_slot = entry;
            ଭାଙ୍ଗ;
        }
        ଯଦି (entry->hash == hash && entry->len == len &&
            memcmp(entry->ଚାବି, s, len) == 0)
        {
            ଫେରନ୍ତୁ entry->ମୂଲ୍ୟ;
        }
    }
    PyObject *id = _PyPegen_new_identifier(p, s);
    ଯଦି (id == ଖାଲି) {
        p->error_indicator = 1;
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (free_slot != ଖାଲି) {
        free_slot->ଚାବି = s;
        free_slot->len = len;
        free_slot->hash = hash;
        free_slot->ମୂଲ୍ୟ = id;
    }
    ଫେରନ୍ତୁ id;
}

ସ୍ଥିର ପଦସମଷ୍ଟି_ty
_PyPegen_name_from_token(Parser *p, Token* t)
{
    ଯଦି (t == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    // Reuse the AST node when backtracking revisits this token. Memo lookup
    // starts before the token and restores the position after it on a hit.
    // Token kinds can be memo keys: generated grammar rule IDs start at 1000.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା mark = p->mark - 1;
    p->mark = mark;
    ପଦସମଷ୍ଟି_ty cached = ଖାଲି;
    ଯଦି (_PyPegen_is_memoized(p, NAME, &cached)) {
        ଫେରନ୍ତୁ cached;
    }
    p->mark = mark + 1;
    PyObject *id = get_cached_identifier(p, t->bytes);
    ଯଦି (id == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ପଦସମଷ୍ଟି_ty result = _PyAST_Name(id, Load, t->lineno, t->col_offset,
                                t->end_lineno, t->end_col_offset, p->arena);
    ଯଦି (result != ଖାଲି && Py_Pegen_ଭର୍ତ୍ତି_memo(p, mark, NAME, result) < 0) {
        p->error_indicator = 1;
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ result;
}

ପଦସମଷ୍ଟି_ty
_PyPegen_name_token(Parser *p)
{
    Token *t = _PyPegen_expect_token(p, NAME);
    ଫେରନ୍ତୁ _PyPegen_name_from_token(p, t);
}

ଶୂନ୍ୟ *
_PyPegen_string_token(Parser *p)
{
    ଫେରନ୍ତୁ _PyPegen_expect_token(p, STRING);
}

ପଦସମଷ୍ଟି_ty _PyPegen_soft_keyword_token(Parser *p) {
    Token *t = _PyPegen_expect_token(p, NAME);
    ଯଦି (t == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଅକ୍ଷର *the_token;
    Py_ssize_t size;
    PyBytes_AsStringAndSize(t->bytes, &the_token, &size);
    ପାଇଁ (ଅକ୍ଷର **keyword = p->soft_keywords; *keyword != ଖାଲି; keyword++) {
        ଯଦି (strlen(*keyword) == (size_t)size &&
            strncmp(*keyword, the_token, (size_t)size) == 0) {
            ଫେରନ୍ତୁ _PyPegen_name_from_token(p, t);
        }
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର PyObject *
parsenumber_raw(ଅଚଳ ଅକ୍ଷର *s)
{
    ଅଚଳ ଅକ୍ଷର *end;
    long x;
    ଦ୍ୱିଗୁଣ dx;
    Py_complex compl;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା imflag;

    ପ୍ରତିଜ୍ଞା(s != ଖାଲି);
    errno = 0;
    end = s + strlen(s) - 1;
    imflag = *end == 'j' || *end == 'J';
    ଯଦି (s[0] == '0') {
        x = (long)PyOS_strtoul(s, (ଅକ୍ଷର **)&end, 0);
        ଯଦି (x < 0 && errno == 0) {
            ଫେରନ୍ତୁ PyLong_FromString(s, (ଅକ୍ଷର **)0, 0);
        }
    }
    ଅନ୍ୟଥା {
        x = PyOS_strtol(s, (ଅକ୍ଷର **)&end, 0);
    }
    ଯଦି (*end == '\0') {
        ଯଦି (errno != 0) {
            ଫେରନ୍ତୁ PyLong_FromString(s, (ଅକ୍ଷର **)0, 0);
        }
        ଫେରନ୍ତୁ PyLong_FromLong(x);
    }
    /* XXX Huge floats may silently fail */
    ଯଦି (imflag) {
        compl.real = 0.;
        compl.imag = PyOS_string_to_double(s, (ଅକ୍ଷର **)&end, ଖାଲି);
        ଯଦି (compl.imag == -1.0 && PyErr_Occurred()) {
            ଫେରନ୍ତୁ ଖାଲି;
        }
        ଫେରନ୍ତୁ PyComplex_FromCComplex(compl);
    }
    dx = PyOS_string_to_double(s, ଖାଲି, ଖାଲି);
    ଯଦି (dx == -1.0 && PyErr_Occurred()) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ PyFloat_FromDouble(dx);
}

ସ୍ଥିର PyObject *
parsenumber(ଅଚଳ ଅକ୍ଷର *s)
{
    ଅକ୍ଷର *dup;
    ଅକ୍ଷର *end;
    PyObject *res = ଖାଲି;

    ପ୍ରତିଜ୍ଞା(s != ଖାଲି);

    ଯଦି (strchr(s, '_') == ଖାଲି) {
        ଫେରନ୍ତୁ parsenumber_raw(s);
    }
    /* Create a duplicate without underscores. */
    dup = PyMem_Malloc(strlen(s) + 1);
    ଯଦି (dup == ଖାଲି) {
        ଫେରନ୍ତୁ PyErr_NoMemory();
    }
    end = dup;
    ପାଇଁ (; *s; s++) {
        ଯଦି (*s != '_') {
            *end++ = *s;
        }
    }
    *end = '\0';
    res = parsenumber_raw(dup);
    PyMem_Free(dup);
    ଫେରନ୍ତୁ res;
}

ପଦସମଷ୍ଟି_ty
Py_Pegen_ସଂଖ୍ୟା_token(Parser *p)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା mark = p->mark;
    Token *t = _PyPegen_expect_token(p, ସଂଖ୍ୟା);
    ଯଦି (t == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    // Look up the node at the token's start, as for NAME tokens. A memo hit
    // advances the mark; on a miss, restore the position after the token.
    p->mark = mark;
    ପଦସମଷ୍ଟି_ty cached = ଖାଲି;
    ଯଦି (_PyPegen_is_memoized(p, ସଂଖ୍ୟା, &cached)) {
        ଫେରନ୍ତୁ cached;
    }
    p->mark = mark + 1;

    ଅଚଳ ଅକ୍ଷର *ଅଙ୍କ_raw = PyBytes_AsString(t->bytes);
    ଯଦି (ଅଙ୍କ_raw == ଖାଲି) {
        p->error_indicator = 1;
        ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (p->feature_version < 6 && strchr(ଅଙ୍କ_raw, '_') != ଖାଲି) {
        p->error_indicator = 1;
        ଫେରନ୍ତୁ RAISE_SYNTAX_ERROR("Underscores in numeric literals are only supported "
                                  "in Python 3.6 and greater");
    }

    PyObject *c = parsenumber(ଅଙ୍କ_raw);

    ଯଦି (c == ଖାଲି) {
        p->error_indicator = 1;
        PyThreadState *tstate = _PyThreadState_GET();
        // The only way a ValueError should happen in _this_ code is via
        // PyLong_FromString hitting a length limit.
        ଯଦି (tstate->current_exception != ଖାଲି &&
            Py_TYPE(tstate->current_exception) == (PyTypeObject *)Py_Exc_ମୂଲ୍ୟ_Error
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
        ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (_PyArena_AddPyObject(p->arena, c) < 0) {
        Py_DECREF(c);
        p->error_indicator = 1;
        ଫେରନ୍ତୁ ଖାଲି;
    }

    ପଦସମଷ୍ଟି_ty result = _PyAST_Constant(c, ଖାଲି, t->lineno, t->col_offset,
                                    t->end_lineno, t->end_col_offset, p->arena);
    ଯଦି (result != ଖାଲି && Py_Pegen_ଭର୍ତ୍ତି_memo(p, mark, ସଂଖ୍ୟା, result) < 0) {
        p->error_indicator = 1;
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ result;
}



ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compute_parser_flags(PyCompilerFlags *flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା parser_flags = 0;
    ଯଦି (!flags) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି (flags->cf_flags & PyCF_DONT_IMPLY_DEDENT) {
        parser_flags |= PyPARSE_DONT_IMPLY_DEDENT;
    }
    ଯଦି (flags->cf_flags & PyCF_IGNORE_COOKIE) {
        parser_flags |= PyPARSE_IGNORE_COOKIE;
    }
    ଯଦି (flags->cf_flags & CO_FUTURE_BARRY_AS_BDFL) {
        parser_flags |= PyPARSE_BARRY_AS_BDFL;
    }
    ଯଦି (flags->cf_flags & PyCF_TYPE_COMMENTS) {
        parser_flags |= PyPARSE_TYPE_COMMENTS;
    }
    ଯଦି (flags->cf_flags & Py_CF_ALLOW_INCOMPLETE_ନିବେଶ) {
        parser_flags |= Py_PARSE_ALLOW_INCOMPLETE_ନିବେଶ;
    }
    ଫେରନ୍ତୁ parser_flags;
}

// Parser API

Parser *
_PyPegen_Parser_New(ସଂରଚନା tok_state *tok, ପୂର୍ଣ୍ଣସଂଖ୍ୟା start_rule, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags,
                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା feature_version, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *errcode, ଅଚଳ ଅକ୍ଷର* source, PyArena *arena)
{
    Parser *p = PyMem_Malloc(ଆକାର(Parser));
    ଯଦି (p == ଖାଲି) {
        ଫେରନ୍ତୁ (Parser *) PyErr_NoMemory();
    }
    ପ୍ରତିଜ୍ଞା(tok != ଖାଲି);
    _PyTokenizer_SetOptions(tok, 0, (flags & PyPARSE_TYPE_COMMENTS) > 0);
    Py_Tokenizer_Allow_Incomplete_ନିବେଶ(tok, flags & Py_PARSE_ALLOW_INCOMPLETE_ନିବେଶ);
    p->tok = tok;
    p->keywords = ଖାଲି;
    p->n_keyword_lists = -1;
    p->soft_keywords = ଖାଲି;
    p->tokens = PyMem_Malloc(ଆକାର(Token *));
    ଯଦି (!p->tokens) {
        PyMem_Free(p);
        ଫେରନ୍ତୁ (Parser *) PyErr_NoMemory();
    }
    p->tokens[0] = PyMem_Calloc(1, ଆକାର(Token));
    ଯଦି (!p->tokens[0]) {
        PyMem_Free(p->tokens);
        PyMem_Free(p);
        ଫେରନ୍ତୁ (Parser *) PyErr_NoMemory();
    }
    p->type_ignore_comments = (growable_comment_array){0};

    p->mark = 0;
    p->fill = 0;
    p->size = 1;

    p->errcode = errcode;
    p->arena = arena;
    p->start_rule = start_rule;
    p->parsing_started = 0;
    p->normalize = ଖାଲି;
    p->error_indicator = 0;

    p->starting_lineno = 0;
    p->starting_col_offset = 0;
    p->flags = flags;
    p->feature_version = feature_version;
    p->known_err_token = ଖାଲି;
    p->identifier_cache = ଖାଲି;
    p->tstate = PyThreadState_Get();
    // Stack limits are initialized when the thread state is attached.
    ପ୍ରତିଜ୍ଞା(((_PyThreadStateImpl *)p->tstate)->c_ଗଦା_hard_limit != 0);
    p->ଗଦା_soft_limit = ((_PyThreadStateImpl *)p->tstate)->c_ଗଦା_soft_limit;
#ଯଦି Py_ଗଦା_GROWS_DOWN
    p->ଗଦା_soft_limit += Py_OS_ଗଦା_MARGIN_BYTES;
#ଅନ୍ୟଥା
    p->ଗଦା_soft_limit -= Py_OS_ଗଦା_MARGIN_BYTES;
#endif
    p->level = 0;
    p->call_invalid_rules = 0;
    p->ନିବେଶ_exhausted = 0;
    p->last_stmt_location.lineno = 0;
    p->last_stmt_location.col_offset = 0;
    p->last_stmt_location.end_lineno = 0;
    p->last_stmt_location.end_col_offset = 0;
#ifdef Py_DEBUG
    p->debug = _Py_GetConfig()->parser_debug;
#endif
    ଫେରନ୍ତୁ p;
}

ଶୂନ୍ୟ
_PyPegen_Parser_Free(Parser *p)
{
    PyMem_Free(p->identifier_cache);
    Py_XDECREF(p->normalize);
    // Resizes allocate blocks starting at indices 1, 2, 4, and so on.
    PyMem_Free(p->tokens[0]);
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < p->size; i *= PEGEN_ARRAY_GROWTH_FACTOR) {
        PyMem_Free(p->tokens[i]);
    }
    PyMem_Free(p->tokens);
    growable_comment_array_deallocate(&p->type_ignore_comments);
    PyMem_Free(p);
}

ସ୍ଥିର ଶୂନ୍ୟ
reset_parser_state_for_error_pass(Parser *p)
{
    p->last_stmt_location.lineno = 0;
    p->last_stmt_location.col_offset = 0;
    p->last_stmt_location.end_lineno = 0;
    p->last_stmt_location.end_col_offset = 0;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < p->fill; i++) {
        p->tokens[i]->memo = ଖାଲି;
        p->tokens[i]->memo_mask = 0;
    }
    p->mark = 0;
    p->flags &= ~Py_PARSE_ALLOW_INCOMPLETE_ନିବେଶ;
    Py_Tokenizer_Allow_Incomplete_ନିବେଶ(p->tok, 0);
    p->call_invalid_rules = 1;
    _PyTokenizer_StopInteractive(p->tok);
}

ସ୍ଥିର ଶୂନ୍ୟ
_PyPegen_set_syntax_error_metadata(Parser *p) {
    PyObject *exc = PyErr_GetRaisedException();
    ଯଦି (!exc || !PyObject_TypeCheck(exc, (PyTypeObject *)PyExc_SyntaxError)) {
        PyErr_SetRaisedException(exc);
        ଫେରନ୍ତୁ;
    }
    ଅଚଳ ଅକ୍ଷର *source = _PyTokenizer_RetainedSource(p->tok);
    ଅଚଳ ଅକ୍ଷର *encoding = _PyTokenizer_GetInfo(p->tok).encoding;
    PyObject* the_source = ଖାଲି;
    ଯଦି (source) {
        ଯଦି (encoding == ଖାଲି) {
            the_source = PyUnicode_FromString(source);
        } ଅନ୍ୟଥା {
            the_source = PyUnicode_Decode(source, strlen(source), encoding, ଖାଲି);
        }
    }
    ଯଦି (!the_source) {
        PyErr_Clear();
        the_source = Py_None;
        Py_INCREF(the_source);
    }
    PyObject* metadata = Py_Build_ମୂଲ୍ୟ(
        "(iiN)",
        p->last_stmt_location.lineno,
        p->last_stmt_location.col_offset,
        the_source // N gives ownership to metadata
    );
    ଯଦି (!metadata) {
        PyErr_Clear();
        PyErr_SetRaisedException(exc);
        ଫେରନ୍ତୁ;
    }
    PySyntaxErrorObject *syntax_error = (PySyntaxErrorObject *)exc;

    Py_XDECREF(syntax_error->metadata);
    syntax_error->metadata = metadata;
    PyErr_SetRaisedException(exc);
}

ଶୂନ୍ୟ *
_PyPegen_run_parser(Parser *p)
{
    ଶୂନ୍ୟ *res = _PyPegen_parse(p);
    ପ୍ରତିଜ୍ଞା(p->level == 0);
    ଯଦି (res != ଖାଲି && PyErr_Occurred()) {
        // Discard a result returned with an exception still pending
        // (e.g. a MemoryError from a recovered-from allocation failure).
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (res == ଖାଲି) {
        ଯଦି (PyErr_Occurred() &&
            (PyErr_ExceptionMatches(Py_Exc_Incomplete_ନିବେଶ_Error) ||
             !PyErr_ExceptionMatches(PyExc_SyntaxError))) {
            ଫେରନ୍ତୁ ଖାଲି;
        }
        ଯଦି ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_ନିବେଶ) &&
            p->ନିବେଶ_exhausted && !PyErr_Occurred()) {
            ଫେରନ୍ତୁ Py_Pegen_raise_incomplete_ନିବେଶ(p);
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
        ଯଦି (PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_set_syntax_error_metadata(p);
        }
       ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (p->start_rule == Py_single_ନିବେଶ && _PyTokenizer_HasTrailingStatement(p->tok)) {
        ଫେରନ୍ତୁ RAISE_SYNTAX_ERROR("multiple statements found while compiling a single statement");
    }

    // test_peg_generator defines _Py_TEST_PEGEN to not call PyAST_Validate()
#ଯଦି defined(Py_DEBUG) && !defined(_Py_TEST_PEGEN)
    ଯଦି (p->start_rule == Py_single_ନିବେଶ ||
        p->start_rule == Py_file_ନିବେଶ ||
        p->start_rule == Py_eval_ନିବେଶ)
    {
        ଯଦି (!_PyAST_Validate(res)) {
            ଫେରନ୍ତୁ ଖାଲି;
        }
    }
#endif
    ଫେରନ୍ତୁ res;
}

mod_ty
_PyPegen_run_parser_from_file_pointer(FILE *fp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା start_rule, PyObject *filename_ob,
                             ଅଚଳ ଅକ୍ଷର *enc, ଅଚଳ ଅକ୍ଷର *ps1, ଅଚଳ ଅକ୍ଷର *ps2,
                             PyCompilerFlags *flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *errcode,
                             PyObject **interactive_src, PyArena *arena)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା parser_flags = compute_parser_flags(flags);
    ସଂରଚନା tok_state *tok = _PyTokenizer_FromFile(fp, enc, ps1, ps2);
    ଯଦି (tok == ଖାଲି) {
        ଯଦି (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        ଅନ୍ୟଥା {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        ଫେରନ୍ତୁ ଖାଲି;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    // From here on we need to clean up even if there's an error
    mod_ty result = ଖାଲି;

    PyObject *module = PyUnicode_FromString("__main__");
    ଯଦି (module == ଖାଲି) {
        ଯାଅ error;
    }
    _PyTokenizer_SetContext(tok, filename_ob, module);
    Py_DECREF(module);

    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, PY_MINOR_VERSION,
                                    errcode, ଖାଲି, arena);
    ଯଦି (p == ଖାଲି) {
        ଯାଅ error;
    }

    result = _PyPegen_run_parser(p);
    _PyPegen_Parser_Free(p);

    ଅଚଳ ଅକ୍ଷର *source = _PyTokenizer_RetainedSource(tok);
    ଯଦି (source != ଖାଲି && result && interactive_src != ଖାଲି) {
        *interactive_src = PyUnicode_FromString(source);
        ଯଦି (!*interactive_src || _PyArena_AddPyObject(arena, *interactive_src) < 0) {
            Py_XDECREF(*interactive_src);
            result = ଖାଲି;
            ଯାଅ error;
        }
    }

error:
    _PyTokenizer_Free(tok);
    ଫେରନ୍ତୁ result;
}

mod_ty
_PyPegen_run_parser_from_string(ଅଚଳ ଅକ୍ଷର *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା start_rule, PyObject *filename_ob,
                       PyCompilerFlags *flags, PyArena *arena, PyObject *module,
                       PyObject **incomplete)
{
    _PyTokenizer_Newline newline_mode = start_rule == Py_file_ନିବେଶ
        ? _PyTokenizer_IMPLY_NEWLINE : _PyTokenizer_KEEP_NEWLINE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା parser_flags = compute_parser_flags(flags);
    ଯଦି (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_ନିବେଶ) {
        newline_mode = _PyTokenizer_APPEND_NEWLINE;
    }

    ସଂରଚନା tok_state *tok;
    ଯଦି (flags != ଖାଲି && flags->cf_flags & PyCF_IGNORE_COOKIE) {
        tok = _PyTokenizer_FromUTF8(str, newline_mode, 0);
    } ଅନ୍ୟଥା {
        tok = _PyTokenizer_FromString(str, newline_mode, 0);
    }
    ଯଦି (tok == ଖାଲି) {
        ଯଦି (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        ଅନ୍ୟଥା {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        ଫେରନ୍ତୁ ଖାଲି;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    _PyTokenizer_SetContext(tok, filename_ob, module);

    // We need to clear up from here on
    mod_ty result = ଖାଲି;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା feature_version = flags && (flags->cf_flags & PyCF_ONLY_AST) ?
        flags->cf_feature_version : PY_MINOR_VERSION;
    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, feature_version,
                                    ଖାଲି, str, arena);
    ଯଦି (p == ଖାଲି) {
        ଯାଅ error;
    }

    result = _PyPegen_run_parser(p);
    ଯଦି (result != ଖାଲି && (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_ନିବେଶ)) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା needs_newline = _PyTokenizer_GetNewlineRequirements(tok);
        ଯଦି ((needs_newline & _PyTokenizer_NEWLINE_CONTINUATION) ||
            (start_rule != Py_file_ନିବେଶ &&
             (needs_newline & _PyTokenizer_NEWLINE_INDENT)) ||
            (start_rule == Py_single_ନିବେଶ && p->ନିବେଶ_exhausted &&
             (needs_newline & _PyTokenizer_NEWLINE_STATEMENT))) {
            Py_Pegen_raise_incomplete_ନିବେଶ(p);
            ଯଦି (incomplete != ଖାଲି && PyErr_ExceptionMatches(Py_Exc_Incomplete_ନିବେଶ_Error)) {
                *incomplete = PyErr_GetRaisedException();
            }
            ଅନ୍ୟଥା {
                result = ଖାଲି;
            }
        }
    }
    _PyPegen_Parser_Free(p);

error:
    _PyTokenizer_Free(tok);
    ଫେରନ୍ତୁ result;
}
