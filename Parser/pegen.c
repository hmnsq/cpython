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

#تعریف_کرو IDENTIFIER_CACHE_سائز 2048  // Must be a power of two.
#تعریف_کرو IDENTIFIER_CACHE_MAX_PROBES 8
#تعریف_کرو PEGEN_ARRAY_GROWTH_ضربی_عدد 2
#تعریف_کرو TYPE_IGNORE_INITIAL_استعداد 10

ساخت identifier_cache_اندراج {
    ثابت حرف *کلید;   // Borrowed from arena-owned token bytes.
    Py_ssize_t len;
    Py_hash_t hash;
    PyObject *قدر;   // Borrowed from an arena-owned identifier.
};

// Internal parser functions

asdl_stmt_seq*
_PyPegen_interactive_exit(Parser *p)
{
    اگر (p->errcode) {
        *(p->errcode) = E_EOF;
    }
    واپس تہی;
}

Py_ssize_t
Py_Pegen_byte_انحراف_مقصد_character_انحراف_سطر(PyObject *سطر, Py_ssize_t ستون_انحراف, Py_ssize_t انتہا_ستون_انحراف)
{
    ثابت غیرعلامتی حرف *ڈیٹا = (ثابت غیرعلامتی حرف*)PyUnicode_AsUTF8(سطر);
    اگر (ڈیٹا == تہی) {
        واپس -1;
    }

    Py_ssize_t len = 0;
    جب (ستون_انحراف < انتہا_ستون_انحراف) {
        Py_UCS4 ch = ڈیٹا[ستون_انحراف];
        اگر (ch < 0x80) {
            ستون_انحراف += 1;
        } ورنہ اگر ((ch & 0xe0) == 0xc0) {
            ستون_انحراف += 2;
        } ورنہ اگر ((ch & 0xf0) == 0xe0) {
            ستون_انحراف += 3;
        } ورنہ اگر ((ch & 0xf8) == 0xf0) {
            ستون_انحراف += 4;
        } ورنہ {
            Py_Err_سیٹ_String(Py_Exc_قدر_Error, "Invalid UTF-8 sequence");
            واپس -1;
        }
        len++;
    }
    واپس len;
}

Py_ssize_t
Py_Pegen_byte_انحراف_مقصد_character_انحراف_raw(ثابت حرف* str, Py_ssize_t ستون_انحراف)
{
    Py_ssize_t len = (Py_ssize_t)لڑی_لمبائی(str);
    اگر (ستون_انحراف > len + 1) {
        ستون_انحراف = len + 1;
    }
    أكد(ستون_انحراف >= 0);
    PyObject *متن = PyUnicode_DecodeUTF8(str, ستون_انحراف, "replace");
    اگر (!متن) {
        واپس -1;
    }
    Py_ssize_t سائز = Py_Unicode_GET_لمبائی(متن);
    Py_DECREF(متن);
    واپس سائز;
}

Py_ssize_t
Py_Pegen_byte_انحراف_مقصد_character_انحراف(PyObject *سطر, Py_ssize_t ستون_انحراف)
{
    ثابت حرف *str = PyUnicode_AsUTF8(سطر);
    اگر (!str) {
        واپس -1;
    }
    واپس Py_Pegen_byte_انحراف_مقصد_character_انحراف_raw(str, ستون_انحراف);
}

// Here, mark is the start of the node, while p->mark is the end.
// If node==NULL, they should be the same.
صحیح
Py_Pegen_داخل_کرو_یادداشت(Parser *p, صحیح mark, صحیح type, خالی *گرہ)
{
    // Insert in front
    یادداشت *m = _PyArena_Malloc(p->arena, حجم(یادداشت));
    اگر (m == تہی) {
        واپس -1;
    }
    m->type = type;
    m->گرہ = گرہ;
    m->mark = p->mark;
    m->اگلا = p->علامات[mark]->یادداشت_3;
    p->علامات[mark]->یادداشت_3 = m;
    p->علامات[mark]->یادداشت_نقاب |= 1ULL << (type & 63);
    واپس 0;
}

// Like _PyPegen_insert_memo(), but returns the inserted Memo so callers
// can update it in place without re-walking the token's memo list.
یادداشت *
Py_Pegen_داخل_کرو_یادداشت_براہ_راست(Parser *p, صحیح mark, صحیح type)
{
    اگر (Py_Pegen_داخل_کرو_یادداشت(p, mark, type, تہی) < 0) {
        واپس تہی;
    }
    واپس p->علامات[mark]->یادداشت_3;
}

// Like _PyPegen_insert_memo(), but updates an existing node if found.
صحیح
Py_Pegen_update_یادداشت(Parser *p, صحیح mark, صحیح type, خالی *گرہ)
{
    برائے (یادداشت *m = p->علامات[mark]->یادداشت_3; m != تہی; m = m->اگلا) {
        اگر (m->type == type) {
            // Update existing node.
            m->گرہ = گرہ;
            m->mark = p->mark;
            واپس 0;
        }
    }
    // Insert new node.
    واپس Py_Pegen_داخل_کرو_یادداشت(p, mark, type, گرہ);
}

ساکن صحیح
init_normalization(Parser *p)
{
    اگر (p->normalize) {
        واپس 1;
    }
    p->normalize = PyImport_ImportModuleAttrString("unicodedata", "normalize");
    اگر (!p->normalize)
    {
        واپس 0;
    }
    واپس 1;
}

صحیح
Py_Pegen_grow_loop_بفر(خالی ***بفر, Py_ssize_t *استعداد)
{
    اگر ((سائز_t)*استعداد > (سائز_t)PY_SSIZE_T_MAX /
            (PEGEN_ARRAY_GROWTH_ضربی_عدد * حجم(**بفر))) {
        واپس -1;
    }
    Py_ssize_t new_استعداد = *استعداد == 0
        ? 1 : *استعداد * PEGEN_ARRAY_GROWTH_ضربی_عدد;
    خالی **new_بفر = PyMem_Realloc(*بفر, new_استعداد * حجم(**بفر));
    اگر (new_بفر == تہی) {
        واپس -1;
    }
    *بفر = new_بفر;
    *استعداد = new_استعداد;
    واپس 0;
}

ساکن صحیح
growable_comment_array_جوڑو(growable_comment_array *arr, صحیح lineno, حرف *comment)
{
    اگر (arr->عدد_اشیاء >= arr->سائز) {
        اگر (arr->سائز > (سائز_t)PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_ضربی_عدد * حجم(*arr->اشیاء))) {
            واپس 0;
        }
        سائز_t new_سائز = arr->سائز == 0
            ? TYPE_IGNORE_INITIAL_استعداد
            : arr->سائز * PEGEN_ARRAY_GROWTH_ضربی_عدد;
        خالی *new_اشیاء_array = PyMem_Realloc(
            arr->اشیاء, new_سائز * حجم(*arr->اشیاء));
        اگر (!new_اشیاء_array) {
            واپس 0;
        }
        arr->اشیاء = new_اشیاء_array;
        arr->سائز = new_سائز;
    }

    arr->اشیاء[arr->عدد_اشیاء].lineno = lineno;
    arr->اشیاء[arr->عدد_اشیاء].comment = comment;  // Take ownership
    arr->عدد_اشیاء++;
    واپس 1;
}

ساکن خالی
growable_comment_array_deallocate(growable_comment_array *arr)
{
    برائے (سائز_t i = 0; i < arr->عدد_اشیاء; i++) {
        PyMem_Free(arr->اشیاء[i].comment);
    }
    PyMem_Free(arr->اشیاء);
}

ساکن صحیح
_get_keyword_or_name_type(Parser *p, ثابت حرف *متن, Py_ssize_t لمبائی_2)
{
    أكد(لمبائی_2 > 0);

    اگر (لمبائی_2 >= p->n_keyword_lists ||
        p->keywords[لمبائی_2] == تہی ||
        p->keywords[لمبائی_2]->type == -1) {
        واپس NAME;
    }
    برائے (KeywordToken *k = p->keywords[لمبائی_2]; k != تہی && k->type != -1; k++) {
        اگر (memcmp(k->str, متن, (سائز_t)لمبائی_2) == 0) {
            واپس k->type;
        }
    }
    واپس NAME;
}

// Keep text only where grammar actions or helpers read it. Keyword error
// actions use fixed spellings, so keywords do not need their text.
ساکن اندرونی صحیح
token_needs_متن(صحیح type)
{
    انتخاب (type) {
        حالت NAME:
        حالت نمبر:
        حالت STRING:
        حالت FSTRING_آغاز:
        حالت FSTRING_درمیان:
        حالت FSTRING_انتہا:
        حالت TSTRING_آغاز:
        حالت TSTRING_درمیان:
        حالت TSTRING_انتہا:
        حالت TYPE_COMMENT:
        حالت NOTEQUAL:  // _PyPegen_check_barry_as_flufl() distinguishes != and <>.
            واپس 1;
        طےشدہ:
            واپس 0;
    }
}

ساکن صحیح
initialize_token(Parser *p, Token *parser_token, ساخت token *new_token, صحیح token_type) {
    أكد(parser_token != تہی);

    parser_token->type = token_type;
    parser_token->بائٹیں = تہی;
    اگر (token_needs_متن(token_type) && new_token->وسعت.آغاز >= 0) {
        Py_ssize_t لمبائی_2;
        ثابت حرف *متن = Py_Token_متن_View(p->tok, new_token, &لمبائی_2);
        اگر (token_type == NAME) {
            parser_token->type = _get_keyword_or_name_type(p, متن, لمبائی_2);
        }
        // A name reclassified as a keyword does not need its text.
        اگر (parser_token->type == token_type) {
            parser_token->بائٹیں = Py_بائٹیں_From_String_And_سائز(متن, لمبائی_2);
            اگر (parser_token->بائٹیں == تہی) {
                واپس -1;
            }
            اگر (Py_Arena_جوڑو_Py_Object(p->arena, parser_token->بائٹیں) < 0) {
                Py_DECREF(parser_token->بائٹیں);
                واپس -1;
            }
        }
    }

    parser_token->metadata = تہی;
    اگر (new_token->metadata != تہی) {
        اگر (Py_Arena_جوڑو_Py_Object(p->arena, new_token->metadata) < 0) {
            واپس -1;
        }
        parser_token->metadata = new_token->metadata;
        new_token->metadata = تہی;
    }

    parser_token->level = new_token->level;
    parser_token->is_raw = new_token->is_raw;
    parser_token->lineno = new_token->آغاز_loc.lineno;
    parser_token->ستون_انحراف = new_token->انتہا_loc.lineno == p->starting_lineno
        ? p->starting_ستون_انحراف + new_token->آغاز_loc.byte_ستون
        : new_token->آغاز_loc.byte_ستون;
    parser_token->انتہا_lineno = new_token->انتہا_loc.lineno;
    parser_token->انتہا_ستون_انحراف = new_token->انتہا_loc.lineno == p->starting_lineno
        ? p->starting_ستون_انحراف + new_token->انتہا_loc.byte_ستون
        : new_token->انتہا_loc.byte_ستون;

    p->fill += 1;

    اگر (token_type == ERRORTOKEN && Py_Tokenizer_Get_حالت(p->tok) == E_DECODE) {
        واپس _Pypegen_raise_decode_error(p);
    }

    واپس (token_type == ERRORTOKEN ? _Pypegen_tokenizer_error(p) : 0);
}

ساکن صحیح
resize_علامات_array(Parser *p) {
    اگر (p->سائز > زیادہ_سے_زیادہ_عدد / PEGEN_ARRAY_GROWTH_ضربی_عدد ||
            (سائز_t)p->سائز > PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_ضربی_عدد * حجم(*p->علامات))) {
        Py_Err_No_حافظہ();
        واپس -1;
    }
    صحیح newsize = p->سائز * PEGEN_ARRAY_GROWTH_ضربی_عدد;
    Token **new_علامات = PyMem_Realloc(p->علامات, (سائز_t)newsize * حجم(Token *));
    اگر (new_علامات == تہی) {
        Py_Err_No_حافظہ();
        واپس -1;
    }
    p->علامات = new_علامات;

    // Allocate new tokens together without moving existing ones: parser rules
    // may still hold pointers into earlier blocks. Parser_Free frees each block.
    Token *chunk = PyMem_Calloc((سائز_t)(newsize - p->سائز), حجم(Token));
    اگر (chunk == تہی) {
        Py_Err_No_حافظہ();
        واپس -1;
    }
    برائے (صحیح i = p->سائز; i < newsize; i++) {
        p->علامات[i] = &chunk[i - p->سائز];
    }
    p->سائز = newsize;
    واپس 0;
}

صحیح
_PyPegen_fill_token(Parser *p)
{
    ساخت token new_token;
    _PyToken_Init(&new_token);
    _PyTokenizer_Get(p->tok, &new_token);

    // Record and skip '# type: ignore' comments
    جب (new_token.type == TYPE_IGNORE) {
        Py_ssize_t len;
        ثابت حرف *متن = Py_Token_متن_View(p->tok, &new_token, &len);
        حرف *لیبل = PyMem_Malloc((سائز_t)len + 1);
        اگر (لیبل == تہی) {
            Py_Err_No_حافظہ();
            جاؤ error;
        }
        حافظہ_نقل(لیبل, متن, (سائز_t)len);
        لیبل[len] = '\0';
        // Ownership of tag passes to the growable array
        اگر (!growable_comment_array_جوڑو(&p->type_ignore_comments, new_token.انتہا_loc.lineno, لیبل)) {
            PyMem_Free(لیبل);
            Py_Err_No_حافظہ();
            جاؤ error;
        }
        _PyTokenizer_Get(p->tok, &new_token);
    }

    صحیح type = new_token.type;
    // EOF can finish an ordinary token. Only a boundary token means that
    // the grammar requested input beyond the last physical line.
    اگر (type == ENDMARKER ||
        (type == DEDENT && (p->flags & Py_PARSE_ALLOW_INCOMPLETE_داخلہ) &&
         Py_Tokenizer_Get_حالت(p->tok) == E_EOF)) {
        p->داخلہ_exhausted = 1;
    }

    // If we have reached the end and we are in single input mode we need to insert a newline and reset the parsing
    اگر (p->آغاز_قاعدہ == Py_single_داخلہ && type == ENDMARKER && p->parsing_started) {
        type = NEWLINE; /* Add an extra newline */
        p->parsing_started = 0;

        اگر (!(p->flags & PyPARSE_DONT_IMPLY_DEDENT)) {
            _PyTokenizer_ImplyDedents(p->tok);
        }
    }
    ورنہ {
        p->parsing_started = 1;
    }

    // Check if we are at the limit of the token array capacity and resize if needed
    اگر ((p->fill == p->سائز) && (resize_علامات_array(p) != 0)) {
        جاؤ error;
    }

    Token *t = p->علامات[p->fill];
    صحیح نتیجہ = initialize_token(p, t, &new_token, type);
    _PyToken_Free(&new_token);
    واپس نتیجہ;
error:
    _PyToken_Free(&new_token);
    واپس -1;
}

#اگر defined(Py_DEBUG)
// Instrumentation to count the effectiveness of memoization.
// The array counts the number of tokens skipped by memoization,
// indexed by type.

#تعریف_کرو NSTATISTICS _PYPEGEN_NSTATISTICS
#تعریف_کرو یادداشت_statistics _PyRuntime.parser.یادداشت_statistics

خالی
Py_Pegen_clear_یادداشت_statistics(خالی)
{
    Py_Mutex_تالا(&_PyRuntime.parser.mutex);
    برائے (صحیح i = 0; i < NSTATISTICS; i++) {
        یادداشت_statistics[i] = 0;
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
}

PyObject *
Py_Pegen_get_یادداشت_statistics(خالی)
{
    PyObject *ret = PyList_New(NSTATISTICS);
    اگر (ret == تہی) {
        واپس تہی;
    }

    Py_Mutex_تالا(&_PyRuntime.parser.mutex);
    برائے (صحیح i = 0; i < NSTATISTICS; i++) {
        PyObject *قدر = PyLong_FromLong(یادداشت_statistics[i]);
        اگر (قدر == تہی) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            واپس تہی;
        }
        // PyList_SetItem borrows a reference to value.
        اگر (Py_List_سیٹ_شے(ret, i, قدر) < 0) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            واپس تہی;
        }
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
    واپس ret;
}
#اگر_ختم

صحیح  // bool
_PyPegen_is_memoized(Parser *p, صحیح type, خالی *pres)
{
    اگر (p->mark == p->fill) {
        اگر (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            واپس -1;
        }
    }

    Token *t = p->علامات[p->mark];

    اگر (!(t->یادداشت_نقاب & (1ULL << (type & 63)))) {
        واپس 0;
    }

    برائے (یادداشت *m = t->یادداشت_3; m != تہی; m = m->اگلا) {
        اگر (m->type == type) {
#اگر defined(Py_DEBUG)
            اگر (0 <= type && type < NSTATISTICS) {
                طویل گنتی = m->mark - p->mark;
                // A memoized negative result counts for one.
                اگر (گنتی <= 0) {
                    گنتی = 1;
                }
                Py_Mutex_تالا(&_PyRuntime.parser.mutex);
                یادداشت_statistics[type] += گنتی;
                PyMutex_Unlock(&_PyRuntime.parser.mutex);
            }
#اگر_ختم
            p->mark = m->mark;
            *(خالی **)(pres) = m->گرہ;
            واپس 1;
        }
    }
    واپس 0;
}

#تعریف_کرو LOOKAHEAD1(NAME, RES_TYPE)                                  \
    صحیح                                                             \
    NAME (صحیح مثبت, RES_TYPE (func)(Parser *), Parser *p)       \
    {                                                               \
        صحیح mark = p->mark;                                         \
        خالی *res = func(p);                                        \
        p->mark = mark;                                             \
        واپس (res != تہی) == مثبت;                           \
    }

LOOKAHEAD1(_PyPegen_lookahead, خالی *)
LOOKAHEAD1(Py_Pegen_lookahead_for_اظہاریہ, اظہاریہ_ty)
LOOKAHEAD1(_PyPegen_lookahead_for_stmt, stmt_ty)
#تعریف_منسوخ LOOKAHEAD1

#تعریف_کرو LOOKAHEAD2(NAME, RES_TYPE, T)                                   \
    صحیح                                                                 \
    NAME (صحیح مثبت, RES_TYPE (func)(Parser *, T), Parser *p, T arg) \
    {                                                                   \
        صحیح mark = p->mark;                                             \
        خالی *res = func(p, arg);                                       \
        p->mark = mark;                                                 \
        واپس (res != تہی) == مثبت;                               \
    }

LOOKAHEAD2(_PyPegen_lookahead_with_int, Token *, صحیح)
LOOKAHEAD2(_PyPegen_lookahead_with_string, اظہاریہ_ty, ثابت حرف *)
#تعریف_منسوخ LOOKAHEAD2

Token *
_PyPegen_expect_token(Parser *p, صحیح type)
{
    اگر (p->mark == p->fill) {
        اگر (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            واپس تہی;
        }
    }
    Token *t = p->علامات[p->mark];
    اگر (t->type != type) {
       واپس تہی;
    }
    p->mark += 1;
    واپس t;
}

خالی*
Py_Pegen_expect_forced_نتیجہ(Parser *p, خالی* نتیجہ, ثابت حرف* expected) {

    اگر (p->error_indicator == 1) {
        واپس تہی;
    }
    اگر (نتیجہ == تہی) {
        RAISE_SYNTAX_ERROR("expected (%s)", expected);
        واپس تہی;
    }
    واپس نتیجہ;
}

Token *
_PyPegen_expect_forced_token(Parser *p, صحیح type, ثابت حرف* expected) {

    اگر (p->error_indicator == 1) {
        واپس تہی;
    }

    اگر (p->mark == p->fill) {
        اگر (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            واپس تہی;
        }
    }
    Token *t = p->علامات[p->mark];
    اگر (t->type != type) {
        RAISE_SYNTAX_ERROR_معلوم_LOCATION(t, "expected '%s'", expected);
        واپس تہی;
    }
    p->mark += 1;
    واپس t;
}

اظہاریہ_ty
_PyPegen_expect_soft_keyword(Parser *p, ثابت حرف *keyword)
{
    اگر (p->mark == p->fill) {
        اگر (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            واپس تہی;
        }
    }
    Token *t = p->علامات[p->mark];
    اگر (t->type != NAME) {
        واپس تہی;
    }
    ثابت حرف *s = Py_بائٹیں_As_String(t->بائٹیں);
    اگر (!s) {
        p->error_indicator = 1;
        واپس تہی;
    }
    اگر (strcmp(s, keyword) != 0) {
        واپس تہی;
    }
    واپس _PyPegen_name_token(p);
}

Token *
Py_Pegen_get_آخری_nonnwhitespace_token(Parser *p)
{
    أكد(p->mark >= 0);
    Token *token = تہی;
    برائے (صحیح m = p->mark - 1; m >= 0; m--) {
        token = p->علامات[m];
        اگر (token->type != ENDMARKER && (token->type < NEWLINE || token->type > DEDENT)) {
            توڑو;
        }
    }
    واپس token;
}

PyObject *
_PyPegen_new_identifier(Parser *p, ثابت حرف *n)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, (Py_ssize_t)لڑی_لمبائی(n), تہی);
    اگر (!id) {
        جاؤ error;
    }
    /* Check whether there are non-ASCII characters in the
       identifier; if so, normalize to NFKC. */
    اگر (!PyUnicode_IS_ASCII(id))
    {
        اگر (!init_normalization(p))
        {
            Py_DECREF(id);
            جاؤ error;
        }
        PyObject *form = PyUnicode_InternFromString("NFKC");
        اگر (form == تہی)
        {
            Py_DECREF(id);
            جاؤ error;
        }
        PyObject *دلائل[2] = {form, id};
        PyObject *id2 = PyObject_Vectorcall(p->normalize, دلائل, 2, تہی);
        Py_DECREF(id);
        Py_DECREF(form);
        اگر (!id2) {
            جاؤ error;
        }

        اگر (!PyUnicode_Check(id2))
        {
            PyErr_Format(PyExc_TypeError,
                         "unicodedata.normalize() must return a string, not "
                         "%.200s",
                         _PyType_Name(Py_TYPE(id2)));
            Py_DECREF(id2);
            جاؤ error;
        }
        id = id2;
    }
    ساکن ثابت حرف * ثابت forbidden[] = {
        "None",
        "True",
        "False",
        تہی
    };
    برائے (صحیح i = 0; forbidden[i] != تہی; i++) {
        اگر (Py_Unicode_Equal_مقصد_ASCIIString(id, forbidden[i])) {
            PyErr_Format(Py_Exc_قدر_Error,
                         "identifier field can't represent '%s' constant",
                         forbidden[i]);
            Py_DECREF(id);
            جاؤ error;
        }
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &id);
    اگر (Py_Arena_جوڑو_Py_Object(p->arena, id) < 0)
    {
        Py_DECREF(id);
        جاؤ error;
    }
    واپس id;

error:
    p->error_indicator = 1;
    واپس تہی;
}

// Return an arena-owned identifier; callers borrow the reference.
ساکن PyObject *
get_cached_identifier(Parser *p, PyObject *بائٹیں)
{
    ثابت حرف *s = Py_بائٹیں_As_String(بائٹیں);
    اگر (!s) {
        p->error_indicator = 1;
        واپس تہی;
    }
    // Identifiers repeat constantly; a small span-keyed cache skips the
    // UTF-8 decode + intern for repeated occurrences. Keys point into
    // arena-owned token bytes and values are arena-owned interned strings,
    // so borrowed references are valid for the lifetime of the parse
    // (including the second error pass, which reuses parser and arena).
    Py_ssize_t len = Py_بائٹیں_GET_سائز(بائٹیں);
    Py_hash_t hash = PyObject_Hash(بائٹیں);
    اگر (hash == -1) {
        p->error_indicator = 1;
        واپس تہی;
    }
    // Parses without identifiers do not need this cache.
    اگر (p->identifier_cache == تہی) {
        p->identifier_cache = PyMem_Calloc(
            IDENTIFIER_CACHE_سائز, حجم(*p->identifier_cache));
        اگر (p->identifier_cache == تہی) {
            p->error_indicator = 1;
            واپس Py_Err_No_حافظہ();
        }
    }
    Identifier_Cache_اندراج *free_خانہ = تہی;
    سائز_t idx = (سائز_t)hash & (IDENTIFIER_CACHE_سائز - 1);
    برائے (صحیح probe = 0; probe < IDENTIFIER_CACHE_MAX_PROBES; probe++) {
        Identifier_Cache_اندراج *اندراج_2 = &p->identifier_cache[
            (idx + probe) & (IDENTIFIER_CACHE_سائز - 1)];
        اگر (اندراج_2->کلید == تہی) {
            free_خانہ = اندراج_2;
            توڑو;
        }
        اگر (اندراج_2->hash == hash && اندراج_2->len == len &&
            memcmp(اندراج_2->کلید, s, len) == 0)
        {
            واپس اندراج_2->قدر;
        }
    }
    PyObject *id = _PyPegen_new_identifier(p, s);
    اگر (id == تہی) {
        p->error_indicator = 1;
        واپس تہی;
    }
    اگر (free_خانہ != تہی) {
        free_خانہ->کلید = s;
        free_خانہ->len = len;
        free_خانہ->hash = hash;
        free_خانہ->قدر = id;
    }
    واپس id;
}

ساکن اظہاریہ_ty
_PyPegen_name_from_token(Parser *p, Token* t)
{
    اگر (t == تہی) {
        واپس تہی;
    }
    // Reuse the AST node when backtracking revisits this token. Memo lookup
    // starts before the token and restores the position after it on a hit.
    // Token kinds can be memo keys: generated grammar rule IDs start at 1000.
    صحیح mark = p->mark - 1;
    p->mark = mark;
    اظہاریہ_ty cached = تہی;
    اگر (_PyPegen_is_memoized(p, NAME, &cached)) {
        واپس cached;
    }
    p->mark = mark + 1;
    PyObject *id = get_cached_identifier(p, t->بائٹیں);
    اگر (id == تہی) {
        واپس تہی;
    }
    اظہاریہ_ty نتیجہ = _PyAST_Name(id, بھرو, t->lineno, t->ستون_انحراف,
                                t->انتہا_lineno, t->انتہا_ستون_انحراف, p->arena);
    اگر (نتیجہ != تہی && Py_Pegen_داخل_کرو_یادداشت(p, mark, NAME, نتیجہ) < 0) {
        p->error_indicator = 1;
        واپس تہی;
    }
    واپس نتیجہ;
}

اظہاریہ_ty
_PyPegen_name_token(Parser *p)
{
    Token *t = _PyPegen_expect_token(p, NAME);
    واپس _PyPegen_name_from_token(p, t);
}

خالی *
_PyPegen_string_token(Parser *p)
{
    واپس _PyPegen_expect_token(p, STRING);
}

اظہاریہ_ty _PyPegen_soft_keyword_token(Parser *p) {
    Token *t = _PyPegen_expect_token(p, NAME);
    اگر (t == تہی) {
        واپس تہی;
    }
    حرف *the_token;
    Py_ssize_t سائز;
    Py_بائٹیں_As_String_And_سائز(t->بائٹیں, &the_token, &سائز);
    برائے (حرف **keyword = p->soft_keywords; *keyword != تہی; keyword++) {
        اگر (لڑی_لمبائی(*keyword) == (سائز_t)سائز &&
            لڑی_موازنہ(*keyword, the_token, (سائز_t)سائز) == 0) {
            واپس _PyPegen_name_from_token(p, t);
        }
    }
    واپس تہی;
}

ساکن PyObject *
parsenumber_raw(ثابت حرف *s)
{
    ثابت حرف *انتہا;
    طویل x;
    دوہرا dx;
    Py_complex compl;
    صحیح imflag;

    أكد(s != تہی);
    errno = 0;
    انتہا = s + لڑی_لمبائی(s) - 1;
    imflag = *انتہا == 'j' || *انتہا == 'J';
    اگر (s[0] == '0') {
        x = (طویل)PyOS_strtoul(s, (حرف **)&انتہا, 0);
        اگر (x < 0 && errno == 0) {
            واپس PyLong_FromString(s, (حرف **)0, 0);
        }
    }
    ورنہ {
        x = PyOS_strtol(s, (حرف **)&انتہا, 0);
    }
    اگر (*انتہا == '\0') {
        اگر (errno != 0) {
            واپس PyLong_FromString(s, (حرف **)0, 0);
        }
        واپس PyLong_FromLong(x);
    }
    /* XXX Huge floats may silently fail */
    اگر (imflag) {
        compl.real = 0.;
        compl.imag = Py_OS_string_مقصد_double(s, (حرف **)&انتہا, تہی);
        اگر (compl.imag == -1.0 && PyErr_Occurred()) {
            واپس تہی;
        }
        واپس PyComplex_FromCComplex(compl);
    }
    dx = Py_OS_string_مقصد_double(s, تہی, تہی);
    اگر (dx == -1.0 && PyErr_Occurred()) {
        واپس تہی;
    }
    واپس PyFloat_FromDouble(dx);
}

ساکن PyObject *
parsenumber(ثابت حرف *s)
{
    حرف *dup;
    حرف *انتہا;
    PyObject *res = تہی;

    أكد(s != تہی);

    اگر (strchr(s, '_') == تہی) {
        واپس parsenumber_raw(s);
    }
    /* Create a duplicate without underscores. */
    dup = PyMem_Malloc(لڑی_لمبائی(s) + 1);
    اگر (dup == تہی) {
        واپس Py_Err_No_حافظہ();
    }
    انتہا = dup;
    برائے (; *s; s++) {
        اگر (*s != '_') {
            *انتہا++ = *s;
        }
    }
    *انتہا = '\0';
    res = parsenumber_raw(dup);
    PyMem_Free(dup);
    واپس res;
}

اظہاریہ_ty
Py_Pegen_نمبر_token(Parser *p)
{
    صحیح mark = p->mark;
    Token *t = _PyPegen_expect_token(p, نمبر);
    اگر (t == تہی) {
        واپس تہی;
    }

    // Look up the node at the token's start, as for NAME tokens. A memo hit
    // advances the mark; on a miss, restore the position after the token.
    p->mark = mark;
    اظہاریہ_ty cached = تہی;
    اگر (_PyPegen_is_memoized(p, نمبر, &cached)) {
        واپس cached;
    }
    p->mark = mark + 1;

    ثابت حرف *عدد_raw = Py_بائٹیں_As_String(t->بائٹیں);
    اگر (عدد_raw == تہی) {
        p->error_indicator = 1;
        واپس تہی;
    }

    اگر (p->feature_نسخہ < 6 && strchr(عدد_raw, '_') != تہی) {
        p->error_indicator = 1;
        واپس RAISE_SYNTAX_ERROR("Underscores in numeric literals are only supported "
                                  "in Python 3.6 and greater");
    }

    PyObject *c = parsenumber(عدد_raw);

    اگر (c == تہی) {
        p->error_indicator = 1;
        PyThreadState *tstate = _PyThreadState_GET();
        // The only way a ValueError should happen in _this_ code is via
        // PyLong_FromString hitting a length limit.
        اگر (tstate->current_exception != تہی &&
            Py_TYPE(tstate->current_exception) == (PyTypeObject *)Py_Exc_قدر_Error
        ) {
            PyObject *exc = PyErr_GetRaisedException();
            /* Intentionally omitting columns to avoid a wall of 1000s of '^'s
             * on the error message. Nobody is going to overlook their huge
             * numeric literal once given the line. */
            RAISE_ERROR_معلوم_LOCATION(
                p, PyExc_SyntaxError,
                t->lineno, -1 /* col_offset */,
                t->انتہا_lineno, -1 /* end_col_offset */,
                "%S - Consider hexadecimal for huge integer literals "
                "to avoid decimal conversion limits.",
                exc);
            Py_DECREF(exc);
        }
        واپس تہی;
    }

    اگر (Py_Arena_جوڑو_Py_Object(p->arena, c) < 0) {
        Py_DECREF(c);
        p->error_indicator = 1;
        واپس تہی;
    }

    اظہاریہ_ty نتیجہ = _PyAST_Constant(c, تہی, t->lineno, t->ستون_انحراف,
                                    t->انتہا_lineno, t->انتہا_ستون_انحراف, p->arena);
    اگر (نتیجہ != تہی && Py_Pegen_داخل_کرو_یادداشت(p, mark, نمبر, نتیجہ) < 0) {
        p->error_indicator = 1;
        واپس تہی;
    }
    واپس نتیجہ;
}



ساکن صحیح
compute_parser_flags(PyCompilerFlags *flags)
{
    صحیح parser_flags = 0;
    اگر (!flags) {
        واپس 0;
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
    اگر (flags->cf_flags & Py_CF_ALLOW_INCOMPLETE_داخلہ) {
        parser_flags |= Py_PARSE_ALLOW_INCOMPLETE_داخلہ;
    }
    واپس parser_flags;
}

// Parser API

Parser *
_PyPegen_Parser_New(ساخت tok_state *tok, صحیح آغاز_قاعدہ, صحیح flags,
                    صحیح feature_نسخہ, صحیح *errcode, ثابت حرف* ماخذ, PyArena *arena)
{
    Parser *p = PyMem_Malloc(حجم(Parser));
    اگر (p == تہی) {
        واپس (Parser *) Py_Err_No_حافظہ();
    }
    أكد(tok != تہی);
    Py_Tokenizer_سیٹ_Options(tok, 0, (flags & PyPARSE_TYPE_COMMENTS) > 0);
    Py_Tokenizer_Allow_Incomplete_داخلہ(tok, flags & Py_PARSE_ALLOW_INCOMPLETE_داخلہ);
    p->tok = tok;
    p->keywords = تہی;
    p->n_keyword_lists = -1;
    p->soft_keywords = تہی;
    p->علامات = PyMem_Malloc(حجم(Token *));
    اگر (!p->علامات) {
        PyMem_Free(p);
        واپس (Parser *) Py_Err_No_حافظہ();
    }
    p->علامات[0] = PyMem_Calloc(1, حجم(Token));
    اگر (!p->علامات[0]) {
        PyMem_Free(p->علامات);
        PyMem_Free(p);
        واپس (Parser *) Py_Err_No_حافظہ();
    }
    p->type_ignore_comments = (growable_comment_array){0};

    p->mark = 0;
    p->fill = 0;
    p->سائز = 1;

    p->errcode = errcode;
    p->arena = arena;
    p->آغاز_قاعدہ = آغاز_قاعدہ;
    p->parsing_started = 0;
    p->normalize = تہی;
    p->error_indicator = 0;

    p->starting_lineno = 0;
    p->starting_ستون_انحراف = 0;
    p->flags = flags;
    p->feature_نسخہ = feature_نسخہ;
    p->معلوم_err_token = تہی;
    p->identifier_cache = تہی;
    p->tstate = PyThreadState_Get();
    // Stack limits are initialized when the thread state is attached.
    أكد(((_PyThreadStateImpl *)p->tstate)->c_ڈھیر_hard_حد_بندی != 0);
    p->ڈھیر_soft_حد_بندی = ((_PyThreadStateImpl *)p->tstate)->c_ڈھیر_soft_حد_بندی;
#اگر Py_ڈھیر_GROWS_نیچے_کو
    p->ڈھیر_soft_حد_بندی += Py_OS_ڈھیر_MARGIN_بائٹیں;
#ورنہ
    p->ڈھیر_soft_حد_بندی -= Py_OS_ڈھیر_MARGIN_بائٹیں;
#اگر_ختم
    p->level = 0;
    p->call_invalid_قواعد = 0;
    p->داخلہ_exhausted = 0;
    p->آخری_stmt_location.lineno = 0;
    p->آخری_stmt_location.ستون_انحراف = 0;
    p->آخری_stmt_location.انتہا_lineno = 0;
    p->آخری_stmt_location.انتہا_ستون_انحراف = 0;
#اگر_تعریف_شدہ Py_DEBUG
    p->debug = _Py_GetConfig()->parser_debug;
#اگر_ختم
    واپس p;
}

خالی
_PyPegen_Parser_Free(Parser *p)
{
    PyMem_Free(p->identifier_cache);
    Py_XDECREF(p->normalize);
    // Resizes allocate blocks starting at indices 1, 2, 4, and so on.
    PyMem_Free(p->علامات[0]);
    برائے (صحیح i = 1; i < p->سائز; i *= PEGEN_ARRAY_GROWTH_ضربی_عدد) {
        PyMem_Free(p->علامات[i]);
    }
    PyMem_Free(p->علامات);
    growable_comment_array_deallocate(&p->type_ignore_comments);
    PyMem_Free(p);
}

ساکن خالی
reset_parser_state_for_error_pass(Parser *p)
{
    p->آخری_stmt_location.lineno = 0;
    p->آخری_stmt_location.ستون_انحراف = 0;
    p->آخری_stmt_location.انتہا_lineno = 0;
    p->آخری_stmt_location.انتہا_ستون_انحراف = 0;
    برائے (صحیح i = 0; i < p->fill; i++) {
        p->علامات[i]->یادداشت_3 = تہی;
        p->علامات[i]->یادداشت_نقاب = 0;
    }
    p->mark = 0;
    p->flags &= ~Py_PARSE_ALLOW_INCOMPLETE_داخلہ;
    Py_Tokenizer_Allow_Incomplete_داخلہ(p->tok, 0);
    p->call_invalid_قواعد = 1;
    _PyTokenizer_StopInteractive(p->tok);
}

ساکن خالی
Py_Pegen_سیٹ_syntax_error_metadata(Parser *p) {
    PyObject *exc = PyErr_GetRaisedException();
    اگر (!exc || !PyObject_TypeCheck(exc, (PyTypeObject *)PyExc_SyntaxError)) {
        Py_Err_سیٹ_Raised_Exception(exc);
        واپس;
    }
    ثابت حرف *ماخذ = Py_Tokenizer_Retained_ماخذ(p->tok);
    ثابت حرف *encoding = _PyTokenizer_GetInfo(p->tok).encoding;
    PyObject* the_ماخذ = تہی;
    اگر (ماخذ) {
        اگر (encoding == تہی) {
            the_ماخذ = PyUnicode_FromString(ماخذ);
        } ورنہ {
            the_ماخذ = PyUnicode_Decode(ماخذ, لڑی_لمبائی(ماخذ), encoding, تہی);
        }
    }
    اگر (!the_ماخذ) {
        PyErr_Clear();
        the_ماخذ = Py_None;
        Py_INCREF(the_ماخذ);
    }
    PyObject* metadata = Py_Build_قدر(
        "(iiN)",
        p->آخری_stmt_location.lineno,
        p->آخری_stmt_location.ستون_انحراف,
        the_ماخذ // N gives ownership to metadata
    );
    اگر (!metadata) {
        PyErr_Clear();
        Py_Err_سیٹ_Raised_Exception(exc);
        واپس;
    }
    PySyntaxErrorObject *syntax_error = (PySyntaxErrorObject *)exc;

    Py_XDECREF(syntax_error->metadata);
    syntax_error->metadata = metadata;
    Py_Err_سیٹ_Raised_Exception(exc);
}

خالی *
Py_Pegen_تسلسل_parser(Parser *p)
{
    خالی *res = _PyPegen_parse(p);
    أكد(p->level == 0);
    اگر (res != تہی && PyErr_Occurred()) {
        // Discard a result returned with an exception still pending
        // (e.g. a MemoryError from a recovered-from allocation failure).
        واپس تہی;
    }
    اگر (res == تہی) {
        اگر (PyErr_Occurred() &&
            (PyErr_ExceptionMatches(Py_Exc_Incomplete_داخلہ_Error) ||
             !PyErr_ExceptionMatches(PyExc_SyntaxError))) {
            واپس تہی;
        }
        اگر ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_داخلہ) &&
            p->داخلہ_exhausted && !PyErr_Occurred()) {
            واپس Py_Pegen_raise_incomplete_داخلہ(p);
        }
       // Make a second parser pass. In this pass we activate heavier and slower checks
        // to produce better error messages and more complete diagnostics. Extra "invalid_*"
        // rules will be active during parsing.
        Token *آخری_token = p->علامات[p->fill - 1];
        reset_parser_state_for_error_pass(p);
        _PyPegen_parse(p);

        // Set SyntaxErrors accordingly depending on the parser/tokenizer status at the failure
        // point.
        Pypegen_سیٹ_syntax_error(p, آخری_token);

        // Set the metadata in the exception from p->last_stmt_location
        اگر (PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            Py_Pegen_سیٹ_syntax_error_metadata(p);
        }
       واپس تہی;
    }

    اگر (p->آغاز_قاعدہ == Py_single_داخلہ && _PyTokenizer_HasTrailingStatement(p->tok)) {
        واپس RAISE_SYNTAX_ERROR("multiple statements found while compiling a single statement");
    }

    // test_peg_generator defines _Py_TEST_PEGEN to not call PyAST_Validate()
#اگر defined(Py_DEBUG) && !defined(_Py_TEST_PEGEN)
    اگر (p->آغاز_قاعدہ == Py_single_داخلہ ||
        p->آغاز_قاعدہ == Py_file_داخلہ ||
        p->آغاز_قاعدہ == Py_eval_داخلہ)
    {
        اگر (!_PyAST_Validate(res)) {
            واپس تہی;
        }
    }
#اگر_ختم
    واپس res;
}

mod_ty
Py_Pegen_تسلسل_parser_from_file_اشارہ_گر(FILE *fp, صحیح آغاز_قاعدہ, PyObject *filename_ob,
                             ثابت حرف *enc, ثابت حرف *ps1, ثابت حرف *ps2,
                             PyCompilerFlags *flags, صحیح *errcode,
                             PyObject **interactive_src, PyArena *arena)
{
    صحیح parser_flags = compute_parser_flags(flags);
    ساخت tok_state *tok = _PyTokenizer_FromFile(fp, enc, ps1, ps2);
    اگر (tok == تہی) {
        اگر (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        ورنہ {
            // The only silent tokenizer init failure is a failed allocation.
            Py_Err_No_حافظہ();
        }
        واپس تہی;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    // From here on we need to clean up even if there's an error
    mod_ty نتیجہ = تہی;

    PyObject *module = PyUnicode_FromString("__main__");
    اگر (module == تہی) {
        جاؤ error;
    }
    Py_Tokenizer_سیٹ_Context(tok, filename_ob, module);
    Py_DECREF(module);

    Parser *p = _PyPegen_Parser_New(tok, آغاز_قاعدہ, parser_flags, PY_MINOR_نسخہ,
                                    errcode, تہی, arena);
    اگر (p == تہی) {
        جاؤ error;
    }

    نتیجہ = Py_Pegen_تسلسل_parser(p);
    _PyPegen_Parser_Free(p);

    ثابت حرف *ماخذ = Py_Tokenizer_Retained_ماخذ(tok);
    اگر (ماخذ != تہی && نتیجہ && interactive_src != تہی) {
        *interactive_src = PyUnicode_FromString(ماخذ);
        اگر (!*interactive_src || Py_Arena_جوڑو_Py_Object(arena, *interactive_src) < 0) {
            Py_XDECREF(*interactive_src);
            نتیجہ = تہی;
            جاؤ error;
        }
    }

error:
    _PyTokenizer_Free(tok);
    واپس نتیجہ;
}

mod_ty
Py_Pegen_تسلسل_parser_from_string(ثابت حرف *str, صحیح آغاز_قاعدہ, PyObject *filename_ob,
                       PyCompilerFlags *flags, PyArena *arena, PyObject *module,
                       PyObject **incomplete)
{
    _PyTokenizer_Newline newline_منوال = آغاز_قاعدہ == Py_file_داخلہ
        ? _PyTokenizer_IMPLY_NEWLINE : _PyTokenizer_KEEP_NEWLINE;
    صحیح parser_flags = compute_parser_flags(flags);
    اگر (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_داخلہ) {
        newline_منوال = _PyTokenizer_APPEND_NEWLINE;
    }

    ساخت tok_state *tok;
    اگر (flags != تہی && flags->cf_flags & PyCF_IGNORE_COOKIE) {
        tok = _PyTokenizer_FromUTF8(str, newline_منوال, 0);
    } ورنہ {
        tok = _PyTokenizer_FromString(str, newline_منوال, 0);
    }
    اگر (tok == تہی) {
        اگر (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        ورنہ {
            // The only silent tokenizer init failure is a failed allocation.
            Py_Err_No_حافظہ();
        }
        واپس تہی;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    Py_Tokenizer_سیٹ_Context(tok, filename_ob, module);

    // We need to clear up from here on
    mod_ty نتیجہ = تہی;

    صحیح feature_نسخہ = flags && (flags->cf_flags & PyCF_ONLY_AST) ?
        flags->cf_feature_نسخہ : PY_MINOR_نسخہ;
    Parser *p = _PyPegen_Parser_New(tok, آغاز_قاعدہ, parser_flags, feature_نسخہ,
                                    تہی, str, arena);
    اگر (p == تہی) {
        جاؤ error;
    }

    نتیجہ = Py_Pegen_تسلسل_parser(p);
    اگر (نتیجہ != تہی && (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_داخلہ)) {
        صحیح needs_newline = _PyTokenizer_GetNewlineRequirements(tok);
        اگر ((needs_newline & _PyTokenizer_NEWLINE_CONTINUATION) ||
            (آغاز_قاعدہ != Py_file_داخلہ &&
             (needs_newline & _PyTokenizer_NEWLINE_INDENT)) ||
            (آغاز_قاعدہ == Py_single_داخلہ && p->داخلہ_exhausted &&
             (needs_newline & _PyTokenizer_NEWLINE_STATEMENT))) {
            Py_Pegen_raise_incomplete_داخلہ(p);
            اگر (incomplete != تہی && PyErr_ExceptionMatches(Py_Exc_Incomplete_داخلہ_Error)) {
                *incomplete = PyErr_GetRaisedException();
            }
            ورنہ {
                نتیجہ = تہی;
            }
        }
    }
    _PyPegen_Parser_Free(p);

error:
    _PyTokenizer_Free(tok);
    واپس نتیجہ;
}
