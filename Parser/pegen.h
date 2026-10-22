#ifndef PEGEN_H
#define PEGEN_H

#அந்தர்பாவ <Python.h>
#அந்தர்பாவ <pycore_ast.h>
#அந்தர்பாவ <pycore_token.h>
#அந்தர்பாவ <pycore_ceval.h>     // _Py_ReachedRecursionLimitWithMargin()
#அந்தர்பாவ <pycore_pystate.h>   // _Py_get_machine_stack_pointer()
#அந்தர்பாவ <pycore_pythonrun.h> // _PyOS_STACK_MARGIN_BYTES

#அந்தர்பாவ "tokenizer/tokenizer.h"

#எனில் 0
#define PyPARSE_YIELD_IS_KEYWORD        0x0001
#endif

#define PyPARSE_DONT_IMPLY_DEDENT       0x0002

#எனில் 0
#define PyPARSE_WITH_IS_KEYWORD         0x0003
#define PyPARSE_PRINT_IS_FUNCTION       0x0004
#define PyPARSE_UNICODE_LITERALS        0x0008
#endif

#define PyPARSE_IGNORE_COOKIE 0x0010
#define PyPARSE_BARRY_AS_BDFL 0x0020
#define PyPARSE_TYPE_COMMENTS 0x0040
#define Py_PARSE_ALLOW_INCOMPLETE_உள்ளீடு 0x0100

#define CURRENT_POS (-5)

வகைவரை கட்டமைப்பு _memo {
    முழு type;
    வெறுமை *முனை;
    முழு mark;
    கட்டமைப்பு _memo *next;
} Memo;

வகைவரை கட்டமைப்பு {
    முழு type;
    // Text required by grammar actions or helpers; otherwise NULL.
    // An unfinished string token also has NULL text. Its grammar action
    // requests more input only if the token is valid at that position.
    PyObject *bytes;
    முழு level;
    முழு is_raw;
    முழு lineno, col_offset, end_lineno, end_col_offset;
    Memo *memo;
    // Filter over the rule types present in `memo` (bit `type & 63` is set
    // for every entry): lets lookups skip walking the list on definite
    // misses, which are the common case.
    uint64_t memo_mask;
    PyObject *metadata;
} Token;

வகைவரை கட்டமைப்பு {
    மாறிலி எழுத்து *str;
    முழு type;
} KeywordToken;


வகைவரை கட்டமைப்பு {
    கட்டமைப்பு {
        முழு lineno;
        எழுத்து *comment;  // The " <tag>" in "# type: ignore <tag>"
    } *பொருட்கள்;
    size_t size;
    size_t இலக்கம்_பொருட்கள்;
} growable_comment_array;

வகைவரை கட்டமைப்பு {
    முழு lineno;
    முழு col_offset;
    முழு end_lineno;
    முழு end_col_offset;
} location;

வகைவரை கட்டமைப்பு _identifier_cache_entry IdentifierCacheEntry;

வகைவரை கட்டமைப்பு {
    கட்டமைப்பு tok_state *tok;
    Token **tokens;
    முழு mark;
    முழு fill, size;
    PyArena *arena;
    KeywordToken **keywords;
    எழுத்து **soft_keywords;
    முழு n_keyword_lists;
    முழு start_rule;
    முழு *errcode;
    unsigned எழுத்து parsing_started;
    unsigned எழுத்து உள்ளீடு_exhausted;
    PyObject* normalize;
    முழு starting_lineno;
    முழு starting_col_offset;
    முழு error_indicator;
    முழு flags;
    முழு feature_version;
    growable_comment_array type_ignore_comments;
    Token *known_err_token;
    முழு level;
    முழு call_invalid_rules;
    முழு debug;
    location last_stmt_location;
    IdentifierCacheEntry *identifier_cache;
    PyThreadState *tstate;
    // Cached copy of tstate->c_stack_soft_limit offset by one margin toward
    // the stack top, so the per-rule stack check is a single inline comparison.
    uintptr_t அடுக்கு_soft_limit;
} Parser;

// C-stack exhaustion check run on every rule entry. The fast path compares
// the machine stack pointer against the soft limit cached in the Parser;
// only when the stack is nearly exhausted is the full check called.
நிலையான உள்வரி முழு
Py_Pegen_அடுக்கு_exhausted(Parser *p)
{
#எனில் Py_அடுக்கு_GROWS_DOWN
    எனில் (Py_get_machine_அடுக்கு_pointer() > p->அடுக்கு_soft_limit) {
#இல்லையெனில்
    எனில் (Py_get_machine_அடுக்கு_pointer() <= p->அடுக்கு_soft_limit) {
#endif
        ப்ரதிஜ்ஞா(!_Py_ReachedRecursionLimitWithMargin(p->tstate, 1));
        திரும்பு 0;
    }
    திரும்பு _Py_ReachedRecursionLimitWithMargin(p->tstate, 1);
}

வகைவரை கட்டமைப்பு {
    cmpop_ty cmpop;
    கோவை_ty கோவை;
} Cmpop_கோவை_Pair;

வகைவரை கட்டமைப்பு {
    கோவை_ty சாவி;
    கோவை_ty மதிப்பு;
} சாவி_மதிப்பு_Pair;

வகைவரை கட்டமைப்பு {
    கோவை_ty சாவி;
    pattern_ty pattern;
} சாவி_Pattern_Pair;

வகைவரை கட்டமைப்பு {
    arg_ty arg;
    கோவை_ty மதிப்பு;
} NameDefaultPair;

வகைவரை கட்டமைப்பு {
    asdl_arg_seq *plain_names;
    asdl_seq *names_with_defaults; // asdl_seq* of NameDefaultsPair's
} SlashWithDefault;

வகைவரை கட்டமைப்பு {
    arg_ty vararg;
    asdl_seq *kwonlyargs; // asdl_seq* of NameDefaultsPair's
    arg_ty kwarg;
} StarEtc;

வகைவரை கட்டமைப்பு { operator_ty kind; } AugOperator;
வகைவரை கட்டமைப்பு {
    வெறுமை *element;
    முழு is_keyword;
} KeywordOrStarred;

வகைவரை கட்டமைப்பு {
    வெறுமை *result;
    PyObject *metadata;
} ResultTokenWithMetadata;

// Internal parser functions
#எனில் defined(Py_DEBUG)
வெறுமை _PyPegen_clear_memo_statistics(வெறுமை);
PyObject *_PyPegen_get_memo_statistics(வெறுமை);
#endif

முழு Py_Pegen_செருகு_memo(Parser *p, முழு mark, முழு type, வெறுமை *முனை);
முழு _PyPegen_update_memo(Parser *p, முழு mark, முழு type, வெறுமை *முனை);
Memo *Py_Pegen_செருகு_memo_direct(Parser *p, முழு mark, முழு type);
முழு _PyPegen_is_memoized(Parser *p, முழு type, வெறுமை *pres);

முழு _PyPegen_lookahead(முழு, வெறுமை *(func)(Parser *), Parser *);
முழு Py_Pegen_lookahead_for_கோவை(முழு, கோவை_ty (func)(Parser *), Parser *);
முழு _PyPegen_lookahead_for_stmt(முழு, stmt_ty (func)(Parser *), Parser *);
முழு _PyPegen_lookahead_with_int(முழு, Token *(func)(Parser *, முழு), Parser *, முழு);
முழு _PyPegen_lookahead_with_string(முழு, கோவை_ty (func)(Parser *, மாறிலி எழுத்து*), Parser *, மாறிலி எழுத்து*);

Token *_PyPegen_expect_token(Parser *p, முழு type);
வெறுமை* _PyPegen_expect_forced_result(Parser *p, வெறுமை* result, மாறிலி எழுத்து* expected);
Token *_PyPegen_expect_forced_token(Parser *p, முழு type, மாறிலி எழுத்து* expected);
கோவை_ty _PyPegen_expect_soft_keyword(Parser *p, மாறிலி எழுத்து *keyword);
கோவை_ty _PyPegen_soft_keyword_token(Parser *p);
கோவை_ty Py_Pegen_fstring_நடு_token(Parser* p);
Token *_PyPegen_get_last_nonnwhitespace_token(Parser *);
முழு _PyPegen_fill_token(Parser *p);
// Return -1 on failure, leaving the buffer and capacity unchanged.
முழு _PyPegen_grow_loop_buffer(வெறுமை ***buffer, Py_ssize_t *capacity);
கோவை_ty _PyPegen_name_token(Parser *p);
கோவை_ty Py_Pegen_எண்_token(Parser *p);
வெறுமை *_PyPegen_string_token(Parser *p);
Py_ssize_t Py_Pegen_byte_offset_to_character_offset_வரி(PyObject *வரி, Py_ssize_t col_offset, Py_ssize_t end_col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset(PyObject *வரி, Py_ssize_t col_offset);
Py_ssize_t _PyPegen_byte_offset_to_character_offset_raw(மாறிலி எழுத்து*, Py_ssize_t col_offset);

வகைவரை எண்ணிக்கை {
    STAR_TARGETS,
    DEL_TARGETS,
    FOR_TARGETS,
    SINGLE_TARGETS,
    ATTRIBUTE_OR_SUBSCRIPT_TARGETS
} TARGETS_TYPE;

// Error handling functions and APIs
முழு _Pypegen_raise_decode_error(Parser *p);
முழு _Pypegen_tokenizer_error(Parser *p);
வெறுமை *Py_Pegen_raise_incomplete_உள்ளீடு(Parser *p);
வெறுமை *_PyPegen_raise_error(Parser *p, PyObject *errtype, முழு use_mark, மாறிலி எழுத்து *errmsg, ...);
வெறுமை *_PyPegen_raise_error_known_location(Parser *p, PyObject *errtype,
                                          Py_ssize_t lineno, Py_ssize_t col_offset,
                                          Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                                          மாறிலி எழுத்து *errmsg, va_list va);
வெறுமை _Pypegen_set_syntax_error(Parser* p, Token* last_token);
வெறுமை Pypegen_அடுக்கு_overflow(Parser *p);

நிலையான உள்வரி வெறுமை *
RAISE_ERROR_KNOWN_LOCATION(Parser *p, PyObject *errtype,
                           Py_ssize_t lineno, Py_ssize_t col_offset,
                           Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                           மாறிலி எழுத்து *errmsg, ...)
{
    va_list va;
    va_start(va, errmsg);
    Py_ssize_t _col_offset = (col_offset == CURRENT_POS ? CURRENT_POS : col_offset + 1);
    Py_ssize_t _end_col_offset = (end_col_offset == CURRENT_POS ? CURRENT_POS : end_col_offset + 1);
    _PyPegen_raise_error_known_location(p, errtype, lineno, _col_offset, end_lineno, _end_col_offset, errmsg, va);
    va_end(va);
    திரும்பு வெற்று;
}
#define RAISE_SYNTAX_ERROR(செய்தி, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 0, செய்தி, ##__VA_ARGS__)
#define RAISE_INDENTATION_ERROR(செய்தி, ...) _PyPegen_raise_error(p, PyExc_IndentationError, 0, செய்தி, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_ON_NEXT_TOKEN(செய்தி, ...) _PyPegen_raise_error(p, PyExc_SyntaxError, 1, செய்தி, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_RANGE(a, b, செய்தி, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (b)->end_lineno, (b)->end_col_offset, செய்தி, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_KNOWN_LOCATION(a, செய்தி, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, (a)->end_lineno, (a)->end_col_offset, செய்தி, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_STARTING_FROM(a, செய்தி, ...) \
    RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError, (a)->lineno, (a)->col_offset, CURRENT_POS, CURRENT_POS, செய்தி, ##__VA_ARGS__)
#define RAISE_SYNTAX_ERROR_INVALID_TARGET(type, e) _RAISE_SYNTAX_ERROR_INVALID_TARGET(p, type, e)

Py_LOCAL_INLINE(முழு)
_PyPegen_tokens_are_adjacent(Token *a, Token *b)
{
    திரும்பு (a->end_lineno == b->lineno) && (a->end_col_offset == b->col_offset);
}

Py_LOCAL_INLINE(வெறுமை *)
CHECK_CALL(Parser *p, வெறுமை *result)
{
    எனில் (result == வெற்று) {
        ப்ரதிஜ்ஞா(PyErr_Occurred());
        p->error_indicator = 1;
    }
    திரும்பு result;
}

/* This is needed for helper functions that are allowed to
   return NULL without an error. Example: _PyPegen_seq_extract_starred_exprs */
Py_LOCAL_INLINE(வெறுமை *)
CHECK_CALL_NULL_ALLOWED(Parser *p, வெறுமை *result)
{
    எனில் (result == வெற்று && PyErr_Occurred()) {
        p->error_indicator = 1;
    }
    திரும்பு result;
}

#define CHECK(type, result) ((type) CHECK_CALL(p, result))
#define CHECK_NULL_ALLOWED(type, result) ((type) CHECK_CALL_NULL_ALLOWED(p, result))

கோவை_ty _PyPegen_get_invalid_target(கோவை_ty e, TARGETS_TYPE targets_type);
மாறிலி எழுத்து *Py_Pegen_get_கோவை_name(கோவை_ty);
Py_LOCAL_INLINE(வெறுமை *)
_RAISE_SYNTAX_ERROR_INVALID_TARGET(Parser *p, TARGETS_TYPE type, வெறுமை *e)
{
    கோவை_ty invalid_target = CHECK_NULL_ALLOWED(கோவை_ty, _PyPegen_get_invalid_target(e, type));
    எனில் (invalid_target != வெற்று) {
        மாறிலி எழுத்து *செய்தி;
        எனில் (type == STAR_TARGETS || type == FOR_TARGETS) {
            செய்தி = "cannot assign to %s";
        }
        இல்லையெனில் {
            செய்தி = "cannot delete %s";
        }
        திரும்பு RAISE_SYNTAX_ERROR_KNOWN_LOCATION(
            invalid_target,
            செய்தி,
            Py_Pegen_get_கோவை_name(invalid_target)
        );
        திரும்பு RAISE_SYNTAX_ERROR_KNOWN_LOCATION(invalid_target, "invalid syntax");
    }
    திரும்பு வெற்று;
}

// Action utility functions

வெறுமை *_PyPegen_dummy_name(Parser *p, ...);
வெறுமை * _PyPegen_seq_last_item(asdl_seq *seq);
#define PyPegen_last_item(seq, type) ((type)_PyPegen_seq_last_item((asdl_seq*)seq))
வெறுமை * _PyPegen_seq_first_item(asdl_seq *seq);
#define PyPegen_first_item(seq, type) ((type)_PyPegen_seq_first_item((asdl_seq*)seq))
#define UNUSED(கோவை) செய் { (வெறுமை)(கோவை); } போது (0)
#define EXTRA_கோவை(head, tail) head->lineno, (head)->col_offset, (tail)->end_lineno, (tail)->end_col_offset, p->arena
#define EXTRA _start_lineno, _start_col_offset, _end_lineno, _end_col_offset, p->arena
PyObject *_PyPegen_new_type_comment(Parser *, மாறிலி எழுத்து *);

Py_LOCAL_INLINE(PyObject *)
NEW_TYPE_COMMENT(Parser *p, Token *tc)
{
    எனில் (tc == வெற்று) {
        திரும்பு வெற்று;
    }
    மாறிலி எழுத்து *bytes = PyBytes_AsString(tc->bytes);
    எனில் (bytes == வெற்று) {
        செல் error;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    எனில் (tco == வெற்று) {
        செல் error;
    }
    திரும்பு tco;
 error:
    p->error_indicator = 1;  // Inline CHECK_CALL
    திரும்பு வெற்று;
}

Py_LOCAL_INLINE(வெறுமை *)
INVALID_VERSION_CHECK(Parser *p, முழு version, எழுத்து *செய்தி, வெறுமை *முனை)
{
    எனில் (முனை == வெற்று) {
        p->error_indicator = 1;  // Inline CHECK_CALL
        திரும்பு வெற்று;
    }
    எனில் (p->feature_version < version) {
        p->error_indicator = 1;
        திரும்பு RAISE_SYNTAX_ERROR("%s only supported in Python 3.%i and greater",
                                  செய்தி, version);
    }
    திரும்பு முனை;
}

#define CHECK_VERSION(type, version, செய்தி, முனை) ((type) INVALID_VERSION_CHECK(p, version, செய்தி, முனை))

arg_ty _PyPegen_add_type_comment_to_arg(Parser *, arg_ty, Token *);
PyObject *_PyPegen_new_identifier(Parser *, மாறிலி எழுத்து *);
asdl_seq *_PyPegen_singleton_seq(Parser *, வெறுமை *);
asdl_seq *Py_Pegen_seq_செருகு_in_front(Parser *, வெறுமை *, asdl_seq *);
asdl_seq *_PyPegen_seq_append_to_end(Parser *, asdl_seq *, வெறுமை *);
asdl_seq *_PyPegen_seq_flatten(Parser *, asdl_seq *);
கோவை_ty _PyPegen_join_names_with_dot(Parser *, கோவை_ty, கோவை_ty);
முழு Py_Pegen_seq_மொத்தம்_dots(asdl_seq *);
alias_ty _PyPegen_alias_for_star(Parser *, முழு, முழு, முழு, முழு, PyArena *);
asdl_identifier_seq *_PyPegen_map_names_to_ids(Parser *, asdl_கோவை_seq *);
Cmpop_கோவை_Pair *Py_Pegen_cmpop_கோவை_pair(Parser *, cmpop_ty, கோவை_ty);
asdl_int_seq *_PyPegen_get_cmpops(Parser *p, asdl_seq *);
asdl_கோவை_seq *_PyPegen_get_exprs(Parser *, asdl_seq *);
கோவை_ty Py_Pegen_set_கோவை_context(Parser *, கோவை_ty, கோவை_context_ty);
கோவை_ty _PyPegen_make_target(Parser *, கோவை_ty, TARGETS_TYPE);
சாவி_மதிப்பு_Pair *Py_Pegen_சாவி_மதிப்பு_pair(Parser *, கோவை_ty, கோவை_ty);
asdl_கோவை_seq *Py_Pegen_get_சாவிகள்(Parser *, asdl_seq *);
asdl_கோவை_seq *Py_Pegen_get_மதிப்புகள்(Parser *, asdl_seq *);
சாவி_Pattern_Pair *Py_Pegen_சாவி_pattern_pair(Parser *, கோவை_ty, pattern_ty);
asdl_கோவை_seq *Py_Pegen_get_pattern_சாவிகள்(Parser *, asdl_seq *);
asdl_pattern_seq *_PyPegen_get_patterns(Parser *, asdl_seq *);
NameDefaultPair *_PyPegen_name_default_pair(Parser *, arg_ty, கோவை_ty, Token *);
SlashWithDefault *_PyPegen_slash_with_default(Parser *, asdl_arg_seq *, asdl_seq *);
StarEtc *_PyPegen_star_etc(Parser *, arg_ty, asdl_seq *, arg_ty);
arguments_ty _PyPegen_make_arguments(Parser *, asdl_arg_seq *, SlashWithDefault *,
                                     asdl_arg_seq *, asdl_seq *, StarEtc *);
arguments_ty _PyPegen_empty_arguments(Parser *);
கோவை_ty _PyPegen_template_str(Parser *p, Token *a, asdl_கோவை_seq *raw_expressions, Token *b);
கோவை_ty _PyPegen_joined_str(Parser *p, Token *a, asdl_கோவை_seq *raw_expressions, Token *b);
கோவை_ty _PyPegen_interpolation(Parser *, கோவை_ty, Token *, ResultTokenWithMetadata *, ResultTokenWithMetadata *, Token *,
                                 முழு, முழு, முழு, முழு, PyArena *);
கோவை_ty Py_Pegen_formatted_மதிப்பு(Parser *, கோவை_ty, Token *, ResultTokenWithMetadata *, ResultTokenWithMetadata *, Token *,
                                 முழு, முழு, முழு, முழு, PyArena *);
AugOperator *_PyPegen_augoperator(Parser*, operator_ty type);
stmt_ty _PyPegen_function_def_decorators(Parser *, asdl_கோவை_seq *, stmt_ty);
stmt_ty _PyPegen_class_def_decorators(Parser *, asdl_கோவை_seq *, stmt_ty);
KeywordOrStarred *_PyPegen_keyword_or_starred(Parser *, வெறுமை *, முழு);
asdl_கோவை_seq *_PyPegen_seq_extract_starred_exprs(Parser *, asdl_seq *);
asdl_keyword_seq *_PyPegen_seq_delete_starred_exprs(Parser *, asdl_seq *);
கோவை_ty _PyPegen_collect_call_seqs(Parser *, asdl_கோவை_seq *, asdl_seq *,
                     முழு lineno, முழு col_offset, முழு end_lineno,
                     முழு end_col_offset, PyArena *arena);
கோவை_ty _PyPegen_constant_from_token(Parser* p, Token* tok);
கோவை_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok);
கோவை_ty _PyPegen_constant_from_string(Parser* p, Token* tok);
கோவை_ty _PyPegen_concatenate_tstrings(Parser *p, asdl_கோவை_seq *, முழு, முழு, முழு, முழு, PyArena *);
கோவை_ty _PyPegen_concatenate_strings(Parser *p, asdl_கோவை_seq *, முழு, முழு, முழு, முழு, PyArena *);
கோவை_ty _PyPegen_FetchRawForm(Parser *p, முழு, முழு, முழு, முழு);
கோவை_ty _PyPegen_ensure_imaginary(Parser *p, கோவை_ty);
கோவை_ty _PyPegen_ensure_real(Parser *p, கோவை_ty);
asdl_seq *_PyPegen_join_sequences(Parser *, asdl_seq *, asdl_seq *);
முழு _PyPegen_check_barry_as_flufl(Parser *, Token *);
முழு _PyPegen_check_legacy_stmt(Parser *p, கோவை_ty t);
வெறுமை *_PyPegen_raise_error_for_missing_comma(Parser *p, கோவை_ty a, கோவை_ty b);
ResultTokenWithMetadata *_PyPegen_check_fstring_conversion(Parser *p, Token *, கோவை_ty t);
ResultTokenWithMetadata *_PyPegen_setup_full_format_spec(Parser *, Token *, asdl_கோவை_seq *, முழு, முழு,
                                                         முழு, முழு, PyArena *);
mod_ty _PyPegen_make_module(Parser *, asdl_stmt_seq *);
வெறுமை *_PyPegen_arguments_parsing_error(Parser *, கோவை_ty);
கோவை_ty _PyPegen_get_last_comprehension_item(comprehension_ty comprehension);
வெறுமை *_PyPegen_nonparen_genexp_in_call(Parser *p, கோவை_ty args, asdl_comprehension_seq *comprehensions);
stmt_ty _PyPegen_checked_from_import(Parser *p, asdl_seq *dots, கோவை_ty module_name,
                                     asdl_alias_seq *names, கோவை_ty lazy_token, முழு lineno,
                                     முழு col_offset, முழு end_lineno, முழு end_col_offset,
                                     PyArena *arena);
asdl_stmt_seq* _PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts);
stmt_ty _PyPegen_register_stmt(Parser *p, stmt_ty s);

// Parser API

Parser *_PyPegen_Parser_New(கட்டமைப்பு tok_state *, முழு, முழு, முழு, முழு *, மாறிலி எழுத்து*, PyArena *);
வெறுமை _PyPegen_Parser_Free(Parser *);
mod_ty _PyPegen_run_parser_from_file_pointer(FILE *, முழு, PyObject *, மாறிலி எழுத்து *,
                                    மாறிலி எழுத்து *, மாறிலி எழுத்து *, PyCompilerFlags *, முழு *, PyObject **,
                                    PyArena *);
வெறுமை *_PyPegen_run_parser(Parser *);
mod_ty _PyPegen_run_parser_from_string(மாறிலி எழுத்து *, முழு, PyObject *, PyCompilerFlags *, PyArena *, PyObject *, PyObject **);
asdl_stmt_seq *_PyPegen_interactive_exit(Parser *);

// Generated function in parse.c - function definition in python.gram
வெறுமை *_PyPegen_parse(Parser *);

#endif
