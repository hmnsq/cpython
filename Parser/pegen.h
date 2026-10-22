#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PEGEN_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PEGEN_H

#ਅਨ੍ਤਰ੍ਭਾਵ <Python.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <pycore_ast.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <pycore_token.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <pycore_ceval.h>     // _Py_ReachedRecursionLimitWithMargin()
#ਅਨ੍ਤਰ੍ਭਾਵ <pycore_pystate.h>   // _Py_get_machine_stack_pointer()
#ਅਨ੍ਤਰ੍ਭਾਵ <pycore_pythonrun.h> // _PyOS_STACK_MARGIN_BYTES

#ਅਨ੍ਤਰ੍ਭਾਵ "tokenizer/tokenizer.h"

#ਜੇ 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PyPARSE_YIELD_IS_KEYWORD        0x0001
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PyPARSE_DONT_IMPLY_DEDENT       0x0002

#ਜੇ 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PyPARSE_WITH_IS_KEYWORD         0x0003
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PyPARSE_PRINT_IS_FUNCTION       0x0004
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PyPARSE_UNICODE_LITERALS        0x0008
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PyPARSE_IGNORE_COOKIE 0x0010
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PyPARSE_BARRY_AS_BDFL 0x0020
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PyPARSE_TYPE_COMMENTS 0x0040
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Py_PARSE_ALLOW_INCOMPLETE_ਨਿਵੇਸ਼ 0x0100

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURRENT_POS (-5)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਯਾਦ_2 {
    ਪੂਰਨ type;
    ਖਾਲੀ *ਗੰਢ;
    ਪੂਰਨ mark;
    ਬਣਤਰ ਯਾਦ_2 *ਅਗਲਾ;
} ਯਾਦ;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ type;
    // Text required by grammar actions or helpers; otherwise NULL.
    // An unfinished string token also has NULL text. Its grammar action
    // requests more input only if the token is valid at that position.
    PyObject *ਬਾਈਟਾਂ;
    ਪੂਰਨ level;
    ਪੂਰਨ is_raw;
    ਪੂਰਨ lineno, ਖਾਨਾ_ਵਿੱਥ, ਸਿਰਾ_lineno, ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ;
    ਯਾਦ *ਯਾਦ_3;
    // Filter over the rule types present in `memo` (bit `type & 63` is set
    // for every entry): lets lookups skip walking the list on definite
    // misses, which are the common case.
    uint64_t ਯਾਦ_ਨਕਾਬ;
    PyObject *metadata;
} Token;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਅਚਲ ਅੱਖਰ *str;
    ਪੂਰਨ type;
} KeywordToken;


ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਬਣਤਰ {
        ਪੂਰਨ lineno;
        ਅੱਖਰ *comment;  // The " <tag>" in "# type: ignore <tag>"
    } *ਚੀਜ਼ਾਂ;
    ਮਾਪ_t ਮਾਪ;
    ਮਾਪ_t ਅੰਕ_ਚੀਜ਼ਾਂ;
} growable_comment_array;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ lineno;
    ਪੂਰਨ ਖਾਨਾ_ਵਿੱਥ;
    ਪੂਰਨ ਸਿਰਾ_lineno;
    ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ;
} location;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ identifier_cache_ਇੰਦਰਾਜ Identifier_Cache_ਇੰਦਰਾਜ;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਬਣਤਰ tok_state *tok;
    Token **ਟੋਕਨ;
    ਪੂਰਨ mark;
    ਪੂਰਨ fill, ਮਾਪ;
    PyArena *arena;
    KeywordToken **keywords;
    ਅੱਖਰ **soft_keywords;
    ਪੂਰਨ n_keyword_lists;
    ਪੂਰਨ ਸ਼ੁਰੂ_ਨਿਯਮ;
    ਪੂਰਨ *errcode;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ parsing_started;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਨਿਵੇਸ਼_exhausted;
    PyObject* normalize;
    ਪੂਰਨ starting_lineno;
    ਪੂਰਨ starting_ਖਾਨਾ_ਵਿੱਥ;
    ਪੂਰਨ error_indicator;
    ਪੂਰਨ flags;
    ਪੂਰਨ feature_ਸੰਸਕਰਣ;
    growable_comment_array type_ignore_comments;
    Token *ਗਿਆਤ_err_token;
    ਪੂਰਨ level;
    ਪੂਰਨ call_invalid_ਨਿਯਮਾਂ;
    ਪੂਰਨ debug;
    location ਆਖਰੀ_stmt_location;
    Identifier_Cache_ਇੰਦਰਾਜ *identifier_cache;
    PyThreadState *tstate;
    // Cached copy of tstate->c_stack_soft_limit offset by one margin toward
    // the stack top, so the per-rule stack check is a single inline comparison.
    uintptr_t ਢੇਰ_soft_ਹੱਦਬੰਦੀ;
} Parser;

// C-stack exhaustion check run on every rule entry. The fast path compares
// the machine stack pointer against the soft limit cached in the Parser;
// only when the stack is nearly exhausted is the full check called.
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ
Py_Pegen_ਢੇਰ_exhausted(Parser *p)
{
#ਜੇ Py_ਢੇਰ_GROWS_ਥੱਲੇ
    ਜੇ (Py_get_machine_ਢੇਰ_ਸੂਚਕ() > p->ਢੇਰ_soft_ਹੱਦਬੰਦੀ) {
#ਨਹੀਂਤਾਂ
    ਜੇ (Py_get_machine_ਢੇਰ_ਸੂਚਕ() <= p->ਢੇਰ_soft_ਹੱਦਬੰਦੀ) {
#ਜੇ_ਅੰਤ
        ਪ੍ਰਤਿਜ੍ਞਾ(!Py_Reached_Recursion_ਹੱਦਬੰਦੀ_With_Margin(p->tstate, 1));
        ਵਾਪਸ 0;
    }
    ਵਾਪਸ Py_Reached_Recursion_ਹੱਦਬੰਦੀ_With_Margin(p->tstate, 1);
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    cmpop_ty cmpop;
    ਵਿਅੰਜਕ_ty ਵਿਅੰਜਕ;
} Cmpop_ਵਿਅੰਜਕ_Pair;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਵਿਅੰਜਕ_ty ਕੁੰਜੀ;
    ਵਿਅੰਜਕ_ty ਮੁੱਲ;
} ਕੁੰਜੀ_ਮੁੱਲ_Pair;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਵਿਅੰਜਕ_ty ਕੁੰਜੀ;
    ਨਮੂਨਾ_ty ਨਮੂਨਾ;
} ਕੁੰਜੀ_ਨਮੂਨਾ_Pair;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    arg_ty arg;
    ਵਿਅੰਜਕ_ty ਮੁੱਲ;
} NameDefaultPair;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    asdl_arg_seq *plain_ਨਾਂ;
    asdl_seq *ਨਾਂ_with_defaults; // asdl_seq* of NameDefaultsPair's
} SlashWithDefault;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    arg_ty vararg;
    asdl_seq *kwonlyargs; // asdl_seq* of NameDefaultsPair's
    arg_ty kwarg;
} ਤਾਰਾ_Etc;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ { operator_ty ਕਿਸਮ; } AugOperator;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਖਾਲੀ *element;
    ਪੂਰਨ is_keyword;
} KeywordOrStarred;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਖਾਲੀ *ਨਤੀਜਾ;
    PyObject *metadata;
} ਨਤੀਜਾ_Token_With_Metadata;

// Internal parser functions
#ਜੇ defined(Py_DEBUG)
ਖਾਲੀ Py_Pegen_clear_ਯਾਦ_statistics(ਖਾਲੀ);
PyObject *Py_Pegen_get_ਯਾਦ_statistics(ਖਾਲੀ);
#ਜੇ_ਅੰਤ

ਪੂਰਨ Py_Pegen_ਪਾਓ_ਯਾਦ(Parser *p, ਪੂਰਨ mark, ਪੂਰਨ type, ਖਾਲੀ *ਗੰਢ);
ਪੂਰਨ Py_Pegen_update_ਯਾਦ(Parser *p, ਪੂਰਨ mark, ਪੂਰਨ type, ਖਾਲੀ *ਗੰਢ);
ਯਾਦ *Py_Pegen_ਪਾਓ_ਯਾਦ_ਸਿੱਧਾ(Parser *p, ਪੂਰਨ mark, ਪੂਰਨ type);
ਪੂਰਨ _PyPegen_is_memoized(Parser *p, ਪੂਰਨ type, ਖਾਲੀ *pres);

ਪੂਰਨ _PyPegen_lookahead(ਪੂਰਨ, ਖਾਲੀ *(func)(Parser *), Parser *);
ਪੂਰਨ Py_Pegen_lookahead_for_ਵਿਅੰਜਕ(ਪੂਰਨ, ਵਿਅੰਜਕ_ty (func)(Parser *), Parser *);
ਪੂਰਨ _PyPegen_lookahead_for_stmt(ਪੂਰਨ, stmt_ty (func)(Parser *), Parser *);
ਪੂਰਨ _PyPegen_lookahead_with_int(ਪੂਰਨ, Token *(func)(Parser *, ਪੂਰਨ), Parser *, ਪੂਰਨ);
ਪੂਰਨ _PyPegen_lookahead_with_string(ਪੂਰਨ, ਵਿਅੰਜਕ_ty (func)(Parser *, ਅਚਲ ਅੱਖਰ*), Parser *, ਅਚਲ ਅੱਖਰ*);

Token *_PyPegen_expect_token(Parser *p, ਪੂਰਨ type);
ਖਾਲੀ* Py_Pegen_expect_forced_ਨਤੀਜਾ(Parser *p, ਖਾਲੀ* ਨਤੀਜਾ, ਅਚਲ ਅੱਖਰ* expected);
Token *_PyPegen_expect_forced_token(Parser *p, ਪੂਰਨ type, ਅਚਲ ਅੱਖਰ* expected);
ਵਿਅੰਜਕ_ty _PyPegen_expect_soft_keyword(Parser *p, ਅਚਲ ਅੱਖਰ *keyword);
ਵਿਅੰਜਕ_ty _PyPegen_soft_keyword_token(Parser *p);
ਵਿਅੰਜਕ_ty Py_Pegen_fstring_ਵਿਚਕਾਰ_token(Parser* p);
Token *Py_Pegen_get_ਆਖਰੀ_nonnwhitespace_token(Parser *);
ਪੂਰਨ _PyPegen_fill_token(Parser *p);
// Return -1 on failure, leaving the buffer and capacity unchanged.
ਪੂਰਨ Py_Pegen_grow_loop_ਬਫਰ(ਖਾਲੀ ***ਬਫਰ, Py_ssize_t *ਸਮਰੱਥਾ);
ਵਿਅੰਜਕ_ty _PyPegen_name_token(Parser *p);
ਵਿਅੰਜਕ_ty Py_Pegen_ਸੰਖਿਆ_token(Parser *p);
ਖਾਲੀ *_PyPegen_string_token(Parser *p);
Py_ssize_t Py_Pegen_byte_ਵਿੱਥ_ਮੰਜ਼ਿਲ_character_ਵਿੱਥ_ਸਤਰ(PyObject *ਸਤਰ, Py_ssize_t ਖਾਨਾ_ਵਿੱਥ, Py_ssize_t ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ);
Py_ssize_t Py_Pegen_byte_ਵਿੱਥ_ਮੰਜ਼ਿਲ_character_ਵਿੱਥ(PyObject *ਸਤਰ, Py_ssize_t ਖਾਨਾ_ਵਿੱਥ);
Py_ssize_t Py_Pegen_byte_ਵਿੱਥ_ਮੰਜ਼ਿਲ_character_ਵਿੱਥ_raw(ਅਚਲ ਅੱਖਰ*, Py_ssize_t ਖਾਨਾ_ਵਿੱਥ);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    ਤਾਰਾ_TARGETS,
    DEL_TARGETS,
    FOR_TARGETS,
    SINGLE_TARGETS,
    ATTRIBUTE_OR_SUBSCRIPT_TARGETS
} TARGETS_TYPE;

// Error handling functions and APIs
ਪੂਰਨ _Pypegen_raise_decode_error(Parser *p);
ਪੂਰਨ _Pypegen_tokenizer_error(Parser *p);
ਖਾਲੀ *Py_Pegen_raise_incomplete_ਨਿਵੇਸ਼(Parser *p);
ਖਾਲੀ *_PyPegen_raise_error(Parser *p, PyObject *errtype, ਪੂਰਨ use_mark, ਅਚਲ ਅੱਖਰ *errmsg, ...);
ਖਾਲੀ *Py_Pegen_raise_error_ਗਿਆਤ_location(Parser *p, PyObject *errtype,
                                          Py_ssize_t lineno, Py_ssize_t ਖਾਨਾ_ਵਿੱਥ,
                                          Py_ssize_t ਸਿਰਾ_lineno, Py_ssize_t ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ,
                                          ਅਚਲ ਅੱਖਰ *errmsg, va_list va);
ਖਾਲੀ Pypegen_ਸੈੱਟ_syntax_error(Parser* p, Token* ਆਖਰੀ_token);
ਖਾਲੀ Pypegen_ਢੇਰ_ਉਛਾਲ(Parser *p);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *
RAISE_ERROR_ਗਿਆਤ_LOCATION(Parser *p, PyObject *errtype,
                           Py_ssize_t lineno, Py_ssize_t ਖਾਨਾ_ਵਿੱਥ,
                           Py_ssize_t ਸਿਰਾ_lineno, Py_ssize_t ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ,
                           ਅਚਲ ਅੱਖਰ *errmsg, ...)
{
    va_list va;
    va_ਸ਼ੁਰੂ(va, errmsg);
    Py_ssize_t ਖਾਨਾ_ਵਿੱਥ_2 = (ਖਾਨਾ_ਵਿੱਥ == CURRENT_POS ? CURRENT_POS : ਖਾਨਾ_ਵਿੱਥ + 1);
    Py_ssize_t ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ_2 = (ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ == CURRENT_POS ? CURRENT_POS : ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ + 1);
    Py_Pegen_raise_error_ਗਿਆਤ_location(p, errtype, lineno, ਖਾਨਾ_ਵਿੱਥ_2, ਸਿਰਾ_lineno, ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ_2, errmsg, va);
    va_ਸਿਰਾ(va);
    ਵਾਪਸ ਨਿਰਾ;
}
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RAISE_SYNTAX_ERROR(ਸੁਨੇਹਾ, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 0, ਸੁਨੇਹਾ, ##__VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RAISE_INDENTATION_ERROR(ਸੁਨੇਹਾ, ...) _PyPegen_raise_error(p, PyExc_IndentationError, 0, ਸੁਨੇਹਾ, ##__VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RAISE_SYNTAX_ERROR_ON_ਅਗਲਾ_TOKEN(ਸੁਨੇਹਾ, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 1, ਸੁਨੇਹਾ, ##__VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RAISE_SYNTAX_ERROR_ਗਿਆਤ_RANGE(a, b, ਸੁਨੇਹਾ, ...) \
    RAISE_ERROR_ਗਿਆਤ_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->ਖਾਨਾ_ਵਿੱਥ, (b)->ਸਿਰਾ_lineno, (b)->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, ਸੁਨੇਹਾ, ##__VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RAISE_SYNTAX_ERROR_ਗਿਆਤ_LOCATION(a, ਸੁਨੇਹਾ, ...) \
    RAISE_ERROR_ਗਿਆਤ_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->ਖਾਨਾ_ਵਿੱਥ, (a)->ਸਿਰਾ_lineno, (a)->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, ਸੁਨੇਹਾ, ##__VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RAISE_SYNTAX_ERROR_STARTING_FROM(a, ਸੁਨੇਹਾ, ...) \
    RAISE_ERROR_ਗਿਆਤ_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->ਖਾਨਾ_ਵਿੱਥ, CURRENT_POS, CURRENT_POS, ਸੁਨੇਹਾ, ##__VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RAISE_SYNTAX_ERROR_INVALID_ਨਿਸ਼ਾਨਾ(type, e) RAISE_SYNTAX_ERROR_INVALID_ਨਿਸ਼ਾਨਾ_2(p, type, e)

Py_LOCAL_INLINE(ਪੂਰਨ)
Py_Pegen_ਟੋਕਨ_are_adjacent(Token *a, Token *b)
{
    ਵਾਪਸ (a->ਸਿਰਾ_lineno == b->lineno) && (a->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ == b->ਖਾਨਾ_ਵਿੱਥ);
}

Py_LOCAL_INLINE(ਖਾਲੀ *)
CHECK_CALL(Parser *p, ਖਾਲੀ *ਨਤੀਜਾ)
{
    ਜੇ (ਨਤੀਜਾ == ਨਿਰਾ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(PyErr_Occurred());
        p->error_indicator = 1;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

/* This is needed for helper functions that are allowed to
   return NULL without an error. Example: _PyPegen_seq_extract_starred_exprs */
Py_LOCAL_INLINE(ਖਾਲੀ *)
CHECK_CALL_NULL_ALLOWED(Parser *p, ਖਾਲੀ *ਨਤੀਜਾ)
{
    ਜੇ (ਨਤੀਜਾ == ਨਿਰਾ && PyErr_Occurred()) {
        p->error_indicator = 1;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CHECK(type, ਨਤੀਜਾ) ((type) CHECK_CALL(p, ਨਤੀਜਾ))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CHECK_NULL_ALLOWED(type, ਨਤੀਜਾ) ((type) CHECK_CALL_NULL_ALLOWED(p, ਨਤੀਜਾ))

ਵਿਅੰਜਕ_ty Py_Pegen_get_invalid_ਨਿਸ਼ਾਨਾ(ਵਿਅੰਜਕ_ty e, TARGETS_TYPE targets_type);
ਅਚਲ ਅੱਖਰ *Py_Pegen_get_ਵਿਅੰਜਕ_name(ਵਿਅੰਜਕ_ty);
Py_LOCAL_INLINE(ਖਾਲੀ *)
RAISE_SYNTAX_ERROR_INVALID_ਨਿਸ਼ਾਨਾ_2(Parser *p, TARGETS_TYPE type, ਖਾਲੀ *e)
{
    ਵਿਅੰਜਕ_ty invalid_ਨਿਸ਼ਾਨਾ = CHECK_NULL_ALLOWED(ਵਿਅੰਜਕ_ty, Py_Pegen_get_invalid_ਨਿਸ਼ਾਨਾ(e, type));
    ਜੇ (invalid_ਨਿਸ਼ਾਨਾ != ਨਿਰਾ) {
        ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ;
        ਜੇ (type == ਤਾਰਾ_TARGETS || type == FOR_TARGETS) {
            ਸੁਨੇਹਾ = "cannot assign to %s";
        }
        ਨਹੀਂਤਾਂ {
            ਸੁਨੇਹਾ = "cannot delete %s";
        }
        ਵਾਪਸ RAISE_SYNTAX_ERROR_ਗਿਆਤ_LOCATION(
            invalid_ਨਿਸ਼ਾਨਾ,
            ਸੁਨੇਹਾ,
            Py_Pegen_get_ਵਿਅੰਜਕ_name(invalid_ਨਿਸ਼ਾਨਾ)
        );
        ਵਾਪਸ RAISE_SYNTAX_ERROR_ਗਿਆਤ_LOCATION(invalid_ਨਿਸ਼ਾਨਾ, "invalid syntax");
    }
    ਵਾਪਸ ਨਿਰਾ;
}

// Action utility functions

ਖਾਲੀ *_PyPegen_dummy_name(Parser *p, ...);
ਖਾਲੀ * Py_Pegen_seq_ਆਖਰੀ_ਵਸਤੂ(asdl_seq *seq);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Py_Pegen_ਆਖਰੀ_ਵਸਤੂ(seq, type) ((type)Py_Pegen_seq_ਆਖਰੀ_ਵਸਤੂ((asdl_seq*)seq))
ਖਾਲੀ * Py_Pegen_seq_ਪਹਿਲਾ_ਵਸਤੂ(asdl_seq *seq);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Py_Pegen_ਪਹਿਲਾ_ਵਸਤੂ(seq, type) ((type)Py_Pegen_seq_ਪਹਿਲਾ_ਵਸਤੂ((asdl_seq*)seq))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅਣਵਰਤਿਆ(ਵਿਅੰਜਕ) ਕਰੋ { (ਖਾਲੀ)(ਵਿਅੰਜਕ); } ਜਦੋਂ (0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXTRA_ਵਿਅੰਜਕ(ਸਿਰ, ਪੂਛ) ਸਿਰ->lineno, (ਸਿਰ)->ਖਾਨਾ_ਵਿੱਥ, (ਪੂਛ)->ਸਿਰਾ_lineno, (ਪੂਛ)->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, p->arena
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXTRA ਸ਼ੁਰੂ_lineno, ਸ਼ੁਰੂ_ਖਾਨਾ_ਵਿੱਥ_2, ਸਿਰਾ_lineno_2, ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ_2, p->arena
PyObject *_PyPegen_new_type_comment(Parser *, ਅਚਲ ਅੱਖਰ *);

Py_LOCAL_INLINE(PyObject *)
NEW_TYPE_COMMENT(Parser *p, Token *tc)
{
    ਜੇ (tc == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਅਚਲ ਅੱਖਰ *ਬਾਈਟਾਂ = Py_ਬਾਈਟਾਂ_As_String(tc->ਬਾਈਟਾਂ);
    ਜੇ (ਬਾਈਟਾਂ == ਨਿਰਾ) {
        ਜਾਓ error;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, ਬਾਈਟਾਂ);
    ਜੇ (tco == ਨਿਰਾ) {
        ਜਾਓ error;
    }
    ਵਾਪਸ tco;
 error:
    p->error_indicator = 1;  // Inline CHECK_CALL
    ਵਾਪਸ ਨਿਰਾ;
}

Py_LOCAL_INLINE(ਖਾਲੀ *)
INVALID_ਸੰਸਕਰਣ_CHECK(Parser *p, ਪੂਰਨ ਸੰਸਕਰਣ, ਅੱਖਰ *ਸੁਨੇਹਾ, ਖਾਲੀ *ਗੰਢ)
{
    ਜੇ (ਗੰਢ == ਨਿਰਾ) {
        p->error_indicator = 1;  // Inline CHECK_CALL
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (p->feature_ਸੰਸਕਰਣ < ਸੰਸਕਰਣ) {
        p->error_indicator = 1;
        ਵਾਪਸ RAISE_SYNTAX_ERROR("%s only supported in Python 3.%i and greater",
                                  ਸੁਨੇਹਾ, ਸੰਸਕਰਣ);
    }
    ਵਾਪਸ ਗੰਢ;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CHECK_ਸੰਸਕਰਣ(type, ਸੰਸਕਰਣ, ਸੁਨੇਹਾ, ਗੰਢ) ((type) INVALID_ਸੰਸਕਰਣ_CHECK(p, ਸੰਸਕਰਣ, ਸੁਨੇਹਾ, ਗੰਢ))

arg_ty Py_Pegen_ਜੋੜੋ_type_comment_ਮੰਜ਼ਿਲ_arg(Parser *, arg_ty, Token *);
PyObject *_PyPegen_new_identifier(Parser *, ਅਚਲ ਅੱਖਰ *);
asdl_seq *_PyPegen_singleton_seq(Parser *, ਖਾਲੀ *);
asdl_seq *Py_Pegen_seq_ਪਾਓ_ਵਿੱਚ_front(Parser *, ਖਾਲੀ *, asdl_seq *);
asdl_seq *Py_Pegen_seq_append_ਮੰਜ਼ਿਲ_ਸਿਰਾ(Parser *, asdl_seq *, ਖਾਲੀ *);
asdl_seq *_PyPegen_seq_flatten(Parser *, asdl_seq *);
ਵਿਅੰਜਕ_ty Py_Pegen_join_ਨਾਂ_with_dot(Parser *, ਵਿਅੰਜਕ_ty, ਵਿਅੰਜਕ_ty);
ਪੂਰਨ Py_Pegen_seq_ਗਿਣਤੀ_dots(asdl_seq *);
alias_ty Py_Pegen_alias_for_ਤਾਰਾ(Parser *, ਪੂਰਨ, ਪੂਰਨ, ਪੂਰਨ, ਪੂਰਨ, PyArena *);
asdl_identifier_seq *Py_Pegen_map_ਨਾਂ_ਮੰਜ਼ਿਲ_ids(Parser *, asdl_ਵਿਅੰਜਕ_seq *);
Cmpop_ਵਿਅੰਜਕ_Pair *Py_Pegen_cmpop_ਵਿਅੰਜਕ_pair(Parser *, cmpop_ty, ਵਿਅੰਜਕ_ty);
asdl_int_seq *_PyPegen_get_cmpops(Parser *p, asdl_seq *);
asdl_ਵਿਅੰਜਕ_seq *_PyPegen_get_exprs(Parser *, asdl_seq *);
ਵਿਅੰਜਕ_ty Py_Pegen_ਸੈੱਟ_ਵਿਅੰਜਕ_context(Parser *, ਵਿਅੰਜਕ_ty, ਵਿਅੰਜਕ_context_ty);
ਵਿਅੰਜਕ_ty Py_Pegen_make_ਨਿਸ਼ਾਨਾ(Parser *, ਵਿਅੰਜਕ_ty, TARGETS_TYPE);
ਕੁੰਜੀ_ਮੁੱਲ_Pair *Py_Pegen_ਕੁੰਜੀ_ਮੁੱਲ_pair(Parser *, ਵਿਅੰਜਕ_ty, ਵਿਅੰਜਕ_ty);
asdl_ਵਿਅੰਜਕ_seq *Py_Pegen_get_ਕੁੰਜੀਆਂ(Parser *, asdl_seq *);
asdl_ਵਿਅੰਜਕ_seq *Py_Pegen_get_ਮੁੱਲਾਂ(Parser *, asdl_seq *);
ਕੁੰਜੀ_ਨਮੂਨਾ_Pair *Py_Pegen_ਕੁੰਜੀ_ਨਮੂਨਾ_pair(Parser *, ਵਿਅੰਜਕ_ty, ਨਮੂਨਾ_ty);
asdl_ਵਿਅੰਜਕ_seq *Py_Pegen_get_ਨਮੂਨਾ_ਕੁੰਜੀਆਂ(Parser *, asdl_seq *);
asdl_ਨਮੂਨਾ_seq *_PyPegen_get_patterns(Parser *, asdl_seq *);
NameDefaultPair *_PyPegen_name_default_pair(Parser *, arg_ty, ਵਿਅੰਜਕ_ty, Token *);
SlashWithDefault *_PyPegen_slash_with_default(Parser *, asdl_arg_seq *, asdl_seq *);
ਤਾਰਾ_Etc *Py_Pegen_ਤਾਰਾ_etc(Parser *, arg_ty, asdl_seq *, arg_ty);
arguments_ty _PyPegen_make_arguments(Parser *, asdl_arg_seq *, SlashWithDefault *,
                                     asdl_arg_seq *, asdl_seq *, ਤਾਰਾ_Etc *);
arguments_ty Py_Pegen_ਖਾਲੀ_ਥਾਵਾਂ_arguments(Parser *);
ਵਿਅੰਜਕ_ty _PyPegen_template_str(Parser *p, Token *a, asdl_ਵਿਅੰਜਕ_seq *raw_expressions, Token *b);
ਵਿਅੰਜਕ_ty _PyPegen_joined_str(Parser *p, Token *a, asdl_ਵਿਅੰਜਕ_seq *raw_expressions, Token *b);
ਵਿਅੰਜਕ_ty _PyPegen_interpolation(Parser *, ਵਿਅੰਜਕ_ty, Token *, ਨਤੀਜਾ_Token_With_Metadata *, ਨਤੀਜਾ_Token_With_Metadata *, Token *,
                                 ਪੂਰਨ, ਪੂਰਨ, ਪੂਰਨ, ਪੂਰਨ, PyArena *);
ਵਿਅੰਜਕ_ty Py_Pegen_formatted_ਮੁੱਲ(Parser *, ਵਿਅੰਜਕ_ty, Token *, ਨਤੀਜਾ_Token_With_Metadata *, ਨਤੀਜਾ_Token_With_Metadata *, Token *,
                                 ਪੂਰਨ, ਪੂਰਨ, ਪੂਰਨ, ਪੂਰਨ, PyArena *);
AugOperator *_PyPegen_augoperator(Parser*, operator_ty type);
stmt_ty _PyPegen_function_def_decorators(Parser *, asdl_ਵਿਅੰਜਕ_seq *, stmt_ty);
stmt_ty _PyPegen_class_def_decorators(Parser *, asdl_ਵਿਅੰਜਕ_seq *, stmt_ty);
KeywordOrStarred *_PyPegen_keyword_or_starred(Parser *, ਖਾਲੀ *, ਪੂਰਨ);
asdl_ਵਿਅੰਜਕ_seq *Py_Pegen_seq_ਕੱਢੋ_starred_exprs(Parser *, asdl_seq *);
asdl_keyword_seq *_PyPegen_seq_delete_starred_exprs(Parser *, asdl_seq *);
ਵਿਅੰਜਕ_ty _PyPegen_collect_call_seqs(Parser *, asdl_ਵਿਅੰਜਕ_seq *, asdl_seq *,
                     ਪੂਰਨ lineno, ਪੂਰਨ ਖਾਨਾ_ਵਿੱਥ, ਪੂਰਨ ਸਿਰਾ_lineno,
                     ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, PyArena *arena);
ਵਿਅੰਜਕ_ty _PyPegen_constant_from_token(Parser* p, Token* tok);
ਵਿਅੰਜਕ_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok);
ਵਿਅੰਜਕ_ty _PyPegen_constant_from_string(Parser* p, Token* tok);
ਵਿਅੰਜਕ_ty _PyPegen_concatenate_tstrings(Parser *p, asdl_ਵਿਅੰਜਕ_seq *, ਪੂਰਨ, ਪੂਰਨ, ਪੂਰਨ, ਪੂਰਨ, PyArena *);
ਵਿਅੰਜਕ_ty Py_Pegen_concatenate_ਲੜੀਆਂ(Parser *p, asdl_ਵਿਅੰਜਕ_seq *, ਪੂਰਨ, ਪੂਰਨ, ਪੂਰਨ, ਪੂਰਨ, PyArena *);
ਵਿਅੰਜਕ_ty _PyPegen_FetchRawForm(Parser *p, ਪੂਰਨ, ਪੂਰਨ, ਪੂਰਨ, ਪੂਰਨ);
ਵਿਅੰਜਕ_ty _PyPegen_ensure_imaginary(Parser *p, ਵਿਅੰਜਕ_ty);
ਵਿਅੰਜਕ_ty _PyPegen_ensure_real(Parser *p, ਵਿਅੰਜਕ_ty);
asdl_seq *_PyPegen_join_sequences(Parser *, asdl_seq *, asdl_seq *);
ਪੂਰਨ _PyPegen_check_barry_as_flufl(Parser *, Token *);
ਪੂਰਨ _PyPegen_check_legacy_stmt(Parser *p, ਵਿਅੰਜਕ_ty t);
ਖਾਲੀ *_PyPegen_raise_error_for_missing_comma(Parser *p, ਵਿਅੰਜਕ_ty a, ਵਿਅੰਜਕ_ty b);
ਨਤੀਜਾ_Token_With_Metadata *_PyPegen_check_fstring_conversion(Parser *p, Token *, ਵਿਅੰਜਕ_ty t);
ਨਤੀਜਾ_Token_With_Metadata *Py_Pegen_ਤਿਆਰੀ_ਭਰਿਆ_format_spec(Parser *, Token *, asdl_ਵਿਅੰਜਕ_seq *, ਪੂਰਨ, ਪੂਰਨ,
                                                         ਪੂਰਨ, ਪੂਰਨ, PyArena *);
mod_ty _PyPegen_make_module(Parser *, asdl_stmt_seq *);
ਖਾਲੀ *_PyPegen_arguments_parsing_error(Parser *, ਵਿਅੰਜਕ_ty);
ਵਿਅੰਜਕ_ty Py_Pegen_get_ਆਖਰੀ_comprehension_ਵਸਤੂ(comprehension_ty comprehension);
ਖਾਲੀ *Py_Pegen_nonparen_genexp_ਵਿੱਚ_call(Parser *p, ਵਿਅੰਜਕ_ty ਦਲੀਲਾਂ, asdl_comprehension_seq *comprehensions);
stmt_ty _PyPegen_checked_from_import(Parser *p, asdl_seq *dots, ਵਿਅੰਜਕ_ty module_name,
                                     asdl_alias_seq *ਨਾਂ, ਵਿਅੰਜਕ_ty lazy_token, ਪੂਰਨ lineno,
                                     ਪੂਰਨ ਖਾਨਾ_ਵਿੱਥ, ਪੂਰਨ ਸਿਰਾ_lineno, ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ,
                                     PyArena *arena);
asdl_stmt_seq* Py_Pegen_ਰਜਿਸਟਰ_stmts(Parser *p, asdl_stmt_seq* stmts);
stmt_ty Py_Pegen_ਰਜਿਸਟਰ_stmt(Parser *p, stmt_ty s);

// Parser API

Parser *_PyPegen_Parser_New(ਬਣਤਰ tok_state *, ਪੂਰਨ, ਪੂਰਨ, ਪੂਰਨ, ਪੂਰਨ *, ਅਚਲ ਅੱਖਰ*, PyArena *);
ਖਾਲੀ _PyPegen_Parser_Free(Parser *);
mod_ty Py_Pegen_ਲੜੀਵਾਰ_parser_from_file_ਸੂਚਕ(FILE *, ਪੂਰਨ, PyObject *, ਅਚਲ ਅੱਖਰ *,
                                    ਅਚਲ ਅੱਖਰ *, ਅਚਲ ਅੱਖਰ *, PyCompilerFlags *, ਪੂਰਨ *, PyObject **,
                                    PyArena *);
ਖਾਲੀ *Py_Pegen_ਲੜੀਵਾਰ_parser(Parser *);
mod_ty Py_Pegen_ਲੜੀਵਾਰ_parser_from_string(ਅਚਲ ਅੱਖਰ *, ਪੂਰਨ, PyObject *, PyCompilerFlags *, PyArena *, PyObject *, PyObject **);
asdl_stmt_seq *_PyPegen_interactive_exit(Parser *);

// Generated function in parse.c - function definition in python.gram
ਖਾਲੀ *_PyPegen_parse(Parser *);

#ਜੇ_ਅੰਤ
