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
    반환 &_PyRuntime.parser.dummy_name;
}

/* Creates a single-element asdl_seq* that contains a */
asdl_seq *
_PyPegen_singleton_seq(Parser *p, void *a)
{
    assert(a != NULL);
    asdl_seq *seq = (asdl_seq*)_Py_asdl_generic_seq_new(1, p->arena);
    만약 (!seq) {
        반환 NULL;
    }
    asdl_seq_SET_UNTYPED(seq, 0, a);
    반환 seq;
}

/* Creates a copy of seq and prepends a to it */
asdl_seq *
_PyPegen_seq_insert_in_front(Parser *p, void *a, asdl_seq *seq)
{
    assert(a != NULL);
    만약 (!seq) {
        반환 _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    만약 (!new_seq) {
        반환 NULL;
    }

    asdl_seq_SET_UNTYPED(new_seq, 0, a);
    위해 (Py_ssize_t i = 1, l = asdl_seq_LEN(new_seq); i < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i - 1));
    }
    반환 new_seq;
}

/* Creates a copy of seq and appends a to it */
asdl_seq *
_PyPegen_seq_append_to_end(Parser *p, asdl_seq *seq, void *a)
{
    assert(a != NULL);
    만약 (!seq) {
        반환 _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    만약 (!new_seq) {
        반환 NULL;
    }

    위해 (Py_ssize_t i = 0, l = asdl_seq_LEN(new_seq); i + 1 < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i));
    }
    asdl_seq_SET_UNTYPED(new_seq, asdl_seq_LEN(new_seq) - 1, a);
    반환 new_seq;
}

static Py_ssize_t
_get_flattened_seq_size(asdl_seq *seqs)
{
    Py_ssize_t size = 0;
    위해 (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        size += asdl_seq_LEN(inner_seq);
    }
    반환 size;
}

/* Flattens an asdl_seq* of asdl_seq*s */
asdl_seq *
_PyPegen_seq_flatten(Parser *p, asdl_seq *seqs)
{
    Py_ssize_t flattened_seq_size = _get_flattened_seq_size(seqs);
    assert(flattened_seq_size > 0);

    asdl_seq *flattened_seq = (asdl_seq*)_Py_asdl_generic_seq_new(flattened_seq_size, p->arena);
    만약 (!flattened_seq) {
        반환 NULL;
    }

    정수 flattened_seq_idx = 0;
    위해 (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        위해 (Py_ssize_t j = 0, li = asdl_seq_LEN(inner_seq); j < li; j++) {
            asdl_seq_SET_UNTYPED(flattened_seq, flattened_seq_idx++, asdl_seq_GET_UNTYPED(inner_seq, j));
        }
    }
    assert(flattened_seq_idx == flattened_seq_size);

    반환 flattened_seq;
}

void *
_PyPegen_seq_last_item(asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    반환 asdl_seq_GET_UNTYPED(seq, len - 1);
}

void *
_PyPegen_seq_first_item(asdl_seq *seq)
{
    반환 asdl_seq_GET_UNTYPED(seq, 0);
}

/* Creates a new name of the form <first_name>.<second_name> */
expr_ty
_PyPegen_join_names_with_dot(Parser *p, expr_ty first_name, expr_ty second_name)
{
    assert(first_name != NULL && second_name != NULL);
    PyObject *uni = PyUnicode_FromFormat("%U.%U",
            first_name->v.Name.id, second_name->v.Name.id);
    만약 (!uni) {
        반환 NULL;
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &uni);
    만약 (_PyArena_AddPyObject(p->arena, uni) < 0) {
        Py_DECREF(uni);
        반환 NULL;
    }

    반환 _PyAST_Name(uni, Load, EXTRA_EXPR(first_name, second_name));
}

/* Counts the total number of dots in seq's tokens */
정수
_PyPegen_seq_count_dots(asdl_seq *seq)
{
    정수 number_of_dots = 0;
    위해 (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        Token *current_expr = asdl_seq_GET_UNTYPED(seq, i);
        switch (current_expr->type) {
            case ELLIPSIS:
                number_of_dots += 3;
                break;
            case DOT:
                number_of_dots += 1;
                break;
            default:
                Py_UNREACHABLE();
        }
    }

    반환 number_of_dots;
}

/* Creates an alias with '*' as the identifier name */
alias_ty
_PyPegen_alias_for_star(Parser *p, 정수 lineno, 정수 col_offset, 정수 end_lineno,
                        정수 end_col_offset, PyArena *arena) {
    PyObject *str = PyUnicode_InternFromString("*");
    만약 (!str) {
        반환 NULL;
    }
    만약 (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        반환 NULL;
    }
    반환 _PyAST_alias(str, NULL, lineno, col_offset, end_lineno, end_col_offset, arena);
}

/* Creates a new asdl_seq* with the identifiers of all the names in seq */
asdl_identifier_seq *
_PyPegen_map_names_to_ids(Parser *p, asdl_expr_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    assert(len > 0);

    asdl_identifier_seq *new_seq = _Py_asdl_identifier_seq_new(len, p->arena);
    만약 (!new_seq) {
        반환 NULL;
    }
    위해 (Py_ssize_t i = 0; i < len; i++) {
        expr_ty e = asdl_seq_GET(seq, i);
        asdl_seq_SET(new_seq, i, e->v.Name.id);
    }
    반환 new_seq;
}

/* Constructs a CmpopExprPair */
CmpopExprPair *
_PyPegen_cmpop_expr_pair(Parser *p, cmpop_ty cmpop, expr_ty expr)
{
    assert(expr != NULL);
    CmpopExprPair *a = _PyArena_Malloc(p->arena, sizeof(CmpopExprPair));
    만약 (!a) {
        반환 NULL;
    }
    a->cmpop = cmpop;
    a->expr = expr;
    반환 a;
}

asdl_int_seq *
_PyPegen_get_cmpops(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    assert(len > 0);

    asdl_int_seq *new_seq = _Py_asdl_int_seq_new(len, p->arena);
    만약 (!new_seq) {
        반환 NULL;
    }
    위해 (Py_ssize_t i = 0; i < len; i++) {
        CmpopExprPair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->cmpop);
    }
    반환 new_seq;
}

asdl_expr_seq *
_PyPegen_get_exprs(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    assert(len > 0);

    asdl_expr_seq *new_seq = _Py_asdl_expr_seq_new(len, p->arena);
    만약 (!new_seq) {
        반환 NULL;
    }
    위해 (Py_ssize_t i = 0; i < len; i++) {
        CmpopExprPair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->expr);
    }
    반환 new_seq;
}

/* Creates an asdl_seq* where all the elements have been changed to have ctx as context */
static asdl_expr_seq *
_set_seq_context(Parser *p, asdl_expr_seq *seq, expr_context_ty ctx)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    만약 (len == 0) {
        반환 NULL;
    }

    asdl_expr_seq *new_seq = _Py_asdl_expr_seq_new(len, p->arena);
    만약 (!new_seq) {
        반환 NULL;
    }
    위해 (Py_ssize_t i = 0; i < len; i++) {
        expr_ty e = asdl_seq_GET(seq, i);
        expr_ty new_e = _PyPegen_set_expr_context(p, e, ctx);
        만약 (!new_e) {
            반환 NULL;
        }
        asdl_seq_SET(new_seq, i, new_e);
    }
    반환 new_seq;
}

static expr_ty
_set_name_context(Parser *p, expr_ty e, expr_context_ty ctx)
{
    반환 _PyAST_Name(e->v.Name.id, ctx, EXTRA_EXPR(e, e));
}

static expr_ty
_set_tuple_context(Parser *p, expr_ty e, expr_context_ty ctx)
{
    asdl_expr_seq *seq = _set_seq_context(p, e->v.Tuple.elts, ctx);
    만약 (!seq && PyErr_Occurred()) {
        반환 NULL;
    }
    반환 _PyAST_Tuple(seq, ctx, EXTRA_EXPR(e, e));
}

static expr_ty
_set_list_context(Parser *p, expr_ty e, expr_context_ty ctx)
{
    asdl_expr_seq *seq = _set_seq_context(p, e->v.List.elts, ctx);
    만약 (!seq && PyErr_Occurred()) {
        반환 NULL;
    }
    반환 _PyAST_List(seq, ctx, EXTRA_EXPR(e, e));
}

static expr_ty
_set_subscript_context(Parser *p, expr_ty e, expr_context_ty ctx)
{
    반환 _PyAST_Subscript(e->v.Subscript.value, e->v.Subscript.slice,
                            ctx, EXTRA_EXPR(e, e));
}

static expr_ty
_set_attribute_context(Parser *p, expr_ty e, expr_context_ty ctx)
{
    반환 _PyAST_Attribute(e->v.Attribute.value, e->v.Attribute.attr,
                            ctx, EXTRA_EXPR(e, e));
}

static expr_ty
_set_starred_context(Parser *p, expr_ty e, expr_context_ty ctx)
{
    expr_ty inner = _PyPegen_set_expr_context(p, e->v.Starred.value, ctx);
    만약 (!inner) {
        반환 NULL;
    }
    반환 _PyAST_Starred(inner, ctx, EXTRA_EXPR(e, e));
}

/* Creates an `expr_ty` equivalent to `expr` but with `ctx` as context */
expr_ty
_PyPegen_set_expr_context(Parser *p, expr_ty expr, expr_context_ty ctx)
{
    assert(expr != NULL);

    expr_ty new = NULL;
    switch (expr->kind) {
        case Name_kind:
            new = _set_name_context(p, expr, ctx);
            break;
        case Tuple_kind:
            new = _set_tuple_context(p, expr, ctx);
            break;
        case List_kind:
            new = _set_list_context(p, expr, ctx);
            break;
        case Subscript_kind:
            new = _set_subscript_context(p, expr, ctx);
            break;
        case Attribute_kind:
            new = _set_attribute_context(p, expr, ctx);
            break;
        case Starred_kind:
            new = _set_starred_context(p, expr, ctx);
            break;
        default:
            new = expr;
    }
    반환 new;
}

/* Invalid targets return NULL without raising, allowing parsing to backtrack.
   Copy valid targets to preserve the memoized expression's context. */
expr_ty
_PyPegen_make_target(Parser *p, expr_ty expr, TARGETS_TYPE targets_type)
{
    assert(expr != NULL);
    assert(targets_type != FOR_TARGETS);
    만약 (_PyPegen_get_invalid_target(expr, targets_type) != NULL) {
        반환 NULL;
    }
    반환 _PyPegen_set_expr_context(
        p, expr, targets_type == DEL_TARGETS ? Del : Store);
}

/* Constructs a KeyValuePair that is used when parsing a dict's key value pairs */
KeyValuePair *
_PyPegen_key_value_pair(Parser *p, expr_ty key, expr_ty value)
{
    KeyValuePair *a = _PyArena_Malloc(p->arena, sizeof(KeyValuePair));
    만약 (!a) {
        반환 NULL;
    }
    a->key = key;
    a->value = value;
    반환 a;
}

/* Extracts all keys from an asdl_seq* of KeyValuePair*'s */
asdl_expr_seq *
_PyPegen_get_keys(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_expr_seq *new_seq = _Py_asdl_expr_seq_new(len, p->arena);
    만약 (!new_seq) {
        반환 NULL;
    }
    위해 (Py_ssize_t i = 0; i < len; i++) {
        KeyValuePair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->key);
    }
    반환 new_seq;
}

/* Extracts all values from an asdl_seq* of KeyValuePair*'s */
asdl_expr_seq *
_PyPegen_get_values(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_expr_seq *new_seq = _Py_asdl_expr_seq_new(len, p->arena);
    만약 (!new_seq) {
        반환 NULL;
    }
    위해 (Py_ssize_t i = 0; i < len; i++) {
        KeyValuePair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->value);
    }
    반환 new_seq;
}

/* Constructs a KeyPatternPair that is used when parsing mapping & class patterns */
KeyPatternPair *
_PyPegen_key_pattern_pair(Parser *p, expr_ty key, pattern_ty pattern)
{
    KeyPatternPair *a = _PyArena_Malloc(p->arena, sizeof(KeyPatternPair));
    만약 (!a) {
        반환 NULL;
    }
    a->key = key;
    a->pattern = pattern;
    반환 a;
}

/* Extracts all keys from an asdl_seq* of KeyPatternPair*'s */
asdl_expr_seq *
_PyPegen_get_pattern_keys(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_expr_seq *new_seq = _Py_asdl_expr_seq_new(len, p->arena);
    만약 (!new_seq) {
        반환 NULL;
    }
    위해 (Py_ssize_t i = 0; i < len; i++) {
        KeyPatternPair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->key);
    }
    반환 new_seq;
}

/* Extracts all patterns from an asdl_seq* of KeyPatternPair*'s */
asdl_pattern_seq *
_PyPegen_get_patterns(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_pattern_seq *new_seq = _Py_asdl_pattern_seq_new(len, p->arena);
    만약 (!new_seq) {
        반환 NULL;
    }
    위해 (Py_ssize_t i = 0; i < len; i++) {
        KeyPatternPair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->pattern);
    }
    반환 new_seq;
}

/* Constructs a NameDefaultPair */
NameDefaultPair *
_PyPegen_name_default_pair(Parser *p, arg_ty arg, expr_ty value, Token *tc)
{
    NameDefaultPair *a = _PyArena_Malloc(p->arena, sizeof(NameDefaultPair));
    만약 (!a) {
        반환 NULL;
    }
    a->arg = _PyPegen_add_type_comment_to_arg(p, arg, tc);
    만약 (!a->arg) {
        반환 NULL;
    }
    a->value = value;
    반환 a;
}

/* Constructs a SlashWithDefault */
SlashWithDefault *
_PyPegen_slash_with_default(Parser *p, asdl_arg_seq *plain_names, asdl_seq *names_with_defaults)
{
    SlashWithDefault *a = _PyArena_Malloc(p->arena, sizeof(SlashWithDefault));
    만약 (!a) {
        반환 NULL;
    }
    a->plain_names = plain_names;
    a->names_with_defaults = names_with_defaults;
    반환 a;
}

/* Constructs a StarEtc */
StarEtc *
_PyPegen_star_etc(Parser *p, arg_ty vararg, asdl_seq *kwonlyargs, arg_ty kwarg)
{
    StarEtc *a = _PyArena_Malloc(p->arena, sizeof(StarEtc));
    만약 (!a) {
        반환 NULL;
    }
    a->vararg = vararg;
    a->kwonlyargs = kwonlyargs;
    a->kwarg = kwarg;
    반환 a;
}

asdl_seq *
_PyPegen_join_sequences(Parser *p, asdl_seq *a, asdl_seq *b)
{
    Py_ssize_t first_len = asdl_seq_LEN(a);
    Py_ssize_t second_len = asdl_seq_LEN(b);
    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(first_len + second_len, p->arena);
    만약 (!new_seq) {
        반환 NULL;
    }

    정수 k = 0;
    위해 (Py_ssize_t i = 0; i < first_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(a, i));
    }
    위해 (Py_ssize_t i = 0; i < second_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(b, i));
    }

    반환 new_seq;
}

static asdl_arg_seq*
_get_names(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_arg_seq *seq = _Py_asdl_arg_seq_new(len, p->arena);
    만약 (!seq) {
        반환 NULL;
    }
    위해 (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->arg);
    }
    반환 seq;
}

static asdl_expr_seq *
_get_defaults(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_expr_seq *seq = _Py_asdl_expr_seq_new(len, p->arena);
    만약 (!seq) {
        반환 NULL;
    }
    위해 (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->value);
    }
    반환 seq;
}

static 정수
_make_posonlyargs(Parser *p,
                  asdl_arg_seq *slash_without_default,
                  SlashWithDefault *slash_with_default,
                  asdl_arg_seq **posonlyargs) {
    만약 (slash_without_default != NULL) {
        *posonlyargs = slash_without_default;
    }
    아니면 만약 (slash_with_default != NULL) {
        asdl_arg_seq *slash_with_default_names =
                _get_names(p, slash_with_default->names_with_defaults);
        만약 (!slash_with_default_names) {
            반환 -1;
        }
        *posonlyargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default->plain_names,
                (asdl_seq*)slash_with_default_names);
    }
    아니면 {
        *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }
    반환 *posonlyargs == NULL ? -1 : 0;
}

static 정수
_make_posargs(Parser *p,
              asdl_arg_seq *plain_names,
              asdl_seq *names_with_default,
              asdl_arg_seq **posargs) {

    만약 (names_with_default != NULL) {
        만약 (plain_names != NULL) {
            asdl_arg_seq *names_with_default_names = _get_names(p, names_with_default);
            만약 (!names_with_default_names) {
                반환 -1;
            }
            *posargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                    p,(asdl_seq*)plain_names, (asdl_seq*)names_with_default_names);
        }
        아니면 {
            *posargs = _get_names(p, names_with_default);
        }
    }
    아니면 {
        만약 (plain_names != NULL) {
            // With the current grammar, we never get here.
            // If that has changed, remove the assert, and test thoroughly.
            assert(0);
            *posargs = plain_names;
        }
        아니면 {
            *posargs = _Py_asdl_arg_seq_new(0, p->arena);
        }
    }
    반환 *posargs == NULL ? -1 : 0;
}

static 정수
_make_posdefaults(Parser *p,
                  SlashWithDefault *slash_with_default,
                  asdl_seq *names_with_default,
                  asdl_expr_seq **posdefaults) {
    만약 (slash_with_default != NULL && names_with_default != NULL) {
        asdl_expr_seq *slash_with_default_values =
                _get_defaults(p, slash_with_default->names_with_defaults);
        만약 (!slash_with_default_values) {
            반환 -1;
        }
        asdl_expr_seq *names_with_default_values = _get_defaults(p, names_with_default);
        만약 (!names_with_default_values) {
            반환 -1;
        }
        *posdefaults = (asdl_expr_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default_values,
                (asdl_seq*)names_with_default_values);
    }
    아니면 만약 (slash_with_default == NULL && names_with_default != NULL) {
        *posdefaults = _get_defaults(p, names_with_default);
    }
    아니면 만약 (slash_with_default != NULL && names_with_default == NULL) {
        *posdefaults = _get_defaults(p, slash_with_default->names_with_defaults);
    }
    아니면 {
        *posdefaults = _Py_asdl_expr_seq_new(0, p->arena);
    }
    반환 *posdefaults == NULL ? -1 : 0;
}

static 정수
_make_kwargs(Parser *p, StarEtc *star_etc,
             asdl_arg_seq **kwonlyargs,
             asdl_expr_seq **kwdefaults) {
    만약 (star_etc != NULL && star_etc->kwonlyargs != NULL) {
        *kwonlyargs = _get_names(p, star_etc->kwonlyargs);
    }
    아니면 {
        *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }

    만약 (*kwonlyargs == NULL) {
        반환 -1;
    }

    만약 (star_etc != NULL && star_etc->kwonlyargs != NULL) {
        *kwdefaults = _get_defaults(p, star_etc->kwonlyargs);
    }
    아니면 {
        *kwdefaults = _Py_asdl_expr_seq_new(0, p->arena);
    }

    만약 (*kwdefaults == NULL) {
        반환 -1;
    }

    반환 0;
}

/* Constructs an arguments_ty object out of all the parsed constructs in the parameters rule */
arguments_ty
_PyPegen_make_arguments(Parser *p, asdl_arg_seq *slash_without_default,
                        SlashWithDefault *slash_with_default, asdl_arg_seq *plain_names,
                        asdl_seq *names_with_default, StarEtc *star_etc)
{
    asdl_arg_seq *posonlyargs;
    만약 (_make_posonlyargs(p, slash_without_default, slash_with_default, &posonlyargs) == -1) {
        반환 NULL;
    }

    asdl_arg_seq *posargs;
    만약 (_make_posargs(p, plain_names, names_with_default, &posargs) == -1) {
        반환 NULL;
    }

    asdl_expr_seq *posdefaults;
    만약 (_make_posdefaults(p,slash_with_default, names_with_default, &posdefaults) == -1) {
        반환 NULL;
    }

    arg_ty vararg = NULL;
    만약 (star_etc != NULL && star_etc->vararg != NULL) {
        vararg = star_etc->vararg;
    }

    asdl_arg_seq *kwonlyargs;
    asdl_expr_seq *kwdefaults;
    만약 (_make_kwargs(p, star_etc, &kwonlyargs, &kwdefaults) == -1) {
        반환 NULL;
    }

    arg_ty kwarg = NULL;
    만약 (star_etc != NULL && star_etc->kwarg != NULL) {
        kwarg = star_etc->kwarg;
    }

    반환 _PyAST_arguments(posonlyargs, posargs, vararg, kwonlyargs,
                            kwdefaults, kwarg, posdefaults, p->arena);
}


/* Constructs an empty arguments_ty object, that gets used when a function accepts no
 * arguments. */
arguments_ty
_PyPegen_empty_arguments(Parser *p)
{
    asdl_arg_seq *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    만약 (!posonlyargs) {
        반환 NULL;
    }
    asdl_arg_seq *posargs = _Py_asdl_arg_seq_new(0, p->arena);
    만약 (!posargs) {
        반환 NULL;
    }
    asdl_expr_seq *posdefaults = _Py_asdl_expr_seq_new(0, p->arena);
    만약 (!posdefaults) {
        반환 NULL;
    }
    asdl_arg_seq *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    만약 (!kwonlyargs) {
        반환 NULL;
    }
    asdl_expr_seq *kwdefaults = _Py_asdl_expr_seq_new(0, p->arena);
    만약 (!kwdefaults) {
        반환 NULL;
    }

    반환 _PyAST_arguments(posonlyargs, posargs, NULL, kwonlyargs,
                            kwdefaults, NULL, posdefaults, p->arena);
}

/* Encapsulates the value of an operator_ty into an AugOperator struct */
AugOperator *
_PyPegen_augoperator(Parser *p, operator_ty kind)
{
    AugOperator *a = _PyArena_Malloc(p->arena, sizeof(AugOperator));
    만약 (!a) {
        반환 NULL;
    }
    a->kind = kind;
    반환 a;
}

/* Construct a FunctionDef equivalent to function_def, but with decorators */
stmt_ty
_PyPegen_function_def_decorators(Parser *p, asdl_expr_seq *decorators, stmt_ty function_def)
{
    assert(function_def != NULL);
    만약 (function_def->kind == AsyncFunctionDef_kind) {
        반환 _PyAST_AsyncFunctionDef(
            function_def->v.AsyncFunctionDef.name,
            function_def->v.AsyncFunctionDef.args,
            function_def->v.AsyncFunctionDef.body, decorators,
            function_def->v.AsyncFunctionDef.returns,
            function_def->v.AsyncFunctionDef.type_comment,
            function_def->v.AsyncFunctionDef.type_params,
            function_def->lineno, function_def->col_offset,
            function_def->end_lineno, function_def->end_col_offset, p->arena);
    }

    반환 _PyAST_FunctionDef(
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
    반환 _PyAST_ClassDef(
        class_def->v.ClassDef.name,
        class_def->v.ClassDef.bases, class_def->v.ClassDef.keywords,
        class_def->v.ClassDef.body, decorators,
        class_def->v.ClassDef.type_params,
        class_def->lineno, class_def->col_offset, class_def->end_lineno,
        class_def->end_col_offset, p->arena);
}

/* Construct a KeywordOrStarred */
KeywordOrStarred *
_PyPegen_keyword_or_starred(Parser *p, void *element, 정수 is_keyword)
{
    KeywordOrStarred *a = _PyArena_Malloc(p->arena, sizeof(KeywordOrStarred));
    만약 (!a) {
        반환 NULL;
    }
    a->element = element;
    a->is_keyword = is_keyword;
    반환 a;
}

/* Get the number of starred expressions in an asdl_seq* of KeywordOrStarred*s */
static 정수
_seq_number_of_starred_exprs(asdl_seq *seq)
{
    정수 n = 0;
    위해 (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(seq, i);
        만약 (!k->is_keyword) {
            n++;
        }
    }
    반환 n;
}

/* Extract the starred expressions of an asdl_seq* of KeywordOrStarred*s */
asdl_expr_seq *
_PyPegen_seq_extract_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    정수 new_len = _seq_number_of_starred_exprs(kwargs);
    만약 (new_len == 0) {
        반환 NULL;
    }
    asdl_expr_seq *new_seq = _Py_asdl_expr_seq_new(new_len, p->arena);
    만약 (!new_seq) {
        반환 NULL;
    }

    정수 idx = 0;
    위해 (Py_ssize_t i = 0, len = asdl_seq_LEN(kwargs); i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        만약 (!k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    반환 new_seq;
}

/* Return a new asdl_seq* with only the keywords in kwargs */
asdl_keyword_seq*
_PyPegen_seq_delete_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    Py_ssize_t len = asdl_seq_LEN(kwargs);
    Py_ssize_t new_len = len - _seq_number_of_starred_exprs(kwargs);
    만약 (new_len == 0) {
        반환 NULL;
    }
    asdl_keyword_seq *new_seq = _Py_asdl_keyword_seq_new(new_len, p->arena);
    만약 (!new_seq) {
        반환 NULL;
    }

    정수 idx = 0;
    위해 (Py_ssize_t i = 0; i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        만약 (k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    반환 new_seq;
}

expr_ty
_PyPegen_ensure_imaginary(Parser *p, expr_ty exp)
{
    만약 (exp->kind != Constant_kind || !PyComplex_CheckExact(exp->v.Constant.value)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "imaginary number required in complex literal");
        반환 NULL;
    }
    반환 exp;
}

expr_ty
_PyPegen_ensure_real(Parser *p, expr_ty exp)
{
    만약 (exp->kind != Constant_kind || PyComplex_CheckExact(exp->v.Constant.value)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "real number required in complex literal");
        반환 NULL;
    }
    반환 exp;
}

mod_ty
_PyPegen_make_module(Parser *p, asdl_stmt_seq *a) {
    asdl_type_ignore_seq *type_ignores = NULL;
    Py_ssize_t num = p->type_ignore_comments.num_items;
    만약 (num > 0) {
        // Turn the raw (comment, lineno) pairs into TypeIgnore objects in the arena
        type_ignores = _Py_asdl_type_ignore_seq_new(num, p->arena);
        만약 (type_ignores == NULL) {
            반환 NULL;
        }
        위해 (Py_ssize_t i = 0; i < num; i++) {
            PyObject *tag = _PyPegen_new_type_comment(p, p->type_ignore_comments.items[i].comment);
            만약 (tag == NULL) {
                반환 NULL;
            }
            type_ignore_ty ti = _PyAST_TypeIgnore(p->type_ignore_comments.items[i].lineno,
                                                  tag, p->arena);
            만약 (ti == NULL) {
                반환 NULL;
            }
            asdl_seq_SET(type_ignores, i, ti);
        }
    }
    반환 _PyAST_Module(a, type_ignores, p->arena);
}

PyObject *
_PyPegen_new_type_comment(Parser *p, const char *s)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, strlen(s), NULL);
    만약 (res == NULL) {
        반환 NULL;
    }
    만약 (_PyArena_AddPyObject(p->arena, res) < 0) {
        Py_DECREF(res);
        반환 NULL;
    }
    반환 res;
}

arg_ty
_PyPegen_add_type_comment_to_arg(Parser *p, arg_ty a, Token *tc)
{
    만약 (tc == NULL) {
        반환 a;
    }
    const char *bytes = PyBytes_AsString(tc->bytes);
    만약 (bytes == NULL) {
        반환 NULL;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    만약 (tco == NULL) {
        반환 NULL;
    }
    반환 _PyAST_arg(a->arg, a->annotation, tco,
                      a->lineno, a->col_offset, a->end_lineno, a->end_col_offset,
                      p->arena);
}

/* Checks if the NOTEQUAL token is valid given the current parser flags
0 indicates success and nonzero indicates failure (an exception may be set) */
정수
_PyPegen_check_barry_as_flufl(Parser *p, Token* t) {
    assert(t->bytes != NULL);
    assert(t->type == NOTEQUAL);

    const char* tok_str = PyBytes_AS_STRING(t->bytes);
    만약 (p->flags & PyPARSE_BARRY_AS_BDFL && strcmp(tok_str, "<>") != 0) {
        RAISE_SYNTAX_ERROR("with Barry as BDFL, use '<>' instead of '!='");
        반환 -1;
    }
    만약 (!(p->flags & PyPARSE_BARRY_AS_BDFL)) {
        반환 strcmp(tok_str, "!=");
    }
    반환 0;
}

정수
_PyPegen_check_legacy_stmt(Parser *p, expr_ty name) {
    만약 (name->kind != Name_kind) {
        반환 0;
    }
    const char* candidates[2] = {"print", "exec"};
    위해 (정수 i=0; i<2; i++) {
        만약 (PyUnicode_CompareWithASCIIString(name->v.Name.id, candidates[i]) == 0) {
            반환 1;
        }
    }
    반환 0;
}

void *
_PyPegen_raise_error_for_missing_comma(Parser *p, expr_ty a, expr_ty b)
{
    // Don't raise for legacy statements like "print x" or "exec x"
    만약 (_PyPegen_check_legacy_stmt(p, a)) {
        반환 NULL;
    }
    // Only raise inside parentheses/brackets (level > 0)
    만약 (p->tokens[p->mark - 1]->level == 0) {
        반환 NULL;
    }
    // For multi-line expressions (like string concatenations), point to the
    // last line instead of the first for a more helpful error message.
    // Use a->col_offset as the starting column since all strings in the
    // concatenation typically share the same indentation.
    만약 (a->end_lineno > a->lineno) {
        반환 RAISE_ERROR_KNOWN_LOCATION(
            p, PyExc_SyntaxError, a->end_lineno, a->col_offset,
            a->end_lineno, a->end_col_offset,
            "invalid syntax. Perhaps you forgot a comma?"
        );
    }
    반환 RAISE_ERROR_KNOWN_LOCATION(
        p, PyExc_SyntaxError, a->lineno, a->col_offset,
        b->end_lineno, b->end_col_offset,
        "invalid syntax. Perhaps you forgot a comma?"
    );
}

static ResultTokenWithMetadata *
result_token_with_metadata(Parser *p, void *result, PyObject *metadata)
{
    ResultTokenWithMetadata *res = _PyArena_Malloc(p->arena, sizeof(ResultTokenWithMetadata));
    만약 (res == NULL) {
        반환 NULL;
    }
    res->metadata = metadata;
    res->result = result;
    반환 res;
}

static char
formatted_string_prefix(const Parser *p)
{
    정수 nested = 0;
    위해 (정수 i = p->mark - 1; i >= 0; i--) {
        정수 type = p->tokens[i]->type;
        만약 (type == FSTRING_END || type == TSTRING_END) {
            nested++;
        }
        아니면 만약 (type == FSTRING_START || type == TSTRING_START) {
            만약 (nested == 0) {
                반환 type == TSTRING_START ? 't' : 'f';
            }
            nested--;
        }
    }
    Py_UNREACHABLE();
}

ResultTokenWithMetadata *
_PyPegen_check_fstring_conversion(Parser *p, Token* conv_token, expr_ty conv)
{
    만약 (conv_token->lineno != conv->lineno || conv_token->end_col_offset != conv->col_offset) {
        반환 RAISE_SYNTAX_ERROR_KNOWN_RANGE(
            conv_token, conv,
            "%c-string: conversion type must come right after the exclamation mark",
            formatted_string_prefix(p)
        );
    }

    Py_UCS4 first = PyUnicode_READ_CHAR(conv->v.Name.id, 0);
    만약 (PyUnicode_GET_LENGTH(conv->v.Name.id) > 1 ||
            !(first == 's' || first == 'r' || first == 'a')) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(conv,
                                            "%c-string: invalid conversion character %R: expected 's', 'r', or 'a'",
                                            formatted_string_prefix(p),
                                            conv->v.Name.id);
        반환 NULL;
    }

    반환 result_token_with_metadata(p, conv, conv_token->metadata);
}

ResultTokenWithMetadata *
_PyPegen_setup_full_format_spec(Parser *p, Token *colon, asdl_expr_seq *spec, 정수 lineno, 정수 col_offset,
                                정수 end_lineno, 정수 end_col_offset, PyArena *arena)
{
    만약 (!spec) {
        반환 NULL;
    }

    // This is needed to keep compatibility with 3.11, where an empty format
    // spec is parsed as an *empty* JoinedStr node, instead of having an empty
    // constant in it.
    Py_ssize_t n_items = asdl_seq_LEN(spec);
    Py_ssize_t non_empty_count = 0;
    위해 (Py_ssize_t i = 0; i < n_items; i++) {
        expr_ty item = asdl_seq_GET(spec, i);
        non_empty_count += !(item->kind == Constant_kind &&
                             PyUnicode_CheckExact(item->v.Constant.value) &&
                             PyUnicode_GET_LENGTH(item->v.Constant.value) == 0);
    }
    만약 (non_empty_count != n_items) {
        asdl_expr_seq *resized_spec =
            _Py_asdl_expr_seq_new(non_empty_count, p->arena);
        만약 (resized_spec == NULL) {
            반환 NULL;
        }
        Py_ssize_t j = 0;
        위해 (Py_ssize_t i = 0; i < n_items; i++) {
            expr_ty item = asdl_seq_GET(spec, i);
            만약 (item->kind == Constant_kind &&
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
    만약 (n == 0 || (n == 1 && asdl_seq_GET(spec, 0)->kind == Constant_kind)) {
        res = _PyAST_JoinedStr(spec, lineno, col_offset, end_lineno,
                                    end_col_offset, p->arena);
    } 아니면 {
        res = _PyPegen_concatenate_strings(p, spec,
                             lineno, col_offset, end_lineno,
                             end_col_offset, arena);
    }
    만약 (!res) {
        반환 NULL;
    }
    반환 result_token_with_metadata(p, res, colon->metadata);
}

const char *
_PyPegen_get_expr_name(expr_ty e)
{
    assert(e != NULL);
    switch (e->kind) {
        case Attribute_kind:
            반환 "attribute";
        case Subscript_kind:
            반환 "subscript";
        case Starred_kind:
            반환 "starred";
        case Name_kind:
            반환 "name";
        case List_kind:
            반환 "list";
        case Tuple_kind:
            반환 "tuple";
        case Lambda_kind:
            반환 "lambda";
        case Call_kind:
            반환 "function call";
        case BoolOp_kind:
        case BinOp_kind:
        case UnaryOp_kind:
            반환 "expression";
        case GeneratorExp_kind:
            반환 "generator expression";
        case Yield_kind:
        case YieldFrom_kind:
            반환 "yield expression";
        case Await_kind:
            반환 "await expression";
        case ListComp_kind:
            반환 "list comprehension";
        case SetComp_kind:
            반환 "set comprehension";
        case DictComp_kind:
            반환 "dict comprehension";
        case Dict_kind:
            반환 "dict literal";
        case Set_kind:
            반환 "set display";
        case JoinedStr_kind:
        case FormattedValue_kind:
            반환 "f-string expression";
        case TemplateStr_kind:
        case Interpolation_kind:
            반환 "t-string expression";
        case Constant_kind: {
            PyObject *value = e->v.Constant.value;
            만약 (value == Py_None) {
                반환 "None";
            }
            만약 (value == Py_False) {
                반환 "False";
            }
            만약 (value == Py_True) {
                반환 "True";
            }
            만약 (value == Py_Ellipsis) {
                반환 "ellipsis";
            }
            반환 "literal";
        }
        case Compare_kind:
            반환 "comparison";
        case IfExp_kind:
            반환 "conditional expression";
        case NamedExpr_kind:
            반환 "named expression";
        default:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->kind, e->lineno);
            반환 NULL;
    }
}

expr_ty
_PyPegen_get_last_comprehension_item(comprehension_ty comprehension) {
    만약 (comprehension->ifs == NULL || asdl_seq_LEN(comprehension->ifs) == 0) {
        반환 comprehension->iter;
    }
    반환 PyPegen_last_item(comprehension->ifs, expr_ty);
}

expr_ty _PyPegen_collect_call_seqs(Parser *p, asdl_expr_seq *a, asdl_seq *b,
                     정수 lineno, 정수 col_offset, 정수 end_lineno,
                     정수 end_col_offset, PyArena *arena) {
    Py_ssize_t args_len = asdl_seq_LEN(a);
    Py_ssize_t total_len = args_len;

    만약 (b == NULL) {
        반환 _PyAST_Call(_PyPegen_dummy_name(p), a, NULL, lineno, col_offset,
                        end_lineno, end_col_offset, arena);

    }

    asdl_expr_seq *starreds = _PyPegen_seq_extract_starred_exprs(p, b);
    만약 (!starreds && PyErr_Occurred()) {
        반환 NULL;
    }

    asdl_keyword_seq *keywords = _PyPegen_seq_delete_starred_exprs(p, b);
    만약 (!keywords && PyErr_Occurred()) {
        반환 NULL;
    }

    만약 (starreds) {
        total_len += asdl_seq_LEN(starreds);
    }

    asdl_expr_seq *args = _Py_asdl_expr_seq_new(total_len, arena);
    만약 (args == NULL) {
        반환 NULL;
    }

    Py_ssize_t i = 0;
    위해 (i = 0; i < args_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(a, i));
    }
    위해 (; i < total_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(starreds, i - args_len));
    }

    반환 _PyAST_Call(_PyPegen_dummy_name(p), args, keywords, lineno,
                       col_offset, end_lineno, end_col_offset, arena);
}

// AST Error reporting helpers

expr_ty
_PyPegen_get_invalid_target(expr_ty e, TARGETS_TYPE targets_type)
{
    만약 (e == NULL) {
        반환 NULL;
    }

    만약 (targets_type == SINGLE_TARGETS ||
        targets_type == ATTRIBUTE_OR_SUBSCRIPT_TARGETS) {
        만약 (targets_type == SINGLE_TARGETS && e->kind == Name_kind) {
            반환 NULL;
        }
        반환 e->kind == Attribute_kind || e->kind == Subscript_kind ? NULL : e;
    }

#define VISIT_CONTAINER(CONTAINER, TYPE) do { \
        Py_ssize_t len = asdl_seq_LEN((CONTAINER)->v.TYPE.elts);\
        위해 (Py_ssize_t i = 0; i < len; i++) {\
            expr_ty other = asdl_seq_GET((CONTAINER)->v.TYPE.elts, i);\
            expr_ty child = _PyPegen_get_invalid_target(other, targets_type);\
            만약 (child != NULL) {\
                반환 child;\
            }\
        }\
    } 동안 (0)

    // We only need to visit List and Tuple nodes recursively as those
    // are the only ones that can contain valid names in targets when
    // they are parsed as expressions. Any other kind of expression
    // that is a container (like Sets or Dicts) is directly invalid and
    // we don't need to visit it recursively.

    switch (e->kind) {
        case List_kind:
            VISIT_CONTAINER(e, List);
            반환 NULL;
        case Tuple_kind:
            VISIT_CONTAINER(e, Tuple);
            반환 NULL;
        case Starred_kind:
            만약 (targets_type == DEL_TARGETS) {
                반환 e;
            }
            반환 _PyPegen_get_invalid_target(e->v.Starred.value, targets_type);
        case Compare_kind:
            // This is needed, because the `a in b` in `for a in b` gets parsed
            // as a comparison, and so we need to search the left side of the comparison
            // for invalid targets.
            만약 (targets_type == FOR_TARGETS) {
                cmpop_ty cmpop = (cmpop_ty) asdl_seq_GET(e->v.Compare.ops, 0);
                만약 (cmpop == In) {
                    반환 _PyPegen_get_invalid_target(e->v.Compare.left, targets_type);
                }
                반환 NULL;
            }
            반환 e;
        case Name_kind:
        case Subscript_kind:
        case Attribute_kind:
            반환 NULL;
        default:
            반환 e;
    }
}

void *_PyPegen_arguments_parsing_error(Parser *p, expr_ty e) {
    정수 kwarg_unpacking = 0;
    위해 (Py_ssize_t i = 0, l = asdl_seq_LEN(e->v.Call.keywords); i < l; i++) {
        keyword_ty keyword = asdl_seq_GET(e->v.Call.keywords, i);
        만약 (!keyword->arg) {
            kwarg_unpacking = 1;
        }
    }

    const char *msg = NULL;
    만약 (kwarg_unpacking) {
        msg = "positional argument follows keyword argument unpacking";
    } 아니면 {
        msg = "positional argument follows keyword argument";
    }

    반환 RAISE_SYNTAX_ERROR(msg);
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
    만약 (len <= 1) {
        반환 NULL;
    }

    comprehension_ty last_comprehension = PyPegen_last_item(comprehensions, comprehension_ty);

    반환 RAISE_SYNTAX_ERROR_KNOWN_RANGE(
        (expr_ty) asdl_seq_GET(args->v.Call.args, len - 1),
        _PyPegen_get_last_comprehension_item(last_comprehension),
        "Generator expression must be parenthesized"
    );
}

// Fstring stuff

static expr_ty
_PyPegen_decode_fstring_part(Parser* p, 정수 is_raw, expr_ty constant, Token* token) {
    assert(PyUnicode_CheckExact(constant->v.Constant.value));

    const char* bstr = PyUnicode_AsUTF8(constant->v.Constant.value);
    만약 (bstr == NULL) {
        반환 NULL;
    }

    size_t len;
    만약 (strcmp(bstr, "{{") == 0 || strcmp(bstr, "}}") == 0) {
        len = 1;
    } 아니면 {
        len = strlen(bstr);
    }

    is_raw = is_raw || strchr(bstr, '\\') == NULL;
    PyObject *str = _PyPegen_decode_string(p, is_raw, bstr, len, token);
    만약 (str == NULL) {
        _Pypegen_raise_decode_error(p);
        반환 NULL;
    }
    만약 (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        반환 NULL;
    }
    반환 _PyAST_Constant(str, NULL, constant->lineno, constant->col_offset,
                           constant->end_lineno, constant->end_col_offset,
                           p->arena);
}

static asdl_expr_seq *
_get_resized_exprs(Parser *p, Token *a, asdl_expr_seq *raw_expressions,
                   Token *b, 정수 is_tstring)
{
    Py_ssize_t n_items = asdl_seq_LEN(raw_expressions);
    Py_ssize_t total_items = n_items;
    위해 (Py_ssize_t i = 0; i < n_items; i++) {
        expr_ty item = asdl_seq_GET(raw_expressions, i);
        만약 (item->kind == JoinedStr_kind) {
            total_items += asdl_seq_LEN(item->v.JoinedStr.values) - 1;
        }
    }

    const char* quote_str = PyBytes_AsString(a->bytes);
    만약 (quote_str == NULL) {
        반환 NULL;
    }
    정수 is_raw = strpbrk(quote_str, "rR") != NULL;

    asdl_expr_seq *seq = _Py_asdl_expr_seq_new(total_items, p->arena);
    만약 (seq == NULL) {
        반환 NULL;
    }

    Py_ssize_t index = 0;
    위해 (Py_ssize_t i = 0; i < n_items; i++) {
        expr_ty item = asdl_seq_GET(raw_expressions, i);

        /* Debug expressions arrive as JoinedStr(text, value); flatten them
           into the surrounding string. */
        만약 (item->kind == JoinedStr_kind) {
            asdl_expr_seq *values = item->v.JoinedStr.values;
            만약 (asdl_seq_LEN(values) != 2) {
                PyErr_Format(PyExc_SystemError,
                             is_tstring
                             ? "unexpected TemplateStr node without debug data in t-string at line %d"
                             : "unexpected JoinedStr node without debug data in f-string at line %d",
                             item->lineno);
                반환 NULL;
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

        만약 (item->kind == Constant_kind) {
            item = _PyPegen_decode_fstring_part(p, is_raw, item, b);
            만약 (item == NULL) {
                반환 NULL;
            }

            /* Tokenizer emits string parts even when the underlying string
            might become an empty value (e.g. FSTRING_MIDDLE with the value \\n)
            so we need to check for them and simplify it here. */
            만약 (PyUnicode_CheckExact(item->v.Constant.value)
                && PyUnicode_GET_LENGTH(item->v.Constant.value) == 0) {
                continue;
            }
        }
        asdl_seq_SET(seq, index++, item);
    }

    asdl_expr_seq *resized_exprs;
    만약 (index != total_items) {
        resized_exprs = _Py_asdl_expr_seq_new(index, p->arena);
        만약 (resized_exprs == NULL) {
            반환 NULL;
        }
        위해 (Py_ssize_t i = 0; i < index; i++) {
            asdl_seq_SET(resized_exprs, i, asdl_seq_GET(seq, i));
        }
    }
    아니면 {
        resized_exprs = seq;
    }
    반환 resized_exprs;
}

expr_ty
_PyPegen_template_str(Parser *p, Token *a, asdl_expr_seq *raw_expressions, Token *b) {

    asdl_expr_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 1);
    만약 (resized_exprs == NULL) {
        반환 NULL;
    }
    반환 _PyAST_TemplateStr(resized_exprs, a->lineno, a->col_offset,
                              b->end_lineno, b->end_col_offset,
                              p->arena);
}

expr_ty
_PyPegen_joined_str(Parser *p, Token* a, asdl_expr_seq* raw_expressions, Token*b) {

    asdl_expr_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 0);
    만약 (resized_exprs == NULL) {
        반환 NULL;
    }
    반환 _PyAST_JoinedStr(resized_exprs, a->lineno, a->col_offset,
                            b->end_lineno, b->end_col_offset,
                            p->arena);
}

expr_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok) {
    만약 (tok->bytes == NULL) {
        반환 _PyPegen_raise_incomplete_input(p);
    }
    Py_ssize_t bsize = PyBytes_GET_SIZE(tok->bytes);
    const char *bstr = PyBytes_AS_STRING(tok->bytes);

    정수 is_raw = tok->is_raw;

    PyObject* str = _PyPegen_decode_string(p, is_raw, bstr, bsize, tok);
    만약 (str == NULL) {
        반환 NULL;
    }
    만약 (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        반환 NULL;
    }
    반환 _PyAST_Constant(str, NULL, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

expr_ty _PyPegen_constant_from_token(Parser* p, Token* tok) {
    만약 (tok->bytes == NULL) {
        반환 _PyPegen_raise_incomplete_input(p);
    }
    const char *bstr = PyBytes_AS_STRING(tok->bytes);
    PyObject* str = PyUnicode_FromString(bstr);
    만약 (str == NULL) {
        반환 NULL;
    }
    만약 (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        반환 NULL;
    }
    반환 _PyAST_Constant(str, NULL, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

expr_ty _PyPegen_constant_from_string(Parser* p, Token* tok) {
    만약 (tok->bytes == NULL) {
        반환 _PyPegen_raise_incomplete_input(p);
    }
    const char *the_str = PyBytes_AS_STRING(tok->bytes);
    PyObject *s = _PyPegen_parse_string(p, tok);
    만약 (s == NULL) {
        _Pypegen_raise_decode_error(p);
        반환 NULL;
    }
    만약 (_PyArena_AddPyObject(p->arena, s) < 0) {
        Py_DECREF(s);
        반환 NULL;
    }
    PyObject *kind = NULL;
    만약 (the_str && the_str[0] == 'u') {
        kind = _PyPegen_new_identifier(p, "u");
        만약 (kind == NULL) {
            반환 NULL;
        }
    }
    반환 _PyAST_Constant(s, kind, tok->lineno, tok->col_offset, tok->end_lineno, tok->end_col_offset, p->arena);
}

static 정수
_get_interpolation_conversion(Parser *p, Token *debug, ResultTokenWithMetadata *conversion,
                              ResultTokenWithMetadata *format)
{
    만약 (conversion != NULL) {
        expr_ty conversion_expr = (expr_ty) conversion->result;
        assert(conversion_expr->kind == Name_kind);
        Py_UCS4 first = PyUnicode_READ_CHAR(conversion_expr->v.Name.id, 0);
        반환 Py_SAFE_DOWNCAST(first, Py_UCS4, 정수);
    }
    아니면 만약 (debug && !format) {
        /* If no conversion is specified, use !r for debug expressions */
        반환 (정수)'r';
    }
    반환 -1;
}

static PyObject *
_strip_interpolation_debug_expr(PyObject *exprstr)
{
    Py_ssize_t len = PyUnicode_GET_LENGTH(exprstr);

    /* Discard whitespace and explicit line continuations after the debug "="
       but preserve whitespace before it. */
    동안 (len > 0) {
        정수 has_newline = 0;
        동안 (len > 0) {
            Py_UCS4 c = PyUnicode_READ_CHAR(exprstr, len - 1);
            만약 (!_PyUnicode_IsWhitespace(c)) {
                break;
            }
            만약 (c == '\r' || c == '\n') {
                has_newline = 1;
            }
            len--;
        }
        만약 (!has_newline || len == 0 ||
            PyUnicode_READ_CHAR(exprstr, len - 1) != '\\')
        {
            break;
        }
        len--;
    }

    /* Preserve unexpected metadata instead of dropping source text. */
    만약 (len == 0 || PyUnicode_READ_CHAR(exprstr, len - 1) != '=') {
        반환 Py_NewRef(exprstr);
    }

    반환 PyUnicode_Substring(exprstr, 0, len - 1);
}

expr_ty _PyPegen_interpolation(Parser *p, expr_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, 정수 lineno, 정수 col_offset,
                                 정수 end_lineno, 정수 end_col_offset, PyArena *arena) {

    정수 conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    /* Find the non whitespace token after the "=" */
    정수 debug_end_line, debug_end_offset;
    PyObject *debug_metadata;
    constant exprstr;

    만약 (conversion) {
        debug_end_line = ((expr_ty) conversion->result)->lineno;
        debug_end_offset = ((expr_ty) conversion->result)->col_offset;
        debug_metadata = exprstr = conversion->metadata;
    }
    아니면 만약 (format) {
        debug_end_line = ((expr_ty) format->result)->lineno;
        debug_end_offset = ((expr_ty) format->result)->col_offset + 1;
        debug_metadata = exprstr = format->metadata;
    }
    아니면 {
        debug_end_line = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = exprstr = closing_brace->metadata;
    }

    assert(exprstr != NULL);
    PyObject *final_exprstr = debug
        ? _strip_interpolation_debug_expr(exprstr)
        : Py_NewRef(exprstr);
    만약 (!final_exprstr || _PyArena_AddPyObject(arena, final_exprstr) < 0) {
        Py_XDECREF(final_exprstr);
        반환 NULL;
    }

    expr_ty interpolation = _PyAST_Interpolation(
        expression, final_exprstr, conversion_val, format ? (expr_ty) format->result : NULL,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    만약 (!interpolation || !debug) {
        반환 interpolation;
    }

    expr_ty debug_text = _PyAST_Constant(debug_metadata, NULL, lineno, col_offset + 1, debug_end_line,
                                            debug_end_offset - 1, p->arena);
    만약 (!debug_text) {
        반환 NULL;
    }

    asdl_expr_seq *values = _Py_asdl_expr_seq_new(2, arena);
    만약 (!values) {
        반환 NULL;
    }
    asdl_seq_SET(values, 0, debug_text);
    asdl_seq_SET(values, 1, interpolation);
    반환 _PyAST_JoinedStr(values, lineno, col_offset, debug_end_line, debug_end_offset, p->arena);
}

expr_ty _PyPegen_formatted_value(Parser *p, expr_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, 정수 lineno, 정수 col_offset,
                                 정수 end_lineno, 정수 end_col_offset, PyArena *arena) {
    정수 conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    expr_ty formatted_value = _PyAST_FormattedValue(
        expression, conversion_val, format ? (expr_ty) format->result : NULL,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    만약 (!formatted_value || !debug) {
        반환 formatted_value;
    }

    /* Find the non whitespace token after the "=" */
    정수 debug_end_line, debug_end_offset;
    PyObject *debug_metadata;

    만약 (conversion) {
        debug_end_line = ((expr_ty) conversion->result)->lineno;
        debug_end_offset = ((expr_ty) conversion->result)->col_offset;
        debug_metadata = conversion->metadata;
    }
    아니면 만약 (format) {
        debug_end_line = ((expr_ty) format->result)->lineno;
        debug_end_offset = ((expr_ty) format->result)->col_offset + 1;
        debug_metadata = format->metadata;
    }
    아니면 {
        debug_end_line = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = closing_brace->metadata;
    }
    expr_ty debug_text = _PyAST_Constant(debug_metadata, NULL, lineno, col_offset + 1, debug_end_line,
                                            debug_end_offset - 1, p->arena);
    만약 (!debug_text) {
        반환 NULL;
    }

    asdl_expr_seq *values = _Py_asdl_expr_seq_new(2, arena);
    만약 (!values) {
        반환 NULL;
    }
    asdl_seq_SET(values, 0, debug_text);
    asdl_seq_SET(values, 1, formatted_value);
    반환 _PyAST_JoinedStr(values, lineno, col_offset, debug_end_line, debug_end_offset, p->arena);
}

static expr_ty
_build_concatenated_bytes(Parser *p, asdl_expr_seq *strings, 정수 lineno,
                        정수 col_offset, 정수 end_lineno, 정수 end_col_offset,
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
    위해 (Py_ssize_t i = 0; i < len; i++) {
        expr_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.value;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        만약 (part > PY_SSIZE_T_MAX - total) {
            PyErr_NoMemory();
            반환 NULL;
        }
        total += part;
    }

    PyBytesWriter *writer = PyBytesWriter_Create(total);
    만약 (writer == NULL) {
        반환 NULL;
    }
    char *out = PyBytesWriter_GetData(writer);

    위해 (Py_ssize_t i = 0; i < len; i++) {
        expr_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.value;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        만약 (part > 0) {
            memcpy(out, PyBytes_AS_STRING(bytes), part);
            out += part;
        }
    }

    PyObject *res = PyBytesWriter_Finish(writer);
    만약 (res == NULL) {
        반환 NULL;
    }
    만약 (_PyArena_AddPyObject(arena, res) < 0) {
        Py_DECREF(res);
        반환 NULL;
    }
    반환 _PyAST_Constant(res, kind, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

static expr_ty
_build_concatenated_unicode(Parser *p, asdl_expr_seq *strings, 정수 lineno,
                        정수 col_offset, 정수 end_lineno, 정수 end_col_offset,
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
    만약 (writer == NULL) {
        반환 NULL;
    }

    위해 (Py_ssize_t i = 0; i < len; i++) {
        expr_ty current_elem = asdl_seq_GET(strings, i);
        assert(current_elem->kind == Constant_kind);

        만약 (PyUnicodeWriter_WriteStr(writer,
                                     current_elem->v.Constant.value)) {
            PyUnicodeWriter_Discard(writer);
            반환 NULL;
        }
    }

    PyObject *final = PyUnicodeWriter_Finish(writer);
    만약 (final == NULL) {
        반환 NULL;
    }
    만약 (_PyArena_AddPyObject(p->arena, final) < 0) {
        Py_DECREF(final);
        반환 NULL;
    }
    반환 _PyAST_Constant(final, kind, lineno, col_offset,
                           end_lineno, end_col_offset, arena);
}

static asdl_expr_seq *
_build_concatenated_str(Parser *p, asdl_expr_seq *strings,
                               정수 lineno, 정수 col_offset, 정수 end_lineno,
                               정수 end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    assert(len > 0);

    Py_ssize_t n_flattened_elements = 0;
    위해 (Py_ssize_t i = 0; i < len; i++) {
        expr_ty elem = asdl_seq_GET(strings, i);
        switch(elem->kind) {
            case JoinedStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.JoinedStr.values);
                break;
            case TemplateStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.TemplateStr.values);
                break;
            default:
                n_flattened_elements++;
                break;
        }
    }


    asdl_expr_seq* flattened = _Py_asdl_expr_seq_new(n_flattened_elements, p->arena);
    만약 (flattened == NULL) {
        반환 NULL;
    }

    /* build flattened list */
    Py_ssize_t current_pos = 0;
    위해 (Py_ssize_t i = 0; i < len; i++) {
        expr_ty elem = asdl_seq_GET(strings, i);
        switch(elem->kind) {
            case JoinedStr_kind:
                위해 (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.JoinedStr.values); j++) {
                    expr_ty subvalue = asdl_seq_GET(elem->v.JoinedStr.values, j);
                    만약 (subvalue == NULL) {
                        반환 NULL;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                break;
            case TemplateStr_kind:
                위해 (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.TemplateStr.values); j++) {
                    expr_ty subvalue = asdl_seq_GET(elem->v.TemplateStr.values, j);
                    만약 (subvalue == NULL) {
                        반환 NULL;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                break;
            default:
                asdl_seq_SET(flattened, current_pos++, elem);
                break;
        }
    }

    /* calculate folded element count */
    Py_ssize_t n_elements = 0;
    정수 prev_is_constant = 0;
    위해 (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        expr_ty elem = asdl_seq_GET(flattened, i);

        /* The concatenation of a FormattedValue and an empty Constant should
           lead to the FormattedValue itself. Thus, we will not take any empty
           constants into account, just as in `_PyPegen_joined_str` */
        만약 (elem->kind == Constant_kind &&
            PyUnicode_CheckExact(elem->v.Constant.value) &&
            PyUnicode_GET_LENGTH(elem->v.Constant.value) == 0)
            continue;

        만약 (!prev_is_constant || elem->kind != Constant_kind) {
            n_elements++;
        }
        prev_is_constant = elem->kind == Constant_kind;
    }

    asdl_expr_seq* values = _Py_asdl_expr_seq_new(n_elements, p->arena);
    만약 (values == NULL) {
        반환 NULL;
    }

    /* build folded list */
    current_pos = 0;
    위해 (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        expr_ty elem = asdl_seq_GET(flattened, i);

        /* if the current elem and the following are constants,
           fold them and all consequent constants */
        만약 (elem->kind == Constant_kind) {
            만약 (i + 1 < n_flattened_elements &&
                asdl_seq_GET(flattened, i + 1)->kind == Constant_kind) {
                expr_ty first_elem = elem;

                /* When a string is getting concatenated, the kind of the string
                   is determined by the first string in the concatenation
                   sequence.

                   u"abc" "def" -> u"abcdef"
                   "abc" u"abc" ->  "abcabc" */
                PyObject *kind = elem->v.Constant.kind;

                PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
                만약 (writer == NULL) {
                    반환 NULL;
                }
                expr_ty last_elem = elem;
                Py_ssize_t j;
                위해 (j = i; j < n_flattened_elements; j++) {
                    expr_ty current_elem = asdl_seq_GET(flattened, j);
                    만약 (current_elem->kind == Constant_kind) {
                        만약 (PyUnicodeWriter_WriteStr(writer,
                                                     current_elem->v.Constant.value)) {
                            PyUnicodeWriter_Discard(writer);
                            반환 NULL;
                        }
                        last_elem = current_elem;
                    } 아니면 {
                        break;
                    }
                }
                i = j - 1;

                PyObject *concat_str = PyUnicodeWriter_Finish(writer);
                만약 (concat_str == NULL) {
                    반환 NULL;
                }
                만약 (_PyArena_AddPyObject(p->arena, concat_str) < 0) {
                    Py_DECREF(concat_str);
                    반환 NULL;
                }
                elem = _PyAST_Constant(concat_str, kind, first_elem->lineno,
                                       first_elem->col_offset,
                                       last_elem->end_lineno,
                                       last_elem->end_col_offset, p->arena);
                만약 (elem == NULL) {
                    반환 NULL;
                }
            }

            /* Drop all empty contanst strings */
            만약 (PyUnicode_CheckExact(elem->v.Constant.value) &&
                PyUnicode_GET_LENGTH(elem->v.Constant.value) == 0) {
                continue;
            }
        }

        asdl_seq_SET(values, current_pos++, elem);
    }

    assert(current_pos == n_elements);
    반환 values;
}

static expr_ty
_build_concatenated_joined_str(Parser *p, asdl_expr_seq *strings,
                               정수 lineno, 정수 col_offset, 정수 end_lineno,
                               정수 end_col_offset, PyArena *arena)
{
    asdl_expr_seq *values = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    만약 (!values) {
        반환 NULL;
    }
    반환 _PyAST_JoinedStr(values, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

expr_ty
_PyPegen_concatenate_tstrings(Parser *p, asdl_expr_seq *strings,
                               정수 lineno, 정수 col_offset, 정수 end_lineno,
                               정수 end_col_offset, PyArena *arena)
{
    asdl_expr_seq *values = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    만약 (!values) {
        반환 NULL;
    }
    반환 _PyAST_TemplateStr(values, lineno, col_offset, end_lineno,
        end_col_offset, arena);
}

expr_ty
_PyPegen_concatenate_strings(Parser *p, asdl_expr_seq *strings,
                             정수 lineno, 정수 col_offset, 정수 end_lineno,
                             정수 end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    assert(len > 0);

    정수 f_string_found = 0;
    정수 unicode_string_found = 0;
    정수 bytes_found = 0;

    Py_ssize_t i = 0;
    위해 (i = 0; i < len; i++) {
        expr_ty elem = asdl_seq_GET(strings, i);
        switch(elem->kind) {
            case Constant_kind:
                만약 (PyBytes_CheckExact(elem->v.Constant.value)) {
                    bytes_found = 1;
                } 아니면 {
                    unicode_string_found = 1;
                }
                break;
            case JoinedStr_kind:
                f_string_found = 1;
                break;
            case TemplateStr_kind:
                // python.gram handles this; we should never get here
                assert(0);
                break;
            default:
                f_string_found = 1;
                break;
        }
    }

    // Cannot mix unicode and bytes
    만약 ((unicode_string_found || f_string_found) && bytes_found) {
        RAISE_SYNTAX_ERROR("cannot mix bytes and nonbytes literals");
        반환 NULL;
    }

    // If it's only bytes or only unicode string, do a simple concat
    만약 (!f_string_found) {
        만약 (len == 1) {
            반환 asdl_seq_GET(strings, 0);
        }
        아니면 만약 (bytes_found) {
            반환 _build_concatenated_bytes(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
        아니면 {
            반환 _build_concatenated_unicode(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
    }

    반환 _build_concatenated_joined_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
}

static 정수
_warn_relative_import_of_lazy(Parser *p, asdl_seq *dots, expr_ty module)
{
    // Warn about `from . lazy import x`: the whitespace between the dots and
    // the module name is insignificant, so this is parsed exactly like
    // `from .lazy import x` (an import of the relative module "lazy"), but it
    // is most likely a transposition of `lazy from . import x` (PEP 810).
    만약 (p->call_invalid_rules) {
        반환 0;
    }

    // Only fire if there is whitespace between the last dot and the name,
    // i.e. not for the common `from .lazy import x` spelling.
    Token *last_dot = asdl_seq_GET_UNTYPED(dots, asdl_seq_LEN(dots) - 1);
    만약 (
        last_dot->end_lineno == module->lineno
        && last_dot->end_col_offset == module->col_offset
    ) {
        반환 0;
    }

    정수 count = _PyPegen_seq_count_dots(dots);
    char *buf = PyMem_RawMalloc(count + 1);
    만약 (buf == NULL) {
        PyErr_NoMemory();
        반환 -1;
    }
    memset(buf, '.', count);
    buf[count] = '\0';

    PyObject *msg = PyUnicode_FromFormat(
        "'from %s lazy import' is the same as 'from %slazy import'; "
        "did you mean 'lazy from %s import'?",
        buf, buf, buf);
    PyMem_RawFree(buf);
    만약 (msg == NULL) {
        반환 -1;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    정수 res = _PyErr_EmitSyntaxWarning(msg,
                                       info.filename,
                                       module->lineno,
                                       module->col_offset + 1,
                                       module->end_lineno,
                                       module->end_col_offset + 1,
                                       info.module);
    Py_DECREF(msg);
    반환 res;
}

stmt_ty
_PyPegen_checked_from_import(Parser *p, asdl_seq *dots, expr_ty module_name,
                             asdl_alias_seq *names, expr_ty lazy_token, 정수 lineno,
                             정수 col_offset, 정수 end_lineno, 정수 end_col_offset,
                             PyArena *arena)
{
    identifier module = module_name->v.Name.id;
    정수 level = _PyPegen_seq_count_dots(dots);
    만약 (level == 0 && PyUnicode_CompareWithASCIIString(module, "__future__") == 0) {
        만약 (lazy_token) {
            RAISE_SYNTAX_ERROR_KNOWN_LOCATION(lazy_token,
                "lazy from __future__ import is not allowed");
            반환 NULL;
        }
        위해 (Py_ssize_t i = 0; i < asdl_seq_LEN(names); i++) {
            alias_ty alias = asdl_seq_GET(names, i);
            만약 (PyUnicode_CompareWithASCIIString(alias->name, "barry_as_FLUFL") == 0) {
                p->flags |= PyPARSE_BARRY_AS_BDFL;
                p->tok->barry_as_bdfl = 1;
            }
        }
    }
    아니면 만약 (
        level > 0
        && lazy_token == NULL
        && PyUnicode_CompareWithASCIIString(module, "lazy") == 0
    ) {
        만약 (_warn_relative_import_of_lazy(p, dots, module_name) < 0) {
            반환 NULL;
        }
    }
    반환 _PyAST_ImportFrom(module, names, level, lazy_token ? 1 : 0, lineno,
                             col_offset, end_lineno, end_col_offset, arena);
}

asdl_stmt_seq*
_PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts) {
    만약 (!p->call_invalid_rules) {
        반환 stmts;
    }
    Py_ssize_t len = asdl_seq_LEN(stmts);
    만약 (len == 0) {
        반환 stmts;
    }
    stmt_ty last_stmt = asdl_seq_GET(stmts, len - 1);
    만약 (p->last_stmt_location.lineno > last_stmt->lineno) {
        반환 stmts;
    }
    p->last_stmt_location.lineno = last_stmt->lineno;
    p->last_stmt_location.col_offset = last_stmt->col_offset;
    p->last_stmt_location.end_lineno = last_stmt->end_lineno;
    p->last_stmt_location.end_col_offset = last_stmt->end_col_offset;
    반환 stmts;
}
