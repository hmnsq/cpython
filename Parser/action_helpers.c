#অন্তর্ভাব <Python.h>
#অন্তর্ভাব "pycore_pystate.h"         // _PyInterpreterState_GET()
#অন্তর্ভাব "pycore_runtime.h"         // _PyRuntime
#অন্তর্ভাব "pycore_unicodeobject.h"   // _PyUnicode_InternImmortal()
#অন্তর্ভাব "pycore_pyerrors.h"        // _PyErr_EmitSyntaxWarning()

#অন্তর্ভাব "pegen.h"
#অন্তর্ভাব "string_parser.h"          // _PyPegen_decode_string()
#অন্তর্ভাব "lexer/state.h"            // tok_state


শূন্য *
_PyPegen_dummy_name(Parser *p, ...)
{
    ফেরত &_PyRuntime.parser.dummy_name;
}

/* Creates a single-element asdl_seq* that contains a */
asdl_seq *
_PyPegen_singleton_seq(Parser *p, শূন্য *a)
{
    প্রতিজ্ঞা(a != ফাঁকা);
    asdl_seq *seq = (asdl_seq*)_Py_asdl_generic_seq_new(1, p->arena);
    যদি (!seq) {
        ফেরত ফাঁকা;
    }
    asdl_seq_SET_UNTYPED(seq, 0, a);
    ফেরত seq;
}

/* Creates a copy of seq and prepends a to it */
asdl_seq *
Py_Pegen_seq_ঢোকাও_in_front(Parser *p, শূন্য *a, asdl_seq *seq)
{
    প্রতিজ্ঞা(a != ফাঁকা);
    যদি (!seq) {
        ফেরত _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    যদি (!new_seq) {
        ফেরত ফাঁকা;
    }

    asdl_seq_SET_UNTYPED(new_seq, 0, a);
    জন্য (Py_ssize_t i = 1, l = asdl_seq_LEN(new_seq); i < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i - 1));
    }
    ফেরত new_seq;
}

/* Creates a copy of seq and appends a to it */
asdl_seq *
_PyPegen_seq_append_to_end(Parser *p, asdl_seq *seq, শূন্য *a)
{
    প্রতিজ্ঞা(a != ফাঁকা);
    যদি (!seq) {
        ফেরত _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    যদি (!new_seq) {
        ফেরত ফাঁকা;
    }

    জন্য (Py_ssize_t i = 0, l = asdl_seq_LEN(new_seq); i + 1 < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i));
    }
    asdl_seq_SET_UNTYPED(new_seq, asdl_seq_LEN(new_seq) - 1, a);
    ফেরত new_seq;
}

স্থির Py_ssize_t
get_flattened_seq_আকার(asdl_seq *seqs)
{
    Py_ssize_t আকার_2 = 0;
    জন্য (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        আকার_2 += asdl_seq_LEN(inner_seq);
    }
    ফেরত আকার_2;
}

/* Flattens an asdl_seq* of asdl_seq*s */
asdl_seq *
_PyPegen_seq_flatten(Parser *p, asdl_seq *seqs)
{
    Py_ssize_t flattened_seq_আকার = get_flattened_seq_আকার(seqs);
    প্রতিজ্ঞা(flattened_seq_আকার > 0);

    asdl_seq *flattened_seq = (asdl_seq*)_Py_asdl_generic_seq_new(flattened_seq_আকার, p->arena);
    যদি (!flattened_seq) {
        ফেরত ফাঁকা;
    }

    পূর্ণসংখ্যা flattened_seq_idx = 0;
    জন্য (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        জন্য (Py_ssize_t j = 0, li = asdl_seq_LEN(inner_seq); j < li; j++) {
            asdl_seq_SET_UNTYPED(flattened_seq, flattened_seq_idx++, asdl_seq_GET_UNTYPED(inner_seq, j));
        }
    }
    প্রতিজ্ঞা(flattened_seq_idx == flattened_seq_আকার);

    ফেরত flattened_seq;
}

শূন্য *
_PyPegen_seq_last_item(asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ফেরত asdl_seq_GET_UNTYPED(seq, len - 1);
}

শূন্য *
_PyPegen_seq_first_item(asdl_seq *seq)
{
    ফেরত asdl_seq_GET_UNTYPED(seq, 0);
}

/* Creates a new name of the form <first_name>.<second_name> */
রাশিমালা_ty
_PyPegen_join_names_with_dot(Parser *p, রাশিমালা_ty first_name, রাশিমালা_ty second_name)
{
    প্রতিজ্ঞা(first_name != ফাঁকা && second_name != ফাঁকা);
    PyObject *uni = PyUnicode_FromFormat("%U.%U",
            first_name->v.Name.id, second_name->v.Name.id);
    যদি (!uni) {
        ফেরত ফাঁকা;
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &uni);
    যদি (_PyArena_AddPyObject(p->arena, uni) < 0) {
        Py_DECREF(uni);
        ফেরত ফাঁকা;
    }

    ফেরত _PyAST_Name(uni, Load, EXTRA_রাশিমালা(first_name, second_name));
}

/* Counts the total number of dots in seq's tokens */
পূর্ণসংখ্যা
Py_Pegen_seq_গুনতি_dots(asdl_seq *seq)
{
    পূর্ণসংখ্যা সংখ্যা_of_dots = 0;
    জন্য (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        Token *current_রাশিমালা = asdl_seq_GET_UNTYPED(seq, i);
        নির্বাচন (current_রাশিমালা->type) {
            ক্ষেত্র ELLIPSIS:
                সংখ্যা_of_dots += 3;
                বিরতি;
            ক্ষেত্র DOT:
                সংখ্যা_of_dots += 1;
                বিরতি;
            পূর্বনির্ধারিত:
                Py_UNREACHABLE();
        }
    }

    ফেরত সংখ্যা_of_dots;
}

/* Creates an alias with '*' as the identifier name */
alias_ty
_PyPegen_alias_for_star(Parser *p, পূর্ণসংখ্যা lineno, পূর্ণসংখ্যা col_offset, পূর্ণসংখ্যা end_lineno,
                        পূর্ণসংখ্যা end_col_offset, PyArena *arena) {
    PyObject *str = PyUnicode_InternFromString("*");
    যদি (!str) {
        ফেরত ফাঁকা;
    }
    যদি (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ফেরত ফাঁকা;
    }
    ফেরত _PyAST_alias(str, ফাঁকা, lineno, col_offset, end_lineno, end_col_offset, arena);
}

/* Creates a new asdl_seq* with the identifiers of all the names in seq */
asdl_identifier_seq *
_PyPegen_map_names_to_ids(Parser *p, asdl_রাশিমালা_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    প্রতিজ্ঞা(len > 0);

    asdl_identifier_seq *new_seq = _Py_asdl_identifier_seq_new(len, p->arena);
    যদি (!new_seq) {
        ফেরত ফাঁকা;
    }
    জন্য (Py_ssize_t i = 0; i < len; i++) {
        রাশিমালা_ty e = asdl_seq_GET(seq, i);
        asdl_seq_SET(new_seq, i, e->v.Name.id);
    }
    ফেরত new_seq;
}

/* Constructs a CmpopExprPair */
Cmpop_রাশিমালা_Pair *
Py_Pegen_cmpop_রাশিমালা_pair(Parser *p, cmpop_ty cmpop, রাশিমালা_ty রাশিমালা)
{
    প্রতিজ্ঞা(রাশিমালা != ফাঁকা);
    Cmpop_রাশিমালা_Pair *a = _PyArena_Malloc(p->arena, আকার(Cmpop_রাশিমালা_Pair));
    যদি (!a) {
        ফেরত ফাঁকা;
    }
    a->cmpop = cmpop;
    a->রাশিমালা = রাশিমালা;
    ফেরত a;
}

asdl_int_seq *
_PyPegen_get_cmpops(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    প্রতিজ্ঞা(len > 0);

    asdl_int_seq *new_seq = _Py_asdl_int_seq_new(len, p->arena);
    যদি (!new_seq) {
        ফেরত ফাঁকা;
    }
    জন্য (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_রাশিমালা_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->cmpop);
    }
    ফেরত new_seq;
}

asdl_রাশিমালা_seq *
_PyPegen_get_exprs(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    প্রতিজ্ঞা(len > 0);

    asdl_রাশিমালা_seq *new_seq = Py_asdl_রাশিমালা_seq_new(len, p->arena);
    যদি (!new_seq) {
        ফেরত ফাঁকা;
    }
    জন্য (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_রাশিমালা_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->রাশিমালা);
    }
    ফেরত new_seq;
}

/* Creates an asdl_seq* where all the elements have been changed to have ctx as context */
স্থির asdl_রাশিমালা_seq *
_set_seq_context(Parser *p, asdl_রাশিমালা_seq *seq, রাশিমালা_context_ty ctx)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    যদি (len == 0) {
        ফেরত ফাঁকা;
    }

    asdl_রাশিমালা_seq *new_seq = Py_asdl_রাশিমালা_seq_new(len, p->arena);
    যদি (!new_seq) {
        ফেরত ফাঁকা;
    }
    জন্য (Py_ssize_t i = 0; i < len; i++) {
        রাশিমালা_ty e = asdl_seq_GET(seq, i);
        রাশিমালা_ty new_e = Py_Pegen_set_রাশিমালা_context(p, e, ctx);
        যদি (!new_e) {
            ফেরত ফাঁকা;
        }
        asdl_seq_SET(new_seq, i, new_e);
    }
    ফেরত new_seq;
}

স্থির রাশিমালা_ty
_set_name_context(Parser *p, রাশিমালা_ty e, রাশিমালা_context_ty ctx)
{
    ফেরত _PyAST_Name(e->v.Name.id, ctx, EXTRA_রাশিমালা(e, e));
}

স্থির রাশিমালা_ty
_set_tuple_context(Parser *p, রাশিমালা_ty e, রাশিমালা_context_ty ctx)
{
    asdl_রাশিমালা_seq *seq = _set_seq_context(p, e->v.Tuple.elts, ctx);
    যদি (!seq && PyErr_Occurred()) {
        ফেরত ফাঁকা;
    }
    ফেরত _PyAST_Tuple(seq, ctx, EXTRA_রাশিমালা(e, e));
}

স্থির রাশিমালা_ty
_set_list_context(Parser *p, রাশিমালা_ty e, রাশিমালা_context_ty ctx)
{
    asdl_রাশিমালা_seq *seq = _set_seq_context(p, e->v.List.elts, ctx);
    যদি (!seq && PyErr_Occurred()) {
        ফেরত ফাঁকা;
    }
    ফেরত _PyAST_List(seq, ctx, EXTRA_রাশিমালা(e, e));
}

স্থির রাশিমালা_ty
_set_subscript_context(Parser *p, রাশিমালা_ty e, রাশিমালা_context_ty ctx)
{
    ফেরত _PyAST_Subscript(e->v.Subscript.মান, e->v.Subscript.slice,
                            ctx, EXTRA_রাশিমালা(e, e));
}

স্থির রাশিমালা_ty
_set_attribute_context(Parser *p, রাশিমালা_ty e, রাশিমালা_context_ty ctx)
{
    ফেরত _PyAST_Attribute(e->v.Attribute.মান, e->v.Attribute.attr,
                            ctx, EXTRA_রাশিমালা(e, e));
}

স্থির রাশিমালা_ty
_set_starred_context(Parser *p, রাশিমালা_ty e, রাশিমালা_context_ty ctx)
{
    রাশিমালা_ty inner = Py_Pegen_set_রাশিমালা_context(p, e->v.Starred.মান, ctx);
    যদি (!inner) {
        ফেরত ফাঁকা;
    }
    ফেরত _PyAST_Starred(inner, ctx, EXTRA_রাশিমালা(e, e));
}

/* Creates an `expr_ty` equivalent to `expr` but with `ctx` as context */
রাশিমালা_ty
Py_Pegen_set_রাশিমালা_context(Parser *p, রাশিমালা_ty রাশিমালা, রাশিমালা_context_ty ctx)
{
    প্রতিজ্ঞা(রাশিমালা != ফাঁকা);

    রাশিমালা_ty new = ফাঁকা;
    নির্বাচন (রাশিমালা->kind) {
        ক্ষেত্র Name_kind:
            new = _set_name_context(p, রাশিমালা, ctx);
            বিরতি;
        ক্ষেত্র Tuple_kind:
            new = _set_tuple_context(p, রাশিমালা, ctx);
            বিরতি;
        ক্ষেত্র List_kind:
            new = _set_list_context(p, রাশিমালা, ctx);
            বিরতি;
        ক্ষেত্র Subscript_kind:
            new = _set_subscript_context(p, রাশিমালা, ctx);
            বিরতি;
        ক্ষেত্র Attribute_kind:
            new = _set_attribute_context(p, রাশিমালা, ctx);
            বিরতি;
        ক্ষেত্র Starred_kind:
            new = _set_starred_context(p, রাশিমালা, ctx);
            বিরতি;
        পূর্বনির্ধারিত:
            new = রাশিমালা;
    }
    ফেরত new;
}

/* Invalid targets return NULL without raising, allowing parsing to backtrack.
   Copy valid targets to preserve the memoized expression's context. */
রাশিমালা_ty
_PyPegen_make_target(Parser *p, রাশিমালা_ty রাশিমালা, TARGETS_TYPE targets_type)
{
    প্রতিজ্ঞা(রাশিমালা != ফাঁকা);
    প্রতিজ্ঞা(targets_type != FOR_TARGETS);
    যদি (_PyPegen_get_invalid_target(রাশিমালা, targets_type) != ফাঁকা) {
        ফেরত ফাঁকা;
    }
    ফেরত Py_Pegen_set_রাশিমালা_context(
        p, রাশিমালা, targets_type == DEL_TARGETS ? Del : Store);
}

/* Constructs a KeyValuePair that is used when parsing a dict's key value pairs */
চাবি_মান_Pair *
Py_Pegen_চাবি_মান_pair(Parser *p, রাশিমালা_ty চাবি, রাশিমালা_ty মান)
{
    চাবি_মান_Pair *a = _PyArena_Malloc(p->arena, আকার(চাবি_মান_Pair));
    যদি (!a) {
        ফেরত ফাঁকা;
    }
    a->চাবি = চাবি;
    a->মান = মান;
    ফেরত a;
}

/* Extracts all keys from an asdl_seq* of KeyValuePair*'s */
asdl_রাশিমালা_seq *
Py_Pegen_get_চাবিগুলি(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_রাশিমালা_seq *new_seq = Py_asdl_রাশিমালা_seq_new(len, p->arena);
    যদি (!new_seq) {
        ফেরত ফাঁকা;
    }
    জন্য (Py_ssize_t i = 0; i < len; i++) {
        চাবি_মান_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->চাবি);
    }
    ফেরত new_seq;
}

/* Extracts all values from an asdl_seq* of KeyValuePair*'s */
asdl_রাশিমালা_seq *
Py_Pegen_get_মানসমূহ(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_রাশিমালা_seq *new_seq = Py_asdl_রাশিমালা_seq_new(len, p->arena);
    যদি (!new_seq) {
        ফেরত ফাঁকা;
    }
    জন্য (Py_ssize_t i = 0; i < len; i++) {
        চাবি_মান_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->মান);
    }
    ফেরত new_seq;
}

/* Constructs a KeyPatternPair that is used when parsing mapping & class patterns */
চাবি_Pattern_Pair *
Py_Pegen_চাবি_pattern_pair(Parser *p, রাশিমালা_ty চাবি, pattern_ty pattern)
{
    চাবি_Pattern_Pair *a = _PyArena_Malloc(p->arena, আকার(চাবি_Pattern_Pair));
    যদি (!a) {
        ফেরত ফাঁকা;
    }
    a->চাবি = চাবি;
    a->pattern = pattern;
    ফেরত a;
}

/* Extracts all keys from an asdl_seq* of KeyPatternPair*'s */
asdl_রাশিমালা_seq *
Py_Pegen_get_pattern_চাবিগুলি(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_রাশিমালা_seq *new_seq = Py_asdl_রাশিমালা_seq_new(len, p->arena);
    যদি (!new_seq) {
        ফেরত ফাঁকা;
    }
    জন্য (Py_ssize_t i = 0; i < len; i++) {
        চাবি_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->চাবি);
    }
    ফেরত new_seq;
}

/* Extracts all patterns from an asdl_seq* of KeyPatternPair*'s */
asdl_pattern_seq *
_PyPegen_get_patterns(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_pattern_seq *new_seq = _Py_asdl_pattern_seq_new(len, p->arena);
    যদি (!new_seq) {
        ফেরত ফাঁকা;
    }
    জন্য (Py_ssize_t i = 0; i < len; i++) {
        চাবি_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->pattern);
    }
    ফেরত new_seq;
}

/* Constructs a NameDefaultPair */
NameDefaultPair *
_PyPegen_name_default_pair(Parser *p, arg_ty arg, রাশিমালা_ty মান, Token *tc)
{
    NameDefaultPair *a = _PyArena_Malloc(p->arena, আকার(NameDefaultPair));
    যদি (!a) {
        ফেরত ফাঁকা;
    }
    a->arg = _PyPegen_add_type_comment_to_arg(p, arg, tc);
    যদি (!a->arg) {
        ফেরত ফাঁকা;
    }
    a->মান = মান;
    ফেরত a;
}

/* Constructs a SlashWithDefault */
SlashWithDefault *
_PyPegen_slash_with_default(Parser *p, asdl_arg_seq *plain_names, asdl_seq *names_with_defaults)
{
    SlashWithDefault *a = _PyArena_Malloc(p->arena, আকার(SlashWithDefault));
    যদি (!a) {
        ফেরত ফাঁকা;
    }
    a->plain_names = plain_names;
    a->names_with_defaults = names_with_defaults;
    ফেরত a;
}

/* Constructs a StarEtc */
StarEtc *
_PyPegen_star_etc(Parser *p, arg_ty vararg, asdl_seq *kwonlyargs, arg_ty kwarg)
{
    StarEtc *a = _PyArena_Malloc(p->arena, আকার(StarEtc));
    যদি (!a) {
        ফেরত ফাঁকা;
    }
    a->vararg = vararg;
    a->kwonlyargs = kwonlyargs;
    a->kwarg = kwarg;
    ফেরত a;
}

asdl_seq *
_PyPegen_join_sequences(Parser *p, asdl_seq *a, asdl_seq *b)
{
    Py_ssize_t first_len = asdl_seq_LEN(a);
    Py_ssize_t second_len = asdl_seq_LEN(b);
    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(first_len + second_len, p->arena);
    যদি (!new_seq) {
        ফেরত ফাঁকা;
    }

    পূর্ণসংখ্যা k = 0;
    জন্য (Py_ssize_t i = 0; i < first_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(a, i));
    }
    জন্য (Py_ssize_t i = 0; i < second_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(b, i));
    }

    ফেরত new_seq;
}

স্থির asdl_arg_seq*
_get_names(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_arg_seq *seq = _Py_asdl_arg_seq_new(len, p->arena);
    যদি (!seq) {
        ফেরত ফাঁকা;
    }
    জন্য (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->arg);
    }
    ফেরত seq;
}

স্থির asdl_রাশিমালা_seq *
_get_defaults(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_রাশিমালা_seq *seq = Py_asdl_রাশিমালা_seq_new(len, p->arena);
    যদি (!seq) {
        ফেরত ফাঁকা;
    }
    জন্য (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->মান);
    }
    ফেরত seq;
}

স্থির পূর্ণসংখ্যা
_make_posonlyargs(Parser *p,
                  asdl_arg_seq *slash_without_default,
                  SlashWithDefault *slash_with_default,
                  asdl_arg_seq **posonlyargs) {
    যদি (slash_without_default != ফাঁকা) {
        *posonlyargs = slash_without_default;
    }
    অন্যথা যদি (slash_with_default != ফাঁকা) {
        asdl_arg_seq *slash_with_default_names =
                _get_names(p, slash_with_default->names_with_defaults);
        যদি (!slash_with_default_names) {
            ফেরত -1;
        }
        *posonlyargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default->plain_names,
                (asdl_seq*)slash_with_default_names);
    }
    অন্যথা {
        *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }
    ফেরত *posonlyargs == ফাঁকা ? -1 : 0;
}

স্থির পূর্ণসংখ্যা
_make_posargs(Parser *p,
              asdl_arg_seq *plain_names,
              asdl_seq *names_with_default,
              asdl_arg_seq **posargs) {

    যদি (names_with_default != ফাঁকা) {
        যদি (plain_names != ফাঁকা) {
            asdl_arg_seq *names_with_default_names = _get_names(p, names_with_default);
            যদি (!names_with_default_names) {
                ফেরত -1;
            }
            *posargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                    p,(asdl_seq*)plain_names, (asdl_seq*)names_with_default_names);
        }
        অন্যথা {
            *posargs = _get_names(p, names_with_default);
        }
    }
    অন্যথা {
        যদি (plain_names != ফাঁকা) {
            // With the current grammar, we never get here.
            // If that has changed, remove the assert, and test thoroughly.
            প্রতিজ্ঞা(0);
            *posargs = plain_names;
        }
        অন্যথা {
            *posargs = _Py_asdl_arg_seq_new(0, p->arena);
        }
    }
    ফেরত *posargs == ফাঁকা ? -1 : 0;
}

স্থির পূর্ণসংখ্যা
_make_posdefaults(Parser *p,
                  SlashWithDefault *slash_with_default,
                  asdl_seq *names_with_default,
                  asdl_রাশিমালা_seq **posdefaults) {
    যদি (slash_with_default != ফাঁকা && names_with_default != ফাঁকা) {
        asdl_রাশিমালা_seq *slash_with_default_মানসমূহ =
                _get_defaults(p, slash_with_default->names_with_defaults);
        যদি (!slash_with_default_মানসমূহ) {
            ফেরত -1;
        }
        asdl_রাশিমালা_seq *names_with_default_মানসমূহ = _get_defaults(p, names_with_default);
        যদি (!names_with_default_মানসমূহ) {
            ফেরত -1;
        }
        *posdefaults = (asdl_রাশিমালা_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default_মানসমূহ,
                (asdl_seq*)names_with_default_মানসমূহ);
    }
    অন্যথা যদি (slash_with_default == ফাঁকা && names_with_default != ফাঁকা) {
        *posdefaults = _get_defaults(p, names_with_default);
    }
    অন্যথা যদি (slash_with_default != ফাঁকা && names_with_default == ফাঁকা) {
        *posdefaults = _get_defaults(p, slash_with_default->names_with_defaults);
    }
    অন্যথা {
        *posdefaults = Py_asdl_রাশিমালা_seq_new(0, p->arena);
    }
    ফেরত *posdefaults == ফাঁকা ? -1 : 0;
}

স্থির পূর্ণসংখ্যা
_make_kwargs(Parser *p, StarEtc *star_etc,
             asdl_arg_seq **kwonlyargs,
             asdl_রাশিমালা_seq **kwdefaults) {
    যদি (star_etc != ফাঁকা && star_etc->kwonlyargs != ফাঁকা) {
        *kwonlyargs = _get_names(p, star_etc->kwonlyargs);
    }
    অন্যথা {
        *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }

    যদি (*kwonlyargs == ফাঁকা) {
        ফেরত -1;
    }

    যদি (star_etc != ফাঁকা && star_etc->kwonlyargs != ফাঁকা) {
        *kwdefaults = _get_defaults(p, star_etc->kwonlyargs);
    }
    অন্যথা {
        *kwdefaults = Py_asdl_রাশিমালা_seq_new(0, p->arena);
    }

    যদি (*kwdefaults == ফাঁকা) {
        ফেরত -1;
    }

    ফেরত 0;
}

/* Constructs an arguments_ty object out of all the parsed constructs in the parameters rule */
arguments_ty
_PyPegen_make_arguments(Parser *p, asdl_arg_seq *slash_without_default,
                        SlashWithDefault *slash_with_default, asdl_arg_seq *plain_names,
                        asdl_seq *names_with_default, StarEtc *star_etc)
{
    asdl_arg_seq *posonlyargs;
    যদি (_make_posonlyargs(p, slash_without_default, slash_with_default, &posonlyargs) == -1) {
        ফেরত ফাঁকা;
    }

    asdl_arg_seq *posargs;
    যদি (_make_posargs(p, plain_names, names_with_default, &posargs) == -1) {
        ফেরত ফাঁকা;
    }

    asdl_রাশিমালা_seq *posdefaults;
    যদি (_make_posdefaults(p,slash_with_default, names_with_default, &posdefaults) == -1) {
        ফেরত ফাঁকা;
    }

    arg_ty vararg = ফাঁকা;
    যদি (star_etc != ফাঁকা && star_etc->vararg != ফাঁকা) {
        vararg = star_etc->vararg;
    }

    asdl_arg_seq *kwonlyargs;
    asdl_রাশিমালা_seq *kwdefaults;
    যদি (_make_kwargs(p, star_etc, &kwonlyargs, &kwdefaults) == -1) {
        ফেরত ফাঁকা;
    }

    arg_ty kwarg = ফাঁকা;
    যদি (star_etc != ফাঁকা && star_etc->kwarg != ফাঁকা) {
        kwarg = star_etc->kwarg;
    }

    ফেরত _PyAST_arguments(posonlyargs, posargs, vararg, kwonlyargs,
                            kwdefaults, kwarg, posdefaults, p->arena);
}


/* Constructs an empty arguments_ty object, that gets used when a function accepts no
 * arguments. */
arguments_ty
_PyPegen_empty_arguments(Parser *p)
{
    asdl_arg_seq *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    যদি (!posonlyargs) {
        ফেরত ফাঁকা;
    }
    asdl_arg_seq *posargs = _Py_asdl_arg_seq_new(0, p->arena);
    যদি (!posargs) {
        ফেরত ফাঁকা;
    }
    asdl_রাশিমালা_seq *posdefaults = Py_asdl_রাশিমালা_seq_new(0, p->arena);
    যদি (!posdefaults) {
        ফেরত ফাঁকা;
    }
    asdl_arg_seq *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    যদি (!kwonlyargs) {
        ফেরত ফাঁকা;
    }
    asdl_রাশিমালা_seq *kwdefaults = Py_asdl_রাশিমালা_seq_new(0, p->arena);
    যদি (!kwdefaults) {
        ফেরত ফাঁকা;
    }

    ফেরত _PyAST_arguments(posonlyargs, posargs, ফাঁকা, kwonlyargs,
                            kwdefaults, ফাঁকা, posdefaults, p->arena);
}

/* Encapsulates the value of an operator_ty into an AugOperator struct */
AugOperator *
_PyPegen_augoperator(Parser *p, operator_ty kind)
{
    AugOperator *a = _PyArena_Malloc(p->arena, আকার(AugOperator));
    যদি (!a) {
        ফেরত ফাঁকা;
    }
    a->kind = kind;
    ফেরত a;
}

/* Construct a FunctionDef equivalent to function_def, but with decorators */
stmt_ty
_PyPegen_function_def_decorators(Parser *p, asdl_রাশিমালা_seq *decorators, stmt_ty function_def)
{
    প্রতিজ্ঞা(function_def != ফাঁকা);
    যদি (function_def->kind == AsyncFunctionDef_kind) {
        ফেরত _PyAST_AsyncFunctionDef(
            function_def->v.AsyncFunctionDef.name,
            function_def->v.AsyncFunctionDef.args,
            function_def->v.AsyncFunctionDef.body, decorators,
            function_def->v.AsyncFunctionDef.returns,
            function_def->v.AsyncFunctionDef.type_comment,
            function_def->v.AsyncFunctionDef.type_params,
            function_def->lineno, function_def->col_offset,
            function_def->end_lineno, function_def->end_col_offset, p->arena);
    }

    ফেরত _PyAST_FunctionDef(
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
_PyPegen_class_def_decorators(Parser *p, asdl_রাশিমালা_seq *decorators, stmt_ty class_def)
{
    প্রতিজ্ঞা(class_def != ফাঁকা);
    ফেরত _PyAST_ClassDef(
        class_def->v.ClassDef.name,
        class_def->v.ClassDef.bases, class_def->v.ClassDef.keywords,
        class_def->v.ClassDef.body, decorators,
        class_def->v.ClassDef.type_params,
        class_def->lineno, class_def->col_offset, class_def->end_lineno,
        class_def->end_col_offset, p->arena);
}

/* Construct a KeywordOrStarred */
KeywordOrStarred *
_PyPegen_keyword_or_starred(Parser *p, শূন্য *element, পূর্ণসংখ্যা is_keyword)
{
    KeywordOrStarred *a = _PyArena_Malloc(p->arena, আকার(KeywordOrStarred));
    যদি (!a) {
        ফেরত ফাঁকা;
    }
    a->element = element;
    a->is_keyword = is_keyword;
    ফেরত a;
}

/* Get the number of starred expressions in an asdl_seq* of KeywordOrStarred*s */
স্থির পূর্ণসংখ্যা
seq_সংখ্যা_of_starred_exprs(asdl_seq *seq)
{
    পূর্ণসংখ্যা n = 0;
    জন্য (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(seq, i);
        যদি (!k->is_keyword) {
            n++;
        }
    }
    ফেরত n;
}

/* Extract the starred expressions of an asdl_seq* of KeywordOrStarred*s */
asdl_রাশিমালা_seq *
_PyPegen_seq_extract_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    পূর্ণসংখ্যা new_len = seq_সংখ্যা_of_starred_exprs(kwargs);
    যদি (new_len == 0) {
        ফেরত ফাঁকা;
    }
    asdl_রাশিমালা_seq *new_seq = Py_asdl_রাশিমালা_seq_new(new_len, p->arena);
    যদি (!new_seq) {
        ফেরত ফাঁকা;
    }

    পূর্ণসংখ্যা idx = 0;
    জন্য (Py_ssize_t i = 0, len = asdl_seq_LEN(kwargs); i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        যদি (!k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    ফেরত new_seq;
}

/* Return a new asdl_seq* with only the keywords in kwargs */
asdl_keyword_seq*
_PyPegen_seq_delete_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    Py_ssize_t len = asdl_seq_LEN(kwargs);
    Py_ssize_t new_len = len - seq_সংখ্যা_of_starred_exprs(kwargs);
    যদি (new_len == 0) {
        ফেরত ফাঁকা;
    }
    asdl_keyword_seq *new_seq = _Py_asdl_keyword_seq_new(new_len, p->arena);
    যদি (!new_seq) {
        ফেরত ফাঁকা;
    }

    পূর্ণসংখ্যা idx = 0;
    জন্য (Py_ssize_t i = 0; i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        যদি (k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    ফেরত new_seq;
}

রাশিমালা_ty
_PyPegen_ensure_imaginary(Parser *p, রাশিমালা_ty exp)
{
    যদি (exp->kind != Constant_kind || !PyComplex_CheckExact(exp->v.Constant.মান)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "imaginary number required in complex literal");
        ফেরত ফাঁকা;
    }
    ফেরত exp;
}

রাশিমালা_ty
_PyPegen_ensure_real(Parser *p, রাশিমালা_ty exp)
{
    যদি (exp->kind != Constant_kind || PyComplex_CheckExact(exp->v.Constant.মান)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "real number required in complex literal");
        ফেরত ফাঁকা;
    }
    ফেরত exp;
}

mod_ty
_PyPegen_make_module(Parser *p, asdl_stmt_seq *a) {
    asdl_type_ignore_seq *type_ignores = ফাঁকা;
    Py_ssize_t অঙ্ক = p->type_ignore_comments.অঙ্ক_বস্তুসমূহ;
    যদি (অঙ্ক > 0) {
        // Turn the raw (comment, lineno) pairs into TypeIgnore objects in the arena
        type_ignores = _Py_asdl_type_ignore_seq_new(অঙ্ক, p->arena);
        যদি (type_ignores == ফাঁকা) {
            ফেরত ফাঁকা;
        }
        জন্য (Py_ssize_t i = 0; i < অঙ্ক; i++) {
            PyObject *tag = _PyPegen_new_type_comment(p, p->type_ignore_comments.বস্তুসমূহ[i].comment);
            যদি (tag == ফাঁকা) {
                ফেরত ফাঁকা;
            }
            type_ignore_ty ti = _PyAST_TypeIgnore(p->type_ignore_comments.বস্তুসমূহ[i].lineno,
                                                  tag, p->arena);
            যদি (ti == ফাঁকা) {
                ফেরত ফাঁকা;
            }
            asdl_seq_SET(type_ignores, i, ti);
        }
    }
    ফেরত _PyAST_Module(a, type_ignores, p->arena);
}

PyObject *
_PyPegen_new_type_comment(Parser *p, ধ্রুবক অক্ষর *s)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, strlen(s), ফাঁকা);
    যদি (res == ফাঁকা) {
        ফেরত ফাঁকা;
    }
    যদি (_PyArena_AddPyObject(p->arena, res) < 0) {
        Py_DECREF(res);
        ফেরত ফাঁকা;
    }
    ফেরত res;
}

arg_ty
_PyPegen_add_type_comment_to_arg(Parser *p, arg_ty a, Token *tc)
{
    যদি (tc == ফাঁকা) {
        ফেরত a;
    }
    ধ্রুবক অক্ষর *bytes = PyBytes_AsString(tc->bytes);
    যদি (bytes == ফাঁকা) {
        ফেরত ফাঁকা;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    যদি (tco == ফাঁকা) {
        ফেরত ফাঁকা;
    }
    ফেরত _PyAST_arg(a->arg, a->annotation, tco,
                      a->lineno, a->col_offset, a->end_lineno, a->end_col_offset,
                      p->arena);
}

/* Checks if the NOTEQUAL token is valid given the current parser flags
0 indicates success and nonzero indicates failure (an exception may be set) */
পূর্ণসংখ্যা
_PyPegen_check_barry_as_flufl(Parser *p, Token* t) {
    প্রতিজ্ঞা(t->bytes != ফাঁকা);
    প্রতিজ্ঞা(t->type == NOTEQUAL);

    ধ্রুবক অক্ষর* tok_str = PyBytes_AS_STRING(t->bytes);
    যদি (p->flags & PyPARSE_BARRY_AS_BDFL && strcmp(tok_str, "<>") != 0) {
        RAISE_SYNTAX_ERROR("with Barry as BDFL, use '<>' instead of '!='");
        ফেরত -1;
    }
    যদি (!(p->flags & PyPARSE_BARRY_AS_BDFL)) {
        ফেরত strcmp(tok_str, "!=");
    }
    ফেরত 0;
}

পূর্ণসংখ্যা
_PyPegen_check_legacy_stmt(Parser *p, রাশিমালা_ty name) {
    যদি (name->kind != Name_kind) {
        ফেরত 0;
    }
    ধ্রুবক অক্ষর* candidates[2] = {"print", "exec"};
    জন্য (পূর্ণসংখ্যা i=0; i<2; i++) {
        যদি (PyUnicode_CompareWithASCIIString(name->v.Name.id, candidates[i]) == 0) {
            ফেরত 1;
        }
    }
    ফেরত 0;
}

শূন্য *
_PyPegen_raise_error_for_missing_comma(Parser *p, রাশিমালা_ty a, রাশিমালা_ty b)
{
    // Don't raise for legacy statements like "print x" or "exec x"
    যদি (_PyPegen_check_legacy_stmt(p, a)) {
        ফেরত ফাঁকা;
    }
    // Only raise inside parentheses/brackets (level > 0)
    যদি (p->tokens[p->mark - 1]->level == 0) {
        ফেরত ফাঁকা;
    }
    // For multi-line expressions (like string concatenations), point to the
    // last line instead of the first for a more helpful error message.
    // Use a->col_offset as the starting column since all strings in the
    // concatenation typically share the same indentation.
    যদি (a->end_lineno > a->lineno) {
        ফেরত RAISE_ERROR_KNOWN_LOCATION(
            p, PyExc_SyntaxError, a->end_lineno, a->col_offset,
            a->end_lineno, a->end_col_offset,
            "invalid syntax. Perhaps you forgot a comma?"
        );
    }
    ফেরত RAISE_ERROR_KNOWN_LOCATION(
        p, PyExc_SyntaxError, a->lineno, a->col_offset,
        b->end_lineno, b->end_col_offset,
        "invalid syntax. Perhaps you forgot a comma?"
    );
}

স্থির ফলাফল_Token_With_Metadata *
ফলাফল_token_with_metadata(Parser *p, শূন্য *ফলাফল, PyObject *metadata)
{
    ফলাফল_Token_With_Metadata *res = _PyArena_Malloc(p->arena, আকার(ফলাফল_Token_With_Metadata));
    যদি (res == ফাঁকা) {
        ফেরত ফাঁকা;
    }
    res->metadata = metadata;
    res->ফলাফল = ফলাফল;
    ফেরত res;
}

স্থির অক্ষর
formatted_string_prefix(ধ্রুবক Parser *p)
{
    পূর্ণসংখ্যা nested = 0;
    জন্য (পূর্ণসংখ্যা i = p->mark - 1; i >= 0; i--) {
        পূর্ণসংখ্যা type = p->tokens[i]->type;
        যদি (type == FSTRING_END || type == TSTRING_END) {
            nested++;
        }
        অন্যথা যদি (type == FSTRING_START || type == TSTRING_START) {
            যদি (nested == 0) {
                ফেরত type == TSTRING_START ? 't' : 'f';
            }
            nested--;
        }
    }
    Py_UNREACHABLE();
}

ফলাফল_Token_With_Metadata *
_PyPegen_check_fstring_conversion(Parser *p, Token* conv_token, রাশিমালা_ty conv)
{
    যদি (conv_token->lineno != conv->lineno || conv_token->end_col_offset != conv->col_offset) {
        ফেরত RAISE_SYNTAX_ERROR_KNOWN_RANGE(
            conv_token, conv,
            "%c-string: conversion type must come right after the exclamation mark",
            formatted_string_prefix(p)
        );
    }

    Py_UCS4 first = PyUnicode_READ_CHAR(conv->v.Name.id, 0);
    যদি (Py_Unicode_GET_দৈর্ঘ্য(conv->v.Name.id) > 1 ||
            !(first == 's' || first == 'r' || first == 'a')) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(conv,
                                            "%c-string: invalid conversion character %R: expected 's', 'r', or 'a'",
                                            formatted_string_prefix(p),
                                            conv->v.Name.id);
        ফেরত ফাঁকা;
    }

    ফেরত ফলাফল_token_with_metadata(p, conv, conv_token->metadata);
}

ফলাফল_Token_With_Metadata *
_PyPegen_setup_full_format_spec(Parser *p, Token *colon, asdl_রাশিমালা_seq *spec, পূর্ণসংখ্যা lineno, পূর্ণসংখ্যা col_offset,
                                পূর্ণসংখ্যা end_lineno, পূর্ণসংখ্যা end_col_offset, PyArena *arena)
{
    যদি (!spec) {
        ফেরত ফাঁকা;
    }

    // This is needed to keep compatibility with 3.11, where an empty format
    // spec is parsed as an *empty* JoinedStr node, instead of having an empty
    // constant in it.
    Py_ssize_t n_বস্তুসমূহ = asdl_seq_LEN(spec);
    Py_ssize_t non_empty_গুনতি = 0;
    জন্য (Py_ssize_t i = 0; i < n_বস্তুসমূহ; i++) {
        রাশিমালা_ty item = asdl_seq_GET(spec, i);
        non_empty_গুনতি += !(item->kind == Constant_kind &&
                             PyUnicode_CheckExact(item->v.Constant.মান) &&
                             Py_Unicode_GET_দৈর্ঘ্য(item->v.Constant.মান) == 0);
    }
    যদি (non_empty_গুনতি != n_বস্তুসমূহ) {
        asdl_রাশিমালা_seq *resized_spec =
            Py_asdl_রাশিমালা_seq_new(non_empty_গুনতি, p->arena);
        যদি (resized_spec == ফাঁকা) {
            ফেরত ফাঁকা;
        }
        Py_ssize_t j = 0;
        জন্য (Py_ssize_t i = 0; i < n_বস্তুসমূহ; i++) {
            রাশিমালা_ty item = asdl_seq_GET(spec, i);
            যদি (item->kind == Constant_kind &&
                PyUnicode_CheckExact(item->v.Constant.মান) &&
                Py_Unicode_GET_দৈর্ঘ্য(item->v.Constant.মান) == 0) {
                চালু;
            }
            asdl_seq_SET(resized_spec, j++, item);
        }
        প্রতিজ্ঞা(j == non_empty_গুনতি);
        spec = resized_spec;
    }
    রাশিমালা_ty res;
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
        ফেরত ফাঁকা;
    }
    ফেরত ফলাফল_token_with_metadata(p, res, colon->metadata);
}

ধ্রুবক অক্ষর *
Py_Pegen_get_রাশিমালা_name(রাশিমালা_ty e)
{
    প্রতিজ্ঞা(e != ফাঁকা);
    নির্বাচন (e->kind) {
        ক্ষেত্র Attribute_kind:
            ফেরত "attribute";
        ক্ষেত্র Subscript_kind:
            ফেরত "subscript";
        ক্ষেত্র Starred_kind:
            ফেরত "starred";
        ক্ষেত্র Name_kind:
            ফেরত "name";
        ক্ষেত্র List_kind:
            ফেরত "list";
        ক্ষেত্র Tuple_kind:
            ফেরত "tuple";
        ক্ষেত্র Lambda_kind:
            ফেরত "lambda";
        ক্ষেত্র Call_kind:
            ফেরত "function call";
        ক্ষেত্র BoolOp_kind:
        ক্ষেত্র BinOp_kind:
        ক্ষেত্র UnaryOp_kind:
            ফেরত "expression";
        ক্ষেত্র GeneratorExp_kind:
            ফেরত "generator expression";
        ক্ষেত্র Yield_kind:
        ক্ষেত্র YieldFrom_kind:
            ফেরত "yield expression";
        ক্ষেত্র Await_kind:
            ফেরত "await expression";
        ক্ষেত্র ListComp_kind:
            ফেরত "list comprehension";
        ক্ষেত্র SetComp_kind:
            ফেরত "set comprehension";
        ক্ষেত্র DictComp_kind:
            ফেরত "dict comprehension";
        ক্ষেত্র Dict_kind:
            ফেরত "dict literal";
        ক্ষেত্র Set_kind:
            ফেরত "set display";
        ক্ষেত্র JoinedStr_kind:
        ক্ষেত্র Formatted_মান_kind:
            ফেরত "f-string expression";
        ক্ষেত্র TemplateStr_kind:
        ক্ষেত্র Interpolation_kind:
            ফেরত "t-string expression";
        ক্ষেত্র Constant_kind: {
            PyObject *মান = e->v.Constant.মান;
            যদি (মান == Py_None) {
                ফেরত "None";
            }
            যদি (মান == Py_False) {
                ফেরত "False";
            }
            যদি (মান == Py_True) {
                ফেরত "True";
            }
            যদি (মান == Py_Ellipsis) {
                ফেরত "ellipsis";
            }
            ফেরত "literal";
        }
        ক্ষেত্র Compare_kind:
            ফেরত "comparison";
        ক্ষেত্র IfExp_kind:
            ফেরত "conditional expression";
        ক্ষেত্র Named_রাশিমালা_kind:
            ফেরত "named expression";
        পূর্বনির্ধারিত:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->kind, e->lineno);
            ফেরত ফাঁকা;
    }
}

রাশিমালা_ty
_PyPegen_get_last_comprehension_item(comprehension_ty comprehension) {
    যদি (comprehension->ifs == ফাঁকা || asdl_seq_LEN(comprehension->ifs) == 0) {
        ফেরত comprehension->iter;
    }
    ফেরত PyPegen_last_item(comprehension->ifs, রাশিমালা_ty);
}

রাশিমালা_ty _PyPegen_collect_call_seqs(Parser *p, asdl_রাশিমালা_seq *a, asdl_seq *b,
                     পূর্ণসংখ্যা lineno, পূর্ণসংখ্যা col_offset, পূর্ণসংখ্যা end_lineno,
                     পূর্ণসংখ্যা end_col_offset, PyArena *arena) {
    Py_ssize_t args_len = asdl_seq_LEN(a);
    Py_ssize_t যোগ_len = args_len;

    যদি (b == ফাঁকা) {
        ফেরত _PyAST_Call(_PyPegen_dummy_name(p), a, ফাঁকা, lineno, col_offset,
                        end_lineno, end_col_offset, arena);

    }

    asdl_রাশিমালা_seq *starreds = _PyPegen_seq_extract_starred_exprs(p, b);
    যদি (!starreds && PyErr_Occurred()) {
        ফেরত ফাঁকা;
    }

    asdl_keyword_seq *keywords = _PyPegen_seq_delete_starred_exprs(p, b);
    যদি (!keywords && PyErr_Occurred()) {
        ফেরত ফাঁকা;
    }

    যদি (starreds) {
        যোগ_len += asdl_seq_LEN(starreds);
    }

    asdl_রাশিমালা_seq *args = Py_asdl_রাশিমালা_seq_new(যোগ_len, arena);
    যদি (args == ফাঁকা) {
        ফেরত ফাঁকা;
    }

    Py_ssize_t i = 0;
    জন্য (i = 0; i < args_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(a, i));
    }
    জন্য (; i < যোগ_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(starreds, i - args_len));
    }

    ফেরত _PyAST_Call(_PyPegen_dummy_name(p), args, keywords, lineno,
                       col_offset, end_lineno, end_col_offset, arena);
}

// AST Error reporting helpers

রাশিমালা_ty
_PyPegen_get_invalid_target(রাশিমালা_ty e, TARGETS_TYPE targets_type)
{
    যদি (e == ফাঁকা) {
        ফেরত ফাঁকা;
    }

    যদি (targets_type == SINGLE_TARGETS ||
        targets_type == ATTRIBUTE_OR_SUBSCRIPT_TARGETS) {
        যদি (targets_type == SINGLE_TARGETS && e->kind == Name_kind) {
            ফেরত ফাঁকা;
        }
        ফেরত e->kind == Attribute_kind || e->kind == Subscript_kind ? ফাঁকা : e;
    }

#define VISIT_CONTAINER(CONTAINER, TYPE) করো { \
        Py_ssize_t len = asdl_seq_LEN((CONTAINER)->v.TYPE.elts);\
        জন্য (Py_ssize_t i = 0; i < len; i++) {\
            রাশিমালা_ty other = asdl_seq_GET((CONTAINER)->v.TYPE.elts, i);\
            রাশিমালা_ty child = _PyPegen_get_invalid_target(other, targets_type);\
            যদি (child != ফাঁকা) {\
                ফেরত child;\
            }\
        }\
    } যখন (0)

    // We only need to visit List and Tuple nodes recursively as those
    // are the only ones that can contain valid names in targets when
    // they are parsed as expressions. Any other kind of expression
    // that is a container (like Sets or Dicts) is directly invalid and
    // we don't need to visit it recursively.

    নির্বাচন (e->kind) {
        ক্ষেত্র List_kind:
            VISIT_CONTAINER(e, List);
            ফেরত ফাঁকা;
        ক্ষেত্র Tuple_kind:
            VISIT_CONTAINER(e, Tuple);
            ফেরত ফাঁকা;
        ক্ষেত্র Starred_kind:
            যদি (targets_type == DEL_TARGETS) {
                ফেরত e;
            }
            ফেরত _PyPegen_get_invalid_target(e->v.Starred.মান, targets_type);
        ক্ষেত্র Compare_kind:
            // This is needed, because the `a in b` in `for a in b` gets parsed
            // as a comparison, and so we need to search the left side of the comparison
            // for invalid targets.
            যদি (targets_type == FOR_TARGETS) {
                cmpop_ty cmpop = (cmpop_ty) asdl_seq_GET(e->v.Compare.ops, 0);
                যদি (cmpop == In) {
                    ফেরত _PyPegen_get_invalid_target(e->v.Compare.বাম, targets_type);
                }
                ফেরত ফাঁকা;
            }
            ফেরত e;
        ক্ষেত্র Name_kind:
        ক্ষেত্র Subscript_kind:
        ক্ষেত্র Attribute_kind:
            ফেরত ফাঁকা;
        পূর্বনির্ধারিত:
            ফেরত e;
    }
}

শূন্য *_PyPegen_arguments_parsing_error(Parser *p, রাশিমালা_ty e) {
    পূর্ণসংখ্যা kwarg_unpacking = 0;
    জন্য (Py_ssize_t i = 0, l = asdl_seq_LEN(e->v.Call.keywords); i < l; i++) {
        keyword_ty keyword = asdl_seq_GET(e->v.Call.keywords, i);
        যদি (!keyword->arg) {
            kwarg_unpacking = 1;
        }
    }

    ধ্রুবক অক্ষর *বার্তা = ফাঁকা;
    যদি (kwarg_unpacking) {
        বার্তা = "positional argument follows keyword argument unpacking";
    } অন্যথা {
        বার্তা = "positional argument follows keyword argument";
    }

    ফেরত RAISE_SYNTAX_ERROR(বার্তা);
}

শূন্য *
_PyPegen_nonparen_genexp_in_call(Parser *p, রাশিমালা_ty args, asdl_comprehension_seq *comprehensions)
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
        ফেরত ফাঁকা;
    }

    comprehension_ty last_comprehension = PyPegen_last_item(comprehensions, comprehension_ty);

    ফেরত RAISE_SYNTAX_ERROR_KNOWN_RANGE(
        (রাশিমালা_ty) asdl_seq_GET(args->v.Call.args, len - 1),
        _PyPegen_get_last_comprehension_item(last_comprehension),
        "Generator expression must be parenthesized"
    );
}

// Fstring stuff

স্থির রাশিমালা_ty
_PyPegen_decode_fstring_part(Parser* p, পূর্ণসংখ্যা is_raw, রাশিমালা_ty constant, Token* token) {
    প্রতিজ্ঞা(PyUnicode_CheckExact(constant->v.Constant.মান));

    ধ্রুবক অক্ষর* bstr = PyUnicode_AsUTF8(constant->v.Constant.মান);
    যদি (bstr == ফাঁকা) {
        ফেরত ফাঁকা;
    }

    আকার_t len;
    যদি (strcmp(bstr, "{{") == 0 || strcmp(bstr, "}}") == 0) {
        len = 1;
    } অন্যথা {
        len = strlen(bstr);
    }

    is_raw = is_raw || strchr(bstr, '\\') == ফাঁকা;
    PyObject *str = _PyPegen_decode_string(p, is_raw, bstr, len, token);
    যদি (str == ফাঁকা) {
        _Pypegen_raise_decode_error(p);
        ফেরত ফাঁকা;
    }
    যদি (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ফেরত ফাঁকা;
    }
    ফেরত _PyAST_Constant(str, ফাঁকা, constant->lineno, constant->col_offset,
                           constant->end_lineno, constant->end_col_offset,
                           p->arena);
}

স্থির asdl_রাশিমালা_seq *
_get_resized_exprs(Parser *p, Token *a, asdl_রাশিমালা_seq *raw_expressions,
                   Token *b, পূর্ণসংখ্যা is_tstring)
{
    Py_ssize_t n_বস্তুসমূহ = asdl_seq_LEN(raw_expressions);
    Py_ssize_t যোগ_বস্তুসমূহ = n_বস্তুসমূহ;
    জন্য (Py_ssize_t i = 0; i < n_বস্তুসমূহ; i++) {
        রাশিমালা_ty item = asdl_seq_GET(raw_expressions, i);
        যদি (item->kind == JoinedStr_kind) {
            যোগ_বস্তুসমূহ += asdl_seq_LEN(item->v.JoinedStr.মানসমূহ) - 1;
        }
    }

    ধ্রুবক অক্ষর* quote_str = PyBytes_AsString(a->bytes);
    যদি (quote_str == ফাঁকা) {
        ফেরত ফাঁকা;
    }
    পূর্ণসংখ্যা is_raw = strpbrk(quote_str, "rR") != ফাঁকা;

    asdl_রাশিমালা_seq *seq = Py_asdl_রাশিমালা_seq_new(যোগ_বস্তুসমূহ, p->arena);
    যদি (seq == ফাঁকা) {
        ফেরত ফাঁকা;
    }

    Py_ssize_t সূচক = 0;
    জন্য (Py_ssize_t i = 0; i < n_বস্তুসমূহ; i++) {
        রাশিমালা_ty item = asdl_seq_GET(raw_expressions, i);

        /* Debug expressions arrive as JoinedStr(text, value); flatten them
           into the surrounding string. */
        যদি (item->kind == JoinedStr_kind) {
            asdl_রাশিমালা_seq *মানসমূহ = item->v.JoinedStr.মানসমূহ;
            যদি (asdl_seq_LEN(মানসমূহ) != 2) {
                PyErr_Format(PyExc_SystemError,
                             is_tstring
                             ? "unexpected TemplateStr node without debug data in t-string at line %d"
                             : "unexpected JoinedStr node without debug data in f-string at line %d",
                             item->lineno);
                ফেরত ফাঁকা;
            }

            রাশিমালা_ty first = asdl_seq_GET(মানসমূহ, 0);
            প্রতিজ্ঞা(first->kind == Constant_kind);
            asdl_seq_SET(seq, সূচক++, first);

            রাশিমালা_ty second = asdl_seq_GET(মানসমূহ, 1);
            প্রতিজ্ঞা((is_tstring &&
                    second->kind == Interpolation_kind) ||
                   second->kind == Formatted_মান_kind);
            asdl_seq_SET(seq, সূচক++, second);

            চালু;
        }

        যদি (item->kind == Constant_kind) {
            item = _PyPegen_decode_fstring_part(p, is_raw, item, b);
            যদি (item == ফাঁকা) {
                ফেরত ফাঁকা;
            }

            /* Tokenizer emits string parts even when the underlying string
            might become an empty value (e.g. FSTRING_MIDDLE with the value \\n)
            so we need to check for them and simplify it here. */
            যদি (PyUnicode_CheckExact(item->v.Constant.মান)
                && Py_Unicode_GET_দৈর্ঘ্য(item->v.Constant.মান) == 0) {
                চালু;
            }
        }
        asdl_seq_SET(seq, সূচক++, item);
    }

    asdl_রাশিমালা_seq *resized_exprs;
    যদি (সূচক != যোগ_বস্তুসমূহ) {
        resized_exprs = Py_asdl_রাশিমালা_seq_new(সূচক, p->arena);
        যদি (resized_exprs == ফাঁকা) {
            ফেরত ফাঁকা;
        }
        জন্য (Py_ssize_t i = 0; i < সূচক; i++) {
            asdl_seq_SET(resized_exprs, i, asdl_seq_GET(seq, i));
        }
    }
    অন্যথা {
        resized_exprs = seq;
    }
    ফেরত resized_exprs;
}

রাশিমালা_ty
_PyPegen_template_str(Parser *p, Token *a, asdl_রাশিমালা_seq *raw_expressions, Token *b) {

    asdl_রাশিমালা_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 1);
    যদি (resized_exprs == ফাঁকা) {
        ফেরত ফাঁকা;
    }
    ফেরত _PyAST_TemplateStr(resized_exprs, a->lineno, a->col_offset,
                              b->end_lineno, b->end_col_offset,
                              p->arena);
}

রাশিমালা_ty
_PyPegen_joined_str(Parser *p, Token* a, asdl_রাশিমালা_seq* raw_expressions, Token*b) {

    asdl_রাশিমালা_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 0);
    যদি (resized_exprs == ফাঁকা) {
        ফেরত ফাঁকা;
    }
    ফেরত _PyAST_JoinedStr(resized_exprs, a->lineno, a->col_offset,
                            b->end_lineno, b->end_col_offset,
                            p->arena);
}

রাশিমালা_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok) {
    যদি (tok->bytes == ফাঁকা) {
        ফেরত Py_Pegen_raise_incomplete_প্রবেশ(p);
    }
    Py_ssize_t bsize = Py_Bytes_GET_আকার(tok->bytes);
    ধ্রুবক অক্ষর *bstr = PyBytes_AS_STRING(tok->bytes);

    পূর্ণসংখ্যা is_raw = tok->is_raw;

    PyObject* str = _PyPegen_decode_string(p, is_raw, bstr, bsize, tok);
    যদি (str == ফাঁকা) {
        ফেরত ফাঁকা;
    }
    যদি (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ফেরত ফাঁকা;
    }
    ফেরত _PyAST_Constant(str, ফাঁকা, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

রাশিমালা_ty _PyPegen_constant_from_token(Parser* p, Token* tok) {
    যদি (tok->bytes == ফাঁকা) {
        ফেরত Py_Pegen_raise_incomplete_প্রবেশ(p);
    }
    ধ্রুবক অক্ষর *bstr = PyBytes_AS_STRING(tok->bytes);
    PyObject* str = PyUnicode_FromString(bstr);
    যদি (str == ফাঁকা) {
        ফেরত ফাঁকা;
    }
    যদি (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        ফেরত ফাঁকা;
    }
    ফেরত _PyAST_Constant(str, ফাঁকা, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

রাশিমালা_ty _PyPegen_constant_from_string(Parser* p, Token* tok) {
    যদি (tok->bytes == ফাঁকা) {
        ফেরত Py_Pegen_raise_incomplete_প্রবেশ(p);
    }
    ধ্রুবক অক্ষর *the_str = PyBytes_AS_STRING(tok->bytes);
    PyObject *s = _PyPegen_parse_string(p, tok);
    যদি (s == ফাঁকা) {
        _Pypegen_raise_decode_error(p);
        ফেরত ফাঁকা;
    }
    যদি (_PyArena_AddPyObject(p->arena, s) < 0) {
        Py_DECREF(s);
        ফেরত ফাঁকা;
    }
    PyObject *kind = ফাঁকা;
    যদি (the_str && the_str[0] == 'u') {
        kind = _PyPegen_new_identifier(p, "u");
        যদি (kind == ফাঁকা) {
            ফেরত ফাঁকা;
        }
    }
    ফেরত _PyAST_Constant(s, kind, tok->lineno, tok->col_offset, tok->end_lineno, tok->end_col_offset, p->arena);
}

স্থির পূর্ণসংখ্যা
_get_interpolation_conversion(Parser *p, Token *debug, ফলাফল_Token_With_Metadata *conversion,
                              ফলাফল_Token_With_Metadata *format)
{
    যদি (conversion != ফাঁকা) {
        রাশিমালা_ty conversion_রাশিমালা = (রাশিমালা_ty) conversion->ফলাফল;
        প্রতিজ্ঞা(conversion_রাশিমালা->kind == Name_kind);
        Py_UCS4 first = PyUnicode_READ_CHAR(conversion_রাশিমালা->v.Name.id, 0);
        ফেরত Py_SAFE_DOWNCAST(first, Py_UCS4, পূর্ণসংখ্যা);
    }
    অন্যথা যদি (debug && !format) {
        /* If no conversion is specified, use !r for debug expressions */
        ফেরত (পূর্ণসংখ্যা)'r';
    }
    ফেরত -1;
}

স্থির PyObject *
strip_interpolation_debug_রাশিমালা(PyObject *exprstr)
{
    Py_ssize_t len = Py_Unicode_GET_দৈর্ঘ্য(exprstr);

    /* Discard whitespace and explicit line continuations after the debug "="
       but preserve whitespace before it. */
    যখন (len > 0) {
        পূর্ণসংখ্যা has_newline = 0;
        যখন (len > 0) {
            Py_UCS4 c = PyUnicode_READ_CHAR(exprstr, len - 1);
            যদি (!_PyUnicode_IsWhitespace(c)) {
                বিরতি;
            }
            যদি (c == '\r' || c == '\n') {
                has_newline = 1;
            }
            len--;
        }
        যদি (!has_newline || len == 0 ||
            PyUnicode_READ_CHAR(exprstr, len - 1) != '\\')
        {
            বিরতি;
        }
        len--;
    }

    /* Preserve unexpected metadata instead of dropping source text. */
    যদি (len == 0 || PyUnicode_READ_CHAR(exprstr, len - 1) != '=') {
        ফেরত Py_NewRef(exprstr);
    }

    ফেরত PyUnicode_Substring(exprstr, 0, len - 1);
}

রাশিমালা_ty _PyPegen_interpolation(Parser *p, রাশিমালা_ty expression, Token *debug, ফলাফল_Token_With_Metadata *conversion,
                                 ফলাফল_Token_With_Metadata *format, Token *closing_brace, পূর্ণসংখ্যা lineno, পূর্ণসংখ্যা col_offset,
                                 পূর্ণসংখ্যা end_lineno, পূর্ণসংখ্যা end_col_offset, PyArena *arena) {

    পূর্ণসংখ্যা conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    /* Find the non whitespace token after the "=" */
    পূর্ণসংখ্যা debug_end_সারি, debug_end_offset;
    PyObject *debug_metadata;
    constant exprstr;

    যদি (conversion) {
        debug_end_সারি = ((রাশিমালা_ty) conversion->ফলাফল)->lineno;
        debug_end_offset = ((রাশিমালা_ty) conversion->ফলাফল)->col_offset;
        debug_metadata = exprstr = conversion->metadata;
    }
    অন্যথা যদি (format) {
        debug_end_সারি = ((রাশিমালা_ty) format->ফলাফল)->lineno;
        debug_end_offset = ((রাশিমালা_ty) format->ফলাফল)->col_offset + 1;
        debug_metadata = exprstr = format->metadata;
    }
    অন্যথা {
        debug_end_সারি = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = exprstr = closing_brace->metadata;
    }

    প্রতিজ্ঞা(exprstr != ফাঁকা);
    PyObject *final_exprstr = debug
        ? strip_interpolation_debug_রাশিমালা(exprstr)
        : Py_NewRef(exprstr);
    যদি (!final_exprstr || _PyArena_AddPyObject(arena, final_exprstr) < 0) {
        Py_XDECREF(final_exprstr);
        ফেরত ফাঁকা;
    }

    রাশিমালা_ty interpolation = _PyAST_Interpolation(
        expression, final_exprstr, conversion_val, format ? (রাশিমালা_ty) format->ফলাফল : ফাঁকা,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    যদি (!interpolation || !debug) {
        ফেরত interpolation;
    }

    রাশিমালা_ty debug_text = _PyAST_Constant(debug_metadata, ফাঁকা, lineno, col_offset + 1, debug_end_সারি,
                                            debug_end_offset - 1, p->arena);
    যদি (!debug_text) {
        ফেরত ফাঁকা;
    }

    asdl_রাশিমালা_seq *মানসমূহ = Py_asdl_রাশিমালা_seq_new(2, arena);
    যদি (!মানসমূহ) {
        ফেরত ফাঁকা;
    }
    asdl_seq_SET(মানসমূহ, 0, debug_text);
    asdl_seq_SET(মানসমূহ, 1, interpolation);
    ফেরত _PyAST_JoinedStr(মানসমূহ, lineno, col_offset, debug_end_সারি, debug_end_offset, p->arena);
}

রাশিমালা_ty Py_Pegen_formatted_মান(Parser *p, রাশিমালা_ty expression, Token *debug, ফলাফল_Token_With_Metadata *conversion,
                                 ফলাফল_Token_With_Metadata *format, Token *closing_brace, পূর্ণসংখ্যা lineno, পূর্ণসংখ্যা col_offset,
                                 পূর্ণসংখ্যা end_lineno, পূর্ণসংখ্যা end_col_offset, PyArena *arena) {
    পূর্ণসংখ্যা conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    রাশিমালা_ty formatted_মান = Py_AST_Formatted_মান(
        expression, conversion_val, format ? (রাশিমালা_ty) format->ফলাফল : ফাঁকা,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    যদি (!formatted_মান || !debug) {
        ফেরত formatted_মান;
    }

    /* Find the non whitespace token after the "=" */
    পূর্ণসংখ্যা debug_end_সারি, debug_end_offset;
    PyObject *debug_metadata;

    যদি (conversion) {
        debug_end_সারি = ((রাশিমালা_ty) conversion->ফলাফল)->lineno;
        debug_end_offset = ((রাশিমালা_ty) conversion->ফলাফল)->col_offset;
        debug_metadata = conversion->metadata;
    }
    অন্যথা যদি (format) {
        debug_end_সারি = ((রাশিমালা_ty) format->ফলাফল)->lineno;
        debug_end_offset = ((রাশিমালা_ty) format->ফলাফল)->col_offset + 1;
        debug_metadata = format->metadata;
    }
    অন্যথা {
        debug_end_সারি = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = closing_brace->metadata;
    }
    রাশিমালা_ty debug_text = _PyAST_Constant(debug_metadata, ফাঁকা, lineno, col_offset + 1, debug_end_সারি,
                                            debug_end_offset - 1, p->arena);
    যদি (!debug_text) {
        ফেরত ফাঁকা;
    }

    asdl_রাশিমালা_seq *মানসমূহ = Py_asdl_রাশিমালা_seq_new(2, arena);
    যদি (!মানসমূহ) {
        ফেরত ফাঁকা;
    }
    asdl_seq_SET(মানসমূহ, 0, debug_text);
    asdl_seq_SET(মানসমূহ, 1, formatted_মান);
    ফেরত _PyAST_JoinedStr(মানসমূহ, lineno, col_offset, debug_end_সারি, debug_end_offset, p->arena);
}

স্থির রাশিমালা_ty
_build_concatenated_bytes(Parser *p, asdl_রাশিমালা_seq *strings, পূর্ণসংখ্যা lineno,
                        পূর্ণসংখ্যা col_offset, পূর্ণসংখ্যা end_lineno, পূর্ণসংখ্যা end_col_offset,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    প্রতিজ্ঞা(len > 0);

    /* Bytes literals never get a kind, but just for consistency
        since they are represented as Constant nodes, we'll mirror
        the same behavior as unicode strings for determining the
        kind. */
    PyObject *kind = asdl_seq_GET(strings, 0)->v.Constant.kind;

    Py_ssize_t যোগ = 0;
    জন্য (Py_ssize_t i = 0; i < len; i++) {
        রাশিমালা_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.মান;
        Py_ssize_t part = Py_Bytes_GET_আকার(bytes);
        যদি (part > PY_SSIZE_T_MAX - যোগ) {
            PyErr_NoMemory();
            ফেরত ফাঁকা;
        }
        যোগ += part;
    }

    PyBytesWriter *writer = PyBytesWriter_Create(যোগ);
    যদি (writer == ফাঁকা) {
        ফেরত ফাঁকা;
    }
    অক্ষর *out = Py_Bytes_Writer_Get_তথ্য(writer);

    জন্য (Py_ssize_t i = 0; i < len; i++) {
        রাশিমালা_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.মান;
        Py_ssize_t part = Py_Bytes_GET_আকার(bytes);
        যদি (part > 0) {
            memcpy(out, PyBytes_AS_STRING(bytes), part);
            out += part;
        }
    }

    PyObject *res = PyBytesWriter_Finish(writer);
    যদি (res == ফাঁকা) {
        ফেরত ফাঁকা;
    }
    যদি (_PyArena_AddPyObject(arena, res) < 0) {
        Py_DECREF(res);
        ফেরত ফাঁকা;
    }
    ফেরত _PyAST_Constant(res, kind, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

স্থির রাশিমালা_ty
_build_concatenated_unicode(Parser *p, asdl_রাশিমালা_seq *strings, পূর্ণসংখ্যা lineno,
                        পূর্ণসংখ্যা col_offset, পূর্ণসংখ্যা end_lineno, পূর্ণসংখ্যা end_col_offset,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    প্রতিজ্ঞা(len > 1);

    রাশিমালা_ty first = asdl_seq_GET(strings, 0);

    /* When a string is getting concatenated, the kind of the string
        is determined by the first string in the concatenation
        sequence.

        u"abc" "def" -> u"abcdef"
        "abc" u"abc" ->  "abcabc" */
    PyObject *kind = first->v.Constant.kind;

    PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
    যদি (writer == ফাঁকা) {
        ফেরত ফাঁকা;
    }

    জন্য (Py_ssize_t i = 0; i < len; i++) {
        রাশিমালা_ty current_elem = asdl_seq_GET(strings, i);
        প্রতিজ্ঞা(current_elem->kind == Constant_kind);

        যদি (PyUnicodeWriter_WriteStr(writer,
                                     current_elem->v.Constant.মান)) {
            PyUnicodeWriter_Discard(writer);
            ফেরত ফাঁকা;
        }
    }

    PyObject *final = PyUnicodeWriter_Finish(writer);
    যদি (final == ফাঁকা) {
        ফেরত ফাঁকা;
    }
    যদি (_PyArena_AddPyObject(p->arena, final) < 0) {
        Py_DECREF(final);
        ফেরত ফাঁকা;
    }
    ফেরত _PyAST_Constant(final, kind, lineno, col_offset,
                           end_lineno, end_col_offset, arena);
}

স্থির asdl_রাশিমালা_seq *
_build_concatenated_str(Parser *p, asdl_রাশিমালা_seq *strings,
                               পূর্ণসংখ্যা lineno, পূর্ণসংখ্যা col_offset, পূর্ণসংখ্যা end_lineno,
                               পূর্ণসংখ্যা end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    প্রতিজ্ঞা(len > 0);

    Py_ssize_t n_flattened_elements = 0;
    জন্য (Py_ssize_t i = 0; i < len; i++) {
        রাশিমালা_ty elem = asdl_seq_GET(strings, i);
        নির্বাচন(elem->kind) {
            ক্ষেত্র JoinedStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.JoinedStr.মানসমূহ);
                বিরতি;
            ক্ষেত্র TemplateStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.TemplateStr.মানসমূহ);
                বিরতি;
            পূর্বনির্ধারিত:
                n_flattened_elements++;
                বিরতি;
        }
    }


    asdl_রাশিমালা_seq* flattened = Py_asdl_রাশিমালা_seq_new(n_flattened_elements, p->arena);
    যদি (flattened == ফাঁকা) {
        ফেরত ফাঁকা;
    }

    /* build flattened list */
    Py_ssize_t current_pos = 0;
    জন্য (Py_ssize_t i = 0; i < len; i++) {
        রাশিমালা_ty elem = asdl_seq_GET(strings, i);
        নির্বাচন(elem->kind) {
            ক্ষেত্র JoinedStr_kind:
                জন্য (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.JoinedStr.মানসমূহ); j++) {
                    রাশিমালা_ty subvalue = asdl_seq_GET(elem->v.JoinedStr.মানসমূহ, j);
                    যদি (subvalue == ফাঁকা) {
                        ফেরত ফাঁকা;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                বিরতি;
            ক্ষেত্র TemplateStr_kind:
                জন্য (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.TemplateStr.মানসমূহ); j++) {
                    রাশিমালা_ty subvalue = asdl_seq_GET(elem->v.TemplateStr.মানসমূহ, j);
                    যদি (subvalue == ফাঁকা) {
                        ফেরত ফাঁকা;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                বিরতি;
            পূর্বনির্ধারিত:
                asdl_seq_SET(flattened, current_pos++, elem);
                বিরতি;
        }
    }

    /* calculate folded element count */
    Py_ssize_t n_elements = 0;
    পূর্ণসংখ্যা পূর্ব_is_constant = 0;
    জন্য (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        রাশিমালা_ty elem = asdl_seq_GET(flattened, i);

        /* The concatenation of a FormattedValue and an empty Constant should
           lead to the FormattedValue itself. Thus, we will not take any empty
           constants into account, just as in `_PyPegen_joined_str` */
        যদি (elem->kind == Constant_kind &&
            PyUnicode_CheckExact(elem->v.Constant.মান) &&
            Py_Unicode_GET_দৈর্ঘ্য(elem->v.Constant.মান) == 0)
            চালু;

        যদি (!পূর্ব_is_constant || elem->kind != Constant_kind) {
            n_elements++;
        }
        পূর্ব_is_constant = elem->kind == Constant_kind;
    }

    asdl_রাশিমালা_seq* মানসমূহ = Py_asdl_রাশিমালা_seq_new(n_elements, p->arena);
    যদি (মানসমূহ == ফাঁকা) {
        ফেরত ফাঁকা;
    }

    /* build folded list */
    current_pos = 0;
    জন্য (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        রাশিমালা_ty elem = asdl_seq_GET(flattened, i);

        /* if the current elem and the following are constants,
           fold them and all consequent constants */
        যদি (elem->kind == Constant_kind) {
            যদি (i + 1 < n_flattened_elements &&
                asdl_seq_GET(flattened, i + 1)->kind == Constant_kind) {
                রাশিমালা_ty first_elem = elem;

                /* When a string is getting concatenated, the kind of the string
                   is determined by the first string in the concatenation
                   sequence.

                   u"abc" "def" -> u"abcdef"
                   "abc" u"abc" ->  "abcabc" */
                PyObject *kind = elem->v.Constant.kind;

                PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
                যদি (writer == ফাঁকা) {
                    ফেরত ফাঁকা;
                }
                রাশিমালা_ty last_elem = elem;
                Py_ssize_t j;
                জন্য (j = i; j < n_flattened_elements; j++) {
                    রাশিমালা_ty current_elem = asdl_seq_GET(flattened, j);
                    যদি (current_elem->kind == Constant_kind) {
                        যদি (PyUnicodeWriter_WriteStr(writer,
                                                     current_elem->v.Constant.মান)) {
                            PyUnicodeWriter_Discard(writer);
                            ফেরত ফাঁকা;
                        }
                        last_elem = current_elem;
                    } অন্যথা {
                        বিরতি;
                    }
                }
                i = j - 1;

                PyObject *concat_str = PyUnicodeWriter_Finish(writer);
                যদি (concat_str == ফাঁকা) {
                    ফেরত ফাঁকা;
                }
                যদি (_PyArena_AddPyObject(p->arena, concat_str) < 0) {
                    Py_DECREF(concat_str);
                    ফেরত ফাঁকা;
                }
                elem = _PyAST_Constant(concat_str, kind, first_elem->lineno,
                                       first_elem->col_offset,
                                       last_elem->end_lineno,
                                       last_elem->end_col_offset, p->arena);
                যদি (elem == ফাঁকা) {
                    ফেরত ফাঁকা;
                }
            }

            /* Drop all empty contanst strings */
            যদি (PyUnicode_CheckExact(elem->v.Constant.মান) &&
                Py_Unicode_GET_দৈর্ঘ্য(elem->v.Constant.মান) == 0) {
                চালু;
            }
        }

        asdl_seq_SET(মানসমূহ, current_pos++, elem);
    }

    প্রতিজ্ঞা(current_pos == n_elements);
    ফেরত মানসমূহ;
}

স্থির রাশিমালা_ty
_build_concatenated_joined_str(Parser *p, asdl_রাশিমালা_seq *strings,
                               পূর্ণসংখ্যা lineno, পূর্ণসংখ্যা col_offset, পূর্ণসংখ্যা end_lineno,
                               পূর্ণসংখ্যা end_col_offset, PyArena *arena)
{
    asdl_রাশিমালা_seq *মানসমূহ = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    যদি (!মানসমূহ) {
        ফেরত ফাঁকা;
    }
    ফেরত _PyAST_JoinedStr(মানসমূহ, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

রাশিমালা_ty
_PyPegen_concatenate_tstrings(Parser *p, asdl_রাশিমালা_seq *strings,
                               পূর্ণসংখ্যা lineno, পূর্ণসংখ্যা col_offset, পূর্ণসংখ্যা end_lineno,
                               পূর্ণসংখ্যা end_col_offset, PyArena *arena)
{
    asdl_রাশিমালা_seq *মানসমূহ = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    যদি (!মানসমূহ) {
        ফেরত ফাঁকা;
    }
    ফেরত _PyAST_TemplateStr(মানসমূহ, lineno, col_offset, end_lineno,
        end_col_offset, arena);
}

রাশিমালা_ty
_PyPegen_concatenate_strings(Parser *p, asdl_রাশিমালা_seq *strings,
                             পূর্ণসংখ্যা lineno, পূর্ণসংখ্যা col_offset, পূর্ণসংখ্যা end_lineno,
                             পূর্ণসংখ্যা end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    প্রতিজ্ঞা(len > 0);

    পূর্ণসংখ্যা f_string_found = 0;
    পূর্ণসংখ্যা unicode_string_found = 0;
    পূর্ণসংখ্যা bytes_found = 0;

    Py_ssize_t i = 0;
    জন্য (i = 0; i < len; i++) {
        রাশিমালা_ty elem = asdl_seq_GET(strings, i);
        নির্বাচন(elem->kind) {
            ক্ষেত্র Constant_kind:
                যদি (PyBytes_CheckExact(elem->v.Constant.মান)) {
                    bytes_found = 1;
                } অন্যথা {
                    unicode_string_found = 1;
                }
                বিরতি;
            ক্ষেত্র JoinedStr_kind:
                f_string_found = 1;
                বিরতি;
            ক্ষেত্র TemplateStr_kind:
                // python.gram handles this; we should never get here
                প্রতিজ্ঞা(0);
                বিরতি;
            পূর্বনির্ধারিত:
                f_string_found = 1;
                বিরতি;
        }
    }

    // Cannot mix unicode and bytes
    যদি ((unicode_string_found || f_string_found) && bytes_found) {
        RAISE_SYNTAX_ERROR("cannot mix bytes and nonbytes literals");
        ফেরত ফাঁকা;
    }

    // If it's only bytes or only unicode string, do a simple concat
    যদি (!f_string_found) {
        যদি (len == 1) {
            ফেরত asdl_seq_GET(strings, 0);
        }
        অন্যথা যদি (bytes_found) {
            ফেরত _build_concatenated_bytes(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
        অন্যথা {
            ফেরত _build_concatenated_unicode(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
    }

    ফেরত _build_concatenated_joined_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
}

স্থির পূর্ণসংখ্যা
_warn_relative_import_of_lazy(Parser *p, asdl_seq *dots, রাশিমালা_ty module)
{
    // Warn about `from . lazy import x`: the whitespace between the dots and
    // the module name is insignificant, so this is parsed exactly like
    // `from .lazy import x` (an import of the relative module "lazy"), but it
    // is most likely a transposition of `lazy from . import x` (PEP 810).
    যদি (p->call_invalid_rules) {
        ফেরত 0;
    }

    // Only fire if there is whitespace between the last dot and the name,
    // i.e. not for the common `from .lazy import x` spelling.
    Token *last_dot = asdl_seq_GET_UNTYPED(dots, asdl_seq_LEN(dots) - 1);
    যদি (
        last_dot->end_lineno == module->lineno
        && last_dot->end_col_offset == module->col_offset
    ) {
        ফেরত 0;
    }

    পূর্ণসংখ্যা গুনতি = Py_Pegen_seq_গুনতি_dots(dots);
    অক্ষর *buf = PyMem_RawMalloc(গুনতি + 1);
    যদি (buf == ফাঁকা) {
        PyErr_NoMemory();
        ফেরত -1;
    }
    memset(buf, '.', গুনতি);
    buf[গুনতি] = '\0';

    PyObject *বার্তা = PyUnicode_FromFormat(
        "'from %s lazy import' is the same as 'from %slazy import'; "
        "did you mean 'lazy from %s import'?",
        buf, buf, buf);
    PyMem_RawFree(buf);
    যদি (বার্তা == ফাঁকা) {
        ফেরত -1;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    পূর্ণসংখ্যা res = _PyErr_EmitSyntaxWarning(বার্তা,
                                       info.filename,
                                       module->lineno,
                                       module->col_offset + 1,
                                       module->end_lineno,
                                       module->end_col_offset + 1,
                                       info.module);
    Py_DECREF(বার্তা);
    ফেরত res;
}

stmt_ty
_PyPegen_checked_from_import(Parser *p, asdl_seq *dots, রাশিমালা_ty module_name,
                             asdl_alias_seq *names, রাশিমালা_ty lazy_token, পূর্ণসংখ্যা lineno,
                             পূর্ণসংখ্যা col_offset, পূর্ণসংখ্যা end_lineno, পূর্ণসংখ্যা end_col_offset,
                             PyArena *arena)
{
    identifier module = module_name->v.Name.id;
    পূর্ণসংখ্যা level = Py_Pegen_seq_গুনতি_dots(dots);
    যদি (level == 0 && PyUnicode_CompareWithASCIIString(module, "__future__") == 0) {
        যদি (lazy_token) {
            RAISE_SYNTAX_ERROR_KNOWN_LOCATION(lazy_token,
                "lazy from __future__ import is not allowed");
            ফেরত ফাঁকা;
        }
        জন্য (Py_ssize_t i = 0; i < asdl_seq_LEN(names); i++) {
            alias_ty alias = asdl_seq_GET(names, i);
            যদি (PyUnicode_CompareWithASCIIString(alias->name, "barry_as_FLUFL") == 0) {
                p->flags |= PyPARSE_BARRY_AS_BDFL;
                p->tok->barry_as_bdfl = 1;
            }
        }
    }
    অন্যথা যদি (
        level > 0
        && lazy_token == ফাঁকা
        && PyUnicode_CompareWithASCIIString(module, "lazy") == 0
    ) {
        যদি (_warn_relative_import_of_lazy(p, dots, module_name) < 0) {
            ফেরত ফাঁকা;
        }
    }
    ফেরত _PyAST_ImportFrom(module, names, level, lazy_token ? 1 : 0, lineno,
                             col_offset, end_lineno, end_col_offset, arena);
}

asdl_stmt_seq*
_PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts) {
    যদি (!p->call_invalid_rules) {
        ফেরত stmts;
    }
    Py_ssize_t len = asdl_seq_LEN(stmts);
    যদি (len == 0) {
        ফেরত stmts;
    }
    stmt_ty last_stmt = asdl_seq_GET(stmts, len - 1);
    যদি (p->last_stmt_location.lineno > last_stmt->lineno) {
        ফেরত stmts;
    }
    p->last_stmt_location.lineno = last_stmt->lineno;
    p->last_stmt_location.col_offset = last_stmt->col_offset;
    p->last_stmt_location.end_lineno = last_stmt->end_lineno;
    p->last_stmt_location.end_col_offset = last_stmt->end_col_offset;
    ফেরত stmts;
}
