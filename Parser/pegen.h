#که_نه_تعریف_شوی PEGEN_H
#تعریف_کړه PEGEN_H

#ضمن <Python.h>
#ضمن <pycore_ast.h>
#ضمن <pycore_token.h>
#ضمن <pycore_ceval.h>     // _Py_ReachedRecursionLimitWithMargin()
#ضمن <pycore_pystate.h>   // _Py_get_machine_stack_pointer()
#ضمن <pycore_pythonrun.h> // _PyOS_STACK_MARGIN_BYTES

#ضمن "tokenizer/tokenizer.h"

#إذا 0
#تعریف_کړه PyPARSE_YIELD_IS_KEYWORD        0x0001
#د_که_پای

#تعریف_کړه PyPARSE_DONT_IMPLY_DEDENT       0x0002

#إذا 0
#تعریف_کړه PyPARSE_WITH_IS_KEYWORD         0x0003
#تعریف_کړه PyPARSE_PRINT_IS_FUNCTION       0x0004
#تعریف_کړه PyPARSE_UNICODE_LITERALS        0x0008
#د_که_پای

#تعریف_کړه PyPARSE_IGNORE_COOKIE 0x0010
#تعریف_کړه PyPARSE_BARRY_AS_BDFL 0x0020
#تعریف_کړه PyPARSE_TYPE_COMMENTS 0x0040
#تعریف_کړه Py_PARSE_ALLOW_INCOMPLETE_ننوت 0x0100

#تعریف_کړه CURRENT_POS (-5)

تعريفنوع بنية یادښت_2 {
    صحيح type;
    فراغ *غوټه;
    صحيح mark;
    بنية یادښت_2 *بل;
} یادښت;

تعريفنوع بنية {
    صحيح type;
    // Text required by grammar actions or helpers; otherwise NULL.
    // An unfinished string token also has NULL text. Its grammar action
    // requests more input only if the token is valid at that position.
    PyObject *بایټونه;
    صحيح level;
    صحيح is_raw;
    صحيح lineno, ستون_دننه_واټن, پای_ټکی_lineno, پای_ټکی_ستون_دننه_واټن;
    یادښت *یادښت_3;
    // Filter over the rule types present in `memo` (bit `type & 63` is set
    // for every entry): lets lookups skip walking the list on definite
    // misses, which are the common case.
    uint64_t یادښت_ماسک;
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
    } *توکي;
    اندازه_t اندازه;
    اندازه_t عدد_توکي;
} growable_comment_array;

تعريفنوع بنية {
    صحيح lineno;
    صحيح ستون_دننه_واټن;
    صحيح پای_ټکی_lineno;
    صحيح پای_ټکی_ستون_دننه_واټن;
} location;

تعريفنوع بنية identifier_cache_ثبت Identifier_Cache_ثبت;

تعريفنوع بنية {
    بنية tok_state *tok;
    Token **نښې;
    صحيح mark;
    صحيح fill, اندازه;
    PyArena *arena;
    KeywordToken **keywords;
    حرف **soft_keywords;
    صحيح n_keyword_lists;
    صحيح پیل_قاعده;
    صحيح *errcode;
    بینښه حرف parsing_started;
    بینښه حرف ننوت_exhausted;
    PyObject* normalize;
    صحيح starting_lineno;
    صحيح starting_ستون_دننه_واټن;
    صحيح error_indicator;
    صحيح flags;
    صحيح feature_نسخه;
    growable_comment_array type_ignore_comments;
    Token *معلوم_err_token;
    صحيح level;
    صحيح call_invalid_قواعد;
    صحيح debug;
    location وروستی_stmt_location;
    Identifier_Cache_ثبت *identifier_cache;
    PyThreadState *tstate;
    // Cached copy of tstate->c_stack_soft_limit offset by one margin toward
    // the stack top, so the per-rule stack check is a single inline comparison.
    uintptr_t ډېری_soft_حد;
} Parser;

// C-stack exhaustion check run on every rule entry. The fast path compares
// the machine stack pointer against the soft limit cached in the Parser;
// only when the stack is nearly exhausted is the full check called.
ساكن مضمن صحيح
Py_Pegen_ډېری_exhausted(Parser *p)
{
#إذا Py_ډېری_GROWS_ښکته
    إذا (Py_get_machine_ډېری_اشاره_ګر() > p->ډېری_soft_حد) {
#وإلا
    إذا (Py_get_machine_ډېری_اشاره_ګر() <= p->ډېری_soft_حد) {
#د_که_پای
        أكد(!Py_Reached_Recursion_حد_With_Margin(p->tstate, 1));
        أرجع 0;
    }
    أرجع Py_Reached_Recursion_حد_With_Margin(p->tstate, 1);
}

تعريفنوع بنية {
    cmpop_ty cmpop;
    بیان_ty بیان;
} Cmpop_بیان_Pair;

تعريفنوع بنية {
    بیان_ty کلی;
    بیان_ty ارزښت;
} کلی_ارزښت_Pair;

تعريفنوع بنية {
    بیان_ty کلی;
    نمونه_ty نمونه;
} کلی_نمونه_Pair;

تعريفنوع بنية {
    arg_ty arg;
    بیان_ty ارزښت;
} NameDefaultPair;

تعريفنوع بنية {
    asdl_arg_seq *plain_نومونه;
    asdl_seq *نومونه_with_defaults; // asdl_seq* of NameDefaultsPair's
} SlashWithDefault;

تعريفنوع بنية {
    arg_ty vararg;
    asdl_seq *kwonlyargs; // asdl_seq* of NameDefaultsPair's
    arg_ty kwarg;
} ستوری_Etc;

تعريفنوع بنية { operator_ty ډول; } AugOperator;
تعريفنوع بنية {
    فراغ *element;
    صحيح is_keyword;
} KeywordOrStarred;

تعريفنوع بنية {
    فراغ *پایله;
    PyObject *metadata;
} پایله_Token_With_Metadata;

// Internal parser functions
#إذا defined(Py_DEBUG)
فراغ Py_Pegen_clear_یادښت_statistics(فراغ);
PyObject *Py_Pegen_get_یادښت_statistics(فراغ);
#د_که_پای

صحيح Py_Pegen_ورننباسه_یادښت(Parser *p, صحيح mark, صحيح type, فراغ *غوټه);
صحيح Py_Pegen_update_یادښت(Parser *p, صحيح mark, صحيح type, فراغ *غوټه);
یادښت *Py_Pegen_ورننباسه_یادښت_مستقیم(Parser *p, صحيح mark, صحيح type);
صحيح _PyPegen_is_memoized(Parser *p, صحيح type, فراغ *pres);

صحيح _PyPegen_lookahead(صحيح, فراغ *(func)(Parser *), Parser *);
صحيح Py_Pegen_lookahead_for_بیان(صحيح, بیان_ty (func)(Parser *), Parser *);
صحيح _PyPegen_lookahead_for_stmt(صحيح, stmt_ty (func)(Parser *), Parser *);
صحيح _PyPegen_lookahead_with_int(صحيح, Token *(func)(Parser *, صحيح), Parser *, صحيح);
صحيح _PyPegen_lookahead_with_string(صحيح, بیان_ty (func)(Parser *, ثابت حرف*), Parser *, ثابت حرف*);

Token *_PyPegen_expect_token(Parser *p, صحيح type);
فراغ* Py_Pegen_expect_forced_پایله(Parser *p, فراغ* پایله, ثابت حرف* expected);
Token *_PyPegen_expect_forced_token(Parser *p, صحيح type, ثابت حرف* expected);
بیان_ty _PyPegen_expect_soft_keyword(Parser *p, ثابت حرف *keyword);
بیان_ty _PyPegen_soft_keyword_token(Parser *p);
بیان_ty Py_Pegen_fstring_منځ_token(Parser* p);
Token *Py_Pegen_get_وروستی_nonnwhitespace_token(Parser *);
صحيح _PyPegen_fill_token(Parser *p);
// Return -1 on failure, leaving the buffer and capacity unchanged.
صحيح Py_Pegen_grow_loop_بفر(فراغ ***بفر, Py_ssize_t *ظرفیت);
بیان_ty _PyPegen_name_token(Parser *p);
بیان_ty Py_Pegen_شمېره_token(Parser *p);
فراغ *_PyPegen_string_token(Parser *p);
Py_ssize_t Py_Pegen_byte_دننه_واټن_منزل_character_دننه_واټن_کرښه(PyObject *کرښه, Py_ssize_t ستون_دننه_واټن, Py_ssize_t پای_ټکی_ستون_دننه_واټن);
Py_ssize_t Py_Pegen_byte_دننه_واټن_منزل_character_دننه_واټن(PyObject *کرښه, Py_ssize_t ستون_دننه_واټن);
Py_ssize_t Py_Pegen_byte_دننه_واټن_منزل_character_دننه_واټن_raw(ثابت حرف*, Py_ssize_t ستون_دننه_واټن);

تعريفنوع تعداد {
    ستوری_TARGETS,
    DEL_TARGETS,
    FOR_TARGETS,
    SINGLE_TARGETS,
    ATTRIBUTE_OR_SUBSCRIPT_TARGETS
} TARGETS_TYPE;

// Error handling functions and APIs
صحيح _Pypegen_raise_decode_error(Parser *p);
صحيح _Pypegen_tokenizer_error(Parser *p);
فراغ *Py_Pegen_raise_incomplete_ننوت(Parser *p);
فراغ *_PyPegen_raise_error(Parser *p, PyObject *errtype, صحيح use_mark, ثابت حرف *errmsg, ...);
فراغ *Py_Pegen_raise_error_معلوم_location(Parser *p, PyObject *errtype,
                                          Py_ssize_t lineno, Py_ssize_t ستون_دننه_واټن,
                                          Py_ssize_t پای_ټکی_lineno, Py_ssize_t پای_ټکی_ستون_دننه_واټن,
                                          ثابت حرف *errmsg, va_list va);
فراغ Pypegen_سېټ_syntax_error(Parser* p, Token* وروستی_token);
فراغ Pypegen_ډېری_ډکېدنه(Parser *p);

ساكن مضمن فراغ *
RAISE_ERROR_معلوم_LOCATION(Parser *p, PyObject *errtype,
                           Py_ssize_t lineno, Py_ssize_t ستون_دننه_واټن,
                           Py_ssize_t پای_ټکی_lineno, Py_ssize_t پای_ټکی_ستون_دننه_واټن,
                           ثابت حرف *errmsg, ...)
{
    va_list va;
    va_پیل(va, errmsg);
    Py_ssize_t ستون_دننه_واټن_2 = (ستون_دننه_واټن == CURRENT_POS ? CURRENT_POS : ستون_دننه_واټن + 1);
    Py_ssize_t پای_ټکی_ستون_دننه_واټن_2 = (پای_ټکی_ستون_دننه_واټن == CURRENT_POS ? CURRENT_POS : پای_ټکی_ستون_دننه_واټن + 1);
    Py_Pegen_raise_error_معلوم_location(p, errtype, lineno, ستون_دننه_واټن_2, پای_ټکی_lineno, پای_ټکی_ستون_دننه_واټن_2, errmsg, va);
    va_پای_ټکی(va);
    أرجع عدم;
}
#تعریف_کړه RAISE_SYNTAX_ERROR(پیغام, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 0, پیغام, ##__VA_ARGS__)
#تعریف_کړه RAISE_INDENTATION_ERROR(پیغام, ...) _PyPegen_raise_error(p, PyExc_IndentationError, 0, پیغام, ##__VA_ARGS__)
#تعریف_کړه RAISE_SYNTAX_ERROR_ON_بل_TOKEN(پیغام, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 1, پیغام, ##__VA_ARGS__)
#تعریف_کړه RAISE_SYNTAX_ERROR_معلوم_RANGE(a, b, پیغام, ...) \
    RAISE_ERROR_معلوم_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->ستون_دننه_واټن, (b)->پای_ټکی_lineno, (b)->پای_ټکی_ستون_دننه_واټن, پیغام, ##__VA_ARGS__)
#تعریف_کړه RAISE_SYNTAX_ERROR_معلوم_LOCATION(a, پیغام, ...) \
    RAISE_ERROR_معلوم_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->ستون_دننه_واټن, (a)->پای_ټکی_lineno, (a)->پای_ټکی_ستون_دننه_واټن, پیغام, ##__VA_ARGS__)
#تعریف_کړه RAISE_SYNTAX_ERROR_STARTING_FROM(a, پیغام, ...) \
    RAISE_ERROR_معلوم_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->ستون_دننه_واټن, CURRENT_POS, CURRENT_POS, پیغام, ##__VA_ARGS__)
#تعریف_کړه RAISE_SYNTAX_ERROR_INVALID_موخه(type, e) RAISE_SYNTAX_ERROR_INVALID_موخه_2(p, type, e)

Py_LOCAL_INLINE(صحيح)
Py_Pegen_نښې_are_adjacent(Token *a, Token *b)
{
    أرجع (a->پای_ټکی_lineno == b->lineno) && (a->پای_ټکی_ستون_دننه_واټن == b->ستون_دننه_واټن);
}

Py_LOCAL_INLINE(فراغ *)
CHECK_CALL(Parser *p, فراغ *پایله)
{
    إذا (پایله == عدم) {
        أكد(PyErr_Occurred());
        p->error_indicator = 1;
    }
    أرجع پایله;
}

/* This is needed for helper functions that are allowed to
   return NULL without an error. Example: _PyPegen_seq_extract_starred_exprs */
Py_LOCAL_INLINE(فراغ *)
CHECK_CALL_NULL_ALLOWED(Parser *p, فراغ *پایله)
{
    إذا (پایله == عدم && PyErr_Occurred()) {
        p->error_indicator = 1;
    }
    أرجع پایله;
}

#تعریف_کړه CHECK(type, پایله) ((type) CHECK_CALL(p, پایله))
#تعریف_کړه CHECK_NULL_ALLOWED(type, پایله) ((type) CHECK_CALL_NULL_ALLOWED(p, پایله))

بیان_ty Py_Pegen_get_invalid_موخه(بیان_ty e, TARGETS_TYPE targets_type);
ثابت حرف *Py_Pegen_get_بیان_name(بیان_ty);
Py_LOCAL_INLINE(فراغ *)
RAISE_SYNTAX_ERROR_INVALID_موخه_2(Parser *p, TARGETS_TYPE type, فراغ *e)
{
    بیان_ty invalid_موخه = CHECK_NULL_ALLOWED(بیان_ty, Py_Pegen_get_invalid_موخه(e, type));
    إذا (invalid_موخه != عدم) {
        ثابت حرف *پیغام;
        إذا (type == ستوری_TARGETS || type == FOR_TARGETS) {
            پیغام = "cannot assign to %s";
        }
        وإلا {
            پیغام = "cannot delete %s";
        }
        أرجع RAISE_SYNTAX_ERROR_معلوم_LOCATION(
            invalid_موخه,
            پیغام,
            Py_Pegen_get_بیان_name(invalid_موخه)
        );
        أرجع RAISE_SYNTAX_ERROR_معلوم_LOCATION(invalid_موخه, "invalid syntax");
    }
    أرجع عدم;
}

// Action utility functions

فراغ *_PyPegen_dummy_name(Parser *p, ...);
فراغ * Py_Pegen_seq_وروستی_توکی(asdl_seq *seq);
#تعریف_کړه Py_Pegen_وروستی_توکی(seq, type) ((type)Py_Pegen_seq_وروستی_توکی((asdl_seq*)seq))
فراغ * Py_Pegen_seq_لومړی_توکی(asdl_seq *seq);
#تعریف_کړه Py_Pegen_لومړی_توکی(seq, type) ((type)Py_Pegen_seq_لومړی_توکی((asdl_seq*)seq))
#تعریف_کړه ناکارول_شوی(بیان) افعل { (فراغ)(بیان); } طالما (0)
#تعریف_کړه EXTRA_بیان(سر, لکۍ) سر->lineno, (سر)->ستون_دننه_واټن, (لکۍ)->پای_ټکی_lineno, (لکۍ)->پای_ټکی_ستون_دننه_واټن, p->arena
#تعریف_کړه EXTRA پیل_lineno, پیل_ستون_دننه_واټن_2, پای_ټکی_lineno_2, پای_ټکی_ستون_دننه_واټن_2, p->arena
PyObject *_PyPegen_new_type_comment(Parser *, ثابت حرف *);

Py_LOCAL_INLINE(PyObject *)
NEW_TYPE_COMMENT(Parser *p, Token *tc)
{
    إذا (tc == عدم) {
        أرجع عدم;
    }
    ثابت حرف *بایټونه = Py_بایټونه_As_String(tc->بایټونه);
    إذا (بایټونه == عدم) {
        اذهب error;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, بایټونه);
    إذا (tco == عدم) {
        اذهب error;
    }
    أرجع tco;
 error:
    p->error_indicator = 1;  // Inline CHECK_CALL
    أرجع عدم;
}

Py_LOCAL_INLINE(فراغ *)
INVALID_نسخه_CHECK(Parser *p, صحيح نسخه, حرف *پیغام, فراغ *غوټه)
{
    إذا (غوټه == عدم) {
        p->error_indicator = 1;  // Inline CHECK_CALL
        أرجع عدم;
    }
    إذا (p->feature_نسخه < نسخه) {
        p->error_indicator = 1;
        أرجع RAISE_SYNTAX_ERROR("%s only supported in Python 3.%i and greater",
                                  پیغام, نسخه);
    }
    أرجع غوټه;
}

#تعریف_کړه CHECK_نسخه(type, نسخه, پیغام, غوټه) ((type) INVALID_نسخه_CHECK(p, نسخه, پیغام, غوټه))

arg_ty Py_Pegen_جمع_کړه_type_comment_منزل_arg(Parser *, arg_ty, Token *);
PyObject *_PyPegen_new_identifier(Parser *, ثابت حرف *);
asdl_seq *_PyPegen_singleton_seq(Parser *, فراغ *);
asdl_seq *Py_Pegen_seq_ورننباسه_دننه_front(Parser *, فراغ *, asdl_seq *);
asdl_seq *Py_Pegen_seq_append_منزل_پای_ټکی(Parser *, asdl_seq *, فراغ *);
asdl_seq *_PyPegen_seq_flatten(Parser *, asdl_seq *);
بیان_ty Py_Pegen_join_نومونه_with_dot(Parser *, بیان_ty, بیان_ty);
صحيح Py_Pegen_seq_شمېر_dots(asdl_seq *);
alias_ty Py_Pegen_alias_for_ستوری(Parser *, صحيح, صحيح, صحيح, صحيح, PyArena *);
asdl_identifier_seq *Py_Pegen_map_نومونه_منزل_ids(Parser *, asdl_بیان_seq *);
Cmpop_بیان_Pair *Py_Pegen_cmpop_بیان_pair(Parser *, cmpop_ty, بیان_ty);
asdl_int_seq *_PyPegen_get_cmpops(Parser *p, asdl_seq *);
asdl_بیان_seq *_PyPegen_get_exprs(Parser *, asdl_seq *);
بیان_ty Py_Pegen_سېټ_بیان_context(Parser *, بیان_ty, بیان_context_ty);
بیان_ty Py_Pegen_make_موخه(Parser *, بیان_ty, TARGETS_TYPE);
کلی_ارزښت_Pair *Py_Pegen_کلی_ارزښت_pair(Parser *, بیان_ty, بیان_ty);
asdl_بیان_seq *Py_Pegen_get_کلیګانې(Parser *, asdl_seq *);
asdl_بیان_seq *Py_Pegen_get_ارزښتونه(Parser *, asdl_seq *);
کلی_نمونه_Pair *Py_Pegen_کلی_نمونه_pair(Parser *, بیان_ty, نمونه_ty);
asdl_بیان_seq *Py_Pegen_get_نمونه_کلیګانې(Parser *, asdl_seq *);
asdl_نمونه_seq *_PyPegen_get_patterns(Parser *, asdl_seq *);
NameDefaultPair *_PyPegen_name_default_pair(Parser *, arg_ty, بیان_ty, Token *);
SlashWithDefault *_PyPegen_slash_with_default(Parser *, asdl_arg_seq *, asdl_seq *);
ستوری_Etc *Py_Pegen_ستوری_etc(Parser *, arg_ty, asdl_seq *, arg_ty);
arguments_ty _PyPegen_make_arguments(Parser *, asdl_arg_seq *, SlashWithDefault *,
                                     asdl_arg_seq *, asdl_seq *, ستوری_Etc *);
arguments_ty Py_Pegen_تشه_arguments(Parser *);
بیان_ty _PyPegen_template_str(Parser *p, Token *a, asdl_بیان_seq *raw_expressions, Token *b);
بیان_ty _PyPegen_joined_str(Parser *p, Token *a, asdl_بیان_seq *raw_expressions, Token *b);
بیان_ty _PyPegen_interpolation(Parser *, بیان_ty, Token *, پایله_Token_With_Metadata *, پایله_Token_With_Metadata *, Token *,
                                 صحيح, صحيح, صحيح, صحيح, PyArena *);
بیان_ty Py_Pegen_formatted_ارزښت(Parser *, بیان_ty, Token *, پایله_Token_With_Metadata *, پایله_Token_With_Metadata *, Token *,
                                 صحيح, صحيح, صحيح, صحيح, PyArena *);
AugOperator *_PyPegen_augoperator(Parser*, operator_ty type);
stmt_ty _PyPegen_function_def_decorators(Parser *, asdl_بیان_seq *, stmt_ty);
stmt_ty _PyPegen_class_def_decorators(Parser *, asdl_بیان_seq *, stmt_ty);
KeywordOrStarred *_PyPegen_keyword_or_starred(Parser *, فراغ *, صحيح);
asdl_بیان_seq *Py_Pegen_seq_ایستل_starred_exprs(Parser *, asdl_seq *);
asdl_keyword_seq *_PyPegen_seq_delete_starred_exprs(Parser *, asdl_seq *);
بیان_ty _PyPegen_collect_call_seqs(Parser *, asdl_بیان_seq *, asdl_seq *,
                     صحيح lineno, صحيح ستون_دننه_واټن, صحيح پای_ټکی_lineno,
                     صحيح پای_ټکی_ستون_دننه_واټن, PyArena *arena);
بیان_ty _PyPegen_constant_from_token(Parser* p, Token* tok);
بیان_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok);
بیان_ty _PyPegen_constant_from_string(Parser* p, Token* tok);
بیان_ty _PyPegen_concatenate_tstrings(Parser *p, asdl_بیان_seq *, صحيح, صحيح, صحيح, صحيح, PyArena *);
بیان_ty Py_Pegen_concatenate_مزي_ګانې(Parser *p, asdl_بیان_seq *, صحيح, صحيح, صحيح, صحيح, PyArena *);
بیان_ty _PyPegen_FetchRawForm(Parser *p, صحيح, صحيح, صحيح, صحيح);
بیان_ty _PyPegen_ensure_imaginary(Parser *p, بیان_ty);
بیان_ty _PyPegen_ensure_real(Parser *p, بیان_ty);
asdl_seq *_PyPegen_join_sequences(Parser *, asdl_seq *, asdl_seq *);
صحيح _PyPegen_check_barry_as_flufl(Parser *, Token *);
صحيح _PyPegen_check_legacy_stmt(Parser *p, بیان_ty t);
فراغ *_PyPegen_raise_error_for_missing_comma(Parser *p, بیان_ty a, بیان_ty b);
پایله_Token_With_Metadata *_PyPegen_check_fstring_conversion(Parser *p, Token *, بیان_ty t);
پایله_Token_With_Metadata *Py_Pegen_تیاری_ډک_format_spec(Parser *, Token *, asdl_بیان_seq *, صحيح, صحيح,
                                                         صحيح, صحيح, PyArena *);
mod_ty _PyPegen_make_module(Parser *, asdl_stmt_seq *);
فراغ *_PyPegen_arguments_parsing_error(Parser *, بیان_ty);
بیان_ty Py_Pegen_get_وروستی_comprehension_توکی(comprehension_ty comprehension);
فراغ *Py_Pegen_nonparen_genexp_دننه_call(Parser *p, بیان_ty دلیلونه, asdl_comprehension_seq *comprehensions);
stmt_ty _PyPegen_checked_from_import(Parser *p, asdl_seq *dots, بیان_ty module_name,
                                     asdl_alias_seq *نومونه, بیان_ty lazy_token, صحيح lineno,
                                     صحيح ستون_دننه_واټن, صحيح پای_ټکی_lineno, صحيح پای_ټکی_ستون_دننه_واټن,
                                     PyArena *arena);
asdl_stmt_seq* Py_Pegen_ثبتوونکی_stmts(Parser *p, asdl_stmt_seq* stmts);
stmt_ty Py_Pegen_ثبتوونکی_stmt(Parser *p, stmt_ty s);

// Parser API

Parser *_PyPegen_Parser_New(بنية tok_state *, صحيح, صحيح, صحيح, صحيح *, ثابت حرف*, PyArena *);
فراغ _PyPegen_Parser_Free(Parser *);
mod_ty Py_Pegen_پرلپسې_parser_from_file_اشاره_ګر(FILE *, صحيح, PyObject *, ثابت حرف *,
                                    ثابت حرف *, ثابت حرف *, PyCompilerFlags *, صحيح *, PyObject **,
                                    PyArena *);
فراغ *Py_Pegen_پرلپسې_parser(Parser *);
mod_ty Py_Pegen_پرلپسې_parser_from_string(ثابت حرف *, صحيح, PyObject *, PyCompilerFlags *, PyArena *, PyObject *, PyObject **);
asdl_stmt_seq *_PyPegen_interactive_exit(Parser *);

// Generated function in parse.c - function definition in python.gram
فراغ *_PyPegen_parse(Parser *);

#د_که_پای
