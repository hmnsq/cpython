#اگر_غیر_تعریف_شدہ PEGEN_H
#تعریف_کرو PEGEN_H

#شامل <Python.h>
#شامل <pycore_ast.h>
#شامل <pycore_token.h>
#شامل <pycore_ceval.h>     // _Py_ReachedRecursionLimitWithMargin()
#شامل <pycore_pystate.h>   // _Py_get_machine_stack_pointer()
#شامل <pycore_pythonrun.h> // _PyOS_STACK_MARGIN_BYTES

#شامل "tokenizer/tokenizer.h"

#اگر 0
#تعریف_کرو PyPARSE_YIELD_IS_KEYWORD        0x0001
#اگر_ختم

#تعریف_کرو PyPARSE_DONT_IMPLY_DEDENT       0x0002

#اگر 0
#تعریف_کرو PyPARSE_WITH_IS_KEYWORD         0x0003
#تعریف_کرو PyPARSE_PRINT_IS_FUNCTION       0x0004
#تعریف_کرو PyPARSE_UNICODE_LITERALS        0x0008
#اگر_ختم

#تعریف_کرو PyPARSE_IGNORE_COOKIE 0x0010
#تعریف_کرو PyPARSE_BARRY_AS_BDFL 0x0020
#تعریف_کرو PyPARSE_TYPE_COMMENTS 0x0040
#تعریف_کرو Py_PARSE_ALLOW_INCOMPLETE_داخلہ 0x0100

#تعریف_کرو CURRENT_POS (-5)

قسمتعریف ساخت یادداشت_2 {
    صحیح type;
    خالی *گرہ;
    صحیح mark;
    ساخت یادداشت_2 *اگلا;
} یادداشت;

قسمتعریف ساخت {
    صحیح type;
    // Text required by grammar actions or helpers; otherwise NULL.
    // An unfinished string token also has NULL text. Its grammar action
    // requests more input only if the token is valid at that position.
    PyObject *بائٹیں;
    صحیح level;
    صحیح is_raw;
    صحیح lineno, ستون_انحراف, انتہا_lineno, انتہا_ستون_انحراف;
    یادداشت *یادداشت_3;
    // Filter over the rule types present in `memo` (bit `type & 63` is set
    // for every entry): lets lookups skip walking the list on definite
    // misses, which are the common case.
    uint64_t یادداشت_نقاب;
    PyObject *metadata;
} Token;

قسمتعریف ساخت {
    ثابت حرف *str;
    صحیح type;
} KeywordToken;


قسمتعریف ساخت {
    ساخت {
        صحیح lineno;
        حرف *comment;  // The " <tag>" in "# type: ignore <tag>"
    } *اشیاء;
    سائز_t سائز;
    سائز_t عدد_اشیاء;
} growable_comment_array;

قسمتعریف ساخت {
    صحیح lineno;
    صحیح ستون_انحراف;
    صحیح انتہا_lineno;
    صحیح انتہا_ستون_انحراف;
} location;

قسمتعریف ساخت identifier_cache_اندراج Identifier_Cache_اندراج;

قسمتعریف ساخت {
    ساخت tok_state *tok;
    Token **علامات;
    صحیح mark;
    صحیح fill, سائز;
    PyArena *arena;
    KeywordToken **keywords;
    حرف **soft_keywords;
    صحیح n_keyword_lists;
    صحیح آغاز_قاعدہ;
    صحیح *errcode;
    غیرعلامتی حرف parsing_started;
    غیرعلامتی حرف داخلہ_exhausted;
    PyObject* normalize;
    صحیح starting_lineno;
    صحیح starting_ستون_انحراف;
    صحیح error_indicator;
    صحیح flags;
    صحیح feature_نسخہ;
    growable_comment_array type_ignore_comments;
    Token *معلوم_err_token;
    صحیح level;
    صحیح call_invalid_قواعد;
    صحیح debug;
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
ساکن اندرونی صحیح
Py_Pegen_ڈھیر_exhausted(Parser *p)
{
#اگر Py_ڈھیر_GROWS_نیچے_کو
    اگر (Py_get_machine_ڈھیر_اشارہ_گر() > p->ڈھیر_soft_حد_بندی) {
#ورنہ
    اگر (Py_get_machine_ڈھیر_اشارہ_گر() <= p->ڈھیر_soft_حد_بندی) {
#اگر_ختم
        أكد(!Py_Reached_Recursion_حد_بندی_With_Margin(p->tstate, 1));
        واپس 0;
    }
    واپس Py_Reached_Recursion_حد_بندی_With_Margin(p->tstate, 1);
}

قسمتعریف ساخت {
    cmpop_ty cmpop;
    اظہاریہ_ty اظہاریہ;
} Cmpop_اظہاریہ_Pair;

قسمتعریف ساخت {
    اظہاریہ_ty کلید;
    اظہاریہ_ty قدر;
} کلید_قدر_Pair;

قسمتعریف ساخت {
    اظہاریہ_ty کلید;
    نمونہ_ty نمونہ;
} کلید_نمونہ_Pair;

قسمتعریف ساخت {
    arg_ty arg;
    اظہاریہ_ty قدر;
} NameDefaultPair;

قسمتعریف ساخت {
    asdl_arg_seq *plain_نام;
    asdl_seq *نام_with_defaults; // asdl_seq* of NameDefaultsPair's
} SlashWithDefault;

قسمتعریف ساخت {
    arg_ty vararg;
    asdl_seq *kwonlyargs; // asdl_seq* of NameDefaultsPair's
    arg_ty kwarg;
} ستارہ_Etc;

قسمتعریف ساخت { operator_ty قسم; } AugOperator;
قسمتعریف ساخت {
    خالی *element;
    صحیح is_keyword;
} KeywordOrStarred;

قسمتعریف ساخت {
    خالی *نتیجہ;
    PyObject *metadata;
} نتیجہ_Token_With_Metadata;

// Internal parser functions
#اگر defined(Py_DEBUG)
خالی Py_Pegen_clear_یادداشت_statistics(خالی);
PyObject *Py_Pegen_get_یادداشت_statistics(خالی);
#اگر_ختم

صحیح Py_Pegen_داخل_کرو_یادداشت(Parser *p, صحیح mark, صحیح type, خالی *گرہ);
صحیح Py_Pegen_update_یادداشت(Parser *p, صحیح mark, صحیح type, خالی *گرہ);
یادداشت *Py_Pegen_داخل_کرو_یادداشت_براہ_راست(Parser *p, صحیح mark, صحیح type);
صحیح _PyPegen_is_memoized(Parser *p, صحیح type, خالی *pres);

صحیح _PyPegen_lookahead(صحیح, خالی *(func)(Parser *), Parser *);
صحیح Py_Pegen_lookahead_for_اظہاریہ(صحیح, اظہاریہ_ty (func)(Parser *), Parser *);
صحیح _PyPegen_lookahead_for_stmt(صحیح, stmt_ty (func)(Parser *), Parser *);
صحیح _PyPegen_lookahead_with_int(صحیح, Token *(func)(Parser *, صحیح), Parser *, صحیح);
صحیح _PyPegen_lookahead_with_string(صحیح, اظہاریہ_ty (func)(Parser *, ثابت حرف*), Parser *, ثابت حرف*);

Token *_PyPegen_expect_token(Parser *p, صحیح type);
خالی* Py_Pegen_expect_forced_نتیجہ(Parser *p, خالی* نتیجہ, ثابت حرف* expected);
Token *_PyPegen_expect_forced_token(Parser *p, صحیح type, ثابت حرف* expected);
اظہاریہ_ty _PyPegen_expect_soft_keyword(Parser *p, ثابت حرف *keyword);
اظہاریہ_ty _PyPegen_soft_keyword_token(Parser *p);
اظہاریہ_ty Py_Pegen_fstring_درمیان_token(Parser* p);
Token *Py_Pegen_get_آخری_nonnwhitespace_token(Parser *);
صحیح _PyPegen_fill_token(Parser *p);
// Return -1 on failure, leaving the buffer and capacity unchanged.
صحیح Py_Pegen_grow_loop_بفر(خالی ***بفر, Py_ssize_t *استعداد);
اظہاریہ_ty _PyPegen_name_token(Parser *p);
اظہاریہ_ty Py_Pegen_نمبر_token(Parser *p);
خالی *_PyPegen_string_token(Parser *p);
Py_ssize_t Py_Pegen_byte_انحراف_مقصد_character_انحراف_سطر(PyObject *سطر, Py_ssize_t ستون_انحراف, Py_ssize_t انتہا_ستون_انحراف);
Py_ssize_t Py_Pegen_byte_انحراف_مقصد_character_انحراف(PyObject *سطر, Py_ssize_t ستون_انحراف);
Py_ssize_t Py_Pegen_byte_انحراف_مقصد_character_انحراف_raw(ثابت حرف*, Py_ssize_t ستون_انحراف);

قسمتعریف شمار {
    ستارہ_TARGETS,
    DEL_TARGETS,
    FOR_TARGETS,
    SINGLE_TARGETS,
    ATTRIBUTE_OR_SUBSCRIPT_TARGETS
} TARGETS_TYPE;

// Error handling functions and APIs
صحیح _Pypegen_raise_decode_error(Parser *p);
صحیح _Pypegen_tokenizer_error(Parser *p);
خالی *Py_Pegen_raise_incomplete_داخلہ(Parser *p);
خالی *_PyPegen_raise_error(Parser *p, PyObject *errtype, صحیح use_mark, ثابت حرف *errmsg, ...);
خالی *Py_Pegen_raise_error_معلوم_location(Parser *p, PyObject *errtype,
                                          Py_ssize_t lineno, Py_ssize_t ستون_انحراف,
                                          Py_ssize_t انتہا_lineno, Py_ssize_t انتہا_ستون_انحراف,
                                          ثابت حرف *errmsg, va_list va);
خالی Pypegen_سیٹ_syntax_error(Parser* p, Token* آخری_token);
خالی Pypegen_ڈھیر_حد_سے_تجاوز(Parser *p);

ساکن اندرونی خالی *
RAISE_ERROR_معلوم_LOCATION(Parser *p, PyObject *errtype,
                           Py_ssize_t lineno, Py_ssize_t ستون_انحراف,
                           Py_ssize_t انتہا_lineno, Py_ssize_t انتہا_ستون_انحراف,
                           ثابت حرف *errmsg, ...)
{
    va_list va;
    va_آغاز(va, errmsg);
    Py_ssize_t ستون_انحراف_2 = (ستون_انحراف == CURRENT_POS ? CURRENT_POS : ستون_انحراف + 1);
    Py_ssize_t انتہا_ستون_انحراف_2 = (انتہا_ستون_انحراف == CURRENT_POS ? CURRENT_POS : انتہا_ستون_انحراف + 1);
    Py_Pegen_raise_error_معلوم_location(p, errtype, lineno, ستون_انحراف_2, انتہا_lineno, انتہا_ستون_انحراف_2, errmsg, va);
    va_انتہا(va);
    واپس تہی;
}
#تعریف_کرو RAISE_SYNTAX_ERROR(پیغام, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 0, پیغام, ##__VA_ARGS__)
#تعریف_کرو RAISE_INDENTATION_ERROR(پیغام, ...) _PyPegen_raise_error(p, PyExc_IndentationError, 0, پیغام, ##__VA_ARGS__)
#تعریف_کرو RAISE_SYNTAX_ERROR_ON_اگلا_TOKEN(پیغام, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 1, پیغام, ##__VA_ARGS__)
#تعریف_کرو RAISE_SYNTAX_ERROR_معلوم_RANGE(a, b, پیغام, ...) \
    RAISE_ERROR_معلوم_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->ستون_انحراف, (b)->انتہا_lineno, (b)->انتہا_ستون_انحراف, پیغام, ##__VA_ARGS__)
#تعریف_کرو RAISE_SYNTAX_ERROR_معلوم_LOCATION(a, پیغام, ...) \
    RAISE_ERROR_معلوم_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->ستون_انحراف, (a)->انتہا_lineno, (a)->انتہا_ستون_انحراف, پیغام, ##__VA_ARGS__)
#تعریف_کرو RAISE_SYNTAX_ERROR_STARTING_FROM(a, پیغام, ...) \
    RAISE_ERROR_معلوم_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->ستون_انحراف, CURRENT_POS, CURRENT_POS, پیغام, ##__VA_ARGS__)
#تعریف_کرو RAISE_SYNTAX_ERROR_INVALID_ہدف(type, e) RAISE_SYNTAX_ERROR_INVALID_ہدف_2(p, type, e)

Py_LOCAL_INLINE(صحیح)
Py_Pegen_علامات_are_adjacent(Token *a, Token *b)
{
    واپس (a->انتہا_lineno == b->lineno) && (a->انتہا_ستون_انحراف == b->ستون_انحراف);
}

Py_LOCAL_INLINE(خالی *)
CHECK_CALL(Parser *p, خالی *نتیجہ)
{
    اگر (نتیجہ == تہی) {
        أكد(PyErr_Occurred());
        p->error_indicator = 1;
    }
    واپس نتیجہ;
}

/* This is needed for helper functions that are allowed to
   return NULL without an error. Example: _PyPegen_seq_extract_starred_exprs */
Py_LOCAL_INLINE(خالی *)
CHECK_CALL_NULL_ALLOWED(Parser *p, خالی *نتیجہ)
{
    اگر (نتیجہ == تہی && PyErr_Occurred()) {
        p->error_indicator = 1;
    }
    واپس نتیجہ;
}

#تعریف_کرو CHECK(type, نتیجہ) ((type) CHECK_CALL(p, نتیجہ))
#تعریف_کرو CHECK_NULL_ALLOWED(type, نتیجہ) ((type) CHECK_CALL_NULL_ALLOWED(p, نتیجہ))

اظہاریہ_ty Py_Pegen_get_invalid_ہدف(اظہاریہ_ty e, TARGETS_TYPE targets_type);
ثابت حرف *Py_Pegen_get_اظہاریہ_name(اظہاریہ_ty);
Py_LOCAL_INLINE(خالی *)
RAISE_SYNTAX_ERROR_INVALID_ہدف_2(Parser *p, TARGETS_TYPE type, خالی *e)
{
    اظہاریہ_ty invalid_ہدف = CHECK_NULL_ALLOWED(اظہاریہ_ty, Py_Pegen_get_invalid_ہدف(e, type));
    اگر (invalid_ہدف != تہی) {
        ثابت حرف *پیغام;
        اگر (type == ستارہ_TARGETS || type == FOR_TARGETS) {
            پیغام = "cannot assign to %s";
        }
        ورنہ {
            پیغام = "cannot delete %s";
        }
        واپس RAISE_SYNTAX_ERROR_معلوم_LOCATION(
            invalid_ہدف,
            پیغام,
            Py_Pegen_get_اظہاریہ_name(invalid_ہدف)
        );
        واپس RAISE_SYNTAX_ERROR_معلوم_LOCATION(invalid_ہدف, "invalid syntax");
    }
    واپس تہی;
}

// Action utility functions

خالی *_PyPegen_dummy_name(Parser *p, ...);
خالی * Py_Pegen_seq_آخری_شے(asdl_seq *seq);
#تعریف_کرو Py_Pegen_آخری_شے(seq, type) ((type)Py_Pegen_seq_آخری_شے((asdl_seq*)seq))
خالی * Py_Pegen_seq_پہلا_شے(asdl_seq *seq);
#تعریف_کرو Py_Pegen_پہلا_شے(seq, type) ((type)Py_Pegen_seq_پہلا_شے((asdl_seq*)seq))
#تعریف_کرو غیر_مستعمل(اظہاریہ) کرو { (خالی)(اظہاریہ); } جب (0)
#تعریف_کرو EXTRA_اظہاریہ(سر, دم) سر->lineno, (سر)->ستون_انحراف, (دم)->انتہا_lineno, (دم)->انتہا_ستون_انحراف, p->arena
#تعریف_کرو EXTRA آغاز_lineno, آغاز_ستون_انحراف_2, انتہا_lineno_2, انتہا_ستون_انحراف_2, p->arena
PyObject *_PyPegen_new_type_comment(Parser *, ثابت حرف *);

Py_LOCAL_INLINE(PyObject *)
NEW_TYPE_COMMENT(Parser *p, Token *tc)
{
    اگر (tc == تہی) {
        واپس تہی;
    }
    ثابت حرف *بائٹیں = Py_بائٹیں_As_String(tc->بائٹیں);
    اگر (بائٹیں == تہی) {
        جاؤ error;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, بائٹیں);
    اگر (tco == تہی) {
        جاؤ error;
    }
    واپس tco;
 error:
    p->error_indicator = 1;  // Inline CHECK_CALL
    واپس تہی;
}

Py_LOCAL_INLINE(خالی *)
INVALID_نسخہ_CHECK(Parser *p, صحیح نسخہ, حرف *پیغام, خالی *گرہ)
{
    اگر (گرہ == تہی) {
        p->error_indicator = 1;  // Inline CHECK_CALL
        واپس تہی;
    }
    اگر (p->feature_نسخہ < نسخہ) {
        p->error_indicator = 1;
        واپس RAISE_SYNTAX_ERROR("%s only supported in Python 3.%i and greater",
                                  پیغام, نسخہ);
    }
    واپس گرہ;
}

#تعریف_کرو CHECK_نسخہ(type, نسخہ, پیغام, گرہ) ((type) INVALID_نسخہ_CHECK(p, نسخہ, پیغام, گرہ))

arg_ty Py_Pegen_جوڑو_type_comment_مقصد_arg(Parser *, arg_ty, Token *);
PyObject *_PyPegen_new_identifier(Parser *, ثابت حرف *);
asdl_seq *_PyPegen_singleton_seq(Parser *, خالی *);
asdl_seq *Py_Pegen_seq_داخل_کرو_اندر_front(Parser *, خالی *, asdl_seq *);
asdl_seq *Py_Pegen_seq_append_مقصد_انتہا(Parser *, asdl_seq *, خالی *);
asdl_seq *_PyPegen_seq_flatten(Parser *, asdl_seq *);
اظہاریہ_ty Py_Pegen_join_نام_with_dot(Parser *, اظہاریہ_ty, اظہاریہ_ty);
صحیح Py_Pegen_seq_گنتی_dots(asdl_seq *);
alias_ty Py_Pegen_alias_for_ستارہ(Parser *, صحیح, صحیح, صحیح, صحیح, PyArena *);
asdl_identifier_seq *Py_Pegen_map_نام_مقصد_ids(Parser *, asdl_اظہاریہ_seq *);
Cmpop_اظہاریہ_Pair *Py_Pegen_cmpop_اظہاریہ_pair(Parser *, cmpop_ty, اظہاریہ_ty);
asdl_int_seq *_PyPegen_get_cmpops(Parser *p, asdl_seq *);
asdl_اظہاریہ_seq *_PyPegen_get_exprs(Parser *, asdl_seq *);
اظہاریہ_ty Py_Pegen_سیٹ_اظہاریہ_context(Parser *, اظہاریہ_ty, اظہاریہ_context_ty);
اظہاریہ_ty Py_Pegen_make_ہدف(Parser *, اظہاریہ_ty, TARGETS_TYPE);
کلید_قدر_Pair *Py_Pegen_کلید_قدر_pair(Parser *, اظہاریہ_ty, اظہاریہ_ty);
asdl_اظہاریہ_seq *Py_Pegen_get_کلیدیں(Parser *, asdl_seq *);
asdl_اظہاریہ_seq *Py_Pegen_get_اقدار(Parser *, asdl_seq *);
کلید_نمونہ_Pair *Py_Pegen_کلید_نمونہ_pair(Parser *, اظہاریہ_ty, نمونہ_ty);
asdl_اظہاریہ_seq *Py_Pegen_get_نمونہ_کلیدیں(Parser *, asdl_seq *);
asdl_نمونہ_seq *_PyPegen_get_patterns(Parser *, asdl_seq *);
NameDefaultPair *_PyPegen_name_default_pair(Parser *, arg_ty, اظہاریہ_ty, Token *);
SlashWithDefault *_PyPegen_slash_with_default(Parser *, asdl_arg_seq *, asdl_seq *);
ستارہ_Etc *Py_Pegen_ستارہ_etc(Parser *, arg_ty, asdl_seq *, arg_ty);
arguments_ty _PyPegen_make_arguments(Parser *, asdl_arg_seq *, SlashWithDefault *,
                                     asdl_arg_seq *, asdl_seq *, ستارہ_Etc *);
arguments_ty Py_Pegen_خالی_خانے_arguments(Parser *);
اظہاریہ_ty _PyPegen_template_str(Parser *p, Token *a, asdl_اظہاریہ_seq *raw_expressions, Token *b);
اظہاریہ_ty _PyPegen_joined_str(Parser *p, Token *a, asdl_اظہاریہ_seq *raw_expressions, Token *b);
اظہاریہ_ty _PyPegen_interpolation(Parser *, اظہاریہ_ty, Token *, نتیجہ_Token_With_Metadata *, نتیجہ_Token_With_Metadata *, Token *,
                                 صحیح, صحیح, صحیح, صحیح, PyArena *);
اظہاریہ_ty Py_Pegen_formatted_قدر(Parser *, اظہاریہ_ty, Token *, نتیجہ_Token_With_Metadata *, نتیجہ_Token_With_Metadata *, Token *,
                                 صحیح, صحیح, صحیح, صحیح, PyArena *);
AugOperator *_PyPegen_augoperator(Parser*, operator_ty type);
stmt_ty _PyPegen_function_def_decorators(Parser *, asdl_اظہاریہ_seq *, stmt_ty);
stmt_ty _PyPegen_class_def_decorators(Parser *, asdl_اظہاریہ_seq *, stmt_ty);
KeywordOrStarred *_PyPegen_keyword_or_starred(Parser *, خالی *, صحیح);
asdl_اظہاریہ_seq *Py_Pegen_seq_نکالو_starred_exprs(Parser *, asdl_seq *);
asdl_keyword_seq *_PyPegen_seq_delete_starred_exprs(Parser *, asdl_seq *);
اظہاریہ_ty _PyPegen_collect_call_seqs(Parser *, asdl_اظہاریہ_seq *, asdl_seq *,
                     صحیح lineno, صحیح ستون_انحراف, صحیح انتہا_lineno,
                     صحیح انتہا_ستون_انحراف, PyArena *arena);
اظہاریہ_ty _PyPegen_constant_from_token(Parser* p, Token* tok);
اظہاریہ_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok);
اظہاریہ_ty _PyPegen_constant_from_string(Parser* p, Token* tok);
اظہاریہ_ty _PyPegen_concatenate_tstrings(Parser *p, asdl_اظہاریہ_seq *, صحیح, صحیح, صحیح, صحیح, PyArena *);
اظہاریہ_ty Py_Pegen_concatenate_لڑیاں(Parser *p, asdl_اظہاریہ_seq *, صحیح, صحیح, صحیح, صحیح, PyArena *);
اظہاریہ_ty _PyPegen_FetchRawForm(Parser *p, صحیح, صحیح, صحیح, صحیح);
اظہاریہ_ty _PyPegen_ensure_imaginary(Parser *p, اظہاریہ_ty);
اظہاریہ_ty _PyPegen_ensure_real(Parser *p, اظہاریہ_ty);
asdl_seq *_PyPegen_join_sequences(Parser *, asdl_seq *, asdl_seq *);
صحیح _PyPegen_check_barry_as_flufl(Parser *, Token *);
صحیح _PyPegen_check_legacy_stmt(Parser *p, اظہاریہ_ty t);
خالی *_PyPegen_raise_error_for_missing_comma(Parser *p, اظہاریہ_ty a, اظہاریہ_ty b);
نتیجہ_Token_With_Metadata *_PyPegen_check_fstring_conversion(Parser *p, Token *, اظہاریہ_ty t);
نتیجہ_Token_With_Metadata *Py_Pegen_تیاری_بھرا_format_spec(Parser *, Token *, asdl_اظہاریہ_seq *, صحیح, صحیح,
                                                         صحیح, صحیح, PyArena *);
mod_ty _PyPegen_make_module(Parser *, asdl_stmt_seq *);
خالی *_PyPegen_arguments_parsing_error(Parser *, اظہاریہ_ty);
اظہاریہ_ty Py_Pegen_get_آخری_comprehension_شے(comprehension_ty comprehension);
خالی *Py_Pegen_nonparen_genexp_اندر_call(Parser *p, اظہاریہ_ty دلائل, asdl_comprehension_seq *comprehensions);
stmt_ty _PyPegen_checked_from_import(Parser *p, asdl_seq *dots, اظہاریہ_ty module_name,
                                     asdl_alias_seq *نام, اظہاریہ_ty lazy_token, صحیح lineno,
                                     صحیح ستون_انحراف, صحیح انتہا_lineno, صحیح انتہا_ستون_انحراف,
                                     PyArena *arena);
asdl_stmt_seq* Py_Pegen_رجسٹر_stmts(Parser *p, asdl_stmt_seq* stmts);
stmt_ty Py_Pegen_رجسٹر_stmt(Parser *p, stmt_ty s);

// Parser API

Parser *_PyPegen_Parser_New(ساخت tok_state *, صحیح, صحیح, صحیح, صحیح *, ثابت حرف*, PyArena *);
خالی _PyPegen_Parser_Free(Parser *);
mod_ty Py_Pegen_تسلسل_parser_from_file_اشارہ_گر(FILE *, صحیح, PyObject *, ثابت حرف *,
                                    ثابت حرف *, ثابت حرف *, PyCompilerFlags *, صحیح *, PyObject **,
                                    PyArena *);
خالی *Py_Pegen_تسلسل_parser(Parser *);
mod_ty Py_Pegen_تسلسل_parser_from_string(ثابت حرف *, صحیح, PyObject *, PyCompilerFlags *, PyArena *, PyObject *, PyObject **);
asdl_stmt_seq *_PyPegen_interactive_exit(Parser *);

// Generated function in parse.c - function definition in python.gram
خالی *_PyPegen_parse(Parser *);

#اگر_ختم
