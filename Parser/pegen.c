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

#परिभाषय IDENTIFIER_CACHE_आकार 2048  // Must be a power of two.
#परिभाषय IDENTIFIER_CACHE_MAX_PROBES 8
#परिभाषय PEGEN_ARRAY_GROWTH_गुणकः 2
#परिभाषय TYPE_IGNORE_INITIAL_क्षमता 10

संरचना identifier_cache_प्रविष्टिः {
    ध्रुव वर्ण *कुञ्चिका;   // Borrowed from arena-owned token bytes.
    Py_ssize_t len;
    Py_hash_t hash;
    PyObject *मानम्;   // Borrowed from an arena-owned identifier.
};

// Internal parser functions

asdl_stmt_seq*
_PyPegen_interactive_exit(Parser *p)
{
    यदि (p->errcode) {
        *(p->errcode) = E_EOF;
    }
    प्रत्यागम रिक्त;
}

Py_ssize_t
Py_Pegen_byte_विस्थापनम्_गन्तव्यम्_character_विस्थापनम्_पङ्क्तिः(PyObject *पङ्क्तिः, Py_ssize_t col_विस्थापनम्, Py_ssize_t अन्तः_col_विस्थापनम्)
{
    ध्रुव अचिह्नित वर्ण *दत्त = (ध्रुव अचिह्नित वर्ण*)PyUnicode_AsUTF8(पङ्क्तिः);
    यदि (दत्त == रिक्त) {
        प्रत्यागम -1;
    }

    Py_ssize_t len = 0;
    यावत् (col_विस्थापनम् < अन्तः_col_विस्थापनम्) {
        Py_UCS4 ch = दत्त[col_विस्थापनम्];
        यदि (ch < 0x80) {
            col_विस्थापनम् += 1;
        } अन्यथा यदि ((ch & 0xe0) == 0xc0) {
            col_विस्थापनम् += 2;
        } अन्यथा यदि ((ch & 0xf0) == 0xe0) {
            col_विस्थापनम् += 3;
        } अन्यथा यदि ((ch & 0xf8) == 0xf0) {
            col_विस्थापनम् += 4;
        } अन्यथा {
            Py_Err_समुच्चयः_String(Py_Exc_मानम्_Error, "Invalid UTF-8 sequence");
            प्रत्यागम -1;
        }
        len++;
    }
    प्रत्यागम len;
}

Py_ssize_t
Py_Pegen_byte_विस्थापनम्_गन्तव्यम्_character_विस्थापनम्_raw(ध्रुव वर्ण* str, Py_ssize_t col_विस्थापनम्)
{
    Py_ssize_t len = (Py_ssize_t)वाक्यदैर्घ्यम्(str);
    यदि (col_विस्थापनम् > len + 1) {
        col_विस्थापनम् = len + 1;
    }
    प्रतिज्ञा(col_विस्थापनम् >= 0);
    PyObject *text = PyUnicode_DecodeUTF8(str, col_विस्थापनम्, "replace");
    यदि (!text) {
        प्रत्यागम -1;
    }
    Py_ssize_t आकार = Py_Unicode_GET_दीर्घता(text);
    Py_DECREF(text);
    प्रत्यागम आकार;
}

Py_ssize_t
Py_Pegen_byte_विस्थापनम्_गन्तव्यम्_character_विस्थापनम्(PyObject *पङ्क्तिः, Py_ssize_t col_विस्थापनम्)
{
    ध्रुव वर्ण *str = PyUnicode_AsUTF8(पङ्क्तिः);
    यदि (!str) {
        प्रत्यागम -1;
    }
    प्रत्यागम Py_Pegen_byte_विस्थापनम्_गन्तव्यम्_character_विस्थापनम्_raw(str, col_विस्थापनम्);
}

// Here, mark is the start of the node, while p->mark is the end.
// If node==NULL, they should be the same.
पूर्णाङ्क
Py_Pegen_निवेशय_memo(Parser *p, पूर्णाङ्क mark, पूर्णाङ्क type, शून्य *ग्रन्थिः)
{
    // Insert in front
    Memo *m = _PyArena_Malloc(p->arena, परिमाण(Memo));
    यदि (m == रिक्त) {
        प्रत्यागम -1;
    }
    m->type = type;
    m->ग्रन्थिः = ग्रन्थिः;
    m->mark = p->mark;
    m->अग्र = p->tokens[mark]->memo;
    p->tokens[mark]->memo = m;
    p->tokens[mark]->memo_आवरणम् |= 1ULL << (type & 63);
    प्रत्यागम 0;
}

// Like _PyPegen_insert_memo(), but returns the inserted Memo so callers
// can update it in place without re-walking the token's memo list.
Memo *
Py_Pegen_निवेशय_memo_प्रत्यक्षम्(Parser *p, पूर्णाङ्क mark, पूर्णाङ्क type)
{
    यदि (Py_Pegen_निवेशय_memo(p, mark, type, रिक्त) < 0) {
        प्रत्यागम रिक्त;
    }
    प्रत्यागम p->tokens[mark]->memo;
}

// Like _PyPegen_insert_memo(), but updates an existing node if found.
पूर्णाङ्क
_PyPegen_update_memo(Parser *p, पूर्णाङ्क mark, पूर्णाङ्क type, शून्य *ग्रन्थिः)
{
    प्रति (Memo *m = p->tokens[mark]->memo; m != रिक्त; m = m->अग्र) {
        यदि (m->type == type) {
            // Update existing node.
            m->ग्रन्थिः = ग्रन्थिः;
            m->mark = p->mark;
            प्रत्यागम 0;
        }
    }
    // Insert new node.
    प्रत्यागम Py_Pegen_निवेशय_memo(p, mark, type, ग्रन्थिः);
}

स्थिर पूर्णाङ्क
init_normalization(Parser *p)
{
    यदि (p->normalize) {
        प्रत्यागम 1;
    }
    p->normalize = PyImport_ImportModuleAttrString("unicodedata", "normalize");
    यदि (!p->normalize)
    {
        प्रत्यागम 0;
    }
    प्रत्यागम 1;
}

पूर्णाङ्क
Py_Pegen_grow_loop_बफर(शून्य ***बफर, Py_ssize_t *क्षमता)
{
    यदि ((आकार_t)*क्षमता > (आकार_t)PY_SSIZE_T_MAX /
            (PEGEN_ARRAY_GROWTH_गुणकः * परिमाण(**बफर))) {
        प्रत्यागम -1;
    }
    Py_ssize_t new_क्षमता = *क्षमता == 0
        ? 1 : *क्षमता * PEGEN_ARRAY_GROWTH_गुणकः;
    शून्य **new_बफर = PyMem_Realloc(*बफर, new_क्षमता * परिमाण(**बफर));
    यदि (new_बफर == रिक्त) {
        प्रत्यागम -1;
    }
    *बफर = new_बफर;
    *क्षमता = new_क्षमता;
    प्रत्यागम 0;
}

स्थिर पूर्णाङ्क
growable_comment_array_सङ्कलय(growable_comment_array *arr, पूर्णाङ्क lineno, वर्ण *comment)
{
    यदि (arr->अङ्कः_वस्तूनि >= arr->आकार) {
        यदि (arr->आकार > (आकार_t)PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_गुणकः * परिमाण(*arr->वस्तूनि))) {
            प्रत्यागम 0;
        }
        आकार_t new_आकार = arr->आकार == 0
            ? TYPE_IGNORE_INITIAL_क्षमता
            : arr->आकार * PEGEN_ARRAY_GROWTH_गुणकः;
        शून्य *new_वस्तूनि_array = PyMem_Realloc(
            arr->वस्तूनि, new_आकार * परिमाण(*arr->वस्तूनि));
        यदि (!new_वस्तूनि_array) {
            प्रत्यागम 0;
        }
        arr->वस्तूनि = new_वस्तूनि_array;
        arr->आकार = new_आकार;
    }

    arr->वस्तूनि[arr->अङ्कः_वस्तूनि].lineno = lineno;
    arr->वस्तूनि[arr->अङ्कः_वस्तूनि].comment = comment;  // Take ownership
    arr->अङ्कः_वस्तूनि++;
    प्रत्यागम 1;
}

स्थिर शून्य
growable_comment_array_deallocate(growable_comment_array *arr)
{
    प्रति (आकार_t i = 0; i < arr->अङ्कः_वस्तूनि; i++) {
        PyMem_Free(arr->वस्तूनि[i].comment);
    }
    PyMem_Free(arr->वस्तूनि);
}

स्थिर पूर्णाङ्क
_get_keyword_or_name_type(Parser *p, ध्रुव वर्ण *text, Py_ssize_t दीर्घता_2)
{
    प्रतिज्ञा(दीर्घता_2 > 0);

    यदि (दीर्घता_2 >= p->n_keyword_lists ||
        p->keywords[दीर्घता_2] == रिक्त ||
        p->keywords[दीर्घता_2]->type == -1) {
        प्रत्यागम NAME;
    }
    प्रति (KeywordToken *k = p->keywords[दीर्घता_2]; k != रिक्त && k->type != -1; k++) {
        यदि (memcmp(k->str, text, (आकार_t)दीर्घता_2) == 0) {
            प्रत्यागम k->type;
        }
    }
    प्रत्यागम NAME;
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
        पक्ष FSTRING_अन्तः:
        पक्ष TSTRING_START:
        पक्ष TSTRING_मध्यः:
        पक्ष TSTRING_अन्तः:
        पक्ष TYPE_COMMENT:
        पक्ष NOTEQUAL:  // _PyPegen_check_barry_as_flufl() distinguishes != and <>.
            प्रत्यागम 1;
        स्वभाव:
            प्रत्यागम 0;
    }
}

स्थिर पूर्णाङ्क
initialize_token(Parser *p, Token *parser_token, संरचना token *new_token, पूर्णाङ्क token_type) {
    प्रतिज्ञा(parser_token != रिक्त);

    parser_token->type = token_type;
    parser_token->अष्टकानि = रिक्त;
    यदि (token_needs_text(token_type) && new_token->विस्तृतिः.start >= 0) {
        Py_ssize_t दीर्घता_2;
        ध्रुव वर्ण *text = _PyToken_TextView(p->tok, new_token, &दीर्घता_2);
        यदि (token_type == NAME) {
            parser_token->type = _get_keyword_or_name_type(p, text, दीर्घता_2);
        }
        // A name reclassified as a keyword does not need its text.
        यदि (parser_token->type == token_type) {
            parser_token->अष्टकानि = Py_अष्टकानि_From_String_And_आकार(text, दीर्घता_2);
            यदि (parser_token->अष्टकानि == रिक्त) {
                प्रत्यागम -1;
            }
            यदि (Py_Arena_सङ्कलय_Py_Object(p->arena, parser_token->अष्टकानि) < 0) {
                Py_DECREF(parser_token->अष्टकानि);
                प्रत्यागम -1;
            }
        }
    }

    parser_token->metadata = रिक्त;
    यदि (new_token->metadata != रिक्त) {
        यदि (Py_Arena_सङ्कलय_Py_Object(p->arena, new_token->metadata) < 0) {
            प्रत्यागम -1;
        }
        parser_token->metadata = new_token->metadata;
        new_token->metadata = रिक्त;
    }

    parser_token->level = new_token->level;
    parser_token->is_raw = new_token->is_raw;
    parser_token->lineno = new_token->start_loc.lineno;
    parser_token->col_विस्थापनम् = new_token->अन्तः_loc.lineno == p->starting_lineno
        ? p->starting_col_विस्थापनम् + new_token->start_loc.byte_col
        : new_token->start_loc.byte_col;
    parser_token->अन्तः_lineno = new_token->अन्तः_loc.lineno;
    parser_token->अन्तः_col_विस्थापनम् = new_token->अन्तः_loc.lineno == p->starting_lineno
        ? p->starting_col_विस्थापनम् + new_token->अन्तः_loc.byte_col
        : new_token->अन्तः_loc.byte_col;

    p->fill += 1;

    यदि (token_type == ERRORTOKEN && Py_Tokenizer_Get_स्थितिः(p->tok) == E_DECODE) {
        प्रत्यागम _Pypegen_raise_decode_error(p);
    }

    प्रत्यागम (token_type == ERRORTOKEN ? _Pypegen_tokenizer_error(p) : 0);
}

स्थिर पूर्णाङ्क
_resize_tokens_array(Parser *p) {
    यदि (p->आकार > अधिकतम_पूर्णाङ्कः / PEGEN_ARRAY_GROWTH_गुणकः ||
            (आकार_t)p->आकार > PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_गुणकः * परिमाण(*p->tokens))) {
        Py_Err_No_स्मृतिः();
        प्रत्यागम -1;
    }
    पूर्णाङ्क newsize = p->आकार * PEGEN_ARRAY_GROWTH_गुणकः;
    Token **new_tokens = PyMem_Realloc(p->tokens, (आकार_t)newsize * परिमाण(Token *));
    यदि (new_tokens == रिक्त) {
        Py_Err_No_स्मृतिः();
        प्रत्यागम -1;
    }
    p->tokens = new_tokens;

    // Allocate new tokens together without moving existing ones: parser rules
    // may still hold pointers into earlier blocks. Parser_Free frees each block.
    Token *chunk = PyMem_Calloc((आकार_t)(newsize - p->आकार), परिमाण(Token));
    यदि (chunk == रिक्त) {
        Py_Err_No_स्मृतिः();
        प्रत्यागम -1;
    }
    प्रति (पूर्णाङ्क i = p->आकार; i < newsize; i++) {
        p->tokens[i] = &chunk[i - p->आकार];
    }
    p->आकार = newsize;
    प्रत्यागम 0;
}

पूर्णाङ्क
_PyPegen_fill_token(Parser *p)
{
    संरचना token new_token;
    _PyToken_Init(&new_token);
    _PyTokenizer_Get(p->tok, &new_token);

    // Record and skip '# type: ignore' comments
    यावत् (new_token.type == TYPE_IGNORE) {
        Py_ssize_t len;
        ध्रुव वर्ण *text = _PyToken_TextView(p->tok, &new_token, &len);
        वर्ण *चिह्नकम् = PyMem_Malloc((आकार_t)len + 1);
        यदि (चिह्नकम् == रिक्त) {
            Py_Err_No_स्मृतिः();
            गच्छ error;
        }
        स्मृतिं_प्रतिलिख(चिह्नकम्, text, (आकार_t)len);
        चिह्नकम्[len] = '\0';
        // Ownership of tag passes to the growable array
        यदि (!growable_comment_array_सङ्कलय(&p->type_ignore_comments, new_token.अन्तः_loc.lineno, चिह्नकम्)) {
            PyMem_Free(चिह्नकम्);
            Py_Err_No_स्मृतिः();
            गच्छ error;
        }
        _PyTokenizer_Get(p->tok, &new_token);
    }

    पूर्णाङ्क type = new_token.type;
    // EOF can finish an ordinary token. Only a boundary token means that
    // the grammar requested input beyond the last physical line.
    यदि (type == ENDMARKER ||
        (type == DEDENT && (p->flags & Py_PARSE_ALLOW_INCOMPLETE_निवेशः) &&
         Py_Tokenizer_Get_स्थितिः(p->tok) == E_EOF)) {
        p->निवेशः_exhausted = 1;
    }

    // If we have reached the end and we are in single input mode we need to insert a newline and reset the parsing
    यदि (p->start_नियमः == Py_single_निवेशः && type == ENDMARKER && p->parsing_started) {
        type = NEWLINE; /* Add an extra newline */
        p->parsing_started = 0;

        यदि (!(p->flags & PyPARSE_DONT_IMPLY_DEDENT)) {
            _PyTokenizer_ImplyDedents(p->tok);
        }
    }
    अन्यथा {
        p->parsing_started = 1;
    }

    // Check if we are at the limit of the token array capacity and resize if needed
    यदि ((p->fill == p->आकार) && (_resize_tokens_array(p) != 0)) {
        गच्छ error;
    }

    Token *t = p->tokens[p->fill];
    पूर्णाङ्क परिणाम = initialize_token(p, t, &new_token, type);
    _PyToken_Free(&new_token);
    प्रत्यागम परिणाम;
error:
    _PyToken_Free(&new_token);
    प्रत्यागम -1;
}

#यदि defined(Py_DEBUG)
// Instrumentation to count the effectiveness of memoization.
// The array counts the number of tokens skipped by memoization,
// indexed by type.

#परिभाषय NSTATISTICS _PYPEGEN_NSTATISTICS
#परिभाषय memo_statistics _PyRuntime.parser.memo_statistics

शून्य
_PyPegen_clear_memo_statistics(शून्य)
{
    Py_Mutex_तालकम्(&_PyRuntime.parser.mutex);
    प्रति (पूर्णाङ्क i = 0; i < NSTATISTICS; i++) {
        memo_statistics[i] = 0;
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
}

PyObject *
_PyPegen_get_memo_statistics(शून्य)
{
    PyObject *ret = PyList_New(NSTATISTICS);
    यदि (ret == रिक्त) {
        प्रत्यागम रिक्त;
    }

    Py_Mutex_तालकम्(&_PyRuntime.parser.mutex);
    प्रति (पूर्णाङ्क i = 0; i < NSTATISTICS; i++) {
        PyObject *मानम् = PyLong_FromLong(memo_statistics[i]);
        यदि (मानम् == रिक्त) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            प्रत्यागम रिक्त;
        }
        // PyList_SetItem borrows a reference to value.
        यदि (Py_List_समुच्चयः_Item(ret, i, मानम्) < 0) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            प्रत्यागम रिक्त;
        }
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
    प्रत्यागम ret;
}
#यदि_अन्तः

पूर्णाङ्क  // bool
_PyPegen_is_memoized(Parser *p, पूर्णाङ्क type, शून्य *pres)
{
    यदि (p->mark == p->fill) {
        यदि (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            प्रत्यागम -1;
        }
    }

    Token *t = p->tokens[p->mark];

    यदि (!(t->memo_आवरणम् & (1ULL << (type & 63)))) {
        प्रत्यागम 0;
    }

    प्रति (Memo *m = t->memo; m != रिक्त; m = m->अग्र) {
        यदि (m->type == type) {
#यदि defined(Py_DEBUG)
            यदि (0 <= type && type < NSTATISTICS) {
                दीर्घ गणनम् = m->mark - p->mark;
                // A memoized negative result counts for one.
                यदि (गणनम् <= 0) {
                    गणनम् = 1;
                }
                Py_Mutex_तालकम्(&_PyRuntime.parser.mutex);
                memo_statistics[type] += गणनम्;
                PyMutex_Unlock(&_PyRuntime.parser.mutex);
            }
#यदि_अन्तः
            p->mark = m->mark;
            *(शून्य **)(pres) = m->ग्रन्थिः;
            प्रत्यागम 1;
        }
    }
    प्रत्यागम 0;
}

#परिभाषय LOOKAHEAD1(NAME, RES_TYPE)                                  \
    पूर्णाङ्क                                                             \
    NAME (पूर्णाङ्क धनात्मकम्, RES_TYPE (func)(Parser *), Parser *p)       \
    {                                                               \
        पूर्णाङ्क mark = p->mark;                                         \
        शून्य *res = func(p);                                        \
        p->mark = mark;                                             \
        प्रत्यागम (res != रिक्त) == धनात्मकम्;                           \
    }

LOOKAHEAD1(_PyPegen_lookahead, शून्य *)
LOOKAHEAD1(Py_Pegen_lookahead_for_व्यञ्जकम्, व्यञ्जकम्_ty)
LOOKAHEAD1(_PyPegen_lookahead_for_stmt, stmt_ty)
#अपरिभाषय LOOKAHEAD1

#परिभाषय LOOKAHEAD2(NAME, RES_TYPE, T)                                   \
    पूर्णाङ्क                                                                 \
    NAME (पूर्णाङ्क धनात्मकम्, RES_TYPE (func)(Parser *, T), Parser *p, T arg) \
    {                                                                   \
        पूर्णाङ्क mark = p->mark;                                             \
        शून्य *res = func(p, arg);                                       \
        p->mark = mark;                                                 \
        प्रत्यागम (res != रिक्त) == धनात्मकम्;                               \
    }

LOOKAHEAD2(_PyPegen_lookahead_with_int, Token *, पूर्णाङ्क)
LOOKAHEAD2(_PyPegen_lookahead_with_string, व्यञ्जकम्_ty, ध्रुव वर्ण *)
#अपरिभाषय LOOKAHEAD2

Token *
_PyPegen_expect_token(Parser *p, पूर्णाङ्क type)
{
    यदि (p->mark == p->fill) {
        यदि (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            प्रत्यागम रिक्त;
        }
    }
    Token *t = p->tokens[p->mark];
    यदि (t->type != type) {
       प्रत्यागम रिक्त;
    }
    p->mark += 1;
    प्रत्यागम t;
}

शून्य*
Py_Pegen_expect_forced_परिणाम(Parser *p, शून्य* परिणाम, ध्रुव वर्ण* expected) {

    यदि (p->error_indicator == 1) {
        प्रत्यागम रिक्त;
    }
    यदि (परिणाम == रिक्त) {
        RAISE_SYNTAX_ERROR("expected (%s)", expected);
        प्रत्यागम रिक्त;
    }
    प्रत्यागम परिणाम;
}

Token *
_PyPegen_expect_forced_token(Parser *p, पूर्णाङ्क type, ध्रुव वर्ण* expected) {

    यदि (p->error_indicator == 1) {
        प्रत्यागम रिक्त;
    }

    यदि (p->mark == p->fill) {
        यदि (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            प्रत्यागम रिक्त;
        }
    }
    Token *t = p->tokens[p->mark];
    यदि (t->type != type) {
        RAISE_SYNTAX_ERROR_ज्ञातम्_LOCATION(t, "expected '%s'", expected);
        प्रत्यागम रिक्त;
    }
    p->mark += 1;
    प्रत्यागम t;
}

व्यञ्जकम्_ty
_PyPegen_expect_soft_keyword(Parser *p, ध्रुव वर्ण *keyword)
{
    यदि (p->mark == p->fill) {
        यदि (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            प्रत्यागम रिक्त;
        }
    }
    Token *t = p->tokens[p->mark];
    यदि (t->type != NAME) {
        प्रत्यागम रिक्त;
    }
    ध्रुव वर्ण *s = Py_अष्टकानि_As_String(t->अष्टकानि);
    यदि (!s) {
        p->error_indicator = 1;
        प्रत्यागम रिक्त;
    }
    यदि (वाक्यं_तुलय(s, keyword) != 0) {
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyPegen_name_token(p);
}

Token *
Py_Pegen_get_चरमः_nonnwhitespace_token(Parser *p)
{
    प्रतिज्ञा(p->mark >= 0);
    Token *token = रिक्त;
    प्रति (पूर्णाङ्क m = p->mark - 1; m >= 0; m--) {
        token = p->tokens[m];
        यदि (token->type != ENDMARKER && (token->type < NEWLINE || token->type > DEDENT)) {
            भङ्ग;
        }
    }
    प्रत्यागम token;
}

PyObject *
_PyPegen_new_identifier(Parser *p, ध्रुव वर्ण *n)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, (Py_ssize_t)वाक्यदैर्घ्यम्(n), रिक्त);
    यदि (!id) {
        गच्छ error;
    }
    /* Check whether there are non-ASCII characters in the
       identifier; if so, normalize to NFKC. */
    यदि (!PyUnicode_IS_ASCII(id))
    {
        यदि (!init_normalization(p))
        {
            Py_DECREF(id);
            गच्छ error;
        }
        PyObject *form = PyUnicode_InternFromString("NFKC");
        यदि (form == रिक्त)
        {
            Py_DECREF(id);
            गच्छ error;
        }
        PyObject *तर्काः[2] = {form, id};
        PyObject *id2 = PyObject_Vectorcall(p->normalize, तर्काः, 2, रिक्त);
        Py_DECREF(id);
        Py_DECREF(form);
        यदि (!id2) {
            गच्छ error;
        }

        यदि (!PyUnicode_Check(id2))
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
    प्रति (पूर्णाङ्क i = 0; forbidden[i] != रिक्त; i++) {
        यदि (Py_Unicode_Equal_गन्तव्यम्_ASCIIString(id, forbidden[i])) {
            PyErr_Format(Py_Exc_मानम्_Error,
                         "identifier field can't represent '%s' constant",
                         forbidden[i]);
            Py_DECREF(id);
            गच्छ error;
        }
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &id);
    यदि (Py_Arena_सङ्कलय_Py_Object(p->arena, id) < 0)
    {
        Py_DECREF(id);
        गच्छ error;
    }
    प्रत्यागम id;

error:
    p->error_indicator = 1;
    प्रत्यागम रिक्त;
}

// Return an arena-owned identifier; callers borrow the reference.
स्थिर PyObject *
get_cached_identifier(Parser *p, PyObject *अष्टकानि)
{
    ध्रुव वर्ण *s = Py_अष्टकानि_As_String(अष्टकानि);
    यदि (!s) {
        p->error_indicator = 1;
        प्रत्यागम रिक्त;
    }
    // Identifiers repeat constantly; a small span-keyed cache skips the
    // UTF-8 decode + intern for repeated occurrences. Keys point into
    // arena-owned token bytes and values are arena-owned interned strings,
    // so borrowed references are valid for the lifetime of the parse
    // (including the second error pass, which reuses parser and arena).
    Py_ssize_t len = Py_अष्टकानि_GET_आकार(अष्टकानि);
    Py_hash_t hash = PyObject_Hash(अष्टकानि);
    यदि (hash == -1) {
        p->error_indicator = 1;
        प्रत्यागम रिक्त;
    }
    // Parses without identifiers do not need this cache.
    यदि (p->identifier_cache == रिक्त) {
        p->identifier_cache = PyMem_Calloc(
            IDENTIFIER_CACHE_आकार, परिमाण(*p->identifier_cache));
        यदि (p->identifier_cache == रिक्त) {
            p->error_indicator = 1;
            प्रत्यागम Py_Err_No_स्मृतिः();
        }
    }
    Identifier_Cache_प्रविष्टिः *free_slot = रिक्त;
    आकार_t idx = (आकार_t)hash & (IDENTIFIER_CACHE_आकार - 1);
    प्रति (पूर्णाङ्क probe = 0; probe < IDENTIFIER_CACHE_MAX_PROBES; probe++) {
        Identifier_Cache_प्रविष्टिः *प्रविष्टिः = &p->identifier_cache[
            (idx + probe) & (IDENTIFIER_CACHE_आकार - 1)];
        यदि (प्रविष्टिः->कुञ्चिका == रिक्त) {
            free_slot = प्रविष्टिः;
            भङ्ग;
        }
        यदि (प्रविष्टिः->hash == hash && प्रविष्टिः->len == len &&
            memcmp(प्रविष्टिः->कुञ्चिका, s, len) == 0)
        {
            प्रत्यागम प्रविष्टिः->मानम्;
        }
    }
    PyObject *id = _PyPegen_new_identifier(p, s);
    यदि (id == रिक्त) {
        p->error_indicator = 1;
        प्रत्यागम रिक्त;
    }
    यदि (free_slot != रिक्त) {
        free_slot->कुञ्चिका = s;
        free_slot->len = len;
        free_slot->hash = hash;
        free_slot->मानम् = id;
    }
    प्रत्यागम id;
}

स्थिर व्यञ्जकम्_ty
_PyPegen_name_from_token(Parser *p, Token* t)
{
    यदि (t == रिक्त) {
        प्रत्यागम रिक्त;
    }
    // Reuse the AST node when backtracking revisits this token. Memo lookup
    // starts before the token and restores the position after it on a hit.
    // Token kinds can be memo keys: generated grammar rule IDs start at 1000.
    पूर्णाङ्क mark = p->mark - 1;
    p->mark = mark;
    व्यञ्जकम्_ty cached = रिक्त;
    यदि (_PyPegen_is_memoized(p, NAME, &cached)) {
        प्रत्यागम cached;
    }
    p->mark = mark + 1;
    PyObject *id = get_cached_identifier(p, t->अष्टकानि);
    यदि (id == रिक्त) {
        प्रत्यागम रिक्त;
    }
    व्यञ्जकम्_ty परिणाम = _PyAST_Name(id, आरोपय, t->lineno, t->col_विस्थापनम्,
                                t->अन्तः_lineno, t->अन्तः_col_विस्थापनम्, p->arena);
    यदि (परिणाम != रिक्त && Py_Pegen_निवेशय_memo(p, mark, NAME, परिणाम) < 0) {
        p->error_indicator = 1;
        प्रत्यागम रिक्त;
    }
    प्रत्यागम परिणाम;
}

व्यञ्जकम्_ty
_PyPegen_name_token(Parser *p)
{
    Token *t = _PyPegen_expect_token(p, NAME);
    प्रत्यागम _PyPegen_name_from_token(p, t);
}

शून्य *
_PyPegen_string_token(Parser *p)
{
    प्रत्यागम _PyPegen_expect_token(p, STRING);
}

व्यञ्जकम्_ty _PyPegen_soft_keyword_token(Parser *p) {
    Token *t = _PyPegen_expect_token(p, NAME);
    यदि (t == रिक्त) {
        प्रत्यागम रिक्त;
    }
    वर्ण *the_token;
    Py_ssize_t आकार;
    Py_अष्टकानि_As_String_And_आकार(t->अष्टकानि, &the_token, &आकार);
    प्रति (वर्ण **keyword = p->soft_keywords; *keyword != रिक्त; keyword++) {
        यदि (वाक्यदैर्घ्यम्(*keyword) == (आकार_t)आकार &&
            वाक्ये_तुलय(*keyword, the_token, (आकार_t)आकार) == 0) {
            प्रत्यागम _PyPegen_name_from_token(p, t);
        }
    }
    प्रत्यागम रिक्त;
}

स्थिर PyObject *
parsenumber_raw(ध्रुव वर्ण *s)
{
    ध्रुव वर्ण *अन्तः_2;
    दीर्घ x;
    द्विगुण dx;
    Py_complex compl;
    पूर्णाङ्क imflag;

    प्रतिज्ञा(s != रिक्त);
    errno = 0;
    अन्तः_2 = s + वाक्यदैर्घ्यम्(s) - 1;
    imflag = *अन्तः_2 == 'j' || *अन्तः_2 == 'J';
    यदि (s[0] == '0') {
        x = (दीर्घ)PyOS_strtoul(s, (वर्ण **)&अन्तः_2, 0);
        यदि (x < 0 && errno == 0) {
            प्रत्यागम PyLong_FromString(s, (वर्ण **)0, 0);
        }
    }
    अन्यथा {
        x = PyOS_strtol(s, (वर्ण **)&अन्तः_2, 0);
    }
    यदि (*अन्तः_2 == '\0') {
        यदि (errno != 0) {
            प्रत्यागम PyLong_FromString(s, (वर्ण **)0, 0);
        }
        प्रत्यागम PyLong_FromLong(x);
    }
    /* XXX Huge floats may silently fail */
    यदि (imflag) {
        compl.real = 0.;
        compl.imag = Py_OS_string_गन्तव्यम्_double(s, (वर्ण **)&अन्तः_2, रिक्त);
        यदि (compl.imag == -1.0 && PyErr_Occurred()) {
            प्रत्यागम रिक्त;
        }
        प्रत्यागम PyComplex_FromCComplex(compl);
    }
    dx = Py_OS_string_गन्तव्यम्_double(s, रिक्त, रिक्त);
    यदि (dx == -1.0 && PyErr_Occurred()) {
        प्रत्यागम रिक्त;
    }
    प्रत्यागम PyFloat_FromDouble(dx);
}

स्थिर PyObject *
parsenumber(ध्रुव वर्ण *s)
{
    वर्ण *dup;
    वर्ण *अन्तः_2;
    PyObject *res = रिक्त;

    प्रतिज्ञा(s != रिक्त);

    यदि (strchr(s, '_') == रिक्त) {
        प्रत्यागम parsenumber_raw(s);
    }
    /* Create a duplicate without underscores. */
    dup = PyMem_Malloc(वाक्यदैर्घ्यम्(s) + 1);
    यदि (dup == रिक्त) {
        प्रत्यागम Py_Err_No_स्मृतिः();
    }
    अन्तः_2 = dup;
    प्रति (; *s; s++) {
        यदि (*s != '_') {
            *अन्तः_2++ = *s;
        }
    }
    *अन्तः_2 = '\0';
    res = parsenumber_raw(dup);
    PyMem_Free(dup);
    प्रत्यागम res;
}

व्यञ्जकम्_ty
Py_Pegen_सङ्ख्या_token(Parser *p)
{
    पूर्णाङ्क mark = p->mark;
    Token *t = _PyPegen_expect_token(p, सङ्ख्या);
    यदि (t == रिक्त) {
        प्रत्यागम रिक्त;
    }

    // Look up the node at the token's start, as for NAME tokens. A memo hit
    // advances the mark; on a miss, restore the position after the token.
    p->mark = mark;
    व्यञ्जकम्_ty cached = रिक्त;
    यदि (_PyPegen_is_memoized(p, सङ्ख्या, &cached)) {
        प्रत्यागम cached;
    }
    p->mark = mark + 1;

    ध्रुव वर्ण *अङ्कः_raw = Py_अष्टकानि_As_String(t->अष्टकानि);
    यदि (अङ्कः_raw == रिक्त) {
        p->error_indicator = 1;
        प्रत्यागम रिक्त;
    }

    यदि (p->feature_संस्करणम् < 6 && strchr(अङ्कः_raw, '_') != रिक्त) {
        p->error_indicator = 1;
        प्रत्यागम RAISE_SYNTAX_ERROR("Underscores in numeric literals are only supported "
                                  "in Python 3.6 and greater");
    }

    PyObject *c = parsenumber(अङ्कः_raw);

    यदि (c == रिक्त) {
        p->error_indicator = 1;
        PyThreadState *tstate = _PyThreadState_GET();
        // The only way a ValueError should happen in _this_ code is via
        // PyLong_FromString hitting a length limit.
        यदि (tstate->current_exception != रिक्त &&
            Py_TYPE(tstate->current_exception) == (PyTypeObject *)Py_Exc_मानम्_Error
        ) {
            PyObject *exc = PyErr_GetRaisedException();
            /* Intentionally omitting columns to avoid a wall of 1000s of '^'s
             * on the error message. Nobody is going to overlook their huge
             * numeric literal once given the line. */
            RAISE_ERROR_ज्ञातम्_LOCATION(
                p, PyExc_SyntaxError,
                t->lineno, -1 /* col_offset */,
                t->अन्तः_lineno, -1 /* end_col_offset */,
                "%S - Consider hexadecimal for huge integer literals "
                "to avoid decimal conversion limits.",
                exc);
            Py_DECREF(exc);
        }
        प्रत्यागम रिक्त;
    }

    यदि (Py_Arena_सङ्कलय_Py_Object(p->arena, c) < 0) {
        Py_DECREF(c);
        p->error_indicator = 1;
        प्रत्यागम रिक्त;
    }

    व्यञ्जकम्_ty परिणाम = _PyAST_Constant(c, रिक्त, t->lineno, t->col_विस्थापनम्,
                                    t->अन्तः_lineno, t->अन्तः_col_विस्थापनम्, p->arena);
    यदि (परिणाम != रिक्त && Py_Pegen_निवेशय_memo(p, mark, सङ्ख्या, परिणाम) < 0) {
        p->error_indicator = 1;
        प्रत्यागम रिक्त;
    }
    प्रत्यागम परिणाम;
}



स्थिर पूर्णाङ्क
compute_parser_flags(PyCompilerFlags *flags)
{
    पूर्णाङ्क parser_flags = 0;
    यदि (!flags) {
        प्रत्यागम 0;
    }
    यदि (flags->cf_flags & PyCF_DONT_IMPLY_DEDENT) {
        parser_flags |= PyPARSE_DONT_IMPLY_DEDENT;
    }
    यदि (flags->cf_flags & PyCF_IGNORE_COOKIE) {
        parser_flags |= PyPARSE_IGNORE_COOKIE;
    }
    यदि (flags->cf_flags & CO_FUTURE_BARRY_AS_BDFL) {
        parser_flags |= PyPARSE_BARRY_AS_BDFL;
    }
    यदि (flags->cf_flags & PyCF_TYPE_COMMENTS) {
        parser_flags |= PyPARSE_TYPE_COMMENTS;
    }
    यदि (flags->cf_flags & Py_CF_ALLOW_INCOMPLETE_निवेशः) {
        parser_flags |= Py_PARSE_ALLOW_INCOMPLETE_निवेशः;
    }
    प्रत्यागम parser_flags;
}

// Parser API

Parser *
_PyPegen_Parser_New(संरचना tok_state *tok, पूर्णाङ्क start_नियमः, पूर्णाङ्क flags,
                    पूर्णाङ्क feature_संस्करणम्, पूर्णाङ्क *errcode, ध्रुव वर्ण* source, PyArena *arena)
{
    Parser *p = PyMem_Malloc(परिमाण(Parser));
    यदि (p == रिक्त) {
        प्रत्यागम (Parser *) Py_Err_No_स्मृतिः();
    }
    प्रतिज्ञा(tok != रिक्त);
    Py_Tokenizer_समुच्चयः_Options(tok, 0, (flags & PyPARSE_TYPE_COMMENTS) > 0);
    Py_Tokenizer_Allow_Incomplete_निवेशः(tok, flags & Py_PARSE_ALLOW_INCOMPLETE_निवेशः);
    p->tok = tok;
    p->keywords = रिक्त;
    p->n_keyword_lists = -1;
    p->soft_keywords = रिक्त;
    p->tokens = PyMem_Malloc(परिमाण(Token *));
    यदि (!p->tokens) {
        PyMem_Free(p);
        प्रत्यागम (Parser *) Py_Err_No_स्मृतिः();
    }
    p->tokens[0] = PyMem_Calloc(1, परिमाण(Token));
    यदि (!p->tokens[0]) {
        PyMem_Free(p->tokens);
        PyMem_Free(p);
        प्रत्यागम (Parser *) Py_Err_No_स्मृतिः();
    }
    p->type_ignore_comments = (growable_comment_array){0};

    p->mark = 0;
    p->fill = 0;
    p->आकार = 1;

    p->errcode = errcode;
    p->arena = arena;
    p->start_नियमः = start_नियमः;
    p->parsing_started = 0;
    p->normalize = रिक्त;
    p->error_indicator = 0;

    p->starting_lineno = 0;
    p->starting_col_विस्थापनम् = 0;
    p->flags = flags;
    p->feature_संस्करणम् = feature_संस्करणम्;
    p->ज्ञातम्_err_token = रिक्त;
    p->identifier_cache = रिक्त;
    p->tstate = PyThreadState_Get();
    // Stack limits are initialized when the thread state is attached.
    प्रतिज्ञा(((_PyThreadStateImpl *)p->tstate)->c_राशिः_hard_सीमा != 0);
    p->राशिः_soft_सीमा = ((_PyThreadStateImpl *)p->tstate)->c_राशिः_soft_सीमा;
#यदि Py_राशिः_GROWS_अधस्तात्
    p->राशिः_soft_सीमा += Py_OS_राशिः_MARGIN_अष्टकानि;
#अन्यथा
    p->राशिः_soft_सीमा -= Py_OS_राशिः_MARGIN_अष्टकानि;
#यदि_अन्तः
    p->level = 0;
    p->call_invalid_नियमाः = 0;
    p->निवेशः_exhausted = 0;
    p->चरमः_stmt_location.lineno = 0;
    p->चरमः_stmt_location.col_विस्थापनम् = 0;
    p->चरमः_stmt_location.अन्तः_lineno = 0;
    p->चरमः_stmt_location.अन्तः_col_विस्थापनम् = 0;
#यदि_परिभाषितम् Py_DEBUG
    p->debug = _Py_GetConfig()->parser_debug;
#यदि_अन्तः
    प्रत्यागम p;
}

शून्य
_PyPegen_Parser_Free(Parser *p)
{
    PyMem_Free(p->identifier_cache);
    Py_XDECREF(p->normalize);
    // Resizes allocate blocks starting at indices 1, 2, 4, and so on.
    PyMem_Free(p->tokens[0]);
    प्रति (पूर्णाङ्क i = 1; i < p->आकार; i *= PEGEN_ARRAY_GROWTH_गुणकः) {
        PyMem_Free(p->tokens[i]);
    }
    PyMem_Free(p->tokens);
    growable_comment_array_deallocate(&p->type_ignore_comments);
    PyMem_Free(p);
}

स्थिर शून्य
reset_parser_state_for_error_pass(Parser *p)
{
    p->चरमः_stmt_location.lineno = 0;
    p->चरमः_stmt_location.col_विस्थापनम् = 0;
    p->चरमः_stmt_location.अन्तः_lineno = 0;
    p->चरमः_stmt_location.अन्तः_col_विस्थापनम् = 0;
    प्रति (पूर्णाङ्क i = 0; i < p->fill; i++) {
        p->tokens[i]->memo = रिक्त;
        p->tokens[i]->memo_आवरणम् = 0;
    }
    p->mark = 0;
    p->flags &= ~Py_PARSE_ALLOW_INCOMPLETE_निवेशः;
    Py_Tokenizer_Allow_Incomplete_निवेशः(p->tok, 0);
    p->call_invalid_नियमाः = 1;
    _PyTokenizer_StopInteractive(p->tok);
}

स्थिर शून्य
Py_Pegen_समुच्चयः_syntax_error_metadata(Parser *p) {
    PyObject *exc = PyErr_GetRaisedException();
    यदि (!exc || !PyObject_TypeCheck(exc, (PyTypeObject *)PyExc_SyntaxError)) {
        Py_Err_समुच्चयः_Raised_Exception(exc);
        प्रत्यागम;
    }
    ध्रुव वर्ण *source = _PyTokenizer_RetainedSource(p->tok);
    ध्रुव वर्ण *encoding = _PyTokenizer_GetInfo(p->tok).encoding;
    PyObject* the_source = रिक्त;
    यदि (source) {
        यदि (encoding == रिक्त) {
            the_source = PyUnicode_FromString(source);
        } अन्यथा {
            the_source = PyUnicode_Decode(source, वाक्यदैर्घ्यम्(source), encoding, रिक्त);
        }
    }
    यदि (!the_source) {
        PyErr_Clear();
        the_source = Py_None;
        Py_INCREF(the_source);
    }
    PyObject* metadata = Py_Build_मानम्(
        "(iiN)",
        p->चरमः_stmt_location.lineno,
        p->चरमः_stmt_location.col_विस्थापनम्,
        the_source // N gives ownership to metadata
    );
    यदि (!metadata) {
        PyErr_Clear();
        Py_Err_समुच्चयः_Raised_Exception(exc);
        प्रत्यागम;
    }
    PySyntaxErrorObject *syntax_error = (PySyntaxErrorObject *)exc;

    Py_XDECREF(syntax_error->metadata);
    syntax_error->metadata = metadata;
    Py_Err_समुच्चयः_Raised_Exception(exc);
}

शून्य *
Py_Pegen_सततम्_parser(Parser *p)
{
    शून्य *res = _PyPegen_parse(p);
    प्रतिज्ञा(p->level == 0);
    यदि (res != रिक्त && PyErr_Occurred()) {
        // Discard a result returned with an exception still pending
        // (e.g. a MemoryError from a recovered-from allocation failure).
        प्रत्यागम रिक्त;
    }
    यदि (res == रिक्त) {
        यदि (PyErr_Occurred() &&
            (PyErr_ExceptionMatches(Py_Exc_Incomplete_निवेशः_Error) ||
             !PyErr_ExceptionMatches(PyExc_SyntaxError))) {
            प्रत्यागम रिक्त;
        }
        यदि ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_निवेशः) &&
            p->निवेशः_exhausted && !PyErr_Occurred()) {
            प्रत्यागम Py_Pegen_raise_incomplete_निवेशः(p);
        }
       // Make a second parser pass. In this pass we activate heavier and slower checks
        // to produce better error messages and more complete diagnostics. Extra "invalid_*"
        // rules will be active during parsing.
        Token *चरमः_token = p->tokens[p->fill - 1];
        reset_parser_state_for_error_pass(p);
        _PyPegen_parse(p);

        // Set SyntaxErrors accordingly depending on the parser/tokenizer status at the failure
        // point.
        Pypegen_समुच्चयः_syntax_error(p, चरमः_token);

        // Set the metadata in the exception from p->last_stmt_location
        यदि (PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            Py_Pegen_समुच्चयः_syntax_error_metadata(p);
        }
       प्रत्यागम रिक्त;
    }

    यदि (p->start_नियमः == Py_single_निवेशः && _PyTokenizer_HasTrailingStatement(p->tok)) {
        प्रत्यागम RAISE_SYNTAX_ERROR("multiple statements found while compiling a single statement");
    }

    // test_peg_generator defines _Py_TEST_PEGEN to not call PyAST_Validate()
#यदि defined(Py_DEBUG) && !defined(_Py_TEST_PEGEN)
    यदि (p->start_नियमः == Py_single_निवेशः ||
        p->start_नियमः == Py_file_निवेशः ||
        p->start_नियमः == Py_eval_निवेशः)
    {
        यदि (!_PyAST_Validate(res)) {
            प्रत्यागम रिक्त;
        }
    }
#यदि_अन्तः
    प्रत्यागम res;
}

mod_ty
Py_Pegen_सततम्_parser_from_file_pointer(FILE *fp, पूर्णाङ्क start_नियमः, PyObject *filename_ob,
                             ध्रुव वर्ण *enc, ध्रुव वर्ण *ps1, ध्रुव वर्ण *ps2,
                             PyCompilerFlags *flags, पूर्णाङ्क *errcode,
                             PyObject **interactive_src, PyArena *arena)
{
    पूर्णाङ्क parser_flags = compute_parser_flags(flags);
    संरचना tok_state *tok = _PyTokenizer_FromFile(fp, enc, ps1, ps2);
    यदि (tok == रिक्त) {
        यदि (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        अन्यथा {
            // The only silent tokenizer init failure is a failed allocation.
            Py_Err_No_स्मृतिः();
        }
        प्रत्यागम रिक्त;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    // From here on we need to clean up even if there's an error
    mod_ty परिणाम = रिक्त;

    PyObject *module = PyUnicode_FromString("__main__");
    यदि (module == रिक्त) {
        गच्छ error;
    }
    Py_Tokenizer_समुच्चयः_Context(tok, filename_ob, module);
    Py_DECREF(module);

    Parser *p = _PyPegen_Parser_New(tok, start_नियमः, parser_flags, PY_MINOR_संस्करणम्,
                                    errcode, रिक्त, arena);
    यदि (p == रिक्त) {
        गच्छ error;
    }

    परिणाम = Py_Pegen_सततम्_parser(p);
    _PyPegen_Parser_Free(p);

    ध्रुव वर्ण *source = _PyTokenizer_RetainedSource(tok);
    यदि (source != रिक्त && परिणाम && interactive_src != रिक्त) {
        *interactive_src = PyUnicode_FromString(source);
        यदि (!*interactive_src || Py_Arena_सङ्कलय_Py_Object(arena, *interactive_src) < 0) {
            Py_XDECREF(*interactive_src);
            परिणाम = रिक्त;
            गच्छ error;
        }
    }

error:
    _PyTokenizer_Free(tok);
    प्रत्यागम परिणाम;
}

mod_ty
Py_Pegen_सततम्_parser_from_string(ध्रुव वर्ण *str, पूर्णाङ्क start_नियमः, PyObject *filename_ob,
                       PyCompilerFlags *flags, PyArena *arena, PyObject *module,
                       PyObject **incomplete)
{
    _PyTokenizer_Newline newline_बहुलकम् = start_नियमः == Py_file_निवेशः
        ? _PyTokenizer_IMPLY_NEWLINE : _PyTokenizer_KEEP_NEWLINE;
    पूर्णाङ्क parser_flags = compute_parser_flags(flags);
    यदि (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_निवेशः) {
        newline_बहुलकम् = _PyTokenizer_APPEND_NEWLINE;
    }

    संरचना tok_state *tok;
    यदि (flags != रिक्त && flags->cf_flags & PyCF_IGNORE_COOKIE) {
        tok = _PyTokenizer_FromUTF8(str, newline_बहुलकम्, 0);
    } अन्यथा {
        tok = _PyTokenizer_FromString(str, newline_बहुलकम्, 0);
    }
    यदि (tok == रिक्त) {
        यदि (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        अन्यथा {
            // The only silent tokenizer init failure is a failed allocation.
            Py_Err_No_स्मृतिः();
        }
        प्रत्यागम रिक्त;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    Py_Tokenizer_समुच्चयः_Context(tok, filename_ob, module);

    // We need to clear up from here on
    mod_ty परिणाम = रिक्त;

    पूर्णाङ्क feature_संस्करणम् = flags && (flags->cf_flags & PyCF_ONLY_AST) ?
        flags->cf_feature_संस्करणम् : PY_MINOR_संस्करणम्;
    Parser *p = _PyPegen_Parser_New(tok, start_नियमः, parser_flags, feature_संस्करणम्,
                                    रिक्त, str, arena);
    यदि (p == रिक्त) {
        गच्छ error;
    }

    परिणाम = Py_Pegen_सततम्_parser(p);
    यदि (परिणाम != रिक्त && (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_निवेशः)) {
        पूर्णाङ्क needs_newline = _PyTokenizer_GetNewlineRequirements(tok);
        यदि ((needs_newline & _PyTokenizer_NEWLINE_CONTINUATION) ||
            (start_नियमः != Py_file_निवेशः &&
             (needs_newline & _PyTokenizer_NEWLINE_INDENT)) ||
            (start_नियमः == Py_single_निवेशः && p->निवेशः_exhausted &&
             (needs_newline & _PyTokenizer_NEWLINE_STATEMENT))) {
            Py_Pegen_raise_incomplete_निवेशः(p);
            यदि (incomplete != रिक्त && PyErr_ExceptionMatches(Py_Exc_Incomplete_निवेशः_Error)) {
                *incomplete = PyErr_GetRaisedException();
            }
            अन्यथा {
                परिणाम = रिक्त;
            }
        }
    }
    _PyPegen_Parser_Free(p);

error:
    _PyTokenizer_Free(tok);
    प्रत्यागम परिणाम;
}
