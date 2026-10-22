#ifndef PEGEN_H
#define PEGEN_H

#ضمن <Python.h>
#ضمن <pycore_ast.h>
#ضمن <pycore_token.h>
#ضمن <pycore_ceval.h>     // _Py_ReachedRecursionLimitWithMargin()
#ضمن <pycore_pystate.h>   // _Py_get_machine_stack_pointer()
#ضمن <pycore_pythonrun.h> // _PyOS_STACK_MARGIN_BYTES

#ضمن "tokenizer/tokenizer.h"

#إذا 0
#define PyPARSE_YIELD_IS_KEYWORD        0x0001
#endif

#define PyPARSE_DONT_IMPLY_DEDENT       0x0002

#إذا 0
#define PyPARSE_WITH_IS_KEYWORD         0x0003
#define Py_PARSE_اطبع_IS_FUNCTION       0x0004
#define PyPARSE_UNICODE_LITERALS        0x0008
#endif

#define PyPARSE_IGNORE_COOKIE 0x0010
#define PyPARSE_BARRY_AS_BDFL 0x0020
#define PyPARSE_TYPE_COMMENTS 0x0040
#define Py_PARSE_ALLOW_INCOMPLETE_مدخل 0x0100

#define CURRENT_POS (-5)

تعريفنوع بنية _memo {
    صحيح type;
    فراغ *عقدة;
    صحيح mark;
    بنية _memo *next;
} Memo;

تعريفنوع بنية {
    صحيح type;
    // Text required by grammar actions or helpers; otherwise NULL.
    // An unfinished string token also has NULL text. Its grammar action
    // requests more input only if the token is valid at that position.
    PyObject *bytes;
    صحيح level;
    صحيح is_raw;
    صحيح lineno, col_offset, end_lineno, end_col_offset;
    Memo *memo;
    // Filter over the rule types present in `memo` (bit `type & 63` is set
    // for every entry): lets lookups skip walking the list on definite
    // misses, which are the common case.
    uint64_t memo_mask;
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
    } *عناصر;
    size_t size;
    size_t رقم_عناصر;
} growable_comment_array;

تعريفنوع بنية {
    صحيح lineno;
    صحيح col_offset;
    صحيح end_lineno;
    صحيح end_col_offset;
} location;

تعريفنوع بنية _identifier_cache_entry IdentifierCacheEntry;

تعريفنوع بنية {
    بنية tok_state *tok;
    Token **tokens;
    صحيح mark;
    صحيح fill, size;
    PyArena *arena;
    KeywordToken **keywords;
    حرف **soft_keywords;
    صحيح n_keyword_lists;
    صحيح start_rule;
    صحيح *errcode;
    unsigned حرف parsing_started;
    unsigned حرف مدخل_exhausted;
    PyObject* normalize;
    صحيح starting_lineno;
    صحيح starting_col_offset;
    صحيح error_indicator;
    صحيح flags;
    صحيح feature_version;
    growable_comment_array type_ignore_comments;
    Token *known_err_token;
    صحيح level;
    صحيح call_invalid_rules;
    صحيح debug;
    location last_stmt_location;
    IdentifierCacheEntry *identifier_cache;
    PyThreadState *tstate;
    // Cached copy of tstate->c_stack_soft_limit offset by one margin toward
    // the stack top, so the per-rule stack check is a single inline comparison.
    uintptr_t مكدس_soft_limit;
} Parser;

// C-stack exhaustion check run on every rule entry. The fast path compares
// the machine stack pointer against the soft limit cached in the Parser;
// only when the stack is nearly exhausted is the full check called.
ساكن مضمن صحيح
Py_Pegen_مكدس_exhausted(Parser *p)
{
#إذا Py_مكدس_GROWS_DOWN
    إذا (Py_get_machine_مكدس_pointer() > p->مكدس_soft_limit) {
#وإلا
    إذا (Py_get_machine_مكدس_pointer() <= p->مكدس_soft_limit) {
#endif
        أكد(!_Py_ReachedRecursionLimitWithMargin(p->tstate, 1));
        أرجع 0;
    }
    أرجع _Py_ReachedRecursionLimitWithMargin(p->tstate, 1);
}

تعريفنوع بنية {
    cmpop_ty cmpop;
    تعبير_ty تعبير;
} Cmpop_تعبير_Pair;

تعريفنوع بنية {
    تعبير_ty مفتاح;
    تعبير_ty قيمة;
} مفتاح_قيمة_Pair;

تعريفنوع بنية {
    تعبير_ty مفتاح;
    pattern_ty pattern;
} مفتاح_Pattern_Pair;

تعريفنوع بنية {
    arg_ty arg;
    تعبير_ty قيمة;
} NameDefaultPair;

تعريفنوع بنية {
    asdl_arg_seq *plain_names;
    asdl_seq *names_with_defaults; // asdl_seq* of NameDefaultsPair's
} SlashWithDefault;

تعريفنوع بنية {
    arg_ty vararg;
    asdl_seq *kwonlyargs; // asdl_seq* of NameDefaultsPair's
    arg_ty kwarg;
} StarEtc;

تعريفنوع بنية { operator_ty kind; } AugOperator;
تعريفنوع بنية {
    فراغ *element;
    صحيح is_keyword;
} KeywordOrStarred;

تعريفنوع بنية {
    فراغ *result;
    PyObject *metadata;
} ResultTokenWithMetadata;

// Internal parser functions
#إذا defined(Py_DEBUG)
فراغ _PyPegen_clear_memo_statistics(فراغ);
PyObject *_PyPegen_get_memo_statistics(فراغ);
#endif

صحيح Py_Pegen_أدرج_memo(Parser *p, صحيح mark, صحيح type, فراغ *عقدة);
صحيح _PyPegen_update_memo(Parser *p, صحيح mark, صحيح type, فراغ *عقدة);
Memo *Py_Pegen_أدرج_memo_direct(Parser *p, صحيح mark, صحيح type);
صحيح _PyPegen_is_memoized(Parser *p, صحيح type, فراغ *pres);

صحيح _PyPegen_lookahead(صحيح, فراغ *(func)(Parser *), Parser *);
صحيح Py_Pegen_lookahead_for_تعبير(صحيح, تعبير_ty (func)(Parser *), Parser *);
صحيح _PyPegen_lookahead_for_stmt(صحيح, stmt_ty (func)(Parser *), Parser *);
صحيح _PyPegen_lookahead_with_int(صحيح, Token *(func)(Parser *, صحيح), Parser *, صحيح);
صحيح _PyPegen_lookahead_with_string(صحيح, تعبير_ty (func)(Parser *, ثابت حرف*), Parser *, ثابت حرف*);

Token *_PyPegen_expect_token(Parser *p, صحيح type);
فراغ* _PyPegen_expect_forced_result(Parser *p, فراغ* result, ثابت حرف* expected);
Token *_PyPegen_expect_forced_token(Parser *p, صحيح type, ثابت حرف* expected);
تعبير_ty _PyPegen_expect_soft_keyword(Parser *p, ثابت حرف *keyword);
تعبير_ty _PyPegen_soft_keyword_token(Parser *p);
تعبير_ty Py_Pegen_fstring_وسط_token(Parser* p);
Token *_PyPegen_get_last_nonnwhitespace_token(Parser *);
صحيح _PyPegen_fill_token(Parser *p);
// Return -1 on failure, leaving the buffer and capacity unchanged.
صحيح _PyPegen_grow_loop_buffer(فراغ ***buffer, Py_ssize_t *capacity);
تعبير_ty _PyPegen_name_token(Parser *p);
تعبير_ty Py_Pegen_رقم_token(Parser *p);
فراغ *_PyPegen_string_token(Parser *p);
Py_ssize_t Py_Pegen_byte_offset_to_character_offset_سطر(PyObject *سطر, Py_ssize_t col_offset, Py_ssize_t end_col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset(PyObject *سطر, Py_ssize_t col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset_raw(ثابت حرف*, Py_ssize_t col_offset);

تعريفنوع تعداد {
    STAR_TARGETS,
    DEL_TARGETS,
    FOR_TARGETS,
    SINGLE_TARGETS,
    ATTRIBUTE_OR_SUBSCRIPT_TARGETS
} TARGETS_TYPE;

// Error handling functions and APIs
صحيح _Pypegen_raise_decode_error(Parser *p);
صحيح _Pypegen_tokenizer_error(Parser *p);
فراغ *Py_Pegen_raise_incomplete_مدخل(Parser *p);
فراغ *_PyPegen_raise_error(Parser *p, PyObject *errtype, صحيح use_mark, ثابت حرف *errmsg, ...);
فراغ *_PyPegen_raise_error_known_location(Parser *p, PyObject *errtype,
                                          Py_ssize_t lineno, Py_ssize_t col_offset,
                                          Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                                          ثابت حرف *errmsg, va_list va);
فراغ _Pypegen_set_syntax_error(Parser* p, Token* last_token);
فراغ Pypegen_مكدس_overflow(Parser *p);

ساكن مضمن فراغ *
RAISE_ERROR_KNOWN_LOCATION(Parser *p, PyObject *errtype,
                           Py_ssize_t lineno, Py_ssize_t col_offset,
                           Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                           ثابت حرف *errmsg, ...)
{
    va_list va;
    va_start(va, errmsg);
    Py_ssize_t _col_offset = (col_offset == CURRENT_POS ? CURRENT_POS : col_offset + 1);
    Py_ssize_t _end_col_offset = (end_col_offset == CURRENT_POS ? CURRENT_POS : end_col_offset + 1);
    _PyPegen_raise_error_known_location(p, errtype, lineno, _col_offset, end_lineno, _end_col_offset, errmsg, va);
    va_end(va);
    أرجع عدم;
}
#define RAISE_SYNTAX_ERROR(رسالة, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 0, رسالة, ##__VA_ARGS__)
#define RAISE_INDENTATION_ERROR(رسالة, ...) _PyPegen_raise_error(p, PyExc_IndentationError, 0, رسالة, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_ON_NEXT_TOKEN(رسالة, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 1, رسالة, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_RANGE(a, b, رسالة, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (b)->end_lineno, (b)->end_col_offset, رسالة, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_LOCATION(a, رسالة, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (a)->end_lineno, (a)->end_col_offset, رسالة, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_STARTING_FROM(a, رسالة, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, CURRENT_POS, CURRENT_POS, رسالة, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_INVALID_TARGET(type, e) _RAISE_SYNTAX_ERROR_INVALID_TARGET(p, type, e)

Py_LOCAL_INLINE(صحيح)
_PyPegen_tokens_are_adjacent(Token *a, Token *b)
{
    أرجع (a->end_lineno == b->lineno) && (a->end_col_offset == b->col_offset);
}

Py_LOCAL_INLINE(فراغ *)
CHECK_CALL(Parser *p, فراغ *result)
{
    إذا (result == عدم) {
        أكد(PyErr_Occurred());
        p->error_indicator = 1;
    }
    أرجع result;
}

/* This is needed for helper functions that are allowed to
   return NULL without an error. Example: _PyPegen_seq_extract_starred_exprs */
Py_LOCAL_INLINE(فراغ *)
CHECK_CALL_NULL_ALLOWED(Parser *p, فراغ *result)
{
    إذا (result == عدم && PyErr_Occurred()) {
        p->error_indicator = 1;
    }
    أرجع result;
}

#define CHECK(type, result) ((type) CHECK_CALL(p, result))
#define CHECK_NULL_ALLOWED(type, result) ((type) CHECK_CALL_NULL_ALLOWED(p, result))

تعبير_ty _PyPegen_get_invalid_target(تعبير_ty e, TARGETS_TYPE targets_type);
ثابت حرف *Py_Pegen_get_تعبير_name(تعبير_ty);
Py_LOCAL_INLINE(فراغ *)
_RAISE_SYNTAX_ERROR_INVALID_TARGET(Parser *p, TARGETS_TYPE type, فراغ *e)
{
    تعبير_ty invalid_target = CHECK_NULL_ALLOWED(تعبير_ty, _PyPegen_get_invalid_target(e, type));
    إذا (invalid_target != عدم) {
        ثابت حرف *رسالة;
        إذا (type == STAR_TARGETS || type == FOR_TARGETS) {
            رسالة = "cannot assign to %s";
        }
        وإلا {
            رسالة = "cannot delete %s";
        }
        أرجع RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
            invalid_target,
            رسالة,
            Py_Pegen_get_تعبير_name(invalid_target)
        );
        أرجع RAISE_SYNTAX_ERROR_KNOWN_LOCATION(invalid_target, "invalid syntax");
    }
    أرجع عدم;
}

// Action utility functions

فراغ *_PyPegen_dummy_name(Parser *p, ...);
فراغ * _PyPegen_seq_last_item(asdl_seq *seq);
#define PyPegen_last_item(seq, type) ((type)_PyPegen_seq_last_item((asdl_seq*)seq))
فراغ * _PyPegen_seq_first_item(asdl_seq *seq);
#define PyPegen_first_item(seq, type) ((type)_PyPegen_seq_first_item((asdl_seq*)seq))
#define UNUSED(تعبير) افعل { (فراغ)(تعبير); } طالما (0)
#define EXTRA_تعبير(head, tail) head->lineno, (head)->col_offset, (tail)->end_lineno, (tail)->end_col_offset, p->arena
#define EXTRA _start_lineno, _start_col_offset, _end_lineno, _end_col_offset, p->arena
PyObject *_PyPegen_new_type_comment(Parser *, ثابت حرف *);

Py_LOCAL_INLINE(PyObject *)
NEW_TYPE_COMMENT(Parser *p, Token *tc)
{
    إذا (tc == عدم) {
        أرجع عدم;
    }
    ثابت حرف *bytes = PyBytes_AsString(tc->bytes);
    إذا (bytes == عدم) {
        اذهب error;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    إذا (tco == عدم) {
        اذهب error;
    }
    أرجع tco;
 error:
    p->error_indicator = 1;  // Inline CHECK_CALL
    أرجع عدم;
}

Py_LOCAL_INLINE(فراغ *)
INVALID_VERSION_CHECK(Parser *p, صحيح version, حرف *رسالة, فراغ *عقدة)
{
    إذا (عقدة == عدم) {
        p->error_indicator = 1;  // Inline CHECK_CALL
        أرجع عدم;
    }
    إذا (p->feature_version < version) {
        p->error_indicator = 1;
        أرجع RAISE_SYNTAX_ERROR("%s only supported in Python 3.%i and greater",
                                  رسالة, version);
    }
    أرجع عقدة;
}

#define CHECK_VERSION(type, version, رسالة, عقدة) ((type) INVALID_VERSION_CHECK(p, version, رسالة, عقدة))

arg_ty _PyPegen_add_type_comment_to_arg(Parser *, arg_ty, Token *);
PyObject *_PyPegen_new_identifier(Parser *, ثابت حرف *);
asdl_seq *_PyPegen_singleton_seq(Parser *, فراغ *);
asdl_seq *Py_Pegen_seq_أدرج_in_front(Parser *, فراغ *, asdl_seq *);
asdl_seq *_PyPegen_seq_append_to_end(Parser *, asdl_seq *, فراغ *);
asdl_seq *_PyPegen_seq_flatten(Parser *, asdl_seq *);
تعبير_ty _PyPegen_join_names_with_dot(Parser *, تعبير_ty, تعبير_ty);
صحيح Py_Pegen_seq_عدد_dots(asdl_seq *);
alias_ty _PyPegen_alias_for_star(Parser *, صحيح, صحيح, صحيح, صحيح, PyArena *);
asdl_identifier_seq *_PyPegen_map_names_to_ids(Parser *, asdl_تعبير_seq *);
Cmpop_تعبير_Pair *Py_Pegen_cmpop_تعبير_pair(Parser *, cmpop_ty, تعبير_ty);
asdl_int_seq *_PyPegen_get_cmpops(Parser *p, asdl_seq *);
asdl_تعبير_seq *_PyPegen_get_exprs(Parser *, asdl_seq *);
تعبير_ty Py_Pegen_set_تعبير_context(Parser *, تعبير_ty, تعبير_context_ty);
تعبير_ty _PyPegen_make_target(Parser *, تعبير_ty, TARGETS_TYPE);
مفتاح_قيمة_Pair *Py_Pegen_مفتاح_قيمة_pair(Parser *, تعبير_ty, تعبير_ty);
asdl_تعبير_seq *Py_Pegen_get_مفاتيح(Parser *, asdl_seq *);
asdl_تعبير_seq *Py_Pegen_get_قيم(Parser *, asdl_seq *);
مفتاح_Pattern_Pair *Py_Pegen_مفتاح_pattern_pair(Parser *, تعبير_ty, pattern_ty);
asdl_تعبير_seq *Py_Pegen_get_pattern_مفاتيح(Parser *, asdl_seq *);
asdl_pattern_seq *_PyPegen_get_patterns(Parser *, asdl_seq *);
NameDefaultPair *_PyPegen_name_default_pair(Parser *, arg_ty, تعبير_ty, Token *);
SlashWithDefault *_PyPegen_slash_with_default(Parser *, asdl_arg_seq *, asdl_seq *);
StarEtc *_PyPegen_star_etc(Parser *, arg_ty, asdl_seq *, arg_ty);
arguments_ty _PyPegen_make_arguments(Parser *, asdl_arg_seq *, SlashWithDefault *,
                                     asdl_arg_seq *, asdl_seq *, StarEtc *);
arguments_ty _PyPegen_empty_arguments(Parser *);
تعبير_ty _PyPegen_template_str(Parser *p, Token *a, asdl_تعبير_seq *raw_expressions, Token *b);
تعبير_ty _PyPegen_joined_str(Parser *p, Token *a, asdl_تعبير_seq *raw_expressions, Token *b);
تعبير_ty _PyPegen_interpolation(Parser *, تعبير_ty, Token *, ResultTokenWithMetadata *, ResultTokenWithMetadata *, Token *,
                                 صحيح, صحيح, صحيح, صحيح, PyArena *);
تعبير_ty Py_Pegen_formatted_قيمة(Parser *, تعبير_ty, Token *, ResultTokenWithMetadata *, ResultTokenWithMetadata *, Token *,
                                 صحيح, صحيح, صحيح, صحيح, PyArena *);
AugOperator *_PyPegen_augoperator(Parser*, operator_ty type);
stmt_ty _PyPegen_function_def_decorators(Parser *, asdl_تعبير_seq *, stmt_ty);
stmt_ty _PyPegen_class_def_decorators(Parser *, asdl_تعبير_seq *, stmt_ty);
KeywordOrStarred *_PyPegen_keyword_or_starred(Parser *, فراغ *, صحيح);
asdl_تعبير_seq *_PyPegen_seq_extract_starred_exprs(Parser *, asdl_seq *);
asdl_keyword_seq *_PyPegen_seq_delete_starred_exprs(Parser *, asdl_seq *);
تعبير_ty _PyPegen_collect_call_seqs(Parser *, asdl_تعبير_seq *, asdl_seq *,
                     صحيح lineno, صحيح col_offset, صحيح end_lineno,
                     صحيح end_col_offset, PyArena *arena);
تعبير_ty _PyPegen_constant_from_token(Parser* p, Token* tok);
تعبير_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok);
تعبير_ty _PyPegen_constant_from_string(Parser* p, Token* tok);
تعبير_ty _PyPegen_concatenate_tstrings(Parser *p, asdl_تعبير_seq *, صحيح, صحيح, صحيح, صحيح, PyArena *);
تعبير_ty _PyPegen_concatenate_strings(Parser *p, asdl_تعبير_seq *, صحيح, صحيح, صحيح, صحيح, PyArena *);
تعبير_ty _PyPegen_FetchRawForm(Parser *p, صحيح, صحيح, صحيح, صحيح);
تعبير_ty _PyPegen_ensure_imaginary(Parser *p, تعبير_ty);
تعبير_ty _PyPegen_ensure_real(Parser *p, تعبير_ty);
asdl_seq *_PyPegen_join_sequences(Parser *, asdl_seq *, asdl_seq *);
صحيح _PyPegen_check_barry_as_flufl(Parser *, Token *);
صحيح _PyPegen_check_legacy_stmt(Parser *p, تعبير_ty t);
فراغ *_PyPegen_raise_error_for_missing_comma(Parser *p, تعبير_ty a, تعبير_ty b);
ResultTokenWithMetadata *_PyPegen_check_fstring_conversion(Parser *p, Token *, تعبير_ty t);
ResultTokenWithMetadata *_PyPegen_setup_full_format_spec(Parser *, Token *, asdl_تعبير_seq *, صحيح, صحيح,
                                                         صحيح, صحيح, PyArena *);
mod_ty _PyPegen_make_module(Parser *, asdl_stmt_seq *);
فراغ *_PyPegen_arguments_parsing_error(Parser *, تعبير_ty);
تعبير_ty _PyPegen_get_last_comprehension_item(comprehension_ty comprehension);
فراغ *_PyPegen_nonparen_genexp_in_call(Parser *p, تعبير_ty وسائط, asdl_comprehension_seq *comprehensions);
stmt_ty _PyPegen_checked_from_import(Parser *p, asdl_seq *dots, تعبير_ty module_name,
                                     asdl_alias_seq *names, تعبير_ty lazy_token, صحيح lineno,
                                     صحيح col_offset, صحيح end_lineno, صحيح end_col_offset,
                                     PyArena *arena);
asdl_stmt_seq* _PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts);
stmt_ty _PyPegen_register_stmt(Parser *p, stmt_ty s);

// Parser API

Parser *_PyPegen_Parser_New(بنية tok_state *, صحيح, صحيح, صحيح, صحيح *, ثابت حرف*, PyArena *);
فراغ _PyPegen_Parser_Free(Parser *);
mod_ty _PyPegen_run_parser_from_file_pointer(FILE *, صحيح, PyObject *, ثابت حرف *,
                                    ثابت حرف *, ثابت حرف *, PyCompilerFlags *, صحيح *, PyObject **,
                                    PyArena *);
فراغ *_PyPegen_run_parser(Parser *);
mod_ty _PyPegen_run_parser_from_string(ثابت حرف *, صحيح, PyObject *, PyCompilerFlags *, PyArena *, PyObject *, PyObject **);
asdl_stmt_seq *_PyPegen_interactive_exit(Parser *);

// Generated function in parse.c - function definition in python.gram
فراغ *_PyPegen_parse(Parser *);

#endif
