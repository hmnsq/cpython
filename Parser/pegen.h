#ifndef PEGEN_H
#define PEGEN_H

#अन्तर्भाव <Python.h>
#अन्तर्भाव <pycore_ast.h>
#अन्तर्भाव <pycore_token.h>
#अन्तर्भाव <pycore_ceval.h>     // _Py_ReachedRecursionLimitWithMargin()
#अन्तर्भाव <pycore_pystate.h>   // _Py_get_machine_stack_pointer()
#अन्तर्भाव <pycore_pythonrun.h> // _PyOS_STACK_MARGIN_BYTES

#अन्तर्भाव "tokenizer/tokenizer.h"

#सचे 0
#define PyPARSE_YIELD_IS_KEYWORD        0x0001
#endif

#define PyPARSE_DONT_IMPLY_DEDENT       0x0002

#सचे 0
#define PyPARSE_WITH_IS_KEYWORD         0x0003
#define PyPARSE_PRINT_IS_FUNCTION       0x0004
#define PyPARSE_UNICODE_LITERALS        0x0008
#endif

#define PyPARSE_IGNORE_COOKIE 0x0010
#define PyPARSE_BARRY_AS_BDFL 0x0020
#define PyPARSE_TYPE_COMMENTS 0x0040
#define Py_PARSE_ALLOW_INCOMPLETE_निवेशः 0x0100

#define CURRENT_POS (-5)

प्रकारनिर्देश संरचना _memo {
    पूर्णाङ्क type;
    शून्य *ग्रन्थिः;
    पूर्णाङ्क mark;
    संरचना _memo *next;
} Memo;

प्रकारनिर्देश संरचना {
    पूर्णाङ्क type;
    // Text required by grammar actions or helpers; otherwise NULL.
    // An unfinished string token also has NULL text. Its grammar action
    // requests more input only if the token is valid at that position.
    PyObject *bytes;
    पूर्णाङ्क level;
    पूर्णाङ्क is_raw;
    पूर्णाङ्क lineno, col_offset, end_lineno, end_col_offset;
    Memo *memo;
    // Filter over the rule types present in `memo` (bit `type & 63` is set
    // for every entry): lets lookups skip walking the list on definite
    // misses, which are the common case.
    uint64_t memo_mask;
    PyObject *metadata;
} Token;

प्रकारनिर्देश संरचना {
    ध्रुव वर्ण *str;
    पूर्णाङ्क type;
} KeywordToken;


प्रकारनिर्देश संरचना {
    संरचना {
        पूर्णाङ्क lineno;
        वर्ण *comment;  // The " <tag>" in "# type: ignore <tag>"
    } *वस्तूनि;
    size_t size;
    size_t अङ्कः_वस्तूनि;
} growable_comment_array;

प्रकारनिर्देश संरचना {
    पूर्णाङ्क lineno;
    पूर्णाङ्क col_offset;
    पूर्णाङ्क end_lineno;
    पूर्णाङ्क end_col_offset;
} location;

प्रकारनिर्देश संरचना _identifier_cache_entry IdentifierCacheEntry;

प्रकारनिर्देश संरचना {
    संरचना tok_state *tok;
    Token **tokens;
    पूर्णाङ्क mark;
    पूर्णाङ्क fill, size;
    PyArena *arena;
    KeywordToken **keywords;
    वर्ण **soft_keywords;
    पूर्णाङ्क n_keyword_lists;
    पूर्णाङ्क start_rule;
    पूर्णाङ्क *errcode;
    unsigned वर्ण parsing_started;
    unsigned वर्ण निवेशः_exhausted;
    PyObject* normalize;
    पूर्णाङ्क starting_lineno;
    पूर्णाङ्क starting_col_offset;
    पूर्णाङ्क error_indicator;
    पूर्णाङ्क flags;
    पूर्णाङ्क feature_version;
    growable_comment_array type_ignore_comments;
    Token *known_err_token;
    पूर्णाङ्क level;
    पूर्णाङ्क call_invalid_rules;
    पूर्णाङ्क debug;
    location last_stmt_location;
    IdentifierCacheEntry *identifier_cache;
    PyThreadState *tstate;
    // Cached copy of tstate->c_stack_soft_limit offset by one margin toward
    // the stack top, so the per-rule stack check is a single inline comparison.
    uintptr_t राशिः_soft_limit;
} Parser;

// C-stack exhaustion check run on every rule entry. The fast path compares
// the machine stack pointer against the soft limit cached in the Parser;
// only when the stack is nearly exhausted is the full check called.
स्थिर अन्तःस्थ पूर्णाङ्क
Py_Pegen_राशिः_exhausted(Parser *p)
{
#सचे Py_राशिः_GROWS_DOWN
    सचे (Py_get_machine_राशिः_pointer() > p->राशिः_soft_limit) {
#अञ्ञथा
    सचे (Py_get_machine_राशिः_pointer() <= p->राशिः_soft_limit) {
#endif
        प्रतिज्ञा(!_Py_ReachedRecursionLimitWithMargin(p->tstate, 1));
        पटिनिवत्त 0;
    }
    पटिनिवत्त _Py_ReachedRecursionLimitWithMargin(p->tstate, 1);
}

प्रकारनिर्देश संरचना {
    cmpop_ty cmpop;
    व्यञ्जकम्_ty व्यञ्जकम्;
} Cmpop_व्यञ्जकम्_Pair;

प्रकारनिर्देश संरचना {
    व्यञ्जकम्_ty कुञ्चिका;
    व्यञ्जकम्_ty मानम्;
} कुञ्चिका_मानम्_Pair;

प्रकारनिर्देश संरचना {
    व्यञ्जकम्_ty कुञ्चिका;
    pattern_ty pattern;
} कुञ्चिका_Pattern_Pair;

प्रकारनिर्देश संरचना {
    arg_ty arg;
    व्यञ्जकम्_ty मानम्;
} NameDefaultPair;

प्रकारनिर्देश संरचना {
    asdl_arg_seq *plain_names;
    asdl_seq *names_with_defaults; // asdl_seq* of NameDefaultsPair's
} SlashWithDefault;

प्रकारनिर्देश संरचना {
    arg_ty vararg;
    asdl_seq *kwonlyargs; // asdl_seq* of NameDefaultsPair's
    arg_ty kwarg;
} StarEtc;

प्रकारनिर्देश संरचना { operator_ty kind; } AugOperator;
प्रकारनिर्देश संरचना {
    शून्य *element;
    पूर्णाङ्क is_keyword;
} KeywordOrStarred;

प्रकारनिर्देश संरचना {
    शून्य *result;
    PyObject *metadata;
} ResultTokenWithMetadata;

// Internal parser functions
#सचे defined(Py_DEBUG)
शून्य _PyPegen_clear_memo_statistics(शून्य);
PyObject *_PyPegen_get_memo_statistics(शून्य);
#endif

पूर्णाङ्क Py_Pegen_निवेशय_memo(Parser *p, पूर्णाङ्क mark, पूर्णाङ्क type, शून्य *ग्रन्थिः);
पूर्णाङ्क _PyPegen_update_memo(Parser *p, पूर्णाङ्क mark, पूर्णाङ्क type, शून्य *ग्रन्थिः);
Memo *Py_Pegen_निवेशय_memo_direct(Parser *p, पूर्णाङ्क mark, पूर्णाङ्क type);
पूर्णाङ्क _PyPegen_is_memoized(Parser *p, पूर्णाङ्क type, शून्य *pres);

पूर्णाङ्क _PyPegen_lookahead(पूर्णाङ्क, शून्य *(func)(Parser *), Parser *);
पूर्णाङ्क Py_Pegen_lookahead_for_व्यञ्जकम्(पूर्णाङ्क, व्यञ्जकम्_ty (func)(Parser *), Parser *);
पूर्णाङ्क _PyPegen_lookahead_for_stmt(पूर्णाङ्क, stmt_ty (func)(Parser *), Parser *);
पूर्णाङ्क _PyPegen_lookahead_with_int(पूर्णाङ्क, Token *(func)(Parser *, पूर्णाङ्क), Parser *, पूर्णाङ्क);
पूर्णाङ्क _PyPegen_lookahead_with_string(पूर्णाङ्क, व्यञ्जकम्_ty (func)(Parser *, ध्रुव वर्ण*), Parser *, ध्रुव वर्ण*);

Token *_PyPegen_expect_token(Parser *p, पूर्णाङ्क type);
शून्य* _PyPegen_expect_forced_result(Parser *p, शून्य* result, ध्रुव वर्ण* expected);
Token *_PyPegen_expect_forced_token(Parser *p, पूर्णाङ्क type, ध्रुव वर्ण* expected);
व्यञ्जकम्_ty _PyPegen_expect_soft_keyword(Parser *p, ध्रुव वर्ण *keyword);
व्यञ्जकम्_ty _PyPegen_soft_keyword_token(Parser *p);
व्यञ्जकम्_ty Py_Pegen_fstring_मध्यः_token(Parser* p);
Token *_PyPegen_get_last_nonnwhitespace_token(Parser *);
पूर्णाङ्क _PyPegen_fill_token(Parser *p);
// Return -1 on failure, leaving the buffer and capacity unchanged.
पूर्णाङ्क _PyPegen_grow_loop_buffer(शून्य ***buffer, Py_ssize_t *capacity);
व्यञ्जकम्_ty _PyPegen_name_token(Parser *p);
व्यञ्जकम्_ty Py_Pegen_सङ्ख्या_token(Parser *p);
शून्य *_PyPegen_string_token(Parser *p);
Py_ssize_t Py_Pegen_byte_offset_to_character_offset_पङ्क्तिः(PyObject *पङ्क्तिः, Py_ssize_t col_offset, Py_ssize_t end_col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset(PyObject *पङ्क्तिः, Py_ssize_t col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset_raw(ध्रुव वर्ण*, Py_ssize_t col_offset);

प्रकारनिर्देश गणना {
    STAR_TARGETS,
    DEL_TARGETS,
    FOR_TARGETS,
    SINGLE_TARGETS,
    ATTRIBUTE_OR_SUBSCRIPT_TARGETS
} TARGETS_TYPE;

// Error handling functions and APIs
पूर्णाङ्क _Pypegen_raise_decode_error(Parser *p);
पूर्णाङ्क _Pypegen_tokenizer_error(Parser *p);
शून्य *Py_Pegen_raise_incomplete_निवेशः(Parser *p);
शून्य *_PyPegen_raise_error(Parser *p, PyObject *errtype, पूर्णाङ्क use_mark, ध्रुव वर्ण *errmsg, ...);
शून्य *_PyPegen_raise_error_known_location(Parser *p, PyObject *errtype,
                                          Py_ssize_t lineno, Py_ssize_t col_offset,
                                          Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                                          ध्रुव वर्ण *errmsg, va_list va);
शून्य _Pypegen_set_syntax_error(Parser* p, Token* last_token);
शून्य Pypegen_राशिः_overflow(Parser *p);

स्थिर अन्तःस्थ शून्य *
RAISE_ERROR_KNOWN_LOCATION(Parser *p, PyObject *errtype,
                           Py_ssize_t lineno, Py_ssize_t col_offset,
                           Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                           ध्रुव वर्ण *errmsg, ...)
{
    va_list va;
    va_start(va, errmsg);
    Py_ssize_t _col_offset = (col_offset == CURRENT_POS ? CURRENT_POS : col_offset + 1);
    Py_ssize_t _end_col_offset = (end_col_offset == CURRENT_POS ? CURRENT_POS : end_col_offset + 1);
    _PyPegen_raise_error_known_location(p, errtype, lineno, _col_offset, end_lineno, _end_col_offset, errmsg, va);
    va_end(va);
    पटिनिवत्त रिक्त;
}
#define RAISE_SYNTAX_ERROR(सन्देशः, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 0, सन्देशः, ##__VA_ARGS__)
#define RAISE_INDENTATION_ERROR(सन्देशः, ...) _PyPegen_raise_error(p, PyExc_IndentationError, 0, सन्देशः, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_ON_NEXT_TOKEN(सन्देशः, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 1, सन्देशः, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_RANGE(a, b, सन्देशः, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (b)->end_lineno, (b)->end_col_offset, सन्देशः, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_LOCATION(a, सन्देशः, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (a)->end_lineno, (a)->end_col_offset, सन्देशः, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_STARTING_FROM(a, सन्देशः, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, CURRENT_POS, CURRENT_POS, सन्देशः, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_INVALID_TARGET(type, e) _RAISE_SYNTAX_ERROR_INVALID_TARGET(p, type, e)

Py_LOCAL_INLINE(पूर्णाङ्क)
_PyPegen_tokens_are_adjacent(Token *a, Token *b)
{
    पटिनिवत्त (a->end_lineno == b->lineno) && (a->end_col_offset == b->col_offset);
}

Py_LOCAL_INLINE(शून्य *)
CHECK_CALL(Parser *p, शून्य *result)
{
    सचे (result == रिक्त) {
        प्रतिज्ञा(PyErr_Occurred());
        p->error_indicator = 1;
    }
    पटिनिवत्त result;
}

/* This is needed for helper functions that are allowed to
   return NULL without an error. Example: _PyPegen_seq_extract_starred_exprs */
Py_LOCAL_INLINE(शून्य *)
CHECK_CALL_NULL_ALLOWED(Parser *p, शून्य *result)
{
    सचे (result == रिक्त && PyErr_Occurred()) {
        p->error_indicator = 1;
    }
    पटिनिवत्त result;
}

#define CHECK(type, result) ((type) CHECK_CALL(p, result))
#define CHECK_NULL_ALLOWED(type, result) ((type) CHECK_CALL_NULL_ALLOWED(p, result))

व्यञ्जकम्_ty _PyPegen_get_invalid_target(व्यञ्जकम्_ty e, TARGETS_TYPE targets_type);
ध्रुव वर्ण *Py_Pegen_get_व्यञ्जकम्_name(व्यञ्जकम्_ty);
Py_LOCAL_INLINE(शून्य *)
_RAISE_SYNTAX_ERROR_INVALID_TARGET(Parser *p, TARGETS_TYPE type, शून्य *e)
{
    व्यञ्जकम्_ty invalid_target = CHECK_NULL_ALLOWED(व्यञ्जकम्_ty, _PyPegen_get_invalid_target(e, type));
    सचे (invalid_target != रिक्त) {
        ध्रुव वर्ण *सन्देशः;
        सचे (type == STAR_TARGETS || type == FOR_TARGETS) {
            सन्देशः = "cannot assign to %s";
        }
        अञ्ञथा {
            सन्देशः = "cannot delete %s";
        }
        पटिनिवत्त RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
            invalid_target,
            सन्देशः,
            Py_Pegen_get_व्यञ्जकम्_name(invalid_target)
        );
        पटिनिवत्त RAISE_SYNTAX_ERROR_KNOWN_LOCATION(invalid_target, "invalid syntax");
    }
    पटिनिवत्त रिक्त;
}

// Action utility functions

शून्य *_PyPegen_dummy_name(Parser *p, ...);
शून्य * _PyPegen_seq_last_item(asdl_seq *seq);
#define PyPegen_last_item(seq, type) ((type)_PyPegen_seq_last_item((asdl_seq*)seq))
शून्य * _PyPegen_seq_first_item(asdl_seq *seq);
#define PyPegen_first_item(seq, type) ((type)_PyPegen_seq_first_item((asdl_seq*)seq))
#define UNUSED(व्यञ्जकम्) करोहि { (शून्य)(व्यञ्जकम्); } याव (0)
#define EXTRA_व्यञ्जकम्(head, tail) head->lineno, (head)->col_offset, (tail)->end_lineno, (tail)->end_col_offset, p->arena
#define EXTRA _start_lineno, _start_col_offset, _end_lineno, _end_col_offset, p->arena
PyObject *_PyPegen_new_type_comment(Parser *, ध्रुव वर्ण *);

Py_LOCAL_INLINE(PyObject *)
NEW_TYPE_COMMENT(Parser *p, Token *tc)
{
    सचे (tc == रिक्त) {
        पटिनिवत्त रिक्त;
    }
    ध्रुव वर्ण *bytes = PyBytes_AsString(tc->bytes);
    सचे (bytes == रिक्त) {
        गच्छ error;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    सचे (tco == रिक्त) {
        गच्छ error;
    }
    पटिनिवत्त tco;
 error:
    p->error_indicator = 1;  // Inline CHECK_CALL
    पटिनिवत्त रिक्त;
}

Py_LOCAL_INLINE(शून्य *)
INVALID_VERSION_CHECK(Parser *p, पूर्णाङ्क version, वर्ण *सन्देशः, शून्य *ग्रन्थिः)
{
    सचे (ग्रन्थिः == रिक्त) {
        p->error_indicator = 1;  // Inline CHECK_CALL
        पटिनिवत्त रिक्त;
    }
    सचे (p->feature_version < version) {
        p->error_indicator = 1;
        पटिनिवत्त RAISE_SYNTAX_ERROR("%s only supported in Python 3.%i and greater",
                                  सन्देशः, version);
    }
    पटिनिवत्त ग्रन्थिः;
}

#define CHECK_VERSION(type, version, सन्देशः, ग्रन्थिः) ((type) INVALID_VERSION_CHECK(p, version, सन्देशः, ग्रन्थिः))

arg_ty _PyPegen_add_type_comment_to_arg(Parser *, arg_ty, Token *);
PyObject *_PyPegen_new_identifier(Parser *, ध्रुव वर्ण *);
asdl_seq *_PyPegen_singleton_seq(Parser *, शून्य *);
asdl_seq *Py_Pegen_seq_निवेशय_in_front(Parser *, शून्य *, asdl_seq *);
asdl_seq *_PyPegen_seq_append_to_end(Parser *, asdl_seq *, शून्य *);
asdl_seq *_PyPegen_seq_flatten(Parser *, asdl_seq *);
व्यञ्जकम्_ty _PyPegen_join_names_with_dot(Parser *, व्यञ्जकम्_ty, व्यञ्जकम्_ty);
पूर्णाङ्क Py_Pegen_seq_गणनम्_dots(asdl_seq *);
alias_ty _PyPegen_alias_for_star(Parser *, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, PyArena *);
asdl_identifier_seq *_PyPegen_map_names_to_ids(Parser *, asdl_व्यञ्जकम्_seq *);
Cmpop_व्यञ्जकम्_Pair *Py_Pegen_cmpop_व्यञ्जकम्_pair(Parser *, cmpop_ty, व्यञ्जकम्_ty);
asdl_int_seq *_PyPegen_get_cmpops(Parser *p, asdl_seq *);
asdl_व्यञ्जकम्_seq *_PyPegen_get_exprs(Parser *, asdl_seq *);
व्यञ्जकम्_ty Py_Pegen_set_व्यञ्जकम्_context(Parser *, व्यञ्जकम्_ty, व्यञ्जकम्_context_ty);
व्यञ्जकम्_ty _PyPegen_make_target(Parser *, व्यञ्जकम्_ty, TARGETS_TYPE);
कुञ्चिका_मानम्_Pair *Py_Pegen_कुञ्चिका_मानम्_pair(Parser *, व्यञ्जकम्_ty, व्यञ्जकम्_ty);
asdl_व्यञ्जकम्_seq *Py_Pegen_get_कुञ्चिकाः(Parser *, asdl_seq *);
asdl_व्यञ्जकम्_seq *Py_Pegen_get_मानानि(Parser *, asdl_seq *);
कुञ्चिका_Pattern_Pair *Py_Pegen_कुञ्चिका_pattern_pair(Parser *, व्यञ्जकम्_ty, pattern_ty);
asdl_व्यञ्जकम्_seq *Py_Pegen_get_pattern_कुञ्चिकाः(Parser *, asdl_seq *);
asdl_pattern_seq *_PyPegen_get_patterns(Parser *, asdl_seq *);
NameDefaultPair *_PyPegen_name_default_pair(Parser *, arg_ty, व्यञ्जकम्_ty, Token *);
SlashWithDefault *_PyPegen_slash_with_default(Parser *, asdl_arg_seq *, asdl_seq *);
StarEtc *_PyPegen_star_etc(Parser *, arg_ty, asdl_seq *, arg_ty);
arguments_ty _PyPegen_make_arguments(Parser *, asdl_arg_seq *, SlashWithDefault *,
                                     asdl_arg_seq *, asdl_seq *, StarEtc *);
arguments_ty _PyPegen_empty_arguments(Parser *);
व्यञ्जकम्_ty _PyPegen_template_str(Parser *p, Token *a, asdl_व्यञ्जकम्_seq *raw_expressions, Token *b);
व्यञ्जकम्_ty _PyPegen_joined_str(Parser *p, Token *a, asdl_व्यञ्जकम्_seq *raw_expressions, Token *b);
व्यञ्जकम्_ty _PyPegen_interpolation(Parser *, व्यञ्जकम्_ty, Token *, ResultTokenWithMetadata *, ResultTokenWithMetadata *, Token *,
                                 पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, PyArena *);
व्यञ्जकम्_ty Py_Pegen_formatted_मानम्(Parser *, व्यञ्जकम्_ty, Token *, ResultTokenWithMetadata *, ResultTokenWithMetadata *, Token *,
                                 पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, PyArena *);
AugOperator *_PyPegen_augoperator(Parser*, operator_ty type);
stmt_ty _PyPegen_function_def_decorators(Parser *, asdl_व्यञ्जकम्_seq *, stmt_ty);
stmt_ty _PyPegen_class_def_decorators(Parser *, asdl_व्यञ्जकम्_seq *, stmt_ty);
KeywordOrStarred *_PyPegen_keyword_or_starred(Parser *, शून्य *, पूर्णाङ्क);
asdl_व्यञ्जकम्_seq *_PyPegen_seq_extract_starred_exprs(Parser *, asdl_seq *);
asdl_keyword_seq *_PyPegen_seq_delete_starred_exprs(Parser *, asdl_seq *);
व्यञ्जकम्_ty _PyPegen_collect_call_seqs(Parser *, asdl_व्यञ्जकम्_seq *, asdl_seq *,
                     पूर्णाङ्क lineno, पूर्णाङ्क col_offset, पूर्णाङ्क end_lineno,
                     पूर्णाङ्क end_col_offset, PyArena *arena);
व्यञ्जकम्_ty _PyPegen_constant_from_token(Parser* p, Token* tok);
व्यञ्जकम्_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok);
व्यञ्जकम्_ty _PyPegen_constant_from_string(Parser* p, Token* tok);
व्यञ्जकम्_ty _PyPegen_concatenate_tstrings(Parser *p, asdl_व्यञ्जकम्_seq *, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, PyArena *);
व्यञ्जकम्_ty _PyPegen_concatenate_strings(Parser *p, asdl_व्यञ्जकम्_seq *, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, PyArena *);
व्यञ्जकम्_ty _PyPegen_FetchRawForm(Parser *p, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क);
व्यञ्जकम्_ty _PyPegen_ensure_imaginary(Parser *p, व्यञ्जकम्_ty);
व्यञ्जकम्_ty _PyPegen_ensure_real(Parser *p, व्यञ्जकम्_ty);
asdl_seq *_PyPegen_join_sequences(Parser *, asdl_seq *, asdl_seq *);
पूर्णाङ्क _PyPegen_check_barry_as_flufl(Parser *, Token *);
पूर्णाङ्क _PyPegen_check_legacy_stmt(Parser *p, व्यञ्जकम्_ty t);
शून्य *_PyPegen_raise_error_for_missing_comma(Parser *p, व्यञ्जकम्_ty a, व्यञ्जकम्_ty b);
ResultTokenWithMetadata *_PyPegen_check_fstring_conversion(Parser *p, Token *, व्यञ्जकम्_ty t);
ResultTokenWithMetadata *_PyPegen_setup_full_format_spec(Parser *, Token *, asdl_व्यञ्जकम्_seq *, पूर्णाङ्क, पूर्णाङ्क,
                                                         पूर्णाङ्क, पूर्णाङ्क, PyArena *);
mod_ty _PyPegen_make_module(Parser *, asdl_stmt_seq *);
शून्य *_PyPegen_arguments_parsing_error(Parser *, व्यञ्जकम्_ty);
व्यञ्जकम्_ty _PyPegen_get_last_comprehension_item(comprehension_ty comprehension);
शून्य *_PyPegen_nonparen_genexp_in_call(Parser *p, व्यञ्जकम्_ty args, asdl_comprehension_seq *comprehensions);
stmt_ty _PyPegen_checked_from_import(Parser *p, asdl_seq *dots, व्यञ्जकम्_ty module_name,
                                     asdl_alias_seq *names, व्यञ्जकम्_ty lazy_token, पूर्णाङ्क lineno,
                                     पूर्णाङ्क col_offset, पूर्णाङ्क end_lineno, पूर्णाङ्क end_col_offset,
                                     PyArena *arena);
asdl_stmt_seq* _PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts);
stmt_ty _PyPegen_register_stmt(Parser *p, stmt_ty s);

// Parser API

Parser *_PyPegen_Parser_New(संरचना tok_state *, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क *, ध्रुव वर्ण*, PyArena *);
शून्य _PyPegen_Parser_Free(Parser *);
mod_ty _PyPegen_run_parser_from_file_pointer(FILE *, पूर्णाङ्क, PyObject *, ध्रुव वर्ण *,
                                    ध्रुव वर्ण *, ध्रुव वर्ण *, PyCompilerFlags *, पूर्णाङ्क *, PyObject **,
                                    PyArena *);
शून्य *_PyPegen_run_parser(Parser *);
mod_ty _PyPegen_run_parser_from_string(ध्रुव वर्ण *, पूर्णाङ्क, PyObject *, PyCompilerFlags *, PyArena *, PyObject *, PyObject **);
asdl_stmt_seq *_PyPegen_interactive_exit(Parser *);

// Generated function in parse.c - function definition in python.gram
शून्य *_PyPegen_parse(Parser *);

#endif
