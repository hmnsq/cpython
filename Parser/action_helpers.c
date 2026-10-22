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
    ត្រឡប់ &_PyRuntime.parser.dummy_name;
}

/* Creates a single-element asdl_seq* that contains a */
asdl_seq *
_PyPegen_singleton_seq(Parser *p, void *a)
{
    assert(a != NULL);
    asdl_seq *seq = (asdl_seq*)_Py_asdl_generic_seq_new(1, p->arena);
    ប្រសិនបើ (!seq) {
        ត្រឡប់ NULL;
    }
    asdl_seq_SET_UNTYPED(seq, 0, a);
    ត្រឡប់ seq;
}

/* Creates a copy of seq and prepends a to it */
asdl_seq *
_PyPegen_seq_insert_in_front(Parser *p, void *a, asdl_seq *seq)
{
    assert(a != NULL);
    ប្រសិនបើ (!seq) {
        ត្រឡប់ _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    ប្រសិនបើ (!new_seq) {
        ត្រឡប់ NULL;
    }

    asdl_seq_SET_UNTYPED(new_seq, 0, a);
    សម្រាប់ (Py_ssize_t i = 1, l = asdl_seq_LEN(new_seq); i < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i - 1));
    }
    ត្រឡប់ new_seq;
}

/* Creates a copy of seq and appends a to it */
asdl_seq *
_PyPegen_seq_append_to_end(Parser *p, asdl_seq *seq, void *a)
{
    assert(a != NULL);
    ប្រសិនបើ (!seq) {
        ត្រឡប់ _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    ប្រសិនបើ (!new_seq) {
        ត្រឡប់ NULL;
    }

    សម្រាប់ (Py_ssize_t i = 0, l = asdl_seq_LEN(new_seq); i + 1 < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i));
    }
    asdl_seq_SET_UNTYPED(new_seq, asdl_seq_LEN(new_seq) - 1, a);
    ត្រឡប់ new_seq;
}

static Py_ssize_t
_get_flattened_seq_size(asdl_seq *seqs)
{
    Py_ssize_t size = 0;
    សម្រាប់ (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        size += asdl_seq_LEN(inner_seq);
    }
    ត្រឡប់ size;
}

/* Flattens an asdl_seq* of asdl_seq*s */
asdl_seq *
_PyPegen_seq_flatten(Parser *p, asdl_seq *seqs)
{
    Py_ssize_t flattened_seq_size = _get_flattened_seq_size(seqs);
    assert(flattened_seq_size > 0);

    asdl_seq *flattened_seq = (asdl_seq*)_Py_asdl_generic_seq_new(flattened_seq_size, p->arena);
    ប្រសិនបើ (!flattened_seq) {
        ត្រឡប់ NULL;
    }

    ចំនួនគត់ flattened_seq_idx = 0;
    សម្រាប់ (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        សម្រាប់ (Py_ssize_t j = 0, li = asdl_seq_LEN(inner_seq); j < li; j++) {
            asdl_seq_SET_UNTYPED(flattened_seq, flattened_seq_idx++, asdl_seq_GET_UNTYPED(inner_seq, j));
        }
    }
    assert(flattened_seq_idx == flattened_seq_size);

    ត្រឡប់ flattened_seq;
}

void *
_PyPegen_seq_last_item(asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ត្រឡប់ asdl_seq_GET_UNTYPED(seq, len - 1);
}

void *
_PyPegen_seq_first_item(asdl_seq *seq)
{
    ត្រឡប់ asdl_seq_GET_UNTYPED(seq, 0);
}

/* Creates a new name of the form <first_name>.<second_name> */
expr_ty
_PyPegen_join_names_with_dot(Parser *p, expr_ty first_name, expr_ty second_name)
{
    assert(first_name != NULL && second_name != NULL);
    PyObject *uni = PyUnicode_FromFormat("%U.%U",
            first_name->v.Name.id, second_name->v.Name.id);
    ប្រសិនបើ (!uni) {
        ត្រឡប់ NULL;
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &uni);
    ប្រសិនបើ (_PyArena_AddPyObject(p->arena, uni) < 0) {
        Py_DECREF(uni);
        ត្រឡប់ NULL;
    }

    ត្រឡប់ _PyAST_Name(uni, Load, EXTRA_EXPR(first_name, second_name));
}

/* Counts the total number of dots in seq's tokens */
ចំនួនគត់
_PyPegen_seq_count_dots(asdl_seq *seq)
{
    ចំនួនគត់ number_of_dots = 0;
    សម្រាប់ (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
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

    ត្រឡប់ number_of_dots;
}

/* Creates an alias with '*' as the identifier name */
alias_ty
_PyPegen_alias_for_star(Parser *p, ចំនួនគត់ lineno, ចំនួនគត់ col_offset, ចំនួនគត់ end_lineno,
                        ចំនួនគត់ end_col_offset, PyArena *arena) {
    PyObject *str = PyUnicode_InternFromString("*");
    ប្រសិនបើ (!str) {
        ត្រឡប់ NULL;
    }
    ប្រសិនបើ (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ត្រឡប់ NULL;
    }
    ត្រឡប់ _PyAST_alias(str, NULL, lineno, col_offset, end_lineno, end_col_offset, arena);
}

/* Creates a new asdl_seq* with the identifiers of all the names in seq */
asdl_identifier_seq *
_PyPegen_map_names_to_ids(Parser *p, asdl_expr_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    assert(len > 0);

    asdl_identifier_seq *new_seq = _Py_asdl_identifier_seq_new(len, p->arena);
    ប្រសិនបើ (!new_seq) {
        ត្រឡប់ NULL;
    }
    សម្រាប់ (Py_ssize_t i = 0; i < len; i++) {
        expr_ty e = asdl_seq_GET(seq, i);
        asdl_seq_SET(new_seq, i, e->v.Name.id);
    }
    ត្រឡប់ new_seq;
}

/* Constructs a CmpopExprPair */
CmpopExprPair *
_PyPegen_cmpop_expr_pair(Parser *p, cmpop_ty cmpop, expr_ty expr)
{
    assert(expr != NULL);
    CmpopExprPair *a = _PyArena_Malloc(p->arena, parimaa_na(CmpopExprPair));
    ប្រសិនបើ (!a) {
        ត្រឡប់ NULL;
    }
    a->cmpop = cmpop;
    a->expr = expr;
    ត្រឡប់ a;
}

asdl_int_seq *
_PyPegen_get_cmpops(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    assert(len > 0);

    asdl_int_seq *new_seq = _Py_asdl_int_seq_new(len, p->arena);
    ប្រសិនបើ (!new_seq) {
        ត្រឡប់ NULL;
    }
    សម្រាប់ (Py_ssize_t i = 0; i < len; i++) {
        CmpopExprPair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->cmpop);
    }
    ត្រឡប់ new_seq;
}

asdl_expr_seq *
_PyPegen_get_exprs(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    assert(len > 0);

    asdl_expr_seq *new_seq = _Py_asdl_expr_seq_new(len, p->arena);
    ប្រសិនបើ (!new_seq) {
        ត្រឡប់ NULL;
    }
    សម្រាប់ (Py_ssize_t i = 0; i < len; i++) {
        CmpopExprPair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->expr);
    }
    ត្រឡប់ new_seq;
}

/* Creates an asdl_seq* where all the elements have been changed to have ctx as context */
static asdl_expr_seq *
_set_seq_context(Parser *p, asdl_expr_seq *seq, expr_context_ty ctx)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ប្រសិនបើ (len == 0) {
        ត្រឡប់ NULL;
    }

    asdl_expr_seq *new_seq = _Py_asdl_expr_seq_new(len, p->arena);
    ប្រសិនបើ (!new_seq) {
        ត្រឡប់ NULL;
    }
    សម្រាប់ (Py_ssize_t i = 0; i < len; i++) {
        expr_ty e = asdl_seq_GET(seq, i);
        expr_ty new_e = _PyPegen_set_expr_context(p, e, ctx);
        ប្រសិនបើ (!new_e) {
            ត្រឡប់ NULL;
        }
        asdl_seq_SET(new_seq, i, new_e);
    }
    ត្រឡប់ new_seq;
}

static expr_ty
_set_name_context(Parser *p, expr_ty e, expr_context_ty ctx)
{
    ត្រឡប់ _PyAST_Name(e->v.Name.id, ctx, EXTRA_EXPR(e, e));
}

static expr_ty
_set_tuple_context(Parser *p, expr_ty e, expr_context_ty ctx)
{
    asdl_expr_seq *seq = _set_seq_context(p, e->v.Tuple.elts, ctx);
    ប្រសិនបើ (!seq && PyErr_Occurred()) {
        ត្រឡប់ NULL;
    }
    ត្រឡប់ _PyAST_Tuple(seq, ctx, EXTRA_EXPR(e, e));
}

static expr_ty
_set_list_context(Parser *p, expr_ty e, expr_context_ty ctx)
{
    asdl_expr_seq *seq = _set_seq_context(p, e->v.List.elts, ctx);
    ប្រសិនបើ (!seq && PyErr_Occurred()) {
        ត្រឡប់ NULL;
    }
    ត្រឡប់ _PyAST_List(seq, ctx, EXTRA_EXPR(e, e));
}

static expr_ty
_set_subscript_context(Parser *p, expr_ty e, expr_context_ty ctx)
{
    ត្រឡប់ _PyAST_Subscript(e->v.Subscript.value, e->v.Subscript.slice,
                            ctx, EXTRA_EXPR(e, e));
}

static expr_ty
_set_attribute_context(Parser *p, expr_ty e, expr_context_ty ctx)
{
    ត្រឡប់ _PyAST_Attribute(e->v.Attribute.value, e->v.Attribute.attr,
                            ctx, EXTRA_EXPR(e, e));
}

static expr_ty
_set_starred_context(Parser *p, expr_ty e, expr_context_ty ctx)
{
    expr_ty inner = _PyPegen_set_expr_context(p, e->v.Starred.value, ctx);
    ប្រសិនបើ (!inner) {
        ត្រឡប់ NULL;
    }
    ត្រឡប់ _PyAST_Starred(inner, ctx, EXTRA_EXPR(e, e));
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
    ត្រឡប់ new;
}

/* Invalid targets return NULL without raising, allowing parsing to backtrack.
   Copy valid targets to preserve the memoized expression's context. */
expr_ty
_PyPegen_make_target(Parser *p, expr_ty expr, TARGETS_TYPE targets_type)
{
    assert(expr != NULL);
    assert(targets_type != FOR_TARGETS);
    ប្រសិនបើ (_PyPegen_get_invalid_target(expr, targets_type) != NULL) {
        ត្រឡប់ NULL;
    }
    ត្រឡប់ _PyPegen_set_expr_context(
        p, expr, targets_type == DEL_TARGETS ? Del : Store);
}

/* Constructs a KeyValuePair that is used when parsing a dict's key value pairs */
KeyValuePair *
_PyPegen_key_value_pair(Parser *p, expr_ty key, expr_ty value)
{
    KeyValuePair *a = _PyArena_Malloc(p->arena, parimaa_na(KeyValuePair));
    ប្រសិនបើ (!a) {
        ត្រឡប់ NULL;
    }
    a->key = key;
    a->value = value;
    ត្រឡប់ a;
}

/* Extracts all keys from an asdl_seq* of KeyValuePair*'s */
asdl_expr_seq *
_PyPegen_get_keys(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_expr_seq *new_seq = _Py_asdl_expr_seq_new(len, p->arena);
    ប្រសិនបើ (!new_seq) {
        ត្រឡប់ NULL;
    }
    សម្រាប់ (Py_ssize_t i = 0; i < len; i++) {
        KeyValuePair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->key);
    }
    ត្រឡប់ new_seq;
}

/* Extracts all values from an asdl_seq* of KeyValuePair*'s */
asdl_expr_seq *
_PyPegen_get_values(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_expr_seq *new_seq = _Py_asdl_expr_seq_new(len, p->arena);
    ប្រសិនបើ (!new_seq) {
        ត្រឡប់ NULL;
    }
    សម្រាប់ (Py_ssize_t i = 0; i < len; i++) {
        KeyValuePair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->value);
    }
    ត្រឡប់ new_seq;
}

/* Constructs a KeyPatternPair that is used when parsing mapping & class patterns */
KeyPatternPair *
_PyPegen_key_pattern_pair(Parser *p, expr_ty key, pattern_ty pattern)
{
    KeyPatternPair *a = _PyArena_Malloc(p->arena, parimaa_na(KeyPatternPair));
    ប្រសិនបើ (!a) {
        ត្រឡប់ NULL;
    }
    a->key = key;
    a->pattern = pattern;
    ត្រឡប់ a;
}

/* Extracts all keys from an asdl_seq* of KeyPatternPair*'s */
asdl_expr_seq *
_PyPegen_get_pattern_keys(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_expr_seq *new_seq = _Py_asdl_expr_seq_new(len, p->arena);
    ប្រសិនបើ (!new_seq) {
        ត្រឡប់ NULL;
    }
    សម្រាប់ (Py_ssize_t i = 0; i < len; i++) {
        KeyPatternPair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->key);
    }
    ត្រឡប់ new_seq;
}

/* Extracts all patterns from an asdl_seq* of KeyPatternPair*'s */
asdl_pattern_seq *
_PyPegen_get_patterns(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_pattern_seq *new_seq = _Py_asdl_pattern_seq_new(len, p->arena);
    ប្រសិនបើ (!new_seq) {
        ត្រឡប់ NULL;
    }
    សម្រាប់ (Py_ssize_t i = 0; i < len; i++) {
        KeyPatternPair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->pattern);
    }
    ត្រឡប់ new_seq;
}

/* Constructs a NameDefaultPair */
NameDefaultPair *
_PyPegen_name_default_pair(Parser *p, arg_ty arg, expr_ty value, Token *tc)
{
    NameDefaultPair *a = _PyArena_Malloc(p->arena, parimaa_na(NameDefaultPair));
    ប្រសិនបើ (!a) {
        ត្រឡប់ NULL;
    }
    a->arg = _PyPegen_add_type_comment_to_arg(p, arg, tc);
    ប្រសិនបើ (!a->arg) {
        ត្រឡប់ NULL;
    }
    a->value = value;
    ត្រឡប់ a;
}

/* Constructs a SlashWithDefault */
SlashWithDefault *
_PyPegen_slash_with_default(Parser *p, asdl_arg_seq *plain_names, asdl_seq *names_with_defaults)
{
    SlashWithDefault *a = _PyArena_Malloc(p->arena, parimaa_na(SlashWithDefault));
    ប្រសិនបើ (!a) {
        ត្រឡប់ NULL;
    }
    a->plain_names = plain_names;
    a->names_with_defaults = names_with_defaults;
    ត្រឡប់ a;
}

/* Constructs a StarEtc */
StarEtc *
_PyPegen_star_etc(Parser *p, arg_ty vararg, asdl_seq *kwonlyargs, arg_ty kwarg)
{
    StarEtc *a = _PyArena_Malloc(p->arena, parimaa_na(StarEtc));
    ប្រសិនបើ (!a) {
        ត្រឡប់ NULL;
    }
    a->vararg = vararg;
    a->kwonlyargs = kwonlyargs;
    a->kwarg = kwarg;
    ត្រឡប់ a;
}

asdl_seq *
_PyPegen_join_sequences(Parser *p, asdl_seq *a, asdl_seq *b)
{
    Py_ssize_t first_len = asdl_seq_LEN(a);
    Py_ssize_t second_len = asdl_seq_LEN(b);
    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(first_len + second_len, p->arena);
    ប្រសិនបើ (!new_seq) {
        ត្រឡប់ NULL;
    }

    ចំនួនគត់ k = 0;
    សម្រាប់ (Py_ssize_t i = 0; i < first_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(a, i));
    }
    សម្រាប់ (Py_ssize_t i = 0; i < second_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(b, i));
    }

    ត្រឡប់ new_seq;
}

static asdl_arg_seq*
_get_names(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_arg_seq *seq = _Py_asdl_arg_seq_new(len, p->arena);
    ប្រសិនបើ (!seq) {
        ត្រឡប់ NULL;
    }
    សម្រាប់ (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->arg);
    }
    ត្រឡប់ seq;
}

static asdl_expr_seq *
_get_defaults(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_expr_seq *seq = _Py_asdl_expr_seq_new(len, p->arena);
    ប្រសិនបើ (!seq) {
        ត្រឡប់ NULL;
    }
    សម្រាប់ (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->value);
    }
    ត្រឡប់ seq;
}

static ចំនួនគត់
_make_posonlyargs(Parser *p,
                  asdl_arg_seq *slash_without_default,
                  SlashWithDefault *slash_with_default,
                  asdl_arg_seq **posonlyargs) {
    ប្រសិនបើ (slash_without_default != NULL) {
        *posonlyargs = slash_without_default;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (slash_with_default != NULL) {
        asdl_arg_seq *slash_with_default_names =
                _get_names(p, slash_with_default->names_with_defaults);
        ប្រសិនបើ (!slash_with_default_names) {
            ត្រឡប់ -1;
        }
        *posonlyargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default->plain_names,
                (asdl_seq*)slash_with_default_names);
    }
    បើមិនដូច្នេះ {
        *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }
    ត្រឡប់ *posonlyargs == NULL ? -1 : 0;
}

static ចំនួនគត់
_make_posargs(Parser *p,
              asdl_arg_seq *plain_names,
              asdl_seq *names_with_default,
              asdl_arg_seq **posargs) {

    ប្រសិនបើ (names_with_default != NULL) {
        ប្រសិនបើ (plain_names != NULL) {
            asdl_arg_seq *names_with_default_names = _get_names(p, names_with_default);
            ប្រសិនបើ (!names_with_default_names) {
                ត្រឡប់ -1;
            }
            *posargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                    p,(asdl_seq*)plain_names, (asdl_seq*)names_with_default_names);
        }
        បើមិនដូច្នេះ {
            *posargs = _get_names(p, names_with_default);
        }
    }
    បើមិនដូច្នេះ {
        ប្រសិនបើ (plain_names != NULL) {
            // With the current grammar, we never get here.
            // If that has changed, remove the assert, and test thoroughly.
            assert(0);
            *posargs = plain_names;
        }
        បើមិនដូច្នេះ {
            *posargs = _Py_asdl_arg_seq_new(0, p->arena);
        }
    }
    ត្រឡប់ *posargs == NULL ? -1 : 0;
}

static ចំនួនគត់
_make_posdefaults(Parser *p,
                  SlashWithDefault *slash_with_default,
                  asdl_seq *names_with_default,
                  asdl_expr_seq **posdefaults) {
    ប្រសិនបើ (slash_with_default != NULL && names_with_default != NULL) {
        asdl_expr_seq *slash_with_default_values =
                _get_defaults(p, slash_with_default->names_with_defaults);
        ប្រសិនបើ (!slash_with_default_values) {
            ត្រឡប់ -1;
        }
        asdl_expr_seq *names_with_default_values = _get_defaults(p, names_with_default);
        ប្រសិនបើ (!names_with_default_values) {
            ត្រឡប់ -1;
        }
        *posdefaults = (asdl_expr_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default_values,
                (asdl_seq*)names_with_default_values);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (slash_with_default == NULL && names_with_default != NULL) {
        *posdefaults = _get_defaults(p, names_with_default);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (slash_with_default != NULL && names_with_default == NULL) {
        *posdefaults = _get_defaults(p, slash_with_default->names_with_defaults);
    }
    បើមិនដូច្នេះ {
        *posdefaults = _Py_asdl_expr_seq_new(0, p->arena);
    }
    ត្រឡប់ *posdefaults == NULL ? -1 : 0;
}

static ចំនួនគត់
_make_kwargs(Parser *p, StarEtc *star_etc,
             asdl_arg_seq **kwonlyargs,
             asdl_expr_seq **kwdefaults) {
    ប្រសិនបើ (star_etc != NULL && star_etc->kwonlyargs != NULL) {
        *kwonlyargs = _get_names(p, star_etc->kwonlyargs);
    }
    បើមិនដូច្នេះ {
        *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }

    ប្រសិនបើ (*kwonlyargs == NULL) {
        ត្រឡប់ -1;
    }

    ប្រសិនបើ (star_etc != NULL && star_etc->kwonlyargs != NULL) {
        *kwdefaults = _get_defaults(p, star_etc->kwonlyargs);
    }
    បើមិនដូច្នេះ {
        *kwdefaults = _Py_asdl_expr_seq_new(0, p->arena);
    }

    ប្រសិនបើ (*kwdefaults == NULL) {
        ត្រឡប់ -1;
    }

    ត្រឡប់ 0;
}

/* Constructs an arguments_ty object out of all the parsed constructs in the parameters rule */
arguments_ty
_PyPegen_make_arguments(Parser *p, asdl_arg_seq *slash_without_default,
                        SlashWithDefault *slash_with_default, asdl_arg_seq *plain_names,
                        asdl_seq *names_with_default, StarEtc *star_etc)
{
    asdl_arg_seq *posonlyargs;
    ប្រសិនបើ (_make_posonlyargs(p, slash_without_default, slash_with_default, &posonlyargs) == -1) {
        ត្រឡប់ NULL;
    }

    asdl_arg_seq *posargs;
    ប្រសិនបើ (_make_posargs(p, plain_names, names_with_default, &posargs) == -1) {
        ត្រឡប់ NULL;
    }

    asdl_expr_seq *posdefaults;
    ប្រសិនបើ (_make_posdefaults(p,slash_with_default, names_with_default, &posdefaults) == -1) {
        ត្រឡប់ NULL;
    }

    arg_ty vararg = NULL;
    ប្រសិនបើ (star_etc != NULL && star_etc->vararg != NULL) {
        vararg = star_etc->vararg;
    }

    asdl_arg_seq *kwonlyargs;
    asdl_expr_seq *kwdefaults;
    ប្រសិនបើ (_make_kwargs(p, star_etc, &kwonlyargs, &kwdefaults) == -1) {
        ត្រឡប់ NULL;
    }

    arg_ty kwarg = NULL;
    ប្រសិនបើ (star_etc != NULL && star_etc->kwarg != NULL) {
        kwarg = star_etc->kwarg;
    }

    ត្រឡប់ _PyAST_arguments(posonlyargs, posargs, vararg, kwonlyargs,
                            kwdefaults, kwarg, posdefaults, p->arena);
}


/* Constructs an empty arguments_ty object, that gets used when a function accepts no
 * arguments. */
arguments_ty
_PyPegen_empty_arguments(Parser *p)
{
    asdl_arg_seq *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    ប្រសិនបើ (!posonlyargs) {
        ត្រឡប់ NULL;
    }
    asdl_arg_seq *posargs = _Py_asdl_arg_seq_new(0, p->arena);
    ប្រសិនបើ (!posargs) {
        ត្រឡប់ NULL;
    }
    asdl_expr_seq *posdefaults = _Py_asdl_expr_seq_new(0, p->arena);
    ប្រសិនបើ (!posdefaults) {
        ត្រឡប់ NULL;
    }
    asdl_arg_seq *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    ប្រសិនបើ (!kwonlyargs) {
        ត្រឡប់ NULL;
    }
    asdl_expr_seq *kwdefaults = _Py_asdl_expr_seq_new(0, p->arena);
    ប្រសិនបើ (!kwdefaults) {
        ត្រឡប់ NULL;
    }

    ត្រឡប់ _PyAST_arguments(posonlyargs, posargs, NULL, kwonlyargs,
                            kwdefaults, NULL, posdefaults, p->arena);
}

/* Encapsulates the value of an operator_ty into an AugOperator struct */
AugOperator *
_PyPegen_augoperator(Parser *p, operator_ty kind)
{
    AugOperator *a = _PyArena_Malloc(p->arena, parimaa_na(AugOperator));
    ប្រសិនបើ (!a) {
        ត្រឡប់ NULL;
    }
    a->kind = kind;
    ត្រឡប់ a;
}

/* Construct a FunctionDef equivalent to function_def, but with decorators */
stmt_ty
_PyPegen_function_def_decorators(Parser *p, asdl_expr_seq *decorators, stmt_ty function_def)
{
    assert(function_def != NULL);
    ប្រសិនបើ (function_def->kind == AsyncFunctionDef_kind) {
        ត្រឡប់ _PyAST_AsyncFunctionDef(
            function_def->v.AsyncFunctionDef.name,
            function_def->v.AsyncFunctionDef.args,
            function_def->v.AsyncFunctionDef.body, decorators,
            function_def->v.AsyncFunctionDef.returns,
            function_def->v.AsyncFunctionDef.type_comment,
            function_def->v.AsyncFunctionDef.type_params,
            function_def->lineno, function_def->col_offset,
            function_def->end_lineno, function_def->end_col_offset, p->arena);
    }

    ត្រឡប់ _PyAST_FunctionDef(
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
    ត្រឡប់ _PyAST_ClassDef(
        class_def->v.ClassDef.name,
        class_def->v.ClassDef.bases, class_def->v.ClassDef.keywords,
        class_def->v.ClassDef.body, decorators,
        class_def->v.ClassDef.type_params,
        class_def->lineno, class_def->col_offset, class_def->end_lineno,
        class_def->end_col_offset, p->arena);
}

/* Construct a KeywordOrStarred */
KeywordOrStarred *
_PyPegen_keyword_or_starred(Parser *p, void *element, ចំនួនគត់ is_keyword)
{
    KeywordOrStarred *a = _PyArena_Malloc(p->arena, parimaa_na(KeywordOrStarred));
    ប្រសិនបើ (!a) {
        ត្រឡប់ NULL;
    }
    a->element = element;
    a->is_keyword = is_keyword;
    ត្រឡប់ a;
}

/* Get the number of starred expressions in an asdl_seq* of KeywordOrStarred*s */
static ចំនួនគត់
_seq_number_of_starred_exprs(asdl_seq *seq)
{
    ចំនួនគត់ n = 0;
    សម្រាប់ (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(seq, i);
        ប្រសិនបើ (!k->is_keyword) {
            n++;
        }
    }
    ត្រឡប់ n;
}

/* Extract the starred expressions of an asdl_seq* of KeywordOrStarred*s */
asdl_expr_seq *
_PyPegen_seq_extract_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    ចំនួនគត់ new_len = _seq_number_of_starred_exprs(kwargs);
    ប្រសិនបើ (new_len == 0) {
        ត្រឡប់ NULL;
    }
    asdl_expr_seq *new_seq = _Py_asdl_expr_seq_new(new_len, p->arena);
    ប្រសិនបើ (!new_seq) {
        ត្រឡប់ NULL;
    }

    ចំនួនគត់ idx = 0;
    សម្រាប់ (Py_ssize_t i = 0, len = asdl_seq_LEN(kwargs); i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        ប្រសិនបើ (!k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    ត្រឡប់ new_seq;
}

/* Return a new asdl_seq* with only the keywords in kwargs */
asdl_keyword_seq*
_PyPegen_seq_delete_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    Py_ssize_t len = asdl_seq_LEN(kwargs);
    Py_ssize_t new_len = len - _seq_number_of_starred_exprs(kwargs);
    ប្រសិនបើ (new_len == 0) {
        ត្រឡប់ NULL;
    }
    asdl_keyword_seq *new_seq = _Py_asdl_keyword_seq_new(new_len, p->arena);
    ប្រសិនបើ (!new_seq) {
        ត្រឡប់ NULL;
    }

    ចំនួនគត់ idx = 0;
    សម្រាប់ (Py_ssize_t i = 0; i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        ប្រសិនបើ (k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    ត្រឡប់ new_seq;
}

expr_ty
_PyPegen_ensure_imaginary(Parser *p, expr_ty exp)
{
    ប្រសិនបើ (exp->kind != Constant_kind || !PyComplex_CheckExact(exp->v.Constant.value)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "imaginary number required in complex literal");
        ត្រឡប់ NULL;
    }
    ត្រឡប់ exp;
}

expr_ty
_PyPegen_ensure_real(Parser *p, expr_ty exp)
{
    ប្រសិនបើ (exp->kind != Constant_kind || PyComplex_CheckExact(exp->v.Constant.value)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "real number required in complex literal");
        ត្រឡប់ NULL;
    }
    ត្រឡប់ exp;
}

mod_ty
_PyPegen_make_module(Parser *p, asdl_stmt_seq *a) {
    asdl_type_ignore_seq *type_ignores = NULL;
    Py_ssize_t num = p->type_ignore_comments.num_items;
    ប្រសិនបើ (num > 0) {
        // Turn the raw (comment, lineno) pairs into TypeIgnore objects in the arena
        type_ignores = _Py_asdl_type_ignore_seq_new(num, p->arena);
        ប្រសិនបើ (type_ignores == NULL) {
            ត្រឡប់ NULL;
        }
        សម្រាប់ (Py_ssize_t i = 0; i < num; i++) {
            PyObject *tag = _PyPegen_new_type_comment(p, p->type_ignore_comments.items[i].comment);
            ប្រសិនបើ (tag == NULL) {
                ត្រឡប់ NULL;
            }
            type_ignore_ty ti = _PyAST_TypeIgnore(p->type_ignore_comments.items[i].lineno,
                                                  tag, p->arena);
            ប្រសិនបើ (ti == NULL) {
                ត្រឡប់ NULL;
            }
            asdl_seq_SET(type_ignores, i, ti);
        }
    }
    ត្រឡប់ _PyAST_Module(a, type_ignores, p->arena);
}

PyObject *
_PyPegen_new_type_comment(Parser *p, const var_na *s)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, strlen(s), NULL);
    ប្រសិនបើ (res == NULL) {
        ត្រឡប់ NULL;
    }
    ប្រសិនបើ (_PyArena_AddPyObject(p->arena, res) < 0) {
        Py_DECREF(res);
        ត្រឡប់ NULL;
    }
    ត្រឡប់ res;
}

arg_ty
_PyPegen_add_type_comment_to_arg(Parser *p, arg_ty a, Token *tc)
{
    ប្រសិនបើ (tc == NULL) {
        ត្រឡប់ a;
    }
    const var_na *bytes = PyBytes_AsString(tc->bytes);
    ប្រសិនបើ (bytes == NULL) {
        ត្រឡប់ NULL;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    ប្រសិនបើ (tco == NULL) {
        ត្រឡប់ NULL;
    }
    ត្រឡប់ _PyAST_arg(a->arg, a->annotation, tco,
                      a->lineno, a->col_offset, a->end_lineno, a->end_col_offset,
                      p->arena);
}

/* Checks if the NOTEQUAL token is valid given the current parser flags
0 indicates success and nonzero indicates failure (an exception may be set) */
ចំនួនគត់
_PyPegen_check_barry_as_flufl(Parser *p, Token* t) {
    assert(t->bytes != NULL);
    assert(t->type == NOTEQUAL);

    const var_na* tok_str = PyBytes_AS_STRING(t->bytes);
    ប្រសិនបើ (p->flags & PyPARSE_BARRY_AS_BDFL && strcmp(tok_str, "<>") != 0) {
        RAISE_SYNTAX_ERROR("with Barry as BDFL, use '<>' instead of '!='");
        ត្រឡប់ -1;
    }
    ប្រសិនបើ (!(p->flags & PyPARSE_BARRY_AS_BDFL)) {
        ត្រឡប់ strcmp(tok_str, "!=");
    }
    ត្រឡប់ 0;
}

ចំនួនគត់
_PyPegen_check_legacy_stmt(Parser *p, expr_ty name) {
    ប្រសិនបើ (name->kind != Name_kind) {
        ត្រឡប់ 0;
    }
    const var_na* candidates[2] = {"print", "exec"};
    សម្រាប់ (ចំនួនគត់ i=0; i<2; i++) {
        ប្រសិនបើ (PyUnicode_CompareWithASCIIString(name->v.Name.id, candidates[i]) == 0) {
            ត្រឡប់ 1;
        }
    }
    ត្រឡប់ 0;
}

void *
_PyPegen_raise_error_for_missing_comma(Parser *p, expr_ty a, expr_ty b)
{
    // Don't raise for legacy statements like "print x" or "exec x"
    ប្រសិនបើ (_PyPegen_check_legacy_stmt(p, a)) {
        ត្រឡប់ NULL;
    }
    // Only raise inside parentheses/brackets (level > 0)
    ប្រសិនបើ (p->tokens[p->mark - 1]->level == 0) {
        ត្រឡប់ NULL;
    }
    // For multi-line expressions (like string concatenations), point to the
    // last line instead of the first for a more helpful error message.
    // Use a->col_offset as the starting column since all strings in the
    // concatenation typically share the same indentation.
    ប្រសិនបើ (a->end_lineno > a->lineno) {
        ត្រឡប់ RAISE_ERROR_KNOWN_LOCATION(
            p, PyExc_SyntaxError, a->end_lineno, a->col_offset,
            a->end_lineno, a->end_col_offset,
            "invalid syntax. Perhaps you forgot a comma?"
        );
    }
    ត្រឡប់ RAISE_ERROR_KNOWN_LOCATION(
        p, PyExc_SyntaxError, a->lineno, a->col_offset,
        b->end_lineno, b->end_col_offset,
        "invalid syntax. Perhaps you forgot a comma?"
    );
}

static ResultTokenWithMetadata *
result_token_with_metadata(Parser *p, void *result, PyObject *metadata)
{
    ResultTokenWithMetadata *res = _PyArena_Malloc(p->arena, parimaa_na(ResultTokenWithMetadata));
    ប្រសិនបើ (res == NULL) {
        ត្រឡប់ NULL;
    }
    res->metadata = metadata;
    res->result = result;
    ត្រឡប់ res;
}

static var_na
formatted_string_prefix(const Parser *p)
{
    ចំនួនគត់ nested = 0;
    សម្រាប់ (ចំនួនគត់ i = p->mark - 1; i >= 0; i--) {
        ចំនួនគត់ type = p->tokens[i]->type;
        ប្រសិនបើ (type == FSTRING_END || type == TSTRING_END) {
            nested++;
        }
        បើមិនដូច្នេះ ប្រសិនបើ (type == FSTRING_START || type == TSTRING_START) {
            ប្រសិនបើ (nested == 0) {
                ត្រឡប់ type == TSTRING_START ? 't' : 'f';
            }
            nested--;
        }
    }
    Py_UNREACHABLE();
}

ResultTokenWithMetadata *
_PyPegen_check_fstring_conversion(Parser *p, Token* conv_token, expr_ty conv)
{
    ប្រសិនបើ (conv_token->lineno != conv->lineno || conv_token->end_col_offset != conv->col_offset) {
        ត្រឡប់ RAISE_SYNTAX_ERROR_KNOWN_RANGE(
            conv_token, conv,
            "%c-string: conversion type must come right after the exclamation mark",
            formatted_string_prefix(p)
        );
    }

    Py_UCS4 first = PyUnicode_READ_CHAR(conv->v.Name.id, 0);
    ប្រសិនបើ (PyUnicode_GET_LENGTH(conv->v.Name.id) > 1 ||
            !(first == 's' || first == 'r' || first == 'a')) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(conv,
                                            "%c-string: invalid conversion character %R: expected 's', 'r', or 'a'",
                                            formatted_string_prefix(p),
                                            conv->v.Name.id);
        ត្រឡប់ NULL;
    }

    ត្រឡប់ result_token_with_metadata(p, conv, conv_token->metadata);
}

ResultTokenWithMetadata *
_PyPegen_setup_full_format_spec(Parser *p, Token *colon, asdl_expr_seq *spec, ចំនួនគត់ lineno, ចំនួនគត់ col_offset,
                                ចំនួនគត់ end_lineno, ចំនួនគត់ end_col_offset, PyArena *arena)
{
    ប្រសិនបើ (!spec) {
        ត្រឡប់ NULL;
    }

    // This is needed to keep compatibility with 3.11, where an empty format
    // spec is parsed as an *empty* JoinedStr node, instead of having an empty
    // constant in it.
    Py_ssize_t n_items = asdl_seq_LEN(spec);
    Py_ssize_t non_empty_count = 0;
    សម្រាប់ (Py_ssize_t i = 0; i < n_items; i++) {
        expr_ty item = asdl_seq_GET(spec, i);
        non_empty_count += !(item->kind == Constant_kind &&
                             PyUnicode_CheckExact(item->v.Constant.value) &&
                             PyUnicode_GET_LENGTH(item->v.Constant.value) == 0);
    }
    ប្រសិនបើ (non_empty_count != n_items) {
        asdl_expr_seq *resized_spec =
            _Py_asdl_expr_seq_new(non_empty_count, p->arena);
        ប្រសិនបើ (resized_spec == NULL) {
            ត្រឡប់ NULL;
        }
        Py_ssize_t j = 0;
        សម្រាប់ (Py_ssize_t i = 0; i < n_items; i++) {
            expr_ty item = asdl_seq_GET(spec, i);
            ប្រសិនបើ (item->kind == Constant_kind &&
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
    ប្រសិនបើ (n == 0 || (n == 1 && asdl_seq_GET(spec, 0)->kind == Constant_kind)) {
        res = _PyAST_JoinedStr(spec, lineno, col_offset, end_lineno,
                                    end_col_offset, p->arena);
    } បើមិនដូច្នេះ {
        res = _PyPegen_concatenate_strings(p, spec,
                             lineno, col_offset, end_lineno,
                             end_col_offset, arena);
    }
    ប្រសិនបើ (!res) {
        ត្រឡប់ NULL;
    }
    ត្រឡប់ result_token_with_metadata(p, res, colon->metadata);
}

const var_na *
_PyPegen_get_expr_name(expr_ty e)
{
    assert(e != NULL);
    switch (e->kind) {
        pak_sa Attribute_kind:
            ត្រឡប់ "attribute";
        pak_sa Subscript_kind:
            ត្រឡប់ "subscript";
        pak_sa Starred_kind:
            ត្រឡប់ "starred";
        pak_sa Name_kind:
            ត្រឡប់ "name";
        pak_sa List_kind:
            ត្រឡប់ "list";
        pak_sa Tuple_kind:
            ត្រឡប់ "tuple";
        pak_sa Lambda_kind:
            ត្រឡប់ "lambda";
        pak_sa Call_kind:
            ត្រឡប់ "function call";
        pak_sa BoolOp_kind:
        pak_sa BinOp_kind:
        pak_sa UnaryOp_kind:
            ត្រឡប់ "expression";
        pak_sa GeneratorExp_kind:
            ត្រឡប់ "generator expression";
        pak_sa Yield_kind:
        pak_sa YieldFrom_kind:
            ត្រឡប់ "yield expression";
        pak_sa Await_kind:
            ត្រឡប់ "await expression";
        pak_sa ListComp_kind:
            ត្រឡប់ "list comprehension";
        pak_sa SetComp_kind:
            ត្រឡប់ "set comprehension";
        pak_sa DictComp_kind:
            ត្រឡប់ "dict comprehension";
        pak_sa Dict_kind:
            ត្រឡប់ "dict literal";
        pak_sa Set_kind:
            ត្រឡប់ "set display";
        pak_sa JoinedStr_kind:
        pak_sa FormattedValue_kind:
            ត្រឡប់ "f-string expression";
        pak_sa TemplateStr_kind:
        pak_sa Interpolation_kind:
            ត្រឡប់ "t-string expression";
        pak_sa Constant_kind: {
            PyObject *value = e->v.Constant.value;
            ប្រសិនបើ (value == Py_None) {
                ត្រឡប់ "None";
            }
            ប្រសិនបើ (value == Py_False) {
                ត្រឡប់ "False";
            }
            ប្រសិនបើ (value == Py_True) {
                ត្រឡប់ "True";
            }
            ប្រសិនបើ (value == Py_Ellipsis) {
                ត្រឡប់ "ellipsis";
            }
            ត្រឡប់ "literal";
        }
        pak_sa Compare_kind:
            ត្រឡប់ "comparison";
        pak_sa IfExp_kind:
            ត្រឡប់ "conditional expression";
        pak_sa NamedExpr_kind:
            ត្រឡប់ "named expression";
        default:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->kind, e->lineno);
            ត្រឡប់ NULL;
    }
}

expr_ty
_PyPegen_get_last_comprehension_item(comprehension_ty comprehension) {
    ប្រសិនបើ (comprehension->ifs == NULL || asdl_seq_LEN(comprehension->ifs) == 0) {
        ត្រឡប់ comprehension->iter;
    }
    ត្រឡប់ PyPegen_last_item(comprehension->ifs, expr_ty);
}

expr_ty _PyPegen_collect_call_seqs(Parser *p, asdl_expr_seq *a, asdl_seq *b,
                     ចំនួនគត់ lineno, ចំនួនគត់ col_offset, ចំនួនគត់ end_lineno,
                     ចំនួនគត់ end_col_offset, PyArena *arena) {
    Py_ssize_t args_len = asdl_seq_LEN(a);
    Py_ssize_t total_len = args_len;

    ប្រសិនបើ (b == NULL) {
        ត្រឡប់ _PyAST_Call(_PyPegen_dummy_name(p), a, NULL, lineno, col_offset,
                        end_lineno, end_col_offset, arena);

    }

    asdl_expr_seq *starreds = _PyPegen_seq_extract_starred_exprs(p, b);
    ប្រសិនបើ (!starreds && PyErr_Occurred()) {
        ត្រឡប់ NULL;
    }

    asdl_keyword_seq *keywords = _PyPegen_seq_delete_starred_exprs(p, b);
    ប្រសិនបើ (!keywords && PyErr_Occurred()) {
        ត្រឡប់ NULL;
    }

    ប្រសិនបើ (starreds) {
        total_len += asdl_seq_LEN(starreds);
    }

    asdl_expr_seq *args = _Py_asdl_expr_seq_new(total_len, arena);
    ប្រសិនបើ (args == NULL) {
        ត្រឡប់ NULL;
    }

    Py_ssize_t i = 0;
    សម្រាប់ (i = 0; i < args_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(a, i));
    }
    សម្រាប់ (; i < total_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(starreds, i - args_len));
    }

    ត្រឡប់ _PyAST_Call(_PyPegen_dummy_name(p), args, keywords, lineno,
                       col_offset, end_lineno, end_col_offset, arena);
}

// AST Error reporting helpers

expr_ty
_PyPegen_get_invalid_target(expr_ty e, TARGETS_TYPE targets_type)
{
    ប្រសិនបើ (e == NULL) {
        ត្រឡប់ NULL;
    }

    ប្រសិនបើ (targets_type == SINGLE_TARGETS ||
        targets_type == ATTRIBUTE_OR_SUBSCRIPT_TARGETS) {
        ប្រសិនបើ (targets_type == SINGLE_TARGETS && e->kind == Name_kind) {
            ត្រឡប់ NULL;
        }
        ត្រឡប់ e->kind == Attribute_kind || e->kind == Subscript_kind ? NULL : e;
    }

#define VISIT_CONTAINER(CONTAINER, TYPE) do { \
        Py_ssize_t len = asdl_seq_LEN((CONTAINER)->v.TYPE.elts);\
        សម្រាប់ (Py_ssize_t i = 0; i < len; i++) {\
            expr_ty other = asdl_seq_GET((CONTAINER)->v.TYPE.elts, i);\
            expr_ty child = _PyPegen_get_invalid_target(other, targets_type);\
            ប្រសិនបើ (child != NULL) {\
                ត្រឡប់ child;\
            }\
        }\
    } ខណៈពេល (0)

    // We only need to visit List and Tuple nodes recursively as those
    // are the only ones that can contain valid names in targets when
    // they are parsed as expressions. Any other kind of expression
    // that is a container (like Sets or Dicts) is directly invalid and
    // we don't need to visit it recursively.

    switch (e->kind) {
        pak_sa List_kind:
            VISIT_CONTAINER(e, List);
            ត្រឡប់ NULL;
        pak_sa Tuple_kind:
            VISIT_CONTAINER(e, Tuple);
            ត្រឡប់ NULL;
        pak_sa Starred_kind:
            ប្រសិនបើ (targets_type == DEL_TARGETS) {
                ត្រឡប់ e;
            }
            ត្រឡប់ _PyPegen_get_invalid_target(e->v.Starred.value, targets_type);
        pak_sa Compare_kind:
            // This is needed, because the `a in b` in `for a in b` gets parsed
            // as a comparison, and so we need to search the left side of the comparison
            // for invalid targets.
            ប្រសិនបើ (targets_type == FOR_TARGETS) {
                cmpop_ty cmpop = (cmpop_ty) asdl_seq_GET(e->v.Compare.ops, 0);
                ប្រសិនបើ (cmpop == In) {
                    ត្រឡប់ _PyPegen_get_invalid_target(e->v.Compare.left, targets_type);
                }
                ត្រឡប់ NULL;
            }
            ត្រឡប់ e;
        pak_sa Name_kind:
        pak_sa Subscript_kind:
        pak_sa Attribute_kind:
            ត្រឡប់ NULL;
        default:
            ត្រឡប់ e;
    }
}

void *_PyPegen_arguments_parsing_error(Parser *p, expr_ty e) {
    ចំនួនគត់ kwarg_unpacking = 0;
    សម្រាប់ (Py_ssize_t i = 0, l = asdl_seq_LEN(e->v.Call.keywords); i < l; i++) {
        keyword_ty keyword = asdl_seq_GET(e->v.Call.keywords, i);
        ប្រសិនបើ (!keyword->arg) {
            kwarg_unpacking = 1;
        }
    }

    const var_na *msg = NULL;
    ប្រសិនបើ (kwarg_unpacking) {
        msg = "positional argument follows keyword argument unpacking";
    } បើមិនដូច្នេះ {
        msg = "positional argument follows keyword argument";
    }

    ត្រឡប់ RAISE_SYNTAX_ERROR(msg);
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
    ប្រសិនបើ (len <= 1) {
        ត្រឡប់ NULL;
    }

    comprehension_ty last_comprehension = PyPegen_last_item(comprehensions, comprehension_ty);

    ត្រឡប់ RAISE_SYNTAX_ERROR_KNOWN_RANGE(
        (expr_ty) asdl_seq_GET(args->v.Call.args, len - 1),
        _PyPegen_get_last_comprehension_item(last_comprehension),
        "Generator expression must be parenthesized"
    );
}

// Fstring stuff

static expr_ty
_PyPegen_decode_fstring_part(Parser* p, ចំនួនគត់ is_raw, expr_ty constant, Token* token) {
    assert(PyUnicode_CheckExact(constant->v.Constant.value));

    const var_na* bstr = PyUnicode_AsUTF8(constant->v.Constant.value);
    ប្រសិនបើ (bstr == NULL) {
        ត្រឡប់ NULL;
    }

    size_t len;
    ប្រសិនបើ (strcmp(bstr, "{{") == 0 || strcmp(bstr, "}}") == 0) {
        len = 1;
    } បើមិនដូច្នេះ {
        len = strlen(bstr);
    }

    is_raw = is_raw || strchr(bstr, '\\') == NULL;
    PyObject *str = _PyPegen_decode_string(p, is_raw, bstr, len, token);
    ប្រសិនបើ (str == NULL) {
        _Pypegen_raise_decode_error(p);
        ត្រឡប់ NULL;
    }
    ប្រសិនបើ (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ត្រឡប់ NULL;
    }
    ត្រឡប់ _PyAST_Constant(str, NULL, constant->lineno, constant->col_offset,
                           constant->end_lineno, constant->end_col_offset,
                           p->arena);
}

static asdl_expr_seq *
_get_resized_exprs(Parser *p, Token *a, asdl_expr_seq *raw_expressions,
                   Token *b, ចំនួនគត់ is_tstring)
{
    Py_ssize_t n_items = asdl_seq_LEN(raw_expressions);
    Py_ssize_t total_items = n_items;
    សម្រាប់ (Py_ssize_t i = 0; i < n_items; i++) {
        expr_ty item = asdl_seq_GET(raw_expressions, i);
        ប្រសិនបើ (item->kind == JoinedStr_kind) {
            total_items += asdl_seq_LEN(item->v.JoinedStr.values) - 1;
        }
    }

    const var_na* quote_str = PyBytes_AsString(a->bytes);
    ប្រសិនបើ (quote_str == NULL) {
        ត្រឡប់ NULL;
    }
    ចំនួនគត់ is_raw = strpbrk(quote_str, "rR") != NULL;

    asdl_expr_seq *seq = _Py_asdl_expr_seq_new(total_items, p->arena);
    ប្រសិនបើ (seq == NULL) {
        ត្រឡប់ NULL;
    }

    Py_ssize_t index = 0;
    សម្រាប់ (Py_ssize_t i = 0; i < n_items; i++) {
        expr_ty item = asdl_seq_GET(raw_expressions, i);

        /* Debug expressions arrive as JoinedStr(text, value); flatten them
           into the surrounding string. */
        ប្រសិនបើ (item->kind == JoinedStr_kind) {
            asdl_expr_seq *values = item->v.JoinedStr.values;
            ប្រសិនបើ (asdl_seq_LEN(values) != 2) {
                PyErr_Format(PyExc_SystemError,
                             is_tstring
                             ? "unexpected TemplateStr node without debug data in t-string at line %d"
                             : "unexpected JoinedStr node without debug data in f-string at line %d",
                             item->lineno);
                ត្រឡប់ NULL;
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

        ប្រសិនបើ (item->kind == Constant_kind) {
            item = _PyPegen_decode_fstring_part(p, is_raw, item, b);
            ប្រសិនបើ (item == NULL) {
                ត្រឡប់ NULL;
            }

            /* Tokenizer emits string parts even when the underlying string
            might become an empty value (e.g. FSTRING_MIDDLE with the value \\n)
            so we need to check for them and simplify it here. */
            ប្រសិនបើ (PyUnicode_CheckExact(item->v.Constant.value)
                && PyUnicode_GET_LENGTH(item->v.Constant.value) == 0) {
                continue;
            }
        }
        asdl_seq_SET(seq, index++, item);
    }

    asdl_expr_seq *resized_exprs;
    ប្រសិនបើ (index != total_items) {
        resized_exprs = _Py_asdl_expr_seq_new(index, p->arena);
        ប្រសិនបើ (resized_exprs == NULL) {
            ត្រឡប់ NULL;
        }
        សម្រាប់ (Py_ssize_t i = 0; i < index; i++) {
            asdl_seq_SET(resized_exprs, i, asdl_seq_GET(seq, i));
        }
    }
    បើមិនដូច្នេះ {
        resized_exprs = seq;
    }
    ត្រឡប់ resized_exprs;
}

expr_ty
_PyPegen_template_str(Parser *p, Token *a, asdl_expr_seq *raw_expressions, Token *b) {

    asdl_expr_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 1);
    ប្រសិនបើ (resized_exprs == NULL) {
        ត្រឡប់ NULL;
    }
    ត្រឡប់ _PyAST_TemplateStr(resized_exprs, a->lineno, a->col_offset,
                              b->end_lineno, b->end_col_offset,
                              p->arena);
}

expr_ty
_PyPegen_joined_str(Parser *p, Token* a, asdl_expr_seq* raw_expressions, Token*b) {

    asdl_expr_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 0);
    ប្រសិនបើ (resized_exprs == NULL) {
        ត្រឡប់ NULL;
    }
    ត្រឡប់ _PyAST_JoinedStr(resized_exprs, a->lineno, a->col_offset,
                            b->end_lineno, b->end_col_offset,
                            p->arena);
}

expr_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok) {
    ប្រសិនបើ (tok->bytes == NULL) {
        ត្រឡប់ _PyPegen_raise_incomplete_input(p);
    }
    Py_ssize_t bsize = PyBytes_GET_SIZE(tok->bytes);
    const var_na *bstr = PyBytes_AS_STRING(tok->bytes);

    ចំនួនគត់ is_raw = tok->is_raw;

    PyObject* str = _PyPegen_decode_string(p, is_raw, bstr, bsize, tok);
    ប្រសិនបើ (str == NULL) {
        ត្រឡប់ NULL;
    }
    ប្រសិនបើ (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ត្រឡប់ NULL;
    }
    ត្រឡប់ _PyAST_Constant(str, NULL, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

expr_ty _PyPegen_constant_from_token(Parser* p, Token* tok) {
    ប្រសិនបើ (tok->bytes == NULL) {
        ត្រឡប់ _PyPegen_raise_incomplete_input(p);
    }
    const var_na *bstr = PyBytes_AS_STRING(tok->bytes);
    PyObject* str = PyUnicode_FromString(bstr);
    ប្រសិនបើ (str == NULL) {
        ត្រឡប់ NULL;
    }
    ប្រសិនបើ (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ត្រឡប់ NULL;
    }
    ត្រឡប់ _PyAST_Constant(str, NULL, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

expr_ty _PyPegen_constant_from_string(Parser* p, Token* tok) {
    ប្រសិនបើ (tok->bytes == NULL) {
        ត្រឡប់ _PyPegen_raise_incomplete_input(p);
    }
    const var_na *the_str = PyBytes_AS_STRING(tok->bytes);
    PyObject *s = _PyPegen_parse_string(p, tok);
    ប្រសិនបើ (s == NULL) {
        _Pypegen_raise_decode_error(p);
        ត្រឡប់ NULL;
    }
    ប្រសិនបើ (_PyArena_AddPyObject(p->arena, s) < 0) {
        Py_DECREF(s);
        ត្រឡប់ NULL;
    }
    PyObject *kind = NULL;
    ប្រសិនបើ (the_str && the_str[0] == 'u') {
        kind = _PyPegen_new_identifier(p, "u");
        ប្រសិនបើ (kind == NULL) {
            ត្រឡប់ NULL;
        }
    }
    ត្រឡប់ _PyAST_Constant(s, kind, tok->lineno, tok->col_offset, tok->end_lineno, tok->end_col_offset, p->arena);
}

static ចំនួនគត់
_get_interpolation_conversion(Parser *p, Token *debug, ResultTokenWithMetadata *conversion,
                              ResultTokenWithMetadata *format)
{
    ប្រសិនបើ (conversion != NULL) {
        expr_ty conversion_expr = (expr_ty) conversion->result;
        assert(conversion_expr->kind == Name_kind);
        Py_UCS4 first = PyUnicode_READ_CHAR(conversion_expr->v.Name.id, 0);
        ត្រឡប់ Py_SAFE_DOWNCAST(first, Py_UCS4, ចំនួនគត់);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (debug && !format) {
        /* If no conversion is specified, use !r for debug expressions */
        ត្រឡប់ (ចំនួនគត់)'r';
    }
    ត្រឡប់ -1;
}

static PyObject *
_strip_interpolation_debug_expr(PyObject *exprstr)
{
    Py_ssize_t len = PyUnicode_GET_LENGTH(exprstr);

    /* Discard whitespace and explicit line continuations after the debug "="
       but preserve whitespace before it. */
    ខណៈពេល (len > 0) {
        ចំនួនគត់ has_newline = 0;
        ខណៈពេល (len > 0) {
            Py_UCS4 c = PyUnicode_READ_CHAR(exprstr, len - 1);
            ប្រសិនបើ (!_PyUnicode_IsWhitespace(c)) {
                bha_nga;
            }
            ប្រសិនបើ (c == '\r' || c == '\n') {
                has_newline = 1;
            }
            len--;
        }
        ប្រសិនបើ (!has_newline || len == 0 ||
            PyUnicode_READ_CHAR(exprstr, len - 1) != '\\')
        {
            bha_nga;
        }
        len--;
    }

    /* Preserve unexpected metadata instead of dropping source text. */
    ប្រសិនបើ (len == 0 || PyUnicode_READ_CHAR(exprstr, len - 1) != '=') {
        ត្រឡប់ Py_NewRef(exprstr);
    }

    ត្រឡប់ PyUnicode_Substring(exprstr, 0, len - 1);
}

expr_ty _PyPegen_interpolation(Parser *p, expr_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, ចំនួនគត់ lineno, ចំនួនគត់ col_offset,
                                 ចំនួនគត់ end_lineno, ចំនួនគត់ end_col_offset, PyArena *arena) {

    ចំនួនគត់ conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    /* Find the non whitespace token after the "=" */
    ចំនួនគត់ debug_end_line, debug_end_offset;
    PyObject *debug_metadata;
    constant exprstr;

    ប្រសិនបើ (conversion) {
        debug_end_line = ((expr_ty) conversion->result)->lineno;
        debug_end_offset = ((expr_ty) conversion->result)->col_offset;
        debug_metadata = exprstr = conversion->metadata;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (format) {
        debug_end_line = ((expr_ty) format->result)->lineno;
        debug_end_offset = ((expr_ty) format->result)->col_offset + 1;
        debug_metadata = exprstr = format->metadata;
    }
    បើមិនដូច្នេះ {
        debug_end_line = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = exprstr = closing_brace->metadata;
    }

    assert(exprstr != NULL);
    PyObject *final_exprstr = debug
        ? _strip_interpolation_debug_expr(exprstr)
        : Py_NewRef(exprstr);
    ប្រសិនបើ (!final_exprstr || _PyArena_AddPyObject(arena, final_exprstr) < 0) {
        Py_XDECREF(final_exprstr);
        ត្រឡប់ NULL;
    }

    expr_ty interpolation = _PyAST_Interpolation(
        expression, final_exprstr, conversion_val, format ? (expr_ty) format->result : NULL,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    ប្រសិនបើ (!interpolation || !debug) {
        ត្រឡប់ interpolation;
    }

    expr_ty debug_text = _PyAST_Constant(debug_metadata, NULL, lineno, col_offset + 1, debug_end_line,
                                            debug_end_offset - 1, p->arena);
    ប្រសិនបើ (!debug_text) {
        ត្រឡប់ NULL;
    }

    asdl_expr_seq *values = _Py_asdl_expr_seq_new(2, arena);
    ប្រសិនបើ (!values) {
        ត្រឡប់ NULL;
    }
    asdl_seq_SET(values, 0, debug_text);
    asdl_seq_SET(values, 1, interpolation);
    ត្រឡប់ _PyAST_JoinedStr(values, lineno, col_offset, debug_end_line, debug_end_offset, p->arena);
}

expr_ty _PyPegen_formatted_value(Parser *p, expr_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, ចំនួនគត់ lineno, ចំនួនគត់ col_offset,
                                 ចំនួនគត់ end_lineno, ចំនួនគត់ end_col_offset, PyArena *arena) {
    ចំនួនគត់ conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    expr_ty formatted_value = _PyAST_FormattedValue(
        expression, conversion_val, format ? (expr_ty) format->result : NULL,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    ប្រសិនបើ (!formatted_value || !debug) {
        ត្រឡប់ formatted_value;
    }

    /* Find the non whitespace token after the "=" */
    ចំនួនគត់ debug_end_line, debug_end_offset;
    PyObject *debug_metadata;

    ប្រសិនបើ (conversion) {
        debug_end_line = ((expr_ty) conversion->result)->lineno;
        debug_end_offset = ((expr_ty) conversion->result)->col_offset;
        debug_metadata = conversion->metadata;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (format) {
        debug_end_line = ((expr_ty) format->result)->lineno;
        debug_end_offset = ((expr_ty) format->result)->col_offset + 1;
        debug_metadata = format->metadata;
    }
    បើមិនដូច្នេះ {
        debug_end_line = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = closing_brace->metadata;
    }
    expr_ty debug_text = _PyAST_Constant(debug_metadata, NULL, lineno, col_offset + 1, debug_end_line,
                                            debug_end_offset - 1, p->arena);
    ប្រសិនបើ (!debug_text) {
        ត្រឡប់ NULL;
    }

    asdl_expr_seq *values = _Py_asdl_expr_seq_new(2, arena);
    ប្រសិនបើ (!values) {
        ត្រឡប់ NULL;
    }
    asdl_seq_SET(values, 0, debug_text);
    asdl_seq_SET(values, 1, formatted_value);
    ត្រឡប់ _PyAST_JoinedStr(values, lineno, col_offset, debug_end_line, debug_end_offset, p->arena);
}

static expr_ty
_build_concatenated_bytes(Parser *p, asdl_expr_seq *strings, ចំនួនគត់ lineno,
                        ចំនួនគត់ col_offset, ចំនួនគត់ end_lineno, ចំនួនគត់ end_col_offset,
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
    សម្រាប់ (Py_ssize_t i = 0; i < len; i++) {
        expr_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.value;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        ប្រសិនបើ (part > PY_SSIZE_T_MAX - total) {
            PyErr_NoMemory();
            ត្រឡប់ NULL;
        }
        total += part;
    }

    PyBytesWriter *writer = PyBytesWriter_Create(total);
    ប្រសិនបើ (writer == NULL) {
        ត្រឡប់ NULL;
    }
    var_na *out = PyBytesWriter_GetData(writer);

    សម្រាប់ (Py_ssize_t i = 0; i < len; i++) {
        expr_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.value;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        ប្រសិនបើ (part > 0) {
            memcpy(out, PyBytes_AS_STRING(bytes), part);
            out += part;
        }
    }

    PyObject *res = PyBytesWriter_Finish(writer);
    ប្រសិនបើ (res == NULL) {
        ត្រឡប់ NULL;
    }
    ប្រសិនបើ (_PyArena_AddPyObject(arena, res) < 0) {
        Py_DECREF(res);
        ត្រឡប់ NULL;
    }
    ត្រឡប់ _PyAST_Constant(res, kind, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

static expr_ty
_build_concatenated_unicode(Parser *p, asdl_expr_seq *strings, ចំនួនគត់ lineno,
                        ចំនួនគត់ col_offset, ចំនួនគត់ end_lineno, ចំនួនគត់ end_col_offset,
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
    ប្រសិនបើ (writer == NULL) {
        ត្រឡប់ NULL;
    }

    សម្រាប់ (Py_ssize_t i = 0; i < len; i++) {
        expr_ty current_elem = asdl_seq_GET(strings, i);
        assert(current_elem->kind == Constant_kind);

        ប្រសិនបើ (PyUnicodeWriter_WriteStr(writer,
                                     current_elem->v.Constant.value)) {
            PyUnicodeWriter_Discard(writer);
            ត្រឡប់ NULL;
        }
    }

    PyObject *final = PyUnicodeWriter_Finish(writer);
    ប្រសិនបើ (final == NULL) {
        ត្រឡប់ NULL;
    }
    ប្រសិនបើ (_PyArena_AddPyObject(p->arena, final) < 0) {
        Py_DECREF(final);
        ត្រឡប់ NULL;
    }
    ត្រឡប់ _PyAST_Constant(final, kind, lineno, col_offset,
                           end_lineno, end_col_offset, arena);
}

static asdl_expr_seq *
_build_concatenated_str(Parser *p, asdl_expr_seq *strings,
                               ចំនួនគត់ lineno, ចំនួនគត់ col_offset, ចំនួនគត់ end_lineno,
                               ចំនួនគត់ end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    assert(len > 0);

    Py_ssize_t n_flattened_elements = 0;
    សម្រាប់ (Py_ssize_t i = 0; i < len; i++) {
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
    ប្រសិនបើ (flattened == NULL) {
        ត្រឡប់ NULL;
    }

    /* build flattened list */
    Py_ssize_t current_pos = 0;
    សម្រាប់ (Py_ssize_t i = 0; i < len; i++) {
        expr_ty elem = asdl_seq_GET(strings, i);
        switch(elem->kind) {
            pak_sa JoinedStr_kind:
                សម្រាប់ (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.JoinedStr.values); j++) {
                    expr_ty subvalue = asdl_seq_GET(elem->v.JoinedStr.values, j);
                    ប្រសិនបើ (subvalue == NULL) {
                        ត្រឡប់ NULL;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                bha_nga;
            pak_sa TemplateStr_kind:
                សម្រាប់ (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.TemplateStr.values); j++) {
                    expr_ty subvalue = asdl_seq_GET(elem->v.TemplateStr.values, j);
                    ប្រសិនបើ (subvalue == NULL) {
                        ត្រឡប់ NULL;
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
    ចំនួនគត់ prev_is_constant = 0;
    សម្រាប់ (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        expr_ty elem = asdl_seq_GET(flattened, i);

        /* The concatenation of a FormattedValue and an empty Constant should
           lead to the FormattedValue itself. Thus, we will not take any empty
           constants into account, just as in `_PyPegen_joined_str` */
        ប្រសិនបើ (elem->kind == Constant_kind &&
            PyUnicode_CheckExact(elem->v.Constant.value) &&
            PyUnicode_GET_LENGTH(elem->v.Constant.value) == 0)
            continue;

        ប្រសិនបើ (!prev_is_constant || elem->kind != Constant_kind) {
            n_elements++;
        }
        prev_is_constant = elem->kind == Constant_kind;
    }

    asdl_expr_seq* values = _Py_asdl_expr_seq_new(n_elements, p->arena);
    ប្រសិនបើ (values == NULL) {
        ត្រឡប់ NULL;
    }

    /* build folded list */
    current_pos = 0;
    សម្រាប់ (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        expr_ty elem = asdl_seq_GET(flattened, i);

        /* if the current elem and the following are constants,
           fold them and all consequent constants */
        ប្រសិនបើ (elem->kind == Constant_kind) {
            ប្រសិនបើ (i + 1 < n_flattened_elements &&
                asdl_seq_GET(flattened, i + 1)->kind == Constant_kind) {
                expr_ty first_elem = elem;

                /* When a string is getting concatenated, the kind of the string
                   is determined by the first string in the concatenation
                   sequence.

                   u"abc" "def" -> u"abcdef"
                   "abc" u"abc" ->  "abcabc" */
                PyObject *kind = elem->v.Constant.kind;

                PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
                ប្រសិនបើ (writer == NULL) {
                    ត្រឡប់ NULL;
                }
                expr_ty last_elem = elem;
                Py_ssize_t j;
                សម្រាប់ (j = i; j < n_flattened_elements; j++) {
                    expr_ty current_elem = asdl_seq_GET(flattened, j);
                    ប្រសិនបើ (current_elem->kind == Constant_kind) {
                        ប្រសិនបើ (PyUnicodeWriter_WriteStr(writer,
                                                     current_elem->v.Constant.value)) {
                            PyUnicodeWriter_Discard(writer);
                            ត្រឡប់ NULL;
                        }
                        last_elem = current_elem;
                    } បើមិនដូច្នេះ {
                        bha_nga;
                    }
                }
                i = j - 1;

                PyObject *concat_str = PyUnicodeWriter_Finish(writer);
                ប្រសិនបើ (concat_str == NULL) {
                    ត្រឡប់ NULL;
                }
                ប្រសិនបើ (_PyArena_AddPyObject(p->arena, concat_str) < 0) {
                    Py_DECREF(concat_str);
                    ត្រឡប់ NULL;
                }
                elem = _PyAST_Constant(concat_str, kind, first_elem->lineno,
                                       first_elem->col_offset,
                                       last_elem->end_lineno,
                                       last_elem->end_col_offset, p->arena);
                ប្រសិនបើ (elem == NULL) {
                    ត្រឡប់ NULL;
                }
            }

            /* Drop all empty contanst strings */
            ប្រសិនបើ (PyUnicode_CheckExact(elem->v.Constant.value) &&
                PyUnicode_GET_LENGTH(elem->v.Constant.value) == 0) {
                continue;
            }
        }

        asdl_seq_SET(values, current_pos++, elem);
    }

    assert(current_pos == n_elements);
    ត្រឡប់ values;
}

static expr_ty
_build_concatenated_joined_str(Parser *p, asdl_expr_seq *strings,
                               ចំនួនគត់ lineno, ចំនួនគត់ col_offset, ចំនួនគត់ end_lineno,
                               ចំនួនគត់ end_col_offset, PyArena *arena)
{
    asdl_expr_seq *values = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    ប្រសិនបើ (!values) {
        ត្រឡប់ NULL;
    }
    ត្រឡប់ _PyAST_JoinedStr(values, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

expr_ty
_PyPegen_concatenate_tstrings(Parser *p, asdl_expr_seq *strings,
                               ចំនួនគត់ lineno, ចំនួនគត់ col_offset, ចំនួនគត់ end_lineno,
                               ចំនួនគត់ end_col_offset, PyArena *arena)
{
    asdl_expr_seq *values = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    ប្រសិនបើ (!values) {
        ត្រឡប់ NULL;
    }
    ត្រឡប់ _PyAST_TemplateStr(values, lineno, col_offset, end_lineno,
        end_col_offset, arena);
}

expr_ty
_PyPegen_concatenate_strings(Parser *p, asdl_expr_seq *strings,
                             ចំនួនគត់ lineno, ចំនួនគត់ col_offset, ចំនួនគត់ end_lineno,
                             ចំនួនគត់ end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    assert(len > 0);

    ចំនួនគត់ f_string_found = 0;
    ចំនួនគត់ unicode_string_found = 0;
    ចំនួនគត់ bytes_found = 0;

    Py_ssize_t i = 0;
    សម្រាប់ (i = 0; i < len; i++) {
        expr_ty elem = asdl_seq_GET(strings, i);
        switch(elem->kind) {
            pak_sa Constant_kind:
                ប្រសិនបើ (PyBytes_CheckExact(elem->v.Constant.value)) {
                    bytes_found = 1;
                } បើមិនដូច្នេះ {
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
    ប្រសិនបើ ((unicode_string_found || f_string_found) && bytes_found) {
        RAISE_SYNTAX_ERROR("cannot mix bytes and nonbytes literals");
        ត្រឡប់ NULL;
    }

    // If it's only bytes or only unicode string, do a simple concat
    ប្រសិនបើ (!f_string_found) {
        ប្រសិនបើ (len == 1) {
            ត្រឡប់ asdl_seq_GET(strings, 0);
        }
        បើមិនដូច្នេះ ប្រសិនបើ (bytes_found) {
            ត្រឡប់ _build_concatenated_bytes(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
        បើមិនដូច្នេះ {
            ត្រឡប់ _build_concatenated_unicode(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
    }

    ត្រឡប់ _build_concatenated_joined_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
}

static ចំនួនគត់
_warn_relative_import_of_lazy(Parser *p, asdl_seq *dots, expr_ty module)
{
    // Warn about `from . lazy import x`: the whitespace between the dots and
    // the module name is insignificant, so this is parsed exactly like
    // `from .lazy import x` (an import of the relative module "lazy"), but it
    // is most likely a transposition of `lazy from . import x` (PEP 810).
    ប្រសិនបើ (p->call_invalid_rules) {
        ត្រឡប់ 0;
    }

    // Only fire if there is whitespace between the last dot and the name,
    // i.e. not for the common `from .lazy import x` spelling.
    Token *last_dot = asdl_seq_GET_UNTYPED(dots, asdl_seq_LEN(dots) - 1);
    ប្រសិនបើ (
        last_dot->end_lineno == module->lineno
        && last_dot->end_col_offset == module->col_offset
    ) {
        ត្រឡប់ 0;
    }

    ចំនួនគត់ count = _PyPegen_seq_count_dots(dots);
    var_na *buf = PyMem_RawMalloc(count + 1);
    ប្រសិនបើ (buf == NULL) {
        PyErr_NoMemory();
        ត្រឡប់ -1;
    }
    memset(buf, '.', count);
    buf[count] = '\0';

    PyObject *msg = PyUnicode_FromFormat(
        "'from %s lazy import' is the same as 'from %slazy import'; "
        "did you mean 'lazy from %s import'?",
        buf, buf, buf);
    PyMem_RawFree(buf);
    ប្រសិនបើ (msg == NULL) {
        ត្រឡប់ -1;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    ចំនួនគត់ res = _PyErr_EmitSyntaxWarning(msg,
                                       info.filename,
                                       module->lineno,
                                       module->col_offset + 1,
                                       module->end_lineno,
                                       module->end_col_offset + 1,
                                       info.module);
    Py_DECREF(msg);
    ត្រឡប់ res;
}

stmt_ty
_PyPegen_checked_from_import(Parser *p, asdl_seq *dots, expr_ty module_name,
                             asdl_alias_seq *names, expr_ty lazy_token, ចំនួនគត់ lineno,
                             ចំនួនគត់ col_offset, ចំនួនគត់ end_lineno, ចំនួនគត់ end_col_offset,
                             PyArena *arena)
{
    identifier module = module_name->v.Name.id;
    ចំនួនគត់ level = _PyPegen_seq_count_dots(dots);
    ប្រសិនបើ (level == 0 && PyUnicode_CompareWithASCIIString(module, "__future__") == 0) {
        ប្រសិនបើ (lazy_token) {
            RAISE_SYNTAX_ERROR_KNOWN_LOCATION(lazy_token,
                "lazy from __future__ import is not allowed");
            ត្រឡប់ NULL;
        }
        សម្រាប់ (Py_ssize_t i = 0; i < asdl_seq_LEN(names); i++) {
            alias_ty alias = asdl_seq_GET(names, i);
            ប្រសិនបើ (PyUnicode_CompareWithASCIIString(alias->name, "barry_as_FLUFL") == 0) {
                p->flags |= PyPARSE_BARRY_AS_BDFL;
                p->tok->barry_as_bdfl = 1;
            }
        }
    }
    បើមិនដូច្នេះ ប្រសិនបើ (
        level > 0
        && lazy_token == NULL
        && PyUnicode_CompareWithASCIIString(module, "lazy") == 0
    ) {
        ប្រសិនបើ (_warn_relative_import_of_lazy(p, dots, module_name) < 0) {
            ត្រឡប់ NULL;
        }
    }
    ត្រឡប់ _PyAST_ImportFrom(module, names, level, lazy_token ? 1 : 0, lineno,
                             col_offset, end_lineno, end_col_offset, arena);
}

asdl_stmt_seq*
_PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts) {
    ប្រសិនបើ (!p->call_invalid_rules) {
        ត្រឡប់ stmts;
    }
    Py_ssize_t len = asdl_seq_LEN(stmts);
    ប្រសិនបើ (len == 0) {
        ត្រឡប់ stmts;
    }
    stmt_ty last_stmt = asdl_seq_GET(stmts, len - 1);
    ប្រសិនបើ (p->last_stmt_location.lineno > last_stmt->lineno) {
        ត្រឡប់ stmts;
    }
    p->last_stmt_location.lineno = last_stmt->lineno;
    p->last_stmt_location.col_offset = last_stmt->col_offset;
    p->last_stmt_location.end_lineno = last_stmt->end_lineno;
    p->last_stmt_location.end_col_offset = last_stmt->end_col_offset;
    ត្រឡប់ stmts;
}
