#ಅನ್ತರ್ಭಾವ <Python.h>
#ಅನ್ತರ್ಭಾವ "pycore_ast.h"           // _PyAST_Validate(),
#ಅನ್ತರ್ಭಾವ "pycore_pystate.h"       // _PyThreadState_GET()
#ಅನ್ತರ್ಭಾವ "pycore_parser.h"        // _PYPEGEN_NSTATISTICS
#ಅನ್ತರ್ಭಾವ "pycore_pyerrors.h"      // PyExc_IncompleteInputError
#ಅನ್ತರ್ಭಾವ "pycore_runtime.h"       // _PyRuntime
#ಅನ್ತರ್ಭಾವ "pycore_unicodeobject.h" // _PyUnicode_InternImmortal
#ಅನ್ತರ್ಭಾವ <errcode.h>

#ಅನ್ತರ್ಭಾವ "lexer/state.h"
#ಅನ್ತರ್ಭಾವ "tokenizer/tokenizer.h"
#ಅನ್ತರ್ಭಾವ "pegen.h"

#define IDENTIFIER_CACHE_SIZE 2048  // Must be a power of two.
#define IDENTIFIER_CACHE_MAX_PROBES 8
#define PEGEN_ARRAY_GROWTH_FACTOR 2
#define TYPE_IGNORE_INITIAL_CAPACITY 10

ಸಂರಚನಾ _identifier_cache_entry {
    ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ;   // Borrowed from arena-owned token bytes.
    Py_ssize_t len;
    Py_hash_t hash;
    PyObject *ಮೌಲ್ಯ;   // Borrowed from an arena-owned identifier.
};

// Internal parser functions

asdl_stmt_seq*
_PyPegen_interactive_exit(Parser *p)
{
    ಯದಿ (p->errcode) {
        *(p->errcode) = E_EOF;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

Py_ssize_t
Py_Pegen_byte_offset_to_character_offset_ಸಾಲು(PyObject *ಸಾಲು, Py_ssize_t col_offset, Py_ssize_t end_col_offset)
{
    ಧ್ರುವ unsigned ವರ್ಣ *data = (ಧ್ರುವ unsigned ವರ್ಣ*)PyUnicode_AsUTF8(ಸಾಲು);
    ಯದಿ (data == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }

    Py_ssize_t len = 0;
    ಯಾವತ್ (col_offset < end_col_offset) {
        Py_UCS4 ch = data[col_offset];
        ಯದಿ (ch < 0x80) {
            col_offset += 1;
        } ಅನ್ಯಥಾ ಯದಿ ((ch & 0xe0) == 0xc0) {
            col_offset += 2;
        } ಅನ್ಯಥಾ ಯದಿ ((ch & 0xf0) == 0xe0) {
            col_offset += 3;
        } ಅನ್ಯಥಾ ಯದಿ ((ch & 0xf8) == 0xf0) {
            col_offset += 4;
        } ಅನ್ಯಥಾ {
            PyErr_SetString(Py_Exc_ಮೌಲ್ಯ_Error, "Invalid UTF-8 sequence");
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        len++;
    }
    ಪ್ರತ್ಯಾಗಮ len;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset_raw(ಧ್ರುವ ವರ್ಣ* str, Py_ssize_t col_offset)
{
    Py_ssize_t len = (Py_ssize_t)strlen(str);
    ಯದಿ (col_offset > len + 1) {
        col_offset = len + 1;
    }
    ಪ್ರತಿಜ್ಞಾ(col_offset >= 0);
    PyObject *text = PyUnicode_DecodeUTF8(str, col_offset, "replace");
    ಯದಿ (!text) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    Py_ssize_t size = PyUnicode_GET_LENGTH(text);
    Py_DECREF(text);
    ಪ್ರತ್ಯಾಗಮ size;
}

Py_ssize_t
_PyPegen_byte_offset_to_character_offset(PyObject *ಸಾಲು, Py_ssize_t col_offset)
{
    ಧ್ರುವ ವರ್ಣ *str = PyUnicode_AsUTF8(ಸಾಲು);
    ಯದಿ (!str) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಪ್ರತ್ಯಾಗಮ _PyPegen_byte_offset_to_character_offset_raw(str, col_offset);
}

// Here, mark is the start of the node, while p->mark is the end.
// If node==NULL, they should be the same.
ಪೂರ್ಣಾಙ್ಕ
Py_Pegen_ಸೇರಿಸು_memo(Parser *p, ಪೂರ್ಣಾಙ್ಕ mark, ಪೂರ್ಣಾಙ್ಕ type, ಶೂನ್ಯ *ಗಂಟು)
{
    // Insert in front
    Memo *m = _PyArena_Malloc(p->arena, ಪರಿಮಾಣ(Memo));
    ಯದಿ (m == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    m->type = type;
    m->ಗಂಟು = ಗಂಟು;
    m->mark = p->mark;
    m->next = p->tokens[mark]->memo;
    p->tokens[mark]->memo = m;
    p->tokens[mark]->memo_mask |= 1ULL << (type & 63);
    ಪ್ರತ್ಯಾಗಮ 0;
}

// Like _PyPegen_insert_memo(), but returns the inserted Memo so callers
// can update it in place without re-walking the token's memo list.
Memo *
Py_Pegen_ಸೇರಿಸು_memo_direct(Parser *p, ಪೂರ್ಣಾಙ್ಕ mark, ಪೂರ್ಣಾಙ್ಕ type)
{
    ಯದಿ (Py_Pegen_ಸೇರಿಸು_memo(p, mark, type, ರಿಕ್ತ) < 0) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ p->tokens[mark]->memo;
}

// Like _PyPegen_insert_memo(), but updates an existing node if found.
ಪೂರ್ಣಾಙ್ಕ
_PyPegen_update_memo(Parser *p, ಪೂರ್ಣಾಙ್ಕ mark, ಪೂರ್ಣಾಙ್ಕ type, ಶೂನ್ಯ *ಗಂಟು)
{
    ಪ್ರತಿ (Memo *m = p->tokens[mark]->memo; m != ರಿಕ್ತ; m = m->next) {
        ಯದಿ (m->type == type) {
            // Update existing node.
            m->ಗಂಟು = ಗಂಟು;
            m->mark = p->mark;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
    }
    // Insert new node.
    ಪ್ರತ್ಯಾಗಮ Py_Pegen_ಸೇರಿಸು_memo(p, mark, type, ಗಂಟು);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
init_normalization(Parser *p)
{
    ಯದಿ (p->normalize) {
        ಪ್ರತ್ಯಾಗಮ 1;
    }
    p->normalize = PyImport_ImportModuleAttrString("unicodedata", "normalize");
    ಯದಿ (!p->normalize)
    {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ
_PyPegen_grow_loop_buffer(ಶೂನ್ಯ ***buffer, Py_ssize_t *capacity)
{
    ಯದಿ ((size_t)*capacity > (size_t)PY_SSIZE_T_MAX /
            (PEGEN_ARRAY_GROWTH_FACTOR * ಪರಿಮಾಣ(**buffer))) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    Py_ssize_t new_capacity = *capacity == 0
        ? 1 : *capacity * PEGEN_ARRAY_GROWTH_FACTOR;
    ಶೂನ್ಯ **new_buffer = PyMem_Realloc(*buffer, new_capacity * ಪರಿಮಾಣ(**buffer));
    ಯದಿ (new_buffer == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    *buffer = new_buffer;
    *capacity = new_capacity;
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
growable_comment_array_add(growable_comment_array *arr, ಪೂರ್ಣಾಙ್ಕ lineno, ವರ್ಣ *comment)
{
    ಯದಿ (arr->ಅಂಕೆ_ವಸ್ತುಗಳು >= arr->size) {
        ಯದಿ (arr->size > (size_t)PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * ಪರಿಮಾಣ(*arr->ವಸ್ತುಗಳು))) {
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        size_t new_size = arr->size == 0
            ? TYPE_IGNORE_INITIAL_CAPACITY
            : arr->size * PEGEN_ARRAY_GROWTH_FACTOR;
        ಶೂನ್ಯ *new_ವಸ್ತುಗಳು_array = PyMem_Realloc(
            arr->ವಸ್ತುಗಳು, new_size * ಪರಿಮಾಣ(*arr->ವಸ್ತುಗಳು));
        ಯದಿ (!new_ವಸ್ತುಗಳು_array) {
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        arr->ವಸ್ತುಗಳು = new_ವಸ್ತುಗಳು_array;
        arr->size = new_size;
    }

    arr->ವಸ್ತುಗಳು[arr->ಅಂಕೆ_ವಸ್ತುಗಳು].lineno = lineno;
    arr->ವಸ್ತುಗಳು[arr->ಅಂಕೆ_ವಸ್ತುಗಳು].comment = comment;  // Take ownership
    arr->ಅಂಕೆ_ವಸ್ತುಗಳು++;
    ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಶೂನ್ಯ
growable_comment_array_deallocate(growable_comment_array *arr)
{
    ಪ್ರತಿ (size_t i = 0; i < arr->ಅಂಕೆ_ವಸ್ತುಗಳು; i++) {
        PyMem_Free(arr->ವಸ್ತುಗಳು[i].comment);
    }
    PyMem_Free(arr->ವಸ್ತುಗಳು);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
_get_keyword_or_name_type(Parser *p, ಧ್ರುವ ವರ್ಣ *text, Py_ssize_t length)
{
    ಪ್ರತಿಜ್ಞಾ(length > 0);

    ಯದಿ (length >= p->n_keyword_lists ||
        p->keywords[length] == ರಿಕ್ತ ||
        p->keywords[length]->type == -1) {
        ಪ್ರತ್ಯಾಗಮ NAME;
    }
    ಪ್ರತಿ (KeywordToken *k = p->keywords[length]; k != ರಿಕ್ತ && k->type != -1; k++) {
        ಯದಿ (memcmp(k->str, text, (size_t)length) == 0) {
            ಪ್ರತ್ಯಾಗಮ k->type;
        }
    }
    ಪ್ರತ್ಯಾಗಮ NAME;
}

// Keep text only where grammar actions or helpers read it. Keyword error
// actions use fixed spellings, so keywords do not need their text.
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ
token_needs_text(ಪೂರ್ಣಾಙ್ಕ type)
{
    ವಿಕಲ್ಪ (type) {
        ಪಕ್ಷ NAME:
        ಪಕ್ಷ ಸಂಖ್ಯೆ:
        ಪಕ್ಷ STRING:
        ಪಕ್ಷ FSTRING_START:
        ಪಕ್ಷ FSTRING_ಮಧ್ಯ:
        ಪಕ್ಷ FSTRING_END:
        ಪಕ್ಷ TSTRING_START:
        ಪಕ್ಷ TSTRING_ಮಧ್ಯ:
        ಪಕ್ಷ TSTRING_END:
        ಪಕ್ಷ TYPE_COMMENT:
        ಪಕ್ಷ NOTEQUAL:  // _PyPegen_check_barry_as_flufl() distinguishes != and <>.
            ಪ್ರತ್ಯಾಗಮ 1;
        ಸ್ವಭಾವ:
            ಪ್ರತ್ಯಾಗಮ 0;
    }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
initialize_token(Parser *p, Token *parser_token, ಸಂರಚನಾ token *new_token, ಪೂರ್ಣಾಙ್ಕ token_type) {
    ಪ್ರತಿಜ್ಞಾ(parser_token != ರಿಕ್ತ);

    parser_token->type = token_type;
    parser_token->bytes = ರಿಕ್ತ;
    ಯದಿ (token_needs_text(token_type) && new_token->span.start >= 0) {
        Py_ssize_t length;
        ಧ್ರುವ ವರ್ಣ *text = _PyToken_TextView(p->tok, new_token, &length);
        ಯದಿ (token_type == NAME) {
            parser_token->type = _get_keyword_or_name_type(p, text, length);
        }
        // A name reclassified as a keyword does not need its text.
        ಯದಿ (parser_token->type == token_type) {
            parser_token->bytes = PyBytes_FromStringAndSize(text, length);
            ಯದಿ (parser_token->bytes == ರಿಕ್ತ) {
                ಪ್ರತ್ಯಾಗಮ -1;
            }
            ಯದಿ (_PyArena_AddPyObject(p->arena, parser_token->bytes) < 0) {
                Py_DECREF(parser_token->bytes);
                ಪ್ರತ್ಯಾಗಮ -1;
            }
        }
    }

    parser_token->metadata = ರಿಕ್ತ;
    ಯದಿ (new_token->metadata != ರಿಕ್ತ) {
        ಯದಿ (_PyArena_AddPyObject(p->arena, new_token->metadata) < 0) {
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        parser_token->metadata = new_token->metadata;
        new_token->metadata = ರಿಕ್ತ;
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

    ಯದಿ (token_type == ERRORTOKEN && _PyTokenizer_GetStatus(p->tok) == E_DECODE) {
        ಪ್ರತ್ಯಾಗಮ _Pypegen_raise_decode_error(p);
    }

    ಪ್ರತ್ಯಾಗಮ (token_type == ERRORTOKEN ? _Pypegen_tokenizer_error(p) : 0);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
_resize_tokens_array(Parser *p) {
    ಯದಿ (p->size > INT_MAX / PEGEN_ARRAY_GROWTH_FACTOR ||
            (size_t)p->size > PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_FACTOR * ಪರಿಮಾಣ(*p->tokens))) {
        PyErr_NoMemory();
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಪೂರ್ಣಾಙ್ಕ newsize = p->size * PEGEN_ARRAY_GROWTH_FACTOR;
    Token **new_tokens = PyMem_Realloc(p->tokens, (size_t)newsize * ಪರಿಮಾಣ(Token *));
    ಯದಿ (new_tokens == ರಿಕ್ತ) {
        PyErr_NoMemory();
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    p->tokens = new_tokens;

    // Allocate new tokens together without moving existing ones: parser rules
    // may still hold pointers into earlier blocks. Parser_Free frees each block.
    Token *chunk = PyMem_Calloc((size_t)(newsize - p->size), ಪರಿಮಾಣ(Token));
    ಯದಿ (chunk == ರಿಕ್ತ) {
        PyErr_NoMemory();
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = p->size; i < newsize; i++) {
        p->tokens[i] = &chunk[i - p->size];
    }
    p->size = newsize;
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ
_PyPegen_fill_token(Parser *p)
{
    ಸಂರಚನಾ token new_token;
    _PyToken_Init(&new_token);
    _PyTokenizer_Get(p->tok, &new_token);

    // Record and skip '# type: ignore' comments
    ಯಾವತ್ (new_token.type == TYPE_IGNORE) {
        Py_ssize_t len;
        ಧ್ರುವ ವರ್ಣ *text = _PyToken_TextView(p->tok, &new_token, &len);
        ವರ್ಣ *tag = PyMem_Malloc((size_t)len + 1);
        ಯದಿ (tag == ರಿಕ್ತ) {
            PyErr_NoMemory();
            ಗಚ್ಛ error;
        }
        memcpy(tag, text, (size_t)len);
        tag[len] = '\0';
        // Ownership of tag passes to the growable array
        ಯದಿ (!growable_comment_array_add(&p->type_ignore_comments, new_token.end_loc.lineno, tag)) {
            PyMem_Free(tag);
            PyErr_NoMemory();
            ಗಚ್ಛ error;
        }
        _PyTokenizer_Get(p->tok, &new_token);
    }

    ಪೂರ್ಣಾಙ್ಕ type = new_token.type;
    // EOF can finish an ordinary token. Only a boundary token means that
    // the grammar requested input beyond the last physical line.
    ಯದಿ (type == ENDMARKER ||
        (type == DEDENT && (p->flags & Py_PARSE_ALLOW_INCOMPLETE_ಒಳಹರಿವು) &&
         _PyTokenizer_GetStatus(p->tok) == E_EOF)) {
        p->ಒಳಹರಿವು_exhausted = 1;
    }

    // If we have reached the end and we are in single input mode we need to insert a newline and reset the parsing
    ಯದಿ (p->start_rule == Py_single_ಒಳಹರಿವು && type == ENDMARKER && p->parsing_started) {
        type = NEWLINE; /* Add an extra newline */
        p->parsing_started = 0;

        ಯದಿ (!(p->flags & PyPARSE_DONT_IMPLY_DEDENT)) {
            _PyTokenizer_ImplyDedents(p->tok);
        }
    }
    ಅನ್ಯಥಾ {
        p->parsing_started = 1;
    }

    // Check if we are at the limit of the token array capacity and resize if needed
    ಯದಿ ((p->fill == p->size) && (_resize_tokens_array(p) != 0)) {
        ಗಚ್ಛ error;
    }

    Token *t = p->tokens[p->fill];
    ಪೂರ್ಣಾಙ್ಕ result = initialize_token(p, t, &new_token, type);
    _PyToken_Free(&new_token);
    ಪ್ರತ್ಯಾಗಮ result;
error:
    _PyToken_Free(&new_token);
    ಪ್ರತ್ಯಾಗಮ -1;
}

#ಯದಿ defined(Py_DEBUG)
// Instrumentation to count the effectiveness of memoization.
// The array counts the number of tokens skipped by memoization,
// indexed by type.

#define NSTATISTICS _PYPEGEN_NSTATISTICS
#define memo_statistics _PyRuntime.parser.memo_statistics

ಶೂನ್ಯ
_PyPegen_clear_memo_statistics(ಶೂನ್ಯ)
{
    PyMutex_Lock(&_PyRuntime.parser.mutex);
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < NSTATISTICS; i++) {
        memo_statistics[i] = 0;
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
}

PyObject *
_PyPegen_get_memo_statistics(ಶೂನ್ಯ)
{
    PyObject *ret = PyList_New(NSTATISTICS);
    ಯದಿ (ret == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    PyMutex_Lock(&_PyRuntime.parser.mutex);
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < NSTATISTICS; i++) {
        PyObject *ಮೌಲ್ಯ = PyLong_FromLong(memo_statistics[i]);
        ಯದಿ (ಮೌಲ್ಯ == ರಿಕ್ತ) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
        // PyList_SetItem borrows a reference to value.
        ಯದಿ (PyList_SetItem(ret, i, ಮೌಲ್ಯ) < 0) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
    ಪ್ರತ್ಯಾಗಮ ret;
}
#endif

ಪೂರ್ಣಾಙ್ಕ  // bool
_PyPegen_is_memoized(Parser *p, ಪೂರ್ಣಾಙ್ಕ type, ಶೂನ್ಯ *pres)
{
    ಯದಿ (p->mark == p->fill) {
        ಯದಿ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ಪ್ರತ್ಯಾಗಮ -1;
        }
    }

    Token *t = p->tokens[p->mark];

    ಯದಿ (!(t->memo_mask & (1ULL << (type & 63)))) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಪ್ರತಿ (Memo *m = t->memo; m != ರಿಕ್ತ; m = m->next) {
        ಯದಿ (m->type == type) {
#ಯದಿ defined(Py_DEBUG)
            ಯದಿ (0 <= type && type < NSTATISTICS) {
                long ಎಣಿಕೆ = m->mark - p->mark;
                // A memoized negative result counts for one.
                ಯದಿ (ಎಣಿಕೆ <= 0) {
                    ಎಣಿಕೆ = 1;
                }
                PyMutex_Lock(&_PyRuntime.parser.mutex);
                memo_statistics[type] += ಎಣಿಕೆ;
                PyMutex_Unlock(&_PyRuntime.parser.mutex);
            }
#endif
            p->mark = m->mark;
            *(ಶೂನ್ಯ **)(pres) = m->ಗಂಟು;
            ಪ್ರತ್ಯಾಗಮ 1;
        }
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

#define LOOKAHEAD1(NAME, RES_TYPE)                                  \
    ಪೂರ್ಣಾಙ್ಕ                                                             \
    NAME (ಪೂರ್ಣಾಙ್ಕ positive, RES_TYPE (func)(Parser *), Parser *p)       \
    {                                                               \
        ಪೂರ್ಣಾಙ್ಕ mark = p->mark;                                         \
        ಶೂನ್ಯ *res = func(p);                                        \
        p->mark = mark;                                             \
        ಪ್ರತ್ಯಾಗಮ (res != ರಿಕ್ತ) == positive;                           \
    }

LOOKAHEAD1(_PyPegen_lookahead, ಶೂನ್ಯ *)
LOOKAHEAD1(Py_Pegen_lookahead_for_ಅಭಿವ್ಯಕ್ತಿ, ಅಭಿವ್ಯಕ್ತಿ_ty)
LOOKAHEAD1(_PyPegen_lookahead_for_stmt, stmt_ty)
#undef LOOKAHEAD1

#define LOOKAHEAD2(NAME, RES_TYPE, T)                                   \
    ಪೂರ್ಣಾಙ್ಕ                                                                 \
    NAME (ಪೂರ್ಣಾಙ್ಕ positive, RES_TYPE (func)(Parser *, T), Parser *p, T arg) \
    {                                                                   \
        ಪೂರ್ಣಾಙ್ಕ mark = p->mark;                                             \
        ಶೂನ್ಯ *res = func(p, arg);                                       \
        p->mark = mark;                                                 \
        ಪ್ರತ್ಯಾಗಮ (res != ರಿಕ್ತ) == positive;                               \
    }

LOOKAHEAD2(_PyPegen_lookahead_with_int, Token *, ಪೂರ್ಣಾಙ್ಕ)
LOOKAHEAD2(_PyPegen_lookahead_with_string, ಅಭಿವ್ಯಕ್ತಿ_ty, ಧ್ರುವ ವರ್ಣ *)
#undef LOOKAHEAD2

Token *
_PyPegen_expect_token(Parser *p, ಪೂರ್ಣಾಙ್ಕ type)
{
    ಯದಿ (p->mark == p->fill) {
        ಯದಿ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
    }
    Token *t = p->tokens[p->mark];
    ಯದಿ (t->type != type) {
       ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    p->mark += 1;
    ಪ್ರತ್ಯಾಗಮ t;
}

ಶೂನ್ಯ*
_PyPegen_expect_forced_result(Parser *p, ಶೂನ್ಯ* result, ಧ್ರುವ ವರ್ಣ* expected) {

    ಯದಿ (p->error_indicator == 1) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (result == ರಿಕ್ತ) {
        RAISE_SYNTAX_ERROR("expected (%s)", expected);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

Token *
_PyPegen_expect_forced_token(Parser *p, ಪೂರ್ಣಾಙ್ಕ type, ಧ್ರುವ ವರ್ಣ* expected) {

    ಯದಿ (p->error_indicator == 1) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (p->mark == p->fill) {
        ಯದಿ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
    }
    Token *t = p->tokens[p->mark];
    ಯದಿ (t->type != type) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(t, "expected '%s'", expected);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    p->mark += 1;
    ಪ್ರತ್ಯಾಗಮ t;
}

ಅಭಿವ್ಯಕ್ತಿ_ty
_PyPegen_expect_soft_keyword(Parser *p, ಧ್ರುವ ವರ್ಣ *keyword)
{
    ಯದಿ (p->mark == p->fill) {
        ಯದಿ (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
    }
    Token *t = p->tokens[p->mark];
    ಯದಿ (t->type != NAME) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಧ್ರುವ ವರ್ಣ *s = PyBytes_AsString(t->bytes);
    ಯದಿ (!s) {
        p->error_indicator = 1;
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (strcmp(s, keyword) != 0) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ _PyPegen_name_token(p);
}

Token *
_PyPegen_get_last_nonnwhitespace_token(Parser *p)
{
    ಪ್ರತಿಜ್ಞಾ(p->mark >= 0);
    Token *token = ರಿಕ್ತ;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ m = p->mark - 1; m >= 0; m--) {
        token = p->tokens[m];
        ಯದಿ (token->type != ENDMARKER && (token->type < NEWLINE || token->type > DEDENT)) {
            ಭಙ್ಗ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ token;
}

PyObject *
_PyPegen_new_identifier(Parser *p, ಧ್ರುವ ವರ್ಣ *n)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, (Py_ssize_t)strlen(n), ರಿಕ್ತ);
    ಯದಿ (!id) {
        ಗಚ್ಛ error;
    }
    /* Check whether there are non-ASCII characters in the
       identifier; if so, normalize to NFKC. */
    ಯದಿ (!PyUnicode_IS_ASCII(id))
    {
        ಯದಿ (!init_normalization(p))
        {
            Py_DECREF(id);
            ಗಚ್ಛ error;
        }
        PyObject *form = PyUnicode_InternFromString("NFKC");
        ಯದಿ (form == ರಿಕ್ತ)
        {
            Py_DECREF(id);
            ಗಚ್ಛ error;
        }
        PyObject *args[2] = {form, id};
        PyObject *id2 = PyObject_Vectorcall(p->normalize, args, 2, ರಿಕ್ತ);
        Py_DECREF(id);
        Py_DECREF(form);
        ಯದಿ (!id2) {
            ಗಚ್ಛ error;
        }

        ಯದಿ (!PyUnicode_Check(id2))
        {
            PyErr_Format(PyExc_TypeError,
                         "unicodedata.normalize() must return a string, not "
                         "%.200s",
                         _PyType_Name(Py_TYPE(id2)));
            Py_DECREF(id2);
            ಗಚ್ಛ error;
        }
        id = id2;
    }
    ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ forbidden[] = {
        "None",
        "True",
        "False",
        ರಿಕ್ತ
    };
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; forbidden[i] != ರಿಕ್ತ; i++) {
        ಯದಿ (_PyUnicode_EqualToASCIIString(id, forbidden[i])) {
            PyErr_Format(Py_Exc_ಮೌಲ್ಯ_Error,
                         "identifier field can't represent '%s' constant",
                         forbidden[i]);
            Py_DECREF(id);
            ಗಚ್ಛ error;
        }
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &id);
    ಯದಿ (_PyArena_AddPyObject(p->arena, id) < 0)
    {
        Py_DECREF(id);
        ಗಚ್ಛ error;
    }
    ಪ್ರತ್ಯಾಗಮ id;

error:
    p->error_indicator = 1;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

// Return an arena-owned identifier; callers borrow the reference.
ಸ್ಥಿರ PyObject *
get_cached_identifier(Parser *p, PyObject *bytes)
{
    ಧ್ರುವ ವರ್ಣ *s = PyBytes_AsString(bytes);
    ಯದಿ (!s) {
        p->error_indicator = 1;
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    // Identifiers repeat constantly; a small span-keyed cache skips the
    // UTF-8 decode + intern for repeated occurrences. Keys point into
    // arena-owned token bytes and values are arena-owned interned strings,
    // so borrowed references are valid for the lifetime of the parse
    // (including the second error pass, which reuses parser and arena).
    Py_ssize_t len = PyBytes_GET_SIZE(bytes);
    Py_hash_t hash = PyObject_Hash(bytes);
    ಯದಿ (hash == -1) {
        p->error_indicator = 1;
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    // Parses without identifiers do not need this cache.
    ಯದಿ (p->identifier_cache == ರಿಕ್ತ) {
        p->identifier_cache = PyMem_Calloc(
            IDENTIFIER_CACHE_SIZE, ಪರಿಮಾಣ(*p->identifier_cache));
        ಯದಿ (p->identifier_cache == ರಿಕ್ತ) {
            p->error_indicator = 1;
            ಪ್ರತ್ಯಾಗಮ PyErr_NoMemory();
        }
    }
    IdentifierCacheEntry *free_slot = ರಿಕ್ತ;
    size_t idx = (size_t)hash & (IDENTIFIER_CACHE_SIZE - 1);
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ probe = 0; probe < IDENTIFIER_CACHE_MAX_PROBES; probe++) {
        IdentifierCacheEntry *entry = &p->identifier_cache[
            (idx + probe) & (IDENTIFIER_CACHE_SIZE - 1)];
        ಯದಿ (entry->ಕೀಲಿ == ರಿಕ್ತ) {
            free_slot = entry;
            ಭಙ್ಗ;
        }
        ಯದಿ (entry->hash == hash && entry->len == len &&
            memcmp(entry->ಕೀಲಿ, s, len) == 0)
        {
            ಪ್ರತ್ಯಾಗಮ entry->ಮೌಲ್ಯ;
        }
    }
    PyObject *id = _PyPegen_new_identifier(p, s);
    ಯದಿ (id == ರಿಕ್ತ) {
        p->error_indicator = 1;
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (free_slot != ರಿಕ್ತ) {
        free_slot->ಕೀಲಿ = s;
        free_slot->len = len;
        free_slot->hash = hash;
        free_slot->ಮೌಲ್ಯ = id;
    }
    ಪ್ರತ್ಯಾಗಮ id;
}

ಸ್ಥಿರ ಅಭಿವ್ಯಕ್ತಿ_ty
_PyPegen_name_from_token(Parser *p, Token* t)
{
    ಯದಿ (t == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    // Reuse the AST node when backtracking revisits this token. Memo lookup
    // starts before the token and restores the position after it on a hit.
    // Token kinds can be memo keys: generated grammar rule IDs start at 1000.
    ಪೂರ್ಣಾಙ್ಕ mark = p->mark - 1;
    p->mark = mark;
    ಅಭಿವ್ಯಕ್ತಿ_ty cached = ರಿಕ್ತ;
    ಯದಿ (_PyPegen_is_memoized(p, NAME, &cached)) {
        ಪ್ರತ್ಯಾಗಮ cached;
    }
    p->mark = mark + 1;
    PyObject *id = get_cached_identifier(p, t->bytes);
    ಯದಿ (id == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಅಭಿವ್ಯಕ್ತಿ_ty result = _PyAST_Name(id, Load, t->lineno, t->col_offset,
                                t->end_lineno, t->end_col_offset, p->arena);
    ಯದಿ (result != ರಿಕ್ತ && Py_Pegen_ಸೇರಿಸು_memo(p, mark, NAME, result) < 0) {
        p->error_indicator = 1;
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

ಅಭಿವ್ಯಕ್ತಿ_ty
_PyPegen_name_token(Parser *p)
{
    Token *t = _PyPegen_expect_token(p, NAME);
    ಪ್ರತ್ಯಾಗಮ _PyPegen_name_from_token(p, t);
}

ಶೂನ್ಯ *
_PyPegen_string_token(Parser *p)
{
    ಪ್ರತ್ಯಾಗಮ _PyPegen_expect_token(p, STRING);
}

ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_soft_keyword_token(Parser *p) {
    Token *t = _PyPegen_expect_token(p, NAME);
    ಯದಿ (t == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ವರ್ಣ *the_token;
    Py_ssize_t size;
    PyBytes_AsStringAndSize(t->bytes, &the_token, &size);
    ಪ್ರತಿ (ವರ್ಣ **keyword = p->soft_keywords; *keyword != ರಿಕ್ತ; keyword++) {
        ಯದಿ (strlen(*keyword) == (size_t)size &&
            strncmp(*keyword, the_token, (size_t)size) == 0) {
            ಪ್ರತ್ಯಾಗಮ _PyPegen_name_from_token(p, t);
        }
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ PyObject *
parsenumber_raw(ಧ್ರುವ ವರ್ಣ *s)
{
    ಧ್ರುವ ವರ್ಣ *end;
    long x;
    ದ್ವಿಗುಣ dx;
    Py_complex compl;
    ಪೂರ್ಣಾಙ್ಕ imflag;

    ಪ್ರತಿಜ್ಞಾ(s != ರಿಕ್ತ);
    errno = 0;
    end = s + strlen(s) - 1;
    imflag = *end == 'j' || *end == 'J';
    ಯದಿ (s[0] == '0') {
        x = (long)PyOS_strtoul(s, (ವರ್ಣ **)&end, 0);
        ಯದಿ (x < 0 && errno == 0) {
            ಪ್ರತ್ಯಾಗಮ PyLong_FromString(s, (ವರ್ಣ **)0, 0);
        }
    }
    ಅನ್ಯಥಾ {
        x = PyOS_strtol(s, (ವರ್ಣ **)&end, 0);
    }
    ಯದಿ (*end == '\0') {
        ಯದಿ (errno != 0) {
            ಪ್ರತ್ಯಾಗಮ PyLong_FromString(s, (ವರ್ಣ **)0, 0);
        }
        ಪ್ರತ್ಯಾಗಮ PyLong_FromLong(x);
    }
    /* XXX Huge floats may silently fail */
    ಯದಿ (imflag) {
        compl.real = 0.;
        compl.imag = PyOS_string_to_double(s, (ವರ್ಣ **)&end, ರಿಕ್ತ);
        ಯದಿ (compl.imag == -1.0 && PyErr_Occurred()) {
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
        ಪ್ರತ್ಯಾಗಮ PyComplex_FromCComplex(compl);
    }
    dx = PyOS_string_to_double(s, ರಿಕ್ತ, ರಿಕ್ತ);
    ಯದಿ (dx == -1.0 && PyErr_Occurred()) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ PyFloat_FromDouble(dx);
}

ಸ್ಥಿರ PyObject *
parsenumber(ಧ್ರುವ ವರ್ಣ *s)
{
    ವರ್ಣ *dup;
    ವರ್ಣ *end;
    PyObject *res = ರಿಕ್ತ;

    ಪ್ರತಿಜ್ಞಾ(s != ರಿಕ್ತ);

    ಯದಿ (strchr(s, '_') == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ parsenumber_raw(s);
    }
    /* Create a duplicate without underscores. */
    dup = PyMem_Malloc(strlen(s) + 1);
    ಯದಿ (dup == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ PyErr_NoMemory();
    }
    end = dup;
    ಪ್ರತಿ (; *s; s++) {
        ಯದಿ (*s != '_') {
            *end++ = *s;
        }
    }
    *end = '\0';
    res = parsenumber_raw(dup);
    PyMem_Free(dup);
    ಪ್ರತ್ಯಾಗಮ res;
}

ಅಭಿವ್ಯಕ್ತಿ_ty
Py_Pegen_ಸಂಖ್ಯೆ_token(Parser *p)
{
    ಪೂರ್ಣಾಙ್ಕ mark = p->mark;
    Token *t = _PyPegen_expect_token(p, ಸಂಖ್ಯೆ);
    ಯದಿ (t == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // Look up the node at the token's start, as for NAME tokens. A memo hit
    // advances the mark; on a miss, restore the position after the token.
    p->mark = mark;
    ಅಭಿವ್ಯಕ್ತಿ_ty cached = ರಿಕ್ತ;
    ಯದಿ (_PyPegen_is_memoized(p, ಸಂಖ್ಯೆ, &cached)) {
        ಪ್ರತ್ಯಾಗಮ cached;
    }
    p->mark = mark + 1;

    ಧ್ರುವ ವರ್ಣ *ಅಂಕೆ_raw = PyBytes_AsString(t->bytes);
    ಯದಿ (ಅಂಕೆ_raw == ರಿಕ್ತ) {
        p->error_indicator = 1;
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (p->feature_version < 6 && strchr(ಅಂಕೆ_raw, '_') != ರಿಕ್ತ) {
        p->error_indicator = 1;
        ಪ್ರತ್ಯಾಗಮ RAISE_SYNTAX_ERROR("Underscores in numeric literals are only supported "
                                  "in Python 3.6 and greater");
    }

    PyObject *c = parsenumber(ಅಂಕೆ_raw);

    ಯದಿ (c == ರಿಕ್ತ) {
        p->error_indicator = 1;
        PyThreadState *tstate = _PyThreadState_GET();
        // The only way a ValueError should happen in _this_ code is via
        // PyLong_FromString hitting a length limit.
        ಯದಿ (tstate->current_exception != ರಿಕ್ತ &&
            Py_TYPE(tstate->current_exception) == (PyTypeObject *)Py_Exc_ಮೌಲ್ಯ_Error
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
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (_PyArena_AddPyObject(p->arena, c) < 0) {
        Py_DECREF(c);
        p->error_indicator = 1;
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಅಭಿವ್ಯಕ್ತಿ_ty result = _PyAST_Constant(c, ರಿಕ್ತ, t->lineno, t->col_offset,
                                    t->end_lineno, t->end_col_offset, p->arena);
    ಯದಿ (result != ರಿಕ್ತ && Py_Pegen_ಸೇರಿಸು_memo(p, mark, ಸಂಖ್ಯೆ, result) < 0) {
        p->error_indicator = 1;
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ result;
}



ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compute_parser_flags(PyCompilerFlags *flags)
{
    ಪೂರ್ಣಾಙ್ಕ parser_flags = 0;
    ಯದಿ (!flags) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ (flags->cf_flags & PyCF_DONT_IMPLY_DEDENT) {
        parser_flags |= PyPARSE_DONT_IMPLY_DEDENT;
    }
    ಯದಿ (flags->cf_flags & PyCF_IGNORE_COOKIE) {
        parser_flags |= PyPARSE_IGNORE_COOKIE;
    }
    ಯದಿ (flags->cf_flags & CO_FUTURE_BARRY_AS_BDFL) {
        parser_flags |= PyPARSE_BARRY_AS_BDFL;
    }
    ಯದಿ (flags->cf_flags & PyCF_TYPE_COMMENTS) {
        parser_flags |= PyPARSE_TYPE_COMMENTS;
    }
    ಯದಿ (flags->cf_flags & Py_CF_ALLOW_INCOMPLETE_ಒಳಹರಿವು) {
        parser_flags |= Py_PARSE_ALLOW_INCOMPLETE_ಒಳಹರಿವು;
    }
    ಪ್ರತ್ಯಾಗಮ parser_flags;
}

// Parser API

Parser *
_PyPegen_Parser_New(ಸಂರಚನಾ tok_state *tok, ಪೂರ್ಣಾಙ್ಕ start_rule, ಪೂರ್ಣಾಙ್ಕ flags,
                    ಪೂರ್ಣಾಙ್ಕ feature_version, ಪೂರ್ಣಾಙ್ಕ *errcode, ಧ್ರುವ ವರ್ಣ* source, PyArena *arena)
{
    Parser *p = PyMem_Malloc(ಪರಿಮಾಣ(Parser));
    ಯದಿ (p == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ (Parser *) PyErr_NoMemory();
    }
    ಪ್ರತಿಜ್ಞಾ(tok != ರಿಕ್ತ);
    _PyTokenizer_SetOptions(tok, 0, (flags & PyPARSE_TYPE_COMMENTS) > 0);
    Py_Tokenizer_Allow_Incomplete_ಒಳಹರಿವು(tok, flags & Py_PARSE_ALLOW_INCOMPLETE_ಒಳಹರಿವು);
    p->tok = tok;
    p->keywords = ರಿಕ್ತ;
    p->n_keyword_lists = -1;
    p->soft_keywords = ರಿಕ್ತ;
    p->tokens = PyMem_Malloc(ಪರಿಮಾಣ(Token *));
    ಯದಿ (!p->tokens) {
        PyMem_Free(p);
        ಪ್ರತ್ಯಾಗಮ (Parser *) PyErr_NoMemory();
    }
    p->tokens[0] = PyMem_Calloc(1, ಪರಿಮಾಣ(Token));
    ಯದಿ (!p->tokens[0]) {
        PyMem_Free(p->tokens);
        PyMem_Free(p);
        ಪ್ರತ್ಯಾಗಮ (Parser *) PyErr_NoMemory();
    }
    p->type_ignore_comments = (growable_comment_array){0};

    p->mark = 0;
    p->fill = 0;
    p->size = 1;

    p->errcode = errcode;
    p->arena = arena;
    p->start_rule = start_rule;
    p->parsing_started = 0;
    p->normalize = ರಿಕ್ತ;
    p->error_indicator = 0;

    p->starting_lineno = 0;
    p->starting_col_offset = 0;
    p->flags = flags;
    p->feature_version = feature_version;
    p->known_err_token = ರಿಕ್ತ;
    p->identifier_cache = ರಿಕ್ತ;
    p->tstate = PyThreadState_Get();
    // Stack limits are initialized when the thread state is attached.
    ಪ್ರತಿಜ್ಞಾ(((_PyThreadStateImpl *)p->tstate)->c_ರಾಶಿ_hard_limit != 0);
    p->ರಾಶಿ_soft_limit = ((_PyThreadStateImpl *)p->tstate)->c_ರಾಶಿ_soft_limit;
#ಯದಿ Py_ರಾಶಿ_GROWS_DOWN
    p->ರಾಶಿ_soft_limit += Py_OS_ರಾಶಿ_MARGIN_BYTES;
#ಅನ್ಯಥಾ
    p->ರಾಶಿ_soft_limit -= Py_OS_ರಾಶಿ_MARGIN_BYTES;
#endif
    p->level = 0;
    p->call_invalid_rules = 0;
    p->ಒಳಹರಿವು_exhausted = 0;
    p->last_stmt_location.lineno = 0;
    p->last_stmt_location.col_offset = 0;
    p->last_stmt_location.end_lineno = 0;
    p->last_stmt_location.end_col_offset = 0;
#ifdef Py_DEBUG
    p->debug = _Py_GetConfig()->parser_debug;
#endif
    ಪ್ರತ್ಯಾಗಮ p;
}

ಶೂನ್ಯ
_PyPegen_Parser_Free(Parser *p)
{
    PyMem_Free(p->identifier_cache);
    Py_XDECREF(p->normalize);
    // Resizes allocate blocks starting at indices 1, 2, 4, and so on.
    PyMem_Free(p->tokens[0]);
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < p->size; i *= PEGEN_ARRAY_GROWTH_FACTOR) {
        PyMem_Free(p->tokens[i]);
    }
    PyMem_Free(p->tokens);
    growable_comment_array_deallocate(&p->type_ignore_comments);
    PyMem_Free(p);
}

ಸ್ಥಿರ ಶೂನ್ಯ
reset_parser_state_for_error_pass(Parser *p)
{
    p->last_stmt_location.lineno = 0;
    p->last_stmt_location.col_offset = 0;
    p->last_stmt_location.end_lineno = 0;
    p->last_stmt_location.end_col_offset = 0;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < p->fill; i++) {
        p->tokens[i]->memo = ರಿಕ್ತ;
        p->tokens[i]->memo_mask = 0;
    }
    p->mark = 0;
    p->flags &= ~Py_PARSE_ALLOW_INCOMPLETE_ಒಳಹರಿವು;
    Py_Tokenizer_Allow_Incomplete_ಒಳಹರಿವು(p->tok, 0);
    p->call_invalid_rules = 1;
    _PyTokenizer_StopInteractive(p->tok);
}

ಸ್ಥಿರ ಶೂನ್ಯ
_PyPegen_set_syntax_error_metadata(Parser *p) {
    PyObject *exc = PyErr_GetRaisedException();
    ಯದಿ (!exc || !PyObject_TypeCheck(exc, (PyTypeObject *)PyExc_SyntaxError)) {
        PyErr_SetRaisedException(exc);
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಧ್ರುವ ವರ್ಣ *source = _PyTokenizer_RetainedSource(p->tok);
    ಧ್ರುವ ವರ್ಣ *encoding = _PyTokenizer_GetInfo(p->tok).encoding;
    PyObject* the_source = ರಿಕ್ತ;
    ಯದಿ (source) {
        ಯದಿ (encoding == ರಿಕ್ತ) {
            the_source = PyUnicode_FromString(source);
        } ಅನ್ಯಥಾ {
            the_source = PyUnicode_Decode(source, strlen(source), encoding, ರಿಕ್ತ);
        }
    }
    ಯದಿ (!the_source) {
        PyErr_Clear();
        the_source = Py_None;
        Py_INCREF(the_source);
    }
    PyObject* metadata = Py_Build_ಮೌಲ್ಯ(
        "(iiN)",
        p->last_stmt_location.lineno,
        p->last_stmt_location.col_offset,
        the_source // N gives ownership to metadata
    );
    ಯದಿ (!metadata) {
        PyErr_Clear();
        PyErr_SetRaisedException(exc);
        ಪ್ರತ್ಯಾಗಮ;
    }
    PySyntaxErrorObject *syntax_error = (PySyntaxErrorObject *)exc;

    Py_XDECREF(syntax_error->metadata);
    syntax_error->metadata = metadata;
    PyErr_SetRaisedException(exc);
}

ಶೂನ್ಯ *
_PyPegen_run_parser(Parser *p)
{
    ಶೂನ್ಯ *res = _PyPegen_parse(p);
    ಪ್ರತಿಜ್ಞಾ(p->level == 0);
    ಯದಿ (res != ರಿಕ್ತ && PyErr_Occurred()) {
        // Discard a result returned with an exception still pending
        // (e.g. a MemoryError from a recovered-from allocation failure).
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (res == ರಿಕ್ತ) {
        ಯದಿ (PyErr_Occurred() &&
            (PyErr_ExceptionMatches(Py_Exc_Incomplete_ಒಳಹರಿವು_Error) ||
             !PyErr_ExceptionMatches(PyExc_SyntaxError))) {
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
        ಯದಿ ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_ಒಳಹರಿವು) &&
            p->ಒಳಹರಿವು_exhausted && !PyErr_Occurred()) {
            ಪ್ರತ್ಯಾಗಮ Py_Pegen_raise_incomplete_ಒಳಹರಿವು(p);
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
        ಯದಿ (PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_set_syntax_error_metadata(p);
        }
       ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (p->start_rule == Py_single_ಒಳಹರಿವು && _PyTokenizer_HasTrailingStatement(p->tok)) {
        ಪ್ರತ್ಯಾಗಮ RAISE_SYNTAX_ERROR("multiple statements found while compiling a single statement");
    }

    // test_peg_generator defines _Py_TEST_PEGEN to not call PyAST_Validate()
#ಯದಿ defined(Py_DEBUG) && !defined(_Py_TEST_PEGEN)
    ಯದಿ (p->start_rule == Py_single_ಒಳಹರಿವು ||
        p->start_rule == Py_file_ಒಳಹರಿವು ||
        p->start_rule == Py_eval_ಒಳಹರಿವು)
    {
        ಯದಿ (!_PyAST_Validate(res)) {
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
    }
#endif
    ಪ್ರತ್ಯಾಗಮ res;
}

mod_ty
_PyPegen_run_parser_from_file_pointer(FILE *fp, ಪೂರ್ಣಾಙ್ಕ start_rule, PyObject *filename_ob,
                             ಧ್ರುವ ವರ್ಣ *enc, ಧ್ರುವ ವರ್ಣ *ps1, ಧ್ರುವ ವರ್ಣ *ps2,
                             PyCompilerFlags *flags, ಪೂರ್ಣಾಙ್ಕ *errcode,
                             PyObject **interactive_src, PyArena *arena)
{
    ಪೂರ್ಣಾಙ್ಕ parser_flags = compute_parser_flags(flags);
    ಸಂರಚನಾ tok_state *tok = _PyTokenizer_FromFile(fp, enc, ps1, ps2);
    ಯದಿ (tok == ರಿಕ್ತ) {
        ಯದಿ (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        ಅನ್ಯಥಾ {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    // From here on we need to clean up even if there's an error
    mod_ty result = ರಿಕ್ತ;

    PyObject *module = PyUnicode_FromString("__main__");
    ಯದಿ (module == ರಿಕ್ತ) {
        ಗಚ್ಛ error;
    }
    _PyTokenizer_SetContext(tok, filename_ob, module);
    Py_DECREF(module);

    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, PY_MINOR_VERSION,
                                    errcode, ರಿಕ್ತ, arena);
    ಯದಿ (p == ರಿಕ್ತ) {
        ಗಚ್ಛ error;
    }

    result = _PyPegen_run_parser(p);
    _PyPegen_Parser_Free(p);

    ಧ್ರುವ ವರ್ಣ *source = _PyTokenizer_RetainedSource(tok);
    ಯದಿ (source != ರಿಕ್ತ && result && interactive_src != ರಿಕ್ತ) {
        *interactive_src = PyUnicode_FromString(source);
        ಯದಿ (!*interactive_src || _PyArena_AddPyObject(arena, *interactive_src) < 0) {
            Py_XDECREF(*interactive_src);
            result = ರಿಕ್ತ;
            ಗಚ್ಛ error;
        }
    }

error:
    _PyTokenizer_Free(tok);
    ಪ್ರತ್ಯಾಗಮ result;
}

mod_ty
_PyPegen_run_parser_from_string(ಧ್ರುವ ವರ್ಣ *str, ಪೂರ್ಣಾಙ್ಕ start_rule, PyObject *filename_ob,
                       PyCompilerFlags *flags, PyArena *arena, PyObject *module,
                       PyObject **incomplete)
{
    _PyTokenizer_Newline newline_mode = start_rule == Py_file_ಒಳಹರಿವು
        ? _PyTokenizer_IMPLY_NEWLINE : _PyTokenizer_KEEP_NEWLINE;
    ಪೂರ್ಣಾಙ್ಕ parser_flags = compute_parser_flags(flags);
    ಯದಿ (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_ಒಳಹರಿವು) {
        newline_mode = _PyTokenizer_APPEND_NEWLINE;
    }

    ಸಂರಚನಾ tok_state *tok;
    ಯದಿ (flags != ರಿಕ್ತ && flags->cf_flags & PyCF_IGNORE_COOKIE) {
        tok = _PyTokenizer_FromUTF8(str, newline_mode, 0);
    } ಅನ್ಯಥಾ {
        tok = _PyTokenizer_FromString(str, newline_mode, 0);
    }
    ಯದಿ (tok == ರಿಕ್ತ) {
        ಯದಿ (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        ಅನ್ಯಥಾ {
            // The only silent tokenizer init failure is a failed allocation.
            PyErr_NoMemory();
        }
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    _PyTokenizer_SetContext(tok, filename_ob, module);

    // We need to clear up from here on
    mod_ty result = ರಿಕ್ತ;

    ಪೂರ್ಣಾಙ್ಕ feature_version = flags && (flags->cf_flags & PyCF_ONLY_AST) ?
        flags->cf_feature_version : PY_MINOR_VERSION;
    Parser *p = _PyPegen_Parser_New(tok, start_rule, parser_flags, feature_version,
                                    ರಿಕ್ತ, str, arena);
    ಯದಿ (p == ರಿಕ್ತ) {
        ಗಚ್ಛ error;
    }

    result = _PyPegen_run_parser(p);
    ಯದಿ (result != ರಿಕ್ತ && (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_ಒಳಹರಿವು)) {
        ಪೂರ್ಣಾಙ್ಕ needs_newline = _PyTokenizer_GetNewlineRequirements(tok);
        ಯದಿ ((needs_newline & _PyTokenizer_NEWLINE_CONTINUATION) ||
            (start_rule != Py_file_ಒಳಹರಿವು &&
             (needs_newline & _PyTokenizer_NEWLINE_INDENT)) ||
            (start_rule == Py_single_ಒಳಹರಿವು && p->ಒಳಹರಿವು_exhausted &&
             (needs_newline & _PyTokenizer_NEWLINE_STATEMENT))) {
            Py_Pegen_raise_incomplete_ಒಳಹರಿವು(p);
            ಯದಿ (incomplete != ರಿಕ್ತ && PyErr_ExceptionMatches(Py_Exc_Incomplete_ಒಳಹರಿವು_Error)) {
                *incomplete = PyErr_GetRaisedException();
            }
            ಅನ್ಯಥಾ {
                result = ರಿಕ್ತ;
            }
        }
    }
    _PyPegen_Parser_Free(p);

error:
    _PyTokenizer_Free(tok);
    ಪ್ರತ್ಯಾಗಮ result;
}
