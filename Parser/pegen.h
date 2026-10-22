#اگر_تعریف_نشده PEGEN_H
#تعریف_کن PEGEN_H

#ضمن <Python.h>
#ضمن <pycore_ast.h>
#ضمن <pycore_token.h>
#ضمن <pycore_ceval.h>     // _Py_ReachedRecursionLimitWithMargin()
#ضمن <pycore_pystate.h>   // _Py_get_machine_stack_pointer()
#ضمن <pycore_pythonrun.h> // _PyOS_STACK_MARGIN_BYTES

#ضمن "tokenizer/tokenizer.h"

#إذا 0
#تعریف_کن PyPARSE_YIELD_IS_KEYWORD        0x0001
#پایان_اگر

#تعریف_کن PyPARSE_DONT_IMPLY_DEDENT       0x0002

#إذا 0
#تعریف_کن PyPARSE_WITH_IS_KEYWORD         0x0003
#تعریف_کن PyPARSE_PRINT_IS_FUNCTION       0x0004
#تعریف_کن PyPARSE_UNICODE_LITERALS        0x0008
#پایان_اگر

#تعریف_کن PyPARSE_IGNORE_COOKIE 0x0010
#تعریف_کن PyPARSE_BARRY_AS_BDFL 0x0020
#تعریف_کن PyPARSE_TYPE_COMMENTS 0x0040
#تعریف_کن Py_PARSE_ALLOW_INCOMPLETE_درون_داد 0x0100

#تعریف_کن CURRENT_POS (-5)

تعريفنوع بنية یادداشت_2 {
    صحيح type;
    فراغ *گره;
    صحيح mark;
    بنية یادداشت_2 *بعدی;
} یادداشت;

تعريفنوع بنية {
    صحيح type;
    // Text required by grammar actions or helpers; otherwise NULL.
    // An unfinished string token also has NULL text. Its grammar action
    // requests more input only if the token is valid at that position.
    PyObject *بایتها;
    صحيح level;
    صحيح is_raw;
    صحيح lineno, ستون_جابه_جایی, انتها_lineno, انتها_ستون_جابه_جایی;
    یادداشت *یادداشت_3;
    // Filter over the rule types present in `memo` (bit `type & 63` is set
    // for every entry): lets lookups skip walking the list on definite
    // misses, which are the common case.
    uint64_t یادداشت_ماسک;
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
    } *اقلام;
    اندازه_t اندازه;
    اندازه_t رقم_اقلام;
} growable_comment_array;

تعريفنوع بنية {
    صحيح lineno;
    صحيح ستون_جابه_جایی;
    صحيح انتها_lineno;
    صحيح انتها_ستون_جابه_جایی;
} location;

تعريفنوع بنية identifier_cache_مدخل Identifier_Cache_مدخل;

تعريفنوع بنية {
    بنية tok_state *tok;
    Token **نشانهها;
    صحيح mark;
    صحيح fill, اندازه;
    PyArena *arena;
    KeywordToken **keywords;
    حرف **soft_keywords;
    صحيح n_keyword_lists;
    صحيح شروع_قاعده;
    صحيح *errcode;
    بیعلامت حرف parsing_started;
    بیعلامت حرف درون_داد_exhausted;
    PyObject* normalize;
    صحيح starting_lineno;
    صحيح starting_ستون_جابه_جایی;
    صحيح error_indicator;
    صحيح flags;
    صحيح feature_نسخه;
    growable_comment_array type_ignore_comments;
    Token *معلوم_err_token;
    صحيح level;
    صحيح call_invalid_قواعد;
    صحيح debug;
    location آخرین_stmt_location;
    Identifier_Cache_مدخل *identifier_cache;
    PyThreadState *tstate;
    // Cached copy of tstate->c_stack_soft_limit offset by one margin toward
    // the stack top, so the per-rule stack check is a single inline comparison.
    uintptr_t پشته_soft_حد;
} Parser;

// C-stack exhaustion check run on every rule entry. The fast path compares
// the machine stack pointer against the soft limit cached in the Parser;
// only when the stack is nearly exhausted is the full check called.
ساكن مضمن صحيح
Py_Pegen_پشته_exhausted(Parser *p)
{
#إذا Py_پشته_GROWS_پایین
    إذا (Py_get_machine_پشته_اشاره_گر() > p->پشته_soft_حد) {
#وإلا
    إذا (Py_get_machine_پشته_اشاره_گر() <= p->پشته_soft_حد) {
#پایان_اگر
        أكد(!Py_Reached_Recursion_حد_With_Margin(p->tstate, 1));
        أرجع 0;
    }
    أرجع Py_Reached_Recursion_حد_With_Margin(p->tstate, 1);
}

تعريفنوع بنية {
    cmpop_ty cmpop;
    عبارت_ty عبارت;
} Cmpop_عبارت_Pair;

تعريفنوع بنية {
    عبارت_ty کلید;
    عبارت_ty مقدار;
} کلید_مقدار_Pair;

تعريفنوع بنية {
    عبارت_ty کلید;
    الگو_ty الگو;
} کلید_الگو_Pair;

تعريفنوع بنية {
    arg_ty arg;
    عبارت_ty مقدار;
} NameDefaultPair;

تعريفنوع بنية {
    asdl_arg_seq *plain_نامها;
    asdl_seq *نامها_with_defaults; // asdl_seq* of NameDefaultsPair's
} SlashWithDefault;

تعريفنوع بنية {
    arg_ty vararg;
    asdl_seq *kwonlyargs; // asdl_seq* of NameDefaultsPair's
    arg_ty kwarg;
} ستاره_Etc;

تعريفنوع بنية { operator_ty نوع; } AugOperator;
تعريفنوع بنية {
    فراغ *element;
    صحيح is_keyword;
} KeywordOrStarred;

تعريفنوع بنية {
    فراغ *نتیجه;
    PyObject *metadata;
} نتیجه_Token_With_Metadata;

// Internal parser functions
#إذا defined(Py_DEBUG)
فراغ Py_Pegen_clear_یادداشت_statistics(فراغ);
PyObject *Py_Pegen_get_یادداشت_statistics(فراغ);
#پایان_اگر

صحيح Py_Pegen_درج_یادداشت(Parser *p, صحيح mark, صحيح type, فراغ *گره);
صحيح Py_Pegen_update_یادداشت(Parser *p, صحيح mark, صحيح type, فراغ *گره);
یادداشت *Py_Pegen_درج_یادداشت_مستقیم(Parser *p, صحيح mark, صحيح type);
صحيح _PyPegen_is_memoized(Parser *p, صحيح type, فراغ *pres);

صحيح _PyPegen_lookahead(صحيح, فراغ *(func)(Parser *), Parser *);
صحيح Py_Pegen_lookahead_for_عبارت(صحيح, عبارت_ty (func)(Parser *), Parser *);
صحيح _PyPegen_lookahead_for_stmt(صحيح, stmt_ty (func)(Parser *), Parser *);
صحيح _PyPegen_lookahead_with_int(صحيح, Token *(func)(Parser *, صحيح), Parser *, صحيح);
صحيح _PyPegen_lookahead_with_string(صحيح, عبارت_ty (func)(Parser *, ثابت حرف*), Parser *, ثابت حرف*);

Token *_PyPegen_expect_token(Parser *p, صحيح type);
فراغ* Py_Pegen_expect_forced_نتیجه(Parser *p, فراغ* نتیجه, ثابت حرف* expected);
Token *_PyPegen_expect_forced_token(Parser *p, صحيح type, ثابت حرف* expected);
عبارت_ty _PyPegen_expect_soft_keyword(Parser *p, ثابت حرف *keyword);
عبارت_ty _PyPegen_soft_keyword_token(Parser *p);
عبارت_ty Py_Pegen_fstring_میانه_token(Parser* p);
Token *Py_Pegen_get_آخرین_nonnwhitespace_token(Parser *);
صحيح _PyPegen_fill_token(Parser *p);
// Return -1 on failure, leaving the buffer and capacity unchanged.
صحيح Py_Pegen_grow_loop_میانگیر(فراغ ***میانگیر, Py_ssize_t *ظرفیت);
عبارت_ty _PyPegen_name_token(Parser *p);
عبارت_ty Py_Pegen_عدد_token(Parser *p);
فراغ *_PyPegen_string_token(Parser *p);
Py_ssize_t Py_Pegen_byte_جابه_جایی_مقصد_character_جابه_جایی_سطر(PyObject *سطر, Py_ssize_t ستون_جابه_جایی, Py_ssize_t انتها_ستون_جابه_جایی);
Py_ssize_t Py_Pegen_byte_جابه_جایی_مقصد_character_جابه_جایی(PyObject *سطر, Py_ssize_t ستون_جابه_جایی);
Py_ssize_t Py_Pegen_byte_جابه_جایی_مقصد_character_جابه_جایی_raw(ثابت حرف*, Py_ssize_t ستون_جابه_جایی);

تعريفنوع تعداد {
    ستاره_TARGETS,
    DEL_TARGETS,
    FOR_TARGETS,
    SINGLE_TARGETS,
    ATTRIBUTE_OR_SUBSCRIPT_TARGETS
} TARGETS_TYPE;

// Error handling functions and APIs
صحيح _Pypegen_raise_decode_error(Parser *p);
صحيح _Pypegen_tokenizer_error(Parser *p);
فراغ *Py_Pegen_raise_incomplete_درون_داد(Parser *p);
فراغ *_PyPegen_raise_error(Parser *p, PyObject *errtype, صحيح use_mark, ثابت حرف *errmsg, ...);
فراغ *Py_Pegen_raise_error_معلوم_location(Parser *p, PyObject *errtype,
                                          Py_ssize_t lineno, Py_ssize_t ستون_جابه_جایی,
                                          Py_ssize_t انتها_lineno, Py_ssize_t انتها_ستون_جابه_جایی,
                                          ثابت حرف *errmsg, va_list va);
فراغ Pypegen_مجموعه_syntax_error(Parser* p, Token* آخرین_token);
فراغ Pypegen_پشته_سرریز(Parser *p);

ساكن مضمن فراغ *
RAISE_ERROR_معلوم_LOCATION(Parser *p, PyObject *errtype,
                           Py_ssize_t lineno, Py_ssize_t ستون_جابه_جایی,
                           Py_ssize_t انتها_lineno, Py_ssize_t انتها_ستون_جابه_جایی,
                           ثابت حرف *errmsg, ...)
{
    va_list va;
    va_شروع(va, errmsg);
    Py_ssize_t ستون_جابه_جایی_2 = (ستون_جابه_جایی == CURRENT_POS ? CURRENT_POS : ستون_جابه_جایی + 1);
    Py_ssize_t انتها_ستون_جابه_جایی_2 = (انتها_ستون_جابه_جایی == CURRENT_POS ? CURRENT_POS : انتها_ستون_جابه_جایی + 1);
    Py_Pegen_raise_error_معلوم_location(p, errtype, lineno, ستون_جابه_جایی_2, انتها_lineno, انتها_ستون_جابه_جایی_2, errmsg, va);
    va_انتها(va);
    أرجع عدم;
}
#تعریف_کن RAISE_SYNTAX_ERROR(پیام, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 0, پیام, ##__VA_ARGS__)
#تعریف_کن RAISE_INDENTATION_ERROR(پیام, ...) _PyPegen_raise_error(p, PyExc_IndentationError, 0, پیام, ##__VA_ARGS__)
#تعریف_کن RAISE_SYNTAX_ERROR_ON_بعدی_TOKEN(پیام, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 1, پیام, ##__VA_ARGS__)
#تعریف_کن RAISE_SYNTAX_ERROR_معلوم_RANGE(a, b, پیام, ...) \
    RAISE_ERROR_معلوم_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->ستون_جابه_جایی, (b)->انتها_lineno, (b)->انتها_ستون_جابه_جایی, پیام, ##__VA_ARGS__)
#تعریف_کن RAISE_SYNTAX_ERROR_معلوم_LOCATION(a, پیام, ...) \
    RAISE_ERROR_معلوم_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->ستون_جابه_جایی, (a)->انتها_lineno, (a)->انتها_ستون_جابه_جایی, پیام, ##__VA_ARGS__)
#تعریف_کن RAISE_SYNTAX_ERROR_STARTING_FROM(a, پیام, ...) \
    RAISE_ERROR_معلوم_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->ستون_جابه_جایی, CURRENT_POS, CURRENT_POS, پیام, ##__VA_ARGS__)
#تعریف_کن RAISE_SYNTAX_ERROR_INVALID_هدف(type, e) RAISE_SYNTAX_ERROR_INVALID_هدف_2(p, type, e)

Py_LOCAL_INLINE(صحيح)
Py_Pegen_نشانهها_are_adjacent(Token *a, Token *b)
{
    أرجع (a->انتها_lineno == b->lineno) && (a->انتها_ستون_جابه_جایی == b->ستون_جابه_جایی);
}

Py_LOCAL_INLINE(فراغ *)
CHECK_CALL(Parser *p, فراغ *نتیجه)
{
    إذا (نتیجه == عدم) {
        أكد(PyErr_Occurred());
        p->error_indicator = 1;
    }
    أرجع نتیجه;
}

/* This is needed for helper functions that are allowed to
   return NULL without an error. Example: _PyPegen_seq_extract_starred_exprs */
Py_LOCAL_INLINE(فراغ *)
CHECK_CALL_NULL_ALLOWED(Parser *p, فراغ *نتیجه)
{
    إذا (نتیجه == عدم && PyErr_Occurred()) {
        p->error_indicator = 1;
    }
    أرجع نتیجه;
}

#تعریف_کن CHECK(type, نتیجه) ((type) CHECK_CALL(p, نتیجه))
#تعریف_کن CHECK_NULL_ALLOWED(type, نتیجه) ((type) CHECK_CALL_NULL_ALLOWED(p, نتیجه))

عبارت_ty Py_Pegen_get_invalid_هدف(عبارت_ty e, TARGETS_TYPE targets_type);
ثابت حرف *Py_Pegen_get_عبارت_name(عبارت_ty);
Py_LOCAL_INLINE(فراغ *)
RAISE_SYNTAX_ERROR_INVALID_هدف_2(Parser *p, TARGETS_TYPE type, فراغ *e)
{
    عبارت_ty invalid_هدف = CHECK_NULL_ALLOWED(عبارت_ty, Py_Pegen_get_invalid_هدف(e, type));
    إذا (invalid_هدف != عدم) {
        ثابت حرف *پیام;
        إذا (type == ستاره_TARGETS || type == FOR_TARGETS) {
            پیام = "cannot assign to %s";
        }
        وإلا {
            پیام = "cannot delete %s";
        }
        أرجع RAISE_SYNTAX_ERROR_معلوم_LOCATION(
            invalid_هدف,
            پیام,
            Py_Pegen_get_عبارت_name(invalid_هدف)
        );
        أرجع RAISE_SYNTAX_ERROR_معلوم_LOCATION(invalid_هدف, "invalid syntax");
    }
    أرجع عدم;
}

// Action utility functions

فراغ *_PyPegen_dummy_name(Parser *p, ...);
فراغ * Py_Pegen_seq_آخرین_قلم(asdl_seq *seq);
#تعریف_کن Py_Pegen_آخرین_قلم(seq, type) ((type)Py_Pegen_seq_آخرین_قلم((asdl_seq*)seq))
فراغ * Py_Pegen_seq_اولی_قلم(asdl_seq *seq);
#تعریف_کن Py_Pegen_اولی_قلم(seq, type) ((type)Py_Pegen_seq_اولی_قلم((asdl_seq*)seq))
#تعریف_کن استفادهنشده(عبارت) افعل { (فراغ)(عبارت); } طالما (0)
#تعریف_کن EXTRA_عبارت(سر, دم) سر->lineno, (سر)->ستون_جابه_جایی, (دم)->انتها_lineno, (دم)->انتها_ستون_جابه_جایی, p->arena
#تعریف_کن EXTRA شروع_lineno, شروع_ستون_جابه_جایی_2, انتها_lineno_2, انتها_ستون_جابه_جایی_2, p->arena
PyObject *_PyPegen_new_type_comment(Parser *, ثابت حرف *);

Py_LOCAL_INLINE(PyObject *)
NEW_TYPE_COMMENT(Parser *p, Token *tc)
{
    إذا (tc == عدم) {
        أرجع عدم;
    }
    ثابت حرف *بایتها = Py_بایتها_As_String(tc->بایتها);
    إذا (بایتها == عدم) {
        اذهب error;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, بایتها);
    إذا (tco == عدم) {
        اذهب error;
    }
    أرجع tco;
 error:
    p->error_indicator = 1;  // Inline CHECK_CALL
    أرجع عدم;
}

Py_LOCAL_INLINE(فراغ *)
INVALID_نسخه_CHECK(Parser *p, صحيح نسخه, حرف *پیام, فراغ *گره)
{
    إذا (گره == عدم) {
        p->error_indicator = 1;  // Inline CHECK_CALL
        أرجع عدم;
    }
    إذا (p->feature_نسخه < نسخه) {
        p->error_indicator = 1;
        أرجع RAISE_SYNTAX_ERROR("%s only supported in Python 3.%i and greater",
                                  پیام, نسخه);
    }
    أرجع گره;
}

#تعریف_کن CHECK_نسخه(type, نسخه, پیام, گره) ((type) INVALID_نسخه_CHECK(p, نسخه, پیام, گره))

arg_ty Py_Pegen_جمع_کن_type_comment_مقصد_arg(Parser *, arg_ty, Token *);
PyObject *_PyPegen_new_identifier(Parser *, ثابت حرف *);
asdl_seq *_PyPegen_singleton_seq(Parser *, فراغ *);
asdl_seq *Py_Pegen_seq_درج_درون_front(Parser *, فراغ *, asdl_seq *);
asdl_seq *Py_Pegen_seq_append_مقصد_انتها(Parser *, asdl_seq *, فراغ *);
asdl_seq *_PyPegen_seq_flatten(Parser *, asdl_seq *);
عبارت_ty Py_Pegen_join_نامها_with_dot(Parser *, عبارت_ty, عبارت_ty);
صحيح Py_Pegen_seq_شمار_dots(asdl_seq *);
alias_ty Py_Pegen_alias_for_ستاره(Parser *, صحيح, صحيح, صحيح, صحيح, PyArena *);
asdl_identifier_seq *Py_Pegen_map_نامها_مقصد_ids(Parser *, asdl_عبارت_seq *);
Cmpop_عبارت_Pair *Py_Pegen_cmpop_عبارت_pair(Parser *, cmpop_ty, عبارت_ty);
asdl_int_seq *_PyPegen_get_cmpops(Parser *p, asdl_seq *);
asdl_عبارت_seq *_PyPegen_get_exprs(Parser *, asdl_seq *);
عبارت_ty Py_Pegen_مجموعه_عبارت_context(Parser *, عبارت_ty, عبارت_context_ty);
عبارت_ty Py_Pegen_make_هدف(Parser *, عبارت_ty, TARGETS_TYPE);
کلید_مقدار_Pair *Py_Pegen_کلید_مقدار_pair(Parser *, عبارت_ty, عبارت_ty);
asdl_عبارت_seq *Py_Pegen_get_کلیدها(Parser *, asdl_seq *);
asdl_عبارت_seq *Py_Pegen_get_مقادیر(Parser *, asdl_seq *);
کلید_الگو_Pair *Py_Pegen_کلید_الگو_pair(Parser *, عبارت_ty, الگو_ty);
asdl_عبارت_seq *Py_Pegen_get_الگو_کلیدها(Parser *, asdl_seq *);
asdl_الگو_seq *_PyPegen_get_patterns(Parser *, asdl_seq *);
NameDefaultPair *_PyPegen_name_default_pair(Parser *, arg_ty, عبارت_ty, Token *);
SlashWithDefault *_PyPegen_slash_with_default(Parser *, asdl_arg_seq *, asdl_seq *);
ستاره_Etc *Py_Pegen_ستاره_etc(Parser *, arg_ty, asdl_seq *, arg_ty);
arguments_ty _PyPegen_make_arguments(Parser *, asdl_arg_seq *, SlashWithDefault *,
                                     asdl_arg_seq *, asdl_seq *, ستاره_Etc *);
arguments_ty Py_Pegen_خالی_arguments(Parser *);
عبارت_ty _PyPegen_template_str(Parser *p, Token *a, asdl_عبارت_seq *raw_expressions, Token *b);
عبارت_ty _PyPegen_joined_str(Parser *p, Token *a, asdl_عبارت_seq *raw_expressions, Token *b);
عبارت_ty _PyPegen_interpolation(Parser *, عبارت_ty, Token *, نتیجه_Token_With_Metadata *, نتیجه_Token_With_Metadata *, Token *,
                                 صحيح, صحيح, صحيح, صحيح, PyArena *);
عبارت_ty Py_Pegen_formatted_مقدار(Parser *, عبارت_ty, Token *, نتیجه_Token_With_Metadata *, نتیجه_Token_With_Metadata *, Token *,
                                 صحيح, صحيح, صحيح, صحيح, PyArena *);
AugOperator *_PyPegen_augoperator(Parser*, operator_ty type);
stmt_ty _PyPegen_function_def_decorators(Parser *, asdl_عبارت_seq *, stmt_ty);
stmt_ty _PyPegen_class_def_decorators(Parser *, asdl_عبارت_seq *, stmt_ty);
KeywordOrStarred *_PyPegen_keyword_or_starred(Parser *, فراغ *, صحيح);
asdl_عبارت_seq *Py_Pegen_seq_استخراج_starred_exprs(Parser *, asdl_seq *);
asdl_keyword_seq *_PyPegen_seq_delete_starred_exprs(Parser *, asdl_seq *);
عبارت_ty _PyPegen_collect_call_seqs(Parser *, asdl_عبارت_seq *, asdl_seq *,
                     صحيح lineno, صحيح ستون_جابه_جایی, صحيح انتها_lineno,
                     صحيح انتها_ستون_جابه_جایی, PyArena *arena);
عبارت_ty _PyPegen_constant_from_token(Parser* p, Token* tok);
عبارت_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok);
عبارت_ty _PyPegen_constant_from_string(Parser* p, Token* tok);
عبارت_ty _PyPegen_concatenate_tstrings(Parser *p, asdl_عبارت_seq *, صحيح, صحيح, صحيح, صحيح, PyArena *);
عبارت_ty Py_Pegen_concatenate_رشتهها(Parser *p, asdl_عبارت_seq *, صحيح, صحيح, صحيح, صحيح, PyArena *);
عبارت_ty _PyPegen_FetchRawForm(Parser *p, صحيح, صحيح, صحيح, صحيح);
عبارت_ty _PyPegen_ensure_imaginary(Parser *p, عبارت_ty);
عبارت_ty _PyPegen_ensure_real(Parser *p, عبارت_ty);
asdl_seq *_PyPegen_join_sequences(Parser *, asdl_seq *, asdl_seq *);
صحيح _PyPegen_check_barry_as_flufl(Parser *, Token *);
صحيح _PyPegen_check_legacy_stmt(Parser *p, عبارت_ty t);
فراغ *_PyPegen_raise_error_for_missing_comma(Parser *p, عبارت_ty a, عبارت_ty b);
نتیجه_Token_With_Metadata *_PyPegen_check_fstring_conversion(Parser *p, Token *, عبارت_ty t);
نتیجه_Token_With_Metadata *Py_Pegen_راهاندازی_پر_format_spec(Parser *, Token *, asdl_عبارت_seq *, صحيح, صحيح,
                                                         صحيح, صحيح, PyArena *);
mod_ty _PyPegen_make_module(Parser *, asdl_stmt_seq *);
فراغ *_PyPegen_arguments_parsing_error(Parser *, عبارت_ty);
عبارت_ty Py_Pegen_get_آخرین_comprehension_قلم(comprehension_ty comprehension);
فراغ *Py_Pegen_nonparen_genexp_درون_call(Parser *p, عبارت_ty آرگومانها, asdl_comprehension_seq *comprehensions);
stmt_ty _PyPegen_checked_from_import(Parser *p, asdl_seq *dots, عبارت_ty module_name,
                                     asdl_alias_seq *نامها, عبارت_ty lazy_token, صحيح lineno,
                                     صحيح ستون_جابه_جایی, صحيح انتها_lineno, صحيح انتها_ستون_جابه_جایی,
                                     PyArena *arena);
asdl_stmt_seq* Py_Pegen_ثبات_stmts(Parser *p, asdl_stmt_seq* stmts);
stmt_ty Py_Pegen_ثبات_stmt(Parser *p, stmt_ty s);

// Parser API

Parser *_PyPegen_Parser_New(بنية tok_state *, صحيح, صحيح, صحيح, صحيح *, ثابت حرف*, PyArena *);
فراغ _PyPegen_Parser_Free(Parser *);
mod_ty Py_Pegen_پیاپی_parser_from_file_اشاره_گر(FILE *, صحيح, PyObject *, ثابت حرف *,
                                    ثابت حرف *, ثابت حرف *, PyCompilerFlags *, صحيح *, PyObject **,
                                    PyArena *);
فراغ *Py_Pegen_پیاپی_parser(Parser *);
mod_ty Py_Pegen_پیاپی_parser_from_string(ثابت حرف *, صحيح, PyObject *, PyCompilerFlags *, PyArena *, PyObject *, PyObject **);
asdl_stmt_seq *_PyPegen_interactive_exit(Parser *);

// Generated function in parse.c - function definition in python.gram
فراغ *_PyPegen_parse(Parser *);

#پایان_اگر
