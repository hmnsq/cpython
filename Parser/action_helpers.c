#ಅನ್ತರ್ಭಾವ <Python.h>
#ಅನ್ತರ್ಭಾವ "pycore_pystate.h"         // _PyInterpreterState_GET()
#ಅನ್ತರ್ಭಾವ "pycore_runtime.h"         // _PyRuntime
#ಅನ್ತರ್ಭಾವ "pycore_unicodeobject.h"   // _PyUnicode_InternImmortal()
#ಅನ್ತರ್ಭಾವ "pycore_pyerrors.h"        // _PyErr_EmitSyntaxWarning()

#ಅನ್ತರ್ಭಾವ "pegen.h"
#ಅನ್ತರ್ಭಾವ "string_parser.h"          // _PyPegen_decode_string()
#ಅನ್ತರ್ಭಾವ "lexer/state.h"            // tok_state


ಶೂನ್ಯ *
_PyPegen_dummy_name(Parser *p, ...)
{
    ಪ್ರತ್ಯಾಗಮ &_PyRuntime.parser.dummy_name;
}

/* Creates a single-element asdl_seq* that contains a */
asdl_seq *
_PyPegen_singleton_seq(Parser *p, ಶೂನ್ಯ *a)
{
    ಪ್ರತಿಜ್ಞಾ(a != ರಿಕ್ತ);
    asdl_seq *seq = (asdl_seq*)_Py_asdl_generic_seq_new(1, p->arena);
    ಯದಿ (!seq) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    asdl_seq_SET_UNTYPED(seq, 0, a);
    ಪ್ರತ್ಯಾಗಮ seq;
}

/* Creates a copy of seq and prepends a to it */
asdl_seq *
Py_Pegen_seq_ಸೇರಿಸು_in_front(Parser *p, ಶೂನ್ಯ *a, asdl_seq *seq)
{
    ಪ್ರತಿಜ್ಞಾ(a != ರಿಕ್ತ);
    ಯದಿ (!seq) {
        ಪ್ರತ್ಯಾಗಮ _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    ಯದಿ (!new_seq) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    asdl_seq_SET_UNTYPED(new_seq, 0, a);
    ಪ್ರತಿ (Py_ssize_t i = 1, l = asdl_seq_LEN(new_seq); i < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i - 1));
    }
    ಪ್ರತ್ಯಾಗಮ new_seq;
}

/* Creates a copy of seq and appends a to it */
asdl_seq *
_PyPegen_seq_append_to_end(Parser *p, asdl_seq *seq, ಶೂನ್ಯ *a)
{
    ಪ್ರತಿಜ್ಞಾ(a != ರಿಕ್ತ);
    ಯದಿ (!seq) {
        ಪ್ರತ್ಯಾಗಮ _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    ಯದಿ (!new_seq) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತಿ (Py_ssize_t i = 0, l = asdl_seq_LEN(new_seq); i + 1 < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i));
    }
    asdl_seq_SET_UNTYPED(new_seq, asdl_seq_LEN(new_seq) - 1, a);
    ಪ್ರತ್ಯಾಗಮ new_seq;
}

ಸ್ಥಿರ Py_ssize_t
_get_flattened_seq_size(asdl_seq *seqs)
{
    Py_ssize_t size = 0;
    ಪ್ರತಿ (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        size += asdl_seq_LEN(inner_seq);
    }
    ಪ್ರತ್ಯಾಗಮ size;
}

/* Flattens an asdl_seq* of asdl_seq*s */
asdl_seq *
_PyPegen_seq_flatten(Parser *p, asdl_seq *seqs)
{
    Py_ssize_t flattened_seq_size = _get_flattened_seq_size(seqs);
    ಪ್ರತಿಜ್ಞಾ(flattened_seq_size > 0);

    asdl_seq *flattened_seq = (asdl_seq*)_Py_asdl_generic_seq_new(flattened_seq_size, p->arena);
    ಯದಿ (!flattened_seq) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪೂರ್ಣಾಙ್ಕ flattened_seq_idx = 0;
    ಪ್ರತಿ (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        ಪ್ರತಿ (Py_ssize_t j = 0, li = asdl_seq_LEN(inner_seq); j < li; j++) {
            asdl_seq_SET_UNTYPED(flattened_seq, flattened_seq_idx++, asdl_seq_GET_UNTYPED(inner_seq, j));
        }
    }
    ಪ್ರತಿಜ್ಞಾ(flattened_seq_idx == flattened_seq_size);

    ಪ್ರತ್ಯಾಗಮ flattened_seq;
}

ಶೂನ್ಯ *
_PyPegen_seq_last_item(asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ಪ್ರತ್ಯಾಗಮ asdl_seq_GET_UNTYPED(seq, len - 1);
}

ಶೂನ್ಯ *
_PyPegen_seq_first_item(asdl_seq *seq)
{
    ಪ್ರತ್ಯಾಗಮ asdl_seq_GET_UNTYPED(seq, 0);
}

/* Creates a new name of the form <first_name>.<second_name> */
ಅಭಿವ್ಯಕ್ತಿ_ty
_PyPegen_join_names_with_dot(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty first_name, ಅಭಿವ್ಯಕ್ತಿ_ty second_name)
{
    ಪ್ರತಿಜ್ಞಾ(first_name != ರಿಕ್ತ && second_name != ರಿಕ್ತ);
    PyObject *uni = PyUnicode_FromFormat("%U.%U",
            first_name->v.Name.id, second_name->v.Name.id);
    ಯದಿ (!uni) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &uni);
    ಯದಿ (_PyArena_AddPyObject(p->arena, uni) < 0) {
        Py_DECREF(uni);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ _PyAST_Name(uni, Load, EXTRA_ಅಭಿವ್ಯಕ್ತಿ(first_name, second_name));
}

/* Counts the total number of dots in seq's tokens */
ಪೂರ್ಣಾಙ್ಕ
Py_Pegen_seq_ಎಣಿಕೆ_dots(asdl_seq *seq)
{
    ಪೂರ್ಣಾಙ್ಕ ಸಂಖ್ಯೆ_of_dots = 0;
    ಪ್ರತಿ (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        Token *current_ಅಭಿವ್ಯಕ್ತಿ = asdl_seq_GET_UNTYPED(seq, i);
        ವಿಕಲ್ಪ (current_ಅಭಿವ್ಯಕ್ತಿ->type) {
            ಪಕ್ಷ ELLIPSIS:
                ಸಂಖ್ಯೆ_of_dots += 3;
                ಭಙ್ಗ;
            ಪಕ್ಷ DOT:
                ಸಂಖ್ಯೆ_of_dots += 1;
                ಭಙ್ಗ;
            ಸ್ವಭಾವ:
                Py_UNREACHABLE();
        }
    }

    ಪ್ರತ್ಯಾಗಮ ಸಂಖ್ಯೆ_of_dots;
}

/* Creates an alias with '*' as the identifier name */
alias_ty
_PyPegen_alias_for_star(Parser *p, ಪೂರ್ಣಾಙ್ಕ lineno, ಪೂರ್ಣಾಙ್ಕ col_offset, ಪೂರ್ಣಾಙ್ಕ end_lineno,
                        ಪೂರ್ಣಾಙ್ಕ end_col_offset, PyArena *arena) {
    PyObject *str = PyUnicode_InternFromString("*");
    ಯದಿ (!str) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ _PyAST_alias(str, ರಿಕ್ತ, lineno, col_offset, end_lineno, end_col_offset, arena);
}

/* Creates a new asdl_seq* with the identifiers of all the names in seq */
asdl_identifier_seq *
_PyPegen_map_names_to_ids(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ಪ್ರತಿಜ್ಞಾ(len > 0);

    asdl_identifier_seq *new_seq = _Py_asdl_identifier_seq_new(len, p->arena);
    ಯದಿ (!new_seq) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತಿ (Py_ssize_t i = 0; i < len; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty e = asdl_seq_GET(seq, i);
        asdl_seq_SET(new_seq, i, e->v.Name.id);
    }
    ಪ್ರತ್ಯಾಗಮ new_seq;
}

/* Constructs a CmpopExprPair */
Cmpop_ಅಭಿವ್ಯಕ್ತಿ_Pair *
Py_Pegen_cmpop_ಅಭಿವ್ಯಕ್ತಿ_pair(Parser *p, cmpop_ty cmpop, ಅಭಿವ್ಯಕ್ತಿ_ty ಅಭಿವ್ಯಕ್ತಿ)
{
    ಪ್ರತಿಜ್ಞಾ(ಅಭಿವ್ಯಕ್ತಿ != ರಿಕ್ತ);
    Cmpop_ಅಭಿವ್ಯಕ್ತಿ_Pair *a = _PyArena_Malloc(p->arena, ಪರಿಮಾಣ(Cmpop_ಅಭಿವ್ಯಕ್ತಿ_Pair));
    ಯದಿ (!a) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    a->cmpop = cmpop;
    a->ಅಭಿವ್ಯಕ್ತಿ = ಅಭಿವ್ಯಕ್ತಿ;
    ಪ್ರತ್ಯಾಗಮ a;
}

asdl_int_seq *
_PyPegen_get_cmpops(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ಪ್ರತಿಜ್ಞಾ(len > 0);

    asdl_int_seq *new_seq = _Py_asdl_int_seq_new(len, p->arena);
    ಯದಿ (!new_seq) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತಿ (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_ಅಭಿವ್ಯಕ್ತಿ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->cmpop);
    }
    ಪ್ರತ್ಯಾಗಮ new_seq;
}

asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *
_PyPegen_get_exprs(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ಪ್ರತಿಜ್ಞಾ(len > 0);

    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *new_seq = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(len, p->arena);
    ಯದಿ (!new_seq) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತಿ (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_ಅಭಿವ್ಯಕ್ತಿ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->ಅಭಿವ್ಯಕ್ತಿ);
    }
    ಪ್ರತ್ಯಾಗಮ new_seq;
}

/* Creates an asdl_seq* where all the elements have been changed to have ctx as context */
ಸ್ಥಿರ asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *
_set_seq_context(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *seq, ಅಭಿವ್ಯಕ್ತಿ_context_ty ctx)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ಯದಿ (len == 0) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *new_seq = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(len, p->arena);
    ಯದಿ (!new_seq) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತಿ (Py_ssize_t i = 0; i < len; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty e = asdl_seq_GET(seq, i);
        ಅಭಿವ್ಯಕ್ತಿ_ty new_e = Py_Pegen_set_ಅಭಿವ್ಯಕ್ತಿ_context(p, e, ctx);
        ಯದಿ (!new_e) {
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
        asdl_seq_SET(new_seq, i, new_e);
    }
    ಪ್ರತ್ಯಾಗಮ new_seq;
}

ಸ್ಥಿರ ಅಭಿವ್ಯಕ್ತಿ_ty
_set_name_context(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty e, ಅಭಿವ್ಯಕ್ತಿ_context_ty ctx)
{
    ಪ್ರತ್ಯಾಗಮ _PyAST_Name(e->v.Name.id, ctx, EXTRA_ಅಭಿವ್ಯಕ್ತಿ(e, e));
}

ಸ್ಥಿರ ಅಭಿವ್ಯಕ್ತಿ_ty
_set_tuple_context(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty e, ಅಭಿವ್ಯಕ್ತಿ_context_ty ctx)
{
    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *seq = _set_seq_context(p, e->v.Tuple.elts, ctx);
    ಯದಿ (!seq && PyErr_Occurred()) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ _PyAST_Tuple(seq, ctx, EXTRA_ಅಭಿವ್ಯಕ್ತಿ(e, e));
}

ಸ್ಥಿರ ಅಭಿವ್ಯಕ್ತಿ_ty
_set_list_context(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty e, ಅಭಿವ್ಯಕ್ತಿ_context_ty ctx)
{
    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *seq = _set_seq_context(p, e->v.List.elts, ctx);
    ಯದಿ (!seq && PyErr_Occurred()) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ _PyAST_List(seq, ctx, EXTRA_ಅಭಿವ್ಯಕ್ತಿ(e, e));
}

ಸ್ಥಿರ ಅಭಿವ್ಯಕ್ತಿ_ty
_set_subscript_context(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty e, ಅಭಿವ್ಯಕ್ತಿ_context_ty ctx)
{
    ಪ್ರತ್ಯಾಗಮ _PyAST_Subscript(e->v.Subscript.ಮೌಲ್ಯ, e->v.Subscript.slice,
                            ctx, EXTRA_ಅಭಿವ್ಯಕ್ತಿ(e, e));
}

ಸ್ಥಿರ ಅಭಿವ್ಯಕ್ತಿ_ty
_set_attribute_context(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty e, ಅಭಿವ್ಯಕ್ತಿ_context_ty ctx)
{
    ಪ್ರತ್ಯಾಗಮ _PyAST_Attribute(e->v.Attribute.ಮೌಲ್ಯ, e->v.Attribute.attr,
                            ctx, EXTRA_ಅಭಿವ್ಯಕ್ತಿ(e, e));
}

ಸ್ಥಿರ ಅಭಿವ್ಯಕ್ತಿ_ty
_set_starred_context(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty e, ಅಭಿವ್ಯಕ್ತಿ_context_ty ctx)
{
    ಅಭಿವ್ಯಕ್ತಿ_ty inner = Py_Pegen_set_ಅಭಿವ್ಯಕ್ತಿ_context(p, e->v.Starred.ಮೌಲ್ಯ, ctx);
    ಯದಿ (!inner) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ _PyAST_Starred(inner, ctx, EXTRA_ಅಭಿವ್ಯಕ್ತಿ(e, e));
}

/* Creates an `expr_ty` equivalent to `expr` but with `ctx` as context */
ಅಭಿವ್ಯಕ್ತಿ_ty
Py_Pegen_set_ಅಭಿವ್ಯಕ್ತಿ_context(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty ಅಭಿವ್ಯಕ್ತಿ, ಅಭಿವ್ಯಕ್ತಿ_context_ty ctx)
{
    ಪ್ರತಿಜ್ಞಾ(ಅಭಿವ್ಯಕ್ತಿ != ರಿಕ್ತ);

    ಅಭಿವ್ಯಕ್ತಿ_ty new = ರಿಕ್ತ;
    ವಿಕಲ್ಪ (ಅಭಿವ್ಯಕ್ತಿ->kind) {
        ಪಕ್ಷ Name_kind:
            new = _set_name_context(p, ಅಭಿವ್ಯಕ್ತಿ, ctx);
            ಭಙ್ಗ;
        ಪಕ್ಷ Tuple_kind:
            new = _set_tuple_context(p, ಅಭಿವ್ಯಕ್ತಿ, ctx);
            ಭಙ್ಗ;
        ಪಕ್ಷ List_kind:
            new = _set_list_context(p, ಅಭಿವ್ಯಕ್ತಿ, ctx);
            ಭಙ್ಗ;
        ಪಕ್ಷ Subscript_kind:
            new = _set_subscript_context(p, ಅಭಿವ್ಯಕ್ತಿ, ctx);
            ಭಙ್ಗ;
        ಪಕ್ಷ Attribute_kind:
            new = _set_attribute_context(p, ಅಭಿವ್ಯಕ್ತಿ, ctx);
            ಭಙ್ಗ;
        ಪಕ್ಷ Starred_kind:
            new = _set_starred_context(p, ಅಭಿವ್ಯಕ್ತಿ, ctx);
            ಭಙ್ಗ;
        ಸ್ವಭಾವ:
            new = ಅಭಿವ್ಯಕ್ತಿ;
    }
    ಪ್ರತ್ಯಾಗಮ new;
}

/* Invalid targets return NULL without raising, allowing parsing to backtrack.
   Copy valid targets to preserve the memoized expression's context. */
ಅಭಿವ್ಯಕ್ತಿ_ty
_PyPegen_make_target(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty ಅಭಿವ್ಯಕ್ತಿ, TARGETS_TYPE targets_type)
{
    ಪ್ರತಿಜ್ಞಾ(ಅಭಿವ್ಯಕ್ತಿ != ರಿಕ್ತ);
    ಪ್ರತಿಜ್ಞಾ(targets_type != FOR_TARGETS);
    ಯದಿ (_PyPegen_get_invalid_target(ಅಭಿವ್ಯಕ್ತಿ, targets_type) != ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ Py_Pegen_set_ಅಭಿವ್ಯಕ್ತಿ_context(
        p, ಅಭಿವ್ಯಕ್ತಿ, targets_type == DEL_TARGETS ? Del : Store);
}

/* Constructs a KeyValuePair that is used when parsing a dict's key value pairs */
ಕೀಲಿ_ಮೌಲ್ಯ_Pair *
Py_Pegen_ಕೀಲಿ_ಮೌಲ್ಯ_pair(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty ಕೀಲಿ, ಅಭಿವ್ಯಕ್ತಿ_ty ಮೌಲ್ಯ)
{
    ಕೀಲಿ_ಮೌಲ್ಯ_Pair *a = _PyArena_Malloc(p->arena, ಪರಿಮಾಣ(ಕೀಲಿ_ಮೌಲ್ಯ_Pair));
    ಯದಿ (!a) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    a->ಕೀಲಿ = ಕೀಲಿ;
    a->ಮೌಲ್ಯ = ಮೌಲ್ಯ;
    ಪ್ರತ್ಯಾಗಮ a;
}

/* Extracts all keys from an asdl_seq* of KeyValuePair*'s */
asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *
Py_Pegen_get_ಕೀಲಿಗಳು(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *new_seq = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(len, p->arena);
    ಯದಿ (!new_seq) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತಿ (Py_ssize_t i = 0; i < len; i++) {
        ಕೀಲಿ_ಮೌಲ್ಯ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->ಕೀಲಿ);
    }
    ಪ್ರತ್ಯಾಗಮ new_seq;
}

/* Extracts all values from an asdl_seq* of KeyValuePair*'s */
asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *
Py_Pegen_get_ಮೌಲ್ಯಗಳು(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *new_seq = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(len, p->arena);
    ಯದಿ (!new_seq) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತಿ (Py_ssize_t i = 0; i < len; i++) {
        ಕೀಲಿ_ಮೌಲ್ಯ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->ಮೌಲ್ಯ);
    }
    ಪ್ರತ್ಯಾಗಮ new_seq;
}

/* Constructs a KeyPatternPair that is used when parsing mapping & class patterns */
ಕೀಲಿ_Pattern_Pair *
Py_Pegen_ಕೀಲಿ_pattern_pair(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty ಕೀಲಿ, pattern_ty pattern)
{
    ಕೀಲಿ_Pattern_Pair *a = _PyArena_Malloc(p->arena, ಪರಿಮಾಣ(ಕೀಲಿ_Pattern_Pair));
    ಯದಿ (!a) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    a->ಕೀಲಿ = ಕೀಲಿ;
    a->pattern = pattern;
    ಪ್ರತ್ಯಾಗಮ a;
}

/* Extracts all keys from an asdl_seq* of KeyPatternPair*'s */
asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *
Py_Pegen_get_pattern_ಕೀಲಿಗಳು(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *new_seq = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(len, p->arena);
    ಯದಿ (!new_seq) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತಿ (Py_ssize_t i = 0; i < len; i++) {
        ಕೀಲಿ_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->ಕೀಲಿ);
    }
    ಪ್ರತ್ಯಾಗಮ new_seq;
}

/* Extracts all patterns from an asdl_seq* of KeyPatternPair*'s */
asdl_pattern_seq *
_PyPegen_get_patterns(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_pattern_seq *new_seq = _Py_asdl_pattern_seq_new(len, p->arena);
    ಯದಿ (!new_seq) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತಿ (Py_ssize_t i = 0; i < len; i++) {
        ಕೀಲಿ_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->pattern);
    }
    ಪ್ರತ್ಯಾಗಮ new_seq;
}

/* Constructs a NameDefaultPair */
NameDefaultPair *
_PyPegen_name_default_pair(Parser *p, arg_ty arg, ಅಭಿವ್ಯಕ್ತಿ_ty ಮೌಲ್ಯ, Token *tc)
{
    NameDefaultPair *a = _PyArena_Malloc(p->arena, ಪರಿಮಾಣ(NameDefaultPair));
    ಯದಿ (!a) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    a->arg = _PyPegen_add_type_comment_to_arg(p, arg, tc);
    ಯದಿ (!a->arg) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    a->ಮೌಲ್ಯ = ಮೌಲ್ಯ;
    ಪ್ರತ್ಯಾಗಮ a;
}

/* Constructs a SlashWithDefault */
SlashWithDefault *
_PyPegen_slash_with_default(Parser *p, asdl_arg_seq *plain_names, asdl_seq *names_with_defaults)
{
    SlashWithDefault *a = _PyArena_Malloc(p->arena, ಪರಿಮಾಣ(SlashWithDefault));
    ಯದಿ (!a) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    a->plain_names = plain_names;
    a->names_with_defaults = names_with_defaults;
    ಪ್ರತ್ಯಾಗಮ a;
}

/* Constructs a StarEtc */
StarEtc *
_PyPegen_star_etc(Parser *p, arg_ty vararg, asdl_seq *kwonlyargs, arg_ty kwarg)
{
    StarEtc *a = _PyArena_Malloc(p->arena, ಪರಿಮಾಣ(StarEtc));
    ಯದಿ (!a) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    a->vararg = vararg;
    a->kwonlyargs = kwonlyargs;
    a->kwarg = kwarg;
    ಪ್ರತ್ಯಾಗಮ a;
}

asdl_seq *
_PyPegen_join_sequences(Parser *p, asdl_seq *a, asdl_seq *b)
{
    Py_ssize_t first_len = asdl_seq_LEN(a);
    Py_ssize_t second_len = asdl_seq_LEN(b);
    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(first_len + second_len, p->arena);
    ಯದಿ (!new_seq) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪೂರ್ಣಾಙ್ಕ k = 0;
    ಪ್ರತಿ (Py_ssize_t i = 0; i < first_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(a, i));
    }
    ಪ್ರತಿ (Py_ssize_t i = 0; i < second_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(b, i));
    }

    ಪ್ರತ್ಯಾಗಮ new_seq;
}

ಸ್ಥಿರ asdl_arg_seq*
_get_names(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_arg_seq *seq = _Py_asdl_arg_seq_new(len, p->arena);
    ಯದಿ (!seq) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತಿ (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->arg);
    }
    ಪ್ರತ್ಯಾಗಮ seq;
}

ಸ್ಥಿರ asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *
_get_defaults(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *seq = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(len, p->arena);
    ಯದಿ (!seq) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತಿ (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->ಮೌಲ್ಯ);
    }
    ಪ್ರತ್ಯಾಗಮ seq;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
_make_posonlyargs(Parser *p,
                  asdl_arg_seq *slash_without_default,
                  SlashWithDefault *slash_with_default,
                  asdl_arg_seq **posonlyargs) {
    ಯದಿ (slash_without_default != ರಿಕ್ತ) {
        *posonlyargs = slash_without_default;
    }
    ಅನ್ಯಥಾ ಯದಿ (slash_with_default != ರಿಕ್ತ) {
        asdl_arg_seq *slash_with_default_names =
                _get_names(p, slash_with_default->names_with_defaults);
        ಯದಿ (!slash_with_default_names) {
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        *posonlyargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default->plain_names,
                (asdl_seq*)slash_with_default_names);
    }
    ಅನ್ಯಥಾ {
        *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }
    ಪ್ರತ್ಯಾಗಮ *posonlyargs == ರಿಕ್ತ ? -1 : 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
_make_posargs(Parser *p,
              asdl_arg_seq *plain_names,
              asdl_seq *names_with_default,
              asdl_arg_seq **posargs) {

    ಯದಿ (names_with_default != ರಿಕ್ತ) {
        ಯದಿ (plain_names != ರಿಕ್ತ) {
            asdl_arg_seq *names_with_default_names = _get_names(p, names_with_default);
            ಯದಿ (!names_with_default_names) {
                ಪ್ರತ್ಯಾಗಮ -1;
            }
            *posargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                    p,(asdl_seq*)plain_names, (asdl_seq*)names_with_default_names);
        }
        ಅನ್ಯಥಾ {
            *posargs = _get_names(p, names_with_default);
        }
    }
    ಅನ್ಯಥಾ {
        ಯದಿ (plain_names != ರಿಕ್ತ) {
            // With the current grammar, we never get here.
            // If that has changed, remove the assert, and test thoroughly.
            ಪ್ರತಿಜ್ಞಾ(0);
            *posargs = plain_names;
        }
        ಅನ್ಯಥಾ {
            *posargs = _Py_asdl_arg_seq_new(0, p->arena);
        }
    }
    ಪ್ರತ್ಯಾಗಮ *posargs == ರಿಕ್ತ ? -1 : 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
_make_posdefaults(Parser *p,
                  SlashWithDefault *slash_with_default,
                  asdl_seq *names_with_default,
                  asdl_ಅಭಿವ್ಯಕ್ತಿ_seq **posdefaults) {
    ಯದಿ (slash_with_default != ರಿಕ್ತ && names_with_default != ರಿಕ್ತ) {
        asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *slash_with_default_ಮೌಲ್ಯಗಳು =
                _get_defaults(p, slash_with_default->names_with_defaults);
        ಯದಿ (!slash_with_default_ಮೌಲ್ಯಗಳು) {
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *names_with_default_ಮೌಲ್ಯಗಳು = _get_defaults(p, names_with_default);
        ಯದಿ (!names_with_default_ಮೌಲ್ಯಗಳು) {
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        *posdefaults = (asdl_ಅಭಿವ್ಯಕ್ತಿ_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default_ಮೌಲ್ಯಗಳು,
                (asdl_seq*)names_with_default_ಮೌಲ್ಯಗಳು);
    }
    ಅನ್ಯಥಾ ಯದಿ (slash_with_default == ರಿಕ್ತ && names_with_default != ರಿಕ್ತ) {
        *posdefaults = _get_defaults(p, names_with_default);
    }
    ಅನ್ಯಥಾ ಯದಿ (slash_with_default != ರಿಕ್ತ && names_with_default == ರಿಕ್ತ) {
        *posdefaults = _get_defaults(p, slash_with_default->names_with_defaults);
    }
    ಅನ್ಯಥಾ {
        *posdefaults = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(0, p->arena);
    }
    ಪ್ರತ್ಯಾಗಮ *posdefaults == ರಿಕ್ತ ? -1 : 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
_make_kwargs(Parser *p, StarEtc *star_etc,
             asdl_arg_seq **kwonlyargs,
             asdl_ಅಭಿವ್ಯಕ್ತಿ_seq **kwdefaults) {
    ಯದಿ (star_etc != ರಿಕ್ತ && star_etc->kwonlyargs != ರಿಕ್ತ) {
        *kwonlyargs = _get_names(p, star_etc->kwonlyargs);
    }
    ಅನ್ಯಥಾ {
        *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }

    ಯದಿ (*kwonlyargs == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಯದಿ (star_etc != ರಿಕ್ತ && star_etc->kwonlyargs != ರಿಕ್ತ) {
        *kwdefaults = _get_defaults(p, star_etc->kwonlyargs);
    }
    ಅನ್ಯಥಾ {
        *kwdefaults = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(0, p->arena);
    }

    ಯದಿ (*kwdefaults == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಪ್ರತ್ಯಾಗಮ 0;
}

/* Constructs an arguments_ty object out of all the parsed constructs in the parameters rule */
arguments_ty
_PyPegen_make_arguments(Parser *p, asdl_arg_seq *slash_without_default,
                        SlashWithDefault *slash_with_default, asdl_arg_seq *plain_names,
                        asdl_seq *names_with_default, StarEtc *star_etc)
{
    asdl_arg_seq *posonlyargs;
    ಯದಿ (_make_posonlyargs(p, slash_without_default, slash_with_default, &posonlyargs) == -1) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    asdl_arg_seq *posargs;
    ಯದಿ (_make_posargs(p, plain_names, names_with_default, &posargs) == -1) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *posdefaults;
    ಯದಿ (_make_posdefaults(p,slash_with_default, names_with_default, &posdefaults) == -1) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    arg_ty vararg = ರಿಕ್ತ;
    ಯದಿ (star_etc != ರಿಕ್ತ && star_etc->vararg != ರಿಕ್ತ) {
        vararg = star_etc->vararg;
    }

    asdl_arg_seq *kwonlyargs;
    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *kwdefaults;
    ಯದಿ (_make_kwargs(p, star_etc, &kwonlyargs, &kwdefaults) == -1) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    arg_ty kwarg = ರಿಕ್ತ;
    ಯದಿ (star_etc != ರಿಕ್ತ && star_etc->kwarg != ರಿಕ್ತ) {
        kwarg = star_etc->kwarg;
    }

    ಪ್ರತ್ಯಾಗಮ _PyAST_arguments(posonlyargs, posargs, vararg, kwonlyargs,
                            kwdefaults, kwarg, posdefaults, p->arena);
}


/* Constructs an empty arguments_ty object, that gets used when a function accepts no
 * arguments. */
arguments_ty
_PyPegen_empty_arguments(Parser *p)
{
    asdl_arg_seq *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    ಯದಿ (!posonlyargs) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    asdl_arg_seq *posargs = _Py_asdl_arg_seq_new(0, p->arena);
    ಯದಿ (!posargs) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *posdefaults = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(0, p->arena);
    ಯದಿ (!posdefaults) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    asdl_arg_seq *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    ಯದಿ (!kwonlyargs) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *kwdefaults = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(0, p->arena);
    ಯದಿ (!kwdefaults) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ _PyAST_arguments(posonlyargs, posargs, ರಿಕ್ತ, kwonlyargs,
                            kwdefaults, ರಿಕ್ತ, posdefaults, p->arena);
}

/* Encapsulates the value of an operator_ty into an AugOperator struct */
AugOperator *
_PyPegen_augoperator(Parser *p, operator_ty kind)
{
    AugOperator *a = _PyArena_Malloc(p->arena, ಪರಿಮಾಣ(AugOperator));
    ಯದಿ (!a) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    a->kind = kind;
    ಪ್ರತ್ಯಾಗಮ a;
}

/* Construct a FunctionDef equivalent to function_def, but with decorators */
stmt_ty
_PyPegen_function_def_decorators(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *decorators, stmt_ty function_def)
{
    ಪ್ರತಿಜ್ಞಾ(function_def != ರಿಕ್ತ);
    ಯದಿ (function_def->kind == AsyncFunctionDef_kind) {
        ಪ್ರತ್ಯಾಗಮ _PyAST_AsyncFunctionDef(
            function_def->v.AsyncFunctionDef.name,
            function_def->v.AsyncFunctionDef.args,
            function_def->v.AsyncFunctionDef.body, decorators,
            function_def->v.AsyncFunctionDef.returns,
            function_def->v.AsyncFunctionDef.type_comment,
            function_def->v.AsyncFunctionDef.type_params,
            function_def->lineno, function_def->col_offset,
            function_def->end_lineno, function_def->end_col_offset, p->arena);
    }

    ಪ್ರತ್ಯಾಗಮ _PyAST_FunctionDef(
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
_PyPegen_class_def_decorators(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *decorators, stmt_ty class_def)
{
    ಪ್ರತಿಜ್ಞಾ(class_def != ರಿಕ್ತ);
    ಪ್ರತ್ಯಾಗಮ _PyAST_ClassDef(
        class_def->v.ClassDef.name,
        class_def->v.ClassDef.bases, class_def->v.ClassDef.keywords,
        class_def->v.ClassDef.body, decorators,
        class_def->v.ClassDef.type_params,
        class_def->lineno, class_def->col_offset, class_def->end_lineno,
        class_def->end_col_offset, p->arena);
}

/* Construct a KeywordOrStarred */
KeywordOrStarred *
_PyPegen_keyword_or_starred(Parser *p, ಶೂನ್ಯ *element, ಪೂರ್ಣಾಙ್ಕ is_keyword)
{
    KeywordOrStarred *a = _PyArena_Malloc(p->arena, ಪರಿಮಾಣ(KeywordOrStarred));
    ಯದಿ (!a) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    a->element = element;
    a->is_keyword = is_keyword;
    ಪ್ರತ್ಯಾಗಮ a;
}

/* Get the number of starred expressions in an asdl_seq* of KeywordOrStarred*s */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
seq_ಸಂಖ್ಯೆ_of_starred_exprs(asdl_seq *seq)
{
    ಪೂರ್ಣಾಙ್ಕ n = 0;
    ಪ್ರತಿ (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(seq, i);
        ಯದಿ (!k->is_keyword) {
            n++;
        }
    }
    ಪ್ರತ್ಯಾಗಮ n;
}

/* Extract the starred expressions of an asdl_seq* of KeywordOrStarred*s */
asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *
_PyPegen_seq_extract_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    ಪೂರ್ಣಾಙ್ಕ new_len = seq_ಸಂಖ್ಯೆ_of_starred_exprs(kwargs);
    ಯದಿ (new_len == 0) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *new_seq = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(new_len, p->arena);
    ಯದಿ (!new_seq) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪೂರ್ಣಾಙ್ಕ idx = 0;
    ಪ್ರತಿ (Py_ssize_t i = 0, len = asdl_seq_LEN(kwargs); i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        ಯದಿ (!k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    ಪ್ರತ್ಯಾಗಮ new_seq;
}

/* Return a new asdl_seq* with only the keywords in kwargs */
asdl_keyword_seq*
_PyPegen_seq_delete_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    Py_ssize_t len = asdl_seq_LEN(kwargs);
    Py_ssize_t new_len = len - seq_ಸಂಖ್ಯೆ_of_starred_exprs(kwargs);
    ಯದಿ (new_len == 0) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    asdl_keyword_seq *new_seq = _Py_asdl_keyword_seq_new(new_len, p->arena);
    ಯದಿ (!new_seq) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪೂರ್ಣಾಙ್ಕ idx = 0;
    ಪ್ರತಿ (Py_ssize_t i = 0; i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        ಯದಿ (k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    ಪ್ರತ್ಯಾಗಮ new_seq;
}

ಅಭಿವ್ಯಕ್ತಿ_ty
_PyPegen_ensure_imaginary(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty exp)
{
    ಯದಿ (exp->kind != Constant_kind || !PyComplex_CheckExact(exp->v.Constant.ಮೌಲ್ಯ)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "imaginary number required in complex literal");
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ exp;
}

ಅಭಿವ್ಯಕ್ತಿ_ty
_PyPegen_ensure_real(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty exp)
{
    ಯದಿ (exp->kind != Constant_kind || PyComplex_CheckExact(exp->v.Constant.ಮೌಲ್ಯ)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "real number required in complex literal");
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ exp;
}

mod_ty
_PyPegen_make_module(Parser *p, asdl_stmt_seq *a) {
    asdl_type_ignore_seq *type_ignores = ರಿಕ್ತ;
    Py_ssize_t ಅಂಕೆ = p->type_ignore_comments.ಅಂಕೆ_ವಸ್ತುಗಳು;
    ಯದಿ (ಅಂಕೆ > 0) {
        // Turn the raw (comment, lineno) pairs into TypeIgnore objects in the arena
        type_ignores = _Py_asdl_type_ignore_seq_new(ಅಂಕೆ, p->arena);
        ಯದಿ (type_ignores == ರಿಕ್ತ) {
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
        ಪ್ರತಿ (Py_ssize_t i = 0; i < ಅಂಕೆ; i++) {
            PyObject *tag = _PyPegen_new_type_comment(p, p->type_ignore_comments.ವಸ್ತುಗಳು[i].comment);
            ಯದಿ (tag == ರಿಕ್ತ) {
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
            }
            type_ignore_ty ti = _PyAST_TypeIgnore(p->type_ignore_comments.ವಸ್ತುಗಳು[i].lineno,
                                                  tag, p->arena);
            ಯದಿ (ti == ರಿಕ್ತ) {
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
            }
            asdl_seq_SET(type_ignores, i, ti);
        }
    }
    ಪ್ರತ್ಯಾಗಮ _PyAST_Module(a, type_ignores, p->arena);
}

PyObject *
_PyPegen_new_type_comment(Parser *p, ಧ್ರುವ ವರ್ಣ *s)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, strlen(s), ರಿಕ್ತ);
    ಯದಿ (res == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (_PyArena_AddPyObject(p->arena, res) < 0) {
        Py_DECREF(res);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ res;
}

arg_ty
_PyPegen_add_type_comment_to_arg(Parser *p, arg_ty a, Token *tc)
{
    ಯದಿ (tc == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ a;
    }
    ಧ್ರುವ ವರ್ಣ *bytes = PyBytes_AsString(tc->bytes);
    ಯದಿ (bytes == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    ಯದಿ (tco == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ _PyAST_arg(a->arg, a->annotation, tco,
                      a->lineno, a->col_offset, a->end_lineno, a->end_col_offset,
                      p->arena);
}

/* Checks if the NOTEQUAL token is valid given the current parser flags
0 indicates success and nonzero indicates failure (an exception may be set) */
ಪೂರ್ಣಾಙ್ಕ
_PyPegen_check_barry_as_flufl(Parser *p, Token* t) {
    ಪ್ರತಿಜ್ಞಾ(t->bytes != ರಿಕ್ತ);
    ಪ್ರತಿಜ್ಞಾ(t->type == NOTEQUAL);

    ಧ್ರುವ ವರ್ಣ* tok_str = PyBytes_AS_STRING(t->bytes);
    ಯದಿ (p->flags & PyPARSE_BARRY_AS_BDFL && strcmp(tok_str, "<>") != 0) {
        RAISE_SYNTAX_ERROR("with Barry as BDFL, use '<>' instead of '!='");
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಯದಿ (!(p->flags & PyPARSE_BARRY_AS_BDFL)) {
        ಪ್ರತ್ಯಾಗಮ strcmp(tok_str, "!=");
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ
_PyPegen_check_legacy_stmt(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty name) {
    ಯದಿ (name->kind != Name_kind) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಧ್ರುವ ವರ್ಣ* candidates[2] = {"print", "exec"};
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i=0; i<2; i++) {
        ಯದಿ (PyUnicode_CompareWithASCIIString(name->v.Name.id, candidates[i]) == 0) {
            ಪ್ರತ್ಯಾಗಮ 1;
        }
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ *
_PyPegen_raise_error_for_missing_comma(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty a, ಅಭಿವ್ಯಕ್ತಿ_ty b)
{
    // Don't raise for legacy statements like "print x" or "exec x"
    ಯದಿ (_PyPegen_check_legacy_stmt(p, a)) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    // Only raise inside parentheses/brackets (level > 0)
    ಯದಿ (p->tokens[p->mark - 1]->level == 0) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    // For multi-line expressions (like string concatenations), point to the
    // last line instead of the first for a more helpful error message.
    // Use a->col_offset as the starting column since all strings in the
    // concatenation typically share the same indentation.
    ಯದಿ (a->end_lineno > a->lineno) {
        ಪ್ರತ್ಯಾಗಮ RAISE_ERROR_KNOWN_LOCATION(
            p, PyExc_SyntaxError, a->end_lineno, a->col_offset,
            a->end_lineno, a->end_col_offset,
            "invalid syntax. Perhaps you forgot a comma?"
        );
    }
    ಪ್ರತ್ಯಾಗಮ RAISE_ERROR_KNOWN_LOCATION(
        p, PyExc_SyntaxError, a->lineno, a->col_offset,
        b->end_lineno, b->end_col_offset,
        "invalid syntax. Perhaps you forgot a comma?"
    );
}

ಸ್ಥಿರ ResultTokenWithMetadata *
result_token_with_metadata(Parser *p, ಶೂನ್ಯ *result, PyObject *metadata)
{
    ResultTokenWithMetadata *res = _PyArena_Malloc(p->arena, ಪರಿಮಾಣ(ResultTokenWithMetadata));
    ಯದಿ (res == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    res->metadata = metadata;
    res->result = result;
    ಪ್ರತ್ಯಾಗಮ res;
}

ಸ್ಥಿರ ವರ್ಣ
formatted_string_prefix(ಧ್ರುವ Parser *p)
{
    ಪೂರ್ಣಾಙ್ಕ nested = 0;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = p->mark - 1; i >= 0; i--) {
        ಪೂರ್ಣಾಙ್ಕ type = p->tokens[i]->type;
        ಯದಿ (type == FSTRING_END || type == TSTRING_END) {
            nested++;
        }
        ಅನ್ಯಥಾ ಯದಿ (type == FSTRING_START || type == TSTRING_START) {
            ಯದಿ (nested == 0) {
                ಪ್ರತ್ಯಾಗಮ type == TSTRING_START ? 't' : 'f';
            }
            nested--;
        }
    }
    Py_UNREACHABLE();
}

ResultTokenWithMetadata *
_PyPegen_check_fstring_conversion(Parser *p, Token* conv_token, ಅಭಿವ್ಯಕ್ತಿ_ty conv)
{
    ಯದಿ (conv_token->lineno != conv->lineno || conv_token->end_col_offset != conv->col_offset) {
        ಪ್ರತ್ಯಾಗಮ RAISE_SYNTAX_ERROR_KNOWN_RANGE(
            conv_token, conv,
            "%c-string: conversion type must come right after the exclamation mark",
            formatted_string_prefix(p)
        );
    }

    Py_UCS4 first = PyUnicode_READ_CHAR(conv->v.Name.id, 0);
    ಯದಿ (PyUnicode_GET_LENGTH(conv->v.Name.id) > 1 ||
            !(first == 's' || first == 'r' || first == 'a')) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(conv,
                                            "%c-string: invalid conversion character %R: expected 's', 'r', or 'a'",
                                            formatted_string_prefix(p),
                                            conv->v.Name.id);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ result_token_with_metadata(p, conv, conv_token->metadata);
}

ResultTokenWithMetadata *
_PyPegen_setup_full_format_spec(Parser *p, Token *colon, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *spec, ಪೂರ್ಣಾಙ್ಕ lineno, ಪೂರ್ಣಾಙ್ಕ col_offset,
                                ಪೂರ್ಣಾಙ್ಕ end_lineno, ಪೂರ್ಣಾಙ್ಕ end_col_offset, PyArena *arena)
{
    ಯದಿ (!spec) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // This is needed to keep compatibility with 3.11, where an empty format
    // spec is parsed as an *empty* JoinedStr node, instead of having an empty
    // constant in it.
    Py_ssize_t n_ವಸ್ತುಗಳು = asdl_seq_LEN(spec);
    Py_ssize_t non_empty_ಎಣಿಕೆ = 0;
    ಪ್ರತಿ (Py_ssize_t i = 0; i < n_ವಸ್ತುಗಳು; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty item = asdl_seq_GET(spec, i);
        non_empty_ಎಣಿಕೆ += !(item->kind == Constant_kind &&
                             PyUnicode_CheckExact(item->v.Constant.ಮೌಲ್ಯ) &&
                             PyUnicode_GET_LENGTH(item->v.Constant.ಮೌಲ್ಯ) == 0);
    }
    ಯದಿ (non_empty_ಎಣಿಕೆ != n_ವಸ್ತುಗಳು) {
        asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *resized_spec =
            Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(non_empty_ಎಣಿಕೆ, p->arena);
        ಯದಿ (resized_spec == ರಿಕ್ತ) {
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
        Py_ssize_t j = 0;
        ಪ್ರತಿ (Py_ssize_t i = 0; i < n_ವಸ್ತುಗಳು; i++) {
            ಅಭಿವ್ಯಕ್ತಿ_ty item = asdl_seq_GET(spec, i);
            ಯದಿ (item->kind == Constant_kind &&
                PyUnicode_CheckExact(item->v.Constant.ಮೌಲ್ಯ) &&
                PyUnicode_GET_LENGTH(item->v.Constant.ಮೌಲ್ಯ) == 0) {
                ಅನುವರ್ತ;
            }
            asdl_seq_SET(resized_spec, j++, item);
        }
        ಪ್ರತಿಜ್ಞಾ(j == non_empty_ಎಣಿಕೆ);
        spec = resized_spec;
    }
    ಅಭಿವ್ಯಕ್ತಿ_ty res;
    Py_ssize_t n = asdl_seq_LEN(spec);
    ಯದಿ (n == 0 || (n == 1 && asdl_seq_GET(spec, 0)->kind == Constant_kind)) {
        res = _PyAST_JoinedStr(spec, lineno, col_offset, end_lineno,
                                    end_col_offset, p->arena);
    } ಅನ್ಯಥಾ {
        res = _PyPegen_concatenate_strings(p, spec,
                             lineno, col_offset, end_lineno,
                             end_col_offset, arena);
    }
    ಯದಿ (!res) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ result_token_with_metadata(p, res, colon->metadata);
}

ಧ್ರುವ ವರ್ಣ *
Py_Pegen_get_ಅಭಿವ್ಯಕ್ತಿ_name(ಅಭಿವ್ಯಕ್ತಿ_ty e)
{
    ಪ್ರತಿಜ್ಞಾ(e != ರಿಕ್ತ);
    ವಿಕಲ್ಪ (e->kind) {
        ಪಕ್ಷ Attribute_kind:
            ಪ್ರತ್ಯಾಗಮ "attribute";
        ಪಕ್ಷ Subscript_kind:
            ಪ್ರತ್ಯಾಗಮ "subscript";
        ಪಕ್ಷ Starred_kind:
            ಪ್ರತ್ಯಾಗಮ "starred";
        ಪಕ್ಷ Name_kind:
            ಪ್ರತ್ಯಾಗಮ "name";
        ಪಕ್ಷ List_kind:
            ಪ್ರತ್ಯಾಗಮ "list";
        ಪಕ್ಷ Tuple_kind:
            ಪ್ರತ್ಯಾಗಮ "tuple";
        ಪಕ್ಷ Lambda_kind:
            ಪ್ರತ್ಯಾಗಮ "lambda";
        ಪಕ್ಷ Call_kind:
            ಪ್ರತ್ಯಾಗಮ "function call";
        ಪಕ್ಷ BoolOp_kind:
        ಪಕ್ಷ BinOp_kind:
        ಪಕ್ಷ UnaryOp_kind:
            ಪ್ರತ್ಯಾಗಮ "expression";
        ಪಕ್ಷ GeneratorExp_kind:
            ಪ್ರತ್ಯಾಗಮ "generator expression";
        ಪಕ್ಷ Yield_kind:
        ಪಕ್ಷ YieldFrom_kind:
            ಪ್ರತ್ಯಾಗಮ "yield expression";
        ಪಕ್ಷ Await_kind:
            ಪ್ರತ್ಯಾಗಮ "await expression";
        ಪಕ್ಷ ListComp_kind:
            ಪ್ರತ್ಯಾಗಮ "list comprehension";
        ಪಕ್ಷ SetComp_kind:
            ಪ್ರತ್ಯಾಗಮ "set comprehension";
        ಪಕ್ಷ DictComp_kind:
            ಪ್ರತ್ಯಾಗಮ "dict comprehension";
        ಪಕ್ಷ Dict_kind:
            ಪ್ರತ್ಯಾಗಮ "dict literal";
        ಪಕ್ಷ Set_kind:
            ಪ್ರತ್ಯಾಗಮ "set display";
        ಪಕ್ಷ JoinedStr_kind:
        ಪಕ್ಷ Formatted_ಮೌಲ್ಯ_kind:
            ಪ್ರತ್ಯಾಗಮ "f-string expression";
        ಪಕ್ಷ TemplateStr_kind:
        ಪಕ್ಷ Interpolation_kind:
            ಪ್ರತ್ಯಾಗಮ "t-string expression";
        ಪಕ್ಷ Constant_kind: {
            PyObject *ಮೌಲ್ಯ = e->v.Constant.ಮೌಲ್ಯ;
            ಯದಿ (ಮೌಲ್ಯ == Py_None) {
                ಪ್ರತ್ಯಾಗಮ "None";
            }
            ಯದಿ (ಮೌಲ್ಯ == Py_False) {
                ಪ್ರತ್ಯಾಗಮ "False";
            }
            ಯದಿ (ಮೌಲ್ಯ == Py_True) {
                ಪ್ರತ್ಯಾಗಮ "True";
            }
            ಯದಿ (ಮೌಲ್ಯ == Py_Ellipsis) {
                ಪ್ರತ್ಯಾಗಮ "ellipsis";
            }
            ಪ್ರತ್ಯಾಗಮ "literal";
        }
        ಪಕ್ಷ Compare_kind:
            ಪ್ರತ್ಯಾಗಮ "comparison";
        ಪಕ್ಷ IfExp_kind:
            ಪ್ರತ್ಯಾಗಮ "conditional expression";
        ಪಕ್ಷ Named_ಅಭಿವ್ಯಕ್ತಿ_kind:
            ಪ್ರತ್ಯಾಗಮ "named expression";
        ಸ್ವಭಾವ:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->kind, e->lineno);
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
}

ಅಭಿವ್ಯಕ್ತಿ_ty
_PyPegen_get_last_comprehension_item(comprehension_ty comprehension) {
    ಯದಿ (comprehension->ifs == ರಿಕ್ತ || asdl_seq_LEN(comprehension->ifs) == 0) {
        ಪ್ರತ್ಯಾಗಮ comprehension->iter;
    }
    ಪ್ರತ್ಯಾಗಮ PyPegen_last_item(comprehension->ifs, ಅಭಿವ್ಯಕ್ತಿ_ty);
}

ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_collect_call_seqs(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *a, asdl_seq *b,
                     ಪೂರ್ಣಾಙ್ಕ lineno, ಪೂರ್ಣಾಙ್ಕ col_offset, ಪೂರ್ಣಾಙ್ಕ end_lineno,
                     ಪೂರ್ಣಾಙ್ಕ end_col_offset, PyArena *arena) {
    Py_ssize_t args_len = asdl_seq_LEN(a);
    Py_ssize_t total_len = args_len;

    ಯದಿ (b == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ _PyAST_Call(_PyPegen_dummy_name(p), a, ರಿಕ್ತ, lineno, col_offset,
                        end_lineno, end_col_offset, arena);

    }

    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *starreds = _PyPegen_seq_extract_starred_exprs(p, b);
    ಯದಿ (!starreds && PyErr_Occurred()) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    asdl_keyword_seq *keywords = _PyPegen_seq_delete_starred_exprs(p, b);
    ಯದಿ (!keywords && PyErr_Occurred()) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (starreds) {
        total_len += asdl_seq_LEN(starreds);
    }

    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *args = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(total_len, arena);
    ಯದಿ (args == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    Py_ssize_t i = 0;
    ಪ್ರತಿ (i = 0; i < args_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(a, i));
    }
    ಪ್ರತಿ (; i < total_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(starreds, i - args_len));
    }

    ಪ್ರತ್ಯಾಗಮ _PyAST_Call(_PyPegen_dummy_name(p), args, keywords, lineno,
                       col_offset, end_lineno, end_col_offset, arena);
}

// AST Error reporting helpers

ಅಭಿವ್ಯಕ್ತಿ_ty
_PyPegen_get_invalid_target(ಅಭಿವ್ಯಕ್ತಿ_ty e, TARGETS_TYPE targets_type)
{
    ಯದಿ (e == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (targets_type == SINGLE_TARGETS ||
        targets_type == ATTRIBUTE_OR_SUBSCRIPT_TARGETS) {
        ಯದಿ (targets_type == SINGLE_TARGETS && e->kind == Name_kind) {
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
        ಪ್ರತ್ಯಾಗಮ e->kind == Attribute_kind || e->kind == Subscript_kind ? ರಿಕ್ತ : e;
    }

#define VISIT_CONTAINER(CONTAINER, TYPE) ಕುರು { \
        Py_ssize_t len = asdl_seq_LEN((CONTAINER)->v.TYPE.elts);\
        ಪ್ರತಿ (Py_ssize_t i = 0; i < len; i++) {\
            ಅಭಿವ್ಯಕ್ತಿ_ty other = asdl_seq_GET((CONTAINER)->v.TYPE.elts, i);\
            ಅಭಿವ್ಯಕ್ತಿ_ty child = _PyPegen_get_invalid_target(other, targets_type);\
            ಯದಿ (child != ರಿಕ್ತ) {\
                ಪ್ರತ್ಯಾಗಮ child;\
            }\
        }\
    } ಯಾವತ್ (0)

    // We only need to visit List and Tuple nodes recursively as those
    // are the only ones that can contain valid names in targets when
    // they are parsed as expressions. Any other kind of expression
    // that is a container (like Sets or Dicts) is directly invalid and
    // we don't need to visit it recursively.

    ವಿಕಲ್ಪ (e->kind) {
        ಪಕ್ಷ List_kind:
            VISIT_CONTAINER(e, List);
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        ಪಕ್ಷ Tuple_kind:
            VISIT_CONTAINER(e, Tuple);
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        ಪಕ್ಷ Starred_kind:
            ಯದಿ (targets_type == DEL_TARGETS) {
                ಪ್ರತ್ಯಾಗಮ e;
            }
            ಪ್ರತ್ಯಾಗಮ _PyPegen_get_invalid_target(e->v.Starred.ಮೌಲ್ಯ, targets_type);
        ಪಕ್ಷ Compare_kind:
            // This is needed, because the `a in b` in `for a in b` gets parsed
            // as a comparison, and so we need to search the left side of the comparison
            // for invalid targets.
            ಯದಿ (targets_type == FOR_TARGETS) {
                cmpop_ty cmpop = (cmpop_ty) asdl_seq_GET(e->v.Compare.ops, 0);
                ಯದಿ (cmpop == In) {
                    ಪ್ರತ್ಯಾಗಮ _PyPegen_get_invalid_target(e->v.Compare.ಎಡ, targets_type);
                }
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
            }
            ಪ್ರತ್ಯಾಗಮ e;
        ಪಕ್ಷ Name_kind:
        ಪಕ್ಷ Subscript_kind:
        ಪಕ್ಷ Attribute_kind:
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        ಸ್ವಭಾವ:
            ಪ್ರತ್ಯಾಗಮ e;
    }
}

ಶೂನ್ಯ *_PyPegen_arguments_parsing_error(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty e) {
    ಪೂರ್ಣಾಙ್ಕ kwarg_unpacking = 0;
    ಪ್ರತಿ (Py_ssize_t i = 0, l = asdl_seq_LEN(e->v.Call.keywords); i < l; i++) {
        keyword_ty keyword = asdl_seq_GET(e->v.Call.keywords, i);
        ಯದಿ (!keyword->arg) {
            kwarg_unpacking = 1;
        }
    }

    ಧ್ರುವ ವರ್ಣ *ಸಂದೇಶ = ರಿಕ್ತ;
    ಯದಿ (kwarg_unpacking) {
        ಸಂದೇಶ = "positional argument follows keyword argument unpacking";
    } ಅನ್ಯಥಾ {
        ಸಂದೇಶ = "positional argument follows keyword argument";
    }

    ಪ್ರತ್ಯಾಗಮ RAISE_SYNTAX_ERROR(ಸಂದೇಶ);
}

ಶೂನ್ಯ *
_PyPegen_nonparen_genexp_in_call(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty args, asdl_comprehension_seq *comprehensions)
{
    /* The rule that calls this function is 'args for_if_clauses'.
       For the input f(L, x for x in y), L and x are in args and
       the for is parsed as a for_if_clause. We have to check if
       len <= 1, so that input like dict((a, b) for a, b in x)
       gets successfully parsed and then we pass the last
       argument (x in the above example) as the location of the
       error */
    Py_ssize_t len = asdl_seq_LEN(args->v.Call.args);
    ಯದಿ (len <= 1) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    comprehension_ty last_comprehension = PyPegen_last_item(comprehensions, comprehension_ty);

    ಪ್ರತ್ಯಾಗಮ RAISE_SYNTAX_ERROR_KNOWN_RANGE(
        (ಅಭಿವ್ಯಕ್ತಿ_ty) asdl_seq_GET(args->v.Call.args, len - 1),
        _PyPegen_get_last_comprehension_item(last_comprehension),
        "Generator expression must be parenthesized"
    );
}

// Fstring stuff

ಸ್ಥಿರ ಅಭಿವ್ಯಕ್ತಿ_ty
_PyPegen_decode_fstring_part(Parser* p, ಪೂರ್ಣಾಙ್ಕ is_raw, ಅಭಿವ್ಯಕ್ತಿ_ty constant, Token* token) {
    ಪ್ರತಿಜ್ಞಾ(PyUnicode_CheckExact(constant->v.Constant.ಮೌಲ್ಯ));

    ಧ್ರುವ ವರ್ಣ* bstr = PyUnicode_AsUTF8(constant->v.Constant.ಮೌಲ್ಯ);
    ಯದಿ (bstr == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    size_t len;
    ಯದಿ (strcmp(bstr, "{{") == 0 || strcmp(bstr, "}}") == 0) {
        len = 1;
    } ಅನ್ಯಥಾ {
        len = strlen(bstr);
    }

    is_raw = is_raw || strchr(bstr, '\\') == ರಿಕ್ತ;
    PyObject *str = _PyPegen_decode_string(p, is_raw, bstr, len, token);
    ಯದಿ (str == ರಿಕ್ತ) {
        _Pypegen_raise_decode_error(p);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ _PyAST_Constant(str, ರಿಕ್ತ, constant->lineno, constant->col_offset,
                           constant->end_lineno, constant->end_col_offset,
                           p->arena);
}

ಸ್ಥಿರ asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *
_get_resized_exprs(Parser *p, Token *a, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *raw_expressions,
                   Token *b, ಪೂರ್ಣಾಙ್ಕ is_tstring)
{
    Py_ssize_t n_ವಸ್ತುಗಳು = asdl_seq_LEN(raw_expressions);
    Py_ssize_t total_ವಸ್ತುಗಳು = n_ವಸ್ತುಗಳು;
    ಪ್ರತಿ (Py_ssize_t i = 0; i < n_ವಸ್ತುಗಳು; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty item = asdl_seq_GET(raw_expressions, i);
        ಯದಿ (item->kind == JoinedStr_kind) {
            total_ವಸ್ತುಗಳು += asdl_seq_LEN(item->v.JoinedStr.ಮೌಲ್ಯಗಳು) - 1;
        }
    }

    ಧ್ರುವ ವರ್ಣ* quote_str = PyBytes_AsString(a->bytes);
    ಯದಿ (quote_str == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪೂರ್ಣಾಙ್ಕ is_raw = strpbrk(quote_str, "rR") != ರಿಕ್ತ;

    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *seq = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(total_ವಸ್ತುಗಳು, p->arena);
    ಯದಿ (seq == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    Py_ssize_t index = 0;
    ಪ್ರತಿ (Py_ssize_t i = 0; i < n_ವಸ್ತುಗಳು; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty item = asdl_seq_GET(raw_expressions, i);

        /* Debug expressions arrive as JoinedStr(text, value); flatten them
           into the surrounding string. */
        ಯದಿ (item->kind == JoinedStr_kind) {
            asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *ಮೌಲ್ಯಗಳು = item->v.JoinedStr.ಮೌಲ್ಯಗಳು;
            ಯದಿ (asdl_seq_LEN(ಮೌಲ್ಯಗಳು) != 2) {
                PyErr_Format(PyExc_SystemError,
                             is_tstring
                             ? "unexpected TemplateStr node without debug data in t-string at line %d"
                             : "unexpected JoinedStr node without debug data in f-string at line %d",
                             item->lineno);
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
            }

            ಅಭಿವ್ಯಕ್ತಿ_ty first = asdl_seq_GET(ಮೌಲ್ಯಗಳು, 0);
            ಪ್ರತಿಜ್ಞಾ(first->kind == Constant_kind);
            asdl_seq_SET(seq, index++, first);

            ಅಭಿವ್ಯಕ್ತಿ_ty second = asdl_seq_GET(ಮೌಲ್ಯಗಳು, 1);
            ಪ್ರತಿಜ್ಞಾ((is_tstring &&
                    second->kind == Interpolation_kind) ||
                   second->kind == Formatted_ಮೌಲ್ಯ_kind);
            asdl_seq_SET(seq, index++, second);

            ಅನುವರ್ತ;
        }

        ಯದಿ (item->kind == Constant_kind) {
            item = _PyPegen_decode_fstring_part(p, is_raw, item, b);
            ಯದಿ (item == ರಿಕ್ತ) {
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
            }

            /* Tokenizer emits string parts even when the underlying string
            might become an empty value (e.g. FSTRING_MIDDLE with the value \\n)
            so we need to check for them and simplify it here. */
            ಯದಿ (PyUnicode_CheckExact(item->v.Constant.ಮೌಲ್ಯ)
                && PyUnicode_GET_LENGTH(item->v.Constant.ಮೌಲ್ಯ) == 0) {
                ಅನುವರ್ತ;
            }
        }
        asdl_seq_SET(seq, index++, item);
    }

    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *resized_exprs;
    ಯದಿ (index != total_ವಸ್ತುಗಳು) {
        resized_exprs = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(index, p->arena);
        ಯದಿ (resized_exprs == ರಿಕ್ತ) {
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
        ಪ್ರತಿ (Py_ssize_t i = 0; i < index; i++) {
            asdl_seq_SET(resized_exprs, i, asdl_seq_GET(seq, i));
        }
    }
    ಅನ್ಯಥಾ {
        resized_exprs = seq;
    }
    ಪ್ರತ್ಯಾಗಮ resized_exprs;
}

ಅಭಿವ್ಯಕ್ತಿ_ty
_PyPegen_template_str(Parser *p, Token *a, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *raw_expressions, Token *b) {

    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 1);
    ಯದಿ (resized_exprs == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ _PyAST_TemplateStr(resized_exprs, a->lineno, a->col_offset,
                              b->end_lineno, b->end_col_offset,
                              p->arena);
}

ಅಭಿವ್ಯಕ್ತಿ_ty
_PyPegen_joined_str(Parser *p, Token* a, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq* raw_expressions, Token*b) {

    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 0);
    ಯದಿ (resized_exprs == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ _PyAST_JoinedStr(resized_exprs, a->lineno, a->col_offset,
                            b->end_lineno, b->end_col_offset,
                            p->arena);
}

ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok) {
    ಯದಿ (tok->bytes == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ Py_Pegen_raise_incomplete_ಒಳಹರಿವು(p);
    }
    Py_ssize_t bsize = PyBytes_GET_SIZE(tok->bytes);
    ಧ್ರುವ ವರ್ಣ *bstr = PyBytes_AS_STRING(tok->bytes);

    ಪೂರ್ಣಾಙ್ಕ is_raw = tok->is_raw;

    PyObject* str = _PyPegen_decode_string(p, is_raw, bstr, bsize, tok);
    ಯದಿ (str == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ _PyAST_Constant(str, ರಿಕ್ತ, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_constant_from_token(Parser* p, Token* tok) {
    ಯದಿ (tok->bytes == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ Py_Pegen_raise_incomplete_ಒಳಹರಿವು(p);
    }
    ಧ್ರುವ ವರ್ಣ *bstr = PyBytes_AS_STRING(tok->bytes);
    PyObject* str = PyUnicode_FromString(bstr);
    ಯದಿ (str == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ _PyAST_Constant(str, ರಿಕ್ತ, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_constant_from_string(Parser* p, Token* tok) {
    ಯದಿ (tok->bytes == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ Py_Pegen_raise_incomplete_ಒಳಹರಿವು(p);
    }
    ಧ್ರುವ ವರ್ಣ *the_str = PyBytes_AS_STRING(tok->bytes);
    PyObject *s = _PyPegen_parse_string(p, tok);
    ಯದಿ (s == ರಿಕ್ತ) {
        _Pypegen_raise_decode_error(p);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (_PyArena_AddPyObject(p->arena, s) < 0) {
        Py_DECREF(s);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    PyObject *kind = ರಿಕ್ತ;
    ಯದಿ (the_str && the_str[0] == 'u') {
        kind = _PyPegen_new_identifier(p, "u");
        ಯದಿ (kind == ರಿಕ್ತ) {
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ _PyAST_Constant(s, kind, tok->lineno, tok->col_offset, tok->end_lineno, tok->end_col_offset, p->arena);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
_get_interpolation_conversion(Parser *p, Token *debug, ResultTokenWithMetadata *conversion,
                              ResultTokenWithMetadata *format)
{
    ಯದಿ (conversion != ರಿಕ್ತ) {
        ಅಭಿವ್ಯಕ್ತಿ_ty conversion_ಅಭಿವ್ಯಕ್ತಿ = (ಅಭಿವ್ಯಕ್ತಿ_ty) conversion->result;
        ಪ್ರತಿಜ್ಞಾ(conversion_ಅಭಿವ್ಯಕ್ತಿ->kind == Name_kind);
        Py_UCS4 first = PyUnicode_READ_CHAR(conversion_ಅಭಿವ್ಯಕ್ತಿ->v.Name.id, 0);
        ಪ್ರತ್ಯಾಗಮ Py_SAFE_DOWNCAST(first, Py_UCS4, ಪೂರ್ಣಾಙ್ಕ);
    }
    ಅನ್ಯಥಾ ಯದಿ (debug && !format) {
        /* If no conversion is specified, use !r for debug expressions */
        ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)'r';
    }
    ಪ್ರತ್ಯಾಗಮ -1;
}

ಸ್ಥಿರ PyObject *
strip_interpolation_debug_ಅಭಿವ್ಯಕ್ತಿ(PyObject *exprstr)
{
    Py_ssize_t len = PyUnicode_GET_LENGTH(exprstr);

    /* Discard whitespace and explicit line continuations after the debug "="
       but preserve whitespace before it. */
    ಯಾವತ್ (len > 0) {
        ಪೂರ್ಣಾಙ್ಕ has_newline = 0;
        ಯಾವತ್ (len > 0) {
            Py_UCS4 c = PyUnicode_READ_CHAR(exprstr, len - 1);
            ಯದಿ (!_PyUnicode_IsWhitespace(c)) {
                ಭಙ್ಗ;
            }
            ಯದಿ (c == '\r' || c == '\n') {
                has_newline = 1;
            }
            len--;
        }
        ಯದಿ (!has_newline || len == 0 ||
            PyUnicode_READ_CHAR(exprstr, len - 1) != '\\')
        {
            ಭಙ್ಗ;
        }
        len--;
    }

    /* Preserve unexpected metadata instead of dropping source text. */
    ಯದಿ (len == 0 || PyUnicode_READ_CHAR(exprstr, len - 1) != '=') {
        ಪ್ರತ್ಯಾಗಮ Py_NewRef(exprstr);
    }

    ಪ್ರತ್ಯಾಗಮ PyUnicode_Substring(exprstr, 0, len - 1);
}

ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_interpolation(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, ಪೂರ್ಣಾಙ್ಕ lineno, ಪೂರ್ಣಾಙ್ಕ col_offset,
                                 ಪೂರ್ಣಾಙ್ಕ end_lineno, ಪೂರ್ಣಾಙ್ಕ end_col_offset, PyArena *arena) {

    ಪೂರ್ಣಾಙ್ಕ conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    /* Find the non whitespace token after the "=" */
    ಪೂರ್ಣಾಙ್ಕ debug_end_ಸಾಲು, debug_end_offset;
    PyObject *debug_metadata;
    constant exprstr;

    ಯದಿ (conversion) {
        debug_end_ಸಾಲು = ((ಅಭಿವ್ಯಕ್ತಿ_ty) conversion->result)->lineno;
        debug_end_offset = ((ಅಭಿವ್ಯಕ್ತಿ_ty) conversion->result)->col_offset;
        debug_metadata = exprstr = conversion->metadata;
    }
    ಅನ್ಯಥಾ ಯದಿ (format) {
        debug_end_ಸಾಲು = ((ಅಭಿವ್ಯಕ್ತಿ_ty) format->result)->lineno;
        debug_end_offset = ((ಅಭಿವ್ಯಕ್ತಿ_ty) format->result)->col_offset + 1;
        debug_metadata = exprstr = format->metadata;
    }
    ಅನ್ಯಥಾ {
        debug_end_ಸಾಲು = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = exprstr = closing_brace->metadata;
    }

    ಪ್ರತಿಜ್ಞಾ(exprstr != ರಿಕ್ತ);
    PyObject *final_exprstr = debug
        ? strip_interpolation_debug_ಅಭಿವ್ಯಕ್ತಿ(exprstr)
        : Py_NewRef(exprstr);
    ಯದಿ (!final_exprstr || _PyArena_AddPyObject(arena, final_exprstr) < 0) {
        Py_XDECREF(final_exprstr);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಅಭಿವ್ಯಕ್ತಿ_ty interpolation = _PyAST_Interpolation(
        expression, final_exprstr, conversion_val, format ? (ಅಭಿವ್ಯಕ್ತಿ_ty) format->result : ರಿಕ್ತ,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    ಯದಿ (!interpolation || !debug) {
        ಪ್ರತ್ಯಾಗಮ interpolation;
    }

    ಅಭಿವ್ಯಕ್ತಿ_ty debug_text = _PyAST_Constant(debug_metadata, ರಿಕ್ತ, lineno, col_offset + 1, debug_end_ಸಾಲು,
                                            debug_end_offset - 1, p->arena);
    ಯದಿ (!debug_text) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *ಮೌಲ್ಯಗಳು = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(2, arena);
    ಯದಿ (!ಮೌಲ್ಯಗಳು) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    asdl_seq_SET(ಮೌಲ್ಯಗಳು, 0, debug_text);
    asdl_seq_SET(ಮೌಲ್ಯಗಳು, 1, interpolation);
    ಪ್ರತ್ಯಾಗಮ _PyAST_JoinedStr(ಮೌಲ್ಯಗಳು, lineno, col_offset, debug_end_ಸಾಲು, debug_end_offset, p->arena);
}

ಅಭಿವ್ಯಕ್ತಿ_ty Py_Pegen_formatted_ಮೌಲ್ಯ(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, ಪೂರ್ಣಾಙ್ಕ lineno, ಪೂರ್ಣಾಙ್ಕ col_offset,
                                 ಪೂರ್ಣಾಙ್ಕ end_lineno, ಪೂರ್ಣಾಙ್ಕ end_col_offset, PyArena *arena) {
    ಪೂರ್ಣಾಙ್ಕ conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    ಅಭಿವ್ಯಕ್ತಿ_ty formatted_ಮೌಲ್ಯ = Py_AST_Formatted_ಮೌಲ್ಯ(
        expression, conversion_val, format ? (ಅಭಿವ್ಯಕ್ತಿ_ty) format->result : ರಿಕ್ತ,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    ಯದಿ (!formatted_ಮೌಲ್ಯ || !debug) {
        ಪ್ರತ್ಯಾಗಮ formatted_ಮೌಲ್ಯ;
    }

    /* Find the non whitespace token after the "=" */
    ಪೂರ್ಣಾಙ್ಕ debug_end_ಸಾಲು, debug_end_offset;
    PyObject *debug_metadata;

    ಯದಿ (conversion) {
        debug_end_ಸಾಲು = ((ಅಭಿವ್ಯಕ್ತಿ_ty) conversion->result)->lineno;
        debug_end_offset = ((ಅಭಿವ್ಯಕ್ತಿ_ty) conversion->result)->col_offset;
        debug_metadata = conversion->metadata;
    }
    ಅನ್ಯಥಾ ಯದಿ (format) {
        debug_end_ಸಾಲು = ((ಅಭಿವ್ಯಕ್ತಿ_ty) format->result)->lineno;
        debug_end_offset = ((ಅಭಿವ್ಯಕ್ತಿ_ty) format->result)->col_offset + 1;
        debug_metadata = format->metadata;
    }
    ಅನ್ಯಥಾ {
        debug_end_ಸಾಲು = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = closing_brace->metadata;
    }
    ಅಭಿವ್ಯಕ್ತಿ_ty debug_text = _PyAST_Constant(debug_metadata, ರಿಕ್ತ, lineno, col_offset + 1, debug_end_ಸಾಲು,
                                            debug_end_offset - 1, p->arena);
    ಯದಿ (!debug_text) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *ಮೌಲ್ಯಗಳು = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(2, arena);
    ಯದಿ (!ಮೌಲ್ಯಗಳು) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    asdl_seq_SET(ಮೌಲ್ಯಗಳು, 0, debug_text);
    asdl_seq_SET(ಮೌಲ್ಯಗಳು, 1, formatted_ಮೌಲ್ಯ);
    ಪ್ರತ್ಯಾಗಮ _PyAST_JoinedStr(ಮೌಲ್ಯಗಳು, lineno, col_offset, debug_end_ಸಾಲು, debug_end_offset, p->arena);
}

ಸ್ಥಿರ ಅಭಿವ್ಯಕ್ತಿ_ty
_build_concatenated_bytes(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *strings, ಪೂರ್ಣಾಙ್ಕ lineno,
                        ಪೂರ್ಣಾಙ್ಕ col_offset, ಪೂರ್ಣಾಙ್ಕ end_lineno, ಪೂರ್ಣಾಙ್ಕ end_col_offset,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    ಪ್ರತಿಜ್ಞಾ(len > 0);

    /* Bytes literals never get a kind, but just for consistency
        since they are represented as Constant nodes, we'll mirror
        the same behavior as unicode strings for determining the
        kind. */
    PyObject *kind = asdl_seq_GET(strings, 0)->v.Constant.kind;

    Py_ssize_t total = 0;
    ಪ್ರತಿ (Py_ssize_t i = 0; i < len; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.ಮೌಲ್ಯ;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        ಯದಿ (part > PY_SSIZE_T_MAX - total) {
            PyErr_NoMemory();
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
        total += part;
    }

    PyBytesWriter *writer = PyBytesWriter_Create(total);
    ಯದಿ (writer == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ವರ್ಣ *out = PyBytesWriter_GetData(writer);

    ಪ್ರತಿ (Py_ssize_t i = 0; i < len; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.ಮೌಲ್ಯ;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        ಯದಿ (part > 0) {
            memcpy(out, PyBytes_AS_STRING(bytes), part);
            out += part;
        }
    }

    PyObject *res = PyBytesWriter_Finish(writer);
    ಯದಿ (res == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (_PyArena_AddPyObject(arena, res) < 0) {
        Py_DECREF(res);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ _PyAST_Constant(res, kind, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

ಸ್ಥಿರ ಅಭಿವ್ಯಕ್ತಿ_ty
_build_concatenated_unicode(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *strings, ಪೂರ್ಣಾಙ್ಕ lineno,
                        ಪೂರ್ಣಾಙ್ಕ col_offset, ಪೂರ್ಣಾಙ್ಕ end_lineno, ಪೂರ್ಣಾಙ್ಕ end_col_offset,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    ಪ್ರತಿಜ್ಞಾ(len > 1);

    ಅಭಿವ್ಯಕ್ತಿ_ty first = asdl_seq_GET(strings, 0);

    /* When a string is getting concatenated, the kind of the string
        is determined by the first string in the concatenation
        sequence.

        u"abc" "def" -> u"abcdef"
        "abc" u"abc" ->  "abcabc" */
    PyObject *kind = first->v.Constant.kind;

    PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
    ಯದಿ (writer == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತಿ (Py_ssize_t i = 0; i < len; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty current_elem = asdl_seq_GET(strings, i);
        ಪ್ರತಿಜ್ಞಾ(current_elem->kind == Constant_kind);

        ಯದಿ (PyUnicodeWriter_WriteStr(writer,
                                     current_elem->v.Constant.ಮೌಲ್ಯ)) {
            PyUnicodeWriter_Discard(writer);
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
    }

    PyObject *final = PyUnicodeWriter_Finish(writer);
    ಯದಿ (final == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (_PyArena_AddPyObject(p->arena, final) < 0) {
        Py_DECREF(final);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ _PyAST_Constant(final, kind, lineno, col_offset,
                           end_lineno, end_col_offset, arena);
}

ಸ್ಥಿರ asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *
_build_concatenated_str(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *strings,
                               ಪೂರ್ಣಾಙ್ಕ lineno, ಪೂರ್ಣಾಙ್ಕ col_offset, ಪೂರ್ಣಾಙ್ಕ end_lineno,
                               ಪೂರ್ಣಾಙ್ಕ end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    ಪ್ರತಿಜ್ಞಾ(len > 0);

    Py_ssize_t n_flattened_elements = 0;
    ಪ್ರತಿ (Py_ssize_t i = 0; i < len; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty elem = asdl_seq_GET(strings, i);
        ವಿಕಲ್ಪ(elem->kind) {
            ಪಕ್ಷ JoinedStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.JoinedStr.ಮೌಲ್ಯಗಳು);
                ಭಙ್ಗ;
            ಪಕ್ಷ TemplateStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.TemplateStr.ಮೌಲ್ಯಗಳು);
                ಭಙ್ಗ;
            ಸ್ವಭಾವ:
                n_flattened_elements++;
                ಭಙ್ಗ;
        }
    }


    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq* flattened = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(n_flattened_elements, p->arena);
    ಯದಿ (flattened == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    /* build flattened list */
    Py_ssize_t current_pos = 0;
    ಪ್ರತಿ (Py_ssize_t i = 0; i < len; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty elem = asdl_seq_GET(strings, i);
        ವಿಕಲ್ಪ(elem->kind) {
            ಪಕ್ಷ JoinedStr_kind:
                ಪ್ರತಿ (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.JoinedStr.ಮೌಲ್ಯಗಳು); j++) {
                    ಅಭಿವ್ಯಕ್ತಿ_ty subvalue = asdl_seq_GET(elem->v.JoinedStr.ಮೌಲ್ಯಗಳು, j);
                    ಯದಿ (subvalue == ರಿಕ್ತ) {
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                ಭಙ್ಗ;
            ಪಕ್ಷ TemplateStr_kind:
                ಪ್ರತಿ (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.TemplateStr.ಮೌಲ್ಯಗಳು); j++) {
                    ಅಭಿವ್ಯಕ್ತಿ_ty subvalue = asdl_seq_GET(elem->v.TemplateStr.ಮೌಲ್ಯಗಳು, j);
                    ಯದಿ (subvalue == ರಿಕ್ತ) {
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                ಭಙ್ಗ;
            ಸ್ವಭಾವ:
                asdl_seq_SET(flattened, current_pos++, elem);
                ಭಙ್ಗ;
        }
    }

    /* calculate folded element count */
    Py_ssize_t n_elements = 0;
    ಪೂರ್ಣಾಙ್ಕ prev_is_constant = 0;
    ಪ್ರತಿ (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty elem = asdl_seq_GET(flattened, i);

        /* The concatenation of a FormattedValue and an empty Constant should
           lead to the FormattedValue itself. Thus, we will not take any empty
           constants into account, just as in `_PyPegen_joined_str` */
        ಯದಿ (elem->kind == Constant_kind &&
            PyUnicode_CheckExact(elem->v.Constant.ಮೌಲ್ಯ) &&
            PyUnicode_GET_LENGTH(elem->v.Constant.ಮೌಲ್ಯ) == 0)
            ಅನುವರ್ತ;

        ಯದಿ (!prev_is_constant || elem->kind != Constant_kind) {
            n_elements++;
        }
        prev_is_constant = elem->kind == Constant_kind;
    }

    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq* ಮೌಲ್ಯಗಳು = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(n_elements, p->arena);
    ಯದಿ (ಮೌಲ್ಯಗಳು == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    /* build folded list */
    current_pos = 0;
    ಪ್ರತಿ (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty elem = asdl_seq_GET(flattened, i);

        /* if the current elem and the following are constants,
           fold them and all consequent constants */
        ಯದಿ (elem->kind == Constant_kind) {
            ಯದಿ (i + 1 < n_flattened_elements &&
                asdl_seq_GET(flattened, i + 1)->kind == Constant_kind) {
                ಅಭಿವ್ಯಕ್ತಿ_ty first_elem = elem;

                /* When a string is getting concatenated, the kind of the string
                   is determined by the first string in the concatenation
                   sequence.

                   u"abc" "def" -> u"abcdef"
                   "abc" u"abc" ->  "abcabc" */
                PyObject *kind = elem->v.Constant.kind;

                PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
                ಯದಿ (writer == ರಿಕ್ತ) {
                    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
                }
                ಅಭಿವ್ಯಕ್ತಿ_ty last_elem = elem;
                Py_ssize_t j;
                ಪ್ರತಿ (j = i; j < n_flattened_elements; j++) {
                    ಅಭಿವ್ಯಕ್ತಿ_ty current_elem = asdl_seq_GET(flattened, j);
                    ಯದಿ (current_elem->kind == Constant_kind) {
                        ಯದಿ (PyUnicodeWriter_WriteStr(writer,
                                                     current_elem->v.Constant.ಮೌಲ್ಯ)) {
                            PyUnicodeWriter_Discard(writer);
                            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
                        }
                        last_elem = current_elem;
                    } ಅನ್ಯಥಾ {
                        ಭಙ್ಗ;
                    }
                }
                i = j - 1;

                PyObject *concat_str = PyUnicodeWriter_Finish(writer);
                ಯದಿ (concat_str == ರಿಕ್ತ) {
                    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
                }
                ಯದಿ (_PyArena_AddPyObject(p->arena, concat_str) < 0) {
                    Py_DECREF(concat_str);
                    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
                }
                elem = _PyAST_Constant(concat_str, kind, first_elem->lineno,
                                       first_elem->col_offset,
                                       last_elem->end_lineno,
                                       last_elem->end_col_offset, p->arena);
                ಯದಿ (elem == ರಿಕ್ತ) {
                    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
                }
            }

            /* Drop all empty contanst strings */
            ಯದಿ (PyUnicode_CheckExact(elem->v.Constant.ಮೌಲ್ಯ) &&
                PyUnicode_GET_LENGTH(elem->v.Constant.ಮೌಲ್ಯ) == 0) {
                ಅನುವರ್ತ;
            }
        }

        asdl_seq_SET(ಮೌಲ್ಯಗಳು, current_pos++, elem);
    }

    ಪ್ರತಿಜ್ಞಾ(current_pos == n_elements);
    ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯಗಳು;
}

ಸ್ಥಿರ ಅಭಿವ್ಯಕ್ತಿ_ty
_build_concatenated_joined_str(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *strings,
                               ಪೂರ್ಣಾಙ್ಕ lineno, ಪೂರ್ಣಾಙ್ಕ col_offset, ಪೂರ್ಣಾಙ್ಕ end_lineno,
                               ಪೂರ್ಣಾಙ್ಕ end_col_offset, PyArena *arena)
{
    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *ಮೌಲ್ಯಗಳು = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    ಯದಿ (!ಮೌಲ್ಯಗಳು) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ _PyAST_JoinedStr(ಮೌಲ್ಯಗಳು, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

ಅಭಿವ್ಯಕ್ತಿ_ty
_PyPegen_concatenate_tstrings(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *strings,
                               ಪೂರ್ಣಾಙ್ಕ lineno, ಪೂರ್ಣಾಙ್ಕ col_offset, ಪೂರ್ಣಾಙ್ಕ end_lineno,
                               ಪೂರ್ಣಾಙ್ಕ end_col_offset, PyArena *arena)
{
    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *ಮೌಲ್ಯಗಳು = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    ಯದಿ (!ಮೌಲ್ಯಗಳು) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ _PyAST_TemplateStr(ಮೌಲ್ಯಗಳು, lineno, col_offset, end_lineno,
        end_col_offset, arena);
}

ಅಭಿವ್ಯಕ್ತಿ_ty
_PyPegen_concatenate_strings(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *strings,
                             ಪೂರ್ಣಾಙ್ಕ lineno, ಪೂರ್ಣಾಙ್ಕ col_offset, ಪೂರ್ಣಾಙ್ಕ end_lineno,
                             ಪೂರ್ಣಾಙ್ಕ end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    ಪ್ರತಿಜ್ಞಾ(len > 0);

    ಪೂರ್ಣಾಙ್ಕ f_string_found = 0;
    ಪೂರ್ಣಾಙ್ಕ unicode_string_found = 0;
    ಪೂರ್ಣಾಙ್ಕ bytes_found = 0;

    Py_ssize_t i = 0;
    ಪ್ರತಿ (i = 0; i < len; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty elem = asdl_seq_GET(strings, i);
        ವಿಕಲ್ಪ(elem->kind) {
            ಪಕ್ಷ Constant_kind:
                ಯದಿ (PyBytes_CheckExact(elem->v.Constant.ಮೌಲ್ಯ)) {
                    bytes_found = 1;
                } ಅನ್ಯಥಾ {
                    unicode_string_found = 1;
                }
                ಭಙ್ಗ;
            ಪಕ್ಷ JoinedStr_kind:
                f_string_found = 1;
                ಭಙ್ಗ;
            ಪಕ್ಷ TemplateStr_kind:
                // python.gram handles this; we should never get here
                ಪ್ರತಿಜ್ಞಾ(0);
                ಭಙ್ಗ;
            ಸ್ವಭಾವ:
                f_string_found = 1;
                ಭಙ್ಗ;
        }
    }

    // Cannot mix unicode and bytes
    ಯದಿ ((unicode_string_found || f_string_found) && bytes_found) {
        RAISE_SYNTAX_ERROR("cannot mix bytes and nonbytes literals");
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // If it's only bytes or only unicode string, do a simple concat
    ಯದಿ (!f_string_found) {
        ಯದಿ (len == 1) {
            ಪ್ರತ್ಯಾಗಮ asdl_seq_GET(strings, 0);
        }
        ಅನ್ಯಥಾ ಯದಿ (bytes_found) {
            ಪ್ರತ್ಯಾಗಮ _build_concatenated_bytes(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
        ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ _build_concatenated_unicode(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
    }

    ಪ್ರತ್ಯಾಗಮ _build_concatenated_joined_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
_warn_relative_import_of_lazy(Parser *p, asdl_seq *dots, ಅಭಿವ್ಯಕ್ತಿ_ty module)
{
    // Warn about `from . lazy import x`: the whitespace between the dots and
    // the module name is insignificant, so this is parsed exactly like
    // `from .lazy import x` (an import of the relative module "lazy"), but it
    // is most likely a transposition of `lazy from . import x` (PEP 810).
    ಯದಿ (p->call_invalid_rules) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    // Only fire if there is whitespace between the last dot and the name,
    // i.e. not for the common `from .lazy import x` spelling.
    Token *last_dot = asdl_seq_GET_UNTYPED(dots, asdl_seq_LEN(dots) - 1);
    ಯದಿ (
        last_dot->end_lineno == module->lineno
        && last_dot->end_col_offset == module->col_offset
    ) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ = Py_Pegen_seq_ಎಣಿಕೆ_dots(dots);
    ವರ್ಣ *buf = PyMem_RawMalloc(ಎಣಿಕೆ + 1);
    ಯದಿ (buf == ರಿಕ್ತ) {
        PyErr_NoMemory();
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    memset(buf, '.', ಎಣಿಕೆ);
    buf[ಎಣಿಕೆ] = '\0';

    PyObject *ಸಂದೇಶ = PyUnicode_FromFormat(
        "'from %s lazy import' is the same as 'from %slazy import'; "
        "did you mean 'lazy from %s import'?",
        buf, buf, buf);
    PyMem_RawFree(buf);
    ಯದಿ (ಸಂದೇಶ == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    ಪೂರ್ಣಾಙ್ಕ res = _PyErr_EmitSyntaxWarning(ಸಂದೇಶ,
                                       info.filename,
                                       module->lineno,
                                       module->col_offset + 1,
                                       module->end_lineno,
                                       module->end_col_offset + 1,
                                       info.module);
    Py_DECREF(ಸಂದೇಶ);
    ಪ್ರತ್ಯಾಗಮ res;
}

stmt_ty
_PyPegen_checked_from_import(Parser *p, asdl_seq *dots, ಅಭಿವ್ಯಕ್ತಿ_ty module_name,
                             asdl_alias_seq *names, ಅಭಿವ್ಯಕ್ತಿ_ty lazy_token, ಪೂರ್ಣಾಙ್ಕ lineno,
                             ಪೂರ್ಣಾಙ್ಕ col_offset, ಪೂರ್ಣಾಙ್ಕ end_lineno, ಪೂರ್ಣಾಙ್ಕ end_col_offset,
                             PyArena *arena)
{
    identifier module = module_name->v.Name.id;
    ಪೂರ್ಣಾಙ್ಕ level = Py_Pegen_seq_ಎಣಿಕೆ_dots(dots);
    ಯದಿ (level == 0 && PyUnicode_CompareWithASCIIString(module, "__future__") == 0) {
        ಯದಿ (lazy_token) {
            RAISE_SYNTAX_ERROR_KNOWN_LOCATION(lazy_token,
                "lazy from __future__ import is not allowed");
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
        ಪ್ರತಿ (Py_ssize_t i = 0; i < asdl_seq_LEN(names); i++) {
            alias_ty alias = asdl_seq_GET(names, i);
            ಯದಿ (PyUnicode_CompareWithASCIIString(alias->name, "barry_as_FLUFL") == 0) {
                p->flags |= PyPARSE_BARRY_AS_BDFL;
                p->tok->barry_as_bdfl = 1;
            }
        }
    }
    ಅನ್ಯಥಾ ಯದಿ (
        level > 0
        && lazy_token == ರಿಕ್ತ
        && PyUnicode_CompareWithASCIIString(module, "lazy") == 0
    ) {
        ಯದಿ (_warn_relative_import_of_lazy(p, dots, module_name) < 0) {
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ _PyAST_ImportFrom(module, names, level, lazy_token ? 1 : 0, lineno,
                             col_offset, end_lineno, end_col_offset, arena);
}

asdl_stmt_seq*
_PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts) {
    ಯದಿ (!p->call_invalid_rules) {
        ಪ್ರತ್ಯಾಗಮ stmts;
    }
    Py_ssize_t len = asdl_seq_LEN(stmts);
    ಯದಿ (len == 0) {
        ಪ್ರತ್ಯಾಗಮ stmts;
    }
    stmt_ty last_stmt = asdl_seq_GET(stmts, len - 1);
    ಯದಿ (p->last_stmt_location.lineno > last_stmt->lineno) {
        ಪ್ರತ್ಯಾಗಮ stmts;
    }
    p->last_stmt_location.lineno = last_stmt->lineno;
    p->last_stmt_location.col_offset = last_stmt->col_offset;
    p->last_stmt_location.end_lineno = last_stmt->end_lineno;
    p->last_stmt_location.end_col_offset = last_stmt->end_col_offset;
    ಪ್ರತ್ಯಾಗಮ stmts;
}
