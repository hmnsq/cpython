#ਅਨ੍ਤਰ੍ਭਾਵ <Python.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_pystate.h"         // _PyInterpreterState_GET()
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_runtime.h"         // _PyRuntime
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_unicodeobject.h"   // _PyUnicode_InternImmortal()
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_pyerrors.h"        // _PyErr_EmitSyntaxWarning()

#ਅਨ੍ਤਰ੍ਭਾਵ "pegen.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string_parser.h"          // _PyPegen_decode_string()
#ਅਨ੍ਤਰ੍ਭਾਵ "lexer/state.h"            // tok_state


ਖਾਲੀ *
_PyPegen_dummy_name(Parser *p, ...)
{
    ਵਾਪਸ &_PyRuntime.parser.dummy_name;
}

/* Creates a single-element asdl_seq* that contains a */
asdl_seq *
_PyPegen_singleton_seq(Parser *p, ਖਾਲੀ *a)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(a != ਨਿਰਾ);
    asdl_seq *seq = (asdl_seq*)_Py_asdl_generic_seq_new(1, p->arena);
    ਜੇ (!seq) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    asdl_seq_ਸੈੱਟ_UNTYPED(seq, 0, a);
    ਵਾਪਸ seq;
}

/* Creates a copy of seq and prepends a to it */
asdl_seq *
Py_Pegen_seq_ਪਾਓ_ਵਿੱਚ_front(Parser *p, ਖਾਲੀ *a, asdl_seq *seq)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(a != ਨਿਰਾ);
    ਜੇ (!seq) {
        ਵਾਪਸ _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    ਜੇ (!new_seq) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    asdl_seq_ਸੈੱਟ_UNTYPED(new_seq, 0, a);
    ਲਈ (Py_ssize_t i = 1, l = asdl_seq_LEN(new_seq); i < l; i++) {
        asdl_seq_ਸੈੱਟ_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i - 1));
    }
    ਵਾਪਸ new_seq;
}

/* Creates a copy of seq and appends a to it */
asdl_seq *
Py_Pegen_seq_append_ਮੰਜ਼ਿਲ_ਸਿਰਾ(Parser *p, asdl_seq *seq, ਖਾਲੀ *a)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(a != ਨਿਰਾ);
    ਜੇ (!seq) {
        ਵਾਪਸ _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    ਜੇ (!new_seq) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    ਲਈ (Py_ssize_t i = 0, l = asdl_seq_LEN(new_seq); i + 1 < l; i++) {
        asdl_seq_ਸੈੱਟ_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i));
    }
    asdl_seq_ਸੈੱਟ_UNTYPED(new_seq, asdl_seq_LEN(new_seq) - 1, a);
    ਵਾਪਸ new_seq;
}

ਸਥਿਰ Py_ssize_t
get_flattened_seq_ਮਾਪ(asdl_seq *seqs)
{
    Py_ssize_t ਮਾਪ = 0;
    ਲਈ (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        ਮਾਪ += asdl_seq_LEN(inner_seq);
    }
    ਵਾਪਸ ਮਾਪ;
}

/* Flattens an asdl_seq* of asdl_seq*s */
asdl_seq *
_PyPegen_seq_flatten(Parser *p, asdl_seq *seqs)
{
    Py_ssize_t flattened_seq_ਮਾਪ = get_flattened_seq_ਮਾਪ(seqs);
    ਪ੍ਰਤਿਜ੍ਞਾ(flattened_seq_ਮਾਪ > 0);

    asdl_seq *flattened_seq = (asdl_seq*)_Py_asdl_generic_seq_new(flattened_seq_ਮਾਪ, p->arena);
    ਜੇ (!flattened_seq) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    ਪੂਰਨ flattened_seq_idx = 0;
    ਲਈ (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        ਲਈ (Py_ssize_t j = 0, li = asdl_seq_LEN(inner_seq); j < li; j++) {
            asdl_seq_ਸੈੱਟ_UNTYPED(flattened_seq, flattened_seq_idx++, asdl_seq_GET_UNTYPED(inner_seq, j));
        }
    }
    ਪ੍ਰਤਿਜ੍ਞਾ(flattened_seq_idx == flattened_seq_ਮਾਪ);

    ਵਾਪਸ flattened_seq;
}

ਖਾਲੀ *
Py_Pegen_seq_ਆਖਰੀ_ਵਸਤੂ(asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ਵਾਪਸ asdl_seq_GET_UNTYPED(seq, len - 1);
}

ਖਾਲੀ *
Py_Pegen_seq_ਪਹਿਲਾ_ਵਸਤੂ(asdl_seq *seq)
{
    ਵਾਪਸ asdl_seq_GET_UNTYPED(seq, 0);
}

/* Creates a new name of the form <first_name>.<second_name> */
ਵਿਅੰਜਕ_ty
Py_Pegen_join_ਨਾਂ_with_dot(Parser *p, ਵਿਅੰਜਕ_ty ਪਹਿਲਾ_name, ਵਿਅੰਜਕ_ty ਦੂਜਾ_name)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(ਪਹਿਲਾ_name != ਨਿਰਾ && ਦੂਜਾ_name != ਨਿਰਾ);
    PyObject *uni = PyUnicode_FromFormat("%U.%U",
            ਪਹਿਲਾ_name->v.Name.id, ਦੂਜਾ_name->v.Name.id);
    ਜੇ (!uni) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &uni);
    ਜੇ (Py_Arena_ਜੋੜੋ_Py_Object(p->arena, uni) < 0) {
        Py_DECREF(uni);
        ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਾਪਸ _PyAST_Name(uni, ਲੱਦੋ, EXTRA_ਵਿਅੰਜਕ(ਪਹਿਲਾ_name, ਦੂਜਾ_name));
}

/* Counts the total number of dots in seq's tokens */
ਪੂਰਨ
Py_Pegen_seq_ਗਿਣਤੀ_dots(asdl_seq *seq)
{
    ਪੂਰਨ ਸੰਖਿਆ_ਦਾ_dots = 0;
    ਲਈ (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        Token *current_ਵਿਅੰਜਕ = asdl_seq_GET_UNTYPED(seq, i);
        ਚੋਣ (current_ਵਿਅੰਜਕ->type) {
            ਹਾਲਤ ELLIPSIS:
                ਸੰਖਿਆ_ਦਾ_dots += 3;
                ਰੋਕੋ;
            ਹਾਲਤ DOT:
                ਸੰਖਿਆ_ਦਾ_dots += 1;
                ਰੋਕੋ;
            ਮੂਲ:
                Py_UNREACHABLE();
        }
    }

    ਵਾਪਸ ਸੰਖਿਆ_ਦਾ_dots;
}

/* Creates an alias with '*' as the identifier name */
alias_ty
Py_Pegen_alias_for_ਤਾਰਾ(Parser *p, ਪੂਰਨ lineno, ਪੂਰਨ ਖਾਨਾ_ਵਿੱਥ, ਪੂਰਨ ਸਿਰਾ_lineno,
                        ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, PyArena *arena) {
    PyObject *str = PyUnicode_InternFromString("*");
    ਜੇ (!str) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (Py_Arena_ਜੋੜੋ_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ _PyAST_alias(str, ਨਿਰਾ, lineno, ਖਾਨਾ_ਵਿੱਥ, ਸਿਰਾ_lineno, ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, arena);
}

/* Creates a new asdl_seq* with the identifiers of all the names in seq */
asdl_identifier_seq *
Py_Pegen_map_ਨਾਂ_ਮੰਜ਼ਿਲ_ids(Parser *p, asdl_ਵਿਅੰਜਕ_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ਪ੍ਰਤਿਜ੍ਞਾ(len > 0);

    asdl_identifier_seq *new_seq = _Py_asdl_identifier_seq_new(len, p->arena);
    ਜੇ (!new_seq) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਲਈ (Py_ssize_t i = 0; i < len; i++) {
        ਵਿਅੰਜਕ_ty e = asdl_seq_GET(seq, i);
        asdl_seq_ਸੈੱਟ(new_seq, i, e->v.Name.id);
    }
    ਵਾਪਸ new_seq;
}

/* Constructs a CmpopExprPair */
Cmpop_ਵਿਅੰਜਕ_Pair *
Py_Pegen_cmpop_ਵਿਅੰਜਕ_pair(Parser *p, cmpop_ty cmpop, ਵਿਅੰਜਕ_ty ਵਿਅੰਜਕ)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿਅੰਜਕ != ਨਿਰਾ);
    Cmpop_ਵਿਅੰਜਕ_Pair *a = _PyArena_Malloc(p->arena, ਆਕਾਰ(Cmpop_ਵਿਅੰਜਕ_Pair));
    ਜੇ (!a) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    a->cmpop = cmpop;
    a->ਵਿਅੰਜਕ = ਵਿਅੰਜਕ;
    ਵਾਪਸ a;
}

asdl_int_seq *
_PyPegen_get_cmpops(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ਪ੍ਰਤਿਜ੍ਞਾ(len > 0);

    asdl_int_seq *new_seq = _Py_asdl_int_seq_new(len, p->arena);
    ਜੇ (!new_seq) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਲਈ (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_ਵਿਅੰਜਕ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_ਸੈੱਟ(new_seq, i, pair->cmpop);
    }
    ਵਾਪਸ new_seq;
}

asdl_ਵਿਅੰਜਕ_seq *
_PyPegen_get_exprs(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ਪ੍ਰਤਿਜ੍ਞਾ(len > 0);

    asdl_ਵਿਅੰਜਕ_seq *new_seq = Py_asdl_ਵਿਅੰਜਕ_seq_new(len, p->arena);
    ਜੇ (!new_seq) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਲਈ (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_ਵਿਅੰਜਕ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_ਸੈੱਟ(new_seq, i, pair->ਵਿਅੰਜਕ);
    }
    ਵਾਪਸ new_seq;
}

/* Creates an asdl_seq* where all the elements have been changed to have ctx as context */
ਸਥਿਰ asdl_ਵਿਅੰਜਕ_seq *
ਸੈੱਟ_seq_context(Parser *p, asdl_ਵਿਅੰਜਕ_seq *seq, ਵਿਅੰਜਕ_context_ty ctx)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ਜੇ (len == 0) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    asdl_ਵਿਅੰਜਕ_seq *new_seq = Py_asdl_ਵਿਅੰਜਕ_seq_new(len, p->arena);
    ਜੇ (!new_seq) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਲਈ (Py_ssize_t i = 0; i < len; i++) {
        ਵਿਅੰਜਕ_ty e = asdl_seq_GET(seq, i);
        ਵਿਅੰਜਕ_ty new_e = Py_Pegen_ਸੈੱਟ_ਵਿਅੰਜਕ_context(p, e, ctx);
        ਜੇ (!new_e) {
            ਵਾਪਸ ਨਿਰਾ;
        }
        asdl_seq_ਸੈੱਟ(new_seq, i, new_e);
    }
    ਵਾਪਸ new_seq;
}

ਸਥਿਰ ਵਿਅੰਜਕ_ty
ਸੈੱਟ_name_context(Parser *p, ਵਿਅੰਜਕ_ty e, ਵਿਅੰਜਕ_context_ty ctx)
{
    ਵਾਪਸ _PyAST_Name(e->v.Name.id, ctx, EXTRA_ਵਿਅੰਜਕ(e, e));
}

ਸਥਿਰ ਵਿਅੰਜਕ_ty
ਸੈੱਟ_tuple_context(Parser *p, ਵਿਅੰਜਕ_ty e, ਵਿਅੰਜਕ_context_ty ctx)
{
    asdl_ਵਿਅੰਜਕ_seq *seq = ਸੈੱਟ_seq_context(p, e->v.Tuple.elts, ctx);
    ਜੇ (!seq && PyErr_Occurred()) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ _PyAST_Tuple(seq, ctx, EXTRA_ਵਿਅੰਜਕ(e, e));
}

ਸਥਿਰ ਵਿਅੰਜਕ_ty
ਸੈੱਟ_list_context(Parser *p, ਵਿਅੰਜਕ_ty e, ਵਿਅੰਜਕ_context_ty ctx)
{
    asdl_ਵਿਅੰਜਕ_seq *seq = ਸੈੱਟ_seq_context(p, e->v.List.elts, ctx);
    ਜੇ (!seq && PyErr_Occurred()) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ _PyAST_List(seq, ctx, EXTRA_ਵਿਅੰਜਕ(e, e));
}

ਸਥਿਰ ਵਿਅੰਜਕ_ty
ਸੈੱਟ_subscript_context(Parser *p, ਵਿਅੰਜਕ_ty e, ਵਿਅੰਜਕ_context_ty ctx)
{
    ਵਾਪਸ _PyAST_Subscript(e->v.Subscript.ਮੁੱਲ, e->v.Subscript.ਟੁਕੜਾ,
                            ctx, EXTRA_ਵਿਅੰਜਕ(e, e));
}

ਸਥਿਰ ਵਿਅੰਜਕ_ty
ਸੈੱਟ_attribute_context(Parser *p, ਵਿਅੰਜਕ_ty e, ਵਿਅੰਜਕ_context_ty ctx)
{
    ਵਾਪਸ _PyAST_Attribute(e->v.Attribute.ਮੁੱਲ, e->v.Attribute.attr,
                            ctx, EXTRA_ਵਿਅੰਜਕ(e, e));
}

ਸਥਿਰ ਵਿਅੰਜਕ_ty
ਸੈੱਟ_starred_context(Parser *p, ਵਿਅੰਜਕ_ty e, ਵਿਅੰਜਕ_context_ty ctx)
{
    ਵਿਅੰਜਕ_ty inner = Py_Pegen_ਸੈੱਟ_ਵਿਅੰਜਕ_context(p, e->v.Starred.ਮੁੱਲ, ctx);
    ਜੇ (!inner) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ _PyAST_Starred(inner, ctx, EXTRA_ਵਿਅੰਜਕ(e, e));
}

/* Creates an `expr_ty` equivalent to `expr` but with `ctx` as context */
ਵਿਅੰਜਕ_ty
Py_Pegen_ਸੈੱਟ_ਵਿਅੰਜਕ_context(Parser *p, ਵਿਅੰਜਕ_ty ਵਿਅੰਜਕ, ਵਿਅੰਜਕ_context_ty ctx)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿਅੰਜਕ != ਨਿਰਾ);

    ਵਿਅੰਜਕ_ty new = ਨਿਰਾ;
    ਚੋਣ (ਵਿਅੰਜਕ->ਕਿਸਮ) {
        ਹਾਲਤ Name_ਕਿਸਮ:
            new = ਸੈੱਟ_name_context(p, ਵਿਅੰਜਕ, ctx);
            ਰੋਕੋ;
        ਹਾਲਤ Tuple_ਕਿਸਮ:
            new = ਸੈੱਟ_tuple_context(p, ਵਿਅੰਜਕ, ctx);
            ਰੋਕੋ;
        ਹਾਲਤ List_ਕਿਸਮ:
            new = ਸੈੱਟ_list_context(p, ਵਿਅੰਜਕ, ctx);
            ਰੋਕੋ;
        ਹਾਲਤ Subscript_ਕਿਸਮ:
            new = ਸੈੱਟ_subscript_context(p, ਵਿਅੰਜਕ, ctx);
            ਰੋਕੋ;
        ਹਾਲਤ Attribute_ਕਿਸਮ:
            new = ਸੈੱਟ_attribute_context(p, ਵਿਅੰਜਕ, ctx);
            ਰੋਕੋ;
        ਹਾਲਤ Starred_ਕਿਸਮ:
            new = ਸੈੱਟ_starred_context(p, ਵਿਅੰਜਕ, ctx);
            ਰੋਕੋ;
        ਮੂਲ:
            new = ਵਿਅੰਜਕ;
    }
    ਵਾਪਸ new;
}

/* Invalid targets return NULL without raising, allowing parsing to backtrack.
   Copy valid targets to preserve the memoized expression's context. */
ਵਿਅੰਜਕ_ty
Py_Pegen_make_ਨਿਸ਼ਾਨਾ(Parser *p, ਵਿਅੰਜਕ_ty ਵਿਅੰਜਕ, TARGETS_TYPE targets_type)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿਅੰਜਕ != ਨਿਰਾ);
    ਪ੍ਰਤਿਜ੍ਞਾ(targets_type != FOR_TARGETS);
    ਜੇ (Py_Pegen_get_invalid_ਨਿਸ਼ਾਨਾ(ਵਿਅੰਜਕ, targets_type) != ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ Py_Pegen_ਸੈੱਟ_ਵਿਅੰਜਕ_context(
        p, ਵਿਅੰਜਕ, targets_type == DEL_TARGETS ? Del : Store);
}

/* Constructs a KeyValuePair that is used when parsing a dict's key value pairs */
ਕੁੰਜੀ_ਮੁੱਲ_Pair *
Py_Pegen_ਕੁੰਜੀ_ਮੁੱਲ_pair(Parser *p, ਵਿਅੰਜਕ_ty ਕੁੰਜੀ, ਵਿਅੰਜਕ_ty ਮੁੱਲ)
{
    ਕੁੰਜੀ_ਮੁੱਲ_Pair *a = _PyArena_Malloc(p->arena, ਆਕਾਰ(ਕੁੰਜੀ_ਮੁੱਲ_Pair));
    ਜੇ (!a) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    a->ਕੁੰਜੀ = ਕੁੰਜੀ;
    a->ਮੁੱਲ = ਮੁੱਲ;
    ਵਾਪਸ a;
}

/* Extracts all keys from an asdl_seq* of KeyValuePair*'s */
asdl_ਵਿਅੰਜਕ_seq *
Py_Pegen_get_ਕੁੰਜੀਆਂ(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_ਵਿਅੰਜਕ_seq *new_seq = Py_asdl_ਵਿਅੰਜਕ_seq_new(len, p->arena);
    ਜੇ (!new_seq) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਲਈ (Py_ssize_t i = 0; i < len; i++) {
        ਕੁੰਜੀ_ਮੁੱਲ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_ਸੈੱਟ(new_seq, i, pair->ਕੁੰਜੀ);
    }
    ਵਾਪਸ new_seq;
}

/* Extracts all values from an asdl_seq* of KeyValuePair*'s */
asdl_ਵਿਅੰਜਕ_seq *
Py_Pegen_get_ਮੁੱਲਾਂ(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_ਵਿਅੰਜਕ_seq *new_seq = Py_asdl_ਵਿਅੰਜਕ_seq_new(len, p->arena);
    ਜੇ (!new_seq) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਲਈ (Py_ssize_t i = 0; i < len; i++) {
        ਕੁੰਜੀ_ਮੁੱਲ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_ਸੈੱਟ(new_seq, i, pair->ਮੁੱਲ);
    }
    ਵਾਪਸ new_seq;
}

/* Constructs a KeyPatternPair that is used when parsing mapping & class patterns */
ਕੁੰਜੀ_ਨਮੂਨਾ_Pair *
Py_Pegen_ਕੁੰਜੀ_ਨਮੂਨਾ_pair(Parser *p, ਵਿਅੰਜਕ_ty ਕੁੰਜੀ, ਨਮੂਨਾ_ty ਨਮੂਨਾ)
{
    ਕੁੰਜੀ_ਨਮੂਨਾ_Pair *a = _PyArena_Malloc(p->arena, ਆਕਾਰ(ਕੁੰਜੀ_ਨਮੂਨਾ_Pair));
    ਜੇ (!a) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    a->ਕੁੰਜੀ = ਕੁੰਜੀ;
    a->ਨਮੂਨਾ = ਨਮੂਨਾ;
    ਵਾਪਸ a;
}

/* Extracts all keys from an asdl_seq* of KeyPatternPair*'s */
asdl_ਵਿਅੰਜਕ_seq *
Py_Pegen_get_ਨਮੂਨਾ_ਕੁੰਜੀਆਂ(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_ਵਿਅੰਜਕ_seq *new_seq = Py_asdl_ਵਿਅੰਜਕ_seq_new(len, p->arena);
    ਜੇ (!new_seq) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਲਈ (Py_ssize_t i = 0; i < len; i++) {
        ਕੁੰਜੀ_ਨਮੂਨਾ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_ਸੈੱਟ(new_seq, i, pair->ਕੁੰਜੀ);
    }
    ਵਾਪਸ new_seq;
}

/* Extracts all patterns from an asdl_seq* of KeyPatternPair*'s */
asdl_ਨਮੂਨਾ_seq *
_PyPegen_get_patterns(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_ਨਮੂਨਾ_seq *new_seq = Py_asdl_ਨਮੂਨਾ_seq_new(len, p->arena);
    ਜੇ (!new_seq) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਲਈ (Py_ssize_t i = 0; i < len; i++) {
        ਕੁੰਜੀ_ਨਮੂਨਾ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_ਸੈੱਟ(new_seq, i, pair->ਨਮੂਨਾ);
    }
    ਵਾਪਸ new_seq;
}

/* Constructs a NameDefaultPair */
NameDefaultPair *
_PyPegen_name_default_pair(Parser *p, arg_ty arg, ਵਿਅੰਜਕ_ty ਮੁੱਲ, Token *tc)
{
    NameDefaultPair *a = _PyArena_Malloc(p->arena, ਆਕਾਰ(NameDefaultPair));
    ਜੇ (!a) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    a->arg = Py_Pegen_ਜੋੜੋ_type_comment_ਮੰਜ਼ਿਲ_arg(p, arg, tc);
    ਜੇ (!a->arg) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    a->ਮੁੱਲ = ਮੁੱਲ;
    ਵਾਪਸ a;
}

/* Constructs a SlashWithDefault */
SlashWithDefault *
_PyPegen_slash_with_default(Parser *p, asdl_arg_seq *plain_ਨਾਂ, asdl_seq *ਨਾਂ_with_defaults)
{
    SlashWithDefault *a = _PyArena_Malloc(p->arena, ਆਕਾਰ(SlashWithDefault));
    ਜੇ (!a) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    a->plain_ਨਾਂ = plain_ਨਾਂ;
    a->ਨਾਂ_with_defaults = ਨਾਂ_with_defaults;
    ਵਾਪਸ a;
}

/* Constructs a StarEtc */
ਤਾਰਾ_Etc *
Py_Pegen_ਤਾਰਾ_etc(Parser *p, arg_ty vararg, asdl_seq *kwonlyargs, arg_ty kwarg)
{
    ਤਾਰਾ_Etc *a = _PyArena_Malloc(p->arena, ਆਕਾਰ(ਤਾਰਾ_Etc));
    ਜੇ (!a) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    a->vararg = vararg;
    a->kwonlyargs = kwonlyargs;
    a->kwarg = kwarg;
    ਵਾਪਸ a;
}

asdl_seq *
_PyPegen_join_sequences(Parser *p, asdl_seq *a, asdl_seq *b)
{
    Py_ssize_t ਪਹਿਲਾ_len = asdl_seq_LEN(a);
    Py_ssize_t ਦੂਜਾ_len = asdl_seq_LEN(b);
    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(ਪਹਿਲਾ_len + ਦੂਜਾ_len, p->arena);
    ਜੇ (!new_seq) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    ਪੂਰਨ k = 0;
    ਲਈ (Py_ssize_t i = 0; i < ਪਹਿਲਾ_len; i++) {
        asdl_seq_ਸੈੱਟ_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(a, i));
    }
    ਲਈ (Py_ssize_t i = 0; i < ਦੂਜਾ_len; i++) {
        asdl_seq_ਸੈੱਟ_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(b, i));
    }

    ਵਾਪਸ new_seq;
}

ਸਥਿਰ asdl_arg_seq*
get_ਨਾਂ(Parser *p, asdl_seq *ਨਾਂ_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(ਨਾਂ_with_defaults);
    asdl_arg_seq *seq = _Py_asdl_arg_seq_new(len, p->arena);
    ਜੇ (!seq) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਲਈ (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(ਨਾਂ_with_defaults, i);
        asdl_seq_ਸੈੱਟ(seq, i, pair->arg);
    }
    ਵਾਪਸ seq;
}

ਸਥਿਰ asdl_ਵਿਅੰਜਕ_seq *
_get_defaults(Parser *p, asdl_seq *ਨਾਂ_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(ਨਾਂ_with_defaults);
    asdl_ਵਿਅੰਜਕ_seq *seq = Py_asdl_ਵਿਅੰਜਕ_seq_new(len, p->arena);
    ਜੇ (!seq) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਲਈ (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(ਨਾਂ_with_defaults, i);
        asdl_seq_ਸੈੱਟ(seq, i, pair->ਮੁੱਲ);
    }
    ਵਾਪਸ seq;
}

ਸਥਿਰ ਪੂਰਨ
_make_posonlyargs(Parser *p,
                  asdl_arg_seq *slash_without_default,
                  SlashWithDefault *slash_with_default,
                  asdl_arg_seq **posonlyargs) {
    ਜੇ (slash_without_default != ਨਿਰਾ) {
        *posonlyargs = slash_without_default;
    }
    ਨਹੀਂਤਾਂ ਜੇ (slash_with_default != ਨਿਰਾ) {
        asdl_arg_seq *slash_with_default_ਨਾਂ =
                get_ਨਾਂ(p, slash_with_default->ਨਾਂ_with_defaults);
        ਜੇ (!slash_with_default_ਨਾਂ) {
            ਵਾਪਸ -1;
        }
        *posonlyargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default->plain_ਨਾਂ,
                (asdl_seq*)slash_with_default_ਨਾਂ);
    }
    ਨਹੀਂਤਾਂ {
        *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }
    ਵਾਪਸ *posonlyargs == ਨਿਰਾ ? -1 : 0;
}

ਸਥਿਰ ਪੂਰਨ
_make_posargs(Parser *p,
              asdl_arg_seq *plain_ਨਾਂ,
              asdl_seq *ਨਾਂ_with_default,
              asdl_arg_seq **posargs) {

    ਜੇ (ਨਾਂ_with_default != ਨਿਰਾ) {
        ਜੇ (plain_ਨਾਂ != ਨਿਰਾ) {
            asdl_arg_seq *ਨਾਂ_with_default_ਨਾਂ = get_ਨਾਂ(p, ਨਾਂ_with_default);
            ਜੇ (!ਨਾਂ_with_default_ਨਾਂ) {
                ਵਾਪਸ -1;
            }
            *posargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                    p,(asdl_seq*)plain_ਨਾਂ, (asdl_seq*)ਨਾਂ_with_default_ਨਾਂ);
        }
        ਨਹੀਂਤਾਂ {
            *posargs = get_ਨਾਂ(p, ਨਾਂ_with_default);
        }
    }
    ਨਹੀਂਤਾਂ {
        ਜੇ (plain_ਨਾਂ != ਨਿਰਾ) {
            // With the current grammar, we never get here.
            // If that has changed, remove the assert, and test thoroughly.
            ਪ੍ਰਤਿਜ੍ਞਾ(0);
            *posargs = plain_ਨਾਂ;
        }
        ਨਹੀਂਤਾਂ {
            *posargs = _Py_asdl_arg_seq_new(0, p->arena);
        }
    }
    ਵਾਪਸ *posargs == ਨਿਰਾ ? -1 : 0;
}

ਸਥਿਰ ਪੂਰਨ
_make_posdefaults(Parser *p,
                  SlashWithDefault *slash_with_default,
                  asdl_seq *ਨਾਂ_with_default,
                  asdl_ਵਿਅੰਜਕ_seq **posdefaults) {
    ਜੇ (slash_with_default != ਨਿਰਾ && ਨਾਂ_with_default != ਨਿਰਾ) {
        asdl_ਵਿਅੰਜਕ_seq *slash_with_default_ਮੁੱਲਾਂ =
                _get_defaults(p, slash_with_default->ਨਾਂ_with_defaults);
        ਜੇ (!slash_with_default_ਮੁੱਲਾਂ) {
            ਵਾਪਸ -1;
        }
        asdl_ਵਿਅੰਜਕ_seq *ਨਾਂ_with_default_ਮੁੱਲਾਂ = _get_defaults(p, ਨਾਂ_with_default);
        ਜੇ (!ਨਾਂ_with_default_ਮੁੱਲਾਂ) {
            ਵਾਪਸ -1;
        }
        *posdefaults = (asdl_ਵਿਅੰਜਕ_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default_ਮੁੱਲਾਂ,
                (asdl_seq*)ਨਾਂ_with_default_ਮੁੱਲਾਂ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (slash_with_default == ਨਿਰਾ && ਨਾਂ_with_default != ਨਿਰਾ) {
        *posdefaults = _get_defaults(p, ਨਾਂ_with_default);
    }
    ਨਹੀਂਤਾਂ ਜੇ (slash_with_default != ਨਿਰਾ && ਨਾਂ_with_default == ਨਿਰਾ) {
        *posdefaults = _get_defaults(p, slash_with_default->ਨਾਂ_with_defaults);
    }
    ਨਹੀਂਤਾਂ {
        *posdefaults = Py_asdl_ਵਿਅੰਜਕ_seq_new(0, p->arena);
    }
    ਵਾਪਸ *posdefaults == ਨਿਰਾ ? -1 : 0;
}

ਸਥਿਰ ਪੂਰਨ
_make_kwargs(Parser *p, ਤਾਰਾ_Etc *ਤਾਰਾ_etc,
             asdl_arg_seq **kwonlyargs,
             asdl_ਵਿਅੰਜਕ_seq **kwdefaults) {
    ਜੇ (ਤਾਰਾ_etc != ਨਿਰਾ && ਤਾਰਾ_etc->kwonlyargs != ਨਿਰਾ) {
        *kwonlyargs = get_ਨਾਂ(p, ਤਾਰਾ_etc->kwonlyargs);
    }
    ਨਹੀਂਤਾਂ {
        *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }

    ਜੇ (*kwonlyargs == ਨਿਰਾ) {
        ਵਾਪਸ -1;
    }

    ਜੇ (ਤਾਰਾ_etc != ਨਿਰਾ && ਤਾਰਾ_etc->kwonlyargs != ਨਿਰਾ) {
        *kwdefaults = _get_defaults(p, ਤਾਰਾ_etc->kwonlyargs);
    }
    ਨਹੀਂਤਾਂ {
        *kwdefaults = Py_asdl_ਵਿਅੰਜਕ_seq_new(0, p->arena);
    }

    ਜੇ (*kwdefaults == ਨਿਰਾ) {
        ਵਾਪਸ -1;
    }

    ਵਾਪਸ 0;
}

/* Constructs an arguments_ty object out of all the parsed constructs in the parameters rule */
arguments_ty
_PyPegen_make_arguments(Parser *p, asdl_arg_seq *slash_without_default,
                        SlashWithDefault *slash_with_default, asdl_arg_seq *plain_ਨਾਂ,
                        asdl_seq *ਨਾਂ_with_default, ਤਾਰਾ_Etc *ਤਾਰਾ_etc)
{
    asdl_arg_seq *posonlyargs;
    ਜੇ (_make_posonlyargs(p, slash_without_default, slash_with_default, &posonlyargs) == -1) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    asdl_arg_seq *posargs;
    ਜੇ (_make_posargs(p, plain_ਨਾਂ, ਨਾਂ_with_default, &posargs) == -1) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    asdl_ਵਿਅੰਜਕ_seq *posdefaults;
    ਜੇ (_make_posdefaults(p,slash_with_default, ਨਾਂ_with_default, &posdefaults) == -1) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    arg_ty vararg = ਨਿਰਾ;
    ਜੇ (ਤਾਰਾ_etc != ਨਿਰਾ && ਤਾਰਾ_etc->vararg != ਨਿਰਾ) {
        vararg = ਤਾਰਾ_etc->vararg;
    }

    asdl_arg_seq *kwonlyargs;
    asdl_ਵਿਅੰਜਕ_seq *kwdefaults;
    ਜੇ (_make_kwargs(p, ਤਾਰਾ_etc, &kwonlyargs, &kwdefaults) == -1) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    arg_ty kwarg = ਨਿਰਾ;
    ਜੇ (ਤਾਰਾ_etc != ਨਿਰਾ && ਤਾਰਾ_etc->kwarg != ਨਿਰਾ) {
        kwarg = ਤਾਰਾ_etc->kwarg;
    }

    ਵਾਪਸ _PyAST_arguments(posonlyargs, posargs, vararg, kwonlyargs,
                            kwdefaults, kwarg, posdefaults, p->arena);
}


/* Constructs an empty arguments_ty object, that gets used when a function accepts no
 * arguments. */
arguments_ty
Py_Pegen_ਖਾਲੀ_ਥਾਵਾਂ_arguments(Parser *p)
{
    asdl_arg_seq *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    ਜੇ (!posonlyargs) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    asdl_arg_seq *posargs = _Py_asdl_arg_seq_new(0, p->arena);
    ਜੇ (!posargs) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    asdl_ਵਿਅੰਜਕ_seq *posdefaults = Py_asdl_ਵਿਅੰਜਕ_seq_new(0, p->arena);
    ਜੇ (!posdefaults) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    asdl_arg_seq *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    ਜੇ (!kwonlyargs) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    asdl_ਵਿਅੰਜਕ_seq *kwdefaults = Py_asdl_ਵਿਅੰਜਕ_seq_new(0, p->arena);
    ਜੇ (!kwdefaults) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਾਪਸ _PyAST_arguments(posonlyargs, posargs, ਨਿਰਾ, kwonlyargs,
                            kwdefaults, ਨਿਰਾ, posdefaults, p->arena);
}

/* Encapsulates the value of an operator_ty into an AugOperator struct */
AugOperator *
_PyPegen_augoperator(Parser *p, operator_ty ਕਿਸਮ)
{
    AugOperator *a = _PyArena_Malloc(p->arena, ਆਕਾਰ(AugOperator));
    ਜੇ (!a) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    a->ਕਿਸਮ = ਕਿਸਮ;
    ਵਾਪਸ a;
}

/* Construct a FunctionDef equivalent to function_def, but with decorators */
stmt_ty
_PyPegen_function_def_decorators(Parser *p, asdl_ਵਿਅੰਜਕ_seq *decorators, stmt_ty function_def)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(function_def != ਨਿਰਾ);
    ਜੇ (function_def->ਕਿਸਮ == Async_Function_Def_ਕਿਸਮ) {
        ਵਾਪਸ _PyAST_AsyncFunctionDef(
            function_def->v.AsyncFunctionDef.name,
            function_def->v.AsyncFunctionDef.ਦਲੀਲਾਂ,
            function_def->v.AsyncFunctionDef.body, decorators,
            function_def->v.AsyncFunctionDef.returns,
            function_def->v.AsyncFunctionDef.type_comment,
            function_def->v.AsyncFunctionDef.type_params,
            function_def->lineno, function_def->ਖਾਨਾ_ਵਿੱਥ,
            function_def->ਸਿਰਾ_lineno, function_def->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, p->arena);
    }

    ਵਾਪਸ _PyAST_FunctionDef(
        function_def->v.FunctionDef.name,
        function_def->v.FunctionDef.ਦਲੀਲਾਂ,
        function_def->v.FunctionDef.body, decorators,
        function_def->v.FunctionDef.returns,
        function_def->v.FunctionDef.type_comment,
        function_def->v.FunctionDef.type_params,
        function_def->lineno, function_def->ਖਾਨਾ_ਵਿੱਥ,
        function_def->ਸਿਰਾ_lineno, function_def->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, p->arena);
}

/* Construct a ClassDef equivalent to class_def, but with decorators */
stmt_ty
_PyPegen_class_def_decorators(Parser *p, asdl_ਵਿਅੰਜਕ_seq *decorators, stmt_ty class_def)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(class_def != ਨਿਰਾ);
    ਵਾਪਸ _PyAST_ClassDef(
        class_def->v.ClassDef.name,
        class_def->v.ClassDef.bases, class_def->v.ClassDef.keywords,
        class_def->v.ClassDef.body, decorators,
        class_def->v.ClassDef.type_params,
        class_def->lineno, class_def->ਖਾਨਾ_ਵਿੱਥ, class_def->ਸਿਰਾ_lineno,
        class_def->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, p->arena);
}

/* Construct a KeywordOrStarred */
KeywordOrStarred *
_PyPegen_keyword_or_starred(Parser *p, ਖਾਲੀ *element, ਪੂਰਨ is_keyword)
{
    KeywordOrStarred *a = _PyArena_Malloc(p->arena, ਆਕਾਰ(KeywordOrStarred));
    ਜੇ (!a) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    a->element = element;
    a->is_keyword = is_keyword;
    ਵਾਪਸ a;
}

/* Get the number of starred expressions in an asdl_seq* of KeywordOrStarred*s */
ਸਥਿਰ ਪੂਰਨ
seq_ਸੰਖਿਆ_ਦਾ_starred_exprs(asdl_seq *seq)
{
    ਪੂਰਨ n = 0;
    ਲਈ (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(seq, i);
        ਜੇ (!k->is_keyword) {
            n++;
        }
    }
    ਵਾਪਸ n;
}

/* Extract the starred expressions of an asdl_seq* of KeywordOrStarred*s */
asdl_ਵਿਅੰਜਕ_seq *
Py_Pegen_seq_ਕੱਢੋ_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    ਪੂਰਨ new_len = seq_ਸੰਖਿਆ_ਦਾ_starred_exprs(kwargs);
    ਜੇ (new_len == 0) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    asdl_ਵਿਅੰਜਕ_seq *new_seq = Py_asdl_ਵਿਅੰਜਕ_seq_new(new_len, p->arena);
    ਜੇ (!new_seq) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    ਪੂਰਨ idx = 0;
    ਲਈ (Py_ssize_t i = 0, len = asdl_seq_LEN(kwargs); i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        ਜੇ (!k->is_keyword) {
            asdl_seq_ਸੈੱਟ(new_seq, idx++, k->element);
        }
    }
    ਵਾਪਸ new_seq;
}

/* Return a new asdl_seq* with only the keywords in kwargs */
asdl_keyword_seq*
_PyPegen_seq_delete_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    Py_ssize_t len = asdl_seq_LEN(kwargs);
    Py_ssize_t new_len = len - seq_ਸੰਖਿਆ_ਦਾ_starred_exprs(kwargs);
    ਜੇ (new_len == 0) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    asdl_keyword_seq *new_seq = _Py_asdl_keyword_seq_new(new_len, p->arena);
    ਜੇ (!new_seq) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    ਪੂਰਨ idx = 0;
    ਲਈ (Py_ssize_t i = 0; i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        ਜੇ (k->is_keyword) {
            asdl_seq_ਸੈੱਟ(new_seq, idx++, k->element);
        }
    }
    ਵਾਪਸ new_seq;
}

ਵਿਅੰਜਕ_ty
_PyPegen_ensure_imaginary(Parser *p, ਵਿਅੰਜਕ_ty ਘਾਤਾਂਕ)
{
    ਜੇ (ਘਾਤਾਂਕ->ਕਿਸਮ != Constant_ਕਿਸਮ || !PyComplex_CheckExact(ਘਾਤਾਂਕ->v.Constant.ਮੁੱਲ)) {
        RAISE_SYNTAX_ERROR_ਗਿਆਤ_LOCATION(ਘਾਤਾਂਕ, "imaginary number required in complex literal");
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ ਘਾਤਾਂਕ;
}

ਵਿਅੰਜਕ_ty
_PyPegen_ensure_real(Parser *p, ਵਿਅੰਜਕ_ty ਘਾਤਾਂਕ)
{
    ਜੇ (ਘਾਤਾਂਕ->ਕਿਸਮ != Constant_ਕਿਸਮ || PyComplex_CheckExact(ਘਾਤਾਂਕ->v.Constant.ਮੁੱਲ)) {
        RAISE_SYNTAX_ERROR_ਗਿਆਤ_LOCATION(ਘਾਤਾਂਕ, "real number required in complex literal");
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ ਘਾਤਾਂਕ;
}

mod_ty
_PyPegen_make_module(Parser *p, asdl_stmt_seq *a) {
    asdl_type_ignore_seq *type_ignores = ਨਿਰਾ;
    Py_ssize_t ਅੰਕ = p->type_ignore_comments.ਅੰਕ_ਚੀਜ਼ਾਂ;
    ਜੇ (ਅੰਕ > 0) {
        // Turn the raw (comment, lineno) pairs into TypeIgnore objects in the arena
        type_ignores = _Py_asdl_type_ignore_seq_new(ਅੰਕ, p->arena);
        ਜੇ (type_ignores == ਨਿਰਾ) {
            ਵਾਪਸ ਨਿਰਾ;
        }
        ਲਈ (Py_ssize_t i = 0; i < ਅੰਕ; i++) {
            PyObject *ਟੈਗ = _PyPegen_new_type_comment(p, p->type_ignore_comments.ਚੀਜ਼ਾਂ[i].comment);
            ਜੇ (ਟੈਗ == ਨਿਰਾ) {
                ਵਾਪਸ ਨਿਰਾ;
            }
            type_ignore_ty ti = _PyAST_TypeIgnore(p->type_ignore_comments.ਚੀਜ਼ਾਂ[i].lineno,
                                                  ਟੈਗ, p->arena);
            ਜੇ (ti == ਨਿਰਾ) {
                ਵਾਪਸ ਨਿਰਾ;
            }
            asdl_seq_ਸੈੱਟ(type_ignores, i, ti);
        }
    }
    ਵਾਪਸ _PyAST_Module(a, type_ignores, p->arena);
}

PyObject *
_PyPegen_new_type_comment(Parser *p, ਅਚਲ ਅੱਖਰ *s)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, ਲੜੀ_ਲੰਬਾਈ(s), ਨਿਰਾ);
    ਜੇ (res == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (Py_Arena_ਜੋੜੋ_Py_Object(p->arena, res) < 0) {
        Py_DECREF(res);
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ res;
}

arg_ty
Py_Pegen_ਜੋੜੋ_type_comment_ਮੰਜ਼ਿਲ_arg(Parser *p, arg_ty a, Token *tc)
{
    ਜੇ (tc == ਨਿਰਾ) {
        ਵਾਪਸ a;
    }
    ਅਚਲ ਅੱਖਰ *ਬਾਈਟਾਂ = Py_ਬਾਈਟਾਂ_As_String(tc->ਬਾਈਟਾਂ);
    ਜੇ (ਬਾਈਟਾਂ == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, ਬਾਈਟਾਂ);
    ਜੇ (tco == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ _PyAST_arg(a->arg, a->annotation, tco,
                      a->lineno, a->ਖਾਨਾ_ਵਿੱਥ, a->ਸਿਰਾ_lineno, a->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ,
                      p->arena);
}

/* Checks if the NOTEQUAL token is valid given the current parser flags
0 indicates success and nonzero indicates failure (an exception may be set) */
ਪੂਰਨ
_PyPegen_check_barry_as_flufl(Parser *p, Token* t) {
    ਪ੍ਰਤਿਜ੍ਞਾ(t->ਬਾਈਟਾਂ != ਨਿਰਾ);
    ਪ੍ਰਤਿਜ੍ਞਾ(t->type == NOTEQUAL);

    ਅਚਲ ਅੱਖਰ* tok_str = Py_ਬਾਈਟਾਂ_AS_STRING(t->ਬਾਈਟਾਂ);
    ਜੇ (p->flags & PyPARSE_BARRY_AS_BDFL && strcmp(tok_str, "<>") != 0) {
        RAISE_SYNTAX_ERROR("with Barry as BDFL, use '<>' instead of '!='");
        ਵਾਪਸ -1;
    }
    ਜੇ (!(p->flags & PyPARSE_BARRY_AS_BDFL)) {
        ਵਾਪਸ strcmp(tok_str, "!=");
    }
    ਵਾਪਸ 0;
}

ਪੂਰਨ
_PyPegen_check_legacy_stmt(Parser *p, ਵਿਅੰਜਕ_ty name) {
    ਜੇ (name->ਕਿਸਮ != Name_ਕਿਸਮ) {
        ਵਾਪਸ 0;
    }
    ਅਚਲ ਅੱਖਰ* candidates[2] = {"print", "exec"};
    ਲਈ (ਪੂਰਨ i=0; i<2; i++) {
        ਜੇ (PyUnicode_CompareWithASCIIString(name->v.Name.id, candidates[i]) == 0) {
            ਵਾਪਸ 1;
        }
    }
    ਵਾਪਸ 0;
}

ਖਾਲੀ *
_PyPegen_raise_error_for_missing_comma(Parser *p, ਵਿਅੰਜਕ_ty a, ਵਿਅੰਜਕ_ty b)
{
    // Don't raise for legacy statements like "print x" or "exec x"
    ਜੇ (_PyPegen_check_legacy_stmt(p, a)) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    // Only raise inside parentheses/brackets (level > 0)
    ਜੇ (p->ਟੋਕਨ[p->mark - 1]->level == 0) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    // For multi-line expressions (like string concatenations), point to the
    // last line instead of the first for a more helpful error message.
    // Use a->col_offset as the starting column since all strings in the
    // concatenation typically share the same indentation.
    ਜੇ (a->ਸਿਰਾ_lineno > a->lineno) {
        ਵਾਪਸ RAISE_ERROR_ਗਿਆਤ_LOCATION(
            p, PyExc_SyntaxError, a->ਸਿਰਾ_lineno, a->ਖਾਨਾ_ਵਿੱਥ,
            a->ਸਿਰਾ_lineno, a->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ,
            "invalid syntax. Perhaps you forgot a comma?"
        );
    }
    ਵਾਪਸ RAISE_ERROR_ਗਿਆਤ_LOCATION(
        p, PyExc_SyntaxError, a->lineno, a->ਖਾਨਾ_ਵਿੱਥ,
        b->ਸਿਰਾ_lineno, b->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ,
        "invalid syntax. Perhaps you forgot a comma?"
    );
}

ਸਥਿਰ ਨਤੀਜਾ_Token_With_Metadata *
ਨਤੀਜਾ_token_with_metadata(Parser *p, ਖਾਲੀ *ਨਤੀਜਾ, PyObject *metadata)
{
    ਨਤੀਜਾ_Token_With_Metadata *res = _PyArena_Malloc(p->arena, ਆਕਾਰ(ਨਤੀਜਾ_Token_With_Metadata));
    ਜੇ (res == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    res->metadata = metadata;
    res->ਨਤੀਜਾ = ਨਤੀਜਾ;
    ਵਾਪਸ res;
}

ਸਥਿਰ ਅੱਖਰ
formatted_string_ਅਗੇਤਰ(ਅਚਲ Parser *p)
{
    ਪੂਰਨ nested = 0;
    ਲਈ (ਪੂਰਨ i = p->mark - 1; i >= 0; i--) {
        ਪੂਰਨ type = p->ਟੋਕਨ[i]->type;
        ਜੇ (type == FSTRING_ਸਿਰਾ || type == TSTRING_ਸਿਰਾ) {
            nested++;
        }
        ਨਹੀਂਤਾਂ ਜੇ (type == FSTRING_ਸ਼ੁਰੂ || type == TSTRING_ਸ਼ੁਰੂ) {
            ਜੇ (nested == 0) {
                ਵਾਪਸ type == TSTRING_ਸ਼ੁਰੂ ? 't' : 'f';
            }
            nested--;
        }
    }
    Py_UNREACHABLE();
}

ਨਤੀਜਾ_Token_With_Metadata *
_PyPegen_check_fstring_conversion(Parser *p, Token* conv_token, ਵਿਅੰਜਕ_ty conv)
{
    ਜੇ (conv_token->lineno != conv->lineno || conv_token->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ != conv->ਖਾਨਾ_ਵਿੱਥ) {
        ਵਾਪਸ RAISE_SYNTAX_ERROR_ਗਿਆਤ_RANGE(
            conv_token, conv,
            "%c-string: conversion type must come right after the exclamation mark",
            formatted_string_ਅਗੇਤਰ(p)
        );
    }

    Py_UCS4 ਪਹਿਲਾ = PyUnicode_READ_CHAR(conv->v.Name.id, 0);
    ਜੇ (Py_Unicode_GET_ਲੰਮਾਈ(conv->v.Name.id) > 1 ||
            !(ਪਹਿਲਾ == 's' || ਪਹਿਲਾ == 'r' || ਪਹਿਲਾ == 'a')) {
        RAISE_SYNTAX_ERROR_ਗਿਆਤ_LOCATION(conv,
                                            "%c-string: invalid conversion character %R: expected 's', 'r', or 'a'",
                                            formatted_string_ਅਗੇਤਰ(p),
                                            conv->v.Name.id);
        ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਾਪਸ ਨਤੀਜਾ_token_with_metadata(p, conv, conv_token->metadata);
}

ਨਤੀਜਾ_Token_With_Metadata *
Py_Pegen_ਤਿਆਰੀ_ਭਰਿਆ_format_spec(Parser *p, Token *colon, asdl_ਵਿਅੰਜਕ_seq *spec, ਪੂਰਨ lineno, ਪੂਰਨ ਖਾਨਾ_ਵਿੱਥ,
                                ਪੂਰਨ ਸਿਰਾ_lineno, ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, PyArena *arena)
{
    ਜੇ (!spec) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    // This is needed to keep compatibility with 3.11, where an empty format
    // spec is parsed as an *empty* JoinedStr node, instead of having an empty
    // constant in it.
    Py_ssize_t n_ਚੀਜ਼ਾਂ = asdl_seq_LEN(spec);
    Py_ssize_t non_ਖਾਲੀ_ਥਾਵਾਂ_ਗਿਣਤੀ = 0;
    ਲਈ (Py_ssize_t i = 0; i < n_ਚੀਜ਼ਾਂ; i++) {
        ਵਿਅੰਜਕ_ty ਵਸਤੂ = asdl_seq_GET(spec, i);
        non_ਖਾਲੀ_ਥਾਵਾਂ_ਗਿਣਤੀ += !(ਵਸਤੂ->ਕਿਸਮ == Constant_ਕਿਸਮ &&
                             PyUnicode_CheckExact(ਵਸਤੂ->v.Constant.ਮੁੱਲ) &&
                             Py_Unicode_GET_ਲੰਮਾਈ(ਵਸਤੂ->v.Constant.ਮੁੱਲ) == 0);
    }
    ਜੇ (non_ਖਾਲੀ_ਥਾਵਾਂ_ਗਿਣਤੀ != n_ਚੀਜ਼ਾਂ) {
        asdl_ਵਿਅੰਜਕ_seq *resized_spec =
            Py_asdl_ਵਿਅੰਜਕ_seq_new(non_ਖਾਲੀ_ਥਾਵਾਂ_ਗਿਣਤੀ, p->arena);
        ਜੇ (resized_spec == ਨਿਰਾ) {
            ਵਾਪਸ ਨਿਰਾ;
        }
        Py_ssize_t j = 0;
        ਲਈ (Py_ssize_t i = 0; i < n_ਚੀਜ਼ਾਂ; i++) {
            ਵਿਅੰਜਕ_ty ਵਸਤੂ = asdl_seq_GET(spec, i);
            ਜੇ (ਵਸਤੂ->ਕਿਸਮ == Constant_ਕਿਸਮ &&
                PyUnicode_CheckExact(ਵਸਤੂ->v.Constant.ਮੁੱਲ) &&
                Py_Unicode_GET_ਲੰਮਾਈ(ਵਸਤੂ->v.Constant.ਮੁੱਲ) == 0) {
                ਜਾਰੀ;
            }
            asdl_seq_ਸੈੱਟ(resized_spec, j++, ਵਸਤੂ);
        }
        ਪ੍ਰਤਿਜ੍ਞਾ(j == non_ਖਾਲੀ_ਥਾਵਾਂ_ਗਿਣਤੀ);
        spec = resized_spec;
    }
    ਵਿਅੰਜਕ_ty res;
    Py_ssize_t n = asdl_seq_LEN(spec);
    ਜੇ (n == 0 || (n == 1 && asdl_seq_GET(spec, 0)->ਕਿਸਮ == Constant_ਕਿਸਮ)) {
        res = _PyAST_JoinedStr(spec, lineno, ਖਾਨਾ_ਵਿੱਥ, ਸਿਰਾ_lineno,
                                    ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, p->arena);
    } ਨਹੀਂਤਾਂ {
        res = Py_Pegen_concatenate_ਲੜੀਆਂ(p, spec,
                             lineno, ਖਾਨਾ_ਵਿੱਥ, ਸਿਰਾ_lineno,
                             ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, arena);
    }
    ਜੇ (!res) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ ਨਤੀਜਾ_token_with_metadata(p, res, colon->metadata);
}

ਅਚਲ ਅੱਖਰ *
Py_Pegen_get_ਵਿਅੰਜਕ_name(ਵਿਅੰਜਕ_ty e)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(e != ਨਿਰਾ);
    ਚੋਣ (e->ਕਿਸਮ) {
        ਹਾਲਤ Attribute_ਕਿਸਮ:
            ਵਾਪਸ "attribute";
        ਹਾਲਤ Subscript_ਕਿਸਮ:
            ਵਾਪਸ "subscript";
        ਹਾਲਤ Starred_ਕਿਸਮ:
            ਵਾਪਸ "starred";
        ਹਾਲਤ Name_ਕਿਸਮ:
            ਵਾਪਸ "name";
        ਹਾਲਤ List_ਕਿਸਮ:
            ਵਾਪਸ "list";
        ਹਾਲਤ Tuple_ਕਿਸਮ:
            ਵਾਪਸ "tuple";
        ਹਾਲਤ Lambda_ਕਿਸਮ:
            ਵਾਪਸ "lambda";
        ਹਾਲਤ Call_ਕਿਸਮ:
            ਵਾਪਸ "function call";
        ਹਾਲਤ Bool_Op_ਕਿਸਮ:
        ਹਾਲਤ Bin_Op_ਕਿਸਮ:
        ਹਾਲਤ Unary_Op_ਕਿਸਮ:
            ਵਾਪਸ "expression";
        ਹਾਲਤ Generator_Exp_ਕਿਸਮ:
            ਵਾਪਸ "generator expression";
        ਹਾਲਤ Yield_ਕਿਸਮ:
        ਹਾਲਤ Yield_From_ਕਿਸਮ:
            ਵਾਪਸ "yield expression";
        ਹਾਲਤ Await_ਕਿਸਮ:
            ਵਾਪਸ "await expression";
        ਹਾਲਤ List_Comp_ਕਿਸਮ:
            ਵਾਪਸ "list comprehension";
        ਹਾਲਤ ਸੈੱਟ_Comp_ਕਿਸਮ:
            ਵਾਪਸ "set comprehension";
        ਹਾਲਤ Dict_Comp_ਕਿਸਮ:
            ਵਾਪਸ "dict comprehension";
        ਹਾਲਤ Dict_ਕਿਸਮ:
            ਵਾਪਸ "dict literal";
        ਹਾਲਤ ਸੈੱਟ_ਕਿਸਮ:
            ਵਾਪਸ "set display";
        ਹਾਲਤ Joined_Str_ਕਿਸਮ:
        ਹਾਲਤ Formatted_ਮੁੱਲ_ਕਿਸਮ:
            ਵਾਪਸ "f-string expression";
        ਹਾਲਤ Template_Str_ਕਿਸਮ:
        ਹਾਲਤ Interpolation_ਕਿਸਮ:
            ਵਾਪਸ "t-string expression";
        ਹਾਲਤ Constant_ਕਿਸਮ: {
            PyObject *ਮੁੱਲ = e->v.Constant.ਮੁੱਲ;
            ਜੇ (ਮੁੱਲ == Py_None) {
                ਵਾਪਸ "None";
            }
            ਜੇ (ਮੁੱਲ == Py_False) {
                ਵਾਪਸ "False";
            }
            ਜੇ (ਮੁੱਲ == Py_True) {
                ਵਾਪਸ "True";
            }
            ਜੇ (ਮੁੱਲ == Py_Ellipsis) {
                ਵਾਪਸ "ellipsis";
            }
            ਵਾਪਸ "literal";
        }
        ਹਾਲਤ Compare_ਕਿਸਮ:
            ਵਾਪਸ "comparison";
        ਹਾਲਤ If_Exp_ਕਿਸਮ:
            ਵਾਪਸ "conditional expression";
        ਹਾਲਤ Named_ਵਿਅੰਜਕ_ਕਿਸਮ:
            ਵਾਪਸ "named expression";
        ਮੂਲ:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->ਕਿਸਮ, e->lineno);
            ਵਾਪਸ ਨਿਰਾ;
    }
}

ਵਿਅੰਜਕ_ty
Py_Pegen_get_ਆਖਰੀ_comprehension_ਵਸਤੂ(comprehension_ty comprehension) {
    ਜੇ (comprehension->ifs == ਨਿਰਾ || asdl_seq_LEN(comprehension->ifs) == 0) {
        ਵਾਪਸ comprehension->iter;
    }
    ਵਾਪਸ Py_Pegen_ਆਖਰੀ_ਵਸਤੂ(comprehension->ifs, ਵਿਅੰਜਕ_ty);
}

ਵਿਅੰਜਕ_ty _PyPegen_collect_call_seqs(Parser *p, asdl_ਵਿਅੰਜਕ_seq *a, asdl_seq *b,
                     ਪੂਰਨ lineno, ਪੂਰਨ ਖਾਨਾ_ਵਿੱਥ, ਪੂਰਨ ਸਿਰਾ_lineno,
                     ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, PyArena *arena) {
    Py_ssize_t ਦਲੀਲਾਂ_len = asdl_seq_LEN(a);
    Py_ssize_t ਕੁੱਲ_len = ਦਲੀਲਾਂ_len;

    ਜੇ (b == ਨਿਰਾ) {
        ਵਾਪਸ _PyAST_Call(_PyPegen_dummy_name(p), a, ਨਿਰਾ, lineno, ਖਾਨਾ_ਵਿੱਥ,
                        ਸਿਰਾ_lineno, ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, arena);

    }

    asdl_ਵਿਅੰਜਕ_seq *starreds = Py_Pegen_seq_ਕੱਢੋ_starred_exprs(p, b);
    ਜੇ (!starreds && PyErr_Occurred()) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    asdl_keyword_seq *keywords = _PyPegen_seq_delete_starred_exprs(p, b);
    ਜੇ (!keywords && PyErr_Occurred()) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (starreds) {
        ਕੁੱਲ_len += asdl_seq_LEN(starreds);
    }

    asdl_ਵਿਅੰਜਕ_seq *ਦਲੀਲਾਂ = Py_asdl_ਵਿਅੰਜਕ_seq_new(ਕੁੱਲ_len, arena);
    ਜੇ (ਦਲੀਲਾਂ == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    Py_ssize_t i = 0;
    ਲਈ (i = 0; i < ਦਲੀਲਾਂ_len; i++) {
        asdl_seq_ਸੈੱਟ(ਦਲੀਲਾਂ, i, asdl_seq_GET(a, i));
    }
    ਲਈ (; i < ਕੁੱਲ_len; i++) {
        asdl_seq_ਸੈੱਟ(ਦਲੀਲਾਂ, i, asdl_seq_GET(starreds, i - ਦਲੀਲਾਂ_len));
    }

    ਵਾਪਸ _PyAST_Call(_PyPegen_dummy_name(p), ਦਲੀਲਾਂ, keywords, lineno,
                       ਖਾਨਾ_ਵਿੱਥ, ਸਿਰਾ_lineno, ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, arena);
}

// AST Error reporting helpers

ਵਿਅੰਜਕ_ty
Py_Pegen_get_invalid_ਨਿਸ਼ਾਨਾ(ਵਿਅੰਜਕ_ty e, TARGETS_TYPE targets_type)
{
    ਜੇ (e == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (targets_type == SINGLE_TARGETS ||
        targets_type == ATTRIBUTE_OR_SUBSCRIPT_TARGETS) {
        ਜੇ (targets_type == SINGLE_TARGETS && e->ਕਿਸਮ == Name_ਕਿਸਮ) {
            ਵਾਪਸ ਨਿਰਾ;
        }
        ਵਾਪਸ e->ਕਿਸਮ == Attribute_ਕਿਸਮ || e->ਕਿਸਮ == Subscript_ਕਿਸਮ ? ਨਿਰਾ : e;
    }

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਦੇਖੋ_CONTAINER(CONTAINER, TYPE) ਕਰੋ { \
        Py_ssize_t len = asdl_seq_LEN((CONTAINER)->v.TYPE.elts);\
        ਲਈ (Py_ssize_t i = 0; i < len; i++) {\
            ਵਿਅੰਜਕ_ty other = asdl_seq_GET((CONTAINER)->v.TYPE.elts, i);\
            ਵਿਅੰਜਕ_ty ਬੱਚਾ = Py_Pegen_get_invalid_ਨਿਸ਼ਾਨਾ(other, targets_type);\
            ਜੇ (ਬੱਚਾ != ਨਿਰਾ) {\
                ਵਾਪਸ ਬੱਚਾ;\
            }\
        }\
    } ਜਦੋਂ (0)

    // We only need to visit List and Tuple nodes recursively as those
    // are the only ones that can contain valid names in targets when
    // they are parsed as expressions. Any other kind of expression
    // that is a container (like Sets or Dicts) is directly invalid and
    // we don't need to visit it recursively.

    ਚੋਣ (e->ਕਿਸਮ) {
        ਹਾਲਤ List_ਕਿਸਮ:
            ਦੇਖੋ_CONTAINER(e, List);
            ਵਾਪਸ ਨਿਰਾ;
        ਹਾਲਤ Tuple_ਕਿਸਮ:
            ਦੇਖੋ_CONTAINER(e, Tuple);
            ਵਾਪਸ ਨਿਰਾ;
        ਹਾਲਤ Starred_ਕਿਸਮ:
            ਜੇ (targets_type == DEL_TARGETS) {
                ਵਾਪਸ e;
            }
            ਵਾਪਸ Py_Pegen_get_invalid_ਨਿਸ਼ਾਨਾ(e->v.Starred.ਮੁੱਲ, targets_type);
        ਹਾਲਤ Compare_ਕਿਸਮ:
            // This is needed, because the `a in b` in `for a in b` gets parsed
            // as a comparison, and so we need to search the left side of the comparison
            // for invalid targets.
            ਜੇ (targets_type == FOR_TARGETS) {
                cmpop_ty cmpop = (cmpop_ty) asdl_seq_GET(e->v.Compare.ops, 0);
                ਜੇ (cmpop == ਵਿੱਚ) {
                    ਵਾਪਸ Py_Pegen_get_invalid_ਨਿਸ਼ਾਨਾ(e->v.Compare.ਖੱਬਾ, targets_type);
                }
                ਵਾਪਸ ਨਿਰਾ;
            }
            ਵਾਪਸ e;
        ਹਾਲਤ Name_ਕਿਸਮ:
        ਹਾਲਤ Subscript_ਕਿਸਮ:
        ਹਾਲਤ Attribute_ਕਿਸਮ:
            ਵਾਪਸ ਨਿਰਾ;
        ਮੂਲ:
            ਵਾਪਸ e;
    }
}

ਖਾਲੀ *_PyPegen_arguments_parsing_error(Parser *p, ਵਿਅੰਜਕ_ty e) {
    ਪੂਰਨ kwarg_unpacking = 0;
    ਲਈ (Py_ssize_t i = 0, l = asdl_seq_LEN(e->v.Call.keywords); i < l; i++) {
        keyword_ty keyword = asdl_seq_GET(e->v.Call.keywords, i);
        ਜੇ (!keyword->arg) {
            kwarg_unpacking = 1;
        }
    }

    ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ = ਨਿਰਾ;
    ਜੇ (kwarg_unpacking) {
        ਸੁਨੇਹਾ = "positional argument follows keyword argument unpacking";
    } ਨਹੀਂਤਾਂ {
        ਸੁਨੇਹਾ = "positional argument follows keyword argument";
    }

    ਵਾਪਸ RAISE_SYNTAX_ERROR(ਸੁਨੇਹਾ);
}

ਖਾਲੀ *
Py_Pegen_nonparen_genexp_ਵਿੱਚ_call(Parser *p, ਵਿਅੰਜਕ_ty ਦਲੀਲਾਂ, asdl_comprehension_seq *comprehensions)
{
    /* The rule that calls this function is 'args for_if_clauses'.
       For the input f(L, x for x in y), L and x are in args and
       the for is parsed as a for_if_clause. We have to check if
       len <= 1, so that input like dict((a, b) for a, b in x)
       gets successfully parsed and then we pass the last
       argument (x in the above example) as the location of the
       error */
    Py_ssize_t len = asdl_seq_LEN(ਦਲੀਲਾਂ->v.Call.ਦਲੀਲਾਂ);
    ਜੇ (len <= 1) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    comprehension_ty ਆਖਰੀ_comprehension = Py_Pegen_ਆਖਰੀ_ਵਸਤੂ(comprehensions, comprehension_ty);

    ਵਾਪਸ RAISE_SYNTAX_ERROR_ਗਿਆਤ_RANGE(
        (ਵਿਅੰਜਕ_ty) asdl_seq_GET(ਦਲੀਲਾਂ->v.Call.ਦਲੀਲਾਂ, len - 1),
        Py_Pegen_get_ਆਖਰੀ_comprehension_ਵਸਤੂ(ਆਖਰੀ_comprehension),
        "Generator expression must be parenthesized"
    );
}

// Fstring stuff

ਸਥਿਰ ਵਿਅੰਜਕ_ty
_PyPegen_decode_fstring_part(Parser* p, ਪੂਰਨ is_raw, ਵਿਅੰਜਕ_ty constant, Token* token) {
    ਪ੍ਰਤਿਜ੍ਞਾ(PyUnicode_CheckExact(constant->v.Constant.ਮੁੱਲ));

    ਅਚਲ ਅੱਖਰ* bstr = PyUnicode_AsUTF8(constant->v.Constant.ਮੁੱਲ);
    ਜੇ (bstr == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    ਮਾਪ_t len;
    ਜੇ (strcmp(bstr, "{{") == 0 || strcmp(bstr, "}}") == 0) {
        len = 1;
    } ਨਹੀਂਤਾਂ {
        len = ਲੜੀ_ਲੰਬਾਈ(bstr);
    }

    is_raw = is_raw || strchr(bstr, '\\') == ਨਿਰਾ;
    PyObject *str = _PyPegen_decode_string(p, is_raw, bstr, len, token);
    ਜੇ (str == ਨਿਰਾ) {
        _Pypegen_raise_decode_error(p);
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (Py_Arena_ਜੋੜੋ_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ _PyAST_Constant(str, ਨਿਰਾ, constant->lineno, constant->ਖਾਨਾ_ਵਿੱਥ,
                           constant->ਸਿਰਾ_lineno, constant->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ,
                           p->arena);
}

ਸਥਿਰ asdl_ਵਿਅੰਜਕ_seq *
_get_resized_exprs(Parser *p, Token *a, asdl_ਵਿਅੰਜਕ_seq *raw_expressions,
                   Token *b, ਪੂਰਨ is_tstring)
{
    Py_ssize_t n_ਚੀਜ਼ਾਂ = asdl_seq_LEN(raw_expressions);
    Py_ssize_t ਕੁੱਲ_ਚੀਜ਼ਾਂ = n_ਚੀਜ਼ਾਂ;
    ਲਈ (Py_ssize_t i = 0; i < n_ਚੀਜ਼ਾਂ; i++) {
        ਵਿਅੰਜਕ_ty ਵਸਤੂ = asdl_seq_GET(raw_expressions, i);
        ਜੇ (ਵਸਤੂ->ਕਿਸਮ == Joined_Str_ਕਿਸਮ) {
            ਕੁੱਲ_ਚੀਜ਼ਾਂ += asdl_seq_LEN(ਵਸਤੂ->v.JoinedStr.ਮੁੱਲਾਂ) - 1;
        }
    }

    ਅਚਲ ਅੱਖਰ* quote_str = Py_ਬਾਈਟਾਂ_As_String(a->ਬਾਈਟਾਂ);
    ਜੇ (quote_str == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਪੂਰਨ is_raw = strpbrk(quote_str, "rR") != ਨਿਰਾ;

    asdl_ਵਿਅੰਜਕ_seq *seq = Py_asdl_ਵਿਅੰਜਕ_seq_new(ਕੁੱਲ_ਚੀਜ਼ਾਂ, p->arena);
    ਜੇ (seq == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    Py_ssize_t ਸੂਚਕਾਂਕ = 0;
    ਲਈ (Py_ssize_t i = 0; i < n_ਚੀਜ਼ਾਂ; i++) {
        ਵਿਅੰਜਕ_ty ਵਸਤੂ = asdl_seq_GET(raw_expressions, i);

        /* Debug expressions arrive as JoinedStr(text, value); flatten them
           into the surrounding string. */
        ਜੇ (ਵਸਤੂ->ਕਿਸਮ == Joined_Str_ਕਿਸਮ) {
            asdl_ਵਿਅੰਜਕ_seq *ਮੁੱਲਾਂ = ਵਸਤੂ->v.JoinedStr.ਮੁੱਲਾਂ;
            ਜੇ (asdl_seq_LEN(ਮੁੱਲਾਂ) != 2) {
                PyErr_Format(PyExc_SystemError,
                             is_tstring
                             ? "unexpected TemplateStr node without debug data in t-string at line %d"
                             : "unexpected JoinedStr node without debug data in f-string at line %d",
                             ਵਸਤੂ->lineno);
                ਵਾਪਸ ਨਿਰਾ;
            }

            ਵਿਅੰਜਕ_ty ਪਹਿਲਾ = asdl_seq_GET(ਮੁੱਲਾਂ, 0);
            ਪ੍ਰਤਿਜ੍ਞਾ(ਪਹਿਲਾ->ਕਿਸਮ == Constant_ਕਿਸਮ);
            asdl_seq_ਸੈੱਟ(seq, ਸੂਚਕਾਂਕ++, ਪਹਿਲਾ);

            ਵਿਅੰਜਕ_ty ਦੂਜਾ = asdl_seq_GET(ਮੁੱਲਾਂ, 1);
            ਪ੍ਰਤਿਜ੍ਞਾ((is_tstring &&
                    ਦੂਜਾ->ਕਿਸਮ == Interpolation_ਕਿਸਮ) ||
                   ਦੂਜਾ->ਕਿਸਮ == Formatted_ਮੁੱਲ_ਕਿਸਮ);
            asdl_seq_ਸੈੱਟ(seq, ਸੂਚਕਾਂਕ++, ਦੂਜਾ);

            ਜਾਰੀ;
        }

        ਜੇ (ਵਸਤੂ->ਕਿਸਮ == Constant_ਕਿਸਮ) {
            ਵਸਤੂ = _PyPegen_decode_fstring_part(p, is_raw, ਵਸਤੂ, b);
            ਜੇ (ਵਸਤੂ == ਨਿਰਾ) {
                ਵਾਪਸ ਨਿਰਾ;
            }

            /* Tokenizer emits string parts even when the underlying string
            might become an empty value (e.g. FSTRING_MIDDLE with the value \\n)
            so we need to check for them and simplify it here. */
            ਜੇ (PyUnicode_CheckExact(ਵਸਤੂ->v.Constant.ਮੁੱਲ)
                && Py_Unicode_GET_ਲੰਮਾਈ(ਵਸਤੂ->v.Constant.ਮੁੱਲ) == 0) {
                ਜਾਰੀ;
            }
        }
        asdl_seq_ਸੈੱਟ(seq, ਸੂਚਕਾਂਕ++, ਵਸਤੂ);
    }

    asdl_ਵਿਅੰਜਕ_seq *resized_exprs;
    ਜੇ (ਸੂਚਕਾਂਕ != ਕੁੱਲ_ਚੀਜ਼ਾਂ) {
        resized_exprs = Py_asdl_ਵਿਅੰਜਕ_seq_new(ਸੂਚਕਾਂਕ, p->arena);
        ਜੇ (resized_exprs == ਨਿਰਾ) {
            ਵਾਪਸ ਨਿਰਾ;
        }
        ਲਈ (Py_ssize_t i = 0; i < ਸੂਚਕਾਂਕ; i++) {
            asdl_seq_ਸੈੱਟ(resized_exprs, i, asdl_seq_GET(seq, i));
        }
    }
    ਨਹੀਂਤਾਂ {
        resized_exprs = seq;
    }
    ਵਾਪਸ resized_exprs;
}

ਵਿਅੰਜਕ_ty
_PyPegen_template_str(Parser *p, Token *a, asdl_ਵਿਅੰਜਕ_seq *raw_expressions, Token *b) {

    asdl_ਵਿਅੰਜਕ_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 1);
    ਜੇ (resized_exprs == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ _PyAST_TemplateStr(resized_exprs, a->lineno, a->ਖਾਨਾ_ਵਿੱਥ,
                              b->ਸਿਰਾ_lineno, b->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ,
                              p->arena);
}

ਵਿਅੰਜਕ_ty
_PyPegen_joined_str(Parser *p, Token* a, asdl_ਵਿਅੰਜਕ_seq* raw_expressions, Token*b) {

    asdl_ਵਿਅੰਜਕ_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 0);
    ਜੇ (resized_exprs == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ _PyAST_JoinedStr(resized_exprs, a->lineno, a->ਖਾਨਾ_ਵਿੱਥ,
                            b->ਸਿਰਾ_lineno, b->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ,
                            p->arena);
}

ਵਿਅੰਜਕ_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok) {
    ਜੇ (tok->ਬਾਈਟਾਂ == ਨਿਰਾ) {
        ਵਾਪਸ Py_Pegen_raise_incomplete_ਨਿਵੇਸ਼(p);
    }
    Py_ssize_t bsize = Py_ਬਾਈਟਾਂ_GET_ਮਾਪ(tok->ਬਾਈਟਾਂ);
    ਅਚਲ ਅੱਖਰ *bstr = Py_ਬਾਈਟਾਂ_AS_STRING(tok->ਬਾਈਟਾਂ);

    ਪੂਰਨ is_raw = tok->is_raw;

    PyObject* str = _PyPegen_decode_string(p, is_raw, bstr, bsize, tok);
    ਜੇ (str == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (Py_Arena_ਜੋੜੋ_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ _PyAST_Constant(str, ਨਿਰਾ, tok->lineno, tok->ਖਾਨਾ_ਵਿੱਥ,
                           tok->ਸਿਰਾ_lineno, tok->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ,
                           p->arena);
}

ਵਿਅੰਜਕ_ty _PyPegen_constant_from_token(Parser* p, Token* tok) {
    ਜੇ (tok->ਬਾਈਟਾਂ == ਨਿਰਾ) {
        ਵਾਪਸ Py_Pegen_raise_incomplete_ਨਿਵੇਸ਼(p);
    }
    ਅਚਲ ਅੱਖਰ *bstr = Py_ਬਾਈਟਾਂ_AS_STRING(tok->ਬਾਈਟਾਂ);
    PyObject* str = PyUnicode_FromString(bstr);
    ਜੇ (str == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (Py_Arena_ਜੋੜੋ_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ _PyAST_Constant(str, ਨਿਰਾ, tok->lineno, tok->ਖਾਨਾ_ਵਿੱਥ,
                           tok->ਸਿਰਾ_lineno, tok->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ,
                           p->arena);
}

ਵਿਅੰਜਕ_ty _PyPegen_constant_from_string(Parser* p, Token* tok) {
    ਜੇ (tok->ਬਾਈਟਾਂ == ਨਿਰਾ) {
        ਵਾਪਸ Py_Pegen_raise_incomplete_ਨਿਵੇਸ਼(p);
    }
    ਅਚਲ ਅੱਖਰ *the_str = Py_ਬਾਈਟਾਂ_AS_STRING(tok->ਬਾਈਟਾਂ);
    PyObject *s = _PyPegen_parse_string(p, tok);
    ਜੇ (s == ਨਿਰਾ) {
        _Pypegen_raise_decode_error(p);
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (Py_Arena_ਜੋੜੋ_Py_Object(p->arena, s) < 0) {
        Py_DECREF(s);
        ਵਾਪਸ ਨਿਰਾ;
    }
    PyObject *ਕਿਸਮ = ਨਿਰਾ;
    ਜੇ (the_str && the_str[0] == 'u') {
        ਕਿਸਮ = _PyPegen_new_identifier(p, "u");
        ਜੇ (ਕਿਸਮ == ਨਿਰਾ) {
            ਵਾਪਸ ਨਿਰਾ;
        }
    }
    ਵਾਪਸ _PyAST_Constant(s, ਕਿਸਮ, tok->lineno, tok->ਖਾਨਾ_ਵਿੱਥ, tok->ਸਿਰਾ_lineno, tok->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, p->arena);
}

ਸਥਿਰ ਪੂਰਨ
_get_interpolation_conversion(Parser *p, Token *debug, ਨਤੀਜਾ_Token_With_Metadata *conversion,
                              ਨਤੀਜਾ_Token_With_Metadata *format)
{
    ਜੇ (conversion != ਨਿਰਾ) {
        ਵਿਅੰਜਕ_ty conversion_ਵਿਅੰਜਕ = (ਵਿਅੰਜਕ_ty) conversion->ਨਤੀਜਾ;
        ਪ੍ਰਤਿਜ੍ਞਾ(conversion_ਵਿਅੰਜਕ->ਕਿਸਮ == Name_ਕਿਸਮ);
        Py_UCS4 ਪਹਿਲਾ = PyUnicode_READ_CHAR(conversion_ਵਿਅੰਜਕ->v.Name.id, 0);
        ਵਾਪਸ Py_ਸੁਰੱਖਿਅਤ_DOWNCAST(ਪਹਿਲਾ, Py_UCS4, ਪੂਰਨ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (debug && !format) {
        /* If no conversion is specified, use !r for debug expressions */
        ਵਾਪਸ (ਪੂਰਨ)'r';
    }
    ਵਾਪਸ -1;
}

ਸਥਿਰ PyObject *
strip_interpolation_debug_ਵਿਅੰਜਕ(PyObject *exprstr)
{
    Py_ssize_t len = Py_Unicode_GET_ਲੰਮਾਈ(exprstr);

    /* Discard whitespace and explicit line continuations after the debug "="
       but preserve whitespace before it. */
    ਜਦੋਂ (len > 0) {
        ਪੂਰਨ has_newline = 0;
        ਜਦੋਂ (len > 0) {
            Py_UCS4 c = PyUnicode_READ_CHAR(exprstr, len - 1);
            ਜੇ (!_PyUnicode_IsWhitespace(c)) {
                ਰੋਕੋ;
            }
            ਜੇ (c == '\r' || c == '\n') {
                has_newline = 1;
            }
            len--;
        }
        ਜੇ (!has_newline || len == 0 ||
            PyUnicode_READ_CHAR(exprstr, len - 1) != '\\')
        {
            ਰੋਕੋ;
        }
        len--;
    }

    /* Preserve unexpected metadata instead of dropping source text. */
    ਜੇ (len == 0 || PyUnicode_READ_CHAR(exprstr, len - 1) != '=') {
        ਵਾਪਸ Py_NewRef(exprstr);
    }

    ਵਾਪਸ PyUnicode_Substring(exprstr, 0, len - 1);
}

ਵਿਅੰਜਕ_ty _PyPegen_interpolation(Parser *p, ਵਿਅੰਜਕ_ty ਵਿਅੰਜਕ_2, Token *debug, ਨਤੀਜਾ_Token_With_Metadata *conversion,
                                 ਨਤੀਜਾ_Token_With_Metadata *format, Token *closing_brace, ਪੂਰਨ lineno, ਪੂਰਨ ਖਾਨਾ_ਵਿੱਥ,
                                 ਪੂਰਨ ਸਿਰਾ_lineno, ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, PyArena *arena) {

    ਪੂਰਨ conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    /* Find the non whitespace token after the "=" */
    ਪੂਰਨ debug_ਸਿਰਾ_ਸਤਰ, debug_ਸਿਰਾ_ਵਿੱਥ;
    PyObject *debug_metadata;
    constant exprstr;

    ਜੇ (conversion) {
        debug_ਸਿਰਾ_ਸਤਰ = ((ਵਿਅੰਜਕ_ty) conversion->ਨਤੀਜਾ)->lineno;
        debug_ਸਿਰਾ_ਵਿੱਥ = ((ਵਿਅੰਜਕ_ty) conversion->ਨਤੀਜਾ)->ਖਾਨਾ_ਵਿੱਥ;
        debug_metadata = exprstr = conversion->metadata;
    }
    ਨਹੀਂਤਾਂ ਜੇ (format) {
        debug_ਸਿਰਾ_ਸਤਰ = ((ਵਿਅੰਜਕ_ty) format->ਨਤੀਜਾ)->lineno;
        debug_ਸਿਰਾ_ਵਿੱਥ = ((ਵਿਅੰਜਕ_ty) format->ਨਤੀਜਾ)->ਖਾਨਾ_ਵਿੱਥ + 1;
        debug_metadata = exprstr = format->metadata;
    }
    ਨਹੀਂਤਾਂ {
        debug_ਸਿਰਾ_ਸਤਰ = ਸਿਰਾ_lineno;
        debug_ਸਿਰਾ_ਵਿੱਥ = ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ;
        debug_metadata = exprstr = closing_brace->metadata;
    }

    ਪ੍ਰਤਿਜ੍ਞਾ(exprstr != ਨਿਰਾ);
    PyObject *final_exprstr = debug
        ? strip_interpolation_debug_ਵਿਅੰਜਕ(exprstr)
        : Py_NewRef(exprstr);
    ਜੇ (!final_exprstr || Py_Arena_ਜੋੜੋ_Py_Object(arena, final_exprstr) < 0) {
        Py_XDECREF(final_exprstr);
        ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਿਅੰਜਕ_ty interpolation = _PyAST_Interpolation(
        ਵਿਅੰਜਕ_2, final_exprstr, conversion_val, format ? (ਵਿਅੰਜਕ_ty) format->ਨਤੀਜਾ : ਨਿਰਾ,
        lineno, ਖਾਨਾ_ਵਿੱਥ, ਸਿਰਾ_lineno,
        ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, arena
    );

    ਜੇ (!interpolation || !debug) {
        ਵਾਪਸ interpolation;
    }

    ਵਿਅੰਜਕ_ty debug_ਪਾਠ = _PyAST_Constant(debug_metadata, ਨਿਰਾ, lineno, ਖਾਨਾ_ਵਿੱਥ + 1, debug_ਸਿਰਾ_ਸਤਰ,
                                            debug_ਸਿਰਾ_ਵਿੱਥ - 1, p->arena);
    ਜੇ (!debug_ਪਾਠ) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    asdl_ਵਿਅੰਜਕ_seq *ਮੁੱਲਾਂ = Py_asdl_ਵਿਅੰਜਕ_seq_new(2, arena);
    ਜੇ (!ਮੁੱਲਾਂ) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    asdl_seq_ਸੈੱਟ(ਮੁੱਲਾਂ, 0, debug_ਪਾਠ);
    asdl_seq_ਸੈੱਟ(ਮੁੱਲਾਂ, 1, interpolation);
    ਵਾਪਸ _PyAST_JoinedStr(ਮੁੱਲਾਂ, lineno, ਖਾਨਾ_ਵਿੱਥ, debug_ਸਿਰਾ_ਸਤਰ, debug_ਸਿਰਾ_ਵਿੱਥ, p->arena);
}

ਵਿਅੰਜਕ_ty Py_Pegen_formatted_ਮੁੱਲ(Parser *p, ਵਿਅੰਜਕ_ty ਵਿਅੰਜਕ_2, Token *debug, ਨਤੀਜਾ_Token_With_Metadata *conversion,
                                 ਨਤੀਜਾ_Token_With_Metadata *format, Token *closing_brace, ਪੂਰਨ lineno, ਪੂਰਨ ਖਾਨਾ_ਵਿੱਥ,
                                 ਪੂਰਨ ਸਿਰਾ_lineno, ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, PyArena *arena) {
    ਪੂਰਨ conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    ਵਿਅੰਜਕ_ty formatted_ਮੁੱਲ = Py_AST_Formatted_ਮੁੱਲ(
        ਵਿਅੰਜਕ_2, conversion_val, format ? (ਵਿਅੰਜਕ_ty) format->ਨਤੀਜਾ : ਨਿਰਾ,
        lineno, ਖਾਨਾ_ਵਿੱਥ, ਸਿਰਾ_lineno,
        ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, arena
    );

    ਜੇ (!formatted_ਮੁੱਲ || !debug) {
        ਵਾਪਸ formatted_ਮੁੱਲ;
    }

    /* Find the non whitespace token after the "=" */
    ਪੂਰਨ debug_ਸਿਰਾ_ਸਤਰ, debug_ਸਿਰਾ_ਵਿੱਥ;
    PyObject *debug_metadata;

    ਜੇ (conversion) {
        debug_ਸਿਰਾ_ਸਤਰ = ((ਵਿਅੰਜਕ_ty) conversion->ਨਤੀਜਾ)->lineno;
        debug_ਸਿਰਾ_ਵਿੱਥ = ((ਵਿਅੰਜਕ_ty) conversion->ਨਤੀਜਾ)->ਖਾਨਾ_ਵਿੱਥ;
        debug_metadata = conversion->metadata;
    }
    ਨਹੀਂਤਾਂ ਜੇ (format) {
        debug_ਸਿਰਾ_ਸਤਰ = ((ਵਿਅੰਜਕ_ty) format->ਨਤੀਜਾ)->lineno;
        debug_ਸਿਰਾ_ਵਿੱਥ = ((ਵਿਅੰਜਕ_ty) format->ਨਤੀਜਾ)->ਖਾਨਾ_ਵਿੱਥ + 1;
        debug_metadata = format->metadata;
    }
    ਨਹੀਂਤਾਂ {
        debug_ਸਿਰਾ_ਸਤਰ = ਸਿਰਾ_lineno;
        debug_ਸਿਰਾ_ਵਿੱਥ = ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ;
        debug_metadata = closing_brace->metadata;
    }
    ਵਿਅੰਜਕ_ty debug_ਪਾਠ = _PyAST_Constant(debug_metadata, ਨਿਰਾ, lineno, ਖਾਨਾ_ਵਿੱਥ + 1, debug_ਸਿਰਾ_ਸਤਰ,
                                            debug_ਸਿਰਾ_ਵਿੱਥ - 1, p->arena);
    ਜੇ (!debug_ਪਾਠ) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    asdl_ਵਿਅੰਜਕ_seq *ਮੁੱਲਾਂ = Py_asdl_ਵਿਅੰਜਕ_seq_new(2, arena);
    ਜੇ (!ਮੁੱਲਾਂ) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    asdl_seq_ਸੈੱਟ(ਮੁੱਲਾਂ, 0, debug_ਪਾਠ);
    asdl_seq_ਸੈੱਟ(ਮੁੱਲਾਂ, 1, formatted_ਮੁੱਲ);
    ਵਾਪਸ _PyAST_JoinedStr(ਮੁੱਲਾਂ, lineno, ਖਾਨਾ_ਵਿੱਥ, debug_ਸਿਰਾ_ਸਤਰ, debug_ਸਿਰਾ_ਵਿੱਥ, p->arena);
}

ਸਥਿਰ ਵਿਅੰਜਕ_ty
build_concatenated_ਬਾਈਟਾਂ(Parser *p, asdl_ਵਿਅੰਜਕ_seq *ਲੜੀਆਂ, ਪੂਰਨ lineno,
                        ਪੂਰਨ ਖਾਨਾ_ਵਿੱਥ, ਪੂਰਨ ਸਿਰਾ_lineno, ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(ਲੜੀਆਂ);
    ਪ੍ਰਤਿਜ੍ਞਾ(len > 0);

    /* Bytes literals never get a kind, but just for consistency
        since they are represented as Constant nodes, we'll mirror
        the same behavior as unicode strings for determining the
        kind. */
    PyObject *ਕਿਸਮ = asdl_seq_GET(ਲੜੀਆਂ, 0)->v.Constant.ਕਿਸਮ;

    Py_ssize_t ਕੁੱਲ = 0;
    ਲਈ (Py_ssize_t i = 0; i < len; i++) {
        ਵਿਅੰਜਕ_ty elem = asdl_seq_GET(ਲੜੀਆਂ, i);
        PyObject *ਬਾਈਟਾਂ = elem->v.Constant.ਮੁੱਲ;
        Py_ssize_t part = Py_ਬਾਈਟਾਂ_GET_ਮਾਪ(ਬਾਈਟਾਂ);
        ਜੇ (part > PY_SSIZE_T_MAX - ਕੁੱਲ) {
            Py_Err_No_ਸਮ੍ਰਿਤੀ();
            ਵਾਪਸ ਨਿਰਾ;
        }
        ਕੁੱਲ += part;
    }

    Py_ਬਾਈਟਾਂ_Writer *writer = Py_ਬਾਈਟਾਂ_Writer_Create(ਕੁੱਲ);
    ਜੇ (writer == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਅੱਖਰ *ਬਾਹਰਲਾ = Py_ਬਾਈਟਾਂ_Writer_Get_ਅੰਕੜੇ(writer);

    ਲਈ (Py_ssize_t i = 0; i < len; i++) {
        ਵਿਅੰਜਕ_ty elem = asdl_seq_GET(ਲੜੀਆਂ, i);
        PyObject *ਬਾਈਟਾਂ = elem->v.Constant.ਮੁੱਲ;
        Py_ssize_t part = Py_ਬਾਈਟਾਂ_GET_ਮਾਪ(ਬਾਈਟਾਂ);
        ਜੇ (part > 0) {
            ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਬਾਹਰਲਾ, Py_ਬਾਈਟਾਂ_AS_STRING(ਬਾਈਟਾਂ), part);
            ਬਾਹਰਲਾ += part;
        }
    }

    PyObject *res = Py_ਬਾਈਟਾਂ_Writer_ਅੰਤ(writer);
    ਜੇ (res == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (Py_Arena_ਜੋੜੋ_Py_Object(arena, res) < 0) {
        Py_DECREF(res);
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ _PyAST_Constant(res, ਕਿਸਮ, lineno, ਖਾਨਾ_ਵਿੱਥ, ਸਿਰਾ_lineno, ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, p->arena);
}

ਸਥਿਰ ਵਿਅੰਜਕ_ty
_build_concatenated_unicode(Parser *p, asdl_ਵਿਅੰਜਕ_seq *ਲੜੀਆਂ, ਪੂਰਨ lineno,
                        ਪੂਰਨ ਖਾਨਾ_ਵਿੱਥ, ਪੂਰਨ ਸਿਰਾ_lineno, ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(ਲੜੀਆਂ);
    ਪ੍ਰਤਿਜ੍ਞਾ(len > 1);

    ਵਿਅੰਜਕ_ty ਪਹਿਲਾ = asdl_seq_GET(ਲੜੀਆਂ, 0);

    /* When a string is getting concatenated, the kind of the string
        is determined by the first string in the concatenation
        sequence.

        u"abc" "def" -> u"abcdef"
        "abc" u"abc" ->  "abcabc" */
    PyObject *ਕਿਸਮ = ਪਹਿਲਾ->v.Constant.ਕਿਸਮ;

    PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
    ਜੇ (writer == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    ਲਈ (Py_ssize_t i = 0; i < len; i++) {
        ਵਿਅੰਜਕ_ty current_elem = asdl_seq_GET(ਲੜੀਆਂ, i);
        ਪ੍ਰਤਿਜ੍ਞਾ(current_elem->ਕਿਸਮ == Constant_ਕਿਸਮ);

        ਜੇ (PyUnicodeWriter_WriteStr(writer,
                                     current_elem->v.Constant.ਮੁੱਲ)) {
            PyUnicodeWriter_Discard(writer);
            ਵਾਪਸ ਨਿਰਾ;
        }
    }

    PyObject *final = Py_Unicode_Writer_ਅੰਤ(writer);
    ਜੇ (final == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (Py_Arena_ਜੋੜੋ_Py_Object(p->arena, final) < 0) {
        Py_DECREF(final);
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ _PyAST_Constant(final, ਕਿਸਮ, lineno, ਖਾਨਾ_ਵਿੱਥ,
                           ਸਿਰਾ_lineno, ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, arena);
}

ਸਥਿਰ asdl_ਵਿਅੰਜਕ_seq *
_build_concatenated_str(Parser *p, asdl_ਵਿਅੰਜਕ_seq *ਲੜੀਆਂ,
                               ਪੂਰਨ lineno, ਪੂਰਨ ਖਾਨਾ_ਵਿੱਥ, ਪੂਰਨ ਸਿਰਾ_lineno,
                               ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(ਲੜੀਆਂ);
    ਪ੍ਰਤਿਜ੍ਞਾ(len > 0);

    Py_ssize_t n_flattened_elements = 0;
    ਲਈ (Py_ssize_t i = 0; i < len; i++) {
        ਵਿਅੰਜਕ_ty elem = asdl_seq_GET(ਲੜੀਆਂ, i);
        ਚੋਣ(elem->ਕਿਸਮ) {
            ਹਾਲਤ Joined_Str_ਕਿਸਮ:
                n_flattened_elements += asdl_seq_LEN(elem->v.JoinedStr.ਮੁੱਲਾਂ);
                ਰੋਕੋ;
            ਹਾਲਤ Template_Str_ਕਿਸਮ:
                n_flattened_elements += asdl_seq_LEN(elem->v.TemplateStr.ਮੁੱਲਾਂ);
                ਰੋਕੋ;
            ਮੂਲ:
                n_flattened_elements++;
                ਰੋਕੋ;
        }
    }


    asdl_ਵਿਅੰਜਕ_seq* flattened = Py_asdl_ਵਿਅੰਜਕ_seq_new(n_flattened_elements, p->arena);
    ਜੇ (flattened == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    /* build flattened list */
    Py_ssize_t current_pos = 0;
    ਲਈ (Py_ssize_t i = 0; i < len; i++) {
        ਵਿਅੰਜਕ_ty elem = asdl_seq_GET(ਲੜੀਆਂ, i);
        ਚੋਣ(elem->ਕਿਸਮ) {
            ਹਾਲਤ Joined_Str_ਕਿਸਮ:
                ਲਈ (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.JoinedStr.ਮੁੱਲਾਂ); j++) {
                    ਵਿਅੰਜਕ_ty subvalue = asdl_seq_GET(elem->v.JoinedStr.ਮੁੱਲਾਂ, j);
                    ਜੇ (subvalue == ਨਿਰਾ) {
                        ਵਾਪਸ ਨਿਰਾ;
                    }
                    asdl_seq_ਸੈੱਟ(flattened, current_pos++, subvalue);
                }
                ਰੋਕੋ;
            ਹਾਲਤ Template_Str_ਕਿਸਮ:
                ਲਈ (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.TemplateStr.ਮੁੱਲਾਂ); j++) {
                    ਵਿਅੰਜਕ_ty subvalue = asdl_seq_GET(elem->v.TemplateStr.ਮੁੱਲਾਂ, j);
                    ਜੇ (subvalue == ਨਿਰਾ) {
                        ਵਾਪਸ ਨਿਰਾ;
                    }
                    asdl_seq_ਸੈੱਟ(flattened, current_pos++, subvalue);
                }
                ਰੋਕੋ;
            ਮੂਲ:
                asdl_seq_ਸੈੱਟ(flattened, current_pos++, elem);
                ਰੋਕੋ;
        }
    }

    /* calculate folded element count */
    Py_ssize_t n_elements = 0;
    ਪੂਰਨ prev_is_constant = 0;
    ਲਈ (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        ਵਿਅੰਜਕ_ty elem = asdl_seq_GET(flattened, i);

        /* The concatenation of a FormattedValue and an empty Constant should
           lead to the FormattedValue itself. Thus, we will not take any empty
           constants into account, just as in `_PyPegen_joined_str` */
        ਜੇ (elem->ਕਿਸਮ == Constant_ਕਿਸਮ &&
            PyUnicode_CheckExact(elem->v.Constant.ਮੁੱਲ) &&
            Py_Unicode_GET_ਲੰਮਾਈ(elem->v.Constant.ਮੁੱਲ) == 0)
            ਜਾਰੀ;

        ਜੇ (!prev_is_constant || elem->ਕਿਸਮ != Constant_ਕਿਸਮ) {
            n_elements++;
        }
        prev_is_constant = elem->ਕਿਸਮ == Constant_ਕਿਸਮ;
    }

    asdl_ਵਿਅੰਜਕ_seq* ਮੁੱਲਾਂ = Py_asdl_ਵਿਅੰਜਕ_seq_new(n_elements, p->arena);
    ਜੇ (ਮੁੱਲਾਂ == ਨਿਰਾ) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    /* build folded list */
    current_pos = 0;
    ਲਈ (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        ਵਿਅੰਜਕ_ty elem = asdl_seq_GET(flattened, i);

        /* if the current elem and the following are constants,
           fold them and all consequent constants */
        ਜੇ (elem->ਕਿਸਮ == Constant_ਕਿਸਮ) {
            ਜੇ (i + 1 < n_flattened_elements &&
                asdl_seq_GET(flattened, i + 1)->ਕਿਸਮ == Constant_ਕਿਸਮ) {
                ਵਿਅੰਜਕ_ty ਪਹਿਲਾ_elem = elem;

                /* When a string is getting concatenated, the kind of the string
                   is determined by the first string in the concatenation
                   sequence.

                   u"abc" "def" -> u"abcdef"
                   "abc" u"abc" ->  "abcabc" */
                PyObject *ਕਿਸਮ = elem->v.Constant.ਕਿਸਮ;

                PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
                ਜੇ (writer == ਨਿਰਾ) {
                    ਵਾਪਸ ਨਿਰਾ;
                }
                ਵਿਅੰਜਕ_ty ਆਖਰੀ_elem = elem;
                Py_ssize_t j;
                ਲਈ (j = i; j < n_flattened_elements; j++) {
                    ਵਿਅੰਜਕ_ty current_elem = asdl_seq_GET(flattened, j);
                    ਜੇ (current_elem->ਕਿਸਮ == Constant_ਕਿਸਮ) {
                        ਜੇ (PyUnicodeWriter_WriteStr(writer,
                                                     current_elem->v.Constant.ਮੁੱਲ)) {
                            PyUnicodeWriter_Discard(writer);
                            ਵਾਪਸ ਨਿਰਾ;
                        }
                        ਆਖਰੀ_elem = current_elem;
                    } ਨਹੀਂਤਾਂ {
                        ਰੋਕੋ;
                    }
                }
                i = j - 1;

                PyObject *concat_str = Py_Unicode_Writer_ਅੰਤ(writer);
                ਜੇ (concat_str == ਨਿਰਾ) {
                    ਵਾਪਸ ਨਿਰਾ;
                }
                ਜੇ (Py_Arena_ਜੋੜੋ_Py_Object(p->arena, concat_str) < 0) {
                    Py_DECREF(concat_str);
                    ਵਾਪਸ ਨਿਰਾ;
                }
                elem = _PyAST_Constant(concat_str, ਕਿਸਮ, ਪਹਿਲਾ_elem->lineno,
                                       ਪਹਿਲਾ_elem->ਖਾਨਾ_ਵਿੱਥ,
                                       ਆਖਰੀ_elem->ਸਿਰਾ_lineno,
                                       ਆਖਰੀ_elem->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, p->arena);
                ਜੇ (elem == ਨਿਰਾ) {
                    ਵਾਪਸ ਨਿਰਾ;
                }
            }

            /* Drop all empty contanst strings */
            ਜੇ (PyUnicode_CheckExact(elem->v.Constant.ਮੁੱਲ) &&
                Py_Unicode_GET_ਲੰਮਾਈ(elem->v.Constant.ਮੁੱਲ) == 0) {
                ਜਾਰੀ;
            }
        }

        asdl_seq_ਸੈੱਟ(ਮੁੱਲਾਂ, current_pos++, elem);
    }

    ਪ੍ਰਤਿਜ੍ਞਾ(current_pos == n_elements);
    ਵਾਪਸ ਮੁੱਲਾਂ;
}

ਸਥਿਰ ਵਿਅੰਜਕ_ty
_build_concatenated_joined_str(Parser *p, asdl_ਵਿਅੰਜਕ_seq *ਲੜੀਆਂ,
                               ਪੂਰਨ lineno, ਪੂਰਨ ਖਾਨਾ_ਵਿੱਥ, ਪੂਰਨ ਸਿਰਾ_lineno,
                               ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, PyArena *arena)
{
    asdl_ਵਿਅੰਜਕ_seq *ਮੁੱਲਾਂ = _build_concatenated_str(p, ਲੜੀਆਂ, lineno,
        ਖਾਨਾ_ਵਿੱਥ, ਸਿਰਾ_lineno, ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, arena);
    ਜੇ (!ਮੁੱਲਾਂ) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ _PyAST_JoinedStr(ਮੁੱਲਾਂ, lineno, ਖਾਨਾ_ਵਿੱਥ, ਸਿਰਾ_lineno, ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, p->arena);
}

ਵਿਅੰਜਕ_ty
_PyPegen_concatenate_tstrings(Parser *p, asdl_ਵਿਅੰਜਕ_seq *ਲੜੀਆਂ,
                               ਪੂਰਨ lineno, ਪੂਰਨ ਖਾਨਾ_ਵਿੱਥ, ਪੂਰਨ ਸਿਰਾ_lineno,
                               ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, PyArena *arena)
{
    asdl_ਵਿਅੰਜਕ_seq *ਮੁੱਲਾਂ = _build_concatenated_str(p, ਲੜੀਆਂ, lineno,
        ਖਾਨਾ_ਵਿੱਥ, ਸਿਰਾ_lineno, ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, arena);
    ਜੇ (!ਮੁੱਲਾਂ) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ _PyAST_TemplateStr(ਮੁੱਲਾਂ, lineno, ਖਾਨਾ_ਵਿੱਥ, ਸਿਰਾ_lineno,
        ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, arena);
}

ਵਿਅੰਜਕ_ty
Py_Pegen_concatenate_ਲੜੀਆਂ(Parser *p, asdl_ਵਿਅੰਜਕ_seq *ਲੜੀਆਂ,
                             ਪੂਰਨ lineno, ਪੂਰਨ ਖਾਨਾ_ਵਿੱਥ, ਪੂਰਨ ਸਿਰਾ_lineno,
                             ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(ਲੜੀਆਂ);
    ਪ੍ਰਤਿਜ੍ਞਾ(len > 0);

    ਪੂਰਨ f_string_ਮਿਲਿਆ = 0;
    ਪੂਰਨ unicode_string_ਮਿਲਿਆ = 0;
    ਪੂਰਨ ਬਾਈਟਾਂ_ਮਿਲਿਆ = 0;

    Py_ssize_t i = 0;
    ਲਈ (i = 0; i < len; i++) {
        ਵਿਅੰਜਕ_ty elem = asdl_seq_GET(ਲੜੀਆਂ, i);
        ਚੋਣ(elem->ਕਿਸਮ) {
            ਹਾਲਤ Constant_ਕਿਸਮ:
                ਜੇ (Py_ਬਾਈਟਾਂ_Check_Exact(elem->v.Constant.ਮੁੱਲ)) {
                    ਬਾਈਟਾਂ_ਮਿਲਿਆ = 1;
                } ਨਹੀਂਤਾਂ {
                    unicode_string_ਮਿਲਿਆ = 1;
                }
                ਰੋਕੋ;
            ਹਾਲਤ Joined_Str_ਕਿਸਮ:
                f_string_ਮਿਲਿਆ = 1;
                ਰੋਕੋ;
            ਹਾਲਤ Template_Str_ਕਿਸਮ:
                // python.gram handles this; we should never get here
                ਪ੍ਰਤਿਜ੍ਞਾ(0);
                ਰੋਕੋ;
            ਮੂਲ:
                f_string_ਮਿਲਿਆ = 1;
                ਰੋਕੋ;
        }
    }

    // Cannot mix unicode and bytes
    ਜੇ ((unicode_string_ਮਿਲਿਆ || f_string_ਮਿਲਿਆ) && ਬਾਈਟਾਂ_ਮਿਲਿਆ) {
        RAISE_SYNTAX_ERROR("cannot mix bytes and nonbytes literals");
        ਵਾਪਸ ਨਿਰਾ;
    }

    // If it's only bytes or only unicode string, do a simple concat
    ਜੇ (!f_string_ਮਿਲਿਆ) {
        ਜੇ (len == 1) {
            ਵਾਪਸ asdl_seq_GET(ਲੜੀਆਂ, 0);
        }
        ਨਹੀਂਤਾਂ ਜੇ (ਬਾਈਟਾਂ_ਮਿਲਿਆ) {
            ਵਾਪਸ build_concatenated_ਬਾਈਟਾਂ(p, ਲੜੀਆਂ, lineno, ਖਾਨਾ_ਵਿੱਥ,
                ਸਿਰਾ_lineno, ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, arena);
        }
        ਨਹੀਂਤਾਂ {
            ਵਾਪਸ _build_concatenated_unicode(p, ਲੜੀਆਂ, lineno, ਖਾਨਾ_ਵਿੱਥ,
                ਸਿਰਾ_lineno, ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, arena);
        }
    }

    ਵਾਪਸ _build_concatenated_joined_str(p, ਲੜੀਆਂ, lineno,
        ਖਾਨਾ_ਵਿੱਥ, ਸਿਰਾ_lineno, ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, arena);
}

ਸਥਿਰ ਪੂਰਨ
warn_relative_import_ਦਾ_lazy(Parser *p, asdl_seq *dots, ਵਿਅੰਜਕ_ty module)
{
    // Warn about `from . lazy import x`: the whitespace between the dots and
    // the module name is insignificant, so this is parsed exactly like
    // `from .lazy import x` (an import of the relative module "lazy"), but it
    // is most likely a transposition of `lazy from . import x` (PEP 810).
    ਜੇ (p->call_invalid_ਨਿਯਮਾਂ) {
        ਵਾਪਸ 0;
    }

    // Only fire if there is whitespace between the last dot and the name,
    // i.e. not for the common `from .lazy import x` spelling.
    Token *ਆਖਰੀ_dot = asdl_seq_GET_UNTYPED(dots, asdl_seq_LEN(dots) - 1);
    ਜੇ (
        ਆਖਰੀ_dot->ਸਿਰਾ_lineno == module->lineno
        && ਆਖਰੀ_dot->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ == module->ਖਾਨਾ_ਵਿੱਥ
    ) {
        ਵਾਪਸ 0;
    }

    ਪੂਰਨ ਗਿਣਤੀ_2 = Py_Pegen_seq_ਗਿਣਤੀ_dots(dots);
    ਅੱਖਰ *buf = PyMem_RawMalloc(ਗਿਣਤੀ_2 + 1);
    ਜੇ (buf == ਨਿਰਾ) {
        Py_Err_No_ਸਮ੍ਰਿਤੀ();
        ਵਾਪਸ -1;
    }
    memset(buf, '.', ਗਿਣਤੀ_2);
    buf[ਗਿਣਤੀ_2] = '\0';

    PyObject *ਸੁਨੇਹਾ = PyUnicode_FromFormat(
        "'from %s lazy import' is the same as 'from %slazy import'; "
        "did you mean 'lazy from %s import'?",
        buf, buf, buf);
    PyMem_RawFree(buf);
    ਜੇ (ਸੁਨੇਹਾ == ਨਿਰਾ) {
        ਵਾਪਸ -1;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    ਪੂਰਨ res = _PyErr_EmitSyntaxWarning(ਸੁਨੇਹਾ,
                                       info.filename,
                                       module->lineno,
                                       module->ਖਾਨਾ_ਵਿੱਥ + 1,
                                       module->ਸਿਰਾ_lineno,
                                       module->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ + 1,
                                       info.module);
    Py_DECREF(ਸੁਨੇਹਾ);
    ਵਾਪਸ res;
}

stmt_ty
_PyPegen_checked_from_import(Parser *p, asdl_seq *dots, ਵਿਅੰਜਕ_ty module_name,
                             asdl_alias_seq *ਨਾਂ, ਵਿਅੰਜਕ_ty lazy_token, ਪੂਰਨ lineno,
                             ਪੂਰਨ ਖਾਨਾ_ਵਿੱਥ, ਪੂਰਨ ਸਿਰਾ_lineno, ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ,
                             PyArena *arena)
{
    identifier module = module_name->v.Name.id;
    ਪੂਰਨ level = Py_Pegen_seq_ਗਿਣਤੀ_dots(dots);
    ਜੇ (level == 0 && PyUnicode_CompareWithASCIIString(module, "__future__") == 0) {
        ਜੇ (lazy_token) {
            RAISE_SYNTAX_ERROR_ਗਿਆਤ_LOCATION(lazy_token,
                "lazy from __future__ import is not allowed");
            ਵਾਪਸ ਨਿਰਾ;
        }
        ਲਈ (Py_ssize_t i = 0; i < asdl_seq_LEN(ਨਾਂ); i++) {
            alias_ty alias = asdl_seq_GET(ਨਾਂ, i);
            ਜੇ (PyUnicode_CompareWithASCIIString(alias->name, "barry_as_FLUFL") == 0) {
                p->flags |= PyPARSE_BARRY_AS_BDFL;
                p->tok->barry_as_bdfl = 1;
            }
        }
    }
    ਨਹੀਂਤਾਂ ਜੇ (
        level > 0
        && lazy_token == ਨਿਰਾ
        && PyUnicode_CompareWithASCIIString(module, "lazy") == 0
    ) {
        ਜੇ (warn_relative_import_ਦਾ_lazy(p, dots, module_name) < 0) {
            ਵਾਪਸ ਨਿਰਾ;
        }
    }
    ਵਾਪਸ _PyAST_ImportFrom(module, ਨਾਂ, level, lazy_token ? 1 : 0, lineno,
                             ਖਾਨਾ_ਵਿੱਥ, ਸਿਰਾ_lineno, ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, arena);
}

asdl_stmt_seq*
Py_Pegen_ਰਜਿਸਟਰ_stmts(Parser *p, asdl_stmt_seq* stmts) {
    ਜੇ (!p->call_invalid_ਨਿਯਮਾਂ) {
        ਵਾਪਸ stmts;
    }
    Py_ssize_t len = asdl_seq_LEN(stmts);
    ਜੇ (len == 0) {
        ਵਾਪਸ stmts;
    }
    stmt_ty ਆਖਰੀ_stmt = asdl_seq_GET(stmts, len - 1);
    ਜੇ (p->ਆਖਰੀ_stmt_location.lineno > ਆਖਰੀ_stmt->lineno) {
        ਵਾਪਸ stmts;
    }
    p->ਆਖਰੀ_stmt_location.lineno = ਆਖਰੀ_stmt->lineno;
    p->ਆਖਰੀ_stmt_location.ਖਾਨਾ_ਵਿੱਥ = ਆਖਰੀ_stmt->ਖਾਨਾ_ਵਿੱਥ;
    p->ਆਖਰੀ_stmt_location.ਸਿਰਾ_lineno = ਆਖਰੀ_stmt->ਸਿਰਾ_lineno;
    p->ਆਖਰੀ_stmt_location.ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ = ਆਖਰੀ_stmt->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ;
    ਵਾਪਸ stmts;
}
