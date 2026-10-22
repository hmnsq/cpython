#include <Python.h>
#include "pycore_pystate.h"         // _PyInterpreterState_GET()
#include "pycore_runtime.h"         // _PyRuntime
#include "pycore_unicodeobject.h"   // _PyUnicode_InternImmortal()
#include "pycore_pyerrors.h"        // _PyErr_EmitSyntaxWarning()

#include "pegen.h"
#include "string_parser.h"          // _PyPegen_decode_string()
#include "lexer/state.h"            // tok_state


void *
_PyPegen_dummy_name(Parser *p, ...)
{
    ལོག &_PyRuntime.parser.dummy_name;
}

/* Creates a single-element asdl_seq* that contains a */
asdl_seq *
_PyPegen_singleton_seq(Parser *p, void *a)
{
    assert(a != NULL);
    asdl_seq *seq = (asdl_seq*)_Py_asdl_generic_seq_new(1, p->arena);
    གལ་སྲིད (!seq) {
        ལོག NULL;
    }
    asdl_seq_SET_UNTYPED(seq, 0, a);
    ལོག seq;
}

/* Creates a copy of seq and prepends a to it */
asdl_seq *
_PyPegen_seq_insert_in_front(Parser *p, void *a, asdl_seq *seq)
{
    assert(a != NULL);
    གལ་སྲིད (!seq) {
        ལོག _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    གལ་སྲིད (!new_seq) {
        ལོག NULL;
    }

    asdl_seq_SET_UNTYPED(new_seq, 0, a);
    དོན་ལུ (Py_ssize_t i = 1, l = asdl_seq_LEN(new_seq); i < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i - 1));
    }
    ལོག new_seq;
}

/* Creates a copy of seq and appends a to it */
asdl_seq *
_PyPegen_seq_append_to_end(Parser *p, asdl_seq *seq, void *a)
{
    assert(a != NULL);
    གལ་སྲིད (!seq) {
        ལོག _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    གལ་སྲིད (!new_seq) {
        ལོག NULL;
    }

    དོན་ལུ (Py_ssize_t i = 0, l = asdl_seq_LEN(new_seq); i + 1 < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i));
    }
    asdl_seq_SET_UNTYPED(new_seq, asdl_seq_LEN(new_seq) - 1, a);
    ལོག new_seq;
}

static Py_ssize_t
_get_flattened_seq_size(asdl_seq *seqs)
{
    Py_ssize_t size = 0;
    དོན་ལུ (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        size += asdl_seq_LEN(inner_seq);
    }
    ལོག size;
}

/* Flattens an asdl_seq* of asdl_seq*s */
asdl_seq *
_PyPegen_seq_flatten(Parser *p, asdl_seq *seqs)
{
    Py_ssize_t flattened_seq_size = _get_flattened_seq_size(seqs);
    assert(flattened_seq_size > 0);

    asdl_seq *flattened_seq = (asdl_seq*)_Py_asdl_generic_seq_new(flattened_seq_size, p->arena);
    གལ་སྲིད (!flattened_seq) {
        ལོག NULL;
    }

    ཧྲིལ flattened_seq_idx = 0;
    དོན་ལུ (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        དོན་ལུ (Py_ssize_t j = 0, li = asdl_seq_LEN(inner_seq); j < li; j++) {
            asdl_seq_SET_UNTYPED(flattened_seq, flattened_seq_idx++, asdl_seq_GET_UNTYPED(inner_seq, j));
        }
    }
    assert(flattened_seq_idx == flattened_seq_size);

    ལོག flattened_seq;
}

void *
_PyPegen_seq_last_item(asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ལོག asdl_seq_GET_UNTYPED(seq, len - 1);
}

void *
_PyPegen_seq_first_item(asdl_seq *seq)
{
    ལོག asdl_seq_GET_UNTYPED(seq, 0);
}

/* Creates a new name of the form <first_name>.<second_name> */
expr_ty
_PyPegen_join_names_with_dot(Parser *p, expr_ty first_name, expr_ty second_name)
{
    assert(first_name != NULL && second_name != NULL);
    PyObject *uni = PyUnicode_FromFormat("%U.%U",
            first_name->v.Name.id, second_name->v.Name.id);
    གལ་སྲིད (!uni) {
        ལོག NULL;
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &uni);
    གལ་སྲིད (_PyArena_AddPyObject(p->arena, uni) < 0) {
        Py_DECREF(uni);
        ལོག NULL;
    }

    ལོག _PyAST_Name(uni, Load, EXTRA_EXPR(first_name, second_name));
}

/* Counts the total number of dots in seq's tokens */
ཧྲིལ
_PyPegen_seq_count_dots(asdl_seq *seq)
{
    ཧྲིལ number_of_dots = 0;
    དོན་ལུ (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        Token *current_expr = asdl_seq_GET_UNTYPED(seq, i);
        switch (current_expr->type) {
            pak_sa ELLIPSIS:
                number_of_dots += 3;
                bha_nga;
            pak_sa DOT:
                number_of_dots += 1;
                bha_nga;
            default:
                Py_UNREACHABLE();
        }
    }

    ལོག number_of_dots;
}

/* Creates an alias with '*' as the identifier name */
alias_ty
_PyPegen_alias_for_star(Parser *p, ཧྲིལ lineno, ཧྲིལ col_offset, ཧྲིལ end_lineno,
                        ཧྲིལ end_col_offset, PyArena *arena) {
    PyObject *str = PyUnicode_InternFromString("*");
    གལ་སྲིད (!str) {
        ལོག NULL;
    }
    གལ་སྲིད (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ལོག NULL;
    }
    ལོག _PyAST_alias(str, NULL, lineno, col_offset, end_lineno, end_col_offset, arena);
}

/* Creates a new asdl_seq* with the identifiers of all the names in seq */
asdl_identifier_seq *
_PyPegen_map_names_to_ids(Parser *p, asdl_expr_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    assert(len > 0);

    asdl_identifier_seq *new_seq = _Py_asdl_identifier_seq_new(len, p->arena);
    གལ་སྲིད (!new_seq) {
        ལོག NULL;
    }
    དོན་ལུ (Py_ssize_t i = 0; i < len; i++) {
        expr_ty e = asdl_seq_GET(seq, i);
        asdl_seq_SET(new_seq, i, e->v.Name.id);
    }
    ལོག new_seq;
}

/* Constructs a CmpopExprPair */
CmpopExprPair *
_PyPegen_cmpop_expr_pair(Parser *p, cmpop_ty cmpop, expr_ty expr)
{
    assert(expr != NULL);
    CmpopExprPair *a = _PyArena_Malloc(p->arena, parimaa_na(CmpopExprPair));
    གལ་སྲིད (!a) {
        ལོག NULL;
    }
    a->cmpop = cmpop;
    a->expr = expr;
    ལོག a;
}

asdl_int_seq *
_PyPegen_get_cmpops(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    assert(len > 0);

    asdl_int_seq *new_seq = _Py_asdl_int_seq_new(len, p->arena);
    གལ་སྲིད (!new_seq) {
        ལོག NULL;
    }
    དོན་ལུ (Py_ssize_t i = 0; i < len; i++) {
        CmpopExprPair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->cmpop);
    }
    ལོག new_seq;
}

asdl_expr_seq *
_PyPegen_get_exprs(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    assert(len > 0);

    asdl_expr_seq *new_seq = _Py_asdl_expr_seq_new(len, p->arena);
    གལ་སྲིད (!new_seq) {
        ལོག NULL;
    }
    དོན་ལུ (Py_ssize_t i = 0; i < len; i++) {
        CmpopExprPair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->expr);
    }
    ལོག new_seq;
}

/* Creates an asdl_seq* where all the elements have been changed to have ctx as context */
static asdl_expr_seq *
_set_seq_context(Parser *p, asdl_expr_seq *seq, expr_context_ty ctx)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    གལ་སྲིད (len == 0) {
        ལོག NULL;
    }

    asdl_expr_seq *new_seq = _Py_asdl_expr_seq_new(len, p->arena);
    གལ་སྲིད (!new_seq) {
        ལོག NULL;
    }
    དོན་ལུ (Py_ssize_t i = 0; i < len; i++) {
        expr_ty e = asdl_seq_GET(seq, i);
        expr_ty new_e = _PyPegen_set_expr_context(p, e, ctx);
        གལ་སྲིད (!new_e) {
            ལོག NULL;
        }
        asdl_seq_SET(new_seq, i, new_e);
    }
    ལོག new_seq;
}

static expr_ty
_set_name_context(Parser *p, expr_ty e, expr_context_ty ctx)
{
    ལོག _PyAST_Name(e->v.Name.id, ctx, EXTRA_EXPR(e, e));
}

static expr_ty
_set_tuple_context(Parser *p, expr_ty e, expr_context_ty ctx)
{
    asdl_expr_seq *seq = _set_seq_context(p, e->v.Tuple.elts, ctx);
    གལ་སྲིད (!seq && PyErr_Occurred()) {
        ལོག NULL;
    }
    ལོག _PyAST_Tuple(seq, ctx, EXTRA_EXPR(e, e));
}

static expr_ty
_set_list_context(Parser *p, expr_ty e, expr_context_ty ctx)
{
    asdl_expr_seq *seq = _set_seq_context(p, e->v.List.elts, ctx);
    གལ་སྲིད (!seq && PyErr_Occurred()) {
        ལོག NULL;
    }
    ལོག _PyAST_List(seq, ctx, EXTRA_EXPR(e, e));
}

static expr_ty
_set_subscript_context(Parser *p, expr_ty e, expr_context_ty ctx)
{
    ལོག _PyAST_Subscript(e->v.Subscript.value, e->v.Subscript.slice,
                            ctx, EXTRA_EXPR(e, e));
}

static expr_ty
_set_attribute_context(Parser *p, expr_ty e, expr_context_ty ctx)
{
    ལོག _PyAST_Attribute(e->v.Attribute.value, e->v.Attribute.attr,
                            ctx, EXTRA_EXPR(e, e));
}

static expr_ty
_set_starred_context(Parser *p, expr_ty e, expr_context_ty ctx)
{
    expr_ty inner = _PyPegen_set_expr_context(p, e->v.Starred.value, ctx);
    གལ་སྲིད (!inner) {
        ལོག NULL;
    }
    ལོག _PyAST_Starred(inner, ctx, EXTRA_EXPR(e, e));
}

/* Creates an `expr_ty` equivalent to `expr` but with `ctx` as context */
expr_ty
_PyPegen_set_expr_context(Parser *p, expr_ty expr, expr_context_ty ctx)
{
    assert(expr != NULL);

    expr_ty new = NULL;
    switch (expr->kind) {
        pak_sa Name_kind:
            new = _set_name_context(p, expr, ctx);
            bha_nga;
        pak_sa Tuple_kind:
            new = _set_tuple_context(p, expr, ctx);
            bha_nga;
        pak_sa List_kind:
            new = _set_list_context(p, expr, ctx);
            bha_nga;
        pak_sa Subscript_kind:
            new = _set_subscript_context(p, expr, ctx);
            bha_nga;
        pak_sa Attribute_kind:
            new = _set_attribute_context(p, expr, ctx);
            bha_nga;
        pak_sa Starred_kind:
            new = _set_starred_context(p, expr, ctx);
            bha_nga;
        default:
            new = expr;
    }
    ལོག new;
}

/* Invalid targets return NULL without raising, allowing parsing to backtrack.
   Copy valid targets to preserve the memoized expression's context. */
expr_ty
_PyPegen_make_target(Parser *p, expr_ty expr, TARGETS_TYPE targets_type)
{
    assert(expr != NULL);
    assert(targets_type != FOR_TARGETS);
    གལ་སྲིད (_PyPegen_get_invalid_target(expr, targets_type) != NULL) {
        ལོག NULL;
    }
    ལོག _PyPegen_set_expr_context(
        p, expr, targets_type == DEL_TARGETS ? Del : Store);
}

/* Constructs a KeyValuePair that is used when parsing a dict's key value pairs */
KeyValuePair *
_PyPegen_key_value_pair(Parser *p, expr_ty key, expr_ty value)
{
    KeyValuePair *a = _PyArena_Malloc(p->arena, parimaa_na(KeyValuePair));
    གལ་སྲིད (!a) {
        ལོག NULL;
    }
    a->key = key;
    a->value = value;
    ལོག a;
}

/* Extracts all keys from an asdl_seq* of KeyValuePair*'s */
asdl_expr_seq *
_PyPegen_get_keys(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_expr_seq *new_seq = _Py_asdl_expr_seq_new(len, p->arena);
    གལ་སྲིད (!new_seq) {
        ལོག NULL;
    }
    དོན་ལུ (Py_ssize_t i = 0; i < len; i++) {
        KeyValuePair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->key);
    }
    ལོག new_seq;
}

/* Extracts all values from an asdl_seq* of KeyValuePair*'s */
asdl_expr_seq *
_PyPegen_get_values(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_expr_seq *new_seq = _Py_asdl_expr_seq_new(len, p->arena);
    གལ་སྲིད (!new_seq) {
        ལོག NULL;
    }
    དོན་ལུ (Py_ssize_t i = 0; i < len; i++) {
        KeyValuePair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->value);
    }
    ལོག new_seq;
}

/* Constructs a KeyPatternPair that is used when parsing mapping & class patterns */
KeyPatternPair *
_PyPegen_key_pattern_pair(Parser *p, expr_ty key, pattern_ty pattern)
{
    KeyPatternPair *a = _PyArena_Malloc(p->arena, parimaa_na(KeyPatternPair));
    གལ་སྲིད (!a) {
        ལོག NULL;
    }
    a->key = key;
    a->pattern = pattern;
    ལོག a;
}

/* Extracts all keys from an asdl_seq* of KeyPatternPair*'s */
asdl_expr_seq *
_PyPegen_get_pattern_keys(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_expr_seq *new_seq = _Py_asdl_expr_seq_new(len, p->arena);
    གལ་སྲིད (!new_seq) {
        ལོག NULL;
    }
    དོན་ལུ (Py_ssize_t i = 0; i < len; i++) {
        KeyPatternPair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->key);
    }
    ལོག new_seq;
}

/* Extracts all patterns from an asdl_seq* of KeyPatternPair*'s */
asdl_pattern_seq *
_PyPegen_get_patterns(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_pattern_seq *new_seq = _Py_asdl_pattern_seq_new(len, p->arena);
    གལ་སྲིད (!new_seq) {
        ལོག NULL;
    }
    དོན་ལུ (Py_ssize_t i = 0; i < len; i++) {
        KeyPatternPair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->pattern);
    }
    ལོག new_seq;
}

/* Constructs a NameDefaultPair */
NameDefaultPair *
_PyPegen_name_default_pair(Parser *p, arg_ty arg, expr_ty value, Token *tc)
{
    NameDefaultPair *a = _PyArena_Malloc(p->arena, parimaa_na(NameDefaultPair));
    གལ་སྲིད (!a) {
        ལོག NULL;
    }
    a->arg = _PyPegen_add_type_comment_to_arg(p, arg, tc);
    གལ་སྲིད (!a->arg) {
        ལོག NULL;
    }
    a->value = value;
    ལོག a;
}

/* Constructs a SlashWithDefault */
SlashWithDefault *
_PyPegen_slash_with_default(Parser *p, asdl_arg_seq *plain_names, asdl_seq *names_with_defaults)
{
    SlashWithDefault *a = _PyArena_Malloc(p->arena, parimaa_na(SlashWithDefault));
    གལ་སྲིད (!a) {
        ལོག NULL;
    }
    a->plain_names = plain_names;
    a->names_with_defaults = names_with_defaults;
    ལོག a;
}

/* Constructs a StarEtc */
StarEtc *
_PyPegen_star_etc(Parser *p, arg_ty vararg, asdl_seq *kwonlyargs, arg_ty kwarg)
{
    StarEtc *a = _PyArena_Malloc(p->arena, parimaa_na(StarEtc));
    གལ་སྲིད (!a) {
        ལོག NULL;
    }
    a->vararg = vararg;
    a->kwonlyargs = kwonlyargs;
    a->kwarg = kwarg;
    ལོག a;
}

asdl_seq *
_PyPegen_join_sequences(Parser *p, asdl_seq *a, asdl_seq *b)
{
    Py_ssize_t first_len = asdl_seq_LEN(a);
    Py_ssize_t second_len = asdl_seq_LEN(b);
    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(first_len + second_len, p->arena);
    གལ་སྲིད (!new_seq) {
        ལོག NULL;
    }

    ཧྲིལ k = 0;
    དོན་ལུ (Py_ssize_t i = 0; i < first_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(a, i));
    }
    དོན་ལུ (Py_ssize_t i = 0; i < second_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(b, i));
    }

    ལོག new_seq;
}

static asdl_arg_seq*
_get_names(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_arg_seq *seq = _Py_asdl_arg_seq_new(len, p->arena);
    གལ་སྲིད (!seq) {
        ལོག NULL;
    }
    དོན་ལུ (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->arg);
    }
    ལོག seq;
}

static asdl_expr_seq *
_get_defaults(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_expr_seq *seq = _Py_asdl_expr_seq_new(len, p->arena);
    གལ་སྲིད (!seq) {
        ལོག NULL;
    }
    དོན་ལུ (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->value);
    }
    ལོག seq;
}

static ཧྲིལ
_make_posonlyargs(Parser *p,
                  asdl_arg_seq *slash_without_default,
                  SlashWithDefault *slash_with_default,
                  asdl_arg_seq **posonlyargs) {
    གལ་སྲིད (slash_without_default != NULL) {
        *posonlyargs = slash_without_default;
    }
    དེ་མིན གལ་སྲིད (slash_with_default != NULL) {
        asdl_arg_seq *slash_with_default_names =
                _get_names(p, slash_with_default->names_with_defaults);
        གལ་སྲིད (!slash_with_default_names) {
            ལོག -1;
        }
        *posonlyargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default->plain_names,
                (asdl_seq*)slash_with_default_names);
    }
    དེ་མིན {
        *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }
    ལོག *posonlyargs == NULL ? -1 : 0;
}

static ཧྲིལ
_make_posargs(Parser *p,
              asdl_arg_seq *plain_names,
              asdl_seq *names_with_default,
              asdl_arg_seq **posargs) {

    གལ་སྲིད (names_with_default != NULL) {
        གལ་སྲིད (plain_names != NULL) {
            asdl_arg_seq *names_with_default_names = _get_names(p, names_with_default);
            གལ་སྲིད (!names_with_default_names) {
                ལོག -1;
            }
            *posargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                    p,(asdl_seq*)plain_names, (asdl_seq*)names_with_default_names);
        }
        དེ་མིན {
            *posargs = _get_names(p, names_with_default);
        }
    }
    དེ་མིན {
        གལ་སྲིད (plain_names != NULL) {
            // With the current grammar, we never get here.
            // If that has changed, remove the assert, and test thoroughly.
            assert(0);
            *posargs = plain_names;
        }
        དེ་མིན {
            *posargs = _Py_asdl_arg_seq_new(0, p->arena);
        }
    }
    ལོག *posargs == NULL ? -1 : 0;
}

static ཧྲིལ
_make_posdefaults(Parser *p,
                  SlashWithDefault *slash_with_default,
                  asdl_seq *names_with_default,
                  asdl_expr_seq **posdefaults) {
    གལ་སྲིད (slash_with_default != NULL && names_with_default != NULL) {
        asdl_expr_seq *slash_with_default_values =
                _get_defaults(p, slash_with_default->names_with_defaults);
        གལ་སྲིད (!slash_with_default_values) {
            ལོག -1;
        }
        asdl_expr_seq *names_with_default_values = _get_defaults(p, names_with_default);
        གལ་སྲིད (!names_with_default_values) {
            ལོག -1;
        }
        *posdefaults = (asdl_expr_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default_values,
                (asdl_seq*)names_with_default_values);
    }
    དེ་མིན གལ་སྲིད (slash_with_default == NULL && names_with_default != NULL) {
        *posdefaults = _get_defaults(p, names_with_default);
    }
    དེ་མིན གལ་སྲིད (slash_with_default != NULL && names_with_default == NULL) {
        *posdefaults = _get_defaults(p, slash_with_default->names_with_defaults);
    }
    དེ་མིན {
        *posdefaults = _Py_asdl_expr_seq_new(0, p->arena);
    }
    ལོག *posdefaults == NULL ? -1 : 0;
}

static ཧྲིལ
_make_kwargs(Parser *p, StarEtc *star_etc,
             asdl_arg_seq **kwonlyargs,
             asdl_expr_seq **kwdefaults) {
    གལ་སྲིད (star_etc != NULL && star_etc->kwonlyargs != NULL) {
        *kwonlyargs = _get_names(p, star_etc->kwonlyargs);
    }
    དེ་མིན {
        *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }

    གལ་སྲིད (*kwonlyargs == NULL) {
        ལོག -1;
    }

    གལ་སྲིད (star_etc != NULL && star_etc->kwonlyargs != NULL) {
        *kwdefaults = _get_defaults(p, star_etc->kwonlyargs);
    }
    དེ་མིན {
        *kwdefaults = _Py_asdl_expr_seq_new(0, p->arena);
    }

    གལ་སྲིད (*kwdefaults == NULL) {
        ལོག -1;
    }

    ལོག 0;
}

/* Constructs an arguments_ty object out of all the parsed constructs in the parameters rule */
arguments_ty
_PyPegen_make_arguments(Parser *p, asdl_arg_seq *slash_without_default,
                        SlashWithDefault *slash_with_default, asdl_arg_seq *plain_names,
                        asdl_seq *names_with_default, StarEtc *star_etc)
{
    asdl_arg_seq *posonlyargs;
    གལ་སྲིད (_make_posonlyargs(p, slash_without_default, slash_with_default, &posonlyargs) == -1) {
        ལོག NULL;
    }

    asdl_arg_seq *posargs;
    གལ་སྲིད (_make_posargs(p, plain_names, names_with_default, &posargs) == -1) {
        ལོག NULL;
    }

    asdl_expr_seq *posdefaults;
    གལ་སྲིད (_make_posdefaults(p,slash_with_default, names_with_default, &posdefaults) == -1) {
        ལོག NULL;
    }

    arg_ty vararg = NULL;
    གལ་སྲིད (star_etc != NULL && star_etc->vararg != NULL) {
        vararg = star_etc->vararg;
    }

    asdl_arg_seq *kwonlyargs;
    asdl_expr_seq *kwdefaults;
    གལ་སྲིད (_make_kwargs(p, star_etc, &kwonlyargs, &kwdefaults) == -1) {
        ལོག NULL;
    }

    arg_ty kwarg = NULL;
    གལ་སྲིད (star_etc != NULL && star_etc->kwarg != NULL) {
        kwarg = star_etc->kwarg;
    }

    ལོག _PyAST_arguments(posonlyargs, posargs, vararg, kwonlyargs,
                            kwdefaults, kwarg, posdefaults, p->arena);
}


/* Constructs an empty arguments_ty object, that gets used when a function accepts no
 * arguments. */
arguments_ty
_PyPegen_empty_arguments(Parser *p)
{
    asdl_arg_seq *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    གལ་སྲིད (!posonlyargs) {
        ལོག NULL;
    }
    asdl_arg_seq *posargs = _Py_asdl_arg_seq_new(0, p->arena);
    གལ་སྲིད (!posargs) {
        ལོག NULL;
    }
    asdl_expr_seq *posdefaults = _Py_asdl_expr_seq_new(0, p->arena);
    གལ་སྲིད (!posdefaults) {
        ལོག NULL;
    }
    asdl_arg_seq *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    གལ་སྲིད (!kwonlyargs) {
        ལོག NULL;
    }
    asdl_expr_seq *kwdefaults = _Py_asdl_expr_seq_new(0, p->arena);
    གལ་སྲིད (!kwdefaults) {
        ལོག NULL;
    }

    ལོག _PyAST_arguments(posonlyargs, posargs, NULL, kwonlyargs,
                            kwdefaults, NULL, posdefaults, p->arena);
}

/* Encapsulates the value of an operator_ty into an AugOperator struct */
AugOperator *
_PyPegen_augoperator(Parser *p, operator_ty kind)
{
    AugOperator *a = _PyArena_Malloc(p->arena, parimaa_na(AugOperator));
    གལ་སྲིད (!a) {
        ལོག NULL;
    }
    a->kind = kind;
    ལོག a;
}

/* Construct a FunctionDef equivalent to function_def, but with decorators */
stmt_ty
_PyPegen_function_def_decorators(Parser *p, asdl_expr_seq *decorators, stmt_ty function_def)
{
    assert(function_def != NULL);
    གལ་སྲིད (function_def->kind == AsyncFunctionDef_kind) {
        ལོག _PyAST_AsyncFunctionDef(
            function_def->v.AsyncFunctionDef.name,
            function_def->v.AsyncFunctionDef.args,
            function_def->v.AsyncFunctionDef.body, decorators,
            function_def->v.AsyncFunctionDef.returns,
            function_def->v.AsyncFunctionDef.type_comment,
            function_def->v.AsyncFunctionDef.type_params,
            function_def->lineno, function_def->col_offset,
            function_def->end_lineno, function_def->end_col_offset, p->arena);
    }

    ལོག _PyAST_FunctionDef(
        function_def->v.FunctionDef.name,
        function_def->v.FunctionDef.args,
        function_def->v.FunctionDef.body, decorators,
        function_def->v.FunctionDef.returns,
        function_def->v.FunctionDef.type_comment,
        function_def->v.FunctionDef.type_params,
        function_def->lineno, function_def->col_offset,
        function_def->end_lineno, function_def->end_col_offset, p->arena);
}

/* Construct a ClassDef equivalent to class_def, but with decorators */
stmt_ty
_PyPegen_class_def_decorators(Parser *p, asdl_expr_seq *decorators, stmt_ty class_def)
{
    assert(class_def != NULL);
    ལོག _PyAST_ClassDef(
        class_def->v.ClassDef.name,
        class_def->v.ClassDef.bases, class_def->v.ClassDef.keywords,
        class_def->v.ClassDef.body, decorators,
        class_def->v.ClassDef.type_params,
        class_def->lineno, class_def->col_offset, class_def->end_lineno,
        class_def->end_col_offset, p->arena);
}

/* Construct a KeywordOrStarred */
KeywordOrStarred *
_PyPegen_keyword_or_starred(Parser *p, void *element, ཧྲིལ is_keyword)
{
    KeywordOrStarred *a = _PyArena_Malloc(p->arena, parimaa_na(KeywordOrStarred));
    གལ་སྲིད (!a) {
        ལོག NULL;
    }
    a->element = element;
    a->is_keyword = is_keyword;
    ལོག a;
}

/* Get the number of starred expressions in an asdl_seq* of KeywordOrStarred*s */
static ཧྲིལ
_seq_number_of_starred_exprs(asdl_seq *seq)
{
    ཧྲིལ n = 0;
    དོན་ལུ (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(seq, i);
        གལ་སྲིད (!k->is_keyword) {
            n++;
        }
    }
    ལོག n;
}

/* Extract the starred expressions of an asdl_seq* of KeywordOrStarred*s */
asdl_expr_seq *
_PyPegen_seq_extract_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    ཧྲིལ new_len = _seq_number_of_starred_exprs(kwargs);
    གལ་སྲིད (new_len == 0) {
        ལོག NULL;
    }
    asdl_expr_seq *new_seq = _Py_asdl_expr_seq_new(new_len, p->arena);
    གལ་སྲིད (!new_seq) {
        ལོག NULL;
    }

    ཧྲིལ idx = 0;
    དོན་ལུ (Py_ssize_t i = 0, len = asdl_seq_LEN(kwargs); i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        གལ་སྲིད (!k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    ལོག new_seq;
}

/* Return a new asdl_seq* with only the keywords in kwargs */
asdl_keyword_seq*
_PyPegen_seq_delete_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    Py_ssize_t len = asdl_seq_LEN(kwargs);
    Py_ssize_t new_len = len - _seq_number_of_starred_exprs(kwargs);
    གལ་སྲིད (new_len == 0) {
        ལོག NULL;
    }
    asdl_keyword_seq *new_seq = _Py_asdl_keyword_seq_new(new_len, p->arena);
    གལ་སྲིད (!new_seq) {
        ལོག NULL;
    }

    ཧྲིལ idx = 0;
    དོན་ལུ (Py_ssize_t i = 0; i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        གལ་སྲིད (k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    ལོག new_seq;
}

expr_ty
_PyPegen_ensure_imaginary(Parser *p, expr_ty exp)
{
    གལ་སྲིད (exp->kind != Constant_kind || !PyComplex_CheckExact(exp->v.Constant.value)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "imaginary number required in complex literal");
        ལོག NULL;
    }
    ལོག exp;
}

expr_ty
_PyPegen_ensure_real(Parser *p, expr_ty exp)
{
    གལ་སྲིད (exp->kind != Constant_kind || PyComplex_CheckExact(exp->v.Constant.value)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "real number required in complex literal");
        ལོག NULL;
    }
    ལོག exp;
}

mod_ty
_PyPegen_make_module(Parser *p, asdl_stmt_seq *a) {
    asdl_type_ignore_seq *type_ignores = NULL;
    Py_ssize_t num = p->type_ignore_comments.num_items;
    གལ་སྲིད (num > 0) {
        // Turn the raw (comment, lineno) pairs into TypeIgnore objects in the arena
        type_ignores = _Py_asdl_type_ignore_seq_new(num, p->arena);
        གལ་སྲིད (type_ignores == NULL) {
            ལོག NULL;
        }
        དོན་ལུ (Py_ssize_t i = 0; i < num; i++) {
            PyObject *tag = _PyPegen_new_type_comment(p, p->type_ignore_comments.items[i].comment);
            གལ་སྲིད (tag == NULL) {
                ལོག NULL;
            }
            type_ignore_ty ti = _PyAST_TypeIgnore(p->type_ignore_comments.items[i].lineno,
                                                  tag, p->arena);
            གལ་སྲིད (ti == NULL) {
                ལོག NULL;
            }
            asdl_seq_SET(type_ignores, i, ti);
        }
    }
    ལོག _PyAST_Module(a, type_ignores, p->arena);
}

PyObject *
_PyPegen_new_type_comment(Parser *p, const var_na *s)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, strlen(s), NULL);
    གལ་སྲིད (res == NULL) {
        ལོག NULL;
    }
    གལ་སྲིད (_PyArena_AddPyObject(p->arena, res) < 0) {
        Py_DECREF(res);
        ལོག NULL;
    }
    ལོག res;
}

arg_ty
_PyPegen_add_type_comment_to_arg(Parser *p, arg_ty a, Token *tc)
{
    གལ་སྲིད (tc == NULL) {
        ལོག a;
    }
    const var_na *bytes = PyBytes_AsString(tc->bytes);
    གལ་སྲིད (bytes == NULL) {
        ལོག NULL;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    གལ་སྲིད (tco == NULL) {
        ལོག NULL;
    }
    ལོག _PyAST_arg(a->arg, a->annotation, tco,
                      a->lineno, a->col_offset, a->end_lineno, a->end_col_offset,
                      p->arena);
}

/* Checks if the NOTEQUAL token is valid given the current parser flags
0 indicates success and nonzero indicates failure (an exception may be set) */
ཧྲིལ
_PyPegen_check_barry_as_flufl(Parser *p, Token* t) {
    assert(t->bytes != NULL);
    assert(t->type == NOTEQUAL);

    const var_na* tok_str = PyBytes_AS_STRING(t->bytes);
    གལ་སྲིད (p->flags & PyPARSE_BARRY_AS_BDFL && strcmp(tok_str, "<>") != 0) {
        RAISE_SYNTAX_ERROR("with Barry as BDFL, use '<>' instead of '!='");
        ལོག -1;
    }
    གལ་སྲིད (!(p->flags & PyPARSE_BARRY_AS_BDFL)) {
        ལོག strcmp(tok_str, "!=");
    }
    ལོག 0;
}

ཧྲིལ
_PyPegen_check_legacy_stmt(Parser *p, expr_ty name) {
    གལ་སྲིད (name->kind != Name_kind) {
        ལོག 0;
    }
    const var_na* candidates[2] = {"print", "exec"};
    དོན་ལུ (ཧྲིལ i=0; i<2; i++) {
        གལ་སྲིད (PyUnicode_CompareWithASCIIString(name->v.Name.id, candidates[i]) == 0) {
            ལོག 1;
        }
    }
    ལོག 0;
}

void *
_PyPegen_raise_error_for_missing_comma(Parser *p, expr_ty a, expr_ty b)
{
    // Don't raise for legacy statements like "print x" or "exec x"
    གལ་སྲིད (_PyPegen_check_legacy_stmt(p, a)) {
        ལོག NULL;
    }
    // Only raise inside parentheses/brackets (level > 0)
    གལ་སྲིད (p->tokens[p->mark - 1]->level == 0) {
        ལོག NULL;
    }
    // For multi-line expressions (like string concatenations), point to the
    // last line instead of the first for a more helpful error message.
    // Use a->col_offset as the starting column since all strings in the
    // concatenation typically share the same indentation.
    གལ་སྲིད (a->end_lineno > a->lineno) {
        ལོག RAISE_ERROR_KNOWN_LOCATION(
            p, PyExc_SyntaxError, a->end_lineno, a->col_offset,
            a->end_lineno, a->end_col_offset,
            "invalid syntax. Perhaps you forgot a comma?"
        );
    }
    ལོག RAISE_ERROR_KNOWN_LOCATION(
        p, PyExc_SyntaxError, a->lineno, a->col_offset,
        b->end_lineno, b->end_col_offset,
        "invalid syntax. Perhaps you forgot a comma?"
    );
}

static ResultTokenWithMetadata *
result_token_with_metadata(Parser *p, void *result, PyObject *metadata)
{
    ResultTokenWithMetadata *res = _PyArena_Malloc(p->arena, parimaa_na(ResultTokenWithMetadata));
    གལ་སྲིད (res == NULL) {
        ལོག NULL;
    }
    res->metadata = metadata;
    res->result = result;
    ལོག res;
}

static var_na
formatted_string_prefix(const Parser *p)
{
    ཧྲིལ nested = 0;
    དོན་ལུ (ཧྲིལ i = p->mark - 1; i >= 0; i--) {
        ཧྲིལ type = p->tokens[i]->type;
        གལ་སྲིད (type == FSTRING_END || type == TSTRING_END) {
            nested++;
        }
        དེ་མིན གལ་སྲིད (type == FSTRING_START || type == TSTRING_START) {
            གལ་སྲིད (nested == 0) {
                ལོག type == TSTRING_START ? 't' : 'f';
            }
            nested--;
        }
    }
    Py_UNREACHABLE();
}

ResultTokenWithMetadata *
_PyPegen_check_fstring_conversion(Parser *p, Token* conv_token, expr_ty conv)
{
    གལ་སྲིད (conv_token->lineno != conv->lineno || conv_token->end_col_offset != conv->col_offset) {
        ལོག RAISE_SYNTAX_ERROR_KNOWN_RANGE(
            conv_token, conv,
            "%c-string: conversion type must come right after the exclamation mark",
            formatted_string_prefix(p)
        );
    }

    Py_UCS4 first = PyUnicode_READ_CHAR(conv->v.Name.id, 0);
    གལ་སྲིད (PyUnicode_GET_LENGTH(conv->v.Name.id) > 1 ||
            !(first == 's' || first == 'r' || first == 'a')) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(conv,
                                            "%c-string: invalid conversion character %R: expected 's', 'r', or 'a'",
                                            formatted_string_prefix(p),
                                            conv->v.Name.id);
        ལོག NULL;
    }

    ལོག result_token_with_metadata(p, conv, conv_token->metadata);
}

ResultTokenWithMetadata *
_PyPegen_setup_full_format_spec(Parser *p, Token *colon, asdl_expr_seq *spec, ཧྲིལ lineno, ཧྲིལ col_offset,
                                ཧྲིལ end_lineno, ཧྲིལ end_col_offset, PyArena *arena)
{
    གལ་སྲིད (!spec) {
        ལོག NULL;
    }

    // This is needed to keep compatibility with 3.11, where an empty format
    // spec is parsed as an *empty* JoinedStr node, instead of having an empty
    // constant in it.
    Py_ssize_t n_items = asdl_seq_LEN(spec);
    Py_ssize_t non_empty_count = 0;
    དོན་ལུ (Py_ssize_t i = 0; i < n_items; i++) {
        expr_ty item = asdl_seq_GET(spec, i);
        non_empty_count += !(item->kind == Constant_kind &&
                             PyUnicode_CheckExact(item->v.Constant.value) &&
                             PyUnicode_GET_LENGTH(item->v.Constant.value) == 0);
    }
    གལ་སྲིད (non_empty_count != n_items) {
        asdl_expr_seq *resized_spec =
            _Py_asdl_expr_seq_new(non_empty_count, p->arena);
        གལ་སྲིད (resized_spec == NULL) {
            ལོག NULL;
        }
        Py_ssize_t j = 0;
        དོན་ལུ (Py_ssize_t i = 0; i < n_items; i++) {
            expr_ty item = asdl_seq_GET(spec, i);
            གལ་སྲིད (item->kind == Constant_kind &&
                PyUnicode_CheckExact(item->v.Constant.value) &&
                PyUnicode_GET_LENGTH(item->v.Constant.value) == 0) {
                continue;
            }
            asdl_seq_SET(resized_spec, j++, item);
        }
        assert(j == non_empty_count);
        spec = resized_spec;
    }
    expr_ty res;
    Py_ssize_t n = asdl_seq_LEN(spec);
    གལ་སྲིད (n == 0 || (n == 1 && asdl_seq_GET(spec, 0)->kind == Constant_kind)) {
        res = _PyAST_JoinedStr(spec, lineno, col_offset, end_lineno,
                                    end_col_offset, p->arena);
    } དེ་མིན {
        res = _PyPegen_concatenate_strings(p, spec,
                             lineno, col_offset, end_lineno,
                             end_col_offset, arena);
    }
    གལ་སྲིད (!res) {
        ལོག NULL;
    }
    ལོག result_token_with_metadata(p, res, colon->metadata);
}

const var_na *
_PyPegen_get_expr_name(expr_ty e)
{
    assert(e != NULL);
    switch (e->kind) {
        pak_sa Attribute_kind:
            ལོག "attribute";
        pak_sa Subscript_kind:
            ལོག "subscript";
        pak_sa Starred_kind:
            ལོག "starred";
        pak_sa Name_kind:
            ལོག "name";
        pak_sa List_kind:
            ལོག "list";
        pak_sa Tuple_kind:
            ལོག "tuple";
        pak_sa Lambda_kind:
            ལོག "lambda";
        pak_sa Call_kind:
            ལོག "function call";
        pak_sa BoolOp_kind:
        pak_sa BinOp_kind:
        pak_sa UnaryOp_kind:
            ལོག "expression";
        pak_sa GeneratorExp_kind:
            ལོག "generator expression";
        pak_sa Yield_kind:
        pak_sa YieldFrom_kind:
            ལོག "yield expression";
        pak_sa Await_kind:
            ལོག "await expression";
        pak_sa ListComp_kind:
            ལོག "list comprehension";
        pak_sa SetComp_kind:
            ལོག "set comprehension";
        pak_sa DictComp_kind:
            ལོག "dict comprehension";
        pak_sa Dict_kind:
            ལོག "dict literal";
        pak_sa Set_kind:
            ལོག "set display";
        pak_sa JoinedStr_kind:
        pak_sa FormattedValue_kind:
            ལོག "f-string expression";
        pak_sa TemplateStr_kind:
        pak_sa Interpolation_kind:
            ལོག "t-string expression";
        pak_sa Constant_kind: {
            PyObject *value = e->v.Constant.value;
            གལ་སྲིད (value == Py_None) {
                ལོག "None";
            }
            གལ་སྲིད (value == Py_False) {
                ལོག "False";
            }
            གལ་སྲིད (value == Py_True) {
                ལོག "True";
            }
            གལ་སྲིད (value == Py_Ellipsis) {
                ལོག "ellipsis";
            }
            ལོག "literal";
        }
        pak_sa Compare_kind:
            ལོག "comparison";
        pak_sa IfExp_kind:
            ལོག "conditional expression";
        pak_sa NamedExpr_kind:
            ལོག "named expression";
        default:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->kind, e->lineno);
            ལོག NULL;
    }
}

expr_ty
_PyPegen_get_last_comprehension_item(comprehension_ty comprehension) {
    གལ་སྲིད (comprehension->ifs == NULL || asdl_seq_LEN(comprehension->ifs) == 0) {
        ལོག comprehension->iter;
    }
    ལོག PyPegen_last_item(comprehension->ifs, expr_ty);
}

expr_ty _PyPegen_collect_call_seqs(Parser *p, asdl_expr_seq *a, asdl_seq *b,
                     ཧྲིལ lineno, ཧྲིལ col_offset, ཧྲིལ end_lineno,
                     ཧྲིལ end_col_offset, PyArena *arena) {
    Py_ssize_t args_len = asdl_seq_LEN(a);
    Py_ssize_t total_len = args_len;

    གལ་སྲིད (b == NULL) {
        ལོག _PyAST_Call(_PyPegen_dummy_name(p), a, NULL, lineno, col_offset,
                        end_lineno, end_col_offset, arena);

    }

    asdl_expr_seq *starreds = _PyPegen_seq_extract_starred_exprs(p, b);
    གལ་སྲིད (!starreds && PyErr_Occurred()) {
        ལོག NULL;
    }

    asdl_keyword_seq *keywords = _PyPegen_seq_delete_starred_exprs(p, b);
    གལ་སྲིད (!keywords && PyErr_Occurred()) {
        ལོག NULL;
    }

    གལ་སྲིད (starreds) {
        total_len += asdl_seq_LEN(starreds);
    }

    asdl_expr_seq *args = _Py_asdl_expr_seq_new(total_len, arena);
    གལ་སྲིད (args == NULL) {
        ལོག NULL;
    }

    Py_ssize_t i = 0;
    དོན་ལུ (i = 0; i < args_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(a, i));
    }
    དོན་ལུ (; i < total_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(starreds, i - args_len));
    }

    ལོག _PyAST_Call(_PyPegen_dummy_name(p), args, keywords, lineno,
                       col_offset, end_lineno, end_col_offset, arena);
}

// AST Error reporting helpers

expr_ty
_PyPegen_get_invalid_target(expr_ty e, TARGETS_TYPE targets_type)
{
    གལ་སྲིད (e == NULL) {
        ལོག NULL;
    }

    གལ་སྲིད (targets_type == SINGLE_TARGETS ||
        targets_type == ATTRIBUTE_OR_SUBSCRIPT_TARGETS) {
        གལ་སྲིད (targets_type == SINGLE_TARGETS && e->kind == Name_kind) {
            ལོག NULL;
        }
        ལོག e->kind == Attribute_kind || e->kind == Subscript_kind ? NULL : e;
    }

#define VISIT_CONTAINER(CONTAINER, TYPE) do { \
        Py_ssize_t len = asdl_seq_LEN((CONTAINER)->v.TYPE.elts);\
        དོན་ལུ (Py_ssize_t i = 0; i < len; i++) {\
            expr_ty other = asdl_seq_GET((CONTAINER)->v.TYPE.elts, i);\
            expr_ty child = _PyPegen_get_invalid_target(other, targets_type);\
            གལ་སྲིད (child != NULL) {\
                ལོག child;\
            }\
        }\
    } སྐབས (0)

    // We only need to visit List and Tuple nodes recursively as those
    // are the only ones that can contain valid names in targets when
    // they are parsed as expressions. Any other kind of expression
    // that is a container (like Sets or Dicts) is directly invalid and
    // we don't need to visit it recursively.

    switch (e->kind) {
        pak_sa List_kind:
            VISIT_CONTAINER(e, List);
            ལོག NULL;
        pak_sa Tuple_kind:
            VISIT_CONTAINER(e, Tuple);
            ལོག NULL;
        pak_sa Starred_kind:
            གལ་སྲིད (targets_type == DEL_TARGETS) {
                ལོག e;
            }
            ལོག _PyPegen_get_invalid_target(e->v.Starred.value, targets_type);
        pak_sa Compare_kind:
            // This is needed, because the `a in b` in `for a in b` gets parsed
            // as a comparison, and so we need to search the left side of the comparison
            // for invalid targets.
            གལ་སྲིད (targets_type == FOR_TARGETS) {
                cmpop_ty cmpop = (cmpop_ty) asdl_seq_GET(e->v.Compare.ops, 0);
                གལ་སྲིད (cmpop == In) {
                    ལོག _PyPegen_get_invalid_target(e->v.Compare.left, targets_type);
                }
                ལོག NULL;
            }
            ལོག e;
        pak_sa Name_kind:
        pak_sa Subscript_kind:
        pak_sa Attribute_kind:
            ལོག NULL;
        default:
            ལོག e;
    }
}

void *_PyPegen_arguments_parsing_error(Parser *p, expr_ty e) {
    ཧྲིལ kwarg_unpacking = 0;
    དོན་ལུ (Py_ssize_t i = 0, l = asdl_seq_LEN(e->v.Call.keywords); i < l; i++) {
        keyword_ty keyword = asdl_seq_GET(e->v.Call.keywords, i);
        གལ་སྲིད (!keyword->arg) {
            kwarg_unpacking = 1;
        }
    }

    const var_na *msg = NULL;
    གལ་སྲིད (kwarg_unpacking) {
        msg = "positional argument follows keyword argument unpacking";
    } དེ་མིན {
        msg = "positional argument follows keyword argument";
    }

    ལོག RAISE_SYNTAX_ERROR(msg);
}

void *
_PyPegen_nonparen_genexp_in_call(Parser *p, expr_ty args, asdl_comprehension_seq *comprehensions)
{
    /* The rule that calls this function is 'args for_if_clauses'.
       For the input f(L, x for x in y), L and x are in args and
       the for is parsed as a for_if_clause. We have to check if
       len <= 1, so that input like dict((a, b) for a, b in x)
       gets successfully parsed and then we pass the last
       argument (x in the above example) as the location of the
       error */
    Py_ssize_t len = asdl_seq_LEN(args->v.Call.args);
    གལ་སྲིད (len <= 1) {
        ལོག NULL;
    }

    comprehension_ty last_comprehension = PyPegen_last_item(comprehensions, comprehension_ty);

    ལོག RAISE_SYNTAX_ERROR_KNOWN_RANGE(
        (expr_ty) asdl_seq_GET(args->v.Call.args, len - 1),
        _PyPegen_get_last_comprehension_item(last_comprehension),
        "Generator expression must be parenthesized"
    );
}

// Fstring stuff

static expr_ty
_PyPegen_decode_fstring_part(Parser* p, ཧྲིལ is_raw, expr_ty constant, Token* token) {
    assert(PyUnicode_CheckExact(constant->v.Constant.value));

    const var_na* bstr = PyUnicode_AsUTF8(constant->v.Constant.value);
    གལ་སྲིད (bstr == NULL) {
        ལོག NULL;
    }

    size_t len;
    གལ་སྲིད (strcmp(bstr, "{{") == 0 || strcmp(bstr, "}}") == 0) {
        len = 1;
    } དེ་མིན {
        len = strlen(bstr);
    }

    is_raw = is_raw || strchr(bstr, '\\') == NULL;
    PyObject *str = _PyPegen_decode_string(p, is_raw, bstr, len, token);
    གལ་སྲིད (str == NULL) {
        _Pypegen_raise_decode_error(p);
        ལོག NULL;
    }
    གལ་སྲིད (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ལོག NULL;
    }
    ལོག _PyAST_Constant(str, NULL, constant->lineno, constant->col_offset,
                           constant->end_lineno, constant->end_col_offset,
                           p->arena);
}

static asdl_expr_seq *
_get_resized_exprs(Parser *p, Token *a, asdl_expr_seq *raw_expressions,
                   Token *b, ཧྲིལ is_tstring)
{
    Py_ssize_t n_items = asdl_seq_LEN(raw_expressions);
    Py_ssize_t total_items = n_items;
    དོན་ལུ (Py_ssize_t i = 0; i < n_items; i++) {
        expr_ty item = asdl_seq_GET(raw_expressions, i);
        གལ་སྲིད (item->kind == JoinedStr_kind) {
            total_items += asdl_seq_LEN(item->v.JoinedStr.values) - 1;
        }
    }

    const var_na* quote_str = PyBytes_AsString(a->bytes);
    གལ་སྲིད (quote_str == NULL) {
        ལོག NULL;
    }
    ཧྲིལ is_raw = strpbrk(quote_str, "rR") != NULL;

    asdl_expr_seq *seq = _Py_asdl_expr_seq_new(total_items, p->arena);
    གལ་སྲིད (seq == NULL) {
        ལོག NULL;
    }

    Py_ssize_t index = 0;
    དོན་ལུ (Py_ssize_t i = 0; i < n_items; i++) {
        expr_ty item = asdl_seq_GET(raw_expressions, i);

        /* Debug expressions arrive as JoinedStr(text, value); flatten them
           into the surrounding string. */
        གལ་སྲིད (item->kind == JoinedStr_kind) {
            asdl_expr_seq *values = item->v.JoinedStr.values;
            གལ་སྲིད (asdl_seq_LEN(values) != 2) {
                PyErr_Format(PyExc_SystemError,
                             is_tstring
                             ? "unexpected TemplateStr node without debug data in t-string at line %d"
                             : "unexpected JoinedStr node without debug data in f-string at line %d",
                             item->lineno);
                ལོག NULL;
            }

            expr_ty first = asdl_seq_GET(values, 0);
            assert(first->kind == Constant_kind);
            asdl_seq_SET(seq, index++, first);

            expr_ty second = asdl_seq_GET(values, 1);
            assert((is_tstring &&
                    second->kind == Interpolation_kind) ||
                   second->kind == FormattedValue_kind);
            asdl_seq_SET(seq, index++, second);

            continue;
        }

        གལ་སྲིད (item->kind == Constant_kind) {
            item = _PyPegen_decode_fstring_part(p, is_raw, item, b);
            གལ་སྲིད (item == NULL) {
                ལོག NULL;
            }

            /* Tokenizer emits string parts even when the underlying string
            might become an empty value (e.g. FSTRING_MIDDLE with the value \\n)
            so we need to check for them and simplify it here. */
            གལ་སྲིད (PyUnicode_CheckExact(item->v.Constant.value)
                && PyUnicode_GET_LENGTH(item->v.Constant.value) == 0) {
                continue;
            }
        }
        asdl_seq_SET(seq, index++, item);
    }

    asdl_expr_seq *resized_exprs;
    གལ་སྲིད (index != total_items) {
        resized_exprs = _Py_asdl_expr_seq_new(index, p->arena);
        གལ་སྲིད (resized_exprs == NULL) {
            ལོག NULL;
        }
        དོན་ལུ (Py_ssize_t i = 0; i < index; i++) {
            asdl_seq_SET(resized_exprs, i, asdl_seq_GET(seq, i));
        }
    }
    དེ་མིན {
        resized_exprs = seq;
    }
    ལོག resized_exprs;
}

expr_ty
_PyPegen_template_str(Parser *p, Token *a, asdl_expr_seq *raw_expressions, Token *b) {

    asdl_expr_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 1);
    གལ་སྲིད (resized_exprs == NULL) {
        ལོག NULL;
    }
    ལོག _PyAST_TemplateStr(resized_exprs, a->lineno, a->col_offset,
                              b->end_lineno, b->end_col_offset,
                              p->arena);
}

expr_ty
_PyPegen_joined_str(Parser *p, Token* a, asdl_expr_seq* raw_expressions, Token*b) {

    asdl_expr_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 0);
    གལ་སྲིད (resized_exprs == NULL) {
        ལོག NULL;
    }
    ལོག _PyAST_JoinedStr(resized_exprs, a->lineno, a->col_offset,
                            b->end_lineno, b->end_col_offset,
                            p->arena);
}

expr_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok) {
    གལ་སྲིད (tok->bytes == NULL) {
        ལོག _PyPegen_raise_incomplete_input(p);
    }
    Py_ssize_t bsize = PyBytes_GET_SIZE(tok->bytes);
    const var_na *bstr = PyBytes_AS_STRING(tok->bytes);

    ཧྲིལ is_raw = tok->is_raw;

    PyObject* str = _PyPegen_decode_string(p, is_raw, bstr, bsize, tok);
    གལ་སྲིད (str == NULL) {
        ལོག NULL;
    }
    གལ་སྲིད (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ལོག NULL;
    }
    ལོག _PyAST_Constant(str, NULL, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

expr_ty _PyPegen_constant_from_token(Parser* p, Token* tok) {
    གལ་སྲིད (tok->bytes == NULL) {
        ལོག _PyPegen_raise_incomplete_input(p);
    }
    const var_na *bstr = PyBytes_AS_STRING(tok->bytes);
    PyObject* str = PyUnicode_FromString(bstr);
    གལ་སྲིད (str == NULL) {
        ལོག NULL;
    }
    གལ་སྲིད (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ལོག NULL;
    }
    ལོག _PyAST_Constant(str, NULL, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

expr_ty _PyPegen_constant_from_string(Parser* p, Token* tok) {
    གལ་སྲིད (tok->bytes == NULL) {
        ལོག _PyPegen_raise_incomplete_input(p);
    }
    const var_na *the_str = PyBytes_AS_STRING(tok->bytes);
    PyObject *s = _PyPegen_parse_string(p, tok);
    གལ་སྲིད (s == NULL) {
        _Pypegen_raise_decode_error(p);
        ལོག NULL;
    }
    གལ་སྲིད (_PyArena_AddPyObject(p->arena, s) < 0) {
        Py_DECREF(s);
        ལོག NULL;
    }
    PyObject *kind = NULL;
    གལ་སྲིད (the_str && the_str[0] == 'u') {
        kind = _PyPegen_new_identifier(p, "u");
        གལ་སྲིད (kind == NULL) {
            ལོག NULL;
        }
    }
    ལོག _PyAST_Constant(s, kind, tok->lineno, tok->col_offset, tok->end_lineno, tok->end_col_offset, p->arena);
}

static ཧྲིལ
_get_interpolation_conversion(Parser *p, Token *debug, ResultTokenWithMetadata *conversion,
                              ResultTokenWithMetadata *format)
{
    གལ་སྲིད (conversion != NULL) {
        expr_ty conversion_expr = (expr_ty) conversion->result;
        assert(conversion_expr->kind == Name_kind);
        Py_UCS4 first = PyUnicode_READ_CHAR(conversion_expr->v.Name.id, 0);
        ལོག Py_SAFE_DOWNCAST(first, Py_UCS4, ཧྲིལ);
    }
    དེ་མིན གལ་སྲིད (debug && !format) {
        /* If no conversion is specified, use !r for debug expressions */
        ལོག (ཧྲིལ)'r';
    }
    ལོག -1;
}

static PyObject *
_strip_interpolation_debug_expr(PyObject *exprstr)
{
    Py_ssize_t len = PyUnicode_GET_LENGTH(exprstr);

    /* Discard whitespace and explicit line continuations after the debug "="
       but preserve whitespace before it. */
    སྐབས (len > 0) {
        ཧྲིལ has_newline = 0;
        སྐབས (len > 0) {
            Py_UCS4 c = PyUnicode_READ_CHAR(exprstr, len - 1);
            གལ་སྲིད (!_PyUnicode_IsWhitespace(c)) {
                bha_nga;
            }
            གལ་སྲིད (c == '\r' || c == '\n') {
                has_newline = 1;
            }
            len--;
        }
        གལ་སྲིད (!has_newline || len == 0 ||
            PyUnicode_READ_CHAR(exprstr, len - 1) != '\\')
        {
            bha_nga;
        }
        len--;
    }

    /* Preserve unexpected metadata instead of dropping source text. */
    གལ་སྲིད (len == 0 || PyUnicode_READ_CHAR(exprstr, len - 1) != '=') {
        ལོག Py_NewRef(exprstr);
    }

    ལོག PyUnicode_Substring(exprstr, 0, len - 1);
}

expr_ty _PyPegen_interpolation(Parser *p, expr_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, ཧྲིལ lineno, ཧྲིལ col_offset,
                                 ཧྲིལ end_lineno, ཧྲིལ end_col_offset, PyArena *arena) {

    ཧྲིལ conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    /* Find the non whitespace token after the "=" */
    ཧྲིལ debug_end_line, debug_end_offset;
    PyObject *debug_metadata;
    constant exprstr;

    གལ་སྲིད (conversion) {
        debug_end_line = ((expr_ty) conversion->result)->lineno;
        debug_end_offset = ((expr_ty) conversion->result)->col_offset;
        debug_metadata = exprstr = conversion->metadata;
    }
    དེ་མིན གལ་སྲིད (format) {
        debug_end_line = ((expr_ty) format->result)->lineno;
        debug_end_offset = ((expr_ty) format->result)->col_offset + 1;
        debug_metadata = exprstr = format->metadata;
    }
    དེ་མིན {
        debug_end_line = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = exprstr = closing_brace->metadata;
    }

    assert(exprstr != NULL);
    PyObject *final_exprstr = debug
        ? _strip_interpolation_debug_expr(exprstr)
        : Py_NewRef(exprstr);
    གལ་སྲིད (!final_exprstr || _PyArena_AddPyObject(arena, final_exprstr) < 0) {
        Py_XDECREF(final_exprstr);
        ལོག NULL;
    }

    expr_ty interpolation = _PyAST_Interpolation(
        expression, final_exprstr, conversion_val, format ? (expr_ty) format->result : NULL,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    གལ་སྲིད (!interpolation || !debug) {
        ལོག interpolation;
    }

    expr_ty debug_text = _PyAST_Constant(debug_metadata, NULL, lineno, col_offset + 1, debug_end_line,
                                            debug_end_offset - 1, p->arena);
    གལ་སྲིད (!debug_text) {
        ལོག NULL;
    }

    asdl_expr_seq *values = _Py_asdl_expr_seq_new(2, arena);
    གལ་སྲིད (!values) {
        ལོག NULL;
    }
    asdl_seq_SET(values, 0, debug_text);
    asdl_seq_SET(values, 1, interpolation);
    ལོག _PyAST_JoinedStr(values, lineno, col_offset, debug_end_line, debug_end_offset, p->arena);
}

expr_ty _PyPegen_formatted_value(Parser *p, expr_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, ཧྲིལ lineno, ཧྲིལ col_offset,
                                 ཧྲིལ end_lineno, ཧྲིལ end_col_offset, PyArena *arena) {
    ཧྲིལ conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    expr_ty formatted_value = _PyAST_FormattedValue(
        expression, conversion_val, format ? (expr_ty) format->result : NULL,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    གལ་སྲིད (!formatted_value || !debug) {
        ལོག formatted_value;
    }

    /* Find the non whitespace token after the "=" */
    ཧྲིལ debug_end_line, debug_end_offset;
    PyObject *debug_metadata;

    གལ་སྲིད (conversion) {
        debug_end_line = ((expr_ty) conversion->result)->lineno;
        debug_end_offset = ((expr_ty) conversion->result)->col_offset;
        debug_metadata = conversion->metadata;
    }
    དེ་མིན གལ་སྲིད (format) {
        debug_end_line = ((expr_ty) format->result)->lineno;
        debug_end_offset = ((expr_ty) format->result)->col_offset + 1;
        debug_metadata = format->metadata;
    }
    དེ་མིན {
        debug_end_line = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = closing_brace->metadata;
    }
    expr_ty debug_text = _PyAST_Constant(debug_metadata, NULL, lineno, col_offset + 1, debug_end_line,
                                            debug_end_offset - 1, p->arena);
    གལ་སྲིད (!debug_text) {
        ལོག NULL;
    }

    asdl_expr_seq *values = _Py_asdl_expr_seq_new(2, arena);
    གལ་སྲིད (!values) {
        ལོག NULL;
    }
    asdl_seq_SET(values, 0, debug_text);
    asdl_seq_SET(values, 1, formatted_value);
    ལོག _PyAST_JoinedStr(values, lineno, col_offset, debug_end_line, debug_end_offset, p->arena);
}

static expr_ty
_build_concatenated_bytes(Parser *p, asdl_expr_seq *strings, ཧྲིལ lineno,
                        ཧྲིལ col_offset, ཧྲིལ end_lineno, ཧྲིལ end_col_offset,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    assert(len > 0);

    /* Bytes literals never get a kind, but just for consistency
        since they are represented as Constant nodes, we'll mirror
        the same behavior as unicode strings for determining the
        kind. */
    PyObject *kind = asdl_seq_GET(strings, 0)->v.Constant.kind;

    Py_ssize_t total = 0;
    དོན་ལུ (Py_ssize_t i = 0; i < len; i++) {
        expr_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.value;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        གལ་སྲིད (part > PY_SSIZE_T_MAX - total) {
            PyErr_NoMemory();
            ལོག NULL;
        }
        total += part;
    }

    PyBytesWriter *writer = PyBytesWriter_Create(total);
    གལ་སྲིད (writer == NULL) {
        ལོག NULL;
    }
    var_na *out = PyBytesWriter_GetData(writer);

    དོན་ལུ (Py_ssize_t i = 0; i < len; i++) {
        expr_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.value;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        གལ་སྲིད (part > 0) {
            memcpy(out, PyBytes_AS_STRING(bytes), part);
            out += part;
        }
    }

    PyObject *res = PyBytesWriter_Finish(writer);
    གལ་སྲིད (res == NULL) {
        ལོག NULL;
    }
    གལ་སྲིད (_PyArena_AddPyObject(arena, res) < 0) {
        Py_DECREF(res);
        ལོག NULL;
    }
    ལོག _PyAST_Constant(res, kind, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

static expr_ty
_build_concatenated_unicode(Parser *p, asdl_expr_seq *strings, ཧྲིལ lineno,
                        ཧྲིལ col_offset, ཧྲིལ end_lineno, ཧྲིལ end_col_offset,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    assert(len > 1);

    expr_ty first = asdl_seq_GET(strings, 0);

    /* When a string is getting concatenated, the kind of the string
        is determined by the first string in the concatenation
        sequence.

        u"abc" "def" -> u"abcdef"
        "abc" u"abc" ->  "abcabc" */
    PyObject *kind = first->v.Constant.kind;

    PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
    གལ་སྲིད (writer == NULL) {
        ལོག NULL;
    }

    དོན་ལུ (Py_ssize_t i = 0; i < len; i++) {
        expr_ty current_elem = asdl_seq_GET(strings, i);
        assert(current_elem->kind == Constant_kind);

        གལ་སྲིད (PyUnicodeWriter_WriteStr(writer,
                                     current_elem->v.Constant.value)) {
            PyUnicodeWriter_Discard(writer);
            ལོག NULL;
        }
    }

    PyObject *final = PyUnicodeWriter_Finish(writer);
    གལ་སྲིད (final == NULL) {
        ལོག NULL;
    }
    གལ་སྲིད (_PyArena_AddPyObject(p->arena, final) < 0) {
        Py_DECREF(final);
        ལོག NULL;
    }
    ལོག _PyAST_Constant(final, kind, lineno, col_offset,
                           end_lineno, end_col_offset, arena);
}

static asdl_expr_seq *
_build_concatenated_str(Parser *p, asdl_expr_seq *strings,
                               ཧྲིལ lineno, ཧྲིལ col_offset, ཧྲིལ end_lineno,
                               ཧྲིལ end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    assert(len > 0);

    Py_ssize_t n_flattened_elements = 0;
    དོན་ལུ (Py_ssize_t i = 0; i < len; i++) {
        expr_ty elem = asdl_seq_GET(strings, i);
        switch(elem->kind) {
            pak_sa JoinedStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.JoinedStr.values);
                bha_nga;
            pak_sa TemplateStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.TemplateStr.values);
                bha_nga;
            default:
                n_flattened_elements++;
                bha_nga;
        }
    }


    asdl_expr_seq* flattened = _Py_asdl_expr_seq_new(n_flattened_elements, p->arena);
    གལ་སྲིད (flattened == NULL) {
        ལོག NULL;
    }

    /* build flattened list */
    Py_ssize_t current_pos = 0;
    དོན་ལུ (Py_ssize_t i = 0; i < len; i++) {
        expr_ty elem = asdl_seq_GET(strings, i);
        switch(elem->kind) {
            pak_sa JoinedStr_kind:
                དོན་ལུ (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.JoinedStr.values); j++) {
                    expr_ty subvalue = asdl_seq_GET(elem->v.JoinedStr.values, j);
                    གལ་སྲིད (subvalue == NULL) {
                        ལོག NULL;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                bha_nga;
            pak_sa TemplateStr_kind:
                དོན་ལུ (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.TemplateStr.values); j++) {
                    expr_ty subvalue = asdl_seq_GET(elem->v.TemplateStr.values, j);
                    གལ་སྲིད (subvalue == NULL) {
                        ལོག NULL;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                bha_nga;
            default:
                asdl_seq_SET(flattened, current_pos++, elem);
                bha_nga;
        }
    }

    /* calculate folded element count */
    Py_ssize_t n_elements = 0;
    ཧྲིལ prev_is_constant = 0;
    དོན་ལུ (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        expr_ty elem = asdl_seq_GET(flattened, i);

        /* The concatenation of a FormattedValue and an empty Constant should
           lead to the FormattedValue itself. Thus, we will not take any empty
           constants into account, just as in `_PyPegen_joined_str` */
        གལ་སྲིད (elem->kind == Constant_kind &&
            PyUnicode_CheckExact(elem->v.Constant.value) &&
            PyUnicode_GET_LENGTH(elem->v.Constant.value) == 0)
            continue;

        གལ་སྲིད (!prev_is_constant || elem->kind != Constant_kind) {
            n_elements++;
        }
        prev_is_constant = elem->kind == Constant_kind;
    }

    asdl_expr_seq* values = _Py_asdl_expr_seq_new(n_elements, p->arena);
    གལ་སྲིད (values == NULL) {
        ལོག NULL;
    }

    /* build folded list */
    current_pos = 0;
    དོན་ལུ (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        expr_ty elem = asdl_seq_GET(flattened, i);

        /* if the current elem and the following are constants,
           fold them and all consequent constants */
        གལ་སྲིད (elem->kind == Constant_kind) {
            གལ་སྲིད (i + 1 < n_flattened_elements &&
                asdl_seq_GET(flattened, i + 1)->kind == Constant_kind) {
                expr_ty first_elem = elem;

                /* When a string is getting concatenated, the kind of the string
                   is determined by the first string in the concatenation
                   sequence.

                   u"abc" "def" -> u"abcdef"
                   "abc" u"abc" ->  "abcabc" */
                PyObject *kind = elem->v.Constant.kind;

                PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
                གལ་སྲིད (writer == NULL) {
                    ལོག NULL;
                }
                expr_ty last_elem = elem;
                Py_ssize_t j;
                དོན་ལུ (j = i; j < n_flattened_elements; j++) {
                    expr_ty current_elem = asdl_seq_GET(flattened, j);
                    གལ་སྲིད (current_elem->kind == Constant_kind) {
                        གལ་སྲིད (PyUnicodeWriter_WriteStr(writer,
                                                     current_elem->v.Constant.value)) {
                            PyUnicodeWriter_Discard(writer);
                            ལོག NULL;
                        }
                        last_elem = current_elem;
                    } དེ་མིན {
                        bha_nga;
                    }
                }
                i = j - 1;

                PyObject *concat_str = PyUnicodeWriter_Finish(writer);
                གལ་སྲིད (concat_str == NULL) {
                    ལོག NULL;
                }
                གལ་སྲིད (_PyArena_AddPyObject(p->arena, concat_str) < 0) {
                    Py_DECREF(concat_str);
                    ལོག NULL;
                }
                elem = _PyAST_Constant(concat_str, kind, first_elem->lineno,
                                       first_elem->col_offset,
                                       last_elem->end_lineno,
                                       last_elem->end_col_offset, p->arena);
                གལ་སྲིད (elem == NULL) {
                    ལོག NULL;
                }
            }

            /* Drop all empty contanst strings */
            གལ་སྲིད (PyUnicode_CheckExact(elem->v.Constant.value) &&
                PyUnicode_GET_LENGTH(elem->v.Constant.value) == 0) {
                continue;
            }
        }

        asdl_seq_SET(values, current_pos++, elem);
    }

    assert(current_pos == n_elements);
    ལོག values;
}

static expr_ty
_build_concatenated_joined_str(Parser *p, asdl_expr_seq *strings,
                               ཧྲིལ lineno, ཧྲིལ col_offset, ཧྲིལ end_lineno,
                               ཧྲིལ end_col_offset, PyArena *arena)
{
    asdl_expr_seq *values = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    གལ་སྲིད (!values) {
        ལོག NULL;
    }
    ལོག _PyAST_JoinedStr(values, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

expr_ty
_PyPegen_concatenate_tstrings(Parser *p, asdl_expr_seq *strings,
                               ཧྲིལ lineno, ཧྲིལ col_offset, ཧྲིལ end_lineno,
                               ཧྲིལ end_col_offset, PyArena *arena)
{
    asdl_expr_seq *values = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    གལ་སྲིད (!values) {
        ལོག NULL;
    }
    ལོག _PyAST_TemplateStr(values, lineno, col_offset, end_lineno,
        end_col_offset, arena);
}

expr_ty
_PyPegen_concatenate_strings(Parser *p, asdl_expr_seq *strings,
                             ཧྲིལ lineno, ཧྲིལ col_offset, ཧྲིལ end_lineno,
                             ཧྲིལ end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    assert(len > 0);

    ཧྲིལ f_string_found = 0;
    ཧྲིལ unicode_string_found = 0;
    ཧྲིལ bytes_found = 0;

    Py_ssize_t i = 0;
    དོན་ལུ (i = 0; i < len; i++) {
        expr_ty elem = asdl_seq_GET(strings, i);
        switch(elem->kind) {
            pak_sa Constant_kind:
                གལ་སྲིད (PyBytes_CheckExact(elem->v.Constant.value)) {
                    bytes_found = 1;
                } དེ་མིན {
                    unicode_string_found = 1;
                }
                bha_nga;
            pak_sa JoinedStr_kind:
                f_string_found = 1;
                bha_nga;
            pak_sa TemplateStr_kind:
                // python.gram handles this; we should never get here
                assert(0);
                bha_nga;
            default:
                f_string_found = 1;
                bha_nga;
        }
    }

    // Cannot mix unicode and bytes
    གལ་སྲིད ((unicode_string_found || f_string_found) && bytes_found) {
        RAISE_SYNTAX_ERROR("cannot mix bytes and nonbytes literals");
        ལོག NULL;
    }

    // If it's only bytes or only unicode string, do a simple concat
    གལ་སྲིད (!f_string_found) {
        གལ་སྲིད (len == 1) {
            ལོག asdl_seq_GET(strings, 0);
        }
        དེ་མིན གལ་སྲིད (bytes_found) {
            ལོག _build_concatenated_bytes(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
        དེ་མིན {
            ལོག _build_concatenated_unicode(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
    }

    ལོག _build_concatenated_joined_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
}

static ཧྲིལ
_warn_relative_import_of_lazy(Parser *p, asdl_seq *dots, expr_ty module)
{
    // Warn about `from . lazy import x`: the whitespace between the dots and
    // the module name is insignificant, so this is parsed exactly like
    // `from .lazy import x` (an import of the relative module "lazy"), but it
    // is most likely a transposition of `lazy from . import x` (PEP 810).
    གལ་སྲིད (p->call_invalid_rules) {
        ལོག 0;
    }

    // Only fire if there is whitespace between the last dot and the name,
    // i.e. not for the common `from .lazy import x` spelling.
    Token *last_dot = asdl_seq_GET_UNTYPED(dots, asdl_seq_LEN(dots) - 1);
    གལ་སྲིད (
        last_dot->end_lineno == module->lineno
        && last_dot->end_col_offset == module->col_offset
    ) {
        ལོག 0;
    }

    ཧྲིལ count = _PyPegen_seq_count_dots(dots);
    var_na *buf = PyMem_RawMalloc(count + 1);
    གལ་སྲིད (buf == NULL) {
        PyErr_NoMemory();
        ལོག -1;
    }
    memset(buf, '.', count);
    buf[count] = '\0';

    PyObject *msg = PyUnicode_FromFormat(
        "'from %s lazy import' is the same as 'from %slazy import'; "
        "did you mean 'lazy from %s import'?",
        buf, buf, buf);
    PyMem_RawFree(buf);
    གལ་སྲིད (msg == NULL) {
        ལོག -1;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    ཧྲིལ res = _PyErr_EmitSyntaxWarning(msg,
                                       info.filename,
                                       module->lineno,
                                       module->col_offset + 1,
                                       module->end_lineno,
                                       module->end_col_offset + 1,
                                       info.module);
    Py_DECREF(msg);
    ལོག res;
}

stmt_ty
_PyPegen_checked_from_import(Parser *p, asdl_seq *dots, expr_ty module_name,
                             asdl_alias_seq *names, expr_ty lazy_token, ཧྲིལ lineno,
                             ཧྲིལ col_offset, ཧྲིལ end_lineno, ཧྲིལ end_col_offset,
                             PyArena *arena)
{
    identifier module = module_name->v.Name.id;
    ཧྲིལ level = _PyPegen_seq_count_dots(dots);
    གལ་སྲིད (level == 0 && PyUnicode_CompareWithASCIIString(module, "__future__") == 0) {
        གལ་སྲིད (lazy_token) {
            RAISE_SYNTAX_ERROR_KNOWN_LOCATION(lazy_token,
                "lazy from __future__ import is not allowed");
            ལོག NULL;
        }
        དོན་ལུ (Py_ssize_t i = 0; i < asdl_seq_LEN(names); i++) {
            alias_ty alias = asdl_seq_GET(names, i);
            གལ་སྲིད (PyUnicode_CompareWithASCIIString(alias->name, "barry_as_FLUFL") == 0) {
                p->flags |= PyPARSE_BARRY_AS_BDFL;
                p->tok->barry_as_bdfl = 1;
            }
        }
    }
    དེ་མིན གལ་སྲིད (
        level > 0
        && lazy_token == NULL
        && PyUnicode_CompareWithASCIIString(module, "lazy") == 0
    ) {
        གལ་སྲིད (_warn_relative_import_of_lazy(p, dots, module_name) < 0) {
            ལོག NULL;
        }
    }
    ལོག _PyAST_ImportFrom(module, names, level, lazy_token ? 1 : 0, lineno,
                             col_offset, end_lineno, end_col_offset, arena);
}

asdl_stmt_seq*
_PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts) {
    གལ་སྲིད (!p->call_invalid_rules) {
        ལོག stmts;
    }
    Py_ssize_t len = asdl_seq_LEN(stmts);
    གལ་སྲིད (len == 0) {
        ལོག stmts;
    }
    stmt_ty last_stmt = asdl_seq_GET(stmts, len - 1);
    གལ་སྲིད (p->last_stmt_location.lineno > last_stmt->lineno) {
        ལོག stmts;
    }
    p->last_stmt_location.lineno = last_stmt->lineno;
    p->last_stmt_location.col_offset = last_stmt->col_offset;
    p->last_stmt_location.end_lineno = last_stmt->end_lineno;
    p->last_stmt_location.end_col_offset = last_stmt->end_col_offset;
    ལོག stmts;
}
