#ifndef PEGEN_H
#define PEGEN_H

#अन्तर्भाव <Python.h>
#अन्तर्भाव <pycore_ast.h>
#अन्तर्भाव <pycore_token.h>
#अन्तर्भाव <pycore_ceval.h>     // _Py_ReachedRecursionLimitWithMargin()
#अन्तर्भाव <pycore_pystate.h>   // _Py_get_machine_stack_pointer()
#अन्तर्भाव <pycore_pythonrun.h> // _PyOS_STACK_MARGIN_BYTES

#अन्तर्भाव "tokenizer/tokenizer.h"

#जर 0
#define PyPARSE_YIELD_IS_KEYWORD        0x0001
#endif

#define PyPARSE_DONT_IMPLY_DEDENT       0x0002

#जर 0
#define PyPARSE_WITH_IS_KEYWORD         0x0003
#define PyPARSE_PRINT_IS_FUNCTION       0x0004
#define PyPARSE_UNICODE_LITERALS        0x0008
#endif

#define PyPARSE_IGNORE_COOKIE 0x0010
#define PyPARSE_BARRY_AS_BDFL 0x0020
#define PyPARSE_TYPE_COMMENTS 0x0040
#define Py_PARSE_ALLOW_INCOMPLETE_आदान 0x0100

#define CURRENT_POS (-5)

प्रकारव्याख्या संरचना _memo {
    पूर्णांक type;
    शून्य *गाठ;
    पूर्णांक mark;
    संरचना _memo *next;
} Memo;

प्रकारव्याख्या संरचना {
    पूर्णांक type;
    // Text required by grammar actions or helpers; otherwise NULL.
    // An unfinished string token also has NULL text. Its grammar action
    // requests more input only if the token is valid at that position.
    PyObject *bytes;
    पूर्णांक level;
    पूर्णांक is_raw;
    पूर्णांक lineno, col_offset, end_lineno, end_col_offset;
    Memo *memo;
    // Filter over the rule types present in `memo` (bit `type & 63` is set
    // for every entry): lets lookups skip walking the list on definite
    // misses, which are the common case.
    uint64_t memo_mask;
    PyObject *metadata;
} Token;

प्रकारव्याख्या संरचना {
    स्थिरांक अक्षर *str;
    पूर्णांक type;
} KeywordToken;


प्रकारव्याख्या संरचना {
    संरचना {
        पूर्णांक lineno;
        अक्षर *comment;  // The " <tag>" in "# type: ignore <tag>"
    } *वस्तुसंच;
    माप_प्रकार size;
    माप_प्रकार अंक_वस्तुसंच;
} growable_comment_array;

प्रकारव्याख्या संरचना {
    पूर्णांक lineno;
    पूर्णांक col_offset;
    पूर्णांक end_lineno;
    पूर्णांक end_col_offset;
} location;

प्रकारव्याख्या संरचना _identifier_cache_entry IdentifierCacheEntry;

प्रकारव्याख्या संरचना {
    संरचना tok_state *tok;
    Token **tokens;
    पूर्णांक mark;
    पूर्णांक fill, size;
    PyArena *arena;
    KeywordToken **keywords;
    अक्षर **soft_keywords;
    पूर्णांक n_keyword_lists;
    पूर्णांक start_rule;
    पूर्णांक *errcode;
    अचिन्हित अक्षर parsing_started;
    अचिन्हित अक्षर आदान_exhausted;
    PyObject* normalize;
    पूर्णांक starting_lineno;
    पूर्णांक starting_col_offset;
    पूर्णांक error_indicator;
    पूर्णांक flags;
    पूर्णांक feature_version;
    growable_comment_array type_ignore_comments;
    Token *known_err_token;
    पूर्णांक level;
    पूर्णांक call_invalid_rules;
    पूर्णांक debug;
    location last_stmt_location;
    IdentifierCacheEntry *identifier_cache;
    PyThreadState *tstate;
    // Cached copy of tstate->c_stack_soft_limit offset by one margin toward
    // the stack top, so the per-rule stack check is a single inline comparison.
    uintptr_t चळत_soft_limit;
} Parser;

// C-stack exhaustion check run on every rule entry. The fast path compares
// the machine stack pointer against the soft limit cached in the Parser;
// only when the stack is nearly exhausted is the full check called.
स्थिर ओळीत पूर्णांक
Py_Pegen_चळत_exhausted(Parser *p)
{
#जर Py_चळत_GROWS_DOWN
    जर (Py_get_machine_चळत_pointer() > p->चळत_soft_limit) {
#अन्यथा
    जर (Py_get_machine_चळत_pointer() <= p->चळत_soft_limit) {
#endif
        प्रतिज्ञा(!_Py_ReachedRecursionLimitWithMargin(p->tstate, 1));
        परत 0;
    }
    परत _Py_ReachedRecursionLimitWithMargin(p->tstate, 1);
}

प्रकारव्याख्या संरचना {
    cmpop_ty cmpop;
    अभिव्यक्ती_ty अभिव्यक्ती;
} Cmpop_अभिव्यक्ती_Pair;

प्रकारव्याख्या संरचना {
    अभिव्यक्ती_ty किल्ली;
    अभिव्यक्ती_ty मूल्य;
} किल्ली_मूल्य_Pair;

प्रकारव्याख्या संरचना {
    अभिव्यक्ती_ty किल्ली;
    pattern_ty pattern;
} किल्ली_Pattern_Pair;

प्रकारव्याख्या संरचना {
    arg_ty arg;
    अभिव्यक्ती_ty मूल्य;
} NameDefaultPair;

प्रकारव्याख्या संरचना {
    asdl_arg_seq *plain_names;
    asdl_seq *names_with_defaults; // asdl_seq* of NameDefaultsPair's
} SlashWithDefault;

प्रकारव्याख्या संरचना {
    arg_ty vararg;
    asdl_seq *kwonlyargs; // asdl_seq* of NameDefaultsPair's
    arg_ty kwarg;
} StarEtc;

प्रकारव्याख्या संरचना { operator_ty kind; } AugOperator;
प्रकारव्याख्या संरचना {
    शून्य *element;
    पूर्णांक is_keyword;
} KeywordOrStarred;

प्रकारव्याख्या संरचना {
    शून्य *result;
    PyObject *metadata;
} ResultTokenWithMetadata;

// Internal parser functions
#जर defined(Py_DEBUG)
शून्य _PyPegen_clear_memo_statistics(शून्य);
PyObject *_PyPegen_get_memo_statistics(शून्य);
#endif

पूर्णांक Py_Pegen_घाला_memo(Parser *p, पूर्णांक mark, पूर्णांक type, शून्य *गाठ);
पूर्णांक _PyPegen_update_memo(Parser *p, पूर्णांक mark, पूर्णांक type, शून्य *गाठ);
Memo *Py_Pegen_घाला_memo_direct(Parser *p, पूर्णांक mark, पूर्णांक type);
पूर्णांक _PyPegen_is_memoized(Parser *p, पूर्णांक type, शून्य *pres);

पूर्णांक _PyPegen_lookahead(पूर्णांक, शून्य *(func)(Parser *), Parser *);
पूर्णांक Py_Pegen_lookahead_for_अभिव्यक्ती(पूर्णांक, अभिव्यक्ती_ty (func)(Parser *), Parser *);
पूर्णांक _PyPegen_lookahead_for_stmt(पूर्णांक, stmt_ty (func)(Parser *), Parser *);
पूर्णांक _PyPegen_lookahead_with_int(पूर्णांक, Token *(func)(Parser *, पूर्णांक), Parser *, पूर्णांक);
पूर्णांक _PyPegen_lookahead_with_string(पूर्णांक, अभिव्यक्ती_ty (func)(Parser *, स्थिरांक अक्षर*), Parser *, स्थिरांक अक्षर*);

Token *_PyPegen_expect_token(Parser *p, पूर्णांक type);
शून्य* _PyPegen_expect_forced_result(Parser *p, शून्य* result, स्थिरांक अक्षर* expected);
Token *_PyPegen_expect_forced_token(Parser *p, पूर्णांक type, स्थिरांक अक्षर* expected);
अभिव्यक्ती_ty _PyPegen_expect_soft_keyword(Parser *p, स्थिरांक अक्षर *keyword);
अभिव्यक्ती_ty _PyPegen_soft_keyword_token(Parser *p);
अभिव्यक्ती_ty Py_Pegen_fstring_मधला_token(Parser* p);
Token *_PyPegen_get_last_nonnwhitespace_token(Parser *);
पूर्णांक _PyPegen_fill_token(Parser *p);
// Return -1 on failure, leaving the buffer and capacity unchanged.
पूर्णांक _PyPegen_grow_loop_buffer(शून्य ***buffer, Py_ssize_t *capacity);
अभिव्यक्ती_ty _PyPegen_name_token(Parser *p);
अभिव्यक्ती_ty Py_Pegen_संख्या_token(Parser *p);
शून्य *_PyPegen_string_token(Parser *p);
Py_ssize_t Py_Pegen_byte_offset_to_character_offset_ओळ(PyObject *ओळ, Py_ssize_t col_offset, Py_ssize_t end_col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset(PyObject *ओळ, Py_ssize_t col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset_raw(स्थिरांक अक्षर*, Py_ssize_t col_offset);

प्रकारव्याख्या गणना {
    STAR_TARGETS,
    DEL_TARGETS,
    FOR_TARGETS,
    SINGLE_TARGETS,
    ATTRIBUTE_OR_SUBSCRIPT_TARGETS
} TARGETS_TYPE;

// Error handling functions and APIs
पूर्णांक _Pypegen_raise_decode_error(Parser *p);
पूर्णांक _Pypegen_tokenizer_error(Parser *p);
शून्य *Py_Pegen_raise_incomplete_आदान(Parser *p);
शून्य *_PyPegen_raise_error(Parser *p, PyObject *errtype, पूर्णांक use_mark, स्थिरांक अक्षर *errmsg, ...);
शून्य *_PyPegen_raise_error_known_location(Parser *p, PyObject *errtype,
                                          Py_ssize_t lineno, Py_ssize_t col_offset,
                                          Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                                          स्थिरांक अक्षर *errmsg, बहु_सूची va);
शून्य _Pypegen_set_syntax_error(Parser* p, Token* last_token);
शून्य Pypegen_चळत_overflow(Parser *p);

स्थिर ओळीत शून्य *
RAISE_ERROR_KNOWN_LOCATION(Parser *p, PyObject *errtype,
                           Py_ssize_t lineno, Py_ssize_t col_offset,
                           Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                           स्थिरांक अक्षर *errmsg, ...)
{
    बहु_सूची va;
    बहु_शुरू(va, errmsg);
    Py_ssize_t _col_offset = (col_offset == CURRENT_POS ? CURRENT_POS : col_offset + 1);
    Py_ssize_t _end_col_offset = (end_col_offset == CURRENT_POS ? CURRENT_POS : end_col_offset + 1);
    _PyPegen_raise_error_known_location(p, errtype, lineno, _col_offset, end_lineno, _end_col_offset, errmsg, va);
    बहु_पूर्ण(va);
    परत रिक्त;
}
#define RAISE_SYNTAX_ERROR(संदेश, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 0, संदेश, ##__VA_ARGS__)
#define RAISE_INDENTATION_ERROR(संदेश, ...) _PyPegen_raise_error(p, PyExc_IndentationError, 0, संदेश, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_ON_NEXT_TOKEN(संदेश, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 1, संदेश, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_RANGE(a, b, संदेश, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (b)->end_lineno, (b)->end_col_offset, संदेश, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_LOCATION(a, संदेश, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (a)->end_lineno, (a)->end_col_offset, संदेश, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_STARTING_FROM(a, संदेश, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, CURRENT_POS, CURRENT_POS, संदेश, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_INVALID_TARGET(type, e) _RAISE_SYNTAX_ERROR_INVALID_TARGET(p, type, e)

Py_LOCAL_INLINE(पूर्णांक)
_PyPegen_tokens_are_adjacent(Token *a, Token *b)
{
    परत (a->end_lineno == b->lineno) && (a->end_col_offset == b->col_offset);
}

Py_LOCAL_INLINE(शून्य *)
CHECK_CALL(Parser *p, शून्य *result)
{
    जर (result == रिक्त) {
        प्रतिज्ञा(PyErr_Occurred());
        p->error_indicator = 1;
    }
    परत result;
}

/* This is needed for helper functions that are allowed to
   return NULL without an error. Example: _PyPegen_seq_extract_starred_exprs */
Py_LOCAL_INLINE(शून्य *)
CHECK_CALL_NULL_ALLOWED(Parser *p, शून्य *result)
{
    जर (result == रिक्त && PyErr_Occurred()) {
        p->error_indicator = 1;
    }
    परत result;
}

#define CHECK(type, result) ((type) CHECK_CALL(p, result))
#define CHECK_NULL_ALLOWED(type, result) ((type) CHECK_CALL_NULL_ALLOWED(p, result))

अभिव्यक्ती_ty _PyPegen_get_invalid_target(अभिव्यक्ती_ty e, TARGETS_TYPE targets_type);
स्थिरांक अक्षर *Py_Pegen_get_अभिव्यक्ती_name(अभिव्यक्ती_ty);
Py_LOCAL_INLINE(शून्य *)
_RAISE_SYNTAX_ERROR_INVALID_TARGET(Parser *p, TARGETS_TYPE type, शून्य *e)
{
    अभिव्यक्ती_ty invalid_target = CHECK_NULL_ALLOWED(अभिव्यक्ती_ty, _PyPegen_get_invalid_target(e, type));
    जर (invalid_target != रिक्त) {
        स्थिरांक अक्षर *संदेश;
        जर (type == STAR_TARGETS || type == FOR_TARGETS) {
            संदेश = "cannot assign to %s";
        }
        अन्यथा {
            संदेश = "cannot delete %s";
        }
        परत RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
            invalid_target,
            संदेश,
            Py_Pegen_get_अभिव्यक्ती_name(invalid_target)
        );
        परत RAISE_SYNTAX_ERROR_KNOWN_LOCATION(invalid_target, "invalid syntax");
    }
    परत रिक्त;
}

// Action utility functions

शून्य *_PyPegen_dummy_name(Parser *p, ...);
शून्य * _PyPegen_seq_last_item(asdl_seq *seq);
#define PyPegen_last_item(seq, type) ((type)_PyPegen_seq_last_item((asdl_seq*)seq))
शून्य * _PyPegen_seq_first_item(asdl_seq *seq);
#define PyPegen_first_item(seq, type) ((type)_PyPegen_seq_first_item((asdl_seq*)seq))
#define UNUSED(अभिव्यक्ती) करा { (शून्य)(अभिव्यक्ती); } पर्यंत (0)
#define EXTRA_अभिव्यक्ती(head, tail) head->lineno, (head)->col_offset, (tail)->end_lineno, (tail)->end_col_offset, p->arena
#define EXTRA _start_lineno, _start_col_offset, _end_lineno, _end_col_offset, p->arena
PyObject *_PyPegen_new_type_comment(Parser *, स्थिरांक अक्षर *);

Py_LOCAL_INLINE(PyObject *)
NEW_TYPE_COMMENT(Parser *p, Token *tc)
{
    जर (tc == रिक्त) {
        परत रिक्त;
    }
    स्थिरांक अक्षर *bytes = PyBytes_AsString(tc->bytes);
    जर (bytes == रिक्त) {
        जा error;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    जर (tco == रिक्त) {
        जा error;
    }
    परत tco;
 error:
    p->error_indicator = 1;  // Inline CHECK_CALL
    परत रिक्त;
}

Py_LOCAL_INLINE(शून्य *)
INVALID_VERSION_CHECK(Parser *p, पूर्णांक version, अक्षर *संदेश, शून्य *गाठ)
{
    जर (गाठ == रिक्त) {
        p->error_indicator = 1;  // Inline CHECK_CALL
        परत रिक्त;
    }
    जर (p->feature_version < version) {
        p->error_indicator = 1;
        परत RAISE_SYNTAX_ERROR("%s only supported in Python 3.%i and greater",
                                  संदेश, version);
    }
    परत गाठ;
}

#define CHECK_VERSION(type, version, संदेश, गाठ) ((type) INVALID_VERSION_CHECK(p, version, संदेश, गाठ))

arg_ty _PyPegen_add_type_comment_to_arg(Parser *, arg_ty, Token *);
PyObject *_PyPegen_new_identifier(Parser *, स्थिरांक अक्षर *);
asdl_seq *_PyPegen_singleton_seq(Parser *, शून्य *);
asdl_seq *Py_Pegen_seq_घाला_in_front(Parser *, शून्य *, asdl_seq *);
asdl_seq *_PyPegen_seq_append_to_end(Parser *, asdl_seq *, शून्य *);
asdl_seq *_PyPegen_seq_flatten(Parser *, asdl_seq *);
अभिव्यक्ती_ty _PyPegen_join_names_with_dot(Parser *, अभिव्यक्ती_ty, अभिव्यक्ती_ty);
पूर्णांक Py_Pegen_seq_मोजणी_dots(asdl_seq *);
alias_ty _PyPegen_alias_for_star(Parser *, पूर्णांक, पूर्णांक, पूर्णांक, पूर्णांक, PyArena *);
asdl_identifier_seq *_PyPegen_map_names_to_ids(Parser *, asdl_अभिव्यक्ती_seq *);
Cmpop_अभिव्यक्ती_Pair *Py_Pegen_cmpop_अभिव्यक्ती_pair(Parser *, cmpop_ty, अभिव्यक्ती_ty);
asdl_int_seq *_PyPegen_get_cmpops(Parser *p, asdl_seq *);
asdl_अभिव्यक्ती_seq *_PyPegen_get_exprs(Parser *, asdl_seq *);
अभिव्यक्ती_ty Py_Pegen_set_अभिव्यक्ती_context(Parser *, अभिव्यक्ती_ty, अभिव्यक्ती_context_ty);
अभिव्यक्ती_ty _PyPegen_make_target(Parser *, अभिव्यक्ती_ty, TARGETS_TYPE);
किल्ली_मूल्य_Pair *Py_Pegen_किल्ली_मूल्य_pair(Parser *, अभिव्यक्ती_ty, अभिव्यक्ती_ty);
asdl_अभिव्यक्ती_seq *Py_Pegen_get_किल्ल्या(Parser *, asdl_seq *);
asdl_अभिव्यक्ती_seq *Py_Pegen_get_मूल्ये(Parser *, asdl_seq *);
किल्ली_Pattern_Pair *Py_Pegen_किल्ली_pattern_pair(Parser *, अभिव्यक्ती_ty, pattern_ty);
asdl_अभिव्यक्ती_seq *Py_Pegen_get_pattern_किल्ल्या(Parser *, asdl_seq *);
asdl_pattern_seq *_PyPegen_get_patterns(Parser *, asdl_seq *);
NameDefaultPair *_PyPegen_name_default_pair(Parser *, arg_ty, अभिव्यक्ती_ty, Token *);
SlashWithDefault *_PyPegen_slash_with_default(Parser *, asdl_arg_seq *, asdl_seq *);
StarEtc *_PyPegen_star_etc(Parser *, arg_ty, asdl_seq *, arg_ty);
arguments_ty _PyPegen_make_arguments(Parser *, asdl_arg_seq *, SlashWithDefault *,
                                     asdl_arg_seq *, asdl_seq *, StarEtc *);
arguments_ty _PyPegen_empty_arguments(Parser *);
अभिव्यक्ती_ty _PyPegen_template_str(Parser *p, Token *a, asdl_अभिव्यक्ती_seq *raw_expressions, Token *b);
अभिव्यक्ती_ty _PyPegen_joined_str(Parser *p, Token *a, asdl_अभिव्यक्ती_seq *raw_expressions, Token *b);
अभिव्यक्ती_ty _PyPegen_interpolation(Parser *, अभिव्यक्ती_ty, Token *, ResultTokenWithMetadata *, ResultTokenWithMetadata *, Token *,
                                 पूर्णांक, पूर्णांक, पूर्णांक, पूर्णांक, PyArena *);
अभिव्यक्ती_ty Py_Pegen_formatted_मूल्य(Parser *, अभिव्यक्ती_ty, Token *, ResultTokenWithMetadata *, ResultTokenWithMetadata *, Token *,
                                 पूर्णांक, पूर्णांक, पूर्णांक, पूर्णांक, PyArena *);
AugOperator *_PyPegen_augoperator(Parser*, operator_ty type);
stmt_ty _PyPegen_function_def_decorators(Parser *, asdl_अभिव्यक्ती_seq *, stmt_ty);
stmt_ty _PyPegen_class_def_decorators(Parser *, asdl_अभिव्यक्ती_seq *, stmt_ty);
KeywordOrStarred *_PyPegen_keyword_or_starred(Parser *, शून्य *, पूर्णांक);
asdl_अभिव्यक्ती_seq *_PyPegen_seq_extract_starred_exprs(Parser *, asdl_seq *);
asdl_keyword_seq *_PyPegen_seq_delete_starred_exprs(Parser *, asdl_seq *);
अभिव्यक्ती_ty _PyPegen_collect_call_seqs(Parser *, asdl_अभिव्यक्ती_seq *, asdl_seq *,
                     पूर्णांक lineno, पूर्णांक col_offset, पूर्णांक end_lineno,
                     पूर्णांक end_col_offset, PyArena *arena);
अभिव्यक्ती_ty _PyPegen_constant_from_token(Parser* p, Token* tok);
अभिव्यक्ती_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok);
अभिव्यक्ती_ty _PyPegen_constant_from_string(Parser* p, Token* tok);
अभिव्यक्ती_ty _PyPegen_concatenate_tstrings(Parser *p, asdl_अभिव्यक्ती_seq *, पूर्णांक, पूर्णांक, पूर्णांक, पूर्णांक, PyArena *);
अभिव्यक्ती_ty _PyPegen_concatenate_strings(Parser *p, asdl_अभिव्यक्ती_seq *, पूर्णांक, पूर्णांक, पूर्णांक, पूर्णांक, PyArena *);
अभिव्यक्ती_ty _PyPegen_FetchRawForm(Parser *p, पूर्णांक, पूर्णांक, पूर्णांक, पूर्णांक);
अभिव्यक्ती_ty _PyPegen_ensure_imaginary(Parser *p, अभिव्यक्ती_ty);
अभिव्यक्ती_ty _PyPegen_ensure_real(Parser *p, अभिव्यक्ती_ty);
asdl_seq *_PyPegen_join_sequences(Parser *, asdl_seq *, asdl_seq *);
पूर्णांक _PyPegen_check_barry_as_flufl(Parser *, Token *);
पूर्णांक _PyPegen_check_legacy_stmt(Parser *p, अभिव्यक्ती_ty t);
शून्य *_PyPegen_raise_error_for_missing_comma(Parser *p, अभिव्यक्ती_ty a, अभिव्यक्ती_ty b);
ResultTokenWithMetadata *_PyPegen_check_fstring_conversion(Parser *p, Token *, अभिव्यक्ती_ty t);
ResultTokenWithMetadata *_PyPegen_setup_full_format_spec(Parser *, Token *, asdl_अभिव्यक्ती_seq *, पूर्णांक, पूर्णांक,
                                                         पूर्णांक, पूर्णांक, PyArena *);
mod_ty _PyPegen_make_module(Parser *, asdl_stmt_seq *);
शून्य *_PyPegen_arguments_parsing_error(Parser *, अभिव्यक्ती_ty);
अभिव्यक्ती_ty _PyPegen_get_last_comprehension_item(comprehension_ty comprehension);
शून्य *_PyPegen_nonparen_genexp_in_call(Parser *p, अभिव्यक्ती_ty args, asdl_comprehension_seq *comprehensions);
stmt_ty _PyPegen_checked_from_import(Parser *p, asdl_seq *dots, अभिव्यक्ती_ty module_name,
                                     asdl_alias_seq *names, अभिव्यक्ती_ty lazy_token, पूर्णांक lineno,
                                     पूर्णांक col_offset, पूर्णांक end_lineno, पूर्णांक end_col_offset,
                                     PyArena *arena);
asdl_stmt_seq* _PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts);
stmt_ty _PyPegen_register_stmt(Parser *p, stmt_ty s);

// Parser API

Parser *_PyPegen_Parser_New(संरचना tok_state *, पूर्णांक, पूर्णांक, पूर्णांक, पूर्णांक *, स्थिरांक अक्षर*, PyArena *);
शून्य _PyPegen_Parser_Free(Parser *);
mod_ty _PyPegen_run_parser_from_file_pointer(खाता *, पूर्णांक, PyObject *, स्थिरांक अक्षर *,
                                    स्थिरांक अक्षर *, स्थिरांक अक्षर *, PyCompilerFlags *, पूर्णांक *, PyObject **,
                                    PyArena *);
शून्य *_PyPegen_run_parser(Parser *);
mod_ty _PyPegen_run_parser_from_string(स्थिरांक अक्षर *, पूर्णांक, PyObject *, PyCompilerFlags *, PyArena *, PyObject *, PyObject **);
asdl_stmt_seq *_PyPegen_interactive_exit(Parser *);

// Generated function in parse.c - function definition in python.gram
शून्य *_PyPegen_parse(Parser *);

#endif
