#ضمن <Python.h>
#ضمن "pycore_pystate.h"         // _PyInterpreterState_GET()
#ضمن "pycore_runtime.h"         // _PyRuntime
#ضمن "pycore_unicodeobject.h"   // _PyUnicode_InternImmortal()
#ضمن "pycore_pyerrors.h"        // _PyErr_EmitSyntaxWarning()

#ضمن "pegen.h"
#ضمن "string_parser.h"          // _PyPegen_decode_string()
#ضمن "lexer/state.h"            // tok_state


فراغ *
_PyPegen_dummy_name(Parser *p, ...)
{
    ܦܢܐ &_PyRuntime.parser.dummy_name;
}

/* Creates a single-element asdl_seq* that contains a */
asdl_seq *
_PyPegen_singleton_seq(Parser *p, فراغ *a)
{
    أكد(a != عدم);
    asdl_seq *seq = (asdl_seq*)_Py_asdl_generic_seq_new(1, p->arena);
    ܐܢ (!seq) {
        ܦܢܐ عدم;
    }
    asdl_seq_SET_UNTYPED(seq, 0, a);
    ܦܢܐ seq;
}

/* Creates a copy of seq and prepends a to it */
asdl_seq *
_PyPegen_seq_insert_in_front(Parser *p, فراغ *a, asdl_seq *seq)
{
    أكد(a != عدم);
    ܐܢ (!seq) {
        ܦܢܐ _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    ܐܢ (!new_seq) {
        ܦܢܐ عدم;
    }

    asdl_seq_SET_UNTYPED(new_seq, 0, a);
    لكل (Py_ssize_t i = 1, l = asdl_seq_LEN(new_seq); i < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i - 1));
    }
    ܦܢܐ new_seq;
}

/* Creates a copy of seq and appends a to it */
asdl_seq *
_PyPegen_seq_append_to_end(Parser *p, asdl_seq *seq, فراغ *a)
{
    أكد(a != عدم);
    ܐܢ (!seq) {
        ܦܢܐ _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    ܐܢ (!new_seq) {
        ܦܢܐ عدم;
    }

    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(new_seq); i + 1 < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i));
    }
    asdl_seq_SET_UNTYPED(new_seq, asdl_seq_LEN(new_seq) - 1, a);
    ܦܢܐ new_seq;
}

ساكن Py_ssize_t
_get_flattened_seq_size(asdl_seq *seqs)
{
    Py_ssize_t size = 0;
    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        size += asdl_seq_LEN(inner_seq);
    }
    ܦܢܐ size;
}

/* Flattens an asdl_seq* of asdl_seq*s */
asdl_seq *
_PyPegen_seq_flatten(Parser *p, asdl_seq *seqs)
{
    Py_ssize_t flattened_seq_size = _get_flattened_seq_size(seqs);
    أكد(flattened_seq_size > 0);

    asdl_seq *flattened_seq = (asdl_seq*)_Py_asdl_generic_seq_new(flattened_seq_size, p->arena);
    ܐܢ (!flattened_seq) {
        ܦܢܐ عدم;
    }

    صحيح flattened_seq_idx = 0;
    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        لكل (Py_ssize_t j = 0, li = asdl_seq_LEN(inner_seq); j < li; j++) {
            asdl_seq_SET_UNTYPED(flattened_seq, flattened_seq_idx++, asdl_seq_GET_UNTYPED(inner_seq, j));
        }
    }
    أكد(flattened_seq_idx == flattened_seq_size);

    ܦܢܐ flattened_seq;
}

فراغ *
_PyPegen_seq_last_item(asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ܦܢܐ asdl_seq_GET_UNTYPED(seq, len - 1);
}

فراغ *
_PyPegen_seq_first_item(asdl_seq *seq)
{
    ܦܢܐ asdl_seq_GET_UNTYPED(seq, 0);
}

/* Creates a new name of the form <first_name>.<second_name> */
expr_ty
_PyPegen_join_names_with_dot(Parser *p, expr_ty first_name, expr_ty second_name)
{
    أكد(first_name != عدم && second_name != عدم);
    PyObject *uni = PyUnicode_FromFormat("%U.%U",
            first_name->v.Name.id, second_name->v.Name.id);
    ܐܢ (!uni) {
        ܦܢܐ عدم;
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &uni);
    ܐܢ (_PyArena_AddPyObject(p->arena, uni) < 0) {
        Py_DECREF(uni);
        ܦܢܐ عدم;
    }

    ܦܢܐ _PyAST_Name(uni, Load, EXTRA_EXPR(first_name, second_name));
}

/* Counts the total number of dots in seq's tokens */
صحيح
_PyPegen_seq_count_dots(asdl_seq *seq)
{
    صحيح number_of_dots = 0;
    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        Token *current_expr = asdl_seq_GET_UNTYPED(seq, i);
        بدل (current_expr->type) {
            حالة ELLIPSIS:
                number_of_dots += 3;
                اكسر;
            حالة DOT:
                number_of_dots += 1;
                اكسر;
            افتراضي:
                Py_UNREACHABLE();
        }
    }

    ܦܢܐ number_of_dots;
}

/* Creates an alias with '*' as the identifier name */
alias_ty
_PyPegen_alias_for_star(Parser *p, صحيح lineno, صحيح col_offset, صحيح end_lineno,
                        صحيح end_col_offset, PyArena *arena) {
    PyObject *str = PyUnicode_InternFromString("*");
    ܐܢ (!str) {
        ܦܢܐ عدم;
    }
    ܐܢ (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ܦܢܐ عدم;
    }
    ܦܢܐ _PyAST_alias(str, عدم, lineno, col_offset, end_lineno, end_col_offset, arena);
}

/* Creates a new asdl_seq* with the identifiers of all the names in seq */
asdl_identifier_seq *
_PyPegen_map_names_to_ids(Parser *p, asdl_expr_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    أكد(len > 0);

    asdl_identifier_seq *new_seq = _Py_asdl_identifier_seq_new(len, p->arena);
    ܐܢ (!new_seq) {
        ܦܢܐ عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        expr_ty e = asdl_seq_GET(seq, i);
        asdl_seq_SET(new_seq, i, e->v.Name.id);
    }
    ܦܢܐ new_seq;
}

/* Constructs a CmpopExprPair */
CmpopExprPair *
_PyPegen_cmpop_expr_pair(Parser *p, cmpop_ty cmpop, expr_ty expr)
{
    أكد(expr != عدم);
    CmpopExprPair *a = _PyArena_Malloc(p->arena, حجم(CmpopExprPair));
    ܐܢ (!a) {
        ܦܢܐ عدم;
    }
    a->cmpop = cmpop;
    a->expr = expr;
    ܦܢܐ a;
}

asdl_int_seq *
_PyPegen_get_cmpops(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    أكد(len > 0);

    asdl_int_seq *new_seq = _Py_asdl_int_seq_new(len, p->arena);
    ܐܢ (!new_seq) {
        ܦܢܐ عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        CmpopExprPair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->cmpop);
    }
    ܦܢܐ new_seq;
}

asdl_expr_seq *
_PyPegen_get_exprs(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    أكد(len > 0);

    asdl_expr_seq *new_seq = _Py_asdl_expr_seq_new(len, p->arena);
    ܐܢ (!new_seq) {
        ܦܢܐ عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        CmpopExprPair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->expr);
    }
    ܦܢܐ new_seq;
}

/* Creates an asdl_seq* where all the elements have been changed to have ctx as context */
ساكن asdl_expr_seq *
_set_seq_context(Parser *p, asdl_expr_seq *seq, expr_context_ty ctx)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ܐܢ (len == 0) {
        ܦܢܐ عدم;
    }

    asdl_expr_seq *new_seq = _Py_asdl_expr_seq_new(len, p->arena);
    ܐܢ (!new_seq) {
        ܦܢܐ عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        expr_ty e = asdl_seq_GET(seq, i);
        expr_ty new_e = _PyPegen_set_expr_context(p, e, ctx);
        ܐܢ (!new_e) {
            ܦܢܐ عدم;
        }
        asdl_seq_SET(new_seq, i, new_e);
    }
    ܦܢܐ new_seq;
}

ساكن expr_ty
_set_name_context(Parser *p, expr_ty e, expr_context_ty ctx)
{
    ܦܢܐ _PyAST_Name(e->v.Name.id, ctx, EXTRA_EXPR(e, e));
}

ساكن expr_ty
_set_tuple_context(Parser *p, expr_ty e, expr_context_ty ctx)
{
    asdl_expr_seq *seq = _set_seq_context(p, e->v.Tuple.elts, ctx);
    ܐܢ (!seq && PyErr_Occurred()) {
        ܦܢܐ عدم;
    }
    ܦܢܐ _PyAST_Tuple(seq, ctx, EXTRA_EXPR(e, e));
}

ساكن expr_ty
_set_list_context(Parser *p, expr_ty e, expr_context_ty ctx)
{
    asdl_expr_seq *seq = _set_seq_context(p, e->v.List.elts, ctx);
    ܐܢ (!seq && PyErr_Occurred()) {
        ܦܢܐ عدم;
    }
    ܦܢܐ _PyAST_List(seq, ctx, EXTRA_EXPR(e, e));
}

ساكن expr_ty
_set_subscript_context(Parser *p, expr_ty e, expr_context_ty ctx)
{
    ܦܢܐ _PyAST_Subscript(e->v.Subscript.value, e->v.Subscript.slice,
                            ctx, EXTRA_EXPR(e, e));
}

ساكن expr_ty
_set_attribute_context(Parser *p, expr_ty e, expr_context_ty ctx)
{
    ܦܢܐ _PyAST_Attribute(e->v.Attribute.value, e->v.Attribute.attr,
                            ctx, EXTRA_EXPR(e, e));
}

ساكن expr_ty
_set_starred_context(Parser *p, expr_ty e, expr_context_ty ctx)
{
    expr_ty inner = _PyPegen_set_expr_context(p, e->v.Starred.value, ctx);
    ܐܢ (!inner) {
        ܦܢܐ عدم;
    }
    ܦܢܐ _PyAST_Starred(inner, ctx, EXTRA_EXPR(e, e));
}

/* Creates an `expr_ty` equivalent to `expr` but with `ctx` as context */
expr_ty
_PyPegen_set_expr_context(Parser *p, expr_ty expr, expr_context_ty ctx)
{
    أكد(expr != عدم);

    expr_ty new = عدم;
    بدل (expr->kind) {
        حالة Name_kind:
            new = _set_name_context(p, expr, ctx);
            اكسر;
        حالة Tuple_kind:
            new = _set_tuple_context(p, expr, ctx);
            اكسر;
        حالة List_kind:
            new = _set_list_context(p, expr, ctx);
            اكسر;
        حالة Subscript_kind:
            new = _set_subscript_context(p, expr, ctx);
            اكسر;
        حالة Attribute_kind:
            new = _set_attribute_context(p, expr, ctx);
            اكسر;
        حالة Starred_kind:
            new = _set_starred_context(p, expr, ctx);
            اكسر;
        افتراضي:
            new = expr;
    }
    ܦܢܐ new;
}

/* Invalid targets return NULL without raising, allowing parsing to backtrack.
   Copy valid targets to preserve the memoized expression's context. */
expr_ty
_PyPegen_make_target(Parser *p, expr_ty expr, TARGETS_TYPE targets_type)
{
    أكد(expr != عدم);
    أكد(targets_type != FOR_TARGETS);
    ܐܢ (_PyPegen_get_invalid_target(expr, targets_type) != عدم) {
        ܦܢܐ عدم;
    }
    ܦܢܐ _PyPegen_set_expr_context(
        p, expr, targets_type == DEL_TARGETS ? Del : Store);
}

/* Constructs a KeyValuePair that is used when parsing a dict's key value pairs */
KeyValuePair *
_PyPegen_key_value_pair(Parser *p, expr_ty key, expr_ty value)
{
    KeyValuePair *a = _PyArena_Malloc(p->arena, حجم(KeyValuePair));
    ܐܢ (!a) {
        ܦܢܐ عدم;
    }
    a->key = key;
    a->value = value;
    ܦܢܐ a;
}

/* Extracts all keys from an asdl_seq* of KeyValuePair*'s */
asdl_expr_seq *
_PyPegen_get_keys(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_expr_seq *new_seq = _Py_asdl_expr_seq_new(len, p->arena);
    ܐܢ (!new_seq) {
        ܦܢܐ عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        KeyValuePair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->key);
    }
    ܦܢܐ new_seq;
}

/* Extracts all values from an asdl_seq* of KeyValuePair*'s */
asdl_expr_seq *
_PyPegen_get_values(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_expr_seq *new_seq = _Py_asdl_expr_seq_new(len, p->arena);
    ܐܢ (!new_seq) {
        ܦܢܐ عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        KeyValuePair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->value);
    }
    ܦܢܐ new_seq;
}

/* Constructs a KeyPatternPair that is used when parsing mapping & class patterns */
KeyPatternPair *
_PyPegen_key_pattern_pair(Parser *p, expr_ty key, pattern_ty pattern)
{
    KeyPatternPair *a = _PyArena_Malloc(p->arena, حجم(KeyPatternPair));
    ܐܢ (!a) {
        ܦܢܐ عدم;
    }
    a->key = key;
    a->pattern = pattern;
    ܦܢܐ a;
}

/* Extracts all keys from an asdl_seq* of KeyPatternPair*'s */
asdl_expr_seq *
_PyPegen_get_pattern_keys(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_expr_seq *new_seq = _Py_asdl_expr_seq_new(len, p->arena);
    ܐܢ (!new_seq) {
        ܦܢܐ عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        KeyPatternPair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->key);
    }
    ܦܢܐ new_seq;
}

/* Extracts all patterns from an asdl_seq* of KeyPatternPair*'s */
asdl_pattern_seq *
_PyPegen_get_patterns(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_pattern_seq *new_seq = _Py_asdl_pattern_seq_new(len, p->arena);
    ܐܢ (!new_seq) {
        ܦܢܐ عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        KeyPatternPair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->pattern);
    }
    ܦܢܐ new_seq;
}

/* Constructs a NameDefaultPair */
NameDefaultPair *
_PyPegen_name_default_pair(Parser *p, arg_ty arg, expr_ty value, Token *tc)
{
    NameDefaultPair *a = _PyArena_Malloc(p->arena, حجم(NameDefaultPair));
    ܐܢ (!a) {
        ܦܢܐ عدم;
    }
    a->arg = _PyPegen_add_type_comment_to_arg(p, arg, tc);
    ܐܢ (!a->arg) {
        ܦܢܐ عدم;
    }
    a->value = value;
    ܦܢܐ a;
}

/* Constructs a SlashWithDefault */
SlashWithDefault *
_PyPegen_slash_with_default(Parser *p, asdl_arg_seq *plain_names, asdl_seq *names_with_defaults)
{
    SlashWithDefault *a = _PyArena_Malloc(p->arena, حجم(SlashWithDefault));
    ܐܢ (!a) {
        ܦܢܐ عدم;
    }
    a->plain_names = plain_names;
    a->names_with_defaults = names_with_defaults;
    ܦܢܐ a;
}

/* Constructs a StarEtc */
StarEtc *
_PyPegen_star_etc(Parser *p, arg_ty vararg, asdl_seq *kwonlyargs, arg_ty kwarg)
{
    StarEtc *a = _PyArena_Malloc(p->arena, حجم(StarEtc));
    ܐܢ (!a) {
        ܦܢܐ عدم;
    }
    a->vararg = vararg;
    a->kwonlyargs = kwonlyargs;
    a->kwarg = kwarg;
    ܦܢܐ a;
}

asdl_seq *
_PyPegen_join_sequences(Parser *p, asdl_seq *a, asdl_seq *b)
{
    Py_ssize_t first_len = asdl_seq_LEN(a);
    Py_ssize_t second_len = asdl_seq_LEN(b);
    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(first_len + second_len, p->arena);
    ܐܢ (!new_seq) {
        ܦܢܐ عدم;
    }

    صحيح k = 0;
    لكل (Py_ssize_t i = 0; i < first_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(a, i));
    }
    لكل (Py_ssize_t i = 0; i < second_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(b, i));
    }

    ܦܢܐ new_seq;
}

ساكن asdl_arg_seq*
_get_names(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_arg_seq *seq = _Py_asdl_arg_seq_new(len, p->arena);
    ܐܢ (!seq) {
        ܦܢܐ عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->arg);
    }
    ܦܢܐ seq;
}

ساكن asdl_expr_seq *
_get_defaults(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_expr_seq *seq = _Py_asdl_expr_seq_new(len, p->arena);
    ܐܢ (!seq) {
        ܦܢܐ عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->value);
    }
    ܦܢܐ seq;
}

ساكن صحيح
_make_posonlyargs(Parser *p,
                  asdl_arg_seq *slash_without_default,
                  SlashWithDefault *slash_with_default,
                  asdl_arg_seq **posonlyargs) {
    ܐܢ (slash_without_default != عدم) {
        *posonlyargs = slash_without_default;
    }
    ܐܚܪܢܐ ܐܢ (slash_with_default != عدم) {
        asdl_arg_seq *slash_with_default_names =
                _get_names(p, slash_with_default->names_with_defaults);
        ܐܢ (!slash_with_default_names) {
            ܦܢܐ -1;
        }
        *posonlyargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default->plain_names,
                (asdl_seq*)slash_with_default_names);
    }
    ܐܚܪܢܐ {
        *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }
    ܦܢܐ *posonlyargs == عدم ? -1 : 0;
}

ساكن صحيح
_make_posargs(Parser *p,
              asdl_arg_seq *plain_names,
              asdl_seq *names_with_default,
              asdl_arg_seq **posargs) {

    ܐܢ (names_with_default != عدم) {
        ܐܢ (plain_names != عدم) {
            asdl_arg_seq *names_with_default_names = _get_names(p, names_with_default);
            ܐܢ (!names_with_default_names) {
                ܦܢܐ -1;
            }
            *posargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                    p,(asdl_seq*)plain_names, (asdl_seq*)names_with_default_names);
        }
        ܐܚܪܢܐ {
            *posargs = _get_names(p, names_with_default);
        }
    }
    ܐܚܪܢܐ {
        ܐܢ (plain_names != عدم) {
            // With the current grammar, we never get here.
            // If that has changed, remove the assert, and test thoroughly.
            أكد(0);
            *posargs = plain_names;
        }
        ܐܚܪܢܐ {
            *posargs = _Py_asdl_arg_seq_new(0, p->arena);
        }
    }
    ܦܢܐ *posargs == عدم ? -1 : 0;
}

ساكن صحيح
_make_posdefaults(Parser *p,
                  SlashWithDefault *slash_with_default,
                  asdl_seq *names_with_default,
                  asdl_expr_seq **posdefaults) {
    ܐܢ (slash_with_default != عدم && names_with_default != عدم) {
        asdl_expr_seq *slash_with_default_values =
                _get_defaults(p, slash_with_default->names_with_defaults);
        ܐܢ (!slash_with_default_values) {
            ܦܢܐ -1;
        }
        asdl_expr_seq *names_with_default_values = _get_defaults(p, names_with_default);
        ܐܢ (!names_with_default_values) {
            ܦܢܐ -1;
        }
        *posdefaults = (asdl_expr_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default_values,
                (asdl_seq*)names_with_default_values);
    }
    ܐܚܪܢܐ ܐܢ (slash_with_default == عدم && names_with_default != عدم) {
        *posdefaults = _get_defaults(p, names_with_default);
    }
    ܐܚܪܢܐ ܐܢ (slash_with_default != عدم && names_with_default == عدم) {
        *posdefaults = _get_defaults(p, slash_with_default->names_with_defaults);
    }
    ܐܚܪܢܐ {
        *posdefaults = _Py_asdl_expr_seq_new(0, p->arena);
    }
    ܦܢܐ *posdefaults == عدم ? -1 : 0;
}

ساكن صحيح
_make_kwargs(Parser *p, StarEtc *star_etc,
             asdl_arg_seq **kwonlyargs,
             asdl_expr_seq **kwdefaults) {
    ܐܢ (star_etc != عدم && star_etc->kwonlyargs != عدم) {
        *kwonlyargs = _get_names(p, star_etc->kwonlyargs);
    }
    ܐܚܪܢܐ {
        *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }

    ܐܢ (*kwonlyargs == عدم) {
        ܦܢܐ -1;
    }

    ܐܢ (star_etc != عدم && star_etc->kwonlyargs != عدم) {
        *kwdefaults = _get_defaults(p, star_etc->kwonlyargs);
    }
    ܐܚܪܢܐ {
        *kwdefaults = _Py_asdl_expr_seq_new(0, p->arena);
    }

    ܐܢ (*kwdefaults == عدم) {
        ܦܢܐ -1;
    }

    ܦܢܐ 0;
}

/* Constructs an arguments_ty object out of all the parsed constructs in the parameters rule */
arguments_ty
_PyPegen_make_arguments(Parser *p, asdl_arg_seq *slash_without_default,
                        SlashWithDefault *slash_with_default, asdl_arg_seq *plain_names,
                        asdl_seq *names_with_default, StarEtc *star_etc)
{
    asdl_arg_seq *posonlyargs;
    ܐܢ (_make_posonlyargs(p, slash_without_default, slash_with_default, &posonlyargs) == -1) {
        ܦܢܐ عدم;
    }

    asdl_arg_seq *posargs;
    ܐܢ (_make_posargs(p, plain_names, names_with_default, &posargs) == -1) {
        ܦܢܐ عدم;
    }

    asdl_expr_seq *posdefaults;
    ܐܢ (_make_posdefaults(p,slash_with_default, names_with_default, &posdefaults) == -1) {
        ܦܢܐ عدم;
    }

    arg_ty vararg = عدم;
    ܐܢ (star_etc != عدم && star_etc->vararg != عدم) {
        vararg = star_etc->vararg;
    }

    asdl_arg_seq *kwonlyargs;
    asdl_expr_seq *kwdefaults;
    ܐܢ (_make_kwargs(p, star_etc, &kwonlyargs, &kwdefaults) == -1) {
        ܦܢܐ عدم;
    }

    arg_ty kwarg = عدم;
    ܐܢ (star_etc != عدم && star_etc->kwarg != عدم) {
        kwarg = star_etc->kwarg;
    }

    ܦܢܐ _PyAST_arguments(posonlyargs, posargs, vararg, kwonlyargs,
                            kwdefaults, kwarg, posdefaults, p->arena);
}


/* Constructs an empty arguments_ty object, that gets used when a function accepts no
 * arguments. */
arguments_ty
_PyPegen_empty_arguments(Parser *p)
{
    asdl_arg_seq *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    ܐܢ (!posonlyargs) {
        ܦܢܐ عدم;
    }
    asdl_arg_seq *posargs = _Py_asdl_arg_seq_new(0, p->arena);
    ܐܢ (!posargs) {
        ܦܢܐ عدم;
    }
    asdl_expr_seq *posdefaults = _Py_asdl_expr_seq_new(0, p->arena);
    ܐܢ (!posdefaults) {
        ܦܢܐ عدم;
    }
    asdl_arg_seq *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    ܐܢ (!kwonlyargs) {
        ܦܢܐ عدم;
    }
    asdl_expr_seq *kwdefaults = _Py_asdl_expr_seq_new(0, p->arena);
    ܐܢ (!kwdefaults) {
        ܦܢܐ عدم;
    }

    ܦܢܐ _PyAST_arguments(posonlyargs, posargs, عدم, kwonlyargs,
                            kwdefaults, عدم, posdefaults, p->arena);
}

/* Encapsulates the value of an operator_ty into an AugOperator struct */
AugOperator *
_PyPegen_augoperator(Parser *p, operator_ty kind)
{
    AugOperator *a = _PyArena_Malloc(p->arena, حجم(AugOperator));
    ܐܢ (!a) {
        ܦܢܐ عدم;
    }
    a->kind = kind;
    ܦܢܐ a;
}

/* Construct a FunctionDef equivalent to function_def, but with decorators */
stmt_ty
_PyPegen_function_def_decorators(Parser *p, asdl_expr_seq *decorators, stmt_ty function_def)
{
    أكد(function_def != عدم);
    ܐܢ (function_def->kind == AsyncFunctionDef_kind) {
        ܦܢܐ _PyAST_AsyncFunctionDef(
            function_def->v.AsyncFunctionDef.name,
            function_def->v.AsyncFunctionDef.args,
            function_def->v.AsyncFunctionDef.body, decorators,
            function_def->v.AsyncFunctionDef.returns,
            function_def->v.AsyncFunctionDef.type_comment,
            function_def->v.AsyncFunctionDef.type_params,
            function_def->lineno, function_def->col_offset,
            function_def->end_lineno, function_def->end_col_offset, p->arena);
    }

    ܦܢܐ _PyAST_FunctionDef(
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
    أكد(class_def != عدم);
    ܦܢܐ _PyAST_ClassDef(
        class_def->v.ClassDef.name,
        class_def->v.ClassDef.bases, class_def->v.ClassDef.keywords,
        class_def->v.ClassDef.body, decorators,
        class_def->v.ClassDef.type_params,
        class_def->lineno, class_def->col_offset, class_def->end_lineno,
        class_def->end_col_offset, p->arena);
}

/* Construct a KeywordOrStarred */
KeywordOrStarred *
_PyPegen_keyword_or_starred(Parser *p, فراغ *element, صحيح is_keyword)
{
    KeywordOrStarred *a = _PyArena_Malloc(p->arena, حجم(KeywordOrStarred));
    ܐܢ (!a) {
        ܦܢܐ عدم;
    }
    a->element = element;
    a->is_keyword = is_keyword;
    ܦܢܐ a;
}

/* Get the number of starred expressions in an asdl_seq* of KeywordOrStarred*s */
ساكن صحيح
_seq_number_of_starred_exprs(asdl_seq *seq)
{
    صحيح n = 0;
    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(seq, i);
        ܐܢ (!k->is_keyword) {
            n++;
        }
    }
    ܦܢܐ n;
}

/* Extract the starred expressions of an asdl_seq* of KeywordOrStarred*s */
asdl_expr_seq *
_PyPegen_seq_extract_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    صحيح new_len = _seq_number_of_starred_exprs(kwargs);
    ܐܢ (new_len == 0) {
        ܦܢܐ عدم;
    }
    asdl_expr_seq *new_seq = _Py_asdl_expr_seq_new(new_len, p->arena);
    ܐܢ (!new_seq) {
        ܦܢܐ عدم;
    }

    صحيح idx = 0;
    لكل (Py_ssize_t i = 0, len = asdl_seq_LEN(kwargs); i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        ܐܢ (!k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    ܦܢܐ new_seq;
}

/* Return a new asdl_seq* with only the keywords in kwargs */
asdl_keyword_seq*
_PyPegen_seq_delete_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    Py_ssize_t len = asdl_seq_LEN(kwargs);
    Py_ssize_t new_len = len - _seq_number_of_starred_exprs(kwargs);
    ܐܢ (new_len == 0) {
        ܦܢܐ عدم;
    }
    asdl_keyword_seq *new_seq = _Py_asdl_keyword_seq_new(new_len, p->arena);
    ܐܢ (!new_seq) {
        ܦܢܐ عدم;
    }

    صحيح idx = 0;
    لكل (Py_ssize_t i = 0; i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        ܐܢ (k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    ܦܢܐ new_seq;
}

expr_ty
_PyPegen_ensure_imaginary(Parser *p, expr_ty exp)
{
    ܐܢ (exp->kind != Constant_kind || !PyComplex_CheckExact(exp->v.Constant.value)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "imaginary number required in complex literal");
        ܦܢܐ عدم;
    }
    ܦܢܐ exp;
}

expr_ty
_PyPegen_ensure_real(Parser *p, expr_ty exp)
{
    ܐܢ (exp->kind != Constant_kind || PyComplex_CheckExact(exp->v.Constant.value)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "real number required in complex literal");
        ܦܢܐ عدم;
    }
    ܦܢܐ exp;
}

mod_ty
_PyPegen_make_module(Parser *p, asdl_stmt_seq *a) {
    asdl_type_ignore_seq *type_ignores = عدم;
    Py_ssize_t num = p->type_ignore_comments.num_items;
    ܐܢ (num > 0) {
        // Turn the raw (comment, lineno) pairs into TypeIgnore objects in the arena
        type_ignores = _Py_asdl_type_ignore_seq_new(num, p->arena);
        ܐܢ (type_ignores == عدم) {
            ܦܢܐ عدم;
        }
        لكل (Py_ssize_t i = 0; i < num; i++) {
            PyObject *tag = _PyPegen_new_type_comment(p, p->type_ignore_comments.items[i].comment);
            ܐܢ (tag == عدم) {
                ܦܢܐ عدم;
            }
            type_ignore_ty ti = _PyAST_TypeIgnore(p->type_ignore_comments.items[i].lineno,
                                                  tag, p->arena);
            ܐܢ (ti == عدم) {
                ܦܢܐ عدم;
            }
            asdl_seq_SET(type_ignores, i, ti);
        }
    }
    ܦܢܐ _PyAST_Module(a, type_ignores, p->arena);
}

PyObject *
_PyPegen_new_type_comment(Parser *p, ثابت حرف *s)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, strlen(s), عدم);
    ܐܢ (res == عدم) {
        ܦܢܐ عدم;
    }
    ܐܢ (_PyArena_AddPyObject(p->arena, res) < 0) {
        Py_DECREF(res);
        ܦܢܐ عدم;
    }
    ܦܢܐ res;
}

arg_ty
_PyPegen_add_type_comment_to_arg(Parser *p, arg_ty a, Token *tc)
{
    ܐܢ (tc == عدم) {
        ܦܢܐ a;
    }
    ثابت حرف *bytes = PyBytes_AsString(tc->bytes);
    ܐܢ (bytes == عدم) {
        ܦܢܐ عدم;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    ܐܢ (tco == عدم) {
        ܦܢܐ عدم;
    }
    ܦܢܐ _PyAST_arg(a->arg, a->annotation, tco,
                      a->lineno, a->col_offset, a->end_lineno, a->end_col_offset,
                      p->arena);
}

/* Checks if the NOTEQUAL token is valid given the current parser flags
0 indicates success and nonzero indicates failure (an exception may be set) */
صحيح
_PyPegen_check_barry_as_flufl(Parser *p, Token* t) {
    أكد(t->bytes != عدم);
    أكد(t->type == NOTEQUAL);

    ثابت حرف* tok_str = PyBytes_AS_STRING(t->bytes);
    ܐܢ (p->flags & PyPARSE_BARRY_AS_BDFL && strcmp(tok_str, "<>") != 0) {
        RAISE_SYNTAX_ERROR("with Barry as BDFL, use '<>' instead of '!='");
        ܦܢܐ -1;
    }
    ܐܢ (!(p->flags & PyPARSE_BARRY_AS_BDFL)) {
        ܦܢܐ strcmp(tok_str, "!=");
    }
    ܦܢܐ 0;
}

صحيح
_PyPegen_check_legacy_stmt(Parser *p, expr_ty name) {
    ܐܢ (name->kind != Name_kind) {
        ܦܢܐ 0;
    }
    ثابت حرف* candidates[2] = {"print", "exec"};
    لكل (صحيح i=0; i<2; i++) {
        ܐܢ (PyUnicode_CompareWithASCIIString(name->v.Name.id, candidates[i]) == 0) {
            ܦܢܐ 1;
        }
    }
    ܦܢܐ 0;
}

فراغ *
_PyPegen_raise_error_for_missing_comma(Parser *p, expr_ty a, expr_ty b)
{
    // Don't raise for legacy statements like "print x" or "exec x"
    ܐܢ (_PyPegen_check_legacy_stmt(p, a)) {
        ܦܢܐ عدم;
    }
    // Only raise inside parentheses/brackets (level > 0)
    ܐܢ (p->tokens[p->mark - 1]->level == 0) {
        ܦܢܐ عدم;
    }
    // For multi-line expressions (like string concatenations), point to the
    // last line instead of the first for a more helpful error message.
    // Use a->col_offset as the starting column since all strings in the
    // concatenation typically share the same indentation.
    ܐܢ (a->end_lineno > a->lineno) {
        ܦܢܐ RAISE_ERROR_KNOWN_LOCATION(
            p, PyExc_SyntaxError, a->end_lineno, a->col_offset,
            a->end_lineno, a->end_col_offset,
            "invalid syntax. Perhaps you forgot a comma?"
        );
    }
    ܦܢܐ RAISE_ERROR_KNOWN_LOCATION(
        p, PyExc_SyntaxError, a->lineno, a->col_offset,
        b->end_lineno, b->end_col_offset,
        "invalid syntax. Perhaps you forgot a comma?"
    );
}

ساكن ResultTokenWithMetadata *
result_token_with_metadata(Parser *p, فراغ *result, PyObject *metadata)
{
    ResultTokenWithMetadata *res = _PyArena_Malloc(p->arena, حجم(ResultTokenWithMetadata));
    ܐܢ (res == عدم) {
        ܦܢܐ عدم;
    }
    res->metadata = metadata;
    res->result = result;
    ܦܢܐ res;
}

ساكن حرف
formatted_string_prefix(ثابت Parser *p)
{
    صحيح nested = 0;
    لكل (صحيح i = p->mark - 1; i >= 0; i--) {
        صحيح type = p->tokens[i]->type;
        ܐܢ (type == FSTRING_END || type == TSTRING_END) {
            nested++;
        }
        ܐܚܪܢܐ ܐܢ (type == FSTRING_START || type == TSTRING_START) {
            ܐܢ (nested == 0) {
                ܦܢܐ type == TSTRING_START ? 't' : 'f';
            }
            nested--;
        }
    }
    Py_UNREACHABLE();
}

ResultTokenWithMetadata *
_PyPegen_check_fstring_conversion(Parser *p, Token* conv_token, expr_ty conv)
{
    ܐܢ (conv_token->lineno != conv->lineno || conv_token->end_col_offset != conv->col_offset) {
        ܦܢܐ RAISE_SYNTAX_ERROR_KNOWN_RANGE(
            conv_token, conv,
            "%c-string: conversion type must come right after the exclamation mark",
            formatted_string_prefix(p)
        );
    }

    Py_UCS4 first = PyUnicode_READ_CHAR(conv->v.Name.id, 0);
    ܐܢ (PyUnicode_GET_LENGTH(conv->v.Name.id) > 1 ||
            !(first == 's' || first == 'r' || first == 'a')) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(conv,
                                            "%c-string: invalid conversion character %R: expected 's', 'r', or 'a'",
                                            formatted_string_prefix(p),
                                            conv->v.Name.id);
        ܦܢܐ عدم;
    }

    ܦܢܐ result_token_with_metadata(p, conv, conv_token->metadata);
}

ResultTokenWithMetadata *
_PyPegen_setup_full_format_spec(Parser *p, Token *colon, asdl_expr_seq *spec, صحيح lineno, صحيح col_offset,
                                صحيح end_lineno, صحيح end_col_offset, PyArena *arena)
{
    ܐܢ (!spec) {
        ܦܢܐ عدم;
    }

    // This is needed to keep compatibility with 3.11, where an empty format
    // spec is parsed as an *empty* JoinedStr node, instead of having an empty
    // constant in it.
    Py_ssize_t n_items = asdl_seq_LEN(spec);
    Py_ssize_t non_empty_count = 0;
    لكل (Py_ssize_t i = 0; i < n_items; i++) {
        expr_ty item = asdl_seq_GET(spec, i);
        non_empty_count += !(item->kind == Constant_kind &&
                             PyUnicode_CheckExact(item->v.Constant.value) &&
                             PyUnicode_GET_LENGTH(item->v.Constant.value) == 0);
    }
    ܐܢ (non_empty_count != n_items) {
        asdl_expr_seq *resized_spec =
            _Py_asdl_expr_seq_new(non_empty_count, p->arena);
        ܐܢ (resized_spec == عدم) {
            ܦܢܐ عدم;
        }
        Py_ssize_t j = 0;
        لكل (Py_ssize_t i = 0; i < n_items; i++) {
            expr_ty item = asdl_seq_GET(spec, i);
            ܐܢ (item->kind == Constant_kind &&
                PyUnicode_CheckExact(item->v.Constant.value) &&
                PyUnicode_GET_LENGTH(item->v.Constant.value) == 0) {
                استمر;
            }
            asdl_seq_SET(resized_spec, j++, item);
        }
        أكد(j == non_empty_count);
        spec = resized_spec;
    }
    expr_ty res;
    Py_ssize_t n = asdl_seq_LEN(spec);
    ܐܢ (n == 0 || (n == 1 && asdl_seq_GET(spec, 0)->kind == Constant_kind)) {
        res = _PyAST_JoinedStr(spec, lineno, col_offset, end_lineno,
                                    end_col_offset, p->arena);
    } ܐܚܪܢܐ {
        res = _PyPegen_concatenate_strings(p, spec,
                             lineno, col_offset, end_lineno,
                             end_col_offset, arena);
    }
    ܐܢ (!res) {
        ܦܢܐ عدم;
    }
    ܦܢܐ result_token_with_metadata(p, res, colon->metadata);
}

ثابت حرف *
_PyPegen_get_expr_name(expr_ty e)
{
    أكد(e != عدم);
    بدل (e->kind) {
        حالة Attribute_kind:
            ܦܢܐ "attribute";
        حالة Subscript_kind:
            ܦܢܐ "subscript";
        حالة Starred_kind:
            ܦܢܐ "starred";
        حالة Name_kind:
            ܦܢܐ "name";
        حالة List_kind:
            ܦܢܐ "list";
        حالة Tuple_kind:
            ܦܢܐ "tuple";
        حالة Lambda_kind:
            ܦܢܐ "lambda";
        حالة Call_kind:
            ܦܢܐ "function call";
        حالة BoolOp_kind:
        حالة BinOp_kind:
        حالة UnaryOp_kind:
            ܦܢܐ "expression";
        حالة GeneratorExp_kind:
            ܦܢܐ "generator expression";
        حالة Yield_kind:
        حالة YieldFrom_kind:
            ܦܢܐ "yield expression";
        حالة Await_kind:
            ܦܢܐ "await expression";
        حالة ListComp_kind:
            ܦܢܐ "list comprehension";
        حالة SetComp_kind:
            ܦܢܐ "set comprehension";
        حالة DictComp_kind:
            ܦܢܐ "dict comprehension";
        حالة Dict_kind:
            ܦܢܐ "dict literal";
        حالة Set_kind:
            ܦܢܐ "set display";
        حالة JoinedStr_kind:
        حالة FormattedValue_kind:
            ܦܢܐ "f-string expression";
        حالة TemplateStr_kind:
        حالة Interpolation_kind:
            ܦܢܐ "t-string expression";
        حالة Constant_kind: {
            PyObject *value = e->v.Constant.value;
            ܐܢ (value == Py_None) {
                ܦܢܐ "None";
            }
            ܐܢ (value == Py_False) {
                ܦܢܐ "False";
            }
            ܐܢ (value == Py_True) {
                ܦܢܐ "True";
            }
            ܐܢ (value == Py_Ellipsis) {
                ܦܢܐ "ellipsis";
            }
            ܦܢܐ "literal";
        }
        حالة Compare_kind:
            ܦܢܐ "comparison";
        حالة IfExp_kind:
            ܦܢܐ "conditional expression";
        حالة NamedExpr_kind:
            ܦܢܐ "named expression";
        افتراضي:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->kind, e->lineno);
            ܦܢܐ عدم;
    }
}

expr_ty
_PyPegen_get_last_comprehension_item(comprehension_ty comprehension) {
    ܐܢ (comprehension->ifs == عدم || asdl_seq_LEN(comprehension->ifs) == 0) {
        ܦܢܐ comprehension->iter;
    }
    ܦܢܐ PyPegen_last_item(comprehension->ifs, expr_ty);
}

expr_ty _PyPegen_collect_call_seqs(Parser *p, asdl_expr_seq *a, asdl_seq *b,
                     صحيح lineno, صحيح col_offset, صحيح end_lineno,
                     صحيح end_col_offset, PyArena *arena) {
    Py_ssize_t args_len = asdl_seq_LEN(a);
    Py_ssize_t total_len = args_len;

    ܐܢ (b == عدم) {
        ܦܢܐ _PyAST_Call(_PyPegen_dummy_name(p), a, عدم, lineno, col_offset,
                        end_lineno, end_col_offset, arena);

    }

    asdl_expr_seq *starreds = _PyPegen_seq_extract_starred_exprs(p, b);
    ܐܢ (!starreds && PyErr_Occurred()) {
        ܦܢܐ عدم;
    }

    asdl_keyword_seq *keywords = _PyPegen_seq_delete_starred_exprs(p, b);
    ܐܢ (!keywords && PyErr_Occurred()) {
        ܦܢܐ عدم;
    }

    ܐܢ (starreds) {
        total_len += asdl_seq_LEN(starreds);
    }

    asdl_expr_seq *args = _Py_asdl_expr_seq_new(total_len, arena);
    ܐܢ (args == عدم) {
        ܦܢܐ عدم;
    }

    Py_ssize_t i = 0;
    لكل (i = 0; i < args_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(a, i));
    }
    لكل (; i < total_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(starreds, i - args_len));
    }

    ܦܢܐ _PyAST_Call(_PyPegen_dummy_name(p), args, keywords, lineno,
                       col_offset, end_lineno, end_col_offset, arena);
}

// AST Error reporting helpers

expr_ty
_PyPegen_get_invalid_target(expr_ty e, TARGETS_TYPE targets_type)
{
    ܐܢ (e == عدم) {
        ܦܢܐ عدم;
    }

    ܐܢ (targets_type == SINGLE_TARGETS ||
        targets_type == ATTRIBUTE_OR_SUBSCRIPT_TARGETS) {
        ܐܢ (targets_type == SINGLE_TARGETS && e->kind == Name_kind) {
            ܦܢܐ عدم;
        }
        ܦܢܐ e->kind == Attribute_kind || e->kind == Subscript_kind ? عدم : e;
    }

#define VISIT_CONTAINER(CONTAINER, TYPE) ܥܒܕ { \
        Py_ssize_t len = asdl_seq_LEN((CONTAINER)->v.TYPE.elts);\
        لكل (Py_ssize_t i = 0; i < len; i++) {\
            expr_ty other = asdl_seq_GET((CONTAINER)->v.TYPE.elts, i);\
            expr_ty child = _PyPegen_get_invalid_target(other, targets_type);\
            ܐܢ (child != عدم) {\
                ܦܢܐ child;\
            }\
        }\
    } ܟܡܐ (0)

    // We only need to visit List and Tuple nodes recursively as those
    // are the only ones that can contain valid names in targets when
    // they are parsed as expressions. Any other kind of expression
    // that is a container (like Sets or Dicts) is directly invalid and
    // we don't need to visit it recursively.

    بدل (e->kind) {
        حالة List_kind:
            VISIT_CONTAINER(e, List);
            ܦܢܐ عدم;
        حالة Tuple_kind:
            VISIT_CONTAINER(e, Tuple);
            ܦܢܐ عدم;
        حالة Starred_kind:
            ܐܢ (targets_type == DEL_TARGETS) {
                ܦܢܐ e;
            }
            ܦܢܐ _PyPegen_get_invalid_target(e->v.Starred.value, targets_type);
        حالة Compare_kind:
            // This is needed, because the `a in b` in `for a in b` gets parsed
            // as a comparison, and so we need to search the left side of the comparison
            // for invalid targets.
            ܐܢ (targets_type == FOR_TARGETS) {
                cmpop_ty cmpop = (cmpop_ty) asdl_seq_GET(e->v.Compare.ops, 0);
                ܐܢ (cmpop == In) {
                    ܦܢܐ _PyPegen_get_invalid_target(e->v.Compare.left, targets_type);
                }
                ܦܢܐ عدم;
            }
            ܦܢܐ e;
        حالة Name_kind:
        حالة Subscript_kind:
        حالة Attribute_kind:
            ܦܢܐ عدم;
        افتراضي:
            ܦܢܐ e;
    }
}

فراغ *_PyPegen_arguments_parsing_error(Parser *p, expr_ty e) {
    صحيح kwarg_unpacking = 0;
    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(e->v.Call.keywords); i < l; i++) {
        keyword_ty keyword = asdl_seq_GET(e->v.Call.keywords, i);
        ܐܢ (!keyword->arg) {
            kwarg_unpacking = 1;
        }
    }

    ثابت حرف *msg = عدم;
    ܐܢ (kwarg_unpacking) {
        msg = "positional argument follows keyword argument unpacking";
    } ܐܚܪܢܐ {
        msg = "positional argument follows keyword argument";
    }

    ܦܢܐ RAISE_SYNTAX_ERROR(msg);
}

فراغ *
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
    ܐܢ (len <= 1) {
        ܦܢܐ عدم;
    }

    comprehension_ty last_comprehension = PyPegen_last_item(comprehensions, comprehension_ty);

    ܦܢܐ RAISE_SYNTAX_ERROR_KNOWN_RANGE(
        (expr_ty) asdl_seq_GET(args->v.Call.args, len - 1),
        _PyPegen_get_last_comprehension_item(last_comprehension),
        "Generator expression must be parenthesized"
    );
}

// Fstring stuff

ساكن expr_ty
_PyPegen_decode_fstring_part(Parser* p, صحيح is_raw, expr_ty constant, Token* token) {
    أكد(PyUnicode_CheckExact(constant->v.Constant.value));

    ثابت حرف* bstr = PyUnicode_AsUTF8(constant->v.Constant.value);
    ܐܢ (bstr == عدم) {
        ܦܢܐ عدم;
    }

    size_t len;
    ܐܢ (strcmp(bstr, "{{") == 0 || strcmp(bstr, "}}") == 0) {
        len = 1;
    } ܐܚܪܢܐ {
        len = strlen(bstr);
    }

    is_raw = is_raw || strchr(bstr, '\\') == عدم;
    PyObject *str = _PyPegen_decode_string(p, is_raw, bstr, len, token);
    ܐܢ (str == عدم) {
        _Pypegen_raise_decode_error(p);
        ܦܢܐ عدم;
    }
    ܐܢ (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ܦܢܐ عدم;
    }
    ܦܢܐ _PyAST_Constant(str, عدم, constant->lineno, constant->col_offset,
                           constant->end_lineno, constant->end_col_offset,
                           p->arena);
}

ساكن asdl_expr_seq *
_get_resized_exprs(Parser *p, Token *a, asdl_expr_seq *raw_expressions,
                   Token *b, صحيح is_tstring)
{
    Py_ssize_t n_items = asdl_seq_LEN(raw_expressions);
    Py_ssize_t total_items = n_items;
    لكل (Py_ssize_t i = 0; i < n_items; i++) {
        expr_ty item = asdl_seq_GET(raw_expressions, i);
        ܐܢ (item->kind == JoinedStr_kind) {
            total_items += asdl_seq_LEN(item->v.JoinedStr.values) - 1;
        }
    }

    ثابت حرف* quote_str = PyBytes_AsString(a->bytes);
    ܐܢ (quote_str == عدم) {
        ܦܢܐ عدم;
    }
    صحيح is_raw = strpbrk(quote_str, "rR") != عدم;

    asdl_expr_seq *seq = _Py_asdl_expr_seq_new(total_items, p->arena);
    ܐܢ (seq == عدم) {
        ܦܢܐ عدم;
    }

    Py_ssize_t index = 0;
    لكل (Py_ssize_t i = 0; i < n_items; i++) {
        expr_ty item = asdl_seq_GET(raw_expressions, i);

        /* Debug expressions arrive as JoinedStr(text, value); flatten them
           into the surrounding string. */
        ܐܢ (item->kind == JoinedStr_kind) {
            asdl_expr_seq *values = item->v.JoinedStr.values;
            ܐܢ (asdl_seq_LEN(values) != 2) {
                PyErr_Format(PyExc_SystemError,
                             is_tstring
                             ? "unexpected TemplateStr node without debug data in t-string at line %d"
                             : "unexpected JoinedStr node without debug data in f-string at line %d",
                             item->lineno);
                ܦܢܐ عدم;
            }

            expr_ty first = asdl_seq_GET(values, 0);
            أكد(first->kind == Constant_kind);
            asdl_seq_SET(seq, index++, first);

            expr_ty second = asdl_seq_GET(values, 1);
            أكد((is_tstring &&
                    second->kind == Interpolation_kind) ||
                   second->kind == FormattedValue_kind);
            asdl_seq_SET(seq, index++, second);

            استمر;
        }

        ܐܢ (item->kind == Constant_kind) {
            item = _PyPegen_decode_fstring_part(p, is_raw, item, b);
            ܐܢ (item == عدم) {
                ܦܢܐ عدم;
            }

            /* Tokenizer emits string parts even when the underlying string
            might become an empty value (e.g. FSTRING_MIDDLE with the value \\n)
            so we need to check for them and simplify it here. */
            ܐܢ (PyUnicode_CheckExact(item->v.Constant.value)
                && PyUnicode_GET_LENGTH(item->v.Constant.value) == 0) {
                استمر;
            }
        }
        asdl_seq_SET(seq, index++, item);
    }

    asdl_expr_seq *resized_exprs;
    ܐܢ (index != total_items) {
        resized_exprs = _Py_asdl_expr_seq_new(index, p->arena);
        ܐܢ (resized_exprs == عدم) {
            ܦܢܐ عدم;
        }
        لكل (Py_ssize_t i = 0; i < index; i++) {
            asdl_seq_SET(resized_exprs, i, asdl_seq_GET(seq, i));
        }
    }
    ܐܚܪܢܐ {
        resized_exprs = seq;
    }
    ܦܢܐ resized_exprs;
}

expr_ty
_PyPegen_template_str(Parser *p, Token *a, asdl_expr_seq *raw_expressions, Token *b) {

    asdl_expr_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 1);
    ܐܢ (resized_exprs == عدم) {
        ܦܢܐ عدم;
    }
    ܦܢܐ _PyAST_TemplateStr(resized_exprs, a->lineno, a->col_offset,
                              b->end_lineno, b->end_col_offset,
                              p->arena);
}

expr_ty
_PyPegen_joined_str(Parser *p, Token* a, asdl_expr_seq* raw_expressions, Token*b) {

    asdl_expr_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 0);
    ܐܢ (resized_exprs == عدم) {
        ܦܢܐ عدم;
    }
    ܦܢܐ _PyAST_JoinedStr(resized_exprs, a->lineno, a->col_offset,
                            b->end_lineno, b->end_col_offset,
                            p->arena);
}

expr_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok) {
    ܐܢ (tok->bytes == عدم) {
        ܦܢܐ _PyPegen_raise_incomplete_input(p);
    }
    Py_ssize_t bsize = PyBytes_GET_SIZE(tok->bytes);
    ثابت حرف *bstr = PyBytes_AS_STRING(tok->bytes);

    صحيح is_raw = tok->is_raw;

    PyObject* str = _PyPegen_decode_string(p, is_raw, bstr, bsize, tok);
    ܐܢ (str == عدم) {
        ܦܢܐ عدم;
    }
    ܐܢ (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ܦܢܐ عدم;
    }
    ܦܢܐ _PyAST_Constant(str, عدم, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

expr_ty _PyPegen_constant_from_token(Parser* p, Token* tok) {
    ܐܢ (tok->bytes == عدم) {
        ܦܢܐ _PyPegen_raise_incomplete_input(p);
    }
    ثابت حرف *bstr = PyBytes_AS_STRING(tok->bytes);
    PyObject* str = PyUnicode_FromString(bstr);
    ܐܢ (str == عدم) {
        ܦܢܐ عدم;
    }
    ܐܢ (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ܦܢܐ عدم;
    }
    ܦܢܐ _PyAST_Constant(str, عدم, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

expr_ty _PyPegen_constant_from_string(Parser* p, Token* tok) {
    ܐܢ (tok->bytes == عدم) {
        ܦܢܐ _PyPegen_raise_incomplete_input(p);
    }
    ثابت حرف *the_str = PyBytes_AS_STRING(tok->bytes);
    PyObject *s = _PyPegen_parse_string(p, tok);
    ܐܢ (s == عدم) {
        _Pypegen_raise_decode_error(p);
        ܦܢܐ عدم;
    }
    ܐܢ (_PyArena_AddPyObject(p->arena, s) < 0) {
        Py_DECREF(s);
        ܦܢܐ عدم;
    }
    PyObject *kind = عدم;
    ܐܢ (the_str && the_str[0] == 'u') {
        kind = _PyPegen_new_identifier(p, "u");
        ܐܢ (kind == عدم) {
            ܦܢܐ عدم;
        }
    }
    ܦܢܐ _PyAST_Constant(s, kind, tok->lineno, tok->col_offset, tok->end_lineno, tok->end_col_offset, p->arena);
}

ساكن صحيح
_get_interpolation_conversion(Parser *p, Token *debug, ResultTokenWithMetadata *conversion,
                              ResultTokenWithMetadata *format)
{
    ܐܢ (conversion != عدم) {
        expr_ty conversion_expr = (expr_ty) conversion->result;
        أكد(conversion_expr->kind == Name_kind);
        Py_UCS4 first = PyUnicode_READ_CHAR(conversion_expr->v.Name.id, 0);
        ܦܢܐ Py_SAFE_DOWNCAST(first, Py_UCS4, صحيح);
    }
    ܐܚܪܢܐ ܐܢ (debug && !format) {
        /* If no conversion is specified, use !r for debug expressions */
        ܦܢܐ (صحيح)'r';
    }
    ܦܢܐ -1;
}

ساكن PyObject *
_strip_interpolation_debug_expr(PyObject *exprstr)
{
    Py_ssize_t len = PyUnicode_GET_LENGTH(exprstr);

    /* Discard whitespace and explicit line continuations after the debug "="
       but preserve whitespace before it. */
    ܟܡܐ (len > 0) {
        صحيح has_newline = 0;
        ܟܡܐ (len > 0) {
            Py_UCS4 c = PyUnicode_READ_CHAR(exprstr, len - 1);
            ܐܢ (!_PyUnicode_IsWhitespace(c)) {
                اكسر;
            }
            ܐܢ (c == '\r' || c == '\n') {
                has_newline = 1;
            }
            len--;
        }
        ܐܢ (!has_newline || len == 0 ||
            PyUnicode_READ_CHAR(exprstr, len - 1) != '\\')
        {
            اكسر;
        }
        len--;
    }

    /* Preserve unexpected metadata instead of dropping source text. */
    ܐܢ (len == 0 || PyUnicode_READ_CHAR(exprstr, len - 1) != '=') {
        ܦܢܐ Py_NewRef(exprstr);
    }

    ܦܢܐ PyUnicode_Substring(exprstr, 0, len - 1);
}

expr_ty _PyPegen_interpolation(Parser *p, expr_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, صحيح lineno, صحيح col_offset,
                                 صحيح end_lineno, صحيح end_col_offset, PyArena *arena) {

    صحيح conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    /* Find the non whitespace token after the "=" */
    صحيح debug_end_line, debug_end_offset;
    PyObject *debug_metadata;
    constant exprstr;

    ܐܢ (conversion) {
        debug_end_line = ((expr_ty) conversion->result)->lineno;
        debug_end_offset = ((expr_ty) conversion->result)->col_offset;
        debug_metadata = exprstr = conversion->metadata;
    }
    ܐܚܪܢܐ ܐܢ (format) {
        debug_end_line = ((expr_ty) format->result)->lineno;
        debug_end_offset = ((expr_ty) format->result)->col_offset + 1;
        debug_metadata = exprstr = format->metadata;
    }
    ܐܚܪܢܐ {
        debug_end_line = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = exprstr = closing_brace->metadata;
    }

    أكد(exprstr != عدم);
    PyObject *final_exprstr = debug
        ? _strip_interpolation_debug_expr(exprstr)
        : Py_NewRef(exprstr);
    ܐܢ (!final_exprstr || _PyArena_AddPyObject(arena, final_exprstr) < 0) {
        Py_XDECREF(final_exprstr);
        ܦܢܐ عدم;
    }

    expr_ty interpolation = _PyAST_Interpolation(
        expression, final_exprstr, conversion_val, format ? (expr_ty) format->result : عدم,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    ܐܢ (!interpolation || !debug) {
        ܦܢܐ interpolation;
    }

    expr_ty debug_text = _PyAST_Constant(debug_metadata, عدم, lineno, col_offset + 1, debug_end_line,
                                            debug_end_offset - 1, p->arena);
    ܐܢ (!debug_text) {
        ܦܢܐ عدم;
    }

    asdl_expr_seq *values = _Py_asdl_expr_seq_new(2, arena);
    ܐܢ (!values) {
        ܦܢܐ عدم;
    }
    asdl_seq_SET(values, 0, debug_text);
    asdl_seq_SET(values, 1, interpolation);
    ܦܢܐ _PyAST_JoinedStr(values, lineno, col_offset, debug_end_line, debug_end_offset, p->arena);
}

expr_ty _PyPegen_formatted_value(Parser *p, expr_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, صحيح lineno, صحيح col_offset,
                                 صحيح end_lineno, صحيح end_col_offset, PyArena *arena) {
    صحيح conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    expr_ty formatted_value = _PyAST_FormattedValue(
        expression, conversion_val, format ? (expr_ty) format->result : عدم,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    ܐܢ (!formatted_value || !debug) {
        ܦܢܐ formatted_value;
    }

    /* Find the non whitespace token after the "=" */
    صحيح debug_end_line, debug_end_offset;
    PyObject *debug_metadata;

    ܐܢ (conversion) {
        debug_end_line = ((expr_ty) conversion->result)->lineno;
        debug_end_offset = ((expr_ty) conversion->result)->col_offset;
        debug_metadata = conversion->metadata;
    }
    ܐܚܪܢܐ ܐܢ (format) {
        debug_end_line = ((expr_ty) format->result)->lineno;
        debug_end_offset = ((expr_ty) format->result)->col_offset + 1;
        debug_metadata = format->metadata;
    }
    ܐܚܪܢܐ {
        debug_end_line = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = closing_brace->metadata;
    }
    expr_ty debug_text = _PyAST_Constant(debug_metadata, عدم, lineno, col_offset + 1, debug_end_line,
                                            debug_end_offset - 1, p->arena);
    ܐܢ (!debug_text) {
        ܦܢܐ عدم;
    }

    asdl_expr_seq *values = _Py_asdl_expr_seq_new(2, arena);
    ܐܢ (!values) {
        ܦܢܐ عدم;
    }
    asdl_seq_SET(values, 0, debug_text);
    asdl_seq_SET(values, 1, formatted_value);
    ܦܢܐ _PyAST_JoinedStr(values, lineno, col_offset, debug_end_line, debug_end_offset, p->arena);
}

ساكن expr_ty
_build_concatenated_bytes(Parser *p, asdl_expr_seq *strings, صحيح lineno,
                        صحيح col_offset, صحيح end_lineno, صحيح end_col_offset,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    أكد(len > 0);

    /* Bytes literals never get a kind, but just for consistency
        since they are represented as Constant nodes, we'll mirror
        the same behavior as unicode strings for determining the
        kind. */
    PyObject *kind = asdl_seq_GET(strings, 0)->v.Constant.kind;

    Py_ssize_t total = 0;
    لكل (Py_ssize_t i = 0; i < len; i++) {
        expr_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.value;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        ܐܢ (part > PY_SSIZE_T_MAX - total) {
            PyErr_NoMemory();
            ܦܢܐ عدم;
        }
        total += part;
    }

    PyBytesWriter *writer = PyBytesWriter_Create(total);
    ܐܢ (writer == عدم) {
        ܦܢܐ عدم;
    }
    حرف *out = PyBytesWriter_GetData(writer);

    لكل (Py_ssize_t i = 0; i < len; i++) {
        expr_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.value;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        ܐܢ (part > 0) {
            memcpy(out, PyBytes_AS_STRING(bytes), part);
            out += part;
        }
    }

    PyObject *res = PyBytesWriter_Finish(writer);
    ܐܢ (res == عدم) {
        ܦܢܐ عدم;
    }
    ܐܢ (_PyArena_AddPyObject(arena, res) < 0) {
        Py_DECREF(res);
        ܦܢܐ عدم;
    }
    ܦܢܐ _PyAST_Constant(res, kind, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

ساكن expr_ty
_build_concatenated_unicode(Parser *p, asdl_expr_seq *strings, صحيح lineno,
                        صحيح col_offset, صحيح end_lineno, صحيح end_col_offset,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    أكد(len > 1);

    expr_ty first = asdl_seq_GET(strings, 0);

    /* When a string is getting concatenated, the kind of the string
        is determined by the first string in the concatenation
        sequence.

        u"abc" "def" -> u"abcdef"
        "abc" u"abc" ->  "abcabc" */
    PyObject *kind = first->v.Constant.kind;

    PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
    ܐܢ (writer == عدم) {
        ܦܢܐ عدم;
    }

    لكل (Py_ssize_t i = 0; i < len; i++) {
        expr_ty current_elem = asdl_seq_GET(strings, i);
        أكد(current_elem->kind == Constant_kind);

        ܐܢ (PyUnicodeWriter_WriteStr(writer,
                                     current_elem->v.Constant.value)) {
            PyUnicodeWriter_Discard(writer);
            ܦܢܐ عدم;
        }
    }

    PyObject *final = PyUnicodeWriter_Finish(writer);
    ܐܢ (final == عدم) {
        ܦܢܐ عدم;
    }
    ܐܢ (_PyArena_AddPyObject(p->arena, final) < 0) {
        Py_DECREF(final);
        ܦܢܐ عدم;
    }
    ܦܢܐ _PyAST_Constant(final, kind, lineno, col_offset,
                           end_lineno, end_col_offset, arena);
}

ساكن asdl_expr_seq *
_build_concatenated_str(Parser *p, asdl_expr_seq *strings,
                               صحيح lineno, صحيح col_offset, صحيح end_lineno,
                               صحيح end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    أكد(len > 0);

    Py_ssize_t n_flattened_elements = 0;
    لكل (Py_ssize_t i = 0; i < len; i++) {
        expr_ty elem = asdl_seq_GET(strings, i);
        بدل(elem->kind) {
            حالة JoinedStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.JoinedStr.values);
                اكسر;
            حالة TemplateStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.TemplateStr.values);
                اكسر;
            افتراضي:
                n_flattened_elements++;
                اكسر;
        }
    }


    asdl_expr_seq* flattened = _Py_asdl_expr_seq_new(n_flattened_elements, p->arena);
    ܐܢ (flattened == عدم) {
        ܦܢܐ عدم;
    }

    /* build flattened list */
    Py_ssize_t current_pos = 0;
    لكل (Py_ssize_t i = 0; i < len; i++) {
        expr_ty elem = asdl_seq_GET(strings, i);
        بدل(elem->kind) {
            حالة JoinedStr_kind:
                لكل (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.JoinedStr.values); j++) {
                    expr_ty subvalue = asdl_seq_GET(elem->v.JoinedStr.values, j);
                    ܐܢ (subvalue == عدم) {
                        ܦܢܐ عدم;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                اكسر;
            حالة TemplateStr_kind:
                لكل (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.TemplateStr.values); j++) {
                    expr_ty subvalue = asdl_seq_GET(elem->v.TemplateStr.values, j);
                    ܐܢ (subvalue == عدم) {
                        ܦܢܐ عدم;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                اكسر;
            افتراضي:
                asdl_seq_SET(flattened, current_pos++, elem);
                اكسر;
        }
    }

    /* calculate folded element count */
    Py_ssize_t n_elements = 0;
    صحيح prev_is_constant = 0;
    لكل (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        expr_ty elem = asdl_seq_GET(flattened, i);

        /* The concatenation of a FormattedValue and an empty Constant should
           lead to the FormattedValue itself. Thus, we will not take any empty
           constants into account, just as in `_PyPegen_joined_str` */
        ܐܢ (elem->kind == Constant_kind &&
            PyUnicode_CheckExact(elem->v.Constant.value) &&
            PyUnicode_GET_LENGTH(elem->v.Constant.value) == 0)
            استمر;

        ܐܢ (!prev_is_constant || elem->kind != Constant_kind) {
            n_elements++;
        }
        prev_is_constant = elem->kind == Constant_kind;
    }

    asdl_expr_seq* values = _Py_asdl_expr_seq_new(n_elements, p->arena);
    ܐܢ (values == عدم) {
        ܦܢܐ عدم;
    }

    /* build folded list */
    current_pos = 0;
    لكل (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        expr_ty elem = asdl_seq_GET(flattened, i);

        /* if the current elem and the following are constants,
           fold them and all consequent constants */
        ܐܢ (elem->kind == Constant_kind) {
            ܐܢ (i + 1 < n_flattened_elements &&
                asdl_seq_GET(flattened, i + 1)->kind == Constant_kind) {
                expr_ty first_elem = elem;

                /* When a string is getting concatenated, the kind of the string
                   is determined by the first string in the concatenation
                   sequence.

                   u"abc" "def" -> u"abcdef"
                   "abc" u"abc" ->  "abcabc" */
                PyObject *kind = elem->v.Constant.kind;

                PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
                ܐܢ (writer == عدم) {
                    ܦܢܐ عدم;
                }
                expr_ty last_elem = elem;
                Py_ssize_t j;
                لكل (j = i; j < n_flattened_elements; j++) {
                    expr_ty current_elem = asdl_seq_GET(flattened, j);
                    ܐܢ (current_elem->kind == Constant_kind) {
                        ܐܢ (PyUnicodeWriter_WriteStr(writer,
                                                     current_elem->v.Constant.value)) {
                            PyUnicodeWriter_Discard(writer);
                            ܦܢܐ عدم;
                        }
                        last_elem = current_elem;
                    } ܐܚܪܢܐ {
                        اكسر;
                    }
                }
                i = j - 1;

                PyObject *concat_str = PyUnicodeWriter_Finish(writer);
                ܐܢ (concat_str == عدم) {
                    ܦܢܐ عدم;
                }
                ܐܢ (_PyArena_AddPyObject(p->arena, concat_str) < 0) {
                    Py_DECREF(concat_str);
                    ܦܢܐ عدم;
                }
                elem = _PyAST_Constant(concat_str, kind, first_elem->lineno,
                                       first_elem->col_offset,
                                       last_elem->end_lineno,
                                       last_elem->end_col_offset, p->arena);
                ܐܢ (elem == عدم) {
                    ܦܢܐ عدم;
                }
            }

            /* Drop all empty contanst strings */
            ܐܢ (PyUnicode_CheckExact(elem->v.Constant.value) &&
                PyUnicode_GET_LENGTH(elem->v.Constant.value) == 0) {
                استمر;
            }
        }

        asdl_seq_SET(values, current_pos++, elem);
    }

    أكد(current_pos == n_elements);
    ܦܢܐ values;
}

ساكن expr_ty
_build_concatenated_joined_str(Parser *p, asdl_expr_seq *strings,
                               صحيح lineno, صحيح col_offset, صحيح end_lineno,
                               صحيح end_col_offset, PyArena *arena)
{
    asdl_expr_seq *values = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    ܐܢ (!values) {
        ܦܢܐ عدم;
    }
    ܦܢܐ _PyAST_JoinedStr(values, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

expr_ty
_PyPegen_concatenate_tstrings(Parser *p, asdl_expr_seq *strings,
                               صحيح lineno, صحيح col_offset, صحيح end_lineno,
                               صحيح end_col_offset, PyArena *arena)
{
    asdl_expr_seq *values = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    ܐܢ (!values) {
        ܦܢܐ عدم;
    }
    ܦܢܐ _PyAST_TemplateStr(values, lineno, col_offset, end_lineno,
        end_col_offset, arena);
}

expr_ty
_PyPegen_concatenate_strings(Parser *p, asdl_expr_seq *strings,
                             صحيح lineno, صحيح col_offset, صحيح end_lineno,
                             صحيح end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    أكد(len > 0);

    صحيح f_string_found = 0;
    صحيح unicode_string_found = 0;
    صحيح bytes_found = 0;

    Py_ssize_t i = 0;
    لكل (i = 0; i < len; i++) {
        expr_ty elem = asdl_seq_GET(strings, i);
        بدل(elem->kind) {
            حالة Constant_kind:
                ܐܢ (PyBytes_CheckExact(elem->v.Constant.value)) {
                    bytes_found = 1;
                } ܐܚܪܢܐ {
                    unicode_string_found = 1;
                }
                اكسر;
            حالة JoinedStr_kind:
                f_string_found = 1;
                اكسر;
            حالة TemplateStr_kind:
                // python.gram handles this; we should never get here
                أكد(0);
                اكسر;
            افتراضي:
                f_string_found = 1;
                اكسر;
        }
    }

    // Cannot mix unicode and bytes
    ܐܢ ((unicode_string_found || f_string_found) && bytes_found) {
        RAISE_SYNTAX_ERROR("cannot mix bytes and nonbytes literals");
        ܦܢܐ عدم;
    }

    // If it's only bytes or only unicode string, do a simple concat
    ܐܢ (!f_string_found) {
        ܐܢ (len == 1) {
            ܦܢܐ asdl_seq_GET(strings, 0);
        }
        ܐܚܪܢܐ ܐܢ (bytes_found) {
            ܦܢܐ _build_concatenated_bytes(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
        ܐܚܪܢܐ {
            ܦܢܐ _build_concatenated_unicode(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
    }

    ܦܢܐ _build_concatenated_joined_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
}

ساكن صحيح
_warn_relative_import_of_lazy(Parser *p, asdl_seq *dots, expr_ty module)
{
    // Warn about `from . lazy import x`: the whitespace between the dots and
    // the module name is insignificant, so this is parsed exactly like
    // `from .lazy import x` (an import of the relative module "lazy"), but it
    // is most likely a transposition of `lazy from . import x` (PEP 810).
    ܐܢ (p->call_invalid_rules) {
        ܦܢܐ 0;
    }

    // Only fire if there is whitespace between the last dot and the name,
    // i.e. not for the common `from .lazy import x` spelling.
    Token *last_dot = asdl_seq_GET_UNTYPED(dots, asdl_seq_LEN(dots) - 1);
    ܐܢ (
        last_dot->end_lineno == module->lineno
        && last_dot->end_col_offset == module->col_offset
    ) {
        ܦܢܐ 0;
    }

    صحيح count = _PyPegen_seq_count_dots(dots);
    حرف *buf = PyMem_RawMalloc(count + 1);
    ܐܢ (buf == عدم) {
        PyErr_NoMemory();
        ܦܢܐ -1;
    }
    memset(buf, '.', count);
    buf[count] = '\0';

    PyObject *msg = PyUnicode_FromFormat(
        "'from %s lazy import' is the same as 'from %slazy import'; "
        "did you mean 'lazy from %s import'?",
        buf, buf, buf);
    PyMem_RawFree(buf);
    ܐܢ (msg == عدم) {
        ܦܢܐ -1;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    صحيح res = _PyErr_EmitSyntaxWarning(msg,
                                       info.filename,
                                       module->lineno,
                                       module->col_offset + 1,
                                       module->end_lineno,
                                       module->end_col_offset + 1,
                                       info.module);
    Py_DECREF(msg);
    ܦܢܐ res;
}

stmt_ty
_PyPegen_checked_from_import(Parser *p, asdl_seq *dots, expr_ty module_name,
                             asdl_alias_seq *names, expr_ty lazy_token, صحيح lineno,
                             صحيح col_offset, صحيح end_lineno, صحيح end_col_offset,
                             PyArena *arena)
{
    identifier module = module_name->v.Name.id;
    صحيح level = _PyPegen_seq_count_dots(dots);
    ܐܢ (level == 0 && PyUnicode_CompareWithASCIIString(module, "__future__") == 0) {
        ܐܢ (lazy_token) {
            RAISE_SYNTAX_ERROR_KNOWN_LOCATION(lazy_token,
                "lazy from __future__ import is not allowed");
            ܦܢܐ عدم;
        }
        لكل (Py_ssize_t i = 0; i < asdl_seq_LEN(names); i++) {
            alias_ty alias = asdl_seq_GET(names, i);
            ܐܢ (PyUnicode_CompareWithASCIIString(alias->name, "barry_as_FLUFL") == 0) {
                p->flags |= PyPARSE_BARRY_AS_BDFL;
                p->tok->barry_as_bdfl = 1;
            }
        }
    }
    ܐܚܪܢܐ ܐܢ (
        level > 0
        && lazy_token == عدم
        && PyUnicode_CompareWithASCIIString(module, "lazy") == 0
    ) {
        ܐܢ (_warn_relative_import_of_lazy(p, dots, module_name) < 0) {
            ܦܢܐ عدم;
        }
    }
    ܦܢܐ _PyAST_ImportFrom(module, names, level, lazy_token ? 1 : 0, lineno,
                             col_offset, end_lineno, end_col_offset, arena);
}

asdl_stmt_seq*
_PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts) {
    ܐܢ (!p->call_invalid_rules) {
        ܦܢܐ stmts;
    }
    Py_ssize_t len = asdl_seq_LEN(stmts);
    ܐܢ (len == 0) {
        ܦܢܐ stmts;
    }
    stmt_ty last_stmt = asdl_seq_GET(stmts, len - 1);
    ܐܢ (p->last_stmt_location.lineno > last_stmt->lineno) {
        ܦܢܐ stmts;
    }
    p->last_stmt_location.lineno = last_stmt->lineno;
    p->last_stmt_location.col_offset = last_stmt->col_offset;
    p->last_stmt_location.end_lineno = last_stmt->end_lineno;
    p->last_stmt_location.end_col_offset = last_stmt->end_col_offset;
    ܦܢܐ stmts;
}
