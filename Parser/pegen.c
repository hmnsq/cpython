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
    स्थिरांक अक्षर *किल्ली;   // Borrowed from arena-owned token bytes.
    Py_ssize_t len;
    Py_hash_t hash;
    PyObject *मूल्य;   // Borrowed from an arena-owned identifier.
};

// Internal parser functions

asdl_stmt_seq*
_PyPegen_interactive_exit(Parser *p)
{
    जर (p->errcode) {
        *(p->errcode) = E_EOF;
    }
    परत रिक्त;
}

Py_ssize_t
Py_Pegen_byte_offset_to_character_offset_ओळ(PyObject *ओळ, Py_ssize_t col_offset, Py_ssize_t end_col_offset)
{
    स्थिरांक अचिन्हित अक्षर *data = (स्थिरांक अचिन्हित अक्षर*)PyUnicode_AsUTF8(ओळ);
    जर (data == रिक्त) {
        परत -1;
    }

    Py_ssize_t len = 0;
    पर्यंत (col_offset < end_col_offset) {
        Py_UCS4 ch = data[col_offset];
        जर (ch < 0x80) {
            col_offset += 1;
        } अन्यथा जर ((ch & 0xe0) == 0xc0) {
            col_offset += 2;
        } अन्यथा जर ((ch & 0xf0) == 0xe0) {
            col_offset += 3;
        } अन्यथा जर ((ch & 0xf8) == 0xf0) {
            col_offset += 4;
        } अन्यथा {
            PyErr_SetString(Py_Exc_मूल्य_Error, "Invalid UTF-8 sequence");
            परत -1;
        }
        len++;
    }
    परत len;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset_raw(स्थिरांक अक्षर* str, Py_ssize_t col_offset)
{
    Py_ssize_t len = (Py_ssize_t)म_माप(str);
    जर (col_offset > len + 1) {
        col_offset = len + 1;
    }
    प्रतिज्ञा(col_offset >= 0);
    PyObject *text = PyUnicode_DecodeUTF8(str, col_offset, "replace");
    जर (!text) {
        परत -1;
    }
    Py_ssize_t size = PyUnicode_GET_LENGTH(text);
    Py_DECREF(text);
    परत size;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset(PyObject *ओळ, Py_ssize_t col_offset)
{
    स्थिरांक अक्षर *str = PyUnicode_AsUTF8(ओळ);
    जर (!str) {
        परत -1;
    }
    परत _PyPegen_byte_offset_to_character_offset_raw(str, col_offset);
}

// Here, mark is the start of the node, while p->mark is the end.
// If node==NULL, they should be the same.
पूर्णांक
Py_Pegen_घाला_memo(Parser *p, पूर्णांक mark, पूर्णांक type, शून्य *गाठ)
{
    // Insert in front
    Memo *m = _PyArena_Malloc(p->arena, आकार(Memo));
    जर (m == रिक्त) {
        परत -1;
    }
    m->type = type;
    m->गाठ = गाठ;
    m->mark = p->mark;
    m->next = p->tokens[mark]->memo;
    p->tokens[mark]->memo = m;
    p->tokens[mark]->memo_mask |= 1ULL << (type & 63);
    परत 0;
}

// Like _PyPegen_insert_memo(), but returns the inserted Memo so callers
// can update it in place without re-walking the token's memo list.
Memo *
Py_Pegen_घाला_memo_direct(Parser *p, पूर्णांक mark, पूर्णांक type)
{
    जर (Py_Pegen_घाला_memo(p, mark, type, रिक्त) < 0) {
        परत रिक्त;
    }
    परत p->tokens[mark]->memo;
}

// Like _PyPegen_insert_memo(), but updates an existing node if found.
पूर्णांक
_PyPegen_update_memo(Parser *p, पूर्णांक mark, पूर्णांक type, शून्य *गाठ)
{
    साठी (Memo *m = p->tokens[mark]->memo; m != रिक्त; m = m->next) {
        जर (m->type == type) {
            // Update existing node.
            m->गाठ = गाठ;
            m->mark = p->mark;
            परत 0;
        }
    }
    // Insert new node.
    परत Py_Pegen_घाला_memo(p, mark, type, गाठ);
}

स्थिर पूर्णांक
init_normalization(Parser *p)
{
    जर (p->normalize) {
        परत 1;
    }
    p->normalize = PyImport_ImportModuleAttrString("unicodedata", "normalize");
    जर (!p->normalize)
    {
        परत 0;
    }
    परत 1;
}

पूर्णांक
_PyPegen_grow_loop_buffer(शून्य ***buffer, Py_ssize_t *capacity)
{
    जर ((माप_प्रकार)*capacity > (माप_प्रकार)PY_SSIZE_T_MAX /
            (PEGEN_ARRAY_GROWTH_FACTOR * आकार(**buffer))) {
        परत -1;
    }
    Py_ssize_t new_capacity = *capacity == 0
        ? 1 : *capacity * PEGEN_ARRAY_GROWTH_FACTOR;
    शून्य **new_buffer = PyMem_Realloc(*buffer, new_capacity * आकार(**buffer));
    जर (new_buffer == रिक्त) {
        परत -1;
    }
    *buffer = new_buffer;
    *capacity = new_capacity;
    परत 0;
}

स्थिर पूर्णांक
growable_comment_array_add(growable_comment_array *arr, पूर्णांक lineno, अक्षर *comment)
{
    जर (arr->अंक_वस्तुसंच >= arr->size) {
        जर (arr->size > (माप_प्रकार)PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * आकार(*arr->वस्तुसंच))) {
            परत 0;
        }
        माप_प्रकार new_size = arr->size == 0
            ? TYPE_IGNORE_INITIAL_CAPACITY
            : arr->size * PEGEN_ARRAY_GROWTH_FACTOR;
        शून्य *new_वस्तुसंच_array = PyMem_Realloc(
            arr->वस्तुसंच, new_size * आकार(*arr->वस्तुसंच));
        जर (!new_वस्तुसंच_array) {
            परत 0;
        }
        arr->वस्तुसंच = new_वस्तुसंच_array;
        arr->size = new_size;
    }

    arr->वस्तुसंच[arr->अंक_वस्तुसंच].lineno = lineno;
    arr->वस्तुसंच[arr->अंक_वस्तुसंच].comment = comment;  // Take ownership
    arr->अंक_वस्तुसंच++;
    परत 1;
}

स्थिर शून्य
growable_comment_array_deallocate(growable_comment_array *arr)
{
    साठी (माप_प्रकार i = 0; i < arr->अंक_वस्तुसंच; i++) {
        PyMem_Free(arr->वस्तुसंच[i].comment);
    }
    PyMem_Free(arr->वस्तुसंच);
}

स्थिर पूर्णांक
_get_keyword_or_name_type(Parser *p, स्थिरांक अक्षर *text, Py_ssize_t length)
{
    प्रतिज्ञा(length > 0);

    जर (length >= p->n_keyword_lists ||
        p->keywords[length] == रिक्त ||
        p->keywords[length]->type == -1) {
        परत NAME;
    }
    साठी (KeywordToken *k = p->keywords[length]; k != रिक्त && k->type != -1; k++) {
        जर (स_भेद(k->str, text, (माप_प्रकार)length) == 0) {
            परत k->type;
        }
    }
    परत NAME;
}

// Keep text only where grammar actions or helpers read it. Keyword error
// actions use fixed spellings, so keywords do not need their text.
स्थिर ओळीत पूर्णांक
token_needs_text(पूर्णांक type)
{
    निवड (type) {
        स्थिती NAME:
        स्थिती संख्या:
        स्थिती STRING:
        स्थिती FSTRING_START:
        स्थिती FSTRING_मधला:
        स्थिती FSTRING_END:
        स्थिती TSTRING_START:
        स्थिती TSTRING_मधला:
        स्थिती TSTRING_END:
        स्थिती TYPE_COMMENT:
        स्थिती NOTEQUAL:  // _PyPegen_check_barry_as_flufl() distinguishes != and <>.
            परत 1;
        पूर्वनिर्धारित:
            परत 0;
    }
}

स्थिर पूर्णांक
initialize_token(Parser *p, Token *parser_token, संरचना token *new_token, पूर्णांक token_type) {
    प्रतिज्ञा(parser_token != रिक्त);

    parser_token->type = token_type;
    parser_token->bytes = रिक्त;
    जर (token_needs_text(token_type) && new_token->span.start >= 0) {
        Py_ssize_t length;
        स्थिरांक अक्षर *text = _PyToken_TextView(p->tok, new_token, &length);
        जर (token_type == NAME) {
            parser_token->type = _get_keyword_or_name_type(p, text, length);
        }
        // A name reclassified as a keyword does not need its text.
        जर (parser_token->type == token_type) {
            parser_token->bytes = PyBytes_FromStringAndSize(text, length);
            जर (parser_token->bytes == रिक्त) {
                परत -1;
            }
            जर (_PyArena_AddPyObject(p->arena, parser_token->bytes) < 0) {
                Py_DECREF(parser_token->bytes);
                परत -1;
            }
        }
    }

    parser_token->metadata = रिक्त;
    जर (new_token->metadata != रिक्त) {
        जर (_PyArena_AddPyObject(p->arena, new_token->metadata) < 0) {
            परत -1;
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

    जर (token_type == ERRORTOKEN && _PyTokenizer_GetStatus(p->tok) == E_DECODE) {
        परत _Pypegen_raise_decode_error(p);
    }

    परत (token_type == ERRORTOKEN ? _Pypegen_tokenizer_error(p) : 0);
}

स्थिर पूर्णांक
_resize_tokens_array(Parser *p) {
    जर (p->size > पूर्णांक_उच्च / PEGEN_ARRAY_GROWTH_FACTOR ||
            (माप_प्रकार)p->size > PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * आकार(*p->tokens))) {
        PyErr_NoMemory();
        परत -1;
    }
    पूर्णांक newsize = p->size * PEGEN_ARRAY_GROWTH_FACTOR;
    Token **new_tokens = PyMem_Realloc(p->tokens, (माप_प्रकार)newsize * आकार(Token *));
    जर (new_tokens == रिक्त) {
        PyErr_NoMemory();
        परत -1;
    }
    p->tokens = new_tokens;

    // Allocate new tokens together without moving existing ones: parser rules
    // may still hold pointers into earlier blocks. Parser_Free frees each block.
    Token *chunk = PyMem_Calloc((माप_प्रकार)(newsize - p->size), आकार(Token));
    जर (chunk == रिक्त) {
        PyErr_NoMemory();
        परत -1;
    }
    साठी (पूर्णांक i = p->size; i < newsize; i++) {
        p->tokens[i] = &chunk[i - p->size];
    }
    p->size = newsize;
    परत 0;
}

पूर्णांक
_PyPegen_fill_token(Parser *p)
{
    संरचना token new_token;
    _PyToken_Init(&new_token);
    _PyTokenizer_Get(p->tok, &new_token);

    // Record and skip '# type: ignore' comments
    पर्यंत (new_token.type == TYPE_IGNORE) {
        Py_ssize_t len;
        स्थिरांक अक्षर *text = _PyToken_TextView(p->tok, &new_token, &len);
        अक्षर *tag = PyMem_Malloc((माप_प्रकार)len + 1);
        जर (tag == रिक्त) {
            PyErr_NoMemory();
            जा error;
        }
        स_नकल(tag, text, (माप_प्रकार)len);
        tag[len] = '\0';
        // Ownership of tag passes to the growable array
        जर (!growable_comment_array_add(&p->type_ignore_comments, new_token.end_loc.lineno, tag)) {
            PyMem_Free(tag);
            PyErr_NoMemory();
            जा error;
        }
        _PyTokenizer_Get(p->tok, &new_token);
    }

    पूर्णांक type = new_token.type;
    // EOF can finish an ordinary token. Only a boundary token means that
    // the grammar requested input beyond the last physical line.
    जर (type == ENDMARKER ||
        (type == DEDENT && (p->flags & Py_PARSE_ALLOW_INCOMPLETE_आदान) &&
         _PyTokenizer_GetStatus(p->tok) == E_EOF)) {
        p->आदान_exhausted = 1;
    }

    // If we have reached the end and we are in single input mode we need to insert a newline and reset the parsing
    जर (p->start_rule == Py_single_आदान && type == ENDMARKER && p->parsing_started) {
        type = NEWLINE; /* Add an extra newline */
        p->parsing_started = 0;

        जर (!(p->flags & PyPARSE_DONT_IMPLY_DEDENT)) {
            _PyTokenizer_ImplyDedents(p->tok);
        }
    }
    अन्यथा {
        p->parsing_started = 1;
    }

    // Check if we are at the limit of the token array capacity and resize if needed
    जर ((p->fill == p->size) && (_resize_tokens_array(p) != 0)) {
        जा error;
    }

    Token *t = p->tokens[p->fill];
    पूर्णांक result = initialize_token(p, t, &new_token, type);
    _PyToken_Free(&new_token);
    परत result;
error:
    _PyToken_Free(&new_token);
    परत -1;
}

#जर defined(Py_DEBUG)
// Instrumentation to count the effectiveness of memoization.
// The array counts the number of tokens skipped by memoization,
// indexed by type.

#define NSTATISTICS _PYPEGEN_NSTATISTICS
#define memo_statistics _PyRuntime.parser.memo_statistics

शून्य
_PyPegen_clear_memo_statistics(शून्य)
{
    PyMutex_Lock(&_PyRuntime.parser.mutex);
    साठी (पूर्णांक i = 0; i < NSTATISTICS; i++) {
        memo_statistics[i] = 0;
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
}

PyObject *
_PyPegen_get_memo_statistics(शून्य)
{
    PyObject *ret = PyList_New(NSTATISTICS);
    जर (ret == रिक्त) {
        परत रिक्त;
    }

    PyMutex_Lock(&_PyRuntime.parser.mutex);
    साठी (पूर्णांक i = 0; i < NSTATISTICS; i++) {
        PyObject *मूल्य = PyLong_FromLong(memo_statistics[i]);
        जर (मूल्य == रिक्त) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            परत रिक्त;
        }
        // PyList_SetItem borrows a reference to value.
        जर (PyList_SetItem(ret, i, मूल्य) < 0) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            परत रिक्त;
        }
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
    परत ret;
}
#endif

पूर्णांक  // bool
_PyPegen_is_memoized(Parser *p, पूर्णांक type, शून्य *pres)
{
    जर (p->mark == p->fill) {
        जर (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            परत -1;
        }
    }

    Token *t = p->tokens[p->mark];

    जर (!(t->memo_mask & (1ULL << (type & 63)))) {
        परत 0;
    }

    साठी (Memo *m = t->memo; m != रिक्त; m = m->next) {
        जर (m->type == type) {
#जर defined(Py_DEBUG)
            जर (0 <= type && type < NSTATISTICS) {
                दीर्घ मोजणी = m->mark - p->mark;
                // A memoized negative result counts for one.
                जर (मोजणी <= 0) {
                    मोजणी = 1;
                }
                PyMutex_Lock(&_PyRuntime.parser.mutex);
                memo_statistics[type] += मोजणी;
                PyMutex_Unlock(&_PyRuntime.parser.mutex);
            }
#endif
            p->mark = m->mark;
            *(शून्य **)(pres) = m->गाठ;
            परत 1;
        }
    }
    परत 0;
}

#define LOOKAHEAD1(NAME, RES_TYPE)                                  \
    पूर्णांक                                                             \
    NAME (पूर्णांक positive, RES_TYPE (func)(Parser *), Parser *p)       \
    {                                                               \
        पूर्णांक mark = p->mark;                                         \
        शून्य *res = func(p);                                        \
        p->mark = mark;                                             \
        परत (res != रिक्त) == positive;                           \
    }

LOOKAHEAD1(_PyPegen_lookahead, शून्य *)
LOOKAHEAD1(Py_Pegen_lookahead_for_अभिव्यक्ती, अभिव्यक्ती_ty)
LOOKAHEAD1(_PyPegen_lookahead_for_stmt, stmt_ty)
#undef LOOKAHEAD1

#define LOOKAHEAD2(NAME, RES_TYPE, T)                                   \
    पूर्णांक                                                                 \
    NAME (पूर्णांक positive, RES_TYPE (func)(Parser *, T), Parser *p, T arg) \
    {                                                                   \
        पूर्णांक mark = p->mark;                                             \
        शून्य *res = func(p, arg);                                       \
        p->mark = mark;                                                 \
        परत (res != रिक्त) == positive;                               \
    }

LOOKAHEAD2(_PyPegen_lookahead_with_int, Token *, पूर्णांक)
LOOKAHEAD2(_PyPegen_lookahead_with_string, अभिव्यक्ती_ty, स्थिरांक अक्षर *)
#undef LOOKAHEAD2

Token *
_PyPegen_expect_token(Parser *p, पूर्णांक type)
{
    जर (p->mark == p->fill) {
        जर (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            परत रिक्त;
        }
    }
    Token *t = p->tokens[p->mark];
    जर (t->type != type) {
       परत रिक्त;
    }
    p->mark += 1;
    परत t;
}

शून्य*
_PyPegen_expect_forced_result(Parser *p, शून्य* result, स्थिरांक अक्षर* expected) {

    जर (p->error_indicator == 1) {
        परत रिक्त;
    }
    जर (result == रिक्त) {
        RAISE_SYNTAX_ERROR("expected (%s)", expected);
        परत रिक्त;
    }
    परत result;
}

Token *
_PyPegen_expect_forced_token(Parser *p, पूर्णांक type, स्थिरांक अक्षर* expected) {

    जर (p->error_indicator == 1) {
        परत रिक्त;
    }

    जर (p->mark == p->fill) {
        जर (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            परत रिक्त;
        }
    }
    Token *t = p->tokens[p->mark];
    जर (t->type != type) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(t, "expected '%s'", expected);
        परत रिक्त;
    }
    p->mark += 1;
    परत t;
}

अभिव्यक्ती_ty
_PyPegen_expect_soft_keyword(Parser *p, स्थिरांक अक्षर *keyword)
{
    जर (p->mark == p->fill) {
        जर (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            परत रिक्त;
        }
    }
    Token *t = p->tokens[p->mark];
    जर (t->type != NAME) {
        परत रिक्त;
    }
    स्थिरांक अक्षर *s = PyBytes_AsString(t->bytes);
    जर (!s) {
        p->error_indicator = 1;
        परत रिक्त;
    }
    जर (म_भेद(s, keyword) != 0) {
        परत रिक्त;
    }
    परत _PyPegen_name_token(p);
}

Token *
_PyPegen_get_last_nonnwhitespace_token(Parser *p)
{
    प्रतिज्ञा(p->mark >= 0);
    Token *token = रिक्त;
    साठी (पूर्णांक m = p->mark - 1; m >= 0; m--) {
        token = p->tokens[m];
        जर (token->type != ENDMARKER && (token->type < NEWLINE || token->type > DEDENT)) {
            थांबा;
        }
    }
    परत token;
}

PyObject *
_PyPegen_new_identifier(Parser *p, स्थिरांक अक्षर *n)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, (Py_ssize_t)म_माप(n), रिक्त);
    जर (!id) {
        जा error;
    }
    /* Check whether there are non-ASCII characters in the
       identifier; if so, normalize to NFKC. */
    जर (!PyUnicode_IS_ASCII(id))
    {
        जर (!init_normalization(p))
        {
            Py_DECREF(id);
            जा error;
        }
        PyObject *form = PyUnicode_InternFromString("NFKC");
        जर (form == रिक्त)
        {
            Py_DECREF(id);
            जा error;
        }
        PyObject *args[2] = {form, id};
        PyObject *id2 = PyObject_Vectorcall(p->normalize, args, 2, रिक्त);
        Py_DECREF(id);
        Py_DECREF(form);
        जर (!id2) {
            जा error;
        }

        जर (!PyUnicode_Check(id2))
        {
            PyErr_Format(PyExc_TypeError,
                         "unicodedata.normalize() must return a string, not "
                         "%.200s",
                         _PyType_Name(Py_TYPE(id2)));
            Py_DECREF(id2);
            जा error;
        }
        id = id2;
    }
    स्थिर स्थिरांक अक्षर * स्थिरांक forbidden[] = {
        "None",
        "True",
        "False",
        रिक्त
    };
    साठी (पूर्णांक i = 0; forbidden[i] != रिक्त; i++) {
        जर (_PyUnicode_EqualToASCIIString(id, forbidden[i])) {
            PyErr_Format(Py_Exc_मूल्य_Error,
                         "identifier field can't represent '%s' constant",
                         forbidden[i]);
            Py_DECREF(id);
            जा error;
        }
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &id);
    जर (_PyArena_AddPyObject(p->arena, id) < 0)
    {
        Py_DECREF(id);
        जा error;
    }
    परत id;

error:
    p->error_indicator = 1;
    परत रिक्त;
}

// Return an arena-owned identifier; callers borrow the reference.
स्थिर PyObject *
get_cached_identifier(Parser *p, PyObject *bytes)
{
    स्थिरांक अक्षर *s = PyBytes_AsString(bytes);
    जर (!s) {
        p->error_indicator = 1;
        परत रिक्त;
    }
    // Identifiers repeat constantly; a small span-keyed cache skips the
    // UTF-8 decode + intern for repeated occurrences. Keys point into
    // arena-owned token bytes and values are arena-owned interned strings,
    // so borrowed references are valid for the lifetime of the parse
    // (including the second error pass, which reuses parser and arena).
    Py_ssize_t len = PyBytes_GET_SIZE(bytes);
    Py_hash_t hash = PyObject_Hash(bytes);
    जर (hash == -1) {
        p->error_indicator = 1;
        परत रिक्त;
    }
    // Parses without identifiers do not need this cache.
    जर (p->identifier_cache == रिक्त) {
        p->identifier_cache = PyMem_Calloc(
            IDENTIFIER_CACHE_SIZE, आकार(*p->identifier_cache));
        जर (p->identifier_cache == रिक्त) {
            p->error_indicator = 1;
            परत PyErr_NoMemory();
        }
    }
    IdentifierCacheEntry *free_slot = रिक्त;
    माप_प्रकार idx = (माप_प्रकार)hash & (IDENTIFIER_CACHE_SIZE - 1);
    साठी (पूर्णांक probe = 0; probe < IDENTIFIER_CACHE_MAX_PROBES; probe++) {
        IdentifierCacheEntry *entry = &p->identifier_cache[
            (idx + probe) & (IDENTIFIER_CACHE_SIZE - 1)];
        जर (entry->किल्ली == रिक्त) {
            free_slot = entry;
            थांबा;
        }
        जर (entry->hash == hash && entry->len == len &&
            स_भेद(entry->किल्ली, s, len) == 0)
        {
            परत entry->मूल्य;
        }
    }
    PyObject *id = _PyPegen_new_identifier(p, s);
    जर (id == रिक्त) {
        p->error_indicator = 1;
        परत रिक्त;
    }
    जर (free_slot != रिक्त) {
        free_slot->किल्ली = s;
        free_slot->len = len;
        free_slot->hash = hash;
        free_slot->मूल्य = id;
    }
    परत id;
}

स्थिर अभिव्यक्ती_ty
_PyPegen_name_from_token(Parser *p, Token* t)
{
    जर (t == रिक्त) {
        परत रिक्त;
    }
    // Reuse the AST node when backtracking revisits this token. Memo lookup
    // starts before the token and restores the position after it on a hit.
    // Token kinds can be memo keys: generated grammar rule IDs start at 1000.
    पूर्णांक mark = p->mark - 1;
    p->mark = mark;
    अभिव्यक्ती_ty cached = रिक्त;
    जर (_PyPegen_is_memoized(p, NAME, &cached)) {
        परत cached;
    }
    p->mark = mark + 1;
    PyObject *id = get_cached_identifier(p, t->bytes);
    जर (id == रिक्त) {
        परत रिक्त;
    }
    अभिव्यक्ती_ty result = _PyAST_Name(id, Load, t->lineno, t->col_offset,
                                t->end_lineno, t->end_col_offset, p->arena);
    जर (result != रिक्त && Py_Pegen_घाला_memo(p, mark, NAME, result) < 0) {
        p->error_indicator = 1;
        परत रिक्त;
    }
    परत result;
}

अभिव्यक्ती_ty
_PyPegen_name_token(Parser *p)
{
    Token *t = _PyPegen_expect_token(p, NAME);
    परत _PyPegen_name_from_token(p, t);
}

शून्य *
_PyPegen_string_token(Parser *p)
{
    परत _PyPegen_expect_token(p, STRING);
}

अभिव्यक्ती_ty _PyPegen_soft_keyword_token(Parser *p) {
    Token *t = _PyPegen_expect_token(p, NAME);
    जर (t == रिक्त) {
        परत रिक्त;
    }
    अक्षर *the_token;
    Py_ssize_t size;
    PyBytes_AsStringAndSize(t->bytes, &the_token, &size);
    साठी (अक्षर **keyword = p->soft_keywords; *keyword != रिक्त; keyword++) {
        जर (म_माप(*keyword) == (माप_प्रकार)size &&
            म_भेदन(*keyword, the_token, (माप_प्रकार)size) == 0) {
            परत _PyPegen_name_from_token(p, t);
        }
    }
    परत रिक्त;
}

स्थिर PyObject *
parsenumber_raw(स्थिरांक अक्षर *s)
{
    स्थिरांक अक्षर *end;
    दीर्घ x;
    द्विगुण dx;
    Py_complex compl;
    पूर्णांक imflag;

    प्रतिज्ञा(s != रिक्त);
    त्रुटि_सं = 0;
    end = s + म_माप(s) - 1;
    imflag = *end == 'j' || *end == 'J';
    जर (s[0] == '0') {
        x = (दीर्घ)PyOS_strtoul(s, (अक्षर **)&end, 0);
        जर (x < 0 && त्रुटि_सं == 0) {
            परत PyLong_FromString(s, (अक्षर **)0, 0);
        }
    }
    अन्यथा {
        x = PyOS_strtol(s, (अक्षर **)&end, 0);
    }
    जर (*end == '\0') {
        जर (त्रुटि_सं != 0) {
            परत PyLong_FromString(s, (अक्षर **)0, 0);
        }
        परत PyLong_FromLong(x);
    }
    /* XXX Huge floats may silently fail */
    जर (imflag) {
        compl.real = 0.;
        compl.imag = PyOS_string_to_double(s, (अक्षर **)&end, रिक्त);
        जर (compl.imag == -1.0 && PyErr_Occurred()) {
            परत रिक्त;
        }
        परत PyComplex_FromCComplex(compl);
    }
    dx = PyOS_string_to_double(s, रिक्त, रिक्त);
    जर (dx == -1.0 && PyErr_Occurred()) {
        परत रिक्त;
    }
    परत PyFloat_FromDouble(dx);
}

स्थिर PyObject *
parsenumber(स्थिरांक अक्षर *s)
{
    अक्षर *dup;
    अक्षर *end;
    PyObject *res = रिक्त;

    प्रतिज्ञा(s != रिक्त);

    जर (म_अक्षर(s, '_') == रिक्त) {
        परत parsenumber_raw(s);
    }
    /* Create a duplicate without underscores. */
    dup = PyMem_Malloc(म_माप(s) + 1);
    जर (dup == रिक्त) {
        परत PyErr_NoMemory();
    }
    end = dup;
    साठी (; *s; s++) {
        जर (*s != '_') {
            *end++ = *s;
        }
    }
    *end = '\0';
    res = parsenumber_raw(dup);
    PyMem_Free(dup);
    परत res;
}

अभिव्यक्ती_ty
Py_Pegen_संख्या_token(Parser *p)
{
    पूर्णांक mark = p->mark;
    Token *t = _PyPegen_expect_token(p, संख्या);
    जर (t == रिक्त) {
        परत रिक्त;
    }

    // Look up the node at the token's start, as for NAME tokens. A memo hit
    // advances the mark; on a miss, restore the position after the token.
    p->mark = mark;
    अभिव्यक्ती_ty cached = रिक्त;
    जर (_PyPegen_is_memoized(p, संख्या, &cached)) {
        परत cached;
    }
    p->mark = mark + 1;

    स्थिरांक अक्षर *अंक_raw = PyBytes_AsString(t->bytes);
    जर (अंक_raw == रिक्त) {
        p->error_indicator = 1;
        परत रिक्त;
    }

    जर (p->feature_version < 6 && म_अक्षर(अंक_raw, '_') != रिक्त) {
        p->error_indicator = 1;
        परत RAISE_SYNTAX_ERROR("Underscores in numeric literals are only supported "
                                  "in Python 3.6 and greater");
    }

    PyObject *c = parsenumber(अंक_raw);

    जर (c == रिक्त) {
        p->error_indicator = 1;
        PyThreadState *tstate = _PyThreadState_GET();
        // The only way a ValueError should happen in _this_ code is via
        // PyLong_FromString hitting a length limit.
        जर (tstate->current_exception != रिक्त &&
            Py_TYPE(tstate->current_exception) == (PyTypeObject *)Py_Exc_मूल्य_Error
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
        परत रिक्त;
    }

    जर (_PyArena_AddPyObject(p->arena, c) < 0) {
        Py_DECREF(c);
        p->error_indicator = 1;
        परत रिक्त;
    }

    अभिव्यक्ती_ty result = _PyAST_Constant(c, रिक्त, t->lineno, t->col_offset,
                                    t->end_lineno, t->end_col_offset, p->arena);
    जर (result != रिक्त && Py_Pegen_घाला_memo(p, mark, संख्या, result) < 0) {
        p->error_indicator = 1;
        परत रिक्त;
    }
    परत result;
}



स्थिर पूर्णांक
compute_parser_flags(PyCompilerFlags *flags)
{
    पूर्णांक parser_flags = 0;
    जर (!flags) {
        परत 0;
    }
    जर (flags->cf_flags & PyCF_DONT_IMPLY_DEDENT) {
        parser_flags |= PyPARSE_DONT_IMPLY_DEDENT;
    }
    जर (flags->cf_flags & PyCF_IGNORE_COOKIE) {
        parser_flags |= PyPARSE_IGNORE_COOKIE;
    }
    जर (flags->cf_flags & CO_FUTURE_BARRY_AS_BDFL) {
        parser_flags |= PyPARSE_BARRY_AS_BDFL;
    }
    जर (flags->cf_flags & PyCF_TYPE_COMMENTS) {
        parser_flags |= PyPARSE_TYPE_COMMENTS;
    }
    जर (flags->cf_flags & Py_CF_ALLOW_INCOMPLETE_आदान) {
        parser_flags |= Py_PARSE_ALLOW_INCOMPLETE_आदान;
    }
    परत parser_flags;
}

// Parser API

Parser *
_PyPegen_Parser_New(संरचना tok_state *tok, पूर्णांक start_rule, पूर्णांक flags,
                    पूर्णांक feature_version, पूर्णांक *errcode, स्थिरांक अक्षर* source, PyArena *arena)
{
    Parser *p = PyMem_Malloc(आकार(Parser));
    जर (p == रिक्त) {
        परत (Parser *) PyErr_NoMemory();
    }
    प्रतिज्ञा(tok != रिक्त);
    _PyTokenizer_SetOptions(tok, 0, (flags & PyPARSE_TYPE_COMMENTS) > 0);
    Py_Tokenizer_Allow_Incomplete_आदान(tok, flags & Py_PARSE_ALLOW_INCOMPLETE_आदान);
    p->tok = tok;
    p->keywords = रिक्त;
    p->n_keyword_lists = -1;
    p->soft_keywords = रिक्त;
    p->tokens = PyMem_Malloc(आकार(Token *));
    जर (!p->tokens) {
        PyMem_Free(p);
        परत (Parser *) PyErr_NoMemory();
    }
    p->tokens[0] = PyMem_Calloc(1, आकार(Token));
    जर (!p->tokens[0]) {
        PyMem_Free(p->tokens);
        PyMem_Free(p);
        परत (Parser *) PyErr_NoMemory();
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
    प्रतिज्ञा(((_PyThreadStateImpl *)p->tstate)->c_चळत_hard_limit != 0);
    p->चळत_soft_limit = ((_PyThreadStateImpl *)p->tstate)->c_चळत_soft_limit;
#जर Py_चळत_GROWS_DOWN
    p->चळत_soft_limit += Py_OS_चळत_MARGIN_BYTES;
#अन्यथा
    p->चळत_soft_limit -= Py_OS_चळत_MARGIN_BYTES;
#endif
    p->level = 0;
    p->call_invalid_rules = 0;
    p->आदान_exhausted = 0;
    p->last_stmt_location.lineno = 0;
    p->last_stmt_location.col_offset = 0;
    p->last_stmt_location.end_lineno = 0;
    p->last_stmt_location.end_col_offset = 0;
#ifdef Py_DEBUG
    p->debug = _Py_GetConfig()->parser_debug;
#endif
    परत p;
}

शून्य
_PyPegen_Parser_Free(Parser *p)
{
    PyMem_Free(p->identifier_cache);
    Py_XDECREF(p->normalize);
    // Resizes allocate blocks starting at indices 1, 2, 4, and so on.
    PyMem_Free(p->tokens[0]);
    साठी (पूर्णांक i = 1; i < p->size; i *= PEGEN_ARRAY_GROWTH_FACTOR) {
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
    साठी (पूर्णांक i = 0; i < p->fill; i++) {
        p->tokens[i]->memo = रिक्त;
        p->tokens[i]->memo_mask = 0;
    }
    p->mark = 0;
    p->flags &= ~Py_PARSE_ALLOW_INCOMPLETE_आदान;
    Py_Tokenizer_Allow_Incomplete_आदान(p->tok, 0);
    p->call_invalid_rules = 1;
    _PyTokenizer_StopInteractive(p->tok);
}

स्थिर शून्य
_PyPegen_set_syntax_error_metadata(Parser *p) {
    PyObject *exc = PyErr_GetRaisedException();
    जर (!exc || !PyObject_TypeCheck(exc, (PyTypeObject *)PyExc_SyntaxError)) {
        PyErr_SetRaisedException(exc);
        परत;
    }
    स्थिरांक अक्षर *source = _PyTokenizer_RetainedSource(p->tok);
    स्थिरांक अक्षर *encoding = _PyTokenizer_GetInfo(p->tok).encoding;
    PyObject* the_source = रिक्त;
    जर (source) {
        जर (encoding == रिक्त) {
            the_source = PyUnicode_FromString(source);
        } अन्यथा {
            the_source = PyUnicode_Decode(source, म_माप(source), encoding, रिक्त);
        }
    }
    जर (!the_source) {
        PyErr_Clear();
        the_source = Py_None;
        Py_INCREF(the_source);
    }
    PyObject* metadata = Py_Build_मूल्य(
        "(iiN)",
        p->last_stmt_location.lineno,
        p->last_stmt_location.col_offset,
        the_source // N gives ownership to metadata
    );
    जर (!metadata) {
        PyErr_Clear();
        PyErr_SetRaisedException(exc);
        परत;
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
    जर (res != रिक्त && PyErr_Occurred()) {
        // Discard a result returned with an exception still pending
        // (e.g. a MemoryError from a recovered-from allocation failure).
        परत रिक्त;
    }
    जर (res == रिक्त) {
        जर (PyErr_Occurred() &&
            (PyErr_ExceptionMatches(Py_Exc_Incomplete_आदान_Error) ||
             !PyErr_ExceptionMatches(PyExc_SyntaxError))) {
            परत रिक्त;
        }
        जर ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_आदान) &&
            p->आदान_exhausted && !PyErr_Occurred()) {
            परत Py_Pegen_raise_incomplete_आदान(p);
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
        जर (PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_set_syntax_error_metadata(p);
        }
       परत रिक्त;
    }

    जर (p->start_rule == Py_single_आदान && _PyTokenizer_HasTrailingStatement(p->tok)) {
        परत RAISE_SYNTAX_ERROR("multiple statements found while compiling a single statement");
    }

    // test_peg_generator defines _Py_TEST_PEGEN to not call PyAST_Validate()
#जर defined(Py_DEBUG) && !defined(_Py_TEST_PEGEN)
    जर (p->start_rule == Py_single_आदान ||
        p->start_rule == Py_file_आदान ||
        p->start_rule == Py_eval_आदान)
    {
        जर (!_PyAST_Validate(res)) {
            परत रिक्त;
        }
    }
#endif
    परत res;
}

mod_ty
_PyPegen_run_parser_from_file_pointer(खाता *fp, पूर्णांक start_rule, PyObject *filename_ob,
                             स्थिरांक अक्षर *enc, स्थिरांक अक्षर *ps1, स्थिरांक अक्षर *ps2,
                             PyCompilerFlags *flags, पूर्णांक *errcode,
                             PyObject **interactive_src, PyArena *arena)
{
    पूर्णांक parser_flags = compute_parser_flags(flags);
    संरचना tok_state *tok = _PyTokenizer_FromFile(fp, enc, ps1, ps2);
    जर (tok == रिक्त) {
        जर (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        अन्यथा {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        परत रिक्त;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    // From here on we need to clean up even if there's an error
    mod_ty result = रिक्त;

    PyObject *module = PyUnicode_FromString("__main__");
    जर (module == रिक्त) {
        जा error;
    }
    _PyTokenizer_SetContext(tok, filename_ob, module);
    Py_DECREF(module);

    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, PY_MINOR_VERSION,
                                    errcode, रिक्त, arena);
    जर (p == रिक्त) {
        जा error;
    }

    result = _PyPegen_run_parser(p);
    _PyPegen_Parser_Free(p);

    स्थिरांक अक्षर *source = _PyTokenizer_RetainedSource(tok);
    जर (source != रिक्त && result && interactive_src != रिक्त) {
        *interactive_src = PyUnicode_FromString(source);
        जर (!*interactive_src || _PyArena_AddPyObject(arena, *interactive_src) < 0) {
            Py_XDECREF(*interactive_src);
            result = रिक्त;
            जा error;
        }
    }

error:
    _PyTokenizer_Free(tok);
    परत result;
}

mod_ty
_PyPegen_run_parser_from_string(स्थिरांक अक्षर *str, पूर्णांक start_rule, PyObject *filename_ob,
                       PyCompilerFlags *flags, PyArena *arena, PyObject *module,
                       PyObject **incomplete)
{
    _PyTokenizer_Newline newline_mode = start_rule == Py_file_आदान
        ? _PyTokenizer_IMPLY_NEWLINE : _PyTokenizer_KEEP_NEWLINE;
    पूर्णांक parser_flags = compute_parser_flags(flags);
    जर (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_आदान) {
        newline_mode = _PyTokenizer_APPEND_NEWLINE;
    }

    संरचना tok_state *tok;
    जर (flags != रिक्त && flags->cf_flags & PyCF_IGNORE_COOKIE) {
        tok = _PyTokenizer_FromUTF8(str, newline_mode, 0);
    } अन्यथा {
        tok = _PyTokenizer_FromString(str, newline_mode, 0);
    }
    जर (tok == रिक्त) {
        जर (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        अन्यथा {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        परत रिक्त;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    _PyTokenizer_SetContext(tok, filename_ob, module);

    // We need to clear up from here on
    mod_ty result = रिक्त;

    पूर्णांक feature_version = flags && (flags->cf_flags & PyCF_ONLY_AST) ?
        flags->cf_feature_version : PY_MINOR_VERSION;
    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, feature_version,
                                    रिक्त, str, arena);
    जर (p == रिक्त) {
        जा error;
    }

    result = _PyPegen_run_parser(p);
    जर (result != रिक्त && (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_आदान)) {
        पूर्णांक needs_newline = _PyTokenizer_GetNewlineRequirements(tok);
        जर ((needs_newline & _PyTokenizer_NEWLINE_CONTINUATION) ||
            (start_rule != Py_file_आदान &&
             (needs_newline & _PyTokenizer_NEWLINE_INDENT)) ||
            (start_rule == Py_single_आदान && p->आदान_exhausted &&
             (needs_newline & _PyTokenizer_NEWLINE_STATEMENT))) {
            Py_Pegen_raise_incomplete_आदान(p);
            जर (incomplete != रिक्त && PyErr_ExceptionMatches(Py_Exc_Incomplete_आदान_Error)) {
                *incomplete = PyErr_GetRaisedException();
            }
            अन्यथा {
                result = रिक्त;
            }
        }
    }
    _PyPegen_Parser_Free(p);

error:
    _PyTokenizer_Free(tok);
    परत result;
}
