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

#تعریف_کرو IDENTIFIER_CACHE_ناپ 2048  // Must be a power of two.
#تعریف_کرو IDENTIFIER_CACHE_MAX_PROBES 8
#تعریف_کرو PEGEN_ARRAY_GROWTH_ضارب 2
#تعریف_کرو TYPE_IGNORE_INITIAL_سمرتھا 10

بنية identifier_cache_اندراج {
    ثابت حرف *کلید;   // Borrowed from arena-owned token bytes.
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
Py_Pegen_byte_وتھ_منزل_character_وتھ_سطر(PyObject *سطر, Py_ssize_t خانہ_وتھ, Py_ssize_t سرا_خانہ_وتھ)
{
    ثابت بنانشانی حرف *انکڑے = (ثابت بنانشانی حرف*)PyUnicode_AsUTF8(سطر);
    إذا (انکڑے == عدم) {
        أرجع -1;
    }

    Py_ssize_t len = 0;
    طالما (خانہ_وتھ < سرا_خانہ_وتھ) {
        Py_UCS4 ch = انکڑے[خانہ_وتھ];
        إذا (ch < 0x80) {
            خانہ_وتھ += 1;
        } وإلا إذا ((ch & 0xe0) == 0xc0) {
            خانہ_وتھ += 2;
        } وإلا إذا ((ch & 0xf0) == 0xe0) {
            خانہ_وتھ += 3;
        } وإلا إذا ((ch & 0xf8) == 0xf0) {
            خانہ_وتھ += 4;
        } وإلا {
            Py_Err_سیٹ_String(Py_Exc_قدر_Error, "Invalid UTF-8 sequence");
            أرجع -1;
        }
        len++;
    }
    أرجع len;
}

Py_ssize_t
Py_Pegen_byte_وتھ_منزل_character_وتھ_raw(ثابت حرف* str, Py_ssize_t خانہ_وتھ)
{
    Py_ssize_t len = (Py_ssize_t)لڑی_لمبائی(str);
    إذا (خانہ_وتھ > len + 1) {
        خانہ_وتھ = len + 1;
    }
    أكد(خانہ_وتھ >= 0);
    PyObject *متن = PyUnicode_DecodeUTF8(str, خانہ_وتھ, "replace");
    إذا (!متن) {
        أرجع -1;
    }
    Py_ssize_t ناپ = Py_Unicode_GET_لمائی(متن);
    Py_DECREF(متن);
    أرجع ناپ;
}

Py_ssize_t
Py_Pegen_byte_وتھ_منزل_character_وتھ(PyObject *سطر, Py_ssize_t خانہ_وتھ)
{
    ثابت حرف *str = PyUnicode_AsUTF8(سطر);
    إذا (!str) {
        أرجع -1;
    }
    أرجع Py_Pegen_byte_وتھ_منزل_character_وتھ_raw(str, خانہ_وتھ);
}

// Here, mark is the start of the node, while p->mark is the end.
// If node==NULL, they should be the same.
صحيح
Py_Pegen_داخل_کرو_یاد(Parser *p, صحيح mark, صحيح type, فراغ *گرہ)
{
    // Insert in front
    یاد *m = _PyArena_Malloc(p->arena, حجم(یاد));
    إذا (m == عدم) {
        أرجع -1;
    }
    m->type = type;
    m->گرہ = گرہ;
    m->mark = p->mark;
    m->اگلا = p->ٹوکن[mark]->یاد_3;
    p->ٹوکن[mark]->یاد_3 = m;
    p->ٹوکن[mark]->یاد_نقاب |= 1ULL << (type & 63);
    أرجع 0;
}

// Like _PyPegen_insert_memo(), but returns the inserted Memo so callers
// can update it in place without re-walking the token's memo list.
یاد *
Py_Pegen_داخل_کرو_یاد_سدھا(Parser *p, صحيح mark, صحيح type)
{
    إذا (Py_Pegen_داخل_کرو_یاد(p, mark, type, عدم) < 0) {
        أرجع عدم;
    }
    أرجع p->ٹوکن[mark]->یاد_3;
}

// Like _PyPegen_insert_memo(), but updates an existing node if found.
صحيح
Py_Pegen_update_یاد(Parser *p, صحيح mark, صحيح type, فراغ *گرہ)
{
    لكل (یاد *m = p->ٹوکن[mark]->یاد_3; m != عدم; m = m->اگلا) {
        إذا (m->type == type) {
            // Update existing node.
            m->گرہ = گرہ;
            m->mark = p->mark;
            أرجع 0;
        }
    }
    // Insert new node.
    أرجع Py_Pegen_داخل_کرو_یاد(p, mark, type, گرہ);
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
Py_Pegen_grow_loop_بفر(فراغ ***بفر, Py_ssize_t *سمرتھا)
{
    إذا ((ناپ_t)*سمرتھا > (ناپ_t)PY_SSIZE_T_MAX /
            (PEGEN_ARRAY_GROWTH_ضارب * حجم(**بفر))) {
        أرجع -1;
    }
    Py_ssize_t new_سمرتھا = *سمرتھا == 0
        ? 1 : *سمرتھا * PEGEN_ARRAY_GROWTH_ضارب;
    فراغ **new_بفر = PyMem_Realloc(*بفر, new_سمرتھا * حجم(**بفر));
    إذا (new_بفر == عدم) {
        أرجع -1;
    }
    *بفر = new_بفر;
    *سمرتھا = new_سمرتھا;
    أرجع 0;
}

ساكن صحيح
growable_comment_array_جوڑو(growable_comment_array *arr, صحيح lineno, حرف *comment)
{
    إذا (arr->عدد_اشیاء >= arr->ناپ) {
        إذا (arr->ناپ > (ناپ_t)PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_ضارب * حجم(*arr->اشیاء))) {
            أرجع 0;
        }
        ناپ_t new_ناپ = arr->ناپ == 0
            ? TYPE_IGNORE_INITIAL_سمرتھا
            : arr->ناپ * PEGEN_ARRAY_GROWTH_ضارب;
        فراغ *new_اشیاء_array = PyMem_Realloc(
            arr->اشیاء, new_ناپ * حجم(*arr->اشیاء));
        إذا (!new_اشیاء_array) {
            أرجع 0;
        }
        arr->اشیاء = new_اشیاء_array;
        arr->ناپ = new_ناپ;
    }

    arr->اشیاء[arr->عدد_اشیاء].lineno = lineno;
    arr->اشیاء[arr->عدد_اشیاء].comment = comment;  // Take ownership
    arr->عدد_اشیاء++;
    أرجع 1;
}

ساكن فراغ
growable_comment_array_deallocate(growable_comment_array *arr)
{
    لكل (ناپ_t i = 0; i < arr->عدد_اشیاء; i++) {
        PyMem_Free(arr->اشیاء[i].comment);
    }
    PyMem_Free(arr->اشیاء);
}

ساكن صحيح
_get_keyword_or_name_type(Parser *p, ثابت حرف *متن, Py_ssize_t لمائی)
{
    أكد(لمائی > 0);

    إذا (لمائی >= p->n_keyword_lists ||
        p->keywords[لمائی] == عدم ||
        p->keywords[لمائی]->type == -1) {
        أرجع NAME;
    }
    لكل (KeywordToken *k = p->keywords[لمائی]; k != عدم && k->type != -1; k++) {
        إذا (memcmp(k->str, متن, (ناپ_t)لمائی) == 0) {
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
        حالة نمبر:
        حالة STRING:
        حالة FSTRING_شروع:
        حالة FSTRING_درمیان:
        حالة FSTRING_سرا:
        حالة TSTRING_شروع:
        حالة TSTRING_درمیان:
        حالة TSTRING_سرا:
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
    parser_token->بائٹاں = عدم;
    إذا (token_needs_متن(token_type) && new_token->وسعت.شروع >= 0) {
        Py_ssize_t لمائی;
        ثابت حرف *متن = Py_Token_متن_View(p->tok, new_token, &لمائی);
        إذا (token_type == NAME) {
            parser_token->type = _get_keyword_or_name_type(p, متن, لمائی);
        }
        // A name reclassified as a keyword does not need its text.
        إذا (parser_token->type == token_type) {
            parser_token->بائٹاں = Py_بائٹاں_From_String_And_ناپ(متن, لمائی);
            إذا (parser_token->بائٹاں == عدم) {
                أرجع -1;
            }
            إذا (Py_Arena_جوڑو_Py_Object(p->arena, parser_token->بائٹاں) < 0) {
                Py_DECREF(parser_token->بائٹاں);
                أرجع -1;
            }
        }
    }

    parser_token->metadata = عدم;
    إذا (new_token->metadata != عدم) {
        إذا (Py_Arena_جوڑو_Py_Object(p->arena, new_token->metadata) < 0) {
            أرجع -1;
        }
        parser_token->metadata = new_token->metadata;
        new_token->metadata = عدم;
    }

    parser_token->level = new_token->level;
    parser_token->is_raw = new_token->is_raw;
    parser_token->lineno = new_token->شروع_loc.lineno;
    parser_token->خانہ_وتھ = new_token->سرا_loc.lineno == p->starting_lineno
        ? p->starting_خانہ_وتھ + new_token->شروع_loc.byte_خانہ
        : new_token->شروع_loc.byte_خانہ;
    parser_token->سرا_lineno = new_token->سرا_loc.lineno;
    parser_token->سرا_خانہ_وتھ = new_token->سرا_loc.lineno == p->starting_lineno
        ? p->starting_خانہ_وتھ + new_token->سرا_loc.byte_خانہ
        : new_token->سرا_loc.byte_خانہ;

    p->fill += 1;

    إذا (token_type == ERRORTOKEN && Py_Tokenizer_Get_حالت(p->tok) == E_DECODE) {
        أرجع _Pypegen_raise_decode_error(p);
    }

    أرجع (token_type == ERRORTOKEN ? _Pypegen_tokenizer_error(p) : 0);
}

ساكن صحيح
resize_ٹوکن_array(Parser *p) {
    إذا (p->ناپ > ودھ_توں_ودھ_عدد / PEGEN_ARRAY_GROWTH_ضارب ||
            (ناپ_t)p->ناپ > PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_ضارب * حجم(*p->ٹوکن))) {
        Py_Err_No_حافظہ();
        أرجع -1;
    }
    صحيح newsize = p->ناپ * PEGEN_ARRAY_GROWTH_ضارب;
    Token **new_ٹوکن = PyMem_Realloc(p->ٹوکن, (ناپ_t)newsize * حجم(Token *));
    إذا (new_ٹوکن == عدم) {
        Py_Err_No_حافظہ();
        أرجع -1;
    }
    p->ٹوکن = new_ٹوکن;

    // Allocate new tokens together without moving existing ones: parser rules
    // may still hold pointers into earlier blocks. Parser_Free frees each block.
    Token *chunk = PyMem_Calloc((ناپ_t)(newsize - p->ناپ), حجم(Token));
    إذا (chunk == عدم) {
        Py_Err_No_حافظہ();
        أرجع -1;
    }
    لكل (صحيح i = p->ناپ; i < newsize; i++) {
        p->ٹوکن[i] = &chunk[i - p->ناپ];
    }
    p->ناپ = newsize;
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
        حرف *ٹیگ = PyMem_Malloc((ناپ_t)len + 1);
        إذا (ٹیگ == عدم) {
            Py_Err_No_حافظہ();
            اذهب error;
        }
        حافظہ_نقل(ٹیگ, متن, (ناپ_t)len);
        ٹیگ[len] = '\0';
        // Ownership of tag passes to the growable array
        إذا (!growable_comment_array_جوڑو(&p->type_ignore_comments, new_token.سرا_loc.lineno, ٹیگ)) {
            PyMem_Free(ٹیگ);
            Py_Err_No_حافظہ();
            اذهب error;
        }
        _PyTokenizer_Get(p->tok, &new_token);
    }

    صحيح type = new_token.type;
    // EOF can finish an ordinary token. Only a boundary token means that
    // the grammar requested input beyond the last physical line.
    إذا (type == ENDMARKER ||
        (type == DEDENT && (p->flags & Py_PARSE_ALLOW_INCOMPLETE_داخلہ) &&
         Py_Tokenizer_Get_حالت(p->tok) == E_EOF)) {
        p->داخلہ_exhausted = 1;
    }

    // If we have reached the end and we are in single input mode we need to insert a newline and reset the parsing
    إذا (p->شروع_قاعدہ == Py_single_داخلہ && type == ENDMARKER && p->parsing_started) {
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
    إذا ((p->fill == p->ناپ) && (resize_ٹوکن_array(p) != 0)) {
        اذهب error;
    }

    Token *t = p->ٹوکن[p->fill];
    صحيح نتیجہ = initialize_token(p, t, &new_token, type);
    _PyToken_Free(&new_token);
    أرجع نتیجہ;
error:
    _PyToken_Free(&new_token);
    أرجع -1;
}

#إذا defined(Py_DEBUG)
// Instrumentation to count the effectiveness of memoization.
// The array counts the number of tokens skipped by memoization,
// indexed by type.

#تعریف_کرو NSTATISTICS _PYPEGEN_NSTATISTICS
#تعریف_کرو یاد_statistics _PyRuntime.parser.یاد_statistics

فراغ
Py_Pegen_clear_یاد_statistics(فراغ)
{
    Py_Mutex_جندرا(&_PyRuntime.parser.mutex);
    لكل (صحيح i = 0; i < NSTATISTICS; i++) {
        یاد_statistics[i] = 0;
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
}

PyObject *
Py_Pegen_get_یاد_statistics(فراغ)
{
    PyObject *ret = PyList_New(NSTATISTICS);
    إذا (ret == عدم) {
        أرجع عدم;
    }

    Py_Mutex_جندرا(&_PyRuntime.parser.mutex);
    لكل (صحيح i = 0; i < NSTATISTICS; i++) {
        PyObject *قدر = PyLong_FromLong(یاد_statistics[i]);
        إذا (قدر == عدم) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            أرجع عدم;
        }
        // PyList_SetItem borrows a reference to value.
        إذا (Py_List_سیٹ_شے(ret, i, قدر) < 0) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            أرجع عدم;
        }
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
    أرجع ret;
}
#جے_انت

صحيح  // bool
_PyPegen_is_memoized(Parser *p, صحيح type, فراغ *pres)
{
    إذا (p->mark == p->fill) {
        إذا (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            أرجع -1;
        }
    }

    Token *t = p->ٹوکن[p->mark];

    إذا (!(t->یاد_نقاب & (1ULL << (type & 63)))) {
        أرجع 0;
    }

    لكل (یاد *m = t->یاد_3; m != عدم; m = m->اگلا) {
        إذا (m->type == type) {
#إذا defined(Py_DEBUG)
            إذا (0 <= type && type < NSTATISTICS) {
                لما گنتی = m->mark - p->mark;
                // A memoized negative result counts for one.
                إذا (گنتی <= 0) {
                    گنتی = 1;
                }
                Py_Mutex_جندرا(&_PyRuntime.parser.mutex);
                یاد_statistics[type] += گنتی;
                PyMutex_Unlock(&_PyRuntime.parser.mutex);
            }
#جے_انت
            p->mark = m->mark;
            *(فراغ **)(pres) = m->گرہ;
            أرجع 1;
        }
    }
    أرجع 0;
}

#تعریف_کرو LOOKAHEAD1(NAME, RES_TYPE)                                  \
    صحيح                                                             \
    NAME (صحيح مثبت, RES_TYPE (func)(Parser *), Parser *p)       \
    {                                                               \
        صحيح mark = p->mark;                                         \
        فراغ *res = func(p);                                        \
        p->mark = mark;                                             \
        أرجع (res != عدم) == مثبت;                           \
    }

LOOKAHEAD1(_PyPegen_lookahead, فراغ *)
LOOKAHEAD1(Py_Pegen_lookahead_for_اظہاریہ, اظہاریہ_ty)
LOOKAHEAD1(_PyPegen_lookahead_for_stmt, stmt_ty)
#تعریف_رد LOOKAHEAD1

#تعریف_کرو LOOKAHEAD2(NAME, RES_TYPE, T)                                   \
    صحيح                                                                 \
    NAME (صحيح مثبت, RES_TYPE (func)(Parser *, T), Parser *p, T arg) \
    {                                                                   \
        صحيح mark = p->mark;                                             \
        فراغ *res = func(p, arg);                                       \
        p->mark = mark;                                                 \
        أرجع (res != عدم) == مثبت;                               \
    }

LOOKAHEAD2(_PyPegen_lookahead_with_int, Token *, صحيح)
LOOKAHEAD2(_PyPegen_lookahead_with_string, اظہاریہ_ty, ثابت حرف *)
#تعریف_رد LOOKAHEAD2

Token *
_PyPegen_expect_token(Parser *p, صحيح type)
{
    إذا (p->mark == p->fill) {
        إذا (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            أرجع عدم;
        }
    }
    Token *t = p->ٹوکن[p->mark];
    إذا (t->type != type) {
       أرجع عدم;
    }
    p->mark += 1;
    أرجع t;
}

فراغ*
Py_Pegen_expect_forced_نتیجہ(Parser *p, فراغ* نتیجہ, ثابت حرف* expected) {

    إذا (p->error_indicator == 1) {
        أرجع عدم;
    }
    إذا (نتیجہ == عدم) {
        RAISE_SYNTAX_ERROR("expected (%s)", expected);
        أرجع عدم;
    }
    أرجع نتیجہ;
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
    Token *t = p->ٹوکن[p->mark];
    إذا (t->type != type) {
        RAISE_SYNTAX_ERROR_معلوم_LOCATION(t, "expected '%s'", expected);
        أرجع عدم;
    }
    p->mark += 1;
    أرجع t;
}

اظہاریہ_ty
_PyPegen_expect_soft_keyword(Parser *p, ثابت حرف *keyword)
{
    إذا (p->mark == p->fill) {
        إذا (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            أرجع عدم;
        }
    }
    Token *t = p->ٹوکن[p->mark];
    إذا (t->type != NAME) {
        أرجع عدم;
    }
    ثابت حرف *s = Py_بائٹاں_As_String(t->بائٹاں);
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
Py_Pegen_get_آخری_nonnwhitespace_token(Parser *p)
{
    أكد(p->mark >= 0);
    Token *token = عدم;
    لكل (صحيح m = p->mark - 1; m >= 0; m--) {
        token = p->ٹوکن[m];
        إذا (token->type != ENDMARKER && (token->type < NEWLINE || token->type > DEDENT)) {
            اكسر;
        }
    }
    أرجع token;
}

PyObject *
_PyPegen_new_identifier(Parser *p, ثابت حرف *n)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, (Py_ssize_t)لڑی_لمبائی(n), عدم);
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
        PyObject *دلیلاں[2] = {form, id};
        PyObject *id2 = PyObject_Vectorcall(p->normalize, دلیلاں, 2, عدم);
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
    إذا (Py_Arena_جوڑو_Py_Object(p->arena, id) < 0)
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
get_cached_identifier(Parser *p, PyObject *بائٹاں)
{
    ثابت حرف *s = Py_بائٹاں_As_String(بائٹاں);
    إذا (!s) {
        p->error_indicator = 1;
        أرجع عدم;
    }
    // Identifiers repeat constantly; a small span-keyed cache skips the
    // UTF-8 decode + intern for repeated occurrences. Keys point into
    // arena-owned token bytes and values are arena-owned interned strings,
    // so borrowed references are valid for the lifetime of the parse
    // (including the second error pass, which reuses parser and arena).
    Py_ssize_t len = Py_بائٹاں_GET_ناپ(بائٹاں);
    Py_hash_t hash = PyObject_Hash(بائٹاں);
    إذا (hash == -1) {
        p->error_indicator = 1;
        أرجع عدم;
    }
    // Parses without identifiers do not need this cache.
    إذا (p->identifier_cache == عدم) {
        p->identifier_cache = PyMem_Calloc(
            IDENTIFIER_CACHE_ناپ, حجم(*p->identifier_cache));
        إذا (p->identifier_cache == عدم) {
            p->error_indicator = 1;
            أرجع Py_Err_No_حافظہ();
        }
    }
    Identifier_Cache_اندراج *free_تھاں = عدم;
    ناپ_t idx = (ناپ_t)hash & (IDENTIFIER_CACHE_ناپ - 1);
    لكل (صحيح probe = 0; probe < IDENTIFIER_CACHE_MAX_PROBES; probe++) {
        Identifier_Cache_اندراج *اندراج = &p->identifier_cache[
            (idx + probe) & (IDENTIFIER_CACHE_ناپ - 1)];
        إذا (اندراج->کلید == عدم) {
            free_تھاں = اندراج;
            اكسر;
        }
        إذا (اندراج->hash == hash && اندراج->len == len &&
            memcmp(اندراج->کلید, s, len) == 0)
        {
            أرجع اندراج->قدر;
        }
    }
    PyObject *id = _PyPegen_new_identifier(p, s);
    إذا (id == عدم) {
        p->error_indicator = 1;
        أرجع عدم;
    }
    إذا (free_تھاں != عدم) {
        free_تھاں->کلید = s;
        free_تھاں->len = len;
        free_تھاں->hash = hash;
        free_تھاں->قدر = id;
    }
    أرجع id;
}

ساكن اظہاریہ_ty
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
    اظہاریہ_ty cached = عدم;
    إذا (_PyPegen_is_memoized(p, NAME, &cached)) {
        أرجع cached;
    }
    p->mark = mark + 1;
    PyObject *id = get_cached_identifier(p, t->بائٹاں);
    إذا (id == عدم) {
        أرجع عدم;
    }
    اظہاریہ_ty نتیجہ = _PyAST_Name(id, لدو, t->lineno, t->خانہ_وتھ,
                                t->سرا_lineno, t->سرا_خانہ_وتھ, p->arena);
    إذا (نتیجہ != عدم && Py_Pegen_داخل_کرو_یاد(p, mark, NAME, نتیجہ) < 0) {
        p->error_indicator = 1;
        أرجع عدم;
    }
    أرجع نتیجہ;
}

اظہاریہ_ty
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

اظہاریہ_ty _PyPegen_soft_keyword_token(Parser *p) {
    Token *t = _PyPegen_expect_token(p, NAME);
    إذا (t == عدم) {
        أرجع عدم;
    }
    حرف *the_token;
    Py_ssize_t ناپ;
    Py_بائٹاں_As_String_And_ناپ(t->بائٹاں, &the_token, &ناپ);
    لكل (حرف **keyword = p->soft_keywords; *keyword != عدم; keyword++) {
        إذا (لڑی_لمبائی(*keyword) == (ناپ_t)ناپ &&
            لڑی_مقابلہ(*keyword, the_token, (ناپ_t)ناپ) == 0) {
            أرجع _PyPegen_name_from_token(p, t);
        }
    }
    أرجع عدم;
}

ساكن PyObject *
parsenumber_raw(ثابت حرف *s)
{
    ثابت حرف *سرا;
    لما x;
    مزدوج dx;
    Py_complex compl;
    صحيح imflag;

    أكد(s != عدم);
    errno = 0;
    سرا = s + لڑی_لمبائی(s) - 1;
    imflag = *سرا == 'j' || *سرا == 'J';
    إذا (s[0] == '0') {
        x = (لما)PyOS_strtoul(s, (حرف **)&سرا, 0);
        إذا (x < 0 && errno == 0) {
            أرجع PyLong_FromString(s, (حرف **)0, 0);
        }
    }
    وإلا {
        x = PyOS_strtol(s, (حرف **)&سرا, 0);
    }
    إذا (*سرا == '\0') {
        إذا (errno != 0) {
            أرجع PyLong_FromString(s, (حرف **)0, 0);
        }
        أرجع PyLong_FromLong(x);
    }
    /* XXX Huge floats may silently fail */
    إذا (imflag) {
        compl.real = 0.;
        compl.imag = Py_OS_string_منزل_double(s, (حرف **)&سرا, عدم);
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
    حرف *سرا;
    PyObject *res = عدم;

    أكد(s != عدم);

    إذا (strchr(s, '_') == عدم) {
        أرجع parsenumber_raw(s);
    }
    /* Create a duplicate without underscores. */
    dup = PyMem_Malloc(لڑی_لمبائی(s) + 1);
    إذا (dup == عدم) {
        أرجع Py_Err_No_حافظہ();
    }
    سرا = dup;
    لكل (; *s; s++) {
        إذا (*s != '_') {
            *سرا++ = *s;
        }
    }
    *سرا = '\0';
    res = parsenumber_raw(dup);
    PyMem_Free(dup);
    أرجع res;
}

اظہاریہ_ty
Py_Pegen_نمبر_token(Parser *p)
{
    صحيح mark = p->mark;
    Token *t = _PyPegen_expect_token(p, نمبر);
    إذا (t == عدم) {
        أرجع عدم;
    }

    // Look up the node at the token's start, as for NAME tokens. A memo hit
    // advances the mark; on a miss, restore the position after the token.
    p->mark = mark;
    اظہاریہ_ty cached = عدم;
    إذا (_PyPegen_is_memoized(p, نمبر, &cached)) {
        أرجع cached;
    }
    p->mark = mark + 1;

    ثابت حرف *عدد_raw = Py_بائٹاں_As_String(t->بائٹاں);
    إذا (عدد_raw == عدم) {
        p->error_indicator = 1;
        أرجع عدم;
    }

    إذا (p->feature_نسخہ < 6 && strchr(عدد_raw, '_') != عدم) {
        p->error_indicator = 1;
        أرجع RAISE_SYNTAX_ERROR("Underscores in numeric literals are only supported "
                                  "in Python 3.6 and greater");
    }

    PyObject *c = parsenumber(عدد_raw);

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
                t->سرا_lineno, -1 /* end_col_offset */,
                "%S - Consider hexadecimal for huge integer literals "
                "to avoid decimal conversion limits.",
                exc);
            Py_DECREF(exc);
        }
        أرجع عدم;
    }

    إذا (Py_Arena_جوڑو_Py_Object(p->arena, c) < 0) {
        Py_DECREF(c);
        p->error_indicator = 1;
        أرجع عدم;
    }

    اظہاریہ_ty نتیجہ = _PyAST_Constant(c, عدم, t->lineno, t->خانہ_وتھ,
                                    t->سرا_lineno, t->سرا_خانہ_وتھ, p->arena);
    إذا (نتیجہ != عدم && Py_Pegen_داخل_کرو_یاد(p, mark, نمبر, نتیجہ) < 0) {
        p->error_indicator = 1;
        أرجع عدم;
    }
    أرجع نتیجہ;
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
    إذا (flags->cf_flags & Py_CF_ALLOW_INCOMPLETE_داخلہ) {
        parser_flags |= Py_PARSE_ALLOW_INCOMPLETE_داخلہ;
    }
    أرجع parser_flags;
}

// Parser API

Parser *
_PyPegen_Parser_New(بنية tok_state *tok, صحيح شروع_قاعدہ, صحيح flags,
                    صحيح feature_نسخہ, صحيح *errcode, ثابت حرف* سروت, PyArena *arena)
{
    Parser *p = PyMem_Malloc(حجم(Parser));
    إذا (p == عدم) {
        أرجع (Parser *) Py_Err_No_حافظہ();
    }
    أكد(tok != عدم);
    Py_Tokenizer_سیٹ_Options(tok, 0, (flags & PyPARSE_TYPE_COMMENTS) > 0);
    Py_Tokenizer_Allow_Incomplete_داخلہ(tok, flags & Py_PARSE_ALLOW_INCOMPLETE_داخلہ);
    p->tok = tok;
    p->keywords = عدم;
    p->n_keyword_lists = -1;
    p->soft_keywords = عدم;
    p->ٹوکن = PyMem_Malloc(حجم(Token *));
    إذا (!p->ٹوکن) {
        PyMem_Free(p);
        أرجع (Parser *) Py_Err_No_حافظہ();
    }
    p->ٹوکن[0] = PyMem_Calloc(1, حجم(Token));
    إذا (!p->ٹوکن[0]) {
        PyMem_Free(p->ٹوکن);
        PyMem_Free(p);
        أرجع (Parser *) Py_Err_No_حافظہ();
    }
    p->type_ignore_comments = (growable_comment_array){0};

    p->mark = 0;
    p->fill = 0;
    p->ناپ = 1;

    p->errcode = errcode;
    p->arena = arena;
    p->شروع_قاعدہ = شروع_قاعدہ;
    p->parsing_started = 0;
    p->normalize = عدم;
    p->error_indicator = 0;

    p->starting_lineno = 0;
    p->starting_خانہ_وتھ = 0;
    p->flags = flags;
    p->feature_نسخہ = feature_نسخہ;
    p->معلوم_err_token = عدم;
    p->identifier_cache = عدم;
    p->tstate = PyThreadState_Get();
    // Stack limits are initialized when the thread state is attached.
    أكد(((_PyThreadStateImpl *)p->tstate)->c_ڈھیر_hard_حد_بندی != 0);
    p->ڈھیر_soft_حد_بندی = ((_PyThreadStateImpl *)p->tstate)->c_ڈھیر_soft_حد_بندی;
#إذا Py_ڈھیر_GROWS_تھلے
    p->ڈھیر_soft_حد_بندی += Py_OS_ڈھیر_MARGIN_بائٹاں;
#وإلا
    p->ڈھیر_soft_حد_بندی -= Py_OS_ڈھیر_MARGIN_بائٹاں;
#جے_انت
    p->level = 0;
    p->call_invalid_قاعدے = 0;
    p->داخلہ_exhausted = 0;
    p->آخری_stmt_location.lineno = 0;
    p->آخری_stmt_location.خانہ_وتھ = 0;
    p->آخری_stmt_location.سرا_lineno = 0;
    p->آخری_stmt_location.سرا_خانہ_وتھ = 0;
#جے_تعریف_شدہ Py_DEBUG
    p->debug = _Py_GetConfig()->parser_debug;
#جے_انت
    أرجع p;
}

فراغ
_PyPegen_Parser_Free(Parser *p)
{
    PyMem_Free(p->identifier_cache);
    Py_XDECREF(p->normalize);
    // Resizes allocate blocks starting at indices 1, 2, 4, and so on.
    PyMem_Free(p->ٹوکن[0]);
    لكل (صحيح i = 1; i < p->ناپ; i *= PEGEN_ARRAY_GROWTH_ضارب) {
        PyMem_Free(p->ٹوکن[i]);
    }
    PyMem_Free(p->ٹوکن);
    growable_comment_array_deallocate(&p->type_ignore_comments);
    PyMem_Free(p);
}

ساكن فراغ
reset_parser_state_for_error_pass(Parser *p)
{
    p->آخری_stmt_location.lineno = 0;
    p->آخری_stmt_location.خانہ_وتھ = 0;
    p->آخری_stmt_location.سرا_lineno = 0;
    p->آخری_stmt_location.سرا_خانہ_وتھ = 0;
    لكل (صحيح i = 0; i < p->fill; i++) {
        p->ٹوکن[i]->یاد_3 = عدم;
        p->ٹوکن[i]->یاد_نقاب = 0;
    }
    p->mark = 0;
    p->flags &= ~Py_PARSE_ALLOW_INCOMPLETE_داخلہ;
    Py_Tokenizer_Allow_Incomplete_داخلہ(p->tok, 0);
    p->call_invalid_قاعدے = 1;
    _PyTokenizer_StopInteractive(p->tok);
}

ساكن فراغ
Py_Pegen_سیٹ_syntax_error_metadata(Parser *p) {
    PyObject *exc = PyErr_GetRaisedException();
    إذا (!exc || !PyObject_TypeCheck(exc, (PyTypeObject *)PyExc_SyntaxError)) {
        Py_Err_سیٹ_Raised_Exception(exc);
        أرجع;
    }
    ثابت حرف *سروت = Py_Tokenizer_Retained_سروت(p->tok);
    ثابت حرف *encoding = _PyTokenizer_GetInfo(p->tok).encoding;
    PyObject* the_سروت = عدم;
    إذا (سروت) {
        إذا (encoding == عدم) {
            the_سروت = PyUnicode_FromString(سروت);
        } وإلا {
            the_سروت = PyUnicode_Decode(سروت, لڑی_لمبائی(سروت), encoding, عدم);
        }
    }
    إذا (!the_سروت) {
        PyErr_Clear();
        the_سروت = Py_None;
        Py_INCREF(the_سروت);
    }
    PyObject* metadata = Py_Build_قدر(
        "(iiN)",
        p->آخری_stmt_location.lineno,
        p->آخری_stmt_location.خانہ_وتھ,
        the_سروت // N gives ownership to metadata
    );
    إذا (!metadata) {
        PyErr_Clear();
        Py_Err_سیٹ_Raised_Exception(exc);
        أرجع;
    }
    PySyntaxErrorObject *syntax_error = (PySyntaxErrorObject *)exc;

    Py_XDECREF(syntax_error->metadata);
    syntax_error->metadata = metadata;
    Py_Err_سیٹ_Raised_Exception(exc);
}

فراغ *
Py_Pegen_لگاتار_parser(Parser *p)
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
            (PyErr_ExceptionMatches(Py_Exc_Incomplete_داخلہ_Error) ||
             !PyErr_ExceptionMatches(PyExc_SyntaxError))) {
            أرجع عدم;
        }
        إذا ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_داخلہ) &&
            p->داخلہ_exhausted && !PyErr_Occurred()) {
            أرجع Py_Pegen_raise_incomplete_داخلہ(p);
        }
       // Make a second parser pass. In this pass we activate heavier and slower checks
        // to produce better error messages and more complete diagnostics. Extra "invalid_*"
        // rules will be active during parsing.
        Token *آخری_token = p->ٹوکن[p->fill - 1];
        reset_parser_state_for_error_pass(p);
        _PyPegen_parse(p);

        // Set SyntaxErrors accordingly depending on the parser/tokenizer status at the failure
        // point.
        Pypegen_سیٹ_syntax_error(p, آخری_token);

        // Set the metadata in the exception from p->last_stmt_location
        إذا (PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            Py_Pegen_سیٹ_syntax_error_metadata(p);
        }
       أرجع عدم;
    }

    إذا (p->شروع_قاعدہ == Py_single_داخلہ && _PyTokenizer_HasTrailingStatement(p->tok)) {
        أرجع RAISE_SYNTAX_ERROR("multiple statements found while compiling a single statement");
    }

    // test_peg_generator defines _Py_TEST_PEGEN to not call PyAST_Validate()
#إذا defined(Py_DEBUG) && !defined(_Py_TEST_PEGEN)
    إذا (p->شروع_قاعدہ == Py_single_داخلہ ||
        p->شروع_قاعدہ == Py_file_داخلہ ||
        p->شروع_قاعدہ == Py_eval_داخلہ)
    {
        إذا (!_PyAST_Validate(res)) {
            أرجع عدم;
        }
    }
#جے_انت
    أرجع res;
}

mod_ty
Py_Pegen_لگاتار_parser_from_file_اشارہ_گر(FILE *fp, صحيح شروع_قاعدہ, PyObject *filename_ob,
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
            Py_Err_No_حافظہ();
        }
        أرجع عدم;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    // From here on we need to clean up even if there's an error
    mod_ty نتیجہ = عدم;

    PyObject *module = PyUnicode_FromString("__main__");
    إذا (module == عدم) {
        اذهب error;
    }
    Py_Tokenizer_سیٹ_Context(tok, filename_ob, module);
    Py_DECREF(module);

    Parser *p = _PyPegen_Parser_New(tok, شروع_قاعدہ, parser_flags, PY_MINOR_نسخہ,
                                    errcode, عدم, arena);
    إذا (p == عدم) {
        اذهب error;
    }

    نتیجہ = Py_Pegen_لگاتار_parser(p);
    _PyPegen_Parser_Free(p);

    ثابت حرف *سروت = Py_Tokenizer_Retained_سروت(tok);
    إذا (سروت != عدم && نتیجہ && interactive_src != عدم) {
        *interactive_src = PyUnicode_FromString(سروت);
        إذا (!*interactive_src || Py_Arena_جوڑو_Py_Object(arena, *interactive_src) < 0) {
            Py_XDECREF(*interactive_src);
            نتیجہ = عدم;
            اذهب error;
        }
    }

error:
    _PyTokenizer_Free(tok);
    أرجع نتیجہ;
}

mod_ty
Py_Pegen_لگاتار_parser_from_string(ثابت حرف *str, صحيح شروع_قاعدہ, PyObject *filename_ob,
                       PyCompilerFlags *flags, PyArena *arena, PyObject *module,
                       PyObject **incomplete)
{
    _PyTokenizer_Newline newline_منوال = شروع_قاعدہ == Py_file_داخلہ
        ? _PyTokenizer_IMPLY_NEWLINE : _PyTokenizer_KEEP_NEWLINE;
    صحيح parser_flags = compute_parser_flags(flags);
    إذا (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_داخلہ) {
        newline_منوال = _PyTokenizer_APPEND_NEWLINE;
    }

    بنية tok_state *tok;
    إذا (flags != عدم && flags->cf_flags & PyCF_IGNORE_COOKIE) {
        tok = _PyTokenizer_FromUTF8(str, newline_منوال, 0);
    } وإلا {
        tok = _PyTokenizer_FromString(str, newline_منوال, 0);
    }
    إذا (tok == عدم) {
        إذا (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        وإلا {
            // The only silent tokenizer init failure is a failed allocation.
            Py_Err_No_حافظہ();
        }
        أرجع عدم;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    Py_Tokenizer_سیٹ_Context(tok, filename_ob, module);

    // We need to clear up from here on
    mod_ty نتیجہ = عدم;

    صحيح feature_نسخہ = flags && (flags->cf_flags & PyCF_ONLY_AST) ?
        flags->cf_feature_نسخہ : PY_MINOR_نسخہ;
    Parser *p = _PyPegen_Parser_New(tok, شروع_قاعدہ, parser_flags, feature_نسخہ,
                                    عدم, str, arena);
    إذا (p == عدم) {
        اذهب error;
    }

    نتیجہ = Py_Pegen_لگاتار_parser(p);
    إذا (نتیجہ != عدم && (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_داخلہ)) {
        صحيح needs_newline = _PyTokenizer_GetNewlineRequirements(tok);
        إذا ((needs_newline & _PyTokenizer_NEWLINE_CONTINUATION) ||
            (شروع_قاعدہ != Py_file_داخلہ &&
             (needs_newline & _PyTokenizer_NEWLINE_INDENT)) ||
            (شروع_قاعدہ == Py_single_داخلہ && p->داخلہ_exhausted &&
             (needs_newline & _PyTokenizer_NEWLINE_STATEMENT))) {
            Py_Pegen_raise_incomplete_داخلہ(p);
            إذا (incomplete != عدم && PyErr_ExceptionMatches(Py_Exc_Incomplete_داخلہ_Error)) {
                *incomplete = PyErr_GetRaisedException();
            }
            وإلا {
                نتیجہ = عدم;
            }
        }
    }
    _PyPegen_Parser_Free(p);

error:
    _PyTokenizer_Free(tok);
    أرجع نتیجہ;
}
