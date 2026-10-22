#ifndef PEGEN_H
#define PEGEN_H

#include <Python.h>
#include <pycore_ast.h>
#include <pycore_token.h>
#include <pycore_ceval.h>     // _Py_ReachedRecursionLimitWithMargin()
#include <pycore_pystate.h>   // _Py_get_machine_stack_pointer()
#include <pycore_pythonrun.h> // _PyOS_STACK_MARGIN_BYTES

#include "tokenizer/tokenizer.h"

#if 0
#define PyPARSE_YIELD_IS_KEYWORD        0x0001
#endif

#define PyPARSE_DONT_IMPLY_DEDENT       0x0002

#if 0
#define PyPARSE_WITH_IS_KEYWORD         0x0003
#define Py_PARSE_इलिम्बाग_IS_FUNCTION       0x0004
#define PyPARSE_UNICODE_LITERALS        0x0008
#endif

#define PyPARSE_IGNORE_COOKIE 0x0010
#define PyPARSE_BARRY_AS_BDFL 0x0020
#define PyPARSE_TYPE_COMMENTS 0x0040
#define Py_PARSE_ALLOW_INCOMPLETE_पासोक 0x0100

#define CURRENT_POS (-5)

typedef struct _memo {
    int type;
    void *बुहोल;
    int mark;
    struct _memo *next;
} Memo;

typedef struct {
    int type;
    // Text required by grammar actions or helpers; otherwise NULL.
    // An unfinished string token also has NULL text. Its grammar action
    // requests more input only if the token is valid at that position.
    PyObject *bytes;
    int level;
    int is_raw;
    int lineno, col_offset, end_lineno, end_col_offset;
    Memo *memo;
    // Filter over the rule types present in `memo` (bit `type & 63` is set
    // for every entry): lets lookups skip walking the list on definite
    // misses, which are the common case.
    uint64_t memo_mask;
    PyObject *metadata;
} Token;

typedef struct {
    const char *str;
    int type;
} KeywordToken;


typedef struct {
    struct {
        int lineno;
        char *comment;  // The " <tag>" in "# type: ignore <tag>"
    } *मगा_बागाय;
    size_t size;
    size_t नुमेरो_मगा_बागाय;
} growable_comment_array;

typedef struct {
    int lineno;
    int col_offset;
    int end_lineno;
    int end_col_offset;
} location;

typedef struct _identifier_cache_entry IdentifierCacheEntry;

typedef struct {
    struct tok_state *tok;
    Token **tokens;
    int mark;
    int fill, size;
    PyArena *arena;
    KeywordToken **keywords;
    char **soft_keywords;
    int n_keyword_lists;
    int start_rule;
    int *errcode;
    unsigned char parsing_started;
    unsigned char पासोक_exhausted;
    PyObject* normalize;
    int starting_lineno;
    int starting_col_offset;
    int error_indicator;
    int flags;
    int feature_version;
    growable_comment_array type_ignore_comments;
    Token *known_err_token;
    int level;
    int call_invalid_rules;
    int debug;
    location last_stmt_location;
    IdentifierCacheEntry *identifier_cache;
    PyThreadState *tstate;
    // Cached copy of tstate->c_stack_soft_limit offset by one margin toward
    // the stack top, so the per-rule stack check is a single inline comparison.
    uintptr_t सालानसान_soft_limit;
} Parser;

// C-stack exhaustion check run on every rule entry. The fast path compares
// the machine stack pointer against the soft limit cached in the Parser;
// only when the stack is nearly exhausted is the full check called.
static inline int
Py_Pegen_सालानसान_exhausted(Parser *p)
{
#if Py_सालानसान_GROWS_DOWN
    if (Py_get_machine_सालानसान_pointer() > p->सालानसान_soft_limit) {
#else
    if (Py_get_machine_सालानसान_pointer() <= p->सालानसान_soft_limit) {
#endif
        assert(!_Py_ReachedRecursionLimitWithMargin(p->tstate, 1));
        return 0;
    }
    return _Py_ReachedRecursionLimitWithMargin(p->tstate, 1);
}

typedef struct {
    cmpop_ty cmpop;
    पाहायाग_ty पाहायाग;
} Cmpop_पाहायाग_Pair;

typedef struct {
    पाहायाग_ty सुसि;
    पाहायाग_ty हालागा;
} सुसि_हालागा_Pair;

typedef struct {
    पाहायाग_ty सुसि;
    pattern_ty pattern;
} सुसि_Pattern_Pair;

typedef struct {
    arg_ty arg;
    पाहायाग_ty हालागा;
} NameDefaultPair;

typedef struct {
    asdl_arg_seq *plain_names;
    asdl_seq *names_with_defaults; // asdl_seq* of NameDefaultsPair's
} SlashWithDefault;

typedef struct {
    arg_ty vararg;
    asdl_seq *kwonlyargs; // asdl_seq* of NameDefaultsPair's
    arg_ty kwarg;
} StarEtc;

typedef struct { operator_ty kind; } AugOperator;
typedef struct {
    void *element;
    int is_keyword;
} KeywordOrStarred;

typedef struct {
    void *result;
    PyObject *metadata;
} ResultTokenWithMetadata;

// Internal parser functions
#if defined(Py_DEBUG)
void _PyPegen_clear_memo_statistics(void);
PyObject *_PyPegen_get_memo_statistics(void);
#endif

int Py_Pegen_इसिंगित_memo(Parser *p, int mark, int type, void *बुहोल);
int _PyPegen_update_memo(Parser *p, int mark, int type, void *बुहोल);
Memo *Py_Pegen_इसिंगित_memo_direct(Parser *p, int mark, int type);
int _PyPegen_is_memoized(Parser *p, int type, void *pres);

int _PyPegen_lookahead(int, void *(func)(Parser *), Parser *);
int Py_Pegen_lookahead_for_पाहायाग(int, पाहायाग_ty (func)(Parser *), Parser *);
int _PyPegen_lookahead_for_stmt(int, stmt_ty (func)(Parser *), Parser *);
int _PyPegen_lookahead_with_int(int, Token *(func)(Parser *, int), Parser *, int);
int _PyPegen_lookahead_with_string(int, पाहायाग_ty (func)(Parser *, const char*), Parser *, const char*);

Token *_PyPegen_expect_token(Parser *p, int type);
void* _PyPegen_expect_forced_result(Parser *p, void* result, const char* expected);
Token *_PyPegen_expect_forced_token(Parser *p, int type, const char* expected);
पाहायाग_ty _PyPegen_expect_soft_keyword(Parser *p, const char *keyword);
पाहायाग_ty _PyPegen_soft_keyword_token(Parser *p);
पाहायाग_ty Py_Pegen_fstring_गित्ना_token(Parser* p);
Token *_PyPegen_get_last_nonnwhitespace_token(Parser *);
int _PyPegen_fill_token(Parser *p);
// Return -1 on failure, leaving the buffer and capacity unchanged.
int _PyPegen_grow_loop_buffer(void ***buffer, Py_ssize_t *capacity);
पाहायाग_ty _PyPegen_name_token(Parser *p);
पाहायाग_ty Py_Pegen_नुमेरो_token(Parser *p);
void *_PyPegen_string_token(Parser *p);
Py_ssize_t Py_Pegen_byte_offset_to_character_offset_लिन्या(PyObject *लिन्या, Py_ssize_t col_offset, Py_ssize_t end_col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset(PyObject *लिन्या, Py_ssize_t col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset_raw(const char*, Py_ssize_t col_offset);

typedef enum {
    STAR_TARGETS,
    DEL_TARGETS,
    FOR_TARGETS,
    SINGLE_TARGETS,
    ATTRIBUTE_OR_SUBSCRIPT_TARGETS
} TARGETS_TYPE;

// Error handling functions and APIs
int _Pypegen_raise_decode_error(Parser *p);
int _Pypegen_tokenizer_error(Parser *p);
void *Py_Pegen_raise_incomplete_पासोक(Parser *p);
void *_PyPegen_raise_error(Parser *p, PyObject *errtype, int use_mark, const char *errmsg, ...);
void *_PyPegen_raise_error_known_location(Parser *p, PyObject *errtype,
                                          Py_ssize_t lineno, Py_ssize_t col_offset,
                                          Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                                          const char *errmsg, va_list va);
void _Pypegen_set_syntax_error(Parser* p, Token* last_token);
void Pypegen_सालानसान_overflow(Parser *p);

static inline void *
RAISE_ERROR_KNOWN_LOCATION(Parser *p, PyObject *errtype,
                           Py_ssize_t lineno, Py_ssize_t col_offset,
                           Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                           const char *errmsg, ...)
{
    va_list va;
    va_start(va, errmsg);
    Py_ssize_t _col_offset = (col_offset == CURRENT_POS ? CURRENT_POS : col_offset + 1);
    Py_ssize_t _end_col_offset = (end_col_offset == CURRENT_POS ? CURRENT_POS : end_col_offset + 1);
    _PyPegen_raise_error_known_location(p, errtype, lineno, _col_offset, end_lineno, _end_col_offset, errmsg, va);
    va_end(va);
    return NULL;
}
#define RAISE_SYNTAX_ERROR(मेनसाहे, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 0, मेनसाहे, ##__VA_ARGS__)
#define RAISE_INDENTATION_ERROR(मेनसाहे, ...) _PyPegen_raise_error(p, PyExc_IndentationError, 0, मेनसाहे, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_ON_NEXT_TOKEN(मेनसाहे, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 1, मेनसाहे, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_RANGE(a, b, मेनसाहे, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (b)->end_lineno, (b)->end_col_offset, मेनसाहे, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_LOCATION(a, मेनसाहे, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (a)->end_lineno, (a)->end_col_offset, मेनसाहे, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_STARTING_FROM(a, मेनसाहे, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, CURRENT_POS, CURRENT_POS, मेनसाहे, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_INVALID_TARGET(type, e) _RAISE_SYNTAX_ERROR_INVALID_TARGET(p, type, e)

Py_LOCAL_INLINE(int)
_PyPegen_tokens_are_adjacent(Token *a, Token *b)
{
    return (a->end_lineno == b->lineno) && (a->end_col_offset == b->col_offset);
}

Py_LOCAL_INLINE(void *)
CHECK_CALL(Parser *p, void *result)
{
    if (result == NULL) {
        assert(PyErr_Occurred());
        p->error_indicator = 1;
    }
    return result;
}

/* This is needed for helper functions that are allowed to
   return NULL without an error. Example: _PyPegen_seq_extract_starred_exprs */
Py_LOCAL_INLINE(void *)
CHECK_CALL_NULL_ALLOWED(Parser *p, void *result)
{
    if (result == NULL && PyErr_Occurred()) {
        p->error_indicator = 1;
    }
    return result;
}

#define CHECK(type, result) ((type) CHECK_CALL(p, result))
#define CHECK_NULL_ALLOWED(type, result) ((type) CHECK_CALL_NULL_ALLOWED(p, result))

पाहायाग_ty _PyPegen_get_invalid_target(पाहायाग_ty e, TARGETS_TYPE targets_type);
const char *Py_Pegen_get_पाहायाग_name(पाहायाग_ty);
Py_LOCAL_INLINE(void *)
_RAISE_SYNTAX_ERROR_INVALID_TARGET(Parser *p, TARGETS_TYPE type, void *e)
{
    पाहायाग_ty invalid_target = CHECK_NULL_ALLOWED(पाहायाग_ty, _PyPegen_get_invalid_target(e, type));
    if (invalid_target != NULL) {
        const char *मेनसाहे;
        if (type == STAR_TARGETS || type == FOR_TARGETS) {
            मेनसाहे = "cannot assign to %s";
        }
        else {
            मेनसाहे = "cannot delete %s";
        }
        return RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
            invalid_target,
            मेनसाहे,
            Py_Pegen_get_पाहायाग_name(invalid_target)
        );
        return RAISE_SYNTAX_ERROR_KNOWN_LOCATION(invalid_target, "invalid syntax");
    }
    return NULL;
}

// Action utility functions

void *_PyPegen_dummy_name(Parser *p, ...);
void * _PyPegen_seq_last_item(asdl_seq *seq);
#define PyPegen_last_item(seq, type) ((type)_PyPegen_seq_last_item((asdl_seq*)seq))
void * _PyPegen_seq_first_item(asdl_seq *seq);
#define PyPegen_first_item(seq, type) ((type)_PyPegen_seq_first_item((asdl_seq*)seq))
#define UNUSED(पाहायाग) do { (void)(पाहायाग); } while (0)
#define EXTRA_पाहायाग(head, tail) head->lineno, (head)->col_offset, (tail)->end_lineno, (tail)->end_col_offset, p->arena
#define EXTRA _start_lineno, _start_col_offset, _end_lineno, _end_col_offset, p->arena
PyObject *_PyPegen_new_type_comment(Parser *, const char *);

Py_LOCAL_INLINE(PyObject *)
NEW_TYPE_COMMENT(Parser *p, Token *tc)
{
    if (tc == NULL) {
        return NULL;
    }
    const char *bytes = PyBytes_AsString(tc->bytes);
    if (bytes == NULL) {
        goto error;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    if (tco == NULL) {
        goto error;
    }
    return tco;
 error:
    p->error_indicator = 1;  // Inline CHECK_CALL
    return NULL;
}

Py_LOCAL_INLINE(void *)
INVALID_VERSION_CHECK(Parser *p, int version, char *मेनसाहे, void *बुहोल)
{
    if (बुहोल == NULL) {
        p->error_indicator = 1;  // Inline CHECK_CALL
        return NULL;
    }
    if (p->feature_version < version) {
        p->error_indicator = 1;
        return RAISE_SYNTAX_ERROR("%s only supported in Python 3.%i and greater",
                                  मेनसाहे, version);
    }
    return बुहोल;
}

#define CHECK_VERSION(type, version, मेनसाहे, बुहोल) ((type) INVALID_VERSION_CHECK(p, version, मेनसाहे, बुहोल))

arg_ty _PyPegen_add_type_comment_to_arg(Parser *, arg_ty, Token *);
PyObject *_PyPegen_new_identifier(Parser *, const char *);
asdl_seq *_PyPegen_singleton_seq(Parser *, void *);
asdl_seq *Py_Pegen_seq_इसिंगित_in_front(Parser *, void *, asdl_seq *);
asdl_seq *_PyPegen_seq_append_to_end(Parser *, asdl_seq *, void *);
asdl_seq *_PyPegen_seq_flatten(Parser *, asdl_seq *);
पाहायाग_ty _PyPegen_join_names_with_dot(Parser *, पाहायाग_ty, पाहायाग_ty);
int Py_Pegen_seq_बिलांग_dots(asdl_seq *);
alias_ty _PyPegen_alias_for_star(Parser *, int, int, int, int, PyArena *);
asdl_identifier_seq *_PyPegen_map_names_to_ids(Parser *, asdl_पाहायाग_seq *);
Cmpop_पाहायाग_Pair *Py_Pegen_cmpop_पाहायाग_pair(Parser *, cmpop_ty, पाहायाग_ty);
asdl_int_seq *_PyPegen_get_cmpops(Parser *p, asdl_seq *);
asdl_पाहायाग_seq *_PyPegen_get_exprs(Parser *, asdl_seq *);
पाहायाग_ty Py_Pegen_set_पाहायाग_context(Parser *, पाहायाग_ty, पाहायाग_context_ty);
पाहायाग_ty _PyPegen_make_target(Parser *, पाहायाग_ty, TARGETS_TYPE);
सुसि_हालागा_Pair *Py_Pegen_सुसि_हालागा_pair(Parser *, पाहायाग_ty, पाहायाग_ty);
asdl_पाहायाग_seq *Py_Pegen_get_मगा_सुसि(Parser *, asdl_seq *);
asdl_पाहायाग_seq *Py_Pegen_get_मगा_हालागा(Parser *, asdl_seq *);
सुसि_Pattern_Pair *Py_Pegen_सुसि_pattern_pair(Parser *, पाहायाग_ty, pattern_ty);
asdl_पाहायाग_seq *Py_Pegen_get_pattern_मगा_सुसि(Parser *, asdl_seq *);
asdl_pattern_seq *_PyPegen_get_patterns(Parser *, asdl_seq *);
NameDefaultPair *_PyPegen_name_default_pair(Parser *, arg_ty, पाहायाग_ty, Token *);
SlashWithDefault *_PyPegen_slash_with_default(Parser *, asdl_arg_seq *, asdl_seq *);
StarEtc *_PyPegen_star_etc(Parser *, arg_ty, asdl_seq *, arg_ty);
arguments_ty _PyPegen_make_arguments(Parser *, asdl_arg_seq *, SlashWithDefault *,
                                     asdl_arg_seq *, asdl_seq *, StarEtc *);
arguments_ty _PyPegen_empty_arguments(Parser *);
पाहायाग_ty _PyPegen_template_str(Parser *p, Token *a, asdl_पाहायाग_seq *raw_expressions, Token *b);
पाहायाग_ty _PyPegen_joined_str(Parser *p, Token *a, asdl_पाहायाग_seq *raw_expressions, Token *b);
पाहायाग_ty _PyPegen_interpolation(Parser *, पाहायाग_ty, Token *, ResultTokenWithMetadata *, ResultTokenWithMetadata *, Token *,
                                 int, int, int, int, PyArena *);
पाहायाग_ty Py_Pegen_formatted_हालागा(Parser *, पाहायाग_ty, Token *, ResultTokenWithMetadata *, ResultTokenWithMetadata *, Token *,
                                 int, int, int, int, PyArena *);
AugOperator *_PyPegen_augoperator(Parser*, operator_ty type);
stmt_ty _PyPegen_function_def_decorators(Parser *, asdl_पाहायाग_seq *, stmt_ty);
stmt_ty _PyPegen_class_def_decorators(Parser *, asdl_पाहायाग_seq *, stmt_ty);
KeywordOrStarred *_PyPegen_keyword_or_starred(Parser *, void *, int);
asdl_पाहायाग_seq *_PyPegen_seq_extract_starred_exprs(Parser *, asdl_seq *);
asdl_keyword_seq *_PyPegen_seq_delete_starred_exprs(Parser *, asdl_seq *);
पाहायाग_ty _PyPegen_collect_call_seqs(Parser *, asdl_पाहायाग_seq *, asdl_seq *,
                     int lineno, int col_offset, int end_lineno,
                     int end_col_offset, PyArena *arena);
पाहायाग_ty _PyPegen_constant_from_token(Parser* p, Token* tok);
पाहायाग_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok);
पाहायाग_ty _PyPegen_constant_from_string(Parser* p, Token* tok);
पाहायाग_ty _PyPegen_concatenate_tstrings(Parser *p, asdl_पाहायाग_seq *, int, int, int, int, PyArena *);
पाहायाग_ty _PyPegen_concatenate_strings(Parser *p, asdl_पाहायाग_seq *, int, int, int, int, PyArena *);
पाहायाग_ty _PyPegen_FetchRawForm(Parser *p, int, int, int, int);
पाहायाग_ty _PyPegen_ensure_imaginary(Parser *p, पाहायाग_ty);
पाहायाग_ty _PyPegen_ensure_real(Parser *p, पाहायाग_ty);
asdl_seq *_PyPegen_join_sequences(Parser *, asdl_seq *, asdl_seq *);
int _PyPegen_check_barry_as_flufl(Parser *, Token *);
int _PyPegen_check_legacy_stmt(Parser *p, पाहायाग_ty t);
void *_PyPegen_raise_error_for_missing_comma(Parser *p, पाहायाग_ty a, पाहायाग_ty b);
ResultTokenWithMetadata *_PyPegen_check_fstring_conversion(Parser *p, Token *, पाहायाग_ty t);
ResultTokenWithMetadata *_PyPegen_setup_full_format_spec(Parser *, Token *, asdl_पाहायाग_seq *, int, int,
                                                         int, int, PyArena *);
mod_ty _PyPegen_make_module(Parser *, asdl_stmt_seq *);
void *_PyPegen_arguments_parsing_error(Parser *, पाहायाग_ty);
पाहायाग_ty _PyPegen_get_last_comprehension_item(comprehension_ty comprehension);
void *_PyPegen_nonparen_genexp_in_call(Parser *p, पाहायाग_ty मगा_कातविरान, asdl_comprehension_seq *comprehensions);
stmt_ty _PyPegen_checked_from_import(Parser *p, asdl_seq *dots, पाहायाग_ty module_name,
                                     asdl_alias_seq *names, पाहायाग_ty lazy_token, int lineno,
                                     int col_offset, int end_lineno, int end_col_offset,
                                     PyArena *arena);
asdl_stmt_seq* _PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts);
stmt_ty _PyPegen_register_stmt(Parser *p, stmt_ty s);

// Parser API

Parser *_PyPegen_Parser_New(struct tok_state *, int, int, int, int *, const char*, PyArena *);
void _PyPegen_Parser_Free(Parser *);
mod_ty _PyPegen_run_parser_from_file_pointer(FILE *, int, PyObject *, const char *,
                                    const char *, const char *, PyCompilerFlags *, int *, PyObject **,
                                    PyArena *);
void *_PyPegen_run_parser(Parser *);
mod_ty _PyPegen_run_parser_from_string(const char *, int, PyObject *, PyCompilerFlags *, PyArena *, PyObject *, PyObject **);
asdl_stmt_seq *_PyPegen_interactive_exit(Parser *);

// Generated function in parse.c - function definition in python.gram
void *_PyPegen_parse(Parser *);

#endif
