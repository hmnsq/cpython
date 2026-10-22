#ifndef PEGEN_H
#define PEGEN_H

#અન્તર્ભાવ <Python.h>
#અન્તર્ભાવ <pycore_ast.h>
#અન્તર્ભાવ <pycore_token.h>
#અન્તર્ભાવ <pycore_ceval.h>     // _Py_ReachedRecursionLimitWithMargin()
#અન્તર્ભાવ <pycore_pystate.h>   // _Py_get_machine_stack_pointer()
#અન્તર્ભાવ <pycore_pythonrun.h> // _PyOS_STACK_MARGIN_BYTES

#અન્તર્ભાવ "tokenizer/tokenizer.h"

#જો 0
#define PyPARSE_YIELD_IS_KEYWORD        0x0001
#endif

#define PyPARSE_DONT_IMPLY_DEDENT       0x0002

#જો 0
#define PyPARSE_WITH_IS_KEYWORD         0x0003
#define PyPARSE_PRINT_IS_FUNCTION       0x0004
#define PyPARSE_UNICODE_LITERALS        0x0008
#endif

#define PyPARSE_IGNORE_COOKIE 0x0010
#define PyPARSE_BARRY_AS_BDFL 0x0020
#define PyPARSE_TYPE_COMMENTS 0x0040
#define Py_PARSE_ALLOW_INCOMPLETE_આગત 0x0100

#define CURRENT_POS (-5)

પ્રકારવ્યાખ્યા સંરચના _memo {
    પૂર્ણાંક type;
    શૂન્ય *ગાંઠ;
    પૂર્ણાંક mark;
    સંરચના _memo *next;
} Memo;

પ્રકારવ્યાખ્યા સંરચના {
    પૂર્ણાંક type;
    // Text required by grammar actions or helpers; otherwise NULL.
    // An unfinished string token also has NULL text. Its grammar action
    // requests more input only if the token is valid at that position.
    PyObject *bytes;
    પૂર્ણાંક level;
    પૂર્ણાંક is_raw;
    પૂર્ણાંક lineno, col_offset, end_lineno, end_col_offset;
    Memo *memo;
    // Filter over the rule types present in `memo` (bit `type & 63` is set
    // for every entry): lets lookups skip walking the list on definite
    // misses, which are the common case.
    uint64_t memo_mask;
    PyObject *metadata;
} Token;

પ્રકારવ્યાખ્યા સંરચના {
    અચલ અક્ષર *str;
    પૂર્ણાંક type;
} KeywordToken;


પ્રકારવ્યાખ્યા સંરચના {
    સંરચના {
        પૂર્ણાંક lineno;
        અક્ષર *comment;  // The " <tag>" in "# type: ignore <tag>"
    } *વસ્તુઓ;
    size_t size;
    size_t અંક_વસ્તુઓ;
} growable_comment_array;

પ્રકારવ્યાખ્યા સંરચના {
    પૂર્ણાંક lineno;
    પૂર્ણાંક col_offset;
    પૂર્ણાંક end_lineno;
    પૂર્ણાંક end_col_offset;
} location;

પ્રકારવ્યાખ્યા સંરચના _identifier_cache_entry IdentifierCacheEntry;

પ્રકારવ્યાખ્યા સંરચના {
    સંરચના tok_state *tok;
    Token **tokens;
    પૂર્ણાંક mark;
    પૂર્ણાંક fill, size;
    PyArena *arena;
    KeywordToken **keywords;
    અક્ષર **soft_keywords;
    પૂર્ણાંક n_keyword_lists;
    પૂર્ણાંક start_rule;
    પૂર્ણાંક *errcode;
    unsigned અક્ષર parsing_started;
    unsigned અક્ષર આગત_exhausted;
    PyObject* normalize;
    પૂર્ણાંક starting_lineno;
    પૂર્ણાંક starting_col_offset;
    પૂર્ણાંક error_indicator;
    પૂર્ણાંક flags;
    પૂર્ણાંક feature_version;
    growable_comment_array type_ignore_comments;
    Token *known_err_token;
    પૂર્ણાંક level;
    પૂર્ણાંક call_invalid_rules;
    પૂર્ણાંક debug;
    location last_stmt_location;
    IdentifierCacheEntry *identifier_cache;
    PyThreadState *tstate;
    // Cached copy of tstate->c_stack_soft_limit offset by one margin toward
    // the stack top, so the per-rule stack check is a single inline comparison.
    uintptr_t ઢગલો_soft_limit;
} Parser;

// C-stack exhaustion check run on every rule entry. The fast path compares
// the machine stack pointer against the soft limit cached in the Parser;
// only when the stack is nearly exhausted is the full check called.
સ્થિર અંતઃરેખ પૂર્ણાંક
Py_Pegen_ઢગલો_exhausted(Parser *p)
{
#જો Py_ઢગલો_GROWS_DOWN
    જો (Py_get_machine_ઢગલો_pointer() > p->ઢગલો_soft_limit) {
#નહીંતર
    જો (Py_get_machine_ઢગલો_pointer() <= p->ઢગલો_soft_limit) {
#endif
        પ્રતિજ્ઞા(!_Py_ReachedRecursionLimitWithMargin(p->tstate, 1));
        પાછા 0;
    }
    પાછા _Py_ReachedRecursionLimitWithMargin(p->tstate, 1);
}

પ્રકારવ્યાખ્યા સંરચના {
    cmpop_ty cmpop;
    પદાવલિ_ty પદાવલિ;
} Cmpop_પદાવલિ_Pair;

પ્રકારવ્યાખ્યા સંરચના {
    પદાવલિ_ty ચાવી;
    પદાવલિ_ty કિંમત;
} ચાવી_કિંમત_Pair;

પ્રકારવ્યાખ્યા સંરચના {
    પદાવલિ_ty ચાવી;
    pattern_ty pattern;
} ચાવી_Pattern_Pair;

પ્રકારવ્યાખ્યા સંરચના {
    arg_ty arg;
    પદાવલિ_ty કિંમત;
} NameDefaultPair;

પ્રકારવ્યાખ્યા સંરચના {
    asdl_arg_seq *plain_names;
    asdl_seq *names_with_defaults; // asdl_seq* of NameDefaultsPair's
} SlashWithDefault;

પ્રકારવ્યાખ્યા સંરચના {
    arg_ty vararg;
    asdl_seq *kwonlyargs; // asdl_seq* of NameDefaultsPair's
    arg_ty kwarg;
} StarEtc;

પ્રકારવ્યાખ્યા સંરચના { operator_ty kind; } AugOperator;
પ્રકારવ્યાખ્યા સંરચના {
    શૂન્ય *element;
    પૂર્ણાંક is_keyword;
} KeywordOrStarred;

પ્રકારવ્યાખ્યા સંરચના {
    શૂન્ય *result;
    PyObject *metadata;
} ResultTokenWithMetadata;

// Internal parser functions
#જો defined(Py_DEBUG)
શૂન્ય _PyPegen_clear_memo_statistics(શૂન્ય);
PyObject *_PyPegen_get_memo_statistics(શૂન્ય);
#endif

પૂર્ણાંક Py_Pegen_ઉમેરો_memo(Parser *p, પૂર્ણાંક mark, પૂર્ણાંક type, શૂન્ય *ગાંઠ);
પૂર્ણાંક _PyPegen_update_memo(Parser *p, પૂર્ણાંક mark, પૂર્ણાંક type, શૂન્ય *ગાંઠ);
Memo *Py_Pegen_ઉમેરો_memo_direct(Parser *p, પૂર્ણાંક mark, પૂર્ણાંક type);
પૂર્ણાંક _PyPegen_is_memoized(Parser *p, પૂર્ણાંક type, શૂન્ય *pres);

પૂર્ણાંક _PyPegen_lookahead(પૂર્ણાંક, શૂન્ય *(func)(Parser *), Parser *);
પૂર્ણાંક Py_Pegen_lookahead_for_પદાવલિ(પૂર્ણાંક, પદાવલિ_ty (func)(Parser *), Parser *);
પૂર્ણાંક _PyPegen_lookahead_for_stmt(પૂર્ણાંક, stmt_ty (func)(Parser *), Parser *);
પૂર્ણાંક _PyPegen_lookahead_with_int(પૂર્ણાંક, Token *(func)(Parser *, પૂર્ણાંક), Parser *, પૂર્ણાંક);
પૂર્ણાંક _PyPegen_lookahead_with_string(પૂર્ણાંક, પદાવલિ_ty (func)(Parser *, અચલ અક્ષર*), Parser *, અચલ અક્ષર*);

Token *_PyPegen_expect_token(Parser *p, પૂર્ણાંક type);
શૂન્ય* _PyPegen_expect_forced_result(Parser *p, શૂન્ય* result, અચલ અક્ષર* expected);
Token *_PyPegen_expect_forced_token(Parser *p, પૂર્ણાંક type, અચલ અક્ષર* expected);
પદાવલિ_ty _PyPegen_expect_soft_keyword(Parser *p, અચલ અક્ષર *keyword);
પદાવલિ_ty _PyPegen_soft_keyword_token(Parser *p);
પદાવલિ_ty Py_Pegen_fstring_મધ્ય_token(Parser* p);
Token *_PyPegen_get_last_nonnwhitespace_token(Parser *);
પૂર્ણાંક _PyPegen_fill_token(Parser *p);
// Return -1 on failure, leaving the buffer and capacity unchanged.
પૂર્ણાંક _PyPegen_grow_loop_buffer(શૂન્ય ***buffer, Py_ssize_t *capacity);
પદાવલિ_ty _PyPegen_name_token(Parser *p);
પદાવલિ_ty Py_Pegen_સંખ્યા_token(Parser *p);
શૂન્ય *_PyPegen_string_token(Parser *p);
Py_ssize_t Py_Pegen_byte_offset_to_character_offset_લીટી(PyObject *લીટી, Py_ssize_t col_offset, Py_ssize_t end_col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset(PyObject *લીટી, Py_ssize_t col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset_raw(અચલ અક્ષર*, Py_ssize_t col_offset);

પ્રકારવ્યાખ્યા ગણના {
    STAR_TARGETS,
    DEL_TARGETS,
    FOR_TARGETS,
    SINGLE_TARGETS,
    ATTRIBUTE_OR_SUBSCRIPT_TARGETS
} TARGETS_TYPE;

// Error handling functions and APIs
પૂર્ણાંક _Pypegen_raise_decode_error(Parser *p);
પૂર્ણાંક _Pypegen_tokenizer_error(Parser *p);
શૂન્ય *Py_Pegen_raise_incomplete_આગત(Parser *p);
શૂન્ય *_PyPegen_raise_error(Parser *p, PyObject *errtype, પૂર્ણાંક use_mark, અચલ અક્ષર *errmsg, ...);
શૂન્ય *_PyPegen_raise_error_known_location(Parser *p, PyObject *errtype,
                                          Py_ssize_t lineno, Py_ssize_t col_offset,
                                          Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                                          અચલ અક્ષર *errmsg, va_list va);
શૂન્ય _Pypegen_set_syntax_error(Parser* p, Token* last_token);
શૂન્ય Pypegen_ઢગલો_overflow(Parser *p);

સ્થિર અંતઃરેખ શૂન્ય *
RAISE_ERROR_KNOWN_LOCATION(Parser *p, PyObject *errtype,
                           Py_ssize_t lineno, Py_ssize_t col_offset,
                           Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                           અચલ અક્ષર *errmsg, ...)
{
    va_list va;
    va_start(va, errmsg);
    Py_ssize_t _col_offset = (col_offset == CURRENT_POS ? CURRENT_POS : col_offset + 1);
    Py_ssize_t _end_col_offset = (end_col_offset == CURRENT_POS ? CURRENT_POS : end_col_offset + 1);
    _PyPegen_raise_error_known_location(p, errtype, lineno, _col_offset, end_lineno, _end_col_offset, errmsg, va);
    va_end(va);
    પાછા ખાલી;
}
#define RAISE_SYNTAX_ERROR(સંદેશ, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 0, સંદેશ, ##__VA_ARGS__)
#define RAISE_INDENTATION_ERROR(સંદેશ, ...) _PyPegen_raise_error(p, PyExc_IndentationError, 0, સંદેશ, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_ON_NEXT_TOKEN(સંદેશ, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 1, સંદેશ, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_RANGE(a, b, સંદેશ, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (b)->end_lineno, (b)->end_col_offset, સંદેશ, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_LOCATION(a, સંદેશ, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (a)->end_lineno, (a)->end_col_offset, સંદેશ, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_STARTING_FROM(a, સંદેશ, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, CURRENT_POS, CURRENT_POS, સંદેશ, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_INVALID_TARGET(type, e) _RAISE_SYNTAX_ERROR_INVALID_TARGET(p, type, e)

Py_LOCAL_INLINE(પૂર્ણાંક)
_PyPegen_tokens_are_adjacent(Token *a, Token *b)
{
    પાછા (a->end_lineno == b->lineno) && (a->end_col_offset == b->col_offset);
}

Py_LOCAL_INLINE(શૂન્ય *)
CHECK_CALL(Parser *p, શૂન્ય *result)
{
    જો (result == ખાલી) {
        પ્રતિજ્ઞા(PyErr_Occurred());
        p->error_indicator = 1;
    }
    પાછા result;
}

/* This is needed for helper functions that are allowed to
   return NULL without an error. Example: _PyPegen_seq_extract_starred_exprs */
Py_LOCAL_INLINE(શૂન્ય *)
CHECK_CALL_NULL_ALLOWED(Parser *p, શૂન્ય *result)
{
    જો (result == ખાલી && PyErr_Occurred()) {
        p->error_indicator = 1;
    }
    પાછા result;
}

#define CHECK(type, result) ((type) CHECK_CALL(p, result))
#define CHECK_NULL_ALLOWED(type, result) ((type) CHECK_CALL_NULL_ALLOWED(p, result))

પદાવલિ_ty _PyPegen_get_invalid_target(પદાવલિ_ty e, TARGETS_TYPE targets_type);
અચલ અક્ષર *Py_Pegen_get_પદાવલિ_name(પદાવલિ_ty);
Py_LOCAL_INLINE(શૂન્ય *)
_RAISE_SYNTAX_ERROR_INVALID_TARGET(Parser *p, TARGETS_TYPE type, શૂન્ય *e)
{
    પદાવલિ_ty invalid_target = CHECK_NULL_ALLOWED(પદાવલિ_ty, _PyPegen_get_invalid_target(e, type));
    જો (invalid_target != ખાલી) {
        અચલ અક્ષર *સંદેશ;
        જો (type == STAR_TARGETS || type == FOR_TARGETS) {
            સંદેશ = "cannot assign to %s";
        }
        નહીંતર {
            સંદેશ = "cannot delete %s";
        }
        પાછા RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
            invalid_target,
            સંદેશ,
            Py_Pegen_get_પદાવલિ_name(invalid_target)
        );
        પાછા RAISE_SYNTAX_ERROR_KNOWN_LOCATION(invalid_target, "invalid syntax");
    }
    પાછા ખાલી;
}

// Action utility functions

શૂન્ય *_PyPegen_dummy_name(Parser *p, ...);
શૂન્ય * _PyPegen_seq_last_item(asdl_seq *seq);
#define PyPegen_last_item(seq, type) ((type)_PyPegen_seq_last_item((asdl_seq*)seq))
શૂન્ય * _PyPegen_seq_first_item(asdl_seq *seq);
#define PyPegen_first_item(seq, type) ((type)_PyPegen_seq_first_item((asdl_seq*)seq))
#define UNUSED(પદાવલિ) કરો { (શૂન્ય)(પદાવલિ); } જ્યારે (0)
#define EXTRA_પદાવલિ(head, tail) head->lineno, (head)->col_offset, (tail)->end_lineno, (tail)->end_col_offset, p->arena
#define EXTRA _start_lineno, _start_col_offset, _end_lineno, _end_col_offset, p->arena
PyObject *_PyPegen_new_type_comment(Parser *, અચલ અક્ષર *);

Py_LOCAL_INLINE(PyObject *)
NEW_TYPE_COMMENT(Parser *p, Token *tc)
{
    જો (tc == ખાલી) {
        પાછા ખાલી;
    }
    અચલ અક્ષર *bytes = PyBytes_AsString(tc->bytes);
    જો (bytes == ખાલી) {
        જાઓ error;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    જો (tco == ખાલી) {
        જાઓ error;
    }
    પાછા tco;
 error:
    p->error_indicator = 1;  // Inline CHECK_CALL
    પાછા ખાલી;
}

Py_LOCAL_INLINE(શૂન્ય *)
INVALID_VERSION_CHECK(Parser *p, પૂર્ણાંક version, અક્ષર *સંદેશ, શૂન્ય *ગાંઠ)
{
    જો (ગાંઠ == ખાલી) {
        p->error_indicator = 1;  // Inline CHECK_CALL
        પાછા ખાલી;
    }
    જો (p->feature_version < version) {
        p->error_indicator = 1;
        પાછા RAISE_SYNTAX_ERROR("%s only supported in Python 3.%i and greater",
                                  સંદેશ, version);
    }
    પાછા ગાંઠ;
}

#define CHECK_VERSION(type, version, સંદેશ, ગાંઠ) ((type) INVALID_VERSION_CHECK(p, version, સંદેશ, ગાંઠ))

arg_ty _PyPegen_add_type_comment_to_arg(Parser *, arg_ty, Token *);
PyObject *_PyPegen_new_identifier(Parser *, અચલ અક્ષર *);
asdl_seq *_PyPegen_singleton_seq(Parser *, શૂન્ય *);
asdl_seq *Py_Pegen_seq_ઉમેરો_in_front(Parser *, શૂન્ય *, asdl_seq *);
asdl_seq *_PyPegen_seq_append_to_end(Parser *, asdl_seq *, શૂન્ય *);
asdl_seq *_PyPegen_seq_flatten(Parser *, asdl_seq *);
પદાવલિ_ty _PyPegen_join_names_with_dot(Parser *, પદાવલિ_ty, પદાવલિ_ty);
પૂર્ણાંક Py_Pegen_seq_ગણતરી_dots(asdl_seq *);
alias_ty _PyPegen_alias_for_star(Parser *, પૂર્ણાંક, પૂર્ણાંક, પૂર્ણાંક, પૂર્ણાંક, PyArena *);
asdl_identifier_seq *_PyPegen_map_names_to_ids(Parser *, asdl_પદાવલિ_seq *);
Cmpop_પદાવલિ_Pair *Py_Pegen_cmpop_પદાવલિ_pair(Parser *, cmpop_ty, પદાવલિ_ty);
asdl_int_seq *_PyPegen_get_cmpops(Parser *p, asdl_seq *);
asdl_પદાવલિ_seq *_PyPegen_get_exprs(Parser *, asdl_seq *);
પદાવલિ_ty Py_Pegen_set_પદાવલિ_context(Parser *, પદાવલિ_ty, પદાવલિ_context_ty);
પદાવલિ_ty _PyPegen_make_target(Parser *, પદાવલિ_ty, TARGETS_TYPE);
ચાવી_કિંમત_Pair *Py_Pegen_ચાવી_કિંમત_pair(Parser *, પદાવલિ_ty, પદાવલિ_ty);
asdl_પદાવલિ_seq *Py_Pegen_get_ચાવીઓ(Parser *, asdl_seq *);
asdl_પદાવલિ_seq *Py_Pegen_get_કિંમતો(Parser *, asdl_seq *);
ચાવી_Pattern_Pair *Py_Pegen_ચાવી_pattern_pair(Parser *, પદાવલિ_ty, pattern_ty);
asdl_પદાવલિ_seq *Py_Pegen_get_pattern_ચાવીઓ(Parser *, asdl_seq *);
asdl_pattern_seq *_PyPegen_get_patterns(Parser *, asdl_seq *);
NameDefaultPair *_PyPegen_name_default_pair(Parser *, arg_ty, પદાવલિ_ty, Token *);
SlashWithDefault *_PyPegen_slash_with_default(Parser *, asdl_arg_seq *, asdl_seq *);
StarEtc *_PyPegen_star_etc(Parser *, arg_ty, asdl_seq *, arg_ty);
arguments_ty _PyPegen_make_arguments(Parser *, asdl_arg_seq *, SlashWithDefault *,
                                     asdl_arg_seq *, asdl_seq *, StarEtc *);
arguments_ty _PyPegen_empty_arguments(Parser *);
પદાવલિ_ty _PyPegen_template_str(Parser *p, Token *a, asdl_પદાવલિ_seq *raw_expressions, Token *b);
પદાવલિ_ty _PyPegen_joined_str(Parser *p, Token *a, asdl_પદાવલિ_seq *raw_expressions, Token *b);
પદાવલિ_ty _PyPegen_interpolation(Parser *, પદાવલિ_ty, Token *, ResultTokenWithMetadata *, ResultTokenWithMetadata *, Token *,
                                 પૂર્ણાંક, પૂર્ણાંક, પૂર્ણાંક, પૂર્ણાંક, PyArena *);
પદાવલિ_ty Py_Pegen_formatted_કિંમત(Parser *, પદાવલિ_ty, Token *, ResultTokenWithMetadata *, ResultTokenWithMetadata *, Token *,
                                 પૂર્ણાંક, પૂર્ણાંક, પૂર્ણાંક, પૂર્ણાંક, PyArena *);
AugOperator *_PyPegen_augoperator(Parser*, operator_ty type);
stmt_ty _PyPegen_function_def_decorators(Parser *, asdl_પદાવલિ_seq *, stmt_ty);
stmt_ty _PyPegen_class_def_decorators(Parser *, asdl_પદાવલિ_seq *, stmt_ty);
KeywordOrStarred *_PyPegen_keyword_or_starred(Parser *, શૂન્ય *, પૂર્ણાંક);
asdl_પદાવલિ_seq *_PyPegen_seq_extract_starred_exprs(Parser *, asdl_seq *);
asdl_keyword_seq *_PyPegen_seq_delete_starred_exprs(Parser *, asdl_seq *);
પદાવલિ_ty _PyPegen_collect_call_seqs(Parser *, asdl_પદાવલિ_seq *, asdl_seq *,
                     પૂર્ણાંક lineno, પૂર્ણાંક col_offset, પૂર્ણાંક end_lineno,
                     પૂર્ણાંક end_col_offset, PyArena *arena);
પદાવલિ_ty _PyPegen_constant_from_token(Parser* p, Token* tok);
પદાવલિ_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok);
પદાવલિ_ty _PyPegen_constant_from_string(Parser* p, Token* tok);
પદાવલિ_ty _PyPegen_concatenate_tstrings(Parser *p, asdl_પદાવલિ_seq *, પૂર્ણાંક, પૂર્ણાંક, પૂર્ણાંક, પૂર્ણાંક, PyArena *);
પદાવલિ_ty _PyPegen_concatenate_strings(Parser *p, asdl_પદાવલિ_seq *, પૂર્ણાંક, પૂર્ણાંક, પૂર્ણાંક, પૂર્ણાંક, PyArena *);
પદાવલિ_ty _PyPegen_FetchRawForm(Parser *p, પૂર્ણાંક, પૂર્ણાંક, પૂર્ણાંક, પૂર્ણાંક);
પદાવલિ_ty _PyPegen_ensure_imaginary(Parser *p, પદાવલિ_ty);
પદાવલિ_ty _PyPegen_ensure_real(Parser *p, પદાવલિ_ty);
asdl_seq *_PyPegen_join_sequences(Parser *, asdl_seq *, asdl_seq *);
પૂર્ણાંક _PyPegen_check_barry_as_flufl(Parser *, Token *);
પૂર્ણાંક _PyPegen_check_legacy_stmt(Parser *p, પદાવલિ_ty t);
શૂન્ય *_PyPegen_raise_error_for_missing_comma(Parser *p, પદાવલિ_ty a, પદાવલિ_ty b);
ResultTokenWithMetadata *_PyPegen_check_fstring_conversion(Parser *p, Token *, પદાવલિ_ty t);
ResultTokenWithMetadata *_PyPegen_setup_full_format_spec(Parser *, Token *, asdl_પદાવલિ_seq *, પૂર્ણાંક, પૂર્ણાંક,
                                                         પૂર્ણાંક, પૂર્ણાંક, PyArena *);
mod_ty _PyPegen_make_module(Parser *, asdl_stmt_seq *);
શૂન્ય *_PyPegen_arguments_parsing_error(Parser *, પદાવલિ_ty);
પદાવલિ_ty _PyPegen_get_last_comprehension_item(comprehension_ty comprehension);
શૂન્ય *_PyPegen_nonparen_genexp_in_call(Parser *p, પદાવલિ_ty args, asdl_comprehension_seq *comprehensions);
stmt_ty _PyPegen_checked_from_import(Parser *p, asdl_seq *dots, પદાવલિ_ty module_name,
                                     asdl_alias_seq *names, પદાવલિ_ty lazy_token, પૂર્ણાંક lineno,
                                     પૂર્ણાંક col_offset, પૂર્ણાંક end_lineno, પૂર્ણાંક end_col_offset,
                                     PyArena *arena);
asdl_stmt_seq* _PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts);
stmt_ty _PyPegen_register_stmt(Parser *p, stmt_ty s);

// Parser API

Parser *_PyPegen_Parser_New(સંરચના tok_state *, પૂર્ણાંક, પૂર્ણાંક, પૂર્ણાંક, પૂર્ણાંક *, અચલ અક્ષર*, PyArena *);
શૂન્ય _PyPegen_Parser_Free(Parser *);
mod_ty _PyPegen_run_parser_from_file_pointer(FILE *, પૂર્ણાંક, PyObject *, અચલ અક્ષર *,
                                    અચલ અક્ષર *, અચલ અક્ષર *, PyCompilerFlags *, પૂર્ણાંક *, PyObject **,
                                    PyArena *);
શૂન્ય *_PyPegen_run_parser(Parser *);
mod_ty _PyPegen_run_parser_from_string(અચલ અક્ષર *, પૂર્ણાંક, PyObject *, PyCompilerFlags *, PyArena *, PyObject *, PyObject **);
asdl_stmt_seq *_PyPegen_interactive_exit(Parser *);

// Generated function in parse.c - function definition in python.gram
શૂન્ય *_PyPegen_parse(Parser *);

#endif
