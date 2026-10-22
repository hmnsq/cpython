#অন্তর্ভাৱ <Python.h>
#অন্তর্ভাৱ "pycore_pystate.h"         // _PyInterpreterState_GET()
#অন্তর্ভাৱ "pycore_runtime.h"         // _PyRuntime
#অন্তর্ভাৱ "pycore_unicodeobject.h"   // _PyUnicode_InternImmortal()
#অন্তর্ভাৱ "pycore_pyerrors.h"        // _PyErr_EmitSyntaxWarning()

#অন্তর্ভাৱ "pegen.h"
#অন্তর্ভাৱ "string_parser.h"          // _PyPegen_decode_string()
#অন্তর্ভাৱ "lexer/state.h"            // tok_state


শূন্য *
_PyPegen_dummy_name(Parser *p, ...)
{
    ঘূৰাই &_PyRuntime.parser.dummy_name;
}

/* Creates a single-element asdl_seq* that contains a */
asdl_seq *
_PyPegen_singleton_seq(Parser *p, শূন্য *a)
{
    প্রতিজ্ঞা(a != রিক্ত);
    asdl_seq *seq = (asdl_seq*)_Py_asdl_generic_seq_new(1, p->arena);
    যদি (!seq) {
        ঘূৰাই রিক্ত;
    }
    asdl_seq_SET_UNTYPED(seq, 0, a);
    ঘূৰাই seq;
}

/* Creates a copy of seq and prepends a to it */
asdl_seq *
Py_Pegen_seq_ঢোকাও_in_front(Parser *p, শূন্য *a, asdl_seq *seq)
{
    প্রতিজ্ঞা(a != রিক্ত);
    যদি (!seq) {
        ঘূৰাই _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    যদি (!new_seq) {
        ঘূৰাই রিক্ত;
    }

    asdl_seq_SET_UNTYPED(new_seq, 0, a);
    বাবে (Py_ssize_t i = 1, l = asdl_seq_LEN(new_seq); i < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i - 1));
    }
    ঘূৰাই new_seq;
}

/* Creates a copy of seq and appends a to it */
asdl_seq *
_PyPegen_seq_append_to_end(Parser *p, asdl_seq *seq, শূন্য *a)
{
    প্রতিজ্ঞা(a != রিক্ত);
    যদি (!seq) {
        ঘূৰাই _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    যদি (!new_seq) {
        ঘূৰাই রিক্ত;
    }

    বাবে (Py_ssize_t i = 0, l = asdl_seq_LEN(new_seq); i + 1 < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i));
    }
    asdl_seq_SET_UNTYPED(new_seq, asdl_seq_LEN(new_seq) - 1, a);
    ঘূৰাই new_seq;
}

স্থির Py_ssize_t
_get_flattened_seq_size(asdl_seq *seqs)
{
    Py_ssize_t size = 0;
    বাবে (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        size += asdl_seq_LEN(inner_seq);
    }
    ঘূৰাই size;
}

/* Flattens an asdl_seq* of asdl_seq*s */
asdl_seq *
_PyPegen_seq_flatten(Parser *p, asdl_seq *seqs)
{
    Py_ssize_t flattened_seq_size = _get_flattened_seq_size(seqs);
    প্রতিজ্ঞা(flattened_seq_size > 0);

    asdl_seq *flattened_seq = (asdl_seq*)_Py_asdl_generic_seq_new(flattened_seq_size, p->arena);
    যদি (!flattened_seq) {
        ঘূৰাই রিক্ত;
    }

    পূর্ণাঙ্ক flattened_seq_idx = 0;
    বাবে (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        বাবে (Py_ssize_t j = 0, li = asdl_seq_LEN(inner_seq); j < li; j++) {
            asdl_seq_SET_UNTYPED(flattened_seq, flattened_seq_idx++, asdl_seq_GET_UNTYPED(inner_seq, j));
        }
    }
    প্রতিজ্ঞা(flattened_seq_idx == flattened_seq_size);

    ঘূৰাই flattened_seq;
}

শূন্য *
_PyPegen_seq_last_item(asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ঘূৰাই asdl_seq_GET_UNTYPED(seq, len - 1);
}

শূন্য *
_PyPegen_seq_first_item(asdl_seq *seq)
{
    ঘূৰাই asdl_seq_GET_UNTYPED(seq, 0);
}

/* Creates a new name of the form <first_name>.<second_name> */
ৰাশিমালা_ty
_PyPegen_join_names_with_dot(Parser *p, ৰাশিমালা_ty first_name, ৰাশিমালা_ty second_name)
{
    প্রতিজ্ঞা(first_name != রিক্ত && second_name != রিক্ত);
    PyObject *uni = PyUnicode_FromFormat("%U.%U",
            first_name->v.Name.id, second_name->v.Name.id);
    যদি (!uni) {
        ঘূৰাই রিক্ত;
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &uni);
    যদি (_PyArena_AddPyObject(p->arena, uni) < 0) {
        Py_DECREF(uni);
        ঘূৰাই রিক্ত;
    }

    ঘূৰাই _PyAST_Name(uni, Load, EXTRA_ৰাশিমালা(first_name, second_name));
}

/* Counts the total number of dots in seq's tokens */
পূর্ণাঙ্ক
Py_Pegen_seq_গুনতি_dots(asdl_seq *seq)
{
    পূর্ণাঙ্ক সংখ্যা_of_dots = 0;
    বাবে (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        Token *current_ৰাশিমালা = asdl_seq_GET_UNTYPED(seq, i);
        ৱিকল্প (current_ৰাশিমালা->type) {
            পক্ষ ELLIPSIS:
                সংখ্যা_of_dots += 3;
                ভঙ্গ;
            পক্ষ DOT:
                সংখ্যা_of_dots += 1;
                ভঙ্গ;
            স্ৱভাৱ:
                Py_UNREACHABLE();
        }
    }

    ঘূৰাই সংখ্যা_of_dots;
}

/* Creates an alias with '*' as the identifier name */
alias_ty
_PyPegen_alias_for_star(Parser *p, পূর্ণাঙ্ক lineno, পূর্ণাঙ্ক col_offset, পূর্ণাঙ্ক end_lineno,
                        পূর্ণাঙ্ক end_col_offset, PyArena *arena) {
    PyObject *str = PyUnicode_InternFromString("*");
    যদি (!str) {
        ঘূৰাই রিক্ত;
    }
    যদি (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই _PyAST_alias(str, রিক্ত, lineno, col_offset, end_lineno, end_col_offset, arena);
}

/* Creates a new asdl_seq* with the identifiers of all the names in seq */
asdl_identifier_seq *
_PyPegen_map_names_to_ids(Parser *p, asdl_ৰাশিমালা_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    প্রতিজ্ঞা(len > 0);

    asdl_identifier_seq *new_seq = _Py_asdl_identifier_seq_new(len, p->arena);
    যদি (!new_seq) {
        ঘূৰাই রিক্ত;
    }
    বাবে (Py_ssize_t i = 0; i < len; i++) {
        ৰাশিমালা_ty e = asdl_seq_GET(seq, i);
        asdl_seq_SET(new_seq, i, e->v.Name.id);
    }
    ঘূৰাই new_seq;
}

/* Constructs a CmpopExprPair */
Cmpop_ৰাশিমালা_Pair *
Py_Pegen_cmpop_ৰাশিমালা_pair(Parser *p, cmpop_ty cmpop, ৰাশিমালা_ty ৰাশিমালা)
{
    প্রতিজ্ঞা(ৰাশিমালা != রিক্ত);
    Cmpop_ৰাশিমালা_Pair *a = _PyArena_Malloc(p->arena, পরিমাণ(Cmpop_ৰাশিমালা_Pair));
    যদি (!a) {
        ঘূৰাই রিক্ত;
    }
    a->cmpop = cmpop;
    a->ৰাশিমালা = ৰাশিমালা;
    ঘূৰাই a;
}

asdl_int_seq *
_PyPegen_get_cmpops(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    প্রতিজ্ঞা(len > 0);

    asdl_int_seq *new_seq = _Py_asdl_int_seq_new(len, p->arena);
    যদি (!new_seq) {
        ঘূৰাই রিক্ত;
    }
    বাবে (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_ৰাশিমালা_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->cmpop);
    }
    ঘূৰাই new_seq;
}

asdl_ৰাশিমালা_seq *
_PyPegen_get_exprs(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    প্রতিজ্ঞা(len > 0);

    asdl_ৰাশিমালা_seq *new_seq = Py_asdl_ৰাশিমালা_seq_new(len, p->arena);
    যদি (!new_seq) {
        ঘূৰাই রিক্ত;
    }
    বাবে (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_ৰাশিমালা_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->ৰাশিমালা);
    }
    ঘূৰাই new_seq;
}

/* Creates an asdl_seq* where all the elements have been changed to have ctx as context */
স্থির asdl_ৰাশিমালা_seq *
_set_seq_context(Parser *p, asdl_ৰাশিমালা_seq *seq, ৰাশিমালা_context_ty ctx)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    যদি (len == 0) {
        ঘূৰাই রিক্ত;
    }

    asdl_ৰাশিমালা_seq *new_seq = Py_asdl_ৰাশিমালা_seq_new(len, p->arena);
    যদি (!new_seq) {
        ঘূৰাই রিক্ত;
    }
    বাবে (Py_ssize_t i = 0; i < len; i++) {
        ৰাশিমালা_ty e = asdl_seq_GET(seq, i);
        ৰাশিমালা_ty new_e = Py_Pegen_set_ৰাশিমালা_context(p, e, ctx);
        যদি (!new_e) {
            ঘূৰাই রিক্ত;
        }
        asdl_seq_SET(new_seq, i, new_e);
    }
    ঘূৰাই new_seq;
}

স্থির ৰাশিমালা_ty
_set_name_context(Parser *p, ৰাশিমালা_ty e, ৰাশিমালা_context_ty ctx)
{
    ঘূৰাই _PyAST_Name(e->v.Name.id, ctx, EXTRA_ৰাশিমালা(e, e));
}

স্থির ৰাশিমালা_ty
_set_tuple_context(Parser *p, ৰাশিমালা_ty e, ৰাশিমালা_context_ty ctx)
{
    asdl_ৰাশিমালা_seq *seq = _set_seq_context(p, e->v.Tuple.elts, ctx);
    যদি (!seq && PyErr_Occurred()) {
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই _PyAST_Tuple(seq, ctx, EXTRA_ৰাশিমালা(e, e));
}

স্থির ৰাশিমালা_ty
_set_list_context(Parser *p, ৰাশিমালা_ty e, ৰাশিমালা_context_ty ctx)
{
    asdl_ৰাশিমালা_seq *seq = _set_seq_context(p, e->v.List.elts, ctx);
    যদি (!seq && PyErr_Occurred()) {
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই _PyAST_List(seq, ctx, EXTRA_ৰাশিমালা(e, e));
}

স্থির ৰাশিমালা_ty
_set_subscript_context(Parser *p, ৰাশিমালা_ty e, ৰাশিমালা_context_ty ctx)
{
    ঘূৰাই _PyAST_Subscript(e->v.Subscript.মান, e->v.Subscript.slice,
                            ctx, EXTRA_ৰাশিমালা(e, e));
}

স্থির ৰাশিমালা_ty
_set_attribute_context(Parser *p, ৰাশিমালা_ty e, ৰাশিমালা_context_ty ctx)
{
    ঘূৰাই _PyAST_Attribute(e->v.Attribute.মান, e->v.Attribute.attr,
                            ctx, EXTRA_ৰাশিমালা(e, e));
}

স্থির ৰাশিমালা_ty
_set_starred_context(Parser *p, ৰাশিমালা_ty e, ৰাশিমালা_context_ty ctx)
{
    ৰাশিমালা_ty inner = Py_Pegen_set_ৰাশিমালা_context(p, e->v.Starred.মান, ctx);
    যদি (!inner) {
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই _PyAST_Starred(inner, ctx, EXTRA_ৰাশিমালা(e, e));
}

/* Creates an `expr_ty` equivalent to `expr` but with `ctx` as context */
ৰাশিমালা_ty
Py_Pegen_set_ৰাশিমালা_context(Parser *p, ৰাশিমালা_ty ৰাশিমালা, ৰাশিমালা_context_ty ctx)
{
    প্রতিজ্ঞা(ৰাশিমালা != রিক্ত);

    ৰাশিমালা_ty new = রিক্ত;
    ৱিকল্প (ৰাশিমালা->kind) {
        পক্ষ Name_kind:
            new = _set_name_context(p, ৰাশিমালা, ctx);
            ভঙ্গ;
        পক্ষ Tuple_kind:
            new = _set_tuple_context(p, ৰাশিমালা, ctx);
            ভঙ্গ;
        পক্ষ List_kind:
            new = _set_list_context(p, ৰাশিমালা, ctx);
            ভঙ্গ;
        পক্ষ Subscript_kind:
            new = _set_subscript_context(p, ৰাশিমালা, ctx);
            ভঙ্গ;
        পক্ষ Attribute_kind:
            new = _set_attribute_context(p, ৰাশিমালা, ctx);
            ভঙ্গ;
        পক্ষ Starred_kind:
            new = _set_starred_context(p, ৰাশিমালা, ctx);
            ভঙ্গ;
        স্ৱভাৱ:
            new = ৰাশিমালা;
    }
    ঘূৰাই new;
}

/* Invalid targets return NULL without raising, allowing parsing to backtrack.
   Copy valid targets to preserve the memoized expression's context. */
ৰাশিমালা_ty
_PyPegen_make_target(Parser *p, ৰাশিমালা_ty ৰাশিমালা, TARGETS_TYPE targets_type)
{
    প্রতিজ্ঞা(ৰাশিমালা != রিক্ত);
    প্রতিজ্ঞা(targets_type != FOR_TARGETS);
    যদি (_PyPegen_get_invalid_target(ৰাশিমালা, targets_type) != রিক্ত) {
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই Py_Pegen_set_ৰাশিমালা_context(
        p, ৰাশিমালা, targets_type == DEL_TARGETS ? Del : Store);
}

/* Constructs a KeyValuePair that is used when parsing a dict's key value pairs */
চাবি_মান_Pair *
Py_Pegen_চাবি_মান_pair(Parser *p, ৰাশিমালা_ty চাবি, ৰাশিমালা_ty মান)
{
    চাবি_মান_Pair *a = _PyArena_Malloc(p->arena, পরিমাণ(চাবি_মান_Pair));
    যদি (!a) {
        ঘূৰাই রিক্ত;
    }
    a->চাবি = চাবি;
    a->মান = মান;
    ঘূৰাই a;
}

/* Extracts all keys from an asdl_seq* of KeyValuePair*'s */
asdl_ৰাশিমালা_seq *
Py_Pegen_get_চাবিগুলি(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_ৰাশিমালা_seq *new_seq = Py_asdl_ৰাশিমালা_seq_new(len, p->arena);
    যদি (!new_seq) {
        ঘূৰাই রিক্ত;
    }
    বাবে (Py_ssize_t i = 0; i < len; i++) {
        চাবি_মান_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->চাবি);
    }
    ঘূৰাই new_seq;
}

/* Extracts all values from an asdl_seq* of KeyValuePair*'s */
asdl_ৰাশিমালা_seq *
Py_Pegen_get_মানসমূহ(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_ৰাশিমালা_seq *new_seq = Py_asdl_ৰাশিমালা_seq_new(len, p->arena);
    যদি (!new_seq) {
        ঘূৰাই রিক্ত;
    }
    বাবে (Py_ssize_t i = 0; i < len; i++) {
        চাবি_মান_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->মান);
    }
    ঘূৰাই new_seq;
}

/* Constructs a KeyPatternPair that is used when parsing mapping & class patterns */
চাবি_Pattern_Pair *
Py_Pegen_চাবি_pattern_pair(Parser *p, ৰাশিমালা_ty চাবি, pattern_ty pattern)
{
    চাবি_Pattern_Pair *a = _PyArena_Malloc(p->arena, পরিমাণ(চাবি_Pattern_Pair));
    যদি (!a) {
        ঘূৰাই রিক্ত;
    }
    a->চাবি = চাবি;
    a->pattern = pattern;
    ঘূৰাই a;
}

/* Extracts all keys from an asdl_seq* of KeyPatternPair*'s */
asdl_ৰাশিমালা_seq *
Py_Pegen_get_pattern_চাবিগুলি(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_ৰাশিমালা_seq *new_seq = Py_asdl_ৰাশিমালা_seq_new(len, p->arena);
    যদি (!new_seq) {
        ঘূৰাই রিক্ত;
    }
    বাবে (Py_ssize_t i = 0; i < len; i++) {
        চাবি_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->চাবি);
    }
    ঘূৰাই new_seq;
}

/* Extracts all patterns from an asdl_seq* of KeyPatternPair*'s */
asdl_pattern_seq *
_PyPegen_get_patterns(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_pattern_seq *new_seq = _Py_asdl_pattern_seq_new(len, p->arena);
    যদি (!new_seq) {
        ঘূৰাই রিক্ত;
    }
    বাবে (Py_ssize_t i = 0; i < len; i++) {
        চাবি_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->pattern);
    }
    ঘূৰাই new_seq;
}

/* Constructs a NameDefaultPair */
NameDefaultPair *
_PyPegen_name_default_pair(Parser *p, arg_ty arg, ৰাশিমালা_ty মান, Token *tc)
{
    NameDefaultPair *a = _PyArena_Malloc(p->arena, পরিমাণ(NameDefaultPair));
    যদি (!a) {
        ঘূৰাই রিক্ত;
    }
    a->arg = _PyPegen_add_type_comment_to_arg(p, arg, tc);
    যদি (!a->arg) {
        ঘূৰাই রিক্ত;
    }
    a->মান = মান;
    ঘূৰাই a;
}

/* Constructs a SlashWithDefault */
SlashWithDefault *
_PyPegen_slash_with_default(Parser *p, asdl_arg_seq *plain_names, asdl_seq *names_with_defaults)
{
    SlashWithDefault *a = _PyArena_Malloc(p->arena, পরিমাণ(SlashWithDefault));
    যদি (!a) {
        ঘূৰাই রিক্ত;
    }
    a->plain_names = plain_names;
    a->names_with_defaults = names_with_defaults;
    ঘূৰাই a;
}

/* Constructs a StarEtc */
StarEtc *
_PyPegen_star_etc(Parser *p, arg_ty vararg, asdl_seq *kwonlyargs, arg_ty kwarg)
{
    StarEtc *a = _PyArena_Malloc(p->arena, পরিমাণ(StarEtc));
    যদি (!a) {
        ঘূৰাই রিক্ত;
    }
    a->vararg = vararg;
    a->kwonlyargs = kwonlyargs;
    a->kwarg = kwarg;
    ঘূৰাই a;
}

asdl_seq *
_PyPegen_join_sequences(Parser *p, asdl_seq *a, asdl_seq *b)
{
    Py_ssize_t first_len = asdl_seq_LEN(a);
    Py_ssize_t second_len = asdl_seq_LEN(b);
    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(first_len + second_len, p->arena);
    যদি (!new_seq) {
        ঘূৰাই রিক্ত;
    }

    পূর্ণাঙ্ক k = 0;
    বাবে (Py_ssize_t i = 0; i < first_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(a, i));
    }
    বাবে (Py_ssize_t i = 0; i < second_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(b, i));
    }

    ঘূৰাই new_seq;
}

স্থির asdl_arg_seq*
_get_names(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_arg_seq *seq = _Py_asdl_arg_seq_new(len, p->arena);
    যদি (!seq) {
        ঘূৰাই রিক্ত;
    }
    বাবে (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->arg);
    }
    ঘূৰাই seq;
}

স্থির asdl_ৰাশিমালা_seq *
_get_defaults(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_ৰাশিমালা_seq *seq = Py_asdl_ৰাশিমালা_seq_new(len, p->arena);
    যদি (!seq) {
        ঘূৰাই রিক্ত;
    }
    বাবে (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->মান);
    }
    ঘূৰাই seq;
}

স্থির পূর্ণাঙ্ক
_make_posonlyargs(Parser *p,
                  asdl_arg_seq *slash_without_default,
                  SlashWithDefault *slash_with_default,
                  asdl_arg_seq **posonlyargs) {
    যদি (slash_without_default != রিক্ত) {
        *posonlyargs = slash_without_default;
    }
    অন্যথা যদি (slash_with_default != রিক্ত) {
        asdl_arg_seq *slash_with_default_names =
                _get_names(p, slash_with_default->names_with_defaults);
        যদি (!slash_with_default_names) {
            ঘূৰাই -1;
        }
        *posonlyargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default->plain_names,
                (asdl_seq*)slash_with_default_names);
    }
    অন্যথা {
        *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }
    ঘূৰাই *posonlyargs == রিক্ত ? -1 : 0;
}

স্থির পূর্ণাঙ্ক
_make_posargs(Parser *p,
              asdl_arg_seq *plain_names,
              asdl_seq *names_with_default,
              asdl_arg_seq **posargs) {

    যদি (names_with_default != রিক্ত) {
        যদি (plain_names != রিক্ত) {
            asdl_arg_seq *names_with_default_names = _get_names(p, names_with_default);
            যদি (!names_with_default_names) {
                ঘূৰাই -1;
            }
            *posargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                    p,(asdl_seq*)plain_names, (asdl_seq*)names_with_default_names);
        }
        অন্যথা {
            *posargs = _get_names(p, names_with_default);
        }
    }
    অন্যথা {
        যদি (plain_names != রিক্ত) {
            // With the current grammar, we never get here.
            // If that has changed, remove the assert, and test thoroughly.
            প্রতিজ্ঞা(0);
            *posargs = plain_names;
        }
        অন্যথা {
            *posargs = _Py_asdl_arg_seq_new(0, p->arena);
        }
    }
    ঘূৰাই *posargs == রিক্ত ? -1 : 0;
}

স্থির পূর্ণাঙ্ক
_make_posdefaults(Parser *p,
                  SlashWithDefault *slash_with_default,
                  asdl_seq *names_with_default,
                  asdl_ৰাশিমালা_seq **posdefaults) {
    যদি (slash_with_default != রিক্ত && names_with_default != রিক্ত) {
        asdl_ৰাশিমালা_seq *slash_with_default_মানসমূহ =
                _get_defaults(p, slash_with_default->names_with_defaults);
        যদি (!slash_with_default_মানসমূহ) {
            ঘূৰাই -1;
        }
        asdl_ৰাশিমালা_seq *names_with_default_মানসমূহ = _get_defaults(p, names_with_default);
        যদি (!names_with_default_মানসমূহ) {
            ঘূৰাই -1;
        }
        *posdefaults = (asdl_ৰাশিমালা_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default_মানসমূহ,
                (asdl_seq*)names_with_default_মানসমূহ);
    }
    অন্যথা যদি (slash_with_default == রিক্ত && names_with_default != রিক্ত) {
        *posdefaults = _get_defaults(p, names_with_default);
    }
    অন্যথা যদি (slash_with_default != রিক্ত && names_with_default == রিক্ত) {
        *posdefaults = _get_defaults(p, slash_with_default->names_with_defaults);
    }
    অন্যথা {
        *posdefaults = Py_asdl_ৰাশিমালা_seq_new(0, p->arena);
    }
    ঘূৰাই *posdefaults == রিক্ত ? -1 : 0;
}

স্থির পূর্ণাঙ্ক
_make_kwargs(Parser *p, StarEtc *star_etc,
             asdl_arg_seq **kwonlyargs,
             asdl_ৰাশিমালা_seq **kwdefaults) {
    যদি (star_etc != রিক্ত && star_etc->kwonlyargs != রিক্ত) {
        *kwonlyargs = _get_names(p, star_etc->kwonlyargs);
    }
    অন্যথা {
        *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }

    যদি (*kwonlyargs == রিক্ত) {
        ঘূৰাই -1;
    }

    যদি (star_etc != রিক্ত && star_etc->kwonlyargs != রিক্ত) {
        *kwdefaults = _get_defaults(p, star_etc->kwonlyargs);
    }
    অন্যথা {
        *kwdefaults = Py_asdl_ৰাশিমালা_seq_new(0, p->arena);
    }

    যদি (*kwdefaults == রিক্ত) {
        ঘূৰাই -1;
    }

    ঘূৰাই 0;
}

/* Constructs an arguments_ty object out of all the parsed constructs in the parameters rule */
arguments_ty
_PyPegen_make_arguments(Parser *p, asdl_arg_seq *slash_without_default,
                        SlashWithDefault *slash_with_default, asdl_arg_seq *plain_names,
                        asdl_seq *names_with_default, StarEtc *star_etc)
{
    asdl_arg_seq *posonlyargs;
    যদি (_make_posonlyargs(p, slash_without_default, slash_with_default, &posonlyargs) == -1) {
        ঘূৰাই রিক্ত;
    }

    asdl_arg_seq *posargs;
    যদি (_make_posargs(p, plain_names, names_with_default, &posargs) == -1) {
        ঘূৰাই রিক্ত;
    }

    asdl_ৰাশিমালা_seq *posdefaults;
    যদি (_make_posdefaults(p,slash_with_default, names_with_default, &posdefaults) == -1) {
        ঘূৰাই রিক্ত;
    }

    arg_ty vararg = রিক্ত;
    যদি (star_etc != রিক্ত && star_etc->vararg != রিক্ত) {
        vararg = star_etc->vararg;
    }

    asdl_arg_seq *kwonlyargs;
    asdl_ৰাশিমালা_seq *kwdefaults;
    যদি (_make_kwargs(p, star_etc, &kwonlyargs, &kwdefaults) == -1) {
        ঘূৰাই রিক্ত;
    }

    arg_ty kwarg = রিক্ত;
    যদি (star_etc != রিক্ত && star_etc->kwarg != রিক্ত) {
        kwarg = star_etc->kwarg;
    }

    ঘূৰাই _PyAST_arguments(posonlyargs, posargs, vararg, kwonlyargs,
                            kwdefaults, kwarg, posdefaults, p->arena);
}


/* Constructs an empty arguments_ty object, that gets used when a function accepts no
 * arguments. */
arguments_ty
_PyPegen_empty_arguments(Parser *p)
{
    asdl_arg_seq *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    যদি (!posonlyargs) {
        ঘূৰাই রিক্ত;
    }
    asdl_arg_seq *posargs = _Py_asdl_arg_seq_new(0, p->arena);
    যদি (!posargs) {
        ঘূৰাই রিক্ত;
    }
    asdl_ৰাশিমালা_seq *posdefaults = Py_asdl_ৰাশিমালা_seq_new(0, p->arena);
    যদি (!posdefaults) {
        ঘূৰাই রিক্ত;
    }
    asdl_arg_seq *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    যদি (!kwonlyargs) {
        ঘূৰাই রিক্ত;
    }
    asdl_ৰাশিমালা_seq *kwdefaults = Py_asdl_ৰাশিমালা_seq_new(0, p->arena);
    যদি (!kwdefaults) {
        ঘূৰাই রিক্ত;
    }

    ঘূৰাই _PyAST_arguments(posonlyargs, posargs, রিক্ত, kwonlyargs,
                            kwdefaults, রিক্ত, posdefaults, p->arena);
}

/* Encapsulates the value of an operator_ty into an AugOperator struct */
AugOperator *
_PyPegen_augoperator(Parser *p, operator_ty kind)
{
    AugOperator *a = _PyArena_Malloc(p->arena, পরিমাণ(AugOperator));
    যদি (!a) {
        ঘূৰাই রিক্ত;
    }
    a->kind = kind;
    ঘূৰাই a;
}

/* Construct a FunctionDef equivalent to function_def, but with decorators */
stmt_ty
_PyPegen_function_def_decorators(Parser *p, asdl_ৰাশিমালা_seq *decorators, stmt_ty function_def)
{
    প্রতিজ্ঞা(function_def != রিক্ত);
    যদি (function_def->kind == AsyncFunctionDef_kind) {
        ঘূৰাই _PyAST_AsyncFunctionDef(
            function_def->v.AsyncFunctionDef.name,
            function_def->v.AsyncFunctionDef.args,
            function_def->v.AsyncFunctionDef.body, decorators,
            function_def->v.AsyncFunctionDef.returns,
            function_def->v.AsyncFunctionDef.type_comment,
            function_def->v.AsyncFunctionDef.type_params,
            function_def->lineno, function_def->col_offset,
            function_def->end_lineno, function_def->end_col_offset, p->arena);
    }

    ঘূৰাই _PyAST_FunctionDef(
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
_PyPegen_class_def_decorators(Parser *p, asdl_ৰাশিমালা_seq *decorators, stmt_ty class_def)
{
    প্রতিজ্ঞা(class_def != রিক্ত);
    ঘূৰাই _PyAST_ClassDef(
        class_def->v.ClassDef.name,
        class_def->v.ClassDef.bases, class_def->v.ClassDef.keywords,
        class_def->v.ClassDef.body, decorators,
        class_def->v.ClassDef.type_params,
        class_def->lineno, class_def->col_offset, class_def->end_lineno,
        class_def->end_col_offset, p->arena);
}

/* Construct a KeywordOrStarred */
KeywordOrStarred *
_PyPegen_keyword_or_starred(Parser *p, শূন্য *element, পূর্ণাঙ্ক is_keyword)
{
    KeywordOrStarred *a = _PyArena_Malloc(p->arena, পরিমাণ(KeywordOrStarred));
    যদি (!a) {
        ঘূৰাই রিক্ত;
    }
    a->element = element;
    a->is_keyword = is_keyword;
    ঘূৰাই a;
}

/* Get the number of starred expressions in an asdl_seq* of KeywordOrStarred*s */
স্থির পূর্ণাঙ্ক
seq_সংখ্যা_of_starred_exprs(asdl_seq *seq)
{
    পূর্ণাঙ্ক n = 0;
    বাবে (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(seq, i);
        যদি (!k->is_keyword) {
            n++;
        }
    }
    ঘূৰাই n;
}

/* Extract the starred expressions of an asdl_seq* of KeywordOrStarred*s */
asdl_ৰাশিমালা_seq *
_PyPegen_seq_extract_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    পূর্ণাঙ্ক new_len = seq_সংখ্যা_of_starred_exprs(kwargs);
    যদি (new_len == 0) {
        ঘূৰাই রিক্ত;
    }
    asdl_ৰাশিমালা_seq *new_seq = Py_asdl_ৰাশিমালা_seq_new(new_len, p->arena);
    যদি (!new_seq) {
        ঘূৰাই রিক্ত;
    }

    পূর্ণাঙ্ক idx = 0;
    বাবে (Py_ssize_t i = 0, len = asdl_seq_LEN(kwargs); i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        যদি (!k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    ঘূৰাই new_seq;
}

/* Return a new asdl_seq* with only the keywords in kwargs */
asdl_keyword_seq*
_PyPegen_seq_delete_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    Py_ssize_t len = asdl_seq_LEN(kwargs);
    Py_ssize_t new_len = len - seq_সংখ্যা_of_starred_exprs(kwargs);
    যদি (new_len == 0) {
        ঘূৰাই রিক্ত;
    }
    asdl_keyword_seq *new_seq = _Py_asdl_keyword_seq_new(new_len, p->arena);
    যদি (!new_seq) {
        ঘূৰাই রিক্ত;
    }

    পূর্ণাঙ্ক idx = 0;
    বাবে (Py_ssize_t i = 0; i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        যদি (k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    ঘূৰাই new_seq;
}

ৰাশিমালা_ty
_PyPegen_ensure_imaginary(Parser *p, ৰাশিমালা_ty exp)
{
    যদি (exp->kind != Constant_kind || !PyComplex_CheckExact(exp->v.Constant.মান)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "imaginary number required in complex literal");
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই exp;
}

ৰাশিমালা_ty
_PyPegen_ensure_real(Parser *p, ৰাশিমালা_ty exp)
{
    যদি (exp->kind != Constant_kind || PyComplex_CheckExact(exp->v.Constant.মান)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "real number required in complex literal");
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই exp;
}

mod_ty
_PyPegen_make_module(Parser *p, asdl_stmt_seq *a) {
    asdl_type_ignore_seq *type_ignores = রিক্ত;
    Py_ssize_t অঙ্ক = p->type_ignore_comments.অঙ্ক_বস্তুসমূহ;
    যদি (অঙ্ক > 0) {
        // Turn the raw (comment, lineno) pairs into TypeIgnore objects in the arena
        type_ignores = _Py_asdl_type_ignore_seq_new(অঙ্ক, p->arena);
        যদি (type_ignores == রিক্ত) {
            ঘূৰাই রিক্ত;
        }
        বাবে (Py_ssize_t i = 0; i < অঙ্ক; i++) {
            PyObject *tag = _PyPegen_new_type_comment(p, p->type_ignore_comments.বস্তুসমূহ[i].comment);
            যদি (tag == রিক্ত) {
                ঘূৰাই রিক্ত;
            }
            type_ignore_ty ti = _PyAST_TypeIgnore(p->type_ignore_comments.বস্তুসমূহ[i].lineno,
                                                  tag, p->arena);
            যদি (ti == রিক্ত) {
                ঘূৰাই রিক্ত;
            }
            asdl_seq_SET(type_ignores, i, ti);
        }
    }
    ঘূৰাই _PyAST_Module(a, type_ignores, p->arena);
}

PyObject *
_PyPegen_new_type_comment(Parser *p, ধ্রুৱ ৱর্ণ *s)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, strlen(s), রিক্ত);
    যদি (res == রিক্ত) {
        ঘূৰাই রিক্ত;
    }
    যদি (_PyArena_AddPyObject(p->arena, res) < 0) {
        Py_DECREF(res);
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই res;
}

arg_ty
_PyPegen_add_type_comment_to_arg(Parser *p, arg_ty a, Token *tc)
{
    যদি (tc == রিক্ত) {
        ঘূৰাই a;
    }
    ধ্রুৱ ৱর্ণ *bytes = PyBytes_AsString(tc->bytes);
    যদি (bytes == রিক্ত) {
        ঘূৰাই রিক্ত;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    যদি (tco == রিক্ত) {
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই _PyAST_arg(a->arg, a->annotation, tco,
                      a->lineno, a->col_offset, a->end_lineno, a->end_col_offset,
                      p->arena);
}

/* Checks if the NOTEQUAL token is valid given the current parser flags
0 indicates success and nonzero indicates failure (an exception may be set) */
পূর্ণাঙ্ক
_PyPegen_check_barry_as_flufl(Parser *p, Token* t) {
    প্রতিজ্ঞা(t->bytes != রিক্ত);
    প্রতিজ্ঞা(t->type == NOTEQUAL);

    ধ্রুৱ ৱর্ণ* tok_str = PyBytes_AS_STRING(t->bytes);
    যদি (p->flags & PyPARSE_BARRY_AS_BDFL && strcmp(tok_str, "<>") != 0) {
        RAISE_SYNTAX_ERROR("with Barry as BDFL, use '<>' instead of '!='");
        ঘূৰাই -1;
    }
    যদি (!(p->flags & PyPARSE_BARRY_AS_BDFL)) {
        ঘূৰাই strcmp(tok_str, "!=");
    }
    ঘূৰাই 0;
}

পূর্ণাঙ্ক
_PyPegen_check_legacy_stmt(Parser *p, ৰাশিমালা_ty name) {
    যদি (name->kind != Name_kind) {
        ঘূৰাই 0;
    }
    ধ্রুৱ ৱর্ণ* candidates[2] = {"print", "exec"};
    বাবে (পূর্ণাঙ্ক i=0; i<2; i++) {
        যদি (PyUnicode_CompareWithASCIIString(name->v.Name.id, candidates[i]) == 0) {
            ঘূৰাই 1;
        }
    }
    ঘূৰাই 0;
}

শূন্য *
_PyPegen_raise_error_for_missing_comma(Parser *p, ৰাশিমালা_ty a, ৰাশিমালা_ty b)
{
    // Don't raise for legacy statements like "print x" or "exec x"
    যদি (_PyPegen_check_legacy_stmt(p, a)) {
        ঘূৰাই রিক্ত;
    }
    // Only raise inside parentheses/brackets (level > 0)
    যদি (p->tokens[p->mark - 1]->level == 0) {
        ঘূৰাই রিক্ত;
    }
    // For multi-line expressions (like string concatenations), point to the
    // last line instead of the first for a more helpful error message.
    // Use a->col_offset as the starting column since all strings in the
    // concatenation typically share the same indentation.
    যদি (a->end_lineno > a->lineno) {
        ঘূৰাই RAISE_ERROR_KNOWN_LOCATION(
            p, PyExc_SyntaxError, a->end_lineno, a->col_offset,
            a->end_lineno, a->end_col_offset,
            "invalid syntax. Perhaps you forgot a comma?"
        );
    }
    ঘূৰাই RAISE_ERROR_KNOWN_LOCATION(
        p, PyExc_SyntaxError, a->lineno, a->col_offset,
        b->end_lineno, b->end_col_offset,
        "invalid syntax. Perhaps you forgot a comma?"
    );
}

স্থির ResultTokenWithMetadata *
result_token_with_metadata(Parser *p, শূন্য *result, PyObject *metadata)
{
    ResultTokenWithMetadata *res = _PyArena_Malloc(p->arena, পরিমাণ(ResultTokenWithMetadata));
    যদি (res == রিক্ত) {
        ঘূৰাই রিক্ত;
    }
    res->metadata = metadata;
    res->result = result;
    ঘূৰাই res;
}

স্থির ৱর্ণ
formatted_string_prefix(ধ্রুৱ Parser *p)
{
    পূর্ণাঙ্ক nested = 0;
    বাবে (পূর্ণাঙ্ক i = p->mark - 1; i >= 0; i--) {
        পূর্ণাঙ্ক type = p->tokens[i]->type;
        যদি (type == FSTRING_END || type == TSTRING_END) {
            nested++;
        }
        অন্যথা যদি (type == FSTRING_START || type == TSTRING_START) {
            যদি (nested == 0) {
                ঘূৰাই type == TSTRING_START ? 't' : 'f';
            }
            nested--;
        }
    }
    Py_UNREACHABLE();
}

ResultTokenWithMetadata *
_PyPegen_check_fstring_conversion(Parser *p, Token* conv_token, ৰাশিমালা_ty conv)
{
    যদি (conv_token->lineno != conv->lineno || conv_token->end_col_offset != conv->col_offset) {
        ঘূৰাই RAISE_SYNTAX_ERROR_KNOWN_RANGE(
            conv_token, conv,
            "%c-string: conversion type must come right after the exclamation mark",
            formatted_string_prefix(p)
        );
    }

    Py_UCS4 first = PyUnicode_READ_CHAR(conv->v.Name.id, 0);
    যদি (PyUnicode_GET_LENGTH(conv->v.Name.id) > 1 ||
            !(first == 's' || first == 'r' || first == 'a')) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(conv,
                                            "%c-string: invalid conversion character %R: expected 's', 'r', or 'a'",
                                            formatted_string_prefix(p),
                                            conv->v.Name.id);
        ঘূৰাই রিক্ত;
    }

    ঘূৰাই result_token_with_metadata(p, conv, conv_token->metadata);
}

ResultTokenWithMetadata *
_PyPegen_setup_full_format_spec(Parser *p, Token *colon, asdl_ৰাশিমালা_seq *spec, পূর্ণাঙ্ক lineno, পূর্ণাঙ্ক col_offset,
                                পূর্ণাঙ্ক end_lineno, পূর্ণাঙ্ক end_col_offset, PyArena *arena)
{
    যদি (!spec) {
        ঘূৰাই রিক্ত;
    }

    // This is needed to keep compatibility with 3.11, where an empty format
    // spec is parsed as an *empty* JoinedStr node, instead of having an empty
    // constant in it.
    Py_ssize_t n_বস্তুসমূহ = asdl_seq_LEN(spec);
    Py_ssize_t non_empty_গুনতি = 0;
    বাবে (Py_ssize_t i = 0; i < n_বস্তুসমূহ; i++) {
        ৰাশিমালা_ty item = asdl_seq_GET(spec, i);
        non_empty_গুনতি += !(item->kind == Constant_kind &&
                             PyUnicode_CheckExact(item->v.Constant.মান) &&
                             PyUnicode_GET_LENGTH(item->v.Constant.মান) == 0);
    }
    যদি (non_empty_গুনতি != n_বস্তুসমূহ) {
        asdl_ৰাশিমালা_seq *resized_spec =
            Py_asdl_ৰাশিমালা_seq_new(non_empty_গুনতি, p->arena);
        যদি (resized_spec == রিক্ত) {
            ঘূৰাই রিক্ত;
        }
        Py_ssize_t j = 0;
        বাবে (Py_ssize_t i = 0; i < n_বস্তুসমূহ; i++) {
            ৰাশিমালা_ty item = asdl_seq_GET(spec, i);
            যদি (item->kind == Constant_kind &&
                PyUnicode_CheckExact(item->v.Constant.মান) &&
                PyUnicode_GET_LENGTH(item->v.Constant.মান) == 0) {
                অনুৱর্ত;
            }
            asdl_seq_SET(resized_spec, j++, item);
        }
        প্রতিজ্ঞা(j == non_empty_গুনতি);
        spec = resized_spec;
    }
    ৰাশিমালা_ty res;
    Py_ssize_t n = asdl_seq_LEN(spec);
    যদি (n == 0 || (n == 1 && asdl_seq_GET(spec, 0)->kind == Constant_kind)) {
        res = _PyAST_JoinedStr(spec, lineno, col_offset, end_lineno,
                                    end_col_offset, p->arena);
    } অন্যথা {
        res = _PyPegen_concatenate_strings(p, spec,
                             lineno, col_offset, end_lineno,
                             end_col_offset, arena);
    }
    যদি (!res) {
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই result_token_with_metadata(p, res, colon->metadata);
}

ধ্রুৱ ৱর্ণ *
Py_Pegen_get_ৰাশিমালা_name(ৰাশিমালা_ty e)
{
    প্রতিজ্ঞা(e != রিক্ত);
    ৱিকল্প (e->kind) {
        পক্ষ Attribute_kind:
            ঘূৰাই "attribute";
        পক্ষ Subscript_kind:
            ঘূৰাই "subscript";
        পক্ষ Starred_kind:
            ঘূৰাই "starred";
        পক্ষ Name_kind:
            ঘূৰাই "name";
        পক্ষ List_kind:
            ঘূৰাই "list";
        পক্ষ Tuple_kind:
            ঘূৰাই "tuple";
        পক্ষ Lambda_kind:
            ঘূৰাই "lambda";
        পক্ষ Call_kind:
            ঘূৰাই "function call";
        পক্ষ BoolOp_kind:
        পক্ষ BinOp_kind:
        পক্ষ UnaryOp_kind:
            ঘূৰাই "expression";
        পক্ষ GeneratorExp_kind:
            ঘূৰাই "generator expression";
        পক্ষ Yield_kind:
        পক্ষ YieldFrom_kind:
            ঘূৰাই "yield expression";
        পক্ষ Await_kind:
            ঘূৰাই "await expression";
        পক্ষ ListComp_kind:
            ঘূৰাই "list comprehension";
        পক্ষ SetComp_kind:
            ঘূৰাই "set comprehension";
        পক্ষ DictComp_kind:
            ঘূৰাই "dict comprehension";
        পক্ষ Dict_kind:
            ঘূৰাই "dict literal";
        পক্ষ Set_kind:
            ঘূৰাই "set display";
        পক্ষ JoinedStr_kind:
        পক্ষ Formatted_মান_kind:
            ঘূৰাই "f-string expression";
        পক্ষ TemplateStr_kind:
        পক্ষ Interpolation_kind:
            ঘূৰাই "t-string expression";
        পক্ষ Constant_kind: {
            PyObject *মান = e->v.Constant.মান;
            যদি (মান == Py_None) {
                ঘূৰাই "None";
            }
            যদি (মান == Py_False) {
                ঘূৰাই "False";
            }
            যদি (মান == Py_True) {
                ঘূৰাই "True";
            }
            যদি (মান == Py_Ellipsis) {
                ঘূৰাই "ellipsis";
            }
            ঘূৰাই "literal";
        }
        পক্ষ Compare_kind:
            ঘূৰাই "comparison";
        পক্ষ IfExp_kind:
            ঘূৰাই "conditional expression";
        পক্ষ Named_ৰাশিমালা_kind:
            ঘূৰাই "named expression";
        স্ৱভাৱ:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->kind, e->lineno);
            ঘূৰাই রিক্ত;
    }
}

ৰাশিমালা_ty
_PyPegen_get_last_comprehension_item(comprehension_ty comprehension) {
    যদি (comprehension->ifs == রিক্ত || asdl_seq_LEN(comprehension->ifs) == 0) {
        ঘূৰাই comprehension->iter;
    }
    ঘূৰাই PyPegen_last_item(comprehension->ifs, ৰাশিমালা_ty);
}

ৰাশিমালা_ty _PyPegen_collect_call_seqs(Parser *p, asdl_ৰাশিমালা_seq *a, asdl_seq *b,
                     পূর্ণাঙ্ক lineno, পূর্ণাঙ্ক col_offset, পূর্ণাঙ্ক end_lineno,
                     পূর্ণাঙ্ক end_col_offset, PyArena *arena) {
    Py_ssize_t args_len = asdl_seq_LEN(a);
    Py_ssize_t total_len = args_len;

    যদি (b == রিক্ত) {
        ঘূৰাই _PyAST_Call(_PyPegen_dummy_name(p), a, রিক্ত, lineno, col_offset,
                        end_lineno, end_col_offset, arena);

    }

    asdl_ৰাশিমালা_seq *starreds = _PyPegen_seq_extract_starred_exprs(p, b);
    যদি (!starreds && PyErr_Occurred()) {
        ঘূৰাই রিক্ত;
    }

    asdl_keyword_seq *keywords = _PyPegen_seq_delete_starred_exprs(p, b);
    যদি (!keywords && PyErr_Occurred()) {
        ঘূৰাই রিক্ত;
    }

    যদি (starreds) {
        total_len += asdl_seq_LEN(starreds);
    }

    asdl_ৰাশিমালা_seq *args = Py_asdl_ৰাশিমালা_seq_new(total_len, arena);
    যদি (args == রিক্ত) {
        ঘূৰাই রিক্ত;
    }

    Py_ssize_t i = 0;
    বাবে (i = 0; i < args_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(a, i));
    }
    বাবে (; i < total_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(starreds, i - args_len));
    }

    ঘূৰাই _PyAST_Call(_PyPegen_dummy_name(p), args, keywords, lineno,
                       col_offset, end_lineno, end_col_offset, arena);
}

// AST Error reporting helpers

ৰাশিমালা_ty
_PyPegen_get_invalid_target(ৰাশিমালা_ty e, TARGETS_TYPE targets_type)
{
    যদি (e == রিক্ত) {
        ঘূৰাই রিক্ত;
    }

    যদি (targets_type == SINGLE_TARGETS ||
        targets_type == ATTRIBUTE_OR_SUBSCRIPT_TARGETS) {
        যদি (targets_type == SINGLE_TARGETS && e->kind == Name_kind) {
            ঘূৰাই রিক্ত;
        }
        ঘূৰাই e->kind == Attribute_kind || e->kind == Subscript_kind ? রিক্ত : e;
    }

#define VISIT_CONTAINER(CONTAINER, TYPE) কৰা { \
        Py_ssize_t len = asdl_seq_LEN((CONTAINER)->v.TYPE.elts);\
        বাবে (Py_ssize_t i = 0; i < len; i++) {\
            ৰাশিমালা_ty other = asdl_seq_GET((CONTAINER)->v.TYPE.elts, i);\
            ৰাশিমালা_ty child = _PyPegen_get_invalid_target(other, targets_type);\
            যদি (child != রিক্ত) {\
                ঘূৰাই child;\
            }\
        }\
    } যেতিয়ালৈ (0)

    // We only need to visit List and Tuple nodes recursively as those
    // are the only ones that can contain valid names in targets when
    // they are parsed as expressions. Any other kind of expression
    // that is a container (like Sets or Dicts) is directly invalid and
    // we don't need to visit it recursively.

    ৱিকল্প (e->kind) {
        পক্ষ List_kind:
            VISIT_CONTAINER(e, List);
            ঘূৰাই রিক্ত;
        পক্ষ Tuple_kind:
            VISIT_CONTAINER(e, Tuple);
            ঘূৰাই রিক্ত;
        পক্ষ Starred_kind:
            যদি (targets_type == DEL_TARGETS) {
                ঘূৰাই e;
            }
            ঘূৰাই _PyPegen_get_invalid_target(e->v.Starred.মান, targets_type);
        পক্ষ Compare_kind:
            // This is needed, because the `a in b` in `for a in b` gets parsed
            // as a comparison, and so we need to search the left side of the comparison
            // for invalid targets.
            যদি (targets_type == FOR_TARGETS) {
                cmpop_ty cmpop = (cmpop_ty) asdl_seq_GET(e->v.Compare.ops, 0);
                যদি (cmpop == In) {
                    ঘূৰাই _PyPegen_get_invalid_target(e->v.Compare.বাম, targets_type);
                }
                ঘূৰাই রিক্ত;
            }
            ঘূৰাই e;
        পক্ষ Name_kind:
        পক্ষ Subscript_kind:
        পক্ষ Attribute_kind:
            ঘূৰাই রিক্ত;
        স্ৱভাৱ:
            ঘূৰাই e;
    }
}

শূন্য *_PyPegen_arguments_parsing_error(Parser *p, ৰাশিমালা_ty e) {
    পূর্ণাঙ্ক kwarg_unpacking = 0;
    বাবে (Py_ssize_t i = 0, l = asdl_seq_LEN(e->v.Call.keywords); i < l; i++) {
        keyword_ty keyword = asdl_seq_GET(e->v.Call.keywords, i);
        যদি (!keyword->arg) {
            kwarg_unpacking = 1;
        }
    }

    ধ্রুৱ ৱর্ণ *বাৰ্তা = রিক্ত;
    যদি (kwarg_unpacking) {
        বাৰ্তা = "positional argument follows keyword argument unpacking";
    } অন্যথা {
        বাৰ্তা = "positional argument follows keyword argument";
    }

    ঘূৰাই RAISE_SYNTAX_ERROR(বাৰ্তা);
}

শূন্য *
_PyPegen_nonparen_genexp_in_call(Parser *p, ৰাশিমালা_ty args, asdl_comprehension_seq *comprehensions)
{
    /* The rule that calls this function is 'args for_if_clauses'.
       For the input f(L, x for x in y), L and x are in args and
       the for is parsed as a for_if_clause. We have to check if
       len <= 1, so that input like dict((a, b) for a, b in x)
       gets successfully parsed and then we pass the last
       argument (x in the above example) as the location of the
       error */
    Py_ssize_t len = asdl_seq_LEN(args->v.Call.args);
    যদি (len <= 1) {
        ঘূৰাই রিক্ত;
    }

    comprehension_ty last_comprehension = PyPegen_last_item(comprehensions, comprehension_ty);

    ঘূৰাই RAISE_SYNTAX_ERROR_KNOWN_RANGE(
        (ৰাশিমালা_ty) asdl_seq_GET(args->v.Call.args, len - 1),
        _PyPegen_get_last_comprehension_item(last_comprehension),
        "Generator expression must be parenthesized"
    );
}

// Fstring stuff

স্থির ৰাশিমালা_ty
_PyPegen_decode_fstring_part(Parser* p, পূর্ণাঙ্ক is_raw, ৰাশিমালা_ty constant, Token* token) {
    প্রতিজ্ঞা(PyUnicode_CheckExact(constant->v.Constant.মান));

    ধ্রুৱ ৱর্ণ* bstr = PyUnicode_AsUTF8(constant->v.Constant.মান);
    যদি (bstr == রিক্ত) {
        ঘূৰাই রিক্ত;
    }

    size_t len;
    যদি (strcmp(bstr, "{{") == 0 || strcmp(bstr, "}}") == 0) {
        len = 1;
    } অন্যথা {
        len = strlen(bstr);
    }

    is_raw = is_raw || strchr(bstr, '\\') == রিক্ত;
    PyObject *str = _PyPegen_decode_string(p, is_raw, bstr, len, token);
    যদি (str == রিক্ত) {
        _Pypegen_raise_decode_error(p);
        ঘূৰাই রিক্ত;
    }
    যদি (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই _PyAST_Constant(str, রিক্ত, constant->lineno, constant->col_offset,
                           constant->end_lineno, constant->end_col_offset,
                           p->arena);
}

স্থির asdl_ৰাশিমালা_seq *
_get_resized_exprs(Parser *p, Token *a, asdl_ৰাশিমালা_seq *raw_expressions,
                   Token *b, পূর্ণাঙ্ক is_tstring)
{
    Py_ssize_t n_বস্তুসমূহ = asdl_seq_LEN(raw_expressions);
    Py_ssize_t total_বস্তুসমূহ = n_বস্তুসমূহ;
    বাবে (Py_ssize_t i = 0; i < n_বস্তুসমূহ; i++) {
        ৰাশিমালা_ty item = asdl_seq_GET(raw_expressions, i);
        যদি (item->kind == JoinedStr_kind) {
            total_বস্তুসমূহ += asdl_seq_LEN(item->v.JoinedStr.মানসমূহ) - 1;
        }
    }

    ধ্রুৱ ৱর্ণ* quote_str = PyBytes_AsString(a->bytes);
    যদি (quote_str == রিক্ত) {
        ঘূৰাই রিক্ত;
    }
    পূর্ণাঙ্ক is_raw = strpbrk(quote_str, "rR") != রিক্ত;

    asdl_ৰাশিমালা_seq *seq = Py_asdl_ৰাশিমালা_seq_new(total_বস্তুসমূহ, p->arena);
    যদি (seq == রিক্ত) {
        ঘূৰাই রিক্ত;
    }

    Py_ssize_t index = 0;
    বাবে (Py_ssize_t i = 0; i < n_বস্তুসমূহ; i++) {
        ৰাশিমালা_ty item = asdl_seq_GET(raw_expressions, i);

        /* Debug expressions arrive as JoinedStr(text, value); flatten them
           into the surrounding string. */
        যদি (item->kind == JoinedStr_kind) {
            asdl_ৰাশিমালা_seq *মানসমূহ = item->v.JoinedStr.মানসমূহ;
            যদি (asdl_seq_LEN(মানসমূহ) != 2) {
                PyErr_Format(PyExc_SystemError,
                             is_tstring
                             ? "unexpected TemplateStr node without debug data in t-string at line %d"
                             : "unexpected JoinedStr node without debug data in f-string at line %d",
                             item->lineno);
                ঘূৰাই রিক্ত;
            }

            ৰাশিমালা_ty first = asdl_seq_GET(মানসমূহ, 0);
            প্রতিজ্ঞা(first->kind == Constant_kind);
            asdl_seq_SET(seq, index++, first);

            ৰাশিমালা_ty second = asdl_seq_GET(মানসমূহ, 1);
            প্রতিজ্ঞা((is_tstring &&
                    second->kind == Interpolation_kind) ||
                   second->kind == Formatted_মান_kind);
            asdl_seq_SET(seq, index++, second);

            অনুৱর্ত;
        }

        যদি (item->kind == Constant_kind) {
            item = _PyPegen_decode_fstring_part(p, is_raw, item, b);
            যদি (item == রিক্ত) {
                ঘূৰাই রিক্ত;
            }

            /* Tokenizer emits string parts even when the underlying string
            might become an empty value (e.g. FSTRING_MIDDLE with the value \\n)
            so we need to check for them and simplify it here. */
            যদি (PyUnicode_CheckExact(item->v.Constant.মান)
                && PyUnicode_GET_LENGTH(item->v.Constant.মান) == 0) {
                অনুৱর্ত;
            }
        }
        asdl_seq_SET(seq, index++, item);
    }

    asdl_ৰাশিমালা_seq *resized_exprs;
    যদি (index != total_বস্তুসমূহ) {
        resized_exprs = Py_asdl_ৰাশিমালা_seq_new(index, p->arena);
        যদি (resized_exprs == রিক্ত) {
            ঘূৰাই রিক্ত;
        }
        বাবে (Py_ssize_t i = 0; i < index; i++) {
            asdl_seq_SET(resized_exprs, i, asdl_seq_GET(seq, i));
        }
    }
    অন্যথা {
        resized_exprs = seq;
    }
    ঘূৰাই resized_exprs;
}

ৰাশিমালা_ty
_PyPegen_template_str(Parser *p, Token *a, asdl_ৰাশিমালা_seq *raw_expressions, Token *b) {

    asdl_ৰাশিমালা_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 1);
    যদি (resized_exprs == রিক্ত) {
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই _PyAST_TemplateStr(resized_exprs, a->lineno, a->col_offset,
                              b->end_lineno, b->end_col_offset,
                              p->arena);
}

ৰাশিমালা_ty
_PyPegen_joined_str(Parser *p, Token* a, asdl_ৰাশিমালা_seq* raw_expressions, Token*b) {

    asdl_ৰাশিমালা_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 0);
    যদি (resized_exprs == রিক্ত) {
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই _PyAST_JoinedStr(resized_exprs, a->lineno, a->col_offset,
                            b->end_lineno, b->end_col_offset,
                            p->arena);
}

ৰাশিমালা_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok) {
    যদি (tok->bytes == রিক্ত) {
        ঘূৰাই Py_Pegen_raise_incomplete_প্ৰবেশ(p);
    }
    Py_ssize_t bsize = PyBytes_GET_SIZE(tok->bytes);
    ধ্রুৱ ৱর্ণ *bstr = PyBytes_AS_STRING(tok->bytes);

    পূর্ণাঙ্ক is_raw = tok->is_raw;

    PyObject* str = _PyPegen_decode_string(p, is_raw, bstr, bsize, tok);
    যদি (str == রিক্ত) {
        ঘূৰাই রিক্ত;
    }
    যদি (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই _PyAST_Constant(str, রিক্ত, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

ৰাশিমালা_ty _PyPegen_constant_from_token(Parser* p, Token* tok) {
    যদি (tok->bytes == রিক্ত) {
        ঘূৰাই Py_Pegen_raise_incomplete_প্ৰবেশ(p);
    }
    ধ্রুৱ ৱর্ণ *bstr = PyBytes_AS_STRING(tok->bytes);
    PyObject* str = PyUnicode_FromString(bstr);
    যদি (str == রিক্ত) {
        ঘূৰাই রিক্ত;
    }
    যদি (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই _PyAST_Constant(str, রিক্ত, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

ৰাশিমালা_ty _PyPegen_constant_from_string(Parser* p, Token* tok) {
    যদি (tok->bytes == রিক্ত) {
        ঘূৰাই Py_Pegen_raise_incomplete_প্ৰবেশ(p);
    }
    ধ্রুৱ ৱর্ণ *the_str = PyBytes_AS_STRING(tok->bytes);
    PyObject *s = _PyPegen_parse_string(p, tok);
    যদি (s == রিক্ত) {
        _Pypegen_raise_decode_error(p);
        ঘূৰাই রিক্ত;
    }
    যদি (_PyArena_AddPyObject(p->arena, s) < 0) {
        Py_DECREF(s);
        ঘূৰাই রিক্ত;
    }
    PyObject *kind = রিক্ত;
    যদি (the_str && the_str[0] == 'u') {
        kind = _PyPegen_new_identifier(p, "u");
        যদি (kind == রিক্ত) {
            ঘূৰাই রিক্ত;
        }
    }
    ঘূৰাই _PyAST_Constant(s, kind, tok->lineno, tok->col_offset, tok->end_lineno, tok->end_col_offset, p->arena);
}

স্থির পূর্ণাঙ্ক
_get_interpolation_conversion(Parser *p, Token *debug, ResultTokenWithMetadata *conversion,
                              ResultTokenWithMetadata *format)
{
    যদি (conversion != রিক্ত) {
        ৰাশিমালা_ty conversion_ৰাশিমালা = (ৰাশিমালা_ty) conversion->result;
        প্রতিজ্ঞা(conversion_ৰাশিমালা->kind == Name_kind);
        Py_UCS4 first = PyUnicode_READ_CHAR(conversion_ৰাশিমালা->v.Name.id, 0);
        ঘূৰাই Py_SAFE_DOWNCAST(first, Py_UCS4, পূর্ণাঙ্ক);
    }
    অন্যথা যদি (debug && !format) {
        /* If no conversion is specified, use !r for debug expressions */
        ঘূৰাই (পূর্ণাঙ্ক)'r';
    }
    ঘূৰাই -1;
}

স্থির PyObject *
strip_interpolation_debug_ৰাশিমালা(PyObject *exprstr)
{
    Py_ssize_t len = PyUnicode_GET_LENGTH(exprstr);

    /* Discard whitespace and explicit line continuations after the debug "="
       but preserve whitespace before it. */
    যেতিয়ালৈ (len > 0) {
        পূর্ণাঙ্ক has_newline = 0;
        যেতিয়ালৈ (len > 0) {
            Py_UCS4 c = PyUnicode_READ_CHAR(exprstr, len - 1);
            যদি (!_PyUnicode_IsWhitespace(c)) {
                ভঙ্গ;
            }
            যদি (c == '\r' || c == '\n') {
                has_newline = 1;
            }
            len--;
        }
        যদি (!has_newline || len == 0 ||
            PyUnicode_READ_CHAR(exprstr, len - 1) != '\\')
        {
            ভঙ্গ;
        }
        len--;
    }

    /* Preserve unexpected metadata instead of dropping source text. */
    যদি (len == 0 || PyUnicode_READ_CHAR(exprstr, len - 1) != '=') {
        ঘূৰাই Py_NewRef(exprstr);
    }

    ঘূৰাই PyUnicode_Substring(exprstr, 0, len - 1);
}

ৰাশিমালা_ty _PyPegen_interpolation(Parser *p, ৰাশিমালা_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, পূর্ণাঙ্ক lineno, পূর্ণাঙ্ক col_offset,
                                 পূর্ণাঙ্ক end_lineno, পূর্ণাঙ্ক end_col_offset, PyArena *arena) {

    পূর্ণাঙ্ক conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    /* Find the non whitespace token after the "=" */
    পূর্ণাঙ্ক debug_end_সাৰি, debug_end_offset;
    PyObject *debug_metadata;
    constant exprstr;

    যদি (conversion) {
        debug_end_সাৰি = ((ৰাশিমালা_ty) conversion->result)->lineno;
        debug_end_offset = ((ৰাশিমালা_ty) conversion->result)->col_offset;
        debug_metadata = exprstr = conversion->metadata;
    }
    অন্যথা যদি (format) {
        debug_end_সাৰি = ((ৰাশিমালা_ty) format->result)->lineno;
        debug_end_offset = ((ৰাশিমালা_ty) format->result)->col_offset + 1;
        debug_metadata = exprstr = format->metadata;
    }
    অন্যথা {
        debug_end_সাৰি = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = exprstr = closing_brace->metadata;
    }

    প্রতিজ্ঞা(exprstr != রিক্ত);
    PyObject *final_exprstr = debug
        ? strip_interpolation_debug_ৰাশিমালা(exprstr)
        : Py_NewRef(exprstr);
    যদি (!final_exprstr || _PyArena_AddPyObject(arena, final_exprstr) < 0) {
        Py_XDECREF(final_exprstr);
        ঘূৰাই রিক্ত;
    }

    ৰাশিমালা_ty interpolation = _PyAST_Interpolation(
        expression, final_exprstr, conversion_val, format ? (ৰাশিমালা_ty) format->result : রিক্ত,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    যদি (!interpolation || !debug) {
        ঘূৰাই interpolation;
    }

    ৰাশিমালা_ty debug_text = _PyAST_Constant(debug_metadata, রিক্ত, lineno, col_offset + 1, debug_end_সাৰি,
                                            debug_end_offset - 1, p->arena);
    যদি (!debug_text) {
        ঘূৰাই রিক্ত;
    }

    asdl_ৰাশিমালা_seq *মানসমূহ = Py_asdl_ৰাশিমালা_seq_new(2, arena);
    যদি (!মানসমূহ) {
        ঘূৰাই রিক্ত;
    }
    asdl_seq_SET(মানসমূহ, 0, debug_text);
    asdl_seq_SET(মানসমূহ, 1, interpolation);
    ঘূৰাই _PyAST_JoinedStr(মানসমূহ, lineno, col_offset, debug_end_সাৰি, debug_end_offset, p->arena);
}

ৰাশিমালা_ty Py_Pegen_formatted_মান(Parser *p, ৰাশিমালা_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, পূর্ণাঙ্ক lineno, পূর্ণাঙ্ক col_offset,
                                 পূর্ণাঙ্ক end_lineno, পূর্ণাঙ্ক end_col_offset, PyArena *arena) {
    পূর্ণাঙ্ক conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    ৰাশিমালা_ty formatted_মান = Py_AST_Formatted_মান(
        expression, conversion_val, format ? (ৰাশিমালা_ty) format->result : রিক্ত,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    যদি (!formatted_মান || !debug) {
        ঘূৰাই formatted_মান;
    }

    /* Find the non whitespace token after the "=" */
    পূর্ণাঙ্ক debug_end_সাৰি, debug_end_offset;
    PyObject *debug_metadata;

    যদি (conversion) {
        debug_end_সাৰি = ((ৰাশিমালা_ty) conversion->result)->lineno;
        debug_end_offset = ((ৰাশিমালা_ty) conversion->result)->col_offset;
        debug_metadata = conversion->metadata;
    }
    অন্যথা যদি (format) {
        debug_end_সাৰি = ((ৰাশিমালা_ty) format->result)->lineno;
        debug_end_offset = ((ৰাশিমালা_ty) format->result)->col_offset + 1;
        debug_metadata = format->metadata;
    }
    অন্যথা {
        debug_end_সাৰি = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = closing_brace->metadata;
    }
    ৰাশিমালা_ty debug_text = _PyAST_Constant(debug_metadata, রিক্ত, lineno, col_offset + 1, debug_end_সাৰি,
                                            debug_end_offset - 1, p->arena);
    যদি (!debug_text) {
        ঘূৰাই রিক্ত;
    }

    asdl_ৰাশিমালা_seq *মানসমূহ = Py_asdl_ৰাশিমালা_seq_new(2, arena);
    যদি (!মানসমূহ) {
        ঘূৰাই রিক্ত;
    }
    asdl_seq_SET(মানসমূহ, 0, debug_text);
    asdl_seq_SET(মানসমূহ, 1, formatted_মান);
    ঘূৰাই _PyAST_JoinedStr(মানসমূহ, lineno, col_offset, debug_end_সাৰি, debug_end_offset, p->arena);
}

স্থির ৰাশিমালা_ty
_build_concatenated_bytes(Parser *p, asdl_ৰাশিমালা_seq *strings, পূর্ণাঙ্ক lineno,
                        পূর্ণাঙ্ক col_offset, পূর্ণাঙ্ক end_lineno, পূর্ণাঙ্ক end_col_offset,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    প্রতিজ্ঞা(len > 0);

    /* Bytes literals never get a kind, but just for consistency
        since they are represented as Constant nodes, we'll mirror
        the same behavior as unicode strings for determining the
        kind. */
    PyObject *kind = asdl_seq_GET(strings, 0)->v.Constant.kind;

    Py_ssize_t total = 0;
    বাবে (Py_ssize_t i = 0; i < len; i++) {
        ৰাশিমালা_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.মান;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        যদি (part > PY_SSIZE_T_MAX - total) {
            PyErr_NoMemory();
            ঘূৰাই রিক্ত;
        }
        total += part;
    }

    PyBytesWriter *writer = PyBytesWriter_Create(total);
    যদি (writer == রিক্ত) {
        ঘূৰাই রিক্ত;
    }
    ৱর্ণ *out = PyBytesWriter_GetData(writer);

    বাবে (Py_ssize_t i = 0; i < len; i++) {
        ৰাশিমালা_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.মান;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        যদি (part > 0) {
            memcpy(out, PyBytes_AS_STRING(bytes), part);
            out += part;
        }
    }

    PyObject *res = PyBytesWriter_Finish(writer);
    যদি (res == রিক্ত) {
        ঘূৰাই রিক্ত;
    }
    যদি (_PyArena_AddPyObject(arena, res) < 0) {
        Py_DECREF(res);
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই _PyAST_Constant(res, kind, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

স্থির ৰাশিমালা_ty
_build_concatenated_unicode(Parser *p, asdl_ৰাশিমালা_seq *strings, পূর্ণাঙ্ক lineno,
                        পূর্ণাঙ্ক col_offset, পূর্ণাঙ্ক end_lineno, পূর্ণাঙ্ক end_col_offset,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    প্রতিজ্ঞা(len > 1);

    ৰাশিমালা_ty first = asdl_seq_GET(strings, 0);

    /* When a string is getting concatenated, the kind of the string
        is determined by the first string in the concatenation
        sequence.

        u"abc" "def" -> u"abcdef"
        "abc" u"abc" ->  "abcabc" */
    PyObject *kind = first->v.Constant.kind;

    PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
    যদি (writer == রিক্ত) {
        ঘূৰাই রিক্ত;
    }

    বাবে (Py_ssize_t i = 0; i < len; i++) {
        ৰাশিমালা_ty current_elem = asdl_seq_GET(strings, i);
        প্রতিজ্ঞা(current_elem->kind == Constant_kind);

        যদি (PyUnicodeWriter_WriteStr(writer,
                                     current_elem->v.Constant.মান)) {
            PyUnicodeWriter_Discard(writer);
            ঘূৰাই রিক্ত;
        }
    }

    PyObject *final = PyUnicodeWriter_Finish(writer);
    যদি (final == রিক্ত) {
        ঘূৰাই রিক্ত;
    }
    যদি (_PyArena_AddPyObject(p->arena, final) < 0) {
        Py_DECREF(final);
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই _PyAST_Constant(final, kind, lineno, col_offset,
                           end_lineno, end_col_offset, arena);
}

স্থির asdl_ৰাশিমালা_seq *
_build_concatenated_str(Parser *p, asdl_ৰাশিমালা_seq *strings,
                               পূর্ণাঙ্ক lineno, পূর্ণাঙ্ক col_offset, পূর্ণাঙ্ক end_lineno,
                               পূর্ণাঙ্ক end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    প্রতিজ্ঞা(len > 0);

    Py_ssize_t n_flattened_elements = 0;
    বাবে (Py_ssize_t i = 0; i < len; i++) {
        ৰাশিমালা_ty elem = asdl_seq_GET(strings, i);
        ৱিকল্প(elem->kind) {
            পক্ষ JoinedStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.JoinedStr.মানসমূহ);
                ভঙ্গ;
            পক্ষ TemplateStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.TemplateStr.মানসমূহ);
                ভঙ্গ;
            স্ৱভাৱ:
                n_flattened_elements++;
                ভঙ্গ;
        }
    }


    asdl_ৰাশিমালা_seq* flattened = Py_asdl_ৰাশিমালা_seq_new(n_flattened_elements, p->arena);
    যদি (flattened == রিক্ত) {
        ঘূৰাই রিক্ত;
    }

    /* build flattened list */
    Py_ssize_t current_pos = 0;
    বাবে (Py_ssize_t i = 0; i < len; i++) {
        ৰাশিমালা_ty elem = asdl_seq_GET(strings, i);
        ৱিকল্প(elem->kind) {
            পক্ষ JoinedStr_kind:
                বাবে (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.JoinedStr.মানসমূহ); j++) {
                    ৰাশিমালা_ty subvalue = asdl_seq_GET(elem->v.JoinedStr.মানসমূহ, j);
                    যদি (subvalue == রিক্ত) {
                        ঘূৰাই রিক্ত;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                ভঙ্গ;
            পক্ষ TemplateStr_kind:
                বাবে (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.TemplateStr.মানসমূহ); j++) {
                    ৰাশিমালা_ty subvalue = asdl_seq_GET(elem->v.TemplateStr.মানসমূহ, j);
                    যদি (subvalue == রিক্ত) {
                        ঘূৰাই রিক্ত;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                ভঙ্গ;
            স্ৱভাৱ:
                asdl_seq_SET(flattened, current_pos++, elem);
                ভঙ্গ;
        }
    }

    /* calculate folded element count */
    Py_ssize_t n_elements = 0;
    পূর্ণাঙ্ক prev_is_constant = 0;
    বাবে (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        ৰাশিমালা_ty elem = asdl_seq_GET(flattened, i);

        /* The concatenation of a FormattedValue and an empty Constant should
           lead to the FormattedValue itself. Thus, we will not take any empty
           constants into account, just as in `_PyPegen_joined_str` */
        যদি (elem->kind == Constant_kind &&
            PyUnicode_CheckExact(elem->v.Constant.মান) &&
            PyUnicode_GET_LENGTH(elem->v.Constant.মান) == 0)
            অনুৱর্ত;

        যদি (!prev_is_constant || elem->kind != Constant_kind) {
            n_elements++;
        }
        prev_is_constant = elem->kind == Constant_kind;
    }

    asdl_ৰাশিমালা_seq* মানসমূহ = Py_asdl_ৰাশিমালা_seq_new(n_elements, p->arena);
    যদি (মানসমূহ == রিক্ত) {
        ঘূৰাই রিক্ত;
    }

    /* build folded list */
    current_pos = 0;
    বাবে (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        ৰাশিমালা_ty elem = asdl_seq_GET(flattened, i);

        /* if the current elem and the following are constants,
           fold them and all consequent constants */
        যদি (elem->kind == Constant_kind) {
            যদি (i + 1 < n_flattened_elements &&
                asdl_seq_GET(flattened, i + 1)->kind == Constant_kind) {
                ৰাশিমালা_ty first_elem = elem;

                /* When a string is getting concatenated, the kind of the string
                   is determined by the first string in the concatenation
                   sequence.

                   u"abc" "def" -> u"abcdef"
                   "abc" u"abc" ->  "abcabc" */
                PyObject *kind = elem->v.Constant.kind;

                PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
                যদি (writer == রিক্ত) {
                    ঘূৰাই রিক্ত;
                }
                ৰাশিমালা_ty last_elem = elem;
                Py_ssize_t j;
                বাবে (j = i; j < n_flattened_elements; j++) {
                    ৰাশিমালা_ty current_elem = asdl_seq_GET(flattened, j);
                    যদি (current_elem->kind == Constant_kind) {
                        যদি (PyUnicodeWriter_WriteStr(writer,
                                                     current_elem->v.Constant.মান)) {
                            PyUnicodeWriter_Discard(writer);
                            ঘূৰাই রিক্ত;
                        }
                        last_elem = current_elem;
                    } অন্যথা {
                        ভঙ্গ;
                    }
                }
                i = j - 1;

                PyObject *concat_str = PyUnicodeWriter_Finish(writer);
                যদি (concat_str == রিক্ত) {
                    ঘূৰাই রিক্ত;
                }
                যদি (_PyArena_AddPyObject(p->arena, concat_str) < 0) {
                    Py_DECREF(concat_str);
                    ঘূৰাই রিক্ত;
                }
                elem = _PyAST_Constant(concat_str, kind, first_elem->lineno,
                                       first_elem->col_offset,
                                       last_elem->end_lineno,
                                       last_elem->end_col_offset, p->arena);
                যদি (elem == রিক্ত) {
                    ঘূৰাই রিক্ত;
                }
            }

            /* Drop all empty contanst strings */
            যদি (PyUnicode_CheckExact(elem->v.Constant.মান) &&
                PyUnicode_GET_LENGTH(elem->v.Constant.মান) == 0) {
                অনুৱর্ত;
            }
        }

        asdl_seq_SET(মানসমূহ, current_pos++, elem);
    }

    প্রতিজ্ঞা(current_pos == n_elements);
    ঘূৰাই মানসমূহ;
}

স্থির ৰাশিমালা_ty
_build_concatenated_joined_str(Parser *p, asdl_ৰাশিমালা_seq *strings,
                               পূর্ণাঙ্ক lineno, পূর্ণাঙ্ক col_offset, পূর্ণাঙ্ক end_lineno,
                               পূর্ণাঙ্ক end_col_offset, PyArena *arena)
{
    asdl_ৰাশিমালা_seq *মানসমূহ = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    যদি (!মানসমূহ) {
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই _PyAST_JoinedStr(মানসমূহ, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

ৰাশিমালা_ty
_PyPegen_concatenate_tstrings(Parser *p, asdl_ৰাশিমালা_seq *strings,
                               পূর্ণাঙ্ক lineno, পূর্ণাঙ্ক col_offset, পূর্ণাঙ্ক end_lineno,
                               পূর্ণাঙ্ক end_col_offset, PyArena *arena)
{
    asdl_ৰাশিমালা_seq *মানসমূহ = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    যদি (!মানসমূহ) {
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই _PyAST_TemplateStr(মানসমূহ, lineno, col_offset, end_lineno,
        end_col_offset, arena);
}

ৰাশিমালা_ty
_PyPegen_concatenate_strings(Parser *p, asdl_ৰাশিমালা_seq *strings,
                             পূর্ণাঙ্ক lineno, পূর্ণাঙ্ক col_offset, পূর্ণাঙ্ক end_lineno,
                             পূর্ণাঙ্ক end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    প্রতিজ্ঞা(len > 0);

    পূর্ণাঙ্ক f_string_found = 0;
    পূর্ণাঙ্ক unicode_string_found = 0;
    পূর্ণাঙ্ক bytes_found = 0;

    Py_ssize_t i = 0;
    বাবে (i = 0; i < len; i++) {
        ৰাশিমালা_ty elem = asdl_seq_GET(strings, i);
        ৱিকল্প(elem->kind) {
            পক্ষ Constant_kind:
                যদি (PyBytes_CheckExact(elem->v.Constant.মান)) {
                    bytes_found = 1;
                } অন্যথা {
                    unicode_string_found = 1;
                }
                ভঙ্গ;
            পক্ষ JoinedStr_kind:
                f_string_found = 1;
                ভঙ্গ;
            পক্ষ TemplateStr_kind:
                // python.gram handles this; we should never get here
                প্রতিজ্ঞা(0);
                ভঙ্গ;
            স্ৱভাৱ:
                f_string_found = 1;
                ভঙ্গ;
        }
    }

    // Cannot mix unicode and bytes
    যদি ((unicode_string_found || f_string_found) && bytes_found) {
        RAISE_SYNTAX_ERROR("cannot mix bytes and nonbytes literals");
        ঘূৰাই রিক্ত;
    }

    // If it's only bytes or only unicode string, do a simple concat
    যদি (!f_string_found) {
        যদি (len == 1) {
            ঘূৰাই asdl_seq_GET(strings, 0);
        }
        অন্যথা যদি (bytes_found) {
            ঘূৰাই _build_concatenated_bytes(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
        অন্যথা {
            ঘূৰাই _build_concatenated_unicode(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
    }

    ঘূৰাই _build_concatenated_joined_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
}

স্থির পূর্ণাঙ্ক
_warn_relative_import_of_lazy(Parser *p, asdl_seq *dots, ৰাশিমালা_ty module)
{
    // Warn about `from . lazy import x`: the whitespace between the dots and
    // the module name is insignificant, so this is parsed exactly like
    // `from .lazy import x` (an import of the relative module "lazy"), but it
    // is most likely a transposition of `lazy from . import x` (PEP 810).
    যদি (p->call_invalid_rules) {
        ঘূৰাই 0;
    }

    // Only fire if there is whitespace between the last dot and the name,
    // i.e. not for the common `from .lazy import x` spelling.
    Token *last_dot = asdl_seq_GET_UNTYPED(dots, asdl_seq_LEN(dots) - 1);
    যদি (
        last_dot->end_lineno == module->lineno
        && last_dot->end_col_offset == module->col_offset
    ) {
        ঘূৰাই 0;
    }

    পূর্ণাঙ্ক গুনতি = Py_Pegen_seq_গুনতি_dots(dots);
    ৱর্ণ *buf = PyMem_RawMalloc(গুনতি + 1);
    যদি (buf == রিক্ত) {
        PyErr_NoMemory();
        ঘূৰাই -1;
    }
    memset(buf, '.', গুনতি);
    buf[গুনতি] = '\0';

    PyObject *বাৰ্তা = PyUnicode_FromFormat(
        "'from %s lazy import' is the same as 'from %slazy import'; "
        "did you mean 'lazy from %s import'?",
        buf, buf, buf);
    PyMem_RawFree(buf);
    যদি (বাৰ্তা == রিক্ত) {
        ঘূৰাই -1;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    পূর্ণাঙ্ক res = _PyErr_EmitSyntaxWarning(বাৰ্তা,
                                       info.filename,
                                       module->lineno,
                                       module->col_offset + 1,
                                       module->end_lineno,
                                       module->end_col_offset + 1,
                                       info.module);
    Py_DECREF(বাৰ্তা);
    ঘূৰাই res;
}

stmt_ty
_PyPegen_checked_from_import(Parser *p, asdl_seq *dots, ৰাশিমালা_ty module_name,
                             asdl_alias_seq *names, ৰাশিমালা_ty lazy_token, পূর্ণাঙ্ক lineno,
                             পূর্ণাঙ্ক col_offset, পূর্ণাঙ্ক end_lineno, পূর্ণাঙ্ক end_col_offset,
                             PyArena *arena)
{
    identifier module = module_name->v.Name.id;
    পূর্ণাঙ্ক level = Py_Pegen_seq_গুনতি_dots(dots);
    যদি (level == 0 && PyUnicode_CompareWithASCIIString(module, "__future__") == 0) {
        যদি (lazy_token) {
            RAISE_SYNTAX_ERROR_KNOWN_LOCATION(lazy_token,
                "lazy from __future__ import is not allowed");
            ঘূৰাই রিক্ত;
        }
        বাবে (Py_ssize_t i = 0; i < asdl_seq_LEN(names); i++) {
            alias_ty alias = asdl_seq_GET(names, i);
            যদি (PyUnicode_CompareWithASCIIString(alias->name, "barry_as_FLUFL") == 0) {
                p->flags |= PyPARSE_BARRY_AS_BDFL;
                p->tok->barry_as_bdfl = 1;
            }
        }
    }
    অন্যথা যদি (
        level > 0
        && lazy_token == রিক্ত
        && PyUnicode_CompareWithASCIIString(module, "lazy") == 0
    ) {
        যদি (_warn_relative_import_of_lazy(p, dots, module_name) < 0) {
            ঘূৰাই রিক্ত;
        }
    }
    ঘূৰাই _PyAST_ImportFrom(module, names, level, lazy_token ? 1 : 0, lineno,
                             col_offset, end_lineno, end_col_offset, arena);
}

asdl_stmt_seq*
_PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts) {
    যদি (!p->call_invalid_rules) {
        ঘূৰাই stmts;
    }
    Py_ssize_t len = asdl_seq_LEN(stmts);
    যদি (len == 0) {
        ঘূৰাই stmts;
    }
    stmt_ty last_stmt = asdl_seq_GET(stmts, len - 1);
    যদি (p->last_stmt_location.lineno > last_stmt->lineno) {
        ঘূৰাই stmts;
    }
    p->last_stmt_location.lineno = last_stmt->lineno;
    p->last_stmt_location.col_offset = last_stmt->col_offset;
    p->last_stmt_location.end_lineno = last_stmt->end_lineno;
    p->last_stmt_location.end_col_offset = last_stmt->end_col_offset;
    ঘূৰাই stmts;
}
