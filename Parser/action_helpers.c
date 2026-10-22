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
    ಹಿಂತಿರುಗಿ &_PyRuntime.parser.dummy_name;
}

/* Creates a single-element asdl_seq* that contains a */
asdl_seq *
_PyPegen_singleton_seq(Parser *p, ಶೂನ್ಯ *a)
{
    ಪ್ರತಿಜ್ಞಾ(a != ಖಾಲಿ);
    asdl_seq *seq = (asdl_seq*)_Py_asdl_generic_seq_new(1, p->arena);
    ಆದರೆ (!seq) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    asdl_seq_ಗಣ_UNTYPED(seq, 0, a);
    ಹಿಂತಿರುಗಿ seq;
}

/* Creates a copy of seq and prepends a to it */
asdl_seq *
Py_Pegen_seq_ಸೇರಿಸು_ಒಳ_front(Parser *p, ಶೂನ್ಯ *a, asdl_seq *seq)
{
    ಪ್ರತಿಜ್ಞಾ(a != ಖಾಲಿ);
    ಆದರೆ (!seq) {
        ಹಿಂತಿರುಗಿ _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    ಆದರೆ (!new_seq) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    asdl_seq_ಗಣ_UNTYPED(new_seq, 0, a);
    ಗಾಗಿ (Py_ssize_t i = 1, l = asdl_seq_LEN(new_seq); i < l; i++) {
        asdl_seq_ಗಣ_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i - 1));
    }
    ಹಿಂತಿರುಗಿ new_seq;
}

/* Creates a copy of seq and appends a to it */
asdl_seq *
Py_Pegen_seq_append_ಗಮ್ಯ_ಕೊನೆ(Parser *p, asdl_seq *seq, ಶೂನ್ಯ *a)
{
    ಪ್ರತಿಜ್ಞಾ(a != ಖಾಲಿ);
    ಆದರೆ (!seq) {
        ಹಿಂತಿರುಗಿ _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    ಆದರೆ (!new_seq) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಗಾಗಿ (Py_ssize_t i = 0, l = asdl_seq_LEN(new_seq); i + 1 < l; i++) {
        asdl_seq_ಗಣ_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i));
    }
    asdl_seq_ಗಣ_UNTYPED(new_seq, asdl_seq_LEN(new_seq) - 1, a);
    ಹಿಂತಿರುಗಿ new_seq;
}

ಸ್ಥಿರ Py_ssize_t
get_flattened_seq_ಗಾತ್ರ(asdl_seq *seqs)
{
    Py_ssize_t ಗಾತ್ರ_2 = 0;
    ಗಾಗಿ (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        ಗಾತ್ರ_2 += asdl_seq_LEN(inner_seq);
    }
    ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2;
}

/* Flattens an asdl_seq* of asdl_seq*s */
asdl_seq *
_PyPegen_seq_flatten(Parser *p, asdl_seq *seqs)
{
    Py_ssize_t flattened_seq_ಗಾತ್ರ = get_flattened_seq_ಗಾತ್ರ(seqs);
    ಪ್ರತಿಜ್ಞಾ(flattened_seq_ಗಾತ್ರ > 0);

    asdl_seq *flattened_seq = (asdl_seq*)_Py_asdl_generic_seq_new(flattened_seq_ಗಾತ್ರ, p->arena);
    ಆದರೆ (!flattened_seq) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಪೂರ್ಣಾಂಕ flattened_seq_idx = 0;
    ಗಾಗಿ (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        ಗಾಗಿ (Py_ssize_t j = 0, li = asdl_seq_LEN(inner_seq); j < li; j++) {
            asdl_seq_ಗಣ_UNTYPED(flattened_seq, flattened_seq_idx++, asdl_seq_GET_UNTYPED(inner_seq, j));
        }
    }
    ಪ್ರತಿಜ್ಞಾ(flattened_seq_idx == flattened_seq_ಗಾತ್ರ);

    ಹಿಂತಿರುಗಿ flattened_seq;
}

ಶೂನ್ಯ *
Py_Pegen_seq_ಕೊನೆಯ_ಅಂಶ(asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ಹಿಂತಿರುಗಿ asdl_seq_GET_UNTYPED(seq, len - 1);
}

ಶೂನ್ಯ *
Py_Pegen_seq_ಮೊದಲ_ಅಂಶ(asdl_seq *seq)
{
    ಹಿಂತಿರುಗಿ asdl_seq_GET_UNTYPED(seq, 0);
}

/* Creates a new name of the form <first_name>.<second_name> */
ಅಭಿವ್ಯಕ್ತಿ_ty
Py_Pegen_join_ಹೆಸರುಗಳು_with_dot(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty ಮೊದಲ_name, ಅಭಿವ್ಯಕ್ತಿ_ty ಎರಡನೇ_name)
{
    ಪ್ರತಿಜ್ಞಾ(ಮೊದಲ_name != ಖಾಲಿ && ಎರಡನೇ_name != ಖಾಲಿ);
    PyObject *uni = PyUnicode_FromFormat("%U.%U",
            ಮೊದಲ_name->v.Name.id, ಎರಡನೇ_name->v.Name.id);
    ಆದರೆ (!uni) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &uni);
    ಆದರೆ (Py_Arena_ಕೂಡಿಸು_Py_Object(p->arena, uni) < 0) {
        Py_DECREF(uni);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ _PyAST_Name(uni, ತುಂಬು, EXTRA_ಅಭಿವ್ಯಕ್ತಿ(ಮೊದಲ_name, ಎರಡನೇ_name));
}

/* Counts the total number of dots in seq's tokens */
ಪೂರ್ಣಾಂಕ
Py_Pegen_seq_ಎಣಿಕೆ_dots(asdl_seq *seq)
{
    ಪೂರ್ಣಾಂಕ ಸಂಖ್ಯೆ_ದ_dots = 0;
    ಗಾಗಿ (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        Token *current_ಅಭಿವ್ಯಕ್ತಿ = asdl_seq_GET_UNTYPED(seq, i);
        ಆಯ್ಕೆ (current_ಅಭಿವ್ಯಕ್ತಿ->type) {
            ಸ್ಥಿತಿ ELLIPSIS:
                ಸಂಖ್ಯೆ_ದ_dots += 3;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ DOT:
                ಸಂಖ್ಯೆ_ದ_dots += 1;
                ನಿಲ್ಲಿಸು;
            ಪೂರ್ವನಿಯೋಜಿತ:
                Py_UNREACHABLE();
        }
    }

    ಹಿಂತಿರುಗಿ ಸಂಖ್ಯೆ_ದ_dots;
}

/* Creates an alias with '*' as the identifier name */
alias_ty
Py_Pegen_alias_for_ನಕ್ಷತ್ರ(Parser *p, ಪೂರ್ಣಾಂಕ lineno, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಕೊನೆ_lineno,
                        ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, PyArena *arena) {
    PyObject *str = PyUnicode_InternFromString("*");
    ಆದರೆ (!str) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (Py_Arena_ಕೂಡಿಸು_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ _PyAST_alias(str, ಖಾಲಿ, lineno, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಕೊನೆ_lineno, ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, arena);
}

/* Creates a new asdl_seq* with the identifiers of all the names in seq */
asdl_identifier_seq *
Py_Pegen_map_ಹೆಸರುಗಳು_ಗಮ್ಯ_ids(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ಪ್ರತಿಜ್ಞಾ(len > 0);

    asdl_identifier_seq *new_seq = _Py_asdl_identifier_seq_new(len, p->arena);
    ಆದರೆ (!new_seq) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಗಾಗಿ (Py_ssize_t i = 0; i < len; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty e = asdl_seq_GET(seq, i);
        asdl_seq_ಗಣ(new_seq, i, e->v.Name.id);
    }
    ಹಿಂತಿರುಗಿ new_seq;
}

/* Constructs a CmpopExprPair */
Cmpop_ಅಭಿವ್ಯಕ್ತಿ_Pair *
Py_Pegen_cmpop_ಅಭಿವ್ಯಕ್ತಿ_pair(Parser *p, cmpop_ty cmpop, ಅಭಿವ್ಯಕ್ತಿ_ty ಅಭಿವ್ಯಕ್ತಿ)
{
    ಪ್ರತಿಜ್ಞಾ(ಅಭಿವ್ಯಕ್ತಿ != ಖಾಲಿ);
    Cmpop_ಅಭಿವ್ಯಕ್ತಿ_Pair *a = _PyArena_Malloc(p->arena, ಗಾತ್ರ(Cmpop_ಅಭಿವ್ಯಕ್ತಿ_Pair));
    ಆದರೆ (!a) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    a->cmpop = cmpop;
    a->ಅಭಿವ್ಯಕ್ತಿ = ಅಭಿವ್ಯಕ್ತಿ;
    ಹಿಂತಿರುಗಿ a;
}

asdl_int_seq *
_PyPegen_get_cmpops(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ಪ್ರತಿಜ್ಞಾ(len > 0);

    asdl_int_seq *new_seq = _Py_asdl_int_seq_new(len, p->arena);
    ಆದರೆ (!new_seq) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಗಾಗಿ (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_ಅಭಿವ್ಯಕ್ತಿ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_ಗಣ(new_seq, i, pair->cmpop);
    }
    ಹಿಂತಿರುಗಿ new_seq;
}

asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *
_PyPegen_get_exprs(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ಪ್ರತಿಜ್ಞಾ(len > 0);

    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *new_seq = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(len, p->arena);
    ಆದರೆ (!new_seq) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಗಾಗಿ (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_ಅಭಿವ್ಯಕ್ತಿ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_ಗಣ(new_seq, i, pair->ಅಭಿವ್ಯಕ್ತಿ);
    }
    ಹಿಂತಿರುಗಿ new_seq;
}

/* Creates an asdl_seq* where all the elements have been changed to have ctx as context */
ಸ್ಥಿರ asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *
ಗಣ_seq_context(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *seq, ಅಭಿವ್ಯಕ್ತಿ_context_ty ctx)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ಆದರೆ (len == 0) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *new_seq = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(len, p->arena);
    ಆದರೆ (!new_seq) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಗಾಗಿ (Py_ssize_t i = 0; i < len; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty e = asdl_seq_GET(seq, i);
        ಅಭಿವ್ಯಕ್ತಿ_ty new_e = Py_Pegen_ಗಣ_ಅಭಿವ್ಯಕ್ತಿ_context(p, e, ctx);
        ಆದರೆ (!new_e) {
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
        asdl_seq_ಗಣ(new_seq, i, new_e);
    }
    ಹಿಂತಿರುಗಿ new_seq;
}

ಸ್ಥಿರ ಅಭಿವ್ಯಕ್ತಿ_ty
ಗಣ_name_context(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty e, ಅಭಿವ್ಯಕ್ತಿ_context_ty ctx)
{
    ಹಿಂತಿರುಗಿ _PyAST_Name(e->v.Name.id, ctx, EXTRA_ಅಭಿವ್ಯಕ್ತಿ(e, e));
}

ಸ್ಥಿರ ಅಭಿವ್ಯಕ್ತಿ_ty
ಗಣ_tuple_context(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty e, ಅಭಿವ್ಯಕ್ತಿ_context_ty ctx)
{
    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *seq = ಗಣ_seq_context(p, e->v.Tuple.elts, ctx);
    ಆದರೆ (!seq && PyErr_Occurred()) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ _PyAST_Tuple(seq, ctx, EXTRA_ಅಭಿವ್ಯಕ್ತಿ(e, e));
}

ಸ್ಥಿರ ಅಭಿವ್ಯಕ್ತಿ_ty
ಗಣ_list_context(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty e, ಅಭಿವ್ಯಕ್ತಿ_context_ty ctx)
{
    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *seq = ಗಣ_seq_context(p, e->v.List.elts, ctx);
    ಆದರೆ (!seq && PyErr_Occurred()) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ _PyAST_List(seq, ctx, EXTRA_ಅಭಿವ್ಯಕ್ತಿ(e, e));
}

ಸ್ಥಿರ ಅಭಿವ್ಯಕ್ತಿ_ty
ಗಣ_subscript_context(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty e, ಅಭಿವ್ಯಕ್ತಿ_context_ty ctx)
{
    ಹಿಂತಿರುಗಿ _PyAST_Subscript(e->v.Subscript.ಮೌಲ್ಯ, e->v.Subscript.ತುಂಡು,
                            ctx, EXTRA_ಅಭಿವ್ಯಕ್ತಿ(e, e));
}

ಸ್ಥಿರ ಅಭಿವ್ಯಕ್ತಿ_ty
ಗಣ_attribute_context(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty e, ಅಭಿವ್ಯಕ್ತಿ_context_ty ctx)
{
    ಹಿಂತಿರುಗಿ _PyAST_Attribute(e->v.Attribute.ಮೌಲ್ಯ, e->v.Attribute.attr,
                            ctx, EXTRA_ಅಭಿವ್ಯಕ್ತಿ(e, e));
}

ಸ್ಥಿರ ಅಭಿವ್ಯಕ್ತಿ_ty
ಗಣ_starred_context(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty e, ಅಭಿವ್ಯಕ್ತಿ_context_ty ctx)
{
    ಅಭಿವ್ಯಕ್ತಿ_ty inner = Py_Pegen_ಗಣ_ಅಭಿವ್ಯಕ್ತಿ_context(p, e->v.Starred.ಮೌಲ್ಯ, ctx);
    ಆದರೆ (!inner) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ _PyAST_Starred(inner, ctx, EXTRA_ಅಭಿವ್ಯಕ್ತಿ(e, e));
}

/* Creates an `expr_ty` equivalent to `expr` but with `ctx` as context */
ಅಭಿವ್ಯಕ್ತಿ_ty
Py_Pegen_ಗಣ_ಅಭಿವ್ಯಕ್ತಿ_context(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty ಅಭಿವ್ಯಕ್ತಿ, ಅಭಿವ್ಯಕ್ತಿ_context_ty ctx)
{
    ಪ್ರತಿಜ್ಞಾ(ಅಭಿವ್ಯಕ್ತಿ != ಖಾಲಿ);

    ಅಭಿವ್ಯಕ್ತಿ_ty new = ಖಾಲಿ;
    ಆಯ್ಕೆ (ಅಭಿವ್ಯಕ್ತಿ->ಬಗೆ) {
        ಸ್ಥಿತಿ Name_ಬಗೆ:
            new = ಗಣ_name_context(p, ಅಭಿವ್ಯಕ್ತಿ, ctx);
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ Tuple_ಬಗೆ:
            new = ಗಣ_tuple_context(p, ಅಭಿವ್ಯಕ್ತಿ, ctx);
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ List_ಬಗೆ:
            new = ಗಣ_list_context(p, ಅಭಿವ್ಯಕ್ತಿ, ctx);
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ Subscript_ಬಗೆ:
            new = ಗಣ_subscript_context(p, ಅಭಿವ್ಯಕ್ತಿ, ctx);
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ Attribute_ಬಗೆ:
            new = ಗಣ_attribute_context(p, ಅಭಿವ್ಯಕ್ತಿ, ctx);
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ Starred_ಬಗೆ:
            new = ಗಣ_starred_context(p, ಅಭಿವ್ಯಕ್ತಿ, ctx);
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            new = ಅಭಿವ್ಯಕ್ತಿ;
    }
    ಹಿಂತಿರುಗಿ new;
}

/* Invalid targets return NULL without raising, allowing parsing to backtrack.
   Copy valid targets to preserve the memoized expression's context. */
ಅಭಿವ್ಯಕ್ತಿ_ty
Py_Pegen_make_ಗುರಿ(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty ಅಭಿವ್ಯಕ್ತಿ, TARGETS_TYPE targets_type)
{
    ಪ್ರತಿಜ್ಞಾ(ಅಭಿವ್ಯಕ್ತಿ != ಖಾಲಿ);
    ಪ್ರತಿಜ್ಞಾ(targets_type != FOR_TARGETS);
    ಆದರೆ (Py_Pegen_get_invalid_ಗುರಿ(ಅಭಿವ್ಯಕ್ತಿ, targets_type) != ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ Py_Pegen_ಗಣ_ಅಭಿವ್ಯಕ್ತಿ_context(
        p, ಅಭಿವ್ಯಕ್ತಿ, targets_type == DEL_TARGETS ? Del : Store);
}

/* Constructs a KeyValuePair that is used when parsing a dict's key value pairs */
ಕೀಲಿ_ಮೌಲ್ಯ_Pair *
Py_Pegen_ಕೀಲಿ_ಮೌಲ್ಯ_pair(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty ಕೀಲಿ, ಅಭಿವ್ಯಕ್ತಿ_ty ಮೌಲ್ಯ)
{
    ಕೀಲಿ_ಮೌಲ್ಯ_Pair *a = _PyArena_Malloc(p->arena, ಗಾತ್ರ(ಕೀಲಿ_ಮೌಲ್ಯ_Pair));
    ಆದರೆ (!a) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    a->ಕೀಲಿ = ಕೀಲಿ;
    a->ಮೌಲ್ಯ = ಮೌಲ್ಯ;
    ಹಿಂತಿರುಗಿ a;
}

/* Extracts all keys from an asdl_seq* of KeyValuePair*'s */
asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *
Py_Pegen_get_ಕೀಲಿಗಳು(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *new_seq = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(len, p->arena);
    ಆದರೆ (!new_seq) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಗಾಗಿ (Py_ssize_t i = 0; i < len; i++) {
        ಕೀಲಿ_ಮೌಲ್ಯ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_ಗಣ(new_seq, i, pair->ಕೀಲಿ);
    }
    ಹಿಂತಿರುಗಿ new_seq;
}

/* Extracts all values from an asdl_seq* of KeyValuePair*'s */
asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *
Py_Pegen_get_ಮೌಲ್ಯಗಳು(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *new_seq = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(len, p->arena);
    ಆದರೆ (!new_seq) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಗಾಗಿ (Py_ssize_t i = 0; i < len; i++) {
        ಕೀಲಿ_ಮೌಲ್ಯ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_ಗಣ(new_seq, i, pair->ಮೌಲ್ಯ);
    }
    ಹಿಂತಿರುಗಿ new_seq;
}

/* Constructs a KeyPatternPair that is used when parsing mapping & class patterns */
ಕೀಲಿ_ನಮೂನೆ_Pair *
Py_Pegen_ಕೀಲಿ_ನಮೂನೆ_pair(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty ಕೀಲಿ, ನಮೂನೆ_ty ನಮೂನೆ)
{
    ಕೀಲಿ_ನಮೂನೆ_Pair *a = _PyArena_Malloc(p->arena, ಗಾತ್ರ(ಕೀಲಿ_ನಮೂನೆ_Pair));
    ಆದರೆ (!a) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    a->ಕೀಲಿ = ಕೀಲಿ;
    a->ನಮೂನೆ = ನಮೂನೆ;
    ಹಿಂತಿರುಗಿ a;
}

/* Extracts all keys from an asdl_seq* of KeyPatternPair*'s */
asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *
Py_Pegen_get_ನಮೂನೆ_ಕೀಲಿಗಳು(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *new_seq = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(len, p->arena);
    ಆದರೆ (!new_seq) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಗಾಗಿ (Py_ssize_t i = 0; i < len; i++) {
        ಕೀಲಿ_ನಮೂನೆ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_ಗಣ(new_seq, i, pair->ಕೀಲಿ);
    }
    ಹಿಂತಿರುಗಿ new_seq;
}

/* Extracts all patterns from an asdl_seq* of KeyPatternPair*'s */
asdl_ನಮೂನೆ_seq *
_PyPegen_get_patterns(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_ನಮೂನೆ_seq *new_seq = Py_asdl_ನಮೂನೆ_seq_new(len, p->arena);
    ಆದರೆ (!new_seq) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಗಾಗಿ (Py_ssize_t i = 0; i < len; i++) {
        ಕೀಲಿ_ನಮೂನೆ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_ಗಣ(new_seq, i, pair->ನಮೂನೆ);
    }
    ಹಿಂತಿರುಗಿ new_seq;
}

/* Constructs a NameDefaultPair */
NameDefaultPair *
_PyPegen_name_default_pair(Parser *p, arg_ty arg, ಅಭಿವ್ಯಕ್ತಿ_ty ಮೌಲ್ಯ, Token *tc)
{
    NameDefaultPair *a = _PyArena_Malloc(p->arena, ಗಾತ್ರ(NameDefaultPair));
    ಆದರೆ (!a) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    a->arg = Py_Pegen_ಕೂಡಿಸು_type_comment_ಗಮ್ಯ_arg(p, arg, tc);
    ಆದರೆ (!a->arg) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    a->ಮೌಲ್ಯ = ಮೌಲ್ಯ;
    ಹಿಂತಿರುಗಿ a;
}

/* Constructs a SlashWithDefault */
SlashWithDefault *
_PyPegen_slash_with_default(Parser *p, asdl_arg_seq *plain_ಹೆಸರುಗಳು, asdl_seq *ಹೆಸರುಗಳು_with_defaults)
{
    SlashWithDefault *a = _PyArena_Malloc(p->arena, ಗಾತ್ರ(SlashWithDefault));
    ಆದರೆ (!a) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    a->plain_ಹೆಸರುಗಳು = plain_ಹೆಸರುಗಳು;
    a->ಹೆಸರುಗಳು_with_defaults = ಹೆಸರುಗಳು_with_defaults;
    ಹಿಂತಿರುಗಿ a;
}

/* Constructs a StarEtc */
ನಕ್ಷತ್ರ_Etc *
Py_Pegen_ನಕ್ಷತ್ರ_etc(Parser *p, arg_ty vararg, asdl_seq *kwonlyargs, arg_ty kwarg)
{
    ನಕ್ಷತ್ರ_Etc *a = _PyArena_Malloc(p->arena, ಗಾತ್ರ(ನಕ್ಷತ್ರ_Etc));
    ಆದರೆ (!a) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    a->vararg = vararg;
    a->kwonlyargs = kwonlyargs;
    a->kwarg = kwarg;
    ಹಿಂತಿರುಗಿ a;
}

asdl_seq *
_PyPegen_join_sequences(Parser *p, asdl_seq *a, asdl_seq *b)
{
    Py_ssize_t ಮೊದಲ_len = asdl_seq_LEN(a);
    Py_ssize_t ಎರಡನೇ_len = asdl_seq_LEN(b);
    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(ಮೊದಲ_len + ಎರಡನೇ_len, p->arena);
    ಆದರೆ (!new_seq) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಪೂರ್ಣಾಂಕ k = 0;
    ಗಾಗಿ (Py_ssize_t i = 0; i < ಮೊದಲ_len; i++) {
        asdl_seq_ಗಣ_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(a, i));
    }
    ಗಾಗಿ (Py_ssize_t i = 0; i < ಎರಡನೇ_len; i++) {
        asdl_seq_ಗಣ_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(b, i));
    }

    ಹಿಂತಿರುಗಿ new_seq;
}

ಸ್ಥಿರ asdl_arg_seq*
get_ಹೆಸರುಗಳು(Parser *p, asdl_seq *ಹೆಸರುಗಳು_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(ಹೆಸರುಗಳು_with_defaults);
    asdl_arg_seq *seq = _Py_asdl_arg_seq_new(len, p->arena);
    ಆದರೆ (!seq) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಗಾಗಿ (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(ಹೆಸರುಗಳು_with_defaults, i);
        asdl_seq_ಗಣ(seq, i, pair->arg);
    }
    ಹಿಂತಿರುಗಿ seq;
}

ಸ್ಥಿರ asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *
_get_defaults(Parser *p, asdl_seq *ಹೆಸರುಗಳು_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(ಹೆಸರುಗಳು_with_defaults);
    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *seq = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(len, p->arena);
    ಆದರೆ (!seq) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಗಾಗಿ (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(ಹೆಸರುಗಳು_with_defaults, i);
        asdl_seq_ಗಣ(seq, i, pair->ಮೌಲ್ಯ);
    }
    ಹಿಂತಿರುಗಿ seq;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
_make_posonlyargs(Parser *p,
                  asdl_arg_seq *slash_without_default,
                  SlashWithDefault *slash_with_default,
                  asdl_arg_seq **posonlyargs) {
    ಆದರೆ (slash_without_default != ಖಾಲಿ) {
        *posonlyargs = slash_without_default;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (slash_with_default != ಖಾಲಿ) {
        asdl_arg_seq *slash_with_default_ಹೆಸರುಗಳು =
                get_ಹೆಸರುಗಳು(p, slash_with_default->ಹೆಸರುಗಳು_with_defaults);
        ಆದರೆ (!slash_with_default_ಹೆಸರುಗಳು) {
            ಹಿಂತಿರುಗಿ -1;
        }
        *posonlyargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default->plain_ಹೆಸರುಗಳು,
                (asdl_seq*)slash_with_default_ಹೆಸರುಗಳು);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }
    ಹಿಂತಿರುಗಿ *posonlyargs == ಖಾಲಿ ? -1 : 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
_make_posargs(Parser *p,
              asdl_arg_seq *plain_ಹೆಸರುಗಳು,
              asdl_seq *ಹೆಸರುಗಳು_with_default,
              asdl_arg_seq **posargs) {

    ಆದರೆ (ಹೆಸರುಗಳು_with_default != ಖಾಲಿ) {
        ಆದರೆ (plain_ಹೆಸರುಗಳು != ಖಾಲಿ) {
            asdl_arg_seq *ಹೆಸರುಗಳು_with_default_ಹೆಸರುಗಳು = get_ಹೆಸರುಗಳು(p, ಹೆಸರುಗಳು_with_default);
            ಆದರೆ (!ಹೆಸರುಗಳು_with_default_ಹೆಸರುಗಳು) {
                ಹಿಂತಿರುಗಿ -1;
            }
            *posargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                    p,(asdl_seq*)plain_ಹೆಸರುಗಳು, (asdl_seq*)ಹೆಸರುಗಳು_with_default_ಹೆಸರುಗಳು);
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            *posargs = get_ಹೆಸರುಗಳು(p, ಹೆಸರುಗಳು_with_default);
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ (plain_ಹೆಸರುಗಳು != ಖಾಲಿ) {
            // With the current grammar, we never get here.
            // If that has changed, remove the assert, and test thoroughly.
            ಪ್ರತಿಜ್ಞಾ(0);
            *posargs = plain_ಹೆಸರುಗಳು;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            *posargs = _Py_asdl_arg_seq_new(0, p->arena);
        }
    }
    ಹಿಂತಿರುಗಿ *posargs == ಖಾಲಿ ? -1 : 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
_make_posdefaults(Parser *p,
                  SlashWithDefault *slash_with_default,
                  asdl_seq *ಹೆಸರುಗಳು_with_default,
                  asdl_ಅಭಿವ್ಯಕ್ತಿ_seq **posdefaults) {
    ಆದರೆ (slash_with_default != ಖಾಲಿ && ಹೆಸರುಗಳು_with_default != ಖಾಲಿ) {
        asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *slash_with_default_ಮೌಲ್ಯಗಳು =
                _get_defaults(p, slash_with_default->ಹೆಸರುಗಳು_with_defaults);
        ಆದರೆ (!slash_with_default_ಮೌಲ್ಯಗಳು) {
            ಹಿಂತಿರುಗಿ -1;
        }
        asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *ಹೆಸರುಗಳು_with_default_ಮೌಲ್ಯಗಳು = _get_defaults(p, ಹೆಸರುಗಳು_with_default);
        ಆದರೆ (!ಹೆಸರುಗಳು_with_default_ಮೌಲ್ಯಗಳು) {
            ಹಿಂತಿರುಗಿ -1;
        }
        *posdefaults = (asdl_ಅಭಿವ್ಯಕ್ತಿ_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default_ಮೌಲ್ಯಗಳು,
                (asdl_seq*)ಹೆಸರುಗಳು_with_default_ಮೌಲ್ಯಗಳು);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (slash_with_default == ಖಾಲಿ && ಹೆಸರುಗಳು_with_default != ಖಾಲಿ) {
        *posdefaults = _get_defaults(p, ಹೆಸರುಗಳು_with_default);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (slash_with_default != ಖಾಲಿ && ಹೆಸರುಗಳು_with_default == ಖಾಲಿ) {
        *posdefaults = _get_defaults(p, slash_with_default->ಹೆಸರುಗಳು_with_defaults);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        *posdefaults = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(0, p->arena);
    }
    ಹಿಂತಿರುಗಿ *posdefaults == ಖಾಲಿ ? -1 : 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
_make_kwargs(Parser *p, ನಕ್ಷತ್ರ_Etc *ನಕ್ಷತ್ರ_etc,
             asdl_arg_seq **kwonlyargs,
             asdl_ಅಭಿವ್ಯಕ್ತಿ_seq **kwdefaults) {
    ಆದರೆ (ನಕ್ಷತ್ರ_etc != ಖಾಲಿ && ನಕ್ಷತ್ರ_etc->kwonlyargs != ಖಾಲಿ) {
        *kwonlyargs = get_ಹೆಸರುಗಳು(p, ನಕ್ಷತ್ರ_etc->kwonlyargs);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }

    ಆದರೆ (*kwonlyargs == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ (ನಕ್ಷತ್ರ_etc != ಖಾಲಿ && ನಕ್ಷತ್ರ_etc->kwonlyargs != ಖಾಲಿ) {
        *kwdefaults = _get_defaults(p, ನಕ್ಷತ್ರ_etc->kwonlyargs);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        *kwdefaults = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(0, p->arena);
    }

    ಆದರೆ (*kwdefaults == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ -1;
    }

    ಹಿಂತಿರುಗಿ 0;
}

/* Constructs an arguments_ty object out of all the parsed constructs in the parameters rule */
arguments_ty
_PyPegen_make_arguments(Parser *p, asdl_arg_seq *slash_without_default,
                        SlashWithDefault *slash_with_default, asdl_arg_seq *plain_ಹೆಸರುಗಳು,
                        asdl_seq *ಹೆಸರುಗಳು_with_default, ನಕ್ಷತ್ರ_Etc *ನಕ್ಷತ್ರ_etc)
{
    asdl_arg_seq *posonlyargs;
    ಆದರೆ (_make_posonlyargs(p, slash_without_default, slash_with_default, &posonlyargs) == -1) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    asdl_arg_seq *posargs;
    ಆದರೆ (_make_posargs(p, plain_ಹೆಸರುಗಳು, ಹೆಸರುಗಳು_with_default, &posargs) == -1) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *posdefaults;
    ಆದರೆ (_make_posdefaults(p,slash_with_default, ಹೆಸರುಗಳು_with_default, &posdefaults) == -1) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    arg_ty vararg = ಖಾಲಿ;
    ಆದರೆ (ನಕ್ಷತ್ರ_etc != ಖಾಲಿ && ನಕ್ಷತ್ರ_etc->vararg != ಖಾಲಿ) {
        vararg = ನಕ್ಷತ್ರ_etc->vararg;
    }

    asdl_arg_seq *kwonlyargs;
    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *kwdefaults;
    ಆದರೆ (_make_kwargs(p, ನಕ್ಷತ್ರ_etc, &kwonlyargs, &kwdefaults) == -1) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    arg_ty kwarg = ಖಾಲಿ;
    ಆದರೆ (ನಕ್ಷತ್ರ_etc != ಖಾಲಿ && ನಕ್ಷತ್ರ_etc->kwarg != ಖಾಲಿ) {
        kwarg = ನಕ್ಷತ್ರ_etc->kwarg;
    }

    ಹಿಂತಿರುಗಿ _PyAST_arguments(posonlyargs, posargs, vararg, kwonlyargs,
                            kwdefaults, kwarg, posdefaults, p->arena);
}


/* Constructs an empty arguments_ty object, that gets used when a function accepts no
 * arguments. */
arguments_ty
Py_Pegen_ಖಾಲಿಗಳು_arguments(Parser *p)
{
    asdl_arg_seq *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    ಆದರೆ (!posonlyargs) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    asdl_arg_seq *posargs = _Py_asdl_arg_seq_new(0, p->arena);
    ಆದರೆ (!posargs) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *posdefaults = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(0, p->arena);
    ಆದರೆ (!posdefaults) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    asdl_arg_seq *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    ಆದರೆ (!kwonlyargs) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *kwdefaults = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(0, p->arena);
    ಆದರೆ (!kwdefaults) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ _PyAST_arguments(posonlyargs, posargs, ಖಾಲಿ, kwonlyargs,
                            kwdefaults, ಖಾಲಿ, posdefaults, p->arena);
}

/* Encapsulates the value of an operator_ty into an AugOperator struct */
AugOperator *
_PyPegen_augoperator(Parser *p, operator_ty ಬಗೆ)
{
    AugOperator *a = _PyArena_Malloc(p->arena, ಗಾತ್ರ(AugOperator));
    ಆದರೆ (!a) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    a->ಬಗೆ = ಬಗೆ;
    ಹಿಂತಿರುಗಿ a;
}

/* Construct a FunctionDef equivalent to function_def, but with decorators */
stmt_ty
_PyPegen_function_def_decorators(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *decorators, stmt_ty function_def)
{
    ಪ್ರತಿಜ್ಞಾ(function_def != ಖಾಲಿ);
    ಆದರೆ (function_def->ಬಗೆ == Async_Function_Def_ಬಗೆ) {
        ಹಿಂತಿರುಗಿ _PyAST_AsyncFunctionDef(
            function_def->v.AsyncFunctionDef.name,
            function_def->v.AsyncFunctionDef.ವಾದಗಳು,
            function_def->v.AsyncFunctionDef.body, decorators,
            function_def->v.AsyncFunctionDef.returns,
            function_def->v.AsyncFunctionDef.type_comment,
            function_def->v.AsyncFunctionDef.type_params,
            function_def->lineno, function_def->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
            function_def->ಕೊನೆ_lineno, function_def->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, p->arena);
    }

    ಹಿಂತಿರುಗಿ _PyAST_FunctionDef(
        function_def->v.FunctionDef.name,
        function_def->v.FunctionDef.ವಾದಗಳು,
        function_def->v.FunctionDef.body, decorators,
        function_def->v.FunctionDef.returns,
        function_def->v.FunctionDef.type_comment,
        function_def->v.FunctionDef.type_params,
        function_def->lineno, function_def->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
        function_def->ಕೊನೆ_lineno, function_def->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, p->arena);
}

/* Construct a ClassDef equivalent to class_def, but with decorators */
stmt_ty
_PyPegen_class_def_decorators(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *decorators, stmt_ty class_def)
{
    ಪ್ರತಿಜ್ಞಾ(class_def != ಖಾಲಿ);
    ಹಿಂತಿರುಗಿ _PyAST_ClassDef(
        class_def->v.ClassDef.name,
        class_def->v.ClassDef.bases, class_def->v.ClassDef.keywords,
        class_def->v.ClassDef.body, decorators,
        class_def->v.ClassDef.type_params,
        class_def->lineno, class_def->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, class_def->ಕೊನೆ_lineno,
        class_def->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, p->arena);
}

/* Construct a KeywordOrStarred */
KeywordOrStarred *
_PyPegen_keyword_or_starred(Parser *p, ಶೂನ್ಯ *element, ಪೂರ್ಣಾಂಕ is_keyword)
{
    KeywordOrStarred *a = _PyArena_Malloc(p->arena, ಗಾತ್ರ(KeywordOrStarred));
    ಆದರೆ (!a) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    a->element = element;
    a->is_keyword = is_keyword;
    ಹಿಂತಿರುಗಿ a;
}

/* Get the number of starred expressions in an asdl_seq* of KeywordOrStarred*s */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
seq_ಸಂಖ್ಯೆ_ದ_starred_exprs(asdl_seq *seq)
{
    ಪೂರ್ಣಾಂಕ n = 0;
    ಗಾಗಿ (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(seq, i);
        ಆದರೆ (!k->is_keyword) {
            n++;
        }
    }
    ಹಿಂತಿರುಗಿ n;
}

/* Extract the starred expressions of an asdl_seq* of KeywordOrStarred*s */
asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *
Py_Pegen_seq_ಹೊರತೆಗೆ_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    ಪೂರ್ಣಾಂಕ new_len = seq_ಸಂಖ್ಯೆ_ದ_starred_exprs(kwargs);
    ಆದರೆ (new_len == 0) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *new_seq = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(new_len, p->arena);
    ಆದರೆ (!new_seq) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಪೂರ್ಣಾಂಕ idx = 0;
    ಗಾಗಿ (Py_ssize_t i = 0, len = asdl_seq_LEN(kwargs); i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        ಆದರೆ (!k->is_keyword) {
            asdl_seq_ಗಣ(new_seq, idx++, k->element);
        }
    }
    ಹಿಂತಿರುಗಿ new_seq;
}

/* Return a new asdl_seq* with only the keywords in kwargs */
asdl_keyword_seq*
_PyPegen_seq_delete_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    Py_ssize_t len = asdl_seq_LEN(kwargs);
    Py_ssize_t new_len = len - seq_ಸಂಖ್ಯೆ_ದ_starred_exprs(kwargs);
    ಆದರೆ (new_len == 0) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    asdl_keyword_seq *new_seq = _Py_asdl_keyword_seq_new(new_len, p->arena);
    ಆದರೆ (!new_seq) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಪೂರ್ಣಾಂಕ idx = 0;
    ಗಾಗಿ (Py_ssize_t i = 0; i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        ಆದರೆ (k->is_keyword) {
            asdl_seq_ಗಣ(new_seq, idx++, k->element);
        }
    }
    ಹಿಂತಿರುಗಿ new_seq;
}

ಅಭಿವ್ಯಕ್ತಿ_ty
_PyPegen_ensure_imaginary(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty ಘಾತಾಂಕ)
{
    ಆದರೆ (ಘಾತಾಂಕ->ಬಗೆ != Constant_ಬಗೆ || !PyComplex_CheckExact(ಘಾತಾಂಕ->v.Constant.ಮೌಲ್ಯ)) {
        RAISE_SYNTAX_ERROR_ತಿಳಿದ_LOCATION(ಘಾತಾಂಕ, "imaginary number required in complex literal");
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ ಘಾತಾಂಕ;
}

ಅಭಿವ್ಯಕ್ತಿ_ty
_PyPegen_ensure_real(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty ಘಾತಾಂಕ)
{
    ಆದರೆ (ಘಾತಾಂಕ->ಬಗೆ != Constant_ಬಗೆ || PyComplex_CheckExact(ಘಾತಾಂಕ->v.Constant.ಮೌಲ್ಯ)) {
        RAISE_SYNTAX_ERROR_ತಿಳಿದ_LOCATION(ಘಾತಾಂಕ, "real number required in complex literal");
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ ಘಾತಾಂಕ;
}

mod_ty
_PyPegen_make_module(Parser *p, asdl_stmt_seq *a) {
    asdl_type_ignore_seq *type_ignores = ಖಾಲಿ;
    Py_ssize_t ಅಂಕೆ = p->type_ignore_comments.ಅಂಕೆ_ವಸ್ತುಗಳು;
    ಆದರೆ (ಅಂಕೆ > 0) {
        // Turn the raw (comment, lineno) pairs into TypeIgnore objects in the arena
        type_ignores = _Py_asdl_type_ignore_seq_new(ಅಂಕೆ, p->arena);
        ಆದರೆ (type_ignores == ಖಾಲಿ) {
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
        ಗಾಗಿ (Py_ssize_t i = 0; i < ಅಂಕೆ; i++) {
            PyObject *ಟ್ಯಾಗ್ = _PyPegen_new_type_comment(p, p->type_ignore_comments.ವಸ್ತುಗಳು[i].comment);
            ಆದರೆ (ಟ್ಯಾಗ್ == ಖಾಲಿ) {
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
            }
            type_ignore_ty ti = _PyAST_TypeIgnore(p->type_ignore_comments.ವಸ್ತುಗಳು[i].lineno,
                                                  ಟ್ಯಾಗ್, p->arena);
            ಆದರೆ (ti == ಖಾಲಿ) {
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
            }
            asdl_seq_ಗಣ(type_ignores, i, ti);
        }
    }
    ಹಿಂತಿರುಗಿ _PyAST_Module(a, type_ignores, p->arena);
}

PyObject *
_PyPegen_new_type_comment(Parser *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s), ಖಾಲಿ);
    ಆದರೆ (res == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (Py_Arena_ಕೂಡಿಸು_Py_Object(p->arena, res) < 0) {
        Py_DECREF(res);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ res;
}

arg_ty
Py_Pegen_ಕೂಡಿಸು_type_comment_ಗಮ್ಯ_arg(Parser *p, arg_ty a, Token *tc)
{
    ಆದರೆ (tc == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ a;
    }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೈಟುಗಳು = Py_ಬೈಟುಗಳು_As_String(tc->ಬೈಟುಗಳು);
    ಆದರೆ (ಬೈಟುಗಳು == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, ಬೈಟುಗಳು);
    ಆದರೆ (tco == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ _PyAST_arg(a->arg, a->annotation, tco,
                      a->lineno, a->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, a->ಕೊನೆ_lineno, a->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                      p->arena);
}

/* Checks if the NOTEQUAL token is valid given the current parser flags
0 indicates success and nonzero indicates failure (an exception may be set) */
ಪೂರ್ಣಾಂಕ
_PyPegen_check_barry_as_flufl(Parser *p, Token* t) {
    ಪ್ರತಿಜ್ಞಾ(t->ಬೈಟುಗಳು != ಖಾಲಿ);
    ಪ್ರತಿಜ್ಞಾ(t->type == NOTEQUAL);

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* tok_str = Py_ಬೈಟುಗಳು_AS_STRING(t->ಬೈಟುಗಳು);
    ಆದರೆ (p->flags & PyPARSE_BARRY_AS_BDFL && strcmp(tok_str, "<>") != 0) {
        RAISE_SYNTAX_ERROR("with Barry as BDFL, use '<>' instead of '!='");
        ಹಿಂತಿರುಗಿ -1;
    }
    ಆದರೆ (!(p->flags & PyPARSE_BARRY_AS_BDFL)) {
        ಹಿಂತಿರುಗಿ strcmp(tok_str, "!=");
    }
    ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ
_PyPegen_check_legacy_stmt(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty name) {
    ಆದರೆ (name->ಬಗೆ != Name_ಬಗೆ) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* candidates[2] = {"print", "exec"};
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i=0; i<2; i++) {
        ಆದರೆ (PyUnicode_CompareWithASCIIString(name->v.Name.id, candidates[i]) == 0) {
            ಹಿಂತಿರುಗಿ 1;
        }
    }
    ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ *
_PyPegen_raise_error_for_missing_comma(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty a, ಅಭಿವ್ಯಕ್ತಿ_ty b)
{
    // Don't raise for legacy statements like "print x" or "exec x"
    ಆದರೆ (_PyPegen_check_legacy_stmt(p, a)) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    // Only raise inside parentheses/brackets (level > 0)
    ಆದರೆ (p->ಟೋಕನುಗಳು[p->mark - 1]->level == 0) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    // For multi-line expressions (like string concatenations), point to the
    // last line instead of the first for a more helpful error message.
    // Use a->col_offset as the starting column since all strings in the
    // concatenation typically share the same indentation.
    ಆದರೆ (a->ಕೊನೆ_lineno > a->lineno) {
        ಹಿಂತಿರುಗಿ RAISE_ERROR_ತಿಳಿದ_LOCATION(
            p, PyExc_SyntaxError, a->ಕೊನೆ_lineno, a->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
            a->ಕೊನೆ_lineno, a->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
            "invalid syntax. Perhaps you forgot a comma?"
        );
    }
    ಹಿಂತಿರುಗಿ RAISE_ERROR_ತಿಳಿದ_LOCATION(
        p, PyExc_SyntaxError, a->lineno, a->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
        b->ಕೊನೆ_lineno, b->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
        "invalid syntax. Perhaps you forgot a comma?"
    );
}

ಸ್ಥಿರ ಫಲಿತಾಂಶ_Token_With_Metadata *
ಫಲಿತಾಂಶ_token_with_metadata(Parser *p, ಶೂನ್ಯ *ಫಲಿತಾಂಶ, PyObject *metadata)
{
    ಫಲಿತಾಂಶ_Token_With_Metadata *res = _PyArena_Malloc(p->arena, ಗಾತ್ರ(ಫಲಿತಾಂಶ_Token_With_Metadata));
    ಆದರೆ (res == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    res->metadata = metadata;
    res->ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ;
    ಹಿಂತಿರುಗಿ res;
}

ಸ್ಥಿರ ಅಕ್ಷರ
formatted_string_ಪೂರ್ವಪ್ರತ್ಯಯ(ಸ್ಥಿರಾಂಕ Parser *p)
{
    ಪೂರ್ಣಾಂಕ nested = 0;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = p->mark - 1; i >= 0; i--) {
        ಪೂರ್ಣಾಂಕ type = p->ಟೋಕನುಗಳು[i]->type;
        ಆದರೆ (type == FSTRING_ಕೊನೆ || type == TSTRING_ಕೊನೆ) {
            nested++;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == FSTRING_ಆರಂಭ || type == TSTRING_ಆರಂಭ) {
            ಆದರೆ (nested == 0) {
                ಹಿಂತಿರುಗಿ type == TSTRING_ಆರಂಭ ? 't' : 'f';
            }
            nested--;
        }
    }
    Py_UNREACHABLE();
}

ಫಲಿತಾಂಶ_Token_With_Metadata *
_PyPegen_check_fstring_conversion(Parser *p, Token* conv_token, ಅಭಿವ್ಯಕ್ತಿ_ty conv)
{
    ಆದರೆ (conv_token->lineno != conv->lineno || conv_token->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ != conv->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ) {
        ಹಿಂತಿರುಗಿ RAISE_SYNTAX_ERROR_ತಿಳಿದ_RANGE(
            conv_token, conv,
            "%c-string: conversion type must come right after the exclamation mark",
            formatted_string_ಪೂರ್ವಪ್ರತ್ಯಯ(p)
        );
    }

    Py_UCS4 ಮೊದಲ = PyUnicode_READ_CHAR(conv->v.Name.id, 0);
    ಆದರೆ (Py_Unicode_GET_ಉದ್ದಳತೆ(conv->v.Name.id) > 1 ||
            !(ಮೊದಲ == 's' || ಮೊದಲ == 'r' || ಮೊದಲ == 'a')) {
        RAISE_SYNTAX_ERROR_ತಿಳಿದ_LOCATION(conv,
                                            "%c-string: invalid conversion character %R: expected 's', 'r', or 'a'",
                                            formatted_string_ಪೂರ್ವಪ್ರತ್ಯಯ(p),
                                            conv->v.Name.id);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ_token_with_metadata(p, conv, conv_token->metadata);
}

ಫಲಿತಾಂಶ_Token_With_Metadata *
Py_Pegen_ಸಿದ್ಧತೆ_ತುಂಬಿದ_format_spec(Parser *p, Token *colon, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *spec, ಪೂರ್ಣಾಂಕ lineno, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                                ಪೂರ್ಣಾಂಕ ಕೊನೆ_lineno, ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, PyArena *arena)
{
    ಆದರೆ (!spec) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // This is needed to keep compatibility with 3.11, where an empty format
    // spec is parsed as an *empty* JoinedStr node, instead of having an empty
    // constant in it.
    Py_ssize_t n_ವಸ್ತುಗಳು = asdl_seq_LEN(spec);
    Py_ssize_t non_ಖಾಲಿಗಳು_ಎಣಿಕೆ = 0;
    ಗಾಗಿ (Py_ssize_t i = 0; i < n_ವಸ್ತುಗಳು; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty ಅಂಶ = asdl_seq_GET(spec, i);
        non_ಖಾಲಿಗಳು_ಎಣಿಕೆ += !(ಅಂಶ->ಬಗೆ == Constant_ಬಗೆ &&
                             PyUnicode_CheckExact(ಅಂಶ->v.Constant.ಮೌಲ್ಯ) &&
                             Py_Unicode_GET_ಉದ್ದಳತೆ(ಅಂಶ->v.Constant.ಮೌಲ್ಯ) == 0);
    }
    ಆದರೆ (non_ಖಾಲಿಗಳು_ಎಣಿಕೆ != n_ವಸ್ತುಗಳು) {
        asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *resized_spec =
            Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(non_ಖಾಲಿಗಳು_ಎಣಿಕೆ, p->arena);
        ಆದರೆ (resized_spec == ಖಾಲಿ) {
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
        Py_ssize_t j = 0;
        ಗಾಗಿ (Py_ssize_t i = 0; i < n_ವಸ್ತುಗಳು; i++) {
            ಅಭಿವ್ಯಕ್ತಿ_ty ಅಂಶ = asdl_seq_GET(spec, i);
            ಆದರೆ (ಅಂಶ->ಬಗೆ == Constant_ಬಗೆ &&
                PyUnicode_CheckExact(ಅಂಶ->v.Constant.ಮೌಲ್ಯ) &&
                Py_Unicode_GET_ಉದ್ದಳತೆ(ಅಂಶ->v.Constant.ಮೌಲ್ಯ) == 0) {
                ಮುಂದುವರಿ;
            }
            asdl_seq_ಗಣ(resized_spec, j++, ಅಂಶ);
        }
        ಪ್ರತಿಜ್ಞಾ(j == non_ಖಾಲಿಗಳು_ಎಣಿಕೆ);
        spec = resized_spec;
    }
    ಅಭಿವ್ಯಕ್ತಿ_ty res;
    Py_ssize_t n = asdl_seq_LEN(spec);
    ಆದರೆ (n == 0 || (n == 1 && asdl_seq_GET(spec, 0)->ಬಗೆ == Constant_ಬಗೆ)) {
        res = _PyAST_JoinedStr(spec, lineno, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಕೊನೆ_lineno,
                                    ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, p->arena);
    } ಇಲ್ಲದಿದ್ದರೆ {
        res = Py_Pegen_concatenate_ಅಕ್ಷರಸರಣಿಗಳು(p, spec,
                             lineno, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಕೊನೆ_lineno,
                             ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, arena);
    }
    ಆದರೆ (!res) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ_token_with_metadata(p, res, colon->metadata);
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *
Py_Pegen_get_ಅಭಿವ್ಯಕ್ತಿ_name(ಅಭಿವ್ಯಕ್ತಿ_ty e)
{
    ಪ್ರತಿಜ್ಞಾ(e != ಖಾಲಿ);
    ಆಯ್ಕೆ (e->ಬಗೆ) {
        ಸ್ಥಿತಿ Attribute_ಬಗೆ:
            ಹಿಂತಿರುಗಿ "attribute";
        ಸ್ಥಿತಿ Subscript_ಬಗೆ:
            ಹಿಂತಿರುಗಿ "subscript";
        ಸ್ಥಿತಿ Starred_ಬಗೆ:
            ಹಿಂತಿರುಗಿ "starred";
        ಸ್ಥಿತಿ Name_ಬಗೆ:
            ಹಿಂತಿರುಗಿ "name";
        ಸ್ಥಿತಿ List_ಬಗೆ:
            ಹಿಂತಿರುಗಿ "list";
        ಸ್ಥಿತಿ Tuple_ಬಗೆ:
            ಹಿಂತಿರುಗಿ "tuple";
        ಸ್ಥಿತಿ Lambda_ಬಗೆ:
            ಹಿಂತಿರುಗಿ "lambda";
        ಸ್ಥಿತಿ Call_ಬಗೆ:
            ಹಿಂತಿರುಗಿ "function call";
        ಸ್ಥಿತಿ Bool_Op_ಬಗೆ:
        ಸ್ಥಿತಿ Bin_Op_ಬಗೆ:
        ಸ್ಥಿತಿ Unary_Op_ಬಗೆ:
            ಹಿಂತಿರುಗಿ "expression";
        ಸ್ಥಿತಿ Generator_Exp_ಬಗೆ:
            ಹಿಂತಿರುಗಿ "generator expression";
        ಸ್ಥಿತಿ Yield_ಬಗೆ:
        ಸ್ಥಿತಿ Yield_From_ಬಗೆ:
            ಹಿಂತಿರುಗಿ "yield expression";
        ಸ್ಥಿತಿ Await_ಬಗೆ:
            ಹಿಂತಿರುಗಿ "await expression";
        ಸ್ಥಿತಿ List_Comp_ಬಗೆ:
            ಹಿಂತಿರುಗಿ "list comprehension";
        ಸ್ಥಿತಿ ಗಣ_Comp_ಬಗೆ:
            ಹಿಂತಿರುಗಿ "set comprehension";
        ಸ್ಥಿತಿ Dict_Comp_ಬಗೆ:
            ಹಿಂತಿರುಗಿ "dict comprehension";
        ಸ್ಥಿತಿ Dict_ಬಗೆ:
            ಹಿಂತಿರುಗಿ "dict literal";
        ಸ್ಥಿತಿ ಗಣ_ಬಗೆ:
            ಹಿಂತಿರುಗಿ "set display";
        ಸ್ಥಿತಿ Joined_Str_ಬಗೆ:
        ಸ್ಥಿತಿ Formatted_ಮೌಲ್ಯ_ಬಗೆ:
            ಹಿಂತಿರುಗಿ "f-string expression";
        ಸ್ಥಿತಿ Template_Str_ಬಗೆ:
        ಸ್ಥಿತಿ Interpolation_ಬಗೆ:
            ಹಿಂತಿರುಗಿ "t-string expression";
        ಸ್ಥಿತಿ Constant_ಬಗೆ: {
            PyObject *ಮೌಲ್ಯ = e->v.Constant.ಮೌಲ್ಯ;
            ಆದರೆ (ಮೌಲ್ಯ == Py_None) {
                ಹಿಂತಿರುಗಿ "None";
            }
            ಆದರೆ (ಮೌಲ್ಯ == Py_False) {
                ಹಿಂತಿರುಗಿ "False";
            }
            ಆದರೆ (ಮೌಲ್ಯ == Py_True) {
                ಹಿಂತಿರುಗಿ "True";
            }
            ಆದರೆ (ಮೌಲ್ಯ == Py_Ellipsis) {
                ಹಿಂತಿರುಗಿ "ellipsis";
            }
            ಹಿಂತಿರುಗಿ "literal";
        }
        ಸ್ಥಿತಿ Compare_ಬಗೆ:
            ಹಿಂತಿರುಗಿ "comparison";
        ಸ್ಥಿತಿ If_Exp_ಬಗೆ:
            ಹಿಂತಿರುಗಿ "conditional expression";
        ಸ್ಥಿತಿ Named_ಅಭಿವ್ಯಕ್ತಿ_ಬಗೆ:
            ಹಿಂತಿರುಗಿ "named expression";
        ಪೂರ್ವನಿಯೋಜಿತ:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->ಬಗೆ, e->lineno);
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
}

ಅಭಿವ್ಯಕ್ತಿ_ty
Py_Pegen_get_ಕೊನೆಯ_comprehension_ಅಂಶ(comprehension_ty comprehension) {
    ಆದರೆ (comprehension->ifs == ಖಾಲಿ || asdl_seq_LEN(comprehension->ifs) == 0) {
        ಹಿಂತಿರುಗಿ comprehension->iter;
    }
    ಹಿಂತಿರುಗಿ Py_Pegen_ಕೊನೆಯ_ಅಂಶ(comprehension->ifs, ಅಭಿವ್ಯಕ್ತಿ_ty);
}

ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_collect_call_seqs(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *a, asdl_seq *b,
                     ಪೂರ್ಣಾಂಕ lineno, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಕೊನೆ_lineno,
                     ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, PyArena *arena) {
    Py_ssize_t ವಾದಗಳು_len = asdl_seq_LEN(a);
    Py_ssize_t ಒಟ್ಟು_len = ವಾದಗಳು_len;

    ಆದರೆ (b == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ _PyAST_Call(_PyPegen_dummy_name(p), a, ಖಾಲಿ, lineno, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                        ಕೊನೆ_lineno, ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, arena);

    }

    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *starreds = Py_Pegen_seq_ಹೊರತೆಗೆ_starred_exprs(p, b);
    ಆದರೆ (!starreds && PyErr_Occurred()) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    asdl_keyword_seq *keywords = _PyPegen_seq_delete_starred_exprs(p, b);
    ಆದರೆ (!keywords && PyErr_Occurred()) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (starreds) {
        ಒಟ್ಟು_len += asdl_seq_LEN(starreds);
    }

    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *ವಾದಗಳು = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(ಒಟ್ಟು_len, arena);
    ಆದರೆ (ವಾದಗಳು == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    Py_ssize_t i = 0;
    ಗಾಗಿ (i = 0; i < ವಾದಗಳು_len; i++) {
        asdl_seq_ಗಣ(ವಾದಗಳು, i, asdl_seq_GET(a, i));
    }
    ಗಾಗಿ (; i < ಒಟ್ಟು_len; i++) {
        asdl_seq_ಗಣ(ವಾದಗಳು, i, asdl_seq_GET(starreds, i - ವಾದಗಳು_len));
    }

    ಹಿಂತಿರುಗಿ _PyAST_Call(_PyPegen_dummy_name(p), ವಾದಗಳು, keywords, lineno,
                       ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಕೊನೆ_lineno, ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, arena);
}

// AST Error reporting helpers

ಅಭಿವ್ಯಕ್ತಿ_ty
Py_Pegen_get_invalid_ಗುರಿ(ಅಭಿವ್ಯಕ್ತಿ_ty e, TARGETS_TYPE targets_type)
{
    ಆದರೆ (e == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (targets_type == SINGLE_TARGETS ||
        targets_type == ATTRIBUTE_OR_SUBSCRIPT_TARGETS) {
        ಆದರೆ (targets_type == SINGLE_TARGETS && e->ಬಗೆ == Name_ಬಗೆ) {
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
        ಹಿಂತಿರುಗಿ e->ಬಗೆ == Attribute_ಬಗೆ || e->ಬಗೆ == Subscript_ಬಗೆ ? ಖಾಲಿ : e;
    }

#ವ್ಯಾಖ್ಯಾನಿಸು ಭೇಟಿ_CONTAINER(CONTAINER, TYPE) ಮಾಡು { \
        Py_ssize_t len = asdl_seq_LEN((CONTAINER)->v.TYPE.elts);\
        ಗಾಗಿ (Py_ssize_t i = 0; i < len; i++) {\
            ಅಭಿವ್ಯಕ್ತಿ_ty other = asdl_seq_GET((CONTAINER)->v.TYPE.elts, i);\
            ಅಭಿವ್ಯಕ್ತಿ_ty ಮಗು = Py_Pegen_get_invalid_ಗುರಿ(other, targets_type);\
            ಆದರೆ (ಮಗು != ಖಾಲಿ) {\
                ಹಿಂತಿರುಗಿ ಮಗು;\
            }\
        }\
    } ಆಗ (0)

    // We only need to visit List and Tuple nodes recursively as those
    // are the only ones that can contain valid names in targets when
    // they are parsed as expressions. Any other kind of expression
    // that is a container (like Sets or Dicts) is directly invalid and
    // we don't need to visit it recursively.

    ಆಯ್ಕೆ (e->ಬಗೆ) {
        ಸ್ಥಿತಿ List_ಬಗೆ:
            ಭೇಟಿ_CONTAINER(e, List);
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        ಸ್ಥಿತಿ Tuple_ಬಗೆ:
            ಭೇಟಿ_CONTAINER(e, Tuple);
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        ಸ್ಥಿತಿ Starred_ಬಗೆ:
            ಆದರೆ (targets_type == DEL_TARGETS) {
                ಹಿಂತಿರುಗಿ e;
            }
            ಹಿಂತಿರುಗಿ Py_Pegen_get_invalid_ಗುರಿ(e->v.Starred.ಮೌಲ್ಯ, targets_type);
        ಸ್ಥಿತಿ Compare_ಬಗೆ:
            // This is needed, because the `a in b` in `for a in b` gets parsed
            // as a comparison, and so we need to search the left side of the comparison
            // for invalid targets.
            ಆದರೆ (targets_type == FOR_TARGETS) {
                cmpop_ty cmpop = (cmpop_ty) asdl_seq_GET(e->v.Compare.ops, 0);
                ಆದರೆ (cmpop == ಒಳ) {
                    ಹಿಂತಿರುಗಿ Py_Pegen_get_invalid_ಗುರಿ(e->v.Compare.ಎಡ, targets_type);
                }
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
            }
            ಹಿಂತಿರುಗಿ e;
        ಸ್ಥಿತಿ Name_ಬಗೆ:
        ಸ್ಥಿತಿ Subscript_ಬಗೆ:
        ಸ್ಥಿತಿ Attribute_ಬಗೆ:
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        ಪೂರ್ವನಿಯೋಜಿತ:
            ಹಿಂತಿರುಗಿ e;
    }
}

ಶೂನ್ಯ *_PyPegen_arguments_parsing_error(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty e) {
    ಪೂರ್ಣಾಂಕ kwarg_unpacking = 0;
    ಗಾಗಿ (Py_ssize_t i = 0, l = asdl_seq_LEN(e->v.Call.keywords); i < l; i++) {
        keyword_ty keyword = asdl_seq_GET(e->v.Call.keywords, i);
        ಆದರೆ (!keyword->arg) {
            kwarg_unpacking = 1;
        }
    }

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ = ಖಾಲಿ;
    ಆದರೆ (kwarg_unpacking) {
        ಸಂದೇಶ = "positional argument follows keyword argument unpacking";
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಸಂದೇಶ = "positional argument follows keyword argument";
    }

    ಹಿಂತಿರುಗಿ RAISE_SYNTAX_ERROR(ಸಂದೇಶ);
}

ಶೂನ್ಯ *
Py_Pegen_nonparen_genexp_ಒಳ_call(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty ವಾದಗಳು, asdl_comprehension_seq *comprehensions)
{
    /* The rule that calls this function is 'args for_if_clauses'.
       For the input f(L, x for x in y), L and x are in args and
       the for is parsed as a for_if_clause. We have to check if
       len <= 1, so that input like dict((a, b) for a, b in x)
       gets successfully parsed and then we pass the last
       argument (x in the above example) as the location of the
       error */
    Py_ssize_t len = asdl_seq_LEN(ವಾದಗಳು->v.Call.ವಾದಗಳು);
    ಆದರೆ (len <= 1) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    comprehension_ty ಕೊನೆಯ_comprehension = Py_Pegen_ಕೊನೆಯ_ಅಂಶ(comprehensions, comprehension_ty);

    ಹಿಂತಿರುಗಿ RAISE_SYNTAX_ERROR_ತಿಳಿದ_RANGE(
        (ಅಭಿವ್ಯಕ್ತಿ_ty) asdl_seq_GET(ವಾದಗಳು->v.Call.ವಾದಗಳು, len - 1),
        Py_Pegen_get_ಕೊನೆಯ_comprehension_ಅಂಶ(ಕೊನೆಯ_comprehension),
        "Generator expression must be parenthesized"
    );
}

// Fstring stuff

ಸ್ಥಿರ ಅಭಿವ್ಯಕ್ತಿ_ty
_PyPegen_decode_fstring_part(Parser* p, ಪೂರ್ಣಾಂಕ is_raw, ಅಭಿವ್ಯಕ್ತಿ_ty constant, Token* token) {
    ಪ್ರತಿಜ್ಞಾ(PyUnicode_CheckExact(constant->v.Constant.ಮೌಲ್ಯ));

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* bstr = PyUnicode_AsUTF8(constant->v.Constant.ಮೌಲ್ಯ);
    ಆದರೆ (bstr == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಗಾತ್ರ_t len;
    ಆದರೆ (strcmp(bstr, "{{") == 0 || strcmp(bstr, "}}") == 0) {
        len = 1;
    } ಇಲ್ಲದಿದ್ದರೆ {
        len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(bstr);
    }

    is_raw = is_raw || strchr(bstr, '\\') == ಖಾಲಿ;
    PyObject *str = _PyPegen_decode_string(p, is_raw, bstr, len, token);
    ಆದರೆ (str == ಖಾಲಿ) {
        _Pypegen_raise_decode_error(p);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (Py_Arena_ಕೂಡಿಸು_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ _PyAST_Constant(str, ಖಾಲಿ, constant->lineno, constant->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                           constant->ಕೊನೆ_lineno, constant->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                           p->arena);
}

ಸ್ಥಿರ asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *
_get_resized_exprs(Parser *p, Token *a, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *raw_expressions,
                   Token *b, ಪೂರ್ಣಾಂಕ is_tstring)
{
    Py_ssize_t n_ವಸ್ತುಗಳು = asdl_seq_LEN(raw_expressions);
    Py_ssize_t ಒಟ್ಟು_ವಸ್ತುಗಳು = n_ವಸ್ತುಗಳು;
    ಗಾಗಿ (Py_ssize_t i = 0; i < n_ವಸ್ತುಗಳು; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty ಅಂಶ = asdl_seq_GET(raw_expressions, i);
        ಆದರೆ (ಅಂಶ->ಬಗೆ == Joined_Str_ಬಗೆ) {
            ಒಟ್ಟು_ವಸ್ತುಗಳು += asdl_seq_LEN(ಅಂಶ->v.JoinedStr.ಮೌಲ್ಯಗಳು) - 1;
        }
    }

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* quote_str = Py_ಬೈಟುಗಳು_As_String(a->ಬೈಟುಗಳು);
    ಆದರೆ (quote_str == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಪೂರ್ಣಾಂಕ is_raw = strpbrk(quote_str, "rR") != ಖಾಲಿ;

    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *seq = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(ಒಟ್ಟು_ವಸ್ತುಗಳು, p->arena);
    ಆದರೆ (seq == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    Py_ssize_t ಸೂಚ್ಯಂಕ = 0;
    ಗಾಗಿ (Py_ssize_t i = 0; i < n_ವಸ್ತುಗಳು; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty ಅಂಶ = asdl_seq_GET(raw_expressions, i);

        /* Debug expressions arrive as JoinedStr(text, value); flatten them
           into the surrounding string. */
        ಆದರೆ (ಅಂಶ->ಬಗೆ == Joined_Str_ಬಗೆ) {
            asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *ಮೌಲ್ಯಗಳು = ಅಂಶ->v.JoinedStr.ಮೌಲ್ಯಗಳು;
            ಆದರೆ (asdl_seq_LEN(ಮೌಲ್ಯಗಳು) != 2) {
                PyErr_Format(PyExc_SystemError,
                             is_tstring
                             ? "unexpected TemplateStr node without debug data in t-string at line %d"
                             : "unexpected JoinedStr node without debug data in f-string at line %d",
                             ಅಂಶ->lineno);
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
            }

            ಅಭಿವ್ಯಕ್ತಿ_ty ಮೊದಲ = asdl_seq_GET(ಮೌಲ್ಯಗಳು, 0);
            ಪ್ರತಿಜ್ಞಾ(ಮೊದಲ->ಬಗೆ == Constant_ಬಗೆ);
            asdl_seq_ಗಣ(seq, ಸೂಚ್ಯಂಕ++, ಮೊದಲ);

            ಅಭಿವ್ಯಕ್ತಿ_ty ಎರಡನೇ = asdl_seq_GET(ಮೌಲ್ಯಗಳು, 1);
            ಪ್ರತಿಜ್ಞಾ((is_tstring &&
                    ಎರಡನೇ->ಬಗೆ == Interpolation_ಬಗೆ) ||
                   ಎರಡನೇ->ಬಗೆ == Formatted_ಮೌಲ್ಯ_ಬಗೆ);
            asdl_seq_ಗಣ(seq, ಸೂಚ್ಯಂಕ++, ಎರಡನೇ);

            ಮುಂದುವರಿ;
        }

        ಆದರೆ (ಅಂಶ->ಬಗೆ == Constant_ಬಗೆ) {
            ಅಂಶ = _PyPegen_decode_fstring_part(p, is_raw, ಅಂಶ, b);
            ಆದರೆ (ಅಂಶ == ಖಾಲಿ) {
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
            }

            /* Tokenizer emits string parts even when the underlying string
            might become an empty value (e.g. FSTRING_MIDDLE with the value \\n)
            so we need to check for them and simplify it here. */
            ಆದರೆ (PyUnicode_CheckExact(ಅಂಶ->v.Constant.ಮೌಲ್ಯ)
                && Py_Unicode_GET_ಉದ್ದಳತೆ(ಅಂಶ->v.Constant.ಮೌಲ್ಯ) == 0) {
                ಮುಂದುವರಿ;
            }
        }
        asdl_seq_ಗಣ(seq, ಸೂಚ್ಯಂಕ++, ಅಂಶ);
    }

    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *resized_exprs;
    ಆದರೆ (ಸೂಚ್ಯಂಕ != ಒಟ್ಟು_ವಸ್ತುಗಳು) {
        resized_exprs = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(ಸೂಚ್ಯಂಕ, p->arena);
        ಆದರೆ (resized_exprs == ಖಾಲಿ) {
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
        ಗಾಗಿ (Py_ssize_t i = 0; i < ಸೂಚ್ಯಂಕ; i++) {
            asdl_seq_ಗಣ(resized_exprs, i, asdl_seq_GET(seq, i));
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        resized_exprs = seq;
    }
    ಹಿಂತಿರುಗಿ resized_exprs;
}

ಅಭಿವ್ಯಕ್ತಿ_ty
_PyPegen_template_str(Parser *p, Token *a, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *raw_expressions, Token *b) {

    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 1);
    ಆದರೆ (resized_exprs == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ _PyAST_TemplateStr(resized_exprs, a->lineno, a->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                              b->ಕೊನೆ_lineno, b->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                              p->arena);
}

ಅಭಿವ್ಯಕ್ತಿ_ty
_PyPegen_joined_str(Parser *p, Token* a, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq* raw_expressions, Token*b) {

    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 0);
    ಆದರೆ (resized_exprs == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ _PyAST_JoinedStr(resized_exprs, a->lineno, a->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                            b->ಕೊನೆ_lineno, b->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                            p->arena);
}

ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok) {
    ಆದರೆ (tok->ಬೈಟುಗಳು == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ Py_Pegen_raise_incomplete_ಒಳಹರಿವು(p);
    }
    Py_ssize_t bsize = Py_ಬೈಟುಗಳು_GET_ಗಾತ್ರ(tok->ಬೈಟುಗಳು);
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bstr = Py_ಬೈಟುಗಳು_AS_STRING(tok->ಬೈಟುಗಳು);

    ಪೂರ್ಣಾಂಕ is_raw = tok->is_raw;

    PyObject* str = _PyPegen_decode_string(p, is_raw, bstr, bsize, tok);
    ಆದರೆ (str == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (Py_Arena_ಕೂಡಿಸು_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ _PyAST_Constant(str, ಖಾಲಿ, tok->lineno, tok->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                           tok->ಕೊನೆ_lineno, tok->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                           p->arena);
}

ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_constant_from_token(Parser* p, Token* tok) {
    ಆದರೆ (tok->ಬೈಟುಗಳು == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ Py_Pegen_raise_incomplete_ಒಳಹರಿವು(p);
    }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bstr = Py_ಬೈಟುಗಳು_AS_STRING(tok->ಬೈಟುಗಳು);
    PyObject* str = PyUnicode_FromString(bstr);
    ಆದರೆ (str == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (Py_Arena_ಕೂಡಿಸು_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ _PyAST_Constant(str, ಖಾಲಿ, tok->lineno, tok->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                           tok->ಕೊನೆ_lineno, tok->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                           p->arena);
}

ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_constant_from_string(Parser* p, Token* tok) {
    ಆದರೆ (tok->ಬೈಟುಗಳು == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ Py_Pegen_raise_incomplete_ಒಳಹರಿವು(p);
    }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *the_str = Py_ಬೈಟುಗಳು_AS_STRING(tok->ಬೈಟುಗಳು);
    PyObject *s = _PyPegen_parse_string(p, tok);
    ಆದರೆ (s == ಖಾಲಿ) {
        _Pypegen_raise_decode_error(p);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (Py_Arena_ಕೂಡಿಸು_Py_Object(p->arena, s) < 0) {
        Py_DECREF(s);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    PyObject *ಬಗೆ = ಖಾಲಿ;
    ಆದರೆ (the_str && the_str[0] == 'u') {
        ಬಗೆ = _PyPegen_new_identifier(p, "u");
        ಆದರೆ (ಬಗೆ == ಖಾಲಿ) {
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
    }
    ಹಿಂತಿರುಗಿ _PyAST_Constant(s, ಬಗೆ, tok->lineno, tok->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, tok->ಕೊನೆ_lineno, tok->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, p->arena);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
_get_interpolation_conversion(Parser *p, Token *debug, ಫಲಿತಾಂಶ_Token_With_Metadata *conversion,
                              ಫಲಿತಾಂಶ_Token_With_Metadata *format)
{
    ಆದರೆ (conversion != ಖಾಲಿ) {
        ಅಭಿವ್ಯಕ್ತಿ_ty conversion_ಅಭಿವ್ಯಕ್ತಿ = (ಅಭಿವ್ಯಕ್ತಿ_ty) conversion->ಫಲಿತಾಂಶ;
        ಪ್ರತಿಜ್ಞಾ(conversion_ಅಭಿವ್ಯಕ್ತಿ->ಬಗೆ == Name_ಬಗೆ);
        Py_UCS4 ಮೊದಲ = PyUnicode_READ_CHAR(conversion_ಅಭಿವ್ಯಕ್ತಿ->v.Name.id, 0);
        ಹಿಂತಿರುಗಿ Py_ಸುರಕ್ಷಿತ_DOWNCAST(ಮೊದಲ, Py_UCS4, ಪೂರ್ಣಾಂಕ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (debug && !format) {
        /* If no conversion is specified, use !r for debug expressions */
        ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)'r';
    }
    ಹಿಂತಿರುಗಿ -1;
}

ಸ್ಥಿರ PyObject *
strip_interpolation_debug_ಅಭಿವ್ಯಕ್ತಿ(PyObject *exprstr)
{
    Py_ssize_t len = Py_Unicode_GET_ಉದ್ದಳತೆ(exprstr);

    /* Discard whitespace and explicit line continuations after the debug "="
       but preserve whitespace before it. */
    ಆಗ (len > 0) {
        ಪೂರ್ಣಾಂಕ has_newline = 0;
        ಆಗ (len > 0) {
            Py_UCS4 c = PyUnicode_READ_CHAR(exprstr, len - 1);
            ಆದರೆ (!_PyUnicode_IsWhitespace(c)) {
                ನಿಲ್ಲಿಸು;
            }
            ಆದರೆ (c == '\r' || c == '\n') {
                has_newline = 1;
            }
            len--;
        }
        ಆದರೆ (!has_newline || len == 0 ||
            PyUnicode_READ_CHAR(exprstr, len - 1) != '\\')
        {
            ನಿಲ್ಲಿಸು;
        }
        len--;
    }

    /* Preserve unexpected metadata instead of dropping source text. */
    ಆದರೆ (len == 0 || PyUnicode_READ_CHAR(exprstr, len - 1) != '=') {
        ಹಿಂತಿರುಗಿ Py_NewRef(exprstr);
    }

    ಹಿಂತಿರುಗಿ PyUnicode_Substring(exprstr, 0, len - 1);
}

ಅಭಿವ್ಯಕ್ತಿ_ty _PyPegen_interpolation(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty ಅಭಿವ್ಯಕ್ತಿ_2, Token *debug, ಫಲಿತಾಂಶ_Token_With_Metadata *conversion,
                                 ಫಲಿತಾಂಶ_Token_With_Metadata *format, Token *closing_brace, ಪೂರ್ಣಾಂಕ lineno, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                                 ಪೂರ್ಣಾಂಕ ಕೊನೆ_lineno, ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, PyArena *arena) {

    ಪೂರ್ಣಾಂಕ conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    /* Find the non whitespace token after the "=" */
    ಪೂರ್ಣಾಂಕ debug_ಕೊನೆ_ಸಾಲು, debug_ಕೊನೆ_ಸ್ಥಾನಾಂತರ;
    PyObject *debug_metadata;
    constant exprstr;

    ಆದರೆ (conversion) {
        debug_ಕೊನೆ_ಸಾಲು = ((ಅಭಿವ್ಯಕ್ತಿ_ty) conversion->ಫಲಿತಾಂಶ)->lineno;
        debug_ಕೊನೆ_ಸ್ಥಾನಾಂತರ = ((ಅಭಿವ್ಯಕ್ತಿ_ty) conversion->ಫಲಿತಾಂಶ)->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;
        debug_metadata = exprstr = conversion->metadata;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (format) {
        debug_ಕೊನೆ_ಸಾಲು = ((ಅಭಿವ್ಯಕ್ತಿ_ty) format->ಫಲಿತಾಂಶ)->lineno;
        debug_ಕೊನೆ_ಸ್ಥಾನಾಂತರ = ((ಅಭಿವ್ಯಕ್ತಿ_ty) format->ಫಲಿತಾಂಶ)->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ + 1;
        debug_metadata = exprstr = format->metadata;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        debug_ಕೊನೆ_ಸಾಲು = ಕೊನೆ_lineno;
        debug_ಕೊನೆ_ಸ್ಥಾನಾಂತರ = ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;
        debug_metadata = exprstr = closing_brace->metadata;
    }

    ಪ್ರತಿಜ್ಞಾ(exprstr != ಖಾಲಿ);
    PyObject *final_exprstr = debug
        ? strip_interpolation_debug_ಅಭಿವ್ಯಕ್ತಿ(exprstr)
        : Py_NewRef(exprstr);
    ಆದರೆ (!final_exprstr || Py_Arena_ಕೂಡಿಸು_Py_Object(arena, final_exprstr) < 0) {
        Py_XDECREF(final_exprstr);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಅಭಿವ್ಯಕ್ತಿ_ty interpolation = _PyAST_Interpolation(
        ಅಭಿವ್ಯಕ್ತಿ_2, final_exprstr, conversion_val, format ? (ಅಭಿವ್ಯಕ್ತಿ_ty) format->ಫಲಿತಾಂಶ : ಖಾಲಿ,
        lineno, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಕೊನೆ_lineno,
        ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, arena
    );

    ಆದರೆ (!interpolation || !debug) {
        ಹಿಂತಿರುಗಿ interpolation;
    }

    ಅಭಿವ್ಯಕ್ತಿ_ty debug_ಪಠ್ಯ = _PyAST_Constant(debug_metadata, ಖಾಲಿ, lineno, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ + 1, debug_ಕೊನೆ_ಸಾಲು,
                                            debug_ಕೊನೆ_ಸ್ಥಾನಾಂತರ - 1, p->arena);
    ಆದರೆ (!debug_ಪಠ್ಯ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *ಮೌಲ್ಯಗಳು = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(2, arena);
    ಆದರೆ (!ಮೌಲ್ಯಗಳು) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    asdl_seq_ಗಣ(ಮೌಲ್ಯಗಳು, 0, debug_ಪಠ್ಯ);
    asdl_seq_ಗಣ(ಮೌಲ್ಯಗಳು, 1, interpolation);
    ಹಿಂತಿರುಗಿ _PyAST_JoinedStr(ಮೌಲ್ಯಗಳು, lineno, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, debug_ಕೊನೆ_ಸಾಲು, debug_ಕೊನೆ_ಸ್ಥಾನಾಂತರ, p->arena);
}

ಅಭಿವ್ಯಕ್ತಿ_ty Py_Pegen_formatted_ಮೌಲ್ಯ(Parser *p, ಅಭಿವ್ಯಕ್ತಿ_ty ಅಭಿವ್ಯಕ್ತಿ_2, Token *debug, ಫಲಿತಾಂಶ_Token_With_Metadata *conversion,
                                 ಫಲಿತಾಂಶ_Token_With_Metadata *format, Token *closing_brace, ಪೂರ್ಣಾಂಕ lineno, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                                 ಪೂರ್ಣಾಂಕ ಕೊನೆ_lineno, ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, PyArena *arena) {
    ಪೂರ್ಣಾಂಕ conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    ಅಭಿವ್ಯಕ್ತಿ_ty formatted_ಮೌಲ್ಯ = Py_AST_Formatted_ಮೌಲ್ಯ(
        ಅಭಿವ್ಯಕ್ತಿ_2, conversion_val, format ? (ಅಭಿವ್ಯಕ್ತಿ_ty) format->ಫಲಿತಾಂಶ : ಖಾಲಿ,
        lineno, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಕೊನೆ_lineno,
        ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, arena
    );

    ಆದರೆ (!formatted_ಮೌಲ್ಯ || !debug) {
        ಹಿಂತಿರುಗಿ formatted_ಮೌಲ್ಯ;
    }

    /* Find the non whitespace token after the "=" */
    ಪೂರ್ಣಾಂಕ debug_ಕೊನೆ_ಸಾಲು, debug_ಕೊನೆ_ಸ್ಥಾನಾಂತರ;
    PyObject *debug_metadata;

    ಆದರೆ (conversion) {
        debug_ಕೊನೆ_ಸಾಲು = ((ಅಭಿವ್ಯಕ್ತಿ_ty) conversion->ಫಲಿತಾಂಶ)->lineno;
        debug_ಕೊನೆ_ಸ್ಥಾನಾಂತರ = ((ಅಭಿವ್ಯಕ್ತಿ_ty) conversion->ಫಲಿತಾಂಶ)->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;
        debug_metadata = conversion->metadata;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (format) {
        debug_ಕೊನೆ_ಸಾಲು = ((ಅಭಿವ್ಯಕ್ತಿ_ty) format->ಫಲಿತಾಂಶ)->lineno;
        debug_ಕೊನೆ_ಸ್ಥಾನಾಂತರ = ((ಅಭಿವ್ಯಕ್ತಿ_ty) format->ಫಲಿತಾಂಶ)->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ + 1;
        debug_metadata = format->metadata;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        debug_ಕೊನೆ_ಸಾಲು = ಕೊನೆ_lineno;
        debug_ಕೊನೆ_ಸ್ಥಾನಾಂತರ = ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;
        debug_metadata = closing_brace->metadata;
    }
    ಅಭಿವ್ಯಕ್ತಿ_ty debug_ಪಠ್ಯ = _PyAST_Constant(debug_metadata, ಖಾಲಿ, lineno, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ + 1, debug_ಕೊನೆ_ಸಾಲು,
                                            debug_ಕೊನೆ_ಸ್ಥಾನಾಂತರ - 1, p->arena);
    ಆದರೆ (!debug_ಪಠ್ಯ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *ಮೌಲ್ಯಗಳು = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(2, arena);
    ಆದರೆ (!ಮೌಲ್ಯಗಳು) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    asdl_seq_ಗಣ(ಮೌಲ್ಯಗಳು, 0, debug_ಪಠ್ಯ);
    asdl_seq_ಗಣ(ಮೌಲ್ಯಗಳು, 1, formatted_ಮೌಲ್ಯ);
    ಹಿಂತಿರುಗಿ _PyAST_JoinedStr(ಮೌಲ್ಯಗಳು, lineno, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, debug_ಕೊನೆ_ಸಾಲು, debug_ಕೊನೆ_ಸ್ಥಾನಾಂತರ, p->arena);
}

ಸ್ಥಿರ ಅಭಿವ್ಯಕ್ತಿ_ty
build_concatenated_ಬೈಟುಗಳು(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *ಅಕ್ಷರಸರಣಿಗಳು, ಪೂರ್ಣಾಂಕ lineno,
                        ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಕೊನೆ_lineno, ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(ಅಕ್ಷರಸರಣಿಗಳು);
    ಪ್ರತಿಜ್ಞಾ(len > 0);

    /* Bytes literals never get a kind, but just for consistency
        since they are represented as Constant nodes, we'll mirror
        the same behavior as unicode strings for determining the
        kind. */
    PyObject *ಬಗೆ = asdl_seq_GET(ಅಕ್ಷರಸರಣಿಗಳು, 0)->v.Constant.ಬಗೆ;

    Py_ssize_t ಒಟ್ಟು = 0;
    ಗಾಗಿ (Py_ssize_t i = 0; i < len; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty elem = asdl_seq_GET(ಅಕ್ಷರಸರಣಿಗಳು, i);
        PyObject *ಬೈಟುಗಳು = elem->v.Constant.ಮೌಲ್ಯ;
        Py_ssize_t part = Py_ಬೈಟುಗಳು_GET_ಗಾತ್ರ(ಬೈಟುಗಳು);
        ಆದರೆ (part > PY_SSIZE_T_MAX - ಒಟ್ಟು) {
            Py_Err_No_ಸ್ಮರಣೆ();
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
        ಒಟ್ಟು += part;
    }

    Py_ಬೈಟುಗಳು_Writer *writer = Py_ಬೈಟುಗಳು_Writer_Create(ಒಟ್ಟು);
    ಆದರೆ (writer == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಅಕ್ಷರ *ಹೊರಹೋಗು = Py_ಬೈಟುಗಳು_Writer_Get_ದತ್ತಾಂಶ(writer);

    ಗಾಗಿ (Py_ssize_t i = 0; i < len; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty elem = asdl_seq_GET(ಅಕ್ಷರಸರಣಿಗಳು, i);
        PyObject *ಬೈಟುಗಳು = elem->v.Constant.ಮೌಲ್ಯ;
        Py_ssize_t part = Py_ಬೈಟುಗಳು_GET_ಗಾತ್ರ(ಬೈಟುಗಳು);
        ಆದರೆ (part > 0) {
            ಸ್ಮರಣೆ_ನಕಲು(ಹೊರಹೋಗು, Py_ಬೈಟುಗಳು_AS_STRING(ಬೈಟುಗಳು), part);
            ಹೊರಹೋಗು += part;
        }
    }

    PyObject *res = Py_ಬೈಟುಗಳು_Writer_ಮುಕ್ತಾಯ(writer);
    ಆದರೆ (res == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (Py_Arena_ಕೂಡಿಸು_Py_Object(arena, res) < 0) {
        Py_DECREF(res);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ _PyAST_Constant(res, ಬಗೆ, lineno, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಕೊನೆ_lineno, ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, p->arena);
}

ಸ್ಥಿರ ಅಭಿವ್ಯಕ್ತಿ_ty
_build_concatenated_unicode(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *ಅಕ್ಷರಸರಣಿಗಳು, ಪೂರ್ಣಾಂಕ lineno,
                        ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಕೊನೆ_lineno, ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(ಅಕ್ಷರಸರಣಿಗಳು);
    ಪ್ರತಿಜ್ಞಾ(len > 1);

    ಅಭಿವ್ಯಕ್ತಿ_ty ಮೊದಲ = asdl_seq_GET(ಅಕ್ಷರಸರಣಿಗಳು, 0);

    /* When a string is getting concatenated, the kind of the string
        is determined by the first string in the concatenation
        sequence.

        u"abc" "def" -> u"abcdef"
        "abc" u"abc" ->  "abcabc" */
    PyObject *ಬಗೆ = ಮೊದಲ->v.Constant.ಬಗೆ;

    PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
    ಆದರೆ (writer == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಗಾಗಿ (Py_ssize_t i = 0; i < len; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty current_elem = asdl_seq_GET(ಅಕ್ಷರಸರಣಿಗಳು, i);
        ಪ್ರತಿಜ್ಞಾ(current_elem->ಬಗೆ == Constant_ಬಗೆ);

        ಆದರೆ (PyUnicodeWriter_WriteStr(writer,
                                     current_elem->v.Constant.ಮೌಲ್ಯ)) {
            PyUnicodeWriter_Discard(writer);
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
    }

    PyObject *final = Py_Unicode_Writer_ಮುಕ್ತಾಯ(writer);
    ಆದರೆ (final == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (Py_Arena_ಕೂಡಿಸು_Py_Object(p->arena, final) < 0) {
        Py_DECREF(final);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ _PyAST_Constant(final, ಬಗೆ, lineno, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                           ಕೊನೆ_lineno, ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, arena);
}

ಸ್ಥಿರ asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *
_build_concatenated_str(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *ಅಕ್ಷರಸರಣಿಗಳು,
                               ಪೂರ್ಣಾಂಕ lineno, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಕೊನೆ_lineno,
                               ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(ಅಕ್ಷರಸರಣಿಗಳು);
    ಪ್ರತಿಜ್ಞಾ(len > 0);

    Py_ssize_t n_flattened_elements = 0;
    ಗಾಗಿ (Py_ssize_t i = 0; i < len; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty elem = asdl_seq_GET(ಅಕ್ಷರಸರಣಿಗಳು, i);
        ಆಯ್ಕೆ(elem->ಬಗೆ) {
            ಸ್ಥಿತಿ Joined_Str_ಬಗೆ:
                n_flattened_elements += asdl_seq_LEN(elem->v.JoinedStr.ಮೌಲ್ಯಗಳು);
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ Template_Str_ಬಗೆ:
                n_flattened_elements += asdl_seq_LEN(elem->v.TemplateStr.ಮೌಲ್ಯಗಳು);
                ನಿಲ್ಲಿಸು;
            ಪೂರ್ವನಿಯೋಜಿತ:
                n_flattened_elements++;
                ನಿಲ್ಲಿಸು;
        }
    }


    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq* flattened = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(n_flattened_elements, p->arena);
    ಆದರೆ (flattened == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    /* build flattened list */
    Py_ssize_t current_pos = 0;
    ಗಾಗಿ (Py_ssize_t i = 0; i < len; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty elem = asdl_seq_GET(ಅಕ್ಷರಸರಣಿಗಳು, i);
        ಆಯ್ಕೆ(elem->ಬಗೆ) {
            ಸ್ಥಿತಿ Joined_Str_ಬಗೆ:
                ಗಾಗಿ (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.JoinedStr.ಮೌಲ್ಯಗಳು); j++) {
                    ಅಭಿವ್ಯಕ್ತಿ_ty subvalue = asdl_seq_GET(elem->v.JoinedStr.ಮೌಲ್ಯಗಳು, j);
                    ಆದರೆ (subvalue == ಖಾಲಿ) {
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
                    }
                    asdl_seq_ಗಣ(flattened, current_pos++, subvalue);
                }
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ Template_Str_ಬಗೆ:
                ಗಾಗಿ (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.TemplateStr.ಮೌಲ್ಯಗಳು); j++) {
                    ಅಭಿವ್ಯಕ್ತಿ_ty subvalue = asdl_seq_GET(elem->v.TemplateStr.ಮೌಲ್ಯಗಳು, j);
                    ಆದರೆ (subvalue == ಖಾಲಿ) {
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
                    }
                    asdl_seq_ಗಣ(flattened, current_pos++, subvalue);
                }
                ನಿಲ್ಲಿಸು;
            ಪೂರ್ವನಿಯೋಜಿತ:
                asdl_seq_ಗಣ(flattened, current_pos++, elem);
                ನಿಲ್ಲಿಸು;
        }
    }

    /* calculate folded element count */
    Py_ssize_t n_elements = 0;
    ಪೂರ್ಣಾಂಕ prev_is_constant = 0;
    ಗಾಗಿ (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty elem = asdl_seq_GET(flattened, i);

        /* The concatenation of a FormattedValue and an empty Constant should
           lead to the FormattedValue itself. Thus, we will not take any empty
           constants into account, just as in `_PyPegen_joined_str` */
        ಆದರೆ (elem->ಬಗೆ == Constant_ಬಗೆ &&
            PyUnicode_CheckExact(elem->v.Constant.ಮೌಲ್ಯ) &&
            Py_Unicode_GET_ಉದ್ದಳತೆ(elem->v.Constant.ಮೌಲ್ಯ) == 0)
            ಮುಂದುವರಿ;

        ಆದರೆ (!prev_is_constant || elem->ಬಗೆ != Constant_ಬಗೆ) {
            n_elements++;
        }
        prev_is_constant = elem->ಬಗೆ == Constant_ಬಗೆ;
    }

    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq* ಮೌಲ್ಯಗಳು = Py_asdl_ಅಭಿವ್ಯಕ್ತಿ_seq_new(n_elements, p->arena);
    ಆದರೆ (ಮೌಲ್ಯಗಳು == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    /* build folded list */
    current_pos = 0;
    ಗಾಗಿ (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty elem = asdl_seq_GET(flattened, i);

        /* if the current elem and the following are constants,
           fold them and all consequent constants */
        ಆದರೆ (elem->ಬಗೆ == Constant_ಬಗೆ) {
            ಆದರೆ (i + 1 < n_flattened_elements &&
                asdl_seq_GET(flattened, i + 1)->ಬಗೆ == Constant_ಬಗೆ) {
                ಅಭಿವ್ಯಕ್ತಿ_ty ಮೊದಲ_elem = elem;

                /* When a string is getting concatenated, the kind of the string
                   is determined by the first string in the concatenation
                   sequence.

                   u"abc" "def" -> u"abcdef"
                   "abc" u"abc" ->  "abcabc" */
                PyObject *ಬಗೆ = elem->v.Constant.ಬಗೆ;

                PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
                ಆದರೆ (writer == ಖಾಲಿ) {
                    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
                }
                ಅಭಿವ್ಯಕ್ತಿ_ty ಕೊನೆಯ_elem = elem;
                Py_ssize_t j;
                ಗಾಗಿ (j = i; j < n_flattened_elements; j++) {
                    ಅಭಿವ್ಯಕ್ತಿ_ty current_elem = asdl_seq_GET(flattened, j);
                    ಆದರೆ (current_elem->ಬಗೆ == Constant_ಬಗೆ) {
                        ಆದರೆ (PyUnicodeWriter_WriteStr(writer,
                                                     current_elem->v.Constant.ಮೌಲ್ಯ)) {
                            PyUnicodeWriter_Discard(writer);
                            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
                        }
                        ಕೊನೆಯ_elem = current_elem;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        ನಿಲ್ಲಿಸು;
                    }
                }
                i = j - 1;

                PyObject *concat_str = Py_Unicode_Writer_ಮುಕ್ತಾಯ(writer);
                ಆದರೆ (concat_str == ಖಾಲಿ) {
                    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
                }
                ಆದರೆ (Py_Arena_ಕೂಡಿಸು_Py_Object(p->arena, concat_str) < 0) {
                    Py_DECREF(concat_str);
                    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
                }
                elem = _PyAST_Constant(concat_str, ಬಗೆ, ಮೊದಲ_elem->lineno,
                                       ಮೊದಲ_elem->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                                       ಕೊನೆಯ_elem->ಕೊನೆ_lineno,
                                       ಕೊನೆಯ_elem->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, p->arena);
                ಆದರೆ (elem == ಖಾಲಿ) {
                    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
                }
            }

            /* Drop all empty contanst strings */
            ಆದರೆ (PyUnicode_CheckExact(elem->v.Constant.ಮೌಲ್ಯ) &&
                Py_Unicode_GET_ಉದ್ದಳತೆ(elem->v.Constant.ಮೌಲ್ಯ) == 0) {
                ಮುಂದುವರಿ;
            }
        }

        asdl_seq_ಗಣ(ಮೌಲ್ಯಗಳು, current_pos++, elem);
    }

    ಪ್ರತಿಜ್ಞಾ(current_pos == n_elements);
    ಹಿಂತಿರುಗಿ ಮೌಲ್ಯಗಳು;
}

ಸ್ಥಿರ ಅಭಿವ್ಯಕ್ತಿ_ty
_build_concatenated_joined_str(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *ಅಕ್ಷರಸರಣಿಗಳು,
                               ಪೂರ್ಣಾಂಕ lineno, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಕೊನೆ_lineno,
                               ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, PyArena *arena)
{
    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *ಮೌಲ್ಯಗಳು = _build_concatenated_str(p, ಅಕ್ಷರಸರಣಿಗಳು, lineno,
        ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಕೊನೆ_lineno, ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, arena);
    ಆದರೆ (!ಮೌಲ್ಯಗಳು) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ _PyAST_JoinedStr(ಮೌಲ್ಯಗಳು, lineno, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಕೊನೆ_lineno, ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, p->arena);
}

ಅಭಿವ್ಯಕ್ತಿ_ty
_PyPegen_concatenate_tstrings(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *ಅಕ್ಷರಸರಣಿಗಳು,
                               ಪೂರ್ಣಾಂಕ lineno, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಕೊನೆ_lineno,
                               ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, PyArena *arena)
{
    asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *ಮೌಲ್ಯಗಳು = _build_concatenated_str(p, ಅಕ್ಷರಸರಣಿಗಳು, lineno,
        ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಕೊನೆ_lineno, ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, arena);
    ಆದರೆ (!ಮೌಲ್ಯಗಳು) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ _PyAST_TemplateStr(ಮೌಲ್ಯಗಳು, lineno, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಕೊನೆ_lineno,
        ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, arena);
}

ಅಭಿವ್ಯಕ್ತಿ_ty
Py_Pegen_concatenate_ಅಕ್ಷರಸರಣಿಗಳು(Parser *p, asdl_ಅಭಿವ್ಯಕ್ತಿ_seq *ಅಕ್ಷರಸರಣಿಗಳು,
                             ಪೂರ್ಣಾಂಕ lineno, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಕೊನೆ_lineno,
                             ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(ಅಕ್ಷರಸರಣಿಗಳು);
    ಪ್ರತಿಜ್ಞಾ(len > 0);

    ಪೂರ್ಣಾಂಕ f_string_ಸಿಕ್ಕಿತು = 0;
    ಪೂರ್ಣಾಂಕ unicode_string_ಸಿಕ್ಕಿತು = 0;
    ಪೂರ್ಣಾಂಕ ಬೈಟುಗಳು_ಸಿಕ್ಕಿತು = 0;

    Py_ssize_t i = 0;
    ಗಾಗಿ (i = 0; i < len; i++) {
        ಅಭಿವ್ಯಕ್ತಿ_ty elem = asdl_seq_GET(ಅಕ್ಷರಸರಣಿಗಳು, i);
        ಆಯ್ಕೆ(elem->ಬಗೆ) {
            ಸ್ಥಿತಿ Constant_ಬಗೆ:
                ಆದರೆ (Py_ಬೈಟುಗಳು_Check_Exact(elem->v.Constant.ಮೌಲ್ಯ)) {
                    ಬೈಟುಗಳು_ಸಿಕ್ಕಿತು = 1;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    unicode_string_ಸಿಕ್ಕಿತು = 1;
                }
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ Joined_Str_ಬಗೆ:
                f_string_ಸಿಕ್ಕಿತು = 1;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ Template_Str_ಬಗೆ:
                // python.gram handles this; we should never get here
                ಪ್ರತಿಜ್ಞಾ(0);
                ನಿಲ್ಲಿಸು;
            ಪೂರ್ವನಿಯೋಜಿತ:
                f_string_ಸಿಕ್ಕಿತು = 1;
                ನಿಲ್ಲಿಸು;
        }
    }

    // Cannot mix unicode and bytes
    ಆದರೆ ((unicode_string_ಸಿಕ್ಕಿತು || f_string_ಸಿಕ್ಕಿತು) && ಬೈಟುಗಳು_ಸಿಕ್ಕಿತು) {
        RAISE_SYNTAX_ERROR("cannot mix bytes and nonbytes literals");
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // If it's only bytes or only unicode string, do a simple concat
    ಆದರೆ (!f_string_ಸಿಕ್ಕಿತು) {
        ಆದರೆ (len == 1) {
            ಹಿಂತಿರುಗಿ asdl_seq_GET(ಅಕ್ಷರಸರಣಿಗಳು, 0);
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬೈಟುಗಳು_ಸಿಕ್ಕಿತು) {
            ಹಿಂತಿರುಗಿ build_concatenated_ಬೈಟುಗಳು(p, ಅಕ್ಷರಸರಣಿಗಳು, lineno, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                ಕೊನೆ_lineno, ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, arena);
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ _build_concatenated_unicode(p, ಅಕ್ಷರಸರಣಿಗಳು, lineno, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                ಕೊನೆ_lineno, ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, arena);
        }
    }

    ಹಿಂತಿರುಗಿ _build_concatenated_joined_str(p, ಅಕ್ಷರಸರಣಿಗಳು, lineno,
        ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಕೊನೆ_lineno, ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, arena);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
warn_relative_import_ದ_lazy(Parser *p, asdl_seq *dots, ಅಭಿವ್ಯಕ್ತಿ_ty module)
{
    // Warn about `from . lazy import x`: the whitespace between the dots and
    // the module name is insignificant, so this is parsed exactly like
    // `from .lazy import x` (an import of the relative module "lazy"), but it
    // is most likely a transposition of `lazy from . import x` (PEP 810).
    ಆದರೆ (p->call_invalid_ನಿಯಮಗಳು) {
        ಹಿಂತಿರುಗಿ 0;
    }

    // Only fire if there is whitespace between the last dot and the name,
    // i.e. not for the common `from .lazy import x` spelling.
    Token *ಕೊನೆಯ_dot = asdl_seq_GET_UNTYPED(dots, asdl_seq_LEN(dots) - 1);
    ಆದರೆ (
        ಕೊನೆಯ_dot->ಕೊನೆ_lineno == module->lineno
        && ಕೊನೆಯ_dot->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ == module->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ
    ) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = Py_Pegen_seq_ಎಣಿಕೆ_dots(dots);
    ಅಕ್ಷರ *buf = PyMem_RawMalloc(ಎಣಿಕೆ_2 + 1);
    ಆದರೆ (buf == ಖಾಲಿ) {
        Py_Err_No_ಸ್ಮರಣೆ();
        ಹಿಂತಿರುಗಿ -1;
    }
    memset(buf, '.', ಎಣಿಕೆ_2);
    buf[ಎಣಿಕೆ_2] = '\0';

    PyObject *ಸಂದೇಶ = PyUnicode_FromFormat(
        "'from %s lazy import' is the same as 'from %slazy import'; "
        "did you mean 'lazy from %s import'?",
        buf, buf, buf);
    PyMem_RawFree(buf);
    ಆದರೆ (ಸಂದೇಶ == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ -1;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    ಪೂರ್ಣಾಂಕ res = _PyErr_EmitSyntaxWarning(ಸಂದೇಶ,
                                       info.filename,
                                       module->lineno,
                                       module->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ + 1,
                                       module->ಕೊನೆ_lineno,
                                       module->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ + 1,
                                       info.module);
    Py_DECREF(ಸಂದೇಶ);
    ಹಿಂತಿರುಗಿ res;
}

stmt_ty
_PyPegen_checked_from_import(Parser *p, asdl_seq *dots, ಅಭಿವ್ಯಕ್ತಿ_ty module_name,
                             asdl_alias_seq *ಹೆಸರುಗಳು, ಅಭಿವ್ಯಕ್ತಿ_ty lazy_token, ಪೂರ್ಣಾಂಕ lineno,
                             ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಕೊನೆ_lineno, ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                             PyArena *arena)
{
    identifier module = module_name->v.Name.id;
    ಪೂರ್ಣಾಂಕ level = Py_Pegen_seq_ಎಣಿಕೆ_dots(dots);
    ಆದರೆ (level == 0 && PyUnicode_CompareWithASCIIString(module, "__future__") == 0) {
        ಆದರೆ (lazy_token) {
            RAISE_SYNTAX_ERROR_ತಿಳಿದ_LOCATION(lazy_token,
                "lazy from __future__ import is not allowed");
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
        ಗಾಗಿ (Py_ssize_t i = 0; i < asdl_seq_LEN(ಹೆಸರುಗಳು); i++) {
            alias_ty alias = asdl_seq_GET(ಹೆಸರುಗಳು, i);
            ಆದರೆ (PyUnicode_CompareWithASCIIString(alias->name, "barry_as_FLUFL") == 0) {
                p->flags |= PyPARSE_BARRY_AS_BDFL;
                p->tok->barry_as_bdfl = 1;
            }
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (
        level > 0
        && lazy_token == ಖಾಲಿ
        && PyUnicode_CompareWithASCIIString(module, "lazy") == 0
    ) {
        ಆದರೆ (warn_relative_import_ದ_lazy(p, dots, module_name) < 0) {
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
    }
    ಹಿಂತಿರುಗಿ _PyAST_ImportFrom(module, ಹೆಸರುಗಳು, level, lazy_token ? 1 : 0, lineno,
                             ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, ಕೊನೆ_lineno, ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, arena);
}

asdl_stmt_seq*
Py_Pegen_ರಿಜಿಸ್ಟರ್_stmts(Parser *p, asdl_stmt_seq* stmts) {
    ಆದರೆ (!p->call_invalid_ನಿಯಮಗಳು) {
        ಹಿಂತಿರುಗಿ stmts;
    }
    Py_ssize_t len = asdl_seq_LEN(stmts);
    ಆದರೆ (len == 0) {
        ಹಿಂತಿರುಗಿ stmts;
    }
    stmt_ty ಕೊನೆಯ_stmt = asdl_seq_GET(stmts, len - 1);
    ಆದರೆ (p->ಕೊನೆಯ_stmt_location.lineno > ಕೊನೆಯ_stmt->lineno) {
        ಹಿಂತಿರುಗಿ stmts;
    }
    p->ಕೊನೆಯ_stmt_location.lineno = ಕೊನೆಯ_stmt->lineno;
    p->ಕೊನೆಯ_stmt_location.ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = ಕೊನೆಯ_stmt->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;
    p->ಕೊನೆಯ_stmt_location.ಕೊನೆ_lineno = ಕೊನೆಯ_stmt->ಕೊನೆ_lineno;
    p->ಕೊನೆಯ_stmt_location.ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = ಕೊನೆಯ_stmt->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;
    ಹಿಂತಿರುಗಿ stmts;
}
