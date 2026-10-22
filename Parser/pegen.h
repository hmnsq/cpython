#నిర్వచితం_కాకపోతే PEGEN_H
#నిర్వచించు PEGEN_H

#అన్తర్భావ <Python.h>
#అన్తర్భావ <pycore_ast.h>
#అన్తర్భావ <pycore_token.h>
#అన్తర్భావ <pycore_ceval.h>     // _Py_ReachedRecursionLimitWithMargin()
#అన్తర్భావ <pycore_pystate.h>   // _Py_get_machine_stack_pointer()
#అన్తర్భావ <pycore_pythonrun.h> // _PyOS_STACK_MARGIN_BYTES

#అన్తర్భావ "tokenizer/tokenizer.h"

#ఒకవేళ 0
#నిర్వచించు PyPARSE_YIELD_IS_KEYWORD        0x0001
#అయితే_ముగింపు

#నిర్వచించు PyPARSE_DONT_IMPLY_DEDENT       0x0002

#ఒకవేళ 0
#నిర్వచించు PyPARSE_WITH_IS_KEYWORD         0x0003
#నిర్వచించు PyPARSE_PRINT_IS_FUNCTION       0x0004
#నిర్వచించు PyPARSE_UNICODE_LITERALS        0x0008
#అయితే_ముగింపు

#నిర్వచించు PyPARSE_IGNORE_COOKIE 0x0010
#నిర్వచించు PyPARSE_BARRY_AS_BDFL 0x0020
#నిర్వచించు PyPARSE_TYPE_COMMENTS 0x0040
#నిర్వచించు Py_PARSE_ALLOW_INCOMPLETE_ఆదానం 0x0100

#నిర్వచించు CURRENT_POS (-5)

రకనిర్వచనం నిర్మాణం జ్ఞాపిక_2 {
    పూర్ణాంకం type;
    శూన్య *కణుపు;
    పూర్ణాంకం mark;
    నిర్మాణం జ్ఞాపిక_2 *తదుపరి;
} జ్ఞాపిక;

రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం type;
    // Text required by grammar actions or helpers; otherwise NULL.
    // An unfinished string token also has NULL text. Its grammar action
    // requests more input only if the token is valid at that position.
    PyObject *బైట్లు;
    పూర్ణాంకం level;
    పూర్ణాంకం is_raw;
    పూర్ణాంకం lineno, నిలువు_స్థానభ్రంశం, చివర_lineno, చివర_నిలువు_స్థానభ్రంశం;
    జ్ఞాపిక *జ్ఞాపిక_3;
    // Filter over the rule types present in `memo` (bit `type & 63` is set
    // for every entry): lets lookups skip walking the list on definite
    // misses, which are the common case.
    uint64_t జ్ఞాపిక_ముసుగు;
    PyObject *metadata;
} Token;

రకనిర్వచనం నిర్మాణం {
    స్థిరరాశి అక్షరం *str;
    పూర్ణాంకం type;
} KeywordToken;


రకనిర్వచనం నిర్మాణం {
    నిర్మాణం {
        పూర్ణాంకం lineno;
        అక్షరం *comment;  // The " <tag>" in "# type: ignore <tag>"
    } *వస్తువులు;
    కొలత_t కొలత;
    కొలత_t అంకె_వస్తువులు;
} growable_comment_array;

రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం lineno;
    పూర్ణాంకం నిలువు_స్థానభ్రంశం;
    పూర్ణాంకం చివర_lineno;
    పూర్ణాంకం చివర_నిలువు_స్థానభ్రంశం;
} location;

రకనిర్వచనం నిర్మాణం identifier_cache_నమోదు Identifier_Cache_నమోదు;

రకనిర్వచనం నిర్మాణం {
    నిర్మాణం tok_state *tok;
    Token **టోకెన్లు;
    పూర్ణాంకం mark;
    పూర్ణాంకం fill, కొలత;
    PyArena *arena;
    KeywordToken **keywords;
    అక్షరం **soft_keywords;
    పూర్ణాంకం n_keyword_lists;
    పూర్ణాంకం ప్రారంభం_నియమం;
    పూర్ణాంకం *errcode;
    గుర్తులేని అక్షరం parsing_started;
    గుర్తులేని అక్షరం ఆదానం_exhausted;
    PyObject* normalize;
    పూర్ణాంకం starting_lineno;
    పూర్ణాంకం starting_నిలువు_స్థానభ్రంశం;
    పూర్ణాంకం error_indicator;
    పూర్ణాంకం flags;
    పూర్ణాంకం feature_సంస్కరణ;
    growable_comment_array type_ignore_comments;
    Token *తెలిసిన_err_token;
    పూర్ణాంకం level;
    పూర్ణాంకం call_invalid_నియమాలు;
    పూర్ణాంకం debug;
    location చివరి_stmt_location;
    Identifier_Cache_నమోదు *identifier_cache;
    PyThreadState *tstate;
    // Cached copy of tstate->c_stack_soft_limit offset by one margin toward
    // the stack top, so the per-rule stack check is a single inline comparison.
    uintptr_t దొంతర_soft_హద్దు;
} Parser;

// C-stack exhaustion check run on every rule entry. The fast path compares
// the machine stack pointer against the soft limit cached in the Parser;
// only when the stack is nearly exhausted is the full check called.
స్థిర అంతఃవరుస పూర్ణాంకం
Py_Pegen_దొంతర_exhausted(Parser *p)
{
#ఒకవేళ Py_దొంతర_GROWS_కిందికి
    ఒకవేళ (Py_get_machine_దొంతర_సూచిక() > p->దొంతర_soft_హద్దు) {
#లేకపోతే
    ఒకవేళ (Py_get_machine_దొంతర_సూచిక() <= p->దొంతర_soft_హద్దు) {
#అయితే_ముగింపు
        ప్రతిజ్ఞా(!Py_Reached_Recursion_హద్దు_With_Margin(p->tstate, 1));
        తిరిగి 0;
    }
    తిరిగి Py_Reached_Recursion_హద్దు_With_Margin(p->tstate, 1);
}

రకనిర్వచనం నిర్మాణం {
    cmpop_ty cmpop;
    సమాసం_ty సమాసం;
} Cmpop_సమాసం_Pair;

రకనిర్వచనం నిర్మాణం {
    సమాసం_ty కీలకం;
    సమాసం_ty విలువ;
} కీలకం_విలువ_Pair;

రకనిర్వచనం నిర్మాణం {
    సమాసం_ty కీలకం;
    నమూనా_ty నమూనా;
} కీలకం_నమూనా_Pair;

రకనిర్వచనం నిర్మాణం {
    arg_ty arg;
    సమాసం_ty విలువ;
} NameDefaultPair;

రకనిర్వచనం నిర్మాణం {
    asdl_arg_seq *plain_పేర్లు;
    asdl_seq *పేర్లు_with_defaults; // asdl_seq* of NameDefaultsPair's
} SlashWithDefault;

రకనిర్వచనం నిర్మాణం {
    arg_ty vararg;
    asdl_seq *kwonlyargs; // asdl_seq* of NameDefaultsPair's
    arg_ty kwarg;
} నక్షత్రం_Etc;

రకనిర్వచనం నిర్మాణం { operator_ty రకం; } AugOperator;
రకనిర్వచనం నిర్మాణం {
    శూన్య *element;
    పూర్ణాంకం is_keyword;
} KeywordOrStarred;

రకనిర్వచనం నిర్మాణం {
    శూన్య *ఫలితం;
    PyObject *metadata;
} ఫలితం_Token_With_Metadata;

// Internal parser functions
#ఒకవేళ defined(Py_DEBUG)
శూన్య Py_Pegen_clear_జ్ఞాపిక_statistics(శూన్య);
PyObject *Py_Pegen_get_జ్ఞాపిక_statistics(శూన్య);
#అయితే_ముగింపు

పూర్ణాంకం Py_Pegen_చొప్పించు_జ్ఞాపిక(Parser *p, పూర్ణాంకం mark, పూర్ణాంకం type, శూన్య *కణుపు);
పూర్ణాంకం Py_Pegen_update_జ్ఞాపిక(Parser *p, పూర్ణాంకం mark, పూర్ణాంకం type, శూన్య *కణుపు);
జ్ఞాపిక *Py_Pegen_చొప్పించు_జ్ఞాపిక_ప్రత్యక్ష(Parser *p, పూర్ణాంకం mark, పూర్ణాంకం type);
పూర్ణాంకం _PyPegen_is_memoized(Parser *p, పూర్ణాంకం type, శూన్య *pres);

పూర్ణాంకం _PyPegen_lookahead(పూర్ణాంకం, శూన్య *(func)(Parser *), Parser *);
పూర్ణాంకం Py_Pegen_lookahead_for_సమాసం(పూర్ణాంకం, సమాసం_ty (func)(Parser *), Parser *);
పూర్ణాంకం _PyPegen_lookahead_for_stmt(పూర్ణాంకం, stmt_ty (func)(Parser *), Parser *);
పూర్ణాంకం _PyPegen_lookahead_with_int(పూర్ణాంకం, Token *(func)(Parser *, పూర్ణాంకం), Parser *, పూర్ణాంకం);
పూర్ణాంకం _PyPegen_lookahead_with_string(పూర్ణాంకం, సమాసం_ty (func)(Parser *, స్థిరరాశి అక్షరం*), Parser *, స్థిరరాశి అక్షరం*);

Token *_PyPegen_expect_token(Parser *p, పూర్ణాంకం type);
శూన్య* Py_Pegen_expect_forced_ఫలితం(Parser *p, శూన్య* ఫలితం, స్థిరరాశి అక్షరం* expected);
Token *_PyPegen_expect_forced_token(Parser *p, పూర్ణాంకం type, స్థిరరాశి అక్షరం* expected);
సమాసం_ty _PyPegen_expect_soft_keyword(Parser *p, స్థిరరాశి అక్షరం *keyword);
సమాసం_ty _PyPegen_soft_keyword_token(Parser *p);
సమాసం_ty Py_Pegen_fstring_మధ్య_token(Parser* p);
Token *Py_Pegen_get_చివరి_nonnwhitespace_token(Parser *);
పూర్ణాంకం _PyPegen_fill_token(Parser *p);
// Return -1 on failure, leaving the buffer and capacity unchanged.
పూర్ణాంకం Py_Pegen_grow_loop_బఫరు(శూన్య ***బఫరు, Py_ssize_t *సామర్థ్యం);
సమాసం_ty _PyPegen_name_token(Parser *p);
సమాసం_ty Py_Pegen_సంఖ్య_token(Parser *p);
శూన్య *_PyPegen_string_token(Parser *p);
Py_ssize_t Py_Pegen_byte_స్థానభ్రంశం_గమ్యం_character_స్థానభ్రంశం_పంక్తి(PyObject *పంక్తి, Py_ssize_t నిలువు_స్థానభ్రంశం, Py_ssize_t చివర_నిలువు_స్థానభ్రంశం);
Py_ssize_t Py_Pegen_byte_స్థానభ్రంశం_గమ్యం_character_స్థానభ్రంశం(PyObject *పంక్తి, Py_ssize_t నిలువు_స్థానభ్రంశం);
Py_ssize_t Py_Pegen_byte_స్థానభ్రంశం_గమ్యం_character_స్థానభ్రంశం_raw(స్థిరరాశి అక్షరం*, Py_ssize_t నిలువు_స్థానభ్రంశం);

రకనిర్వచనం గణన {
    నక్షత్రం_TARGETS,
    DEL_TARGETS,
    FOR_TARGETS,
    SINGLE_TARGETS,
    ATTRIBUTE_OR_SUBSCRIPT_TARGETS
} TARGETS_TYPE;

// Error handling functions and APIs
పూర్ణాంకం _Pypegen_raise_decode_error(Parser *p);
పూర్ణాంకం _Pypegen_tokenizer_error(Parser *p);
శూన్య *Py_Pegen_raise_incomplete_ఆదానం(Parser *p);
శూన్య *_PyPegen_raise_error(Parser *p, PyObject *errtype, పూర్ణాంకం use_mark, స్థిరరాశి అక్షరం *errmsg, ...);
శూన్య *Py_Pegen_raise_error_తెలిసిన_location(Parser *p, PyObject *errtype,
                                          Py_ssize_t lineno, Py_ssize_t నిలువు_స్థానభ్రంశం,
                                          Py_ssize_t చివర_lineno, Py_ssize_t చివర_నిలువు_స్థానభ్రంశం,
                                          స్థిరరాశి అక్షరం *errmsg, va_list va);
శూన్య Pypegen_సమితి_syntax_error(Parser* p, Token* చివరి_token);
శూన్య Pypegen_దొంతర_అధికప్రవాహం(Parser *p);

స్థిర అంతఃవరుస శూన్య *
RAISE_ERROR_తెలిసిన_LOCATION(Parser *p, PyObject *errtype,
                           Py_ssize_t lineno, Py_ssize_t నిలువు_స్థానభ్రంశం,
                           Py_ssize_t చివర_lineno, Py_ssize_t చివర_నిలువు_స్థానభ్రంశం,
                           స్థిరరాశి అక్షరం *errmsg, ...)
{
    va_list va;
    va_ప్రారంభం(va, errmsg);
    Py_ssize_t నిలువు_స్థానభ్రంశం_2 = (నిలువు_స్థానభ్రంశం == CURRENT_POS ? CURRENT_POS : నిలువు_స్థానభ్రంశం + 1);
    Py_ssize_t చివర_నిలువు_స్థానభ్రంశం_2 = (చివర_నిలువు_స్థానభ్రంశం == CURRENT_POS ? CURRENT_POS : చివర_నిలువు_స్థానభ్రంశం + 1);
    Py_Pegen_raise_error_తెలిసిన_location(p, errtype, lineno, నిలువు_స్థానభ్రంశం_2, చివర_lineno, చివర_నిలువు_స్థానభ్రంశం_2, errmsg, va);
    va_చివర(va);
    తిరిగి ఖాళీ;
}
#నిర్వచించు RAISE_SYNTAX_ERROR(సందేశం, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 0, సందేశం, ##__VA_ARGS__)
#నిర్వచించు RAISE_INDENTATION_ERROR(సందేశం, ...) _PyPegen_raise_error(p, PyExc_IndentationError, 0, సందేశం, ##__VA_ARGS__)
#నిర్వచించు RAISE_SYNTAX_ERROR_ON_తదుపరి_TOKEN(సందేశం, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 1, సందేశం, ##__VA_ARGS__)
#నిర్వచించు RAISE_SYNTAX_ERROR_తెలిసిన_RANGE(a, b, సందేశం, ...) \
    RAISE_ERROR_తెలిసిన_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->నిలువు_స్థానభ్రంశం, (b)->చివర_lineno, (b)->చివర_నిలువు_స్థానభ్రంశం, సందేశం, ##__VA_ARGS__)
#నిర్వచించు RAISE_SYNTAX_ERROR_తెలిసిన_LOCATION(a, సందేశం, ...) \
    RAISE_ERROR_తెలిసిన_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->నిలువు_స్థానభ్రంశం, (a)->చివర_lineno, (a)->చివర_నిలువు_స్థానభ్రంశం, సందేశం, ##__VA_ARGS__)
#నిర్వచించు RAISE_SYNTAX_ERROR_STARTING_FROM(a, సందేశం, ...) \
    RAISE_ERROR_తెలిసిన_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->నిలువు_స్థానభ్రంశం, CURRENT_POS, CURRENT_POS, సందేశం, ##__VA_ARGS__)
#నిర్వచించు RAISE_SYNTAX_ERROR_INVALID_లక్ష్యం(type, e) RAISE_SYNTAX_ERROR_INVALID_లక్ష్యం_2(p, type, e)

Py_LOCAL_INLINE(పూర్ణాంకం)
Py_Pegen_టోకెన్లు_are_adjacent(Token *a, Token *b)
{
    తిరిగి (a->చివర_lineno == b->lineno) && (a->చివర_నిలువు_స్థానభ్రంశం == b->నిలువు_స్థానభ్రంశం);
}

Py_LOCAL_INLINE(శూన్య *)
CHECK_CALL(Parser *p, శూన్య *ఫలితం)
{
    ఒకవేళ (ఫలితం == ఖాళీ) {
        ప్రతిజ్ఞా(PyErr_Occurred());
        p->error_indicator = 1;
    }
    తిరిగి ఫలితం;
}

/* This is needed for helper functions that are allowed to
   return NULL without an error. Example: _PyPegen_seq_extract_starred_exprs */
Py_LOCAL_INLINE(శూన్య *)
CHECK_CALL_NULL_ALLOWED(Parser *p, శూన్య *ఫలితం)
{
    ఒకవేళ (ఫలితం == ఖాళీ && PyErr_Occurred()) {
        p->error_indicator = 1;
    }
    తిరిగి ఫలితం;
}

#నిర్వచించు CHECK(type, ఫలితం) ((type) CHECK_CALL(p, ఫలితం))
#నిర్వచించు CHECK_NULL_ALLOWED(type, ఫలితం) ((type) CHECK_CALL_NULL_ALLOWED(p, ఫలితం))

సమాసం_ty Py_Pegen_get_invalid_లక్ష్యం(సమాసం_ty e, TARGETS_TYPE targets_type);
స్థిరరాశి అక్షరం *Py_Pegen_get_సమాసం_name(సమాసం_ty);
Py_LOCAL_INLINE(శూన్య *)
RAISE_SYNTAX_ERROR_INVALID_లక్ష్యం_2(Parser *p, TARGETS_TYPE type, శూన్య *e)
{
    సమాసం_ty invalid_లక్ష్యం = CHECK_NULL_ALLOWED(సమాసం_ty, Py_Pegen_get_invalid_లక్ష్యం(e, type));
    ఒకవేళ (invalid_లక్ష్యం != ఖాళీ) {
        స్థిరరాశి అక్షరం *సందేశం;
        ఒకవేళ (type == నక్షత్రం_TARGETS || type == FOR_TARGETS) {
            సందేశం = "cannot assign to %s";
        }
        లేకపోతే {
            సందేశం = "cannot delete %s";
        }
        తిరిగి RAISE_SYNTAX_ERROR_తెలిసిన_LOCATION(
            invalid_లక్ష్యం,
            సందేశం,
            Py_Pegen_get_సమాసం_name(invalid_లక్ష్యం)
        );
        తిరిగి RAISE_SYNTAX_ERROR_తెలిసిన_LOCATION(invalid_లక్ష్యం, "invalid syntax");
    }
    తిరిగి ఖాళీ;
}

// Action utility functions

శూన్య *_PyPegen_dummy_name(Parser *p, ...);
శూన్య * Py_Pegen_seq_చివరి_అంశం(asdl_seq *seq);
#నిర్వచించు Py_Pegen_చివరి_అంశం(seq, type) ((type)Py_Pegen_seq_చివరి_అంశం((asdl_seq*)seq))
శూన్య * Py_Pegen_seq_మొదటి_అంశం(asdl_seq *seq);
#నిర్వచించు Py_Pegen_మొదటి_అంశం(seq, type) ((type)Py_Pegen_seq_మొదటి_అంశం((asdl_seq*)seq))
#నిర్వచించు వాడని(సమాసం) చేయి { (శూన్య)(సమాసం); } అయితే (0)
#నిర్వచించు EXTRA_సమాసం(తల, తోక) తల->lineno, (తల)->నిలువు_స్థానభ్రంశం, (తోక)->చివర_lineno, (తోక)->చివర_నిలువు_స్థానభ్రంశం, p->arena
#నిర్వచించు EXTRA ప్రారంభం_lineno, ప్రారంభం_నిలువు_స్థానభ్రంశం_2, చివర_lineno_2, చివర_నిలువు_స్థానభ్రంశం_2, p->arena
PyObject *_PyPegen_new_type_comment(Parser *, స్థిరరాశి అక్షరం *);

Py_LOCAL_INLINE(PyObject *)
NEW_TYPE_COMMENT(Parser *p, Token *tc)
{
    ఒకవేళ (tc == ఖాళీ) {
        తిరిగి ఖాళీ;
    }
    స్థిరరాశి అక్షరం *బైట్లు = Py_బైట్లు_As_String(tc->బైట్లు);
    ఒకవేళ (బైట్లు == ఖాళీ) {
        వెళ్ళు error;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, బైట్లు);
    ఒకవేళ (tco == ఖాళీ) {
        వెళ్ళు error;
    }
    తిరిగి tco;
 error:
    p->error_indicator = 1;  // Inline CHECK_CALL
    తిరిగి ఖాళీ;
}

Py_LOCAL_INLINE(శూన్య *)
INVALID_సంస్కరణ_CHECK(Parser *p, పూర్ణాంకం సంస్కరణ, అక్షరం *సందేశం, శూన్య *కణుపు)
{
    ఒకవేళ (కణుపు == ఖాళీ) {
        p->error_indicator = 1;  // Inline CHECK_CALL
        తిరిగి ఖాళీ;
    }
    ఒకవేళ (p->feature_సంస్కరణ < సంస్కరణ) {
        p->error_indicator = 1;
        తిరిగి RAISE_SYNTAX_ERROR("%s only supported in Python 3.%i and greater",
                                  సందేశం, సంస్కరణ);
    }
    తిరిగి కణుపు;
}

#నిర్వచించు CHECK_సంస్కరణ(type, సంస్కరణ, సందేశం, కణుపు) ((type) INVALID_సంస్కరణ_CHECK(p, సంస్కరణ, సందేశం, కణుపు))

arg_ty Py_Pegen_కూడు_type_comment_గమ్యం_arg(Parser *, arg_ty, Token *);
PyObject *_PyPegen_new_identifier(Parser *, స్థిరరాశి అక్షరం *);
asdl_seq *_PyPegen_singleton_seq(Parser *, శూన్య *);
asdl_seq *Py_Pegen_seq_చొప్పించు_లోపల_front(Parser *, శూన్య *, asdl_seq *);
asdl_seq *Py_Pegen_seq_append_గమ్యం_చివర(Parser *, asdl_seq *, శూన్య *);
asdl_seq *_PyPegen_seq_flatten(Parser *, asdl_seq *);
సమాసం_ty Py_Pegen_join_పేర్లు_with_dot(Parser *, సమాసం_ty, సమాసం_ty);
పూర్ణాంకం Py_Pegen_seq_లెక్క_dots(asdl_seq *);
alias_ty Py_Pegen_alias_for_నక్షత్రం(Parser *, పూర్ణాంకం, పూర్ణాంకం, పూర్ణాంకం, పూర్ణాంకం, PyArena *);
asdl_identifier_seq *Py_Pegen_map_పేర్లు_గమ్యం_ids(Parser *, asdl_సమాసం_seq *);
Cmpop_సమాసం_Pair *Py_Pegen_cmpop_సమాసం_pair(Parser *, cmpop_ty, సమాసం_ty);
asdl_int_seq *_PyPegen_get_cmpops(Parser *p, asdl_seq *);
asdl_సమాసం_seq *_PyPegen_get_exprs(Parser *, asdl_seq *);
సమాసం_ty Py_Pegen_సమితి_సమాసం_context(Parser *, సమాసం_ty, సమాసం_context_ty);
సమాసం_ty Py_Pegen_make_లక్ష్యం(Parser *, సమాసం_ty, TARGETS_TYPE);
కీలకం_విలువ_Pair *Py_Pegen_కీలకం_విలువ_pair(Parser *, సమాసం_ty, సమాసం_ty);
asdl_సమాసం_seq *Py_Pegen_get_కీలకాలు(Parser *, asdl_seq *);
asdl_సమాసం_seq *Py_Pegen_get_విలువలు(Parser *, asdl_seq *);
కీలకం_నమూనా_Pair *Py_Pegen_కీలకం_నమూనా_pair(Parser *, సమాసం_ty, నమూనా_ty);
asdl_సమాసం_seq *Py_Pegen_get_నమూనా_కీలకాలు(Parser *, asdl_seq *);
asdl_నమూనా_seq *_PyPegen_get_patterns(Parser *, asdl_seq *);
NameDefaultPair *_PyPegen_name_default_pair(Parser *, arg_ty, సమాసం_ty, Token *);
SlashWithDefault *_PyPegen_slash_with_default(Parser *, asdl_arg_seq *, asdl_seq *);
నక్షత్రం_Etc *Py_Pegen_నక్షత్రం_etc(Parser *, arg_ty, asdl_seq *, arg_ty);
arguments_ty _PyPegen_make_arguments(Parser *, asdl_arg_seq *, SlashWithDefault *,
                                     asdl_arg_seq *, asdl_seq *, నక్షత్రం_Etc *);
arguments_ty Py_Pegen_ఖాళీలు_arguments(Parser *);
సమాసం_ty _PyPegen_template_str(Parser *p, Token *a, asdl_సమాసం_seq *raw_expressions, Token *b);
సమాసం_ty _PyPegen_joined_str(Parser *p, Token *a, asdl_సమాసం_seq *raw_expressions, Token *b);
సమాసం_ty _PyPegen_interpolation(Parser *, సమాసం_ty, Token *, ఫలితం_Token_With_Metadata *, ఫలితం_Token_With_Metadata *, Token *,
                                 పూర్ణాంకం, పూర్ణాంకం, పూర్ణాంకం, పూర్ణాంకం, PyArena *);
సమాసం_ty Py_Pegen_formatted_విలువ(Parser *, సమాసం_ty, Token *, ఫలితం_Token_With_Metadata *, ఫలితం_Token_With_Metadata *, Token *,
                                 పూర్ణాంకం, పూర్ణాంకం, పూర్ణాంకం, పూర్ణాంకం, PyArena *);
AugOperator *_PyPegen_augoperator(Parser*, operator_ty type);
stmt_ty _PyPegen_function_def_decorators(Parser *, asdl_సమాసం_seq *, stmt_ty);
stmt_ty _PyPegen_class_def_decorators(Parser *, asdl_సమాసం_seq *, stmt_ty);
KeywordOrStarred *_PyPegen_keyword_or_starred(Parser *, శూన్య *, పూర్ణాంకం);
asdl_సమాసం_seq *Py_Pegen_seq_వెలికితీయి_starred_exprs(Parser *, asdl_seq *);
asdl_keyword_seq *_PyPegen_seq_delete_starred_exprs(Parser *, asdl_seq *);
సమాసం_ty _PyPegen_collect_call_seqs(Parser *, asdl_సమాసం_seq *, asdl_seq *,
                     పూర్ణాంకం lineno, పూర్ణాంకం నిలువు_స్థానభ్రంశం, పూర్ణాంకం చివర_lineno,
                     పూర్ణాంకం చివర_నిలువు_స్థానభ్రంశం, PyArena *arena);
సమాసం_ty _PyPegen_constant_from_token(Parser* p, Token* tok);
సమాసం_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok);
సమాసం_ty _PyPegen_constant_from_string(Parser* p, Token* tok);
సమాసం_ty _PyPegen_concatenate_tstrings(Parser *p, asdl_సమాసం_seq *, పూర్ణాంకం, పూర్ణాంకం, పూర్ణాంకం, పూర్ణాంకం, PyArena *);
సమాసం_ty Py_Pegen_concatenate_అక్షరశ్రేణులు(Parser *p, asdl_సమాసం_seq *, పూర్ణాంకం, పూర్ణాంకం, పూర్ణాంకం, పూర్ణాంకం, PyArena *);
సమాసం_ty _PyPegen_FetchRawForm(Parser *p, పూర్ణాంకం, పూర్ణాంకం, పూర్ణాంకం, పూర్ణాంకం);
సమాసం_ty _PyPegen_ensure_imaginary(Parser *p, సమాసం_ty);
సమాసం_ty _PyPegen_ensure_real(Parser *p, సమాసం_ty);
asdl_seq *_PyPegen_join_sequences(Parser *, asdl_seq *, asdl_seq *);
పూర్ణాంకం _PyPegen_check_barry_as_flufl(Parser *, Token *);
పూర్ణాంకం _PyPegen_check_legacy_stmt(Parser *p, సమాసం_ty t);
శూన్య *_PyPegen_raise_error_for_missing_comma(Parser *p, సమాసం_ty a, సమాసం_ty b);
ఫలితం_Token_With_Metadata *_PyPegen_check_fstring_conversion(Parser *p, Token *, సమాసం_ty t);
ఫలితం_Token_With_Metadata *Py_Pegen_అమరిక_నిండిన_format_spec(Parser *, Token *, asdl_సమాసం_seq *, పూర్ణాంకం, పూర్ణాంకం,
                                                         పూర్ణాంకం, పూర్ణాంకం, PyArena *);
mod_ty _PyPegen_make_module(Parser *, asdl_stmt_seq *);
శూన్య *_PyPegen_arguments_parsing_error(Parser *, సమాసం_ty);
సమాసం_ty Py_Pegen_get_చివరి_comprehension_అంశం(comprehension_ty comprehension);
శూన్య *Py_Pegen_nonparen_genexp_లోపల_call(Parser *p, సమాసం_ty వాదనలు, asdl_comprehension_seq *comprehensions);
stmt_ty _PyPegen_checked_from_import(Parser *p, asdl_seq *dots, సమాసం_ty module_name,
                                     asdl_alias_seq *పేర్లు, సమాసం_ty lazy_token, పూర్ణాంకం lineno,
                                     పూర్ణాంకం నిలువు_స్థానభ్రంశం, పూర్ణాంకం చివర_lineno, పూర్ణాంకం చివర_నిలువు_స్థానభ్రంశం,
                                     PyArena *arena);
asdl_stmt_seq* Py_Pegen_రిజిస్టరు_stmts(Parser *p, asdl_stmt_seq* stmts);
stmt_ty Py_Pegen_రిజిస్టరు_stmt(Parser *p, stmt_ty s);

// Parser API

Parser *_PyPegen_Parser_New(నిర్మాణం tok_state *, పూర్ణాంకం, పూర్ణాంకం, పూర్ణాంకం, పూర్ణాంకం *, స్థిరరాశి అక్షరం*, PyArena *);
శూన్య _PyPegen_Parser_Free(Parser *);
mod_ty Py_Pegen_నిరంతరం_parser_from_file_సూచిక(FILE *, పూర్ణాంకం, PyObject *, స్థిరరాశి అక్షరం *,
                                    స్థిరరాశి అక్షరం *, స్థిరరాశి అక్షరం *, PyCompilerFlags *, పూర్ణాంకం *, PyObject **,
                                    PyArena *);
శూన్య *Py_Pegen_నిరంతరం_parser(Parser *);
mod_ty Py_Pegen_నిరంతరం_parser_from_string(స్థిరరాశి అక్షరం *, పూర్ణాంకం, PyObject *, PyCompilerFlags *, PyArena *, PyObject *, PyObject **);
asdl_stmt_seq *_PyPegen_interactive_exit(Parser *);

// Generated function in parse.c - function definition in python.gram
శూన్య *_PyPegen_parse(Parser *);

#అయితే_ముగింపు
