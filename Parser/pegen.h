#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PEGEN_H
#ವ್ಯಾಖ್ಯಾನಿಸು PEGEN_H

#ಅನ್ತರ್ಭಾವ <Python.h>
#ಅನ್ತರ್ಭಾವ <pycore_ast.h>
#ಅನ್ತರ್ಭಾವ <pycore_token.h>
#ಅನ್ತರ್ಭಾವ <pycore_ceval.h>     // _Py_ReachedRecursionLimitWithMargin()
#ಅನ್ತರ್ಭಾವ <pycore_pystate.h>   // _Py_get_machine_stack_pointer()
#ಅನ್ತರ್ಭಾವ <pycore_pythonrun.h> // _PyOS_STACK_MARGIN_BYTES

#ಅನ್ತರ್ಭಾವ "tokenizer/tokenizer.h"

#ಆದರೆ 0
#ವ್ಯಾಖ್ಯಾನಿಸು PyPARSE_YIELD_IS_KEYWORD        0x0001
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು PyPARSE_DONT_IMPLY_DEDENT       0x0002

#ಆದರೆ 0
#ವ್ಯಾಖ್ಯಾನಿಸು PyPARSE_WITH_IS_KEYWORD         0x0003
#ವ್ಯಾಖ್ಯಾನಿಸು PyPARSE_PRINT_IS_FUNCTION       0x0004
#ವ್ಯಾಖ್ಯಾನಿಸು PyPARSE_UNICODE_LITERALS        0x0008
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು PyPARSE_IGNORE_COOKIE 0x0010
#ವ್ಯಾಖ್ಯಾನಿಸು PyPARSE_BARRY_AS_BDFL 0x0020
#ವ್ಯಾಖ್ಯಾನಿಸು PyPARSE_TYPE_COMMENTS 0x0040
#ವ್ಯಾಖ್ಯಾನಿಸು Py_PARSE_ALLOW_INCOMPLETE_ಒಳಹರಿವು 0x0100

#ವ್ಯಾಖ್ಯಾನಿಸು CURRENT_POS (-5)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ಜ್ಞಾಪನ_2 {
    ಪೂರ್ಣಾಂಕ type;
    ಶೂನ್ಯ *ಗಂಟು;
    ಪೂರ್ಣಾಂಕ mark;
    ರಚನೆ ಜ್ಞಾಪನ_2 *ಮುಂದಿನ;
} ಜ್ಞಾಪನ;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ type;
    // Text required by grammar actions or helpers; otherwise NULL.
    // An unfinished string token also has NULL text. Its grammar action
    // requests more input only if the token is valid at that position.
    PyObject *ಬೈಟುಗಳು;
    ಪೂರ್ಣಾಂಕ level;
    ಪೂರ್ಣಾಂಕ is_raw;
    ಪೂರ್ಣಾಂಕ lineno, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಕೊನೆ_lineno, ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;
    ಜ್ಞಾಪನ *ಜ್ಞಾಪನ_3;
    // Filter over the rule types present in `memo` (bit `type & 63` is set
    // for every entry): lets lookups skip walking the list on definite
    // misses, which are the common case.
    uint64_t ಜ್ಞಾಪನ_ಮುಖವಾಡ;
    PyObject *metadata;
} Token;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str;
    ಪೂರ್ಣಾಂಕ type;
} KeywordToken;


ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ರಚನೆ {
        ಪೂರ್ಣಾಂಕ lineno;
        ಅಕ್ಷರ *comment;  // The " <tag>" in "# type: ignore <tag>"
    } *ವಸ್ತುಗಳು;
    ಗಾತ್ರ_t ಗಾತ್ರ_2;
    ಗಾತ್ರ_t ಅಂಕೆ_ವಸ್ತುಗಳು;
} growable_comment_array;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ lineno;
    ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;
    ಪೂರ್ಣಾಂಕ ಕೊನೆ_lineno;
    ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;
} location;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ identifier_cache_ನಮೂದು Identifier_Cache_ನಮೂದು;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ರಚನೆ tok_state *tok;
    Token **ಟೋಕನುಗಳು;
    ಪೂರ್ಣಾಂಕ mark;
    ಪೂರ್ಣಾಂಕ fill, ಗಾತ್ರ_2;
    PyArena *arena;
    KeywordToken **keywords;
    ಅಕ್ಷರ **soft_keywords;
    ಪೂರ್ಣಾಂಕ n_keyword_lists;
    ಪೂರ್ಣಾಂಕ ಆರಂಭ_ನಿಯಮ;
    ಪೂರ್ಣಾಂಕ *errcode;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ parsing_started;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಒಳಹರಿವು_exhausted;
    PyObject* normalize;
    ಪೂರ್ಣಾಂಕ starting_lineno;
    ಪೂರ್ಣಾಂಕ starting_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;
    ಪೂರ್ಣಾಂಕ error_indicator;
    ಪೂರ್ಣಾಂಕ flags;
    ಪೂರ್ಣಾಂಕ feature_ಆವೃತ್ತಿ;
    growable_comment_array type_ignore_comments;
    Token *ತಿಳಿದ_err_token;
    ಪೂರ್ಣಾಂಕ level;
    ಪೂರ್ಣಾಂಕ call_invalid_ನಿಯಮಗಳು;
    ಪೂರ್ಣಾಂಕ debug;
    location ಕೊನೆಯ_stmt_location;
    Identifier_Cache_ನಮೂದು *identifier_cache;
    PyThreadState *tstate;
    // Cached copy of tstate->c_stack_soft_limit offset by one margin toward
    // the stack top, so the per-rule stack check is a single inline comparison.
    uintptr_t ರಾಶಿ_soft_ಎಲ್ಲೆ;
} Parser;

// C-stack exhaustion check run on every rule entry. The fast path compares
// the machine stack pointer against the soft limit cached in the Parser;
// only when the stack is nearly exhausted is the full check called.
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ
Py_Pegen_ರಾಶಿ_exhausted(Parser *p)
{
#ಆದರೆ Py_ರಾಶಿ_GROWS_ಕೆಳಕ್ಕೆ
    ಆದರೆ (Py_get_machine_ರಾಶಿ_ಸೂಚಕ() > p->ರಾಶಿ_soft_ಎಲ್ಲೆ) {
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (Py_get_machine_ರಾಶಿ_ಸೂಚಕ() <= p->ರಾಶಿ_soft_ಎಲ್ಲೆ) {
#ಆದರೆ_ಅಂತ್ಯ
        ಪ್ರತಿಜ್ಞಾ(!Py_Reached_Recursion_ಎಲ್ಲೆ_With_Margin(p->tstate, 1));
        ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ Py_Reached_Recursion_ಎಲ್ಲೆ_With_Margin(p->tstate, 1);
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    cmpop_ty cmpop;
    ಅಭಿವ್ಯಕ್ತಿ_ty ಅಭಿವ್ಯಕ್ತಿ;
} Cmpop_ಅಭಿವ್ಯಕ್ತಿ_Pair;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಅಭಿವ್ಯಕ್ತಿ_ty ಕೀಲಿ;
    ಅಭಿವ್ಯಕ್ತಿ_ty ಮೌಲ್ಯ;
} ಕೀಲಿ_ಮೌಲ್ಯ_Pair;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಅಭಿವ್ಯಕ್ತಿ_ty ಕೀಲಿ;
    ನಮೂನೆ_ty ನಮೂನೆ;
} ಕೀಲಿ_ನಮೂನೆ_Pair;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    arg_ty arg;
    ಅಭಿವ್ಯಕ್ತಿ_ty ಮೌಲ್ಯ;
} NameDefaultPair;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    asdl_arg_seq *plain_ಹೆಸರುಗಳು;
    asdl_seq *ಹೆಸರುಗಳು_with_defaults; // asdl_seq* of NameDefaultsPair's
} SlashWithDefault;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    arg_ty vararg;
    asdl_seq *kwonlyargs; // asdl_seq* of NameDefaultsPair's
    arg_ty kwarg;
} ನಕ್ಷತ್ರ_Etc;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ { operator_ty ಬಗೆ; } AugOperator;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಶೂನ್ಯ *element;
    ಪೂರ್ಣಾಂಕ is_keyword;
} KeywordOrStarred;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಶೂನ್ಯ *ಫಲಿತಾಂಶ;
    PyObject *metadata;
} ಫಲಿತಾಂಶ_Token_With_Metadata;

// Internal parser functions
#ಆದರೆ defined(Py_DEBUG)
ಶೂನ್ಯ Py_Pegen_clear_ಜ್ಞಾಪನ_statistics(ಶೂನ್ಯ);
PyObject *Py_Pegen_get_ಜ್ಞಾಪನ_statistics(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ Py_Pegen_ಸೇರಿಸು_ಜ್ಞಾಪನ(Parser *p, ಪೂರ್ಣಾಂಕ mark, ಪೂರ್ಣಾಂಕ type, ಶೂನ್ಯ *ಗಂಟು);
ಪೂರ್ಣಾಂಕ Py_Pegen_update_ಜ್ಞಾಪನ(Parser *p, ಪೂರ್ಣಾಂಕ mark, ಪೂರ್ಣಾಂಕ type, ಶೂನ್ಯ *ಗಂಟು);
ಜ್ಞಾಪನ *Py_Pegen_ಸೇರಿಸು_ಜ್ಞಾಪನ_ನೇರ(Parser *p, ಪೂರ್ಣಾಂಕ mark, ಪೂರ್ಣಾಂಕ type);
ಪೂರ್ಣಾಂಕ _PyPegen_is_memoized(Parser *p, ಪೂರ್ಣಾಂಕ type, ಶೂನ್ಯ *pres);

ಪೂರ್ಣಾಂಕ _PyPegen_lookahead(ಪೂರ್ಣಾಂಕ, ಶೂನ್ಯ *(func)(Parser *), Parser *);
ಪೂರ್ಣಾಂಕ Py_Pegen_lookahead_for_ಅಭಿವ್ಯಕ್ತಿ(ಪೂರ್ಣಾಂಕ, ಅಭಿವ್ಯಕ್ತಿ_ty (func)(Parser *), Parser *);
ಪೂರ್ಣಾಂಕ _PyPegen_lookahead_for_stmt(ಪೂರ್ಣಾಂಕ, stmt_ty (func)(Parser *), Parser *);
ಪೂರ್ಣಾಂಕ _PyPegen_lookahead_with_int(ಪೂರ್ಣಾಂಕ, Token *(func)(Parser *, ಪೂರ್ಣಾಂಕ), Parser *, ಪೂರ್ಣಾಂಕ);
ಪೂರ್ಣಾಂಕ _PyPegen_lookahead_with_string(ಪೂರ್ಣಾಂಕ, ಅಭಿವ್ಯಕ್ತಿ_ty (func)(Parser *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*), Parser *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*);

Token *_PyPegen_expect_token(Parser *p, ಪೂರ್ಣಾಂಕ type);
ಶೂನ್ಯ* Py_Pegen_expect_forced_ಫಲಿತಾಂಶ(Parser *p, ಶೂನ್ಯ* ಫಲಿತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* expected);
Token *_PyPegen_expect_forced_token(Parser *p, ಪೂರ್ಣಾಂಕ type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* expected);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_expect_soft_keyword(Parser *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *keyword);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_soft_keyword_token(Parser *p);
ಅಭಿವ್ಯಕ್ತಿ_ty Py_Pegen_fstring_ಮಧ್ಯ_token(Parser* p);
Token *Py_Pegen_get_ಕೊನೆಯ_nonnwhitespace_token(Parser *);
ಪೂರ್ಣಾಂಕ _PyPegen_fill_token(Parser *p);
// Return -1 on failure, leaving the buffer and capacity unchanged.
ಪೂರ್ಣಾಂಕ Py_Pegen_grow_loop_ಬಫರ್(ಶೂನ್ಯ ***ಬಫರ್, Py_ssize_t *ಸಾಮರ್ಥ್ಯ);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_name_token(Parser *p);
ಅಭಿವ್ಯಕ್ತಿ_ty Py_Pegen_ಸಂಖ್ಯೆ_token(Parser *p);
ಶೂನ್ಯ *_PyPegen_string_token(Parser *p);
Py_ssize_t Py_Pegen_byte_ಸ್ಥಾನಾಂತರ_ಗಮ್ಯ_character_ಸ್ಥಾನಾಂತರ_ಸಾಲು(PyObject *ಸಾಲು, Py_ssize_t ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, Py_ssize_t ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ);
Py_ssize_t Py_Pegen_byte_ಸ್ಥಾನಾಂತರ_ಗಮ್ಯ_character_ಸ್ಥಾನಾಂತರ(PyObject *ಸಾಲು, Py_ssize_t ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ);
Py_ssize_t Py_Pegen_byte_ಸ್ಥಾನಾಂತರ_ಗಮ್ಯ_character_ಸ್ಥಾನಾಂತರ_raw(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*, Py_ssize_t ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    ನಕ್ಷತ್ರ_TARGETS,
    DEL_TARGETS,
    FOR_TARGETS,
    SINGLE_TARGETS,
    ATTRIBUTE_OR_SUBSCRIPT_TARGETS
} TARGETS_TYPE;

// Error handling functions and APIs
ಪೂರ್ಣಾಂಕ _Pypegen_raise_decode_error(Parser *p);
ಪೂರ್ಣಾಂಕ _Pypegen_tokenizer_error(Parser *p);
ಶೂನ್ಯ *Py_Pegen_raise_incomplete_ಒಳಹರಿವು(Parser *p);
ಶೂನ್ಯ *_PyPegen_raise_error(Parser *p, PyObject *errtype, ಪೂರ್ಣಾಂಕ use_mark, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *errmsg, ...);
ಶೂನ್ಯ *Py_Pegen_raise_error_ತಿಳಿದ_location(Parser *p, PyObject *errtype,
                                          Py_ssize_t lineno, Py_ssize_t ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                                          Py_ssize_t ಕೊನೆ_lineno, Py_ssize_t ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *errmsg, va_list va);
ಶೂನ್ಯ Pypegen_ಗಣ_syntax_error(Parser* p, Token* ಕೊನೆಯ_token);
ಶೂನ್ಯ Pypegen_ರಾಶಿ_ಅತಿಪ್ರವಾಹ(Parser *p);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *
RAISE_ERROR_ತಿಳಿದ_LOCATION(Parser *p, PyObject *errtype,
                           Py_ssize_t lineno, Py_ssize_t ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                           Py_ssize_t ಕೊನೆ_lineno, Py_ssize_t ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *errmsg, ...)
{
    va_list va;
    va_ಆರಂಭ(va, errmsg);
    Py_ssize_t ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ_2 = (ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ == CURRENT_POS ? CURRENT_POS : ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ + 1);
    Py_ssize_t ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ_2 = (ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ == CURRENT_POS ? CURRENT_POS : ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ + 1);
    Py_Pegen_raise_error_ತಿಳಿದ_location(p, errtype, lineno, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ_2, ಕೊನೆ_lineno, ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ_2, errmsg, va);
    va_ಕೊನೆ(va);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ವ್ಯಾಖ್ಯಾನಿಸು RAISE_SYNTAX_ERROR(ಸಂದೇಶ, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 0, ಸಂದೇಶ, ##__VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು RAISE_INDENTATION_ERROR(ಸಂದೇಶ, ...) _PyPegen_raise_error(p, PyExc_IndentationError, 0, ಸಂದೇಶ, ##__VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು RAISE_SYNTAX_ERROR_ON_ಮುಂದಿನ_TOKEN(ಸಂದೇಶ, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 1, ಸಂದೇಶ, ##__VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು RAISE_SYNTAX_ERROR_ತಿಳಿದ_RANGE(a, b, ಸಂದೇಶ, ...) \
    RAISE_ERROR_ತಿಳಿದ_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, (b)->ಕೊನೆ_lineno, (b)->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಸಂದೇಶ, ##__VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು RAISE_SYNTAX_ERROR_ತಿಳಿದ_LOCATION(a, ಸಂದೇಶ, ...) \
    RAISE_ERROR_ತಿಳಿದ_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, (a)->ಕೊನೆ_lineno, (a)->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಸಂದೇಶ, ##__VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು RAISE_SYNTAX_ERROR_STARTING_FROM(a, ಸಂದೇಶ, ...) \
    RAISE_ERROR_ತಿಳಿದ_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, CURRENT_POS, CURRENT_POS, ಸಂದೇಶ, ##__VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು RAISE_SYNTAX_ERROR_INVALID_ಗುರಿ(type, e) RAISE_SYNTAX_ERROR_INVALID_ಗುರಿ_2(p, type, e)

Py_LOCAL_INLINE(ಪೂರ್ಣಾಂಕ)
Py_Pegen_ಟೋಕನುಗಳು_are_adjacent(Token *a, Token *b)
{
    ಹಿಂತಿರುಗಿ (a->ಕೊನೆ_lineno == b->lineno) && (a->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ == b->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ);
}

Py_LOCAL_INLINE(ಶೂನ್ಯ *)
CHECK_CALL(Parser *p, ಶೂನ್ಯ *ಫಲಿತಾಂಶ)
{
    ಆದರೆ (ಫಲಿತಾಂಶ == ಖಾಲಿ) {
        ಪ್ರತಿಜ್ಞಾ(PyErr_Occurred());
        p->error_indicator = 1;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* This is needed for helper functions that are allowed to
   return NULL without an error. Example: _PyPegen_seq_extract_starred_exprs */
Py_LOCAL_INLINE(ಶೂನ್ಯ *)
CHECK_CALL_NULL_ALLOWED(Parser *p, ಶೂನ್ಯ *ಫಲಿತಾಂಶ)
{
    ಆದರೆ (ಫಲಿತಾಂಶ == ಖಾಲಿ && PyErr_Occurred()) {
        p->error_indicator = 1;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿಸು CHECK(type, ಫಲಿತಾಂಶ) ((type) CHECK_CALL(p, ಫಲಿತಾಂಶ))
#ವ್ಯಾಖ್ಯಾನಿಸು CHECK_NULL_ALLOWED(type, ಫಲಿತಾಂಶ) ((type) CHECK_CALL_NULL_ALLOWED(p, ಫಲಿತಾಂಶ))

ಅಭಿವ್ಯಕ್ತಿ_ty Py_Pegen_get_invalid_ಗುರಿ(ಅಭಿವ್ಯಕ್ತಿ_ty e, TARGETS_TYPE targets_type);
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Py_Pegen_get_ಅಭಿವ್ಯಕ್ತಿ_name(ಅಭಿವ್ಯಕ್ತಿ_ty);
Py_LOCAL_INLINE(ಶೂನ್ಯ *)
RAISE_SYNTAX_ERROR_INVALID_ಗುರಿ_2(Parser *p, TARGETS_TYPE type, ಶೂನ್ಯ *e)
{
    ಅಭಿವ್ಯಕ್ತಿ_ty invalid_ಗುರಿ = CHECK_NULL_ALLOWED(ಅಭಿವ್ಯಕ್ತಿ_ty, Py_Pegen_get_invalid_ಗುರಿ(e, type));
    ಆದರೆ (invalid_ಗುರಿ != ಖಾಲಿ) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ;
        ಆದರೆ (type == ನಕ್ಷತ್ರ_TARGETS || type == FOR_TARGETS) {
            ಸಂದೇಶ = "cannot assign to %s";
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            ಸಂದೇಶ = "cannot delete %s";
        }
        ಹಿಂತಿರುಗಿ RAISE_SYNTAX_ERROR_ತಿಳಿದ_LOCATION(
            invalid_ಗುರಿ,
            ಸಂದೇಶ,
            Py_Pegen_get_ಅಭಿವ್ಯಕ್ತಿ_name(invalid_ಗುರಿ)
        );
        ಹಿಂತಿರುಗಿ RAISE_SYNTAX_ERROR_ತಿಳಿದ_LOCATION(invalid_ಗುರಿ, "invalid syntax");
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

// Action utility functions

ಶೂನ್ಯ *_PyPegen_dummy_name(Parser *p, ...);
ಶೂನ್ಯ * Py_Pegen_seq_ಕೊನೆಯ_ಅಂಶ(asdl_seq *seq);
#ವ್ಯಾಖ್ಯಾನಿಸು Py_Pegen_ಕೊನೆಯ_ಅಂಶ(seq, type) ((type)Py_Pegen_seq_ಕೊನೆಯ_ಅಂಶ((asdl_seq*)seq))
ಶೂನ್ಯ * Py_Pegen_seq_ಮೊದಲ_ಅಂಶ(asdl_seq *seq);
#ವ್ಯಾಖ್ಯಾನಿಸು Py_Pegen_ಮೊದಲ_ಅಂಶ(seq, type) ((type)Py_Pegen_seq_ಮೊದಲ_ಅಂಶ((asdl_seq*)seq))
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಳಸದ(ಅಭಿವ್ಯಕ್ತಿ) ಮಾಡು { (ಶೂನ್ಯ)(ಅಭಿವ್ಯಕ್ತಿ); } ಆಗ (0)
#ವ್ಯಾಖ್ಯಾನಿಸು EXTRA_ಅಭಿವ್ಯಕ್ತಿ(ತಲೆ, ಬಾಲ) ತಲೆ->lineno, (ತಲೆ)->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, (ಬಾಲ)->ಕೊನೆ_lineno, (ಬಾಲ)->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, p->arena
#ವ್ಯಾಖ್ಯಾನಿಸು EXTRA ಆರಂಭ_lineno, ಆರಂಭ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ_2, ಕೊನೆ_lineno_2, ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ_2, p->arena
PyObject *_PyPegen_new_type_comment(Parser *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);

Py_LOCAL_INLINE(PyObject *)
NEW_TYPE_COMMENT(Parser *p, Token *tc)
{
    ಆದರೆ (tc == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೈಟುಗಳು = Py_ಬೈಟುಗಳು_As_String(tc->ಬೈಟುಗಳು);
    ಆದರೆ (ಬೈಟುಗಳು == ಖಾಲಿ) {
        ಹೋಗು error;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, ಬೈಟುಗಳು);
    ಆದರೆ (tco == ಖಾಲಿ) {
        ಹೋಗು error;
    }
    ಹಿಂತಿರುಗಿ tco;
 error:
    p->error_indicator = 1;  // Inline CHECK_CALL
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

Py_LOCAL_INLINE(ಶೂನ್ಯ *)
INVALID_ಆವೃತ್ತಿ_CHECK(Parser *p, ಪೂರ್ಣಾಂಕ ಆವೃತ್ತಿ, ಅಕ್ಷರ *ಸಂದೇಶ, ಶೂನ್ಯ *ಗಂಟು)
{
    ಆದರೆ (ಗಂಟು == ಖಾಲಿ) {
        p->error_indicator = 1;  // Inline CHECK_CALL
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (p->feature_ಆವೃತ್ತಿ < ಆವೃತ್ತಿ) {
        p->error_indicator = 1;
        ಹಿಂತಿರುಗಿ RAISE_SYNTAX_ERROR("%s only supported in Python 3.%i and greater",
                                  ಸಂದೇಶ, ಆವೃತ್ತಿ);
    }
    ಹಿಂತಿರುಗಿ ಗಂಟು;
}

#ವ್ಯಾಖ್ಯಾನಿಸು CHECK_ಆವೃತ್ತಿ(type, ಆವೃತ್ತಿ, ಸಂದೇಶ, ಗಂಟು) ((type) INVALID_ಆವೃತ್ತಿ_CHECK(p, ಆವೃತ್ತಿ, ಸಂದೇಶ, ಗಂಟು))

arg_ty Py_Pegen_ಕೂಡಿಸು_type_comment_ಗಮ್ಯ_arg(Parser *, arg_ty, Token *);
PyObject *_PyPegen_new_identifier(Parser *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
asdl_seq *_PyPegen_singleton_seq(Parser *, ಶೂನ್ಯ *);
asdl_seq *Py_Pegen_seq_ಸೇರಿಸು_ಒಳ_front(Parser *, ಶೂನ್ಯ *, asdl_seq *);
asdl_seq *Py_Pegen_seq_append_ಗಮ್ಯ_ಕೊನೆ(Parser *, asdl_seq *, ಶೂನ್ಯ *);
asdl_seq *_PyPegen_seq_flatten(Parser *, asdl_seq *);
ಅಭಿವ್ಯಕ್ತಿ_ty Py_Pegen_join_ಹೆಸರುಗಳು_with_dot(Parser *, ಅಭಿವ್ಯಕ್ತಿ_ty, ಅಭಿವ್ಯಕ್ತಿ_ty);
ಪೂರ್ಣಾಂಕ Py_Pegen_seq_ಎಣಿಕೆ_dots(asdl_seq *);
alias_ty Py_Pegen_alias_for_ನಕ್ಷತ್ರ(Parser *, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, PyArena *);
asdl_identifier_seq *Py_Pegen_map_ಹೆಸರುಗಳು_ಗಮ್ಯ_ids(Parser *, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *);
Cmpop_ಅಭಿವ್ಯಕ್ತಿ_Pair *Py_Pegen_cmpop_ಅಭಿವ್ಯಕ್ತಿ_pair(Parser *, cmpop_ty, ಅಭಿವ್ಯಕ್ತಿ_ty);
asdl_int_seq *_PyPegen_get_cmpops(Parser *p, asdl_seq *);
asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *_PyPegen_get_exprs(Parser *, asdl_seq *);
ಅಭಿವ್ಯಕ್ತಿ_ty Py_Pegen_ಗಣ_ಅಭಿವ್ಯಕ್ತಿ_context(Parser *, ಅಭಿವ್ಯಕ್ತಿ_ty, ಅಭಿವ್ಯಕ್ತಿ_context_ty);
ಅಭಿವ್ಯಕ್ತಿ_ty Py_Pegen_make_ಗುರಿ(Parser *, ಅಭಿವ್ಯಕ್ತಿ_ty, TARGETS_TYPE);
ಕೀಲಿ_ಮೌಲ್ಯ_Pair *Py_Pegen_ಕೀಲಿ_ಮೌಲ್ಯ_pair(Parser *, ಅಭಿವ್ಯಕ್ತಿ_ty, ಅಭಿವ್ಯಕ್ತಿ_ty);
asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *Py_Pegen_get_ಕೀಲಿಗಳು(Parser *, asdl_seq *);
asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *Py_Pegen_get_ಮೌಲ್ಯಗಳು(Parser *, asdl_seq *);
ಕೀಲಿ_ನಮೂನೆ_Pair *Py_Pegen_ಕೀಲಿ_ನಮೂನೆ_pair(Parser *, ಅಭಿವ್ಯಕ್ತಿ_ty, ನಮೂನೆ_ty);
asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *Py_Pegen_get_ನಮೂನೆ_ಕೀಲಿಗಳು(Parser *, asdl_seq *);
asdl_ನಮೂನೆ_seq *_PyPegen_get_patterns(Parser *, asdl_seq *);
NameDefaultPair *_PyPegen_name_default_pair(Parser *, arg_ty, ಅಭಿವ್ಯಕ್ತಿ_ty, Token *);
SlashWithDefault *_PyPegen_slash_with_default(Parser *, asdl_arg_seq *, asdl_seq *);
ನಕ್ಷತ್ರ_Etc *Py_Pegen_ನಕ್ಷತ್ರ_etc(Parser *, arg_ty, asdl_seq *, arg_ty);
arguments_ty _PyPegen_make_arguments(Parser *, asdl_arg_seq *, SlashWithDefault *,
                                     asdl_arg_seq *, asdl_seq *, ನಕ್ಷತ್ರ_Etc *);
arguments_ty Py_Pegen_ಖಾಲಿಗಳು_arguments(Parser *);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_template_str(Parser *p, Token *a, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *raw_expressions, Token *b);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_joined_str(Parser *p, Token *a, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *raw_expressions, Token *b);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_interpolation(Parser *, ಅಭಿವ್ಯಕ್ತಿ_ty, Token *, ಫಲಿತಾಂಶ_Token_With_Metadata *, ಫಲಿತಾಂಶ_Token_With_Metadata *, Token *,
                                 ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, PyArena *);
ಅಭಿವ್ಯಕ್ತಿ_ty Py_Pegen_formatted_ಮೌಲ್ಯ(Parser *, ಅಭಿವ್ಯಕ್ತಿ_ty, Token *, ಫಲಿತಾಂಶ_Token_With_Metadata *, ಫಲಿತಾಂಶ_Token_With_Metadata *, Token *,
                                 ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, PyArena *);
AugOperator *_PyPegen_augoperator(Parser*, operator_ty type);
stmt_ty _PyPegen_function_def_decorators(Parser *, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *, stmt_ty);
stmt_ty _PyPegen_class_def_decorators(Parser *, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *, stmt_ty);
KeywordOrStarred *_PyPegen_keyword_or_starred(Parser *, ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ);
asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *Py_Pegen_seq_ಹೊರತೆಗೆ_starred_exprs(Parser *, asdl_seq *);
asdl_keyword_seq *_PyPegen_seq_delete_starred_exprs(Parser *, asdl_seq *);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_collect_call_seqs(Parser *, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *, asdl_seq *,
                     ಪೂರ್ಣಾಂಕ lineno, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಕೊನೆ_lineno,
                     ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, PyArena *arena);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_constant_from_token(Parser* p, Token* tok);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_constant_from_string(Parser* p, Token* tok);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_concatenate_tstrings(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, PyArena *);
ಅಭಿವ್ಯಕ್ತಿ_ty Py_Pegen_concatenate_ಅಕ್ಷರಸರಣಿಗಳು(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, PyArena *);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_FetchRawForm(Parser *p, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_ensure_imaginary(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty);
ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_ensure_real(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty);
asdl_seq *_PyPegen_join_sequences(Parser *, asdl_seq *, asdl_seq *);
ಪೂರ್ಣಾಂಕ _PyPegen_check_barry_as_flufl(Parser *, Token *);
ಪೂರ್ಣಾಂಕ _PyPegen_check_legacy_stmt(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty t);
ಶೂನ್ಯ *_PyPegen_raise_error_for_missing_comma(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty a, ಅಭಿವ್ಯಕ್ತಿ_ty b);
ಫಲಿತಾಂಶ_Token_With_Metadata *_PyPegen_check_fstring_conversion(Parser *p, Token *, ಅಭಿವ್ಯಕ್ತಿ_ty t);
ಫಲಿತಾಂಶ_Token_With_Metadata *Py_Pegen_ಸಿದ್ಧತೆ_ತುಂಬಿದ_format_spec(Parser *, Token *, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ,
                                                         ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, PyArena *);
mod_ty _PyPegen_make_module(Parser *, asdl_stmt_seq *);
ಶೂನ್ಯ *_PyPegen_arguments_parsing_error(Parser *, ಅಭಿವ್ಯಕ್ತಿ_ty);
ಅಭಿವ್ಯಕ್ತಿ_ty Py_Pegen_get_ಕೊನೆಯ_comprehension_ಅಂಶ(comprehension_ty comprehension);
ಶೂನ್ಯ *Py_Pegen_nonparen_genexp_ಒಳ_call(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty ವಾದಗಳು, asdl_comprehension_seq *comprehensions);
stmt_ty _PyPegen_checked_from_import(Parser *p, asdl_seq *dots, ಅಭಿವ್ಯಕ್ತಿ_ty module_name,
                                     asdl_alias_seq *ಹೆಸರುಗಳು, ಅಭಿವ್ಯಕ್ತಿ_ty lazy_token, ಪೂರ್ಣಾಂಕ lineno,
                                     ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಕೊನೆ_lineno, ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                                     PyArena *arena);
asdl_stmt_seq* Py_Pegen_ರಿಜಿಸ್ಟರ್_stmts(Parser *p, asdl_stmt_seq* stmts);
stmt_ty Py_Pegen_ರಿಜಿಸ್ಟರ್_stmt(Parser *p, stmt_ty s);

// Parser API

Parser *_PyPegen_Parser_New(ರಚನೆ tok_state *, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*, PyArena *);
ಶೂನ್ಯ _PyPegen_Parser_Free(Parser *);
mod_ty Py_Pegen_ಸತತ_parser_from_file_ಸೂಚಕ(FILE *, ಪೂರ್ಣಾಂಕ, PyObject *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *,
                                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, PyCompilerFlags *, ಪೂರ್ಣಾಂಕ *, PyObject **,
                                    PyArena *);
ಶೂನ್ಯ *Py_Pegen_ಸತತ_parser(Parser *);
mod_ty Py_Pegen_ಸತತ_parser_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಪೂರ್ಣಾಂಕ, PyObject *, PyCompilerFlags *, PyArena *, PyObject *, PyObject **);
asdl_stmt_seq *_PyPegen_interactive_exit(Parser *);

// Generated function in parse.c - function definition in python.gram
ಶೂನ್ಯ *_PyPegen_parse(Parser *);

#ಆದರೆ_ಅಂತ್ಯ
