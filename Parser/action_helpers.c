#ଅନ୍ତର୍ଭାଵ <Python.h>
#ଅନ୍ତର୍ଭାଵ "pycore_pystate.h"         // _PyInterpreterState_GET()
#ଅନ୍ତର୍ଭାଵ "pycore_runtime.h"         // _PyRuntime
#ଅନ୍ତର୍ଭାଵ "pycore_unicodeobject.h"   // _PyUnicode_InternImmortal()
#ଅନ୍ତର୍ଭାଵ "pycore_pyerrors.h"        // _PyErr_EmitSyntaxWarning()

#ଅନ୍ତର୍ଭାଵ "pegen.h"
#ଅନ୍ତର୍ଭାଵ "string_parser.h"          // _PyPegen_decode_string()
#ଅନ୍ତର୍ଭାଵ "lexer/state.h"            // tok_state


ଶୂନ୍ୟ *
_PyPegen_dummy_name(Parser *p, ...)
{
    ଫେରନ୍ତୁ &_PyRuntime.parser.dummy_name;
}

/* Creates a single-element asdl_seq* that contains a */
asdl_seq *
_PyPegen_singleton_seq(Parser *p, ଶୂନ୍ୟ *a)
{
    ପ୍ରତିଜ୍ଞା(a != ଖାଲି);
    asdl_seq *seq = (asdl_seq*)_Py_asdl_generic_seq_new(1, p->arena);
    ଯଦି (!seq) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    asdl_seq_SET_UNTYPED(seq, 0, a);
    ଫେରନ୍ତୁ seq;
}

/* Creates a copy of seq and prepends a to it */
asdl_seq *
Py_Pegen_seq_ଭର୍ତ୍ତି_in_front(Parser *p, ଶୂନ୍ୟ *a, asdl_seq *seq)
{
    ପ୍ରତିଜ୍ଞା(a != ଖାଲି);
    ଯଦି (!seq) {
        ଫେରନ୍ତୁ _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    ଯଦି (!new_seq) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    asdl_seq_SET_UNTYPED(new_seq, 0, a);
    ପାଇଁ (Py_ssize_t i = 1, l = asdl_seq_LEN(new_seq); i < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i - 1));
    }
    ଫେରନ୍ତୁ new_seq;
}

/* Creates a copy of seq and appends a to it */
asdl_seq *
_PyPegen_seq_append_to_end(Parser *p, asdl_seq *seq, ଶୂନ୍ୟ *a)
{
    ପ୍ରତିଜ୍ଞା(a != ଖାଲି);
    ଯଦି (!seq) {
        ଫେରନ୍ତୁ _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    ଯଦି (!new_seq) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    ପାଇଁ (Py_ssize_t i = 0, l = asdl_seq_LEN(new_seq); i + 1 < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i));
    }
    asdl_seq_SET_UNTYPED(new_seq, asdl_seq_LEN(new_seq) - 1, a);
    ଫେରନ୍ତୁ new_seq;
}

ସ୍ଥିର Py_ssize_t
_get_flattened_seq_size(asdl_seq *seqs)
{
    Py_ssize_t size = 0;
    ପାଇଁ (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        size += asdl_seq_LEN(inner_seq);
    }
    ଫେରନ୍ତୁ size;
}

/* Flattens an asdl_seq* of asdl_seq*s */
asdl_seq *
_PyPegen_seq_flatten(Parser *p, asdl_seq *seqs)
{
    Py_ssize_t flattened_seq_size = _get_flattened_seq_size(seqs);
    ପ୍ରତିଜ୍ଞା(flattened_seq_size > 0);

    asdl_seq *flattened_seq = (asdl_seq*)_Py_asdl_generic_seq_new(flattened_seq_size, p->arena);
    ଯଦି (!flattened_seq) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା flattened_seq_idx = 0;
    ପାଇଁ (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        ପାଇଁ (Py_ssize_t j = 0, li = asdl_seq_LEN(inner_seq); j < li; j++) {
            asdl_seq_SET_UNTYPED(flattened_seq, flattened_seq_idx++, asdl_seq_GET_UNTYPED(inner_seq, j));
        }
    }
    ପ୍ରତିଜ୍ଞା(flattened_seq_idx == flattened_seq_size);

    ଫେରନ୍ତୁ flattened_seq;
}

ଶୂନ୍ୟ *
_PyPegen_seq_last_item(asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ଫେରନ୍ତୁ asdl_seq_GET_UNTYPED(seq, len - 1);
}

ଶୂନ୍ୟ *
_PyPegen_seq_first_item(asdl_seq *seq)
{
    ଫେରନ୍ତୁ asdl_seq_GET_UNTYPED(seq, 0);
}

/* Creates a new name of the form <first_name>.<second_name> */
ପଦସମଷ୍ଟି_ty
_PyPegen_join_names_with_dot(Parser *p, ପଦସମଷ୍ଟି_ty first_name, ପଦସମଷ୍ଟି_ty second_name)
{
    ପ୍ରତିଜ୍ଞା(first_name != ଖାଲି && second_name != ଖାଲି);
    PyObject *uni = PyUnicode_FromFormat("%U.%U",
            first_name->v.Name.id, second_name->v.Name.id);
    ଯଦି (!uni) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &uni);
    ଯଦି (_PyArena_AddPyObject(p->arena, uni) < 0) {
        Py_DECREF(uni);
        ଫେରନ୍ତୁ ଖାଲି;
    }

    ଫେରନ୍ତୁ _PyAST_Name(uni, Load, EXTRA_ପଦସମଷ୍ଟି(first_name, second_name));
}

/* Counts the total number of dots in seq's tokens */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
Py_Pegen_seq_ଗଣନା_dots(asdl_seq *seq)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସଂଖ୍ୟା_of_dots = 0;
    ପାଇଁ (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        Token *current_ପଦସମଷ୍ଟି = asdl_seq_GET_UNTYPED(seq, i);
        ବାଛ (current_ପଦସମଷ୍ଟି->type) {
            ସ୍ଥିତି ELLIPSIS:
                ସଂଖ୍ୟା_of_dots += 3;
                ଭାଙ୍ଗ;
            ସ୍ଥିତି DOT:
                ସଂଖ୍ୟା_of_dots += 1;
                ଭାଙ୍ଗ;
            ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                Py_UNREACHABLE();
        }
    }

    ଫେରନ୍ତୁ ସଂଖ୍ୟା_of_dots;
}

/* Creates an alias with '*' as the identifier name */
alias_ty
_PyPegen_alias_for_star(Parser *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_lineno,
                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col_offset, PyArena *arena) {
    PyObject *str = PyUnicode_InternFromString("*");
    ଯଦି (!str) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ _PyAST_alias(str, ଖାଲି, lineno, col_offset, end_lineno, end_col_offset, arena);
}

/* Creates a new asdl_seq* with the identifiers of all the names in seq */
asdl_identifier_seq *
_PyPegen_map_names_to_ids(Parser *p, asdl_ପଦସମଷ୍ଟି_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ପ୍ରତିଜ୍ଞା(len > 0);

    asdl_identifier_seq *new_seq = _Py_asdl_identifier_seq_new(len, p->arena);
    ଯଦି (!new_seq) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ପାଇଁ (Py_ssize_t i = 0; i < len; i++) {
        ପଦସମଷ୍ଟି_ty e = asdl_seq_GET(seq, i);
        asdl_seq_SET(new_seq, i, e->v.Name.id);
    }
    ଫେରନ୍ତୁ new_seq;
}

/* Constructs a CmpopExprPair */
Cmpop_ପଦସମଷ୍ଟି_Pair *
Py_Pegen_cmpop_ପଦସମଷ୍ଟି_pair(Parser *p, cmpop_ty cmpop, ପଦସମଷ୍ଟି_ty ପଦସମଷ୍ଟି)
{
    ପ୍ରତିଜ୍ଞା(ପଦସମଷ୍ଟି != ଖାଲି);
    Cmpop_ପଦସମଷ୍ଟି_Pair *a = _PyArena_Malloc(p->arena, ଆକାର(Cmpop_ପଦସମଷ୍ଟି_Pair));
    ଯଦି (!a) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    a->cmpop = cmpop;
    a->ପଦସମଷ୍ଟି = ପଦସମଷ୍ଟି;
    ଫେରନ୍ତୁ a;
}

asdl_int_seq *
_PyPegen_get_cmpops(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ପ୍ରତିଜ୍ଞା(len > 0);

    asdl_int_seq *new_seq = _Py_asdl_int_seq_new(len, p->arena);
    ଯଦି (!new_seq) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ପାଇଁ (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_ପଦସମଷ୍ଟି_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->cmpop);
    }
    ଫେରନ୍ତୁ new_seq;
}

asdl_ପଦସମଷ୍ଟି_seq *
_PyPegen_get_exprs(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ପ୍ରତିଜ୍ଞା(len > 0);

    asdl_ପଦସମଷ୍ଟି_seq *new_seq = Py_asdl_ପଦସମଷ୍ଟି_seq_new(len, p->arena);
    ଯଦି (!new_seq) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ପାଇଁ (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_ପଦସମଷ୍ଟି_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->ପଦସମଷ୍ଟି);
    }
    ଫେରନ୍ତୁ new_seq;
}

/* Creates an asdl_seq* where all the elements have been changed to have ctx as context */
ସ୍ଥିର asdl_ପଦସମଷ୍ଟି_seq *
_set_seq_context(Parser *p, asdl_ପଦସମଷ୍ଟି_seq *seq, ପଦସମଷ୍ଟି_context_ty ctx)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ଯଦି (len == 0) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    asdl_ପଦସମଷ୍ଟି_seq *new_seq = Py_asdl_ପଦସମଷ୍ଟି_seq_new(len, p->arena);
    ଯଦି (!new_seq) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ପାଇଁ (Py_ssize_t i = 0; i < len; i++) {
        ପଦସମଷ୍ଟି_ty e = asdl_seq_GET(seq, i);
        ପଦସମଷ୍ଟି_ty new_e = Py_Pegen_set_ପଦସମଷ୍ଟି_context(p, e, ctx);
        ଯଦି (!new_e) {
            ଫେରନ୍ତୁ ଖାଲି;
        }
        asdl_seq_SET(new_seq, i, new_e);
    }
    ଫେରନ୍ତୁ new_seq;
}

ସ୍ଥିର ପଦସମଷ୍ଟି_ty
_set_name_context(Parser *p, ପଦସମଷ୍ଟି_ty e, ପଦସମଷ୍ଟି_context_ty ctx)
{
    ଫେରନ୍ତୁ _PyAST_Name(e->v.Name.id, ctx, EXTRA_ପଦସମଷ୍ଟି(e, e));
}

ସ୍ଥିର ପଦସମଷ୍ଟି_ty
_set_tuple_context(Parser *p, ପଦସମଷ୍ଟି_ty e, ପଦସମଷ୍ଟି_context_ty ctx)
{
    asdl_ପଦସମଷ୍ଟି_seq *seq = _set_seq_context(p, e->v.Tuple.elts, ctx);
    ଯଦି (!seq && PyErr_Occurred()) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ _PyAST_Tuple(seq, ctx, EXTRA_ପଦସମଷ୍ଟି(e, e));
}

ସ୍ଥିର ପଦସମଷ୍ଟି_ty
_set_list_context(Parser *p, ପଦସମଷ୍ଟି_ty e, ପଦସମଷ୍ଟି_context_ty ctx)
{
    asdl_ପଦସମଷ୍ଟି_seq *seq = _set_seq_context(p, e->v.List.elts, ctx);
    ଯଦି (!seq && PyErr_Occurred()) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ _PyAST_List(seq, ctx, EXTRA_ପଦସମଷ୍ଟି(e, e));
}

ସ୍ଥିର ପଦସମଷ୍ଟି_ty
_set_subscript_context(Parser *p, ପଦସମଷ୍ଟି_ty e, ପଦସମଷ୍ଟି_context_ty ctx)
{
    ଫେରନ୍ତୁ _PyAST_Subscript(e->v.Subscript.ମୂଲ୍ୟ, e->v.Subscript.slice,
                            ctx, EXTRA_ପଦସମଷ୍ଟି(e, e));
}

ସ୍ଥିର ପଦସମଷ୍ଟି_ty
_set_attribute_context(Parser *p, ପଦସମଷ୍ଟି_ty e, ପଦସମଷ୍ଟି_context_ty ctx)
{
    ଫେରନ୍ତୁ _PyAST_Attribute(e->v.Attribute.ମୂଲ୍ୟ, e->v.Attribute.attr,
                            ctx, EXTRA_ପଦସମଷ୍ଟି(e, e));
}

ସ୍ଥିର ପଦସମଷ୍ଟି_ty
_set_starred_context(Parser *p, ପଦସମଷ୍ଟି_ty e, ପଦସମଷ୍ଟି_context_ty ctx)
{
    ପଦସମଷ୍ଟି_ty inner = Py_Pegen_set_ପଦସମଷ୍ଟି_context(p, e->v.Starred.ମୂଲ୍ୟ, ctx);
    ଯଦି (!inner) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ _PyAST_Starred(inner, ctx, EXTRA_ପଦସମଷ୍ଟି(e, e));
}

/* Creates an `expr_ty` equivalent to `expr` but with `ctx` as context */
ପଦସମଷ୍ଟି_ty
Py_Pegen_set_ପଦସମଷ୍ଟି_context(Parser *p, ପଦସମଷ୍ଟି_ty ପଦସମଷ୍ଟି, ପଦସମଷ୍ଟି_context_ty ctx)
{
    ପ୍ରତିଜ୍ଞା(ପଦସମଷ୍ଟି != ଖାଲି);

    ପଦସମଷ୍ଟି_ty new = ଖାଲି;
    ବାଛ (ପଦସମଷ୍ଟି->kind) {
        ସ୍ଥିତି Name_kind:
            new = _set_name_context(p, ପଦସମଷ୍ଟି, ctx);
            ଭାଙ୍ଗ;
        ସ୍ଥିତି Tuple_kind:
            new = _set_tuple_context(p, ପଦସମଷ୍ଟି, ctx);
            ଭାଙ୍ଗ;
        ସ୍ଥିତି List_kind:
            new = _set_list_context(p, ପଦସମଷ୍ଟି, ctx);
            ଭାଙ୍ଗ;
        ସ୍ଥିତି Subscript_kind:
            new = _set_subscript_context(p, ପଦସମଷ୍ଟି, ctx);
            ଭାଙ୍ଗ;
        ସ୍ଥିତି Attribute_kind:
            new = _set_attribute_context(p, ପଦସମଷ୍ଟି, ctx);
            ଭାଙ୍ଗ;
        ସ୍ଥିତି Starred_kind:
            new = _set_starred_context(p, ପଦସମଷ୍ଟି, ctx);
            ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            new = ପଦସମଷ୍ଟି;
    }
    ଫେରନ୍ତୁ new;
}

/* Invalid targets return NULL without raising, allowing parsing to backtrack.
   Copy valid targets to preserve the memoized expression's context. */
ପଦସମଷ୍ଟି_ty
_PyPegen_make_target(Parser *p, ପଦସମଷ୍ଟି_ty ପଦସମଷ୍ଟି, TARGETS_TYPE targets_type)
{
    ପ୍ରତିଜ୍ଞା(ପଦସମଷ୍ଟି != ଖାଲି);
    ପ୍ରତିଜ୍ଞା(targets_type != FOR_TARGETS);
    ଯଦି (_PyPegen_get_invalid_target(ପଦସମଷ୍ଟି, targets_type) != ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ Py_Pegen_set_ପଦସମଷ୍ଟି_context(
        p, ପଦସମଷ୍ଟି, targets_type == DEL_TARGETS ? Del : Store);
}

/* Constructs a KeyValuePair that is used when parsing a dict's key value pairs */
ଚାବି_ମୂଲ୍ୟ_Pair *
Py_Pegen_ଚାବି_ମୂଲ୍ୟ_pair(Parser *p, ପଦସମଷ୍ଟି_ty ଚାବି, ପଦସମଷ୍ଟି_ty ମୂଲ୍ୟ)
{
    ଚାବି_ମୂଲ୍ୟ_Pair *a = _PyArena_Malloc(p->arena, ଆକାର(ଚାବି_ମୂଲ୍ୟ_Pair));
    ଯଦି (!a) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    a->ଚାବି = ଚାବି;
    a->ମୂଲ୍ୟ = ମୂଲ୍ୟ;
    ଫେରନ୍ତୁ a;
}

/* Extracts all keys from an asdl_seq* of KeyValuePair*'s */
asdl_ପଦସମଷ୍ଟି_seq *
Py_Pegen_get_ଚାବିଗୁଡ଼ିକ(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_ପଦସମଷ୍ଟି_seq *new_seq = Py_asdl_ପଦସମଷ୍ଟି_seq_new(len, p->arena);
    ଯଦି (!new_seq) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ପାଇଁ (Py_ssize_t i = 0; i < len; i++) {
        ଚାବି_ମୂଲ୍ୟ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->ଚାବି);
    }
    ଫେରନ୍ତୁ new_seq;
}

/* Extracts all values from an asdl_seq* of KeyValuePair*'s */
asdl_ପଦସମଷ୍ଟି_seq *
Py_Pegen_get_ମୂଲ୍ୟଗୁଡ଼ିକ(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_ପଦସମଷ୍ଟି_seq *new_seq = Py_asdl_ପଦସମଷ୍ଟି_seq_new(len, p->arena);
    ଯଦି (!new_seq) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ପାଇଁ (Py_ssize_t i = 0; i < len; i++) {
        ଚାବି_ମୂଲ୍ୟ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->ମୂଲ୍ୟ);
    }
    ଫେରନ୍ତୁ new_seq;
}

/* Constructs a KeyPatternPair that is used when parsing mapping & class patterns */
ଚାବି_Pattern_Pair *
Py_Pegen_ଚାବି_pattern_pair(Parser *p, ପଦସମଷ୍ଟି_ty ଚାବି, pattern_ty pattern)
{
    ଚାବି_Pattern_Pair *a = _PyArena_Malloc(p->arena, ଆକାର(ଚାବି_Pattern_Pair));
    ଯଦି (!a) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    a->ଚାବି = ଚାବି;
    a->pattern = pattern;
    ଫେରନ୍ତୁ a;
}

/* Extracts all keys from an asdl_seq* of KeyPatternPair*'s */
asdl_ପଦସମଷ୍ଟି_seq *
Py_Pegen_get_pattern_ଚାବିଗୁଡ଼ିକ(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_ପଦସମଷ୍ଟି_seq *new_seq = Py_asdl_ପଦସମଷ୍ଟି_seq_new(len, p->arena);
    ଯଦି (!new_seq) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ପାଇଁ (Py_ssize_t i = 0; i < len; i++) {
        ଚାବି_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->ଚାବି);
    }
    ଫେରନ୍ତୁ new_seq;
}

/* Extracts all patterns from an asdl_seq* of KeyPatternPair*'s */
asdl_pattern_seq *
_PyPegen_get_patterns(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_pattern_seq *new_seq = _Py_asdl_pattern_seq_new(len, p->arena);
    ଯଦି (!new_seq) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ପାଇଁ (Py_ssize_t i = 0; i < len; i++) {
        ଚାବି_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->pattern);
    }
    ଫେରନ୍ତୁ new_seq;
}

/* Constructs a NameDefaultPair */
NameDefaultPair *
_PyPegen_name_default_pair(Parser *p, arg_ty arg, ପଦସମଷ୍ଟି_ty ମୂଲ୍ୟ, Token *tc)
{
    NameDefaultPair *a = _PyArena_Malloc(p->arena, ଆକାର(NameDefaultPair));
    ଯଦି (!a) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    a->arg = _PyPegen_add_type_comment_to_arg(p, arg, tc);
    ଯଦି (!a->arg) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    a->ମୂଲ୍ୟ = ମୂଲ୍ୟ;
    ଫେରନ୍ତୁ a;
}

/* Constructs a SlashWithDefault */
SlashWithDefault *
_PyPegen_slash_with_default(Parser *p, asdl_arg_seq *plain_names, asdl_seq *names_with_defaults)
{
    SlashWithDefault *a = _PyArena_Malloc(p->arena, ଆକାର(SlashWithDefault));
    ଯଦି (!a) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    a->plain_names = plain_names;
    a->names_with_defaults = names_with_defaults;
    ଫେରନ୍ତୁ a;
}

/* Constructs a StarEtc */
StarEtc *
_PyPegen_star_etc(Parser *p, arg_ty vararg, asdl_seq *kwonlyargs, arg_ty kwarg)
{
    StarEtc *a = _PyArena_Malloc(p->arena, ଆକାର(StarEtc));
    ଯଦି (!a) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    a->vararg = vararg;
    a->kwonlyargs = kwonlyargs;
    a->kwarg = kwarg;
    ଫେରନ୍ତୁ a;
}

asdl_seq *
_PyPegen_join_sequences(Parser *p, asdl_seq *a, asdl_seq *b)
{
    Py_ssize_t first_len = asdl_seq_LEN(a);
    Py_ssize_t second_len = asdl_seq_LEN(b);
    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(first_len + second_len, p->arena);
    ଯଦି (!new_seq) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0;
    ପାଇଁ (Py_ssize_t i = 0; i < first_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(a, i));
    }
    ପାଇଁ (Py_ssize_t i = 0; i < second_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(b, i));
    }

    ଫେରନ୍ତୁ new_seq;
}

ସ୍ଥିର asdl_arg_seq*
_get_names(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_arg_seq *seq = _Py_asdl_arg_seq_new(len, p->arena);
    ଯଦି (!seq) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ପାଇଁ (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->arg);
    }
    ଫେରନ୍ତୁ seq;
}

ସ୍ଥିର asdl_ପଦସମଷ୍ଟି_seq *
_get_defaults(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_ପଦସମଷ୍ଟି_seq *seq = Py_asdl_ପଦସମଷ୍ଟି_seq_new(len, p->arena);
    ଯଦି (!seq) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ପାଇଁ (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->ମୂଲ୍ୟ);
    }
    ଫେରନ୍ତୁ seq;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_make_posonlyargs(Parser *p,
                  asdl_arg_seq *slash_without_default,
                  SlashWithDefault *slash_with_default,
                  asdl_arg_seq **posonlyargs) {
    ଯଦି (slash_without_default != ଖାଲି) {
        *posonlyargs = slash_without_default;
    }
    ଅନ୍ୟଥା ଯଦି (slash_with_default != ଖାଲି) {
        asdl_arg_seq *slash_with_default_names =
                _get_names(p, slash_with_default->names_with_defaults);
        ଯଦି (!slash_with_default_names) {
            ଫେରନ୍ତୁ -1;
        }
        *posonlyargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default->plain_names,
                (asdl_seq*)slash_with_default_names);
    }
    ଅନ୍ୟଥା {
        *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }
    ଫେରନ୍ତୁ *posonlyargs == ଖାଲି ? -1 : 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_make_posargs(Parser *p,
              asdl_arg_seq *plain_names,
              asdl_seq *names_with_default,
              asdl_arg_seq **posargs) {

    ଯଦି (names_with_default != ଖାଲି) {
        ଯଦି (plain_names != ଖାଲି) {
            asdl_arg_seq *names_with_default_names = _get_names(p, names_with_default);
            ଯଦି (!names_with_default_names) {
                ଫେରନ୍ତୁ -1;
            }
            *posargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                    p,(asdl_seq*)plain_names, (asdl_seq*)names_with_default_names);
        }
        ଅନ୍ୟଥା {
            *posargs = _get_names(p, names_with_default);
        }
    }
    ଅନ୍ୟଥା {
        ଯଦି (plain_names != ଖାଲି) {
            // With the current grammar, we never get here.
            // If that has changed, remove the assert, and test thoroughly.
            ପ୍ରତିଜ୍ଞା(0);
            *posargs = plain_names;
        }
        ଅନ୍ୟଥା {
            *posargs = _Py_asdl_arg_seq_new(0, p->arena);
        }
    }
    ଫେରନ୍ତୁ *posargs == ଖାଲି ? -1 : 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_make_posdefaults(Parser *p,
                  SlashWithDefault *slash_with_default,
                  asdl_seq *names_with_default,
                  asdl_ପଦସମଷ୍ଟି_seq **posdefaults) {
    ଯଦି (slash_with_default != ଖାଲି && names_with_default != ଖାଲି) {
        asdl_ପଦସମଷ୍ଟି_seq *slash_with_default_ମୂଲ୍ୟଗୁଡ଼ିକ =
                _get_defaults(p, slash_with_default->names_with_defaults);
        ଯଦି (!slash_with_default_ମୂଲ୍ୟଗୁଡ଼ିକ) {
            ଫେରନ୍ତୁ -1;
        }
        asdl_ପଦସମଷ୍ଟି_seq *names_with_default_ମୂଲ୍ୟଗୁଡ଼ିକ = _get_defaults(p, names_with_default);
        ଯଦି (!names_with_default_ମୂଲ୍ୟଗୁଡ଼ିକ) {
            ଫେରନ୍ତୁ -1;
        }
        *posdefaults = (asdl_ପଦସମଷ୍ଟି_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default_ମୂଲ୍ୟଗୁଡ଼ିକ,
                (asdl_seq*)names_with_default_ମୂଲ୍ୟଗୁଡ଼ିକ);
    }
    ଅନ୍ୟଥା ଯଦି (slash_with_default == ଖାଲି && names_with_default != ଖାଲି) {
        *posdefaults = _get_defaults(p, names_with_default);
    }
    ଅନ୍ୟଥା ଯଦି (slash_with_default != ଖାଲି && names_with_default == ଖାଲି) {
        *posdefaults = _get_defaults(p, slash_with_default->names_with_defaults);
    }
    ଅନ୍ୟଥା {
        *posdefaults = Py_asdl_ପଦସମଷ୍ଟି_seq_new(0, p->arena);
    }
    ଫେରନ୍ତୁ *posdefaults == ଖାଲି ? -1 : 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_make_kwargs(Parser *p, StarEtc *star_etc,
             asdl_arg_seq **kwonlyargs,
             asdl_ପଦସମଷ୍ଟି_seq **kwdefaults) {
    ଯଦି (star_etc != ଖାଲି && star_etc->kwonlyargs != ଖାଲି) {
        *kwonlyargs = _get_names(p, star_etc->kwonlyargs);
    }
    ଅନ୍ୟଥା {
        *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }

    ଯଦି (*kwonlyargs == ଖାଲି) {
        ଫେରନ୍ତୁ -1;
    }

    ଯଦି (star_etc != ଖାଲି && star_etc->kwonlyargs != ଖାଲି) {
        *kwdefaults = _get_defaults(p, star_etc->kwonlyargs);
    }
    ଅନ୍ୟଥା {
        *kwdefaults = Py_asdl_ପଦସମଷ୍ଟି_seq_new(0, p->arena);
    }

    ଯଦି (*kwdefaults == ଖାଲି) {
        ଫେରନ୍ତୁ -1;
    }

    ଫେରନ୍ତୁ 0;
}

/* Constructs an arguments_ty object out of all the parsed constructs in the parameters rule */
arguments_ty
_PyPegen_make_arguments(Parser *p, asdl_arg_seq *slash_without_default,
                        SlashWithDefault *slash_with_default, asdl_arg_seq *plain_names,
                        asdl_seq *names_with_default, StarEtc *star_etc)
{
    asdl_arg_seq *posonlyargs;
    ଯଦି (_make_posonlyargs(p, slash_without_default, slash_with_default, &posonlyargs) == -1) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    asdl_arg_seq *posargs;
    ଯଦି (_make_posargs(p, plain_names, names_with_default, &posargs) == -1) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    asdl_ପଦସମଷ୍ଟି_seq *posdefaults;
    ଯଦି (_make_posdefaults(p,slash_with_default, names_with_default, &posdefaults) == -1) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    arg_ty vararg = ଖାଲି;
    ଯଦି (star_etc != ଖାଲି && star_etc->vararg != ଖାଲି) {
        vararg = star_etc->vararg;
    }

    asdl_arg_seq *kwonlyargs;
    asdl_ପଦସମଷ୍ଟି_seq *kwdefaults;
    ଯଦି (_make_kwargs(p, star_etc, &kwonlyargs, &kwdefaults) == -1) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    arg_ty kwarg = ଖାଲି;
    ଯଦି (star_etc != ଖାଲି && star_etc->kwarg != ଖାଲି) {
        kwarg = star_etc->kwarg;
    }

    ଫେରନ୍ତୁ _PyAST_arguments(posonlyargs, posargs, vararg, kwonlyargs,
                            kwdefaults, kwarg, posdefaults, p->arena);
}


/* Constructs an empty arguments_ty object, that gets used when a function accepts no
 * arguments. */
arguments_ty
_PyPegen_empty_arguments(Parser *p)
{
    asdl_arg_seq *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    ଯଦି (!posonlyargs) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    asdl_arg_seq *posargs = _Py_asdl_arg_seq_new(0, p->arena);
    ଯଦି (!posargs) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    asdl_ପଦସମଷ୍ଟି_seq *posdefaults = Py_asdl_ପଦସମଷ୍ଟି_seq_new(0, p->arena);
    ଯଦି (!posdefaults) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    asdl_arg_seq *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    ଯଦି (!kwonlyargs) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    asdl_ପଦସମଷ୍ଟି_seq *kwdefaults = Py_asdl_ପଦସମଷ୍ଟି_seq_new(0, p->arena);
    ଯଦି (!kwdefaults) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    ଫେରନ୍ତୁ _PyAST_arguments(posonlyargs, posargs, ଖାଲି, kwonlyargs,
                            kwdefaults, ଖାଲି, posdefaults, p->arena);
}

/* Encapsulates the value of an operator_ty into an AugOperator struct */
AugOperator *
_PyPegen_augoperator(Parser *p, operator_ty kind)
{
    AugOperator *a = _PyArena_Malloc(p->arena, ଆକାର(AugOperator));
    ଯଦି (!a) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    a->kind = kind;
    ଫେରନ୍ତୁ a;
}

/* Construct a FunctionDef equivalent to function_def, but with decorators */
stmt_ty
_PyPegen_function_def_decorators(Parser *p, asdl_ପଦସମଷ୍ଟି_seq *decorators, stmt_ty function_def)
{
    ପ୍ରତିଜ୍ଞା(function_def != ଖାଲି);
    ଯଦି (function_def->kind == AsyncFunctionDef_kind) {
        ଫେରନ୍ତୁ _PyAST_AsyncFunctionDef(
            function_def->v.AsyncFunctionDef.name,
            function_def->v.AsyncFunctionDef.args,
            function_def->v.AsyncFunctionDef.body, decorators,
            function_def->v.AsyncFunctionDef.returns,
            function_def->v.AsyncFunctionDef.type_comment,
            function_def->v.AsyncFunctionDef.type_params,
            function_def->lineno, function_def->col_offset,
            function_def->end_lineno, function_def->end_col_offset, p->arena);
    }

    ଫେରନ୍ତୁ _PyAST_FunctionDef(
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
_PyPegen_class_def_decorators(Parser *p, asdl_ପଦସମଷ୍ଟି_seq *decorators, stmt_ty class_def)
{
    ପ୍ରତିଜ୍ଞା(class_def != ଖାଲି);
    ଫେରନ୍ତୁ _PyAST_ClassDef(
        class_def->v.ClassDef.name,
        class_def->v.ClassDef.bases, class_def->v.ClassDef.keywords,
        class_def->v.ClassDef.body, decorators,
        class_def->v.ClassDef.type_params,
        class_def->lineno, class_def->col_offset, class_def->end_lineno,
        class_def->end_col_offset, p->arena);
}

/* Construct a KeywordOrStarred */
KeywordOrStarred *
_PyPegen_keyword_or_starred(Parser *p, ଶୂନ୍ୟ *element, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_keyword)
{
    KeywordOrStarred *a = _PyArena_Malloc(p->arena, ଆକାର(KeywordOrStarred));
    ଯଦି (!a) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    a->element = element;
    a->is_keyword = is_keyword;
    ଫେରନ୍ତୁ a;
}

/* Get the number of starred expressions in an asdl_seq* of KeywordOrStarred*s */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
seq_ସଂଖ୍ୟା_of_starred_exprs(asdl_seq *seq)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = 0;
    ପାଇଁ (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(seq, i);
        ଯଦି (!k->is_keyword) {
            n++;
        }
    }
    ଫେରନ୍ତୁ n;
}

/* Extract the starred expressions of an asdl_seq* of KeywordOrStarred*s */
asdl_ପଦସମଷ୍ଟି_seq *
_PyPegen_seq_extract_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_len = seq_ସଂଖ୍ୟା_of_starred_exprs(kwargs);
    ଯଦି (new_len == 0) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    asdl_ପଦସମଷ୍ଟି_seq *new_seq = Py_asdl_ପଦସମଷ୍ଟି_seq_new(new_len, p->arena);
    ଯଦି (!new_seq) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0;
    ପାଇଁ (Py_ssize_t i = 0, len = asdl_seq_LEN(kwargs); i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        ଯଦି (!k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    ଫେରନ୍ତୁ new_seq;
}

/* Return a new asdl_seq* with only the keywords in kwargs */
asdl_keyword_seq*
_PyPegen_seq_delete_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    Py_ssize_t len = asdl_seq_LEN(kwargs);
    Py_ssize_t new_len = len - seq_ସଂଖ୍ୟା_of_starred_exprs(kwargs);
    ଯଦି (new_len == 0) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    asdl_keyword_seq *new_seq = _Py_asdl_keyword_seq_new(new_len, p->arena);
    ଯଦି (!new_seq) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0;
    ପାଇଁ (Py_ssize_t i = 0; i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        ଯଦି (k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    ଫେରନ୍ତୁ new_seq;
}

ପଦସମଷ୍ଟି_ty
_PyPegen_ensure_imaginary(Parser *p, ପଦସମଷ୍ଟି_ty exp)
{
    ଯଦି (exp->kind != Constant_kind || !PyComplex_CheckExact(exp->v.Constant.ମୂଲ୍ୟ)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "imaginary number required in complex literal");
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ exp;
}

ପଦସମଷ୍ଟି_ty
_PyPegen_ensure_real(Parser *p, ପଦସମଷ୍ଟି_ty exp)
{
    ଯଦି (exp->kind != Constant_kind || PyComplex_CheckExact(exp->v.Constant.ମୂଲ୍ୟ)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "real number required in complex literal");
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ exp;
}

mod_ty
_PyPegen_make_module(Parser *p, asdl_stmt_seq *a) {
    asdl_type_ignore_seq *type_ignores = ଖାଲି;
    Py_ssize_t ଅଙ୍କ = p->type_ignore_comments.ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ;
    ଯଦି (ଅଙ୍କ > 0) {
        // Turn the raw (comment, lineno) pairs into TypeIgnore objects in the arena
        type_ignores = _Py_asdl_type_ignore_seq_new(ଅଙ୍କ, p->arena);
        ଯଦି (type_ignores == ଖାଲି) {
            ଫେରନ୍ତୁ ଖାଲି;
        }
        ପାଇଁ (Py_ssize_t i = 0; i < ଅଙ୍କ; i++) {
            PyObject *tag = _PyPegen_new_type_comment(p, p->type_ignore_comments.ବସ୍ତୁଗୁଡ଼ିକ[i].comment);
            ଯଦି (tag == ଖାଲି) {
                ଫେରନ୍ତୁ ଖାଲି;
            }
            type_ignore_ty ti = _PyAST_TypeIgnore(p->type_ignore_comments.ବସ୍ତୁଗୁଡ଼ିକ[i].lineno,
                                                  tag, p->arena);
            ଯଦି (ti == ଖାଲି) {
                ଫେରନ୍ତୁ ଖାଲି;
            }
            asdl_seq_SET(type_ignores, i, ti);
        }
    }
    ଫେରନ୍ତୁ _PyAST_Module(a, type_ignores, p->arena);
}

PyObject *
_PyPegen_new_type_comment(Parser *p, ଅଚଳ ଅକ୍ଷର *s)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, strlen(s), ଖାଲି);
    ଯଦି (res == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (_PyArena_AddPyObject(p->arena, res) < 0) {
        Py_DECREF(res);
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ res;
}

arg_ty
_PyPegen_add_type_comment_to_arg(Parser *p, arg_ty a, Token *tc)
{
    ଯଦି (tc == ଖାଲି) {
        ଫେରନ୍ତୁ a;
    }
    ଅଚଳ ଅକ୍ଷର *bytes = PyBytes_AsString(tc->bytes);
    ଯଦି (bytes == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    ଯଦି (tco == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ _PyAST_arg(a->arg, a->annotation, tco,
                      a->lineno, a->col_offset, a->end_lineno, a->end_col_offset,
                      p->arena);
}

/* Checks if the NOTEQUAL token is valid given the current parser flags
0 indicates success and nonzero indicates failure (an exception may be set) */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyPegen_check_barry_as_flufl(Parser *p, Token* t) {
    ପ୍ରତିଜ୍ଞା(t->bytes != ଖାଲି);
    ପ୍ରତିଜ୍ଞା(t->type == NOTEQUAL);

    ଅଚଳ ଅକ୍ଷର* tok_str = PyBytes_AS_STRING(t->bytes);
    ଯଦି (p->flags & PyPARSE_BARRY_AS_BDFL && strcmp(tok_str, "<>") != 0) {
        RAISE_SYNTAX_ERROR("with Barry as BDFL, use '<>' instead of '!='");
        ଫେରନ୍ତୁ -1;
    }
    ଯଦି (!(p->flags & PyPARSE_BARRY_AS_BDFL)) {
        ଫେରନ୍ତୁ strcmp(tok_str, "!=");
    }
    ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyPegen_check_legacy_stmt(Parser *p, ପଦସମଷ୍ଟି_ty name) {
    ଯଦି (name->kind != Name_kind) {
        ଫେରନ୍ତୁ 0;
    }
    ଅଚଳ ଅକ୍ଷର* candidates[2] = {"print", "exec"};
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<2; i++) {
        ଯଦି (PyUnicode_CompareWithASCIIString(name->v.Name.id, candidates[i]) == 0) {
            ଫେରନ୍ତୁ 1;
        }
    }
    ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ *
_PyPegen_raise_error_for_missing_comma(Parser *p, ପଦସମଷ୍ଟି_ty a, ପଦସମଷ୍ଟି_ty b)
{
    // Don't raise for legacy statements like "print x" or "exec x"
    ଯଦି (_PyPegen_check_legacy_stmt(p, a)) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    // Only raise inside parentheses/brackets (level > 0)
    ଯଦି (p->tokens[p->mark - 1]->level == 0) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    // For multi-line expressions (like string concatenations), point to the
    // last line instead of the first for a more helpful error message.
    // Use a->col_offset as the starting column since all strings in the
    // concatenation typically share the same indentation.
    ଯଦି (a->end_lineno > a->lineno) {
        ଫେରନ୍ତୁ RAISE_ERROR_KNOWN_LOCATION(
            p, PyExc_SyntaxError, a->end_lineno, a->col_offset,
            a->end_lineno, a->end_col_offset,
            "invalid syntax. Perhaps you forgot a comma?"
        );
    }
    ଫେରନ୍ତୁ RAISE_ERROR_KNOWN_LOCATION(
        p, PyExc_SyntaxError, a->lineno, a->col_offset,
        b->end_lineno, b->end_col_offset,
        "invalid syntax. Perhaps you forgot a comma?"
    );
}

ସ୍ଥିର ResultTokenWithMetadata *
result_token_with_metadata(Parser *p, ଶୂନ୍ୟ *result, PyObject *metadata)
{
    ResultTokenWithMetadata *res = _PyArena_Malloc(p->arena, ଆକାର(ResultTokenWithMetadata));
    ଯଦି (res == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    res->metadata = metadata;
    res->result = result;
    ଫେରନ୍ତୁ res;
}

ସ୍ଥିର ଅକ୍ଷର
formatted_string_prefix(ଅଚଳ Parser *p)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା nested = 0;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = p->mark - 1; i >= 0; i--) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା type = p->tokens[i]->type;
        ଯଦି (type == FSTRING_END || type == TSTRING_END) {
            nested++;
        }
        ଅନ୍ୟଥା ଯଦି (type == FSTRING_START || type == TSTRING_START) {
            ଯଦି (nested == 0) {
                ଫେରନ୍ତୁ type == TSTRING_START ? 't' : 'f';
            }
            nested--;
        }
    }
    Py_UNREACHABLE();
}

ResultTokenWithMetadata *
_PyPegen_check_fstring_conversion(Parser *p, Token* conv_token, ପଦସମଷ୍ଟି_ty conv)
{
    ଯଦି (conv_token->lineno != conv->lineno || conv_token->end_col_offset != conv->col_offset) {
        ଫେରନ୍ତୁ RAISE_SYNTAX_ERROR_KNOWN_RANGE(
            conv_token, conv,
            "%c-string: conversion type must come right after the exclamation mark",
            formatted_string_prefix(p)
        );
    }

    Py_UCS4 first = PyUnicode_READ_CHAR(conv->v.Name.id, 0);
    ଯଦି (PyUnicode_GET_LENGTH(conv->v.Name.id) > 1 ||
            !(first == 's' || first == 'r' || first == 'a')) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(conv,
                                            "%c-string: invalid conversion character %R: expected 's', 'r', or 'a'",
                                            formatted_string_prefix(p),
                                            conv->v.Name.id);
        ଫେରନ୍ତୁ ଖାଲି;
    }

    ଫେରନ୍ତୁ result_token_with_metadata(p, conv, conv_token->metadata);
}

ResultTokenWithMetadata *
_PyPegen_setup_full_format_spec(Parser *p, Token *colon, asdl_ପଦସମଷ୍ଟି_seq *spec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_offset,
                                ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_lineno, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col_offset, PyArena *arena)
{
    ଯଦି (!spec) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    // This is needed to keep compatibility with 3.11, where an empty format
    // spec is parsed as an *empty* JoinedStr node, instead of having an empty
    // constant in it.
    Py_ssize_t n_ବସ୍ତୁଗୁଡ଼ିକ = asdl_seq_LEN(spec);
    Py_ssize_t non_empty_ଗଣନା = 0;
    ପାଇଁ (Py_ssize_t i = 0; i < n_ବସ୍ତୁଗୁଡ଼ିକ; i++) {
        ପଦସମଷ୍ଟି_ty item = asdl_seq_GET(spec, i);
        non_empty_ଗଣନା += !(item->kind == Constant_kind &&
                             PyUnicode_CheckExact(item->v.Constant.ମୂଲ୍ୟ) &&
                             PyUnicode_GET_LENGTH(item->v.Constant.ମୂଲ୍ୟ) == 0);
    }
    ଯଦି (non_empty_ଗଣନା != n_ବସ୍ତୁଗୁଡ଼ିକ) {
        asdl_ପଦସମଷ୍ଟି_seq *resized_spec =
            Py_asdl_ପଦସମଷ୍ଟି_seq_new(non_empty_ଗଣନା, p->arena);
        ଯଦି (resized_spec == ଖାଲି) {
            ଫେରନ୍ତୁ ଖାଲି;
        }
        Py_ssize_t j = 0;
        ପାଇଁ (Py_ssize_t i = 0; i < n_ବସ୍ତୁଗୁଡ଼ିକ; i++) {
            ପଦସମଷ୍ଟି_ty item = asdl_seq_GET(spec, i);
            ଯଦି (item->kind == Constant_kind &&
                PyUnicode_CheckExact(item->v.Constant.ମୂଲ୍ୟ) &&
                PyUnicode_GET_LENGTH(item->v.Constant.ମୂଲ୍ୟ) == 0) {
                ଜାରି;
            }
            asdl_seq_SET(resized_spec, j++, item);
        }
        ପ୍ରତିଜ୍ଞା(j == non_empty_ଗଣନା);
        spec = resized_spec;
    }
    ପଦସମଷ୍ଟି_ty res;
    Py_ssize_t n = asdl_seq_LEN(spec);
    ଯଦି (n == 0 || (n == 1 && asdl_seq_GET(spec, 0)->kind == Constant_kind)) {
        res = _PyAST_JoinedStr(spec, lineno, col_offset, end_lineno,
                                    end_col_offset, p->arena);
    } ଅନ୍ୟଥା {
        res = _PyPegen_concatenate_strings(p, spec,
                             lineno, col_offset, end_lineno,
                             end_col_offset, arena);
    }
    ଯଦି (!res) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ result_token_with_metadata(p, res, colon->metadata);
}

ଅଚଳ ଅକ୍ଷର *
Py_Pegen_get_ପଦସମଷ୍ଟି_name(ପଦସମଷ୍ଟି_ty e)
{
    ପ୍ରତିଜ୍ଞା(e != ଖାଲି);
    ବାଛ (e->kind) {
        ସ୍ଥିତି Attribute_kind:
            ଫେରନ୍ତୁ "attribute";
        ସ୍ଥିତି Subscript_kind:
            ଫେରନ୍ତୁ "subscript";
        ସ୍ଥିତି Starred_kind:
            ଫେରନ୍ତୁ "starred";
        ସ୍ଥିତି Name_kind:
            ଫେରନ୍ତୁ "name";
        ସ୍ଥିତି List_kind:
            ଫେରନ୍ତୁ "list";
        ସ୍ଥିତି Tuple_kind:
            ଫେରନ୍ତୁ "tuple";
        ସ୍ଥିତି Lambda_kind:
            ଫେରନ୍ତୁ "lambda";
        ସ୍ଥିତି Call_kind:
            ଫେରନ୍ତୁ "function call";
        ସ୍ଥିତି BoolOp_kind:
        ସ୍ଥିତି BinOp_kind:
        ସ୍ଥିତି UnaryOp_kind:
            ଫେରନ୍ତୁ "expression";
        ସ୍ଥିତି GeneratorExp_kind:
            ଫେରନ୍ତୁ "generator expression";
        ସ୍ଥିତି Yield_kind:
        ସ୍ଥିତି YieldFrom_kind:
            ଫେରନ୍ତୁ "yield expression";
        ସ୍ଥିତି Await_kind:
            ଫେରନ୍ତୁ "await expression";
        ସ୍ଥିତି ListComp_kind:
            ଫେରନ୍ତୁ "list comprehension";
        ସ୍ଥିତି SetComp_kind:
            ଫେରନ୍ତୁ "set comprehension";
        ସ୍ଥିତି DictComp_kind:
            ଫେରନ୍ତୁ "dict comprehension";
        ସ୍ଥିତି Dict_kind:
            ଫେରନ୍ତୁ "dict literal";
        ସ୍ଥିତି Set_kind:
            ଫେରନ୍ତୁ "set display";
        ସ୍ଥିତି JoinedStr_kind:
        ସ୍ଥିତି Formatted_ମୂଲ୍ୟ_kind:
            ଫେରନ୍ତୁ "f-string expression";
        ସ୍ଥିତି TemplateStr_kind:
        ସ୍ଥିତି Interpolation_kind:
            ଫେରନ୍ତୁ "t-string expression";
        ସ୍ଥିତି Constant_kind: {
            PyObject *ମୂଲ୍ୟ = e->v.Constant.ମୂଲ୍ୟ;
            ଯଦି (ମୂଲ୍ୟ == Py_None) {
                ଫେରନ୍ତୁ "None";
            }
            ଯଦି (ମୂଲ୍ୟ == Py_False) {
                ଫେରନ୍ତୁ "False";
            }
            ଯଦି (ମୂଲ୍ୟ == Py_True) {
                ଫେରନ୍ତୁ "True";
            }
            ଯଦି (ମୂଲ୍ୟ == Py_Ellipsis) {
                ଫେରନ୍ତୁ "ellipsis";
            }
            ଫେରନ୍ତୁ "literal";
        }
        ସ୍ଥିତି Compare_kind:
            ଫେରନ୍ତୁ "comparison";
        ସ୍ଥିତି IfExp_kind:
            ଫେରନ୍ତୁ "conditional expression";
        ସ୍ଥିତି Named_ପଦସମଷ୍ଟି_kind:
            ଫେରନ୍ତୁ "named expression";
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->kind, e->lineno);
            ଫେରନ୍ତୁ ଖାଲି;
    }
}

ପଦସମଷ୍ଟି_ty
_PyPegen_get_last_comprehension_item(comprehension_ty comprehension) {
    ଯଦି (comprehension->ifs == ଖାଲି || asdl_seq_LEN(comprehension->ifs) == 0) {
        ଫେରନ୍ତୁ comprehension->iter;
    }
    ଫେରନ୍ତୁ PyPegen_last_item(comprehension->ifs, ପଦସମଷ୍ଟି_ty);
}

ପଦସମଷ୍ଟି_ty _PyPegen_collect_call_seqs(Parser *p, asdl_ପଦସମଷ୍ଟି_seq *a, asdl_seq *b,
                     ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_lineno,
                     ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col_offset, PyArena *arena) {
    Py_ssize_t args_len = asdl_seq_LEN(a);
    Py_ssize_t total_len = args_len;

    ଯଦି (b == ଖାଲି) {
        ଫେରନ୍ତୁ _PyAST_Call(_PyPegen_dummy_name(p), a, ଖାଲି, lineno, col_offset,
                        end_lineno, end_col_offset, arena);

    }

    asdl_ପଦସମଷ୍ଟି_seq *starreds = _PyPegen_seq_extract_starred_exprs(p, b);
    ଯଦି (!starreds && PyErr_Occurred()) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    asdl_keyword_seq *keywords = _PyPegen_seq_delete_starred_exprs(p, b);
    ଯଦି (!keywords && PyErr_Occurred()) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (starreds) {
        total_len += asdl_seq_LEN(starreds);
    }

    asdl_ପଦସମଷ୍ଟି_seq *args = Py_asdl_ପଦସମଷ୍ଟି_seq_new(total_len, arena);
    ଯଦି (args == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    Py_ssize_t i = 0;
    ପାଇଁ (i = 0; i < args_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(a, i));
    }
    ପାଇଁ (; i < total_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(starreds, i - args_len));
    }

    ଫେରନ୍ତୁ _PyAST_Call(_PyPegen_dummy_name(p), args, keywords, lineno,
                       col_offset, end_lineno, end_col_offset, arena);
}

// AST Error reporting helpers

ପଦସମଷ୍ଟି_ty
_PyPegen_get_invalid_target(ପଦସମଷ୍ଟି_ty e, TARGETS_TYPE targets_type)
{
    ଯଦି (e == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (targets_type == SINGLE_TARGETS ||
        targets_type == ATTRIBUTE_OR_SUBSCRIPT_TARGETS) {
        ଯଦି (targets_type == SINGLE_TARGETS && e->kind == Name_kind) {
            ଫେରନ୍ତୁ ଖାଲି;
        }
        ଫେରନ୍ତୁ e->kind == Attribute_kind || e->kind == Subscript_kind ? ଖାଲି : e;
    }

#define VISIT_CONTAINER(CONTAINER, TYPE) କର { \
        Py_ssize_t len = asdl_seq_LEN((CONTAINER)->v.TYPE.elts);\
        ପାଇଁ (Py_ssize_t i = 0; i < len; i++) {\
            ପଦସମଷ୍ଟି_ty other = asdl_seq_GET((CONTAINER)->v.TYPE.elts, i);\
            ପଦସମଷ୍ଟି_ty child = _PyPegen_get_invalid_target(other, targets_type);\
            ଯଦି (child != ଖାଲି) {\
                ଫେରନ୍ତୁ child;\
            }\
        }\
    } ଯେତେବେଳେ (0)

    // We only need to visit List and Tuple nodes recursively as those
    // are the only ones that can contain valid names in targets when
    // they are parsed as expressions. Any other kind of expression
    // that is a container (like Sets or Dicts) is directly invalid and
    // we don't need to visit it recursively.

    ବାଛ (e->kind) {
        ସ୍ଥିତି List_kind:
            VISIT_CONTAINER(e, List);
            ଫେରନ୍ତୁ ଖାଲି;
        ସ୍ଥିତି Tuple_kind:
            VISIT_CONTAINER(e, Tuple);
            ଫେରନ୍ତୁ ଖାଲି;
        ସ୍ଥିତି Starred_kind:
            ଯଦି (targets_type == DEL_TARGETS) {
                ଫେରନ୍ତୁ e;
            }
            ଫେରନ୍ତୁ _PyPegen_get_invalid_target(e->v.Starred.ମୂଲ୍ୟ, targets_type);
        ସ୍ଥିତି Compare_kind:
            // This is needed, because the `a in b` in `for a in b` gets parsed
            // as a comparison, and so we need to search the left side of the comparison
            // for invalid targets.
            ଯଦି (targets_type == FOR_TARGETS) {
                cmpop_ty cmpop = (cmpop_ty) asdl_seq_GET(e->v.Compare.ops, 0);
                ଯଦି (cmpop == In) {
                    ଫେରନ୍ତୁ _PyPegen_get_invalid_target(e->v.Compare.ବାମ, targets_type);
                }
                ଫେରନ୍ତୁ ଖାଲି;
            }
            ଫେରନ୍ତୁ e;
        ସ୍ଥିତି Name_kind:
        ସ୍ଥିତି Subscript_kind:
        ସ୍ଥିତି Attribute_kind:
            ଫେରନ୍ତୁ ଖାଲି;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            ଫେରନ୍ତୁ e;
    }
}

ଶୂନ୍ୟ *_PyPegen_arguments_parsing_error(Parser *p, ପଦସମଷ୍ଟି_ty e) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା kwarg_unpacking = 0;
    ପାଇଁ (Py_ssize_t i = 0, l = asdl_seq_LEN(e->v.Call.keywords); i < l; i++) {
        keyword_ty keyword = asdl_seq_GET(e->v.Call.keywords, i);
        ଯଦି (!keyword->arg) {
            kwarg_unpacking = 1;
        }
    }

    ଅଚଳ ଅକ୍ଷର *ସନ୍ଦେଶ = ଖାଲି;
    ଯଦି (kwarg_unpacking) {
        ସନ୍ଦେଶ = "positional argument follows keyword argument unpacking";
    } ଅନ୍ୟଥା {
        ସନ୍ଦେଶ = "positional argument follows keyword argument";
    }

    ଫେରନ୍ତୁ RAISE_SYNTAX_ERROR(ସନ୍ଦେଶ);
}

ଶୂନ୍ୟ *
_PyPegen_nonparen_genexp_in_call(Parser *p, ପଦସମଷ୍ଟି_ty args, asdl_comprehension_seq *comprehensions)
{
    /* The rule that calls this function is 'args for_if_clauses'.
       For the input f(L, x for x in y), L and x are in args and
       the for is parsed as a for_if_clause. We have to check if
       len <= 1, so that input like dict((a, b) for a, b in x)
       gets successfully parsed and then we pass the last
       argument (x in the above example) as the location of the
       error */
    Py_ssize_t len = asdl_seq_LEN(args->v.Call.args);
    ଯଦି (len <= 1) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    comprehension_ty last_comprehension = PyPegen_last_item(comprehensions, comprehension_ty);

    ଫେରନ୍ତୁ RAISE_SYNTAX_ERROR_KNOWN_RANGE(
        (ପଦସମଷ୍ଟି_ty) asdl_seq_GET(args->v.Call.args, len - 1),
        _PyPegen_get_last_comprehension_item(last_comprehension),
        "Generator expression must be parenthesized"
    );
}

// Fstring stuff

ସ୍ଥିର ପଦସମଷ୍ଟି_ty
_PyPegen_decode_fstring_part(Parser* p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_raw, ପଦସମଷ୍ଟି_ty constant, Token* token) {
    ପ୍ରତିଜ୍ଞା(PyUnicode_CheckExact(constant->v.Constant.ମୂଲ୍ୟ));

    ଅଚଳ ଅକ୍ଷର* bstr = PyUnicode_AsUTF8(constant->v.Constant.ମୂଲ୍ୟ);
    ଯଦି (bstr == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    size_t len;
    ଯଦି (strcmp(bstr, "{{") == 0 || strcmp(bstr, "}}") == 0) {
        len = 1;
    } ଅନ୍ୟଥା {
        len = strlen(bstr);
    }

    is_raw = is_raw || strchr(bstr, '\\') == ଖାଲି;
    PyObject *str = _PyPegen_decode_string(p, is_raw, bstr, len, token);
    ଯଦି (str == ଖାଲି) {
        _Pypegen_raise_decode_error(p);
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ _PyAST_Constant(str, ଖାଲି, constant->lineno, constant->col_offset,
                           constant->end_lineno, constant->end_col_offset,
                           p->arena);
}

ସ୍ଥିର asdl_ପଦସମଷ୍ଟି_seq *
_get_resized_exprs(Parser *p, Token *a, asdl_ପଦସମଷ୍ଟି_seq *raw_expressions,
                   Token *b, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_tstring)
{
    Py_ssize_t n_ବସ୍ତୁଗୁଡ଼ିକ = asdl_seq_LEN(raw_expressions);
    Py_ssize_t total_ବସ୍ତୁଗୁଡ଼ିକ = n_ବସ୍ତୁଗୁଡ଼ିକ;
    ପାଇଁ (Py_ssize_t i = 0; i < n_ବସ୍ତୁଗୁଡ଼ିକ; i++) {
        ପଦସମଷ୍ଟି_ty item = asdl_seq_GET(raw_expressions, i);
        ଯଦି (item->kind == JoinedStr_kind) {
            total_ବସ୍ତୁଗୁଡ଼ିକ += asdl_seq_LEN(item->v.JoinedStr.ମୂଲ୍ୟଗୁଡ଼ିକ) - 1;
        }
    }

    ଅଚଳ ଅକ୍ଷର* quote_str = PyBytes_AsString(a->bytes);
    ଯଦି (quote_str == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_raw = strpbrk(quote_str, "rR") != ଖାଲି;

    asdl_ପଦସମଷ୍ଟି_seq *seq = Py_asdl_ପଦସମଷ୍ଟି_seq_new(total_ବସ୍ତୁଗୁଡ଼ିକ, p->arena);
    ଯଦି (seq == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    Py_ssize_t index = 0;
    ପାଇଁ (Py_ssize_t i = 0; i < n_ବସ୍ତୁଗୁଡ଼ିକ; i++) {
        ପଦସମଷ୍ଟି_ty item = asdl_seq_GET(raw_expressions, i);

        /* Debug expressions arrive as JoinedStr(text, value); flatten them
           into the surrounding string. */
        ଯଦି (item->kind == JoinedStr_kind) {
            asdl_ପଦସମଷ୍ଟି_seq *ମୂଲ୍ୟଗୁଡ଼ିକ = item->v.JoinedStr.ମୂଲ୍ୟଗୁଡ଼ିକ;
            ଯଦି (asdl_seq_LEN(ମୂଲ୍ୟଗୁଡ଼ିକ) != 2) {
                PyErr_Format(PyExc_SystemError,
                             is_tstring
                             ? "unexpected TemplateStr node without debug data in t-string at line %d"
                             : "unexpected JoinedStr node without debug data in f-string at line %d",
                             item->lineno);
                ଫେରନ୍ତୁ ଖାଲି;
            }

            ପଦସମଷ୍ଟି_ty first = asdl_seq_GET(ମୂଲ୍ୟଗୁଡ଼ିକ, 0);
            ପ୍ରତିଜ୍ଞା(first->kind == Constant_kind);
            asdl_seq_SET(seq, index++, first);

            ପଦସମଷ୍ଟି_ty second = asdl_seq_GET(ମୂଲ୍ୟଗୁଡ଼ିକ, 1);
            ପ୍ରତିଜ୍ଞା((is_tstring &&
                    second->kind == Interpolation_kind) ||
                   second->kind == Formatted_ମୂଲ୍ୟ_kind);
            asdl_seq_SET(seq, index++, second);

            ଜାରି;
        }

        ଯଦି (item->kind == Constant_kind) {
            item = _PyPegen_decode_fstring_part(p, is_raw, item, b);
            ଯଦି (item == ଖାଲି) {
                ଫେରନ୍ତୁ ଖାଲି;
            }

            /* Tokenizer emits string parts even when the underlying string
            might become an empty value (e.g. FSTRING_MIDDLE with the value \\n)
            so we need to check for them and simplify it here. */
            ଯଦି (PyUnicode_CheckExact(item->v.Constant.ମୂଲ୍ୟ)
                && PyUnicode_GET_LENGTH(item->v.Constant.ମୂଲ୍ୟ) == 0) {
                ଜାରି;
            }
        }
        asdl_seq_SET(seq, index++, item);
    }

    asdl_ପଦସମଷ୍ଟି_seq *resized_exprs;
    ଯଦି (index != total_ବସ୍ତୁଗୁଡ଼ିକ) {
        resized_exprs = Py_asdl_ପଦସମଷ୍ଟି_seq_new(index, p->arena);
        ଯଦି (resized_exprs == ଖାଲି) {
            ଫେରନ୍ତୁ ଖାଲି;
        }
        ପାଇଁ (Py_ssize_t i = 0; i < index; i++) {
            asdl_seq_SET(resized_exprs, i, asdl_seq_GET(seq, i));
        }
    }
    ଅନ୍ୟଥା {
        resized_exprs = seq;
    }
    ଫେରନ୍ତୁ resized_exprs;
}

ପଦସମଷ୍ଟି_ty
_PyPegen_template_str(Parser *p, Token *a, asdl_ପଦସମଷ୍ଟି_seq *raw_expressions, Token *b) {

    asdl_ପଦସମଷ୍ଟି_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 1);
    ଯଦି (resized_exprs == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ _PyAST_TemplateStr(resized_exprs, a->lineno, a->col_offset,
                              b->end_lineno, b->end_col_offset,
                              p->arena);
}

ପଦସମଷ୍ଟି_ty
_PyPegen_joined_str(Parser *p, Token* a, asdl_ପଦସମଷ୍ଟି_seq* raw_expressions, Token*b) {

    asdl_ପଦସମଷ୍ଟି_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 0);
    ଯଦି (resized_exprs == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ _PyAST_JoinedStr(resized_exprs, a->lineno, a->col_offset,
                            b->end_lineno, b->end_col_offset,
                            p->arena);
}

ପଦସମଷ୍ଟି_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok) {
    ଯଦି (tok->bytes == ଖାଲି) {
        ଫେରନ୍ତୁ Py_Pegen_raise_incomplete_ନିବେଶ(p);
    }
    Py_ssize_t bsize = PyBytes_GET_SIZE(tok->bytes);
    ଅଚଳ ଅକ୍ଷର *bstr = PyBytes_AS_STRING(tok->bytes);

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_raw = tok->is_raw;

    PyObject* str = _PyPegen_decode_string(p, is_raw, bstr, bsize, tok);
    ଯଦି (str == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ _PyAST_Constant(str, ଖାଲି, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

ପଦସମଷ୍ଟି_ty _PyPegen_constant_from_token(Parser* p, Token* tok) {
    ଯଦି (tok->bytes == ଖାଲି) {
        ଫେରନ୍ତୁ Py_Pegen_raise_incomplete_ନିବେଶ(p);
    }
    ଅଚଳ ଅକ୍ଷର *bstr = PyBytes_AS_STRING(tok->bytes);
    PyObject* str = PyUnicode_FromString(bstr);
    ଯଦି (str == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ _PyAST_Constant(str, ଖାଲି, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

ପଦସମଷ୍ଟି_ty _PyPegen_constant_from_string(Parser* p, Token* tok) {
    ଯଦି (tok->bytes == ଖାଲି) {
        ଫେରନ୍ତୁ Py_Pegen_raise_incomplete_ନିବେଶ(p);
    }
    ଅଚଳ ଅକ୍ଷର *the_str = PyBytes_AS_STRING(tok->bytes);
    PyObject *s = _PyPegen_parse_string(p, tok);
    ଯଦି (s == ଖାଲି) {
        _Pypegen_raise_decode_error(p);
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (_PyArena_AddPyObject(p->arena, s) < 0) {
        Py_DECREF(s);
        ଫେରନ୍ତୁ ଖାଲି;
    }
    PyObject *kind = ଖାଲି;
    ଯଦି (the_str && the_str[0] == 'u') {
        kind = _PyPegen_new_identifier(p, "u");
        ଯଦି (kind == ଖାଲି) {
            ଫେରନ୍ତୁ ଖାଲି;
        }
    }
    ଫେରନ୍ତୁ _PyAST_Constant(s, kind, tok->lineno, tok->col_offset, tok->end_lineno, tok->end_col_offset, p->arena);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_get_interpolation_conversion(Parser *p, Token *debug, ResultTokenWithMetadata *conversion,
                              ResultTokenWithMetadata *format)
{
    ଯଦି (conversion != ଖାଲି) {
        ପଦସମଷ୍ଟି_ty conversion_ପଦସମଷ୍ଟି = (ପଦସମଷ୍ଟି_ty) conversion->result;
        ପ୍ରତିଜ୍ଞା(conversion_ପଦସମଷ୍ଟି->kind == Name_kind);
        Py_UCS4 first = PyUnicode_READ_CHAR(conversion_ପଦସମଷ୍ଟି->v.Name.id, 0);
        ଫେରନ୍ତୁ Py_SAFE_DOWNCAST(first, Py_UCS4, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
    }
    ଅନ୍ୟଥା ଯଦି (debug && !format) {
        /* If no conversion is specified, use !r for debug expressions */
        ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)'r';
    }
    ଫେରନ୍ତୁ -1;
}

ସ୍ଥିର PyObject *
strip_interpolation_debug_ପଦସମଷ୍ଟି(PyObject *exprstr)
{
    Py_ssize_t len = PyUnicode_GET_LENGTH(exprstr);

    /* Discard whitespace and explicit line continuations after the debug "="
       but preserve whitespace before it. */
    ଯେତେବେଳେ (len > 0) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_newline = 0;
        ଯେତେବେଳେ (len > 0) {
            Py_UCS4 c = PyUnicode_READ_CHAR(exprstr, len - 1);
            ଯଦି (!_PyUnicode_IsWhitespace(c)) {
                ଭାଙ୍ଗ;
            }
            ଯଦି (c == '\r' || c == '\n') {
                has_newline = 1;
            }
            len--;
        }
        ଯଦି (!has_newline || len == 0 ||
            PyUnicode_READ_CHAR(exprstr, len - 1) != '\\')
        {
            ଭାଙ୍ଗ;
        }
        len--;
    }

    /* Preserve unexpected metadata instead of dropping source text. */
    ଯଦି (len == 0 || PyUnicode_READ_CHAR(exprstr, len - 1) != '=') {
        ଫେରନ୍ତୁ Py_NewRef(exprstr);
    }

    ଫେରନ୍ତୁ PyUnicode_Substring(exprstr, 0, len - 1);
}

ପଦସମଷ୍ଟି_ty _PyPegen_interpolation(Parser *p, ପଦସମଷ୍ଟି_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_offset,
                                 ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_lineno, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col_offset, PyArena *arena) {

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    /* Find the non whitespace token after the "=" */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା debug_end_ଧାଡ଼ି, debug_end_offset;
    PyObject *debug_metadata;
    constant exprstr;

    ଯଦି (conversion) {
        debug_end_ଧାଡ଼ି = ((ପଦସମଷ୍ଟି_ty) conversion->result)->lineno;
        debug_end_offset = ((ପଦସମଷ୍ଟି_ty) conversion->result)->col_offset;
        debug_metadata = exprstr = conversion->metadata;
    }
    ଅନ୍ୟଥା ଯଦି (format) {
        debug_end_ଧାଡ଼ି = ((ପଦସମଷ୍ଟି_ty) format->result)->lineno;
        debug_end_offset = ((ପଦସମଷ୍ଟି_ty) format->result)->col_offset + 1;
        debug_metadata = exprstr = format->metadata;
    }
    ଅନ୍ୟଥା {
        debug_end_ଧାଡ଼ି = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = exprstr = closing_brace->metadata;
    }

    ପ୍ରତିଜ୍ଞା(exprstr != ଖାଲି);
    PyObject *final_exprstr = debug
        ? strip_interpolation_debug_ପଦସମଷ୍ଟି(exprstr)
        : Py_NewRef(exprstr);
    ଯଦି (!final_exprstr || _PyArena_AddPyObject(arena, final_exprstr) < 0) {
        Py_XDECREF(final_exprstr);
        ଫେରନ୍ତୁ ଖାଲି;
    }

    ପଦସମଷ୍ଟି_ty interpolation = _PyAST_Interpolation(
        expression, final_exprstr, conversion_val, format ? (ପଦସମଷ୍ଟି_ty) format->result : ଖାଲି,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    ଯଦି (!interpolation || !debug) {
        ଫେରନ୍ତୁ interpolation;
    }

    ପଦସମଷ୍ଟି_ty debug_text = _PyAST_Constant(debug_metadata, ଖାଲି, lineno, col_offset + 1, debug_end_ଧାଡ଼ି,
                                            debug_end_offset - 1, p->arena);
    ଯଦି (!debug_text) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    asdl_ପଦସମଷ୍ଟି_seq *ମୂଲ୍ୟଗୁଡ଼ିକ = Py_asdl_ପଦସମଷ୍ଟି_seq_new(2, arena);
    ଯଦି (!ମୂଲ୍ୟଗୁଡ଼ିକ) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    asdl_seq_SET(ମୂଲ୍ୟଗୁଡ଼ିକ, 0, debug_text);
    asdl_seq_SET(ମୂଲ୍ୟଗୁଡ଼ିକ, 1, interpolation);
    ଫେରନ୍ତୁ _PyAST_JoinedStr(ମୂଲ୍ୟଗୁଡ଼ିକ, lineno, col_offset, debug_end_ଧାଡ଼ି, debug_end_offset, p->arena);
}

ପଦସମଷ୍ଟି_ty Py_Pegen_formatted_ମୂଲ୍ୟ(Parser *p, ପଦସମଷ୍ଟି_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_offset,
                                 ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_lineno, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col_offset, PyArena *arena) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    ପଦସମଷ୍ଟି_ty formatted_ମୂଲ୍ୟ = Py_AST_Formatted_ମୂଲ୍ୟ(
        expression, conversion_val, format ? (ପଦସମଷ୍ଟି_ty) format->result : ଖାଲି,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    ଯଦି (!formatted_ମୂଲ୍ୟ || !debug) {
        ଫେରନ୍ତୁ formatted_ମୂଲ୍ୟ;
    }

    /* Find the non whitespace token after the "=" */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା debug_end_ଧାଡ଼ି, debug_end_offset;
    PyObject *debug_metadata;

    ଯଦି (conversion) {
        debug_end_ଧାଡ଼ି = ((ପଦସମଷ୍ଟି_ty) conversion->result)->lineno;
        debug_end_offset = ((ପଦସମଷ୍ଟି_ty) conversion->result)->col_offset;
        debug_metadata = conversion->metadata;
    }
    ଅନ୍ୟଥା ଯଦି (format) {
        debug_end_ଧାଡ଼ି = ((ପଦସମଷ୍ଟି_ty) format->result)->lineno;
        debug_end_offset = ((ପଦସମଷ୍ଟି_ty) format->result)->col_offset + 1;
        debug_metadata = format->metadata;
    }
    ଅନ୍ୟଥା {
        debug_end_ଧାଡ଼ି = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = closing_brace->metadata;
    }
    ପଦସମଷ୍ଟି_ty debug_text = _PyAST_Constant(debug_metadata, ଖାଲି, lineno, col_offset + 1, debug_end_ଧାଡ଼ି,
                                            debug_end_offset - 1, p->arena);
    ଯଦି (!debug_text) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    asdl_ପଦସମଷ୍ଟି_seq *ମୂଲ୍ୟଗୁଡ଼ିକ = Py_asdl_ପଦସମଷ୍ଟି_seq_new(2, arena);
    ଯଦି (!ମୂଲ୍ୟଗୁଡ଼ିକ) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    asdl_seq_SET(ମୂଲ୍ୟଗୁଡ଼ିକ, 0, debug_text);
    asdl_seq_SET(ମୂଲ୍ୟଗୁଡ଼ିକ, 1, formatted_ମୂଲ୍ୟ);
    ଫେରନ୍ତୁ _PyAST_JoinedStr(ମୂଲ୍ୟଗୁଡ଼ିକ, lineno, col_offset, debug_end_ଧାଡ଼ି, debug_end_offset, p->arena);
}

ସ୍ଥିର ପଦସମଷ୍ଟି_ty
_build_concatenated_bytes(Parser *p, asdl_ପଦସମଷ୍ଟି_seq *strings, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno,
                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_lineno, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col_offset,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    ପ୍ରତିଜ୍ଞା(len > 0);

    /* Bytes literals never get a kind, but just for consistency
        since they are represented as Constant nodes, we'll mirror
        the same behavior as unicode strings for determining the
        kind. */
    PyObject *kind = asdl_seq_GET(strings, 0)->v.Constant.kind;

    Py_ssize_t total = 0;
    ପାଇଁ (Py_ssize_t i = 0; i < len; i++) {
        ପଦସମଷ୍ଟି_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.ମୂଲ୍ୟ;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        ଯଦି (part > PY_SSIZE_T_MAX - total) {
            PyErr_NoMemory();
            ଫେରନ୍ତୁ ଖାଲି;
        }
        total += part;
    }

    PyBytesWriter *writer = PyBytesWriter_Create(total);
    ଯଦି (writer == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଅକ୍ଷର *out = PyBytesWriter_GetData(writer);

    ପାଇଁ (Py_ssize_t i = 0; i < len; i++) {
        ପଦସମଷ୍ଟି_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.ମୂଲ୍ୟ;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        ଯଦି (part > 0) {
            memcpy(out, PyBytes_AS_STRING(bytes), part);
            out += part;
        }
    }

    PyObject *res = PyBytesWriter_Finish(writer);
    ଯଦି (res == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (_PyArena_AddPyObject(arena, res) < 0) {
        Py_DECREF(res);
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ _PyAST_Constant(res, kind, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

ସ୍ଥିର ପଦସମଷ୍ଟି_ty
_build_concatenated_unicode(Parser *p, asdl_ପଦସମଷ୍ଟି_seq *strings, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno,
                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_lineno, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col_offset,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    ପ୍ରତିଜ୍ଞା(len > 1);

    ପଦସମଷ୍ଟି_ty first = asdl_seq_GET(strings, 0);

    /* When a string is getting concatenated, the kind of the string
        is determined by the first string in the concatenation
        sequence.

        u"abc" "def" -> u"abcdef"
        "abc" u"abc" ->  "abcabc" */
    PyObject *kind = first->v.Constant.kind;

    PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
    ଯଦି (writer == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    ପାଇଁ (Py_ssize_t i = 0; i < len; i++) {
        ପଦସମଷ୍ଟି_ty current_elem = asdl_seq_GET(strings, i);
        ପ୍ରତିଜ୍ଞା(current_elem->kind == Constant_kind);

        ଯଦି (PyUnicodeWriter_WriteStr(writer,
                                     current_elem->v.Constant.ମୂଲ୍ୟ)) {
            PyUnicodeWriter_Discard(writer);
            ଫେରନ୍ତୁ ଖାଲି;
        }
    }

    PyObject *final = PyUnicodeWriter_Finish(writer);
    ଯଦି (final == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (_PyArena_AddPyObject(p->arena, final) < 0) {
        Py_DECREF(final);
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ _PyAST_Constant(final, kind, lineno, col_offset,
                           end_lineno, end_col_offset, arena);
}

ସ୍ଥିର asdl_ପଦସମଷ୍ଟି_seq *
_build_concatenated_str(Parser *p, asdl_ପଦସମଷ୍ଟି_seq *strings,
                               ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_lineno,
                               ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    ପ୍ରତିଜ୍ଞା(len > 0);

    Py_ssize_t n_flattened_elements = 0;
    ପାଇଁ (Py_ssize_t i = 0; i < len; i++) {
        ପଦସମଷ୍ଟି_ty elem = asdl_seq_GET(strings, i);
        ବାଛ(elem->kind) {
            ସ୍ଥିତି JoinedStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.JoinedStr.ମୂଲ୍ୟଗୁଡ଼ିକ);
                ଭାଙ୍ଗ;
            ସ୍ଥିତି TemplateStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.TemplateStr.ମୂଲ୍ୟଗୁଡ଼ିକ);
                ଭାଙ୍ଗ;
            ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                n_flattened_elements++;
                ଭାଙ୍ଗ;
        }
    }


    asdl_ପଦସମଷ୍ଟି_seq* flattened = Py_asdl_ପଦସମଷ୍ଟି_seq_new(n_flattened_elements, p->arena);
    ଯଦି (flattened == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    /* build flattened list */
    Py_ssize_t current_pos = 0;
    ପାଇଁ (Py_ssize_t i = 0; i < len; i++) {
        ପଦସମଷ୍ଟି_ty elem = asdl_seq_GET(strings, i);
        ବାଛ(elem->kind) {
            ସ୍ଥିତି JoinedStr_kind:
                ପାଇଁ (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.JoinedStr.ମୂଲ୍ୟଗୁଡ଼ିକ); j++) {
                    ପଦସମଷ୍ଟି_ty subvalue = asdl_seq_GET(elem->v.JoinedStr.ମୂଲ୍ୟଗୁଡ଼ିକ, j);
                    ଯଦି (subvalue == ଖାଲି) {
                        ଫେରନ୍ତୁ ଖାଲି;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                ଭାଙ୍ଗ;
            ସ୍ଥିତି TemplateStr_kind:
                ପାଇଁ (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.TemplateStr.ମୂଲ୍ୟଗୁଡ଼ିକ); j++) {
                    ପଦସମଷ୍ଟି_ty subvalue = asdl_seq_GET(elem->v.TemplateStr.ମୂଲ୍ୟଗୁଡ଼ିକ, j);
                    ଯଦି (subvalue == ଖାଲି) {
                        ଫେରନ୍ତୁ ଖାଲି;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                ଭାଙ୍ଗ;
            ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                asdl_seq_SET(flattened, current_pos++, elem);
                ଭାଙ୍ଗ;
        }
    }

    /* calculate folded element count */
    Py_ssize_t n_elements = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_is_constant = 0;
    ପାଇଁ (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        ପଦସମଷ୍ଟି_ty elem = asdl_seq_GET(flattened, i);

        /* The concatenation of a FormattedValue and an empty Constant should
           lead to the FormattedValue itself. Thus, we will not take any empty
           constants into account, just as in `_PyPegen_joined_str` */
        ଯଦି (elem->kind == Constant_kind &&
            PyUnicode_CheckExact(elem->v.Constant.ମୂଲ୍ୟ) &&
            PyUnicode_GET_LENGTH(elem->v.Constant.ମୂଲ୍ୟ) == 0)
            ଜାରି;

        ଯଦି (!prev_is_constant || elem->kind != Constant_kind) {
            n_elements++;
        }
        prev_is_constant = elem->kind == Constant_kind;
    }

    asdl_ପଦସମଷ୍ଟି_seq* ମୂଲ୍ୟଗୁଡ଼ିକ = Py_asdl_ପଦସମଷ୍ଟି_seq_new(n_elements, p->arena);
    ଯଦି (ମୂଲ୍ୟଗୁଡ଼ିକ == ଖାଲି) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    /* build folded list */
    current_pos = 0;
    ପାଇଁ (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        ପଦସମଷ୍ଟି_ty elem = asdl_seq_GET(flattened, i);

        /* if the current elem and the following are constants,
           fold them and all consequent constants */
        ଯଦି (elem->kind == Constant_kind) {
            ଯଦି (i + 1 < n_flattened_elements &&
                asdl_seq_GET(flattened, i + 1)->kind == Constant_kind) {
                ପଦସମଷ୍ଟି_ty first_elem = elem;

                /* When a string is getting concatenated, the kind of the string
                   is determined by the first string in the concatenation
                   sequence.

                   u"abc" "def" -> u"abcdef"
                   "abc" u"abc" ->  "abcabc" */
                PyObject *kind = elem->v.Constant.kind;

                PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
                ଯଦି (writer == ଖାଲି) {
                    ଫେରନ୍ତୁ ଖାଲି;
                }
                ପଦସମଷ୍ଟି_ty last_elem = elem;
                Py_ssize_t j;
                ପାଇଁ (j = i; j < n_flattened_elements; j++) {
                    ପଦସମଷ୍ଟି_ty current_elem = asdl_seq_GET(flattened, j);
                    ଯଦି (current_elem->kind == Constant_kind) {
                        ଯଦି (PyUnicodeWriter_WriteStr(writer,
                                                     current_elem->v.Constant.ମୂଲ୍ୟ)) {
                            PyUnicodeWriter_Discard(writer);
                            ଫେରନ୍ତୁ ଖାଲି;
                        }
                        last_elem = current_elem;
                    } ଅନ୍ୟଥା {
                        ଭାଙ୍ଗ;
                    }
                }
                i = j - 1;

                PyObject *concat_str = PyUnicodeWriter_Finish(writer);
                ଯଦି (concat_str == ଖାଲି) {
                    ଫେରନ୍ତୁ ଖାଲି;
                }
                ଯଦି (_PyArena_AddPyObject(p->arena, concat_str) < 0) {
                    Py_DECREF(concat_str);
                    ଫେରନ୍ତୁ ଖାଲି;
                }
                elem = _PyAST_Constant(concat_str, kind, first_elem->lineno,
                                       first_elem->col_offset,
                                       last_elem->end_lineno,
                                       last_elem->end_col_offset, p->arena);
                ଯଦି (elem == ଖାଲି) {
                    ଫେରନ୍ତୁ ଖାଲି;
                }
            }

            /* Drop all empty contanst strings */
            ଯଦି (PyUnicode_CheckExact(elem->v.Constant.ମୂଲ୍ୟ) &&
                PyUnicode_GET_LENGTH(elem->v.Constant.ମୂଲ୍ୟ) == 0) {
                ଜାରି;
            }
        }

        asdl_seq_SET(ମୂଲ୍ୟଗୁଡ଼ିକ, current_pos++, elem);
    }

    ପ୍ରତିଜ୍ଞା(current_pos == n_elements);
    ଫେରନ୍ତୁ ମୂଲ୍ୟଗୁଡ଼ିକ;
}

ସ୍ଥିର ପଦସମଷ୍ଟି_ty
_build_concatenated_joined_str(Parser *p, asdl_ପଦସମଷ୍ଟି_seq *strings,
                               ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_lineno,
                               ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col_offset, PyArena *arena)
{
    asdl_ପଦସମଷ୍ଟି_seq *ମୂଲ୍ୟଗୁଡ଼ିକ = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    ଯଦି (!ମୂଲ୍ୟଗୁଡ଼ିକ) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ _PyAST_JoinedStr(ମୂଲ୍ୟଗୁଡ଼ିକ, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

ପଦସମଷ୍ଟି_ty
_PyPegen_concatenate_tstrings(Parser *p, asdl_ପଦସମଷ୍ଟି_seq *strings,
                               ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_lineno,
                               ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col_offset, PyArena *arena)
{
    asdl_ପଦସମଷ୍ଟି_seq *ମୂଲ୍ୟଗୁଡ଼ିକ = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    ଯଦି (!ମୂଲ୍ୟଗୁଡ଼ିକ) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ _PyAST_TemplateStr(ମୂଲ୍ୟଗୁଡ଼ିକ, lineno, col_offset, end_lineno,
        end_col_offset, arena);
}

ପଦସମଷ୍ଟି_ty
_PyPegen_concatenate_strings(Parser *p, asdl_ପଦସମଷ୍ଟି_seq *strings,
                             ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_lineno,
                             ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    ପ୍ରତିଜ୍ଞା(len > 0);

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା f_string_found = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା unicode_string_found = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା bytes_found = 0;

    Py_ssize_t i = 0;
    ପାଇଁ (i = 0; i < len; i++) {
        ପଦସମଷ୍ଟି_ty elem = asdl_seq_GET(strings, i);
        ବାଛ(elem->kind) {
            ସ୍ଥିତି Constant_kind:
                ଯଦି (PyBytes_CheckExact(elem->v.Constant.ମୂଲ୍ୟ)) {
                    bytes_found = 1;
                } ଅନ୍ୟଥା {
                    unicode_string_found = 1;
                }
                ଭାଙ୍ଗ;
            ସ୍ଥିତି JoinedStr_kind:
                f_string_found = 1;
                ଭାଙ୍ଗ;
            ସ୍ଥିତି TemplateStr_kind:
                // python.gram handles this; we should never get here
                ପ୍ରତିଜ୍ଞା(0);
                ଭାଙ୍ଗ;
            ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                f_string_found = 1;
                ଭାଙ୍ଗ;
        }
    }

    // Cannot mix unicode and bytes
    ଯଦି ((unicode_string_found || f_string_found) && bytes_found) {
        RAISE_SYNTAX_ERROR("cannot mix bytes and nonbytes literals");
        ଫେରନ୍ତୁ ଖାଲି;
    }

    // If it's only bytes or only unicode string, do a simple concat
    ଯଦି (!f_string_found) {
        ଯଦି (len == 1) {
            ଫେରନ୍ତୁ asdl_seq_GET(strings, 0);
        }
        ଅନ୍ୟଥା ଯଦି (bytes_found) {
            ଫେରନ୍ତୁ _build_concatenated_bytes(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
        ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ _build_concatenated_unicode(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
    }

    ଫେରନ୍ତୁ _build_concatenated_joined_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_warn_relative_import_of_lazy(Parser *p, asdl_seq *dots, ପଦସମଷ୍ଟି_ty module)
{
    // Warn about `from . lazy import x`: the whitespace between the dots and
    // the module name is insignificant, so this is parsed exactly like
    // `from .lazy import x` (an import of the relative module "lazy"), but it
    // is most likely a transposition of `lazy from . import x` (PEP 810).
    ଯଦି (p->call_invalid_rules) {
        ଫେରନ୍ତୁ 0;
    }

    // Only fire if there is whitespace between the last dot and the name,
    // i.e. not for the common `from .lazy import x` spelling.
    Token *last_dot = asdl_seq_GET_UNTYPED(dots, asdl_seq_LEN(dots) - 1);
    ଯଦି (
        last_dot->end_lineno == module->lineno
        && last_dot->end_col_offset == module->col_offset
    ) {
        ଫେରନ୍ତୁ 0;
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2 = Py_Pegen_seq_ଗଣନା_dots(dots);
    ଅକ୍ଷର *buf = PyMem_RawMalloc(ଗଣନା_2 + 1);
    ଯଦି (buf == ଖାଲି) {
        PyErr_NoMemory();
        ଫେରନ୍ତୁ -1;
    }
    memset(buf, '.', ଗଣନା_2);
    buf[ଗଣନା_2] = '\0';

    PyObject *ସନ୍ଦେଶ = PyUnicode_FromFormat(
        "'from %s lazy import' is the same as 'from %slazy import'; "
        "did you mean 'lazy from %s import'?",
        buf, buf, buf);
    PyMem_RawFree(buf);
    ଯଦି (ସନ୍ଦେଶ == ଖାଲି) {
        ଫେରନ୍ତୁ -1;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା res = _PyErr_EmitSyntaxWarning(ସନ୍ଦେଶ,
                                       info.filename,
                                       module->lineno,
                                       module->col_offset + 1,
                                       module->end_lineno,
                                       module->end_col_offset + 1,
                                       info.module);
    Py_DECREF(ସନ୍ଦେଶ);
    ଫେରନ୍ତୁ res;
}

stmt_ty
_PyPegen_checked_from_import(Parser *p, asdl_seq *dots, ପଦସମଷ୍ଟି_ty module_name,
                             asdl_alias_seq *names, ପଦସମଷ୍ଟି_ty lazy_token, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno,
                             ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_lineno, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col_offset,
                             PyArena *arena)
{
    identifier module = module_name->v.Name.id;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା level = Py_Pegen_seq_ଗଣନା_dots(dots);
    ଯଦି (level == 0 && PyUnicode_CompareWithASCIIString(module, "__future__") == 0) {
        ଯଦି (lazy_token) {
            RAISE_SYNTAX_ERROR_KNOWN_LOCATION(lazy_token,
                "lazy from __future__ import is not allowed");
            ଫେରନ୍ତୁ ଖାଲି;
        }
        ପାଇଁ (Py_ssize_t i = 0; i < asdl_seq_LEN(names); i++) {
            alias_ty alias = asdl_seq_GET(names, i);
            ଯଦି (PyUnicode_CompareWithASCIIString(alias->name, "barry_as_FLUFL") == 0) {
                p->flags |= PyPARSE_BARRY_AS_BDFL;
                p->tok->barry_as_bdfl = 1;
            }
        }
    }
    ଅନ୍ୟଥା ଯଦି (
        level > 0
        && lazy_token == ଖାଲି
        && PyUnicode_CompareWithASCIIString(module, "lazy") == 0
    ) {
        ଯଦି (_warn_relative_import_of_lazy(p, dots, module_name) < 0) {
            ଫେରନ୍ତୁ ଖାଲି;
        }
    }
    ଫେରନ୍ତୁ _PyAST_ImportFrom(module, names, level, lazy_token ? 1 : 0, lineno,
                             col_offset, end_lineno, end_col_offset, arena);
}

asdl_stmt_seq*
_PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts) {
    ଯଦି (!p->call_invalid_rules) {
        ଫେରନ୍ତୁ stmts;
    }
    Py_ssize_t len = asdl_seq_LEN(stmts);
    ଯଦି (len == 0) {
        ଫେରନ୍ତୁ stmts;
    }
    stmt_ty last_stmt = asdl_seq_GET(stmts, len - 1);
    ଯଦି (p->last_stmt_location.lineno > last_stmt->lineno) {
        ଫେରନ୍ତୁ stmts;
    }
    p->last_stmt_location.lineno = last_stmt->lineno;
    p->last_stmt_location.col_offset = last_stmt->col_offset;
    p->last_stmt_location.end_lineno = last_stmt->end_lineno;
    p->last_stmt_location.end_col_offset = last_stmt->end_col_offset;
    ଫେରନ୍ତୁ stmts;
}
