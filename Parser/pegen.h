#यदि_अपरिभाषितम् PEGEN_H
#परिभाषय PEGEN_H

#अन्तर्भाव <Python.h>
#अन्तर्भाव <pycore_ast.h>
#अन्तर्भाव <pycore_token.h>
#अन्तर्भाव <pycore_ceval.h>     // _Py_ReachedRecursionLimitWithMargin()
#अन्तर्भाव <pycore_pystate.h>   // _Py_get_machine_stack_pointer()
#अन्तर्भाव <pycore_pythonrun.h> // _PyOS_STACK_MARGIN_BYTES

#अन्तर्भाव "tokenizer/tokenizer.h"

#यदि 0
#परिभाषय PyPARSE_YIELD_IS_KEYWORD        0x0001
#यदि_अन्तः

#परिभाषय PyPARSE_DONT_IMPLY_DEDENT       0x0002

#यदि 0
#परिभाषय PyPARSE_WITH_IS_KEYWORD         0x0003
#परिभाषय PyPARSE_PRINT_IS_FUNCTION       0x0004
#परिभाषय PyPARSE_UNICODE_LITERALS        0x0008
#यदि_अन्तः

#परिभाषय PyPARSE_IGNORE_COOKIE 0x0010
#परिभाषय PyPARSE_BARRY_AS_BDFL 0x0020
#परिभाषय PyPARSE_TYPE_COMMENTS 0x0040
#परिभाषय Py_PARSE_ALLOW_INCOMPLETE_निवेशः 0x0100

#परिभाषय CURRENT_POS (-5)

प्रकारनिर्देश संरचना _memo {
    पूर्णाङ्क type;
    शून्य *ग्रन्थिः;
    पूर्णाङ्क mark;
    संरचना _memo *अग्र;
} Memo;

प्रकारनिर्देश संरचना {
    पूर्णाङ्क type;
    // Text required by grammar actions or helpers; otherwise NULL.
    // An unfinished string token also has NULL text. Its grammar action
    // requests more input only if the token is valid at that position.
    PyObject *अष्टकानि;
    पूर्णाङ्क level;
    पूर्णाङ्क is_raw;
    पूर्णाङ्क lineno, col_विस्थापनम्, अन्तः_lineno, अन्तः_col_विस्थापनम्;
    Memo *memo;
    // Filter over the rule types present in `memo` (bit `type & 63` is set
    // for every entry): lets lookups skip walking the list on definite
    // misses, which are the common case.
    uint64_t memo_आवरणम्;
    PyObject *metadata;
} Token;

प्रकारनिर्देश संरचना {
    ध्रुव वर्ण *str;
    पूर्णाङ्क type;
} KeywordToken;


प्रकारनिर्देश संरचना {
    संरचना {
        पूर्णाङ्क lineno;
        वर्ण *comment;  // The " <tag>" in "# type: ignore <tag>"
    } *वस्तूनि;
    आकार_t आकार;
    आकार_t अङ्कः_वस्तूनि;
} growable_comment_array;

प्रकारनिर्देश संरचना {
    पूर्णाङ्क lineno;
    पूर्णाङ्क col_विस्थापनम्;
    पूर्णाङ्क अन्तः_lineno;
    पूर्णाङ्क अन्तः_col_विस्थापनम्;
} location;

प्रकारनिर्देश संरचना identifier_cache_प्रविष्टिः Identifier_Cache_प्रविष्टिः;

प्रकारनिर्देश संरचना {
    संरचना tok_state *tok;
    Token **tokens;
    पूर्णाङ्क mark;
    पूर्णाङ्क fill, आकार;
    PyArena *arena;
    KeywordToken **keywords;
    वर्ण **soft_keywords;
    पूर्णाङ्क n_keyword_lists;
    पूर्णाङ्क start_नियमः;
    पूर्णाङ्क *errcode;
    अचिह्नित वर्ण parsing_started;
    अचिह्नित वर्ण निवेशः_exhausted;
    PyObject* normalize;
    पूर्णाङ्क starting_lineno;
    पूर्णाङ्क starting_col_विस्थापनम्;
    पूर्णाङ्क error_indicator;
    पूर्णाङ्क flags;
    पूर्णाङ्क feature_संस्करणम्;
    growable_comment_array type_ignore_comments;
    Token *ज्ञातम्_err_token;
    पूर्णाङ्क level;
    पूर्णाङ्क call_invalid_नियमाः;
    पूर्णाङ्क debug;
    location चरमः_stmt_location;
    Identifier_Cache_प्रविष्टिः *identifier_cache;
    PyThreadState *tstate;
    // Cached copy of tstate->c_stack_soft_limit offset by one margin toward
    // the stack top, so the per-rule stack check is a single inline comparison.
    uintptr_t राशिः_soft_सीमा;
} Parser;

// C-stack exhaustion check run on every rule entry. The fast path compares
// the machine stack pointer against the soft limit cached in the Parser;
// only when the stack is nearly exhausted is the full check called.
स्थिर अन्तःस्थ पूर्णाङ्क
Py_Pegen_राशिः_exhausted(Parser *p)
{
#यदि Py_राशिः_GROWS_अधस्तात्
    यदि (Py_get_machine_राशिः_pointer() > p->राशिः_soft_सीमा) {
#अन्यथा
    यदि (Py_get_machine_राशिः_pointer() <= p->राशिः_soft_सीमा) {
#यदि_अन्तः
        प्रतिज्ञा(!Py_Reached_Recursion_सीमा_With_Margin(p->tstate, 1));
        प्रत्यागम 0;
    }
    प्रत्यागम Py_Reached_Recursion_सीमा_With_Margin(p->tstate, 1);
}

प्रकारनिर्देश संरचना {
    cmpop_ty cmpop;
    व्यञ्जकम्_ty व्यञ्जकम्;
} Cmpop_व्यञ्जकम्_Pair;

प्रकारनिर्देश संरचना {
    व्यञ्जकम्_ty कुञ्चिका;
    व्यञ्जकम्_ty मानम्;
} कुञ्चिका_मानम्_Pair;

प्रकारनिर्देश संरचना {
    व्यञ्जकम्_ty कुञ्चिका;
    आदर्शः_ty आदर्शः;
} कुञ्चिका_आदर्शः_Pair;

प्रकारनिर्देश संरचना {
    arg_ty arg;
    व्यञ्जकम्_ty मानम्;
} NameDefaultPair;

प्रकारनिर्देश संरचना {
    asdl_arg_seq *plain_नामानि;
    asdl_seq *नामानि_with_defaults; // asdl_seq* of NameDefaultsPair's
} SlashWithDefault;

प्रकारनिर्देश संरचना {
    arg_ty vararg;
    asdl_seq *kwonlyargs; // asdl_seq* of NameDefaultsPair's
    arg_ty kwarg;
} तारकम्_Etc;

प्रकारनिर्देश संरचना { operator_ty विधा; } AugOperator;
प्रकारनिर्देश संरचना {
    शून्य *element;
    पूर्णाङ्क is_keyword;
} KeywordOrStarred;

प्रकारनिर्देश संरचना {
    शून्य *परिणाम;
    PyObject *metadata;
} परिणाम_Token_With_Metadata;

// Internal parser functions
#यदि defined(Py_DEBUG)
शून्य _PyPegen_clear_memo_statistics(शून्य);
PyObject *_PyPegen_get_memo_statistics(शून्य);
#यदि_अन्तः

पूर्णाङ्क Py_Pegen_निवेशय_memo(Parser *p, पूर्णाङ्क mark, पूर्णाङ्क type, शून्य *ग्रन्थिः);
पूर्णाङ्क _PyPegen_update_memo(Parser *p, पूर्णाङ्क mark, पूर्णाङ्क type, शून्य *ग्रन्थिः);
Memo *Py_Pegen_निवेशय_memo_प्रत्यक्षम्(Parser *p, पूर्णाङ्क mark, पूर्णाङ्क type);
पूर्णाङ्क _PyPegen_is_memoized(Parser *p, पूर्णाङ्क type, शून्य *pres);

पूर्णाङ्क _PyPegen_lookahead(पूर्णाङ्क, शून्य *(func)(Parser *), Parser *);
पूर्णाङ्क Py_Pegen_lookahead_for_व्यञ्जकम्(पूर्णाङ्क, व्यञ्जकम्_ty (func)(Parser *), Parser *);
पूर्णाङ्क _PyPegen_lookahead_for_stmt(पूर्णाङ्क, stmt_ty (func)(Parser *), Parser *);
पूर्णाङ्क _PyPegen_lookahead_with_int(पूर्णाङ्क, Token *(func)(Parser *, पूर्णाङ्क), Parser *, पूर्णाङ्क);
पूर्णाङ्क _PyPegen_lookahead_with_string(पूर्णाङ्क, व्यञ्जकम्_ty (func)(Parser *, ध्रुव वर्ण*), Parser *, ध्रुव वर्ण*);

Token *_PyPegen_expect_token(Parser *p, पूर्णाङ्क type);
शून्य* Py_Pegen_expect_forced_परिणाम(Parser *p, शून्य* परिणाम, ध्रुव वर्ण* expected);
Token *_PyPegen_expect_forced_token(Parser *p, पूर्णाङ्क type, ध्रुव वर्ण* expected);
व्यञ्जकम्_ty _PyPegen_expect_soft_keyword(Parser *p, ध्रुव वर्ण *keyword);
व्यञ्जकम्_ty _PyPegen_soft_keyword_token(Parser *p);
व्यञ्जकम्_ty Py_Pegen_fstring_मध्यः_token(Parser* p);
Token *Py_Pegen_get_चरमः_nonnwhitespace_token(Parser *);
पूर्णाङ्क _PyPegen_fill_token(Parser *p);
// Return -1 on failure, leaving the buffer and capacity unchanged.
पूर्णाङ्क Py_Pegen_grow_loop_बफर(शून्य ***बफर, Py_ssize_t *क्षमता);
व्यञ्जकम्_ty _PyPegen_name_token(Parser *p);
व्यञ्जकम्_ty Py_Pegen_सङ्ख्या_token(Parser *p);
शून्य *_PyPegen_string_token(Parser *p);
Py_ssize_t Py_Pegen_byte_विस्थापनम्_गन्तव्यम्_character_विस्थापनम्_पङ्क्तिः(PyObject *पङ्क्तिः, Py_ssize_t col_विस्थापनम्, Py_ssize_t अन्तः_col_विस्थापनम्);
Py_ssize_t Py_Pegen_byte_विस्थापनम्_गन्तव्यम्_character_विस्थापनम्(PyObject *पङ्क्तिः, Py_ssize_t col_विस्थापनम्);
Py_ssize_t Py_Pegen_byte_विस्थापनम्_गन्तव्यम्_character_विस्थापनम्_raw(ध्रुव वर्ण*, Py_ssize_t col_विस्थापनम्);

प्रकारनिर्देश गणना {
    तारकम्_TARGETS,
    DEL_TARGETS,
    FOR_TARGETS,
    SINGLE_TARGETS,
    ATTRIBUTE_OR_SUBSCRIPT_TARGETS
} TARGETS_TYPE;

// Error handling functions and APIs
पूर्णाङ्क _Pypegen_raise_decode_error(Parser *p);
पूर्णाङ्क _Pypegen_tokenizer_error(Parser *p);
शून्य *Py_Pegen_raise_incomplete_निवेशः(Parser *p);
शून्य *_PyPegen_raise_error(Parser *p, PyObject *errtype, पूर्णाङ्क use_mark, ध्रुव वर्ण *errmsg, ...);
शून्य *Py_Pegen_raise_error_ज्ञातम्_location(Parser *p, PyObject *errtype,
                                          Py_ssize_t lineno, Py_ssize_t col_विस्थापनम्,
                                          Py_ssize_t अन्तः_lineno, Py_ssize_t अन्तः_col_विस्थापनम्,
                                          ध्रुव वर्ण *errmsg, va_list va);
शून्य Pypegen_समुच्चयः_syntax_error(Parser* p, Token* चरमः_token);
शून्य Pypegen_राशिः_अतिप्रवाहः(Parser *p);

स्थिर अन्तःस्थ शून्य *
RAISE_ERROR_ज्ञातम्_LOCATION(Parser *p, PyObject *errtype,
                           Py_ssize_t lineno, Py_ssize_t col_विस्थापनम्,
                           Py_ssize_t अन्तः_lineno, Py_ssize_t अन्तः_col_विस्थापनम्,
                           ध्रुव वर्ण *errmsg, ...)
{
    va_list va;
    va_start(va, errmsg);
    Py_ssize_t col_विस्थापनम्_2 = (col_विस्थापनम् == CURRENT_POS ? CURRENT_POS : col_विस्थापनम् + 1);
    Py_ssize_t अन्तः_col_विस्थापनम्_2 = (अन्तः_col_विस्थापनम् == CURRENT_POS ? CURRENT_POS : अन्तः_col_विस्थापनम् + 1);
    Py_Pegen_raise_error_ज्ञातम्_location(p, errtype, lineno, col_विस्थापनम्_2, अन्तः_lineno, अन्तः_col_विस्थापनम्_2, errmsg, va);
    va_अन्तः(va);
    प्रत्यागम रिक्त;
}
#परिभाषय RAISE_SYNTAX_ERROR(सन्देशः, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 0, सन्देशः, ##__VA_ARGS__)
#परिभाषय RAISE_INDENTATION_ERROR(सन्देशः, ...) _PyPegen_raise_error(p, PyExc_IndentationError, 0, सन्देशः, ##__VA_ARGS__)
#परिभाषय RAISE_SYNTAX_ERROR_ON_अग्र_TOKEN(सन्देशः, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 1, सन्देशः, ##__VA_ARGS__)
#परिभाषय RAISE_SYNTAX_ERROR_ज्ञातम्_RANGE(a, b, सन्देशः, ...) \
    RAISE_ERROR_ज्ञातम्_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_विस्थापनम्, (b)->अन्तः_lineno, (b)->अन्तः_col_विस्थापनम्, सन्देशः, ##__VA_ARGS__)
#परिभाषय RAISE_SYNTAX_ERROR_ज्ञातम्_LOCATION(a, सन्देशः, ...) \
    RAISE_ERROR_ज्ञातम्_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_विस्थापनम्, (a)->अन्तः_lineno, (a)->अन्तः_col_विस्थापनम्, सन्देशः, ##__VA_ARGS__)
#परिभाषय RAISE_SYNTAX_ERROR_STARTING_FROM(a, सन्देशः, ...) \
    RAISE_ERROR_ज्ञातम्_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_विस्थापनम्, CURRENT_POS, CURRENT_POS, सन्देशः, ##__VA_ARGS__)
#परिभाषय RAISE_SYNTAX_ERROR_INVALID_TARGET(type, e) _RAISE_SYNTAX_ERROR_INVALID_TARGET(p, type, e)

Py_LOCAL_INLINE(पूर्णाङ्क)
_PyPegen_tokens_are_adjacent(Token *a, Token *b)
{
    प्रत्यागम (a->अन्तः_lineno == b->lineno) && (a->अन्तः_col_विस्थापनम् == b->col_विस्थापनम्);
}

Py_LOCAL_INLINE(शून्य *)
CHECK_CALL(Parser *p, शून्य *परिणाम)
{
    यदि (परिणाम == रिक्त) {
        प्रतिज्ञा(PyErr_Occurred());
        p->error_indicator = 1;
    }
    प्रत्यागम परिणाम;
}

/* This is needed for helper functions that are allowed to
   return NULL without an error. Example: _PyPegen_seq_extract_starred_exprs */
Py_LOCAL_INLINE(शून्य *)
CHECK_CALL_NULL_ALLOWED(Parser *p, शून्य *परिणाम)
{
    यदि (परिणाम == रिक्त && PyErr_Occurred()) {
        p->error_indicator = 1;
    }
    प्रत्यागम परिणाम;
}

#परिभाषय CHECK(type, परिणाम) ((type) CHECK_CALL(p, परिणाम))
#परिभाषय CHECK_NULL_ALLOWED(type, परिणाम) ((type) CHECK_CALL_NULL_ALLOWED(p, परिणाम))

व्यञ्जकम्_ty _PyPegen_get_invalid_target(व्यञ्जकम्_ty e, TARGETS_TYPE targets_type);
ध्रुव वर्ण *Py_Pegen_get_व्यञ्जकम्_name(व्यञ्जकम्_ty);
Py_LOCAL_INLINE(शून्य *)
_RAISE_SYNTAX_ERROR_INVALID_TARGET(Parser *p, TARGETS_TYPE type, शून्य *e)
{
    व्यञ्जकम्_ty invalid_target = CHECK_NULL_ALLOWED(व्यञ्जकम्_ty, _PyPegen_get_invalid_target(e, type));
    यदि (invalid_target != रिक्त) {
        ध्रुव वर्ण *सन्देशः;
        यदि (type == तारकम्_TARGETS || type == FOR_TARGETS) {
            सन्देशः = "cannot assign to %s";
        }
        अन्यथा {
            सन्देशः = "cannot delete %s";
        }
        प्रत्यागम RAISE_SYNTAX_ERROR_ज्ञातम्_LOCATION(
            invalid_target,
            सन्देशः,
            Py_Pegen_get_व्यञ्जकम्_name(invalid_target)
        );
        प्रत्यागम RAISE_SYNTAX_ERROR_ज्ञातम्_LOCATION(invalid_target, "invalid syntax");
    }
    प्रत्यागम रिक्त;
}

// Action utility functions

शून्य *_PyPegen_dummy_name(Parser *p, ...);
शून्य * Py_Pegen_seq_चरमः_item(asdl_seq *seq);
#परिभाषय Py_Pegen_चरमः_item(seq, type) ((type)Py_Pegen_seq_चरमः_item((asdl_seq*)seq))
शून्य * _PyPegen_seq_first_item(asdl_seq *seq);
#परिभाषय PyPegen_first_item(seq, type) ((type)_PyPegen_seq_first_item((asdl_seq*)seq))
#परिभाषय अप्रयुक्तम्(व्यञ्जकम्) कुरु { (शून्य)(व्यञ्जकम्); } यावत् (0)
#परिभाषय EXTRA_व्यञ्जकम्(शीर्ष, पुच्छ) शीर्ष->lineno, (शीर्ष)->col_विस्थापनम्, (पुच्छ)->अन्तः_lineno, (पुच्छ)->अन्तः_col_विस्थापनम्, p->arena
#परिभाषय EXTRA _start_lineno, start_col_विस्थापनम्_2, अन्तः_lineno_2, अन्तः_col_विस्थापनम्_2, p->arena
PyObject *_PyPegen_new_type_comment(Parser *, ध्रुव वर्ण *);

Py_LOCAL_INLINE(PyObject *)
NEW_TYPE_COMMENT(Parser *p, Token *tc)
{
    यदि (tc == रिक्त) {
        प्रत्यागम रिक्त;
    }
    ध्रुव वर्ण *अष्टकानि = Py_अष्टकानि_As_String(tc->अष्टकानि);
    यदि (अष्टकानि == रिक्त) {
        गच्छ error;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, अष्टकानि);
    यदि (tco == रिक्त) {
        गच्छ error;
    }
    प्रत्यागम tco;
 error:
    p->error_indicator = 1;  // Inline CHECK_CALL
    प्रत्यागम रिक्त;
}

Py_LOCAL_INLINE(शून्य *)
INVALID_संस्करणम्_CHECK(Parser *p, पूर्णाङ्क संस्करणम्, वर्ण *सन्देशः, शून्य *ग्रन्थिः)
{
    यदि (ग्रन्थिः == रिक्त) {
        p->error_indicator = 1;  // Inline CHECK_CALL
        प्रत्यागम रिक्त;
    }
    यदि (p->feature_संस्करणम् < संस्करणम्) {
        p->error_indicator = 1;
        प्रत्यागम RAISE_SYNTAX_ERROR("%s only supported in Python 3.%i and greater",
                                  सन्देशः, संस्करणम्);
    }
    प्रत्यागम ग्रन्थिः;
}

#परिभाषय CHECK_संस्करणम्(type, संस्करणम्, सन्देशः, ग्रन्थिः) ((type) INVALID_संस्करणम्_CHECK(p, संस्करणम्, सन्देशः, ग्रन्थिः))

arg_ty Py_Pegen_सङ्कलय_type_comment_गन्तव्यम्_arg(Parser *, arg_ty, Token *);
PyObject *_PyPegen_new_identifier(Parser *, ध्रुव वर्ण *);
asdl_seq *_PyPegen_singleton_seq(Parser *, शून्य *);
asdl_seq *Py_Pegen_seq_निवेशय_अन्तर्_front(Parser *, शून्य *, asdl_seq *);
asdl_seq *Py_Pegen_seq_append_गन्तव्यम्_अन्तः(Parser *, asdl_seq *, शून्य *);
asdl_seq *_PyPegen_seq_flatten(Parser *, asdl_seq *);
व्यञ्जकम्_ty Py_Pegen_join_नामानि_with_dot(Parser *, व्यञ्जकम्_ty, व्यञ्जकम्_ty);
पूर्णाङ्क Py_Pegen_seq_गणनम्_dots(asdl_seq *);
alias_ty Py_Pegen_alias_for_तारकम्(Parser *, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, PyArena *);
asdl_identifier_seq *Py_Pegen_map_नामानि_गन्तव्यम्_ids(Parser *, asdl_व्यञ्जकम्_seq *);
Cmpop_व्यञ्जकम्_Pair *Py_Pegen_cmpop_व्यञ्जकम्_pair(Parser *, cmpop_ty, व्यञ्जकम्_ty);
asdl_int_seq *_PyPegen_get_cmpops(Parser *p, asdl_seq *);
asdl_व्यञ्जकम्_seq *_PyPegen_get_exprs(Parser *, asdl_seq *);
व्यञ्जकम्_ty Py_Pegen_समुच्चयः_व्यञ्जकम्_context(Parser *, व्यञ्जकम्_ty, व्यञ्जकम्_context_ty);
व्यञ्जकम्_ty _PyPegen_make_target(Parser *, व्यञ्जकम्_ty, TARGETS_TYPE);
कुञ्चिका_मानम्_Pair *Py_Pegen_कुञ्चिका_मानम्_pair(Parser *, व्यञ्जकम्_ty, व्यञ्जकम्_ty);
asdl_व्यञ्जकम्_seq *Py_Pegen_get_कुञ्चिकाः(Parser *, asdl_seq *);
asdl_व्यञ्जकम्_seq *Py_Pegen_get_मानानि(Parser *, asdl_seq *);
कुञ्चिका_आदर्शः_Pair *Py_Pegen_कुञ्चिका_आदर्शः_pair(Parser *, व्यञ्जकम्_ty, आदर्शः_ty);
asdl_व्यञ्जकम्_seq *Py_Pegen_get_आदर्शः_कुञ्चिकाः(Parser *, asdl_seq *);
asdl_आदर्शः_seq *_PyPegen_get_patterns(Parser *, asdl_seq *);
NameDefaultPair *_PyPegen_name_default_pair(Parser *, arg_ty, व्यञ्जकम्_ty, Token *);
SlashWithDefault *_PyPegen_slash_with_default(Parser *, asdl_arg_seq *, asdl_seq *);
तारकम्_Etc *Py_Pegen_तारकम्_etc(Parser *, arg_ty, asdl_seq *, arg_ty);
arguments_ty _PyPegen_make_arguments(Parser *, asdl_arg_seq *, SlashWithDefault *,
                                     asdl_arg_seq *, asdl_seq *, तारकम्_Etc *);
arguments_ty Py_Pegen_रिक्तानि_arguments(Parser *);
व्यञ्जकम्_ty _PyPegen_template_str(Parser *p, Token *a, asdl_व्यञ्जकम्_seq *raw_expressions, Token *b);
व्यञ्जकम्_ty _PyPegen_joined_str(Parser *p, Token *a, asdl_व्यञ्जकम्_seq *raw_expressions, Token *b);
व्यञ्जकम्_ty _PyPegen_interpolation(Parser *, व्यञ्जकम्_ty, Token *, परिणाम_Token_With_Metadata *, परिणाम_Token_With_Metadata *, Token *,
                                 पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, PyArena *);
व्यञ्जकम्_ty Py_Pegen_formatted_मानम्(Parser *, व्यञ्जकम्_ty, Token *, परिणाम_Token_With_Metadata *, परिणाम_Token_With_Metadata *, Token *,
                                 पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, PyArena *);
AugOperator *_PyPegen_augoperator(Parser*, operator_ty type);
stmt_ty _PyPegen_function_def_decorators(Parser *, asdl_व्यञ्जकम्_seq *, stmt_ty);
stmt_ty _PyPegen_class_def_decorators(Parser *, asdl_व्यञ्जकम्_seq *, stmt_ty);
KeywordOrStarred *_PyPegen_keyword_or_starred(Parser *, शून्य *, पूर्णाङ्क);
asdl_व्यञ्जकम्_seq *_PyPegen_seq_extract_starred_exprs(Parser *, asdl_seq *);
asdl_keyword_seq *_PyPegen_seq_delete_starred_exprs(Parser *, asdl_seq *);
व्यञ्जकम्_ty _PyPegen_collect_call_seqs(Parser *, asdl_व्यञ्जकम्_seq *, asdl_seq *,
                     पूर्णाङ्क lineno, पूर्णाङ्क col_विस्थापनम्, पूर्णाङ्क अन्तः_lineno,
                     पूर्णाङ्क अन्तः_col_विस्थापनम्, PyArena *arena);
व्यञ्जकम्_ty _PyPegen_constant_from_token(Parser* p, Token* tok);
व्यञ्जकम्_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok);
व्यञ्जकम्_ty _PyPegen_constant_from_string(Parser* p, Token* tok);
व्यञ्जकम्_ty _PyPegen_concatenate_tstrings(Parser *p, asdl_व्यञ्जकम्_seq *, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, PyArena *);
व्यञ्जकम्_ty Py_Pegen_concatenate_वाक्यानि(Parser *p, asdl_व्यञ्जकम्_seq *, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, PyArena *);
व्यञ्जकम्_ty _PyPegen_FetchRawForm(Parser *p, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क);
व्यञ्जकम्_ty _PyPegen_ensure_imaginary(Parser *p, व्यञ्जकम्_ty);
व्यञ्जकम्_ty _PyPegen_ensure_real(Parser *p, व्यञ्जकम्_ty);
asdl_seq *_PyPegen_join_sequences(Parser *, asdl_seq *, asdl_seq *);
पूर्णाङ्क _PyPegen_check_barry_as_flufl(Parser *, Token *);
पूर्णाङ्क _PyPegen_check_legacy_stmt(Parser *p, व्यञ्जकम्_ty t);
शून्य *_PyPegen_raise_error_for_missing_comma(Parser *p, व्यञ्जकम्_ty a, व्यञ्जकम्_ty b);
परिणाम_Token_With_Metadata *_PyPegen_check_fstring_conversion(Parser *p, Token *, व्यञ्जकम्_ty t);
परिणाम_Token_With_Metadata *Py_Pegen_सज्जता_पूर्णम्_format_spec(Parser *, Token *, asdl_व्यञ्जकम्_seq *, पूर्णाङ्क, पूर्णाङ्क,
                                                         पूर्णाङ्क, पूर्णाङ्क, PyArena *);
mod_ty _PyPegen_make_module(Parser *, asdl_stmt_seq *);
शून्य *_PyPegen_arguments_parsing_error(Parser *, व्यञ्जकम्_ty);
व्यञ्जकम्_ty Py_Pegen_get_चरमः_comprehension_item(comprehension_ty comprehension);
शून्य *Py_Pegen_nonparen_genexp_अन्तर्_call(Parser *p, व्यञ्जकम्_ty तर्काः, asdl_comprehension_seq *comprehensions);
stmt_ty _PyPegen_checked_from_import(Parser *p, asdl_seq *dots, व्यञ्जकम्_ty module_name,
                                     asdl_alias_seq *नामानि, व्यञ्जकम्_ty lazy_token, पूर्णाङ्क lineno,
                                     पूर्णाङ्क col_विस्थापनम्, पूर्णाङ्क अन्तः_lineno, पूर्णाङ्क अन्तः_col_विस्थापनम्,
                                     PyArena *arena);
asdl_stmt_seq* Py_Pegen_पञ्जिकाकोशः_stmts(Parser *p, asdl_stmt_seq* stmts);
stmt_ty Py_Pegen_पञ्जिकाकोशः_stmt(Parser *p, stmt_ty s);

// Parser API

Parser *_PyPegen_Parser_New(संरचना tok_state *, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क *, ध्रुव वर्ण*, PyArena *);
शून्य _PyPegen_Parser_Free(Parser *);
mod_ty Py_Pegen_सततम्_parser_from_file_pointer(FILE *, पूर्णाङ्क, PyObject *, ध्रुव वर्ण *,
                                    ध्रुव वर्ण *, ध्रुव वर्ण *, PyCompilerFlags *, पूर्णाङ्क *, PyObject **,
                                    PyArena *);
शून्य *Py_Pegen_सततम्_parser(Parser *);
mod_ty Py_Pegen_सततम्_parser_from_string(ध्रुव वर्ण *, पूर्णाङ्क, PyObject *, PyCompilerFlags *, PyArena *, PyObject *, PyObject **);
asdl_stmt_seq *_PyPegen_interactive_exit(Parser *);

// Generated function in parse.c - function definition in python.gram
शून्य *_PyPegen_parse(Parser *);

#यदि_अन्तः
