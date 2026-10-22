#جيڪڏهن_وصف_نه_ٿيل PEGEN_H
#وصف_ڪريو PEGEN_H

#ضمن <Python.h>
#ضمن <pycore_ast.h>
#ضمن <pycore_token.h>
#ضمن <pycore_ceval.h>     // _Py_ReachedRecursionLimitWithMargin()
#ضمن <pycore_pystate.h>   // _Py_get_machine_stack_pointer()
#ضمن <pycore_pythonrun.h> // _PyOS_STACK_MARGIN_BYTES

#ضمن "tokenizer/tokenizer.h"

#إذا 0
#وصف_ڪريو PyPARSE_YIELD_IS_KEYWORD        0x0001
#جيڪڏهن_پورو

#وصف_ڪريو PyPARSE_DONT_IMPLY_DEDENT       0x0002

#إذا 0
#وصف_ڪريو PyPARSE_WITH_IS_KEYWORD         0x0003
#وصف_ڪريو PyPARSE_PRINT_IS_FUNCTION       0x0004
#وصف_ڪريو PyPARSE_UNICODE_LITERALS        0x0008
#جيڪڏهن_پورو

#وصف_ڪريو PyPARSE_IGNORE_COOKIE 0x0010
#وصف_ڪريو PyPARSE_BARRY_AS_BDFL 0x0020
#وصف_ڪريو PyPARSE_TYPE_COMMENTS 0x0040
#وصف_ڪريو Py_PARSE_ALLOW_INCOMPLETE_داخلا 0x0100

#وصف_ڪريو CURRENT_POS (-5)

تعريفنوع بنية ياداشت_2 {
    صحيح type;
    فراغ *ڳنڍ;
    صحيح mark;
    بنية ياداشت_2 *ايندڙ;
} ياداشت;

تعريفنوع بنية {
    صحيح type;
    // Text required by grammar actions or helpers; otherwise NULL.
    // An unfinished string token also has NULL text. Its grammar action
    // requests more input only if the token is valid at that position.
    PyObject *بائيٽون;
    صحيح level;
    صحيح is_raw;
    صحيح lineno, کانو_وٿي, پڇاڙي_lineno, پڇاڙي_کانو_وٿي;
    ياداشت *ياداشت_3;
    // Filter over the rule types present in `memo` (bit `type & 63` is set
    // for every entry): lets lookups skip walking the list on definite
    // misses, which are the common case.
    uint64_t ياداشت_نقاب;
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
    } *شيون;
    سائيز_t سائيز;
    سائيز_t انگ_شيون;
} growable_comment_array;

تعريفنوع بنية {
    صحيح lineno;
    صحيح کانو_وٿي;
    صحيح پڇاڙي_lineno;
    صحيح پڇاڙي_کانو_وٿي;
} location;

تعريفنوع بنية identifier_cache_داخلا Identifier_Cache_داخلا;

تعريفنوع بنية {
    بنية tok_state *tok;
    Token **ٽوڪن;
    صحيح mark;
    صحيح fill, سائيز;
    PyArena *arena;
    KeywordToken **keywords;
    حرف **soft_keywords;
    صحيح n_keyword_lists;
    صحيح شروعات_قاعدو;
    صحيح *errcode;
    بنانشان حرف parsing_started;
    بنانشان حرف داخلا_exhausted;
    PyObject* normalize;
    صحيح starting_lineno;
    صحيح starting_کانو_وٿي;
    صحيح error_indicator;
    صحيح flags;
    صحيح feature_نسخو;
    growable_comment_array type_ignore_comments;
    Token *معلوم_err_token;
    صحيح level;
    صحيح call_invalid_قاعدا;
    صحيح debug;
    location آخري_stmt_location;
    Identifier_Cache_داخلا *identifier_cache;
    PyThreadState *tstate;
    // Cached copy of tstate->c_stack_soft_limit offset by one margin toward
    // the stack top, so the per-rule stack check is a single inline comparison.
    uintptr_t ڍير_soft_حد_بندي;
} Parser;

// C-stack exhaustion check run on every rule entry. The fast path compares
// the machine stack pointer against the soft limit cached in the Parser;
// only when the stack is nearly exhausted is the full check called.
ساكن مضمن صحيح
Py_Pegen_ڍير_exhausted(Parser *p)
{
#إذا Py_ڍير_GROWS_هيٺ_ڏانهن
    إذا (Py_get_machine_ڍير_اشارو() > p->ڍير_soft_حد_بندي) {
#وإلا
    إذا (Py_get_machine_ڍير_اشارو() <= p->ڍير_soft_حد_بندي) {
#جيڪڏهن_پورو
        أكد(!Py_Reached_Recursion_حد_بندي_With_Margin(p->tstate, 1));
        أرجع 0;
    }
    أرجع Py_Reached_Recursion_حد_بندي_With_Margin(p->tstate, 1);
}

تعريفنوع بنية {
    cmpop_ty cmpop;
    اظهار_ty اظهار;
} Cmpop_اظهار_Pair;

تعريفنوع بنية {
    اظهار_ty ڪنجي;
    اظهار_ty قدر;
} ڪنجي_قدر_Pair;

تعريفنوع بنية {
    اظهار_ty ڪنجي;
    نمونو_ty نمونو;
} ڪنجي_نمونو_Pair;

تعريفنوع بنية {
    arg_ty arg;
    اظهار_ty قدر;
} NameDefaultPair;

تعريفنوع بنية {
    asdl_arg_seq *plain_نالا;
    asdl_seq *نالا_with_defaults; // asdl_seq* of NameDefaultsPair's
} SlashWithDefault;

تعريفنوع بنية {
    arg_ty vararg;
    asdl_seq *kwonlyargs; // asdl_seq* of NameDefaultsPair's
    arg_ty kwarg;
} تارو_Etc;

تعريفنوع بنية { operator_ty قسم; } AugOperator;
تعريفنوع بنية {
    فراغ *element;
    صحيح is_keyword;
} KeywordOrStarred;

تعريفنوع بنية {
    فراغ *نتيجو;
    PyObject *metadata;
} نتيجو_Token_With_Metadata;

// Internal parser functions
#إذا defined(Py_DEBUG)
فراغ Py_Pegen_clear_ياداشت_statistics(فراغ);
PyObject *Py_Pegen_get_ياداشت_statistics(فراغ);
#جيڪڏهن_پورو

صحيح Py_Pegen_وجهو_ياداشت(Parser *p, صحيح mark, صحيح type, فراغ *ڳنڍ);
صحيح Py_Pegen_update_ياداشت(Parser *p, صحيح mark, صحيح type, فراغ *ڳنڍ);
ياداشت *Py_Pegen_وجهو_ياداشت_سڌو(Parser *p, صحيح mark, صحيح type);
صحيح _PyPegen_is_memoized(Parser *p, صحيح type, فراغ *pres);

صحيح _PyPegen_lookahead(صحيح, فراغ *(func)(Parser *), Parser *);
صحيح Py_Pegen_lookahead_for_اظهار(صحيح, اظهار_ty (func)(Parser *), Parser *);
صحيح _PyPegen_lookahead_for_stmt(صحيح, stmt_ty (func)(Parser *), Parser *);
صحيح _PyPegen_lookahead_with_int(صحيح, Token *(func)(Parser *, صحيح), Parser *, صحيح);
صحيح _PyPegen_lookahead_with_string(صحيح, اظهار_ty (func)(Parser *, ثابت حرف*), Parser *, ثابت حرف*);

Token *_PyPegen_expect_token(Parser *p, صحيح type);
فراغ* Py_Pegen_expect_forced_نتيجو(Parser *p, فراغ* نتيجو, ثابت حرف* expected);
Token *_PyPegen_expect_forced_token(Parser *p, صحيح type, ثابت حرف* expected);
اظهار_ty _PyPegen_expect_soft_keyword(Parser *p, ثابت حرف *keyword);
اظهار_ty _PyPegen_soft_keyword_token(Parser *p);
اظهار_ty Py_Pegen_fstring_وچ_token(Parser* p);
Token *Py_Pegen_get_آخري_nonnwhitespace_token(Parser *);
صحيح _PyPegen_fill_token(Parser *p);
// Return -1 on failure, leaving the buffer and capacity unchanged.
صحيح Py_Pegen_grow_loop_بفر(فراغ ***بفر, Py_ssize_t *صلاحيت);
اظهار_ty _PyPegen_name_token(Parser *p);
اظهار_ty Py_Pegen_عدد_token(Parser *p);
فراغ *_PyPegen_string_token(Parser *p);
Py_ssize_t Py_Pegen_byte_وٿي_منزل_character_وٿي_سٽ(PyObject *سٽ, Py_ssize_t کانو_وٿي, Py_ssize_t پڇاڙي_کانو_وٿي);
Py_ssize_t Py_Pegen_byte_وٿي_منزل_character_وٿي(PyObject *سٽ, Py_ssize_t کانو_وٿي);
Py_ssize_t Py_Pegen_byte_وٿي_منزل_character_وٿي_raw(ثابت حرف*, Py_ssize_t کانو_وٿي);

تعريفنوع تعداد {
    تارو_TARGETS,
    DEL_TARGETS,
    FOR_TARGETS,
    SINGLE_TARGETS,
    ATTRIBUTE_OR_SUBSCRIPT_TARGETS
} TARGETS_TYPE;

// Error handling functions and APIs
صحيح _Pypegen_raise_decode_error(Parser *p);
صحيح _Pypegen_tokenizer_error(Parser *p);
فراغ *Py_Pegen_raise_incomplete_داخلا(Parser *p);
فراغ *_PyPegen_raise_error(Parser *p, PyObject *errtype, صحيح use_mark, ثابت حرف *errmsg, ...);
فراغ *Py_Pegen_raise_error_معلوم_location(Parser *p, PyObject *errtype,
                                          Py_ssize_t lineno, Py_ssize_t کانو_وٿي,
                                          Py_ssize_t پڇاڙي_lineno, Py_ssize_t پڇاڙي_کانو_وٿي,
                                          ثابت حرف *errmsg, va_list va);
فراغ Pypegen_سيٽ_syntax_error(Parser* p, Token* آخري_token);
فراغ Pypegen_ڍير_حد_کان_مٿي(Parser *p);

ساكن مضمن فراغ *
RAISE_ERROR_معلوم_LOCATION(Parser *p, PyObject *errtype,
                           Py_ssize_t lineno, Py_ssize_t کانو_وٿي,
                           Py_ssize_t پڇاڙي_lineno, Py_ssize_t پڇاڙي_کانو_وٿي,
                           ثابت حرف *errmsg, ...)
{
    va_list va;
    va_شروعات(va, errmsg);
    Py_ssize_t کانو_وٿي_2 = (کانو_وٿي == CURRENT_POS ? CURRENT_POS : کانو_وٿي + 1);
    Py_ssize_t پڇاڙي_کانو_وٿي_2 = (پڇاڙي_کانو_وٿي == CURRENT_POS ? CURRENT_POS : پڇاڙي_کانو_وٿي + 1);
    Py_Pegen_raise_error_معلوم_location(p, errtype, lineno, کانو_وٿي_2, پڇاڙي_lineno, پڇاڙي_کانو_وٿي_2, errmsg, va);
    va_پڇاڙي(va);
    أرجع عدم;
}
#وصف_ڪريو RAISE_SYNTAX_ERROR(نياپو, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 0, نياپو, ##__VA_ARGS__)
#وصف_ڪريو RAISE_INDENTATION_ERROR(نياپو, ...) _PyPegen_raise_error(p, PyExc_IndentationError, 0, نياپو, ##__VA_ARGS__)
#وصف_ڪريو RAISE_SYNTAX_ERROR_ON_ايندڙ_TOKEN(نياپو, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 1, نياپو, ##__VA_ARGS__)
#وصف_ڪريو RAISE_SYNTAX_ERROR_معلوم_RANGE(a, b, نياپو, ...) \
    RAISE_ERROR_معلوم_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->کانو_وٿي, (b)->پڇاڙي_lineno, (b)->پڇاڙي_کانو_وٿي, نياپو, ##__VA_ARGS__)
#وصف_ڪريو RAISE_SYNTAX_ERROR_معلوم_LOCATION(a, نياپو, ...) \
    RAISE_ERROR_معلوم_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->کانو_وٿي, (a)->پڇاڙي_lineno, (a)->پڇاڙي_کانو_وٿي, نياپو, ##__VA_ARGS__)
#وصف_ڪريو RAISE_SYNTAX_ERROR_STARTING_FROM(a, نياپو, ...) \
    RAISE_ERROR_معلوم_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->کانو_وٿي, CURRENT_POS, CURRENT_POS, نياپو, ##__VA_ARGS__)
#وصف_ڪريو RAISE_SYNTAX_ERROR_INVALID_هدف(type, e) RAISE_SYNTAX_ERROR_INVALID_هدف_2(p, type, e)

Py_LOCAL_INLINE(صحيح)
Py_Pegen_ٽوڪن_are_adjacent(Token *a, Token *b)
{
    أرجع (a->پڇاڙي_lineno == b->lineno) && (a->پڇاڙي_کانو_وٿي == b->کانو_وٿي);
}

Py_LOCAL_INLINE(فراغ *)
CHECK_CALL(Parser *p, فراغ *نتيجو)
{
    إذا (نتيجو == عدم) {
        أكد(PyErr_Occurred());
        p->error_indicator = 1;
    }
    أرجع نتيجو;
}

/* This is needed for helper functions that are allowed to
   return NULL without an error. Example: _PyPegen_seq_extract_starred_exprs */
Py_LOCAL_INLINE(فراغ *)
CHECK_CALL_NULL_ALLOWED(Parser *p, فراغ *نتيجو)
{
    إذا (نتيجو == عدم && PyErr_Occurred()) {
        p->error_indicator = 1;
    }
    أرجع نتيجو;
}

#وصف_ڪريو CHECK(type, نتيجو) ((type) CHECK_CALL(p, نتيجو))
#وصف_ڪريو CHECK_NULL_ALLOWED(type, نتيجو) ((type) CHECK_CALL_NULL_ALLOWED(p, نتيجو))

اظهار_ty Py_Pegen_get_invalid_هدف(اظهار_ty e, TARGETS_TYPE targets_type);
ثابت حرف *Py_Pegen_get_اظهار_name(اظهار_ty);
Py_LOCAL_INLINE(فراغ *)
RAISE_SYNTAX_ERROR_INVALID_هدف_2(Parser *p, TARGETS_TYPE type, فراغ *e)
{
    اظهار_ty invalid_هدف = CHECK_NULL_ALLOWED(اظهار_ty, Py_Pegen_get_invalid_هدف(e, type));
    إذا (invalid_هدف != عدم) {
        ثابت حرف *نياپو;
        إذا (type == تارو_TARGETS || type == FOR_TARGETS) {
            نياپو = "cannot assign to %s";
        }
        وإلا {
            نياپو = "cannot delete %s";
        }
        أرجع RAISE_SYNTAX_ERROR_معلوم_LOCATION(
            invalid_هدف,
            نياپو,
            Py_Pegen_get_اظهار_name(invalid_هدف)
        );
        أرجع RAISE_SYNTAX_ERROR_معلوم_LOCATION(invalid_هدف, "invalid syntax");
    }
    أرجع عدم;
}

// Action utility functions

فراغ *_PyPegen_dummy_name(Parser *p, ...);
فراغ * Py_Pegen_seq_آخري_شي(asdl_seq *seq);
#وصف_ڪريو Py_Pegen_آخري_شي(seq, type) ((type)Py_Pegen_seq_آخري_شي((asdl_seq*)seq))
فراغ * Py_Pegen_seq_پهريون_شي(asdl_seq *seq);
#وصف_ڪريو Py_Pegen_پهريون_شي(seq, type) ((type)Py_Pegen_seq_پهريون_شي((asdl_seq*)seq))
#وصف_ڪريو اڻ_استعمال_ٿيل(اظهار) افعل { (فراغ)(اظهار); } طالما (0)
#وصف_ڪريو EXTRA_اظهار(سر, پڇ) سر->lineno, (سر)->کانو_وٿي, (پڇ)->پڇاڙي_lineno, (پڇ)->پڇاڙي_کانو_وٿي, p->arena
#وصف_ڪريو EXTRA شروعات_lineno, شروعات_کانو_وٿي_2, پڇاڙي_lineno_2, پڇاڙي_کانو_وٿي_2, p->arena
PyObject *_PyPegen_new_type_comment(Parser *, ثابت حرف *);

Py_LOCAL_INLINE(PyObject *)
NEW_TYPE_COMMENT(Parser *p, Token *tc)
{
    إذا (tc == عدم) {
        أرجع عدم;
    }
    ثابت حرف *بائيٽون = Py_بائيٽون_As_String(tc->بائيٽون);
    إذا (بائيٽون == عدم) {
        اذهب error;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, بائيٽون);
    إذا (tco == عدم) {
        اذهب error;
    }
    أرجع tco;
 error:
    p->error_indicator = 1;  // Inline CHECK_CALL
    أرجع عدم;
}

Py_LOCAL_INLINE(فراغ *)
INVALID_نسخو_CHECK(Parser *p, صحيح نسخو, حرف *نياپو, فراغ *ڳنڍ)
{
    إذا (ڳنڍ == عدم) {
        p->error_indicator = 1;  // Inline CHECK_CALL
        أرجع عدم;
    }
    إذا (p->feature_نسخو < نسخو) {
        p->error_indicator = 1;
        أرجع RAISE_SYNTAX_ERROR("%s only supported in Python 3.%i and greater",
                                  نياپو, نسخو);
    }
    أرجع ڳنڍ;
}

#وصف_ڪريو CHECK_نسخو(type, نسخو, نياپو, ڳنڍ) ((type) INVALID_نسخو_CHECK(p, نسخو, نياپو, ڳنڍ))

arg_ty Py_Pegen_جوڙيو_type_comment_منزل_arg(Parser *, arg_ty, Token *);
PyObject *_PyPegen_new_identifier(Parser *, ثابت حرف *);
asdl_seq *_PyPegen_singleton_seq(Parser *, فراغ *);
asdl_seq *Py_Pegen_seq_وجهو_اندريون_front(Parser *, فراغ *, asdl_seq *);
asdl_seq *Py_Pegen_seq_append_منزل_پڇاڙي(Parser *, asdl_seq *, فراغ *);
asdl_seq *_PyPegen_seq_flatten(Parser *, asdl_seq *);
اظهار_ty Py_Pegen_join_نالا_with_dot(Parser *, اظهار_ty, اظهار_ty);
صحيح Py_Pegen_seq_ڳڻپ_dots(asdl_seq *);
alias_ty Py_Pegen_alias_for_تارو(Parser *, صحيح, صحيح, صحيح, صحيح, PyArena *);
asdl_identifier_seq *Py_Pegen_map_نالا_منزل_ids(Parser *, asdl_اظهار_seq *);
Cmpop_اظهار_Pair *Py_Pegen_cmpop_اظهار_pair(Parser *, cmpop_ty, اظهار_ty);
asdl_int_seq *_PyPegen_get_cmpops(Parser *p, asdl_seq *);
asdl_اظهار_seq *_PyPegen_get_exprs(Parser *, asdl_seq *);
اظهار_ty Py_Pegen_سيٽ_اظهار_context(Parser *, اظهار_ty, اظهار_context_ty);
اظهار_ty Py_Pegen_make_هدف(Parser *, اظهار_ty, TARGETS_TYPE);
ڪنجي_قدر_Pair *Py_Pegen_ڪنجي_قدر_pair(Parser *, اظهار_ty, اظهار_ty);
asdl_اظهار_seq *Py_Pegen_get_ڪنجيون(Parser *, asdl_seq *);
asdl_اظهار_seq *Py_Pegen_get_قدرون(Parser *, asdl_seq *);
ڪنجي_نمونو_Pair *Py_Pegen_ڪنجي_نمونو_pair(Parser *, اظهار_ty, نمونو_ty);
asdl_اظهار_seq *Py_Pegen_get_نمونو_ڪنجيون(Parser *, asdl_seq *);
asdl_نمونو_seq *_PyPegen_get_patterns(Parser *, asdl_seq *);
NameDefaultPair *_PyPegen_name_default_pair(Parser *, arg_ty, اظهار_ty, Token *);
SlashWithDefault *_PyPegen_slash_with_default(Parser *, asdl_arg_seq *, asdl_seq *);
تارو_Etc *Py_Pegen_تارو_etc(Parser *, arg_ty, asdl_seq *, arg_ty);
arguments_ty _PyPegen_make_arguments(Parser *, asdl_arg_seq *, SlashWithDefault *,
                                     asdl_arg_seq *, asdl_seq *, تارو_Etc *);
arguments_ty Py_Pegen_خالي_جايون_arguments(Parser *);
اظهار_ty _PyPegen_template_str(Parser *p, Token *a, asdl_اظهار_seq *raw_expressions, Token *b);
اظهار_ty _PyPegen_joined_str(Parser *p, Token *a, asdl_اظهار_seq *raw_expressions, Token *b);
اظهار_ty _PyPegen_interpolation(Parser *, اظهار_ty, Token *, نتيجو_Token_With_Metadata *, نتيجو_Token_With_Metadata *, Token *,
                                 صحيح, صحيح, صحيح, صحيح, PyArena *);
اظهار_ty Py_Pegen_formatted_قدر(Parser *, اظهار_ty, Token *, نتيجو_Token_With_Metadata *, نتيجو_Token_With_Metadata *, Token *,
                                 صحيح, صحيح, صحيح, صحيح, PyArena *);
AugOperator *_PyPegen_augoperator(Parser*, operator_ty type);
stmt_ty _PyPegen_function_def_decorators(Parser *, asdl_اظهار_seq *, stmt_ty);
stmt_ty _PyPegen_class_def_decorators(Parser *, asdl_اظهار_seq *, stmt_ty);
KeywordOrStarred *_PyPegen_keyword_or_starred(Parser *, فراغ *, صحيح);
asdl_اظهار_seq *Py_Pegen_seq_ڪڍو_starred_exprs(Parser *, asdl_seq *);
asdl_keyword_seq *_PyPegen_seq_delete_starred_exprs(Parser *, asdl_seq *);
اظهار_ty _PyPegen_collect_call_seqs(Parser *, asdl_اظهار_seq *, asdl_seq *,
                     صحيح lineno, صحيح کانو_وٿي, صحيح پڇاڙي_lineno,
                     صحيح پڇاڙي_کانو_وٿي, PyArena *arena);
اظهار_ty _PyPegen_constant_from_token(Parser* p, Token* tok);
اظهار_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok);
اظهار_ty _PyPegen_constant_from_string(Parser* p, Token* tok);
اظهار_ty _PyPegen_concatenate_tstrings(Parser *p, asdl_اظهار_seq *, صحيح, صحيح, صحيح, صحيح, PyArena *);
اظهار_ty Py_Pegen_concatenate_لڙيون(Parser *p, asdl_اظهار_seq *, صحيح, صحيح, صحيح, صحيح, PyArena *);
اظهار_ty _PyPegen_FetchRawForm(Parser *p, صحيح, صحيح, صحيح, صحيح);
اظهار_ty _PyPegen_ensure_imaginary(Parser *p, اظهار_ty);
اظهار_ty _PyPegen_ensure_real(Parser *p, اظهار_ty);
asdl_seq *_PyPegen_join_sequences(Parser *, asdl_seq *, asdl_seq *);
صحيح _PyPegen_check_barry_as_flufl(Parser *, Token *);
صحيح _PyPegen_check_legacy_stmt(Parser *p, اظهار_ty t);
فراغ *_PyPegen_raise_error_for_missing_comma(Parser *p, اظهار_ty a, اظهار_ty b);
نتيجو_Token_With_Metadata *_PyPegen_check_fstring_conversion(Parser *p, Token *, اظهار_ty t);
نتيجو_Token_With_Metadata *Py_Pegen_تياري_ڀريل_format_spec(Parser *, Token *, asdl_اظهار_seq *, صحيح, صحيح,
                                                         صحيح, صحيح, PyArena *);
mod_ty _PyPegen_make_module(Parser *, asdl_stmt_seq *);
فراغ *_PyPegen_arguments_parsing_error(Parser *, اظهار_ty);
اظهار_ty Py_Pegen_get_آخري_comprehension_شي(comprehension_ty comprehension);
فراغ *Py_Pegen_nonparen_genexp_اندريون_call(Parser *p, اظهار_ty دليلون, asdl_comprehension_seq *comprehensions);
stmt_ty _PyPegen_checked_from_import(Parser *p, asdl_seq *dots, اظهار_ty module_name,
                                     asdl_alias_seq *نالا, اظهار_ty lazy_token, صحيح lineno,
                                     صحيح کانو_وٿي, صحيح پڇاڙي_lineno, صحيح پڇاڙي_کانو_وٿي,
                                     PyArena *arena);
asdl_stmt_seq* Py_Pegen_رجسٽر_stmts(Parser *p, asdl_stmt_seq* stmts);
stmt_ty Py_Pegen_رجسٽر_stmt(Parser *p, stmt_ty s);

// Parser API

Parser *_PyPegen_Parser_New(بنية tok_state *, صحيح, صحيح, صحيح, صحيح *, ثابت حرف*, PyArena *);
فراغ _PyPegen_Parser_Free(Parser *);
mod_ty Py_Pegen_لاڳيتو_parser_from_file_اشارو(FILE *, صحيح, PyObject *, ثابت حرف *,
                                    ثابت حرف *, ثابت حرف *, PyCompilerFlags *, صحيح *, PyObject **,
                                    PyArena *);
فراغ *Py_Pegen_لاڳيتو_parser(Parser *);
mod_ty Py_Pegen_لاڳيتو_parser_from_string(ثابت حرف *, صحيح, PyObject *, PyCompilerFlags *, PyArena *, PyObject *, PyObject **);
asdl_stmt_seq *_PyPegen_interactive_exit(Parser *);

// Generated function in parse.c - function definition in python.gram
فراغ *_PyPegen_parse(Parser *);

#جيڪڏهن_پورو
