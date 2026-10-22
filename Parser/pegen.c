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

#تعریف_کن IDENTIFIER_CACHE_اندازه 2048  // Must be a power of two.
#تعریف_کن IDENTIFIER_CACHE_MAX_PROBES 8
#تعریف_کن PEGEN_ARRAY_GROWTH_ضریب 2
#تعریف_کن TYPE_IGNORE_INITIAL_ظرفیت 10

بنية identifier_cache_مدخل {
    ثابت حرف *کلید;   // Borrowed from arena-owned token bytes.
    Py_ssize_t len;
    Py_hash_t hash;
    PyObject *مقدار;   // Borrowed from an arena-owned identifier.
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
Py_Pegen_byte_جابه_جایی_مقصد_character_جابه_جایی_سطر(PyObject *سطر, Py_ssize_t ستون_جابه_جایی, Py_ssize_t انتها_ستون_جابه_جایی)
{
    ثابت بیعلامت حرف *داده = (ثابت بیعلامت حرف*)PyUnicode_AsUTF8(سطر);
    إذا (داده == عدم) {
        أرجع -1;
    }

    Py_ssize_t len = 0;
    طالما (ستون_جابه_جایی < انتها_ستون_جابه_جایی) {
        Py_UCS4 ch = داده[ستون_جابه_جایی];
        إذا (ch < 0x80) {
            ستون_جابه_جایی += 1;
        } وإلا إذا ((ch & 0xe0) == 0xc0) {
            ستون_جابه_جایی += 2;
        } وإلا إذا ((ch & 0xf0) == 0xe0) {
            ستون_جابه_جایی += 3;
        } وإلا إذا ((ch & 0xf8) == 0xf0) {
            ستون_جابه_جایی += 4;
        } وإلا {
            Py_Err_مجموعه_String(Py_Exc_مقدار_Error, "Invalid UTF-8 sequence");
            أرجع -1;
        }
        len++;
    }
    أرجع len;
}

Py_ssize_t
Py_Pegen_byte_جابه_جایی_مقصد_character_جابه_جایی_raw(ثابت حرف* str, Py_ssize_t ستون_جابه_جایی)
{
    Py_ssize_t len = (Py_ssize_t)طول_رشته(str);
    إذا (ستون_جابه_جایی > len + 1) {
        ستون_جابه_جایی = len + 1;
    }
    أكد(ستون_جابه_جایی >= 0);
    PyObject *متن = PyUnicode_DecodeUTF8(str, ستون_جابه_جایی, "replace");
    إذا (!متن) {
        أرجع -1;
    }
    Py_ssize_t اندازه = Py_Unicode_GET_درازی(متن);
    Py_DECREF(متن);
    أرجع اندازه;
}

Py_ssize_t
Py_Pegen_byte_جابه_جایی_مقصد_character_جابه_جایی(PyObject *سطر, Py_ssize_t ستون_جابه_جایی)
{
    ثابت حرف *str = PyUnicode_AsUTF8(سطر);
    إذا (!str) {
        أرجع -1;
    }
    أرجع Py_Pegen_byte_جابه_جایی_مقصد_character_جابه_جایی_raw(str, ستون_جابه_جایی);
}

// Here, mark is the start of the node, while p->mark is the end.
// If node==NULL, they should be the same.
صحيح
Py_Pegen_درج_یادداشت(Parser *p, صحيح mark, صحيح type, فراغ *گره)
{
    // Insert in front
    یادداشت *m = _PyArena_Malloc(p->arena, حجم(یادداشت));
    إذا (m == عدم) {
        أرجع -1;
    }
    m->type = type;
    m->گره = گره;
    m->mark = p->mark;
    m->بعدی = p->نشانهها[mark]->یادداشت_3;
    p->نشانهها[mark]->یادداشت_3 = m;
    p->نشانهها[mark]->یادداشت_ماسک |= 1ULL << (type & 63);
    أرجع 0;
}

// Like _PyPegen_insert_memo(), but returns the inserted Memo so callers
// can update it in place without re-walking the token's memo list.
یادداشت *
Py_Pegen_درج_یادداشت_مستقیم(Parser *p, صحيح mark, صحيح type)
{
    إذا (Py_Pegen_درج_یادداشت(p, mark, type, عدم) < 0) {
        أرجع عدم;
    }
    أرجع p->نشانهها[mark]->یادداشت_3;
}

// Like _PyPegen_insert_memo(), but updates an existing node if found.
صحيح
Py_Pegen_update_یادداشت(Parser *p, صحيح mark, صحيح type, فراغ *گره)
{
    لكل (یادداشت *m = p->نشانهها[mark]->یادداشت_3; m != عدم; m = m->بعدی) {
        إذا (m->type == type) {
            // Update existing node.
            m->گره = گره;
            m->mark = p->mark;
            أرجع 0;
        }
    }
    // Insert new node.
    أرجع Py_Pegen_درج_یادداشت(p, mark, type, گره);
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
Py_Pegen_grow_loop_میانگیر(فراغ ***میانگیر, Py_ssize_t *ظرفیت)
{
    إذا ((اندازه_t)*ظرفیت > (اندازه_t)PY_SSIZE_T_MAX /
            (PEGEN_ARRAY_GROWTH_ضریب * حجم(**میانگیر))) {
        أرجع -1;
    }
    Py_ssize_t new_ظرفیت = *ظرفیت == 0
        ? 1 : *ظرفیت * PEGEN_ARRAY_GROWTH_ضریب;
    فراغ **new_میانگیر = PyMem_Realloc(*میانگیر, new_ظرفیت * حجم(**میانگیر));
    إذا (new_میانگیر == عدم) {
        أرجع -1;
    }
    *میانگیر = new_میانگیر;
    *ظرفیت = new_ظرفیت;
    أرجع 0;
}

ساكن صحيح
growable_comment_array_جمع_کن(growable_comment_array *arr, صحيح lineno, حرف *comment)
{
    إذا (arr->رقم_اقلام >= arr->اندازه) {
        إذا (arr->اندازه > (اندازه_t)PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_ضریب * حجم(*arr->اقلام))) {
            أرجع 0;
        }
        اندازه_t new_اندازه = arr->اندازه == 0
            ? TYPE_IGNORE_INITIAL_ظرفیت
            : arr->اندازه * PEGEN_ARRAY_GROWTH_ضریب;
        فراغ *new_اقلام_array = PyMem_Realloc(
            arr->اقلام, new_اندازه * حجم(*arr->اقلام));
        إذا (!new_اقلام_array) {
            أرجع 0;
        }
        arr->اقلام = new_اقلام_array;
        arr->اندازه = new_اندازه;
    }

    arr->اقلام[arr->رقم_اقلام].lineno = lineno;
    arr->اقلام[arr->رقم_اقلام].comment = comment;  // Take ownership
    arr->رقم_اقلام++;
    أرجع 1;
}

ساكن فراغ
growable_comment_array_deallocate(growable_comment_array *arr)
{
    لكل (اندازه_t i = 0; i < arr->رقم_اقلام; i++) {
        PyMem_Free(arr->اقلام[i].comment);
    }
    PyMem_Free(arr->اقلام);
}

ساكن صحيح
_get_keyword_or_name_type(Parser *p, ثابت حرف *متن, Py_ssize_t درازی)
{
    أكد(درازی > 0);

    إذا (درازی >= p->n_keyword_lists ||
        p->keywords[درازی] == عدم ||
        p->keywords[درازی]->type == -1) {
        أرجع NAME;
    }
    لكل (KeywordToken *k = p->keywords[درازی]; k != عدم && k->type != -1; k++) {
        إذا (memcmp(k->str, متن, (اندازه_t)درازی) == 0) {
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
        حالة FSTRING_شروع:
        حالة FSTRING_میانه:
        حالة FSTRING_انتها:
        حالة TSTRING_شروع:
        حالة TSTRING_میانه:
        حالة TSTRING_انتها:
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
    parser_token->بایتها = عدم;
    إذا (token_needs_متن(token_type) && new_token->بازه.شروع >= 0) {
        Py_ssize_t درازی;
        ثابت حرف *متن = Py_Token_متن_View(p->tok, new_token, &درازی);
        إذا (token_type == NAME) {
            parser_token->type = _get_keyword_or_name_type(p, متن, درازی);
        }
        // A name reclassified as a keyword does not need its text.
        إذا (parser_token->type == token_type) {
            parser_token->بایتها = Py_بایتها_From_String_And_اندازه(متن, درازی);
            إذا (parser_token->بایتها == عدم) {
                أرجع -1;
            }
            إذا (Py_Arena_جمع_کن_Py_Object(p->arena, parser_token->بایتها) < 0) {
                Py_DECREF(parser_token->بایتها);
                أرجع -1;
            }
        }
    }

    parser_token->metadata = عدم;
    إذا (new_token->metadata != عدم) {
        إذا (Py_Arena_جمع_کن_Py_Object(p->arena, new_token->metadata) < 0) {
            أرجع -1;
        }
        parser_token->metadata = new_token->metadata;
        new_token->metadata = عدم;
    }

    parser_token->level = new_token->level;
    parser_token->is_raw = new_token->is_raw;
    parser_token->lineno = new_token->شروع_loc.lineno;
    parser_token->ستون_جابه_جایی = new_token->انتها_loc.lineno == p->starting_lineno
        ? p->starting_ستون_جابه_جایی + new_token->شروع_loc.byte_ستون
        : new_token->شروع_loc.byte_ستون;
    parser_token->انتها_lineno = new_token->انتها_loc.lineno;
    parser_token->انتها_ستون_جابه_جایی = new_token->انتها_loc.lineno == p->starting_lineno
        ? p->starting_ستون_جابه_جایی + new_token->انتها_loc.byte_ستون
        : new_token->انتها_loc.byte_ستون;

    p->fill += 1;

    إذا (token_type == ERRORTOKEN && Py_Tokenizer_Get_وضعیت(p->tok) == E_DECODE) {
        أرجع _Pypegen_raise_decode_error(p);
    }

    أرجع (token_type == ERRORTOKEN ? _Pypegen_tokenizer_error(p) : 0);
}

ساكن صحيح
resize_نشانهها_array(Parser *p) {
    إذا (p->اندازه > بیشترین_صحیح / PEGEN_ARRAY_GROWTH_ضریب ||
            (اندازه_t)p->اندازه > PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_ضریب * حجم(*p->نشانهها))) {
        Py_Err_No_حافظه();
        أرجع -1;
    }
    صحيح newsize = p->اندازه * PEGEN_ARRAY_GROWTH_ضریب;
    Token **new_نشانهها = PyMem_Realloc(p->نشانهها, (اندازه_t)newsize * حجم(Token *));
    إذا (new_نشانهها == عدم) {
        Py_Err_No_حافظه();
        أرجع -1;
    }
    p->نشانهها = new_نشانهها;

    // Allocate new tokens together without moving existing ones: parser rules
    // may still hold pointers into earlier blocks. Parser_Free frees each block.
    Token *chunk = PyMem_Calloc((اندازه_t)(newsize - p->اندازه), حجم(Token));
    إذا (chunk == عدم) {
        Py_Err_No_حافظه();
        أرجع -1;
    }
    لكل (صحيح i = p->اندازه; i < newsize; i++) {
        p->نشانهها[i] = &chunk[i - p->اندازه];
    }
    p->اندازه = newsize;
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
        حرف *برچسب = PyMem_Malloc((اندازه_t)len + 1);
        إذا (برچسب == عدم) {
            Py_Err_No_حافظه();
            اذهب error;
        }
        کپی_حافظه(برچسب, متن, (اندازه_t)len);
        برچسب[len] = '\0';
        // Ownership of tag passes to the growable array
        إذا (!growable_comment_array_جمع_کن(&p->type_ignore_comments, new_token.انتها_loc.lineno, برچسب)) {
            PyMem_Free(برچسب);
            Py_Err_No_حافظه();
            اذهب error;
        }
        _PyTokenizer_Get(p->tok, &new_token);
    }

    صحيح type = new_token.type;
    // EOF can finish an ordinary token. Only a boundary token means that
    // the grammar requested input beyond the last physical line.
    إذا (type == ENDMARKER ||
        (type == DEDENT && (p->flags & Py_PARSE_ALLOW_INCOMPLETE_درون_داد) &&
         Py_Tokenizer_Get_وضعیت(p->tok) == E_EOF)) {
        p->درون_داد_exhausted = 1;
    }

    // If we have reached the end and we are in single input mode we need to insert a newline and reset the parsing
    إذا (p->شروع_قاعده == Py_single_درون_داد && type == ENDMARKER && p->parsing_started) {
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
    إذا ((p->fill == p->اندازه) && (resize_نشانهها_array(p) != 0)) {
        اذهب error;
    }

    Token *t = p->نشانهها[p->fill];
    صحيح نتیجه = initialize_token(p, t, &new_token, type);
    _PyToken_Free(&new_token);
    أرجع نتیجه;
error:
    _PyToken_Free(&new_token);
    أرجع -1;
}

#إذا defined(Py_DEBUG)
// Instrumentation to count the effectiveness of memoization.
// The array counts the number of tokens skipped by memoization,
// indexed by type.

#تعریف_کن NSTATISTICS _PYPEGEN_NSTATISTICS
#تعریف_کن یادداشت_statistics _PyRuntime.parser.یادداشت_statistics

فراغ
Py_Pegen_clear_یادداشت_statistics(فراغ)
{
    Py_Mutex_قفل(&_PyRuntime.parser.mutex);
    لكل (صحيح i = 0; i < NSTATISTICS; i++) {
        یادداشت_statistics[i] = 0;
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
}

PyObject *
Py_Pegen_get_یادداشت_statistics(فراغ)
{
    PyObject *ret = PyList_New(NSTATISTICS);
    إذا (ret == عدم) {
        أرجع عدم;
    }

    Py_Mutex_قفل(&_PyRuntime.parser.mutex);
    لكل (صحيح i = 0; i < NSTATISTICS; i++) {
        PyObject *مقدار = PyLong_FromLong(یادداشت_statistics[i]);
        إذا (مقدار == عدم) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            أرجع عدم;
        }
        // PyList_SetItem borrows a reference to value.
        إذا (Py_List_مجموعه_قلم(ret, i, مقدار) < 0) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            أرجع عدم;
        }
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
    أرجع ret;
}
#پایان_اگر

صحيح  // bool
_PyPegen_is_memoized(Parser *p, صحيح type, فراغ *pres)
{
    إذا (p->mark == p->fill) {
        إذا (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            أرجع -1;
        }
    }

    Token *t = p->نشانهها[p->mark];

    إذا (!(t->یادداشت_ماسک & (1ULL << (type & 63)))) {
        أرجع 0;
    }

    لكل (یادداشت *m = t->یادداشت_3; m != عدم; m = m->بعدی) {
        إذا (m->type == type) {
#إذا defined(Py_DEBUG)
            إذا (0 <= type && type < NSTATISTICS) {
                دراز شمار = m->mark - p->mark;
                // A memoized negative result counts for one.
                إذا (شمار <= 0) {
                    شمار = 1;
                }
                Py_Mutex_قفل(&_PyRuntime.parser.mutex);
                یادداشت_statistics[type] += شمار;
                PyMutex_Unlock(&_PyRuntime.parser.mutex);
            }
#پایان_اگر
            p->mark = m->mark;
            *(فراغ **)(pres) = m->گره;
            أرجع 1;
        }
    }
    أرجع 0;
}

#تعریف_کن LOOKAHEAD1(NAME, RES_TYPE)                                  \
    صحيح                                                             \
    NAME (صحيح مثبت, RES_TYPE (func)(Parser *), Parser *p)       \
    {                                                               \
        صحيح mark = p->mark;                                         \
        فراغ *res = func(p);                                        \
        p->mark = mark;                                             \
        أرجع (res != عدم) == مثبت;                           \
    }

LOOKAHEAD1(_PyPegen_lookahead, فراغ *)
LOOKAHEAD1(Py_Pegen_lookahead_for_عبارت, عبارت_ty)
LOOKAHEAD1(_PyPegen_lookahead_for_stmt, stmt_ty)
#لغو_تعریف LOOKAHEAD1

#تعریف_کن LOOKAHEAD2(NAME, RES_TYPE, T)                                   \
    صحيح                                                                 \
    NAME (صحيح مثبت, RES_TYPE (func)(Parser *, T), Parser *p, T arg) \
    {                                                                   \
        صحيح mark = p->mark;                                             \
        فراغ *res = func(p, arg);                                       \
        p->mark = mark;                                                 \
        أرجع (res != عدم) == مثبت;                               \
    }

LOOKAHEAD2(_PyPegen_lookahead_with_int, Token *, صحيح)
LOOKAHEAD2(_PyPegen_lookahead_with_string, عبارت_ty, ثابت حرف *)
#لغو_تعریف LOOKAHEAD2

Token *
_PyPegen_expect_token(Parser *p, صحيح type)
{
    إذا (p->mark == p->fill) {
        إذا (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            أرجع عدم;
        }
    }
    Token *t = p->نشانهها[p->mark];
    إذا (t->type != type) {
       أرجع عدم;
    }
    p->mark += 1;
    أرجع t;
}

فراغ*
Py_Pegen_expect_forced_نتیجه(Parser *p, فراغ* نتیجه, ثابت حرف* expected) {

    إذا (p->error_indicator == 1) {
        أرجع عدم;
    }
    إذا (نتیجه == عدم) {
        RAISE_SYNTAX_ERROR("expected (%s)", expected);
        أرجع عدم;
    }
    أرجع نتیجه;
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
    Token *t = p->نشانهها[p->mark];
    إذا (t->type != type) {
        RAISE_SYNTAX_ERROR_معلوم_LOCATION(t, "expected '%s'", expected);
        أرجع عدم;
    }
    p->mark += 1;
    أرجع t;
}

عبارت_ty
_PyPegen_expect_soft_keyword(Parser *p, ثابت حرف *keyword)
{
    إذا (p->mark == p->fill) {
        إذا (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            أرجع عدم;
        }
    }
    Token *t = p->نشانهها[p->mark];
    إذا (t->type != NAME) {
        أرجع عدم;
    }
    ثابت حرف *s = Py_بایتها_As_String(t->بایتها);
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
Py_Pegen_get_آخرین_nonnwhitespace_token(Parser *p)
{
    أكد(p->mark >= 0);
    Token *token = عدم;
    لكل (صحيح m = p->mark - 1; m >= 0; m--) {
        token = p->نشانهها[m];
        إذا (token->type != ENDMARKER && (token->type < NEWLINE || token->type > DEDENT)) {
            اكسر;
        }
    }
    أرجع token;
}

PyObject *
_PyPegen_new_identifier(Parser *p, ثابت حرف *n)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, (Py_ssize_t)طول_رشته(n), عدم);
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
        PyObject *آرگومانها[2] = {form, id};
        PyObject *id2 = PyObject_Vectorcall(p->normalize, آرگومانها, 2, عدم);
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
        إذا (Py_Unicode_Equal_مقصد_ASCIIString(id, forbidden[i])) {
            PyErr_Format(Py_Exc_مقدار_Error,
                         "identifier field can't represent '%s' constant",
                         forbidden[i]);
            Py_DECREF(id);
            اذهب error;
        }
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &id);
    إذا (Py_Arena_جمع_کن_Py_Object(p->arena, id) < 0)
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
get_cached_identifier(Parser *p, PyObject *بایتها)
{
    ثابت حرف *s = Py_بایتها_As_String(بایتها);
    إذا (!s) {
        p->error_indicator = 1;
        أرجع عدم;
    }
    // Identifiers repeat constantly; a small span-keyed cache skips the
    // UTF-8 decode + intern for repeated occurrences. Keys point into
    // arena-owned token bytes and values are arena-owned interned strings,
    // so borrowed references are valid for the lifetime of the parse
    // (including the second error pass, which reuses parser and arena).
    Py_ssize_t len = Py_بایتها_GET_اندازه(بایتها);
    Py_hash_t hash = PyObject_Hash(بایتها);
    إذا (hash == -1) {
        p->error_indicator = 1;
        أرجع عدم;
    }
    // Parses without identifiers do not need this cache.
    إذا (p->identifier_cache == عدم) {
        p->identifier_cache = PyMem_Calloc(
            IDENTIFIER_CACHE_اندازه, حجم(*p->identifier_cache));
        إذا (p->identifier_cache == عدم) {
            p->error_indicator = 1;
            أرجع Py_Err_No_حافظه();
        }
    }
    Identifier_Cache_مدخل *free_جایگاه = عدم;
    اندازه_t idx = (اندازه_t)hash & (IDENTIFIER_CACHE_اندازه - 1);
    لكل (صحيح probe = 0; probe < IDENTIFIER_CACHE_MAX_PROBES; probe++) {
        Identifier_Cache_مدخل *مدخل = &p->identifier_cache[
            (idx + probe) & (IDENTIFIER_CACHE_اندازه - 1)];
        إذا (مدخل->کلید == عدم) {
            free_جایگاه = مدخل;
            اكسر;
        }
        إذا (مدخل->hash == hash && مدخل->len == len &&
            memcmp(مدخل->کلید, s, len) == 0)
        {
            أرجع مدخل->مقدار;
        }
    }
    PyObject *id = _PyPegen_new_identifier(p, s);
    إذا (id == عدم) {
        p->error_indicator = 1;
        أرجع عدم;
    }
    إذا (free_جایگاه != عدم) {
        free_جایگاه->کلید = s;
        free_جایگاه->len = len;
        free_جایگاه->hash = hash;
        free_جایگاه->مقدار = id;
    }
    أرجع id;
}

ساكن عبارت_ty
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
    عبارت_ty cached = عدم;
    إذا (_PyPegen_is_memoized(p, NAME, &cached)) {
        أرجع cached;
    }
    p->mark = mark + 1;
    PyObject *id = get_cached_identifier(p, t->بایتها);
    إذا (id == عدم) {
        أرجع عدم;
    }
    عبارت_ty نتیجه = _PyAST_Name(id, بارگیری, t->lineno, t->ستون_جابه_جایی,
                                t->انتها_lineno, t->انتها_ستون_جابه_جایی, p->arena);
    إذا (نتیجه != عدم && Py_Pegen_درج_یادداشت(p, mark, NAME, نتیجه) < 0) {
        p->error_indicator = 1;
        أرجع عدم;
    }
    أرجع نتیجه;
}

عبارت_ty
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

عبارت_ty _PyPegen_soft_keyword_token(Parser *p) {
    Token *t = _PyPegen_expect_token(p, NAME);
    إذا (t == عدم) {
        أرجع عدم;
    }
    حرف *the_token;
    Py_ssize_t اندازه;
    Py_بایتها_As_String_And_اندازه(t->بایتها, &the_token, &اندازه);
    لكل (حرف **keyword = p->soft_keywords; *keyword != عدم; keyword++) {
        إذا (طول_رشته(*keyword) == (اندازه_t)اندازه &&
            مقایسه_رشته(*keyword, the_token, (اندازه_t)اندازه) == 0) {
            أرجع _PyPegen_name_from_token(p, t);
        }
    }
    أرجع عدم;
}

ساكن PyObject *
parsenumber_raw(ثابت حرف *s)
{
    ثابت حرف *انتها;
    دراز x;
    مزدوج dx;
    Py_complex compl;
    صحيح imflag;

    أكد(s != عدم);
    errno = 0;
    انتها = s + طول_رشته(s) - 1;
    imflag = *انتها == 'j' || *انتها == 'J';
    إذا (s[0] == '0') {
        x = (دراز)PyOS_strtoul(s, (حرف **)&انتها, 0);
        إذا (x < 0 && errno == 0) {
            أرجع PyLong_FromString(s, (حرف **)0, 0);
        }
    }
    وإلا {
        x = PyOS_strtol(s, (حرف **)&انتها, 0);
    }
    إذا (*انتها == '\0') {
        إذا (errno != 0) {
            أرجع PyLong_FromString(s, (حرف **)0, 0);
        }
        أرجع PyLong_FromLong(x);
    }
    /* XXX Huge floats may silently fail */
    إذا (imflag) {
        compl.real = 0.;
        compl.imag = Py_OS_string_مقصد_double(s, (حرف **)&انتها, عدم);
        إذا (compl.imag == -1.0 && PyErr_Occurred()) {
            أرجع عدم;
        }
        أرجع PyComplex_FromCComplex(compl);
    }
    dx = Py_OS_string_مقصد_double(s, عدم, عدم);
    إذا (dx == -1.0 && PyErr_Occurred()) {
        أرجع عدم;
    }
    أرجع PyFloat_FromDouble(dx);
}

ساكن PyObject *
parsenumber(ثابت حرف *s)
{
    حرف *dup;
    حرف *انتها;
    PyObject *res = عدم;

    أكد(s != عدم);

    إذا (strchr(s, '_') == عدم) {
        أرجع parsenumber_raw(s);
    }
    /* Create a duplicate without underscores. */
    dup = PyMem_Malloc(طول_رشته(s) + 1);
    إذا (dup == عدم) {
        أرجع Py_Err_No_حافظه();
    }
    انتها = dup;
    لكل (; *s; s++) {
        إذا (*s != '_') {
            *انتها++ = *s;
        }
    }
    *انتها = '\0';
    res = parsenumber_raw(dup);
    PyMem_Free(dup);
    أرجع res;
}

عبارت_ty
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
    عبارت_ty cached = عدم;
    إذا (_PyPegen_is_memoized(p, عدد, &cached)) {
        أرجع cached;
    }
    p->mark = mark + 1;

    ثابت حرف *رقم_raw = Py_بایتها_As_String(t->بایتها);
    إذا (رقم_raw == عدم) {
        p->error_indicator = 1;
        أرجع عدم;
    }

    إذا (p->feature_نسخه < 6 && strchr(رقم_raw, '_') != عدم) {
        p->error_indicator = 1;
        أرجع RAISE_SYNTAX_ERROR("Underscores in numeric literals are only supported "
                                  "in Python 3.6 and greater");
    }

    PyObject *c = parsenumber(رقم_raw);

    إذا (c == عدم) {
        p->error_indicator = 1;
        PyThreadState *tstate = _PyThreadState_GET();
        // The only way a ValueError should happen in _this_ code is via
        // PyLong_FromString hitting a length limit.
        إذا (tstate->current_exception != عدم &&
            Py_TYPE(tstate->current_exception) == (PyTypeObject *)Py_Exc_مقدار_Error
        ) {
            PyObject *exc = PyErr_GetRaisedException();
            /* Intentionally omitting columns to avoid a wall of 1000s of '^'s
             * on the error message. Nobody is going to overlook their huge
             * numeric literal once given the line. */
            RAISE_ERROR_معلوم_LOCATION(
                p, PyExc_SyntaxError,
                t->lineno, -1 /* col_offset */,
                t->انتها_lineno, -1 /* end_col_offset */,
                "%S - Consider hexadecimal for huge integer literals "
                "to avoid decimal conversion limits.",
                exc);
            Py_DECREF(exc);
        }
        أرجع عدم;
    }

    إذا (Py_Arena_جمع_کن_Py_Object(p->arena, c) < 0) {
        Py_DECREF(c);
        p->error_indicator = 1;
        أرجع عدم;
    }

    عبارت_ty نتیجه = _PyAST_Constant(c, عدم, t->lineno, t->ستون_جابه_جایی,
                                    t->انتها_lineno, t->انتها_ستون_جابه_جایی, p->arena);
    إذا (نتیجه != عدم && Py_Pegen_درج_یادداشت(p, mark, عدد, نتیجه) < 0) {
        p->error_indicator = 1;
        أرجع عدم;
    }
    أرجع نتیجه;
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
    إذا (flags->cf_flags & Py_CF_ALLOW_INCOMPLETE_درون_داد) {
        parser_flags |= Py_PARSE_ALLOW_INCOMPLETE_درون_داد;
    }
    أرجع parser_flags;
}

// Parser API

Parser *
_PyPegen_Parser_New(بنية tok_state *tok, صحيح شروع_قاعده, صحيح flags,
                    صحيح feature_نسخه, صحيح *errcode, ثابت حرف* منبع, PyArena *arena)
{
    Parser *p = PyMem_Malloc(حجم(Parser));
    إذا (p == عدم) {
        أرجع (Parser *) Py_Err_No_حافظه();
    }
    أكد(tok != عدم);
    Py_Tokenizer_مجموعه_Options(tok, 0, (flags & PyPARSE_TYPE_COMMENTS) > 0);
    Py_Tokenizer_Allow_Incomplete_درون_داد(tok, flags & Py_PARSE_ALLOW_INCOMPLETE_درون_داد);
    p->tok = tok;
    p->keywords = عدم;
    p->n_keyword_lists = -1;
    p->soft_keywords = عدم;
    p->نشانهها = PyMem_Malloc(حجم(Token *));
    إذا (!p->نشانهها) {
        PyMem_Free(p);
        أرجع (Parser *) Py_Err_No_حافظه();
    }
    p->نشانهها[0] = PyMem_Calloc(1, حجم(Token));
    إذا (!p->نشانهها[0]) {
        PyMem_Free(p->نشانهها);
        PyMem_Free(p);
        أرجع (Parser *) Py_Err_No_حافظه();
    }
    p->type_ignore_comments = (growable_comment_array){0};

    p->mark = 0;
    p->fill = 0;
    p->اندازه = 1;

    p->errcode = errcode;
    p->arena = arena;
    p->شروع_قاعده = شروع_قاعده;
    p->parsing_started = 0;
    p->normalize = عدم;
    p->error_indicator = 0;

    p->starting_lineno = 0;
    p->starting_ستون_جابه_جایی = 0;
    p->flags = flags;
    p->feature_نسخه = feature_نسخه;
    p->معلوم_err_token = عدم;
    p->identifier_cache = عدم;
    p->tstate = PyThreadState_Get();
    // Stack limits are initialized when the thread state is attached.
    أكد(((_PyThreadStateImpl *)p->tstate)->c_پشته_hard_حد != 0);
    p->پشته_soft_حد = ((_PyThreadStateImpl *)p->tstate)->c_پشته_soft_حد;
#إذا Py_پشته_GROWS_پایین
    p->پشته_soft_حد += Py_OS_پشته_MARGIN_بایتها;
#وإلا
    p->پشته_soft_حد -= Py_OS_پشته_MARGIN_بایتها;
#پایان_اگر
    p->level = 0;
    p->call_invalid_قواعد = 0;
    p->درون_داد_exhausted = 0;
    p->آخرین_stmt_location.lineno = 0;
    p->آخرین_stmt_location.ستون_جابه_جایی = 0;
    p->آخرین_stmt_location.انتها_lineno = 0;
    p->آخرین_stmt_location.انتها_ستون_جابه_جایی = 0;
#اگر_تعریف_شده Py_DEBUG
    p->debug = _Py_GetConfig()->parser_debug;
#پایان_اگر
    أرجع p;
}

فراغ
_PyPegen_Parser_Free(Parser *p)
{
    PyMem_Free(p->identifier_cache);
    Py_XDECREF(p->normalize);
    // Resizes allocate blocks starting at indices 1, 2, 4, and so on.
    PyMem_Free(p->نشانهها[0]);
    لكل (صحيح i = 1; i < p->اندازه; i *= PEGEN_ARRAY_GROWTH_ضریب) {
        PyMem_Free(p->نشانهها[i]);
    }
    PyMem_Free(p->نشانهها);
    growable_comment_array_deallocate(&p->type_ignore_comments);
    PyMem_Free(p);
}

ساكن فراغ
reset_parser_state_for_error_pass(Parser *p)
{
    p->آخرین_stmt_location.lineno = 0;
    p->آخرین_stmt_location.ستون_جابه_جایی = 0;
    p->آخرین_stmt_location.انتها_lineno = 0;
    p->آخرین_stmt_location.انتها_ستون_جابه_جایی = 0;
    لكل (صحيح i = 0; i < p->fill; i++) {
        p->نشانهها[i]->یادداشت_3 = عدم;
        p->نشانهها[i]->یادداشت_ماسک = 0;
    }
    p->mark = 0;
    p->flags &= ~Py_PARSE_ALLOW_INCOMPLETE_درون_داد;
    Py_Tokenizer_Allow_Incomplete_درون_داد(p->tok, 0);
    p->call_invalid_قواعد = 1;
    _PyTokenizer_StopInteractive(p->tok);
}

ساكن فراغ
Py_Pegen_مجموعه_syntax_error_metadata(Parser *p) {
    PyObject *exc = PyErr_GetRaisedException();
    إذا (!exc || !PyObject_TypeCheck(exc, (PyTypeObject *)PyExc_SyntaxError)) {
        Py_Err_مجموعه_Raised_Exception(exc);
        أرجع;
    }
    ثابت حرف *منبع = Py_Tokenizer_Retained_منبع(p->tok);
    ثابت حرف *encoding = _PyTokenizer_GetInfo(p->tok).encoding;
    PyObject* the_منبع = عدم;
    إذا (منبع) {
        إذا (encoding == عدم) {
            the_منبع = PyUnicode_FromString(منبع);
        } وإلا {
            the_منبع = PyUnicode_Decode(منبع, طول_رشته(منبع), encoding, عدم);
        }
    }
    إذا (!the_منبع) {
        PyErr_Clear();
        the_منبع = Py_None;
        Py_INCREF(the_منبع);
    }
    PyObject* metadata = Py_Build_مقدار(
        "(iiN)",
        p->آخرین_stmt_location.lineno,
        p->آخرین_stmt_location.ستون_جابه_جایی,
        the_منبع // N gives ownership to metadata
    );
    إذا (!metadata) {
        PyErr_Clear();
        Py_Err_مجموعه_Raised_Exception(exc);
        أرجع;
    }
    PySyntaxErrorObject *syntax_error = (PySyntaxErrorObject *)exc;

    Py_XDECREF(syntax_error->metadata);
    syntax_error->metadata = metadata;
    Py_Err_مجموعه_Raised_Exception(exc);
}

فراغ *
Py_Pegen_پیاپی_parser(Parser *p)
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
            (PyErr_ExceptionMatches(Py_Exc_Incomplete_درون_داد_Error) ||
             !PyErr_ExceptionMatches(PyExc_SyntaxError))) {
            أرجع عدم;
        }
        إذا ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_درون_داد) &&
            p->درون_داد_exhausted && !PyErr_Occurred()) {
            أرجع Py_Pegen_raise_incomplete_درون_داد(p);
        }
       // Make a second parser pass. In this pass we activate heavier and slower checks
        // to produce better error messages and more complete diagnostics. Extra "invalid_*"
        // rules will be active during parsing.
        Token *آخرین_token = p->نشانهها[p->fill - 1];
        reset_parser_state_for_error_pass(p);
        _PyPegen_parse(p);

        // Set SyntaxErrors accordingly depending on the parser/tokenizer status at the failure
        // point.
        Pypegen_مجموعه_syntax_error(p, آخرین_token);

        // Set the metadata in the exception from p->last_stmt_location
        إذا (PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            Py_Pegen_مجموعه_syntax_error_metadata(p);
        }
       أرجع عدم;
    }

    إذا (p->شروع_قاعده == Py_single_درون_داد && _PyTokenizer_HasTrailingStatement(p->tok)) {
        أرجع RAISE_SYNTAX_ERROR("multiple statements found while compiling a single statement");
    }

    // test_peg_generator defines _Py_TEST_PEGEN to not call PyAST_Validate()
#إذا defined(Py_DEBUG) && !defined(_Py_TEST_PEGEN)
    إذا (p->شروع_قاعده == Py_single_درون_داد ||
        p->شروع_قاعده == Py_file_درون_داد ||
        p->شروع_قاعده == Py_eval_درون_داد)
    {
        إذا (!_PyAST_Validate(res)) {
            أرجع عدم;
        }
    }
#پایان_اگر
    أرجع res;
}

mod_ty
Py_Pegen_پیاپی_parser_from_file_اشاره_گر(FILE *fp, صحيح شروع_قاعده, PyObject *filename_ob,
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
            Py_Err_No_حافظه();
        }
        أرجع عدم;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    // From here on we need to clean up even if there's an error
    mod_ty نتیجه = عدم;

    PyObject *module = PyUnicode_FromString("__main__");
    إذا (module == عدم) {
        اذهب error;
    }
    Py_Tokenizer_مجموعه_Context(tok, filename_ob, module);
    Py_DECREF(module);

    Parser *p = _PyPegen_Parser_New(tok, شروع_قاعده, parser_flags, PY_MINOR_نسخه,
                                    errcode, عدم, arena);
    إذا (p == عدم) {
        اذهب error;
    }

    نتیجه = Py_Pegen_پیاپی_parser(p);
    _PyPegen_Parser_Free(p);

    ثابت حرف *منبع = Py_Tokenizer_Retained_منبع(tok);
    إذا (منبع != عدم && نتیجه && interactive_src != عدم) {
        *interactive_src = PyUnicode_FromString(منبع);
        إذا (!*interactive_src || Py_Arena_جمع_کن_Py_Object(arena, *interactive_src) < 0) {
            Py_XDECREF(*interactive_src);
            نتیجه = عدم;
            اذهب error;
        }
    }

error:
    _PyTokenizer_Free(tok);
    أرجع نتیجه;
}

mod_ty
Py_Pegen_پیاپی_parser_from_string(ثابت حرف *str, صحيح شروع_قاعده, PyObject *filename_ob,
                       PyCompilerFlags *flags, PyArena *arena, PyObject *module,
                       PyObject **incomplete)
{
    _PyTokenizer_Newline newline_نما = شروع_قاعده == Py_file_درون_داد
        ? _PyTokenizer_IMPLY_NEWLINE : _PyTokenizer_KEEP_NEWLINE;
    صحيح parser_flags = compute_parser_flags(flags);
    إذا (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_درون_داد) {
        newline_نما = _PyTokenizer_APPEND_NEWLINE;
    }

    بنية tok_state *tok;
    إذا (flags != عدم && flags->cf_flags & PyCF_IGNORE_COOKIE) {
        tok = _PyTokenizer_FromUTF8(str, newline_نما, 0);
    } وإلا {
        tok = _PyTokenizer_FromString(str, newline_نما, 0);
    }
    إذا (tok == عدم) {
        إذا (PyErr_Occurred()) {
            _PyTokenizer_raise_init_error(filename_ob);
        }
        وإلا {
            // The only silent tokenizer init failure is a failed allocation.
            Py_Err_No_حافظه();
        }
        أرجع عدم;
    }
    tok->barry_as_bdfl = parser_flags & PyPARSE_BARRY_AS_BDFL;

    Py_Tokenizer_مجموعه_Context(tok, filename_ob, module);

    // We need to clear up from here on
    mod_ty نتیجه = عدم;

    صحيح feature_نسخه = flags && (flags->cf_flags & PyCF_ONLY_AST) ?
        flags->cf_feature_نسخه : PY_MINOR_نسخه;
    Parser *p = _PyPegen_Parser_New(tok, شروع_قاعده, parser_flags, feature_نسخه,
                                    عدم, str, arena);
    إذا (p == عدم) {
        اذهب error;
    }

    نتیجه = Py_Pegen_پیاپی_parser(p);
    إذا (نتیجه != عدم && (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_درون_داد)) {
        صحيح needs_newline = _PyTokenizer_GetNewlineRequirements(tok);
        إذا ((needs_newline & _PyTokenizer_NEWLINE_CONTINUATION) ||
            (شروع_قاعده != Py_file_درون_داد &&
             (needs_newline & _PyTokenizer_NEWLINE_INDENT)) ||
            (شروع_قاعده == Py_single_درون_داد && p->درون_داد_exhausted &&
             (needs_newline & _PyTokenizer_NEWLINE_STATEMENT))) {
            Py_Pegen_raise_incomplete_درون_داد(p);
            إذا (incomplete != عدم && PyErr_ExceptionMatches(Py_Exc_Incomplete_درون_داد_Error)) {
                *incomplete = PyErr_GetRaisedException();
            }
            وإلا {
                نتیجه = عدم;
            }
        }
    }
    _PyPegen_Parser_Free(p);

error:
    _PyTokenizer_Free(tok);
    أرجع نتیجه;
}
