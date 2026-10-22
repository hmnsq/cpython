#ifndef PEGEN_H
#define PEGEN_H

#ଅନ୍ତର୍ଭାଵ <Python.h>
#ଅନ୍ତର୍ଭାଵ <pycore_ast.h>
#ଅନ୍ତର୍ଭାଵ <pycore_token.h>
#ଅନ୍ତର୍ଭାଵ <pycore_ceval.h>     // _Py_ReachedRecursionLimitWithMargin()
#ଅନ୍ତର୍ଭାଵ <pycore_pystate.h>   // _Py_get_machine_stack_pointer()
#ଅନ୍ତର୍ଭାଵ <pycore_pythonrun.h> // _PyOS_STACK_MARGIN_BYTES

#ଅନ୍ତର୍ଭାଵ "tokenizer/tokenizer.h"

#ଯଦି 0
#define PyPARSE_YIELD_IS_KEYWORD        0x0001
#endif

#define PyPARSE_DONT_IMPLY_DEDENT       0x0002

#ଯଦି 0
#define PyPARSE_WITH_IS_KEYWORD         0x0003
#define PyPARSE_PRINT_IS_FUNCTION       0x0004
#define PyPARSE_UNICODE_LITERALS        0x0008
#endif

#define PyPARSE_IGNORE_COOKIE 0x0010
#define PyPARSE_BARRY_AS_BDFL 0x0020
#define PyPARSE_TYPE_COMMENTS 0x0040
#define Py_PARSE_ALLOW_INCOMPLETE_ନିବେଶ 0x0100

#define CURRENT_POS (-5)

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା _memo {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା type;
    ଶୂନ୍ୟ *ଗଣ୍ଠି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା mark;
    ସଂରଚନା _memo *next;
} Memo;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା type;
    // Text required by grammar actions or helpers; otherwise NULL.
    // An unfinished string token also has NULL text. Its grammar action
    // requests more input only if the token is valid at that position.
    PyObject *bytes;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା level;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_raw;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno, col_offset, end_lineno, end_col_offset;
    Memo *memo;
    // Filter over the rule types present in `memo` (bit `type & 63` is set
    // for every entry): lets lookups skip walking the list on definite
    // misses, which are the common case.
    uint64_t memo_mask;
    PyObject *metadata;
} Token;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ଅଚଳ ଅକ୍ଷର *str;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା type;
} KeywordToken;


ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ସଂରଚନା {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno;
        ଅକ୍ଷର *comment;  // The " <tag>" in "# type: ignore <tag>"
    } *ବସ୍ତୁଗୁଡ଼ିକ;
    size_t size;
    size_t ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ;
} growable_comment_array;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_offset;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_lineno;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col_offset;
} location;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା _identifier_cache_entry IdentifierCacheEntry;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ସଂରଚନା tok_state *tok;
    Token **tokens;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା mark;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fill, size;
    PyArena *arena;
    KeywordToken **keywords;
    ଅକ୍ଷର **soft_keywords;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n_keyword_lists;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା start_rule;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *errcode;
    unsigned ଅକ୍ଷର parsing_started;
    unsigned ଅକ୍ଷର ନିବେଶ_exhausted;
    PyObject* normalize;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା starting_lineno;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା starting_col_offset;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା error_indicator;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା feature_version;
    growable_comment_array type_ignore_comments;
    Token *known_err_token;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା level;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା call_invalid_rules;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା debug;
    location last_stmt_location;
    IdentifierCacheEntry *identifier_cache;
    PyThreadState *tstate;
    // Cached copy of tstate->c_stack_soft_limit offset by one margin toward
    // the stack top, so the per-rule stack check is a single inline comparison.
    uintptr_t ଗଦା_soft_limit;
} Parser;

// C-stack exhaustion check run on every rule entry. The fast path compares
// the machine stack pointer against the soft limit cached in the Parser;
// only when the stack is nearly exhausted is the full check called.
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା
Py_Pegen_ଗଦା_exhausted(Parser *p)
{
#ଯଦି Py_ଗଦା_GROWS_DOWN
    ଯଦି (Py_get_machine_ଗଦା_pointer() > p->ଗଦା_soft_limit) {
#ଅନ୍ୟଥା
    ଯଦି (Py_get_machine_ଗଦା_pointer() <= p->ଗଦା_soft_limit) {
#endif
        ପ୍ରତିଜ୍ଞା(!_Py_ReachedRecursionLimitWithMargin(p->tstate, 1));
        ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ _Py_ReachedRecursionLimitWithMargin(p->tstate, 1);
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    cmpop_ty cmpop;
    ପଦସମଷ୍ଟି_ty ପଦସମଷ୍ଟି;
} Cmpop_ପଦସମଷ୍ଟି_Pair;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ପଦସମଷ୍ଟି_ty ଚାବି;
    ପଦସମଷ୍ଟି_ty ମୂଲ୍ୟ;
} ଚାବି_ମୂଲ୍ୟ_Pair;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ପଦସମଷ୍ଟି_ty ଚାବି;
    pattern_ty pattern;
} ଚାବି_Pattern_Pair;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    arg_ty arg;
    ପଦସମଷ୍ଟି_ty ମୂଲ୍ୟ;
} NameDefaultPair;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    asdl_arg_seq *plain_names;
    asdl_seq *names_with_defaults; // asdl_seq* of NameDefaultsPair's
} SlashWithDefault;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    arg_ty vararg;
    asdl_seq *kwonlyargs; // asdl_seq* of NameDefaultsPair's
    arg_ty kwarg;
} StarEtc;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା { operator_ty kind; } AugOperator;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ଶୂନ୍ୟ *element;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_keyword;
} KeywordOrStarred;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ଶୂନ୍ୟ *result;
    PyObject *metadata;
} ResultTokenWithMetadata;

// Internal parser functions
#ଯଦି defined(Py_DEBUG)
ଶୂନ୍ୟ _PyPegen_clear_memo_statistics(ଶୂନ୍ୟ);
PyObject *_PyPegen_get_memo_statistics(ଶୂନ୍ୟ);
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Py_Pegen_ଭର୍ତ୍ତି_memo(Parser *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mark, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, ଶୂନ୍ୟ *ଗଣ୍ଠି);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyPegen_update_memo(Parser *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mark, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, ଶୂନ୍ୟ *ଗଣ୍ଠି);
Memo *Py_Pegen_ଭର୍ତ୍ତି_memo_direct(Parser *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mark, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyPegen_is_memoized(Parser *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, ଶୂନ୍ୟ *pres);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyPegen_lookahead(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଶୂନ୍ୟ *(func)(Parser *), Parser *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Py_Pegen_lookahead_for_ପଦସମଷ୍ଟି(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପଦସମଷ୍ଟି_ty (func)(Parser *), Parser *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyPegen_lookahead_for_stmt(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, stmt_ty (func)(Parser *), Parser *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyPegen_lookahead_with_int(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, Token *(func)(Parser *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା), Parser *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyPegen_lookahead_with_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପଦସମଷ୍ଟି_ty (func)(Parser *, ଅଚଳ ଅକ୍ଷର*), Parser *, ଅଚଳ ଅକ୍ଷର*);

Token *_PyPegen_expect_token(Parser *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type);
ଶୂନ୍ୟ* _PyPegen_expect_forced_result(Parser *p, ଶୂନ୍ୟ* result, ଅଚଳ ଅକ୍ଷର* expected);
Token *_PyPegen_expect_forced_token(Parser *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, ଅଚଳ ଅକ୍ଷର* expected);
ପଦସମଷ୍ଟି_ty _PyPegen_expect_soft_keyword(Parser *p, ଅଚଳ ଅକ୍ଷର *keyword);
ପଦସମଷ୍ଟି_ty _PyPegen_soft_keyword_token(Parser *p);
ପଦସମଷ୍ଟି_ty Py_Pegen_fstring_ମଧ୍ୟ_token(Parser* p);
Token *_PyPegen_get_last_nonnwhitespace_token(Parser *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyPegen_fill_token(Parser *p);
// Return -1 on failure, leaving the buffer and capacity unchanged.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyPegen_grow_loop_buffer(ଶୂନ୍ୟ ***buffer, Py_ssize_t *capacity);
ପଦସମଷ୍ଟି_ty _PyPegen_name_token(Parser *p);
ପଦସମଷ୍ଟି_ty Py_Pegen_ସଂଖ୍ୟା_token(Parser *p);
ଶୂନ୍ୟ *_PyPegen_string_token(Parser *p);
Py_ssize_t Py_Pegen_byte_offset_to_character_offset_ଧାଡ଼ି(PyObject *ଧାଡ଼ି, Py_ssize_t col_offset, Py_ssize_t end_col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset(PyObject *ଧାଡ଼ି, Py_ssize_t col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset_raw(ଅଚଳ ଅକ୍ଷର*, Py_ssize_t col_offset);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
    STAR_TARGETS,
    DEL_TARGETS,
    FOR_TARGETS,
    SINGLE_TARGETS,
    ATTRIBUTE_OR_SUBSCRIPT_TARGETS
} TARGETS_TYPE;

// Error handling functions and APIs
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _Pypegen_raise_decode_error(Parser *p);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _Pypegen_tokenizer_error(Parser *p);
ଶୂନ୍ୟ *Py_Pegen_raise_incomplete_ନିବେଶ(Parser *p);
ଶୂନ୍ୟ *_PyPegen_raise_error(Parser *p, PyObject *errtype, ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_mark, ଅଚଳ ଅକ୍ଷର *errmsg, ...);
ଶୂନ୍ୟ *_PyPegen_raise_error_known_location(Parser *p, PyObject *errtype,
                                          Py_ssize_t lineno, Py_ssize_t col_offset,
                                          Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                                          ଅଚଳ ଅକ୍ଷର *errmsg, va_list va);
ଶୂନ୍ୟ _Pypegen_set_syntax_error(Parser* p, Token* last_token);
ଶୂନ୍ୟ Pypegen_ଗଦା_overflow(Parser *p);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *
RAISE_ERROR_KNOWN_LOCATION(Parser *p, PyObject *errtype,
                           Py_ssize_t lineno, Py_ssize_t col_offset,
                           Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                           ଅଚଳ ଅକ୍ଷର *errmsg, ...)
{
    va_list va;
    va_start(va, errmsg);
    Py_ssize_t _col_offset = (col_offset == CURRENT_POS ? CURRENT_POS : col_offset + 1);
    Py_ssize_t _end_col_offset = (end_col_offset == CURRENT_POS ? CURRENT_POS : end_col_offset + 1);
    _PyPegen_raise_error_known_location(p, errtype, lineno, _col_offset, end_lineno, _end_col_offset, errmsg, va);
    va_end(va);
    ଫେରନ୍ତୁ ଖାଲି;
}
#define RAISE_SYNTAX_ERROR(ସନ୍ଦେଶ, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 0, ସନ୍ଦେଶ, ##__VA_ARGS__)
#define RAISE_INDENTATION_ERROR(ସନ୍ଦେଶ, ...) _PyPegen_raise_error(p, PyExc_IndentationError, 0, ସନ୍ଦେଶ, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_ON_NEXT_TOKEN(ସନ୍ଦେଶ, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 1, ସନ୍ଦେଶ, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_RANGE(a, b, ସନ୍ଦେଶ, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (b)->end_lineno, (b)->end_col_offset, ସନ୍ଦେଶ, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_LOCATION(a, ସନ୍ଦେଶ, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (a)->end_lineno, (a)->end_col_offset, ସନ୍ଦେଶ, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_STARTING_FROM(a, ସନ୍ଦେଶ, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, CURRENT_POS, CURRENT_POS, ସନ୍ଦେଶ, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_INVALID_TARGET(type, e) _RAISE_SYNTAX_ERROR_INVALID_TARGET(p, type, e)

Py_LOCAL_INLINE(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
_PyPegen_tokens_are_adjacent(Token *a, Token *b)
{
    ଫେରନ୍ତୁ (a->end_lineno == b->lineno) && (a->end_col_offset == b->col_offset);
}

Py_LOCAL_INLINE(ଶୂନ୍ୟ *)
CHECK_CALL(Parser *p, ଶୂନ୍ୟ *result)
{
    ଯଦି (result == ଖାଲି) {
        ପ୍ରତିଜ୍ଞା(PyErr_Occurred());
        p->error_indicator = 1;
    }
    ଫେରନ୍ତୁ result;
}

/* This is needed for helper functions that are allowed to
   return NULL without an error. Example: _PyPegen_seq_extract_starred_exprs */
Py_LOCAL_INLINE(ଶୂନ୍ୟ *)
CHECK_CALL_NULL_ALLOWED(Parser *p, ଶୂନ୍ୟ *result)
{
    ଯଦି (result == ଖାଲି && PyErr_Occurred()) {
        p->error_indicator = 1;
    }
    ଫେରନ୍ତୁ result;
}

#define CHECK(type, result) ((type) CHECK_CALL(p, result))
#define CHECK_NULL_ALLOWED(type, result) ((type) CHECK_CALL_NULL_ALLOWED(p, result))

ପଦସମଷ୍ଟି_ty _PyPegen_get_invalid_target(ପଦସମଷ୍ଟି_ty e, TARGETS_TYPE targets_type);
ଅଚଳ ଅକ୍ଷର *Py_Pegen_get_ପଦସମଷ୍ଟି_name(ପଦସମଷ୍ଟି_ty);
Py_LOCAL_INLINE(ଶୂନ୍ୟ *)
_RAISE_SYNTAX_ERROR_INVALID_TARGET(Parser *p, TARGETS_TYPE type, ଶୂନ୍ୟ *e)
{
    ପଦସମଷ୍ଟି_ty invalid_target = CHECK_NULL_ALLOWED(ପଦସମଷ୍ଟି_ty, _PyPegen_get_invalid_target(e, type));
    ଯଦି (invalid_target != ଖାଲି) {
        ଅଚଳ ଅକ୍ଷର *ସନ୍ଦେଶ;
        ଯଦି (type == STAR_TARGETS || type == FOR_TARGETS) {
            ସନ୍ଦେଶ = "cannot assign to %s";
        }
        ଅନ୍ୟଥା {
            ସନ୍ଦେଶ = "cannot delete %s";
        }
        ଫେରନ୍ତୁ RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
            invalid_target,
            ସନ୍ଦେଶ,
            Py_Pegen_get_ପଦସମଷ୍ଟି_name(invalid_target)
        );
        ଫେରନ୍ତୁ RAISE_SYNTAX_ERROR_KNOWN_LOCATION(invalid_target, "invalid syntax");
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

// Action utility functions

ଶୂନ୍ୟ *_PyPegen_dummy_name(Parser *p, ...);
ଶୂନ୍ୟ * _PyPegen_seq_last_item(asdl_seq *seq);
#define PyPegen_last_item(seq, type) ((type)_PyPegen_seq_last_item((asdl_seq*)seq))
ଶୂନ୍ୟ * _PyPegen_seq_first_item(asdl_seq *seq);
#define PyPegen_first_item(seq, type) ((type)_PyPegen_seq_first_item((asdl_seq*)seq))
#define UNUSED(ପଦସମଷ୍ଟି) କର { (ଶୂନ୍ୟ)(ପଦସମଷ୍ଟି); } ଯେତେବେଳେ (0)
#define EXTRA_ପଦସମଷ୍ଟି(head, tail) head->lineno, (head)->col_offset, (tail)->end_lineno, (tail)->end_col_offset, p->arena
#define EXTRA _start_lineno, _start_col_offset, _end_lineno, _end_col_offset, p->arena
PyObject *_PyPegen_new_type_comment(Parser *, ଅଚଳ ଅକ୍ଷର *);

Py_LOCAL_INLINE(PyObject *)
NEW_TYPE_COMMENT(Parser *p, Token *tc)
{
    ଯଦି (tc == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଅଚଳ ଅକ୍ଷର *bytes = PyBytes_AsString(tc->bytes);
    ଯଦି (bytes == ଖାଲି) {
        ଯାଅ error;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    ଯଦି (tco == ଖାଲି) {
        ଯାଅ error;
    }
    ଫେରନ୍ତୁ tco;
 error:
    p->error_indicator = 1;  // Inline CHECK_CALL
    ଫେରନ୍ତୁ ଖାଲି;
}

Py_LOCAL_INLINE(ଶୂନ୍ୟ *)
INVALID_VERSION_CHECK(Parser *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା version, ଅକ୍ଷର *ସନ୍ଦେଶ, ଶୂନ୍ୟ *ଗଣ୍ଠି)
{
    ଯଦି (ଗଣ୍ଠି == ଖାଲି) {
        p->error_indicator = 1;  // Inline CHECK_CALL
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (p->feature_version < version) {
        p->error_indicator = 1;
        ଫେରନ୍ତୁ RAISE_SYNTAX_ERROR("%s only supported in Python 3.%i and greater",
                                  ସନ୍ଦେଶ, version);
    }
    ଫେରନ୍ତୁ ଗଣ୍ଠି;
}

#define CHECK_VERSION(type, version, ସନ୍ଦେଶ, ଗଣ୍ଠି) ((type) INVALID_VERSION_CHECK(p, version, ସନ୍ଦେଶ, ଗଣ୍ଠି))

arg_ty _PyPegen_add_type_comment_to_arg(Parser *, arg_ty, Token *);
PyObject *_PyPegen_new_identifier(Parser *, ଅଚଳ ଅକ୍ଷର *);
asdl_seq *_PyPegen_singleton_seq(Parser *, ଶୂନ୍ୟ *);
asdl_seq *Py_Pegen_seq_ଭର୍ତ୍ତି_in_front(Parser *, ଶୂନ୍ୟ *, asdl_seq *);
asdl_seq *_PyPegen_seq_append_to_end(Parser *, asdl_seq *, ଶୂନ୍ୟ *);
asdl_seq *_PyPegen_seq_flatten(Parser *, asdl_seq *);
ପଦସମଷ୍ଟି_ty _PyPegen_join_names_with_dot(Parser *, ପଦସମଷ୍ଟି_ty, ପଦସମଷ୍ଟି_ty);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Py_Pegen_seq_ଗଣନା_dots(asdl_seq *);
alias_ty _PyPegen_alias_for_star(Parser *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, PyArena *);
asdl_identifier_seq *_PyPegen_map_names_to_ids(Parser *, asdl_ପଦସମଷ୍ଟି_seq *);
Cmpop_ପଦସମଷ୍ଟି_Pair *Py_Pegen_cmpop_ପଦସମଷ୍ଟି_pair(Parser *, cmpop_ty, ପଦସମଷ୍ଟି_ty);
asdl_int_seq *_PyPegen_get_cmpops(Parser *p, asdl_seq *);
asdl_ପଦସମଷ୍ଟି_seq *_PyPegen_get_exprs(Parser *, asdl_seq *);
ପଦସମଷ୍ଟି_ty Py_Pegen_set_ପଦସମଷ୍ଟି_context(Parser *, ପଦସମଷ୍ଟି_ty, ପଦସମଷ୍ଟି_context_ty);
ପଦସମଷ୍ଟି_ty _PyPegen_make_target(Parser *, ପଦସମଷ୍ଟି_ty, TARGETS_TYPE);
ଚାବି_ମୂଲ୍ୟ_Pair *Py_Pegen_ଚାବି_ମୂଲ୍ୟ_pair(Parser *, ପଦସମଷ୍ଟି_ty, ପଦସମଷ୍ଟି_ty);
asdl_ପଦସମଷ୍ଟି_seq *Py_Pegen_get_ଚାବିଗୁଡ଼ିକ(Parser *, asdl_seq *);
asdl_ପଦସମଷ୍ଟି_seq *Py_Pegen_get_ମୂଲ୍ୟଗୁଡ଼ିକ(Parser *, asdl_seq *);
ଚାବି_Pattern_Pair *Py_Pegen_ଚାବି_pattern_pair(Parser *, ପଦସମଷ୍ଟି_ty, pattern_ty);
asdl_ପଦସମଷ୍ଟି_seq *Py_Pegen_get_pattern_ଚାବିଗୁଡ଼ିକ(Parser *, asdl_seq *);
asdl_pattern_seq *_PyPegen_get_patterns(Parser *, asdl_seq *);
NameDefaultPair *_PyPegen_name_default_pair(Parser *, arg_ty, ପଦସମଷ୍ଟି_ty, Token *);
SlashWithDefault *_PyPegen_slash_with_default(Parser *, asdl_arg_seq *, asdl_seq *);
StarEtc *_PyPegen_star_etc(Parser *, arg_ty, asdl_seq *, arg_ty);
arguments_ty _PyPegen_make_arguments(Parser *, asdl_arg_seq *, SlashWithDefault *,
                                     asdl_arg_seq *, asdl_seq *, StarEtc *);
arguments_ty _PyPegen_empty_arguments(Parser *);
ପଦସମଷ୍ଟି_ty _PyPegen_template_str(Parser *p, Token *a, asdl_ପଦସମଷ୍ଟି_seq *raw_expressions, Token *b);
ପଦସମଷ୍ଟି_ty _PyPegen_joined_str(Parser *p, Token *a, asdl_ପଦସମଷ୍ଟି_seq *raw_expressions, Token *b);
ପଦସମଷ୍ଟି_ty _PyPegen_interpolation(Parser *, ପଦସମଷ୍ଟି_ty, Token *, ResultTokenWithMetadata *, ResultTokenWithMetadata *, Token *,
                                 ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, PyArena *);
ପଦସମଷ୍ଟି_ty Py_Pegen_formatted_ମୂଲ୍ୟ(Parser *, ପଦସମଷ୍ଟି_ty, Token *, ResultTokenWithMetadata *, ResultTokenWithMetadata *, Token *,
                                 ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, PyArena *);
AugOperator *_PyPegen_augoperator(Parser*, operator_ty type);
stmt_ty _PyPegen_function_def_decorators(Parser *, asdl_ପଦସମଷ୍ଟି_seq *, stmt_ty);
stmt_ty _PyPegen_class_def_decorators(Parser *, asdl_ପଦସମଷ୍ଟି_seq *, stmt_ty);
KeywordOrStarred *_PyPegen_keyword_or_starred(Parser *, ଶୂନ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
asdl_ପଦସମଷ୍ଟି_seq *_PyPegen_seq_extract_starred_exprs(Parser *, asdl_seq *);
asdl_keyword_seq *_PyPegen_seq_delete_starred_exprs(Parser *, asdl_seq *);
ପଦସମଷ୍ଟି_ty _PyPegen_collect_call_seqs(Parser *, asdl_ପଦସମଷ୍ଟି_seq *, asdl_seq *,
                     ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_lineno,
                     ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col_offset, PyArena *arena);
ପଦସମଷ୍ଟି_ty _PyPegen_constant_from_token(Parser* p, Token* tok);
ପଦସମଷ୍ଟି_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok);
ପଦସମଷ୍ଟି_ty _PyPegen_constant_from_string(Parser* p, Token* tok);
ପଦସମଷ୍ଟି_ty _PyPegen_concatenate_tstrings(Parser *p, asdl_ପଦସମଷ୍ଟି_seq *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, PyArena *);
ପଦସମଷ୍ଟି_ty _PyPegen_concatenate_strings(Parser *p, asdl_ପଦସମଷ୍ଟି_seq *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, PyArena *);
ପଦସମଷ୍ଟି_ty _PyPegen_FetchRawForm(Parser *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପଦସମଷ୍ଟି_ty _PyPegen_ensure_imaginary(Parser *p, ପଦସମଷ୍ଟି_ty);
ପଦସମଷ୍ଟି_ty _PyPegen_ensure_real(Parser *p, ପଦସମଷ୍ଟି_ty);
asdl_seq *_PyPegen_join_sequences(Parser *, asdl_seq *, asdl_seq *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyPegen_check_barry_as_flufl(Parser *, Token *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _PyPegen_check_legacy_stmt(Parser *p, ପଦସମଷ୍ଟି_ty t);
ଶୂନ୍ୟ *_PyPegen_raise_error_for_missing_comma(Parser *p, ପଦସମଷ୍ଟି_ty a, ପଦସମଷ୍ଟି_ty b);
ResultTokenWithMetadata *_PyPegen_check_fstring_conversion(Parser *p, Token *, ପଦସମଷ୍ଟି_ty t);
ResultTokenWithMetadata *_PyPegen_setup_full_format_spec(Parser *, Token *, asdl_ପଦସମଷ୍ଟି_seq *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା,
                                                         ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, PyArena *);
mod_ty _PyPegen_make_module(Parser *, asdl_stmt_seq *);
ଶୂନ୍ୟ *_PyPegen_arguments_parsing_error(Parser *, ପଦସମଷ୍ଟି_ty);
ପଦସମଷ୍ଟି_ty _PyPegen_get_last_comprehension_item(comprehension_ty comprehension);
ଶୂନ୍ୟ *_PyPegen_nonparen_genexp_in_call(Parser *p, ପଦସମଷ୍ଟି_ty args, asdl_comprehension_seq *comprehensions);
stmt_ty _PyPegen_checked_from_import(Parser *p, asdl_seq *dots, ପଦସମଷ୍ଟି_ty module_name,
                                     asdl_alias_seq *names, ପଦସମଷ୍ଟି_ty lazy_token, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno,
                                     ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_lineno, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col_offset,
                                     PyArena *arena);
asdl_stmt_seq* _PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts);
stmt_ty _PyPegen_register_stmt(Parser *p, stmt_ty s);

// Parser API

Parser *_PyPegen_Parser_New(ସଂରଚନା tok_state *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *, ଅଚଳ ଅକ୍ଷର*, PyArena *);
ଶୂନ୍ୟ _PyPegen_Parser_Free(Parser *);
mod_ty _PyPegen_run_parser_from_file_pointer(FILE *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, PyObject *, ଅଚଳ ଅକ୍ଷର *,
                                    ଅଚଳ ଅକ୍ଷର *, ଅଚଳ ଅକ୍ଷର *, PyCompilerFlags *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *, PyObject **,
                                    PyArena *);
ଶୂନ୍ୟ *_PyPegen_run_parser(Parser *);
mod_ty _PyPegen_run_parser_from_string(ଅଚଳ ଅକ୍ଷର *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, PyObject *, PyCompilerFlags *, PyArena *, PyObject *, PyObject **);
asdl_stmt_seq *_PyPegen_interactive_exit(Parser *);

// Generated function in parse.c - function definition in python.gram
ଶୂନ୍ୟ *_PyPegen_parse(Parser *);

#endif
