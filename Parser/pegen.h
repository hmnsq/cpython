#ifndef PEGEN_H
#define PEGEN_H

#অন্তর্ভাব <Python.h>
#অন্তর্ভাব <pycore_ast.h>
#অন্তর্ভাব <pycore_token.h>
#অন্তর্ভাব <pycore_ceval.h>     // _Py_ReachedRecursionLimitWithMargin()
#অন্তর্ভাব <pycore_pystate.h>   // _Py_get_machine_stack_pointer()
#অন্তর্ভাব <pycore_pythonrun.h> // _PyOS_STACK_MARGIN_BYTES

#অন্তর্ভাব "tokenizer/tokenizer.h"

#যদি 0
#define PyPARSE_YIELD_IS_KEYWORD        0x0001
#endif

#define PyPARSE_DONT_IMPLY_DEDENT       0x0002

#যদি 0
#define PyPARSE_WITH_IS_KEYWORD         0x0003
#define PyPARSE_PRINT_IS_FUNCTION       0x0004
#define PyPARSE_UNICODE_LITERALS        0x0008
#endif

#define PyPARSE_IGNORE_COOKIE 0x0010
#define PyPARSE_BARRY_AS_BDFL 0x0020
#define PyPARSE_TYPE_COMMENTS 0x0040
#define Py_PARSE_ALLOW_INCOMPLETE_প্রবেশ 0x0100

#define CURRENT_POS (-5)

প্রকারলেখ কাঠামো _memo {
    পূর্ণসংখ্যা type;
    শূন্য *গ্রন্থি;
    পূর্ণসংখ্যা mark;
    কাঠামো _memo *পরবর্তী;
} Memo;

প্রকারলেখ কাঠামো {
    পূর্ণসংখ্যা type;
    // Text required by grammar actions or helpers; otherwise NULL.
    // An unfinished string token also has NULL text. Its grammar action
    // requests more input only if the token is valid at that position.
    PyObject *bytes;
    পূর্ণসংখ্যা level;
    পূর্ণসংখ্যা is_raw;
    পূর্ণসংখ্যা lineno, col_offset, end_lineno, end_col_offset;
    Memo *memo;
    // Filter over the rule types present in `memo` (bit `type & 63` is set
    // for every entry): lets lookups skip walking the list on definite
    // misses, which are the common case.
    uint64_t memo_mask;
    PyObject *metadata;
} Token;

প্রকারলেখ কাঠামো {
    ধ্রুবক অক্ষর *str;
    পূর্ণসংখ্যা type;
} KeywordToken;


প্রকারলেখ কাঠামো {
    কাঠামো {
        পূর্ণসংখ্যা lineno;
        অক্ষর *comment;  // The " <tag>" in "# type: ignore <tag>"
    } *বস্তুসমূহ;
    আকার_t আকার_2;
    আকার_t অঙ্ক_বস্তুসমূহ;
} growable_comment_array;

প্রকারলেখ কাঠামো {
    পূর্ণসংখ্যা lineno;
    পূর্ণসংখ্যা col_offset;
    পূর্ণসংখ্যা end_lineno;
    পূর্ণসংখ্যা end_col_offset;
} location;

প্রকারলেখ কাঠামো _identifier_cache_entry IdentifierCacheEntry;

প্রকারলেখ কাঠামো {
    কাঠামো tok_state *tok;
    Token **tokens;
    পূর্ণসংখ্যা mark;
    পূর্ণসংখ্যা fill, আকার_2;
    PyArena *arena;
    KeywordToken **keywords;
    অক্ষর **soft_keywords;
    পূর্ণসংখ্যা n_keyword_lists;
    পূর্ণসংখ্যা start_rule;
    পূর্ণসংখ্যা *errcode;
    unsigned অক্ষর parsing_started;
    unsigned অক্ষর প্রবেশ_exhausted;
    PyObject* normalize;
    পূর্ণসংখ্যা starting_lineno;
    পূর্ণসংখ্যা starting_col_offset;
    পূর্ণসংখ্যা error_indicator;
    পূর্ণসংখ্যা flags;
    পূর্ণসংখ্যা feature_version;
    growable_comment_array type_ignore_comments;
    Token *known_err_token;
    পূর্ণসংখ্যা level;
    পূর্ণসংখ্যা call_invalid_rules;
    পূর্ণসংখ্যা debug;
    location last_stmt_location;
    IdentifierCacheEntry *identifier_cache;
    PyThreadState *tstate;
    // Cached copy of tstate->c_stack_soft_limit offset by one margin toward
    // the stack top, so the per-rule stack check is a single inline comparison.
    uintptr_t স্তূপ_soft_limit;
} Parser;

// C-stack exhaustion check run on every rule entry. The fast path compares
// the machine stack pointer against the soft limit cached in the Parser;
// only when the stack is nearly exhausted is the full check called.
স্থির অন্তঃরেখ পূর্ণসংখ্যা
Py_Pegen_স্তূপ_exhausted(Parser *p)
{
#যদি Py_স্তূপ_GROWS_DOWN
    যদি (Py_get_machine_স্তূপ_pointer() > p->স্তূপ_soft_limit) {
#অন্যথা
    যদি (Py_get_machine_স্তূপ_pointer() <= p->স্তূপ_soft_limit) {
#endif
        প্রতিজ্ঞা(!_Py_ReachedRecursionLimitWithMargin(p->tstate, 1));
        ফেরত 0;
    }
    ফেরত _Py_ReachedRecursionLimitWithMargin(p->tstate, 1);
}

প্রকারলেখ কাঠামো {
    cmpop_ty cmpop;
    রাশিমালা_ty রাশিমালা;
} Cmpop_রাশিমালা_Pair;

প্রকারলেখ কাঠামো {
    রাশিমালা_ty চাবি;
    রাশিমালা_ty মান;
} চাবি_মান_Pair;

প্রকারলেখ কাঠামো {
    রাশিমালা_ty চাবি;
    pattern_ty pattern;
} চাবি_Pattern_Pair;

প্রকারলেখ কাঠামো {
    arg_ty arg;
    রাশিমালা_ty মান;
} NameDefaultPair;

প্রকারলেখ কাঠামো {
    asdl_arg_seq *plain_names;
    asdl_seq *names_with_defaults; // asdl_seq* of NameDefaultsPair's
} SlashWithDefault;

প্রকারলেখ কাঠামো {
    arg_ty vararg;
    asdl_seq *kwonlyargs; // asdl_seq* of NameDefaultsPair's
    arg_ty kwarg;
} StarEtc;

প্রকারলেখ কাঠামো { operator_ty kind; } AugOperator;
প্রকারলেখ কাঠামো {
    শূন্য *element;
    পূর্ণসংখ্যা is_keyword;
} KeywordOrStarred;

প্রকারলেখ কাঠামো {
    শূন্য *ফলাফল;
    PyObject *metadata;
} ফলাফল_Token_With_Metadata;

// Internal parser functions
#যদি defined(Py_DEBUG)
শূন্য _PyPegen_clear_memo_statistics(শূন্য);
PyObject *_PyPegen_get_memo_statistics(শূন্য);
#endif

পূর্ণসংখ্যা Py_Pegen_ঢোকাও_memo(Parser *p, পূর্ণসংখ্যা mark, পূর্ণসংখ্যা type, শূন্য *গ্রন্থি);
পূর্ণসংখ্যা _PyPegen_update_memo(Parser *p, পূর্ণসংখ্যা mark, পূর্ণসংখ্যা type, শূন্য *গ্রন্থি);
Memo *Py_Pegen_ঢোকাও_memo_direct(Parser *p, পূর্ণসংখ্যা mark, পূর্ণসংখ্যা type);
পূর্ণসংখ্যা _PyPegen_is_memoized(Parser *p, পূর্ণসংখ্যা type, শূন্য *pres);

পূর্ণসংখ্যা _PyPegen_lookahead(পূর্ণসংখ্যা, শূন্য *(func)(Parser *), Parser *);
পূর্ণসংখ্যা Py_Pegen_lookahead_for_রাশিমালা(পূর্ণসংখ্যা, রাশিমালা_ty (func)(Parser *), Parser *);
পূর্ণসংখ্যা _PyPegen_lookahead_for_stmt(পূর্ণসংখ্যা, stmt_ty (func)(Parser *), Parser *);
পূর্ণসংখ্যা _PyPegen_lookahead_with_int(পূর্ণসংখ্যা, Token *(func)(Parser *, পূর্ণসংখ্যা), Parser *, পূর্ণসংখ্যা);
পূর্ণসংখ্যা _PyPegen_lookahead_with_string(পূর্ণসংখ্যা, রাশিমালা_ty (func)(Parser *, ধ্রুবক অক্ষর*), Parser *, ধ্রুবক অক্ষর*);

Token *_PyPegen_expect_token(Parser *p, পূর্ণসংখ্যা type);
শূন্য* Py_Pegen_expect_forced_ফলাফল(Parser *p, শূন্য* ফলাফল, ধ্রুবক অক্ষর* expected);
Token *_PyPegen_expect_forced_token(Parser *p, পূর্ণসংখ্যা type, ধ্রুবক অক্ষর* expected);
রাশিমালা_ty _PyPegen_expect_soft_keyword(Parser *p, ধ্রুবক অক্ষর *keyword);
রাশিমালা_ty _PyPegen_soft_keyword_token(Parser *p);
রাশিমালা_ty Py_Pegen_fstring_মধ্য_token(Parser* p);
Token *_PyPegen_get_last_nonnwhitespace_token(Parser *);
পূর্ণসংখ্যা _PyPegen_fill_token(Parser *p);
// Return -1 on failure, leaving the buffer and capacity unchanged.
পূর্ণসংখ্যা Py_Pegen_grow_loop_বাফার(শূন্য ***বাফার, Py_ssize_t *capacity);
রাশিমালা_ty _PyPegen_name_token(Parser *p);
রাশিমালা_ty Py_Pegen_সংখ্যা_token(Parser *p);
শূন্য *_PyPegen_string_token(Parser *p);
Py_ssize_t Py_Pegen_byte_offset_to_character_offset_সারি(PyObject *সারি, Py_ssize_t col_offset, Py_ssize_t end_col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset(PyObject *সারি, Py_ssize_t col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset_raw(ধ্রুবক অক্ষর*, Py_ssize_t col_offset);

প্রকারলেখ গণনা {
    STAR_TARGETS,
    DEL_TARGETS,
    FOR_TARGETS,
    SINGLE_TARGETS,
    ATTRIBUTE_OR_SUBSCRIPT_TARGETS
} TARGETS_TYPE;

// Error handling functions and APIs
পূর্ণসংখ্যা _Pypegen_raise_decode_error(Parser *p);
পূর্ণসংখ্যা _Pypegen_tokenizer_error(Parser *p);
শূন্য *Py_Pegen_raise_incomplete_প্রবেশ(Parser *p);
শূন্য *_PyPegen_raise_error(Parser *p, PyObject *errtype, পূর্ণসংখ্যা use_mark, ধ্রুবক অক্ষর *errmsg, ...);
শূন্য *_PyPegen_raise_error_known_location(Parser *p, PyObject *errtype,
                                          Py_ssize_t lineno, Py_ssize_t col_offset,
                                          Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                                          ধ্রুবক অক্ষর *errmsg, va_list va);
শূন্য _Pypegen_set_syntax_error(Parser* p, Token* last_token);
শূন্য Pypegen_স্তূপ_overflow(Parser *p);

স্থির অন্তঃরেখ শূন্য *
RAISE_ERROR_KNOWN_LOCATION(Parser *p, PyObject *errtype,
                           Py_ssize_t lineno, Py_ssize_t col_offset,
                           Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                           ধ্রুবক অক্ষর *errmsg, ...)
{
    va_list va;
    va_start(va, errmsg);
    Py_ssize_t _col_offset = (col_offset == CURRENT_POS ? CURRENT_POS : col_offset + 1);
    Py_ssize_t _end_col_offset = (end_col_offset == CURRENT_POS ? CURRENT_POS : end_col_offset + 1);
    _PyPegen_raise_error_known_location(p, errtype, lineno, _col_offset, end_lineno, _end_col_offset, errmsg, va);
    va_end(va);
    ফেরত ফাঁকা;
}
#define RAISE_SYNTAX_ERROR(বার্তা, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 0, বার্তা, ##__VA_ARGS__)
#define RAISE_INDENTATION_ERROR(বার্তা, ...) _PyPegen_raise_error(p, PyExc_IndentationError, 0, বার্তা, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_ON_পরবর্তী_TOKEN(বার্তা, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 1, বার্তা, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_RANGE(a, b, বার্তা, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (b)->end_lineno, (b)->end_col_offset, বার্তা, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_LOCATION(a, বার্তা, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (a)->end_lineno, (a)->end_col_offset, বার্তা, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_STARTING_FROM(a, বার্তা, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, CURRENT_POS, CURRENT_POS, বার্তা, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_INVALID_TARGET(type, e) _RAISE_SYNTAX_ERROR_INVALID_TARGET(p, type, e)

Py_LOCAL_INLINE(পূর্ণসংখ্যা)
_PyPegen_tokens_are_adjacent(Token *a, Token *b)
{
    ফেরত (a->end_lineno == b->lineno) && (a->end_col_offset == b->col_offset);
}

Py_LOCAL_INLINE(শূন্য *)
CHECK_CALL(Parser *p, শূন্য *ফলাফল)
{
    যদি (ফলাফল == ফাঁকা) {
        প্রতিজ্ঞা(PyErr_Occurred());
        p->error_indicator = 1;
    }
    ফেরত ফলাফল;
}

/* This is needed for helper functions that are allowed to
   return NULL without an error. Example: _PyPegen_seq_extract_starred_exprs */
Py_LOCAL_INLINE(শূন্য *)
CHECK_CALL_NULL_ALLOWED(Parser *p, শূন্য *ফলাফল)
{
    যদি (ফলাফল == ফাঁকা && PyErr_Occurred()) {
        p->error_indicator = 1;
    }
    ফেরত ফলাফল;
}

#define CHECK(type, ফলাফল) ((type) CHECK_CALL(p, ফলাফল))
#define CHECK_NULL_ALLOWED(type, ফলাফল) ((type) CHECK_CALL_NULL_ALLOWED(p, ফলাফল))

রাশিমালা_ty _PyPegen_get_invalid_target(রাশিমালা_ty e, TARGETS_TYPE targets_type);
ধ্রুবক অক্ষর *Py_Pegen_get_রাশিমালা_name(রাশিমালা_ty);
Py_LOCAL_INLINE(শূন্য *)
_RAISE_SYNTAX_ERROR_INVALID_TARGET(Parser *p, TARGETS_TYPE type, শূন্য *e)
{
    রাশিমালা_ty invalid_target = CHECK_NULL_ALLOWED(রাশিমালা_ty, _PyPegen_get_invalid_target(e, type));
    যদি (invalid_target != ফাঁকা) {
        ধ্রুবক অক্ষর *বার্তা;
        যদি (type == STAR_TARGETS || type == FOR_TARGETS) {
            বার্তা = "cannot assign to %s";
        }
        অন্যথা {
            বার্তা = "cannot delete %s";
        }
        ফেরত RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
            invalid_target,
            বার্তা,
            Py_Pegen_get_রাশিমালা_name(invalid_target)
        );
        ফেরত RAISE_SYNTAX_ERROR_KNOWN_LOCATION(invalid_target, "invalid syntax");
    }
    ফেরত ফাঁকা;
}

// Action utility functions

শূন্য *_PyPegen_dummy_name(Parser *p, ...);
শূন্য * _PyPegen_seq_last_item(asdl_seq *seq);
#define PyPegen_last_item(seq, type) ((type)_PyPegen_seq_last_item((asdl_seq*)seq))
শূন্য * _PyPegen_seq_first_item(asdl_seq *seq);
#define PyPegen_first_item(seq, type) ((type)_PyPegen_seq_first_item((asdl_seq*)seq))
#define UNUSED(রাশিমালা) করো { (শূন্য)(রাশিমালা); } যখন (0)
#define EXTRA_রাশিমালা(শীর্ষ, লেজ) শীর্ষ->lineno, (শীর্ষ)->col_offset, (লেজ)->end_lineno, (লেজ)->end_col_offset, p->arena
#define EXTRA _start_lineno, _start_col_offset, _end_lineno, _end_col_offset, p->arena
PyObject *_PyPegen_new_type_comment(Parser *, ধ্রুবক অক্ষর *);

Py_LOCAL_INLINE(PyObject *)
NEW_TYPE_COMMENT(Parser *p, Token *tc)
{
    যদি (tc == ফাঁকা) {
        ফেরত ফাঁকা;
    }
    ধ্রুবক অক্ষর *bytes = PyBytes_AsString(tc->bytes);
    যদি (bytes == ফাঁকা) {
        যাও error;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    যদি (tco == ফাঁকা) {
        যাও error;
    }
    ফেরত tco;
 error:
    p->error_indicator = 1;  // Inline CHECK_CALL
    ফেরত ফাঁকা;
}

Py_LOCAL_INLINE(শূন্য *)
INVALID_VERSION_CHECK(Parser *p, পূর্ণসংখ্যা version, অক্ষর *বার্তা, শূন্য *গ্রন্থি)
{
    যদি (গ্রন্থি == ফাঁকা) {
        p->error_indicator = 1;  // Inline CHECK_CALL
        ফেরত ফাঁকা;
    }
    যদি (p->feature_version < version) {
        p->error_indicator = 1;
        ফেরত RAISE_SYNTAX_ERROR("%s only supported in Python 3.%i and greater",
                                  বার্তা, version);
    }
    ফেরত গ্রন্থি;
}

#define CHECK_VERSION(type, version, বার্তা, গ্রন্থি) ((type) INVALID_VERSION_CHECK(p, version, বার্তা, গ্রন্থি))

arg_ty _PyPegen_add_type_comment_to_arg(Parser *, arg_ty, Token *);
PyObject *_PyPegen_new_identifier(Parser *, ধ্রুবক অক্ষর *);
asdl_seq *_PyPegen_singleton_seq(Parser *, শূন্য *);
asdl_seq *Py_Pegen_seq_ঢোকাও_in_front(Parser *, শূন্য *, asdl_seq *);
asdl_seq *_PyPegen_seq_append_to_end(Parser *, asdl_seq *, শূন্য *);
asdl_seq *_PyPegen_seq_flatten(Parser *, asdl_seq *);
রাশিমালা_ty _PyPegen_join_names_with_dot(Parser *, রাশিমালা_ty, রাশিমালা_ty);
পূর্ণসংখ্যা Py_Pegen_seq_গুনতি_dots(asdl_seq *);
alias_ty _PyPegen_alias_for_star(Parser *, পূর্ণসংখ্যা, পূর্ণসংখ্যা, পূর্ণসংখ্যা, পূর্ণসংখ্যা, PyArena *);
asdl_identifier_seq *_PyPegen_map_names_to_ids(Parser *, asdl_রাশিমালা_seq *);
Cmpop_রাশিমালা_Pair *Py_Pegen_cmpop_রাশিমালা_pair(Parser *, cmpop_ty, রাশিমালা_ty);
asdl_int_seq *_PyPegen_get_cmpops(Parser *p, asdl_seq *);
asdl_রাশিমালা_seq *_PyPegen_get_exprs(Parser *, asdl_seq *);
রাশিমালা_ty Py_Pegen_set_রাশিমালা_context(Parser *, রাশিমালা_ty, রাশিমালা_context_ty);
রাশিমালা_ty _PyPegen_make_target(Parser *, রাশিমালা_ty, TARGETS_TYPE);
চাবি_মান_Pair *Py_Pegen_চাবি_মান_pair(Parser *, রাশিমালা_ty, রাশিমালা_ty);
asdl_রাশিমালা_seq *Py_Pegen_get_চাবিগুলি(Parser *, asdl_seq *);
asdl_রাশিমালা_seq *Py_Pegen_get_মানসমূহ(Parser *, asdl_seq *);
চাবি_Pattern_Pair *Py_Pegen_চাবি_pattern_pair(Parser *, রাশিমালা_ty, pattern_ty);
asdl_রাশিমালা_seq *Py_Pegen_get_pattern_চাবিগুলি(Parser *, asdl_seq *);
asdl_pattern_seq *_PyPegen_get_patterns(Parser *, asdl_seq *);
NameDefaultPair *_PyPegen_name_default_pair(Parser *, arg_ty, রাশিমালা_ty, Token *);
SlashWithDefault *_PyPegen_slash_with_default(Parser *, asdl_arg_seq *, asdl_seq *);
StarEtc *_PyPegen_star_etc(Parser *, arg_ty, asdl_seq *, arg_ty);
arguments_ty _PyPegen_make_arguments(Parser *, asdl_arg_seq *, SlashWithDefault *,
                                     asdl_arg_seq *, asdl_seq *, StarEtc *);
arguments_ty _PyPegen_empty_arguments(Parser *);
রাশিমালা_ty _PyPegen_template_str(Parser *p, Token *a, asdl_রাশিমালা_seq *raw_expressions, Token *b);
রাশিমালা_ty _PyPegen_joined_str(Parser *p, Token *a, asdl_রাশিমালা_seq *raw_expressions, Token *b);
রাশিমালা_ty _PyPegen_interpolation(Parser *, রাশিমালা_ty, Token *, ফলাফল_Token_With_Metadata *, ফলাফল_Token_With_Metadata *, Token *,
                                 পূর্ণসংখ্যা, পূর্ণসংখ্যা, পূর্ণসংখ্যা, পূর্ণসংখ্যা, PyArena *);
রাশিমালা_ty Py_Pegen_formatted_মান(Parser *, রাশিমালা_ty, Token *, ফলাফল_Token_With_Metadata *, ফলাফল_Token_With_Metadata *, Token *,
                                 পূর্ণসংখ্যা, পূর্ণসংখ্যা, পূর্ণসংখ্যা, পূর্ণসংখ্যা, PyArena *);
AugOperator *_PyPegen_augoperator(Parser*, operator_ty type);
stmt_ty _PyPegen_function_def_decorators(Parser *, asdl_রাশিমালা_seq *, stmt_ty);
stmt_ty _PyPegen_class_def_decorators(Parser *, asdl_রাশিমালা_seq *, stmt_ty);
KeywordOrStarred *_PyPegen_keyword_or_starred(Parser *, শূন্য *, পূর্ণসংখ্যা);
asdl_রাশিমালা_seq *_PyPegen_seq_extract_starred_exprs(Parser *, asdl_seq *);
asdl_keyword_seq *_PyPegen_seq_delete_starred_exprs(Parser *, asdl_seq *);
রাশিমালা_ty _PyPegen_collect_call_seqs(Parser *, asdl_রাশিমালা_seq *, asdl_seq *,
                     পূর্ণসংখ্যা lineno, পূর্ণসংখ্যা col_offset, পূর্ণসংখ্যা end_lineno,
                     পূর্ণসংখ্যা end_col_offset, PyArena *arena);
রাশিমালা_ty _PyPegen_constant_from_token(Parser* p, Token* tok);
রাশিমালা_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok);
রাশিমালা_ty _PyPegen_constant_from_string(Parser* p, Token* tok);
রাশিমালা_ty _PyPegen_concatenate_tstrings(Parser *p, asdl_রাশিমালা_seq *, পূর্ণসংখ্যা, পূর্ণসংখ্যা, পূর্ণসংখ্যা, পূর্ণসংখ্যা, PyArena *);
রাশিমালা_ty _PyPegen_concatenate_strings(Parser *p, asdl_রাশিমালা_seq *, পূর্ণসংখ্যা, পূর্ণসংখ্যা, পূর্ণসংখ্যা, পূর্ণসংখ্যা, PyArena *);
রাশিমালা_ty _PyPegen_FetchRawForm(Parser *p, পূর্ণসংখ্যা, পূর্ণসংখ্যা, পূর্ণসংখ্যা, পূর্ণসংখ্যা);
রাশিমালা_ty _PyPegen_ensure_imaginary(Parser *p, রাশিমালা_ty);
রাশিমালা_ty _PyPegen_ensure_real(Parser *p, রাশিমালা_ty);
asdl_seq *_PyPegen_join_sequences(Parser *, asdl_seq *, asdl_seq *);
পূর্ণসংখ্যা _PyPegen_check_barry_as_flufl(Parser *, Token *);
পূর্ণসংখ্যা _PyPegen_check_legacy_stmt(Parser *p, রাশিমালা_ty t);
শূন্য *_PyPegen_raise_error_for_missing_comma(Parser *p, রাশিমালা_ty a, রাশিমালা_ty b);
ফলাফল_Token_With_Metadata *_PyPegen_check_fstring_conversion(Parser *p, Token *, রাশিমালা_ty t);
ফলাফল_Token_With_Metadata *_PyPegen_setup_full_format_spec(Parser *, Token *, asdl_রাশিমালা_seq *, পূর্ণসংখ্যা, পূর্ণসংখ্যা,
                                                         পূর্ণসংখ্যা, পূর্ণসংখ্যা, PyArena *);
mod_ty _PyPegen_make_module(Parser *, asdl_stmt_seq *);
শূন্য *_PyPegen_arguments_parsing_error(Parser *, রাশিমালা_ty);
রাশিমালা_ty _PyPegen_get_last_comprehension_item(comprehension_ty comprehension);
শূন্য *_PyPegen_nonparen_genexp_in_call(Parser *p, রাশিমালা_ty args, asdl_comprehension_seq *comprehensions);
stmt_ty _PyPegen_checked_from_import(Parser *p, asdl_seq *dots, রাশিমালা_ty module_name,
                                     asdl_alias_seq *names, রাশিমালা_ty lazy_token, পূর্ণসংখ্যা lineno,
                                     পূর্ণসংখ্যা col_offset, পূর্ণসংখ্যা end_lineno, পূর্ণসংখ্যা end_col_offset,
                                     PyArena *arena);
asdl_stmt_seq* _PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts);
stmt_ty _PyPegen_register_stmt(Parser *p, stmt_ty s);

// Parser API

Parser *_PyPegen_Parser_New(কাঠামো tok_state *, পূর্ণসংখ্যা, পূর্ণসংখ্যা, পূর্ণসংখ্যা, পূর্ণসংখ্যা *, ধ্রুবক অক্ষর*, PyArena *);
শূন্য _PyPegen_Parser_Free(Parser *);
mod_ty _PyPegen_run_parser_from_file_pointer(FILE *, পূর্ণসংখ্যা, PyObject *, ধ্রুবক অক্ষর *,
                                    ধ্রুবক অক্ষর *, ধ্রুবক অক্ষর *, PyCompilerFlags *, পূর্ণসংখ্যা *, PyObject **,
                                    PyArena *);
শূন্য *_PyPegen_run_parser(Parser *);
mod_ty _PyPegen_run_parser_from_string(ধ্রুবক অক্ষর *, পূর্ণসংখ্যা, PyObject *, PyCompilerFlags *, PyArena *, PyObject *, PyObject **);
asdl_stmt_seq *_PyPegen_interactive_exit(Parser *);

// Generated function in parse.c - function definition in python.gram
শূন্য *_PyPegen_parse(Parser *);

#endif
