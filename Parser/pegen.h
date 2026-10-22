#ifndef PEGEN_H
#define PEGEN_H

#অন্তর্ভাৱ <Python.h>
#অন্তর্ভাৱ <pycore_ast.h>
#অন্তর্ভাৱ <pycore_token.h>
#অন্তর্ভাৱ <pycore_ceval.h>     // _Py_ReachedRecursionLimitWithMargin()
#অন্তর্ভাৱ <pycore_pystate.h>   // _Py_get_machine_stack_pointer()
#অন্তর্ভাৱ <pycore_pythonrun.h> // _PyOS_STACK_MARGIN_BYTES

#অন্তর্ভাৱ "tokenizer/tokenizer.h"

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
#define Py_PARSE_ALLOW_INCOMPLETE_প্ৰবেশ 0x0100

#define CURRENT_POS (-5)

প্রকারনির্দেশ সংরচনা _memo {
    পূর্ণাঙ্ক type;
    শূন্য *গ্ৰন্থি;
    পূর্ণাঙ্ক mark;
    সংরচনা _memo *next;
} Memo;

প্রকারনির্দেশ সংরচনা {
    পূর্ণাঙ্ক type;
    // Text required by grammar actions or helpers; otherwise NULL.
    // An unfinished string token also has NULL text. Its grammar action
    // requests more input only if the token is valid at that position.
    PyObject *bytes;
    পূর্ণাঙ্ক level;
    পূর্ণাঙ্ক is_raw;
    পূর্ণাঙ্ক lineno, col_offset, end_lineno, end_col_offset;
    Memo *memo;
    // Filter over the rule types present in `memo` (bit `type & 63` is set
    // for every entry): lets lookups skip walking the list on definite
    // misses, which are the common case.
    uint64_t memo_mask;
    PyObject *metadata;
} Token;

প্রকারনির্দেশ সংরচনা {
    ধ্রুৱ ৱর্ণ *str;
    পূর্ণাঙ্ক type;
} KeywordToken;


প্রকারনির্দেশ সংরচনা {
    সংরচনা {
        পূর্ণাঙ্ক lineno;
        ৱর্ণ *comment;  // The " <tag>" in "# type: ignore <tag>"
    } *বস্তুসমূহ;
    size_t size;
    size_t অঙ্ক_বস্তুসমূহ;
} growable_comment_array;

প্রকারনির্দেশ সংরচনা {
    পূর্ণাঙ্ক lineno;
    পূর্ণাঙ্ক col_offset;
    পূর্ণাঙ্ক end_lineno;
    পূর্ণাঙ্ক end_col_offset;
} location;

প্রকারনির্দেশ সংরচনা _identifier_cache_entry IdentifierCacheEntry;

প্রকারনির্দেশ সংরচনা {
    সংরচনা tok_state *tok;
    Token **tokens;
    পূর্ণাঙ্ক mark;
    পূর্ণাঙ্ক fill, size;
    PyArena *arena;
    KeywordToken **keywords;
    ৱর্ণ **soft_keywords;
    পূর্ণাঙ্ক n_keyword_lists;
    পূর্ণাঙ্ক start_rule;
    পূর্ণাঙ্ক *errcode;
    unsigned ৱর্ণ parsing_started;
    unsigned ৱর্ণ প্ৰবেশ_exhausted;
    PyObject* normalize;
    পূর্ণাঙ্ক starting_lineno;
    পূর্ণাঙ্ক starting_col_offset;
    পূর্ণাঙ্ক error_indicator;
    পূর্ণাঙ্ক flags;
    পূর্ণাঙ্ক feature_version;
    growable_comment_array type_ignore_comments;
    Token *known_err_token;
    পূর্ণাঙ্ক level;
    পূর্ণাঙ্ক call_invalid_rules;
    পূর্ণাঙ্ক debug;
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
স্থির অন্তঃস্থ পূর্ণাঙ্ক
Py_Pegen_স্তূপ_exhausted(Parser *p)
{
#যদি Py_স্তূপ_GROWS_DOWN
    যদি (Py_get_machine_স্তূপ_pointer() > p->স্তূপ_soft_limit) {
#অন্যথা
    যদি (Py_get_machine_স্তূপ_pointer() <= p->স্তূপ_soft_limit) {
#endif
        প্রতিজ্ঞা(!_Py_ReachedRecursionLimitWithMargin(p->tstate, 1));
        ঘূৰাই 0;
    }
    ঘূৰাই _Py_ReachedRecursionLimitWithMargin(p->tstate, 1);
}

প্রকারনির্দেশ সংরচনা {
    cmpop_ty cmpop;
    ৰাশিমালা_ty ৰাশিমালা;
} Cmpop_ৰাশিমালা_Pair;

প্রকারনির্দেশ সংরচনা {
    ৰাশিমালা_ty চাবি;
    ৰাশিমালা_ty মান;
} চাবি_মান_Pair;

প্রকারনির্দেশ সংরচনা {
    ৰাশিমালা_ty চাবি;
    pattern_ty pattern;
} চাবি_Pattern_Pair;

প্রকারনির্দেশ সংরচনা {
    arg_ty arg;
    ৰাশিমালা_ty মান;
} NameDefaultPair;

প্রকারনির্দেশ সংরচনা {
    asdl_arg_seq *plain_names;
    asdl_seq *names_with_defaults; // asdl_seq* of NameDefaultsPair's
} SlashWithDefault;

প্রকারনির্দেশ সংরচনা {
    arg_ty vararg;
    asdl_seq *kwonlyargs; // asdl_seq* of NameDefaultsPair's
    arg_ty kwarg;
} StarEtc;

প্রকারনির্দেশ সংরচনা { operator_ty kind; } AugOperator;
প্রকারনির্দেশ সংরচনা {
    শূন্য *element;
    পূর্ণাঙ্ক is_keyword;
} KeywordOrStarred;

প্রকারনির্দেশ সংরচনা {
    শূন্য *result;
    PyObject *metadata;
} ResultTokenWithMetadata;

// Internal parser functions
#যদি defined(Py_DEBUG)
শূন্য _PyPegen_clear_memo_statistics(শূন্য);
PyObject *_PyPegen_get_memo_statistics(শূন্য);
#endif

পূর্ণাঙ্ক Py_Pegen_ঢোকাও_memo(Parser *p, পূর্ণাঙ্ক mark, পূর্ণাঙ্ক type, শূন্য *গ্ৰন্থি);
পূর্ণাঙ্ক _PyPegen_update_memo(Parser *p, পূর্ণাঙ্ক mark, পূর্ণাঙ্ক type, শূন্য *গ্ৰন্থি);
Memo *Py_Pegen_ঢোকাও_memo_direct(Parser *p, পূর্ণাঙ্ক mark, পূর্ণাঙ্ক type);
পূর্ণাঙ্ক _PyPegen_is_memoized(Parser *p, পূর্ণাঙ্ক type, শূন্য *pres);

পূর্ণাঙ্ক _PyPegen_lookahead(পূর্ণাঙ্ক, শূন্য *(func)(Parser *), Parser *);
পূর্ণাঙ্ক Py_Pegen_lookahead_for_ৰাশিমালা(পূর্ণাঙ্ক, ৰাশিমালা_ty (func)(Parser *), Parser *);
পূর্ণাঙ্ক _PyPegen_lookahead_for_stmt(পূর্ণাঙ্ক, stmt_ty (func)(Parser *), Parser *);
পূর্ণাঙ্ক _PyPegen_lookahead_with_int(পূর্ণাঙ্ক, Token *(func)(Parser *, পূর্ণাঙ্ক), Parser *, পূর্ণাঙ্ক);
পূর্ণাঙ্ক _PyPegen_lookahead_with_string(পূর্ণাঙ্ক, ৰাশিমালা_ty (func)(Parser *, ধ্রুৱ ৱর্ণ*), Parser *, ধ্রুৱ ৱর্ণ*);

Token *_PyPegen_expect_token(Parser *p, পূর্ণাঙ্ক type);
শূন্য* _PyPegen_expect_forced_result(Parser *p, শূন্য* result, ধ্রুৱ ৱর্ণ* expected);
Token *_PyPegen_expect_forced_token(Parser *p, পূর্ণাঙ্ক type, ধ্রুৱ ৱর্ণ* expected);
ৰাশিমালা_ty _PyPegen_expect_soft_keyword(Parser *p, ধ্রুৱ ৱর্ণ *keyword);
ৰাশিমালা_ty _PyPegen_soft_keyword_token(Parser *p);
ৰাশিমালা_ty Py_Pegen_fstring_মধ্য_token(Parser* p);
Token *_PyPegen_get_last_nonnwhitespace_token(Parser *);
পূর্ণাঙ্ক _PyPegen_fill_token(Parser *p);
// Return -1 on failure, leaving the buffer and capacity unchanged.
পূর্ণাঙ্ক _PyPegen_grow_loop_buffer(শূন্য ***buffer, Py_ssize_t *capacity);
ৰাশিমালা_ty _PyPegen_name_token(Parser *p);
ৰাশিমালা_ty Py_Pegen_সংখ্যা_token(Parser *p);
শূন্য *_PyPegen_string_token(Parser *p);
Py_ssize_t Py_Pegen_byte_offset_to_character_offset_সাৰি(PyObject *সাৰি, Py_ssize_t col_offset, Py_ssize_t end_col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset(PyObject *সাৰি, Py_ssize_t col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset_raw(ধ্রুৱ ৱর্ণ*, Py_ssize_t col_offset);

প্রকারনির্দেশ গণনা {
    STAR_TARGETS,
    DEL_TARGETS,
    FOR_TARGETS,
    SINGLE_TARGETS,
    ATTRIBUTE_OR_SUBSCRIPT_TARGETS
} TARGETS_TYPE;

// Error handling functions and APIs
পূর্ণাঙ্ক _Pypegen_raise_decode_error(Parser *p);
পূর্ণাঙ্ক _Pypegen_tokenizer_error(Parser *p);
শূন্য *Py_Pegen_raise_incomplete_প্ৰবেশ(Parser *p);
শূন্য *_PyPegen_raise_error(Parser *p, PyObject *errtype, পূর্ণাঙ্ক use_mark, ধ্রুৱ ৱর্ণ *errmsg, ...);
শূন্য *_PyPegen_raise_error_known_location(Parser *p, PyObject *errtype,
                                          Py_ssize_t lineno, Py_ssize_t col_offset,
                                          Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                                          ধ্রুৱ ৱর্ণ *errmsg, va_list va);
শূন্য _Pypegen_set_syntax_error(Parser* p, Token* last_token);
শূন্য Pypegen_স্তূপ_overflow(Parser *p);

স্থির অন্তঃস্থ শূন্য *
RAISE_ERROR_KNOWN_LOCATION(Parser *p, PyObject *errtype,
                           Py_ssize_t lineno, Py_ssize_t col_offset,
                           Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                           ধ্রুৱ ৱর্ণ *errmsg, ...)
{
    va_list va;
    va_start(va, errmsg);
    Py_ssize_t _col_offset = (col_offset == CURRENT_POS ? CURRENT_POS : col_offset + 1);
    Py_ssize_t _end_col_offset = (end_col_offset == CURRENT_POS ? CURRENT_POS : end_col_offset + 1);
    _PyPegen_raise_error_known_location(p, errtype, lineno, _col_offset, end_lineno, _end_col_offset, errmsg, va);
    va_end(va);
    ঘূৰাই রিক্ত;
}
#define RAISE_SYNTAX_ERROR(বাৰ্তা, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 0, বাৰ্তা, ##__VA_ARGS__)
#define RAISE_INDENTATION_ERROR(বাৰ্তা, ...) _PyPegen_raise_error(p, PyExc_IndentationError, 0, বাৰ্তা, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_ON_NEXT_TOKEN(বাৰ্তা, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 1, বাৰ্তা, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_RANGE(a, b, বাৰ্তা, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (b)->end_lineno, (b)->end_col_offset, বাৰ্তা, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_LOCATION(a, বাৰ্তা, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (a)->end_lineno, (a)->end_col_offset, বাৰ্তা, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_STARTING_FROM(a, বাৰ্তা, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, CURRENT_POS, CURRENT_POS, বাৰ্তা, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_INVALID_TARGET(type, e) _RAISE_SYNTAX_ERROR_INVALID_TARGET(p, type, e)

Py_LOCAL_INLINE(পূর্ণাঙ্ক)
_PyPegen_tokens_are_adjacent(Token *a, Token *b)
{
    ঘূৰাই (a->end_lineno == b->lineno) && (a->end_col_offset == b->col_offset);
}

Py_LOCAL_INLINE(শূন্য *)
CHECK_CALL(Parser *p, শূন্য *result)
{
    যদি (result == রিক্ত) {
        প্রতিজ্ঞা(PyErr_Occurred());
        p->error_indicator = 1;
    }
    ঘূৰাই result;
}

/* This is needed for helper functions that are allowed to
   return NULL without an error. Example: _PyPegen_seq_extract_starred_exprs */
Py_LOCAL_INLINE(শূন্য *)
CHECK_CALL_NULL_ALLOWED(Parser *p, শূন্য *result)
{
    যদি (result == রিক্ত && PyErr_Occurred()) {
        p->error_indicator = 1;
    }
    ঘূৰাই result;
}

#define CHECK(type, result) ((type) CHECK_CALL(p, result))
#define CHECK_NULL_ALLOWED(type, result) ((type) CHECK_CALL_NULL_ALLOWED(p, result))

ৰাশিমালা_ty _PyPegen_get_invalid_target(ৰাশিমালা_ty e, TARGETS_TYPE targets_type);
ধ্রুৱ ৱর্ণ *Py_Pegen_get_ৰাশিমালা_name(ৰাশিমালা_ty);
Py_LOCAL_INLINE(শূন্য *)
_RAISE_SYNTAX_ERROR_INVALID_TARGET(Parser *p, TARGETS_TYPE type, শূন্য *e)
{
    ৰাশিমালা_ty invalid_target = CHECK_NULL_ALLOWED(ৰাশিমালা_ty, _PyPegen_get_invalid_target(e, type));
    যদি (invalid_target != রিক্ত) {
        ধ্রুৱ ৱর্ণ *বাৰ্তা;
        যদি (type == STAR_TARGETS || type == FOR_TARGETS) {
            বাৰ্তা = "cannot assign to %s";
        }
        অন্যথা {
            বাৰ্তা = "cannot delete %s";
        }
        ঘূৰাই RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
            invalid_target,
            বাৰ্তা,
            Py_Pegen_get_ৰাশিমালা_name(invalid_target)
        );
        ঘূৰাই RAISE_SYNTAX_ERROR_KNOWN_LOCATION(invalid_target, "invalid syntax");
    }
    ঘূৰাই রিক্ত;
}

// Action utility functions

শূন্য *_PyPegen_dummy_name(Parser *p, ...);
শূন্য * _PyPegen_seq_last_item(asdl_seq *seq);
#define PyPegen_last_item(seq, type) ((type)_PyPegen_seq_last_item((asdl_seq*)seq))
শূন্য * _PyPegen_seq_first_item(asdl_seq *seq);
#define PyPegen_first_item(seq, type) ((type)_PyPegen_seq_first_item((asdl_seq*)seq))
#define UNUSED(ৰাশিমালা) কৰা { (শূন্য)(ৰাশিমালা); } যেতিয়ালৈ (0)
#define EXTRA_ৰাশিমালা(head, tail) head->lineno, (head)->col_offset, (tail)->end_lineno, (tail)->end_col_offset, p->arena
#define EXTRA _start_lineno, _start_col_offset, _end_lineno, _end_col_offset, p->arena
PyObject *_PyPegen_new_type_comment(Parser *, ধ্রুৱ ৱর্ণ *);

Py_LOCAL_INLINE(PyObject *)
NEW_TYPE_COMMENT(Parser *p, Token *tc)
{
    যদি (tc == রিক্ত) {
        ঘূৰাই রিক্ত;
    }
    ধ্রুৱ ৱর্ণ *bytes = PyBytes_AsString(tc->bytes);
    যদি (bytes == রিক্ত) {
        গচ্ছ error;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    যদি (tco == রিক্ত) {
        গচ্ছ error;
    }
    ঘূৰাই tco;
 error:
    p->error_indicator = 1;  // Inline CHECK_CALL
    ঘূৰাই রিক্ত;
}

Py_LOCAL_INLINE(শূন্য *)
INVALID_VERSION_CHECK(Parser *p, পূর্ণাঙ্ক version, ৱর্ণ *বাৰ্তা, শূন্য *গ্ৰন্থি)
{
    যদি (গ্ৰন্থি == রিক্ত) {
        p->error_indicator = 1;  // Inline CHECK_CALL
        ঘূৰাই রিক্ত;
    }
    যদি (p->feature_version < version) {
        p->error_indicator = 1;
        ঘূৰাই RAISE_SYNTAX_ERROR("%s only supported in Python 3.%i and greater",
                                  বাৰ্তা, version);
    }
    ঘূৰাই গ্ৰন্থি;
}

#define CHECK_VERSION(type, version, বাৰ্তা, গ্ৰন্থি) ((type) INVALID_VERSION_CHECK(p, version, বাৰ্তা, গ্ৰন্থি))

arg_ty _PyPegen_add_type_comment_to_arg(Parser *, arg_ty, Token *);
PyObject *_PyPegen_new_identifier(Parser *, ধ্রুৱ ৱর্ণ *);
asdl_seq *_PyPegen_singleton_seq(Parser *, শূন্য *);
asdl_seq *Py_Pegen_seq_ঢোকাও_in_front(Parser *, শূন্য *, asdl_seq *);
asdl_seq *_PyPegen_seq_append_to_end(Parser *, asdl_seq *, শূন্য *);
asdl_seq *_PyPegen_seq_flatten(Parser *, asdl_seq *);
ৰাশিমালা_ty _PyPegen_join_names_with_dot(Parser *, ৰাশিমালা_ty, ৰাশিমালা_ty);
পূর্ণাঙ্ক Py_Pegen_seq_গুনতি_dots(asdl_seq *);
alias_ty _PyPegen_alias_for_star(Parser *, পূর্ণাঙ্ক, পূর্ণাঙ্ক, পূর্ণাঙ্ক, পূর্ণাঙ্ক, PyArena *);
asdl_identifier_seq *_PyPegen_map_names_to_ids(Parser *, asdl_ৰাশিমালা_seq *);
Cmpop_ৰাশিমালা_Pair *Py_Pegen_cmpop_ৰাশিমালা_pair(Parser *, cmpop_ty, ৰাশিমালা_ty);
asdl_int_seq *_PyPegen_get_cmpops(Parser *p, asdl_seq *);
asdl_ৰাশিমালা_seq *_PyPegen_get_exprs(Parser *, asdl_seq *);
ৰাশিমালা_ty Py_Pegen_set_ৰাশিমালা_context(Parser *, ৰাশিমালা_ty, ৰাশিমালা_context_ty);
ৰাশিমালা_ty _PyPegen_make_target(Parser *, ৰাশিমালা_ty, TARGETS_TYPE);
চাবি_মান_Pair *Py_Pegen_চাবি_মান_pair(Parser *, ৰাশিমালা_ty, ৰাশিমালা_ty);
asdl_ৰাশিমালা_seq *Py_Pegen_get_চাবিগুলি(Parser *, asdl_seq *);
asdl_ৰাশিমালা_seq *Py_Pegen_get_মানসমূহ(Parser *, asdl_seq *);
চাবি_Pattern_Pair *Py_Pegen_চাবি_pattern_pair(Parser *, ৰাশিমালা_ty, pattern_ty);
asdl_ৰাশিমালা_seq *Py_Pegen_get_pattern_চাবিগুলি(Parser *, asdl_seq *);
asdl_pattern_seq *_PyPegen_get_patterns(Parser *, asdl_seq *);
NameDefaultPair *_PyPegen_name_default_pair(Parser *, arg_ty, ৰাশিমালা_ty, Token *);
SlashWithDefault *_PyPegen_slash_with_default(Parser *, asdl_arg_seq *, asdl_seq *);
StarEtc *_PyPegen_star_etc(Parser *, arg_ty, asdl_seq *, arg_ty);
arguments_ty _PyPegen_make_arguments(Parser *, asdl_arg_seq *, SlashWithDefault *,
                                     asdl_arg_seq *, asdl_seq *, StarEtc *);
arguments_ty _PyPegen_empty_arguments(Parser *);
ৰাশিমালা_ty _PyPegen_template_str(Parser *p, Token *a, asdl_ৰাশিমালা_seq *raw_expressions, Token *b);
ৰাশিমালা_ty _PyPegen_joined_str(Parser *p, Token *a, asdl_ৰাশিমালা_seq *raw_expressions, Token *b);
ৰাশিমালা_ty _PyPegen_interpolation(Parser *, ৰাশিমালা_ty, Token *, ResultTokenWithMetadata *, ResultTokenWithMetadata *, Token *,
                                 পূর্ণাঙ্ক, পূর্ণাঙ্ক, পূর্ণাঙ্ক, পূর্ণাঙ্ক, PyArena *);
ৰাশিমালা_ty Py_Pegen_formatted_মান(Parser *, ৰাশিমালা_ty, Token *, ResultTokenWithMetadata *, ResultTokenWithMetadata *, Token *,
                                 পূর্ণাঙ্ক, পূর্ণাঙ্ক, পূর্ণাঙ্ক, পূর্ণাঙ্ক, PyArena *);
AugOperator *_PyPegen_augoperator(Parser*, operator_ty type);
stmt_ty _PyPegen_function_def_decorators(Parser *, asdl_ৰাশিমালা_seq *, stmt_ty);
stmt_ty _PyPegen_class_def_decorators(Parser *, asdl_ৰাশিমালা_seq *, stmt_ty);
KeywordOrStarred *_PyPegen_keyword_or_starred(Parser *, শূন্য *, পূর্ণাঙ্ক);
asdl_ৰাশিমালা_seq *_PyPegen_seq_extract_starred_exprs(Parser *, asdl_seq *);
asdl_keyword_seq *_PyPegen_seq_delete_starred_exprs(Parser *, asdl_seq *);
ৰাশিমালা_ty _PyPegen_collect_call_seqs(Parser *, asdl_ৰাশিমালা_seq *, asdl_seq *,
                     পূর্ণাঙ্ক lineno, পূর্ণাঙ্ক col_offset, পূর্ণাঙ্ক end_lineno,
                     পূর্ণাঙ্ক end_col_offset, PyArena *arena);
ৰাশিমালা_ty _PyPegen_constant_from_token(Parser* p, Token* tok);
ৰাশিমালা_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok);
ৰাশিমালা_ty _PyPegen_constant_from_string(Parser* p, Token* tok);
ৰাশিমালা_ty _PyPegen_concatenate_tstrings(Parser *p, asdl_ৰাশিমালা_seq *, পূর্ণাঙ্ক, পূর্ণাঙ্ক, পূর্ণাঙ্ক, পূর্ণাঙ্ক, PyArena *);
ৰাশিমালা_ty _PyPegen_concatenate_strings(Parser *p, asdl_ৰাশিমালা_seq *, পূর্ণাঙ্ক, পূর্ণাঙ্ক, পূর্ণাঙ্ক, পূর্ণাঙ্ক, PyArena *);
ৰাশিমালা_ty _PyPegen_FetchRawForm(Parser *p, পূর্ণাঙ্ক, পূর্ণাঙ্ক, পূর্ণাঙ্ক, পূর্ণাঙ্ক);
ৰাশিমালা_ty _PyPegen_ensure_imaginary(Parser *p, ৰাশিমালা_ty);
ৰাশিমালা_ty _PyPegen_ensure_real(Parser *p, ৰাশিমালা_ty);
asdl_seq *_PyPegen_join_sequences(Parser *, asdl_seq *, asdl_seq *);
পূর্ণাঙ্ক _PyPegen_check_barry_as_flufl(Parser *, Token *);
পূর্ণাঙ্ক _PyPegen_check_legacy_stmt(Parser *p, ৰাশিমালা_ty t);
শূন্য *_PyPegen_raise_error_for_missing_comma(Parser *p, ৰাশিমালা_ty a, ৰাশিমালা_ty b);
ResultTokenWithMetadata *_PyPegen_check_fstring_conversion(Parser *p, Token *, ৰাশিমালা_ty t);
ResultTokenWithMetadata *_PyPegen_setup_full_format_spec(Parser *, Token *, asdl_ৰাশিমালা_seq *, পূর্ণাঙ্ক, পূর্ণাঙ্ক,
                                                         পূর্ণাঙ্ক, পূর্ণাঙ্ক, PyArena *);
mod_ty _PyPegen_make_module(Parser *, asdl_stmt_seq *);
শূন্য *_PyPegen_arguments_parsing_error(Parser *, ৰাশিমালা_ty);
ৰাশিমালা_ty _PyPegen_get_last_comprehension_item(comprehension_ty comprehension);
শূন্য *_PyPegen_nonparen_genexp_in_call(Parser *p, ৰাশিমালা_ty args, asdl_comprehension_seq *comprehensions);
stmt_ty _PyPegen_checked_from_import(Parser *p, asdl_seq *dots, ৰাশিমালা_ty module_name,
                                     asdl_alias_seq *names, ৰাশিমালা_ty lazy_token, পূর্ণাঙ্ক lineno,
                                     পূর্ণাঙ্ক col_offset, পূর্ণাঙ্ক end_lineno, পূর্ণাঙ্ক end_col_offset,
                                     PyArena *arena);
asdl_stmt_seq* _PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts);
stmt_ty _PyPegen_register_stmt(Parser *p, stmt_ty s);

// Parser API

Parser *_PyPegen_Parser_New(সংরচনা tok_state *, পূর্ণাঙ্ক, পূর্ণাঙ্ক, পূর্ণাঙ্ক, পূর্ণাঙ্ক *, ধ্রুৱ ৱর্ণ*, PyArena *);
শূন্য _PyPegen_Parser_Free(Parser *);
mod_ty _PyPegen_run_parser_from_file_pointer(FILE *, পূর্ণাঙ্ক, PyObject *, ধ্রুৱ ৱর্ণ *,
                                    ধ্রুৱ ৱর্ণ *, ধ্রুৱ ৱর্ণ *, PyCompilerFlags *, পূর্ণাঙ্ক *, PyObject **,
                                    PyArena *);
শূন্য *_PyPegen_run_parser(Parser *);
mod_ty _PyPegen_run_parser_from_string(ধ্রুৱ ৱর্ণ *, পূর্ণাঙ্ক, PyObject *, PyCompilerFlags *, PyArena *, PyObject *, PyObject **);
asdl_stmt_seq *_PyPegen_interactive_exit(Parser *);

// Generated function in parse.c - function definition in python.gram
শূন্য *_PyPegen_parse(Parser *);

#endif
