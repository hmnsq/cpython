#ضمن <Python.h>
#ضمن "pycore_ast.h"           // _PyAST_Validate(),
#ضمن "pycore_pystate.h"       // _PyThreadState_GET()
#ضمن "pycore_parser.h"        // _PYPEGEN_NSTATISTICS
#ضمن "pycore_pyerrors.h"      // PyExc_IncompleteInputError
#ضمن "pycore_runtime.h"       // _PyRuntime
#ضمن "pycore_unicodeobject.h" // _PyUnicode_InternImmortal
#ضمن <errcode.h>

#ضمن "lexer/state.h"
#ضمن "tokenizer/tokenizer.h"
#ضمن "pegen.h"

#وصف_ڪريو IDENTIFIER_CACHE_سائيز 2048  // Must be a power of two.
#وصف_ڪريو IDENTIFIER_CACHE_MAX_PROBES 8
#وصف_ڪريو PEGEN_ARRAY_GROWTH_ضرب_ڪندڙ 2
#وصف_ڪريو TYPE_IGNORE_INITIAL_صلاحيت 10

بنية identifier_cache_داخلا {
    ثابت حرف *ڪنجي;   // Borrowed from arena-owned token bytes.
    Py_ssize_t len;
    Py_hash_t hash;
    PyObject *قدر;   // Borrowed from an arena-owned identifier.
};

// Internal parser functions

asdl_stmt_seq*
_PyPegen_interactive_exit(Parser *p)
{
    إذا (p->errcode) {
        *(p->errcode) = E_EOF;
    }
    أرجع عدم;
}

Py_ssize_t
Py_Pegen_byte_وٿي_منزل_character_وٿي_سٽ(PyObject *سٽ, Py_ssize_t کانو_وٿي, Py_ssize_t پڇاڙي_کانو_وٿي)
{
    ثابت بنانشان حرف *معلومات = (ثابت بنانشان حرف*)PyUnicode_AsUTF8(سٽ);
    إذا (معلومات == عدم) {
        أرجع -1;
    }

    Py_ssize_t len = 0;
    طالما (کانو_وٿي < پڇاڙي_کانو_وٿي) {
        Py_UCS4 ch = معلومات[کانو_وٿي];
        إذا (ch < 0x80) {
            کانو_وٿي += 1;
        } وإلا إذا ((ch & 0xe0) == 0xc0) {
            کانو_وٿي += 2;
        } وإلا إذا ((ch & 0xf0) == 0xe0) {
            کانو_وٿي += 3;
        } وإلا إذا ((ch & 0xf8) == 0xf0) {
            کانو_وٿي += 4;
        } وإلا {
            Py_Err_سيٽ_String(Py_Exc_قدر_Error, "Invalid UTF-8 sequence");
            أرجع -1;
        }
        len++;
    }
    أرجع len;
}

Py_ssize_t
Py_Pegen_byte_وٿي_منزل_character_وٿي_raw(ثابت حرف* str, Py_ssize_t کانو_وٿي)
{
    Py_ssize_t len = (Py_ssize_t)لڙي_ڊيگهه(str);
    إذا (کانو_وٿي > len + 1) {
        کانو_وٿي = len + 1;
    }
    أكد(کانو_وٿي >= 0);
    PyObject *متن = PyUnicode_DecodeUTF8(str, کانو_وٿي, "replace");
    إذا (!متن) {
        أرجع -1;
    }
    Py_ssize_t سائيز = Py_Unicode_GET_ڊيگهائي(متن);
    Py_DECREF(متن);
    أرجع سائيز;
}

Py_ssize_t
Py_Pegen_byte_وٿي_منزل_character_وٿي(PyObject *سٽ, Py_ssize_t کانو_وٿي)
{
    ثابت حرف *str = PyUnicode_AsUTF8(سٽ);
    إذا (!str) {
        أرجع -1;
    }
    أرجع Py_Pegen_byte_وٿي_منزل_character_وٿي_raw(str, کانو_وٿي);
}

// Here, mark is the start of the node, while p->mark is the end.
// If node==NULL, they should be the same.
صحيح
Py_Pegen_وجهو_ياداشت(Parser *p, صحيح mark, صحيح type, فراغ *ڳنڍ)
{
    // Insert in front
    ياداشت *m = _PyArena_Malloc(p->arena, حجم(ياداشت));
    إذا (m == عدم) {
        أرجع -1;
    }
    m->type = type;
    m->ڳنڍ = ڳنڍ;
    m->mark = p->mark;
    m->ايندڙ = p->ٽوڪن[mark]->ياداشت_3;
    p->ٽوڪن[mark]->ياداشت_3 = m;
    p->ٽوڪن[mark]->ياداشت_نقاب |= 1ULL << (type & 63);
    أرجع 0;
}

// Like _PyPegen_insert_memo(), but returns the inserted Memo so callers
// can update it in place without re-walking the token's memo list.
ياداشت *
Py_Pegen_وجهو_ياداشت_سڌو(Parser *p, صحيح mark, صحيح type)
{
    إذا (Py_Pegen_وجهو_ياداشت(p, mark, type, عدم) < 0) {
        أرجع عدم;
    }
    أرجع p->ٽوڪن[mark]->ياداشت_3;
}

// Like _PyPegen_insert_memo(), but updates an existing node if found.
صحيح
Py_Pegen_update_ياداشت(Parser *p, صحيح mark, صحيح type, فراغ *ڳنڍ)
{
    لكل (ياداشت *m = p->ٽوڪن[mark]->ياداشت_3; m != عدم; m = m->ايندڙ) {
        إذا (m->type == type) {
            // Update existing node.
            m->ڳنڍ = ڳنڍ;
            m->mark = p->mark;
            أرجع 0;
        }
    }
    // Insert new node.
    أرجع Py_Pegen_وجهو_ياداشت(p, mark, type, ڳنڍ);
}

ساكن صحيح
init_normalization(Parser *p)
{
    إذا (p->normalize) {
        أرجع 1;
    }
    p->normalize = PyImport_ImportModuleAttrString("unicodedata", "normalize");
    إذا (!p->normalize)
    {
        أرجع 0;
    }
    أرجع 1;
}

صحيح
Py_Pegen_grow_loop_بفر(فراغ ***بفر, Py_ssize_t *صلاحيت)
{
    إذا ((سائيز_t)*صلاحيت > (سائيز_t)PY_SSIZE_T_MAX /
            (PEGEN_ARRAY_GROWTH_ضرب_ڪندڙ * حجم(**بفر))) {
        أرجع -1;
    }
    Py_ssize_t new_صلاحيت = *صلاحيت == 0
        ? 1 : *صلاحيت * PEGEN_ARRAY_GROWTH_ضرب_ڪندڙ;
    فراغ **new_بفر = PyMem_Realloc(*بفر, new_صلاحيت * حجم(**بفر));
    إذا (new_بفر == عدم) {
        أرجع -1;
    }
    *بفر = new_بفر;
    *صلاحيت = new_صلاحيت;
    أرجع 0;
}

ساكن صحيح
growable_comment_array_جوڙيو(growable_comment_array *arr, صحيح lineno, حرف *comment)
{
    إذا (arr->انگ_شيون >= arr->سائيز) {
        إذا (arr->سائيز > (سائيز_t)PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_ضرب_ڪندڙ * حجم(*arr->شيون))) {
            أرجع 0;
        }
        سائيز_t new_سائيز = arr->سائيز == 0
            ? TYPE_IGNORE_INITIAL_صلاحيت
            : arr->سائيز * PEGEN_ARRAY_GROWTH_ضرب_ڪندڙ;
        فراغ *new_شيون_array = PyMem_Realloc(
            arr->شيون, new_سائيز * حجم(*arr->شيون));
        إذا (!new_شيون_array) {
            أرجع 0;
        }
        arr->شيون = new_شيون_array;
        arr->سائيز = new_سائيز;
    }

    arr->شيون[arr->انگ_شيون].lineno = lineno;
    arr->شيون[arr->انگ_شيون].comment = comment;  // Take ownership
    arr->انگ_شيون++;
    أرجع 1;
}

ساكن فراغ
growable_comment_array_deallocate(growable_comment_array *arr)
{
    لكل (سائيز_t i = 0; i < arr->انگ_شيون; i++) {
        PyMem_Free(arr->شيون[i].comment);
    }
    PyMem_Free(arr->شيون);
}

ساكن صحيح
_get_keyword_or_name_type(Parser *p, ثابت حرف *متن, Py_ssize_t ڊيگهائي)
{
    أكد(ڊيگهائي > 0);

    إذا (ڊيگهائي >= p->n_keyword_lists ||
        p->keywords[ڊيگهائي] == عدم ||
        p->keywords[ڊيگهائي]->type == -1) {
        أرجع NAME;
    }
    لكل (KeywordToken *k = p->keywords[ڊيگهائي]; k != عدم && k->type != -1; k++) {
        إذا (memcmp(k->str, متن, (سائيز_t)ڊيگهائي) == 0) {
            أرجع k->type;
        }
    }
    أرجع NAME;
}

// Keep text only where grammar actions or helpers read it. Keyword error
// actions use fixed spellings, so keywords do not need their text.
ساكن مضمن صحيح
token_needs_متن(صحيح type)
{
    بدل (type) {
        حالة NAME:
        حالة عدد:
        حالة STRING:
        حالة FSTRING_شروعات:
        حالة FSTRING_وچ:
        حالة FSTRING_پڇاڙي:
        حالة TSTRING_شروعات:
        حالة TSTRING_وچ:
        حالة TSTRING_پڇاڙي:
        حالة TYPE_COMMENT:
        حالة NOTEQUAL:  // _PyPegen_check_barry_as_flufl() distinguishes != and <>.
            أرجع 1;
        افتراضي:
            أرجع 0;
    }
}

ساكن صحيح
initialize_token(Parser *p, Token *parser_token, بنية token *new_token, صحيح token_type) {
    أكد(parser_token != عدم);

    parser_token->type = token_type;
    parser_token->بائيٽون = عدم;
    إذا (token_needs_متن(token_type) && new_token->وسعت.شروعات >= 0) {
        Py_ssize_t ڊيگهائي;
        ثابت حرف *متن = Py_Token_متن_View(p->tok, new_token, &ڊيگهائي);
        إذا (token_type == NAME) {
            parser_token->type = _get_keyword_or_name_type(p, متن, ڊيگهائي);
        }
        // A name reclassified as a keyword does not need its text.
        إذا (parser_token->type == token_type) {
            parser_token->بائيٽون = Py_بائيٽون_From_String_And_سائيز(متن, ڊيگهائي);
            إذا (parser_token->بائيٽون == عدم) {
                أرجع -1;
            }
            إذا (Py_Arena_جوڙيو_Py_Object(p->arena, parser_token->بائيٽون) < 0) {
                Py_DECREF(parser_token->بائيٽون);
                أرجع -1;
            }
        }
    }

    parser_token->metadata = عدم;
    إذا (new_token->metadata != عدم) {
        إذا (Py_Arena_جوڙيو_Py_Object(p->arena, new_token->metadata) < 0) {
            أرجع -1;
        }
        parser_token->metadata = new_token->metadata;
        new_token->metadata = عدم;
    }

    parser_token->level = new_token->level;
    parser_token->is_raw = new_token->is_raw;
    parser_token->lineno = new_token->شروعات_loc.lineno;
    parser_token->کانو_وٿي = new_token->پڇاڙي_loc.lineno == p->starting_lineno
        ? p->starting_کانو_وٿي + new_token->شروعات_loc.byte_کانو
        : new_token->شروعات_loc.byte_کانو;
    parser_token->پڇاڙي_lineno = new_token->پڇاڙي_loc.lineno;
    parser_token->پڇاڙي_کانو_وٿي = new_token->پڇاڙي_loc.lineno == p->starting_lineno
        ? p->starting_کانو_وٿي + new_token->پڇاڙي_loc.byte_کانو
        : new_token->پڇاڙي_loc.byte_کانو;

    p->fill += 1;

    إذا (token_type == ERRORTOKEN && Py_Tokenizer_Get_حالت(p->tok) == E_DECODE) {
        أرجع _Pypegen_raise_decode_error(p);
    }

    أرجع (token_type == ERRORTOKEN ? _Pypegen_tokenizer_error(p) : 0);
}

ساكن صحيح
resize_ٽوڪن_array(Parser *p) {
    إذا (p->سائيز > وڌ_کان_وڌ_عدد / PEGEN_ARRAY_GROWTH_ضرب_ڪندڙ ||
            (سائيز_t)p->سائيز > PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_ضرب_ڪندڙ * حجم(*p->ٽوڪن))) {
        Py_Err_No_يادگيري();
        أرجع -1;
    }
    صحيح newsize = p->سائيز * PEGEN_ARRAY_GROWTH_ضرب_ڪندڙ;
    Token **new_ٽوڪن = PyMem_Realloc(p->ٽوڪن, (سائيز_t)newsize * حجم(Token *));
    إذا (new_ٽوڪن == عدم) {
        Py_Err_No_يادگيري();
        أرجع -1;
    }
    p->ٽوڪن = new_ٽوڪن;

    // Allocate new tokens together without moving existing ones: parser rules
    // may still hold pointers into earlier blocks. Parser_Free frees each block.
    Token *chunk = PyMem_Calloc((سائيز_t)(newsize - p->سائيز), حجم(Token));
    إذا (chunk == عدم) {
        Py_Err_No_يادگيري();
        أرجع -1;
    }
    لكل (صحيح i = p->سائيز; i < newsize; i++) {
        p->ٽوڪن[i] = &chunk[i - p->سائيز];
    }
    p->سائيز = newsize;
    أرجع 0;
}

صحيح
_PyPegen_fill_token(Parser *p)
{
    بنية token new_token;
    _PyToken_Init(&new_token);
    _PyTokenizer_Get(p->tok, &new_token);

    // Record and skip '# type: ignore' comments
    طالما (new_token.type == TYPE_IGNORE) {
        Py_ssize_t len;
        ثابت حرف *متن = Py_Token_متن_View(p->tok, &new_token, &len);
        حرف *ٽيگ = PyMem_Malloc((سائيز_t)len + 1);
        إذا (ٽيگ == عدم) {
            Py_Err_No_يادگيري();
            اذهب error;
        }
        يادگيري_نقل(ٽيگ, متن, (سائيز_t)len);
        ٽيگ[len] = '\0';
        // Ownership of tag passes to the growable array
        إذا (!growable_comment_array_جوڙيو(&p->type_ignore_comments, new_token.پڇاڙي_loc.lineno, ٽيگ)) {
            PyMem_Free(ٽيگ);
            Py_Err_No_يادگيري();
            اذهب error;
        }
        _PyTokenizer_Get(p->tok, &new_token);
    }

    صحيح type = new_token.type;
    // EOF can finish an ordinary token. Only a boundary token means that
    // the grammar requested input beyond the last physical line.
    إذا (type == ENDMARKER ||
        (type == DEDENT && (p->flags & Py_PARSE_ALLOW_INCOMPLETE_داخلا) &&
         Py_Tokenizer_Get_حالت(p->tok) == E_EOF)) {
        p->داخلا_exhausted = 1;
    }

    // If we have reached the end and we are in single input mode we need to insert a newline and reset the parsing
    إذا (p->شروعات_قاعدو == Py_single_داخلا && type == ENDMARKER && p->parsing_started) {
        type = NEWLINE; /* Add an extra newline */
        p->parsing_started = 0;

        إذا (!(p->flags & PyPARSE_DONT_IMPLY_DEDENT)) {
            _PyTokenizer_ImplyDedents(p->tok);
        }
    }
    وإلا {
        p->parsing_started = 1;
    }

    // Check if we are at the limit of the token array capacity and resize if needed
    إذا ((p->fill == p->سائيز) && (resize_ٽوڪن_array(p) != 0)) {
        اذهب error;
    }

    Token *t = p->ٽوڪن[p->fill];
    صحيح نتيجو = initialize_token(p, t, &new_token, type);
    _PyToken_Free(&new_token);
    أرجع نتيجو;
error:
    _PyToken_Free(&new_token);
    أرجع -1;
}

#إذا defined(Py_DEBUG)
// Instrumentation to count the effectiveness of memoization.
// The array counts the number of tokens skipped by memoization,
// indexed by type.

#وصف_ڪريو NSTATISTICS _PYPEGEN_NSTATISTICS
#وصف_ڪريو ياداشت_statistics _PyRuntime.parser.ياداشت_statistics

فراغ
Py_Pegen_clear_ياداشت_statistics(فراغ)
{
    Py_Mutex_تالو(&_PyRuntime.parser.mutex);
    لكل (صحيح i = 0; i < NSTATISTICS; i++) {
        ياداشت_statistics[i] = 0;
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
}

PyObject *
Py_Pegen_get_ياداشت_statistics(فراغ)
{
    PyObject *ret = PyList_New(NSTATISTICS);
    إذا (ret == عدم) {
        أرجع عدم;
    }

    Py_Mutex_تالو(&_PyRuntime.parser.mutex);
    لكل (صحيح i = 0; i < NSTATISTICS; i++) {
        PyObject *قدر = PyLong_FromLong(ياداشت_statistics[i]);
        إذا (قدر == عدم) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            أرجع عدم;
        }
        // PyList_SetItem borrows a reference to value.
        إذا (Py_List_سيٽ_شي(ret, i, قدر) < 0) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            أرجع عدم;
        }
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
    أرجع ret;
}
#جيڪڏهن_پورو

صحيح  // bool
_PyPegen_is_memoized(Parser *p, صحيح type, فراغ *pres)
{
    إذا (p->mark == p->fill) {
        إذا (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            أرجع -1;
        }
    }

    Token *t = p->ٽوڪن[p->mark];

    إذا (!(t->ياداشت_نقاب & (1ULL << (type & 63)))) {
        أرجع 0;
    }

    لكل (ياداشت *m = t->ياداشت_3; m != عدم; m = m->ايندڙ) {
        إذا (m->type == type) {
#إذا defined(Py_DEBUG)
            إذا (0 <= type && type < NSTATISTICS) {
                ڊگهو ڳڻپ = m->mark - p->mark;
                // A memoized negative result counts for one.
                إذا (ڳڻپ <= 0) {
                    ڳڻپ = 1;
                }
                Py_Mutex_تالو(&_PyRuntime.parser.mutex);
                ياداشت_statistics[type] += ڳڻپ;
                PyMutex_Unlock(&_PyRuntime.parser.mutex);
            }
#جيڪڏهن_پورو
            p->mark = m->mark;
            *(فراغ **)(pres) = m->ڳنڍ;
            أرجع 1;
        }
    }
    أرجع 0;
}

#وصف_ڪريو LOOKAHEAD1(NAME, RES_TYPE)                                  \
    صحيح                                                             \
    NAME (صحيح مثبت, RES_TYPE (func)(Parser *), Parser *p)       \
    {                                                               \
        صحيح mark = p->mark;                                         \
        فراغ *res = func(p);                                        \
        p->mark = mark;                                             \
        أرجع (res != عدم) == مثبت;                           \
    }

LOOKAHEAD1(_PyPegen_lookahead, فراغ *)
LOOKAHEAD1(Py_Pegen_lookahead_for_اظهار, اظهار_ty)
LOOKAHEAD1(_PyPegen_lookahead_for_stmt, stmt_ty)
#وصف_رد LOOKAHEAD1

#وصف_ڪريو LOOKAHEAD2(NAME, RES_TYPE, T)                                   \
    صحيح                                                                 \
    NAME (صحيح مثبت, RES_TYPE (func)(Parser *, T), Parser *p, T arg) \
    {                                                                   \
        صحيح mark = p->mark;                                             \
        فراغ *res = func(p, arg);                                       \
        p->mark = mark;                                                 \
        أرجع (res != عدم) == مثبت;                               \
    }

LOOKAHEAD2(_PyPegen_lookahead_with_int, Token *, صحيح)
LOOKAHEAD2(_PyPegen_lookahead_with_string, اظهار_ty, ثابت حرف *)
#وصف_رد LOOKAHEAD2

Token *
_PyPegen_expect_token(Parser *p, صحيح type)
{
    إذا (p->mark == p->fill) {
        إذا (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            أرجع عدم;
        }
    }
    Token *t = p->ٽوڪن[p->mark];
    إذا (t->type != type) {
       أرجع عدم;
    }
    p->mark += 1;
    أرجع t;
}

فراغ*
Py_Pegen_expect_forced_نتيجو(Parser *p, فراغ* نتيجو, ثابت حرف* expected) {

    إذا (p->error_indicator == 1) {
        أرجع عدم;
    }
    إذا (نتيجو == عدم) {
        RAISE_SYNTAX_ERROR("expected (%s)", expected);
        أرجع عدم;
    }
    أرجع نتيجو;
}

Token *
_PyPegen_expect_forced_token(Parser *p, صحيح type, ثابت حرف* expected) {

    إذا (p->error_indicator == 1) {
        أرجع عدم;
    }

    إذا (p->mark == p->fill) {
        إذا (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            أرجع عدم;
        }
    }
    Token *t = p->ٽوڪن[p->mark];
    إذا (t->type != type) {
        RAISE_SYNTAX_ERROR_معلوم_LOCATION(t, "expected '%s'", expected);
        أرجع عدم;
    }
    p->mark += 1;
    أرجع t;
}

اظهار_ty
_PyPegen_expect_soft_keyword(Parser *p, ثابت حرف *keyword)
{
    إذا (p->mark == p->fill) {
        إذا (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            أرجع عدم;
        }
    }
    Token *t = p->ٽوڪن[p->mark];
    إذا (t->type != NAME) {
        أرجع عدم;
    }
    ثابت حرف *s = Py_بائيٽون_As_String(t->بائيٽون);
    إذا (!s) {
        p->error_indicator = 1;
        أرجع عدم;
    }
    إذا (strcmp(s, keyword) != 0) {
        أرجع عدم;
    }
    أرجع _PyPegen_name_token(p);
}

Token *
Py_Pegen_get_آخري_nonnwhitespace_token(Parser *p)
{
    أكد(p->mark >= 0);
    Token *token = عدم;
    لكل (صحيح m = p->mark - 1; m >= 0; m--) {
        token = p->ٽوڪن[m];
        إذا (token->type != ENDMARKER && (token->type < NEWLINE || token->type > DEDENT)) {
            اكسر;
        }
    }
    أرجع token;
}

PyObject *
_PyPegen_new_identifier(Parser *p, ثابت حرف *n)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, (Py_ssize_t)لڙي_ڊيگهه(n), عدم);
    إذا (!id) {
        اذهب error;
    }
    /* Check whether there are non-ASCII characters in the
       identifier; if so, normalize to NFKC. */
    إذا (!PyUnicode_IS_ASCII(id))
    {
        إذا (!init_normalization(p))
        {
            Py_DECREF(id);
            اذهب error;
        }
        PyObject *form = PyUnicode_InternFromString("NFKC");
        إذا (form == عدم)
        {
            Py_DECREF(id);
            اذهب error;
        }
        PyObject *دليلون[2] = {form, id};
        PyObject *id2 = PyObject_Vectorcall(p->normalize, دليلون, 2, عدم);
        Py_DECREF(id);
        Py_DECREF(form);
        إذا (!id2) {
            اذهب error;
        }

        إذا (!PyUnicode_Check(id2))
        {
            PyErr_Format(PyExc_TypeError,
                         "unicodedata.normalize() must return a string, not "
                         "%.200s",
                         _PyType_Name(Py_TYPE(id2)));
            Py_DECREF(id2);
            اذهب error;
        }
        id = id2;
    }
    ساكن ثابت حرف * ثابت forbidden[] = {
        "None",
        "True",
        "False",
        عدم
    };
    لكل (صحيح i = 0; forbidden[i] != عدم; i++) {
        إذا (Py_Unicode_Equal_منزل_ASCIIString(id, forbidden[i])) {
            PyErr_Format(Py_Exc_قدر_Error,
                         "identifier field can't represent '%s' constant",
                         forbidden[i]);
            Py_DECREF(id);
            اذهب error;
        }
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &id);
    إذا (Py_Arena_جوڙيو_Py_Object(p->arena, id) < 0)
    {
        Py_DECREF(id);
        اذهب error;
    }
    أرجع id;

error:
    p->error_indicator = 1;
    أرجع عدم;
}

// Return an arena-owned identifier; callers borrow the reference.
ساكن PyObject *
get_cached_identifier(Parser *p, PyObject *بائيٽون)
{
    ثابت حرف *s = Py_بائيٽون_As_String(بائيٽون);
    إذا (!s) {
        p->error_indicator = 1;
        أرجع عدم;
    }
    // Identifiers repeat constantly; a small span-keyed cache skips the
    // UTF-8 decode + intern for repeated occurrences. Keys point into
    // arena-owned token bytes and values are arena-owned interned strings,
    // so borrowed references are valid for the lifetime of the parse
    // (including the second error pass, which reuses parser and arena).
    Py_ssize_t len = Py_بائيٽون_GET_سائيز(بائيٽون);
    Py_hash_t hash = PyObject_Hash(بائيٽون);
    إذا (hash == -1) {
        p->error_indicator = 1;
        أرجع عدم;
    }
    // Parses without identifiers do not need this cache.
    إذا (p->identifier_cache == عدم) {
        p->identifier_cache = PyMem_Calloc(
            IDENTIFIER_CACHE_سائيز, حجم(*p->identifier_cache));
        إذا (p->identifier_cache == عدم) {
            p->error_indicator = 1;
            أرجع Py_Err_No_يادگيري();
        }
    }
    Identifier_Cache_داخلا *free_جڳهه = عدم;
    سائيز_t idx = (سائيز_t)hash & (IDENTIFIER_CACHE_سائيز - 1);
    لكل (صحيح probe = 0; probe < IDENTIFIER_CACHE_MAX_PROBES; probe++) {
        Identifier_Cache_داخلا *داخلا = &p->identifier_cache[
            (idx + probe) & (IDENTIFIER_CACHE_سائيز - 1)];
        إذا (داخلا->ڪنجي == عدم) {
            free_جڳهه = داخلا;
            اكسر;
        }
        إذا (داخلا->hash == hash && داخلا->len == len &&
            memcmp(داخلا->ڪنجي, s, len) == 0)
        {
            أرجع داخلا->قدر;
        }
    }
    PyObject *id = _PyPegen_new_identifier(p, s);
    إذا (id == عدم) {
        p->error_indicator = 1;
        أرجع عدم;
    }
    إذا (free_جڳهه != عدم) {
        free_جڳهه->ڪنجي = s;
        free_جڳهه->len = len;
        free_جڳهه->hash = hash;
        free_جڳهه->قدر = id;
    }
    أرجع id;
}

ساكن اظهار_ty
_PyPegen_name_from_token(Parser *p, Token* t)
{
    إذا (t == عدم) {
        أرجع عدم;
    }
    // Reuse the AST node when backtracking revisits this token. Memo lookup
    // starts before the token and restores the position after it on a hit.
    // Token kinds can be memo keys: generated grammar rule IDs start at 1000.
    صحيح mark = p->mark - 1;
    p->mark = mark;
    اظهار_ty cached = عدم;
    إذا (_PyPegen_is_memoized(p, NAME, &cached)) {
        أرجع cached;
    }
    p->mark = mark + 1;
    PyObject *id = get_cached_identifier(p, t->بائيٽون);
    إذا (id == عدم) {
        أرجع عدم;
    }
    اظهار_ty نتيجو = _PyAST_Name(id, ڀريو, t->lineno, t->کانو_وٿي,
                                t->پڇاڙي_lineno, t->پڇاڙي_کانو_وٿي, p->arena);
    إذا (نتيجو != عدم && Py_Pegen_وجهو_ياداشت(p, mark, NAME, نتيجو) < 0) {
        p->error_indicator = 1;
        أرجع عدم;
    }
    أرجع نتيجو;
}

اظهار_ty
_PyPegen_name_token(Parser *p)
{
    Token *t = _PyPegen_expect_token(p, NAME);
    أرجع _PyPegen_name_from_token(p, t);
}

فراغ *
_PyPegen_string_token(Parser *p)
{
    أرجع _PyPegen_expect_token(p, STRING);
}

اظهار_ty _PyPegen_soft_keyword_token(Parser *p) {
    Token *t = _PyPegen_expect_token(p, NAME);
    إذا (t == عدم) {
        أرجع عدم;
    }
    حرف *the_token;
    Py_ssize_t سائيز;
    Py_بائيٽون_As_String_And_سائيز(t->بائيٽون, &the_token, &سائيز);
    لكل (حرف **keyword = p->soft_keywords; *keyword != عدم; keyword++) {
        إذا (لڙي_ڊيگهه(*keyword) == (سائيز_t)سائيز &&
            لڙي_ڀيٽ(*keyword, the_token, (سائيز_t)سائيز) == 0) {
            أرجع _PyPegen_name_from_token(p, t);
        }
    }
    أرجع عدم;
}

ساكن PyObject *
parsenumber_raw(ثابت حرف *s)
{
    ثابت حرف *پڇاڙي;
    ڊگهو x;
    مزدوج dx;
    Py_complex compl;
    صحيح imflag;

    أكد(s != عدم);
    errno = 0;
    پڇاڙي = s + لڙي_ڊيگهه(s) - 1;
    imflag = *پڇاڙي == 'j' || *پڇاڙي == 'J';
    إذا (s[0] == '0') {
        x = (ڊگهو)PyOS_strtoul(s, (حرف **)&پڇاڙي, 0);
        إذا (x < 0 && errno == 0) {
            أرجع PyLong_FromString(s, (حرف **)0, 0);
        }
    }
    وإلا {
        x = PyOS_strtol(s, (حرف **)&پڇاڙي, 0);
    }
    إذا (*پڇاڙي == '\0') {
        إذا (errno != 0) {
            أرجع PyLong_FromString(s, (حرف **)0, 0);
        }
        أرجع PyLong_FromLong(x);
    }
    /* XXX Huge floats may silently fail */
    إذا (imflag) {
        compl.real = 0.;
        compl.imag = Py_OS_string_منزل_double(s, (حرف **)&پڇاڙي, عدم);
        إذا (compl.imag == -1.0 && PyErr_Occurred()) {
            أرجع عدم;
        }
        أرجع PyComplex_FromCComplex(compl);
    }
    dx = Py_OS_string_منزل_double(s, عدم, عدم);
    إذا (dx == -1.0 && PyErr_Occurred()) {
        أرجع عدم;
    }
    أرجع PyFloat_FromDouble(dx);
}

ساكن PyObject *
parsenumber(ثابت حرف *s)
{
    حرف *dup;
    حرف *پڇاڙي;
    PyObject *res = عدم;

    أكد(s != عدم);

    إذا (strchr(s, '_') == عدم) {
        أرجع parsenumber_raw(s);
    }
    /* Create a duplicate without underscores. */
    dup = PyMem_Malloc(لڙي_ڊيگهه(s) + 1);
    إذا (dup == عدم) {
        أرجع Py_Err_No_يادگيري();
    }
    پڇاڙي = dup;
    لكل (; *s; s++) {
        إذا (*s != '_') {
            *پڇاڙي++ = *s;
        }
    }
    *پڇاڙي = '\0';
    res = parsenumber_raw(dup);
    PyMem_Free(dup);
    أرجع res;
}

اظهار_ty
Py_Pegen_عدد_token(Parser *p)
{
    صحيح mark = p->mark;
    Token *t = _PyPegen_expect_token(p, عدد);
    إذا (t == عدم) {
        أرجع عدم;
    }

    // Look up the node at the token's start, as for NAME tokens. A memo hit
    // advances the mark; on a miss, restore the position after the token.
    p->mark = mark;
    اظهار_ty cached = عدم;
    إذا (_PyPegen_is_memoized(p, عدد, &cached)) {
        أرجع cached;
    }
    p->mark = mark + 1;

    ثابت حرف *انگ_raw = Py_بائيٽون_As_String(t->بائيٽون);
    إذا (انگ_raw == عدم) {
        p->error_indicator = 1;
        أرجع عدم;
    }

    إذا (p->feature_نسخو < 6 && strchr(انگ_raw, '_') != عدم) {
        p->error_indicator = 1;
        أرجع RAISE_SYNTAX_ERROR("Underscores in numeric literals are only supported "
                                  "in Python 3.6 and greater");
    }

    PyObject *c = parsenumber(انگ_raw);

    إذا (c == عدم) {
        p->error_indicator = 1;
        PyThreadState *tstate = _PyThreadState_GET();
        // The only way a ValueError should happen in _this_ code is via
        // PyLong_FromString hitting a length limit.
        إذا (tstate->current_exception != عدم &&
            Py_TYPE(tstate->current_exception) == (PyTypeObject *)Py_Exc_قدر_Error
        ) {
            PyObject *exc = PyErr_GetRaisedException();
            /* Intentionally omitting columns to avoid a wall of 1000s of '^'s
             * on the error message. Nobody is going to overlook their huge
             * numeric literal once given the line. */
            RAISE_ERROR_معلوم_LOCATION(
                p, PyExc_SyntaxError,
                t->lineno, -1 /* col_offset */,
                t->پڇاڙي_lineno, -1 /* end_col_offset */,
                "%S - Consider hexadecimal for huge integer literals "
                "to avoid decimal conversion limits.",
                exc);
            Py_DECREF(exc);
        }
        أرجع عدم;
    }

    إذا (Py_Arena_جوڙيو_Py_Object(p->arena, c) < 0) {
        Py_DECREF(c);
        p->error_indicator = 1;
        أرجع عدم;
    }

    اظهار_ty نتيجو = _PyAST_Constant(c, عدم, t->lineno, t->کانو_وٿي,
                                    t->پڇاڙي_lineno, t->پڇاڙي_کانو_وٿي, p->arena);
    إذا (نتيجو != عدم && Py_Pegen_وجهو_ياداشت(p, mark, عدد, نتيجو) < 0) {
        p->error_indicator = 1;
        أرجع عدم;
    }
    أرجع نتيجو;
}



ساكن صحيح
compute_parser_flags(PyCompilerFlags *flags)
{
    صحيح parser_flags = 0;
    إذا (!flags) {
        أرجع 0;
    }
    إذا (flags->cf_flags & PyCF_DONT_IMPLY_DEDENT) {
        parser_flags |= PyPARSE_DONT_IMPLY_DEDENT;
    }
    إذا (flags->cf_flags & PyCF_IGNORE_COOKIE) {
        parser_flags |= PyPARSE_IGNORE_COOKIE;
    }
    إذا (flags->cf_flags & CO_FUTURE_BARRY_AS_BDFL) {
        parser_flags |= PyPARSE_BARRY_AS_BDFL;
    }
    إذا (flags->cf_flags & PyCF_TYPE_COMMENTS) {
        parser_flags |= PyPARSE_TYPE_COMMENTS;
    }
    إذا (flags->cf_flags & Py_CF_ALLOW_INCOMPLETE_داخلا) {
        parser_flags |= Py_PARSE_ALLOW_INCOMPLETE_داخلا;
    }
    أرجع parser_flags;
}

// Parser API

Parser *
_PyPegen_Parser_New(بنية tok_state *tok, صحيح شروعات_قاعدو, صحيح flags,
                    صحيح feature_نسخو, صحيح *errcode, ثابت حرف* ذريعو, PyArena *arena)
{
    Parser *p = PyMem_Malloc(حجم(Parser));
    إذا (p == عدم) {
        أرجع (Parser *) Py_Err_No_يادگيري();
    }
    أكد(tok != عدم);
    Py_Tokenizer_سيٽ_Options(tok, 0, (flags & PyPARSE_TYPE_COMMENTS) > 0);
    Py_Tokenizer_Allow_Incomplete_داخلا(tok, flags & Py_PARSE_ALLOW_INCOMPLETE_داخلا);
    p->tok = tok;
    p->keywords = عدم;
    p->n_keyword_lists = -1;
    p->soft_keywords = عدم;
    p->ٽوڪن = PyMem_Malloc(حجم(Token *));
    إذا (!p->ٽوڪن) {
        PyMem_Free(p);
        أرجع (Parser *) Py_Err_No_يادگيري();
    }
    p->ٽوڪن[0] = PyMem_Calloc(1, حجم(Token));
    إذا (!p->ٽوڪن[0]) {
        PyMem_Free(p->ٽوڪن);
        PyMem_Free(p);
        أرجع (Parser *) Py_Err_No_يادگيري();
    }
    p->type_ignore_comments = (growable_comment_array){0};

    p->mark = 0;
    p->fill = 0;
    p->سائيز = 1;

    p->errcode = errcode;
    p->arena = arena;
    p->شروعات_قاعدو = شروعات_قاعدو;
    p->parsing_started = 0;
    p->normalize = عدم;
    p->error_indicator = 0;

    p->starting_lineno = 0;
    p->starting_کانو_وٿي = 0;
    p->flags = flags;
    p->feature_نسخو = feature_نسخو;
    p->معلوم_err_token = عدم;
    p->identifier_cache = عدم;
    p->tstate = PyThreadState_Get();
    // Stack limits are initialized when the thread state is attached.
    أكد(((_PyThreadStateImpl *)p->tstate)->c_ڍير_hard_حد_بندي != 0);
    p->ڍير_soft_حد_بندي = ((_PyThreadStateImpl *)p->tstate)->c_ڍير_soft_حد_بندي;
#إذا Py_ڍير_GROWS_هيٺ_ڏانهن
    p->ڍير_soft_حد_بندي += Py_OS_ڍير_MARGIN_بائيٽون;
#وإلا
    p->ڍير_soft_حد_بندي -= Py_OS_ڍير_MARGIN_بائيٽون;
#جيڪڏهن_پورو
    p->level = 0;
    p->call_invalid_قاعدا = 0;
    p->داخلا_exhausted = 0;
    p->آخري_stmt_location.lineno = 0;
    p->آخري_stmt_location.کانو_وٿي = 0;
    p->آخري_stmt_location.پڇاڙي_lineno = 0;
    p->آخري_stmt_location.پڇاڙي_کانو_وٿي = 0;
#جيڪڏهن_وصف_ٿيل Py_DEBUG
    p->debug = _Py_GetConfig()->parser_debug;
#جيڪڏهن_پورو
    أرجع p;
}

فراغ
_PyPegen_Parser_Free(Parser *p)
{
    PyMem_Free(p->identifier_cache);
    Py_XDECREF(p->normalize);
    // Resizes allocate blocks starting at indices 1, 2, 4, and so on.
    PyMem_Free(p->ٽوڪن[0]);
    لكل (صحيح i = 1; i < p->سائيز; i *= PEGEN_ARRAY_GROWTH_ضرب_ڪندڙ) {
        PyMem_Free(p->ٽوڪن[i]);
    }
    PyMem_Free(p->ٽوڪن);
    growable_comment_array_deallocate(&p->type_ignore_comments);
    PyMem_Free(p);
}

ساكن فراغ
reset_parser_state_for_error_pass(Parser *p)
{
    p->آخري_stmt_location.lineno = 0;
    p->آخري_stmt_location.کانو_وٿي = 0;
    p->آخري_stmt_location.پڇاڙي_lineno = 0;
    p->آخري_stmt_location.پڇاڙي_کانو_وٿي = 0;
    لكل (صحيح i = 0; i < p->fill; i++) {
        p->ٽوڪن[i]->ياداشت_3 = عدم;
        p->ٽوڪن[i]->ياداشت_نقاب = 0;
    }
    p->mark = 0;
    p->flags &= ~Py_PARSE_ALLOW_INCOMPLETE_داخلا;
    Py_Tokenizer_Allow_Incomplete_داخلا(p->tok, 0);
    p->call_invalid_قاعدا = 1;
    _PyTokenizer_StopInteractive(p->tok);
}

ساكن فراغ
Py_Pegen_سيٽ_syntax_error_metadata(Parser *p) {
    PyObject *exc = PyErr_GetRaisedException();
    إذا (!exc || !PyObject_TypeCheck(exc, (PyTypeObject *)PyExc_SyntaxError)) {
        Py_Err_سيٽ_Raised_Exception(exc);
        أرجع;
    }
    ثابت حرف *ذريعو = Py_Tokenizer_Retained_ذريعو(p->tok);
    ثابت حرف *encoding = _PyTokenizer_GetInfo(p->tok).encoding;
    PyObject* the_ذريعو = عدم;
    إذا (ذريعو) {
        إذا (encoding == عدم) {
            the_ذريعو = PyUnicode_FromString(ذريعو);
        } وإلا {
            the_ذريعو = PyUnicode_Decode(ذريعو, لڙي_ڊيگهه(ذريعو), encoding, عدم);
        }
    }
    إذا (!the_ذريعو) {
        PyErr_Clear();
        the_ذريعو = Py_None;
        Py_INCREF(the_ذريعو);
    }
    PyObject* metadata = Py_Build_قدر(
        "(iiN)",
        p->آخري_stmt_location.lineno,
        p->آخري_stmt_location.کانو_وٿي,
        the_ذريعو // N gives ownership to metadata
    );
    إذا (!metadata) {
        PyErr_Clear();
        Py_Err_سيٽ_Raised_Exception(exc);
        أرجع;
    }
    PySyntaxErrorObject *syntax_error = (PySyntaxErrorObject *)exc;

    Py_XDECREF(syntax_error->metadata);
    syntax_error->metadata = metadata;
    Py_Err_سيٽ_Raised_Exception(exc);
}

فراغ *
Py_Pegen_لاڳيتو_parser(Parser *p)
{
    فراغ *res = _PyPegen_parse(p);
    أكد(p->level == 0);
    إذا (res != عدم && PyErr_Occurred()) {
        // Discard a result returned with an exception still pending
        // (e.g. a MemoryError from a recovered-from allocation failure).
        أرجع عدم;
    }
    إذا (res == عدم) {
        إذا (PyErr_Occurred() &&
            (PyErr_ExceptionMatches(Py_Exc_Incomplete_داخلا_Error) ||
             !PyErr_ExceptionMatches(PyExc_SyntaxError))) {
            أرجع عدم;
        }
        إذا ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_داخلا) &&
            p->داخلا_exhausted && !PyErr_Occurred()) {
            أرجع Py_Pegen_raise_incomplete_داخلا(p);
        }
       // Make a second parser pass. In this pass we activate heavier and slower checks
        // to produce better error messages and more complete diagnostics. Extra "invalid_*"
        // rules will be active during parsing.
        Token *آخري_token = p->ٽوڪن[p->fill - 1];
        reset_parser_state_for_error_pass(p);
        _PyPegen_parse(p);

        // Set SyntaxErrors accordingly depending on the parser/tokenizer status at the failure
        // point.
        Pypegen_سيٽ_syntax_error(p, آخري_token);

        // Set the metadata in the exception from p->last_stmt_location
        إذا (PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            Py_Pegen_سيٽ_syntax_error_metadata(p);
        }
       أرجع عدم;
    }

    إذا (p->شروعات_قاعدو == Py_single_داخلا && _PyTokenizer_HasTrailingStatement(p->tok)) {
        أرجع RAISE_SYNTAX_ERROR("multiple statements found while compiling a single statement");
    }

    // test_peg_generator defines _Py_TEST_PEGEN to not call PyAST_Validate()
#إذا defined(Py_DEBUG) && !defined(_Py_TEST_PEGEN)
    إذا (p->شروعات_قاعدو == Py_single_داخلا ||
        p->شروعات_قاعدو == Py_file_داخلا ||
        p->شروعات_قاعدو == Py_eval_داخلا)
    {
        إذا (!_PyAST_Validate(res)) {
            أرجع عدم;
        }
    }
#جيڪڏهن_پورو
    أرجع res;
}

mod_ty
Py_Pegen_لاڳيتو_parser_from_file_اشارو(FILE *fp, صحيح شروعات_قاعدو, PyObject *filename_ob,
                             ثابت حرف *enc, ثابت حرف *ps1, ثابت حرف *ps2,
                             PyCompilerFlags *flags, صحيح *errcode,
                             PyObject **interactive_src, PyArena *arena)
{
    صحيح parser_flags = compute_parser_flags(flags);
    بنية tok_state *tok = _PyTokenizer_FromFile(fp, enc, ps1, ps2);
    إذا (tok == عدم) {
        إذا (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        وإلا {
            // The only silent tokenizer init failure is a failed allocation.
            Py_Err_No_يادگيري();
        }
        أرجع عدم;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    // From here on we need to clean up even if there's an error
    mod_ty نتيجو = عدم;

    PyObject *module = PyUnicode_FromString("__main__");
    إذا (module == عدم) {
        اذهب error;
    }
    Py_Tokenizer_سيٽ_Context(tok, filename_ob, module);
    Py_DECREF(module);

    Parser *p = _PyPegen_Parser_New(tok, شروعات_قاعدو, parser_flags, PY_MINOR_نسخو,
                                    errcode, عدم, arena);
    إذا (p == عدم) {
        اذهب error;
    }

    نتيجو = Py_Pegen_لاڳيتو_parser(p);
    _PyPegen_Parser_Free(p);

    ثابت حرف *ذريعو = Py_Tokenizer_Retained_ذريعو(tok);
    إذا (ذريعو != عدم && نتيجو && interactive_src != عدم) {
        *interactive_src = PyUnicode_FromString(ذريعو);
        إذا (!*interactive_src || Py_Arena_جوڙيو_Py_Object(arena, *interactive_src) < 0) {
            Py_XDECREF(*interactive_src);
            نتيجو = عدم;
            اذهب error;
        }
    }

error:
    _PyTokenizer_Free(tok);
    أرجع نتيجو;
}

mod_ty
Py_Pegen_لاڳيتو_parser_from_string(ثابت حرف *str, صحيح شروعات_قاعدو, PyObject *filename_ob,
                       PyCompilerFlags *flags, PyArena *arena, PyObject *module,
                       PyObject **incomplete)
{
    _PyTokenizer_Newline newline_گهڻائي = شروعات_قاعدو == Py_file_داخلا
        ? _PyTokenizer_IMPLY_NEWLINE : _PyTokenizer_KEEP_NEWLINE;
    صحيح parser_flags = compute_parser_flags(flags);
    إذا (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_داخلا) {
        newline_گهڻائي = _PyTokenizer_APPEND_NEWLINE;
    }

    بنية tok_state *tok;
    إذا (flags != عدم && flags->cf_flags & PyCF_IGNORE_COOKIE) {
        tok = _PyTokenizer_FromUTF8(str, newline_گهڻائي, 0);
    } وإلا {
        tok = _PyTokenizer_FromString(str, newline_گهڻائي, 0);
    }
    إذا (tok == عدم) {
        إذا (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        وإلا {
            // The only silent tokenizer init failure is a failed allocation.
            Py_Err_No_يادگيري();
        }
        أرجع عدم;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    Py_Tokenizer_سيٽ_Context(tok, filename_ob, module);

    // We need to clear up from here on
    mod_ty نتيجو = عدم;

    صحيح feature_نسخو = flags && (flags->cf_flags & PyCF_ONLY_AST) ?
        flags->cf_feature_نسخو : PY_MINOR_نسخو;
    Parser *p = _PyPegen_Parser_New(tok, شروعات_قاعدو, parser_flags, feature_نسخو,
                                    عدم, str, arena);
    إذا (p == عدم) {
        اذهب error;
    }

    نتيجو = Py_Pegen_لاڳيتو_parser(p);
    إذا (نتيجو != عدم && (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_داخلا)) {
        صحيح needs_newline = _PyTokenizer_GetNewlineRequirements(tok);
        إذا ((needs_newline & _PyTokenizer_NEWLINE_CONTINUATION) ||
            (شروعات_قاعدو != Py_file_داخلا &&
             (needs_newline & _PyTokenizer_NEWLINE_INDENT)) ||
            (شروعات_قاعدو == Py_single_داخلا && p->داخلا_exhausted &&
             (needs_newline & _PyTokenizer_NEWLINE_STATEMENT))) {
            Py_Pegen_raise_incomplete_داخلا(p);
            إذا (incomplete != عدم && PyErr_ExceptionMatches(Py_Exc_Incomplete_داخلا_Error)) {
                *incomplete = PyErr_GetRaisedException();
            }
            وإلا {
                نتيجو = عدم;
            }
        }
    }
    _PyPegen_Parser_Free(p);

error:
    _PyTokenizer_Free(tok);
    أرجع نتيجو;
}
