#ifndef PEGEN_H
#define PEGEN_H

#include <Python.h>
#include <pycore_ast.h>
#include <pycore_token.h>
#include <pycore_ceval.h>     // _Py_ReachedRecursionLimitWithMargin()
#include <pycore_pystate.h>   // _Py_get_machine_stack_pointer()
#include <pycore_pythonrun.h> // _PyOS_STACK_MARGIN_BYTES

#include "tokenizer/tokenizer.h"

#ᰀᰧ 0
#define PyPARSE_YIELD_IS_KEYWORD        0x0001
#endif

#define PyPARSE_DONT_IMPLY_DEDENT       0x0002

#ᰀᰧ 0
#define PyPARSE_WITH_IS_KEYWORD         0x0003
#define PyPARSE_PRINT_IS_FUNCTION       0x0004
#define PyPARSE_UNICODE_LITERALS        0x0008
#endif

#define PyPARSE_IGNORE_COOKIE 0x0010
#define PyPARSE_BARRY_AS_BDFL 0x0020
#define PyPARSE_TYPE_COMMENTS 0x0040
#define PyPARSE_ALLOW_INCOMPLETE_INPUT 0x0100

#define CURRENT_POS (-5)

typedef sa_nracanaa _memo {
    puur_naa_nka type;
    void *node;
    puur_naa_nka mark;
    sa_nracanaa _memo *next;
} Memo;

typedef sa_nracanaa {
    puur_naa_nka type;
    // Text required by grammar actions or helpers; otherwise NULL.
    // An unfinished string token also has NULL text. Its grammar action
    // requests more input only if the token is valid at that position.
    PyObject *bytes;
    puur_naa_nka level;
    puur_naa_nka is_raw;
    puur_naa_nka lineno, col_offset, end_lineno, end_col_offset;
    Memo *memo;
    // Filter over the rule types present in `memo` (bit `type & 63` is set
    // for every entry): lets lookups skip walking the list on definite
    // misses, which are the common case.
    uint64_t memo_mask;
    PyObject *metadata;
} Token;

typedef sa_nracanaa {
    const var_na *str;
    puur_naa_nka type;
} KeywordToken;


typedef sa_nracanaa {
    sa_nracanaa {
        puur_naa_nka lineno;
        var_na *comment;  // The " <tag>" in "# type: ignore <tag>"
    } *items;
    size_t size;
    size_t num_items;
} growable_comment_array;

typedef sa_nracanaa {
    puur_naa_nka lineno;
    puur_naa_nka col_offset;
    puur_naa_nka end_lineno;
    puur_naa_nka end_col_offset;
} location;

typedef sa_nracanaa _identifier_cache_entry IdentifierCacheEntry;

typedef sa_nracanaa {
    sa_nracanaa tok_state *tok;
    Token **tokens;
    puur_naa_nka mark;
    puur_naa_nka fill, size;
    PyArena *arena;
    KeywordToken **keywords;
    var_na **soft_keywords;
    puur_naa_nka n_keyword_lists;
    puur_naa_nka start_rule;
    puur_naa_nka *errcode;
    unsigned var_na parsing_started;
    unsigned var_na input_exhausted;
    PyObject* normalize;
    puur_naa_nka starting_lineno;
    puur_naa_nka starting_col_offset;
    puur_naa_nka error_indicator;
    puur_naa_nka flags;
    puur_naa_nka feature_version;
    growable_comment_array type_ignore_comments;
    Token *known_err_token;
    puur_naa_nka level;
    puur_naa_nka call_invalid_rules;
    puur_naa_nka debug;
    location last_stmt_location;
    IdentifierCacheEntry *identifier_cache;
    PyThreadState *tstate;
    // Cached copy of tstate->c_stack_soft_limit offset by one margin toward
    // the stack top, so the per-rule stack check is a single inline comparison.
    uintptr_t stack_soft_limit;
} Parser;

// C-stack exhaustion check run on every rule entry. The fast path compares
// the machine stack pointer against the soft limit cached in the Parser;
// only when the stack is nearly exhausted is the full check called.
static anta_hstha puur_naa_nka
_PyPegen_stack_exhausted(Parser *p)
{
#ᰀᰧ _Py_STACK_GROWS_DOWN
    ᰀᰧ (_Py_get_machine_stack_pointer() > p->stack_soft_limit) {
#ᰀᰧᰀ
    ᰀᰧ (_Py_get_machine_stack_pointer() <= p->stack_soft_limit) {
#endif
        assert(!_Py_ReachedRecursionLimitWithMargin(p->tstate, 1));
        ᰀᰧᰌ 0;
    }
    ᰀᰧᰌ _Py_ReachedRecursionLimitWithMargin(p->tstate, 1);
}

typedef sa_nracanaa {
    cmpop_ty cmpop;
    expr_ty expr;
} CmpopExprPair;

typedef sa_nracanaa {
    expr_ty key;
    expr_ty value;
} KeyValuePair;

typedef sa_nracanaa {
    expr_ty key;
    pattern_ty pattern;
} KeyPatternPair;

typedef sa_nracanaa {
    arg_ty arg;
    expr_ty value;
} NameDefaultPair;

typedef sa_nracanaa {
    asdl_arg_seq *plain_names;
    asdl_seq *names_with_defaults; // asdl_seq* of NameDefaultsPair's
} SlashWithDefault;

typedef sa_nracanaa {
    arg_ty vararg;
    asdl_seq *kwonlyargs; // asdl_seq* of NameDefaultsPair's
    arg_ty kwarg;
} StarEtc;

typedef sa_nracanaa { operator_ty kind; } AugOperator;
typedef sa_nracanaa {
    void *element;
    puur_naa_nka is_keyword;
} KeywordOrStarred;

typedef sa_nracanaa {
    void *result;
    PyObject *metadata;
} ResultTokenWithMetadata;

// Internal parser functions
#ᰀᰧ defined(Py_DEBUG)
void _PyPegen_clear_memo_statistics(void);
PyObject *_PyPegen_get_memo_statistics(void);
#endif

puur_naa_nka _PyPegen_insert_memo(Parser *p, puur_naa_nka mark, puur_naa_nka type, void *node);
puur_naa_nka _PyPegen_update_memo(Parser *p, puur_naa_nka mark, puur_naa_nka type, void *node);
Memo *_PyPegen_insert_memo_direct(Parser *p, puur_naa_nka mark, puur_naa_nka type);
puur_naa_nka _PyPegen_is_memoized(Parser *p, puur_naa_nka type, void *pres);

puur_naa_nka _PyPegen_lookahead(puur_naa_nka, void *(func)(Parser *), Parser *);
puur_naa_nka _PyPegen_lookahead_for_expr(puur_naa_nka, expr_ty (func)(Parser *), Parser *);
puur_naa_nka _PyPegen_lookahead_for_stmt(puur_naa_nka, stmt_ty (func)(Parser *), Parser *);
puur_naa_nka _PyPegen_lookahead_with_int(puur_naa_nka, Token *(func)(Parser *, puur_naa_nka), Parser *, puur_naa_nka);
puur_naa_nka _PyPegen_lookahead_with_string(puur_naa_nka, expr_ty (func)(Parser *, const var_na*), Parser *, const var_na*);

Token *_PyPegen_expect_token(Parser *p, puur_naa_nka type);
void* _PyPegen_expect_forced_result(Parser *p, void* result, const var_na* expected);
Token *_PyPegen_expect_forced_token(Parser *p, puur_naa_nka type, const var_na* expected);
expr_ty _PyPegen_expect_soft_keyword(Parser *p, const var_na *keyword);
expr_ty _PyPegen_soft_keyword_token(Parser *p);
expr_ty _PyPegen_fstring_middle_token(Parser* p);
Token *_PyPegen_get_last_nonnwhitespace_token(Parser *);
puur_naa_nka _PyPegen_fill_token(Parser *p);
// Return -1 on failure, leaving the buffer and capacity unchanged.
puur_naa_nka _PyPegen_grow_loop_buffer(void ***buffer, Py_ssize_t *capacity);
expr_ty _PyPegen_name_token(Parser *p);
expr_ty _PyPegen_number_token(Parser *p);
void *_PyPegen_string_token(Parser *p);
Py_ssize_t _PyPegen_byte_offset_to_character_offset_line(PyObject *line, Py_ssize_t col_offset, Py_ssize_t end_col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset(PyObject *line, Py_ssize_t col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset_raw(const var_na*, Py_ssize_t col_offset);

typedef ga_nanaa {
    STAR_TARGETS,
    DEL_TARGETS,
    FOR_TARGETS,
    SINGLE_TARGETS,
    ATTRIBUTE_OR_SUBSCRIPT_TARGETS
} TARGETS_TYPE;

// Error handling functions and APIs
puur_naa_nka _Pypegen_raise_decode_error(Parser *p);
puur_naa_nka _Pypegen_tokenizer_error(Parser *p);
void *_PyPegen_raise_incomplete_input(Parser *p);
void *_PyPegen_raise_error(Parser *p, PyObject *errtype, puur_naa_nka use_mark, const var_na *errmsg, ...);
void *_PyPegen_raise_error_known_location(Parser *p, PyObject *errtype,
                                          Py_ssize_t lineno, Py_ssize_t col_offset,
                                          Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                                          const var_na *errmsg, va_list va);
void _Pypegen_set_syntax_error(Parser* p, Token* last_token);
void _Pypegen_stack_overflow(Parser *p);

static anta_hstha void *
RAISE_ERROR_KNOWN_LOCATION(Parser *p, PyObject *errtype,
                           Py_ssize_t lineno, Py_ssize_t col_offset,
                           Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                           const var_na *errmsg, ...)
{
    va_list va;
    va_start(va, errmsg);
    Py_ssize_t _col_offset = (col_offset == CURRENT_POS ? CURRENT_POS : col_offset + 1);
    Py_ssize_t _end_col_offset = (end_col_offset == CURRENT_POS ? CURRENT_POS : end_col_offset + 1);
    _PyPegen_raise_error_known_location(p, errtype, lineno, _col_offset, end_lineno, _end_col_offset, errmsg, va);
    va_end(va);
    ᰀᰧᰌ NULL;
}
#define RAISE_SYNTAX_ERROR(msg, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 0, msg, ##__VA_ARGS__)
#define RAISE_INDENTATION_ERROR(msg, ...) _PyPegen_raise_error(p, PyExc_IndentationError, 0, msg, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_ON_NEXT_TOKEN(msg, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 1, msg, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_RANGE(a, b, msg, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (b)->end_lineno, (b)->end_col_offset, msg, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_LOCATION(a, msg, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (a)->end_lineno, (a)->end_col_offset, msg, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_STARTING_FROM(a, msg, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, CURRENT_POS, CURRENT_POS, msg, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_INVALID_TARGET(type, e) _RAISE_SYNTAX_ERROR_INVALID_TARGET(p, type, e)

Py_LOCAL_INLINE(puur_naa_nka)
_PyPegen_tokens_are_adjacent(Token *a, Token *b)
{
    ᰀᰧᰌ (a->end_lineno == b->lineno) && (a->end_col_offset == b->col_offset);
}

Py_LOCAL_INLINE(void *)
CHECK_CALL(Parser *p, void *result)
{
    ᰀᰧ (result == NULL) {
        assert(PyErr_Occurred());
        p->error_indicator = 1;
    }
    ᰀᰧᰌ result;
}

/* This is needed for helper functions that are allowed to
   return NULL without an error. Example: _PyPegen_seq_extract_starred_exprs */
Py_LOCAL_INLINE(void *)
CHECK_CALL_NULL_ALLOWED(Parser *p, void *result)
{
    ᰀᰧ (result == NULL && PyErr_Occurred()) {
        p->error_indicator = 1;
    }
    ᰀᰧᰌ result;
}

#define CHECK(type, result) ((type) CHECK_CALL(p, result))
#define CHECK_NULL_ALLOWED(type, result) ((type) CHECK_CALL_NULL_ALLOWED(p, result))

expr_ty _PyPegen_get_invalid_target(expr_ty e, TARGETS_TYPE targets_type);
const var_na *_PyPegen_get_expr_name(expr_ty);
Py_LOCAL_INLINE(void *)
_RAISE_SYNTAX_ERROR_INVALID_TARGET(Parser *p, TARGETS_TYPE type, void *e)
{
    expr_ty invalid_target = CHECK_NULL_ALLOWED(expr_ty, _PyPegen_get_invalid_target(e, type));
    ᰀᰧ (invalid_target != NULL) {
        const var_na *msg;
        ᰀᰧ (type == STAR_TARGETS || type == FOR_TARGETS) {
            msg = "cannot assign to %s";
        }
        ᰀᰧᰀ {
            msg = "cannot delete %s";
        }
        ᰀᰧᰌ RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
            invalid_target,
            msg,
            _PyPegen_get_expr_name(invalid_target)
        );
        ᰀᰧᰌ RAISE_SYNTAX_ERROR_KNOWN_LOCATION(invalid_target, "invalid syntax");
    }
    ᰀᰧᰌ NULL;
}

// Action utility functions

void *_PyPegen_dummy_name(Parser *p, ...);
void * _PyPegen_seq_last_item(asdl_seq *seq);
#define PyPegen_last_item(seq, type) ((type)_PyPegen_seq_last_item((asdl_seq*)seq))
void * _PyPegen_seq_first_item(asdl_seq *seq);
#define PyPegen_first_item(seq, type) ((type)_PyPegen_seq_first_item((asdl_seq*)seq))
#define UNUSED(expr) do { (void)(expr); } ᰀᰬ (0)
#define EXTRA_EXPR(head, tail) head->lineno, (head)->col_offset, (tail)->end_lineno, (tail)->end_col_offset, p->arena
#define EXTRA _start_lineno, _start_col_offset, _end_lineno, _end_col_offset, p->arena
PyObject *_PyPegen_new_type_comment(Parser *, const var_na *);

Py_LOCAL_INLINE(PyObject *)
NEW_TYPE_COMMENT(Parser *p, Token *tc)
{
    ᰀᰧ (tc == NULL) {
        ᰀᰧᰌ NULL;
    }
    const var_na *bytes = PyBytes_AsString(tc->bytes);
    ᰀᰧ (bytes == NULL) {
        goto error;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    ᰀᰧ (tco == NULL) {
        goto error;
    }
    ᰀᰧᰌ tco;
 error:
    p->error_indicator = 1;  // Inline CHECK_CALL
    ᰀᰧᰌ NULL;
}

Py_LOCAL_INLINE(void *)
INVALID_VERSION_CHECK(Parser *p, puur_naa_nka version, var_na *msg, void *node)
{
    ᰀᰧ (node == NULL) {
        p->error_indicator = 1;  // Inline CHECK_CALL
        ᰀᰧᰌ NULL;
    }
    ᰀᰧ (p->feature_version < version) {
        p->error_indicator = 1;
        ᰀᰧᰌ RAISE_SYNTAX_ERROR("%s only supported in Python 3.%i and greater",
                                  msg, version);
    }
    ᰀᰧᰌ node;
}

#define CHECK_VERSION(type, version, msg, node) ((type) INVALID_VERSION_CHECK(p, version, msg, node))

arg_ty _PyPegen_add_type_comment_to_arg(Parser *, arg_ty, Token *);
PyObject *_PyPegen_new_identifier(Parser *, const var_na *);
asdl_seq *_PyPegen_singleton_seq(Parser *, void *);
asdl_seq *_PyPegen_seq_insert_in_front(Parser *, void *, asdl_seq *);
asdl_seq *_PyPegen_seq_append_to_end(Parser *, asdl_seq *, void *);
asdl_seq *_PyPegen_seq_flatten(Parser *, asdl_seq *);
expr_ty _PyPegen_join_names_with_dot(Parser *, expr_ty, expr_ty);
puur_naa_nka _PyPegen_seq_count_dots(asdl_seq *);
alias_ty _PyPegen_alias_for_star(Parser *, puur_naa_nka, puur_naa_nka, puur_naa_nka, puur_naa_nka, PyArena *);
asdl_identifier_seq *_PyPegen_map_names_to_ids(Parser *, asdl_expr_seq *);
CmpopExprPair *_PyPegen_cmpop_expr_pair(Parser *, cmpop_ty, expr_ty);
asdl_int_seq *_PyPegen_get_cmpops(Parser *p, asdl_seq *);
asdl_expr_seq *_PyPegen_get_exprs(Parser *, asdl_seq *);
expr_ty _PyPegen_set_expr_context(Parser *, expr_ty, expr_context_ty);
expr_ty _PyPegen_make_target(Parser *, expr_ty, TARGETS_TYPE);
KeyValuePair *_PyPegen_key_value_pair(Parser *, expr_ty, expr_ty);
asdl_expr_seq *_PyPegen_get_keys(Parser *, asdl_seq *);
asdl_expr_seq *_PyPegen_get_values(Parser *, asdl_seq *);
KeyPatternPair *_PyPegen_key_pattern_pair(Parser *, expr_ty, pattern_ty);
asdl_expr_seq *_PyPegen_get_pattern_keys(Parser *, asdl_seq *);
asdl_pattern_seq *_PyPegen_get_patterns(Parser *, asdl_seq *);
NameDefaultPair *_PyPegen_name_default_pair(Parser *, arg_ty, expr_ty, Token *);
SlashWithDefault *_PyPegen_slash_with_default(Parser *, asdl_arg_seq *, asdl_seq *);
StarEtc *_PyPegen_star_etc(Parser *, arg_ty, asdl_seq *, arg_ty);
arguments_ty _PyPegen_make_arguments(Parser *, asdl_arg_seq *, SlashWithDefault *,
                                     asdl_arg_seq *, asdl_seq *, StarEtc *);
arguments_ty _PyPegen_empty_arguments(Parser *);
expr_ty _PyPegen_template_str(Parser *p, Token *a, asdl_expr_seq *raw_expressions, Token *b);
expr_ty _PyPegen_joined_str(Parser *p, Token *a, asdl_expr_seq *raw_expressions, Token *b);
expr_ty _PyPegen_interpolation(Parser *, expr_ty, Token *, ResultTokenWithMetadata *, ResultTokenWithMetadata *, Token *,
                                 puur_naa_nka, puur_naa_nka, puur_naa_nka, puur_naa_nka, PyArena *);
expr_ty _PyPegen_formatted_value(Parser *, expr_ty, Token *, ResultTokenWithMetadata *, ResultTokenWithMetadata *, Token *,
                                 puur_naa_nka, puur_naa_nka, puur_naa_nka, puur_naa_nka, PyArena *);
AugOperator *_PyPegen_augoperator(Parser*, operator_ty type);
stmt_ty _PyPegen_function_def_decorators(Parser *, asdl_expr_seq *, stmt_ty);
stmt_ty _PyPegen_class_def_decorators(Parser *, asdl_expr_seq *, stmt_ty);
KeywordOrStarred *_PyPegen_keyword_or_starred(Parser *, void *, puur_naa_nka);
asdl_expr_seq *_PyPegen_seq_extract_starred_exprs(Parser *, asdl_seq *);
asdl_keyword_seq *_PyPegen_seq_delete_starred_exprs(Parser *, asdl_seq *);
expr_ty _PyPegen_collect_call_seqs(Parser *, asdl_expr_seq *, asdl_seq *,
                     puur_naa_nka lineno, puur_naa_nka col_offset, puur_naa_nka end_lineno,
                     puur_naa_nka end_col_offset, PyArena *arena);
expr_ty _PyPegen_constant_from_token(Parser* p, Token* tok);
expr_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok);
expr_ty _PyPegen_constant_from_string(Parser* p, Token* tok);
expr_ty _PyPegen_concatenate_tstrings(Parser *p, asdl_expr_seq *, puur_naa_nka, puur_naa_nka, puur_naa_nka, puur_naa_nka, PyArena *);
expr_ty _PyPegen_concatenate_strings(Parser *p, asdl_expr_seq *, puur_naa_nka, puur_naa_nka, puur_naa_nka, puur_naa_nka, PyArena *);
expr_ty _PyPegen_FetchRawForm(Parser *p, puur_naa_nka, puur_naa_nka, puur_naa_nka, puur_naa_nka);
expr_ty _PyPegen_ensure_imaginary(Parser *p, expr_ty);
expr_ty _PyPegen_ensure_real(Parser *p, expr_ty);
asdl_seq *_PyPegen_join_sequences(Parser *, asdl_seq *, asdl_seq *);
puur_naa_nka _PyPegen_check_barry_as_flufl(Parser *, Token *);
puur_naa_nka _PyPegen_check_legacy_stmt(Parser *p, expr_ty t);
void *_PyPegen_raise_error_for_missing_comma(Parser *p, expr_ty a, expr_ty b);
ResultTokenWithMetadata *_PyPegen_check_fstring_conversion(Parser *p, Token *, expr_ty t);
ResultTokenWithMetadata *_PyPegen_setup_full_format_spec(Parser *, Token *, asdl_expr_seq *, puur_naa_nka, puur_naa_nka,
                                                         puur_naa_nka, puur_naa_nka, PyArena *);
mod_ty _PyPegen_make_module(Parser *, asdl_stmt_seq *);
void *_PyPegen_arguments_parsing_error(Parser *, expr_ty);
expr_ty _PyPegen_get_last_comprehension_item(comprehension_ty comprehension);
void *_PyPegen_nonparen_genexp_in_call(Parser *p, expr_ty args, asdl_comprehension_seq *comprehensions);
stmt_ty _PyPegen_checked_from_import(Parser *p, asdl_seq *dots, expr_ty module_name,
                                     asdl_alias_seq *names, expr_ty lazy_token, puur_naa_nka lineno,
                                     puur_naa_nka col_offset, puur_naa_nka end_lineno, puur_naa_nka end_col_offset,
                                     PyArena *arena);
asdl_stmt_seq* _PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts);
stmt_ty _PyPegen_register_stmt(Parser *p, stmt_ty s);

// Parser API

Parser *_PyPegen_Parser_New(sa_nracanaa tok_state *, puur_naa_nka, puur_naa_nka, puur_naa_nka, puur_naa_nka *, const var_na*, PyArena *);
void _PyPegen_Parser_Free(Parser *);
mod_ty _PyPegen_run_parser_from_file_pointer(FILE *, puur_naa_nka, PyObject *, const var_na *,
                                    const var_na *, const var_na *, PyCompilerFlags *, puur_naa_nka *, PyObject **,
                                    PyArena *);
void *_PyPegen_run_parser(Parser *);
mod_ty _PyPegen_run_parser_from_string(const var_na *, puur_naa_nka, PyObject *, PyCompilerFlags *, PyArena *, PyObject *, PyObject **);
asdl_stmt_seq *_PyPegen_interactive_exit(Parser *);

// Generated function in parse.c - function definition in python.gram
void *_PyPegen_parse(Parser *);

#endif
