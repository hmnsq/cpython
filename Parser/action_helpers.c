#અન્તર્ભાવ <Python.h>
#અન્તર્ભાવ "pycore_pystate.h"         // _PyInterpreterState_GET()
#અન્તર્ભાવ "pycore_runtime.h"         // _PyRuntime
#અન્તર્ભાવ "pycore_unicodeobject.h"   // _PyUnicode_InternImmortal()
#અન્તર્ભાવ "pycore_pyerrors.h"        // _PyErr_EmitSyntaxWarning()

#અન્તર્ભાવ "pegen.h"
#અન્તર્ભાવ "string_parser.h"          // _PyPegen_decode_string()
#અન્તર્ભાવ "lexer/state.h"            // tok_state


શૂન્ય *
_PyPegen_dummy_name(Parser *p, ...)
{
    પાછા &_PyRuntime.parser.dummy_name;
}

/* Creates a single-element asdl_seq* that contains a */
asdl_seq *
_PyPegen_singleton_seq(Parser *p, શૂન્ય *a)
{
    પ્રતિજ્ઞા(a != ખાલી);
    asdl_seq *seq = (asdl_seq*)_Py_asdl_generic_seq_new(1, p->arena);
    જો (!seq) {
        પાછા ખાલી;
    }
    asdl_seq_SET_UNTYPED(seq, 0, a);
    પાછા seq;
}

/* Creates a copy of seq and prepends a to it */
asdl_seq *
Py_Pegen_seq_ઉમેરો_in_front(Parser *p, શૂન્ય *a, asdl_seq *seq)
{
    પ્રતિજ્ઞા(a != ખાલી);
    જો (!seq) {
        પાછા _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    જો (!new_seq) {
        પાછા ખાલી;
    }

    asdl_seq_SET_UNTYPED(new_seq, 0, a);
    માટે (Py_ssize_t i = 1, l = asdl_seq_LEN(new_seq); i < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i - 1));
    }
    પાછા new_seq;
}

/* Creates a copy of seq and appends a to it */
asdl_seq *
_PyPegen_seq_append_to_end(Parser *p, asdl_seq *seq, શૂન્ય *a)
{
    પ્રતિજ્ઞા(a != ખાલી);
    જો (!seq) {
        પાછા _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    જો (!new_seq) {
        પાછા ખાલી;
    }

    માટે (Py_ssize_t i = 0, l = asdl_seq_LEN(new_seq); i + 1 < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i));
    }
    asdl_seq_SET_UNTYPED(new_seq, asdl_seq_LEN(new_seq) - 1, a);
    પાછા new_seq;
}

સ્થિર Py_ssize_t
_get_flattened_seq_size(asdl_seq *seqs)
{
    Py_ssize_t size = 0;
    માટે (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        size += asdl_seq_LEN(inner_seq);
    }
    પાછા size;
}

/* Flattens an asdl_seq* of asdl_seq*s */
asdl_seq *
_PyPegen_seq_flatten(Parser *p, asdl_seq *seqs)
{
    Py_ssize_t flattened_seq_size = _get_flattened_seq_size(seqs);
    પ્રતિજ્ઞા(flattened_seq_size > 0);

    asdl_seq *flattened_seq = (asdl_seq*)_Py_asdl_generic_seq_new(flattened_seq_size, p->arena);
    જો (!flattened_seq) {
        પાછા ખાલી;
    }

    પૂર્ણાંક flattened_seq_idx = 0;
    માટે (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        માટે (Py_ssize_t j = 0, li = asdl_seq_LEN(inner_seq); j < li; j++) {
            asdl_seq_SET_UNTYPED(flattened_seq, flattened_seq_idx++, asdl_seq_GET_UNTYPED(inner_seq, j));
        }
    }
    પ્રતિજ્ઞા(flattened_seq_idx == flattened_seq_size);

    પાછા flattened_seq;
}

શૂન્ય *
_PyPegen_seq_last_item(asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    પાછા asdl_seq_GET_UNTYPED(seq, len - 1);
}

શૂન્ય *
_PyPegen_seq_first_item(asdl_seq *seq)
{
    પાછા asdl_seq_GET_UNTYPED(seq, 0);
}

/* Creates a new name of the form <first_name>.<second_name> */
પદાવલિ_ty
_PyPegen_join_names_with_dot(Parser *p, પદાવલિ_ty first_name, પદાવલિ_ty second_name)
{
    પ્રતિજ્ઞા(first_name != ખાલી && second_name != ખાલી);
    PyObject *uni = PyUnicode_FromFormat("%U.%U",
            first_name->v.Name.id, second_name->v.Name.id);
    જો (!uni) {
        પાછા ખાલી;
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &uni);
    જો (_PyArena_AddPyObject(p->arena, uni) < 0) {
        Py_DECREF(uni);
        પાછા ખાલી;
    }

    પાછા _PyAST_Name(uni, Load, EXTRA_પદાવલિ(first_name, second_name));
}

/* Counts the total number of dots in seq's tokens */
પૂર્ણાંક
Py_Pegen_seq_ગણતરી_dots(asdl_seq *seq)
{
    પૂર્ણાંક સંખ્યા_of_dots = 0;
    માટે (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        Token *current_પદાવલિ = asdl_seq_GET_UNTYPED(seq, i);
        પસંદગી (current_પદાવલિ->type) {
            સ્થિતિ ELLIPSIS:
                સંખ્યા_of_dots += 3;
                તોડો;
            સ્થિતિ DOT:
                સંખ્યા_of_dots += 1;
                તોડો;
            મૂળભૂત:
                Py_UNREACHABLE();
        }
    }

    પાછા સંખ્યા_of_dots;
}

/* Creates an alias with '*' as the identifier name */
alias_ty
_PyPegen_alias_for_star(Parser *p, પૂર્ણાંક lineno, પૂર્ણાંક col_offset, પૂર્ણાંક end_lineno,
                        પૂર્ણાંક end_col_offset, PyArena *arena) {
    PyObject *str = PyUnicode_InternFromString("*");
    જો (!str) {
        પાછા ખાલી;
    }
    જો (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        પાછા ખાલી;
    }
    પાછા _PyAST_alias(str, ખાલી, lineno, col_offset, end_lineno, end_col_offset, arena);
}

/* Creates a new asdl_seq* with the identifiers of all the names in seq */
asdl_identifier_seq *
_PyPegen_map_names_to_ids(Parser *p, asdl_પદાવલિ_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    પ્રતિજ્ઞા(len > 0);

    asdl_identifier_seq *new_seq = _Py_asdl_identifier_seq_new(len, p->arena);
    જો (!new_seq) {
        પાછા ખાલી;
    }
    માટે (Py_ssize_t i = 0; i < len; i++) {
        પદાવલિ_ty e = asdl_seq_GET(seq, i);
        asdl_seq_SET(new_seq, i, e->v.Name.id);
    }
    પાછા new_seq;
}

/* Constructs a CmpopExprPair */
Cmpop_પદાવલિ_Pair *
Py_Pegen_cmpop_પદાવલિ_pair(Parser *p, cmpop_ty cmpop, પદાવલિ_ty પદાવલિ)
{
    પ્રતિજ્ઞા(પદાવલિ != ખાલી);
    Cmpop_પદાવલિ_Pair *a = _PyArena_Malloc(p->arena, કદ(Cmpop_પદાવલિ_Pair));
    જો (!a) {
        પાછા ખાલી;
    }
    a->cmpop = cmpop;
    a->પદાવલિ = પદાવલિ;
    પાછા a;
}

asdl_int_seq *
_PyPegen_get_cmpops(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    પ્રતિજ્ઞા(len > 0);

    asdl_int_seq *new_seq = _Py_asdl_int_seq_new(len, p->arena);
    જો (!new_seq) {
        પાછા ખાલી;
    }
    માટે (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_પદાવલિ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->cmpop);
    }
    પાછા new_seq;
}

asdl_પદાવલિ_seq *
_PyPegen_get_exprs(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    પ્રતિજ્ઞા(len > 0);

    asdl_પદાવલિ_seq *new_seq = Py_asdl_પદાવલિ_seq_new(len, p->arena);
    જો (!new_seq) {
        પાછા ખાલી;
    }
    માટે (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_પદાવલિ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->પદાવલિ);
    }
    પાછા new_seq;
}

/* Creates an asdl_seq* where all the elements have been changed to have ctx as context */
સ્થિર asdl_પદાવલિ_seq *
_set_seq_context(Parser *p, asdl_પદાવલિ_seq *seq, પદાવલિ_context_ty ctx)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    જો (len == 0) {
        પાછા ખાલી;
    }

    asdl_પદાવલિ_seq *new_seq = Py_asdl_પદાવલિ_seq_new(len, p->arena);
    જો (!new_seq) {
        પાછા ખાલી;
    }
    માટે (Py_ssize_t i = 0; i < len; i++) {
        પદાવલિ_ty e = asdl_seq_GET(seq, i);
        પદાવલિ_ty new_e = Py_Pegen_set_પદાવલિ_context(p, e, ctx);
        જો (!new_e) {
            પાછા ખાલી;
        }
        asdl_seq_SET(new_seq, i, new_e);
    }
    પાછા new_seq;
}

સ્થિર પદાવલિ_ty
_set_name_context(Parser *p, પદાવલિ_ty e, પદાવલિ_context_ty ctx)
{
    પાછા _PyAST_Name(e->v.Name.id, ctx, EXTRA_પદાવલિ(e, e));
}

સ્થિર પદાવલિ_ty
_set_tuple_context(Parser *p, પદાવલિ_ty e, પદાવલિ_context_ty ctx)
{
    asdl_પદાવલિ_seq *seq = _set_seq_context(p, e->v.Tuple.elts, ctx);
    જો (!seq && PyErr_Occurred()) {
        પાછા ખાલી;
    }
    પાછા _PyAST_Tuple(seq, ctx, EXTRA_પદાવલિ(e, e));
}

સ્થિર પદાવલિ_ty
_set_list_context(Parser *p, પદાવલિ_ty e, પદાવલિ_context_ty ctx)
{
    asdl_પદાવલિ_seq *seq = _set_seq_context(p, e->v.List.elts, ctx);
    જો (!seq && PyErr_Occurred()) {
        પાછા ખાલી;
    }
    પાછા _PyAST_List(seq, ctx, EXTRA_પદાવલિ(e, e));
}

સ્થિર પદાવલિ_ty
_set_subscript_context(Parser *p, પદાવલિ_ty e, પદાવલિ_context_ty ctx)
{
    પાછા _PyAST_Subscript(e->v.Subscript.કિંમત, e->v.Subscript.slice,
                            ctx, EXTRA_પદાવલિ(e, e));
}

સ્થિર પદાવલિ_ty
_set_attribute_context(Parser *p, પદાવલિ_ty e, પદાવલિ_context_ty ctx)
{
    પાછા _PyAST_Attribute(e->v.Attribute.કિંમત, e->v.Attribute.attr,
                            ctx, EXTRA_પદાવલિ(e, e));
}

સ્થિર પદાવલિ_ty
_set_starred_context(Parser *p, પદાવલિ_ty e, પદાવલિ_context_ty ctx)
{
    પદાવલિ_ty inner = Py_Pegen_set_પદાવલિ_context(p, e->v.Starred.કિંમત, ctx);
    જો (!inner) {
        પાછા ખાલી;
    }
    પાછા _PyAST_Starred(inner, ctx, EXTRA_પદાવલિ(e, e));
}

/* Creates an `expr_ty` equivalent to `expr` but with `ctx` as context */
પદાવલિ_ty
Py_Pegen_set_પદાવલિ_context(Parser *p, પદાવલિ_ty પદાવલિ, પદાવલિ_context_ty ctx)
{
    પ્રતિજ્ઞા(પદાવલિ != ખાલી);

    પદાવલિ_ty new = ખાલી;
    પસંદગી (પદાવલિ->kind) {
        સ્થિતિ Name_kind:
            new = _set_name_context(p, પદાવલિ, ctx);
            તોડો;
        સ્થિતિ Tuple_kind:
            new = _set_tuple_context(p, પદાવલિ, ctx);
            તોડો;
        સ્થિતિ List_kind:
            new = _set_list_context(p, પદાવલિ, ctx);
            તોડો;
        સ્થિતિ Subscript_kind:
            new = _set_subscript_context(p, પદાવલિ, ctx);
            તોડો;
        સ્થિતિ Attribute_kind:
            new = _set_attribute_context(p, પદાવલિ, ctx);
            તોડો;
        સ્થિતિ Starred_kind:
            new = _set_starred_context(p, પદાવલિ, ctx);
            તોડો;
        મૂળભૂત:
            new = પદાવલિ;
    }
    પાછા new;
}

/* Invalid targets return NULL without raising, allowing parsing to backtrack.
   Copy valid targets to preserve the memoized expression's context. */
પદાવલિ_ty
_PyPegen_make_target(Parser *p, પદાવલિ_ty પદાવલિ, TARGETS_TYPE targets_type)
{
    પ્રતિજ્ઞા(પદાવલિ != ખાલી);
    પ્રતિજ્ઞા(targets_type != FOR_TARGETS);
    જો (_PyPegen_get_invalid_target(પદાવલિ, targets_type) != ખાલી) {
        પાછા ખાલી;
    }
    પાછા Py_Pegen_set_પદાવલિ_context(
        p, પદાવલિ, targets_type == DEL_TARGETS ? Del : Store);
}

/* Constructs a KeyValuePair that is used when parsing a dict's key value pairs */
ચાવી_કિંમત_Pair *
Py_Pegen_ચાવી_કિંમત_pair(Parser *p, પદાવલિ_ty ચાવી, પદાવલિ_ty કિંમત)
{
    ચાવી_કિંમત_Pair *a = _PyArena_Malloc(p->arena, કદ(ચાવી_કિંમત_Pair));
    જો (!a) {
        પાછા ખાલી;
    }
    a->ચાવી = ચાવી;
    a->કિંમત = કિંમત;
    પાછા a;
}

/* Extracts all keys from an asdl_seq* of KeyValuePair*'s */
asdl_પદાવલિ_seq *
Py_Pegen_get_ચાવીઓ(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_પદાવલિ_seq *new_seq = Py_asdl_પદાવલિ_seq_new(len, p->arena);
    જો (!new_seq) {
        પાછા ખાલી;
    }
    માટે (Py_ssize_t i = 0; i < len; i++) {
        ચાવી_કિંમત_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->ચાવી);
    }
    પાછા new_seq;
}

/* Extracts all values from an asdl_seq* of KeyValuePair*'s */
asdl_પદાવલિ_seq *
Py_Pegen_get_કિંમતો(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_પદાવલિ_seq *new_seq = Py_asdl_પદાવલિ_seq_new(len, p->arena);
    જો (!new_seq) {
        પાછા ખાલી;
    }
    માટે (Py_ssize_t i = 0; i < len; i++) {
        ચાવી_કિંમત_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->કિંમત);
    }
    પાછા new_seq;
}

/* Constructs a KeyPatternPair that is used when parsing mapping & class patterns */
ચાવી_Pattern_Pair *
Py_Pegen_ચાવી_pattern_pair(Parser *p, પદાવલિ_ty ચાવી, pattern_ty pattern)
{
    ચાવી_Pattern_Pair *a = _PyArena_Malloc(p->arena, કદ(ચાવી_Pattern_Pair));
    જો (!a) {
        પાછા ખાલી;
    }
    a->ચાવી = ચાવી;
    a->pattern = pattern;
    પાછા a;
}

/* Extracts all keys from an asdl_seq* of KeyPatternPair*'s */
asdl_પદાવલિ_seq *
Py_Pegen_get_pattern_ચાવીઓ(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_પદાવલિ_seq *new_seq = Py_asdl_પદાવલિ_seq_new(len, p->arena);
    જો (!new_seq) {
        પાછા ખાલી;
    }
    માટે (Py_ssize_t i = 0; i < len; i++) {
        ચાવી_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->ચાવી);
    }
    પાછા new_seq;
}

/* Extracts all patterns from an asdl_seq* of KeyPatternPair*'s */
asdl_pattern_seq *
_PyPegen_get_patterns(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_pattern_seq *new_seq = _Py_asdl_pattern_seq_new(len, p->arena);
    જો (!new_seq) {
        પાછા ખાલી;
    }
    માટે (Py_ssize_t i = 0; i < len; i++) {
        ચાવી_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->pattern);
    }
    પાછા new_seq;
}

/* Constructs a NameDefaultPair */
NameDefaultPair *
_PyPegen_name_default_pair(Parser *p, arg_ty arg, પદાવલિ_ty કિંમત, Token *tc)
{
    NameDefaultPair *a = _PyArena_Malloc(p->arena, કદ(NameDefaultPair));
    જો (!a) {
        પાછા ખાલી;
    }
    a->arg = _PyPegen_add_type_comment_to_arg(p, arg, tc);
    જો (!a->arg) {
        પાછા ખાલી;
    }
    a->કિંમત = કિંમત;
    પાછા a;
}

/* Constructs a SlashWithDefault */
SlashWithDefault *
_PyPegen_slash_with_default(Parser *p, asdl_arg_seq *plain_names, asdl_seq *names_with_defaults)
{
    SlashWithDefault *a = _PyArena_Malloc(p->arena, કદ(SlashWithDefault));
    જો (!a) {
        પાછા ખાલી;
    }
    a->plain_names = plain_names;
    a->names_with_defaults = names_with_defaults;
    પાછા a;
}

/* Constructs a StarEtc */
StarEtc *
_PyPegen_star_etc(Parser *p, arg_ty vararg, asdl_seq *kwonlyargs, arg_ty kwarg)
{
    StarEtc *a = _PyArena_Malloc(p->arena, કદ(StarEtc));
    જો (!a) {
        પાછા ખાલી;
    }
    a->vararg = vararg;
    a->kwonlyargs = kwonlyargs;
    a->kwarg = kwarg;
    પાછા a;
}

asdl_seq *
_PyPegen_join_sequences(Parser *p, asdl_seq *a, asdl_seq *b)
{
    Py_ssize_t first_len = asdl_seq_LEN(a);
    Py_ssize_t second_len = asdl_seq_LEN(b);
    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(first_len + second_len, p->arena);
    જો (!new_seq) {
        પાછા ખાલી;
    }

    પૂર્ણાંક k = 0;
    માટે (Py_ssize_t i = 0; i < first_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(a, i));
    }
    માટે (Py_ssize_t i = 0; i < second_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(b, i));
    }

    પાછા new_seq;
}

સ્થિર asdl_arg_seq*
_get_names(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_arg_seq *seq = _Py_asdl_arg_seq_new(len, p->arena);
    જો (!seq) {
        પાછા ખાલી;
    }
    માટે (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->arg);
    }
    પાછા seq;
}

સ્થિર asdl_પદાવલિ_seq *
_get_defaults(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_પદાવલિ_seq *seq = Py_asdl_પદાવલિ_seq_new(len, p->arena);
    જો (!seq) {
        પાછા ખાલી;
    }
    માટે (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->કિંમત);
    }
    પાછા seq;
}

સ્થિર પૂર્ણાંક
_make_posonlyargs(Parser *p,
                  asdl_arg_seq *slash_without_default,
                  SlashWithDefault *slash_with_default,
                  asdl_arg_seq **posonlyargs) {
    જો (slash_without_default != ખાલી) {
        *posonlyargs = slash_without_default;
    }
    નહીંતર જો (slash_with_default != ખાલી) {
        asdl_arg_seq *slash_with_default_names =
                _get_names(p, slash_with_default->names_with_defaults);
        જો (!slash_with_default_names) {
            પાછા -1;
        }
        *posonlyargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default->plain_names,
                (asdl_seq*)slash_with_default_names);
    }
    નહીંતર {
        *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }
    પાછા *posonlyargs == ખાલી ? -1 : 0;
}

સ્થિર પૂર્ણાંક
_make_posargs(Parser *p,
              asdl_arg_seq *plain_names,
              asdl_seq *names_with_default,
              asdl_arg_seq **posargs) {

    જો (names_with_default != ખાલી) {
        જો (plain_names != ખાલી) {
            asdl_arg_seq *names_with_default_names = _get_names(p, names_with_default);
            જો (!names_with_default_names) {
                પાછા -1;
            }
            *posargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                    p,(asdl_seq*)plain_names, (asdl_seq*)names_with_default_names);
        }
        નહીંતર {
            *posargs = _get_names(p, names_with_default);
        }
    }
    નહીંતર {
        જો (plain_names != ખાલી) {
            // With the current grammar, we never get here.
            // If that has changed, remove the assert, and test thoroughly.
            પ્રતિજ્ઞા(0);
            *posargs = plain_names;
        }
        નહીંતર {
            *posargs = _Py_asdl_arg_seq_new(0, p->arena);
        }
    }
    પાછા *posargs == ખાલી ? -1 : 0;
}

સ્થિર પૂર્ણાંક
_make_posdefaults(Parser *p,
                  SlashWithDefault *slash_with_default,
                  asdl_seq *names_with_default,
                  asdl_પદાવલિ_seq **posdefaults) {
    જો (slash_with_default != ખાલી && names_with_default != ખાલી) {
        asdl_પદાવલિ_seq *slash_with_default_કિંમતો =
                _get_defaults(p, slash_with_default->names_with_defaults);
        જો (!slash_with_default_કિંમતો) {
            પાછા -1;
        }
        asdl_પદાવલિ_seq *names_with_default_કિંમતો = _get_defaults(p, names_with_default);
        જો (!names_with_default_કિંમતો) {
            પાછા -1;
        }
        *posdefaults = (asdl_પદાવલિ_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default_કિંમતો,
                (asdl_seq*)names_with_default_કિંમતો);
    }
    નહીંતર જો (slash_with_default == ખાલી && names_with_default != ખાલી) {
        *posdefaults = _get_defaults(p, names_with_default);
    }
    નહીંતર જો (slash_with_default != ખાલી && names_with_default == ખાલી) {
        *posdefaults = _get_defaults(p, slash_with_default->names_with_defaults);
    }
    નહીંતર {
        *posdefaults = Py_asdl_પદાવલિ_seq_new(0, p->arena);
    }
    પાછા *posdefaults == ખાલી ? -1 : 0;
}

સ્થિર પૂર્ણાંક
_make_kwargs(Parser *p, StarEtc *star_etc,
             asdl_arg_seq **kwonlyargs,
             asdl_પદાવલિ_seq **kwdefaults) {
    જો (star_etc != ખાલી && star_etc->kwonlyargs != ખાલી) {
        *kwonlyargs = _get_names(p, star_etc->kwonlyargs);
    }
    નહીંતર {
        *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }

    જો (*kwonlyargs == ખાલી) {
        પાછા -1;
    }

    જો (star_etc != ખાલી && star_etc->kwonlyargs != ખાલી) {
        *kwdefaults = _get_defaults(p, star_etc->kwonlyargs);
    }
    નહીંતર {
        *kwdefaults = Py_asdl_પદાવલિ_seq_new(0, p->arena);
    }

    જો (*kwdefaults == ખાલી) {
        પાછા -1;
    }

    પાછા 0;
}

/* Constructs an arguments_ty object out of all the parsed constructs in the parameters rule */
arguments_ty
_PyPegen_make_arguments(Parser *p, asdl_arg_seq *slash_without_default,
                        SlashWithDefault *slash_with_default, asdl_arg_seq *plain_names,
                        asdl_seq *names_with_default, StarEtc *star_etc)
{
    asdl_arg_seq *posonlyargs;
    જો (_make_posonlyargs(p, slash_without_default, slash_with_default, &posonlyargs) == -1) {
        પાછા ખાલી;
    }

    asdl_arg_seq *posargs;
    જો (_make_posargs(p, plain_names, names_with_default, &posargs) == -1) {
        પાછા ખાલી;
    }

    asdl_પદાવલિ_seq *posdefaults;
    જો (_make_posdefaults(p,slash_with_default, names_with_default, &posdefaults) == -1) {
        પાછા ખાલી;
    }

    arg_ty vararg = ખાલી;
    જો (star_etc != ખાલી && star_etc->vararg != ખાલી) {
        vararg = star_etc->vararg;
    }

    asdl_arg_seq *kwonlyargs;
    asdl_પદાવલિ_seq *kwdefaults;
    જો (_make_kwargs(p, star_etc, &kwonlyargs, &kwdefaults) == -1) {
        પાછા ખાલી;
    }

    arg_ty kwarg = ખાલી;
    જો (star_etc != ખાલી && star_etc->kwarg != ખાલી) {
        kwarg = star_etc->kwarg;
    }

    પાછા _PyAST_arguments(posonlyargs, posargs, vararg, kwonlyargs,
                            kwdefaults, kwarg, posdefaults, p->arena);
}


/* Constructs an empty arguments_ty object, that gets used when a function accepts no
 * arguments. */
arguments_ty
_PyPegen_empty_arguments(Parser *p)
{
    asdl_arg_seq *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    જો (!posonlyargs) {
        પાછા ખાલી;
    }
    asdl_arg_seq *posargs = _Py_asdl_arg_seq_new(0, p->arena);
    જો (!posargs) {
        પાછા ખાલી;
    }
    asdl_પદાવલિ_seq *posdefaults = Py_asdl_પદાવલિ_seq_new(0, p->arena);
    જો (!posdefaults) {
        પાછા ખાલી;
    }
    asdl_arg_seq *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    જો (!kwonlyargs) {
        પાછા ખાલી;
    }
    asdl_પદાવલિ_seq *kwdefaults = Py_asdl_પદાવલિ_seq_new(0, p->arena);
    જો (!kwdefaults) {
        પાછા ખાલી;
    }

    પાછા _PyAST_arguments(posonlyargs, posargs, ખાલી, kwonlyargs,
                            kwdefaults, ખાલી, posdefaults, p->arena);
}

/* Encapsulates the value of an operator_ty into an AugOperator struct */
AugOperator *
_PyPegen_augoperator(Parser *p, operator_ty kind)
{
    AugOperator *a = _PyArena_Malloc(p->arena, કદ(AugOperator));
    જો (!a) {
        પાછા ખાલી;
    }
    a->kind = kind;
    પાછા a;
}

/* Construct a FunctionDef equivalent to function_def, but with decorators */
stmt_ty
_PyPegen_function_def_decorators(Parser *p, asdl_પદાવલિ_seq *decorators, stmt_ty function_def)
{
    પ્રતિજ્ઞા(function_def != ખાલી);
    જો (function_def->kind == AsyncFunctionDef_kind) {
        પાછા _PyAST_AsyncFunctionDef(
            function_def->v.AsyncFunctionDef.name,
            function_def->v.AsyncFunctionDef.args,
            function_def->v.AsyncFunctionDef.body, decorators,
            function_def->v.AsyncFunctionDef.returns,
            function_def->v.AsyncFunctionDef.type_comment,
            function_def->v.AsyncFunctionDef.type_params,
            function_def->lineno, function_def->col_offset,
            function_def->end_lineno, function_def->end_col_offset, p->arena);
    }

    પાછા _PyAST_FunctionDef(
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
_PyPegen_class_def_decorators(Parser *p, asdl_પદાવલિ_seq *decorators, stmt_ty class_def)
{
    પ્રતિજ્ઞા(class_def != ખાલી);
    પાછા _PyAST_ClassDef(
        class_def->v.ClassDef.name,
        class_def->v.ClassDef.bases, class_def->v.ClassDef.keywords,
        class_def->v.ClassDef.body, decorators,
        class_def->v.ClassDef.type_params,
        class_def->lineno, class_def->col_offset, class_def->end_lineno,
        class_def->end_col_offset, p->arena);
}

/* Construct a KeywordOrStarred */
KeywordOrStarred *
_PyPegen_keyword_or_starred(Parser *p, શૂન્ય *element, પૂર્ણાંક is_keyword)
{
    KeywordOrStarred *a = _PyArena_Malloc(p->arena, કદ(KeywordOrStarred));
    જો (!a) {
        પાછા ખાલી;
    }
    a->element = element;
    a->is_keyword = is_keyword;
    પાછા a;
}

/* Get the number of starred expressions in an asdl_seq* of KeywordOrStarred*s */
સ્થિર પૂર્ણાંક
seq_સંખ્યા_of_starred_exprs(asdl_seq *seq)
{
    પૂર્ણાંક n = 0;
    માટે (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(seq, i);
        જો (!k->is_keyword) {
            n++;
        }
    }
    પાછા n;
}

/* Extract the starred expressions of an asdl_seq* of KeywordOrStarred*s */
asdl_પદાવલિ_seq *
_PyPegen_seq_extract_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    પૂર્ણાંક new_len = seq_સંખ્યા_of_starred_exprs(kwargs);
    જો (new_len == 0) {
        પાછા ખાલી;
    }
    asdl_પદાવલિ_seq *new_seq = Py_asdl_પદાવલિ_seq_new(new_len, p->arena);
    જો (!new_seq) {
        પાછા ખાલી;
    }

    પૂર્ણાંક idx = 0;
    માટે (Py_ssize_t i = 0, len = asdl_seq_LEN(kwargs); i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        જો (!k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    પાછા new_seq;
}

/* Return a new asdl_seq* with only the keywords in kwargs */
asdl_keyword_seq*
_PyPegen_seq_delete_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    Py_ssize_t len = asdl_seq_LEN(kwargs);
    Py_ssize_t new_len = len - seq_સંખ્યા_of_starred_exprs(kwargs);
    જો (new_len == 0) {
        પાછા ખાલી;
    }
    asdl_keyword_seq *new_seq = _Py_asdl_keyword_seq_new(new_len, p->arena);
    જો (!new_seq) {
        પાછા ખાલી;
    }

    પૂર્ણાંક idx = 0;
    માટે (Py_ssize_t i = 0; i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        જો (k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    પાછા new_seq;
}

પદાવલિ_ty
_PyPegen_ensure_imaginary(Parser *p, પદાવલિ_ty exp)
{
    જો (exp->kind != Constant_kind || !PyComplex_CheckExact(exp->v.Constant.કિંમત)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "imaginary number required in complex literal");
        પાછા ખાલી;
    }
    પાછા exp;
}

પદાવલિ_ty
_PyPegen_ensure_real(Parser *p, પદાવલિ_ty exp)
{
    જો (exp->kind != Constant_kind || PyComplex_CheckExact(exp->v.Constant.કિંમત)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "real number required in complex literal");
        પાછા ખાલી;
    }
    પાછા exp;
}

mod_ty
_PyPegen_make_module(Parser *p, asdl_stmt_seq *a) {
    asdl_type_ignore_seq *type_ignores = ખાલી;
    Py_ssize_t અંક = p->type_ignore_comments.અંક_વસ્તુઓ;
    જો (અંક > 0) {
        // Turn the raw (comment, lineno) pairs into TypeIgnore objects in the arena
        type_ignores = _Py_asdl_type_ignore_seq_new(અંક, p->arena);
        જો (type_ignores == ખાલી) {
            પાછા ખાલી;
        }
        માટે (Py_ssize_t i = 0; i < અંક; i++) {
            PyObject *tag = _PyPegen_new_type_comment(p, p->type_ignore_comments.વસ્તુઓ[i].comment);
            જો (tag == ખાલી) {
                પાછા ખાલી;
            }
            type_ignore_ty ti = _PyAST_TypeIgnore(p->type_ignore_comments.વસ્તુઓ[i].lineno,
                                                  tag, p->arena);
            જો (ti == ખાલી) {
                પાછા ખાલી;
            }
            asdl_seq_SET(type_ignores, i, ti);
        }
    }
    પાછા _PyAST_Module(a, type_ignores, p->arena);
}

PyObject *
_PyPegen_new_type_comment(Parser *p, અચલ અક્ષર *s)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, strlen(s), ખાલી);
    જો (res == ખાલી) {
        પાછા ખાલી;
    }
    જો (_PyArena_AddPyObject(p->arena, res) < 0) {
        Py_DECREF(res);
        પાછા ખાલી;
    }
    પાછા res;
}

arg_ty
_PyPegen_add_type_comment_to_arg(Parser *p, arg_ty a, Token *tc)
{
    જો (tc == ખાલી) {
        પાછા a;
    }
    અચલ અક્ષર *bytes = PyBytes_AsString(tc->bytes);
    જો (bytes == ખાલી) {
        પાછા ખાલી;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    જો (tco == ખાલી) {
        પાછા ખાલી;
    }
    પાછા _PyAST_arg(a->arg, a->annotation, tco,
                      a->lineno, a->col_offset, a->end_lineno, a->end_col_offset,
                      p->arena);
}

/* Checks if the NOTEQUAL token is valid given the current parser flags
0 indicates success and nonzero indicates failure (an exception may be set) */
પૂર્ણાંક
_PyPegen_check_barry_as_flufl(Parser *p, Token* t) {
    પ્રતિજ્ઞા(t->bytes != ખાલી);
    પ્રતિજ્ઞા(t->type == NOTEQUAL);

    અચલ અક્ષર* tok_str = PyBytes_AS_STRING(t->bytes);
    જો (p->flags & PyPARSE_BARRY_AS_BDFL && strcmp(tok_str, "<>") != 0) {
        RAISE_SYNTAX_ERROR("with Barry as BDFL, use '<>' instead of '!='");
        પાછા -1;
    }
    જો (!(p->flags & PyPARSE_BARRY_AS_BDFL)) {
        પાછા strcmp(tok_str, "!=");
    }
    પાછા 0;
}

પૂર્ણાંક
_PyPegen_check_legacy_stmt(Parser *p, પદાવલિ_ty name) {
    જો (name->kind != Name_kind) {
        પાછા 0;
    }
    અચલ અક્ષર* candidates[2] = {"print", "exec"};
    માટે (પૂર્ણાંક i=0; i<2; i++) {
        જો (PyUnicode_CompareWithASCIIString(name->v.Name.id, candidates[i]) == 0) {
            પાછા 1;
        }
    }
    પાછા 0;
}

શૂન્ય *
_PyPegen_raise_error_for_missing_comma(Parser *p, પદાવલિ_ty a, પદાવલિ_ty b)
{
    // Don't raise for legacy statements like "print x" or "exec x"
    જો (_PyPegen_check_legacy_stmt(p, a)) {
        પાછા ખાલી;
    }
    // Only raise inside parentheses/brackets (level > 0)
    જો (p->tokens[p->mark - 1]->level == 0) {
        પાછા ખાલી;
    }
    // For multi-line expressions (like string concatenations), point to the
    // last line instead of the first for a more helpful error message.
    // Use a->col_offset as the starting column since all strings in the
    // concatenation typically share the same indentation.
    જો (a->end_lineno > a->lineno) {
        પાછા RAISE_ERROR_KNOWN_LOCATION(
            p, PyExc_SyntaxError, a->end_lineno, a->col_offset,
            a->end_lineno, a->end_col_offset,
            "invalid syntax. Perhaps you forgot a comma?"
        );
    }
    પાછા RAISE_ERROR_KNOWN_LOCATION(
        p, PyExc_SyntaxError, a->lineno, a->col_offset,
        b->end_lineno, b->end_col_offset,
        "invalid syntax. Perhaps you forgot a comma?"
    );
}

સ્થિર ResultTokenWithMetadata *
result_token_with_metadata(Parser *p, શૂન્ય *result, PyObject *metadata)
{
    ResultTokenWithMetadata *res = _PyArena_Malloc(p->arena, કદ(ResultTokenWithMetadata));
    જો (res == ખાલી) {
        પાછા ખાલી;
    }
    res->metadata = metadata;
    res->result = result;
    પાછા res;
}

સ્થિર અક્ષર
formatted_string_prefix(અચલ Parser *p)
{
    પૂર્ણાંક nested = 0;
    માટે (પૂર્ણાંક i = p->mark - 1; i >= 0; i--) {
        પૂર્ણાંક type = p->tokens[i]->type;
        જો (type == FSTRING_END || type == TSTRING_END) {
            nested++;
        }
        નહીંતર જો (type == FSTRING_START || type == TSTRING_START) {
            જો (nested == 0) {
                પાછા type == TSTRING_START ? 't' : 'f';
            }
            nested--;
        }
    }
    Py_UNREACHABLE();
}

ResultTokenWithMetadata *
_PyPegen_check_fstring_conversion(Parser *p, Token* conv_token, પદાવલિ_ty conv)
{
    જો (conv_token->lineno != conv->lineno || conv_token->end_col_offset != conv->col_offset) {
        પાછા RAISE_SYNTAX_ERROR_KNOWN_RANGE(
            conv_token, conv,
            "%c-string: conversion type must come right after the exclamation mark",
            formatted_string_prefix(p)
        );
    }

    Py_UCS4 first = PyUnicode_READ_CHAR(conv->v.Name.id, 0);
    જો (PyUnicode_GET_LENGTH(conv->v.Name.id) > 1 ||
            !(first == 's' || first == 'r' || first == 'a')) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(conv,
                                            "%c-string: invalid conversion character %R: expected 's', 'r', or 'a'",
                                            formatted_string_prefix(p),
                                            conv->v.Name.id);
        પાછા ખાલી;
    }

    પાછા result_token_with_metadata(p, conv, conv_token->metadata);
}

ResultTokenWithMetadata *
_PyPegen_setup_full_format_spec(Parser *p, Token *colon, asdl_પદાવલિ_seq *spec, પૂર્ણાંક lineno, પૂર્ણાંક col_offset,
                                પૂર્ણાંક end_lineno, પૂર્ણાંક end_col_offset, PyArena *arena)
{
    જો (!spec) {
        પાછા ખાલી;
    }

    // This is needed to keep compatibility with 3.11, where an empty format
    // spec is parsed as an *empty* JoinedStr node, instead of having an empty
    // constant in it.
    Py_ssize_t n_વસ્તુઓ = asdl_seq_LEN(spec);
    Py_ssize_t non_empty_ગણતરી = 0;
    માટે (Py_ssize_t i = 0; i < n_વસ્તુઓ; i++) {
        પદાવલિ_ty item = asdl_seq_GET(spec, i);
        non_empty_ગણતરી += !(item->kind == Constant_kind &&
                             PyUnicode_CheckExact(item->v.Constant.કિંમત) &&
                             PyUnicode_GET_LENGTH(item->v.Constant.કિંમત) == 0);
    }
    જો (non_empty_ગણતરી != n_વસ્તુઓ) {
        asdl_પદાવલિ_seq *resized_spec =
            Py_asdl_પદાવલિ_seq_new(non_empty_ગણતરી, p->arena);
        જો (resized_spec == ખાલી) {
            પાછા ખાલી;
        }
        Py_ssize_t j = 0;
        માટે (Py_ssize_t i = 0; i < n_વસ્તુઓ; i++) {
            પદાવલિ_ty item = asdl_seq_GET(spec, i);
            જો (item->kind == Constant_kind &&
                PyUnicode_CheckExact(item->v.Constant.કિંમત) &&
                PyUnicode_GET_LENGTH(item->v.Constant.કિંમત) == 0) {
                ચાલુ;
            }
            asdl_seq_SET(resized_spec, j++, item);
        }
        પ્રતિજ્ઞા(j == non_empty_ગણતરી);
        spec = resized_spec;
    }
    પદાવલિ_ty res;
    Py_ssize_t n = asdl_seq_LEN(spec);
    જો (n == 0 || (n == 1 && asdl_seq_GET(spec, 0)->kind == Constant_kind)) {
        res = _PyAST_JoinedStr(spec, lineno, col_offset, end_lineno,
                                    end_col_offset, p->arena);
    } નહીંતર {
        res = _PyPegen_concatenate_strings(p, spec,
                             lineno, col_offset, end_lineno,
                             end_col_offset, arena);
    }
    જો (!res) {
        પાછા ખાલી;
    }
    પાછા result_token_with_metadata(p, res, colon->metadata);
}

અચલ અક્ષર *
Py_Pegen_get_પદાવલિ_name(પદાવલિ_ty e)
{
    પ્રતિજ્ઞા(e != ખાલી);
    પસંદગી (e->kind) {
        સ્થિતિ Attribute_kind:
            પાછા "attribute";
        સ્થિતિ Subscript_kind:
            પાછા "subscript";
        સ્થિતિ Starred_kind:
            પાછા "starred";
        સ્થિતિ Name_kind:
            પાછા "name";
        સ્થિતિ List_kind:
            પાછા "list";
        સ્થિતિ Tuple_kind:
            પાછા "tuple";
        સ્થિતિ Lambda_kind:
            પાછા "lambda";
        સ્થિતિ Call_kind:
            પાછા "function call";
        સ્થિતિ BoolOp_kind:
        સ્થિતિ BinOp_kind:
        સ્થિતિ UnaryOp_kind:
            પાછા "expression";
        સ્થિતિ GeneratorExp_kind:
            પાછા "generator expression";
        સ્થિતિ Yield_kind:
        સ્થિતિ YieldFrom_kind:
            પાછા "yield expression";
        સ્થિતિ Await_kind:
            પાછા "await expression";
        સ્થિતિ ListComp_kind:
            પાછા "list comprehension";
        સ્થિતિ SetComp_kind:
            પાછા "set comprehension";
        સ્થિતિ DictComp_kind:
            પાછા "dict comprehension";
        સ્થિતિ Dict_kind:
            પાછા "dict literal";
        સ્થિતિ Set_kind:
            પાછા "set display";
        સ્થિતિ JoinedStr_kind:
        સ્થિતિ Formatted_કિંમત_kind:
            પાછા "f-string expression";
        સ્થિતિ TemplateStr_kind:
        સ્થિતિ Interpolation_kind:
            પાછા "t-string expression";
        સ્થિતિ Constant_kind: {
            PyObject *કિંમત = e->v.Constant.કિંમત;
            જો (કિંમત == Py_None) {
                પાછા "None";
            }
            જો (કિંમત == Py_False) {
                પાછા "False";
            }
            જો (કિંમત == Py_True) {
                પાછા "True";
            }
            જો (કિંમત == Py_Ellipsis) {
                પાછા "ellipsis";
            }
            પાછા "literal";
        }
        સ્થિતિ Compare_kind:
            પાછા "comparison";
        સ્થિતિ IfExp_kind:
            પાછા "conditional expression";
        સ્થિતિ Named_પદાવલિ_kind:
            પાછા "named expression";
        મૂળભૂત:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->kind, e->lineno);
            પાછા ખાલી;
    }
}

પદાવલિ_ty
_PyPegen_get_last_comprehension_item(comprehension_ty comprehension) {
    જો (comprehension->ifs == ખાલી || asdl_seq_LEN(comprehension->ifs) == 0) {
        પાછા comprehension->iter;
    }
    પાછા PyPegen_last_item(comprehension->ifs, પદાવલિ_ty);
}

પદાવલિ_ty _PyPegen_collect_call_seqs(Parser *p, asdl_પદાવલિ_seq *a, asdl_seq *b,
                     પૂર્ણાંક lineno, પૂર્ણાંક col_offset, પૂર્ણાંક end_lineno,
                     પૂર્ણાંક end_col_offset, PyArena *arena) {
    Py_ssize_t args_len = asdl_seq_LEN(a);
    Py_ssize_t total_len = args_len;

    જો (b == ખાલી) {
        પાછા _PyAST_Call(_PyPegen_dummy_name(p), a, ખાલી, lineno, col_offset,
                        end_lineno, end_col_offset, arena);

    }

    asdl_પદાવલિ_seq *starreds = _PyPegen_seq_extract_starred_exprs(p, b);
    જો (!starreds && PyErr_Occurred()) {
        પાછા ખાલી;
    }

    asdl_keyword_seq *keywords = _PyPegen_seq_delete_starred_exprs(p, b);
    જો (!keywords && PyErr_Occurred()) {
        પાછા ખાલી;
    }

    જો (starreds) {
        total_len += asdl_seq_LEN(starreds);
    }

    asdl_પદાવલિ_seq *args = Py_asdl_પદાવલિ_seq_new(total_len, arena);
    જો (args == ખાલી) {
        પાછા ખાલી;
    }

    Py_ssize_t i = 0;
    માટે (i = 0; i < args_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(a, i));
    }
    માટે (; i < total_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(starreds, i - args_len));
    }

    પાછા _PyAST_Call(_PyPegen_dummy_name(p), args, keywords, lineno,
                       col_offset, end_lineno, end_col_offset, arena);
}

// AST Error reporting helpers

પદાવલિ_ty
_PyPegen_get_invalid_target(પદાવલિ_ty e, TARGETS_TYPE targets_type)
{
    જો (e == ખાલી) {
        પાછા ખાલી;
    }

    જો (targets_type == SINGLE_TARGETS ||
        targets_type == ATTRIBUTE_OR_SUBSCRIPT_TARGETS) {
        જો (targets_type == SINGLE_TARGETS && e->kind == Name_kind) {
            પાછા ખાલી;
        }
        પાછા e->kind == Attribute_kind || e->kind == Subscript_kind ? ખાલી : e;
    }

#define VISIT_CONTAINER(CONTAINER, TYPE) કરો { \
        Py_ssize_t len = asdl_seq_LEN((CONTAINER)->v.TYPE.elts);\
        માટે (Py_ssize_t i = 0; i < len; i++) {\
            પદાવલિ_ty other = asdl_seq_GET((CONTAINER)->v.TYPE.elts, i);\
            પદાવલિ_ty child = _PyPegen_get_invalid_target(other, targets_type);\
            જો (child != ખાલી) {\
                પાછા child;\
            }\
        }\
    } જ્યારે (0)

    // We only need to visit List and Tuple nodes recursively as those
    // are the only ones that can contain valid names in targets when
    // they are parsed as expressions. Any other kind of expression
    // that is a container (like Sets or Dicts) is directly invalid and
    // we don't need to visit it recursively.

    પસંદગી (e->kind) {
        સ્થિતિ List_kind:
            VISIT_CONTAINER(e, List);
            પાછા ખાલી;
        સ્થિતિ Tuple_kind:
            VISIT_CONTAINER(e, Tuple);
            પાછા ખાલી;
        સ્થિતિ Starred_kind:
            જો (targets_type == DEL_TARGETS) {
                પાછા e;
            }
            પાછા _PyPegen_get_invalid_target(e->v.Starred.કિંમત, targets_type);
        સ્થિતિ Compare_kind:
            // This is needed, because the `a in b` in `for a in b` gets parsed
            // as a comparison, and so we need to search the left side of the comparison
            // for invalid targets.
            જો (targets_type == FOR_TARGETS) {
                cmpop_ty cmpop = (cmpop_ty) asdl_seq_GET(e->v.Compare.ops, 0);
                જો (cmpop == In) {
                    પાછા _PyPegen_get_invalid_target(e->v.Compare.ડાબું, targets_type);
                }
                પાછા ખાલી;
            }
            પાછા e;
        સ્થિતિ Name_kind:
        સ્થિતિ Subscript_kind:
        સ્થિતિ Attribute_kind:
            પાછા ખાલી;
        મૂળભૂત:
            પાછા e;
    }
}

શૂન્ય *_PyPegen_arguments_parsing_error(Parser *p, પદાવલિ_ty e) {
    પૂર્ણાંક kwarg_unpacking = 0;
    માટે (Py_ssize_t i = 0, l = asdl_seq_LEN(e->v.Call.keywords); i < l; i++) {
        keyword_ty keyword = asdl_seq_GET(e->v.Call.keywords, i);
        જો (!keyword->arg) {
            kwarg_unpacking = 1;
        }
    }

    અચલ અક્ષર *સંદેશ = ખાલી;
    જો (kwarg_unpacking) {
        સંદેશ = "positional argument follows keyword argument unpacking";
    } નહીંતર {
        સંદેશ = "positional argument follows keyword argument";
    }

    પાછા RAISE_SYNTAX_ERROR(સંદેશ);
}

શૂન્ય *
_PyPegen_nonparen_genexp_in_call(Parser *p, પદાવલિ_ty args, asdl_comprehension_seq *comprehensions)
{
    /* The rule that calls this function is 'args for_if_clauses'.
       For the input f(L, x for x in y), L and x are in args and
       the for is parsed as a for_if_clause. We have to check if
       len <= 1, so that input like dict((a, b) for a, b in x)
       gets successfully parsed and then we pass the last
       argument (x in the above example) as the location of the
       error */
    Py_ssize_t len = asdl_seq_LEN(args->v.Call.args);
    જો (len <= 1) {
        પાછા ખાલી;
    }

    comprehension_ty last_comprehension = PyPegen_last_item(comprehensions, comprehension_ty);

    પાછા RAISE_SYNTAX_ERROR_KNOWN_RANGE(
        (પદાવલિ_ty) asdl_seq_GET(args->v.Call.args, len - 1),
        _PyPegen_get_last_comprehension_item(last_comprehension),
        "Generator expression must be parenthesized"
    );
}

// Fstring stuff

સ્થિર પદાવલિ_ty
_PyPegen_decode_fstring_part(Parser* p, પૂર્ણાંક is_raw, પદાવલિ_ty constant, Token* token) {
    પ્રતિજ્ઞા(PyUnicode_CheckExact(constant->v.Constant.કિંમત));

    અચલ અક્ષર* bstr = PyUnicode_AsUTF8(constant->v.Constant.કિંમત);
    જો (bstr == ખાલી) {
        પાછા ખાલી;
    }

    size_t len;
    જો (strcmp(bstr, "{{") == 0 || strcmp(bstr, "}}") == 0) {
        len = 1;
    } નહીંતર {
        len = strlen(bstr);
    }

    is_raw = is_raw || strchr(bstr, '\\') == ખાલી;
    PyObject *str = _PyPegen_decode_string(p, is_raw, bstr, len, token);
    જો (str == ખાલી) {
        _Pypegen_raise_decode_error(p);
        પાછા ખાલી;
    }
    જો (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        પાછા ખાલી;
    }
    પાછા _PyAST_Constant(str, ખાલી, constant->lineno, constant->col_offset,
                           constant->end_lineno, constant->end_col_offset,
                           p->arena);
}

સ્થિર asdl_પદાવલિ_seq *
_get_resized_exprs(Parser *p, Token *a, asdl_પદાવલિ_seq *raw_expressions,
                   Token *b, પૂર્ણાંક is_tstring)
{
    Py_ssize_t n_વસ્તુઓ = asdl_seq_LEN(raw_expressions);
    Py_ssize_t total_વસ્તુઓ = n_વસ્તુઓ;
    માટે (Py_ssize_t i = 0; i < n_વસ્તુઓ; i++) {
        પદાવલિ_ty item = asdl_seq_GET(raw_expressions, i);
        જો (item->kind == JoinedStr_kind) {
            total_વસ્તુઓ += asdl_seq_LEN(item->v.JoinedStr.કિંમતો) - 1;
        }
    }

    અચલ અક્ષર* quote_str = PyBytes_AsString(a->bytes);
    જો (quote_str == ખાલી) {
        પાછા ખાલી;
    }
    પૂર્ણાંક is_raw = strpbrk(quote_str, "rR") != ખાલી;

    asdl_પદાવલિ_seq *seq = Py_asdl_પદાવલિ_seq_new(total_વસ્તુઓ, p->arena);
    જો (seq == ખાલી) {
        પાછા ખાલી;
    }

    Py_ssize_t index = 0;
    માટે (Py_ssize_t i = 0; i < n_વસ્તુઓ; i++) {
        પદાવલિ_ty item = asdl_seq_GET(raw_expressions, i);

        /* Debug expressions arrive as JoinedStr(text, value); flatten them
           into the surrounding string. */
        જો (item->kind == JoinedStr_kind) {
            asdl_પદાવલિ_seq *કિંમતો = item->v.JoinedStr.કિંમતો;
            જો (asdl_seq_LEN(કિંમતો) != 2) {
                PyErr_Format(PyExc_SystemError,
                             is_tstring
                             ? "unexpected TemplateStr node without debug data in t-string at line %d"
                             : "unexpected JoinedStr node without debug data in f-string at line %d",
                             item->lineno);
                પાછા ખાલી;
            }

            પદાવલિ_ty first = asdl_seq_GET(કિંમતો, 0);
            પ્રતિજ્ઞા(first->kind == Constant_kind);
            asdl_seq_SET(seq, index++, first);

            પદાવલિ_ty second = asdl_seq_GET(કિંમતો, 1);
            પ્રતિજ્ઞા((is_tstring &&
                    second->kind == Interpolation_kind) ||
                   second->kind == Formatted_કિંમત_kind);
            asdl_seq_SET(seq, index++, second);

            ચાલુ;
        }

        જો (item->kind == Constant_kind) {
            item = _PyPegen_decode_fstring_part(p, is_raw, item, b);
            જો (item == ખાલી) {
                પાછા ખાલી;
            }

            /* Tokenizer emits string parts even when the underlying string
            might become an empty value (e.g. FSTRING_MIDDLE with the value \\n)
            so we need to check for them and simplify it here. */
            જો (PyUnicode_CheckExact(item->v.Constant.કિંમત)
                && PyUnicode_GET_LENGTH(item->v.Constant.કિંમત) == 0) {
                ચાલુ;
            }
        }
        asdl_seq_SET(seq, index++, item);
    }

    asdl_પદાવલિ_seq *resized_exprs;
    જો (index != total_વસ્તુઓ) {
        resized_exprs = Py_asdl_પદાવલિ_seq_new(index, p->arena);
        જો (resized_exprs == ખાલી) {
            પાછા ખાલી;
        }
        માટે (Py_ssize_t i = 0; i < index; i++) {
            asdl_seq_SET(resized_exprs, i, asdl_seq_GET(seq, i));
        }
    }
    નહીંતર {
        resized_exprs = seq;
    }
    પાછા resized_exprs;
}

પદાવલિ_ty
_PyPegen_template_str(Parser *p, Token *a, asdl_પદાવલિ_seq *raw_expressions, Token *b) {

    asdl_પદાવલિ_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 1);
    જો (resized_exprs == ખાલી) {
        પાછા ખાલી;
    }
    પાછા _PyAST_TemplateStr(resized_exprs, a->lineno, a->col_offset,
                              b->end_lineno, b->end_col_offset,
                              p->arena);
}

પદાવલિ_ty
_PyPegen_joined_str(Parser *p, Token* a, asdl_પદાવલિ_seq* raw_expressions, Token*b) {

    asdl_પદાવલિ_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 0);
    જો (resized_exprs == ખાલી) {
        પાછા ખાલી;
    }
    પાછા _PyAST_JoinedStr(resized_exprs, a->lineno, a->col_offset,
                            b->end_lineno, b->end_col_offset,
                            p->arena);
}

પદાવલિ_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok) {
    જો (tok->bytes == ખાલી) {
        પાછા Py_Pegen_raise_incomplete_આગત(p);
    }
    Py_ssize_t bsize = PyBytes_GET_SIZE(tok->bytes);
    અચલ અક્ષર *bstr = PyBytes_AS_STRING(tok->bytes);

    પૂર્ણાંક is_raw = tok->is_raw;

    PyObject* str = _PyPegen_decode_string(p, is_raw, bstr, bsize, tok);
    જો (str == ખાલી) {
        પાછા ખાલી;
    }
    જો (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        પાછા ખાલી;
    }
    પાછા _PyAST_Constant(str, ખાલી, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

પદાવલિ_ty _PyPegen_constant_from_token(Parser* p, Token* tok) {
    જો (tok->bytes == ખાલી) {
        પાછા Py_Pegen_raise_incomplete_આગત(p);
    }
    અચલ અક્ષર *bstr = PyBytes_AS_STRING(tok->bytes);
    PyObject* str = PyUnicode_FromString(bstr);
    જો (str == ખાલી) {
        પાછા ખાલી;
    }
    જો (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        પાછા ખાલી;
    }
    પાછા _PyAST_Constant(str, ખાલી, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

પદાવલિ_ty _PyPegen_constant_from_string(Parser* p, Token* tok) {
    જો (tok->bytes == ખાલી) {
        પાછા Py_Pegen_raise_incomplete_આગત(p);
    }
    અચલ અક્ષર *the_str = PyBytes_AS_STRING(tok->bytes);
    PyObject *s = _PyPegen_parse_string(p, tok);
    જો (s == ખાલી) {
        _Pypegen_raise_decode_error(p);
        પાછા ખાલી;
    }
    જો (_PyArena_AddPyObject(p->arena, s) < 0) {
        Py_DECREF(s);
        પાછા ખાલી;
    }
    PyObject *kind = ખાલી;
    જો (the_str && the_str[0] == 'u') {
        kind = _PyPegen_new_identifier(p, "u");
        જો (kind == ખાલી) {
            પાછા ખાલી;
        }
    }
    પાછા _PyAST_Constant(s, kind, tok->lineno, tok->col_offset, tok->end_lineno, tok->end_col_offset, p->arena);
}

સ્થિર પૂર્ણાંક
_get_interpolation_conversion(Parser *p, Token *debug, ResultTokenWithMetadata *conversion,
                              ResultTokenWithMetadata *format)
{
    જો (conversion != ખાલી) {
        પદાવલિ_ty conversion_પદાવલિ = (પદાવલિ_ty) conversion->result;
        પ્રતિજ્ઞા(conversion_પદાવલિ->kind == Name_kind);
        Py_UCS4 first = PyUnicode_READ_CHAR(conversion_પદાવલિ->v.Name.id, 0);
        પાછા Py_SAFE_DOWNCAST(first, Py_UCS4, પૂર્ણાંક);
    }
    નહીંતર જો (debug && !format) {
        /* If no conversion is specified, use !r for debug expressions */
        પાછા (પૂર્ણાંક)'r';
    }
    પાછા -1;
}

સ્થિર PyObject *
strip_interpolation_debug_પદાવલિ(PyObject *exprstr)
{
    Py_ssize_t len = PyUnicode_GET_LENGTH(exprstr);

    /* Discard whitespace and explicit line continuations after the debug "="
       but preserve whitespace before it. */
    જ્યારે (len > 0) {
        પૂર્ણાંક has_newline = 0;
        જ્યારે (len > 0) {
            Py_UCS4 c = PyUnicode_READ_CHAR(exprstr, len - 1);
            જો (!_PyUnicode_IsWhitespace(c)) {
                તોડો;
            }
            જો (c == '\r' || c == '\n') {
                has_newline = 1;
            }
            len--;
        }
        જો (!has_newline || len == 0 ||
            PyUnicode_READ_CHAR(exprstr, len - 1) != '\\')
        {
            તોડો;
        }
        len--;
    }

    /* Preserve unexpected metadata instead of dropping source text. */
    જો (len == 0 || PyUnicode_READ_CHAR(exprstr, len - 1) != '=') {
        પાછા Py_NewRef(exprstr);
    }

    પાછા PyUnicode_Substring(exprstr, 0, len - 1);
}

પદાવલિ_ty _PyPegen_interpolation(Parser *p, પદાવલિ_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, પૂર્ણાંક lineno, પૂર્ણાંક col_offset,
                                 પૂર્ણાંક end_lineno, પૂર્ણાંક end_col_offset, PyArena *arena) {

    પૂર્ણાંક conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    /* Find the non whitespace token after the "=" */
    પૂર્ણાંક debug_end_લીટી, debug_end_offset;
    PyObject *debug_metadata;
    constant exprstr;

    જો (conversion) {
        debug_end_લીટી = ((પદાવલિ_ty) conversion->result)->lineno;
        debug_end_offset = ((પદાવલિ_ty) conversion->result)->col_offset;
        debug_metadata = exprstr = conversion->metadata;
    }
    નહીંતર જો (format) {
        debug_end_લીટી = ((પદાવલિ_ty) format->result)->lineno;
        debug_end_offset = ((પદાવલિ_ty) format->result)->col_offset + 1;
        debug_metadata = exprstr = format->metadata;
    }
    નહીંતર {
        debug_end_લીટી = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = exprstr = closing_brace->metadata;
    }

    પ્રતિજ્ઞા(exprstr != ખાલી);
    PyObject *final_exprstr = debug
        ? strip_interpolation_debug_પદાવલિ(exprstr)
        : Py_NewRef(exprstr);
    જો (!final_exprstr || _PyArena_AddPyObject(arena, final_exprstr) < 0) {
        Py_XDECREF(final_exprstr);
        પાછા ખાલી;
    }

    પદાવલિ_ty interpolation = _PyAST_Interpolation(
        expression, final_exprstr, conversion_val, format ? (પદાવલિ_ty) format->result : ખાલી,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    જો (!interpolation || !debug) {
        પાછા interpolation;
    }

    પદાવલિ_ty debug_text = _PyAST_Constant(debug_metadata, ખાલી, lineno, col_offset + 1, debug_end_લીટી,
                                            debug_end_offset - 1, p->arena);
    જો (!debug_text) {
        પાછા ખાલી;
    }

    asdl_પદાવલિ_seq *કિંમતો = Py_asdl_પદાવલિ_seq_new(2, arena);
    જો (!કિંમતો) {
        પાછા ખાલી;
    }
    asdl_seq_SET(કિંમતો, 0, debug_text);
    asdl_seq_SET(કિંમતો, 1, interpolation);
    પાછા _PyAST_JoinedStr(કિંમતો, lineno, col_offset, debug_end_લીટી, debug_end_offset, p->arena);
}

પદાવલિ_ty Py_Pegen_formatted_કિંમત(Parser *p, પદાવલિ_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, પૂર્ણાંક lineno, પૂર્ણાંક col_offset,
                                 પૂર્ણાંક end_lineno, પૂર્ણાંક end_col_offset, PyArena *arena) {
    પૂર્ણાંક conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    પદાવલિ_ty formatted_કિંમત = Py_AST_Formatted_કિંમત(
        expression, conversion_val, format ? (પદાવલિ_ty) format->result : ખાલી,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    જો (!formatted_કિંમત || !debug) {
        પાછા formatted_કિંમત;
    }

    /* Find the non whitespace token after the "=" */
    પૂર્ણાંક debug_end_લીટી, debug_end_offset;
    PyObject *debug_metadata;

    જો (conversion) {
        debug_end_લીટી = ((પદાવલિ_ty) conversion->result)->lineno;
        debug_end_offset = ((પદાવલિ_ty) conversion->result)->col_offset;
        debug_metadata = conversion->metadata;
    }
    નહીંતર જો (format) {
        debug_end_લીટી = ((પદાવલિ_ty) format->result)->lineno;
        debug_end_offset = ((પદાવલિ_ty) format->result)->col_offset + 1;
        debug_metadata = format->metadata;
    }
    નહીંતર {
        debug_end_લીટી = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = closing_brace->metadata;
    }
    પદાવલિ_ty debug_text = _PyAST_Constant(debug_metadata, ખાલી, lineno, col_offset + 1, debug_end_લીટી,
                                            debug_end_offset - 1, p->arena);
    જો (!debug_text) {
        પાછા ખાલી;
    }

    asdl_પદાવલિ_seq *કિંમતો = Py_asdl_પદાવલિ_seq_new(2, arena);
    જો (!કિંમતો) {
        પાછા ખાલી;
    }
    asdl_seq_SET(કિંમતો, 0, debug_text);
    asdl_seq_SET(કિંમતો, 1, formatted_કિંમત);
    પાછા _PyAST_JoinedStr(કિંમતો, lineno, col_offset, debug_end_લીટી, debug_end_offset, p->arena);
}

સ્થિર પદાવલિ_ty
_build_concatenated_bytes(Parser *p, asdl_પદાવલિ_seq *strings, પૂર્ણાંક lineno,
                        પૂર્ણાંક col_offset, પૂર્ણાંક end_lineno, પૂર્ણાંક end_col_offset,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    પ્રતિજ્ઞા(len > 0);

    /* Bytes literals never get a kind, but just for consistency
        since they are represented as Constant nodes, we'll mirror
        the same behavior as unicode strings for determining the
        kind. */
    PyObject *kind = asdl_seq_GET(strings, 0)->v.Constant.kind;

    Py_ssize_t total = 0;
    માટે (Py_ssize_t i = 0; i < len; i++) {
        પદાવલિ_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.કિંમત;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        જો (part > PY_SSIZE_T_MAX - total) {
            PyErr_NoMemory();
            પાછા ખાલી;
        }
        total += part;
    }

    PyBytesWriter *writer = PyBytesWriter_Create(total);
    જો (writer == ખાલી) {
        પાછા ખાલી;
    }
    અક્ષર *out = PyBytesWriter_GetData(writer);

    માટે (Py_ssize_t i = 0; i < len; i++) {
        પદાવલિ_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.કિંમત;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        જો (part > 0) {
            memcpy(out, PyBytes_AS_STRING(bytes), part);
            out += part;
        }
    }

    PyObject *res = PyBytesWriter_Finish(writer);
    જો (res == ખાલી) {
        પાછા ખાલી;
    }
    જો (_PyArena_AddPyObject(arena, res) < 0) {
        Py_DECREF(res);
        પાછા ખાલી;
    }
    પાછા _PyAST_Constant(res, kind, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

સ્થિર પદાવલિ_ty
_build_concatenated_unicode(Parser *p, asdl_પદાવલિ_seq *strings, પૂર્ણાંક lineno,
                        પૂર્ણાંક col_offset, પૂર્ણાંક end_lineno, પૂર્ણાંક end_col_offset,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    પ્રતિજ્ઞા(len > 1);

    પદાવલિ_ty first = asdl_seq_GET(strings, 0);

    /* When a string is getting concatenated, the kind of the string
        is determined by the first string in the concatenation
        sequence.

        u"abc" "def" -> u"abcdef"
        "abc" u"abc" ->  "abcabc" */
    PyObject *kind = first->v.Constant.kind;

    PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
    જો (writer == ખાલી) {
        પાછા ખાલી;
    }

    માટે (Py_ssize_t i = 0; i < len; i++) {
        પદાવલિ_ty current_elem = asdl_seq_GET(strings, i);
        પ્રતિજ્ઞા(current_elem->kind == Constant_kind);

        જો (PyUnicodeWriter_WriteStr(writer,
                                     current_elem->v.Constant.કિંમત)) {
            PyUnicodeWriter_Discard(writer);
            પાછા ખાલી;
        }
    }

    PyObject *final = PyUnicodeWriter_Finish(writer);
    જો (final == ખાલી) {
        પાછા ખાલી;
    }
    જો (_PyArena_AddPyObject(p->arena, final) < 0) {
        Py_DECREF(final);
        પાછા ખાલી;
    }
    પાછા _PyAST_Constant(final, kind, lineno, col_offset,
                           end_lineno, end_col_offset, arena);
}

સ્થિર asdl_પદાવલિ_seq *
_build_concatenated_str(Parser *p, asdl_પદાવલિ_seq *strings,
                               પૂર્ણાંક lineno, પૂર્ણાંક col_offset, પૂર્ણાંક end_lineno,
                               પૂર્ણાંક end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    પ્રતિજ્ઞા(len > 0);

    Py_ssize_t n_flattened_elements = 0;
    માટે (Py_ssize_t i = 0; i < len; i++) {
        પદાવલિ_ty elem = asdl_seq_GET(strings, i);
        પસંદગી(elem->kind) {
            સ્થિતિ JoinedStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.JoinedStr.કિંમતો);
                તોડો;
            સ્થિતિ TemplateStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.TemplateStr.કિંમતો);
                તોડો;
            મૂળભૂત:
                n_flattened_elements++;
                તોડો;
        }
    }


    asdl_પદાવલિ_seq* flattened = Py_asdl_પદાવલિ_seq_new(n_flattened_elements, p->arena);
    જો (flattened == ખાલી) {
        પાછા ખાલી;
    }

    /* build flattened list */
    Py_ssize_t current_pos = 0;
    માટે (Py_ssize_t i = 0; i < len; i++) {
        પદાવલિ_ty elem = asdl_seq_GET(strings, i);
        પસંદગી(elem->kind) {
            સ્થિતિ JoinedStr_kind:
                માટે (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.JoinedStr.કિંમતો); j++) {
                    પદાવલિ_ty subvalue = asdl_seq_GET(elem->v.JoinedStr.કિંમતો, j);
                    જો (subvalue == ખાલી) {
                        પાછા ખાલી;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                તોડો;
            સ્થિતિ TemplateStr_kind:
                માટે (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.TemplateStr.કિંમતો); j++) {
                    પદાવલિ_ty subvalue = asdl_seq_GET(elem->v.TemplateStr.કિંમતો, j);
                    જો (subvalue == ખાલી) {
                        પાછા ખાલી;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                તોડો;
            મૂળભૂત:
                asdl_seq_SET(flattened, current_pos++, elem);
                તોડો;
        }
    }

    /* calculate folded element count */
    Py_ssize_t n_elements = 0;
    પૂર્ણાંક prev_is_constant = 0;
    માટે (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        પદાવલિ_ty elem = asdl_seq_GET(flattened, i);

        /* The concatenation of a FormattedValue and an empty Constant should
           lead to the FormattedValue itself. Thus, we will not take any empty
           constants into account, just as in `_PyPegen_joined_str` */
        જો (elem->kind == Constant_kind &&
            PyUnicode_CheckExact(elem->v.Constant.કિંમત) &&
            PyUnicode_GET_LENGTH(elem->v.Constant.કિંમત) == 0)
            ચાલુ;

        જો (!prev_is_constant || elem->kind != Constant_kind) {
            n_elements++;
        }
        prev_is_constant = elem->kind == Constant_kind;
    }

    asdl_પદાવલિ_seq* કિંમતો = Py_asdl_પદાવલિ_seq_new(n_elements, p->arena);
    જો (કિંમતો == ખાલી) {
        પાછા ખાલી;
    }

    /* build folded list */
    current_pos = 0;
    માટે (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        પદાવલિ_ty elem = asdl_seq_GET(flattened, i);

        /* if the current elem and the following are constants,
           fold them and all consequent constants */
        જો (elem->kind == Constant_kind) {
            જો (i + 1 < n_flattened_elements &&
                asdl_seq_GET(flattened, i + 1)->kind == Constant_kind) {
                પદાવલિ_ty first_elem = elem;

                /* When a string is getting concatenated, the kind of the string
                   is determined by the first string in the concatenation
                   sequence.

                   u"abc" "def" -> u"abcdef"
                   "abc" u"abc" ->  "abcabc" */
                PyObject *kind = elem->v.Constant.kind;

                PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
                જો (writer == ખાલી) {
                    પાછા ખાલી;
                }
                પદાવલિ_ty last_elem = elem;
                Py_ssize_t j;
                માટે (j = i; j < n_flattened_elements; j++) {
                    પદાવલિ_ty current_elem = asdl_seq_GET(flattened, j);
                    જો (current_elem->kind == Constant_kind) {
                        જો (PyUnicodeWriter_WriteStr(writer,
                                                     current_elem->v.Constant.કિંમત)) {
                            PyUnicodeWriter_Discard(writer);
                            પાછા ખાલી;
                        }
                        last_elem = current_elem;
                    } નહીંતર {
                        તોડો;
                    }
                }
                i = j - 1;

                PyObject *concat_str = PyUnicodeWriter_Finish(writer);
                જો (concat_str == ખાલી) {
                    પાછા ખાલી;
                }
                જો (_PyArena_AddPyObject(p->arena, concat_str) < 0) {
                    Py_DECREF(concat_str);
                    પાછા ખાલી;
                }
                elem = _PyAST_Constant(concat_str, kind, first_elem->lineno,
                                       first_elem->col_offset,
                                       last_elem->end_lineno,
                                       last_elem->end_col_offset, p->arena);
                જો (elem == ખાલી) {
                    પાછા ખાલી;
                }
            }

            /* Drop all empty contanst strings */
            જો (PyUnicode_CheckExact(elem->v.Constant.કિંમત) &&
                PyUnicode_GET_LENGTH(elem->v.Constant.કિંમત) == 0) {
                ચાલુ;
            }
        }

        asdl_seq_SET(કિંમતો, current_pos++, elem);
    }

    પ્રતિજ્ઞા(current_pos == n_elements);
    પાછા કિંમતો;
}

સ્થિર પદાવલિ_ty
_build_concatenated_joined_str(Parser *p, asdl_પદાવલિ_seq *strings,
                               પૂર્ણાંક lineno, પૂર્ણાંક col_offset, પૂર્ણાંક end_lineno,
                               પૂર્ણાંક end_col_offset, PyArena *arena)
{
    asdl_પદાવલિ_seq *કિંમતો = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    જો (!કિંમતો) {
        પાછા ખાલી;
    }
    પાછા _PyAST_JoinedStr(કિંમતો, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

પદાવલિ_ty
_PyPegen_concatenate_tstrings(Parser *p, asdl_પદાવલિ_seq *strings,
                               પૂર્ણાંક lineno, પૂર્ણાંક col_offset, પૂર્ણાંક end_lineno,
                               પૂર્ણાંક end_col_offset, PyArena *arena)
{
    asdl_પદાવલિ_seq *કિંમતો = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    જો (!કિંમતો) {
        પાછા ખાલી;
    }
    પાછા _PyAST_TemplateStr(કિંમતો, lineno, col_offset, end_lineno,
        end_col_offset, arena);
}

પદાવલિ_ty
_PyPegen_concatenate_strings(Parser *p, asdl_પદાવલિ_seq *strings,
                             પૂર્ણાંક lineno, પૂર્ણાંક col_offset, પૂર્ણાંક end_lineno,
                             પૂર્ણાંક end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    પ્રતિજ્ઞા(len > 0);

    પૂર્ણાંક f_string_found = 0;
    પૂર્ણાંક unicode_string_found = 0;
    પૂર્ણાંક bytes_found = 0;

    Py_ssize_t i = 0;
    માટે (i = 0; i < len; i++) {
        પદાવલિ_ty elem = asdl_seq_GET(strings, i);
        પસંદગી(elem->kind) {
            સ્થિતિ Constant_kind:
                જો (PyBytes_CheckExact(elem->v.Constant.કિંમત)) {
                    bytes_found = 1;
                } નહીંતર {
                    unicode_string_found = 1;
                }
                તોડો;
            સ્થિતિ JoinedStr_kind:
                f_string_found = 1;
                તોડો;
            સ્થિતિ TemplateStr_kind:
                // python.gram handles this; we should never get here
                પ્રતિજ્ઞા(0);
                તોડો;
            મૂળભૂત:
                f_string_found = 1;
                તોડો;
        }
    }

    // Cannot mix unicode and bytes
    જો ((unicode_string_found || f_string_found) && bytes_found) {
        RAISE_SYNTAX_ERROR("cannot mix bytes and nonbytes literals");
        પાછા ખાલી;
    }

    // If it's only bytes or only unicode string, do a simple concat
    જો (!f_string_found) {
        જો (len == 1) {
            પાછા asdl_seq_GET(strings, 0);
        }
        નહીંતર જો (bytes_found) {
            પાછા _build_concatenated_bytes(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
        નહીંતર {
            પાછા _build_concatenated_unicode(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
    }

    પાછા _build_concatenated_joined_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
}

સ્થિર પૂર્ણાંક
_warn_relative_import_of_lazy(Parser *p, asdl_seq *dots, પદાવલિ_ty module)
{
    // Warn about `from . lazy import x`: the whitespace between the dots and
    // the module name is insignificant, so this is parsed exactly like
    // `from .lazy import x` (an import of the relative module "lazy"), but it
    // is most likely a transposition of `lazy from . import x` (PEP 810).
    જો (p->call_invalid_rules) {
        પાછા 0;
    }

    // Only fire if there is whitespace between the last dot and the name,
    // i.e. not for the common `from .lazy import x` spelling.
    Token *last_dot = asdl_seq_GET_UNTYPED(dots, asdl_seq_LEN(dots) - 1);
    જો (
        last_dot->end_lineno == module->lineno
        && last_dot->end_col_offset == module->col_offset
    ) {
        પાછા 0;
    }

    પૂર્ણાંક ગણતરી = Py_Pegen_seq_ગણતરી_dots(dots);
    અક્ષર *buf = PyMem_RawMalloc(ગણતરી + 1);
    જો (buf == ખાલી) {
        PyErr_NoMemory();
        પાછા -1;
    }
    memset(buf, '.', ગણતરી);
    buf[ગણતરી] = '\0';

    PyObject *સંદેશ = PyUnicode_FromFormat(
        "'from %s lazy import' is the same as 'from %slazy import'; "
        "did you mean 'lazy from %s import'?",
        buf, buf, buf);
    PyMem_RawFree(buf);
    જો (સંદેશ == ખાલી) {
        પાછા -1;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    પૂર્ણાંક res = _PyErr_EmitSyntaxWarning(સંદેશ,
                                       info.filename,
                                       module->lineno,
                                       module->col_offset + 1,
                                       module->end_lineno,
                                       module->end_col_offset + 1,
                                       info.module);
    Py_DECREF(સંદેશ);
    પાછા res;
}

stmt_ty
_PyPegen_checked_from_import(Parser *p, asdl_seq *dots, પદાવલિ_ty module_name,
                             asdl_alias_seq *names, પદાવલિ_ty lazy_token, પૂર્ણાંક lineno,
                             પૂર્ણાંક col_offset, પૂર્ણાંક end_lineno, પૂર્ણાંક end_col_offset,
                             PyArena *arena)
{
    identifier module = module_name->v.Name.id;
    પૂર્ણાંક level = Py_Pegen_seq_ગણતરી_dots(dots);
    જો (level == 0 && PyUnicode_CompareWithASCIIString(module, "__future__") == 0) {
        જો (lazy_token) {
            RAISE_SYNTAX_ERROR_KNOWN_LOCATION(lazy_token,
                "lazy from __future__ import is not allowed");
            પાછા ખાલી;
        }
        માટે (Py_ssize_t i = 0; i < asdl_seq_LEN(names); i++) {
            alias_ty alias = asdl_seq_GET(names, i);
            જો (PyUnicode_CompareWithASCIIString(alias->name, "barry_as_FLUFL") == 0) {
                p->flags |= PyPARSE_BARRY_AS_BDFL;
                p->tok->barry_as_bdfl = 1;
            }
        }
    }
    નહીંતર જો (
        level > 0
        && lazy_token == ખાલી
        && PyUnicode_CompareWithASCIIString(module, "lazy") == 0
    ) {
        જો (_warn_relative_import_of_lazy(p, dots, module_name) < 0) {
            પાછા ખાલી;
        }
    }
    પાછા _PyAST_ImportFrom(module, names, level, lazy_token ? 1 : 0, lineno,
                             col_offset, end_lineno, end_col_offset, arena);
}

asdl_stmt_seq*
_PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts) {
    જો (!p->call_invalid_rules) {
        પાછા stmts;
    }
    Py_ssize_t len = asdl_seq_LEN(stmts);
    જો (len == 0) {
        પાછા stmts;
    }
    stmt_ty last_stmt = asdl_seq_GET(stmts, len - 1);
    જો (p->last_stmt_location.lineno > last_stmt->lineno) {
        પાછા stmts;
    }
    p->last_stmt_location.lineno = last_stmt->lineno;
    p->last_stmt_location.col_offset = last_stmt->col_offset;
    p->last_stmt_location.end_lineno = last_stmt->end_lineno;
    p->last_stmt_location.end_col_offset = last_stmt->end_col_offset;
    પાછા stmts;
}
