#جے_بنا_تعریف PEGEN_H
#تعریف_کرو PEGEN_H

#ضمن <Python.h>
#ضمن <pycore_ast.h>
#ضمن <pycore_token.h>
#ضمن <pycore_ceval.h>     // _Py_ReachedRecursionLimitWithMargin()
#ضمن <pycore_pystate.h>   // _Py_get_machine_stack_pointer()
#ضمن <pycore_pythonrun.h> // _PyOS_STACK_MARGIN_BYTES

#ضمن "tokenizer/tokenizer.h"

#إذا 0
#تعریف_کرو PyPARSE_YIELD_IS_KEYWORD        0x0001
#جے_انت

#تعریف_کرو PyPARSE_DONT_IMPLY_DEDENT       0x0002

#إذا 0
#تعریف_کرو PyPARSE_WITH_IS_KEYWORD         0x0003
#تعریف_کرو PyPARSE_PRINT_IS_FUNCTION       0x0004
#تعریف_کرو PyPARSE_UNICODE_LITERALS        0x0008
#جے_انت

#تعریف_کرو PyPARSE_IGNORE_COOKIE 0x0010
#تعریف_کرو PyPARSE_BARRY_AS_BDFL 0x0020
#تعریف_کرو PyPARSE_TYPE_COMMENTS 0x0040
#تعریف_کرو Py_PARSE_ALLOW_INCOMPLETE_داخلہ 0x0100

#تعریف_کرو CURRENT_POS (-5)

تعريفنوع بنية یاد_2 {
    صحيح type;
    فراغ *گرہ;
    صحيح mark;
    بنية یاد_2 *اگلا;
} یاد;

تعريفنوع بنية {
    صحيح type;
    // Text required by grammar actions or helpers; otherwise NULL.
    // An unfinished string token also has NULL text. Its grammar action
    // requests more input only if the token is valid at that position.
    PyObject *بائٹاں;
    صحيح level;
    صحيح is_raw;
    صحيح lineno, خانہ_وتھ, سرا_lineno, سرا_خانہ_وتھ;
    یاد *یاد_3;
    // Filter over the rule types present in `memo` (bit `type & 63` is set
    // for every entry): lets lookups skip walking the list on definite
    // misses, which are the common case.
    uint64_t یاد_نقاب;
    PyObject *metadata;
} Token;

تعريفنوع بنية {
    ثابت حرف *str;
    صحيح type;
} KeywordToken;


تعريفنوع بنية {
    بنية {
        صحيح lineno;
        حرف *comment;  // The " <tag>" in "# type: ignore <tag>"
    } *اشیاء;
    ناپ_t ناپ;
    ناپ_t عدد_اشیاء;
} growable_comment_array;

تعريفنوع بنية {
    صحيح lineno;
    صحيح خانہ_وتھ;
    صحيح سرا_lineno;
    صحيح سرا_خانہ_وتھ;
} location;

تعريفنوع بنية identifier_cache_اندراج Identifier_Cache_اندراج;

تعريفنوع بنية {
    بنية tok_state *tok;
    Token **ٹوکن;
    صحيح mark;
    صحيح fill, ناپ;
    PyArena *arena;
    KeywordToken **keywords;
    حرف **soft_keywords;
    صحيح n_keyword_lists;
    صحيح شروع_قاعدہ;
    صحيح *errcode;
    بنانشانی حرف parsing_started;
    بنانشانی حرف داخلہ_exhausted;
    PyObject* normalize;
    صحيح starting_lineno;
    صحيح starting_خانہ_وتھ;
    صحيح error_indicator;
    صحيح flags;
    صحيح feature_نسخہ;
    growable_comment_array type_ignore_comments;
    Token *معلوم_err_token;
    صحيح level;
    صحيح call_invalid_قاعدے;
    صحيح debug;
    location آخری_stmt_location;
    Identifier_Cache_اندراج *identifier_cache;
    PyThreadState *tstate;
    // Cached copy of tstate->c_stack_soft_limit offset by one margin toward
    // the stack top, so the per-rule stack check is a single inline comparison.
    uintptr_t ڈھیر_soft_حد_بندی;
} Parser;

// C-stack exhaustion check run on every rule entry. The fast path compares
// the machine stack pointer against the soft limit cached in the Parser;
// only when the stack is nearly exhausted is the full check called.
ساكن مضمن صحيح
Py_Pegen_ڈھیر_exhausted(Parser *p)
{
#إذا Py_ڈھیر_GROWS_تھلے
    إذا (Py_get_machine_ڈھیر_اشارہ_گر() > p->ڈھیر_soft_حد_بندی) {
#وإلا
    إذا (Py_get_machine_ڈھیر_اشارہ_گر() <= p->ڈھیر_soft_حد_بندی) {
#جے_انت
        أكد(!Py_Reached_Recursion_حد_بندی_With_Margin(p->tstate, 1));
        أرجع 0;
    }
    أرجع Py_Reached_Recursion_حد_بندی_With_Margin(p->tstate, 1);
}

تعريفنوع بنية {
    cmpop_ty cmpop;
    اظہاریہ_ty اظہاریہ;
} Cmpop_اظہاریہ_Pair;

تعريفنوع بنية {
    اظہاریہ_ty کلید;
    اظہاریہ_ty قدر;
} کلید_قدر_Pair;

تعريفنوع بنية {
    اظہاریہ_ty کلید;
    نمونہ_ty نمونہ;
} کلید_نمونہ_Pair;

تعريفنوع بنية {
    arg_ty arg;
    اظہاریہ_ty قدر;
} NameDefaultPair;

تعريفنوع بنية {
    asdl_arg_seq *plain_ناں;
    asdl_seq *ناں_with_defaults; // asdl_seq* of NameDefaultsPair's
} SlashWithDefault;

تعريفنوع بنية {
    arg_ty vararg;
    asdl_seq *kwonlyargs; // asdl_seq* of NameDefaultsPair's
    arg_ty kwarg;
} تارا_Etc;

تعريفنوع بنية { operator_ty قسم; } AugOperator;
تعريفنوع بنية {
    فراغ *element;
    صحيح is_keyword;
} KeywordOrStarred;

تعريفنوع بنية {
    فراغ *نتیجہ;
    PyObject *metadata;
} نتیجہ_Token_With_Metadata;

// Internal parser functions
#إذا defined(Py_DEBUG)
فراغ Py_Pegen_clear_یاد_statistics(فراغ);
PyObject *Py_Pegen_get_یاد_statistics(فراغ);
#جے_انت

صحيح Py_Pegen_داخل_کرو_یاد(Parser *p, صحيح mark, صحيح type, فراغ *گرہ);
صحيح Py_Pegen_update_یاد(Parser *p, صحيح mark, صحيح type, فراغ *گرہ);
یاد *Py_Pegen_داخل_کرو_یاد_سدھا(Parser *p, صحيح mark, صحيح type);
صحيح _PyPegen_is_memoized(Parser *p, صحيح type, فراغ *pres);

صحيح _PyPegen_lookahead(صحيح, فراغ *(func)(Parser *), Parser *);
صحيح Py_Pegen_lookahead_for_اظہاریہ(صحيح, اظہاریہ_ty (func)(Parser *), Parser *);
صحيح _PyPegen_lookahead_for_stmt(صحيح, stmt_ty (func)(Parser *), Parser *);
صحيح _PyPegen_lookahead_with_int(صحيح, Token *(func)(Parser *, صحيح), Parser *, صحيح);
صحيح _PyPegen_lookahead_with_string(صحيح, اظہاریہ_ty (func)(Parser *, ثابت حرف*), Parser *, ثابت حرف*);

Token *_PyPegen_expect_token(Parser *p, صحيح type);
فراغ* Py_Pegen_expect_forced_نتیجہ(Parser *p, فراغ* نتیجہ, ثابت حرف* expected);
Token *_PyPegen_expect_forced_token(Parser *p, صحيح type, ثابت حرف* expected);
اظہاریہ_ty _PyPegen_expect_soft_keyword(Parser *p, ثابت حرف *keyword);
اظہاریہ_ty _PyPegen_soft_keyword_token(Parser *p);
اظہاریہ_ty Py_Pegen_fstring_درمیان_token(Parser* p);
Token *Py_Pegen_get_آخری_nonnwhitespace_token(Parser *);
صحيح _PyPegen_fill_token(Parser *p);
// Return -1 on failure, leaving the buffer and capacity unchanged.
صحيح Py_Pegen_grow_loop_بفر(فراغ ***بفر, Py_ssize_t *سمرتھا);
اظہاریہ_ty _PyPegen_name_token(Parser *p);
اظہاریہ_ty Py_Pegen_نمبر_token(Parser *p);
فراغ *_PyPegen_string_token(Parser *p);
Py_ssize_t Py_Pegen_byte_وتھ_منزل_character_وتھ_سطر(PyObject *سطر, Py_ssize_t خانہ_وتھ, Py_ssize_t سرا_خانہ_وتھ);
Py_ssize_t Py_Pegen_byte_وتھ_منزل_character_وتھ(PyObject *سطر, Py_ssize_t خانہ_وتھ);
Py_ssize_t Py_Pegen_byte_وتھ_منزل_character_وتھ_raw(ثابت حرف*, Py_ssize_t خانہ_وتھ);

تعريفنوع تعداد {
    تارا_TARGETS,
    DEL_TARGETS,
    FOR_TARGETS,
    SINGLE_TARGETS,
    ATTRIBUTE_OR_SUBSCRIPT_TARGETS
} TARGETS_TYPE;

// Error handling functions and APIs
صحيح _Pypegen_raise_decode_error(Parser *p);
صحيح _Pypegen_tokenizer_error(Parser *p);
فراغ *Py_Pegen_raise_incomplete_داخلہ(Parser *p);
فراغ *_PyPegen_raise_error(Parser *p, PyObject *errtype, صحيح use_mark, ثابت حرف *errmsg, ...);
فراغ *Py_Pegen_raise_error_معلوم_location(Parser *p, PyObject *errtype,
                                          Py_ssize_t lineno, Py_ssize_t خانہ_وتھ,
                                          Py_ssize_t سرا_lineno, Py_ssize_t سرا_خانہ_وتھ,
                                          ثابت حرف *errmsg, va_list va);
فراغ Pypegen_سیٹ_syntax_error(Parser* p, Token* آخری_token);
فراغ Pypegen_ڈھیر_حد_توں_ودھ(Parser *p);

ساكن مضمن فراغ *
RAISE_ERROR_معلوم_LOCATION(Parser *p, PyObject *errtype,
                           Py_ssize_t lineno, Py_ssize_t خانہ_وتھ,
                           Py_ssize_t سرا_lineno, Py_ssize_t سرا_خانہ_وتھ,
                           ثابت حرف *errmsg, ...)
{
    va_list va;
    va_شروع(va, errmsg);
    Py_ssize_t خانہ_وتھ_2 = (خانہ_وتھ == CURRENT_POS ? CURRENT_POS : خانہ_وتھ + 1);
    Py_ssize_t سرا_خانہ_وتھ_2 = (سرا_خانہ_وتھ == CURRENT_POS ? CURRENT_POS : سرا_خانہ_وتھ + 1);
    Py_Pegen_raise_error_معلوم_location(p, errtype, lineno, خانہ_وتھ_2, سرا_lineno, سرا_خانہ_وتھ_2, errmsg, va);
    va_سرا(va);
    أرجع عدم;
}
#تعریف_کرو RAISE_SYNTAX_ERROR(پیغام, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 0, پیغام, ##__VA_ARGS__)
#تعریف_کرو RAISE_INDENTATION_ERROR(پیغام, ...) _PyPegen_raise_error(p, PyExc_IndentationError, 0, پیغام, ##__VA_ARGS__)
#تعریف_کرو RAISE_SYNTAX_ERROR_ON_اگلا_TOKEN(پیغام, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 1, پیغام, ##__VA_ARGS__)
#تعریف_کرو RAISE_SYNTAX_ERROR_معلوم_RANGE(a, b, پیغام, ...) \
    RAISE_ERROR_معلوم_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->خانہ_وتھ, (b)->سرا_lineno, (b)->سرا_خانہ_وتھ, پیغام, ##__VA_ARGS__)
#تعریف_کرو RAISE_SYNTAX_ERROR_معلوم_LOCATION(a, پیغام, ...) \
    RAISE_ERROR_معلوم_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->خانہ_وتھ, (a)->سرا_lineno, (a)->سرا_خانہ_وتھ, پیغام, ##__VA_ARGS__)
#تعریف_کرو RAISE_SYNTAX_ERROR_STARTING_FROM(a, پیغام, ...) \
    RAISE_ERROR_معلوم_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->خانہ_وتھ, CURRENT_POS, CURRENT_POS, پیغام, ##__VA_ARGS__)
#تعریف_کرو RAISE_SYNTAX_ERROR_INVALID_نشانہ(type, e) RAISE_SYNTAX_ERROR_INVALID_نشانہ_2(p, type, e)

Py_LOCAL_INLINE(صحيح)
Py_Pegen_ٹوکن_are_adjacent(Token *a, Token *b)
{
    أرجع (a->سرا_lineno == b->lineno) && (a->سرا_خانہ_وتھ == b->خانہ_وتھ);
}

Py_LOCAL_INLINE(فراغ *)
CHECK_CALL(Parser *p, فراغ *نتیجہ)
{
    إذا (نتیجہ == عدم) {
        أكد(PyErr_Occurred());
        p->error_indicator = 1;
    }
    أرجع نتیجہ;
}

/* This is needed for helper functions that are allowed to
   return NULL without an error. Example: _PyPegen_seq_extract_starred_exprs */
Py_LOCAL_INLINE(فراغ *)
CHECK_CALL_NULL_ALLOWED(Parser *p, فراغ *نتیجہ)
{
    إذا (نتیجہ == عدم && PyErr_Occurred()) {
        p->error_indicator = 1;
    }
    أرجع نتیجہ;
}

#تعریف_کرو CHECK(type, نتیجہ) ((type) CHECK_CALL(p, نتیجہ))
#تعریف_کرو CHECK_NULL_ALLOWED(type, نتیجہ) ((type) CHECK_CALL_NULL_ALLOWED(p, نتیجہ))

اظہاریہ_ty Py_Pegen_get_invalid_نشانہ(اظہاریہ_ty e, TARGETS_TYPE targets_type);
ثابت حرف *Py_Pegen_get_اظہاریہ_name(اظہاریہ_ty);
Py_LOCAL_INLINE(فراغ *)
RAISE_SYNTAX_ERROR_INVALID_نشانہ_2(Parser *p, TARGETS_TYPE type, فراغ *e)
{
    اظہاریہ_ty invalid_نشانہ = CHECK_NULL_ALLOWED(اظہاریہ_ty, Py_Pegen_get_invalid_نشانہ(e, type));
    إذا (invalid_نشانہ != عدم) {
        ثابت حرف *پیغام;
        إذا (type == تارا_TARGETS || type == FOR_TARGETS) {
            پیغام = "cannot assign to %s";
        }
        وإلا {
            پیغام = "cannot delete %s";
        }
        أرجع RAISE_SYNTAX_ERROR_معلوم_LOCATION(
            invalid_نشانہ,
            پیغام,
            Py_Pegen_get_اظہاریہ_name(invalid_نشانہ)
        );
        أرجع RAISE_SYNTAX_ERROR_معلوم_LOCATION(invalid_نشانہ, "invalid syntax");
    }
    أرجع عدم;
}

// Action utility functions

فراغ *_PyPegen_dummy_name(Parser *p, ...);
فراغ * Py_Pegen_seq_آخری_شے(asdl_seq *seq);
#تعریف_کرو Py_Pegen_آخری_شے(seq, type) ((type)Py_Pegen_seq_آخری_شے((asdl_seq*)seq))
فراغ * Py_Pegen_seq_پہلا_شے(asdl_seq *seq);
#تعریف_کرو Py_Pegen_پہلا_شے(seq, type) ((type)Py_Pegen_seq_پہلا_شے((asdl_seq*)seq))
#تعریف_کرو بن_ورتیا(اظہاریہ) افعل { (فراغ)(اظہاریہ); } طالما (0)
#تعریف_کرو EXTRA_اظہاریہ(سر, پوچھ) سر->lineno, (سر)->خانہ_وتھ, (پوچھ)->سرا_lineno, (پوچھ)->سرا_خانہ_وتھ, p->arena
#تعریف_کرو EXTRA شروع_lineno, شروع_خانہ_وتھ_2, سرا_lineno_2, سرا_خانہ_وتھ_2, p->arena
PyObject *_PyPegen_new_type_comment(Parser *, ثابت حرف *);

Py_LOCAL_INLINE(PyObject *)
NEW_TYPE_COMMENT(Parser *p, Token *tc)
{
    إذا (tc == عدم) {
        أرجع عدم;
    }
    ثابت حرف *بائٹاں = Py_بائٹاں_As_String(tc->بائٹاں);
    إذا (بائٹاں == عدم) {
        اذهب error;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, بائٹاں);
    إذا (tco == عدم) {
        اذهب error;
    }
    أرجع tco;
 error:
    p->error_indicator = 1;  // Inline CHECK_CALL
    أرجع عدم;
}

Py_LOCAL_INLINE(فراغ *)
INVALID_نسخہ_CHECK(Parser *p, صحيح نسخہ, حرف *پیغام, فراغ *گرہ)
{
    إذا (گرہ == عدم) {
        p->error_indicator = 1;  // Inline CHECK_CALL
        أرجع عدم;
    }
    إذا (p->feature_نسخہ < نسخہ) {
        p->error_indicator = 1;
        أرجع RAISE_SYNTAX_ERROR("%s only supported in Python 3.%i and greater",
                                  پیغام, نسخہ);
    }
    أرجع گرہ;
}

#تعریف_کرو CHECK_نسخہ(type, نسخہ, پیغام, گرہ) ((type) INVALID_نسخہ_CHECK(p, نسخہ, پیغام, گرہ))

arg_ty Py_Pegen_جوڑو_type_comment_منزل_arg(Parser *, arg_ty, Token *);
PyObject *_PyPegen_new_identifier(Parser *, ثابت حرف *);
asdl_seq *_PyPegen_singleton_seq(Parser *, فراغ *);
asdl_seq *Py_Pegen_seq_داخل_کرو_وچ_front(Parser *, فراغ *, asdl_seq *);
asdl_seq *Py_Pegen_seq_append_منزل_سرا(Parser *, asdl_seq *, فراغ *);
asdl_seq *_PyPegen_seq_flatten(Parser *, asdl_seq *);
اظہاریہ_ty Py_Pegen_join_ناں_with_dot(Parser *, اظہاریہ_ty, اظہاریہ_ty);
صحيح Py_Pegen_seq_گنتی_dots(asdl_seq *);
alias_ty Py_Pegen_alias_for_تارا(Parser *, صحيح, صحيح, صحيح, صحيح, PyArena *);
asdl_identifier_seq *Py_Pegen_map_ناں_منزل_ids(Parser *, asdl_اظہاریہ_seq *);
Cmpop_اظہاریہ_Pair *Py_Pegen_cmpop_اظہاریہ_pair(Parser *, cmpop_ty, اظہاریہ_ty);
asdl_int_seq *_PyPegen_get_cmpops(Parser *p, asdl_seq *);
asdl_اظہاریہ_seq *_PyPegen_get_exprs(Parser *, asdl_seq *);
اظہاریہ_ty Py_Pegen_سیٹ_اظہاریہ_context(Parser *, اظہاریہ_ty, اظہاریہ_context_ty);
اظہاریہ_ty Py_Pegen_make_نشانہ(Parser *, اظہاریہ_ty, TARGETS_TYPE);
کلید_قدر_Pair *Py_Pegen_کلید_قدر_pair(Parser *, اظہاریہ_ty, اظہاریہ_ty);
asdl_اظہاریہ_seq *Py_Pegen_get_کلیدیں(Parser *, asdl_seq *);
asdl_اظہاریہ_seq *Py_Pegen_get_اقدار(Parser *, asdl_seq *);
کلید_نمونہ_Pair *Py_Pegen_کلید_نمونہ_pair(Parser *, اظہاریہ_ty, نمونہ_ty);
asdl_اظہاریہ_seq *Py_Pegen_get_نمونہ_کلیدیں(Parser *, asdl_seq *);
asdl_نمونہ_seq *_PyPegen_get_patterns(Parser *, asdl_seq *);
NameDefaultPair *_PyPegen_name_default_pair(Parser *, arg_ty, اظہاریہ_ty, Token *);
SlashWithDefault *_PyPegen_slash_with_default(Parser *, asdl_arg_seq *, asdl_seq *);
تارا_Etc *Py_Pegen_تارا_etc(Parser *, arg_ty, asdl_seq *, arg_ty);
arguments_ty _PyPegen_make_arguments(Parser *, asdl_arg_seq *, SlashWithDefault *,
                                     asdl_arg_seq *, asdl_seq *, تارا_Etc *);
arguments_ty Py_Pegen_خالی_تھاواں_arguments(Parser *);
اظہاریہ_ty _PyPegen_template_str(Parser *p, Token *a, asdl_اظہاریہ_seq *raw_expressions, Token *b);
اظہاریہ_ty _PyPegen_joined_str(Parser *p, Token *a, asdl_اظہاریہ_seq *raw_expressions, Token *b);
اظہاریہ_ty _PyPegen_interpolation(Parser *, اظہاریہ_ty, Token *, نتیجہ_Token_With_Metadata *, نتیجہ_Token_With_Metadata *, Token *,
                                 صحيح, صحيح, صحيح, صحيح, PyArena *);
اظہاریہ_ty Py_Pegen_formatted_قدر(Parser *, اظہاریہ_ty, Token *, نتیجہ_Token_With_Metadata *, نتیجہ_Token_With_Metadata *, Token *,
                                 صحيح, صحيح, صحيح, صحيح, PyArena *);
AugOperator *_PyPegen_augoperator(Parser*, operator_ty type);
stmt_ty _PyPegen_function_def_decorators(Parser *, asdl_اظہاریہ_seq *, stmt_ty);
stmt_ty _PyPegen_class_def_decorators(Parser *, asdl_اظہاریہ_seq *, stmt_ty);
KeywordOrStarred *_PyPegen_keyword_or_starred(Parser *, فراغ *, صحيح);
asdl_اظہاریہ_seq *Py_Pegen_seq_کڈھو_starred_exprs(Parser *, asdl_seq *);
asdl_keyword_seq *_PyPegen_seq_delete_starred_exprs(Parser *, asdl_seq *);
اظہاریہ_ty _PyPegen_collect_call_seqs(Parser *, asdl_اظہاریہ_seq *, asdl_seq *,
                     صحيح lineno, صحيح خانہ_وتھ, صحيح سرا_lineno,
                     صحيح سرا_خانہ_وتھ, PyArena *arena);
اظہاریہ_ty _PyPegen_constant_from_token(Parser* p, Token* tok);
اظہاریہ_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok);
اظہاریہ_ty _PyPegen_constant_from_string(Parser* p, Token* tok);
اظہاریہ_ty _PyPegen_concatenate_tstrings(Parser *p, asdl_اظہاریہ_seq *, صحيح, صحيح, صحيح, صحيح, PyArena *);
اظہاریہ_ty Py_Pegen_concatenate_لڑیاں(Parser *p, asdl_اظہاریہ_seq *, صحيح, صحيح, صحيح, صحيح, PyArena *);
اظہاریہ_ty _PyPegen_FetchRawForm(Parser *p, صحيح, صحيح, صحيح, صحيح);
اظہاریہ_ty _PyPegen_ensure_imaginary(Parser *p, اظہاریہ_ty);
اظہاریہ_ty _PyPegen_ensure_real(Parser *p, اظہاریہ_ty);
asdl_seq *_PyPegen_join_sequences(Parser *, asdl_seq *, asdl_seq *);
صحيح _PyPegen_check_barry_as_flufl(Parser *, Token *);
صحيح _PyPegen_check_legacy_stmt(Parser *p, اظہاریہ_ty t);
فراغ *_PyPegen_raise_error_for_missing_comma(Parser *p, اظہاریہ_ty a, اظہاریہ_ty b);
نتیجہ_Token_With_Metadata *_PyPegen_check_fstring_conversion(Parser *p, Token *, اظہاریہ_ty t);
نتیجہ_Token_With_Metadata *Py_Pegen_تیاری_بھریا_format_spec(Parser *, Token *, asdl_اظہاریہ_seq *, صحيح, صحيح,
                                                         صحيح, صحيح, PyArena *);
mod_ty _PyPegen_make_module(Parser *, asdl_stmt_seq *);
فراغ *_PyPegen_arguments_parsing_error(Parser *, اظہاریہ_ty);
اظہاریہ_ty Py_Pegen_get_آخری_comprehension_شے(comprehension_ty comprehension);
فراغ *Py_Pegen_nonparen_genexp_وچ_call(Parser *p, اظہاریہ_ty دلیلاں, asdl_comprehension_seq *comprehensions);
stmt_ty _PyPegen_checked_from_import(Parser *p, asdl_seq *dots, اظہاریہ_ty module_name,
                                     asdl_alias_seq *ناں, اظہاریہ_ty lazy_token, صحيح lineno,
                                     صحيح خانہ_وتھ, صحيح سرا_lineno, صحيح سرا_خانہ_وتھ,
                                     PyArena *arena);
asdl_stmt_seq* Py_Pegen_رجسٹر_stmts(Parser *p, asdl_stmt_seq* stmts);
stmt_ty Py_Pegen_رجسٹر_stmt(Parser *p, stmt_ty s);

// Parser API

Parser *_PyPegen_Parser_New(بنية tok_state *, صحيح, صحيح, صحيح, صحيح *, ثابت حرف*, PyArena *);
فراغ _PyPegen_Parser_Free(Parser *);
mod_ty Py_Pegen_لگاتار_parser_from_file_اشارہ_گر(FILE *, صحيح, PyObject *, ثابت حرف *,
                                    ثابت حرف *, ثابت حرف *, PyCompilerFlags *, صحيح *, PyObject **,
                                    PyArena *);
فراغ *Py_Pegen_لگاتار_parser(Parser *);
mod_ty Py_Pegen_لگاتار_parser_from_string(ثابت حرف *, صحيح, PyObject *, PyCompilerFlags *, PyArena *, PyObject *, PyObject **);
asdl_stmt_seq *_PyPegen_interactive_exit(Parser *);

// Generated function in parse.c - function definition in python.gram
فراغ *_PyPegen_parse(Parser *);

#جے_انت
