#ifndef PEGEN_H
#define PEGEN_H

#شامل <Python.h>
#شامل <pycore_ast.h>
#شامل <pycore_token.h>
#شامل <pycore_ceval.h>     // _Py_ReachedRecursionLimitWithMargin()
#شامل <pycore_pystate.h>   // _Py_get_machine_stack_pointer()
#شامل <pycore_pythonrun.h> // _PyOS_STACK_MARGIN_BYTES

#شامل "tokenizer/tokenizer.h"

#اگر 0
#define PyPARSE_YIELD_IS_KEYWORD        0x0001
#endif

#define PyPARSE_DONT_IMPLY_DEDENT       0x0002

#اگر 0
#define PyPARSE_WITH_IS_KEYWORD         0x0003
#define PyPARSE_PRINT_IS_FUNCTION       0x0004
#define PyPARSE_UNICODE_LITERALS        0x0008
#endif

#define PyPARSE_IGNORE_COOKIE 0x0010
#define PyPARSE_BARRY_AS_BDFL 0x0020
#define PyPARSE_TYPE_COMMENTS 0x0040
#define Py_PARSE_ALLOW_INCOMPLETE_درون_داد 0x0100

#define CURRENT_POS (-5)

تعریفنوع ساختار _memo {
    صحیح type;
    تهی *گره;
    صحیح mark;
    ساختار _memo *next;
} Memo;

تعریفنوع ساختار {
    صحیح type;
    // Text required by grammar actions or helpers; otherwise NULL.
    // An unfinished string token also has NULL text. Its grammar action
    // requests more input only if the token is valid at that position.
    PyObject *bytes;
    صحیح level;
    صحیح is_raw;
    صحیح lineno, col_offset, end_lineno, end_col_offset;
    Memo *memo;
    // Filter over the rule types present in `memo` (bit `type & 63` is set
    // for every entry): lets lookups skip walking the list on definite
    // misses, which are the common case.
    uint64_t memo_mask;
    PyObject *metadata;
} Token;

تعریفنوع ساختار {
    ثابت نویسه *str;
    صحیح type;
} KeywordToken;


تعریفنوع ساختار {
    ساختار {
        صحیح lineno;
        نویسه *comment;  // The " <tag>" in "# type: ignore <tag>"
    } *اقلام;
    size_t size;
    size_t رقم_اقلام;
} growable_comment_array;

تعریفنوع ساختار {
    صحیح lineno;
    صحیح col_offset;
    صحیح end_lineno;
    صحیح end_col_offset;
} location;

تعریفنوع ساختار _identifier_cache_entry IdentifierCacheEntry;

تعریفنوع ساختار {
    ساختار tok_state *tok;
    Token **tokens;
    صحیح mark;
    صحیح fill, size;
    PyArena *arena;
    KeywordToken **keywords;
    نویسه **soft_keywords;
    صحیح n_keyword_lists;
    صحیح start_rule;
    صحیح *errcode;
    unsigned نویسه parsing_started;
    unsigned نویسه درون_داد_exhausted;
    PyObject* normalize;
    صحیح starting_lineno;
    صحیح starting_col_offset;
    صحیح error_indicator;
    صحیح flags;
    صحیح feature_version;
    growable_comment_array type_ignore_comments;
    Token *known_err_token;
    صحیح level;
    صحیح call_invalid_rules;
    صحیح debug;
    location last_stmt_location;
    IdentifierCacheEntry *identifier_cache;
    PyThreadState *tstate;
    // Cached copy of tstate->c_stack_soft_limit offset by one margin toward
    // the stack top, so the per-rule stack check is a single inline comparison.
    uintptr_t پشته_soft_limit;
} Parser;

// C-stack exhaustion check run on every rule entry. The fast path compares
// the machine stack pointer against the soft limit cached in the Parser;
// only when the stack is nearly exhausted is the full check called.
ایستا درونخطی صحیح
Py_Pegen_پشته_exhausted(Parser *p)
{
#اگر Py_پشته_GROWS_DOWN
    اگر (Py_get_machine_پشته_pointer() > p->پشته_soft_limit) {
#وگرنه
    اگر (Py_get_machine_پشته_pointer() <= p->پشته_soft_limit) {
#endif
        أكد(!_Py_ReachedRecursionLimitWithMargin(p->tstate, 1));
        بازگشت 0;
    }
    بازگشت _Py_ReachedRecursionLimitWithMargin(p->tstate, 1);
}

تعریفنوع ساختار {
    cmpop_ty cmpop;
    عبارت_ty عبارت;
} Cmpop_عبارت_Pair;

تعریفنوع ساختار {
    عبارت_ty کلید;
    عبارت_ty مقدار;
} کلید_مقدار_Pair;

تعریفنوع ساختار {
    عبارت_ty کلید;
    pattern_ty pattern;
} کلید_Pattern_Pair;

تعریفنوع ساختار {
    arg_ty arg;
    عبارت_ty مقدار;
} NameDefaultPair;

تعریفنوع ساختار {
    asdl_arg_seq *plain_names;
    asdl_seq *names_with_defaults; // asdl_seq* of NameDefaultsPair's
} SlashWithDefault;

تعریفنوع ساختار {
    arg_ty vararg;
    asdl_seq *kwonlyargs; // asdl_seq* of NameDefaultsPair's
    arg_ty kwarg;
} StarEtc;

تعریفنوع ساختار { operator_ty kind; } AugOperator;
تعریفنوع ساختار {
    تهی *element;
    صحیح is_keyword;
} KeywordOrStarred;

تعریفنوع ساختار {
    تهی *result;
    PyObject *metadata;
} ResultTokenWithMetadata;

// Internal parser functions
#اگر defined(Py_DEBUG)
تهی _PyPegen_clear_memo_statistics(تهی);
PyObject *_PyPegen_get_memo_statistics(تهی);
#endif

صحیح Py_Pegen_درج_memo(Parser *p, صحیح mark, صحیح type, تهی *گره);
صحیح _PyPegen_update_memo(Parser *p, صحیح mark, صحیح type, تهی *گره);
Memo *Py_Pegen_درج_memo_direct(Parser *p, صحیح mark, صحیح type);
صحیح _PyPegen_is_memoized(Parser *p, صحیح type, تهی *pres);

صحیح _PyPegen_lookahead(صحیح, تهی *(func)(Parser *), Parser *);
صحیح Py_Pegen_lookahead_for_عبارت(صحیح, عبارت_ty (func)(Parser *), Parser *);
صحیح _PyPegen_lookahead_for_stmt(صحیح, stmt_ty (func)(Parser *), Parser *);
صحیح _PyPegen_lookahead_with_int(صحیح, Token *(func)(Parser *, صحیح), Parser *, صحیح);
صحیح _PyPegen_lookahead_with_string(صحیح, عبارت_ty (func)(Parser *, ثابت نویسه*), Parser *, ثابت نویسه*);

Token *_PyPegen_expect_token(Parser *p, صحیح type);
تهی* _PyPegen_expect_forced_result(Parser *p, تهی* result, ثابت نویسه* expected);
Token *_PyPegen_expect_forced_token(Parser *p, صحیح type, ثابت نویسه* expected);
عبارت_ty _PyPegen_expect_soft_keyword(Parser *p, ثابت نویسه *keyword);
عبارت_ty _PyPegen_soft_keyword_token(Parser *p);
عبارت_ty Py_Pegen_fstring_میانه_token(Parser* p);
Token *_PyPegen_get_last_nonnwhitespace_token(Parser *);
صحیح _PyPegen_fill_token(Parser *p);
// Return -1 on failure, leaving the buffer and capacity unchanged.
صحیح _PyPegen_grow_loop_buffer(تهی ***buffer, Py_ssize_t *capacity);
عبارت_ty _PyPegen_name_token(Parser *p);
عبارت_ty Py_Pegen_عدد_token(Parser *p);
تهی *_PyPegen_string_token(Parser *p);
Py_ssize_t Py_Pegen_byte_offset_to_character_offset_سطر(PyObject *سطر, Py_ssize_t col_offset, Py_ssize_t end_col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset(PyObject *سطر, Py_ssize_t col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset_raw(ثابت نویسه*, Py_ssize_t col_offset);

تعریفنوع شمارش {
    STAR_TARGETS,
    DEL_TARGETS,
    FOR_TARGETS,
    SINGLE_TARGETS,
    ATTRIBUTE_OR_SUBSCRIPT_TARGETS
} TARGETS_TYPE;

// Error handling functions and APIs
صحیح _Pypegen_raise_decode_error(Parser *p);
صحیح _Pypegen_tokenizer_error(Parser *p);
تهی *Py_Pegen_raise_incomplete_درون_داد(Parser *p);
تهی *_PyPegen_raise_error(Parser *p, PyObject *errtype, صحیح use_mark, ثابت نویسه *errmsg, ...);
تهی *_PyPegen_raise_error_known_location(Parser *p, PyObject *errtype,
                                          Py_ssize_t lineno, Py_ssize_t col_offset,
                                          Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                                          ثابت نویسه *errmsg, va_list va);
تهی _Pypegen_set_syntax_error(Parser* p, Token* last_token);
تهی Pypegen_پشته_overflow(Parser *p);

ایستا درونخطی تهی *
RAISE_ERROR_KNOWN_LOCATION(Parser *p, PyObject *errtype,
                           Py_ssize_t lineno, Py_ssize_t col_offset,
                           Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                           ثابت نویسه *errmsg, ...)
{
    va_list va;
    va_start(va, errmsg);
    Py_ssize_t _col_offset = (col_offset == CURRENT_POS ? CURRENT_POS : col_offset + 1);
    Py_ssize_t _end_col_offset = (end_col_offset == CURRENT_POS ? CURRENT_POS : end_col_offset + 1);
    _PyPegen_raise_error_known_location(p, errtype, lineno, _col_offset, end_lineno, _end_col_offset, errmsg, va);
    va_end(va);
    بازگشت پوچ;
}
#define RAISE_SYNTAX_ERROR(پیام, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 0, پیام, ##__VA_ARGS__)
#define RAISE_INDENTATION_ERROR(پیام, ...) _PyPegen_raise_error(p, PyExc_IndentationError, 0, پیام, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_ON_NEXT_TOKEN(پیام, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 1, پیام, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_RANGE(a, b, پیام, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (b)->end_lineno, (b)->end_col_offset, پیام, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_LOCATION(a, پیام, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (a)->end_lineno, (a)->end_col_offset, پیام, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_STARTING_FROM(a, پیام, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, CURRENT_POS, CURRENT_POS, پیام, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_INVALID_TARGET(type, e) _RAISE_SYNTAX_ERROR_INVALID_TARGET(p, type, e)

Py_LOCAL_INLINE(صحیح)
_PyPegen_tokens_are_adjacent(Token *a, Token *b)
{
    بازگشت (a->end_lineno == b->lineno) && (a->end_col_offset == b->col_offset);
}

Py_LOCAL_INLINE(تهی *)
CHECK_CALL(Parser *p, تهی *result)
{
    اگر (result == پوچ) {
        أكد(PyErr_Occurred());
        p->error_indicator = 1;
    }
    بازگشت result;
}

/* This is needed for helper functions that are allowed to
   return NULL without an error. Example: _PyPegen_seq_extract_starred_exprs */
Py_LOCAL_INLINE(تهی *)
CHECK_CALL_NULL_ALLOWED(Parser *p, تهی *result)
{
    اگر (result == پوچ && PyErr_Occurred()) {
        p->error_indicator = 1;
    }
    بازگشت result;
}

#define CHECK(type, result) ((type) CHECK_CALL(p, result))
#define CHECK_NULL_ALLOWED(type, result) ((type) CHECK_CALL_NULL_ALLOWED(p, result))

عبارت_ty _PyPegen_get_invalid_target(عبارت_ty e, TARGETS_TYPE targets_type);
ثابت نویسه *Py_Pegen_get_عبارت_name(عبارت_ty);
Py_LOCAL_INLINE(تهی *)
_RAISE_SYNTAX_ERROR_INVALID_TARGET(Parser *p, TARGETS_TYPE type, تهی *e)
{
    عبارت_ty invalid_target = CHECK_NULL_ALLOWED(عبارت_ty, _PyPegen_get_invalid_target(e, type));
    اگر (invalid_target != پوچ) {
        ثابت نویسه *پیام;
        اگر (type == STAR_TARGETS || type == FOR_TARGETS) {
            پیام = "cannot assign to %s";
        }
        وگرنه {
            پیام = "cannot delete %s";
        }
        بازگشت RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
            invalid_target,
            پیام,
            Py_Pegen_get_عبارت_name(invalid_target)
        );
        بازگشت RAISE_SYNTAX_ERROR_KNOWN_LOCATION(invalid_target, "invalid syntax");
    }
    بازگشت پوچ;
}

// Action utility functions

تهی *_PyPegen_dummy_name(Parser *p, ...);
تهی * _PyPegen_seq_last_item(asdl_seq *seq);
#define PyPegen_last_item(seq, type) ((type)_PyPegen_seq_last_item((asdl_seq*)seq))
تهی * _PyPegen_seq_first_item(asdl_seq *seq);
#define PyPegen_first_item(seq, type) ((type)_PyPegen_seq_first_item((asdl_seq*)seq))
#define UNUSED(عبارت) انجامبده { (تهی)(عبارت); } وقتی (0)
#define EXTRA_عبارت(head, tail) head->lineno, (head)->col_offset, (tail)->end_lineno, (tail)->end_col_offset, p->arena
#define EXTRA _start_lineno, _start_col_offset, _end_lineno, _end_col_offset, p->arena
PyObject *_PyPegen_new_type_comment(Parser *, ثابت نویسه *);

Py_LOCAL_INLINE(PyObject *)
NEW_TYPE_COMMENT(Parser *p, Token *tc)
{
    اگر (tc == پوچ) {
        بازگشت پوچ;
    }
    ثابت نویسه *bytes = PyBytes_AsString(tc->bytes);
    اگر (bytes == پوچ) {
        برو error;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    اگر (tco == پوچ) {
        برو error;
    }
    بازگشت tco;
 error:
    p->error_indicator = 1;  // Inline CHECK_CALL
    بازگشت پوچ;
}

Py_LOCAL_INLINE(تهی *)
INVALID_VERSION_CHECK(Parser *p, صحیح version, نویسه *پیام, تهی *گره)
{
    اگر (گره == پوچ) {
        p->error_indicator = 1;  // Inline CHECK_CALL
        بازگشت پوچ;
    }
    اگر (p->feature_version < version) {
        p->error_indicator = 1;
        بازگشت RAISE_SYNTAX_ERROR("%s only supported in Python 3.%i and greater",
                                  پیام, version);
    }
    بازگشت گره;
}

#define CHECK_VERSION(type, version, پیام, گره) ((type) INVALID_VERSION_CHECK(p, version, پیام, گره))

arg_ty _PyPegen_add_type_comment_to_arg(Parser *, arg_ty, Token *);
PyObject *_PyPegen_new_identifier(Parser *, ثابت نویسه *);
asdl_seq *_PyPegen_singleton_seq(Parser *, تهی *);
asdl_seq *Py_Pegen_seq_درج_in_front(Parser *, تهی *, asdl_seq *);
asdl_seq *_PyPegen_seq_append_to_end(Parser *, asdl_seq *, تهی *);
asdl_seq *_PyPegen_seq_flatten(Parser *, asdl_seq *);
عبارت_ty _PyPegen_join_names_with_dot(Parser *, عبارت_ty, عبارت_ty);
صحیح Py_Pegen_seq_شمار_dots(asdl_seq *);
alias_ty _PyPegen_alias_for_star(Parser *, صحیح, صحیح, صحیح, صحیح, PyArena *);
asdl_identifier_seq *_PyPegen_map_names_to_ids(Parser *, asdl_عبارت_seq *);
Cmpop_عبارت_Pair *Py_Pegen_cmpop_عبارت_pair(Parser *, cmpop_ty, عبارت_ty);
asdl_int_seq *_PyPegen_get_cmpops(Parser *p, asdl_seq *);
asdl_عبارت_seq *_PyPegen_get_exprs(Parser *, asdl_seq *);
عبارت_ty Py_Pegen_set_عبارت_context(Parser *, عبارت_ty, عبارت_context_ty);
عبارت_ty _PyPegen_make_target(Parser *, عبارت_ty, TARGETS_TYPE);
کلید_مقدار_Pair *Py_Pegen_کلید_مقدار_pair(Parser *, عبارت_ty, عبارت_ty);
asdl_عبارت_seq *Py_Pegen_get_کلیدها(Parser *, asdl_seq *);
asdl_عبارت_seq *Py_Pegen_get_مقادیر(Parser *, asdl_seq *);
کلید_Pattern_Pair *Py_Pegen_کلید_pattern_pair(Parser *, عبارت_ty, pattern_ty);
asdl_عبارت_seq *Py_Pegen_get_pattern_کلیدها(Parser *, asdl_seq *);
asdl_pattern_seq *_PyPegen_get_patterns(Parser *, asdl_seq *);
NameDefaultPair *_PyPegen_name_default_pair(Parser *, arg_ty, عبارت_ty, Token *);
SlashWithDefault *_PyPegen_slash_with_default(Parser *, asdl_arg_seq *, asdl_seq *);
StarEtc *_PyPegen_star_etc(Parser *, arg_ty, asdl_seq *, arg_ty);
arguments_ty _PyPegen_make_arguments(Parser *, asdl_arg_seq *, SlashWithDefault *,
                                     asdl_arg_seq *, asdl_seq *, StarEtc *);
arguments_ty _PyPegen_empty_arguments(Parser *);
عبارت_ty _PyPegen_template_str(Parser *p, Token *a, asdl_عبارت_seq *raw_expressions, Token *b);
عبارت_ty _PyPegen_joined_str(Parser *p, Token *a, asdl_عبارت_seq *raw_expressions, Token *b);
عبارت_ty _PyPegen_interpolation(Parser *, عبارت_ty, Token *, ResultTokenWithMetadata *, ResultTokenWithMetadata *, Token *,
                                 صحیح, صحیح, صحیح, صحیح, PyArena *);
عبارت_ty Py_Pegen_formatted_مقدار(Parser *, عبارت_ty, Token *, ResultTokenWithMetadata *, ResultTokenWithMetadata *, Token *,
                                 صحیح, صحیح, صحیح, صحیح, PyArena *);
AugOperator *_PyPegen_augoperator(Parser*, operator_ty type);
stmt_ty _PyPegen_function_def_decorators(Parser *, asdl_عبارت_seq *, stmt_ty);
stmt_ty _PyPegen_class_def_decorators(Parser *, asdl_عبارت_seq *, stmt_ty);
KeywordOrStarred *_PyPegen_keyword_or_starred(Parser *, تهی *, صحیح);
asdl_عبارت_seq *_PyPegen_seq_extract_starred_exprs(Parser *, asdl_seq *);
asdl_keyword_seq *_PyPegen_seq_delete_starred_exprs(Parser *, asdl_seq *);
عبارت_ty _PyPegen_collect_call_seqs(Parser *, asdl_عبارت_seq *, asdl_seq *,
                     صحیح lineno, صحیح col_offset, صحیح end_lineno,
                     صحیح end_col_offset, PyArena *arena);
عبارت_ty _PyPegen_constant_from_token(Parser* p, Token* tok);
عبارت_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok);
عبارت_ty _PyPegen_constant_from_string(Parser* p, Token* tok);
عبارت_ty _PyPegen_concatenate_tstrings(Parser *p, asdl_عبارت_seq *, صحیح, صحیح, صحیح, صحیح, PyArena *);
عبارت_ty _PyPegen_concatenate_strings(Parser *p, asdl_عبارت_seq *, صحیح, صحیح, صحیح, صحیح, PyArena *);
عبارت_ty _PyPegen_FetchRawForm(Parser *p, صحیح, صحیح, صحیح, صحیح);
عبارت_ty _PyPegen_ensure_imaginary(Parser *p, عبارت_ty);
عبارت_ty _PyPegen_ensure_real(Parser *p, عبارت_ty);
asdl_seq *_PyPegen_join_sequences(Parser *, asdl_seq *, asdl_seq *);
صحیح _PyPegen_check_barry_as_flufl(Parser *, Token *);
صحیح _PyPegen_check_legacy_stmt(Parser *p, عبارت_ty t);
تهی *_PyPegen_raise_error_for_missing_comma(Parser *p, عبارت_ty a, عبارت_ty b);
ResultTokenWithMetadata *_PyPegen_check_fstring_conversion(Parser *p, Token *, عبارت_ty t);
ResultTokenWithMetadata *_PyPegen_setup_full_format_spec(Parser *, Token *, asdl_عبارت_seq *, صحیح, صحیح,
                                                         صحیح, صحیح, PyArena *);
mod_ty _PyPegen_make_module(Parser *, asdl_stmt_seq *);
تهی *_PyPegen_arguments_parsing_error(Parser *, عبارت_ty);
عبارت_ty _PyPegen_get_last_comprehension_item(comprehension_ty comprehension);
تهی *_PyPegen_nonparen_genexp_in_call(Parser *p, عبارت_ty args, asdl_comprehension_seq *comprehensions);
stmt_ty _PyPegen_checked_from_import(Parser *p, asdl_seq *dots, عبارت_ty module_name,
                                     asdl_alias_seq *names, عبارت_ty lazy_token, صحیح lineno,
                                     صحیح col_offset, صحیح end_lineno, صحیح end_col_offset,
                                     PyArena *arena);
asdl_stmt_seq* _PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts);
stmt_ty _PyPegen_register_stmt(Parser *p, stmt_ty s);

// Parser API

Parser *_PyPegen_Parser_New(ساختار tok_state *, صحیح, صحیح, صحیح, صحیح *, ثابت نویسه*, PyArena *);
تهی _PyPegen_Parser_Free(Parser *);
mod_ty _PyPegen_run_parser_from_file_pointer(FILE *, صحیح, PyObject *, ثابت نویسه *,
                                    ثابت نویسه *, ثابت نویسه *, PyCompilerFlags *, صحیح *, PyObject **,
                                    PyArena *);
تهی *_PyPegen_run_parser(Parser *);
mod_ty _PyPegen_run_parser_from_string(ثابت نویسه *, صحیح, PyObject *, PyCompilerFlags *, PyArena *, PyObject *, PyObject **);
asdl_stmt_seq *_PyPegen_interactive_exit(Parser *);

// Generated function in parse.c - function definition in python.gram
تهی *_PyPegen_parse(Parser *);

#endif
