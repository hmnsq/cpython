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

#تعریف_کړه IDENTIFIER_CACHE_اندازه 2048  // Must be a power of two.
#تعریف_کړه IDENTIFIER_CACHE_MAX_PROBES 8
#تعریف_کړه PEGEN_ARRAY_GROWTH_ضریب 2
#تعریف_کړه TYPE_IGNORE_INITIAL_ظرفیت 10

بنية identifier_cache_ثبت {
    ثابت حرف *کلی;   // Borrowed from arena-owned token bytes.
    Py_ssize_t len;
    Py_hash_t hash;
    PyObject *ارزښت;   // Borrowed from an arena-owned identifier.
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
Py_Pegen_byte_دننه_واټن_منزل_character_دننه_واټن_کرښه(PyObject *کرښه, Py_ssize_t ستون_دننه_واټن, Py_ssize_t پای_ټکی_ستون_دننه_واټن)
{
    ثابت بینښه حرف *معلومات = (ثابت بینښه حرف*)PyUnicode_AsUTF8(کرښه);
    إذا (معلومات == عدم) {
        أرجع -1;
    }

    Py_ssize_t len = 0;
    طالما (ستون_دننه_واټن < پای_ټکی_ستون_دننه_واټن) {
        Py_UCS4 ch = معلومات[ستون_دننه_واټن];
        إذا (ch < 0x80) {
            ستون_دننه_واټن += 1;
        } وإلا إذا ((ch & 0xe0) == 0xc0) {
            ستون_دننه_واټن += 2;
        } وإلا إذا ((ch & 0xf0) == 0xe0) {
            ستون_دننه_واټن += 3;
        } وإلا إذا ((ch & 0xf8) == 0xf0) {
            ستون_دننه_واټن += 4;
        } وإلا {
            Py_Err_سېټ_String(Py_Exc_ارزښت_Error, "Invalid UTF-8 sequence");
            أرجع -1;
        }
        len++;
    }
    أرجع len;
}

Py_ssize_t
Py_Pegen_byte_دننه_واټن_منزل_character_دننه_واټن_raw(ثابت حرف* str, Py_ssize_t ستون_دننه_واټن)
{
    Py_ssize_t len = (Py_ssize_t)مزي_اوږدوالی(str);
    إذا (ستون_دننه_واټن > len + 1) {
        ستون_دننه_واټن = len + 1;
    }
    أكد(ستون_دننه_واټن >= 0);
    PyObject *متن = PyUnicode_DecodeUTF8(str, ستون_دننه_واټن, "replace");
    إذا (!متن) {
        أرجع -1;
    }
    Py_ssize_t اندازه = Py_Unicode_GET_اوږدتیا(متن);
    Py_DECREF(متن);
    أرجع اندازه;
}

Py_ssize_t
Py_Pegen_byte_دننه_واټن_منزل_character_دننه_واټن(PyObject *کرښه, Py_ssize_t ستون_دننه_واټن)
{
    ثابت حرف *str = PyUnicode_AsUTF8(کرښه);
    إذا (!str) {
        أرجع -1;
    }
    أرجع Py_Pegen_byte_دننه_واټن_منزل_character_دننه_واټن_raw(str, ستون_دننه_واټن);
}

// Here, mark is the start of the node, while p->mark is the end.
// If node==NULL, they should be the same.
صحيح
Py_Pegen_ورننباسه_یادښت(Parser *p, صحيح mark, صحيح type, فراغ *غوټه)
{
    // Insert in front
    یادښت *m = _PyArena_Malloc(p->arena, حجم(یادښت));
    إذا (m == عدم) {
        أرجع -1;
    }
    m->type = type;
    m->غوټه = غوټه;
    m->mark = p->mark;
    m->بل = p->نښې[mark]->یادښت_3;
    p->نښې[mark]->یادښت_3 = m;
    p->نښې[mark]->یادښت_ماسک |= 1ULL << (type & 63);
    أرجع 0;
}

// Like _PyPegen_insert_memo(), but returns the inserted Memo so callers
// can update it in place without re-walking the token's memo list.
یادښت *
Py_Pegen_ورننباسه_یادښت_مستقیم(Parser *p, صحيح mark, صحيح type)
{
    إذا (Py_Pegen_ورننباسه_یادښت(p, mark, type, عدم) < 0) {
        أرجع عدم;
    }
    أرجع p->نښې[mark]->یادښت_3;
}

// Like _PyPegen_insert_memo(), but updates an existing node if found.
صحيح
Py_Pegen_update_یادښت(Parser *p, صحيح mark, صحيح type, فراغ *غوټه)
{
    لكل (یادښت *m = p->نښې[mark]->یادښت_3; m != عدم; m = m->بل) {
        إذا (m->type == type) {
            // Update existing node.
            m->غوټه = غوټه;
            m->mark = p->mark;
            أرجع 0;
        }
    }
    // Insert new node.
    أرجع Py_Pegen_ورننباسه_یادښت(p, mark, type, غوټه);
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
Py_Pegen_grow_loop_بفر(فراغ ***بفر, Py_ssize_t *ظرفیت)
{
    إذا ((اندازه_t)*ظرفیت > (اندازه_t)PY_SSIZE_T_MAX /
            (PEGEN_ARRAY_GROWTH_ضریب * حجم(**بفر))) {
        أرجع -1;
    }
    Py_ssize_t new_ظرفیت = *ظرفیت == 0
        ? 1 : *ظرفیت * PEGEN_ARRAY_GROWTH_ضریب;
    فراغ **new_بفر = PyMem_Realloc(*بفر, new_ظرفیت * حجم(**بفر));
    إذا (new_بفر == عدم) {
        أرجع -1;
    }
    *بفر = new_بفر;
    *ظرفیت = new_ظرفیت;
    أرجع 0;
}

ساكن صحيح
growable_comment_array_جمع_کړه(growable_comment_array *arr, صحيح lineno, حرف *comment)
{
    إذا (arr->عدد_توکي >= arr->اندازه) {
        إذا (arr->اندازه > (اندازه_t)PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_ضریب * حجم(*arr->توکي))) {
            أرجع 0;
        }
        اندازه_t new_اندازه = arr->اندازه == 0
            ? TYPE_IGNORE_INITIAL_ظرفیت
            : arr->اندازه * PEGEN_ARRAY_GROWTH_ضریب;
        فراغ *new_توکي_array = PyMem_Realloc(
            arr->توکي, new_اندازه * حجم(*arr->توکي));
        إذا (!new_توکي_array) {
            أرجع 0;
        }
        arr->توکي = new_توکي_array;
        arr->اندازه = new_اندازه;
    }

    arr->توکي[arr->عدد_توکي].lineno = lineno;
    arr->توکي[arr->عدد_توکي].comment = comment;  // Take ownership
    arr->عدد_توکي++;
    أرجع 1;
}

ساكن فراغ
growable_comment_array_deallocate(growable_comment_array *arr)
{
    لكل (اندازه_t i = 0; i < arr->عدد_توکي; i++) {
        PyMem_Free(arr->توکي[i].comment);
    }
    PyMem_Free(arr->توکي);
}

ساكن صحيح
_get_keyword_or_name_type(Parser *p, ثابت حرف *متن, Py_ssize_t اوږدتیا)
{
    أكد(اوږدتیا > 0);

    إذا (اوږدتیا >= p->n_keyword_lists ||
        p->keywords[اوږدتیا] == عدم ||
        p->keywords[اوږدتیا]->type == -1) {
        أرجع NAME;
    }
    لكل (KeywordToken *k = p->keywords[اوږدتیا]; k != عدم && k->type != -1; k++) {
        إذا (memcmp(k->str, متن, (اندازه_t)اوږدتیا) == 0) {
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
        حالة شمېره:
        حالة STRING:
        حالة FSTRING_پیل:
        حالة FSTRING_منځ:
        حالة FSTRING_پای_ټکی:
        حالة TSTRING_پیل:
        حالة TSTRING_منځ:
        حالة TSTRING_پای_ټکی:
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
    parser_token->بایټونه = عدم;
    إذا (token_needs_متن(token_type) && new_token->پراختیا.پیل >= 0) {
        Py_ssize_t اوږدتیا;
        ثابت حرف *متن = Py_Token_متن_View(p->tok, new_token, &اوږدتیا);
        إذا (token_type == NAME) {
            parser_token->type = _get_keyword_or_name_type(p, متن, اوږدتیا);
        }
        // A name reclassified as a keyword does not need its text.
        إذا (parser_token->type == token_type) {
            parser_token->بایټونه = Py_بایټونه_From_String_And_اندازه(متن, اوږدتیا);
            إذا (parser_token->بایټونه == عدم) {
                أرجع -1;
            }
            إذا (Py_Arena_جمع_کړه_Py_Object(p->arena, parser_token->بایټونه) < 0) {
                Py_DECREF(parser_token->بایټونه);
                أرجع -1;
            }
        }
    }

    parser_token->metadata = عدم;
    إذا (new_token->metadata != عدم) {
        إذا (Py_Arena_جمع_کړه_Py_Object(p->arena, new_token->metadata) < 0) {
            أرجع -1;
        }
        parser_token->metadata = new_token->metadata;
        new_token->metadata = عدم;
    }

    parser_token->level = new_token->level;
    parser_token->is_raw = new_token->is_raw;
    parser_token->lineno = new_token->پیل_loc.lineno;
    parser_token->ستون_دننه_واټن = new_token->پای_ټکی_loc.lineno == p->starting_lineno
        ? p->starting_ستون_دننه_واټن + new_token->پیل_loc.byte_ستون
        : new_token->پیل_loc.byte_ستون;
    parser_token->پای_ټکی_lineno = new_token->پای_ټکی_loc.lineno;
    parser_token->پای_ټکی_ستون_دننه_واټن = new_token->پای_ټکی_loc.lineno == p->starting_lineno
        ? p->starting_ستون_دننه_واټن + new_token->پای_ټکی_loc.byte_ستون
        : new_token->پای_ټکی_loc.byte_ستون;

    p->fill += 1;

    إذا (token_type == ERRORTOKEN && Py_Tokenizer_Get_حالت(p->tok) == E_DECODE) {
        أرجع _Pypegen_raise_decode_error(p);
    }

    أرجع (token_type == ERRORTOKEN ? _Pypegen_tokenizer_error(p) : 0);
}

ساكن صحيح
resize_نښې_array(Parser *p) {
    إذا (p->اندازه > لوی_عدد / PEGEN_ARRAY_GROWTH_ضریب ||
            (اندازه_t)p->اندازه > PY_SSIZE_T_MAX /
                (PEGEN_ARRAY_GROWTH_ضریب * حجم(*p->نښې))) {
        Py_Err_No_حافظه();
        أرجع -1;
    }
    صحيح newsize = p->اندازه * PEGEN_ARRAY_GROWTH_ضریب;
    Token **new_نښې = PyMem_Realloc(p->نښې, (اندازه_t)newsize * حجم(Token *));
    إذا (new_نښې == عدم) {
        Py_Err_No_حافظه();
        أرجع -1;
    }
    p->نښې = new_نښې;

    // Allocate new tokens together without moving existing ones: parser rules
    // may still hold pointers into earlier blocks. Parser_Free frees each block.
    Token *chunk = PyMem_Calloc((اندازه_t)(newsize - p->اندازه), حجم(Token));
    إذا (chunk == عدم) {
        Py_Err_No_حافظه();
        أرجع -1;
    }
    لكل (صحيح i = p->اندازه; i < newsize; i++) {
        p->نښې[i] = &chunk[i - p->اندازه];
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
        حرف *نښان = PyMem_Malloc((اندازه_t)len + 1);
        إذا (نښان == عدم) {
            Py_Err_No_حافظه();
            اذهب error;
        }
        حافظه_نقل(نښان, متن, (اندازه_t)len);
        نښان[len] = '\0';
        // Ownership of tag passes to the growable array
        إذا (!growable_comment_array_جمع_کړه(&p->type_ignore_comments, new_token.پای_ټکی_loc.lineno, نښان)) {
            PyMem_Free(نښان);
            Py_Err_No_حافظه();
            اذهب error;
        }
        _PyTokenizer_Get(p->tok, &new_token);
    }

    صحيح type = new_token.type;
    // EOF can finish an ordinary token. Only a boundary token means that
    // the grammar requested input beyond the last physical line.
    إذا (type == ENDMARKER ||
        (type == DEDENT && (p->flags & Py_PARSE_ALLOW_INCOMPLETE_ننوت) &&
         Py_Tokenizer_Get_حالت(p->tok) == E_EOF)) {
        p->ننوت_exhausted = 1;
    }

    // If we have reached the end and we are in single input mode we need to insert a newline and reset the parsing
    إذا (p->پیل_قاعده == Py_single_ننوت && type == ENDMARKER && p->parsing_started) {
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
    إذا ((p->fill == p->اندازه) && (resize_نښې_array(p) != 0)) {
        اذهب error;
    }

    Token *t = p->نښې[p->fill];
    صحيح پایله = initialize_token(p, t, &new_token, type);
    _PyToken_Free(&new_token);
    أرجع پایله;
error:
    _PyToken_Free(&new_token);
    أرجع -1;
}

#إذا defined(Py_DEBUG)
// Instrumentation to count the effectiveness of memoization.
// The array counts the number of tokens skipped by memoization,
// indexed by type.

#تعریف_کړه NSTATISTICS _PYPEGEN_NSTATISTICS
#تعریف_کړه یادښت_statistics _PyRuntime.parser.یادښت_statistics

فراغ
Py_Pegen_clear_یادښت_statistics(فراغ)
{
    Py_Mutex_قلف(&_PyRuntime.parser.mutex);
    لكل (صحيح i = 0; i < NSTATISTICS; i++) {
        یادښت_statistics[i] = 0;
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
}

PyObject *
Py_Pegen_get_یادښت_statistics(فراغ)
{
    PyObject *ret = PyList_New(NSTATISTICS);
    إذا (ret == عدم) {
        أرجع عدم;
    }

    Py_Mutex_قلف(&_PyRuntime.parser.mutex);
    لكل (صحيح i = 0; i < NSTATISTICS; i++) {
        PyObject *ارزښت = PyLong_FromLong(یادښت_statistics[i]);
        إذا (ارزښت == عدم) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            أرجع عدم;
        }
        // PyList_SetItem borrows a reference to value.
        إذا (Py_List_سېټ_توکی(ret, i, ارزښت) < 0) {
            PyMutex_Unlock(&_PyRuntime.parser.mutex);
            Py_DECREF(ret);
            أرجع عدم;
        }
    }
    PyMutex_Unlock(&_PyRuntime.parser.mutex);
    أرجع ret;
}
#د_که_پای

صحيح  // bool
_PyPegen_is_memoized(Parser *p, صحيح type, فراغ *pres)
{
    إذا (p->mark == p->fill) {
        إذا (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            أرجع -1;
        }
    }

    Token *t = p->نښې[p->mark];

    إذا (!(t->یادښت_ماسک & (1ULL << (type & 63)))) {
        أرجع 0;
    }

    لكل (یادښت *m = t->یادښت_3; m != عدم; m = m->بل) {
        إذا (m->type == type) {
#إذا defined(Py_DEBUG)
            إذا (0 <= type && type < NSTATISTICS) {
                اوږد شمېر = m->mark - p->mark;
                // A memoized negative result counts for one.
                إذا (شمېر <= 0) {
                    شمېر = 1;
                }
                Py_Mutex_قلف(&_PyRuntime.parser.mutex);
                یادښت_statistics[type] += شمېر;
                PyMutex_Unlock(&_PyRuntime.parser.mutex);
            }
#د_که_پای
            p->mark = m->mark;
            *(فراغ **)(pres) = m->غوټه;
            أرجع 1;
        }
    }
    أرجع 0;
}

#تعریف_کړه LOOKAHEAD1(NAME, RES_TYPE)                                  \
    صحيح                                                             \
    NAME (صحيح مثبت, RES_TYPE (func)(Parser *), Parser *p)       \
    {                                                               \
        صحيح mark = p->mark;                                         \
        فراغ *res = func(p);                                        \
        p->mark = mark;                                             \
        أرجع (res != عدم) == مثبت;                           \
    }

LOOKAHEAD1(_PyPegen_lookahead, فراغ *)
LOOKAHEAD1(Py_Pegen_lookahead_for_بیان, بیان_ty)
LOOKAHEAD1(_PyPegen_lookahead_for_stmt, stmt_ty)
#تعریف_لغوه LOOKAHEAD1

#تعریف_کړه LOOKAHEAD2(NAME, RES_TYPE, T)                                   \
    صحيح                                                                 \
    NAME (صحيح مثبت, RES_TYPE (func)(Parser *, T), Parser *p, T arg) \
    {                                                                   \
        صحيح mark = p->mark;                                             \
        فراغ *res = func(p, arg);                                       \
        p->mark = mark;                                                 \
        أرجع (res != عدم) == مثبت;                               \
    }

LOOKAHEAD2(_PyPegen_lookahead_with_int, Token *, صحيح)
LOOKAHEAD2(_PyPegen_lookahead_with_string, بیان_ty, ثابت حرف *)
#تعریف_لغوه LOOKAHEAD2

Token *
_PyPegen_expect_token(Parser *p, صحيح type)
{
    إذا (p->mark == p->fill) {
        إذا (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            أرجع عدم;
        }
    }
    Token *t = p->نښې[p->mark];
    إذا (t->type != type) {
       أرجع عدم;
    }
    p->mark += 1;
    أرجع t;
}

فراغ*
Py_Pegen_expect_forced_پایله(Parser *p, فراغ* پایله, ثابت حرف* expected) {

    إذا (p->error_indicator == 1) {
        أرجع عدم;
    }
    إذا (پایله == عدم) {
        RAISE_SYNTAX_ERROR("expected (%s)", expected);
        أرجع عدم;
    }
    أرجع پایله;
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
    Token *t = p->نښې[p->mark];
    إذا (t->type != type) {
        RAISE_SYNTAX_ERROR_معلوم_LOCATION(t, "expected '%s'", expected);
        أرجع عدم;
    }
    p->mark += 1;
    أرجع t;
}

بیان_ty
_PyPegen_expect_soft_keyword(Parser *p, ثابت حرف *keyword)
{
    إذا (p->mark == p->fill) {
        إذا (_PyPegen_fill_token(p) < 0) {
            p->error_indicator = 1;
            أرجع عدم;
        }
    }
    Token *t = p->نښې[p->mark];
    إذا (t->type != NAME) {
        أرجع عدم;
    }
    ثابت حرف *s = Py_بایټونه_As_String(t->بایټونه);
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
Py_Pegen_get_وروستی_nonnwhitespace_token(Parser *p)
{
    أكد(p->mark >= 0);
    Token *token = عدم;
    لكل (صحيح m = p->mark - 1; m >= 0; m--) {
        token = p->نښې[m];
        إذا (token->type != ENDMARKER && (token->type < NEWLINE || token->type > DEDENT)) {
            اكسر;
        }
    }
    أرجع token;
}

PyObject *
_PyPegen_new_identifier(Parser *p, ثابت حرف *n)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, (Py_ssize_t)مزي_اوږدوالی(n), عدم);
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
        PyObject *دلیلونه[2] = {form, id};
        PyObject *id2 = PyObject_Vectorcall(p->normalize, دلیلونه, 2, عدم);
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
            PyErr_Format(Py_Exc_ارزښت_Error,
                         "identifier field can't represent '%s' constant",
                         forbidden[i]);
            Py_DECREF(id);
            اذهب error;
        }
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &id);
    إذا (Py_Arena_جمع_کړه_Py_Object(p->arena, id) < 0)
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
get_cached_identifier(Parser *p, PyObject *بایټونه)
{
    ثابت حرف *s = Py_بایټونه_As_String(بایټونه);
    إذا (!s) {
        p->error_indicator = 1;
        أرجع عدم;
    }
    // Identifiers repeat constantly; a small span-keyed cache skips the
    // UTF-8 decode + intern for repeated occurrences. Keys point into
    // arena-owned token bytes and values are arena-owned interned strings,
    // so borrowed references are valid for the lifetime of the parse
    // (including the second error pass, which reuses parser and arena).
    Py_ssize_t len = Py_بایټونه_GET_اندازه(بایټونه);
    Py_hash_t hash = PyObject_Hash(بایټونه);
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
    Identifier_Cache_ثبت *free_خونه = عدم;
    اندازه_t idx = (اندازه_t)hash & (IDENTIFIER_CACHE_اندازه - 1);
    لكل (صحيح probe = 0; probe < IDENTIFIER_CACHE_MAX_PROBES; probe++) {
        Identifier_Cache_ثبت *ثبت = &p->identifier_cache[
            (idx + probe) & (IDENTIFIER_CACHE_اندازه - 1)];
        إذا (ثبت->کلی == عدم) {
            free_خونه = ثبت;
            اكسر;
        }
        إذا (ثبت->hash == hash && ثبت->len == len &&
            memcmp(ثبت->کلی, s, len) == 0)
        {
            أرجع ثبت->ارزښت;
        }
    }
    PyObject *id = _PyPegen_new_identifier(p, s);
    إذا (id == عدم) {
        p->error_indicator = 1;
        أرجع عدم;
    }
    إذا (free_خونه != عدم) {
        free_خونه->کلی = s;
        free_خونه->len = len;
        free_خونه->hash = hash;
        free_خونه->ارزښت = id;
    }
    أرجع id;
}

ساكن بیان_ty
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
    بیان_ty cached = عدم;
    إذا (_PyPegen_is_memoized(p, NAME, &cached)) {
        أرجع cached;
    }
    p->mark = mark + 1;
    PyObject *id = get_cached_identifier(p, t->بایټونه);
    إذا (id == عدم) {
        أرجع عدم;
    }
    بیان_ty پایله = _PyAST_Name(id, بارول, t->lineno, t->ستون_دننه_واټن,
                                t->پای_ټکی_lineno, t->پای_ټکی_ستون_دننه_واټن, p->arena);
    إذا (پایله != عدم && Py_Pegen_ورننباسه_یادښت(p, mark, NAME, پایله) < 0) {
        p->error_indicator = 1;
        أرجع عدم;
    }
    أرجع پایله;
}

بیان_ty
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

بیان_ty _PyPegen_soft_keyword_token(Parser *p) {
    Token *t = _PyPegen_expect_token(p, NAME);
    إذا (t == عدم) {
        أرجع عدم;
    }
    حرف *the_token;
    Py_ssize_t اندازه;
    Py_بایټونه_As_String_And_اندازه(t->بایټونه, &the_token, &اندازه);
    لكل (حرف **keyword = p->soft_keywords; *keyword != عدم; keyword++) {
        إذا (مزي_اوږدوالی(*keyword) == (اندازه_t)اندازه &&
            مزي_پرتله(*keyword, the_token, (اندازه_t)اندازه) == 0) {
            أرجع _PyPegen_name_from_token(p, t);
        }
    }
    أرجع عدم;
}

ساكن PyObject *
parsenumber_raw(ثابت حرف *s)
{
    ثابت حرف *پای_ټکی;
    اوږد x;
    مزدوج dx;
    Py_complex compl;
    صحيح imflag;

    أكد(s != عدم);
    errno = 0;
    پای_ټکی = s + مزي_اوږدوالی(s) - 1;
    imflag = *پای_ټکی == 'j' || *پای_ټکی == 'J';
    إذا (s[0] == '0') {
        x = (اوږد)PyOS_strtoul(s, (حرف **)&پای_ټکی, 0);
        إذا (x < 0 && errno == 0) {
            أرجع PyLong_FromString(s, (حرف **)0, 0);
        }
    }
    وإلا {
        x = PyOS_strtol(s, (حرف **)&پای_ټکی, 0);
    }
    إذا (*پای_ټکی == '\0') {
        إذا (errno != 0) {
            أرجع PyLong_FromString(s, (حرف **)0, 0);
        }
        أرجع PyLong_FromLong(x);
    }
    /* XXX Huge floats may silently fail */
    إذا (imflag) {
        compl.real = 0.;
        compl.imag = Py_OS_string_منزل_double(s, (حرف **)&پای_ټکی, عدم);
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
    حرف *پای_ټکی;
    PyObject *res = عدم;

    أكد(s != عدم);

    إذا (strchr(s, '_') == عدم) {
        أرجع parsenumber_raw(s);
    }
    /* Create a duplicate without underscores. */
    dup = PyMem_Malloc(مزي_اوږدوالی(s) + 1);
    إذا (dup == عدم) {
        أرجع Py_Err_No_حافظه();
    }
    پای_ټکی = dup;
    لكل (; *s; s++) {
        إذا (*s != '_') {
            *پای_ټکی++ = *s;
        }
    }
    *پای_ټکی = '\0';
    res = parsenumber_raw(dup);
    PyMem_Free(dup);
    أرجع res;
}

بیان_ty
Py_Pegen_شمېره_token(Parser *p)
{
    صحيح mark = p->mark;
    Token *t = _PyPegen_expect_token(p, شمېره);
    إذا (t == عدم) {
        أرجع عدم;
    }

    // Look up the node at the token's start, as for NAME tokens. A memo hit
    // advances the mark; on a miss, restore the position after the token.
    p->mark = mark;
    بیان_ty cached = عدم;
    إذا (_PyPegen_is_memoized(p, شمېره, &cached)) {
        أرجع cached;
    }
    p->mark = mark + 1;

    ثابت حرف *عدد_raw = Py_بایټونه_As_String(t->بایټونه);
    إذا (عدد_raw == عدم) {
        p->error_indicator = 1;
        أرجع عدم;
    }

    إذا (p->feature_نسخه < 6 && strchr(عدد_raw, '_') != عدم) {
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
            Py_TYPE(tstate->current_exception) == (PyTypeObject *)Py_Exc_ارزښت_Error
        ) {
            PyObject *exc = PyErr_GetRaisedException();
            /* Intentionally omitting columns to avoid a wall of 1000s of '^'s
             * on the error message. Nobody is going to overlook their huge
             * numeric literal once given the line. */
            RAISE_ERROR_معلوم_LOCATION(
                p, PyExc_SyntaxError,
                t->lineno, -1 /* col_offset */,
                t->پای_ټکی_lineno, -1 /* end_col_offset */,
                "%S - Consider hexadecimal for huge integer literals "
                "to avoid decimal conversion limits.",
                exc);
            Py_DECREF(exc);
        }
        أرجع عدم;
    }

    إذا (Py_Arena_جمع_کړه_Py_Object(p->arena, c) < 0) {
        Py_DECREF(c);
        p->error_indicator = 1;
        أرجع عدم;
    }

    بیان_ty پایله = _PyAST_Constant(c, عدم, t->lineno, t->ستون_دننه_واټن,
                                    t->پای_ټکی_lineno, t->پای_ټکی_ستون_دننه_واټن, p->arena);
    إذا (پایله != عدم && Py_Pegen_ورننباسه_یادښت(p, mark, شمېره, پایله) < 0) {
        p->error_indicator = 1;
        أرجع عدم;
    }
    أرجع پایله;
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
    إذا (flags->cf_flags & Py_CF_ALLOW_INCOMPLETE_ننوت) {
        parser_flags |= Py_PARSE_ALLOW_INCOMPLETE_ننوت;
    }
    أرجع parser_flags;
}

// Parser API

Parser *
_PyPegen_Parser_New(بنية tok_state *tok, صحيح پیل_قاعده, صحيح flags,
                    صحيح feature_نسخه, صحيح *errcode, ثابت حرف* سرچینه, PyArena *arena)
{
    Parser *p = PyMem_Malloc(حجم(Parser));
    إذا (p == عدم) {
        أرجع (Parser *) Py_Err_No_حافظه();
    }
    أكد(tok != عدم);
    Py_Tokenizer_سېټ_Options(tok, 0, (flags & PyPARSE_TYPE_COMMENTS) > 0);
    Py_Tokenizer_Allow_Incomplete_ننوت(tok, flags & Py_PARSE_ALLOW_INCOMPLETE_ننوت);
    p->tok = tok;
    p->keywords = عدم;
    p->n_keyword_lists = -1;
    p->soft_keywords = عدم;
    p->نښې = PyMem_Malloc(حجم(Token *));
    إذا (!p->نښې) {
        PyMem_Free(p);
        أرجع (Parser *) Py_Err_No_حافظه();
    }
    p->نښې[0] = PyMem_Calloc(1, حجم(Token));
    إذا (!p->نښې[0]) {
        PyMem_Free(p->نښې);
        PyMem_Free(p);
        أرجع (Parser *) Py_Err_No_حافظه();
    }
    p->type_ignore_comments = (growable_comment_array){0};

    p->mark = 0;
    p->fill = 0;
    p->اندازه = 1;

    p->errcode = errcode;
    p->arena = arena;
    p->پیل_قاعده = پیل_قاعده;
    p->parsing_started = 0;
    p->normalize = عدم;
    p->error_indicator = 0;

    p->starting_lineno = 0;
    p->starting_ستون_دننه_واټن = 0;
    p->flags = flags;
    p->feature_نسخه = feature_نسخه;
    p->معلوم_err_token = عدم;
    p->identifier_cache = عدم;
    p->tstate = PyThreadState_Get();
    // Stack limits are initialized when the thread state is attached.
    أكد(((_PyThreadStateImpl *)p->tstate)->c_ډېری_hard_حد != 0);
    p->ډېری_soft_حد = ((_PyThreadStateImpl *)p->tstate)->c_ډېری_soft_حد;
#إذا Py_ډېری_GROWS_ښکته
    p->ډېری_soft_حد += Py_OS_ډېری_MARGIN_بایټونه;
#وإلا
    p->ډېری_soft_حد -= Py_OS_ډېری_MARGIN_بایټونه;
#د_که_پای
    p->level = 0;
    p->call_invalid_قواعد = 0;
    p->ننوت_exhausted = 0;
    p->وروستی_stmt_location.lineno = 0;
    p->وروستی_stmt_location.ستون_دننه_واټن = 0;
    p->وروستی_stmt_location.پای_ټکی_lineno = 0;
    p->وروستی_stmt_location.پای_ټکی_ستون_دننه_واټن = 0;
#که_تعریف_شوی Py_DEBUG
    p->debug = _Py_GetConfig()->parser_debug;
#د_که_پای
    أرجع p;
}

فراغ
_PyPegen_Parser_Free(Parser *p)
{
    PyMem_Free(p->identifier_cache);
    Py_XDECREF(p->normalize);
    // Resizes allocate blocks starting at indices 1, 2, 4, and so on.
    PyMem_Free(p->نښې[0]);
    لكل (صحيح i = 1; i < p->اندازه; i *= PEGEN_ARRAY_GROWTH_ضریب) {
        PyMem_Free(p->نښې[i]);
    }
    PyMem_Free(p->نښې);
    growable_comment_array_deallocate(&p->type_ignore_comments);
    PyMem_Free(p);
}

ساكن فراغ
reset_parser_state_for_error_pass(Parser *p)
{
    p->وروستی_stmt_location.lineno = 0;
    p->وروستی_stmt_location.ستون_دننه_واټن = 0;
    p->وروستی_stmt_location.پای_ټکی_lineno = 0;
    p->وروستی_stmt_location.پای_ټکی_ستون_دننه_واټن = 0;
    لكل (صحيح i = 0; i < p->fill; i++) {
        p->نښې[i]->یادښت_3 = عدم;
        p->نښې[i]->یادښت_ماسک = 0;
    }
    p->mark = 0;
    p->flags &= ~Py_PARSE_ALLOW_INCOMPLETE_ننوت;
    Py_Tokenizer_Allow_Incomplete_ننوت(p->tok, 0);
    p->call_invalid_قواعد = 1;
    _PyTokenizer_StopInteractive(p->tok);
}

ساكن فراغ
Py_Pegen_سېټ_syntax_error_metadata(Parser *p) {
    PyObject *exc = PyErr_GetRaisedException();
    إذا (!exc || !PyObject_TypeCheck(exc, (PyTypeObject *)PyExc_SyntaxError)) {
        Py_Err_سېټ_Raised_Exception(exc);
        أرجع;
    }
    ثابت حرف *سرچینه = Py_Tokenizer_Retained_سرچینه(p->tok);
    ثابت حرف *encoding = _PyTokenizer_GetInfo(p->tok).encoding;
    PyObject* the_سرچینه = عدم;
    إذا (سرچینه) {
        إذا (encoding == عدم) {
            the_سرچینه = PyUnicode_FromString(سرچینه);
        } وإلا {
            the_سرچینه = PyUnicode_Decode(سرچینه, مزي_اوږدوالی(سرچینه), encoding, عدم);
        }
    }
    إذا (!the_سرچینه) {
        PyErr_Clear();
        the_سرچینه = Py_None;
        Py_INCREF(the_سرچینه);
    }
    PyObject* metadata = Py_Build_ارزښت(
        "(iiN)",
        p->وروستی_stmt_location.lineno,
        p->وروستی_stmt_location.ستون_دننه_واټن,
        the_سرچینه // N gives ownership to metadata
    );
    إذا (!metadata) {
        PyErr_Clear();
        Py_Err_سېټ_Raised_Exception(exc);
        أرجع;
    }
    PySyntaxErrorObject *syntax_error = (PySyntaxErrorObject *)exc;

    Py_XDECREF(syntax_error->metadata);
    syntax_error->metadata = metadata;
    Py_Err_سېټ_Raised_Exception(exc);
}

فراغ *
Py_Pegen_پرلپسې_parser(Parser *p)
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
            (PyErr_ExceptionMatches(Py_Exc_Incomplete_ننوت_Error) ||
             !PyErr_ExceptionMatches(PyExc_SyntaxError))) {
            أرجع عدم;
        }
        إذا ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_ننوت) &&
            p->ننوت_exhausted && !PyErr_Occurred()) {
            أرجع Py_Pegen_raise_incomplete_ننوت(p);
        }
       // Make a second parser pass. In this pass we activate heavier and slower checks
        // to produce better error messages and more complete diagnostics. Extra "invalid_*"
        // rules will be active during parsing.
        Token *وروستی_token = p->نښې[p->fill - 1];
        reset_parser_state_for_error_pass(p);
        _PyPegen_parse(p);

        // Set SyntaxErrors accordingly depending on the parser/tokenizer status at the failure
        // point.
        Pypegen_سېټ_syntax_error(p, وروستی_token);

        // Set the metadata in the exception from p->last_stmt_location
        إذا (PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            Py_Pegen_سېټ_syntax_error_metadata(p);
        }
       أرجع عدم;
    }

    إذا (p->پیل_قاعده == Py_single_ننوت && _PyTokenizer_HasTrailingStatement(p->tok)) {
        أرجع RAISE_SYNTAX_ERROR("multiple statements found while compiling a single statement");
    }

    // test_peg_generator defines _Py_TEST_PEGEN to not call PyAST_Validate()
#إذا defined(Py_DEBUG) && !defined(_Py_TEST_PEGEN)
    إذا (p->پیل_قاعده == Py_single_ننوت ||
        p->پیل_قاعده == Py_file_ننوت ||
        p->پیل_قاعده == Py_eval_ننوت)
    {
        إذا (!_PyAST_Validate(res)) {
            أرجع عدم;
        }
    }
#د_که_پای
    أرجع res;
}

mod_ty
Py_Pegen_پرلپسې_parser_from_file_اشاره_ګر(FILE *fp, صحيح پیل_قاعده, PyObject *filename_ob,
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
    mod_ty پایله = عدم;

    PyObject *module = PyUnicode_FromString("__main__");
    إذا (module == عدم) {
        اذهب error;
    }
    Py_Tokenizer_سېټ_Context(tok, filename_ob, module);
    Py_DECREF(module);

    Parser *p = _PyPegen_Parser_New(tok, پیل_قاعده, parser_flags, PY_MINOR_نسخه,
                                    errcode, عدم, arena);
    إذا (p == عدم) {
        اذهب error;
    }

    پایله = Py_Pegen_پرلپسې_parser(p);
    _PyPegen_Parser_Free(p);

    ثابت حرف *سرچینه = Py_Tokenizer_Retained_سرچینه(tok);
    إذا (سرچینه != عدم && پایله && interactive_src != عدم) {
        *interactive_src = PyUnicode_FromString(سرچینه);
        إذا (!*interactive_src || Py_Arena_جمع_کړه_Py_Object(arena, *interactive_src) < 0) {
            Py_XDECREF(*interactive_src);
            پایله = عدم;
            اذهب error;
        }
    }

error:
    _PyTokenizer_Free(tok);
    أرجع پایله;
}

mod_ty
Py_Pegen_پرلپسې_parser_from_string(ثابت حرف *str, صحيح پیل_قاعده, PyObject *filename_ob,
                       PyCompilerFlags *flags, PyArena *arena, PyObject *module,
                       PyObject **incomplete)
{
    _PyTokenizer_Newline newline_ډېرپېښ = پیل_قاعده == Py_file_ننوت
        ? _PyTokenizer_IMPLY_NEWLINE : _PyTokenizer_KEEP_NEWLINE;
    صحيح parser_flags = compute_parser_flags(flags);
    إذا (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_ننوت) {
        newline_ډېرپېښ = _PyTokenizer_APPEND_NEWLINE;
    }

    بنية tok_state *tok;
    إذا (flags != عدم && flags->cf_flags & PyCF_IGNORE_COOKIE) {
        tok = _PyTokenizer_FromUTF8(str, newline_ډېرپېښ, 0);
    } وإلا {
        tok = _PyTokenizer_FromString(str, newline_ډېرپېښ, 0);
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

    Py_Tokenizer_سېټ_Context(tok, filename_ob, module);

    // We need to clear up from here on
    mod_ty پایله = عدم;

    صحيح feature_نسخه = flags && (flags->cf_flags & PyCF_ONLY_AST) ?
        flags->cf_feature_نسخه : PY_MINOR_نسخه;
    Parser *p = _PyPegen_Parser_New(tok, پیل_قاعده, parser_flags, feature_نسخه,
                                    عدم, str, arena);
    إذا (p == عدم) {
        اذهب error;
    }

    پایله = Py_Pegen_پرلپسې_parser(p);
    إذا (پایله != عدم && (parser_flags & Py_PARSE_ALLOW_INCOMPLETE_ننوت)) {
        صحيح needs_newline = _PyTokenizer_GetNewlineRequirements(tok);
        إذا ((needs_newline & _PyTokenizer_NEWLINE_CONTINUATION) ||
            (پیل_قاعده != Py_file_ننوت &&
             (needs_newline & _PyTokenizer_NEWLINE_INDENT)) ||
            (پیل_قاعده == Py_single_ننوت && p->ننوت_exhausted &&
             (needs_newline & _PyTokenizer_NEWLINE_STATEMENT))) {
            Py_Pegen_raise_incomplete_ننوت(p);
            إذا (incomplete != عدم && PyErr_ExceptionMatches(Py_Exc_Incomplete_ننوت_Error)) {
                *incomplete = PyErr_GetRaisedException();
            }
            وإلا {
                پایله = عدم;
            }
        }
    }
    _PyPegen_Parser_Free(p);

error:
    _PyTokenizer_Free(tok);
    أرجع پایله;
}
