#ifndef PEGEN_H
#define PEGEN_H

#അന്തര്ഭാവ <Python.h>
#അന്തര്ഭാവ <pycore_ast.h>
#അന്തര്ഭാവ <pycore_token.h>
#അന്തര്ഭാവ <pycore_ceval.h>     // _Py_ReachedRecursionLimitWithMargin()
#അന്തര്ഭാവ <pycore_pystate.h>   // _Py_get_machine_stack_pointer()
#അന്തര്ഭാവ <pycore_pythonrun.h> // _PyOS_STACK_MARGIN_BYTES

#അന്തര്ഭാവ "tokenizer/tokenizer.h"

#എങ്കിൽ 0
#define PyPARSE_YIELD_IS_KEYWORD        0x0001
#endif

#define PyPARSE_DONT_IMPLY_DEDENT       0x0002

#എങ്കിൽ 0
#define PyPARSE_WITH_IS_KEYWORD         0x0003
#define PyPARSE_PRINT_IS_FUNCTION       0x0004
#define PyPARSE_UNICODE_LITERALS        0x0008
#endif

#define PyPARSE_IGNORE_COOKIE 0x0010
#define PyPARSE_BARRY_AS_BDFL 0x0020
#define PyPARSE_TYPE_COMMENTS 0x0040
#define Py_PARSE_ALLOW_INCOMPLETE_നിവേശം 0x0100

#define CURRENT_POS (-5)

തരനിർവചനം ഘടന _memo {
    പൂർണ്ണസംഖ്യ type;
    ശൂന്യം *കണ്ണി;
    പൂർണ്ണസംഖ്യ mark;
    ഘടന _memo *next;
} Memo;

തരനിർവചനം ഘടന {
    പൂർണ്ണസംഖ്യ type;
    // Text required by grammar actions or helpers; otherwise NULL.
    // An unfinished string token also has NULL text. Its grammar action
    // requests more input only if the token is valid at that position.
    PyObject *bytes;
    പൂർണ്ണസംഖ്യ level;
    പൂർണ്ണസംഖ്യ is_raw;
    പൂർണ്ണസംഖ്യ lineno, col_offset, end_lineno, end_col_offset;
    Memo *memo;
    // Filter over the rule types present in `memo` (bit `type & 63` is set
    // for every entry): lets lookups skip walking the list on definite
    // misses, which are the common case.
    uint64_t memo_mask;
    PyObject *metadata;
} Token;

തരനിർവചനം ഘടന {
    സ്ഥിരം അക്ഷരം *str;
    പൂർണ്ണസംഖ്യ type;
} KeywordToken;


തരനിർവചനം ഘടന {
    ഘടന {
        പൂർണ്ണസംഖ്യ lineno;
        അക്ഷരം *comment;  // The " <tag>" in "# type: ignore <tag>"
    } *ഇനങ്ങൾ;
    size_t size;
    size_t അക്കം_ഇനങ്ങൾ;
} growable_comment_array;

തരനിർവചനം ഘടന {
    പൂർണ്ണസംഖ്യ lineno;
    പൂർണ്ണസംഖ്യ col_offset;
    പൂർണ്ണസംഖ്യ end_lineno;
    പൂർണ്ണസംഖ്യ end_col_offset;
} location;

തരനിർവചനം ഘടന _identifier_cache_entry IdentifierCacheEntry;

തരനിർവചനം ഘടന {
    ഘടന tok_state *tok;
    Token **tokens;
    പൂർണ്ണസംഖ്യ mark;
    പൂർണ്ണസംഖ്യ fill, size;
    PyArena *arena;
    KeywordToken **keywords;
    അക്ഷരം **soft_keywords;
    പൂർണ്ണസംഖ്യ n_keyword_lists;
    പൂർണ്ണസംഖ്യ start_rule;
    പൂർണ്ണസംഖ്യ *errcode;
    unsigned അക്ഷരം parsing_started;
    unsigned അക്ഷരം നിവേശം_exhausted;
    PyObject* normalize;
    പൂർണ്ണസംഖ്യ starting_lineno;
    പൂർണ്ണസംഖ്യ starting_col_offset;
    പൂർണ്ണസംഖ്യ error_indicator;
    പൂർണ്ണസംഖ്യ flags;
    പൂർണ്ണസംഖ്യ feature_version;
    growable_comment_array type_ignore_comments;
    Token *known_err_token;
    പൂർണ്ണസംഖ്യ level;
    പൂർണ്ണസംഖ്യ call_invalid_rules;
    പൂർണ്ണസംഖ്യ debug;
    location last_stmt_location;
    IdentifierCacheEntry *identifier_cache;
    PyThreadState *tstate;
    // Cached copy of tstate->c_stack_soft_limit offset by one margin toward
    // the stack top, so the per-rule stack check is a single inline comparison.
    uintptr_t അട്ടി_soft_limit;
} Parser;

// C-stack exhaustion check run on every rule entry. The fast path compares
// the machine stack pointer against the soft limit cached in the Parser;
// only when the stack is nearly exhausted is the full check called.
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ
Py_Pegen_അട്ടി_exhausted(Parser *p)
{
#എങ്കിൽ Py_അട്ടി_GROWS_DOWN
    എങ്കിൽ (Py_get_machine_അട്ടി_pointer() > p->അട്ടി_soft_limit) {
#അല്ലെങ്കിൽ
    എങ്കിൽ (Py_get_machine_അട്ടി_pointer() <= p->അട്ടി_soft_limit) {
#endif
        പ്രതിജ്ഞാ(!_Py_ReachedRecursionLimitWithMargin(p->tstate, 1));
        തിരികെ 0;
    }
    തിരികെ _Py_ReachedRecursionLimitWithMargin(p->tstate, 1);
}

തരനിർവചനം ഘടന {
    cmpop_ty cmpop;
    വ്യഞ്ജകം_ty വ്യഞ്ജകം;
} Cmpop_വ്യഞ്ജകം_Pair;

തരനിർവചനം ഘടന {
    വ്യഞ്ജകം_ty താക്കോൽ;
    വ്യഞ്ജകം_ty മൂല്യം;
} താക്കോൽ_മൂല്യം_Pair;

തരനിർവചനം ഘടന {
    വ്യഞ്ജകം_ty താക്കോൽ;
    pattern_ty pattern;
} താക്കോൽ_Pattern_Pair;

തരനിർവചനം ഘടന {
    arg_ty arg;
    വ്യഞ്ജകം_ty മൂല്യം;
} NameDefaultPair;

തരനിർവചനം ഘടന {
    asdl_arg_seq *plain_names;
    asdl_seq *names_with_defaults; // asdl_seq* of NameDefaultsPair's
} SlashWithDefault;

തരനിർവചനം ഘടന {
    arg_ty vararg;
    asdl_seq *kwonlyargs; // asdl_seq* of NameDefaultsPair's
    arg_ty kwarg;
} StarEtc;

തരനിർവചനം ഘടന { operator_ty kind; } AugOperator;
തരനിർവചനം ഘടന {
    ശൂന്യം *element;
    പൂർണ്ണസംഖ്യ is_keyword;
} KeywordOrStarred;

തരനിർവചനം ഘടന {
    ശൂന്യം *result;
    PyObject *metadata;
} ResultTokenWithMetadata;

// Internal parser functions
#എങ്കിൽ defined(Py_DEBUG)
ശൂന്യം _PyPegen_clear_memo_statistics(ശൂന്യം);
PyObject *_PyPegen_get_memo_statistics(ശൂന്യം);
#endif

പൂർണ്ണസംഖ്യ Py_Pegen_തിരുകുക_memo(Parser *p, പൂർണ്ണസംഖ്യ mark, പൂർണ്ണസംഖ്യ type, ശൂന്യം *കണ്ണി);
പൂർണ്ണസംഖ്യ _PyPegen_update_memo(Parser *p, പൂർണ്ണസംഖ്യ mark, പൂർണ്ണസംഖ്യ type, ശൂന്യം *കണ്ണി);
Memo *Py_Pegen_തിരുകുക_memo_direct(Parser *p, പൂർണ്ണസംഖ്യ mark, പൂർണ്ണസംഖ്യ type);
പൂർണ്ണസംഖ്യ _PyPegen_is_memoized(Parser *p, പൂർണ്ണസംഖ്യ type, ശൂന്യം *pres);

പൂർണ്ണസംഖ്യ _PyPegen_lookahead(പൂർണ്ണസംഖ്യ, ശൂന്യം *(func)(Parser *), Parser *);
പൂർണ്ണസംഖ്യ Py_Pegen_lookahead_for_വ്യഞ്ജകം(പൂർണ്ണസംഖ്യ, വ്യഞ്ജകം_ty (func)(Parser *), Parser *);
പൂർണ്ണസംഖ്യ _PyPegen_lookahead_for_stmt(പൂർണ്ണസംഖ്യ, stmt_ty (func)(Parser *), Parser *);
പൂർണ്ണസംഖ്യ _PyPegen_lookahead_with_int(പൂർണ്ണസംഖ്യ, Token *(func)(Parser *, പൂർണ്ണസംഖ്യ), Parser *, പൂർണ്ണസംഖ്യ);
പൂർണ്ണസംഖ്യ _PyPegen_lookahead_with_string(പൂർണ്ണസംഖ്യ, വ്യഞ്ജകം_ty (func)(Parser *, സ്ഥിരം അക്ഷരം*), Parser *, സ്ഥിരം അക്ഷരം*);

Token *_PyPegen_expect_token(Parser *p, പൂർണ്ണസംഖ്യ type);
ശൂന്യം* _PyPegen_expect_forced_result(Parser *p, ശൂന്യം* result, സ്ഥിരം അക്ഷരം* expected);
Token *_PyPegen_expect_forced_token(Parser *p, പൂർണ്ണസംഖ്യ type, സ്ഥിരം അക്ഷരം* expected);
വ്യഞ്ജകം_ty _PyPegen_expect_soft_keyword(Parser *p, സ്ഥിരം അക്ഷരം *keyword);
വ്യഞ്ജകം_ty _PyPegen_soft_keyword_token(Parser *p);
വ്യഞ്ജകം_ty Py_Pegen_fstring_നടുവ്_token(Parser* p);
Token *_PyPegen_get_last_nonnwhitespace_token(Parser *);
പൂർണ്ണസംഖ്യ _PyPegen_fill_token(Parser *p);
// Return -1 on failure, leaving the buffer and capacity unchanged.
പൂർണ്ണസംഖ്യ _PyPegen_grow_loop_buffer(ശൂന്യം ***buffer, Py_ssize_t *capacity);
വ്യഞ്ജകം_ty _PyPegen_name_token(Parser *p);
വ്യഞ്ജകം_ty Py_Pegen_സംഖ്യ_token(Parser *p);
ശൂന്യം *_PyPegen_string_token(Parser *p);
Py_ssize_t Py_Pegen_byte_offset_to_character_offset_വരി(PyObject *വരി, Py_ssize_t col_offset, Py_ssize_t end_col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset(PyObject *വരി, Py_ssize_t col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset_raw(സ്ഥിരം അക്ഷരം*, Py_ssize_t col_offset);

തരനിർവചനം എണ്ണൽ {
    STAR_TARGETS,
    DEL_TARGETS,
    FOR_TARGETS,
    SINGLE_TARGETS,
    ATTRIBUTE_OR_SUBSCRIPT_TARGETS
} TARGETS_TYPE;

// Error handling functions and APIs
പൂർണ്ണസംഖ്യ _Pypegen_raise_decode_error(Parser *p);
പൂർണ്ണസംഖ്യ _Pypegen_tokenizer_error(Parser *p);
ശൂന്യം *Py_Pegen_raise_incomplete_നിവേശം(Parser *p);
ശൂന്യം *_PyPegen_raise_error(Parser *p, PyObject *errtype, പൂർണ്ണസംഖ്യ use_mark, സ്ഥിരം അക്ഷരം *errmsg, ...);
ശൂന്യം *_PyPegen_raise_error_known_location(Parser *p, PyObject *errtype,
                                          Py_ssize_t lineno, Py_ssize_t col_offset,
                                          Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                                          സ്ഥിരം അക്ഷരം *errmsg, va_list va);
ശൂന്യം _Pypegen_set_syntax_error(Parser* p, Token* last_token);
ശൂന്യം Pypegen_അട്ടി_overflow(Parser *p);

സ്ഥിതം വരിയിൽ ശൂന്യം *
RAISE_ERROR_KNOWN_LOCATION(Parser *p, PyObject *errtype,
                           Py_ssize_t lineno, Py_ssize_t col_offset,
                           Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                           സ്ഥിരം അക്ഷരം *errmsg, ...)
{
    va_list va;
    va_start(va, errmsg);
    Py_ssize_t _col_offset = (col_offset == CURRENT_POS ? CURRENT_POS : col_offset + 1);
    Py_ssize_t _end_col_offset = (end_col_offset == CURRENT_POS ? CURRENT_POS : end_col_offset + 1);
    _PyPegen_raise_error_known_location(p, errtype, lineno, _col_offset, end_lineno, _end_col_offset, errmsg, va);
    va_end(va);
    തിരികെ ശൂന്യ;
}
#define RAISE_SYNTAX_ERROR(സന്ദേശം, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 0, സന്ദേശം, ##__VA_ARGS__)
#define RAISE_INDENTATION_ERROR(സന്ദേശം, ...) _PyPegen_raise_error(p, PyExc_IndentationError, 0, സന്ദേശം, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_ON_NEXT_TOKEN(സന്ദേശം, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 1, സന്ദേശം, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_RANGE(a, b, സന്ദേശം, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (b)->end_lineno, (b)->end_col_offset, സന്ദേശം, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_LOCATION(a, സന്ദേശം, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (a)->end_lineno, (a)->end_col_offset, സന്ദേശം, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_STARTING_FROM(a, സന്ദേശം, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, CURRENT_POS, CURRENT_POS, സന്ദേശം, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_INVALID_TARGET(type, e) _RAISE_SYNTAX_ERROR_INVALID_TARGET(p, type, e)

Py_LOCAL_INLINE(പൂർണ്ണസംഖ്യ)
_PyPegen_tokens_are_adjacent(Token *a, Token *b)
{
    തിരികെ (a->end_lineno == b->lineno) && (a->end_col_offset == b->col_offset);
}

Py_LOCAL_INLINE(ശൂന്യം *)
CHECK_CALL(Parser *p, ശൂന്യം *result)
{
    എങ്കിൽ (result == ശൂന്യ) {
        പ്രതിജ്ഞാ(PyErr_Occurred());
        p->error_indicator = 1;
    }
    തിരികെ result;
}

/* This is needed for helper functions that are allowed to
   return NULL without an error. Example: _PyPegen_seq_extract_starred_exprs */
Py_LOCAL_INLINE(ശൂന്യം *)
CHECK_CALL_NULL_ALLOWED(Parser *p, ശൂന്യം *result)
{
    എങ്കിൽ (result == ശൂന്യ && PyErr_Occurred()) {
        p->error_indicator = 1;
    }
    തിരികെ result;
}

#define CHECK(type, result) ((type) CHECK_CALL(p, result))
#define CHECK_NULL_ALLOWED(type, result) ((type) CHECK_CALL_NULL_ALLOWED(p, result))

വ്യഞ്ജകം_ty _PyPegen_get_invalid_target(വ്യഞ്ജകം_ty e, TARGETS_TYPE targets_type);
സ്ഥിരം അക്ഷരം *Py_Pegen_get_വ്യഞ്ജകം_name(വ്യഞ്ജകം_ty);
Py_LOCAL_INLINE(ശൂന്യം *)
_RAISE_SYNTAX_ERROR_INVALID_TARGET(Parser *p, TARGETS_TYPE type, ശൂന്യം *e)
{
    വ്യഞ്ജകം_ty invalid_target = CHECK_NULL_ALLOWED(വ്യഞ്ജകം_ty, _PyPegen_get_invalid_target(e, type));
    എങ്കിൽ (invalid_target != ശൂന്യ) {
        സ്ഥിരം അക്ഷരം *സന്ദേശം;
        എങ്കിൽ (type == STAR_TARGETS || type == FOR_TARGETS) {
            സന്ദേശം = "cannot assign to %s";
        }
        അല്ലെങ്കിൽ {
            സന്ദേശം = "cannot delete %s";
        }
        തിരികെ RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
            invalid_target,
            സന്ദേശം,
            Py_Pegen_get_വ്യഞ്ജകം_name(invalid_target)
        );
        തിരികെ RAISE_SYNTAX_ERROR_KNOWN_LOCATION(invalid_target, "invalid syntax");
    }
    തിരികെ ശൂന്യ;
}

// Action utility functions

ശൂന്യം *_PyPegen_dummy_name(Parser *p, ...);
ശൂന്യം * _PyPegen_seq_last_item(asdl_seq *seq);
#define PyPegen_last_item(seq, type) ((type)_PyPegen_seq_last_item((asdl_seq*)seq))
ശൂന്യം * _PyPegen_seq_first_item(asdl_seq *seq);
#define PyPegen_first_item(seq, type) ((type)_PyPegen_seq_first_item((asdl_seq*)seq))
#define UNUSED(വ്യഞ്ജകം) ചെയ്യുക { (ശൂന്യം)(വ്യഞ്ജകം); } ആകുമ്പോൾ (0)
#define EXTRA_വ്യഞ്ജകം(head, tail) head->lineno, (head)->col_offset, (tail)->end_lineno, (tail)->end_col_offset, p->arena
#define EXTRA _start_lineno, _start_col_offset, _end_lineno, _end_col_offset, p->arena
PyObject *_PyPegen_new_type_comment(Parser *, സ്ഥിരം അക്ഷരം *);

Py_LOCAL_INLINE(PyObject *)
NEW_TYPE_COMMENT(Parser *p, Token *tc)
{
    എങ്കിൽ (tc == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }
    സ്ഥിരം അക്ഷരം *bytes = PyBytes_AsString(tc->bytes);
    എങ്കിൽ (bytes == ശൂന്യ) {
        പോകുക error;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    എങ്കിൽ (tco == ശൂന്യ) {
        പോകുക error;
    }
    തിരികെ tco;
 error:
    p->error_indicator = 1;  // Inline CHECK_CALL
    തിരികെ ശൂന്യ;
}

Py_LOCAL_INLINE(ശൂന്യം *)
INVALID_VERSION_CHECK(Parser *p, പൂർണ്ണസംഖ്യ version, അക്ഷരം *സന്ദേശം, ശൂന്യം *കണ്ണി)
{
    എങ്കിൽ (കണ്ണി == ശൂന്യ) {
        p->error_indicator = 1;  // Inline CHECK_CALL
        തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (p->feature_version < version) {
        p->error_indicator = 1;
        തിരികെ RAISE_SYNTAX_ERROR("%s only supported in Python 3.%i and greater",
                                  സന്ദേശം, version);
    }
    തിരികെ കണ്ണി;
}

#define CHECK_VERSION(type, version, സന്ദേശം, കണ്ണി) ((type) INVALID_VERSION_CHECK(p, version, സന്ദേശം, കണ്ണി))

arg_ty _PyPegen_add_type_comment_to_arg(Parser *, arg_ty, Token *);
PyObject *_PyPegen_new_identifier(Parser *, സ്ഥിരം അക്ഷരം *);
asdl_seq *_PyPegen_singleton_seq(Parser *, ശൂന്യം *);
asdl_seq *Py_Pegen_seq_തിരുകുക_in_front(Parser *, ശൂന്യം *, asdl_seq *);
asdl_seq *_PyPegen_seq_append_to_end(Parser *, asdl_seq *, ശൂന്യം *);
asdl_seq *_PyPegen_seq_flatten(Parser *, asdl_seq *);
വ്യഞ്ജകം_ty _PyPegen_join_names_with_dot(Parser *, വ്യഞ്ജകം_ty, വ്യഞ്ജകം_ty);
പൂർണ്ണസംഖ്യ Py_Pegen_seq_എണ്ണം_dots(asdl_seq *);
alias_ty _PyPegen_alias_for_star(Parser *, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, PyArena *);
asdl_identifier_seq *_PyPegen_map_names_to_ids(Parser *, asdl_വ്യഞ്ജകം_seq *);
Cmpop_വ്യഞ്ജകം_Pair *Py_Pegen_cmpop_വ്യഞ്ജകം_pair(Parser *, cmpop_ty, വ്യഞ്ജകം_ty);
asdl_int_seq *_PyPegen_get_cmpops(Parser *p, asdl_seq *);
asdl_വ്യഞ്ജകം_seq *_PyPegen_get_exprs(Parser *, asdl_seq *);
വ്യഞ്ജകം_ty Py_Pegen_set_വ്യഞ്ജകം_context(Parser *, വ്യഞ്ജകം_ty, വ്യഞ്ജകം_context_ty);
വ്യഞ്ജകം_ty _PyPegen_make_target(Parser *, വ്യഞ്ജകം_ty, TARGETS_TYPE);
താക്കോൽ_മൂല്യം_Pair *Py_Pegen_താക്കോൽ_മൂല്യം_pair(Parser *, വ്യഞ്ജകം_ty, വ്യഞ്ജകം_ty);
asdl_വ്യഞ്ജകം_seq *Py_Pegen_get_താക്കോലുകൾ(Parser *, asdl_seq *);
asdl_വ്യഞ്ജകം_seq *Py_Pegen_get_മൂല്യങ്ങൾ(Parser *, asdl_seq *);
താക്കോൽ_Pattern_Pair *Py_Pegen_താക്കോൽ_pattern_pair(Parser *, വ്യഞ്ജകം_ty, pattern_ty);
asdl_വ്യഞ്ജകം_seq *Py_Pegen_get_pattern_താക്കോലുകൾ(Parser *, asdl_seq *);
asdl_pattern_seq *_PyPegen_get_patterns(Parser *, asdl_seq *);
NameDefaultPair *_PyPegen_name_default_pair(Parser *, arg_ty, വ്യഞ്ജകം_ty, Token *);
SlashWithDefault *_PyPegen_slash_with_default(Parser *, asdl_arg_seq *, asdl_seq *);
StarEtc *_PyPegen_star_etc(Parser *, arg_ty, asdl_seq *, arg_ty);
arguments_ty _PyPegen_make_arguments(Parser *, asdl_arg_seq *, SlashWithDefault *,
                                     asdl_arg_seq *, asdl_seq *, StarEtc *);
arguments_ty _PyPegen_empty_arguments(Parser *);
വ്യഞ്ജകം_ty _PyPegen_template_str(Parser *p, Token *a, asdl_വ്യഞ്ജകം_seq *raw_expressions, Token *b);
വ്യഞ്ജകം_ty _PyPegen_joined_str(Parser *p, Token *a, asdl_വ്യഞ്ജകം_seq *raw_expressions, Token *b);
വ്യഞ്ജകം_ty _PyPegen_interpolation(Parser *, വ്യഞ്ജകം_ty, Token *, ResultTokenWithMetadata *, ResultTokenWithMetadata *, Token *,
                                 പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, PyArena *);
വ്യഞ്ജകം_ty Py_Pegen_formatted_മൂല്യം(Parser *, വ്യഞ്ജകം_ty, Token *, ResultTokenWithMetadata *, ResultTokenWithMetadata *, Token *,
                                 പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, PyArena *);
AugOperator *_PyPegen_augoperator(Parser*, operator_ty type);
stmt_ty _PyPegen_function_def_decorators(Parser *, asdl_വ്യഞ്ജകം_seq *, stmt_ty);
stmt_ty _PyPegen_class_def_decorators(Parser *, asdl_വ്യഞ്ജകം_seq *, stmt_ty);
KeywordOrStarred *_PyPegen_keyword_or_starred(Parser *, ശൂന്യം *, പൂർണ്ണസംഖ്യ);
asdl_വ്യഞ്ജകം_seq *_PyPegen_seq_extract_starred_exprs(Parser *, asdl_seq *);
asdl_keyword_seq *_PyPegen_seq_delete_starred_exprs(Parser *, asdl_seq *);
വ്യഞ്ജകം_ty _PyPegen_collect_call_seqs(Parser *, asdl_വ്യഞ്ജകം_seq *, asdl_seq *,
                     പൂർണ്ണസംഖ്യ lineno, പൂർണ്ണസംഖ്യ col_offset, പൂർണ്ണസംഖ്യ end_lineno,
                     പൂർണ്ണസംഖ്യ end_col_offset, PyArena *arena);
വ്യഞ്ജകം_ty _PyPegen_constant_from_token(Parser* p, Token* tok);
വ്യഞ്ജകം_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok);
വ്യഞ്ജകം_ty _PyPegen_constant_from_string(Parser* p, Token* tok);
വ്യഞ്ജകം_ty _PyPegen_concatenate_tstrings(Parser *p, asdl_വ്യഞ്ജകം_seq *, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, PyArena *);
വ്യഞ്ജകം_ty _PyPegen_concatenate_strings(Parser *p, asdl_വ്യഞ്ജകം_seq *, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, PyArena *);
വ്യഞ്ജകം_ty _PyPegen_FetchRawForm(Parser *p, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ);
വ്യഞ്ജകം_ty _PyPegen_ensure_imaginary(Parser *p, വ്യഞ്ജകം_ty);
വ്യഞ്ജകം_ty _PyPegen_ensure_real(Parser *p, വ്യഞ്ജകം_ty);
asdl_seq *_PyPegen_join_sequences(Parser *, asdl_seq *, asdl_seq *);
പൂർണ്ണസംഖ്യ _PyPegen_check_barry_as_flufl(Parser *, Token *);
പൂർണ്ണസംഖ്യ _PyPegen_check_legacy_stmt(Parser *p, വ്യഞ്ജകം_ty t);
ശൂന്യം *_PyPegen_raise_error_for_missing_comma(Parser *p, വ്യഞ്ജകം_ty a, വ്യഞ്ജകം_ty b);
ResultTokenWithMetadata *_PyPegen_check_fstring_conversion(Parser *p, Token *, വ്യഞ്ജകം_ty t);
ResultTokenWithMetadata *_PyPegen_setup_full_format_spec(Parser *, Token *, asdl_വ്യഞ്ജകം_seq *, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ,
                                                         പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, PyArena *);
mod_ty _PyPegen_make_module(Parser *, asdl_stmt_seq *);
ശൂന്യം *_PyPegen_arguments_parsing_error(Parser *, വ്യഞ്ജകം_ty);
വ്യഞ്ജകം_ty _PyPegen_get_last_comprehension_item(comprehension_ty comprehension);
ശൂന്യം *_PyPegen_nonparen_genexp_in_call(Parser *p, വ്യഞ്ജകം_ty args, asdl_comprehension_seq *comprehensions);
stmt_ty _PyPegen_checked_from_import(Parser *p, asdl_seq *dots, വ്യഞ്ജകം_ty module_name,
                                     asdl_alias_seq *names, വ്യഞ്ജകം_ty lazy_token, പൂർണ്ണസംഖ്യ lineno,
                                     പൂർണ്ണസംഖ്യ col_offset, പൂർണ്ണസംഖ്യ end_lineno, പൂർണ്ണസംഖ്യ end_col_offset,
                                     PyArena *arena);
asdl_stmt_seq* _PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts);
stmt_ty _PyPegen_register_stmt(Parser *p, stmt_ty s);

// Parser API

Parser *_PyPegen_Parser_New(ഘടന tok_state *, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ *, സ്ഥിരം അക്ഷരം*, PyArena *);
ശൂന്യം _PyPegen_Parser_Free(Parser *);
mod_ty _PyPegen_run_parser_from_file_pointer(FILE *, പൂർണ്ണസംഖ്യ, PyObject *, സ്ഥിരം അക്ഷരം *,
                                    സ്ഥിരം അക്ഷരം *, സ്ഥിരം അക്ഷരം *, PyCompilerFlags *, പൂർണ്ണസംഖ്യ *, PyObject **,
                                    PyArena *);
ശൂന്യം *_PyPegen_run_parser(Parser *);
mod_ty _PyPegen_run_parser_from_string(സ്ഥിരം അക്ഷരം *, പൂർണ്ണസംഖ്യ, PyObject *, PyCompilerFlags *, PyArena *, PyObject *, PyObject **);
asdl_stmt_seq *_PyPegen_interactive_exit(Parser *);

// Generated function in parse.c - function definition in python.gram
ശൂന്യം *_PyPegen_parse(Parser *);

#endif
