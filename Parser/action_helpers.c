#അന്തര്ഭാവ <Python.h>
#അന്തര്ഭാവ "pycore_pystate.h"         // _PyInterpreterState_GET()
#അന്തര്ഭാവ "pycore_runtime.h"         // _PyRuntime
#അന്തര്ഭാവ "pycore_unicodeobject.h"   // _PyUnicode_InternImmortal()
#അന്തര്ഭാവ "pycore_pyerrors.h"        // _PyErr_EmitSyntaxWarning()

#അന്തര്ഭാവ "pegen.h"
#അന്തര്ഭാവ "string_parser.h"          // _PyPegen_decode_string()
#അന്തര്ഭാവ "lexer/state.h"            // tok_state


ശൂന്യം *
_PyPegen_dummy_name(Parser *p, ...)
{
    തിരികെ &_PyRuntime.parser.dummy_name;
}

/* Creates a single-element asdl_seq* that contains a */
asdl_seq *
_PyPegen_singleton_seq(Parser *p, ശൂന്യം *a)
{
    പ്രതിജ്ഞാ(a != ശൂന്യ);
    asdl_seq *seq = (asdl_seq*)_Py_asdl_generic_seq_new(1, p->arena);
    എങ്കിൽ (!seq) {
        തിരികെ ശൂന്യ;
    }
    asdl_seq_SET_UNTYPED(seq, 0, a);
    തിരികെ seq;
}

/* Creates a copy of seq and prepends a to it */
asdl_seq *
Py_Pegen_seq_തിരുകുക_in_front(Parser *p, ശൂന്യം *a, asdl_seq *seq)
{
    പ്രതിജ്ഞാ(a != ശൂന്യ);
    എങ്കിൽ (!seq) {
        തിരികെ _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    എങ്കിൽ (!new_seq) {
        തിരികെ ശൂന്യ;
    }

    asdl_seq_SET_UNTYPED(new_seq, 0, a);
    വേണ്ടി (Py_ssize_t i = 1, l = asdl_seq_LEN(new_seq); i < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i - 1));
    }
    തിരികെ new_seq;
}

/* Creates a copy of seq and appends a to it */
asdl_seq *
_PyPegen_seq_append_to_end(Parser *p, asdl_seq *seq, ശൂന്യം *a)
{
    പ്രതിജ്ഞാ(a != ശൂന്യ);
    എങ്കിൽ (!seq) {
        തിരികെ _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    എങ്കിൽ (!new_seq) {
        തിരികെ ശൂന്യ;
    }

    വേണ്ടി (Py_ssize_t i = 0, l = asdl_seq_LEN(new_seq); i + 1 < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i));
    }
    asdl_seq_SET_UNTYPED(new_seq, asdl_seq_LEN(new_seq) - 1, a);
    തിരികെ new_seq;
}

സ്ഥിതം Py_ssize_t
_get_flattened_seq_size(asdl_seq *seqs)
{
    Py_ssize_t size = 0;
    വേണ്ടി (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        size += asdl_seq_LEN(inner_seq);
    }
    തിരികെ size;
}

/* Flattens an asdl_seq* of asdl_seq*s */
asdl_seq *
_PyPegen_seq_flatten(Parser *p, asdl_seq *seqs)
{
    Py_ssize_t flattened_seq_size = _get_flattened_seq_size(seqs);
    പ്രതിജ്ഞാ(flattened_seq_size > 0);

    asdl_seq *flattened_seq = (asdl_seq*)_Py_asdl_generic_seq_new(flattened_seq_size, p->arena);
    എങ്കിൽ (!flattened_seq) {
        തിരികെ ശൂന്യ;
    }

    പൂർണ്ണസംഖ്യ flattened_seq_idx = 0;
    വേണ്ടി (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        വേണ്ടി (Py_ssize_t j = 0, li = asdl_seq_LEN(inner_seq); j < li; j++) {
            asdl_seq_SET_UNTYPED(flattened_seq, flattened_seq_idx++, asdl_seq_GET_UNTYPED(inner_seq, j));
        }
    }
    പ്രതിജ്ഞാ(flattened_seq_idx == flattened_seq_size);

    തിരികെ flattened_seq;
}

ശൂന്യം *
_PyPegen_seq_last_item(asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    തിരികെ asdl_seq_GET_UNTYPED(seq, len - 1);
}

ശൂന്യം *
_PyPegen_seq_first_item(asdl_seq *seq)
{
    തിരികെ asdl_seq_GET_UNTYPED(seq, 0);
}

/* Creates a new name of the form <first_name>.<second_name> */
വ്യഞ്ജകം_ty
_PyPegen_join_names_with_dot(Parser *p, വ്യഞ്ജകം_ty first_name, വ്യഞ്ജകം_ty second_name)
{
    പ്രതിജ്ഞാ(first_name != ശൂന്യ && second_name != ശൂന്യ);
    PyObject *uni = PyUnicode_FromFormat("%U.%U",
            first_name->v.Name.id, second_name->v.Name.id);
    എങ്കിൽ (!uni) {
        തിരികെ ശൂന്യ;
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &uni);
    എങ്കിൽ (_PyArena_AddPyObject(p->arena, uni) < 0) {
        Py_DECREF(uni);
        തിരികെ ശൂന്യ;
    }

    തിരികെ _PyAST_Name(uni, Load, EXTRA_വ്യഞ്ജകം(first_name, second_name));
}

/* Counts the total number of dots in seq's tokens */
പൂർണ്ണസംഖ്യ
Py_Pegen_seq_എണ്ണം_dots(asdl_seq *seq)
{
    പൂർണ്ണസംഖ്യ സംഖ്യ_of_dots = 0;
    വേണ്ടി (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        Token *current_വ്യഞ്ജകം = asdl_seq_GET_UNTYPED(seq, i);
        തിരഞ്ഞെടുപ്പ് (current_വ്യഞ്ജകം->type) {
            അവസ്ഥ ELLIPSIS:
                സംഖ്യ_of_dots += 3;
                നിർത്തുക;
            അവസ്ഥ DOT:
                സംഖ്യ_of_dots += 1;
                നിർത്തുക;
            സ്ഥിരസ്ഥിതി:
                Py_UNREACHABLE();
        }
    }

    തിരികെ സംഖ്യ_of_dots;
}

/* Creates an alias with '*' as the identifier name */
alias_ty
_PyPegen_alias_for_star(Parser *p, പൂർണ്ണസംഖ്യ lineno, പൂർണ്ണസംഖ്യ col_offset, പൂർണ്ണസംഖ്യ end_lineno,
                        പൂർണ്ണസംഖ്യ end_col_offset, PyArena *arena) {
    PyObject *str = PyUnicode_InternFromString("*");
    എങ്കിൽ (!str) {
        തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        തിരികെ ശൂന്യ;
    }
    തിരികെ _PyAST_alias(str, ശൂന്യ, lineno, col_offset, end_lineno, end_col_offset, arena);
}

/* Creates a new asdl_seq* with the identifiers of all the names in seq */
asdl_identifier_seq *
_PyPegen_map_names_to_ids(Parser *p, asdl_വ്യഞ്ജകം_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    പ്രതിജ്ഞാ(len > 0);

    asdl_identifier_seq *new_seq = _Py_asdl_identifier_seq_new(len, p->arena);
    എങ്കിൽ (!new_seq) {
        തിരികെ ശൂന്യ;
    }
    വേണ്ടി (Py_ssize_t i = 0; i < len; i++) {
        വ്യഞ്ജകം_ty e = asdl_seq_GET(seq, i);
        asdl_seq_SET(new_seq, i, e->v.Name.id);
    }
    തിരികെ new_seq;
}

/* Constructs a CmpopExprPair */
Cmpop_വ്യഞ്ജകം_Pair *
Py_Pegen_cmpop_വ്യഞ്ജകം_pair(Parser *p, cmpop_ty cmpop, വ്യഞ്ജകം_ty വ്യഞ്ജകം)
{
    പ്രതിജ്ഞാ(വ്യഞ്ജകം != ശൂന്യ);
    Cmpop_വ്യഞ്ജകം_Pair *a = _PyArena_Malloc(p->arena, വലുപ്പം(Cmpop_വ്യഞ്ജകം_Pair));
    എങ്കിൽ (!a) {
        തിരികെ ശൂന്യ;
    }
    a->cmpop = cmpop;
    a->വ്യഞ്ജകം = വ്യഞ്ജകം;
    തിരികെ a;
}

asdl_int_seq *
_PyPegen_get_cmpops(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    പ്രതിജ്ഞാ(len > 0);

    asdl_int_seq *new_seq = _Py_asdl_int_seq_new(len, p->arena);
    എങ്കിൽ (!new_seq) {
        തിരികെ ശൂന്യ;
    }
    വേണ്ടി (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_വ്യഞ്ജകം_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->cmpop);
    }
    തിരികെ new_seq;
}

asdl_വ്യഞ്ജകം_seq *
_PyPegen_get_exprs(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    പ്രതിജ്ഞാ(len > 0);

    asdl_വ്യഞ്ജകം_seq *new_seq = Py_asdl_വ്യഞ്ജകം_seq_new(len, p->arena);
    എങ്കിൽ (!new_seq) {
        തിരികെ ശൂന്യ;
    }
    വേണ്ടി (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_വ്യഞ്ജകം_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->വ്യഞ്ജകം);
    }
    തിരികെ new_seq;
}

/* Creates an asdl_seq* where all the elements have been changed to have ctx as context */
സ്ഥിതം asdl_വ്യഞ്ജകം_seq *
_set_seq_context(Parser *p, asdl_വ്യഞ്ജകം_seq *seq, വ്യഞ്ജകം_context_ty ctx)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    എങ്കിൽ (len == 0) {
        തിരികെ ശൂന്യ;
    }

    asdl_വ്യഞ്ജകം_seq *new_seq = Py_asdl_വ്യഞ്ജകം_seq_new(len, p->arena);
    എങ്കിൽ (!new_seq) {
        തിരികെ ശൂന്യ;
    }
    വേണ്ടി (Py_ssize_t i = 0; i < len; i++) {
        വ്യഞ്ജകം_ty e = asdl_seq_GET(seq, i);
        വ്യഞ്ജകം_ty new_e = Py_Pegen_set_വ്യഞ്ജകം_context(p, e, ctx);
        എങ്കിൽ (!new_e) {
            തിരികെ ശൂന്യ;
        }
        asdl_seq_SET(new_seq, i, new_e);
    }
    തിരികെ new_seq;
}

സ്ഥിതം വ്യഞ്ജകം_ty
_set_name_context(Parser *p, വ്യഞ്ജകം_ty e, വ്യഞ്ജകം_context_ty ctx)
{
    തിരികെ _PyAST_Name(e->v.Name.id, ctx, EXTRA_വ്യഞ്ജകം(e, e));
}

സ്ഥിതം വ്യഞ്ജകം_ty
_set_tuple_context(Parser *p, വ്യഞ്ജകം_ty e, വ്യഞ്ജകം_context_ty ctx)
{
    asdl_വ്യഞ്ജകം_seq *seq = _set_seq_context(p, e->v.Tuple.elts, ctx);
    എങ്കിൽ (!seq && PyErr_Occurred()) {
        തിരികെ ശൂന്യ;
    }
    തിരികെ _PyAST_Tuple(seq, ctx, EXTRA_വ്യഞ്ജകം(e, e));
}

സ്ഥിതം വ്യഞ്ജകം_ty
_set_list_context(Parser *p, വ്യഞ്ജകം_ty e, വ്യഞ്ജകം_context_ty ctx)
{
    asdl_വ്യഞ്ജകം_seq *seq = _set_seq_context(p, e->v.List.elts, ctx);
    എങ്കിൽ (!seq && PyErr_Occurred()) {
        തിരികെ ശൂന്യ;
    }
    തിരികെ _PyAST_List(seq, ctx, EXTRA_വ്യഞ്ജകം(e, e));
}

സ്ഥിതം വ്യഞ്ജകം_ty
_set_subscript_context(Parser *p, വ്യഞ്ജകം_ty e, വ്യഞ്ജകം_context_ty ctx)
{
    തിരികെ _PyAST_Subscript(e->v.Subscript.മൂല്യം, e->v.Subscript.slice,
                            ctx, EXTRA_വ്യഞ്ജകം(e, e));
}

സ്ഥിതം വ്യഞ്ജകം_ty
_set_attribute_context(Parser *p, വ്യഞ്ജകം_ty e, വ്യഞ്ജകം_context_ty ctx)
{
    തിരികെ _PyAST_Attribute(e->v.Attribute.മൂല്യം, e->v.Attribute.attr,
                            ctx, EXTRA_വ്യഞ്ജകം(e, e));
}

സ്ഥിതം വ്യഞ്ജകം_ty
_set_starred_context(Parser *p, വ്യഞ്ജകം_ty e, വ്യഞ്ജകം_context_ty ctx)
{
    വ്യഞ്ജകം_ty inner = Py_Pegen_set_വ്യഞ്ജകം_context(p, e->v.Starred.മൂല്യം, ctx);
    എങ്കിൽ (!inner) {
        തിരികെ ശൂന്യ;
    }
    തിരികെ _PyAST_Starred(inner, ctx, EXTRA_വ്യഞ്ജകം(e, e));
}

/* Creates an `expr_ty` equivalent to `expr` but with `ctx` as context */
വ്യഞ്ജകം_ty
Py_Pegen_set_വ്യഞ്ജകം_context(Parser *p, വ്യഞ്ജകം_ty വ്യഞ്ജകം, വ്യഞ്ജകം_context_ty ctx)
{
    പ്രതിജ്ഞാ(വ്യഞ്ജകം != ശൂന്യ);

    വ്യഞ്ജകം_ty new = ശൂന്യ;
    തിരഞ്ഞെടുപ്പ് (വ്യഞ്ജകം->kind) {
        അവസ്ഥ Name_kind:
            new = _set_name_context(p, വ്യഞ്ജകം, ctx);
            നിർത്തുക;
        അവസ്ഥ Tuple_kind:
            new = _set_tuple_context(p, വ്യഞ്ജകം, ctx);
            നിർത്തുക;
        അവസ്ഥ List_kind:
            new = _set_list_context(p, വ്യഞ്ജകം, ctx);
            നിർത്തുക;
        അവസ്ഥ Subscript_kind:
            new = _set_subscript_context(p, വ്യഞ്ജകം, ctx);
            നിർത്തുക;
        അവസ്ഥ Attribute_kind:
            new = _set_attribute_context(p, വ്യഞ്ജകം, ctx);
            നിർത്തുക;
        അവസ്ഥ Starred_kind:
            new = _set_starred_context(p, വ്യഞ്ജകം, ctx);
            നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
            new = വ്യഞ്ജകം;
    }
    തിരികെ new;
}

/* Invalid targets return NULL without raising, allowing parsing to backtrack.
   Copy valid targets to preserve the memoized expression's context. */
വ്യഞ്ജകം_ty
_PyPegen_make_target(Parser *p, വ്യഞ്ജകം_ty വ്യഞ്ജകം, TARGETS_TYPE targets_type)
{
    പ്രതിജ്ഞാ(വ്യഞ്ജകം != ശൂന്യ);
    പ്രതിജ്ഞാ(targets_type != FOR_TARGETS);
    എങ്കിൽ (_PyPegen_get_invalid_target(വ്യഞ്ജകം, targets_type) != ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }
    തിരികെ Py_Pegen_set_വ്യഞ്ജകം_context(
        p, വ്യഞ്ജകം, targets_type == DEL_TARGETS ? Del : Store);
}

/* Constructs a KeyValuePair that is used when parsing a dict's key value pairs */
താക്കോൽ_മൂല്യം_Pair *
Py_Pegen_താക്കോൽ_മൂല്യം_pair(Parser *p, വ്യഞ്ജകം_ty താക്കോൽ, വ്യഞ്ജകം_ty മൂല്യം)
{
    താക്കോൽ_മൂല്യം_Pair *a = _PyArena_Malloc(p->arena, വലുപ്പം(താക്കോൽ_മൂല്യം_Pair));
    എങ്കിൽ (!a) {
        തിരികെ ശൂന്യ;
    }
    a->താക്കോൽ = താക്കോൽ;
    a->മൂല്യം = മൂല്യം;
    തിരികെ a;
}

/* Extracts all keys from an asdl_seq* of KeyValuePair*'s */
asdl_വ്യഞ്ജകം_seq *
Py_Pegen_get_താക്കോലുകൾ(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_വ്യഞ്ജകം_seq *new_seq = Py_asdl_വ്യഞ്ജകം_seq_new(len, p->arena);
    എങ്കിൽ (!new_seq) {
        തിരികെ ശൂന്യ;
    }
    വേണ്ടി (Py_ssize_t i = 0; i < len; i++) {
        താക്കോൽ_മൂല്യം_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->താക്കോൽ);
    }
    തിരികെ new_seq;
}

/* Extracts all values from an asdl_seq* of KeyValuePair*'s */
asdl_വ്യഞ്ജകം_seq *
Py_Pegen_get_മൂല്യങ്ങൾ(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_വ്യഞ്ജകം_seq *new_seq = Py_asdl_വ്യഞ്ജകം_seq_new(len, p->arena);
    എങ്കിൽ (!new_seq) {
        തിരികെ ശൂന്യ;
    }
    വേണ്ടി (Py_ssize_t i = 0; i < len; i++) {
        താക്കോൽ_മൂല്യം_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->മൂല്യം);
    }
    തിരികെ new_seq;
}

/* Constructs a KeyPatternPair that is used when parsing mapping & class patterns */
താക്കോൽ_Pattern_Pair *
Py_Pegen_താക്കോൽ_pattern_pair(Parser *p, വ്യഞ്ജകം_ty താക്കോൽ, pattern_ty pattern)
{
    താക്കോൽ_Pattern_Pair *a = _PyArena_Malloc(p->arena, വലുപ്പം(താക്കോൽ_Pattern_Pair));
    എങ്കിൽ (!a) {
        തിരികെ ശൂന്യ;
    }
    a->താക്കോൽ = താക്കോൽ;
    a->pattern = pattern;
    തിരികെ a;
}

/* Extracts all keys from an asdl_seq* of KeyPatternPair*'s */
asdl_വ്യഞ്ജകം_seq *
Py_Pegen_get_pattern_താക്കോലുകൾ(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_വ്യഞ്ജകം_seq *new_seq = Py_asdl_വ്യഞ്ജകം_seq_new(len, p->arena);
    എങ്കിൽ (!new_seq) {
        തിരികെ ശൂന്യ;
    }
    വേണ്ടി (Py_ssize_t i = 0; i < len; i++) {
        താക്കോൽ_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->താക്കോൽ);
    }
    തിരികെ new_seq;
}

/* Extracts all patterns from an asdl_seq* of KeyPatternPair*'s */
asdl_pattern_seq *
_PyPegen_get_patterns(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_pattern_seq *new_seq = _Py_asdl_pattern_seq_new(len, p->arena);
    എങ്കിൽ (!new_seq) {
        തിരികെ ശൂന്യ;
    }
    വേണ്ടി (Py_ssize_t i = 0; i < len; i++) {
        താക്കോൽ_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->pattern);
    }
    തിരികെ new_seq;
}

/* Constructs a NameDefaultPair */
NameDefaultPair *
_PyPegen_name_default_pair(Parser *p, arg_ty arg, വ്യഞ്ജകം_ty മൂല്യം, Token *tc)
{
    NameDefaultPair *a = _PyArena_Malloc(p->arena, വലുപ്പം(NameDefaultPair));
    എങ്കിൽ (!a) {
        തിരികെ ശൂന്യ;
    }
    a->arg = _PyPegen_add_type_comment_to_arg(p, arg, tc);
    എങ്കിൽ (!a->arg) {
        തിരികെ ശൂന്യ;
    }
    a->മൂല്യം = മൂല്യം;
    തിരികെ a;
}

/* Constructs a SlashWithDefault */
SlashWithDefault *
_PyPegen_slash_with_default(Parser *p, asdl_arg_seq *plain_names, asdl_seq *names_with_defaults)
{
    SlashWithDefault *a = _PyArena_Malloc(p->arena, വലുപ്പം(SlashWithDefault));
    എങ്കിൽ (!a) {
        തിരികെ ശൂന്യ;
    }
    a->plain_names = plain_names;
    a->names_with_defaults = names_with_defaults;
    തിരികെ a;
}

/* Constructs a StarEtc */
StarEtc *
_PyPegen_star_etc(Parser *p, arg_ty vararg, asdl_seq *kwonlyargs, arg_ty kwarg)
{
    StarEtc *a = _PyArena_Malloc(p->arena, വലുപ്പം(StarEtc));
    എങ്കിൽ (!a) {
        തിരികെ ശൂന്യ;
    }
    a->vararg = vararg;
    a->kwonlyargs = kwonlyargs;
    a->kwarg = kwarg;
    തിരികെ a;
}

asdl_seq *
_PyPegen_join_sequences(Parser *p, asdl_seq *a, asdl_seq *b)
{
    Py_ssize_t first_len = asdl_seq_LEN(a);
    Py_ssize_t second_len = asdl_seq_LEN(b);
    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(first_len + second_len, p->arena);
    എങ്കിൽ (!new_seq) {
        തിരികെ ശൂന്യ;
    }

    പൂർണ്ണസംഖ്യ k = 0;
    വേണ്ടി (Py_ssize_t i = 0; i < first_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(a, i));
    }
    വേണ്ടി (Py_ssize_t i = 0; i < second_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(b, i));
    }

    തിരികെ new_seq;
}

സ്ഥിതം asdl_arg_seq*
_get_names(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_arg_seq *seq = _Py_asdl_arg_seq_new(len, p->arena);
    എങ്കിൽ (!seq) {
        തിരികെ ശൂന്യ;
    }
    വേണ്ടി (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->arg);
    }
    തിരികെ seq;
}

സ്ഥിതം asdl_വ്യഞ്ജകം_seq *
_get_defaults(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_വ്യഞ്ജകം_seq *seq = Py_asdl_വ്യഞ്ജകം_seq_new(len, p->arena);
    എങ്കിൽ (!seq) {
        തിരികെ ശൂന്യ;
    }
    വേണ്ടി (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->മൂല്യം);
    }
    തിരികെ seq;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
_make_posonlyargs(Parser *p,
                  asdl_arg_seq *slash_without_default,
                  SlashWithDefault *slash_with_default,
                  asdl_arg_seq **posonlyargs) {
    എങ്കിൽ (slash_without_default != ശൂന്യ) {
        *posonlyargs = slash_without_default;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (slash_with_default != ശൂന്യ) {
        asdl_arg_seq *slash_with_default_names =
                _get_names(p, slash_with_default->names_with_defaults);
        എങ്കിൽ (!slash_with_default_names) {
            തിരികെ -1;
        }
        *posonlyargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default->plain_names,
                (asdl_seq*)slash_with_default_names);
    }
    അല്ലെങ്കിൽ {
        *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }
    തിരികെ *posonlyargs == ശൂന്യ ? -1 : 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
_make_posargs(Parser *p,
              asdl_arg_seq *plain_names,
              asdl_seq *names_with_default,
              asdl_arg_seq **posargs) {

    എങ്കിൽ (names_with_default != ശൂന്യ) {
        എങ്കിൽ (plain_names != ശൂന്യ) {
            asdl_arg_seq *names_with_default_names = _get_names(p, names_with_default);
            എങ്കിൽ (!names_with_default_names) {
                തിരികെ -1;
            }
            *posargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                    p,(asdl_seq*)plain_names, (asdl_seq*)names_with_default_names);
        }
        അല്ലെങ്കിൽ {
            *posargs = _get_names(p, names_with_default);
        }
    }
    അല്ലെങ്കിൽ {
        എങ്കിൽ (plain_names != ശൂന്യ) {
            // With the current grammar, we never get here.
            // If that has changed, remove the assert, and test thoroughly.
            പ്രതിജ്ഞാ(0);
            *posargs = plain_names;
        }
        അല്ലെങ്കിൽ {
            *posargs = _Py_asdl_arg_seq_new(0, p->arena);
        }
    }
    തിരികെ *posargs == ശൂന്യ ? -1 : 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
_make_posdefaults(Parser *p,
                  SlashWithDefault *slash_with_default,
                  asdl_seq *names_with_default,
                  asdl_വ്യഞ്ജകം_seq **posdefaults) {
    എങ്കിൽ (slash_with_default != ശൂന്യ && names_with_default != ശൂന്യ) {
        asdl_വ്യഞ്ജകം_seq *slash_with_default_മൂല്യങ്ങൾ =
                _get_defaults(p, slash_with_default->names_with_defaults);
        എങ്കിൽ (!slash_with_default_മൂല്യങ്ങൾ) {
            തിരികെ -1;
        }
        asdl_വ്യഞ്ജകം_seq *names_with_default_മൂല്യങ്ങൾ = _get_defaults(p, names_with_default);
        എങ്കിൽ (!names_with_default_മൂല്യങ്ങൾ) {
            തിരികെ -1;
        }
        *posdefaults = (asdl_വ്യഞ്ജകം_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default_മൂല്യങ്ങൾ,
                (asdl_seq*)names_with_default_മൂല്യങ്ങൾ);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (slash_with_default == ശൂന്യ && names_with_default != ശൂന്യ) {
        *posdefaults = _get_defaults(p, names_with_default);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (slash_with_default != ശൂന്യ && names_with_default == ശൂന്യ) {
        *posdefaults = _get_defaults(p, slash_with_default->names_with_defaults);
    }
    അല്ലെങ്കിൽ {
        *posdefaults = Py_asdl_വ്യഞ്ജകം_seq_new(0, p->arena);
    }
    തിരികെ *posdefaults == ശൂന്യ ? -1 : 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
_make_kwargs(Parser *p, StarEtc *star_etc,
             asdl_arg_seq **kwonlyargs,
             asdl_വ്യഞ്ജകം_seq **kwdefaults) {
    എങ്കിൽ (star_etc != ശൂന്യ && star_etc->kwonlyargs != ശൂന്യ) {
        *kwonlyargs = _get_names(p, star_etc->kwonlyargs);
    }
    അല്ലെങ്കിൽ {
        *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }

    എങ്കിൽ (*kwonlyargs == ശൂന്യ) {
        തിരികെ -1;
    }

    എങ്കിൽ (star_etc != ശൂന്യ && star_etc->kwonlyargs != ശൂന്യ) {
        *kwdefaults = _get_defaults(p, star_etc->kwonlyargs);
    }
    അല്ലെങ്കിൽ {
        *kwdefaults = Py_asdl_വ്യഞ്ജകം_seq_new(0, p->arena);
    }

    എങ്കിൽ (*kwdefaults == ശൂന്യ) {
        തിരികെ -1;
    }

    തിരികെ 0;
}

/* Constructs an arguments_ty object out of all the parsed constructs in the parameters rule */
arguments_ty
_PyPegen_make_arguments(Parser *p, asdl_arg_seq *slash_without_default,
                        SlashWithDefault *slash_with_default, asdl_arg_seq *plain_names,
                        asdl_seq *names_with_default, StarEtc *star_etc)
{
    asdl_arg_seq *posonlyargs;
    എങ്കിൽ (_make_posonlyargs(p, slash_without_default, slash_with_default, &posonlyargs) == -1) {
        തിരികെ ശൂന്യ;
    }

    asdl_arg_seq *posargs;
    എങ്കിൽ (_make_posargs(p, plain_names, names_with_default, &posargs) == -1) {
        തിരികെ ശൂന്യ;
    }

    asdl_വ്യഞ്ജകം_seq *posdefaults;
    എങ്കിൽ (_make_posdefaults(p,slash_with_default, names_with_default, &posdefaults) == -1) {
        തിരികെ ശൂന്യ;
    }

    arg_ty vararg = ശൂന്യ;
    എങ്കിൽ (star_etc != ശൂന്യ && star_etc->vararg != ശൂന്യ) {
        vararg = star_etc->vararg;
    }

    asdl_arg_seq *kwonlyargs;
    asdl_വ്യഞ്ജകം_seq *kwdefaults;
    എങ്കിൽ (_make_kwargs(p, star_etc, &kwonlyargs, &kwdefaults) == -1) {
        തിരികെ ശൂന്യ;
    }

    arg_ty kwarg = ശൂന്യ;
    എങ്കിൽ (star_etc != ശൂന്യ && star_etc->kwarg != ശൂന്യ) {
        kwarg = star_etc->kwarg;
    }

    തിരികെ _PyAST_arguments(posonlyargs, posargs, vararg, kwonlyargs,
                            kwdefaults, kwarg, posdefaults, p->arena);
}


/* Constructs an empty arguments_ty object, that gets used when a function accepts no
 * arguments. */
arguments_ty
_PyPegen_empty_arguments(Parser *p)
{
    asdl_arg_seq *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    എങ്കിൽ (!posonlyargs) {
        തിരികെ ശൂന്യ;
    }
    asdl_arg_seq *posargs = _Py_asdl_arg_seq_new(0, p->arena);
    എങ്കിൽ (!posargs) {
        തിരികെ ശൂന്യ;
    }
    asdl_വ്യഞ്ജകം_seq *posdefaults = Py_asdl_വ്യഞ്ജകം_seq_new(0, p->arena);
    എങ്കിൽ (!posdefaults) {
        തിരികെ ശൂന്യ;
    }
    asdl_arg_seq *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    എങ്കിൽ (!kwonlyargs) {
        തിരികെ ശൂന്യ;
    }
    asdl_വ്യഞ്ജകം_seq *kwdefaults = Py_asdl_വ്യഞ്ജകം_seq_new(0, p->arena);
    എങ്കിൽ (!kwdefaults) {
        തിരികെ ശൂന്യ;
    }

    തിരികെ _PyAST_arguments(posonlyargs, posargs, ശൂന്യ, kwonlyargs,
                            kwdefaults, ശൂന്യ, posdefaults, p->arena);
}

/* Encapsulates the value of an operator_ty into an AugOperator struct */
AugOperator *
_PyPegen_augoperator(Parser *p, operator_ty kind)
{
    AugOperator *a = _PyArena_Malloc(p->arena, വലുപ്പം(AugOperator));
    എങ്കിൽ (!a) {
        തിരികെ ശൂന്യ;
    }
    a->kind = kind;
    തിരികെ a;
}

/* Construct a FunctionDef equivalent to function_def, but with decorators */
stmt_ty
_PyPegen_function_def_decorators(Parser *p, asdl_വ്യഞ്ജകം_seq *decorators, stmt_ty function_def)
{
    പ്രതിജ്ഞാ(function_def != ശൂന്യ);
    എങ്കിൽ (function_def->kind == AsyncFunctionDef_kind) {
        തിരികെ _PyAST_AsyncFunctionDef(
            function_def->v.AsyncFunctionDef.name,
            function_def->v.AsyncFunctionDef.args,
            function_def->v.AsyncFunctionDef.body, decorators,
            function_def->v.AsyncFunctionDef.returns,
            function_def->v.AsyncFunctionDef.type_comment,
            function_def->v.AsyncFunctionDef.type_params,
            function_def->lineno, function_def->col_offset,
            function_def->end_lineno, function_def->end_col_offset, p->arena);
    }

    തിരികെ _PyAST_FunctionDef(
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
_PyPegen_class_def_decorators(Parser *p, asdl_വ്യഞ്ജകം_seq *decorators, stmt_ty class_def)
{
    പ്രതിജ്ഞാ(class_def != ശൂന്യ);
    തിരികെ _PyAST_ClassDef(
        class_def->v.ClassDef.name,
        class_def->v.ClassDef.bases, class_def->v.ClassDef.keywords,
        class_def->v.ClassDef.body, decorators,
        class_def->v.ClassDef.type_params,
        class_def->lineno, class_def->col_offset, class_def->end_lineno,
        class_def->end_col_offset, p->arena);
}

/* Construct a KeywordOrStarred */
KeywordOrStarred *
_PyPegen_keyword_or_starred(Parser *p, ശൂന്യം *element, പൂർണ്ണസംഖ്യ is_keyword)
{
    KeywordOrStarred *a = _PyArena_Malloc(p->arena, വലുപ്പം(KeywordOrStarred));
    എങ്കിൽ (!a) {
        തിരികെ ശൂന്യ;
    }
    a->element = element;
    a->is_keyword = is_keyword;
    തിരികെ a;
}

/* Get the number of starred expressions in an asdl_seq* of KeywordOrStarred*s */
സ്ഥിതം പൂർണ്ണസംഖ്യ
seq_സംഖ്യ_of_starred_exprs(asdl_seq *seq)
{
    പൂർണ്ണസംഖ്യ n = 0;
    വേണ്ടി (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(seq, i);
        എങ്കിൽ (!k->is_keyword) {
            n++;
        }
    }
    തിരികെ n;
}

/* Extract the starred expressions of an asdl_seq* of KeywordOrStarred*s */
asdl_വ്യഞ്ജകം_seq *
_PyPegen_seq_extract_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    പൂർണ്ണസംഖ്യ new_len = seq_സംഖ്യ_of_starred_exprs(kwargs);
    എങ്കിൽ (new_len == 0) {
        തിരികെ ശൂന്യ;
    }
    asdl_വ്യഞ്ജകം_seq *new_seq = Py_asdl_വ്യഞ്ജകം_seq_new(new_len, p->arena);
    എങ്കിൽ (!new_seq) {
        തിരികെ ശൂന്യ;
    }

    പൂർണ്ണസംഖ്യ idx = 0;
    വേണ്ടി (Py_ssize_t i = 0, len = asdl_seq_LEN(kwargs); i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        എങ്കിൽ (!k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    തിരികെ new_seq;
}

/* Return a new asdl_seq* with only the keywords in kwargs */
asdl_keyword_seq*
_PyPegen_seq_delete_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    Py_ssize_t len = asdl_seq_LEN(kwargs);
    Py_ssize_t new_len = len - seq_സംഖ്യ_of_starred_exprs(kwargs);
    എങ്കിൽ (new_len == 0) {
        തിരികെ ശൂന്യ;
    }
    asdl_keyword_seq *new_seq = _Py_asdl_keyword_seq_new(new_len, p->arena);
    എങ്കിൽ (!new_seq) {
        തിരികെ ശൂന്യ;
    }

    പൂർണ്ണസംഖ്യ idx = 0;
    വേണ്ടി (Py_ssize_t i = 0; i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        എങ്കിൽ (k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    തിരികെ new_seq;
}

വ്യഞ്ജകം_ty
_PyPegen_ensure_imaginary(Parser *p, വ്യഞ്ജകം_ty exp)
{
    എങ്കിൽ (exp->kind != Constant_kind || !PyComplex_CheckExact(exp->v.Constant.മൂല്യം)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "imaginary number required in complex literal");
        തിരികെ ശൂന്യ;
    }
    തിരികെ exp;
}

വ്യഞ്ജകം_ty
_PyPegen_ensure_real(Parser *p, വ്യഞ്ജകം_ty exp)
{
    എങ്കിൽ (exp->kind != Constant_kind || PyComplex_CheckExact(exp->v.Constant.മൂല്യം)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "real number required in complex literal");
        തിരികെ ശൂന്യ;
    }
    തിരികെ exp;
}

mod_ty
_PyPegen_make_module(Parser *p, asdl_stmt_seq *a) {
    asdl_type_ignore_seq *type_ignores = ശൂന്യ;
    Py_ssize_t അക്കം = p->type_ignore_comments.അക്കം_ഇനങ്ങൾ;
    എങ്കിൽ (അക്കം > 0) {
        // Turn the raw (comment, lineno) pairs into TypeIgnore objects in the arena
        type_ignores = _Py_asdl_type_ignore_seq_new(അക്കം, p->arena);
        എങ്കിൽ (type_ignores == ശൂന്യ) {
            തിരികെ ശൂന്യ;
        }
        വേണ്ടി (Py_ssize_t i = 0; i < അക്കം; i++) {
            PyObject *tag = _PyPegen_new_type_comment(p, p->type_ignore_comments.ഇനങ്ങൾ[i].comment);
            എങ്കിൽ (tag == ശൂന്യ) {
                തിരികെ ശൂന്യ;
            }
            type_ignore_ty ti = _PyAST_TypeIgnore(p->type_ignore_comments.ഇനങ്ങൾ[i].lineno,
                                                  tag, p->arena);
            എങ്കിൽ (ti == ശൂന്യ) {
                തിരികെ ശൂന്യ;
            }
            asdl_seq_SET(type_ignores, i, ti);
        }
    }
    തിരികെ _PyAST_Module(a, type_ignores, p->arena);
}

PyObject *
_PyPegen_new_type_comment(Parser *p, സ്ഥിരം അക്ഷരം *s)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, strlen(s), ശൂന്യ);
    എങ്കിൽ (res == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (_PyArena_AddPyObject(p->arena, res) < 0) {
        Py_DECREF(res);
        തിരികെ ശൂന്യ;
    }
    തിരികെ res;
}

arg_ty
_PyPegen_add_type_comment_to_arg(Parser *p, arg_ty a, Token *tc)
{
    എങ്കിൽ (tc == ശൂന്യ) {
        തിരികെ a;
    }
    സ്ഥിരം അക്ഷരം *bytes = PyBytes_AsString(tc->bytes);
    എങ്കിൽ (bytes == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    എങ്കിൽ (tco == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }
    തിരികെ _PyAST_arg(a->arg, a->annotation, tco,
                      a->lineno, a->col_offset, a->end_lineno, a->end_col_offset,
                      p->arena);
}

/* Checks if the NOTEQUAL token is valid given the current parser flags
0 indicates success and nonzero indicates failure (an exception may be set) */
പൂർണ്ണസംഖ്യ
_PyPegen_check_barry_as_flufl(Parser *p, Token* t) {
    പ്രതിജ്ഞാ(t->bytes != ശൂന്യ);
    പ്രതിജ്ഞാ(t->type == NOTEQUAL);

    സ്ഥിരം അക്ഷരം* tok_str = PyBytes_AS_STRING(t->bytes);
    എങ്കിൽ (p->flags & PyPARSE_BARRY_AS_BDFL && strcmp(tok_str, "<>") != 0) {
        RAISE_SYNTAX_ERROR("with Barry as BDFL, use '<>' instead of '!='");
        തിരികെ -1;
    }
    എങ്കിൽ (!(p->flags & PyPARSE_BARRY_AS_BDFL)) {
        തിരികെ strcmp(tok_str, "!=");
    }
    തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
_PyPegen_check_legacy_stmt(Parser *p, വ്യഞ്ജകം_ty name) {
    എങ്കിൽ (name->kind != Name_kind) {
        തിരികെ 0;
    }
    സ്ഥിരം അക്ഷരം* candidates[2] = {"print", "exec"};
    വേണ്ടി (പൂർണ്ണസംഖ്യ i=0; i<2; i++) {
        എങ്കിൽ (PyUnicode_CompareWithASCIIString(name->v.Name.id, candidates[i]) == 0) {
            തിരികെ 1;
        }
    }
    തിരികെ 0;
}

ശൂന്യം *
_PyPegen_raise_error_for_missing_comma(Parser *p, വ്യഞ്ജകം_ty a, വ്യഞ്ജകം_ty b)
{
    // Don't raise for legacy statements like "print x" or "exec x"
    എങ്കിൽ (_PyPegen_check_legacy_stmt(p, a)) {
        തിരികെ ശൂന്യ;
    }
    // Only raise inside parentheses/brackets (level > 0)
    എങ്കിൽ (p->tokens[p->mark - 1]->level == 0) {
        തിരികെ ശൂന്യ;
    }
    // For multi-line expressions (like string concatenations), point to the
    // last line instead of the first for a more helpful error message.
    // Use a->col_offset as the starting column since all strings in the
    // concatenation typically share the same indentation.
    എങ്കിൽ (a->end_lineno > a->lineno) {
        തിരികെ RAISE_ERROR_KNOWN_LOCATION(
            p, PyExc_SyntaxError, a->end_lineno, a->col_offset,
            a->end_lineno, a->end_col_offset,
            "invalid syntax. Perhaps you forgot a comma?"
        );
    }
    തിരികെ RAISE_ERROR_KNOWN_LOCATION(
        p, PyExc_SyntaxError, a->lineno, a->col_offset,
        b->end_lineno, b->end_col_offset,
        "invalid syntax. Perhaps you forgot a comma?"
    );
}

സ്ഥിതം ResultTokenWithMetadata *
result_token_with_metadata(Parser *p, ശൂന്യം *result, PyObject *metadata)
{
    ResultTokenWithMetadata *res = _PyArena_Malloc(p->arena, വലുപ്പം(ResultTokenWithMetadata));
    എങ്കിൽ (res == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }
    res->metadata = metadata;
    res->result = result;
    തിരികെ res;
}

സ്ഥിതം അക്ഷരം
formatted_string_prefix(സ്ഥിരം Parser *p)
{
    പൂർണ്ണസംഖ്യ nested = 0;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = p->mark - 1; i >= 0; i--) {
        പൂർണ്ണസംഖ്യ type = p->tokens[i]->type;
        എങ്കിൽ (type == FSTRING_END || type == TSTRING_END) {
            nested++;
        }
        അല്ലെങ്കിൽ എങ്കിൽ (type == FSTRING_START || type == TSTRING_START) {
            എങ്കിൽ (nested == 0) {
                തിരികെ type == TSTRING_START ? 't' : 'f';
            }
            nested--;
        }
    }
    Py_UNREACHABLE();
}

ResultTokenWithMetadata *
_PyPegen_check_fstring_conversion(Parser *p, Token* conv_token, വ്യഞ്ജകം_ty conv)
{
    എങ്കിൽ (conv_token->lineno != conv->lineno || conv_token->end_col_offset != conv->col_offset) {
        തിരികെ RAISE_SYNTAX_ERROR_KNOWN_RANGE(
            conv_token, conv,
            "%c-string: conversion type must come right after the exclamation mark",
            formatted_string_prefix(p)
        );
    }

    Py_UCS4 first = PyUnicode_READ_CHAR(conv->v.Name.id, 0);
    എങ്കിൽ (PyUnicode_GET_LENGTH(conv->v.Name.id) > 1 ||
            !(first == 's' || first == 'r' || first == 'a')) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(conv,
                                            "%c-string: invalid conversion character %R: expected 's', 'r', or 'a'",
                                            formatted_string_prefix(p),
                                            conv->v.Name.id);
        തിരികെ ശൂന്യ;
    }

    തിരികെ result_token_with_metadata(p, conv, conv_token->metadata);
}

ResultTokenWithMetadata *
_PyPegen_setup_full_format_spec(Parser *p, Token *colon, asdl_വ്യഞ്ജകം_seq *spec, പൂർണ്ണസംഖ്യ lineno, പൂർണ്ണസംഖ്യ col_offset,
                                പൂർണ്ണസംഖ്യ end_lineno, പൂർണ്ണസംഖ്യ end_col_offset, PyArena *arena)
{
    എങ്കിൽ (!spec) {
        തിരികെ ശൂന്യ;
    }

    // This is needed to keep compatibility with 3.11, where an empty format
    // spec is parsed as an *empty* JoinedStr node, instead of having an empty
    // constant in it.
    Py_ssize_t n_ഇനങ്ങൾ = asdl_seq_LEN(spec);
    Py_ssize_t non_empty_എണ്ണം = 0;
    വേണ്ടി (Py_ssize_t i = 0; i < n_ഇനങ്ങൾ; i++) {
        വ്യഞ്ജകം_ty item = asdl_seq_GET(spec, i);
        non_empty_എണ്ണം += !(item->kind == Constant_kind &&
                             PyUnicode_CheckExact(item->v.Constant.മൂല്യം) &&
                             PyUnicode_GET_LENGTH(item->v.Constant.മൂല്യം) == 0);
    }
    എങ്കിൽ (non_empty_എണ്ണം != n_ഇനങ്ങൾ) {
        asdl_വ്യഞ്ജകം_seq *resized_spec =
            Py_asdl_വ്യഞ്ജകം_seq_new(non_empty_എണ്ണം, p->arena);
        എങ്കിൽ (resized_spec == ശൂന്യ) {
            തിരികെ ശൂന്യ;
        }
        Py_ssize_t j = 0;
        വേണ്ടി (Py_ssize_t i = 0; i < n_ഇനങ്ങൾ; i++) {
            വ്യഞ്ജകം_ty item = asdl_seq_GET(spec, i);
            എങ്കിൽ (item->kind == Constant_kind &&
                PyUnicode_CheckExact(item->v.Constant.മൂല്യം) &&
                PyUnicode_GET_LENGTH(item->v.Constant.മൂല്യം) == 0) {
                തുടരുക;
            }
            asdl_seq_SET(resized_spec, j++, item);
        }
        പ്രതിജ്ഞാ(j == non_empty_എണ്ണം);
        spec = resized_spec;
    }
    വ്യഞ്ജകം_ty res;
    Py_ssize_t n = asdl_seq_LEN(spec);
    എങ്കിൽ (n == 0 || (n == 1 && asdl_seq_GET(spec, 0)->kind == Constant_kind)) {
        res = _PyAST_JoinedStr(spec, lineno, col_offset, end_lineno,
                                    end_col_offset, p->arena);
    } അല്ലെങ്കിൽ {
        res = _PyPegen_concatenate_strings(p, spec,
                             lineno, col_offset, end_lineno,
                             end_col_offset, arena);
    }
    എങ്കിൽ (!res) {
        തിരികെ ശൂന്യ;
    }
    തിരികെ result_token_with_metadata(p, res, colon->metadata);
}

സ്ഥിരം അക്ഷരം *
Py_Pegen_get_വ്യഞ്ജകം_name(വ്യഞ്ജകം_ty e)
{
    പ്രതിജ്ഞാ(e != ശൂന്യ);
    തിരഞ്ഞെടുപ്പ് (e->kind) {
        അവസ്ഥ Attribute_kind:
            തിരികെ "attribute";
        അവസ്ഥ Subscript_kind:
            തിരികെ "subscript";
        അവസ്ഥ Starred_kind:
            തിരികെ "starred";
        അവസ്ഥ Name_kind:
            തിരികെ "name";
        അവസ്ഥ List_kind:
            തിരികെ "list";
        അവസ്ഥ Tuple_kind:
            തിരികെ "tuple";
        അവസ്ഥ Lambda_kind:
            തിരികെ "lambda";
        അവസ്ഥ Call_kind:
            തിരികെ "function call";
        അവസ്ഥ BoolOp_kind:
        അവസ്ഥ BinOp_kind:
        അവസ്ഥ UnaryOp_kind:
            തിരികെ "expression";
        അവസ്ഥ GeneratorExp_kind:
            തിരികെ "generator expression";
        അവസ്ഥ Yield_kind:
        അവസ്ഥ YieldFrom_kind:
            തിരികെ "yield expression";
        അവസ്ഥ Await_kind:
            തിരികെ "await expression";
        അവസ്ഥ ListComp_kind:
            തിരികെ "list comprehension";
        അവസ്ഥ SetComp_kind:
            തിരികെ "set comprehension";
        അവസ്ഥ DictComp_kind:
            തിരികെ "dict comprehension";
        അവസ്ഥ Dict_kind:
            തിരികെ "dict literal";
        അവസ്ഥ Set_kind:
            തിരികെ "set display";
        അവസ്ഥ JoinedStr_kind:
        അവസ്ഥ Formatted_മൂല്യം_kind:
            തിരികെ "f-string expression";
        അവസ്ഥ TemplateStr_kind:
        അവസ്ഥ Interpolation_kind:
            തിരികെ "t-string expression";
        അവസ്ഥ Constant_kind: {
            PyObject *മൂല്യം = e->v.Constant.മൂല്യം;
            എങ്കിൽ (മൂല്യം == Py_None) {
                തിരികെ "None";
            }
            എങ്കിൽ (മൂല്യം == Py_False) {
                തിരികെ "False";
            }
            എങ്കിൽ (മൂല്യം == Py_True) {
                തിരികെ "True";
            }
            എങ്കിൽ (മൂല്യം == Py_Ellipsis) {
                തിരികെ "ellipsis";
            }
            തിരികെ "literal";
        }
        അവസ്ഥ Compare_kind:
            തിരികെ "comparison";
        അവസ്ഥ IfExp_kind:
            തിരികെ "conditional expression";
        അവസ്ഥ Named_വ്യഞ്ജകം_kind:
            തിരികെ "named expression";
        സ്ഥിരസ്ഥിതി:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->kind, e->lineno);
            തിരികെ ശൂന്യ;
    }
}

വ്യഞ്ജകം_ty
_PyPegen_get_last_comprehension_item(comprehension_ty comprehension) {
    എങ്കിൽ (comprehension->ifs == ശൂന്യ || asdl_seq_LEN(comprehension->ifs) == 0) {
        തിരികെ comprehension->iter;
    }
    തിരികെ PyPegen_last_item(comprehension->ifs, വ്യഞ്ജകം_ty);
}

വ്യഞ്ജകം_ty _PyPegen_collect_call_seqs(Parser *p, asdl_വ്യഞ്ജകം_seq *a, asdl_seq *b,
                     പൂർണ്ണസംഖ്യ lineno, പൂർണ്ണസംഖ്യ col_offset, പൂർണ്ണസംഖ്യ end_lineno,
                     പൂർണ്ണസംഖ്യ end_col_offset, PyArena *arena) {
    Py_ssize_t args_len = asdl_seq_LEN(a);
    Py_ssize_t total_len = args_len;

    എങ്കിൽ (b == ശൂന്യ) {
        തിരികെ _PyAST_Call(_PyPegen_dummy_name(p), a, ശൂന്യ, lineno, col_offset,
                        end_lineno, end_col_offset, arena);

    }

    asdl_വ്യഞ്ജകം_seq *starreds = _PyPegen_seq_extract_starred_exprs(p, b);
    എങ്കിൽ (!starreds && PyErr_Occurred()) {
        തിരികെ ശൂന്യ;
    }

    asdl_keyword_seq *keywords = _PyPegen_seq_delete_starred_exprs(p, b);
    എങ്കിൽ (!keywords && PyErr_Occurred()) {
        തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (starreds) {
        total_len += asdl_seq_LEN(starreds);
    }

    asdl_വ്യഞ്ജകം_seq *args = Py_asdl_വ്യഞ്ജകം_seq_new(total_len, arena);
    എങ്കിൽ (args == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }

    Py_ssize_t i = 0;
    വേണ്ടി (i = 0; i < args_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(a, i));
    }
    വേണ്ടി (; i < total_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(starreds, i - args_len));
    }

    തിരികെ _PyAST_Call(_PyPegen_dummy_name(p), args, keywords, lineno,
                       col_offset, end_lineno, end_col_offset, arena);
}

// AST Error reporting helpers

വ്യഞ്ജകം_ty
_PyPegen_get_invalid_target(വ്യഞ്ജകം_ty e, TARGETS_TYPE targets_type)
{
    എങ്കിൽ (e == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (targets_type == SINGLE_TARGETS ||
        targets_type == ATTRIBUTE_OR_SUBSCRIPT_TARGETS) {
        എങ്കിൽ (targets_type == SINGLE_TARGETS && e->kind == Name_kind) {
            തിരികെ ശൂന്യ;
        }
        തിരികെ e->kind == Attribute_kind || e->kind == Subscript_kind ? ശൂന്യ : e;
    }

#define VISIT_CONTAINER(CONTAINER, TYPE) ചെയ്യുക { \
        Py_ssize_t len = asdl_seq_LEN((CONTAINER)->v.TYPE.elts);\
        വേണ്ടി (Py_ssize_t i = 0; i < len; i++) {\
            വ്യഞ്ജകം_ty other = asdl_seq_GET((CONTAINER)->v.TYPE.elts, i);\
            വ്യഞ്ജകം_ty child = _PyPegen_get_invalid_target(other, targets_type);\
            എങ്കിൽ (child != ശൂന്യ) {\
                തിരികെ child;\
            }\
        }\
    } ആകുമ്പോൾ (0)

    // We only need to visit List and Tuple nodes recursively as those
    // are the only ones that can contain valid names in targets when
    // they are parsed as expressions. Any other kind of expression
    // that is a container (like Sets or Dicts) is directly invalid and
    // we don't need to visit it recursively.

    തിരഞ്ഞെടുപ്പ് (e->kind) {
        അവസ്ഥ List_kind:
            VISIT_CONTAINER(e, List);
            തിരികെ ശൂന്യ;
        അവസ്ഥ Tuple_kind:
            VISIT_CONTAINER(e, Tuple);
            തിരികെ ശൂന്യ;
        അവസ്ഥ Starred_kind:
            എങ്കിൽ (targets_type == DEL_TARGETS) {
                തിരികെ e;
            }
            തിരികെ _PyPegen_get_invalid_target(e->v.Starred.മൂല്യം, targets_type);
        അവസ്ഥ Compare_kind:
            // This is needed, because the `a in b` in `for a in b` gets parsed
            // as a comparison, and so we need to search the left side of the comparison
            // for invalid targets.
            എങ്കിൽ (targets_type == FOR_TARGETS) {
                cmpop_ty cmpop = (cmpop_ty) asdl_seq_GET(e->v.Compare.ops, 0);
                എങ്കിൽ (cmpop == In) {
                    തിരികെ _PyPegen_get_invalid_target(e->v.Compare.ഇടത്, targets_type);
                }
                തിരികെ ശൂന്യ;
            }
            തിരികെ e;
        അവസ്ഥ Name_kind:
        അവസ്ഥ Subscript_kind:
        അവസ്ഥ Attribute_kind:
            തിരികെ ശൂന്യ;
        സ്ഥിരസ്ഥിതി:
            തിരികെ e;
    }
}

ശൂന്യം *_PyPegen_arguments_parsing_error(Parser *p, വ്യഞ്ജകം_ty e) {
    പൂർണ്ണസംഖ്യ kwarg_unpacking = 0;
    വേണ്ടി (Py_ssize_t i = 0, l = asdl_seq_LEN(e->v.Call.keywords); i < l; i++) {
        keyword_ty keyword = asdl_seq_GET(e->v.Call.keywords, i);
        എങ്കിൽ (!keyword->arg) {
            kwarg_unpacking = 1;
        }
    }

    സ്ഥിരം അക്ഷരം *സന്ദേശം = ശൂന്യ;
    എങ്കിൽ (kwarg_unpacking) {
        സന്ദേശം = "positional argument follows keyword argument unpacking";
    } അല്ലെങ്കിൽ {
        സന്ദേശം = "positional argument follows keyword argument";
    }

    തിരികെ RAISE_SYNTAX_ERROR(സന്ദേശം);
}

ശൂന്യം *
_PyPegen_nonparen_genexp_in_call(Parser *p, വ്യഞ്ജകം_ty args, asdl_comprehension_seq *comprehensions)
{
    /* The rule that calls this function is 'args for_if_clauses'.
       For the input f(L, x for x in y), L and x are in args and
       the for is parsed as a for_if_clause. We have to check if
       len <= 1, so that input like dict((a, b) for a, b in x)
       gets successfully parsed and then we pass the last
       argument (x in the above example) as the location of the
       error */
    Py_ssize_t len = asdl_seq_LEN(args->v.Call.args);
    എങ്കിൽ (len <= 1) {
        തിരികെ ശൂന്യ;
    }

    comprehension_ty last_comprehension = PyPegen_last_item(comprehensions, comprehension_ty);

    തിരികെ RAISE_SYNTAX_ERROR_KNOWN_RANGE(
        (വ്യഞ്ജകം_ty) asdl_seq_GET(args->v.Call.args, len - 1),
        _PyPegen_get_last_comprehension_item(last_comprehension),
        "Generator expression must be parenthesized"
    );
}

// Fstring stuff

സ്ഥിതം വ്യഞ്ജകം_ty
_PyPegen_decode_fstring_part(Parser* p, പൂർണ്ണസംഖ്യ is_raw, വ്യഞ്ജകം_ty constant, Token* token) {
    പ്രതിജ്ഞാ(PyUnicode_CheckExact(constant->v.Constant.മൂല്യം));

    സ്ഥിരം അക്ഷരം* bstr = PyUnicode_AsUTF8(constant->v.Constant.മൂല്യം);
    എങ്കിൽ (bstr == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }

    size_t len;
    എങ്കിൽ (strcmp(bstr, "{{") == 0 || strcmp(bstr, "}}") == 0) {
        len = 1;
    } അല്ലെങ്കിൽ {
        len = strlen(bstr);
    }

    is_raw = is_raw || strchr(bstr, '\\') == ശൂന്യ;
    PyObject *str = _PyPegen_decode_string(p, is_raw, bstr, len, token);
    എങ്കിൽ (str == ശൂന്യ) {
        _Pypegen_raise_decode_error(p);
        തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        തിരികെ ശൂന്യ;
    }
    തിരികെ _PyAST_Constant(str, ശൂന്യ, constant->lineno, constant->col_offset,
                           constant->end_lineno, constant->end_col_offset,
                           p->arena);
}

സ്ഥിതം asdl_വ്യഞ്ജകം_seq *
_get_resized_exprs(Parser *p, Token *a, asdl_വ്യഞ്ജകം_seq *raw_expressions,
                   Token *b, പൂർണ്ണസംഖ്യ is_tstring)
{
    Py_ssize_t n_ഇനങ്ങൾ = asdl_seq_LEN(raw_expressions);
    Py_ssize_t total_ഇനങ്ങൾ = n_ഇനങ്ങൾ;
    വേണ്ടി (Py_ssize_t i = 0; i < n_ഇനങ്ങൾ; i++) {
        വ്യഞ്ജകം_ty item = asdl_seq_GET(raw_expressions, i);
        എങ്കിൽ (item->kind == JoinedStr_kind) {
            total_ഇനങ്ങൾ += asdl_seq_LEN(item->v.JoinedStr.മൂല്യങ്ങൾ) - 1;
        }
    }

    സ്ഥിരം അക്ഷരം* quote_str = PyBytes_AsString(a->bytes);
    എങ്കിൽ (quote_str == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }
    പൂർണ്ണസംഖ്യ is_raw = strpbrk(quote_str, "rR") != ശൂന്യ;

    asdl_വ്യഞ്ജകം_seq *seq = Py_asdl_വ്യഞ്ജകം_seq_new(total_ഇനങ്ങൾ, p->arena);
    എങ്കിൽ (seq == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }

    Py_ssize_t index = 0;
    വേണ്ടി (Py_ssize_t i = 0; i < n_ഇനങ്ങൾ; i++) {
        വ്യഞ്ജകം_ty item = asdl_seq_GET(raw_expressions, i);

        /* Debug expressions arrive as JoinedStr(text, value); flatten them
           into the surrounding string. */
        എങ്കിൽ (item->kind == JoinedStr_kind) {
            asdl_വ്യഞ്ജകം_seq *മൂല്യങ്ങൾ = item->v.JoinedStr.മൂല്യങ്ങൾ;
            എങ്കിൽ (asdl_seq_LEN(മൂല്യങ്ങൾ) != 2) {
                PyErr_Format(PyExc_SystemError,
                             is_tstring
                             ? "unexpected TemplateStr node without debug data in t-string at line %d"
                             : "unexpected JoinedStr node without debug data in f-string at line %d",
                             item->lineno);
                തിരികെ ശൂന്യ;
            }

            വ്യഞ്ജകം_ty first = asdl_seq_GET(മൂല്യങ്ങൾ, 0);
            പ്രതിജ്ഞാ(first->kind == Constant_kind);
            asdl_seq_SET(seq, index++, first);

            വ്യഞ്ജകം_ty second = asdl_seq_GET(മൂല്യങ്ങൾ, 1);
            പ്രതിജ്ഞാ((is_tstring &&
                    second->kind == Interpolation_kind) ||
                   second->kind == Formatted_മൂല്യം_kind);
            asdl_seq_SET(seq, index++, second);

            തുടരുക;
        }

        എങ്കിൽ (item->kind == Constant_kind) {
            item = _PyPegen_decode_fstring_part(p, is_raw, item, b);
            എങ്കിൽ (item == ശൂന്യ) {
                തിരികെ ശൂന്യ;
            }

            /* Tokenizer emits string parts even when the underlying string
            might become an empty value (e.g. FSTRING_MIDDLE with the value \\n)
            so we need to check for them and simplify it here. */
            എങ്കിൽ (PyUnicode_CheckExact(item->v.Constant.മൂല്യം)
                && PyUnicode_GET_LENGTH(item->v.Constant.മൂല്യം) == 0) {
                തുടരുക;
            }
        }
        asdl_seq_SET(seq, index++, item);
    }

    asdl_വ്യഞ്ജകം_seq *resized_exprs;
    എങ്കിൽ (index != total_ഇനങ്ങൾ) {
        resized_exprs = Py_asdl_വ്യഞ്ജകം_seq_new(index, p->arena);
        എങ്കിൽ (resized_exprs == ശൂന്യ) {
            തിരികെ ശൂന്യ;
        }
        വേണ്ടി (Py_ssize_t i = 0; i < index; i++) {
            asdl_seq_SET(resized_exprs, i, asdl_seq_GET(seq, i));
        }
    }
    അല്ലെങ്കിൽ {
        resized_exprs = seq;
    }
    തിരികെ resized_exprs;
}

വ്യഞ്ജകം_ty
_PyPegen_template_str(Parser *p, Token *a, asdl_വ്യഞ്ജകം_seq *raw_expressions, Token *b) {

    asdl_വ്യഞ്ജകം_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 1);
    എങ്കിൽ (resized_exprs == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }
    തിരികെ _PyAST_TemplateStr(resized_exprs, a->lineno, a->col_offset,
                              b->end_lineno, b->end_col_offset,
                              p->arena);
}

വ്യഞ്ജകം_ty
_PyPegen_joined_str(Parser *p, Token* a, asdl_വ്യഞ്ജകം_seq* raw_expressions, Token*b) {

    asdl_വ്യഞ്ജകം_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 0);
    എങ്കിൽ (resized_exprs == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }
    തിരികെ _PyAST_JoinedStr(resized_exprs, a->lineno, a->col_offset,
                            b->end_lineno, b->end_col_offset,
                            p->arena);
}

വ്യഞ്ജകം_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok) {
    എങ്കിൽ (tok->bytes == ശൂന്യ) {
        തിരികെ Py_Pegen_raise_incomplete_നിവേശം(p);
    }
    Py_ssize_t bsize = PyBytes_GET_SIZE(tok->bytes);
    സ്ഥിരം അക്ഷരം *bstr = PyBytes_AS_STRING(tok->bytes);

    പൂർണ്ണസംഖ്യ is_raw = tok->is_raw;

    PyObject* str = _PyPegen_decode_string(p, is_raw, bstr, bsize, tok);
    എങ്കിൽ (str == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        തിരികെ ശൂന്യ;
    }
    തിരികെ _PyAST_Constant(str, ശൂന്യ, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

വ്യഞ്ജകം_ty _PyPegen_constant_from_token(Parser* p, Token* tok) {
    എങ്കിൽ (tok->bytes == ശൂന്യ) {
        തിരികെ Py_Pegen_raise_incomplete_നിവേശം(p);
    }
    സ്ഥിരം അക്ഷരം *bstr = PyBytes_AS_STRING(tok->bytes);
    PyObject* str = PyUnicode_FromString(bstr);
    എങ്കിൽ (str == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        തിരികെ ശൂന്യ;
    }
    തിരികെ _PyAST_Constant(str, ശൂന്യ, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

വ്യഞ്ജകം_ty _PyPegen_constant_from_string(Parser* p, Token* tok) {
    എങ്കിൽ (tok->bytes == ശൂന്യ) {
        തിരികെ Py_Pegen_raise_incomplete_നിവേശം(p);
    }
    സ്ഥിരം അക്ഷരം *the_str = PyBytes_AS_STRING(tok->bytes);
    PyObject *s = _PyPegen_parse_string(p, tok);
    എങ്കിൽ (s == ശൂന്യ) {
        _Pypegen_raise_decode_error(p);
        തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (_PyArena_AddPyObject(p->arena, s) < 0) {
        Py_DECREF(s);
        തിരികെ ശൂന്യ;
    }
    PyObject *kind = ശൂന്യ;
    എങ്കിൽ (the_str && the_str[0] == 'u') {
        kind = _PyPegen_new_identifier(p, "u");
        എങ്കിൽ (kind == ശൂന്യ) {
            തിരികെ ശൂന്യ;
        }
    }
    തിരികെ _PyAST_Constant(s, kind, tok->lineno, tok->col_offset, tok->end_lineno, tok->end_col_offset, p->arena);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
_get_interpolation_conversion(Parser *p, Token *debug, ResultTokenWithMetadata *conversion,
                              ResultTokenWithMetadata *format)
{
    എങ്കിൽ (conversion != ശൂന്യ) {
        വ്യഞ്ജകം_ty conversion_വ്യഞ്ജകം = (വ്യഞ്ജകം_ty) conversion->result;
        പ്രതിജ്ഞാ(conversion_വ്യഞ്ജകം->kind == Name_kind);
        Py_UCS4 first = PyUnicode_READ_CHAR(conversion_വ്യഞ്ജകം->v.Name.id, 0);
        തിരികെ Py_SAFE_DOWNCAST(first, Py_UCS4, പൂർണ്ണസംഖ്യ);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (debug && !format) {
        /* If no conversion is specified, use !r for debug expressions */
        തിരികെ (പൂർണ്ണസംഖ്യ)'r';
    }
    തിരികെ -1;
}

സ്ഥിതം PyObject *
strip_interpolation_debug_വ്യഞ്ജകം(PyObject *exprstr)
{
    Py_ssize_t len = PyUnicode_GET_LENGTH(exprstr);

    /* Discard whitespace and explicit line continuations after the debug "="
       but preserve whitespace before it. */
    ആകുമ്പോൾ (len > 0) {
        പൂർണ്ണസംഖ്യ has_newline = 0;
        ആകുമ്പോൾ (len > 0) {
            Py_UCS4 c = PyUnicode_READ_CHAR(exprstr, len - 1);
            എങ്കിൽ (!_PyUnicode_IsWhitespace(c)) {
                നിർത്തുക;
            }
            എങ്കിൽ (c == '\r' || c == '\n') {
                has_newline = 1;
            }
            len--;
        }
        എങ്കിൽ (!has_newline || len == 0 ||
            PyUnicode_READ_CHAR(exprstr, len - 1) != '\\')
        {
            നിർത്തുക;
        }
        len--;
    }

    /* Preserve unexpected metadata instead of dropping source text. */
    എങ്കിൽ (len == 0 || PyUnicode_READ_CHAR(exprstr, len - 1) != '=') {
        തിരികെ Py_NewRef(exprstr);
    }

    തിരികെ PyUnicode_Substring(exprstr, 0, len - 1);
}

വ്യഞ്ജകം_ty _PyPegen_interpolation(Parser *p, വ്യഞ്ജകം_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, പൂർണ്ണസംഖ്യ lineno, പൂർണ്ണസംഖ്യ col_offset,
                                 പൂർണ്ണസംഖ്യ end_lineno, പൂർണ്ണസംഖ്യ end_col_offset, PyArena *arena) {

    പൂർണ്ണസംഖ്യ conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    /* Find the non whitespace token after the "=" */
    പൂർണ്ണസംഖ്യ debug_end_വരി, debug_end_offset;
    PyObject *debug_metadata;
    constant exprstr;

    എങ്കിൽ (conversion) {
        debug_end_വരി = ((വ്യഞ്ജകം_ty) conversion->result)->lineno;
        debug_end_offset = ((വ്യഞ്ജകം_ty) conversion->result)->col_offset;
        debug_metadata = exprstr = conversion->metadata;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (format) {
        debug_end_വരി = ((വ്യഞ്ജകം_ty) format->result)->lineno;
        debug_end_offset = ((വ്യഞ്ജകം_ty) format->result)->col_offset + 1;
        debug_metadata = exprstr = format->metadata;
    }
    അല്ലെങ്കിൽ {
        debug_end_വരി = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = exprstr = closing_brace->metadata;
    }

    പ്രതിജ്ഞാ(exprstr != ശൂന്യ);
    PyObject *final_exprstr = debug
        ? strip_interpolation_debug_വ്യഞ്ജകം(exprstr)
        : Py_NewRef(exprstr);
    എങ്കിൽ (!final_exprstr || _PyArena_AddPyObject(arena, final_exprstr) < 0) {
        Py_XDECREF(final_exprstr);
        തിരികെ ശൂന്യ;
    }

    വ്യഞ്ജകം_ty interpolation = _PyAST_Interpolation(
        expression, final_exprstr, conversion_val, format ? (വ്യഞ്ജകം_ty) format->result : ശൂന്യ,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    എങ്കിൽ (!interpolation || !debug) {
        തിരികെ interpolation;
    }

    വ്യഞ്ജകം_ty debug_text = _PyAST_Constant(debug_metadata, ശൂന്യ, lineno, col_offset + 1, debug_end_വരി,
                                            debug_end_offset - 1, p->arena);
    എങ്കിൽ (!debug_text) {
        തിരികെ ശൂന്യ;
    }

    asdl_വ്യഞ്ജകം_seq *മൂല്യങ്ങൾ = Py_asdl_വ്യഞ്ജകം_seq_new(2, arena);
    എങ്കിൽ (!മൂല്യങ്ങൾ) {
        തിരികെ ശൂന്യ;
    }
    asdl_seq_SET(മൂല്യങ്ങൾ, 0, debug_text);
    asdl_seq_SET(മൂല്യങ്ങൾ, 1, interpolation);
    തിരികെ _PyAST_JoinedStr(മൂല്യങ്ങൾ, lineno, col_offset, debug_end_വരി, debug_end_offset, p->arena);
}

വ്യഞ്ജകം_ty Py_Pegen_formatted_മൂല്യം(Parser *p, വ്യഞ്ജകം_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, പൂർണ്ണസംഖ്യ lineno, പൂർണ്ണസംഖ്യ col_offset,
                                 പൂർണ്ണസംഖ്യ end_lineno, പൂർണ്ണസംഖ്യ end_col_offset, PyArena *arena) {
    പൂർണ്ണസംഖ്യ conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    വ്യഞ്ജകം_ty formatted_മൂല്യം = Py_AST_Formatted_മൂല്യം(
        expression, conversion_val, format ? (വ്യഞ്ജകം_ty) format->result : ശൂന്യ,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    എങ്കിൽ (!formatted_മൂല്യം || !debug) {
        തിരികെ formatted_മൂല്യം;
    }

    /* Find the non whitespace token after the "=" */
    പൂർണ്ണസംഖ്യ debug_end_വരി, debug_end_offset;
    PyObject *debug_metadata;

    എങ്കിൽ (conversion) {
        debug_end_വരി = ((വ്യഞ്ജകം_ty) conversion->result)->lineno;
        debug_end_offset = ((വ്യഞ്ജകം_ty) conversion->result)->col_offset;
        debug_metadata = conversion->metadata;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (format) {
        debug_end_വരി = ((വ്യഞ്ജകം_ty) format->result)->lineno;
        debug_end_offset = ((വ്യഞ്ജകം_ty) format->result)->col_offset + 1;
        debug_metadata = format->metadata;
    }
    അല്ലെങ്കിൽ {
        debug_end_വരി = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = closing_brace->metadata;
    }
    വ്യഞ്ജകം_ty debug_text = _PyAST_Constant(debug_metadata, ശൂന്യ, lineno, col_offset + 1, debug_end_വരി,
                                            debug_end_offset - 1, p->arena);
    എങ്കിൽ (!debug_text) {
        തിരികെ ശൂന്യ;
    }

    asdl_വ്യഞ്ജകം_seq *മൂല്യങ്ങൾ = Py_asdl_വ്യഞ്ജകം_seq_new(2, arena);
    എങ്കിൽ (!മൂല്യങ്ങൾ) {
        തിരികെ ശൂന്യ;
    }
    asdl_seq_SET(മൂല്യങ്ങൾ, 0, debug_text);
    asdl_seq_SET(മൂല്യങ്ങൾ, 1, formatted_മൂല്യം);
    തിരികെ _PyAST_JoinedStr(മൂല്യങ്ങൾ, lineno, col_offset, debug_end_വരി, debug_end_offset, p->arena);
}

സ്ഥിതം വ്യഞ്ജകം_ty
_build_concatenated_bytes(Parser *p, asdl_വ്യഞ്ജകം_seq *strings, പൂർണ്ണസംഖ്യ lineno,
                        പൂർണ്ണസംഖ്യ col_offset, പൂർണ്ണസംഖ്യ end_lineno, പൂർണ്ണസംഖ്യ end_col_offset,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    പ്രതിജ്ഞാ(len > 0);

    /* Bytes literals never get a kind, but just for consistency
        since they are represented as Constant nodes, we'll mirror
        the same behavior as unicode strings for determining the
        kind. */
    PyObject *kind = asdl_seq_GET(strings, 0)->v.Constant.kind;

    Py_ssize_t total = 0;
    വേണ്ടി (Py_ssize_t i = 0; i < len; i++) {
        വ്യഞ്ജകം_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.മൂല്യം;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        എങ്കിൽ (part > PY_SSIZE_T_MAX - total) {
            PyErr_NoMemory();
            തിരികെ ശൂന്യ;
        }
        total += part;
    }

    PyBytesWriter *writer = PyBytesWriter_Create(total);
    എങ്കിൽ (writer == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }
    അക്ഷരം *out = PyBytesWriter_GetData(writer);

    വേണ്ടി (Py_ssize_t i = 0; i < len; i++) {
        വ്യഞ്ജകം_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.മൂല്യം;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        എങ്കിൽ (part > 0) {
            memcpy(out, PyBytes_AS_STRING(bytes), part);
            out += part;
        }
    }

    PyObject *res = PyBytesWriter_Finish(writer);
    എങ്കിൽ (res == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (_PyArena_AddPyObject(arena, res) < 0) {
        Py_DECREF(res);
        തിരികെ ശൂന്യ;
    }
    തിരികെ _PyAST_Constant(res, kind, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

സ്ഥിതം വ്യഞ്ജകം_ty
_build_concatenated_unicode(Parser *p, asdl_വ്യഞ്ജകം_seq *strings, പൂർണ്ണസംഖ്യ lineno,
                        പൂർണ്ണസംഖ്യ col_offset, പൂർണ്ണസംഖ്യ end_lineno, പൂർണ്ണസംഖ്യ end_col_offset,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    പ്രതിജ്ഞാ(len > 1);

    വ്യഞ്ജകം_ty first = asdl_seq_GET(strings, 0);

    /* When a string is getting concatenated, the kind of the string
        is determined by the first string in the concatenation
        sequence.

        u"abc" "def" -> u"abcdef"
        "abc" u"abc" ->  "abcabc" */
    PyObject *kind = first->v.Constant.kind;

    PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
    എങ്കിൽ (writer == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }

    വേണ്ടി (Py_ssize_t i = 0; i < len; i++) {
        വ്യഞ്ജകം_ty current_elem = asdl_seq_GET(strings, i);
        പ്രതിജ്ഞാ(current_elem->kind == Constant_kind);

        എങ്കിൽ (PyUnicodeWriter_WriteStr(writer,
                                     current_elem->v.Constant.മൂല്യം)) {
            PyUnicodeWriter_Discard(writer);
            തിരികെ ശൂന്യ;
        }
    }

    PyObject *final = PyUnicodeWriter_Finish(writer);
    എങ്കിൽ (final == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (_PyArena_AddPyObject(p->arena, final) < 0) {
        Py_DECREF(final);
        തിരികെ ശൂന്യ;
    }
    തിരികെ _PyAST_Constant(final, kind, lineno, col_offset,
                           end_lineno, end_col_offset, arena);
}

സ്ഥിതം asdl_വ്യഞ്ജകം_seq *
_build_concatenated_str(Parser *p, asdl_വ്യഞ്ജകം_seq *strings,
                               പൂർണ്ണസംഖ്യ lineno, പൂർണ്ണസംഖ്യ col_offset, പൂർണ്ണസംഖ്യ end_lineno,
                               പൂർണ്ണസംഖ്യ end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    പ്രതിജ്ഞാ(len > 0);

    Py_ssize_t n_flattened_elements = 0;
    വേണ്ടി (Py_ssize_t i = 0; i < len; i++) {
        വ്യഞ്ജകം_ty elem = asdl_seq_GET(strings, i);
        തിരഞ്ഞെടുപ്പ്(elem->kind) {
            അവസ്ഥ JoinedStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.JoinedStr.മൂല്യങ്ങൾ);
                നിർത്തുക;
            അവസ്ഥ TemplateStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.TemplateStr.മൂല്യങ്ങൾ);
                നിർത്തുക;
            സ്ഥിരസ്ഥിതി:
                n_flattened_elements++;
                നിർത്തുക;
        }
    }


    asdl_വ്യഞ്ജകം_seq* flattened = Py_asdl_വ്യഞ്ജകം_seq_new(n_flattened_elements, p->arena);
    എങ്കിൽ (flattened == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }

    /* build flattened list */
    Py_ssize_t current_pos = 0;
    വേണ്ടി (Py_ssize_t i = 0; i < len; i++) {
        വ്യഞ്ജകം_ty elem = asdl_seq_GET(strings, i);
        തിരഞ്ഞെടുപ്പ്(elem->kind) {
            അവസ്ഥ JoinedStr_kind:
                വേണ്ടി (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.JoinedStr.മൂല്യങ്ങൾ); j++) {
                    വ്യഞ്ജകം_ty subvalue = asdl_seq_GET(elem->v.JoinedStr.മൂല്യങ്ങൾ, j);
                    എങ്കിൽ (subvalue == ശൂന്യ) {
                        തിരികെ ശൂന്യ;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                നിർത്തുക;
            അവസ്ഥ TemplateStr_kind:
                വേണ്ടി (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.TemplateStr.മൂല്യങ്ങൾ); j++) {
                    വ്യഞ്ജകം_ty subvalue = asdl_seq_GET(elem->v.TemplateStr.മൂല്യങ്ങൾ, j);
                    എങ്കിൽ (subvalue == ശൂന്യ) {
                        തിരികെ ശൂന്യ;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                നിർത്തുക;
            സ്ഥിരസ്ഥിതി:
                asdl_seq_SET(flattened, current_pos++, elem);
                നിർത്തുക;
        }
    }

    /* calculate folded element count */
    Py_ssize_t n_elements = 0;
    പൂർണ്ണസംഖ്യ prev_is_constant = 0;
    വേണ്ടി (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        വ്യഞ്ജകം_ty elem = asdl_seq_GET(flattened, i);

        /* The concatenation of a FormattedValue and an empty Constant should
           lead to the FormattedValue itself. Thus, we will not take any empty
           constants into account, just as in `_PyPegen_joined_str` */
        എങ്കിൽ (elem->kind == Constant_kind &&
            PyUnicode_CheckExact(elem->v.Constant.മൂല്യം) &&
            PyUnicode_GET_LENGTH(elem->v.Constant.മൂല്യം) == 0)
            തുടരുക;

        എങ്കിൽ (!prev_is_constant || elem->kind != Constant_kind) {
            n_elements++;
        }
        prev_is_constant = elem->kind == Constant_kind;
    }

    asdl_വ്യഞ്ജകം_seq* മൂല്യങ്ങൾ = Py_asdl_വ്യഞ്ജകം_seq_new(n_elements, p->arena);
    എങ്കിൽ (മൂല്യങ്ങൾ == ശൂന്യ) {
        തിരികെ ശൂന്യ;
    }

    /* build folded list */
    current_pos = 0;
    വേണ്ടി (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        വ്യഞ്ജകം_ty elem = asdl_seq_GET(flattened, i);

        /* if the current elem and the following are constants,
           fold them and all consequent constants */
        എങ്കിൽ (elem->kind == Constant_kind) {
            എങ്കിൽ (i + 1 < n_flattened_elements &&
                asdl_seq_GET(flattened, i + 1)->kind == Constant_kind) {
                വ്യഞ്ജകം_ty first_elem = elem;

                /* When a string is getting concatenated, the kind of the string
                   is determined by the first string in the concatenation
                   sequence.

                   u"abc" "def" -> u"abcdef"
                   "abc" u"abc" ->  "abcabc" */
                PyObject *kind = elem->v.Constant.kind;

                PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
                എങ്കിൽ (writer == ശൂന്യ) {
                    തിരികെ ശൂന്യ;
                }
                വ്യഞ്ജകം_ty last_elem = elem;
                Py_ssize_t j;
                വേണ്ടി (j = i; j < n_flattened_elements; j++) {
                    വ്യഞ്ജകം_ty current_elem = asdl_seq_GET(flattened, j);
                    എങ്കിൽ (current_elem->kind == Constant_kind) {
                        എങ്കിൽ (PyUnicodeWriter_WriteStr(writer,
                                                     current_elem->v.Constant.മൂല്യം)) {
                            PyUnicodeWriter_Discard(writer);
                            തിരികെ ശൂന്യ;
                        }
                        last_elem = current_elem;
                    } അല്ലെങ്കിൽ {
                        നിർത്തുക;
                    }
                }
                i = j - 1;

                PyObject *concat_str = PyUnicodeWriter_Finish(writer);
                എങ്കിൽ (concat_str == ശൂന്യ) {
                    തിരികെ ശൂന്യ;
                }
                എങ്കിൽ (_PyArena_AddPyObject(p->arena, concat_str) < 0) {
                    Py_DECREF(concat_str);
                    തിരികെ ശൂന്യ;
                }
                elem = _PyAST_Constant(concat_str, kind, first_elem->lineno,
                                       first_elem->col_offset,
                                       last_elem->end_lineno,
                                       last_elem->end_col_offset, p->arena);
                എങ്കിൽ (elem == ശൂന്യ) {
                    തിരികെ ശൂന്യ;
                }
            }

            /* Drop all empty contanst strings */
            എങ്കിൽ (PyUnicode_CheckExact(elem->v.Constant.മൂല്യം) &&
                PyUnicode_GET_LENGTH(elem->v.Constant.മൂല്യം) == 0) {
                തുടരുക;
            }
        }

        asdl_seq_SET(മൂല്യങ്ങൾ, current_pos++, elem);
    }

    പ്രതിജ്ഞാ(current_pos == n_elements);
    തിരികെ മൂല്യങ്ങൾ;
}

സ്ഥിതം വ്യഞ്ജകം_ty
_build_concatenated_joined_str(Parser *p, asdl_വ്യഞ്ജകം_seq *strings,
                               പൂർണ്ണസംഖ്യ lineno, പൂർണ്ണസംഖ്യ col_offset, പൂർണ്ണസംഖ്യ end_lineno,
                               പൂർണ്ണസംഖ്യ end_col_offset, PyArena *arena)
{
    asdl_വ്യഞ്ജകം_seq *മൂല്യങ്ങൾ = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    എങ്കിൽ (!മൂല്യങ്ങൾ) {
        തിരികെ ശൂന്യ;
    }
    തിരികെ _PyAST_JoinedStr(മൂല്യങ്ങൾ, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

വ്യഞ്ജകം_ty
_PyPegen_concatenate_tstrings(Parser *p, asdl_വ്യഞ്ജകം_seq *strings,
                               പൂർണ്ണസംഖ്യ lineno, പൂർണ്ണസംഖ്യ col_offset, പൂർണ്ണസംഖ്യ end_lineno,
                               പൂർണ്ണസംഖ്യ end_col_offset, PyArena *arena)
{
    asdl_വ്യഞ്ജകം_seq *മൂല്യങ്ങൾ = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    എങ്കിൽ (!മൂല്യങ്ങൾ) {
        തിരികെ ശൂന്യ;
    }
    തിരികെ _PyAST_TemplateStr(മൂല്യങ്ങൾ, lineno, col_offset, end_lineno,
        end_col_offset, arena);
}

വ്യഞ്ജകം_ty
_PyPegen_concatenate_strings(Parser *p, asdl_വ്യഞ്ജകം_seq *strings,
                             പൂർണ്ണസംഖ്യ lineno, പൂർണ്ണസംഖ്യ col_offset, പൂർണ്ണസംഖ്യ end_lineno,
                             പൂർണ്ണസംഖ്യ end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    പ്രതിജ്ഞാ(len > 0);

    പൂർണ്ണസംഖ്യ f_string_found = 0;
    പൂർണ്ണസംഖ്യ unicode_string_found = 0;
    പൂർണ്ണസംഖ്യ bytes_found = 0;

    Py_ssize_t i = 0;
    വേണ്ടി (i = 0; i < len; i++) {
        വ്യഞ്ജകം_ty elem = asdl_seq_GET(strings, i);
        തിരഞ്ഞെടുപ്പ്(elem->kind) {
            അവസ്ഥ Constant_kind:
                എങ്കിൽ (PyBytes_CheckExact(elem->v.Constant.മൂല്യം)) {
                    bytes_found = 1;
                } അല്ലെങ്കിൽ {
                    unicode_string_found = 1;
                }
                നിർത്തുക;
            അവസ്ഥ JoinedStr_kind:
                f_string_found = 1;
                നിർത്തുക;
            അവസ്ഥ TemplateStr_kind:
                // python.gram handles this; we should never get here
                പ്രതിജ്ഞാ(0);
                നിർത്തുക;
            സ്ഥിരസ്ഥിതി:
                f_string_found = 1;
                നിർത്തുക;
        }
    }

    // Cannot mix unicode and bytes
    എങ്കിൽ ((unicode_string_found || f_string_found) && bytes_found) {
        RAISE_SYNTAX_ERROR("cannot mix bytes and nonbytes literals");
        തിരികെ ശൂന്യ;
    }

    // If it's only bytes or only unicode string, do a simple concat
    എങ്കിൽ (!f_string_found) {
        എങ്കിൽ (len == 1) {
            തിരികെ asdl_seq_GET(strings, 0);
        }
        അല്ലെങ്കിൽ എങ്കിൽ (bytes_found) {
            തിരികെ _build_concatenated_bytes(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
        അല്ലെങ്കിൽ {
            തിരികെ _build_concatenated_unicode(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
    }

    തിരികെ _build_concatenated_joined_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
_warn_relative_import_of_lazy(Parser *p, asdl_seq *dots, വ്യഞ്ജകം_ty module)
{
    // Warn about `from . lazy import x`: the whitespace between the dots and
    // the module name is insignificant, so this is parsed exactly like
    // `from .lazy import x` (an import of the relative module "lazy"), but it
    // is most likely a transposition of `lazy from . import x` (PEP 810).
    എങ്കിൽ (p->call_invalid_rules) {
        തിരികെ 0;
    }

    // Only fire if there is whitespace between the last dot and the name,
    // i.e. not for the common `from .lazy import x` spelling.
    Token *last_dot = asdl_seq_GET_UNTYPED(dots, asdl_seq_LEN(dots) - 1);
    എങ്കിൽ (
        last_dot->end_lineno == module->lineno
        && last_dot->end_col_offset == module->col_offset
    ) {
        തിരികെ 0;
    }

    പൂർണ്ണസംഖ്യ എണ്ണം = Py_Pegen_seq_എണ്ണം_dots(dots);
    അക്ഷരം *buf = PyMem_RawMalloc(എണ്ണം + 1);
    എങ്കിൽ (buf == ശൂന്യ) {
        PyErr_NoMemory();
        തിരികെ -1;
    }
    memset(buf, '.', എണ്ണം);
    buf[എണ്ണം] = '\0';

    PyObject *സന്ദേശം = PyUnicode_FromFormat(
        "'from %s lazy import' is the same as 'from %slazy import'; "
        "did you mean 'lazy from %s import'?",
        buf, buf, buf);
    PyMem_RawFree(buf);
    എങ്കിൽ (സന്ദേശം == ശൂന്യ) {
        തിരികെ -1;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    പൂർണ്ണസംഖ്യ res = _PyErr_EmitSyntaxWarning(സന്ദേശം,
                                       info.filename,
                                       module->lineno,
                                       module->col_offset + 1,
                                       module->end_lineno,
                                       module->end_col_offset + 1,
                                       info.module);
    Py_DECREF(സന്ദേശം);
    തിരികെ res;
}

stmt_ty
_PyPegen_checked_from_import(Parser *p, asdl_seq *dots, വ്യഞ്ജകം_ty module_name,
                             asdl_alias_seq *names, വ്യഞ്ജകം_ty lazy_token, പൂർണ്ണസംഖ്യ lineno,
                             പൂർണ്ണസംഖ്യ col_offset, പൂർണ്ണസംഖ്യ end_lineno, പൂർണ്ണസംഖ്യ end_col_offset,
                             PyArena *arena)
{
    identifier module = module_name->v.Name.id;
    പൂർണ്ണസംഖ്യ level = Py_Pegen_seq_എണ്ണം_dots(dots);
    എങ്കിൽ (level == 0 && PyUnicode_CompareWithASCIIString(module, "__future__") == 0) {
        എങ്കിൽ (lazy_token) {
            RAISE_SYNTAX_ERROR_KNOWN_LOCATION(lazy_token,
                "lazy from __future__ import is not allowed");
            തിരികെ ശൂന്യ;
        }
        വേണ്ടി (Py_ssize_t i = 0; i < asdl_seq_LEN(names); i++) {
            alias_ty alias = asdl_seq_GET(names, i);
            എങ്കിൽ (PyUnicode_CompareWithASCIIString(alias->name, "barry_as_FLUFL") == 0) {
                p->flags |= PyPARSE_BARRY_AS_BDFL;
                p->tok->barry_as_bdfl = 1;
            }
        }
    }
    അല്ലെങ്കിൽ എങ്കിൽ (
        level > 0
        && lazy_token == ശൂന്യ
        && PyUnicode_CompareWithASCIIString(module, "lazy") == 0
    ) {
        എങ്കിൽ (_warn_relative_import_of_lazy(p, dots, module_name) < 0) {
            തിരികെ ശൂന്യ;
        }
    }
    തിരികെ _PyAST_ImportFrom(module, names, level, lazy_token ? 1 : 0, lineno,
                             col_offset, end_lineno, end_col_offset, arena);
}

asdl_stmt_seq*
_PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts) {
    എങ്കിൽ (!p->call_invalid_rules) {
        തിരികെ stmts;
    }
    Py_ssize_t len = asdl_seq_LEN(stmts);
    എങ്കിൽ (len == 0) {
        തിരികെ stmts;
    }
    stmt_ty last_stmt = asdl_seq_GET(stmts, len - 1);
    എങ്കിൽ (p->last_stmt_location.lineno > last_stmt->lineno) {
        തിരികെ stmts;
    }
    p->last_stmt_location.lineno = last_stmt->lineno;
    p->last_stmt_location.col_offset = last_stmt->col_offset;
    p->last_stmt_location.end_lineno = last_stmt->end_lineno;
    p->last_stmt_location.end_col_offset = last_stmt->end_col_offset;
    തിരികെ stmts;
}
