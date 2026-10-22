#ifndef PEGEN_H
#define PEGEN_H

#ಅನ್ತರ್ಭಾವ <Python.h>
#ಅನ್ತರ್ಭಾವ <pycore_ast.h>
#ಅನ್ತರ್ಭಾವ <pycore_token.h>
#ಅನ್ತರ್ಭಾವ <pycore_ceval.h>     // _Py_ReachedRecursionLimitWithMargin()
#ಅನ್ತರ್ಭಾವ <pycore_pystate.h>   // _Py_get_machine_stack_pointer()
#ಅನ್ತರ್ಭಾವ <pycore_pythonrun.h> // _PyOS_STACK_MARGIN_BYTES

#ಅನ್ತರ್ಭಾವ "tokenizer/tokenizer.h"

#ಯದಿ 0
#define PyPARSE_YIELD_IS_KEYWORD        0x0001
#endif

#define PyPARSE_DONT_IMPLY_DEDENT       0x0002

#ಯದಿ 0
#define PyPARSE_WITH_IS_KEYWORD         0x0003
#define PyPARSE_PRINT_IS_FUNCTION       0x0004
#define PyPARSE_UNICODE_LITERALS        0x0008
#endif

#define PyPARSE_IGNORE_COOKIE 0x0010
#define PyPARSE_BARRY_AS_BDFL 0x0020
#define PyPARSE_TYPE_COMMENTS 0x0040
#define Py_PARSE_ALLOW_INCOMPLETE_ಒಳಹರಿವು 0x0100

#define CURRENT_POS (-5)

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ _memo {
    ಪೂರ್ಣಾಙ್ಕ type;
    ಶೂನ್ಯ *ಗಂಟು;
    ಪೂರ್ಣಾಙ್ಕ mark;
    ಸಂರಚನಾ _memo *next;
} Memo;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಪೂರ್ಣಾಙ್ಕ type;
    // Text required by grammar actions or helpers; otherwise NULL.
    // An unfinished string token also has NULL text. Its grammar action
    // requests more input only if the token is valid at that position.
    PyObject *bytes;
    ಪೂರ್ಣಾಙ್ಕ level;
    ಪೂರ್ಣಾಙ್ಕ is_raw;
    ಪೂರ್ಣಾಙ್ಕ lineno, col_offset, end_lineno, end_col_offset;
    Memo *memo;
    // Filter over the rule types present in `memo` (bit `type & 63` is set
    // for every entry): lets lookups skip walking the list on definite
    // misses, which are the common case.
    uint64_t memo_mask;
    PyObject *metadata;
} Token;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಧ್ರುವ ವರ್ಣ *str;
    ಪೂರ್ಣಾಙ್ಕ type;
} KeywordToken;


ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಸಂರಚನಾ {
        ಪೂರ್ಣಾಙ್ಕ lineno;
        ವರ್ಣ *comment;  // The " <tag>" in "# type: ignore <tag>"
    } *ವಸ್ತುಗಳು;
    size_t size;
    size_t ಅಂಕೆ_ವಸ್ತುಗಳು;
} growable_comment_array;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಪೂರ್ಣಾಙ್ಕ lineno;
    ಪೂರ್ಣಾಙ್ಕ col_offset;
    ಪೂರ್ಣಾಙ್ಕ end_lineno;
    ಪೂರ್ಣಾಙ್ಕ end_col_offset;
} location;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ _identifier_cache_entry IdentifierCacheEntry;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಸಂರಚನಾ tok_state *tok;
    Token **tokens;
    ಪೂರ್ಣಾಙ್ಕ mark;
    ಪೂರ್ಣಾಙ್ಕ fill, size;
    PyArena *arena;
    KeywordToken **keywords;
    ವರ್ಣ **soft_keywords;
    ಪೂರ್ಣಾಙ್ಕ n_keyword_lists;
    ಪೂರ್ಣಾಙ್ಕ start_rule;
    ಪೂರ್ಣಾಙ್ಕ *errcode;
    unsigned ವರ್ಣ parsing_started;
    unsigned ವರ್ಣ ಒಳಹರಿವು_exhausted;
    PyObject* normalize;
    ಪೂರ್ಣಾಙ್ಕ starting_lineno;
    ಪೂರ್ಣಾಙ್ಕ starting_col_offset;
    ಪೂರ್ಣಾಙ್ಕ error_indicator;
    ಪೂರ್ಣಾಙ್ಕ flags;
    ಪೂರ್ಣಾಙ್ಕ feature_version;
    growable_comment_array type_ignore_comments;
    Token *known_err_token;
    ಪೂರ್ಣಾಙ್ಕ level;
    ಪೂರ್ಣಾಙ್ಕ call_invalid_rules;
    ಪೂರ್ಣಾಙ್ಕ debug;
    location last_stmt_location;
    IdentifierCacheEntry *identifier_cache;
    PyThreadState *tstate;
    // Cached copy of tstate->c_stack_soft_limit offset by one margin toward
    // the stack top, so the per-rule stack check is a single inline comparison.
    uintptr_t ರಾಶಿ_soft_limit;
} Parser;

// C-stack exhaustion check run on every rule entry. The fast path compares
// the machine stack pointer against the soft limit cached in the Parser;
// only when the stack is nearly exhausted is the full check called.
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ
Py_Pegen_ರಾಶಿ_exhausted(Parser *p)
{
#ಯದಿ Py_ರಾಶಿ_GROWS_DOWN
    ಯದಿ (Py_get_machine_ರಾಶಿ_pointer() > p->ರಾಶಿ_soft_limit) {
#ಅನ್ಯಥಾ
    ಯದಿ (Py_get_machine_ರಾಶಿ_pointer() <= p->ರಾಶಿ_soft_limit) {
#endif
        ಪ್ರತಿಜ್ಞಾ(!_Py_ReachedRecursionLimitWithMargin(p->tstate, 1));
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತ್ಯಾಗಮ _Py_ReachedRecursionLimitWithMargin(p->tstate, 1);
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    cmpop_ty cmpop;
    ಅಭಿವ್ಯಕ್ತಿ_ty ಅಭಿವ್ಯಕ್ತಿ;
} Cmpop_ಅಭಿವ್ಯಕ್ತಿ_Pair;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಅಭಿವ್ಯಕ್ತಿ_ty ಕೀಲಿ;
    ಅಭಿವ್ಯಕ್ತಿ_ty ಮೌಲ್ಯ;
} ಕೀಲಿ_ಮೌಲ್ಯ_Pair;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಅಭಿವ್ಯಕ್ತಿ_ty ಕೀಲಿ;
    pattern_ty pattern;
} ಕೀಲಿ_Pattern_Pair;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    arg_ty arg;
    ಅಭಿವ್ಯಕ್ತಿ_ty ಮೌಲ್ಯ;
} NameDefaultPair;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    asdl_arg_seq *plain_names;
    asdl_seq *names_with_defaults; // asdl_seq* of NameDefaultsPair's
} SlashWithDefault;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    arg_ty vararg;
    asdl_seq *kwonlyargs; // asdl_seq* of NameDefaultsPair's
    arg_ty kwarg;
} StarEtc;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ { operator_ty kind; } AugOperator;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಶೂನ್ಯ *element;
    ಪೂರ್ಣಾಙ್ಕ is_keyword;
} KeywordOrStarred;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಶೂನ್ಯ *result;
    PyObject *metadata;
} ResultTokenWithMetadata;

// Internal parser functions
#ಯದಿ defined(Py_DEBUG)
ಶೂನ್ಯ _PyPegen_clear_memo_statistics(ಶೂನ್ಯ);
PyObject *_PyPegen_get_memo_statistics(ಶೂನ್ಯ);
#endif

ಪೂರ್ಣಾಙ್ಕ Py_Pegen_ಸೇರಿಸು_memo(Parser *p, ಪೂರ್ಣಾಙ್ಕ mark, ಪೂರ್ಣಾಙ್ಕ type, ಶೂನ್ಯ *ಗಂಟು);
ಪೂರ್ಣಾಙ್ಕ _PyPegen_update_memo(Parser *p, ಪೂರ್ಣಾಙ್ಕ mark, ಪೂರ್ಣಾಙ್ಕ type, ಶೂನ್ಯ *ಗಂಟು);
Memo *Py_Pegen_ಸೇರಿಸು_memo_direct(Parser *p, ಪೂರ್ಣಾಙ್ಕ mark, ಪೂರ್ಣಾಙ್ಕ type);
ಪೂರ್ಣಾಙ್ಕ _PyPegen_is_memoized(Parser *p, ಪೂರ್ಣಾಙ್ಕ type, ಶೂನ್ಯ *pres);

ಪೂರ್ಣಾಙ್ಕ _PyPegen_lookahead(ಪೂರ್ಣಾಙ್ಕ, ಶೂನ್ಯ *(func)(Parser *), Parser *);
ಪೂರ್ಣಾಙ್ಕ Py_Pegen_lookahead_for_ಅಭಿವ್ಯಕ್ತಿ(ಪೂರ್ಣಾಙ್ಕ, ಅಭಿವ್ಯಕ್ತಿ_ty (func)(Parser *), Parser *);
ಪೂರ್ಣಾಙ್ಕ _PyPegen_lookahead_for_stmt(ಪೂರ್ಣಾಙ್ಕ, stmt_ty (func)(Parser *), Parser *);
ಪೂರ್ಣಾಙ್ಕ _PyPegen_lookahead_with_int(ಪೂರ್ಣಾಙ್ಕ, Token *(func)(Parser *, ಪೂರ್ಣಾಙ್ಕ), Parser *, ಪೂರ್ಣಾಙ್ಕ);
ಪೂರ್ಣಾಙ್ಕ _PyPegen_lookahead_with_string(ಪೂರ್ಣಾಙ್ಕ, ಅಭಿವ್ಯಕ್ತಿ_ty (func)(Parser *, ಧ್ರುವ ವರ್ಣ*), Parser *, ಧ್ರುವ ವರ್ಣ*);

Token *_PyPegen_expect_token(Parser *p, ಪೂರ್ಣಾಙ್ಕ type);
ಶೂನ್ಯ* _PyPegen_expect_forced_result(Parser *p, ಶೂನ್ಯ* result, ಧ್ರುವ ವರ್ಣ* expected);
Token *_PyPegen_expect_forced_token(Parser *p, ಪೂರ್ಣಾಙ್ಕ type, ಧ್ರುವ ವರ್ಣ* expected);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_expect_soft_keyword(Parser *p, ಧ್ರುವ ವರ್ಣ *keyword);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_soft_keyword_token(Parser *p);
ಅಭಿವ್ಯಕ್ತಿ_ty Py_Pegen_fstring_ಮಧ್ಯ_token(Parser* p);
Token *_PyPegen_get_last_nonnwhitespace_token(Parser *);
ಪೂರ್ಣಾಙ್ಕ _PyPegen_fill_token(Parser *p);
// Return -1 on failure, leaving the buffer and capacity unchanged.
ಪೂರ್ಣಾಙ್ಕ _PyPegen_grow_loop_buffer(ಶೂನ್ಯ ***buffer, Py_ssize_t *capacity);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_name_token(Parser *p);
ಅಭಿವ್ಯಕ್ತಿ_ty Py_Pegen_ಸಂಖ್ಯೆ_token(Parser *p);
ಶೂನ್ಯ *_PyPegen_string_token(Parser *p);
Py_ssize_t Py_Pegen_byte_offset_to_character_offset_ಸಾಲು(PyObject *ಸಾಲು, Py_ssize_t col_offset, Py_ssize_t end_col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset(PyObject *ಸಾಲು, Py_ssize_t col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset_raw(ಧ್ರುವ ವರ್ಣ*, Py_ssize_t col_offset);

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
    STAR_TARGETS,
    DEL_TARGETS,
    FOR_TARGETS,
    SINGLE_TARGETS,
    ATTRIBUTE_OR_SUBSCRIPT_TARGETS
} TARGETS_TYPE;

// Error handling functions and APIs
ಪೂರ್ಣಾಙ್ಕ _Pypegen_raise_decode_error(Parser *p);
ಪೂರ್ಣಾಙ್ಕ _Pypegen_tokenizer_error(Parser *p);
ಶೂನ್ಯ *Py_Pegen_raise_incomplete_ಒಳಹರಿವು(Parser *p);
ಶೂನ್ಯ *_PyPegen_raise_error(Parser *p, PyObject *errtype, ಪೂರ್ಣಾಙ್ಕ use_mark, ಧ್ರುವ ವರ್ಣ *errmsg, ...);
ಶೂನ್ಯ *_PyPegen_raise_error_known_location(Parser *p, PyObject *errtype,
                                          Py_ssize_t lineno, Py_ssize_t col_offset,
                                          Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                                          ಧ್ರುವ ವರ್ಣ *errmsg, va_list va);
ಶೂನ್ಯ _Pypegen_set_syntax_error(Parser* p, Token* last_token);
ಶೂನ್ಯ Pypegen_ರಾಶಿ_overflow(Parser *p);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *
RAISE_ERROR_KNOWN_LOCATION(Parser *p, PyObject *errtype,
                           Py_ssize_t lineno, Py_ssize_t col_offset,
                           Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                           ಧ್ರುವ ವರ್ಣ *errmsg, ...)
{
    va_list va;
    va_start(va, errmsg);
    Py_ssize_t _col_offset = (col_offset == CURRENT_POS ? CURRENT_POS : col_offset + 1);
    Py_ssize_t _end_col_offset = (end_col_offset == CURRENT_POS ? CURRENT_POS : end_col_offset + 1);
    _PyPegen_raise_error_known_location(p, errtype, lineno, _col_offset, end_lineno, _end_col_offset, errmsg, va);
    va_end(va);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#define RAISE_SYNTAX_ERROR(ಸಂದೇಶ, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 0, ಸಂದೇಶ, ##__VA_ARGS__)
#define RAISE_INDENTATION_ERROR(ಸಂದೇಶ, ...) _PyPegen_raise_error(p, PyExc_IndentationError, 0, ಸಂದೇಶ, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_ON_NEXT_TOKEN(ಸಂದೇಶ, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 1, ಸಂದೇಶ, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_RANGE(a, b, ಸಂದೇಶ, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (b)->end_lineno, (b)->end_col_offset, ಸಂದೇಶ, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_LOCATION(a, ಸಂದೇಶ, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (a)->end_lineno, (a)->end_col_offset, ಸಂದೇಶ, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_STARTING_FROM(a, ಸಂದೇಶ, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, CURRENT_POS, CURRENT_POS, ಸಂದೇಶ, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_INVALID_TARGET(type, e) _RAISE_SYNTAX_ERROR_INVALID_TARGET(p, type, e)

Py_LOCAL_INLINE(ಪೂರ್ಣಾಙ್ಕ)
_PyPegen_tokens_are_adjacent(Token *a, Token *b)
{
    ಪ್ರತ್ಯಾಗಮ (a->end_lineno == b->lineno) && (a->end_col_offset == b->col_offset);
}

Py_LOCAL_INLINE(ಶೂನ್ಯ *)
CHECK_CALL(Parser *p, ಶೂನ್ಯ *result)
{
    ಯದಿ (result == ರಿಕ್ತ) {
        ಪ್ರತಿಜ್ಞಾ(PyErr_Occurred());
        p->error_indicator = 1;
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

/* This is needed for helper functions that are allowed to
   return NULL without an error. Example: _PyPegen_seq_extract_starred_exprs */
Py_LOCAL_INLINE(ಶೂನ್ಯ *)
CHECK_CALL_NULL_ALLOWED(Parser *p, ಶೂನ್ಯ *result)
{
    ಯದಿ (result == ರಿಕ್ತ && PyErr_Occurred()) {
        p->error_indicator = 1;
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

#define CHECK(type, result) ((type) CHECK_CALL(p, result))
#define CHECK_NULL_ALLOWED(type, result) ((type) CHECK_CALL_NULL_ALLOWED(p, result))

ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_get_invalid_target(ಅಭಿವ್ಯಕ್ತಿ_ty e, TARGETS_TYPE targets_type);
ಧ್ರುವ ವರ್ಣ *Py_Pegen_get_ಅಭಿವ್ಯಕ್ತಿ_name(ಅಭಿವ್ಯಕ್ತಿ_ty);
Py_LOCAL_INLINE(ಶೂನ್ಯ *)
_RAISE_SYNTAX_ERROR_INVALID_TARGET(Parser *p, TARGETS_TYPE type, ಶೂನ್ಯ *e)
{
    ಅಭಿವ್ಯಕ್ತಿ_ty invalid_target = CHECK_NULL_ALLOWED(ಅಭಿವ್ಯಕ್ತಿ_ty, _PyPegen_get_invalid_target(e, type));
    ಯದಿ (invalid_target != ರಿಕ್ತ) {
        ಧ್ರುವ ವರ್ಣ *ಸಂದೇಶ;
        ಯದಿ (type == STAR_TARGETS || type == FOR_TARGETS) {
            ಸಂದೇಶ = "cannot assign to %s";
        }
        ಅನ್ಯಥಾ {
            ಸಂದೇಶ = "cannot delete %s";
        }
        ಪ್ರತ್ಯಾಗಮ RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
            invalid_target,
            ಸಂದೇಶ,
            Py_Pegen_get_ಅಭಿವ್ಯಕ್ತಿ_name(invalid_target)
        );
        ಪ್ರತ್ಯಾಗಮ RAISE_SYNTAX_ERROR_KNOWN_LOCATION(invalid_target, "invalid syntax");
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

// Action utility functions

ಶೂನ್ಯ *_PyPegen_dummy_name(Parser *p, ...);
ಶೂನ್ಯ * _PyPegen_seq_last_item(asdl_seq *seq);
#define PyPegen_last_item(seq, type) ((type)_PyPegen_seq_last_item((asdl_seq*)seq))
ಶೂನ್ಯ * _PyPegen_seq_first_item(asdl_seq *seq);
#define PyPegen_first_item(seq, type) ((type)_PyPegen_seq_first_item((asdl_seq*)seq))
#define UNUSED(ಅಭಿವ್ಯಕ್ತಿ) ಕುರು { (ಶೂನ್ಯ)(ಅಭಿವ್ಯಕ್ತಿ); } ಯಾವತ್ (0)
#define EXTRA_ಅಭಿವ್ಯಕ್ತಿ(head, tail) head->lineno, (head)->col_offset, (tail)->end_lineno, (tail)->end_col_offset, p->arena
#define EXTRA _start_lineno, _start_col_offset, _end_lineno, _end_col_offset, p->arena
PyObject *_PyPegen_new_type_comment(Parser *, ಧ್ರುವ ವರ್ಣ *);

Py_LOCAL_INLINE(PyObject *)
NEW_TYPE_COMMENT(Parser *p, Token *tc)
{
    ಯದಿ (tc == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಧ್ರುವ ವರ್ಣ *bytes = PyBytes_AsString(tc->bytes);
    ಯದಿ (bytes == ರಿಕ್ತ) {
        ಗಚ್ಛ error;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    ಯದಿ (tco == ರಿಕ್ತ) {
        ಗಚ್ಛ error;
    }
    ಪ್ರತ್ಯಾಗಮ tco;
 error:
    p->error_indicator = 1;  // Inline CHECK_CALL
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

Py_LOCAL_INLINE(ಶೂನ್ಯ *)
INVALID_VERSION_CHECK(Parser *p, ಪೂರ್ಣಾಙ್ಕ version, ವರ್ಣ *ಸಂದೇಶ, ಶೂನ್ಯ *ಗಂಟು)
{
    ಯದಿ (ಗಂಟು == ರಿಕ್ತ) {
        p->error_indicator = 1;  // Inline CHECK_CALL
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (p->feature_version < version) {
        p->error_indicator = 1;
        ಪ್ರತ್ಯಾಗಮ RAISE_SYNTAX_ERROR("%s only supported in Python 3.%i and greater",
                                  ಸಂದೇಶ, version);
    }
    ಪ್ರತ್ಯಾಗಮ ಗಂಟು;
}

#define CHECK_VERSION(type, version, ಸಂದೇಶ, ಗಂಟು) ((type) INVALID_VERSION_CHECK(p, version, ಸಂದೇಶ, ಗಂಟು))

arg_ty _PyPegen_add_type_comment_to_arg(Parser *, arg_ty, Token *);
PyObject *_PyPegen_new_identifier(Parser *, ಧ್ರುವ ವರ್ಣ *);
asdl_seq *_PyPegen_singleton_seq(Parser *, ಶೂನ್ಯ *);
asdl_seq *Py_Pegen_seq_ಸೇರಿಸು_in_front(Parser *, ಶೂನ್ಯ *, asdl_seq *);
asdl_seq *_PyPegen_seq_append_to_end(Parser *, asdl_seq *, ಶೂನ್ಯ *);
asdl_seq *_PyPegen_seq_flatten(Parser *, asdl_seq *);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_join_names_with_dot(Parser *, ಅಭಿವ್ಯಕ್ತಿ_ty, ಅಭಿವ್ಯಕ್ತಿ_ty);
ಪೂರ್ಣಾಙ್ಕ Py_Pegen_seq_ಎಣಿಕೆ_dots(asdl_seq *);
alias_ty _PyPegen_alias_for_star(Parser *, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, PyArena *);
asdl_identifier_seq *_PyPegen_map_names_to_ids(Parser *, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *);
Cmpop_ಅಭಿವ್ಯಕ್ತಿ_Pair *Py_Pegen_cmpop_ಅಭಿವ್ಯಕ್ತಿ_pair(Parser *, cmpop_ty, ಅಭಿವ್ಯಕ್ತಿ_ty);
asdl_int_seq *_PyPegen_get_cmpops(Parser *p, asdl_seq *);
asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *_PyPegen_get_exprs(Parser *, asdl_seq *);
ಅಭಿವ್ಯಕ್ತಿ_ty Py_Pegen_set_ಅಭಿವ್ಯಕ್ತಿ_context(Parser *, ಅಭಿವ್ಯಕ್ತಿ_ty, ಅಭಿವ್ಯಕ್ತಿ_context_ty);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_make_target(Parser *, ಅಭಿವ್ಯಕ್ತಿ_ty, TARGETS_TYPE);
ಕೀಲಿ_ಮೌಲ್ಯ_Pair *Py_Pegen_ಕೀಲಿ_ಮೌಲ್ಯ_pair(Parser *, ಅಭಿವ್ಯಕ್ತಿ_ty, ಅಭಿವ್ಯಕ್ತಿ_ty);
asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *Py_Pegen_get_ಕೀಲಿಗಳು(Parser *, asdl_seq *);
asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *Py_Pegen_get_ಮೌಲ್ಯಗಳು(Parser *, asdl_seq *);
ಕೀಲಿ_Pattern_Pair *Py_Pegen_ಕೀಲಿ_pattern_pair(Parser *, ಅಭಿವ್ಯಕ್ತಿ_ty, pattern_ty);
asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *Py_Pegen_get_pattern_ಕೀಲಿಗಳು(Parser *, asdl_seq *);
asdl_pattern_seq *_PyPegen_get_patterns(Parser *, asdl_seq *);
NameDefaultPair *_PyPegen_name_default_pair(Parser *, arg_ty, ಅಭಿವ್ಯಕ್ತಿ_ty, Token *);
SlashWithDefault *_PyPegen_slash_with_default(Parser *, asdl_arg_seq *, asdl_seq *);
StarEtc *_PyPegen_star_etc(Parser *, arg_ty, asdl_seq *, arg_ty);
arguments_ty _PyPegen_make_arguments(Parser *, asdl_arg_seq *, SlashWithDefault *,
                                     asdl_arg_seq *, asdl_seq *, StarEtc *);
arguments_ty _PyPegen_empty_arguments(Parser *);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_template_str(Parser *p, Token *a, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *raw_expressions, Token *b);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_joined_str(Parser *p, Token *a, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *raw_expressions, Token *b);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_interpolation(Parser *, ಅಭಿವ್ಯಕ್ತಿ_ty, Token *, ResultTokenWithMetadata *, ResultTokenWithMetadata *, Token *,
                                 ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, PyArena *);
ಅಭಿವ್ಯಕ್ತಿ_ty Py_Pegen_formatted_ಮೌಲ್ಯ(Parser *, ಅಭಿವ್ಯಕ್ತಿ_ty, Token *, ResultTokenWithMetadata *, ResultTokenWithMetadata *, Token *,
                                 ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, PyArena *);
AugOperator *_PyPegen_augoperator(Parser*, operator_ty type);
stmt_ty _PyPegen_function_def_decorators(Parser *, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *, stmt_ty);
stmt_ty _PyPegen_class_def_decorators(Parser *, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *, stmt_ty);
KeywordOrStarred *_PyPegen_keyword_or_starred(Parser *, ಶೂನ್ಯ *, ಪೂರ್ಣಾಙ್ಕ);
asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *_PyPegen_seq_extract_starred_exprs(Parser *, asdl_seq *);
asdl_keyword_seq *_PyPegen_seq_delete_starred_exprs(Parser *, asdl_seq *);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_collect_call_seqs(Parser *, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *, asdl_seq *,
                     ಪೂರ್ಣಾಙ್ಕ lineno, ಪೂರ್ಣಾಙ್ಕ col_offset, ಪೂರ್ಣಾಙ್ಕ end_lineno,
                     ಪೂರ್ಣಾಙ್ಕ end_col_offset, PyArena *arena);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_constant_from_token(Parser* p, Token* tok);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_constant_from_string(Parser* p, Token* tok);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_concatenate_tstrings(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, PyArena *);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_concatenate_strings(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, PyArena *);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_FetchRawForm(Parser *p, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_ensure_imaginary(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_ensure_real(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty);
asdl_seq *_PyPegen_join_sequences(Parser *, asdl_seq *, asdl_seq *);
ಪೂರ್ಣಾಙ್ಕ _PyPegen_check_barry_as_flufl(Parser *, Token *);
ಪೂರ್ಣಾಙ್ಕ _PyPegen_check_legacy_stmt(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty t);
ಶೂನ್ಯ *_PyPegen_raise_error_for_missing_comma(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty a, ಅಭಿವ್ಯಕ್ತಿ_ty b);
ResultTokenWithMetadata *_PyPegen_check_fstring_conversion(Parser *p, Token *, ಅಭಿವ್ಯಕ್ತಿ_ty t);
ResultTokenWithMetadata *_PyPegen_setup_full_format_spec(Parser *, Token *, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ,
                                                         ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, PyArena *);
mod_ty _PyPegen_make_module(Parser *, asdl_stmt_seq *);
ಶೂನ್ಯ *_PyPegen_arguments_parsing_error(Parser *, ಅಭಿವ್ಯಕ್ತಿ_ty);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_get_last_comprehension_item(comprehension_ty comprehension);
ಶೂನ್ಯ *_PyPegen_nonparen_genexp_in_call(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty args, asdl_comprehension_seq *comprehensions);
stmt_ty _PyPegen_checked_from_import(Parser *p, asdl_seq *dots, ಅಭಿವ್ಯಕ್ತಿ_ty module_name,
                                     asdl_alias_seq *names, ಅಭಿವ್ಯಕ್ತಿ_ty lazy_token, ಪೂರ್ಣಾಙ್ಕ lineno,
                                     ಪೂರ್ಣಾಙ್ಕ col_offset, ಪೂರ್ಣಾಙ್ಕ end_lineno, ಪೂರ್ಣಾಙ್ಕ end_col_offset,
                                     PyArena *arena);
asdl_stmt_seq* _PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts);
stmt_ty _PyPegen_register_stmt(Parser *p, stmt_ty s);

// Parser API

Parser *_PyPegen_Parser_New(ಸಂರಚನಾ tok_state *, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ *, ಧ್ರುವ ವರ್ಣ*, PyArena *);
ಶೂನ್ಯ _PyPegen_Parser_Free(Parser *);
mod_ty _PyPegen_run_parser_from_file_pointer(FILE *, ಪೂರ್ಣಾಙ್ಕ, PyObject *, ಧ್ರುವ ವರ್ಣ *,
                                    ಧ್ರುವ ವರ್ಣ *, ಧ್ರುವ ವರ್ಣ *, PyCompilerFlags *, ಪೂರ್ಣಾಙ್ಕ *, PyObject **,
                                    PyArena *);
ಶೂನ್ಯ *_PyPegen_run_parser(Parser *);
mod_ty _PyPegen_run_parser_from_string(ಧ್ರುವ ವರ್ಣ *, ಪೂರ್ಣಾಙ್ಕ, PyObject *, PyCompilerFlags *, PyArena *, PyObject *, PyObject **);
asdl_stmt_seq *_PyPegen_interactive_exit(Parser *);

// Generated function in parse.c - function definition in python.gram
ಶೂನ್ಯ *_PyPegen_parse(Parser *);

#endif
