#અન્તર્ભાવ <Python.h>
#અન્તર્ભાવ "pycore_ast.h"           // _PyAST_Validate(),
#અન્તર્ભાવ "pycore_pystate.h"       // _PyThreadState_GET()
#અન્તર્ભાવ "pycore_parser.h"        // _PYPEGEN_NSTATISTICS
#અન્તર્ભાવ "pycore_pyerrors.h"      // PyExc_IncompleteInputError
#અન્તર્ભાવ "pycore_runtime.h"       // _PyRuntime
#અન્તર્ભાવ "pycore_unicodeobject.h" // _PyUnicode_InternImmortal
#અન્તર્ભાવ <errcode.h>

#અન્તર્ભાવ "lexer/state.h"
#અન્તર્ભાવ "tokenizer/tokenizer.h"
#અન્તર્ભાવ "pegen.h"

#define IDENTIFIER_CACHE_SIZE 2048  // Must be a power of two.
#define IDENTIFIER_CACHE_MAX_PROBES 8
#define PEGEN_ARRAY_GROWTH_FACTOR 2
#define TYPE_IGNORE_INITIAL_CAPACITY 10

સંરચના _identifier_cache_entry {
    અચલ અક્ષર *ચાવી;   // Borrowed from arena-owned token bytes.
    Py_ssize_t len;
    Py_hash_t hash;
    PyObject *કિંમત;   // Borrowed from an arena-owned identifier.
};

// Internal parser functions

asdl_stmt_seq*
_PyPegen_interactive_exit(Parser *p)
{
    જો (p->errcode) {
        *(p->errcode) = E_EOF;
    }
    પાછા ખાલી;
}

Py_ssize_t
Py_Pegen_byte_offset_to_character_offset_લીટી(PyObject *લીટી, Py_ssize_t col_offset, Py_ssize_t end_col_offset)
{
    અચલ unsigned અક્ષર *data = (અચલ unsigned અક્ષર*)PyUnicode_AsUTF8(લીટી);
    જો (data == ખાલી) {
        પાછા -1;
    }

    Py_ssize_t len = 0;
    જ્યારે (col_offset < end_col_offset) {
        Py_UCS4 ch = data[col_offset];
        જો (ch < 0x80) {
            col_offset += 1;
        } નહીંતર જો ((ch & 0xe0) == 0xc0) {
            col_offset += 2;
        } નહીંતર જો ((ch & 0xf0) == 0xe0) {
            col_offset += 3;
        } નહીંતર જો ((ch & 0xf8) == 0xf0) {
            col_offset += 4;
        } નહીંતર {
            PyErr_SetString(Py_Exc_કિંમત_Error, "Invalid UTF-8 sequence");
            પાછા -1;
        }
        len++;
    }
    પાછા len;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset_raw(અચલ અક્ષર* str, Py_ssize_t col_offset)
{
    Py_ssize_t len = (Py_ssize_t)strlen(str);
    જો (col_offset > len + 1) {
        col_offset = len + 1;
    }
    પ્રતિજ્ઞા(col_offset >= 0);
    PyObject *text = PyUnicode_DecodeUTF8(str, col_offset, "replace");
    જો (!text) {
        પાછા -1;
    }
    Py_ssize_t size = PyUnicode_GET_LENGTH(text);
    Py_DECREF(text);
    પાછા size;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset(PyObject *લીટી, Py_ssize_t col_offset)
{
    અચલ અક્ષર *str = PyUnicode_AsUTF8(લીટી);
    જો (!str) {
        પાછા -1;
    }
    પાછા _PyPegen_byte_offset_to_character_offset_raw(str, col_offset);
}

// Here, mark is the start of the node, while p->mark is the end.
// If node==NULL, they should be the same.
પૂર્ણાંક
Py_Pegen_ઉમેરો_memo(Parser *p, પૂર્ણાંક mark, પૂર્ણાંક type, શૂન્ય *ગાંઠ)
{
    // Insert in front
    Memo *m = _PyArena_Malloc(p->arena, કદ(Memo));
    જો (m == ખાલી) {
        પાછા -1;
    }
    m->type = type;
    m->ગાંઠ = ગાંઠ;
    m->mark = p->mark;
    m->next = p->tokens[mark]->memo;
    p->tokens[mark]->memo = m;
    p->tokens[mark]->memo_mask |= 1ULL << (type & 63);
    પાછા 0;
}

// Like _PyPegen_insert_memo(), but returns the inserted Memo so callers
// can update it in place without re-walking the token's memo list.
Memo *
Py_Pegen_ઉમેરો_memo_direct(Parser *p, પૂર્ણાંક mark, પૂર્ણાંક type)
{
    જો (Py_Pegen_ઉમેરો_memo(p, mark, type, ખાલી) < 0) {
        પાછા ખાલી;
    }
    પાછા p->tokens[mark]->memo;
}

// Like _PyPegen_insert_memo(), but updates an existing node if found.
પૂર્ણાંક
_PyPegen_update_memo(Parser *p, પૂર્ણાંક mark, પૂર્ણાંક type, શૂન્ય *ગાંઠ)
{
    માટે (Memo *m = p->tokens[mark]->memo; m != ખાલી; m = m->next) {
        જો (m->type == type) {
            // Update existing node.
            m->ગાંઠ = ગાંઠ;
            m->mark = p->mark;
            પાછા 0;
        }
    }
    // Insert new node.
    પાછા Py_Pegen_ઉમેરો_memo(p, mark, type, ગાંઠ);
}

સ્થિર પૂર્ણાંક
init_normalization(Parser *p)
{
    જો (p->normalize) {
        પાછા 1;
    }
    p->normalize = PyImport_ImportModuleAttrString("unicodedata", "normalize");
    જો (!p->normalize)
    {
        પાછા 0;
    }
    પાછા 1;
}

પૂર્ણાંક
_PyPegen_grow_loop_buffer(શૂન્ય ***buffer, Py_ssize_t *capacity)
{
    જો ((size_t)*capacity > (size_t)PY_SSIZE_T_MAX /
            (PEGEN_ARRAY_GROWTH_FACTOR * કદ(**buffer))) {
        પાછા -1;
    }
    Py_ssize_t new_capacity = *capacity == 0
        ? 1 : *capacity * PEGEN_ARRAY_GROWTH_FACTOR;
    શૂન્ય **new_buffer = PyMem_Realloc(*buffer, new_capacity * કદ(**buffer));
    જો (new_buffer == ખાલી) {
        પાછા -1;
    }
    *buffer = new_buffer;
    *capacity = new_capacity;
    પાછા 0;
}

સ્થિર પૂર્ણાંક
growable_comment_array_add(growable_comment_array *arr, પૂર્ણાંક lineno, અક્ષર *comment)
{
    જો (arr->અંક_વસ્તુઓ >= arr->size) {
        જો (arr->size > (size_t)PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * કદ(*arr->વસ્તુઓ))) {
            પાછા 0;
        }
        size_t new_size = arr->size == 0
            ? TYPE_IGNORE_INITIAL_CAPACITY
            : arr->size * PEGEN_ARRAY_GROWTH_FACTOR;
        શૂન્ય *new_વસ્તુઓ_array = PyMem_Realloc(
            arr->વસ્તુઓ, new_size * કદ(*arr->વસ્તુઓ));
        જો (!new_વસ્તુઓ_array) {
            પાછા 0;
        }
        arr->વસ્તુઓ = new_વસ્તુઓ_array;
        arr->size = new_size;
    }

    arr->વસ્તુઓ[arr->અંક_વસ્તુઓ].lineno = lineno;
    arr->વસ્તુઓ[arr->અંક_વસ્તુઓ].comment = comment;  // Take ownership
    arr->અંક_વસ્તુઓ++;
    પાછા 1;
}

સ્થિર શૂન્ય
growable_comment_array_deallocate(growable_comment_array *arr)
{
    માટે (size_t i = 0; i < arr->અંક_વસ્તુઓ; i++) {
        PyMem_Free(arr->વસ્તુઓ[i].comment);
    }
    PyMem_Free(arr->વસ્તુઓ);
}

સ્થિર પૂર્ણાંક
_get_keyword_or_name_type(Parser *p, અચલ અક્ષર *text, Py_ssize_t length)
{
    પ્રતિજ્ઞા(length > 0);

    જો (length >= p->n_keyword_lists ||
        p->keywords[length] == ખાલી ||
        p->keywords[length]->type == -1) {
        પાછા NAME;
    }
    માટે (KeywordToken *k = p->keywords[length]; k != ખાલી && k->type != -1; k++) {
        જો (memcmp(k->str, text, (size_t)length) == 0) {
            પાછા k->type;
        }
    }
    પાછા NAME;
}

// Keep text only where grammar actions or helpers read it. Keyword error
// actions use fixed spellings, so keywords do not need their text.
સ્થિર અંતઃરેખ પૂર્ણાંક
token_needs_text(પૂર્ણાંક type)
{
    પસંદગી (type) {
        સ્થિતિ NAME:
        સ્થિતિ સંખ્યા:
        સ્થિતિ STRING:
        સ્થિતિ FSTRING_START:
        સ્થિતિ FSTRING_મધ્ય:
        સ્થિતિ FSTRING_END:
        સ્થિતિ TSTRING_START:
        સ્થિતિ TSTRING_મધ્ય:
        સ્થિતિ TSTRING_END:
        સ્થિતિ TYPE_COMMENT:
        સ્થિતિ NOTEQUAL:  // _PyPegen_check_barry_as_flufl() distinguishes != and <>.
            પાછા 1;
        મૂળભૂત:
            પાછા 0;
    }
}

સ્થિર પૂર્ણાંક
initialize_token(Parser *p, Token *parser_token, સંરચના token *new_token, પૂર્ણાંક token_type) {
    પ્રતિજ્ઞા(parser_token != ખાલી);

    parser_token->type = token_type;
    parser_token->bytes = ખાલી;
    જો (token_needs_text(token_type) && new_token->span.start >= 0) {
        Py_ssize_t length;
        અચલ અક્ષર *text = _PyToken_TextView(p->tok, new_token, &length);
        જો (token_type == NAME) {
            parser_token->type = _get_keyword_or_name_type(p, text, length);
        }
        // A name reclassified as a keyword does not need its text.
        જો (parser_token->type == token_type) {
            parser_token->bytes = PyBytes_FromStringAndSize(text, length);
            જો (parser_token->bytes == ખાલી) {
                પાછા -1;
            }
            જો (_PyArena_AddPyObject(p->arena, parser_token->bytes) < 0) {
                Py_DECREF(parser_token->bytes);
                પાછા -1;
            }
        }
    }

    parser_token->metadata = ખાલી;
    જો (new_token->metadata != ખાલી) {
        જો (_PyArena_AddPyObject(p->arena, new_token->metadata) < 0) {
            પાછા -1;
        }
        parser_token->metadata = new_token->metadata;
        new_token->metadata = ખાલી;
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

    જો (token_type == ERRORTOKEN && _PyTokenizer_GetStatus(p->tok) == E_DECODE) {
        પાછા _Pypegen_raise_decode_error(p);
    }

    પાછા (token_type == ERRORTOKEN ? _Pypegen_tokenizer_error(p) : 0);
}

સ્થિર પૂર્ણાંક
_resize_tokens_array(Parser *p) {
    જો (p->size > INT_MAX / PEGEN_ARRAY_GROWTH_FACTOR ||
            (size_t)p->size > PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * કદ(*p->tokens))) {
        PyErr_NoMemory();
        પાછા -1;
    }
    પૂર્ણાંક newsize = p->size * PEGEN_ARRAY_GROWTH_FACTOR;
    Token **new_tokens = PyMem_Realloc(p->tokens, (size_t)newsize * કદ(Token *));
    જો (new_tokens == ખાલી) {
        PyErr_NoMemory();
        પાછા -1;
    }
    p->tokens = new_tokens;

    // Allocate new tokens together without moving existing ones: parser rules
    // may still hold pointers into earlier blocks. Parser_Free frees each block.
    Token *chunk = PyMem_Calloc((size_t)(newsize - p->size), કદ(Token));
    જો (chunk == ખાલી) {
        PyErr_NoMemory();
        પાછા -1;
    }
    માટે (પૂર્ણાંક i = p->size; i < newsize; i++) {
        p->tokens[i] = &chunk[i - p->size];
    }
    p->size = newsize;
    પાછા 0;
}

પૂર્ણાંક
_PyPegen_fill_token(Parser *p)
{
    સંરચના token new_token;
    _PyToken_Init(&new_token);
    _PyTokenizer_Get(p->tok, &new_token);

    // Record and skip '# type: ignore' comments
    જ્યારે (new_token.type == TYPE_IGNORE) {
        Py_ssize_t len;
        અચલ અક્ષર *text = _PyToken_TextView(p->tok, &new_token, &len);
        અક્ષર *tag = PyMem_Malloc((size_t)len + 1);
        જો (tag == ખાલી) {
            PyErr_NoMemory();
            જાઓ error;
        }
        memcpy(tag, text, (size_t)len);
        tag[len] = '\0';
        // Ownership of tag passes to the growable array
        જો (!growable_comment_array_add(&p->type_ignore_comments, new_token.end_loc.lineno, tag)) {
            PyMem_Free(tag);
            PyErr_NoMemory();
            જાઓ error;
        }
        _PyTokenizer_Get(p->tok, &new_token);
    }

    પૂર્ણાંક type = new_token.type;
    // EOF can finish an ordinary token. Only a boundary token means that
    // the grammar requested input beyond the last physical line.
    જો (type == ENDMARKER ||
        (type == DEDENT && (p->flags & Py_PARSE_ALLOW_INCOMPLETE_આગત) &&
         _PyTokenizer_GetStatus(p->tok) == E_EOF)) {
        p->આગત_exhausted = 1;
    }

    // If we have reached the end and we are in single input mode we need to insert a newline and reset the parsing
    જો (p->start_rule == Py_single_આગત && type == ENDMARKER && p->parsing_started) {
        type = NEWLINE; /* Add an extra newline */
        p->parsing_started = 0;

        જો (!(p->flags & PyPARSE_DONT_IMPLY_DEDENT)) {
            _PyTokenizer_ImplyDedents(p->tok);
        }
    }
    નહીંતર {
        p->parsing_started = 1;
    }

    // Check if we are at the limit of the token array capacity and resize if needed
    જો ((p->fill == p->size) && (_resize_tokens_array(p) != 0)) {
        જાઓ error;
    }

    Token *t = p->tokens[p->fill];
    પૂર્ણાંક result = initialize_token(p, t, &new_token, type);
    _PyToken_Free(&new_token);
    પાછા result;
error:
    _PyToken_Free(&new_token);
    પાછા -1;
}

#જો defined(Py_DEBUG)
// Instrumentation to count the effectiveness of memoization.
// The array counts the number of tokens skipped by memoization,
// indexed by type.

#define NSTATISTICS _PYPEGEN_NSTATISTICS
#define memo_statistics _PyRuntime.parser.memo_statistics

શૂન્ય
_PyPegen_clear_memo_statistics(શૂન્ય)
{
    PyMutex_Lock(&_PyRuntime.parser.mutex);
    માટે (પૂર્ણાંક i = 0; i < NSTATISTICS; i++) {
        memo_statistics[i] = 0;
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
}

PyObject *
_PyPegen_get_memo_statistics(શૂન્ય)
{
    PyObject *ret = PyList_New(NSTATISTICS);
    જો (ret == ખાલી) {
        પાછા ખાલી;
    }

    PyMutex_Lock(&_PyRuntime.parser.mutex);
    માટે (પૂર્ણાંક i = 0; i < NSTATISTICS; i++) {
        PyObject *કિંમત = PyLong_FromLong(memo_statistics[i]);
        જો (કિંમત == ખાલી) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            પાછા ખાલી;
        }
        // PyList_SetItem borrows a reference to value.
        જો (PyList_SetItem(ret, i, કિંમત) < 0) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            પાછા ખાલી;
        }
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
    પાછા ret;
}
#endif

પૂર્ણાંક  // bool
_PyPegen_is_memoized(Parser *p, પૂર્ણાંક type, શૂન્ય *pres)
{
    જો (p->mark == p->fill) {
        જો (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            પાછા -1;
        }
    }

    Token *t = p->tokens[p->mark];

    જો (!(t->memo_mask & (1ULL << (type & 63)))) {
        પાછા 0;
    }

    માટે (Memo *m = t->memo; m != ખાલી; m = m->next) {
        જો (m->type == type) {
#જો defined(Py_DEBUG)
            જો (0 <= type && type < NSTATISTICS) {
                long ગણતરી = m->mark - p->mark;
                // A memoized negative result counts for one.
                જો (ગણતરી <= 0) {
                    ગણતરી = 1;
                }
                PyMutex_Lock(&_PyRuntime.parser.mutex);
                memo_statistics[type] += ગણતરી;
                PyMutex_Unlock(&_PyRuntime.parser.mutex);
            }
#endif
            p->mark = m->mark;
            *(શૂન્ય **)(pres) = m->ગાંઠ;
            પાછા 1;
        }
    }
    પાછા 0;
}

#define LOOKAHEAD1(NAME, RES_TYPE)                                  \
    પૂર્ણાંક                                                             \
    NAME (પૂર્ણાંક positive, RES_TYPE (func)(Parser *), Parser *p)       \
    {                                                               \
        પૂર્ણાંક mark = p->mark;                                         \
        શૂન્ય *res = func(p);                                        \
        p->mark = mark;                                             \
        પાછા (res != ખાલી) == positive;                           \
    }

LOOKAHEAD1(_PyPegen_lookahead, શૂન્ય *)
LOOKAHEAD1(Py_Pegen_lookahead_for_પદાવલિ, પદાવલિ_ty)
LOOKAHEAD1(_PyPegen_lookahead_for_stmt, stmt_ty)
#undef LOOKAHEAD1

#define LOOKAHEAD2(NAME, RES_TYPE, T)                                   \
    પૂર્ણાંક                                                                 \
    NAME (પૂર્ણાંક positive, RES_TYPE (func)(Parser *, T), Parser *p, T arg) \
    {                                                                   \
        પૂર્ણાંક mark = p->mark;                                             \
        શૂન્ય *res = func(p, arg);                                       \
        p->mark = mark;                                                 \
        પાછા (res != ખાલી) == positive;                               \
    }

LOOKAHEAD2(_PyPegen_lookahead_with_int, Token *, પૂર્ણાંક)
LOOKAHEAD2(_PyPegen_lookahead_with_string, પદાવલિ_ty, અચલ અક્ષર *)
#undef LOOKAHEAD2

Token *
_PyPegen_expect_token(Parser *p, પૂર્ણાંક type)
{
    જો (p->mark == p->fill) {
        જો (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            પાછા ખાલી;
        }
    }
    Token *t = p->tokens[p->mark];
    જો (t->type != type) {
       પાછા ખાલી;
    }
    p->mark += 1;
    પાછા t;
}

શૂન્ય*
_PyPegen_expect_forced_result(Parser *p, શૂન્ય* result, અચલ અક્ષર* expected) {

    જો (p->error_indicator == 1) {
        પાછા ખાલી;
    }
    જો (result == ખાલી) {
        RAISE_SYNTAX_ERROR("expected (%s)", expected);
        પાછા ખાલી;
    }
    પાછા result;
}

Token *
_PyPegen_expect_forced_token(Parser *p, પૂર્ણાંક type, અચલ અક્ષર* expected) {

    જો (p->error_indicator == 1) {
        પાછા ખાલી;
    }

    જો (p->mark == p->fill) {
        જો (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            પાછા ખાલી;
        }
    }
    Token *t = p->tokens[p->mark];
    જો (t->type != type) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(t, "expected '%s'", expected);
        પાછા ખાલી;
    }
    p->mark += 1;
    પાછા t;
}

પદાવલિ_ty
_PyPegen_expect_soft_keyword(Parser *p, અચલ અક્ષર *keyword)
{
    જો (p->mark == p->fill) {
        જો (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            પાછા ખાલી;
        }
    }
    Token *t = p->tokens[p->mark];
    જો (t->type != NAME) {
        પાછા ખાલી;
    }
    અચલ અક્ષર *s = PyBytes_AsString(t->bytes);
    જો (!s) {
        p->error_indicator = 1;
        પાછા ખાલી;
    }
    જો (strcmp(s, keyword) != 0) {
        પાછા ખાલી;
    }
    પાછા _PyPegen_name_token(p);
}

Token *
_PyPegen_get_last_nonnwhitespace_token(Parser *p)
{
    પ્રતિજ્ઞા(p->mark >= 0);
    Token *token = ખાલી;
    માટે (પૂર્ણાંક m = p->mark - 1; m >= 0; m--) {
        token = p->tokens[m];
        જો (token->type != ENDMARKER && (token->type < NEWLINE || token->type > DEDENT)) {
            તોડો;
        }
    }
    પાછા token;
}

PyObject *
_PyPegen_new_identifier(Parser *p, અચલ અક્ષર *n)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, (Py_ssize_t)strlen(n), ખાલી);
    જો (!id) {
        જાઓ error;
    }
    /* Check whether there are non-ASCII characters in the
       identifier; if so, normalize to NFKC. */
    જો (!PyUnicode_IS_ASCII(id))
    {
        જો (!init_normalization(p))
        {
            Py_DECREF(id);
            જાઓ error;
        }
        PyObject *form = PyUnicode_InternFromString("NFKC");
        જો (form == ખાલી)
        {
            Py_DECREF(id);
            જાઓ error;
        }
        PyObject *args[2] = {form, id};
        PyObject *id2 = PyObject_Vectorcall(p->normalize, args, 2, ખાલી);
        Py_DECREF(id);
        Py_DECREF(form);
        જો (!id2) {
            જાઓ error;
        }

        જો (!PyUnicode_Check(id2))
        {
            PyErr_Format(PyExc_TypeError,
                         "unicodedata.normalize() must return a string, not "
                         "%.200s",
                         _PyType_Name(Py_TYPE(id2)));
            Py_DECREF(id2);
            જાઓ error;
        }
        id = id2;
    }
    સ્થિર અચલ અક્ષર * અચલ forbidden[] = {
        "None",
        "True",
        "False",
        ખાલી
    };
    માટે (પૂર્ણાંક i = 0; forbidden[i] != ખાલી; i++) {
        જો (_PyUnicode_EqualToASCIIString(id, forbidden[i])) {
            PyErr_Format(Py_Exc_કિંમત_Error,
                         "identifier field can't represent '%s' constant",
                         forbidden[i]);
            Py_DECREF(id);
            જાઓ error;
        }
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &id);
    જો (_PyArena_AddPyObject(p->arena, id) < 0)
    {
        Py_DECREF(id);
        જાઓ error;
    }
    પાછા id;

error:
    p->error_indicator = 1;
    પાછા ખાલી;
}

// Return an arena-owned identifier; callers borrow the reference.
સ્થિર PyObject *
get_cached_identifier(Parser *p, PyObject *bytes)
{
    અચલ અક્ષર *s = PyBytes_AsString(bytes);
    જો (!s) {
        p->error_indicator = 1;
        પાછા ખાલી;
    }
    // Identifiers repeat constantly; a small span-keyed cache skips the
    // UTF-8 decode + intern for repeated occurrences. Keys point into
    // arena-owned token bytes and values are arena-owned interned strings,
    // so borrowed references are valid for the lifetime of the parse
    // (including the second error pass, which reuses parser and arena).
    Py_ssize_t len = PyBytes_GET_SIZE(bytes);
    Py_hash_t hash = PyObject_Hash(bytes);
    જો (hash == -1) {
        p->error_indicator = 1;
        પાછા ખાલી;
    }
    // Parses without identifiers do not need this cache.
    જો (p->identifier_cache == ખાલી) {
        p->identifier_cache = PyMem_Calloc(
            IDENTIFIER_CACHE_SIZE, કદ(*p->identifier_cache));
        જો (p->identifier_cache == ખાલી) {
            p->error_indicator = 1;
            પાછા PyErr_NoMemory();
        }
    }
    IdentifierCacheEntry *free_slot = ખાલી;
    size_t idx = (size_t)hash & (IDENTIFIER_CACHE_SIZE - 1);
    માટે (પૂર્ણાંક probe = 0; probe < IDENTIFIER_CACHE_MAX_PROBES; probe++) {
        IdentifierCacheEntry *entry = &p->identifier_cache[
            (idx + probe) & (IDENTIFIER_CACHE_SIZE - 1)];
        જો (entry->ચાવી == ખાલી) {
            free_slot = entry;
            તોડો;
        }
        જો (entry->hash == hash && entry->len == len &&
            memcmp(entry->ચાવી, s, len) == 0)
        {
            પાછા entry->કિંમત;
        }
    }
    PyObject *id = _PyPegen_new_identifier(p, s);
    જો (id == ખાલી) {
        p->error_indicator = 1;
        પાછા ખાલી;
    }
    જો (free_slot != ખાલી) {
        free_slot->ચાવી = s;
        free_slot->len = len;
        free_slot->hash = hash;
        free_slot->કિંમત = id;
    }
    પાછા id;
}

સ્થિર પદાવલિ_ty
_PyPegen_name_from_token(Parser *p, Token* t)
{
    જો (t == ખાલી) {
        પાછા ખાલી;
    }
    // Reuse the AST node when backtracking revisits this token. Memo lookup
    // starts before the token and restores the position after it on a hit.
    // Token kinds can be memo keys: generated grammar rule IDs start at 1000.
    પૂર્ણાંક mark = p->mark - 1;
    p->mark = mark;
    પદાવલિ_ty cached = ખાલી;
    જો (_PyPegen_is_memoized(p, NAME, &cached)) {
        પાછા cached;
    }
    p->mark = mark + 1;
    PyObject *id = get_cached_identifier(p, t->bytes);
    જો (id == ખાલી) {
        પાછા ખાલી;
    }
    પદાવલિ_ty result = _PyAST_Name(id, Load, t->lineno, t->col_offset,
                                t->end_lineno, t->end_col_offset, p->arena);
    જો (result != ખાલી && Py_Pegen_ઉમેરો_memo(p, mark, NAME, result) < 0) {
        p->error_indicator = 1;
        પાછા ખાલી;
    }
    પાછા result;
}

પદાવલિ_ty
_PyPegen_name_token(Parser *p)
{
    Token *t = _PyPegen_expect_token(p, NAME);
    પાછા _PyPegen_name_from_token(p, t);
}

શૂન્ય *
_PyPegen_string_token(Parser *p)
{
    પાછા _PyPegen_expect_token(p, STRING);
}

પદાવલિ_ty _PyPegen_soft_keyword_token(Parser *p) {
    Token *t = _PyPegen_expect_token(p, NAME);
    જો (t == ખાલી) {
        પાછા ખાલી;
    }
    અક્ષર *the_token;
    Py_ssize_t size;
    PyBytes_AsStringAndSize(t->bytes, &the_token, &size);
    માટે (અક્ષર **keyword = p->soft_keywords; *keyword != ખાલી; keyword++) {
        જો (strlen(*keyword) == (size_t)size &&
            strncmp(*keyword, the_token, (size_t)size) == 0) {
            પાછા _PyPegen_name_from_token(p, t);
        }
    }
    પાછા ખાલી;
}

સ્થિર PyObject *
parsenumber_raw(અચલ અક્ષર *s)
{
    અચલ અક્ષર *end;
    long x;
    દ્વિગુણ dx;
    Py_complex compl;
    પૂર્ણાંક imflag;

    પ્રતિજ્ઞા(s != ખાલી);
    errno = 0;
    end = s + strlen(s) - 1;
    imflag = *end == 'j' || *end == 'J';
    જો (s[0] == '0') {
        x = (long)PyOS_strtoul(s, (અક્ષર **)&end, 0);
        જો (x < 0 && errno == 0) {
            પાછા PyLong_FromString(s, (અક્ષર **)0, 0);
        }
    }
    નહીંતર {
        x = PyOS_strtol(s, (અક્ષર **)&end, 0);
    }
    જો (*end == '\0') {
        જો (errno != 0) {
            પાછા PyLong_FromString(s, (અક્ષર **)0, 0);
        }
        પાછા PyLong_FromLong(x);
    }
    /* XXX Huge floats may silently fail */
    જો (imflag) {
        compl.real = 0.;
        compl.imag = PyOS_string_to_double(s, (અક્ષર **)&end, ખાલી);
        જો (compl.imag == -1.0 && PyErr_Occurred()) {
            પાછા ખાલી;
        }
        પાછા PyComplex_FromCComplex(compl);
    }
    dx = PyOS_string_to_double(s, ખાલી, ખાલી);
    જો (dx == -1.0 && PyErr_Occurred()) {
        પાછા ખાલી;
    }
    પાછા PyFloat_FromDouble(dx);
}

સ્થિર PyObject *
parsenumber(અચલ અક્ષર *s)
{
    અક્ષર *dup;
    અક્ષર *end;
    PyObject *res = ખાલી;

    પ્રતિજ્ઞા(s != ખાલી);

    જો (strchr(s, '_') == ખાલી) {
        પાછા parsenumber_raw(s);
    }
    /* Create a duplicate without underscores. */
    dup = PyMem_Malloc(strlen(s) + 1);
    જો (dup == ખાલી) {
        પાછા PyErr_NoMemory();
    }
    end = dup;
    માટે (; *s; s++) {
        જો (*s != '_') {
            *end++ = *s;
        }
    }
    *end = '\0';
    res = parsenumber_raw(dup);
    PyMem_Free(dup);
    પાછા res;
}

પદાવલિ_ty
Py_Pegen_સંખ્યા_token(Parser *p)
{
    પૂર્ણાંક mark = p->mark;
    Token *t = _PyPegen_expect_token(p, સંખ્યા);
    જો (t == ખાલી) {
        પાછા ખાલી;
    }

    // Look up the node at the token's start, as for NAME tokens. A memo hit
    // advances the mark; on a miss, restore the position after the token.
    p->mark = mark;
    પદાવલિ_ty cached = ખાલી;
    જો (_PyPegen_is_memoized(p, સંખ્યા, &cached)) {
        પાછા cached;
    }
    p->mark = mark + 1;

    અચલ અક્ષર *અંક_raw = PyBytes_AsString(t->bytes);
    જો (અંક_raw == ખાલી) {
        p->error_indicator = 1;
        પાછા ખાલી;
    }

    જો (p->feature_version < 6 && strchr(અંક_raw, '_') != ખાલી) {
        p->error_indicator = 1;
        પાછા RAISE_SYNTAX_ERROR("Underscores in numeric literals are only supported "
                                  "in Python 3.6 and greater");
    }

    PyObject *c = parsenumber(અંક_raw);

    જો (c == ખાલી) {
        p->error_indicator = 1;
        PyThreadState *tstate = _PyThreadState_GET();
        // The only way a ValueError should happen in _this_ code is via
        // PyLong_FromString hitting a length limit.
        જો (tstate->current_exception != ખાલી &&
            Py_TYPE(tstate->current_exception) == (PyTypeObject *)Py_Exc_કિંમત_Error
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
        પાછા ખાલી;
    }

    જો (_PyArena_AddPyObject(p->arena, c) < 0) {
        Py_DECREF(c);
        p->error_indicator = 1;
        પાછા ખાલી;
    }

    પદાવલિ_ty result = _PyAST_Constant(c, ખાલી, t->lineno, t->col_offset,
                                    t->end_lineno, t->end_col_offset, p->arena);
    જો (result != ખાલી && Py_Pegen_ઉમેરો_memo(p, mark, સંખ્યા, result) < 0) {
        p->error_indicator = 1;
        પાછા ખાલી;
    }
    પાછા result;
}



સ્થિર પૂર્ણાંક
compute_parser_flags(PyCompilerFlags *flags)
{
    પૂર્ણાંક parser_flags = 0;
    જો (!flags) {
        પાછા 0;
    }
    જો (flags->cf_flags & PyCF_DONT_IMPLY_DEDENT) {
        parser_flags |= PyPARSE_DONT_IMPLY_DEDENT;
    }
    જો (flags->cf_flags & PyCF_IGNORE_COOKIE) {
        parser_flags |= PyPARSE_IGNORE_COOKIE;
    }
    જો (flags->cf_flags & CO_FUTURE_BARRY_AS_BDFL) {
        parser_flags |= PyPARSE_BARRY_AS_BDFL;
    }
    જો (flags->cf_flags & PyCF_TYPE_COMMENTS) {
        parser_flags |= PyPARSE_TYPE_COMMENTS;
    }
    જો (flags->cf_flags & Py_CF_ALLOW_INCOMPLETE_આગત) {
        parser_flags |= Py_PARSE_ALLOW_INCOMPLETE_આગત;
    }
    પાછા parser_flags;
}

// Parser API

Parser *
_PyPegen_Parser_New(સંરચના tok_state *tok, પૂર્ણાંક start_rule, પૂર્ણાંક flags,
                    પૂર્ણાંક feature_version, પૂર્ણાંક *errcode, અચલ અક્ષર* source, PyArena *arena)
{
    Parser *p = PyMem_Malloc(કદ(Parser));
    જો (p == ખાલી) {
        પાછા (Parser *) PyErr_NoMemory();
    }
    પ્રતિજ્ઞા(tok != ખાલી);
    _PyTokenizer_SetOptions(tok, 0, (flags & PyPARSE_TYPE_COMMENTS) > 0);
    Py_Tokenizer_Allow_Incomplete_આગત(tok, flags & Py_PARSE_ALLOW_INCOMPLETE_આગત);
    p->tok = tok;
    p->keywords = ખાલી;
    p->n_keyword_lists = -1;
    p->soft_keywords = ખાલી;
    p->tokens = PyMem_Malloc(કદ(Token *));
    જો (!p->tokens) {
        PyMem_Free(p);
        પાછા (Parser *) PyErr_NoMemory();
    }
    p->tokens[0] = PyMem_Calloc(1, કદ(Token));
    જો (!p->tokens[0]) {
        PyMem_Free(p->tokens);
        PyMem_Free(p);
        પાછા (Parser *) PyErr_NoMemory();
    }
    p->type_ignore_comments = (growable_comment_array){0};

    p->mark = 0;
    p->fill = 0;
    p->size = 1;

    p->errcode = errcode;
    p->arena = arena;
    p->start_rule = start_rule;
    p->parsing_started = 0;
    p->normalize = ખાલી;
    p->error_indicator = 0;

    p->starting_lineno = 0;
    p->starting_col_offset = 0;
    p->flags = flags;
    p->feature_version = feature_version;
    p->known_err_token = ખાલી;
    p->identifier_cache = ખાલી;
    p->tstate = PyThreadState_Get();
    // Stack limits are initialized when the thread state is attached.
    પ્રતિજ્ઞા(((_PyThreadStateImpl *)p->tstate)->c_ઢગલો_hard_limit != 0);
    p->ઢગલો_soft_limit = ((_PyThreadStateImpl *)p->tstate)->c_ઢગલો_soft_limit;
#જો Py_ઢગલો_GROWS_DOWN
    p->ઢગલો_soft_limit += Py_OS_ઢગલો_MARGIN_BYTES;
#નહીંતર
    p->ઢગલો_soft_limit -= Py_OS_ઢગલો_MARGIN_BYTES;
#endif
    p->level = 0;
    p->call_invalid_rules = 0;
    p->આગત_exhausted = 0;
    p->last_stmt_location.lineno = 0;
    p->last_stmt_location.col_offset = 0;
    p->last_stmt_location.end_lineno = 0;
    p->last_stmt_location.end_col_offset = 0;
#ifdef Py_DEBUG
    p->debug = _Py_GetConfig()->parser_debug;
#endif
    પાછા p;
}

શૂન્ય
_PyPegen_Parser_Free(Parser *p)
{
    PyMem_Free(p->identifier_cache);
    Py_XDECREF(p->normalize);
    // Resizes allocate blocks starting at indices 1, 2, 4, and so on.
    PyMem_Free(p->tokens[0]);
    માટે (પૂર્ણાંક i = 1; i < p->size; i *= PEGEN_ARRAY_GROWTH_FACTOR) {
        PyMem_Free(p->tokens[i]);
    }
    PyMem_Free(p->tokens);
    growable_comment_array_deallocate(&p->type_ignore_comments);
    PyMem_Free(p);
}

સ્થિર શૂન્ય
reset_parser_state_for_error_pass(Parser *p)
{
    p->last_stmt_location.lineno = 0;
    p->last_stmt_location.col_offset = 0;
    p->last_stmt_location.end_lineno = 0;
    p->last_stmt_location.end_col_offset = 0;
    માટે (પૂર્ણાંક i = 0; i < p->fill; i++) {
        p->tokens[i]->memo = ખાલી;
        p->tokens[i]->memo_mask = 0;
    }
    p->mark = 0;
    p->flags &= ~Py_PARSE_ALLOW_INCOMPLETE_આગત;
    Py_Tokenizer_Allow_Incomplete_આગત(p->tok, 0);
    p->call_invalid_rules = 1;
    _PyTokenizer_StopInteractive(p->tok);
}

સ્થિર શૂન્ય
_PyPegen_set_syntax_error_metadata(Parser *p) {
    PyObject *exc = PyErr_GetRaisedException();
    જો (!exc || !PyObject_TypeCheck(exc, (PyTypeObject *)PyExc_SyntaxError)) {
        PyErr_SetRaisedException(exc);
        પાછા;
    }
    અચલ અક્ષર *source = _PyTokenizer_RetainedSource(p->tok);
    અચલ અક્ષર *encoding = _PyTokenizer_GetInfo(p->tok).encoding;
    PyObject* the_source = ખાલી;
    જો (source) {
        જો (encoding == ખાલી) {
            the_source = PyUnicode_FromString(source);
        } નહીંતર {
            the_source = PyUnicode_Decode(source, strlen(source), encoding, ખાલી);
        }
    }
    જો (!the_source) {
        PyErr_Clear();
        the_source = Py_None;
        Py_INCREF(the_source);
    }
    PyObject* metadata = Py_Build_કિંમત(
        "(iiN)",
        p->last_stmt_location.lineno,
        p->last_stmt_location.col_offset,
        the_source // N gives ownership to metadata
    );
    જો (!metadata) {
        PyErr_Clear();
        PyErr_SetRaisedException(exc);
        પાછા;
    }
    PySyntaxErrorObject *syntax_error = (PySyntaxErrorObject *)exc;

    Py_XDECREF(syntax_error->metadata);
    syntax_error->metadata = metadata;
    PyErr_SetRaisedException(exc);
}

શૂન્ય *
_PyPegen_run_parser(Parser *p)
{
    શૂન્ય *res = _PyPegen_parse(p);
    પ્રતિજ્ઞા(p->level == 0);
    જો (res != ખાલી && PyErr_Occurred()) {
        // Discard a result returned with an exception still pending
        // (e.g. a MemoryError from a recovered-from allocation failure).
        પાછા ખાલી;
    }
    જો (res == ખાલી) {
        જો (PyErr_Occurred() &&
            (PyErr_ExceptionMatches(Py_Exc_Incomplete_આગત_Error) ||
             !PyErr_ExceptionMatches(PyExc_SyntaxError))) {
            પાછા ખાલી;
        }
        જો ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_આગત) &&
            p->આગત_exhausted && !PyErr_Occurred()) {
            પાછા Py_Pegen_raise_incomplete_આગત(p);
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
        જો (PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_set_syntax_error_metadata(p);
        }
       પાછા ખાલી;
    }

    જો (p->start_rule == Py_single_આગત && _PyTokenizer_HasTrailingStatement(p->tok)) {
        પાછા RAISE_SYNTAX_ERROR("multiple statements found while compiling a single statement");
    }

    // test_peg_generator defines _Py_TEST_PEGEN to not call PyAST_Validate()
#જો defined(Py_DEBUG) && !defined(_Py_TEST_PEGEN)
    જો (p->start_rule == Py_single_આગત ||
        p->start_rule == Py_file_આગત ||
        p->start_rule == Py_eval_આગત)
    {
        જો (!_PyAST_Validate(res)) {
            પાછા ખાલી;
        }
    }
#endif
    પાછા res;
}

mod_ty
_PyPegen_run_parser_from_file_pointer(FILE *fp, પૂર્ણાંક start_rule, PyObject *filename_ob,
                             અચલ અક્ષર *enc, અચલ અક્ષર *ps1, અચલ અક્ષર *ps2,
                             PyCompilerFlags *flags, પૂર્ણાંક *errcode,
                             PyObject **interactive_src, PyArena *arena)
{
    પૂર્ણાંક parser_flags = compute_parser_flags(flags);
    સંરચના tok_state *tok = _PyTokenizer_FromFile(fp, enc, ps1, ps2);
    જો (tok == ખાલી) {
        જો (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        નહીંતર {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        પાછા ખાલી;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    // From here on we need to clean up even if there's an error
    mod_ty result = ખાલી;

    PyObject *module = PyUnicode_FromString("__main__");
    જો (module == ખાલી) {
        જાઓ error;
    }
    _PyTokenizer_SetContext(tok, filename_ob, module);
    Py_DECREF(module);

    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, PY_MINOR_VERSION,
                                    errcode, ખાલી, arena);
    જો (p == ખાલી) {
        જાઓ error;
    }

    result = _PyPegen_run_parser(p);
    _PyPegen_Parser_Free(p);

    અચલ અક્ષર *source = _PyTokenizer_RetainedSource(tok);
    જો (source != ખાલી && result && interactive_src != ખાલી) {
        *interactive_src = PyUnicode_FromString(source);
        જો (!*interactive_src || _PyArena_AddPyObject(arena, *interactive_src) < 0) {
            Py_XDECREF(*interactive_src);
            result = ખાલી;
            જાઓ error;
        }
    }

error:
    _PyTokenizer_Free(tok);
    પાછા result;
}

mod_ty
_PyPegen_run_parser_from_string(અચલ અક્ષર *str, પૂર્ણાંક start_rule, PyObject *filename_ob,
                       PyCompilerFlags *flags, PyArena *arena, PyObject *module,
                       PyObject **incomplete)
{
    _PyTokenizer_Newline newline_mode = start_rule == Py_file_આગત
        ? _PyTokenizer_IMPLY_NEWLINE : _PyTokenizer_KEEP_NEWLINE;
    પૂર્ણાંક parser_flags = compute_parser_flags(flags);
    જો (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_આગત) {
        newline_mode = _PyTokenizer_APPEND_NEWLINE;
    }

    સંરચના tok_state *tok;
    જો (flags != ખાલી && flags->cf_flags & PyCF_IGNORE_COOKIE) {
        tok = _PyTokenizer_FromUTF8(str, newline_mode, 0);
    } નહીંતર {
        tok = _PyTokenizer_FromString(str, newline_mode, 0);
    }
    જો (tok == ખાલી) {
        જો (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        નહીંતર {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        પાછા ખાલી;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    _PyTokenizer_SetContext(tok, filename_ob, module);

    // We need to clear up from here on
    mod_ty result = ખાલી;

    પૂર્ણાંક feature_version = flags && (flags->cf_flags & PyCF_ONLY_AST) ?
        flags->cf_feature_version : PY_MINOR_VERSION;
    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, feature_version,
                                    ખાલી, str, arena);
    જો (p == ખાલી) {
        જાઓ error;
    }

    result = _PyPegen_run_parser(p);
    જો (result != ખાલી && (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_આગત)) {
        પૂર્ણાંક needs_newline = _PyTokenizer_GetNewlineRequirements(tok);
        જો ((needs_newline & _PyTokenizer_NEWLINE_CONTINUATION) ||
            (start_rule != Py_file_આગત &&
             (needs_newline & _PyTokenizer_NEWLINE_INDENT)) ||
            (start_rule == Py_single_આગત && p->આગત_exhausted &&
             (needs_newline & _PyTokenizer_NEWLINE_STATEMENT))) {
            Py_Pegen_raise_incomplete_આગત(p);
            જો (incomplete != ખાલી && PyErr_ExceptionMatches(Py_Exc_Incomplete_આગત_Error)) {
                *incomplete = PyErr_GetRaisedException();
            }
            નહીંતર {
                result = ખાલી;
            }
        }
    }
    _PyPegen_Parser_Free(p);

error:
    _PyTokenizer_Free(tok);
    પાછા result;
}
